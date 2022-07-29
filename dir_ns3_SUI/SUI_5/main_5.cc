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
	  uint16_t numberOfRrhs = 20;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 311;
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
    positionAlloc->Add (Vector (772.2222222222222,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,150.0, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,850.0, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,383.33333333333337, 0.0));
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
    positionAlloc4->Add (Vector(707.5083930664341,667.628463267109, 0.0));
    positionAlloc4->Add (Vector(367.21593747318326,502.3415914243078, 0.0));
    positionAlloc4->Add (Vector(633.8726646501925,267.29657751315455, 0.0));
    positionAlloc4->Add (Vector(531.1887272582728,261.915834356509, 0.0));
    positionAlloc4->Add (Vector(413.9377517011892,425.32446926963206, 0.0));
    positionAlloc4->Add (Vector(348.9290069982101,352.27796744474097, 0.0));
    positionAlloc4->Add (Vector(771.7339447629129,289.45378008595355, 0.0));
    positionAlloc4->Add (Vector(786.441495344541,723.7374086833514, 0.0));
    positionAlloc4->Add (Vector(688.6098055950897,804.3568622449484, 0.0));
    positionAlloc4->Add (Vector(159.26016903370578,508.2463195986365, 0.0));
    positionAlloc4->Add (Vector(268.16700404919493,747.760017499297, 0.0));
    positionAlloc4->Add (Vector(322.8728168943247,751.7517417468246, 0.0));
    positionAlloc4->Add (Vector(204.49956372613872,818.8137340066472, 0.0));
    positionAlloc4->Add (Vector(724.8751081917931,531.2416097749951, 0.0));
    positionAlloc4->Add (Vector(786.6006525801135,283.090136423094, 0.0));
    positionAlloc4->Add (Vector(236.6922186940722,199.3749463150696, 0.0));
    positionAlloc4->Add (Vector(235.74850337619245,594.434661407214, 0.0));
    positionAlloc4->Add (Vector(265.0007902906865,698.213001646588, 0.0));
    positionAlloc4->Add (Vector(358.64634081900294,374.2809002981504, 0.0));
    positionAlloc4->Add (Vector(192.15886143020822,358.3551942977963, 0.0));
    positionAlloc4->Add (Vector(224.76049280165455,817.3820155674665, 0.0));
    positionAlloc4->Add (Vector(495.069880159102,381.10916522092305, 0.0));
    positionAlloc4->Add (Vector(763.1334474999262,783.7092993073609, 0.0));
    positionAlloc4->Add (Vector(442.17634763040354,413.80681624303196, 0.0));
    positionAlloc4->Add (Vector(538.884698919934,353.95275098642117, 0.0));
    positionAlloc4->Add (Vector(197.73063383582388,280.9869892873512, 0.0));
    positionAlloc4->Add (Vector(318.81565001543333,313.4971527480277, 0.0));
    positionAlloc4->Add (Vector(150.41855718435673,365.4734050182834, 0.0));
    positionAlloc4->Add (Vector(386.13142619203586,729.2665857676719, 0.0));
    positionAlloc4->Add (Vector(314.3280952074799,723.20848936433, 0.0));
    positionAlloc4->Add (Vector(154.88758331624368,432.43667831889974, 0.0));
    positionAlloc4->Add (Vector(644.4603977669972,391.9057101095691, 0.0));
    positionAlloc4->Add (Vector(750.2714254263466,218.15682565385475, 0.0));
    positionAlloc4->Add (Vector(526.8764855008703,844.494947635008, 0.0));
    positionAlloc4->Add (Vector(678.1985528440156,324.0234192293144, 0.0));
    positionAlloc4->Add (Vector(488.2624531128046,840.8271617718083, 0.0));
    positionAlloc4->Add (Vector(472.5135201691717,842.1883789125642, 0.0));
    positionAlloc4->Add (Vector(156.93748684380756,694.6324145382395, 0.0));
    positionAlloc4->Add (Vector(753.0119802149269,756.2249725320296, 0.0));
    positionAlloc4->Add (Vector(622.9107414972071,506.22584904891613, 0.0));
    positionAlloc4->Add (Vector(388.8711148304044,460.9283848148566, 0.0));
    positionAlloc4->Add (Vector(424.2676262320202,711.681249115927, 0.0));
    positionAlloc4->Add (Vector(282.912730367144,177.27049244392495, 0.0));
    positionAlloc4->Add (Vector(312.9098442408863,187.68136743592135, 0.0));
    positionAlloc4->Add (Vector(174.91316156453348,846.0312118800005, 0.0));
    positionAlloc4->Add (Vector(653.1026792835416,227.4225192112126, 0.0));
    positionAlloc4->Add (Vector(412.3774470449984,584.931436908995, 0.0));
    positionAlloc4->Add (Vector(508.6749745124746,394.18233522104106, 0.0));
    positionAlloc4->Add (Vector(451.3750746391259,233.39847039723372, 0.0));
    positionAlloc4->Add (Vector(656.8204321410449,800.1655724815197, 0.0));
    positionAlloc4->Add (Vector(234.25450334699025,197.3233246306682, 0.0));
    positionAlloc4->Add (Vector(669.604562040863,267.47021695222, 0.0));
    positionAlloc4->Add (Vector(790.5010764232691,740.7155878616503, 0.0));
    positionAlloc4->Add (Vector(303.7444500161983,150.15892675277888, 0.0));
    positionAlloc4->Add (Vector(292.9694744579002,443.08660032937894, 0.0));
    positionAlloc4->Add (Vector(391.51178794719624,188.31289484749013, 0.0));
    positionAlloc4->Add (Vector(454.68359575958766,794.4839805450337, 0.0));
    positionAlloc4->Add (Vector(567.2350810326652,847.0141164359261, 0.0));
    positionAlloc4->Add (Vector(509.01231961368745,374.59817992489053, 0.0));
    positionAlloc4->Add (Vector(329.6530450302671,676.4800328878823, 0.0));
    positionAlloc4->Add (Vector(443.27884164566706,470.620031608708, 0.0));
    positionAlloc4->Add (Vector(724.1773232614228,604.1229797265128, 0.0));
    positionAlloc4->Add (Vector(699.5447447325018,286.99884806975433, 0.0));
    positionAlloc4->Add (Vector(440.601957794922,445.5345637164746, 0.0));
    positionAlloc4->Add (Vector(614.2023742826461,566.7564591098109, 0.0));
    positionAlloc4->Add (Vector(324.98102360818075,201.35484722172185, 0.0));
    positionAlloc4->Add (Vector(246.86011630438963,837.9176786372701, 0.0));
    positionAlloc4->Add (Vector(503.8645542064432,505.43048673199814, 0.0));
    positionAlloc4->Add (Vector(458.46541839700546,765.9402387089101, 0.0));
    positionAlloc4->Add (Vector(390.86413249460946,514.3806861134009, 0.0));
    positionAlloc4->Add (Vector(408.39533566644315,400.0346337951314, 0.0));
    positionAlloc4->Add (Vector(530.9801359106756,833.0451729842529, 0.0));
    positionAlloc4->Add (Vector(247.47784954929278,157.50070838675126, 0.0));
    positionAlloc4->Add (Vector(692.8537077032995,425.18525874495543, 0.0));
    positionAlloc4->Add (Vector(809.4407536420168,358.7907179638859, 0.0));
    positionAlloc4->Add (Vector(339.391479541594,324.15425211742394, 0.0));
    positionAlloc4->Add (Vector(625.4137978236939,591.9631843503637, 0.0));
    positionAlloc4->Add (Vector(183.0376911881653,822.9916337494346, 0.0));
    positionAlloc4->Add (Vector(435.4446240843139,770.4662533720992, 0.0));
    positionAlloc4->Add (Vector(657.7601201644002,228.81332657043612, 0.0));
    positionAlloc4->Add (Vector(404.08798828271017,765.4956893099047, 0.0));
    positionAlloc4->Add (Vector(619.7090863730368,637.9169162030096, 0.0));
    positionAlloc4->Add (Vector(223.78096417752357,737.1977033328482, 0.0));
    positionAlloc4->Add (Vector(730.6939009781349,646.5721752770703, 0.0));
    positionAlloc4->Add (Vector(472.6288367446073,730.0962886726045, 0.0));
    positionAlloc4->Add (Vector(507.77954694265037,573.0190875682239, 0.0));
    positionAlloc4->Add (Vector(282.43705467324844,654.268799010512, 0.0));
    positionAlloc4->Add (Vector(735.5095347289088,534.2367421480501, 0.0));
    positionAlloc4->Add (Vector(418.37799874078127,267.795011955399, 0.0));
    positionAlloc4->Add (Vector(482.8330006629136,744.3686139710733, 0.0));
    positionAlloc4->Add (Vector(187.54435406458157,652.2254934283385, 0.0));
    positionAlloc4->Add (Vector(761.1302029106423,268.2009243592765, 0.0));
    positionAlloc4->Add (Vector(624.4062688409243,326.62031529460705, 0.0));
    positionAlloc4->Add (Vector(643.5286344475148,669.3844106597901, 0.0));
    positionAlloc4->Add (Vector(750.9545181860882,242.18083240534492, 0.0));
    positionAlloc4->Add (Vector(812.024791558151,369.0436836201758, 0.0));
    positionAlloc4->Add (Vector(676.2776021696768,718.3623642465184, 0.0));
    positionAlloc4->Add (Vector(380.1758324940265,613.619617111504, 0.0));
    positionAlloc4->Add (Vector(246.11002290089917,816.6707266791072, 0.0));
    positionAlloc4->Add (Vector(711.6772213875428,752.9926322668622, 0.0));
    positionAlloc4->Add (Vector(233.96705921167887,368.44483125641443, 0.0));
    positionAlloc4->Add (Vector(602.0075303457376,507.39928610995247, 0.0));
    positionAlloc4->Add (Vector(382.13179437548376,500.4055222015819, 0.0));
    positionAlloc4->Add (Vector(811.5168452063981,627.7783351370099, 0.0));
    positionAlloc4->Add (Vector(252.68657981062708,633.2259928305102, 0.0));
    positionAlloc4->Add (Vector(750.0616561588538,685.2556652694115, 0.0));
    positionAlloc4->Add (Vector(361.72642222302875,751.0886848712017, 0.0));
    positionAlloc4->Add (Vector(716.1784264441789,762.8420552771275, 0.0));
    positionAlloc4->Add (Vector(323.97787464919816,371.85983523709695, 0.0));
    positionAlloc4->Add (Vector(490.3215578734181,226.3095309486501, 0.0));
    positionAlloc4->Add (Vector(387.4684628216854,193.70192909444623, 0.0));
    positionAlloc4->Add (Vector(819.4043170593529,413.6735083006237, 0.0));
    positionAlloc4->Add (Vector(742.2869473711621,277.96937197745814, 0.0));
    positionAlloc4->Add (Vector(675.8952470616368,777.8967025550878, 0.0));
    positionAlloc4->Add (Vector(786.4768861058947,409.1625299792037, 0.0));
    positionAlloc4->Add (Vector(289.0421297033084,698.3710539970868, 0.0));
    positionAlloc4->Add (Vector(433.7277524528673,338.41309969414044, 0.0));
    positionAlloc4->Add (Vector(671.6503198835842,312.5572836309402, 0.0));
    positionAlloc4->Add (Vector(449.2265697166823,601.6530030912647, 0.0));
    positionAlloc4->Add (Vector(503.1191591685488,368.5328155983906, 0.0));
    positionAlloc4->Add (Vector(552.5424014211703,294.7725251933774, 0.0));
    positionAlloc4->Add (Vector(620.1962014708425,660.5168603340444, 0.0));
    positionAlloc4->Add (Vector(425.4887352744209,241.5021265128301, 0.0));
    positionAlloc4->Add (Vector(634.1589563065791,825.9472826867603, 0.0));
    positionAlloc4->Add (Vector(697.3236390978228,686.1010507193195, 0.0));
    positionAlloc4->Add (Vector(804.1807165008609,362.3698805403932, 0.0));
    positionAlloc4->Add (Vector(833.592716326033,601.9218039408881, 0.0));
    positionAlloc4->Add (Vector(437.477843107198,296.7831942011406, 0.0));
    positionAlloc4->Add (Vector(682.0038814734689,368.8186174358101, 0.0));
    positionAlloc4->Add (Vector(317.34646885252073,680.6282947078515, 0.0));
    positionAlloc4->Add (Vector(500.0407814337607,622.4252421379083, 0.0));
    positionAlloc4->Add (Vector(246.68770519817306,751.3782303528416, 0.0));
    positionAlloc4->Add (Vector(329.33406010754845,304.4557417352239, 0.0));
    positionAlloc4->Add (Vector(311.7565970121474,778.1700022246005, 0.0));
    positionAlloc4->Add (Vector(765.8266816060406,184.4546401785331, 0.0));
    positionAlloc4->Add (Vector(654.1102226516582,351.0548464436056, 0.0));
    positionAlloc4->Add (Vector(802.2182823215558,847.8645921881688, 0.0));
    positionAlloc4->Add (Vector(614.1190426062032,625.3466231469462, 0.0));
    positionAlloc4->Add (Vector(470.96299353091075,545.2134472534244, 0.0));
    positionAlloc4->Add (Vector(257.6485938592513,770.3199058391951, 0.0));
    positionAlloc4->Add (Vector(802.9270573422192,164.0964942645029, 0.0));
    positionAlloc4->Add (Vector(618.2135717284798,709.0590462496456, 0.0));
    positionAlloc4->Add (Vector(159.7034951390021,604.5651741656357, 0.0));
    positionAlloc4->Add (Vector(430.21898532109583,718.3062868072659, 0.0));
    positionAlloc4->Add (Vector(299.79627508881947,348.2990824448041, 0.0));
    positionAlloc4->Add (Vector(270.2806222503609,512.6542486282412, 0.0));
    positionAlloc4->Add (Vector(207.10244792795615,211.451765940712, 0.0));
    positionAlloc4->Add (Vector(448.54982306930276,580.7945458745849, 0.0));
    positionAlloc4->Add (Vector(538.593650727232,561.594140624683, 0.0));
    positionAlloc4->Add (Vector(322.79991931701306,364.49081454572547, 0.0));
    positionAlloc4->Add (Vector(338.15833052113874,289.05120382818404, 0.0));
    positionAlloc4->Add (Vector(550.4860746055444,276.5496053936657, 0.0));
    positionAlloc4->Add (Vector(674.9019519017882,789.1290268228992, 0.0));
    positionAlloc4->Add (Vector(717.1127629717918,450.77135434860355, 0.0));
    positionAlloc4->Add (Vector(773.4609188232897,567.8616563892208, 0.0));
    positionAlloc4->Add (Vector(753.1654192893707,553.9819576715345, 0.0));
    positionAlloc4->Add (Vector(287.4060116177469,165.79515819374308, 0.0));
    positionAlloc4->Add (Vector(514.9639150316189,777.6022609283071, 0.0));
    positionAlloc4->Add (Vector(535.3721764173579,435.15586694898263, 0.0));
    positionAlloc4->Add (Vector(213.9953942719967,587.9773385829469, 0.0));
    positionAlloc4->Add (Vector(272.08648697804966,480.15464352541676, 0.0));
    positionAlloc4->Add (Vector(559.3794679140536,809.0319662542457, 0.0));
    positionAlloc4->Add (Vector(664.2258104973746,166.76238568933843, 0.0));
    positionAlloc4->Add (Vector(497.3674553305697,349.9381919935702, 0.0));
    positionAlloc4->Add (Vector(351.7291681716706,154.5250504527974, 0.0));
    positionAlloc4->Add (Vector(569.5095009044721,509.59549865071335, 0.0));
    positionAlloc4->Add (Vector(332.93311251976866,443.3696310419597, 0.0));
    positionAlloc4->Add (Vector(491.7243727175698,243.22748796726637, 0.0));
    positionAlloc4->Add (Vector(664.5176303328764,577.0755454253642, 0.0));
    positionAlloc4->Add (Vector(547.1736609092513,707.370526619471, 0.0));
    positionAlloc4->Add (Vector(332.8137027576022,636.4671835401334, 0.0));
    positionAlloc4->Add (Vector(696.1488539749434,544.7250447387817, 0.0));
    positionAlloc4->Add (Vector(167.63936637474873,398.38229888021385, 0.0));
    positionAlloc4->Add (Vector(297.07503503864626,669.5899745766008, 0.0));
    positionAlloc4->Add (Vector(537.4302433468949,549.2008571119734, 0.0));
    positionAlloc4->Add (Vector(263.02048384496806,480.21788626290737, 0.0));
    positionAlloc4->Add (Vector(565.6464423604166,552.9154000480573, 0.0));
    positionAlloc4->Add (Vector(442.8726733648335,400.46787453491515, 0.0));
    positionAlloc4->Add (Vector(618.4533209474066,488.6617779024531, 0.0));
    positionAlloc4->Add (Vector(295.82077300596256,398.8075361548432, 0.0));
    positionAlloc4->Add (Vector(784.9293722905153,844.0729430791706, 0.0));
    positionAlloc4->Add (Vector(814.4911743719318,765.7210476982334, 0.0));
    positionAlloc4->Add (Vector(243.10685923513014,261.11573436201667, 0.0));
    positionAlloc4->Add (Vector(622.8192317354979,427.20067646993067, 0.0));
    positionAlloc4->Add (Vector(847.5684161590334,244.6521195989983, 0.0));
    positionAlloc4->Add (Vector(431.9176393227815,513.7281730621053, 0.0));
    positionAlloc4->Add (Vector(441.0729976128877,534.0118905382819, 0.0));
    positionAlloc4->Add (Vector(634.4353871278214,247.991163705234, 0.0));
    positionAlloc4->Add (Vector(756.49622847779,191.7321277412613, 0.0));
    positionAlloc4->Add (Vector(732.8282413282918,712.9158954887189, 0.0));
    positionAlloc4->Add (Vector(444.007703006964,554.8518341815779, 0.0));
    positionAlloc4->Add (Vector(383.9348999211769,661.6092420433123, 0.0));
    positionAlloc4->Add (Vector(276.6243309652288,709.4105797733474, 0.0));
    positionAlloc4->Add (Vector(436.77100850476336,834.7544679685908, 0.0));
    positionAlloc4->Add (Vector(307.26017834923664,429.53280613945583, 0.0));
    positionAlloc4->Add (Vector(762.0151338959564,731.435932302254, 0.0));
    positionAlloc4->Add (Vector(844.76023411338,318.46936033000713, 0.0));
    positionAlloc4->Add (Vector(394.95649749949985,697.6980685489515, 0.0));
    positionAlloc4->Add (Vector(200.9144634450679,346.4205692665412, 0.0));
    positionAlloc4->Add (Vector(482.56291100673764,795.7039985592871, 0.0));
    positionAlloc4->Add (Vector(624.2820636286431,532.8502131898338, 0.0));
    positionAlloc4->Add (Vector(275.32214620910867,708.2598317075401, 0.0));
    positionAlloc4->Add (Vector(315.8467957277064,217.8759696757973, 0.0));
    positionAlloc4->Add (Vector(370.75462028628226,425.20299064581064, 0.0));
    positionAlloc4->Add (Vector(569.5518075295159,660.0593989656219, 0.0));
    positionAlloc4->Add (Vector(218.89693237095813,819.7043037225449, 0.0));
    positionAlloc4->Add (Vector(384.76012435222367,273.95757877810286, 0.0));
    positionAlloc4->Add (Vector(440.81010041446274,166.8035313914222, 0.0));
    positionAlloc4->Add (Vector(437.55216699972823,492.75085322242285, 0.0));
    positionAlloc4->Add (Vector(619.3040336578422,599.2736617610047, 0.0));
    positionAlloc4->Add (Vector(203.7306433221166,152.93121053957051, 0.0));
    positionAlloc4->Add (Vector(417.4976323827985,538.312262297648, 0.0));
    positionAlloc4->Add (Vector(814.2864669329009,249.7895885231809, 0.0));
    positionAlloc4->Add (Vector(842.6537333690258,629.0199742477414, 0.0));
    positionAlloc4->Add (Vector(322.0932097453802,400.6978015704011, 0.0));
    positionAlloc4->Add (Vector(236.2986455092628,512.9931820660138, 0.0));
    positionAlloc4->Add (Vector(268.9209512533401,238.86108962848087, 0.0));
    positionAlloc4->Add (Vector(357.62873750996516,204.03127115852757, 0.0));
    positionAlloc4->Add (Vector(812.1796228919195,739.5157636552667, 0.0));
    positionAlloc4->Add (Vector(601.9005269991997,181.90961056406815, 0.0));
    positionAlloc4->Add (Vector(279.693795233478,528.8890776591747, 0.0));
    positionAlloc4->Add (Vector(218.60537934651876,180.84610309926697, 0.0));
    positionAlloc4->Add (Vector(821.8537142833951,184.08639387995083, 0.0));
    positionAlloc4->Add (Vector(764.6234589911198,540.5115729620512, 0.0));
    positionAlloc4->Add (Vector(551.7060947739674,546.2128974214747, 0.0));
    positionAlloc4->Add (Vector(193.428991869588,199.80897521453682, 0.0));
    positionAlloc4->Add (Vector(251.72868469435286,503.9877206469618, 0.0));
    positionAlloc4->Add (Vector(417.9488702821284,737.4026256495267, 0.0));
    positionAlloc4->Add (Vector(508.1952129802906,488.30599947642764, 0.0));
    positionAlloc4->Add (Vector(185.80514393427833,428.4554086503687, 0.0));
    positionAlloc4->Add (Vector(291.60576441074255,420.6949399307845, 0.0));
    positionAlloc4->Add (Vector(264.35673087889415,279.12133574191836, 0.0));
    positionAlloc4->Add (Vector(206.07400862672156,459.2835933613682, 0.0));
    positionAlloc4->Add (Vector(308.74083772375195,616.5653836851245, 0.0));
    positionAlloc4->Add (Vector(684.5845885452244,516.3809596454057, 0.0));
    positionAlloc4->Add (Vector(191.71096814720062,573.0515238003397, 0.0));
    positionAlloc4->Add (Vector(550.1253546380607,279.38687856291745, 0.0));
    positionAlloc4->Add (Vector(304.34134759032446,587.9555525990936, 0.0));
    positionAlloc4->Add (Vector(528.7796774151914,573.5626629783769, 0.0));
    positionAlloc4->Add (Vector(439.8987477509663,820.5621019482963, 0.0));
    positionAlloc4->Add (Vector(699.0838312489581,568.994095252432, 0.0));
    positionAlloc4->Add (Vector(728.1526159850114,592.0211548081145, 0.0));
    positionAlloc4->Add (Vector(709.8483551120484,538.2809173518569, 0.0));
    positionAlloc4->Add (Vector(561.60032440074,397.17202225687265, 0.0));
    positionAlloc4->Add (Vector(724.3413091361924,728.4385676651307, 0.0));
    positionAlloc4->Add (Vector(459.80414215297077,183.8538977341466, 0.0));
    positionAlloc4->Add (Vector(418.2507561630762,301.43294434157895, 0.0));
    positionAlloc4->Add (Vector(569.6227977611017,781.6822932774724, 0.0));
    positionAlloc4->Add (Vector(416.6662915164325,188.19578579788927, 0.0));
    positionAlloc4->Add (Vector(457.89894779968444,600.7696056133295, 0.0));
    positionAlloc4->Add (Vector(346.23432678598874,242.52576042771022, 0.0));
    positionAlloc4->Add (Vector(210.29249734457625,720.070745488133, 0.0));
    positionAlloc4->Add (Vector(165.4463040776036,837.7648905792271, 0.0));
    positionAlloc4->Add (Vector(716.7725253861225,815.3729746680863, 0.0));
    positionAlloc4->Add (Vector(620.5754811059362,486.8945658534659, 0.0));
    positionAlloc4->Add (Vector(556.4516581367438,805.1560737739917, 0.0));
    positionAlloc4->Add (Vector(433.79597759740847,511.3983094383718, 0.0));
    positionAlloc4->Add (Vector(655.4462449426496,178.19986314459894, 0.0));
    positionAlloc4->Add (Vector(829.658174723233,274.0074662558379, 0.0));
    positionAlloc4->Add (Vector(237.04208900160924,762.5935330348065, 0.0));
    positionAlloc4->Add (Vector(343.9484747993797,808.616118691443, 0.0));
    positionAlloc4->Add (Vector(730.0570342828024,698.4286920344874, 0.0));
    positionAlloc4->Add (Vector(483.7872144131871,591.0651888031643, 0.0));
    positionAlloc4->Add (Vector(729.9070696572936,206.1307297684793, 0.0));
    positionAlloc4->Add (Vector(678.5935322116371,560.6367427985639, 0.0));
    positionAlloc4->Add (Vector(206.43078361872122,807.698012317616, 0.0));
    positionAlloc4->Add (Vector(452.94003903927324,566.9758769130707, 0.0));
    positionAlloc4->Add (Vector(250.42861991261404,256.0118653547501, 0.0));
    positionAlloc4->Add (Vector(427.4188378487544,357.42806132986004, 0.0));
    positionAlloc4->Add (Vector(546.0953922192726,785.6129817951637, 0.0));
    positionAlloc4->Add (Vector(528.1952252311947,205.49619924183423, 0.0));
    positionAlloc4->Add (Vector(391.836597606412,712.1083170684244, 0.0));
    positionAlloc4->Add (Vector(760.3391428963538,553.8871286923772, 0.0));
    positionAlloc4->Add (Vector(536.656296472064,566.6608129035811, 0.0));
    positionAlloc4->Add (Vector(296.18165484621295,190.9957136991737, 0.0));
    positionAlloc4->Add (Vector(263.8625705738396,664.0559735445529, 0.0));
    positionAlloc4->Add (Vector(238.77681578529317,424.21971539359475, 0.0));
    positionAlloc4->Add (Vector(431.0232453623303,386.65908655638293, 0.0));
    positionAlloc4->Add (Vector(571.264713761813,557.2156531948307, 0.0));
    positionAlloc4->Add (Vector(256.0026200589743,484.1913326362476, 0.0));
    positionAlloc4->Add (Vector(604.1357703115702,818.1961502011675, 0.0));
    positionAlloc4->Add (Vector(719.6063406055592,808.3527670155818, 0.0));
    positionAlloc4->Add (Vector(603.7634281654923,193.01240309155125, 0.0));
    positionAlloc4->Add (Vector(611.7831910513321,459.42339465178645, 0.0));
    positionAlloc4->Add (Vector(440.3779588649464,421.09208799738894, 0.0));
    positionAlloc4->Add (Vector(230.6023248766408,768.2915209983566, 0.0));
    positionAlloc4->Add (Vector(398.3099897111489,430.49291678412663, 0.0));
    positionAlloc4->Add (Vector(848.4245723071405,466.2176331870107, 0.0));
    positionAlloc4->Add (Vector(263.34610464254956,610.6715526395444, 0.0));
    positionAlloc4->Add (Vector(387.5136871166972,504.1521580236909, 0.0));
    positionAlloc4->Add (Vector(414.48744960903065,193.42573454014706, 0.0));
    positionAlloc4->Add (Vector(559.1801792816207,828.5252685989252, 0.0));
    positionAlloc4->Add (Vector(325.61536992349016,234.3131379975958, 0.0));
    positionAlloc4->Add (Vector(438.59580948216586,804.3109408418106, 0.0));
    positionAlloc4->Add (Vector(679.377653995704,467.92856391569427, 0.0));
    positionAlloc4->Add (Vector(622.9640315449511,427.1560583371649, 0.0));
    positionAlloc4->Add (Vector(641.4752178796548,381.9465531154219, 0.0));
    positionAlloc4->Add (Vector(189.8169784443526,686.569121355139, 0.0));
    positionAlloc4->Add (Vector(164.65388247871329,371.0412058746472, 0.0));
    positionAlloc4->Add (Vector(401.035107367514,787.9374330550415, 0.0));
    positionAlloc4->Add (Vector(641.21679171408,539.5252461441754, 0.0));
    positionAlloc4->Add (Vector(476.32711378928985,282.9552955202927, 0.0));
    positionAlloc4->Add (Vector(405.3757291454591,598.6758696691304, 0.0));
    positionAlloc4->Add (Vector(558.1911533697084,661.1552024300745, 0.0));
    positionAlloc4->Add (Vector(778.5547054106158,716.9470942120296, 0.0));
    positionAlloc4->Add (Vector(365.43963370520436,579.5465320925687, 0.0));
    positionAlloc4->Add (Vector(632.5172884718831,326.618348948781, 0.0));
    positionAlloc4->Add (Vector(179.10527189261563,498.6909604836297, 0.0));
    positionAlloc4->Add (Vector(596.4418992919141,724.0472514389546, 0.0));
    positionAlloc4->Add (Vector(531.0414213440247,313.61855442572096, 0.0));
    positionAlloc4->Add (Vector(223.83839922272497,559.9113787108288, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_5.xml");
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
	  flowmon->SerializeToXmlFile ("switch_SUI_flowmon/switch_SUI_5.flowmon", false, false);
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