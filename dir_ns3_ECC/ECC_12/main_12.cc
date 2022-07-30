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
	  uint16_t numberOfRrhs = 74;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 454;
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
    positionAlloc->Add (Vector (850.0,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (850.0,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,850.0, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (772.2222222222222,150.0, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (694.4444444444445,150.0, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (616.6666666666667,461.1111111111111, 0.0));
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
    positionAlloc->Add (Vector (461.1111111111111,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (461.1111111111111,227.77777777777777, 0.0));
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
    positionAlloc->Add (Vector (227.77777777777777,850.0, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,538.8888888888889, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,305.55555555555554, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,227.77777777777777, 0.0));
    positionAlloc->Add (Vector (227.77777777777777,150.0, 0.0));
    positionAlloc->Add (Vector (150.0,772.2222222222222, 0.0));
    positionAlloc->Add (Vector (150.0,694.4444444444445, 0.0));
    positionAlloc->Add (Vector (150.0,616.6666666666667, 0.0));
    positionAlloc->Add (Vector (150.0,461.1111111111111, 0.0));
    positionAlloc->Add (Vector (150.0,383.33333333333337, 0.0));
    positionAlloc->Add (Vector (150.0,305.55555555555554, 0.0));
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
    positionAlloc4->Add (Vector(615.6909853658376,205.08900369525662, 0.0));
    positionAlloc4->Add (Vector(790.231518160634,296.0563392109017, 0.0));
    positionAlloc4->Add (Vector(211.30219409036675,593.6948731147268, 0.0));
    positionAlloc4->Add (Vector(150.55265776525314,717.0980635218228, 0.0));
    positionAlloc4->Add (Vector(519.2195246052124,248.7882828095253, 0.0));
    positionAlloc4->Add (Vector(803.5752342930546,529.6705921333071, 0.0));
    positionAlloc4->Add (Vector(217.28809621250605,447.2008250301689, 0.0));
    positionAlloc4->Add (Vector(782.1354571717608,247.80439688526786, 0.0));
    positionAlloc4->Add (Vector(153.744292085956,673.7671643266301, 0.0));
    positionAlloc4->Add (Vector(537.8408684221475,801.5830049230652, 0.0));
    positionAlloc4->Add (Vector(448.12520852567724,839.8668932786526, 0.0));
    positionAlloc4->Add (Vector(306.27878074855266,795.6154428385094, 0.0));
    positionAlloc4->Add (Vector(200.66347523588573,817.8921490997508, 0.0));
    positionAlloc4->Add (Vector(315.94927382192463,463.0910777099854, 0.0));
    positionAlloc4->Add (Vector(173.39857316525513,406.7264534981581, 0.0));
    positionAlloc4->Add (Vector(526.1654333106985,738.5495503573802, 0.0));
    positionAlloc4->Add (Vector(537.9268788141592,683.4229047273358, 0.0));
    positionAlloc4->Add (Vector(184.4125804085337,171.73981322460892, 0.0));
    positionAlloc4->Add (Vector(601.3039841672723,398.70275025352316, 0.0));
    positionAlloc4->Add (Vector(847.6782945542758,675.5531618258514, 0.0));
    positionAlloc4->Add (Vector(390.7857454214591,344.0808914444303, 0.0));
    positionAlloc4->Add (Vector(440.1008687269256,632.6739462337906, 0.0));
    positionAlloc4->Add (Vector(805.1959253012444,624.6857510756524, 0.0));
    positionAlloc4->Add (Vector(329.6631829271401,565.0893234288458, 0.0));
    positionAlloc4->Add (Vector(200.65008041892864,766.773603849596, 0.0));
    positionAlloc4->Add (Vector(364.36747165988027,736.488966633863, 0.0));
    positionAlloc4->Add (Vector(673.0572515620108,368.19232504496836, 0.0));
    positionAlloc4->Add (Vector(215.56021682523215,687.6625648930666, 0.0));
    positionAlloc4->Add (Vector(580.4321101968553,487.6965211163102, 0.0));
    positionAlloc4->Add (Vector(806.2852361851369,520.9129378627442, 0.0));
    positionAlloc4->Add (Vector(201.49434795103596,187.40422167371202, 0.0));
    positionAlloc4->Add (Vector(674.5207485997041,825.2833351799227, 0.0));
    positionAlloc4->Add (Vector(512.3408798785771,554.5139570086365, 0.0));
    positionAlloc4->Add (Vector(767.9855434746554,519.4684758069345, 0.0));
    positionAlloc4->Add (Vector(688.8406151699099,542.9490769325366, 0.0));
    positionAlloc4->Add (Vector(759.6773199037798,389.6338577226935, 0.0));
    positionAlloc4->Add (Vector(599.9614351134578,634.9608390409361, 0.0));
    positionAlloc4->Add (Vector(400.25932989906585,344.19871436068536, 0.0));
    positionAlloc4->Add (Vector(694.514393875814,500.2508522690731, 0.0));
    positionAlloc4->Add (Vector(176.85308835158224,585.0250958597492, 0.0));
    positionAlloc4->Add (Vector(483.8943407647225,599.5947144549805, 0.0));
    positionAlloc4->Add (Vector(651.4277404332197,618.5697965689085, 0.0));
    positionAlloc4->Add (Vector(676.6864632484064,677.31905231548, 0.0));
    positionAlloc4->Add (Vector(201.2719228751731,718.6221328998832, 0.0));
    positionAlloc4->Add (Vector(200.5605437907345,253.2171064863257, 0.0));
    positionAlloc4->Add (Vector(373.7125983819988,564.2655353643281, 0.0));
    positionAlloc4->Add (Vector(510.19978138746035,290.10794787455677, 0.0));
    positionAlloc4->Add (Vector(265.3024228600866,632.4003498862364, 0.0));
    positionAlloc4->Add (Vector(456.80902562321614,333.8853169160036, 0.0));
    positionAlloc4->Add (Vector(465.0169676694618,382.883125784869, 0.0));
    positionAlloc4->Add (Vector(189.43329445946785,166.71893392339993, 0.0));
    positionAlloc4->Add (Vector(378.6353340236061,586.3201036313403, 0.0));
    positionAlloc4->Add (Vector(617.1458141352772,180.69279674764093, 0.0));
    positionAlloc4->Add (Vector(377.906792763308,838.518109817142, 0.0));
    positionAlloc4->Add (Vector(492.0234481145893,572.2554949605747, 0.0));
    positionAlloc4->Add (Vector(780.8243362521489,675.4683099971584, 0.0));
    positionAlloc4->Add (Vector(772.7811888001654,458.3041994592323, 0.0));
    positionAlloc4->Add (Vector(509.4041479880741,605.4245662764724, 0.0));
    positionAlloc4->Add (Vector(260.06496556596346,633.435919456939, 0.0));
    positionAlloc4->Add (Vector(752.7233799805682,546.1041514917337, 0.0));
    positionAlloc4->Add (Vector(829.8539383198461,456.62355968835277, 0.0));
    positionAlloc4->Add (Vector(374.3753167645469,671.0934776693282, 0.0));
    positionAlloc4->Add (Vector(321.69551348205715,463.885488711153, 0.0));
    positionAlloc4->Add (Vector(781.3412754865179,428.0308204219902, 0.0));
    positionAlloc4->Add (Vector(369.2274354925446,275.1887479554596, 0.0));
    positionAlloc4->Add (Vector(385.5152462722571,297.84578101156615, 0.0));
    positionAlloc4->Add (Vector(281.53251215889156,587.3567791031077, 0.0));
    positionAlloc4->Add (Vector(374.126817199907,619.1777832310514, 0.0));
    positionAlloc4->Add (Vector(556.1599725393992,782.8244905181915, 0.0));
    positionAlloc4->Add (Vector(617.613616758792,273.67761992341366, 0.0));
    positionAlloc4->Add (Vector(848.1960573374879,346.75171981157916, 0.0));
    positionAlloc4->Add (Vector(296.5846997649535,158.2459134006494, 0.0));
    positionAlloc4->Add (Vector(232.2900743846556,843.1557742305517, 0.0));
    positionAlloc4->Add (Vector(657.4389226371077,776.3333909087465, 0.0));
    positionAlloc4->Add (Vector(284.4113756775642,737.99875627076, 0.0));
    positionAlloc4->Add (Vector(515.3036067325252,201.92333691601854, 0.0));
    positionAlloc4->Add (Vector(442.21379872752374,661.4753080698731, 0.0));
    positionAlloc4->Add (Vector(524.1881813292274,622.5909200910937, 0.0));
    positionAlloc4->Add (Vector(150.89909786876098,234.11148111493537, 0.0));
    positionAlloc4->Add (Vector(620.2454891784062,426.038802445465, 0.0));
    positionAlloc4->Add (Vector(791.6501718857413,643.8476563542367, 0.0));
    positionAlloc4->Add (Vector(556.557780897433,650.0712934261046, 0.0));
    positionAlloc4->Add (Vector(155.848292920497,268.0193339038258, 0.0));
    positionAlloc4->Add (Vector(253.6705590389892,672.4196687983538, 0.0));
    positionAlloc4->Add (Vector(407.0187379865159,529.6099829347554, 0.0));
    positionAlloc4->Add (Vector(724.4490922481351,176.38817329586942, 0.0));
    positionAlloc4->Add (Vector(793.9096538913799,188.60961342124983, 0.0));
    positionAlloc4->Add (Vector(500.21360944816035,775.8696752743115, 0.0));
    positionAlloc4->Add (Vector(229.37096688680077,654.4314034646005, 0.0));
    positionAlloc4->Add (Vector(664.8644940555068,614.6541367590108, 0.0));
    positionAlloc4->Add (Vector(520.0089410062096,717.4226937821128, 0.0));
    positionAlloc4->Add (Vector(282.04257249685236,505.52819120236666, 0.0));
    positionAlloc4->Add (Vector(591.4120881159804,464.7192342054161, 0.0));
    positionAlloc4->Add (Vector(613.3788876154439,475.3983465692895, 0.0));
    positionAlloc4->Add (Vector(811.9714759355633,436.2490650296704, 0.0));
    positionAlloc4->Add (Vector(730.7506637470499,642.7556669241608, 0.0));
    positionAlloc4->Add (Vector(695.672177276753,326.81020467930693, 0.0));
    positionAlloc4->Add (Vector(266.780793465184,182.67937392949165, 0.0));
    positionAlloc4->Add (Vector(563.6412649790587,842.4516783944102, 0.0));
    positionAlloc4->Add (Vector(284.31122908315103,192.85166070050042, 0.0));
    positionAlloc4->Add (Vector(433.2576191539851,425.577320193429, 0.0));
    positionAlloc4->Add (Vector(702.328422941433,395.68293674826396, 0.0));
    positionAlloc4->Add (Vector(738.3099253822141,640.9053394618688, 0.0));
    positionAlloc4->Add (Vector(307.2076678577307,500.1385548659497, 0.0));
    positionAlloc4->Add (Vector(209.25412750924028,233.4901704278204, 0.0));
    positionAlloc4->Add (Vector(598.3383051995486,331.6144197476534, 0.0));
    positionAlloc4->Add (Vector(559.7627959536694,533.331270457623, 0.0));
    positionAlloc4->Add (Vector(398.3920064014608,578.8790412722251, 0.0));
    positionAlloc4->Add (Vector(542.2440720452264,660.0306097549438, 0.0));
    positionAlloc4->Add (Vector(408.56785330780957,504.06645581463084, 0.0));
    positionAlloc4->Add (Vector(700.8830595668406,189.4672976972621, 0.0));
    positionAlloc4->Add (Vector(349.67103961473657,317.44776162395317, 0.0));
    positionAlloc4->Add (Vector(576.6274343566354,724.4035075336948, 0.0));
    positionAlloc4->Add (Vector(165.79800824166395,229.00070129893356, 0.0));
    positionAlloc4->Add (Vector(735.8494829912365,846.8385275132539, 0.0));
    positionAlloc4->Add (Vector(574.68252623852,801.3140203815218, 0.0));
    positionAlloc4->Add (Vector(440.49843085952796,378.8594701782865, 0.0));
    positionAlloc4->Add (Vector(539.7728521948153,812.4650573508596, 0.0));
    positionAlloc4->Add (Vector(237.56223688548246,240.8480760058279, 0.0));
    positionAlloc4->Add (Vector(530.0146926525636,733.0255164210606, 0.0));
    positionAlloc4->Add (Vector(298.9950602268502,618.1272781148696, 0.0));
    positionAlloc4->Add (Vector(200.80235863129755,745.2223957103192, 0.0));
    positionAlloc4->Add (Vector(752.7794640429979,204.83675243340468, 0.0));
    positionAlloc4->Add (Vector(385.7933910130069,548.6376716956142, 0.0));
    positionAlloc4->Add (Vector(226.5491321766352,428.103905062134, 0.0));
    positionAlloc4->Add (Vector(569.4942624765142,715.2345549563363, 0.0));
    positionAlloc4->Add (Vector(444.6275701314582,822.9923044926013, 0.0));
    positionAlloc4->Add (Vector(432.55268614543445,625.0620656788279, 0.0));
    positionAlloc4->Add (Vector(457.6257620509095,497.4259398990652, 0.0));
    positionAlloc4->Add (Vector(390.0765637900547,246.77189594383998, 0.0));
    positionAlloc4->Add (Vector(760.0862995207734,462.71766771797496, 0.0));
    positionAlloc4->Add (Vector(617.1908713736818,158.93579856252816, 0.0));
    positionAlloc4->Add (Vector(428.775164404389,498.01709650237996, 0.0));
    positionAlloc4->Add (Vector(759.5580206366792,508.9249298557507, 0.0));
    positionAlloc4->Add (Vector(722.5329237827076,176.8547029489908, 0.0));
    positionAlloc4->Add (Vector(579.9181952232249,383.9001156687035, 0.0));
    positionAlloc4->Add (Vector(735.6341472573607,605.5518245928313, 0.0));
    positionAlloc4->Add (Vector(411.4112138018048,418.47415870378387, 0.0));
    positionAlloc4->Add (Vector(789.670165837461,407.80682735899666, 0.0));
    positionAlloc4->Add (Vector(485.61848470483596,407.09439089649646, 0.0));
    positionAlloc4->Add (Vector(307.6449701513411,329.96726257657826, 0.0));
    positionAlloc4->Add (Vector(575.4339381203972,433.2941120304191, 0.0));
    positionAlloc4->Add (Vector(759.9335146381977,849.2332898341537, 0.0));
    positionAlloc4->Add (Vector(337.5115079722085,572.8034984176501, 0.0));
    positionAlloc4->Add (Vector(525.4872681560213,456.00367353152427, 0.0));
    positionAlloc4->Add (Vector(605.5795653425296,231.39157620501646, 0.0));
    positionAlloc4->Add (Vector(540.1973212690993,329.2148211430591, 0.0));
    positionAlloc4->Add (Vector(387.76243433700995,645.5634786908323, 0.0));
    positionAlloc4->Add (Vector(243.9416984284452,601.329708613065, 0.0));
    positionAlloc4->Add (Vector(597.484018226954,202.6000672243106, 0.0));
    positionAlloc4->Add (Vector(345.7210028515923,239.1856501868003, 0.0));
    positionAlloc4->Add (Vector(297.49466555243436,309.2737138555243, 0.0));
    positionAlloc4->Add (Vector(166.1329618846252,777.7225849354212, 0.0));
    positionAlloc4->Add (Vector(285.4130596083562,297.2604570708851, 0.0));
    positionAlloc4->Add (Vector(350.35701506799603,472.07183859038156, 0.0));
    positionAlloc4->Add (Vector(614.6140524857569,616.0490810282097, 0.0));
    positionAlloc4->Add (Vector(220.43398405581306,366.74226700199205, 0.0));
    positionAlloc4->Add (Vector(478.4242472649402,407.39595568928024, 0.0));
    positionAlloc4->Add (Vector(423.2635122328677,441.042360692756, 0.0));
    positionAlloc4->Add (Vector(287.3639971814989,633.0527479321584, 0.0));
    positionAlloc4->Add (Vector(210.13727174197433,649.8833986474411, 0.0));
    positionAlloc4->Add (Vector(363.33263895812803,277.2659259668648, 0.0));
    positionAlloc4->Add (Vector(489.6789229030688,530.3840965387849, 0.0));
    positionAlloc4->Add (Vector(327.90309608947007,727.5944129629934, 0.0));
    positionAlloc4->Add (Vector(446.92953902218335,462.68210522126543, 0.0));
    positionAlloc4->Add (Vector(273.0006720857781,567.9816340108523, 0.0));
    positionAlloc4->Add (Vector(618.7278795762624,829.4676194557094, 0.0));
    positionAlloc4->Add (Vector(404.5280144330447,301.7699931503758, 0.0));
    positionAlloc4->Add (Vector(550.6854081774356,433.1636671207645, 0.0));
    positionAlloc4->Add (Vector(195.92053546152067,633.4307186706454, 0.0));
    positionAlloc4->Add (Vector(630.2169567551236,324.9312051458229, 0.0));
    positionAlloc4->Add (Vector(263.748890832109,776.5237011974058, 0.0));
    positionAlloc4->Add (Vector(344.7520309481755,193.12558970472847, 0.0));
    positionAlloc4->Add (Vector(717.1813913621008,525.6047330200342, 0.0));
    positionAlloc4->Add (Vector(604.9047451918025,167.2881083802779, 0.0));
    positionAlloc4->Add (Vector(350.38132476822824,446.66572962150195, 0.0));
    positionAlloc4->Add (Vector(216.3172403455409,833.1096039484756, 0.0));
    positionAlloc4->Add (Vector(631.2058026586071,377.7927964778255, 0.0));
    positionAlloc4->Add (Vector(840.9700601468816,542.8051487046221, 0.0));
    positionAlloc4->Add (Vector(451.68814399308087,272.3507005316106, 0.0));
    positionAlloc4->Add (Vector(562.1671891093563,212.455517640067, 0.0));
    positionAlloc4->Add (Vector(519.1176269496752,431.9498738325465, 0.0));
    positionAlloc4->Add (Vector(234.67246356694463,205.08837263677935, 0.0));
    positionAlloc4->Add (Vector(219.6473583264467,572.9217567208143, 0.0));
    positionAlloc4->Add (Vector(621.9308641807297,593.2882421362422, 0.0));
    positionAlloc4->Add (Vector(282.3543576448442,780.6354111868935, 0.0));
    positionAlloc4->Add (Vector(495.27324511819006,555.3908628245345, 0.0));
    positionAlloc4->Add (Vector(486.0944577468612,411.2451583735888, 0.0));
    positionAlloc4->Add (Vector(528.4276960273103,744.7201224877207, 0.0));
    positionAlloc4->Add (Vector(643.1455454881793,467.6925946648381, 0.0));
    positionAlloc4->Add (Vector(287.9742271765565,762.7542679305325, 0.0));
    positionAlloc4->Add (Vector(839.4503759090442,812.0423019059226, 0.0));
    positionAlloc4->Add (Vector(267.9083857923407,682.3252243932467, 0.0));
    positionAlloc4->Add (Vector(497.4065878504478,531.122932504418, 0.0));
    positionAlloc4->Add (Vector(380.52489423775296,179.5403280069089, 0.0));
    positionAlloc4->Add (Vector(644.6497652242538,808.4479994302797, 0.0));
    positionAlloc4->Add (Vector(468.46554378939976,479.5494979088951, 0.0));
    positionAlloc4->Add (Vector(508.80335114694816,337.4420202228822, 0.0));
    positionAlloc4->Add (Vector(635.7043396298761,492.525923232585, 0.0));
    positionAlloc4->Add (Vector(810.8288947966493,427.3940837441904, 0.0));
    positionAlloc4->Add (Vector(607.9650459617226,523.5318519164475, 0.0));
    positionAlloc4->Add (Vector(184.84670106613063,337.9500533382468, 0.0));
    positionAlloc4->Add (Vector(396.6162189464248,313.8404425122494, 0.0));
    positionAlloc4->Add (Vector(809.529818142747,512.9450824595683, 0.0));
    positionAlloc4->Add (Vector(300.6706743506676,753.1374650587849, 0.0));
    positionAlloc4->Add (Vector(282.39708742871807,551.4442347236836, 0.0));
    positionAlloc4->Add (Vector(300.1953740649949,666.7055283891827, 0.0));
    positionAlloc4->Add (Vector(197.60716236669435,285.65239101634836, 0.0));
    positionAlloc4->Add (Vector(487.6493460079271,642.0065108350709, 0.0));
    positionAlloc4->Add (Vector(322.19981821475665,522.4607767566079, 0.0));
    positionAlloc4->Add (Vector(459.9151493945208,814.2413527321006, 0.0));
    positionAlloc4->Add (Vector(177.50436160548472,608.4349862035014, 0.0));
    positionAlloc4->Add (Vector(196.48374742234816,842.5446598294275, 0.0));
    positionAlloc4->Add (Vector(362.2967182458117,403.80824881627393, 0.0));
    positionAlloc4->Add (Vector(826.8058852053755,518.6164598128526, 0.0));
    positionAlloc4->Add (Vector(702.5034582739733,171.8494922371233, 0.0));
    positionAlloc4->Add (Vector(733.0958969871003,358.2995015085708, 0.0));
    positionAlloc4->Add (Vector(178.40999849428655,789.6092458045215, 0.0));
    positionAlloc4->Add (Vector(342.29352296500053,771.4260672701816, 0.0));
    positionAlloc4->Add (Vector(240.32548622259742,451.22149962967364, 0.0));
    positionAlloc4->Add (Vector(404.8847397890538,597.5704950178686, 0.0));
    positionAlloc4->Add (Vector(260.96416381918664,491.2274848247818, 0.0));
    positionAlloc4->Add (Vector(262.49934443369,834.7858244706318, 0.0));
    positionAlloc4->Add (Vector(224.29101351631363,494.94694210546896, 0.0));
    positionAlloc4->Add (Vector(655.661455342959,204.59570648658635, 0.0));
    positionAlloc4->Add (Vector(813.897608879045,739.7248747849026, 0.0));
    positionAlloc4->Add (Vector(531.4171802205753,348.45503318965757, 0.0));
    positionAlloc4->Add (Vector(295.47668902118505,502.849055823897, 0.0));
    positionAlloc4->Add (Vector(248.34930685686214,585.7845773491797, 0.0));
    positionAlloc4->Add (Vector(549.2187554785528,187.50359172518554, 0.0));
    positionAlloc4->Add (Vector(680.8693401129425,574.209682355257, 0.0));
    positionAlloc4->Add (Vector(361.0193304874676,216.18944134207175, 0.0));
    positionAlloc4->Add (Vector(451.12313591101923,762.5060898207792, 0.0));
    positionAlloc4->Add (Vector(599.105776870376,166.40306875717175, 0.0));
    positionAlloc4->Add (Vector(374.49851548900574,824.741208451232, 0.0));
    positionAlloc4->Add (Vector(489.4011178012725,398.49658106975926, 0.0));
    positionAlloc4->Add (Vector(284.0427304356836,420.3654767093627, 0.0));
    positionAlloc4->Add (Vector(714.1238184377979,205.47472429095018, 0.0));
    positionAlloc4->Add (Vector(618.9674436014527,150.28108081773792, 0.0));
    positionAlloc4->Add (Vector(329.40466275845233,283.2894472174371, 0.0));
    positionAlloc4->Add (Vector(654.261248912542,461.13598458445375, 0.0));
    positionAlloc4->Add (Vector(185.35522958259747,180.71676185812052, 0.0));
    positionAlloc4->Add (Vector(374.4639283155026,536.636609418852, 0.0));
    positionAlloc4->Add (Vector(335.69956236023665,782.7671456329225, 0.0));
    positionAlloc4->Add (Vector(747.3770134758591,790.0383098665385, 0.0));
    positionAlloc4->Add (Vector(264.0318581341758,718.6895603367303, 0.0));
    positionAlloc4->Add (Vector(617.9826865797897,753.9395442214579, 0.0));
    positionAlloc4->Add (Vector(349.1583718197109,182.9428385138632, 0.0));
    positionAlloc4->Add (Vector(233.83197650253805,522.3530842869541, 0.0));
    positionAlloc4->Add (Vector(723.5994975692555,783.9074963011925, 0.0));
    positionAlloc4->Add (Vector(613.33110353035,453.83420793472646, 0.0));
    positionAlloc4->Add (Vector(579.219252520182,758.1607670724742, 0.0));
    positionAlloc4->Add (Vector(386.9878192059359,481.4044535158341, 0.0));
    positionAlloc4->Add (Vector(182.2484187728273,530.7145448127823, 0.0));
    positionAlloc4->Add (Vector(156.51720353364757,459.6756185237355, 0.0));
    positionAlloc4->Add (Vector(813.1995986418552,446.3506191455502, 0.0));
    positionAlloc4->Add (Vector(751.5274657493333,227.01240478928054, 0.0));
    positionAlloc4->Add (Vector(776.5596209364202,830.4374322364417, 0.0));
    positionAlloc4->Add (Vector(242.09571464754822,361.593690347514, 0.0));
    positionAlloc4->Add (Vector(812.252932648182,321.30091791659066, 0.0));
    positionAlloc4->Add (Vector(806.3932307655574,788.578192770809, 0.0));
    positionAlloc4->Add (Vector(546.0231145858022,650.2916214675804, 0.0));
    positionAlloc4->Add (Vector(253.21774219829445,578.1589729522099, 0.0));
    positionAlloc4->Add (Vector(337.0469268402519,506.37936963126543, 0.0));
    positionAlloc4->Add (Vector(469.42177438567364,269.5377820023358, 0.0));
    positionAlloc4->Add (Vector(418.21512799138804,706.2640246785223, 0.0));
    positionAlloc4->Add (Vector(326.9361019658148,434.6455688194793, 0.0));
    positionAlloc4->Add (Vector(803.7911929588321,278.5057391486022, 0.0));
    positionAlloc4->Add (Vector(512.8797386431456,228.82646054445448, 0.0));
    positionAlloc4->Add (Vector(408.31241268782094,244.8618114925278, 0.0));
    positionAlloc4->Add (Vector(802.7107848641456,813.1148294658734, 0.0));
    positionAlloc4->Add (Vector(241.75590319329217,686.3099711079445, 0.0));
    positionAlloc4->Add (Vector(718.9130330132756,529.2322239587107, 0.0));
    positionAlloc4->Add (Vector(503.4529628625801,273.4883372995203, 0.0));
    positionAlloc4->Add (Vector(541.9233722920499,490.48352487532475, 0.0));
    positionAlloc4->Add (Vector(501.3549065466025,573.5498774064206, 0.0));
    positionAlloc4->Add (Vector(564.7977279353788,415.94700367341017, 0.0));
    positionAlloc4->Add (Vector(309.6702645316696,795.5563227084021, 0.0));
    positionAlloc4->Add (Vector(696.0588166144753,689.8573707032825, 0.0));
    positionAlloc4->Add (Vector(313.5373345539966,436.69264908375646, 0.0));
    positionAlloc4->Add (Vector(297.3699419084291,501.0090315885375, 0.0));
    positionAlloc4->Add (Vector(844.1802982749149,689.3064619546359, 0.0));
    positionAlloc4->Add (Vector(217.77311169105303,483.2396915057432, 0.0));
    positionAlloc4->Add (Vector(414.84330717785286,674.8080227484803, 0.0));
    positionAlloc4->Add (Vector(488.1098789907653,770.7820776282714, 0.0));
    positionAlloc4->Add (Vector(333.6912571873501,319.23515523577714, 0.0));
    positionAlloc4->Add (Vector(751.8234639795469,470.77732033553355, 0.0));
    positionAlloc4->Add (Vector(647.7852719107001,186.37626869182617, 0.0));
    positionAlloc4->Add (Vector(590.4534351058045,639.5190448087126, 0.0));
    positionAlloc4->Add (Vector(660.9223823925184,437.65303347393075, 0.0));
    positionAlloc4->Add (Vector(370.1038740679838,556.6780106421147, 0.0));
    positionAlloc4->Add (Vector(512.8267461278764,473.264537093413, 0.0));
    positionAlloc4->Add (Vector(339.244013346909,725.9321701226323, 0.0));
    positionAlloc4->Add (Vector(823.6557215181699,773.1544693881892, 0.0));
    positionAlloc4->Add (Vector(688.9815327446206,222.77156313867272, 0.0));
    positionAlloc4->Add (Vector(460.1279836446126,582.4692362125005, 0.0));
    positionAlloc4->Add (Vector(677.6695990848583,303.3414211419183, 0.0));
    positionAlloc4->Add (Vector(803.5152005908761,653.9716775586357, 0.0));
    positionAlloc4->Add (Vector(692.2710560272589,335.3714252448232, 0.0));
    positionAlloc4->Add (Vector(812.0435480724431,242.80185423913764, 0.0));
    positionAlloc4->Add (Vector(618.3374607218635,238.39843080919826, 0.0));
    positionAlloc4->Add (Vector(756.7986491189462,588.3814922636986, 0.0));
    positionAlloc4->Add (Vector(490.50967099931216,666.9773276331354, 0.0));
    positionAlloc4->Add (Vector(619.9130964949959,263.52836068551414, 0.0));
    positionAlloc4->Add (Vector(347.86715237881134,472.6081146887564, 0.0));
    positionAlloc4->Add (Vector(377.15465441826314,811.5857141214209, 0.0));
    positionAlloc4->Add (Vector(199.3224743668708,344.7005483142376, 0.0));
    positionAlloc4->Add (Vector(437.0166507192201,487.3964424130069, 0.0));
    positionAlloc4->Add (Vector(523.8252792432596,765.6238854583246, 0.0));
    positionAlloc4->Add (Vector(785.2242234424979,235.73699179520804, 0.0));
    positionAlloc4->Add (Vector(634.8009600543073,550.3282657212676, 0.0));
    positionAlloc4->Add (Vector(713.0620898963904,196.08089035420764, 0.0));
    positionAlloc4->Add (Vector(511.98255159984365,785.6585809418981, 0.0));
    positionAlloc4->Add (Vector(435.6283231219716,283.8941597261779, 0.0));
    positionAlloc4->Add (Vector(654.2333584161951,792.7995247659704, 0.0));
    positionAlloc4->Add (Vector(767.0400737122374,441.13955800825664, 0.0));
    positionAlloc4->Add (Vector(169.73393831305552,321.3215557458491, 0.0));
    positionAlloc4->Add (Vector(333.1699566462545,196.89376607709332, 0.0));
    positionAlloc4->Add (Vector(821.3169231810762,579.3812925655197, 0.0));
    positionAlloc4->Add (Vector(156.41588512867153,290.97937195572115, 0.0));
    positionAlloc4->Add (Vector(315.6431920290062,772.824293206766, 0.0));
    positionAlloc4->Add (Vector(734.9001935634541,180.56265009910598, 0.0));
    positionAlloc4->Add (Vector(399.70621851623446,564.7380957311175, 0.0));
    positionAlloc4->Add (Vector(632.495524858086,238.97182963951826, 0.0));
    positionAlloc4->Add (Vector(213.6548149391288,796.25662288966, 0.0));
    positionAlloc4->Add (Vector(366.89298172729593,210.05599942964264, 0.0));
    positionAlloc4->Add (Vector(607.7959549941738,583.0096043504807, 0.0));
    positionAlloc4->Add (Vector(603.67148908275,162.85086879945678, 0.0));
    positionAlloc4->Add (Vector(452.93695719573554,488.9291488258178, 0.0));
    positionAlloc4->Add (Vector(384.92647096863095,685.5746141431916, 0.0));
    positionAlloc4->Add (Vector(539.6964795869977,666.8068573348154, 0.0));
    positionAlloc4->Add (Vector(551.7826225222193,596.1832285828284, 0.0));
    positionAlloc4->Add (Vector(502.58423286621127,277.8375911771708, 0.0));
    positionAlloc4->Add (Vector(586.2206971725311,202.09989290696097, 0.0));
    positionAlloc4->Add (Vector(405.53329668482104,452.70601670155213, 0.0));
    positionAlloc4->Add (Vector(258.3898771341592,278.0460481734593, 0.0));
    positionAlloc4->Add (Vector(174.00853091231818,498.2962583721081, 0.0));
    positionAlloc4->Add (Vector(241.71023305532592,570.670036498248, 0.0));
    positionAlloc4->Add (Vector(623.9656493485511,694.9611160431645, 0.0));
    positionAlloc4->Add (Vector(810.3170783119048,514.1613423721631, 0.0));
    positionAlloc4->Add (Vector(779.7287425606205,787.5769542312772, 0.0));
    positionAlloc4->Add (Vector(711.8593709065539,322.8757887269437, 0.0));
    positionAlloc4->Add (Vector(493.31436771516496,233.82275195037232, 0.0));
    positionAlloc4->Add (Vector(800.9093376057153,488.6578276079574, 0.0));
    positionAlloc4->Add (Vector(764.7767274126426,519.0786049654739, 0.0));
    positionAlloc4->Add (Vector(293.6310000333649,490.9577810265046, 0.0));
    positionAlloc4->Add (Vector(660.664995383507,164.23875731752435, 0.0));
    positionAlloc4->Add (Vector(802.853670379735,418.1411644947025, 0.0));
    positionAlloc4->Add (Vector(602.2328608676172,255.4276671479634, 0.0));
    positionAlloc4->Add (Vector(698.7802953539126,426.3650980147792, 0.0));
    positionAlloc4->Add (Vector(526.3844900136801,614.5373045371098, 0.0));
    positionAlloc4->Add (Vector(335.4769430888101,464.0428141324139, 0.0));
    positionAlloc4->Add (Vector(462.01087463200844,735.2830308815298, 0.0));
    positionAlloc4->Add (Vector(560.0447896517333,560.7375354912139, 0.0));
    positionAlloc4->Add (Vector(616.6961987775197,349.0018023668215, 0.0));
    positionAlloc4->Add (Vector(418.43678580252976,256.55974974180674, 0.0));
    positionAlloc4->Add (Vector(178.04836044378666,839.0253851698288, 0.0));
    positionAlloc4->Add (Vector(572.6621551906273,495.33911163666636, 0.0));
    positionAlloc4->Add (Vector(821.7145033264637,704.863733726132, 0.0));
    positionAlloc4->Add (Vector(535.4833812863562,721.7553587020432, 0.0));
    positionAlloc4->Add (Vector(399.4833150947653,673.3562771954845, 0.0));
    positionAlloc4->Add (Vector(318.9447926862499,204.9753699549591, 0.0));
    positionAlloc4->Add (Vector(502.9844225551221,755.9515254609123, 0.0));
    positionAlloc4->Add (Vector(345.7716995825102,284.0649254351037, 0.0));
    positionAlloc4->Add (Vector(325.33806272483446,714.134089773409, 0.0));
    positionAlloc4->Add (Vector(318.8710352266195,295.76226602575014, 0.0));
    positionAlloc4->Add (Vector(523.4881034875057,150.16870327959418, 0.0));
    positionAlloc4->Add (Vector(395.44988402000286,413.76743944194874, 0.0));
    positionAlloc4->Add (Vector(731.8720635629597,686.7378494368351, 0.0));
    positionAlloc4->Add (Vector(177.15961831022122,280.74076976179055, 0.0));
    positionAlloc4->Add (Vector(391.93411823055266,585.0551177966588, 0.0));
    positionAlloc4->Add (Vector(616.9257999877477,788.1038210841368, 0.0));
    positionAlloc4->Add (Vector(799.955426360645,555.8159804578404, 0.0));
    positionAlloc4->Add (Vector(317.96423260654194,208.36533763813168, 0.0));
    positionAlloc4->Add (Vector(812.8320296223618,318.4988099644547, 0.0));
    positionAlloc4->Add (Vector(260.16010834493727,171.7476911711194, 0.0));
    positionAlloc4->Add (Vector(450.6135279550918,479.4941631211562, 0.0));
    positionAlloc4->Add (Vector(739.7741558462611,362.2457356317724, 0.0));
    positionAlloc4->Add (Vector(804.3754283106608,734.4697425364818, 0.0));
    positionAlloc4->Add (Vector(285.8901393121805,525.625256484642, 0.0));
    positionAlloc4->Add (Vector(689.8701518232247,177.16906346826147, 0.0));
    positionAlloc4->Add (Vector(674.0705226756936,729.5474681817187, 0.0));
    positionAlloc4->Add (Vector(532.6304577645697,455.23288188300535, 0.0));
    positionAlloc4->Add (Vector(192.4781482654776,439.33972190983457, 0.0));
    positionAlloc4->Add (Vector(289.25084585652684,571.1806260343255, 0.0));
    positionAlloc4->Add (Vector(612.3288049551018,479.5714935561287, 0.0));
    positionAlloc4->Add (Vector(739.7881510256269,403.52712195590607, 0.0));
    positionAlloc4->Add (Vector(358.70524939358893,238.5249717824998, 0.0));
    positionAlloc4->Add (Vector(817.5925385441956,599.4771645178648, 0.0));
    positionAlloc4->Add (Vector(521.9379672656264,429.8036856820993, 0.0));
    positionAlloc4->Add (Vector(641.8407035065411,819.1860411782505, 0.0));
    positionAlloc4->Add (Vector(528.53594191038,624.1104804349616, 0.0));
    positionAlloc4->Add (Vector(723.5787616765449,401.87743044559653, 0.0));
    positionAlloc4->Add (Vector(462.7405111877248,154.967572746421, 0.0));
    positionAlloc4->Add (Vector(676.9089964781832,481.64130476009603, 0.0));
    positionAlloc4->Add (Vector(767.7465735476956,164.08520529715182, 0.0));
    positionAlloc4->Add (Vector(245.55210845850752,310.06952596922235, 0.0));
    positionAlloc4->Add (Vector(546.4260161165724,684.3064383980972, 0.0));
    positionAlloc4->Add (Vector(621.1057164020739,182.8750956853671, 0.0));
    positionAlloc4->Add (Vector(718.3774196735612,757.0065057007854, 0.0));
    positionAlloc4->Add (Vector(498.5640148075847,367.0238168087151, 0.0));
    positionAlloc4->Add (Vector(372.0814220498073,686.4128497832301, 0.0));
    positionAlloc4->Add (Vector(813.336560964467,482.6196062158398, 0.0));
    positionAlloc4->Add (Vector(448.97871249353386,240.12810615763883, 0.0));
    positionAlloc4->Add (Vector(374.39425705264716,215.46408651853665, 0.0));
    positionAlloc4->Add (Vector(638.649382482381,596.6979317613884, 0.0));
    positionAlloc4->Add (Vector(432.12713053499255,604.4077753816109, 0.0));
    positionAlloc4->Add (Vector(580.8868362352255,177.6902545938292, 0.0));
    positionAlloc4->Add (Vector(297.19332453130295,546.1220235712312, 0.0));
    positionAlloc4->Add (Vector(804.0875561998504,183.92496273307563, 0.0));
    positionAlloc4->Add (Vector(580.743488440002,438.36258402858454, 0.0));
    positionAlloc4->Add (Vector(415.5559187429306,787.5839588928277, 0.0));
    positionAlloc4->Add (Vector(354.3070239254798,270.9301227967611, 0.0));
    positionAlloc4->Add (Vector(290.3952617116489,300.75149799822594, 0.0));
    positionAlloc4->Add (Vector(441.24946928608034,834.4975854512472, 0.0));
    positionAlloc4->Add (Vector(746.0424271525826,488.1463716353119, 0.0));
    positionAlloc4->Add (Vector(311.6855173696446,711.3397669852886, 0.0));
    positionAlloc4->Add (Vector(333.75720333821596,186.64302490191915, 0.0));
    positionAlloc4->Add (Vector(176.44915461697576,600.3588075488324, 0.0));
    positionAlloc4->Add (Vector(310.8013985782622,289.70013112715714, 0.0));
    positionAlloc4->Add (Vector(374.04169492850775,445.2419693552881, 0.0));
    positionAlloc4->Add (Vector(714.695465421543,152.55941794125448, 0.0));
    positionAlloc4->Add (Vector(494.2165601132926,777.3653744022931, 0.0));
    positionAlloc4->Add (Vector(801.0037644537508,418.49717370510444, 0.0));
    positionAlloc4->Add (Vector(472.4077854862666,780.5321137155127, 0.0));
    positionAlloc4->Add (Vector(254.57957191764064,506.989555519448, 0.0));
    positionAlloc4->Add (Vector(294.8103127956015,400.753328870151, 0.0));
    positionAlloc4->Add (Vector(810.2370947854155,802.2119973276773, 0.0));
    positionAlloc4->Add (Vector(375.18045809091655,475.84088070778955, 0.0));
    positionAlloc4->Add (Vector(687.0633759114561,294.1211175366196, 0.0));
    positionAlloc4->Add (Vector(688.1349265048123,335.91131881515196, 0.0));
    positionAlloc4->Add (Vector(284.9877262595719,283.77393588380147, 0.0));
    positionAlloc4->Add (Vector(749.1476327139085,454.27470382152916, 0.0));
    positionAlloc4->Add (Vector(584.0009334341859,388.1560961158799, 0.0));
    positionAlloc4->Add (Vector(502.15082019012635,208.0402069761368, 0.0));
    positionAlloc4->Add (Vector(554.6504714086888,820.5855472742235, 0.0));
    positionAlloc4->Add (Vector(424.9873535369817,275.75585256873933, 0.0));
    positionAlloc4->Add (Vector(522.6521644029204,362.3549975499193, 0.0));
    positionAlloc4->Add (Vector(820.4366607233558,204.39869287420785, 0.0));
    positionAlloc4->Add (Vector(200.33470212151192,786.3039121849921, 0.0));
    positionAlloc4->Add (Vector(786.7106865376442,593.7226232827093, 0.0));
    positionAlloc4->Add (Vector(589.5711147400355,342.633148241433, 0.0));
    positionAlloc4->Add (Vector(163.5808719327508,378.9047379472715, 0.0));
    positionAlloc4->Add (Vector(785.8870750879987,696.9034531839845, 0.0));
    positionAlloc4->Add (Vector(308.7310321803611,802.0432708432483, 0.0));
    positionAlloc4->Add (Vector(569.3077309494972,200.21474062319749, 0.0));
    positionAlloc4->Add (Vector(441.3994499810483,464.1959559564021, 0.0));
    positionAlloc4->Add (Vector(786.6105648256417,285.53666685682254, 0.0));
    positionAlloc4->Add (Vector(234.8088228826898,209.7859379657857, 0.0));
    positionAlloc4->Add (Vector(790.3245301093623,266.8651253241284, 0.0));
    positionAlloc4->Add (Vector(261.5764193399815,827.6155204320171, 0.0));
    positionAlloc4->Add (Vector(406.6463900675195,583.3849800326874, 0.0));
    positionAlloc4->Add (Vector(782.8795623356536,259.5704493910189, 0.0));
    positionAlloc4->Add (Vector(508.0679485666451,792.9650713101848, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_12.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_ECC_flowmon/switch_ECC_12.flowmon", false, false);
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