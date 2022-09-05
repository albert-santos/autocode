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
	  uint16_t numberOfRrhs = 33;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 720;
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
    positionAlloc->Add (Vector (772.2222222222222,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,850.0, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (150.0,227.77777777777777, 0.0));
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
    positionAlloc4->Add (Vector(291.29741648606387,621.4027033274422, 0.0));
    positionAlloc4->Add (Vector(365.55095911383364,643.2765339156526, 0.0));
    positionAlloc4->Add (Vector(247.75587832941903,170.98151291087456, 0.0));
    positionAlloc4->Add (Vector(492.1926061798441,616.804980282273, 0.0));
    positionAlloc4->Add (Vector(468.3178989437014,510.91722000547514, 0.0));
    positionAlloc4->Add (Vector(597.3839564708503,757.5428061640155, 0.0));
    positionAlloc4->Add (Vector(153.45798665408674,816.2682497962672, 0.0));
    positionAlloc4->Add (Vector(840.6410860711928,825.5830875052319, 0.0));
    positionAlloc4->Add (Vector(179.10100133527447,253.8402406734622, 0.0));
    positionAlloc4->Add (Vector(371.30927348372535,505.0643660818747, 0.0));
    positionAlloc4->Add (Vector(466.5393197984047,803.7125122695335, 0.0));
    positionAlloc4->Add (Vector(177.54047846795675,235.47693980701877, 0.0));
    positionAlloc4->Add (Vector(521.8186696007224,805.6239846064731, 0.0));
    positionAlloc4->Add (Vector(338.82472806189276,189.9607992625204, 0.0));
    positionAlloc4->Add (Vector(294.0888470257378,228.3437335784095, 0.0));
    positionAlloc4->Add (Vector(559.7480550777266,757.7898963592811, 0.0));
    positionAlloc4->Add (Vector(740.919314278438,713.9691070650372, 0.0));
    positionAlloc4->Add (Vector(614.6631693352635,482.2416130248697, 0.0));
    positionAlloc4->Add (Vector(175.66999885348008,665.4814105459484, 0.0));
    positionAlloc4->Add (Vector(266.9293355306649,267.33345140182234, 0.0));
    positionAlloc4->Add (Vector(582.001490622821,644.7440518198121, 0.0));
    positionAlloc4->Add (Vector(451.62126656326416,324.09999081134094, 0.0));
    positionAlloc4->Add (Vector(385.9022235019004,730.2183561178266, 0.0));
    positionAlloc4->Add (Vector(268.8305335612785,513.8417639892124, 0.0));
    positionAlloc4->Add (Vector(315.2353243425248,201.44772313100884, 0.0));
    positionAlloc4->Add (Vector(828.8747315549813,467.6526789685475, 0.0));
    positionAlloc4->Add (Vector(691.4311879806205,241.6115625028449, 0.0));
    positionAlloc4->Add (Vector(609.7156651789046,377.6330449572864, 0.0));
    positionAlloc4->Add (Vector(828.5886256347012,330.6934028907893, 0.0));
    positionAlloc4->Add (Vector(264.4667108095159,423.7570756562325, 0.0));
    positionAlloc4->Add (Vector(401.463358579836,565.0655960044492, 0.0));
    positionAlloc4->Add (Vector(461.71367850186584,510.37214775078314, 0.0));
    positionAlloc4->Add (Vector(323.06175208222396,759.524930890758, 0.0));
    positionAlloc4->Add (Vector(748.0695653587054,494.5944995136243, 0.0));
    positionAlloc4->Add (Vector(829.9765658291477,276.6838668966428, 0.0));
    positionAlloc4->Add (Vector(275.7781762091694,316.28032744671435, 0.0));
    positionAlloc4->Add (Vector(651.077475277462,786.3227644659487, 0.0));
    positionAlloc4->Add (Vector(663.1791156906967,242.09737958908187, 0.0));
    positionAlloc4->Add (Vector(586.076608494438,592.0921540647644, 0.0));
    positionAlloc4->Add (Vector(788.6543419316837,297.46353230396664, 0.0));
    positionAlloc4->Add (Vector(343.77945806453613,564.6452300153535, 0.0));
    positionAlloc4->Add (Vector(780.491304807881,445.2046488114188, 0.0));
    positionAlloc4->Add (Vector(490.668258997292,761.3962666571624, 0.0));
    positionAlloc4->Add (Vector(457.0307934327133,657.6803622905956, 0.0));
    positionAlloc4->Add (Vector(756.922047285714,555.0727860006754, 0.0));
    positionAlloc4->Add (Vector(841.3546946152405,178.07643398873773, 0.0));
    positionAlloc4->Add (Vector(392.15449478287405,409.72713408922186, 0.0));
    positionAlloc4->Add (Vector(669.8945648399867,447.84533411365277, 0.0));
    positionAlloc4->Add (Vector(757.7907537858664,659.0183488364156, 0.0));
    positionAlloc4->Add (Vector(330.37802195784957,473.8360697175214, 0.0));
    positionAlloc4->Add (Vector(200.53888628542308,171.45675154406425, 0.0));
    positionAlloc4->Add (Vector(289.6797156504016,626.159769701904, 0.0));
    positionAlloc4->Add (Vector(832.3552135027481,605.2279804428558, 0.0));
    positionAlloc4->Add (Vector(309.9183189319091,847.4337412397193, 0.0));
    positionAlloc4->Add (Vector(538.7544856563418,653.4412447905847, 0.0));
    positionAlloc4->Add (Vector(611.0954428506133,498.7370015171973, 0.0));
    positionAlloc4->Add (Vector(277.4825409820895,702.7889549997354, 0.0));
    positionAlloc4->Add (Vector(833.1056790733919,385.8270542397748, 0.0));
    positionAlloc4->Add (Vector(760.2484966145412,303.44979115063103, 0.0));
    positionAlloc4->Add (Vector(322.97226380710254,261.68127607923185, 0.0));
    positionAlloc4->Add (Vector(207.14098742141272,192.24645879339465, 0.0));
    positionAlloc4->Add (Vector(518.4818626552792,662.1126652287323, 0.0));
    positionAlloc4->Add (Vector(466.6227991628101,600.5043866375674, 0.0));
    positionAlloc4->Add (Vector(533.3146633076153,722.1084389737456, 0.0));
    positionAlloc4->Add (Vector(206.0803924692726,776.3716592423693, 0.0));
    positionAlloc4->Add (Vector(387.61513477996425,720.3775796225585, 0.0));
    positionAlloc4->Add (Vector(649.8608524926995,711.2096581488543, 0.0));
    positionAlloc4->Add (Vector(646.3423636156415,759.67838662226, 0.0));
    positionAlloc4->Add (Vector(186.3798985560307,528.9074347107494, 0.0));
    positionAlloc4->Add (Vector(172.11030137794944,805.3485723705828, 0.0));
    positionAlloc4->Add (Vector(595.5011746805457,644.5330085408308, 0.0));
    positionAlloc4->Add (Vector(267.2378075813488,223.1974882340802, 0.0));
    positionAlloc4->Add (Vector(710.410689376358,290.1051709713671, 0.0));
    positionAlloc4->Add (Vector(547.2785937830906,632.625478396086, 0.0));
    positionAlloc4->Add (Vector(427.84629334111725,242.90102783014225, 0.0));
    positionAlloc4->Add (Vector(173.2421761539566,205.00494800851453, 0.0));
    positionAlloc4->Add (Vector(211.50654940138335,395.43885184642653, 0.0));
    positionAlloc4->Add (Vector(378.5229157707943,512.398150369298, 0.0));
    positionAlloc4->Add (Vector(541.7540784408399,156.83959319342856, 0.0));
    positionAlloc4->Add (Vector(582.7090140226834,641.7254793808727, 0.0));
    positionAlloc4->Add (Vector(249.74688003105928,499.22170741594925, 0.0));
    positionAlloc4->Add (Vector(319.43151110376925,840.6638152870025, 0.0));
    positionAlloc4->Add (Vector(486.35486308048024,223.45112489272242, 0.0));
    positionAlloc4->Add (Vector(695.8956922253449,177.73317774385, 0.0));
    positionAlloc4->Add (Vector(636.4873202885798,752.0856780324432, 0.0));
    positionAlloc4->Add (Vector(570.0143836835126,354.16380509651225, 0.0));
    positionAlloc4->Add (Vector(424.74548948467935,485.95718430198644, 0.0));
    positionAlloc4->Add (Vector(158.13437793567044,200.71282145619122, 0.0));
    positionAlloc4->Add (Vector(367.2533712465016,468.65410435726085, 0.0));
    positionAlloc4->Add (Vector(338.2953831045684,708.2913704460317, 0.0));
    positionAlloc4->Add (Vector(194.94625595259384,557.3799813400792, 0.0));
    positionAlloc4->Add (Vector(290.36730869023575,494.1161827190058, 0.0));
    positionAlloc4->Add (Vector(629.89570106028,451.2141264296239, 0.0));
    positionAlloc4->Add (Vector(232.11573227367614,237.6300545417853, 0.0));
    positionAlloc4->Add (Vector(707.5761036833234,487.06564605878503, 0.0));
    positionAlloc4->Add (Vector(794.5934934709808,581.6552201645165, 0.0));
    positionAlloc4->Add (Vector(241.8714143776713,172.76832564387635, 0.0));
    positionAlloc4->Add (Vector(682.7167936600993,588.3542251419908, 0.0));
    positionAlloc4->Add (Vector(689.6452912508364,741.3482596818213, 0.0));
    positionAlloc4->Add (Vector(758.0362175689276,693.6177969542192, 0.0));
    positionAlloc4->Add (Vector(827.7353995403053,279.8678609921848, 0.0));
    positionAlloc4->Add (Vector(755.0507748877459,481.24693468409004, 0.0));
    positionAlloc4->Add (Vector(606.1667777631195,407.9684406066328, 0.0));
    positionAlloc4->Add (Vector(299.1732928175583,171.79385801915794, 0.0));
    positionAlloc4->Add (Vector(775.8694425753941,310.96822412376036, 0.0));
    positionAlloc4->Add (Vector(838.7843535718797,436.5338044205301, 0.0));
    positionAlloc4->Add (Vector(428.316078573956,341.95064164406904, 0.0));
    positionAlloc4->Add (Vector(443.8091218482856,596.96816336486, 0.0));
    positionAlloc4->Add (Vector(452.63436072982137,810.5311441173192, 0.0));
    positionAlloc4->Add (Vector(815.6044935752873,255.74052424663017, 0.0));
    positionAlloc4->Add (Vector(608.1578499967947,341.2816688987465, 0.0));
    positionAlloc4->Add (Vector(207.66673193108988,626.3495114522991, 0.0));
    positionAlloc4->Add (Vector(635.577558047042,304.53872895213226, 0.0));
    positionAlloc4->Add (Vector(264.5366748341684,706.3666841249506, 0.0));
    positionAlloc4->Add (Vector(448.32691156518104,465.97091703077285, 0.0));
    positionAlloc4->Add (Vector(341.53866639716614,847.9871501103978, 0.0));
    positionAlloc4->Add (Vector(628.3067636245537,462.1318944637238, 0.0));
    positionAlloc4->Add (Vector(492.05023328326604,689.2851924654552, 0.0));
    positionAlloc4->Add (Vector(165.48648607029807,376.52715340467887, 0.0));
    positionAlloc4->Add (Vector(680.7842371503158,167.21084152116106, 0.0));
    positionAlloc4->Add (Vector(674.7562822200473,241.0603680914424, 0.0));
    positionAlloc4->Add (Vector(831.4422784241779,828.214498699984, 0.0));
    positionAlloc4->Add (Vector(182.019423668759,760.0757643066634, 0.0));
    positionAlloc4->Add (Vector(392.1181398670697,584.640862437234, 0.0));
    positionAlloc4->Add (Vector(197.1454210281402,557.5193221233912, 0.0));
    positionAlloc4->Add (Vector(360.6744968627714,645.720462051697, 0.0));
    positionAlloc4->Add (Vector(631.5200056105693,197.48594137362034, 0.0));
    positionAlloc4->Add (Vector(221.75680959347548,806.0599410655158, 0.0));
    positionAlloc4->Add (Vector(592.8353251937638,802.4196738570122, 0.0));
    positionAlloc4->Add (Vector(824.0944027012963,389.31051622708026, 0.0));
    positionAlloc4->Add (Vector(682.7195317514196,528.8866050732772, 0.0));
    positionAlloc4->Add (Vector(695.8917766058005,224.88644717887087, 0.0));
    positionAlloc4->Add (Vector(593.803992169728,527.9897658010989, 0.0));
    positionAlloc4->Add (Vector(592.2694689116679,331.86250674491185, 0.0));
    positionAlloc4->Add (Vector(581.8488428630196,836.8303246191964, 0.0));
    positionAlloc4->Add (Vector(154.4701102017835,350.926693222439, 0.0));
    positionAlloc4->Add (Vector(732.4939245704194,505.22741363258535, 0.0));
    positionAlloc4->Add (Vector(262.53192723381574,722.5769168833435, 0.0));
    positionAlloc4->Add (Vector(384.58319798139934,670.4557794041255, 0.0));
    positionAlloc4->Add (Vector(728.0717253953338,240.20890723141164, 0.0));
    positionAlloc4->Add (Vector(760.2904694216624,170.4730981241914, 0.0));
    positionAlloc4->Add (Vector(261.0235737051516,766.6095314392498, 0.0));
    positionAlloc4->Add (Vector(583.027964750629,676.9016476903489, 0.0));
    positionAlloc4->Add (Vector(638.2300231684985,626.0985402322103, 0.0));
    positionAlloc4->Add (Vector(645.8540837005244,735.9093298545704, 0.0));
    positionAlloc4->Add (Vector(745.4072103659694,757.1059406718616, 0.0));
    positionAlloc4->Add (Vector(708.1982151248496,610.9162842608978, 0.0));
    positionAlloc4->Add (Vector(336.91978911859496,765.0409904486106, 0.0));
    positionAlloc4->Add (Vector(624.6113067913445,546.0123747970538, 0.0));
    positionAlloc4->Add (Vector(727.2766761476045,393.4664455315351, 0.0));
    positionAlloc4->Add (Vector(486.4106041286536,564.6854492616624, 0.0));
    positionAlloc4->Add (Vector(745.844839834138,496.602568909782, 0.0));
    positionAlloc4->Add (Vector(628.2811345617172,203.29160164321013, 0.0));
    positionAlloc4->Add (Vector(206.73235113535355,304.1282413648679, 0.0));
    positionAlloc4->Add (Vector(656.3940795820095,497.11301455513956, 0.0));
    positionAlloc4->Add (Vector(410.8819619086198,331.8276486322284, 0.0));
    positionAlloc4->Add (Vector(210.89073193798708,435.91724914948907, 0.0));
    positionAlloc4->Add (Vector(608.6693864473152,535.6001594263976, 0.0));
    positionAlloc4->Add (Vector(753.4893320858155,716.6612523074147, 0.0));
    positionAlloc4->Add (Vector(772.123655908284,818.9560487608273, 0.0));
    positionAlloc4->Add (Vector(335.92002981209424,240.66044489796292, 0.0));
    positionAlloc4->Add (Vector(214.0407861784284,764.2226038412663, 0.0));
    positionAlloc4->Add (Vector(565.0579543891922,556.0603454864679, 0.0));
    positionAlloc4->Add (Vector(306.67233037583617,525.8820960253895, 0.0));
    positionAlloc4->Add (Vector(721.0831874934324,307.10840028566247, 0.0));
    positionAlloc4->Add (Vector(637.2144326376546,257.72713443789854, 0.0));
    positionAlloc4->Add (Vector(427.36941323186113,680.7669329928639, 0.0));
    positionAlloc4->Add (Vector(846.4354758525047,271.7266744536411, 0.0));
    positionAlloc4->Add (Vector(768.650119474398,799.426598236649, 0.0));
    positionAlloc4->Add (Vector(190.48987359437746,828.371841098568, 0.0));
    positionAlloc4->Add (Vector(695.7091314967273,395.08768141671106, 0.0));
    positionAlloc4->Add (Vector(304.34034634912274,280.50678350097814, 0.0));
    positionAlloc4->Add (Vector(311.46886820243105,273.56962240385576, 0.0));
    positionAlloc4->Add (Vector(575.4157564152014,508.881957711556, 0.0));
    positionAlloc4->Add (Vector(809.912241894588,604.752654294658, 0.0));
    positionAlloc4->Add (Vector(843.8180382958835,312.17019096841034, 0.0));
    positionAlloc4->Add (Vector(579.3879724474954,266.92376226445265, 0.0));
    positionAlloc4->Add (Vector(811.3124156039872,477.1810578749088, 0.0));
    positionAlloc4->Add (Vector(442.30654304944983,765.3828152935153, 0.0));
    positionAlloc4->Add (Vector(700.4415100908865,212.58770161858166, 0.0));
    positionAlloc4->Add (Vector(302.77772150554495,814.9728287651577, 0.0));
    positionAlloc4->Add (Vector(419.0286530117772,625.7530978851489, 0.0));
    positionAlloc4->Add (Vector(841.405672815222,449.44119361510366, 0.0));
    positionAlloc4->Add (Vector(179.4652327454533,162.51175431338967, 0.0));
    positionAlloc4->Add (Vector(357.415684319193,313.52542573493696, 0.0));
    positionAlloc4->Add (Vector(398.2355103485436,351.0672941578572, 0.0));
    positionAlloc4->Add (Vector(317.1440020378117,227.33187919197627, 0.0));
    positionAlloc4->Add (Vector(444.9499015939491,666.3076111122241, 0.0));
    positionAlloc4->Add (Vector(687.7913897051627,442.43674751713365, 0.0));
    positionAlloc4->Add (Vector(761.5739799304371,838.8584284395066, 0.0));
    positionAlloc4->Add (Vector(485.9129342127362,589.253824252859, 0.0));
    positionAlloc4->Add (Vector(687.0318022517888,431.3516395264695, 0.0));
    positionAlloc4->Add (Vector(848.5538773733363,395.4982708161971, 0.0));
    positionAlloc4->Add (Vector(807.3473889590273,709.1482650415658, 0.0));
    positionAlloc4->Add (Vector(762.2911708789396,520.2559938190484, 0.0));
    positionAlloc4->Add (Vector(759.0692117306509,164.888587595271, 0.0));
    positionAlloc4->Add (Vector(313.0716323275436,167.0805470895766, 0.0));
    positionAlloc4->Add (Vector(770.184423765331,581.1042317276132, 0.0));
    positionAlloc4->Add (Vector(373.126752052449,588.1239635059064, 0.0));
    positionAlloc4->Add (Vector(527.6439046627361,703.5062153872744, 0.0));
    positionAlloc4->Add (Vector(195.74943855925966,370.7032364814272, 0.0));
    positionAlloc4->Add (Vector(387.9124746376463,657.336740070133, 0.0));
    positionAlloc4->Add (Vector(203.86724114044642,637.4021431146978, 0.0));
    positionAlloc4->Add (Vector(311.00618610139213,234.39682004409516, 0.0));
    positionAlloc4->Add (Vector(175.90534430084483,567.3889149374371, 0.0));
    positionAlloc4->Add (Vector(373.37033127027894,730.2408319054741, 0.0));
    positionAlloc4->Add (Vector(753.4298556925194,741.1845789745493, 0.0));
    positionAlloc4->Add (Vector(810.9865423794038,481.94995166022466, 0.0));
    positionAlloc4->Add (Vector(797.0773367635712,829.7407716157575, 0.0));
    positionAlloc4->Add (Vector(638.1941870727879,358.1443051607855, 0.0));
    positionAlloc4->Add (Vector(790.8418083116293,173.7621850100592, 0.0));
    positionAlloc4->Add (Vector(276.4027415891335,547.5616613868406, 0.0));
    positionAlloc4->Add (Vector(643.9026207609819,797.6738221909661, 0.0));
    positionAlloc4->Add (Vector(810.8415377806293,177.59876795795412, 0.0));
    positionAlloc4->Add (Vector(304.8466092737216,841.3676055976529, 0.0));
    positionAlloc4->Add (Vector(324.76760620143017,492.59747455019834, 0.0));
    positionAlloc4->Add (Vector(634.5935951624967,744.4369560957235, 0.0));
    positionAlloc4->Add (Vector(714.5928035030148,611.1483566975021, 0.0));
    positionAlloc4->Add (Vector(761.0620851134858,662.5574204866929, 0.0));
    positionAlloc4->Add (Vector(279.5930501870338,529.3306631648506, 0.0));
    positionAlloc4->Add (Vector(815.1661476745617,462.204041070569, 0.0));
    positionAlloc4->Add (Vector(792.11950282357,245.773041643273, 0.0));
    positionAlloc4->Add (Vector(402.68172785801875,214.1074513286439, 0.0));
    positionAlloc4->Add (Vector(340.5015960609739,563.161466808361, 0.0));
    positionAlloc4->Add (Vector(709.5911152782854,636.2875295980987, 0.0));
    positionAlloc4->Add (Vector(322.4814232673748,723.3716223719398, 0.0));
    positionAlloc4->Add (Vector(631.8816774994152,454.37913702262233, 0.0));
    positionAlloc4->Add (Vector(659.7321293425223,496.8439213443389, 0.0));
    positionAlloc4->Add (Vector(224.61877719364026,202.59644721506612, 0.0));
    positionAlloc4->Add (Vector(383.97072357187255,689.6385426220753, 0.0));
    positionAlloc4->Add (Vector(497.45637304086546,567.7899942418225, 0.0));
    positionAlloc4->Add (Vector(566.5545514901784,185.99806943897525, 0.0));
    positionAlloc4->Add (Vector(171.82819454289927,333.5848310098776, 0.0));
    positionAlloc4->Add (Vector(662.8004680471752,300.45826501352974, 0.0));
    positionAlloc4->Add (Vector(614.5698157538536,805.7808175273306, 0.0));
    positionAlloc4->Add (Vector(546.483248207828,483.2352815393698, 0.0));
    positionAlloc4->Add (Vector(653.3321336289719,357.98520093622255, 0.0));
    positionAlloc4->Add (Vector(352.0856157394899,398.9709015254644, 0.0));
    positionAlloc4->Add (Vector(321.7443767769247,422.5069298293836, 0.0));
    positionAlloc4->Add (Vector(207.95906754758852,572.2064766264717, 0.0));
    positionAlloc4->Add (Vector(266.3983581292556,666.6235314984593, 0.0));
    positionAlloc4->Add (Vector(513.2423936979862,182.87617674253852, 0.0));
    positionAlloc4->Add (Vector(443.13936742445117,418.1969697746482, 0.0));
    positionAlloc4->Add (Vector(272.7598950364147,790.0883621474718, 0.0));
    positionAlloc4->Add (Vector(570.3618023035574,615.5256721823789, 0.0));
    positionAlloc4->Add (Vector(761.7230542937607,568.1891715192157, 0.0));
    positionAlloc4->Add (Vector(245.8199410481651,780.0130492936736, 0.0));
    positionAlloc4->Add (Vector(294.4948902324222,157.69708006093634, 0.0));
    positionAlloc4->Add (Vector(605.994271846305,730.8192625950411, 0.0));
    positionAlloc4->Add (Vector(341.8386544810442,537.9246017610553, 0.0));
    positionAlloc4->Add (Vector(232.4257580230988,553.5615328599774, 0.0));
    positionAlloc4->Add (Vector(162.8734735832267,790.0091645923493, 0.0));
    positionAlloc4->Add (Vector(270.17550009381245,340.6151285635583, 0.0));
    positionAlloc4->Add (Vector(839.0815264924864,517.98312761188, 0.0));
    positionAlloc4->Add (Vector(247.54925185097687,734.4511657595094, 0.0));
    positionAlloc4->Add (Vector(593.4347538783031,330.5213798032416, 0.0));
    positionAlloc4->Add (Vector(338.0741515694623,529.1439502626068, 0.0));
    positionAlloc4->Add (Vector(447.6494531284634,165.72851582408217, 0.0));
    positionAlloc4->Add (Vector(776.186558825864,709.8986791631206, 0.0));
    positionAlloc4->Add (Vector(757.9275831285389,491.20183877465956, 0.0));
    positionAlloc4->Add (Vector(684.5240559118325,248.0792230365168, 0.0));
    positionAlloc4->Add (Vector(356.12603167934236,813.9173148735097, 0.0));
    positionAlloc4->Add (Vector(536.4330265963862,455.28857039472166, 0.0));
    positionAlloc4->Add (Vector(586.9042354943938,436.5881565338683, 0.0));
    positionAlloc4->Add (Vector(563.2064597603654,337.6445485760134, 0.0));
    positionAlloc4->Add (Vector(331.59640340190475,415.78468571708686, 0.0));
    positionAlloc4->Add (Vector(722.2104932169086,389.27214618851315, 0.0));
    positionAlloc4->Add (Vector(444.99718541474857,774.8701977538824, 0.0));
    positionAlloc4->Add (Vector(277.0048045628581,558.1290926787844, 0.0));
    positionAlloc4->Add (Vector(295.5203786022191,168.5725427116738, 0.0));
    positionAlloc4->Add (Vector(159.10134645751523,209.85597159812767, 0.0));
    positionAlloc4->Add (Vector(355.20341118270926,777.4350729647838, 0.0));
    positionAlloc4->Add (Vector(617.6595955994524,293.48900679107464, 0.0));
    positionAlloc4->Add (Vector(731.1539051968941,428.0284707329184, 0.0));
    positionAlloc4->Add (Vector(655.5868522989614,748.9875276744, 0.0));
    positionAlloc4->Add (Vector(219.6089749315416,320.2205336996178, 0.0));
    positionAlloc4->Add (Vector(606.9222754963123,316.7279467327086, 0.0));
    positionAlloc4->Add (Vector(709.9412597612326,274.9812634380335, 0.0));
    positionAlloc4->Add (Vector(163.79737642805492,222.87600738920167, 0.0));
    positionAlloc4->Add (Vector(311.75235761165,620.3249205191098, 0.0));
    positionAlloc4->Add (Vector(168.96719475243844,305.3805133530646, 0.0));
    positionAlloc4->Add (Vector(651.1805601931783,611.2467593371059, 0.0));
    positionAlloc4->Add (Vector(167.7704489622414,259.30681815633324, 0.0));
    positionAlloc4->Add (Vector(600.799680267311,436.1139197686675, 0.0));
    positionAlloc4->Add (Vector(411.31652139582707,339.5212712004876, 0.0));
    positionAlloc4->Add (Vector(753.2499347094621,812.2808234935429, 0.0));
    positionAlloc4->Add (Vector(258.66817215149877,277.3496913466898, 0.0));
    positionAlloc4->Add (Vector(430.5341426127282,541.7482319157525, 0.0));
    positionAlloc4->Add (Vector(517.073188529306,458.5339228567597, 0.0));
    positionAlloc4->Add (Vector(692.8693316865074,467.3021843236193, 0.0));
    positionAlloc4->Add (Vector(545.3887932838315,195.99604759984666, 0.0));
    positionAlloc4->Add (Vector(175.58476749366642,480.4468615769995, 0.0));
    positionAlloc4->Add (Vector(293.59774833740505,283.4814044645666, 0.0));
    positionAlloc4->Add (Vector(296.93391516691,778.6380026084996, 0.0));
    positionAlloc4->Add (Vector(196.95353417189924,674.9723292794245, 0.0));
    positionAlloc4->Add (Vector(501.5365072318698,566.7695121240306, 0.0));
    positionAlloc4->Add (Vector(209.7447676550507,847.8119762921632, 0.0));
    positionAlloc4->Add (Vector(254.06390942717906,440.2368866089166, 0.0));
    positionAlloc4->Add (Vector(274.625176808826,822.7492222286186, 0.0));
    positionAlloc4->Add (Vector(159.16611961183355,614.4761604116975, 0.0));
    positionAlloc4->Add (Vector(556.4118451946304,592.5850015436713, 0.0));
    positionAlloc4->Add (Vector(790.3921307476701,403.6757293816432, 0.0));
    positionAlloc4->Add (Vector(498.8574409176358,443.2478146531835, 0.0));
    positionAlloc4->Add (Vector(248.5999142658599,273.4708905543563, 0.0));
    positionAlloc4->Add (Vector(307.22337605798737,466.7076612928854, 0.0));
    positionAlloc4->Add (Vector(666.2508736768239,466.55255862286504, 0.0));
    positionAlloc4->Add (Vector(392.44219605617286,392.86724132735935, 0.0));
    positionAlloc4->Add (Vector(526.176922056546,793.8989314889324, 0.0));
    positionAlloc4->Add (Vector(383.44942018380095,428.7180855136603, 0.0));
    positionAlloc4->Add (Vector(711.997414018608,153.242416103222, 0.0));
    positionAlloc4->Add (Vector(188.33314717542194,384.63806915213064, 0.0));
    positionAlloc4->Add (Vector(563.8928050214505,429.3018927090912, 0.0));
    positionAlloc4->Add (Vector(758.9274534434898,513.1866399882505, 0.0));
    positionAlloc4->Add (Vector(237.26387684228678,794.5893178321606, 0.0));
    positionAlloc4->Add (Vector(839.3888392736342,737.2286605121539, 0.0));
    positionAlloc4->Add (Vector(465.58755998289246,229.2666320885287, 0.0));
    positionAlloc4->Add (Vector(804.0569103383104,797.8979590347434, 0.0));
    positionAlloc4->Add (Vector(446.89146991167024,665.510964937306, 0.0));
    positionAlloc4->Add (Vector(553.4018521167693,574.7005324679992, 0.0));
    positionAlloc4->Add (Vector(645.067044019379,256.7773300399829, 0.0));
    positionAlloc4->Add (Vector(637.6555767138245,795.749566824107, 0.0));
    positionAlloc4->Add (Vector(802.4827977385287,545.5932285124463, 0.0));
    positionAlloc4->Add (Vector(162.67712867458835,760.486736260906, 0.0));
    positionAlloc4->Add (Vector(504.1220752191262,578.0179648042536, 0.0));
    positionAlloc4->Add (Vector(257.34598218125615,560.360882411758, 0.0));
    positionAlloc4->Add (Vector(473.4040625508929,813.0983377282707, 0.0));
    positionAlloc4->Add (Vector(558.3742563362844,430.5959917345479, 0.0));
    positionAlloc4->Add (Vector(277.118401760998,785.9153929335398, 0.0));
    positionAlloc4->Add (Vector(326.25934123486695,391.9871954527436, 0.0));
    positionAlloc4->Add (Vector(635.76391112697,529.9071103433173, 0.0));
    positionAlloc4->Add (Vector(654.025575582252,598.8033430197081, 0.0));
    positionAlloc4->Add (Vector(310.475976662849,655.5767978751464, 0.0));
    positionAlloc4->Add (Vector(708.1067560456876,254.94219914414163, 0.0));
    positionAlloc4->Add (Vector(239.2420636323877,428.1011033922571, 0.0));
    positionAlloc4->Add (Vector(227.44221586864506,742.5166042141627, 0.0));
    positionAlloc4->Add (Vector(389.45722410113524,724.880113270409, 0.0));
    positionAlloc4->Add (Vector(683.7210800184833,763.7359457332246, 0.0));
    positionAlloc4->Add (Vector(173.47270431504558,548.9227794099215, 0.0));
    positionAlloc4->Add (Vector(323.942397870126,670.7624048008739, 0.0));
    positionAlloc4->Add (Vector(256.1724158129258,368.2054474568838, 0.0));
    positionAlloc4->Add (Vector(775.7248081356704,702.0180478571715, 0.0));
    positionAlloc4->Add (Vector(584.0509291499595,523.4227643834251, 0.0));
    positionAlloc4->Add (Vector(247.4904508643711,705.8176324892072, 0.0));
    positionAlloc4->Add (Vector(589.3956371499926,384.4339302599603, 0.0));
    positionAlloc4->Add (Vector(231.82862849585484,807.8824865294326, 0.0));
    positionAlloc4->Add (Vector(784.5304776751829,586.9665971397944, 0.0));
    positionAlloc4->Add (Vector(418.723854961248,530.487002779792, 0.0));
    positionAlloc4->Add (Vector(312.5488866027721,779.702201292198, 0.0));
    positionAlloc4->Add (Vector(673.3265661233758,501.90427306473543, 0.0));
    positionAlloc4->Add (Vector(473.8758499687245,360.96451191489945, 0.0));
    positionAlloc4->Add (Vector(457.3944359991529,413.8611005974353, 0.0));
    positionAlloc4->Add (Vector(424.7115751958448,721.6196076301039, 0.0));
    positionAlloc4->Add (Vector(401.1813388864517,764.0403785691758, 0.0));
    positionAlloc4->Add (Vector(615.63805448734,516.1457973383835, 0.0));
    positionAlloc4->Add (Vector(264.98605855169507,720.2023041619675, 0.0));
    positionAlloc4->Add (Vector(291.1391240334212,264.55916000599314, 0.0));
    positionAlloc4->Add (Vector(247.84066363840265,706.6871183019231, 0.0));
    positionAlloc4->Add (Vector(552.1311792259384,739.2801437307904, 0.0));
    positionAlloc4->Add (Vector(196.54738715674426,668.7844545030414, 0.0));
    positionAlloc4->Add (Vector(806.8383197747265,276.8631463159621, 0.0));
    positionAlloc4->Add (Vector(237.00805643988133,661.607232642551, 0.0));
    positionAlloc4->Add (Vector(731.4335854514104,825.5188374665588, 0.0));
    positionAlloc4->Add (Vector(394.2077865800967,800.6704009734781, 0.0));
    positionAlloc4->Add (Vector(480.10333198511086,438.288639903088, 0.0));
    positionAlloc4->Add (Vector(682.1595019227806,566.947118425491, 0.0));
    positionAlloc4->Add (Vector(328.24845833463473,627.0519055567505, 0.0));
    positionAlloc4->Add (Vector(375.6476914823061,716.9121120982444, 0.0));
    positionAlloc4->Add (Vector(194.36247933093742,842.9721626729616, 0.0));
    positionAlloc4->Add (Vector(385.62299473574444,244.09027385728197, 0.0));
    positionAlloc4->Add (Vector(522.8126027861101,186.27647577569883, 0.0));
    positionAlloc4->Add (Vector(258.05390069194164,486.79807129837224, 0.0));
    positionAlloc4->Add (Vector(352.04138726817905,556.8667549958286, 0.0));
    positionAlloc4->Add (Vector(230.3857773174425,577.4340297091428, 0.0));
    positionAlloc4->Add (Vector(505.3272650791185,319.68681269055617, 0.0));
    positionAlloc4->Add (Vector(376.57546548297057,447.80550703149174, 0.0));
    positionAlloc4->Add (Vector(688.6450532343558,280.9306239902663, 0.0));
    positionAlloc4->Add (Vector(550.1288826289299,345.9853655621989, 0.0));
    positionAlloc4->Add (Vector(800.5631923893101,539.7340251097509, 0.0));
    positionAlloc4->Add (Vector(269.6213111730461,618.0502539471017, 0.0));
    positionAlloc4->Add (Vector(721.950449664496,330.53197893145784, 0.0));
    positionAlloc4->Add (Vector(828.6293032927582,755.8549881257862, 0.0));
    positionAlloc4->Add (Vector(228.42723151763664,428.49891933201917, 0.0));
    positionAlloc4->Add (Vector(437.2777788927631,837.6650318360245, 0.0));
    positionAlloc4->Add (Vector(206.16362196314955,517.1579726991783, 0.0));
    positionAlloc4->Add (Vector(271.36151670632836,777.7357972384976, 0.0));
    positionAlloc4->Add (Vector(280.59691683591404,349.566618085693, 0.0));
    positionAlloc4->Add (Vector(322.1704098691557,262.476961281689, 0.0));
    positionAlloc4->Add (Vector(221.2477308342826,257.0013974290324, 0.0));
    positionAlloc4->Add (Vector(574.0423735498209,316.05875421519613, 0.0));
    positionAlloc4->Add (Vector(399.26770812849236,840.5606741550857, 0.0));
    positionAlloc4->Add (Vector(602.1420689321209,549.5448132682384, 0.0));
    positionAlloc4->Add (Vector(348.6431340591071,404.1576074790446, 0.0));
    positionAlloc4->Add (Vector(452.16168236077556,239.30621195689463, 0.0));
    positionAlloc4->Add (Vector(560.7887826146155,671.807631929031, 0.0));
    positionAlloc4->Add (Vector(210.8442016611966,525.9745792729834, 0.0));
    positionAlloc4->Add (Vector(702.4198670251368,498.9095392503401, 0.0));
    positionAlloc4->Add (Vector(565.5147174878341,625.3716234011426, 0.0));
    positionAlloc4->Add (Vector(233.83349227347585,773.3690965179264, 0.0));
    positionAlloc4->Add (Vector(298.0136048975812,379.2481465245403, 0.0));
    positionAlloc4->Add (Vector(164.04983844440846,375.45541590822484, 0.0));
    positionAlloc4->Add (Vector(338.4702616645667,414.09891417323587, 0.0));
    positionAlloc4->Add (Vector(522.8320865185162,319.72935254431076, 0.0));
    positionAlloc4->Add (Vector(225.02040793784323,463.18865677641827, 0.0));
    positionAlloc4->Add (Vector(508.1742094809663,522.479439685721, 0.0));
    positionAlloc4->Add (Vector(649.3447101191866,776.6447863810396, 0.0));
    positionAlloc4->Add (Vector(175.67824202341853,326.7058610408145, 0.0));
    positionAlloc4->Add (Vector(473.7743012932035,673.2296566936295, 0.0));
    positionAlloc4->Add (Vector(582.7783400712071,408.2395476988407, 0.0));
    positionAlloc4->Add (Vector(409.3304719539674,489.16608963371493, 0.0));
    positionAlloc4->Add (Vector(279.1647135494641,296.54452633569724, 0.0));
    positionAlloc4->Add (Vector(714.3408958551429,673.7817832685608, 0.0));
    positionAlloc4->Add (Vector(192.75457189840463,343.81435370304257, 0.0));
    positionAlloc4->Add (Vector(761.5050368747441,794.6106803722693, 0.0));
    positionAlloc4->Add (Vector(589.5302726436355,207.23132236421034, 0.0));
    positionAlloc4->Add (Vector(177.51849788876925,709.5872792363299, 0.0));
    positionAlloc4->Add (Vector(502.3920895983998,633.2763800405558, 0.0));
    positionAlloc4->Add (Vector(294.4336028840547,404.9889027336528, 0.0));
    positionAlloc4->Add (Vector(234.44510977042057,472.54513762290804, 0.0));
    positionAlloc4->Add (Vector(273.90733899372276,212.09177362993893, 0.0));
    positionAlloc4->Add (Vector(268.2545441505152,345.013237322371, 0.0));
    positionAlloc4->Add (Vector(284.6023131660695,178.6080313865352, 0.0));
    positionAlloc4->Add (Vector(617.290416964719,270.62390984114086, 0.0));
    positionAlloc4->Add (Vector(777.7518869342083,481.77314818401067, 0.0));
    positionAlloc4->Add (Vector(277.3160590794014,391.9670983267042, 0.0));
    positionAlloc4->Add (Vector(247.1790208545871,598.6123213622502, 0.0));
    positionAlloc4->Add (Vector(239.6320872704407,256.33188545860605, 0.0));
    positionAlloc4->Add (Vector(203.10432967606474,240.21580533745447, 0.0));
    positionAlloc4->Add (Vector(593.380598021708,318.10193314473514, 0.0));
    positionAlloc4->Add (Vector(494.8326657950087,380.1261272012557, 0.0));
    positionAlloc4->Add (Vector(471.3950293667829,836.022802699834, 0.0));
    positionAlloc4->Add (Vector(494.4814051556767,330.77107180927084, 0.0));
    positionAlloc4->Add (Vector(252.95143665032657,798.6113376130461, 0.0));
    positionAlloc4->Add (Vector(753.6545961227216,814.6644365603495, 0.0));
    positionAlloc4->Add (Vector(427.5799476248891,585.4606901595114, 0.0));
    positionAlloc4->Add (Vector(509.7926424118676,698.5571089188002, 0.0));
    positionAlloc4->Add (Vector(782.9110861456239,393.7788269163102, 0.0));
    positionAlloc4->Add (Vector(424.57083156981264,456.23244541661035, 0.0));
    positionAlloc4->Add (Vector(395.41129116461786,562.9423807982432, 0.0));
    positionAlloc4->Add (Vector(245.46003291073885,160.18126596751736, 0.0));
    positionAlloc4->Add (Vector(642.8558376694565,299.0749130562375, 0.0));
    positionAlloc4->Add (Vector(678.5960789715333,584.2166859474253, 0.0));
    positionAlloc4->Add (Vector(243.81065991544853,170.1478734193373, 0.0));
    positionAlloc4->Add (Vector(763.6575218608415,432.4380063486779, 0.0));
    positionAlloc4->Add (Vector(536.3347871421399,542.6565800747703, 0.0));
    positionAlloc4->Add (Vector(514.1144507783954,746.5270502229781, 0.0));
    positionAlloc4->Add (Vector(514.2153906326553,690.6228742149475, 0.0));
    positionAlloc4->Add (Vector(229.49031453974033,620.1012369149597, 0.0));
    positionAlloc4->Add (Vector(582.689768320527,400.71390262298837, 0.0));
    positionAlloc4->Add (Vector(310.0499638524239,620.1904790840997, 0.0));
    positionAlloc4->Add (Vector(817.8357190811057,574.3222441156161, 0.0));
    positionAlloc4->Add (Vector(465.3808947170655,240.4115906060465, 0.0));
    positionAlloc4->Add (Vector(744.753173474188,323.1436712202384, 0.0));
    positionAlloc4->Add (Vector(688.2911906263573,811.3360109176996, 0.0));
    positionAlloc4->Add (Vector(847.6457859517337,538.8021939218393, 0.0));
    positionAlloc4->Add (Vector(719.0307063910543,849.8023713002914, 0.0));
    positionAlloc4->Add (Vector(189.46284765564897,755.3056633785372, 0.0));
    positionAlloc4->Add (Vector(440.37365163636565,341.38495035149435, 0.0));
    positionAlloc4->Add (Vector(736.8300932977965,478.0852118564792, 0.0));
    positionAlloc4->Add (Vector(828.9400784702223,739.4832461949961, 0.0));
    positionAlloc4->Add (Vector(565.8915007464946,416.62359964171503, 0.0));
    positionAlloc4->Add (Vector(841.0932905204611,255.29182377246974, 0.0));
    positionAlloc4->Add (Vector(413.5243568146347,717.387331547283, 0.0));
    positionAlloc4->Add (Vector(181.6530690743326,168.4568821480764, 0.0));
    positionAlloc4->Add (Vector(802.9099383478093,756.2447226502859, 0.0));
    positionAlloc4->Add (Vector(579.3906371550529,442.7701254034332, 0.0));
    positionAlloc4->Add (Vector(814.8599393043469,658.0036558718521, 0.0));
    positionAlloc4->Add (Vector(787.6490291456897,725.7760700462183, 0.0));
    positionAlloc4->Add (Vector(629.3118207877155,263.8318575414239, 0.0));
    positionAlloc4->Add (Vector(366.6693314004294,719.656513794864, 0.0));
    positionAlloc4->Add (Vector(638.7131998138216,525.2674564408842, 0.0));
    positionAlloc4->Add (Vector(832.0763653663165,840.347521135722, 0.0));
    positionAlloc4->Add (Vector(558.5592782956105,204.41424587548937, 0.0));
    positionAlloc4->Add (Vector(474.5888834579875,397.34655961823694, 0.0));
    positionAlloc4->Add (Vector(344.16682808679286,281.6848118066329, 0.0));
    positionAlloc4->Add (Vector(468.95972694999904,584.0928184610757, 0.0));
    positionAlloc4->Add (Vector(815.2880388535912,773.1846487236414, 0.0));
    positionAlloc4->Add (Vector(181.81246623902052,346.6742327145612, 0.0));
    positionAlloc4->Add (Vector(559.0200183461702,780.8106073436923, 0.0));
    positionAlloc4->Add (Vector(468.1787453374413,589.2906560588444, 0.0));
    positionAlloc4->Add (Vector(590.5790466946837,637.024082289232, 0.0));
    positionAlloc4->Add (Vector(828.5117749333692,516.1684909191042, 0.0));
    positionAlloc4->Add (Vector(201.6575953147792,537.6979990809245, 0.0));
    positionAlloc4->Add (Vector(519.39077636041,845.0753572835102, 0.0));
    positionAlloc4->Add (Vector(180.21193836540328,773.8001795886497, 0.0));
    positionAlloc4->Add (Vector(158.50813228287143,828.9151786953473, 0.0));
    positionAlloc4->Add (Vector(306.51708696361686,824.2557776084279, 0.0));
    positionAlloc4->Add (Vector(349.99846039730255,758.0193897326222, 0.0));
    positionAlloc4->Add (Vector(823.5207839321794,412.4094056665928, 0.0));
    positionAlloc4->Add (Vector(672.2314154857963,558.3581355680117, 0.0));
    positionAlloc4->Add (Vector(232.26837274397343,604.4472114812445, 0.0));
    positionAlloc4->Add (Vector(189.41229297728614,233.17254138765034, 0.0));
    positionAlloc4->Add (Vector(157.94755133719335,478.0624493958884, 0.0));
    positionAlloc4->Add (Vector(227.83823761430594,494.8375730036014, 0.0));
    positionAlloc4->Add (Vector(446.9554064728834,784.4480695663152, 0.0));
    positionAlloc4->Add (Vector(694.0028411492058,540.8829748150636, 0.0));
    positionAlloc4->Add (Vector(413.381709871641,208.45050168906812, 0.0));
    positionAlloc4->Add (Vector(840.2007446760097,677.3647296812592, 0.0));
    positionAlloc4->Add (Vector(718.9596970802518,813.0740041687011, 0.0));
    positionAlloc4->Add (Vector(411.98955702928424,409.44950294682366, 0.0));
    positionAlloc4->Add (Vector(209.38639985963434,840.6477049508228, 0.0));
    positionAlloc4->Add (Vector(644.3695590095474,206.94414644932982, 0.0));
    positionAlloc4->Add (Vector(200.93410445493546,725.3020415032494, 0.0));
    positionAlloc4->Add (Vector(208.8747642804982,640.6783920213841, 0.0));
    positionAlloc4->Add (Vector(582.7526443213827,220.7858732859532, 0.0));
    positionAlloc4->Add (Vector(250.0941774369851,683.0574422022306, 0.0));
    positionAlloc4->Add (Vector(395.6405087864551,231.9469114993241, 0.0));
    positionAlloc4->Add (Vector(459.0713509613639,771.3929279308098, 0.0));
    positionAlloc4->Add (Vector(190.291132011651,534.6702176266149, 0.0));
    positionAlloc4->Add (Vector(213.84427213408327,496.094213291807, 0.0));
    positionAlloc4->Add (Vector(704.1053306080939,702.7326999561877, 0.0));
    positionAlloc4->Add (Vector(620.7629192539268,676.662170606376, 0.0));
    positionAlloc4->Add (Vector(355.2475401886452,380.0651817589412, 0.0));
    positionAlloc4->Add (Vector(592.2860485188595,463.91186538482384, 0.0));
    positionAlloc4->Add (Vector(815.4250028562893,551.4065216863003, 0.0));
    positionAlloc4->Add (Vector(373.51954631329045,815.0433769355634, 0.0));
    positionAlloc4->Add (Vector(783.7673834934845,213.9006737502935, 0.0));
    positionAlloc4->Add (Vector(422.4411751139142,157.58635603620885, 0.0));
    positionAlloc4->Add (Vector(374.03472072142824,776.8662599473404, 0.0));
    positionAlloc4->Add (Vector(503.6766607135,728.5202262754192, 0.0));
    positionAlloc4->Add (Vector(222.18670818515284,781.7870346756966, 0.0));
    positionAlloc4->Add (Vector(675.6097474540481,678.1800118592777, 0.0));
    positionAlloc4->Add (Vector(750.8889262901083,450.2958191625128, 0.0));
    positionAlloc4->Add (Vector(388.5624781771125,801.5301279224954, 0.0));
    positionAlloc4->Add (Vector(410.4126600549482,693.4890725193978, 0.0));
    positionAlloc4->Add (Vector(843.3535994004714,438.83237383799855, 0.0));
    positionAlloc4->Add (Vector(675.4303261486008,714.7843174870848, 0.0));
    positionAlloc4->Add (Vector(347.4242007999145,274.20768064580597, 0.0));
    positionAlloc4->Add (Vector(664.5497306627201,712.436651269394, 0.0));
    positionAlloc4->Add (Vector(590.2599927459188,764.2304094689371, 0.0));
    positionAlloc4->Add (Vector(232.71537933515938,637.7160157549055, 0.0));
    positionAlloc4->Add (Vector(720.127937812255,347.29834234137996, 0.0));
    positionAlloc4->Add (Vector(741.4127176786182,166.2903550150781, 0.0));
    positionAlloc4->Add (Vector(199.8320539221603,373.28294120563294, 0.0));
    positionAlloc4->Add (Vector(216.11007252914987,628.1047413914498, 0.0));
    positionAlloc4->Add (Vector(220.54460096419865,162.7552779560848, 0.0));
    positionAlloc4->Add (Vector(287.0537943525075,198.55291061261582, 0.0));
    positionAlloc4->Add (Vector(822.83339467674,783.7454493692446, 0.0));
    positionAlloc4->Add (Vector(809.0253661435676,420.27184646054195, 0.0));
    positionAlloc4->Add (Vector(470.77702334406166,531.9317525211545, 0.0));
    positionAlloc4->Add (Vector(421.7189483445653,600.3017529096472, 0.0));
    positionAlloc4->Add (Vector(808.1611689974703,552.8319245131153, 0.0));
    positionAlloc4->Add (Vector(168.55791737913526,560.0429068914243, 0.0));
    positionAlloc4->Add (Vector(199.3981231803009,599.693494744421, 0.0));
    positionAlloc4->Add (Vector(611.0855421332615,264.05409895700075, 0.0));
    positionAlloc4->Add (Vector(502.1953810060614,749.5428888318256, 0.0));
    positionAlloc4->Add (Vector(412.22883023852984,588.1019409999654, 0.0));
    positionAlloc4->Add (Vector(594.5655556073507,181.71280684043117, 0.0));
    positionAlloc4->Add (Vector(428.9533140781614,581.0950686365516, 0.0));
    positionAlloc4->Add (Vector(271.2413089370106,453.6961544665811, 0.0));
    positionAlloc4->Add (Vector(494.13111258178265,481.43023954639597, 0.0));
    positionAlloc4->Add (Vector(481.92168036112764,218.48391245570105, 0.0));
    positionAlloc4->Add (Vector(748.8224088077051,642.5605015660166, 0.0));
    positionAlloc4->Add (Vector(182.20184941522692,178.5101616232791, 0.0));
    positionAlloc4->Add (Vector(160.0754420408291,231.25084835484137, 0.0));
    positionAlloc4->Add (Vector(399.1808750558831,680.4922773250584, 0.0));
    positionAlloc4->Add (Vector(443.72004369945296,323.4116926508102, 0.0));
    positionAlloc4->Add (Vector(538.0478367579805,578.2045223640466, 0.0));
    positionAlloc4->Add (Vector(818.4056964642359,666.527841687038, 0.0));
    positionAlloc4->Add (Vector(612.1381637024119,345.95372786216, 0.0));
    positionAlloc4->Add (Vector(534.4586652942844,650.2171458900436, 0.0));
    positionAlloc4->Add (Vector(259.5059298697602,325.17002871521674, 0.0));
    positionAlloc4->Add (Vector(436.26706108998894,271.30602404959205, 0.0));
    positionAlloc4->Add (Vector(735.1860843585441,637.1819442655885, 0.0));
    positionAlloc4->Add (Vector(167.74552039082025,823.8538902745476, 0.0));
    positionAlloc4->Add (Vector(457.27999449394525,204.9194669395862, 0.0));
    positionAlloc4->Add (Vector(706.6303363380925,339.58257588345197, 0.0));
    positionAlloc4->Add (Vector(511.41024381083673,376.06953105887214, 0.0));
    positionAlloc4->Add (Vector(658.263933605863,833.0964556237742, 0.0));
    positionAlloc4->Add (Vector(528.4445851270746,596.2009310487446, 0.0));
    positionAlloc4->Add (Vector(186.1772048180254,521.9482420977035, 0.0));
    positionAlloc4->Add (Vector(586.0233330733001,209.7432254956376, 0.0));
    positionAlloc4->Add (Vector(688.9954871690276,301.0747192477902, 0.0));
    positionAlloc4->Add (Vector(238.94235885631934,515.5702820383631, 0.0));
    positionAlloc4->Add (Vector(725.8203924617608,402.13344898788404, 0.0));
    positionAlloc4->Add (Vector(171.35017495733462,176.14373212445008, 0.0));
    positionAlloc4->Add (Vector(350.5788764897991,563.5832823983076, 0.0));
    positionAlloc4->Add (Vector(475.9186126927221,529.8512442953036, 0.0));
    positionAlloc4->Add (Vector(554.7960986973218,175.15959165771946, 0.0));
    positionAlloc4->Add (Vector(412.9080668479608,215.7873578886083, 0.0));
    positionAlloc4->Add (Vector(773.0037392722965,386.5966097960751, 0.0));
    positionAlloc4->Add (Vector(641.3768140803924,707.0548686052707, 0.0));
    positionAlloc4->Add (Vector(708.8066252767426,604.9747515444355, 0.0));
    positionAlloc4->Add (Vector(507.93967920265646,501.0614062810397, 0.0));
    positionAlloc4->Add (Vector(756.9506230060725,789.2683867895299, 0.0));
    positionAlloc4->Add (Vector(716.5528088349595,717.0793762754265, 0.0));
    positionAlloc4->Add (Vector(401.6438058538286,355.51429098514535, 0.0));
    positionAlloc4->Add (Vector(637.9110186966875,641.7388587909766, 0.0));
    positionAlloc4->Add (Vector(764.1611503495689,668.5380324279431, 0.0));
    positionAlloc4->Add (Vector(248.05881066556682,512.8307566546157, 0.0));
    positionAlloc4->Add (Vector(580.5050586894188,843.0376950729843, 0.0));
    positionAlloc4->Add (Vector(549.0428190625555,218.02354563953799, 0.0));
    positionAlloc4->Add (Vector(805.3083419125467,637.2208576724811, 0.0));
    positionAlloc4->Add (Vector(268.64492268026476,539.7608837208452, 0.0));
    positionAlloc4->Add (Vector(745.7956700617998,371.84540360939434, 0.0));
    positionAlloc4->Add (Vector(356.11422444871,419.5684100213477, 0.0));
    positionAlloc4->Add (Vector(824.533983962477,747.106618827152, 0.0));
    positionAlloc4->Add (Vector(449.72890382212,826.6583733305687, 0.0));
    positionAlloc4->Add (Vector(160.8196315702072,799.8229938114077, 0.0));
    positionAlloc4->Add (Vector(790.405073154314,409.1110901788111, 0.0));
    positionAlloc4->Add (Vector(808.32100904704,817.7500039018112, 0.0));
    positionAlloc4->Add (Vector(601.8306071350175,273.97747558493427, 0.0));
    positionAlloc4->Add (Vector(346.54065370859905,167.02117650201768, 0.0));
    positionAlloc4->Add (Vector(286.6700914586312,655.7164806830538, 0.0));
    positionAlloc4->Add (Vector(553.8327192384513,494.76238567320473, 0.0));
    positionAlloc4->Add (Vector(706.3679124791539,501.84596517363445, 0.0));
    positionAlloc4->Add (Vector(774.4325886239793,591.797038198084, 0.0));
    positionAlloc4->Add (Vector(831.1073883267119,425.1684072667323, 0.0));
    positionAlloc4->Add (Vector(335.6416573772078,320.79275036535853, 0.0));
    positionAlloc4->Add (Vector(215.57233747966472,407.40106214160346, 0.0));
    positionAlloc4->Add (Vector(610.3290246102404,547.8160223348301, 0.0));
    positionAlloc4->Add (Vector(604.7430293330503,744.4563873459246, 0.0));
    positionAlloc4->Add (Vector(377.7797887114874,672.5439835826169, 0.0));
    positionAlloc4->Add (Vector(648.5254549432751,316.07436124776007, 0.0));
    positionAlloc4->Add (Vector(606.3728816518203,306.98851662239184, 0.0));
    positionAlloc4->Add (Vector(534.1136525566103,634.3266383848852, 0.0));
    positionAlloc4->Add (Vector(153.5431086923531,262.56563599224904, 0.0));
    positionAlloc4->Add (Vector(221.03008000622248,614.451238375639, 0.0));
    positionAlloc4->Add (Vector(574.5003417617129,156.43793613164075, 0.0));
    positionAlloc4->Add (Vector(263.14009986622966,778.9879319689554, 0.0));
    positionAlloc4->Add (Vector(599.4221523080014,208.89797549653417, 0.0));
    positionAlloc4->Add (Vector(770.9604969855822,695.9128821003367, 0.0));
    positionAlloc4->Add (Vector(658.1689752091676,777.977181966804, 0.0));
    positionAlloc4->Add (Vector(387.32085373409046,394.4467022288626, 0.0));
    positionAlloc4->Add (Vector(780.2926400143314,593.1710200912876, 0.0));
    positionAlloc4->Add (Vector(215.17193746412943,778.0510305673288, 0.0));
    positionAlloc4->Add (Vector(520.7554048492045,319.2966036303162, 0.0));
    positionAlloc4->Add (Vector(722.0555401408511,538.6405681196236, 0.0));
    positionAlloc4->Add (Vector(330.55913940060896,611.9888322239253, 0.0));
    positionAlloc4->Add (Vector(797.285532794994,764.1375473666867, 0.0));
    positionAlloc4->Add (Vector(388.74644573518094,229.43146486841235, 0.0));
    positionAlloc4->Add (Vector(805.7109921157141,246.26466090219526, 0.0));
    positionAlloc4->Add (Vector(633.6564166717262,420.7147424103762, 0.0));
    positionAlloc4->Add (Vector(601.6209532866148,272.056500303494, 0.0));
    positionAlloc4->Add (Vector(508.4651408094967,308.5470426385566, 0.0));
    positionAlloc4->Add (Vector(624.2950532895977,161.6114802494993, 0.0));
    positionAlloc4->Add (Vector(178.54259908833473,563.6250602317693, 0.0));
    positionAlloc4->Add (Vector(374.54604548021507,280.562957279898, 0.0));
    positionAlloc4->Add (Vector(213.2028390380858,734.7117469124665, 0.0));
    positionAlloc4->Add (Vector(593.4707975326263,525.0422788523309, 0.0));
    positionAlloc4->Add (Vector(407.44819290347726,592.6140814155056, 0.0));
    positionAlloc4->Add (Vector(316.3462807946797,659.7514440283016, 0.0));
    positionAlloc4->Add (Vector(620.494883184115,683.1337307301023, 0.0));
    positionAlloc4->Add (Vector(839.9553178774205,319.43869005805027, 0.0));
    positionAlloc4->Add (Vector(251.42641046385617,492.6169321942941, 0.0));
    positionAlloc4->Add (Vector(785.7861201766576,317.69332346688066, 0.0));
    positionAlloc4->Add (Vector(186.03512587499256,345.052524965628, 0.0));
    positionAlloc4->Add (Vector(576.6950337859278,501.875316260203, 0.0));
    positionAlloc4->Add (Vector(349.38834606422733,175.82086314817295, 0.0));
    positionAlloc4->Add (Vector(498.0739541246698,360.61481687173875, 0.0));
    positionAlloc4->Add (Vector(823.7131064594779,326.2476070777549, 0.0));
    positionAlloc4->Add (Vector(404.54072166550577,830.2474579046725, 0.0));
    positionAlloc4->Add (Vector(513.1534352203562,642.1132712266244, 0.0));
    positionAlloc4->Add (Vector(810.0367927387634,376.24205241452864, 0.0));
    positionAlloc4->Add (Vector(233.90575713357796,386.3306199825396, 0.0));
    positionAlloc4->Add (Vector(442.1877022041453,305.47546732951116, 0.0));
    positionAlloc4->Add (Vector(154.8664913718335,507.52311180429604, 0.0));
    positionAlloc4->Add (Vector(722.6103361457452,752.5114082794404, 0.0));
    positionAlloc4->Add (Vector(309.5546137793592,448.97545203837944, 0.0));
    positionAlloc4->Add (Vector(372.60243248030474,227.03634716937373, 0.0));
    positionAlloc4->Add (Vector(715.2081086148438,777.2639099464452, 0.0));
    positionAlloc4->Add (Vector(774.7912989429844,527.5395693409555, 0.0));
    positionAlloc4->Add (Vector(712.5704538293801,280.5990412202251, 0.0));
    positionAlloc4->Add (Vector(593.3826295572965,760.0224131314023, 0.0));
    positionAlloc4->Add (Vector(800.7883566398011,715.6842656950919, 0.0));
    positionAlloc4->Add (Vector(352.8260170396376,262.8549010778031, 0.0));
    positionAlloc4->Add (Vector(310.1587158441357,203.8859368801551, 0.0));
    positionAlloc4->Add (Vector(437.26804612494084,678.885796980134, 0.0));
    positionAlloc4->Add (Vector(785.2965317482651,324.5045604042125, 0.0));
    positionAlloc4->Add (Vector(370.04918675342867,505.99948381529197, 0.0));
    positionAlloc4->Add (Vector(172.000429980714,595.4872878846463, 0.0));
    positionAlloc4->Add (Vector(167.79338872086663,225.52399889531313, 0.0));
    positionAlloc4->Add (Vector(515.9129805673958,449.27871285098473, 0.0));
    positionAlloc4->Add (Vector(689.6770259681927,495.65691745507354, 0.0));
    positionAlloc4->Add (Vector(682.5495340318282,542.8940382986474, 0.0));
    positionAlloc4->Add (Vector(233.71597215681118,649.2713510560965, 0.0));
    positionAlloc4->Add (Vector(771.0489198034286,480.5504476133645, 0.0));
    positionAlloc4->Add (Vector(660.7243248366449,596.2902299486492, 0.0));
    positionAlloc4->Add (Vector(367.68762520096357,377.62832541872297, 0.0));
    positionAlloc4->Add (Vector(381.62861739685445,194.49084520021654, 0.0));
    positionAlloc4->Add (Vector(754.1723981129155,586.3086887792906, 0.0));
    positionAlloc4->Add (Vector(400.92600998099084,231.1083416675908, 0.0));
    positionAlloc4->Add (Vector(689.889242680162,201.83125621386324, 0.0));
    positionAlloc4->Add (Vector(644.800140693332,660.3050176286911, 0.0));
    positionAlloc4->Add (Vector(153.86548198652167,720.8229999183839, 0.0));
    positionAlloc4->Add (Vector(690.5713385076602,289.1009770739207, 0.0));
    positionAlloc4->Add (Vector(202.18545057583958,840.8208556203621, 0.0));
    positionAlloc4->Add (Vector(248.64695748233385,711.5378865278278, 0.0));
    positionAlloc4->Add (Vector(346.65415678116653,529.887258210774, 0.0));
    positionAlloc4->Add (Vector(730.1162564063505,399.7273286855125, 0.0));
    positionAlloc4->Add (Vector(422.0741027826374,339.94432224172715, 0.0));
    positionAlloc4->Add (Vector(181.65910223737666,377.73123153428503, 0.0));
    positionAlloc4->Add (Vector(211.94475143634367,287.18800369340164, 0.0));
    positionAlloc4->Add (Vector(568.5299851677596,795.3119645161818, 0.0));
    positionAlloc4->Add (Vector(807.6492590949324,776.3791452993541, 0.0));
    positionAlloc4->Add (Vector(828.709239435191,692.5929763527802, 0.0));
    positionAlloc4->Add (Vector(532.6971955402958,279.3981188678689, 0.0));
    positionAlloc4->Add (Vector(368.19775326258764,514.0476148244676, 0.0));
    positionAlloc4->Add (Vector(331.14598712001185,613.7655990477874, 0.0));
    positionAlloc4->Add (Vector(174.07196478065288,786.5242814551474, 0.0));
    positionAlloc4->Add (Vector(235.42676439134516,496.6238370778891, 0.0));
    positionAlloc4->Add (Vector(628.9631185585099,458.1067456177209, 0.0));
    positionAlloc4->Add (Vector(363.2296384213595,218.37047980446872, 0.0));
    positionAlloc4->Add (Vector(825.9424231521915,715.8781436815228, 0.0));
    positionAlloc4->Add (Vector(195.53611508969567,814.2198760773333, 0.0));
    positionAlloc4->Add (Vector(575.2813963310068,269.36688658110404, 0.0));
    positionAlloc4->Add (Vector(564.6901982034296,182.5152889039984, 0.0));
    positionAlloc4->Add (Vector(289.77164751085184,509.9641068895281, 0.0));
    positionAlloc4->Add (Vector(469.2489889519251,699.6231729751095, 0.0));
    positionAlloc4->Add (Vector(354.50125397465274,406.4532903055842, 0.0));
    positionAlloc4->Add (Vector(578.2970263056657,247.6457024564293, 0.0));
    positionAlloc4->Add (Vector(452.36151304948106,353.8603981386293, 0.0));
    positionAlloc4->Add (Vector(362.9695700716764,517.3295021425665, 0.0));
    positionAlloc4->Add (Vector(277.27747704497045,278.38315689740364, 0.0));
    positionAlloc4->Add (Vector(732.7098485602952,298.6373774747933, 0.0));
    positionAlloc4->Add (Vector(164.4091460070617,828.936896513396, 0.0));
    positionAlloc4->Add (Vector(570.7805082202462,645.6508044572319, 0.0));
    positionAlloc4->Add (Vector(190.65852851773963,756.3233020424547, 0.0));
    positionAlloc4->Add (Vector(259.2130483097055,259.1961642353418, 0.0));
    positionAlloc4->Add (Vector(662.3957592679835,569.0609389379256, 0.0));
    positionAlloc4->Add (Vector(412.1780831931537,815.5000144869413, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_1.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_1.flowmon", false, false);
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