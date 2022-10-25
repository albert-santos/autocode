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
	  uint16_t numberOfRrhs = 22;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 584;
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
    positionAlloc->Add (Vector (356.6666666666667,10.0, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,140.0, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (270.0,270.0, 0.0));
    positionAlloc->Add (Vector (270.0,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,270.0, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (140.0,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (140.0,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (140.0,10.0, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,270.0, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,10.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,140.0, 0.0));
    positionAlloc->Add (Vector (10.0,53.333333333333336, 0.0));
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
    positionAlloc4->Add (Vector(203.7105777022503,339.6857588512979, 0.0));
    positionAlloc4->Add (Vector(159.03361831729498,102.23373188247761, 0.0));
    positionAlloc4->Add (Vector(43.430538278355655,378.91086919099786, 0.0));
    positionAlloc4->Add (Vector(197.26354634930345,367.47505197267094, 0.0));
    positionAlloc4->Add (Vector(325.3760857377844,32.972960397053484, 0.0));
    positionAlloc4->Add (Vector(170.63443794266738,335.62485619302817, 0.0));
    positionAlloc4->Add (Vector(39.25092068628469,123.26427770775497, 0.0));
    positionAlloc4->Add (Vector(102.50999802837732,236.30486744181113, 0.0));
    positionAlloc4->Add (Vector(247.70645688307474,260.392460169885, 0.0));
    positionAlloc4->Add (Vector(298.6722848271927,375.62585043686994, 0.0));
    positionAlloc4->Add (Vector(107.77924158253218,42.73429768461688, 0.0));
    positionAlloc4->Add (Vector(140.39900322669325,69.07224825192783, 0.0));
    positionAlloc4->Add (Vector(283.89378238074283,361.1566476968084, 0.0));
    positionAlloc4->Add (Vector(329.6011767806801,55.533163652469895, 0.0));
    positionAlloc4->Add (Vector(27.85378784366288,194.38955008504996, 0.0));
    positionAlloc4->Add (Vector(157.95744495739666,159.37467669512282, 0.0));
    positionAlloc4->Add (Vector(350.26658489089465,10.210886491863436, 0.0));
    positionAlloc4->Add (Vector(110.84628353498096,225.90114997692046, 0.0));
    positionAlloc4->Add (Vector(387.1162261794594,22.96460983358304, 0.0));
    positionAlloc4->Add (Vector(68.82186829696235,305.41582980766213, 0.0));
    positionAlloc4->Add (Vector(120.42726096867496,182.87715255890967, 0.0));
    positionAlloc4->Add (Vector(331.5109246462552,242.47393171077363, 0.0));
    positionAlloc4->Add (Vector(218.31682273446594,10.913970460840705, 0.0));
    positionAlloc4->Add (Vector(14.046221134313575,95.5149686861943, 0.0));
    positionAlloc4->Add (Vector(312.2039294776621,383.0069533740986, 0.0));
    positionAlloc4->Add (Vector(270.8127146248441,107.52286187960122, 0.0));
    positionAlloc4->Add (Vector(119.43062568528356,350.2061858360786, 0.0));
    positionAlloc4->Add (Vector(216.5528239017181,181.6359669032309, 0.0));
    positionAlloc4->Add (Vector(92.41562393880609,42.85393529219492, 0.0));
    positionAlloc4->Add (Vector(103.57316387896378,271.5626709304734, 0.0));
    positionAlloc4->Add (Vector(211.63453941494632,93.62520971491386, 0.0));
    positionAlloc4->Add (Vector(199.46629222146143,208.27382072604962, 0.0));
    positionAlloc4->Add (Vector(80.81038668366713,260.41716341095963, 0.0));
    positionAlloc4->Add (Vector(42.7546116792858,137.71109682985866, 0.0));
    positionAlloc4->Add (Vector(261.81404714692576,328.7935760752899, 0.0));
    positionAlloc4->Add (Vector(3.2145969155617404,395.1792533740381, 0.0));
    positionAlloc4->Add (Vector(371.82032983160013,191.33249469469135, 0.0));
    positionAlloc4->Add (Vector(282.4700061233087,7.563220597647691, 0.0));
    positionAlloc4->Add (Vector(376.37947299984444,42.29252999033459, 0.0));
    positionAlloc4->Add (Vector(185.9197282705475,8.810302972590645, 0.0));
    positionAlloc4->Add (Vector(88.6433875964102,281.6215787188447, 0.0));
    positionAlloc4->Add (Vector(271.92527515674476,33.106692557742875, 0.0));
    positionAlloc4->Add (Vector(42.028060474173,33.264777754532474, 0.0));
    positionAlloc4->Add (Vector(148.93808381854575,129.39282061931777, 0.0));
    positionAlloc4->Add (Vector(368.44286241579323,1.3913407935696398, 0.0));
    positionAlloc4->Add (Vector(290.2668137824311,292.1863494247079, 0.0));
    positionAlloc4->Add (Vector(123.12819976843431,49.40083090311447, 0.0));
    positionAlloc4->Add (Vector(206.41287352721963,55.31963376258573, 0.0));
    positionAlloc4->Add (Vector(15.780492968134041,2.0435561626333865, 0.0));
    positionAlloc4->Add (Vector(21.88643648942854,206.63240513553913, 0.0));
    positionAlloc4->Add (Vector(64.72283352143239,303.7405212805706, 0.0));
    positionAlloc4->Add (Vector(101.22588838621893,159.73250222016406, 0.0));
    positionAlloc4->Add (Vector(327.74543661717024,39.89413363646661, 0.0));
    positionAlloc4->Add (Vector(23.815967810117613,224.7388287316844, 0.0));
    positionAlloc4->Add (Vector(52.830642923487005,265.4645929524716, 0.0));
    positionAlloc4->Add (Vector(320.47047564940505,295.6210098524884, 0.0));
    positionAlloc4->Add (Vector(29.160902891201257,230.16873867617673, 0.0));
    positionAlloc4->Add (Vector(250.97399432753065,148.64699332060613, 0.0));
    positionAlloc4->Add (Vector(44.00648389901103,122.21918240601343, 0.0));
    positionAlloc4->Add (Vector(73.77841214070497,69.41290486423131, 0.0));
    positionAlloc4->Add (Vector(294.5250554946249,212.50224951867796, 0.0));
    positionAlloc4->Add (Vector(301.44822571945207,54.04935327406788, 0.0));
    positionAlloc4->Add (Vector(302.7933140690099,318.2698956450109, 0.0));
    positionAlloc4->Add (Vector(1.9435804007573054,187.23529877204416, 0.0));
    positionAlloc4->Add (Vector(92.41745030890387,336.7086932067832, 0.0));
    positionAlloc4->Add (Vector(389.84567052118945,38.413920969702886, 0.0));
    positionAlloc4->Add (Vector(348.3246240914366,9.68030302499705, 0.0));
    positionAlloc4->Add (Vector(230.89728330994794,137.18459515155095, 0.0));
    positionAlloc4->Add (Vector(194.30422527889078,251.95248419178444, 0.0));
    positionAlloc4->Add (Vector(268.3366827766041,191.38001486224456, 0.0));
    positionAlloc4->Add (Vector(303.0797365002468,64.42572176613344, 0.0));
    positionAlloc4->Add (Vector(238.82580978610713,271.63293907562047, 0.0));
    positionAlloc4->Add (Vector(47.68047650564604,94.23836465921967, 0.0));
    positionAlloc4->Add (Vector(127.41623903667394,390.08045400569904, 0.0));
    positionAlloc4->Add (Vector(352.41955456372966,82.71479204247929, 0.0));
    positionAlloc4->Add (Vector(188.10982453024891,188.23013258281654, 0.0));
    positionAlloc4->Add (Vector(265.30556178436655,17.544174834932313, 0.0));
    positionAlloc4->Add (Vector(95.16349405046753,251.5690759129588, 0.0));
    positionAlloc4->Add (Vector(367.1423392944809,285.07425575908496, 0.0));
    positionAlloc4->Add (Vector(193.98605318028555,368.48816091860573, 0.0));
    positionAlloc4->Add (Vector(327.24535135684147,399.4875263846108, 0.0));
    positionAlloc4->Add (Vector(121.8990790733411,316.0640613811393, 0.0));
    positionAlloc4->Add (Vector(97.39877314850412,20.763384841938137, 0.0));
    positionAlloc4->Add (Vector(147.76192247618928,86.70349915938687, 0.0));
    positionAlloc4->Add (Vector(157.16725214104704,273.50121455710826, 0.0));
    positionAlloc4->Add (Vector(200.07858271903322,0.8998592374512171, 0.0));
    positionAlloc4->Add (Vector(31.230613647835213,204.61462815885204, 0.0));
    positionAlloc4->Add (Vector(301.32962437642277,127.76655668746093, 0.0));
    positionAlloc4->Add (Vector(69.02456851197924,114.97589027906253, 0.0));
    positionAlloc4->Add (Vector(332.38224611357987,36.597018768701474, 0.0));
    positionAlloc4->Add (Vector(170.57574832969667,55.57839066075698, 0.0));
    positionAlloc4->Add (Vector(232.97704317501035,50.15884536570088, 0.0));
    positionAlloc4->Add (Vector(256.274706395705,207.1987142450698, 0.0));
    positionAlloc4->Add (Vector(254.44430758189557,54.34943463341418, 0.0));
    positionAlloc4->Add (Vector(386.4626332389137,374.5113695528889, 0.0));
    positionAlloc4->Add (Vector(395.668269530963,180.39366633379242, 0.0));
    positionAlloc4->Add (Vector(94.17796772662723,359.1253967378874, 0.0));
    positionAlloc4->Add (Vector(377.4634887858356,100.13918679182319, 0.0));
    positionAlloc4->Add (Vector(221.02538278630678,224.96300176556926, 0.0));
    positionAlloc4->Add (Vector(256.3978788617282,294.57125866414555, 0.0));
    positionAlloc4->Add (Vector(183.59197127963202,49.293713249885315, 0.0));
    positionAlloc4->Add (Vector(379.50414634888307,311.64334123627214, 0.0));
    positionAlloc4->Add (Vector(13.937949668586258,112.14669783517795, 0.0));
    positionAlloc4->Add (Vector(336.73166944184476,347.255555081859, 0.0));
    positionAlloc4->Add (Vector(247.33213181811533,184.3985359202044, 0.0));
    positionAlloc4->Add (Vector(205.57500969012327,321.80094180351375, 0.0));
    positionAlloc4->Add (Vector(103.35564422402364,323.8064683932072, 0.0));
    positionAlloc4->Add (Vector(14.905368948367537,311.3207040860462, 0.0));
    positionAlloc4->Add (Vector(327.8865103649354,236.85249831003418, 0.0));
    positionAlloc4->Add (Vector(392.61772003225684,127.30216264665093, 0.0));
    positionAlloc4->Add (Vector(331.46500824591897,343.3808282915081, 0.0));
    positionAlloc4->Add (Vector(167.8266458186001,366.71091581743843, 0.0));
    positionAlloc4->Add (Vector(184.88843505886462,53.19303114374412, 0.0));
    positionAlloc4->Add (Vector(298.94453104060614,273.5946517334753, 0.0));
    positionAlloc4->Add (Vector(349.15877502994607,310.7120505697335, 0.0));
    positionAlloc4->Add (Vector(285.72277860022825,395.20701728163215, 0.0));
    positionAlloc4->Add (Vector(277.5154660343292,336.33336940998163, 0.0));
    positionAlloc4->Add (Vector(117.69418334390957,386.4313200146685, 0.0));
    positionAlloc4->Add (Vector(84.20653077604183,195.86821928479975, 0.0));
    positionAlloc4->Add (Vector(152.65779158130633,265.9015579160808, 0.0));
    positionAlloc4->Add (Vector(278.373613949382,383.46243449797345, 0.0));
    positionAlloc4->Add (Vector(343.7366348179452,15.268397320282645, 0.0));
    positionAlloc4->Add (Vector(176.09280907910278,354.73131198004825, 0.0));
    positionAlloc4->Add (Vector(34.88746146067756,166.74174553438954, 0.0));
    positionAlloc4->Add (Vector(306.87113712877346,195.79566781255275, 0.0));
    positionAlloc4->Add (Vector(230.18435997926017,177.7212773562395, 0.0));
    positionAlloc4->Add (Vector(45.35714140099909,224.511874104138, 0.0));
    positionAlloc4->Add (Vector(390.9907538628406,136.5094964118605, 0.0));
    positionAlloc4->Add (Vector(195.41309679331337,343.0180963100045, 0.0));
    positionAlloc4->Add (Vector(342.56463258753627,377.4437904684033, 0.0));
    positionAlloc4->Add (Vector(340.5398186945574,76.01264303943465, 0.0));
    positionAlloc4->Add (Vector(75.40728628748616,323.5150020102282, 0.0));
    positionAlloc4->Add (Vector(154.95819915102462,30.88728407907455, 0.0));
    positionAlloc4->Add (Vector(87.37202682404232,96.71752490786791, 0.0));
    positionAlloc4->Add (Vector(180.1970775188126,18.07159356186432, 0.0));
    positionAlloc4->Add (Vector(49.1272354146147,64.84637259850837, 0.0));
    positionAlloc4->Add (Vector(12.235457653232151,228.52447362410916, 0.0));
    positionAlloc4->Add (Vector(345.20006802282495,98.82343991924891, 0.0));
    positionAlloc4->Add (Vector(195.8711662654113,72.50129497671645, 0.0));
    positionAlloc4->Add (Vector(259.24021012152735,176.5953730881952, 0.0));
    positionAlloc4->Add (Vector(316.09325109422014,3.946051321781896, 0.0));
    positionAlloc4->Add (Vector(126.7253814595052,366.19713798939733, 0.0));
    positionAlloc4->Add (Vector(105.79324304425354,347.96524444920664, 0.0));
    positionAlloc4->Add (Vector(274.84069979073655,384.6161221170782, 0.0));
    positionAlloc4->Add (Vector(91.4643248459349,382.12177605534373, 0.0));
    positionAlloc4->Add (Vector(61.038960250121036,132.990842163032, 0.0));
    positionAlloc4->Add (Vector(253.77364249733475,152.92127446579994, 0.0));
    positionAlloc4->Add (Vector(31.75928017389369,304.6365318580719, 0.0));
    positionAlloc4->Add (Vector(378.53147501236276,293.97626578674283, 0.0));
    positionAlloc4->Add (Vector(394.74456151761285,221.42819080661096, 0.0));
    positionAlloc4->Add (Vector(189.3329834968683,161.9359396436244, 0.0));
    positionAlloc4->Add (Vector(92.92744087416777,301.1958296667122, 0.0));
    positionAlloc4->Add (Vector(22.966211154216154,389.75354916983486, 0.0));
    positionAlloc4->Add (Vector(368.1776406047888,195.99911525110832, 0.0));
    positionAlloc4->Add (Vector(347.1043748987481,25.380551699756992, 0.0));
    positionAlloc4->Add (Vector(80.05453504620799,17.257347726487726, 0.0));
    positionAlloc4->Add (Vector(130.76695586997621,13.959791268635513, 0.0));
    positionAlloc4->Add (Vector(317.95453683755,23.53387947148473, 0.0));
    positionAlloc4->Add (Vector(383.0173761236696,295.8489818205399, 0.0));
    positionAlloc4->Add (Vector(163.68993067111083,16.348108843620324, 0.0));
    positionAlloc4->Add (Vector(26.508608117569253,61.6930249845181, 0.0));
    positionAlloc4->Add (Vector(186.49423093597645,133.8925090310172, 0.0));
    positionAlloc4->Add (Vector(289.9306588967831,192.9053346385226, 0.0));
    positionAlloc4->Add (Vector(347.6339012907582,362.4201181692485, 0.0));
    positionAlloc4->Add (Vector(245.1599153750819,226.8285382285813, 0.0));
    positionAlloc4->Add (Vector(123.60647591820131,99.98244683466781, 0.0));
    positionAlloc4->Add (Vector(33.095837712839106,18.752091198951113, 0.0));
    positionAlloc4->Add (Vector(222.74120192722756,346.4702517085246, 0.0));
    positionAlloc4->Add (Vector(358.132165025547,190.0267146969548, 0.0));
    positionAlloc4->Add (Vector(378.1571049909322,375.54164207931876, 0.0));
    positionAlloc4->Add (Vector(332.15889758710694,212.47413851517493, 0.0));
    positionAlloc4->Add (Vector(10.413678417579542,4.06465618254801, 0.0));
    positionAlloc4->Add (Vector(257.89927124040014,184.3424309404374, 0.0));
    positionAlloc4->Add (Vector(266.31582770958465,144.55702106917263, 0.0));
    positionAlloc4->Add (Vector(39.76332322571396,309.38230336235495, 0.0));
    positionAlloc4->Add (Vector(366.9699738834662,35.13224377043622, 0.0));
    positionAlloc4->Add (Vector(263.3727645926333,231.81562079877517, 0.0));
    positionAlloc4->Add (Vector(102.5247041497467,370.78866709641903, 0.0));
    positionAlloc4->Add (Vector(346.75024420850406,216.904030326499, 0.0));
    positionAlloc4->Add (Vector(221.2162591290287,306.0631722340618, 0.0));
    positionAlloc4->Add (Vector(263.6439529163227,33.00776364611888, 0.0));
    positionAlloc4->Add (Vector(70.08041823078379,150.6298476264642, 0.0));
    positionAlloc4->Add (Vector(290.47319549170754,216.4849050872143, 0.0));
    positionAlloc4->Add (Vector(113.7374789926311,54.588481892027474, 0.0));
    positionAlloc4->Add (Vector(172.01351293098998,228.29968797013956, 0.0));
    positionAlloc4->Add (Vector(322.65522408419207,123.50757604174572, 0.0));
    positionAlloc4->Add (Vector(249.5283103748833,300.3875307779455, 0.0));
    positionAlloc4->Add (Vector(290.98927908180497,66.20211747591047, 0.0));
    positionAlloc4->Add (Vector(380.2508050971845,70.2708721010743, 0.0));
    positionAlloc4->Add (Vector(127.35398170783223,181.73336949175373, 0.0));
    positionAlloc4->Add (Vector(319.0345330930855,305.84962013123106, 0.0));
    positionAlloc4->Add (Vector(329.99905542237974,65.57466957699165, 0.0));
    positionAlloc4->Add (Vector(335.9083857202608,361.26590872375283, 0.0));
    positionAlloc4->Add (Vector(140.259794010235,0.15738012424066916, 0.0));
    positionAlloc4->Add (Vector(62.55461213162068,346.5410885650799, 0.0));
    positionAlloc4->Add (Vector(391.0309143766077,116.8806997519959, 0.0));
    positionAlloc4->Add (Vector(330.14618089482207,132.7193465410906, 0.0));
    positionAlloc4->Add (Vector(286.60800460395717,260.93103953516476, 0.0));
    positionAlloc4->Add (Vector(321.46399953584034,49.757275537577605, 0.0));
    positionAlloc4->Add (Vector(295.74409842193376,237.79468714162869, 0.0));
    positionAlloc4->Add (Vector(272.4682985681178,358.4821320825256, 0.0));
    positionAlloc4->Add (Vector(259.0887254099837,229.30338180742186, 0.0));
    positionAlloc4->Add (Vector(17.01280343804421,384.044722336245, 0.0));
    positionAlloc4->Add (Vector(129.5759977688017,26.388170964588433, 0.0));
    positionAlloc4->Add (Vector(62.33600270697135,399.3556848460353, 0.0));
    positionAlloc4->Add (Vector(94.02041938730257,257.28884842830985, 0.0));
    positionAlloc4->Add (Vector(388.37205079323826,21.315930708237254, 0.0));
    positionAlloc4->Add (Vector(114.15510106872175,105.67005763106718, 0.0));
    positionAlloc4->Add (Vector(109.45343035470447,24.658193276830566, 0.0));
    positionAlloc4->Add (Vector(235.66010458232495,196.71380658365268, 0.0));
    positionAlloc4->Add (Vector(265.53137296718734,329.5230237739314, 0.0));
    positionAlloc4->Add (Vector(63.42718100347842,98.12510385911568, 0.0));
    positionAlloc4->Add (Vector(279.6762598096208,161.24640356716978, 0.0));
    positionAlloc4->Add (Vector(178.15369604863287,255.55074777476315, 0.0));
    positionAlloc4->Add (Vector(262.6070758925504,121.58348242721706, 0.0));
    positionAlloc4->Add (Vector(316.52562305663236,345.54341743706686, 0.0));
    positionAlloc4->Add (Vector(332.6744112561177,356.97035770146783, 0.0));
    positionAlloc4->Add (Vector(331.6768737906358,13.558153347847446, 0.0));
    positionAlloc4->Add (Vector(365.98485546089813,206.13770387863144, 0.0));
    positionAlloc4->Add (Vector(233.27131551405742,15.674770304396057, 0.0));
    positionAlloc4->Add (Vector(120.20761922815359,306.590182695343, 0.0));
    positionAlloc4->Add (Vector(210.2491786694438,201.6186735806003, 0.0));
    positionAlloc4->Add (Vector(267.2779157491638,93.62439864282064, 0.0));
    positionAlloc4->Add (Vector(63.77287164044856,157.5793285935086, 0.0));
    positionAlloc4->Add (Vector(337.3699859557116,388.0917425577391, 0.0));
    positionAlloc4->Add (Vector(9.207269817935249,348.7619677036301, 0.0));
    positionAlloc4->Add (Vector(197.79925337283152,23.291086871663637, 0.0));
    positionAlloc4->Add (Vector(131.14905530729075,105.50299896511692, 0.0));
    positionAlloc4->Add (Vector(39.57031913286833,347.4106423304392, 0.0));
    positionAlloc4->Add (Vector(292.0952882637421,62.44679796928514, 0.0));
    positionAlloc4->Add (Vector(347.45208726890706,331.1503691224996, 0.0));
    positionAlloc4->Add (Vector(288.33896692205235,44.89471148017454, 0.0));
    positionAlloc4->Add (Vector(289.0981700089281,309.7253954831788, 0.0));
    positionAlloc4->Add (Vector(140.41636496555267,199.80226128405693, 0.0));
    positionAlloc4->Add (Vector(213.62738606186204,82.66543496698881, 0.0));
    positionAlloc4->Add (Vector(182.08622469391696,57.82709339051411, 0.0));
    positionAlloc4->Add (Vector(203.88769886605135,325.63028229866603, 0.0));
    positionAlloc4->Add (Vector(233.2850438712557,251.98420291833727, 0.0));
    positionAlloc4->Add (Vector(148.9072673520908,27.871799853266666, 0.0));
    positionAlloc4->Add (Vector(345.3578304447152,78.5977451161786, 0.0));
    positionAlloc4->Add (Vector(187.5356943387887,265.1562123565817, 0.0));
    positionAlloc4->Add (Vector(162.86658642696153,105.63123262101738, 0.0));
    positionAlloc4->Add (Vector(252.97883914159848,256.4298313828179, 0.0));
    positionAlloc4->Add (Vector(336.7096150268846,62.242734491798004, 0.0));
    positionAlloc4->Add (Vector(381.49931220230184,202.81571533156736, 0.0));
    positionAlloc4->Add (Vector(42.17214305075743,173.4320756453177, 0.0));
    positionAlloc4->Add (Vector(165.65428662464944,149.54198837377265, 0.0));
    positionAlloc4->Add (Vector(321.35196915908114,209.60882810689031, 0.0));
    positionAlloc4->Add (Vector(399.7820615914113,239.30033744475364, 0.0));
    positionAlloc4->Add (Vector(25.2655732017546,290.5657171257448, 0.0));
    positionAlloc4->Add (Vector(152.39829734522402,215.85688069442628, 0.0));
    positionAlloc4->Add (Vector(70.28159766513254,368.94305247947943, 0.0));
    positionAlloc4->Add (Vector(143.94311509519122,386.77679525672886, 0.0));
    positionAlloc4->Add (Vector(128.1325111142906,277.865848774473, 0.0));
    positionAlloc4->Add (Vector(333.4001863546473,166.44307439383286, 0.0));
    positionAlloc4->Add (Vector(94.23941827744353,313.8121662814415, 0.0));
    positionAlloc4->Add (Vector(53.55768737062587,336.67259982194054, 0.0));
    positionAlloc4->Add (Vector(274.46371750492716,373.5307825609901, 0.0));
    positionAlloc4->Add (Vector(6.0281661374695705,166.5456631498977, 0.0));
    positionAlloc4->Add (Vector(398.3861171088289,292.82302157537697, 0.0));
    positionAlloc4->Add (Vector(262.5569911908572,251.08539781716973, 0.0));
    positionAlloc4->Add (Vector(139.63898318680398,180.75060259827822, 0.0));
    positionAlloc4->Add (Vector(357.6182235318902,209.33827116600142, 0.0));
    positionAlloc4->Add (Vector(244.43860036292938,102.39158767452578, 0.0));
    positionAlloc4->Add (Vector(306.14157126182835,171.1515114945408, 0.0));
    positionAlloc4->Add (Vector(395.927725036225,53.226218343448515, 0.0));
    positionAlloc4->Add (Vector(196.74071602734136,293.57433542834804, 0.0));
    positionAlloc4->Add (Vector(162.67022586882698,273.1961065662917, 0.0));
    positionAlloc4->Add (Vector(137.32771737335048,179.07084507269184, 0.0));
    positionAlloc4->Add (Vector(169.68561825177116,86.04639637539368, 0.0));
    positionAlloc4->Add (Vector(200.7771613869125,369.9985132552648, 0.0));
    positionAlloc4->Add (Vector(349.6680569715184,334.7683192494153, 0.0));
    positionAlloc4->Add (Vector(33.41404877935141,366.95783691124683, 0.0));
    positionAlloc4->Add (Vector(20.610082147007436,87.86946935928732, 0.0));
    positionAlloc4->Add (Vector(153.09035710841513,44.40606789537465, 0.0));
    positionAlloc4->Add (Vector(234.77106018234048,65.39988844260094, 0.0));
    positionAlloc4->Add (Vector(270.49955741889664,27.17840333789492, 0.0));
    positionAlloc4->Add (Vector(232.36669977742892,331.15619608183016, 0.0));
    positionAlloc4->Add (Vector(372.3737259926755,370.6258520857546, 0.0));
    positionAlloc4->Add (Vector(339.2879837443935,21.701316956094896, 0.0));
    positionAlloc4->Add (Vector(128.83689483885314,114.86369466372346, 0.0));
    positionAlloc4->Add (Vector(204.3356390395235,136.51655342597468, 0.0));
    positionAlloc4->Add (Vector(154.26810328181557,53.31145099693031, 0.0));
    positionAlloc4->Add (Vector(242.53120263077847,160.19082290113516, 0.0));
    positionAlloc4->Add (Vector(181.20784652231103,51.7308728253504, 0.0));
    positionAlloc4->Add (Vector(296.95232722438254,38.900572324318404, 0.0));
    positionAlloc4->Add (Vector(173.69092653510984,204.0453952072337, 0.0));
    positionAlloc4->Add (Vector(315.2820286103485,135.8197094738899, 0.0));
    positionAlloc4->Add (Vector(223.68248826315346,384.457200804357, 0.0));
    positionAlloc4->Add (Vector(165.17664802077564,33.83439553734635, 0.0));
    positionAlloc4->Add (Vector(150.53965100343555,114.97947728875317, 0.0));
    positionAlloc4->Add (Vector(304.9934935683016,193.71053830177974, 0.0));
    positionAlloc4->Add (Vector(170.91744472062317,380.38259627840705, 0.0));
    positionAlloc4->Add (Vector(9.046671657531213,45.83612793071112, 0.0));
    positionAlloc4->Add (Vector(9.985355673938079,105.15376006737998, 0.0));
    positionAlloc4->Add (Vector(351.0607451074978,8.837537700152387, 0.0));
    positionAlloc4->Add (Vector(216.2821156741676,31.776396354442802, 0.0));
    positionAlloc4->Add (Vector(393.22088042832434,95.23218290930924, 0.0));
    positionAlloc4->Add (Vector(50.1742852757888,280.3424552032974, 0.0));
    positionAlloc4->Add (Vector(190.0182452150862,295.9908444934465, 0.0));
    positionAlloc4->Add (Vector(47.94390279609253,348.96265004499054, 0.0));
    positionAlloc4->Add (Vector(27.554294064777807,143.34500396741657, 0.0));
    positionAlloc4->Add (Vector(139.08760636770015,230.50711834055733, 0.0));
    positionAlloc4->Add (Vector(175.58224621648688,380.6106708394669, 0.0));
    positionAlloc4->Add (Vector(42.40138570943222,133.6931086583585, 0.0));
    positionAlloc4->Add (Vector(104.05983686786718,28.686706015307053, 0.0));
    positionAlloc4->Add (Vector(306.0080277740985,97.8150128187385, 0.0));
    positionAlloc4->Add (Vector(356.92109448658016,325.7287626299655, 0.0));
    positionAlloc4->Add (Vector(308.94014859165486,44.73072688250279, 0.0));
    positionAlloc4->Add (Vector(124.10760487546857,245.5435557365892, 0.0));
    positionAlloc4->Add (Vector(358.84073192176044,155.06463925820628, 0.0));
    positionAlloc4->Add (Vector(371.08382575069123,6.315173151666187, 0.0));
    positionAlloc4->Add (Vector(14.848608887535564,137.18114002028295, 0.0));
    positionAlloc4->Add (Vector(167.32853129755608,185.54185373880537, 0.0));
    positionAlloc4->Add (Vector(323.78344315396566,154.81677419030834, 0.0));
    positionAlloc4->Add (Vector(367.2676294107395,389.4339191523484, 0.0));
    positionAlloc4->Add (Vector(243.5990660715177,54.17186264129788, 0.0));
    positionAlloc4->Add (Vector(310.12382079768037,93.18864530275106, 0.0));
    positionAlloc4->Add (Vector(161.57886133671533,332.5148869185989, 0.0));
    positionAlloc4->Add (Vector(212.93071708697394,32.702568627146356, 0.0));
    positionAlloc4->Add (Vector(190.94601426994885,59.42541401815902, 0.0));
    positionAlloc4->Add (Vector(358.1414391195939,78.70424732609003, 0.0));
    positionAlloc4->Add (Vector(49.1928768181654,110.50139897549212, 0.0));
    positionAlloc4->Add (Vector(157.1130026905386,397.7394890054617, 0.0));
    positionAlloc4->Add (Vector(385.63951575160206,369.163196557232, 0.0));
    positionAlloc4->Add (Vector(353.53074768670655,44.47216828182601, 0.0));
    positionAlloc4->Add (Vector(81.25825950242427,208.49460525891635, 0.0));
    positionAlloc4->Add (Vector(142.80440866663665,31.10954862345401, 0.0));
    positionAlloc4->Add (Vector(56.00366545045814,141.56395790625496, 0.0));
    positionAlloc4->Add (Vector(105.96744957615915,43.042869714373964, 0.0));
    positionAlloc4->Add (Vector(183.80061416698416,219.19349263008164, 0.0));
    positionAlloc4->Add (Vector(311.40951402813573,234.862547223291, 0.0));
    positionAlloc4->Add (Vector(318.24218264804597,1.7831704956591032, 0.0));
    positionAlloc4->Add (Vector(61.295805241631825,177.56200530348906, 0.0));
    positionAlloc4->Add (Vector(320.67341983384114,120.67181783431167, 0.0));
    positionAlloc4->Add (Vector(16.059385454202513,305.2295956403038, 0.0));
    positionAlloc4->Add (Vector(99.83555645191448,58.81362344018499, 0.0));
    positionAlloc4->Add (Vector(204.40563679773925,344.27324254358217, 0.0));
    positionAlloc4->Add (Vector(309.6965345650437,295.77679522628847, 0.0));
    positionAlloc4->Add (Vector(0.5756953667624742,41.94932976390638, 0.0));
    positionAlloc4->Add (Vector(331.07946379652066,191.4284449606964, 0.0));
    positionAlloc4->Add (Vector(5.389609473787749,300.4590166852343, 0.0));
    positionAlloc4->Add (Vector(284.4457687782262,268.66827888143325, 0.0));
    positionAlloc4->Add (Vector(299.85655536480465,21.38060511193185, 0.0));
    positionAlloc4->Add (Vector(87.00287566846274,122.43361169533236, 0.0));
    positionAlloc4->Add (Vector(83.15620127155591,387.99891103844976, 0.0));
    positionAlloc4->Add (Vector(264.15365766146374,250.05008934690838, 0.0));
    positionAlloc4->Add (Vector(252.90393880916815,232.79543247660385, 0.0));
    positionAlloc4->Add (Vector(320.43859575375865,222.09377904839877, 0.0));
    positionAlloc4->Add (Vector(138.96100853866434,101.93127848584838, 0.0));
    positionAlloc4->Add (Vector(126.37273850880311,248.52275386304265, 0.0));
    positionAlloc4->Add (Vector(119.43427416861607,290.9152413073817, 0.0));
    positionAlloc4->Add (Vector(10.561546189669846,290.7917221999091, 0.0));
    positionAlloc4->Add (Vector(267.97795559993614,34.433380631844734, 0.0));
    positionAlloc4->Add (Vector(253.8123139447311,0.01885167376460828, 0.0));
    positionAlloc4->Add (Vector(145.83708126986514,183.52229524306608, 0.0));
    positionAlloc4->Add (Vector(20.735676833324224,97.75050632420133, 0.0));
    positionAlloc4->Add (Vector(59.434008519401125,57.129518185526564, 0.0));
    positionAlloc4->Add (Vector(186.32674860142055,21.661528989425527, 0.0));
    positionAlloc4->Add (Vector(356.2860782793864,25.395412499920145, 0.0));
    positionAlloc4->Add (Vector(310.1154866732125,4.117152048545547, 0.0));
    positionAlloc4->Add (Vector(301.9619532874991,141.2322579241269, 0.0));
    positionAlloc4->Add (Vector(246.42746879204594,251.88748117827154, 0.0));
    positionAlloc4->Add (Vector(191.12930754625842,34.173752799459756, 0.0));
    positionAlloc4->Add (Vector(218.62015138842713,372.7632427393085, 0.0));
    positionAlloc4->Add (Vector(242.1634851410177,0.8434761057543483, 0.0));
    positionAlloc4->Add (Vector(389.25361690298854,134.22158904315066, 0.0));
    positionAlloc4->Add (Vector(201.89265673163283,87.52104979523216, 0.0));
    positionAlloc4->Add (Vector(305.44662349725564,174.36762497542526, 0.0));
    positionAlloc4->Add (Vector(312.4596182928795,41.43146368569681, 0.0));
    positionAlloc4->Add (Vector(89.0031270727639,205.23544395782594, 0.0));
    positionAlloc4->Add (Vector(198.53489585182675,75.40163767749041, 0.0));
    positionAlloc4->Add (Vector(70.6878789029667,349.5493338683948, 0.0));
    positionAlloc4->Add (Vector(343.7093364791395,397.89335865106915, 0.0));
    positionAlloc4->Add (Vector(105.24205278651083,278.7481687614178, 0.0));
    positionAlloc4->Add (Vector(201.50858894876552,206.06369402819075, 0.0));
    positionAlloc4->Add (Vector(374.72051626808906,364.1993250311217, 0.0));
    positionAlloc4->Add (Vector(82.84644018724548,251.2607196423627, 0.0));
    positionAlloc4->Add (Vector(329.14082846689996,241.31024340475417, 0.0));
    positionAlloc4->Add (Vector(191.0322665927581,311.5655311428713, 0.0));
    positionAlloc4->Add (Vector(256.1748171255712,183.14961410105144, 0.0));
    positionAlloc4->Add (Vector(50.38910844489229,348.0218413970408, 0.0));
    positionAlloc4->Add (Vector(147.87697706381073,140.93135093459694, 0.0));
    positionAlloc4->Add (Vector(3.0810354520713457,54.49781424812681, 0.0));
    positionAlloc4->Add (Vector(85.93139095487285,141.36291034086543, 0.0));
    positionAlloc4->Add (Vector(100.04594406196507,0.6324703409080978, 0.0));
    positionAlloc4->Add (Vector(352.48130142970433,139.75024906075456, 0.0));
    positionAlloc4->Add (Vector(345.5632599508099,399.343104592159, 0.0));
    positionAlloc4->Add (Vector(286.22712891009445,343.073792945197, 0.0));
    positionAlloc4->Add (Vector(216.95102407191052,276.6806692248966, 0.0));
    positionAlloc4->Add (Vector(255.83273149932126,78.85975721106303, 0.0));
    positionAlloc4->Add (Vector(143.17242269556755,310.178791191949, 0.0));
    positionAlloc4->Add (Vector(103.13613706180647,361.5658600821765, 0.0));
    positionAlloc4->Add (Vector(241.03856835767687,7.410231206990581, 0.0));
    positionAlloc4->Add (Vector(58.26103934290363,188.91453384493903, 0.0));
    positionAlloc4->Add (Vector(254.55077360284045,288.58839248641067, 0.0));
    positionAlloc4->Add (Vector(83.48919733762799,98.87607722670002, 0.0));
    positionAlloc4->Add (Vector(219.11492824912878,130.95302120067748, 0.0));
    positionAlloc4->Add (Vector(366.65305124556437,391.24971776128376, 0.0));
    positionAlloc4->Add (Vector(192.2316126324253,372.5774374541019, 0.0));
    positionAlloc4->Add (Vector(383.50103993703516,154.8871450915779, 0.0));
    positionAlloc4->Add (Vector(293.06879147102256,299.0361711002134, 0.0));
    positionAlloc4->Add (Vector(288.9804786170222,152.58730041706957, 0.0));
    positionAlloc4->Add (Vector(234.14384495865113,172.52865029865356, 0.0));
    positionAlloc4->Add (Vector(188.6071830459434,182.125789256188, 0.0));
    positionAlloc4->Add (Vector(102.26956684106466,256.8525909820618, 0.0));
    positionAlloc4->Add (Vector(96.00580423839888,4.713612980471105, 0.0));
    positionAlloc4->Add (Vector(348.230970967512,356.27903332808546, 0.0));
    positionAlloc4->Add (Vector(158.07114649397604,372.91848177093163, 0.0));
    positionAlloc4->Add (Vector(40.11712106988794,328.59490780335034, 0.0));
    positionAlloc4->Add (Vector(321.6162133295549,283.5504767734085, 0.0));
    positionAlloc4->Add (Vector(167.95630689356534,97.29227845767348, 0.0));
    positionAlloc4->Add (Vector(395.96289931974746,291.01578290937107, 0.0));
    positionAlloc4->Add (Vector(113.6282375249595,262.3017567188698, 0.0));
    positionAlloc4->Add (Vector(128.91117936482635,134.72844595087673, 0.0));
    positionAlloc4->Add (Vector(203.52854652102334,384.482165523054, 0.0));
    positionAlloc4->Add (Vector(111.32783013530876,269.6762102014751, 0.0));
    positionAlloc4->Add (Vector(9.607685739502392,49.05713174459918, 0.0));
    positionAlloc4->Add (Vector(210.24538893663413,260.99330850346445, 0.0));
    positionAlloc4->Add (Vector(183.25379404215374,378.58288700986805, 0.0));
    positionAlloc4->Add (Vector(226.9985428339285,208.75091277348537, 0.0));
    positionAlloc4->Add (Vector(87.77338850664216,44.32411042698652, 0.0));
    positionAlloc4->Add (Vector(146.01009760661125,81.93005679736088, 0.0));
    positionAlloc4->Add (Vector(46.59777461300334,193.9254405139418, 0.0));
    positionAlloc4->Add (Vector(211.95513548952346,318.93519246584316, 0.0));
    positionAlloc4->Add (Vector(382.4716618324921,346.5075169673949, 0.0));
    positionAlloc4->Add (Vector(185.1362724715144,329.88831465244186, 0.0));
    positionAlloc4->Add (Vector(311.240117939187,124.72466127241795, 0.0));
    positionAlloc4->Add (Vector(87.47264171412982,156.81339101692697, 0.0));
    positionAlloc4->Add (Vector(154.9214191736492,310.1034950337536, 0.0));
    positionAlloc4->Add (Vector(0.44604174159261767,279.8515818425905, 0.0));
    positionAlloc4->Add (Vector(322.4980641117811,298.0932816355076, 0.0));
    positionAlloc4->Add (Vector(358.39930745659143,203.41721148372, 0.0));
    positionAlloc4->Add (Vector(201.08056542921724,240.1384212048888, 0.0));
    positionAlloc4->Add (Vector(264.64728927969844,283.659552276697, 0.0));
    positionAlloc4->Add (Vector(50.64643831396931,197.6325126303256, 0.0));
    positionAlloc4->Add (Vector(330.5888417333467,297.98956319226636, 0.0));
    positionAlloc4->Add (Vector(372.5513185890005,183.58623918041354, 0.0));
    positionAlloc4->Add (Vector(142.386157839343,170.08752914539082, 0.0));
    positionAlloc4->Add (Vector(299.40581051898755,396.5544512216897, 0.0));
    positionAlloc4->Add (Vector(395.0168730120763,92.58875549537042, 0.0));
    positionAlloc4->Add (Vector(265.78573567274543,166.85775838901455, 0.0));
    positionAlloc4->Add (Vector(387.9868501260253,231.2954165378117, 0.0));
    positionAlloc4->Add (Vector(209.50164436116475,226.03650150834994, 0.0));
    positionAlloc4->Add (Vector(203.67131671936787,323.5944358869331, 0.0));
    positionAlloc4->Add (Vector(274.8403919544431,8.092105205228384, 0.0));
    positionAlloc4->Add (Vector(280.61528837160876,225.18014654654684, 0.0));
    positionAlloc4->Add (Vector(233.24809110391635,210.66158736755386, 0.0));
    positionAlloc4->Add (Vector(109.08484816224582,29.424320022756277, 0.0));
    positionAlloc4->Add (Vector(193.69813151644584,63.35893714943745, 0.0));
    positionAlloc4->Add (Vector(34.912831651198985,243.03908726683522, 0.0));
    positionAlloc4->Add (Vector(28.629204870414206,74.4186849921038, 0.0));
    positionAlloc4->Add (Vector(222.01291927086103,222.4186255878958, 0.0));
    positionAlloc4->Add (Vector(210.7114186412475,99.14396789647685, 0.0));
    positionAlloc4->Add (Vector(230.71915629271592,203.87524567475594, 0.0));
    positionAlloc4->Add (Vector(391.9548454557295,223.7030328931436, 0.0));
    positionAlloc4->Add (Vector(345.49109701443854,295.38009490821173, 0.0));
    positionAlloc4->Add (Vector(154.2275637557161,349.64576168244565, 0.0));
    positionAlloc4->Add (Vector(129.28312752085534,292.6899305463447, 0.0));
    positionAlloc4->Add (Vector(61.34061698990667,178.19420449501737, 0.0));
    positionAlloc4->Add (Vector(125.83593189688345,164.21962411514747, 0.0));
    positionAlloc4->Add (Vector(235.0318505903668,173.1762035518024, 0.0));
    positionAlloc4->Add (Vector(4.321969681563509,224.63969117081226, 0.0));
    positionAlloc4->Add (Vector(396.05425476717585,185.5583048597409, 0.0));
    positionAlloc4->Add (Vector(279.4318711533386,179.18946879788845, 0.0));
    positionAlloc4->Add (Vector(176.3664116679335,280.90400671849005, 0.0));
    positionAlloc4->Add (Vector(282.6518190176371,343.19628698920036, 0.0));
    positionAlloc4->Add (Vector(185.93658088707636,311.80378910793, 0.0));
    positionAlloc4->Add (Vector(120.13245468193121,217.41805542373876, 0.0));
    positionAlloc4->Add (Vector(172.35082904665546,223.59137553653136, 0.0));
    positionAlloc4->Add (Vector(123.38791982926271,13.31365024067428, 0.0));
    positionAlloc4->Add (Vector(99.08102670963221,271.6034038824, 0.0));
    positionAlloc4->Add (Vector(342.66168990703966,325.24051478477105, 0.0));
    positionAlloc4->Add (Vector(29.56842340524659,177.61125504222224, 0.0));
    positionAlloc4->Add (Vector(259.12701396479525,256.34065280737406, 0.0));
    positionAlloc4->Add (Vector(299.8753145283056,72.6801893689816, 0.0));
    positionAlloc4->Add (Vector(312.7447581943342,88.8944991409093, 0.0));
    positionAlloc4->Add (Vector(243.142047807742,264.87795284903814, 0.0));
    positionAlloc4->Add (Vector(254.14836708676717,165.78073181665118, 0.0));
    positionAlloc4->Add (Vector(344.37415885493436,145.14269497532356, 0.0));
    positionAlloc4->Add (Vector(311.2191778887946,342.13573131942366, 0.0));
    positionAlloc4->Add (Vector(378.0525295261211,11.171550154133936, 0.0));
    positionAlloc4->Add (Vector(247.03286955738005,136.4459734057951, 0.0));
    positionAlloc4->Add (Vector(143.651574954175,198.6349538503488, 0.0));
    positionAlloc4->Add (Vector(8.316988115654134,388.0228493596809, 0.0));
    positionAlloc4->Add (Vector(199.9806786784813,148.77663198159357, 0.0));
    positionAlloc4->Add (Vector(7.86940034980006,301.4908376568795, 0.0));
    positionAlloc4->Add (Vector(160.9620995357564,143.85438629604863, 0.0));
    positionAlloc4->Add (Vector(127.89548650783465,268.69303104073, 0.0));
    positionAlloc4->Add (Vector(155.65625324000254,75.48111035865657, 0.0));
    positionAlloc4->Add (Vector(378.67072550788936,120.70207629665464, 0.0));
    positionAlloc4->Add (Vector(81.52053254969273,121.76423215262781, 0.0));
    positionAlloc4->Add (Vector(121.0688217948067,147.19183228354945, 0.0));
    positionAlloc4->Add (Vector(283.8676137201537,205.30487460073198, 0.0));
    positionAlloc4->Add (Vector(292.5989551741993,208.71353850190144, 0.0));
    positionAlloc4->Add (Vector(18.104625641251104,198.5082072570601, 0.0));
    positionAlloc4->Add (Vector(100.43081532205855,204.12626538273125, 0.0));
    positionAlloc4->Add (Vector(182.79545318590218,176.18135249434062, 0.0));
    positionAlloc4->Add (Vector(299.11494492993285,341.13464909669375, 0.0));
    positionAlloc4->Add (Vector(318.34906221910967,117.83156333716711, 0.0));
    positionAlloc4->Add (Vector(171.3982151913777,265.3946254423399, 0.0));
    positionAlloc4->Add (Vector(313.4510890952773,297.9492634305413, 0.0));
    positionAlloc4->Add (Vector(197.85181091567924,46.243877231432194, 0.0));
    positionAlloc4->Add (Vector(243.46364128795796,216.4850524118871, 0.0));
    positionAlloc4->Add (Vector(115.0063744957488,390.8936610257441, 0.0));
    positionAlloc4->Add (Vector(361.9477176913294,112.6022969015295, 0.0));
    positionAlloc4->Add (Vector(27.893429678232586,325.2285958109872, 0.0));
    positionAlloc4->Add (Vector(9.758028548051811,201.4591237613538, 0.0));
    positionAlloc4->Add (Vector(376.31856120635325,158.13141583732894, 0.0));
    positionAlloc4->Add (Vector(245.61223016627417,35.58574932622638, 0.0));
    positionAlloc4->Add (Vector(294.7479992521487,113.84555880752517, 0.0));
    positionAlloc4->Add (Vector(47.358359438655874,45.669966874381004, 0.0));
    positionAlloc4->Add (Vector(211.28995288392227,117.00993227404375, 0.0));
    positionAlloc4->Add (Vector(354.20021054309103,394.5153637202155, 0.0));
    positionAlloc4->Add (Vector(154.03621014033428,364.87247516309293, 0.0));
    positionAlloc4->Add (Vector(377.52901341004844,296.74621171090956, 0.0));
    positionAlloc4->Add (Vector(137.44709636801784,148.57576206450028, 0.0));
    positionAlloc4->Add (Vector(114.33374380420926,52.30632886198299, 0.0));
    positionAlloc4->Add (Vector(316.98553124958465,98.17745279202109, 0.0));
    positionAlloc4->Add (Vector(30.825863415394217,236.40273551001098, 0.0));
    positionAlloc4->Add (Vector(293.75881488073475,253.57352744418998, 0.0));
    positionAlloc4->Add (Vector(11.239494019359553,9.500448598017641, 0.0));
    positionAlloc4->Add (Vector(68.78301396536939,284.5886583808009, 0.0));
    positionAlloc4->Add (Vector(90.94525027357703,42.42266251959599, 0.0));
    positionAlloc4->Add (Vector(347.5247680621098,265.98224492628776, 0.0));
    positionAlloc4->Add (Vector(87.1398211798895,338.69837328038454, 0.0));
    positionAlloc4->Add (Vector(306.1673115271885,207.5793967417085, 0.0));
    positionAlloc4->Add (Vector(115.98606582581827,276.83938756622814, 0.0));
    positionAlloc4->Add (Vector(392.5027272616253,180.48939193079016, 0.0));
    positionAlloc4->Add (Vector(218.4774241312838,101.62522417807006, 0.0));
    positionAlloc4->Add (Vector(126.98034352823902,235.05685476190882, 0.0));
    positionAlloc4->Add (Vector(345.8908671451265,229.14719184622237, 0.0));
    positionAlloc4->Add (Vector(114.25896557456596,63.766062942541126, 0.0));
    positionAlloc4->Add (Vector(13.594685854984645,62.24270614002809, 0.0));
    positionAlloc4->Add (Vector(135.76700693241773,179.80147782807933, 0.0));
    positionAlloc4->Add (Vector(335.0809764552152,144.87433975912668, 0.0));
    positionAlloc4->Add (Vector(274.57460974951044,398.87583954971245, 0.0));
    positionAlloc4->Add (Vector(348.93964194187106,36.95501658374533, 0.0));
    positionAlloc4->Add (Vector(106.88249732034846,271.45683236693253, 0.0));
    positionAlloc4->Add (Vector(374.7972779531918,23.61860357424206, 0.0));
    positionAlloc4->Add (Vector(382.4640993261644,11.27549728787911, 0.0));
    positionAlloc4->Add (Vector(181.74059510111897,62.42492148486769, 0.0));
    positionAlloc4->Add (Vector(41.79782288192801,3.9785308639166406, 0.0));
    positionAlloc4->Add (Vector(44.34849284697546,383.17496843052834, 0.0));
    positionAlloc4->Add (Vector(372.1681342090623,182.14661400655586, 0.0));
    positionAlloc4->Add (Vector(370.5880737013573,260.69426264843725, 0.0));
    positionAlloc4->Add (Vector(28.628786746478283,296.8186022590734, 0.0));
    positionAlloc4->Add (Vector(199.04085601786906,388.4441093667794, 0.0));
    positionAlloc4->Add (Vector(104.43807990236888,241.43090050220266, 0.0));
    positionAlloc4->Add (Vector(352.77738017920444,295.1330644528162, 0.0));
    positionAlloc4->Add (Vector(217.4525379475548,371.1637107115839, 0.0));
    positionAlloc4->Add (Vector(359.6109514964024,250.65491926104815, 0.0));
    positionAlloc4->Add (Vector(33.615381252498324,83.28945452076186, 0.0));
    positionAlloc4->Add (Vector(288.9820051692262,99.90852958946573, 0.0));
    positionAlloc4->Add (Vector(240.95482158664515,181.26299334754435, 0.0));
    positionAlloc4->Add (Vector(210.12199872566325,146.48333957992534, 0.0));
    positionAlloc4->Add (Vector(175.6532424014718,121.54929229587887, 0.0));
    positionAlloc4->Add (Vector(302.39723824234466,225.05379684686795, 0.0));
    positionAlloc4->Add (Vector(371.91901133828753,62.81358001141486, 0.0));
    positionAlloc4->Add (Vector(373.16571365149633,348.42739085648884, 0.0));
    positionAlloc4->Add (Vector(336.6255421391311,52.08446400769189, 0.0));
    positionAlloc4->Add (Vector(254.64464207836562,79.51931209612151, 0.0));
    positionAlloc4->Add (Vector(153.71175720817087,227.24198239266732, 0.0));
    positionAlloc4->Add (Vector(43.00839395215594,24.434987977251986, 0.0));
    positionAlloc4->Add (Vector(268.2333514953135,338.1347619590533, 0.0));
    positionAlloc4->Add (Vector(45.206193262292736,86.56297971682125, 0.0));
    positionAlloc4->Add (Vector(273.8390013216244,110.79791615272808, 0.0));
    positionAlloc4->Add (Vector(11.470981828807236,282.94829376720526, 0.0));
    positionAlloc4->Add (Vector(77.86796189552017,275.22002694876767, 0.0));
    positionAlloc4->Add (Vector(254.198344470775,62.142822246853946, 0.0));
    positionAlloc4->Add (Vector(277.59362062034427,175.66909615835868, 0.0));
    positionAlloc4->Add (Vector(258.0202573668741,350.124316990155, 0.0));
    positionAlloc4->Add (Vector(189.71945635947017,369.8869590535225, 0.0));
    positionAlloc4->Add (Vector(212.53890548947706,222.3380050127108, 0.0));
    positionAlloc4->Add (Vector(245.60680896983428,322.3108630306227, 0.0));
    positionAlloc4->Add (Vector(178.8549159947423,235.42745215096056, 0.0));
    positionAlloc4->Add (Vector(373.93297877654925,211.12467974424524, 0.0));
    positionAlloc4->Add (Vector(205.91388818267097,153.2928930314495, 0.0));
    positionAlloc4->Add (Vector(195.4247487463722,103.54269917754553, 0.0));
    positionAlloc4->Add (Vector(8.842897359032964,147.75299247996426, 0.0));
    positionAlloc4->Add (Vector(237.58488893262188,313.6406375817335, 0.0));
    positionAlloc4->Add (Vector(213.6457095535069,322.9403513791376, 0.0));
    positionAlloc4->Add (Vector(99.9739631388008,154.35931381080178, 0.0));
    positionAlloc4->Add (Vector(107.18804986435573,373.58916857251324, 0.0));
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_15.flowmon", false, false);
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