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
	  uint16_t numberOfRrhs = 40;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 448;
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
    positionAlloc->Add (Vector (500.0,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (500.0,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (500.0,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,0.0, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,0.0, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (0.0,388.8888888888889, 0.0));
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
    positionAlloc4->Add (Vector(359.53069752826195,455.0893537862722, 0.0));
    positionAlloc4->Add (Vector(139.1824893112215,129.74615425392406, 0.0));
    positionAlloc4->Add (Vector(194.9276954155545,287.7135285559372, 0.0));
    positionAlloc4->Add (Vector(107.6048300816635,185.96150570213644, 0.0));
    positionAlloc4->Add (Vector(134.78350917347714,176.02170134757844, 0.0));
    positionAlloc4->Add (Vector(211.0632839079168,456.7848433307625, 0.0));
    positionAlloc4->Add (Vector(189.56130138768262,219.04071871392728, 0.0));
    positionAlloc4->Add (Vector(134.61176675085724,86.52835959109223, 0.0));
    positionAlloc4->Add (Vector(235.75996917429137,358.8344903885235, 0.0));
    positionAlloc4->Add (Vector(190.594306873269,316.3562698093768, 0.0));
    positionAlloc4->Add (Vector(46.8683380373211,158.99051583935636, 0.0));
    positionAlloc4->Add (Vector(85.8053471234621,147.41750246043705, 0.0));
    positionAlloc4->Add (Vector(366.2505575542269,464.37346536387804, 0.0));
    positionAlloc4->Add (Vector(206.9889581346837,439.5710928321179, 0.0));
    positionAlloc4->Add (Vector(499.15837514223483,407.25280397117535, 0.0));
    positionAlloc4->Add (Vector(303.68826947443745,143.73643710116525, 0.0));
    positionAlloc4->Add (Vector(388.5095746802088,147.33242449943472, 0.0));
    positionAlloc4->Add (Vector(296.1436928829569,359.8978546869598, 0.0));
    positionAlloc4->Add (Vector(407.8852922746642,139.008165834391, 0.0));
    positionAlloc4->Add (Vector(443.9900449700879,47.88801464990644, 0.0));
    positionAlloc4->Add (Vector(282.58674085095595,353.7799739161116, 0.0));
    positionAlloc4->Add (Vector(426.5427561949449,58.688802050473996, 0.0));
    positionAlloc4->Add (Vector(442.1920583879358,465.4341481272791, 0.0));
    positionAlloc4->Add (Vector(386.86399170114686,39.26145741705178, 0.0));
    positionAlloc4->Add (Vector(21.645080436953734,180.74379232086895, 0.0));
    positionAlloc4->Add (Vector(353.13162227565806,330.6730671907848, 0.0));
    positionAlloc4->Add (Vector(345.9586146383924,134.0037909286602, 0.0));
    positionAlloc4->Add (Vector(122.97116760806954,490.81086431150834, 0.0));
    positionAlloc4->Add (Vector(24.921279067260492,40.21571698477355, 0.0));
    positionAlloc4->Add (Vector(354.35525397250666,244.16705906022435, 0.0));
    positionAlloc4->Add (Vector(454.9948108412241,221.79601931016686, 0.0));
    positionAlloc4->Add (Vector(407.0679073619101,406.14905768422375, 0.0));
    positionAlloc4->Add (Vector(79.68010311459828,222.65680130768263, 0.0));
    positionAlloc4->Add (Vector(49.26950532027652,67.79852369617656, 0.0));
    positionAlloc4->Add (Vector(135.90152624831063,1.0001503042158522, 0.0));
    positionAlloc4->Add (Vector(380.79615593950257,142.41668605915336, 0.0));
    positionAlloc4->Add (Vector(54.053692794255205,35.56248976141912, 0.0));
    positionAlloc4->Add (Vector(439.1600998404724,415.3512846202839, 0.0));
    positionAlloc4->Add (Vector(117.74888064995682,336.783348988016, 0.0));
    positionAlloc4->Add (Vector(131.62352945009476,0.3932707648182099, 0.0));
    positionAlloc4->Add (Vector(345.7442117769849,266.0068750386908, 0.0));
    positionAlloc4->Add (Vector(311.1083181569669,103.23997959381582, 0.0));
    positionAlloc4->Add (Vector(307.6493607330412,422.19534132167934, 0.0));
    positionAlloc4->Add (Vector(388.04056868344884,285.6269885212087, 0.0));
    positionAlloc4->Add (Vector(418.15113869270203,440.8454596769712, 0.0));
    positionAlloc4->Add (Vector(417.11799474817457,272.2090805016188, 0.0));
    positionAlloc4->Add (Vector(53.262328995296535,468.3085115307435, 0.0));
    positionAlloc4->Add (Vector(482.77128979616936,261.49403750686434, 0.0));
    positionAlloc4->Add (Vector(419.97688586101646,382.92549614496966, 0.0));
    positionAlloc4->Add (Vector(281.8531276432279,119.29279797129189, 0.0));
    positionAlloc4->Add (Vector(49.17318947421012,180.26315249823043, 0.0));
    positionAlloc4->Add (Vector(222.88726313736262,375.4512798322833, 0.0));
    positionAlloc4->Add (Vector(406.76328974250015,188.14709182844908, 0.0));
    positionAlloc4->Add (Vector(55.91211987580391,265.52367729361106, 0.0));
    positionAlloc4->Add (Vector(401.50001987325044,498.0707186816349, 0.0));
    positionAlloc4->Add (Vector(484.10071912234974,143.56168876439767, 0.0));
    positionAlloc4->Add (Vector(187.32498963676036,406.91999619729773, 0.0));
    positionAlloc4->Add (Vector(489.86981045093717,249.63850548166906, 0.0));
    positionAlloc4->Add (Vector(61.27668245568157,263.3046678531537, 0.0));
    positionAlloc4->Add (Vector(1.572860232186224,219.12651626049308, 0.0));
    positionAlloc4->Add (Vector(334.7852242701181,343.6717369584236, 0.0));
    positionAlloc4->Add (Vector(46.25167907530198,322.94169370882116, 0.0));
    positionAlloc4->Add (Vector(154.0047354104448,119.15445420341136, 0.0));
    positionAlloc4->Add (Vector(231.83685560635053,202.42423576136136, 0.0));
    positionAlloc4->Add (Vector(292.7928970495103,360.6921383236148, 0.0));
    positionAlloc4->Add (Vector(122.9899788697174,277.44619053792263, 0.0));
    positionAlloc4->Add (Vector(396.1564358853739,359.5826488766774, 0.0));
    positionAlloc4->Add (Vector(7.954980796281264,124.44907099128987, 0.0));
    positionAlloc4->Add (Vector(382.84305938537693,489.3469094875227, 0.0));
    positionAlloc4->Add (Vector(111.43194612678042,97.19751025236145, 0.0));
    positionAlloc4->Add (Vector(381.2205910797897,298.8098934949881, 0.0));
    positionAlloc4->Add (Vector(148.08835920153763,341.3764330822216, 0.0));
    positionAlloc4->Add (Vector(159.95753102192728,143.12389641309258, 0.0));
    positionAlloc4->Add (Vector(281.23308233800657,206.1963041101485, 0.0));
    positionAlloc4->Add (Vector(392.146001174813,327.39959378020893, 0.0));
    positionAlloc4->Add (Vector(291.10752106965356,364.4853346880556, 0.0));
    positionAlloc4->Add (Vector(213.74604728026526,200.53415271724805, 0.0));
    positionAlloc4->Add (Vector(409.9847111225419,233.90902390768588, 0.0));
    positionAlloc4->Add (Vector(439.9677881109618,410.6860207716442, 0.0));
    positionAlloc4->Add (Vector(46.807124231995125,32.7992580675216, 0.0));
    positionAlloc4->Add (Vector(231.16770206755788,480.3283476434982, 0.0));
    positionAlloc4->Add (Vector(67.67719543103662,15.31934904543486, 0.0));
    positionAlloc4->Add (Vector(12.140712662339293,409.0873726655827, 0.0));
    positionAlloc4->Add (Vector(438.34290329828696,280.7432407663397, 0.0));
    positionAlloc4->Add (Vector(268.88526411565124,3.367353497101655, 0.0));
    positionAlloc4->Add (Vector(253.4651936641633,174.2698076528489, 0.0));
    positionAlloc4->Add (Vector(407.9533364189112,298.50809389228607, 0.0));
    positionAlloc4->Add (Vector(498.88118278441016,262.7988941089097, 0.0));
    positionAlloc4->Add (Vector(129.91698787024387,166.89872841185831, 0.0));
    positionAlloc4->Add (Vector(458.3250560404663,280.5494029363183, 0.0));
    positionAlloc4->Add (Vector(407.25008538830224,461.1612404264482, 0.0));
    positionAlloc4->Add (Vector(106.37951803407103,325.682680998934, 0.0));
    positionAlloc4->Add (Vector(312.6454679279234,300.4827976990449, 0.0));
    positionAlloc4->Add (Vector(453.7366013674,391.29048902229925, 0.0));
    positionAlloc4->Add (Vector(318.41798767327504,346.2087516893014, 0.0));
    positionAlloc4->Add (Vector(102.3675432461285,316.0961876459129, 0.0));
    positionAlloc4->Add (Vector(440.3418089535075,221.55570016339172, 0.0));
    positionAlloc4->Add (Vector(282.72779829527093,449.7654579430355, 0.0));
    positionAlloc4->Add (Vector(164.9866144027597,322.0978462691082, 0.0));
    positionAlloc4->Add (Vector(78.37580500171565,377.25850380119317, 0.0));
    positionAlloc4->Add (Vector(253.29109375346349,97.98350727126076, 0.0));
    positionAlloc4->Add (Vector(279.14328922382595,197.46006705361697, 0.0));
    positionAlloc4->Add (Vector(287.729670561904,119.32510146446495, 0.0));
    positionAlloc4->Add (Vector(156.9921640234732,240.81246536037088, 0.0));
    positionAlloc4->Add (Vector(207.82050276021081,11.220877179339894, 0.0));
    positionAlloc4->Add (Vector(299.8571828079965,4.759858108279003, 0.0));
    positionAlloc4->Add (Vector(315.7968825654553,481.83135510653494, 0.0));
    positionAlloc4->Add (Vector(139.40837771442426,441.301001251535, 0.0));
    positionAlloc4->Add (Vector(258.15802283977456,77.78894899603705, 0.0));
    positionAlloc4->Add (Vector(406.0577839790731,51.57339129173211, 0.0));
    positionAlloc4->Add (Vector(70.42899844968342,48.17927593062166, 0.0));
    positionAlloc4->Add (Vector(293.5490674415249,27.576042509411923, 0.0));
    positionAlloc4->Add (Vector(469.7409621042272,188.323856873165, 0.0));
    positionAlloc4->Add (Vector(94.10970780314393,86.62213575408644, 0.0));
    positionAlloc4->Add (Vector(160.17714507530613,275.8689853108538, 0.0));
    positionAlloc4->Add (Vector(134.83513319517414,177.4096743608244, 0.0));
    positionAlloc4->Add (Vector(233.2717456868703,439.6157848722156, 0.0));
    positionAlloc4->Add (Vector(165.5072170328974,329.85490191939977, 0.0));
    positionAlloc4->Add (Vector(183.41847107919133,48.566340801642625, 0.0));
    positionAlloc4->Add (Vector(110.26489737186185,183.59616295163278, 0.0));
    positionAlloc4->Add (Vector(247.7487173735567,436.3463614924361, 0.0));
    positionAlloc4->Add (Vector(176.0776800482659,455.71757016189616, 0.0));
    positionAlloc4->Add (Vector(50.730428912002026,393.1346611742968, 0.0));
    positionAlloc4->Add (Vector(479.2871055098354,363.58271746346463, 0.0));
    positionAlloc4->Add (Vector(152.63073578290658,35.67823747230808, 0.0));
    positionAlloc4->Add (Vector(151.41425961632032,44.38385303957115, 0.0));
    positionAlloc4->Add (Vector(117.78311825121018,309.0909209202228, 0.0));
    positionAlloc4->Add (Vector(307.7360706875514,406.9347814047425, 0.0));
    positionAlloc4->Add (Vector(262.88059363603463,325.1448846015027, 0.0));
    positionAlloc4->Add (Vector(172.09817946513405,35.17393984260392, 0.0));
    positionAlloc4->Add (Vector(319.2389777007737,61.10618911849164, 0.0));
    positionAlloc4->Add (Vector(55.97772217079056,255.63644681713095, 0.0));
    positionAlloc4->Add (Vector(77.00658916897557,188.90677373222087, 0.0));
    positionAlloc4->Add (Vector(96.03783650227865,113.12579429353275, 0.0));
    positionAlloc4->Add (Vector(280.20059054239096,298.88284268841966, 0.0));
    positionAlloc4->Add (Vector(493.20128781870847,423.12086529389086, 0.0));
    positionAlloc4->Add (Vector(357.1477569394392,147.6569830509496, 0.0));
    positionAlloc4->Add (Vector(398.60990367936915,6.772173419563821, 0.0));
    positionAlloc4->Add (Vector(273.25574968211396,329.8319777124605, 0.0));
    positionAlloc4->Add (Vector(239.1859155358493,176.37121875053703, 0.0));
    positionAlloc4->Add (Vector(113.9129434452988,15.371409174848527, 0.0));
    positionAlloc4->Add (Vector(494.63234236790015,390.22846778717064, 0.0));
    positionAlloc4->Add (Vector(337.43529208225004,35.96348240460723, 0.0));
    positionAlloc4->Add (Vector(116.42704862199798,377.40392555023067, 0.0));
    positionAlloc4->Add (Vector(61.34052092950315,266.93094215170834, 0.0));
    positionAlloc4->Add (Vector(210.46154225743996,193.07525593529678, 0.0));
    positionAlloc4->Add (Vector(323.01512853578635,302.5495670777563, 0.0));
    positionAlloc4->Add (Vector(151.7362989807625,209.66614908344312, 0.0));
    positionAlloc4->Add (Vector(55.588475959903015,432.68205289118987, 0.0));
    positionAlloc4->Add (Vector(438.44184990432575,159.8250559310263, 0.0));
    positionAlloc4->Add (Vector(493.7594543522208,357.91598453629683, 0.0));
    positionAlloc4->Add (Vector(440.9167403758983,436.8346846784692, 0.0));
    positionAlloc4->Add (Vector(292.13470261379035,167.85755387780154, 0.0));
    positionAlloc4->Add (Vector(1.3679030227001787,31.680255687017656, 0.0));
    positionAlloc4->Add (Vector(486.62627588353405,368.79425495834056, 0.0));
    positionAlloc4->Add (Vector(11.951889683109673,404.28207523719425, 0.0));
    positionAlloc4->Add (Vector(494.53784275397345,159.22855990684127, 0.0));
    positionAlloc4->Add (Vector(344.27759987907405,13.1739394349914, 0.0));
    positionAlloc4->Add (Vector(475.8878891285469,281.39027484471245, 0.0));
    positionAlloc4->Add (Vector(64.36580229827382,385.96357303652417, 0.0));
    positionAlloc4->Add (Vector(482.5939958965214,326.3576842128477, 0.0));
    positionAlloc4->Add (Vector(492.289889480353,94.7090110218769, 0.0));
    positionAlloc4->Add (Vector(137.7966072051584,308.1303616649302, 0.0));
    positionAlloc4->Add (Vector(58.72096811379479,441.9555329681746, 0.0));
    positionAlloc4->Add (Vector(258.7563460713097,206.7990805645535, 0.0));
    positionAlloc4->Add (Vector(218.9709623885619,396.9022535582126, 0.0));
    positionAlloc4->Add (Vector(383.11240345895953,354.3821279656307, 0.0));
    positionAlloc4->Add (Vector(53.357044177768934,91.98096987633458, 0.0));
    positionAlloc4->Add (Vector(479.7981012654596,119.40706664819601, 0.0));
    positionAlloc4->Add (Vector(108.80265177956156,124.74102554931143, 0.0));
    positionAlloc4->Add (Vector(478.00979181546694,282.3050495343559, 0.0));
    positionAlloc4->Add (Vector(159.32343239019124,115.63214744271878, 0.0));
    positionAlloc4->Add (Vector(414.10012935372436,287.03737759827544, 0.0));
    positionAlloc4->Add (Vector(193.68415349940477,460.4565019023761, 0.0));
    positionAlloc4->Add (Vector(148.59389709836378,8.312053266452956, 0.0));
    positionAlloc4->Add (Vector(88.16902038493285,209.4175715262464, 0.0));
    positionAlloc4->Add (Vector(292.9141922139605,190.03560263939562, 0.0));
    positionAlloc4->Add (Vector(407.49732985299244,142.69176622476547, 0.0));
    positionAlloc4->Add (Vector(54.77085860813424,489.5044687870951, 0.0));
    positionAlloc4->Add (Vector(332.5493891679103,112.43748998368287, 0.0));
    positionAlloc4->Add (Vector(383.9934897414029,247.38606691676364, 0.0));
    positionAlloc4->Add (Vector(487.9349687814568,232.13853377212163, 0.0));
    positionAlloc4->Add (Vector(246.24032695655518,438.90475073997504, 0.0));
    positionAlloc4->Add (Vector(278.9144943810295,356.14049980125105, 0.0));
    positionAlloc4->Add (Vector(5.736227511453029,83.26882180577006, 0.0));
    positionAlloc4->Add (Vector(128.6073742267277,408.2263351637387, 0.0));
    positionAlloc4->Add (Vector(136.50441865595886,347.92422817972295, 0.0));
    positionAlloc4->Add (Vector(351.0347321144242,309.4652115487445, 0.0));
    positionAlloc4->Add (Vector(129.36085375330364,460.81845932807005, 0.0));
    positionAlloc4->Add (Vector(250.49200619697027,364.8116892840901, 0.0));
    positionAlloc4->Add (Vector(422.3806814218175,208.30957428605916, 0.0));
    positionAlloc4->Add (Vector(425.7343834385192,138.46152823449637, 0.0));
    positionAlloc4->Add (Vector(186.80485946320303,301.7568083356595, 0.0));
    positionAlloc4->Add (Vector(257.07913387809384,249.08341249725612, 0.0));
    positionAlloc4->Add (Vector(52.70987321965859,350.89367325595794, 0.0));
    positionAlloc4->Add (Vector(419.4724739866243,31.41199010863471, 0.0));
    positionAlloc4->Add (Vector(2.4385222411132945,162.6656648844802, 0.0));
    positionAlloc4->Add (Vector(336.751507342843,202.91020299952257, 0.0));
    positionAlloc4->Add (Vector(77.88866280775642,287.1416771754416, 0.0));
    positionAlloc4->Add (Vector(467.71350617070436,419.4224170990914, 0.0));
    positionAlloc4->Add (Vector(460.3377988795611,214.16760687986124, 0.0));
    positionAlloc4->Add (Vector(496.6183922327728,127.87551347594983, 0.0));
    positionAlloc4->Add (Vector(181.8422039263924,141.2187366175225, 0.0));
    positionAlloc4->Add (Vector(421.089559433505,212.3063550473014, 0.0));
    positionAlloc4->Add (Vector(152.2426220261654,431.24319985215044, 0.0));
    positionAlloc4->Add (Vector(469.27532714301265,485.3750740265808, 0.0));
    positionAlloc4->Add (Vector(441.9087949987559,90.11676261611102, 0.0));
    positionAlloc4->Add (Vector(364.6233072226976,157.82706589158042, 0.0));
    positionAlloc4->Add (Vector(245.77859239076972,102.19343314893243, 0.0));
    positionAlloc4->Add (Vector(1.3158980627516192,388.1028937301905, 0.0));
    positionAlloc4->Add (Vector(48.409205346605034,353.4747240193171, 0.0));
    positionAlloc4->Add (Vector(78.28621229918859,359.38752642432496, 0.0));
    positionAlloc4->Add (Vector(342.4619984524034,370.3909065097925, 0.0));
    positionAlloc4->Add (Vector(426.4849249248464,389.16310934010096, 0.0));
    positionAlloc4->Add (Vector(333.4039975693808,5.551666684961432, 0.0));
    positionAlloc4->Add (Vector(476.0455428564199,334.88740316362, 0.0));
    positionAlloc4->Add (Vector(323.0453607479696,136.0465071466372, 0.0));
    positionAlloc4->Add (Vector(480.3986530103224,278.29582955105093, 0.0));
    positionAlloc4->Add (Vector(95.67943876786,488.7277821965081, 0.0));
    positionAlloc4->Add (Vector(396.7423370835167,17.939574483008368, 0.0));
    positionAlloc4->Add (Vector(84.23848582322357,264.5436052276252, 0.0));
    positionAlloc4->Add (Vector(107.07346239037064,219.76955107769558, 0.0));
    positionAlloc4->Add (Vector(483.8044487562731,418.0881115676792, 0.0));
    positionAlloc4->Add (Vector(270.5604430544767,226.7176572348658, 0.0));
    positionAlloc4->Add (Vector(486.0103108951587,107.19895208790703, 0.0));
    positionAlloc4->Add (Vector(490.55428153181344,218.2813858405716, 0.0));
    positionAlloc4->Add (Vector(244.90389235504412,497.7886059231981, 0.0));
    positionAlloc4->Add (Vector(153.79429458740123,248.7241392923216, 0.0));
    positionAlloc4->Add (Vector(124.87987714335192,56.07524161054839, 0.0));
    positionAlloc4->Add (Vector(305.3162584338229,106.45729525758179, 0.0));
    positionAlloc4->Add (Vector(145.94761051373288,222.9764073744639, 0.0));
    positionAlloc4->Add (Vector(157.46451119630177,183.42965929559185, 0.0));
    positionAlloc4->Add (Vector(286.8219287852096,70.63966632050084, 0.0));
    positionAlloc4->Add (Vector(494.4109423586498,388.7397156049705, 0.0));
    positionAlloc4->Add (Vector(153.57105463543064,438.3885199689806, 0.0));
    positionAlloc4->Add (Vector(319.4739101842158,282.81573448836696, 0.0));
    positionAlloc4->Add (Vector(326.3464571672615,420.9490039133689, 0.0));
    positionAlloc4->Add (Vector(312.08986163195544,132.4183275880979, 0.0));
    positionAlloc4->Add (Vector(264.8286602850213,143.49187637353728, 0.0));
    positionAlloc4->Add (Vector(273.78727992728636,377.33483723743234, 0.0));
    positionAlloc4->Add (Vector(41.39796929417372,152.75533064301555, 0.0));
    positionAlloc4->Add (Vector(347.5760323638452,103.85902003149383, 0.0));
    positionAlloc4->Add (Vector(159.84538693636475,330.4195694780025, 0.0));
    positionAlloc4->Add (Vector(55.038253923508975,36.959443955636104, 0.0));
    positionAlloc4->Add (Vector(306.1611693423113,157.69808939007157, 0.0));
    positionAlloc4->Add (Vector(377.8714278981152,470.42370995052283, 0.0));
    positionAlloc4->Add (Vector(192.45456492699142,285.66057502936775, 0.0));
    positionAlloc4->Add (Vector(10.842702263050953,385.57616662688935, 0.0));
    positionAlloc4->Add (Vector(268.0120083442055,168.0627214686656, 0.0));
    positionAlloc4->Add (Vector(162.77014879949476,42.92465056014677, 0.0));
    positionAlloc4->Add (Vector(481.6207824568353,379.52700610816765, 0.0));
    positionAlloc4->Add (Vector(194.61937737048197,266.7017486330072, 0.0));
    positionAlloc4->Add (Vector(41.07200101596581,453.1675347901679, 0.0));
    positionAlloc4->Add (Vector(354.7946078390542,59.38885719924253, 0.0));
    positionAlloc4->Add (Vector(105.1475358763005,320.7123912768107, 0.0));
    positionAlloc4->Add (Vector(3.105133595258047,215.78112264020083, 0.0));
    positionAlloc4->Add (Vector(95.02475180834492,148.03164470911628, 0.0));
    positionAlloc4->Add (Vector(388.9103054501922,464.99006817317826, 0.0));
    positionAlloc4->Add (Vector(108.01267562580352,141.97171892312338, 0.0));
    positionAlloc4->Add (Vector(216.05354564985325,345.2170099271569, 0.0));
    positionAlloc4->Add (Vector(397.48371234646163,342.37835248928826, 0.0));
    positionAlloc4->Add (Vector(276.8759063051476,425.9881290324407, 0.0));
    positionAlloc4->Add (Vector(195.5758802675141,150.14921468573155, 0.0));
    positionAlloc4->Add (Vector(342.85828611286826,476.0312695272666, 0.0));
    positionAlloc4->Add (Vector(113.02208543764708,0.5574151262033311, 0.0));
    positionAlloc4->Add (Vector(18.888887903073247,207.28143984619746, 0.0));
    positionAlloc4->Add (Vector(347.3729961683308,156.610847573443, 0.0));
    positionAlloc4->Add (Vector(264.6279634338585,330.0709048117984, 0.0));
    positionAlloc4->Add (Vector(41.94767624968898,54.27935280435881, 0.0));
    positionAlloc4->Add (Vector(267.19991845227537,411.7757779983802, 0.0));
    positionAlloc4->Add (Vector(157.0127250254484,21.01238004305722, 0.0));
    positionAlloc4->Add (Vector(244.1044715441024,158.96794553755765, 0.0));
    positionAlloc4->Add (Vector(136.38411833239405,388.5352464711919, 0.0));
    positionAlloc4->Add (Vector(19.788955737684578,394.07010332184154, 0.0));
    positionAlloc4->Add (Vector(284.82677834203696,122.47542755741719, 0.0));
    positionAlloc4->Add (Vector(239.63570764179477,44.115075500047894, 0.0));
    positionAlloc4->Add (Vector(366.1353673781343,462.25507499205133, 0.0));
    positionAlloc4->Add (Vector(250.89655903158476,226.1881238889258, 0.0));
    positionAlloc4->Add (Vector(179.45557089442843,309.4850543700711, 0.0));
    positionAlloc4->Add (Vector(483.2057078261552,282.39311763331807, 0.0));
    positionAlloc4->Add (Vector(190.10330262273973,315.87220425069575, 0.0));
    positionAlloc4->Add (Vector(101.44945031142122,413.0157689227094, 0.0));
    positionAlloc4->Add (Vector(306.30802707477704,328.5825211817585, 0.0));
    positionAlloc4->Add (Vector(163.3959126654388,188.3784434600732, 0.0));
    positionAlloc4->Add (Vector(12.770042437895368,368.128628723267, 0.0));
    positionAlloc4->Add (Vector(325.9879103853709,119.25800639950651, 0.0));
    positionAlloc4->Add (Vector(333.66689249909876,296.96807169188475, 0.0));
    positionAlloc4->Add (Vector(249.48373724998,413.72204570553794, 0.0));
    positionAlloc4->Add (Vector(464.103609563588,112.19213302199226, 0.0));
    positionAlloc4->Add (Vector(399.4130445280411,405.49845971076337, 0.0));
    positionAlloc4->Add (Vector(434.38943477751,76.2483708806027, 0.0));
    positionAlloc4->Add (Vector(180.89788045358068,492.65840104266147, 0.0));
    positionAlloc4->Add (Vector(290.8436346844624,12.422930181865633, 0.0));
    positionAlloc4->Add (Vector(192.3580339626878,45.520434522991096, 0.0));
    positionAlloc4->Add (Vector(286.21540846353633,417.1898323952584, 0.0));
    positionAlloc4->Add (Vector(3.6699458768219007,286.5661023419709, 0.0));
    positionAlloc4->Add (Vector(25.145468014351856,481.491876543927, 0.0));
    positionAlloc4->Add (Vector(446.7133186751397,414.09010953656235, 0.0));
    positionAlloc4->Add (Vector(292.4412993852603,231.36909013466263, 0.0));
    positionAlloc4->Add (Vector(378.08494419386767,396.87837361244624, 0.0));
    positionAlloc4->Add (Vector(498.5693996834604,64.7979086350699, 0.0));
    positionAlloc4->Add (Vector(254.93698631421796,255.78994483236627, 0.0));
    positionAlloc4->Add (Vector(94.79585953638242,231.7105771952257, 0.0));
    positionAlloc4->Add (Vector(150.08591661802424,85.49367447150918, 0.0));
    positionAlloc4->Add (Vector(288.4801854663042,125.82848675007247, 0.0));
    positionAlloc4->Add (Vector(54.68382775342684,86.77132342125077, 0.0));
    positionAlloc4->Add (Vector(443.5605883286532,297.73308448477144, 0.0));
    positionAlloc4->Add (Vector(9.181318251286207,465.70273363242717, 0.0));
    positionAlloc4->Add (Vector(92.1285335792234,118.40426684711963, 0.0));
    positionAlloc4->Add (Vector(445.3620957467565,429.3575578634726, 0.0));
    positionAlloc4->Add (Vector(82.48613394947624,363.61772057163853, 0.0));
    positionAlloc4->Add (Vector(75.75703092606156,485.64165586629, 0.0));
    positionAlloc4->Add (Vector(287.7259172324912,153.89613916735527, 0.0));
    positionAlloc4->Add (Vector(142.8914666778811,4.853798287408284, 0.0));
    positionAlloc4->Add (Vector(128.72914190383727,325.84201587033425, 0.0));
    positionAlloc4->Add (Vector(443.8415652398624,17.615635326679758, 0.0));
    positionAlloc4->Add (Vector(85.91487980341356,5.629946746094738, 0.0));
    positionAlloc4->Add (Vector(256.5061468300089,341.99960705847883, 0.0));
    positionAlloc4->Add (Vector(465.49493267298595,66.8891538715457, 0.0));
    positionAlloc4->Add (Vector(43.33623632645506,297.30194405581295, 0.0));
    positionAlloc4->Add (Vector(474.1826450945113,81.47449929109224, 0.0));
    positionAlloc4->Add (Vector(44.479555480526734,163.3506289648008, 0.0));
    positionAlloc4->Add (Vector(366.72385003438296,363.10090489760046, 0.0));
    positionAlloc4->Add (Vector(130.52486570234328,16.540921703757817, 0.0));
    positionAlloc4->Add (Vector(118.92784362060993,379.0903117065637, 0.0));
    positionAlloc4->Add (Vector(95.16839735859456,441.2487674293046, 0.0));
    positionAlloc4->Add (Vector(120.413104785777,116.45773419191585, 0.0));
    positionAlloc4->Add (Vector(471.40399253764235,401.3662750787756, 0.0));
    positionAlloc4->Add (Vector(164.05515746568406,372.64593642900775, 0.0));
    positionAlloc4->Add (Vector(339.77794726657515,17.536809652539453, 0.0));
    positionAlloc4->Add (Vector(193.85256589070494,148.77446618861862, 0.0));
    positionAlloc4->Add (Vector(130.19909875789276,154.43637615812798, 0.0));
    positionAlloc4->Add (Vector(113.63869070757943,112.10141962796816, 0.0));
    positionAlloc4->Add (Vector(391.16903168113004,272.4418533380973, 0.0));
    positionAlloc4->Add (Vector(159.33767815016364,60.31639767869823, 0.0));
    positionAlloc4->Add (Vector(226.78752158541897,48.92103917278495, 0.0));
    positionAlloc4->Add (Vector(382.2654702052368,255.10691690303878, 0.0));
    positionAlloc4->Add (Vector(68.92960013205357,458.6980934292122, 0.0));
    positionAlloc4->Add (Vector(99.8227853258311,207.54494381068122, 0.0));
    positionAlloc4->Add (Vector(429.6866641504285,289.1611937257756, 0.0));
    positionAlloc4->Add (Vector(137.59651530526335,369.2059353023034, 0.0));
    positionAlloc4->Add (Vector(43.339383070053195,96.50001878712649, 0.0));
    positionAlloc4->Add (Vector(468.15422997284657,451.6893500554264, 0.0));
    positionAlloc4->Add (Vector(337.3034359084121,88.13806399171581, 0.0));
    positionAlloc4->Add (Vector(347.6987679857762,433.4941648038719, 0.0));
    positionAlloc4->Add (Vector(20.008892836321845,132.340815858427, 0.0));
    positionAlloc4->Add (Vector(337.7237578390099,397.7513551493326, 0.0));
    positionAlloc4->Add (Vector(392.58263147615344,382.3630984227088, 0.0));
    positionAlloc4->Add (Vector(352.9049005805243,489.56374547910855, 0.0));
    positionAlloc4->Add (Vector(51.57735987452833,12.261243527316113, 0.0));
    positionAlloc4->Add (Vector(436.62270990798436,266.8805456863916, 0.0));
    positionAlloc4->Add (Vector(92.37407617464183,215.4868268629372, 0.0));
    positionAlloc4->Add (Vector(175.1302528536014,375.36414617761255, 0.0));
    positionAlloc4->Add (Vector(38.41614238734042,122.65057205999635, 0.0));
    positionAlloc4->Add (Vector(464.63659107019845,201.03908690028078, 0.0));
    positionAlloc4->Add (Vector(445.15251361212063,202.2571899901789, 0.0));
    positionAlloc4->Add (Vector(251.65339179983116,26.391928059285252, 0.0));
    positionAlloc4->Add (Vector(17.680959745656978,408.6758646420066, 0.0));
    positionAlloc4->Add (Vector(496.4906788187392,162.08065159061746, 0.0));
    positionAlloc4->Add (Vector(110.18345057684742,242.6687269841903, 0.0));
    positionAlloc4->Add (Vector(272.7195446709022,341.23903964897215, 0.0));
    positionAlloc4->Add (Vector(247.3234240534545,157.51354448428378, 0.0));
    positionAlloc4->Add (Vector(125.99523105536925,169.74369714839622, 0.0));
    positionAlloc4->Add (Vector(301.5897771162627,438.6498821107557, 0.0));
    positionAlloc4->Add (Vector(357.9932236395234,373.6913673271217, 0.0));
    positionAlloc4->Add (Vector(89.45666818292331,113.95970590637761, 0.0));
    positionAlloc4->Add (Vector(470.42460468732196,489.79893570892057, 0.0));
    positionAlloc4->Add (Vector(408.6385130563753,358.7786015454775, 0.0));
    positionAlloc4->Add (Vector(198.12968364134693,223.97298925738556, 0.0));
    positionAlloc4->Add (Vector(368.3236023774437,404.66898450284003, 0.0));
    positionAlloc4->Add (Vector(239.550287112872,321.1613193242357, 0.0));
    positionAlloc4->Add (Vector(175.53276142410584,468.58704436089056, 0.0));
    positionAlloc4->Add (Vector(363.0862941538137,477.1703523121056, 0.0));
    positionAlloc4->Add (Vector(186.93428603267108,437.2438983105885, 0.0));
    positionAlloc4->Add (Vector(236.1867211076199,299.6279369393817, 0.0));
    positionAlloc4->Add (Vector(341.11032023697527,155.33134803192794, 0.0));
    positionAlloc4->Add (Vector(10.650565730690786,480.2522363775499, 0.0));
    positionAlloc4->Add (Vector(459.6129455101367,125.72922611596704, 0.0));
    positionAlloc4->Add (Vector(91.94779954094162,352.01534776816715, 0.0));
    positionAlloc4->Add (Vector(437.7651575190614,40.61828121570249, 0.0));
    positionAlloc4->Add (Vector(137.44341491262975,90.28802633342848, 0.0));
    positionAlloc4->Add (Vector(107.48824039583882,16.432627940958287, 0.0));
    positionAlloc4->Add (Vector(64.68235361367775,191.91095593021186, 0.0));
    positionAlloc4->Add (Vector(265.5723471966544,163.79694938298596, 0.0));
    positionAlloc4->Add (Vector(395.04776057295,257.387714186613, 0.0));
    positionAlloc4->Add (Vector(471.56216536412734,308.69115651248256, 0.0));
    positionAlloc4->Add (Vector(451.63635043516746,191.58792521554662, 0.0));
    positionAlloc4->Add (Vector(425.9773224021491,95.11104465609176, 0.0));
    positionAlloc4->Add (Vector(197.73185815375572,331.9730407164003, 0.0));
    positionAlloc4->Add (Vector(154.01849352726566,293.4811036148396, 0.0));
    positionAlloc4->Add (Vector(314.42078429859555,59.482698311002224, 0.0));
    positionAlloc4->Add (Vector(370.41343909896455,281.3364432966141, 0.0));
    positionAlloc4->Add (Vector(90.74369877886518,319.52642964738766, 0.0));
    positionAlloc4->Add (Vector(97.14499104309337,439.3150132913602, 0.0));
    positionAlloc4->Add (Vector(331.20459034562396,244.04022276066206, 0.0));
    positionAlloc4->Add (Vector(395.01307530134267,61.10695504692809, 0.0));
    positionAlloc4->Add (Vector(59.84163859624325,245.04474804381664, 0.0));
    positionAlloc4->Add (Vector(133.5201549641476,499.9378937871312, 0.0));
    positionAlloc4->Add (Vector(413.90161972961107,5.737756302906682, 0.0));
    positionAlloc4->Add (Vector(420.45391572676624,421.3338612975163, 0.0));
    positionAlloc4->Add (Vector(94.48129662290572,234.4194748210905, 0.0));
    positionAlloc4->Add (Vector(472.81714249319566,191.957120989269, 0.0));
    positionAlloc4->Add (Vector(80.16161599419353,243.36946787887663, 0.0));
    positionAlloc4->Add (Vector(161.18333756720975,305.1025293731851, 0.0));
    positionAlloc4->Add (Vector(309.97284693081497,175.65916017909626, 0.0));
    positionAlloc4->Add (Vector(332.0754078220377,151.5403379715331, 0.0));
    positionAlloc4->Add (Vector(327.2346756137172,462.6305939293831, 0.0));
    positionAlloc4->Add (Vector(432.208130849174,209.35433752858296, 0.0));
    positionAlloc4->Add (Vector(493.5915410331872,281.19722797858907, 0.0));
    positionAlloc4->Add (Vector(307.6108457542273,350.6040459593162, 0.0));
    positionAlloc4->Add (Vector(392.6844749344329,309.9961016601195, 0.0));
    positionAlloc4->Add (Vector(297.0721639457697,62.64097309307437, 0.0));
    positionAlloc4->Add (Vector(89.6750083183176,206.78018988789592, 0.0));
    positionAlloc4->Add (Vector(366.1803879347789,367.23471767496716, 0.0));
    positionAlloc4->Add (Vector(74.5797544600944,65.93584536422054, 0.0));
    positionAlloc4->Add (Vector(296.87692917772176,145.42996890523528, 0.0));
    positionAlloc4->Add (Vector(471.2992178040859,248.64489605833174, 0.0));
    positionAlloc4->Add (Vector(260.8906046170292,394.2395666867576, 0.0));
    positionAlloc4->Add (Vector(141.97544314451798,42.976113788708915, 0.0));
    positionAlloc4->Add (Vector(138.64752017160504,174.38149176921797, 0.0));
    positionAlloc4->Add (Vector(255.99961533196148,0.9818226912600148, 0.0));
    positionAlloc4->Add (Vector(263.58008786113066,144.03979404287742, 0.0));
    positionAlloc4->Add (Vector(423.5564580410972,251.68880481827654, 0.0));
    positionAlloc4->Add (Vector(240.7429060933261,96.01563301557215, 0.0));
    positionAlloc4->Add (Vector(368.903688801992,136.316140536254, 0.0));
    positionAlloc4->Add (Vector(2.1325092035535675,23.465421613116323, 0.0));
    positionAlloc4->Add (Vector(419.20145318948954,159.95835020899273, 0.0));
    positionAlloc4->Add (Vector(239.56963938145682,407.0368766878326, 0.0));
    positionAlloc4->Add (Vector(364.407465972577,176.19472124151835, 0.0));
    positionAlloc4->Add (Vector(231.04209649887108,377.95054876890254, 0.0));
    positionAlloc4->Add (Vector(8.026104583531035,76.35296559058824, 0.0));
    positionAlloc4->Add (Vector(129.09433200374198,18.620127805392407, 0.0));
    positionAlloc4->Add (Vector(103.70328240966198,373.64016052961125, 0.0));
    positionAlloc4->Add (Vector(158.97237692366696,475.39641703886025, 0.0));
    positionAlloc4->Add (Vector(48.03149937996959,59.36520255629679, 0.0));
    positionAlloc4->Add (Vector(201.04695304659674,138.6100414893413, 0.0));
    positionAlloc4->Add (Vector(52.872107565026084,188.0285377736578, 0.0));
    positionAlloc4->Add (Vector(434.1192857204445,7.509710876712039, 0.0));
    positionAlloc4->Add (Vector(287.53508349435697,107.39126794242937, 0.0));
    positionAlloc4->Add (Vector(430.50568430698775,64.75047297144792, 0.0));
    positionAlloc4->Add (Vector(464.11400912505053,394.97389819152016, 0.0));
    positionAlloc4->Add (Vector(256.6501747339533,174.12834087537348, 0.0));
    positionAlloc4->Add (Vector(195.2198183688082,144.50554159388486, 0.0));
    positionAlloc4->Add (Vector(136.9590466147775,52.55353454102474, 0.0));
    positionAlloc4->Add (Vector(485.3710047606973,45.87691672059574, 0.0));
    positionAlloc4->Add (Vector(210.74550521404905,277.55221500421925, 0.0));
    positionAlloc4->Add (Vector(93.71238536267573,27.382063033261627, 0.0));
    positionAlloc4->Add (Vector(94.22689345563079,431.5556300831675, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_3.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_ECC_flowmon/switch_ECC_3.flowmon", false, false);
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