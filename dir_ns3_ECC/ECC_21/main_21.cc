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
	  uint16_t numberOfRrhs = 79;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 575;
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
    positionAlloc->Add (Vector (850.0,850.0, 0.0));
    positionAlloc->Add (Vector (850.0,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (850.0,538.8888888888889, 0.0));
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
    positionAlloc->Add (Vector (694.4444444444445,850.0, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,850.0, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,150.0, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,850.0, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,850.0, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,616.6666666666667, 0.0));
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
    positionAlloc->Add (Vector (150.0,850.0, 0.0));
    positionAlloc->Add (Vector (150.0,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (150.0,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (150.0,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (150.0,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (150.0,383.33333333333337, 0.0));
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
    positionAlloc4->Add (Vector(196.60036311170455,282.11789198895514, 0.0));
    positionAlloc4->Add (Vector(183.96816610368512,281.79624422812543, 0.0));
    positionAlloc4->Add (Vector(778.788754743691,463.55940789767027, 0.0));
    positionAlloc4->Add (Vector(591.03654150474,740.2285386601458, 0.0));
    positionAlloc4->Add (Vector(232.40273935783915,791.2775907087464, 0.0));
    positionAlloc4->Add (Vector(621.5580315475703,198.20027632527908, 0.0));
    positionAlloc4->Add (Vector(488.5552654232946,846.536227292504, 0.0));
    positionAlloc4->Add (Vector(829.1645436996145,810.4494607927488, 0.0));
    positionAlloc4->Add (Vector(253.98160605698666,153.06157537068566, 0.0));
    positionAlloc4->Add (Vector(436.16798867462074,372.2906977337793, 0.0));
    positionAlloc4->Add (Vector(291.3272392930229,435.7152654310884, 0.0));
    positionAlloc4->Add (Vector(503.5446489259972,245.90772008189987, 0.0));
    positionAlloc4->Add (Vector(276.16355627292285,507.27693576406926, 0.0));
    positionAlloc4->Add (Vector(645.7371565807755,381.229382556588, 0.0));
    positionAlloc4->Add (Vector(797.7484349693825,167.80863079270756, 0.0));
    positionAlloc4->Add (Vector(644.2286887707178,546.0660439303219, 0.0));
    positionAlloc4->Add (Vector(379.45024841322976,430.4585012411222, 0.0));
    positionAlloc4->Add (Vector(798.8584639474276,383.6820665914504, 0.0));
    positionAlloc4->Add (Vector(802.0722609867054,538.4260633226465, 0.0));
    positionAlloc4->Add (Vector(240.71001100698908,151.45644742340002, 0.0));
    positionAlloc4->Add (Vector(293.28417223525327,582.8382220959588, 0.0));
    positionAlloc4->Add (Vector(392.0783146499755,624.0014561304142, 0.0));
    positionAlloc4->Add (Vector(559.8290203160225,804.1189807339471, 0.0));
    positionAlloc4->Add (Vector(222.99443048208238,777.5062050031156, 0.0));
    positionAlloc4->Add (Vector(443.65051814793696,234.99103571836793, 0.0));
    positionAlloc4->Add (Vector(156.45559151105886,311.0508347725924, 0.0));
    positionAlloc4->Add (Vector(158.46861518891654,351.7098646232214, 0.0));
    positionAlloc4->Add (Vector(414.62153681201727,204.19558433298806, 0.0));
    positionAlloc4->Add (Vector(678.7394108038937,654.887805439792, 0.0));
    positionAlloc4->Add (Vector(244.75372323218045,250.02155393458702, 0.0));
    positionAlloc4->Add (Vector(658.3267854000107,627.6009852345883, 0.0));
    positionAlloc4->Add (Vector(682.0330441726842,461.3620937256431, 0.0));
    positionAlloc4->Add (Vector(195.4931822371379,205.64422384688123, 0.0));
    positionAlloc4->Add (Vector(420.59051161204485,293.2574707524289, 0.0));
    positionAlloc4->Add (Vector(420.6840462471516,712.0064606782822, 0.0));
    positionAlloc4->Add (Vector(309.22067733104444,331.56599665482645, 0.0));
    positionAlloc4->Add (Vector(744.8615825616475,191.93329070235464, 0.0));
    positionAlloc4->Add (Vector(778.7911455770889,518.9307977800768, 0.0));
    positionAlloc4->Add (Vector(422.7166739070397,160.57521191139307, 0.0));
    positionAlloc4->Add (Vector(450.95782344958315,218.05709254849734, 0.0));
    positionAlloc4->Add (Vector(752.2452343659895,248.1877131448, 0.0));
    positionAlloc4->Add (Vector(612.9375158720078,606.4776000531097, 0.0));
    positionAlloc4->Add (Vector(236.23432162998114,411.8735944694145, 0.0));
    positionAlloc4->Add (Vector(755.7629284714767,725.6970692058659, 0.0));
    positionAlloc4->Add (Vector(344.1379425572576,150.42814460891094, 0.0));
    positionAlloc4->Add (Vector(600.8162516448836,649.1274926810422, 0.0));
    positionAlloc4->Add (Vector(400.57747789176585,766.8190619319221, 0.0));
    positionAlloc4->Add (Vector(629.6128404299303,642.984300774242, 0.0));
    positionAlloc4->Add (Vector(501.0479668752446,323.3808149126209, 0.0));
    positionAlloc4->Add (Vector(525.460416606888,487.0926774441422, 0.0));
    positionAlloc4->Add (Vector(165.411280966983,298.32057761497026, 0.0));
    positionAlloc4->Add (Vector(438.34583952591385,269.2069089764236, 0.0));
    positionAlloc4->Add (Vector(604.7642144286692,283.9413338913108, 0.0));
    positionAlloc4->Add (Vector(595.6403829115341,248.732643508609, 0.0));
    positionAlloc4->Add (Vector(422.18556193318,745.6515792032687, 0.0));
    positionAlloc4->Add (Vector(450.50644250270926,719.9591296511496, 0.0));
    positionAlloc4->Add (Vector(331.29725130974293,781.1684019426053, 0.0));
    positionAlloc4->Add (Vector(777.1992661766394,633.8682877956271, 0.0));
    positionAlloc4->Add (Vector(829.5931810395066,240.75528034379005, 0.0));
    positionAlloc4->Add (Vector(556.7113653737886,500.3777739924878, 0.0));
    positionAlloc4->Add (Vector(189.78734275587374,652.4808432986239, 0.0));
    positionAlloc4->Add (Vector(455.15414362378976,839.783348436442, 0.0));
    positionAlloc4->Add (Vector(437.4228470042387,809.5026221463368, 0.0));
    positionAlloc4->Add (Vector(625.4087819177412,216.6147300561505, 0.0));
    positionAlloc4->Add (Vector(173.67514920343945,463.2504883322158, 0.0));
    positionAlloc4->Add (Vector(654.6687173385706,782.8571959789158, 0.0));
    positionAlloc4->Add (Vector(303.2079446306828,682.6814951721573, 0.0));
    positionAlloc4->Add (Vector(499.6936177981648,646.2826021575372, 0.0));
    positionAlloc4->Add (Vector(438.00536619866364,795.284156976975, 0.0));
    positionAlloc4->Add (Vector(170.05215191717963,581.7948958931455, 0.0));
    positionAlloc4->Add (Vector(667.6385099349294,495.965148485329, 0.0));
    positionAlloc4->Add (Vector(325.3511178192923,397.6167769459323, 0.0));
    positionAlloc4->Add (Vector(627.804695963058,182.64901564806775, 0.0));
    positionAlloc4->Add (Vector(469.98440198763086,197.60019855330415, 0.0));
    positionAlloc4->Add (Vector(496.45262703746533,846.2830909751676, 0.0));
    positionAlloc4->Add (Vector(398.1426855477709,719.0670885530442, 0.0));
    positionAlloc4->Add (Vector(635.9854510438295,494.0179004976364, 0.0));
    positionAlloc4->Add (Vector(849.9100067918937,600.9246631185641, 0.0));
    positionAlloc4->Add (Vector(475.56206404018155,544.5915535927342, 0.0));
    positionAlloc4->Add (Vector(509.3730514697267,324.8383090015207, 0.0));
    positionAlloc4->Add (Vector(724.4537260345675,696.0426462527178, 0.0));
    positionAlloc4->Add (Vector(159.78655428765828,457.18571665889164, 0.0));
    positionAlloc4->Add (Vector(795.3065048605182,486.3719402108858, 0.0));
    positionAlloc4->Add (Vector(399.35882124256295,493.7269288866858, 0.0));
    positionAlloc4->Add (Vector(229.63320519761362,483.87752376716116, 0.0));
    positionAlloc4->Add (Vector(601.5591524708808,554.4750543901298, 0.0));
    positionAlloc4->Add (Vector(360.43883457705397,617.0817521876444, 0.0));
    positionAlloc4->Add (Vector(498.59308942833803,529.5369514202516, 0.0));
    positionAlloc4->Add (Vector(364.2727721103318,474.3281573816853, 0.0));
    positionAlloc4->Add (Vector(808.3414158784749,215.16719898011195, 0.0));
    positionAlloc4->Add (Vector(395.11307237762287,664.0167666260232, 0.0));
    positionAlloc4->Add (Vector(735.7021558322359,651.624392895535, 0.0));
    positionAlloc4->Add (Vector(590.8669899942824,382.5254749323193, 0.0));
    positionAlloc4->Add (Vector(153.54698014808358,435.9660270347038, 0.0));
    positionAlloc4->Add (Vector(358.27068737684226,208.87116979212425, 0.0));
    positionAlloc4->Add (Vector(223.4932625875946,843.5127054630436, 0.0));
    positionAlloc4->Add (Vector(771.8342387451321,786.6493300585013, 0.0));
    positionAlloc4->Add (Vector(639.286981827528,213.0722958020598, 0.0));
    positionAlloc4->Add (Vector(166.1443708980589,392.76517952253266, 0.0));
    positionAlloc4->Add (Vector(215.31082485007752,456.2316759395886, 0.0));
    positionAlloc4->Add (Vector(314.86011608966396,705.0875650073864, 0.0));
    positionAlloc4->Add (Vector(672.0583923931069,280.55040838336276, 0.0));
    positionAlloc4->Add (Vector(191.04009029580877,254.77425389931415, 0.0));
    positionAlloc4->Add (Vector(342.321522708016,555.5545321285831, 0.0));
    positionAlloc4->Add (Vector(370.5461126826398,450.2465954181116, 0.0));
    positionAlloc4->Add (Vector(262.42981852844264,833.266625378042, 0.0));
    positionAlloc4->Add (Vector(419.4156226190206,697.9143889499194, 0.0));
    positionAlloc4->Add (Vector(247.27225951954873,466.0211922003434, 0.0));
    positionAlloc4->Add (Vector(832.8373947134328,190.1719322249635, 0.0));
    positionAlloc4->Add (Vector(837.5815855094919,629.973645433586, 0.0));
    positionAlloc4->Add (Vector(693.1279576724291,591.065694618788, 0.0));
    positionAlloc4->Add (Vector(602.8576143564469,758.7318989149085, 0.0));
    positionAlloc4->Add (Vector(767.4996199581027,544.5676315653091, 0.0));
    positionAlloc4->Add (Vector(809.0753722269449,817.7317227904749, 0.0));
    positionAlloc4->Add (Vector(813.4635128537277,324.75506493081195, 0.0));
    positionAlloc4->Add (Vector(178.11746976540687,428.26102607886486, 0.0));
    positionAlloc4->Add (Vector(529.0309791891609,784.296311297064, 0.0));
    positionAlloc4->Add (Vector(344.1035993878964,766.053696218691, 0.0));
    positionAlloc4->Add (Vector(773.0050922858404,524.7782691978339, 0.0));
    positionAlloc4->Add (Vector(764.3684874962327,599.8477694655185, 0.0));
    positionAlloc4->Add (Vector(643.894233252586,730.1755799737673, 0.0));
    positionAlloc4->Add (Vector(624.125111573032,582.626236393242, 0.0));
    positionAlloc4->Add (Vector(460.7283101947958,779.0282653439556, 0.0));
    positionAlloc4->Add (Vector(817.7341995990997,344.99389941481485, 0.0));
    positionAlloc4->Add (Vector(306.89252576205513,224.3012569749743, 0.0));
    positionAlloc4->Add (Vector(333.25300077189524,238.1845025734178, 0.0));
    positionAlloc4->Add (Vector(494.70958260655107,209.82621607560324, 0.0));
    positionAlloc4->Add (Vector(326.56745335386654,199.71936060089675, 0.0));
    positionAlloc4->Add (Vector(584.1610367743293,821.7891603857031, 0.0));
    positionAlloc4->Add (Vector(189.29211161475064,720.8797061933051, 0.0));
    positionAlloc4->Add (Vector(840.0705740694609,329.4392098430805, 0.0));
    positionAlloc4->Add (Vector(327.3438551310013,722.7661009134608, 0.0));
    positionAlloc4->Add (Vector(449.09719568912936,445.6197144992226, 0.0));
    positionAlloc4->Add (Vector(508.4549177563753,731.5326067692586, 0.0));
    positionAlloc4->Add (Vector(761.8767394027055,834.599855492903, 0.0));
    positionAlloc4->Add (Vector(634.3519937711644,773.0688969313691, 0.0));
    positionAlloc4->Add (Vector(230.41816375515225,556.507406299964, 0.0));
    positionAlloc4->Add (Vector(665.4728530350312,827.4510343534145, 0.0));
    positionAlloc4->Add (Vector(566.3777024916399,694.0599640011604, 0.0));
    positionAlloc4->Add (Vector(842.3714101775237,453.36502695444534, 0.0));
    positionAlloc4->Add (Vector(250.9120422718587,804.4275139623019, 0.0));
    positionAlloc4->Add (Vector(770.3829691708693,605.5868511858347, 0.0));
    positionAlloc4->Add (Vector(219.80652004624812,670.3528753415469, 0.0));
    positionAlloc4->Add (Vector(481.04117968492034,284.73653743929515, 0.0));
    positionAlloc4->Add (Vector(713.1826844997731,725.5014156992612, 0.0));
    positionAlloc4->Add (Vector(294.7574564503245,695.6418425300736, 0.0));
    positionAlloc4->Add (Vector(368.5282517750577,672.0475106008903, 0.0));
    positionAlloc4->Add (Vector(674.3734729798234,550.2161178122255, 0.0));
    positionAlloc4->Add (Vector(566.3812641512907,699.1230268253654, 0.0));
    positionAlloc4->Add (Vector(640.4706413923118,740.5466089920561, 0.0));
    positionAlloc4->Add (Vector(588.1231508771793,386.62969119792785, 0.0));
    positionAlloc4->Add (Vector(768.9194609576202,701.3961116204931, 0.0));
    positionAlloc4->Add (Vector(451.679589753698,382.38386651286316, 0.0));
    positionAlloc4->Add (Vector(162.32176659094148,319.3087452796532, 0.0));
    positionAlloc4->Add (Vector(560.7367346796192,611.7661521835238, 0.0));
    positionAlloc4->Add (Vector(306.57264843845724,342.5537495965461, 0.0));
    positionAlloc4->Add (Vector(690.2727484718486,302.1917839786797, 0.0));
    positionAlloc4->Add (Vector(356.44829369171885,243.5691742052915, 0.0));
    positionAlloc4->Add (Vector(185.42758853002144,537.4394675090296, 0.0));
    positionAlloc4->Add (Vector(268.02476440266,680.1392464707859, 0.0));
    positionAlloc4->Add (Vector(152.74823989248577,404.95951680234776, 0.0));
    positionAlloc4->Add (Vector(167.85948420533876,779.893604204971, 0.0));
    positionAlloc4->Add (Vector(613.9863202035108,846.5218575042248, 0.0));
    positionAlloc4->Add (Vector(502.38287619112316,380.202442771474, 0.0));
    positionAlloc4->Add (Vector(791.4890426702993,421.55455297520854, 0.0));
    positionAlloc4->Add (Vector(467.8456794324085,344.0575051953176, 0.0));
    positionAlloc4->Add (Vector(500.0436286131457,772.3252308932412, 0.0));
    positionAlloc4->Add (Vector(581.3413009901076,799.7231573463645, 0.0));
    positionAlloc4->Add (Vector(743.2724024234124,753.3767293099756, 0.0));
    positionAlloc4->Add (Vector(303.1885718655433,502.49098242786573, 0.0));
    positionAlloc4->Add (Vector(171.436927566434,215.8755781162916, 0.0));
    positionAlloc4->Add (Vector(157.14293109967164,351.48403821105825, 0.0));
    positionAlloc4->Add (Vector(837.2457222945926,714.5816572369434, 0.0));
    positionAlloc4->Add (Vector(798.242960950727,709.1953221404441, 0.0));
    positionAlloc4->Add (Vector(325.91907110781057,795.2286715188251, 0.0));
    positionAlloc4->Add (Vector(524.8498007956615,161.505240552629, 0.0));
    positionAlloc4->Add (Vector(204.23464617461138,649.2972056675653, 0.0));
    positionAlloc4->Add (Vector(827.1701913645852,717.7759600399773, 0.0));
    positionAlloc4->Add (Vector(727.3853940076307,639.7862906701555, 0.0));
    positionAlloc4->Add (Vector(826.938782259817,472.61689368507297, 0.0));
    positionAlloc4->Add (Vector(390.26112247728605,721.6751174931463, 0.0));
    positionAlloc4->Add (Vector(181.57630439973528,419.894016077224, 0.0));
    positionAlloc4->Add (Vector(379.52210304600135,641.3613772022118, 0.0));
    positionAlloc4->Add (Vector(401.8344231877085,742.8990198300879, 0.0));
    positionAlloc4->Add (Vector(331.5560748172464,670.3208455586899, 0.0));
    positionAlloc4->Add (Vector(753.3369638130656,381.5962634108515, 0.0));
    positionAlloc4->Add (Vector(475.03441152726987,428.4709165855811, 0.0));
    positionAlloc4->Add (Vector(629.3057760549325,510.8639411400636, 0.0));
    positionAlloc4->Add (Vector(535.3528985391524,644.533541053582, 0.0));
    positionAlloc4->Add (Vector(477.41258978012934,399.8872641144304, 0.0));
    positionAlloc4->Add (Vector(657.0498543808849,249.20064577330072, 0.0));
    positionAlloc4->Add (Vector(229.21130461277338,225.91053242588333, 0.0));
    positionAlloc4->Add (Vector(582.6355658347607,214.08621502587226, 0.0));
    positionAlloc4->Add (Vector(273.498687621681,655.1609846389101, 0.0));
    positionAlloc4->Add (Vector(628.6144144749987,708.4700257871419, 0.0));
    positionAlloc4->Add (Vector(826.2084672893238,812.9219778463993, 0.0));
    positionAlloc4->Add (Vector(301.4239783132804,504.08322575962166, 0.0));
    positionAlloc4->Add (Vector(468.8347061165776,746.2853811333304, 0.0));
    positionAlloc4->Add (Vector(393.8716420714402,843.8865491215304, 0.0));
    positionAlloc4->Add (Vector(832.6346877500686,826.478104312001, 0.0));
    positionAlloc4->Add (Vector(577.8447263007354,848.3910792659668, 0.0));
    positionAlloc4->Add (Vector(235.91752368101626,528.0752072452536, 0.0));
    positionAlloc4->Add (Vector(431.00671339920547,519.8792883810406, 0.0));
    positionAlloc4->Add (Vector(284.11646477909613,810.3658032963257, 0.0));
    positionAlloc4->Add (Vector(569.1415928935317,822.7870812962021, 0.0));
    positionAlloc4->Add (Vector(737.420967357208,681.4137368143669, 0.0));
    positionAlloc4->Add (Vector(700.9486519067522,586.6053784954283, 0.0));
    positionAlloc4->Add (Vector(819.5561223643215,693.8166970308154, 0.0));
    positionAlloc4->Add (Vector(385.14489713673447,332.461158913218, 0.0));
    positionAlloc4->Add (Vector(249.29487534346947,634.1406434191592, 0.0));
    positionAlloc4->Add (Vector(322.4599790395873,505.1404726255514, 0.0));
    positionAlloc4->Add (Vector(731.9633986000593,493.1410209893868, 0.0));
    positionAlloc4->Add (Vector(491.65843403800125,417.00680870430506, 0.0));
    positionAlloc4->Add (Vector(745.4203838110897,712.396498368714, 0.0));
    positionAlloc4->Add (Vector(729.0250946606089,733.0083292585927, 0.0));
    positionAlloc4->Add (Vector(589.1905778892067,486.79226757922555, 0.0));
    positionAlloc4->Add (Vector(356.28298980756256,535.5714278154937, 0.0));
    positionAlloc4->Add (Vector(669.3068020606146,649.801895966458, 0.0));
    positionAlloc4->Add (Vector(790.5315883012572,362.70302479210545, 0.0));
    positionAlloc4->Add (Vector(832.9036441048913,658.5258876554719, 0.0));
    positionAlloc4->Add (Vector(227.4212260378039,298.4910496094277, 0.0));
    positionAlloc4->Add (Vector(821.6873900099299,662.8799839777608, 0.0));
    positionAlloc4->Add (Vector(457.3662351003128,490.6270305825676, 0.0));
    positionAlloc4->Add (Vector(575.6708307964642,586.8199060209697, 0.0));
    positionAlloc4->Add (Vector(413.98194492017615,303.986143428727, 0.0));
    positionAlloc4->Add (Vector(496.16081987082293,789.089784501706, 0.0));
    positionAlloc4->Add (Vector(691.9555338092092,574.1362602493075, 0.0));
    positionAlloc4->Add (Vector(573.1938652229662,432.7209821265233, 0.0));
    positionAlloc4->Add (Vector(796.0494356217779,295.4110575412447, 0.0));
    positionAlloc4->Add (Vector(801.6111650401642,521.4816987437107, 0.0));
    positionAlloc4->Add (Vector(733.8048391513701,269.77793333620076, 0.0));
    positionAlloc4->Add (Vector(377.60321236180516,592.9005870106942, 0.0));
    positionAlloc4->Add (Vector(221.68918618875895,599.8860915836349, 0.0));
    positionAlloc4->Add (Vector(383.8552268736723,532.7614181401585, 0.0));
    positionAlloc4->Add (Vector(639.3971807905593,345.501247055916, 0.0));
    positionAlloc4->Add (Vector(191.0406913690308,158.08939670992993, 0.0));
    positionAlloc4->Add (Vector(666.546756235195,797.8161145949732, 0.0));
    positionAlloc4->Add (Vector(406.11019220428295,705.5654867250609, 0.0));
    positionAlloc4->Add (Vector(260.313225509528,315.5387901459279, 0.0));
    positionAlloc4->Add (Vector(673.6845600776484,282.2499888140861, 0.0));
    positionAlloc4->Add (Vector(653.4150672452226,617.9592270274752, 0.0));
    positionAlloc4->Add (Vector(682.7051031301397,280.4148427219917, 0.0));
    positionAlloc4->Add (Vector(229.62011358960703,730.3480523512635, 0.0));
    positionAlloc4->Add (Vector(374.065176352886,640.2695932003054, 0.0));
    positionAlloc4->Add (Vector(244.6600578826366,370.3605782938257, 0.0));
    positionAlloc4->Add (Vector(634.5787727171332,471.5131352939022, 0.0));
    positionAlloc4->Add (Vector(262.16014321540354,641.4794851201798, 0.0));
    positionAlloc4->Add (Vector(245.8410547338234,657.4391999966402, 0.0));
    positionAlloc4->Add (Vector(254.97974162444802,380.78642069353623, 0.0));
    positionAlloc4->Add (Vector(606.5795624414809,504.9304758556968, 0.0));
    positionAlloc4->Add (Vector(674.73480889455,680.2757507580604, 0.0));
    positionAlloc4->Add (Vector(247.1891497835897,766.8489127258596, 0.0));
    positionAlloc4->Add (Vector(231.76862176848587,411.6134617820272, 0.0));
    positionAlloc4->Add (Vector(696.1424477997331,600.6930419680828, 0.0));
    positionAlloc4->Add (Vector(627.177113078692,804.0310057518611, 0.0));
    positionAlloc4->Add (Vector(227.81853292768886,563.1506649400249, 0.0));
    positionAlloc4->Add (Vector(403.26515762484314,845.921654177214, 0.0));
    positionAlloc4->Add (Vector(504.4563774932524,493.14199794393977, 0.0));
    positionAlloc4->Add (Vector(767.373699482308,746.4488864799898, 0.0));
    positionAlloc4->Add (Vector(748.8272703812738,185.11819044908208, 0.0));
    positionAlloc4->Add (Vector(474.3684401064238,456.3566553070956, 0.0));
    positionAlloc4->Add (Vector(384.4134054356736,624.7255053884465, 0.0));
    positionAlloc4->Add (Vector(498.955226566871,581.0848608251025, 0.0));
    positionAlloc4->Add (Vector(695.9410453737869,824.2103901792715, 0.0));
    positionAlloc4->Add (Vector(355.0339728383517,661.7917762750965, 0.0));
    positionAlloc4->Add (Vector(262.2513739227302,776.1257655205634, 0.0));
    positionAlloc4->Add (Vector(648.8347555438253,507.8124172289218, 0.0));
    positionAlloc4->Add (Vector(468.97856655092147,381.70303518822703, 0.0));
    positionAlloc4->Add (Vector(181.032575561597,304.7461055330517, 0.0));
    positionAlloc4->Add (Vector(513.2968195025155,620.7579585081842, 0.0));
    positionAlloc4->Add (Vector(310.2007930764479,283.6782284314746, 0.0));
    positionAlloc4->Add (Vector(369.38741035373744,781.7174774438645, 0.0));
    positionAlloc4->Add (Vector(472.5195520828201,818.8366263465809, 0.0));
    positionAlloc4->Add (Vector(199.67524329987484,382.244256303253, 0.0));
    positionAlloc4->Add (Vector(312.81551928303134,390.03185048561716, 0.0));
    positionAlloc4->Add (Vector(652.109406181274,426.7019227487269, 0.0));
    positionAlloc4->Add (Vector(725.3421664121332,412.708652761184, 0.0));
    positionAlloc4->Add (Vector(780.5474001318978,784.5714424836425, 0.0));
    positionAlloc4->Add (Vector(532.0440072574563,473.386577966487, 0.0));
    positionAlloc4->Add (Vector(211.7640276999441,437.54722003660436, 0.0));
    positionAlloc4->Add (Vector(533.7511374584444,527.6525133150042, 0.0));
    positionAlloc4->Add (Vector(589.3247049324445,688.9006039752254, 0.0));
    positionAlloc4->Add (Vector(663.3202628861343,433.8300310208851, 0.0));
    positionAlloc4->Add (Vector(626.8239672493693,410.87326684708654, 0.0));
    positionAlloc4->Add (Vector(240.96843285384472,241.22687949208478, 0.0));
    positionAlloc4->Add (Vector(842.6942972803978,231.0842608389588, 0.0));
    positionAlloc4->Add (Vector(511.7939194155482,772.458824109491, 0.0));
    positionAlloc4->Add (Vector(505.8429672095455,602.8759788149162, 0.0));
    positionAlloc4->Add (Vector(767.5886615986509,253.53779544187014, 0.0));
    positionAlloc4->Add (Vector(691.7725598762471,467.62426775628603, 0.0));
    positionAlloc4->Add (Vector(279.42817665623954,364.0162318102366, 0.0));
    positionAlloc4->Add (Vector(526.1419118891004,473.44186132624117, 0.0));
    positionAlloc4->Add (Vector(767.3236754636607,674.7104030537708, 0.0));
    positionAlloc4->Add (Vector(427.7960292393048,843.4868961294263, 0.0));
    positionAlloc4->Add (Vector(575.4059234859884,553.1373271921775, 0.0));
    positionAlloc4->Add (Vector(546.3337611781849,333.33692431277257, 0.0));
    positionAlloc4->Add (Vector(783.7456316289258,553.3297887550907, 0.0));
    positionAlloc4->Add (Vector(455.3869943630939,453.33942125782613, 0.0));
    positionAlloc4->Add (Vector(512.2258875576085,740.3527902103926, 0.0));
    positionAlloc4->Add (Vector(410.9439320525007,319.8860789190142, 0.0));
    positionAlloc4->Add (Vector(547.8485321906345,226.05414539305133, 0.0));
    positionAlloc4->Add (Vector(411.7995569179172,717.2869529145211, 0.0));
    positionAlloc4->Add (Vector(581.937972272947,244.7674303166192, 0.0));
    positionAlloc4->Add (Vector(239.70848919419288,714.0150753134858, 0.0));
    positionAlloc4->Add (Vector(478.12574230408984,499.576919419746, 0.0));
    positionAlloc4->Add (Vector(307.75369772422664,612.9448880581441, 0.0));
    positionAlloc4->Add (Vector(759.7924847575176,279.59861952942487, 0.0));
    positionAlloc4->Add (Vector(614.5992767641358,193.70399881960753, 0.0));
    positionAlloc4->Add (Vector(260.013976094225,541.3045772901231, 0.0));
    positionAlloc4->Add (Vector(284.56240316894474,464.97170345802664, 0.0));
    positionAlloc4->Add (Vector(380.30882964875286,459.41932590129665, 0.0));
    positionAlloc4->Add (Vector(652.3846173789326,250.20941816485038, 0.0));
    positionAlloc4->Add (Vector(833.49093699168,319.42799494323265, 0.0));
    positionAlloc4->Add (Vector(447.8208328441038,257.7040786176859, 0.0));
    positionAlloc4->Add (Vector(466.9254563006251,694.5902831043551, 0.0));
    positionAlloc4->Add (Vector(408.7230083296352,574.7233832278714, 0.0));
    positionAlloc4->Add (Vector(667.8564948605011,156.8116721088661, 0.0));
    positionAlloc4->Add (Vector(150.8704072062714,685.9292988155156, 0.0));
    positionAlloc4->Add (Vector(808.5227061543662,675.3460691465676, 0.0));
    positionAlloc4->Add (Vector(412.46340206706475,243.16064742095818, 0.0));
    positionAlloc4->Add (Vector(568.9523898361759,411.8740161215539, 0.0));
    positionAlloc4->Add (Vector(304.2247200986998,425.8793684589099, 0.0));
    positionAlloc4->Add (Vector(568.179223831007,630.6066496149015, 0.0));
    positionAlloc4->Add (Vector(549.8832799098336,209.61701669738943, 0.0));
    positionAlloc4->Add (Vector(646.1999792429923,823.9505561796491, 0.0));
    positionAlloc4->Add (Vector(302.0951451369955,731.7047522928422, 0.0));
    positionAlloc4->Add (Vector(756.5141464439849,786.6861135425709, 0.0));
    positionAlloc4->Add (Vector(814.9726306138241,838.6873059785422, 0.0));
    positionAlloc4->Add (Vector(492.7139499389202,827.0148385952365, 0.0));
    positionAlloc4->Add (Vector(392.12305290741625,582.622738926796, 0.0));
    positionAlloc4->Add (Vector(768.2070904602823,738.8499018577372, 0.0));
    positionAlloc4->Add (Vector(188.76539390357055,185.45172851060843, 0.0));
    positionAlloc4->Add (Vector(840.2689594361393,562.6102544229743, 0.0));
    positionAlloc4->Add (Vector(708.5627396246957,550.0139960236551, 0.0));
    positionAlloc4->Add (Vector(728.883814054663,156.69319899533477, 0.0));
    positionAlloc4->Add (Vector(412.81578404004875,397.22142354679386, 0.0));
    positionAlloc4->Add (Vector(230.31517288264027,572.2976236154119, 0.0));
    positionAlloc4->Add (Vector(306.73569207878006,470.4245571675395, 0.0));
    positionAlloc4->Add (Vector(265.6040984769821,819.6620618969494, 0.0));
    positionAlloc4->Add (Vector(665.3663177005825,662.5522920370029, 0.0));
    positionAlloc4->Add (Vector(517.1632598590667,159.21394000232831, 0.0));
    positionAlloc4->Add (Vector(414.84752228333844,683.4269243723525, 0.0));
    positionAlloc4->Add (Vector(823.0296191673244,524.0551002015654, 0.0));
    positionAlloc4->Add (Vector(190.87735684140773,302.611548972357, 0.0));
    positionAlloc4->Add (Vector(613.6601370773742,604.1781135504298, 0.0));
    positionAlloc4->Add (Vector(731.6693242895592,562.2808122134395, 0.0));
    positionAlloc4->Add (Vector(294.9005820906316,842.327469229057, 0.0));
    positionAlloc4->Add (Vector(595.2012627922725,812.3373987639958, 0.0));
    positionAlloc4->Add (Vector(603.429032900203,647.120125182101, 0.0));
    positionAlloc4->Add (Vector(383.76746630257287,737.9006023797099, 0.0));
    positionAlloc4->Add (Vector(809.1863820511616,374.00342003294367, 0.0));
    positionAlloc4->Add (Vector(375.98644441191686,268.26086401610945, 0.0));
    positionAlloc4->Add (Vector(401.31583168306025,223.63179941528017, 0.0));
    positionAlloc4->Add (Vector(158.5178417674232,591.1671686680331, 0.0));
    positionAlloc4->Add (Vector(259.02285229741096,239.6549562894155, 0.0));
    positionAlloc4->Add (Vector(251.2398366864132,502.72803880480785, 0.0));
    positionAlloc4->Add (Vector(194.19111222711715,589.4299540831175, 0.0));
    positionAlloc4->Add (Vector(690.5584613699756,798.5818790049392, 0.0));
    positionAlloc4->Add (Vector(337.3537136567944,772.2665789834481, 0.0));
    positionAlloc4->Add (Vector(819.4805456033916,336.5268588464419, 0.0));
    positionAlloc4->Add (Vector(675.7626713096702,695.0072941044504, 0.0));
    positionAlloc4->Add (Vector(503.61054303971673,748.1005707472112, 0.0));
    positionAlloc4->Add (Vector(717.0886445083714,687.3242136207622, 0.0));
    positionAlloc4->Add (Vector(579.0901034445819,477.728727220137, 0.0));
    positionAlloc4->Add (Vector(418.932406437786,530.8086039541769, 0.0));
    positionAlloc4->Add (Vector(687.6629336533981,295.66105039507715, 0.0));
    positionAlloc4->Add (Vector(421.8854837103663,821.9894357663727, 0.0));
    positionAlloc4->Add (Vector(404.6828971878698,452.42989946853345, 0.0));
    positionAlloc4->Add (Vector(695.368557855219,500.6505830915538, 0.0));
    positionAlloc4->Add (Vector(222.97023921934425,625.2048623598595, 0.0));
    positionAlloc4->Add (Vector(568.6128553856457,270.2713248108939, 0.0));
    positionAlloc4->Add (Vector(329.87819772948956,364.9844864104426, 0.0));
    positionAlloc4->Add (Vector(695.8327386953854,317.3251602048643, 0.0));
    positionAlloc4->Add (Vector(316.0173769090175,634.7118368690828, 0.0));
    positionAlloc4->Add (Vector(636.4486875080715,262.18661932841087, 0.0));
    positionAlloc4->Add (Vector(188.9663022393394,849.2600733863625, 0.0));
    positionAlloc4->Add (Vector(804.2237716540773,707.6203195488639, 0.0));
    positionAlloc4->Add (Vector(477.62428504590895,299.41453257122646, 0.0));
    positionAlloc4->Add (Vector(404.3683596975475,751.3543861152399, 0.0));
    positionAlloc4->Add (Vector(485.4696625275314,351.16594067889787, 0.0));
    positionAlloc4->Add (Vector(502.8497026996713,659.1474619531716, 0.0));
    positionAlloc4->Add (Vector(443.78644644971797,608.203559921897, 0.0));
    positionAlloc4->Add (Vector(269.5124532791348,759.2293155932845, 0.0));
    positionAlloc4->Add (Vector(595.2955712996917,281.2696335625926, 0.0));
    positionAlloc4->Add (Vector(253.44889343482242,671.8975662147458, 0.0));
    positionAlloc4->Add (Vector(191.4739166798093,820.3230597813197, 0.0));
    positionAlloc4->Add (Vector(734.200369809663,611.63577480075, 0.0));
    positionAlloc4->Add (Vector(410.32859340245886,472.7373313690318, 0.0));
    positionAlloc4->Add (Vector(795.2584295524563,465.48180202363204, 0.0));
    positionAlloc4->Add (Vector(730.0549369738205,242.1158539360571, 0.0));
    positionAlloc4->Add (Vector(182.74400513307933,831.9905683695622, 0.0));
    positionAlloc4->Add (Vector(749.6887561892981,536.3819366902458, 0.0));
    positionAlloc4->Add (Vector(211.38769641361654,482.8990537275575, 0.0));
    positionAlloc4->Add (Vector(816.8101869221933,334.87727312535304, 0.0));
    positionAlloc4->Add (Vector(180.0385908621359,653.6338252235253, 0.0));
    positionAlloc4->Add (Vector(254.32954642264525,392.28997805011153, 0.0));
    positionAlloc4->Add (Vector(629.5316229960338,620.0734703382402, 0.0));
    positionAlloc4->Add (Vector(590.3792004363013,697.5603003304864, 0.0));
    positionAlloc4->Add (Vector(258.75924915578247,440.06821854939363, 0.0));
    positionAlloc4->Add (Vector(212.77477966156607,599.9134816892131, 0.0));
    positionAlloc4->Add (Vector(610.8499645297072,509.97799534338253, 0.0));
    positionAlloc4->Add (Vector(533.9772944498322,701.4832317298477, 0.0));
    positionAlloc4->Add (Vector(206.15700426072098,807.6035360273312, 0.0));
    positionAlloc4->Add (Vector(435.8930952490848,361.1224964080152, 0.0));
    positionAlloc4->Add (Vector(686.1061908571656,364.03117620898917, 0.0));
    positionAlloc4->Add (Vector(251.31305333665057,364.52041194531546, 0.0));
    positionAlloc4->Add (Vector(731.7374086281368,219.22358798762187, 0.0));
    positionAlloc4->Add (Vector(619.9308721706132,474.38248317022413, 0.0));
    positionAlloc4->Add (Vector(270.5319692890339,197.06004489956734, 0.0));
    positionAlloc4->Add (Vector(233.920109576803,298.13072914456706, 0.0));
    positionAlloc4->Add (Vector(218.96055142797368,542.4597974504773, 0.0));
    positionAlloc4->Add (Vector(353.3522730390333,611.7931375029643, 0.0));
    positionAlloc4->Add (Vector(164.4632961154021,217.83590427446774, 0.0));
    positionAlloc4->Add (Vector(564.8746461396219,400.88156708567163, 0.0));
    positionAlloc4->Add (Vector(253.40646987799215,370.4366832687014, 0.0));
    positionAlloc4->Add (Vector(359.78848196600313,266.4539070182053, 0.0));
    positionAlloc4->Add (Vector(223.29360980369506,651.7224680370543, 0.0));
    positionAlloc4->Add (Vector(668.7600584167537,373.3976608739353, 0.0));
    positionAlloc4->Add (Vector(284.7743724066413,563.0087277915871, 0.0));
    positionAlloc4->Add (Vector(699.2731865102143,309.71713406640913, 0.0));
    positionAlloc4->Add (Vector(663.7487065427405,691.7864173868372, 0.0));
    positionAlloc4->Add (Vector(270.51735182511567,490.3999006459426, 0.0));
    positionAlloc4->Add (Vector(732.2047841959427,746.8357062332818, 0.0));
    positionAlloc4->Add (Vector(407.64879365210913,619.9338217721039, 0.0));
    positionAlloc4->Add (Vector(219.50534240776256,641.2283405809488, 0.0));
    positionAlloc4->Add (Vector(579.5089293991136,186.51006159733046, 0.0));
    positionAlloc4->Add (Vector(632.0196644728467,378.30480198355315, 0.0));
    positionAlloc4->Add (Vector(187.74167865283908,551.3196212000655, 0.0));
    positionAlloc4->Add (Vector(347.85895693014123,278.73843586297755, 0.0));
    positionAlloc4->Add (Vector(535.2355484333591,700.2912957235272, 0.0));
    positionAlloc4->Add (Vector(550.3130015296662,655.028676510041, 0.0));
    positionAlloc4->Add (Vector(181.01686786682836,374.4157349562224, 0.0));
    positionAlloc4->Add (Vector(843.13894332365,723.558872554051, 0.0));
    positionAlloc4->Add (Vector(773.448270396432,585.8049431549091, 0.0));
    positionAlloc4->Add (Vector(619.4814352645628,464.3913961254437, 0.0));
    positionAlloc4->Add (Vector(688.110459608457,680.4693139859862, 0.0));
    positionAlloc4->Add (Vector(552.2964420073027,542.4500476021904, 0.0));
    positionAlloc4->Add (Vector(307.7814803381601,318.2259379443068, 0.0));
    positionAlloc4->Add (Vector(407.5997630136672,619.4240668049417, 0.0));
    positionAlloc4->Add (Vector(264.99359613158015,789.7455393464128, 0.0));
    positionAlloc4->Add (Vector(195.9833005384527,678.9253994512587, 0.0));
    positionAlloc4->Add (Vector(350.2614447434783,229.04861390221254, 0.0));
    positionAlloc4->Add (Vector(176.58755467213666,295.9403447199527, 0.0));
    positionAlloc4->Add (Vector(764.0110957890371,225.30509189970388, 0.0));
    positionAlloc4->Add (Vector(612.131598248501,380.9510145897324, 0.0));
    positionAlloc4->Add (Vector(343.4655417211296,414.1000070598679, 0.0));
    positionAlloc4->Add (Vector(767.0717336294043,790.694585238984, 0.0));
    positionAlloc4->Add (Vector(275.10409044412825,207.64254702205045, 0.0));
    positionAlloc4->Add (Vector(635.493719179329,797.3176956396245, 0.0));
    positionAlloc4->Add (Vector(291.93844655818737,354.96899857334904, 0.0));
    positionAlloc4->Add (Vector(838.5188302147077,548.3512369130516, 0.0));
    positionAlloc4->Add (Vector(627.5282163069446,457.7630726594134, 0.0));
    positionAlloc4->Add (Vector(718.2675386074363,698.2650032145737, 0.0));
    positionAlloc4->Add (Vector(619.6003923314868,662.2179815099236, 0.0));
    positionAlloc4->Add (Vector(371.0497684079153,695.8898067135218, 0.0));
    positionAlloc4->Add (Vector(649.7240285296791,159.18548789314346, 0.0));
    positionAlloc4->Add (Vector(373.2480008023226,452.5170786217901, 0.0));
    positionAlloc4->Add (Vector(452.795149487902,266.50245366371996, 0.0));
    positionAlloc4->Add (Vector(663.6152082084493,777.9913264176555, 0.0));
    positionAlloc4->Add (Vector(590.6913391966025,612.0836109841913, 0.0));
    positionAlloc4->Add (Vector(655.0259202508443,378.87089418912353, 0.0));
    positionAlloc4->Add (Vector(187.07648392679707,411.5324406172156, 0.0));
    positionAlloc4->Add (Vector(658.7292401787652,363.28743806040256, 0.0));
    positionAlloc4->Add (Vector(389.08586229857355,366.51536583970835, 0.0));
    positionAlloc4->Add (Vector(309.38909939807814,777.1363971120436, 0.0));
    positionAlloc4->Add (Vector(761.9904186140616,768.0197146516289, 0.0));
    positionAlloc4->Add (Vector(406.82696330576255,674.1413252752654, 0.0));
    positionAlloc4->Add (Vector(437.2139294919406,503.23727805218834, 0.0));
    positionAlloc4->Add (Vector(496.56471499520933,452.35339635443364, 0.0));
    positionAlloc4->Add (Vector(477.1510792332433,656.5755576649653, 0.0));
    positionAlloc4->Add (Vector(630.661911788306,846.4649902853592, 0.0));
    positionAlloc4->Add (Vector(405.7981162115731,277.15026559040126, 0.0));
    positionAlloc4->Add (Vector(684.2726014599211,575.8855453576357, 0.0));
    positionAlloc4->Add (Vector(760.3397921310456,830.8394189774932, 0.0));
    positionAlloc4->Add (Vector(656.9347126579084,720.9311407429193, 0.0));
    positionAlloc4->Add (Vector(446.7388112413426,790.5985176621763, 0.0));
    positionAlloc4->Add (Vector(323.527880925613,216.76166552861926, 0.0));
    positionAlloc4->Add (Vector(182.56294744154806,513.8847570085943, 0.0));
    positionAlloc4->Add (Vector(504.25172892669707,209.64986083123964, 0.0));
    positionAlloc4->Add (Vector(442.43247013717956,261.622068450737, 0.0));
    positionAlloc4->Add (Vector(228.28786338912866,369.32948250592085, 0.0));
    positionAlloc4->Add (Vector(415.73852648889795,585.932834638132, 0.0));
    positionAlloc4->Add (Vector(246.4498644725678,179.20735499745228, 0.0));
    positionAlloc4->Add (Vector(203.51080626519774,556.1364322735839, 0.0));
    positionAlloc4->Add (Vector(342.7629770324612,161.32926825232383, 0.0));
    positionAlloc4->Add (Vector(174.85235810304266,815.5887239603779, 0.0));
    positionAlloc4->Add (Vector(532.9274005344591,523.0199456486088, 0.0));
    positionAlloc4->Add (Vector(239.46408995664905,396.964206486134, 0.0));
    positionAlloc4->Add (Vector(829.7738558553282,551.7422107717052, 0.0));
    positionAlloc4->Add (Vector(410.69872582317777,227.7324711043288, 0.0));
    positionAlloc4->Add (Vector(421.44821021246526,317.84581719083326, 0.0));
    positionAlloc4->Add (Vector(727.8058409234989,465.4179903725199, 0.0));
    positionAlloc4->Add (Vector(627.209905938594,788.2421829169, 0.0));
    positionAlloc4->Add (Vector(379.74122537633224,477.44338549022433, 0.0));
    positionAlloc4->Add (Vector(189.56023707164363,624.3716423376327, 0.0));
    positionAlloc4->Add (Vector(575.2348761521828,416.31111623617284, 0.0));
    positionAlloc4->Add (Vector(194.6063286821028,742.1733668871691, 0.0));
    positionAlloc4->Add (Vector(758.1834308911252,758.0857264291964, 0.0));
    positionAlloc4->Add (Vector(731.5549059873518,806.9265238324342, 0.0));
    positionAlloc4->Add (Vector(604.8129941301735,780.8461150891287, 0.0));
    positionAlloc4->Add (Vector(291.8703344151133,617.2701727569187, 0.0));
    positionAlloc4->Add (Vector(559.6281990488894,675.83414551597, 0.0));
    positionAlloc4->Add (Vector(758.4280047863573,848.5365252532004, 0.0));
    positionAlloc4->Add (Vector(207.5903234733445,774.4178983209814, 0.0));
    positionAlloc4->Add (Vector(439.4617980650192,460.00081670559337, 0.0));
    positionAlloc4->Add (Vector(772.3606083288059,447.87215136061826, 0.0));
    positionAlloc4->Add (Vector(241.88354776500967,617.1790371925654, 0.0));
    positionAlloc4->Add (Vector(726.246959526445,646.093640384302, 0.0));
    positionAlloc4->Add (Vector(253.409814095402,716.7566324493163, 0.0));
    positionAlloc4->Add (Vector(171.52705835680143,804.0621782677509, 0.0));
    positionAlloc4->Add (Vector(744.4810385497526,246.983209711286, 0.0));
    positionAlloc4->Add (Vector(741.1894629985122,621.7868906215855, 0.0));
    positionAlloc4->Add (Vector(464.24869648822613,192.7720695406225, 0.0));
    positionAlloc4->Add (Vector(532.9805758300449,767.1535682023674, 0.0));
    positionAlloc4->Add (Vector(665.9181340532477,613.5732832650363, 0.0));
    positionAlloc4->Add (Vector(475.6675206259552,640.7470281769142, 0.0));
    positionAlloc4->Add (Vector(716.7054826404893,815.2271772770913, 0.0));
    positionAlloc4->Add (Vector(640.3548969752162,327.1590822549988, 0.0));
    positionAlloc4->Add (Vector(505.44410024869524,588.1062767307576, 0.0));
    positionAlloc4->Add (Vector(740.1304056701924,647.6129393084797, 0.0));
    positionAlloc4->Add (Vector(467.11680952084447,690.1485160367112, 0.0));
    positionAlloc4->Add (Vector(633.6819058060947,770.8537507964425, 0.0));
    positionAlloc4->Add (Vector(650.6605028434087,819.9745869992227, 0.0));
    positionAlloc4->Add (Vector(483.1831780618223,338.88632337251437, 0.0));
    positionAlloc4->Add (Vector(380.8794805974155,193.43573684480293, 0.0));
    positionAlloc4->Add (Vector(518.0521904748825,772.5030154503689, 0.0));
    positionAlloc4->Add (Vector(295.6326579863885,763.2978492788051, 0.0));
    positionAlloc4->Add (Vector(726.41749512928,672.0187955859819, 0.0));
    positionAlloc4->Add (Vector(614.586597238668,843.5335088775657, 0.0));
    positionAlloc4->Add (Vector(554.9582546866118,275.2732719945007, 0.0));
    positionAlloc4->Add (Vector(389.4537571797496,801.5175774536889, 0.0));
    positionAlloc4->Add (Vector(763.4142207667558,340.55316058784297, 0.0));
    positionAlloc4->Add (Vector(480.3363769402113,651.7073478770154, 0.0));
    positionAlloc4->Add (Vector(601.9834679146768,431.69294597988744, 0.0));
    positionAlloc4->Add (Vector(634.4214866071793,725.0779026479565, 0.0));
    positionAlloc4->Add (Vector(682.3725414731055,657.984294240322, 0.0));
    positionAlloc4->Add (Vector(659.4779684217607,585.2992598392032, 0.0));
    positionAlloc4->Add (Vector(162.82831798163906,531.6774123833793, 0.0));
    positionAlloc4->Add (Vector(357.32202022112165,759.644462594929, 0.0));
    positionAlloc4->Add (Vector(646.0224670384681,173.03755786845747, 0.0));
    positionAlloc4->Add (Vector(467.6192313660111,746.0045265002324, 0.0));
    positionAlloc4->Add (Vector(666.9849120590766,485.5466024510833, 0.0));
    positionAlloc4->Add (Vector(656.6800772760616,280.7378913890062, 0.0));
    positionAlloc4->Add (Vector(473.3435053874462,254.08952280828376, 0.0));
    positionAlloc4->Add (Vector(457.1160452572884,709.6922213695602, 0.0));
    positionAlloc4->Add (Vector(527.6660604588548,278.0984016214692, 0.0));
    positionAlloc4->Add (Vector(527.3631015155732,289.0080847582584, 0.0));
    positionAlloc4->Add (Vector(195.1063378868846,663.6810835950191, 0.0));
    positionAlloc4->Add (Vector(183.28332621681443,598.8504931645173, 0.0));
    positionAlloc4->Add (Vector(450.8920205944451,253.72441123063157, 0.0));
    positionAlloc4->Add (Vector(739.701561505313,430.86099972818425, 0.0));
    positionAlloc4->Add (Vector(527.3623036447557,707.7410023988919, 0.0));
    positionAlloc4->Add (Vector(453.6656255628287,401.27930078090606, 0.0));
    positionAlloc4->Add (Vector(321.993564739273,156.36592621169348, 0.0));
    positionAlloc4->Add (Vector(203.62203585074712,822.8642927245322, 0.0));
    positionAlloc4->Add (Vector(376.2727487608049,806.7282064311563, 0.0));
    positionAlloc4->Add (Vector(769.4394200483955,678.6655718399663, 0.0));
    positionAlloc4->Add (Vector(197.37261425791996,407.8215539382879, 0.0));
    positionAlloc4->Add (Vector(682.6743224325661,522.0272471701891, 0.0));
    positionAlloc4->Add (Vector(449.6943912977661,334.5021417409687, 0.0));
    positionAlloc4->Add (Vector(434.90755456612567,551.7379842233785, 0.0));
    positionAlloc4->Add (Vector(425.9693914844415,364.6717801816533, 0.0));
    positionAlloc4->Add (Vector(582.6155950770587,552.8086485714657, 0.0));
    positionAlloc4->Add (Vector(601.101345460233,410.2606984021015, 0.0));
    positionAlloc4->Add (Vector(825.1214877956587,765.3223803135297, 0.0));
    positionAlloc4->Add (Vector(609.5292757208599,476.3337254302569, 0.0));
    positionAlloc4->Add (Vector(738.4770560323075,700.8717780749379, 0.0));
    positionAlloc4->Add (Vector(550.4964230157475,240.9249784091288, 0.0));
    positionAlloc4->Add (Vector(678.4487856463862,512.0091793422706, 0.0));
    positionAlloc4->Add (Vector(426.86186017529,747.5869997867277, 0.0));
    positionAlloc4->Add (Vector(266.59637505908427,184.2382776949409, 0.0));
    positionAlloc4->Add (Vector(407.3826657754472,697.3959998577423, 0.0));
    positionAlloc4->Add (Vector(542.5229545116354,611.1443749413932, 0.0));
    positionAlloc4->Add (Vector(188.4705122550465,808.6696348812544, 0.0));
    positionAlloc4->Add (Vector(338.48222152968435,767.438082995138, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_21.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_ECC_flowmon/switch_ECC_21.flowmon", false, false);
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