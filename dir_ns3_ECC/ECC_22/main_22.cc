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
	  uint16_t numberOfNodes = 647;
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
    positionAlloc->Add (Vector (850.0,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (850.0,538.8888888888889, 0.0));
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
    positionAlloc->Add (Vector (616.6666666666667,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,150.0, 0.0));
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
    positionAlloc->Add (Vector (383.33333333333337,850.0, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,616.6666666666667, 0.0));
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
    positionAlloc->Add (Vector (227.77777777777777,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,150.0, 0.0));
    positionAlloc->Add (Vector (150.0,850.0, 0.0));
    positionAlloc->Add (Vector (150.0,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (150.0,538.8888888888889, 0.0));
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
    positionAlloc4->Add (Vector(376.6069616908359,842.7850074505656, 0.0));
    positionAlloc4->Add (Vector(838.3905806864996,362.7212975795753, 0.0));
    positionAlloc4->Add (Vector(446.1738368934309,326.6149985310466, 0.0));
    positionAlloc4->Add (Vector(472.26661279818427,728.5495536107055, 0.0));
    positionAlloc4->Add (Vector(800.974503046357,749.6856623285951, 0.0));
    positionAlloc4->Add (Vector(339.4880266455034,571.2668661998864, 0.0));
    positionAlloc4->Add (Vector(348.0495503580327,395.15059109205663, 0.0));
    positionAlloc4->Add (Vector(812.7370998478224,521.1533334746956, 0.0));
    positionAlloc4->Add (Vector(419.5704752583619,228.93939094240716, 0.0));
    positionAlloc4->Add (Vector(761.3389153232198,548.0005071981499, 0.0));
    positionAlloc4->Add (Vector(458.9793637562488,194.08453559064313, 0.0));
    positionAlloc4->Add (Vector(836.8712204776735,347.2268048392184, 0.0));
    positionAlloc4->Add (Vector(535.8152210315568,828.6685924479207, 0.0));
    positionAlloc4->Add (Vector(680.6969230565013,173.61107522040328, 0.0));
    positionAlloc4->Add (Vector(710.9430007731986,394.1534119724147, 0.0));
    positionAlloc4->Add (Vector(685.8209130728047,400.52197150798247, 0.0));
    positionAlloc4->Add (Vector(302.92930171362457,605.8178723889587, 0.0));
    positionAlloc4->Add (Vector(566.8844962840974,576.3463184781488, 0.0));
    positionAlloc4->Add (Vector(235.68756468059684,681.383366908982, 0.0));
    positionAlloc4->Add (Vector(320.7744369929943,350.0382301539313, 0.0));
    positionAlloc4->Add (Vector(417.55348733602375,779.0709288945466, 0.0));
    positionAlloc4->Add (Vector(575.6435997135778,500.54564365842106, 0.0));
    positionAlloc4->Add (Vector(239.99778522207822,839.4329743944387, 0.0));
    positionAlloc4->Add (Vector(697.1768455717338,639.4923567347731, 0.0));
    positionAlloc4->Add (Vector(401.49384103519174,152.522967921801, 0.0));
    positionAlloc4->Add (Vector(345.25471604638136,814.3925070416651, 0.0));
    positionAlloc4->Add (Vector(376.48315080765553,326.7285082648957, 0.0));
    positionAlloc4->Add (Vector(526.3414736224815,733.2335631049526, 0.0));
    positionAlloc4->Add (Vector(408.1251501539342,300.78749368349816, 0.0));
    positionAlloc4->Add (Vector(576.7069074125204,652.0901824311352, 0.0));
    positionAlloc4->Add (Vector(608.3989670177282,178.09015235477904, 0.0));
    positionAlloc4->Add (Vector(740.5846824066468,355.3062572461755, 0.0));
    positionAlloc4->Add (Vector(748.2100381666367,206.81147438730045, 0.0));
    positionAlloc4->Add (Vector(646.1126327954054,527.5480912048274, 0.0));
    positionAlloc4->Add (Vector(378.1660807681047,259.67049404326264, 0.0));
    positionAlloc4->Add (Vector(512.8787335515781,788.0843338136212, 0.0));
    positionAlloc4->Add (Vector(689.7052070269198,719.7412745962591, 0.0));
    positionAlloc4->Add (Vector(444.3572506535726,734.217017039822, 0.0));
    positionAlloc4->Add (Vector(775.6302557939325,329.29176367348066, 0.0));
    positionAlloc4->Add (Vector(303.4014777628062,753.8326712316014, 0.0));
    positionAlloc4->Add (Vector(597.9275052582764,308.7929668507395, 0.0));
    positionAlloc4->Add (Vector(311.12689520988044,783.4428942409869, 0.0));
    positionAlloc4->Add (Vector(740.2816364534199,169.10135545539592, 0.0));
    positionAlloc4->Add (Vector(335.04158240868736,385.128789081905, 0.0));
    positionAlloc4->Add (Vector(700.815438474674,341.56637787127863, 0.0));
    positionAlloc4->Add (Vector(565.4333398980133,848.1411844984488, 0.0));
    positionAlloc4->Add (Vector(433.30771801145494,725.8524430024096, 0.0));
    positionAlloc4->Add (Vector(474.9576163324392,379.65545252801655, 0.0));
    positionAlloc4->Add (Vector(567.7517610101081,397.8461032555086, 0.0));
    positionAlloc4->Add (Vector(195.31643625353644,229.25805457585227, 0.0));
    positionAlloc4->Add (Vector(515.5822831703113,799.0836280555461, 0.0));
    positionAlloc4->Add (Vector(737.5757797839385,756.0339109277793, 0.0));
    positionAlloc4->Add (Vector(551.5240502866623,505.4740102835616, 0.0));
    positionAlloc4->Add (Vector(199.20132982094307,813.7449432708252, 0.0));
    positionAlloc4->Add (Vector(458.0928932323689,580.8418935398603, 0.0));
    positionAlloc4->Add (Vector(726.9045836399239,383.3224706156694, 0.0));
    positionAlloc4->Add (Vector(733.5880518958184,417.1264525879077, 0.0));
    positionAlloc4->Add (Vector(715.6079940677415,420.8727556624161, 0.0));
    positionAlloc4->Add (Vector(518.0235398666825,313.4583010152852, 0.0));
    positionAlloc4->Add (Vector(573.9050739215331,319.86994523226315, 0.0));
    positionAlloc4->Add (Vector(806.4162289267955,695.39607975832, 0.0));
    positionAlloc4->Add (Vector(349.6367983990943,705.0054909717297, 0.0));
    positionAlloc4->Add (Vector(735.5941247134102,356.6463006148537, 0.0));
    positionAlloc4->Add (Vector(232.7138469737647,764.1143332184403, 0.0));
    positionAlloc4->Add (Vector(347.14505534741977,160.0152428559643, 0.0));
    positionAlloc4->Add (Vector(729.0433600607147,688.2888001234846, 0.0));
    positionAlloc4->Add (Vector(666.6292635099444,379.1418789793116, 0.0));
    positionAlloc4->Add (Vector(524.862855122502,843.2282661688059, 0.0));
    positionAlloc4->Add (Vector(309.65245760925325,199.02465773181552, 0.0));
    positionAlloc4->Add (Vector(606.1101315874637,446.7646550919051, 0.0));
    positionAlloc4->Add (Vector(585.2768604140423,722.1059831120584, 0.0));
    positionAlloc4->Add (Vector(317.8890906403061,753.6391283747133, 0.0));
    positionAlloc4->Add (Vector(699.4078183957218,833.3901203059279, 0.0));
    positionAlloc4->Add (Vector(678.2352479359245,457.6095969068413, 0.0));
    positionAlloc4->Add (Vector(733.2017127036977,406.4748667003637, 0.0));
    positionAlloc4->Add (Vector(318.49870084758646,391.8105266223599, 0.0));
    positionAlloc4->Add (Vector(304.492464385634,183.14388332259853, 0.0));
    positionAlloc4->Add (Vector(568.6320757269164,284.4999940875881, 0.0));
    positionAlloc4->Add (Vector(698.1059779567121,619.6961374198213, 0.0));
    positionAlloc4->Add (Vector(757.396263448014,348.9047514230308, 0.0));
    positionAlloc4->Add (Vector(491.26123138432337,252.52953205482186, 0.0));
    positionAlloc4->Add (Vector(192.02541910165107,423.94593709040595, 0.0));
    positionAlloc4->Add (Vector(707.9695149471416,817.1474503893209, 0.0));
    positionAlloc4->Add (Vector(523.5372114086323,223.8259410065936, 0.0));
    positionAlloc4->Add (Vector(410.31635321201384,714.1614220999072, 0.0));
    positionAlloc4->Add (Vector(599.4400027161846,615.6840470257221, 0.0));
    positionAlloc4->Add (Vector(185.9527511229004,575.7000596682051, 0.0));
    positionAlloc4->Add (Vector(646.015315335402,178.3120096861005, 0.0));
    positionAlloc4->Add (Vector(285.6101503376125,306.04320318008837, 0.0));
    positionAlloc4->Add (Vector(497.96877057415844,314.02337563361834, 0.0));
    positionAlloc4->Add (Vector(788.4211795159621,830.6300972172753, 0.0));
    positionAlloc4->Add (Vector(802.0775311102726,618.6367034230195, 0.0));
    positionAlloc4->Add (Vector(371.05490143507575,279.28624064594374, 0.0));
    positionAlloc4->Add (Vector(759.4651343331292,166.20237571894467, 0.0));
    positionAlloc4->Add (Vector(419.05877301404234,241.72954640180268, 0.0));
    positionAlloc4->Add (Vector(181.78710925859258,254.71279203935327, 0.0));
    positionAlloc4->Add (Vector(842.1403864865889,200.41866601123544, 0.0));
    positionAlloc4->Add (Vector(469.40974925338907,742.7077861529027, 0.0));
    positionAlloc4->Add (Vector(467.98742024183576,714.9929800981715, 0.0));
    positionAlloc4->Add (Vector(406.76706632882815,340.5780515691263, 0.0));
    positionAlloc4->Add (Vector(622.3192092066618,698.032900509192, 0.0));
    positionAlloc4->Add (Vector(579.4915133680597,812.0168953926169, 0.0));
    positionAlloc4->Add (Vector(323.9903949117183,360.4838307066194, 0.0));
    positionAlloc4->Add (Vector(471.16162075080445,579.5186481385881, 0.0));
    positionAlloc4->Add (Vector(331.9157385148648,283.16791866615927, 0.0));
    positionAlloc4->Add (Vector(313.311601290804,593.1148536766129, 0.0));
    positionAlloc4->Add (Vector(204.52563591660993,189.1217877771527, 0.0));
    positionAlloc4->Add (Vector(495.60344312347377,768.7947850506861, 0.0));
    positionAlloc4->Add (Vector(797.3751213518188,309.9676033947211, 0.0));
    positionAlloc4->Add (Vector(606.3935977060985,570.7905869511881, 0.0));
    positionAlloc4->Add (Vector(495.01542568085864,631.6259934871902, 0.0));
    positionAlloc4->Add (Vector(713.2870495278828,271.69219847620695, 0.0));
    positionAlloc4->Add (Vector(692.0100824461056,375.1788326669241, 0.0));
    positionAlloc4->Add (Vector(326.23115915212753,274.61116305506835, 0.0));
    positionAlloc4->Add (Vector(194.1736574010749,202.35171515200562, 0.0));
    positionAlloc4->Add (Vector(732.3092865456747,367.2310049941941, 0.0));
    positionAlloc4->Add (Vector(367.2170920804425,184.89281188011802, 0.0));
    positionAlloc4->Add (Vector(349.27998005961115,617.8543430998095, 0.0));
    positionAlloc4->Add (Vector(636.7156756654715,199.46115487207038, 0.0));
    positionAlloc4->Add (Vector(574.6355762667074,280.00482985067725, 0.0));
    positionAlloc4->Add (Vector(824.1448789341142,539.0060083874241, 0.0));
    positionAlloc4->Add (Vector(637.0799607535064,817.2512213543304, 0.0));
    positionAlloc4->Add (Vector(204.68500155282095,400.14408416521474, 0.0));
    positionAlloc4->Add (Vector(381.6832486062299,473.45922323742775, 0.0));
    positionAlloc4->Add (Vector(546.8509665257792,561.5595668717851, 0.0));
    positionAlloc4->Add (Vector(499.5206036568714,641.3113481180178, 0.0));
    positionAlloc4->Add (Vector(803.0692294994549,731.5225883043892, 0.0));
    positionAlloc4->Add (Vector(606.4460737796944,470.8896822545384, 0.0));
    positionAlloc4->Add (Vector(767.6433282878277,729.9859859255289, 0.0));
    positionAlloc4->Add (Vector(588.455575516634,241.85746624410322, 0.0));
    positionAlloc4->Add (Vector(365.87244903116243,351.70167402806976, 0.0));
    positionAlloc4->Add (Vector(784.0404670498684,187.41885978823885, 0.0));
    positionAlloc4->Add (Vector(813.0735136943838,285.0256205769641, 0.0));
    positionAlloc4->Add (Vector(445.0932292818437,568.0488859301952, 0.0));
    positionAlloc4->Add (Vector(592.1828510585938,553.8616698507722, 0.0));
    positionAlloc4->Add (Vector(659.4540420850278,436.69111304172856, 0.0));
    positionAlloc4->Add (Vector(644.8306311730046,671.3254851363334, 0.0));
    positionAlloc4->Add (Vector(502.0537559582742,499.69350915444653, 0.0));
    positionAlloc4->Add (Vector(296.22372397038345,196.94593997896297, 0.0));
    positionAlloc4->Add (Vector(208.94941784948094,517.4231979286054, 0.0));
    positionAlloc4->Add (Vector(318.85618757657767,707.305355616152, 0.0));
    positionAlloc4->Add (Vector(637.5715555229556,505.04957717466283, 0.0));
    positionAlloc4->Add (Vector(700.4057501891888,439.001489431951, 0.0));
    positionAlloc4->Add (Vector(265.66652314728066,440.2813967546021, 0.0));
    positionAlloc4->Add (Vector(709.4986653673549,495.3283937575828, 0.0));
    positionAlloc4->Add (Vector(478.5987261612709,633.642475358723, 0.0));
    positionAlloc4->Add (Vector(443.9900589191651,257.09705067472214, 0.0));
    positionAlloc4->Add (Vector(441.00952949567125,566.2189177642845, 0.0));
    positionAlloc4->Add (Vector(355.3876370135765,597.7649867528894, 0.0));
    positionAlloc4->Add (Vector(793.9259265732752,448.78995222693584, 0.0));
    positionAlloc4->Add (Vector(671.9297336233708,691.122329094002, 0.0));
    positionAlloc4->Add (Vector(456.5902150933762,551.2816991849982, 0.0));
    positionAlloc4->Add (Vector(789.017221655888,618.2671542201126, 0.0));
    positionAlloc4->Add (Vector(310.4443478138837,250.55315555048372, 0.0));
    positionAlloc4->Add (Vector(670.856310873542,826.0770971091426, 0.0));
    positionAlloc4->Add (Vector(550.7312595570962,447.81438158627185, 0.0));
    positionAlloc4->Add (Vector(180.48260449036275,686.0628616216214, 0.0));
    positionAlloc4->Add (Vector(511.24327565268396,787.1590394977131, 0.0));
    positionAlloc4->Add (Vector(369.5883466489318,286.99939044978566, 0.0));
    positionAlloc4->Add (Vector(193.41741274694817,750.6546505574606, 0.0));
    positionAlloc4->Add (Vector(646.5645512169224,352.49018560713466, 0.0));
    positionAlloc4->Add (Vector(291.273725738434,676.4328494641027, 0.0));
    positionAlloc4->Add (Vector(776.2768220328676,181.68081970938601, 0.0));
    positionAlloc4->Add (Vector(555.7413991656293,315.0330590045011, 0.0));
    positionAlloc4->Add (Vector(201.36487282064317,718.91103460211, 0.0));
    positionAlloc4->Add (Vector(656.9597661147853,419.9610828432578, 0.0));
    positionAlloc4->Add (Vector(699.8115215965676,228.74857046971977, 0.0));
    positionAlloc4->Add (Vector(303.1842743113848,225.22998277807687, 0.0));
    positionAlloc4->Add (Vector(254.36283811010162,165.49741405950215, 0.0));
    positionAlloc4->Add (Vector(521.5655496632913,419.79692466065103, 0.0));
    positionAlloc4->Add (Vector(378.54602936427614,621.074921178271, 0.0));
    positionAlloc4->Add (Vector(379.35852094514996,373.8851463043769, 0.0));
    positionAlloc4->Add (Vector(569.3500572346684,319.19961246392256, 0.0));
    positionAlloc4->Add (Vector(669.2690364865606,830.5280777309288, 0.0));
    positionAlloc4->Add (Vector(389.3900637015895,779.7096515234671, 0.0));
    positionAlloc4->Add (Vector(245.13516760488926,373.8441023342298, 0.0));
    positionAlloc4->Add (Vector(440.13370918529847,173.00390805659788, 0.0));
    positionAlloc4->Add (Vector(646.6974014786426,311.24090894248013, 0.0));
    positionAlloc4->Add (Vector(822.8455308560018,231.58268140063825, 0.0));
    positionAlloc4->Add (Vector(487.46254116713595,666.9997270444256, 0.0));
    positionAlloc4->Add (Vector(392.64778605892036,614.2964458004728, 0.0));
    positionAlloc4->Add (Vector(660.9988176929792,279.1583969190981, 0.0));
    positionAlloc4->Add (Vector(460.3240045353561,270.5853700490518, 0.0));
    positionAlloc4->Add (Vector(182.76977006395927,362.5889283902627, 0.0));
    positionAlloc4->Add (Vector(780.7269484138162,763.9270403303132, 0.0));
    positionAlloc4->Add (Vector(212.66098985641986,494.8649580813701, 0.0));
    positionAlloc4->Add (Vector(552.2047725713998,681.5264980048813, 0.0));
    positionAlloc4->Add (Vector(315.65048351733094,814.1605807743448, 0.0));
    positionAlloc4->Add (Vector(448.07200255819106,702.8358414562665, 0.0));
    positionAlloc4->Add (Vector(387.69905096367665,776.8932112288143, 0.0));
    positionAlloc4->Add (Vector(744.7991312062759,379.0247666135225, 0.0));
    positionAlloc4->Add (Vector(772.9720851307228,477.27818358680844, 0.0));
    positionAlloc4->Add (Vector(213.00555831215704,459.4581120325444, 0.0));
    positionAlloc4->Add (Vector(164.11296748252587,320.4543825170163, 0.0));
    positionAlloc4->Add (Vector(684.3054867912632,758.0029424262177, 0.0));
    positionAlloc4->Add (Vector(534.9922874799518,758.299585566262, 0.0));
    positionAlloc4->Add (Vector(375.3375345072752,720.6012181980491, 0.0));
    positionAlloc4->Add (Vector(586.7037791321305,637.0308365945265, 0.0));
    positionAlloc4->Add (Vector(666.7532338782395,341.24128217108745, 0.0));
    positionAlloc4->Add (Vector(264.5940726968933,210.42279144031517, 0.0));
    positionAlloc4->Add (Vector(632.64240942887,782.0837065322196, 0.0));
    positionAlloc4->Add (Vector(515.4586664135936,285.86617750272364, 0.0));
    positionAlloc4->Add (Vector(440.01028828169586,501.48966704993745, 0.0));
    positionAlloc4->Add (Vector(690.991460964268,764.0423408658033, 0.0));
    positionAlloc4->Add (Vector(239.61733400275375,847.5422063237743, 0.0));
    positionAlloc4->Add (Vector(572.7267101013406,574.0402856356616, 0.0));
    positionAlloc4->Add (Vector(515.3509669626321,816.9632098947585, 0.0));
    positionAlloc4->Add (Vector(487.45681076259126,474.4510963170296, 0.0));
    positionAlloc4->Add (Vector(345.66951978827325,264.14610715204446, 0.0));
    positionAlloc4->Add (Vector(455.087747254187,525.6346319984709, 0.0));
    positionAlloc4->Add (Vector(834.6686835209993,726.0652240155513, 0.0));
    positionAlloc4->Add (Vector(214.3751971222595,552.7671925269593, 0.0));
    positionAlloc4->Add (Vector(401.00521047483824,267.39194319280887, 0.0));
    positionAlloc4->Add (Vector(651.1086805082645,718.4731772048628, 0.0));
    positionAlloc4->Add (Vector(708.1939799021782,323.0439925974546, 0.0));
    positionAlloc4->Add (Vector(317.142505823692,666.3278132124929, 0.0));
    positionAlloc4->Add (Vector(338.40754867976284,677.9413061197454, 0.0));
    positionAlloc4->Add (Vector(786.1790498678537,815.1178385341656, 0.0));
    positionAlloc4->Add (Vector(816.9740432485071,333.46572802596756, 0.0));
    positionAlloc4->Add (Vector(194.20444133480441,565.3405600319302, 0.0));
    positionAlloc4->Add (Vector(555.7399693082009,663.4090456674608, 0.0));
    positionAlloc4->Add (Vector(387.2795860340919,398.36099349696144, 0.0));
    positionAlloc4->Add (Vector(760.0164452331902,781.4937079129035, 0.0));
    positionAlloc4->Add (Vector(386.7515620277539,302.7741827581, 0.0));
    positionAlloc4->Add (Vector(209.95527574078713,739.726485048838, 0.0));
    positionAlloc4->Add (Vector(646.8370987590874,403.9801073088115, 0.0));
    positionAlloc4->Add (Vector(521.4460796659565,221.7094813628574, 0.0));
    positionAlloc4->Add (Vector(759.1691607729013,699.341242199669, 0.0));
    positionAlloc4->Add (Vector(374.6055315167788,644.5925700260987, 0.0));
    positionAlloc4->Add (Vector(211.34966518081808,452.40321216640774, 0.0));
    positionAlloc4->Add (Vector(241.32365299380828,193.5558867651899, 0.0));
    positionAlloc4->Add (Vector(302.0009104879432,224.5940186187362, 0.0));
    positionAlloc4->Add (Vector(792.1848482649239,371.53217214650084, 0.0));
    positionAlloc4->Add (Vector(339.37439089379234,590.041850898225, 0.0));
    positionAlloc4->Add (Vector(230.80354798845644,671.2765084971713, 0.0));
    positionAlloc4->Add (Vector(633.1633333901663,218.8180221660662, 0.0));
    positionAlloc4->Add (Vector(258.56598960249664,160.65157078368404, 0.0));
    positionAlloc4->Add (Vector(553.0333186013618,745.5688665290855, 0.0));
    positionAlloc4->Add (Vector(837.7281920506856,673.3702390561106, 0.0));
    positionAlloc4->Add (Vector(572.6605376085322,520.8074627660671, 0.0));
    positionAlloc4->Add (Vector(661.5962412298265,271.0023674921089, 0.0));
    positionAlloc4->Add (Vector(812.5206662133846,685.6703260025308, 0.0));
    positionAlloc4->Add (Vector(495.4091903937961,194.0835081026625, 0.0));
    positionAlloc4->Add (Vector(247.24923593900397,406.4683745796096, 0.0));
    positionAlloc4->Add (Vector(332.4405085124765,270.8390891884566, 0.0));
    positionAlloc4->Add (Vector(398.95390298218774,453.800011616834, 0.0));
    positionAlloc4->Add (Vector(643.4892469343065,207.66721735488744, 0.0));
    positionAlloc4->Add (Vector(607.7424517913429,584.2400029220512, 0.0));
    positionAlloc4->Add (Vector(402.76340529670495,532.0442397839274, 0.0));
    positionAlloc4->Add (Vector(691.4368423649793,658.5994958294556, 0.0));
    positionAlloc4->Add (Vector(296.34989971024436,846.1958977460037, 0.0));
    positionAlloc4->Add (Vector(663.5508456837965,803.4936181219132, 0.0));
    positionAlloc4->Add (Vector(818.3275752621698,651.2945442074987, 0.0));
    positionAlloc4->Add (Vector(155.63683513769683,739.7689359013272, 0.0));
    positionAlloc4->Add (Vector(341.32352396999914,707.2260084044937, 0.0));
    positionAlloc4->Add (Vector(195.22325366042466,520.9041635856743, 0.0));
    positionAlloc4->Add (Vector(424.64917233709184,318.72081263514804, 0.0));
    positionAlloc4->Add (Vector(156.9157246494396,364.1974861300656, 0.0));
    positionAlloc4->Add (Vector(260.34396220177246,199.69243380597052, 0.0));
    positionAlloc4->Add (Vector(640.6874042152788,180.90923437821186, 0.0));
    positionAlloc4->Add (Vector(692.2737378614178,751.3268586824097, 0.0));
    positionAlloc4->Add (Vector(843.5067373876445,548.0075264829984, 0.0));
    positionAlloc4->Add (Vector(261.7040629506995,771.9859678923576, 0.0));
    positionAlloc4->Add (Vector(507.6202057572648,421.4747621074133, 0.0));
    positionAlloc4->Add (Vector(373.0927293197523,262.3335334219293, 0.0));
    positionAlloc4->Add (Vector(530.6531329509105,829.9773868926487, 0.0));
    positionAlloc4->Add (Vector(728.6460034397039,339.7782333548007, 0.0));
    positionAlloc4->Add (Vector(802.4320853670359,624.9708918058597, 0.0));
    positionAlloc4->Add (Vector(332.8639407127991,377.355009551821, 0.0));
    positionAlloc4->Add (Vector(477.1873501802254,620.7040813736401, 0.0));
    positionAlloc4->Add (Vector(806.8952964691389,507.283282103322, 0.0));
    positionAlloc4->Add (Vector(498.8852798992434,427.60499291810413, 0.0));
    positionAlloc4->Add (Vector(401.1800248400685,713.9089744005393, 0.0));
    positionAlloc4->Add (Vector(692.8470261452546,801.2320032375186, 0.0));
    positionAlloc4->Add (Vector(528.7496445590314,371.5514979372035, 0.0));
    positionAlloc4->Add (Vector(778.0135363793305,354.7553865203445, 0.0));
    positionAlloc4->Add (Vector(287.2204411924554,847.0237589416707, 0.0));
    positionAlloc4->Add (Vector(575.858013487365,494.0347640929125, 0.0));
    positionAlloc4->Add (Vector(588.8269758675019,162.6766061015689, 0.0));
    positionAlloc4->Add (Vector(323.3550689322233,689.9661321041594, 0.0));
    positionAlloc4->Add (Vector(490.34608196388984,567.9870640242845, 0.0));
    positionAlloc4->Add (Vector(812.555864804359,200.2235749579996, 0.0));
    positionAlloc4->Add (Vector(753.604770687948,389.77951596154566, 0.0));
    positionAlloc4->Add (Vector(674.3056379739347,292.07858991492833, 0.0));
    positionAlloc4->Add (Vector(574.8017925220632,715.8284792450951, 0.0));
    positionAlloc4->Add (Vector(498.07802116393003,192.2813081550489, 0.0));
    positionAlloc4->Add (Vector(230.9993096982787,507.65140361590335, 0.0));
    positionAlloc4->Add (Vector(775.3400669629341,578.0015650595222, 0.0));
    positionAlloc4->Add (Vector(448.5983032980752,200.6003627852218, 0.0));
    positionAlloc4->Add (Vector(536.8940363140143,554.9134290968875, 0.0));
    positionAlloc4->Add (Vector(342.7413950969109,162.34438033664244, 0.0));
    positionAlloc4->Add (Vector(215.08619351135388,584.7858220382672, 0.0));
    positionAlloc4->Add (Vector(377.2373067849111,585.8814695875838, 0.0));
    positionAlloc4->Add (Vector(422.6693258607873,512.962692826799, 0.0));
    positionAlloc4->Add (Vector(647.0159120501291,514.2745581950736, 0.0));
    positionAlloc4->Add (Vector(167.10191651538167,841.9521489497444, 0.0));
    positionAlloc4->Add (Vector(765.8739836938165,798.8584361840761, 0.0));
    positionAlloc4->Add (Vector(683.3739872225025,405.7374845274903, 0.0));
    positionAlloc4->Add (Vector(426.1869448755517,220.2236494352956, 0.0));
    positionAlloc4->Add (Vector(378.7065782012188,410.07885711471204, 0.0));
    positionAlloc4->Add (Vector(777.8741851974318,354.1097614585367, 0.0));
    positionAlloc4->Add (Vector(650.3385509494817,832.5801549320081, 0.0));
    positionAlloc4->Add (Vector(604.159037205222,370.62508686112227, 0.0));
    positionAlloc4->Add (Vector(298.43035353507173,515.0790144354598, 0.0));
    positionAlloc4->Add (Vector(838.4452440598424,442.32367311167775, 0.0));
    positionAlloc4->Add (Vector(186.7343765805827,555.3876296427015, 0.0));
    positionAlloc4->Add (Vector(824.4850967141383,260.7189232865983, 0.0));
    positionAlloc4->Add (Vector(829.6945039666699,595.0157447786884, 0.0));
    positionAlloc4->Add (Vector(779.0594686281119,646.3958940915979, 0.0));
    positionAlloc4->Add (Vector(772.7175241953355,637.4528868415308, 0.0));
    positionAlloc4->Add (Vector(525.5750415379973,467.34747653993674, 0.0));
    positionAlloc4->Add (Vector(439.4783659204421,411.5880188153561, 0.0));
    positionAlloc4->Add (Vector(647.8450125138158,414.3403560981012, 0.0));
    positionAlloc4->Add (Vector(519.9606198118323,634.3316198585346, 0.0));
    positionAlloc4->Add (Vector(425.02460708182537,499.7834220202842, 0.0));
    positionAlloc4->Add (Vector(498.1879656439984,455.916853269071, 0.0));
    positionAlloc4->Add (Vector(498.96828690247855,477.4528393307791, 0.0));
    positionAlloc4->Add (Vector(687.3728031260954,616.1510218544759, 0.0));
    positionAlloc4->Add (Vector(238.54367152417797,187.150947519957, 0.0));
    positionAlloc4->Add (Vector(338.33573855633995,640.2755057447084, 0.0));
    positionAlloc4->Add (Vector(495.5042070705275,455.64192462591217, 0.0));
    positionAlloc4->Add (Vector(527.5087846839829,344.57070999168945, 0.0));
    positionAlloc4->Add (Vector(791.3741622978444,295.3722538513747, 0.0));
    positionAlloc4->Add (Vector(167.0048923798462,727.1325323711515, 0.0));
    positionAlloc4->Add (Vector(431.24967161393124,224.01468754851533, 0.0));
    positionAlloc4->Add (Vector(476.3797187821614,152.3687906017842, 0.0));
    positionAlloc4->Add (Vector(380.08817105208453,234.99548563188375, 0.0));
    positionAlloc4->Add (Vector(203.35287541153374,537.214295769973, 0.0));
    positionAlloc4->Add (Vector(424.2145673995847,474.98683169827933, 0.0));
    positionAlloc4->Add (Vector(834.4929611296399,448.78898655828465, 0.0));
    positionAlloc4->Add (Vector(385.70161809606657,583.4060861020291, 0.0));
    positionAlloc4->Add (Vector(629.4078380187932,476.629535961488, 0.0));
    positionAlloc4->Add (Vector(328.64148681091854,570.3903592424405, 0.0));
    positionAlloc4->Add (Vector(602.5279643020847,630.9619304725929, 0.0));
    positionAlloc4->Add (Vector(652.884394388852,292.60572335136084, 0.0));
    positionAlloc4->Add (Vector(179.9212245672263,436.40179531182605, 0.0));
    positionAlloc4->Add (Vector(194.5643332687053,196.08641951501176, 0.0));
    positionAlloc4->Add (Vector(734.9120262250475,416.45230268822013, 0.0));
    positionAlloc4->Add (Vector(469.37011809240613,437.1435036984726, 0.0));
    positionAlloc4->Add (Vector(217.20770847955595,502.12063655769043, 0.0));
    positionAlloc4->Add (Vector(581.5212546164073,796.2675686097314, 0.0));
    positionAlloc4->Add (Vector(741.3712368005004,332.3815098768165, 0.0));
    positionAlloc4->Add (Vector(163.00623383856941,817.4897647040689, 0.0));
    positionAlloc4->Add (Vector(637.1262560243651,526.0765904023372, 0.0));
    positionAlloc4->Add (Vector(292.0481199913918,449.77549018193224, 0.0));
    positionAlloc4->Add (Vector(614.9525805952176,568.3774896194652, 0.0));
    positionAlloc4->Add (Vector(757.5377922174829,621.7289765025041, 0.0));
    positionAlloc4->Add (Vector(269.34826525152596,746.5035886964064, 0.0));
    positionAlloc4->Add (Vector(483.51201540568,237.4183192684523, 0.0));
    positionAlloc4->Add (Vector(570.7679429607565,590.4401102147626, 0.0));
    positionAlloc4->Add (Vector(152.35366482605565,416.4677846627433, 0.0));
    positionAlloc4->Add (Vector(831.0479972862297,202.90956468381313, 0.0));
    positionAlloc4->Add (Vector(758.621371988283,479.48385234831204, 0.0));
    positionAlloc4->Add (Vector(464.11852559325433,754.1181727841231, 0.0));
    positionAlloc4->Add (Vector(524.415170827313,629.2701391911032, 0.0));
    positionAlloc4->Add (Vector(633.8240012041961,319.8249118674165, 0.0));
    positionAlloc4->Add (Vector(358.00452308229416,487.65299152531765, 0.0));
    positionAlloc4->Add (Vector(558.8134259074922,252.31574766828606, 0.0));
    positionAlloc4->Add (Vector(228.53899512939486,733.47419697648, 0.0));
    positionAlloc4->Add (Vector(632.6357156100182,816.0836795737177, 0.0));
    positionAlloc4->Add (Vector(521.5314240300775,488.42834502634577, 0.0));
    positionAlloc4->Add (Vector(321.00320617601517,774.7580414449706, 0.0));
    positionAlloc4->Add (Vector(433.8676550205605,309.05448586993737, 0.0));
    positionAlloc4->Add (Vector(284.95330507337405,242.78414537341206, 0.0));
    positionAlloc4->Add (Vector(434.880536437764,562.1774252738053, 0.0));
    positionAlloc4->Add (Vector(787.8187091605148,505.4116748085667, 0.0));
    positionAlloc4->Add (Vector(440.92102680526745,220.11970504100327, 0.0));
    positionAlloc4->Add (Vector(630.3652196810726,606.7569949507272, 0.0));
    positionAlloc4->Add (Vector(278.76339431243514,621.9471122713534, 0.0));
    positionAlloc4->Add (Vector(280.27039335746906,352.83172142005924, 0.0));
    positionAlloc4->Add (Vector(349.19669705589615,716.6108307150442, 0.0));
    positionAlloc4->Add (Vector(692.7901604764965,517.3834596609638, 0.0));
    positionAlloc4->Add (Vector(707.8948367853504,708.8773043020936, 0.0));
    positionAlloc4->Add (Vector(574.0970778687271,352.17944602175453, 0.0));
    positionAlloc4->Add (Vector(672.0607341362075,270.9287757935059, 0.0));
    positionAlloc4->Add (Vector(164.5348628071227,409.58385921579264, 0.0));
    positionAlloc4->Add (Vector(187.08515531181786,204.87189887918095, 0.0));
    positionAlloc4->Add (Vector(632.588195901359,568.7770513619364, 0.0));
    positionAlloc4->Add (Vector(777.4551934567677,417.3527191084305, 0.0));
    positionAlloc4->Add (Vector(848.7762531303791,401.0277365968195, 0.0));
    positionAlloc4->Add (Vector(532.2047260735112,640.2139146201332, 0.0));
    positionAlloc4->Add (Vector(293.1373651754057,190.9550060146055, 0.0));
    positionAlloc4->Add (Vector(658.3698119551595,635.3779183907743, 0.0));
    positionAlloc4->Add (Vector(848.5284969279519,451.69533944771865, 0.0));
    positionAlloc4->Add (Vector(373.939544598211,481.45601141874516, 0.0));
    positionAlloc4->Add (Vector(264.95448122169955,199.61985406638829, 0.0));
    positionAlloc4->Add (Vector(676.431035817839,259.02483185926104, 0.0));
    positionAlloc4->Add (Vector(550.3943617146957,677.5097512996466, 0.0));
    positionAlloc4->Add (Vector(733.2604847914639,782.4718581389548, 0.0));
    positionAlloc4->Add (Vector(210.53910950614699,664.7573548127688, 0.0));
    positionAlloc4->Add (Vector(675.7493351936073,510.70680634201193, 0.0));
    positionAlloc4->Add (Vector(653.2368707276477,493.063483850053, 0.0));
    positionAlloc4->Add (Vector(393.66232257617816,308.10352713551924, 0.0));
    positionAlloc4->Add (Vector(543.6354291742447,202.23333889809572, 0.0));
    positionAlloc4->Add (Vector(692.5999348317372,285.12711890564424, 0.0));
    positionAlloc4->Add (Vector(742.345438915274,468.3808407070548, 0.0));
    positionAlloc4->Add (Vector(487.3238618893077,834.5705628681532, 0.0));
    positionAlloc4->Add (Vector(207.92325272290998,563.7691594903948, 0.0));
    positionAlloc4->Add (Vector(407.7821667610563,657.0538945942528, 0.0));
    positionAlloc4->Add (Vector(379.7443179999957,774.3789389429238, 0.0));
    positionAlloc4->Add (Vector(252.8000040481416,280.0313393443471, 0.0));
    positionAlloc4->Add (Vector(395.9513100322466,233.05909129657294, 0.0));
    positionAlloc4->Add (Vector(671.6061574687501,386.597221918346, 0.0));
    positionAlloc4->Add (Vector(635.9571424279079,198.5048546266495, 0.0));
    positionAlloc4->Add (Vector(802.1043571130036,496.8773089979409, 0.0));
    positionAlloc4->Add (Vector(765.8566452454218,461.2672295871014, 0.0));
    positionAlloc4->Add (Vector(567.865977383271,157.51728725858774, 0.0));
    positionAlloc4->Add (Vector(769.9193663948847,678.6180146067294, 0.0));
    positionAlloc4->Add (Vector(577.3328579475751,444.0896552969327, 0.0));
    positionAlloc4->Add (Vector(849.1782416599738,286.92629737653465, 0.0));
    positionAlloc4->Add (Vector(223.28269779855816,285.12531355002477, 0.0));
    positionAlloc4->Add (Vector(798.2929871412636,550.1967563709254, 0.0));
    positionAlloc4->Add (Vector(347.32563941953265,303.3467753017196, 0.0));
    positionAlloc4->Add (Vector(487.1938358908063,675.8611322356575, 0.0));
    positionAlloc4->Add (Vector(185.0749209579779,508.62071787966346, 0.0));
    positionAlloc4->Add (Vector(390.82855203030954,762.2056614711397, 0.0));
    positionAlloc4->Add (Vector(703.3446493738314,300.01578351328214, 0.0));
    positionAlloc4->Add (Vector(797.6083728733805,536.3159594830474, 0.0));
    positionAlloc4->Add (Vector(574.9405947539833,354.0269168681359, 0.0));
    positionAlloc4->Add (Vector(666.5636057983243,668.8663879960354, 0.0));
    positionAlloc4->Add (Vector(439.4439040455761,403.0888471091831, 0.0));
    positionAlloc4->Add (Vector(776.6020157771975,629.7371473449493, 0.0));
    positionAlloc4->Add (Vector(304.1728583019603,615.8738001190097, 0.0));
    positionAlloc4->Add (Vector(741.676257030027,156.77077719284344, 0.0));
    positionAlloc4->Add (Vector(626.8078506313124,627.6843133687223, 0.0));
    positionAlloc4->Add (Vector(849.5096331744489,562.8269981724738, 0.0));
    positionAlloc4->Add (Vector(534.1546186589192,338.4991638660965, 0.0));
    positionAlloc4->Add (Vector(828.6244706820825,653.4899695484509, 0.0));
    positionAlloc4->Add (Vector(713.0054246439798,772.5363284150137, 0.0));
    positionAlloc4->Add (Vector(400.31966798052196,150.69929458685988, 0.0));
    positionAlloc4->Add (Vector(592.0493746714483,419.7821829626137, 0.0));
    positionAlloc4->Add (Vector(265.8872258422857,454.4551302543863, 0.0));
    positionAlloc4->Add (Vector(443.209463572233,609.5731489321013, 0.0));
    positionAlloc4->Add (Vector(773.5305769062941,790.1907943817107, 0.0));
    positionAlloc4->Add (Vector(432.4412326963535,319.61397856649495, 0.0));
    positionAlloc4->Add (Vector(720.2884922840108,370.3486710689674, 0.0));
    positionAlloc4->Add (Vector(802.6463285308707,837.0647102034059, 0.0));
    positionAlloc4->Add (Vector(507.84984722141473,317.7564931112664, 0.0));
    positionAlloc4->Add (Vector(830.2929379965519,275.143401285341, 0.0));
    positionAlloc4->Add (Vector(312.6266526723493,345.69970375939863, 0.0));
    positionAlloc4->Add (Vector(601.9857078687426,847.6206384384374, 0.0));
    positionAlloc4->Add (Vector(387.60954401616914,236.58428244716544, 0.0));
    positionAlloc4->Add (Vector(354.6831454923594,756.6598468202345, 0.0));
    positionAlloc4->Add (Vector(256.5820910783137,181.40083532977008, 0.0));
    positionAlloc4->Add (Vector(373.51022422873,202.27560297169674, 0.0));
    positionAlloc4->Add (Vector(246.95474447761143,388.62000712842917, 0.0));
    positionAlloc4->Add (Vector(218.666056368687,832.843348831739, 0.0));
    positionAlloc4->Add (Vector(430.6253227866386,433.74612953786504, 0.0));
    positionAlloc4->Add (Vector(769.537627120976,429.46741876842543, 0.0));
    positionAlloc4->Add (Vector(746.7501707156351,347.4473371809937, 0.0));
    positionAlloc4->Add (Vector(651.4060313407869,577.6493359516202, 0.0));
    positionAlloc4->Add (Vector(385.43844696453675,691.0534782518289, 0.0));
    positionAlloc4->Add (Vector(325.88141442698895,214.11442390112808, 0.0));
    positionAlloc4->Add (Vector(205.23229080920754,167.09002037555547, 0.0));
    positionAlloc4->Add (Vector(545.3100202792964,319.28449646812584, 0.0));
    positionAlloc4->Add (Vector(458.0232071028038,424.71139092384743, 0.0));
    positionAlloc4->Add (Vector(418.81052080902896,586.2812829849596, 0.0));
    positionAlloc4->Add (Vector(284.4683516409468,336.2614151082647, 0.0));
    positionAlloc4->Add (Vector(799.7016546269494,515.2493367004203, 0.0));
    positionAlloc4->Add (Vector(314.61916738722823,451.5193824169067, 0.0));
    positionAlloc4->Add (Vector(264.05550727833224,304.45216127526993, 0.0));
    positionAlloc4->Add (Vector(509.73329878477097,556.7759651889537, 0.0));
    positionAlloc4->Add (Vector(647.0728662149819,769.3807866019702, 0.0));
    positionAlloc4->Add (Vector(460.37696608340855,169.01291101755734, 0.0));
    positionAlloc4->Add (Vector(468.6297646240683,692.3668045895777, 0.0));
    positionAlloc4->Add (Vector(379.2334707002361,460.84531617597634, 0.0));
    positionAlloc4->Add (Vector(806.830845003563,210.05376178342556, 0.0));
    positionAlloc4->Add (Vector(408.3208294678185,618.8591094422308, 0.0));
    positionAlloc4->Add (Vector(155.6831649258964,460.2801389515341, 0.0));
    positionAlloc4->Add (Vector(495.6521391794857,469.91769633665757, 0.0));
    positionAlloc4->Add (Vector(763.4859420056374,237.39603490329904, 0.0));
    positionAlloc4->Add (Vector(797.2512988076899,527.0689717798032, 0.0));
    positionAlloc4->Add (Vector(279.3354918949498,626.9559029063918, 0.0));
    positionAlloc4->Add (Vector(348.9580954462797,782.875533820345, 0.0));
    positionAlloc4->Add (Vector(524.2667284250176,445.77334841948374, 0.0));
    positionAlloc4->Add (Vector(506.82528812963704,407.58943485639554, 0.0));
    positionAlloc4->Add (Vector(547.8539311933157,187.4260068879399, 0.0));
    positionAlloc4->Add (Vector(505.27682756445284,389.49892210792734, 0.0));
    positionAlloc4->Add (Vector(252.75563160587404,354.25625270660765, 0.0));
    positionAlloc4->Add (Vector(694.3025189273352,340.06160441314995, 0.0));
    positionAlloc4->Add (Vector(549.3994963912717,649.0340532553863, 0.0));
    positionAlloc4->Add (Vector(656.8534952532823,499.98847212338455, 0.0));
    positionAlloc4->Add (Vector(395.22226811063,410.2945118927712, 0.0));
    positionAlloc4->Add (Vector(700.1101379151738,688.3430313668197, 0.0));
    positionAlloc4->Add (Vector(847.1346482704396,566.88549086274, 0.0));
    positionAlloc4->Add (Vector(238.5679734209046,577.2154470780672, 0.0));
    positionAlloc4->Add (Vector(776.6030112802094,701.7008097775378, 0.0));
    positionAlloc4->Add (Vector(441.0280713729213,464.60574271563183, 0.0));
    positionAlloc4->Add (Vector(461.47665971456195,505.3619975158617, 0.0));
    positionAlloc4->Add (Vector(758.0390858356723,767.9157872875712, 0.0));
    positionAlloc4->Add (Vector(781.1509244392165,531.9505328270194, 0.0));
    positionAlloc4->Add (Vector(562.3978711144065,158.90909718294438, 0.0));
    positionAlloc4->Add (Vector(350.6457969937825,663.8807107450933, 0.0));
    positionAlloc4->Add (Vector(694.1611904473841,556.7268269198704, 0.0));
    positionAlloc4->Add (Vector(645.3580681826841,524.3508128031806, 0.0));
    positionAlloc4->Add (Vector(381.6383199748622,655.5032631199576, 0.0));
    positionAlloc4->Add (Vector(177.33823288878926,669.7990775197193, 0.0));
    positionAlloc4->Add (Vector(691.6122158564548,366.7433265958752, 0.0));
    positionAlloc4->Add (Vector(820.6794702295234,194.6633208685545, 0.0));
    positionAlloc4->Add (Vector(387.7734369819682,415.93363203275834, 0.0));
    positionAlloc4->Add (Vector(661.3054060475839,639.0307142077846, 0.0));
    positionAlloc4->Add (Vector(190.70711959289508,746.4929037335704, 0.0));
    positionAlloc4->Add (Vector(376.00196091861676,211.7082509766094, 0.0));
    positionAlloc4->Add (Vector(665.8857170048062,777.8147692809633, 0.0));
    positionAlloc4->Add (Vector(725.1118530400696,740.8394329509949, 0.0));
    positionAlloc4->Add (Vector(203.91868745986204,428.85290038671803, 0.0));
    positionAlloc4->Add (Vector(171.37938470481686,213.86967587960464, 0.0));
    positionAlloc4->Add (Vector(624.2042791124104,848.0645501113108, 0.0));
    positionAlloc4->Add (Vector(289.7536134269339,558.3243347706396, 0.0));
    positionAlloc4->Add (Vector(283.7316124587067,605.1144727623506, 0.0));
    positionAlloc4->Add (Vector(710.9185853988309,700.4547556216893, 0.0));
    positionAlloc4->Add (Vector(742.1715557961088,601.1675949692521, 0.0));
    positionAlloc4->Add (Vector(553.4161739223396,488.98230061262893, 0.0));
    positionAlloc4->Add (Vector(506.9143583322846,597.0974072340122, 0.0));
    positionAlloc4->Add (Vector(631.8472396781599,829.7203918715697, 0.0));
    positionAlloc4->Add (Vector(563.749496753394,781.4745260028191, 0.0));
    positionAlloc4->Add (Vector(211.66499549832065,751.6700347791394, 0.0));
    positionAlloc4->Add (Vector(202.92111222627534,350.47760489930494, 0.0));
    positionAlloc4->Add (Vector(159.63528533477998,532.1958947960825, 0.0));
    positionAlloc4->Add (Vector(307.0430304602352,811.7073714561692, 0.0));
    positionAlloc4->Add (Vector(426.91942729487914,168.4061084678707, 0.0));
    positionAlloc4->Add (Vector(483.6117252437443,824.6662787346997, 0.0));
    positionAlloc4->Add (Vector(836.3231445768814,847.3055533710956, 0.0));
    positionAlloc4->Add (Vector(153.94329486696776,839.9662161171549, 0.0));
    positionAlloc4->Add (Vector(417.0771897314826,831.0543687525916, 0.0));
    positionAlloc4->Add (Vector(303.9839564578137,386.28416917022764, 0.0));
    positionAlloc4->Add (Vector(727.4536863485563,531.7414888010815, 0.0));
    positionAlloc4->Add (Vector(777.118685947283,566.8188378954917, 0.0));
    positionAlloc4->Add (Vector(369.0924277457133,262.4108095061881, 0.0));
    positionAlloc4->Add (Vector(664.8200761727949,535.9831324341304, 0.0));
    positionAlloc4->Add (Vector(848.0046078307167,331.56696138587336, 0.0));
    positionAlloc4->Add (Vector(683.6937351520064,654.6181521556869, 0.0));
    positionAlloc4->Add (Vector(380.4930104557203,737.5823407261539, 0.0));
    positionAlloc4->Add (Vector(498.4091635346475,556.3969165135513, 0.0));
    positionAlloc4->Add (Vector(815.9838609971109,255.79379989422569, 0.0));
    positionAlloc4->Add (Vector(711.7685744043212,730.4059822086073, 0.0));
    positionAlloc4->Add (Vector(669.3093395678691,847.5897174643244, 0.0));
    positionAlloc4->Add (Vector(761.8321400388149,662.4838602874411, 0.0));
    positionAlloc4->Add (Vector(594.5357491417424,316.03604579375485, 0.0));
    positionAlloc4->Add (Vector(649.2771218758896,689.0275209774862, 0.0));
    positionAlloc4->Add (Vector(569.4379637147757,594.9557498112572, 0.0));
    positionAlloc4->Add (Vector(441.54328865834896,825.8436569064494, 0.0));
    positionAlloc4->Add (Vector(181.5776968642757,435.5040743979633, 0.0));
    positionAlloc4->Add (Vector(794.957181889663,542.0030548540813, 0.0));
    positionAlloc4->Add (Vector(427.58219088153055,464.8445590959922, 0.0));
    positionAlloc4->Add (Vector(180.69291309767425,314.060110482664, 0.0));
    positionAlloc4->Add (Vector(496.1517183571455,843.9654529138069, 0.0));
    positionAlloc4->Add (Vector(608.8259910335007,687.4185668968963, 0.0));
    positionAlloc4->Add (Vector(172.56825899758812,676.1941721255205, 0.0));
    positionAlloc4->Add (Vector(790.5390108433081,767.435361876176, 0.0));
    positionAlloc4->Add (Vector(271.35055114796785,251.8378547194821, 0.0));
    positionAlloc4->Add (Vector(432.1528798901596,847.8908306236032, 0.0));
    positionAlloc4->Add (Vector(255.61463277722314,237.58131175463987, 0.0));
    positionAlloc4->Add (Vector(750.3322498377951,176.56534038261705, 0.0));
    positionAlloc4->Add (Vector(252.81215907817884,399.9462971273142, 0.0));
    positionAlloc4->Add (Vector(373.7686298642128,474.27090183866557, 0.0));
    positionAlloc4->Add (Vector(710.6207393354256,435.0349947800975, 0.0));
    positionAlloc4->Add (Vector(817.2934735451109,578.8628730063994, 0.0));
    positionAlloc4->Add (Vector(750.3278647278902,194.68169441827948, 0.0));
    positionAlloc4->Add (Vector(846.257956070677,847.2468686363578, 0.0));
    positionAlloc4->Add (Vector(525.9519511323283,590.4532804620295, 0.0));
    positionAlloc4->Add (Vector(628.3986132926026,162.34198625381728, 0.0));
    positionAlloc4->Add (Vector(790.2836472957624,800.1069385944311, 0.0));
    positionAlloc4->Add (Vector(517.9164928798614,247.26215515992388, 0.0));
    positionAlloc4->Add (Vector(658.5296525263102,487.32362024023917, 0.0));
    positionAlloc4->Add (Vector(382.38837618030186,544.5610897445106, 0.0));
    positionAlloc4->Add (Vector(403.2442741071138,280.4556616513621, 0.0));
    positionAlloc4->Add (Vector(272.4625596305915,663.2536987069872, 0.0));
    positionAlloc4->Add (Vector(502.14792443518166,496.2677975591404, 0.0));
    positionAlloc4->Add (Vector(357.09581564843086,265.1940136338743, 0.0));
    positionAlloc4->Add (Vector(677.1604761089305,823.3803088418496, 0.0));
    positionAlloc4->Add (Vector(294.31539124184496,844.3694595622064, 0.0));
    positionAlloc4->Add (Vector(588.2311293260016,261.14340040108357, 0.0));
    positionAlloc4->Add (Vector(760.1180056660927,400.52865988195356, 0.0));
    positionAlloc4->Add (Vector(505.12295047575424,240.68761643234694, 0.0));
    positionAlloc4->Add (Vector(599.9222242435291,324.66530031030663, 0.0));
    positionAlloc4->Add (Vector(174.79328129660462,672.1418565989894, 0.0));
    positionAlloc4->Add (Vector(445.37856642611916,815.2690060653823, 0.0));
    positionAlloc4->Add (Vector(817.2278178376197,839.8327793031026, 0.0));
    positionAlloc4->Add (Vector(450.25034131184225,683.5405410482578, 0.0));
    positionAlloc4->Add (Vector(755.7595610507407,180.42787952660038, 0.0));
    positionAlloc4->Add (Vector(165.55444766395686,385.8881857088754, 0.0));
    positionAlloc4->Add (Vector(529.3166676408767,269.15024441740337, 0.0));
    positionAlloc4->Add (Vector(274.0497357883658,378.1985429749219, 0.0));
    positionAlloc4->Add (Vector(628.5707652176548,835.5630737665182, 0.0));
    positionAlloc4->Add (Vector(528.9109213794253,339.9583105175395, 0.0));
    positionAlloc4->Add (Vector(648.9271014419521,839.769598187534, 0.0));
    positionAlloc4->Add (Vector(283.10770403882736,623.9284796034021, 0.0));
    positionAlloc4->Add (Vector(718.6581620116466,551.6209697812105, 0.0));
    positionAlloc4->Add (Vector(793.5089068643721,694.5247330832079, 0.0));
    positionAlloc4->Add (Vector(782.3247736884781,849.0859771213103, 0.0));
    positionAlloc4->Add (Vector(730.2713120474714,340.24635083383805, 0.0));
    positionAlloc4->Add (Vector(650.4092874741697,530.3053522597527, 0.0));
    positionAlloc4->Add (Vector(515.5702760968188,187.33155288701226, 0.0));
    positionAlloc4->Add (Vector(279.1415140150784,409.86445413188363, 0.0));
    positionAlloc4->Add (Vector(315.4827712671802,469.40711818945925, 0.0));
    positionAlloc4->Add (Vector(666.0887781879931,781.3916660052092, 0.0));
    positionAlloc4->Add (Vector(198.13942137980348,704.4740455904706, 0.0));
    positionAlloc4->Add (Vector(165.11293779987278,418.46056615721346, 0.0));
    positionAlloc4->Add (Vector(711.48151301403,698.8662904058936, 0.0));
    positionAlloc4->Add (Vector(537.6290854365291,235.655380136217, 0.0));
    positionAlloc4->Add (Vector(599.6288243655589,468.75741345249446, 0.0));
    positionAlloc4->Add (Vector(240.35156571478066,313.2803490177986, 0.0));
    positionAlloc4->Add (Vector(357.56520427380224,288.3181693887556, 0.0));
    positionAlloc4->Add (Vector(820.9630257269985,336.304962850812, 0.0));
    positionAlloc4->Add (Vector(227.17114067639656,338.32348414313765, 0.0));
    positionAlloc4->Add (Vector(515.9500131359921,650.8987448311505, 0.0));
    positionAlloc4->Add (Vector(271.0786672221526,796.0112221415235, 0.0));
    positionAlloc4->Add (Vector(352.62878207077534,773.109453801376, 0.0));
    positionAlloc4->Add (Vector(238.15969144676586,282.46471766448906, 0.0));
    positionAlloc4->Add (Vector(627.904824414712,541.3021490523815, 0.0));
    positionAlloc4->Add (Vector(829.5587044908704,651.5690233549833, 0.0));
    positionAlloc4->Add (Vector(788.4951313462284,768.2436413114235, 0.0));
    positionAlloc4->Add (Vector(343.02238240952545,185.68044817322294, 0.0));
    positionAlloc4->Add (Vector(363.4968628118801,582.5427797865938, 0.0));
    positionAlloc4->Add (Vector(414.7077775652523,588.4558793833805, 0.0));
    positionAlloc4->Add (Vector(318.2140246611574,596.6337408029077, 0.0));
    positionAlloc4->Add (Vector(838.2964218026217,357.4114315834988, 0.0));
    positionAlloc4->Add (Vector(800.4742406701952,461.69538178607127, 0.0));
    positionAlloc4->Add (Vector(730.1306516203375,567.1062374700176, 0.0));
    positionAlloc4->Add (Vector(322.7682216078253,835.8044504176676, 0.0));
    positionAlloc4->Add (Vector(256.97972440812464,172.91403842295307, 0.0));
    positionAlloc4->Add (Vector(539.6241287262055,739.9358053052108, 0.0));
    positionAlloc4->Add (Vector(343.1881235698336,276.0349299767498, 0.0));
    positionAlloc4->Add (Vector(401.93554924557134,474.76257920540917, 0.0));
    positionAlloc4->Add (Vector(527.5621911768194,416.0741991010628, 0.0));
    positionAlloc4->Add (Vector(500.4939133457197,418.6007446502287, 0.0));
    positionAlloc4->Add (Vector(837.0862165420544,587.3599454509238, 0.0));
    positionAlloc4->Add (Vector(397.8914454433867,170.2382762865901, 0.0));
    positionAlloc4->Add (Vector(471.7281702415228,350.90012535527893, 0.0));
    positionAlloc4->Add (Vector(567.9434146600212,389.30576035568583, 0.0));
    positionAlloc4->Add (Vector(796.768436877451,262.7769336300832, 0.0));
    positionAlloc4->Add (Vector(225.70471860928384,609.2102057140544, 0.0));
    positionAlloc4->Add (Vector(612.9437444183438,575.6493133893571, 0.0));
    positionAlloc4->Add (Vector(553.9943844084803,335.4865410418181, 0.0));
    positionAlloc4->Add (Vector(246.5657073278499,305.67032333782186, 0.0));
    positionAlloc4->Add (Vector(175.7097322341409,709.2737676798798, 0.0));
    positionAlloc4->Add (Vector(704.7414091590499,675.6144435601428, 0.0));
    positionAlloc4->Add (Vector(224.11531503166157,776.7063699094372, 0.0));
    positionAlloc4->Add (Vector(796.6926369348871,594.8381108825417, 0.0));
    positionAlloc4->Add (Vector(300.795332525097,730.2038012172862, 0.0));
    positionAlloc4->Add (Vector(504.4776932567214,686.4835164979096, 0.0));
    positionAlloc4->Add (Vector(219.48210451643644,746.9642777636313, 0.0));
    positionAlloc4->Add (Vector(806.0326842130982,225.50503691096975, 0.0));
    positionAlloc4->Add (Vector(450.4204194313434,601.1346181859185, 0.0));
    positionAlloc4->Add (Vector(388.1196652719427,477.36068693162866, 0.0));
    positionAlloc4->Add (Vector(266.7132734637403,462.0752474501754, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_22.xml");
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
	  flowmon->SerializeToXmlFile ("switch_ECC_flowmon/switch_ECC_22.flowmon", false, false);
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