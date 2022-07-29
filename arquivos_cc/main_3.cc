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
	  uint16_t numberOfRrhs = 68;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 447;
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
    positionAlloc->Add (Vector (772.2222222222222,150.0, 0.0));
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
    positionAlloc->Add (Vector (538.8888888888889,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,850.0, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,150.0, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,150.0, 0.0));
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
    positionAlloc->Add (Vector (227.77777777777777,150.0, 0.0));
    positionAlloc->Add (Vector (150.0,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (150.0,383.33333333333337, 0.0));
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
    positionAlloc4->Add (Vector(799.727468679766,800.397664010824, 0.0));
    positionAlloc4->Add (Vector(310.2128237384493,567.7963138214316, 0.0));
    positionAlloc4->Add (Vector(645.1779000230495,422.46133018425195, 0.0));
    positionAlloc4->Add (Vector(679.7111169479977,459.6056638157947, 0.0));
    positionAlloc4->Add (Vector(556.6652659217254,843.475409946036, 0.0));
    positionAlloc4->Add (Vector(755.1032728078549,560.855554591235, 0.0));
    positionAlloc4->Add (Vector(544.3767454988481,587.2298336801721, 0.0));
    positionAlloc4->Add (Vector(735.9837394814565,583.6683400254124, 0.0));
    positionAlloc4->Add (Vector(683.6118371506719,177.25661168136594, 0.0));
    positionAlloc4->Add (Vector(685.0473705054992,586.6201594048064, 0.0));
    positionAlloc4->Add (Vector(312.41272621616747,160.06701225045953, 0.0));
    positionAlloc4->Add (Vector(368.9445924472478,608.0394038449037, 0.0));
    positionAlloc4->Add (Vector(698.6167757526655,546.3264838575496, 0.0));
    positionAlloc4->Add (Vector(165.6981224134386,402.2987789990403, 0.0));
    positionAlloc4->Add (Vector(437.8340625962102,241.05646888026286, 0.0));
    positionAlloc4->Add (Vector(319.0475396798075,178.2455788017898, 0.0));
    positionAlloc4->Add (Vector(750.6162897050605,848.3398081815096, 0.0));
    positionAlloc4->Add (Vector(673.7287384341925,714.6655371187716, 0.0));
    positionAlloc4->Add (Vector(849.2248284233034,378.64747459813765, 0.0));
    positionAlloc4->Add (Vector(197.07913977715748,216.44255577516878, 0.0));
    positionAlloc4->Add (Vector(324.3061152763629,553.6916792004295, 0.0));
    positionAlloc4->Add (Vector(727.7909648867636,741.4099434286603, 0.0));
    positionAlloc4->Add (Vector(780.6726269833275,847.3103714491451, 0.0));
    positionAlloc4->Add (Vector(771.6827322538251,377.70343285322343, 0.0));
    positionAlloc4->Add (Vector(376.1191420278657,778.543430870366, 0.0));
    positionAlloc4->Add (Vector(177.28552446279127,616.7931739139606, 0.0));
    positionAlloc4->Add (Vector(320.37767825297135,540.1360187011056, 0.0));
    positionAlloc4->Add (Vector(173.57348271212481,765.4300408208734, 0.0));
    positionAlloc4->Add (Vector(171.80436495537376,331.7035144925344, 0.0));
    positionAlloc4->Add (Vector(407.84304965604593,452.6423584823431, 0.0));
    positionAlloc4->Add (Vector(211.8694628891631,639.2641799411699, 0.0));
    positionAlloc4->Add (Vector(682.5959316515472,281.40568398457646, 0.0));
    positionAlloc4->Add (Vector(720.4578099783774,694.6176702012572, 0.0));
    positionAlloc4->Add (Vector(335.2445188477501,613.378261355708, 0.0));
    positionAlloc4->Add (Vector(303.1009290985491,566.7122861492423, 0.0));
    positionAlloc4->Add (Vector(283.64860955015047,636.2486712256094, 0.0));
    positionAlloc4->Add (Vector(377.1582601227395,338.65830360459086, 0.0));
    positionAlloc4->Add (Vector(807.3479356152042,595.461997073362, 0.0));
    positionAlloc4->Add (Vector(634.6512457227407,334.84338928560317, 0.0));
    positionAlloc4->Add (Vector(707.369015378958,340.3622275929141, 0.0));
    positionAlloc4->Add (Vector(725.2816626097515,611.6575380485392, 0.0));
    positionAlloc4->Add (Vector(637.3093015965476,417.9591351054715, 0.0));
    positionAlloc4->Add (Vector(281.5442665707261,203.88016856661184, 0.0));
    positionAlloc4->Add (Vector(785.4869597184738,504.9417527171028, 0.0));
    positionAlloc4->Add (Vector(300.4388459254771,447.39559000766087, 0.0));
    positionAlloc4->Add (Vector(369.6787657570909,319.0504395949834, 0.0));
    positionAlloc4->Add (Vector(339.65988882145575,291.5779263671744, 0.0));
    positionAlloc4->Add (Vector(481.7448540812719,651.4779783706692, 0.0));
    positionAlloc4->Add (Vector(592.6739270298713,259.53370120252043, 0.0));
    positionAlloc4->Add (Vector(772.389546758007,555.1647281583583, 0.0));
    positionAlloc4->Add (Vector(568.0087141932113,607.8929159195116, 0.0));
    positionAlloc4->Add (Vector(785.759942090226,681.1007248147445, 0.0));
    positionAlloc4->Add (Vector(773.28666205353,674.9844589164495, 0.0));
    positionAlloc4->Add (Vector(268.9167042355147,286.4826399302025, 0.0));
    positionAlloc4->Add (Vector(584.3316495363795,669.577231039361, 0.0));
    positionAlloc4->Add (Vector(248.2271627354605,619.8102313657821, 0.0));
    positionAlloc4->Add (Vector(712.101450370486,512.6056464832374, 0.0));
    positionAlloc4->Add (Vector(697.3719840105714,676.2228946770883, 0.0));
    positionAlloc4->Add (Vector(686.5605346728092,558.5995114119548, 0.0));
    positionAlloc4->Add (Vector(533.8934140954939,196.52432804339048, 0.0));
    positionAlloc4->Add (Vector(713.1462497022757,690.9667013032362, 0.0));
    positionAlloc4->Add (Vector(488.792557725823,153.71943296784215, 0.0));
    positionAlloc4->Add (Vector(302.3671797063752,613.1790018798995, 0.0));
    positionAlloc4->Add (Vector(646.135958660408,155.85457731379998, 0.0));
    positionAlloc4->Add (Vector(655.9164265671633,609.7106393051256, 0.0));
    positionAlloc4->Add (Vector(408.7019395987412,474.96696381745744, 0.0));
    positionAlloc4->Add (Vector(410.77329163168554,198.0995744067727, 0.0));
    positionAlloc4->Add (Vector(304.0779281333272,395.36437400031326, 0.0));
    positionAlloc4->Add (Vector(541.3414994626215,374.81350684516747, 0.0));
    positionAlloc4->Add (Vector(453.4305629289441,848.0276477320652, 0.0));
    positionAlloc4->Add (Vector(412.82824992655026,712.6701816354127, 0.0));
    positionAlloc4->Add (Vector(603.8226052164669,201.87379580240696, 0.0));
    positionAlloc4->Add (Vector(827.6460684355312,196.595309539956, 0.0));
    positionAlloc4->Add (Vector(422.5834572852127,529.2326749872943, 0.0));
    positionAlloc4->Add (Vector(196.88152841769005,764.145317192987, 0.0));
    positionAlloc4->Add (Vector(781.2418199823601,292.4309197483171, 0.0));
    positionAlloc4->Add (Vector(421.4782629418519,712.4763616238223, 0.0));
    positionAlloc4->Add (Vector(476.41682929811907,202.98980911685123, 0.0));
    positionAlloc4->Add (Vector(748.481813693032,767.791913740643, 0.0));
    positionAlloc4->Add (Vector(465.90587912047846,278.4095374824042, 0.0));
    positionAlloc4->Add (Vector(383.539877085806,704.090741813959, 0.0));
    positionAlloc4->Add (Vector(740.3283174515182,489.5662619856781, 0.0));
    positionAlloc4->Add (Vector(735.2823922450741,724.971854030486, 0.0));
    positionAlloc4->Add (Vector(656.4105636484384,524.959619543305, 0.0));
    positionAlloc4->Add (Vector(327.2906684659006,156.98514366091456, 0.0));
    positionAlloc4->Add (Vector(589.898519630211,632.042140515513, 0.0));
    positionAlloc4->Add (Vector(481.9785146838675,542.474806457265, 0.0));
    positionAlloc4->Add (Vector(704.930679500969,198.3321344202198, 0.0));
    positionAlloc4->Add (Vector(443.4901628162154,162.6868287320108, 0.0));
    positionAlloc4->Add (Vector(293.0214683792792,505.4635447711454, 0.0));
    positionAlloc4->Add (Vector(424.3015612316182,776.9723443331451, 0.0));
    positionAlloc4->Add (Vector(634.4515529170583,294.37647968530683, 0.0));
    positionAlloc4->Add (Vector(386.04460289602645,249.9893753700323, 0.0));
    positionAlloc4->Add (Vector(573.946522526388,232.0427339761634, 0.0));
    positionAlloc4->Add (Vector(522.7528464911873,230.77624952524218, 0.0));
    positionAlloc4->Add (Vector(759.368876388569,591.9932448528634, 0.0));
    positionAlloc4->Add (Vector(500.07892645363887,438.37066652387136, 0.0));
    positionAlloc4->Add (Vector(641.5022673272806,270.1857353938147, 0.0));
    positionAlloc4->Add (Vector(528.6335026866566,637.5008649252751, 0.0));
    positionAlloc4->Add (Vector(441.04257246632534,450.261033401101, 0.0));
    positionAlloc4->Add (Vector(581.4103401480069,539.8149102204671, 0.0));
    positionAlloc4->Add (Vector(303.68774626086133,821.9039618994187, 0.0));
    positionAlloc4->Add (Vector(423.72300838917903,313.9423596484339, 0.0));
    positionAlloc4->Add (Vector(321.74661991634775,190.41251145758204, 0.0));
    positionAlloc4->Add (Vector(513.4571995461436,575.4268781669568, 0.0));
    positionAlloc4->Add (Vector(223.7418391369332,305.6529133237192, 0.0));
    positionAlloc4->Add (Vector(319.07578919117725,397.98992905205097, 0.0));
    positionAlloc4->Add (Vector(266.02372387565447,773.5988744882027, 0.0));
    positionAlloc4->Add (Vector(208.48803625748283,163.39027650610092, 0.0));
    positionAlloc4->Add (Vector(510.45334493928164,262.9539607387686, 0.0));
    positionAlloc4->Add (Vector(653.1177324974713,662.6807343408653, 0.0));
    positionAlloc4->Add (Vector(265.6991431000702,151.3944946533492, 0.0));
    positionAlloc4->Add (Vector(310.71871252777805,232.285910105154, 0.0));
    positionAlloc4->Add (Vector(275.4772790174965,635.7647133571097, 0.0));
    positionAlloc4->Add (Vector(262.2557842838202,621.5517922324354, 0.0));
    positionAlloc4->Add (Vector(457.44742087480216,577.3827285107972, 0.0));
    positionAlloc4->Add (Vector(751.1349955446306,673.8490975660178, 0.0));
    positionAlloc4->Add (Vector(623.8585480977861,736.5358623962625, 0.0));
    positionAlloc4->Add (Vector(285.7041475558249,751.1717398177083, 0.0));
    positionAlloc4->Add (Vector(537.3598328248403,502.192730786078, 0.0));
    positionAlloc4->Add (Vector(221.79757380851288,424.91152028999136, 0.0));
    positionAlloc4->Add (Vector(808.7845687735714,763.4236031996181, 0.0));
    positionAlloc4->Add (Vector(398.09746415513837,503.9439073953712, 0.0));
    positionAlloc4->Add (Vector(816.9705119017542,378.7197794210286, 0.0));
    positionAlloc4->Add (Vector(365.45708217652196,789.6519107112815, 0.0));
    positionAlloc4->Add (Vector(729.988918534257,702.50499382378, 0.0));
    positionAlloc4->Add (Vector(399.82018261977714,397.6524111155052, 0.0));
    positionAlloc4->Add (Vector(510.5039617968062,524.8876704157708, 0.0));
    positionAlloc4->Add (Vector(366.86481272572826,324.96986247833485, 0.0));
    positionAlloc4->Add (Vector(227.30230438777187,352.4312124071649, 0.0));
    positionAlloc4->Add (Vector(719.8879799138133,629.5940169121388, 0.0));
    positionAlloc4->Add (Vector(614.9535694922778,767.1834282904556, 0.0));
    positionAlloc4->Add (Vector(199.34436195020126,279.9526934308701, 0.0));
    positionAlloc4->Add (Vector(261.6445837789508,262.3238299898585, 0.0));
    positionAlloc4->Add (Vector(608.6216001670366,332.16561858966054, 0.0));
    positionAlloc4->Add (Vector(685.1085096528531,436.76973736319485, 0.0));
    positionAlloc4->Add (Vector(798.9774402008991,223.77525267546366, 0.0));
    positionAlloc4->Add (Vector(189.3232541432073,493.7670105682156, 0.0));
    positionAlloc4->Add (Vector(360.7322544840191,501.84186984109135, 0.0));
    positionAlloc4->Add (Vector(460.74361815446446,165.33933014250627, 0.0));
    positionAlloc4->Add (Vector(727.3202661429053,354.66817728853516, 0.0));
    positionAlloc4->Add (Vector(805.3964350282487,194.80444262603464, 0.0));
    positionAlloc4->Add (Vector(459.7818603703219,604.5256272038172, 0.0));
    positionAlloc4->Add (Vector(643.8225811984404,449.32093359233613, 0.0));
    positionAlloc4->Add (Vector(591.1730536193106,323.8685261202453, 0.0));
    positionAlloc4->Add (Vector(701.170918726633,468.0417961473586, 0.0));
    positionAlloc4->Add (Vector(776.7422624072185,616.524133268785, 0.0));
    positionAlloc4->Add (Vector(338.1974600675355,407.91424394160526, 0.0));
    positionAlloc4->Add (Vector(559.1142368301983,526.8323602789994, 0.0));
    positionAlloc4->Add (Vector(409.77905690108946,794.5366259243501, 0.0));
    positionAlloc4->Add (Vector(515.0143169168161,197.92110353350463, 0.0));
    positionAlloc4->Add (Vector(670.0170455675573,666.5421753478178, 0.0));
    positionAlloc4->Add (Vector(290.19741250733813,581.7485254967788, 0.0));
    positionAlloc4->Add (Vector(802.8973556929329,530.9777762795884, 0.0));
    positionAlloc4->Add (Vector(713.563222946964,726.0886635292372, 0.0));
    positionAlloc4->Add (Vector(816.0434971237511,736.9442836901173, 0.0));
    positionAlloc4->Add (Vector(621.0101680186558,206.8044249753674, 0.0));
    positionAlloc4->Add (Vector(662.9134323503156,574.7122073022056, 0.0));
    positionAlloc4->Add (Vector(193.76840701999203,471.25836161777426, 0.0));
    positionAlloc4->Add (Vector(339.5644296258808,323.2918391959205, 0.0));
    positionAlloc4->Add (Vector(323.86338225886016,300.0845380952152, 0.0));
    positionAlloc4->Add (Vector(667.1062055085964,159.62188780794168, 0.0));
    positionAlloc4->Add (Vector(289.4892355267606,764.2290326581374, 0.0));
    positionAlloc4->Add (Vector(649.8369063646887,213.9740404322435, 0.0));
    positionAlloc4->Add (Vector(596.3817728271549,319.41545598661406, 0.0));
    positionAlloc4->Add (Vector(562.617792639288,579.3041794211654, 0.0));
    positionAlloc4->Add (Vector(656.4134724950006,156.3379131062597, 0.0));
    positionAlloc4->Add (Vector(331.09814950880866,454.08232330462687, 0.0));
    positionAlloc4->Add (Vector(790.1840646939912,453.81468089613116, 0.0));
    positionAlloc4->Add (Vector(214.83783813595704,673.8690447733619, 0.0));
    positionAlloc4->Add (Vector(155.82395955903442,453.50795484283003, 0.0));
    positionAlloc4->Add (Vector(833.9048681072604,221.32517061105574, 0.0));
    positionAlloc4->Add (Vector(281.97842402447577,620.7223486175202, 0.0));
    positionAlloc4->Add (Vector(429.3247223497043,737.8611246650987, 0.0));
    positionAlloc4->Add (Vector(465.1779723000232,820.3067105586721, 0.0));
    positionAlloc4->Add (Vector(558.0482397629842,327.17107742572614, 0.0));
    positionAlloc4->Add (Vector(435.7061836066887,160.77642418391594, 0.0));
    positionAlloc4->Add (Vector(640.7977160922316,626.0783198128809, 0.0));
    positionAlloc4->Add (Vector(291.8428438947402,561.3101097765814, 0.0));
    positionAlloc4->Add (Vector(665.5746598343547,768.0956773808412, 0.0));
    positionAlloc4->Add (Vector(258.3800288184039,811.3153811481558, 0.0));
    positionAlloc4->Add (Vector(812.7136102463769,224.13406308263035, 0.0));
    positionAlloc4->Add (Vector(568.3677978918922,756.266943528184, 0.0));
    positionAlloc4->Add (Vector(369.0351759495537,339.76737763484607, 0.0));
    positionAlloc4->Add (Vector(644.6409089150967,577.0051919556399, 0.0));
    positionAlloc4->Add (Vector(325.6251319837718,623.2184016367833, 0.0));
    positionAlloc4->Add (Vector(160.57747895013253,825.5107586181304, 0.0));
    positionAlloc4->Add (Vector(603.9533740830817,337.18141950027893, 0.0));
    positionAlloc4->Add (Vector(569.331211005267,586.5541448031863, 0.0));
    positionAlloc4->Add (Vector(358.1035532151335,214.4468902577328, 0.0));
    positionAlloc4->Add (Vector(202.9085296978469,815.9962545918962, 0.0));
    positionAlloc4->Add (Vector(572.5460403915625,203.8856269454764, 0.0));
    positionAlloc4->Add (Vector(609.72924443219,827.9761321899246, 0.0));
    positionAlloc4->Add (Vector(337.9736292473251,351.22316542807505, 0.0));
    positionAlloc4->Add (Vector(293.73305709475983,488.127788291223, 0.0));
    positionAlloc4->Add (Vector(428.3286649870094,334.9933842110475, 0.0));
    positionAlloc4->Add (Vector(445.9100458257526,392.56691756690276, 0.0));
    positionAlloc4->Add (Vector(666.5194893842244,643.1880996192117, 0.0));
    positionAlloc4->Add (Vector(229.31245330922494,432.52070165311386, 0.0));
    positionAlloc4->Add (Vector(275.55527740854734,829.4735817868209, 0.0));
    positionAlloc4->Add (Vector(197.25178267867534,513.0906960664165, 0.0));
    positionAlloc4->Add (Vector(452.4132493161542,836.2581186595296, 0.0));
    positionAlloc4->Add (Vector(344.4969432627986,343.4233759260975, 0.0));
    positionAlloc4->Add (Vector(675.6578588543227,220.9610282872114, 0.0));
    positionAlloc4->Add (Vector(289.2944972716058,442.83370487159607, 0.0));
    positionAlloc4->Add (Vector(620.0354040670449,592.597507546348, 0.0));
    positionAlloc4->Add (Vector(811.1140078860124,599.2140781089907, 0.0));
    positionAlloc4->Add (Vector(189.60995930270093,411.7447329089695, 0.0));
    positionAlloc4->Add (Vector(629.810718255331,382.7193100040984, 0.0));
    positionAlloc4->Add (Vector(350.0308272148942,452.93681571948383, 0.0));
    positionAlloc4->Add (Vector(197.84724021697104,698.6318897388751, 0.0));
    positionAlloc4->Add (Vector(662.8893354131962,433.16916990170876, 0.0));
    positionAlloc4->Add (Vector(584.904714334727,383.83497596813845, 0.0));
    positionAlloc4->Add (Vector(364.74361966549884,397.07095241664956, 0.0));
    positionAlloc4->Add (Vector(534.2010655541492,351.34291033057474, 0.0));
    positionAlloc4->Add (Vector(621.8139433411549,186.2850593838103, 0.0));
    positionAlloc4->Add (Vector(784.2609423368824,158.50974003417076, 0.0));
    positionAlloc4->Add (Vector(550.222188200624,732.9812637517898, 0.0));
    positionAlloc4->Add (Vector(325.3275291547493,587.4118697248992, 0.0));
    positionAlloc4->Add (Vector(620.8920959462287,707.6769780339606, 0.0));
    positionAlloc4->Add (Vector(353.0318723737058,759.989823933598, 0.0));
    positionAlloc4->Add (Vector(260.0912457258007,366.85150144636947, 0.0));
    positionAlloc4->Add (Vector(242.2005200053655,755.7070312362387, 0.0));
    positionAlloc4->Add (Vector(536.8949530029818,427.58757739297425, 0.0));
    positionAlloc4->Add (Vector(275.478928335453,710.002437234577, 0.0));
    positionAlloc4->Add (Vector(652.9177631257851,753.8483298044252, 0.0));
    positionAlloc4->Add (Vector(186.16810516057276,503.35691881316967, 0.0));
    positionAlloc4->Add (Vector(532.5012212544932,826.9246926793023, 0.0));
    positionAlloc4->Add (Vector(643.4097070303671,721.9896994109096, 0.0));
    positionAlloc4->Add (Vector(456.36292970374245,314.4813834971669, 0.0));
    positionAlloc4->Add (Vector(642.5898457403357,829.17997680824, 0.0));
    positionAlloc4->Add (Vector(737.2981294026075,157.99258841785155, 0.0));
    positionAlloc4->Add (Vector(419.0485269668944,167.63538977146055, 0.0));
    positionAlloc4->Add (Vector(700.3439565827406,664.110752452914, 0.0));
    positionAlloc4->Add (Vector(416.36239690892614,264.1247358521137, 0.0));
    positionAlloc4->Add (Vector(812.9634669759633,497.38112701765306, 0.0));
    positionAlloc4->Add (Vector(658.9397331307731,543.4509560817997, 0.0));
    positionAlloc4->Add (Vector(749.5760566235488,640.8189031131818, 0.0));
    positionAlloc4->Add (Vector(384.92731798888985,834.9678278723984, 0.0));
    positionAlloc4->Add (Vector(286.6684220695444,391.7393665714215, 0.0));
    positionAlloc4->Add (Vector(341.8501125395337,714.9496327066921, 0.0));
    positionAlloc4->Add (Vector(660.2867431590715,753.1836326511417, 0.0));
    positionAlloc4->Add (Vector(794.6961647676031,636.2167990772998, 0.0));
    positionAlloc4->Add (Vector(650.0286450144984,473.20130066918335, 0.0));
    positionAlloc4->Add (Vector(729.2544239112549,638.0465673367457, 0.0));
    positionAlloc4->Add (Vector(408.49653915087873,803.0474037987705, 0.0));
    positionAlloc4->Add (Vector(509.6981249447374,506.22338891373914, 0.0));
    positionAlloc4->Add (Vector(681.3036977653517,221.76117484653213, 0.0));
    positionAlloc4->Add (Vector(357.5919385441515,650.0969148498696, 0.0));
    positionAlloc4->Add (Vector(335.72829628882687,571.0877155098768, 0.0));
    positionAlloc4->Add (Vector(816.5552499883163,179.6956087448242, 0.0));
    positionAlloc4->Add (Vector(696.44220219789,558.6409463598782, 0.0));
    positionAlloc4->Add (Vector(646.5245799308267,267.12491280074227, 0.0));
    positionAlloc4->Add (Vector(204.91907142198676,531.1478713727056, 0.0));
    positionAlloc4->Add (Vector(563.4464837962464,612.7099779731242, 0.0));
    positionAlloc4->Add (Vector(677.9737817774566,425.23664071954556, 0.0));
    positionAlloc4->Add (Vector(714.6854637863174,839.4008819337331, 0.0));
    positionAlloc4->Add (Vector(409.65870571043564,781.3581377244051, 0.0));
    positionAlloc4->Add (Vector(721.7456265539453,277.12977742962187, 0.0));
    positionAlloc4->Add (Vector(377.70659381612177,648.834933168959, 0.0));
    positionAlloc4->Add (Vector(445.8404379095264,362.0516339483005, 0.0));
    positionAlloc4->Add (Vector(669.3274288711006,743.8977653633673, 0.0));
    positionAlloc4->Add (Vector(307.00543829953153,614.1330342623958, 0.0));
    positionAlloc4->Add (Vector(660.5413558615974,826.7918498594997, 0.0));
    positionAlloc4->Add (Vector(311.29166348214494,151.031675460418, 0.0));
    positionAlloc4->Add (Vector(362.9391037453966,362.75975857851915, 0.0));
    positionAlloc4->Add (Vector(831.154714080633,308.2877318747703, 0.0));
    positionAlloc4->Add (Vector(515.7571267599119,188.2440940006806, 0.0));
    positionAlloc4->Add (Vector(150.0081443287563,441.59607993760454, 0.0));
    positionAlloc4->Add (Vector(239.50676085569228,685.8827253611026, 0.0));
    positionAlloc4->Add (Vector(447.65972980638577,176.29976327923515, 0.0));
    positionAlloc4->Add (Vector(527.3661869152061,347.7306984590026, 0.0));
    positionAlloc4->Add (Vector(256.1165523600696,763.1181584619399, 0.0));
    positionAlloc4->Add (Vector(689.7859710963206,608.2554808061682, 0.0));
    positionAlloc4->Add (Vector(382.53372318092323,635.5326402316566, 0.0));
    positionAlloc4->Add (Vector(717.6150067055429,735.0781495373612, 0.0));
    positionAlloc4->Add (Vector(504.84430657328426,436.58845027191285, 0.0));
    positionAlloc4->Add (Vector(198.3053148289714,808.5491643521756, 0.0));
    positionAlloc4->Add (Vector(294.95306644759864,199.81079962140296, 0.0));
    positionAlloc4->Add (Vector(382.09528934703565,748.5211411339546, 0.0));
    positionAlloc4->Add (Vector(563.7912122809615,280.29512842734357, 0.0));
    positionAlloc4->Add (Vector(498.2778965022715,293.27607634009877, 0.0));
    positionAlloc4->Add (Vector(696.2558521048172,483.43501331076914, 0.0));
    positionAlloc4->Add (Vector(774.7907592405321,443.6135957701195, 0.0));
    positionAlloc4->Add (Vector(775.8660117732755,812.5195592052838, 0.0));
    positionAlloc4->Add (Vector(347.74724618099657,397.13527412021165, 0.0));
    positionAlloc4->Add (Vector(511.68886965838743,681.1433776301276, 0.0));
    positionAlloc4->Add (Vector(455.62993283813626,556.0621842686182, 0.0));
    positionAlloc4->Add (Vector(815.321419402707,557.1356050924355, 0.0));
    positionAlloc4->Add (Vector(756.7392481542001,347.73512078391866, 0.0));
    positionAlloc4->Add (Vector(175.21317280399848,272.65524047407916, 0.0));
    positionAlloc4->Add (Vector(304.59694807410625,177.2509743279672, 0.0));
    positionAlloc4->Add (Vector(221.26257806467584,504.17053541445216, 0.0));
    positionAlloc4->Add (Vector(284.11506201437805,792.7673606728059, 0.0));
    positionAlloc4->Add (Vector(609.6417515955133,318.27923902545484, 0.0));
    positionAlloc4->Add (Vector(544.8518201834625,401.0049230674112, 0.0));
    positionAlloc4->Add (Vector(701.2843601011455,325.7009682836102, 0.0));
    positionAlloc4->Add (Vector(553.9858102268245,399.98481151035196, 0.0));
    positionAlloc4->Add (Vector(792.8985025017861,815.6500161517839, 0.0));
    positionAlloc4->Add (Vector(424.720730360636,677.9854813429159, 0.0));
    positionAlloc4->Add (Vector(682.7055464467744,410.20684648164024, 0.0));
    positionAlloc4->Add (Vector(845.2785107960748,181.4379092969311, 0.0));
    positionAlloc4->Add (Vector(428.48563785150986,793.6717172592348, 0.0));
    positionAlloc4->Add (Vector(232.9518365187061,352.21945723641596, 0.0));
    positionAlloc4->Add (Vector(486.6231026221063,613.7038081907476, 0.0));
    positionAlloc4->Add (Vector(450.09141469050354,170.41883455168514, 0.0));
    positionAlloc4->Add (Vector(660.2030095889628,216.77949411924743, 0.0));
    positionAlloc4->Add (Vector(375.1689927616841,743.6294627210387, 0.0));
    positionAlloc4->Add (Vector(594.054582210181,305.4531094853244, 0.0));
    positionAlloc4->Add (Vector(798.3358479022473,310.07374572252616, 0.0));
    positionAlloc4->Add (Vector(156.32692835550452,399.92054077900855, 0.0));
    positionAlloc4->Add (Vector(499.22696517515936,158.4721817432752, 0.0));
    positionAlloc4->Add (Vector(545.5597187169294,690.1388376850254, 0.0));
    positionAlloc4->Add (Vector(616.6203353326334,263.8898206659961, 0.0));
    positionAlloc4->Add (Vector(323.1382210733583,598.4306284842212, 0.0));
    positionAlloc4->Add (Vector(483.830074527579,652.8318227604027, 0.0));
    positionAlloc4->Add (Vector(680.1171449941072,263.1700053658733, 0.0));
    positionAlloc4->Add (Vector(515.0217471050304,834.1155355940526, 0.0));
    positionAlloc4->Add (Vector(810.5911149741546,822.094923083813, 0.0));
    positionAlloc4->Add (Vector(704.8165814018683,785.5291556457336, 0.0));
    positionAlloc4->Add (Vector(836.8244857336061,283.0753548486093, 0.0));
    positionAlloc4->Add (Vector(201.1570934189105,585.6402852320723, 0.0));
    positionAlloc4->Add (Vector(455.8971199659346,565.4926547651502, 0.0));
    positionAlloc4->Add (Vector(548.6778456969726,331.67924822326324, 0.0));
    positionAlloc4->Add (Vector(648.2059640222342,836.8006122542753, 0.0));
    positionAlloc4->Add (Vector(816.0008672532825,846.3735057761658, 0.0));
    positionAlloc4->Add (Vector(822.4388828592398,287.8479931173999, 0.0));
    positionAlloc4->Add (Vector(747.649445193267,319.38399122193175, 0.0));
    positionAlloc4->Add (Vector(836.8347704148811,705.2000117977176, 0.0));
    positionAlloc4->Add (Vector(184.9853200617751,740.5547648362766, 0.0));
    positionAlloc4->Add (Vector(461.87914711932365,296.4298257855495, 0.0));
    positionAlloc4->Add (Vector(180.72995985010357,266.02484587410913, 0.0));
    positionAlloc4->Add (Vector(532.6039994226064,452.9712015466285, 0.0));
    positionAlloc4->Add (Vector(714.8758436187956,471.76540928186284, 0.0));
    positionAlloc4->Add (Vector(203.36745258149514,342.2926705335244, 0.0));
    positionAlloc4->Add (Vector(500.4224998082953,625.7342329145754, 0.0));
    positionAlloc4->Add (Vector(520.7618067619339,319.32530587902244, 0.0));
    positionAlloc4->Add (Vector(243.99239563159287,820.5538847397393, 0.0));
    positionAlloc4->Add (Vector(591.6452531604143,463.37139778729284, 0.0));
    positionAlloc4->Add (Vector(253.11213099527924,155.48911354034178, 0.0));
    positionAlloc4->Add (Vector(645.4049451872693,193.54225831529192, 0.0));
    positionAlloc4->Add (Vector(478.9917877105556,773.0987961772103, 0.0));
    positionAlloc4->Add (Vector(792.2400901923932,353.3531539699943, 0.0));
    positionAlloc4->Add (Vector(331.933485469983,391.63454594594737, 0.0));
    positionAlloc4->Add (Vector(421.71599428889687,621.891206875244, 0.0));
    positionAlloc4->Add (Vector(717.7866389534296,595.16829501461, 0.0));
    positionAlloc4->Add (Vector(554.8088795942524,252.87389671174677, 0.0));
    positionAlloc4->Add (Vector(585.6895490961977,696.9252990231136, 0.0));
    positionAlloc4->Add (Vector(287.083158893989,248.56206422487327, 0.0));
    positionAlloc4->Add (Vector(405.97652378295993,307.7668876047361, 0.0));
    positionAlloc4->Add (Vector(823.9378451772443,202.92352843172554, 0.0));
    positionAlloc4->Add (Vector(843.9295310949544,531.137632960184, 0.0));
    positionAlloc4->Add (Vector(809.9070233658786,699.6326802038859, 0.0));
    positionAlloc4->Add (Vector(190.49111461301243,820.4528606917427, 0.0));
    positionAlloc4->Add (Vector(673.2110846176857,481.6668798785626, 0.0));
    positionAlloc4->Add (Vector(322.62254448461726,488.85394879714113, 0.0));
    positionAlloc4->Add (Vector(195.98640126805606,309.76475444558935, 0.0));
    positionAlloc4->Add (Vector(428.60370506815195,362.04101018323485, 0.0));
    positionAlloc4->Add (Vector(696.2632864996863,470.59703130250006, 0.0));
    positionAlloc4->Add (Vector(778.9924232723876,698.2388893090382, 0.0));
    positionAlloc4->Add (Vector(742.9425788887296,166.5164752518704, 0.0));
    positionAlloc4->Add (Vector(602.5211191465514,831.93611067145, 0.0));
    positionAlloc4->Add (Vector(150.9475380008277,359.8603692956651, 0.0));
    positionAlloc4->Add (Vector(393.66691521780166,410.2175064344312, 0.0));
    positionAlloc4->Add (Vector(691.1993117430701,512.5723693988052, 0.0));
    positionAlloc4->Add (Vector(162.28665435664024,355.3160275463953, 0.0));
    positionAlloc4->Add (Vector(234.14506038440337,335.90798307102426, 0.0));
    positionAlloc4->Add (Vector(689.1107482299672,715.0088761554177, 0.0));
    positionAlloc4->Add (Vector(237.88515033385738,161.8633654227067, 0.0));
    positionAlloc4->Add (Vector(514.9072333578972,833.2096762954495, 0.0));
    positionAlloc4->Add (Vector(691.7939647069338,691.0450373132805, 0.0));
    positionAlloc4->Add (Vector(624.2795872212672,368.30470054181825, 0.0));
    positionAlloc4->Add (Vector(824.4494686213118,513.3225960906841, 0.0));
    positionAlloc4->Add (Vector(156.94667478948494,474.8666116174527, 0.0));
    positionAlloc4->Add (Vector(521.6563748884657,566.5921724339771, 0.0));
    positionAlloc4->Add (Vector(495.00520911321263,433.00282307840837, 0.0));
    positionAlloc4->Add (Vector(237.05787930035717,439.5498724430447, 0.0));
    positionAlloc4->Add (Vector(386.14188273836487,163.52934164232073, 0.0));
    positionAlloc4->Add (Vector(160.2853720584677,415.36583407035533, 0.0));
    positionAlloc4->Add (Vector(337.31487370958007,734.836158019536, 0.0));
    positionAlloc4->Add (Vector(228.0585486248935,511.1033318986297, 0.0));
    positionAlloc4->Add (Vector(407.45019142930704,190.66436645624486, 0.0));
    positionAlloc4->Add (Vector(773.1505018453221,351.1867175259058, 0.0));
    positionAlloc4->Add (Vector(649.2448182978324,289.45069242583713, 0.0));
    positionAlloc4->Add (Vector(601.3828228156196,426.5916353280523, 0.0));
    positionAlloc4->Add (Vector(667.0182206926315,370.939140634345, 0.0));
    positionAlloc4->Add (Vector(245.62063255582083,606.1478323340937, 0.0));
    positionAlloc4->Add (Vector(575.4264855769059,356.71103430028296, 0.0));
    positionAlloc4->Add (Vector(301.0184754235179,624.469240217031, 0.0));
    positionAlloc4->Add (Vector(480.5972629140872,766.1097165450395, 0.0));
    positionAlloc4->Add (Vector(298.39583828906063,610.14743751909, 0.0));
    positionAlloc4->Add (Vector(725.5259207714963,745.2436007839268, 0.0));
    positionAlloc4->Add (Vector(687.2973399625428,441.5531023327153, 0.0));
    positionAlloc4->Add (Vector(408.8692791910236,186.64851413123068, 0.0));
    positionAlloc4->Add (Vector(296.2179068036146,408.09501960535664, 0.0));
    positionAlloc4->Add (Vector(452.91039521379327,470.38972361150576, 0.0));
    positionAlloc4->Add (Vector(819.7598708390003,500.2808019971409, 0.0));
    positionAlloc4->Add (Vector(183.73020867530735,402.03532613068484, 0.0));
    positionAlloc4->Add (Vector(697.2767043414549,600.9044550331412, 0.0));
    positionAlloc4->Add (Vector(358.6365460476102,223.75544557559954, 0.0));
    positionAlloc4->Add (Vector(815.5408954119366,641.7783510743649, 0.0));
    positionAlloc4->Add (Vector(826.233253590074,682.726453175548, 0.0));
    positionAlloc4->Add (Vector(649.1322354440231,616.4697895047445, 0.0));
    positionAlloc4->Add (Vector(153.05428875521636,266.8671621230391, 0.0));
    positionAlloc4->Add (Vector(592.8983151240207,642.6999641440318, 0.0));
    positionAlloc4->Add (Vector(495.66982831985007,421.35416381895294, 0.0));
    positionAlloc4->Add (Vector(212.50266716790105,630.9134865522699, 0.0));
    positionAlloc4->Add (Vector(414.122637774235,824.8548915430757, 0.0));
    positionAlloc4->Add (Vector(453.67219370357066,786.9602006980745, 0.0));
    positionAlloc4->Add (Vector(702.2527302432051,774.3105013675143, 0.0));
    positionAlloc4->Add (Vector(739.8983315123885,369.3435423096002, 0.0));
    positionAlloc4->Add (Vector(402.16374316029953,339.27530046916786, 0.0));
    positionAlloc4->Add (Vector(371.3891048105147,221.32467164620948, 0.0));
    positionAlloc4->Add (Vector(322.7767084413316,668.63283930645, 0.0));
    positionAlloc4->Add (Vector(737.5369446492016,152.67394586100818, 0.0));
    positionAlloc4->Add (Vector(644.8835412190012,193.88273513530612, 0.0));
    positionAlloc4->Add (Vector(182.79633596932217,343.31869047462965, 0.0));
    positionAlloc4->Add (Vector(150.09428510313973,507.7903974660609, 0.0));
    positionAlloc4->Add (Vector(674.05166388834,372.31075668278163, 0.0));
    positionAlloc4->Add (Vector(590.6903878242156,188.03242437603268, 0.0));
    positionAlloc4->Add (Vector(348.9918650492617,243.6798540997852, 0.0));
    positionAlloc4->Add (Vector(339.98656631270006,497.6867959725626, 0.0));
    positionAlloc4->Add (Vector(250.0131729375882,692.7423052821949, 0.0));
    positionAlloc4->Add (Vector(536.7506771855224,514.1189246175259, 0.0));
    positionAlloc4->Add (Vector(725.7743560689378,712.7709552867642, 0.0));
    positionAlloc4->Add (Vector(319.3521762208707,649.7668427546129, 0.0));
    positionAlloc4->Add (Vector(200.2430643585591,627.8320548563817, 0.0));
    positionAlloc4->Add (Vector(560.1053519174634,587.3487039525817, 0.0));
    positionAlloc4->Add (Vector(502.15876554588544,464.5448194277494, 0.0));
    positionAlloc4->Add (Vector(236.04447492577128,772.061356185779, 0.0));
    positionAlloc4->Add (Vector(400.4698588794842,603.4722121933698, 0.0));
    positionAlloc4->Add (Vector(835.2867806223662,572.4451672303651, 0.0));
    positionAlloc4->Add (Vector(308.22029847119984,598.3332295230907, 0.0));
    positionAlloc4->Add (Vector(375.04699087351736,483.5058904406541, 0.0));
    positionAlloc4->Add (Vector(433.5718861692965,784.277701492748, 0.0));
    positionAlloc4->Add (Vector(712.4379203078192,800.4596375422977, 0.0));
    positionAlloc4->Add (Vector(312.6690261392335,346.70314923609953, 0.0));
    positionAlloc4->Add (Vector(432.011297842846,356.7033458335603, 0.0));
    positionAlloc4->Add (Vector(185.15820802478163,201.03128475753246, 0.0));
    positionAlloc4->Add (Vector(706.1608811435241,644.9605340866888, 0.0));
    positionAlloc4->Add (Vector(203.80345269331445,500.43698764929246, 0.0));
    positionAlloc4->Add (Vector(515.8036195357216,359.6959745677117, 0.0));
    positionAlloc4->Add (Vector(200.62153268055704,818.580645104191, 0.0));
    positionAlloc4->Add (Vector(715.7646470064556,813.3816181751296, 0.0));
    positionAlloc4->Add (Vector(394.1743737786355,227.69866915392038, 0.0));
    positionAlloc4->Add (Vector(434.9745423879393,196.2068908981691, 0.0));
    positionAlloc4->Add (Vector(204.35453427012354,435.2801217227016, 0.0));
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
	  flowmon->SerializeToXmlFile ("switch_ECC_flowmon/switch_ECC_3.flowmon", false, false);
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