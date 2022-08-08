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
	  uint16_t numberOfNodes = 599;
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
    positionAlloc->Add (Vector (772.2222222222222,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,150.0, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,150.0, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,461.1111111111111, 0.0));
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
    positionAlloc4->Add (Vector(224.45489311901594,297.66690347692736, 0.0));
    positionAlloc4->Add (Vector(576.8681605850911,794.5646109054685, 0.0));
    positionAlloc4->Add (Vector(268.0211838662867,162.9888773489023, 0.0));
    positionAlloc4->Add (Vector(554.992038639384,474.0561335040284, 0.0));
    positionAlloc4->Add (Vector(426.3886998355147,449.72298122350526, 0.0));
    positionAlloc4->Add (Vector(482.2537104472634,801.8106637774356, 0.0));
    positionAlloc4->Add (Vector(782.2446675410525,164.68809740041223, 0.0));
    positionAlloc4->Add (Vector(189.90461625478466,416.5486142144768, 0.0));
    positionAlloc4->Add (Vector(511.6623470613582,419.59851541810565, 0.0));
    positionAlloc4->Add (Vector(223.4330704445179,760.477286155119, 0.0));
    positionAlloc4->Add (Vector(253.887086367846,625.7960404953666, 0.0));
    positionAlloc4->Add (Vector(808.9029652759939,587.9685309474345, 0.0));
    positionAlloc4->Add (Vector(397.9544746239932,635.9939819557474, 0.0));
    positionAlloc4->Add (Vector(696.5780833876497,688.2392470638832, 0.0));
    positionAlloc4->Add (Vector(372.583234612635,518.4895272764147, 0.0));
    positionAlloc4->Add (Vector(282.49145011751284,523.4553637615406, 0.0));
    positionAlloc4->Add (Vector(487.02404234616944,608.3936436723895, 0.0));
    positionAlloc4->Add (Vector(353.860529761885,715.2581174136043, 0.0));
    positionAlloc4->Add (Vector(493.8072220173153,443.4658590481521, 0.0));
    positionAlloc4->Add (Vector(316.23368650393684,333.5200862191308, 0.0));
    positionAlloc4->Add (Vector(662.0679306880071,555.0027879682566, 0.0));
    positionAlloc4->Add (Vector(817.0086642506586,345.5494262685319, 0.0));
    positionAlloc4->Add (Vector(181.37141746956507,820.2440905096661, 0.0));
    positionAlloc4->Add (Vector(509.9317266447079,434.39236039171055, 0.0));
    positionAlloc4->Add (Vector(207.1421548959368,358.9458789511581, 0.0));
    positionAlloc4->Add (Vector(704.5511326633423,193.4316822952807, 0.0));
    positionAlloc4->Add (Vector(354.66080903117825,256.44180944462266, 0.0));
    positionAlloc4->Add (Vector(809.8044123250211,266.49360655212195, 0.0));
    positionAlloc4->Add (Vector(434.71967103391796,533.4009957254651, 0.0));
    positionAlloc4->Add (Vector(536.071457052598,640.3342287830992, 0.0));
    positionAlloc4->Add (Vector(213.05170406282377,847.5929936596998, 0.0));
    positionAlloc4->Add (Vector(579.4602779022588,474.2796230761519, 0.0));
    positionAlloc4->Add (Vector(672.4932578161371,844.5060046610337, 0.0));
    positionAlloc4->Add (Vector(399.03250049601183,739.597469123751, 0.0));
    positionAlloc4->Add (Vector(480.0754713047713,397.7493736027126, 0.0));
    positionAlloc4->Add (Vector(843.2784572643753,326.83362242124224, 0.0));
    positionAlloc4->Add (Vector(826.493521763219,388.9885768392138, 0.0));
    positionAlloc4->Add (Vector(752.5564229021192,156.70788139920893, 0.0));
    positionAlloc4->Add (Vector(701.7435777501419,650.697491929024, 0.0));
    positionAlloc4->Add (Vector(187.55651480678353,450.47911221708586, 0.0));
    positionAlloc4->Add (Vector(450.0518985206279,313.7074205461634, 0.0));
    positionAlloc4->Add (Vector(156.96753799935505,480.1501679712065, 0.0));
    positionAlloc4->Add (Vector(313.79753515343947,278.56307714732, 0.0));
    positionAlloc4->Add (Vector(400.8260500125682,607.3031713316384, 0.0));
    positionAlloc4->Add (Vector(302.43383298431246,273.63639821721404, 0.0));
    positionAlloc4->Add (Vector(230.2768696650691,649.984423975294, 0.0));
    positionAlloc4->Add (Vector(623.7365788677484,451.47150003771856, 0.0));
    positionAlloc4->Add (Vector(722.1424263679526,314.434382791867, 0.0));
    positionAlloc4->Add (Vector(744.1158948152217,310.90871384157805, 0.0));
    positionAlloc4->Add (Vector(480.11627059375564,589.1359772977239, 0.0));
    positionAlloc4->Add (Vector(574.8617889867427,236.16149738228938, 0.0));
    positionAlloc4->Add (Vector(195.07506252636546,338.43050812018106, 0.0));
    positionAlloc4->Add (Vector(765.4743301544493,263.7354073638324, 0.0));
    positionAlloc4->Add (Vector(349.0942267326541,337.0728825497366, 0.0));
    positionAlloc4->Add (Vector(295.80840615928423,644.8978745412928, 0.0));
    positionAlloc4->Add (Vector(820.2433717148548,471.0399412020261, 0.0));
    positionAlloc4->Add (Vector(336.1877775659593,308.4367949062402, 0.0));
    positionAlloc4->Add (Vector(830.0339491891917,810.3991563751982, 0.0));
    positionAlloc4->Add (Vector(372.99416868819276,690.6854054862692, 0.0));
    positionAlloc4->Add (Vector(463.7671925887775,577.5469492545631, 0.0));
    positionAlloc4->Add (Vector(786.7296578180385,167.52543536621067, 0.0));
    positionAlloc4->Add (Vector(424.4897028784906,656.7097268203021, 0.0));
    positionAlloc4->Add (Vector(778.7670187401031,476.98616488519764, 0.0));
    positionAlloc4->Add (Vector(319.6785133763223,344.33364973357186, 0.0));
    positionAlloc4->Add (Vector(228.30196244242572,592.0758368198623, 0.0));
    positionAlloc4->Add (Vector(315.182903983616,349.3543923466999, 0.0));
    positionAlloc4->Add (Vector(450.3509372185023,162.33155499877512, 0.0));
    positionAlloc4->Add (Vector(204.58798309298834,547.1379506982549, 0.0));
    positionAlloc4->Add (Vector(398.1231342071206,669.9716834041321, 0.0));
    positionAlloc4->Add (Vector(733.8295875486692,159.1933615969536, 0.0));
    positionAlloc4->Add (Vector(830.3339778561693,652.1294509286087, 0.0));
    positionAlloc4->Add (Vector(349.96106870801793,279.5761112994917, 0.0));
    positionAlloc4->Add (Vector(645.8610972159083,730.1209860099339, 0.0));
    positionAlloc4->Add (Vector(430.70687384107674,721.2879015577677, 0.0));
    positionAlloc4->Add (Vector(164.18914397113682,687.7931634596658, 0.0));
    positionAlloc4->Add (Vector(269.4505121264737,367.9634358337267, 0.0));
    positionAlloc4->Add (Vector(394.6321471707185,566.7175262904151, 0.0));
    positionAlloc4->Add (Vector(598.5133515152118,719.3907900246082, 0.0));
    positionAlloc4->Add (Vector(233.8448659184927,264.325686800016, 0.0));
    positionAlloc4->Add (Vector(657.4093539998466,578.402728132012, 0.0));
    positionAlloc4->Add (Vector(232.43479459363294,381.20090874447715, 0.0));
    positionAlloc4->Add (Vector(552.2922427436415,440.89199610599957, 0.0));
    positionAlloc4->Add (Vector(666.9899284434897,355.741617823396, 0.0));
    positionAlloc4->Add (Vector(238.6347851835411,408.8267555811196, 0.0));
    positionAlloc4->Add (Vector(840.7429181698903,334.5887788157469, 0.0));
    positionAlloc4->Add (Vector(398.238966110183,182.22319529920298, 0.0));
    positionAlloc4->Add (Vector(617.9675969122795,741.5739486861833, 0.0));
    positionAlloc4->Add (Vector(437.63144129201163,166.16273587754057, 0.0));
    positionAlloc4->Add (Vector(383.5197065895941,362.52353296096464, 0.0));
    positionAlloc4->Add (Vector(625.6131350716184,465.83389638768773, 0.0));
    positionAlloc4->Add (Vector(610.6647926577675,452.8945144740926, 0.0));
    positionAlloc4->Add (Vector(276.26583821254997,485.7296279148583, 0.0));
    positionAlloc4->Add (Vector(486.1214093017127,569.2766454267128, 0.0));
    positionAlloc4->Add (Vector(529.0005760134977,580.1035583845667, 0.0));
    positionAlloc4->Add (Vector(265.86381492343673,250.97591988161406, 0.0));
    positionAlloc4->Add (Vector(669.2483316227923,801.5859220919374, 0.0));
    positionAlloc4->Add (Vector(280.68600822298845,737.0043664599974, 0.0));
    positionAlloc4->Add (Vector(780.283722995547,364.9576891583388, 0.0));
    positionAlloc4->Add (Vector(320.0367702751548,576.2069662404631, 0.0));
    positionAlloc4->Add (Vector(390.0225307172392,378.6746726666586, 0.0));
    positionAlloc4->Add (Vector(352.33481306204544,395.9208816772229, 0.0));
    positionAlloc4->Add (Vector(755.4779916013362,525.4447395296892, 0.0));
    positionAlloc4->Add (Vector(248.36660692606011,664.1446804277357, 0.0));
    positionAlloc4->Add (Vector(765.9899840925855,220.5633494079903, 0.0));
    positionAlloc4->Add (Vector(665.0183149254144,160.84994168322473, 0.0));
    positionAlloc4->Add (Vector(442.139031286428,830.6558393459848, 0.0));
    positionAlloc4->Add (Vector(416.7494735040956,692.1921749402111, 0.0));
    positionAlloc4->Add (Vector(699.3860748478447,228.4795538367373, 0.0));
    positionAlloc4->Add (Vector(599.2225097507402,762.3085536548457, 0.0));
    positionAlloc4->Add (Vector(237.37236832928986,707.6173689855357, 0.0));
    positionAlloc4->Add (Vector(194.11077348496735,570.417692505615, 0.0));
    positionAlloc4->Add (Vector(784.2136190435417,401.88242246479103, 0.0));
    positionAlloc4->Add (Vector(347.3708289998518,351.52225056588975, 0.0));
    positionAlloc4->Add (Vector(641.7836563876439,691.8023190368317, 0.0));
    positionAlloc4->Add (Vector(259.7386061345016,747.0349614077904, 0.0));
    positionAlloc4->Add (Vector(563.8293509812513,828.381571658517, 0.0));
    positionAlloc4->Add (Vector(643.9521554541093,233.23211237459537, 0.0));
    positionAlloc4->Add (Vector(504.9020933473204,691.3264868928537, 0.0));
    positionAlloc4->Add (Vector(588.5523150862675,346.5263655224388, 0.0));
    positionAlloc4->Add (Vector(627.7152941332802,209.77838449455675, 0.0));
    positionAlloc4->Add (Vector(549.1676097120605,547.8528277050423, 0.0));
    positionAlloc4->Add (Vector(648.6760113670916,230.82884262452154, 0.0));
    positionAlloc4->Add (Vector(163.10729965607348,647.8590970293517, 0.0));
    positionAlloc4->Add (Vector(510.1124996947168,548.6442869640068, 0.0));
    positionAlloc4->Add (Vector(504.31905995864173,520.4141706689637, 0.0));
    positionAlloc4->Add (Vector(605.1246881727084,238.98350429993394, 0.0));
    positionAlloc4->Add (Vector(831.3149556502635,843.2689243887534, 0.0));
    positionAlloc4->Add (Vector(493.565074616892,463.6637789571439, 0.0));
    positionAlloc4->Add (Vector(206.75244130310915,789.9619634409138, 0.0));
    positionAlloc4->Add (Vector(192.98460819878517,793.2543201773481, 0.0));
    positionAlloc4->Add (Vector(284.7632102738586,646.5819388035319, 0.0));
    positionAlloc4->Add (Vector(547.5233838137837,734.5883143402428, 0.0));
    positionAlloc4->Add (Vector(361.6789638802844,317.11691175382, 0.0));
    positionAlloc4->Add (Vector(393.30593305025013,846.3948649432548, 0.0));
    positionAlloc4->Add (Vector(204.94469712683448,653.8510837277993, 0.0));
    positionAlloc4->Add (Vector(820.8089381499104,748.2717091752112, 0.0));
    positionAlloc4->Add (Vector(391.47345933146255,223.36279267771232, 0.0));
    positionAlloc4->Add (Vector(345.10890470898687,515.8504031918044, 0.0));
    positionAlloc4->Add (Vector(491.95954799702713,725.8574213352584, 0.0));
    positionAlloc4->Add (Vector(161.41413125055337,271.8448961851159, 0.0));
    positionAlloc4->Add (Vector(468.99439909420823,470.44554588594787, 0.0));
    positionAlloc4->Add (Vector(473.30512130011874,597.7212239591812, 0.0));
    positionAlloc4->Add (Vector(672.4143186413265,497.8141741807569, 0.0));
    positionAlloc4->Add (Vector(593.8616403298827,761.7995978765259, 0.0));
    positionAlloc4->Add (Vector(601.4746395216373,538.9443300434789, 0.0));
    positionAlloc4->Add (Vector(488.91497317440127,477.6571347595689, 0.0));
    positionAlloc4->Add (Vector(814.3679622498985,842.8061927883005, 0.0));
    positionAlloc4->Add (Vector(173.03963806282158,575.5700286670317, 0.0));
    positionAlloc4->Add (Vector(815.4773557649577,226.20231265265863, 0.0));
    positionAlloc4->Add (Vector(446.2676090431305,544.7244789804863, 0.0));
    positionAlloc4->Add (Vector(650.6203668267065,408.6351065436133, 0.0));
    positionAlloc4->Add (Vector(679.5386943699416,408.2400613475341, 0.0));
    positionAlloc4->Add (Vector(181.73191143359585,403.1314388972827, 0.0));
    positionAlloc4->Add (Vector(445.17715279702605,461.0695186466392, 0.0));
    positionAlloc4->Add (Vector(202.38997502998356,482.60302610981773, 0.0));
    positionAlloc4->Add (Vector(163.1112044422939,424.1213166450888, 0.0));
    positionAlloc4->Add (Vector(236.91213058346756,504.9333613333747, 0.0));
    positionAlloc4->Add (Vector(379.080161589765,610.7381857493567, 0.0));
    positionAlloc4->Add (Vector(736.8793619406845,693.6189375376816, 0.0));
    positionAlloc4->Add (Vector(277.8472829912589,231.3720268829566, 0.0));
    positionAlloc4->Add (Vector(697.7075866492124,645.5244214885532, 0.0));
    positionAlloc4->Add (Vector(310.52326521535775,261.18638676741097, 0.0));
    positionAlloc4->Add (Vector(339.85506426888276,706.3811831230288, 0.0));
    positionAlloc4->Add (Vector(403.6874492693112,599.0412295639117, 0.0));
    positionAlloc4->Add (Vector(606.6234410874856,541.3076248177892, 0.0));
    positionAlloc4->Add (Vector(374.7138673856298,335.72684904526704, 0.0));
    positionAlloc4->Add (Vector(223.9319316832152,662.5681751441224, 0.0));
    positionAlloc4->Add (Vector(500.81663999422045,408.2285604644155, 0.0));
    positionAlloc4->Add (Vector(197.01044766797492,661.7250308298176, 0.0));
    positionAlloc4->Add (Vector(169.40772733329442,189.82120684930436, 0.0));
    positionAlloc4->Add (Vector(471.39093544280547,701.3208025231511, 0.0));
    positionAlloc4->Add (Vector(556.8979123727418,226.21713122181603, 0.0));
    positionAlloc4->Add (Vector(799.0286382155433,842.9544660240997, 0.0));
    positionAlloc4->Add (Vector(698.7725509427125,696.6406765081682, 0.0));
    positionAlloc4->Add (Vector(613.0337381994009,779.1677651191951, 0.0));
    positionAlloc4->Add (Vector(660.6526785005685,571.4346804845455, 0.0));
    positionAlloc4->Add (Vector(537.4429513503037,211.91901847987558, 0.0));
    positionAlloc4->Add (Vector(598.666553893723,581.9535710043584, 0.0));
    positionAlloc4->Add (Vector(633.5060259602782,822.1319010069477, 0.0));
    positionAlloc4->Add (Vector(398.6648621313518,257.7726958730983, 0.0));
    positionAlloc4->Add (Vector(794.9004881760751,326.1755827022172, 0.0));
    positionAlloc4->Add (Vector(425.3895034452969,663.7555183132989, 0.0));
    positionAlloc4->Add (Vector(438.155557761142,225.4765716963629, 0.0));
    positionAlloc4->Add (Vector(749.888663997963,532.3082138450375, 0.0));
    positionAlloc4->Add (Vector(704.813798137581,495.8702910309076, 0.0));
    positionAlloc4->Add (Vector(468.99935560977985,649.7598978451643, 0.0));
    positionAlloc4->Add (Vector(543.4546962959541,845.4384651943745, 0.0));
    positionAlloc4->Add (Vector(513.0524025951685,695.3666790690971, 0.0));
    positionAlloc4->Add (Vector(813.8546117150362,402.8747862015233, 0.0));
    positionAlloc4->Add (Vector(246.85960326954677,745.0588149314453, 0.0));
    positionAlloc4->Add (Vector(801.4927952347318,776.7898406469138, 0.0));
    positionAlloc4->Add (Vector(477.7244340161402,784.3860548011346, 0.0));
    positionAlloc4->Add (Vector(600.2130158143391,425.9664152905107, 0.0));
    positionAlloc4->Add (Vector(768.7428775039949,506.11820260109937, 0.0));
    positionAlloc4->Add (Vector(655.5632793310358,782.783320313604, 0.0));
    positionAlloc4->Add (Vector(279.6231571915414,538.3895332601255, 0.0));
    positionAlloc4->Add (Vector(436.9858081205968,736.9378833960769, 0.0));
    positionAlloc4->Add (Vector(542.8439571692086,567.268969369067, 0.0));
    positionAlloc4->Add (Vector(802.8319120192336,484.9057866286136, 0.0));
    positionAlloc4->Add (Vector(667.5353371272006,528.7720415179058, 0.0));
    positionAlloc4->Add (Vector(618.8220651979475,808.8551445667034, 0.0));
    positionAlloc4->Add (Vector(202.68032091574406,531.64775517218, 0.0));
    positionAlloc4->Add (Vector(248.01798262891992,430.5687110210248, 0.0));
    positionAlloc4->Add (Vector(270.9282522083769,746.8425629400779, 0.0));
    positionAlloc4->Add (Vector(503.8792064008176,635.9623151761961, 0.0));
    positionAlloc4->Add (Vector(576.6925753333669,627.2151664459275, 0.0));
    positionAlloc4->Add (Vector(794.5165549851394,203.71765962167936, 0.0));
    positionAlloc4->Add (Vector(318.3841937502109,341.81728582694006, 0.0));
    positionAlloc4->Add (Vector(485.6123601159041,804.0629757482945, 0.0));
    positionAlloc4->Add (Vector(485.94798230307583,573.2968557211743, 0.0));
    positionAlloc4->Add (Vector(336.0415597722048,510.49149830948306, 0.0));
    positionAlloc4->Add (Vector(795.32193990982,188.07087620110065, 0.0));
    positionAlloc4->Add (Vector(800.7570144533893,155.56792689497846, 0.0));
    positionAlloc4->Add (Vector(745.3546230647366,802.1252466033627, 0.0));
    positionAlloc4->Add (Vector(467.85330101863923,518.2999317824024, 0.0));
    positionAlloc4->Add (Vector(757.4827477141476,361.08080930220575, 0.0));
    positionAlloc4->Add (Vector(640.0238286753988,254.0790746496877, 0.0));
    positionAlloc4->Add (Vector(563.8085032359174,774.56160196421, 0.0));
    positionAlloc4->Add (Vector(633.2753370155785,251.7960013668079, 0.0));
    positionAlloc4->Add (Vector(402.0146564981525,772.642993279671, 0.0));
    positionAlloc4->Add (Vector(389.7774606567254,165.29596404544512, 0.0));
    positionAlloc4->Add (Vector(482.51172989539316,421.92454982769755, 0.0));
    positionAlloc4->Add (Vector(695.0040308532868,377.95254453121726, 0.0));
    positionAlloc4->Add (Vector(839.8635498320058,406.9810371383747, 0.0));
    positionAlloc4->Add (Vector(445.30087465724785,163.38844543661125, 0.0));
    positionAlloc4->Add (Vector(475.30662937502615,576.6231102075351, 0.0));
    positionAlloc4->Add (Vector(582.6634362154932,197.1967063425772, 0.0));
    positionAlloc4->Add (Vector(252.0188231117844,310.99272930401173, 0.0));
    positionAlloc4->Add (Vector(195.43468735026886,421.7130135722218, 0.0));
    positionAlloc4->Add (Vector(812.6422221757364,423.8597548984381, 0.0));
    positionAlloc4->Add (Vector(372.947419284715,519.268146572219, 0.0));
    positionAlloc4->Add (Vector(528.5684495586529,219.20980853755566, 0.0));
    positionAlloc4->Add (Vector(837.7428822558433,259.69304345716364, 0.0));
    positionAlloc4->Add (Vector(374.8254812772248,446.2209876687284, 0.0));
    positionAlloc4->Add (Vector(247.7468794609386,508.8087768539632, 0.0));
    positionAlloc4->Add (Vector(310.81425788296997,225.12306233181056, 0.0));
    positionAlloc4->Add (Vector(777.5396908497476,252.81031308324515, 0.0));
    positionAlloc4->Add (Vector(524.3178007894578,635.6082430237095, 0.0));
    positionAlloc4->Add (Vector(457.1934667979548,803.853794490039, 0.0));
    positionAlloc4->Add (Vector(434.32040903804017,574.3241140472373, 0.0));
    positionAlloc4->Add (Vector(207.11414127084888,487.1123385755144, 0.0));
    positionAlloc4->Add (Vector(569.88885480018,173.23742054589323, 0.0));
    positionAlloc4->Add (Vector(796.4799298553583,847.1203930318011, 0.0));
    positionAlloc4->Add (Vector(842.9461996945099,345.2095942938561, 0.0));
    positionAlloc4->Add (Vector(253.35044484601116,168.4370912239864, 0.0));
    positionAlloc4->Add (Vector(409.8350906539671,165.79315553012935, 0.0));
    positionAlloc4->Add (Vector(356.88674904465233,833.0525354408803, 0.0));
    positionAlloc4->Add (Vector(795.3151553585443,275.08506660041934, 0.0));
    positionAlloc4->Add (Vector(524.5204489381488,589.8052599370869, 0.0));
    positionAlloc4->Add (Vector(521.2874815827823,427.7193798839178, 0.0));
    positionAlloc4->Add (Vector(203.59804927408138,793.930956505845, 0.0));
    positionAlloc4->Add (Vector(263.820285882589,311.03805742282793, 0.0));
    positionAlloc4->Add (Vector(320.6766968202834,394.36789054585745, 0.0));
    positionAlloc4->Add (Vector(227.30405991610155,480.11094652140685, 0.0));
    positionAlloc4->Add (Vector(354.3367296532406,492.7830295126171, 0.0));
    positionAlloc4->Add (Vector(692.3064004544209,620.2409084535609, 0.0));
    positionAlloc4->Add (Vector(196.04369238780123,684.3924699544702, 0.0));
    positionAlloc4->Add (Vector(642.7901453052643,677.3642816724579, 0.0));
    positionAlloc4->Add (Vector(262.75192464862806,836.3142290454749, 0.0));
    positionAlloc4->Add (Vector(415.7155506757079,597.037779371698, 0.0));
    positionAlloc4->Add (Vector(435.60991689878813,837.2656159024789, 0.0));
    positionAlloc4->Add (Vector(765.0351177653007,698.5715151137226, 0.0));
    positionAlloc4->Add (Vector(677.5353164090513,755.762018464977, 0.0));
    positionAlloc4->Add (Vector(695.5542855334028,314.6088990058919, 0.0));
    positionAlloc4->Add (Vector(585.7383233203261,483.18813840238295, 0.0));
    positionAlloc4->Add (Vector(395.1413477190778,690.3343703800624, 0.0));
    positionAlloc4->Add (Vector(304.6987102665446,500.69778562853696, 0.0));
    positionAlloc4->Add (Vector(654.8778682008177,678.5824525137921, 0.0));
    positionAlloc4->Add (Vector(825.9822415922584,319.520486133094, 0.0));
    positionAlloc4->Add (Vector(401.42085539151014,485.9695454243189, 0.0));
    positionAlloc4->Add (Vector(338.0674492394793,720.6282626049269, 0.0));
    positionAlloc4->Add (Vector(154.99101415470423,627.1889290579842, 0.0));
    positionAlloc4->Add (Vector(292.67364560155795,377.19499897883344, 0.0));
    positionAlloc4->Add (Vector(605.7357818845627,302.22224052272645, 0.0));
    positionAlloc4->Add (Vector(658.9649104569653,845.0011426292408, 0.0));
    positionAlloc4->Add (Vector(706.465652977274,826.6708666937052, 0.0));
    positionAlloc4->Add (Vector(792.6535935880489,283.8281273681214, 0.0));
    positionAlloc4->Add (Vector(545.8304920851753,308.16689759727774, 0.0));
    positionAlloc4->Add (Vector(713.750369113243,596.7625724521417, 0.0));
    positionAlloc4->Add (Vector(345.9507351595104,309.467203473292, 0.0));
    positionAlloc4->Add (Vector(337.5975141567545,425.2239221113203, 0.0));
    positionAlloc4->Add (Vector(609.0840713321837,258.56552438970164, 0.0));
    positionAlloc4->Add (Vector(696.7301847538777,654.4734513647628, 0.0));
    positionAlloc4->Add (Vector(593.7813859083095,230.25346211552352, 0.0));
    positionAlloc4->Add (Vector(457.8379527217005,638.128039222541, 0.0));
    positionAlloc4->Add (Vector(461.03461695807897,611.3794651350831, 0.0));
    positionAlloc4->Add (Vector(704.6456750484514,329.6889825251957, 0.0));
    positionAlloc4->Add (Vector(662.6575301031208,635.3980614697174, 0.0));
    positionAlloc4->Add (Vector(289.9022080919392,159.79538711932383, 0.0));
    positionAlloc4->Add (Vector(155.11734205845855,334.43836400003215, 0.0));
    positionAlloc4->Add (Vector(290.45057807003695,842.7109518984689, 0.0));
    positionAlloc4->Add (Vector(268.9974077236009,783.59371239093, 0.0));
    positionAlloc4->Add (Vector(663.1222466621291,577.1809186331618, 0.0));
    positionAlloc4->Add (Vector(700.3178950145599,338.8019028577772, 0.0));
    positionAlloc4->Add (Vector(534.9447493637642,338.7453156516732, 0.0));
    positionAlloc4->Add (Vector(502.9039428098214,712.9636523225587, 0.0));
    positionAlloc4->Add (Vector(579.3983830648624,170.8403351849924, 0.0));
    positionAlloc4->Add (Vector(419.58754964345553,152.48429430533992, 0.0));
    positionAlloc4->Add (Vector(778.4418477785357,431.0157053286604, 0.0));
    positionAlloc4->Add (Vector(499.03712428639847,575.9437605945318, 0.0));
    positionAlloc4->Add (Vector(506.5813086716792,657.7174028784902, 0.0));
    positionAlloc4->Add (Vector(694.0070357606315,504.1483781445193, 0.0));
    positionAlloc4->Add (Vector(558.8270604786353,566.8149520285076, 0.0));
    positionAlloc4->Add (Vector(150.8021654397794,347.683030192105, 0.0));
    positionAlloc4->Add (Vector(268.57938794282506,707.082861265309, 0.0));
    positionAlloc4->Add (Vector(730.5977829304325,191.77015897774766, 0.0));
    positionAlloc4->Add (Vector(193.4490956959578,212.9005426000328, 0.0));
    positionAlloc4->Add (Vector(453.3236431630278,533.1644274498224, 0.0));
    positionAlloc4->Add (Vector(571.0934481753302,677.2313926937219, 0.0));
    positionAlloc4->Add (Vector(762.54127491826,722.3078814945352, 0.0));
    positionAlloc4->Add (Vector(542.7341147566926,576.4224155657607, 0.0));
    positionAlloc4->Add (Vector(320.4466814296274,768.2231794300262, 0.0));
    positionAlloc4->Add (Vector(462.74681412196463,716.8488187168197, 0.0));
    positionAlloc4->Add (Vector(653.0867710032772,532.9779238167966, 0.0));
    positionAlloc4->Add (Vector(586.7349230005275,312.52498512852355, 0.0));
    positionAlloc4->Add (Vector(376.9080820735212,501.3287262818653, 0.0));
    positionAlloc4->Add (Vector(424.22366132364823,705.4648852482272, 0.0));
    positionAlloc4->Add (Vector(415.7578111080712,402.2788835836602, 0.0));
    positionAlloc4->Add (Vector(234.7341433206371,305.1264317674056, 0.0));
    positionAlloc4->Add (Vector(511.01527309005894,672.2804000422655, 0.0));
    positionAlloc4->Add (Vector(323.41531482458106,436.073185154538, 0.0));
    positionAlloc4->Add (Vector(516.234577526085,403.72189403019235, 0.0));
    positionAlloc4->Add (Vector(494.69588257901626,749.4473441694561, 0.0));
    positionAlloc4->Add (Vector(373.4477251210908,446.96607170669137, 0.0));
    positionAlloc4->Add (Vector(354.643196950606,535.9362798893533, 0.0));
    positionAlloc4->Add (Vector(779.0947807023551,274.05566831452876, 0.0));
    positionAlloc4->Add (Vector(676.1343855913696,420.7391460220703, 0.0));
    positionAlloc4->Add (Vector(682.2711675263524,422.3424967330568, 0.0));
    positionAlloc4->Add (Vector(716.9171099939975,675.8075925519049, 0.0));
    positionAlloc4->Add (Vector(166.13694841164707,828.5800787963307, 0.0));
    positionAlloc4->Add (Vector(545.9434160062533,697.6681652564852, 0.0));
    positionAlloc4->Add (Vector(448.63776143347616,384.37949608874544, 0.0));
    positionAlloc4->Add (Vector(276.4506132572865,566.1867936526636, 0.0));
    positionAlloc4->Add (Vector(723.9174918816134,825.8575771975532, 0.0));
    positionAlloc4->Add (Vector(156.84056199363687,696.1446005408336, 0.0));
    positionAlloc4->Add (Vector(504.0674784638005,574.2625383895756, 0.0));
    positionAlloc4->Add (Vector(368.7655899270145,307.7757260217808, 0.0));
    positionAlloc4->Add (Vector(205.2628753856354,524.8756744580364, 0.0));
    positionAlloc4->Add (Vector(693.2909808300791,817.3988883805692, 0.0));
    positionAlloc4->Add (Vector(301.0678304930607,432.96502908763557, 0.0));
    positionAlloc4->Add (Vector(689.540881747425,614.4110755472761, 0.0));
    positionAlloc4->Add (Vector(660.0964739327468,682.8480606158317, 0.0));
    positionAlloc4->Add (Vector(558.0202010830524,578.2165340897302, 0.0));
    positionAlloc4->Add (Vector(517.8785117880013,213.74644919123082, 0.0));
    positionAlloc4->Add (Vector(547.8135732163857,708.6875336045007, 0.0));
    positionAlloc4->Add (Vector(830.1554232336453,245.51578278932095, 0.0));
    positionAlloc4->Add (Vector(796.2581149403628,567.4658828916561, 0.0));
    positionAlloc4->Add (Vector(171.50902575534968,526.1171813025273, 0.0));
    positionAlloc4->Add (Vector(579.8058373272561,398.30747756326923, 0.0));
    positionAlloc4->Add (Vector(242.7492693864006,753.4355998832895, 0.0));
    positionAlloc4->Add (Vector(636.2356012083335,417.28736759278866, 0.0));
    positionAlloc4->Add (Vector(585.4416564974756,206.22617398267337, 0.0));
    positionAlloc4->Add (Vector(675.5527724639556,250.7060199647907, 0.0));
    positionAlloc4->Add (Vector(668.3628552777145,732.7530800226482, 0.0));
    positionAlloc4->Add (Vector(510.1515153999644,477.67143111472376, 0.0));
    positionAlloc4->Add (Vector(218.7462223342045,516.6989249299759, 0.0));
    positionAlloc4->Add (Vector(442.9614706568787,417.0439052695591, 0.0));
    positionAlloc4->Add (Vector(546.6212022783427,656.7775116414618, 0.0));
    positionAlloc4->Add (Vector(218.55685576799914,703.3558668212094, 0.0));
    positionAlloc4->Add (Vector(222.23986833408523,523.1303015220083, 0.0));
    positionAlloc4->Add (Vector(254.03689132893348,433.4979272138634, 0.0));
    positionAlloc4->Add (Vector(396.7393093186433,470.08256321427444, 0.0));
    positionAlloc4->Add (Vector(281.87508768250166,651.9836398726046, 0.0));
    positionAlloc4->Add (Vector(150.59758820618228,288.0441280617006, 0.0));
    positionAlloc4->Add (Vector(436.43256230385913,844.2504487015509, 0.0));
    positionAlloc4->Add (Vector(305.6227464499584,299.0481653378208, 0.0));
    positionAlloc4->Add (Vector(473.0702709780362,363.68501112520516, 0.0));
    positionAlloc4->Add (Vector(832.7096770549159,599.4095092726689, 0.0));
    positionAlloc4->Add (Vector(683.2046237666316,732.7407422337516, 0.0));
    positionAlloc4->Add (Vector(447.8740084048397,368.8023089545008, 0.0));
    positionAlloc4->Add (Vector(152.84241040081298,653.886922945343, 0.0));
    positionAlloc4->Add (Vector(152.93801258139564,281.634127202836, 0.0));
    positionAlloc4->Add (Vector(449.20496868590794,316.87470953230684, 0.0));
    positionAlloc4->Add (Vector(732.7346000223362,339.5824982379715, 0.0));
    positionAlloc4->Add (Vector(617.1996042048926,830.3398910879793, 0.0));
    positionAlloc4->Add (Vector(778.7908651447874,829.6888405138094, 0.0));
    positionAlloc4->Add (Vector(562.3662730716067,790.6355979971966, 0.0));
    positionAlloc4->Add (Vector(534.046874186904,593.8258813895261, 0.0));
    positionAlloc4->Add (Vector(637.8946321189069,356.10154456461805, 0.0));
    positionAlloc4->Add (Vector(732.0817319399575,580.5594765413784, 0.0));
    positionAlloc4->Add (Vector(700.5113762360963,383.81026953035797, 0.0));
    positionAlloc4->Add (Vector(848.0139699956198,577.8460569708798, 0.0));
    positionAlloc4->Add (Vector(504.03580963436383,469.0091804761724, 0.0));
    positionAlloc4->Add (Vector(409.8868468805496,253.11094199832513, 0.0));
    positionAlloc4->Add (Vector(663.0528354122657,313.9180803376497, 0.0));
    positionAlloc4->Add (Vector(744.4881253067268,618.9236075008848, 0.0));
    positionAlloc4->Add (Vector(567.5192744150984,595.411870273339, 0.0));
    positionAlloc4->Add (Vector(639.8800837576946,820.9878055646697, 0.0));
    positionAlloc4->Add (Vector(711.0012675460515,654.6988940314327, 0.0));
    positionAlloc4->Add (Vector(544.2853043644343,639.2318003259, 0.0));
    positionAlloc4->Add (Vector(841.7730089831717,816.4867146916094, 0.0));
    positionAlloc4->Add (Vector(581.3253571249054,320.41945044039835, 0.0));
    positionAlloc4->Add (Vector(689.1116661603068,306.08423235137695, 0.0));
    positionAlloc4->Add (Vector(437.062778282807,715.7489813615521, 0.0));
    positionAlloc4->Add (Vector(253.168287318144,377.20343494055714, 0.0));
    positionAlloc4->Add (Vector(489.05770593986057,551.7039417738551, 0.0));
    positionAlloc4->Add (Vector(268.8742421270142,710.2870443275724, 0.0));
    positionAlloc4->Add (Vector(304.86474354253147,519.175459861493, 0.0));
    positionAlloc4->Add (Vector(530.6087143811153,342.94475901133615, 0.0));
    positionAlloc4->Add (Vector(208.42850662737004,353.49339913306744, 0.0));
    positionAlloc4->Add (Vector(605.678000335229,428.8487253761363, 0.0));
    positionAlloc4->Add (Vector(448.87647593370644,781.4821160495944, 0.0));
    positionAlloc4->Add (Vector(672.1682325327997,591.4810318186214, 0.0));
    positionAlloc4->Add (Vector(661.3724147037535,730.0864595708013, 0.0));
    positionAlloc4->Add (Vector(658.5683355269459,788.8217652172307, 0.0));
    positionAlloc4->Add (Vector(203.76771934794436,677.1403269404366, 0.0));
    positionAlloc4->Add (Vector(592.8972169763349,396.76949588469597, 0.0));
    positionAlloc4->Add (Vector(732.0172639717487,744.727830557756, 0.0));
    positionAlloc4->Add (Vector(383.8005031786157,175.06650609241152, 0.0));
    positionAlloc4->Add (Vector(672.9603465763776,311.9298186148708, 0.0));
    positionAlloc4->Add (Vector(753.0135449830436,803.5602817857902, 0.0));
    positionAlloc4->Add (Vector(823.5848546246204,360.4039756378155, 0.0));
    positionAlloc4->Add (Vector(212.74418137457945,628.1349286206944, 0.0));
    positionAlloc4->Add (Vector(520.4221172272946,822.3358260926842, 0.0));
    positionAlloc4->Add (Vector(706.1088406449757,329.53636845767414, 0.0));
    positionAlloc4->Add (Vector(548.0229494989541,696.2367733376099, 0.0));
    positionAlloc4->Add (Vector(222.12599168257032,783.1246309866301, 0.0));
    positionAlloc4->Add (Vector(470.3998360487628,599.9765817928021, 0.0));
    positionAlloc4->Add (Vector(667.5027401225059,791.3310036517213, 0.0));
    positionAlloc4->Add (Vector(250.3629697477677,411.77918149413284, 0.0));
    positionAlloc4->Add (Vector(549.1736874631797,315.6587656357997, 0.0));
    positionAlloc4->Add (Vector(580.7839741994395,375.10875825372557, 0.0));
    positionAlloc4->Add (Vector(397.7968986812667,538.0548285798868, 0.0));
    positionAlloc4->Add (Vector(673.3512288177411,745.0832076392784, 0.0));
    positionAlloc4->Add (Vector(532.2639277074747,820.4692279273505, 0.0));
    positionAlloc4->Add (Vector(718.7157922044158,543.2697162368161, 0.0));
    positionAlloc4->Add (Vector(776.2913027169286,173.04301073476762, 0.0));
    positionAlloc4->Add (Vector(306.9468685461902,621.563693925404, 0.0));
    positionAlloc4->Add (Vector(653.8329891135324,204.18817195770515, 0.0));
    positionAlloc4->Add (Vector(478.57606497193035,803.0615247269009, 0.0));
    positionAlloc4->Add (Vector(509.30550772164884,209.17283070550064, 0.0));
    positionAlloc4->Add (Vector(292.65568470994515,171.77600893119129, 0.0));
    positionAlloc4->Add (Vector(550.9272814737137,213.92935105573264, 0.0));
    positionAlloc4->Add (Vector(518.3279670734023,245.94388718192567, 0.0));
    positionAlloc4->Add (Vector(549.5381576101311,789.9318968934576, 0.0));
    positionAlloc4->Add (Vector(209.34643661082805,551.3265991721162, 0.0));
    positionAlloc4->Add (Vector(300.68814182300184,653.0927646513811, 0.0));
    positionAlloc4->Add (Vector(506.8387714493343,516.2092442008782, 0.0));
    positionAlloc4->Add (Vector(834.1242135790837,561.4379238242627, 0.0));
    positionAlloc4->Add (Vector(223.652931899277,248.717352020235, 0.0));
    positionAlloc4->Add (Vector(488.4549522043657,732.9061684580025, 0.0));
    positionAlloc4->Add (Vector(387.21732037407196,743.6954025122247, 0.0));
    positionAlloc4->Add (Vector(511.421593162819,461.3804803185713, 0.0));
    positionAlloc4->Add (Vector(612.9814231831482,377.41240696950206, 0.0));
    positionAlloc4->Add (Vector(747.6341714013843,711.8412951200236, 0.0));
    positionAlloc4->Add (Vector(702.5261072528145,371.9621510918879, 0.0));
    positionAlloc4->Add (Vector(478.2250898561487,165.25226483357886, 0.0));
    positionAlloc4->Add (Vector(349.82040420100554,358.3703855424126, 0.0));
    positionAlloc4->Add (Vector(645.2873491435239,663.802978180579, 0.0));
    positionAlloc4->Add (Vector(373.5954690999431,215.27840884975956, 0.0));
    positionAlloc4->Add (Vector(354.03647080536894,828.1896849688741, 0.0));
    positionAlloc4->Add (Vector(576.50820750399,231.09694390263516, 0.0));
    positionAlloc4->Add (Vector(799.7717425066529,707.8823452270691, 0.0));
    positionAlloc4->Add (Vector(802.9684104791797,650.6153141769064, 0.0));
    positionAlloc4->Add (Vector(456.8865326514887,416.800320705085, 0.0));
    positionAlloc4->Add (Vector(775.7228339055341,756.0418856492528, 0.0));
    positionAlloc4->Add (Vector(772.3524417227037,464.53524160480384, 0.0));
    positionAlloc4->Add (Vector(719.063391361235,624.9794208535313, 0.0));
    positionAlloc4->Add (Vector(642.4395773475967,245.7285165293713, 0.0));
    positionAlloc4->Add (Vector(525.2394480289611,612.8336298123565, 0.0));
    positionAlloc4->Add (Vector(414.2483963962658,371.9576921750654, 0.0));
    positionAlloc4->Add (Vector(533.832398970857,215.0381629307923, 0.0));
    positionAlloc4->Add (Vector(199.30799017918224,479.9063799733217, 0.0));
    positionAlloc4->Add (Vector(772.9752252506363,826.9848247783233, 0.0));
    positionAlloc4->Add (Vector(357.6600232826426,346.9173293295198, 0.0));
    positionAlloc4->Add (Vector(356.2328097794324,155.27876490119908, 0.0));
    positionAlloc4->Add (Vector(817.8966555686665,745.5114792102261, 0.0));
    positionAlloc4->Add (Vector(703.7600050696209,398.4424882552951, 0.0));
    positionAlloc4->Add (Vector(219.64958997963097,609.313264511414, 0.0));
    positionAlloc4->Add (Vector(320.9676276617232,782.5080897111492, 0.0));
    positionAlloc4->Add (Vector(265.80914801321444,604.4324992455452, 0.0));
    positionAlloc4->Add (Vector(637.6995202471198,806.13769418729, 0.0));
    positionAlloc4->Add (Vector(356.35417880525654,711.3623930192427, 0.0));
    positionAlloc4->Add (Vector(670.7217008830333,481.4065074867921, 0.0));
    positionAlloc4->Add (Vector(523.8016140232495,315.8694538117097, 0.0));
    positionAlloc4->Add (Vector(729.0938684103573,629.7846361282247, 0.0));
    positionAlloc4->Add (Vector(667.9518760292857,613.2751185923748, 0.0));
    positionAlloc4->Add (Vector(575.5157507114884,686.3234796289379, 0.0));
    positionAlloc4->Add (Vector(772.7209765870763,788.1781030308151, 0.0));
    positionAlloc4->Add (Vector(263.9243754476664,291.4590204842072, 0.0));
    positionAlloc4->Add (Vector(834.7565680069622,479.85633163262025, 0.0));
    positionAlloc4->Add (Vector(600.5489610151084,830.5549596769039, 0.0));
    positionAlloc4->Add (Vector(820.7999397011316,208.21321319272542, 0.0));
    positionAlloc4->Add (Vector(205.16468863178517,394.3130987797102, 0.0));
    positionAlloc4->Add (Vector(759.2126449498898,807.6021389698576, 0.0));
    positionAlloc4->Add (Vector(280.49179864712926,234.13407901187708, 0.0));
    positionAlloc4->Add (Vector(405.6143489746201,485.4185431768093, 0.0));
    positionAlloc4->Add (Vector(660.4763811898902,174.99766431970016, 0.0));
    positionAlloc4->Add (Vector(762.9034566674001,510.59637142277046, 0.0));
    positionAlloc4->Add (Vector(323.0238783074243,331.28752420237674, 0.0));
    positionAlloc4->Add (Vector(266.7218980381556,467.29925243636075, 0.0));
    positionAlloc4->Add (Vector(264.6695645160207,178.10343902011826, 0.0));
    positionAlloc4->Add (Vector(585.2315702667781,370.4582139768747, 0.0));
    positionAlloc4->Add (Vector(194.25703747544355,706.8156707243112, 0.0));
    positionAlloc4->Add (Vector(266.8845175603073,337.136293688165, 0.0));
    positionAlloc4->Add (Vector(447.54772153120524,797.3501367998095, 0.0));
    positionAlloc4->Add (Vector(664.0263857529673,658.9087182898068, 0.0));
    positionAlloc4->Add (Vector(613.1074149629153,264.0008041464794, 0.0));
    positionAlloc4->Add (Vector(820.3595320720882,725.8086125130026, 0.0));
    positionAlloc4->Add (Vector(465.43639019908767,382.044591278418, 0.0));
    positionAlloc4->Add (Vector(178.48985667766533,491.7469847935443, 0.0));
    positionAlloc4->Add (Vector(809.1824459093644,289.7617806542848, 0.0));
    positionAlloc4->Add (Vector(816.9361280073331,615.2055996063278, 0.0));
    positionAlloc4->Add (Vector(778.3298058396697,176.56481116790957, 0.0));
    positionAlloc4->Add (Vector(404.101014987793,193.5569208565694, 0.0));
    positionAlloc4->Add (Vector(627.5584636941937,826.5043670590237, 0.0));
    positionAlloc4->Add (Vector(464.36157001800393,399.0812650673626, 0.0));
    positionAlloc4->Add (Vector(349.8800565086734,259.94121886939706, 0.0));
    positionAlloc4->Add (Vector(804.7675825129503,753.8124689389546, 0.0));
    positionAlloc4->Add (Vector(198.14646038836128,403.439095120744, 0.0));
    positionAlloc4->Add (Vector(642.8856479627043,613.2805027746724, 0.0));
    positionAlloc4->Add (Vector(155.38846206829896,731.0457605370456, 0.0));
    positionAlloc4->Add (Vector(577.9471215425992,515.9792691176501, 0.0));
    positionAlloc4->Add (Vector(327.77647881855546,311.0635544280277, 0.0));
    positionAlloc4->Add (Vector(830.8969814929704,542.1347879639634, 0.0));
    positionAlloc4->Add (Vector(527.5927349274269,449.114230264143, 0.0));
    positionAlloc4->Add (Vector(670.6466511013992,563.6999713601676, 0.0));
    positionAlloc4->Add (Vector(502.8665802732192,718.7191652821787, 0.0));
    positionAlloc4->Add (Vector(309.11168506696663,327.95752105775637, 0.0));
    positionAlloc4->Add (Vector(213.96000839299035,452.83045301043194, 0.0));
    positionAlloc4->Add (Vector(514.7015210638499,584.9114624204692, 0.0));
    positionAlloc4->Add (Vector(161.1619805173176,747.128463481557, 0.0));
    positionAlloc4->Add (Vector(619.2240460815472,780.7918675003202, 0.0));
    positionAlloc4->Add (Vector(299.6048069754122,616.8266896092396, 0.0));
    positionAlloc4->Add (Vector(719.2058406912574,357.96205723106857, 0.0));
    positionAlloc4->Add (Vector(415.12601390847163,593.3370072898126, 0.0));
    positionAlloc4->Add (Vector(337.9878743467756,451.98399256884977, 0.0));
    positionAlloc4->Add (Vector(462.2400579651087,368.46434113495746, 0.0));
    positionAlloc4->Add (Vector(728.9069171528465,349.24093835752336, 0.0));
    positionAlloc4->Add (Vector(631.2814338698315,174.05412045180276, 0.0));
    positionAlloc4->Add (Vector(644.3597189705815,845.7383917622475, 0.0));
    positionAlloc4->Add (Vector(639.1744068709975,818.3442866976508, 0.0));
    positionAlloc4->Add (Vector(777.9402278606045,161.98546005801333, 0.0));
    positionAlloc4->Add (Vector(379.4088696774497,413.17277151294775, 0.0));
    positionAlloc4->Add (Vector(714.4767053344881,613.7489208529132, 0.0));
    positionAlloc4->Add (Vector(593.402421897372,625.0389034917764, 0.0));
    positionAlloc4->Add (Vector(727.2420275923793,671.4756389226253, 0.0));
    positionAlloc4->Add (Vector(784.3871781028811,745.316591353581, 0.0));
    positionAlloc4->Add (Vector(176.06184122660426,748.7433680785358, 0.0));
    positionAlloc4->Add (Vector(564.7322532945304,620.5767500519494, 0.0));
    positionAlloc4->Add (Vector(655.6213241057678,241.82957274455904, 0.0));
    positionAlloc4->Add (Vector(551.3637516010909,736.9517677265459, 0.0));
    positionAlloc4->Add (Vector(563.9851890292397,170.29085106563065, 0.0));
    positionAlloc4->Add (Vector(224.70048030786288,534.9982398411505, 0.0));
    positionAlloc4->Add (Vector(561.1482417965926,171.9362224737084, 0.0));
    positionAlloc4->Add (Vector(507.6977543540397,205.40403466480933, 0.0));
    positionAlloc4->Add (Vector(292.80604555753695,234.4819274923683, 0.0));
    positionAlloc4->Add (Vector(294.50988449034827,519.2087318904449, 0.0));
    positionAlloc4->Add (Vector(515.3110809354279,722.2133553548673, 0.0));
    positionAlloc4->Add (Vector(382.4526695269352,193.57060831114666, 0.0));
    positionAlloc4->Add (Vector(559.4380501902724,158.53128570821977, 0.0));
    positionAlloc4->Add (Vector(784.102394654769,411.05249272906445, 0.0));
    positionAlloc4->Add (Vector(627.2481848486418,630.701661723394, 0.0));
    positionAlloc4->Add (Vector(656.3029638399714,199.46773321565493, 0.0));
    positionAlloc4->Add (Vector(457.3494303193577,815.2348266678733, 0.0));
    positionAlloc4->Add (Vector(767.8239366101891,827.0782357674663, 0.0));
    positionAlloc4->Add (Vector(729.6759564878153,704.0787044332402, 0.0));
    positionAlloc4->Add (Vector(528.5377629791354,639.971588899686, 0.0));
    positionAlloc4->Add (Vector(181.9007651296742,781.507932057038, 0.0));
    positionAlloc4->Add (Vector(250.39302856539246,267.4210842977219, 0.0));
    positionAlloc4->Add (Vector(385.17455398755646,253.01039067460636, 0.0));
    positionAlloc4->Add (Vector(828.4651980869763,644.1034426273793, 0.0));
    positionAlloc4->Add (Vector(558.5285512987348,179.81366777921917, 0.0));
    positionAlloc4->Add (Vector(764.7514171919421,818.8296933466149, 0.0));
    positionAlloc4->Add (Vector(403.97402132993204,579.1753171689128, 0.0));
    positionAlloc4->Add (Vector(496.01997363504177,561.9106793483662, 0.0));
    positionAlloc4->Add (Vector(201.49272028440134,498.9092883044961, 0.0));
    positionAlloc4->Add (Vector(421.9058569868019,454.77834114811975, 0.0));
    positionAlloc4->Add (Vector(441.6762663020066,370.0870184616567, 0.0));
    positionAlloc4->Add (Vector(603.6825461145843,785.8020364411302, 0.0));
    positionAlloc4->Add (Vector(534.4540699208046,419.10011314716803, 0.0));
    positionAlloc4->Add (Vector(604.1134377117787,657.7738723112611, 0.0));
    positionAlloc4->Add (Vector(749.1831022766005,776.190602014168, 0.0));
    positionAlloc4->Add (Vector(681.7864984599382,545.0388175112055, 0.0));
    positionAlloc4->Add (Vector(519.1613029795888,318.4794364563386, 0.0));
    positionAlloc4->Add (Vector(712.4973209802171,300.8172938689503, 0.0));
    positionAlloc4->Add (Vector(273.9815980748611,519.841285405588, 0.0));
    positionAlloc4->Add (Vector(193.575412959018,795.484272858346, 0.0));
    positionAlloc4->Add (Vector(513.4843901414911,504.10710897008187, 0.0));
    positionAlloc4->Add (Vector(449.0730373621931,779.5565702446488, 0.0));
    positionAlloc4->Add (Vector(627.3336165225401,593.6970306184796, 0.0));
    positionAlloc4->Add (Vector(285.265194039407,481.3479255329608, 0.0));
    positionAlloc4->Add (Vector(715.6831284897248,578.4006562898077, 0.0));
    positionAlloc4->Add (Vector(706.2635080057732,278.0813683904086, 0.0));
    positionAlloc4->Add (Vector(739.7304900153506,227.80347922697626, 0.0));
    positionAlloc4->Add (Vector(302.7729383468077,517.0628406587497, 0.0));
    positionAlloc4->Add (Vector(702.9529047391161,786.3507684529062, 0.0));
    positionAlloc4->Add (Vector(511.8009678761222,783.5588264326676, 0.0));
    positionAlloc4->Add (Vector(733.1347417482937,526.3372205231483, 0.0));
    positionAlloc4->Add (Vector(500.2936470845572,531.8319629167383, 0.0));
    positionAlloc4->Add (Vector(751.4273230774733,791.8883780269817, 0.0));
    positionAlloc4->Add (Vector(328.7196608198896,596.8480134198344, 0.0));
    positionAlloc4->Add (Vector(554.6984716334841,186.81080215006395, 0.0));
    positionAlloc4->Add (Vector(702.5680724820405,523.1551756272423, 0.0));
    positionAlloc4->Add (Vector(172.52908225078153,183.41365277512926, 0.0));
    positionAlloc4->Add (Vector(664.8907387875419,720.0964969554063, 0.0));
    positionAlloc4->Add (Vector(584.4458664845317,683.7679739657831, 0.0));
    positionAlloc4->Add (Vector(369.9171889543745,411.9489570173406, 0.0));
    positionAlloc4->Add (Vector(358.36515415051997,458.9177786719221, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_20.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_20.flowmon", false, false);
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