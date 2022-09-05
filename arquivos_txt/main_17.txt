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
	  uint16_t numberOfRrhs = 86;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 592;
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
    positionAlloc->Add (Vector (850.0,850.0, 0.0));
    positionAlloc->Add (Vector (850.0,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (850.0,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (850.0,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (850.0,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (850.0,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (850.0,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (850.0,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (850.0,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (850.0,150.0, 0.0));
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
    positionAlloc->Add (Vector (694.4444444444445,150.0, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,850.0, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,150.0, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,850.0, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (538.8888888888889,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,850.0, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,150.0, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,850.0, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (383.33333333333337,150.0, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (305.55555555555554,150.0, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,150.0, 0.0));
    positionAlloc->Add (Vector (150.0,850.0, 0.0));
    positionAlloc->Add (Vector (150.0,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (150.0,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (150.0,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (150.0,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (150.0,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (150.0,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (150.0,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (150.0,150.0, 0.0));
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
    positionAlloc4->Add (Vector(400.6597113253647,775.9596299181728, 0.0));
    positionAlloc4->Add (Vector(732.0986488554031,564.0149520101174, 0.0));
    positionAlloc4->Add (Vector(177.2535951025598,175.1928802080675, 0.0));
    positionAlloc4->Add (Vector(540.5641190041363,279.4617360482124, 0.0));
    positionAlloc4->Add (Vector(322.52190874625353,250.23825271616948, 0.0));
    positionAlloc4->Add (Vector(271.18780741560903,183.46774459173585, 0.0));
    positionAlloc4->Add (Vector(797.7308158122188,462.7839613639501, 0.0));
    positionAlloc4->Add (Vector(713.2701324988909,665.5459461017455, 0.0));
    positionAlloc4->Add (Vector(481.47775282922333,672.596671662375, 0.0));
    positionAlloc4->Add (Vector(556.1532745833783,771.6755433080305, 0.0));
    positionAlloc4->Add (Vector(744.1154294277775,247.43762939181994, 0.0));
    positionAlloc4->Add (Vector(749.2911542471321,311.57244963144717, 0.0));
    positionAlloc4->Add (Vector(795.2918496834494,175.37662226035638, 0.0));
    positionAlloc4->Add (Vector(511.01533186183144,413.8952647566444, 0.0));
    positionAlloc4->Add (Vector(583.017873050771,736.7044157634499, 0.0));
    positionAlloc4->Add (Vector(328.4173694878285,719.5395161560862, 0.0));
    positionAlloc4->Add (Vector(824.1802615694158,236.01290592590686, 0.0));
    positionAlloc4->Add (Vector(567.7172039459356,235.91803827223433, 0.0));
    positionAlloc4->Add (Vector(537.0628255045247,637.577315671525, 0.0));
    positionAlloc4->Add (Vector(354.5237623674314,801.7200173942938, 0.0));
    positionAlloc4->Add (Vector(685.079441793802,737.9017008718293, 0.0));
    positionAlloc4->Add (Vector(293.2378307913139,627.9860862202293, 0.0));
    positionAlloc4->Add (Vector(618.9598293546364,159.41083078839137, 0.0));
    positionAlloc4->Add (Vector(327.45433224751025,793.285527850279, 0.0));
    positionAlloc4->Add (Vector(608.2093601340222,222.4482525154974, 0.0));
    positionAlloc4->Add (Vector(489.9258225977929,476.3331611088737, 0.0));
    positionAlloc4->Add (Vector(620.5577663830225,815.7230473240243, 0.0));
    positionAlloc4->Add (Vector(511.23557563925493,392.1807414649181, 0.0));
    positionAlloc4->Add (Vector(431.5053333364501,196.635461157223, 0.0));
    positionAlloc4->Add (Vector(600.225031808927,767.5486882721194, 0.0));
    positionAlloc4->Add (Vector(697.7091446401354,366.47927733129984, 0.0));
    positionAlloc4->Add (Vector(582.629968457229,472.04636727056567, 0.0));
    positionAlloc4->Add (Vector(709.1244042664006,329.9802641029015, 0.0));
    positionAlloc4->Add (Vector(656.6566611223499,618.8061054818054, 0.0));
    positionAlloc4->Add (Vector(364.06180771463517,769.3621566721371, 0.0));
    positionAlloc4->Add (Vector(366.7763950460191,357.98201068948936, 0.0));
    positionAlloc4->Add (Vector(517.0001883623258,435.7867417484981, 0.0));
    positionAlloc4->Add (Vector(200.59040857866586,763.120004956866, 0.0));
    positionAlloc4->Add (Vector(350.0119999960469,758.6213815765212, 0.0));
    positionAlloc4->Add (Vector(839.5068234806124,217.37540826640793, 0.0));
    positionAlloc4->Add (Vector(247.84016276544614,558.3908046820868, 0.0));
    positionAlloc4->Add (Vector(521.8012427952647,252.46315706770991, 0.0));
    positionAlloc4->Add (Vector(639.28957572214,469.36803083733486, 0.0));
    positionAlloc4->Add (Vector(448.3966343781744,241.7101881035301, 0.0));
    positionAlloc4->Add (Vector(345.34219402595556,262.8145815273128, 0.0));
    positionAlloc4->Add (Vector(239.69452817536683,508.47269882465946, 0.0));
    positionAlloc4->Add (Vector(474.48655631188,666.2426391219435, 0.0));
    positionAlloc4->Add (Vector(824.08315965514,732.0744807128127, 0.0));
    positionAlloc4->Add (Vector(426.9527748376987,587.1910451421761, 0.0));
    positionAlloc4->Add (Vector(631.3210002445136,845.0324436294851, 0.0));
    positionAlloc4->Add (Vector(429.06150126288844,245.81622378427937, 0.0));
    positionAlloc4->Add (Vector(174.31950214562886,478.1134687750221, 0.0));
    positionAlloc4->Add (Vector(493.32166761024155,201.88342207017064, 0.0));
    positionAlloc4->Add (Vector(569.2930634417185,319.07996890118454, 0.0));
    positionAlloc4->Add (Vector(831.6237498825724,428.00674996004426, 0.0));
    positionAlloc4->Add (Vector(272.8580167258473,323.0223212161623, 0.0));
    positionAlloc4->Add (Vector(606.882327538648,234.91329319561453, 0.0));
    positionAlloc4->Add (Vector(331.2989181696698,491.8367007967231, 0.0));
    positionAlloc4->Add (Vector(562.3272584258384,327.9766818301807, 0.0));
    positionAlloc4->Add (Vector(368.7538198896417,272.0873802514905, 0.0));
    positionAlloc4->Add (Vector(714.0346827801048,156.4696202067547, 0.0));
    positionAlloc4->Add (Vector(354.16113935065596,236.27893396561723, 0.0));
    positionAlloc4->Add (Vector(427.15876706978185,340.02246411469366, 0.0));
    positionAlloc4->Add (Vector(708.7280927023901,239.0428835364466, 0.0));
    positionAlloc4->Add (Vector(826.1320943060676,481.4711139925348, 0.0));
    positionAlloc4->Add (Vector(776.2673370198095,158.13862089975663, 0.0));
    positionAlloc4->Add (Vector(786.0588929382517,715.9592912822875, 0.0));
    positionAlloc4->Add (Vector(825.4180959146972,556.9694267355578, 0.0));
    positionAlloc4->Add (Vector(763.5399291611559,746.497694698133, 0.0));
    positionAlloc4->Add (Vector(590.2024097411515,683.5197738385745, 0.0));
    positionAlloc4->Add (Vector(325.5628927506275,326.9836482156129, 0.0));
    positionAlloc4->Add (Vector(669.1261837603369,635.3980894113914, 0.0));
    positionAlloc4->Add (Vector(646.3866885706603,209.3316312927253, 0.0));
    positionAlloc4->Add (Vector(333.62109511587425,305.77084774922, 0.0));
    positionAlloc4->Add (Vector(398.59325072105344,346.7195483891417, 0.0));
    positionAlloc4->Add (Vector(846.8641597850651,159.5743557732509, 0.0));
    positionAlloc4->Add (Vector(703.4616682319275,637.9978697620481, 0.0));
    positionAlloc4->Add (Vector(433.86480864247517,548.1383717660141, 0.0));
    positionAlloc4->Add (Vector(765.5263974088039,806.3774715116292, 0.0));
    positionAlloc4->Add (Vector(687.7734443524214,712.0944125070071, 0.0));
    positionAlloc4->Add (Vector(547.7458385512437,719.9476288680075, 0.0));
    positionAlloc4->Add (Vector(681.1346254913143,283.5020771033217, 0.0));
    positionAlloc4->Add (Vector(210.61115072554517,564.71180855329, 0.0));
    positionAlloc4->Add (Vector(703.414490055289,493.07803364551876, 0.0));
    positionAlloc4->Add (Vector(189.08675461353207,834.4064583108506, 0.0));
    positionAlloc4->Add (Vector(159.56048549825599,487.17762571863443, 0.0));
    positionAlloc4->Add (Vector(278.7631924931688,475.23894960798117, 0.0));
    positionAlloc4->Add (Vector(379.6724330892752,724.2415105332713, 0.0));
    positionAlloc4->Add (Vector(284.9280253209277,552.3289834587831, 0.0));
    positionAlloc4->Add (Vector(324.22886963523433,314.18087052363717, 0.0));
    positionAlloc4->Add (Vector(205.14744970274003,343.8311828186563, 0.0));
    positionAlloc4->Add (Vector(661.0526166223303,299.042099923014, 0.0));
    positionAlloc4->Add (Vector(844.9701235788771,624.8986669061157, 0.0));
    positionAlloc4->Add (Vector(216.45240669593267,373.19626625560767, 0.0));
    positionAlloc4->Add (Vector(609.775614559947,607.6632615690314, 0.0));
    positionAlloc4->Add (Vector(340.5732754593299,206.60882208247423, 0.0));
    positionAlloc4->Add (Vector(264.5492889295466,759.8192323726807, 0.0));
    positionAlloc4->Add (Vector(786.6678915536307,729.5092656543217, 0.0));
    positionAlloc4->Add (Vector(656.8061545130877,280.17425684069224, 0.0));
    positionAlloc4->Add (Vector(819.5694243546063,702.3517880171855, 0.0));
    positionAlloc4->Add (Vector(388.1576690656744,372.6927719334436, 0.0));
    positionAlloc4->Add (Vector(281.7289184175619,298.38706455948295, 0.0));
    positionAlloc4->Add (Vector(843.3260124431034,715.3617211023461, 0.0));
    positionAlloc4->Add (Vector(813.0589191426976,817.9076089834246, 0.0));
    positionAlloc4->Add (Vector(305.1646901611718,303.94718362756197, 0.0));
    positionAlloc4->Add (Vector(475.17692446709344,673.6771688119549, 0.0));
    positionAlloc4->Add (Vector(370.0204151072845,209.2688699927026, 0.0));
    positionAlloc4->Add (Vector(539.8234467911661,700.9396577932564, 0.0));
    positionAlloc4->Add (Vector(282.53362744794936,298.0643624209592, 0.0));
    positionAlloc4->Add (Vector(699.7321837971488,235.4376369569212, 0.0));
    positionAlloc4->Add (Vector(439.0944421350213,255.9388810840731, 0.0));
    positionAlloc4->Add (Vector(834.0017329602841,707.6066580106032, 0.0));
    positionAlloc4->Add (Vector(541.8887669087942,322.8415465761533, 0.0));
    positionAlloc4->Add (Vector(482.3094496893384,227.01487009403365, 0.0));
    positionAlloc4->Add (Vector(669.1626221026621,499.5831929661712, 0.0));
    positionAlloc4->Add (Vector(812.2015353373334,648.3326197089136, 0.0));
    positionAlloc4->Add (Vector(308.5252841741765,240.39548284643402, 0.0));
    positionAlloc4->Add (Vector(812.8071782471753,354.20672102625264, 0.0));
    positionAlloc4->Add (Vector(592.0314969929251,209.96493583288103, 0.0));
    positionAlloc4->Add (Vector(684.1135784709672,267.9984010744789, 0.0));
    positionAlloc4->Add (Vector(493.33488504564707,177.86397825546112, 0.0));
    positionAlloc4->Add (Vector(353.6709013625732,706.0466304733726, 0.0));
    positionAlloc4->Add (Vector(551.2560702893705,838.692145993282, 0.0));
    positionAlloc4->Add (Vector(702.6068791708185,275.5473514851863, 0.0));
    positionAlloc4->Add (Vector(740.5678627936087,508.5993201351492, 0.0));
    positionAlloc4->Add (Vector(808.8536701219601,744.6623663348746, 0.0));
    positionAlloc4->Add (Vector(467.19543203494976,370.6670398807148, 0.0));
    positionAlloc4->Add (Vector(411.28321242048816,772.1012662289224, 0.0));
    positionAlloc4->Add (Vector(830.5818612191974,540.9613072125776, 0.0));
    positionAlloc4->Add (Vector(212.98988944473447,350.58317898767194, 0.0));
    positionAlloc4->Add (Vector(264.8279629534891,168.17271284774466, 0.0));
    positionAlloc4->Add (Vector(777.2108660731357,753.0949989749695, 0.0));
    positionAlloc4->Add (Vector(460.65638868581016,349.4360361292356, 0.0));
    positionAlloc4->Add (Vector(479.8656811041534,763.4464791190342, 0.0));
    positionAlloc4->Add (Vector(576.2334027210266,631.4411248818371, 0.0));
    positionAlloc4->Add (Vector(690.9631450032184,324.7767450375275, 0.0));
    positionAlloc4->Add (Vector(291.25970693953315,199.37400666612535, 0.0));
    positionAlloc4->Add (Vector(403.0348338659929,425.8932699798182, 0.0));
    positionAlloc4->Add (Vector(563.4586586260486,830.4342753430001, 0.0));
    positionAlloc4->Add (Vector(150.9927540937408,541.9749030020903, 0.0));
    positionAlloc4->Add (Vector(464.63737562979964,319.1698581808207, 0.0));
    positionAlloc4->Add (Vector(513.8197405060871,627.2577678990706, 0.0));
    positionAlloc4->Add (Vector(653.4375923595583,599.9624561480132, 0.0));
    positionAlloc4->Add (Vector(600.7596137351372,578.9983417743547, 0.0));
    positionAlloc4->Add (Vector(601.5682119498573,540.2321842829576, 0.0));
    positionAlloc4->Add (Vector(508.9155291996576,482.95568911614566, 0.0));
    positionAlloc4->Add (Vector(578.9987530957233,631.7992692576056, 0.0));
    positionAlloc4->Add (Vector(676.2784718983772,464.78277135853784, 0.0));
    positionAlloc4->Add (Vector(613.0085869155423,743.5469135948877, 0.0));
    positionAlloc4->Add (Vector(250.50422441401668,612.7189407640569, 0.0));
    positionAlloc4->Add (Vector(277.04098740108907,234.26807534223383, 0.0));
    positionAlloc4->Add (Vector(774.955349494462,848.8122090137708, 0.0));
    positionAlloc4->Add (Vector(498.0718361326943,178.30718184886294, 0.0));
    positionAlloc4->Add (Vector(467.75307322009326,435.5547597380074, 0.0));
    positionAlloc4->Add (Vector(741.5751093037885,571.508014288518, 0.0));
    positionAlloc4->Add (Vector(488.0673542516659,358.7381087056506, 0.0));
    positionAlloc4->Add (Vector(162.4495988837384,173.4914271530758, 0.0));
    positionAlloc4->Add (Vector(226.9018299369134,447.2784441878723, 0.0));
    positionAlloc4->Add (Vector(492.49055812832574,672.7051211788435, 0.0));
    positionAlloc4->Add (Vector(378.24935162898635,243.49078610443846, 0.0));
    positionAlloc4->Add (Vector(540.1415453015268,772.0147462363044, 0.0));
    positionAlloc4->Add (Vector(309.8929197823551,446.11011236923383, 0.0));
    positionAlloc4->Add (Vector(414.70837299251923,628.6114854573773, 0.0));
    positionAlloc4->Add (Vector(162.90394679051937,253.5683171417795, 0.0));
    positionAlloc4->Add (Vector(480.72223019776334,494.12934138823147, 0.0));
    positionAlloc4->Add (Vector(280.6892894140209,288.88051047200145, 0.0));
    positionAlloc4->Add (Vector(186.52949437645398,807.4568953198446, 0.0));
    positionAlloc4->Add (Vector(538.5063605323959,320.417808380236, 0.0));
    positionAlloc4->Add (Vector(469.82618848769954,437.62993349426455, 0.0));
    positionAlloc4->Add (Vector(794.2728820500258,765.4294424453685, 0.0));
    positionAlloc4->Add (Vector(314.5201509826936,445.40367559200615, 0.0));
    positionAlloc4->Add (Vector(156.690730303648,315.9921216683261, 0.0));
    positionAlloc4->Add (Vector(810.9220544539633,268.79939147643745, 0.0));
    positionAlloc4->Add (Vector(356.76390268843124,164.97219267255585, 0.0));
    positionAlloc4->Add (Vector(292.9853308331038,686.6981720945023, 0.0));
    positionAlloc4->Add (Vector(257.62152239705017,243.2587235890473, 0.0));
    positionAlloc4->Add (Vector(828.0640392027163,620.504080852685, 0.0));
    positionAlloc4->Add (Vector(370.2822817099286,487.3352981852015, 0.0));
    positionAlloc4->Add (Vector(526.4864762045154,696.5025969945657, 0.0));
    positionAlloc4->Add (Vector(803.4670189016414,518.7248249165234, 0.0));
    positionAlloc4->Add (Vector(251.66300835344066,170.84655112464466, 0.0));
    positionAlloc4->Add (Vector(817.278399004584,745.3170424486533, 0.0));
    positionAlloc4->Add (Vector(308.7386873455366,469.117811896872, 0.0));
    positionAlloc4->Add (Vector(433.25879253326656,450.333595780481, 0.0));
    positionAlloc4->Add (Vector(570.7537593044749,539.2808790410492, 0.0));
    positionAlloc4->Add (Vector(194.71304870332187,399.4924986104513, 0.0));
    positionAlloc4->Add (Vector(213.62923103218347,510.33716185281, 0.0));
    positionAlloc4->Add (Vector(282.2396995035868,681.2905083104379, 0.0));
    positionAlloc4->Add (Vector(712.0508889619933,429.746937745755, 0.0));
    positionAlloc4->Add (Vector(717.9021977622278,301.51015838817005, 0.0));
    positionAlloc4->Add (Vector(392.91961772794525,526.776922441336, 0.0));
    positionAlloc4->Add (Vector(751.5521573269749,221.10050055126288, 0.0));
    positionAlloc4->Add (Vector(557.7861874692202,537.0585237324765, 0.0));
    positionAlloc4->Add (Vector(303.4290419566322,624.9774709500631, 0.0));
    positionAlloc4->Add (Vector(186.871332280788,772.4014733970498, 0.0));
    positionAlloc4->Add (Vector(478.31010138695154,761.6738433960365, 0.0));
    positionAlloc4->Add (Vector(733.9788525096203,362.92773433996126, 0.0));
    positionAlloc4->Add (Vector(412.3965909577632,709.9629340094173, 0.0));
    positionAlloc4->Add (Vector(472.46462129749267,326.2315705818136, 0.0));
    positionAlloc4->Add (Vector(425.57332872231495,361.9287594325673, 0.0));
    positionAlloc4->Add (Vector(790.6763188215309,432.9839005456072, 0.0));
    positionAlloc4->Add (Vector(533.4162110269917,208.2523919180493, 0.0));
    positionAlloc4->Add (Vector(731.7943357825872,347.42964835276973, 0.0));
    positionAlloc4->Add (Vector(540.7935670827411,752.9539032695967, 0.0));
    positionAlloc4->Add (Vector(240.42874246612465,750.1903926555606, 0.0));
    positionAlloc4->Add (Vector(437.94237421705026,320.3587993844632, 0.0));
    positionAlloc4->Add (Vector(330.1393917326726,338.883417303766, 0.0));
    positionAlloc4->Add (Vector(476.89401773127037,799.4895866243362, 0.0));
    positionAlloc4->Add (Vector(182.44728860399567,534.7687683025838, 0.0));
    positionAlloc4->Add (Vector(673.3022452378773,661.3798420476053, 0.0));
    positionAlloc4->Add (Vector(496.4664935934941,812.0165715816058, 0.0));
    positionAlloc4->Add (Vector(382.85815063353584,564.8610535198127, 0.0));
    positionAlloc4->Add (Vector(536.225618057686,707.4420428357544, 0.0));
    positionAlloc4->Add (Vector(234.7110017240542,800.9893481938943, 0.0));
    positionAlloc4->Add (Vector(264.95187075914214,189.18619940318047, 0.0));
    positionAlloc4->Add (Vector(308.19073596304474,609.489193960771, 0.0));
    positionAlloc4->Add (Vector(409.56293828139326,646.2678863998954, 0.0));
    positionAlloc4->Add (Vector(799.4125958481027,216.11993588810344, 0.0));
    positionAlloc4->Add (Vector(648.5742242908098,413.11876835697166, 0.0));
    positionAlloc4->Add (Vector(335.8749239296335,461.80357270022114, 0.0));
    positionAlloc4->Add (Vector(749.2667325654686,267.19606342237586, 0.0));
    positionAlloc4->Add (Vector(316.1199126263261,456.3184206276555, 0.0));
    positionAlloc4->Add (Vector(521.8391559690581,627.9456501358912, 0.0));
    positionAlloc4->Add (Vector(696.6532258533459,844.0553817949514, 0.0));
    positionAlloc4->Add (Vector(581.6986315369943,215.60740624180147, 0.0));
    positionAlloc4->Add (Vector(636.2704025509396,155.6706591531257, 0.0));
    positionAlloc4->Add (Vector(450.27898650225507,755.3531553999268, 0.0));
    positionAlloc4->Add (Vector(214.35655514458637,630.1504544957537, 0.0));
    positionAlloc4->Add (Vector(479.87550163646966,168.7394099921895, 0.0));
    positionAlloc4->Add (Vector(325.3810969899241,383.68944504037006, 0.0));
    positionAlloc4->Add (Vector(258.8166045095201,567.159117011493, 0.0));
    positionAlloc4->Add (Vector(314.4639939833749,459.7296488954629, 0.0));
    positionAlloc4->Add (Vector(520.9294230635053,740.4083105653182, 0.0));
    positionAlloc4->Add (Vector(610.444885066725,556.7856799503388, 0.0));
    positionAlloc4->Add (Vector(402.76611010513494,677.0086111524994, 0.0));
    positionAlloc4->Add (Vector(832.8802356067673,277.7720857060717, 0.0));
    positionAlloc4->Add (Vector(843.3956489297143,346.27592501324534, 0.0));
    positionAlloc4->Add (Vector(163.46219241973213,228.54095673594998, 0.0));
    positionAlloc4->Add (Vector(444.47756592864994,732.9901650067862, 0.0));
    positionAlloc4->Add (Vector(775.3391492793257,270.3792812904801, 0.0));
    positionAlloc4->Add (Vector(699.1570949742871,567.3139585374743, 0.0));
    positionAlloc4->Add (Vector(732.0049335048977,281.87633718034454, 0.0));
    positionAlloc4->Add (Vector(421.3071672057832,553.360588244554, 0.0));
    positionAlloc4->Add (Vector(308.9884559143059,771.8154232063938, 0.0));
    positionAlloc4->Add (Vector(789.0271862143638,678.4522131505668, 0.0));
    positionAlloc4->Add (Vector(414.7202701170519,673.4220746528396, 0.0));
    positionAlloc4->Add (Vector(748.7913082418261,454.9245482366761, 0.0));
    positionAlloc4->Add (Vector(384.5344196569377,694.9820749958823, 0.0));
    positionAlloc4->Add (Vector(423.93298552633485,821.949684408451, 0.0));
    positionAlloc4->Add (Vector(353.7023543801971,514.4357256200162, 0.0));
    positionAlloc4->Add (Vector(225.5340020560833,804.5536311863282, 0.0));
    positionAlloc4->Add (Vector(333.71941208942326,380.0145141196384, 0.0));
    positionAlloc4->Add (Vector(392.54654422377746,692.0789759120789, 0.0));
    positionAlloc4->Add (Vector(758.8314746973819,707.8380960198408, 0.0));
    positionAlloc4->Add (Vector(735.4585051070604,632.9938374480143, 0.0));
    positionAlloc4->Add (Vector(803.4309543632905,304.09059458503486, 0.0));
    positionAlloc4->Add (Vector(765.294615431055,678.2777646076988, 0.0));
    positionAlloc4->Add (Vector(204.3661032448475,821.8087866272729, 0.0));
    positionAlloc4->Add (Vector(792.1122018248354,471.607122632865, 0.0));
    positionAlloc4->Add (Vector(176.0227016347522,248.56830346509963, 0.0));
    positionAlloc4->Add (Vector(235.08964066278833,300.49455651187384, 0.0));
    positionAlloc4->Add (Vector(278.89101615682944,738.2530537697231, 0.0));
    positionAlloc4->Add (Vector(604.3311270877498,767.9682592071595, 0.0));
    positionAlloc4->Add (Vector(336.5850843180009,391.41164858524684, 0.0));
    positionAlloc4->Add (Vector(405.7842916310898,479.30257336426894, 0.0));
    positionAlloc4->Add (Vector(841.647559372805,611.1842902511867, 0.0));
    positionAlloc4->Add (Vector(182.0964128622911,155.07669961060114, 0.0));
    positionAlloc4->Add (Vector(534.1037239081346,486.74703359323155, 0.0));
    positionAlloc4->Add (Vector(188.46741770803862,273.629584801886, 0.0));
    positionAlloc4->Add (Vector(311.79823940689334,772.2941469894326, 0.0));
    positionAlloc4->Add (Vector(194.67822429679953,381.2503820893427, 0.0));
    positionAlloc4->Add (Vector(588.8413955197269,271.6591999946678, 0.0));
    positionAlloc4->Add (Vector(575.1494684344207,279.6155317913418, 0.0));
    positionAlloc4->Add (Vector(462.175689283084,366.0253771448056, 0.0));
    positionAlloc4->Add (Vector(663.2433634670836,451.9558194716014, 0.0));
    positionAlloc4->Add (Vector(208.665903441015,262.50090942405393, 0.0));
    positionAlloc4->Add (Vector(594.1171978289221,684.8389128967069, 0.0));
    positionAlloc4->Add (Vector(779.5375863128821,828.9479014961604, 0.0));
    positionAlloc4->Add (Vector(470.5365848753008,547.0151824071095, 0.0));
    positionAlloc4->Add (Vector(162.29581899277687,583.4455475208761, 0.0));
    positionAlloc4->Add (Vector(746.9109756949833,323.0015490212053, 0.0));
    positionAlloc4->Add (Vector(718.6817388219207,483.5382454986893, 0.0));
    positionAlloc4->Add (Vector(264.98456460443674,742.1869047122092, 0.0));
    positionAlloc4->Add (Vector(372.7935822836363,584.793384793832, 0.0));
    positionAlloc4->Add (Vector(399.31687572011015,410.63647286976675, 0.0));
    positionAlloc4->Add (Vector(330.1878611376938,417.0087574739262, 0.0));
    positionAlloc4->Add (Vector(760.8456813838385,358.14062327803197, 0.0));
    positionAlloc4->Add (Vector(174.88631428433857,848.1667887696692, 0.0));
    positionAlloc4->Add (Vector(164.2385817473207,657.2419358295756, 0.0));
    positionAlloc4->Add (Vector(757.4799515154529,600.9689805781873, 0.0));
    positionAlloc4->Add (Vector(780.8323853682211,332.58461396278346, 0.0));
    positionAlloc4->Add (Vector(451.50163512560727,501.1920889234444, 0.0));
    positionAlloc4->Add (Vector(527.963759054969,288.30921007982283, 0.0));
    positionAlloc4->Add (Vector(545.8104496957399,455.402551505586, 0.0));
    positionAlloc4->Add (Vector(365.894762180076,625.5859903165908, 0.0));
    positionAlloc4->Add (Vector(732.9990660325448,181.0428033675208, 0.0));
    positionAlloc4->Add (Vector(424.92512847709696,399.5893565818494, 0.0));
    positionAlloc4->Add (Vector(749.4618687503616,367.01144907529425, 0.0));
    positionAlloc4->Add (Vector(441.66905824445934,670.7056966217198, 0.0));
    positionAlloc4->Add (Vector(506.9248030116102,754.1188637047366, 0.0));
    positionAlloc4->Add (Vector(277.88636889796794,367.87167680501784, 0.0));
    positionAlloc4->Add (Vector(378.3295428945365,822.642202090643, 0.0));
    positionAlloc4->Add (Vector(495.3101513672714,357.19483760077685, 0.0));
    positionAlloc4->Add (Vector(611.3909388362124,185.40289625530727, 0.0));
    positionAlloc4->Add (Vector(549.9879751309641,463.16587584664717, 0.0));
    positionAlloc4->Add (Vector(627.0745001459295,387.3199848207368, 0.0));
    positionAlloc4->Add (Vector(771.3122935032009,546.4735044417682, 0.0));
    positionAlloc4->Add (Vector(288.35344678373076,485.74236933966876, 0.0));
    positionAlloc4->Add (Vector(500.06463504756323,222.31033446929592, 0.0));
    positionAlloc4->Add (Vector(280.590538536931,595.4410207226625, 0.0));
    positionAlloc4->Add (Vector(364.0316410004531,471.71090411561073, 0.0));
    positionAlloc4->Add (Vector(802.3318790944223,508.9062407514865, 0.0));
    positionAlloc4->Add (Vector(528.7212023439681,799.0859428419864, 0.0));
    positionAlloc4->Add (Vector(581.0547864709697,593.6518936815015, 0.0));
    positionAlloc4->Add (Vector(525.5006821288505,734.7536479252248, 0.0));
    positionAlloc4->Add (Vector(511.7604162834368,299.4135169282296, 0.0));
    positionAlloc4->Add (Vector(394.75736632356643,214.99156057143438, 0.0));
    positionAlloc4->Add (Vector(829.8531529725474,659.0696236989853, 0.0));
    positionAlloc4->Add (Vector(376.69474152209136,303.95401968140686, 0.0));
    positionAlloc4->Add (Vector(534.492505689774,150.93354764355442, 0.0));
    positionAlloc4->Add (Vector(522.7604088295641,229.97391728063943, 0.0));
    positionAlloc4->Add (Vector(354.0597008410474,314.0840210857813, 0.0));
    positionAlloc4->Add (Vector(765.3434031865751,433.1614821805922, 0.0));
    positionAlloc4->Add (Vector(540.2627852670927,819.8219098430764, 0.0));
    positionAlloc4->Add (Vector(584.7877093635803,314.72788567182477, 0.0));
    positionAlloc4->Add (Vector(690.084954701472,195.26588069539946, 0.0));
    positionAlloc4->Add (Vector(444.93787162773384,747.8108713188216, 0.0));
    positionAlloc4->Add (Vector(527.6546017890068,633.6443990498365, 0.0));
    positionAlloc4->Add (Vector(648.5315873856446,371.95163855414074, 0.0));
    positionAlloc4->Add (Vector(270.7162879135258,570.9158546702929, 0.0));
    positionAlloc4->Add (Vector(267.5248842635093,378.77421437621246, 0.0));
    positionAlloc4->Add (Vector(821.1506507582856,753.5902105470575, 0.0));
    positionAlloc4->Add (Vector(165.00790663617929,667.0568554116995, 0.0));
    positionAlloc4->Add (Vector(556.8297399124795,821.0884189557789, 0.0));
    positionAlloc4->Add (Vector(617.7659118841407,651.8189229180941, 0.0));
    positionAlloc4->Add (Vector(672.7393112336003,377.82829207363585, 0.0));
    positionAlloc4->Add (Vector(175.59886865407074,280.48969301642876, 0.0));
    positionAlloc4->Add (Vector(216.38130719087096,392.7999076502227, 0.0));
    positionAlloc4->Add (Vector(298.02922403482256,336.02246333066296, 0.0));
    positionAlloc4->Add (Vector(387.0477862805279,355.43500415131007, 0.0));
    positionAlloc4->Add (Vector(660.1839029856123,458.952055079935, 0.0));
    positionAlloc4->Add (Vector(556.5327643665771,488.57631203050653, 0.0));
    positionAlloc4->Add (Vector(385.25487141956665,268.84385783068547, 0.0));
    positionAlloc4->Add (Vector(658.031212817293,621.9655931397393, 0.0));
    positionAlloc4->Add (Vector(253.79777652714117,641.4249679374996, 0.0));
    positionAlloc4->Add (Vector(314.4504351204003,220.2317372484481, 0.0));
    positionAlloc4->Add (Vector(832.5713307701402,823.5946421067242, 0.0));
    positionAlloc4->Add (Vector(315.84950305782763,635.5443885093537, 0.0));
    positionAlloc4->Add (Vector(622.7264687650162,802.7471121596177, 0.0));
    positionAlloc4->Add (Vector(729.2120151857292,167.26458180860755, 0.0));
    positionAlloc4->Add (Vector(182.64513032311328,452.6434168366921, 0.0));
    positionAlloc4->Add (Vector(226.60128338518552,339.32837393155614, 0.0));
    positionAlloc4->Add (Vector(438.30442577051616,651.2643424820594, 0.0));
    positionAlloc4->Add (Vector(457.1335544561408,313.85334278888064, 0.0));
    positionAlloc4->Add (Vector(698.741556775442,778.042891652254, 0.0));
    positionAlloc4->Add (Vector(448.83146112598604,837.34244915099, 0.0));
    positionAlloc4->Add (Vector(737.7405712982944,402.2646850895987, 0.0));
    positionAlloc4->Add (Vector(369.43270295387106,615.5542079988996, 0.0));
    positionAlloc4->Add (Vector(172.550752315261,473.7744603069943, 0.0));
    positionAlloc4->Add (Vector(282.83449070220763,798.5504333309549, 0.0));
    positionAlloc4->Add (Vector(622.6006981106939,269.16312270683136, 0.0));
    positionAlloc4->Add (Vector(253.81121436542298,748.7352912116203, 0.0));
    positionAlloc4->Add (Vector(589.6584336000049,738.2353667547582, 0.0));
    positionAlloc4->Add (Vector(503.20387752496043,791.5278440662396, 0.0));
    positionAlloc4->Add (Vector(538.863066691817,727.4569358721782, 0.0));
    positionAlloc4->Add (Vector(153.3598773721595,506.3795847422154, 0.0));
    positionAlloc4->Add (Vector(391.64282172784897,280.1311980893869, 0.0));
    positionAlloc4->Add (Vector(847.1937356622736,505.7101145921033, 0.0));
    positionAlloc4->Add (Vector(541.0770107560636,744.9830369767169, 0.0));
    positionAlloc4->Add (Vector(200.87873770183614,548.1731122365464, 0.0));
    positionAlloc4->Add (Vector(736.2939142734267,404.67752855840445, 0.0));
    positionAlloc4->Add (Vector(818.4355362545024,821.2332233916437, 0.0));
    positionAlloc4->Add (Vector(314.94963239978085,804.6134075227446, 0.0));
    positionAlloc4->Add (Vector(502.9237742366301,744.3956453127855, 0.0));
    positionAlloc4->Add (Vector(228.12223961632424,281.0015220235583, 0.0));
    positionAlloc4->Add (Vector(215.24974812488895,182.15102256036297, 0.0));
    positionAlloc4->Add (Vector(358.1104194579475,479.37904023218266, 0.0));
    positionAlloc4->Add (Vector(680.2376273180834,443.44000508951416, 0.0));
    positionAlloc4->Add (Vector(303.55764554568907,663.9528376702082, 0.0));
    positionAlloc4->Add (Vector(421.1333945996847,739.1842376679026, 0.0));
    positionAlloc4->Add (Vector(522.4839021432945,453.96568762797705, 0.0));
    positionAlloc4->Add (Vector(543.1427732990658,634.9658756943704, 0.0));
    positionAlloc4->Add (Vector(426.9689398674212,282.0192518901144, 0.0));
    positionAlloc4->Add (Vector(373.1588532253859,185.8892327242073, 0.0));
    positionAlloc4->Add (Vector(530.4426203960436,219.3395039511916, 0.0));
    positionAlloc4->Add (Vector(526.3539247913754,565.1741685785153, 0.0));
    positionAlloc4->Add (Vector(790.4148664321592,248.18704942141824, 0.0));
    positionAlloc4->Add (Vector(275.6698047455667,237.14922878756346, 0.0));
    positionAlloc4->Add (Vector(319.0249597394513,193.62240753442995, 0.0));
    positionAlloc4->Add (Vector(151.31092074341802,395.45449806781306, 0.0));
    positionAlloc4->Add (Vector(333.96999218705923,181.66074164415045, 0.0));
    positionAlloc4->Add (Vector(651.0950108687678,683.3788212180075, 0.0));
    positionAlloc4->Add (Vector(811.0325650238042,577.5239464680167, 0.0));
    positionAlloc4->Add (Vector(228.59093083023672,658.7183956605238, 0.0));
    positionAlloc4->Add (Vector(417.4243812878897,214.1467436230095, 0.0));
    positionAlloc4->Add (Vector(817.1326103118914,347.27797536977926, 0.0));
    positionAlloc4->Add (Vector(769.807135053614,196.54355662233655, 0.0));
    positionAlloc4->Add (Vector(806.4718756627934,152.99723048527255, 0.0));
    positionAlloc4->Add (Vector(607.6596565335317,846.5369267354731, 0.0));
    positionAlloc4->Add (Vector(579.42019864188,438.3717759270592, 0.0));
    positionAlloc4->Add (Vector(617.7816060457221,277.73343176946054, 0.0));
    positionAlloc4->Add (Vector(642.3386681632527,291.73499489896597, 0.0));
    positionAlloc4->Add (Vector(270.0590983536155,545.3944196997021, 0.0));
    positionAlloc4->Add (Vector(223.45742622956465,165.64275189880013, 0.0));
    positionAlloc4->Add (Vector(196.49148313700445,536.2797569565096, 0.0));
    positionAlloc4->Add (Vector(608.3819508758025,180.3776859092363, 0.0));
    positionAlloc4->Add (Vector(821.1606679811935,294.2121830039632, 0.0));
    positionAlloc4->Add (Vector(381.63600815461825,529.7120642540448, 0.0));
    positionAlloc4->Add (Vector(557.2538853443971,515.4514997961332, 0.0));
    positionAlloc4->Add (Vector(213.8753984975717,455.513879176179, 0.0));
    positionAlloc4->Add (Vector(306.10884898831466,517.9417196002587, 0.0));
    positionAlloc4->Add (Vector(828.672779074514,271.5475188377884, 0.0));
    positionAlloc4->Add (Vector(421.05249995544335,786.3286589199652, 0.0));
    positionAlloc4->Add (Vector(648.8045602730825,594.4660610326872, 0.0));
    positionAlloc4->Add (Vector(290.6095149841604,691.9823979027159, 0.0));
    positionAlloc4->Add (Vector(293.37487295309813,360.9974382589909, 0.0));
    positionAlloc4->Add (Vector(380.81177500234867,473.16142653299073, 0.0));
    positionAlloc4->Add (Vector(264.8801077170532,188.1657912147958, 0.0));
    positionAlloc4->Add (Vector(784.7196608007877,848.3552368628315, 0.0));
    positionAlloc4->Add (Vector(268.8876033108736,224.41407491906597, 0.0));
    positionAlloc4->Add (Vector(284.8032681427882,399.26277162832287, 0.0));
    positionAlloc4->Add (Vector(636.4772224240927,430.92781075885677, 0.0));
    positionAlloc4->Add (Vector(390.0545810856937,828.201534827452, 0.0));
    positionAlloc4->Add (Vector(309.0338772275513,152.87101530111696, 0.0));
    positionAlloc4->Add (Vector(488.79352523277794,641.303698776951, 0.0));
    positionAlloc4->Add (Vector(341.04520927291577,268.7627568797032, 0.0));
    positionAlloc4->Add (Vector(737.5596044765233,811.1415158916706, 0.0));
    positionAlloc4->Add (Vector(721.8779153324133,607.492507881635, 0.0));
    positionAlloc4->Add (Vector(815.3852603606294,587.7783409162889, 0.0));
    positionAlloc4->Add (Vector(276.7440293811751,530.9657303272922, 0.0));
    positionAlloc4->Add (Vector(432.6131028734511,753.6662691669918, 0.0));
    positionAlloc4->Add (Vector(790.2870204648502,622.108749956808, 0.0));
    positionAlloc4->Add (Vector(576.9870449701301,542.3314375214513, 0.0));
    positionAlloc4->Add (Vector(403.1517294285137,570.9633480102123, 0.0));
    positionAlloc4->Add (Vector(286.028447711642,831.7549018730992, 0.0));
    positionAlloc4->Add (Vector(707.5839878325489,351.7523080691069, 0.0));
    positionAlloc4->Add (Vector(724.5973132862644,343.01074588652915, 0.0));
    positionAlloc4->Add (Vector(342.20088697571765,172.7369254251263, 0.0));
    positionAlloc4->Add (Vector(179.5882959651542,560.7283576839716, 0.0));
    positionAlloc4->Add (Vector(849.1913790292418,555.4540246592906, 0.0));
    positionAlloc4->Add (Vector(799.3468094331031,435.1344443522825, 0.0));
    positionAlloc4->Add (Vector(836.9298037969099,512.8205806417657, 0.0));
    positionAlloc4->Add (Vector(273.94964534115786,542.365529426563, 0.0));
    positionAlloc4->Add (Vector(448.20056735168663,601.3017864703202, 0.0));
    positionAlloc4->Add (Vector(817.5005566846111,737.7371471030925, 0.0));
    positionAlloc4->Add (Vector(449.38503638134324,598.7836255363856, 0.0));
    positionAlloc4->Add (Vector(594.1079588178466,729.3755850424783, 0.0));
    positionAlloc4->Add (Vector(526.1343773901544,460.9009415540837, 0.0));
    positionAlloc4->Add (Vector(179.65335236694742,621.7611143511165, 0.0));
    positionAlloc4->Add (Vector(444.3763830482136,219.20643691921026, 0.0));
    positionAlloc4->Add (Vector(363.45424638836255,402.04584548550974, 0.0));
    positionAlloc4->Add (Vector(280.2059022991967,328.33115663989173, 0.0));
    positionAlloc4->Add (Vector(178.01374851730418,290.5958512357814, 0.0));
    positionAlloc4->Add (Vector(534.2570749630725,152.45759563745543, 0.0));
    positionAlloc4->Add (Vector(817.3046774564029,622.7482389060615, 0.0));
    positionAlloc4->Add (Vector(557.4849595738852,756.8115079594253, 0.0));
    positionAlloc4->Add (Vector(439.99379331291783,834.0280346164972, 0.0));
    positionAlloc4->Add (Vector(742.9328432093884,528.0460006989202, 0.0));
    positionAlloc4->Add (Vector(821.3049607351041,597.1174794024665, 0.0));
    positionAlloc4->Add (Vector(667.592539566106,276.7352523305033, 0.0));
    positionAlloc4->Add (Vector(545.877240233762,596.1133784998271, 0.0));
    positionAlloc4->Add (Vector(262.6559078416929,276.87346424630846, 0.0));
    positionAlloc4->Add (Vector(836.378212821948,825.9380059863148, 0.0));
    positionAlloc4->Add (Vector(157.06328351531707,632.4100622071558, 0.0));
    positionAlloc4->Add (Vector(813.3561571567623,408.0741966944013, 0.0));
    positionAlloc4->Add (Vector(832.0223460430899,162.02109346365867, 0.0));
    positionAlloc4->Add (Vector(406.51965243798577,180.24958968499882, 0.0));
    positionAlloc4->Add (Vector(635.0364060976566,757.3332063323927, 0.0));
    positionAlloc4->Add (Vector(418.4563171449173,808.1639350110227, 0.0));
    positionAlloc4->Add (Vector(202.46080888894778,227.1937097935968, 0.0));
    positionAlloc4->Add (Vector(786.084936412034,512.8852813142031, 0.0));
    positionAlloc4->Add (Vector(474.8946310391124,167.259238994825, 0.0));
    positionAlloc4->Add (Vector(811.412949337381,154.29005311957656, 0.0));
    positionAlloc4->Add (Vector(390.7724295084973,785.6924395924416, 0.0));
    positionAlloc4->Add (Vector(215.28208412658222,794.6203104811366, 0.0));
    positionAlloc4->Add (Vector(278.6172480896995,508.04073844618347, 0.0));
    positionAlloc4->Add (Vector(267.885564307021,659.4991076351027, 0.0));
    positionAlloc4->Add (Vector(193.71454896674726,174.61249930204707, 0.0));
    positionAlloc4->Add (Vector(837.5851929489874,646.0342803045917, 0.0));
    positionAlloc4->Add (Vector(720.5054438886303,376.2328493822554, 0.0));
    positionAlloc4->Add (Vector(502.2893136704091,798.3498737789981, 0.0));
    positionAlloc4->Add (Vector(587.0348685858439,316.3348522922943, 0.0));
    positionAlloc4->Add (Vector(448.2701077866021,661.3963893033833, 0.0));
    positionAlloc4->Add (Vector(192.5384878773686,316.0036512655515, 0.0));
    positionAlloc4->Add (Vector(726.6883606198356,289.6238807831364, 0.0));
    positionAlloc4->Add (Vector(152.4532644717408,169.34082906589057, 0.0));
    positionAlloc4->Add (Vector(389.70296586766375,848.7385155015003, 0.0));
    positionAlloc4->Add (Vector(831.2825203547295,775.1971028656035, 0.0));
    positionAlloc4->Add (Vector(444.5321454435225,644.6860662397603, 0.0));
    positionAlloc4->Add (Vector(581.090726094865,354.63066108984344, 0.0));
    positionAlloc4->Add (Vector(461.0676536107454,744.595588618423, 0.0));
    positionAlloc4->Add (Vector(769.1299195028693,209.7581455285957, 0.0));
    positionAlloc4->Add (Vector(519.5368795607469,301.1479206572703, 0.0));
    positionAlloc4->Add (Vector(335.73973236485324,367.0152923512717, 0.0));
    positionAlloc4->Add (Vector(162.8440631715428,678.9772439346639, 0.0));
    positionAlloc4->Add (Vector(347.8379241660452,621.3945601728094, 0.0));
    positionAlloc4->Add (Vector(268.9180126125774,349.7084046231597, 0.0));
    positionAlloc4->Add (Vector(540.6342829406412,189.63048119430638, 0.0));
    positionAlloc4->Add (Vector(610.5655796835899,652.405578320444, 0.0));
    positionAlloc4->Add (Vector(523.6044629464565,386.9363572575844, 0.0));
    positionAlloc4->Add (Vector(466.47154403502,722.2366545630982, 0.0));
    positionAlloc4->Add (Vector(592.3939408920282,710.4849606417118, 0.0));
    positionAlloc4->Add (Vector(458.3756536863648,513.6169365622894, 0.0));
    positionAlloc4->Add (Vector(449.9946864639881,400.85734263267886, 0.0));
    positionAlloc4->Add (Vector(448.4729263385976,807.6366214004129, 0.0));
    positionAlloc4->Add (Vector(759.6233990186666,299.4230612972782, 0.0));
    positionAlloc4->Add (Vector(189.1758900834049,504.5227831369559, 0.0));
    positionAlloc4->Add (Vector(421.2964368098825,579.1506169179005, 0.0));
    positionAlloc4->Add (Vector(744.6806316878855,499.8755804240123, 0.0));
    positionAlloc4->Add (Vector(847.3027816533369,231.58122699904283, 0.0));
    positionAlloc4->Add (Vector(536.9232133176924,827.3529649129661, 0.0));
    positionAlloc4->Add (Vector(700.4166868971959,773.0738314098928, 0.0));
    positionAlloc4->Add (Vector(552.1036301074508,719.1841908349936, 0.0));
    positionAlloc4->Add (Vector(754.7067291917458,567.361755889645, 0.0));
    positionAlloc4->Add (Vector(177.28065477789616,743.453189254012, 0.0));
    positionAlloc4->Add (Vector(770.4070213259798,340.03895224990094, 0.0));
    positionAlloc4->Add (Vector(771.1037012441307,157.67614123951307, 0.0));
    positionAlloc4->Add (Vector(495.65922700102226,761.7986081291582, 0.0));
    positionAlloc4->Add (Vector(412.52848876814704,224.84792171307134, 0.0));
    positionAlloc4->Add (Vector(267.1572715086559,834.9196705289987, 0.0));
    positionAlloc4->Add (Vector(611.9470066813419,512.3536623123235, 0.0));
    positionAlloc4->Add (Vector(817.5595156742899,698.5359197104009, 0.0));
    positionAlloc4->Add (Vector(387.6953918944701,367.5923088621753, 0.0));
    positionAlloc4->Add (Vector(229.75672818958972,208.76616304984867, 0.0));
    positionAlloc4->Add (Vector(537.0829102122079,491.9928124093891, 0.0));
    positionAlloc4->Add (Vector(678.6001350812991,712.6263820128571, 0.0));
    positionAlloc4->Add (Vector(532.5595960908424,293.68027081538906, 0.0));
    positionAlloc4->Add (Vector(586.280256771192,616.4137261898046, 0.0));
    positionAlloc4->Add (Vector(820.8440085416596,454.50834920623265, 0.0));
    positionAlloc4->Add (Vector(409.8218844823791,202.74842736022427, 0.0));
    positionAlloc4->Add (Vector(648.9026020709847,767.3913135362908, 0.0));
    positionAlloc4->Add (Vector(559.4552828373335,615.642872436747, 0.0));
    positionAlloc4->Add (Vector(388.07625555200815,275.0942846528262, 0.0));
    positionAlloc4->Add (Vector(514.9954296438268,695.291954129726, 0.0));
    positionAlloc4->Add (Vector(514.0629563240841,430.48949687027704, 0.0));
    positionAlloc4->Add (Vector(383.9724619099919,821.890398716963, 0.0));
    positionAlloc4->Add (Vector(477.64582229836185,302.23904770408467, 0.0));
    positionAlloc4->Add (Vector(710.2274015845425,587.162610755043, 0.0));
    positionAlloc4->Add (Vector(383.95070535953187,295.5218478734979, 0.0));
    positionAlloc4->Add (Vector(763.7935987760195,519.2601639219813, 0.0));
    positionAlloc4->Add (Vector(307.54084186027046,437.5439626670034, 0.0));
    positionAlloc4->Add (Vector(208.7317792797386,511.65744248287, 0.0));
    positionAlloc4->Add (Vector(810.2141894973445,768.8490765910119, 0.0));
    positionAlloc4->Add (Vector(464.80911582664737,344.94440235017066, 0.0));
    positionAlloc4->Add (Vector(468.5077226148701,254.7457050562487, 0.0));
    positionAlloc4->Add (Vector(747.8005764474583,550.1478062891035, 0.0));
    positionAlloc4->Add (Vector(284.640209532515,339.50593073718835, 0.0));
    positionAlloc4->Add (Vector(793.1964358812368,430.41676187369063, 0.0));
    positionAlloc4->Add (Vector(360.41378594664286,609.6353646311904, 0.0));
    positionAlloc4->Add (Vector(411.3134930860212,442.26596427527124, 0.0));
    positionAlloc4->Add (Vector(396.73280911698964,271.47584374721845, 0.0));
    positionAlloc4->Add (Vector(412.84806892513467,227.52395548135632, 0.0));
    positionAlloc4->Add (Vector(685.1946026399714,166.45169597282683, 0.0));
    positionAlloc4->Add (Vector(803.2810673065977,286.9889775121277, 0.0));
    positionAlloc4->Add (Vector(251.25843140488377,577.850031839181, 0.0));
    positionAlloc4->Add (Vector(252.7937183513983,535.72194687611, 0.0));
    positionAlloc4->Add (Vector(448.87633949685346,508.87876741495995, 0.0));
    positionAlloc4->Add (Vector(704.5754901148127,823.001918668336, 0.0));
    positionAlloc4->Add (Vector(151.76928488233625,220.80574054290003, 0.0));
    positionAlloc4->Add (Vector(358.4509512605092,188.84350474974246, 0.0));
    positionAlloc4->Add (Vector(351.14756321939774,564.7404832423475, 0.0));
    positionAlloc4->Add (Vector(486.9172649908053,567.664034852737, 0.0));
    positionAlloc4->Add (Vector(315.9541012893502,376.5639467250602, 0.0));
    positionAlloc4->Add (Vector(721.9594628437546,727.4628506223902, 0.0));
    positionAlloc4->Add (Vector(637.3844327103732,186.9772511388957, 0.0));
    positionAlloc4->Add (Vector(541.1063857487867,190.63243591295267, 0.0));
    positionAlloc4->Add (Vector(621.8183253882929,454.48892541932406, 0.0));
    positionAlloc4->Add (Vector(726.6583308045239,486.7357892334228, 0.0));
    positionAlloc4->Add (Vector(439.5513283917787,816.6944863274193, 0.0));
    positionAlloc4->Add (Vector(704.1017362510195,818.7899153222676, 0.0));
    positionAlloc4->Add (Vector(354.90710648784034,272.04969535561014, 0.0));
    positionAlloc4->Add (Vector(468.434622017678,401.4617984352061, 0.0));
    positionAlloc4->Add (Vector(175.37842680906917,838.8127051374523, 0.0));
    positionAlloc4->Add (Vector(529.8908730796516,709.307259946651, 0.0));
    positionAlloc4->Add (Vector(245.32189965309985,325.26113549488434, 0.0));
    positionAlloc4->Add (Vector(626.0427198147856,778.0978704948652, 0.0));
    positionAlloc4->Add (Vector(683.6231331857831,486.9490824754966, 0.0));
    positionAlloc4->Add (Vector(274.79236582542853,797.1010642489836, 0.0));
    positionAlloc4->Add (Vector(477.4866091873005,443.57288657787257, 0.0));
    positionAlloc4->Add (Vector(274.9730331778004,413.30311700734313, 0.0));
    positionAlloc4->Add (Vector(652.3923531965359,577.9138619387707, 0.0));
    positionAlloc4->Add (Vector(359.3450167261902,401.41475704376717, 0.0));
    positionAlloc4->Add (Vector(589.3412484312515,804.675220076033, 0.0));
    positionAlloc4->Add (Vector(683.1724337759285,531.992111223064, 0.0));
    positionAlloc4->Add (Vector(443.9087136092148,579.2983427893291, 0.0));
    positionAlloc4->Add (Vector(255.5158951218179,584.391436793299, 0.0));
    positionAlloc4->Add (Vector(299.3426021920885,377.77329649286764, 0.0));
    positionAlloc4->Add (Vector(411.32078445615866,762.2035337027405, 0.0));
    positionAlloc4->Add (Vector(609.6598696359916,735.9054570621821, 0.0));
    positionAlloc4->Add (Vector(271.24254153193846,188.08708942512456, 0.0));
    positionAlloc4->Add (Vector(169.7994498036858,245.94755977831363, 0.0));
    positionAlloc4->Add (Vector(272.9308740318165,410.3271996788232, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_17.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_ECC_flowmon/switch_ECC_17.flowmon", false, false);
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