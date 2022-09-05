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
	  uint16_t numberOfRrhs = 38;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 488;
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
    positionAlloc->Add (Vector (772.2222222222222,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,850.0, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,150.0, 0.0));
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
    positionAlloc4->Add (Vector(213.91621430471253,675.0106254600867, 0.0));
    positionAlloc4->Add (Vector(776.2973187988059,323.9752014089854, 0.0));
    positionAlloc4->Add (Vector(491.5635364799454,280.5947017442618, 0.0));
    positionAlloc4->Add (Vector(616.4986299042391,557.9050331886451, 0.0));
    positionAlloc4->Add (Vector(796.2232199194309,673.6625559921886, 0.0));
    positionAlloc4->Add (Vector(313.1350689672309,562.0505688012431, 0.0));
    positionAlloc4->Add (Vector(331.0481656461251,477.6426039568251, 0.0));
    positionAlloc4->Add (Vector(589.9468010623434,270.85957508267927, 0.0));
    positionAlloc4->Add (Vector(302.4593805814951,379.0400154272504, 0.0));
    positionAlloc4->Add (Vector(548.7954953989533,364.24143875132313, 0.0));
    positionAlloc4->Add (Vector(718.0171212895834,729.9770031489346, 0.0));
    positionAlloc4->Add (Vector(638.763092869191,798.332245007615, 0.0));
    positionAlloc4->Add (Vector(782.6220394262592,784.9821934260927, 0.0));
    positionAlloc4->Add (Vector(542.1890529178811,209.27330866988294, 0.0));
    positionAlloc4->Add (Vector(648.3174596605024,190.9016935982026, 0.0));
    positionAlloc4->Add (Vector(200.61740234095402,670.6371431614808, 0.0));
    positionAlloc4->Add (Vector(235.45979515586288,228.88850230682763, 0.0));
    positionAlloc4->Add (Vector(613.3233882906302,535.3012715629843, 0.0));
    positionAlloc4->Add (Vector(260.7674838719495,267.3299793026406, 0.0));
    positionAlloc4->Add (Vector(469.88625173960344,379.79060549070584, 0.0));
    positionAlloc4->Add (Vector(276.6811447846016,708.0813179536752, 0.0));
    positionAlloc4->Add (Vector(248.6195843992791,763.9190898831461, 0.0));
    positionAlloc4->Add (Vector(678.1663151978952,655.14074520329, 0.0));
    positionAlloc4->Add (Vector(491.7687906034014,331.2221193793531, 0.0));
    positionAlloc4->Add (Vector(805.1339097238713,245.87558666650636, 0.0));
    positionAlloc4->Add (Vector(255.55497108264603,735.1643776380978, 0.0));
    positionAlloc4->Add (Vector(655.5089055216395,580.5513380129574, 0.0));
    positionAlloc4->Add (Vector(510.4631460601114,719.788782201316, 0.0));
    positionAlloc4->Add (Vector(564.070158148761,663.8448245161755, 0.0));
    positionAlloc4->Add (Vector(627.9298473134639,449.04211909728417, 0.0));
    positionAlloc4->Add (Vector(597.7317127024434,271.0457483390828, 0.0));
    positionAlloc4->Add (Vector(514.8895648243083,794.9783116001374, 0.0));
    positionAlloc4->Add (Vector(428.2177479669188,297.2943386190627, 0.0));
    positionAlloc4->Add (Vector(660.1755260649218,673.4436467904706, 0.0));
    positionAlloc4->Add (Vector(671.8269267449386,322.0789928432514, 0.0));
    positionAlloc4->Add (Vector(356.04371656076466,684.3156540260909, 0.0));
    positionAlloc4->Add (Vector(385.8488124748269,677.2397913399606, 0.0));
    positionAlloc4->Add (Vector(558.00855283443,186.16352308029278, 0.0));
    positionAlloc4->Add (Vector(200.26528181182843,215.99318452925525, 0.0));
    positionAlloc4->Add (Vector(374.8786213897388,222.11343610954427, 0.0));
    positionAlloc4->Add (Vector(735.1468511194694,680.9121698999049, 0.0));
    positionAlloc4->Add (Vector(462.0947439391024,392.6045915130454, 0.0));
    positionAlloc4->Add (Vector(618.2300739330324,608.517287483563, 0.0));
    positionAlloc4->Add (Vector(336.35980042222775,572.3040574284121, 0.0));
    positionAlloc4->Add (Vector(527.4613691355382,705.6367649208278, 0.0));
    positionAlloc4->Add (Vector(665.1891421299557,155.49144486843625, 0.0));
    positionAlloc4->Add (Vector(353.4552305557579,313.50535818530636, 0.0));
    positionAlloc4->Add (Vector(349.5696495680959,468.53619336569705, 0.0));
    positionAlloc4->Add (Vector(426.22346616286836,275.8911127848789, 0.0));
    positionAlloc4->Add (Vector(635.3765018188644,791.9319131081332, 0.0));
    positionAlloc4->Add (Vector(265.93404158711996,292.8511408812648, 0.0));
    positionAlloc4->Add (Vector(391.77766344451726,437.728546402228, 0.0));
    positionAlloc4->Add (Vector(684.7979345236802,844.3486328156775, 0.0));
    positionAlloc4->Add (Vector(432.8209652299219,624.845440618514, 0.0));
    positionAlloc4->Add (Vector(438.32823485347785,672.4934629908735, 0.0));
    positionAlloc4->Add (Vector(655.3367017946128,717.4660414384587, 0.0));
    positionAlloc4->Add (Vector(653.152324374018,300.73136927011205, 0.0));
    positionAlloc4->Add (Vector(194.93684019847004,526.54612188752, 0.0));
    positionAlloc4->Add (Vector(493.0834652887474,170.20053933528717, 0.0));
    positionAlloc4->Add (Vector(552.578796857553,666.7343281271494, 0.0));
    positionAlloc4->Add (Vector(401.1330972262964,790.2655751966246, 0.0));
    positionAlloc4->Add (Vector(366.84073930683434,318.22601652138144, 0.0));
    positionAlloc4->Add (Vector(808.4455855643702,291.19319017050924, 0.0));
    positionAlloc4->Add (Vector(664.7898178549021,751.7507373957773, 0.0));
    positionAlloc4->Add (Vector(258.9732878793828,591.2952237182462, 0.0));
    positionAlloc4->Add (Vector(207.19277172156012,244.7650172077708, 0.0));
    positionAlloc4->Add (Vector(786.5522287074776,604.4699129865794, 0.0));
    positionAlloc4->Add (Vector(332.7467599267304,345.09514193449604, 0.0));
    positionAlloc4->Add (Vector(339.0034975255098,174.48774181564397, 0.0));
    positionAlloc4->Add (Vector(756.3635360012457,773.2955426454156, 0.0));
    positionAlloc4->Add (Vector(572.5511671541915,776.8640842665783, 0.0));
    positionAlloc4->Add (Vector(724.4727683807494,465.8148025813171, 0.0));
    positionAlloc4->Add (Vector(479.5321551673916,421.8990548594999, 0.0));
    positionAlloc4->Add (Vector(344.163069333311,483.12475758064346, 0.0));
    positionAlloc4->Add (Vector(536.5505476382407,540.3628733356913, 0.0));
    positionAlloc4->Add (Vector(636.8329229323542,456.90022440782644, 0.0));
    positionAlloc4->Add (Vector(202.72500251718338,172.02003667221825, 0.0));
    positionAlloc4->Add (Vector(421.3619303911593,448.03313045121365, 0.0));
    positionAlloc4->Add (Vector(767.0267354156892,597.0893664182208, 0.0));
    positionAlloc4->Add (Vector(813.541955254148,651.7860447137318, 0.0));
    positionAlloc4->Add (Vector(295.9692782094314,685.5444019558948, 0.0));
    positionAlloc4->Add (Vector(301.5433970862822,334.5010656430207, 0.0));
    positionAlloc4->Add (Vector(679.615753809891,725.6956170113941, 0.0));
    positionAlloc4->Add (Vector(711.3442745791879,790.5537356951642, 0.0));
    positionAlloc4->Add (Vector(421.49156978009285,220.75354466021986, 0.0));
    positionAlloc4->Add (Vector(174.43410135117648,529.1490554831371, 0.0));
    positionAlloc4->Add (Vector(566.1316984388333,391.4224219212283, 0.0));
    positionAlloc4->Add (Vector(226.80666087298408,235.2263983533749, 0.0));
    positionAlloc4->Add (Vector(827.1234205614138,717.9852518904594, 0.0));
    positionAlloc4->Add (Vector(557.0558167368506,241.13254444056167, 0.0));
    positionAlloc4->Add (Vector(657.3525108368048,191.66833781027574, 0.0));
    positionAlloc4->Add (Vector(420.01262952988327,695.3729311370527, 0.0));
    positionAlloc4->Add (Vector(624.7278092123477,781.535882867873, 0.0));
    positionAlloc4->Add (Vector(695.0209510903895,324.09500917652747, 0.0));
    positionAlloc4->Add (Vector(766.9274226254662,560.2114642348076, 0.0));
    positionAlloc4->Add (Vector(628.075093082081,596.1735952613095, 0.0));
    positionAlloc4->Add (Vector(579.1853816190919,521.7021908200737, 0.0));
    positionAlloc4->Add (Vector(352.5835914930889,613.7009813949462, 0.0));
    positionAlloc4->Add (Vector(490.4980845182502,244.1199285369451, 0.0));
    positionAlloc4->Add (Vector(600.051845712532,752.7911701313469, 0.0));
    positionAlloc4->Add (Vector(573.8991917257512,625.9110117630224, 0.0));
    positionAlloc4->Add (Vector(329.7191209390845,220.62216147410123, 0.0));
    positionAlloc4->Add (Vector(812.0910863439005,302.41032397291724, 0.0));
    positionAlloc4->Add (Vector(205.0785598593285,448.30608391550663, 0.0));
    positionAlloc4->Add (Vector(328.6618518429958,212.4748261456088, 0.0));
    positionAlloc4->Add (Vector(613.2356083091771,654.7950975193305, 0.0));
    positionAlloc4->Add (Vector(254.60233149941337,556.1234588575737, 0.0));
    positionAlloc4->Add (Vector(187.588031557562,369.38389303218946, 0.0));
    positionAlloc4->Add (Vector(284.3212848026667,607.2900960829206, 0.0));
    positionAlloc4->Add (Vector(581.2689935045482,282.311947134188, 0.0));
    positionAlloc4->Add (Vector(158.86047993891353,150.84252973631254, 0.0));
    positionAlloc4->Add (Vector(161.31867796816383,564.1681744154528, 0.0));
    positionAlloc4->Add (Vector(603.7176342856804,432.8140720039695, 0.0));
    positionAlloc4->Add (Vector(188.17776247260548,739.547056147182, 0.0));
    positionAlloc4->Add (Vector(447.7157052108073,572.8504704193444, 0.0));
    positionAlloc4->Add (Vector(477.6588421313527,840.4490568540187, 0.0));
    positionAlloc4->Add (Vector(704.9911736859854,452.8497364228251, 0.0));
    positionAlloc4->Add (Vector(260.3820704512031,511.4378731868286, 0.0));
    positionAlloc4->Add (Vector(374.94661407065774,842.6953657664648, 0.0));
    positionAlloc4->Add (Vector(291.01253715205564,754.7699196805162, 0.0));
    positionAlloc4->Add (Vector(615.7833548467778,819.5842361104445, 0.0));
    positionAlloc4->Add (Vector(413.03154346277404,837.7378557427749, 0.0));
    positionAlloc4->Add (Vector(280.7655287333489,608.4337114824817, 0.0));
    positionAlloc4->Add (Vector(714.7003376313093,161.57761450291363, 0.0));
    positionAlloc4->Add (Vector(685.8489502837097,738.0540117814069, 0.0));
    positionAlloc4->Add (Vector(608.9157658713875,529.4675564388231, 0.0));
    positionAlloc4->Add (Vector(201.92084119944568,844.5202001138866, 0.0));
    positionAlloc4->Add (Vector(385.1396915746827,243.96689336562048, 0.0));
    positionAlloc4->Add (Vector(713.0775128406412,294.9593774213973, 0.0));
    positionAlloc4->Add (Vector(308.3887093309046,705.031594830745, 0.0));
    positionAlloc4->Add (Vector(425.4048426058022,194.89467656616827, 0.0));
    positionAlloc4->Add (Vector(667.1792061458257,298.0700381043896, 0.0));
    positionAlloc4->Add (Vector(303.8252101168156,195.34053966764122, 0.0));
    positionAlloc4->Add (Vector(731.0972315936575,747.1779724099255, 0.0));
    positionAlloc4->Add (Vector(340.4589103045844,211.81890485300445, 0.0));
    positionAlloc4->Add (Vector(298.8362327335191,412.0353075877177, 0.0));
    positionAlloc4->Add (Vector(627.0601748096365,394.6133485485399, 0.0));
    positionAlloc4->Add (Vector(354.63331789836775,381.2177037777926, 0.0));
    positionAlloc4->Add (Vector(643.8518624641961,626.1493596535676, 0.0));
    positionAlloc4->Add (Vector(535.9313847846108,573.7802603673409, 0.0));
    positionAlloc4->Add (Vector(302.3661132461568,245.6865521423254, 0.0));
    positionAlloc4->Add (Vector(357.1558773700789,321.3662729884934, 0.0));
    positionAlloc4->Add (Vector(843.0768486144419,694.4541062937833, 0.0));
    positionAlloc4->Add (Vector(270.5528255449229,683.5615527495185, 0.0));
    positionAlloc4->Add (Vector(447.4075489476077,352.2211095619587, 0.0));
    positionAlloc4->Add (Vector(308.4315878159522,478.0058630625973, 0.0));
    positionAlloc4->Add (Vector(360.3687300669472,254.77142520852078, 0.0));
    positionAlloc4->Add (Vector(160.79061625357747,721.8023363787157, 0.0));
    positionAlloc4->Add (Vector(763.8010081183202,697.2055861915718, 0.0));
    positionAlloc4->Add (Vector(636.4756881730473,365.8762143756022, 0.0));
    positionAlloc4->Add (Vector(505.50342633119567,848.8148512673916, 0.0));
    positionAlloc4->Add (Vector(234.79700169423103,154.261271682913, 0.0));
    positionAlloc4->Add (Vector(789.2719336418212,312.37791651045006, 0.0));
    positionAlloc4->Add (Vector(732.9943291140098,580.087006522016, 0.0));
    positionAlloc4->Add (Vector(277.0483397895888,321.1165471796635, 0.0));
    positionAlloc4->Add (Vector(488.3480121412293,382.18317617136995, 0.0));
    positionAlloc4->Add (Vector(326.46906169360994,818.4227970064806, 0.0));
    positionAlloc4->Add (Vector(770.5714694936174,543.5977519372659, 0.0));
    positionAlloc4->Add (Vector(840.1803607652406,184.3375008114557, 0.0));
    positionAlloc4->Add (Vector(337.14129851119736,602.1415684838598, 0.0));
    positionAlloc4->Add (Vector(471.13780296961505,565.5247782344172, 0.0));
    positionAlloc4->Add (Vector(625.3260157957079,732.558046353065, 0.0));
    positionAlloc4->Add (Vector(255.31398554125138,476.1767237302957, 0.0));
    positionAlloc4->Add (Vector(834.5271914502886,454.09409668283064, 0.0));
    positionAlloc4->Add (Vector(696.6820805026366,406.6670245122432, 0.0));
    positionAlloc4->Add (Vector(228.54807747309104,551.1895462907953, 0.0));
    positionAlloc4->Add (Vector(249.94939509769526,220.7876317271608, 0.0));
    positionAlloc4->Add (Vector(299.77321158855676,830.4294659236365, 0.0));
    positionAlloc4->Add (Vector(608.4494143404952,766.0746670444834, 0.0));
    positionAlloc4->Add (Vector(829.7507210394738,835.4667178239891, 0.0));
    positionAlloc4->Add (Vector(180.49247640069402,302.0801803457052, 0.0));
    positionAlloc4->Add (Vector(849.0889372889008,200.19736461945288, 0.0));
    positionAlloc4->Add (Vector(437.85915384216236,338.1579673168843, 0.0));
    positionAlloc4->Add (Vector(670.6157704667119,519.6408953244872, 0.0));
    positionAlloc4->Add (Vector(524.8681178869197,311.29538027003605, 0.0));
    positionAlloc4->Add (Vector(570.0867579576476,259.3829378679489, 0.0));
    positionAlloc4->Add (Vector(729.2366718299043,402.0824008423027, 0.0));
    positionAlloc4->Add (Vector(614.4208261112648,759.8910353321762, 0.0));
    positionAlloc4->Add (Vector(627.9206877389288,808.5019557473759, 0.0));
    positionAlloc4->Add (Vector(820.961392501824,332.5658990349434, 0.0));
    positionAlloc4->Add (Vector(678.9969457446749,465.9352229175021, 0.0));
    positionAlloc4->Add (Vector(469.27377966204887,293.9406328725969, 0.0));
    positionAlloc4->Add (Vector(500.5286923186902,492.692634359324, 0.0));
    positionAlloc4->Add (Vector(369.0619321438509,551.9133997267081, 0.0));
    positionAlloc4->Add (Vector(519.1093931901557,780.6075260811054, 0.0));
    positionAlloc4->Add (Vector(194.74621493435714,650.2812146044679, 0.0));
    positionAlloc4->Add (Vector(788.2280324010339,675.6498754617056, 0.0));
    positionAlloc4->Add (Vector(409.00247840252194,349.3602619746856, 0.0));
    positionAlloc4->Add (Vector(185.10081615026715,427.62615953404134, 0.0));
    positionAlloc4->Add (Vector(347.087703909737,738.6222836980395, 0.0));
    positionAlloc4->Add (Vector(731.408327580116,398.8146245063349, 0.0));
    positionAlloc4->Add (Vector(576.0821323705226,256.8570860004249, 0.0));
    positionAlloc4->Add (Vector(593.1747759024122,185.1491850203009, 0.0));
    positionAlloc4->Add (Vector(692.379436658235,386.1306717723903, 0.0));
    positionAlloc4->Add (Vector(251.70419837604715,474.6808122716425, 0.0));
    positionAlloc4->Add (Vector(717.809383111254,691.5456515040325, 0.0));
    positionAlloc4->Add (Vector(525.984523800718,298.10096454251163, 0.0));
    positionAlloc4->Add (Vector(326.8757394175443,379.03412408023905, 0.0));
    positionAlloc4->Add (Vector(609.3048001522158,748.4479595577499, 0.0));
    positionAlloc4->Add (Vector(194.9646736401946,285.8377161719235, 0.0));
    positionAlloc4->Add (Vector(259.14295676397893,702.9637888369269, 0.0));
    positionAlloc4->Add (Vector(476.8995936732542,718.4233051181958, 0.0));
    positionAlloc4->Add (Vector(388.8414506745074,613.3424111105485, 0.0));
    positionAlloc4->Add (Vector(611.8120725825299,376.5043228276583, 0.0));
    positionAlloc4->Add (Vector(705.2607747913255,255.10755092871267, 0.0));
    positionAlloc4->Add (Vector(281.8428113026151,500.34642655362967, 0.0));
    positionAlloc4->Add (Vector(389.81913985268363,223.90294955362435, 0.0));
    positionAlloc4->Add (Vector(767.6775743364547,819.5947499928183, 0.0));
    positionAlloc4->Add (Vector(579.0841391125523,839.9435340613928, 0.0));
    positionAlloc4->Add (Vector(788.4451170134724,286.2407530422572, 0.0));
    positionAlloc4->Add (Vector(698.5920921326826,228.82500432751826, 0.0));
    positionAlloc4->Add (Vector(290.6818229538677,703.0518888742923, 0.0));
    positionAlloc4->Add (Vector(248.22646383747826,561.1567456202403, 0.0));
    positionAlloc4->Add (Vector(452.1920850594933,828.5447300938777, 0.0));
    positionAlloc4->Add (Vector(155.81338878633082,829.0401520266486, 0.0));
    positionAlloc4->Add (Vector(443.7583214013036,281.7222522370426, 0.0));
    positionAlloc4->Add (Vector(628.9467694118593,300.88029957318173, 0.0));
    positionAlloc4->Add (Vector(395.85374146729873,369.71556143863546, 0.0));
    positionAlloc4->Add (Vector(580.1367057420877,625.7725917984444, 0.0));
    positionAlloc4->Add (Vector(270.67730030891687,653.7647479176981, 0.0));
    positionAlloc4->Add (Vector(503.5817029294721,821.0834577382287, 0.0));
    positionAlloc4->Add (Vector(787.2877627046813,341.79342033081303, 0.0));
    positionAlloc4->Add (Vector(391.1512985997232,587.9670264421152, 0.0));
    positionAlloc4->Add (Vector(810.9565721316508,562.6537689227978, 0.0));
    positionAlloc4->Add (Vector(216.96809729974888,418.04148513084186, 0.0));
    positionAlloc4->Add (Vector(493.7475647575049,569.4229446474044, 0.0));
    positionAlloc4->Add (Vector(329.4928587027945,773.8606087220272, 0.0));
    positionAlloc4->Add (Vector(841.872915047904,478.86145298837573, 0.0));
    positionAlloc4->Add (Vector(449.21537326973987,437.23631603503964, 0.0));
    positionAlloc4->Add (Vector(206.97322164308804,158.00287367054284, 0.0));
    positionAlloc4->Add (Vector(684.8088379660045,354.7118226981771, 0.0));
    positionAlloc4->Add (Vector(354.2762313017771,685.5780573078948, 0.0));
    positionAlloc4->Add (Vector(299.7460093498024,442.499330565492, 0.0));
    positionAlloc4->Add (Vector(491.4694372653193,842.9072388265254, 0.0));
    positionAlloc4->Add (Vector(620.1500450184617,422.415231559436, 0.0));
    positionAlloc4->Add (Vector(668.566555312921,571.0297706168451, 0.0));
    positionAlloc4->Add (Vector(380.7985227899425,423.5252433963653, 0.0));
    positionAlloc4->Add (Vector(801.0047038832163,644.9962625549806, 0.0));
    positionAlloc4->Add (Vector(796.8241447011782,204.370689290987, 0.0));
    positionAlloc4->Add (Vector(577.4499256451513,233.4751173770266, 0.0));
    positionAlloc4->Add (Vector(177.57311402278066,404.2723249935241, 0.0));
    positionAlloc4->Add (Vector(693.781958613802,409.9918383952625, 0.0));
    positionAlloc4->Add (Vector(684.3734749089517,219.83410282669348, 0.0));
    positionAlloc4->Add (Vector(837.2090521671613,437.72979579465135, 0.0));
    positionAlloc4->Add (Vector(523.4560102941593,726.4171376315245, 0.0));
    positionAlloc4->Add (Vector(370.1822683223503,542.7770761378805, 0.0));
    positionAlloc4->Add (Vector(708.242284399112,567.0652726729787, 0.0));
    positionAlloc4->Add (Vector(693.2975825840655,524.5516248454503, 0.0));
    positionAlloc4->Add (Vector(617.5021555677763,694.9871178629795, 0.0));
    positionAlloc4->Add (Vector(296.59387176019663,829.0536332370007, 0.0));
    positionAlloc4->Add (Vector(173.22191042904166,223.15529384990685, 0.0));
    positionAlloc4->Add (Vector(213.40975028227123,229.39919872208907, 0.0));
    positionAlloc4->Add (Vector(748.9218389378196,799.0854091448904, 0.0));
    positionAlloc4->Add (Vector(221.98297124491017,187.08103423227058, 0.0));
    positionAlloc4->Add (Vector(383.60039393305505,844.4269316450162, 0.0));
    positionAlloc4->Add (Vector(690.2668433889969,778.760435213981, 0.0));
    positionAlloc4->Add (Vector(481.16617666956233,828.0296707421506, 0.0));
    positionAlloc4->Add (Vector(227.92002503428512,298.48777910535796, 0.0));
    positionAlloc4->Add (Vector(580.4539245420033,834.0314173926928, 0.0));
    positionAlloc4->Add (Vector(535.2068662281906,339.88801053822374, 0.0));
    positionAlloc4->Add (Vector(384.83362739260883,207.6470932357307, 0.0));
    positionAlloc4->Add (Vector(699.3420248139525,489.8471238161108, 0.0));
    positionAlloc4->Add (Vector(287.08746932596364,489.6772782152588, 0.0));
    positionAlloc4->Add (Vector(202.69056870252268,201.3525316618437, 0.0));
    positionAlloc4->Add (Vector(221.10218096476694,379.8464516675027, 0.0));
    positionAlloc4->Add (Vector(493.79867839426345,627.7353463658034, 0.0));
    positionAlloc4->Add (Vector(594.5171962991267,460.0093542741234, 0.0));
    positionAlloc4->Add (Vector(844.1184428709896,838.251190657622, 0.0));
    positionAlloc4->Add (Vector(581.8200482825713,506.30152847571674, 0.0));
    positionAlloc4->Add (Vector(396.278827960768,312.81624587469526, 0.0));
    positionAlloc4->Add (Vector(778.7564631825135,556.937451852609, 0.0));
    positionAlloc4->Add (Vector(197.84620039539635,158.81482886724635, 0.0));
    positionAlloc4->Add (Vector(241.31744102415257,683.8803193182259, 0.0));
    positionAlloc4->Add (Vector(370.84244453755673,704.7511773030262, 0.0));
    positionAlloc4->Add (Vector(354.9849357221823,625.6556159155888, 0.0));
    positionAlloc4->Add (Vector(713.9793522138247,263.1312425112082, 0.0));
    positionAlloc4->Add (Vector(367.8347110480636,464.7758956196101, 0.0));
    positionAlloc4->Add (Vector(808.8813449243534,562.1561704261608, 0.0));
    positionAlloc4->Add (Vector(565.2897650915946,313.3768162098847, 0.0));
    positionAlloc4->Add (Vector(507.1838523544545,453.64908154516087, 0.0));
    positionAlloc4->Add (Vector(641.9955763772227,579.8305248807944, 0.0));
    positionAlloc4->Add (Vector(200.87750908856765,649.885069181614, 0.0));
    positionAlloc4->Add (Vector(534.991891106167,439.3916026217361, 0.0));
    positionAlloc4->Add (Vector(344.54540753855133,773.2201193495558, 0.0));
    positionAlloc4->Add (Vector(489.3446420633129,224.04523482584688, 0.0));
    positionAlloc4->Add (Vector(176.5984861133195,573.8704924238905, 0.0));
    positionAlloc4->Add (Vector(615.0627875960936,626.7963893506734, 0.0));
    positionAlloc4->Add (Vector(177.47050322838248,841.5878510941827, 0.0));
    positionAlloc4->Add (Vector(405.5358942792638,765.5921171850772, 0.0));
    positionAlloc4->Add (Vector(537.3800187833905,403.6050988215403, 0.0));
    positionAlloc4->Add (Vector(752.5985196304141,677.2494071172141, 0.0));
    positionAlloc4->Add (Vector(390.67574388112666,579.8218389140554, 0.0));
    positionAlloc4->Add (Vector(533.0661995154985,311.823118196176, 0.0));
    positionAlloc4->Add (Vector(413.77372430357957,589.7278231572486, 0.0));
    positionAlloc4->Add (Vector(424.1863428575876,257.78204581597873, 0.0));
    positionAlloc4->Add (Vector(159.56191840470441,204.35354074730353, 0.0));
    positionAlloc4->Add (Vector(567.2966431875743,356.1248691888121, 0.0));
    positionAlloc4->Add (Vector(246.61932625652776,621.3741053810816, 0.0));
    positionAlloc4->Add (Vector(450.6843864644729,491.2083316318519, 0.0));
    positionAlloc4->Add (Vector(240.58210041899753,326.16510453182957, 0.0));
    positionAlloc4->Add (Vector(169.7852907349844,160.43718490836554, 0.0));
    positionAlloc4->Add (Vector(481.64835361547557,603.9483182830429, 0.0));
    positionAlloc4->Add (Vector(619.1298271878453,272.6224045929267, 0.0));
    positionAlloc4->Add (Vector(570.9573388930464,822.5989037493458, 0.0));
    positionAlloc4->Add (Vector(221.80919223171458,746.5655674831377, 0.0));
    positionAlloc4->Add (Vector(683.9988345274602,575.5798173187138, 0.0));
    positionAlloc4->Add (Vector(229.83520839377914,393.3702190282379, 0.0));
    positionAlloc4->Add (Vector(341.9217002580266,229.4881806904457, 0.0));
    positionAlloc4->Add (Vector(579.3732996445234,340.7718183281447, 0.0));
    positionAlloc4->Add (Vector(619.1845807793143,474.4580362509857, 0.0));
    positionAlloc4->Add (Vector(338.5498427263459,842.3521258315932, 0.0));
    positionAlloc4->Add (Vector(379.28308965694504,796.2007214018004, 0.0));
    positionAlloc4->Add (Vector(566.4778501390292,831.7584085012801, 0.0));
    positionAlloc4->Add (Vector(394.2964547113377,467.69727897953663, 0.0));
    positionAlloc4->Add (Vector(702.2656275788635,681.9980108316779, 0.0));
    positionAlloc4->Add (Vector(481.1245875585902,711.4437915175621, 0.0));
    positionAlloc4->Add (Vector(846.9260980600983,203.63386975075406, 0.0));
    positionAlloc4->Add (Vector(337.2583001477067,724.0415560223776, 0.0));
    positionAlloc4->Add (Vector(227.73671106467654,655.9334229877877, 0.0));
    positionAlloc4->Add (Vector(448.5869979691478,280.16741177371864, 0.0));
    positionAlloc4->Add (Vector(681.252364030277,357.95638491459886, 0.0));
    positionAlloc4->Add (Vector(361.24545619539356,743.2953573949462, 0.0));
    positionAlloc4->Add (Vector(835.6155341789766,708.0677025159601, 0.0));
    positionAlloc4->Add (Vector(434.80487527750984,333.38289499496227, 0.0));
    positionAlloc4->Add (Vector(669.9116046343236,611.57935593376, 0.0));
    positionAlloc4->Add (Vector(274.04656587486767,761.1249210682956, 0.0));
    positionAlloc4->Add (Vector(581.5355509788711,280.55770875284185, 0.0));
    positionAlloc4->Add (Vector(245.9417784756069,744.3496124324445, 0.0));
    positionAlloc4->Add (Vector(341.12015697310153,321.1552481410977, 0.0));
    positionAlloc4->Add (Vector(578.5048751293732,534.475889964526, 0.0));
    positionAlloc4->Add (Vector(589.2073053775057,546.3301890221626, 0.0));
    positionAlloc4->Add (Vector(202.0316106475061,352.24002106266823, 0.0));
    positionAlloc4->Add (Vector(186.20461778825742,297.7268311460308, 0.0));
    positionAlloc4->Add (Vector(556.6397287723948,587.9127084288268, 0.0));
    positionAlloc4->Add (Vector(166.77296958698886,684.954338689616, 0.0));
    positionAlloc4->Add (Vector(282.54935937646553,533.5095674460538, 0.0));
    positionAlloc4->Add (Vector(433.01870185383115,762.7303578465356, 0.0));
    positionAlloc4->Add (Vector(254.36697507000375,820.4735266692813, 0.0));
    positionAlloc4->Add (Vector(474.646418152646,440.55469030545936, 0.0));
    positionAlloc4->Add (Vector(754.4101934572334,376.1305092330217, 0.0));
    positionAlloc4->Add (Vector(381.9758972598462,834.0690955003143, 0.0));
    positionAlloc4->Add (Vector(801.3934189249009,449.8399561044723, 0.0));
    positionAlloc4->Add (Vector(555.0904533845696,602.7639622443678, 0.0));
    positionAlloc4->Add (Vector(816.6109692207747,705.0955559747579, 0.0));
    positionAlloc4->Add (Vector(236.71639570689334,545.1092891576693, 0.0));
    positionAlloc4->Add (Vector(717.5743764867059,455.95465793023254, 0.0));
    positionAlloc4->Add (Vector(674.6686261655368,506.91183941448196, 0.0));
    positionAlloc4->Add (Vector(605.2558923468121,679.8963316427589, 0.0));
    positionAlloc4->Add (Vector(344.4348346340786,380.8710661994528, 0.0));
    positionAlloc4->Add (Vector(715.9222026340038,261.95856165685007, 0.0));
    positionAlloc4->Add (Vector(427.2170424116423,838.2690055306791, 0.0));
    positionAlloc4->Add (Vector(658.5089506751917,216.87534534536601, 0.0));
    positionAlloc4->Add (Vector(741.0839286044461,467.30981705258, 0.0));
    positionAlloc4->Add (Vector(586.4554669693555,400.4017354596993, 0.0));
    positionAlloc4->Add (Vector(540.0533314510308,294.7887267004929, 0.0));
    positionAlloc4->Add (Vector(316.78368933795105,745.7643175625011, 0.0));
    positionAlloc4->Add (Vector(763.7934419078783,530.4807402546094, 0.0));
    positionAlloc4->Add (Vector(679.9083074231522,530.7255875448487, 0.0));
    positionAlloc4->Add (Vector(501.67896949047497,387.0757231199217, 0.0));
    positionAlloc4->Add (Vector(463.5239413911242,366.4882151545328, 0.0));
    positionAlloc4->Add (Vector(674.3809053181095,545.6212793567562, 0.0));
    positionAlloc4->Add (Vector(457.64475879890455,192.31284608968915, 0.0));
    positionAlloc4->Add (Vector(748.4418709918397,831.9697678652155, 0.0));
    positionAlloc4->Add (Vector(430.15336687716785,553.8454970197504, 0.0));
    positionAlloc4->Add (Vector(272.1821184524476,199.57029097003516, 0.0));
    positionAlloc4->Add (Vector(200.8776209128704,735.6975044084231, 0.0));
    positionAlloc4->Add (Vector(297.476278129294,587.0032805482888, 0.0));
    positionAlloc4->Add (Vector(770.1465692917623,743.3279619509544, 0.0));
    positionAlloc4->Add (Vector(821.8840669792138,380.3956938892562, 0.0));
    positionAlloc4->Add (Vector(464.26880623476626,835.4737950913897, 0.0));
    positionAlloc4->Add (Vector(450.125985838948,758.8686560697462, 0.0));
    positionAlloc4->Add (Vector(520.0800975751738,799.5783432509313, 0.0));
    positionAlloc4->Add (Vector(319.63086801761733,691.5755425354755, 0.0));
    positionAlloc4->Add (Vector(608.0144250738392,206.02279883615566, 0.0));
    positionAlloc4->Add (Vector(807.0122007241076,371.86170204490986, 0.0));
    positionAlloc4->Add (Vector(761.568058955932,365.12762669142467, 0.0));
    positionAlloc4->Add (Vector(834.3426355853305,153.25113815949214, 0.0));
    positionAlloc4->Add (Vector(822.008860076903,176.4489552685724, 0.0));
    positionAlloc4->Add (Vector(220.3082925033762,258.9132270695055, 0.0));
    positionAlloc4->Add (Vector(692.3051358265392,812.7538205971344, 0.0));
    positionAlloc4->Add (Vector(342.9801278318902,272.65700465628066, 0.0));
    positionAlloc4->Add (Vector(424.20943454182475,562.4902328659607, 0.0));
    positionAlloc4->Add (Vector(522.510239489385,225.0096878488464, 0.0));
    positionAlloc4->Add (Vector(249.2801800813724,152.62156629789936, 0.0));
    positionAlloc4->Add (Vector(477.5089395437521,342.5281812585133, 0.0));
    positionAlloc4->Add (Vector(463.279564826392,791.9151983163895, 0.0));
    positionAlloc4->Add (Vector(260.69285200355523,752.4231432447967, 0.0));
    positionAlloc4->Add (Vector(421.0087758377882,585.815657593883, 0.0));
    positionAlloc4->Add (Vector(216.49065148411847,374.53508981519013, 0.0));
    positionAlloc4->Add (Vector(330.41804291806307,676.259261163096, 0.0));
    positionAlloc4->Add (Vector(764.430839534913,220.02216974700758, 0.0));
    positionAlloc4->Add (Vector(533.6840454872729,467.1103252661319, 0.0));
    positionAlloc4->Add (Vector(366.83099428103446,287.35095581920666, 0.0));
    positionAlloc4->Add (Vector(827.4684958940661,743.0102199021754, 0.0));
    positionAlloc4->Add (Vector(507.70333448121085,789.6703588353488, 0.0));
    positionAlloc4->Add (Vector(624.9244843540125,817.811062883431, 0.0));
    positionAlloc4->Add (Vector(732.5580484527338,742.4441459064963, 0.0));
    positionAlloc4->Add (Vector(184.5043993166631,636.0491222966897, 0.0));
    positionAlloc4->Add (Vector(845.3609517121224,242.09949831524835, 0.0));
    positionAlloc4->Add (Vector(534.4089835531674,655.2972123731786, 0.0));
    positionAlloc4->Add (Vector(383.8540521924931,340.7125087761822, 0.0));
    positionAlloc4->Add (Vector(797.2287726330803,728.3804105324483, 0.0));
    positionAlloc4->Add (Vector(317.7268824197437,751.6693936732429, 0.0));
    positionAlloc4->Add (Vector(213.58672674298992,240.25611379387203, 0.0));
    positionAlloc4->Add (Vector(215.4023067897495,489.0400315904874, 0.0));
    positionAlloc4->Add (Vector(704.55564307075,441.24916488729605, 0.0));
    positionAlloc4->Add (Vector(421.63448904089,279.9228447688032, 0.0));
    positionAlloc4->Add (Vector(204.24005821534402,263.839807421002, 0.0));
    positionAlloc4->Add (Vector(632.888898405175,545.8950278975681, 0.0));
    positionAlloc4->Add (Vector(476.10991608745786,317.1002787545445, 0.0));
    positionAlloc4->Add (Vector(152.88900827222272,164.71736505372155, 0.0));
    positionAlloc4->Add (Vector(357.2171860644272,759.6242533274642, 0.0));
    positionAlloc4->Add (Vector(218.90628430069353,835.5943595743197, 0.0));
    positionAlloc4->Add (Vector(726.1198161570352,311.6582514545136, 0.0));
    positionAlloc4->Add (Vector(321.2803316862908,739.5747096394639, 0.0));
    positionAlloc4->Add (Vector(761.8395941972852,848.4490714740454, 0.0));
    positionAlloc4->Add (Vector(790.4523312392074,842.4289150562671, 0.0));
    positionAlloc4->Add (Vector(292.20775603297767,793.7223935950057, 0.0));
    positionAlloc4->Add (Vector(798.5899805992866,572.3155148722601, 0.0));
    positionAlloc4->Add (Vector(450.9179306873936,157.71965898823024, 0.0));
    positionAlloc4->Add (Vector(695.9735416721455,484.51411395714035, 0.0));
    positionAlloc4->Add (Vector(641.2117285290494,679.4322505361114, 0.0));
    positionAlloc4->Add (Vector(626.8207853076534,442.21924976561905, 0.0));
    positionAlloc4->Add (Vector(195.53217041599842,832.4107734018701, 0.0));
    positionAlloc4->Add (Vector(634.2730102875355,235.13270977375458, 0.0));
    positionAlloc4->Add (Vector(196.9196553262659,777.0034223236011, 0.0));
    positionAlloc4->Add (Vector(385.87535149278085,357.0693539259518, 0.0));
    positionAlloc4->Add (Vector(236.7574359851076,499.4480524732867, 0.0));
    positionAlloc4->Add (Vector(622.2717944162132,188.9470615300919, 0.0));
    positionAlloc4->Add (Vector(542.2659020957378,428.57009410703847, 0.0));
    positionAlloc4->Add (Vector(320.68736978715833,358.5134320659554, 0.0));
    positionAlloc4->Add (Vector(652.7665964868991,227.76804619219337, 0.0));
    positionAlloc4->Add (Vector(268.8064078235111,150.11042120463972, 0.0));
    positionAlloc4->Add (Vector(493.4267818182176,301.6806505937749, 0.0));
    positionAlloc4->Add (Vector(594.4399647184449,749.4095287012818, 0.0));
    positionAlloc4->Add (Vector(712.186916325327,448.6717864310633, 0.0));
    positionAlloc4->Add (Vector(727.9260707553158,805.7417646661013, 0.0));
    positionAlloc4->Add (Vector(382.3343994282477,587.0974490290747, 0.0));
    positionAlloc4->Add (Vector(840.8395999527486,428.630221872867, 0.0));
    positionAlloc4->Add (Vector(590.1216580796497,590.7708563019212, 0.0));
    positionAlloc4->Add (Vector(206.8953733886214,164.59166786539058, 0.0));
    positionAlloc4->Add (Vector(702.5103143757051,241.1361219092567, 0.0));
    positionAlloc4->Add (Vector(716.3722517761323,806.9234371872682, 0.0));
    positionAlloc4->Add (Vector(787.4287551608237,160.3811137021955, 0.0));
    positionAlloc4->Add (Vector(704.5519232087462,680.6904445477915, 0.0));
    positionAlloc4->Add (Vector(522.3802141475487,399.7573789584542, 0.0));
    positionAlloc4->Add (Vector(544.4293316689964,234.00192473182165, 0.0));
    positionAlloc4->Add (Vector(766.5240039519326,680.3603316220807, 0.0));
    positionAlloc4->Add (Vector(351.55642371999056,772.5133066797529, 0.0));
    positionAlloc4->Add (Vector(369.9690643496534,831.4685862860763, 0.0));
    positionAlloc4->Add (Vector(782.1007596060474,719.8478011706765, 0.0));
    positionAlloc4->Add (Vector(837.4280613136874,194.12355674983309, 0.0));
    positionAlloc4->Add (Vector(770.7140743621331,619.4643212014696, 0.0));
    positionAlloc4->Add (Vector(158.60346280896962,565.5568056127672, 0.0));
    positionAlloc4->Add (Vector(215.32968418887847,270.86018076831283, 0.0));
    positionAlloc4->Add (Vector(562.0822820352113,694.8769300472482, 0.0));
    positionAlloc4->Add (Vector(523.2404650583281,220.04527009657332, 0.0));
    positionAlloc4->Add (Vector(690.167674417522,793.6714402389828, 0.0));
    positionAlloc4->Add (Vector(467.8387258266844,710.8469301705403, 0.0));
    positionAlloc4->Add (Vector(265.31872685429033,179.50529187405394, 0.0));
    positionAlloc4->Add (Vector(387.8001806852591,805.1686076100224, 0.0));
    positionAlloc4->Add (Vector(722.5402382326654,353.3371239262606, 0.0));
    positionAlloc4->Add (Vector(455.44583601252816,702.9384011919917, 0.0));
    positionAlloc4->Add (Vector(795.4919278705872,609.7058945197114, 0.0));
    positionAlloc4->Add (Vector(679.6359516629617,366.6872185818555, 0.0));
    positionAlloc4->Add (Vector(548.793242424787,305.1154013076132, 0.0));
    positionAlloc4->Add (Vector(318.3745605611629,510.8828958525995, 0.0));
    positionAlloc4->Add (Vector(285.7494741964763,263.17063488759806, 0.0));
    positionAlloc4->Add (Vector(825.8399599055538,808.6044835262882, 0.0));
    positionAlloc4->Add (Vector(171.13441073603204,223.733725971475, 0.0));
    positionAlloc4->Add (Vector(658.178145529843,186.2218357683708, 0.0));
    positionAlloc4->Add (Vector(846.3691205126406,230.35039506242867, 0.0));
    positionAlloc4->Add (Vector(411.4249517549054,845.2195527188329, 0.0));
    positionAlloc4->Add (Vector(273.99481835510176,404.5394241398099, 0.0));
    positionAlloc4->Add (Vector(242.80752612682542,426.5534277717348, 0.0));
    positionAlloc4->Add (Vector(307.52827933093954,483.7566567302682, 0.0));
    positionAlloc4->Add (Vector(479.17428996198333,760.4208143900067, 0.0));
    positionAlloc4->Add (Vector(844.3979063103958,278.02564063043945, 0.0));
    positionAlloc4->Add (Vector(722.7484009593242,153.9564733888293, 0.0));
    positionAlloc4->Add (Vector(450.8994583778486,599.0992068955699, 0.0));
    positionAlloc4->Add (Vector(759.073820840873,555.2555915849023, 0.0));
    positionAlloc4->Add (Vector(616.5769187083822,759.5672973429325, 0.0));
    positionAlloc4->Add (Vector(650.3991819402863,659.6731446574197, 0.0));
    positionAlloc4->Add (Vector(357.2574623948152,543.1313118540185, 0.0));
    positionAlloc4->Add (Vector(750.5702187782558,624.6422637513726, 0.0));
    positionAlloc4->Add (Vector(239.19454835157939,531.8937809174306, 0.0));
    positionAlloc4->Add (Vector(840.7576159259277,364.2042736462603, 0.0));
    positionAlloc4->Add (Vector(317.6421001990909,304.85505992760943, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_16.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_16.flowmon", false, false);
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