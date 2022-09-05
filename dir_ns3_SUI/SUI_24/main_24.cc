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
	  uint16_t numberOfRrhs = 27;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 800;
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
    positionAlloc->Add (Vector (772.2222222222222,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,305.55555555555554, 0.0));
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
    positionAlloc4->Add (Vector(721.8304162794184,483.7393644244982, 0.0));
    positionAlloc4->Add (Vector(650.0556133631301,523.8065221150325, 0.0));
    positionAlloc4->Add (Vector(839.2881726300847,300.9625693825569, 0.0));
    positionAlloc4->Add (Vector(796.0867929139034,451.6557122492533, 0.0));
    positionAlloc4->Add (Vector(681.2447265343864,724.0292538390823, 0.0));
    positionAlloc4->Add (Vector(665.4421038825287,599.2264539920848, 0.0));
    positionAlloc4->Add (Vector(458.3385559377747,399.8404310300606, 0.0));
    positionAlloc4->Add (Vector(350.5253720015153,345.1856292474873, 0.0));
    positionAlloc4->Add (Vector(566.1248339690653,799.9792343991671, 0.0));
    positionAlloc4->Add (Vector(171.54658222746616,560.1175714927144, 0.0));
    positionAlloc4->Add (Vector(461.529475257168,699.5653145485943, 0.0));
    positionAlloc4->Add (Vector(367.1724972813274,205.59658974280353, 0.0));
    positionAlloc4->Add (Vector(411.46723946866797,756.9105549304062, 0.0));
    positionAlloc4->Add (Vector(764.2254647702446,517.2347711335624, 0.0));
    positionAlloc4->Add (Vector(467.26251556994913,597.3122002487339, 0.0));
    positionAlloc4->Add (Vector(769.0703172177008,326.5197739656302, 0.0));
    positionAlloc4->Add (Vector(658.2051692586372,415.98244712088575, 0.0));
    positionAlloc4->Add (Vector(290.4524862225761,195.66831792283824, 0.0));
    positionAlloc4->Add (Vector(179.34844806458847,292.88559919066836, 0.0));
    positionAlloc4->Add (Vector(713.0587375057681,324.54824108774176, 0.0));
    positionAlloc4->Add (Vector(321.7534391905051,689.6269782957073, 0.0));
    positionAlloc4->Add (Vector(638.4738901604172,722.5126100295892, 0.0));
    positionAlloc4->Add (Vector(660.197358911636,379.94924378847935, 0.0));
    positionAlloc4->Add (Vector(152.64472964243393,833.7805253130963, 0.0));
    positionAlloc4->Add (Vector(189.35744107693523,813.5256758792725, 0.0));
    positionAlloc4->Add (Vector(722.666426532116,277.4473285946172, 0.0));
    positionAlloc4->Add (Vector(512.7766179892469,434.37798649947933, 0.0));
    positionAlloc4->Add (Vector(748.637064337885,554.0240367599398, 0.0));
    positionAlloc4->Add (Vector(832.2361817652959,741.9233565688776, 0.0));
    positionAlloc4->Add (Vector(829.623881340683,728.9318437999808, 0.0));
    positionAlloc4->Add (Vector(195.18346351671826,605.9379586086573, 0.0));
    positionAlloc4->Add (Vector(243.38453689474932,378.0725153739782, 0.0));
    positionAlloc4->Add (Vector(441.7785527473964,621.9681964650364, 0.0));
    positionAlloc4->Add (Vector(680.9814446276296,195.06887415007657, 0.0));
    positionAlloc4->Add (Vector(780.1515516664246,497.75388090078707, 0.0));
    positionAlloc4->Add (Vector(225.19714155595338,290.5195282160203, 0.0));
    positionAlloc4->Add (Vector(186.02669362317835,519.0082127064437, 0.0));
    positionAlloc4->Add (Vector(713.41984859407,774.6639802153098, 0.0));
    positionAlloc4->Add (Vector(445.9624307141729,703.439137568154, 0.0));
    positionAlloc4->Add (Vector(440.94909665348115,200.38735593950014, 0.0));
    positionAlloc4->Add (Vector(492.9205082028386,543.103015638224, 0.0));
    positionAlloc4->Add (Vector(747.8107906441589,820.9872297188386, 0.0));
    positionAlloc4->Add (Vector(552.7272051201446,641.6460226055747, 0.0));
    positionAlloc4->Add (Vector(786.456750684744,787.9157142137996, 0.0));
    positionAlloc4->Add (Vector(252.85659716220886,389.3484132046448, 0.0));
    positionAlloc4->Add (Vector(373.5601665067832,601.7141503123237, 0.0));
    positionAlloc4->Add (Vector(614.190116770931,708.1199008606376, 0.0));
    positionAlloc4->Add (Vector(512.1341489356466,311.5487457595726, 0.0));
    positionAlloc4->Add (Vector(522.1962976067367,723.9303009891207, 0.0));
    positionAlloc4->Add (Vector(466.7290352900397,507.5480933608358, 0.0));
    positionAlloc4->Add (Vector(493.8910426731303,267.1609960780952, 0.0));
    positionAlloc4->Add (Vector(431.1330018554659,439.27570226329345, 0.0));
    positionAlloc4->Add (Vector(698.9843641066964,341.83346573855476, 0.0));
    positionAlloc4->Add (Vector(659.0509816130497,736.1000444948328, 0.0));
    positionAlloc4->Add (Vector(688.7651419490605,334.9996548487374, 0.0));
    positionAlloc4->Add (Vector(491.3205749790942,316.74983698040336, 0.0));
    positionAlloc4->Add (Vector(234.67043322816642,841.3940668013386, 0.0));
    positionAlloc4->Add (Vector(794.3554526406077,204.44964014032757, 0.0));
    positionAlloc4->Add (Vector(795.1146170093226,791.6363615746789, 0.0));
    positionAlloc4->Add (Vector(744.0103324400038,532.9130303436914, 0.0));
    positionAlloc4->Add (Vector(559.4447663073807,210.31668538099052, 0.0));
    positionAlloc4->Add (Vector(206.4279019263523,686.7448971732703, 0.0));
    positionAlloc4->Add (Vector(258.54180498868936,528.0653947684002, 0.0));
    positionAlloc4->Add (Vector(278.3686834456167,647.4951533084941, 0.0));
    positionAlloc4->Add (Vector(165.28231344669618,845.5410993477609, 0.0));
    positionAlloc4->Add (Vector(164.6030308461863,578.3866044711586, 0.0));
    positionAlloc4->Add (Vector(315.8024966082513,160.05914414872427, 0.0));
    positionAlloc4->Add (Vector(716.0229765216843,546.7598906632977, 0.0));
    positionAlloc4->Add (Vector(308.68315866795933,830.2758901833729, 0.0));
    positionAlloc4->Add (Vector(658.9719793995594,160.6916866440193, 0.0));
    positionAlloc4->Add (Vector(728.7584178344822,847.7676582280529, 0.0));
    positionAlloc4->Add (Vector(157.96135620325407,151.1567416993798, 0.0));
    positionAlloc4->Add (Vector(216.78240394361103,774.2869201413479, 0.0));
    positionAlloc4->Add (Vector(456.95087379220394,350.63089091480936, 0.0));
    positionAlloc4->Add (Vector(405.3486693591735,542.4884722249863, 0.0));
    positionAlloc4->Add (Vector(371.14069771894515,555.6775404456077, 0.0));
    positionAlloc4->Add (Vector(400.91461299349453,313.24344165516504, 0.0));
    positionAlloc4->Add (Vector(700.4220012894834,261.95032700343745, 0.0));
    positionAlloc4->Add (Vector(780.4045513942152,696.0936944365644, 0.0));
    positionAlloc4->Add (Vector(188.4748429650563,551.7822973278171, 0.0));
    positionAlloc4->Add (Vector(530.2874891188908,508.22912129002214, 0.0));
    positionAlloc4->Add (Vector(438.700643008931,724.4355779982076, 0.0));
    positionAlloc4->Add (Vector(828.4884710439684,818.9896912432104, 0.0));
    positionAlloc4->Add (Vector(668.003877026332,507.14924165501736, 0.0));
    positionAlloc4->Add (Vector(427.3963627675598,633.1838264297808, 0.0));
    positionAlloc4->Add (Vector(591.6938079495496,442.74194903990684, 0.0));
    positionAlloc4->Add (Vector(722.3895182933242,612.0819672876078, 0.0));
    positionAlloc4->Add (Vector(240.7075175509459,283.4680067148213, 0.0));
    positionAlloc4->Add (Vector(251.7913050648304,632.7181032294131, 0.0));
    positionAlloc4->Add (Vector(584.622793219546,437.10480392204784, 0.0));
    positionAlloc4->Add (Vector(538.8454475254945,354.96419908657197, 0.0));
    positionAlloc4->Add (Vector(628.2648815967175,608.7992507721729, 0.0));
    positionAlloc4->Add (Vector(261.78265872345884,800.2228786588624, 0.0));
    positionAlloc4->Add (Vector(219.90714704539295,248.19761601977387, 0.0));
    positionAlloc4->Add (Vector(188.9064441388539,651.8992006581832, 0.0));
    positionAlloc4->Add (Vector(483.2358357513392,602.9435449343346, 0.0));
    positionAlloc4->Add (Vector(841.6381224868564,257.2626241642279, 0.0));
    positionAlloc4->Add (Vector(618.0216663925864,448.203555591441, 0.0));
    positionAlloc4->Add (Vector(374.7854231406974,828.1362276977353, 0.0));
    positionAlloc4->Add (Vector(215.44510815810156,345.78650934988366, 0.0));
    positionAlloc4->Add (Vector(327.25631525937274,345.098472622862, 0.0));
    positionAlloc4->Add (Vector(597.7816715018002,152.08609154763167, 0.0));
    positionAlloc4->Add (Vector(794.4819088187869,490.0763570059105, 0.0));
    positionAlloc4->Add (Vector(549.6455062602611,740.6964407413816, 0.0));
    positionAlloc4->Add (Vector(754.3705953525536,329.5000862561954, 0.0));
    positionAlloc4->Add (Vector(813.9841021526131,442.995918696151, 0.0));
    positionAlloc4->Add (Vector(236.30752645987346,477.1186647085476, 0.0));
    positionAlloc4->Add (Vector(781.151796278562,556.3720531204156, 0.0));
    positionAlloc4->Add (Vector(826.1195090603396,318.43765871056996, 0.0));
    positionAlloc4->Add (Vector(324.6359262422378,382.56561667280164, 0.0));
    positionAlloc4->Add (Vector(564.176293672401,528.4162577732243, 0.0));
    positionAlloc4->Add (Vector(281.46544429029836,672.2604259751392, 0.0));
    positionAlloc4->Add (Vector(242.50933811302588,200.8978500412142, 0.0));
    positionAlloc4->Add (Vector(544.3722514177819,708.2646995226065, 0.0));
    positionAlloc4->Add (Vector(507.763140809493,190.77041694002986, 0.0));
    positionAlloc4->Add (Vector(536.7638462568846,761.2288644004426, 0.0));
    positionAlloc4->Add (Vector(317.88922393662267,287.48417735435, 0.0));
    positionAlloc4->Add (Vector(671.5255738711145,232.6412298332699, 0.0));
    positionAlloc4->Add (Vector(473.3636336873334,276.44558566327277, 0.0));
    positionAlloc4->Add (Vector(834.4680423967363,228.02349770846178, 0.0));
    positionAlloc4->Add (Vector(245.0649738984346,611.3803193298331, 0.0));
    positionAlloc4->Add (Vector(326.3581486863794,165.57508784665765, 0.0));
    positionAlloc4->Add (Vector(201.62091559881247,765.6651148102612, 0.0));
    positionAlloc4->Add (Vector(781.8698191925258,461.64848544610106, 0.0));
    positionAlloc4->Add (Vector(827.3649387024946,217.9542433315602, 0.0));
    positionAlloc4->Add (Vector(786.0631688915354,375.82844300899234, 0.0));
    positionAlloc4->Add (Vector(275.13095242974504,488.26530937160635, 0.0));
    positionAlloc4->Add (Vector(613.1280475558747,783.4966581806274, 0.0));
    positionAlloc4->Add (Vector(502.7873479804539,827.0424190895354, 0.0));
    positionAlloc4->Add (Vector(613.1790493656654,437.3703797627012, 0.0));
    positionAlloc4->Add (Vector(453.3458448514475,705.2508752092324, 0.0));
    positionAlloc4->Add (Vector(470.7858376777658,654.8998840184781, 0.0));
    positionAlloc4->Add (Vector(712.7134640009396,374.34274608575356, 0.0));
    positionAlloc4->Add (Vector(498.9903481972432,409.0434440089829, 0.0));
    positionAlloc4->Add (Vector(562.2085731450479,280.6350440967787, 0.0));
    positionAlloc4->Add (Vector(247.17167757032462,205.32287301303845, 0.0));
    positionAlloc4->Add (Vector(452.7590823225828,179.44609305061903, 0.0));
    positionAlloc4->Add (Vector(306.6406041065974,804.7586495833103, 0.0));
    positionAlloc4->Add (Vector(215.9027265982565,304.258783396544, 0.0));
    positionAlloc4->Add (Vector(445.48508993512723,389.188838343156, 0.0));
    positionAlloc4->Add (Vector(153.2269551812177,604.7765865617688, 0.0));
    positionAlloc4->Add (Vector(598.3876576508336,491.0017539465662, 0.0));
    positionAlloc4->Add (Vector(499.7850429996814,411.8036395613025, 0.0));
    positionAlloc4->Add (Vector(560.274982556056,373.31908786134466, 0.0));
    positionAlloc4->Add (Vector(252.09889121845316,330.4627418943785, 0.0));
    positionAlloc4->Add (Vector(493.82824030072214,625.2183568225116, 0.0));
    positionAlloc4->Add (Vector(305.05054670643113,782.2565374796553, 0.0));
    positionAlloc4->Add (Vector(658.316493789412,661.6025761311989, 0.0));
    positionAlloc4->Add (Vector(804.1959022949972,352.51302197283724, 0.0));
    positionAlloc4->Add (Vector(596.4219663720023,483.9540200914494, 0.0));
    positionAlloc4->Add (Vector(577.7064739179324,206.82719377418502, 0.0));
    positionAlloc4->Add (Vector(396.8190749813271,659.9798190633609, 0.0));
    positionAlloc4->Add (Vector(429.33585306980785,307.49907391229976, 0.0));
    positionAlloc4->Add (Vector(340.1559236844357,312.404340897265, 0.0));
    positionAlloc4->Add (Vector(666.4045153177325,260.39535129836594, 0.0));
    positionAlloc4->Add (Vector(196.36765146184746,470.2268946120372, 0.0));
    positionAlloc4->Add (Vector(744.0468785671414,164.58577838345667, 0.0));
    positionAlloc4->Add (Vector(204.29823508349415,582.6508758051632, 0.0));
    positionAlloc4->Add (Vector(263.388048850877,274.6206707237546, 0.0));
    positionAlloc4->Add (Vector(568.2810621690539,441.08798700139823, 0.0));
    positionAlloc4->Add (Vector(667.3282712489188,474.0815656019986, 0.0));
    positionAlloc4->Add (Vector(627.1246131178164,362.97729838683347, 0.0));
    positionAlloc4->Add (Vector(731.8315692115114,369.01803881715296, 0.0));
    positionAlloc4->Add (Vector(668.5276609043755,694.9337803813658, 0.0));
    positionAlloc4->Add (Vector(690.5929290402289,340.1487915209218, 0.0));
    positionAlloc4->Add (Vector(267.38812603860555,588.6988031697513, 0.0));
    positionAlloc4->Add (Vector(586.359532154628,778.5652403674312, 0.0));
    positionAlloc4->Add (Vector(703.6423201946903,805.7376933604717, 0.0));
    positionAlloc4->Add (Vector(710.4840245019998,650.7429149012319, 0.0));
    positionAlloc4->Add (Vector(807.4734060125762,371.2558030756535, 0.0));
    positionAlloc4->Add (Vector(388.20052696887785,273.9676860058587, 0.0));
    positionAlloc4->Add (Vector(505.1544802571945,486.607221045358, 0.0));
    positionAlloc4->Add (Vector(642.0446986433776,746.0297097208141, 0.0));
    positionAlloc4->Add (Vector(522.4050928267645,450.2678785627809, 0.0));
    positionAlloc4->Add (Vector(696.7684285869377,464.9052867529938, 0.0));
    positionAlloc4->Add (Vector(832.9431457558538,575.4412567141815, 0.0));
    positionAlloc4->Add (Vector(305.24285684662345,432.3498802165126, 0.0));
    positionAlloc4->Add (Vector(758.1302021235391,552.0485492127145, 0.0));
    positionAlloc4->Add (Vector(503.4634339530592,774.8307338623221, 0.0));
    positionAlloc4->Add (Vector(332.4739887841406,425.2552311629653, 0.0));
    positionAlloc4->Add (Vector(426.63387101878845,779.4555063823092, 0.0));
    positionAlloc4->Add (Vector(277.75155610040235,738.5223082807706, 0.0));
    positionAlloc4->Add (Vector(829.6906563325248,534.6321357798436, 0.0));
    positionAlloc4->Add (Vector(243.85819953863287,285.1505916577119, 0.0));
    positionAlloc4->Add (Vector(737.7238156933615,326.8719483703038, 0.0));
    positionAlloc4->Add (Vector(589.7470540163735,776.1346389107739, 0.0));
    positionAlloc4->Add (Vector(214.2208356159841,375.0420849531336, 0.0));
    positionAlloc4->Add (Vector(158.38508262911503,395.22531047435564, 0.0));
    positionAlloc4->Add (Vector(275.6029404948375,293.208360505307, 0.0));
    positionAlloc4->Add (Vector(689.0079729315975,401.63010340253913, 0.0));
    positionAlloc4->Add (Vector(647.4601639467996,600.7042645073011, 0.0));
    positionAlloc4->Add (Vector(525.7663596942193,604.0781314329636, 0.0));
    positionAlloc4->Add (Vector(816.0925967773111,203.19205632687914, 0.0));
    positionAlloc4->Add (Vector(755.3836702035395,750.1319590928616, 0.0));
    positionAlloc4->Add (Vector(235.61302140560667,301.7839523850934, 0.0));
    positionAlloc4->Add (Vector(655.1799195153885,401.36746847547374, 0.0));
    positionAlloc4->Add (Vector(729.8583050611384,704.0717498155387, 0.0));
    positionAlloc4->Add (Vector(427.0254237001914,609.2056759296426, 0.0));
    positionAlloc4->Add (Vector(676.3228541493842,257.1794716340134, 0.0));
    positionAlloc4->Add (Vector(618.8635288668237,328.0721657981343, 0.0));
    positionAlloc4->Add (Vector(468.1244547890287,471.9848194578861, 0.0));
    positionAlloc4->Add (Vector(453.3861135994335,643.1022744533168, 0.0));
    positionAlloc4->Add (Vector(307.01973651404955,172.06552971497905, 0.0));
    positionAlloc4->Add (Vector(475.14486873982094,780.922365614509, 0.0));
    positionAlloc4->Add (Vector(661.7464613089205,163.30518001796995, 0.0));
    positionAlloc4->Add (Vector(734.6661217768357,509.37468148797376, 0.0));
    positionAlloc4->Add (Vector(401.16271639820684,438.43445812257016, 0.0));
    positionAlloc4->Add (Vector(553.9820100581659,210.39121041348193, 0.0));
    positionAlloc4->Add (Vector(814.3607710735613,413.70130775473575, 0.0));
    positionAlloc4->Add (Vector(580.930576572152,702.4537139993162, 0.0));
    positionAlloc4->Add (Vector(682.5002395087649,575.1161200448928, 0.0));
    positionAlloc4->Add (Vector(503.6338601269385,565.0337389836843, 0.0));
    positionAlloc4->Add (Vector(499.62349392103977,657.9588245507064, 0.0));
    positionAlloc4->Add (Vector(429.8514232795739,685.4226700278007, 0.0));
    positionAlloc4->Add (Vector(621.7439118591756,423.8424092000977, 0.0));
    positionAlloc4->Add (Vector(766.6361354937704,249.50245931956175, 0.0));
    positionAlloc4->Add (Vector(532.3916747078815,193.27098569111777, 0.0));
    positionAlloc4->Add (Vector(198.0792803535721,251.59778528629843, 0.0));
    positionAlloc4->Add (Vector(384.09011594822863,591.291219893383, 0.0));
    positionAlloc4->Add (Vector(512.7588254324144,533.3800061204008, 0.0));
    positionAlloc4->Add (Vector(301.8984222422218,235.94935868340553, 0.0));
    positionAlloc4->Add (Vector(783.8837971227412,547.0211701296411, 0.0));
    positionAlloc4->Add (Vector(248.1094951914912,400.8980196661041, 0.0));
    positionAlloc4->Add (Vector(559.8900311239079,632.0824056518172, 0.0));
    positionAlloc4->Add (Vector(670.7181776597514,318.9111495784748, 0.0));
    positionAlloc4->Add (Vector(430.44593093161956,800.8897455435944, 0.0));
    positionAlloc4->Add (Vector(484.99110055624686,508.5881255491164, 0.0));
    positionAlloc4->Add (Vector(527.8904657798917,376.3261696687242, 0.0));
    positionAlloc4->Add (Vector(643.2478736085038,739.35704144974, 0.0));
    positionAlloc4->Add (Vector(825.9450800956802,291.65449222525257, 0.0));
    positionAlloc4->Add (Vector(212.7934139293775,773.0582186827306, 0.0));
    positionAlloc4->Add (Vector(701.4045511072735,551.9866827206287, 0.0));
    positionAlloc4->Add (Vector(202.62640950981813,675.0155332714465, 0.0));
    positionAlloc4->Add (Vector(252.49857915423001,437.2208860317227, 0.0));
    positionAlloc4->Add (Vector(306.30828437144675,340.36909222719476, 0.0));
    positionAlloc4->Add (Vector(154.39988210801386,249.5003193662726, 0.0));
    positionAlloc4->Add (Vector(649.595671592027,613.7667372685928, 0.0));
    positionAlloc4->Add (Vector(415.8982401337165,684.4114269032451, 0.0));
    positionAlloc4->Add (Vector(343.8861819824539,304.89372876056325, 0.0));
    positionAlloc4->Add (Vector(662.933302967178,237.54986761539834, 0.0));
    positionAlloc4->Add (Vector(291.07655589136675,605.7880214666271, 0.0));
    positionAlloc4->Add (Vector(699.4159366494976,453.5521473124209, 0.0));
    positionAlloc4->Add (Vector(261.5175148675105,326.4328017704494, 0.0));
    positionAlloc4->Add (Vector(351.6308068493954,703.5437660008054, 0.0));
    positionAlloc4->Add (Vector(843.980379086495,518.6618782779911, 0.0));
    positionAlloc4->Add (Vector(450.38632244732173,807.2087717365201, 0.0));
    positionAlloc4->Add (Vector(357.6876883396603,798.6349440003456, 0.0));
    positionAlloc4->Add (Vector(664.5096659225585,216.2580662755474, 0.0));
    positionAlloc4->Add (Vector(346.8949150511776,511.2037045216538, 0.0));
    positionAlloc4->Add (Vector(516.2729526679859,278.93196786635997, 0.0));
    positionAlloc4->Add (Vector(678.8994921696669,319.7357419769322, 0.0));
    positionAlloc4->Add (Vector(742.1311672059791,754.8493522018282, 0.0));
    positionAlloc4->Add (Vector(395.5952420379408,208.52159671708984, 0.0));
    positionAlloc4->Add (Vector(152.5311032400974,226.81621304871496, 0.0));
    positionAlloc4->Add (Vector(238.59551666978626,620.2893802631272, 0.0));
    positionAlloc4->Add (Vector(430.86042668642403,723.9138382782547, 0.0));
    positionAlloc4->Add (Vector(326.6607335650296,190.97899536505457, 0.0));
    positionAlloc4->Add (Vector(223.3347003767832,707.4799921394522, 0.0));
    positionAlloc4->Add (Vector(782.6109833495464,218.9009082925382, 0.0));
    positionAlloc4->Add (Vector(660.8148499428767,724.100847566309, 0.0));
    positionAlloc4->Add (Vector(646.7054357147856,684.294519448884, 0.0));
    positionAlloc4->Add (Vector(715.012054804529,343.3591194956914, 0.0));
    positionAlloc4->Add (Vector(651.9251451106788,834.5243005373216, 0.0));
    positionAlloc4->Add (Vector(192.28839418614592,567.7056025571537, 0.0));
    positionAlloc4->Add (Vector(527.070749987136,740.5268803962092, 0.0));
    positionAlloc4->Add (Vector(283.68742137814206,433.35678410321674, 0.0));
    positionAlloc4->Add (Vector(720.1758726038146,586.484684592345, 0.0));
    positionAlloc4->Add (Vector(181.7640769462257,224.57568552586037, 0.0));
    positionAlloc4->Add (Vector(562.8101778261162,701.1676977910639, 0.0));
    positionAlloc4->Add (Vector(374.4928653544985,823.2832164039238, 0.0));
    positionAlloc4->Add (Vector(193.45493882242528,362.77919317970384, 0.0));
    positionAlloc4->Add (Vector(300.4166804649524,348.9474108433444, 0.0));
    positionAlloc4->Add (Vector(355.78504153064785,482.67843182562, 0.0));
    positionAlloc4->Add (Vector(604.259355166633,393.46675471014174, 0.0));
    positionAlloc4->Add (Vector(420.5358249912209,525.0593458959515, 0.0));
    positionAlloc4->Add (Vector(385.2905954196609,743.1946213386591, 0.0));
    positionAlloc4->Add (Vector(593.7186881537281,300.7088081250123, 0.0));
    positionAlloc4->Add (Vector(407.1711024063705,752.5532667213251, 0.0));
    positionAlloc4->Add (Vector(361.06017466767344,596.1195407669929, 0.0));
    positionAlloc4->Add (Vector(376.6729741907925,559.914277654318, 0.0));
    positionAlloc4->Add (Vector(233.4037354308158,435.60716204827565, 0.0));
    positionAlloc4->Add (Vector(242.0122116537487,708.2142440534171, 0.0));
    positionAlloc4->Add (Vector(582.535196883054,766.9736990458279, 0.0));
    positionAlloc4->Add (Vector(763.1911321951872,807.3484344707479, 0.0));
    positionAlloc4->Add (Vector(616.055166638331,196.5091422725302, 0.0));
    positionAlloc4->Add (Vector(614.2500291089193,498.1547441517024, 0.0));
    positionAlloc4->Add (Vector(268.4853255007933,497.34411596500564, 0.0));
    positionAlloc4->Add (Vector(374.1610286779653,454.11216585391503, 0.0));
    positionAlloc4->Add (Vector(197.69398464353858,361.4551594914477, 0.0));
    positionAlloc4->Add (Vector(730.6220657868553,779.2200277035016, 0.0));
    positionAlloc4->Add (Vector(449.3896603171031,656.5047481488045, 0.0));
    positionAlloc4->Add (Vector(832.9471033683436,806.4494697611304, 0.0));
    positionAlloc4->Add (Vector(629.2118783571586,571.7046789133083, 0.0));
    positionAlloc4->Add (Vector(699.6267145062939,437.1999974572114, 0.0));
    positionAlloc4->Add (Vector(434.9958957140941,386.12247820677186, 0.0));
    positionAlloc4->Add (Vector(701.2426980405617,201.74520165203455, 0.0));
    positionAlloc4->Add (Vector(397.48729381070825,780.3199509235876, 0.0));
    positionAlloc4->Add (Vector(315.1461611293288,716.4772593554584, 0.0));
    positionAlloc4->Add (Vector(797.9266138498563,828.4721539477632, 0.0));
    positionAlloc4->Add (Vector(229.28835821124565,611.5040309517499, 0.0));
    positionAlloc4->Add (Vector(156.29600497220179,593.3505383811669, 0.0));
    positionAlloc4->Add (Vector(628.7850737742874,356.4950628887858, 0.0));
    positionAlloc4->Add (Vector(682.1898703819836,318.26176782202634, 0.0));
    positionAlloc4->Add (Vector(607.6127500883005,357.00673346617225, 0.0));
    positionAlloc4->Add (Vector(692.8977034666023,237.18877187964253, 0.0));
    positionAlloc4->Add (Vector(658.7173922107393,645.9930980073652, 0.0));
    positionAlloc4->Add (Vector(578.7502645004658,205.26837885209773, 0.0));
    positionAlloc4->Add (Vector(847.7843333728108,695.3314091382025, 0.0));
    positionAlloc4->Add (Vector(510.36930312738036,757.5460360095798, 0.0));
    positionAlloc4->Add (Vector(635.3537056907882,630.7812770260842, 0.0));
    positionAlloc4->Add (Vector(657.3478597420713,845.435954033539, 0.0));
    positionAlloc4->Add (Vector(215.95966225860735,636.7139743216076, 0.0));
    positionAlloc4->Add (Vector(660.2757989685167,606.0008685372118, 0.0));
    positionAlloc4->Add (Vector(623.2010926593351,637.0504144963988, 0.0));
    positionAlloc4->Add (Vector(454.92830047555674,576.5905725606694, 0.0));
    positionAlloc4->Add (Vector(728.4039554561466,239.29798305080192, 0.0));
    positionAlloc4->Add (Vector(684.5540437387879,552.8268221970909, 0.0));
    positionAlloc4->Add (Vector(225.69238063756256,532.8363134856596, 0.0));
    positionAlloc4->Add (Vector(378.46194372286413,362.13272382301056, 0.0));
    positionAlloc4->Add (Vector(465.3271293933304,268.269928770923, 0.0));
    positionAlloc4->Add (Vector(252.15028571463316,590.3328955109353, 0.0));
    positionAlloc4->Add (Vector(606.1565128055013,314.08101760661503, 0.0));
    positionAlloc4->Add (Vector(400.1580754043831,692.5175389406454, 0.0));
    positionAlloc4->Add (Vector(353.35658894473227,575.9633687472732, 0.0));
    positionAlloc4->Add (Vector(649.7236741518227,171.09892240576963, 0.0));
    positionAlloc4->Add (Vector(439.33958490417075,540.7817418740195, 0.0));
    positionAlloc4->Add (Vector(744.5881115645236,286.3556485463213, 0.0));
    positionAlloc4->Add (Vector(771.3060340126975,331.63295470027293, 0.0));
    positionAlloc4->Add (Vector(588.4116326847155,723.340218968545, 0.0));
    positionAlloc4->Add (Vector(830.289464033753,757.8773466233305, 0.0));
    positionAlloc4->Add (Vector(614.9006300689571,218.82656696437468, 0.0));
    positionAlloc4->Add (Vector(180.02073188758766,283.41321078457815, 0.0));
    positionAlloc4->Add (Vector(215.38537723215117,664.9369051522563, 0.0));
    positionAlloc4->Add (Vector(418.76741701839546,610.4573855180245, 0.0));
    positionAlloc4->Add (Vector(696.7214930337433,428.20469067095297, 0.0));
    positionAlloc4->Add (Vector(322.6247345761649,667.5890225785731, 0.0));
    positionAlloc4->Add (Vector(584.676499437011,742.9497963837958, 0.0));
    positionAlloc4->Add (Vector(255.42019139931492,809.1237358341962, 0.0));
    positionAlloc4->Add (Vector(565.7014854758686,674.0100086982619, 0.0));
    positionAlloc4->Add (Vector(261.6089881341487,632.8282486844244, 0.0));
    positionAlloc4->Add (Vector(505.4620193589595,784.7525827202176, 0.0));
    positionAlloc4->Add (Vector(479.7472344215985,250.4329928939503, 0.0));
    positionAlloc4->Add (Vector(475.4685165324113,607.4207947437772, 0.0));
    positionAlloc4->Add (Vector(216.17013480034691,481.04408180324555, 0.0));
    positionAlloc4->Add (Vector(464.3204719101559,264.46834602568424, 0.0));
    positionAlloc4->Add (Vector(363.6356841079669,707.5794462207521, 0.0));
    positionAlloc4->Add (Vector(502.235850805276,353.46348494378447, 0.0));
    positionAlloc4->Add (Vector(790.2828748011342,528.8216810966737, 0.0));
    positionAlloc4->Add (Vector(512.1899321718288,629.6475224369284, 0.0));
    positionAlloc4->Add (Vector(156.92150551184452,713.4919912832854, 0.0));
    positionAlloc4->Add (Vector(704.7717412345294,349.54818833060415, 0.0));
    positionAlloc4->Add (Vector(466.31774555565244,156.3441687306511, 0.0));
    positionAlloc4->Add (Vector(742.3941607310435,783.7752630503037, 0.0));
    positionAlloc4->Add (Vector(439.843358149339,377.356152004358, 0.0));
    positionAlloc4->Add (Vector(698.8925810259103,659.5143354498213, 0.0));
    positionAlloc4->Add (Vector(630.0206126313385,798.6545856451987, 0.0));
    positionAlloc4->Add (Vector(596.3979967206994,818.1472199183457, 0.0));
    positionAlloc4->Add (Vector(356.31259304368393,333.27509590655933, 0.0));
    positionAlloc4->Add (Vector(335.39668012773626,241.7199540686093, 0.0));
    positionAlloc4->Add (Vector(379.97458161734414,221.7836881472526, 0.0));
    positionAlloc4->Add (Vector(682.7144229508127,645.5800553479177, 0.0));
    positionAlloc4->Add (Vector(499.65123899926834,462.8566261555618, 0.0));
    positionAlloc4->Add (Vector(749.3588592341582,782.0053556541486, 0.0));
    positionAlloc4->Add (Vector(792.408066363865,401.4192458188529, 0.0));
    positionAlloc4->Add (Vector(465.871537355814,430.2833160470259, 0.0));
    positionAlloc4->Add (Vector(531.2473389325007,454.6905672047589, 0.0));
    positionAlloc4->Add (Vector(154.84406690937874,157.73807908790667, 0.0));
    positionAlloc4->Add (Vector(812.0538903591927,526.1654315395953, 0.0));
    positionAlloc4->Add (Vector(768.69101292904,739.3724768636561, 0.0));
    positionAlloc4->Add (Vector(667.2800723673028,792.4789376300365, 0.0));
    positionAlloc4->Add (Vector(154.89354977942185,593.8338635054555, 0.0));
    positionAlloc4->Add (Vector(675.7376538494736,837.9617777935133, 0.0));
    positionAlloc4->Add (Vector(840.752961899058,638.0011852872697, 0.0));
    positionAlloc4->Add (Vector(744.8471891181613,708.3927512098379, 0.0));
    positionAlloc4->Add (Vector(689.8407203256435,791.5257143627775, 0.0));
    positionAlloc4->Add (Vector(428.8170399093683,380.3807890263791, 0.0));
    positionAlloc4->Add (Vector(770.9488340410145,321.60361392821585, 0.0));
    positionAlloc4->Add (Vector(481.34968093024236,443.48066889792983, 0.0));
    positionAlloc4->Add (Vector(614.8506573786989,578.153132916319, 0.0));
    positionAlloc4->Add (Vector(620.0694514698473,219.25997543536639, 0.0));
    positionAlloc4->Add (Vector(150.69430512873896,758.440061645365, 0.0));
    positionAlloc4->Add (Vector(538.3224126823479,268.64179244403965, 0.0));
    positionAlloc4->Add (Vector(667.111773823682,169.29776858509194, 0.0));
    positionAlloc4->Add (Vector(473.9913303605841,304.7020246272534, 0.0));
    positionAlloc4->Add (Vector(477.1511090590266,259.15460592125476, 0.0));
    positionAlloc4->Add (Vector(357.43117576429955,246.15805820243196, 0.0));
    positionAlloc4->Add (Vector(392.3919865341859,209.8747515096234, 0.0));
    positionAlloc4->Add (Vector(644.5189797216065,728.5387586867499, 0.0));
    positionAlloc4->Add (Vector(530.400776881015,387.070347421248, 0.0));
    positionAlloc4->Add (Vector(561.4077748839011,554.7135404196246, 0.0));
    positionAlloc4->Add (Vector(216.42242266747576,421.92433882140836, 0.0));
    positionAlloc4->Add (Vector(321.58334002064043,328.54161672863245, 0.0));
    positionAlloc4->Add (Vector(188.33464426807785,205.92958066632548, 0.0));
    positionAlloc4->Add (Vector(784.3124441723253,788.9086382618783, 0.0));
    positionAlloc4->Add (Vector(305.40737399202465,534.1707311142491, 0.0));
    positionAlloc4->Add (Vector(287.21305171340066,323.9516292515915, 0.0));
    positionAlloc4->Add (Vector(540.0739861329818,406.5856567166132, 0.0));
    positionAlloc4->Add (Vector(688.1986894828697,417.8015940709247, 0.0));
    positionAlloc4->Add (Vector(664.9821240691432,847.8376225084409, 0.0));
    positionAlloc4->Add (Vector(203.0129862216329,650.6700699847951, 0.0));
    positionAlloc4->Add (Vector(427.4843076079593,795.1299562963112, 0.0));
    positionAlloc4->Add (Vector(203.40548031973927,800.9521192871518, 0.0));
    positionAlloc4->Add (Vector(673.4826526544216,776.8045705292742, 0.0));
    positionAlloc4->Add (Vector(701.1860399016313,763.9083712599975, 0.0));
    positionAlloc4->Add (Vector(683.8706967835878,848.0436007820109, 0.0));
    positionAlloc4->Add (Vector(261.9080586659825,481.0886366564937, 0.0));
    positionAlloc4->Add (Vector(468.1525206878214,287.94447383102136, 0.0));
    positionAlloc4->Add (Vector(449.13547293703544,781.0105800944726, 0.0));
    positionAlloc4->Add (Vector(725.9988665516685,328.3605771339445, 0.0));
    positionAlloc4->Add (Vector(761.7699859231413,258.73097712273164, 0.0));
    positionAlloc4->Add (Vector(622.3341289739931,694.9792705602823, 0.0));
    positionAlloc4->Add (Vector(398.6163339734959,345.53171297123845, 0.0));
    positionAlloc4->Add (Vector(433.02828599463516,492.9756234289909, 0.0));
    positionAlloc4->Add (Vector(347.81358277627265,544.7592387511409, 0.0));
    positionAlloc4->Add (Vector(829.071235032093,696.5514921858547, 0.0));
    positionAlloc4->Add (Vector(270.05687608616995,392.7615538516026, 0.0));
    positionAlloc4->Add (Vector(677.6783831851197,766.9486682140247, 0.0));
    positionAlloc4->Add (Vector(744.9380045644966,301.0064433259266, 0.0));
    positionAlloc4->Add (Vector(669.3175178262185,628.0564583895423, 0.0));
    positionAlloc4->Add (Vector(839.0164336037749,510.22895386643313, 0.0));
    positionAlloc4->Add (Vector(231.4853805253736,469.8293496401434, 0.0));
    positionAlloc4->Add (Vector(296.4138784025855,368.32137895046344, 0.0));
    positionAlloc4->Add (Vector(662.6443487535286,205.51173232737045, 0.0));
    positionAlloc4->Add (Vector(511.73244400775974,748.278245665008, 0.0));
    positionAlloc4->Add (Vector(191.77170223597506,480.52852464412723, 0.0));
    positionAlloc4->Add (Vector(614.7300644814038,454.3262083679924, 0.0));
    positionAlloc4->Add (Vector(831.3818262498493,305.1250797196768, 0.0));
    positionAlloc4->Add (Vector(653.5490822253662,164.00361509831797, 0.0));
    positionAlloc4->Add (Vector(297.8123289739267,168.6421808398735, 0.0));
    positionAlloc4->Add (Vector(468.420514350982,536.4029959197223, 0.0));
    positionAlloc4->Add (Vector(369.8102971713844,232.97865588295377, 0.0));
    positionAlloc4->Add (Vector(672.1753932347817,243.85794036848824, 0.0));
    positionAlloc4->Add (Vector(399.3380147432497,429.492768810904, 0.0));
    positionAlloc4->Add (Vector(839.8473899744599,805.5534866312918, 0.0));
    positionAlloc4->Add (Vector(750.1342514460138,721.5970468174664, 0.0));
    positionAlloc4->Add (Vector(633.1818192551566,192.72360300126647, 0.0));
    positionAlloc4->Add (Vector(599.1980375990015,379.105550499621, 0.0));
    positionAlloc4->Add (Vector(198.4994310109581,503.2305770696235, 0.0));
    positionAlloc4->Add (Vector(318.3136877856556,216.50193327605146, 0.0));
    positionAlloc4->Add (Vector(399.5795002499906,808.3914825297913, 0.0));
    positionAlloc4->Add (Vector(616.5193747023834,336.2435332274474, 0.0));
    positionAlloc4->Add (Vector(762.4382832783518,257.03381083516, 0.0));
    positionAlloc4->Add (Vector(503.51896195160583,314.6642059794526, 0.0));
    positionAlloc4->Add (Vector(692.0522703271116,150.3569923111605, 0.0));
    positionAlloc4->Add (Vector(502.5809249863137,647.7777938354022, 0.0));
    positionAlloc4->Add (Vector(561.8274961237124,293.18077028145456, 0.0));
    positionAlloc4->Add (Vector(729.4622283087767,848.9675790927149, 0.0));
    positionAlloc4->Add (Vector(556.9833391495602,833.1163806312196, 0.0));
    positionAlloc4->Add (Vector(344.59820381395656,695.7790557559046, 0.0));
    positionAlloc4->Add (Vector(555.8839635692542,847.7097498556241, 0.0));
    positionAlloc4->Add (Vector(658.0743644859992,576.878700809722, 0.0));
    positionAlloc4->Add (Vector(783.3388609953658,722.9851758361089, 0.0));
    positionAlloc4->Add (Vector(505.2166777157997,177.71690025935447, 0.0));
    positionAlloc4->Add (Vector(492.85018464288646,303.82588506884713, 0.0));
    positionAlloc4->Add (Vector(497.90208075722165,847.2538410576185, 0.0));
    positionAlloc4->Add (Vector(311.81607375774416,835.5099981921837, 0.0));
    positionAlloc4->Add (Vector(379.2862353885334,436.5707857715144, 0.0));
    positionAlloc4->Add (Vector(544.2645482126259,618.36681139868, 0.0));
    positionAlloc4->Add (Vector(213.2588239587347,693.8117385531751, 0.0));
    positionAlloc4->Add (Vector(564.4020875115417,688.1681593667519, 0.0));
    positionAlloc4->Add (Vector(481.59434538251594,512.3785567710521, 0.0));
    positionAlloc4->Add (Vector(384.82333989880914,639.1149936052104, 0.0));
    positionAlloc4->Add (Vector(345.1740953726877,375.25783601132616, 0.0));
    positionAlloc4->Add (Vector(529.6030973079569,613.6910222568109, 0.0));
    positionAlloc4->Add (Vector(314.81510193886015,838.9829060910492, 0.0));
    positionAlloc4->Add (Vector(208.34452884459645,456.2957276029998, 0.0));
    positionAlloc4->Add (Vector(176.8443356477851,168.95855837974543, 0.0));
    positionAlloc4->Add (Vector(536.988006134837,678.1765833057726, 0.0));
    positionAlloc4->Add (Vector(769.935276421263,648.9639146741833, 0.0));
    positionAlloc4->Add (Vector(722.8611377089263,533.3255902592737, 0.0));
    positionAlloc4->Add (Vector(455.5997863160137,469.64029240579913, 0.0));
    positionAlloc4->Add (Vector(164.33492278707953,591.1667099743131, 0.0));
    positionAlloc4->Add (Vector(719.8417721138394,671.6529340373091, 0.0));
    positionAlloc4->Add (Vector(670.1532943478732,501.008522999087, 0.0));
    positionAlloc4->Add (Vector(395.5017022445161,452.11678741836863, 0.0));
    positionAlloc4->Add (Vector(609.4546675607048,423.3349083682747, 0.0));
    positionAlloc4->Add (Vector(808.2917548427682,463.009869740231, 0.0));
    positionAlloc4->Add (Vector(726.7384892196403,774.3492346882391, 0.0));
    positionAlloc4->Add (Vector(469.14629159026265,746.6434606668465, 0.0));
    positionAlloc4->Add (Vector(759.0791287762057,303.03283188120133, 0.0));
    positionAlloc4->Add (Vector(791.3636833192505,157.6268311177401, 0.0));
    positionAlloc4->Add (Vector(383.2429624881013,640.3331309250784, 0.0));
    positionAlloc4->Add (Vector(577.9354428670672,665.5077494792882, 0.0));
    positionAlloc4->Add (Vector(808.8014630427159,577.5489888631764, 0.0));
    positionAlloc4->Add (Vector(274.7180312159868,560.9723485441261, 0.0));
    positionAlloc4->Add (Vector(607.744958948026,682.4687467172889, 0.0));
    positionAlloc4->Add (Vector(611.6828965648477,651.2095486529905, 0.0));
    positionAlloc4->Add (Vector(674.5050381651059,201.0879003116656, 0.0));
    positionAlloc4->Add (Vector(341.28371818890764,431.59223681649524, 0.0));
    positionAlloc4->Add (Vector(469.5173822370098,225.24952469456963, 0.0));
    positionAlloc4->Add (Vector(306.57291867451755,690.6254316584561, 0.0));
    positionAlloc4->Add (Vector(393.3570633083077,700.0798267231852, 0.0));
    positionAlloc4->Add (Vector(485.78072823434024,765.4880382605276, 0.0));
    positionAlloc4->Add (Vector(838.0336630017829,734.4585622495987, 0.0));
    positionAlloc4->Add (Vector(463.8354390207264,405.38874286679146, 0.0));
    positionAlloc4->Add (Vector(277.96002117194166,363.3691942301049, 0.0));
    positionAlloc4->Add (Vector(616.1759328149233,697.2804926003184, 0.0));
    positionAlloc4->Add (Vector(402.5949483955513,584.3868610164168, 0.0));
    positionAlloc4->Add (Vector(323.2810009126322,767.6948459408652, 0.0));
    positionAlloc4->Add (Vector(445.1855996027272,226.6756234841599, 0.0));
    positionAlloc4->Add (Vector(816.6460527713388,377.7294787348991, 0.0));
    positionAlloc4->Add (Vector(757.3822140267995,832.95649197389, 0.0));
    positionAlloc4->Add (Vector(447.10632280062543,647.3704208549734, 0.0));
    positionAlloc4->Add (Vector(758.8416597084655,673.3046836331825, 0.0));
    positionAlloc4->Add (Vector(622.5363428043916,353.10754447198224, 0.0));
    positionAlloc4->Add (Vector(707.4113274962968,783.2137110972178, 0.0));
    positionAlloc4->Add (Vector(808.6548332877201,649.3572874948493, 0.0));
    positionAlloc4->Add (Vector(617.5878464805762,604.8756356549774, 0.0));
    positionAlloc4->Add (Vector(706.9026917248424,175.78507197231232, 0.0));
    positionAlloc4->Add (Vector(626.246534506324,425.8655540004471, 0.0));
    positionAlloc4->Add (Vector(624.4156197065483,281.0728642571065, 0.0));
    positionAlloc4->Add (Vector(310.0109410933718,451.46738267298815, 0.0));
    positionAlloc4->Add (Vector(721.1332104670198,368.5999031224786, 0.0));
    positionAlloc4->Add (Vector(756.3245451515367,572.1311985096836, 0.0));
    positionAlloc4->Add (Vector(543.5562748909474,683.0551167735666, 0.0));
    positionAlloc4->Add (Vector(387.235742298606,603.6774114536804, 0.0));
    positionAlloc4->Add (Vector(688.09763613719,825.8899600348307, 0.0));
    positionAlloc4->Add (Vector(824.8145060700597,165.3590273182948, 0.0));
    positionAlloc4->Add (Vector(719.1737655761688,386.1301437835144, 0.0));
    positionAlloc4->Add (Vector(556.0819231294215,724.8349308545298, 0.0));
    positionAlloc4->Add (Vector(607.3206573384026,829.3336525092477, 0.0));
    positionAlloc4->Add (Vector(342.4713772778323,806.5919694054394, 0.0));
    positionAlloc4->Add (Vector(689.9595452105567,562.2828043330642, 0.0));
    positionAlloc4->Add (Vector(419.0223448208602,746.4665126329318, 0.0));
    positionAlloc4->Add (Vector(349.2065173333542,443.96879690344997, 0.0));
    positionAlloc4->Add (Vector(833.8812139078634,674.9685707728546, 0.0));
    positionAlloc4->Add (Vector(272.7732344815288,637.3553405034069, 0.0));
    positionAlloc4->Add (Vector(782.89852423301,314.71587865275956, 0.0));
    positionAlloc4->Add (Vector(305.80117790204457,666.8745157003, 0.0));
    positionAlloc4->Add (Vector(572.1379875527373,467.18778483252345, 0.0));
    positionAlloc4->Add (Vector(361.5198843137015,764.9400934298372, 0.0));
    positionAlloc4->Add (Vector(657.6562139495948,847.2806191350386, 0.0));
    positionAlloc4->Add (Vector(187.22973902700465,780.1083260133588, 0.0));
    positionAlloc4->Add (Vector(163.59089191359593,347.72846122924864, 0.0));
    positionAlloc4->Add (Vector(522.219912130249,511.4112746133805, 0.0));
    positionAlloc4->Add (Vector(499.7671042470088,364.7360744570661, 0.0));
    positionAlloc4->Add (Vector(576.5770344025684,579.8107904262097, 0.0));
    positionAlloc4->Add (Vector(164.66651920616744,185.54854746572593, 0.0));
    positionAlloc4->Add (Vector(600.7961309979388,214.13853914950386, 0.0));
    positionAlloc4->Add (Vector(166.55806116881894,807.7474340847047, 0.0));
    positionAlloc4->Add (Vector(547.245939636744,510.6957368784598, 0.0));
    positionAlloc4->Add (Vector(845.4954995473206,600.9910061787738, 0.0));
    positionAlloc4->Add (Vector(815.4371284892217,253.69885240566296, 0.0));
    positionAlloc4->Add (Vector(772.817283361294,554.8146546134103, 0.0));
    positionAlloc4->Add (Vector(703.8328358943099,206.83621357422703, 0.0));
    positionAlloc4->Add (Vector(204.58209646526805,518.5685732226227, 0.0));
    positionAlloc4->Add (Vector(605.1206195007576,537.4702734812935, 0.0));
    positionAlloc4->Add (Vector(360.81587590088145,321.5925153841595, 0.0));
    positionAlloc4->Add (Vector(526.6739609914623,314.42311618027406, 0.0));
    positionAlloc4->Add (Vector(256.29041195169293,182.73200966781636, 0.0));
    positionAlloc4->Add (Vector(833.3888635230666,587.0749350070897, 0.0));
    positionAlloc4->Add (Vector(773.2306677396581,156.47774415187442, 0.0));
    positionAlloc4->Add (Vector(496.1636940791991,438.1627736189905, 0.0));
    positionAlloc4->Add (Vector(491.6437219394304,264.35481533063734, 0.0));
    positionAlloc4->Add (Vector(154.02728362631092,503.4137610049197, 0.0));
    positionAlloc4->Add (Vector(807.1401277100729,157.63489672245638, 0.0));
    positionAlloc4->Add (Vector(612.510279104422,319.99101210031336, 0.0));
    positionAlloc4->Add (Vector(299.1969327513302,283.6557490455359, 0.0));
    positionAlloc4->Add (Vector(272.59844372030483,827.8875520838059, 0.0));
    positionAlloc4->Add (Vector(470.6482036581642,458.0290273579416, 0.0));
    positionAlloc4->Add (Vector(762.8835107258016,779.2937392364657, 0.0));
    positionAlloc4->Add (Vector(192.98211350674924,322.9466517969462, 0.0));
    positionAlloc4->Add (Vector(749.8215679437499,721.0261112199804, 0.0));
    positionAlloc4->Add (Vector(492.5113816858081,758.0123990927012, 0.0));
    positionAlloc4->Add (Vector(178.0945353044599,793.9314879619283, 0.0));
    positionAlloc4->Add (Vector(768.7457793728838,530.6441161879197, 0.0));
    positionAlloc4->Add (Vector(573.7800936253668,215.91466619698087, 0.0));
    positionAlloc4->Add (Vector(622.1526451363663,476.18724742670594, 0.0));
    positionAlloc4->Add (Vector(794.4312589579519,678.883408088952, 0.0));
    positionAlloc4->Add (Vector(559.7313793230435,695.5751379709552, 0.0));
    positionAlloc4->Add (Vector(841.3894349327578,383.1268963790713, 0.0));
    positionAlloc4->Add (Vector(456.6962999586967,241.98376026382462, 0.0));
    positionAlloc4->Add (Vector(667.7269990223286,224.89998003851952, 0.0));
    positionAlloc4->Add (Vector(776.7290259607378,786.1297718533017, 0.0));
    positionAlloc4->Add (Vector(224.68304938448995,792.2256578883627, 0.0));
    positionAlloc4->Add (Vector(347.4017985983413,685.1615927252923, 0.0));
    positionAlloc4->Add (Vector(248.86066721806958,458.0811353037416, 0.0));
    positionAlloc4->Add (Vector(332.584931188111,166.06390060796127, 0.0));
    positionAlloc4->Add (Vector(471.9362453686125,609.3229416764918, 0.0));
    positionAlloc4->Add (Vector(413.86493781647266,426.16581617538344, 0.0));
    positionAlloc4->Add (Vector(790.1325742063034,764.9114390657555, 0.0));
    positionAlloc4->Add (Vector(779.5514397733219,260.86172810939775, 0.0));
    positionAlloc4->Add (Vector(836.0436207710991,815.7959325092505, 0.0));
    positionAlloc4->Add (Vector(439.29122416520835,689.3842864854976, 0.0));
    positionAlloc4->Add (Vector(728.8851792388216,271.91737664478495, 0.0));
    positionAlloc4->Add (Vector(161.8515251803606,780.0677680241824, 0.0));
    positionAlloc4->Add (Vector(580.586302756132,501.56658158175964, 0.0));
    positionAlloc4->Add (Vector(552.9184241367568,545.5361104032822, 0.0));
    positionAlloc4->Add (Vector(286.7010057112067,595.51151949413, 0.0));
    positionAlloc4->Add (Vector(479.3254878323217,836.0648997014925, 0.0));
    positionAlloc4->Add (Vector(606.7207039415675,293.4210600842428, 0.0));
    positionAlloc4->Add (Vector(655.5946693254446,454.47996308219047, 0.0));
    positionAlloc4->Add (Vector(839.5668663378584,314.6886064926948, 0.0));
    positionAlloc4->Add (Vector(235.91866252128796,509.8367187054833, 0.0));
    positionAlloc4->Add (Vector(452.7383717396095,589.6381718052976, 0.0));
    positionAlloc4->Add (Vector(832.3158630802552,603.7256907857613, 0.0));
    positionAlloc4->Add (Vector(738.9517192715326,174.71157815984876, 0.0));
    positionAlloc4->Add (Vector(416.9713641514961,392.99841796634314, 0.0));
    positionAlloc4->Add (Vector(492.16413541445934,662.0840777769963, 0.0));
    positionAlloc4->Add (Vector(346.90635686977146,705.0899083204632, 0.0));
    positionAlloc4->Add (Vector(292.6730761294183,742.8463926768567, 0.0));
    positionAlloc4->Add (Vector(675.681281766974,647.5996714263968, 0.0));
    positionAlloc4->Add (Vector(230.71876552740554,261.4696758069249, 0.0));
    positionAlloc4->Add (Vector(498.7178909811417,239.1632227694957, 0.0));
    positionAlloc4->Add (Vector(230.26424429937225,739.7179430607601, 0.0));
    positionAlloc4->Add (Vector(748.4144017653073,421.06070573607843, 0.0));
    positionAlloc4->Add (Vector(727.0326364146739,655.5320287329721, 0.0));
    positionAlloc4->Add (Vector(726.469863186873,738.1234084761028, 0.0));
    positionAlloc4->Add (Vector(771.8136066076532,487.7495624060223, 0.0));
    positionAlloc4->Add (Vector(541.8857619975129,441.08383518762076, 0.0));
    positionAlloc4->Add (Vector(626.5719629674752,435.9056738244396, 0.0));
    positionAlloc4->Add (Vector(358.32799730499823,847.9795438089175, 0.0));
    positionAlloc4->Add (Vector(802.1085594171841,550.4662247097215, 0.0));
    positionAlloc4->Add (Vector(580.7058833619782,803.3880006862258, 0.0));
    positionAlloc4->Add (Vector(807.9882997735785,531.6834890728001, 0.0));
    positionAlloc4->Add (Vector(453.9594438950805,237.1608281835343, 0.0));
    positionAlloc4->Add (Vector(605.5579992040557,719.229269139717, 0.0));
    positionAlloc4->Add (Vector(640.5617369113606,535.0977723777218, 0.0));
    positionAlloc4->Add (Vector(262.86576590425193,160.65573004404357, 0.0));
    positionAlloc4->Add (Vector(744.4141491005344,561.1703335311652, 0.0));
    positionAlloc4->Add (Vector(224.97876072058656,683.8674714713494, 0.0));
    positionAlloc4->Add (Vector(699.4385081155104,419.2694373582038, 0.0));
    positionAlloc4->Add (Vector(225.14170381753686,183.60830592643487, 0.0));
    positionAlloc4->Add (Vector(580.1630757723949,635.3712693307184, 0.0));
    positionAlloc4->Add (Vector(807.4114627885834,180.3371133117358, 0.0));
    positionAlloc4->Add (Vector(753.1132469407495,170.04219331354906, 0.0));
    positionAlloc4->Add (Vector(557.9355794104811,724.7994888290757, 0.0));
    positionAlloc4->Add (Vector(699.6872927062205,209.52271745135607, 0.0));
    positionAlloc4->Add (Vector(795.6786192027563,646.3606769966947, 0.0));
    positionAlloc4->Add (Vector(722.2019864497088,343.5953686909372, 0.0));
    positionAlloc4->Add (Vector(705.2677898883979,273.2987937119236, 0.0));
    positionAlloc4->Add (Vector(773.0998504359214,788.378005904258, 0.0));
    positionAlloc4->Add (Vector(253.66148662811216,595.0175502711311, 0.0));
    positionAlloc4->Add (Vector(278.1864218491624,776.1350373367432, 0.0));
    positionAlloc4->Add (Vector(441.8348756564336,402.56027528141846, 0.0));
    positionAlloc4->Add (Vector(517.9102648895105,232.18284365451993, 0.0));
    positionAlloc4->Add (Vector(754.3130005047755,405.37472955226133, 0.0));
    positionAlloc4->Add (Vector(395.59347683652584,688.2366076832326, 0.0));
    positionAlloc4->Add (Vector(188.2384021365392,591.8720045331333, 0.0));
    positionAlloc4->Add (Vector(327.0402161484358,669.5253268066095, 0.0));
    positionAlloc4->Add (Vector(658.5524215848737,812.9767299019124, 0.0));
    positionAlloc4->Add (Vector(191.0332794295063,667.936098112218, 0.0));
    positionAlloc4->Add (Vector(765.8997381898581,810.9056364629721, 0.0));
    positionAlloc4->Add (Vector(246.45984043079352,669.6634680108466, 0.0));
    positionAlloc4->Add (Vector(486.7868163588005,370.1586592608292, 0.0));
    positionAlloc4->Add (Vector(554.6280781401601,515.9641647706194, 0.0));
    positionAlloc4->Add (Vector(423.7482154059939,345.00962099294594, 0.0));
    positionAlloc4->Add (Vector(610.5044539234789,195.16568776835135, 0.0));
    positionAlloc4->Add (Vector(238.17674341328043,735.0901174077086, 0.0));
    positionAlloc4->Add (Vector(195.69206929116064,207.2767459921778, 0.0));
    positionAlloc4->Add (Vector(587.5375010792566,353.60198176005156, 0.0));
    positionAlloc4->Add (Vector(231.15345746025446,421.1509427955725, 0.0));
    positionAlloc4->Add (Vector(311.12844393380107,610.9545130346773, 0.0));
    positionAlloc4->Add (Vector(311.25736439737125,334.49472242986224, 0.0));
    positionAlloc4->Add (Vector(323.3522819319589,279.49978825138555, 0.0));
    positionAlloc4->Add (Vector(795.6261430350684,634.7947940600329, 0.0));
    positionAlloc4->Add (Vector(435.1406265191311,453.21075354303815, 0.0));
    positionAlloc4->Add (Vector(763.78758425263,409.4994945519975, 0.0));
    positionAlloc4->Add (Vector(388.27069693593864,669.2316338236748, 0.0));
    positionAlloc4->Add (Vector(801.5195527969996,485.01676501717094, 0.0));
    positionAlloc4->Add (Vector(755.9025438879846,268.85650345497555, 0.0));
    positionAlloc4->Add (Vector(388.517548997709,183.24595727762556, 0.0));
    positionAlloc4->Add (Vector(695.4586557575788,598.760398771761, 0.0));
    positionAlloc4->Add (Vector(307.23184152074134,187.64258956410103, 0.0));
    positionAlloc4->Add (Vector(365.34632567006275,492.49076398325883, 0.0));
    positionAlloc4->Add (Vector(258.45078079130633,525.057652330861, 0.0));
    positionAlloc4->Add (Vector(309.2329526524039,436.45931585886837, 0.0));
    positionAlloc4->Add (Vector(291.33150737995743,804.93354943006, 0.0));
    positionAlloc4->Add (Vector(492.2253681848678,515.341109477623, 0.0));
    positionAlloc4->Add (Vector(427.0276951565166,238.27698693576448, 0.0));
    positionAlloc4->Add (Vector(490.6680319061881,638.6527581121105, 0.0));
    positionAlloc4->Add (Vector(656.2400999085876,550.6900645810981, 0.0));
    positionAlloc4->Add (Vector(848.1677456777586,424.50965866886014, 0.0));
    positionAlloc4->Add (Vector(822.4146014993255,265.5261313640584, 0.0));
    positionAlloc4->Add (Vector(591.0512049026645,651.5843476163043, 0.0));
    positionAlloc4->Add (Vector(692.730286667398,468.781962484765, 0.0));
    positionAlloc4->Add (Vector(353.66529788649024,571.0450670824589, 0.0));
    positionAlloc4->Add (Vector(830.0444395312496,394.4087810274963, 0.0));
    positionAlloc4->Add (Vector(261.41213840555844,731.100355536816, 0.0));
    positionAlloc4->Add (Vector(506.196151674917,358.582186642651, 0.0));
    positionAlloc4->Add (Vector(598.2842818437719,793.5763888460749, 0.0));
    positionAlloc4->Add (Vector(297.05246975305465,271.0375620933655, 0.0));
    positionAlloc4->Add (Vector(394.89597092127497,365.38457055715116, 0.0));
    positionAlloc4->Add (Vector(441.8594113897415,412.9905420954585, 0.0));
    positionAlloc4->Add (Vector(737.9585720028396,211.13828987976103, 0.0));
    positionAlloc4->Add (Vector(500.4739523638334,428.50061276607926, 0.0));
    positionAlloc4->Add (Vector(312.79797875133187,823.5831655965294, 0.0));
    positionAlloc4->Add (Vector(687.57724755553,207.06731165001037, 0.0));
    positionAlloc4->Add (Vector(817.9672043228055,694.6631566941185, 0.0));
    positionAlloc4->Add (Vector(736.0229085476415,160.62108892028434, 0.0));
    positionAlloc4->Add (Vector(823.801020312835,718.6766692222328, 0.0));
    positionAlloc4->Add (Vector(778.8254137839585,372.18373041127836, 0.0));
    positionAlloc4->Add (Vector(799.0293215421464,832.0420774841006, 0.0));
    positionAlloc4->Add (Vector(459.6723819752197,711.7804982835896, 0.0));
    positionAlloc4->Add (Vector(410.13933290688146,801.1676437301376, 0.0));
    positionAlloc4->Add (Vector(607.1950409471026,270.00319882473184, 0.0));
    positionAlloc4->Add (Vector(194.11824888207678,515.5495757394071, 0.0));
    positionAlloc4->Add (Vector(207.62070880113012,412.71548941994445, 0.0));
    positionAlloc4->Add (Vector(240.2335377171462,655.2738160473809, 0.0));
    positionAlloc4->Add (Vector(173.691059583102,717.4152126139068, 0.0));
    positionAlloc4->Add (Vector(653.1878237439668,369.1475925187968, 0.0));
    positionAlloc4->Add (Vector(564.0571538383012,577.5755450147485, 0.0));
    positionAlloc4->Add (Vector(753.4216120054568,567.7797549556682, 0.0));
    positionAlloc4->Add (Vector(447.3874386764893,338.5393998248285, 0.0));
    positionAlloc4->Add (Vector(441.70629679520056,702.4840978938995, 0.0));
    positionAlloc4->Add (Vector(494.1471012798348,508.2564416908338, 0.0));
    positionAlloc4->Add (Vector(462.8259120396033,787.3612069524045, 0.0));
    positionAlloc4->Add (Vector(320.5981873968301,586.8409099441441, 0.0));
    positionAlloc4->Add (Vector(519.100706823579,377.9301549892237, 0.0));
    positionAlloc4->Add (Vector(774.5959200995372,581.8573966336336, 0.0));
    positionAlloc4->Add (Vector(743.7922204670599,386.5245012154223, 0.0));
    positionAlloc4->Add (Vector(440.148267597265,462.43381800444655, 0.0));
    positionAlloc4->Add (Vector(759.5791171761441,378.51460922595857, 0.0));
    positionAlloc4->Add (Vector(288.9781419795258,152.42792147044676, 0.0));
    positionAlloc4->Add (Vector(362.74657852928647,752.2473432525869, 0.0));
    positionAlloc4->Add (Vector(794.4725673677412,262.84868974285894, 0.0));
    positionAlloc4->Add (Vector(702.3075901383812,748.1746402111756, 0.0));
    positionAlloc4->Add (Vector(652.7694183904844,342.41956412517965, 0.0));
    positionAlloc4->Add (Vector(306.29538885268136,630.3340503392214, 0.0));
    positionAlloc4->Add (Vector(471.95253763777214,414.9066616339147, 0.0));
    positionAlloc4->Add (Vector(449.00074757698485,560.8898381047526, 0.0));
    positionAlloc4->Add (Vector(348.863042223165,794.31263672705, 0.0));
    positionAlloc4->Add (Vector(560.1560607812547,297.082380362651, 0.0));
    positionAlloc4->Add (Vector(477.90095834667545,791.1696168259746, 0.0));
    positionAlloc4->Add (Vector(711.0558609134331,313.499628632716, 0.0));
    positionAlloc4->Add (Vector(773.6054699250433,203.22384431604414, 0.0));
    positionAlloc4->Add (Vector(263.8104182341795,263.982502080624, 0.0));
    positionAlloc4->Add (Vector(659.3391033997063,250.61887921380344, 0.0));
    positionAlloc4->Add (Vector(649.8859612794189,450.4067531863284, 0.0));
    positionAlloc4->Add (Vector(657.6402776170276,594.7796902353786, 0.0));
    positionAlloc4->Add (Vector(418.92358864309216,559.5856275400081, 0.0));
    positionAlloc4->Add (Vector(515.2525878711261,263.0667456094829, 0.0));
    positionAlloc4->Add (Vector(809.8131499938381,648.7155352983843, 0.0));
    positionAlloc4->Add (Vector(626.8949712203599,840.0609455239652, 0.0));
    positionAlloc4->Add (Vector(188.71048042984387,564.1826316201762, 0.0));
    positionAlloc4->Add (Vector(708.0174405577616,827.8234443624713, 0.0));
    positionAlloc4->Add (Vector(514.178909229399,520.1562282626242, 0.0));
    positionAlloc4->Add (Vector(357.3506831561939,217.76843961660018, 0.0));
    positionAlloc4->Add (Vector(490.1919461961341,711.7514237058082, 0.0));
    positionAlloc4->Add (Vector(671.1970366472215,443.32199195888927, 0.0));
    positionAlloc4->Add (Vector(822.0407930353414,340.25562683657245, 0.0));
    positionAlloc4->Add (Vector(708.0446001942228,456.6733527533034, 0.0));
    positionAlloc4->Add (Vector(412.1345287782113,545.8099134605828, 0.0));
    positionAlloc4->Add (Vector(551.3280347223445,176.2401629877382, 0.0));
    positionAlloc4->Add (Vector(836.5199076576383,643.8977414528968, 0.0));
    positionAlloc4->Add (Vector(546.4672604271452,709.8503340220743, 0.0));
    positionAlloc4->Add (Vector(601.2494046244444,784.8622022040647, 0.0));
    positionAlloc4->Add (Vector(591.0313044489474,481.8441869081554, 0.0));
    positionAlloc4->Add (Vector(483.93495640181754,705.7330800928836, 0.0));
    positionAlloc4->Add (Vector(685.2451959994816,154.5614209843127, 0.0));
    positionAlloc4->Add (Vector(695.2115537791739,412.312409417185, 0.0));
    positionAlloc4->Add (Vector(630.5038923317784,669.376693720005, 0.0));
    positionAlloc4->Add (Vector(314.7695048673155,470.6968540468978, 0.0));
    positionAlloc4->Add (Vector(439.97097606470237,393.62726039507726, 0.0));
    positionAlloc4->Add (Vector(614.0971274792305,417.60791047267367, 0.0));
    positionAlloc4->Add (Vector(454.004238601208,641.7479817767382, 0.0));
    positionAlloc4->Add (Vector(790.5699353255039,424.4476371366607, 0.0));
    positionAlloc4->Add (Vector(354.3832060658951,794.4297285652437, 0.0));
    positionAlloc4->Add (Vector(562.7090014273452,572.6720532777902, 0.0));
    positionAlloc4->Add (Vector(283.7745066654353,444.3840193840362, 0.0));
    positionAlloc4->Add (Vector(817.0213290194018,244.66476284636553, 0.0));
    positionAlloc4->Add (Vector(290.244554694022,322.25502346655173, 0.0));
    positionAlloc4->Add (Vector(599.6518766017447,770.8914305103925, 0.0));
    positionAlloc4->Add (Vector(828.9664334720306,841.2876802030958, 0.0));
    positionAlloc4->Add (Vector(319.91278671483985,575.497905775454, 0.0));
    positionAlloc4->Add (Vector(791.9361144282657,628.3278027751194, 0.0));
    positionAlloc4->Add (Vector(468.21727486830406,700.352761449823, 0.0));
    positionAlloc4->Add (Vector(714.7681786236503,826.4174962246889, 0.0));
    positionAlloc4->Add (Vector(317.36677509063065,591.29852529956, 0.0));
    positionAlloc4->Add (Vector(202.63758794670542,450.0148249666199, 0.0));
    positionAlloc4->Add (Vector(639.4672961048893,839.8372254001212, 0.0));
    positionAlloc4->Add (Vector(789.493789087512,627.1392488518437, 0.0));
    positionAlloc4->Add (Vector(557.6321355638063,269.7856953863966, 0.0));
    positionAlloc4->Add (Vector(576.7912825060807,785.4319938683308, 0.0));
    positionAlloc4->Add (Vector(542.9583120528567,749.7347212776242, 0.0));
    positionAlloc4->Add (Vector(584.2372048433997,615.2615894773691, 0.0));
    positionAlloc4->Add (Vector(802.258358126199,535.268264715861, 0.0));
    positionAlloc4->Add (Vector(633.2026709535082,150.03368672601823, 0.0));
    positionAlloc4->Add (Vector(690.0991193391735,392.8393242596281, 0.0));
    positionAlloc4->Add (Vector(225.7306601272624,721.5221774029843, 0.0));
    positionAlloc4->Add (Vector(672.7249177607433,743.2192129943468, 0.0));
    positionAlloc4->Add (Vector(492.7366819871221,212.50012924255506, 0.0));
    positionAlloc4->Add (Vector(686.908185812895,296.9491990956451, 0.0));
    positionAlloc4->Add (Vector(150.21495252362885,631.4965295295076, 0.0));
    positionAlloc4->Add (Vector(651.470590519438,770.1316643777947, 0.0));
    positionAlloc4->Add (Vector(488.170764465737,346.6727372211151, 0.0));
    positionAlloc4->Add (Vector(184.22176065362967,781.9257104818544, 0.0));
    positionAlloc4->Add (Vector(653.3387784438062,354.29637614161277, 0.0));
    positionAlloc4->Add (Vector(472.4938688449601,202.71862641999087, 0.0));
    positionAlloc4->Add (Vector(255.6829770597165,714.4456830638036, 0.0));
    positionAlloc4->Add (Vector(675.0238685628327,459.37574871796767, 0.0));
    positionAlloc4->Add (Vector(661.9032702461218,799.2228961703987, 0.0));
    positionAlloc4->Add (Vector(639.4283624932068,457.3705640245176, 0.0));
    positionAlloc4->Add (Vector(722.4925101423012,188.33157027721472, 0.0));
    positionAlloc4->Add (Vector(560.3937938937329,327.47706376342273, 0.0));
    positionAlloc4->Add (Vector(389.71939970764464,707.6608616948863, 0.0));
    positionAlloc4->Add (Vector(373.89712725079704,320.2173014680103, 0.0));
    positionAlloc4->Add (Vector(636.8573335207925,607.0111650791118, 0.0));
    positionAlloc4->Add (Vector(752.1439460447085,462.0240361476505, 0.0));
    positionAlloc4->Add (Vector(533.1668634286045,300.8107909782524, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_24.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_24.flowmon", false, false);
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