//-----------------------------------------------------Include
// Carregamento de módulos através da inclusão dos arquivos
	#include <fstream>
	#include <string.h>
	#include "ns3/csma-helper.h"
	//#include "ns3/evalvid-client-server-helper.h"
	#include "ns3/lte-helper.h"
	#include "ns3/epc-helper.h"
	#include "ns3/core-module.h"
	#include "ns3/network-module.h"
	#include "ns3/ipv4-global-routing-helper.h"
	#include "ns3/internet-module.h"
	#include "ns3/mobility-module.h"
	#include "ns3/lte-module.h"
	#include "ns3/applications-module.h"
	#include "ns3/point-to-point-helper.h"
	#include "ns3/config-store.h"
	#include <ns3/ofswitch13-module.h>
	#include "ns3/netanim-module.h"
	#include "my-controller.h"
	#include "ns3/flow-monitor-module.h"
	#include "ns3/flow-monitor.h"
	#include "ns3/flow-monitor-helper.h"
	#include "ns3/gnuplot.h"
	#include "ns3/string.h"
	#include "ns3/double.h"
	#include <ns3/boolean.h>
	#include <ns3/enum.h>
	#include <iomanip>
	#include <string>
	#include <fstream>
	#include <iostream>
	#include <vector>
	#include <stdio.h>
	#include <stdlib.h>
	//#include <ns3/my-epc-helper.h>

	using namespace ns3;

	/* Criação de dicionários
		mymap relaciona o IMSI (International mobile subscriber identity) do usuário e o seu IP (Internet Protocol)
		mymap2 relaciona o IMSI (International mobile subscriber identity) com a RRH_ID a qual está conectado
	*/
	std::map <uint64_t, Ipv4Address> mymap;
	std::map <uint64_t, uint64_t> mymap2;

//-----------------------------------------------------Funções
	/* Função para o obter o mymap (IMSI X IP)
		Foi adicionado o método RecvMymap no código do epcHelper
		RecvMymap retorna o dicionário com a relação (IMSI X IP) 
	*/
	void CallMap (Ptr<PointToPointEpcHelper>  epcHelper){
		mymap = epcHelper->RecvMymap();
		// for (std::map<uint64_t, Ipv4Address >::iterator it=mymap.begin(); it!=mymap.end(); ++it)
		// std::cout <<"imsi: "<< it->first << " ip address: " << it->second << '\n';
	}

	/* Função para alocar as RRHs às BBUs 
		Utiliza o MyCtrl gerado pelo OFSwitch13
		O método de alocação toma como argumento dois objetos do tipo map,
  		relacionando imsi a IP e imsi a cellid, respectivamente.
  		O objetivo desta função é relacionar UE, ENB e BBU,
  		através da construção de um map relacionando ip a bbu antes de começar o tráfego de dados.
	*/
	void SendToController(Ptr<MyController> MyCtrl){
		MyCtrl->Allocation(mymap,mymap2);
		//for (std::map<uint64_t, Ipv4Address>::iterator it=mymap.begin(); it!=mymap.end(); ++it)
		//std::cout <<"imsi: "<< it->first << " ip address: " << it->second << '\n';
	}

	/* Função que cria a relção IMSI X Cellid
		Após a conexão ser estabelecida podemos obter a relação
	*/
 	void NotifyConnectionEstablishedUe (std::string context,
																uint64_t imsi,
																uint16_t cellid,
																uint16_t rnti)
 	{
	 //std::cout << Simulator::Now ().GetSeconds () << " " << context
	 //          << " UE IMSI " << imsi
	 //          << ": connected to CellId " << cellid
	 //          << " with RNTI " << rnti
	 //          << std::endl;
		 //std::map <uint64_t, uint64_t> mymap2;
		 mymap2[imsi]=cellid;
		//  for (std::map<uint64_t, uint64_t>::iterator it=mymap2.begin(); it!=mymap2.end(); ++it)
		//  std::cout <<"imsi: "<< it->first << " connected to cellid: " << it->second << '\n';
 	}
//-----------------------------------------------------Configurações
 int main (int argc, char *argv[])
 {
	//LogComponentEnable ("EvalvidClient", LOG_LEVEL_INFO);
	//LogComponentEnable ("EvalvidServer", LOG_LEVEL_INFO);
	//uint16_t numberOfRrhs = 19;
	//AUTOCODE NUMBEROFRRHS INICIO
	  uint16_t numberOfRrhs = 12;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 384;
	//AUTOCODE NUMBEROFUSERS FIM
	//98
	//uint16_t numberOfNodes[19] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	/*
	Não-balanceado
	BBU 1 = 7,12= 65+65 = 130
	BBU 2 = 3,6 = 50+55 = 105
	BBU 3 = 1,2 = 70+45 = 115
	BBU 4 = 13,14,17,18 = 60+45+45+50 = 200
	BBU 5 = 4,5,8,9 = 45+60+60+65 = 230
	BBU 6 = 10,11,15,16,19 = 50+45+60+45+50 = 250
	Balanceado
	BBU 1 = 170
	BBU 2 = 180
	BBU 3 = 165
	BBU 4 = 160
	BBU 5 = 175
	BBU 6 = 180
	*/
	double simTime = 5.0;
	double distance = 5000.0;
	double interPacketInterval = 50;
	bool trace = true;
	ns3::SeedManager::SetSeed(1);

	//LogComponentEnable ("Ipv4StaticRouting", LOG_LEVEL_ALL);

	// Command line arguments
	CommandLine cmd;
	cmd.AddValue("simTime", "Total duration of the simulation [s])", simTime);
	cmd.AddValue("distance", "Distance between eNBs [m]", distance);
	cmd.AddValue("interPacketInterval", "Inter packet interval [ms])", interPacketInterval);
	cmd.Parse(argc, argv);

	Mac48Address::Allocate ();

	// Cria  o helper(Assistente) para o LTE
	Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

	/*Cria um assistente EPC(Evolved Packet Core) para criar uma rede EPC com links ponto a ponto
	  no Backhaul da rede */ 
	Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();

	// Informa para o assistente LTE qual é o assistente EPC
	lteHelper->SetEpcHelper (epcHelper);

	/* Configurações do EPC
		S1uLinkMtu: indica o MTU (Maximum Transmission Unit) para a interface S1-U em bytes. S1-U é a interface
		entre o EPC (especificamente o SGW-Serving Gateway) com as eNB (Evolved EnodeB). 
		S1uLinkDelay: indica o delay que será utillizado na interface S1-U
		S1uLinkDataRate: indica a taxa de dados para ser usada no link S1-U.
	*/
	epcHelper->SetAttribute ("S1uLinkMtu", UintegerValue(7000));
	epcHelper->SetAttribute ("S1uLinkDelay", TimeValue(Seconds(0.00015)));
	epcHelper->SetAttribute ("S1uLinkDataRate", DataRateValue (DataRate ("70Gb/s")));

	/* Configurações do LTE
		SetSchedulerType: indica o tipo de scheduler que será utillizado nas eNBs.
		SetHandoverAlgorithmType: indica o tipo de handover que será utilizado pelas eNBs.
		ServingCellThreshold: Se o RSRQ (Reference Signal Received Quality) da célula servidora for pior que esse limite, 
		 as células vizinhas serão consideradas para transferência.
		NeighbourCellOffset: OffSet (diferença de RSRQ) mínimo entre a célula servidora e a melhor célula vizinha para acionar o handover.
	*/
	lteHelper->SetSchedulerType ("ns3::RrFfMacScheduler");
	//lteHelper->SetHandoverAlgorithmType ("ns3::NoOpHandoverAlgorithm"); // disable automatic handover
	lteHelper->SetHandoverAlgorithmType ("ns3::A2A4RsrqHandoverAlgorithm");
	lteHelper->SetHandoverAlgorithmAttribute ("ServingCellThreshold",
                                          UintegerValue (30));
	lteHelper->SetHandoverAlgorithmAttribute ("NeighbourCellOffset",
                                          UintegerValue (1));


	//Configuração dos parâmetros das eNbs e dos UE
	ConfigStore inputConfig;
	inputConfig.ConfigureDefaults();
  	// UlBandwidth e DlBandwidth indicam a largura de banda em quantidade de PRBs (50PRBs = 10Mhz de largura de banda)
  	Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (50));
  	Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue (50));
  	// DlEarfcn e UlEarfcn são códigos que indica a frência que será utilizada. Para saber os valores correspondentes acesse: https://www.sqimway.com/lte_band.php
  	Config::SetDefault ("ns3::LteEnbNetDevice::DlEarfcn", UintegerValue (1575));
  	Config::SetDefault ("ns3::LteEnbNetDevice::UlEarfcn", UintegerValue (19575));
  	Config::SetDefault ("ns3::LteUeNetDevice::DlEarfcn", UintegerValue (1575));
  	// Potência de transmissão da eNb em dBm
  	Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (49));

	/* GlobalValue::Bind realiza uma busca em todas variáveis globais para encontrar a variável indicada ("ChecksumEnabled")
		Ao encontrar, configura o seu valor de acordo com o que foi passado (BooleanValue (true))
	*/
	GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));

	/* Configurando a periodicidade do SRS (Sounding Reference Signal)
		SRS é um sinal de referência que o UE (User Equipment) transmite no sentido de uplink para as eNBs.
		As eNBs utilizam esse sinal para estimar a qualidade do canal de uplink em uma largura de banda mais ampla. 
		SrsPeriodicity: indica o intervalo de tempo (ms) em que o UE envia o SRS para a eNB
	*/
	Config::SetDefault ("ns3::LteEnbRrc::SrsPeriodicity", UintegerValue (160));
	//Config::SetDefault ("ns3::OFSwitch13Device::PipelineCapacity", DataRateValue (DataRate ("100Gb/s")));

	// Defina o tipo de modelo de perda de caminho a ser usado para os canais DL(Dowlink) e UL(Uplink).
	lteHelper->SetAttribute("PathlossModel", StringValue("ns3::FabricioPropagationLossModel"));

	// parse again so you can override default values from the command line
	cmd.Parse(argc, argv);

	// Obtém o nó PGW (Packet Data Network Gateway) do EPC
	Ptr<Node> pgw = epcHelper->GetPgwNode ();
	//Mac48Address::Allocate ();

//-----------------------------------------------------Nós
	// Cria contêineres (conjunto) de nós
	NodeContainer remoteHostContainer;
	NodeContainer mainswitchC;
	NodeContainer topswitchC;

	//Indica a quantidade nós que o contêiner vai ter
	mainswitchC.Create (1);
	topswitchC.Create (1);
	
	//Obtém o ponteiro do primeiro nó no contêiner
	Ptr<Node> topswitch = topswitchC.Get(0);
	Ptr<Node> mainswitch = mainswitchC.Get(0);

	//cria o contêiner de nós que representarão as BBUs
	NodeContainer midswitch;
	midswitch.Create (6);

	/* Configuração do remoteHost
		Indica a quantidade de nós que devem ser criados no remoteHostContainer
		Obtém o ponteiro do nó criado no contêiner
		Cria um assistem de pilhas protocolos de internet (InternetStackHelper)
		 *InternetStackHelper é um assistente de topologia inter-rede
		Instala os as pilhas protocolos da Internet (TCP,UDP-User Datagram Protocol, IP, etc) em cada nó do contêiner remoteHostContainer
	*/remoteHostContainer.Create (1);
	Ptr<Node> remoteHost = remoteHostContainer.Get (0);
	InternetStackHelper internet;
	internet.Install (remoteHostContainer);
//-----------------------------------------------------Links

	/*Construção de um conjunto de objetos CsmaNetDevice
	  CSMA (Carrier Sense Multiple Access) foi desenvolvido para minimizar a probabilidade de colisão e, assim, aumentar o desempenho.
	*/
	//Cria um assistente CSMA
	CsmaHelper csmaHelper;
	//Configura a taxa de dados do canal
	csmaHelper.SetChannelAttribute ("DataRate", DataRateValue (DataRate ("1Gbps")));//600Gbps
	//Configura o delay do canal
	csmaHelper.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (15)));//10
	//Configura o MTU de nível MAC (Media Access Control)
	csmaHelper.SetDeviceAttribute ("Mtu", UintegerValue (9000));
	//Anexa uma fila ao CsmaNetDevice. Os parâmetros do SetQueue indicam 
	//o tipo de fila, o atributo a ser configurado na fila e o valor do atributo a ser configurado
	csmaHelper.SetQueue ("ns3::DropTailQueue","MaxPackets", UintegerValue (65536));

	// Cria contêineres que possuem um vetor de ponteiros ns3::NetDevice
	NetDeviceContainer hostDevices;
	NetDeviceContainer switchPorts[8];

	// Cria um contêiner de nós por meio da concatenação dos NodeContainers remoteHost e topswitch.
	NodeContainer pair (remoteHost, topswitch);
	/*Criação de um canal CSMA entre o remoteHost e topswitch  
		O método install cria um ns3::CsmaChannel com os atributos configurados por CsmaHelper::SetChannelAttribute.
		Retorna um contêiner contendo os dispositivos de rede adicionados.
	*/
	NetDeviceContainer link = csmaHelper.Install (pair);
	// Anexa o conteúdo do primeiro dispositivo (remoteHost) do NetDeviceContainer link ao final do contêiner hostDevices.
	hostDevices.Add (link.Get (0));
	// Anexa o conteúdo do segundo dispositivo (topswitch) do NetDeviceContainer link ao indice 0 do contêiner switchPorts.
	switchPorts[0].Add (link.Get (1));


	// Contêiner pair passa a ser a concatenação do topswitch com o primeiro dispositivo (Primeira BBU) do contêiner midswitch
	pair = NodeContainer(topswitch,midswitch.Get(0));
	// link passa a ser um canal CSMA entre o topswitch e o  primeiro dispositivo (Primeira BBU) do contêiner midswitch
	link = csmaHelper.Install (pair);
	// Anexa o conteúdo do primeiro dispositivo (topswitch) do NetDeviceContainer link ao indice 0 do contêiner switchPorts.
	switchPorts[0].Add (link.Get (0));
	// Anexa o conteúdo do segundo dispositivo (midswitch[0]- BBU 0) do NetDeviceContainer link ao indice 1 do contêiner switchPorts.
	switchPorts[1].Add (link.Get (1));


	// Contêiner pair passa a ser a concatenação do primeiro dispositivo (Primeira BBU) do contêiner midswitch com o mainswitch
	pair =NodeContainer(midswitch.Get(0), mainswitch);
	// link passa a ser um canal CSMA entre o primeiro dispositivo (Primeira BBU) do contêiner midswitch e o mainswitch 
	link = csmaHelper.Install (pair);
	// Anexa o conteúdo do primeiro dispositivo (midswitch[0]- BBU 0) do NetDeviceContainer link ao indice 1 do contêiner switchPorts.
	switchPorts[1].Add (link.Get (0));
	// Anexa o conteúdo do segundo dispositivo (mainswitch) do NetDeviceContainer link ao indice 3 do contêiner switchPorts.
	switchPorts[3].Add (link.Get (1));


	// Contêiner pair passa a ser a concatenação do primeiro dispositivo (Primeira BBU) do contêiner midswitch com o mainswitch
	pair = NodeContainer(topswitch,midswitch.Get(1));
	// link passa a ser um canal CSMA entre o topswitch e o segundo dispositivo (Segunda BBU) do contêiner midswitch 
	link = csmaHelper.Install (pair);
	// Anexa o conteúdo do primeiro dispositivo (topswitch) do NetDeviceContainer link ao indice 0 do contêiner switchPorts.
	switchPorts[0].Add (link.Get (0));
	// Anexa o conteúdo do segundo dispositivo (midswitch[1]- BBU 1) do NetDeviceContainer link ao indice 2 do contêiner switchPorts.
	switchPorts[2].Add (link.Get (1));


	pair =NodeContainer(midswitch.Get(1), mainswitch);
	link = csmaHelper.Install (pair);
	switchPorts[2].Add (link.Get (0));
	switchPorts[3].Add (link.Get (1));

	pair = NodeContainer(topswitch,midswitch.Get(2));
	link = csmaHelper.Install (pair);
	switchPorts[0].Add (link.Get (0));
	switchPorts[4].Add (link.Get (1));

	pair =NodeContainer(midswitch.Get(2), mainswitch);
	link = csmaHelper.Install (pair);
	switchPorts[4].Add (link.Get (0));
	switchPorts[3].Add (link.Get (1));

	pair = NodeContainer(topswitch,midswitch.Get(3));
	link = csmaHelper.Install (pair);
	switchPorts[0].Add (link.Get (0));
	switchPorts[5].Add (link.Get (1));

	pair =NodeContainer(midswitch.Get(3), mainswitch);
	link = csmaHelper.Install (pair);
	switchPorts[5].Add (link.Get (0));
	switchPorts[3].Add (link.Get (1));

	pair = NodeContainer(topswitch,midswitch.Get(4));
	link = csmaHelper.Install (pair);
	switchPorts[0].Add (link.Get (0));
	switchPorts[6].Add (link.Get (1));

	pair =NodeContainer(midswitch.Get(4), mainswitch);
	link = csmaHelper.Install (pair);
	switchPorts[6].Add (link.Get (0));
	switchPorts[3].Add (link.Get (1));

	pair = NodeContainer(topswitch,midswitch.Get(5));
	link = csmaHelper.Install (pair);
	switchPorts[0].Add (link.Get (0));
	switchPorts[7].Add (link.Get (1));

	pair =NodeContainer(midswitch.Get(5), mainswitch);
	link = csmaHelper.Install (pair);
	switchPorts[7].Add (link.Get (0));
	switchPorts[3].Add (link.Get (1));

	pair = NodeContainer(pgw,mainswitch);
	link = csmaHelper.Install (pair);
	hostDevices.Add (link.Get (0));
	switchPorts[3].Add (link.Get (1));

	/* Ao final teremos a seguinte configuração

														remoteHost
															|
														Topswitch

							|				|				|				|				|				|
						Midswitch[0]	Midswitch[1]	Midswitch[2]	Midswitch[3]	Midswitch[4]	Midswitch[5]
							|				|				|				|				|				|
															
														Mainswithc
															|
														   PGW


		Cada uma da BBUs, representadas pelo Midswitch, são conectadas ao topswitch e ao mainswitch
		O topswitch por sua vez, se conecta com o remoteHost onde se encontra a parte das aplicações
		E o Mainswitch está conectado ao PGW do núcle da rede LTE 
	*/


//-----------------------------------------------------Switchs
	// Create the controller node
	Ptr<Node> controllerNode = CreateObject<Node> ();
	Ptr<Node> controllerNode2 = CreateObject<Node> ();

	//Ptr<OFSwitch13InternalHelper> of13Helper = CreateObject<OFSwitch13InternalHelper> ();
	//Ptr<OFSwitch13LearningController> learnCtrl = CreateObject<OFSwitch13LearningController> ();
	
	// Contém um vetor de ponteiros ns3::OFSwitch13Device
	OFSwitch13DeviceContainer ofSwitchDevices;
	//of13Helper->InstallController (controllerNode, learnCtrl);

	/* Configura o domínio de rede OpenFlow
		OFSwitch13InternalHelper: Este auxiliar estende a classe base e pode ser instanciado para criar e configurar um domínio de rede OpenFlow 1.3 
		 composto por um ou mais switches OpenFlow conectados a um ou vários controladores OpenFlow simulados internos.
	*/
	Ptr<OFSwitch13InternalHelper> ofMyCtrlHelper = CreateObject<OFSwitch13InternalHelper> ();

	// Cria o controlador MyCrrl baseado na classe MyController do OFSwich13
	Ptr<MyController> MyCtrl = CreateObject<MyController> ();
	//Instala o aplicativo do controlador MyCtrl no nó controllerNode2
	ofMyCtrlHelper->InstallController (controllerNode2, MyCtrl);

	/* Criando Dispositivos OpenFlow
		InstallSwitch: cria um dispositivo OpenFlow e o agrega ao nó do switch.
			Primeiro Parâmetro: nó do switch onde instalar o dispositivo OpenFlow.
			Segundo Parâmetro: dispositivos a serem adicionados como portas de switch físico.
	*/
	ofSwitchDevices.Add (ofMyCtrlHelper->InstallSwitch (mainswitch, switchPorts[3]));
	ofSwitchDevices.Add (ofMyCtrlHelper->InstallSwitch (topswitch, switchPorts[0]));
	ofSwitchDevices.Add (ofMyCtrlHelper->InstallSwitch (midswitch.Get(0), switchPorts[1]));
	ofSwitchDevices.Add (ofMyCtrlHelper->InstallSwitch (midswitch.Get(1), switchPorts[2]));
	ofSwitchDevices.Add (ofMyCtrlHelper->InstallSwitch (midswitch.Get(2), switchPorts[4]));
	ofSwitchDevices.Add (ofMyCtrlHelper->InstallSwitch (midswitch.Get(3), switchPorts[5]));
	ofSwitchDevices.Add (ofMyCtrlHelper->InstallSwitch (midswitch.Get(4), switchPorts[6]));
	ofSwitchDevices.Add (ofMyCtrlHelper->InstallSwitch (midswitch.Get(5), switchPorts[7]));

	// CreateOpenFlowChannels: Este método virtual deve interconectar todos os switches a todos os controladores 
	//	instalados por este assistente e iniciar as conexões individuais do canal OpenFlow
	ofMyCtrlHelper->CreateOpenFlowChannels ();
	//of13Helper->CreateOpenFlowChannels ();

//-----------------------------------------------------Endereços IP

	// Uma classe assistente para facilitar a atribuição simples de endereços IPv4
	Ipv4AddressHelper ipv4h;
	// Defina o número de rede base, máscara de rede e endereço base.
	ipv4h.SetBase ("1.0.0.0", "255.255.255.0","0.0.0.2");
	/* 
		Assign: Atribui endereços IP aos dispositivos de rede especificados no contêiner hostDevices com base no 
		prefixo de rede e endereços atuais.
		*O hostDevices possui o remoteHost e o PGW. Ou seja, esse dois vão receber endereços do tipo 1.0.0.2 e 1.0.0.3
	*/
	Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (hostDevices);
	// Obtém o endereço IP do remoteHost
	Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (0);
	// Obtém o endereço IP do PGW
	Ipv4Address pgwAddr = internetIpIfaces.GetAddress (1);

	//Classe assistente que adiciona objetos ns3::Ipv4StaticRouting.
	Ipv4StaticRoutingHelper ipv4RoutingHelper;

	/* GetStaticRouting:
		Tente encontrar o protocolo de roteamento estático como o protocolo de roteamento principal ou 
		 na lista de protocolos de roteamento associados ao IPv4 fornecido (protocolos do remoteHost).
		Retorna um ponteiro do tipo Ipv4StaticRouting ou 0 se não for encontrado
	*/
	Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
	
	/*  AddNetworkRouteTo:
			Adiciona uma rota de rede à tabela de roteamento estático.
			Parâmetros:
				network: A rede Ipv4Address para esta rota.
				networkMask: O Ipv4Mask da rede.
				nextHop: O próximo salto na rota para a rede de destino.
				interface: O índice de interface de rede usado para enviar pacotes para o destino.
				métrica: Métrica de rota no caso de várias rotas para o mesmo destino
	*/
	remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"),pgwAddr, 1,1);
	//remoteHostStaticRouting->SetDefaultRoute (internetIpIfaces.GetAddress(1), 1,1);


//-----------------------------------------------------Macro BS (Base Station)
	// Para simular uma macro é necessário criar quatro eNBs sendo que cada eNB terá uma antena. Cada antena terá uma direção diferente

	Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (100));
  Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue (100));
	
  // Cria um contêiner para os nós da Macro
	NodeContainer enbMacroNodes;
	// Indica a quantidade de nós do contêiner
	enbMacroNodes.Create(4);


	/* ListPositionAllocator:
		Aloca posições de uma lista determinística especificada pelo usuário.
	*/
	Ptr<ListPositionAllocator> positionAllocMacro = CreateObject<ListPositionAllocator> ();
	
	/* Mobilidade da Macro
		Classe assistente usada para atribuir posições e modelos de mobilidade aos nós.
		SetMobilityModel: define o modelo de mobilidade, nesse caso é constante (ns3::ConstantPositionMobilityModel)
	*/
	MobilityHelper mob;
	mob.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	/* Posição dos nós da macro
		positionAllocMacro->Add: Adiciona uma posição (indicada pelo Vector) à lista de posições (positionAllocMacro).
		A macro possui 4 nós que devem estar na mesma posição (250,  250, 0).
		A macro possui 4 nós porque cada nó terá uma antena que apontará para uma direção para poder cobrir um espaço de 360°
	*/
	positionAllocMacro->Add (Vector (  500,  500, 0));
	positionAllocMacro->Add (Vector (  500,  500, 0));
	positionAllocMacro->Add (Vector (  500,  500, 0));
	positionAllocMacro->Add (Vector (  500,  500, 0));

	// Indica as posições para o helper de mobilidade
	mob.SetPositionAllocator(positionAllocMacro);
	// Instala as configurações de mobilidade, feita pelo assistente mob, nos nós da Macro 
	mob.Install(NodeContainer(enbMacroNodes));

	// Cria um contêiner para os dispositivos de rede da macro
	NetDeviceContainer enbLteDevsMacro;
    
	/* Configurando a direção das antenas da macro
		Cada antena terá uma direção diferente em graus (0°, 90°, 180°, 270° e 360°)
	*/
	// Define o tipo da antena 1
	lteHelper->SetEnbAntennaModelType ("ns3::CosineAntennaModel");
	// Define a orientação (°) da antena 1
	lteHelper->SetEnbAntennaModelAttribute ("Orientation", DoubleValue (0));
	// Beamwidth: determina a intensidade do sinal esperada dada a direção e a distância de radiação de uma antena
	lteHelper->SetEnbAntennaModelAttribute ("Beamwidth", DoubleValue (60));
	// Maxgain: O ganho (dB) na mira da antena (a direção do ganho máximo)
	lteHelper->SetEnbAntennaModelAttribute ("MaxGain", DoubleValue (0.0));
	// Instala um dispositivo de rede do tipo LteEnbNetDevice em um nó do contêiner enbMacroNodes
    NetDeviceContainer device1 = lteHelper->InstallEnbDevice (enbMacroNodes.Get(0));
	// Adiciona o dispositivo no contêiner de dispositivos da macro
    enbLteDevsMacro.Add (device1);

	lteHelper->SetEnbAntennaModelType ("ns3::CosineAntennaModel");
	lteHelper->SetEnbAntennaModelAttribute ("Orientation", DoubleValue (90));
	lteHelper->SetEnbAntennaModelAttribute ("Beamwidth", DoubleValue (60));
	lteHelper->SetEnbAntennaModelAttribute ("MaxGain", DoubleValue (0.0));  
    NetDeviceContainer device2 = lteHelper->InstallEnbDevice (enbMacroNodes.Get(1));
    enbLteDevsMacro.Add (device2);

	lteHelper->SetEnbAntennaModelType ("ns3::CosineAntennaModel");
	lteHelper->SetEnbAntennaModelAttribute ("Orientation", DoubleValue (180));
	lteHelper->SetEnbAntennaModelAttribute ("Beamwidth", DoubleValue (60));
	lteHelper->SetEnbAntennaModelAttribute ("MaxGain", DoubleValue (0.0));  
    NetDeviceContainer device3 = lteHelper->InstallEnbDevice (enbMacroNodes.Get(2));
    enbLteDevsMacro.Add (device3);

	lteHelper->SetEnbAntennaModelType ("ns3::CosineAntennaModel");
	lteHelper->SetEnbAntennaModelAttribute ("Orientation", DoubleValue (270));
	lteHelper->SetEnbAntennaModelAttribute ("Beamwidth", DoubleValue (60));
	lteHelper->SetEnbAntennaModelAttribute ("MaxGain", DoubleValue (0.0));  
    NetDeviceContainer device4 = lteHelper->InstallEnbDevice (enbMacroNodes.Get(3));
    enbLteDevsMacro.Add (device4);

//-----------------------------------------------------Mobilidade

	// Criando contêiner para as eNBs
	NodeContainer enbNodes;
	// Indica a quantidade de nós que devem ser criados
	enbNodes.Create(numberOfRrhs);
	
	// Cria a lista para armazenar posições das eNBs
	Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
	// for (uint16_t i = 0; i < numberOfRrhs; i++)
	// 	{
	// 		positionAlloc->Add (Vector(50,distance * i , 0));
	// 	}
	// Cria um assistente de mobilidade
	MobilityHelper mobility;
	// Configura o modelo de mobilidade (constante)
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

	// mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
    //                              "MinX", DoubleValue (0.0),
    //                              "MinY", DoubleValue (0.0),
    //                              "DeltaX", DoubleValue (distance/4),
    //                              "DeltaY", DoubleValue (distance/4),
    //                              "GridWidth", UintegerValue (4),
    //                              "LayoutType", StringValue ("RowFirst"));
	
	// Indica as posições das eNBs
	//AUTOCODE SMALLS INICIO
    positionAlloc->Add (Vector (356.6666666666667,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (270.0,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,10.0, 0.0));
    positionAlloc->Add (Vector (140.0,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,270.0, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (10.0,140.0, 0.0));
	//AUTOCODE SMALLS FIM
	// Passa a lista de posições para o assistente de mobilidade
	mobility.SetPositionAllocator(positionAlloc);
	// Instala as configurações de mobilidade nos nós das eNBs (enbNodes)
	mobility.Install(enbNodes);

	// Instala dispositivos LTE nos nós enbNodes
	NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);

	// // Cria a lista para armazenar posições do remoteHost e dos switchs
	Ptr<ListPositionAllocator> positionAlloc2 = CreateObject<ListPositionAllocator> ();
	// Indica as posições
	positionAlloc2->Add (Vector (  0,  10, 0));  // Server
	positionAlloc2->Add (Vector (  10, 10, 0));  // top
	positionAlloc2->Add (Vector ( 25, 10, 0));  // mid1
	positionAlloc2->Add (Vector ( 25, 20, 0));  // mid2
	positionAlloc2->Add (Vector ( 25, 30, 0));  // mid3
	positionAlloc2->Add (Vector ( 25, 40, 0));  // mid4
	positionAlloc2->Add (Vector ( 25, 50, 0));  // mid5
	positionAlloc2->Add (Vector ( 25, 60, 0));  // mid6
	positionAlloc2->Add (Vector (40, 10, 0));  // main

	// Cria um assistente de mobilidade
	MobilityHelper mobility2;
	// Configura o modelo de mobilidade (constante)
	mobility2.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	// Passa a lista de posições para o assistente de mobilidade
	mobility2.SetPositionAllocator(positionAlloc2);
	// Instala a configuração de mobilidade nos respectivos dispositivos
	mobility2.Install(NodeContainer(remoteHostContainer,topswitch, midswitch, mainswitch));
	
	// Cria uma lista de posições para o ctrl1, ctrl2 e o PGW
	Ptr<ListPositionAllocator> positionAlloc3 = CreateObject<ListPositionAllocator> ();
	// Indica as posições
	positionAlloc3->Add (Vector (  10,  20, 0));  // ctrl1
	positionAlloc3->Add (Vector (  40, 20, 0));  // ctrl2
	positionAlloc3->Add (Vector ( 50, 10, 0));  // pgw
	//Cria um assistente de mobilidade
	MobilityHelper mobility3;
	// Define o tipo modelo de mobilidade
	mobility3.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	// Passa as posições para o assistente de mobilidade
	mobility3.SetPositionAllocator(positionAlloc3);
	//Instala a configuração de mobilidade nos dispositivos indicados
	mobility3.Install(NodeContainer( controllerNode,controllerNode2, pgw));


	//--------------------------------------------Nós UE
	//Cria o contêiner de nós para os UE
	NodeContainer ueNodes;
	// Indica a quantidade de usuários que devem ser criados
	ueNodes.Create(numberOfNodes);

	// Cria o assistente de mobilidade
	MobilityHelper uesMobility;
	//double yvalue=0;

	//yvalue= i*distance;
	// uesMobility[i].SetPositionAllocator("ns3::RandomDiscPositionAllocator",
	//                                 "X", StringValue ("50.0"),
	//                                 //"Y", StringValue (std::to_string(yvalue)),
	//                                 "Y", DoubleValue (105.0),
	//                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
	

	// uesMobility.SetPositionAllocator("ns3::RandomBoxPositionAllocator",
	//                                 "X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=5000.0]"),
	//                                 //"Y", StringValue (std::to_string(yvalue)),
	//                                 "Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=5000.0]"),
	//                                 "Z", StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));  
	// uesMobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
	//                                "Mode", StringValue ("Time"),
	//                                "Time", StringValue ("2s"),
	//                                "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
	//                                "Bounds", RectangleValue (Rectangle (0, 5000, 0, 5000)));
	
	// Cria uma lista para armazenar a posição dos usuários
	Ptr<ListPositionAllocator> positionAlloc4 = CreateObject<ListPositionAllocator> ();
	// Passa as posições dos usuários
	//AUTOCODE USERS INICIO
    positionAlloc4->Add (Vector(33.49330646016453,206.38786481659994, 0.0));
    positionAlloc4->Add (Vector(342.71048994735935,75.06160462199122, 0.0));
    positionAlloc4->Add (Vector(342.7017077388325,191.0380094440022, 0.0));
    positionAlloc4->Add (Vector(220.30582420914908,17.585065412630208, 0.0));
    positionAlloc4->Add (Vector(139.4709051170451,270.86672717299626, 0.0));
    positionAlloc4->Add (Vector(90.15717201231084,47.1823409049756, 0.0));
    positionAlloc4->Add (Vector(33.38421859475598,262.3561268064668, 0.0));
    positionAlloc4->Add (Vector(398.68026854143255,329.0731924599625, 0.0));
    positionAlloc4->Add (Vector(5.540829798607527,178.19484346999252, 0.0));
    positionAlloc4->Add (Vector(85.64068957418054,204.31496732516302, 0.0));
    positionAlloc4->Add (Vector(210.18822830043703,76.38692357119706, 0.0));
    positionAlloc4->Add (Vector(165.3813103023113,121.70206617600053, 0.0));
    positionAlloc4->Add (Vector(182.16407814451915,239.762435453782, 0.0));
    positionAlloc4->Add (Vector(300.50865563178684,355.9365451736904, 0.0));
    positionAlloc4->Add (Vector(140.8457297039091,389.451785592424, 0.0));
    positionAlloc4->Add (Vector(230.88675763825378,223.82440937904607, 0.0));
    positionAlloc4->Add (Vector(167.37875469417048,118.1005880574765, 0.0));
    positionAlloc4->Add (Vector(398.4485394234021,60.682167047786706, 0.0));
    positionAlloc4->Add (Vector(256.25886282884125,19.04671336425561, 0.0));
    positionAlloc4->Add (Vector(256.47462312607075,222.7823778717324, 0.0));
    positionAlloc4->Add (Vector(231.23199474009334,349.3650911155107, 0.0));
    positionAlloc4->Add (Vector(159.4280954017713,247.5940398283075, 0.0));
    positionAlloc4->Add (Vector(175.8273793859773,16.26711595566266, 0.0));
    positionAlloc4->Add (Vector(132.43097223642164,144.80926615021605, 0.0));
    positionAlloc4->Add (Vector(167.1044970041788,388.78018484381323, 0.0));
    positionAlloc4->Add (Vector(265.26053102520024,102.57165626458016, 0.0));
    positionAlloc4->Add (Vector(384.31113471354263,82.20012843021007, 0.0));
    positionAlloc4->Add (Vector(231.58049930436286,241.13774632765305, 0.0));
    positionAlloc4->Add (Vector(49.703697450180414,161.8330606088751, 0.0));
    positionAlloc4->Add (Vector(277.6362253481819,236.30252685141107, 0.0));
    positionAlloc4->Add (Vector(297.5513339954409,360.4376335166054, 0.0));
    positionAlloc4->Add (Vector(288.72905256137096,238.12284181093574, 0.0));
    positionAlloc4->Add (Vector(19.558940468984567,349.75048500816246, 0.0));
    positionAlloc4->Add (Vector(2.7181781364669533,148.64740378156492, 0.0));
    positionAlloc4->Add (Vector(117.72247987616771,114.19783621798109, 0.0));
    positionAlloc4->Add (Vector(241.4345198042137,93.76317181353934, 0.0));
    positionAlloc4->Add (Vector(145.56013795511754,122.03915769904334, 0.0));
    positionAlloc4->Add (Vector(229.95583709003236,135.62103078069617, 0.0));
    positionAlloc4->Add (Vector(356.9909827429369,27.213288922834966, 0.0));
    positionAlloc4->Add (Vector(299.128232136976,132.63058293858947, 0.0));
    positionAlloc4->Add (Vector(218.87950602895958,180.1430192609244, 0.0));
    positionAlloc4->Add (Vector(138.35736338867878,226.38650269403598, 0.0));
    positionAlloc4->Add (Vector(38.65442393559437,366.3936124465356, 0.0));
    positionAlloc4->Add (Vector(122.79455751199974,60.026273954361955, 0.0));
    positionAlloc4->Add (Vector(150.16252909942077,91.36362072465674, 0.0));
    positionAlloc4->Add (Vector(52.4600748752225,57.47438919952867, 0.0));
    positionAlloc4->Add (Vector(173.25062876513505,293.7635384459153, 0.0));
    positionAlloc4->Add (Vector(285.91185397824205,130.7829671718536, 0.0));
    positionAlloc4->Add (Vector(106.87328443399701,249.52620257136596, 0.0));
    positionAlloc4->Add (Vector(298.3603715178696,387.6031288968141, 0.0));
    positionAlloc4->Add (Vector(334.12832794056186,3.7354748971222484, 0.0));
    positionAlloc4->Add (Vector(215.6322145389766,89.44102102637484, 0.0));
    positionAlloc4->Add (Vector(38.751292105921294,80.29464876883124, 0.0));
    positionAlloc4->Add (Vector(101.23448684341274,18.499369275820943, 0.0));
    positionAlloc4->Add (Vector(172.8734854035524,377.0044573532859, 0.0));
    positionAlloc4->Add (Vector(345.89114735053016,312.6720215771494, 0.0));
    positionAlloc4->Add (Vector(389.1986312908691,281.84762648441546, 0.0));
    positionAlloc4->Add (Vector(317.410377647077,41.57605284404609, 0.0));
    positionAlloc4->Add (Vector(152.41955874256584,182.2987448249942, 0.0));
    positionAlloc4->Add (Vector(148.9949627747778,223.36716844941202, 0.0));
    positionAlloc4->Add (Vector(296.8939152700507,106.44046754006501, 0.0));
    positionAlloc4->Add (Vector(373.7758394717926,357.91049534053656, 0.0));
    positionAlloc4->Add (Vector(128.66029040985384,284.2661586664789, 0.0));
    positionAlloc4->Add (Vector(325.2590653145714,270.93674396822223, 0.0));
    positionAlloc4->Add (Vector(48.87872516908196,201.2260194204399, 0.0));
    positionAlloc4->Add (Vector(190.69362127873913,288.56110117370594, 0.0));
    positionAlloc4->Add (Vector(360.78307106728926,164.6135108738507, 0.0));
    positionAlloc4->Add (Vector(195.47122142477065,90.15418323916245, 0.0));
    positionAlloc4->Add (Vector(241.14970804458252,130.81491354649836, 0.0));
    positionAlloc4->Add (Vector(299.0700774294983,62.92911821205269, 0.0));
    positionAlloc4->Add (Vector(345.364737313689,255.5276752672426, 0.0));
    positionAlloc4->Add (Vector(122.77994694820605,366.2336089799941, 0.0));
    positionAlloc4->Add (Vector(115.15354835176468,338.0986132986225, 0.0));
    positionAlloc4->Add (Vector(369.44474588087934,166.67379442426474, 0.0));
    positionAlloc4->Add (Vector(108.58351413315255,190.2894509241478, 0.0));
    positionAlloc4->Add (Vector(2.622750154373721,126.40938532183358, 0.0));
    positionAlloc4->Add (Vector(165.8615707203484,23.45667087438832, 0.0));
    positionAlloc4->Add (Vector(318.68063199365724,226.9853350671625, 0.0));
    positionAlloc4->Add (Vector(125.70239611815524,53.1700220548843, 0.0));
    positionAlloc4->Add (Vector(318.09834567187585,348.89708373686733, 0.0));
    positionAlloc4->Add (Vector(90.64307862162492,316.95209010821094, 0.0));
    positionAlloc4->Add (Vector(303.14777769786843,396.879689475975, 0.0));
    positionAlloc4->Add (Vector(14.781209109334403,186.1642034933345, 0.0));
    positionAlloc4->Add (Vector(9.053885653457172,17.719635520849497, 0.0));
    positionAlloc4->Add (Vector(22.650542717603138,272.3184137031476, 0.0));
    positionAlloc4->Add (Vector(285.11705181358826,41.560443302114926, 0.0));
    positionAlloc4->Add (Vector(347.20483943903224,78.54881600940038, 0.0));
    positionAlloc4->Add (Vector(277.2377378005518,267.4202896091562, 0.0));
    positionAlloc4->Add (Vector(354.77536181935056,15.042176150308917, 0.0));
    positionAlloc4->Add (Vector(116.04829507692851,257.59387850918637, 0.0));
    positionAlloc4->Add (Vector(308.30733097030543,177.73007094911145, 0.0));
    positionAlloc4->Add (Vector(31.418095797048995,62.04780522001458, 0.0));
    positionAlloc4->Add (Vector(91.55674710753247,288.36668284484415, 0.0));
    positionAlloc4->Add (Vector(103.43759780883079,21.862462167728935, 0.0));
    positionAlloc4->Add (Vector(351.6683051205767,39.13568121308351, 0.0));
    positionAlloc4->Add (Vector(239.41459678525513,336.5622134799032, 0.0));
    positionAlloc4->Add (Vector(215.51067999316194,283.5044550032266, 0.0));
    positionAlloc4->Add (Vector(211.32027707890177,267.42740976629375, 0.0));
    positionAlloc4->Add (Vector(255.94268613773875,84.70851631553403, 0.0));
    positionAlloc4->Add (Vector(193.7111607987256,180.42751770532126, 0.0));
    positionAlloc4->Add (Vector(151.76077759875403,338.5423503190684, 0.0));
    positionAlloc4->Add (Vector(368.2777812195066,152.0023213809917, 0.0));
    positionAlloc4->Add (Vector(226.6365632793834,13.642566959331948, 0.0));
    positionAlloc4->Add (Vector(143.53841346141633,261.50058900412125, 0.0));
    positionAlloc4->Add (Vector(231.89481881893735,293.40680730123734, 0.0));
    positionAlloc4->Add (Vector(130.36348818335023,29.376066547199464, 0.0));
    positionAlloc4->Add (Vector(388.7992559937537,283.7495163282976, 0.0));
    positionAlloc4->Add (Vector(27.10833298324551,272.2809301866324, 0.0));
    positionAlloc4->Add (Vector(37.74901437368574,359.7474125571504, 0.0));
    positionAlloc4->Add (Vector(248.4757547483499,198.0029583371203, 0.0));
    positionAlloc4->Add (Vector(354.6591258183418,98.37773650942117, 0.0));
    positionAlloc4->Add (Vector(36.6379208686181,109.47092152198658, 0.0));
    positionAlloc4->Add (Vector(205.25960301648632,171.76881187226084, 0.0));
    positionAlloc4->Add (Vector(320.05772476699985,364.52334170945846, 0.0));
    positionAlloc4->Add (Vector(301.6672743777787,128.71448022095686, 0.0));
    positionAlloc4->Add (Vector(161.55345502046958,110.56168794414813, 0.0));
    positionAlloc4->Add (Vector(34.96292646469761,148.7685826055611, 0.0));
    positionAlloc4->Add (Vector(338.92168216985544,52.14300577662003, 0.0));
    positionAlloc4->Add (Vector(148.43096264874274,292.47632968014585, 0.0));
    positionAlloc4->Add (Vector(275.0022100644092,280.80675013654263, 0.0));
    positionAlloc4->Add (Vector(4.2056033138120785,59.65079950690484, 0.0));
    positionAlloc4->Add (Vector(8.797003994009911,67.42627290202438, 0.0));
    positionAlloc4->Add (Vector(0.29636518421956026,294.0862216944936, 0.0));
    positionAlloc4->Add (Vector(390.2167911549985,305.4880127133579, 0.0));
    positionAlloc4->Add (Vector(120.35577325211352,361.7381031087405, 0.0));
    positionAlloc4->Add (Vector(36.58717678624597,27.234309695889447, 0.0));
    positionAlloc4->Add (Vector(80.03674522735706,186.06061266198344, 0.0));
    positionAlloc4->Add (Vector(24.300294531877633,139.4052837555262, 0.0));
    positionAlloc4->Add (Vector(315.96265674657764,361.0538682745235, 0.0));
    positionAlloc4->Add (Vector(227.73295499349086,234.06266918506157, 0.0));
    positionAlloc4->Add (Vector(16.82425057969308,393.95410562332063, 0.0));
    positionAlloc4->Add (Vector(293.07742979167296,225.21628942935803, 0.0));
    positionAlloc4->Add (Vector(71.90106137202159,15.423624505117983, 0.0));
    positionAlloc4->Add (Vector(179.5568082983183,335.13144455192696, 0.0));
    positionAlloc4->Add (Vector(207.80254143458282,26.253935303401697, 0.0));
    positionAlloc4->Add (Vector(302.58815420964106,347.4355995721926, 0.0));
    positionAlloc4->Add (Vector(300.2761486606687,66.34681179181116, 0.0));
    positionAlloc4->Add (Vector(248.82765010918675,92.43845101473025, 0.0));
    positionAlloc4->Add (Vector(305.81923573385865,117.2996742827578, 0.0));
    positionAlloc4->Add (Vector(373.1153401776543,386.5988802720878, 0.0));
    positionAlloc4->Add (Vector(119.4984769533379,276.00601170597, 0.0));
    positionAlloc4->Add (Vector(40.86814373375596,9.93081167516321, 0.0));
    positionAlloc4->Add (Vector(300.09758031164654,219.54376017231456, 0.0));
    positionAlloc4->Add (Vector(253.0141025376571,74.82430207018504, 0.0));
    positionAlloc4->Add (Vector(256.20065262303626,307.1129050472639, 0.0));
    positionAlloc4->Add (Vector(37.12614327765556,226.25539987042194, 0.0));
    positionAlloc4->Add (Vector(24.890073354540874,144.85978397075772, 0.0));
    positionAlloc4->Add (Vector(197.0918843698306,167.2502542680229, 0.0));
    positionAlloc4->Add (Vector(345.82026112319903,217.39287445670783, 0.0));
    positionAlloc4->Add (Vector(242.73258265022685,176.73752112087072, 0.0));
    positionAlloc4->Add (Vector(76.80463965070952,236.65102638122698, 0.0));
    positionAlloc4->Add (Vector(87.98436958266281,306.55923578435755, 0.0));
    positionAlloc4->Add (Vector(186.2956274967622,334.21013067456863, 0.0));
    positionAlloc4->Add (Vector(390.030110236802,225.8938367636163, 0.0));
    positionAlloc4->Add (Vector(307.9062277366734,383.4418491702143, 0.0));
    positionAlloc4->Add (Vector(247.15469424021705,302.0982064587434, 0.0));
    positionAlloc4->Add (Vector(331.71200895462175,167.00630052089576, 0.0));
    positionAlloc4->Add (Vector(318.8129444579899,323.0831275967937, 0.0));
    positionAlloc4->Add (Vector(302.8905865062816,324.93484388733356, 0.0));
    positionAlloc4->Add (Vector(336.8522080097019,96.17668304765755, 0.0));
    positionAlloc4->Add (Vector(229.3808927930833,229.62377269217558, 0.0));
    positionAlloc4->Add (Vector(274.1759899010273,183.6465678570717, 0.0));
    positionAlloc4->Add (Vector(262.4695272205548,307.1142735733789, 0.0));
    positionAlloc4->Add (Vector(360.9195360078224,204.77906323698946, 0.0));
    positionAlloc4->Add (Vector(2.106073730527802,332.9215838344408, 0.0));
    positionAlloc4->Add (Vector(358.1274275696766,355.2053395987967, 0.0));
    positionAlloc4->Add (Vector(91.04591563802691,85.40163573880623, 0.0));
    positionAlloc4->Add (Vector(17.008115516401688,245.62145429265078, 0.0));
    positionAlloc4->Add (Vector(196.59181061555898,210.46770506740523, 0.0));
    positionAlloc4->Add (Vector(63.83012378281694,352.66381357400627, 0.0));
    positionAlloc4->Add (Vector(33.46197366522472,297.89671030387785, 0.0));
    positionAlloc4->Add (Vector(40.43915583909809,97.93934365290498, 0.0));
    positionAlloc4->Add (Vector(255.62556241521094,398.6411279926186, 0.0));
    positionAlloc4->Add (Vector(68.55307843532832,350.3197359544198, 0.0));
    positionAlloc4->Add (Vector(284.18424540032566,160.42247576420635, 0.0));
    positionAlloc4->Add (Vector(60.3079224526935,330.53341908840616, 0.0));
    positionAlloc4->Add (Vector(81.33394204907928,163.12518504016813, 0.0));
    positionAlloc4->Add (Vector(264.51681283779203,134.45857555982505, 0.0));
    positionAlloc4->Add (Vector(247.4001535707893,283.97144136734477, 0.0));
    positionAlloc4->Add (Vector(232.45538406563574,368.92831296681754, 0.0));
    positionAlloc4->Add (Vector(292.85422190609927,166.11659662183297, 0.0));
    positionAlloc4->Add (Vector(240.3309904504534,387.97541508640154, 0.0));
    positionAlloc4->Add (Vector(259.03303208171803,358.07355188965107, 0.0));
    positionAlloc4->Add (Vector(217.01797665886292,213.9079902330983, 0.0));
    positionAlloc4->Add (Vector(353.3614607871087,146.77929526004672, 0.0));
    positionAlloc4->Add (Vector(284.5244440418658,303.2529131116201, 0.0));
    positionAlloc4->Add (Vector(290.00395372694203,357.9463736051818, 0.0));
    positionAlloc4->Add (Vector(111.05120091210293,166.00281455391803, 0.0));
    positionAlloc4->Add (Vector(384.8689988627315,391.650197429796, 0.0));
    positionAlloc4->Add (Vector(143.49734111930297,341.33983940556266, 0.0));
    positionAlloc4->Add (Vector(177.16633253450203,311.2722271018282, 0.0));
    positionAlloc4->Add (Vector(288.1288840494177,210.52724641788421, 0.0));
    positionAlloc4->Add (Vector(57.86557420569159,42.72786249584466, 0.0));
    positionAlloc4->Add (Vector(85.64076630785196,299.08560106031206, 0.0));
    positionAlloc4->Add (Vector(96.97749823597928,347.9950505370714, 0.0));
    positionAlloc4->Add (Vector(277.32806231638574,253.22650715828564, 0.0));
    positionAlloc4->Add (Vector(385.2208347449632,131.2796990458537, 0.0));
    positionAlloc4->Add (Vector(346.2497950229709,355.48001981210484, 0.0));
    positionAlloc4->Add (Vector(145.37235721047875,141.2174743379758, 0.0));
    positionAlloc4->Add (Vector(157.55125510806022,24.56486162326574, 0.0));
    positionAlloc4->Add (Vector(169.8998036732353,129.70208965083555, 0.0));
    positionAlloc4->Add (Vector(359.9212295289873,313.25258890513464, 0.0));
    positionAlloc4->Add (Vector(308.72091830995447,263.23319239430197, 0.0));
    positionAlloc4->Add (Vector(130.77424676249882,360.93785568182665, 0.0));
    positionAlloc4->Add (Vector(353.43683601027413,334.5068864605652, 0.0));
    positionAlloc4->Add (Vector(117.53782823130106,353.89476745577383, 0.0));
    positionAlloc4->Add (Vector(350.03085351989773,86.45518005863977, 0.0));
    positionAlloc4->Add (Vector(259.94377048379675,28.669389476366458, 0.0));
    positionAlloc4->Add (Vector(221.95279073787682,68.66252171363469, 0.0));
    positionAlloc4->Add (Vector(240.32320861964766,129.1413874706287, 0.0));
    positionAlloc4->Add (Vector(238.18226531560094,225.50542780073025, 0.0));
    positionAlloc4->Add (Vector(168.89104913931945,12.319524465869325, 0.0));
    positionAlloc4->Add (Vector(328.79545772602825,251.56977533632502, 0.0));
    positionAlloc4->Add (Vector(278.93861583559567,285.3039590306946, 0.0));
    positionAlloc4->Add (Vector(152.41774993114038,340.9613150630217, 0.0));
    positionAlloc4->Add (Vector(285.4752365282006,313.90197463026766, 0.0));
    positionAlloc4->Add (Vector(95.61846573830817,278.66168017609215, 0.0));
    positionAlloc4->Add (Vector(45.924227114106664,79.96314522455035, 0.0));
    positionAlloc4->Add (Vector(153.2258848870266,392.4067167803948, 0.0));
    positionAlloc4->Add (Vector(258.74289710067393,115.09405382198321, 0.0));
    positionAlloc4->Add (Vector(191.86323431708306,161.73675615578298, 0.0));
    positionAlloc4->Add (Vector(289.32765425037877,41.03461009085416, 0.0));
    positionAlloc4->Add (Vector(360.7978378197293,260.6961466994896, 0.0));
    positionAlloc4->Add (Vector(115.9076503407571,192.66075826166548, 0.0));
    positionAlloc4->Add (Vector(30.991025601184763,211.22685570224328, 0.0));
    positionAlloc4->Add (Vector(268.6504594409334,106.04387758938509, 0.0));
    positionAlloc4->Add (Vector(278.20463016204366,147.89591181660612, 0.0));
    positionAlloc4->Add (Vector(145.26503798125194,387.09096364322437, 0.0));
    positionAlloc4->Add (Vector(210.9670081606609,22.539756693789805, 0.0));
    positionAlloc4->Add (Vector(227.18581832582316,265.21752926708785, 0.0));
    positionAlloc4->Add (Vector(88.35533453656232,82.20394443746085, 0.0));
    positionAlloc4->Add (Vector(89.0663817486554,55.205984817287444, 0.0));
    positionAlloc4->Add (Vector(349.84063948611725,301.30059406629675, 0.0));
    positionAlloc4->Add (Vector(264.86964137406846,382.29150997075305, 0.0));
    positionAlloc4->Add (Vector(302.0992997893587,273.6335285338467, 0.0));
    positionAlloc4->Add (Vector(220.45469222056582,21.48541536396671, 0.0));
    positionAlloc4->Add (Vector(92.4942887765892,368.9461676605054, 0.0));
    positionAlloc4->Add (Vector(14.695905812194976,125.85239125951131, 0.0));
    positionAlloc4->Add (Vector(254.32088744926818,394.13773523942933, 0.0));
    positionAlloc4->Add (Vector(375.1736314978532,149.95773622444645, 0.0));
    positionAlloc4->Add (Vector(146.20752925430932,132.29156441005787, 0.0));
    positionAlloc4->Add (Vector(373.392183333717,323.33078674190256, 0.0));
    positionAlloc4->Add (Vector(197.05617631716626,33.083319212755754, 0.0));
    positionAlloc4->Add (Vector(247.79605010446653,343.90522667679807, 0.0));
    positionAlloc4->Add (Vector(168.16080557215224,129.47576546350555, 0.0));
    positionAlloc4->Add (Vector(122.98155287352452,9.669468885933119, 0.0));
    positionAlloc4->Add (Vector(215.19505424709422,117.71528689732516, 0.0));
    positionAlloc4->Add (Vector(82.84403836294887,207.59346573161807, 0.0));
    positionAlloc4->Add (Vector(22.574572143376372,181.5216740461207, 0.0));
    positionAlloc4->Add (Vector(344.03510876411366,123.33799442074644, 0.0));
    positionAlloc4->Add (Vector(385.29420906562956,230.65385386010985, 0.0));
    positionAlloc4->Add (Vector(92.98323913813462,72.99419056468524, 0.0));
    positionAlloc4->Add (Vector(371.4432485596525,130.30736828172707, 0.0));
    positionAlloc4->Add (Vector(51.181268757118126,304.89550882432064, 0.0));
    positionAlloc4->Add (Vector(371.5451197285117,105.27204145461782, 0.0));
    positionAlloc4->Add (Vector(27.005620165347864,174.52481943969954, 0.0));
    positionAlloc4->Add (Vector(30.82569259897059,336.36326325009406, 0.0));
    positionAlloc4->Add (Vector(385.23662911317444,293.45691438020225, 0.0));
    positionAlloc4->Add (Vector(64.19663610568324,296.5641667919806, 0.0));
    positionAlloc4->Add (Vector(230.48591593036295,137.21947807662082, 0.0));
    positionAlloc4->Add (Vector(56.99833879381826,173.34016102381798, 0.0));
    positionAlloc4->Add (Vector(195.72371624170586,56.915000878929774, 0.0));
    positionAlloc4->Add (Vector(202.37624383015253,16.614073191429355, 0.0));
    positionAlloc4->Add (Vector(282.1475319636111,178.6791161265557, 0.0));
    positionAlloc4->Add (Vector(297.8449637845866,200.65488080215985, 0.0));
    positionAlloc4->Add (Vector(280.9410483344648,92.256362369187, 0.0));
    positionAlloc4->Add (Vector(296.4467414898115,200.40998576563274, 0.0));
    positionAlloc4->Add (Vector(269.02272412811806,184.19343599389526, 0.0));
    positionAlloc4->Add (Vector(15.050778107409046,233.21356849217798, 0.0));
    positionAlloc4->Add (Vector(22.37718611031303,140.6682223480227, 0.0));
    positionAlloc4->Add (Vector(104.20540770296971,43.321741085232105, 0.0));
    positionAlloc4->Add (Vector(152.74755583625955,364.94138770110357, 0.0));
    positionAlloc4->Add (Vector(80.20652254580271,40.666117461395544, 0.0));
    positionAlloc4->Add (Vector(225.08881149425878,330.57830640913164, 0.0));
    positionAlloc4->Add (Vector(67.12640239363337,119.43578451061838, 0.0));
    positionAlloc4->Add (Vector(348.3348771307754,293.89632772060867, 0.0));
    positionAlloc4->Add (Vector(28.21940668875058,97.18564679571884, 0.0));
    positionAlloc4->Add (Vector(274.7747592887275,256.2971079747432, 0.0));
    positionAlloc4->Add (Vector(88.26724883486948,274.4558807914979, 0.0));
    positionAlloc4->Add (Vector(397.34811558499234,262.48033373684245, 0.0));
    positionAlloc4->Add (Vector(0.15935202190071607,92.52756758229302, 0.0));
    positionAlloc4->Add (Vector(317.1551721789974,104.70041757778704, 0.0));
    positionAlloc4->Add (Vector(211.35387868915183,98.85233024005137, 0.0));
    positionAlloc4->Add (Vector(149.3111511459313,324.8747902995939, 0.0));
    positionAlloc4->Add (Vector(326.9353461918016,16.851264251242526, 0.0));
    positionAlloc4->Add (Vector(89.18056495018143,398.84466194351853, 0.0));
    positionAlloc4->Add (Vector(337.54069254477076,147.82694204692604, 0.0));
    positionAlloc4->Add (Vector(362.1312144309467,349.8266059541506, 0.0));
    positionAlloc4->Add (Vector(229.12971916843935,34.723353706991844, 0.0));
    positionAlloc4->Add (Vector(43.83944276366263,288.0830987100978, 0.0));
    positionAlloc4->Add (Vector(43.6259827389383,340.20139814696375, 0.0));
    positionAlloc4->Add (Vector(355.03551685735226,312.1038439825676, 0.0));
    positionAlloc4->Add (Vector(145.96307936938143,113.89360991625699, 0.0));
    positionAlloc4->Add (Vector(84.58693774274116,30.41907622990463, 0.0));
    positionAlloc4->Add (Vector(66.25587059169274,88.23020540617765, 0.0));
    positionAlloc4->Add (Vector(396.23383702564195,113.67803036823423, 0.0));
    positionAlloc4->Add (Vector(159.06083593884003,194.0850614920473, 0.0));
    positionAlloc4->Add (Vector(163.44339847084996,70.76168894417098, 0.0));
    positionAlloc4->Add (Vector(324.51635668160134,244.94357698404605, 0.0));
    positionAlloc4->Add (Vector(126.36916183277695,196.9871942188476, 0.0));
    positionAlloc4->Add (Vector(80.94693858957794,320.1127577584497, 0.0));
    positionAlloc4->Add (Vector(148.5169696098826,117.0845500780747, 0.0));
    positionAlloc4->Add (Vector(202.27058530344726,216.16666019843592, 0.0));
    positionAlloc4->Add (Vector(3.826757751404264,100.78056925817607, 0.0));
    positionAlloc4->Add (Vector(374.6109690707775,0.038780275069916215, 0.0));
    positionAlloc4->Add (Vector(223.6838864447308,197.08833192329115, 0.0));
    positionAlloc4->Add (Vector(305.108985488312,331.1260923687889, 0.0));
    positionAlloc4->Add (Vector(303.90918221920106,29.854086049144435, 0.0));
    positionAlloc4->Add (Vector(269.3510920778165,73.2004719147815, 0.0));
    positionAlloc4->Add (Vector(310.6454535928336,392.4840675164546, 0.0));
    positionAlloc4->Add (Vector(52.03438907924176,199.58324414601526, 0.0));
    positionAlloc4->Add (Vector(196.10798223588256,349.53340235464964, 0.0));
    positionAlloc4->Add (Vector(92.23138876930763,323.0737969806585, 0.0));
    positionAlloc4->Add (Vector(82.47126549369264,140.8865095327803, 0.0));
    positionAlloc4->Add (Vector(63.85940478917917,331.43026209214406, 0.0));
    positionAlloc4->Add (Vector(73.34349045122788,285.0967135162113, 0.0));
    positionAlloc4->Add (Vector(1.9145672423538151,245.15544773173878, 0.0));
    positionAlloc4->Add (Vector(5.4122032420875765,176.3516153349022, 0.0));
    positionAlloc4->Add (Vector(200.00495938751567,395.6561184563094, 0.0));
    positionAlloc4->Add (Vector(101.96943572341719,16.836703936949114, 0.0));
    positionAlloc4->Add (Vector(292.19546746145414,9.41116595113125, 0.0));
    positionAlloc4->Add (Vector(316.0534009738156,240.6397343647817, 0.0));
    positionAlloc4->Add (Vector(112.4502585455433,236.2383447035512, 0.0));
    positionAlloc4->Add (Vector(315.31780804493013,18.60507290710065, 0.0));
    positionAlloc4->Add (Vector(191.58163670957634,115.92133567694884, 0.0));
    positionAlloc4->Add (Vector(273.4549859124201,159.20468608651893, 0.0));
    positionAlloc4->Add (Vector(132.1284813878646,35.21936712374081, 0.0));
    positionAlloc4->Add (Vector(269.54075075330246,15.777119414155027, 0.0));
    positionAlloc4->Add (Vector(235.17589808418444,183.01487879454882, 0.0));
    positionAlloc4->Add (Vector(15.35497279056175,351.72033743863386, 0.0));
    positionAlloc4->Add (Vector(352.19896541061246,317.51325297273587, 0.0));
    positionAlloc4->Add (Vector(106.14874009520507,65.80052117546522, 0.0));
    positionAlloc4->Add (Vector(363.8013413929221,285.3477052076393, 0.0));
    positionAlloc4->Add (Vector(94.49110077483999,123.55978820404667, 0.0));
    positionAlloc4->Add (Vector(391.9996865774543,26.35750800205021, 0.0));
    positionAlloc4->Add (Vector(147.0584230678548,83.30627761203525, 0.0));
    positionAlloc4->Add (Vector(286.6117506365572,229.0230390781125, 0.0));
    positionAlloc4->Add (Vector(125.13269919080052,295.0031195176513, 0.0));
    positionAlloc4->Add (Vector(133.95553191453874,397.1504627036146, 0.0));
    positionAlloc4->Add (Vector(46.756690839356985,8.192499487360783, 0.0));
    positionAlloc4->Add (Vector(379.9315657449673,302.2533122316608, 0.0));
    positionAlloc4->Add (Vector(34.90665607041312,262.1558625929901, 0.0));
    positionAlloc4->Add (Vector(43.65133129656598,158.67244056110115, 0.0));
    positionAlloc4->Add (Vector(107.85729846440906,84.1831164162703, 0.0));
    positionAlloc4->Add (Vector(316.39023841877975,286.29941935649344, 0.0));
    positionAlloc4->Add (Vector(291.00155466344313,322.8764408096036, 0.0));
    positionAlloc4->Add (Vector(185.89275089240908,93.74257736059492, 0.0));
    positionAlloc4->Add (Vector(104.36032147795298,133.6607334072299, 0.0));
    positionAlloc4->Add (Vector(109.73646136327982,43.37233353274983, 0.0));
    positionAlloc4->Add (Vector(78.46031677733708,269.5703974231768, 0.0));
    positionAlloc4->Add (Vector(58.63673825325782,382.5458910587386, 0.0));
    positionAlloc4->Add (Vector(214.120071851535,261.8165215279752, 0.0));
    positionAlloc4->Add (Vector(218.1014056428121,25.390219951257144, 0.0));
    positionAlloc4->Add (Vector(373.814202071614,225.4036669900683, 0.0));
    positionAlloc4->Add (Vector(43.35155516166087,186.4855903252326, 0.0));
    positionAlloc4->Add (Vector(280.84452196564354,353.2931286607339, 0.0));
    positionAlloc4->Add (Vector(162.45716107232394,64.23473140369711, 0.0));
    positionAlloc4->Add (Vector(327.8662951557314,232.52798985068773, 0.0));
    positionAlloc4->Add (Vector(198.706424485334,230.92901252973158, 0.0));
    positionAlloc4->Add (Vector(314.8908545510428,167.38570556456995, 0.0));
    positionAlloc4->Add (Vector(160.1177226943103,123.16237337236208, 0.0));
    positionAlloc4->Add (Vector(192.5874061494932,313.8923839946917, 0.0));
    positionAlloc4->Add (Vector(197.73157221760522,334.8823983797679, 0.0));
    positionAlloc4->Add (Vector(59.47394644529216,223.87459960070916, 0.0));
    positionAlloc4->Add (Vector(94.66695070604096,275.045984864326, 0.0));
    positionAlloc4->Add (Vector(183.94294719969452,239.04345955074803, 0.0));
    positionAlloc4->Add (Vector(116.12005551016367,319.6029992786527, 0.0));
    positionAlloc4->Add (Vector(106.90904540795555,356.02292150963655, 0.0));
    positionAlloc4->Add (Vector(331.6029963794386,393.66393835951385, 0.0));
    positionAlloc4->Add (Vector(374.01829076528344,22.078499461802224, 0.0));
    positionAlloc4->Add (Vector(155.59165568972335,199.17838155497046, 0.0));
    positionAlloc4->Add (Vector(284.54827532040997,67.12846544943774, 0.0));
    positionAlloc4->Add (Vector(58.75032628902854,166.22868828665435, 0.0));
    positionAlloc4->Add (Vector(260.0651945262738,35.99804121307888, 0.0));
    positionAlloc4->Add (Vector(284.7757609117438,223.017565630989, 0.0));
    positionAlloc4->Add (Vector(73.33876270756932,304.1719057225754, 0.0));
    positionAlloc4->Add (Vector(272.35221084933465,338.0994586015295, 0.0));
    positionAlloc4->Add (Vector(118.89657812067055,254.03197301801956, 0.0));
    positionAlloc4->Add (Vector(59.49342512102436,122.37817883446418, 0.0));
    positionAlloc4->Add (Vector(126.44285290327923,267.84670587435045, 0.0));
    positionAlloc4->Add (Vector(80.67149392119322,327.1028774772498, 0.0));
    positionAlloc4->Add (Vector(85.60390908306412,241.8286413225495, 0.0));
    positionAlloc4->Add (Vector(53.24385795036126,273.4430731217989, 0.0));
    positionAlloc4->Add (Vector(48.838727825376615,72.91558829999394, 0.0));
	//AUTOCODE USERS FIM

	// Define o modelo de mobilidade (constante)
	uesMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	// Passa a lista de posições para o assistente de mobilidade
	uesMobility.SetPositionAllocator(positionAlloc4);
	// instala as configurações de mobilidade nos nós ueNodes
	uesMobility.Install (ueNodes);

	// Crie um contêiner de dispositivos de rede para os UE
	NetDeviceContainer ueLteDevs;
	// Cria um conjunto de UE nos nós ueNodes.
	ueLteDevs = lteHelper->InstallUeDevice (ueNodes);

//-----------------------------------------------------Roteamento
	// Instala pilha de protocolos da internet em cada nó do contêiner ueNodes
	internet.Install (ueNodes);
	
	// Ipv4InterfaceContainer contém um vetor de std::pair de Ptr<Ipv4> e índice de interface.
	Ipv4InterfaceContainer ueIpIface; 
	// Atribui endereços IPv4 a dispositivos UE (ueLteDevs).
	ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));

	for (uint32_t u = 0; u < numberOfNodes; u++)
		{
			// Obtém o nó do contêiner
			Ptr<Node> ueNode_sg = ueNodes.Get (u);
			// Tente encontrar o protocolo de roteamento estático como o protocolo de roteamento principal ou 
		 	// na lista de protocolos de roteamento associados ao IPv4 fornecido (ueNode_sg)
			Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode_sg->GetObject<Ipv4> ());
			
			/* SetDefaultRoute (nextHop, interface): Adiciona uma rota padrão à tabela de roteamento estático.
				nextHop: O Ipv4Address para enviar pacotes na esperança de que eles sejam encaminhados corretamente.
				interface: O índice de interface de rede usado para enviar pacotes
				GetUeDefaultGatewayAddress: Retorna o endereço IPv4 do Gateway padrão a ser usado pelos UEs para acessar a Internet
			*/
			ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
		}
	 // for (int i = 0; i < numberOfRrhs; i++)
	 // {
		// for (uint16_t u = 0; u < numberOfNodes[i]; u++)
		// 	{
		// 		lteHelper->Attach (ueLteDevs[i].Get(u), enbLteDevs.Get(i));
		// 	}
	 // }

		/* lteHelper->Attach: Permite a conexão automática de um conjunto de dispositivos UE a uma célula adequada usando o 
			procedimento de seleção de célula inicial no modo inativo.

			Ao chamar o attach, o UE iniciará o procedimento inicial de seleção de células no início da simulação. Além disso, 
			a função também instrui cada UE a entrar imediatamente no modo CONNECTED e ativa o EPS (Evolved Packet switched System) bearer padrão.
			Se esta função for chamada quando o UE estiver em uma situação em que não é possível entrar no modo CONNECTED (por exemplo, antes do início da simulação), 
			o UE tentará se conectar o mais cedo possível (por exemplo, depois de acampar em uma célula adequada).
		*/
		lteHelper->Attach (ueLteDevs);

//-----------------------------------------------------Bearer

	// Laço que percorre todos os usuários
	for (uint32_t u = 0; u < numberOfNodes; u++)
		{
			//Obtém o ponteiro do dispositivo de rede (ueLteNetDevice) de um usuário do contêiner ueLteDevs
			Ptr<NetDevice> ueDevice = ueLteDevs.Get (u);

			/* Informação sobre o 3GPP (3rd Generation Partnership Project ) TS 36.413 9.2.1.18 GBR (Guaranteed Bit Rate) QoS (Quality of Service)
				GBR garante uma taxa de dados entre o UE e o PGW. Para garantir a taxa de dados é necessário que o EPS bearer seja dedicado
				Todos os usuários vão ter essa configuração de EPS bearer.
				Para mais informações sobre EPS bearer, acesse: https://www.netmanias.com/en/post/blog/5933/lte-qos/lte-qos-part-2-lte-qos-parameters-qci-arp-gbr-mbr-and-ambr

			*/
			GbrQosInformation qos;

			/*  Define a taxa de dados(bits/s) garantida de Downlink no EPS bearer considerando 
				IP, UDP, RLC (Radio Link Control), PDCP (Packet Data Convergence Protocol) header size 
			*/
			qos.gbrDl = 132;
			// Define a taxa de dados(bits/s) garantida de Uplink no EPS bearer 
			qos.gbrUl = 132;

			/* MBR (Maximum Bit Rate)
				Define o MBR de Downlink e Uplink do EPS beater
			*/
			qos.mbrDl = qos.gbrDl;
			qos.mbrUl = qos.gbrUl;

			/* QCI (Qos Class Identifier)
				Número inteiro que define o tipo de performace de Qos
				GBR_CONV_VOICE: Possui QCI = 1 e pode ser utilizado para serviços de voz. Mais informações em:
					https://www.nsnam.org/doxygen/classns3_1_1_eps_bearer.html#details
			*/
			enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;

			/* Cria um EPS bearer com as especificações anteriores
				q: corresponde ao QCI configurado
				qos: corresponde GbrQosInformation configurado
			*/
			EpsBearer bearer (q, qos);

			// Verdadeiro significa que o EPS bearer pode ter prioridade ou antecipação sobre outros
			bearer.arp.preemptionCapability = true;
			// Verdadeiro significa que o EPS bearer pode ser antecipado por outros 
			bearer.arp.preemptionVulnerability = true;

			/* Ativa um EPS bearer dedicado em um determinado conjunto de dispositivos UE.
				ueDevice: o conjunto de dispositvos UE do LTE
				bearer: as características do bearer a ser ativado
				EpcTft: Essa classe implementa o modelo de fluxo de tráfego (TFT) do EPS bearer, 
					que é o conjunto de todos os filtros de pacote associados a um portador EPS.
			*/
			lteHelper->ActivateDedicatedEpsBearer (ueDevice, bearer, EpcTft::Default ());
		}

//-----------------------------------------------------Aplicação
	//uint16_t dlPort = 900;
	 
	// Número da porta de uplink
	uint16_t ulPort = 900;
	/* ApplicationContainer: contém um vetor de ponteiros ns3::Application (clase base para aplicações no NS-3)
		Cria um contêiner para as aplicações dos usuários e do servidor remoto
	*/
	ApplicationContainer clientApps;
	ApplicationContainer serverApps;
	
	/* Tempo de início das aplicações
		Gera um número aleatório entre 2.0 e 2.020
	*/
	Ptr<UniformRandomVariable> startTimeSeconds = CreateObject<UniformRandomVariable> ();
	startTimeSeconds->SetAttribute ("Min", DoubleValue (2.0));
	startTimeSeconds->SetAttribute ("Max", DoubleValue (2.020));
	//++ulPort;
	//++dlPort;
	//PacketSinkHelper dlPacketSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));

	/* PacketSinkHelper: um assistente para facilitar a instanciação de um ns3::PacketSinkApplication em um conjunto de nós.
		ns3::UdpSocketFactory: o nome do protocolo a ser usado para receber tráfego
			Essa string identifica o tipo de SocketFactory usado para criar soquetes para os aplicativos. Nesse caso é UDP
		O segundo parâmetro é o endereço do sink
		InetSocketAddress: essa classe contém um Ipv4Address e um número de porta para formar um endpoint de transporte IPv4.
	*/
	PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
	//PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), otherPort));
	//serverApps.Add (dlPacketSinkHelper.Install (ueNodes[u]));

	//Instala um ns3::PacketSinkApplication em cada nó do contêiner remoteHost
	// Adiciona a aplicação instalada no contêiner de apliacações serverApps 
	serverApps.Add (ulPacketSinkHelper.Install (remoteHost));

	//serverApps.Add (packetSinkHelper.Install (ueNodes.Get(u))); 
	//BulkSendHelper dlClient ("ns3::TcpSocketFactory", InetSocketAddress (ueIpIface.GetAddress (u), dlPort));
	//dlClient.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
	//dlClient.SetAttribute ("MaxPackets", UintegerValue(1000000));
		
	//BulkSendHelper ulClient ("ns3::TcpSocketFactory", InetSocketAddress (remoteHostAddr, ulPort));

	/* Configuração de UDP
		UdpClientHelper: Cria uma aplicação de cliente que envia pacotes UDP com um número de sequência de 32 bits e um timestamp de 64 bits.
		remoteHostAddr: O endereço IP do servidor UDP remoto
		ulPort: O número da porta do servidor UDP remoto
	*/
	UdpClientHelper ulClient (remoteHostAddr, ulPort);
	// Interval: O tempo de espera entre os pacotes
	ulClient.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
	// MaxPackets: Número máximo de pacotes que a aplicação vai enviar
	ulClient.SetAttribute ("MaxPackets", UintegerValue(1000000));

	//UdpClientHelper client (ueIpIface.GetAddress (u), otherPort);
	//client.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
	//client.SetAttribute ("MaxPackets", UintegerValue(1000000));

	//clientApps.Add (dlClient.Install (remoteHost));
	
	/* Instala aplicações UDP nos usuários
		ulClient.Install: Instala as aplicações UDP em cada um dos nós dos usuários e retorna essa aplicações sendo uma aplicação por nó de usuário
		Adiciona essas aplicações no final do contêiner clientApps		
	*/
	clientApps.Add (ulClient.Install (ueNodes));

	/* Tempo de início das aplicações
		Obtém o número aleatório armazenado em startTimeSeconds e transforma para segundo
		Armazena esse valor no objeto startTime (instanciado a partir da classe Time)
	*/
	Time startTime = Seconds (startTimeSeconds->GetValue ());
	// Start(Time start): indica o tempo de início das aplicações de acordo com o parâmetro do tipo Time fornecido.
	serverApps.Start (startTime);
	clientApps.Start (startTime);
	// Indica o tempo (s) de parada das aplicações de clientes contidas no contêiner clientApps
	clientApps.Stop (Seconds (simTime));
		 

//-----------------------------------------------------OpenFLow
	// Cria um ponteiro da classe FlowMonitor
	Ptr<FlowMonitor> flowmon;
	// Helper para habilitar o monitoramento de fluxo IP em um conjunto de nós.
	FlowMonitorHelper flowmonHelper;

	// Habilita o monitoramento do fluxo em todos os nós
	flowmon = flowmonHelper.InstallAll ();

	// Executa caso a variável trace esteja com valor True
	if (trace)
	{
		// Habilita os rastreamentos de pcap no canal OpenFlow entre o controlador e os switches.
		ofMyCtrlHelper->EnableOpenFlowPcap ("openflow");
		// Habilita as estatísticas do OpenFlow datapath em dispositivos de switch OpenFlow configurados por este assistente (ofMyCtrlHelper).
		// Prefixo do nome de arquivo a ser usado para arquivos de estatísticas.
		ofMyCtrlHelper->EnableDatapathStats ("switch-stats");

		// Habilita a saída pcap em cada dispositivo no contêiner
		// csmaHelper.EnablePcap (prefixo do nome do arquivo para usar nos arquivos pcap, contêiner de dispositivos do tipo ns3::CsmaNetDevice)
		csmaHelper.EnablePcap ("mainswitch", mainswitchC);
		csmaHelper.EnablePcap ("topswitch", topswitchC);
		csmaHelper.EnablePcap ("midswitch", midswitch);
		csmaHelper.EnablePcap ("server", hostDevices);
		//lteHelper->EnableTraces ();

		// Habilita trace sinks para a camada PHY 
		lteHelper->EnablePhyTraces ();
		// Habilita trace sinks para a camada MAC
		lteHelper->EnableMacTraces ();
		// Habilita trace sinks para a camada RLC
		lteHelper->EnableRlcTraces ();
		// Habilita trace sinks para a camada PDCP
		lteHelper->EnablePdcpTraces ();
	}

	// Cria interface para o network animator.
	//AUTOCODE ANIMATION INICIO
	  AnimationInterface anim ("animations/animation_SUI_4.xml");
	//AUTOCODE ANIMATION FIM
	
	// Indica o tempo (s) de parada do simulador
	Simulator::Stop(Seconds(simTime));
	// AnimationInterface anim ("cran.xml");

	// Defina o máximo de pacotes por arquivo de rastreamento
	anim.SetMaxPktsPerTraceFile (999999);

//-----------------------------------------------------Handover

	/* Configura interface X2 entre as eNBs
		Cria uma interface X2 entre todos os eNBs fornecidas.
		X2: é a interface de interconexão entre dois eNodeBs na rede LTE
	*/
	lteHelper->AddX2Interface (enbNodes);

	// X2-based Handover
	//lteHelper->HandoverRequest (Seconds (3.0), ueLteDevs[1].Get (0), enbLteDevs.Get (1), enbLteDevs.Get (2));

		//Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/ConnectionEstablished",
		//               MakeCallback (&MapearUE));

		/* 
			Config::Connect - Esta função tentará encontrar todas as fontes de rastreamento que correspondam ao caminho de entrada informado no primeiro parâmetro e
			conectará a callback de entrada a elas de forma que a callback receba uma string de contexto extra na notificação do evento de rastreamento

			Config::Connect (caminho para corresponder às origens de rastreamento ou trace sources, Função de callback para se conectar às origens de rastreamento correspondentes)	
		*/
		Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/ConnectionEstablished",
									 MakeCallback (&NotifyConnectionEstablishedUe));
		Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverEndOk",
									 MakeCallback (&MyController::Update, MyCtrl));

//-----------------------------------------------------Schedule/Callbacks

	// Agenda a função de callback indicada para ocorrer no tempo especificado
	// Passa os parêmetros necessários da função
	// * Essas funções foram declaradas antes do main no início desse arquivo
	Simulator::Schedule(Seconds(0.5),&CallMap, epcHelper);
	Simulator::Schedule(Seconds(1.5),&SendToController, MyCtrl);

	// std::map <uint64_t, Ipv4Address> mymap = epcHelper->RecvMymap();
	// for (std::map<uint64_t, Ipv4Address >::iterator it=mymap.begin(); it!=mymap.end(); ++it)
	// std::cout << it->first << " => " << it->second << '\n';
	
	// Executa a simulaçao
	Simulator::Run ();
	
	// Executa os eventos agendados com ScheduleDestroy()
	// ScheduleDestroy() agenda um evento para ser executado no final da simulação
	Simulator::Destroy();
	 
	 /* Serializa os resultados para um std::string no formato XML.
	 */
	//INICIO FLOW MONITOR
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_4.flowmon", false, false);
	//FIM FLOW MONITOR

	/* PacketSink: Recebe e consume o tráfego gerado para um endereço IP e porta.
		Nesse caso, está relecionado às aplicações do remoteHost
	*/
	Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (serverApps.Get (0));

	// Imprime no terminal o total de bytes recebidos nessa aplicação sink (sink1)  
	std::cout << "Bytes received by server 1: " << sink1->GetTotalRx () << " ("
							<< (8. * sink1->GetTotalRx ()) / 1000000 / simTime << " Mbps)"
							<< std::endl;
	return 0;
	 
}