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
	  uint16_t numberOfNodes = 583;
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
    positionAlloc->Add (Vector (772.2222222222222,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,305.55555555555554, 0.0));
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
    positionAlloc4->Add (Vector(155.84481625963878,227.24853959051285, 0.0));
    positionAlloc4->Add (Vector(157.16369948613027,165.51167488116508, 0.0));
    positionAlloc4->Add (Vector(329.8901564976468,186.52954045773276, 0.0));
    positionAlloc4->Add (Vector(452.20181951862776,593.8353558147036, 0.0));
    positionAlloc4->Add (Vector(545.7022719677875,324.08426662979457, 0.0));
    positionAlloc4->Add (Vector(635.9923225051041,228.260259863756, 0.0));
    positionAlloc4->Add (Vector(786.8028183354431,506.4716753520126, 0.0));
    positionAlloc4->Add (Vector(717.1440946523348,737.3851221743154, 0.0));
    positionAlloc4->Add (Vector(429.5094792180501,505.52594667189436, 0.0));
    positionAlloc4->Add (Vector(665.7251546392591,300.9263441575333, 0.0));
    positionAlloc4->Add (Vector(523.3366804248005,750.5966559091511, 0.0));
    positionAlloc4->Add (Vector(320.8444157300477,230.0039000857996, 0.0));
    positionAlloc4->Add (Vector(153.91590911730867,179.9482877730875, 0.0));
    positionAlloc4->Add (Vector(719.8363484950007,265.25828177922074, 0.0));
    positionAlloc4->Add (Vector(365.22352112346823,839.4565082796045, 0.0));
    positionAlloc4->Add (Vector(375.6333182603315,456.59842352688787, 0.0));
    positionAlloc4->Add (Vector(649.8380314588201,354.82890919642574, 0.0));
    positionAlloc4->Add (Vector(586.4022554103526,601.077313597306, 0.0));
    positionAlloc4->Add (Vector(464.54592171068236,646.0053875454994, 0.0));
    positionAlloc4->Add (Vector(364.74092594698266,482.6592269012182, 0.0));
    positionAlloc4->Add (Vector(647.1015263175524,296.1859143163887, 0.0));
    positionAlloc4->Add (Vector(238.93938575037203,222.71165319516734, 0.0));
    positionAlloc4->Add (Vector(284.77094080669764,671.59292394888, 0.0));
    positionAlloc4->Add (Vector(548.6028781663982,437.34518017893924, 0.0));
    positionAlloc4->Add (Vector(495.41099549791704,550.5028144504802, 0.0));
    positionAlloc4->Add (Vector(234.75690876902394,755.2613797229242, 0.0));
    positionAlloc4->Add (Vector(326.9198681239171,367.9839515314146, 0.0));
    positionAlloc4->Add (Vector(203.1722008754418,636.785004012514, 0.0));
    positionAlloc4->Add (Vector(294.3023121961148,431.9275331232094, 0.0));
    positionAlloc4->Add (Vector(632.6950877363754,436.19056320754777, 0.0));
    positionAlloc4->Add (Vector(749.3873574668569,232.2234509574688, 0.0));
    positionAlloc4->Add (Vector(412.66879274784037,569.3088459802286, 0.0));
    positionAlloc4->Add (Vector(814.6468022174132,847.713639206067, 0.0));
    positionAlloc4->Add (Vector(253.86976475223435,595.2282485923201, 0.0));
    positionAlloc4->Add (Vector(224.63349096379645,391.9066113440882, 0.0));
    positionAlloc4->Add (Vector(242.3508780276676,496.8564241314468, 0.0));
    positionAlloc4->Add (Vector(780.97079976746,212.9147960704466, 0.0));
    positionAlloc4->Add (Vector(474.9864942001578,799.0223864151922, 0.0));
    positionAlloc4->Add (Vector(332.78900110315396,384.3998656992868, 0.0));
    positionAlloc4->Add (Vector(690.5067533652252,799.6061192421878, 0.0));
    positionAlloc4->Add (Vector(345.71605936626236,549.5206192133239, 0.0));
    positionAlloc4->Add (Vector(380.1917017462927,607.4422524407732, 0.0));
    positionAlloc4->Add (Vector(191.15721158124595,660.4864840856434, 0.0));
    positionAlloc4->Add (Vector(813.3217511254055,474.7056550001933, 0.0));
    positionAlloc4->Add (Vector(501.7707386911589,339.8244499978424, 0.0));
    positionAlloc4->Add (Vector(264.96874705620814,315.75862421728317, 0.0));
    positionAlloc4->Add (Vector(523.8711830601059,302.7886379973703, 0.0));
    positionAlloc4->Add (Vector(366.69854656676205,581.8188503764763, 0.0));
    positionAlloc4->Add (Vector(175.8253022375377,303.8604983733466, 0.0));
    positionAlloc4->Add (Vector(313.0805484546273,783.8426342439279, 0.0));
    positionAlloc4->Add (Vector(385.69396822827343,251.13298761622374, 0.0));
    positionAlloc4->Add (Vector(631.6797133827388,164.8146697874268, 0.0));
    positionAlloc4->Add (Vector(551.9892661799611,733.1398588849495, 0.0));
    positionAlloc4->Add (Vector(697.2221011706921,398.7377957536829, 0.0));
    positionAlloc4->Add (Vector(590.7488639627202,706.5985057580657, 0.0));
    positionAlloc4->Add (Vector(750.8492386113677,849.027998926218, 0.0));
    positionAlloc4->Add (Vector(410.30208073316606,359.1621448582922, 0.0));
    positionAlloc4->Add (Vector(815.19883511934,849.1250240739218, 0.0));
    positionAlloc4->Add (Vector(772.785607729292,576.0812695179605, 0.0));
    positionAlloc4->Add (Vector(201.77423108966917,272.4977801254088, 0.0));
    positionAlloc4->Add (Vector(401.43538167242707,556.2927160461029, 0.0));
    positionAlloc4->Add (Vector(161.57598719246957,720.3544011451272, 0.0));
    positionAlloc4->Add (Vector(396.70171788691846,432.08096008637597, 0.0));
    positionAlloc4->Add (Vector(338.69318527672164,220.11852836613093, 0.0));
    positionAlloc4->Add (Vector(454.48613852163163,659.1047340676979, 0.0));
    positionAlloc4->Add (Vector(840.3262181260866,724.786538282407, 0.0));
    positionAlloc4->Add (Vector(487.1898866086387,430.4604939065673, 0.0));
    positionAlloc4->Add (Vector(579.5985272979597,586.538163108085, 0.0));
    positionAlloc4->Add (Vector(373.7536193113374,416.776528205798, 0.0));
    positionAlloc4->Add (Vector(650.9735360858037,769.2776641951814, 0.0));
    positionAlloc4->Add (Vector(800.2627149008363,787.5262689167031, 0.0));
    positionAlloc4->Add (Vector(390.0244639442858,442.85823709240503, 0.0));
    positionAlloc4->Add (Vector(682.390680863946,489.8963415984004, 0.0));
    positionAlloc4->Add (Vector(230.88968918543713,199.95690972256602, 0.0));
    positionAlloc4->Add (Vector(195.63496152928371,184.94573996436537, 0.0));
    positionAlloc4->Add (Vector(536.2454986079867,341.8379452006834, 0.0));
    positionAlloc4->Add (Vector(792.4617781467085,247.7496311467085, 0.0));
    positionAlloc4->Add (Vector(374.95896901860925,176.24156980876154, 0.0));
    positionAlloc4->Add (Vector(167.50287899541448,700.047505275628, 0.0));
    positionAlloc4->Add (Vector(294.5592154508488,828.8745496687283, 0.0));
    positionAlloc4->Add (Vector(782.4915125880973,317.29061142897353, 0.0));
    positionAlloc4->Add (Vector(173.37524340610017,420.001698932714, 0.0));
    positionAlloc4->Add (Vector(396.00917142947844,266.97006143594376, 0.0));
    positionAlloc4->Add (Vector(414.28179609689,702.0805004619968, 0.0));
    positionAlloc4->Add (Vector(814.9685065325428,713.9458476924427, 0.0));
    positionAlloc4->Add (Vector(419.52886648828814,735.5312341661798, 0.0));
    positionAlloc4->Add (Vector(300.5500300255367,609.3595932861647, 0.0));
    positionAlloc4->Add (Vector(510.4858674538413,241.51431762049498, 0.0));
    positionAlloc4->Add (Vector(760.95742755561,707.8913583312898, 0.0));
    positionAlloc4->Add (Vector(385.3117654895692,356.0252209618651, 0.0));
    positionAlloc4->Add (Vector(480.9631248789073,779.8317307577802, 0.0));
    positionAlloc4->Add (Vector(699.1090794495103,625.0992653507477, 0.0));
    positionAlloc4->Add (Vector(439.9403880132504,181.18964213235725, 0.0));
    positionAlloc4->Add (Vector(597.6876784641179,614.63671528519, 0.0));
    positionAlloc4->Add (Vector(776.1771965684137,650.5739295326625, 0.0));
    positionAlloc4->Add (Vector(226.28014051274107,325.5122782014381, 0.0));
    positionAlloc4->Add (Vector(625.8857318304042,717.7151825045588, 0.0));
    positionAlloc4->Add (Vector(603.0116927888702,792.1980969631509, 0.0));
    positionAlloc4->Add (Vector(697.0431702810004,256.1115383316236, 0.0));
    positionAlloc4->Add (Vector(210.92349556408797,350.66147391307686, 0.0));
    positionAlloc4->Add (Vector(343.2525952485872,460.5519526224351, 0.0));
    positionAlloc4->Add (Vector(220.15977965001898,678.0631688674367, 0.0));
    positionAlloc4->Add (Vector(152.75052499555247,496.82143503206106, 0.0));
    positionAlloc4->Add (Vector(715.8552831237716,334.3825283690911, 0.0));
    positionAlloc4->Add (Vector(758.8379976097481,452.41735974361916, 0.0));
    positionAlloc4->Add (Vector(439.8480336083097,327.5877809970675, 0.0));
    positionAlloc4->Add (Vector(235.60210482641966,744.0113599216147, 0.0));
    positionAlloc4->Add (Vector(752.6513378050872,725.1526180151501, 0.0));
    positionAlloc4->Add (Vector(574.8455961585782,545.3689467603276, 0.0));
    positionAlloc4->Add (Vector(718.5676770261231,625.3901819344034, 0.0));
    positionAlloc4->Add (Vector(276.2030914309484,656.8951851828432, 0.0));
    positionAlloc4->Add (Vector(533.7426409545061,817.2242767841974, 0.0));
    positionAlloc4->Add (Vector(506.4481096722298,811.5353873762225, 0.0));
    positionAlloc4->Add (Vector(465.4363975757183,446.72982639789876, 0.0));
    positionAlloc4->Add (Vector(717.0485157604593,486.8933053581317, 0.0));
    positionAlloc4->Add (Vector(732.9310679168999,235.0578186763663, 0.0));
    positionAlloc4->Add (Vector(565.8732467927975,645.70313421899, 0.0));
    positionAlloc4->Add (Vector(784.3779657165078,504.4601714816017, 0.0));
    positionAlloc4->Add (Vector(425.0711646456776,746.0070874317801, 0.0));
    positionAlloc4->Add (Vector(780.8568160844525,238.27188719118652, 0.0));
    positionAlloc4->Add (Vector(829.9812940260523,208.57669177338812, 0.0));
    positionAlloc4->Add (Vector(538.8562848768382,632.9783084992075, 0.0));
    positionAlloc4->Add (Vector(227.40543094806162,736.8364744153697, 0.0));
    positionAlloc4->Add (Vector(549.2702697033587,679.5343751882887, 0.0));
    positionAlloc4->Add (Vector(301.36302607369385,198.63598941847232, 0.0));
    positionAlloc4->Add (Vector(630.0981446810442,162.51493882437265, 0.0));
    positionAlloc4->Add (Vector(646.5266969431771,748.1956182588683, 0.0));
    positionAlloc4->Add (Vector(412.95411484510055,248.74185964460796, 0.0));
    positionAlloc4->Add (Vector(668.0251614889239,589.390289520518, 0.0));
    positionAlloc4->Add (Vector(748.3740370945316,417.10207147786247, 0.0));
    positionAlloc4->Add (Vector(179.43135725316128,568.7664318963275, 0.0));
    positionAlloc4->Add (Vector(387.70119884571875,384.6562261495002, 0.0));
    positionAlloc4->Add (Vector(768.3578086216384,262.2757125043016, 0.0));
    positionAlloc4->Add (Vector(530.4740648618865,738.3751159644356, 0.0));
    positionAlloc4->Add (Vector(623.3636145283342,388.64148643446333, 0.0));
    positionAlloc4->Add (Vector(365.8485494426172,769.1475743557045, 0.0));
    positionAlloc4->Add (Vector(339.5136126952127,849.5345859817808, 0.0));
    positionAlloc4->Add (Vector(187.33811410362847,429.01976390291907, 0.0));
    positionAlloc4->Add (Vector(532.2172153245251,532.9197024971133, 0.0));
    positionAlloc4->Add (Vector(440.9925047994503,700.869767335329, 0.0));
    positionAlloc4->Add (Vector(808.221307267485,355.57348935090533, 0.0));
    positionAlloc4->Add (Vector(682.7774119396805,487.77688522667086, 0.0));
    positionAlloc4->Add (Vector(390.40141161364517,430.63190580538827, 0.0));
    positionAlloc4->Add (Vector(265.8747048857062,431.9126589068763, 0.0));
    positionAlloc4->Add (Vector(540.349675340211,553.7325932051469, 0.0));
    positionAlloc4->Add (Vector(572.3923049305984,422.38610673052113, 0.0));
    positionAlloc4->Add (Vector(370.13364510655737,730.2279783831144, 0.0));
    positionAlloc4->Add (Vector(576.9081144898764,658.274247858593, 0.0));
    positionAlloc4->Add (Vector(642.0759644812505,277.8641341999096, 0.0));
    positionAlloc4->Add (Vector(848.992173939548,453.15327181075486, 0.0));
    positionAlloc4->Add (Vector(191.0136213970948,300.8373879499022, 0.0));
    positionAlloc4->Add (Vector(298.65945871174847,750.3113730336811, 0.0));
    positionAlloc4->Add (Vector(243.3794646242426,630.4605629408504, 0.0));
    positionAlloc4->Add (Vector(620.4590734242441,172.48382924747014, 0.0));
    positionAlloc4->Add (Vector(588.9968758427822,385.45809831859685, 0.0));
    positionAlloc4->Add (Vector(316.3891322143602,206.8314509353018, 0.0));
    positionAlloc4->Add (Vector(783.561686371785,758.5380755351487, 0.0));
    positionAlloc4->Add (Vector(446.5852063997496,365.0049651329665, 0.0));
    positionAlloc4->Add (Vector(840.1722114807449,220.48552077266217, 0.0));
    positionAlloc4->Add (Vector(167.47963030389184,429.654532682395, 0.0));
    positionAlloc4->Add (Vector(271.99507495095247,585.3173747232754, 0.0));
    positionAlloc4->Add (Vector(162.39311155126603,753.2416878525482, 0.0));
    positionAlloc4->Add (Vector(777.2198557274952,440.2340436775345, 0.0));
    positionAlloc4->Add (Vector(488.94914603185345,732.1480281185317, 0.0));
    positionAlloc4->Add (Vector(306.72806331397953,460.74168863257785, 0.0));
    positionAlloc4->Add (Vector(357.3790273217489,531.5058910067584, 0.0));
    positionAlloc4->Add (Vector(772.976108122677,700.0603256315821, 0.0));
    positionAlloc4->Add (Vector(497.08802177281683,801.2964452251445, 0.0));
    positionAlloc4->Add (Vector(722.2487795679666,505.44893086578475, 0.0));
    positionAlloc4->Add (Vector(835.5939111964965,475.08203442445216, 0.0));
    positionAlloc4->Add (Vector(284.9613318665215,589.1628531047536, 0.0));
    positionAlloc4->Add (Vector(155.43175383619715,482.7050835958558, 0.0));
    positionAlloc4->Add (Vector(220.14794433536719,621.5981534045941, 0.0));
    positionAlloc4->Add (Vector(731.9268498996747,405.53198543327943, 0.0));
    positionAlloc4->Add (Vector(809.3334809868304,521.0619478400968, 0.0));
    positionAlloc4->Add (Vector(642.8599680496347,332.0811881107794, 0.0));
    positionAlloc4->Add (Vector(680.5006854504701,521.1732684563699, 0.0));
    positionAlloc4->Add (Vector(797.8810281035564,412.99406376618936, 0.0));
    positionAlloc4->Add (Vector(441.1415730529012,363.76703737792457, 0.0));
    positionAlloc4->Add (Vector(200.93366336773897,219.13833597912588, 0.0));
    positionAlloc4->Add (Vector(233.74130766124924,441.69606469306086, 0.0));
    positionAlloc4->Add (Vector(504.1883304099444,429.0489346374688, 0.0));
    positionAlloc4->Add (Vector(284.3136991716498,556.7743712885275, 0.0));
    positionAlloc4->Add (Vector(737.6966945327954,563.1773556817861, 0.0));
    positionAlloc4->Add (Vector(766.8381376889487,444.83396587777355, 0.0));
    positionAlloc4->Add (Vector(253.0916659488287,466.79515240490787, 0.0));
    positionAlloc4->Add (Vector(375.9701483996903,531.8417036398735, 0.0));
    positionAlloc4->Add (Vector(684.5423408829938,631.5888150665688, 0.0));
    positionAlloc4->Add (Vector(701.5053037098808,327.71880767274774, 0.0));
    positionAlloc4->Add (Vector(606.7688144252932,578.4555227427014, 0.0));
    positionAlloc4->Add (Vector(157.67455719208036,268.8548476856632, 0.0));
    positionAlloc4->Add (Vector(842.9245135679123,434.8153438372696, 0.0));
    positionAlloc4->Add (Vector(181.0766854785289,722.2726911706844, 0.0));
    positionAlloc4->Add (Vector(347.6284455123597,487.0047421703239, 0.0));
    positionAlloc4->Add (Vector(573.5500396265368,632.0405029303803, 0.0));
    positionAlloc4->Add (Vector(447.8018359794419,678.8462599099504, 0.0));
    positionAlloc4->Add (Vector(309.46052899899934,289.5450902612804, 0.0));
    positionAlloc4->Add (Vector(671.0126021361563,476.8771864146585, 0.0));
    positionAlloc4->Add (Vector(792.2673090612436,465.0869908446672, 0.0));
    positionAlloc4->Add (Vector(708.7548271116093,453.1521536011956, 0.0));
    positionAlloc4->Add (Vector(803.0737839969556,319.2970629453223, 0.0));
    positionAlloc4->Add (Vector(394.6396276156707,795.2091943717462, 0.0));
    positionAlloc4->Add (Vector(443.7706045100878,836.9639730283363, 0.0));
    positionAlloc4->Add (Vector(320.86620074482164,660.3093949050781, 0.0));
    positionAlloc4->Add (Vector(680.9190231163886,410.262093892175, 0.0));
    positionAlloc4->Add (Vector(688.4307529971496,450.1980529146469, 0.0));
    positionAlloc4->Add (Vector(675.8038574758078,562.8457205446549, 0.0));
    positionAlloc4->Add (Vector(227.37895509171474,795.4648575006321, 0.0));
    positionAlloc4->Add (Vector(709.4861761719776,243.37203196528168, 0.0));
    positionAlloc4->Add (Vector(341.7102737937597,568.340295236892, 0.0));
    positionAlloc4->Add (Vector(424.9326432473034,153.06178759810655, 0.0));
    positionAlloc4->Add (Vector(274.1178774658514,753.8718492323223, 0.0));
    positionAlloc4->Add (Vector(812.4162294458712,251.42906151514222, 0.0));
    positionAlloc4->Add (Vector(404.19009734274516,816.1224937498516, 0.0));
    positionAlloc4->Add (Vector(418.1459732600451,760.1317924301951, 0.0));
    positionAlloc4->Add (Vector(428.98060005576775,268.920672364634, 0.0));
    positionAlloc4->Add (Vector(751.4275395877759,432.4659365550606, 0.0));
    positionAlloc4->Add (Vector(368.85942004865365,274.7631692381084, 0.0));
    positionAlloc4->Add (Vector(265.26918375511354,546.0725950016276, 0.0));
    positionAlloc4->Add (Vector(770.5948788418065,660.8408461353101, 0.0));
    positionAlloc4->Add (Vector(733.7935334693675,248.19950895157675, 0.0));
    positionAlloc4->Add (Vector(735.8470309365044,169.34228299915145, 0.0));
    positionAlloc4->Add (Vector(459.8123163271211,230.24796050999478, 0.0));
    positionAlloc4->Add (Vector(811.6320058316811,369.6146811912578, 0.0));
    positionAlloc4->Add (Vector(312.3307852382404,182.68847251957914, 0.0));
    positionAlloc4->Add (Vector(572.0361679972799,555.3507693801962, 0.0));
    positionAlloc4->Add (Vector(399.32497402989475,812.9612828828888, 0.0));
    positionAlloc4->Add (Vector(571.4712810891449,468.47222217510773, 0.0));
    positionAlloc4->Add (Vector(811.3848171145162,695.7260497797256, 0.0));
    positionAlloc4->Add (Vector(475.4096511080406,816.1062166079087, 0.0));
    positionAlloc4->Add (Vector(225.71224656462752,506.2133759865343, 0.0));
    positionAlloc4->Add (Vector(645.9347771957846,197.54834295597837, 0.0));
    positionAlloc4->Add (Vector(817.980909060717,249.60035302765507, 0.0));
    positionAlloc4->Add (Vector(166.17479695123617,185.792146674688, 0.0));
    positionAlloc4->Add (Vector(533.9385100243871,231.23742156919823, 0.0));
    positionAlloc4->Add (Vector(452.9957747569697,288.3696558119883, 0.0));
    positionAlloc4->Add (Vector(615.0410040253,299.8367193592039, 0.0));
    positionAlloc4->Add (Vector(470.6468573510239,242.28563246245994, 0.0));
    positionAlloc4->Add (Vector(179.63446213174447,157.8395136789929, 0.0));
    positionAlloc4->Add (Vector(537.6816411074765,661.4948190540204, 0.0));
    positionAlloc4->Add (Vector(561.3683360312823,372.55753079477597, 0.0));
    positionAlloc4->Add (Vector(458.2462417252766,644.9844297610666, 0.0));
    positionAlloc4->Add (Vector(813.0990261628375,389.6150993792638, 0.0));
    positionAlloc4->Add (Vector(765.804373142151,289.8099633856835, 0.0));
    positionAlloc4->Add (Vector(826.0540710748774,155.76536911437208, 0.0));
    positionAlloc4->Add (Vector(333.7391183015337,321.7173485901859, 0.0));
    positionAlloc4->Add (Vector(721.7216956308771,682.9447593420247, 0.0));
    positionAlloc4->Add (Vector(347.15710677912466,572.9676572324996, 0.0));
    positionAlloc4->Add (Vector(247.14311887884153,360.02133641684765, 0.0));
    positionAlloc4->Add (Vector(544.3686184663159,229.63372373645626, 0.0));
    positionAlloc4->Add (Vector(254.92600672971068,212.97709520267261, 0.0));
    positionAlloc4->Add (Vector(802.8773316523516,335.14501134323496, 0.0));
    positionAlloc4->Add (Vector(621.9395624600024,293.12670413830284, 0.0));
    positionAlloc4->Add (Vector(658.7143854473397,776.437449795266, 0.0));
    positionAlloc4->Add (Vector(578.6193693928456,456.83294293004764, 0.0));
    positionAlloc4->Add (Vector(341.1570152249228,361.8355995573221, 0.0));
    positionAlloc4->Add (Vector(777.0592002561891,342.24581587188806, 0.0));
    positionAlloc4->Add (Vector(167.4131174102238,413.784892711316, 0.0));
    positionAlloc4->Add (Vector(379.95633803650423,816.7158842024697, 0.0));
    positionAlloc4->Add (Vector(777.9269613754792,781.2191732284494, 0.0));
    positionAlloc4->Add (Vector(537.6417680061066,652.5074957918846, 0.0));
    positionAlloc4->Add (Vector(402.7697484385268,746.5499447060856, 0.0));
    positionAlloc4->Add (Vector(482.06084798223327,395.80058338976846, 0.0));
    positionAlloc4->Add (Vector(281.0211804488629,691.3128280800628, 0.0));
    positionAlloc4->Add (Vector(269.7075980927599,448.1079926182006, 0.0));
    positionAlloc4->Add (Vector(311.6764437512221,761.9763136790353, 0.0));
    positionAlloc4->Add (Vector(473.7459918087067,345.60969556512595, 0.0));
    positionAlloc4->Add (Vector(520.3722873623069,763.3423535574894, 0.0));
    positionAlloc4->Add (Vector(280.7223992181288,507.1511538548829, 0.0));
    positionAlloc4->Add (Vector(227.4104407747683,589.412914893586, 0.0));
    positionAlloc4->Add (Vector(707.1812704866109,576.7854985618269, 0.0));
    positionAlloc4->Add (Vector(336.94265816755217,458.1009445765558, 0.0));
    positionAlloc4->Add (Vector(641.8389744265564,809.9404007736936, 0.0));
    positionAlloc4->Add (Vector(439.44799594436023,446.5100788153552, 0.0));
    positionAlloc4->Add (Vector(685.3935252539884,790.6234360306184, 0.0));
    positionAlloc4->Add (Vector(195.93959673501826,431.01048448200265, 0.0));
    positionAlloc4->Add (Vector(450.59315079657733,458.9014619599604, 0.0));
    positionAlloc4->Add (Vector(452.55895284372957,617.5536148190197, 0.0));
    positionAlloc4->Add (Vector(735.6156453916658,324.12028640382755, 0.0));
    positionAlloc4->Add (Vector(612.7914107780018,180.78014913501838, 0.0));
    positionAlloc4->Add (Vector(249.08419569147634,647.2713532068087, 0.0));
    positionAlloc4->Add (Vector(167.36653102979605,418.64175124893666, 0.0));
    positionAlloc4->Add (Vector(693.6480520608361,309.8315856153173, 0.0));
    positionAlloc4->Add (Vector(781.1573706988936,790.8310913014606, 0.0));
    positionAlloc4->Add (Vector(574.5204428804819,678.7372506527805, 0.0));
    positionAlloc4->Add (Vector(309.0043621858624,422.7382121506073, 0.0));
    positionAlloc4->Add (Vector(491.642109388764,808.7982346020882, 0.0));
    positionAlloc4->Add (Vector(734.6718834166016,378.96405420114706, 0.0));
    positionAlloc4->Add (Vector(239.7609631126419,335.626612591226, 0.0));
    positionAlloc4->Add (Vector(294.7226520181674,585.1407046302495, 0.0));
    positionAlloc4->Add (Vector(575.3365031187572,257.4197824757135, 0.0));
    positionAlloc4->Add (Vector(348.3783861994157,598.0632776103191, 0.0));
    positionAlloc4->Add (Vector(213.46346195653683,617.0713800432677, 0.0));
    positionAlloc4->Add (Vector(619.9124591654614,843.1342524761702, 0.0));
    positionAlloc4->Add (Vector(706.9061748560287,558.6832908377958, 0.0));
    positionAlloc4->Add (Vector(656.6619436413663,783.828084496191, 0.0));
    positionAlloc4->Add (Vector(599.4904028049823,171.55059820252262, 0.0));
    positionAlloc4->Add (Vector(570.1530277433785,833.8797382383826, 0.0));
    positionAlloc4->Add (Vector(808.2809605835299,538.1587889836755, 0.0));
    positionAlloc4->Add (Vector(810.5435694539464,635.4268341748191, 0.0));
    positionAlloc4->Add (Vector(740.3326484243562,652.6043470753535, 0.0));
    positionAlloc4->Add (Vector(490.07439120227934,210.1816146954481, 0.0));
    positionAlloc4->Add (Vector(193.1762073771668,296.12211767904046, 0.0));
    positionAlloc4->Add (Vector(380.3671162203317,523.7695828544538, 0.0));
    positionAlloc4->Add (Vector(734.3589448059583,683.2636784684667, 0.0));
    positionAlloc4->Add (Vector(326.53661299954274,377.3254262770414, 0.0));
    positionAlloc4->Add (Vector(613.9056963577244,620.4735469096353, 0.0));
    positionAlloc4->Add (Vector(810.1341674463341,275.74367846049853, 0.0));
    positionAlloc4->Add (Vector(732.4285174531432,366.88784006937476, 0.0));
    positionAlloc4->Add (Vector(407.4825570246695,440.9627010503144, 0.0));
    positionAlloc4->Add (Vector(165.6111934290505,576.6535857146017, 0.0));
    positionAlloc4->Add (Vector(753.6830466051624,747.9873848602715, 0.0));
    positionAlloc4->Add (Vector(478.497594684169,392.73205034404805, 0.0));
    positionAlloc4->Add (Vector(164.1522641034582,619.5320151980737, 0.0));
    positionAlloc4->Add (Vector(301.33200170933276,690.039412371848, 0.0));
    positionAlloc4->Add (Vector(686.9087709865386,809.3821025005132, 0.0));
    positionAlloc4->Add (Vector(631.7221057055845,840.1820223798636, 0.0));
    positionAlloc4->Add (Vector(366.95093646041653,779.9882938150579, 0.0));
    positionAlloc4->Add (Vector(544.2386647137129,263.5606997675479, 0.0));
    positionAlloc4->Add (Vector(376.7832745930773,625.2147626734177, 0.0));
    positionAlloc4->Add (Vector(692.9582447282233,351.70464232336246, 0.0));
    positionAlloc4->Add (Vector(463.559692024015,745.8975485515717, 0.0));
    positionAlloc4->Add (Vector(734.8748508877168,429.399162765766, 0.0));
    positionAlloc4->Add (Vector(162.71057592495518,655.7229172372678, 0.0));
    positionAlloc4->Add (Vector(409.745314362472,593.9595991695303, 0.0));
    positionAlloc4->Add (Vector(672.257509601025,293.3610674574169, 0.0));
    positionAlloc4->Add (Vector(677.6624386379814,457.5122906295978, 0.0));
    positionAlloc4->Add (Vector(799.7517569533161,520.1636527097364, 0.0));
    positionAlloc4->Add (Vector(548.6583085410948,763.623694859419, 0.0));
    positionAlloc4->Add (Vector(171.2781903680683,453.5358108311717, 0.0));
    positionAlloc4->Add (Vector(599.1339991000864,296.6403559037198, 0.0));
    positionAlloc4->Add (Vector(354.6031208964772,609.0441112489261, 0.0));
    positionAlloc4->Add (Vector(702.5334376383867,522.6314218109719, 0.0));
    positionAlloc4->Add (Vector(663.8871805598781,222.85316949277288, 0.0));
    positionAlloc4->Add (Vector(686.6358656221403,403.52513029070224, 0.0));
    positionAlloc4->Add (Vector(340.78335352512033,762.4926465170951, 0.0));
    positionAlloc4->Add (Vector(661.9415666296901,842.5333007797926, 0.0));
    positionAlloc4->Add (Vector(640.1312925813195,728.3884049038068, 0.0));
    positionAlloc4->Add (Vector(479.38127946585587,370.1623005463737, 0.0));
    positionAlloc4->Add (Vector(178.00919137458703,382.61693751797173, 0.0));
    positionAlloc4->Add (Vector(816.9626937744213,706.1866060951938, 0.0));
    positionAlloc4->Add (Vector(465.27305348415166,602.6709272119335, 0.0));
    positionAlloc4->Add (Vector(783.4506618872707,542.0027924602429, 0.0));
    positionAlloc4->Add (Vector(652.9090323148789,424.42067436251386, 0.0));
    positionAlloc4->Add (Vector(417.1347309236227,582.6262672738699, 0.0));
    positionAlloc4->Add (Vector(847.3016694869343,380.6794934667174, 0.0));
    positionAlloc4->Add (Vector(241.92627483502218,723.4151500673609, 0.0));
    positionAlloc4->Add (Vector(435.75527581176243,341.6897633694814, 0.0));
    positionAlloc4->Add (Vector(360.15897807897045,521.7765957809088, 0.0));
    positionAlloc4->Add (Vector(407.7839529103392,181.88510917574675, 0.0));
    positionAlloc4->Add (Vector(337.3667318386099,788.117820592192, 0.0));
    positionAlloc4->Add (Vector(506.5022934759483,522.6434095682401, 0.0));
    positionAlloc4->Add (Vector(184.38882572429662,384.3204722595535, 0.0));
    positionAlloc4->Add (Vector(173.3821046042041,213.75909041603603, 0.0));
    positionAlloc4->Add (Vector(752.8745696281455,719.1735329775908, 0.0));
    positionAlloc4->Add (Vector(678.4851132044142,212.28287510853687, 0.0));
    positionAlloc4->Add (Vector(266.9292469367068,652.35191043992, 0.0));
    positionAlloc4->Add (Vector(569.5654555827712,806.0014767705853, 0.0));
    positionAlloc4->Add (Vector(841.7424449088819,392.454452175946, 0.0));
    positionAlloc4->Add (Vector(608.2350470579386,711.6021714545813, 0.0));
    positionAlloc4->Add (Vector(166.15983638623914,243.36686564641997, 0.0));
    positionAlloc4->Add (Vector(434.4261222919055,266.78676409458427, 0.0));
    positionAlloc4->Add (Vector(178.1947139147715,420.49089328140684, 0.0));
    positionAlloc4->Add (Vector(407.8621553093138,649.1197416023676, 0.0));
    positionAlloc4->Add (Vector(797.0214485052004,648.6758199310452, 0.0));
    positionAlloc4->Add (Vector(552.0999381022011,636.9481510032551, 0.0));
    positionAlloc4->Add (Vector(796.9035643956893,692.7210277162633, 0.0));
    positionAlloc4->Add (Vector(575.9532290795505,328.2388066570268, 0.0));
    positionAlloc4->Add (Vector(709.1402803802399,811.6328327889046, 0.0));
    positionAlloc4->Add (Vector(440.99791640489383,659.8208573456143, 0.0));
    positionAlloc4->Add (Vector(341.0420122270599,151.71821051069097, 0.0));
    positionAlloc4->Add (Vector(731.789051702422,160.83772139626075, 0.0));
    positionAlloc4->Add (Vector(254.56918273985954,478.41623399717906, 0.0));
    positionAlloc4->Add (Vector(472.228889344483,546.5266241857439, 0.0));
    positionAlloc4->Add (Vector(784.8109822733369,489.1309663232348, 0.0));
    positionAlloc4->Add (Vector(641.9480140161825,179.40403719316834, 0.0));
    positionAlloc4->Add (Vector(428.51501123415795,817.11132322831, 0.0));
    positionAlloc4->Add (Vector(619.2908602612696,801.8866391809364, 0.0));
    positionAlloc4->Add (Vector(762.4839410368824,796.8544637059886, 0.0));
    positionAlloc4->Add (Vector(822.1766276410062,327.5686213851875, 0.0));
    positionAlloc4->Add (Vector(508.881650630168,623.0227322430239, 0.0));
    positionAlloc4->Add (Vector(623.2392035092772,399.88858449807907, 0.0));
    positionAlloc4->Add (Vector(599.0432583450015,763.8432523095929, 0.0));
    positionAlloc4->Add (Vector(186.64413676672172,814.0593103945954, 0.0));
    positionAlloc4->Add (Vector(563.87003402543,370.3499366018706, 0.0));
    positionAlloc4->Add (Vector(523.1213930417789,246.5291752112338, 0.0));
    positionAlloc4->Add (Vector(330.0239952170991,544.8594451820376, 0.0));
    positionAlloc4->Add (Vector(798.6952530373634,639.0598860568681, 0.0));
    positionAlloc4->Add (Vector(759.2875799908102,401.6115183275674, 0.0));
    positionAlloc4->Add (Vector(744.4534012858734,486.1868479659392, 0.0));
    positionAlloc4->Add (Vector(612.3726908055403,580.2499732544202, 0.0));
    positionAlloc4->Add (Vector(759.038635176498,624.8752024841351, 0.0));
    positionAlloc4->Add (Vector(176.44807764521659,180.10977506603874, 0.0));
    positionAlloc4->Add (Vector(359.92001803906345,299.687222646685, 0.0));
    positionAlloc4->Add (Vector(765.0883086782532,399.8220635962808, 0.0));
    positionAlloc4->Add (Vector(634.8294103975704,564.5331263004528, 0.0));
    positionAlloc4->Add (Vector(470.5472059026285,548.5346412432658, 0.0));
    positionAlloc4->Add (Vector(806.268190731708,665.6597146531534, 0.0));
    positionAlloc4->Add (Vector(668.1045755883135,463.6900896256117, 0.0));
    positionAlloc4->Add (Vector(322.6321958834718,617.8524914821758, 0.0));
    positionAlloc4->Add (Vector(423.5022095782369,812.5946900851206, 0.0));
    positionAlloc4->Add (Vector(610.2459609894594,286.2334780271956, 0.0));
    positionAlloc4->Add (Vector(254.6560476644873,553.836674725926, 0.0));
    positionAlloc4->Add (Vector(774.0185451894606,266.11454726377485, 0.0));
    positionAlloc4->Add (Vector(489.7606598431853,397.7340783798011, 0.0));
    positionAlloc4->Add (Vector(370.5391785230581,516.5706799992896, 0.0));
    positionAlloc4->Add (Vector(539.9461528787419,747.0692795716242, 0.0));
    positionAlloc4->Add (Vector(432.49927471472097,641.6772997782988, 0.0));
    positionAlloc4->Add (Vector(762.4533270454542,751.1258814336244, 0.0));
    positionAlloc4->Add (Vector(306.3887814507375,206.5927611502778, 0.0));
    positionAlloc4->Add (Vector(270.0658856510309,690.0825201186642, 0.0));
    positionAlloc4->Add (Vector(305.7647309242125,849.7712436694645, 0.0));
    positionAlloc4->Add (Vector(190.69365371878,800.381446883928, 0.0));
    positionAlloc4->Add (Vector(387.6457405419599,626.110982731215, 0.0));
    positionAlloc4->Add (Vector(367.72919353988874,374.00769247294437, 0.0));
    positionAlloc4->Add (Vector(585.620749517985,613.1725425022712, 0.0));
    positionAlloc4->Add (Vector(168.06501773386177,164.19598163719397, 0.0));
    positionAlloc4->Add (Vector(783.0977396220052,288.6729359687612, 0.0));
    positionAlloc4->Add (Vector(453.1561425214918,284.4293954913029, 0.0));
    positionAlloc4->Add (Vector(296.2222249003902,692.6022544100163, 0.0));
    positionAlloc4->Add (Vector(754.5361006439673,410.0637026705013, 0.0));
    positionAlloc4->Add (Vector(553.9542098422357,761.0641112863013, 0.0));
    positionAlloc4->Add (Vector(734.1316204238399,444.20318203797865, 0.0));
    positionAlloc4->Add (Vector(675.1051969089624,300.3275660775579, 0.0));
    positionAlloc4->Add (Vector(377.37687682398825,167.7600416296209, 0.0));
    positionAlloc4->Add (Vector(651.7584016895594,566.4412146432101, 0.0));
    positionAlloc4->Add (Vector(576.4558639385482,342.43583623756854, 0.0));
    positionAlloc4->Add (Vector(224.47188378985413,496.6201145726664, 0.0));
    positionAlloc4->Add (Vector(751.3324307946592,203.70916034974414, 0.0));
    positionAlloc4->Add (Vector(291.723528547182,773.0805954168666, 0.0));
    positionAlloc4->Add (Vector(154.00465207937523,605.9239280583674, 0.0));
    positionAlloc4->Add (Vector(739.5162210429681,311.0003167838799, 0.0));
    positionAlloc4->Add (Vector(481.6659044720971,440.1645805731268, 0.0));
    positionAlloc4->Add (Vector(576.1308275906871,438.24741924004223, 0.0));
    positionAlloc4->Add (Vector(702.90767516582,609.367925273851, 0.0));
    positionAlloc4->Add (Vector(510.0682725577141,562.1316244955067, 0.0));
    positionAlloc4->Add (Vector(239.4976725360869,368.68412837574226, 0.0));
    positionAlloc4->Add (Vector(196.3181052076266,301.15674281409457, 0.0));
    positionAlloc4->Add (Vector(578.546344280051,257.0209809535987, 0.0));
    positionAlloc4->Add (Vector(734.1179504241874,402.55188075510864, 0.0));
    positionAlloc4->Add (Vector(748.8401682871703,214.32726445397438, 0.0));
    positionAlloc4->Add (Vector(723.4614037880716,205.9255814539526, 0.0));
    positionAlloc4->Add (Vector(188.83875944753743,349.17842719043927, 0.0));
    positionAlloc4->Add (Vector(509.5568702749307,636.1793818260918, 0.0));
    positionAlloc4->Add (Vector(421.10382157273096,570.1417829648065, 0.0));
    positionAlloc4->Add (Vector(412.17530671572877,206.54544971204052, 0.0));
    positionAlloc4->Add (Vector(220.16231472340027,802.5132034495426, 0.0));
    positionAlloc4->Add (Vector(464.8077874144743,530.6337191430589, 0.0));
    positionAlloc4->Add (Vector(787.1551909872185,362.87700513998584, 0.0));
    positionAlloc4->Add (Vector(559.6542305736687,346.61829114731984, 0.0));
    positionAlloc4->Add (Vector(196.08951515605827,585.5168469215255, 0.0));
    positionAlloc4->Add (Vector(517.5716786995708,408.0344567488021, 0.0));
    positionAlloc4->Add (Vector(177.0794707800341,677.4527307751156, 0.0));
    positionAlloc4->Add (Vector(210.06754019660718,828.6685663268048, 0.0));
    positionAlloc4->Add (Vector(225.70215963603906,504.5508407258477, 0.0));
    positionAlloc4->Add (Vector(814.6762059795562,387.6702433914853, 0.0));
    positionAlloc4->Add (Vector(719.8468341658868,574.2767198970812, 0.0));
    positionAlloc4->Add (Vector(646.1100059334296,244.10175798501382, 0.0));
    positionAlloc4->Add (Vector(346.4131511820583,539.7588690612201, 0.0));
    positionAlloc4->Add (Vector(588.100877949082,705.5138380416265, 0.0));
    positionAlloc4->Add (Vector(500.67851130120783,412.47518435217046, 0.0));
    positionAlloc4->Add (Vector(806.4790448946683,187.53987326001442, 0.0));
    positionAlloc4->Add (Vector(561.1906146548667,772.3646787742721, 0.0));
    positionAlloc4->Add (Vector(240.1691516340917,748.7201250540569, 0.0));
    positionAlloc4->Add (Vector(758.0397193572527,354.1037387030357, 0.0));
    positionAlloc4->Add (Vector(748.2252939133599,479.95767712109824, 0.0));
    positionAlloc4->Add (Vector(605.615186247044,482.67256861104033, 0.0));
    positionAlloc4->Add (Vector(552.467445260749,772.9517391230686, 0.0));
    positionAlloc4->Add (Vector(504.6838407044091,826.4865072864003, 0.0));
    positionAlloc4->Add (Vector(404.3747356024886,328.79979673226336, 0.0));
    positionAlloc4->Add (Vector(310.2596352976117,235.136023476066, 0.0));
    positionAlloc4->Add (Vector(746.2857661732533,516.9805495326848, 0.0));
    positionAlloc4->Add (Vector(167.94257491909582,797.3232331050532, 0.0));
    positionAlloc4->Add (Vector(638.2228870587088,504.3617252636161, 0.0));
    positionAlloc4->Add (Vector(818.246259352039,240.00408255781494, 0.0));
    positionAlloc4->Add (Vector(495.8300068550133,188.65233872528395, 0.0));
    positionAlloc4->Add (Vector(291.1899738417178,450.8217667529667, 0.0));
    positionAlloc4->Add (Vector(772.8147343481396,640.214447718665, 0.0));
    positionAlloc4->Add (Vector(643.9338629261008,554.1392076374592, 0.0));
    positionAlloc4->Add (Vector(232.585299605918,733.2466198371836, 0.0));
    positionAlloc4->Add (Vector(601.0559903447972,680.2568759729542, 0.0));
    positionAlloc4->Add (Vector(693.4739580569083,567.1487684817137, 0.0));
    positionAlloc4->Add (Vector(353.89076557901586,410.6804662247493, 0.0));
    positionAlloc4->Add (Vector(840.421789590409,211.99772167755606, 0.0));
    positionAlloc4->Add (Vector(196.7579540835204,809.6404553699502, 0.0));
    positionAlloc4->Add (Vector(395.85191374694165,736.0679996608216, 0.0));
    positionAlloc4->Add (Vector(684.5529227662539,217.8659748387003, 0.0));
    positionAlloc4->Add (Vector(435.9411722175913,394.0919675900849, 0.0));
    positionAlloc4->Add (Vector(271.3612097302557,388.61357770875554, 0.0));
    positionAlloc4->Add (Vector(723.5336266092028,241.4887776238191, 0.0));
    positionAlloc4->Add (Vector(700.6567612534427,387.2553539559184, 0.0));
    positionAlloc4->Add (Vector(651.2893483337162,391.10680189995696, 0.0));
    positionAlloc4->Add (Vector(303.99378882785146,156.17038330613084, 0.0));
    positionAlloc4->Add (Vector(655.7033961156892,209.53053491424978, 0.0));
    positionAlloc4->Add (Vector(791.8851243129518,644.7356973031417, 0.0));
    positionAlloc4->Add (Vector(821.3307978099854,501.6819554360887, 0.0));
    positionAlloc4->Add (Vector(705.9138961863267,291.727279467025, 0.0));
    positionAlloc4->Add (Vector(360.6704008910707,674.2423483349637, 0.0));
    positionAlloc4->Add (Vector(299.7536479607331,154.06263819005878, 0.0));
    positionAlloc4->Add (Vector(453.58785317487167,291.5912272194213, 0.0));
    positionAlloc4->Add (Vector(480.72347974042333,638.8193267583921, 0.0));
    positionAlloc4->Add (Vector(487.7160292667823,568.1445106844972, 0.0));
    positionAlloc4->Add (Vector(561.2932405048589,397.48419794303595, 0.0));
    positionAlloc4->Add (Vector(706.7009313580558,591.5458131734524, 0.0));
    positionAlloc4->Add (Vector(643.5241655843329,308.8093614320459, 0.0));
    positionAlloc4->Add (Vector(183.50591923427496,435.50201791363935, 0.0));
    positionAlloc4->Add (Vector(387.361329283876,646.9268033670176, 0.0));
    positionAlloc4->Add (Vector(277.85613312064055,675.3667864877243, 0.0));
    positionAlloc4->Add (Vector(257.85301273115846,602.5745691465128, 0.0));
    positionAlloc4->Add (Vector(540.4353313686432,605.9352740601439, 0.0));
    positionAlloc4->Add (Vector(448.51066400001224,574.3354761968983, 0.0));
    positionAlloc4->Add (Vector(465.7532502774018,160.7563755143063, 0.0));
    positionAlloc4->Add (Vector(764.008430259836,757.4231730572114, 0.0));
    positionAlloc4->Add (Vector(466.0824526811395,555.7276294597987, 0.0));
    positionAlloc4->Add (Vector(310.2910297102182,518.6495844676383, 0.0));
    positionAlloc4->Add (Vector(265.7806331509513,694.0128373439165, 0.0));
    positionAlloc4->Add (Vector(307.32502672415023,286.62223052800437, 0.0));
    positionAlloc4->Add (Vector(693.306304873936,490.48636929376056, 0.0));
    positionAlloc4->Add (Vector(404.94155585466126,296.95228217143904, 0.0));
    positionAlloc4->Add (Vector(679.5868620404714,577.3592148645121, 0.0));
    positionAlloc4->Add (Vector(404.9069733732239,746.6557444834306, 0.0));
    positionAlloc4->Add (Vector(273.92900874193447,593.8005371911917, 0.0));
    positionAlloc4->Add (Vector(410.7282572384758,469.01506980593933, 0.0));
    positionAlloc4->Add (Vector(635.6305442443527,426.24034893422237, 0.0));
    positionAlloc4->Add (Vector(506.3024118378328,771.472680475499, 0.0));
    positionAlloc4->Add (Vector(625.1431884643002,311.762245494198, 0.0));
    positionAlloc4->Add (Vector(157.80821111828135,319.628011172425, 0.0));
    positionAlloc4->Add (Vector(349.5932226264961,286.7958106468443, 0.0));
    positionAlloc4->Add (Vector(830.7601375118198,176.28924881813268, 0.0));
    positionAlloc4->Add (Vector(808.1232443107397,683.450157407808, 0.0));
    positionAlloc4->Add (Vector(538.5129669437038,351.04879978915307, 0.0));
    positionAlloc4->Add (Vector(613.5301199982835,231.32654250832366, 0.0));
    positionAlloc4->Add (Vector(588.5246986778391,526.655680011838, 0.0));
    positionAlloc4->Add (Vector(217.37254379850145,562.4482064168343, 0.0));
    positionAlloc4->Add (Vector(266.0243107719184,837.4312359504687, 0.0));
    positionAlloc4->Add (Vector(279.0577666137857,460.1531530534142, 0.0));
    positionAlloc4->Add (Vector(659.1781056921661,409.4974599264488, 0.0));
    positionAlloc4->Add (Vector(827.5019468569911,484.5234626620247, 0.0));
    positionAlloc4->Add (Vector(767.9194533449563,716.6312467308042, 0.0));
    positionAlloc4->Add (Vector(818.5689893159803,285.110489783989, 0.0));
    positionAlloc4->Add (Vector(657.7884862350793,197.8438600137363, 0.0));
    positionAlloc4->Add (Vector(709.0811596512538,320.31134958807695, 0.0));
    positionAlloc4->Add (Vector(469.1411559154796,199.84470227886277, 0.0));
    positionAlloc4->Add (Vector(457.6137563050442,358.49314385201524, 0.0));
    positionAlloc4->Add (Vector(323.14596027242,662.1521327439216, 0.0));
    positionAlloc4->Add (Vector(483.1486881554743,818.0421661363106, 0.0));
    positionAlloc4->Add (Vector(281.7399871759036,349.6945978586158, 0.0));
    positionAlloc4->Add (Vector(762.524171871681,579.9138918313392, 0.0));
    positionAlloc4->Add (Vector(159.78719907995531,690.2638279347642, 0.0));
    positionAlloc4->Add (Vector(293.2155300042431,277.05557313504687, 0.0));
    positionAlloc4->Add (Vector(786.2212082705275,300.51579854932925, 0.0));
    positionAlloc4->Add (Vector(643.2102224933451,219.43617107901503, 0.0));
    positionAlloc4->Add (Vector(152.3714935878474,321.70306555055004, 0.0));
    positionAlloc4->Add (Vector(562.2605539551441,247.43146193059093, 0.0));
    positionAlloc4->Add (Vector(767.433730205389,461.0756732410388, 0.0));
    positionAlloc4->Add (Vector(479.353555294629,183.57729100563313, 0.0));
    positionAlloc4->Add (Vector(462.28001498763143,545.372819772138, 0.0));
    positionAlloc4->Add (Vector(452.48202293112615,527.1735570747683, 0.0));
    positionAlloc4->Add (Vector(417.6552185416976,804.1906507802972, 0.0));
    positionAlloc4->Add (Vector(741.0984331840608,548.3990993179877, 0.0));
    positionAlloc4->Add (Vector(805.6815486670047,728.8643175588776, 0.0));
    positionAlloc4->Add (Vector(844.8129131536913,625.5754416786983, 0.0));
    positionAlloc4->Add (Vector(774.0177112013246,743.3768385735516, 0.0));
    positionAlloc4->Add (Vector(300.1207605269131,201.91137404182152, 0.0));
    positionAlloc4->Add (Vector(482.6384036198646,811.5822551397822, 0.0));
    positionAlloc4->Add (Vector(550.3087825602124,776.0120253230536, 0.0));
    positionAlloc4->Add (Vector(675.5679669621073,335.88092936226894, 0.0));
    positionAlloc4->Add (Vector(460.76147222870327,678.5657771087025, 0.0));
    positionAlloc4->Add (Vector(404.8192284501964,821.6077170188154, 0.0));
    positionAlloc4->Add (Vector(303.9180927951444,407.81726746071223, 0.0));
    positionAlloc4->Add (Vector(538.5578210063558,669.7202949494896, 0.0));
    positionAlloc4->Add (Vector(295.8288628020278,517.2818884654341, 0.0));
    positionAlloc4->Add (Vector(736.7114409807607,517.6415401615579, 0.0));
    positionAlloc4->Add (Vector(823.6040791708108,607.5220667746585, 0.0));
    positionAlloc4->Add (Vector(580.3396672093836,781.9432334351815, 0.0));
    positionAlloc4->Add (Vector(400.34838471223793,353.52070612347177, 0.0));
    positionAlloc4->Add (Vector(244.16784460242917,270.95275631772427, 0.0));
    positionAlloc4->Add (Vector(800.3534521797766,762.007592155522, 0.0));
    positionAlloc4->Add (Vector(422.6844151370657,507.9070223722242, 0.0));
    positionAlloc4->Add (Vector(248.04519155125553,393.792230754234, 0.0));
    positionAlloc4->Add (Vector(525.5882697075596,676.3133710584336, 0.0));
    positionAlloc4->Add (Vector(673.8231607857435,758.130061375611, 0.0));
    positionAlloc4->Add (Vector(604.122856065658,759.8309324452224, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_15.xml");
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
	  flowmon->SerializeToXmlFile ("switch_SUI_flowmon/switch_SUI_15.flowmon", false, false);
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