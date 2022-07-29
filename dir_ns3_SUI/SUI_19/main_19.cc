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
	  uint16_t numberOfRrhs = 37;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 679;
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
    positionAlloc->Add (Vector (772.2222222222222,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,772.2222222222222, 0.0));
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
    positionAlloc4->Add (Vector(818.9587976690735,798.9483812358295, 0.0));
    positionAlloc4->Add (Vector(495.28510387357454,791.305718770081, 0.0));
    positionAlloc4->Add (Vector(668.9764805958248,379.8501466290438, 0.0));
    positionAlloc4->Add (Vector(574.1651211053977,486.8704187637856, 0.0));
    positionAlloc4->Add (Vector(794.5353698856238,831.525354160489, 0.0));
    positionAlloc4->Add (Vector(667.5880188196841,203.37609504237741, 0.0));
    positionAlloc4->Add (Vector(254.11905691004824,553.7527959480653, 0.0));
    positionAlloc4->Add (Vector(746.0857819345571,188.8830634413522, 0.0));
    positionAlloc4->Add (Vector(616.9010728766052,205.85793259162017, 0.0));
    positionAlloc4->Add (Vector(628.2458166833105,487.087377989082, 0.0));
    positionAlloc4->Add (Vector(697.8345523539065,614.667073675198, 0.0));
    positionAlloc4->Add (Vector(697.4451651007017,272.7251748650194, 0.0));
    positionAlloc4->Add (Vector(652.2758044588389,371.5764119344713, 0.0));
    positionAlloc4->Add (Vector(531.2816654037836,493.9896282360172, 0.0));
    positionAlloc4->Add (Vector(357.96367814622454,554.0415652881702, 0.0));
    positionAlloc4->Add (Vector(338.21239453906037,320.6386451579092, 0.0));
    positionAlloc4->Add (Vector(208.76899214963112,769.4955293765181, 0.0));
    positionAlloc4->Add (Vector(801.3365737059069,588.4107663417061, 0.0));
    positionAlloc4->Add (Vector(170.20538762062154,426.56196564964296, 0.0));
    positionAlloc4->Add (Vector(582.7066031816901,389.13124372713435, 0.0));
    positionAlloc4->Add (Vector(580.0671478297276,446.28139993608045, 0.0));
    positionAlloc4->Add (Vector(849.4046967450612,674.1874319232958, 0.0));
    positionAlloc4->Add (Vector(561.1685166880529,367.533186760695, 0.0));
    positionAlloc4->Add (Vector(652.8521532400671,393.98265612236435, 0.0));
    positionAlloc4->Add (Vector(589.3372906461373,780.5858861976265, 0.0));
    positionAlloc4->Add (Vector(310.59307356099936,260.40422195827483, 0.0));
    positionAlloc4->Add (Vector(758.3329717337718,600.3270195730518, 0.0));
    positionAlloc4->Add (Vector(678.1776728867712,251.95205721854842, 0.0));
    positionAlloc4->Add (Vector(300.85080908363585,167.34188885406863, 0.0));
    positionAlloc4->Add (Vector(545.6584668195815,154.15344772223955, 0.0));
    positionAlloc4->Add (Vector(759.9501388625455,508.0099837423361, 0.0));
    positionAlloc4->Add (Vector(426.8654953759934,808.713875888064, 0.0));
    positionAlloc4->Add (Vector(171.33828221797162,164.53752069727383, 0.0));
    positionAlloc4->Add (Vector(248.40539006415761,377.96671259466586, 0.0));
    positionAlloc4->Add (Vector(812.9815522654649,400.97287835889074, 0.0));
    positionAlloc4->Add (Vector(498.84632391732237,833.611137440269, 0.0));
    positionAlloc4->Add (Vector(741.7345854158497,495.3055063938846, 0.0));
    positionAlloc4->Add (Vector(603.8967522401051,352.97082526265075, 0.0));
    positionAlloc4->Add (Vector(393.946743772202,725.8592858972697, 0.0));
    positionAlloc4->Add (Vector(728.825263677913,203.27073908335365, 0.0));
    positionAlloc4->Add (Vector(426.93222605528786,510.3434954003175, 0.0));
    positionAlloc4->Add (Vector(492.4259730829254,217.18298768727908, 0.0));
    positionAlloc4->Add (Vector(251.8776719258524,574.2870722180844, 0.0));
    positionAlloc4->Add (Vector(546.6592543214437,340.1192465062784, 0.0));
    positionAlloc4->Add (Vector(494.2557860438598,663.2948071073561, 0.0));
    positionAlloc4->Add (Vector(488.32275508657733,551.7617255404687, 0.0));
    positionAlloc4->Add (Vector(446.18273828341125,782.32026729633, 0.0));
    positionAlloc4->Add (Vector(556.8035404812823,720.3380769429174, 0.0));
    positionAlloc4->Add (Vector(184.49614101342144,815.3095638630723, 0.0));
    positionAlloc4->Add (Vector(277.3589812239036,376.75489625462365, 0.0));
    positionAlloc4->Add (Vector(550.7958177283822,385.0980211025825, 0.0));
    positionAlloc4->Add (Vector(312.49002147217413,183.3286648992417, 0.0));
    positionAlloc4->Add (Vector(392.32971539954724,518.1250780052999, 0.0));
    positionAlloc4->Add (Vector(290.8288995166562,516.2526139771046, 0.0));
    positionAlloc4->Add (Vector(571.2351094773285,849.0535194539546, 0.0));
    positionAlloc4->Add (Vector(300.5911584972754,536.1756946268476, 0.0));
    positionAlloc4->Add (Vector(628.7862327921325,824.3753648588944, 0.0));
    positionAlloc4->Add (Vector(432.09983133752155,655.7878676313094, 0.0));
    positionAlloc4->Add (Vector(598.9567734577115,691.894261095199, 0.0));
    positionAlloc4->Add (Vector(276.19007521286323,588.8634986692284, 0.0));
    positionAlloc4->Add (Vector(606.4797850366238,186.9483453609838, 0.0));
    positionAlloc4->Add (Vector(607.1914692027592,455.7057907089666, 0.0));
    positionAlloc4->Add (Vector(415.9366813768842,713.6894180450756, 0.0));
    positionAlloc4->Add (Vector(180.34193394912805,405.0987679346758, 0.0));
    positionAlloc4->Add (Vector(264.0412733357035,339.6202385184623, 0.0));
    positionAlloc4->Add (Vector(412.03594407213734,744.0680729330968, 0.0));
    positionAlloc4->Add (Vector(616.6532259894942,722.5118445133307, 0.0));
    positionAlloc4->Add (Vector(639.0188007089275,530.062609200289, 0.0));
    positionAlloc4->Add (Vector(183.70555242124064,503.4726432595174, 0.0));
    positionAlloc4->Add (Vector(661.6731957127813,745.3654767234822, 0.0));
    positionAlloc4->Add (Vector(223.94999420916932,837.237145838576, 0.0));
    positionAlloc4->Add (Vector(777.1068574105076,671.1418729644864, 0.0));
    positionAlloc4->Add (Vector(377.2320696180342,277.94173186444743, 0.0));
    positionAlloc4->Add (Vector(847.5654503472706,359.17416436799226, 0.0));
    positionAlloc4->Add (Vector(516.7381736930781,575.5917618375763, 0.0));
    positionAlloc4->Add (Vector(782.0432812631377,785.9123439832811, 0.0));
    positionAlloc4->Add (Vector(672.7936849072827,506.8058938965367, 0.0));
    positionAlloc4->Add (Vector(150.1767181433935,684.2220699268172, 0.0));
    positionAlloc4->Add (Vector(720.910886361038,589.2119268718595, 0.0));
    positionAlloc4->Add (Vector(597.8233984263757,523.6273244156503, 0.0));
    positionAlloc4->Add (Vector(812.7528437076113,385.2767523544227, 0.0));
    positionAlloc4->Add (Vector(258.8780640503411,824.5810161696558, 0.0));
    positionAlloc4->Add (Vector(261.8498934794993,418.1268452927833, 0.0));
    positionAlloc4->Add (Vector(493.7313752406641,213.99158438016238, 0.0));
    positionAlloc4->Add (Vector(844.3121812776177,188.23507554232674, 0.0));
    positionAlloc4->Add (Vector(606.0332615503489,511.6734748237394, 0.0));
    positionAlloc4->Add (Vector(680.9886599267537,532.3011293243641, 0.0));
    positionAlloc4->Add (Vector(168.39934063632495,465.434733551193, 0.0));
    positionAlloc4->Add (Vector(579.3004756831908,719.6238068972833, 0.0));
    positionAlloc4->Add (Vector(593.5968203961777,417.84880854590006, 0.0));
    positionAlloc4->Add (Vector(436.5400746802612,613.103718401841, 0.0));
    positionAlloc4->Add (Vector(371.4254399066365,262.66884121971043, 0.0));
    positionAlloc4->Add (Vector(675.1083766564452,591.8219428063048, 0.0));
    positionAlloc4->Add (Vector(225.93688029649098,822.8954696571705, 0.0));
    positionAlloc4->Add (Vector(457.38104199784766,626.1393864387328, 0.0));
    positionAlloc4->Add (Vector(538.1186595066474,592.3530739960112, 0.0));
    positionAlloc4->Add (Vector(735.095924825968,531.6335803632076, 0.0));
    positionAlloc4->Add (Vector(274.39312283776985,700.9860040576093, 0.0));
    positionAlloc4->Add (Vector(653.7827172317482,228.08493514280013, 0.0));
    positionAlloc4->Add (Vector(486.5469925643793,797.2206932951989, 0.0));
    positionAlloc4->Add (Vector(760.7965628505743,532.8543967137475, 0.0));
    positionAlloc4->Add (Vector(587.645825220051,493.28725279576366, 0.0));
    positionAlloc4->Add (Vector(765.8368565366957,458.219083026334, 0.0));
    positionAlloc4->Add (Vector(300.4275035019623,231.5239242241425, 0.0));
    positionAlloc4->Add (Vector(564.47504942973,395.5272503245485, 0.0));
    positionAlloc4->Add (Vector(317.31794576256846,408.3102895421618, 0.0));
    positionAlloc4->Add (Vector(578.432955760429,449.02681368586025, 0.0));
    positionAlloc4->Add (Vector(361.4342058425381,649.7687962694362, 0.0));
    positionAlloc4->Add (Vector(357.28199142833626,770.5052387487733, 0.0));
    positionAlloc4->Add (Vector(386.6810401616939,235.8334543202069, 0.0));
    positionAlloc4->Add (Vector(217.93789168836437,452.8620753835018, 0.0));
    positionAlloc4->Add (Vector(426.5874193421971,371.4337364319865, 0.0));
    positionAlloc4->Add (Vector(171.6076927797227,265.9642172334889, 0.0));
    positionAlloc4->Add (Vector(840.8137928592043,361.57457643923823, 0.0));
    positionAlloc4->Add (Vector(826.3933763281772,507.2627846402035, 0.0));
    positionAlloc4->Add (Vector(417.5791402594832,603.3049435648013, 0.0));
    positionAlloc4->Add (Vector(177.52364136793128,452.23727673899054, 0.0));
    positionAlloc4->Add (Vector(410.6965873225049,702.9381901559635, 0.0));
    positionAlloc4->Add (Vector(447.0637369097147,529.8614974955974, 0.0));
    positionAlloc4->Add (Vector(207.5411126373536,379.85044959311347, 0.0));
    positionAlloc4->Add (Vector(720.9352397282706,275.446894692718, 0.0));
    positionAlloc4->Add (Vector(349.4777943041181,414.59168134697313, 0.0));
    positionAlloc4->Add (Vector(389.7307893650802,547.2074758851311, 0.0));
    positionAlloc4->Add (Vector(660.6107918285849,316.2960903684394, 0.0));
    positionAlloc4->Add (Vector(487.045574984576,268.5003471909526, 0.0));
    positionAlloc4->Add (Vector(307.1363508629098,197.9172185982299, 0.0));
    positionAlloc4->Add (Vector(268.7368651221007,638.1506467836393, 0.0));
    positionAlloc4->Add (Vector(180.25395607847005,736.4780039465206, 0.0));
    positionAlloc4->Add (Vector(322.0626993945061,621.7057757437817, 0.0));
    positionAlloc4->Add (Vector(622.3575007210891,677.4147971694739, 0.0));
    positionAlloc4->Add (Vector(202.3576535978575,841.9672671281451, 0.0));
    positionAlloc4->Add (Vector(165.149256225765,699.423976264695, 0.0));
    positionAlloc4->Add (Vector(742.6905063452807,260.64291020415453, 0.0));
    positionAlloc4->Add (Vector(784.6681702306331,436.0234795062681, 0.0));
    positionAlloc4->Add (Vector(728.680412570016,750.2540179700793, 0.0));
    positionAlloc4->Add (Vector(215.232150180619,806.4546007507471, 0.0));
    positionAlloc4->Add (Vector(259.7835899274541,634.8041655778339, 0.0));
    positionAlloc4->Add (Vector(426.39757208257487,328.372336551067, 0.0));
    positionAlloc4->Add (Vector(627.8889533430859,424.96540186677714, 0.0));
    positionAlloc4->Add (Vector(636.9096220124809,642.8697790782151, 0.0));
    positionAlloc4->Add (Vector(360.43439189894946,612.8079888033651, 0.0));
    positionAlloc4->Add (Vector(714.1505563116259,331.9415317004066, 0.0));
    positionAlloc4->Add (Vector(353.61581257095355,650.8156745543868, 0.0));
    positionAlloc4->Add (Vector(460.75355479217876,567.8722007918695, 0.0));
    positionAlloc4->Add (Vector(338.6069435236642,569.1063387397113, 0.0));
    positionAlloc4->Add (Vector(514.9763153840545,628.2936272956729, 0.0));
    positionAlloc4->Add (Vector(321.5678422700453,650.3055412665984, 0.0));
    positionAlloc4->Add (Vector(618.9644267021561,330.4583635148881, 0.0));
    positionAlloc4->Add (Vector(659.9956481432896,391.4879919871148, 0.0));
    positionAlloc4->Add (Vector(826.911192159281,577.9315273119969, 0.0));
    positionAlloc4->Add (Vector(544.7471205446743,431.71862479527874, 0.0));
    positionAlloc4->Add (Vector(645.7061814184208,695.0722654259091, 0.0));
    positionAlloc4->Add (Vector(375.64856538432343,606.4102307373919, 0.0));
    positionAlloc4->Add (Vector(497.3646217089658,780.1939189893842, 0.0));
    positionAlloc4->Add (Vector(317.08221700190836,289.46931500923273, 0.0));
    positionAlloc4->Add (Vector(765.5285493110765,777.1541474043315, 0.0));
    positionAlloc4->Add (Vector(451.2748326112528,467.6865168798863, 0.0));
    positionAlloc4->Add (Vector(557.3560497958233,707.0094004788868, 0.0));
    positionAlloc4->Add (Vector(181.85296645320727,815.6777305166795, 0.0));
    positionAlloc4->Add (Vector(471.4661739548636,831.7585366667472, 0.0));
    positionAlloc4->Add (Vector(439.1380435673786,487.7328110924828, 0.0));
    positionAlloc4->Add (Vector(581.7868637489694,270.81005583449047, 0.0));
    positionAlloc4->Add (Vector(325.96180510328963,366.5287419517683, 0.0));
    positionAlloc4->Add (Vector(391.6102248951861,162.31294134876723, 0.0));
    positionAlloc4->Add (Vector(178.54129203896002,620.5900553439348, 0.0));
    positionAlloc4->Add (Vector(761.2115011679786,293.87692948684054, 0.0));
    positionAlloc4->Add (Vector(518.5489691297689,511.3631685550804, 0.0));
    positionAlloc4->Add (Vector(201.26205299406374,569.907581208533, 0.0));
    positionAlloc4->Add (Vector(520.4788786655251,814.258091898982, 0.0));
    positionAlloc4->Add (Vector(258.2727121895389,487.18143806307654, 0.0));
    positionAlloc4->Add (Vector(211.7977490094415,291.35860515838783, 0.0));
    positionAlloc4->Add (Vector(439.3187929807772,444.162842961022, 0.0));
    positionAlloc4->Add (Vector(561.66495405977,679.4064467022756, 0.0));
    positionAlloc4->Add (Vector(452.3913146976101,189.70059679755786, 0.0));
    positionAlloc4->Add (Vector(608.1655883076594,328.05167716446726, 0.0));
    positionAlloc4->Add (Vector(161.78332390239302,423.20237058753366, 0.0));
    positionAlloc4->Add (Vector(750.2871889699393,412.2571641572827, 0.0));
    positionAlloc4->Add (Vector(402.85591466400035,332.34130534014434, 0.0));
    positionAlloc4->Add (Vector(394.52892518313786,740.43774497, 0.0));
    positionAlloc4->Add (Vector(190.59830073089591,558.4706842061393, 0.0));
    positionAlloc4->Add (Vector(510.67875487880383,838.1115014943039, 0.0));
    positionAlloc4->Add (Vector(156.65434169267394,821.3059105261083, 0.0));
    positionAlloc4->Add (Vector(206.0289268640084,678.8493480877511, 0.0));
    positionAlloc4->Add (Vector(543.7142588899931,236.6324662588077, 0.0));
    positionAlloc4->Add (Vector(401.98146272366864,161.22548002145203, 0.0));
    positionAlloc4->Add (Vector(391.5144666273266,515.6026313178204, 0.0));
    positionAlloc4->Add (Vector(676.2019290349384,672.8822232807394, 0.0));
    positionAlloc4->Add (Vector(150.99800980127284,662.2404175608912, 0.0));
    positionAlloc4->Add (Vector(530.1087570100419,418.45131678113495, 0.0));
    positionAlloc4->Add (Vector(391.27119870878346,394.9499736399854, 0.0));
    positionAlloc4->Add (Vector(602.8021568120552,829.2056129241266, 0.0));
    positionAlloc4->Add (Vector(672.5218314552443,799.3299646036396, 0.0));
    positionAlloc4->Add (Vector(280.85474152917345,595.8605027988152, 0.0));
    positionAlloc4->Add (Vector(644.7910502003688,806.2270314696211, 0.0));
    positionAlloc4->Add (Vector(288.30666104533395,154.54165864636852, 0.0));
    positionAlloc4->Add (Vector(784.1915343812406,314.171610389692, 0.0));
    positionAlloc4->Add (Vector(821.1492233075218,691.282806455992, 0.0));
    positionAlloc4->Add (Vector(768.7000845199715,787.4744179428133, 0.0));
    positionAlloc4->Add (Vector(319.6911387814047,566.148856024831, 0.0));
    positionAlloc4->Add (Vector(450.7905649302119,660.3263366741319, 0.0));
    positionAlloc4->Add (Vector(285.60115959569225,501.60608608564854, 0.0));
    positionAlloc4->Add (Vector(175.3293582730476,320.5627087792914, 0.0));
    positionAlloc4->Add (Vector(590.0790333972234,790.1701345458047, 0.0));
    positionAlloc4->Add (Vector(408.0036856866963,440.63834739933645, 0.0));
    positionAlloc4->Add (Vector(355.46956791756475,254.5439152236314, 0.0));
    positionAlloc4->Add (Vector(594.1360512214362,563.7434546342924, 0.0));
    positionAlloc4->Add (Vector(420.0987637052153,387.7749064104578, 0.0));
    positionAlloc4->Add (Vector(440.8374606897223,591.7099354531717, 0.0));
    positionAlloc4->Add (Vector(501.85453423559755,609.2959308574139, 0.0));
    positionAlloc4->Add (Vector(315.37287740058025,785.8677831598848, 0.0));
    positionAlloc4->Add (Vector(331.0331399173333,377.95134386492487, 0.0));
    positionAlloc4->Add (Vector(719.1358450003157,741.3639669252003, 0.0));
    positionAlloc4->Add (Vector(374.25337934461436,287.0486648022834, 0.0));
    positionAlloc4->Add (Vector(175.0422834998055,162.1057830369578, 0.0));
    positionAlloc4->Add (Vector(844.9509127845105,585.8136285597664, 0.0));
    positionAlloc4->Add (Vector(276.56744452634473,602.7424727573527, 0.0));
    positionAlloc4->Add (Vector(715.6772426300404,474.65121902874716, 0.0));
    positionAlloc4->Add (Vector(354.1572297762893,704.0728330668244, 0.0));
    positionAlloc4->Add (Vector(273.58793456998836,594.4965491546466, 0.0));
    positionAlloc4->Add (Vector(298.3713631805815,522.424280783373, 0.0));
    positionAlloc4->Add (Vector(702.8935359397432,270.14451219076113, 0.0));
    positionAlloc4->Add (Vector(799.4397688847735,590.2918710570466, 0.0));
    positionAlloc4->Add (Vector(417.06938626221785,664.2581484971447, 0.0));
    positionAlloc4->Add (Vector(627.3982130965135,386.23228652567116, 0.0));
    positionAlloc4->Add (Vector(819.0682202220621,556.1619009419692, 0.0));
    positionAlloc4->Add (Vector(280.9758960420771,697.5358252325168, 0.0));
    positionAlloc4->Add (Vector(796.4332264728686,569.5102723114608, 0.0));
    positionAlloc4->Add (Vector(457.43451597786685,700.7576963452032, 0.0));
    positionAlloc4->Add (Vector(164.17456585306448,653.5014601065961, 0.0));
    positionAlloc4->Add (Vector(665.5900420788912,211.21510955192758, 0.0));
    positionAlloc4->Add (Vector(186.14925805592964,847.3694690319145, 0.0));
    positionAlloc4->Add (Vector(217.82451281491768,315.1306066888019, 0.0));
    positionAlloc4->Add (Vector(387.1211459705705,438.23518515846416, 0.0));
    positionAlloc4->Add (Vector(633.3546424077952,182.1440392638841, 0.0));
    positionAlloc4->Add (Vector(423.0792494037532,620.7970365662361, 0.0));
    positionAlloc4->Add (Vector(529.7991423856474,458.1532810797014, 0.0));
    positionAlloc4->Add (Vector(361.9415146496847,290.2522128024152, 0.0));
    positionAlloc4->Add (Vector(500.71440041093757,573.0530324728434, 0.0));
    positionAlloc4->Add (Vector(466.6199223344343,664.1130944590816, 0.0));
    positionAlloc4->Add (Vector(226.1651731610459,802.4910513108146, 0.0));
    positionAlloc4->Add (Vector(650.1473069490446,538.5596753309738, 0.0));
    positionAlloc4->Add (Vector(653.642615181586,346.1857101220998, 0.0));
    positionAlloc4->Add (Vector(256.3737681147506,176.6921298640309, 0.0));
    positionAlloc4->Add (Vector(208.50755540024898,725.837449374992, 0.0));
    positionAlloc4->Add (Vector(250.64962111133454,782.1091274176647, 0.0));
    positionAlloc4->Add (Vector(182.00863033359087,811.7506941195728, 0.0));
    positionAlloc4->Add (Vector(377.4206245368531,511.93384828309325, 0.0));
    positionAlloc4->Add (Vector(752.0117270366818,401.74572022530174, 0.0));
    positionAlloc4->Add (Vector(836.4144866633511,713.1911001280411, 0.0));
    positionAlloc4->Add (Vector(241.73902039516724,651.6636926935878, 0.0));
    positionAlloc4->Add (Vector(378.2164165945894,601.873793616104, 0.0));
    positionAlloc4->Add (Vector(498.4227718694632,449.63345435945223, 0.0));
    positionAlloc4->Add (Vector(827.4280263611851,221.05270081247482, 0.0));
    positionAlloc4->Add (Vector(632.4988572256754,386.6776445152925, 0.0));
    positionAlloc4->Add (Vector(805.51249596823,352.61443601305854, 0.0));
    positionAlloc4->Add (Vector(846.6274844374572,202.91711261312088, 0.0));
    positionAlloc4->Add (Vector(308.39783910556736,832.7789343236582, 0.0));
    positionAlloc4->Add (Vector(694.9600063967902,689.0781565611998, 0.0));
    positionAlloc4->Add (Vector(592.2153237763875,685.25472811325, 0.0));
    positionAlloc4->Add (Vector(647.0776243343491,189.34669050635551, 0.0));
    positionAlloc4->Add (Vector(845.8433883681772,526.1494833410209, 0.0));
    positionAlloc4->Add (Vector(802.3457049799109,589.556245309622, 0.0));
    positionAlloc4->Add (Vector(541.5814811287686,182.04343760010792, 0.0));
    positionAlloc4->Add (Vector(619.7087323914091,601.9797276219686, 0.0));
    positionAlloc4->Add (Vector(368.5622877345146,786.6769989393721, 0.0));
    positionAlloc4->Add (Vector(248.58339935710862,628.6353773444631, 0.0));
    positionAlloc4->Add (Vector(464.76316417904576,346.30090310150376, 0.0));
    positionAlloc4->Add (Vector(749.3047008171591,786.6982603412113, 0.0));
    positionAlloc4->Add (Vector(544.6722364026198,260.21707003289265, 0.0));
    positionAlloc4->Add (Vector(552.7590453780018,465.50258987250317, 0.0));
    positionAlloc4->Add (Vector(848.1244281478394,347.52505195446315, 0.0));
    positionAlloc4->Add (Vector(745.9984760905043,809.7204869733074, 0.0));
    positionAlloc4->Add (Vector(185.25409929537705,607.3379470931676, 0.0));
    positionAlloc4->Add (Vector(457.7320610062054,604.7066612324872, 0.0));
    positionAlloc4->Add (Vector(483.36296946201367,841.4413324959856, 0.0));
    positionAlloc4->Add (Vector(444.8608135798472,733.966900748448, 0.0));
    positionAlloc4->Add (Vector(576.0002070142116,421.72670073197537, 0.0));
    positionAlloc4->Add (Vector(529.4293891877642,571.1690152345157, 0.0));
    positionAlloc4->Add (Vector(326.8955017963874,288.4982547743651, 0.0));
    positionAlloc4->Add (Vector(361.53371884963633,197.0296375667309, 0.0));
    positionAlloc4->Add (Vector(559.15422518832,457.7496243673926, 0.0));
    positionAlloc4->Add (Vector(546.0095379410046,358.2594535796445, 0.0));
    positionAlloc4->Add (Vector(833.082322847895,540.4976943911367, 0.0));
    positionAlloc4->Add (Vector(398.26957862721736,771.1852317782744, 0.0));
    positionAlloc4->Add (Vector(699.7062667058713,637.9989453446793, 0.0));
    positionAlloc4->Add (Vector(485.09117732324506,222.80720840969852, 0.0));
    positionAlloc4->Add (Vector(775.8753188785632,825.206130061157, 0.0));
    positionAlloc4->Add (Vector(466.3010655939683,284.68772652099756, 0.0));
    positionAlloc4->Add (Vector(811.2963355777886,228.6832978945642, 0.0));
    positionAlloc4->Add (Vector(452.0880029671751,598.624165386045, 0.0));
    positionAlloc4->Add (Vector(293.9930064835989,483.8337978725926, 0.0));
    positionAlloc4->Add (Vector(785.3029927742865,189.8670071069768, 0.0));
    positionAlloc4->Add (Vector(672.4608723546477,801.1780239895393, 0.0));
    positionAlloc4->Add (Vector(785.9668213503586,458.95729222385273, 0.0));
    positionAlloc4->Add (Vector(772.1094099862003,416.3962930630514, 0.0));
    positionAlloc4->Add (Vector(504.6982347529588,519.6980400916276, 0.0));
    positionAlloc4->Add (Vector(315.3203877380141,413.8956804473184, 0.0));
    positionAlloc4->Add (Vector(731.0818379156858,625.6863035996295, 0.0));
    positionAlloc4->Add (Vector(357.9538704575309,313.84374480045244, 0.0));
    positionAlloc4->Add (Vector(805.9382434960428,388.655116219937, 0.0));
    positionAlloc4->Add (Vector(727.7901063016675,768.731270472076, 0.0));
    positionAlloc4->Add (Vector(386.5309980425442,848.7223165247256, 0.0));
    positionAlloc4->Add (Vector(428.5722635115804,310.9476144818924, 0.0));
    positionAlloc4->Add (Vector(182.51417210003308,260.9790749141273, 0.0));
    positionAlloc4->Add (Vector(480.40523748213053,709.0746741353424, 0.0));
    positionAlloc4->Add (Vector(694.9747597023838,379.8185914797246, 0.0));
    positionAlloc4->Add (Vector(529.5189373120004,162.11008591867886, 0.0));
    positionAlloc4->Add (Vector(601.8278978627174,181.34066076266654, 0.0));
    positionAlloc4->Add (Vector(480.35654998952106,746.9157220123521, 0.0));
    positionAlloc4->Add (Vector(495.6386439608631,423.44845836303733, 0.0));
    positionAlloc4->Add (Vector(765.7955876828372,835.8622141729081, 0.0));
    positionAlloc4->Add (Vector(633.0919435558245,425.05611351946465, 0.0));
    positionAlloc4->Add (Vector(346.93701023194575,566.7236820480266, 0.0));
    positionAlloc4->Add (Vector(601.8433774164471,812.3215933574418, 0.0));
    positionAlloc4->Add (Vector(550.3878797408088,759.7100011756245, 0.0));
    positionAlloc4->Add (Vector(679.7458648849762,735.8575508037351, 0.0));
    positionAlloc4->Add (Vector(320.41194382012134,843.5075386733236, 0.0));
    positionAlloc4->Add (Vector(539.6869695745972,323.5129006267655, 0.0));
    positionAlloc4->Add (Vector(694.9278379406295,198.87141103989336, 0.0));
    positionAlloc4->Add (Vector(335.4350354484741,203.47532175671398, 0.0));
    positionAlloc4->Add (Vector(763.1537478117274,273.2469252263603, 0.0));
    positionAlloc4->Add (Vector(825.2798895127892,762.8896580447243, 0.0));
    positionAlloc4->Add (Vector(337.4261658776792,550.329277362259, 0.0));
    positionAlloc4->Add (Vector(215.02980014588357,194.21741163212175, 0.0));
    positionAlloc4->Add (Vector(201.79221894133354,292.3026888051693, 0.0));
    positionAlloc4->Add (Vector(187.3628459240826,631.9585318866941, 0.0));
    positionAlloc4->Add (Vector(235.0098310098625,347.07452807482605, 0.0));
    positionAlloc4->Add (Vector(403.8318661098234,239.44806820169026, 0.0));
    positionAlloc4->Add (Vector(454.15188560351305,167.1252919312534, 0.0));
    positionAlloc4->Add (Vector(397.80346022686984,847.7084807404132, 0.0));
    positionAlloc4->Add (Vector(738.7193464301882,683.2691678131047, 0.0));
    positionAlloc4->Add (Vector(587.3176219345207,680.2688958929912, 0.0));
    positionAlloc4->Add (Vector(680.3086829033401,517.9423402196528, 0.0));
    positionAlloc4->Add (Vector(546.8653329471672,685.1030036591344, 0.0));
    positionAlloc4->Add (Vector(795.5398066849797,256.1946308527289, 0.0));
    positionAlloc4->Add (Vector(401.97306741820887,597.537134450984, 0.0));
    positionAlloc4->Add (Vector(174.82870973916658,754.2311438576394, 0.0));
    positionAlloc4->Add (Vector(226.7185092345889,776.8117196366956, 0.0));
    positionAlloc4->Add (Vector(559.1289521512291,423.3934716989222, 0.0));
    positionAlloc4->Add (Vector(417.3102265641503,716.9134905269985, 0.0));
    positionAlloc4->Add (Vector(675.2187068658038,533.623150555115, 0.0));
    positionAlloc4->Add (Vector(755.7911172519106,442.47316427437755, 0.0));
    positionAlloc4->Add (Vector(471.8757016364253,297.1295725350806, 0.0));
    positionAlloc4->Add (Vector(327.4283988700257,762.9895610597958, 0.0));
    positionAlloc4->Add (Vector(641.8893229720622,672.0259309889939, 0.0));
    positionAlloc4->Add (Vector(716.3129795866882,503.73472468400035, 0.0));
    positionAlloc4->Add (Vector(227.31018885880576,458.8041775327613, 0.0));
    positionAlloc4->Add (Vector(492.677679465908,637.463094909396, 0.0));
    positionAlloc4->Add (Vector(845.3572506174622,827.8088202621185, 0.0));
    positionAlloc4->Add (Vector(841.1208844390401,704.4134455580946, 0.0));
    positionAlloc4->Add (Vector(295.5201103435628,531.3457307590697, 0.0));
    positionAlloc4->Add (Vector(424.00521415599894,842.945043892458, 0.0));
    positionAlloc4->Add (Vector(181.26202021405663,384.58698668735735, 0.0));
    positionAlloc4->Add (Vector(515.433891319881,415.96928760318593, 0.0));
    positionAlloc4->Add (Vector(480.8653105553913,192.7963410085565, 0.0));
    positionAlloc4->Add (Vector(832.1727875636622,305.44562217897436, 0.0));
    positionAlloc4->Add (Vector(480.58442255665074,170.50559350375946, 0.0));
    positionAlloc4->Add (Vector(153.0811263026504,150.23111148163352, 0.0));
    positionAlloc4->Add (Vector(671.5561198135526,783.483817247177, 0.0));
    positionAlloc4->Add (Vector(550.8694082337847,652.03082280769, 0.0));
    positionAlloc4->Add (Vector(806.451341321153,155.31399132173988, 0.0));
    positionAlloc4->Add (Vector(825.3761878305735,550.1776282694898, 0.0));
    positionAlloc4->Add (Vector(575.9586397677426,425.4363854120383, 0.0));
    positionAlloc4->Add (Vector(670.9302750621152,522.3872599335004, 0.0));
    positionAlloc4->Add (Vector(836.000290919532,373.78698066637537, 0.0));
    positionAlloc4->Add (Vector(517.8424611465223,381.97516138756134, 0.0));
    positionAlloc4->Add (Vector(774.6983242752276,600.6183208640725, 0.0));
    positionAlloc4->Add (Vector(610.2299581993075,759.1001038862161, 0.0));
    positionAlloc4->Add (Vector(274.3338736186416,261.4172741358701, 0.0));
    positionAlloc4->Add (Vector(476.972584619989,734.4818513554231, 0.0));
    positionAlloc4->Add (Vector(640.5123162542857,748.937196323114, 0.0));
    positionAlloc4->Add (Vector(803.5315634612571,508.0605952582575, 0.0));
    positionAlloc4->Add (Vector(768.7132783901172,153.64876293384745, 0.0));
    positionAlloc4->Add (Vector(416.8225006756261,653.2145862758946, 0.0));
    positionAlloc4->Add (Vector(531.4112990000147,491.2957542952473, 0.0));
    positionAlloc4->Add (Vector(617.283790634036,403.04190097567334, 0.0));
    positionAlloc4->Add (Vector(551.6793015087001,335.71445908429325, 0.0));
    positionAlloc4->Add (Vector(538.855146628932,703.9390668137455, 0.0));
    positionAlloc4->Add (Vector(526.4395444562267,191.68089660174738, 0.0));
    positionAlloc4->Add (Vector(681.915092827153,489.0978295840793, 0.0));
    positionAlloc4->Add (Vector(255.82804353607798,717.2121961334295, 0.0));
    positionAlloc4->Add (Vector(384.2655069891814,373.6843440148846, 0.0));
    positionAlloc4->Add (Vector(813.771175005728,231.06342254318542, 0.0));
    positionAlloc4->Add (Vector(494.0439115816658,690.3923491782837, 0.0));
    positionAlloc4->Add (Vector(316.2661292936482,625.3894161806113, 0.0));
    positionAlloc4->Add (Vector(331.5741251945991,532.3735773220219, 0.0));
    positionAlloc4->Add (Vector(562.5368645653834,249.47487188280766, 0.0));
    positionAlloc4->Add (Vector(455.2344544353237,341.6585939813385, 0.0));
    positionAlloc4->Add (Vector(326.5528752878604,764.3188612129933, 0.0));
    positionAlloc4->Add (Vector(409.64639757634546,270.8484485462038, 0.0));
    positionAlloc4->Add (Vector(434.42923113270774,171.97974086891878, 0.0));
    positionAlloc4->Add (Vector(771.8545779608849,167.8831807851229, 0.0));
    positionAlloc4->Add (Vector(177.39257813870157,378.8151528357237, 0.0));
    positionAlloc4->Add (Vector(380.57112539061745,407.69351298679675, 0.0));
    positionAlloc4->Add (Vector(258.2005060323373,753.032368639886, 0.0));
    positionAlloc4->Add (Vector(160.239977443549,765.525161485542, 0.0));
    positionAlloc4->Add (Vector(316.6854499994247,446.70052591849014, 0.0));
    positionAlloc4->Add (Vector(772.9982378067924,799.6925302108107, 0.0));
    positionAlloc4->Add (Vector(725.5280340717137,695.0565057524366, 0.0));
    positionAlloc4->Add (Vector(333.9761183486721,553.0250584642174, 0.0));
    positionAlloc4->Add (Vector(735.4169125109702,435.73515727219257, 0.0));
    positionAlloc4->Add (Vector(702.7896012208603,223.33078167694862, 0.0));
    positionAlloc4->Add (Vector(803.1104111912944,678.9998935207593, 0.0));
    positionAlloc4->Add (Vector(293.08264233587533,322.38600678655, 0.0));
    positionAlloc4->Add (Vector(803.7678922566666,250.93262473430713, 0.0));
    positionAlloc4->Add (Vector(287.0775341986104,241.43677680850425, 0.0));
    positionAlloc4->Add (Vector(335.4516203556842,304.88899903811665, 0.0));
    positionAlloc4->Add (Vector(377.5360665267113,312.94577910623525, 0.0));
    positionAlloc4->Add (Vector(178.74646162262812,200.07717532326348, 0.0));
    positionAlloc4->Add (Vector(626.1859401100022,773.5683260196621, 0.0));
    positionAlloc4->Add (Vector(343.8093618756096,428.376794601524, 0.0));
    positionAlloc4->Add (Vector(748.0960722293328,561.5819738592439, 0.0));
    positionAlloc4->Add (Vector(176.14614437106897,500.1851552342233, 0.0));
    positionAlloc4->Add (Vector(508.27943246378817,803.7069278959239, 0.0));
    positionAlloc4->Add (Vector(631.3235706956552,394.48543409334724, 0.0));
    positionAlloc4->Add (Vector(675.0588524640635,293.115928362771, 0.0));
    positionAlloc4->Add (Vector(369.86331866350463,276.3381056476929, 0.0));
    positionAlloc4->Add (Vector(173.29988448441986,501.99059745019485, 0.0));
    positionAlloc4->Add (Vector(201.3215164845358,306.36000647915824, 0.0));
    positionAlloc4->Add (Vector(267.5152307579924,156.76638668312762, 0.0));
    positionAlloc4->Add (Vector(471.54245992913894,785.3416673342949, 0.0));
    positionAlloc4->Add (Vector(605.9802510906418,524.4333489328565, 0.0));
    positionAlloc4->Add (Vector(378.0376837503579,174.56132785141, 0.0));
    positionAlloc4->Add (Vector(762.1110717443116,389.2303216089939, 0.0));
    positionAlloc4->Add (Vector(709.9239129881719,797.7296377049146, 0.0));
    positionAlloc4->Add (Vector(544.6023756833324,203.80697236722838, 0.0));
    positionAlloc4->Add (Vector(561.6780993189217,329.5728323276849, 0.0));
    positionAlloc4->Add (Vector(209.18033476608613,839.7413030811175, 0.0));
    positionAlloc4->Add (Vector(166.7971620600511,496.21037584185206, 0.0));
    positionAlloc4->Add (Vector(745.6344255056998,470.92732710200664, 0.0));
    positionAlloc4->Add (Vector(311.44425196298937,353.9815864017714, 0.0));
    positionAlloc4->Add (Vector(260.5624606952377,782.9145787692524, 0.0));
    positionAlloc4->Add (Vector(273.80424846574095,613.4891441979742, 0.0));
    positionAlloc4->Add (Vector(254.87140023227232,460.9113506187502, 0.0));
    positionAlloc4->Add (Vector(655.3467789502533,691.4634331694722, 0.0));
    positionAlloc4->Add (Vector(181.83456591422586,562.4406347346971, 0.0));
    positionAlloc4->Add (Vector(376.1621231061866,405.94951389294795, 0.0));
    positionAlloc4->Add (Vector(553.5218995214638,803.7254067049043, 0.0));
    positionAlloc4->Add (Vector(808.9979702725158,774.3077430669282, 0.0));
    positionAlloc4->Add (Vector(781.109938346904,496.83009414573337, 0.0));
    positionAlloc4->Add (Vector(711.7838123762057,545.8723138468404, 0.0));
    positionAlloc4->Add (Vector(466.66192391631046,239.36664448761374, 0.0));
    positionAlloc4->Add (Vector(661.5336112973096,359.35614815878796, 0.0));
    positionAlloc4->Add (Vector(330.63072582570084,201.07190473906837, 0.0));
    positionAlloc4->Add (Vector(358.4038936409619,542.520237210792, 0.0));
    positionAlloc4->Add (Vector(208.56438703582822,341.3186510576037, 0.0));
    positionAlloc4->Add (Vector(704.8110120234979,552.2494494352873, 0.0));
    positionAlloc4->Add (Vector(197.9681913178917,757.5957533352773, 0.0));
    positionAlloc4->Add (Vector(195.41154648457507,284.60420626175585, 0.0));
    positionAlloc4->Add (Vector(649.0711708916683,804.7395969778337, 0.0));
    positionAlloc4->Add (Vector(410.96699244224385,625.7731072434477, 0.0));
    positionAlloc4->Add (Vector(575.8083296193278,650.7472487501439, 0.0));
    positionAlloc4->Add (Vector(229.42235106733835,187.39167035415744, 0.0));
    positionAlloc4->Add (Vector(770.117916300221,314.2784730196752, 0.0));
    positionAlloc4->Add (Vector(248.7347831748881,724.755004212795, 0.0));
    positionAlloc4->Add (Vector(454.4713744458518,228.3810793408953, 0.0));
    positionAlloc4->Add (Vector(548.1095591537119,466.14567927825647, 0.0));
    positionAlloc4->Add (Vector(495.7824918136862,343.4873787655723, 0.0));
    positionAlloc4->Add (Vector(242.25066804387717,235.56393499816824, 0.0));
    positionAlloc4->Add (Vector(406.2093742881058,766.243737096393, 0.0));
    positionAlloc4->Add (Vector(260.02773012586147,297.3235519994024, 0.0));
    positionAlloc4->Add (Vector(289.8746442389549,717.7112771838591, 0.0));
    positionAlloc4->Add (Vector(709.0291111464157,470.9841400355801, 0.0));
    positionAlloc4->Add (Vector(389.08368184622344,821.5267331557496, 0.0));
    positionAlloc4->Add (Vector(325.38224125178976,384.1123308148273, 0.0));
    positionAlloc4->Add (Vector(602.1539293606568,455.54114451539493, 0.0));
    positionAlloc4->Add (Vector(326.98534215235304,424.3867790348721, 0.0));
    positionAlloc4->Add (Vector(219.26661118057444,627.8492610115425, 0.0));
    positionAlloc4->Add (Vector(173.23754027382893,847.7338094904864, 0.0));
    positionAlloc4->Add (Vector(791.7763044163942,599.9193302809975, 0.0));
    positionAlloc4->Add (Vector(835.7842225050799,603.3617099482062, 0.0));
    positionAlloc4->Add (Vector(308.5224226306384,778.3936585308063, 0.0));
    positionAlloc4->Add (Vector(409.7837838531124,217.6828014364201, 0.0));
    positionAlloc4->Add (Vector(535.536008460507,325.3509972871577, 0.0));
    positionAlloc4->Add (Vector(592.3874650883143,581.0528592860871, 0.0));
    positionAlloc4->Add (Vector(245.35944482291433,276.75649161555873, 0.0));
    positionAlloc4->Add (Vector(536.253044505647,194.80005792667083, 0.0));
    positionAlloc4->Add (Vector(224.90747082309463,354.8175105367599, 0.0));
    positionAlloc4->Add (Vector(231.92903788191046,161.2411758760796, 0.0));
    positionAlloc4->Add (Vector(615.533678667792,336.76264746457593, 0.0));
    positionAlloc4->Add (Vector(786.6339758575234,735.8324117216583, 0.0));
    positionAlloc4->Add (Vector(369.1582185459464,418.91212816473717, 0.0));
    positionAlloc4->Add (Vector(469.7714283444239,728.0974040372754, 0.0));
    positionAlloc4->Add (Vector(216.96451049318915,176.54186085154333, 0.0));
    positionAlloc4->Add (Vector(598.6799268854963,192.42724998246945, 0.0));
    positionAlloc4->Add (Vector(480.1078172659628,695.4435803172184, 0.0));
    positionAlloc4->Add (Vector(792.4569382774758,616.7292429151641, 0.0));
    positionAlloc4->Add (Vector(432.5715603368195,648.8628579527222, 0.0));
    positionAlloc4->Add (Vector(387.7562915118688,449.7046093946464, 0.0));
    positionAlloc4->Add (Vector(329.95727529595774,675.723073076326, 0.0));
    positionAlloc4->Add (Vector(504.3762141481288,404.1899099922504, 0.0));
    positionAlloc4->Add (Vector(795.886488768057,413.3155773017544, 0.0));
    positionAlloc4->Add (Vector(681.3636729772189,699.0062933521085, 0.0));
    positionAlloc4->Add (Vector(425.2725253544264,302.0634322147043, 0.0));
    positionAlloc4->Add (Vector(485.5276944400608,656.124101595557, 0.0));
    positionAlloc4->Add (Vector(795.4843051728526,185.68182505416297, 0.0));
    positionAlloc4->Add (Vector(687.2546867767549,199.87687972495547, 0.0));
    positionAlloc4->Add (Vector(431.22164379748494,199.63554563228922, 0.0));
    positionAlloc4->Add (Vector(403.1835679101771,666.751760095424, 0.0));
    positionAlloc4->Add (Vector(163.80381254301878,680.4684223244811, 0.0));
    positionAlloc4->Add (Vector(150.21681234050416,767.5967665317681, 0.0));
    positionAlloc4->Add (Vector(219.56794526560208,748.3839058661353, 0.0));
    positionAlloc4->Add (Vector(204.67301281463898,810.2141367223076, 0.0));
    positionAlloc4->Add (Vector(562.342107467704,534.7789790972654, 0.0));
    positionAlloc4->Add (Vector(443.02362997034055,181.00680169789467, 0.0));
    positionAlloc4->Add (Vector(297.7391224867954,452.7383631108619, 0.0));
    positionAlloc4->Add (Vector(436.35322419626686,169.96294573790482, 0.0));
    positionAlloc4->Add (Vector(359.07878866989404,233.47143792348282, 0.0));
    positionAlloc4->Add (Vector(585.3182130184532,497.2230229589904, 0.0));
    positionAlloc4->Add (Vector(746.6450816786838,645.2769427302716, 0.0));
    positionAlloc4->Add (Vector(349.68456335907297,514.2605515086775, 0.0));
    positionAlloc4->Add (Vector(521.3241000285707,208.23557379605506, 0.0));
    positionAlloc4->Add (Vector(274.0653384406474,627.5604873036089, 0.0));
    positionAlloc4->Add (Vector(312.38948948858183,782.6074442616654, 0.0));
    positionAlloc4->Add (Vector(801.0187865217137,232.53441827419897, 0.0));
    positionAlloc4->Add (Vector(683.0133074889848,702.2840314966056, 0.0));
    positionAlloc4->Add (Vector(152.5526919492734,681.7981622372772, 0.0));
    positionAlloc4->Add (Vector(724.0194755857975,156.63259188318756, 0.0));
    positionAlloc4->Add (Vector(790.3706044338594,374.4653083559949, 0.0));
    positionAlloc4->Add (Vector(363.5764331484575,560.6501087119441, 0.0));
    positionAlloc4->Add (Vector(787.759592101834,719.1608748796738, 0.0));
    positionAlloc4->Add (Vector(288.4373393322629,723.5423041052397, 0.0));
    positionAlloc4->Add (Vector(581.4494265193175,629.5209644884319, 0.0));
    positionAlloc4->Add (Vector(583.2456493643051,188.77351101465743, 0.0));
    positionAlloc4->Add (Vector(530.974800146947,172.5374954611358, 0.0));
    positionAlloc4->Add (Vector(636.4209600650961,722.3161828660824, 0.0));
    positionAlloc4->Add (Vector(492.92589949157747,415.0834643331016, 0.0));
    positionAlloc4->Add (Vector(360.7610263256496,226.73749043100162, 0.0));
    positionAlloc4->Add (Vector(321.3682118988949,413.45943910878736, 0.0));
    positionAlloc4->Add (Vector(426.7047485654702,838.2136029379859, 0.0));
    positionAlloc4->Add (Vector(660.3722985164134,326.335785757195, 0.0));
    positionAlloc4->Add (Vector(691.9333442000274,318.02689313490487, 0.0));
    positionAlloc4->Add (Vector(168.22068487199442,315.92905624307605, 0.0));
    positionAlloc4->Add (Vector(286.63566999314787,250.30719400165407, 0.0));
    positionAlloc4->Add (Vector(716.4720857274378,724.4379151470363, 0.0));
    positionAlloc4->Add (Vector(324.31397347473956,807.4382029780335, 0.0));
    positionAlloc4->Add (Vector(778.1372758981381,351.9208452926899, 0.0));
    positionAlloc4->Add (Vector(472.35641193572513,450.44754567918983, 0.0));
    positionAlloc4->Add (Vector(734.1149701434341,676.7630623992535, 0.0));
    positionAlloc4->Add (Vector(545.7696120694371,217.25318294684928, 0.0));
    positionAlloc4->Add (Vector(435.0724920834494,168.2405201899151, 0.0));
    positionAlloc4->Add (Vector(834.3307572666273,624.5986106698092, 0.0));
    positionAlloc4->Add (Vector(449.84519866920544,475.30536933912464, 0.0));
    positionAlloc4->Add (Vector(768.6442207460105,225.9488415493069, 0.0));
    positionAlloc4->Add (Vector(576.1113378093941,763.3563051921486, 0.0));
    positionAlloc4->Add (Vector(474.2607461863202,376.7573841125011, 0.0));
    positionAlloc4->Add (Vector(713.8521778819043,661.7531172141839, 0.0));
    positionAlloc4->Add (Vector(449.6650136374606,739.8118656035673, 0.0));
    positionAlloc4->Add (Vector(849.9473787832236,454.70393204673235, 0.0));
    positionAlloc4->Add (Vector(532.4166999655093,641.8476248046788, 0.0));
    positionAlloc4->Add (Vector(760.1457393640385,366.20564070851043, 0.0));
    positionAlloc4->Add (Vector(339.0017367848367,396.93792609986906, 0.0));
    positionAlloc4->Add (Vector(544.5048399435841,664.9882537143776, 0.0));
    positionAlloc4->Add (Vector(626.6845504482346,536.8442072804357, 0.0));
    positionAlloc4->Add (Vector(698.7418501925839,630.1107186620081, 0.0));
    positionAlloc4->Add (Vector(335.2137243381104,297.03704412551366, 0.0));
    positionAlloc4->Add (Vector(565.9115505157561,710.80891811081, 0.0));
    positionAlloc4->Add (Vector(388.9164683429254,826.6304945849499, 0.0));
    positionAlloc4->Add (Vector(672.8325587488663,291.0689020427697, 0.0));
    positionAlloc4->Add (Vector(240.66862693024666,740.0806814339942, 0.0));
    positionAlloc4->Add (Vector(230.00627679966846,353.75521696295243, 0.0));
    positionAlloc4->Add (Vector(645.8512552270629,612.2705081899408, 0.0));
    positionAlloc4->Add (Vector(609.7558032410222,577.1742848028634, 0.0));
    positionAlloc4->Add (Vector(227.85160250155076,172.2892211836685, 0.0));
    positionAlloc4->Add (Vector(295.5216386842011,427.59460175795664, 0.0));
    positionAlloc4->Add (Vector(227.5201841338299,352.195786332142, 0.0));
    positionAlloc4->Add (Vector(294.47071957372657,535.63884796352, 0.0));
    positionAlloc4->Add (Vector(809.5828215609928,428.7794864099081, 0.0));
    positionAlloc4->Add (Vector(706.2387537187471,675.9151069565236, 0.0));
    positionAlloc4->Add (Vector(474.33023108239115,612.1208491528698, 0.0));
    positionAlloc4->Add (Vector(266.8817115707878,552.9039879888942, 0.0));
    positionAlloc4->Add (Vector(764.8349241213109,287.11052955539617, 0.0));
    positionAlloc4->Add (Vector(761.7350671496168,549.3869984649805, 0.0));
    positionAlloc4->Add (Vector(257.90923473773904,467.22138832571767, 0.0));
    positionAlloc4->Add (Vector(510.48768258706406,375.2792557183559, 0.0));
    positionAlloc4->Add (Vector(337.04509372959836,313.27802527578393, 0.0));
    positionAlloc4->Add (Vector(708.6326079857743,299.59908239680533, 0.0));
    positionAlloc4->Add (Vector(519.0573261202771,303.5909469898327, 0.0));
    positionAlloc4->Add (Vector(794.0005843247584,785.4077401905399, 0.0));
    positionAlloc4->Add (Vector(793.1066245336814,777.7259552291453, 0.0));
    positionAlloc4->Add (Vector(394.2067052566241,713.3159453368584, 0.0));
    positionAlloc4->Add (Vector(221.35233474639904,193.90511363097187, 0.0));
    positionAlloc4->Add (Vector(493.79697498707884,364.7866914918328, 0.0));
    positionAlloc4->Add (Vector(165.64145363640384,716.1007778198078, 0.0));
    positionAlloc4->Add (Vector(600.5436516837344,320.60199251726834, 0.0));
    positionAlloc4->Add (Vector(672.2654235359588,619.5080180551063, 0.0));
    positionAlloc4->Add (Vector(183.9734155107683,670.9966575059149, 0.0));
    positionAlloc4->Add (Vector(441.0522311040772,359.9940293875978, 0.0));
    positionAlloc4->Add (Vector(524.3727154602009,567.4293235027035, 0.0));
    positionAlloc4->Add (Vector(349.0407368738553,607.7605705259484, 0.0));
    positionAlloc4->Add (Vector(334.4580154542334,781.6155814094692, 0.0));
    positionAlloc4->Add (Vector(787.8716090280099,215.27144345495086, 0.0));
    positionAlloc4->Add (Vector(211.23572402564963,609.0942640017118, 0.0));
    positionAlloc4->Add (Vector(519.2052594504596,510.187163933646, 0.0));
    positionAlloc4->Add (Vector(708.2958867741653,646.8789524439132, 0.0));
    positionAlloc4->Add (Vector(524.476693965783,241.30761817334255, 0.0));
    positionAlloc4->Add (Vector(404.75721838283005,743.666264660785, 0.0));
    positionAlloc4->Add (Vector(154.54266497089804,842.8060233264144, 0.0));
    positionAlloc4->Add (Vector(474.6752031373642,576.3844355493078, 0.0));
    positionAlloc4->Add (Vector(189.86356408891373,412.0278952289977, 0.0));
    positionAlloc4->Add (Vector(475.6215876596468,210.55648825782328, 0.0));
    positionAlloc4->Add (Vector(775.7848743181758,657.1222962759882, 0.0));
    positionAlloc4->Add (Vector(354.40987030851386,694.6879629005653, 0.0));
    positionAlloc4->Add (Vector(485.0915405955499,280.29250805766964, 0.0));
    positionAlloc4->Add (Vector(686.0966128259065,415.3975210408147, 0.0));
    positionAlloc4->Add (Vector(613.9621604630152,741.0334844395363, 0.0));
    positionAlloc4->Add (Vector(405.05780587005614,807.0202210563092, 0.0));
    positionAlloc4->Add (Vector(822.4205670087649,631.8865355189953, 0.0));
    positionAlloc4->Add (Vector(190.6443372237639,749.3708291841342, 0.0));
    positionAlloc4->Add (Vector(435.3188336825269,168.5181538362969, 0.0));
    positionAlloc4->Add (Vector(423.02679435801355,716.8276022049647, 0.0));
    positionAlloc4->Add (Vector(536.8966991166885,435.2049569363007, 0.0));
    positionAlloc4->Add (Vector(499.0104011738485,283.47316272835405, 0.0));
    positionAlloc4->Add (Vector(390.2870099506829,630.985764251864, 0.0));
    positionAlloc4->Add (Vector(747.4851595708217,204.09020838702392, 0.0));
    positionAlloc4->Add (Vector(655.613030530677,720.5517934778269, 0.0));
    positionAlloc4->Add (Vector(579.8454893091789,655.8547916525995, 0.0));
    positionAlloc4->Add (Vector(574.2234167642649,298.10749419934393, 0.0));
    positionAlloc4->Add (Vector(817.5014761886081,278.1947601002389, 0.0));
    positionAlloc4->Add (Vector(665.1630412062557,532.5511997285182, 0.0));
    positionAlloc4->Add (Vector(506.00454530810055,207.72065458876773, 0.0));
    positionAlloc4->Add (Vector(579.5945372947037,198.65545756063526, 0.0));
    positionAlloc4->Add (Vector(188.31774513695584,693.2342712085662, 0.0));
    positionAlloc4->Add (Vector(668.5723605524807,735.512127381729, 0.0));
    positionAlloc4->Add (Vector(778.7813585723771,590.9185925698396, 0.0));
    positionAlloc4->Add (Vector(729.6876342998307,596.9675323341653, 0.0));
    positionAlloc4->Add (Vector(408.21923566180396,355.6614201708552, 0.0));
    positionAlloc4->Add (Vector(268.6905960590949,722.2335075134445, 0.0));
    positionAlloc4->Add (Vector(625.6878833751022,533.5774577557496, 0.0));
    positionAlloc4->Add (Vector(756.490590667377,165.26475518707693, 0.0));
    positionAlloc4->Add (Vector(285.91345419015676,483.9467246970907, 0.0));
    positionAlloc4->Add (Vector(842.0595884398087,500.6089673675159, 0.0));
    positionAlloc4->Add (Vector(546.3433764550017,316.5469135714884, 0.0));
    positionAlloc4->Add (Vector(456.6856429566042,401.23909369862145, 0.0));
    positionAlloc4->Add (Vector(477.4700073266802,367.9910277168111, 0.0));
    positionAlloc4->Add (Vector(470.2817851613885,388.53444254422106, 0.0));
    positionAlloc4->Add (Vector(509.9900468097858,568.0054931382031, 0.0));
    positionAlloc4->Add (Vector(753.6136656036693,260.3452250830864, 0.0));
    positionAlloc4->Add (Vector(640.4604802214978,166.04510120474586, 0.0));
    positionAlloc4->Add (Vector(708.319544642852,174.49918888478996, 0.0));
    positionAlloc4->Add (Vector(461.7237327566263,192.25609046054015, 0.0));
    positionAlloc4->Add (Vector(566.8518204009654,657.2966412250644, 0.0));
    positionAlloc4->Add (Vector(580.5358158699954,835.3785188213616, 0.0));
    positionAlloc4->Add (Vector(755.1338325324798,349.8134510399059, 0.0));
    positionAlloc4->Add (Vector(578.0039657176319,198.25289148686235, 0.0));
    positionAlloc4->Add (Vector(804.5309374735114,545.6382648244257, 0.0));
    positionAlloc4->Add (Vector(301.10647836164196,646.5735216757901, 0.0));
    positionAlloc4->Add (Vector(375.47881717839664,630.4939257983493, 0.0));
    positionAlloc4->Add (Vector(487.97224604528776,398.981172926062, 0.0));
    positionAlloc4->Add (Vector(256.0374791209707,430.4125501238678, 0.0));
    positionAlloc4->Add (Vector(634.394765439721,677.0640197641775, 0.0));
    positionAlloc4->Add (Vector(768.066719093248,365.1609034428314, 0.0));
    positionAlloc4->Add (Vector(673.5245778182583,539.7705825785752, 0.0));
    positionAlloc4->Add (Vector(372.4497860108047,367.0012969476137, 0.0));
    positionAlloc4->Add (Vector(663.7166812916194,349.2587754455144, 0.0));
    positionAlloc4->Add (Vector(802.5975008495466,325.39990399550345, 0.0));
    positionAlloc4->Add (Vector(741.3532522026932,287.04257798919537, 0.0));
    positionAlloc4->Add (Vector(585.9804658728257,257.0221919513574, 0.0));
    positionAlloc4->Add (Vector(520.6380176655758,629.1143339242379, 0.0));
    positionAlloc4->Add (Vector(405.53545787387156,711.5549232799112, 0.0));
    positionAlloc4->Add (Vector(794.189838321077,449.24571039602415, 0.0));
    positionAlloc4->Add (Vector(353.91196609360526,716.0103186003686, 0.0));
    positionAlloc4->Add (Vector(254.71835930329087,728.1815687084409, 0.0));
    positionAlloc4->Add (Vector(476.6150114049336,778.4817557209936, 0.0));
    positionAlloc4->Add (Vector(312.0056413243973,326.2211249977713, 0.0));
    positionAlloc4->Add (Vector(809.2748882753087,608.8907381921442, 0.0));
    positionAlloc4->Add (Vector(159.3833699763937,740.3741524303495, 0.0));
    positionAlloc4->Add (Vector(772.2215577595379,375.53541545357393, 0.0));
    positionAlloc4->Add (Vector(730.970543268723,290.3566664856212, 0.0));
    positionAlloc4->Add (Vector(739.4271485238648,654.226994128481, 0.0));
    positionAlloc4->Add (Vector(178.363667807769,609.2901630570414, 0.0));
    positionAlloc4->Add (Vector(625.5511529451526,491.2692992834999, 0.0));
    positionAlloc4->Add (Vector(280.7605887315249,587.0803412305141, 0.0));
    positionAlloc4->Add (Vector(152.81769519777953,431.1516240484293, 0.0));
    positionAlloc4->Add (Vector(753.1600088479337,465.8874567598868, 0.0));
    positionAlloc4->Add (Vector(740.1769696034819,157.42519635922358, 0.0));
    positionAlloc4->Add (Vector(245.46977505357376,207.6831042088311, 0.0));
    positionAlloc4->Add (Vector(770.9993990859007,184.4710809858024, 0.0));
    positionAlloc4->Add (Vector(428.8643729616014,373.3667379110916, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_19.xml");
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
	  flowmon->SerializeToXmlFile ("switch_SUI_flowmon/switch_SUI_19.flowmon", false, false);
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