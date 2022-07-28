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
	  uint16_t numberOfRrhs = 50;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 383;
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
	positionAllocMacro->Add (Vector (  250,  250, 0));
	positionAllocMacro->Add (Vector (  250,  250, 0));
	positionAllocMacro->Add (Vector (  250,  250, 0));
	positionAllocMacro->Add (Vector (  250,  250, 0));

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
    positionAlloc->Add (Vector (772.2222222222222,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,694.4444444444445, 0.0));
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
    positionAlloc->Add (Vector (616.6666666666667,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,150.0, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,150.0, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,850.0, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,150.0, 0.0));
    positionAlloc->Add (Vector (150.0,461.1111111111111, 0.0));
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
    positionAlloc4->Add (Vector(559.8946101553113,528.9620946352068, 0.0));
    positionAlloc4->Add (Vector(225.63119156348498,484.94374907503243, 0.0));
    positionAlloc4->Add (Vector(237.88485292893398,824.4359632320293, 0.0));
    positionAlloc4->Add (Vector(744.9715313522438,287.37200169431594, 0.0));
    positionAlloc4->Add (Vector(460.1235944363336,581.9741611000594, 0.0));
    positionAlloc4->Add (Vector(167.98275140858573,349.17896997385367, 0.0));
    positionAlloc4->Add (Vector(583.4209147560857,280.6479578583778, 0.0));
    positionAlloc4->Add (Vector(499.97129437535193,661.1744326706998, 0.0));
    positionAlloc4->Add (Vector(677.4304774617177,761.0015950959652, 0.0));
    positionAlloc4->Add (Vector(635.3954896394163,640.4280474761161, 0.0));
    positionAlloc4->Add (Vector(239.82810353213517,554.944755760531, 0.0));
    positionAlloc4->Add (Vector(243.24694689564006,592.6773795258559, 0.0));
    positionAlloc4->Add (Vector(627.5341824785692,370.6676042546155, 0.0));
    positionAlloc4->Add (Vector(431.89260469619217,620.7550838715545, 0.0));
    positionAlloc4->Add (Vector(378.8587170356727,364.0207702131038, 0.0));
    positionAlloc4->Add (Vector(557.900114541277,718.7316747309202, 0.0));
    positionAlloc4->Add (Vector(761.5329701160507,846.8982303434815, 0.0));
    positionAlloc4->Add (Vector(460.87248983463303,522.3102365535116, 0.0));
    positionAlloc4->Add (Vector(273.8278655767888,841.1353110056822, 0.0));
    positionAlloc4->Add (Vector(370.44291121222705,217.6513840014266, 0.0));
    positionAlloc4->Add (Vector(318.20938641525674,216.47569265986357, 0.0));
    positionAlloc4->Add (Vector(574.9946966185039,422.1084435135827, 0.0));
    positionAlloc4->Add (Vector(534.2109610798198,415.9853552757762, 0.0));
    positionAlloc4->Add (Vector(510.12292941792595,209.53946984246878, 0.0));
    positionAlloc4->Add (Vector(820.3421523908115,546.0214654293025, 0.0));
    positionAlloc4->Add (Vector(528.2095847330497,745.8055147981872, 0.0));
    positionAlloc4->Add (Vector(328.13693213278214,159.0282288526678, 0.0));
    positionAlloc4->Add (Vector(648.4959321164235,399.61732319034394, 0.0));
    positionAlloc4->Add (Vector(232.75303560376202,150.0215031917677, 0.0));
    positionAlloc4->Add (Vector(660.25744007361,695.8746188070477, 0.0));
    positionAlloc4->Add (Vector(222.40388040233836,431.4190518217685, 0.0));
    positionAlloc4->Add (Vector(792.8782794570906,604.8516327350097, 0.0));
    positionAlloc4->Add (Vector(216.09322836233667,732.0628331785565, 0.0));
    positionAlloc4->Add (Vector(389.911647799535,244.20338697287542, 0.0));
    positionAlloc4->Add (Vector(615.5391923046418,539.6048999515074, 0.0));
    positionAlloc4->Add (Vector(660.1446381154533,376.47128873984406, 0.0));
    positionAlloc4->Add (Vector(565.4229678933067,161.37041024661758, 0.0));
    positionAlloc4->Add (Vector(613.2842095797987,593.5981198358703, 0.0));
    positionAlloc4->Add (Vector(340.3681915787391,302.5239657387208, 0.0));
    positionAlloc4->Add (Vector(357.07686824757525,150.7784659373312, 0.0));
    positionAlloc4->Add (Vector(625.7432276766006,812.1472508224606, 0.0));
    positionAlloc4->Add (Vector(704.5338363611733,623.4527475109069, 0.0));
    positionAlloc4->Add (Vector(638.482949853619,780.1848778414493, 0.0));
    positionAlloc4->Add (Vector(355.3473950087069,517.8292151491253, 0.0));
    positionAlloc4->Add (Vector(250.29702109184876,650.3031834514252, 0.0));
    positionAlloc4->Add (Vector(792.0876466043765,434.6133761516306, 0.0));
    positionAlloc4->Add (Vector(570.6384039613603,388.6710000048908, 0.0));
    positionAlloc4->Add (Vector(198.57586013292837,212.91379621531246, 0.0));
    positionAlloc4->Add (Vector(544.7436967289366,500.19497464543224, 0.0));
    positionAlloc4->Add (Vector(773.2690245986863,553.8168409178459, 0.0));
    positionAlloc4->Add (Vector(825.7119065412799,185.29174152154644, 0.0));
    positionAlloc4->Add (Vector(357.1524206056313,614.6771323969076, 0.0));
    positionAlloc4->Add (Vector(533.7016419096154,272.2530301274307, 0.0));
    positionAlloc4->Add (Vector(591.9627282337757,448.4788497611358, 0.0));
    positionAlloc4->Add (Vector(331.0141137943488,169.49656857475208, 0.0));
    positionAlloc4->Add (Vector(159.57769033247678,207.11910400338687, 0.0));
    positionAlloc4->Add (Vector(382.3800533150918,475.73585375764026, 0.0));
    positionAlloc4->Add (Vector(484.96985894366577,800.201684891896, 0.0));
    positionAlloc4->Add (Vector(663.9058813291524,388.9230521650365, 0.0));
    positionAlloc4->Add (Vector(744.6124843580149,181.49777486479707, 0.0));
    positionAlloc4->Add (Vector(241.68162009592186,574.1978770897178, 0.0));
    positionAlloc4->Add (Vector(303.0997222345774,191.9603106029158, 0.0));
    positionAlloc4->Add (Vector(587.7311189681177,672.9547869903203, 0.0));
    positionAlloc4->Add (Vector(634.6480143080898,313.3329758545286, 0.0));
    positionAlloc4->Add (Vector(688.0069316392414,308.39070100223034, 0.0));
    positionAlloc4->Add (Vector(450.3436585118662,343.1458860663828, 0.0));
    positionAlloc4->Add (Vector(627.7727216764051,552.751421190369, 0.0));
    positionAlloc4->Add (Vector(565.969279116018,572.2050840860629, 0.0));
    positionAlloc4->Add (Vector(377.0305641460626,623.5386246644231, 0.0));
    positionAlloc4->Add (Vector(621.6872287890033,742.821057100267, 0.0));
    positionAlloc4->Add (Vector(238.16863746555077,835.4060885704309, 0.0));
    positionAlloc4->Add (Vector(755.8936682205424,221.9898145102337, 0.0));
    positionAlloc4->Add (Vector(685.7543547006285,327.1184386441772, 0.0));
    positionAlloc4->Add (Vector(355.84722352374445,297.22298940005555, 0.0));
    positionAlloc4->Add (Vector(772.9393634540038,844.3739230670503, 0.0));
    positionAlloc4->Add (Vector(722.8662973456746,178.09720135975655, 0.0));
    positionAlloc4->Add (Vector(695.6616909968444,161.34027813762705, 0.0));
    positionAlloc4->Add (Vector(726.2912488760211,539.3334848410082, 0.0));
    positionAlloc4->Add (Vector(804.103127127409,632.3613576237608, 0.0));
    positionAlloc4->Add (Vector(745.4021696105813,291.06986460441897, 0.0));
    positionAlloc4->Add (Vector(381.1992672514591,544.9622716791753, 0.0));
    positionAlloc4->Add (Vector(437.8333331086438,449.24051786318756, 0.0));
    positionAlloc4->Add (Vector(477.3768487394139,583.5729785565513, 0.0));
    positionAlloc4->Add (Vector(756.2710817644843,352.60766513364155, 0.0));
    positionAlloc4->Add (Vector(611.5960734573982,554.4649351757594, 0.0));
    positionAlloc4->Add (Vector(834.0959527965131,705.6726727356278, 0.0));
    positionAlloc4->Add (Vector(259.3410555514195,841.8946957744367, 0.0));
    positionAlloc4->Add (Vector(206.86074436467086,844.8072568414343, 0.0));
    positionAlloc4->Add (Vector(619.8895776603928,339.45075938723596, 0.0));
    positionAlloc4->Add (Vector(666.5656631783145,402.33215359908775, 0.0));
    positionAlloc4->Add (Vector(427.8945135452319,259.4218794962026, 0.0));
    positionAlloc4->Add (Vector(621.1644916378518,727.0453130745447, 0.0));
    positionAlloc4->Add (Vector(620.6686903041925,728.2452791837999, 0.0));
    positionAlloc4->Add (Vector(437.91278044636186,318.37210962610413, 0.0));
    positionAlloc4->Add (Vector(358.1010429969285,310.99562777426837, 0.0));
    positionAlloc4->Add (Vector(770.2602761694615,776.9829609866316, 0.0));
    positionAlloc4->Add (Vector(599.0531368793762,331.8796965582096, 0.0));
    positionAlloc4->Add (Vector(638.0145844965864,285.4047400695378, 0.0));
    positionAlloc4->Add (Vector(174.27089295824646,424.3373940607363, 0.0));
    positionAlloc4->Add (Vector(625.7880517833416,189.67630251608477, 0.0));
    positionAlloc4->Add (Vector(402.75230843872606,435.91890698593784, 0.0));
    positionAlloc4->Add (Vector(254.47462881903175,337.72205742143933, 0.0));
    positionAlloc4->Add (Vector(682.1126508531113,257.72993328564155, 0.0));
    positionAlloc4->Add (Vector(550.397114798515,713.8026305099904, 0.0));
    positionAlloc4->Add (Vector(520.9584108511631,262.9571508718682, 0.0));
    positionAlloc4->Add (Vector(384.0808252921241,426.20062072324663, 0.0));
    positionAlloc4->Add (Vector(494.7747665000025,459.474924275804, 0.0));
    positionAlloc4->Add (Vector(676.7278363105461,185.90569428685092, 0.0));
    positionAlloc4->Add (Vector(208.46744027601312,297.7881597906262, 0.0));
    positionAlloc4->Add (Vector(386.2179952749603,759.4819188202075, 0.0));
    positionAlloc4->Add (Vector(216.75264489110248,465.0410475983577, 0.0));
    positionAlloc4->Add (Vector(722.1137185492657,801.2788941185279, 0.0));
    positionAlloc4->Add (Vector(307.0229911974684,308.25534970739386, 0.0));
    positionAlloc4->Add (Vector(732.6544777626965,713.9541637543393, 0.0));
    positionAlloc4->Add (Vector(185.4011726769602,448.28486656656156, 0.0));
    positionAlloc4->Add (Vector(412.3053793551635,534.9079939433436, 0.0));
    positionAlloc4->Add (Vector(340.04440177989795,782.9703039852956, 0.0));
    positionAlloc4->Add (Vector(342.6674267259058,646.6072052437823, 0.0));
    positionAlloc4->Add (Vector(831.6602893500046,530.9390637525916, 0.0));
    positionAlloc4->Add (Vector(561.8465546882073,428.3790901031383, 0.0));
    positionAlloc4->Add (Vector(364.5029758923601,178.18060210646246, 0.0));
    positionAlloc4->Add (Vector(157.91177561231572,701.1603707044911, 0.0));
    positionAlloc4->Add (Vector(170.05669930435818,558.2766746187897, 0.0));
    positionAlloc4->Add (Vector(551.7649062483073,425.0265762252253, 0.0));
    positionAlloc4->Add (Vector(185.43573058835187,177.57428358497359, 0.0));
    positionAlloc4->Add (Vector(798.5246585693294,443.73916796476726, 0.0));
    positionAlloc4->Add (Vector(312.0326496357087,757.8866667628538, 0.0));
    positionAlloc4->Add (Vector(832.283417233986,244.82167575628358, 0.0));
    positionAlloc4->Add (Vector(166.59542755116286,734.4630439860827, 0.0));
    positionAlloc4->Add (Vector(264.11337967042033,313.6689831651449, 0.0));
    positionAlloc4->Add (Vector(613.2030778507697,571.7146093766114, 0.0));
    positionAlloc4->Add (Vector(220.17189460601364,161.61473644281853, 0.0));
    positionAlloc4->Add (Vector(208.88936358156798,425.17273529158075, 0.0));
    positionAlloc4->Add (Vector(680.0980847928505,778.575252793452, 0.0));
    positionAlloc4->Add (Vector(269.6554816945938,442.70135171395975, 0.0));
    positionAlloc4->Add (Vector(442.03539299520355,262.1579206835544, 0.0));
    positionAlloc4->Add (Vector(365.05149191488965,532.5209792096871, 0.0));
    positionAlloc4->Add (Vector(585.9273822678888,305.8732553861595, 0.0));
    positionAlloc4->Add (Vector(806.9708484311913,276.828295335058, 0.0));
    positionAlloc4->Add (Vector(631.6318899518901,234.2713051523623, 0.0));
    positionAlloc4->Add (Vector(572.4701645507846,611.654187155929, 0.0));
    positionAlloc4->Add (Vector(843.2326126555538,168.0387191350458, 0.0));
    positionAlloc4->Add (Vector(698.3085880692333,336.1499154424831, 0.0));
    positionAlloc4->Add (Vector(812.7293712845685,430.1485930947704, 0.0));
    positionAlloc4->Add (Vector(619.7652099601833,713.8010301321854, 0.0));
    positionAlloc4->Add (Vector(659.8595980377522,392.0642688872975, 0.0));
    positionAlloc4->Add (Vector(826.0230468738232,426.60870823061356, 0.0));
    positionAlloc4->Add (Vector(154.46533038461348,834.8483509397996, 0.0));
    positionAlloc4->Add (Vector(177.4885899691566,340.39902374104923, 0.0));
    positionAlloc4->Add (Vector(551.1295404333109,377.6478373933479, 0.0));
    positionAlloc4->Add (Vector(509.8668097858818,365.9277970426388, 0.0));
    positionAlloc4->Add (Vector(740.9486277003002,488.81440075658287, 0.0));
    positionAlloc4->Add (Vector(790.5425127620557,325.8184872838809, 0.0));
    positionAlloc4->Add (Vector(451.91119443871025,415.3283397681245, 0.0));
    positionAlloc4->Add (Vector(528.2075722532151,741.266136363157, 0.0));
    positionAlloc4->Add (Vector(252.5053796219713,733.0508009019596, 0.0));
    positionAlloc4->Add (Vector(662.0475248301973,727.1300623231843, 0.0));
    positionAlloc4->Add (Vector(624.2066498598061,449.9205326941568, 0.0));
    positionAlloc4->Add (Vector(676.6785176126515,521.9058128456523, 0.0));
    positionAlloc4->Add (Vector(590.8744581042954,432.24744322867593, 0.0));
    positionAlloc4->Add (Vector(791.4943811724463,594.7900498033368, 0.0));
    positionAlloc4->Add (Vector(770.3272404905416,804.0128799172431, 0.0));
    positionAlloc4->Add (Vector(820.6826062436037,304.3122029717932, 0.0));
    positionAlloc4->Add (Vector(501.6991010164635,662.0404517921811, 0.0));
    positionAlloc4->Add (Vector(539.9283592925246,311.9664454637936, 0.0));
    positionAlloc4->Add (Vector(267.74255903656274,765.5311509749318, 0.0));
    positionAlloc4->Add (Vector(179.34065170138027,443.42801313148493, 0.0));
    positionAlloc4->Add (Vector(259.768893721764,729.1115048160722, 0.0));
    positionAlloc4->Add (Vector(426.68152080181045,277.2452070521533, 0.0));
    positionAlloc4->Add (Vector(610.7118770083014,199.07569774887986, 0.0));
    positionAlloc4->Add (Vector(439.22076204540366,623.0760475728515, 0.0));
    positionAlloc4->Add (Vector(224.11202274919052,398.42837844058784, 0.0));
    positionAlloc4->Add (Vector(411.42056159289365,328.7354616342709, 0.0));
    positionAlloc4->Add (Vector(720.2697032969925,578.281714072255, 0.0));
    positionAlloc4->Add (Vector(432.2454292932766,733.5508073326206, 0.0));
    positionAlloc4->Add (Vector(398.266343489558,535.4643656085962, 0.0));
    positionAlloc4->Add (Vector(613.2444127301469,203.76213382009507, 0.0));
    positionAlloc4->Add (Vector(578.9570848755573,806.9375107499083, 0.0));
    positionAlloc4->Add (Vector(442.3668970718963,231.4509396151471, 0.0));
    positionAlloc4->Add (Vector(650.327293510762,197.91531679621175, 0.0));
    positionAlloc4->Add (Vector(829.8056351578001,543.5321677451675, 0.0));
    positionAlloc4->Add (Vector(262.8531683825858,609.4867224504599, 0.0));
    positionAlloc4->Add (Vector(482.91733427540584,435.4236735019126, 0.0));
    positionAlloc4->Add (Vector(522.1005068807119,172.8613425491471, 0.0));
    positionAlloc4->Add (Vector(803.624596101029,628.7065162847402, 0.0));
    positionAlloc4->Add (Vector(802.7884148935184,233.0506360191062, 0.0));
    positionAlloc4->Add (Vector(471.3868163006766,362.300123594498, 0.0));
    positionAlloc4->Add (Vector(726.5365275242815,419.2277531851377, 0.0));
    positionAlloc4->Add (Vector(416.6938915678799,715.8840172255836, 0.0));
    positionAlloc4->Add (Vector(738.636097282035,304.8670875122448, 0.0));
    positionAlloc4->Add (Vector(476.6851846190859,487.49246924388933, 0.0));
    positionAlloc4->Add (Vector(296.3415615947922,209.2818393127148, 0.0));
    positionAlloc4->Add (Vector(714.0001679677525,379.72563795832843, 0.0));
    positionAlloc4->Add (Vector(395.5733765345109,403.7918575913662, 0.0));
    positionAlloc4->Add (Vector(724.4873125367455,484.1452786856809, 0.0));
    positionAlloc4->Add (Vector(401.44612288788323,726.4865586282733, 0.0));
    positionAlloc4->Add (Vector(327.00068277507785,554.3422280410958, 0.0));
    positionAlloc4->Add (Vector(308.565138225274,579.7792144276629, 0.0));
    positionAlloc4->Add (Vector(634.4078791626582,470.5502961038195, 0.0));
    positionAlloc4->Add (Vector(185.24466887854726,186.93568050172183, 0.0));
    positionAlloc4->Add (Vector(451.86370885409053,393.2631567196282, 0.0));
    positionAlloc4->Add (Vector(331.76958726528983,828.8065429585839, 0.0));
    positionAlloc4->Add (Vector(723.1181606388355,838.4970445595641, 0.0));
    positionAlloc4->Add (Vector(357.4503294593019,806.8210100559213, 0.0));
    positionAlloc4->Add (Vector(270.76671386245346,547.5820081212564, 0.0));
    positionAlloc4->Add (Vector(522.6043131660181,222.2855062125339, 0.0));
    positionAlloc4->Add (Vector(172.71041223303087,611.1023471089155, 0.0));
    positionAlloc4->Add (Vector(566.3931656513366,667.2066739931229, 0.0));
    positionAlloc4->Add (Vector(300.28712122434877,197.06392606339278, 0.0));
    positionAlloc4->Add (Vector(233.09636685637804,836.3977589425065, 0.0));
    positionAlloc4->Add (Vector(553.8001667928902,474.78718871682116, 0.0));
    positionAlloc4->Add (Vector(710.6864902823554,223.11816267010596, 0.0));
    positionAlloc4->Add (Vector(157.56775854463743,728.585751934025, 0.0));
    positionAlloc4->Add (Vector(429.66314921085433,378.75629410044417, 0.0));
    positionAlloc4->Add (Vector(609.9213487638751,778.8705266088244, 0.0));
    positionAlloc4->Add (Vector(724.6867121093986,546.6236812338241, 0.0));
    positionAlloc4->Add (Vector(611.0629484578369,802.0884400166367, 0.0));
    positionAlloc4->Add (Vector(390.7703805903799,663.1000750558362, 0.0));
    positionAlloc4->Add (Vector(532.6869774504911,781.6891233002724, 0.0));
    positionAlloc4->Add (Vector(265.08387035521514,231.6081521889877, 0.0));
    positionAlloc4->Add (Vector(567.2573686668143,453.69074599867093, 0.0));
    positionAlloc4->Add (Vector(494.97370917297724,359.9594488439159, 0.0));
    positionAlloc4->Add (Vector(522.0184399325137,351.5617398731947, 0.0));
    positionAlloc4->Add (Vector(233.92376543089847,664.4480931764259, 0.0));
    positionAlloc4->Add (Vector(450.9703338458752,731.6644197385478, 0.0));
    positionAlloc4->Add (Vector(279.24272014315846,389.4753346045158, 0.0));
    positionAlloc4->Add (Vector(340.14322689205875,283.42688315211484, 0.0));
    positionAlloc4->Add (Vector(369.7146535605191,633.7390689174832, 0.0));
    positionAlloc4->Add (Vector(776.8939936922872,786.42268483751, 0.0));
    positionAlloc4->Add (Vector(431.68438150951823,769.2108217607296, 0.0));
    positionAlloc4->Add (Vector(614.4665157394165,437.83824221778207, 0.0));
    positionAlloc4->Add (Vector(154.8145735537545,298.7130940368311, 0.0));
    positionAlloc4->Add (Vector(679.9336802890379,188.92386517632244, 0.0));
    positionAlloc4->Add (Vector(783.0518424957,722.1854931142766, 0.0));
    positionAlloc4->Add (Vector(677.707657896939,554.5237788858618, 0.0));
    positionAlloc4->Add (Vector(613.339542845908,675.8239958678299, 0.0));
    positionAlloc4->Add (Vector(322.66103106378455,479.6721767043135, 0.0));
    positionAlloc4->Add (Vector(462.70016908096426,501.0625722451165, 0.0));
    positionAlloc4->Add (Vector(255.33058970419825,827.6534670155135, 0.0));
    positionAlloc4->Add (Vector(263.95369244436057,566.6489838888124, 0.0));
    positionAlloc4->Add (Vector(193.26592248245584,214.65236967796494, 0.0));
    positionAlloc4->Add (Vector(410.7562520874261,800.0413934807758, 0.0));
    positionAlloc4->Add (Vector(804.839159469247,280.01315040362994, 0.0));
    positionAlloc4->Add (Vector(390.37069152435095,734.2002531789394, 0.0));
    positionAlloc4->Add (Vector(459.9586955806162,332.73396836439366, 0.0));
    positionAlloc4->Add (Vector(387.9803046552737,205.25758206258888, 0.0));
    positionAlloc4->Add (Vector(843.076192260806,368.31560503460764, 0.0));
    positionAlloc4->Add (Vector(644.3723137886835,585.8826786648582, 0.0));
    positionAlloc4->Add (Vector(157.19914715498442,651.2349812695011, 0.0));
    positionAlloc4->Add (Vector(347.1312294739422,824.3522243122266, 0.0));
    positionAlloc4->Add (Vector(667.3468831575608,344.2252758074678, 0.0));
    positionAlloc4->Add (Vector(813.0130240301735,769.3985823872031, 0.0));
    positionAlloc4->Add (Vector(372.2847901803119,743.6764645109411, 0.0));
    positionAlloc4->Add (Vector(578.0245278579625,624.9499778048464, 0.0));
    positionAlloc4->Add (Vector(803.2435614315143,335.84748520035066, 0.0));
    positionAlloc4->Add (Vector(823.0446272018785,617.5075346905694, 0.0));
    positionAlloc4->Add (Vector(391.5586218592636,171.07271054792326, 0.0));
    positionAlloc4->Add (Vector(692.6708362844645,314.09792000806027, 0.0));
    positionAlloc4->Add (Vector(728.8009926809272,517.3149463630025, 0.0));
    positionAlloc4->Add (Vector(196.31563218410832,163.40883988587703, 0.0));
    positionAlloc4->Add (Vector(800.4013020481812,421.1257625827198, 0.0));
    positionAlloc4->Add (Vector(624.175190922211,546.0470883886974, 0.0));
    positionAlloc4->Add (Vector(690.9399937655554,315.85332743465403, 0.0));
    positionAlloc4->Add (Vector(675.538754447739,237.84040775940548, 0.0));
    positionAlloc4->Add (Vector(711.0120433560676,424.4655038332009, 0.0));
    positionAlloc4->Add (Vector(271.4472352485154,415.82606604352225, 0.0));
    positionAlloc4->Add (Vector(575.8236500054402,346.73482511682244, 0.0));
    positionAlloc4->Add (Vector(657.2188007933403,228.11509997007323, 0.0));
    positionAlloc4->Add (Vector(316.23498972658024,184.7300613371862, 0.0));
    positionAlloc4->Add (Vector(167.4801634819347,464.32713755108813, 0.0));
    positionAlloc4->Add (Vector(737.4262764862091,262.47570929829163, 0.0));
    positionAlloc4->Add (Vector(535.8119733414604,456.72502927384113, 0.0));
    positionAlloc4->Add (Vector(185.1701068760875,174.43808685935667, 0.0));
    positionAlloc4->Add (Vector(506.89780668532035,500.89295123801054, 0.0));
    positionAlloc4->Add (Vector(580.5903983788398,787.4240020500952, 0.0));
    positionAlloc4->Add (Vector(388.28808927998176,507.4065302216919, 0.0));
    positionAlloc4->Add (Vector(171.32458041627208,293.1138607249911, 0.0));
    positionAlloc4->Add (Vector(729.2871365918329,269.4958543187361, 0.0));
    positionAlloc4->Add (Vector(688.614225857433,428.505992112199, 0.0));
    positionAlloc4->Add (Vector(567.8374833826592,849.690392275307, 0.0));
    positionAlloc4->Add (Vector(491.08574787003556,450.9896498847418, 0.0));
    positionAlloc4->Add (Vector(682.4029497186976,258.1445428034481, 0.0));
    positionAlloc4->Add (Vector(477.6483379640031,476.4095890578732, 0.0));
    positionAlloc4->Add (Vector(316.7026575659874,785.9041863155627, 0.0));
    positionAlloc4->Add (Vector(354.0226394456557,295.27865534399245, 0.0));
    positionAlloc4->Add (Vector(794.5177552398447,178.04138333548613, 0.0));
    positionAlloc4->Add (Vector(215.64356837991153,490.29133965044684, 0.0));
    positionAlloc4->Add (Vector(470.8265162113028,395.8535711441466, 0.0));
    positionAlloc4->Add (Vector(675.1860347571895,256.43399719714427, 0.0));
    positionAlloc4->Add (Vector(704.8163692463943,589.1302545634746, 0.0));
    positionAlloc4->Add (Vector(150.45737353290968,759.4683238878623, 0.0));
    positionAlloc4->Add (Vector(324.7682669181611,334.27314256260786, 0.0));
    positionAlloc4->Add (Vector(302.74527418652053,761.8502413136296, 0.0));
    positionAlloc4->Add (Vector(620.9352300887267,304.35298440495296, 0.0));
    positionAlloc4->Add (Vector(707.1484710004517,272.5861980912473, 0.0));
    positionAlloc4->Add (Vector(539.7494376497316,741.5845960472243, 0.0));
    positionAlloc4->Add (Vector(407.1735100879493,210.91205281195212, 0.0));
    positionAlloc4->Add (Vector(412.1837703657069,708.4125843437498, 0.0));
    positionAlloc4->Add (Vector(549.8823289559682,250.1007858660085, 0.0));
    positionAlloc4->Add (Vector(798.9866473051145,828.9485677416336, 0.0));
    positionAlloc4->Add (Vector(727.9976660924669,349.9952973626232, 0.0));
    positionAlloc4->Add (Vector(303.0968602265083,277.98644590817196, 0.0));
    positionAlloc4->Add (Vector(279.6892487683266,345.0501441805255, 0.0));
    positionAlloc4->Add (Vector(248.8564803612046,549.6141917876334, 0.0));
    positionAlloc4->Add (Vector(410.52127867282803,613.1670817404323, 0.0));
    positionAlloc4->Add (Vector(310.8787080583329,536.8635874437238, 0.0));
    positionAlloc4->Add (Vector(492.7371234351713,601.3216740037731, 0.0));
    positionAlloc4->Add (Vector(547.4091225517382,555.5201266633565, 0.0));
    positionAlloc4->Add (Vector(816.0113779641644,272.8441391669502, 0.0));
    positionAlloc4->Add (Vector(238.20141950479598,287.49345111041214, 0.0));
    positionAlloc4->Add (Vector(467.7929378397893,683.7128710724079, 0.0));
    positionAlloc4->Add (Vector(434.7633606662065,379.7631739748376, 0.0));
    positionAlloc4->Add (Vector(402.9826453470305,751.856272368152, 0.0));
    positionAlloc4->Add (Vector(488.22660431956797,736.7612947572604, 0.0));
    positionAlloc4->Add (Vector(669.1490162852295,494.7983495273083, 0.0));
    positionAlloc4->Add (Vector(286.6165620530553,492.5747761846121, 0.0));
    positionAlloc4->Add (Vector(772.6635177306342,410.02708383706374, 0.0));
    positionAlloc4->Add (Vector(738.0756549074399,842.6157916755395, 0.0));
    positionAlloc4->Add (Vector(369.71284770572936,526.615893754732, 0.0));
    positionAlloc4->Add (Vector(693.9344999562293,753.5420815607322, 0.0));
    positionAlloc4->Add (Vector(552.0013025631476,617.0486360359267, 0.0));
    positionAlloc4->Add (Vector(470.9019968920066,657.5042534093832, 0.0));
    positionAlloc4->Add (Vector(189.8851088981578,443.02509244283095, 0.0));
    positionAlloc4->Add (Vector(364.5244276299742,725.666847829748, 0.0));
    positionAlloc4->Add (Vector(207.59062699394332,471.70884004953615, 0.0));
    positionAlloc4->Add (Vector(341.54829250254016,491.48402298080873, 0.0));
    positionAlloc4->Add (Vector(250.8551502092397,586.7989398702878, 0.0));
    positionAlloc4->Add (Vector(556.4893591879995,503.85613227290474, 0.0));
    positionAlloc4->Add (Vector(712.6634233384326,246.00394170692022, 0.0));
    positionAlloc4->Add (Vector(361.69365789497544,544.3858551386979, 0.0));
    positionAlloc4->Add (Vector(207.91578493273292,489.17659655600755, 0.0));
    positionAlloc4->Add (Vector(755.2612453006981,258.44771780040986, 0.0));
    positionAlloc4->Add (Vector(252.84534650863952,389.85160256576506, 0.0));
    positionAlloc4->Add (Vector(297.35485776574006,716.3165482041735, 0.0));
    positionAlloc4->Add (Vector(621.7959457020113,193.7953895536161, 0.0));
    positionAlloc4->Add (Vector(329.5241427117362,398.7238239317214, 0.0));
    positionAlloc4->Add (Vector(312.7541945454818,240.77528475696477, 0.0));
    positionAlloc4->Add (Vector(378.3777426288027,359.5683615937438, 0.0));
    positionAlloc4->Add (Vector(344.8579371638596,708.8052380464306, 0.0));
    positionAlloc4->Add (Vector(827.4330256886934,457.818340555536, 0.0));
    positionAlloc4->Add (Vector(831.1537585386088,681.5047548429316, 0.0));
    positionAlloc4->Add (Vector(267.2437623658754,253.09289448646365, 0.0));
    positionAlloc4->Add (Vector(337.9935172175498,220.68467477243277, 0.0));
    positionAlloc4->Add (Vector(310.55604392637065,478.3750477333051, 0.0));
    positionAlloc4->Add (Vector(199.72442233080147,297.55517108502, 0.0));
    positionAlloc4->Add (Vector(209.57244028384974,419.9183147593333, 0.0));
    positionAlloc4->Add (Vector(746.7831948721794,806.5756620037259, 0.0));
    positionAlloc4->Add (Vector(832.7401934353494,721.8817350902857, 0.0));
    positionAlloc4->Add (Vector(384.695335054807,196.911576425847, 0.0));
    positionAlloc4->Add (Vector(743.1715513999616,193.67584801879406, 0.0));
    positionAlloc4->Add (Vector(831.3678334153748,475.5066622684993, 0.0));
    positionAlloc4->Add (Vector(778.1561060536524,585.8413735190607, 0.0));
    positionAlloc4->Add (Vector(848.0665299325219,171.01820373057495, 0.0));
    positionAlloc4->Add (Vector(350.5636842489861,711.8482186470508, 0.0));
    positionAlloc4->Add (Vector(287.6575094670711,666.1982600394741, 0.0));
    positionAlloc4->Add (Vector(754.0977819512104,770.2654047192457, 0.0));
    positionAlloc4->Add (Vector(611.3317870925626,297.8622820310517, 0.0));
    positionAlloc4->Add (Vector(766.6811039477602,768.0403715631055, 0.0));
    positionAlloc4->Add (Vector(209.1957646680665,574.7928688780974, 0.0));
    positionAlloc4->Add (Vector(172.22063576109204,446.33460090723827, 0.0));
    positionAlloc4->Add (Vector(398.447262613907,186.39254638439525, 0.0));
    positionAlloc4->Add (Vector(558.2335629939682,564.9881263556772, 0.0));
    positionAlloc4->Add (Vector(343.90876702818514,794.9152585490829, 0.0));
    positionAlloc4->Add (Vector(257.5837299909432,550.8250630174157, 0.0));
    positionAlloc4->Add (Vector(491.94694286193374,431.3210399235111, 0.0));
    positionAlloc4->Add (Vector(822.6441028501772,525.1545551964828, 0.0));
    positionAlloc4->Add (Vector(736.7084824920498,711.9676543771503, 0.0));
    positionAlloc4->Add (Vector(576.7881976068311,639.7476987354852, 0.0));
    positionAlloc4->Add (Vector(163.40140759778566,695.3718104770508, 0.0));
    positionAlloc4->Add (Vector(736.1569558391054,268.8099000038462, 0.0));
    positionAlloc4->Add (Vector(468.6203942135595,617.4324141222071, 0.0));
    positionAlloc4->Add (Vector(275.54920720246866,643.5974018993943, 0.0));
    positionAlloc4->Add (Vector(595.1582287786975,569.5114812976065, 0.0));
    positionAlloc4->Add (Vector(342.7017494163021,259.34459808063536, 0.0));
    positionAlloc4->Add (Vector(637.9950641242665,604.0472697381708, 0.0));
    positionAlloc4->Add (Vector(500.2516123508924,633.618783585576, 0.0));
    positionAlloc4->Add (Vector(809.1843906606214,267.1499225657467, 0.0));
    positionAlloc4->Add (Vector(624.2721297185792,411.1858328956685, 0.0));
    positionAlloc4->Add (Vector(468.33503978686275,622.3504115171874, 0.0));
    positionAlloc4->Add (Vector(565.2107533276278,187.3435978922284, 0.0));
    positionAlloc4->Add (Vector(713.0898417742313,650.57011289859, 0.0));
    positionAlloc4->Add (Vector(836.782535507555,604.656250795213, 0.0));
    positionAlloc4->Add (Vector(298.1451289989028,846.9417766492185, 0.0));
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
	  AnimationInterface anim ("scratch/animations/animation_SUI_4.xml");
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
	  flowmon->SerializeToXmlFile ("scratch/switch_SUI_flowmon/switch_SUI_4.flowmon", false, false);
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