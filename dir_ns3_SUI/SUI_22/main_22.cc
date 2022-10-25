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
	  uint16_t numberOfRrhs = 34;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 648;
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
    positionAlloc->Add (Vector (356.6666666666667,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,270.0, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,140.0, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (270.0,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (270.0,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (270.0,270.0, 0.0));
    positionAlloc->Add (Vector (270.0,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (270.0,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (270.0,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,10.0, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,270.0, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,140.0, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,10.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,140.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,10.0, 0.0));
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
    positionAlloc4->Add (Vector(85.43287687406189,200.67236136654208, 0.0));
    positionAlloc4->Add (Vector(289.0978880486225,244.6786890864953, 0.0));
    positionAlloc4->Add (Vector(205.4320204938682,322.16577227313746, 0.0));
    positionAlloc4->Add (Vector(302.749459138263,35.846390586675355, 0.0));
    positionAlloc4->Add (Vector(258.7862395423658,287.6350814725203, 0.0));
    positionAlloc4->Add (Vector(167.16842033581827,55.38168303486244, 0.0));
    positionAlloc4->Add (Vector(58.287255375191464,178.84214469326153, 0.0));
    positionAlloc4->Add (Vector(240.56340327985927,213.5224287651805, 0.0));
    positionAlloc4->Add (Vector(49.46650858750661,65.28082600280953, 0.0));
    positionAlloc4->Add (Vector(141.25052489547508,279.0586449058134, 0.0));
    positionAlloc4->Add (Vector(244.02054050893872,344.23671474324397, 0.0));
    positionAlloc4->Add (Vector(283.1803135849369,42.16632063314996, 0.0));
    positionAlloc4->Add (Vector(16.333965273855043,141.52461957594076, 0.0));
    positionAlloc4->Add (Vector(268.6377203641585,203.74769624523162, 0.0));
    positionAlloc4->Add (Vector(59.43270860205896,118.33859139160658, 0.0));
    positionAlloc4->Add (Vector(222.53420568995352,283.98367078622385, 0.0));
    positionAlloc4->Add (Vector(252.7201717471541,45.31137379980446, 0.0));
    positionAlloc4->Add (Vector(334.20460995105566,256.377398048101, 0.0));
    positionAlloc4->Add (Vector(331.2375427985013,259.91330800456086, 0.0));
    positionAlloc4->Add (Vector(4.050485092452227,101.7528985088064, 0.0));
    positionAlloc4->Add (Vector(150.01801276045987,41.59129289906169, 0.0));
    positionAlloc4->Add (Vector(78.3252861540027,309.7505905359257, 0.0));
    positionAlloc4->Add (Vector(306.8449833434091,152.9015164466745, 0.0));
    positionAlloc4->Add (Vector(8.35218890359668,136.07270949074712, 0.0));
    positionAlloc4->Add (Vector(113.80992789554783,349.42989063388546, 0.0));
    positionAlloc4->Add (Vector(277.6217539245747,215.1501476976874, 0.0));
    positionAlloc4->Add (Vector(96.26999127662152,215.87520480945943, 0.0));
    positionAlloc4->Add (Vector(110.1663943186018,124.72390541371148, 0.0));
    positionAlloc4->Add (Vector(71.23521522699217,73.17736821363545, 0.0));
    positionAlloc4->Add (Vector(350.6878650425027,298.3522941996117, 0.0));
    positionAlloc4->Add (Vector(200.06331013896323,234.45365867337117, 0.0));
    positionAlloc4->Add (Vector(316.52622538041754,175.43246739683792, 0.0));
    positionAlloc4->Add (Vector(333.7083717643137,148.9773509757064, 0.0));
    positionAlloc4->Add (Vector(104.46085623559517,236.482296963321, 0.0));
    positionAlloc4->Add (Vector(377.91461688939904,392.86443125063187, 0.0));
    positionAlloc4->Add (Vector(110.22250778198601,158.71933331094215, 0.0));
    positionAlloc4->Add (Vector(0.6437166105928593,5.924604709727488, 0.0));
    positionAlloc4->Add (Vector(29.203999396732485,330.5063147336685, 0.0));
    positionAlloc4->Add (Vector(66.96857528238351,324.3344952620137, 0.0));
    positionAlloc4->Add (Vector(379.45653163817764,157.11251899757747, 0.0));
    positionAlloc4->Add (Vector(390.50342932499416,108.38223051868545, 0.0));
    positionAlloc4->Add (Vector(79.38487582651055,139.8570260475261, 0.0));
    positionAlloc4->Add (Vector(275.0032082675673,55.69516965738757, 0.0));
    positionAlloc4->Add (Vector(325.8409946125074,26.521697770565787, 0.0));
    positionAlloc4->Add (Vector(226.8309283732381,180.42598886680693, 0.0));
    positionAlloc4->Add (Vector(207.80322091438404,379.4547073309435, 0.0));
    positionAlloc4->Add (Vector(161.76461451528522,170.21519420099028, 0.0));
    positionAlloc4->Add (Vector(170.88285301366466,389.8040175240058, 0.0));
    positionAlloc4->Add (Vector(86.32856470549709,150.3777841133125, 0.0));
    positionAlloc4->Add (Vector(147.0709330114253,149.07563782508188, 0.0));
    positionAlloc4->Add (Vector(117.69792945356397,115.2305039149939, 0.0));
    positionAlloc4->Add (Vector(360.2414850762838,251.84665033788497, 0.0));
    positionAlloc4->Add (Vector(234.55248560893415,170.72195360601182, 0.0));
    positionAlloc4->Add (Vector(38.28865898332574,5.8650194434240355, 0.0));
    positionAlloc4->Add (Vector(334.2334141871257,397.45503897196596, 0.0));
    positionAlloc4->Add (Vector(357.7405870621117,100.67451573809825, 0.0));
    positionAlloc4->Add (Vector(74.37053717767945,355.7522208534328, 0.0));
    positionAlloc4->Add (Vector(316.0929280403659,90.5698621953677, 0.0));
    positionAlloc4->Add (Vector(93.747366532511,383.8025405582659, 0.0));
    positionAlloc4->Add (Vector(89.9713100891081,241.79213574723553, 0.0));
    positionAlloc4->Add (Vector(87.1222364944598,103.07652954403449, 0.0));
    positionAlloc4->Add (Vector(74.61538290305998,166.7801992074681, 0.0));
    positionAlloc4->Add (Vector(355.344074513657,170.46059858800353, 0.0));
    positionAlloc4->Add (Vector(53.68394293818568,249.54563327500722, 0.0));
    positionAlloc4->Add (Vector(193.03551761616887,57.12443980013222, 0.0));
    positionAlloc4->Add (Vector(257.88269104635725,46.87613348348094, 0.0));
    positionAlloc4->Add (Vector(128.47306590879057,189.52478482472924, 0.0));
    positionAlloc4->Add (Vector(262.7962288486321,163.38754731592684, 0.0));
    positionAlloc4->Add (Vector(111.58989182331153,255.01601658957586, 0.0));
    positionAlloc4->Add (Vector(49.99944018942695,148.4408231936161, 0.0));
    positionAlloc4->Add (Vector(96.71343610644749,189.52355440537292, 0.0));
    positionAlloc4->Add (Vector(296.2808986915792,127.17586476037272, 0.0));
    positionAlloc4->Add (Vector(376.5043260615509,139.63587734938295, 0.0));
    positionAlloc4->Add (Vector(107.44138031932371,8.329782345820291, 0.0));
    positionAlloc4->Add (Vector(191.64233972013918,110.53488909087888, 0.0));
    positionAlloc4->Add (Vector(234.1081592018232,215.54523942747412, 0.0));
    positionAlloc4->Add (Vector(297.23204370711755,393.82311438210974, 0.0));
    positionAlloc4->Add (Vector(257.1714984159297,47.56700349759693, 0.0));
    positionAlloc4->Add (Vector(301.1716019176167,362.2860759884703, 0.0));
    positionAlloc4->Add (Vector(364.41238757704923,384.92134613483057, 0.0));
    positionAlloc4->Add (Vector(351.7116854559692,365.29136420787125, 0.0));
    positionAlloc4->Add (Vector(96.00156834273736,72.02612969280966, 0.0));
    positionAlloc4->Add (Vector(62.24679865437004,94.28722786839474, 0.0));
    positionAlloc4->Add (Vector(182.78951708847617,392.8005403631879, 0.0));
    positionAlloc4->Add (Vector(377.2871051189582,141.78342805573374, 0.0));
    positionAlloc4->Add (Vector(94.00656126217952,276.06394671606233, 0.0));
    positionAlloc4->Add (Vector(225.15273231476084,138.0787652554776, 0.0));
    positionAlloc4->Add (Vector(310.32073460964114,81.15380200836478, 0.0));
    positionAlloc4->Add (Vector(34.74443939722529,389.4865221459265, 0.0));
    positionAlloc4->Add (Vector(149.71193435497088,163.09506690881744, 0.0));
    positionAlloc4->Add (Vector(199.72675360386506,42.366495787186324, 0.0));
    positionAlloc4->Add (Vector(16.748239285507793,273.55485655780694, 0.0));
    positionAlloc4->Add (Vector(349.8969329899448,233.2830602056886, 0.0));
    positionAlloc4->Add (Vector(386.3181286001707,350.25324818698584, 0.0));
    positionAlloc4->Add (Vector(12.975140123846662,299.06310756257204, 0.0));
    positionAlloc4->Add (Vector(249.92815104116806,57.30739617690182, 0.0));
    positionAlloc4->Add (Vector(110.03311417049937,28.393036728531218, 0.0));
    positionAlloc4->Add (Vector(17.316585324533172,42.73545541954746, 0.0));
    positionAlloc4->Add (Vector(51.34097751675601,24.459300113873628, 0.0));
    positionAlloc4->Add (Vector(389.99160992007876,154.28821604277613, 0.0));
    positionAlloc4->Add (Vector(266.58601612244166,311.6341279264816, 0.0));
    positionAlloc4->Add (Vector(193.1913584037936,182.75771714816972, 0.0));
    positionAlloc4->Add (Vector(171.3840533006215,290.76932351957026, 0.0));
    positionAlloc4->Add (Vector(50.40970861860043,95.891492917653, 0.0));
    positionAlloc4->Add (Vector(285.8028204314679,60.76719359449907, 0.0));
    positionAlloc4->Add (Vector(355.2823234789128,68.87137994563095, 0.0));
    positionAlloc4->Add (Vector(46.70153208811079,118.6021158230862, 0.0));
    positionAlloc4->Add (Vector(221.1243749755798,353.1442072123123, 0.0));
    positionAlloc4->Add (Vector(264.0355688050984,133.9179687386002, 0.0));
    positionAlloc4->Add (Vector(36.51289628006054,314.257042672928, 0.0));
    positionAlloc4->Add (Vector(17.858092558102445,52.56607732816798, 0.0));
    positionAlloc4->Add (Vector(78.230929764438,153.73557355597165, 0.0));
    positionAlloc4->Add (Vector(381.0580583640706,120.18688217194202, 0.0));
    positionAlloc4->Add (Vector(321.4234965597785,378.9884724445365, 0.0));
    positionAlloc4->Add (Vector(209.92965423974655,310.5809808662454, 0.0));
    positionAlloc4->Add (Vector(64.10627485747354,112.72261499658995, 0.0));
    positionAlloc4->Add (Vector(170.47392597774848,357.0892517059016, 0.0));
    positionAlloc4->Add (Vector(297.70329085504443,80.16068481952767, 0.0));
    positionAlloc4->Add (Vector(96.27353159793883,215.3896730353233, 0.0));
    positionAlloc4->Add (Vector(16.932355081220287,19.754783122044106, 0.0));
    positionAlloc4->Add (Vector(392.31419717157456,121.60129629661922, 0.0));
    positionAlloc4->Add (Vector(379.72712704074735,232.0361942889114, 0.0));
    positionAlloc4->Add (Vector(35.008188398910754,38.28857491408888, 0.0));
    positionAlloc4->Add (Vector(144.8477308189299,22.109922688426707, 0.0));
    positionAlloc4->Add (Vector(32.77758554913124,303.6588553006851, 0.0));
    positionAlloc4->Add (Vector(238.1283197050389,267.2647317406019, 0.0));
    positionAlloc4->Add (Vector(253.37268794683837,37.21532378587731, 0.0));
    positionAlloc4->Add (Vector(58.5828229074874,130.98312002130768, 0.0));
    positionAlloc4->Add (Vector(53.03836266499138,124.33799311997325, 0.0));
    positionAlloc4->Add (Vector(210.93600462354067,217.9015655542613, 0.0));
    positionAlloc4->Add (Vector(150.9332627165846,309.16166050725906, 0.0));
    positionAlloc4->Add (Vector(111.49114131982473,360.7401839084386, 0.0));
    positionAlloc4->Add (Vector(61.32325652123378,338.3947514428773, 0.0));
    positionAlloc4->Add (Vector(300.4087040505536,168.90282766586745, 0.0));
    positionAlloc4->Add (Vector(183.27237162558103,27.597171444806577, 0.0));
    positionAlloc4->Add (Vector(218.7391857100484,377.19085085909114, 0.0));
    positionAlloc4->Add (Vector(273.23481652463744,242.08236301003922, 0.0));
    positionAlloc4->Add (Vector(67.16994055855264,223.3035075915104, 0.0));
    positionAlloc4->Add (Vector(227.0408274427633,271.74504671628085, 0.0));
    positionAlloc4->Add (Vector(262.9118448205566,363.87243001555987, 0.0));
    positionAlloc4->Add (Vector(242.7110232587835,340.9715034038021, 0.0));
    positionAlloc4->Add (Vector(280.3880894175676,164.6083228569855, 0.0));
    positionAlloc4->Add (Vector(256.3621897609889,362.0028617462899, 0.0));
    positionAlloc4->Add (Vector(286.9846101454371,245.95122964491068, 0.0));
    positionAlloc4->Add (Vector(7.435785022150521,132.40671806304726, 0.0));
    positionAlloc4->Add (Vector(208.748390088147,285.55070328218164, 0.0));
    positionAlloc4->Add (Vector(287.34410113267245,110.02246583904616, 0.0));
    positionAlloc4->Add (Vector(96.54510528391017,267.07410269362197, 0.0));
    positionAlloc4->Add (Vector(169.75121009346674,329.69892902228355, 0.0));
    positionAlloc4->Add (Vector(53.12353049628405,138.89289900937172, 0.0));
    positionAlloc4->Add (Vector(178.51379296368228,184.8013704700403, 0.0));
    positionAlloc4->Add (Vector(109.15912669222804,52.27771086182322, 0.0));
    positionAlloc4->Add (Vector(181.68368353218202,393.8014738583293, 0.0));
    positionAlloc4->Add (Vector(2.7466872058010683,165.51037328699124, 0.0));
    positionAlloc4->Add (Vector(196.43068008281043,88.96376207502547, 0.0));
    positionAlloc4->Add (Vector(305.1681659938242,69.41982570500484, 0.0));
    positionAlloc4->Add (Vector(21.203251990994197,378.69224658447234, 0.0));
    positionAlloc4->Add (Vector(81.34058232242025,292.218917229828, 0.0));
    positionAlloc4->Add (Vector(189.92452497238466,276.5095398102484, 0.0));
    positionAlloc4->Add (Vector(210.94705151977516,66.23093938752542, 0.0));
    positionAlloc4->Add (Vector(117.48800947588958,132.69409092016963, 0.0));
    positionAlloc4->Add (Vector(234.7417370790347,43.533608321968046, 0.0));
    positionAlloc4->Add (Vector(207.82433622541384,165.52098899413124, 0.0));
    positionAlloc4->Add (Vector(13.515901967552502,340.0167537293722, 0.0));
    positionAlloc4->Add (Vector(52.21073834985566,175.9251335422902, 0.0));
    positionAlloc4->Add (Vector(127.03744204096749,272.7989947077403, 0.0));
    positionAlloc4->Add (Vector(232.30055345444308,126.27178883622312, 0.0));
    positionAlloc4->Add (Vector(304.4522399200288,332.1555982611003, 0.0));
    positionAlloc4->Add (Vector(73.16480444220974,168.12882161613615, 0.0));
    positionAlloc4->Add (Vector(147.55166798937404,80.9526367246665, 0.0));
    positionAlloc4->Add (Vector(280.50875598300655,43.51052590510367, 0.0));
    positionAlloc4->Add (Vector(177.91818619086328,13.650184392799547, 0.0));
    positionAlloc4->Add (Vector(231.16575111253422,52.976999316156935, 0.0));
    positionAlloc4->Add (Vector(222.81964262804044,362.43860375963254, 0.0));
    positionAlloc4->Add (Vector(101.84438916510739,318.8113629231163, 0.0));
    positionAlloc4->Add (Vector(78.11996544658632,230.1898267194962, 0.0));
    positionAlloc4->Add (Vector(358.66601136246305,370.8917761753065, 0.0));
    positionAlloc4->Add (Vector(68.0173514565936,144.39288329205047, 0.0));
    positionAlloc4->Add (Vector(197.1361553770599,159.99163267589563, 0.0));
    positionAlloc4->Add (Vector(333.11611709946226,399.5174500358582, 0.0));
    positionAlloc4->Add (Vector(351.5547025622749,379.01398563241156, 0.0));
    positionAlloc4->Add (Vector(322.32081452008345,216.63496298984072, 0.0));
    positionAlloc4->Add (Vector(278.61124410160073,119.85709674349198, 0.0));
    positionAlloc4->Add (Vector(181.34087917014665,18.07558526135944, 0.0));
    positionAlloc4->Add (Vector(20.938431820540693,69.97785169668012, 0.0));
    positionAlloc4->Add (Vector(290.7918401283561,200.57918395484734, 0.0));
    positionAlloc4->Add (Vector(206.14893829421607,329.97781548528565, 0.0));
    positionAlloc4->Add (Vector(187.24652393335305,183.132361660515, 0.0));
    positionAlloc4->Add (Vector(295.83360245745394,117.47441684051383, 0.0));
    positionAlloc4->Add (Vector(314.0146512276876,29.784885814299944, 0.0));
    positionAlloc4->Add (Vector(383.1893931168846,14.980718996882958, 0.0));
    positionAlloc4->Add (Vector(302.70713980243266,353.61724710643097, 0.0));
    positionAlloc4->Add (Vector(61.79852859696759,288.59990942161573, 0.0));
    positionAlloc4->Add (Vector(144.0884664271288,135.71397033926672, 0.0));
    positionAlloc4->Add (Vector(70.88945687514338,286.61180440037634, 0.0));
    positionAlloc4->Add (Vector(18.678482416360342,88.68706732543691, 0.0));
    positionAlloc4->Add (Vector(345.2848342072163,20.05051846568895, 0.0));
    positionAlloc4->Add (Vector(143.09702194733066,207.67421016568983, 0.0));
    positionAlloc4->Add (Vector(188.94609866660056,254.20833676266184, 0.0));
    positionAlloc4->Add (Vector(117.8167795615027,228.09733416853382, 0.0));
    positionAlloc4->Add (Vector(195.2092093412191,315.02254600764445, 0.0));
    positionAlloc4->Add (Vector(346.17738916740734,323.68833603262857, 0.0));
    positionAlloc4->Add (Vector(106.22247174028905,363.5302051283536, 0.0));
    positionAlloc4->Add (Vector(296.6013897937254,373.6487993178245, 0.0));
    positionAlloc4->Add (Vector(52.095749494952635,214.9624342133764, 0.0));
    positionAlloc4->Add (Vector(290.6162713673948,224.54421714967987, 0.0));
    positionAlloc4->Add (Vector(48.62614974393375,128.51297151871597, 0.0));
    positionAlloc4->Add (Vector(299.2080272271294,145.53831591545983, 0.0));
    positionAlloc4->Add (Vector(97.62594349064489,100.03962131206143, 0.0));
    positionAlloc4->Add (Vector(206.34276761871772,104.77500086618763, 0.0));
    positionAlloc4->Add (Vector(332.1659277761597,38.87029958124746, 0.0));
    positionAlloc4->Add (Vector(77.23189250485322,42.97841141249932, 0.0));
    positionAlloc4->Add (Vector(241.341499298197,103.21646891014909, 0.0));
    positionAlloc4->Add (Vector(79.274397059539,241.55683838540574, 0.0));
    positionAlloc4->Add (Vector(284.40966902327614,260.67530234658267, 0.0));
    positionAlloc4->Add (Vector(170.68323732131933,142.56977215922797, 0.0));
    positionAlloc4->Add (Vector(397.70771471330556,235.25857236243343, 0.0));
    positionAlloc4->Add (Vector(149.40539376428225,388.1070418058149, 0.0));
    positionAlloc4->Add (Vector(379.87794592685947,28.93495087612532, 0.0));
    positionAlloc4->Add (Vector(177.17668587151132,147.5693132195481, 0.0));
    positionAlloc4->Add (Vector(366.93891649895306,181.9332635677105, 0.0));
    positionAlloc4->Add (Vector(303.3984123902452,232.43927216206936, 0.0));
    positionAlloc4->Add (Vector(106.22020149564943,68.51886101532006, 0.0));
    positionAlloc4->Add (Vector(364.9957844650628,92.68193783941388, 0.0));
    positionAlloc4->Add (Vector(337.0114920099895,372.13572713611325, 0.0));
    positionAlloc4->Add (Vector(300.259016392249,204.25299444521238, 0.0));
    positionAlloc4->Add (Vector(196.17714256519622,104.45288217073569, 0.0));
    positionAlloc4->Add (Vector(283.27445840686676,295.68606897273406, 0.0));
    positionAlloc4->Add (Vector(343.2931108446063,242.50029406301135, 0.0));
    positionAlloc4->Add (Vector(95.86665365000408,394.3105152320427, 0.0));
    positionAlloc4->Add (Vector(110.99946955855108,104.47131785154933, 0.0));
    positionAlloc4->Add (Vector(240.84841455544762,330.01911759445704, 0.0));
    positionAlloc4->Add (Vector(196.95141893566336,60.31677971071643, 0.0));
    positionAlloc4->Add (Vector(113.7890753472456,323.2497029637218, 0.0));
    positionAlloc4->Add (Vector(157.48187188892064,213.32757313950012, 0.0));
    positionAlloc4->Add (Vector(75.89538210413926,0.1399623651420434, 0.0));
    positionAlloc4->Add (Vector(50.01781702684376,2.0171460089193793, 0.0));
    positionAlloc4->Add (Vector(218.255898497509,102.11958063477265, 0.0));
    positionAlloc4->Add (Vector(108.57910733969601,339.31383416039023, 0.0));
    positionAlloc4->Add (Vector(105.76423418956989,169.30103041574802, 0.0));
    positionAlloc4->Add (Vector(40.972460344134106,110.84432416284322, 0.0));
    positionAlloc4->Add (Vector(179.67538250229947,141.0004506889881, 0.0));
    positionAlloc4->Add (Vector(75.6695777813027,285.57280289752356, 0.0));
    positionAlloc4->Add (Vector(272.3757232283858,280.1284557696012, 0.0));
    positionAlloc4->Add (Vector(299.8179155423574,4.831577350043803, 0.0));
    positionAlloc4->Add (Vector(337.528304638996,56.621442931070696, 0.0));
    positionAlloc4->Add (Vector(109.15900133768899,170.1566324786659, 0.0));
    positionAlloc4->Add (Vector(145.2362074667389,50.22587022573028, 0.0));
    positionAlloc4->Add (Vector(90.45254280591068,132.52597980324293, 0.0));
    positionAlloc4->Add (Vector(27.907860816587473,288.910957684802, 0.0));
    positionAlloc4->Add (Vector(150.15410783040517,339.29885590293577, 0.0));
    positionAlloc4->Add (Vector(306.31925899814786,222.60998767324196, 0.0));
    positionAlloc4->Add (Vector(384.7129392088065,220.5746520643462, 0.0));
    positionAlloc4->Add (Vector(283.9394969690906,32.4432652921474, 0.0));
    positionAlloc4->Add (Vector(387.2245877134676,250.48988812488332, 0.0));
    positionAlloc4->Add (Vector(90.51851944260076,330.52205247984665, 0.0));
    positionAlloc4->Add (Vector(50.7315725376587,19.457371757846786, 0.0));
    positionAlloc4->Add (Vector(367.3888383528491,53.33084712347493, 0.0));
    positionAlloc4->Add (Vector(360.1604359917991,209.77687173505254, 0.0));
    positionAlloc4->Add (Vector(267.0924045622543,227.64876110471624, 0.0));
    positionAlloc4->Add (Vector(143.26718020785322,101.60861177687775, 0.0));
    positionAlloc4->Add (Vector(232.41781857092283,298.45083505868564, 0.0));
    positionAlloc4->Add (Vector(336.41703124049474,1.5903908711853987, 0.0));
    positionAlloc4->Add (Vector(259.4725282363287,93.7978402108207, 0.0));
    positionAlloc4->Add (Vector(372.1476349079829,230.77372829189264, 0.0));
    positionAlloc4->Add (Vector(278.8288113355775,20.558074042630192, 0.0));
    positionAlloc4->Add (Vector(42.45020746825139,111.62982541200424, 0.0));
    positionAlloc4->Add (Vector(51.42991988588355,296.46701654653634, 0.0));
    positionAlloc4->Add (Vector(372.65674058516646,237.29225897822707, 0.0));
    positionAlloc4->Add (Vector(178.02245191355567,194.62809124698924, 0.0));
    positionAlloc4->Add (Vector(30.429214837031758,68.98949355171466, 0.0));
    positionAlloc4->Add (Vector(280.57270077333584,90.39412163328203, 0.0));
    positionAlloc4->Add (Vector(92.90868490828075,237.53729688704118, 0.0));
    positionAlloc4->Add (Vector(76.58796595638542,353.8997892158704, 0.0));
    positionAlloc4->Add (Vector(314.8161635843175,161.70947118280628, 0.0));
    positionAlloc4->Add (Vector(265.1611162218884,254.1228467429434, 0.0));
    positionAlloc4->Add (Vector(25.162537264356466,182.07629105781166, 0.0));
    positionAlloc4->Add (Vector(191.6398354025215,225.7834267885742, 0.0));
    positionAlloc4->Add (Vector(387.30647190107834,107.33424239811869, 0.0));
    positionAlloc4->Add (Vector(216.69629029243362,113.7784853026937, 0.0));
    positionAlloc4->Add (Vector(381.97123372532377,80.54601721572752, 0.0));
    positionAlloc4->Add (Vector(366.71628336344236,230.4359559841607, 0.0));
    positionAlloc4->Add (Vector(214.50710229274398,343.63298685640973, 0.0));
    positionAlloc4->Add (Vector(316.13878423223724,121.94824780679822, 0.0));
    positionAlloc4->Add (Vector(243.64425872574475,385.5730863419908, 0.0));
    positionAlloc4->Add (Vector(323.84726335907163,399.796094901563, 0.0));
    positionAlloc4->Add (Vector(91.96023126346415,23.4879243307605, 0.0));
    positionAlloc4->Add (Vector(104.13918913315014,65.9671737993687, 0.0));
    positionAlloc4->Add (Vector(265.1268398543451,255.44451090046329, 0.0));
    positionAlloc4->Add (Vector(113.84007740451119,138.42173632844066, 0.0));
    positionAlloc4->Add (Vector(260.3320500859362,257.97705146625987, 0.0));
    positionAlloc4->Add (Vector(46.35196770356749,127.5298765844842, 0.0));
    positionAlloc4->Add (Vector(154.27413840669203,15.77378559166025, 0.0));
    positionAlloc4->Add (Vector(149.7556938107893,39.118529955504, 0.0));
    positionAlloc4->Add (Vector(37.28752620431371,175.64620229834725, 0.0));
    positionAlloc4->Add (Vector(170.43553143854604,196.16530018101167, 0.0));
    positionAlloc4->Add (Vector(22.762739643568743,131.62604333122826, 0.0));
    positionAlloc4->Add (Vector(207.6250978761232,265.005282349782, 0.0));
    positionAlloc4->Add (Vector(95.0874309949357,4.624179749140467, 0.0));
    positionAlloc4->Add (Vector(329.1548428416559,274.6428989678941, 0.0));
    positionAlloc4->Add (Vector(36.160874156609644,66.05881914336793, 0.0));
    positionAlloc4->Add (Vector(151.5649079457111,378.45143129661096, 0.0));
    positionAlloc4->Add (Vector(342.8447147127462,315.20839082284317, 0.0));
    positionAlloc4->Add (Vector(13.45285152699569,131.37272818692873, 0.0));
    positionAlloc4->Add (Vector(398.05864063664404,254.81044878979105, 0.0));
    positionAlloc4->Add (Vector(252.64669227551084,353.74986841203474, 0.0));
    positionAlloc4->Add (Vector(313.707138421458,94.54405092367261, 0.0));
    positionAlloc4->Add (Vector(37.83438271889099,113.07207022287855, 0.0));
    positionAlloc4->Add (Vector(283.2966965850457,241.229071933501, 0.0));
    positionAlloc4->Add (Vector(81.80361186606441,115.2855774836476, 0.0));
    positionAlloc4->Add (Vector(384.6839438505072,240.51669341054608, 0.0));
    positionAlloc4->Add (Vector(321.8969087672345,253.6149905864181, 0.0));
    positionAlloc4->Add (Vector(180.68807736848598,220.2433719701264, 0.0));
    positionAlloc4->Add (Vector(223.35011035256156,319.17892329595827, 0.0));
    positionAlloc4->Add (Vector(74.63341855591983,263.61449637984714, 0.0));
    positionAlloc4->Add (Vector(186.6342266385084,214.72887454569064, 0.0));
    positionAlloc4->Add (Vector(151.95582947592575,183.56647907166246, 0.0));
    positionAlloc4->Add (Vector(277.724163320028,292.21372824330695, 0.0));
    positionAlloc4->Add (Vector(362.38697301508194,322.07948061745293, 0.0));
    positionAlloc4->Add (Vector(196.22234597925333,229.42236474203042, 0.0));
    positionAlloc4->Add (Vector(272.9369844602761,103.59954004902305, 0.0));
    positionAlloc4->Add (Vector(62.77563869093888,9.737661896582628, 0.0));
    positionAlloc4->Add (Vector(176.89488371687804,349.2405226822109, 0.0));
    positionAlloc4->Add (Vector(354.09964231146495,316.71485683883907, 0.0));
    positionAlloc4->Add (Vector(144.27558398295992,168.429984516534, 0.0));
    positionAlloc4->Add (Vector(66.88662266396364,129.62520567808804, 0.0));
    positionAlloc4->Add (Vector(243.33727089725582,147.89952449356494, 0.0));
    positionAlloc4->Add (Vector(166.8198185041929,65.70241238228166, 0.0));
    positionAlloc4->Add (Vector(309.41449958293896,145.0226542306361, 0.0));
    positionAlloc4->Add (Vector(8.682900814684658,166.12277239438922, 0.0));
    positionAlloc4->Add (Vector(322.54157179236967,276.05395186472, 0.0));
    positionAlloc4->Add (Vector(202.24200895633132,58.51217546126324, 0.0));
    positionAlloc4->Add (Vector(379.64861355892384,347.04577426344565, 0.0));
    positionAlloc4->Add (Vector(56.10256159058431,349.42307004454005, 0.0));
    positionAlloc4->Add (Vector(283.94906122573514,366.770348047129, 0.0));
    positionAlloc4->Add (Vector(185.75320060982574,33.24138653408206, 0.0));
    positionAlloc4->Add (Vector(321.82595842206854,178.99928427307853, 0.0));
    positionAlloc4->Add (Vector(341.4758330979849,358.2968152379237, 0.0));
    positionAlloc4->Add (Vector(23.405648818012814,22.86430843828451, 0.0));
    positionAlloc4->Add (Vector(256.14920085154324,35.340747598656776, 0.0));
    positionAlloc4->Add (Vector(173.18093535605533,29.187697236379684, 0.0));
    positionAlloc4->Add (Vector(225.04790743735802,319.6746101179699, 0.0));
    positionAlloc4->Add (Vector(246.91837146241502,377.0013633591478, 0.0));
    positionAlloc4->Add (Vector(83.125505304838,212.31354612902416, 0.0));
    positionAlloc4->Add (Vector(172.5862890483806,83.26341694508498, 0.0));
    positionAlloc4->Add (Vector(113.89678835904947,346.5946042555179, 0.0));
    positionAlloc4->Add (Vector(119.72824082872818,284.9926986038432, 0.0));
    positionAlloc4->Add (Vector(394.5969786137699,173.96196346108357, 0.0));
    positionAlloc4->Add (Vector(280.88190285314647,102.26863906953652, 0.0));
    positionAlloc4->Add (Vector(193.60149409239372,348.73541150317243, 0.0));
    positionAlloc4->Add (Vector(118.6958095325208,355.7904274052289, 0.0));
    positionAlloc4->Add (Vector(88.78324627507226,221.6419175450685, 0.0));
    positionAlloc4->Add (Vector(386.0374357017304,104.69219557310025, 0.0));
    positionAlloc4->Add (Vector(126.86009152751171,354.1215500286387, 0.0));
    positionAlloc4->Add (Vector(193.4440575038028,174.30363490962444, 0.0));
    positionAlloc4->Add (Vector(65.97720847015825,50.28020626182972, 0.0));
    positionAlloc4->Add (Vector(124.13514508657686,378.9960255190514, 0.0));
    positionAlloc4->Add (Vector(245.43384185124012,228.01687015070385, 0.0));
    positionAlloc4->Add (Vector(29.519665532212393,348.71864033332474, 0.0));
    positionAlloc4->Add (Vector(13.434212394278289,123.88259092874976, 0.0));
    positionAlloc4->Add (Vector(79.34245205500328,292.1499896979971, 0.0));
    positionAlloc4->Add (Vector(196.15291195476036,86.21039339606536, 0.0));
    positionAlloc4->Add (Vector(334.33838321808867,169.27941772667063, 0.0));
    positionAlloc4->Add (Vector(359.90782995578735,4.895689206773479, 0.0));
    positionAlloc4->Add (Vector(216.4334987858795,203.53156341295568, 0.0));
    positionAlloc4->Add (Vector(183.73967986558335,219.0743900702509, 0.0));
    positionAlloc4->Add (Vector(230.3857993302608,215.61965074501023, 0.0));
    positionAlloc4->Add (Vector(288.61256306114416,379.76359983652037, 0.0));
    positionAlloc4->Add (Vector(133.77985897013266,229.76922845320584, 0.0));
    positionAlloc4->Add (Vector(399.9600384584867,264.1959252960194, 0.0));
    positionAlloc4->Add (Vector(288.52664557263165,323.2811601958647, 0.0));
    positionAlloc4->Add (Vector(175.1378137277751,293.21769733499764, 0.0));
    positionAlloc4->Add (Vector(213.315842881153,175.83545819242747, 0.0));
    positionAlloc4->Add (Vector(195.15848687076186,18.261767506307393, 0.0));
    positionAlloc4->Add (Vector(245.8130281209308,205.13278105281793, 0.0));
    positionAlloc4->Add (Vector(59.4604284291179,307.9550169444662, 0.0));
    positionAlloc4->Add (Vector(339.4031073570518,198.87427603550063, 0.0));
    positionAlloc4->Add (Vector(18.729558997960538,44.553006986362135, 0.0));
    positionAlloc4->Add (Vector(272.3968993768613,315.68689889975536, 0.0));
    positionAlloc4->Add (Vector(328.0763555187747,273.6435050695357, 0.0));
    positionAlloc4->Add (Vector(372.69296676440837,43.48264847310674, 0.0));
    positionAlloc4->Add (Vector(389.5994831149673,304.63084973135494, 0.0));
    positionAlloc4->Add (Vector(361.3327542891257,219.5416839428043, 0.0));
    positionAlloc4->Add (Vector(25.1372436189623,366.46371368856353, 0.0));
    positionAlloc4->Add (Vector(166.9704300239082,55.13191284591015, 0.0));
    positionAlloc4->Add (Vector(322.4949816503881,194.55134870029886, 0.0));
    positionAlloc4->Add (Vector(134.24345452282415,107.83048357927645, 0.0));
    positionAlloc4->Add (Vector(292.78798220067875,99.7992825712148, 0.0));
    positionAlloc4->Add (Vector(298.7454428171257,346.3900803877433, 0.0));
    positionAlloc4->Add (Vector(346.13004104273824,337.8369051269389, 0.0));
    positionAlloc4->Add (Vector(364.07213296750666,70.63046566875846, 0.0));
    positionAlloc4->Add (Vector(186.20775676712645,277.8882200633628, 0.0));
    positionAlloc4->Add (Vector(125.47818077279823,179.98891124585663, 0.0));
    positionAlloc4->Add (Vector(324.5263860334848,261.1826267772476, 0.0));
    positionAlloc4->Add (Vector(393.69118450886356,61.69679437475884, 0.0));
    positionAlloc4->Add (Vector(108.63149874650429,269.5074656683176, 0.0));
    positionAlloc4->Add (Vector(289.08776263764923,347.6629928304491, 0.0));
    positionAlloc4->Add (Vector(264.9176314759593,2.815127627650771, 0.0));
    positionAlloc4->Add (Vector(28.710878628563563,180.81760904396953, 0.0));
    positionAlloc4->Add (Vector(138.85733871724906,43.72426555946909, 0.0));
    positionAlloc4->Add (Vector(314.40606695895434,307.38510946764006, 0.0));
    positionAlloc4->Add (Vector(44.04589725780576,160.99573012207026, 0.0));
    positionAlloc4->Add (Vector(159.45041531411403,215.52124001460453, 0.0));
    positionAlloc4->Add (Vector(116.61486182712997,26.896399192072185, 0.0));
    positionAlloc4->Add (Vector(330.19995266497403,37.44831894496947, 0.0));
    positionAlloc4->Add (Vector(375.58815637632824,114.06993192004902, 0.0));
    positionAlloc4->Add (Vector(121.55683154810677,153.0453239033608, 0.0));
    positionAlloc4->Add (Vector(328.78996319332845,316.8338301629751, 0.0));
    positionAlloc4->Add (Vector(64.83612863957866,252.42318196010686, 0.0));
    positionAlloc4->Add (Vector(370.4913349740446,199.96212877868996, 0.0));
    positionAlloc4->Add (Vector(270.52187212079355,371.3749342544919, 0.0));
    positionAlloc4->Add (Vector(137.96441681820278,207.03901944902964, 0.0));
    positionAlloc4->Add (Vector(90.6947208104861,398.7655091185799, 0.0));
    positionAlloc4->Add (Vector(247.72698510007208,174.7326293788178, 0.0));
    positionAlloc4->Add (Vector(277.38939460724777,332.1649548539167, 0.0));
    positionAlloc4->Add (Vector(0.6363721757629115,263.22480730051774, 0.0));
    positionAlloc4->Add (Vector(181.19841124774806,15.623161915948858, 0.0));
    positionAlloc4->Add (Vector(398.12992267481377,319.8061733754905, 0.0));
    positionAlloc4->Add (Vector(323.5516360878302,275.79780784276363, 0.0));
    positionAlloc4->Add (Vector(69.30529223280027,65.52336866845279, 0.0));
    positionAlloc4->Add (Vector(387.3264195352235,28.731575462791437, 0.0));
    positionAlloc4->Add (Vector(332.3773161582421,174.50605495022674, 0.0));
    positionAlloc4->Add (Vector(45.777018121517045,208.82797845107515, 0.0));
    positionAlloc4->Add (Vector(163.36557129455915,1.9769325541069804, 0.0));
    positionAlloc4->Add (Vector(159.97850497739697,370.2874462598401, 0.0));
    positionAlloc4->Add (Vector(27.750593502148348,117.25836922269148, 0.0));
    positionAlloc4->Add (Vector(75.92100639282955,157.91444042805728, 0.0));
    positionAlloc4->Add (Vector(38.90177064339153,194.4903209106263, 0.0));
    positionAlloc4->Add (Vector(249.3135747042126,356.9697806622456, 0.0));
    positionAlloc4->Add (Vector(191.8811466534386,339.6038118354431, 0.0));
    positionAlloc4->Add (Vector(114.1624364270811,389.99017726757114, 0.0));
    positionAlloc4->Add (Vector(43.92726849614368,165.06753243588378, 0.0));
    positionAlloc4->Add (Vector(304.42490100016124,258.0153318007884, 0.0));
    positionAlloc4->Add (Vector(21.082382194141402,291.5567111755823, 0.0));
    positionAlloc4->Add (Vector(397.56135594430714,28.950814993026253, 0.0));
    positionAlloc4->Add (Vector(328.2399606887676,398.89780730812794, 0.0));
    positionAlloc4->Add (Vector(302.7108364115042,28.823068147492094, 0.0));
    positionAlloc4->Add (Vector(202.95251849858812,194.79015413858036, 0.0));
    positionAlloc4->Add (Vector(270.6030298072688,251.95590223178078, 0.0));
    positionAlloc4->Add (Vector(151.6796497551812,159.0303427028805, 0.0));
    positionAlloc4->Add (Vector(208.4748401951279,65.49263697369364, 0.0));
    positionAlloc4->Add (Vector(379.456713667335,186.15546533439797, 0.0));
    positionAlloc4->Add (Vector(106.06235837734843,346.25600754832334, 0.0));
    positionAlloc4->Add (Vector(63.003711446395585,127.65977400121051, 0.0));
    positionAlloc4->Add (Vector(357.6739712464529,106.67786455925325, 0.0));
    positionAlloc4->Add (Vector(204.76351469765913,120.78526844493274, 0.0));
    positionAlloc4->Add (Vector(92.83844260961685,178.768480140794, 0.0));
    positionAlloc4->Add (Vector(19.440190726966655,270.19179267585685, 0.0));
    positionAlloc4->Add (Vector(305.37971513880143,53.97283477882273, 0.0));
    positionAlloc4->Add (Vector(163.3898121214066,41.925549939831086, 0.0));
    positionAlloc4->Add (Vector(6.93979945512373,360.79085751514356, 0.0));
    positionAlloc4->Add (Vector(256.55914184328543,83.4485680690774, 0.0));
    positionAlloc4->Add (Vector(397.0682897413402,268.2314133884144, 0.0));
    positionAlloc4->Add (Vector(69.50030835066224,159.48206921200057, 0.0));
    positionAlloc4->Add (Vector(342.47040294689015,381.18177907984233, 0.0));
    positionAlloc4->Add (Vector(104.96268124267623,205.03044428668323, 0.0));
    positionAlloc4->Add (Vector(123.75764034106491,341.39865757483676, 0.0));
    positionAlloc4->Add (Vector(336.11144696173614,12.933237448256607, 0.0));
    positionAlloc4->Add (Vector(68.21887306149765,2.815613208437817, 0.0));
    positionAlloc4->Add (Vector(357.2412089862352,218.1512615245063, 0.0));
    positionAlloc4->Add (Vector(397.3723378293544,385.97527871282716, 0.0));
    positionAlloc4->Add (Vector(130.8976938827921,213.89888791966564, 0.0));
    positionAlloc4->Add (Vector(5.325463447946799,225.78573285826263, 0.0));
    positionAlloc4->Add (Vector(213.05462746766,239.9118958112491, 0.0));
    positionAlloc4->Add (Vector(106.88217834931018,269.19554351321545, 0.0));
    positionAlloc4->Add (Vector(372.0895803209776,125.38126450063474, 0.0));
    positionAlloc4->Add (Vector(135.8670963801888,270.17659345221995, 0.0));
    positionAlloc4->Add (Vector(97.22103026235769,388.04812585415834, 0.0));
    positionAlloc4->Add (Vector(37.63601302624502,184.2166719331759, 0.0));
    positionAlloc4->Add (Vector(231.33226691000695,237.81614117977207, 0.0));
    positionAlloc4->Add (Vector(275.0888002757486,315.8036662025573, 0.0));
    positionAlloc4->Add (Vector(146.70984926433258,152.00549145454056, 0.0));
    positionAlloc4->Add (Vector(211.83013445025526,179.9334030189207, 0.0));
    positionAlloc4->Add (Vector(295.8780353006508,284.5991953612924, 0.0));
    positionAlloc4->Add (Vector(73.62827782629479,339.99972741116966, 0.0));
    positionAlloc4->Add (Vector(108.12479536057413,185.20004254769114, 0.0));
    positionAlloc4->Add (Vector(133.01726616034077,396.30304107966555, 0.0));
    positionAlloc4->Add (Vector(326.8353998099374,201.6223634861619, 0.0));
    positionAlloc4->Add (Vector(224.00951655140568,231.52720365125873, 0.0));
    positionAlloc4->Add (Vector(314.17897645667904,127.822824946454, 0.0));
    positionAlloc4->Add (Vector(393.16794742360963,60.87548882218181, 0.0));
    positionAlloc4->Add (Vector(322.89455429082193,327.2583760294758, 0.0));
    positionAlloc4->Add (Vector(82.80950660562345,203.20483454513786, 0.0));
    positionAlloc4->Add (Vector(253.5368810094797,63.58027273593518, 0.0));
    positionAlloc4->Add (Vector(64.06337444732787,17.084051359443553, 0.0));
    positionAlloc4->Add (Vector(377.22240176638434,145.6967233769578, 0.0));
    positionAlloc4->Add (Vector(23.703074837037263,80.06768932135384, 0.0));
    positionAlloc4->Add (Vector(39.07107279262685,62.90035652866695, 0.0));
    positionAlloc4->Add (Vector(102.77557057410607,390.92374073598955, 0.0));
    positionAlloc4->Add (Vector(261.8168341438966,106.23964624406415, 0.0));
    positionAlloc4->Add (Vector(54.01284758269118,366.83195759086186, 0.0));
    positionAlloc4->Add (Vector(80.93153130830113,31.106913885442733, 0.0));
    positionAlloc4->Add (Vector(305.740406182041,378.13316540825485, 0.0));
    positionAlloc4->Add (Vector(211.8719820790635,127.1802280569665, 0.0));
    positionAlloc4->Add (Vector(1.1349190964652145,74.09891673370707, 0.0));
    positionAlloc4->Add (Vector(166.31662882433548,43.460121312755405, 0.0));
    positionAlloc4->Add (Vector(365.5300102890072,43.54794939659903, 0.0));
    positionAlloc4->Add (Vector(121.99625154477319,284.70322076077093, 0.0));
    positionAlloc4->Add (Vector(50.47380488593842,52.68837744239514, 0.0));
    positionAlloc4->Add (Vector(96.79260959700073,47.33113773849462, 0.0));
    positionAlloc4->Add (Vector(370.17615767709674,101.43121933584678, 0.0));
    positionAlloc4->Add (Vector(306.1758650706306,171.44866063679794, 0.0));
    positionAlloc4->Add (Vector(322.3757703819598,237.49952910954448, 0.0));
    positionAlloc4->Add (Vector(75.75350700582719,282.52981158865856, 0.0));
    positionAlloc4->Add (Vector(40.23671170059426,292.0841787623821, 0.0));
    positionAlloc4->Add (Vector(7.586765952153307,216.1341943388411, 0.0));
    positionAlloc4->Add (Vector(281.18512161620737,296.0834954411058, 0.0));
    positionAlloc4->Add (Vector(69.5592805629576,292.69767090080035, 0.0));
    positionAlloc4->Add (Vector(75.34322136668786,353.38540413398596, 0.0));
    positionAlloc4->Add (Vector(201.62080673851483,190.55487457902385, 0.0));
    positionAlloc4->Add (Vector(357.8044921259517,393.0053142832942, 0.0));
    positionAlloc4->Add (Vector(46.831516715105124,317.7364765994863, 0.0));
    positionAlloc4->Add (Vector(209.3344001624647,297.6302075012744, 0.0));
    positionAlloc4->Add (Vector(13.734675941562857,211.890698043998, 0.0));
    positionAlloc4->Add (Vector(79.68276229309458,309.6590822759646, 0.0));
    positionAlloc4->Add (Vector(157.68909069228806,378.5313433943265, 0.0));
    positionAlloc4->Add (Vector(337.9686764422458,100.56710273830447, 0.0));
    positionAlloc4->Add (Vector(257.0333447757319,277.9549609869476, 0.0));
    positionAlloc4->Add (Vector(138.74837859194952,7.795500590695159, 0.0));
    positionAlloc4->Add (Vector(81.27654716156778,163.76150596029822, 0.0));
    positionAlloc4->Add (Vector(90.1704327521827,294.13936021326725, 0.0));
    positionAlloc4->Add (Vector(104.81105406922823,206.83979528104942, 0.0));
    positionAlloc4->Add (Vector(275.56853680672987,302.4220303272751, 0.0));
    positionAlloc4->Add (Vector(196.41940660627225,294.19370549576627, 0.0));
    positionAlloc4->Add (Vector(135.4824623884951,389.89451553232016, 0.0));
    positionAlloc4->Add (Vector(11.412875192034377,235.53837936685275, 0.0));
    positionAlloc4->Add (Vector(329.7653555321888,316.3588122788787, 0.0));
    positionAlloc4->Add (Vector(263.56024068764106,326.90402144585977, 0.0));
    positionAlloc4->Add (Vector(208.41763360460942,77.88886453180885, 0.0));
    positionAlloc4->Add (Vector(337.115807787728,76.68948318243203, 0.0));
    positionAlloc4->Add (Vector(363.1902594787986,83.10172989172666, 0.0));
    positionAlloc4->Add (Vector(45.895820803137035,93.32630839123777, 0.0));
    positionAlloc4->Add (Vector(384.39725516462875,207.04205172567632, 0.0));
    positionAlloc4->Add (Vector(301.55567160471435,239.74965458583677, 0.0));
    positionAlloc4->Add (Vector(307.4777915579476,267.7303476654677, 0.0));
    positionAlloc4->Add (Vector(146.0902208598878,331.5038610853119, 0.0));
    positionAlloc4->Add (Vector(356.8233968441807,276.50259640007283, 0.0));
    positionAlloc4->Add (Vector(358.248573076637,82.39075917145237, 0.0));
    positionAlloc4->Add (Vector(236.5642304584023,186.89434161395354, 0.0));
    positionAlloc4->Add (Vector(35.038168101705836,16.774559657913144, 0.0));
    positionAlloc4->Add (Vector(160.49378282204344,348.3367346729228, 0.0));
    positionAlloc4->Add (Vector(0.38717749509862287,244.67931163550423, 0.0));
    positionAlloc4->Add (Vector(169.12737039403117,315.0089018672798, 0.0));
    positionAlloc4->Add (Vector(320.66767833416645,90.38783670710289, 0.0));
    positionAlloc4->Add (Vector(318.0185061155741,369.86234970622587, 0.0));
    positionAlloc4->Add (Vector(325.01240783010644,347.64542505914045, 0.0));
    positionAlloc4->Add (Vector(53.60716008519897,267.80181385276427, 0.0));
    positionAlloc4->Add (Vector(376.4231701551688,172.92494304025786, 0.0));
    positionAlloc4->Add (Vector(290.8771250825319,94.35926159465211, 0.0));
    positionAlloc4->Add (Vector(267.7156894907367,357.4106146522797, 0.0));
    positionAlloc4->Add (Vector(263.1929669530528,383.17338564536135, 0.0));
    positionAlloc4->Add (Vector(208.7454412243134,158.3080664952837, 0.0));
    positionAlloc4->Add (Vector(281.8246970849699,193.17175377623136, 0.0));
    positionAlloc4->Add (Vector(229.39204328586084,259.5564187614064, 0.0));
    positionAlloc4->Add (Vector(148.88363518749793,104.53202198415372, 0.0));
    positionAlloc4->Add (Vector(68.55048349411467,220.16671839868644, 0.0));
    positionAlloc4->Add (Vector(361.28576146487654,232.57170513971164, 0.0));
    positionAlloc4->Add (Vector(294.16479710880475,326.9100488576564, 0.0));
    positionAlloc4->Add (Vector(135.61722334220576,188.54337213022342, 0.0));
    positionAlloc4->Add (Vector(36.901163964153305,111.47874897909138, 0.0));
    positionAlloc4->Add (Vector(345.1023599049135,195.9354630654973, 0.0));
    positionAlloc4->Add (Vector(300.77951638033414,299.45412367015126, 0.0));
    positionAlloc4->Add (Vector(269.4053863899451,361.1330954639091, 0.0));
    positionAlloc4->Add (Vector(216.20374958151868,326.2630240381657, 0.0));
    positionAlloc4->Add (Vector(393.5552268761212,375.4791075998536, 0.0));
    positionAlloc4->Add (Vector(150.17380428533093,299.22580840357784, 0.0));
    positionAlloc4->Add (Vector(91.37431092050959,59.64611042707406, 0.0));
    positionAlloc4->Add (Vector(312.4450448731218,71.04380457820874, 0.0));
    positionAlloc4->Add (Vector(53.51909422388057,31.042901627047748, 0.0));
    positionAlloc4->Add (Vector(94.5238257176718,345.7735381939292, 0.0));
    positionAlloc4->Add (Vector(312.6429702177582,301.12071479711085, 0.0));
    positionAlloc4->Add (Vector(271.1997293190916,348.9643098852598, 0.0));
    positionAlloc4->Add (Vector(253.97652882891103,130.105827296315, 0.0));
    positionAlloc4->Add (Vector(149.5527567696288,11.427869094951992, 0.0));
    positionAlloc4->Add (Vector(181.21810960789193,71.70334960156097, 0.0));
    positionAlloc4->Add (Vector(361.97456850055863,365.2105984945769, 0.0));
    positionAlloc4->Add (Vector(369.9403589955138,2.2283714565115798, 0.0));
    positionAlloc4->Add (Vector(308.87593874387187,205.373342133223, 0.0));
    positionAlloc4->Add (Vector(385.2837896379893,68.68626615309314, 0.0));
    positionAlloc4->Add (Vector(295.88117738060754,96.75384042319779, 0.0));
    positionAlloc4->Add (Vector(175.6332943753947,132.96497108766013, 0.0));
    positionAlloc4->Add (Vector(238.3878888430572,74.28323128736389, 0.0));
    positionAlloc4->Add (Vector(236.98292479561124,64.10666154698438, 0.0));
    positionAlloc4->Add (Vector(348.06033261559503,203.88144676937125, 0.0));
    positionAlloc4->Add (Vector(369.1091303956463,13.741694167323049, 0.0));
    positionAlloc4->Add (Vector(236.12443713150358,249.05941052035794, 0.0));
    positionAlloc4->Add (Vector(149.66720130380122,347.44006388034825, 0.0));
    positionAlloc4->Add (Vector(257.3362976016851,210.70138232132766, 0.0));
    positionAlloc4->Add (Vector(259.0052113247182,196.88461606264647, 0.0));
    positionAlloc4->Add (Vector(252.89676192141584,314.48558193837215, 0.0));
    positionAlloc4->Add (Vector(323.6987519648781,97.66506126803512, 0.0));
    positionAlloc4->Add (Vector(210.51482585609372,259.214446125893, 0.0));
    positionAlloc4->Add (Vector(296.0867570176042,211.37062750291994, 0.0));
    positionAlloc4->Add (Vector(75.16172408724398,105.32381007553448, 0.0));
    positionAlloc4->Add (Vector(49.38536145290202,343.70757836124295, 0.0));
    positionAlloc4->Add (Vector(176.18017704284736,241.0385098566834, 0.0));
    positionAlloc4->Add (Vector(70.91360389310223,33.73871022972619, 0.0));
    positionAlloc4->Add (Vector(295.9326567059028,172.9208435124181, 0.0));
    positionAlloc4->Add (Vector(240.1766751660383,58.557673581146034, 0.0));
    positionAlloc4->Add (Vector(184.9755904402083,40.372822148353336, 0.0));
    positionAlloc4->Add (Vector(23.779914664301003,307.23894583370264, 0.0));
    positionAlloc4->Add (Vector(248.0384553072137,70.82366708595286, 0.0));
    positionAlloc4->Add (Vector(47.68542218034661,149.56487754205722, 0.0));
    positionAlloc4->Add (Vector(315.2609149017505,13.042132780118676, 0.0));
    positionAlloc4->Add (Vector(335.2519490477719,19.1449766090237, 0.0));
    positionAlloc4->Add (Vector(41.80572798375293,266.6220143841442, 0.0));
    positionAlloc4->Add (Vector(73.7477611045577,282.4710032912134, 0.0));
    positionAlloc4->Add (Vector(190.19494679345405,320.87371679092934, 0.0));
    positionAlloc4->Add (Vector(185.75812613166187,378.23017397216773, 0.0));
    positionAlloc4->Add (Vector(28.34977052758063,36.0302373225855, 0.0));
    positionAlloc4->Add (Vector(338.1824522404373,143.96511309774021, 0.0));
    positionAlloc4->Add (Vector(117.25392665777497,330.2366359153862, 0.0));
    positionAlloc4->Add (Vector(84.7368559921052,382.36927076419767, 0.0));
    positionAlloc4->Add (Vector(7.955361804977246,30.261260855551477, 0.0));
    positionAlloc4->Add (Vector(215.1979138126702,183.09896192436833, 0.0));
    positionAlloc4->Add (Vector(339.2870044507176,157.71208589598763, 0.0));
    positionAlloc4->Add (Vector(308.89287356264146,76.88098872624707, 0.0));
    positionAlloc4->Add (Vector(282.49950509041446,248.89598270799218, 0.0));
    positionAlloc4->Add (Vector(303.0235887430894,268.9106454686474, 0.0));
    positionAlloc4->Add (Vector(349.3003676548015,57.704322725707954, 0.0));
    positionAlloc4->Add (Vector(239.04165481646328,13.54190399791082, 0.0));
    positionAlloc4->Add (Vector(306.7375397416085,253.7314367777793, 0.0));
    positionAlloc4->Add (Vector(87.14201069725559,154.13053093414555, 0.0));
    positionAlloc4->Add (Vector(227.49987375369133,113.59074898447395, 0.0));
    positionAlloc4->Add (Vector(205.1964799791231,187.17120516452894, 0.0));
    positionAlloc4->Add (Vector(292.7571685359474,141.31025722365825, 0.0));
    positionAlloc4->Add (Vector(115.22611857327743,343.2667393318739, 0.0));
    positionAlloc4->Add (Vector(215.3467777153042,60.60563905374528, 0.0));
    positionAlloc4->Add (Vector(262.33328442393827,11.30726272461171, 0.0));
    positionAlloc4->Add (Vector(269.4980307521944,329.2616090405457, 0.0));
    positionAlloc4->Add (Vector(44.436780825974424,214.9353826080075, 0.0));
    positionAlloc4->Add (Vector(25.868998798848075,216.32933312342723, 0.0));
    positionAlloc4->Add (Vector(392.31953158122906,190.4017879778955, 0.0));
    positionAlloc4->Add (Vector(321.56049446037775,120.59808838665474, 0.0));
    positionAlloc4->Add (Vector(196.11735385369707,48.692528344485275, 0.0));
    positionAlloc4->Add (Vector(283.6016108277896,248.64967242914412, 0.0));
    positionAlloc4->Add (Vector(105.65668219588207,14.669107242495993, 0.0));
    positionAlloc4->Add (Vector(102.31045241182896,275.9619241102885, 0.0));
    positionAlloc4->Add (Vector(158.309765929814,356.2931098777231, 0.0));
    positionAlloc4->Add (Vector(180.25811570804683,258.99979358761914, 0.0));
    positionAlloc4->Add (Vector(40.58297549042473,100.81494565454663, 0.0));
    positionAlloc4->Add (Vector(218.66127841655722,289.8818365904078, 0.0));
    positionAlloc4->Add (Vector(285.06102624748684,160.54248106028268, 0.0));
    positionAlloc4->Add (Vector(10.274231835885095,355.7754839183139, 0.0));
    positionAlloc4->Add (Vector(326.00500914476436,179.22346246876035, 0.0));
    positionAlloc4->Add (Vector(244.36887535896457,291.9685577400956, 0.0));
    positionAlloc4->Add (Vector(70.17229260061914,189.27597336075235, 0.0));
    positionAlloc4->Add (Vector(237.27375313685104,13.36890035969276, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_22.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_22.flowmon", false, false);
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