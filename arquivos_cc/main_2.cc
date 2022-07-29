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
	  uint16_t numberOfNodes = 551;
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
    positionAlloc->Add (Vector (850.0,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (850.0,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (850.0,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (850.0,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (850.0,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,461.1111111111111, 0.0));
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
    positionAlloc->Add (Vector (694.4444444444445,150.0, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,850.0, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,383.33333333333337, 0.0));
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
    positionAlloc->Add (Vector (383.33333333333337,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,150.0, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,850.0, 0.0));
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
    positionAlloc->Add (Vector (150.0,616.6666666666667, 0.0));
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
    positionAlloc4->Add (Vector(742.1450077744125,579.5228799662209, 0.0));
    positionAlloc4->Add (Vector(610.9783609690385,686.2811529569103, 0.0));
    positionAlloc4->Add (Vector(677.3995455798783,688.2459474753487, 0.0));
    positionAlloc4->Add (Vector(668.3332923674634,249.995201110587, 0.0));
    positionAlloc4->Add (Vector(666.5086629930711,733.9012179004731, 0.0));
    positionAlloc4->Add (Vector(356.2335774423352,160.020694366942, 0.0));
    positionAlloc4->Add (Vector(623.7749092001769,729.9285096143819, 0.0));
    positionAlloc4->Add (Vector(156.20429957942824,848.4745989773847, 0.0));
    positionAlloc4->Add (Vector(297.4934747070422,820.2136133266673, 0.0));
    positionAlloc4->Add (Vector(713.1060453756535,345.1319106901871, 0.0));
    positionAlloc4->Add (Vector(359.5769118601769,401.1008677919094, 0.0));
    positionAlloc4->Add (Vector(634.7727020759755,244.1045905856853, 0.0));
    positionAlloc4->Add (Vector(631.4054297013924,655.89487038876, 0.0));
    positionAlloc4->Add (Vector(163.3812661008348,483.790427822472, 0.0));
    positionAlloc4->Add (Vector(588.6147577652472,798.4678247110608, 0.0));
    positionAlloc4->Add (Vector(463.8233387585955,507.30274230296254, 0.0));
    positionAlloc4->Add (Vector(193.72031553335,768.4253340902425, 0.0));
    positionAlloc4->Add (Vector(742.8781979449424,775.1631436367904, 0.0));
    positionAlloc4->Add (Vector(747.7733768687955,457.40487164362384, 0.0));
    positionAlloc4->Add (Vector(290.33216086988193,226.484947216735, 0.0));
    positionAlloc4->Add (Vector(551.4702660189013,819.2330005283532, 0.0));
    positionAlloc4->Add (Vector(729.2632454744122,685.1694562583024, 0.0));
    positionAlloc4->Add (Vector(836.5175405084005,435.30894600167187, 0.0));
    positionAlloc4->Add (Vector(554.0953204631919,755.9040019371865, 0.0));
    positionAlloc4->Add (Vector(507.8928235405258,494.62783600082736, 0.0));
    positionAlloc4->Add (Vector(683.2587339643881,420.5783547761195, 0.0));
    positionAlloc4->Add (Vector(350.04848886810913,292.2561637602611, 0.0));
    positionAlloc4->Add (Vector(562.1120654919239,412.7309355437213, 0.0));
    positionAlloc4->Add (Vector(488.38520518120265,754.4832657601816, 0.0));
    positionAlloc4->Add (Vector(473.87292354577545,836.6911647138796, 0.0));
    positionAlloc4->Add (Vector(810.8294656777784,254.15717858358096, 0.0));
    positionAlloc4->Add (Vector(343.03729777594987,763.9634067127323, 0.0));
    positionAlloc4->Add (Vector(237.3745697767938,705.9474637845838, 0.0));
    positionAlloc4->Add (Vector(241.45664067642406,703.3568559924195, 0.0));
    positionAlloc4->Add (Vector(523.3706401357538,758.9784117533794, 0.0));
    positionAlloc4->Add (Vector(786.6182276560694,655.5002648191341, 0.0));
    positionAlloc4->Add (Vector(309.42481527224675,824.8201930158596, 0.0));
    positionAlloc4->Add (Vector(482.88222624077866,530.1172087703642, 0.0));
    positionAlloc4->Add (Vector(426.59642028502304,177.6216730236295, 0.0));
    positionAlloc4->Add (Vector(584.4623358772088,290.9184754757033, 0.0));
    positionAlloc4->Add (Vector(586.384170711608,548.5641528275979, 0.0));
    positionAlloc4->Add (Vector(845.6938722297887,766.2203335042489, 0.0));
    positionAlloc4->Add (Vector(492.22848225471597,431.245160458027, 0.0));
    positionAlloc4->Add (Vector(460.83062444154206,588.3640249947447, 0.0));
    positionAlloc4->Add (Vector(556.3273206955872,247.43769094198686, 0.0));
    positionAlloc4->Add (Vector(540.3961783750308,393.0939273213553, 0.0));
    positionAlloc4->Add (Vector(339.1733844915483,826.4921701064851, 0.0));
    positionAlloc4->Add (Vector(190.35330753931743,389.71358518429935, 0.0));
    positionAlloc4->Add (Vector(331.53442812196613,286.2366856934103, 0.0));
    positionAlloc4->Add (Vector(458.3089452186968,361.63327454604564, 0.0));
    positionAlloc4->Add (Vector(683.331268823409,733.5549733563099, 0.0));
    positionAlloc4->Add (Vector(537.5980397506078,529.1933686870028, 0.0));
    positionAlloc4->Add (Vector(528.9853251433096,257.901530224941, 0.0));
    positionAlloc4->Add (Vector(419.85833331584564,388.27097297815396, 0.0));
    positionAlloc4->Add (Vector(822.794839785315,422.63732643169425, 0.0));
    positionAlloc4->Add (Vector(838.809214247325,351.88380647312204, 0.0));
    positionAlloc4->Add (Vector(572.060013535623,728.6951518190413, 0.0));
    positionAlloc4->Add (Vector(369.77670216414697,771.8825706160768, 0.0));
    positionAlloc4->Add (Vector(231.2972721403679,819.1431802687234, 0.0));
    positionAlloc4->Add (Vector(352.4713403808582,780.0582281115366, 0.0));
    positionAlloc4->Add (Vector(679.3495297651237,470.9661062173235, 0.0));
    positionAlloc4->Add (Vector(262.01745552423586,166.34646742973013, 0.0));
    positionAlloc4->Add (Vector(171.66905364354048,421.59073196280866, 0.0));
    positionAlloc4->Add (Vector(418.94469909006017,177.29720880243812, 0.0));
    positionAlloc4->Add (Vector(503.81858253731497,824.8121849823916, 0.0));
    positionAlloc4->Add (Vector(489.91278976157844,169.27570390862584, 0.0));
    positionAlloc4->Add (Vector(631.791965046166,248.38707730485254, 0.0));
    positionAlloc4->Add (Vector(841.6251763089143,467.8011348372804, 0.0));
    positionAlloc4->Add (Vector(405.215609738278,302.52478797849307, 0.0));
    positionAlloc4->Add (Vector(623.9844478145066,574.0454746168832, 0.0));
    positionAlloc4->Add (Vector(436.7402264266624,367.8522465443755, 0.0));
    positionAlloc4->Add (Vector(388.4229166069042,200.1983221933557, 0.0));
    positionAlloc4->Add (Vector(602.0756704005389,595.8316222183203, 0.0));
    positionAlloc4->Add (Vector(258.12182305632666,167.81343193402563, 0.0));
    positionAlloc4->Add (Vector(773.0679246286659,392.11422251650015, 0.0));
    positionAlloc4->Add (Vector(451.3705369235453,249.81590085733612, 0.0));
    positionAlloc4->Add (Vector(412.18702091408494,266.802141189127, 0.0));
    positionAlloc4->Add (Vector(643.9101038506772,631.8835129444958, 0.0));
    positionAlloc4->Add (Vector(409.8206964063393,570.7519167601631, 0.0));
    positionAlloc4->Add (Vector(373.2067728247972,557.5391226158797, 0.0));
    positionAlloc4->Add (Vector(791.6446650888452,451.6896543593074, 0.0));
    positionAlloc4->Add (Vector(725.6956615677949,750.2936363628986, 0.0));
    positionAlloc4->Add (Vector(462.4936614811075,708.8662383628467, 0.0));
    positionAlloc4->Add (Vector(364.4194491447489,702.2896119445553, 0.0));
    positionAlloc4->Add (Vector(621.0766356761027,342.76682197879296, 0.0));
    positionAlloc4->Add (Vector(634.7449786684576,450.95175417310367, 0.0));
    positionAlloc4->Add (Vector(606.6041540143578,741.9230540929348, 0.0));
    positionAlloc4->Add (Vector(390.21714722086074,660.4214412846136, 0.0));
    positionAlloc4->Add (Vector(236.10230658662195,541.0496775152977, 0.0));
    positionAlloc4->Add (Vector(554.0399207141855,180.61735061626314, 0.0));
    positionAlloc4->Add (Vector(356.5284953579094,490.20034565832214, 0.0));
    positionAlloc4->Add (Vector(158.19462751163286,445.4239820893473, 0.0));
    positionAlloc4->Add (Vector(523.7772307834346,249.49598665542038, 0.0));
    positionAlloc4->Add (Vector(781.7173151000892,196.5106910490424, 0.0));
    positionAlloc4->Add (Vector(656.7811388262656,234.39089148258319, 0.0));
    positionAlloc4->Add (Vector(578.8014278413734,761.5890154264415, 0.0));
    positionAlloc4->Add (Vector(595.2672324866795,789.3008633033658, 0.0));
    positionAlloc4->Add (Vector(565.5625538459769,157.09182881942036, 0.0));
    positionAlloc4->Add (Vector(625.0689696707954,403.34399358650546, 0.0));
    positionAlloc4->Add (Vector(809.2079981332475,618.0008527060398, 0.0));
    positionAlloc4->Add (Vector(206.1260098573324,398.33990428518894, 0.0));
    positionAlloc4->Add (Vector(675.8246334658927,199.70536517327884, 0.0));
    positionAlloc4->Add (Vector(234.71750033963514,443.2634531312328, 0.0));
    positionAlloc4->Add (Vector(469.3655562688794,408.3371152326416, 0.0));
    positionAlloc4->Add (Vector(230.14134664613357,418.6872181314856, 0.0));
    positionAlloc4->Add (Vector(572.6568170645714,630.6129394448247, 0.0));
    positionAlloc4->Add (Vector(330.32505423360533,191.66729024344016, 0.0));
    positionAlloc4->Add (Vector(595.4809591283849,185.7947670094705, 0.0));
    positionAlloc4->Add (Vector(724.8387977851586,151.09557363403104, 0.0));
    positionAlloc4->Add (Vector(726.6532689549063,818.5423580394922, 0.0));
    positionAlloc4->Add (Vector(575.5336498634927,366.56090250394743, 0.0));
    positionAlloc4->Add (Vector(785.3277017895884,649.8228511267995, 0.0));
    positionAlloc4->Add (Vector(445.27972497718196,437.71475268588205, 0.0));
    positionAlloc4->Add (Vector(342.31206786366363,426.20325291835246, 0.0));
    positionAlloc4->Add (Vector(844.9717872993492,242.04238392858713, 0.0));
    positionAlloc4->Add (Vector(495.48260623808306,641.581136222165, 0.0));
    positionAlloc4->Add (Vector(505.1883571024662,761.955767279623, 0.0));
    positionAlloc4->Add (Vector(229.68687543362097,391.6561999611845, 0.0));
    positionAlloc4->Add (Vector(210.96493675704005,581.9465612824608, 0.0));
    positionAlloc4->Add (Vector(802.2276036029715,427.91052886603416, 0.0));
    positionAlloc4->Add (Vector(508.8674081694915,355.4421762456851, 0.0));
    positionAlloc4->Add (Vector(711.809975705869,550.4340503585106, 0.0));
    positionAlloc4->Add (Vector(225.61802789148396,387.20245453712613, 0.0));
    positionAlloc4->Add (Vector(303.3481663713004,175.7049537505748, 0.0));
    positionAlloc4->Add (Vector(153.8540594122946,155.36482285445774, 0.0));
    positionAlloc4->Add (Vector(294.89480490689874,511.60083991218534, 0.0));
    positionAlloc4->Add (Vector(577.5460497530378,346.478250861182, 0.0));
    positionAlloc4->Add (Vector(487.42874067845776,287.26839891687507, 0.0));
    positionAlloc4->Add (Vector(830.0728836307354,381.9429049721355, 0.0));
    positionAlloc4->Add (Vector(248.01686828732238,343.7700154539141, 0.0));
    positionAlloc4->Add (Vector(480.7453485045595,181.95412185166862, 0.0));
    positionAlloc4->Add (Vector(328.0741065892929,403.2971652435971, 0.0));
    positionAlloc4->Add (Vector(593.872821963432,199.6525512936414, 0.0));
    positionAlloc4->Add (Vector(670.5157413235919,504.3862416466419, 0.0));
    positionAlloc4->Add (Vector(815.3717368103424,570.4557465838491, 0.0));
    positionAlloc4->Add (Vector(491.36694803488507,606.5569816818307, 0.0));
    positionAlloc4->Add (Vector(222.95736196584338,419.1579293513688, 0.0));
    positionAlloc4->Add (Vector(316.2034278623644,508.6546442820535, 0.0));
    positionAlloc4->Add (Vector(564.1880136144279,641.8436411598614, 0.0));
    positionAlloc4->Add (Vector(574.9627051031277,591.9152871643921, 0.0));
    positionAlloc4->Add (Vector(774.7663029683922,460.3660100654314, 0.0));
    positionAlloc4->Add (Vector(691.7157590111094,571.3464017105921, 0.0));
    positionAlloc4->Add (Vector(371.86261145378353,454.21717260109824, 0.0));
    positionAlloc4->Add (Vector(512.212446228529,451.2991872246931, 0.0));
    positionAlloc4->Add (Vector(765.2852301975871,550.2406464338255, 0.0));
    positionAlloc4->Add (Vector(618.440184056655,730.5007595217289, 0.0));
    positionAlloc4->Add (Vector(215.70009937634933,786.7387736056147, 0.0));
    positionAlloc4->Add (Vector(569.2849557240474,230.4530979416968, 0.0));
    positionAlloc4->Add (Vector(194.54863650064965,731.9614600699969, 0.0));
    positionAlloc4->Add (Vector(748.9872656862776,515.0119823305126, 0.0));
    positionAlloc4->Add (Vector(557.9647845584427,846.806261333917, 0.0));
    positionAlloc4->Add (Vector(706.2047950901332,174.2691867085383, 0.0));
    positionAlloc4->Add (Vector(205.27841353358002,317.6571189366767, 0.0));
    positionAlloc4->Add (Vector(774.6117486977565,279.73112567010014, 0.0));
    positionAlloc4->Add (Vector(617.4632782584845,565.255099075169, 0.0));
    positionAlloc4->Add (Vector(782.026496634391,746.5847120738271, 0.0));
    positionAlloc4->Add (Vector(670.2352391121224,183.6999851848474, 0.0));
    positionAlloc4->Add (Vector(358.0664653960321,359.9433780548893, 0.0));
    positionAlloc4->Add (Vector(360.88568253200026,568.8502390084311, 0.0));
    positionAlloc4->Add (Vector(158.68282539412036,478.1375758251541, 0.0));
    positionAlloc4->Add (Vector(217.57412913680855,319.3256797811873, 0.0));
    positionAlloc4->Add (Vector(642.580184222557,528.4889336514373, 0.0));
    positionAlloc4->Add (Vector(760.9276876962896,228.8062732883559, 0.0));
    positionAlloc4->Add (Vector(730.5762654011928,347.06483889045097, 0.0));
    positionAlloc4->Add (Vector(658.6696944492262,363.3257667702933, 0.0));
    positionAlloc4->Add (Vector(162.60458892031815,823.9672499518182, 0.0));
    positionAlloc4->Add (Vector(677.7733325177487,713.434743789007, 0.0));
    positionAlloc4->Add (Vector(354.55276132332847,787.5391131507406, 0.0));
    positionAlloc4->Add (Vector(346.0261306853426,750.8391737593846, 0.0));
    positionAlloc4->Add (Vector(667.9328845368512,792.6535865394054, 0.0));
    positionAlloc4->Add (Vector(445.78389981836125,179.4172726199837, 0.0));
    positionAlloc4->Add (Vector(833.7602299574615,227.50441532842098, 0.0));
    positionAlloc4->Add (Vector(532.6537541314519,416.28038427928675, 0.0));
    positionAlloc4->Add (Vector(472.2507898413484,755.194526665869, 0.0));
    positionAlloc4->Add (Vector(227.69262844943677,462.9388806644978, 0.0));
    positionAlloc4->Add (Vector(619.5126183340838,542.308441686308, 0.0));
    positionAlloc4->Add (Vector(332.2008002962021,785.036521494081, 0.0));
    positionAlloc4->Add (Vector(701.913140277309,615.4894324331967, 0.0));
    positionAlloc4->Add (Vector(759.8431375841365,686.2453435747118, 0.0));
    positionAlloc4->Add (Vector(607.2139533718981,806.8133729139379, 0.0));
    positionAlloc4->Add (Vector(173.23254819369836,362.76248086850666, 0.0));
    positionAlloc4->Add (Vector(363.7174736877119,298.85106345976726, 0.0));
    positionAlloc4->Add (Vector(635.9054014568817,508.85832720153707, 0.0));
    positionAlloc4->Add (Vector(345.49326191808734,600.0173370410953, 0.0));
    positionAlloc4->Add (Vector(309.0256899164604,735.928787670058, 0.0));
    positionAlloc4->Add (Vector(272.41700602317155,749.0160812340505, 0.0));
    positionAlloc4->Add (Vector(630.4290376802683,198.74880371368403, 0.0));
    positionAlloc4->Add (Vector(623.3242934528039,377.5728108021893, 0.0));
    positionAlloc4->Add (Vector(593.8994968070158,238.3853190951519, 0.0));
    positionAlloc4->Add (Vector(479.9010171787152,835.3734566106082, 0.0));
    positionAlloc4->Add (Vector(396.66246262675804,739.013287159332, 0.0));
    positionAlloc4->Add (Vector(605.8033094171719,584.6499972349018, 0.0));
    positionAlloc4->Add (Vector(465.24568523378883,674.1784358743146, 0.0));
    positionAlloc4->Add (Vector(249.38039283269575,393.5589881075166, 0.0));
    positionAlloc4->Add (Vector(220.44623496647333,317.9118789291613, 0.0));
    positionAlloc4->Add (Vector(648.4098142315465,728.1847488956572, 0.0));
    positionAlloc4->Add (Vector(384.64563315961095,271.63311519502065, 0.0));
    positionAlloc4->Add (Vector(495.9487130752099,729.7744767060528, 0.0));
    positionAlloc4->Add (Vector(471.4856380212497,400.45878572780924, 0.0));
    positionAlloc4->Add (Vector(479.81752925297025,725.1458283973302, 0.0));
    positionAlloc4->Add (Vector(647.169959036318,370.0450792286239, 0.0));
    positionAlloc4->Add (Vector(723.3444597031593,528.5556284898811, 0.0));
    positionAlloc4->Add (Vector(281.18836086830754,750.7239372100148, 0.0));
    positionAlloc4->Add (Vector(205.58460925012065,662.0475432382536, 0.0));
    positionAlloc4->Add (Vector(653.547535658411,773.1806371124615, 0.0));
    positionAlloc4->Add (Vector(541.8289785468678,519.1797354858734, 0.0));
    positionAlloc4->Add (Vector(263.56338404773555,158.82526398303133, 0.0));
    positionAlloc4->Add (Vector(767.3494308132508,449.2427815444306, 0.0));
    positionAlloc4->Add (Vector(594.269891277759,266.1684267070495, 0.0));
    positionAlloc4->Add (Vector(838.8772233255485,617.543999162539, 0.0));
    positionAlloc4->Add (Vector(826.9886306814193,442.43268549256976, 0.0));
    positionAlloc4->Add (Vector(712.5224748090747,503.64474150364674, 0.0));
    positionAlloc4->Add (Vector(208.42485949256832,767.6914578851371, 0.0));
    positionAlloc4->Add (Vector(458.2263928578443,580.7086175488614, 0.0));
    positionAlloc4->Add (Vector(608.0047866274733,256.472998255591, 0.0));
    positionAlloc4->Add (Vector(278.9573874666183,775.7927970055093, 0.0));
    positionAlloc4->Add (Vector(248.4980696341309,291.410967881743, 0.0));
    positionAlloc4->Add (Vector(343.83314116446627,838.4650309006554, 0.0));
    positionAlloc4->Add (Vector(750.025479899127,784.3002783360365, 0.0));
    positionAlloc4->Add (Vector(705.2829453067781,371.7331739472687, 0.0));
    positionAlloc4->Add (Vector(260.5683759847436,637.1293827181617, 0.0));
    positionAlloc4->Add (Vector(790.5630478849303,523.120207072298, 0.0));
    positionAlloc4->Add (Vector(708.5399117311258,427.1793308637184, 0.0));
    positionAlloc4->Add (Vector(554.3153561168103,594.2076922852126, 0.0));
    positionAlloc4->Add (Vector(350.9965706592029,704.9980933236875, 0.0));
    positionAlloc4->Add (Vector(292.6365213893823,516.3307418363189, 0.0));
    positionAlloc4->Add (Vector(692.9282956477292,825.1336577305681, 0.0));
    positionAlloc4->Add (Vector(725.4722124201578,416.29400525529365, 0.0));
    positionAlloc4->Add (Vector(223.8315899696414,609.7460733070059, 0.0));
    positionAlloc4->Add (Vector(527.0912585669,498.46875562400317, 0.0));
    positionAlloc4->Add (Vector(474.75196376863295,786.4541441030921, 0.0));
    positionAlloc4->Add (Vector(169.01719485789187,195.64321388234444, 0.0));
    positionAlloc4->Add (Vector(839.4927592986126,690.4087334169933, 0.0));
    positionAlloc4->Add (Vector(317.1573479670775,465.8675409266917, 0.0));
    positionAlloc4->Add (Vector(765.7072984660705,759.4048975322048, 0.0));
    positionAlloc4->Add (Vector(293.4168919054988,764.4810547064286, 0.0));
    positionAlloc4->Add (Vector(679.7260539820445,513.5701016527562, 0.0));
    positionAlloc4->Add (Vector(265.29444740260715,381.15054541004827, 0.0));
    positionAlloc4->Add (Vector(290.9546474951821,331.03458442162713, 0.0));
    positionAlloc4->Add (Vector(186.3696042879879,700.3947395688232, 0.0));
    positionAlloc4->Add (Vector(714.4825320629876,471.4679143178706, 0.0));
    positionAlloc4->Add (Vector(193.59117411731992,508.7518541392322, 0.0));
    positionAlloc4->Add (Vector(605.681427005668,531.8023085848338, 0.0));
    positionAlloc4->Add (Vector(473.606892700425,438.235879109156, 0.0));
    positionAlloc4->Add (Vector(744.9827600364633,618.3156361303759, 0.0));
    positionAlloc4->Add (Vector(266.04217811429305,811.9887029001976, 0.0));
    positionAlloc4->Add (Vector(470.8104490700595,489.2874009910329, 0.0));
    positionAlloc4->Add (Vector(530.4767521398522,207.20774202274478, 0.0));
    positionAlloc4->Add (Vector(197.3678533300011,645.484568060838, 0.0));
    positionAlloc4->Add (Vector(752.0977853446045,655.9700122459386, 0.0));
    positionAlloc4->Add (Vector(516.3736379620566,230.33821030845223, 0.0));
    positionAlloc4->Add (Vector(745.4312404887964,542.8559517977674, 0.0));
    positionAlloc4->Add (Vector(307.7218509658646,325.6766629382086, 0.0));
    positionAlloc4->Add (Vector(229.86704942155336,255.8727801144542, 0.0));
    positionAlloc4->Add (Vector(575.9690908875266,394.6544604607732, 0.0));
    positionAlloc4->Add (Vector(474.75274068183535,830.4037307420759, 0.0));
    positionAlloc4->Add (Vector(288.6683429318181,545.7311782001269, 0.0));
    positionAlloc4->Add (Vector(329.57104371097347,607.9222210318521, 0.0));
    positionAlloc4->Add (Vector(661.5746754865108,836.9079633480727, 0.0));
    positionAlloc4->Add (Vector(443.15532335191295,332.1104536734358, 0.0));
    positionAlloc4->Add (Vector(805.7757234231643,186.58051550021662, 0.0));
    positionAlloc4->Add (Vector(824.3761039494852,542.1177284232893, 0.0));
    positionAlloc4->Add (Vector(165.27637774284264,614.6527803229123, 0.0));
    positionAlloc4->Add (Vector(393.1403283811899,779.8833419810908, 0.0));
    positionAlloc4->Add (Vector(522.694477592652,437.7669218204946, 0.0));
    positionAlloc4->Add (Vector(784.5062173410877,247.4717597823166, 0.0));
    positionAlloc4->Add (Vector(173.52952012829783,193.85724241874817, 0.0));
    positionAlloc4->Add (Vector(195.75631029506968,491.299835571369, 0.0));
    positionAlloc4->Add (Vector(464.817889715378,693.6973898686268, 0.0));
    positionAlloc4->Add (Vector(340.3648082900417,681.5787836925746, 0.0));
    positionAlloc4->Add (Vector(784.3782063647257,324.68233989465426, 0.0));
    positionAlloc4->Add (Vector(333.19397861513414,322.8851593503999, 0.0));
    positionAlloc4->Add (Vector(189.93830480587803,347.5309522852401, 0.0));
    positionAlloc4->Add (Vector(819.6302347584206,198.27061144949482, 0.0));
    positionAlloc4->Add (Vector(675.962560220332,609.8686093869976, 0.0));
    positionAlloc4->Add (Vector(577.1576046343489,244.74785759437987, 0.0));
    positionAlloc4->Add (Vector(685.8590292937769,261.2357173685475, 0.0));
    positionAlloc4->Add (Vector(455.7954918306257,660.6557600566778, 0.0));
    positionAlloc4->Add (Vector(659.5575148816356,497.52280664870983, 0.0));
    positionAlloc4->Add (Vector(243.43598069116064,206.42105962998727, 0.0));
    positionAlloc4->Add (Vector(470.7308207165249,488.0782104597487, 0.0));
    positionAlloc4->Add (Vector(280.27099249239416,711.8503168569973, 0.0));
    positionAlloc4->Add (Vector(760.9111753392212,739.3534967542661, 0.0));
    positionAlloc4->Add (Vector(331.5077312060416,450.7597689172546, 0.0));
    positionAlloc4->Add (Vector(359.1079015121411,808.4754715807384, 0.0));
    positionAlloc4->Add (Vector(341.1495669809687,512.6551363671708, 0.0));
    positionAlloc4->Add (Vector(166.93423472394866,732.1879305730414, 0.0));
    positionAlloc4->Add (Vector(384.1579021193525,168.33769597244535, 0.0));
    positionAlloc4->Add (Vector(279.1333771935475,403.5648091566019, 0.0));
    positionAlloc4->Add (Vector(550.3105898590306,561.4385526035387, 0.0));
    positionAlloc4->Add (Vector(737.8615999449321,418.6830663531308, 0.0));
    positionAlloc4->Add (Vector(232.8353336463711,436.93342278740175, 0.0));
    positionAlloc4->Add (Vector(595.3241552957629,699.9561326561624, 0.0));
    positionAlloc4->Add (Vector(599.795639674634,444.8736373058504, 0.0));
    positionAlloc4->Add (Vector(848.8432579750647,803.1793319404903, 0.0));
    positionAlloc4->Add (Vector(568.7577058446172,664.9641799674329, 0.0));
    positionAlloc4->Add (Vector(212.02030077573207,304.42836689784787, 0.0));
    positionAlloc4->Add (Vector(379.73492752651805,453.81355632171267, 0.0));
    positionAlloc4->Add (Vector(458.1392985688434,253.21676869439875, 0.0));
    positionAlloc4->Add (Vector(702.0304752706845,512.0883395091266, 0.0));
    positionAlloc4->Add (Vector(699.4278839780272,645.376654318641, 0.0));
    positionAlloc4->Add (Vector(190.98565047078543,383.9668230872426, 0.0));
    positionAlloc4->Add (Vector(498.77897758441753,203.94079539403182, 0.0));
    positionAlloc4->Add (Vector(338.8745040897501,727.5345871260434, 0.0));
    positionAlloc4->Add (Vector(623.5275560584536,713.4922997765389, 0.0));
    positionAlloc4->Add (Vector(542.4620660913197,572.4413666175184, 0.0));
    positionAlloc4->Add (Vector(555.988651199692,543.0392115716666, 0.0));
    positionAlloc4->Add (Vector(599.6278681336591,195.39751613807783, 0.0));
    positionAlloc4->Add (Vector(561.7415868638734,486.68102292028664, 0.0));
    positionAlloc4->Add (Vector(705.9381318939589,215.0934478761925, 0.0));
    positionAlloc4->Add (Vector(583.7125362218885,398.84510730895016, 0.0));
    positionAlloc4->Add (Vector(636.9327025477648,152.25278452299509, 0.0));
    positionAlloc4->Add (Vector(424.06798872448053,438.8352885940366, 0.0));
    positionAlloc4->Add (Vector(770.3370463255521,565.1310339307489, 0.0));
    positionAlloc4->Add (Vector(464.0274416681798,655.449656741916, 0.0));
    positionAlloc4->Add (Vector(847.4340363390731,407.8166713547938, 0.0));
    positionAlloc4->Add (Vector(363.9229508431504,763.6555332797404, 0.0));
    positionAlloc4->Add (Vector(378.8521314366554,254.22163523271126, 0.0));
    positionAlloc4->Add (Vector(820.6514360835662,407.5585259114591, 0.0));
    positionAlloc4->Add (Vector(672.0136479118624,291.73639211566615, 0.0));
    positionAlloc4->Add (Vector(682.6935320196657,174.71794845816734, 0.0));
    positionAlloc4->Add (Vector(537.9585598924116,153.66072948003892, 0.0));
    positionAlloc4->Add (Vector(398.7691980800176,698.654249139778, 0.0));
    positionAlloc4->Add (Vector(581.1898151821254,281.6173142307913, 0.0));
    positionAlloc4->Add (Vector(538.5594645116004,663.1499072207723, 0.0));
    positionAlloc4->Add (Vector(710.0496780894116,292.82494424022923, 0.0));
    positionAlloc4->Add (Vector(219.42466971105114,431.04602110290176, 0.0));
    positionAlloc4->Add (Vector(658.5232005791033,844.2669659445544, 0.0));
    positionAlloc4->Add (Vector(156.5321338789653,322.6752051651361, 0.0));
    positionAlloc4->Add (Vector(262.0501444682011,473.09023209385293, 0.0));
    positionAlloc4->Add (Vector(595.4039477854736,682.7850759302106, 0.0));
    positionAlloc4->Add (Vector(632.2197114350635,761.3260411025653, 0.0));
    positionAlloc4->Add (Vector(358.2839159231744,545.5153950013113, 0.0));
    positionAlloc4->Add (Vector(563.243383321141,505.852382841396, 0.0));
    positionAlloc4->Add (Vector(662.9610813363754,529.5660438442937, 0.0));
    positionAlloc4->Add (Vector(504.0968850662803,738.49211960784, 0.0));
    positionAlloc4->Add (Vector(722.4497308998616,299.4579564174703, 0.0));
    positionAlloc4->Add (Vector(242.21014141819333,658.1859369897068, 0.0));
    positionAlloc4->Add (Vector(180.22161505790817,265.370371010606, 0.0));
    positionAlloc4->Add (Vector(839.2688176671558,806.1716465479833, 0.0));
    positionAlloc4->Add (Vector(274.2001386287948,531.5188432278569, 0.0));
    positionAlloc4->Add (Vector(693.5132125498072,609.31328637675, 0.0));
    positionAlloc4->Add (Vector(739.3989870230431,659.9434740131292, 0.0));
    positionAlloc4->Add (Vector(678.5293501223948,472.377648091898, 0.0));
    positionAlloc4->Add (Vector(793.2485735476881,326.5687141213531, 0.0));
    positionAlloc4->Add (Vector(752.9589974544052,742.5600370831946, 0.0));
    positionAlloc4->Add (Vector(350.1347524674121,721.1628572780321, 0.0));
    positionAlloc4->Add (Vector(770.8474301464887,602.0675688586442, 0.0));
    positionAlloc4->Add (Vector(235.05096341064262,288.2293429019561, 0.0));
    positionAlloc4->Add (Vector(810.1530864212954,318.4848982827535, 0.0));
    positionAlloc4->Add (Vector(457.58349278611655,579.0577077461678, 0.0));
    positionAlloc4->Add (Vector(410.5169166862593,452.4538180609372, 0.0));
    positionAlloc4->Add (Vector(799.6585482282348,307.23092829069634, 0.0));
    positionAlloc4->Add (Vector(239.66702245634002,386.78725419614887, 0.0));
    positionAlloc4->Add (Vector(718.2388516630963,609.5350251320518, 0.0));
    positionAlloc4->Add (Vector(557.6958607450725,673.6512256006724, 0.0));
    positionAlloc4->Add (Vector(835.9792305511954,571.3171127454254, 0.0));
    positionAlloc4->Add (Vector(709.0999341582022,354.9754256906753, 0.0));
    positionAlloc4->Add (Vector(179.3658564021454,827.9421443160686, 0.0));
    positionAlloc4->Add (Vector(600.7329774787916,521.2114932744108, 0.0));
    positionAlloc4->Add (Vector(580.5090776272066,692.2436464464516, 0.0));
    positionAlloc4->Add (Vector(293.9810835368886,234.97049271158448, 0.0));
    positionAlloc4->Add (Vector(587.2644981979847,417.35774775702714, 0.0));
    positionAlloc4->Add (Vector(356.81134423419417,688.4562034866935, 0.0));
    positionAlloc4->Add (Vector(207.52016723349914,827.5020243915213, 0.0));
    positionAlloc4->Add (Vector(335.5324998275028,164.66147272163897, 0.0));
    positionAlloc4->Add (Vector(612.2119037909367,685.0608635552152, 0.0));
    positionAlloc4->Add (Vector(271.87653473385905,218.87695479937813, 0.0));
    positionAlloc4->Add (Vector(612.9594277492901,279.63689750722756, 0.0));
    positionAlloc4->Add (Vector(394.7922608220947,692.2574718268273, 0.0));
    positionAlloc4->Add (Vector(397.2759859514449,357.57490690135864, 0.0));
    positionAlloc4->Add (Vector(819.550978809636,666.2558516213048, 0.0));
    positionAlloc4->Add (Vector(199.87970943568104,158.4759326644212, 0.0));
    positionAlloc4->Add (Vector(651.4263716438776,200.45903519907137, 0.0));
    positionAlloc4->Add (Vector(179.171030421669,209.35441123206226, 0.0));
    positionAlloc4->Add (Vector(508.55534456887216,626.1594454458473, 0.0));
    positionAlloc4->Add (Vector(255.11830057192915,666.2036381050399, 0.0));
    positionAlloc4->Add (Vector(196.65642839248298,512.8856153616429, 0.0));
    positionAlloc4->Add (Vector(595.6687775170451,579.7535093320432, 0.0));
    positionAlloc4->Add (Vector(611.5477455440479,776.5824184945754, 0.0));
    positionAlloc4->Add (Vector(473.03422523731393,596.0410337657963, 0.0));
    positionAlloc4->Add (Vector(176.8984442736275,781.2333944947965, 0.0));
    positionAlloc4->Add (Vector(328.2694354322358,738.6101055981459, 0.0));
    positionAlloc4->Add (Vector(631.6942942605795,456.32218683877437, 0.0));
    positionAlloc4->Add (Vector(700.8323703787065,370.7848270236089, 0.0));
    positionAlloc4->Add (Vector(774.4592822581641,818.7342805527497, 0.0));
    positionAlloc4->Add (Vector(471.61273788419066,699.5115806859399, 0.0));
    positionAlloc4->Add (Vector(413.12179960203855,426.13339499902474, 0.0));
    positionAlloc4->Add (Vector(666.1499541714597,483.21269055813656, 0.0));
    positionAlloc4->Add (Vector(255.03406528578898,377.273148823042, 0.0));
    positionAlloc4->Add (Vector(218.65780654897122,491.7687959152447, 0.0));
    positionAlloc4->Add (Vector(468.06413419197264,741.0499579177311, 0.0));
    positionAlloc4->Add (Vector(226.81533907425245,533.5716422815851, 0.0));
    positionAlloc4->Add (Vector(821.7036510247234,491.49783074845374, 0.0));
    positionAlloc4->Add (Vector(833.1116284370377,711.0759783180927, 0.0));
    positionAlloc4->Add (Vector(158.4845422507253,318.84102031967905, 0.0));
    positionAlloc4->Add (Vector(524.7170920168141,300.6413104120662, 0.0));
    positionAlloc4->Add (Vector(682.2028761854945,245.97679401029933, 0.0));
    positionAlloc4->Add (Vector(163.43459377613968,331.86924612152393, 0.0));
    positionAlloc4->Add (Vector(441.30779872583304,486.9409061447265, 0.0));
    positionAlloc4->Add (Vector(688.5134255489379,157.72188510005304, 0.0));
    positionAlloc4->Add (Vector(729.276526896812,663.5300663929814, 0.0));
    positionAlloc4->Add (Vector(407.43154561410273,439.8908267702489, 0.0));
    positionAlloc4->Add (Vector(464.84925417492576,286.23575827453556, 0.0));
    positionAlloc4->Add (Vector(241.49726371813964,770.6229506683267, 0.0));
    positionAlloc4->Add (Vector(518.9290247381562,448.25836872326005, 0.0));
    positionAlloc4->Add (Vector(364.3775345506282,586.7270854041469, 0.0));
    positionAlloc4->Add (Vector(719.7791886542132,580.6536067617076, 0.0));
    positionAlloc4->Add (Vector(506.5954897048528,847.6877976970837, 0.0));
    positionAlloc4->Add (Vector(634.5129391858923,193.60425869104387, 0.0));
    positionAlloc4->Add (Vector(362.2926032973169,220.7320677074527, 0.0));
    positionAlloc4->Add (Vector(249.5745580810373,234.96945759570022, 0.0));
    positionAlloc4->Add (Vector(838.5752476843895,317.42607295114794, 0.0));
    positionAlloc4->Add (Vector(190.44639269745664,636.5246602009872, 0.0));
    positionAlloc4->Add (Vector(190.71465685383305,846.406422018837, 0.0));
    positionAlloc4->Add (Vector(696.5331102479079,471.8602966385241, 0.0));
    positionAlloc4->Add (Vector(256.4173971900537,192.9447389774677, 0.0));
    positionAlloc4->Add (Vector(583.7115684284806,605.1407646787073, 0.0));
    positionAlloc4->Add (Vector(598.5784202831444,301.2267277567197, 0.0));
    positionAlloc4->Add (Vector(457.6581820430371,223.96527898322074, 0.0));
    positionAlloc4->Add (Vector(368.2712931596817,504.2993039881709, 0.0));
    positionAlloc4->Add (Vector(826.075512807254,452.7485148310924, 0.0));
    positionAlloc4->Add (Vector(824.1805781124217,534.6706751265119, 0.0));
    positionAlloc4->Add (Vector(168.0683348646409,244.61640240837593, 0.0));
    positionAlloc4->Add (Vector(407.274095604072,197.7206165698455, 0.0));
    positionAlloc4->Add (Vector(429.06411944061915,252.46179451357438, 0.0));
    positionAlloc4->Add (Vector(272.38703908228877,501.7962535244097, 0.0));
    positionAlloc4->Add (Vector(574.3770564256843,685.3668606320969, 0.0));
    positionAlloc4->Add (Vector(795.930033359376,490.94334317325513, 0.0));
    positionAlloc4->Add (Vector(242.76727091929388,753.4101375908358, 0.0));
    positionAlloc4->Add (Vector(598.6804541417324,725.7363945440527, 0.0));
    positionAlloc4->Add (Vector(392.26980857644986,832.9271406019404, 0.0));
    positionAlloc4->Add (Vector(271.46716049055607,636.4657622317231, 0.0));
    positionAlloc4->Add (Vector(206.64349813587654,432.08612513561303, 0.0));
    positionAlloc4->Add (Vector(703.021305414943,363.27539736862855, 0.0));
    positionAlloc4->Add (Vector(757.6396830752152,714.1477800823347, 0.0));
    positionAlloc4->Add (Vector(416.43630543360604,764.5820243115487, 0.0));
    positionAlloc4->Add (Vector(231.45647851983327,722.2147959438341, 0.0));
    positionAlloc4->Add (Vector(295.9573600501351,801.4166502507413, 0.0));
    positionAlloc4->Add (Vector(844.5106103763188,745.9974704215671, 0.0));
    positionAlloc4->Add (Vector(793.5743812500922,497.87443803408115, 0.0));
    positionAlloc4->Add (Vector(570.4160333524321,510.9755993875484, 0.0));
    positionAlloc4->Add (Vector(427.8870619519984,463.041778353385, 0.0));
    positionAlloc4->Add (Vector(537.6615684986634,363.86701746061647, 0.0));
    positionAlloc4->Add (Vector(211.34992273468697,805.870476121744, 0.0));
    positionAlloc4->Add (Vector(334.17879893759005,402.09548523006333, 0.0));
    positionAlloc4->Add (Vector(175.31226636733348,362.18984916173, 0.0));
    positionAlloc4->Add (Vector(697.4196613692701,646.1212884200835, 0.0));
    positionAlloc4->Add (Vector(239.39943854466438,728.2473411767699, 0.0));
    positionAlloc4->Add (Vector(212.38710248121163,678.6892943477656, 0.0));
    positionAlloc4->Add (Vector(514.6705840904295,586.5099816019317, 0.0));
    positionAlloc4->Add (Vector(527.9699911963917,739.4233060634949, 0.0));
    positionAlloc4->Add (Vector(844.7384515108627,798.9006775470067, 0.0));
    positionAlloc4->Add (Vector(429.5211410665157,256.6912093312922, 0.0));
    positionAlloc4->Add (Vector(799.0244736356169,314.24945729881745, 0.0));
    positionAlloc4->Add (Vector(615.4382071410525,706.8149725528104, 0.0));
    positionAlloc4->Add (Vector(679.3143302266723,393.3000669672874, 0.0));
    positionAlloc4->Add (Vector(217.67351475270374,465.2596553807296, 0.0));
    positionAlloc4->Add (Vector(290.65358206133953,525.0793791311569, 0.0));
    positionAlloc4->Add (Vector(272.71205965036177,633.0034235744661, 0.0));
    positionAlloc4->Add (Vector(543.607771114537,820.3376991537662, 0.0));
    positionAlloc4->Add (Vector(475.6439285986027,192.09963982054964, 0.0));
    positionAlloc4->Add (Vector(540.4483921590174,661.4553571674722, 0.0));
    positionAlloc4->Add (Vector(845.8975528548173,613.287432894565, 0.0));
    positionAlloc4->Add (Vector(383.10133914073856,618.6408686387331, 0.0));
    positionAlloc4->Add (Vector(523.0704505859347,377.0336271807629, 0.0));
    positionAlloc4->Add (Vector(766.0553612930207,605.6141583330359, 0.0));
    positionAlloc4->Add (Vector(313.5984573373031,528.0083402231573, 0.0));
    positionAlloc4->Add (Vector(197.32879451455042,710.2495554756531, 0.0));
    positionAlloc4->Add (Vector(198.59668992226534,513.3418536357892, 0.0));
    positionAlloc4->Add (Vector(308.65948155889157,398.49741408235616, 0.0));
    positionAlloc4->Add (Vector(284.30712730312007,376.3600934808645, 0.0));
    positionAlloc4->Add (Vector(209.96077471750138,775.9336122648828, 0.0));
    positionAlloc4->Add (Vector(341.91246659186413,205.59355829229062, 0.0));
    positionAlloc4->Add (Vector(657.0371719901188,554.8708419044864, 0.0));
    positionAlloc4->Add (Vector(239.87600261788765,222.87675456891645, 0.0));
    positionAlloc4->Add (Vector(634.7053059270588,843.8791955915318, 0.0));
    positionAlloc4->Add (Vector(449.73973044793036,652.3184359607234, 0.0));
    positionAlloc4->Add (Vector(636.6011878333156,433.0231376730045, 0.0));
    positionAlloc4->Add (Vector(841.4498641504211,245.5078267897132, 0.0));
    positionAlloc4->Add (Vector(566.1334346303211,497.7368012014186, 0.0));
    positionAlloc4->Add (Vector(312.3629672811828,423.63419713386025, 0.0));
    positionAlloc4->Add (Vector(574.9658252621466,532.688188899849, 0.0));
    positionAlloc4->Add (Vector(166.54701023243547,599.3800503102862, 0.0));
    positionAlloc4->Add (Vector(301.71786006354296,217.00018886411323, 0.0));
    positionAlloc4->Add (Vector(665.8116214609274,753.95795428026, 0.0));
    positionAlloc4->Add (Vector(560.6259242228318,267.6099454214044, 0.0));
    positionAlloc4->Add (Vector(586.7015272736012,466.2373751160604, 0.0));
    positionAlloc4->Add (Vector(617.2849295040294,216.87471968426775, 0.0));
    positionAlloc4->Add (Vector(725.8300229595749,446.616777554134, 0.0));
    positionAlloc4->Add (Vector(696.3678606707033,342.3642248836953, 0.0));
    positionAlloc4->Add (Vector(397.3949655119027,315.8093912821374, 0.0));
    positionAlloc4->Add (Vector(737.9029120067042,780.0161863786078, 0.0));
    positionAlloc4->Add (Vector(577.6866921956941,437.7199288646933, 0.0));
    positionAlloc4->Add (Vector(309.65584036425275,298.43138793575866, 0.0));
    positionAlloc4->Add (Vector(723.4322015987532,768.9445243438854, 0.0));
    positionAlloc4->Add (Vector(815.9823395446599,571.1282741024318, 0.0));
    positionAlloc4->Add (Vector(180.3887729983734,593.2059629158157, 0.0));
    positionAlloc4->Add (Vector(714.1947848426107,535.1585795093832, 0.0));
    positionAlloc4->Add (Vector(824.0488126663058,488.4819527039473, 0.0));
    positionAlloc4->Add (Vector(547.1185537402325,329.5939485909546, 0.0));
    positionAlloc4->Add (Vector(646.3323832057029,224.81384616632067, 0.0));
    positionAlloc4->Add (Vector(277.00684338693884,517.6143685669422, 0.0));
    positionAlloc4->Add (Vector(276.8045446742762,626.782497775488, 0.0));
    positionAlloc4->Add (Vector(439.48437106774605,235.32026533586327, 0.0));
    positionAlloc4->Add (Vector(651.3054719562355,340.83680051347363, 0.0));
    positionAlloc4->Add (Vector(367.9153894092026,577.2336483176429, 0.0));
    positionAlloc4->Add (Vector(712.1181284828644,527.4128936726781, 0.0));
    positionAlloc4->Add (Vector(349.41103137334596,365.10297544840057, 0.0));
    positionAlloc4->Add (Vector(490.110231430428,329.74359740435557, 0.0));
    positionAlloc4->Add (Vector(396.1358810677441,220.99457991445155, 0.0));
    positionAlloc4->Add (Vector(192.1871732026926,804.10521876386, 0.0));
    positionAlloc4->Add (Vector(320.4376076034095,798.9241813574147, 0.0));
    positionAlloc4->Add (Vector(603.2322999082221,778.6213842649909, 0.0));
    positionAlloc4->Add (Vector(287.04458997848224,155.2940124846995, 0.0));
    positionAlloc4->Add (Vector(818.8665871684543,162.76280787345314, 0.0));
    positionAlloc4->Add (Vector(719.0900171012654,652.6861533585461, 0.0));
    positionAlloc4->Add (Vector(575.5335649554113,685.4182830794898, 0.0));
    positionAlloc4->Add (Vector(556.1206584591325,523.521782708815, 0.0));
    positionAlloc4->Add (Vector(416.13693315009994,429.17594662783034, 0.0));
    positionAlloc4->Add (Vector(167.94944606450497,388.87347933830944, 0.0));
    positionAlloc4->Add (Vector(513.3572495953526,485.4273143408117, 0.0));
    positionAlloc4->Add (Vector(415.8500461254609,754.3335547002777, 0.0));
    positionAlloc4->Add (Vector(583.6432422067302,474.44583066040065, 0.0));
    positionAlloc4->Add (Vector(572.4140153669646,617.7489589885114, 0.0));
    positionAlloc4->Add (Vector(309.08873908420605,201.23714663791054, 0.0));
    positionAlloc4->Add (Vector(316.2808916120292,411.2762541405381, 0.0));
    positionAlloc4->Add (Vector(421.99067088924204,464.08553583577145, 0.0));
    positionAlloc4->Add (Vector(427.32088412647875,636.8270446829035, 0.0));
    positionAlloc4->Add (Vector(377.2100809301892,406.5230744099785, 0.0));
    positionAlloc4->Add (Vector(531.1423685725947,154.0310623486851, 0.0));
    positionAlloc4->Add (Vector(722.4839513225152,234.14633875942593, 0.0));
    positionAlloc4->Add (Vector(708.4695542010442,454.9083203690531, 0.0));
    positionAlloc4->Add (Vector(715.492159358881,683.599594343631, 0.0));
    positionAlloc4->Add (Vector(378.9232130942671,617.3153698945093, 0.0));
    positionAlloc4->Add (Vector(764.9608929109435,432.8982251557231, 0.0));
    positionAlloc4->Add (Vector(546.9113826906674,666.9144464759286, 0.0));
    positionAlloc4->Add (Vector(759.5274152360814,697.6698388407914, 0.0));
    positionAlloc4->Add (Vector(834.0219289549221,434.83557250600944, 0.0));
    positionAlloc4->Add (Vector(849.5896080394666,183.74842720238922, 0.0));
    positionAlloc4->Add (Vector(224.6151212155935,610.4917132221252, 0.0));
    positionAlloc4->Add (Vector(425.7050839579109,605.8630839290445, 0.0));
    positionAlloc4->Add (Vector(300.97260667840203,611.21982916286, 0.0));
    positionAlloc4->Add (Vector(632.6687257220929,623.5593700440726, 0.0));
    positionAlloc4->Add (Vector(171.2709417877858,801.6640868548844, 0.0));
    positionAlloc4->Add (Vector(758.2334891351028,549.4268226471808, 0.0));
    positionAlloc4->Add (Vector(498.0223348058991,582.4930681408497, 0.0));
    positionAlloc4->Add (Vector(811.2299536318162,628.2736198027521, 0.0));
    positionAlloc4->Add (Vector(634.8381665583152,736.6807027536771, 0.0));
    positionAlloc4->Add (Vector(757.6995780642537,274.0048526131701, 0.0));
    positionAlloc4->Add (Vector(620.8214827575885,460.6251535131998, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_2.xml");
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
	  flowmon->SerializeToXmlFile ("switch_ECC_flowmon/switch_ECC_2.flowmon", false, false);
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