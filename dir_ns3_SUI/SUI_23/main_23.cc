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
	  uint16_t numberOfNodes = 767;
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
    positionAlloc->Add (Vector (772.2222222222222,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,150.0, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,850.0, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,538.8888888888889, 0.0));
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
    positionAlloc4->Add (Vector(221.23605758595588,211.15187026489644, 0.0));
    positionAlloc4->Add (Vector(200.8844041431575,179.03590402611667, 0.0));
    positionAlloc4->Add (Vector(612.19094793352,664.4968501850951, 0.0));
    positionAlloc4->Add (Vector(838.6334941982315,750.4369650064859, 0.0));
    positionAlloc4->Add (Vector(816.0795027288489,191.39758841091458, 0.0));
    positionAlloc4->Add (Vector(754.1517990134015,276.354983891078, 0.0));
    positionAlloc4->Add (Vector(608.3337699543754,640.1991024485081, 0.0));
    positionAlloc4->Add (Vector(251.85675195172476,455.4459267742324, 0.0));
    positionAlloc4->Add (Vector(795.7335736466858,589.09364841756, 0.0));
    positionAlloc4->Add (Vector(522.6942348733129,547.276157274793, 0.0));
    positionAlloc4->Add (Vector(823.6662290589273,254.1614749899195, 0.0));
    positionAlloc4->Add (Vector(627.4881688545738,703.2577574055613, 0.0));
    positionAlloc4->Add (Vector(478.04897900848704,207.01795031408028, 0.0));
    positionAlloc4->Add (Vector(477.9046950949656,218.39207737820377, 0.0));
    positionAlloc4->Add (Vector(567.9934396521991,635.1925226355208, 0.0));
    positionAlloc4->Add (Vector(479.3298757525213,277.63013378854987, 0.0));
    positionAlloc4->Add (Vector(506.4601696936777,231.09642681095733, 0.0));
    positionAlloc4->Add (Vector(514.7557872728116,843.3605808151488, 0.0));
    positionAlloc4->Add (Vector(510.9493071805478,798.5298431917606, 0.0));
    positionAlloc4->Add (Vector(606.1148245365073,576.626762174531, 0.0));
    positionAlloc4->Add (Vector(510.1639988018403,228.4834751138892, 0.0));
    positionAlloc4->Add (Vector(247.57382161124548,657.1659870052467, 0.0));
    positionAlloc4->Add (Vector(669.7760137188258,777.2577924428145, 0.0));
    positionAlloc4->Add (Vector(579.0473953659173,362.33476767649097, 0.0));
    positionAlloc4->Add (Vector(181.48679642185564,170.43358834612764, 0.0));
    positionAlloc4->Add (Vector(191.16317372181334,520.6253963800266, 0.0));
    positionAlloc4->Add (Vector(444.300269298316,617.4660003338724, 0.0));
    positionAlloc4->Add (Vector(692.4021880876562,748.0942865669763, 0.0));
    positionAlloc4->Add (Vector(475.501409118685,356.93005214759216, 0.0));
    positionAlloc4->Add (Vector(441.80884835510614,729.828372264922, 0.0));
    positionAlloc4->Add (Vector(422.82092853888247,492.0068014825316, 0.0));
    positionAlloc4->Add (Vector(635.17774168294,434.389491153099, 0.0));
    positionAlloc4->Add (Vector(694.5603119791981,832.4933256880879, 0.0));
    positionAlloc4->Add (Vector(403.9158076846135,849.7594162694371, 0.0));
    positionAlloc4->Add (Vector(375.23939917394785,365.86719480181466, 0.0));
    positionAlloc4->Add (Vector(674.3389285458856,322.1321297942867, 0.0));
    positionAlloc4->Add (Vector(516.2709675163399,612.7586370881983, 0.0));
    positionAlloc4->Add (Vector(357.18824960134566,375.2155614190736, 0.0));
    positionAlloc4->Add (Vector(397.52369336554017,194.5559592342392, 0.0));
    positionAlloc4->Add (Vector(151.86969535736432,706.7648179312517, 0.0));
    positionAlloc4->Add (Vector(849.0577430262143,811.1282669680785, 0.0));
    positionAlloc4->Add (Vector(217.92504495694754,566.5068442628807, 0.0));
    positionAlloc4->Add (Vector(575.5328870080034,548.1602442186306, 0.0));
    positionAlloc4->Add (Vector(382.8981073526618,172.09724748691693, 0.0));
    positionAlloc4->Add (Vector(157.86728786576384,309.03679028139226, 0.0));
    positionAlloc4->Add (Vector(253.18963653909918,303.9898465966778, 0.0));
    positionAlloc4->Add (Vector(151.91012115911101,369.8002629284887, 0.0));
    positionAlloc4->Add (Vector(262.7589728419781,222.56839483756573, 0.0));
    positionAlloc4->Add (Vector(475.7374587124357,770.7669059615812, 0.0));
    positionAlloc4->Add (Vector(568.8652602396475,450.84238410895625, 0.0));
    positionAlloc4->Add (Vector(641.4449806191005,439.81954662875256, 0.0));
    positionAlloc4->Add (Vector(172.14493204905267,798.8814995542377, 0.0));
    positionAlloc4->Add (Vector(782.6242515203738,527.3057662922098, 0.0));
    positionAlloc4->Add (Vector(335.98671459303864,735.2021271660407, 0.0));
    positionAlloc4->Add (Vector(618.4428034690359,235.12848047004073, 0.0));
    positionAlloc4->Add (Vector(644.9345134460918,443.2112586867467, 0.0));
    positionAlloc4->Add (Vector(563.4803024675764,527.2944063643727, 0.0));
    positionAlloc4->Add (Vector(660.5939952779797,540.6999099084587, 0.0));
    positionAlloc4->Add (Vector(847.5878757231167,811.7575823684018, 0.0));
    positionAlloc4->Add (Vector(398.2926306869464,792.7596389876248, 0.0));
    positionAlloc4->Add (Vector(277.6986798423796,165.4656229733424, 0.0));
    positionAlloc4->Add (Vector(838.7497411911326,516.1426362811363, 0.0));
    positionAlloc4->Add (Vector(158.48068907672163,514.3383900733838, 0.0));
    positionAlloc4->Add (Vector(645.4322140777826,464.2520118034579, 0.0));
    positionAlloc4->Add (Vector(399.71192967861384,839.9022409330423, 0.0));
    positionAlloc4->Add (Vector(433.0177686143075,496.1025405802918, 0.0));
    positionAlloc4->Add (Vector(848.2517524368379,692.3004846078338, 0.0));
    positionAlloc4->Add (Vector(567.2919716897833,834.7098129457755, 0.0));
    positionAlloc4->Add (Vector(518.3460737068112,398.6700027072486, 0.0));
    positionAlloc4->Add (Vector(818.7006296946468,424.41893925528393, 0.0));
    positionAlloc4->Add (Vector(194.27782161200466,405.4073265242879, 0.0));
    positionAlloc4->Add (Vector(737.6654390055485,695.9744674521687, 0.0));
    positionAlloc4->Add (Vector(407.53765647124953,161.6313587090124, 0.0));
    positionAlloc4->Add (Vector(675.7452781927021,286.9968700352475, 0.0));
    positionAlloc4->Add (Vector(183.83249785229376,541.6560434061296, 0.0));
    positionAlloc4->Add (Vector(222.29331587233023,332.43855933595296, 0.0));
    positionAlloc4->Add (Vector(354.2216177688173,458.6827719124861, 0.0));
    positionAlloc4->Add (Vector(354.92487763090446,838.8417006697879, 0.0));
    positionAlloc4->Add (Vector(427.03958251692677,536.4536873968734, 0.0));
    positionAlloc4->Add (Vector(169.06597180894985,354.6660789006846, 0.0));
    positionAlloc4->Add (Vector(310.8742943675312,223.45395296008303, 0.0));
    positionAlloc4->Add (Vector(381.98333530921855,610.2884130833372, 0.0));
    positionAlloc4->Add (Vector(533.1301196402046,471.73119711036486, 0.0));
    positionAlloc4->Add (Vector(480.7395177599675,828.7132355182205, 0.0));
    positionAlloc4->Add (Vector(736.9922543786123,842.5485105892634, 0.0));
    positionAlloc4->Add (Vector(645.2869817981539,181.82145791326917, 0.0));
    positionAlloc4->Add (Vector(601.2877028457826,786.7465456146592, 0.0));
    positionAlloc4->Add (Vector(429.98330436099945,504.0886037213413, 0.0));
    positionAlloc4->Add (Vector(272.65925911192096,687.9368605011142, 0.0));
    positionAlloc4->Add (Vector(531.3429169704167,382.06938372204786, 0.0));
    positionAlloc4->Add (Vector(464.8141507489818,514.3990748089166, 0.0));
    positionAlloc4->Add (Vector(350.28463547665103,495.25039051691556, 0.0));
    positionAlloc4->Add (Vector(697.4324120740931,294.73610160359044, 0.0));
    positionAlloc4->Add (Vector(742.0076630384855,182.86782746268105, 0.0));
    positionAlloc4->Add (Vector(695.0056178946512,425.4267031181954, 0.0));
    positionAlloc4->Add (Vector(475.86400371990464,755.4997288242372, 0.0));
    positionAlloc4->Add (Vector(780.8783579720522,365.07848679016774, 0.0));
    positionAlloc4->Add (Vector(692.3055157202042,676.5805485316782, 0.0));
    positionAlloc4->Add (Vector(788.4240569233118,404.24206134365846, 0.0));
    positionAlloc4->Add (Vector(187.72079389901478,837.4632222670016, 0.0));
    positionAlloc4->Add (Vector(392.5498397117934,752.589122168233, 0.0));
    positionAlloc4->Add (Vector(512.1075471230513,777.2396812485892, 0.0));
    positionAlloc4->Add (Vector(289.6002008127697,829.7517987860753, 0.0));
    positionAlloc4->Add (Vector(349.22504827892936,415.21709858136876, 0.0));
    positionAlloc4->Add (Vector(800.7882490658119,668.4159396333308, 0.0));
    positionAlloc4->Add (Vector(606.1157398796556,357.8796701460992, 0.0));
    positionAlloc4->Add (Vector(313.5830249466321,823.8564981591992, 0.0));
    positionAlloc4->Add (Vector(276.0959189925444,389.4791552243655, 0.0));
    positionAlloc4->Add (Vector(612.5819089689365,159.0558245927492, 0.0));
    positionAlloc4->Add (Vector(643.7346517679108,723.7981359273275, 0.0));
    positionAlloc4->Add (Vector(692.8953869272365,192.61086570398064, 0.0));
    positionAlloc4->Add (Vector(206.97444061257076,476.3792379704465, 0.0));
    positionAlloc4->Add (Vector(395.91974457529903,773.6420963954321, 0.0));
    positionAlloc4->Add (Vector(618.0641628681813,537.9503007232179, 0.0));
    positionAlloc4->Add (Vector(484.0679123662519,573.2373405415776, 0.0));
    positionAlloc4->Add (Vector(299.0758150010039,402.06306706948556, 0.0));
    positionAlloc4->Add (Vector(809.5374371850108,623.63955996441, 0.0));
    positionAlloc4->Add (Vector(191.22742779940063,738.2846588945999, 0.0));
    positionAlloc4->Add (Vector(262.3936644657234,686.6322248905202, 0.0));
    positionAlloc4->Add (Vector(261.1964533117592,389.17182796589805, 0.0));
    positionAlloc4->Add (Vector(828.1162422263225,676.0945422438365, 0.0));
    positionAlloc4->Add (Vector(492.61162686187345,156.94624501416155, 0.0));
    positionAlloc4->Add (Vector(680.9126900053029,210.945292319077, 0.0));
    positionAlloc4->Add (Vector(602.8486885759798,303.1521220653084, 0.0));
    positionAlloc4->Add (Vector(207.66687641489483,619.4464224285118, 0.0));
    positionAlloc4->Add (Vector(813.4432220272298,654.7455751120211, 0.0));
    positionAlloc4->Add (Vector(743.0854887334747,168.36715214763038, 0.0));
    positionAlloc4->Add (Vector(303.3663758900808,838.7814907276617, 0.0));
    positionAlloc4->Add (Vector(316.08094134428325,497.99156243894896, 0.0));
    positionAlloc4->Add (Vector(829.1077738142052,568.1036996928443, 0.0));
    positionAlloc4->Add (Vector(766.341027852858,317.9118413252361, 0.0));
    positionAlloc4->Add (Vector(156.07684645841607,260.81667196873707, 0.0));
    positionAlloc4->Add (Vector(339.58522638696604,196.79305387237324, 0.0));
    positionAlloc4->Add (Vector(663.85314098554,223.7754317656043, 0.0));
    positionAlloc4->Add (Vector(741.3800374682072,661.9269236721066, 0.0));
    positionAlloc4->Add (Vector(516.3293155991304,788.7904403150619, 0.0));
    positionAlloc4->Add (Vector(730.0632631180937,757.5858407078233, 0.0));
    positionAlloc4->Add (Vector(593.5427060862794,281.23553158234404, 0.0));
    positionAlloc4->Add (Vector(742.7370396686645,549.7198738753168, 0.0));
    positionAlloc4->Add (Vector(216.87775184811738,285.80127009064915, 0.0));
    positionAlloc4->Add (Vector(161.0184277817026,483.9042884504104, 0.0));
    positionAlloc4->Add (Vector(398.6516298920917,182.52303405533675, 0.0));
    positionAlloc4->Add (Vector(705.1089074155557,370.8774895108983, 0.0));
    positionAlloc4->Add (Vector(211.43303213522017,753.0347012442244, 0.0));
    positionAlloc4->Add (Vector(640.1307047083524,790.7287158524653, 0.0));
    positionAlloc4->Add (Vector(369.0427894586536,428.354900826109, 0.0));
    positionAlloc4->Add (Vector(492.75910051018883,720.2246790624446, 0.0));
    positionAlloc4->Add (Vector(184.33492178892143,736.3196972540392, 0.0));
    positionAlloc4->Add (Vector(659.307714926481,202.76897825062863, 0.0));
    positionAlloc4->Add (Vector(819.4937355244097,757.0987201223784, 0.0));
    positionAlloc4->Add (Vector(833.3687230556026,690.6237853616268, 0.0));
    positionAlloc4->Add (Vector(745.2506736676756,836.4579340937771, 0.0));
    positionAlloc4->Add (Vector(608.6783815873696,776.8329240266756, 0.0));
    positionAlloc4->Add (Vector(576.4668489003457,537.8119133753332, 0.0));
    positionAlloc4->Add (Vector(572.400900946337,189.55765864716386, 0.0));
    positionAlloc4->Add (Vector(558.5876079623908,278.43824305201616, 0.0));
    positionAlloc4->Add (Vector(591.3618896497986,603.9383393090894, 0.0));
    positionAlloc4->Add (Vector(243.27955408310814,162.2869022908185, 0.0));
    positionAlloc4->Add (Vector(278.78494832763613,543.806737635264, 0.0));
    positionAlloc4->Add (Vector(801.0133411736471,587.9494479863145, 0.0));
    positionAlloc4->Add (Vector(551.1494609618519,213.43710570415044, 0.0));
    positionAlloc4->Add (Vector(479.2774469907806,450.9947780170147, 0.0));
    positionAlloc4->Add (Vector(835.9581670007121,576.7341986592166, 0.0));
    positionAlloc4->Add (Vector(606.3965984645006,187.4487456620795, 0.0));
    positionAlloc4->Add (Vector(271.941056073538,163.4473731514604, 0.0));
    positionAlloc4->Add (Vector(482.09623600215906,754.3143168845636, 0.0));
    positionAlloc4->Add (Vector(575.2372722143077,299.3603351484036, 0.0));
    positionAlloc4->Add (Vector(247.0347443572428,582.2054659230485, 0.0));
    positionAlloc4->Add (Vector(447.0934114173491,230.3992485346236, 0.0));
    positionAlloc4->Add (Vector(673.6025542782083,272.63849796671286, 0.0));
    positionAlloc4->Add (Vector(746.3910527293394,416.756539449614, 0.0));
    positionAlloc4->Add (Vector(236.0214526229723,375.3032994729343, 0.0));
    positionAlloc4->Add (Vector(458.19541958854967,346.5427850821702, 0.0));
    positionAlloc4->Add (Vector(548.2025581528243,330.225732772348, 0.0));
    positionAlloc4->Add (Vector(542.4186494954452,398.7999713555645, 0.0));
    positionAlloc4->Add (Vector(530.6708547898662,622.6137003683668, 0.0));
    positionAlloc4->Add (Vector(645.7937779600652,722.5214194901245, 0.0));
    positionAlloc4->Add (Vector(786.9587373746582,742.5088459383442, 0.0));
    positionAlloc4->Add (Vector(752.0349157873519,234.25533324616077, 0.0));
    positionAlloc4->Add (Vector(658.3419513473702,492.1476923793025, 0.0));
    positionAlloc4->Add (Vector(475.1333020021368,722.2146601334105, 0.0));
    positionAlloc4->Add (Vector(530.9712346885302,177.33932517150737, 0.0));
    positionAlloc4->Add (Vector(227.42402635846514,756.7479509809599, 0.0));
    positionAlloc4->Add (Vector(273.26753618722137,793.1351805859583, 0.0));
    positionAlloc4->Add (Vector(426.13420868748307,261.643443681847, 0.0));
    positionAlloc4->Add (Vector(183.5286150271974,709.9080231699809, 0.0));
    positionAlloc4->Add (Vector(571.7172842145164,745.4386513695292, 0.0));
    positionAlloc4->Add (Vector(553.8801583437566,205.99017055848765, 0.0));
    positionAlloc4->Add (Vector(288.9700832697899,227.2256078153547, 0.0));
    positionAlloc4->Add (Vector(172.09168434293355,833.909981719926, 0.0));
    positionAlloc4->Add (Vector(236.27239057520342,754.1055170494407, 0.0));
    positionAlloc4->Add (Vector(289.06244461992264,248.43144981759872, 0.0));
    positionAlloc4->Add (Vector(570.2482798563573,487.5776169924352, 0.0));
    positionAlloc4->Add (Vector(269.6326566235696,410.58104932632654, 0.0));
    positionAlloc4->Add (Vector(191.5820817235397,731.0511687439041, 0.0));
    positionAlloc4->Add (Vector(616.3283329954131,405.72369421539037, 0.0));
    positionAlloc4->Add (Vector(769.3048909289349,634.0354275691054, 0.0));
    positionAlloc4->Add (Vector(732.4618223765815,333.61828943218313, 0.0));
    positionAlloc4->Add (Vector(728.6781190898195,151.7468175205804, 0.0));
    positionAlloc4->Add (Vector(368.86378034213726,241.1145157143681, 0.0));
    positionAlloc4->Add (Vector(275.6807095559168,783.4276280311884, 0.0));
    positionAlloc4->Add (Vector(333.2202205281717,797.3380805051312, 0.0));
    positionAlloc4->Add (Vector(666.7023991271728,635.246844587266, 0.0));
    positionAlloc4->Add (Vector(658.105297578531,816.3186640327914, 0.0));
    positionAlloc4->Add (Vector(618.8236823814768,695.556085023513, 0.0));
    positionAlloc4->Add (Vector(668.6038409732414,227.06671368332047, 0.0));
    positionAlloc4->Add (Vector(582.5991099478925,186.94220781949727, 0.0));
    positionAlloc4->Add (Vector(776.234188347647,805.7404487809599, 0.0));
    positionAlloc4->Add (Vector(211.14359193700045,652.4095070782218, 0.0));
    positionAlloc4->Add (Vector(464.3773086056767,288.5648481342651, 0.0));
    positionAlloc4->Add (Vector(164.6894872293993,341.06840734832986, 0.0));
    positionAlloc4->Add (Vector(372.82764913938706,329.6834717062442, 0.0));
    positionAlloc4->Add (Vector(536.3045450841493,504.73047261663834, 0.0));
    positionAlloc4->Add (Vector(604.046158396802,825.4052195837098, 0.0));
    positionAlloc4->Add (Vector(167.38408523601,196.18663218134924, 0.0));
    positionAlloc4->Add (Vector(660.2092051680143,449.009228804772, 0.0));
    positionAlloc4->Add (Vector(162.74982165238455,406.59320391004314, 0.0));
    positionAlloc4->Add (Vector(332.55396986711844,788.6636296115028, 0.0));
    positionAlloc4->Add (Vector(686.3672312224958,552.559994124661, 0.0));
    positionAlloc4->Add (Vector(654.1717805424873,611.6639682689685, 0.0));
    positionAlloc4->Add (Vector(613.8203520161994,672.6560029863165, 0.0));
    positionAlloc4->Add (Vector(243.59187747676896,594.9396390849865, 0.0));
    positionAlloc4->Add (Vector(627.5237835896536,172.61133904074393, 0.0));
    positionAlloc4->Add (Vector(222.91950866342683,732.0754005565475, 0.0));
    positionAlloc4->Add (Vector(766.4364611825156,660.1330208326685, 0.0));
    positionAlloc4->Add (Vector(758.8768062209219,413.69364169114584, 0.0));
    positionAlloc4->Add (Vector(783.2714062722157,213.3404042946869, 0.0));
    positionAlloc4->Add (Vector(497.4623655809979,360.31176568501576, 0.0));
    positionAlloc4->Add (Vector(640.8953042907948,512.8507864113535, 0.0));
    positionAlloc4->Add (Vector(482.83866432830075,198.32409175836656, 0.0));
    positionAlloc4->Add (Vector(367.52962574249045,283.0658627816571, 0.0));
    positionAlloc4->Add (Vector(531.1834975987341,713.4850233141805, 0.0));
    positionAlloc4->Add (Vector(377.8625871418857,337.2945361570552, 0.0));
    positionAlloc4->Add (Vector(488.34546437221826,401.4846423079183, 0.0));
    positionAlloc4->Add (Vector(794.2001612454925,766.0438887387078, 0.0));
    positionAlloc4->Add (Vector(540.8021225061918,437.2195393500565, 0.0));
    positionAlloc4->Add (Vector(359.5355200325557,297.7161534371882, 0.0));
    positionAlloc4->Add (Vector(262.14332807688174,694.988821938077, 0.0));
    positionAlloc4->Add (Vector(489.53686766683086,575.2001326448055, 0.0));
    positionAlloc4->Add (Vector(703.3893671338461,290.88452158262476, 0.0));
    positionAlloc4->Add (Vector(235.5377332804666,529.9676269536765, 0.0));
    positionAlloc4->Add (Vector(163.60287068594852,847.964412327174, 0.0));
    positionAlloc4->Add (Vector(162.84491351786542,402.74178879473664, 0.0));
    positionAlloc4->Add (Vector(662.9577037838071,665.4918382435144, 0.0));
    positionAlloc4->Add (Vector(780.6159292917268,377.66676247469724, 0.0));
    positionAlloc4->Add (Vector(803.6233446748123,392.79031319583163, 0.0));
    positionAlloc4->Add (Vector(749.9825654367312,663.2929870837838, 0.0));
    positionAlloc4->Add (Vector(353.8935442715118,830.8336242560811, 0.0));
    positionAlloc4->Add (Vector(216.02686765624054,520.0590825237994, 0.0));
    positionAlloc4->Add (Vector(521.8076116294769,661.414003167632, 0.0));
    positionAlloc4->Add (Vector(743.5738139024398,656.0826354560343, 0.0));
    positionAlloc4->Add (Vector(591.2845629927315,160.05715142837272, 0.0));
    positionAlloc4->Add (Vector(733.006208682452,256.7979687128689, 0.0));
    positionAlloc4->Add (Vector(732.0882817271043,736.6645586948661, 0.0));
    positionAlloc4->Add (Vector(670.449033381503,180.14719841426802, 0.0));
    positionAlloc4->Add (Vector(514.1255813289645,302.0074848846312, 0.0));
    positionAlloc4->Add (Vector(741.1998033679971,153.05945135020858, 0.0));
    positionAlloc4->Add (Vector(536.3265976494021,421.86953125271845, 0.0));
    positionAlloc4->Add (Vector(208.55016340828692,545.7046866625283, 0.0));
    positionAlloc4->Add (Vector(268.2679413009757,675.9619857506075, 0.0));
    positionAlloc4->Add (Vector(602.3843639285092,796.3024877654005, 0.0));
    positionAlloc4->Add (Vector(297.9891110431329,671.6540886840627, 0.0));
    positionAlloc4->Add (Vector(539.3190476348275,597.3805898728622, 0.0));
    positionAlloc4->Add (Vector(197.74436431990785,204.80808689083332, 0.0));
    positionAlloc4->Add (Vector(520.1187080778454,738.0883119385755, 0.0));
    positionAlloc4->Add (Vector(622.1228984110614,356.11982492788195, 0.0));
    positionAlloc4->Add (Vector(533.5624128494267,691.4620765899059, 0.0));
    positionAlloc4->Add (Vector(468.83612067484927,385.9291819417015, 0.0));
    positionAlloc4->Add (Vector(390.78808324276787,353.17585413363474, 0.0));
    positionAlloc4->Add (Vector(540.3703332917967,381.766004400249, 0.0));
    positionAlloc4->Add (Vector(193.88694722046887,762.4649940431962, 0.0));
    positionAlloc4->Add (Vector(665.7051505384812,434.59305239656146, 0.0));
    positionAlloc4->Add (Vector(329.19693436860393,254.3059803881558, 0.0));
    positionAlloc4->Add (Vector(806.3576498905848,298.3065778126117, 0.0));
    positionAlloc4->Add (Vector(285.4007141759696,627.0121132838747, 0.0));
    positionAlloc4->Add (Vector(736.91107032055,540.2491317759632, 0.0));
    positionAlloc4->Add (Vector(730.6448455395162,848.649877514566, 0.0));
    positionAlloc4->Add (Vector(361.71876588170744,597.8904543945276, 0.0));
    positionAlloc4->Add (Vector(536.5300174194537,560.0322508596239, 0.0));
    positionAlloc4->Add (Vector(283.72483443205385,592.7736381631078, 0.0));
    positionAlloc4->Add (Vector(187.82771464353164,522.874408613875, 0.0));
    positionAlloc4->Add (Vector(630.2531749835189,165.62601102840233, 0.0));
    positionAlloc4->Add (Vector(500.749224495094,611.2655929038127, 0.0));
    positionAlloc4->Add (Vector(312.8747237754596,613.6208671094887, 0.0));
    positionAlloc4->Add (Vector(433.0132606438596,311.29446574963725, 0.0));
    positionAlloc4->Add (Vector(508.4005996299118,783.9757447638865, 0.0));
    positionAlloc4->Add (Vector(828.2111970778235,782.1978183319264, 0.0));
    positionAlloc4->Add (Vector(638.5717977615462,521.5994221798137, 0.0));
    positionAlloc4->Add (Vector(401.04580751220277,825.6593616539835, 0.0));
    positionAlloc4->Add (Vector(244.5492494141538,370.4144081424248, 0.0));
    positionAlloc4->Add (Vector(681.7065250105508,391.43335649587164, 0.0));
    positionAlloc4->Add (Vector(312.4972666557329,266.98740306315955, 0.0));
    positionAlloc4->Add (Vector(579.0289058870626,678.0989204119496, 0.0));
    positionAlloc4->Add (Vector(842.555249786304,577.4494466422443, 0.0));
    positionAlloc4->Add (Vector(536.687326807606,762.4791431345035, 0.0));
    positionAlloc4->Add (Vector(813.5039401444726,723.578491293879, 0.0));
    positionAlloc4->Add (Vector(234.17624144060375,237.30663903658058, 0.0));
    positionAlloc4->Add (Vector(220.78738738154124,709.7751622923698, 0.0));
    positionAlloc4->Add (Vector(723.4995285194154,708.8191936343428, 0.0));
    positionAlloc4->Add (Vector(502.62058676231004,720.1590157314823, 0.0));
    positionAlloc4->Add (Vector(453.6097958422268,437.9385532905152, 0.0));
    positionAlloc4->Add (Vector(381.18077385819896,440.04367590213866, 0.0));
    positionAlloc4->Add (Vector(714.7954207971358,342.95096116959417, 0.0));
    positionAlloc4->Add (Vector(380.93067210434515,282.71225176840187, 0.0));
    positionAlloc4->Add (Vector(819.7377520503952,367.25436311885557, 0.0));
    positionAlloc4->Add (Vector(216.68344593781916,329.88856374123776, 0.0));
    positionAlloc4->Add (Vector(670.5943133883509,426.5081690095879, 0.0));
    positionAlloc4->Add (Vector(356.9610002901687,788.4476419218228, 0.0));
    positionAlloc4->Add (Vector(574.3862444726601,742.0338522824633, 0.0));
    positionAlloc4->Add (Vector(343.3527179576649,648.2051142090663, 0.0));
    positionAlloc4->Add (Vector(295.27109478896153,295.83300024197524, 0.0));
    positionAlloc4->Add (Vector(534.1064418443516,566.4543904481611, 0.0));
    positionAlloc4->Add (Vector(606.3531416330254,168.25830067930528, 0.0));
    positionAlloc4->Add (Vector(593.6517392351043,563.3060965452404, 0.0));
    positionAlloc4->Add (Vector(498.94315892408997,281.1230337913372, 0.0));
    positionAlloc4->Add (Vector(831.000410639319,526.2417069839341, 0.0));
    positionAlloc4->Add (Vector(211.4999095069419,648.7787388805306, 0.0));
    positionAlloc4->Add (Vector(425.98972434899645,639.2672851638156, 0.0));
    positionAlloc4->Add (Vector(802.3895125107323,763.8111304456758, 0.0));
    positionAlloc4->Add (Vector(237.0132457704429,410.908693383866, 0.0));
    positionAlloc4->Add (Vector(507.7730145599992,835.4635156811371, 0.0));
    positionAlloc4->Add (Vector(401.914608633684,341.9453235421713, 0.0));
    positionAlloc4->Add (Vector(323.2934468207359,214.64952076748347, 0.0));
    positionAlloc4->Add (Vector(239.02403873431413,526.5717421508693, 0.0));
    positionAlloc4->Add (Vector(702.2453526898722,154.21542825788447, 0.0));
    positionAlloc4->Add (Vector(279.6025792176161,264.36701952518536, 0.0));
    positionAlloc4->Add (Vector(808.7240126526814,276.6118267549881, 0.0));
    positionAlloc4->Add (Vector(499.24686885150106,814.3033506709414, 0.0));
    positionAlloc4->Add (Vector(157.88861233257882,396.6424882796066, 0.0));
    positionAlloc4->Add (Vector(734.6728642198511,423.276061148583, 0.0));
    positionAlloc4->Add (Vector(363.57849777488644,429.1750719852806, 0.0));
    positionAlloc4->Add (Vector(632.8428191972486,157.50596202606957, 0.0));
    positionAlloc4->Add (Vector(657.790941550516,291.582614378809, 0.0));
    positionAlloc4->Add (Vector(668.2552005092663,709.5276170810338, 0.0));
    positionAlloc4->Add (Vector(456.3967524917174,216.13871262527454, 0.0));
    positionAlloc4->Add (Vector(226.8445265401744,603.263291109332, 0.0));
    positionAlloc4->Add (Vector(465.57358802107314,240.86389106611557, 0.0));
    positionAlloc4->Add (Vector(325.39322959758636,817.6837803124708, 0.0));
    positionAlloc4->Add (Vector(745.7724121756019,624.8283432418842, 0.0));
    positionAlloc4->Add (Vector(361.51910735083857,675.5517235178005, 0.0));
    positionAlloc4->Add (Vector(783.9092092181827,760.1682364886589, 0.0));
    positionAlloc4->Add (Vector(627.8071704996903,838.9236500264175, 0.0));
    positionAlloc4->Add (Vector(577.3761991147126,227.61642601893953, 0.0));
    positionAlloc4->Add (Vector(644.2016866970339,716.961506407032, 0.0));
    positionAlloc4->Add (Vector(435.7984592562379,557.9030914931154, 0.0));
    positionAlloc4->Add (Vector(270.77219048097606,169.7332208475754, 0.0));
    positionAlloc4->Add (Vector(639.7175169724145,655.2391941224666, 0.0));
    positionAlloc4->Add (Vector(299.2644058118716,213.36392856444925, 0.0));
    positionAlloc4->Add (Vector(754.1937737662129,559.1090248984451, 0.0));
    positionAlloc4->Add (Vector(165.1484829822093,565.8088906356588, 0.0));
    positionAlloc4->Add (Vector(655.8552342630887,210.76320160024466, 0.0));
    positionAlloc4->Add (Vector(760.22246403479,680.2845500007226, 0.0));
    positionAlloc4->Add (Vector(716.0960513745208,283.22485620921236, 0.0));
    positionAlloc4->Add (Vector(481.2155337638501,543.1853623960324, 0.0));
    positionAlloc4->Add (Vector(367.370698539404,402.3215709747261, 0.0));
    positionAlloc4->Add (Vector(360.68286078728636,803.5406108231448, 0.0));
    positionAlloc4->Add (Vector(505.7294183008092,158.19183876762483, 0.0));
    positionAlloc4->Add (Vector(364.46802567799307,262.63369877445024, 0.0));
    positionAlloc4->Add (Vector(194.68896529237048,352.88482366574823, 0.0));
    positionAlloc4->Add (Vector(257.842555611374,737.9327681895959, 0.0));
    positionAlloc4->Add (Vector(438.52113276672816,365.04587077623336, 0.0));
    positionAlloc4->Add (Vector(184.49512502036276,642.6467035242654, 0.0));
    positionAlloc4->Add (Vector(842.3517834574931,657.8956381316075, 0.0));
    positionAlloc4->Add (Vector(236.73562141786266,269.6093036869328, 0.0));
    positionAlloc4->Add (Vector(811.1942845075769,552.2173747930307, 0.0));
    positionAlloc4->Add (Vector(756.2745929131795,560.3052269141185, 0.0));
    positionAlloc4->Add (Vector(470.1239693474977,300.9642118721897, 0.0));
    positionAlloc4->Add (Vector(585.7163121432417,377.26821452542094, 0.0));
    positionAlloc4->Add (Vector(670.0853941422698,245.44765657753027, 0.0));
    positionAlloc4->Add (Vector(195.66286288724078,204.56504860164313, 0.0));
    positionAlloc4->Add (Vector(847.7750149273278,173.55067578063347, 0.0));
    positionAlloc4->Add (Vector(543.765990523206,640.0906937191669, 0.0));
    positionAlloc4->Add (Vector(306.6407983596446,524.591140223785, 0.0));
    positionAlloc4->Add (Vector(630.4007338319299,626.7766490509816, 0.0));
    positionAlloc4->Add (Vector(642.1200315765622,799.9280566766098, 0.0));
    positionAlloc4->Add (Vector(832.0510197311221,339.76646015591814, 0.0));
    positionAlloc4->Add (Vector(425.4598088645345,691.729068276776, 0.0));
    positionAlloc4->Add (Vector(572.4538524147338,630.8209830146789, 0.0));
    positionAlloc4->Add (Vector(248.73564474916557,552.3810187989834, 0.0));
    positionAlloc4->Add (Vector(698.2849391528432,205.79331761182135, 0.0));
    positionAlloc4->Add (Vector(265.03842085978744,164.36575819876072, 0.0));
    positionAlloc4->Add (Vector(830.528292667015,580.6857575796811, 0.0));
    positionAlloc4->Add (Vector(641.2430393412872,273.26143078844154, 0.0));
    positionAlloc4->Add (Vector(458.2547518251586,603.4396918898033, 0.0));
    positionAlloc4->Add (Vector(813.8690983908317,159.62068428367004, 0.0));
    positionAlloc4->Add (Vector(298.5249185762355,666.8975708700183, 0.0));
    positionAlloc4->Add (Vector(386.9904687884856,705.4192453010271, 0.0));
    positionAlloc4->Add (Vector(732.7446590393009,742.5636723140657, 0.0));
    positionAlloc4->Add (Vector(509.97297999800105,708.2251432608363, 0.0));
    positionAlloc4->Add (Vector(437.80964656871464,314.0322543762056, 0.0));
    positionAlloc4->Add (Vector(347.24984762319104,587.971248672131, 0.0));
    positionAlloc4->Add (Vector(446.0346422977435,711.640799287367, 0.0));
    positionAlloc4->Add (Vector(154.5146100752906,255.45814206244364, 0.0));
    positionAlloc4->Add (Vector(599.8436414400983,201.55530422373934, 0.0));
    positionAlloc4->Add (Vector(788.8846614110911,160.91130934275475, 0.0));
    positionAlloc4->Add (Vector(263.2800628878034,524.6675358403133, 0.0));
    positionAlloc4->Add (Vector(614.5542926528269,515.1467376134563, 0.0));
    positionAlloc4->Add (Vector(196.23187611420485,244.56618961671683, 0.0));
    positionAlloc4->Add (Vector(443.0225321687778,558.6860823516272, 0.0));
    positionAlloc4->Add (Vector(630.7314869577253,190.93548453113797, 0.0));
    positionAlloc4->Add (Vector(484.75548041818246,596.5109831514335, 0.0));
    positionAlloc4->Add (Vector(697.7539451645515,190.2687486002579, 0.0));
    positionAlloc4->Add (Vector(788.1011980963885,156.99564240099954, 0.0));
    positionAlloc4->Add (Vector(290.0340432075426,478.706492346278, 0.0));
    positionAlloc4->Add (Vector(619.8596544996691,286.13756151132225, 0.0));
    positionAlloc4->Add (Vector(476.15294831076466,616.4635669346827, 0.0));
    positionAlloc4->Add (Vector(845.8391697376795,759.0157690324202, 0.0));
    positionAlloc4->Add (Vector(380.75254772340736,200.76084743629374, 0.0));
    positionAlloc4->Add (Vector(363.80625160640005,797.3638354341529, 0.0));
    positionAlloc4->Add (Vector(789.1761508436846,703.8993802477806, 0.0));
    positionAlloc4->Add (Vector(192.77495492825312,455.7156744562487, 0.0));
    positionAlloc4->Add (Vector(321.7874144612231,608.4834261434182, 0.0));
    positionAlloc4->Add (Vector(320.3682954824262,163.6704962078433, 0.0));
    positionAlloc4->Add (Vector(179.48941586709665,200.8275968172561, 0.0));
    positionAlloc4->Add (Vector(243.00322389926748,327.12208047249794, 0.0));
    positionAlloc4->Add (Vector(371.08204660714,578.1325179279141, 0.0));
    positionAlloc4->Add (Vector(595.5047412014363,396.27254079409875, 0.0));
    positionAlloc4->Add (Vector(568.9591556267426,556.1314343131726, 0.0));
    positionAlloc4->Add (Vector(345.1096303347658,415.8531830345687, 0.0));
    positionAlloc4->Add (Vector(456.6704821537342,479.2894332313669, 0.0));
    positionAlloc4->Add (Vector(742.3624616874071,373.87420076059254, 0.0));
    positionAlloc4->Add (Vector(822.0749274179934,536.5302762543153, 0.0));
    positionAlloc4->Add (Vector(553.038007086599,535.673554993107, 0.0));
    positionAlloc4->Add (Vector(540.5134211081953,476.51296283122383, 0.0));
    positionAlloc4->Add (Vector(526.5402901384172,302.5988130719042, 0.0));
    positionAlloc4->Add (Vector(819.6728759556435,166.36430277681092, 0.0));
    positionAlloc4->Add (Vector(420.51948980040163,677.2937089919125, 0.0));
    positionAlloc4->Add (Vector(758.668258322131,681.9883585091258, 0.0));
    positionAlloc4->Add (Vector(840.374899530319,849.9306839064621, 0.0));
    positionAlloc4->Add (Vector(644.8777259388589,554.9296461096069, 0.0));
    positionAlloc4->Add (Vector(684.2329095036463,318.95944660280276, 0.0));
    positionAlloc4->Add (Vector(505.800507807447,292.5923041596034, 0.0));
    positionAlloc4->Add (Vector(337.9556293775429,283.7367405216407, 0.0));
    positionAlloc4->Add (Vector(302.16957299409205,160.3623959729249, 0.0));
    positionAlloc4->Add (Vector(366.318768834302,740.8551140675644, 0.0));
    positionAlloc4->Add (Vector(330.47798715515785,632.896140122411, 0.0));
    positionAlloc4->Add (Vector(427.2502977416831,562.0253099856989, 0.0));
    positionAlloc4->Add (Vector(742.3598261807878,722.7666197343509, 0.0));
    positionAlloc4->Add (Vector(516.9454032633892,305.9306794829746, 0.0));
    positionAlloc4->Add (Vector(388.7308946331464,649.2181341001981, 0.0));
    positionAlloc4->Add (Vector(236.94230577464168,810.6632962942176, 0.0));
    positionAlloc4->Add (Vector(588.7110606821498,629.6208832110831, 0.0));
    positionAlloc4->Add (Vector(287.48986077029076,360.018483825427, 0.0));
    positionAlloc4->Add (Vector(727.8866655923804,200.35320603695607, 0.0));
    positionAlloc4->Add (Vector(598.8071902021095,429.29504166917303, 0.0));
    positionAlloc4->Add (Vector(180.15895130980502,259.585472449499, 0.0));
    positionAlloc4->Add (Vector(812.9618273268865,202.33049270524623, 0.0));
    positionAlloc4->Add (Vector(284.11577753209656,536.0739747524524, 0.0));
    positionAlloc4->Add (Vector(702.2708345072423,201.45579376187266, 0.0));
    positionAlloc4->Add (Vector(690.7513759792946,486.8010518571828, 0.0));
    positionAlloc4->Add (Vector(467.86548122303276,450.1255189088181, 0.0));
    positionAlloc4->Add (Vector(726.501994145915,369.562636422968, 0.0));
    positionAlloc4->Add (Vector(468.7098934231404,255.6754951912064, 0.0));
    positionAlloc4->Add (Vector(683.8947062562986,173.34058850127087, 0.0));
    positionAlloc4->Add (Vector(638.462159433695,173.86850316672724, 0.0));
    positionAlloc4->Add (Vector(618.4382145651605,494.9980372334558, 0.0));
    positionAlloc4->Add (Vector(727.6504964332012,533.1940363111391, 0.0));
    positionAlloc4->Add (Vector(789.1231686376535,204.95785274752075, 0.0));
    positionAlloc4->Add (Vector(248.75524506356209,688.5271527325765, 0.0));
    positionAlloc4->Add (Vector(794.1838185964677,713.8688036835233, 0.0));
    positionAlloc4->Add (Vector(350.27841625950407,331.24233255196765, 0.0));
    positionAlloc4->Add (Vector(221.48658861414935,779.9765638573335, 0.0));
    positionAlloc4->Add (Vector(355.82081670857303,410.1235695105994, 0.0));
    positionAlloc4->Add (Vector(325.9232780384208,786.7738453335592, 0.0));
    positionAlloc4->Add (Vector(826.980932159953,634.473778469742, 0.0));
    positionAlloc4->Add (Vector(588.7625411718311,503.06074519740656, 0.0));
    positionAlloc4->Add (Vector(651.171232897578,488.0125853808828, 0.0));
    positionAlloc4->Add (Vector(798.4897150085884,385.7549521080691, 0.0));
    positionAlloc4->Add (Vector(812.149502907253,505.23204826438524, 0.0));
    positionAlloc4->Add (Vector(748.3072158829049,254.89844440970606, 0.0));
    positionAlloc4->Add (Vector(371.58555963433673,383.642354588696, 0.0));
    positionAlloc4->Add (Vector(636.9359620007733,811.3110898282212, 0.0));
    positionAlloc4->Add (Vector(514.0438026292294,829.434591980792, 0.0));
    positionAlloc4->Add (Vector(706.4256247945483,590.3403189917436, 0.0));
    positionAlloc4->Add (Vector(832.7327010746052,841.8020125477342, 0.0));
    positionAlloc4->Add (Vector(300.1457120725678,352.24528227109283, 0.0));
    positionAlloc4->Add (Vector(845.1056253989151,278.21595814572095, 0.0));
    positionAlloc4->Add (Vector(664.7417252699415,340.24514194581093, 0.0));
    positionAlloc4->Add (Vector(721.9278450264254,616.099580840514, 0.0));
    positionAlloc4->Add (Vector(720.7355342437694,676.8386602039977, 0.0));
    positionAlloc4->Add (Vector(202.7240297811879,598.6750771981324, 0.0));
    positionAlloc4->Add (Vector(401.63841627904884,258.40199826510724, 0.0));
    positionAlloc4->Add (Vector(163.3225775149169,223.55991662071, 0.0));
    positionAlloc4->Add (Vector(593.67945086076,652.0980803524747, 0.0));
    positionAlloc4->Add (Vector(590.3855535372996,558.9362447341932, 0.0));
    positionAlloc4->Add (Vector(183.94815822575674,348.74280364287233, 0.0));
    positionAlloc4->Add (Vector(433.60250685066273,176.8150553940157, 0.0));
    positionAlloc4->Add (Vector(795.3493694412588,771.239630695135, 0.0));
    positionAlloc4->Add (Vector(315.78802580858667,375.625282589191, 0.0));
    positionAlloc4->Add (Vector(809.5404630335827,637.6665273645633, 0.0));
    positionAlloc4->Add (Vector(540.6759088515669,306.3522329139682, 0.0));
    positionAlloc4->Add (Vector(632.7719407719901,653.808084920496, 0.0));
    positionAlloc4->Add (Vector(497.3819846292871,580.2526427380327, 0.0));
    positionAlloc4->Add (Vector(665.7324163124699,161.6977656164084, 0.0));
    positionAlloc4->Add (Vector(646.816034767865,844.9161131328408, 0.0));
    positionAlloc4->Add (Vector(453.5340597189361,228.47710793189373, 0.0));
    positionAlloc4->Add (Vector(699.2479401282527,606.9798787554014, 0.0));
    positionAlloc4->Add (Vector(752.4166573011659,804.8958921415538, 0.0));
    positionAlloc4->Add (Vector(533.3677641586424,647.8684142620577, 0.0));
    positionAlloc4->Add (Vector(731.333920080637,253.23878231608722, 0.0));
    positionAlloc4->Add (Vector(305.42513445932224,502.57430139474366, 0.0));
    positionAlloc4->Add (Vector(719.9252276461664,224.11127890076472, 0.0));
    positionAlloc4->Add (Vector(321.8621733419218,320.89808308649935, 0.0));
    positionAlloc4->Add (Vector(273.1591427347835,792.6332989170139, 0.0));
    positionAlloc4->Add (Vector(732.9299391097263,620.1658848625866, 0.0));
    positionAlloc4->Add (Vector(768.9671215592591,430.6042256404664, 0.0));
    positionAlloc4->Add (Vector(795.5363870440594,287.4154599267856, 0.0));
    positionAlloc4->Add (Vector(767.4809810359088,178.16829512743348, 0.0));
    positionAlloc4->Add (Vector(683.1388809525064,500.97143760341885, 0.0));
    positionAlloc4->Add (Vector(370.1935683230727,589.8852475069875, 0.0));
    positionAlloc4->Add (Vector(807.8786498509735,325.09377000447097, 0.0));
    positionAlloc4->Add (Vector(802.6048031112169,362.8316887450653, 0.0));
    positionAlloc4->Add (Vector(159.5299030792706,377.4911866899603, 0.0));
    positionAlloc4->Add (Vector(198.61347823808063,567.0844278305058, 0.0));
    positionAlloc4->Add (Vector(611.8210997886223,544.3541332926886, 0.0));
    positionAlloc4->Add (Vector(760.2173615385153,507.68384093788944, 0.0));
    positionAlloc4->Add (Vector(766.4331825131832,777.6801926541096, 0.0));
    positionAlloc4->Add (Vector(272.71763736024775,212.70343610610033, 0.0));
    positionAlloc4->Add (Vector(450.27618298073463,821.534617798743, 0.0));
    positionAlloc4->Add (Vector(344.6778731970495,462.7888781257078, 0.0));
    positionAlloc4->Add (Vector(767.5398450322289,632.0213134212777, 0.0));
    positionAlloc4->Add (Vector(510.79571360483914,476.2835934874313, 0.0));
    positionAlloc4->Add (Vector(793.0712208045751,416.5373005104039, 0.0));
    positionAlloc4->Add (Vector(178.58348174714436,828.6209967744953, 0.0));
    positionAlloc4->Add (Vector(331.7728189122645,577.6662386053941, 0.0));
    positionAlloc4->Add (Vector(522.8429208201474,307.01183877748224, 0.0));
    positionAlloc4->Add (Vector(717.7893228447815,304.4293321784798, 0.0));
    positionAlloc4->Add (Vector(817.93351123435,808.3715016773756, 0.0));
    positionAlloc4->Add (Vector(357.7572983361152,847.9524138392005, 0.0));
    positionAlloc4->Add (Vector(176.66489811267,764.2139385556833, 0.0));
    positionAlloc4->Add (Vector(682.3195075519064,558.9840490909555, 0.0));
    positionAlloc4->Add (Vector(384.1453338080857,390.80959578983243, 0.0));
    positionAlloc4->Add (Vector(296.09029129926654,433.9173473050645, 0.0));
    positionAlloc4->Add (Vector(843.2621660770484,640.9589032128303, 0.0));
    positionAlloc4->Add (Vector(766.766077468412,525.4308777002743, 0.0));
    positionAlloc4->Add (Vector(562.2145979982131,845.3916935749902, 0.0));
    positionAlloc4->Add (Vector(826.6883317560249,347.2956709636429, 0.0));
    positionAlloc4->Add (Vector(583.2625472971721,361.82393463609264, 0.0));
    positionAlloc4->Add (Vector(755.7919247072842,507.95115211334564, 0.0));
    positionAlloc4->Add (Vector(797.5606427402984,187.7808427856391, 0.0));
    positionAlloc4->Add (Vector(477.4826375602197,296.74092156594236, 0.0));
    positionAlloc4->Add (Vector(485.31887128301366,361.2302900751615, 0.0));
    positionAlloc4->Add (Vector(247.1533306456788,632.7820764636037, 0.0));
    positionAlloc4->Add (Vector(151.9256968877447,405.9903711365515, 0.0));
    positionAlloc4->Add (Vector(465.6203340255727,541.0571083510067, 0.0));
    positionAlloc4->Add (Vector(484.5180962423929,720.2912313466582, 0.0));
    positionAlloc4->Add (Vector(785.5065736129308,603.3165823291108, 0.0));
    positionAlloc4->Add (Vector(526.7940676813811,274.80528314558666, 0.0));
    positionAlloc4->Add (Vector(152.21711989688023,347.47812427251836, 0.0));
    positionAlloc4->Add (Vector(517.6442486199619,773.2915319085265, 0.0));
    positionAlloc4->Add (Vector(630.2117151859684,571.8313938542836, 0.0));
    positionAlloc4->Add (Vector(803.7292799127785,407.4422705079845, 0.0));
    positionAlloc4->Add (Vector(654.6527838393379,697.6560566058117, 0.0));
    positionAlloc4->Add (Vector(581.0732298529092,370.6395426580444, 0.0));
    positionAlloc4->Add (Vector(187.08024237425758,697.5365595476517, 0.0));
    positionAlloc4->Add (Vector(303.14647758388554,759.9013450142664, 0.0));
    positionAlloc4->Add (Vector(501.4452074010974,696.570241759707, 0.0));
    positionAlloc4->Add (Vector(632.2251756227131,243.2530158280863, 0.0));
    positionAlloc4->Add (Vector(268.3502617897355,678.2180389904546, 0.0));
    positionAlloc4->Add (Vector(775.8656414894696,190.31428012489403, 0.0));
    positionAlloc4->Add (Vector(648.1670238899751,806.0071044280361, 0.0));
    positionAlloc4->Add (Vector(406.44372929606567,467.5688928769682, 0.0));
    positionAlloc4->Add (Vector(739.8390392038432,845.0755012735254, 0.0));
    positionAlloc4->Add (Vector(581.4076668249202,230.89395753133522, 0.0));
    positionAlloc4->Add (Vector(251.43618828432113,222.25371401904528, 0.0));
    positionAlloc4->Add (Vector(596.7929335941012,245.7483407443636, 0.0));
    positionAlloc4->Add (Vector(801.9658034896675,725.0889903205947, 0.0));
    positionAlloc4->Add (Vector(402.1916038131074,314.8127778861651, 0.0));
    positionAlloc4->Add (Vector(478.75424633213663,676.4556062707054, 0.0));
    positionAlloc4->Add (Vector(413.9582217194917,210.8851257449478, 0.0));
    positionAlloc4->Add (Vector(739.8358322330741,560.7669808447317, 0.0));
    positionAlloc4->Add (Vector(553.7738513138765,787.9761757650987, 0.0));
    positionAlloc4->Add (Vector(504.0823559973496,263.4923616926191, 0.0));
    positionAlloc4->Add (Vector(354.52669959261186,669.8439980072646, 0.0));
    positionAlloc4->Add (Vector(776.7530799968096,541.9583073256734, 0.0));
    positionAlloc4->Add (Vector(432.316921595038,479.55959045506563, 0.0));
    positionAlloc4->Add (Vector(368.2348361564907,280.38702761902573, 0.0));
    positionAlloc4->Add (Vector(600.827490768487,837.826993983321, 0.0));
    positionAlloc4->Add (Vector(684.5781703148092,819.7782264244963, 0.0));
    positionAlloc4->Add (Vector(757.2049216247466,569.2089840587889, 0.0));
    positionAlloc4->Add (Vector(332.30466339645176,299.1859600738909, 0.0));
    positionAlloc4->Add (Vector(466.7882633639975,175.7228136364879, 0.0));
    positionAlloc4->Add (Vector(384.4126932930651,523.5910480235862, 0.0));
    positionAlloc4->Add (Vector(480.3122107399037,464.39697600112555, 0.0));
    positionAlloc4->Add (Vector(815.7458081009145,503.5201399708995, 0.0));
    positionAlloc4->Add (Vector(839.3191249956717,501.5960904879695, 0.0));
    positionAlloc4->Add (Vector(676.5419011652857,312.388685947161, 0.0));
    positionAlloc4->Add (Vector(176.78490149251093,760.698580093293, 0.0));
    positionAlloc4->Add (Vector(820.3454057989048,296.62790226366485, 0.0));
    positionAlloc4->Add (Vector(739.4108638029555,389.31692543928045, 0.0));
    positionAlloc4->Add (Vector(713.2357090782832,477.29925312930783, 0.0));
    positionAlloc4->Add (Vector(226.5477533484811,433.442399358164, 0.0));
    positionAlloc4->Add (Vector(217.7038834857907,670.6338719770232, 0.0));
    positionAlloc4->Add (Vector(454.9338689389646,511.4211921943825, 0.0));
    positionAlloc4->Add (Vector(536.1527152686335,328.77816191511164, 0.0));
    positionAlloc4->Add (Vector(521.7476138756542,780.3608447241957, 0.0));
    positionAlloc4->Add (Vector(692.3349468125693,463.9609950527622, 0.0));
    positionAlloc4->Add (Vector(679.8066913704731,836.9872242575341, 0.0));
    positionAlloc4->Add (Vector(434.86764773197893,784.3926677125473, 0.0));
    positionAlloc4->Add (Vector(767.1304586928867,380.37240173343804, 0.0));
    positionAlloc4->Add (Vector(836.5716077467126,588.7005560726941, 0.0));
    positionAlloc4->Add (Vector(346.6024666753086,614.3224905721368, 0.0));
    positionAlloc4->Add (Vector(634.5637122517945,414.7560338542328, 0.0));
    positionAlloc4->Add (Vector(741.5786891498127,837.8114680862606, 0.0));
    positionAlloc4->Add (Vector(382.58880312258145,813.1369584565435, 0.0));
    positionAlloc4->Add (Vector(731.1402314595348,531.9487886154956, 0.0));
    positionAlloc4->Add (Vector(506.30323509344186,371.3246356133459, 0.0));
    positionAlloc4->Add (Vector(342.4937908529432,516.1289342304107, 0.0));
    positionAlloc4->Add (Vector(484.7157542936841,530.5874509325993, 0.0));
    positionAlloc4->Add (Vector(340.78457654315184,430.5146785795786, 0.0));
    positionAlloc4->Add (Vector(433.38249378965776,177.7629134819893, 0.0));
    positionAlloc4->Add (Vector(406.9903626058439,237.43273372694205, 0.0));
    positionAlloc4->Add (Vector(743.565402309191,255.77011142221903, 0.0));
    positionAlloc4->Add (Vector(565.7865272450013,351.000989537692, 0.0));
    positionAlloc4->Add (Vector(204.39838298222338,430.74851745974775, 0.0));
    positionAlloc4->Add (Vector(586.6650847446126,413.7374181430881, 0.0));
    positionAlloc4->Add (Vector(765.9627200111898,472.0084679564956, 0.0));
    positionAlloc4->Add (Vector(826.6575699294197,744.0756952959897, 0.0));
    positionAlloc4->Add (Vector(301.32973266506747,367.00000784808157, 0.0));
    positionAlloc4->Add (Vector(664.0277194464993,526.6678548309859, 0.0));
    positionAlloc4->Add (Vector(324.10097738520375,267.6748028442936, 0.0));
    positionAlloc4->Add (Vector(247.02104866874123,490.78059955619494, 0.0));
    positionAlloc4->Add (Vector(654.2241493002234,717.7825109184867, 0.0));
    positionAlloc4->Add (Vector(412.0679493002331,295.78747488729834, 0.0));
    positionAlloc4->Add (Vector(584.2121630303659,601.4875834371992, 0.0));
    positionAlloc4->Add (Vector(157.12250181248726,636.9394217821173, 0.0));
    positionAlloc4->Add (Vector(194.38139295786448,359.090630941071, 0.0));
    positionAlloc4->Add (Vector(285.2156477852397,736.470913567079, 0.0));
    positionAlloc4->Add (Vector(186.1217801762861,233.36603113072846, 0.0));
    positionAlloc4->Add (Vector(383.92868966282873,395.6673273091657, 0.0));
    positionAlloc4->Add (Vector(757.4795396119587,215.2480295424087, 0.0));
    positionAlloc4->Add (Vector(829.9064383810562,395.7964695169594, 0.0));
    positionAlloc4->Add (Vector(244.05872972441654,423.85271552899474, 0.0));
    positionAlloc4->Add (Vector(741.1569427871,570.3823673007253, 0.0));
    positionAlloc4->Add (Vector(720.6693587486927,629.2172350814287, 0.0));
    positionAlloc4->Add (Vector(814.4071373436358,810.5809606527016, 0.0));
    positionAlloc4->Add (Vector(801.681039969991,814.3893766414845, 0.0));
    positionAlloc4->Add (Vector(422.58947978437277,170.36133301228205, 0.0));
    positionAlloc4->Add (Vector(572.7497990697169,635.5739255163455, 0.0));
    positionAlloc4->Add (Vector(499.0186226779108,768.8728160637478, 0.0));
    positionAlloc4->Add (Vector(456.1541439962443,480.2876246914026, 0.0));
    positionAlloc4->Add (Vector(355.62276113789267,834.8381629576083, 0.0));
    positionAlloc4->Add (Vector(428.7044987540312,234.2286601782745, 0.0));
    positionAlloc4->Add (Vector(783.5995047520369,592.1046766353606, 0.0));
    positionAlloc4->Add (Vector(404.9362909043188,500.46664815590316, 0.0));
    positionAlloc4->Add (Vector(730.7799980894574,601.9015458112199, 0.0));
    positionAlloc4->Add (Vector(161.5659108505686,252.3082775919495, 0.0));
    positionAlloc4->Add (Vector(425.05808947218026,641.1724418893364, 0.0));
    positionAlloc4->Add (Vector(824.8198035827078,632.5386052733488, 0.0));
    positionAlloc4->Add (Vector(745.8379612351332,370.9191846999022, 0.0));
    positionAlloc4->Add (Vector(826.0135908653606,829.456143133453, 0.0));
    positionAlloc4->Add (Vector(433.0633405013719,496.3873474739124, 0.0));
    positionAlloc4->Add (Vector(209.9512661118425,275.0421991888197, 0.0));
    positionAlloc4->Add (Vector(493.36871591783125,627.0902742405767, 0.0));
    positionAlloc4->Add (Vector(524.8142997532041,752.560842332549, 0.0));
    positionAlloc4->Add (Vector(752.2570642703522,510.76203152127266, 0.0));
    positionAlloc4->Add (Vector(391.6449705847326,412.12682239138735, 0.0));
    positionAlloc4->Add (Vector(466.52106891330874,769.5451856416673, 0.0));
    positionAlloc4->Add (Vector(385.29133633820186,306.6080127522987, 0.0));
    positionAlloc4->Add (Vector(726.8728866986553,307.3121509201886, 0.0));
    positionAlloc4->Add (Vector(498.20565746611135,652.8746812486922, 0.0));
    positionAlloc4->Add (Vector(698.0451570586266,620.9534966262211, 0.0));
    positionAlloc4->Add (Vector(809.6669553139438,576.5855418974133, 0.0));
    positionAlloc4->Add (Vector(159.11769621904708,567.3375127057448, 0.0));
    positionAlloc4->Add (Vector(648.4762529975029,263.148234969542, 0.0));
    positionAlloc4->Add (Vector(403.36785015376023,369.9890687546458, 0.0));
    positionAlloc4->Add (Vector(626.4367498819155,387.7517261504104, 0.0));
    positionAlloc4->Add (Vector(577.2878425855663,552.4684198869993, 0.0));
    positionAlloc4->Add (Vector(528.9302014888626,705.7774082305452, 0.0));
    positionAlloc4->Add (Vector(247.79713345441084,181.86784985982047, 0.0));
    positionAlloc4->Add (Vector(220.98570236627157,243.06658953617688, 0.0));
    positionAlloc4->Add (Vector(432.71849682026146,628.5065833272968, 0.0));
    positionAlloc4->Add (Vector(436.56535723916153,690.9644851860593, 0.0));
    positionAlloc4->Add (Vector(181.40722414381986,543.3678997569081, 0.0));
    positionAlloc4->Add (Vector(542.9732612080609,487.45823922100226, 0.0));
    positionAlloc4->Add (Vector(784.4905928483075,792.9161703489855, 0.0));
    positionAlloc4->Add (Vector(701.527419131306,173.99959491524265, 0.0));
    positionAlloc4->Add (Vector(562.972452533262,693.4572232593391, 0.0));
    positionAlloc4->Add (Vector(825.3162917213775,165.83508998897048, 0.0));
    positionAlloc4->Add (Vector(598.4838873316905,193.8704942913761, 0.0));
    positionAlloc4->Add (Vector(162.45642300897762,674.6468538231729, 0.0));
    positionAlloc4->Add (Vector(199.63931755019374,383.39260101221157, 0.0));
    positionAlloc4->Add (Vector(446.5734867324933,614.7228849743134, 0.0));
    positionAlloc4->Add (Vector(443.953686930005,294.16405130203793, 0.0));
    positionAlloc4->Add (Vector(322.2518995937799,557.4064966173391, 0.0));
    positionAlloc4->Add (Vector(805.7036355291459,497.22436658438505, 0.0));
    positionAlloc4->Add (Vector(833.8030281575947,839.858603271935, 0.0));
    positionAlloc4->Add (Vector(723.2638620179371,237.43276123901717, 0.0));
    positionAlloc4->Add (Vector(242.71975360681492,685.9117663505771, 0.0));
    positionAlloc4->Add (Vector(353.14034685459023,689.0855453688374, 0.0));
    positionAlloc4->Add (Vector(828.3013118995071,497.74332291140945, 0.0));
    positionAlloc4->Add (Vector(368.8061812018325,787.7516298345354, 0.0));
    positionAlloc4->Add (Vector(574.6770823339039,436.94955334575076, 0.0));
    positionAlloc4->Add (Vector(846.1011254401894,606.4758383611556, 0.0));
    positionAlloc4->Add (Vector(275.2653082080241,610.049239767927, 0.0));
    positionAlloc4->Add (Vector(294.2972427472779,325.7043251420613, 0.0));
    positionAlloc4->Add (Vector(601.9661378486412,279.6510027053927, 0.0));
    positionAlloc4->Add (Vector(448.42376676302035,648.7809857006152, 0.0));
    positionAlloc4->Add (Vector(195.1588275166237,490.187249194872, 0.0));
    positionAlloc4->Add (Vector(453.0629848367816,491.9286375749547, 0.0));
    positionAlloc4->Add (Vector(572.0274376124848,352.7872144191403, 0.0));
    positionAlloc4->Add (Vector(433.4620465592031,167.70398629274317, 0.0));
    positionAlloc4->Add (Vector(655.918537440135,822.6202904574817, 0.0));
    positionAlloc4->Add (Vector(197.95207417149163,566.852950700272, 0.0));
    positionAlloc4->Add (Vector(831.0099273753339,209.14307297138026, 0.0));
    positionAlloc4->Add (Vector(699.4973213745541,310.7512745931027, 0.0));
    positionAlloc4->Add (Vector(387.1900661081146,676.1234078261999, 0.0));
    positionAlloc4->Add (Vector(736.003201927582,372.60449357608024, 0.0));
    positionAlloc4->Add (Vector(750.5514575679736,488.58301838534993, 0.0));
    positionAlloc4->Add (Vector(741.6694726885823,211.49932902856017, 0.0));
    positionAlloc4->Add (Vector(290.40882105826824,390.72569027665656, 0.0));
    positionAlloc4->Add (Vector(560.1784250899268,574.4417138392282, 0.0));
    positionAlloc4->Add (Vector(456.75924618436994,714.6452852076818, 0.0));
    positionAlloc4->Add (Vector(290.1511979591512,186.42089286489153, 0.0));
    positionAlloc4->Add (Vector(418.96137662754427,231.10759593397006, 0.0));
    positionAlloc4->Add (Vector(487.8970323673993,244.9460410644412, 0.0));
    positionAlloc4->Add (Vector(330.9673169814681,773.905048692378, 0.0));
    positionAlloc4->Add (Vector(835.7676912956794,232.9696201740883, 0.0));
    positionAlloc4->Add (Vector(401.64141369648394,194.4146926136225, 0.0));
    positionAlloc4->Add (Vector(516.5204011414662,239.87732026157613, 0.0));
    positionAlloc4->Add (Vector(420.247553416208,459.77605104410543, 0.0));
    positionAlloc4->Add (Vector(252.98424154730924,530.61726931475, 0.0));
    positionAlloc4->Add (Vector(587.5160992520445,585.9732460007381, 0.0));
    positionAlloc4->Add (Vector(605.3353456947196,623.0704064912644, 0.0));
    positionAlloc4->Add (Vector(751.6973121677797,828.1274097099622, 0.0));
    positionAlloc4->Add (Vector(506.17501394369646,795.3270618036498, 0.0));
    positionAlloc4->Add (Vector(213.7925212636598,253.04172276853632, 0.0));
    positionAlloc4->Add (Vector(302.4772111300505,623.6608131464892, 0.0));
    positionAlloc4->Add (Vector(347.61774049794474,619.3595190331373, 0.0));
    positionAlloc4->Add (Vector(284.12235026297265,337.4160531421953, 0.0));
    positionAlloc4->Add (Vector(266.0809832276457,187.20471452106352, 0.0));
    positionAlloc4->Add (Vector(357.57410054836873,733.7358617471634, 0.0));
    positionAlloc4->Add (Vector(576.4724172630445,576.6511174734068, 0.0));
    positionAlloc4->Add (Vector(477.99296329338875,741.7708633147234, 0.0));
    positionAlloc4->Add (Vector(789.2709580485435,435.8706620123904, 0.0));
    positionAlloc4->Add (Vector(198.59187101309635,155.19421551645513, 0.0));
    positionAlloc4->Add (Vector(510.4570757111169,179.41301388645175, 0.0));
    positionAlloc4->Add (Vector(798.6493772734117,525.2174726839355, 0.0));
    positionAlloc4->Add (Vector(566.5819915772227,533.5401105736544, 0.0));
    positionAlloc4->Add (Vector(434.2040412974531,715.8816098582928, 0.0));
    positionAlloc4->Add (Vector(773.772200917392,375.005021714912, 0.0));
    positionAlloc4->Add (Vector(705.7570945723694,813.713963514214, 0.0));
    positionAlloc4->Add (Vector(586.8146579961247,295.79991024496354, 0.0));
    positionAlloc4->Add (Vector(575.4766404989715,500.94001629734555, 0.0));
    positionAlloc4->Add (Vector(306.58493533529986,656.491504396093, 0.0));
    positionAlloc4->Add (Vector(644.4654469321292,441.9246571592195, 0.0));
    positionAlloc4->Add (Vector(793.0789245603083,553.4832854047743, 0.0));
    positionAlloc4->Add (Vector(445.2591734991716,448.52536015893844, 0.0));
    positionAlloc4->Add (Vector(811.078386507099,646.4834656456501, 0.0));
    positionAlloc4->Add (Vector(665.7294417319962,394.3728808147067, 0.0));
    positionAlloc4->Add (Vector(737.1961089850641,586.1497233817634, 0.0));
    positionAlloc4->Add (Vector(232.29694933125944,441.7884614822574, 0.0));
    positionAlloc4->Add (Vector(175.6268010895406,455.3029699410214, 0.0));
    positionAlloc4->Add (Vector(340.5762705685789,753.4045305880118, 0.0));
    positionAlloc4->Add (Vector(389.82795932303134,165.96536248133614, 0.0));
    positionAlloc4->Add (Vector(794.0110438972048,182.4830510054083, 0.0));
    positionAlloc4->Add (Vector(794.2334931705872,552.4388089245302, 0.0));
    positionAlloc4->Add (Vector(585.8852698339829,311.5682119519644, 0.0));
    positionAlloc4->Add (Vector(356.58823080425395,402.79626197400717, 0.0));
    positionAlloc4->Add (Vector(546.1536240844331,360.37068480200287, 0.0));
    positionAlloc4->Add (Vector(365.848807460266,725.1698850576112, 0.0));
    positionAlloc4->Add (Vector(557.1834093233762,749.4074161592799, 0.0));
    positionAlloc4->Add (Vector(452.3475849421458,539.7526338431926, 0.0));
    positionAlloc4->Add (Vector(703.0187331495993,457.09808421866035, 0.0));
    positionAlloc4->Add (Vector(228.09829466937896,441.331362653587, 0.0));
    positionAlloc4->Add (Vector(151.76523319197403,412.5412608791744, 0.0));
    positionAlloc4->Add (Vector(805.0689309379086,508.256495992793, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_23.xml");
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
	  flowmon->SerializeToXmlFile ("switch_SUI_flowmon/switch_SUI_23.flowmon", false, false);
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