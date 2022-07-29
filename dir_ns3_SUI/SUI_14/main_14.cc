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
	  uint16_t numberOfRrhs = 28;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 735;
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
	double simTime = 20.0;
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
    positionAlloc->Add (Vector (772.2222222222222,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,150.0, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,850.0, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,538.8888888888889, 0.0));
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
    positionAlloc4->Add (Vector(612.9127736008068,787.8793062840017, 0.0));
    positionAlloc4->Add (Vector(285.5902049499914,295.8685753061778, 0.0));
    positionAlloc4->Add (Vector(213.7463274575267,481.7438326378344, 0.0));
    positionAlloc4->Add (Vector(533.4179366283959,576.6979737289248, 0.0));
    positionAlloc4->Add (Vector(751.8101484515628,640.1317815058371, 0.0));
    positionAlloc4->Add (Vector(436.4006515423356,327.8148043668939, 0.0));
    positionAlloc4->Add (Vector(693.737789945051,588.092666235427, 0.0));
    positionAlloc4->Add (Vector(711.8730687684008,247.66874639176848, 0.0));
    positionAlloc4->Add (Vector(202.5154622250427,776.955861281343, 0.0));
    positionAlloc4->Add (Vector(520.842440679759,375.21592090079446, 0.0));
    positionAlloc4->Add (Vector(480.5966750301607,537.6852577515544, 0.0));
    positionAlloc4->Add (Vector(436.88693467671584,843.5609491621565, 0.0));
    positionAlloc4->Add (Vector(239.1604252233526,726.6070002958911, 0.0));
    positionAlloc4->Add (Vector(833.5379433406096,781.2868215091697, 0.0));
    positionAlloc4->Add (Vector(784.166343436251,780.6744472271347, 0.0));
    positionAlloc4->Add (Vector(357.7601435362873,397.98600492933804, 0.0));
    positionAlloc4->Add (Vector(591.0049948125662,741.7934411905389, 0.0));
    positionAlloc4->Add (Vector(799.9654525297008,563.2093502289197, 0.0));
    positionAlloc4->Add (Vector(745.2091156002542,453.8497572711863, 0.0));
    positionAlloc4->Add (Vector(182.60595946057913,643.5792026472965, 0.0));
    positionAlloc4->Add (Vector(213.7082648177619,311.7252965629416, 0.0));
    positionAlloc4->Add (Vector(232.5020760758391,820.7758205627523, 0.0));
    positionAlloc4->Add (Vector(366.83520822963095,829.9082222146144, 0.0));
    positionAlloc4->Add (Vector(223.3758486585359,750.8039348871743, 0.0));
    positionAlloc4->Add (Vector(674.9908890659826,506.92296374392714, 0.0));
    positionAlloc4->Add (Vector(553.003513731843,558.3703626294237, 0.0));
    positionAlloc4->Add (Vector(454.7725772896094,431.3290955691766, 0.0));
    positionAlloc4->Add (Vector(357.55109971675734,261.4981261446209, 0.0));
    positionAlloc4->Add (Vector(817.1509380354959,755.8005835578803, 0.0));
    positionAlloc4->Add (Vector(783.7612317836991,436.629519322144, 0.0));
    positionAlloc4->Add (Vector(666.5671841770569,687.0948107375538, 0.0));
    positionAlloc4->Add (Vector(223.18812070571698,578.7426549679583, 0.0));
    positionAlloc4->Add (Vector(543.8578837699165,485.32850797222954, 0.0));
    positionAlloc4->Add (Vector(381.4430684016794,213.22299220935113, 0.0));
    positionAlloc4->Add (Vector(567.7998025491437,449.48839943893705, 0.0));
    positionAlloc4->Add (Vector(817.1777596973513,187.89408588162453, 0.0));
    positionAlloc4->Add (Vector(673.8774493468202,681.1558998930191, 0.0));
    positionAlloc4->Add (Vector(774.4784109837569,171.5481715663481, 0.0));
    positionAlloc4->Add (Vector(557.7876859293958,199.704746155812, 0.0));
    positionAlloc4->Add (Vector(438.9341509241556,684.9719345966661, 0.0));
    positionAlloc4->Add (Vector(761.4917215908289,452.589901451718, 0.0));
    positionAlloc4->Add (Vector(523.9329242660335,625.2321146017948, 0.0));
    positionAlloc4->Add (Vector(667.8574603552694,644.155632398134, 0.0));
    positionAlloc4->Add (Vector(584.1951719705901,530.9400014795394, 0.0));
    positionAlloc4->Add (Vector(602.8626958361137,495.88801879888945, 0.0));
    positionAlloc4->Add (Vector(378.8843362699175,459.74391956762616, 0.0));
    positionAlloc4->Add (Vector(235.11114467346883,539.240134634023, 0.0));
    positionAlloc4->Add (Vector(649.1220194625728,341.45200608079494, 0.0));
    positionAlloc4->Add (Vector(467.09867058097467,509.0178217567016, 0.0));
    positionAlloc4->Add (Vector(707.4101301773645,452.21297668999443, 0.0));
    positionAlloc4->Add (Vector(667.3872669955823,372.6067933700087, 0.0));
    positionAlloc4->Add (Vector(176.07771642993725,168.44313253574234, 0.0));
    positionAlloc4->Add (Vector(358.62918884284016,174.54546431310308, 0.0));
    positionAlloc4->Add (Vector(520.0408862756335,563.0814956411295, 0.0));
    positionAlloc4->Add (Vector(578.5163545062019,266.5819120156609, 0.0));
    positionAlloc4->Add (Vector(314.82999190761404,246.40981106199877, 0.0));
    positionAlloc4->Add (Vector(322.53453670371533,156.30396498513267, 0.0));
    positionAlloc4->Add (Vector(589.3339117878941,375.2926446409242, 0.0));
    positionAlloc4->Add (Vector(156.97181884301233,805.9787457450304, 0.0));
    positionAlloc4->Add (Vector(184.61320567554307,744.1268903956645, 0.0));
    positionAlloc4->Add (Vector(688.639771120911,321.35266332059734, 0.0));
    positionAlloc4->Add (Vector(326.05912777348055,636.2702107178702, 0.0));
    positionAlloc4->Add (Vector(491.0746922418157,398.80567650717194, 0.0));
    positionAlloc4->Add (Vector(356.2542321437503,185.01317463153043, 0.0));
    positionAlloc4->Add (Vector(649.7402088585441,454.80529023142486, 0.0));
    positionAlloc4->Add (Vector(714.149707390751,242.54806597420978, 0.0));
    positionAlloc4->Add (Vector(550.4698718801376,747.1094912915479, 0.0));
    positionAlloc4->Add (Vector(708.9471745227391,516.8263689073108, 0.0));
    positionAlloc4->Add (Vector(446.76932562428334,402.3730424961534, 0.0));
    positionAlloc4->Add (Vector(263.1989448310756,408.21776508912245, 0.0));
    positionAlloc4->Add (Vector(831.8218574804447,788.0178024183875, 0.0));
    positionAlloc4->Add (Vector(503.5404601892861,517.6753253813417, 0.0));
    positionAlloc4->Add (Vector(164.1653628479653,516.795217225267, 0.0));
    positionAlloc4->Add (Vector(326.25191734281907,409.22412281536555, 0.0));
    positionAlloc4->Add (Vector(720.0934380502724,471.41139126313794, 0.0));
    positionAlloc4->Add (Vector(734.47868463683,782.0307078143597, 0.0));
    positionAlloc4->Add (Vector(660.2314950535249,816.7439103040272, 0.0));
    positionAlloc4->Add (Vector(483.1871832444542,845.6411719375521, 0.0));
    positionAlloc4->Add (Vector(425.98076484702165,623.9573803657756, 0.0));
    positionAlloc4->Add (Vector(336.65933449275315,651.2654377429731, 0.0));
    positionAlloc4->Add (Vector(747.0887783210957,830.99962329724, 0.0));
    positionAlloc4->Add (Vector(635.6189090888299,681.6932837397358, 0.0));
    positionAlloc4->Add (Vector(469.1623300380629,391.0996323520318, 0.0));
    positionAlloc4->Add (Vector(351.00658440634004,500.51443430093883, 0.0));
    positionAlloc4->Add (Vector(785.523420288983,404.2324372635944, 0.0));
    positionAlloc4->Add (Vector(599.2427831087078,480.35615602870604, 0.0));
    positionAlloc4->Add (Vector(256.9253021673316,208.04900675680824, 0.0));
    positionAlloc4->Add (Vector(222.5035962721105,736.4103690358594, 0.0));
    positionAlloc4->Add (Vector(376.52662860384953,737.06000814472, 0.0));
    positionAlloc4->Add (Vector(696.661373265341,738.3147071471639, 0.0));
    positionAlloc4->Add (Vector(570.718114242083,379.16367608077945, 0.0));
    positionAlloc4->Add (Vector(281.18790158864385,463.7586074757572, 0.0));
    positionAlloc4->Add (Vector(442.784856631803,541.5113104537788, 0.0));
    positionAlloc4->Add (Vector(638.4579231437667,207.56325505742038, 0.0));
    positionAlloc4->Add (Vector(806.5915058216478,727.9700121856347, 0.0));
    positionAlloc4->Add (Vector(316.12239294272376,352.0664118728223, 0.0));
    positionAlloc4->Add (Vector(705.6021980496487,699.6420075389775, 0.0));
    positionAlloc4->Add (Vector(577.1250033656493,257.78189082068036, 0.0));
    positionAlloc4->Add (Vector(338.48929701364534,563.4127604170094, 0.0));
    positionAlloc4->Add (Vector(831.0841662942134,257.1794120405389, 0.0));
    positionAlloc4->Add (Vector(261.19688436076905,286.9664069377397, 0.0));
    positionAlloc4->Add (Vector(270.25237663443676,345.0996005988043, 0.0));
    positionAlloc4->Add (Vector(501.8478634828747,494.26792562571916, 0.0));
    positionAlloc4->Add (Vector(503.7324233351162,603.3963523531609, 0.0));
    positionAlloc4->Add (Vector(185.84444443323036,671.8521597469409, 0.0));
    positionAlloc4->Add (Vector(602.4812156660598,531.7539964935492, 0.0));
    positionAlloc4->Add (Vector(770.4634692370767,529.1807768236099, 0.0));
    positionAlloc4->Add (Vector(757.8267745552575,775.2653030469627, 0.0));
    positionAlloc4->Add (Vector(241.31894558597574,832.9234780123784, 0.0));
    positionAlloc4->Add (Vector(775.4104257633855,285.9068876329843, 0.0));
    positionAlloc4->Add (Vector(327.95248034668174,753.458860088162, 0.0));
    positionAlloc4->Add (Vector(702.0072355759297,645.6166487985382, 0.0));
    positionAlloc4->Add (Vector(719.811714863951,693.802228828797, 0.0));
    positionAlloc4->Add (Vector(274.8787763500067,509.1485960044066, 0.0));
    positionAlloc4->Add (Vector(848.2596219633272,495.45025485083164, 0.0));
    positionAlloc4->Add (Vector(502.2598417702823,475.3489441562506, 0.0));
    positionAlloc4->Add (Vector(405.5177128271761,783.1317490969013, 0.0));
    positionAlloc4->Add (Vector(571.0839544657942,574.4218631621725, 0.0));
    positionAlloc4->Add (Vector(722.9798059338522,406.37710419893193, 0.0));
    positionAlloc4->Add (Vector(151.49864354183265,416.16881648937317, 0.0));
    positionAlloc4->Add (Vector(820.3644323662812,757.3938218366816, 0.0));
    positionAlloc4->Add (Vector(766.0919474035492,771.4274565897905, 0.0));
    positionAlloc4->Add (Vector(220.25724656653776,593.6453276802754, 0.0));
    positionAlloc4->Add (Vector(700.3278684387492,413.8649801803905, 0.0));
    positionAlloc4->Add (Vector(454.8535192172815,200.64395692823987, 0.0));
    positionAlloc4->Add (Vector(224.81836256092197,724.7773694190612, 0.0));
    positionAlloc4->Add (Vector(554.8303718930129,632.4608154540663, 0.0));
    positionAlloc4->Add (Vector(514.6939720545499,491.31890663469267, 0.0));
    positionAlloc4->Add (Vector(491.6645821854831,799.1959855379336, 0.0));
    positionAlloc4->Add (Vector(783.5546894561368,454.22535770707736, 0.0));
    positionAlloc4->Add (Vector(249.44312661014357,643.3570852925307, 0.0));
    positionAlloc4->Add (Vector(238.5811210993857,545.9514176693519, 0.0));
    positionAlloc4->Add (Vector(395.3291416225495,751.1697402723997, 0.0));
    positionAlloc4->Add (Vector(325.0397309571072,441.93410982463064, 0.0));
    positionAlloc4->Add (Vector(266.4899990176579,700.4571701731886, 0.0));
    positionAlloc4->Add (Vector(212.30222458787574,635.794475872493, 0.0));
    positionAlloc4->Add (Vector(648.4443949864195,555.9733863745778, 0.0));
    positionAlloc4->Add (Vector(493.9443631881497,760.7258662809772, 0.0));
    positionAlloc4->Add (Vector(439.8474780374715,312.91116930903746, 0.0));
    positionAlloc4->Add (Vector(289.8097960778656,319.61742447121344, 0.0));
    positionAlloc4->Add (Vector(818.9349658469492,170.04183568495696, 0.0));
    positionAlloc4->Add (Vector(536.9892571893378,327.27175412817553, 0.0));
    positionAlloc4->Add (Vector(643.9170528435443,283.56664401241494, 0.0));
    positionAlloc4->Add (Vector(484.5139539859114,306.44601997908796, 0.0));
    positionAlloc4->Add (Vector(646.4904081688692,522.4777936859026, 0.0));
    positionAlloc4->Add (Vector(817.4050773304846,187.48125562487243, 0.0));
    positionAlloc4->Add (Vector(833.4892556688393,346.88201426633225, 0.0));
    positionAlloc4->Add (Vector(785.7809882739463,366.63710543640684, 0.0));
    positionAlloc4->Add (Vector(785.6244134350051,314.98098003149664, 0.0));
    positionAlloc4->Add (Vector(331.0089550869417,656.9434389658591, 0.0));
    positionAlloc4->Add (Vector(239.56495864383538,848.5405940886014, 0.0));
    positionAlloc4->Add (Vector(787.86401722027,724.4391753933257, 0.0));
    positionAlloc4->Add (Vector(181.8405790305922,741.9384100805132, 0.0));
    positionAlloc4->Add (Vector(761.8726572463476,401.34954533020584, 0.0));
    positionAlloc4->Add (Vector(488.8272581594939,834.7886401992916, 0.0));
    positionAlloc4->Add (Vector(371.6174438888527,624.0335065708588, 0.0));
    positionAlloc4->Add (Vector(515.6612847767608,155.05396290260242, 0.0));
    positionAlloc4->Add (Vector(583.3651666897838,250.58412508321373, 0.0));
    positionAlloc4->Add (Vector(449.5086603970938,219.06016371296636, 0.0));
    positionAlloc4->Add (Vector(226.29999290408327,225.68634673396724, 0.0));
    positionAlloc4->Add (Vector(797.8705251528264,730.2509238351084, 0.0));
    positionAlloc4->Add (Vector(436.66293619509975,412.31171577503187, 0.0));
    positionAlloc4->Add (Vector(402.3241204044789,451.2953356615768, 0.0));
    positionAlloc4->Add (Vector(586.6195718496788,731.1280766323857, 0.0));
    positionAlloc4->Add (Vector(795.5616088871109,486.1656994297187, 0.0));
    positionAlloc4->Add (Vector(835.4833059260498,212.04308232470652, 0.0));
    positionAlloc4->Add (Vector(418.9359294481842,452.6337299150657, 0.0));
    positionAlloc4->Add (Vector(367.10305476356143,465.9357076078681, 0.0));
    positionAlloc4->Add (Vector(479.7579954931592,428.5671393409334, 0.0));
    positionAlloc4->Add (Vector(726.1612703048163,179.38090837045422, 0.0));
    positionAlloc4->Add (Vector(618.3560118035982,809.6341464545259, 0.0));
    positionAlloc4->Add (Vector(627.0682209206552,626.0767889032941, 0.0));
    positionAlloc4->Add (Vector(761.3689150597332,208.80120016443794, 0.0));
    positionAlloc4->Add (Vector(561.5199843508472,500.977535016727, 0.0));
    positionAlloc4->Add (Vector(804.6524671196125,158.2355660568984, 0.0));
    positionAlloc4->Add (Vector(178.34147702981312,432.02270526964156, 0.0));
    positionAlloc4->Add (Vector(639.3191898294665,327.038263889401, 0.0));
    positionAlloc4->Add (Vector(838.9154716382504,163.9310386180545, 0.0));
    positionAlloc4->Add (Vector(262.05187384071144,701.1280862201767, 0.0));
    positionAlloc4->Add (Vector(336.69240144605,826.8895126196262, 0.0));
    positionAlloc4->Add (Vector(482.8685651383353,372.7226466560922, 0.0));
    positionAlloc4->Add (Vector(532.2691656023036,299.80497922171196, 0.0));
    positionAlloc4->Add (Vector(731.9916353512178,342.2006208857151, 0.0));
    positionAlloc4->Add (Vector(691.1630991319784,382.87109057122666, 0.0));
    positionAlloc4->Add (Vector(709.705050871402,556.4338323376476, 0.0));
    positionAlloc4->Add (Vector(752.3580406112115,481.63210761710417, 0.0));
    positionAlloc4->Add (Vector(508.81364768710745,381.4205669790013, 0.0));
    positionAlloc4->Add (Vector(285.61725311736745,333.2270083259166, 0.0));
    positionAlloc4->Add (Vector(644.75335307777,703.2852331180916, 0.0));
    positionAlloc4->Add (Vector(162.91949333011584,531.9259250990054, 0.0));
    positionAlloc4->Add (Vector(825.5221360369432,332.27815112155014, 0.0));
    positionAlloc4->Add (Vector(625.261796996469,657.5672132283555, 0.0));
    positionAlloc4->Add (Vector(529.316121293354,213.87138891297874, 0.0));
    positionAlloc4->Add (Vector(585.7611078889013,370.5395476253195, 0.0));
    positionAlloc4->Add (Vector(784.4479477669523,698.8432458799351, 0.0));
    positionAlloc4->Add (Vector(233.0589922373766,549.2711277572715, 0.0));
    positionAlloc4->Add (Vector(833.188183574715,381.849670236185, 0.0));
    positionAlloc4->Add (Vector(169.92017429074573,590.5484598402569, 0.0));
    positionAlloc4->Add (Vector(701.9862894057796,267.0370981830879, 0.0));
    positionAlloc4->Add (Vector(261.7409532769349,293.6279833766125, 0.0));
    positionAlloc4->Add (Vector(363.8021848781226,375.0237345634631, 0.0));
    positionAlloc4->Add (Vector(514.8874276768461,270.4698968549035, 0.0));
    positionAlloc4->Add (Vector(291.02304705521226,313.93609793176904, 0.0));
    positionAlloc4->Add (Vector(652.2041156147775,683.857442938039, 0.0));
    positionAlloc4->Add (Vector(308.83053707066574,800.4964561800664, 0.0));
    positionAlloc4->Add (Vector(162.9995031355692,772.0498685540686, 0.0));
    positionAlloc4->Add (Vector(591.2806482251997,360.85802364359563, 0.0));
    positionAlloc4->Add (Vector(269.3922646045629,245.36355608200796, 0.0));
    positionAlloc4->Add (Vector(744.3925735919574,762.2913890286985, 0.0));
    positionAlloc4->Add (Vector(598.0341578510395,496.3560484593114, 0.0));
    positionAlloc4->Add (Vector(421.5289702662109,413.1719367484646, 0.0));
    positionAlloc4->Add (Vector(625.5216657194874,479.3505033627776, 0.0));
    positionAlloc4->Add (Vector(588.6505783182308,659.3153403133203, 0.0));
    positionAlloc4->Add (Vector(683.8567911292986,449.209344599592, 0.0));
    positionAlloc4->Add (Vector(279.0319442917339,195.38395315033387, 0.0));
    positionAlloc4->Add (Vector(509.19905105929394,319.69316972104764, 0.0));
    positionAlloc4->Add (Vector(513.222672155757,247.94328405889587, 0.0));
    positionAlloc4->Add (Vector(498.70550417580387,470.34231218162256, 0.0));
    positionAlloc4->Add (Vector(442.91275280135955,391.1933355915128, 0.0));
    positionAlloc4->Add (Vector(790.833816852832,237.92889246421146, 0.0));
    positionAlloc4->Add (Vector(482.0278993852037,424.16658001516623, 0.0));
    positionAlloc4->Add (Vector(537.4716197500284,581.2775602910048, 0.0));
    positionAlloc4->Add (Vector(780.9799588604632,733.7416203227258, 0.0));
    positionAlloc4->Add (Vector(310.3816297379793,312.25319433160575, 0.0));
    positionAlloc4->Add (Vector(706.907285579598,441.9386653628939, 0.0));
    positionAlloc4->Add (Vector(346.1514848933997,187.18443693930078, 0.0));
    positionAlloc4->Add (Vector(772.0852503493588,318.15901647688173, 0.0));
    positionAlloc4->Add (Vector(750.5663617816355,423.96466535462287, 0.0));
    positionAlloc4->Add (Vector(698.7476953993213,513.4960576766899, 0.0));
    positionAlloc4->Add (Vector(587.4457288519195,695.8730729362005, 0.0));
    positionAlloc4->Add (Vector(797.23597248977,428.3143085174526, 0.0));
    positionAlloc4->Add (Vector(172.29973376978205,478.5120974520049, 0.0));
    positionAlloc4->Add (Vector(547.0141615393745,537.4006928146092, 0.0));
    positionAlloc4->Add (Vector(233.46000447250105,604.3642183769837, 0.0));
    positionAlloc4->Add (Vector(177.616367045884,160.12944905200553, 0.0));
    positionAlloc4->Add (Vector(586.7619023329719,472.22497304017884, 0.0));
    positionAlloc4->Add (Vector(845.4510922769957,312.63488214681274, 0.0));
    positionAlloc4->Add (Vector(570.5178421152754,627.283122423001, 0.0));
    positionAlloc4->Add (Vector(495.12458507814335,742.7481190181804, 0.0));
    positionAlloc4->Add (Vector(201.07783304798141,724.7882731396259, 0.0));
    positionAlloc4->Add (Vector(773.466673124335,652.9719226855723, 0.0));
    positionAlloc4->Add (Vector(164.3576435242738,324.5630515971245, 0.0));
    positionAlloc4->Add (Vector(215.94725452874565,677.9976331246512, 0.0));
    positionAlloc4->Add (Vector(221.7741766788508,578.8149657931135, 0.0));
    positionAlloc4->Add (Vector(215.97014648316792,270.18579518994386, 0.0));
    positionAlloc4->Add (Vector(643.0965350342406,494.4816599601031, 0.0));
    positionAlloc4->Add (Vector(334.0164913925282,175.02387612610525, 0.0));
    positionAlloc4->Add (Vector(373.73268785564983,267.33509468840913, 0.0));
    positionAlloc4->Add (Vector(332.84011989173905,508.3314848887368, 0.0));
    positionAlloc4->Add (Vector(642.122189981892,305.8225455788664, 0.0));
    positionAlloc4->Add (Vector(836.9910052835647,386.4291909925604, 0.0));
    positionAlloc4->Add (Vector(830.4671952486177,582.290119246733, 0.0));
    positionAlloc4->Add (Vector(318.8447422396161,267.69876012632295, 0.0));
    positionAlloc4->Add (Vector(562.3831151861656,654.5714215324409, 0.0));
    positionAlloc4->Add (Vector(729.538443504345,417.97694557445266, 0.0));
    positionAlloc4->Add (Vector(410.3963318524133,742.9084247723584, 0.0));
    positionAlloc4->Add (Vector(304.8936137463288,262.9918913813018, 0.0));
    positionAlloc4->Add (Vector(680.106900449835,308.90090953689446, 0.0));
    positionAlloc4->Add (Vector(651.3217138695014,441.7243424368372, 0.0));
    positionAlloc4->Add (Vector(172.44446196617912,578.6179021882778, 0.0));
    positionAlloc4->Add (Vector(609.057395929804,497.91655344404467, 0.0));
    positionAlloc4->Add (Vector(561.9034894914973,539.8664641649227, 0.0));
    positionAlloc4->Add (Vector(189.78839949867194,834.9461252702536, 0.0));
    positionAlloc4->Add (Vector(291.8598453933017,459.50787965060795, 0.0));
    positionAlloc4->Add (Vector(838.0614451341548,341.8953170343568, 0.0));
    positionAlloc4->Add (Vector(594.915437405731,362.60466751383206, 0.0));
    positionAlloc4->Add (Vector(805.4399273787594,715.1596578547426, 0.0));
    positionAlloc4->Add (Vector(502.503827437014,775.8382236427205, 0.0));
    positionAlloc4->Add (Vector(626.8464804750843,676.5084245803662, 0.0));
    positionAlloc4->Add (Vector(253.69374541146817,607.6809746793743, 0.0));
    positionAlloc4->Add (Vector(558.2366133181322,310.97299078072393, 0.0));
    positionAlloc4->Add (Vector(330.71071736093484,176.42312187720336, 0.0));
    positionAlloc4->Add (Vector(518.7382350886444,371.70198541765126, 0.0));
    positionAlloc4->Add (Vector(463.3367108658675,692.633538505734, 0.0));
    positionAlloc4->Add (Vector(517.4538589704273,483.9908371769783, 0.0));
    positionAlloc4->Add (Vector(782.6370829793746,436.893371182845, 0.0));
    positionAlloc4->Add (Vector(716.9391842142633,472.14463270963074, 0.0));
    positionAlloc4->Add (Vector(680.0054062152075,839.6451281377656, 0.0));
    positionAlloc4->Add (Vector(312.49373896010934,150.66012976924324, 0.0));
    positionAlloc4->Add (Vector(684.8651001490871,583.5459141554093, 0.0));
    positionAlloc4->Add (Vector(307.4277640112851,540.6939106897505, 0.0));
    positionAlloc4->Add (Vector(795.3689410929251,248.53932043984935, 0.0));
    positionAlloc4->Add (Vector(543.898080339542,219.81168064071767, 0.0));
    positionAlloc4->Add (Vector(775.5540543633366,354.0618497058347, 0.0));
    positionAlloc4->Add (Vector(611.4711759356103,391.3508500581756, 0.0));
    positionAlloc4->Add (Vector(764.4828225030701,776.5743580110644, 0.0));
    positionAlloc4->Add (Vector(153.737399378782,839.4100962795214, 0.0));
    positionAlloc4->Add (Vector(607.7308024584802,620.5639048604236, 0.0));
    positionAlloc4->Add (Vector(788.56073539972,774.9571577129201, 0.0));
    positionAlloc4->Add (Vector(665.2575010359598,256.5035208436616, 0.0));
    positionAlloc4->Add (Vector(786.5129024842346,155.75619197222406, 0.0));
    positionAlloc4->Add (Vector(273.9303061698782,169.96401738705376, 0.0));
    positionAlloc4->Add (Vector(220.5650379973597,332.1479575550275, 0.0));
    positionAlloc4->Add (Vector(154.87031111230385,318.0886343160639, 0.0));
    positionAlloc4->Add (Vector(502.24659172608017,159.85576429802347, 0.0));
    positionAlloc4->Add (Vector(799.4670431533874,382.85961987462406, 0.0));
    positionAlloc4->Add (Vector(770.2517769668676,660.3236199806458, 0.0));
    positionAlloc4->Add (Vector(684.0642149580927,681.640598431815, 0.0));
    positionAlloc4->Add (Vector(172.4767063374408,600.7075492885884, 0.0));
    positionAlloc4->Add (Vector(483.6369123560402,755.9906946268026, 0.0));
    positionAlloc4->Add (Vector(273.307738752002,575.0866690710416, 0.0));
    positionAlloc4->Add (Vector(411.54857292668896,383.03846736198227, 0.0));
    positionAlloc4->Add (Vector(428.24159469583947,301.9982828263703, 0.0));
    positionAlloc4->Add (Vector(778.2561845758478,812.6955405237218, 0.0));
    positionAlloc4->Add (Vector(237.22994539526485,750.3591917692792, 0.0));
    positionAlloc4->Add (Vector(479.7772484984277,723.1165933182564, 0.0));
    positionAlloc4->Add (Vector(465.2682491273056,377.5930498801851, 0.0));
    positionAlloc4->Add (Vector(173.39259629354163,721.0745957177066, 0.0));
    positionAlloc4->Add (Vector(377.35924076625236,355.3727729080848, 0.0));
    positionAlloc4->Add (Vector(759.2803241740859,540.7622251957628, 0.0));
    positionAlloc4->Add (Vector(368.4009142759152,786.8765485824423, 0.0));
    positionAlloc4->Add (Vector(397.7339670482719,572.3278959512188, 0.0));
    positionAlloc4->Add (Vector(815.4282423200232,593.9768574160762, 0.0));
    positionAlloc4->Add (Vector(489.74142799409265,731.1240700997112, 0.0));
    positionAlloc4->Add (Vector(460.2774090324735,280.3714568098423, 0.0));
    positionAlloc4->Add (Vector(819.894238209309,192.30558145839484, 0.0));
    positionAlloc4->Add (Vector(196.63959888726228,833.2557803550274, 0.0));
    positionAlloc4->Add (Vector(674.7380265043166,245.12030508931593, 0.0));
    positionAlloc4->Add (Vector(650.8541682347901,226.73973608240783, 0.0));
    positionAlloc4->Add (Vector(494.2534428365677,545.8194562518757, 0.0));
    positionAlloc4->Add (Vector(488.98633867680786,534.985290468705, 0.0));
    positionAlloc4->Add (Vector(561.6603720982885,549.4827631037936, 0.0));
    positionAlloc4->Add (Vector(616.8337706789472,313.8197227148122, 0.0));
    positionAlloc4->Add (Vector(669.2645326723205,679.6934370780288, 0.0));
    positionAlloc4->Add (Vector(266.34037576612184,394.63188367283783, 0.0));
    positionAlloc4->Add (Vector(420.9913562798038,325.91538099950975, 0.0));
    positionAlloc4->Add (Vector(508.6999859636594,620.8995280503053, 0.0));
    positionAlloc4->Add (Vector(508.00548441111107,458.10391305689274, 0.0));
    positionAlloc4->Add (Vector(381.62096138565596,338.8572405246549, 0.0));
    positionAlloc4->Add (Vector(352.5466871555724,327.45071127404117, 0.0));
    positionAlloc4->Add (Vector(158.52588479157902,612.9366796853488, 0.0));
    positionAlloc4->Add (Vector(807.0099170449537,187.72880327565215, 0.0));
    positionAlloc4->Add (Vector(762.153706526654,674.4520155636287, 0.0));
    positionAlloc4->Add (Vector(480.95534171110813,623.9892420017313, 0.0));
    positionAlloc4->Add (Vector(362.457087414678,226.0251816425448, 0.0));
    positionAlloc4->Add (Vector(309.21377784535974,152.18515594948673, 0.0));
    positionAlloc4->Add (Vector(726.2584514725496,222.1291854016879, 0.0));
    positionAlloc4->Add (Vector(734.6605035922104,775.4213379169811, 0.0));
    positionAlloc4->Add (Vector(461.5130786077487,849.6536327959616, 0.0));
    positionAlloc4->Add (Vector(522.3374878169118,214.2952360604407, 0.0));
    positionAlloc4->Add (Vector(573.4585692384285,579.8635165829912, 0.0));
    positionAlloc4->Add (Vector(344.483029695258,758.9341537232247, 0.0));
    positionAlloc4->Add (Vector(170.67937802804528,453.75467474196165, 0.0));
    positionAlloc4->Add (Vector(235.7633997759657,298.4194953515603, 0.0));
    positionAlloc4->Add (Vector(466.0471573836109,806.9277486560687, 0.0));
    positionAlloc4->Add (Vector(217.8664865192914,574.4868437206603, 0.0));
    positionAlloc4->Add (Vector(379.70302636054805,785.732716987552, 0.0));
    positionAlloc4->Add (Vector(267.96025042142895,171.29647775785992, 0.0));
    positionAlloc4->Add (Vector(741.4652640908229,150.16861216250652, 0.0));
    positionAlloc4->Add (Vector(792.1442310547242,424.035811233811, 0.0));
    positionAlloc4->Add (Vector(254.15179869440703,540.8691342991553, 0.0));
    positionAlloc4->Add (Vector(166.17746792745453,411.1049699149925, 0.0));
    positionAlloc4->Add (Vector(731.6457435752953,179.11716937952866, 0.0));
    positionAlloc4->Add (Vector(652.4323813713615,625.872687653979, 0.0));
    positionAlloc4->Add (Vector(660.7246544647398,220.168959187314, 0.0));
    positionAlloc4->Add (Vector(718.3031898828089,653.2348612233532, 0.0));
    positionAlloc4->Add (Vector(651.9335570949734,208.98939142653586, 0.0));
    positionAlloc4->Add (Vector(278.8287243750127,357.1484442862019, 0.0));
    positionAlloc4->Add (Vector(773.8978653794254,712.7192961146774, 0.0));
    positionAlloc4->Add (Vector(188.43342363754203,621.3740353208846, 0.0));
    positionAlloc4->Add (Vector(405.81603925729155,805.3024348024791, 0.0));
    positionAlloc4->Add (Vector(446.0135279267452,546.0260949572076, 0.0));
    positionAlloc4->Add (Vector(654.4651181555037,538.3647307917636, 0.0));
    positionAlloc4->Add (Vector(190.7174311046473,410.57170663025, 0.0));
    positionAlloc4->Add (Vector(569.3695275719267,212.2506659285167, 0.0));
    positionAlloc4->Add (Vector(334.3545332986555,296.9052740953848, 0.0));
    positionAlloc4->Add (Vector(350.24203616149987,511.7857374141494, 0.0));
    positionAlloc4->Add (Vector(787.5585377342074,272.20878927345217, 0.0));
    positionAlloc4->Add (Vector(449.47641964677496,288.2403971564226, 0.0));
    positionAlloc4->Add (Vector(635.0802556415672,153.51785235571836, 0.0));
    positionAlloc4->Add (Vector(803.2934886653467,714.2115532221484, 0.0));
    positionAlloc4->Add (Vector(744.3241592984311,515.1196590937851, 0.0));
    positionAlloc4->Add (Vector(382.45172357667434,258.45924973655514, 0.0));
    positionAlloc4->Add (Vector(204.96903015910098,542.3775466917396, 0.0));
    positionAlloc4->Add (Vector(754.1201454751994,486.9678982238992, 0.0));
    positionAlloc4->Add (Vector(189.01486855098148,551.7730622511408, 0.0));
    positionAlloc4->Add (Vector(602.1660717784316,831.0260725713864, 0.0));
    positionAlloc4->Add (Vector(563.7667663749298,502.64141060724387, 0.0));
    positionAlloc4->Add (Vector(743.5801853188133,407.36063224137587, 0.0));
    positionAlloc4->Add (Vector(230.77990079657974,434.7665039497946, 0.0));
    positionAlloc4->Add (Vector(355.82779702799934,647.1703393490814, 0.0));
    positionAlloc4->Add (Vector(751.2904737224818,844.1159649649167, 0.0));
    positionAlloc4->Add (Vector(660.7897392127046,717.7107624524485, 0.0));
    positionAlloc4->Add (Vector(773.0132308041353,321.4674558561693, 0.0));
    positionAlloc4->Add (Vector(233.19680493336602,160.46497977296724, 0.0));
    positionAlloc4->Add (Vector(263.796948064435,846.8615140132273, 0.0));
    positionAlloc4->Add (Vector(436.5414446176748,776.7888978359532, 0.0));
    positionAlloc4->Add (Vector(689.7952337195755,612.885714385564, 0.0));
    positionAlloc4->Add (Vector(482.99285473233937,525.6362958148457, 0.0));
    positionAlloc4->Add (Vector(495.79102610044384,704.148050528144, 0.0));
    positionAlloc4->Add (Vector(624.96194863909,288.8870708354939, 0.0));
    positionAlloc4->Add (Vector(595.4608939967134,675.8911818564221, 0.0));
    positionAlloc4->Add (Vector(405.08690619678225,397.10332568028537, 0.0));
    positionAlloc4->Add (Vector(567.2211126648608,472.2703229725284, 0.0));
    positionAlloc4->Add (Vector(819.974967200036,637.756859775039, 0.0));
    positionAlloc4->Add (Vector(326.2973536419963,694.1115037007428, 0.0));
    positionAlloc4->Add (Vector(229.1740295512039,729.7914953711919, 0.0));
    positionAlloc4->Add (Vector(840.4675201402246,523.5128110772861, 0.0));
    positionAlloc4->Add (Vector(667.1635350550805,587.6480737002605, 0.0));
    positionAlloc4->Add (Vector(541.9182159038887,702.7715296549289, 0.0));
    positionAlloc4->Add (Vector(241.58965294417385,623.0681450199131, 0.0));
    positionAlloc4->Add (Vector(699.418088371456,290.0062447789233, 0.0));
    positionAlloc4->Add (Vector(440.3150617709956,801.2458239931475, 0.0));
    positionAlloc4->Add (Vector(798.0261851387138,601.7419362754255, 0.0));
    positionAlloc4->Add (Vector(509.82157363310864,551.2044735868719, 0.0));
    positionAlloc4->Add (Vector(160.4728387794105,335.1824003582292, 0.0));
    positionAlloc4->Add (Vector(326.9984851337912,512.1809028774762, 0.0));
    positionAlloc4->Add (Vector(501.4921486436727,164.00820654014234, 0.0));
    positionAlloc4->Add (Vector(212.44459117187466,297.8106262023489, 0.0));
    positionAlloc4->Add (Vector(529.651265344625,687.8799502641459, 0.0));
    positionAlloc4->Add (Vector(636.9485274117857,806.5208832664184, 0.0));
    positionAlloc4->Add (Vector(405.6709411307603,424.94995243009345, 0.0));
    positionAlloc4->Add (Vector(429.3765096901982,580.7805282964679, 0.0));
    positionAlloc4->Add (Vector(526.2661701421339,846.6115871400849, 0.0));
    positionAlloc4->Add (Vector(705.6851385040314,475.3948625928365, 0.0));
    positionAlloc4->Add (Vector(272.9385258976016,444.96385884969845, 0.0));
    positionAlloc4->Add (Vector(686.2181296635075,387.16081894897906, 0.0));
    positionAlloc4->Add (Vector(840.2195497375008,276.2966754569593, 0.0));
    positionAlloc4->Add (Vector(593.8385392076883,682.6240310728681, 0.0));
    positionAlloc4->Add (Vector(769.2003909407838,300.18956493100154, 0.0));
    positionAlloc4->Add (Vector(829.5979385073902,813.2340640769372, 0.0));
    positionAlloc4->Add (Vector(797.6852491598751,252.82759821444546, 0.0));
    positionAlloc4->Add (Vector(288.1826136113798,552.8641518762868, 0.0));
    positionAlloc4->Add (Vector(319.9004907590082,844.1711369821747, 0.0));
    positionAlloc4->Add (Vector(305.15312023229137,338.64303509335434, 0.0));
    positionAlloc4->Add (Vector(494.2998099752032,687.9274569648936, 0.0));
    positionAlloc4->Add (Vector(391.62588680694137,224.53723429929448, 0.0));
    positionAlloc4->Add (Vector(175.2261642494312,802.9453872814141, 0.0));
    positionAlloc4->Add (Vector(704.7196683974843,364.75599817707086, 0.0));
    positionAlloc4->Add (Vector(335.7588131842408,631.5739880830963, 0.0));
    positionAlloc4->Add (Vector(746.3215877292682,628.8174031844503, 0.0));
    positionAlloc4->Add (Vector(156.54146744037095,173.0838605778154, 0.0));
    positionAlloc4->Add (Vector(368.2443527635154,596.4755254815872, 0.0));
    positionAlloc4->Add (Vector(630.4566246050756,162.84900706539926, 0.0));
    positionAlloc4->Add (Vector(672.3784260451438,573.2288994536551, 0.0));
    positionAlloc4->Add (Vector(478.7579798661365,698.053920917907, 0.0));
    positionAlloc4->Add (Vector(246.86499377934285,673.7132593200873, 0.0));
    positionAlloc4->Add (Vector(280.3155250222492,237.96280190570792, 0.0));
    positionAlloc4->Add (Vector(724.7223629675017,293.13842283707646, 0.0));
    positionAlloc4->Add (Vector(246.79286937446602,826.3246173841757, 0.0));
    positionAlloc4->Add (Vector(649.8609158795123,665.1547743562119, 0.0));
    positionAlloc4->Add (Vector(689.0623218778574,473.62994822149784, 0.0));
    positionAlloc4->Add (Vector(268.79366740867147,652.9221248262168, 0.0));
    positionAlloc4->Add (Vector(300.559660149565,340.30250870814103, 0.0));
    positionAlloc4->Add (Vector(486.49983953904916,499.19571662099196, 0.0));
    positionAlloc4->Add (Vector(330.4286509396768,308.500723561355, 0.0));
    positionAlloc4->Add (Vector(281.62876481215005,751.7658833665326, 0.0));
    positionAlloc4->Add (Vector(396.7029120771891,538.4368836777862, 0.0));
    positionAlloc4->Add (Vector(151.84250890571172,465.6325571652037, 0.0));
    positionAlloc4->Add (Vector(392.27506854767654,270.8263790315922, 0.0));
    positionAlloc4->Add (Vector(650.9448853904337,710.0380855659599, 0.0));
    positionAlloc4->Add (Vector(696.5129110200688,300.50333778902, 0.0));
    positionAlloc4->Add (Vector(522.3918227895473,716.3241851682392, 0.0));
    positionAlloc4->Add (Vector(311.65916521912106,670.4517750453526, 0.0));
    positionAlloc4->Add (Vector(448.83372444541254,536.4127206069559, 0.0));
    positionAlloc4->Add (Vector(425.3469871234333,742.585543073904, 0.0));
    positionAlloc4->Add (Vector(827.0492447657879,457.05876694526216, 0.0));
    positionAlloc4->Add (Vector(418.1710035134091,734.8994969751923, 0.0));
    positionAlloc4->Add (Vector(349.8675502007377,358.6297659539491, 0.0));
    positionAlloc4->Add (Vector(537.7592136982462,465.40774497128217, 0.0));
    positionAlloc4->Add (Vector(369.1177482911069,723.9077032689034, 0.0));
    positionAlloc4->Add (Vector(505.8181859936862,166.78281316357217, 0.0));
    positionAlloc4->Add (Vector(505.17586569147244,380.24384474750707, 0.0));
    positionAlloc4->Add (Vector(813.6372972970232,198.12566646207412, 0.0));
    positionAlloc4->Add (Vector(428.70464546603455,555.9774741422, 0.0));
    positionAlloc4->Add (Vector(421.56651805842444,261.9392715168893, 0.0));
    positionAlloc4->Add (Vector(468.7228870433499,638.6520369350542, 0.0));
    positionAlloc4->Add (Vector(644.3627489722473,697.8560939760137, 0.0));
    positionAlloc4->Add (Vector(183.26243004497726,303.50669775338145, 0.0));
    positionAlloc4->Add (Vector(363.8109585839372,671.2225753023661, 0.0));
    positionAlloc4->Add (Vector(333.78516101400015,177.47713714969993, 0.0));
    positionAlloc4->Add (Vector(542.1690246481368,328.502556750753, 0.0));
    positionAlloc4->Add (Vector(599.1729971130471,722.081488291266, 0.0));
    positionAlloc4->Add (Vector(811.4631945750532,286.8994529710661, 0.0));
    positionAlloc4->Add (Vector(513.6141504738663,543.8821290535107, 0.0));
    positionAlloc4->Add (Vector(324.50674500322793,307.42074615787226, 0.0));
    positionAlloc4->Add (Vector(351.13123325935464,263.3269270366509, 0.0));
    positionAlloc4->Add (Vector(562.884076107829,290.4314597872569, 0.0));
    positionAlloc4->Add (Vector(697.6770660511214,521.5254216716996, 0.0));
    positionAlloc4->Add (Vector(413.4712764141327,620.5840579527962, 0.0));
    positionAlloc4->Add (Vector(375.1637299763229,242.51324426751657, 0.0));
    positionAlloc4->Add (Vector(488.51513367303033,500.73316445609277, 0.0));
    positionAlloc4->Add (Vector(456.4440903226983,165.13889472325565, 0.0));
    positionAlloc4->Add (Vector(805.4467904774898,162.9672037221691, 0.0));
    positionAlloc4->Add (Vector(350.3266095585351,289.2417696160573, 0.0));
    positionAlloc4->Add (Vector(224.00719230179442,615.3654310061712, 0.0));
    positionAlloc4->Add (Vector(260.7685192706839,636.7479755123088, 0.0));
    positionAlloc4->Add (Vector(183.3082347822787,419.8293042898148, 0.0));
    positionAlloc4->Add (Vector(566.7490057572184,760.5431702992265, 0.0));
    positionAlloc4->Add (Vector(420.51034565157215,381.2059228276939, 0.0));
    positionAlloc4->Add (Vector(422.7601956016808,669.1221138160461, 0.0));
    positionAlloc4->Add (Vector(459.5092966789682,200.05211703214474, 0.0));
    positionAlloc4->Add (Vector(655.6178178265955,283.3198979210322, 0.0));
    positionAlloc4->Add (Vector(631.5290518353527,807.124921795883, 0.0));
    positionAlloc4->Add (Vector(837.881767713928,752.1734676842252, 0.0));
    positionAlloc4->Add (Vector(782.3854291039551,223.0600206579803, 0.0));
    positionAlloc4->Add (Vector(639.799154008036,322.3895138722213, 0.0));
    positionAlloc4->Add (Vector(293.7577292829897,666.0593740748787, 0.0));
    positionAlloc4->Add (Vector(719.6803921149902,495.45642786241996, 0.0));
    positionAlloc4->Add (Vector(607.5890850335554,691.7552901765706, 0.0));
    positionAlloc4->Add (Vector(269.66142549731006,180.73322954144606, 0.0));
    positionAlloc4->Add (Vector(163.42103752492878,690.7484922674197, 0.0));
    positionAlloc4->Add (Vector(319.3340408142305,428.95142016278623, 0.0));
    positionAlloc4->Add (Vector(498.42959452482745,390.1808614919066, 0.0));
    positionAlloc4->Add (Vector(334.04697334250295,412.8490354145041, 0.0));
    positionAlloc4->Add (Vector(667.2944442093707,427.82158475744325, 0.0));
    positionAlloc4->Add (Vector(507.14904153468547,239.4948158954269, 0.0));
    positionAlloc4->Add (Vector(398.93539948029587,703.7028518457851, 0.0));
    positionAlloc4->Add (Vector(276.45843146381924,434.59746553384366, 0.0));
    positionAlloc4->Add (Vector(810.6522224455706,712.9420351084467, 0.0));
    positionAlloc4->Add (Vector(251.1606567348652,243.75885358000028, 0.0));
    positionAlloc4->Add (Vector(341.9832951112298,533.3283888999666, 0.0));
    positionAlloc4->Add (Vector(185.5977781836179,304.7862840223105, 0.0));
    positionAlloc4->Add (Vector(652.5559165357986,669.9750511094061, 0.0));
    positionAlloc4->Add (Vector(416.105099370369,386.0664255850894, 0.0));
    positionAlloc4->Add (Vector(193.05401552104698,185.9340599265743, 0.0));
    positionAlloc4->Add (Vector(431.50946729077725,221.67612395522357, 0.0));
    positionAlloc4->Add (Vector(497.3231482707807,727.3062148538442, 0.0));
    positionAlloc4->Add (Vector(584.582842941935,773.6967622509853, 0.0));
    positionAlloc4->Add (Vector(415.4126047859604,388.1924875638216, 0.0));
    positionAlloc4->Add (Vector(633.4907276494162,523.8194112693025, 0.0));
    positionAlloc4->Add (Vector(732.9453497163063,200.59019630794603, 0.0));
    positionAlloc4->Add (Vector(637.963796782531,560.7743115514945, 0.0));
    positionAlloc4->Add (Vector(224.84390506587496,503.3280264958792, 0.0));
    positionAlloc4->Add (Vector(601.694428726494,728.7093538773975, 0.0));
    positionAlloc4->Add (Vector(663.3530603124655,596.2491620539142, 0.0));
    positionAlloc4->Add (Vector(230.9726565536816,812.3851451021574, 0.0));
    positionAlloc4->Add (Vector(479.174359730128,818.113386397092, 0.0));
    positionAlloc4->Add (Vector(251.940116967816,173.40658989276838, 0.0));
    positionAlloc4->Add (Vector(383.8954883133214,225.1527829154674, 0.0));
    positionAlloc4->Add (Vector(393.5780663086515,478.34223815480755, 0.0));
    positionAlloc4->Add (Vector(280.7130958681597,392.1646625042421, 0.0));
    positionAlloc4->Add (Vector(173.65240566377406,565.209196868994, 0.0));
    positionAlloc4->Add (Vector(593.9701636842799,692.1341934931884, 0.0));
    positionAlloc4->Add (Vector(442.86833639929006,458.5985788917611, 0.0));
    positionAlloc4->Add (Vector(404.19281318162354,741.2964169531037, 0.0));
    positionAlloc4->Add (Vector(204.7921365109131,803.3893243146885, 0.0));
    positionAlloc4->Add (Vector(842.2129984962803,814.348575551795, 0.0));
    positionAlloc4->Add (Vector(773.5768920574393,216.12369806076083, 0.0));
    positionAlloc4->Add (Vector(202.32213254542086,472.47948143749363, 0.0));
    positionAlloc4->Add (Vector(807.3760888171684,620.2213883342383, 0.0));
    positionAlloc4->Add (Vector(375.63692065666055,577.2757066508375, 0.0));
    positionAlloc4->Add (Vector(520.6050183788935,426.84235332849914, 0.0));
    positionAlloc4->Add (Vector(478.6044035078307,654.226041534992, 0.0));
    positionAlloc4->Add (Vector(586.640533455597,282.31209792254805, 0.0));
    positionAlloc4->Add (Vector(203.1101488568333,638.7508174461309, 0.0));
    positionAlloc4->Add (Vector(531.1719050331557,522.6171843659374, 0.0));
    positionAlloc4->Add (Vector(351.74837791367423,262.2163276608982, 0.0));
    positionAlloc4->Add (Vector(211.9656206503099,576.2993777363499, 0.0));
    positionAlloc4->Add (Vector(414.5778489488989,474.5730640412246, 0.0));
    positionAlloc4->Add (Vector(539.7910891941938,800.3146200017608, 0.0));
    positionAlloc4->Add (Vector(511.664981890712,665.2121578557352, 0.0));
    positionAlloc4->Add (Vector(819.2137973405969,300.3145875850943, 0.0));
    positionAlloc4->Add (Vector(496.72235285373614,449.38425162342276, 0.0));
    positionAlloc4->Add (Vector(602.9491163812118,474.990658527038, 0.0));
    positionAlloc4->Add (Vector(393.93458305695873,275.4257722377258, 0.0));
    positionAlloc4->Add (Vector(500.94569554362215,180.3780250049926, 0.0));
    positionAlloc4->Add (Vector(219.80834269735755,404.327132898307, 0.0));
    positionAlloc4->Add (Vector(153.94191892363762,280.42174356297346, 0.0));
    positionAlloc4->Add (Vector(779.4752148870953,586.6273690584629, 0.0));
    positionAlloc4->Add (Vector(579.9286067891775,713.0242282680208, 0.0));
    positionAlloc4->Add (Vector(228.56875039411204,353.85632300479324, 0.0));
    positionAlloc4->Add (Vector(345.9561307843346,224.16913794083337, 0.0));
    positionAlloc4->Add (Vector(835.1992106247642,766.176241297181, 0.0));
    positionAlloc4->Add (Vector(792.7352109231921,720.6261623789223, 0.0));
    positionAlloc4->Add (Vector(768.7984438098307,294.8904121000837, 0.0));
    positionAlloc4->Add (Vector(475.52472731147094,632.9272023193525, 0.0));
    positionAlloc4->Add (Vector(508.518969153721,402.09947989725663, 0.0));
    positionAlloc4->Add (Vector(798.0519846461979,786.4855304187386, 0.0));
    positionAlloc4->Add (Vector(719.2275030991228,818.2463564045279, 0.0));
    positionAlloc4->Add (Vector(664.9498620079556,490.91122836136765, 0.0));
    positionAlloc4->Add (Vector(628.5896820072346,163.7782507669896, 0.0));
    positionAlloc4->Add (Vector(614.2034805798758,389.19831553918186, 0.0));
    positionAlloc4->Add (Vector(416.13649858262426,759.8902956650819, 0.0));
    positionAlloc4->Add (Vector(751.6042079834856,329.37844672235076, 0.0));
    positionAlloc4->Add (Vector(252.46114661227745,282.25086963546664, 0.0));
    positionAlloc4->Add (Vector(463.97978455792156,588.3713759341231, 0.0));
    positionAlloc4->Add (Vector(376.2358457270353,373.08808009977815, 0.0));
    positionAlloc4->Add (Vector(829.4013328391524,651.2441373936906, 0.0));
    positionAlloc4->Add (Vector(483.1344292312405,181.19902849884159, 0.0));
    positionAlloc4->Add (Vector(316.26128441584206,415.9303656258928, 0.0));
    positionAlloc4->Add (Vector(269.8293206845877,223.13767671548425, 0.0));
    positionAlloc4->Add (Vector(150.63408467226316,650.6657974378431, 0.0));
    positionAlloc4->Add (Vector(657.8977732919637,310.87822925477525, 0.0));
    positionAlloc4->Add (Vector(358.2448689948677,357.4436538164065, 0.0));
    positionAlloc4->Add (Vector(640.7987919685038,353.45007851457177, 0.0));
    positionAlloc4->Add (Vector(726.0216478054514,282.2091307339981, 0.0));
    positionAlloc4->Add (Vector(452.677452129563,730.0670004735158, 0.0));
    positionAlloc4->Add (Vector(349.78370321198906,740.9467100433625, 0.0));
    positionAlloc4->Add (Vector(814.3557222279718,694.0301366077267, 0.0));
    positionAlloc4->Add (Vector(558.098935802821,431.1571880350662, 0.0));
    positionAlloc4->Add (Vector(797.0480423356603,534.358255798106, 0.0));
    positionAlloc4->Add (Vector(519.3371197870955,512.2227065690038, 0.0));
    positionAlloc4->Add (Vector(568.7277118794017,335.08300855075174, 0.0));
    positionAlloc4->Add (Vector(477.4616884261374,199.61019045841152, 0.0));
    positionAlloc4->Add (Vector(677.7563791089968,227.27596397495364, 0.0));
    positionAlloc4->Add (Vector(260.1649604082073,619.6822966400495, 0.0));
    positionAlloc4->Add (Vector(289.179971200536,505.8841836512389, 0.0));
    positionAlloc4->Add (Vector(312.59135535335054,524.3606838434176, 0.0));
    positionAlloc4->Add (Vector(266.99402040851504,435.0216170192127, 0.0));
    positionAlloc4->Add (Vector(541.9251710751646,360.64335121579313, 0.0));
    positionAlloc4->Add (Vector(630.8491917420117,355.8023720164672, 0.0));
    positionAlloc4->Add (Vector(491.83443910303424,165.79878760140986, 0.0));
    positionAlloc4->Add (Vector(156.47676570513164,472.39620529357904, 0.0));
    positionAlloc4->Add (Vector(414.62174356323663,155.67573782034927, 0.0));
    positionAlloc4->Add (Vector(314.7850741890686,658.8859099420708, 0.0));
    positionAlloc4->Add (Vector(814.378719821773,849.0321234150692, 0.0));
    positionAlloc4->Add (Vector(473.4983312889305,534.4008106690158, 0.0));
    positionAlloc4->Add (Vector(788.739537961258,336.1818452578785, 0.0));
    positionAlloc4->Add (Vector(524.4109290311287,763.6186831845131, 0.0));
    positionAlloc4->Add (Vector(554.6525344293822,413.1568627823053, 0.0));
    positionAlloc4->Add (Vector(158.3099426685515,360.4589483925209, 0.0));
    positionAlloc4->Add (Vector(763.4197789408562,358.58585626358865, 0.0));
    positionAlloc4->Add (Vector(636.7232647370693,436.61355633911637, 0.0));
    positionAlloc4->Add (Vector(202.8881456339387,322.6819008207981, 0.0));
    positionAlloc4->Add (Vector(185.04516896530603,761.5436758442901, 0.0));
    positionAlloc4->Add (Vector(696.5498383558335,284.2414208055697, 0.0));
    positionAlloc4->Add (Vector(703.9469065566341,556.9565900432024, 0.0));
    positionAlloc4->Add (Vector(521.9315548712229,713.9065785088635, 0.0));
    positionAlloc4->Add (Vector(349.39687362742956,503.61481280532035, 0.0));
    positionAlloc4->Add (Vector(443.47447452327606,285.79576935637806, 0.0));
    positionAlloc4->Add (Vector(514.0578782516087,731.1751349469695, 0.0));
    positionAlloc4->Add (Vector(748.0421879479009,630.7374569180188, 0.0));
    positionAlloc4->Add (Vector(734.8403089503073,263.8565527329304, 0.0));
    positionAlloc4->Add (Vector(204.76946900601592,701.6832579616533, 0.0));
    positionAlloc4->Add (Vector(763.494660523145,291.15864788682455, 0.0));
    positionAlloc4->Add (Vector(228.6680824500966,393.4164848915209, 0.0));
    positionAlloc4->Add (Vector(605.8383984667425,749.3995896327336, 0.0));
    positionAlloc4->Add (Vector(676.681717603627,754.7323214058792, 0.0));
    positionAlloc4->Add (Vector(190.80804270364993,848.8948637466418, 0.0));
    positionAlloc4->Add (Vector(740.3994569963345,683.2198945286611, 0.0));
    positionAlloc4->Add (Vector(621.8009597488975,529.0543974857414, 0.0));
    positionAlloc4->Add (Vector(367.42067997673075,472.67871454877775, 0.0));
    positionAlloc4->Add (Vector(592.873924388952,833.2097479191559, 0.0));
    positionAlloc4->Add (Vector(420.54560600473957,297.8840418567375, 0.0));
    positionAlloc4->Add (Vector(458.8407715490527,651.5333838495169, 0.0));
    positionAlloc4->Add (Vector(636.9743642507373,686.049004901007, 0.0));
    positionAlloc4->Add (Vector(386.3975423069571,716.7719806616512, 0.0));
    positionAlloc4->Add (Vector(678.5477428921375,295.6171939132742, 0.0));
    positionAlloc4->Add (Vector(277.63462850475366,809.6106565308447, 0.0));
    positionAlloc4->Add (Vector(581.3569761248671,450.10759922603705, 0.0));
    positionAlloc4->Add (Vector(296.7626454781903,769.2724894673449, 0.0));
    positionAlloc4->Add (Vector(603.9739113233397,657.7727211632828, 0.0));
    positionAlloc4->Add (Vector(713.5123852642395,408.9220379536874, 0.0));
    positionAlloc4->Add (Vector(538.6843258255581,219.95231393808675, 0.0));
    positionAlloc4->Add (Vector(645.8268343545792,407.4930267999607, 0.0));
    positionAlloc4->Add (Vector(335.4063804695,394.9031859557349, 0.0));
    positionAlloc4->Add (Vector(265.3138062214068,395.94915425844874, 0.0));
    positionAlloc4->Add (Vector(326.6896690835921,662.2432665263707, 0.0));
    positionAlloc4->Add (Vector(673.6243175391121,247.27193367424135, 0.0));
    positionAlloc4->Add (Vector(742.5741451863837,503.6159980659127, 0.0));
    positionAlloc4->Add (Vector(335.58628301201765,496.53916195820943, 0.0));
    positionAlloc4->Add (Vector(810.3996750856998,218.9462491755823, 0.0));
    positionAlloc4->Add (Vector(763.0019255912575,710.991767636039, 0.0));
    positionAlloc4->Add (Vector(814.0934427674266,781.571936167362, 0.0));
    positionAlloc4->Add (Vector(526.8284604069385,545.5516540360736, 0.0));
    positionAlloc4->Add (Vector(508.4163723761315,534.1892289154316, 0.0));
    positionAlloc4->Add (Vector(432.87689342006996,305.0507355976969, 0.0));
    positionAlloc4->Add (Vector(577.6625679918573,724.2050724264242, 0.0));
    positionAlloc4->Add (Vector(742.4226089466116,662.6105394257266, 0.0));
    positionAlloc4->Add (Vector(655.0582458903966,211.68859865039042, 0.0));
    positionAlloc4->Add (Vector(442.5786805105689,810.0388979742295, 0.0));
    positionAlloc4->Add (Vector(166.60145221405344,205.85238654380026, 0.0));
    positionAlloc4->Add (Vector(353.19857412033224,528.8316390999171, 0.0));
    positionAlloc4->Add (Vector(653.6291619193669,385.31573700570596, 0.0));
    positionAlloc4->Add (Vector(614.5931352483792,416.77035952826986, 0.0));
    positionAlloc4->Add (Vector(366.83571990003236,515.3191233492348, 0.0));
    positionAlloc4->Add (Vector(252.21183391447576,352.4247776995102, 0.0));
    positionAlloc4->Add (Vector(357.50850734557804,346.09388314276583, 0.0));
    positionAlloc4->Add (Vector(717.5044710616223,181.3260037483667, 0.0));
    positionAlloc4->Add (Vector(631.1729388800002,593.2984376330969, 0.0));
    positionAlloc4->Add (Vector(467.62763217524747,504.8510812540361, 0.0));
    positionAlloc4->Add (Vector(692.4008815273692,240.79438059301881, 0.0));
    positionAlloc4->Add (Vector(319.5371244091995,268.34042439359405, 0.0));
    positionAlloc4->Add (Vector(689.5701651650248,379.0548282130177, 0.0));
    positionAlloc4->Add (Vector(323.975020748965,362.9086781398256, 0.0));
    positionAlloc4->Add (Vector(179.63414476281235,593.4747055313192, 0.0));
    positionAlloc4->Add (Vector(289.5665733074389,365.35856114943556, 0.0));
    positionAlloc4->Add (Vector(492.4375625864309,465.18563544764675, 0.0));
    positionAlloc4->Add (Vector(494.7652978143559,790.4203050850527, 0.0));
    positionAlloc4->Add (Vector(639.0449126023692,642.7551351294816, 0.0));
    positionAlloc4->Add (Vector(384.31472860549536,636.8419214243552, 0.0));
    positionAlloc4->Add (Vector(497.3536356429471,632.9750499884537, 0.0));
    positionAlloc4->Add (Vector(163.21661500336427,220.37125108428512, 0.0));
    positionAlloc4->Add (Vector(664.1409282256825,407.02167010190453, 0.0));
    positionAlloc4->Add (Vector(799.553940865915,467.86761536427565, 0.0));
    positionAlloc4->Add (Vector(483.1595522834069,235.8082095550281, 0.0));
    positionAlloc4->Add (Vector(418.76422598408084,665.7717822807901, 0.0));
    positionAlloc4->Add (Vector(431.8280068381139,773.2117899794641, 0.0));
    positionAlloc4->Add (Vector(779.6171591876847,799.1564866230874, 0.0));
    positionAlloc4->Add (Vector(522.4825807211479,336.46434900101667, 0.0));
    positionAlloc4->Add (Vector(215.12126231862698,562.487137129667, 0.0));
    positionAlloc4->Add (Vector(742.588025725782,239.24980867039, 0.0));
    positionAlloc4->Add (Vector(525.8077525794215,787.5564513569642, 0.0));
    positionAlloc4->Add (Vector(166.4213152048081,380.5916601621101, 0.0));
    positionAlloc4->Add (Vector(195.02345623550846,431.6593776789621, 0.0));
    positionAlloc4->Add (Vector(786.2424208850177,809.3940128187102, 0.0));
    positionAlloc4->Add (Vector(704.3345768955659,558.1130532108955, 0.0));
    positionAlloc4->Add (Vector(500.78379057774373,630.8667089698068, 0.0));
    positionAlloc4->Add (Vector(802.9923549522094,203.2593087358551, 0.0));
    positionAlloc4->Add (Vector(569.505496417335,483.4368054460563, 0.0));
    positionAlloc4->Add (Vector(811.9407744242978,460.0559852141052, 0.0));
    positionAlloc4->Add (Vector(624.9831389927124,549.3929832372246, 0.0));
    positionAlloc4->Add (Vector(667.1446202430184,821.9416827494637, 0.0));
    positionAlloc4->Add (Vector(736.053400071089,215.30116930537795, 0.0));
    positionAlloc4->Add (Vector(803.7011570500175,716.8519700816732, 0.0));
    positionAlloc4->Add (Vector(782.7871876124997,160.9503067152039, 0.0));
    positionAlloc4->Add (Vector(817.7445626927055,719.9461369382641, 0.0));
    positionAlloc4->Add (Vector(659.9296618910143,150.03697885260777, 0.0));
    positionAlloc4->Add (Vector(641.4680318656438,654.6252533364684, 0.0));
    positionAlloc4->Add (Vector(229.7616886111805,623.29925202102, 0.0));
    positionAlloc4->Add (Vector(450.5259359929367,435.51944879525126, 0.0));
    positionAlloc4->Add (Vector(544.1886157244328,154.03946290507758, 0.0));
    positionAlloc4->Add (Vector(312.30172971799385,708.56023247731, 0.0));
    positionAlloc4->Add (Vector(341.7404094409326,289.91108225461056, 0.0));
    positionAlloc4->Add (Vector(172.31935664601448,203.26999228654202, 0.0));
    positionAlloc4->Add (Vector(339.6364562728098,748.7857134730016, 0.0));
    positionAlloc4->Add (Vector(339.6742455098537,349.93861977850395, 0.0));
    positionAlloc4->Add (Vector(747.7639241101806,270.535987198028, 0.0));
    positionAlloc4->Add (Vector(822.0083165733878,724.3229031108206, 0.0));
    positionAlloc4->Add (Vector(688.4189086665112,407.9937283193997, 0.0));
    positionAlloc4->Add (Vector(786.286624340405,596.5513427057658, 0.0));
    positionAlloc4->Add (Vector(611.643040687209,431.25556923335466, 0.0));
    positionAlloc4->Add (Vector(802.8520021388174,573.465009203393, 0.0));
    positionAlloc4->Add (Vector(268.37273029445606,238.2950504070356, 0.0));
    positionAlloc4->Add (Vector(244.35004944220668,497.2528349503718, 0.0));
    positionAlloc4->Add (Vector(530.4289089869542,498.00767111801014, 0.0));
    positionAlloc4->Add (Vector(436.43765064995483,530.9260811593675, 0.0));
    positionAlloc4->Add (Vector(398.59223481800143,435.0150089269875, 0.0));
    positionAlloc4->Add (Vector(663.7442592013904,437.7330354892407, 0.0));
    positionAlloc4->Add (Vector(823.1483446207128,508.0272780298322, 0.0));
    positionAlloc4->Add (Vector(433.4375699888908,288.86077789339095, 0.0));
    positionAlloc4->Add (Vector(486.8917662793235,582.1479756916469, 0.0));
    positionAlloc4->Add (Vector(612.6816744547731,376.0307480045307, 0.0));
    positionAlloc4->Add (Vector(693.6247878574734,536.5555961216926, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_14.xml");
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
	  flowmon->SerializeToXmlFile ("switch_SUI_flowmon/switch_SUI_14.flowmon", false, false);
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