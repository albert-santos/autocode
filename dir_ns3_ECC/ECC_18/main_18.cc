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
	  uint16_t numberOfRrhs = 81;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 711;
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


	ConfigStore inputConfig;
	inputConfig.ConfigureDefaults();

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
    positionAlloc->Add (Vector (850.0,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (850.0,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (850.0,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (850.0,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (850.0,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (850.0,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,850.0, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,150.0, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,150.0, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,850.0, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,150.0, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,850.0, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,150.0, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,850.0, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,150.0, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,850.0, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,150.0, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,850.0, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,150.0, 0.0));
    positionAlloc->Add (Vector (150.0,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (150.0,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (150.0,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (150.0,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (150.0,305.55555555555554, 0.0));
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
    positionAlloc4->Add (Vector(416.935071985599,685.6331420867209, 0.0));
    positionAlloc4->Add (Vector(600.9438372884846,792.7342412608137, 0.0));
    positionAlloc4->Add (Vector(158.49622082064036,492.0507279691309, 0.0));
    positionAlloc4->Add (Vector(309.38063815071894,585.5383523960932, 0.0));
    positionAlloc4->Add (Vector(426.5283749770489,544.9525839233559, 0.0));
    positionAlloc4->Add (Vector(653.8812182249676,733.6665162110032, 0.0));
    positionAlloc4->Add (Vector(340.86759318770083,152.3651927658886, 0.0));
    positionAlloc4->Add (Vector(644.1346685286608,425.56438983894856, 0.0));
    positionAlloc4->Add (Vector(478.7538494818845,203.62253141959704, 0.0));
    positionAlloc4->Add (Vector(173.19342353524763,337.86485569400344, 0.0));
    positionAlloc4->Add (Vector(517.6230529539569,734.3165683438773, 0.0));
    positionAlloc4->Add (Vector(560.3874902352877,639.4352385869635, 0.0));
    positionAlloc4->Add (Vector(190.3498869181754,806.0834313444883, 0.0));
    positionAlloc4->Add (Vector(833.3618796906821,491.7091692611163, 0.0));
    positionAlloc4->Add (Vector(601.0017095694452,268.03974761956675, 0.0));
    positionAlloc4->Add (Vector(308.9222839975881,537.3747081608346, 0.0));
    positionAlloc4->Add (Vector(185.4209709852633,774.757993365906, 0.0));
    positionAlloc4->Add (Vector(697.9149972521343,672.3053526787515, 0.0));
    positionAlloc4->Add (Vector(680.1139167344128,560.2773751286945, 0.0));
    positionAlloc4->Add (Vector(165.14752746161463,313.2031125326606, 0.0));
    positionAlloc4->Add (Vector(269.8978454045059,736.723144480017, 0.0));
    positionAlloc4->Add (Vector(263.59657701981655,158.69126241859644, 0.0));
    positionAlloc4->Add (Vector(845.3729151173477,639.1088465927154, 0.0));
    positionAlloc4->Add (Vector(745.2539513604953,197.37147566058837, 0.0));
    positionAlloc4->Add (Vector(572.5213441548663,801.8573314252286, 0.0));
    positionAlloc4->Add (Vector(607.6868728716383,203.26255994470338, 0.0));
    positionAlloc4->Add (Vector(309.8241216578599,417.30033765528276, 0.0));
    positionAlloc4->Add (Vector(842.6836835064139,485.1829710821771, 0.0));
    positionAlloc4->Add (Vector(587.8477859816928,424.4943803719426, 0.0));
    positionAlloc4->Add (Vector(780.2632156433194,302.98392306439405, 0.0));
    positionAlloc4->Add (Vector(378.08491639149923,699.9212546961157, 0.0));
    positionAlloc4->Add (Vector(638.3443759828649,633.970511650513, 0.0));
    positionAlloc4->Add (Vector(393.9540598821137,733.0984195675925, 0.0));
    positionAlloc4->Add (Vector(651.4317190796867,848.5192957599223, 0.0));
    positionAlloc4->Add (Vector(637.3021474569788,773.4750169238296, 0.0));
    positionAlloc4->Add (Vector(525.5012777526445,669.4648147907711, 0.0));
    positionAlloc4->Add (Vector(794.2101894009237,464.3977819215737, 0.0));
    positionAlloc4->Add (Vector(805.3028906189711,743.6195318600969, 0.0));
    positionAlloc4->Add (Vector(399.2974134104777,824.6373549596162, 0.0));
    positionAlloc4->Add (Vector(434.95948097076945,605.4023537765422, 0.0));
    positionAlloc4->Add (Vector(412.62347881917174,490.2024137667587, 0.0));
    positionAlloc4->Add (Vector(403.5041484361658,676.6176794714191, 0.0));
    positionAlloc4->Add (Vector(150.38792920967774,575.8235809800469, 0.0));
    positionAlloc4->Add (Vector(313.6938583598344,205.9405765426247, 0.0));
    positionAlloc4->Add (Vector(277.8179917609695,581.6114866498724, 0.0));
    positionAlloc4->Add (Vector(595.6487247511095,358.5702002708514, 0.0));
    positionAlloc4->Add (Vector(551.6768484322581,476.202460047109, 0.0));
    positionAlloc4->Add (Vector(618.2661492324374,732.5840201791041, 0.0));
    positionAlloc4->Add (Vector(710.1107656653575,599.2159313698163, 0.0));
    positionAlloc4->Add (Vector(698.0918500860151,276.5431683885088, 0.0));
    positionAlloc4->Add (Vector(598.8397432625037,796.5218675845424, 0.0));
    positionAlloc4->Add (Vector(287.68050388880295,680.4924050527297, 0.0));
    positionAlloc4->Add (Vector(173.23134115750105,511.9103067207012, 0.0));
    positionAlloc4->Add (Vector(709.4589744040688,340.07343803595177, 0.0));
    positionAlloc4->Add (Vector(715.6103243439484,332.5817560175061, 0.0));
    positionAlloc4->Add (Vector(259.102257310954,282.46632178167374, 0.0));
    positionAlloc4->Add (Vector(481.54152883922666,818.2321600528378, 0.0));
    positionAlloc4->Add (Vector(845.7984133895698,530.4781664113996, 0.0));
    positionAlloc4->Add (Vector(457.06107908497256,162.14214879760254, 0.0));
    positionAlloc4->Add (Vector(413.05643160164004,554.3605972462049, 0.0));
    positionAlloc4->Add (Vector(222.48050036694877,470.46955505923745, 0.0));
    positionAlloc4->Add (Vector(792.1533895456936,474.61221515192454, 0.0));
    positionAlloc4->Add (Vector(280.78408440620814,583.7649061337961, 0.0));
    positionAlloc4->Add (Vector(730.9426216908711,774.8593367348363, 0.0));
    positionAlloc4->Add (Vector(253.32477341031205,394.2034677391912, 0.0));
    positionAlloc4->Add (Vector(154.15922909104808,183.39583901771044, 0.0));
    positionAlloc4->Add (Vector(844.9072337155395,821.4354405209651, 0.0));
    positionAlloc4->Add (Vector(756.0890327939595,807.2183205493969, 0.0));
    positionAlloc4->Add (Vector(318.3118945731365,434.57681972090916, 0.0));
    positionAlloc4->Add (Vector(752.491422779505,218.32699876079525, 0.0));
    positionAlloc4->Add (Vector(518.5609315651411,527.5985602033654, 0.0));
    positionAlloc4->Add (Vector(750.4606560978135,190.7057843964808, 0.0));
    positionAlloc4->Add (Vector(785.707760259252,524.8558722751484, 0.0));
    positionAlloc4->Add (Vector(243.7990768439695,211.9633367941473, 0.0));
    positionAlloc4->Add (Vector(706.8943537515887,684.5230514140966, 0.0));
    positionAlloc4->Add (Vector(491.75355143946695,430.8503793979302, 0.0));
    positionAlloc4->Add (Vector(453.8534759378437,284.3828541014665, 0.0));
    positionAlloc4->Add (Vector(595.8292805158362,515.2792320546914, 0.0));
    positionAlloc4->Add (Vector(291.5505612212813,530.064956563679, 0.0));
    positionAlloc4->Add (Vector(722.6768493209173,789.0010829750712, 0.0));
    positionAlloc4->Add (Vector(323.9845175258257,415.0944502957641, 0.0));
    positionAlloc4->Add (Vector(348.74217976510147,320.0983150025809, 0.0));
    positionAlloc4->Add (Vector(835.407267826042,201.41128370403302, 0.0));
    positionAlloc4->Add (Vector(299.8813703890549,536.2475598758725, 0.0));
    positionAlloc4->Add (Vector(446.58312506234796,507.5966554746586, 0.0));
    positionAlloc4->Add (Vector(157.39698594211006,621.4615692304762, 0.0));
    positionAlloc4->Add (Vector(692.3065153131921,509.7001514883857, 0.0));
    positionAlloc4->Add (Vector(232.37355885170928,273.10051692133476, 0.0));
    positionAlloc4->Add (Vector(516.5888703899163,205.40126779474662, 0.0));
    positionAlloc4->Add (Vector(378.7292330701216,352.67992345559344, 0.0));
    positionAlloc4->Add (Vector(816.0496074591467,650.0225794272611, 0.0));
    positionAlloc4->Add (Vector(674.5032523844752,363.6108313761028, 0.0));
    positionAlloc4->Add (Vector(268.47331821002615,225.98875044085392, 0.0));
    positionAlloc4->Add (Vector(557.3605490540481,700.2808134621927, 0.0));
    positionAlloc4->Add (Vector(183.1449841287876,287.1851057441674, 0.0));
    positionAlloc4->Add (Vector(359.0121294018048,261.27556581584145, 0.0));
    positionAlloc4->Add (Vector(464.55611013760546,495.2368638515504, 0.0));
    positionAlloc4->Add (Vector(459.06308755760557,520.4330691225969, 0.0));
    positionAlloc4->Add (Vector(330.30514774859955,383.41991437215313, 0.0));
    positionAlloc4->Add (Vector(323.22741065093624,570.3310042751918, 0.0));
    positionAlloc4->Add (Vector(502.75030540697094,626.3883933511372, 0.0));
    positionAlloc4->Add (Vector(818.0650201546251,297.6785027981896, 0.0));
    positionAlloc4->Add (Vector(643.3490054134088,745.5641705772142, 0.0));
    positionAlloc4->Add (Vector(806.8555106331816,229.55588291817833, 0.0));
    positionAlloc4->Add (Vector(647.0993177937696,683.1769491862228, 0.0));
    positionAlloc4->Add (Vector(639.2493626616517,849.6213470304815, 0.0));
    positionAlloc4->Add (Vector(648.0590302182729,442.7788872792335, 0.0));
    positionAlloc4->Add (Vector(647.4670721599846,837.12659913241, 0.0));
    positionAlloc4->Add (Vector(338.542115342773,723.5231336428752, 0.0));
    positionAlloc4->Add (Vector(169.23963638055966,341.58686117644424, 0.0));
    positionAlloc4->Add (Vector(689.5100307145962,227.23434167764208, 0.0));
    positionAlloc4->Add (Vector(787.7950390910584,170.2949214992859, 0.0));
    positionAlloc4->Add (Vector(336.66176087174085,839.4061582413899, 0.0));
    positionAlloc4->Add (Vector(750.6095602978288,389.6672362064319, 0.0));
    positionAlloc4->Add (Vector(598.2431424084159,477.81066331997084, 0.0));
    positionAlloc4->Add (Vector(283.905221941475,341.33070818726407, 0.0));
    positionAlloc4->Add (Vector(509.4654538387385,576.6642179169469, 0.0));
    positionAlloc4->Add (Vector(548.3326024255718,174.97209145237267, 0.0));
    positionAlloc4->Add (Vector(190.02575817118657,567.87477600151, 0.0));
    positionAlloc4->Add (Vector(619.4060355788547,776.0728975889435, 0.0));
    positionAlloc4->Add (Vector(301.67897079230187,729.0306999717252, 0.0));
    positionAlloc4->Add (Vector(782.3387095947562,155.9402084584101, 0.0));
    positionAlloc4->Add (Vector(685.8025517213401,670.9646809060672, 0.0));
    positionAlloc4->Add (Vector(558.1045381031859,252.94893318524038, 0.0));
    positionAlloc4->Add (Vector(198.96080709704188,443.37356040069466, 0.0));
    positionAlloc4->Add (Vector(756.3322406859065,767.9639754319394, 0.0));
    positionAlloc4->Add (Vector(280.4837898476777,542.3561138545812, 0.0));
    positionAlloc4->Add (Vector(476.11021023537273,190.37829920848066, 0.0));
    positionAlloc4->Add (Vector(618.6490035854862,168.68521101079799, 0.0));
    positionAlloc4->Add (Vector(362.43894598609256,541.3429014356761, 0.0));
    positionAlloc4->Add (Vector(564.9964034245551,493.8873399430005, 0.0));
    positionAlloc4->Add (Vector(469.90161656385845,430.1018270991914, 0.0));
    positionAlloc4->Add (Vector(308.18486925862845,597.6279076444725, 0.0));
    positionAlloc4->Add (Vector(566.7613565842255,389.91828951219213, 0.0));
    positionAlloc4->Add (Vector(715.2491121596626,244.6678795893058, 0.0));
    positionAlloc4->Add (Vector(321.45733198555945,220.1003014168458, 0.0));
    positionAlloc4->Add (Vector(418.6055223036181,233.40993187832538, 0.0));
    positionAlloc4->Add (Vector(618.9484395129261,478.9229356290077, 0.0));
    positionAlloc4->Add (Vector(203.79462330286395,425.0611075613519, 0.0));
    positionAlloc4->Add (Vector(180.2562059924453,389.74999510569614, 0.0));
    positionAlloc4->Add (Vector(802.2519801358219,516.0911157341968, 0.0));
    positionAlloc4->Add (Vector(580.0949087561206,169.6037236307209, 0.0));
    positionAlloc4->Add (Vector(803.165851176898,307.8521717646738, 0.0));
    positionAlloc4->Add (Vector(248.49548157790198,811.6130200245278, 0.0));
    positionAlloc4->Add (Vector(170.32793154645356,582.4183504374986, 0.0));
    positionAlloc4->Add (Vector(220.0126630170256,634.3370186336933, 0.0));
    positionAlloc4->Add (Vector(214.45634518653384,317.8209210567809, 0.0));
    positionAlloc4->Add (Vector(161.80400905747308,256.1274465513332, 0.0));
    positionAlloc4->Add (Vector(315.42821232662055,456.86662799865206, 0.0));
    positionAlloc4->Add (Vector(248.02746188504648,619.5036679949337, 0.0));
    positionAlloc4->Add (Vector(480.2370058048765,670.7035014201989, 0.0));
    positionAlloc4->Add (Vector(208.79265097241125,773.216438442195, 0.0));
    positionAlloc4->Add (Vector(759.230324187866,693.1030070610564, 0.0));
    positionAlloc4->Add (Vector(398.5663364542329,496.9629408245618, 0.0));
    positionAlloc4->Add (Vector(250.31702539292746,461.78380000448436, 0.0));
    positionAlloc4->Add (Vector(651.883830744714,557.8206557491496, 0.0));
    positionAlloc4->Add (Vector(261.0186040866296,599.1805555823598, 0.0));
    positionAlloc4->Add (Vector(524.2616967243595,164.13644615903303, 0.0));
    positionAlloc4->Add (Vector(758.1955262608147,440.39357886687617, 0.0));
    positionAlloc4->Add (Vector(418.7124674039943,480.29008238632963, 0.0));
    positionAlloc4->Add (Vector(576.3571585147095,715.1200105383366, 0.0));
    positionAlloc4->Add (Vector(178.8888231622353,793.196999118173, 0.0));
    positionAlloc4->Add (Vector(601.8092832284475,813.2660863858665, 0.0));
    positionAlloc4->Add (Vector(491.2515950341959,406.4122016836373, 0.0));
    positionAlloc4->Add (Vector(362.9431626945026,608.0753553814023, 0.0));
    positionAlloc4->Add (Vector(219.95583459803368,766.8324658260162, 0.0));
    positionAlloc4->Add (Vector(208.60689317623695,691.1970710196942, 0.0));
    positionAlloc4->Add (Vector(340.66920548430267,732.4434677279055, 0.0));
    positionAlloc4->Add (Vector(284.9264072888986,631.8776934488358, 0.0));
    positionAlloc4->Add (Vector(508.06369022401157,644.6813959223805, 0.0));
    positionAlloc4->Add (Vector(757.1418532914787,744.1267266681903, 0.0));
    positionAlloc4->Add (Vector(839.0669188330223,477.9828648540306, 0.0));
    positionAlloc4->Add (Vector(197.28839041898095,270.00254313230266, 0.0));
    positionAlloc4->Add (Vector(502.77604587777546,292.21202685494626, 0.0));
    positionAlloc4->Add (Vector(330.50497755139145,545.782374365353, 0.0));
    positionAlloc4->Add (Vector(279.9061657484888,303.5297288216434, 0.0));
    positionAlloc4->Add (Vector(543.8667464594332,208.31180817825748, 0.0));
    positionAlloc4->Add (Vector(499.7691706103374,672.7227407588159, 0.0));
    positionAlloc4->Add (Vector(628.0731154794454,325.75531682054174, 0.0));
    positionAlloc4->Add (Vector(274.4588369956741,185.8676788024973, 0.0));
    positionAlloc4->Add (Vector(680.8139994593053,605.6184395867086, 0.0));
    positionAlloc4->Add (Vector(316.228235055922,429.0402731508525, 0.0));
    positionAlloc4->Add (Vector(807.4529464143797,555.9883243164587, 0.0));
    positionAlloc4->Add (Vector(617.4292176389749,748.3448424069055, 0.0));
    positionAlloc4->Add (Vector(705.3413795026831,688.3162671478535, 0.0));
    positionAlloc4->Add (Vector(682.5611702292713,466.907798640958, 0.0));
    positionAlloc4->Add (Vector(843.3301706430797,411.98512749551395, 0.0));
    positionAlloc4->Add (Vector(331.3687437674779,390.8698348208155, 0.0));
    positionAlloc4->Add (Vector(636.7305845167969,668.7749291407765, 0.0));
    positionAlloc4->Add (Vector(531.200464042184,293.6637487462164, 0.0));
    positionAlloc4->Add (Vector(179.64482323349,541.1720644428251, 0.0));
    positionAlloc4->Add (Vector(295.94613246798235,263.9926026953663, 0.0));
    positionAlloc4->Add (Vector(849.8636008936611,495.09470037317226, 0.0));
    positionAlloc4->Add (Vector(437.92588777628873,571.9913227504455, 0.0));
    positionAlloc4->Add (Vector(452.1562338157727,382.5594348463987, 0.0));
    positionAlloc4->Add (Vector(419.39938374513747,693.2816188894504, 0.0));
    positionAlloc4->Add (Vector(764.2818797916311,222.6473497236703, 0.0));
    positionAlloc4->Add (Vector(502.2432690524748,688.1143553651618, 0.0));
    positionAlloc4->Add (Vector(575.8163196724918,421.87522626872084, 0.0));
    positionAlloc4->Add (Vector(772.3294823831128,735.0512055165401, 0.0));
    positionAlloc4->Add (Vector(563.9398128590492,650.9474652468716, 0.0));
    positionAlloc4->Add (Vector(698.7386664110636,382.71340277334684, 0.0));
    positionAlloc4->Add (Vector(549.9460959948617,254.98391690608128, 0.0));
    positionAlloc4->Add (Vector(466.4654656732689,360.4423500071729, 0.0));
    positionAlloc4->Add (Vector(184.43229316241545,458.8005195583536, 0.0));
    positionAlloc4->Add (Vector(246.28647803259778,848.788283342848, 0.0));
    positionAlloc4->Add (Vector(237.0963638350493,391.5356085254398, 0.0));
    positionAlloc4->Add (Vector(489.76102661843595,290.7584418842322, 0.0));
    positionAlloc4->Add (Vector(406.8078760003606,682.5817210136154, 0.0));
    positionAlloc4->Add (Vector(691.3471794384008,737.2167563535847, 0.0));
    positionAlloc4->Add (Vector(200.15542996203925,810.4524749432674, 0.0));
    positionAlloc4->Add (Vector(542.8928603993486,386.42903304550975, 0.0));
    positionAlloc4->Add (Vector(238.93727516660277,501.77217897995257, 0.0));
    positionAlloc4->Add (Vector(239.313867501164,452.89344023858496, 0.0));
    positionAlloc4->Add (Vector(805.0598820705039,470.5434170397346, 0.0));
    positionAlloc4->Add (Vector(508.0558553359471,335.9749258519861, 0.0));
    positionAlloc4->Add (Vector(794.7206612376614,524.5747460004225, 0.0));
    positionAlloc4->Add (Vector(667.0634374932777,785.1026930168003, 0.0));
    positionAlloc4->Add (Vector(665.2378101379527,509.0851893400393, 0.0));
    positionAlloc4->Add (Vector(696.5412644210326,404.65023611934, 0.0));
    positionAlloc4->Add (Vector(424.48949849075177,232.8268511021967, 0.0));
    positionAlloc4->Add (Vector(667.6858041144951,573.5021429366232, 0.0));
    positionAlloc4->Add (Vector(330.2974079546618,530.7288944992033, 0.0));
    positionAlloc4->Add (Vector(172.85852421594183,198.5213829599218, 0.0));
    positionAlloc4->Add (Vector(466.7735508205104,504.5876711169924, 0.0));
    positionAlloc4->Add (Vector(650.8389258420725,716.2491150524539, 0.0));
    positionAlloc4->Add (Vector(203.83744711473426,293.56562760652696, 0.0));
    positionAlloc4->Add (Vector(246.05206212704616,262.3607906964839, 0.0));
    positionAlloc4->Add (Vector(155.4111871382443,446.17940169705037, 0.0));
    positionAlloc4->Add (Vector(526.1686897075576,262.08976812924817, 0.0));
    positionAlloc4->Add (Vector(202.83454009682026,198.02068942660196, 0.0));
    positionAlloc4->Add (Vector(215.0016310604649,703.5236830042334, 0.0));
    positionAlloc4->Add (Vector(629.1028827071191,477.52476969241746, 0.0));
    positionAlloc4->Add (Vector(812.6471090504668,794.3746618677393, 0.0));
    positionAlloc4->Add (Vector(456.15290409383033,202.74088230120552, 0.0));
    positionAlloc4->Add (Vector(443.88714764855365,772.7996040870783, 0.0));
    positionAlloc4->Add (Vector(555.4548326997748,476.644058965131, 0.0));
    positionAlloc4->Add (Vector(723.1830294498825,290.1863083025973, 0.0));
    positionAlloc4->Add (Vector(630.3632960675134,173.93681494483218, 0.0));
    positionAlloc4->Add (Vector(165.8807895277133,801.7155953496958, 0.0));
    positionAlloc4->Add (Vector(691.9508907164511,574.9200580691827, 0.0));
    positionAlloc4->Add (Vector(419.0373195709694,666.8741458552403, 0.0));
    positionAlloc4->Add (Vector(285.96305078690716,440.89167248632157, 0.0));
    positionAlloc4->Add (Vector(236.86781327677764,443.9352833889273, 0.0));
    positionAlloc4->Add (Vector(623.0536745395896,440.2831079160504, 0.0));
    positionAlloc4->Add (Vector(662.3774868468985,179.98612483229888, 0.0));
    positionAlloc4->Add (Vector(236.2642676068628,633.1475436896275, 0.0));
    positionAlloc4->Add (Vector(349.1925878331218,552.8326945204553, 0.0));
    positionAlloc4->Add (Vector(561.5536131239852,739.6497514028674, 0.0));
    positionAlloc4->Add (Vector(502.8266053401301,628.4169203947353, 0.0));
    positionAlloc4->Add (Vector(477.82701515798857,753.2796915772219, 0.0));
    positionAlloc4->Add (Vector(674.4927357768731,287.29754481371015, 0.0));
    positionAlloc4->Add (Vector(734.8774954689978,660.9118088498485, 0.0));
    positionAlloc4->Add (Vector(565.6980552940606,429.5802112448063, 0.0));
    positionAlloc4->Add (Vector(405.1506751841547,842.1322841259832, 0.0));
    positionAlloc4->Add (Vector(708.7195618324916,700.6291428525313, 0.0));
    positionAlloc4->Add (Vector(311.2623756562168,555.5839417606583, 0.0));
    positionAlloc4->Add (Vector(306.9205937228312,401.376960077318, 0.0));
    positionAlloc4->Add (Vector(272.3858408263478,387.01018035910147, 0.0));
    positionAlloc4->Add (Vector(442.41533615809385,682.1439371704122, 0.0));
    positionAlloc4->Add (Vector(641.1195449297179,454.2216581025943, 0.0));
    positionAlloc4->Add (Vector(717.2813586012883,449.39081957346696, 0.0));
    positionAlloc4->Add (Vector(466.4058015909568,513.3642112378097, 0.0));
    positionAlloc4->Add (Vector(591.1298983382007,712.6284632984334, 0.0));
    positionAlloc4->Add (Vector(505.3786772445591,634.708246762691, 0.0));
    positionAlloc4->Add (Vector(425.4137072565451,242.29639275037786, 0.0));
    positionAlloc4->Add (Vector(365.5605953633524,345.0398331628299, 0.0));
    positionAlloc4->Add (Vector(384.67348499023177,150.92390828903078, 0.0));
    positionAlloc4->Add (Vector(161.85054669787453,515.4666658403523, 0.0));
    positionAlloc4->Add (Vector(462.70508919291154,403.91565358073854, 0.0));
    positionAlloc4->Add (Vector(563.0653796416054,460.30019025546767, 0.0));
    positionAlloc4->Add (Vector(500.8683341977473,317.42375640016337, 0.0));
    positionAlloc4->Add (Vector(328.35464901191295,490.0488800486627, 0.0));
    positionAlloc4->Add (Vector(620.3915099816282,829.5097147900561, 0.0));
    positionAlloc4->Add (Vector(353.20823813082404,729.638481078491, 0.0));
    positionAlloc4->Add (Vector(199.82375920457375,540.3933333346698, 0.0));
    positionAlloc4->Add (Vector(808.2438643691343,248.42406285356876, 0.0));
    positionAlloc4->Add (Vector(825.8207249078624,716.4968850600033, 0.0));
    positionAlloc4->Add (Vector(434.4798969981482,661.3920566282256, 0.0));
    positionAlloc4->Add (Vector(352.9559655636645,280.6787481400217, 0.0));
    positionAlloc4->Add (Vector(297.48154192121103,735.8571913669693, 0.0));
    positionAlloc4->Add (Vector(590.6971766181402,621.6865728168839, 0.0));
    positionAlloc4->Add (Vector(468.92177625116346,404.333647341385, 0.0));
    positionAlloc4->Add (Vector(211.12916079092838,798.2569427508431, 0.0));
    positionAlloc4->Add (Vector(667.8751440559175,526.7004010570181, 0.0));
    positionAlloc4->Add (Vector(461.2239194652706,692.3948925738694, 0.0));
    positionAlloc4->Add (Vector(765.6942663289468,556.0304887086688, 0.0));
    positionAlloc4->Add (Vector(777.0842736831135,819.3583379533077, 0.0));
    positionAlloc4->Add (Vector(485.790113773482,751.2386282669489, 0.0));
    positionAlloc4->Add (Vector(335.6043015519075,161.82534855006293, 0.0));
    positionAlloc4->Add (Vector(735.0644587324853,666.9916486810566, 0.0));
    positionAlloc4->Add (Vector(480.4544842760092,509.9450221341004, 0.0));
    positionAlloc4->Add (Vector(751.595813602913,757.9257388486579, 0.0));
    positionAlloc4->Add (Vector(289.5267012654017,195.67863238750417, 0.0));
    positionAlloc4->Add (Vector(471.1101769401686,837.078975131019, 0.0));
    positionAlloc4->Add (Vector(798.359215144081,397.20808159777647, 0.0));
    positionAlloc4->Add (Vector(162.17212004780373,433.62747266067277, 0.0));
    positionAlloc4->Add (Vector(549.4509277815314,768.9361298598585, 0.0));
    positionAlloc4->Add (Vector(656.6224697854499,167.51757109470662, 0.0));
    positionAlloc4->Add (Vector(511.6580805100219,576.7868186014889, 0.0));
    positionAlloc4->Add (Vector(457.874516483271,690.3245024276781, 0.0));
    positionAlloc4->Add (Vector(518.7620431991556,526.045725576422, 0.0));
    positionAlloc4->Add (Vector(320.8957745648345,288.98668247596504, 0.0));
    positionAlloc4->Add (Vector(223.9078990330996,843.981304134728, 0.0));
    positionAlloc4->Add (Vector(470.1098585978285,535.6609356884221, 0.0));
    positionAlloc4->Add (Vector(229.3788291414361,453.6089438522114, 0.0));
    positionAlloc4->Add (Vector(714.1339529132313,449.20174981215, 0.0));
    positionAlloc4->Add (Vector(773.7354318513743,565.5725520785313, 0.0));
    positionAlloc4->Add (Vector(758.2095568775704,400.0896453972316, 0.0));
    positionAlloc4->Add (Vector(193.38252519266356,771.6562675811635, 0.0));
    positionAlloc4->Add (Vector(529.0041336268914,162.92613342526516, 0.0));
    positionAlloc4->Add (Vector(262.39046753421917,623.7156890759206, 0.0));
    positionAlloc4->Add (Vector(828.5310877461623,237.76521887573136, 0.0));
    positionAlloc4->Add (Vector(801.508056181778,802.6994480851707, 0.0));
    positionAlloc4->Add (Vector(555.8829188868046,514.2424358101019, 0.0));
    positionAlloc4->Add (Vector(790.8617101016877,746.7574454024436, 0.0));
    positionAlloc4->Add (Vector(398.1974747813056,325.7786007733043, 0.0));
    positionAlloc4->Add (Vector(579.4449402963858,216.9803451141344, 0.0));
    positionAlloc4->Add (Vector(282.99755644159916,390.4109983058069, 0.0));
    positionAlloc4->Add (Vector(802.8409760175517,776.1648704849545, 0.0));
    positionAlloc4->Add (Vector(523.5216810677995,244.00787507663068, 0.0));
    positionAlloc4->Add (Vector(535.3447286261425,365.1055502696052, 0.0));
    positionAlloc4->Add (Vector(436.06037754688526,186.81402986847496, 0.0));
    positionAlloc4->Add (Vector(338.233013193037,189.22039327423957, 0.0));
    positionAlloc4->Add (Vector(530.5909937514423,480.156960929872, 0.0));
    positionAlloc4->Add (Vector(789.9461527765683,504.20373433685626, 0.0));
    positionAlloc4->Add (Vector(528.6438693011091,551.5666182843067, 0.0));
    positionAlloc4->Add (Vector(846.153572954391,158.4064141589849, 0.0));
    positionAlloc4->Add (Vector(292.5540487862942,600.7985675413315, 0.0));
    positionAlloc4->Add (Vector(242.2396220179252,418.0769077450193, 0.0));
    positionAlloc4->Add (Vector(284.9204550486579,361.7527722459881, 0.0));
    positionAlloc4->Add (Vector(715.0258391417437,610.2458414793757, 0.0));
    positionAlloc4->Add (Vector(730.580154139255,283.5545858330871, 0.0));
    positionAlloc4->Add (Vector(817.9619502334951,410.0037654527126, 0.0));
    positionAlloc4->Add (Vector(391.81247006385945,682.8900007377522, 0.0));
    positionAlloc4->Add (Vector(702.565743423347,267.67178486362604, 0.0));
    positionAlloc4->Add (Vector(651.6995267213488,227.1630361048629, 0.0));
    positionAlloc4->Add (Vector(517.2952788275861,165.57524634843168, 0.0));
    positionAlloc4->Add (Vector(433.588251335802,760.7643617923273, 0.0));
    positionAlloc4->Add (Vector(279.90222192153345,685.5302329793428, 0.0));
    positionAlloc4->Add (Vector(269.5281856973375,175.69558796323986, 0.0));
    positionAlloc4->Add (Vector(225.99420129196656,704.213025099757, 0.0));
    positionAlloc4->Add (Vector(498.9814148319053,762.9573713722101, 0.0));
    positionAlloc4->Add (Vector(604.105316108434,423.1371770749316, 0.0));
    positionAlloc4->Add (Vector(252.1224725383011,537.1264907927209, 0.0));
    positionAlloc4->Add (Vector(245.97223165203005,526.9331836638942, 0.0));
    positionAlloc4->Add (Vector(381.15065761201924,524.9421569930623, 0.0));
    positionAlloc4->Add (Vector(739.1726464900421,702.3034191924862, 0.0));
    positionAlloc4->Add (Vector(302.0087334437455,838.9748365439033, 0.0));
    positionAlloc4->Add (Vector(624.6507586253417,629.1811031219268, 0.0));
    positionAlloc4->Add (Vector(483.05565421431504,735.2789634493814, 0.0));
    positionAlloc4->Add (Vector(409.77816464361166,638.2493252259662, 0.0));
    positionAlloc4->Add (Vector(302.90677149207625,529.6870900748161, 0.0));
    positionAlloc4->Add (Vector(725.6544596687014,532.3953733531987, 0.0));
    positionAlloc4->Add (Vector(766.874669825824,580.4370600613586, 0.0));
    positionAlloc4->Add (Vector(700.8930583693776,383.98436234544647, 0.0));
    positionAlloc4->Add (Vector(680.5564177243161,582.501407843579, 0.0));
    positionAlloc4->Add (Vector(522.5643933736885,687.8873801798239, 0.0));
    positionAlloc4->Add (Vector(639.4699146718317,597.1063861719042, 0.0));
    positionAlloc4->Add (Vector(341.24245423516277,436.29055261368404, 0.0));
    positionAlloc4->Add (Vector(571.8430448999444,279.17453551565086, 0.0));
    positionAlloc4->Add (Vector(283.5237845940619,635.8139620771958, 0.0));
    positionAlloc4->Add (Vector(765.4908488280365,695.4421957355755, 0.0));
    positionAlloc4->Add (Vector(609.1640975868744,233.88846307884273, 0.0));
    positionAlloc4->Add (Vector(382.93460073497556,343.6689595253573, 0.0));
    positionAlloc4->Add (Vector(475.05922412742876,511.28720894174074, 0.0));
    positionAlloc4->Add (Vector(408.53111817375134,772.1150525706378, 0.0));
    positionAlloc4->Add (Vector(772.4498093969336,392.4795238339966, 0.0));
    positionAlloc4->Add (Vector(694.3497987743824,494.9242272905364, 0.0));
    positionAlloc4->Add (Vector(326.4103135753368,416.80335735273553, 0.0));
    positionAlloc4->Add (Vector(525.1551951903499,769.6602889478208, 0.0));
    positionAlloc4->Add (Vector(624.0330657837499,638.463288946086, 0.0));
    positionAlloc4->Add (Vector(423.5834844870553,738.9158460497913, 0.0));
    positionAlloc4->Add (Vector(509.7765587658519,646.0930024495526, 0.0));
    positionAlloc4->Add (Vector(695.2248515718002,591.7460447173873, 0.0));
    positionAlloc4->Add (Vector(687.5562861326079,747.0470507249148, 0.0));
    positionAlloc4->Add (Vector(212.31128731372735,340.2169615391372, 0.0));
    positionAlloc4->Add (Vector(259.14837652168205,327.48484315233645, 0.0));
    positionAlloc4->Add (Vector(427.5774592951389,843.6422323561586, 0.0));
    positionAlloc4->Add (Vector(805.181859297507,842.4445680586651, 0.0));
    positionAlloc4->Add (Vector(489.31895017978445,734.5209728867928, 0.0));
    positionAlloc4->Add (Vector(723.3970464261425,538.9924785042109, 0.0));
    positionAlloc4->Add (Vector(663.7047887957874,846.3275816433801, 0.0));
    positionAlloc4->Add (Vector(563.6906794776944,212.78950790577824, 0.0));
    positionAlloc4->Add (Vector(837.7841330777195,276.5389435106322, 0.0));
    positionAlloc4->Add (Vector(695.5353857236408,398.45937514246924, 0.0));
    positionAlloc4->Add (Vector(473.7737897648103,443.25858945608934, 0.0));
    positionAlloc4->Add (Vector(487.36099557490456,179.72963734188795, 0.0));
    positionAlloc4->Add (Vector(248.4867967025224,303.7722745962952, 0.0));
    positionAlloc4->Add (Vector(839.8783364718274,515.2831339463949, 0.0));
    positionAlloc4->Add (Vector(389.466504031598,384.7018219556294, 0.0));
    positionAlloc4->Add (Vector(522.6457885619973,746.6488222712396, 0.0));
    positionAlloc4->Add (Vector(278.69200882250846,808.9878269929509, 0.0));
    positionAlloc4->Add (Vector(689.9195432575098,438.0581220294473, 0.0));
    positionAlloc4->Add (Vector(839.5236915041718,772.4998234519843, 0.0));
    positionAlloc4->Add (Vector(465.85071929487736,254.36282413777343, 0.0));
    positionAlloc4->Add (Vector(477.80199737100753,729.3697948984359, 0.0));
    positionAlloc4->Add (Vector(655.033957913812,193.3797048855872, 0.0));
    positionAlloc4->Add (Vector(611.6431197613563,179.12226334872494, 0.0));
    positionAlloc4->Add (Vector(498.9984518744887,523.3287899270555, 0.0));
    positionAlloc4->Add (Vector(329.90436255570256,434.96436931061385, 0.0));
    positionAlloc4->Add (Vector(682.565279001754,745.5462678414774, 0.0));
    positionAlloc4->Add (Vector(365.4246616254742,238.49835900851963, 0.0));
    positionAlloc4->Add (Vector(546.6672176251152,607.9178183943526, 0.0));
    positionAlloc4->Add (Vector(517.4140526039077,569.7481008482764, 0.0));
    positionAlloc4->Add (Vector(441.64229508358807,397.5465041980773, 0.0));
    positionAlloc4->Add (Vector(278.4940356839361,349.42204723214275, 0.0));
    positionAlloc4->Add (Vector(380.3439718306367,568.7357149625254, 0.0));
    positionAlloc4->Add (Vector(757.1828792571973,378.96976578005547, 0.0));
    positionAlloc4->Add (Vector(578.0721162303417,317.6253456741267, 0.0));
    positionAlloc4->Add (Vector(691.8431948622813,255.81337073175857, 0.0));
    positionAlloc4->Add (Vector(191.9854905340615,491.57034785352414, 0.0));
    positionAlloc4->Add (Vector(516.5860325712817,837.7131499329522, 0.0));
    positionAlloc4->Add (Vector(728.4514471225029,466.49733858571324, 0.0));
    positionAlloc4->Add (Vector(429.90108659605085,402.56477837202584, 0.0));
    positionAlloc4->Add (Vector(497.220925977481,393.417847607148, 0.0));
    positionAlloc4->Add (Vector(763.8921816817899,296.06733715811424, 0.0));
    positionAlloc4->Add (Vector(699.9387630015984,809.3373813960359, 0.0));
    positionAlloc4->Add (Vector(819.3814386191668,453.60951802718444, 0.0));
    positionAlloc4->Add (Vector(496.43812334366237,479.95770899104343, 0.0));
    positionAlloc4->Add (Vector(425.91931820845434,269.0748174084532, 0.0));
    positionAlloc4->Add (Vector(222.4046166541336,311.8059572938739, 0.0));
    positionAlloc4->Add (Vector(803.9087406551045,680.0166377887638, 0.0));
    positionAlloc4->Add (Vector(802.7152947564502,591.4259301554266, 0.0));
    positionAlloc4->Add (Vector(769.6979803678303,403.11318649421673, 0.0));
    positionAlloc4->Add (Vector(172.98480035871432,477.8866815170511, 0.0));
    positionAlloc4->Add (Vector(184.83703761200286,516.8780788588685, 0.0));
    positionAlloc4->Add (Vector(673.9912899694737,781.5542005782619, 0.0));
    positionAlloc4->Add (Vector(392.2016890805935,465.8437857486776, 0.0));
    positionAlloc4->Add (Vector(686.2559993827549,461.7063406626473, 0.0));
    positionAlloc4->Add (Vector(650.9716437987377,423.4917854816765, 0.0));
    positionAlloc4->Add (Vector(303.3268531494409,200.32959306024725, 0.0));
    positionAlloc4->Add (Vector(759.0335976244743,672.3862429325436, 0.0));
    positionAlloc4->Add (Vector(296.81729575699615,379.8160972505384, 0.0));
    positionAlloc4->Add (Vector(177.9525324859265,734.6520824261382, 0.0));
    positionAlloc4->Add (Vector(201.368811656406,433.6580480505338, 0.0));
    positionAlloc4->Add (Vector(159.5230768154678,438.3288083269556, 0.0));
    positionAlloc4->Add (Vector(206.3615910140583,543.2584968634966, 0.0));
    positionAlloc4->Add (Vector(333.5147372861596,298.99553967193964, 0.0));
    positionAlloc4->Add (Vector(790.3711830354865,831.365437042582, 0.0));
    positionAlloc4->Add (Vector(423.36098837907576,516.850712554693, 0.0));
    positionAlloc4->Add (Vector(210.6749928055815,753.5761398117411, 0.0));
    positionAlloc4->Add (Vector(772.3673699013677,845.3525985383203, 0.0));
    positionAlloc4->Add (Vector(559.5699289814903,300.40982169439417, 0.0));
    positionAlloc4->Add (Vector(368.3800797026945,784.1686468245678, 0.0));
    positionAlloc4->Add (Vector(839.8061690587622,388.315486880524, 0.0));
    positionAlloc4->Add (Vector(320.3467458235191,178.08160480196082, 0.0));
    positionAlloc4->Add (Vector(828.3090744520999,691.3927476337541, 0.0));
    positionAlloc4->Add (Vector(668.2133535291872,689.0213280910615, 0.0));
    positionAlloc4->Add (Vector(286.52116225240803,230.02617988338767, 0.0));
    positionAlloc4->Add (Vector(410.65640876002186,410.10117596134006, 0.0));
    positionAlloc4->Add (Vector(481.2505209426735,829.9073591999819, 0.0));
    positionAlloc4->Add (Vector(770.7084682014925,268.62669622190765, 0.0));
    positionAlloc4->Add (Vector(530.9777365456041,819.5486873439689, 0.0));
    positionAlloc4->Add (Vector(775.8382288915012,497.63666984421997, 0.0));
    positionAlloc4->Add (Vector(394.08852675314563,430.1046877340916, 0.0));
    positionAlloc4->Add (Vector(251.8974310567508,731.7972751047678, 0.0));
    positionAlloc4->Add (Vector(375.79146564328784,447.10372220030536, 0.0));
    positionAlloc4->Add (Vector(608.6627392034409,388.222183008734, 0.0));
    positionAlloc4->Add (Vector(653.651158533436,459.47262234544525, 0.0));
    positionAlloc4->Add (Vector(793.6007203908853,397.4837557944186, 0.0));
    positionAlloc4->Add (Vector(164.75646229997588,751.0547052318416, 0.0));
    positionAlloc4->Add (Vector(847.0995004247136,778.1076119733062, 0.0));
    positionAlloc4->Add (Vector(532.6159844480712,211.7947350778866, 0.0));
    positionAlloc4->Add (Vector(644.8377105692932,178.09757171578713, 0.0));
    positionAlloc4->Add (Vector(416.4031589646749,688.5654025242209, 0.0));
    positionAlloc4->Add (Vector(620.3692983464384,219.6231182165329, 0.0));
    positionAlloc4->Add (Vector(815.7634756969534,526.8376432450113, 0.0));
    positionAlloc4->Add (Vector(828.4694913206266,288.101816101948, 0.0));
    positionAlloc4->Add (Vector(663.8748866167887,584.6119312897608, 0.0));
    positionAlloc4->Add (Vector(579.1364181086351,589.7956213456495, 0.0));
    positionAlloc4->Add (Vector(577.3543218292734,307.3321663611736, 0.0));
    positionAlloc4->Add (Vector(538.2064700749371,709.8806178349311, 0.0));
    positionAlloc4->Add (Vector(714.2782723608834,456.4596718923415, 0.0));
    positionAlloc4->Add (Vector(178.35048053734366,578.8679708176355, 0.0));
    positionAlloc4->Add (Vector(520.694165553836,170.32714786184212, 0.0));
    positionAlloc4->Add (Vector(194.15262204007672,702.4007659050384, 0.0));
    positionAlloc4->Add (Vector(176.70752415476403,450.87238869896737, 0.0));
    positionAlloc4->Add (Vector(691.3745572401406,186.436678857115, 0.0));
    positionAlloc4->Add (Vector(798.6989917154414,172.13050788986948, 0.0));
    positionAlloc4->Add (Vector(502.85246278806187,518.6825512484459, 0.0));
    positionAlloc4->Add (Vector(211.20288824554973,591.5284986285374, 0.0));
    positionAlloc4->Add (Vector(155.98833908271914,733.5252910817683, 0.0));
    positionAlloc4->Add (Vector(406.21077131431315,362.8491993461799, 0.0));
    positionAlloc4->Add (Vector(720.1555776913721,601.5578843767504, 0.0));
    positionAlloc4->Add (Vector(730.6662158505283,735.5641361133052, 0.0));
    positionAlloc4->Add (Vector(358.70007980369815,254.39457549327648, 0.0));
    positionAlloc4->Add (Vector(804.9585905866812,560.2842441638479, 0.0));
    positionAlloc4->Add (Vector(363.2059629610743,174.94085677309752, 0.0));
    positionAlloc4->Add (Vector(740.7194759659426,621.1063706741367, 0.0));
    positionAlloc4->Add (Vector(796.9663905698931,727.5272374363532, 0.0));
    positionAlloc4->Add (Vector(682.3514030920678,516.2246068876505, 0.0));
    positionAlloc4->Add (Vector(762.417377209501,644.5707426122337, 0.0));
    positionAlloc4->Add (Vector(823.4527646260561,695.7440492934354, 0.0));
    positionAlloc4->Add (Vector(842.4793670305412,688.7201059063126, 0.0));
    positionAlloc4->Add (Vector(202.95127646014475,473.9814600391654, 0.0));
    positionAlloc4->Add (Vector(611.7177789411705,749.6091109592203, 0.0));
    positionAlloc4->Add (Vector(816.4408928080475,703.0766585869661, 0.0));
    positionAlloc4->Add (Vector(763.5004923897045,740.2393991365796, 0.0));
    positionAlloc4->Add (Vector(838.2803276730989,380.1831328168332, 0.0));
    positionAlloc4->Add (Vector(216.01885602392332,449.6896354970992, 0.0));
    positionAlloc4->Add (Vector(340.18623131848153,396.33353903106257, 0.0));
    positionAlloc4->Add (Vector(240.36744490174334,469.55822970525315, 0.0));
    positionAlloc4->Add (Vector(280.4334334410851,404.7162755388183, 0.0));
    positionAlloc4->Add (Vector(255.24211231223657,446.46686461071585, 0.0));
    positionAlloc4->Add (Vector(840.767477259129,181.4366475561121, 0.0));
    positionAlloc4->Add (Vector(250.2152954443054,811.5554384654823, 0.0));
    positionAlloc4->Add (Vector(377.2050614156954,278.0213556275869, 0.0));
    positionAlloc4->Add (Vector(410.7268634779713,470.3386428126087, 0.0));
    positionAlloc4->Add (Vector(554.93127570132,841.3170421592966, 0.0));
    positionAlloc4->Add (Vector(364.66804121918153,823.1052797272412, 0.0));
    positionAlloc4->Add (Vector(229.88033003074497,318.35842968387584, 0.0));
    positionAlloc4->Add (Vector(246.65855962140634,242.5985956067238, 0.0));
    positionAlloc4->Add (Vector(357.7964239959896,256.68484402651694, 0.0));
    positionAlloc4->Add (Vector(437.5902242366368,410.11008863877447, 0.0));
    positionAlloc4->Add (Vector(840.8325665046789,770.8526774220204, 0.0));
    positionAlloc4->Add (Vector(589.8354557090577,557.659394210844, 0.0));
    positionAlloc4->Add (Vector(253.83742650379915,165.49329289068265, 0.0));
    positionAlloc4->Add (Vector(764.5590947997242,205.8817465940114, 0.0));
    positionAlloc4->Add (Vector(238.56883983435296,184.18913632452075, 0.0));
    positionAlloc4->Add (Vector(693.1817361249217,653.4879541640304, 0.0));
    positionAlloc4->Add (Vector(607.652755585706,456.62224819412876, 0.0));
    positionAlloc4->Add (Vector(308.58769396840387,426.6043759483457, 0.0));
    positionAlloc4->Add (Vector(293.1124569459641,317.09056328928494, 0.0));
    positionAlloc4->Add (Vector(252.10838886772726,279.65633158165656, 0.0));
    positionAlloc4->Add (Vector(483.0577704962207,607.4956068767715, 0.0));
    positionAlloc4->Add (Vector(208.87706951678,276.92809927301926, 0.0));
    positionAlloc4->Add (Vector(258.7807901173108,604.8535092588372, 0.0));
    positionAlloc4->Add (Vector(500.8629086748856,643.3853382621355, 0.0));
    positionAlloc4->Add (Vector(719.5452166612814,153.13718532787018, 0.0));
    positionAlloc4->Add (Vector(696.1588416325262,679.1119058248711, 0.0));
    positionAlloc4->Add (Vector(302.0726007366163,195.59606706962717, 0.0));
    positionAlloc4->Add (Vector(687.6227179778576,158.84982098001294, 0.0));
    positionAlloc4->Add (Vector(694.4685004107422,321.66097076217517, 0.0));
    positionAlloc4->Add (Vector(198.7954225282728,751.8960303409642, 0.0));
    positionAlloc4->Add (Vector(464.6214352286188,632.5371755400579, 0.0));
    positionAlloc4->Add (Vector(632.5380809428232,617.3250608089022, 0.0));
    positionAlloc4->Add (Vector(775.2446119855894,595.5811420037069, 0.0));
    positionAlloc4->Add (Vector(708.5519572579209,548.2790236328688, 0.0));
    positionAlloc4->Add (Vector(280.9397852184662,492.312166242616, 0.0));
    positionAlloc4->Add (Vector(285.05410540495666,292.7205328223667, 0.0));
    positionAlloc4->Add (Vector(270.28418984184134,400.7028064811852, 0.0));
    positionAlloc4->Add (Vector(500.1036582150795,445.64631089051414, 0.0));
    positionAlloc4->Add (Vector(778.7957952806564,363.1104142932618, 0.0));
    positionAlloc4->Add (Vector(814.2171374296247,192.5468180405824, 0.0));
    positionAlloc4->Add (Vector(660.7807640560136,296.59987029725346, 0.0));
    positionAlloc4->Add (Vector(481.3947914434119,730.5006742713599, 0.0));
    positionAlloc4->Add (Vector(452.1308621606444,729.8260780492899, 0.0));
    positionAlloc4->Add (Vector(256.1668722503342,207.37814431544336, 0.0));
    positionAlloc4->Add (Vector(358.49500790027196,814.8595474234021, 0.0));
    positionAlloc4->Add (Vector(721.9890975191014,791.1944009534542, 0.0));
    positionAlloc4->Add (Vector(485.9881458702942,729.0051368527139, 0.0));
    positionAlloc4->Add (Vector(218.0779510988727,748.9393770746636, 0.0));
    positionAlloc4->Add (Vector(447.7486920300786,846.8844853176985, 0.0));
    positionAlloc4->Add (Vector(178.17133943347548,677.2654900161326, 0.0));
    positionAlloc4->Add (Vector(681.0717192807467,218.30541870338863, 0.0));
    positionAlloc4->Add (Vector(745.9021875082605,453.3568640877844, 0.0));
    positionAlloc4->Add (Vector(714.3467586207032,771.9021936008672, 0.0));
    positionAlloc4->Add (Vector(506.8437912924587,272.81847913785873, 0.0));
    positionAlloc4->Add (Vector(807.3569134209141,340.32182534296726, 0.0));
    positionAlloc4->Add (Vector(800.0244307039136,833.1253365271291, 0.0));
    positionAlloc4->Add (Vector(293.4412227611998,776.299873328929, 0.0));
    positionAlloc4->Add (Vector(221.08962240042518,643.4278362683572, 0.0));
    positionAlloc4->Add (Vector(151.2073754768885,218.36642268828604, 0.0));
    positionAlloc4->Add (Vector(512.4184599731236,156.14232854334247, 0.0));
    positionAlloc4->Add (Vector(515.270990968279,527.6040488415553, 0.0));
    positionAlloc4->Add (Vector(459.05472030241975,512.7294596401769, 0.0));
    positionAlloc4->Add (Vector(641.0485303960485,840.47900353971, 0.0));
    positionAlloc4->Add (Vector(324.9252364071553,491.96284775157835, 0.0));
    positionAlloc4->Add (Vector(206.310240146534,259.55552935631954, 0.0));
    positionAlloc4->Add (Vector(422.15658327298183,233.37004032654892, 0.0));
    positionAlloc4->Add (Vector(344.0124296909363,761.3586077545931, 0.0));
    positionAlloc4->Add (Vector(186.0884112863759,821.1203078359061, 0.0));
    positionAlloc4->Add (Vector(798.2051923222755,166.66423028084012, 0.0));
    positionAlloc4->Add (Vector(470.6102370211621,755.2744268099552, 0.0));
    positionAlloc4->Add (Vector(719.283610945458,599.9020083218907, 0.0));
    positionAlloc4->Add (Vector(686.6884766834378,718.3595759112636, 0.0));
    positionAlloc4->Add (Vector(477.7584828648427,771.8280702918354, 0.0));
    positionAlloc4->Add (Vector(656.7967689013166,545.5124382342774, 0.0));
    positionAlloc4->Add (Vector(257.54206427528663,685.0800779725142, 0.0));
    positionAlloc4->Add (Vector(219.12707807718186,198.71429317741627, 0.0));
    positionAlloc4->Add (Vector(492.5315821369652,719.3403303315758, 0.0));
    positionAlloc4->Add (Vector(444.0438687056129,192.80232673338034, 0.0));
    positionAlloc4->Add (Vector(357.30812778470056,589.12712866622, 0.0));
    positionAlloc4->Add (Vector(471.2415670091147,828.7004362855907, 0.0));
    positionAlloc4->Add (Vector(543.3286721009985,453.36659368045855, 0.0));
    positionAlloc4->Add (Vector(762.6310494211102,393.0167877720048, 0.0));
    positionAlloc4->Add (Vector(215.9556651232552,370.6311031077747, 0.0));
    positionAlloc4->Add (Vector(204.1054452053261,330.509002483606, 0.0));
    positionAlloc4->Add (Vector(373.5688817835796,560.4548989260668, 0.0));
    positionAlloc4->Add (Vector(802.587487186717,763.4076483440394, 0.0));
    positionAlloc4->Add (Vector(621.0652801724581,721.4784084530994, 0.0));
    positionAlloc4->Add (Vector(691.9200500160504,171.29562007529898, 0.0));
    positionAlloc4->Add (Vector(269.3032413159972,396.7239490794679, 0.0));
    positionAlloc4->Add (Vector(367.49246649076866,759.2836189594699, 0.0));
    positionAlloc4->Add (Vector(497.45506072641496,480.8948290454024, 0.0));
    positionAlloc4->Add (Vector(648.823278978313,198.7072484805053, 0.0));
    positionAlloc4->Add (Vector(225.06506123866646,214.80324693646912, 0.0));
    positionAlloc4->Add (Vector(752.5099915716593,160.79047091388537, 0.0));
    positionAlloc4->Add (Vector(766.4077101846766,574.7820002396132, 0.0));
    positionAlloc4->Add (Vector(679.6217488056982,732.742632093614, 0.0));
    positionAlloc4->Add (Vector(436.64739484188624,268.6641122306237, 0.0));
    positionAlloc4->Add (Vector(597.5736201321012,761.0113000369881, 0.0));
    positionAlloc4->Add (Vector(248.52035437380727,259.64392683336314, 0.0));
    positionAlloc4->Add (Vector(612.0018232022196,352.270473398502, 0.0));
    positionAlloc4->Add (Vector(507.9271805627885,595.4436876869396, 0.0));
    positionAlloc4->Add (Vector(526.1504244588141,478.47146210937774, 0.0));
    positionAlloc4->Add (Vector(394.5975788856516,643.6322852421483, 0.0));
    positionAlloc4->Add (Vector(630.7748511946072,547.4782639797088, 0.0));
    positionAlloc4->Add (Vector(370.6963591727458,192.38657936888242, 0.0));
    positionAlloc4->Add (Vector(643.3769157735014,366.1906663997214, 0.0));
    positionAlloc4->Add (Vector(453.48017196266073,477.02189334671255, 0.0));
    positionAlloc4->Add (Vector(170.16095939853346,440.3814703222678, 0.0));
    positionAlloc4->Add (Vector(609.8252642582793,345.99128461786813, 0.0));
    positionAlloc4->Add (Vector(633.3868482797513,229.7733869507066, 0.0));
    positionAlloc4->Add (Vector(621.6254523812011,660.4648566278526, 0.0));
    positionAlloc4->Add (Vector(602.2072036626893,399.3737771613087, 0.0));
    positionAlloc4->Add (Vector(613.7478112289948,409.8593534302014, 0.0));
    positionAlloc4->Add (Vector(840.1731790809922,172.07423969185686, 0.0));
    positionAlloc4->Add (Vector(396.06962911781625,194.32347288062465, 0.0));
    positionAlloc4->Add (Vector(347.7491857931383,376.42713431134763, 0.0));
    positionAlloc4->Add (Vector(280.83667437895406,529.4247698542827, 0.0));
    positionAlloc4->Add (Vector(730.612647833165,571.9108603553213, 0.0));
    positionAlloc4->Add (Vector(721.8883665663486,227.350945859383, 0.0));
    positionAlloc4->Add (Vector(543.9390495610721,324.54661763469136, 0.0));
    positionAlloc4->Add (Vector(698.0892547032205,357.6944024189297, 0.0));
    positionAlloc4->Add (Vector(353.2876454995726,362.4350886096733, 0.0));
    positionAlloc4->Add (Vector(639.5652060132547,664.5810773882772, 0.0));
    positionAlloc4->Add (Vector(632.2065390978893,776.5229261236186, 0.0));
    positionAlloc4->Add (Vector(543.8914037597474,832.2264476042691, 0.0));
    positionAlloc4->Add (Vector(636.2253281387099,653.4203459894876, 0.0));
    positionAlloc4->Add (Vector(444.0941879281703,828.2312019798266, 0.0));
    positionAlloc4->Add (Vector(819.6203974371153,423.8937987848619, 0.0));
    positionAlloc4->Add (Vector(163.9159360880963,528.8544396230258, 0.0));
    positionAlloc4->Add (Vector(673.4356939980225,347.120427164676, 0.0));
    positionAlloc4->Add (Vector(280.83837535340314,468.45455025821127, 0.0));
    positionAlloc4->Add (Vector(830.5095566743249,300.3315799737304, 0.0));
    positionAlloc4->Add (Vector(281.02502629194316,710.4365594889708, 0.0));
    positionAlloc4->Add (Vector(378.18533063816847,544.0870531606861, 0.0));
    positionAlloc4->Add (Vector(565.0679910491824,234.51684689629053, 0.0));
    positionAlloc4->Add (Vector(252.26348984116026,592.6806265946375, 0.0));
    positionAlloc4->Add (Vector(393.1781532252372,801.9508570445215, 0.0));
    positionAlloc4->Add (Vector(531.9020427218117,275.8937038864394, 0.0));
    positionAlloc4->Add (Vector(709.0932016898472,297.4519183448757, 0.0));
    positionAlloc4->Add (Vector(824.9020571058907,377.1863342227498, 0.0));
    positionAlloc4->Add (Vector(678.8176665276804,766.2038841094011, 0.0));
    positionAlloc4->Add (Vector(510.0285352369012,481.58379260349034, 0.0));
    positionAlloc4->Add (Vector(689.3425509509074,826.1651435788015, 0.0));
    positionAlloc4->Add (Vector(434.20877533175343,839.2798877406673, 0.0));
    positionAlloc4->Add (Vector(494.3582678265497,725.7752132394302, 0.0));
    positionAlloc4->Add (Vector(306.449128048251,743.046641301372, 0.0));
    positionAlloc4->Add (Vector(620.8541176026598,562.0842042543832, 0.0));
    positionAlloc4->Add (Vector(607.4023469220535,419.0862516406294, 0.0));
    positionAlloc4->Add (Vector(787.8888522884578,163.50939255888935, 0.0));
    positionAlloc4->Add (Vector(204.50904501445643,284.18948981314406, 0.0));
    positionAlloc4->Add (Vector(282.7874553950898,386.4977199867126, 0.0));
    positionAlloc4->Add (Vector(588.7975348774734,201.53278307357718, 0.0));
    positionAlloc4->Add (Vector(168.18760560804623,256.96406652279825, 0.0));
    positionAlloc4->Add (Vector(516.7819418799997,244.4123924856334, 0.0));
    positionAlloc4->Add (Vector(352.18060094372606,436.30541571539123, 0.0));
    positionAlloc4->Add (Vector(552.0588069673098,687.8554948848204, 0.0));
    positionAlloc4->Add (Vector(771.7245372648679,321.36751804263054, 0.0));
    positionAlloc4->Add (Vector(443.29557971716173,720.3573208619832, 0.0));
    positionAlloc4->Add (Vector(195.06241846052774,219.76280691187407, 0.0));
    positionAlloc4->Add (Vector(610.1751276462642,160.95312255944617, 0.0));
    positionAlloc4->Add (Vector(559.8598297367557,393.66688325551183, 0.0));
    positionAlloc4->Add (Vector(395.49162920150917,301.73039792538947, 0.0));
    positionAlloc4->Add (Vector(723.6066572552877,343.1457871476722, 0.0));
    positionAlloc4->Add (Vector(825.7720925644007,254.48332789216434, 0.0));
    positionAlloc4->Add (Vector(810.9863117588856,163.51309170250664, 0.0));
    positionAlloc4->Add (Vector(335.83125749835426,296.1197733998729, 0.0));
    positionAlloc4->Add (Vector(328.0380613043229,690.7684962138161, 0.0));
    positionAlloc4->Add (Vector(731.7590851496166,788.2163957941475, 0.0));
    positionAlloc4->Add (Vector(500.4876394567169,837.127458949703, 0.0));
    positionAlloc4->Add (Vector(421.85681437008327,567.4870186092523, 0.0));
    positionAlloc4->Add (Vector(703.2906752234471,791.389586140472, 0.0));
    positionAlloc4->Add (Vector(606.3815997460724,490.8584992674738, 0.0));
    positionAlloc4->Add (Vector(201.22256913760145,212.63486808769218, 0.0));
    positionAlloc4->Add (Vector(429.185630387829,675.2722489634912, 0.0));
    positionAlloc4->Add (Vector(790.9839450361463,498.55663737692436, 0.0));
    positionAlloc4->Add (Vector(753.4124012910117,447.03669785739294, 0.0));
    positionAlloc4->Add (Vector(282.54340654090106,402.8699558287073, 0.0));
    positionAlloc4->Add (Vector(204.24110309502464,565.2816291227641, 0.0));
    positionAlloc4->Add (Vector(233.50970642126873,221.96599321509777, 0.0));
    positionAlloc4->Add (Vector(156.70994810107618,348.6923038422956, 0.0));
    positionAlloc4->Add (Vector(821.955600259219,415.3607061341965, 0.0));
    positionAlloc4->Add (Vector(330.9543765034684,349.28857822128884, 0.0));
    positionAlloc4->Add (Vector(229.2544549534145,739.903386477628, 0.0));
    positionAlloc4->Add (Vector(156.4439671734501,290.15465505206623, 0.0));
    positionAlloc4->Add (Vector(569.7804199596416,382.52142167194995, 0.0));
    positionAlloc4->Add (Vector(702.7984064473843,286.68685463373777, 0.0));
    positionAlloc4->Add (Vector(685.7463957437577,776.9684488465252, 0.0));
    positionAlloc4->Add (Vector(292.98728726346394,531.6650028279021, 0.0));
    positionAlloc4->Add (Vector(683.562793430935,514.2861352778191, 0.0));
    positionAlloc4->Add (Vector(824.7673977718009,801.4978556327804, 0.0));
    positionAlloc4->Add (Vector(688.4618424790064,655.909094631239, 0.0));
    positionAlloc4->Add (Vector(803.4513136917044,174.48731146130748, 0.0));
    positionAlloc4->Add (Vector(341.36874267964504,554.1794869931637, 0.0));
    positionAlloc4->Add (Vector(602.6180986617293,395.50295661797566, 0.0));
    positionAlloc4->Add (Vector(205.83439781574975,431.34413089487356, 0.0));
    positionAlloc4->Add (Vector(824.2418486799917,645.0895081611186, 0.0));
    positionAlloc4->Add (Vector(829.5536933983207,700.9707927751116, 0.0));
    positionAlloc4->Add (Vector(848.3873530628495,677.3716804438851, 0.0));
    positionAlloc4->Add (Vector(522.6790810588391,779.0851376615126, 0.0));
    positionAlloc4->Add (Vector(204.67678212580932,449.19058413139663, 0.0));
    positionAlloc4->Add (Vector(317.1101683378015,792.3432336451971, 0.0));
    positionAlloc4->Add (Vector(215.44465310879283,297.61448842514653, 0.0));
    positionAlloc4->Add (Vector(255.5126851075292,345.18630850432584, 0.0));
    positionAlloc4->Add (Vector(450.5399483595421,753.1428243718832, 0.0));
    positionAlloc4->Add (Vector(625.3483692838779,684.3668818484938, 0.0));
    positionAlloc4->Add (Vector(581.4042531781226,278.4249777781939, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_18.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_ECC_flowmon/switch_ECC_18.flowmon", false, false);
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