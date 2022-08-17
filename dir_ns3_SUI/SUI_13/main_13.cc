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
	  uint16_t numberOfRrhs = 42;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 695;
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
    positionAlloc->Add (Vector (772.2222222222222,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,150.0, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (150.0,616.6666666666667, 0.0));
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
    positionAlloc4->Add (Vector(321.2313918365862,672.2916847670496, 0.0));
    positionAlloc4->Add (Vector(646.8632992906603,815.4144091531568, 0.0));
    positionAlloc4->Add (Vector(441.0502434484281,570.7578657752509, 0.0));
    positionAlloc4->Add (Vector(425.06553549394715,823.2337348418282, 0.0));
    positionAlloc4->Add (Vector(610.2110290242254,776.5467060553499, 0.0));
    positionAlloc4->Add (Vector(772.3913749934009,477.4944283044301, 0.0));
    positionAlloc4->Add (Vector(806.435191522757,753.5070708051852, 0.0));
    positionAlloc4->Add (Vector(571.9652725867315,840.1941005194616, 0.0));
    positionAlloc4->Add (Vector(378.12310037554994,482.958521647442, 0.0));
    positionAlloc4->Add (Vector(528.0204345824814,472.72349766846844, 0.0));
    positionAlloc4->Add (Vector(810.3088780719568,519.440546348214, 0.0));
    positionAlloc4->Add (Vector(305.85446465874975,652.1824887836472, 0.0));
    positionAlloc4->Add (Vector(251.29744529556393,717.8613908809505, 0.0));
    positionAlloc4->Add (Vector(244.79319487247963,353.90562304385367, 0.0));
    positionAlloc4->Add (Vector(377.0195647129064,651.3151767243278, 0.0));
    positionAlloc4->Add (Vector(553.4838860175433,704.4021882911371, 0.0));
    positionAlloc4->Add (Vector(805.5959281606043,414.7374446697533, 0.0));
    positionAlloc4->Add (Vector(352.25591166030785,210.37441991277544, 0.0));
    positionAlloc4->Add (Vector(509.25398307947313,760.4742446226068, 0.0));
    positionAlloc4->Add (Vector(491.51362419864165,641.5150300538187, 0.0));
    positionAlloc4->Add (Vector(453.0811348877944,199.71719735452865, 0.0));
    positionAlloc4->Add (Vector(308.1169890535031,578.5110612602604, 0.0));
    positionAlloc4->Add (Vector(429.32831643381286,835.9401029476662, 0.0));
    positionAlloc4->Add (Vector(380.5474669749683,246.76246074973602, 0.0));
    positionAlloc4->Add (Vector(381.5550939993589,678.4581401108303, 0.0));
    positionAlloc4->Add (Vector(386.57898546833513,478.89180448857246, 0.0));
    positionAlloc4->Add (Vector(655.8282073841146,293.1530370849571, 0.0));
    positionAlloc4->Add (Vector(698.9310254624558,684.2996331422114, 0.0));
    positionAlloc4->Add (Vector(408.9992201719842,668.5459070124707, 0.0));
    positionAlloc4->Add (Vector(446.007886341361,512.005253598714, 0.0));
    positionAlloc4->Add (Vector(770.6127173182625,676.1758369219963, 0.0));
    positionAlloc4->Add (Vector(171.3475834969281,242.2672614482214, 0.0));
    positionAlloc4->Add (Vector(327.37867529156085,503.51963230413685, 0.0));
    positionAlloc4->Add (Vector(503.06649300913676,475.03222135660695, 0.0));
    positionAlloc4->Add (Vector(692.4184025770642,424.9145799569593, 0.0));
    positionAlloc4->Add (Vector(795.7492274117523,746.8663071712044, 0.0));
    positionAlloc4->Add (Vector(304.07688101341444,290.8210769797305, 0.0));
    positionAlloc4->Add (Vector(751.3912518258633,309.8489299251934, 0.0));
    positionAlloc4->Add (Vector(552.8846554817723,815.415776151787, 0.0));
    positionAlloc4->Add (Vector(178.54950940486268,370.5057926328418, 0.0));
    positionAlloc4->Add (Vector(723.3751283243814,532.9893497982305, 0.0));
    positionAlloc4->Add (Vector(817.1872287555756,511.7152314580451, 0.0));
    positionAlloc4->Add (Vector(428.0592706963628,549.924010803172, 0.0));
    positionAlloc4->Add (Vector(469.2890462626433,171.01415800213542, 0.0));
    positionAlloc4->Add (Vector(833.249410222333,329.7821021969411, 0.0));
    positionAlloc4->Add (Vector(251.70020682205066,371.4046441079371, 0.0));
    positionAlloc4->Add (Vector(665.0025907446769,298.20174492337594, 0.0));
    positionAlloc4->Add (Vector(225.2172515093206,742.2834298885782, 0.0));
    positionAlloc4->Add (Vector(467.98950705229765,653.6908892858044, 0.0));
    positionAlloc4->Add (Vector(404.7295845467327,733.2920135201304, 0.0));
    positionAlloc4->Add (Vector(393.76144779515806,647.1468938213854, 0.0));
    positionAlloc4->Add (Vector(847.8933519687857,447.8891143877023, 0.0));
    positionAlloc4->Add (Vector(545.0107452688811,230.17300782844376, 0.0));
    positionAlloc4->Add (Vector(512.3472549826042,417.37061333168816, 0.0));
    positionAlloc4->Add (Vector(489.49459920424965,363.903521529478, 0.0));
    positionAlloc4->Add (Vector(340.7904241707804,760.0819764956885, 0.0));
    positionAlloc4->Add (Vector(177.2115879116825,611.2599557543199, 0.0));
    positionAlloc4->Add (Vector(222.64229604431847,719.0498837757559, 0.0));
    positionAlloc4->Add (Vector(327.4199907711114,201.97280153138203, 0.0));
    positionAlloc4->Add (Vector(208.72857244023652,186.593460590544, 0.0));
    positionAlloc4->Add (Vector(350.6624069009161,406.87048227189666, 0.0));
    positionAlloc4->Add (Vector(498.69077192878626,555.6754584049868, 0.0));
    positionAlloc4->Add (Vector(360.81131384981177,554.7608503186595, 0.0));
    positionAlloc4->Add (Vector(591.0242042797067,388.4031561947831, 0.0));
    positionAlloc4->Add (Vector(657.437190285259,455.23380858081146, 0.0));
    positionAlloc4->Add (Vector(789.5046079818865,517.7523372736487, 0.0));
    positionAlloc4->Add (Vector(225.54718847549572,802.3608977580604, 0.0));
    positionAlloc4->Add (Vector(265.7592978592302,834.3989689778499, 0.0));
    positionAlloc4->Add (Vector(357.24585702726296,510.45064677473795, 0.0));
    positionAlloc4->Add (Vector(388.2693863742844,483.597560506013, 0.0));
    positionAlloc4->Add (Vector(326.5273872734886,292.6021752943666, 0.0));
    positionAlloc4->Add (Vector(722.5893397540437,561.3861286719185, 0.0));
    positionAlloc4->Add (Vector(413.2292537764095,809.8267971110589, 0.0));
    positionAlloc4->Add (Vector(157.97276766198695,637.5550284338224, 0.0));
    positionAlloc4->Add (Vector(596.2117651025314,537.3993905509476, 0.0));
    positionAlloc4->Add (Vector(678.4107702359925,604.0865297731248, 0.0));
    positionAlloc4->Add (Vector(600.6394256708713,382.9728527641704, 0.0));
    positionAlloc4->Add (Vector(380.1221918447246,654.3035915580446, 0.0));
    positionAlloc4->Add (Vector(450.72192041398273,538.0258186585588, 0.0));
    positionAlloc4->Add (Vector(342.4854343815481,205.7515903243361, 0.0));
    positionAlloc4->Add (Vector(231.51186685929798,567.5322042807667, 0.0));
    positionAlloc4->Add (Vector(549.9376699171539,196.72517839552984, 0.0));
    positionAlloc4->Add (Vector(564.340083093349,665.395929148395, 0.0));
    positionAlloc4->Add (Vector(539.768500198518,613.0670269023224, 0.0));
    positionAlloc4->Add (Vector(465.3755773469532,158.15142965216734, 0.0));
    positionAlloc4->Add (Vector(335.217978316721,513.6512983354868, 0.0));
    positionAlloc4->Add (Vector(576.4367739753284,747.2210850527057, 0.0));
    positionAlloc4->Add (Vector(601.4344267054419,746.0391145197957, 0.0));
    positionAlloc4->Add (Vector(387.4424018903426,375.8785949544741, 0.0));
    positionAlloc4->Add (Vector(153.72693339974498,545.3734140204773, 0.0));
    positionAlloc4->Add (Vector(342.18120931831123,726.866143339062, 0.0));
    positionAlloc4->Add (Vector(322.0252185301215,553.8714050846854, 0.0));
    positionAlloc4->Add (Vector(697.0661906731469,801.5653706413333, 0.0));
    positionAlloc4->Add (Vector(362.41725151472434,754.7550652219684, 0.0));
    positionAlloc4->Add (Vector(754.5039021974021,800.4826826385245, 0.0));
    positionAlloc4->Add (Vector(473.6530130734174,423.0462460233398, 0.0));
    positionAlloc4->Add (Vector(422.4864777663338,369.35858450125056, 0.0));
    positionAlloc4->Add (Vector(211.60421925801657,775.4742084489059, 0.0));
    positionAlloc4->Add (Vector(576.6117081290952,600.4864860911923, 0.0));
    positionAlloc4->Add (Vector(269.92718111806244,617.3211995082645, 0.0));
    positionAlloc4->Add (Vector(425.6034187571263,309.1952101064705, 0.0));
    positionAlloc4->Add (Vector(236.4245016115152,836.0363600835393, 0.0));
    positionAlloc4->Add (Vector(565.52738111249,264.92947483945113, 0.0));
    positionAlloc4->Add (Vector(260.25038365306267,566.0322137486171, 0.0));
    positionAlloc4->Add (Vector(580.6504276178398,729.6303000419765, 0.0));
    positionAlloc4->Add (Vector(518.9496569594208,176.74194863017374, 0.0));
    positionAlloc4->Add (Vector(460.64850549763497,549.5971178810189, 0.0));
    positionAlloc4->Add (Vector(509.8564869334912,730.4272459649424, 0.0));
    positionAlloc4->Add (Vector(208.66291136654255,488.02610561901906, 0.0));
    positionAlloc4->Add (Vector(760.723970558572,300.9478339080944, 0.0));
    positionAlloc4->Add (Vector(662.6033971979873,693.8624718747648, 0.0));
    positionAlloc4->Add (Vector(537.6097619788529,840.4898129380223, 0.0));
    positionAlloc4->Add (Vector(596.6022646513736,590.1049709179074, 0.0));
    positionAlloc4->Add (Vector(574.7037058475623,588.7486719362912, 0.0));
    positionAlloc4->Add (Vector(696.3652619299153,678.8060907609068, 0.0));
    positionAlloc4->Add (Vector(405.55857129908054,788.8089525473761, 0.0));
    positionAlloc4->Add (Vector(202.5655520637198,810.3501090969171, 0.0));
    positionAlloc4->Add (Vector(375.45640272138604,555.0097440150648, 0.0));
    positionAlloc4->Add (Vector(156.79024844612957,248.03136849396031, 0.0));
    positionAlloc4->Add (Vector(836.3295584481498,495.69291031909296, 0.0));
    positionAlloc4->Add (Vector(340.7270288231672,732.2497101296115, 0.0));
    positionAlloc4->Add (Vector(367.6811149681233,219.7647454537431, 0.0));
    positionAlloc4->Add (Vector(657.4095975059163,638.6818468584665, 0.0));
    positionAlloc4->Add (Vector(717.3030395012194,632.791074403308, 0.0));
    positionAlloc4->Add (Vector(173.92291232876119,373.75644357871045, 0.0));
    positionAlloc4->Add (Vector(559.5851626489873,460.0398686293745, 0.0));
    positionAlloc4->Add (Vector(411.49640022735645,300.0100506260151, 0.0));
    positionAlloc4->Add (Vector(216.41896110330225,383.1053431365709, 0.0));
    positionAlloc4->Add (Vector(797.2557737836265,228.75289326666405, 0.0));
    positionAlloc4->Add (Vector(620.0582650967262,153.0517059696063, 0.0));
    positionAlloc4->Add (Vector(791.968953620256,243.78240672484685, 0.0));
    positionAlloc4->Add (Vector(604.3564214132191,395.16563002895373, 0.0));
    positionAlloc4->Add (Vector(442.5994518442461,545.8902382584708, 0.0));
    positionAlloc4->Add (Vector(158.9295357952014,636.660759023415, 0.0));
    positionAlloc4->Add (Vector(752.7061749156095,580.9370776781886, 0.0));
    positionAlloc4->Add (Vector(504.01960884881106,721.9714931018304, 0.0));
    positionAlloc4->Add (Vector(552.0054777960015,585.4206354764065, 0.0));
    positionAlloc4->Add (Vector(299.9902665171817,195.1434763190924, 0.0));
    positionAlloc4->Add (Vector(487.28607750338074,237.04172220968752, 0.0));
    positionAlloc4->Add (Vector(170.29772103516362,632.8747499301334, 0.0));
    positionAlloc4->Add (Vector(794.0796693258401,479.20735588410236, 0.0));
    positionAlloc4->Add (Vector(457.3204707863136,181.4167115309026, 0.0));
    positionAlloc4->Add (Vector(803.1792548180571,610.0383631480436, 0.0));
    positionAlloc4->Add (Vector(163.33038649200427,662.7182403936108, 0.0));
    positionAlloc4->Add (Vector(319.34940968346564,763.4046439667562, 0.0));
    positionAlloc4->Add (Vector(551.4497094546512,747.54094996957, 0.0));
    positionAlloc4->Add (Vector(794.1378902519917,184.41096653499932, 0.0));
    positionAlloc4->Add (Vector(502.1478258424197,558.9573521822026, 0.0));
    positionAlloc4->Add (Vector(810.4956520098839,179.31627568714285, 0.0));
    positionAlloc4->Add (Vector(654.2912483646401,508.0753359643543, 0.0));
    positionAlloc4->Add (Vector(390.13080405510715,437.49028041058017, 0.0));
    positionAlloc4->Add (Vector(424.0179895561298,333.47908041175754, 0.0));
    positionAlloc4->Add (Vector(605.2078818514553,380.304010763563, 0.0));
    positionAlloc4->Add (Vector(764.9715244450091,361.2842626250266, 0.0));
    positionAlloc4->Add (Vector(239.0277175427576,200.64386868524014, 0.0));
    positionAlloc4->Add (Vector(208.71161698640844,844.96278473021, 0.0));
    positionAlloc4->Add (Vector(553.937339199481,389.58591495282235, 0.0));
    positionAlloc4->Add (Vector(560.3276478618391,400.0693777190953, 0.0));
    positionAlloc4->Add (Vector(608.234835460824,646.5200063624384, 0.0));
    positionAlloc4->Add (Vector(742.0214148280343,392.38623555155135, 0.0));
    positionAlloc4->Add (Vector(292.1494150440435,431.3663472306537, 0.0));
    positionAlloc4->Add (Vector(178.3564210589253,236.0378277795715, 0.0));
    positionAlloc4->Add (Vector(440.39332459238204,687.5026461880988, 0.0));
    positionAlloc4->Add (Vector(630.6552963478262,738.9497399136391, 0.0));
    positionAlloc4->Add (Vector(486.5503177258091,844.411178577785, 0.0));
    positionAlloc4->Add (Vector(684.5307092731597,330.78505052101445, 0.0));
    positionAlloc4->Add (Vector(238.58171783746405,483.63431941737826, 0.0));
    positionAlloc4->Add (Vector(245.87664418096512,504.7390285024517, 0.0));
    positionAlloc4->Add (Vector(456.0971537712323,432.48403456958187, 0.0));
    positionAlloc4->Add (Vector(248.45733746356672,651.6423556914219, 0.0));
    positionAlloc4->Add (Vector(777.068470979005,190.4517379139049, 0.0));
    positionAlloc4->Add (Vector(211.4410398864084,617.7264633705336, 0.0));
    positionAlloc4->Add (Vector(497.07363085397736,662.5665398646013, 0.0));
    positionAlloc4->Add (Vector(546.5105795606386,480.229416573612, 0.0));
    positionAlloc4->Add (Vector(366.20262946552424,611.1987829587895, 0.0));
    positionAlloc4->Add (Vector(250.15697679211974,750.5224296723679, 0.0));
    positionAlloc4->Add (Vector(691.0117554672971,387.278948320975, 0.0));
    positionAlloc4->Add (Vector(831.5844943951391,311.44704814998477, 0.0));
    positionAlloc4->Add (Vector(813.4129628044137,238.4590993629036, 0.0));
    positionAlloc4->Add (Vector(539.365475818041,437.98765223232584, 0.0));
    positionAlloc4->Add (Vector(328.30435032028424,334.27232238138106, 0.0));
    positionAlloc4->Add (Vector(428.9914569008893,269.10684880116185, 0.0));
    positionAlloc4->Add (Vector(704.372710126701,239.02683245182084, 0.0));
    positionAlloc4->Add (Vector(265.07487902988987,712.8764722427921, 0.0));
    positionAlloc4->Add (Vector(516.2107048723492,779.0472498107723, 0.0));
    positionAlloc4->Add (Vector(163.7273727319775,619.957606227757, 0.0));
    positionAlloc4->Add (Vector(296.92498961349804,494.31317162257665, 0.0));
    positionAlloc4->Add (Vector(311.3463861460646,379.2964057454882, 0.0));
    positionAlloc4->Add (Vector(820.4847131139521,748.2322242062847, 0.0));
    positionAlloc4->Add (Vector(326.0011065312244,271.5005009060078, 0.0));
    positionAlloc4->Add (Vector(576.7957368757318,287.53785668204307, 0.0));
    positionAlloc4->Add (Vector(236.50074341964336,220.71977254388096, 0.0));
    positionAlloc4->Add (Vector(254.6845229898612,222.6875702776493, 0.0));
    positionAlloc4->Add (Vector(511.5825572724836,503.31763276624616, 0.0));
    positionAlloc4->Add (Vector(452.2905846184208,187.75280798860985, 0.0));
    positionAlloc4->Add (Vector(569.5544862167859,443.1312933378585, 0.0));
    positionAlloc4->Add (Vector(212.87802680045883,563.5276049471454, 0.0));
    positionAlloc4->Add (Vector(423.76457090375436,205.98773795772837, 0.0));
    positionAlloc4->Add (Vector(420.10161278527636,837.4541650449415, 0.0));
    positionAlloc4->Add (Vector(326.44892079512005,467.91722911743466, 0.0));
    positionAlloc4->Add (Vector(273.26250156069773,670.3330394959597, 0.0));
    positionAlloc4->Add (Vector(399.17787358256555,348.8729044592031, 0.0));
    positionAlloc4->Add (Vector(432.6677036732495,300.32115168585017, 0.0));
    positionAlloc4->Add (Vector(756.0090659747556,657.0956381885205, 0.0));
    positionAlloc4->Add (Vector(304.0463783492843,421.9718036042294, 0.0));
    positionAlloc4->Add (Vector(369.27551834217536,677.5662965367069, 0.0));
    positionAlloc4->Add (Vector(768.3447907154728,497.00190705479287, 0.0));
    positionAlloc4->Add (Vector(517.8023988167496,634.0538820695161, 0.0));
    positionAlloc4->Add (Vector(772.38354900921,741.6578907125179, 0.0));
    positionAlloc4->Add (Vector(614.8880174983453,448.61433073807535, 0.0));
    positionAlloc4->Add (Vector(714.1790900696677,352.64850558380374, 0.0));
    positionAlloc4->Add (Vector(197.45747707967882,656.5064903079866, 0.0));
    positionAlloc4->Add (Vector(670.8346158600044,502.2534450280735, 0.0));
    positionAlloc4->Add (Vector(333.8435286562516,503.09472780064914, 0.0));
    positionAlloc4->Add (Vector(376.0851133546656,316.0780545599961, 0.0));
    positionAlloc4->Add (Vector(799.5304409034189,729.8647019211523, 0.0));
    positionAlloc4->Add (Vector(479.8207915646993,551.3811381283926, 0.0));
    positionAlloc4->Add (Vector(824.0093458012668,562.6295621652728, 0.0));
    positionAlloc4->Add (Vector(513.2561549853299,574.0316425512804, 0.0));
    positionAlloc4->Add (Vector(334.6642524624503,158.44895471043256, 0.0));
    positionAlloc4->Add (Vector(449.6135467077507,610.7986483983102, 0.0));
    positionAlloc4->Add (Vector(306.0650059904275,281.6409965353529, 0.0));
    positionAlloc4->Add (Vector(370.9575388876268,210.8576924845779, 0.0));
    positionAlloc4->Add (Vector(385.8453391537219,559.840156820525, 0.0));
    positionAlloc4->Add (Vector(273.3360843149733,574.3516495545362, 0.0));
    positionAlloc4->Add (Vector(156.46911188386005,715.7099343169489, 0.0));
    positionAlloc4->Add (Vector(523.6077895700112,250.6841296932176, 0.0));
    positionAlloc4->Add (Vector(250.58133762894892,646.9531514039725, 0.0));
    positionAlloc4->Add (Vector(192.0512178484877,377.8523366882948, 0.0));
    positionAlloc4->Add (Vector(498.3546881536527,299.9233769177108, 0.0));
    positionAlloc4->Add (Vector(813.0194317903758,245.15292315712446, 0.0));
    positionAlloc4->Add (Vector(326.55619396143317,717.5113256612889, 0.0));
    positionAlloc4->Add (Vector(644.921818526019,274.2372844937082, 0.0));
    positionAlloc4->Add (Vector(537.9188051277001,541.1886096682665, 0.0));
    positionAlloc4->Add (Vector(174.92850519098903,498.58341181117527, 0.0));
    positionAlloc4->Add (Vector(453.6065919819947,438.4655267427774, 0.0));
    positionAlloc4->Add (Vector(259.02613600587085,766.2953320115233, 0.0));
    positionAlloc4->Add (Vector(526.0185251228202,292.47486573726337, 0.0));
    positionAlloc4->Add (Vector(410.02433753413004,464.23820581495306, 0.0));
    positionAlloc4->Add (Vector(309.44483913246097,558.7007871156108, 0.0));
    positionAlloc4->Add (Vector(287.86754114610494,438.93082419417544, 0.0));
    positionAlloc4->Add (Vector(653.022618483137,299.61568158733746, 0.0));
    positionAlloc4->Add (Vector(458.0845396894553,213.79613736279506, 0.0));
    positionAlloc4->Add (Vector(525.334595497543,464.4037419125488, 0.0));
    positionAlloc4->Add (Vector(342.8126758533564,335.52444269838327, 0.0));
    positionAlloc4->Add (Vector(729.0107038993364,447.50168988623824, 0.0));
    positionAlloc4->Add (Vector(574.5585577262532,572.9816611908476, 0.0));
    positionAlloc4->Add (Vector(710.2549319187417,469.91676653483785, 0.0));
    positionAlloc4->Add (Vector(352.4352063761088,716.284424729596, 0.0));
    positionAlloc4->Add (Vector(174.3594399078451,436.47470164977807, 0.0));
    positionAlloc4->Add (Vector(621.9537427131779,305.04548420174797, 0.0));
    positionAlloc4->Add (Vector(348.4228044848554,630.7290041973363, 0.0));
    positionAlloc4->Add (Vector(343.917814208737,753.6344415078121, 0.0));
    positionAlloc4->Add (Vector(164.28112748174445,573.1422875809217, 0.0));
    positionAlloc4->Add (Vector(501.7833247054162,743.8924085013577, 0.0));
    positionAlloc4->Add (Vector(847.2089483675738,275.9997316274808, 0.0));
    positionAlloc4->Add (Vector(160.49895026399568,262.77001627949556, 0.0));
    positionAlloc4->Add (Vector(820.8293805369707,218.39509277073094, 0.0));
    positionAlloc4->Add (Vector(313.13723158940104,815.2658260374652, 0.0));
    positionAlloc4->Add (Vector(845.2146172534349,224.26443496356643, 0.0));
    positionAlloc4->Add (Vector(442.6921198555359,787.710989985454, 0.0));
    positionAlloc4->Add (Vector(598.1756404665934,764.2424692120416, 0.0));
    positionAlloc4->Add (Vector(631.2253628348096,324.90722568055054, 0.0));
    positionAlloc4->Add (Vector(466.0829159721971,435.61406389955675, 0.0));
    positionAlloc4->Add (Vector(752.8625590822701,258.27939372940284, 0.0));
    positionAlloc4->Add (Vector(442.85412690015517,649.9072024745218, 0.0));
    positionAlloc4->Add (Vector(514.3643735490601,608.7682396244172, 0.0));
    positionAlloc4->Add (Vector(520.7511689245935,393.4390683231064, 0.0));
    positionAlloc4->Add (Vector(668.219121590766,529.9889620338522, 0.0));
    positionAlloc4->Add (Vector(432.02302173503404,785.0702170939551, 0.0));
    positionAlloc4->Add (Vector(743.1485377365568,156.67726781788147, 0.0));
    positionAlloc4->Add (Vector(509.1540528576349,619.5230403302414, 0.0));
    positionAlloc4->Add (Vector(761.502714031814,683.9593370130638, 0.0));
    positionAlloc4->Add (Vector(474.4834874812453,479.2268335560445, 0.0));
    positionAlloc4->Add (Vector(769.510727183081,305.06900633165696, 0.0));
    positionAlloc4->Add (Vector(338.98154579882345,687.8947497820694, 0.0));
    positionAlloc4->Add (Vector(391.1541253758443,193.0801238943858, 0.0));
    positionAlloc4->Add (Vector(624.3278274697159,270.9740360195166, 0.0));
    positionAlloc4->Add (Vector(752.7440555358036,198.90334447926318, 0.0));
    positionAlloc4->Add (Vector(350.80681248795577,338.0057230330766, 0.0));
    positionAlloc4->Add (Vector(425.2193041949849,421.4350299708248, 0.0));
    positionAlloc4->Add (Vector(463.7126113583014,342.0888421449772, 0.0));
    positionAlloc4->Add (Vector(793.3267939200698,779.9366803482702, 0.0));
    positionAlloc4->Add (Vector(489.6965019771388,529.4233415680415, 0.0));
    positionAlloc4->Add (Vector(609.3022642932095,767.298360574478, 0.0));
    positionAlloc4->Add (Vector(269.3712725756427,778.1933350547216, 0.0));
    positionAlloc4->Add (Vector(690.4222501806483,729.748930520616, 0.0));
    positionAlloc4->Add (Vector(196.9656128409151,565.5940480660042, 0.0));
    positionAlloc4->Add (Vector(433.93357317880964,625.6862856008788, 0.0));
    positionAlloc4->Add (Vector(603.7464242139891,439.2361351619584, 0.0));
    positionAlloc4->Add (Vector(790.2091046552234,213.72542046180922, 0.0));
    positionAlloc4->Add (Vector(421.6850197379479,216.98308124060082, 0.0));
    positionAlloc4->Add (Vector(712.1318672731159,411.5552074610889, 0.0));
    positionAlloc4->Add (Vector(428.6383884752564,509.5433687500557, 0.0));
    positionAlloc4->Add (Vector(712.982808064915,611.408881220094, 0.0));
    positionAlloc4->Add (Vector(560.0964857032178,621.9344816305006, 0.0));
    positionAlloc4->Add (Vector(479.5336746713221,764.680637889991, 0.0));
    positionAlloc4->Add (Vector(245.37539556438452,771.2941502713339, 0.0));
    positionAlloc4->Add (Vector(528.9347678908871,376.7268868062654, 0.0));
    positionAlloc4->Add (Vector(539.9333103345639,701.1441938867949, 0.0));
    positionAlloc4->Add (Vector(786.6887999060417,162.622406877661, 0.0));
    positionAlloc4->Add (Vector(626.0322394619889,594.6098922047545, 0.0));
    positionAlloc4->Add (Vector(460.8791036227845,696.9493528766402, 0.0));
    positionAlloc4->Add (Vector(526.1918570198649,165.75302920008053, 0.0));
    positionAlloc4->Add (Vector(259.7340633195063,448.48423954438096, 0.0));
    positionAlloc4->Add (Vector(466.62643434400724,447.0723126507573, 0.0));
    positionAlloc4->Add (Vector(470.12162692497424,742.3357063080614, 0.0));
    positionAlloc4->Add (Vector(819.1431599734309,796.3096557889506, 0.0));
    positionAlloc4->Add (Vector(373.5218160149618,326.5416815115316, 0.0));
    positionAlloc4->Add (Vector(648.8209851088752,631.6213632663869, 0.0));
    positionAlloc4->Add (Vector(279.86818268040344,606.1758564148988, 0.0));
    positionAlloc4->Add (Vector(791.1601027678366,351.55386586525333, 0.0));
    positionAlloc4->Add (Vector(150.28720260742242,567.3278685816638, 0.0));
    positionAlloc4->Add (Vector(257.35537919102194,663.3295361018036, 0.0));
    positionAlloc4->Add (Vector(346.444905913561,830.5607964514503, 0.0));
    positionAlloc4->Add (Vector(447.1449828735746,366.52695705075604, 0.0));
    positionAlloc4->Add (Vector(269.0999181240105,371.273421558627, 0.0));
    positionAlloc4->Add (Vector(846.0015900327413,538.932008277682, 0.0));
    positionAlloc4->Add (Vector(280.08243889573976,834.5953966167642, 0.0));
    positionAlloc4->Add (Vector(478.05614589746966,807.0047677102232, 0.0));
    positionAlloc4->Add (Vector(717.2123093863328,244.5266956763832, 0.0));
    positionAlloc4->Add (Vector(312.1080939022996,276.85263052953206, 0.0));
    positionAlloc4->Add (Vector(214.038070550753,509.71308434476356, 0.0));
    positionAlloc4->Add (Vector(654.4104935594462,686.034324733278, 0.0));
    positionAlloc4->Add (Vector(799.4540355627208,802.4937407787982, 0.0));
    positionAlloc4->Add (Vector(152.11460994931286,323.552714864065, 0.0));
    positionAlloc4->Add (Vector(607.5595334839343,819.3548215505393, 0.0));
    positionAlloc4->Add (Vector(209.6403342917634,605.064715797952, 0.0));
    positionAlloc4->Add (Vector(761.2780496841708,643.4377929119353, 0.0));
    positionAlloc4->Add (Vector(460.51979092273103,269.7717348893813, 0.0));
    positionAlloc4->Add (Vector(304.09047093826865,347.35581903923094, 0.0));
    positionAlloc4->Add (Vector(356.45179506270506,160.68538775222083, 0.0));
    positionAlloc4->Add (Vector(455.74401852632377,514.1265155967318, 0.0));
    positionAlloc4->Add (Vector(846.9229412624542,326.86410151989764, 0.0));
    positionAlloc4->Add (Vector(339.26826471745943,712.4792437784994, 0.0));
    positionAlloc4->Add (Vector(823.5301366442601,781.0400957705243, 0.0));
    positionAlloc4->Add (Vector(819.7702694136144,380.9824267031935, 0.0));
    positionAlloc4->Add (Vector(599.9833735710166,417.3860318240622, 0.0));
    positionAlloc4->Add (Vector(811.4893498890901,249.12794543252951, 0.0));
    positionAlloc4->Add (Vector(636.3745109102044,459.0831022158813, 0.0));
    positionAlloc4->Add (Vector(621.4617507720814,230.50765577966848, 0.0));
    positionAlloc4->Add (Vector(232.23312921354503,699.9666213611193, 0.0));
    positionAlloc4->Add (Vector(545.5505981256856,751.0704384506473, 0.0));
    positionAlloc4->Add (Vector(275.7615023209384,632.3763570407768, 0.0));
    positionAlloc4->Add (Vector(607.8161543611322,441.3519217226201, 0.0));
    positionAlloc4->Add (Vector(339.040926271089,294.78597743162595, 0.0));
    positionAlloc4->Add (Vector(598.4971573090551,439.7482406368122, 0.0));
    positionAlloc4->Add (Vector(465.06668801360155,165.49411572064795, 0.0));
    positionAlloc4->Add (Vector(823.0497754660031,322.72319446716665, 0.0));
    positionAlloc4->Add (Vector(194.83046670535197,753.7038910988098, 0.0));
    positionAlloc4->Add (Vector(289.3576189795704,561.3922598390336, 0.0));
    positionAlloc4->Add (Vector(366.0428539667655,241.22676269162972, 0.0));
    positionAlloc4->Add (Vector(231.81513241365735,840.1009082876562, 0.0));
    positionAlloc4->Add (Vector(241.23850649835032,366.4876780497867, 0.0));
    positionAlloc4->Add (Vector(309.64327183001393,158.06820983708968, 0.0));
    positionAlloc4->Add (Vector(550.2801398278837,327.5497320389629, 0.0));
    positionAlloc4->Add (Vector(682.4933642307265,254.90395922058616, 0.0));
    positionAlloc4->Add (Vector(692.7866602147462,549.478222550385, 0.0));
    positionAlloc4->Add (Vector(243.4608859469737,183.57041270589136, 0.0));
    positionAlloc4->Add (Vector(395.1906594685293,466.52805755207555, 0.0));
    positionAlloc4->Add (Vector(463.93965960184363,836.4537536948347, 0.0));
    positionAlloc4->Add (Vector(506.80622012249563,415.9719665818268, 0.0));
    positionAlloc4->Add (Vector(152.60376257264144,319.0543587992429, 0.0));
    positionAlloc4->Add (Vector(468.1379130687497,296.5014330710839, 0.0));
    positionAlloc4->Add (Vector(558.3330369919622,526.8689308275032, 0.0));
    positionAlloc4->Add (Vector(499.3253210328569,802.0745344086833, 0.0));
    positionAlloc4->Add (Vector(750.8613658327437,819.7849672801441, 0.0));
    positionAlloc4->Add (Vector(562.8103252535261,464.93894675985376, 0.0));
    positionAlloc4->Add (Vector(701.7118516949945,642.8730525401459, 0.0));
    positionAlloc4->Add (Vector(821.4648047266754,650.963856450618, 0.0));
    positionAlloc4->Add (Vector(574.2105017356103,212.74224131323254, 0.0));
    positionAlloc4->Add (Vector(566.5324414466329,213.13699014017106, 0.0));
    positionAlloc4->Add (Vector(832.8691303146312,383.26921838550936, 0.0));
    positionAlloc4->Add (Vector(330.5576547508304,494.0599580822786, 0.0));
    positionAlloc4->Add (Vector(614.0343766411961,746.7771185991769, 0.0));
    positionAlloc4->Add (Vector(510.44925620247045,269.4742047115636, 0.0));
    positionAlloc4->Add (Vector(552.8507129283413,446.40716342985473, 0.0));
    positionAlloc4->Add (Vector(269.53219841526993,308.7859716278466, 0.0));
    positionAlloc4->Add (Vector(556.5159978581463,421.69315670157454, 0.0));
    positionAlloc4->Add (Vector(427.8225751623277,670.4809887461062, 0.0));
    positionAlloc4->Add (Vector(259.432006874987,455.92360781483956, 0.0));
    positionAlloc4->Add (Vector(582.8424149318203,229.16981759667573, 0.0));
    positionAlloc4->Add (Vector(463.13269866633146,702.9052347248771, 0.0));
    positionAlloc4->Add (Vector(511.9846216793313,684.527335729911, 0.0));
    positionAlloc4->Add (Vector(313.37738862068886,467.35942392291014, 0.0));
    positionAlloc4->Add (Vector(718.8851303630477,242.82279352276774, 0.0));
    positionAlloc4->Add (Vector(186.17997593779228,260.97531094994645, 0.0));
    positionAlloc4->Add (Vector(347.35068622609253,521.6013240019331, 0.0));
    positionAlloc4->Add (Vector(288.119779403242,372.07865151479, 0.0));
    positionAlloc4->Add (Vector(746.7072199493111,533.8806557193382, 0.0));
    positionAlloc4->Add (Vector(731.0228785396523,184.49155750794003, 0.0));
    positionAlloc4->Add (Vector(477.50162384391194,602.6347935602887, 0.0));
    positionAlloc4->Add (Vector(717.8138227153926,172.9778491734681, 0.0));
    positionAlloc4->Add (Vector(827.6507520367749,677.0903351274296, 0.0));
    positionAlloc4->Add (Vector(275.85846944655276,477.4307616703057, 0.0));
    positionAlloc4->Add (Vector(725.1043842325865,538.1423800248195, 0.0));
    positionAlloc4->Add (Vector(473.83349035361067,699.5503030944589, 0.0));
    positionAlloc4->Add (Vector(808.6760514899557,490.4604705776033, 0.0));
    positionAlloc4->Add (Vector(700.0659389710451,473.71977194560355, 0.0));
    positionAlloc4->Add (Vector(712.1529496364215,452.2635705501384, 0.0));
    positionAlloc4->Add (Vector(671.0577430127935,583.6115516727073, 0.0));
    positionAlloc4->Add (Vector(754.96142158309,475.2079351074722, 0.0));
    positionAlloc4->Add (Vector(238.1584487332504,782.1682549345168, 0.0));
    positionAlloc4->Add (Vector(195.50055226422194,727.7158816960876, 0.0));
    positionAlloc4->Add (Vector(294.2516459560227,458.86076836420176, 0.0));
    positionAlloc4->Add (Vector(241.65187648874422,637.3550052848907, 0.0));
    positionAlloc4->Add (Vector(750.4358838607095,496.4879562987227, 0.0));
    positionAlloc4->Add (Vector(756.8587299813586,672.7203154587119, 0.0));
    positionAlloc4->Add (Vector(427.02138089314803,359.0705101712848, 0.0));
    positionAlloc4->Add (Vector(170.41946800185212,573.5711355660055, 0.0));
    positionAlloc4->Add (Vector(483.7277128558297,339.47222417570254, 0.0));
    positionAlloc4->Add (Vector(442.91464802282064,505.97722404839556, 0.0));
    positionAlloc4->Add (Vector(275.04021372800963,386.42359771439305, 0.0));
    positionAlloc4->Add (Vector(178.2311127360591,290.7955798132641, 0.0));
    positionAlloc4->Add (Vector(646.0737039758549,300.39822449749863, 0.0));
    positionAlloc4->Add (Vector(701.7926457195807,331.324600872343, 0.0));
    positionAlloc4->Add (Vector(169.83621526836924,729.971181707056, 0.0));
    positionAlloc4->Add (Vector(582.3862103320137,601.0962187408716, 0.0));
    positionAlloc4->Add (Vector(689.926200251795,739.0078319693713, 0.0));
    positionAlloc4->Add (Vector(199.64133400699868,282.55261305430395, 0.0));
    positionAlloc4->Add (Vector(828.6075629731148,635.2466795532075, 0.0));
    positionAlloc4->Add (Vector(202.15732327304806,507.2749049440946, 0.0));
    positionAlloc4->Add (Vector(458.89203572504806,162.57050462510568, 0.0));
    positionAlloc4->Add (Vector(561.4567835464605,743.1077218748458, 0.0));
    positionAlloc4->Add (Vector(761.4889033589621,380.5646469239524, 0.0));
    positionAlloc4->Add (Vector(384.89004118766,833.214110267146, 0.0));
    positionAlloc4->Add (Vector(428.6610247653616,168.51283181836723, 0.0));
    positionAlloc4->Add (Vector(506.814857002756,445.71205302161536, 0.0));
    positionAlloc4->Add (Vector(514.4587476138781,669.5407063847928, 0.0));
    positionAlloc4->Add (Vector(198.75210742378596,562.2552157937835, 0.0));
    positionAlloc4->Add (Vector(389.73265343041294,225.5737136775849, 0.0));
    positionAlloc4->Add (Vector(578.0463969034394,637.2219848923796, 0.0));
    positionAlloc4->Add (Vector(609.9831873428914,405.5657441622641, 0.0));
    positionAlloc4->Add (Vector(721.814590125648,159.76431308433803, 0.0));
    positionAlloc4->Add (Vector(680.8825711239423,165.39056542752652, 0.0));
    positionAlloc4->Add (Vector(209.91968697593782,278.4205193312055, 0.0));
    positionAlloc4->Add (Vector(579.595578680623,705.4761815601817, 0.0));
    positionAlloc4->Add (Vector(326.2702914063922,836.7206423805533, 0.0));
    positionAlloc4->Add (Vector(438.9875784312345,751.8981432345852, 0.0));
    positionAlloc4->Add (Vector(157.46908890431394,585.9188934642343, 0.0));
    positionAlloc4->Add (Vector(795.0204153515713,834.9541768528829, 0.0));
    positionAlloc4->Add (Vector(789.2441732699207,708.4616605460254, 0.0));
    positionAlloc4->Add (Vector(301.29506861764446,563.6103460819809, 0.0));
    positionAlloc4->Add (Vector(383.0651728736613,618.5884449775504, 0.0));
    positionAlloc4->Add (Vector(167.09881805844324,438.22336731317023, 0.0));
    positionAlloc4->Add (Vector(647.1212866131125,624.1392922570615, 0.0));
    positionAlloc4->Add (Vector(590.3823050439867,256.8607151785486, 0.0));
    positionAlloc4->Add (Vector(641.9168095027107,704.892384046526, 0.0));
    positionAlloc4->Add (Vector(217.8421289468964,533.6150480603945, 0.0));
    positionAlloc4->Add (Vector(707.8632528109521,563.298272175524, 0.0));
    positionAlloc4->Add (Vector(292.09549143383805,466.3083312565945, 0.0));
    positionAlloc4->Add (Vector(562.5321514634097,517.5985962387408, 0.0));
    positionAlloc4->Add (Vector(401.6802011640024,238.0616354422359, 0.0));
    positionAlloc4->Add (Vector(394.926824553179,535.9221734639699, 0.0));
    positionAlloc4->Add (Vector(218.33313237460425,778.4570644744182, 0.0));
    positionAlloc4->Add (Vector(759.89389168481,284.8071427209695, 0.0));
    positionAlloc4->Add (Vector(356.57620212899974,663.381294534434, 0.0));
    positionAlloc4->Add (Vector(543.9811660502221,369.3573861200947, 0.0));
    positionAlloc4->Add (Vector(415.62296641542537,504.58499993598303, 0.0));
    positionAlloc4->Add (Vector(161.08664966141194,540.4670880148042, 0.0));
    positionAlloc4->Add (Vector(271.80731361128693,682.5315158708651, 0.0));
    positionAlloc4->Add (Vector(363.78944897992596,260.3627187578356, 0.0));
    positionAlloc4->Add (Vector(238.92350318126222,190.9756200560351, 0.0));
    positionAlloc4->Add (Vector(614.851884839884,328.5610556949787, 0.0));
    positionAlloc4->Add (Vector(805.8644777360868,414.0763900009868, 0.0));
    positionAlloc4->Add (Vector(283.90240968756007,435.18369012794415, 0.0));
    positionAlloc4->Add (Vector(554.039822495793,196.1245155223809, 0.0));
    positionAlloc4->Add (Vector(624.1127715086986,352.707893269509, 0.0));
    positionAlloc4->Add (Vector(414.4577767365881,371.6790492118482, 0.0));
    positionAlloc4->Add (Vector(384.50599897038705,295.0977657848149, 0.0));
    positionAlloc4->Add (Vector(761.3740205364536,426.4931893843227, 0.0));
    positionAlloc4->Add (Vector(514.112156117077,405.3726674154998, 0.0));
    positionAlloc4->Add (Vector(318.69738059714257,805.2879000780943, 0.0));
    positionAlloc4->Add (Vector(352.16697896786275,254.4876088684731, 0.0));
    positionAlloc4->Add (Vector(198.2930060973216,847.0616574459431, 0.0));
    positionAlloc4->Add (Vector(154.75172181651286,257.0602305327983, 0.0));
    positionAlloc4->Add (Vector(178.2276678046655,216.33792972421577, 0.0));
    positionAlloc4->Add (Vector(315.20555122659846,661.3602578184286, 0.0));
    positionAlloc4->Add (Vector(596.6649080575166,351.70649504418213, 0.0));
    positionAlloc4->Add (Vector(490.47413966222564,358.9154628034324, 0.0));
    positionAlloc4->Add (Vector(721.2234516114479,711.7925237061395, 0.0));
    positionAlloc4->Add (Vector(455.3500919840318,286.35566185074873, 0.0));
    positionAlloc4->Add (Vector(186.58806729311758,629.8770343938363, 0.0));
    positionAlloc4->Add (Vector(413.4182718684983,404.2323866013575, 0.0));
    positionAlloc4->Add (Vector(444.4411658151736,486.6591256590125, 0.0));
    positionAlloc4->Add (Vector(823.9493655865125,238.65911099303577, 0.0));
    positionAlloc4->Add (Vector(431.6062746534231,698.3030647072064, 0.0));
    positionAlloc4->Add (Vector(648.4055728387139,419.47110584629, 0.0));
    positionAlloc4->Add (Vector(706.5510168268148,276.87517888608636, 0.0));
    positionAlloc4->Add (Vector(222.20273261485272,810.1144967520153, 0.0));
    positionAlloc4->Add (Vector(523.9542869518732,726.6612249443765, 0.0));
    positionAlloc4->Add (Vector(374.6548068236083,326.5769836986193, 0.0));
    positionAlloc4->Add (Vector(362.60669614227805,794.748640409707, 0.0));
    positionAlloc4->Add (Vector(815.658946306084,165.54196909149522, 0.0));
    positionAlloc4->Add (Vector(601.3992304008087,411.75709952264225, 0.0));
    positionAlloc4->Add (Vector(234.1896741900248,642.4698381270713, 0.0));
    positionAlloc4->Add (Vector(527.2866121150844,351.51565233439123, 0.0));
    positionAlloc4->Add (Vector(427.37314277148414,824.6723628317263, 0.0));
    positionAlloc4->Add (Vector(627.0224287620199,194.475005696743, 0.0));
    positionAlloc4->Add (Vector(829.6332980149086,411.52396167384137, 0.0));
    positionAlloc4->Add (Vector(694.8603331818439,576.0001152217819, 0.0));
    positionAlloc4->Add (Vector(757.1482473807505,313.5263224253658, 0.0));
    positionAlloc4->Add (Vector(238.85524037192346,803.5307644845659, 0.0));
    positionAlloc4->Add (Vector(348.3075695983915,150.1700541041589, 0.0));
    positionAlloc4->Add (Vector(232.55707241196345,608.4587731971251, 0.0));
    positionAlloc4->Add (Vector(352.9911748508993,590.2336014465305, 0.0));
    positionAlloc4->Add (Vector(270.19699228158174,520.8837222734492, 0.0));
    positionAlloc4->Add (Vector(661.7720881024935,319.5284119156374, 0.0));
    positionAlloc4->Add (Vector(435.7662635424091,346.3242569432567, 0.0));
    positionAlloc4->Add (Vector(831.6198074269455,614.353827669545, 0.0));
    positionAlloc4->Add (Vector(499.7328883991334,363.6501710902014, 0.0));
    positionAlloc4->Add (Vector(669.617171873607,689.2882509480506, 0.0));
    positionAlloc4->Add (Vector(288.98000120774117,496.3799196349941, 0.0));
    positionAlloc4->Add (Vector(710.8200523067273,573.0665033579658, 0.0));
    positionAlloc4->Add (Vector(422.1960475318815,183.19094529805943, 0.0));
    positionAlloc4->Add (Vector(721.3594963039602,841.7834806002307, 0.0));
    positionAlloc4->Add (Vector(458.52846040892246,559.3297658361132, 0.0));
    positionAlloc4->Add (Vector(208.1271304671305,283.736308096078, 0.0));
    positionAlloc4->Add (Vector(522.6130546937163,783.7252798632628, 0.0));
    positionAlloc4->Add (Vector(560.104135369545,833.2548327307632, 0.0));
    positionAlloc4->Add (Vector(753.872614062008,460.3242245408891, 0.0));
    positionAlloc4->Add (Vector(186.43208712929203,768.9716217502679, 0.0));
    positionAlloc4->Add (Vector(815.424460105911,660.7748895530689, 0.0));
    positionAlloc4->Add (Vector(628.1487026330869,612.5091751764061, 0.0));
    positionAlloc4->Add (Vector(299.660847463913,223.85955248084076, 0.0));
    positionAlloc4->Add (Vector(369.64750031401013,249.29107833188965, 0.0));
    positionAlloc4->Add (Vector(408.8164676108821,490.24027933187665, 0.0));
    positionAlloc4->Add (Vector(576.6699568278445,559.6363497749977, 0.0));
    positionAlloc4->Add (Vector(733.2898974250935,324.9712868676469, 0.0));
    positionAlloc4->Add (Vector(662.9063907893146,630.1133925605252, 0.0));
    positionAlloc4->Add (Vector(454.9124106088963,440.3927952111165, 0.0));
    positionAlloc4->Add (Vector(464.38638975117556,549.4652052865523, 0.0));
    positionAlloc4->Add (Vector(486.7879842717919,383.35748668159397, 0.0));
    positionAlloc4->Add (Vector(825.9572223364886,160.39928575525266, 0.0));
    positionAlloc4->Add (Vector(814.0578566320935,268.8309487449951, 0.0));
    positionAlloc4->Add (Vector(592.2423905569287,329.98295279908615, 0.0));
    positionAlloc4->Add (Vector(486.45361981868626,536.2801134739179, 0.0));
    positionAlloc4->Add (Vector(385.4189092534689,314.13880474821894, 0.0));
    positionAlloc4->Add (Vector(710.9600748245457,428.3444900129271, 0.0));
    positionAlloc4->Add (Vector(822.1864509444466,635.2185521776922, 0.0));
    positionAlloc4->Add (Vector(239.1183151227075,272.97060857800915, 0.0));
    positionAlloc4->Add (Vector(759.1376833445695,247.0567703793126, 0.0));
    positionAlloc4->Add (Vector(645.26597328463,171.82243637834486, 0.0));
    positionAlloc4->Add (Vector(465.6905518538406,220.5269907717034, 0.0));
    positionAlloc4->Add (Vector(178.68475557492332,643.5442989437654, 0.0));
    positionAlloc4->Add (Vector(835.8140343417018,337.7932143174897, 0.0));
    positionAlloc4->Add (Vector(392.52850095506705,831.1317446599128, 0.0));
    positionAlloc4->Add (Vector(339.9669765557092,748.576595398208, 0.0));
    positionAlloc4->Add (Vector(706.0689575649994,219.27803638609478, 0.0));
    positionAlloc4->Add (Vector(321.94210183417016,226.49561175771194, 0.0));
    positionAlloc4->Add (Vector(397.43423258986513,599.5467459018882, 0.0));
    positionAlloc4->Add (Vector(517.1054421004245,714.2677235725232, 0.0));
    positionAlloc4->Add (Vector(728.3521630426541,309.34277507357467, 0.0));
    positionAlloc4->Add (Vector(311.05914430023256,434.2574927918397, 0.0));
    positionAlloc4->Add (Vector(710.9464407891741,437.3533749438496, 0.0));
    positionAlloc4->Add (Vector(498.5084263600234,739.2660963201732, 0.0));
    positionAlloc4->Add (Vector(469.428146675679,775.8639584804919, 0.0));
    positionAlloc4->Add (Vector(481.2492308190791,582.408846843256, 0.0));
    positionAlloc4->Add (Vector(623.9515988609136,690.5681931252936, 0.0));
    positionAlloc4->Add (Vector(595.6850400505592,304.8664738646786, 0.0));
    positionAlloc4->Add (Vector(737.5856263264903,463.71959291492294, 0.0));
    positionAlloc4->Add (Vector(635.4568859324243,177.79784530057304, 0.0));
    positionAlloc4->Add (Vector(679.5770084467147,330.1558208811438, 0.0));
    positionAlloc4->Add (Vector(547.9305415097738,404.607864794979, 0.0));
    positionAlloc4->Add (Vector(217.02793906435036,795.80068343406, 0.0));
    positionAlloc4->Add (Vector(654.1875467631769,631.0981030891894, 0.0));
    positionAlloc4->Add (Vector(785.9062835105855,223.13622172777065, 0.0));
    positionAlloc4->Add (Vector(296.6634570187725,468.7301248660215, 0.0));
    positionAlloc4->Add (Vector(231.76287541376638,341.2117342589616, 0.0));
    positionAlloc4->Add (Vector(341.8082462698833,370.38880878611474, 0.0));
    positionAlloc4->Add (Vector(608.3430258989822,309.89809661589413, 0.0));
    positionAlloc4->Add (Vector(704.5329567926716,737.0352573386398, 0.0));
    positionAlloc4->Add (Vector(698.7667626684887,467.28324076810173, 0.0));
    positionAlloc4->Add (Vector(457.0701877621342,697.401277915759, 0.0));
    positionAlloc4->Add (Vector(631.8050988990785,303.80592507727783, 0.0));
    positionAlloc4->Add (Vector(290.23787476750965,297.7046841952682, 0.0));
    positionAlloc4->Add (Vector(662.1936909062061,646.4264926560833, 0.0));
    positionAlloc4->Add (Vector(795.208668616163,680.2283355061721, 0.0));
    positionAlloc4->Add (Vector(511.37347624004235,257.62451847204267, 0.0));
    positionAlloc4->Add (Vector(263.14638564921427,784.0945729995141, 0.0));
    positionAlloc4->Add (Vector(837.8566880454985,595.9332874525393, 0.0));
    positionAlloc4->Add (Vector(444.23477243684925,776.570020346371, 0.0));
    positionAlloc4->Add (Vector(342.4269607799614,172.9599119938272, 0.0));
    positionAlloc4->Add (Vector(568.8142529872132,171.6794223957571, 0.0));
    positionAlloc4->Add (Vector(240.28523554901568,477.92286596096056, 0.0));
    positionAlloc4->Add (Vector(765.2168021389965,370.4532592212528, 0.0));
    positionAlloc4->Add (Vector(255.21743573877814,177.0140933566937, 0.0));
    positionAlloc4->Add (Vector(618.5438455339572,732.1017481373168, 0.0));
    positionAlloc4->Add (Vector(775.6419314333614,520.8663399490379, 0.0));
    positionAlloc4->Add (Vector(572.1097043552118,189.99290354033099, 0.0));
    positionAlloc4->Add (Vector(815.1478441058011,194.28387027447235, 0.0));
    positionAlloc4->Add (Vector(802.3389128182191,345.2103483826754, 0.0));
    positionAlloc4->Add (Vector(587.4606392854823,660.209356642017, 0.0));
    positionAlloc4->Add (Vector(416.9861669321864,248.60802276560293, 0.0));
    positionAlloc4->Add (Vector(780.8814454575943,264.7441834328614, 0.0));
    positionAlloc4->Add (Vector(404.4601201584867,598.7042401244909, 0.0));
    positionAlloc4->Add (Vector(401.53729336105107,482.751347376608, 0.0));
    positionAlloc4->Add (Vector(506.82994683060997,559.6826128568925, 0.0));
    positionAlloc4->Add (Vector(333.7560229865809,235.32244845625127, 0.0));
    positionAlloc4->Add (Vector(567.706093074352,386.0414246182669, 0.0));
    positionAlloc4->Add (Vector(725.2610214779403,688.4753914691107, 0.0));
    positionAlloc4->Add (Vector(690.7658142337812,268.183524656799, 0.0));
    positionAlloc4->Add (Vector(747.3425483261067,754.4440153239728, 0.0));
    positionAlloc4->Add (Vector(808.277923761881,332.4664677934712, 0.0));
    positionAlloc4->Add (Vector(274.91547186886237,205.71170316042972, 0.0));
    positionAlloc4->Add (Vector(603.4859270257585,648.4503945735514, 0.0));
    positionAlloc4->Add (Vector(229.6333711631846,437.6724256251323, 0.0));
    positionAlloc4->Add (Vector(626.4081650431372,234.8854253513957, 0.0));
    positionAlloc4->Add (Vector(151.6432317072172,321.3599111012081, 0.0));
    positionAlloc4->Add (Vector(526.2456275712641,184.50976368641895, 0.0));
    positionAlloc4->Add (Vector(766.4554042252045,439.03783271244924, 0.0));
    positionAlloc4->Add (Vector(426.34272174017804,494.5513283012268, 0.0));
    positionAlloc4->Add (Vector(241.24617580035425,707.2479677778562, 0.0));
    positionAlloc4->Add (Vector(180.90189346121883,844.4645309201741, 0.0));
    positionAlloc4->Add (Vector(338.38382684660723,712.1931622553722, 0.0));
    positionAlloc4->Add (Vector(553.9142811833908,304.8033270774854, 0.0));
    positionAlloc4->Add (Vector(330.3869537324258,769.4487227914766, 0.0));
    positionAlloc4->Add (Vector(644.2406802466217,493.3180859638493, 0.0));
    positionAlloc4->Add (Vector(493.4295031287394,744.1373096969199, 0.0));
    positionAlloc4->Add (Vector(711.1498128819571,459.07380726522786, 0.0));
    positionAlloc4->Add (Vector(255.34684865080987,502.49905208248845, 0.0));
    positionAlloc4->Add (Vector(371.62170437918905,430.1086910057793, 0.0));
    positionAlloc4->Add (Vector(297.0331361044487,425.8585539570223, 0.0));
    positionAlloc4->Add (Vector(223.26244373552737,725.8535490357225, 0.0));
    positionAlloc4->Add (Vector(534.7356107742245,771.4723482350734, 0.0));
    positionAlloc4->Add (Vector(187.8333391448086,470.06129373368105, 0.0));
    positionAlloc4->Add (Vector(603.0555983784989,181.33787208194082, 0.0));
    positionAlloc4->Add (Vector(607.6445460693094,420.93500835442086, 0.0));
    positionAlloc4->Add (Vector(524.8653412601745,530.5169024379045, 0.0));
    positionAlloc4->Add (Vector(808.125884310469,774.4520457655344, 0.0));
    positionAlloc4->Add (Vector(737.4455671276213,172.39612210102732, 0.0));
    positionAlloc4->Add (Vector(400.4888110490732,155.91604724372237, 0.0));
    positionAlloc4->Add (Vector(285.6019355434085,392.07999921671575, 0.0));
    positionAlloc4->Add (Vector(371.462785171378,831.201297998836, 0.0));
    positionAlloc4->Add (Vector(156.44062833776562,589.0808804451848, 0.0));
    positionAlloc4->Add (Vector(188.73267101434226,288.6915192765039, 0.0));
    positionAlloc4->Add (Vector(245.45977352062084,287.6819907618492, 0.0));
    positionAlloc4->Add (Vector(437.3659490459995,311.94409924010534, 0.0));
    positionAlloc4->Add (Vector(496.95774461827295,444.7832331038571, 0.0));
    positionAlloc4->Add (Vector(703.1819327591857,357.02698423423055, 0.0));
    positionAlloc4->Add (Vector(362.25368270301385,638.5933300484547, 0.0));
    positionAlloc4->Add (Vector(232.9231935661373,159.54789208511676, 0.0));
    positionAlloc4->Add (Vector(732.5137176103829,495.80816941951394, 0.0));
    positionAlloc4->Add (Vector(724.5150885710112,580.1787127549208, 0.0));
    positionAlloc4->Add (Vector(197.06844383642562,475.2826279116155, 0.0));
    positionAlloc4->Add (Vector(547.8234057545478,457.65544631328623, 0.0));
    positionAlloc4->Add (Vector(418.1485588085547,758.36934810731, 0.0));
    positionAlloc4->Add (Vector(573.9894683219129,634.3047357125911, 0.0));
    positionAlloc4->Add (Vector(260.80037924136866,468.9625750871644, 0.0));
    positionAlloc4->Add (Vector(294.3419011312452,763.5923435521023, 0.0));
    positionAlloc4->Add (Vector(290.3283334364513,439.42603669711207, 0.0));
    positionAlloc4->Add (Vector(542.4861369629762,216.5395757827951, 0.0));
    positionAlloc4->Add (Vector(810.8663501512552,385.1623073652537, 0.0));
    positionAlloc4->Add (Vector(466.1472968631865,749.7109625541282, 0.0));
    positionAlloc4->Add (Vector(438.29659820114784,659.3055254944609, 0.0));
    positionAlloc4->Add (Vector(398.0967943911703,416.8028174243149, 0.0));
    positionAlloc4->Add (Vector(784.1607775483851,546.7642735073262, 0.0));
    positionAlloc4->Add (Vector(327.44562728003325,808.3761107720634, 0.0));
    positionAlloc4->Add (Vector(310.9428596031428,511.16211496779636, 0.0));
    positionAlloc4->Add (Vector(222.37645564102905,452.0556838933497, 0.0));
    positionAlloc4->Add (Vector(306.1630129117491,189.56098355773014, 0.0));
    positionAlloc4->Add (Vector(329.86660965592887,164.63665863466449, 0.0));
    positionAlloc4->Add (Vector(653.6219773689803,748.1980648228487, 0.0));
    positionAlloc4->Add (Vector(230.71076323733453,224.89388282223632, 0.0));
    positionAlloc4->Add (Vector(840.4449460609134,370.6517161641002, 0.0));
    positionAlloc4->Add (Vector(176.4740955864247,236.13648651322745, 0.0));
    positionAlloc4->Add (Vector(313.50630395565145,334.4314119856938, 0.0));
    positionAlloc4->Add (Vector(805.7974010637107,265.1677606749687, 0.0));
    positionAlloc4->Add (Vector(658.9918378173493,150.50180109460584, 0.0));
    positionAlloc4->Add (Vector(450.2212019451474,606.5848041606391, 0.0));
    positionAlloc4->Add (Vector(534.2527671495363,303.5242808479675, 0.0));
    positionAlloc4->Add (Vector(167.07369517706772,556.997426408361, 0.0));
    positionAlloc4->Add (Vector(389.3541455023086,796.952904389862, 0.0));
    positionAlloc4->Add (Vector(811.3250561471483,492.45137408166346, 0.0));
    positionAlloc4->Add (Vector(765.4430363097679,159.1071493981109, 0.0));
    positionAlloc4->Add (Vector(468.8511866534542,171.57723308955917, 0.0));
    positionAlloc4->Add (Vector(778.108947024817,512.7528209451492, 0.0));
    positionAlloc4->Add (Vector(388.73351098143644,712.3132014626235, 0.0));
    positionAlloc4->Add (Vector(383.95073608208025,168.01968682305133, 0.0));
    positionAlloc4->Add (Vector(324.26046735781097,377.1067600445464, 0.0));
    positionAlloc4->Add (Vector(502.72215775389145,646.1359426419582, 0.0));
    positionAlloc4->Add (Vector(222.0623012902003,653.3159743035208, 0.0));
    positionAlloc4->Add (Vector(347.7786045637105,414.6174646040814, 0.0));
    positionAlloc4->Add (Vector(611.5962727949413,587.2904685846256, 0.0));
    positionAlloc4->Add (Vector(424.07596972114044,649.3252106508905, 0.0));
    positionAlloc4->Add (Vector(579.6921615054071,211.40234920397504, 0.0));
    positionAlloc4->Add (Vector(774.2741479846289,287.5089206559501, 0.0));
    positionAlloc4->Add (Vector(333.4245031548122,767.2021144767774, 0.0));
    positionAlloc4->Add (Vector(323.0442516172138,673.8626745997278, 0.0));
    positionAlloc4->Add (Vector(239.73165096732322,447.74954738898856, 0.0));
    positionAlloc4->Add (Vector(260.79813548956156,631.6823614444452, 0.0));
    positionAlloc4->Add (Vector(361.8482384457702,613.6603019389274, 0.0));
    positionAlloc4->Add (Vector(269.8913874663677,669.1464730323346, 0.0));
    positionAlloc4->Add (Vector(474.37370616325234,569.6013794193352, 0.0));
    positionAlloc4->Add (Vector(665.1674923606246,360.86190635573575, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_13.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_13.flowmon", false, false);
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