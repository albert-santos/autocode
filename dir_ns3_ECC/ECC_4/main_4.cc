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
	  uint16_t numberOfRrhs = 32;
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
    positionAlloc->Add (Vector (444.44444444444446,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,0.0, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,55.55555555555556, 0.0));
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
    positionAlloc4->Add (Vector(41.86663307520566,257.98483102074994, 0.0));
    positionAlloc4->Add (Vector(428.38811243419923,93.82700577748903, 0.0));
    positionAlloc4->Add (Vector(428.3771346735406,238.79751180500276, 0.0));
    positionAlloc4->Add (Vector(275.38228026143634,21.981331765787758, 0.0));
    positionAlloc4->Add (Vector(174.3386313963064,338.5834089662453, 0.0));
    positionAlloc4->Add (Vector(112.69646501538855,58.97792613121949, 0.0));
    positionAlloc4->Add (Vector(41.73027324344497,327.9451585080835, 0.0));
    positionAlloc4->Add (Vector(498.35033567679073,411.3414905749531, 0.0));
    positionAlloc4->Add (Vector(6.926037248259409,222.74355433749065, 0.0));
    positionAlloc4->Add (Vector(107.05086196772568,255.39370915645375, 0.0));
    positionAlloc4->Add (Vector(262.7352853755463,95.48365446399632, 0.0));
    positionAlloc4->Add (Vector(206.72663787788915,152.12758272000065, 0.0));
    positionAlloc4->Add (Vector(227.70509768064895,299.7030443172275, 0.0));
    positionAlloc4->Add (Vector(375.6358195397336,444.920681467113, 0.0));
    positionAlloc4->Add (Vector(176.05716212988636,486.81473199053005, 0.0));
    positionAlloc4->Add (Vector(288.60844704781726,279.7805117238076, 0.0));
    positionAlloc4->Add (Vector(209.2234433677131,147.6257350718456, 0.0));
    positionAlloc4->Add (Vector(498.0606742792527,75.85270880973339, 0.0));
    positionAlloc4->Add (Vector(320.3235785360515,23.808391705319508, 0.0));
    positionAlloc4->Add (Vector(320.59327890758846,278.4779723396655, 0.0));
    positionAlloc4->Add (Vector(289.0399934251167,436.7063638943884, 0.0));
    positionAlloc4->Add (Vector(199.2851192522141,309.49254978538437, 0.0));
    positionAlloc4->Add (Vector(219.78422423247162,20.333894944578322, 0.0));
    positionAlloc4->Add (Vector(165.53871529552706,181.01158268777007, 0.0));
    positionAlloc4->Add (Vector(208.8806212552235,485.97523105476654, 0.0));
    positionAlloc4->Add (Vector(331.5756637815003,128.2145703307252, 0.0));
    positionAlloc4->Add (Vector(480.3889183919283,102.7501605377626, 0.0));
    positionAlloc4->Add (Vector(289.4756241304536,301.4221829095663, 0.0));
    positionAlloc4->Add (Vector(62.129621812725524,202.29132576109387, 0.0));
    positionAlloc4->Add (Vector(347.04528168522734,295.37815856426386, 0.0));
    positionAlloc4->Add (Vector(371.93916749430116,450.5470418957567, 0.0));
    positionAlloc4->Add (Vector(360.9113157017137,297.65355226366967, 0.0));
    positionAlloc4->Add (Vector(24.448675586230706,437.1881062602031, 0.0));
    positionAlloc4->Add (Vector(3.3977226705836916,185.80925472695614, 0.0));
    positionAlloc4->Add (Vector(147.15309984520962,142.74729527247638, 0.0));
    positionAlloc4->Add (Vector(301.7931497552671,117.20396476692419, 0.0));
    positionAlloc4->Add (Vector(181.95017244389695,152.54894712380417, 0.0));
    positionAlloc4->Add (Vector(287.44479636254044,169.5262884758702, 0.0));
    positionAlloc4->Add (Vector(446.2387284286712,34.01661115354371, 0.0));
    positionAlloc4->Add (Vector(373.91029017122,165.78822867323683, 0.0));
    positionAlloc4->Add (Vector(273.59938253619947,225.1787740761555, 0.0));
    positionAlloc4->Add (Vector(172.94670423584847,282.98312836754496, 0.0));
    positionAlloc4->Add (Vector(48.318029919492965,457.9920155581695, 0.0));
    positionAlloc4->Add (Vector(153.4931968899997,75.03284244295244, 0.0));
    positionAlloc4->Add (Vector(187.70316137427596,114.20452590582092, 0.0));
    positionAlloc4->Add (Vector(65.57509359402813,71.84298649941084, 0.0));
    positionAlloc4->Add (Vector(216.56328595641884,367.2044230573941, 0.0));
    positionAlloc4->Add (Vector(357.3898174728026,163.47870896481703, 0.0));
    positionAlloc4->Add (Vector(133.59160554249627,311.90775321420745, 0.0));
    positionAlloc4->Add (Vector(372.95046439733704,484.5039111210176, 0.0));
    positionAlloc4->Add (Vector(417.6604099257023,4.669343621402811, 0.0));
    positionAlloc4->Add (Vector(269.54026817372073,111.80127628296854, 0.0));
    positionAlloc4->Add (Vector(48.439115132401625,100.36831096103904, 0.0));
    positionAlloc4->Add (Vector(126.54310855426593,23.124211594776177, 0.0));
    positionAlloc4->Add (Vector(216.09185675444047,471.2555716916074, 0.0));
    positionAlloc4->Add (Vector(432.3639341881627,390.8400269714367, 0.0));
    positionAlloc4->Add (Vector(486.4982891135864,352.30953310551934, 0.0));
    positionAlloc4->Add (Vector(396.7629720588462,51.97006605505761, 0.0));
    positionAlloc4->Add (Vector(190.5244484282073,227.87343103124275, 0.0));
    positionAlloc4->Add (Vector(186.24370346847223,279.208960561765, 0.0));
    positionAlloc4->Add (Vector(371.11739408756335,133.05058442508127, 0.0));
    positionAlloc4->Add (Vector(467.21979933974075,447.3881191756707, 0.0));
    positionAlloc4->Add (Vector(160.82536301231733,355.3326983330986, 0.0));
    positionAlloc4->Add (Vector(406.57383164321425,338.67092996027776, 0.0));
    positionAlloc4->Add (Vector(61.098406461352454,251.5325242755499, 0.0));
    positionAlloc4->Add (Vector(238.3670265984239,360.70137646713243, 0.0));
    positionAlloc4->Add (Vector(450.97883883411157,205.76688859231334, 0.0));
    positionAlloc4->Add (Vector(244.3390267809633,112.69272904895305, 0.0));
    positionAlloc4->Add (Vector(301.43713505572816,163.51864193312292, 0.0));
    positionAlloc4->Add (Vector(373.8375967868729,78.66139776506587, 0.0));
    positionAlloc4->Add (Vector(431.70592164211126,319.4095940840533, 0.0));
    positionAlloc4->Add (Vector(153.47493368525755,457.79201122499256, 0.0));
    positionAlloc4->Add (Vector(143.94193543970584,422.62326662327814, 0.0));
    positionAlloc4->Add (Vector(461.80593235109916,208.34224303033093, 0.0));
    positionAlloc4->Add (Vector(135.72939266644067,237.8618136551847, 0.0));
    positionAlloc4->Add (Vector(3.278437692967151,158.01173165229199, 0.0));
    positionAlloc4->Add (Vector(207.3269634004355,29.3208385929854, 0.0));
    positionAlloc4->Add (Vector(398.35078999207155,283.7316688339531, 0.0));
    positionAlloc4->Add (Vector(157.12799514769404,66.46252756860538, 0.0));
    positionAlloc4->Add (Vector(397.62293208984477,436.12135467108413, 0.0));
    positionAlloc4->Add (Vector(113.30384827703116,396.19011263526363, 0.0));
    positionAlloc4->Add (Vector(378.93472212233553,496.09961184496876, 0.0));
    positionAlloc4->Add (Vector(18.476511386668005,232.7052543666681, 0.0));
    positionAlloc4->Add (Vector(11.317357066821465,22.14954440106187, 0.0));
    positionAlloc4->Add (Vector(28.313178397003924,340.39801712893455, 0.0));
    positionAlloc4->Add (Vector(356.3963147669853,51.950554127643656, 0.0));
    positionAlloc4->Add (Vector(434.0060492987903,98.18602001175047, 0.0));
    positionAlloc4->Add (Vector(346.5471722506898,334.2753620114453, 0.0));
    positionAlloc4->Add (Vector(443.46920227418826,18.802720187886145, 0.0));
    positionAlloc4->Add (Vector(145.06036884616063,321.992348136483, 0.0));
    positionAlloc4->Add (Vector(385.3841637128818,222.16258868638934, 0.0));
    positionAlloc4->Add (Vector(39.27261974631124,77.55975652501823, 0.0));
    positionAlloc4->Add (Vector(114.4459338844156,360.45835355605516, 0.0));
    positionAlloc4->Add (Vector(129.29699726103848,27.32807770966117, 0.0));
    positionAlloc4->Add (Vector(439.58538140072085,48.91960151635438, 0.0));
    positionAlloc4->Add (Vector(299.26824598156895,420.7027668498791, 0.0));
    positionAlloc4->Add (Vector(269.38834999145246,354.3805687540332, 0.0));
    positionAlloc4->Add (Vector(264.15034634862724,334.28426220786724, 0.0));
    positionAlloc4->Add (Vector(319.92835767217343,105.88564539441752, 0.0));
    positionAlloc4->Add (Vector(242.13895099840698,225.5343971316516, 0.0));
    positionAlloc4->Add (Vector(189.70097199844255,423.1779378988355, 0.0));
    positionAlloc4->Add (Vector(460.34722652438325,190.00290172623963, 0.0));
    positionAlloc4->Add (Vector(283.2957040992293,17.053208699164934, 0.0));
    positionAlloc4->Add (Vector(179.4230168267704,326.87573625515154, 0.0));
    positionAlloc4->Add (Vector(289.8685235236717,366.7585091265467, 0.0));
    positionAlloc4->Add (Vector(162.9543602291878,36.72008318399933, 0.0));
    positionAlloc4->Add (Vector(485.9990699921922,354.686895410372, 0.0));
    positionAlloc4->Add (Vector(33.88541622905689,340.35116273329055, 0.0));
    positionAlloc4->Add (Vector(47.18626796710718,449.68426569643805, 0.0));
    positionAlloc4->Add (Vector(310.59469343543736,247.50369792140037, 0.0));
    positionAlloc4->Add (Vector(443.3239072729273,122.97217063677645, 0.0));
    positionAlloc4->Add (Vector(45.79740108577263,136.83865190248324, 0.0));
    positionAlloc4->Add (Vector(256.5745037706079,214.71101484032607, 0.0));
    positionAlloc4->Add (Vector(400.07215595874976,455.654177136823, 0.0));
    positionAlloc4->Add (Vector(377.0840929722234,160.89310027619607, 0.0));
    positionAlloc4->Add (Vector(201.94181877558697,138.20210993018517, 0.0));
    positionAlloc4->Add (Vector(43.70365808087201,185.96072825695137, 0.0));
    positionAlloc4->Add (Vector(423.6521027123193,65.17875722077504, 0.0));
    positionAlloc4->Add (Vector(185.5387033109284,365.59541210018233, 0.0));
    positionAlloc4->Add (Vector(343.7527625805115,351.0084376706783, 0.0));
    positionAlloc4->Add (Vector(5.257004142265098,74.56349938363105, 0.0));
    positionAlloc4->Add (Vector(10.99625499251239,84.28284112753049, 0.0));
    positionAlloc4->Add (Vector(0.37045648027445033,367.607777118117, 0.0));
    positionAlloc4->Add (Vector(487.7709889437481,381.86001589169734, 0.0));
    positionAlloc4->Add (Vector(150.4447165651419,452.17262888592563, 0.0));
    positionAlloc4->Add (Vector(45.733970982807456,34.04288711986181, 0.0));
    positionAlloc4->Add (Vector(100.04593153419633,232.5757658274793, 0.0));
    positionAlloc4->Add (Vector(30.37536816484704,174.25660469440774, 0.0));
    positionAlloc4->Add (Vector(394.9533209332221,451.31733534315435, 0.0));
    positionAlloc4->Add (Vector(284.6661937418636,292.57833648132697, 0.0));
    positionAlloc4->Add (Vector(21.03031322461635,492.4426320291508, 0.0));
    positionAlloc4->Add (Vector(366.3467872395912,281.52036178669755, 0.0));
    positionAlloc4->Add (Vector(89.87632671502699,19.279530631397478, 0.0));
    positionAlloc4->Add (Vector(224.44601037289786,418.9143056899087, 0.0));
    positionAlloc4->Add (Vector(259.75317679322853,32.81741912925212, 0.0));
    positionAlloc4->Add (Vector(378.2351927620513,434.29449946524073, 0.0));
    positionAlloc4->Add (Vector(375.34518582583587,82.93351473976396, 0.0));
    positionAlloc4->Add (Vector(311.03456263648343,115.54806376841282, 0.0));
    positionAlloc4->Add (Vector(382.2740446673233,146.62459285344727, 0.0));
    positionAlloc4->Add (Vector(466.3941752220678,483.24860034010976, 0.0));
    positionAlloc4->Add (Vector(149.37309619167237,345.0075146324625, 0.0));
    positionAlloc4->Add (Vector(51.08517966719495,12.413514593954012, 0.0));
    positionAlloc4->Add (Vector(375.12197538955814,274.4297002153932, 0.0));
    positionAlloc4->Add (Vector(316.2676281720714,93.53037758773131, 0.0));
    positionAlloc4->Add (Vector(320.2508157787953,383.89113130907987, 0.0));
    positionAlloc4->Add (Vector(46.40767909706944,282.8192498380274, 0.0));
    positionAlloc4->Add (Vector(31.112591693176096,181.07472996344714, 0.0));
    positionAlloc4->Add (Vector(246.36485546228826,209.06281783502862, 0.0));
    positionAlloc4->Add (Vector(432.2753264039988,271.7410930708848, 0.0));
    positionAlloc4->Add (Vector(303.4157283127836,220.9219014010884, 0.0));
    positionAlloc4->Add (Vector(96.0057995633869,295.81378297653373, 0.0));
    positionAlloc4->Add (Vector(109.9804619783285,383.199044730447, 0.0));
    positionAlloc4->Add (Vector(232.86953437095275,417.76266334321076, 0.0));
    positionAlloc4->Add (Vector(487.5376377960025,282.3672959545204, 0.0));
    positionAlloc4->Add (Vector(384.8827846708417,479.30231146276793, 0.0));
    positionAlloc4->Add (Vector(308.9433678002713,377.6227580734293, 0.0));
    positionAlloc4->Add (Vector(414.6400111932772,208.7578756511197, 0.0));
    positionAlloc4->Add (Vector(398.5161805724874,403.8539094959921, 0.0));
    positionAlloc4->Add (Vector(378.6132331328519,406.16855485916693, 0.0));
    positionAlloc4->Add (Vector(421.06526001212734,120.22085380957192, 0.0));
    positionAlloc4->Add (Vector(286.7261159913541,287.0297158652195, 0.0));
    positionAlloc4->Add (Vector(342.71998737628417,229.55820982133963, 0.0));
    positionAlloc4->Add (Vector(328.08690902569356,383.8928419667236, 0.0));
    positionAlloc4->Add (Vector(451.149420009778,255.97382904623683, 0.0));
    positionAlloc4->Add (Vector(2.6325921631597526,416.151979793051, 0.0));
    positionAlloc4->Add (Vector(447.6592844620958,444.00667449849584, 0.0));
    positionAlloc4->Add (Vector(113.80739454753363,106.7520446735078, 0.0));
    positionAlloc4->Add (Vector(21.260144395502113,307.0268178658135, 0.0));
    positionAlloc4->Add (Vector(245.73976326944873,263.0846313342565, 0.0));
    positionAlloc4->Add (Vector(79.78765472852118,440.8297669675078, 0.0));
    positionAlloc4->Add (Vector(41.8274670815309,372.3708878798474, 0.0));
    positionAlloc4->Add (Vector(50.54894479887262,122.42417956613122, 0.0));
    positionAlloc4->Add (Vector(319.53195301901366,498.3014099907732, 0.0));
    positionAlloc4->Add (Vector(85.69134804416039,437.8996699430248, 0.0));
    positionAlloc4->Add (Vector(355.23030675040707,200.52809470525796, 0.0));
    positionAlloc4->Add (Vector(75.38490306586687,413.1667738605077, 0.0));
    positionAlloc4->Add (Vector(101.66742756134911,203.90648130021017, 0.0));
    positionAlloc4->Add (Vector(330.64601604724004,168.0732194497813, 0.0));
    positionAlloc4->Add (Vector(309.25019196348666,354.964301709181, 0.0));
    positionAlloc4->Add (Vector(290.56923008204467,461.1603912085219, 0.0));
    positionAlloc4->Add (Vector(366.06777738262406,207.6457457772912, 0.0));
    positionAlloc4->Add (Vector(300.4137380630668,484.9692688580019, 0.0));
    positionAlloc4->Add (Vector(323.79129010214757,447.59193986206384, 0.0));
    positionAlloc4->Add (Vector(271.27247082357866,267.38498779137285, 0.0));
    positionAlloc4->Add (Vector(441.70182598388584,183.47411907505838, 0.0));
    positionAlloc4->Add (Vector(355.65555505233226,379.06614138952517, 0.0));
    positionAlloc4->Add (Vector(362.5049421586776,447.4329670064772, 0.0));
    positionAlloc4->Add (Vector(138.81400114012865,207.50351819239754, 0.0));
    positionAlloc4->Add (Vector(481.0862485784143,489.562746787245, 0.0));
    positionAlloc4->Add (Vector(179.37167639912872,426.6747992569533, 0.0));
    positionAlloc4->Add (Vector(221.45791566812755,389.0902838772853, 0.0));
    positionAlloc4->Add (Vector(360.1611050617721,263.1590580223553, 0.0));
    positionAlloc4->Add (Vector(72.33196775711448,53.40982811980582, 0.0));
    positionAlloc4->Add (Vector(107.05095788481495,373.85700132539006, 0.0));
    positionAlloc4->Add (Vector(121.22187279497409,434.99381317133924, 0.0));
    positionAlloc4->Add (Vector(346.6600778954821,316.53313394785704, 0.0));
    positionAlloc4->Add (Vector(481.526043431204,164.09962380731713, 0.0));
    positionAlloc4->Add (Vector(432.81224377871365,444.3500247651311, 0.0));
    positionAlloc4->Add (Vector(181.71544651309844,176.52184292246974, 0.0));
    positionAlloc4->Add (Vector(196.93906888507527,30.70607702908218, 0.0));
    positionAlloc4->Add (Vector(212.37475459154413,162.12761206354443, 0.0));
    positionAlloc4->Add (Vector(449.9015369112341,391.5657361314183, 0.0));
    positionAlloc4->Add (Vector(385.9011478874431,329.0414904928775, 0.0));
    positionAlloc4->Add (Vector(163.46780845312352,451.1723196022833, 0.0));
    positionAlloc4->Add (Vector(441.7960450128427,418.13360807570655, 0.0));
    positionAlloc4->Add (Vector(146.9222852891263,442.3684593197173, 0.0));
    positionAlloc4->Add (Vector(437.5385668998722,108.06897507329971, 0.0));
    positionAlloc4->Add (Vector(324.92971310474593,35.83673684545807, 0.0));
    positionAlloc4->Add (Vector(277.44098842234604,85.82815214204336, 0.0));
    positionAlloc4->Add (Vector(300.4040107745596,161.42673433828585, 0.0));
    positionAlloc4->Add (Vector(297.7278316445012,281.8817847509128, 0.0));
    positionAlloc4->Add (Vector(211.1138114241493,15.399405582336655, 0.0));
    positionAlloc4->Add (Vector(410.99432215753535,314.4622191704063, 0.0));
    positionAlloc4->Add (Vector(348.6732697944946,356.6299487883682, 0.0));
    positionAlloc4->Add (Vector(190.52218741392545,426.2016438287771, 0.0));
    positionAlloc4->Add (Vector(356.84404566025074,392.3774682878346, 0.0));
    positionAlloc4->Add (Vector(119.52308217288521,348.3271002201152, 0.0));
    positionAlloc4->Add (Vector(57.40528389263333,99.95393153068794, 0.0));
    positionAlloc4->Add (Vector(191.53235610878326,490.5083959754935, 0.0));
    positionAlloc4->Add (Vector(323.42862137584245,143.86756727747903, 0.0));
    positionAlloc4->Add (Vector(239.82904289635383,202.17094519472872, 0.0));
    positionAlloc4->Add (Vector(361.6595678129735,51.293262613567705, 0.0));
    positionAlloc4->Add (Vector(450.99729727466166,325.87018337436194, 0.0));
    positionAlloc4->Add (Vector(144.88456292594637,240.82594782708182, 0.0));
    positionAlloc4->Add (Vector(38.738782001480956,264.0335696278041, 0.0));
    positionAlloc4->Add (Vector(335.81307430116675,132.55484698673138, 0.0));
    positionAlloc4->Add (Vector(347.7557877025545,184.86988977075762, 0.0));
    positionAlloc4->Add (Vector(181.58129747656494,483.86370455403045, 0.0));
    positionAlloc4->Add (Vector(263.70876020082613,28.174695867237254, 0.0));
    positionAlloc4->Add (Vector(283.98227290727897,331.52191158385983, 0.0));
    positionAlloc4->Add (Vector(110.44416817070291,102.75493054682605, 0.0));
    positionAlloc4->Add (Vector(111.33297718581925,69.00748102160931, 0.0));
    positionAlloc4->Add (Vector(437.3007993576466,376.62574258287094, 0.0));
    positionAlloc4->Add (Vector(331.08705171758555,477.8643874634413, 0.0));
    positionAlloc4->Add (Vector(377.62412473669843,342.0419106673084, 0.0));
    positionAlloc4->Add (Vector(275.56836527570727,26.856769204958386, 0.0));
    positionAlloc4->Add (Vector(115.6178609707365,461.1827095756318, 0.0));
    positionAlloc4->Add (Vector(18.36988226524372,157.31548907438915, 0.0));
    positionAlloc4->Add (Vector(317.90110931158523,492.67216904928665, 0.0));
    positionAlloc4->Add (Vector(468.9670393723165,187.44717028055808, 0.0));
    positionAlloc4->Add (Vector(182.75941156788667,165.36445551257233, 0.0));
    positionAlloc4->Add (Vector(466.74022916714625,404.1634834273782, 0.0));
    positionAlloc4->Add (Vector(246.32022039645784,41.3541490159447, 0.0));
    positionAlloc4->Add (Vector(309.74506263058316,429.8815333459976, 0.0));
    positionAlloc4->Add (Vector(210.2010069651903,161.84470682938195, 0.0));
    positionAlloc4->Add (Vector(153.72694109190564,12.0868361074164, 0.0));
    positionAlloc4->Add (Vector(268.9938178088678,147.14410862165644, 0.0));
    positionAlloc4->Add (Vector(103.55504795368608,259.4918321645226, 0.0));
    positionAlloc4->Add (Vector(28.21821517922046,226.90209255765086, 0.0));
    positionAlloc4->Add (Vector(430.0438859551421,154.17249302593305, 0.0));
    positionAlloc4->Add (Vector(481.61776133203693,288.3173173251373, 0.0));
    positionAlloc4->Add (Vector(116.22904892266828,91.24273820585654, 0.0));
    positionAlloc4->Add (Vector(464.3040606995656,162.88421035215885, 0.0));
    positionAlloc4->Add (Vector(63.976585946397655,381.1193860304008, 0.0));
    positionAlloc4->Add (Vector(464.4313996606396,131.59005181827226, 0.0));
    positionAlloc4->Add (Vector(33.75702520668483,218.15602429962445, 0.0));
    positionAlloc4->Add (Vector(38.53211574871324,420.45407906261755, 0.0));
    positionAlloc4->Add (Vector(481.545786391468,366.8211429752528, 0.0));
    positionAlloc4->Add (Vector(80.24579513210406,370.70520848997575, 0.0));
    positionAlloc4->Add (Vector(288.1073949129537,171.524347595776, 0.0));
    positionAlloc4->Add (Vector(71.24792349227282,216.67520127977247, 0.0));
    positionAlloc4->Add (Vector(244.65464530213234,71.14375109866222, 0.0));
    positionAlloc4->Add (Vector(252.97030478769068,20.767591489286698, 0.0));
    positionAlloc4->Add (Vector(352.68441495451384,223.3488951581946, 0.0));
    positionAlloc4->Add (Vector(372.3062047307332,250.8186010026998, 0.0));
    positionAlloc4->Add (Vector(351.176310418081,115.32045296148374, 0.0));
    positionAlloc4->Add (Vector(370.5584268622643,250.51248220704093, 0.0));
    positionAlloc4->Add (Vector(336.27840516014754,230.24179499236908, 0.0));
    positionAlloc4->Add (Vector(18.81347263426131,291.5169606152225, 0.0));
    positionAlloc4->Add (Vector(27.971482637891288,175.8352779350284, 0.0));
    positionAlloc4->Add (Vector(130.25675962871213,54.152176356540124, 0.0));
    positionAlloc4->Add (Vector(190.93444479532445,456.17673462637947, 0.0));
    positionAlloc4->Add (Vector(100.2581531822534,50.83264682674443, 0.0));
    positionAlloc4->Add (Vector(281.3610143678235,413.2228830114145, 0.0));
    positionAlloc4->Add (Vector(83.9080029920417,149.29473063827297, 0.0));
    positionAlloc4->Add (Vector(435.41859641346923,367.37040965076085, 0.0));
    positionAlloc4->Add (Vector(35.27425836093823,121.48205849464856, 0.0));
    positionAlloc4->Add (Vector(343.4684491109094,320.37138496842897, 0.0));
    positionAlloc4->Add (Vector(110.33406104358684,343.0698509893724, 0.0));
    positionAlloc4->Add (Vector(496.68514448124046,328.1004171710531, 0.0));
    positionAlloc4->Add (Vector(0.19919002737589508,115.65945947786626, 0.0));
    positionAlloc4->Add (Vector(396.4439652237468,130.8755219722338, 0.0));
    positionAlloc4->Add (Vector(264.19234836143977,123.56541280006422, 0.0));
    positionAlloc4->Add (Vector(186.63893893241413,406.0934878744923, 0.0));
    positionAlloc4->Add (Vector(408.669182739752,21.064080314053157, 0.0));
    positionAlloc4->Add (Vector(111.47570618772679,498.5558274293981, 0.0));
    positionAlloc4->Add (Vector(421.92586568096345,184.78367755865753, 0.0));
    positionAlloc4->Add (Vector(452.66401803868337,437.28325744268824, 0.0));
    positionAlloc4->Add (Vector(286.4121489605492,43.404192133739805, 0.0));
    positionAlloc4->Add (Vector(54.799303454578286,360.10387338762223, 0.0));
    positionAlloc4->Add (Vector(54.53247842367287,425.25174768370465, 0.0));
    positionAlloc4->Add (Vector(443.79439607169036,390.1298049782095, 0.0));
    positionAlloc4->Add (Vector(182.4538492117268,142.36701239532124, 0.0));
    positionAlloc4->Add (Vector(105.73367217842645,38.02384528738079, 0.0));
    positionAlloc4->Add (Vector(82.81983823961592,110.28775675772206, 0.0));
    positionAlloc4->Add (Vector(495.29229628205246,142.09753796029278, 0.0));
    positionAlloc4->Add (Vector(198.82604492355006,242.60632686505912, 0.0));
    positionAlloc4->Add (Vector(204.30424808856245,88.45211118021373, 0.0));
    positionAlloc4->Add (Vector(405.64544585200167,306.1794712300576, 0.0));
    positionAlloc4->Add (Vector(157.96145229097118,246.23399277355952, 0.0));
    positionAlloc4->Add (Vector(101.18367323697241,400.14094719806207, 0.0));
    positionAlloc4->Add (Vector(185.64621201235326,146.35568759759337, 0.0));
    positionAlloc4->Add (Vector(252.83823162930906,270.2083252480449, 0.0));
    positionAlloc4->Add (Vector(4.78344718925533,125.97571157272009, 0.0));
    positionAlloc4->Add (Vector(468.26371133847186,0.04847534383739527, 0.0));
    positionAlloc4->Add (Vector(279.6048580559135,246.36041490411392, 0.0));
    positionAlloc4->Add (Vector(381.38623186039,413.9076154609861, 0.0));
    positionAlloc4->Add (Vector(379.8864777740013,37.31760756143054, 0.0));
    positionAlloc4->Add (Vector(336.6888650972706,91.50058989347687, 0.0));
    positionAlloc4->Add (Vector(388.30681699104196,490.60508439556827, 0.0));
    positionAlloc4->Add (Vector(65.0429863490522,249.47905518251906, 0.0));
    positionAlloc4->Add (Vector(245.13497779485323,436.9167529433121, 0.0));
    positionAlloc4->Add (Vector(115.28923596163455,403.84224622582315, 0.0));
    positionAlloc4->Add (Vector(103.0890818671158,176.10813691597536, 0.0));
    positionAlloc4->Add (Vector(79.82425598647397,414.28782761518005, 0.0));
    positionAlloc4->Add (Vector(91.67936306403485,356.3708918952641, 0.0));
    positionAlloc4->Add (Vector(2.393209052942269,306.4443096646734, 0.0));
    positionAlloc4->Add (Vector(6.765254052609471,220.43951916862775, 0.0));
    positionAlloc4->Add (Vector(250.0061992343946,494.5701480703867, 0.0));
    positionAlloc4->Add (Vector(127.46179465427149,21.045879921186394, 0.0));
    positionAlloc4->Add (Vector(365.2443343268177,11.763957438914064, 0.0));
    positionAlloc4->Add (Vector(395.06675121726954,300.7996679559771, 0.0));
    positionAlloc4->Add (Vector(140.56282318192913,295.29793087943904, 0.0));
    positionAlloc4->Add (Vector(394.1472600561627,23.256341133875814, 0.0));
    positionAlloc4->Add (Vector(239.47704588697044,144.90166959618605, 0.0));
    positionAlloc4->Add (Vector(341.8187323905251,199.00585760814866, 0.0));
    positionAlloc4->Add (Vector(165.16060173483078,44.02420890467601, 0.0));
    positionAlloc4->Add (Vector(336.9259384416281,19.721399267693783, 0.0));
    positionAlloc4->Add (Vector(293.96987260523053,228.76859849318603, 0.0));
    positionAlloc4->Add (Vector(19.193715988202186,439.6504217982923, 0.0));
    positionAlloc4->Add (Vector(440.2487067632656,396.8915662159198, 0.0));
    positionAlloc4->Add (Vector(132.68592511900636,82.25065146933153, 0.0));
    positionAlloc4->Add (Vector(454.7516767411526,356.68463150954915, 0.0));
    positionAlloc4->Add (Vector(118.11387596854999,154.44973525505833, 0.0));
    positionAlloc4->Add (Vector(489.9996082218179,32.946885002562766, 0.0));
    positionAlloc4->Add (Vector(183.8230288348185,104.13284701504405, 0.0));
    positionAlloc4->Add (Vector(358.2646882956965,286.27879884764064, 0.0));
    positionAlloc4->Add (Vector(156.41587398850066,368.7538993970642, 0.0));
    positionAlloc4->Add (Vector(167.44441489317342,496.43807837951823, 0.0));
    positionAlloc4->Add (Vector(58.44586354919623,10.240624359200979, 0.0));
    positionAlloc4->Add (Vector(474.9144571812092,377.816640289576, 0.0));
    positionAlloc4->Add (Vector(43.633320088016404,327.69482824123764, 0.0));
    positionAlloc4->Add (Vector(54.56416412070747,198.34055070137646, 0.0));
    positionAlloc4->Add (Vector(134.82162308051133,105.22889552033787, 0.0));
    positionAlloc4->Add (Vector(395.4877980234747,357.8742741956168, 0.0));
    positionAlloc4->Add (Vector(363.75194332930386,403.5955510120045, 0.0));
    positionAlloc4->Add (Vector(232.36593861551137,117.17822170074366, 0.0));
    positionAlloc4->Add (Vector(130.4504018474412,167.07591675903737, 0.0));
    positionAlloc4->Add (Vector(137.17057670409977,54.21541691593729, 0.0));
    positionAlloc4->Add (Vector(98.07539597167137,336.962996778971, 0.0));
    positionAlloc4->Add (Vector(73.29592281657227,478.18236382342326, 0.0));
    positionAlloc4->Add (Vector(267.6500898144187,327.270651909969, 0.0));
    positionAlloc4->Add (Vector(272.6267570535151,31.73777493907143, 0.0));
    positionAlloc4->Add (Vector(467.2677525895175,281.7545837375854, 0.0));
    positionAlloc4->Add (Vector(54.18944395207609,233.10698790654078, 0.0));
    positionAlloc4->Add (Vector(351.05565245705446,441.61641082591734, 0.0));
    positionAlloc4->Add (Vector(203.07145134040493,80.29341425462138, 0.0));
    positionAlloc4->Add (Vector(409.83286894466426,290.6599873133597, 0.0));
    positionAlloc4->Add (Vector(248.3830306066675,288.6612656621645, 0.0));
    positionAlloc4->Add (Vector(393.6135681888035,209.23213195571245, 0.0));
    positionAlloc4->Add (Vector(200.14715336788785,153.95296671545262, 0.0));
    positionAlloc4->Add (Vector(240.73425768686653,392.3654799933646, 0.0));
    positionAlloc4->Add (Vector(247.16446527200654,418.60299797470987, 0.0));
    positionAlloc4->Add (Vector(74.34243305661519,279.8432495008865, 0.0));
    positionAlloc4->Add (Vector(118.3336883825512,343.80748108040746, 0.0));
    positionAlloc4->Add (Vector(229.92868399961813,298.804324438435, 0.0));
    positionAlloc4->Add (Vector(145.15006938770458,399.5037490983159, 0.0));
    positionAlloc4->Add (Vector(133.63630675994443,445.02865188704567, 0.0));
    positionAlloc4->Add (Vector(414.5037454742982,492.0799229493923, 0.0));
    positionAlloc4->Add (Vector(467.52286345660434,27.598124327252783, 0.0));
    positionAlloc4->Add (Vector(194.48956961215418,248.97297694371306, 0.0));
    positionAlloc4->Add (Vector(355.68534415051244,83.91058181179717, 0.0));
    positionAlloc4->Add (Vector(73.43790786128568,207.78586035831793, 0.0));
    positionAlloc4->Add (Vector(325.08149315784226,44.9975515163486, 0.0));
    positionAlloc4->Add (Vector(355.9697011396798,278.7719570387363, 0.0));
    positionAlloc4->Add (Vector(91.67345338446165,380.21488215321926, 0.0));
    positionAlloc4->Add (Vector(340.44026356166836,422.62432325191185, 0.0));
    positionAlloc4->Add (Vector(148.6207226508382,317.53996627252445, 0.0));
    positionAlloc4->Add (Vector(74.36678140128045,152.97272354308024, 0.0));
    positionAlloc4->Add (Vector(158.05356612909904,334.8083823429381, 0.0));
    positionAlloc4->Add (Vector(100.83936740149151,408.87859684656223, 0.0));
    positionAlloc4->Add (Vector(107.00488635383016,302.28580165318687, 0.0));
    positionAlloc4->Add (Vector(66.55482243795157,341.80384140224857, 0.0));
    positionAlloc4->Add (Vector(61.04840978172077,91.14448537499243, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_4.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_ECC_flowmon/switch_ECC_4.flowmon", false, false);
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