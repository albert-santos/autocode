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
	  uint16_t numberOfRrhs = 48;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 800;
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


	//Configuração dos parâmetros das eNbs e dos UE
	ConfigStore inputConfig;
	inputConfig.ConfigureDefaults();
  	// UlBandwidth e DlBandwidth indicam a largura de banda em quantidade de PRBs (50PRBs = 10Mhz de largura de banda)
  	Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (50));
  	Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue (50));
  	// DlEarfcn e UlEarfcn são códigos que indica a frência que será utilizada. Para saber os valores correspondentes acesse: https://www.sqimway.com/lte_band.php
  	Config::SetDefault ("ns3::LteEnbNetDevice::DlEarfcn", UintegerValue (1575));
  	Config::SetDefault ("ns3::LteEnbNetDevice::UlEarfcn", UintegerValue (19575));
  	Config::SetDefault ("ns3::LteUeNetDevice::DlEarfcn", UintegerValue (1575));
  	// Potência de transmissão da eNb em dBm
  	Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (49));

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

	Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (100));
  Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue (100));
	
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
    positionAlloc->Add (Vector (356.6666666666667,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,140.0, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,270.0, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (270.0,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (270.0,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (270.0,270.0, 0.0));
    positionAlloc->Add (Vector (270.0,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (270.0,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (270.0,140.0, 0.0));
    positionAlloc->Add (Vector (270.0,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (270.0,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,270.0, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,10.0, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,270.0, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (140.0,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (140.0,270.0, 0.0));
    positionAlloc->Add (Vector (140.0,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (140.0,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (140.0,140.0, 0.0));
    positionAlloc->Add (Vector (140.0,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (140.0,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,270.0, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,140.0, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,400.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,270.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (10.0,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (10.0,140.0, 0.0));
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
    positionAlloc4->Add (Vector(271.21662660787536,322.10401108453027, 0.0));
    positionAlloc4->Add (Vector(318.25581646826043,56.534581450545396, 0.0));
    positionAlloc4->Add (Vector(60.105303672212116,395.93611298060256, 0.0));
    positionAlloc4->Add (Vector(22.34425592253806,19.373835949430518, 0.0));
    positionAlloc4->Add (Vector(118.31046275073489,19.276803324717218, 0.0));
    positionAlloc4->Add (Vector(199.92045886414988,307.40816584926813, 0.0));
    positionAlloc4->Add (Vector(213.83394306835692,29.32957128214566, 0.0));
    positionAlloc4->Add (Vector(375.52012191858984,169.71256935532728, 0.0));
    positionAlloc4->Add (Vector(291.925038166436,284.32076288967545, 0.0));
    positionAlloc4->Add (Vector(332.08622903845446,283.41115291446266, 0.0));
    positionAlloc4->Add (Vector(42.379654186617664,27.745445707215932, 0.0));
    positionAlloc4->Add (Vector(72.07665032361655,239.8111234524183, 0.0));
    positionAlloc4->Add (Vector(334.8151641624027,356.60214510408264, 0.0));
    positionAlloc4->Add (Vector(336.1990210991538,36.454524346662474, 0.0));
    positionAlloc4->Add (Vector(41.38359827599017,384.9385416482911, 0.0));
    positionAlloc4->Add (Vector(364.343494631607,17.684466617961103, 0.0));
    positionAlloc4->Add (Vector(326.8582761042894,121.21105284856726, 0.0));
    positionAlloc4->Add (Vector(75.4156407995219,181.9487241000274, 0.0));
    positionAlloc4->Add (Vector(387.72870127308636,120.79582886855, 0.0));
    positionAlloc4->Add (Vector(366.0251478446854,276.8390695001349, 0.0));
    positionAlloc4->Add (Vector(104.97319691404785,351.41597284398176, 0.0));
    positionAlloc4->Add (Vector(399.1472125496983,388.5896977907534, 0.0));
    positionAlloc4->Add (Vector(328.5387686151399,47.471045019187706, 0.0));
    positionAlloc4->Add (Vector(150.50921320753483,59.05970043277677, 0.0));
    positionAlloc4->Add (Vector(232.5174188235227,47.14694833329296, 0.0));
    positionAlloc4->Add (Vector(123.51433122522147,199.23587287476937, 0.0));
    positionAlloc4->Add (Vector(344.3328238142579,155.8967594700419, 0.0));
    positionAlloc4->Add (Vector(332.5491709873691,137.45804627428177, 0.0));
    positionAlloc4->Add (Vector(245.11034676081076,193.7461003797794, 0.0));
    positionAlloc4->Add (Vector(69.8247667035481,332.53605579180515, 0.0));
    positionAlloc4->Add (Vector(116.60354767194718,230.23504313658108, 0.0));
    positionAlloc4->Add (Vector(122.39427585205655,228.58897097418094, 0.0));
    positionAlloc4->Add (Vector(57.22232030050023,385.0912134615008, 0.0));
    positionAlloc4->Add (Vector(309.52287744075164,286.6001097239351, 0.0));
    positionAlloc4->Add (Vector(138.4007938110206,322.3026609335318, 0.0));
    positionAlloc4->Add (Vector(195.06732478665944,339.67475353072405, 0.0));
    positionAlloc4->Add (Vector(176.11100636853888,81.84164110455438, 0.0));
    positionAlloc4->Add (Vector(372.68585149155456,375.4458122349201, 0.0));
    positionAlloc4->Add (Vector(320.36067168815316,52.568803852852895, 0.0));
    positionAlloc4->Add (Vector(392.49354947251646,125.80711604977965, 0.0));
    positionAlloc4->Add (Vector(267.8897029308486,114.11497550597241, 0.0));
    positionAlloc4->Add (Vector(7.087443146711037,139.68987586056457, 0.0));
    positionAlloc4->Add (Vector(272.32885170673194,337.4876817538982, 0.0));
    positionAlloc4->Add (Vector(92.01790818068649,71.36168103036482, 0.0));
    positionAlloc4->Add (Vector(183.73288988943543,370.20226099715217, 0.0));
    positionAlloc4->Add (Vector(135.08709722312028,269.98817669030626, 0.0));
    positionAlloc4->Add (Vector(356.0835336759808,74.84616877780562, 0.0));
    positionAlloc4->Add (Vector(94.15065364473088,142.8251875883139, 0.0));
    positionAlloc4->Add (Vector(91.71937533203058,108.64698169034996, 0.0));
    positionAlloc4->Add (Vector(280.4411796255306,350.9910558809003, 0.0));
    positionAlloc4->Add (Vector(210.07403353336852,382.85659693682305, 0.0));
    positionAlloc4->Add (Vector(167.48032148625236,203.1880129687778, 0.0));
    positionAlloc4->Add (Vector(171.61895737937996,383.5671098455656, 0.0));
    positionAlloc4->Add (Vector(127.33932473916347,105.38796969223387, 0.0));
    positionAlloc4->Add (Vector(286.37818908838705,392.8090387343686, 0.0));
    positionAlloc4->Add (Vector(329.69728044469656,253.8527369432948, 0.0));
    positionAlloc4->Add (Vector(148.88004540652645,58.50816479241625, 0.0));
    positionAlloc4->Add (Vector(84.95552366282388,357.8135548354909, 0.0));
    positionAlloc4->Add (Vector(183.93069634531108,349.2208042153661, 0.0));
    positionAlloc4->Add (Vector(122.00153827254701,263.71522293579, 0.0));
    positionAlloc4->Add (Vector(71.14960331526548,98.28042033797497, 0.0));
    positionAlloc4->Add (Vector(269.8840909828359,214.38521837067515, 0.0));
    positionAlloc4->Add (Vector(69.91953392998576,237.17426152526983, 0.0));
    positionAlloc4->Add (Vector(142.88737456275697,397.33225767593865, 0.0));
    positionAlloc4->Add (Vector(135.31402137017108,291.60772477707775, 0.0));
    positionAlloc4->Add (Vector(353.9388383775976,283.71025394500333, 0.0));
    positionAlloc4->Add (Vector(57.753747780998225,73.58916565335396, 0.0));
    positionAlloc4->Add (Vector(69.87829061947251,195.5308075406066, 0.0));
    positionAlloc4->Add (Vector(89.73431208010597,23.513622672284296, 0.0));
    positionAlloc4->Add (Vector(330.07593213540844,123.53517646960648, 0.0));
    positionAlloc4->Add (Vector(26.38444171600711,139.1276961599266, 0.0));
    positionAlloc4->Add (Vector(150.78854832602585,324.24449523034895, 0.0));
    positionAlloc4->Add (Vector(182.19570268635593,351.05784323364446, 0.0));
    positionAlloc4->Add (Vector(147.02080296871128,241.99431107861216, 0.0));
    positionAlloc4->Add (Vector(84.53568238072621,259.3458014952689, 0.0));
    positionAlloc4->Add (Vector(39.31525640690903,183.89401291945074, 0.0));
    positionAlloc4->Add (Vector(234.8567656749551,353.33851552415643, 0.0));
    positionAlloc4->Add (Vector(227.60758190064183,284.17522912832976, 0.0));
    positionAlloc4->Add (Vector(229.61043921413798,166.99082015057888, 0.0));
    positionAlloc4->Add (Vector(356.5578942495223,355.71660700101876, 0.0));
    positionAlloc4->Add (Vector(42.096338709941875,384.89048523861817, 0.0));
    positionAlloc4->Add (Vector(79.51108409494121,66.74882947151728, 0.0));
    positionAlloc4->Add (Vector(393.2381368044721,189.56815914739954, 0.0));
    positionAlloc4->Add (Vector(257.858235000503,346.9791476913135, 0.0));
    positionAlloc4->Add (Vector(225.672386869862,219.6064991530692, 0.0));
    positionAlloc4->Add (Vector(213.11882223788805,34.46566271684062, 0.0));
    positionAlloc4->Add (Vector(71.48704368884098,135.9994230020991, 0.0));
    positionAlloc4->Add (Vector(28.452341195780882,107.76601196633631, 0.0));
    positionAlloc4->Add (Vector(103.26645570814614,357.92903072490003, 0.0));
    positionAlloc4->Add (Vector(159.58543037676503,316.7992739947334, 0.0));
    positionAlloc4->Add (Vector(241.5073473840633,182.27817620473564, 0.0));
    positionAlloc4->Add (Vector(3.5697090227733153,103.80832122164074, 0.0));
    positionAlloc4->Add (Vector(178.84685623130184,333.3581423315197, 0.0));
    positionAlloc4->Add (Vector(358.49659314889556,113.46669879195316, 0.0));
    positionAlloc4->Add (Vector(217.17870419543846,185.34535487817288, 0.0));
    positionAlloc4->Add (Vector(22.384901292512094,282.8048596819117, 0.0));
    positionAlloc4->Add (Vector(306.1255720282565,295.97193500997963, 0.0));
    positionAlloc4->Add (Vector(79.10230430284177,300.5956371351796, 0.0));
    positionAlloc4->Add (Vector(384.65476603348134,68.40744280901463, 0.0));
    positionAlloc4->Add (Vector(54.042531310682975,47.39016075168392, 0.0));
    positionAlloc4->Add (Vector(104.10422184625396,186.99083286208543, 0.0));
    positionAlloc4->Add (Vector(129.958620536646,70.05503173965293, 0.0));
    positionAlloc4->Add (Vector(327.67426271044843,47.219135247030586, 0.0));
    positionAlloc4->Add (Vector(2.9542822173147076,125.2939260960483, 0.0));
    positionAlloc4->Add (Vector(133.257098579613,101.30338319073746, 0.0));
    positionAlloc4->Add (Vector(113.70723053741804,123.72344298652176, 0.0));
    positionAlloc4->Add (Vector(258.98107516326536,148.87456137054969, 0.0));
    positionAlloc4->Add (Vector(188.85576232599445,15.566173890242974, 0.0));
    positionAlloc4->Add (Vector(130.13272691810954,373.4429978138558, 0.0));
    positionAlloc4->Add (Vector(80.91847197622721,143.3282920920018, 0.0));
    positionAlloc4->Add (Vector(282.6599260887911,146.6363873277416, 0.0));
    positionAlloc4->Add (Vector(157.13071887847846,143.16486098091775, 0.0));
    positionAlloc4->Add (Vector(119.84786769815337,91.8358595865561, 0.0));
    positionAlloc4->Add (Vector(227.89566194401309,349.843679422097, 0.0));
    positionAlloc4->Add (Vector(184.28550467623145,338.5456417488312, 0.0));
    positionAlloc4->Add (Vector(149.89627757249235,349.55965502570183, 0.0));
    positionAlloc4->Add (Vector(75.76482400412759,320.5774748812454, 0.0));
    positionAlloc4->Add (Vector(30.853006170401365,55.360487195664774, 0.0));
    positionAlloc4->Add (Vector(142.27098000317588,75.77218044292948, 0.0));
    positionAlloc4->Add (Vector(15.280211537055255,296.6421128836602, 0.0));
    positionAlloc4->Add (Vector(94.22096065895983,180.7882954756023, 0.0));
    positionAlloc4->Add (Vector(279.2391148191506,318.9431916189202, 0.0));
    positionAlloc4->Add (Vector(307.5377474208747,195.6857670023592, 0.0));
    positionAlloc4->Add (Vector(173.3410254228653,356.53133320836594, 0.0));
    positionAlloc4->Add (Vector(0.8000293755805021,227.37185483860145, 0.0));
    positionAlloc4->Add (Vector(356.03636862477754,124.94734894740631, 0.0));
    positionAlloc4->Add (Vector(206.74537601526072,24.752987154049542, 0.0));
    positionAlloc4->Add (Vector(128.22673359709657,105.43316026350146, 0.0));
    positionAlloc4->Add (Vector(192.6525046173732,251.34280808263304, 0.0));
    positionAlloc4->Add (Vector(33.57368396966751,262.13840554533084, 0.0));
    positionAlloc4->Add (Vector(176.75821216392987,262.0938120919093, 0.0));
    positionAlloc4->Add (Vector(173.42917957385362,110.93081429199133, 0.0));
    positionAlloc4->Add (Vector(316.9018698396575,193.35050636732606, 0.0));
    positionAlloc4->Add (Vector(208.25327139349756,275.2525300652053, 0.0));
    positionAlloc4->Add (Vector(396.047060690566,349.7014323424871, 0.0));
    positionAlloc4->Add (Vector(37.016474829297685,136.46091813570956, 0.0));
    positionAlloc4->Add (Vector(177.65960306844866,93.40039509462312, 0.0));
    positionAlloc4->Add (Vector(23.480322445232325,283.1632234521622, 0.0));
    positionAlloc4->Add (Vector(147.38758933992108,382.20651368203676, 0.0));
    positionAlloc4->Add (Vector(242.07324905223788,304.6878204825166, 0.0));
    positionAlloc4->Add (Vector(188.42282021536633,250.23233804742836, 0.0));
    positionAlloc4->Add (Vector(15.01510095486469,261.49328937876646, 0.0));
    positionAlloc4->Add (Vector(79.38693671020674,304.3372876940833, 0.0));
    positionAlloc4->Add (Vector(117.46821952485847,381.9996707307763, 0.0));
    positionAlloc4->Add (Vector(55.16888738479033,369.3605120696965, 0.0));
    positionAlloc4->Add (Vector(120.92475587551891,322.9643900493617, 0.0));
    positionAlloc4->Add (Vector(311.12300558591727,174.55398193692363, 0.0));
    positionAlloc4->Add (Vector(354.5759469630699,243.57760516719074, 0.0));
    positionAlloc4->Add (Vector(278.76591087854916,392.0141797543473, 0.0));
    positionAlloc4->Add (Vector(258.66541421153966,128.28501922988397, 0.0));
    positionAlloc4->Add (Vector(358.622258597063,15.357539599315473, 0.0));
    positionAlloc4->Add (Vector(19.690938117630672,2.4335543952020977, 0.0));
    positionAlloc4->Add (Vector(272.74381074877556,9.049442299967092, 0.0));
    positionAlloc4->Add (Vector(104.72113605004454,180.71152543808395, 0.0));
    positionAlloc4->Add (Vector(271.42719096308156,281.5821187104308, 0.0));
    positionAlloc4->Add (Vector(390.1771495560284,299.7007413816053, 0.0));
    positionAlloc4->Add (Vector(273.4800321377516,285.67601875363584, 0.0));
    positionAlloc4->Add (Vector(40.0140479177217,111.88755707130822, 0.0));
    positionAlloc4->Add (Vector(244.1986343305751,372.56371448728027, 0.0));
    positionAlloc4->Add (Vector(194.3310631071411,120.37984864952324, 0.0));
    positionAlloc4->Add (Vector(147.03944830520078,304.0638657986619, 0.0));
    positionAlloc4->Add (Vector(222.71084254213315,2.344844465657303, 0.0));
    positionAlloc4->Add (Vector(360.28870904280615,137.71441032886793, 0.0));
    positionAlloc4->Add (Vector(310.9548789308575,174.47978437191242, 0.0));
    positionAlloc4->Add (Vector(281.76357706712855,323.57424018102705, 0.0));
    positionAlloc4->Add (Vector(203.92903921826732,138.09560645344732, 0.0));
    positionAlloc4->Add (Vector(395.60954391102143,268.59612072737997, 0.0));
    positionAlloc4->Add (Vector(177.47211313244503,315.5041560524388, 0.0));
    positionAlloc4->Add (Vector(156.64235589813265,27.628114273644933, 0.0));
    positionAlloc4->Add (Vector(286.92159569190306,339.03962660458416, 0.0));
    positionAlloc4->Add (Vector(238.80243755208116,25.391920089891507, 0.0));
    positionAlloc4->Add (Vector(260.0269665227884,121.22507861189509, 0.0));
    positionAlloc4->Add (Vector(170.07178949163028,78.52792785388418, 0.0));
    positionAlloc4->Add (Vector(193.86379927295087,34.34141258830174, 0.0));
    positionAlloc4->Add (Vector(294.22314916134206,301.33357269811165, 0.0));
    positionAlloc4->Add (Vector(326.5812601547547,110.61265416850992, 0.0));
    positionAlloc4->Add (Vector(72.75689975563738,67.37718262723295, 0.0));
    positionAlloc4->Add (Vector(369.49410347266536,354.2361686855479, 0.0));
    positionAlloc4->Add (Vector(42.93287190392343,192.61469377499782, 0.0));
    positionAlloc4->Add (Vector(377.2284960517151,349.2544866348237, 0.0));
    positionAlloc4->Add (Vector(367.64287656707836,66.41997329661802, 0.0));
    positionAlloc4->Add (Vector(253.09081311135705,117.80887121473577, 0.0));
    positionAlloc4->Add (Vector(363.4919258022572,202.70065723057317, 0.0));
    positionAlloc4->Add (Vector(26.273307174192297,342.1970401378316, 0.0));
    positionAlloc4->Add (Vector(149.87113684176342,138.8204857777016, 0.0));
    positionAlloc4->Add (Vector(204.20392010879164,371.7163938641072, 0.0));
    positionAlloc4->Add (Vector(43.06559756110442,206.9662469255875, 0.0));
    positionAlloc4->Add (Vector(396.27262486427657,334.11294472978386, 0.0));
    positionAlloc4->Add (Vector(217.3483744697947,264.7360491828276, 0.0));
    positionAlloc4->Add (Vector(64.4674865151024,381.8595901320493, 0.0));
    positionAlloc4->Add (Vector(137.88187479632725,328.5102745067938, 0.0));
    positionAlloc4->Add (Vector(329.3675660895146,95.91075659257965, 0.0));
    positionAlloc4->Add (Vector(224.6281166651519,10.135355179211114, 0.0));
    positionAlloc4->Add (Vector(52.427035627036744,381.0755340447911, 0.0));
    positionAlloc4->Add (Vector(74.55686738806331,393.68034027096473, 0.0));
    positionAlloc4->Add (Vector(118.17051820293187,153.02674673147973, 0.0));
    positionAlloc4->Add (Vector(84.76160001336375,312.71369499542357, 0.0));
    positionAlloc4->Add (Vector(199.70329823457544,156.38154876635335, 0.0));
    positionAlloc4->Add (Vector(276.98826947708886,329.6982157519709, 0.0));
    positionAlloc4->Add (Vector(123.56478701324072,168.12844121235676, 0.0));
    positionAlloc4->Add (Vector(385.8856966514096,230.21209758623223, 0.0));
    positionAlloc4->Add (Vector(91.42111870711958,56.61130965222512, 0.0));
    positionAlloc4->Add (Vector(243.81140060095933,0.5247711498840069, 0.0));
    positionAlloc4->Add (Vector(31.198600991785376,346.23697368085993, 0.0));
    positionAlloc4->Add (Vector(142.27236974419077,131.74533224926975, 0.0));
    positionAlloc4->Add (Vector(115.20001371009477,92.794708748437, 0.0));
    positionAlloc4->Add (Vector(178.12504064718323,92.44227378474856, 0.0));
    positionAlloc4->Add (Vector(376.91056258726036,84.50157288977258, 0.0));
    positionAlloc4->Add (Vector(398.2378158397852,226.00169003591998, 0.0));
    positionAlloc4->Add (Vector(50.6895331563439,337.9549876217894, 0.0));
    positionAlloc4->Add (Vector(280.9590946081908,284.052938671477, 0.0));
    positionAlloc4->Add (Vector(377.96412341484455,385.9755469923573, 0.0));
    positionAlloc4->Add (Vector(84.92332511302307,96.53379554065312, 0.0));
    positionAlloc4->Add (Vector(169.71913061166813,1.0983612066741255, 0.0));
    positionAlloc4->Add (Vector(397.0271301116906,256.53350992436043, 0.0));
    positionAlloc4->Add (Vector(236.25230698019772,175.18497896590364, 0.0));
    positionAlloc4->Add (Vector(97.84518181722777,29.73170231106006, 0.0));
    positionAlloc4->Add (Vector(121.74884928377581,260.90657562515025, 0.0));
    positionAlloc4->Add (Vector(64.57257910058787,57.29495198653609, 0.0));
    positionAlloc4->Add (Vector(132.0663347169211,186.57927867629604, 0.0));
    positionAlloc4->Add (Vector(181.1284090615867,262.6371871890818, 0.0));
    positionAlloc4->Add (Vector(208.61236149313228,48.206662186834095, 0.0));
    positionAlloc4->Add (Vector(211.032534473217,41.16341911651706, 0.0));
    positionAlloc4->Add (Vector(334.33854206127216,241.99090379792113, 0.0));
    positionAlloc4->Add (Vector(36.06927645127289,372.0579355361934, 0.0));
    positionAlloc4->Add (Vector(115.65566565087164,142.7687481405136, 0.0));
    positionAlloc4->Add (Vector(230.33152299581968,251.79666804047628, 0.0));
    positionAlloc4->Add (Vector(14.598756177552419,288.3808055675487, 0.0));
    positionAlloc4->Add (Vector(356.78676915374433,205.89252894571018, 0.0));
    positionAlloc4->Add (Vector(364.9722801948934,151.53592281776525, 0.0));
    positionAlloc4->Add (Vector(72.008788331618,233.7622383050975, 0.0));
    positionAlloc4->Add (Vector(249.93512759176886,86.64364910304765, 0.0));
    positionAlloc4->Add (Vector(341.19679113267387,368.04380946259255, 0.0));
    positionAlloc4->Add (Vector(310.77313339299053,325.0655686464645, 0.0));
    positionAlloc4->Add (Vector(100.69371462925871,9.154841829901716, 0.0));
    positionAlloc4->Add (Vector(104.28869105545911,397.8013963155583, 0.0));
    positionAlloc4->Add (Vector(105.9585924935352,296.3690426708914, 0.0));
    positionAlloc4->Add (Vector(306.24669337036823,15.848205663796433, 0.0));
    positionAlloc4->Add (Vector(132.1831966298575,254.039932785274, 0.0));
    positionAlloc4->Add (Vector(168.8219682538432,138.3762986639442, 0.0));
    positionAlloc4->Add (Vector(182.90679498055488,329.933062781308, 0.0));
    positionAlloc4->Add (Vector(293.41173204018304,166.71733102432734, 0.0));
    positionAlloc4->Add (Vector(383.2638420853839,54.472395629094095, 0.0));
    positionAlloc4->Add (Vector(174.1936910875086,325.8405708201324, 0.0));
    positionAlloc4->Add (Vector(293.2448934274933,277.40188056539097, 0.0));
    positionAlloc4->Add (Vector(193.5885248543265,120.21353360657523, 0.0));
    positionAlloc4->Add (Vector(144.91219750143247,240.85017060796906, 0.0));
    positionAlloc4->Add (Vector(328.3951262557871,208.64916627311766, 0.0));
    positionAlloc4->Add (Vector(109.41209893460724,102.63335902095028, 0.0));
    positionAlloc4->Add (Vector(99.42911937859762,364.7770851635308, 0.0));
    positionAlloc4->Add (Vector(130.25690546875848,317.88280209382185, 0.0));
    positionAlloc4->Add (Vector(57.5250654479623,46.37312036425496, 0.0));
    positionAlloc4->Add (Vector(318.70483624410446,177.57947974831572, 0.0));
    positionAlloc4->Add (Vector(52.490438719555364,156.16055343815552, 0.0));
    positionAlloc4->Add (Vector(57.43888969995625,42.78614551605307, 0.0));
    positionAlloc4->Add (Vector(200.9401270431849,14.391714339731587, 0.0));
    positionAlloc4->Add (Vector(83.8678760784776,225.07980592287237, 0.0));
    positionAlloc4->Add (Vector(394.02985880102733,303.7386480226688, 0.0));
    positionAlloc4->Add (Vector(38.862262011468474,274.6786552841293, 0.0));
    positionAlloc4->Add (Vector(116.11303304984646,292.8844859805704, 0.0));
    positionAlloc4->Add (Vector(207.41306626956688,54.66589032479816, 0.0));
    positionAlloc4->Add (Vector(382.0139590307719,30.36368620531902, 0.0));
    positionAlloc4->Add (Vector(253.9029065137845,197.3702954589436, 0.0));
    positionAlloc4->Add (Vector(310.82230269727415,156.5083813670778, 0.0));
    positionAlloc4->Add (Vector(266.40788905091824,221.8221914685311, 0.0));
    positionAlloc4->Add (Vector(149.96872283878258,256.1217827262998, 0.0));
    positionAlloc4->Add (Vector(300.8275614908404,357.2100529636156, 0.0));
    positionAlloc4->Add (Vector(75.2902091748898,47.53662787103057, 0.0));
    positionAlloc4->Add (Vector(161.61788758717574,363.2097950359461, 0.0));
    positionAlloc4->Add (Vector(289.85858309562,281.90089855654935, 0.0));
    positionAlloc4->Add (Vector(200.99265371596667,41.36969481024168, 0.0));
    positionAlloc4->Add (Vector(302.9221254707935,109.70165988640885, 0.0));
    positionAlloc4->Add (Vector(210.68674056939437,193.487379613844, 0.0));
    positionAlloc4->Add (Vector(76.7598137312326,156.66448953092146, 0.0));
    positionAlloc4->Add (Vector(126.33563012718207,128.06214888284768, 0.0));
    positionAlloc4->Add (Vector(376.1930938226406,124.48214356340368, 0.0));
    positionAlloc4->Add (Vector(279.9775127019961,114.93323903132553, 0.0));
    positionAlloc4->Add (Vector(377.53270202811524,205.19285989244662, 0.0));
    positionAlloc4->Add (Vector(193.54389908178945,62.5574063017476, 0.0));
    positionAlloc4->Add (Vector(127.2835234273738,94.40759566555354, 0.0));
    positionAlloc4->Add (Vector(238.32330769587142,39.96858415253479, 0.0));
    positionAlloc4->Add (Vector(164.51037078861253,17.629020916981553, 0.0));
    positionAlloc4->Add (Vector(41.18651029955793,264.0773679449936, 0.0));
    positionAlloc4->Add (Vector(387.61602075831496,211.32011401610185, 0.0));
    positionAlloc4->Add (Vector(327.63232368643935,338.7825472148391, 0.0));
    positionAlloc4->Add (Vector(354.83615106139587,154.88843752533458, 0.0));
    positionAlloc4->Add (Vector(388.5633736605574,96.4903915240753, 0.0));
    positionAlloc4->Add (Vector(197.90001628563104,185.6086397568361, 0.0));
    positionAlloc4->Add (Vector(317.32090948716666,10.171571741024543, 0.0));
    positionAlloc4->Add (Vector(289.66930296364154,372.20491349410884, 0.0));
    positionAlloc4->Add (Vector(210.538374097655,81.29932158908608, 0.0));
    positionAlloc4->Add (Vector(271.6993942256012,351.5438258309134, 0.0));
    positionAlloc4->Add (Vector(301.37451692381296,336.0057510037202, 0.0));
    positionAlloc4->Add (Vector(117.97020263342222,223.3608960806272, 0.0));
    positionAlloc4->Add (Vector(22.37551773216144,133.3654868633322, 0.0));
    positionAlloc4->Add (Vector(302.2864251485077,193.69151482088992, 0.0));
    positionAlloc4->Add (Vector(288.8627695922174,52.11820829085387, 0.0));
    positionAlloc4->Add (Vector(306.78320187651076,80.45645733498108, 0.0));
    positionAlloc4->Add (Vector(180.7474350638356,252.4629708634055, 0.0));
    positionAlloc4->Add (Vector(11.253191002524776,310.7017817935817, 0.0));
    positionAlloc4->Add (Vector(80.7488406431938,251.99766022437365, 0.0));
    positionAlloc4->Add (Vector(38.9977342581433,96.43165599932732, 0.0));
    positionAlloc4->Add (Vector(214.11049562334176,333.5284319509483, 0.0));
    positionAlloc4->Add (Vector(281.34434988250024,27.82838317284453, 0.0));
    positionAlloc4->Add (Vector(56.66307102408843,52.174329837448894, 0.0));
    positionAlloc4->Add (Vector(104.49816866870817,24.17380653166181, 0.0));
    positionAlloc4->Add (Vector(348.561648408345,386.0022309354914, 0.0));
    positionAlloc4->Add (Vector(233.04928689451927,302.96123492013413, 0.0));
    positionAlloc4->Add (Vector(365.2373427814496,116.97596238257275, 0.0));
    positionAlloc4->Add (Vector(270.4456610160504,133.66594833471788, 0.0));
    positionAlloc4->Add (Vector(298.98509709658356,261.13980118831796, 0.0));
    positionAlloc4->Add (Vector(214.37914671131796,368.69961313839127, 0.0));
    positionAlloc4->Add (Vector(157.71230366883736,158.3496036067805, 0.0));
    positionAlloc4->Add (Vector(337.3563885751531,275.1350921620379, 0.0));
    positionAlloc4->Add (Vector(126.02393626676283,23.290052322100998, 0.0));
    positionAlloc4->Add (Vector(83.69020642859049,357.3654556519405, 0.0));
    positionAlloc4->Add (Vector(336.7572292714989,295.3288797869579, 0.0));
    positionAlloc4->Add (Vector(265.4990834263006,341.84055752714835, 0.0));
    positionAlloc4->Add (Vector(157.9797344943127,61.95666991045519, 0.0));
    positionAlloc4->Add (Vector(314.42477667419905,153.18009180313535, 0.0));
    positionAlloc4->Add (Vector(67.33298526773184,4.606207204636847, 0.0));
    positionAlloc4->Add (Vector(244.2905146016912,342.98954711985846, 0.0));
    positionAlloc4->Add (Vector(114.90493141207621,39.92953040109728, 0.0));
    positionAlloc4->Add (Vector(123.52377473437781,294.3436327187906, 0.0));
    positionAlloc4->Add (Vector(310.42404223938894,9.190802396911124, 0.0));
    positionAlloc4->Add (Vector(337.24486534727436,75.28194362719529, 0.0));
    positionAlloc4->Add (Vector(274.679142331907,83.29335198614771, 0.0));
    positionAlloc4->Add (Vector(366.3954807478651,91.24771231112425, 0.0));
    positionAlloc4->Add (Vector(279.77856350664115,108.22220153040685, 0.0));
    positionAlloc4->Add (Vector(159.62071332731176,355.46769663484935, 0.0));
    positionAlloc4->Add (Vector(106.29175229893563,263.06852347374246, 0.0));
    positionAlloc4->Add (Vector(340.60350834048984,385.52433863274024, 0.0));
    positionAlloc4->Add (Vector(217.2262978181699,63.24831884195996, 0.0));
    positionAlloc4->Add (Vector(135.27717574952257,59.75655060384266, 0.0));
    positionAlloc4->Add (Vector(71.34865507150306,251.4039461783805, 0.0));
    positionAlloc4->Add (Vector(313.313026426545,4.046012691306666, 0.0));
    positionAlloc4->Add (Vector(396.2573277723427,152.24135796965274, 0.0));
    positionAlloc4->Add (Vector(80.65927643103086,261.72260931318624, 0.0));
    positionAlloc4->Add (Vector(155.44732800677633,201.97701515124052, 0.0));
    positionAlloc4->Add (Vector(124.4274366648638,142.44529386163828, 0.0));
    positionAlloc4->Add (Vector(256.441925827576,37.55266411308958, 0.0));
    positionAlloc4->Add (Vector(225.7355581352392,360.80319963830294, 0.0));
    positionAlloc4->Add (Vector(138.53730385327862,49.64965460547517, 0.0));
    positionAlloc4->Add (Vector(103.29969792048801,135.9511575444566, 0.0));
    positionAlloc4->Add (Vector(186.52108445437725,250.99391435226676, 0.0));
    positionAlloc4->Add (Vector(353.2807111848192,224.90017583950475, 0.0));
    positionAlloc4->Add (Vector(381.3930077745542,162.9989606771954, 0.0));
    positionAlloc4->Add (Vector(110.06464488353478,160.65569031243302, 0.0));
    positionAlloc4->Add (Vector(335.6369793233251,371.40830567853965, 0.0));
    positionAlloc4->Add (Vector(137.14539966524944,188.35530832882492, 0.0));
    positionAlloc4->Add (Vector(277.4381081609701,51.353630929407814, 0.0));
    positionAlloc4->Add (Vector(55.3298931308873,331.58605536117534, 0.0));
    positionAlloc4->Add (Vector(89.40562067413818,374.285074914559, 0.0));
    positionAlloc4->Add (Vector(53.79349476180053,74.16894387911248, 0.0));
    positionAlloc4->Add (Vector(109.75236432319116,246.3237601336535, 0.0));
    positionAlloc4->Add (Vector(375.74875246008634,204.58557283811297, 0.0));
    positionAlloc4->Add (Vector(245.7778318046551,188.46921500415243, 0.0));
    positionAlloc4->Add (Vector(158.5557952164255,210.99173845556894, 0.0));
    positionAlloc4->Add (Vector(96.38107153155242,90.1728768765885, 0.0));
    positionAlloc4->Add (Vector(286.96646854753743,273.6878641031803, 0.0));
    positionAlloc4->Add (Vector(256.84680872715353,296.8003970907744, 0.0));
    positionAlloc4->Add (Vector(213.22411221390024,113.05060815084596, 0.0));
    positionAlloc4->Add (Vector(93.62497093137296,60.56383210884238, 0.0));
    positionAlloc4->Add (Vector(265.247826682641,133.11119673170361, 0.0));
    positionAlloc4->Add (Vector(165.79183055348966,183.33605053740914, 0.0));
    positionAlloc4->Add (Vector(10.496117843477615,121.5478314523597, 0.0));
    positionAlloc4->Add (Vector(350.35518513672656,21.450321372675596, 0.0));
    positionAlloc4->Add (Vector(93.26583838198559,222.7005536323151, 0.0));
    positionAlloc4->Add (Vector(205.30676379463938,290.0739245667982, 0.0));
    positionAlloc4->Add (Vector(92.47027423225344,154.21165920663466, 0.0));
    positionAlloc4->Add (Vector(167.01574697828602,162.88845549275726, 0.0));
    positionAlloc4->Add (Vector(60.64972240878923,198.81602027577227, 0.0));
    positionAlloc4->Add (Vector(182.00351850012723,315.5560762291276, 0.0));
    positionAlloc4->Add (Vector(1.104169053272619,331.2011149941823, 0.0));
    positionAlloc4->Add (Vector(156.8700797965224,303.7801426989697, 0.0));
    positionAlloc4->Add (Vector(193.29051609653027,377.6932951128516, 0.0));
    positionAlloc4->Add (Vector(353.7163213877888,213.55989059140592, 0.0));
    positionAlloc4->Add (Vector(129.7663361788199,395.6868459791341, 0.0));
    positionAlloc4->Add (Vector(208.18941667723135,313.8484203084878, 0.0));
    positionAlloc4->Add (Vector(131.3231064698234,184.1501577243346, 0.0));
    positionAlloc4->Add (Vector(77.26262118718724,5.499841694031904, 0.0));
    positionAlloc4->Add (Vector(243.51589342540163,13.603846336332648, 0.0));
    positionAlloc4->Add (Vector(238.3904775509118,236.54355465922373, 0.0));
    positionAlloc4->Add (Vector(74.84938814411906,185.7323795582011, 0.0));
    positionAlloc4->Add (Vector(100.04202041536772,145.25253261976988, 0.0));
    positionAlloc4->Add (Vector(172.87975132410156,296.4621059911799, 0.0));
    positionAlloc4->Add (Vector(167.37964598019883,235.2221485952078, 0.0));
    positionAlloc4->Add (Vector(260.95781375885116,68.25330271304195, 0.0));
    positionAlloc4->Add (Vector(294.27468142385254,280.1795548476186, 0.0));
    positionAlloc4->Add (Vector(219.27330692830557,275.3679223489472, 0.0));
    positionAlloc4->Add (Vector(193.190939561456,329.49008100936624, 0.0));
    positionAlloc4->Add (Vector(71.05581307341313,50.03213920683414, 0.0));
    positionAlloc4->Add (Vector(125.98295355207378,123.61778129835277, 0.0));
    positionAlloc4->Add (Vector(52.18698120746055,308.19693670223063, 0.0));
    positionAlloc4->Add (Vector(154.38177810897506,303.8630276661442, 0.0));
    positionAlloc4->Add (Vector(218.17018842537692,386.39108688255584, 0.0));
    positionAlloc4->Add (Vector(140.99677024432492,208.0540093370676, 0.0));
    positionAlloc4->Add (Vector(136.7834391081013,292.2786572569284, 0.0));
    positionAlloc4->Add (Vector(33.315204562059094,212.83486187121886, 0.0));
    positionAlloc4->Add (Vector(135.49905320837402,205.97063114653142, 0.0));
    positionAlloc4->Add (Vector(201.7195329817635,321.6374025359237, 0.0));
    positionAlloc4->Add (Vector(274.19427071611534,102.35989241047432, 0.0));
    positionAlloc4->Add (Vector(96.60181972192294,259.01731118605096, 0.0));
    positionAlloc4->Add (Vector(150.89937952126186,278.23966140455343, 0.0));
    positionAlloc4->Add (Vector(10.094349267048486,344.87674028934833, 0.0));
    positionAlloc4->Add (Vector(349.50117745490303,47.341538045358256, 0.0));
    positionAlloc4->Add (Vector(196.16027031954536,109.52861455437196, 0.0));
    positionAlloc4->Add (Vector(237.14651598657488,199.01469740689421, 0.0));
    positionAlloc4->Add (Vector(388.46107145387015,146.1522396906617, 0.0));
    positionAlloc4->Add (Vector(364.84146839094353,221.71190817481116, 0.0));
    positionAlloc4->Add (Vector(396.93332381561123,250.53529327419875, 0.0));
    positionAlloc4->Add (Vector(309.94967638290416,229.11265806340884, 0.0));
    positionAlloc4->Add (Vector(123.15710851052408,48.613240039489284, 0.0));
    positionAlloc4->Add (Vector(172.69287763164556,168.76705980524284, 0.0));
    positionAlloc4->Add (Vector(177.30731829749953,393.3273502937865, 0.0));
    positionAlloc4->Add (Vector(116.03541905859247,385.63485722861697, 0.0));
    positionAlloc4->Add (Vector(280.9815861438821,239.0740313209834, 0.0));
    positionAlloc4->Add (Vector(64.09677247298026,186.08376511949288, 0.0));
    positionAlloc4->Add (Vector(221.59506338641597,176.93754884311778, 0.0));
    positionAlloc4->Add (Vector(256.0419603011882,224.5745485786944, 0.0));
    positionAlloc4->Add (Vector(308.92546573510157,148.98792547813918, 0.0));
    positionAlloc4->Add (Vector(83.02634512850298,143.0608668524596, 0.0));
    positionAlloc4->Add (Vector(221.66665472469006,146.817560071674, 0.0));
    positionAlloc4->Add (Vector(102.94152590534557,93.18551215528079, 0.0));
    positionAlloc4->Add (Vector(360.12994161128285,26.651282856073834, 0.0));
    positionAlloc4->Add (Vector(395.305611984626,74.15038164561514, 0.0));
    positionAlloc4->Add (Vector(265.2935449228053,393.2891561869588, 0.0));
    positionAlloc4->Add (Vector(254.5810832587811,313.64000298713063, 0.0));
    positionAlloc4->Add (Vector(5.627103873003048,208.92045380422584, 0.0));
    positionAlloc4->Add (Vector(267.67575887682545,310.9325556491689, 0.0));
    positionAlloc4->Add (Vector(7.78121171692181,109.66858887396972, 0.0));
    positionAlloc4->Add (Vector(286.4178089180827,61.383871067930684, 0.0));
    positionAlloc4->Add (Vector(393.35288637444665,222.55394594192234, 0.0));
    positionAlloc4->Add (Vector(72.65166179621079,359.6139143720197, 0.0));
    positionAlloc4->Add (Vector(296.2994077275467,24.43059973890658, 0.0));
    positionAlloc4->Add (Vector(260.45337247041635,384.1401564031422, 0.0));
    positionAlloc4->Add (Vector(266.4419117864796,172.23705348424846, 0.0));
    positionAlloc4->Add (Vector(264.0813498193988,322.43686541625453, 0.0));
    positionAlloc4->Add (Vector(378.145449862582,60.2425806872088, 0.0));
    positionAlloc4->Add (Vector(103.80256913465864,288.00678033337306, 0.0));
    positionAlloc4->Add (Vector(11.363382914122466,305.15593493158735, 0.0));
    positionAlloc4->Add (Vector(261.91102213004854,376.48913631579325, 0.0));
    positionAlloc4->Add (Vector(348.8624341462435,33.86204947765932, 0.0));
    positionAlloc4->Add (Vector(87.7640542768083,75.54248463564326, 0.0));
    positionAlloc4->Add (Vector(111.31867837670258,395.7667478591246, 0.0));
    positionAlloc4->Add (Vector(279.4879189051222,101.63937544730861, 0.0));
    positionAlloc4->Add (Vector(266.73599215523353,200.94230200996788, 0.0));
    positionAlloc4->Add (Vector(132.2274541034029,256.35093323623806, 0.0));
    positionAlloc4->Add (Vector(101.36841071052652,273.88718567112363, 0.0));
    positionAlloc4->Add (Vector(46.96021909466239,211.47065716739414, 0.0));
    positionAlloc4->Add (Vector(110.56608552972645,106.72375617993488, 0.0));
    positionAlloc4->Add (Vector(191.35720264083508,320.0367747504841, 0.0));
    positionAlloc4->Add (Vector(278.6104791527387,250.6645908101389, 0.0));
    positionAlloc4->Add (Vector(184.25910211932273,344.5894235559024, 0.0));
    positionAlloc4->Add (Vector(251.50281528494136,33.80553624864664, 0.0));
    positionAlloc4->Add (Vector(142.19439760868218,254.60383824140456, 0.0));
    positionAlloc4->Add (Vector(39.50130562642933,372.8673252400517, 0.0));
    positionAlloc4->Add (Vector(395.60839698741336,279.93446758506406, 0.0));
    positionAlloc4->Add (Vector(321.765572859798,6.640381186033295, 0.0));
    positionAlloc4->Add (Vector(18.93284891091218,332.2351087365725, 0.0));
    positionAlloc4->Add (Vector(134.75134424814823,330.207500586909, 0.0));
    positionAlloc4->Add (Vector(130.10156963229923,19.935357156711753, 0.0));
    positionAlloc4->Add (Vector(89.53254019278529,107.97678108907367, 0.0));
    positionAlloc4->Add (Vector(204.9342357215759,10.104815806865552, 0.0));
    positionAlloc4->Add (Vector(214.59072449387295,25.378480263345125, 0.0));
    positionAlloc4->Add (Vector(86.0075768305407,96.3620570201857, 0.0));
    positionAlloc4->Add (Vector(73.63200150328582,228.17773098888762, 0.0));
    positionAlloc4->Add (Vector(127.05710448584426,126.9575157895782, 0.0));
    positionAlloc4->Add (Vector(267.69776890749966,77.51663636998694, 0.0));
    positionAlloc4->Add (Vector(78.87030464251015,397.32380369403285, 0.0));
    positionAlloc4->Add (Vector(225.72665531191439,53.674329191703364, 0.0));
    positionAlloc4->Add (Vector(365.0857609712083,132.17299380622003, 0.0));
    positionAlloc4->Add (Vector(190.46381985529135,294.09849213562205, 0.0));
    positionAlloc4->Add (Vector(57.15925303072091,22.342919221600923, 0.0));
    positionAlloc4->Add (Vector(95.23760993253126,319.82156613029315, 0.0));
    positionAlloc4->Add (Vector(272.38782913355266,189.80978178574586, 0.0));
    positionAlloc4->Add (Vector(88.82254876094633,47.94115083035759, 0.0));
    positionAlloc4->Add (Vector(127.88804017250924,170.57304511523964, 0.0));
    positionAlloc4->Add (Vector(14.013827627564268,37.96816770824223, 0.0));
    positionAlloc4->Add (Vector(353.61325967805567,345.0398909959547, 0.0));
    positionAlloc4->Add (Vector(19.692318055866707,65.60258603517069, 0.0));
    positionAlloc4->Add (Vector(247.13904407076447,184.47963022823788, 0.0));
    positionAlloc4->Add (Vector(218.54070618628808,308.546425247143, 0.0));
    positionAlloc4->Add (Vector(153.65925538162074,115.66391187125475, 0.0));
    positionAlloc4->Add (Vector(137.7860345846046,59.83862439561274, 0.0));
    positionAlloc4->Add (Vector(40.67921957537939,141.8065466552181, 0.0));
    positionAlloc4->Add (Vector(301.81133417725187,51.04963514649925, 0.0));
    positionAlloc4->Add (Vector(50.64183677107219,39.61474343763736, 0.0));
    positionAlloc4->Add (Vector(333.4496662336639,207.42160728745088, 0.0));
    positionAlloc4->Add (Vector(40.75916986912746,87.68930786233109, 0.0));
    positionAlloc4->Add (Vector(295.21233880585845,33.82798546154917, 0.0));
    positionAlloc4->Add (Vector(12.013990798171648,44.23615966793126, 0.0));
    positionAlloc4->Add (Vector(50.14226858759514,48.10701645438713, 0.0));
    positionAlloc4->Add (Vector(202.66027279801136,375.95639975691364, 0.0));
    positionAlloc4->Add (Vector(351.54061659092736,245.25631524933638, 0.0));
    positionAlloc4->Add (Vector(350.61540394298004,204.24739878665753, 0.0));
    positionAlloc4->Add (Vector(171.91934438655494,109.46942981865516, 0.0));
    positionAlloc4->Add (Vector(338.17901340349437,202.2805327971647, 0.0));
    positionAlloc4->Add (Vector(358.85115023536497,174.3272847400811, 0.0));
    positionAlloc4->Add (Vector(280.42068555630715,139.4998887866743, 0.0));
    positionAlloc4->Add (Vector(151.12724022290945,156.70867317182146, 0.0));
    positionAlloc4->Add (Vector(197.11135351345047,151.40282453717782, 0.0));
    positionAlloc4->Add (Vector(168.31457243588463,219.4389640942094, 0.0));
    positionAlloc4->Add (Vector(57.926687930780574,327.71253829905646, 0.0));
    positionAlloc4->Add (Vector(203.6844286514579,61.654180305634874, 0.0));
    positionAlloc4->Add (Vector(343.25123033228317,304.1621668095925, 0.0));
    positionAlloc4->Add (Vector(361.4248088075174,219.95764452598667, 0.0));
    positionAlloc4->Add (Vector(256.39157678658137,196.1388182128998, 0.0));
    positionAlloc4->Add (Vector(65.72880926779159,180.79282883851295, 0.0));
    positionAlloc4->Add (Vector(38.902983143157144,73.5504980886958, 0.0));
    positionAlloc4->Add (Vector(333.41061449013614,266.738373629381, 0.0));
    positionAlloc4->Add (Vector(77.9542251678505,158.78529678149812, 0.0));
    positionAlloc4->Add (Vector(278.01497534767947,124.06929541815646, 0.0));
    positionAlloc4->Add (Vector(264.78757954604305,125.39147434094579, 0.0));
    positionAlloc4->Add (Vector(230.54179189713216,117.69252940957698, 0.0));
    positionAlloc4->Add (Vector(122.7846718019578,118.0984548603425, 0.0));
    positionAlloc4->Add (Vector(218.92884140691717,394.69209522640676, 0.0));
    positionAlloc4->Add (Vector(391.9579059849483,22.334522042306617, 0.0));
    positionAlloc4->Add (Vector(76.19528421124345,361.87001693014855, 0.0));
    positionAlloc4->Add (Vector(45.71597364521196,205.33767272190994, 0.0));
    positionAlloc4->Add (Vector(135.35665630499102,185.35684547285288, 0.0));
    positionAlloc4->Add (Vector(270.9587088184354,347.5371876624829, 0.0));
    positionAlloc4->Add (Vector(335.2622353433617,178.1171881194036, 0.0));
    positionAlloc4->Add (Vector(46.230963304068325,31.890510388975592, 0.0));
    positionAlloc4->Add (Vector(240.41585683647,76.6008650995536, 0.0));
    positionAlloc4->Add (Vector(314.30758507094305,369.02679851907743, 0.0));
    positionAlloc4->Add (Vector(186.5122453774109,46.24464914712174, 0.0));
    positionAlloc4->Add (Vector(126.23288261106187,205.11606838081042, 0.0));
    positionAlloc4->Add (Vector(195.31304161458854,322.14305078590314, 0.0));
    positionAlloc4->Add (Vector(40.91562601662599,248.4960348304562, 0.0));
    positionAlloc4->Add (Vector(352.39986074481305,140.49099397799293, 0.0));
    positionAlloc4->Add (Vector(191.90309002417035,366.50305126618156, 0.0));
    positionAlloc4->Add (Vector(79.90568887060685,23.631338916040036, 0.0));
    positionAlloc4->Add (Vector(253.57445108007482,87.86690265374611, 0.0));
    positionAlloc4->Add (Vector(331.9061062962685,41.81693762093337, 0.0));
    positionAlloc4->Add (Vector(164.22093830986398,278.831647983651, 0.0));
    positionAlloc4->Add (Vector(166.90886727337002,269.2838574486184, 0.0));
    positionAlloc4->Add (Vector(350.31373950591103,101.93907676162026, 0.0));
    positionAlloc4->Add (Vector(273.68736654273806,263.03131745558915, 0.0));
    positionAlloc4->Add (Vector(396.54529164721686,35.94552595677003, 0.0));
    positionAlloc4->Add (Vector(236.0064137277572,114.8015975497104, 0.0));
    positionAlloc4->Add (Vector(34.763422262714585,44.17787644619504, 0.0));
    positionAlloc4->Add (Vector(127.90967217201508,123.56152458530528, 0.0));
    positionAlloc4->Add (Vector(45.674557902150916,267.6808978227036, 0.0));
    positionAlloc4->Add (Vector(253.24158170918562,344.3712432705304, 0.0));
    positionAlloc4->Add (Vector(45.417562599129944,58.09762276643471, 0.0));
    positionAlloc4->Add (Vector(231.09197229845978,365.6322082494022, 0.0));
    positionAlloc4->Add (Vector(284.19455242800694,341.7302340497991, 0.0));
    positionAlloc4->Add (Vector(292.66003283637076,148.43365886018853, 0.0));
    positionAlloc4->Add (Vector(394.6700808000147,303.3443451836674, 0.0));
    positionAlloc4->Add (Vector(384.5092502581851,105.49589691852721, 0.0));
    positionAlloc4->Add (Vector(238.8324020777391,70.0748425812169, 0.0));
    positionAlloc4->Add (Vector(152.2741157740662,391.6318452678146, 0.0));
    positionAlloc4->Add (Vector(4.201664399584626,365.88280248526354, 0.0));
    positionAlloc4->Add (Vector(183.3829644891879,30.560375799375493, 0.0));
    positionAlloc4->Add (Vector(160.50826677002883,209.23369255909589, 0.0));
    positionAlloc4->Add (Vector(6.049437761136334,252.85965109312338, 0.0));
    positionAlloc4->Add (Vector(254.1856041803418,38.97423512052387, 0.0));
    positionAlloc4->Add (Vector(318.032148683713,33.5389728349309, 0.0));
    positionAlloc4->Add (Vector(7.71210984456121,145.1052619737409, 0.0));
    positionAlloc4->Add (Vector(60.81936929947247,34.375802493612895, 0.0));
    positionAlloc4->Add (Vector(96.15771157614277,334.08997122444987, 0.0));
    positionAlloc4->Add (Vector(150.83671114954805,304.86540465680577, 0.0));
    positionAlloc4->Add (Vector(154.925223068191,107.76044473260447, 0.0));
    positionAlloc4->Add (Vector(125.22608287522235,227.820502348865, 0.0));
    positionAlloc4->Add (Vector(180.42557396968348,44.41838473603181, 0.0));
    positionAlloc4->Add (Vector(133.85446869199015,260.9882063061959, 0.0));
    positionAlloc4->Add (Vector(260.6688769237988,226.36105772753456, 0.0));
    positionAlloc4->Add (Vector(57.4620197902795,106.87936459156649, 0.0));
    positionAlloc4->Add (Vector(243.51584451217087,92.83828239449522, 0.0));
    positionAlloc4->Add (Vector(168.51184038928798,316.30117759661215, 0.0));
    positionAlloc4->Add (Vector(329.4274340874607,216.31443155983155, 0.0));
    positionAlloc4->Add (Vector(41.840943638928074,380.5560019480592, 0.0));
    positionAlloc4->Add (Vector(239.67422078010281,24.009657772220194, 0.0));
    positionAlloc4->Add (Vector(146.6192799171891,136.52471031935397, 0.0));
    positionAlloc4->Add (Vector(330.74385635723246,349.7061152637092, 0.0));
    positionAlloc4->Add (Vector(70.32915771899573,88.92007007665201, 0.0));
    positionAlloc4->Add (Vector(81.59272831351578,69.86307776199214, 0.0));
    positionAlloc4->Add (Vector(141.9146883702742,121.13986454298535, 0.0));
    positionAlloc4->Add (Vector(248.40539732125842,213.57504880830035, 0.0));
    positionAlloc4->Add (Vector(21.454734790429917,0.4835494388993222, 0.0));
    positionAlloc4->Add (Vector(94.88586735170674,123.53260930872865, 0.0));
    positionAlloc4->Add (Vector(152.3975935597653,18.449887518288577, 0.0));
    positionAlloc4->Add (Vector(278.7084896374586,268.9290057393331, 0.0));
    positionAlloc4->Add (Vector(116.80524040412381,365.19577360630626, 0.0));
    positionAlloc4->Add (Vector(276.63002822263536,165.23570654273846, 0.0));
    positionAlloc4->Add (Vector(361.1930781164942,219.92520742638578, 0.0));
    positionAlloc4->Add (Vector(279.44873490110217,186.79681185421327, 0.0));
    positionAlloc4->Add (Vector(136.07236700946052,296.78733694025226, 0.0));
    positionAlloc4->Add (Vector(67.41901674321618,87.00254031947776, 0.0));
    positionAlloc4->Add (Vector(183.5932348735688,302.67700513081894, 0.0));
    positionAlloc4->Add (Vector(13.701694408583798,329.86994144864883, 0.0));
    positionAlloc4->Add (Vector(283.59893152801584,136.85370064976522, 0.0));
    positionAlloc4->Add (Vector(206.35190467513326,275.55822564032565, 0.0));
    positionAlloc4->Add (Vector(74.49339511527162,245.56515020664972, 0.0));
    positionAlloc4->Add (Vector(304.4324591564448,175.2126057431195, 0.0));
    positionAlloc4->Add (Vector(74.36284324356208,136.9882198417289, 0.0));
    positionAlloc4->Add (Vector(246.83312620436394,310.967156230541, 0.0));
    positionAlloc4->Add (Vector(93.81124045594942,79.41699706102696, 0.0));
    positionAlloc4->Add (Vector(19.74733649235656,186.3533638493104, 0.0));
    positionAlloc4->Add (Vector(315.51694493032295,100.13368105085232, 0.0));
    positionAlloc4->Add (Vector(381.70388898632905,7.421249343632486, 0.0));
    positionAlloc4->Add (Vector(2.383255014233665,88.91188353287367, 0.0));
    positionAlloc4->Add (Vector(155.6574900991851,36.93116810806609, 0.0));
    positionAlloc4->Add (Vector(51.09119076617596,87.48488310134009, 0.0));
    positionAlloc4->Add (Vector(165.98953882047778,352.0967713365809, 0.0));
    positionAlloc4->Add (Vector(146.77674230673367,375.3377145425744, 0.0));
    positionAlloc4->Add (Vector(296.9080932486538,238.70422755198265, 0.0));
    positionAlloc4->Add (Vector(12.776019807435146,296.2215011058509, 0.0));
    positionAlloc4->Add (Vector(233.04328725696223,282.5387271103155, 0.0));
    positionAlloc4->Add (Vector(269.45476344669663,322.5631695236035, 0.0));
    positionAlloc4->Add (Vector(280.43072230559056,302.15873693010315, 0.0));
    positionAlloc4->Add (Vector(253.25529632125873,43.16675363420956, 0.0));
    positionAlloc4->Add (Vector(236.35408991402502,345.82916523906255, 0.0));
    positionAlloc4->Add (Vector(392.08423816463227,228.19086505215057, 0.0));
    positionAlloc4->Add (Vector(365.52081716900375,355.85311424412146, 0.0));
    positionAlloc4->Add (Vector(39.0899949589262,287.4104092342696, 0.0));
    positionAlloc4->Add (Vector(368.2962986070217,202.6577572647729, 0.0));
    positionAlloc4->Add (Vector(176.28724240387302,143.98993414655226, 0.0));
    positionAlloc4->Add (Vector(233.73569280479342,158.1644828928865, 0.0));
    positionAlloc4->Add (Vector(29.22165289316463,358.4544983468293, 0.0));
    positionAlloc4->Add (Vector(223.09330682130897,260.6604244819716, 0.0));
    positionAlloc4->Add (Vector(258.570434545602,331.7220097818301, 0.0));
    positionAlloc4->Add (Vector(257.31332798757956,121.49924259537386, 0.0));
    positionAlloc4->Add (Vector(213.0622702793384,305.5625193996607, 0.0));
    positionAlloc4->Add (Vector(258.5061866166702,232.78055677318662, 0.0));
    positionAlloc4->Add (Vector(102.7431285576601,153.72203827214474, 0.0));
    positionAlloc4->Add (Vector(345.9497849778084,380.53223375394504, 0.0));
    positionAlloc4->Add (Vector(126.89869055600069,67.9930433004746, 0.0));
    positionAlloc4->Add (Vector(218.29223093806976,289.22137535862163, 0.0));
    positionAlloc4->Add (Vector(132.00980876364036,25.614851525361804, 0.0));
    positionAlloc4->Add (Vector(267.3043911131423,146.00198225742372, 0.0));
    positionAlloc4->Add (Vector(137.98904756404,371.50677548372863, 0.0));
    positionAlloc4->Add (Vector(135.1873334488578,82.2772103503063, 0.0));
    positionAlloc4->Add (Vector(338.94099646891993,50.936838377975405, 0.0));
    positionAlloc4->Add (Vector(253.08692552909062,190.3634073335047, 0.0));
    positionAlloc4->Add (Vector(325.5321872135818,322.74544289111793, 0.0));
    positionAlloc4->Add (Vector(140.26656067640016,192.0329382925864, 0.0));
    positionAlloc4->Add (Vector(196.39359560035632,98.95809504322801, 0.0));
    positionAlloc4->Add (Vector(292.92285902200643,191.57810877166912, 0.0));
    positionAlloc4->Add (Vector(316.3596774991724,244.54930321828883, 0.0));
    positionAlloc4->Add (Vector(154.07183159404792,78.25842226170607, 0.0));
    positionAlloc4->Add (Vector(111.47696042296498,382.42441606454264, 0.0));
    positionAlloc4->Add (Vector(255.80171803124153,231.84953486631176, 0.0));
    positionAlloc4->Add (Vector(269.1744098363951,134.71067105442847, 0.0));
    positionAlloc4->Add (Vector(130.3613291875876,334.2437749474854, 0.0));
    positionAlloc4->Add (Vector(192.36671774073127,227.46036945280483, 0.0));
    positionAlloc4->Add (Vector(281.41132252032077,249.4740324510203, 0.0));
    positionAlloc4->Add (Vector(18.058382902940906,129.40586480312945, 0.0));
    positionAlloc4->Add (Vector(246.87255722718487,159.75617537962776, 0.0));
    positionAlloc4->Add (Vector(174.24001980733976,285.556849102069, 0.0));
    positionAlloc4->Add (Vector(289.0966167394859,14.936121956673931, 0.0));
    positionAlloc4->Add (Vector(187.8139564528987,191.06431882611673, 0.0));
    positionAlloc4->Add (Vector(225.01762254636986,15.785318114244662, 0.0));
    positionAlloc4->Add (Vector(166.82854636676848,23.78591095660454, 0.0));
    positionAlloc4->Add (Vector(307.18942026538593,86.0181874628955, 0.0));
    positionAlloc4->Add (Vector(322.0250719301803,342.7912477093527, 0.0));
    positionAlloc4->Add (Vector(35.981029073955284,192.20422081625648, 0.0));
    positionAlloc4->Add (Vector(332.38327454485284,48.463959232786195, 0.0));
    positionAlloc4->Add (Vector(149.68780591115635,143.9225544377564, 0.0));
    positionAlloc4->Add (Vector(103.4273900818599,273.8920880341216, 0.0));
    positionAlloc4->Add (Vector(69.37496940388365,372.5699654704829, 0.0));
    positionAlloc4->Add (Vector(183.74880648488752,272.42550347431074, 0.0));
    positionAlloc4->Add (Vector(294.16621874174547,195.17910318669558, 0.0));
    positionAlloc4->Add (Vector(133.8390631814173,103.09989690108918, 0.0));
    positionAlloc4->Add (Vector(19.25194559226817,124.02229068365828, 0.0));
    positionAlloc4->Add (Vector(27.747278185529645,341.9562042443067, 0.0));
    positionAlloc4->Add (Vector(95.46067692328424,118.48664798336759, 0.0));
    positionAlloc4->Add (Vector(379.4224895149948,307.22119190279085, 0.0));
    positionAlloc4->Add (Vector(180.7781695465622,202.1236265917252, 0.0));
    positionAlloc4->Add (Vector(315.91057383435367,98.17890102648548, 0.0));
    positionAlloc4->Add (Vector(366.47963599236857,95.49050822358404, 0.0));
    positionAlloc4->Add (Vector(226.46678013827275,363.7526765536243, 0.0));
    positionAlloc4->Add (Vector(214.44557962922119,334.232040893272, 0.0));
    positionAlloc4->Add (Vector(98.04873098170029,202.0314128445547, 0.0));
    positionAlloc4->Add (Vector(121.26813962892697,205.77176254135202, 0.0));
    positionAlloc4->Add (Vector(174.08371046805993,386.4132171613024, 0.0));
    positionAlloc4->Add (Vector(310.6539853248865,296.92427968420765, 0.0));
    positionAlloc4->Add (Vector(199.92842816137522,65.5342516265161, 0.0));
    positionAlloc4->Add (Vector(1.1759057157963326,392.7077634648198, 0.0));
    positionAlloc4->Add (Vector(279.1160538624378,370.4959716212874, 0.0));
    positionAlloc4->Add (Vector(351.40891232740853,48.90715244839421, 0.0));
    positionAlloc4->Add (Vector(182.62974561172487,269.7123407937831, 0.0));
    positionAlloc4->Add (Vector(168.9194010234238,246.3757128995166, 0.0));
    positionAlloc4->Add (Vector(156.48621293553933,162.24525234068477, 0.0));
    positionAlloc4->Add (Vector(214.44954211271542,277.0524063059869, 0.0));
    positionAlloc4->Add (Vector(391.84442616848804,96.95059683619948, 0.0));
    positionAlloc4->Add (Vector(385.0881310873952,334.31917634936923, 0.0));
    positionAlloc4->Add (Vector(298.51800431743396,113.35401967868184, 0.0));
    positionAlloc4->Add (Vector(80.28086758458635,91.37147831346013, 0.0));
    positionAlloc4->Add (Vector(237.97433516276496,125.83530641840971, 0.0));
    positionAlloc4->Add (Vector(375.2900907865832,380.99703997464115, 0.0));
    positionAlloc4->Add (Vector(379.51093104337315,369.32403683837396, 0.0));
    positionAlloc4->Add (Vector(2.4442734113148568,60.852882861030764, 0.0));
    positionAlloc4->Add (Vector(26.488231776499347,114.02305774851587, 0.0));
    positionAlloc4->Add (Vector(94.89621915468591,206.51264199834455, 0.0));
    positionAlloc4->Add (Vector(8.91016039220327,33.652888792689595, 0.0));
    positionAlloc4->Add (Vector(278.38004676029715,15.758306763093488, 0.0));
    positionAlloc4->Add (Vector(16.2407352797187,334.1562552195951, 0.0));
    positionAlloc4->Add (Vector(218.2830587480491,210.72720042674766, 0.0));
    positionAlloc4->Add (Vector(378.1640326859434,120.34999608818654, 0.0));
    positionAlloc4->Add (Vector(320.39137637361017,287.06059768407033, 0.0));
    positionAlloc4->Add (Vector(353.65165523457864,102.61911377752155, 0.0));
    positionAlloc4->Add (Vector(353.8489346138823,27.724929810782186, 0.0));
    positionAlloc4->Add (Vector(197.7710129318956,282.9318652362399, 0.0));
    positionAlloc4->Add (Vector(33.796542536474796,53.50134445926833, 0.0));
    positionAlloc4->Add (Vector(300.55906361538956,179.36264893940464, 0.0));
    positionAlloc4->Add (Vector(254.3989346857226,382.7373014225886, 0.0));
    positionAlloc4->Add (Vector(353.5599867441141,201.4741993667332, 0.0));
    positionAlloc4->Add (Vector(32.671165830082,377.25761516150686, 0.0));
    positionAlloc4->Add (Vector(296.1761460428069,303.1753953830707, 0.0));
    positionAlloc4->Add (Vector(201.49174632988647,176.94930782838298, 0.0));
    positionAlloc4->Add (Vector(217.71201461708003,260.2214392128927, 0.0));
    positionAlloc4->Add (Vector(20.576181306912833,229.1577771908542, 0.0));
    positionAlloc4->Add (Vector(363.0570661406346,36.74541963589544, 0.0));
    positionAlloc4->Add (Vector(320.2148163901023,305.6302088211212, 0.0));
    positionAlloc4->Add (Vector(373.9131291757066,20.87809766780535, 0.0));
    positionAlloc4->Add (Vector(325.4369475962785,371.5405723272191, 0.0));
    positionAlloc4->Add (Vector(247.8953288346623,81.10855928815957, 0.0));
    positionAlloc4->Add (Vector(244.38338485333682,272.75881310673907, 0.0));
    positionAlloc4->Add (Vector(217.44455055061195,193.63372324265634, 0.0));
    positionAlloc4->Add (Vector(269.5232774108794,12.060063330658988, 0.0));
    positionAlloc4->Add (Vector(222.2036308541168,278.3555147947868, 0.0));
    positionAlloc4->Add (Vector(50.05097750119778,243.91891982902783, 0.0));
    positionAlloc4->Add (Vector(301.580580216319,223.7463054135107, 0.0));
    positionAlloc4->Add (Vector(38.782039129521316,67.08609728867181, 0.0));
    positionAlloc4->Add (Vector(264.04453425558927,131.6008065224577, 0.0));
    positionAlloc4->Add (Vector(104.97565495037708,346.3445617372819, 0.0));
    positionAlloc4->Add (Vector(296.1901784067423,225.88474715234898, 0.0));
    positionAlloc4->Add (Vector(379.49981469463347,245.848548358661, 0.0));
    positionAlloc4->Add (Vector(108.25856428874911,204.09717202127248, 0.0));
    positionAlloc4->Add (Vector(19.211405836194473,83.85297936699607, 0.0));
    positionAlloc4->Add (Vector(181.64914399588275,37.444889548842575, 0.0));
    positionAlloc4->Add (Vector(108.84187600194353,386.1859938455991, 0.0));
    positionAlloc4->Add (Vector(258.74791665315314,318.4804795968436, 0.0));
    positionAlloc4->Add (Vector(373.2500834307909,71.86313118799137, 0.0));
    positionAlloc4->Add (Vector(96.80457626578813,375.6764724310198, 0.0));
    positionAlloc4->Add (Vector(356.751258171789,19.011378056826267, 0.0));
    positionAlloc4->Add (Vector(232.12342267862178,360.2605976999215, 0.0));
    positionAlloc4->Add (Vector(311.97512609419283,286.46753468492034, 0.0));
    positionAlloc4->Add (Vector(308.5808486209412,293.6638412293853, 0.0));
    positionAlloc4->Add (Vector(296.27710878622844,370.90922817895625, 0.0));
    positionAlloc4->Add (Vector(388.92854741823936,226.66367004415474, 0.0));
    positionAlloc4->Add (Vector(383.1817859196044,107.4867344479186, 0.0));
    positionAlloc4->Add (Vector(42.91916840557981,256.79578622292547, 0.0));
    positionAlloc4->Add (Vector(390.5996616635013,10.957740825365558, 0.0));
    positionAlloc4->Add (Vector(5.98117261294977,176.8194743071201, 0.0));
    positionAlloc4->Add (Vector(220.3247116921462,197.25599558041895, 0.0));
    positionAlloc4->Add (Vector(274.23657029106766,354.06554572271546, 0.0));
    positionAlloc4->Add (Vector(106.02678526312292,97.85611114765587, 0.0));
    positionAlloc4->Add (Vector(85.2829747755516,160.4709940959574, 0.0));
    positionAlloc4->Add (Vector(257.71444082618024,367.35496099968924, 0.0));
    positionAlloc4->Add (Vector(231.42485444956736,32.04606613210497, 0.0));
    positionAlloc4->Add (Vector(160.92183159860625,92.23733159203968, 0.0));
    positionAlloc4->Add (Vector(216.74283422970285,224.59122570410906, 0.0));
    positionAlloc4->Add (Vector(76.56669443659685,390.75444826653086, 0.0));
    positionAlloc4->Add (Vector(378.06531890126746,45.376851081812305, 0.0));
    positionAlloc4->Add (Vector(355.46456166368483,370.6272267341142, 0.0));
    positionAlloc4->Add (Vector(392.1522951492619,0.4770103158209693, 0.0));
    positionAlloc4->Add (Vector(240.82590770135505,397.27890583813974, 0.0));
    positionAlloc4->Add (Vector(97.41537973084422,379.52109569703157, 0.0));
    positionAlloc4->Add (Vector(222.19164535856243,132.72381259845565, 0.0));
    positionAlloc4->Add (Vector(238.2192690190644,229.48666422719097, 0.0));
    positionAlloc4->Add (Vector(375.68815739476184,156.82228258646217, 0.0));
    positionAlloc4->Add (Vector(150.47826015102913,337.4597542435322, 0.0));
    positionAlloc4->Add (Vector(175.81655984072307,371.23589321810624, 0.0));
    positionAlloc4->Add (Vector(87.02934618722766,58.91817002120207, 0.0));
    positionAlloc4->Add (Vector(378.6295961736541,207.30407745050857, 0.0));
    positionAlloc4->Add (Vector(149.67499884296308,241.43787856552626, 0.0));
    positionAlloc4->Add (Vector(333.85388563622007,43.11187877205973, 0.0));
    positionAlloc4->Add (Vector(281.2293616362248,388.06956047625147, 0.0));
    positionAlloc4->Add (Vector(6.991180274040465,326.23990081830476, 0.0));
    positionAlloc4->Add (Vector(372.24012393005586,193.00660808502914, 0.0));
    positionAlloc4->Add (Vector(278.3473623016591,280.43696408654955, 0.0));
    positionAlloc4->Add (Vector(342.25363880415915,108.03048855825841, 0.0));
    positionAlloc4->Add (Vector(44.32894706719366,318.7097397877905, 0.0));
    positionAlloc4->Add (Vector(296.01205665750365,294.4130828410998, 0.0));
    positionAlloc4->Add (Vector(20.87085900510455,149.18806899247096, 0.0));
    positionAlloc4->Add (Vector(22.193242185244433,340.5927314553855, 0.0));
    positionAlloc4->Add (Vector(92.62975969627493,241.86584303663756, 0.0));
    positionAlloc4->Add (Vector(368.87206921101387,68.61356334531013, 0.0));
    positionAlloc4->Add (Vector(29.904572731234815,277.05661900090496, 0.0));
    positionAlloc4->Add (Vector(116.15462786362004,251.87702057713142, 0.0));
    positionAlloc4->Add (Vector(141.58521494770474,339.7324862237585, 0.0));
    positionAlloc4->Add (Vector(356.0914069985143,48.68674591531361, 0.0));
    positionAlloc4->Add (Vector(133.19484802734252,389.6215671454016, 0.0));
    positionAlloc4->Add (Vector(356.53054086817957,203.53196372950157, 0.0));
    positionAlloc4->Add (Vector(314.7106412244592,5.756294333501, 0.0));
    positionAlloc4->Add (Vector(113.7524281757818,72.73006914969851, 0.0));
    positionAlloc4->Add (Vector(239.2591621336536,222.88545533219636, 0.0));
    positionAlloc4->Add (Vector(19.631021211049138,278.95592199869924, 0.0));
    positionAlloc4->Add (Vector(171.1022595827941,278.182384093304, 0.0));
    positionAlloc4->Add (Vector(333.0629300341037,357.9866241262587, 0.0));
    positionAlloc4->Add (Vector(110.84101809908047,355.04485533064053, 0.0));
    positionAlloc4->Add (Vector(98.05723410283176,241.9170752458118, 0.0));
    positionAlloc4->Add (Vector(330.40604013849475,176.6576945175993, 0.0));
    positionAlloc4->Add (Vector(211.59312143380924,25.024177373584557, 0.0));
    positionAlloc4->Add (Vector(72.65861991304905,168.58710277948487, 0.0));
    positionAlloc4->Add (Vector(97.97409008705174,257.29447666126964, 0.0));
    positionAlloc4->Add (Vector(194.33370769724178,78.12062658556167, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_24.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_24.flowmon", false, false);
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