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
	  uint16_t numberOfNodes = 312;
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
    positionAlloc->Add (Vector (444.44444444444446,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,500.0, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,55.55555555555556, 0.0));
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
    positionAlloc4->Add (Vector(353.5275893374069,166.5042384579957, 0.0));
    positionAlloc4->Add (Vector(288.0744252683725,106.4571188253115, 0.0));
    positionAlloc4->Add (Vector(70.9151970189113,401.1943811938244, 0.0));
    positionAlloc4->Add (Vector(36.811658558241646,267.14751356133263, 0.0));
    positionAlloc4->Add (Vector(331.5823034343791,247.10836296511894, 0.0));
    positionAlloc4->Add (Vector(369.5296243056274,486.6597981519247, 0.0));
    positionAlloc4->Add (Vector(0.4889040794771593,354.0565197793057, 0.0));
    positionAlloc4->Add (Vector(41.90061346018942,256.5003070148943, 0.0));
    positionAlloc4->Add (Vector(404.5664118471506,126.20386778060283, 0.0));
    positionAlloc4->Add (Vector(380.2531167399709,115.80764203683874, 0.0));
    positionAlloc4->Add (Vector(50.28753723632351,224.05479518670705, 0.0));
    positionAlloc4->Add (Vector(356.9109842898595,69.54323027473957, 0.0));
    positionAlloc4->Add (Vector(450.5349676756326,214.28856599217872, 0.0));
    positionAlloc4->Add (Vector(200.2622650240579,353.33938711140195, 0.0));
    positionAlloc4->Add (Vector(65.31710205727148,144.7701730182193, 0.0));
    positionAlloc4->Add (Vector(119.56057897004125,262.73478636616255, 0.0));
    positionAlloc4->Add (Vector(97.49621392074332,324.95506462064014, 0.0));
    positionAlloc4->Add (Vector(72.75597637660336,468.3024088439277, 0.0));
    positionAlloc4->Add (Vector(157.57514430992185,59.81724786276194, 0.0));
    positionAlloc4->Add (Vector(112.80266589597909,270.25803350942596, 0.0));
    positionAlloc4->Add (Vector(216.40488777631762,427.81156573959606, 0.0));
    positionAlloc4->Add (Vector(457.3169906790788,409.7926778408892, 0.0));
    positionAlloc4->Add (Vector(269.0239034217355,419.9880590391424, 0.0));
    positionAlloc4->Add (Vector(444.6599217818898,288.5716685682813, 0.0));
    positionAlloc4->Add (Vector(114.39322697394306,497.64514565752114, 0.0));
    positionAlloc4->Add (Vector(386.0612293155915,258.1744343799141, 0.0));
    positionAlloc4->Add (Vector(101.59264760458075,266.33216556398486, 0.0));
    positionAlloc4->Add (Vector(395.24456768167687,474.5058828281979, 0.0));
    positionAlloc4->Add (Vector(159.28664668989944,390.3231355505064, 0.0));
    positionAlloc4->Add (Vector(364.45275453750116,202.8600736452485, 0.0));
    positionAlloc4->Add (Vector(349.55382851146277,85.4289473119218, 0.0));
    positionAlloc4->Add (Vector(337.43608158581605,469.23870636371083, 0.0));
    positionAlloc4->Add (Vector(344.1883618174393,464.16918968188236, 0.0));
    positionAlloc4->Add (Vector(459.3376451921892,184.0678867585736, 0.0));
    positionAlloc4->Add (Vector(326.1454909733871,60.11250195545925, 0.0));
    positionAlloc4->Add (Vector(158.40132698136873,306.475760362776, 0.0));
    positionAlloc4->Add (Vector(397.6540315195218,399.5454802009167, 0.0));
    positionAlloc4->Add (Vector(103.25597490028116,400.0764397842939, 0.0));
    positionAlloc4->Add (Vector(444.8762708679759,365.2596933366577, 0.0));
    positionAlloc4->Add (Vector(176.6442282971948,272.6233721976561, 0.0));
    positionAlloc4->Add (Vector(208.46379218619887,259.8674597521555, 0.0));
    positionAlloc4->Add (Vector(487.7040569048564,192.75791060049673, 0.0));
    positionAlloc4->Add (Vector(401.36248100398706,349.0739889612576, 0.0));
    positionAlloc4->Add (Vector(201.52140773478206,100.76534638214008, 0.0));
    positionAlloc4->Add (Vector(35.56356803879906,233.69099036275753, 0.0));
    positionAlloc4->Add (Vector(220.26822345317558,200.20512858275836, 0.0));
    positionAlloc4->Add (Vector(416.38600243274465,292.7786713452788, 0.0));
    positionAlloc4->Add (Vector(300.1245105593455,421.7592162421865, 0.0));
    positionAlloc4->Add (Vector(335.0989837542011,220.0867871699287, 0.0));
    positionAlloc4->Add (Vector(167.96281703490035,141.25789630117873, 0.0));
    positionAlloc4->Add (Vector(224.24705650168426,99.70500602344151, 0.0));
    positionAlloc4->Add (Vector(205.26095301161322,9.973532654332761, 0.0));
    positionAlloc4->Add (Vector(459.1392611112547,211.93483644774614, 0.0));
    positionAlloc4->Add (Vector(219.82639080670097,475.1361767507482, 0.0));
    positionAlloc4->Add (Vector(268.8503061723958,489.94639853078843, 0.0));
    positionAlloc4->Add (Vector(291.42915716124804,395.8667812419139, 0.0));
    positionAlloc4->Add (Vector(67.88455902893486,350.22171240122583, 0.0));
    positionAlloc4->Add (Vector(244.53328258956452,480.13374629692316, 0.0));
    positionAlloc4->Add (Vector(363.0760062746129,487.40499979583404, 0.0));
    positionAlloc4->Add (Vector(202.6734400015569,56.17949904284342, 0.0));
    positionAlloc4->Add (Vector(15.248182553431244,66.54109107776807, 0.0));
    positionAlloc4->Add (Vector(214.9025258849349,429.6755043656846, 0.0));
    positionAlloc4->Add (Vector(443.37300618650556,97.25564256145597, 0.0));
    positionAlloc4->Add (Vector(261.450938998814,359.82856548113693, 0.0));
    positionAlloc4->Add (Vector(447.41960084000556,369.8547775316437, 0.0));
    positionAlloc4->Add (Vector(433.043759793491,266.200640875643, 0.0));
    positionAlloc4->Add (Vector(332.6186816971089,202.03049097928388, 0.0));
    positionAlloc4->Add (Vector(374.72625769844325,279.14100489775535, 0.0));
    positionAlloc4->Add (Vector(53.008306497203115,344.73011071012854, 0.0));
    positionAlloc4->Add (Vector(338.8100360420707,144.15673248656387, 0.0));
    positionAlloc4->Add (Vector(15.330357571043173,380.39025087932606, 0.0));
    positionAlloc4->Add (Vector(327.3421784781657,84.50504809892561, 0.0));
    positionAlloc4->Add (Vector(275.5715513629829,452.71316877024344, 0.0));
    positionAlloc4->Add (Vector(390.84934488999085,482.88062949377456, 0.0));
    positionAlloc4->Add (Vector(356.7004180098325,440.31084799626007, 0.0));
    positionAlloc4->Add (Vector(122.94212895482632,274.2025138601103, 0.0));
    positionAlloc4->Add (Vector(165.1607080477539,474.4840710716857, 0.0));
    positionAlloc4->Add (Vector(281.8385442964208,469.90090508474344, 0.0));
    positionAlloc4->Add (Vector(213.77794496465137,212.0162072502803, 0.0));
    positionAlloc4->Add (Vector(29.537010872967397,226.53600600358203, 0.0));
    positionAlloc4->Add (Vector(350.76687116755045,420.4991673785436, 0.0));
    positionAlloc4->Add (Vector(202.33464301368508,247.94914521918483, 0.0));
    positionAlloc4->Add (Vector(116.55264909206502,179.46800201882346, 0.0));
    positionAlloc4->Add (Vector(399.74386835514173,155.61762463684374, 0.0));
    positionAlloc4->Add (Vector(221.5506193940997,493.49232444908023, 0.0));
    positionAlloc4->Add (Vector(370.52945941407387,458.54243193853927, 0.0));
    positionAlloc4->Add (Vector(211.6000291210281,149.6763187981166, 0.0));
    positionAlloc4->Add (Vector(379.66491176895477,213.4850605773327, 0.0));
    positionAlloc4->Add (Vector(366.713005527327,172.8371688453816, 0.0));
    positionAlloc4->Add (Vector(3.2464862912650605,295.25768321047536, 0.0));
    positionAlloc4->Add (Vector(180.04634401514008,302.33701019099396, 0.0));
    positionAlloc4->Add (Vector(77.28777917493196,371.08626562773907, 0.0));
    positionAlloc4->Add (Vector(323.3068395800004,282.06966652011937, 0.0));
    positionAlloc4->Add (Vector(140.6246456912989,99.46371768884855, 0.0));
    positionAlloc4->Add (Vector(390.97429751717533,429.35866810379696, 0.0));
    positionAlloc4->Add (Vector(490.92703400437557,168.47446465858445, 0.0));
    positionAlloc4->Add (Vector(248.3477579712775,439.4475220838345, 0.0));
    positionAlloc4->Add (Vector(51.98429162066659,174.9717813555126, 0.0));
    positionAlloc4->Add (Vector(444.08557401637046,426.96270463759527, 0.0));
    positionAlloc4->Add (Vector(492.4063796431127,234.2268782340605, 0.0));
    positionAlloc4->Add (Vector(139.6158316447168,101.38646270719215, 0.0));
    positionAlloc4->Add (Vector(447.12368579373884,32.86802068396533, 0.0));
    positionAlloc4->Add (Vector(91.2282621717705,241.39373907884325, 0.0));
    positionAlloc4->Add (Vector(209.66776815352262,470.5812785630788, 0.0));
    positionAlloc4->Add (Vector(181.25927716190583,404.31002192158434, 0.0));
    positionAlloc4->Add (Vector(117.5681726167841,252.65292775734093, 0.0));
    positionAlloc4->Add (Vector(363.67602251714214,67.9806816649069, 0.0));
    positionAlloc4->Add (Vector(212.63173966287223,441.8187089601938, 0.0));
    positionAlloc4->Add (Vector(267.80946485747256,267.89279913076723, 0.0));
    positionAlloc4->Add (Vector(38.36723112295215,140.8952731719344, 0.0));
    positionAlloc4->Add (Vector(446.81429665652286,3.154520519664139, 0.0));
    positionAlloc4->Add (Vector(238.08052985204725,365.2000677059007, 0.0));
    positionAlloc4->Add (Vector(20.021729954048272,165.87535874239296, 0.0));
    positionAlloc4->Add (Vector(343.9730065037449,170.0467589943772, 0.0));
    positionAlloc4->Add (Vector(345.2396887886454,461.3532620480438, 0.0));
    positionAlloc4->Add (Vector(101.92878579766985,174.8029076319867, 0.0));
    positionAlloc4->Add (Vector(171.84966396920964,452.6224401246487, 0.0));
    positionAlloc4->Add (Vector(53.02875044542016,438.6410617611545, 0.0));
    positionAlloc4->Add (Vector(145.34145135351218,220.07327972913683, 0.0));
    positionAlloc4->Add (Vector(407.16874596107175,474.1491193908965, 0.0));
    positionAlloc4->Add (Vector(460.80890191651605,405.3858414590046, 0.0));
    positionAlloc4->Add (Vector(168.30000332271695,383.2972022320284, 0.0));
    positionAlloc4->Add (Vector(380.8492481538444,479.80182386749357, 0.0));
    positionAlloc4->Add (Vector(214.10969093876804,406.72462745962247, 0.0));
    positionAlloc4->Add (Vector(238.8590226229152,407.1096466853404, 0.0));
    positionAlloc4->Add (Vector(366.99298075874566,478.4578167681183, 0.0));
    positionAlloc4->Add (Vector(459.34316996917204,94.79927674768007, 0.0));
    positionAlloc4->Add (Vector(194.17635635884457,88.18737249247465, 0.0));
    positionAlloc4->Add (Vector(14.912050511885933,105.26096889548514, 0.0));
    positionAlloc4->Add (Vector(445.61030092064584,445.64574398960804, 0.0));
    positionAlloc4->Add (Vector(195.15029790730165,159.78234643538997, 0.0));
    positionAlloc4->Add (Vector(47.095370597294796,67.03416267347207, 0.0));
    positionAlloc4->Add (Vector(417.70789162007054,322.38538523061845, 0.0));
    positionAlloc4->Add (Vector(109.52602611077705,259.25304079991383, 0.0));
    positionAlloc4->Add (Vector(292.23510234092385,116.06121362652816, 0.0));
    positionAlloc4->Add (Vector(284.0633667246058,130.31869022280574, 0.0));
    positionAlloc4->Add (Vector(261.82226435686545,145.71184372244156, 0.0));
    positionAlloc4->Add (Vector(65.15409058691945,412.85000041741404, 0.0));
    positionAlloc4->Add (Vector(64.58299818583806,257.5164956863433, 0.0));
    positionAlloc4->Add (Vector(334.7281041952293,101.84783432792938, 0.0));
    positionAlloc4->Add (Vector(367.10370357495304,436.0914483750195, 0.0));
    positionAlloc4->Add (Vector(464.8754968042542,92.65960154097036, 0.0));
    positionAlloc4->Add (Vector(127.61330939592075,258.93438351154737, 0.0));
    positionAlloc4->Add (Vector(349.1104729379379,279.4260672171621, 0.0));
    positionAlloc4->Add (Vector(132.77445079606693,381.4541507857849, 0.0));
    positionAlloc4->Add (Vector(64.48716057201564,59.41042007109426, 0.0));
    positionAlloc4->Add (Vector(47.272132127857915,275.68749284409535, 0.0));
    positionAlloc4->Add (Vector(446.9888325081766,322.20539869882725, 0.0));
    positionAlloc4->Add (Vector(292.63712291023285,16.648384818231055, 0.0));
    positionAlloc4->Add (Vector(197.999880600414,91.93203209372342, 0.0));
    positionAlloc4->Add (Vector(268.23178175603425,308.2614093195408, 0.0));
    positionAlloc4->Add (Vector(221.65497762004287,184.59864539273724, 0.0));
    positionAlloc4->Add (Vector(494.16430489310403,8.883124285506083, 0.0));
    positionAlloc4->Add (Vector(107.56284640582197,436.69670770467803, 0.0));
    positionAlloc4->Add (Vector(23.200414870579557,62.90401139893825, 0.0));
    positionAlloc4->Add (Vector(272.7282364527657,113.49980508276808, 0.0));
    positionAlloc4->Add (Vector(452.29144376186304,262.87873208963157, 0.0));
    positionAlloc4->Add (Vector(27.787728157368687,218.56805562150998, 0.0));
    positionAlloc4->Add (Vector(135.7247040592053,120.49095819583606, 0.0));
    positionAlloc4->Add (Vector(239.5875409334552,101.0343603933071, 0.0));
    positionAlloc4->Add (Vector(162.60897553946452,455.31694464678736, 0.0));
    positionAlloc4->Add (Vector(477.19741592428414,441.5773277010132, 0.0));
    positionAlloc4->Add (Vector(11.489085891298101,141.9862427242985, 0.0));
    positionAlloc4->Add (Vector(398.69113699039315,323.73386947762725, 0.0));
    positionAlloc4->Add (Vector(169.14170798626088,234.90180927346216, 0.0));
    positionAlloc4->Add (Vector(343.5366730288849,376.39059624446844, 0.0));
    positionAlloc4->Add (Vector(114.92659456750998,2.3433698031938843, 0.0));
    positionAlloc4->Add (Vector(167.99850059277733,162.22716368982864, 0.0));
    positionAlloc4->Add (Vector(30.69912945431469,269.0292591861961, 0.0));
    positionAlloc4->Add (Vector(92.59682582082768,271.29739053320947, 0.0));
    positionAlloc4->Add (Vector(408.06390938749934,77.12303370981071, 0.0));
    positionAlloc4->Add (Vector(419.5804793278578,342.9564028876536, 0.0));
    positionAlloc4->Add (Vector(286.19492710428165,171.14470199284736, 0.0));
    positionAlloc4->Add (Vector(251.3867277532566,139.27305645512521, 0.0));
    positionAlloc4->Add (Vector(307.3914823257504,142.77857260508142, 0.0));
    positionAlloc4->Add (Vector(179.76724765990386,240.1898983510531, 0.0));
    positionAlloc4->Add (Vector(327.9027707260714,92.85904948754632, 0.0));
    positionAlloc4->Add (Vector(210.1646247903277,250.31598655571574, 0.0));
    positionAlloc4->Add (Vector(51.57111809296294,303.55342770726986, 0.0));
    positionAlloc4->Add (Vector(471.04832887377535,334.9974524152042, 0.0));
    positionAlloc4->Add (Vector(281.16376293164467,227.6159098375217, 0.0));
    positionAlloc4->Add (Vector(204.9493018545354,72.93129163120699, 0.0));
    positionAlloc4->Add (Vector(70.7635155282737,94.68479526632773, 0.0));
    positionAlloc4->Add (Vector(80.11516584587946,255.02211820649762, 0.0));
    positionAlloc4->Add (Vector(228.73964313399242,384.31730213811943, 0.0));
    positionAlloc4->Add (Vector(135.5130613311285,251.5398492237974, 0.0));
    positionAlloc4->Add (Vector(196.00482996485175,22.37790846525978, 0.0));
    positionAlloc4->Add (Vector(213.61707121483326,48.404144981590626, 0.0));
    positionAlloc4->Add (Vector(83.51706901359074,340.0972663585883, 0.0));
    positionAlloc4->Add (Vector(211.38990296330894,54.130115797643654, 0.0));
    positionAlloc4->Add (Vector(23.622654342931092,402.476217353331, 0.0));
    positionAlloc4->Add (Vector(193.49703579123135,31.242739183301616, 0.0));
    positionAlloc4->Add (Vector(491.21622725960196,329.1522507493857, 0.0));
    positionAlloc4->Add (Vector(77.96540548254704,17.74518961813004, 0.0));
    positionAlloc4->Add (Vector(26.66906462742402,89.64190370767506, 0.0));
    positionAlloc4->Add (Vector(77.13529704432321,56.339769066754755, 0.0));
    positionAlloc4->Add (Vector(8.281285862143772,274.83937418737025, 0.0));
    positionAlloc4->Add (Vector(185.87028139828172,182.66724600996693, 0.0));
    positionAlloc4->Add (Vector(418.103069154961,426.3695996439327, 0.0));
    positionAlloc4->Add (Vector(313.8181511187904,155.40841361872094, 0.0));
    positionAlloc4->Add (Vector(3.9043345469196855,125.45215422939188, 0.0));
    positionAlloc4->Add (Vector(415.4130734026564,110.20339298266823, 0.0));
    positionAlloc4->Add (Vector(97.79164112493677,340.4132544548369, 0.0));
    positionAlloc4->Add (Vector(149.19701430041448,245.18082934975905, 0.0));
    positionAlloc4->Add (Vector(53.20854657576307,361.56040730043946, 0.0));
    positionAlloc4->Add (Vector(35.438003793869136,212.5256166420386, 0.0));
    positionAlloc4->Add (Vector(213.68483878366683,113.58767724803597, 0.0));
    positionAlloc4->Add (Vector(167.55334900774892,126.16246700122963, 0.0));
    positionAlloc4->Add (Vector(280.5379453096329,281.04890648009166, 0.0));
    positionAlloc4->Add (Vector(272.5194387429068,498.7738672724476, 0.0));
    positionAlloc4->Add (Vector(149.33149481835238,434.2486500833669, 0.0));
    positionAlloc4->Add (Vector(13.200977762174826,269.82516104974786, 0.0));
    positionAlloc4->Add (Vector(53.998361329638755,358.1888387805494, 0.0));
    positionAlloc4->Add (Vector(138.86853488501043,34.83618036481217, 0.0));
    positionAlloc4->Add (Vector(293.27130121199053,66.13554844061542, 0.0));
    positionAlloc4->Add (Vector(295.53914282568724,239.84585592891867, 0.0));
    positionAlloc4->Add (Vector(96.91755631436743,81.43338764623037, 0.0));
    positionAlloc4->Add (Vector(372.8997611175834,398.5033055316521, 0.0));
    positionAlloc4->Add (Vector(472.3863496838313,15.243138092798315, 0.0));
    positionAlloc4->Add (Vector(466.43716331979834,198.98267085038745, 0.0));
    positionAlloc4->Add (Vector(189.54281391406437,273.2624213597426, 0.0));
    positionAlloc4->Add (Vector(488.39485395778087,427.56882000602315, 0.0));
    positionAlloc4->Add (Vector(246.17225526766313,29.551695477436468, 0.0));
    positionAlloc4->Add (Vector(222.8462027375942,296.3698180043959, 0.0));
    positionAlloc4->Add (Vector(294.9075440489012,202.81568902833803, 0.0));
    positionAlloc4->Add (Vector(289.31457477503557,207.05252393318003, 0.0));
    positionAlloc4->Add (Vector(380.99124897716115,267.784398908416, 0.0));
    positionAlloc4->Add (Vector(80.73101518090975,13.497396462193223, 0.0));
    positionAlloc4->Add (Vector(443.7004823011977,318.6887200216652, 0.0));
    positionAlloc4->Add (Vector(282.30324518417683,281.03062199329037, 0.0));
    positionAlloc4->Add (Vector(127.6031265282086,148.79169332026765, 0.0));
    positionAlloc4->Add (Vector(393.22971783509536,13.975924519897486, 0.0));
    positionAlloc4->Add (Vector(396.23088079203535,36.9755419796905, 0.0));
    positionAlloc4->Add (Vector(92.76602472455359,28.255887743871998, 0.0));
    positionAlloc4->Add (Vector(298.8683299885573,13.7239783507912, 0.0));
    positionAlloc4->Add (Vector(489.39159126278633,430.18572349507093, 0.0));
    positionAlloc4->Add (Vector(495.60605179028465,176.4058675533001, 0.0));
    positionAlloc4->Add (Vector(369.55254166422213,175.2033474286116, 0.0));
    positionAlloc4->Add (Vector(488.12209279391226,89.10560356210084, 0.0));
    positionAlloc4->Add (Vector(354.5702272425698,285.1525370598186, 0.0));
    positionAlloc4->Add (Vector(467.98970125746905,217.03449284065556, 0.0));
    positionAlloc4->Add (Vector(72.57343197850975,237.514773785171, 0.0));
    positionAlloc4->Add (Vector(138.2182006826896,24.521739432754597, 0.0));
    positionAlloc4->Add (Vector(46.21546343668454,61.74750804276452, 0.0));
    positionAlloc4->Add (Vector(340.69742393817484,133.44573096270307, 0.0));
    positionAlloc4->Add (Vector(63.86902348970569,446.77968703592427, 0.0));
    positionAlloc4->Add (Vector(202.70643296968015,152.36781814407664, 0.0));
    positionAlloc4->Add (Vector(131.69873641306012,205.30050459776496, 0.0));
    positionAlloc4->Add (Vector(37.2669255917521,403.8453976509978, 0.0));
    positionAlloc4->Add (Vector(121.49397264598205,156.38954127882704, 0.0));
    positionAlloc4->Add (Vector(291.6094405000696,67.12470498986272, 0.0));
    positionAlloc4->Add (Vector(39.258508328871955,186.43483555559703, 0.0));
    positionAlloc4->Add (Vector(177.40212192706505,271.500782414405, 0.0));
    positionAlloc4->Add (Vector(180.58096924072265,139.89269613530215, 0.0));
    positionAlloc4->Add (Vector(76.43106469791444,180.25817849660385, 0.0));
    positionAlloc4->Add (Vector(64.62570403874484,443.0100969722026, 0.0));
    positionAlloc4->Add (Vector(492.90891018135915,468.91581750945204, 0.0));
    positionAlloc4->Add (Vector(416.2396052556034,469.5730099683343, 0.0));
    positionAlloc4->Add (Vector(480.2590201862656,10.694746007982591, 0.0));
    positionAlloc4->Add (Vector(233.6115632155517,367.6028717162661, 0.0));
    positionAlloc4->Add (Vector(325.91053566312934,60.94101358803772, 0.0));
    positionAlloc4->Add (Vector(142.78039060726562,231.77196564309665, 0.0));
    positionAlloc4->Add (Vector(152.01093543405685,332.5385254908473, 0.0));
    positionAlloc4->Add (Vector(493.10255248655994,244.45926621055747, 0.0));
    positionAlloc4->Add (Vector(307.7917683863768,3.774168113232723, 0.0));
    positionAlloc4->Add (Vector(128.59554092092512,388.1707493922008, 0.0));
    positionAlloc4->Add (Vector(490.5759940730493,200.3792357443599, 0.0));
    positionAlloc4->Add (Vector(41.33079257720451,260.68506214767785, 0.0));
    positionAlloc4->Add (Vector(398.3593794770584,139.9227919865513, 0.0));
    positionAlloc4->Add (Vector(193.4948992122626,229.91025348658596, 0.0));
    positionAlloc4->Add (Vector(89.6797722189685,147.4193844464433, 0.0));
    positionAlloc4->Add (Vector(320.84531624448255,0.7522350927156585, 0.0));
    positionAlloc4->Add (Vector(460.52590596244556,188.84957003222775, 0.0));
    positionAlloc4->Add (Vector(366.4389233384645,239.49646641409072, 0.0));
    positionAlloc4->Add (Vector(366.7590669526798,489.1866445928206, 0.0));
    positionAlloc4->Add (Vector(87.41202330238207,170.98960075818513, 0.0));
    positionAlloc4->Add (Vector(410.8554694161335,3.477458769731545, 0.0));
    positionAlloc4->Add (Vector(407.54882381927837,365.3048344175647, 0.0));
    positionAlloc4->Add (Vector(376.9041041475485,468.3085840928762, 0.0));
    positionAlloc4->Add (Vector(345.7886035730742,117.9090976391411, 0.0));
    positionAlloc4->Add (Vector(233.33775522257517,322.69896606754463, 0.0));
    positionAlloc4->Add (Vector(352.0740650036593,266.31308344515475, 0.0));
    positionAlloc4->Add (Vector(394.99158545105314,355.27371757647995, 0.0));
    positionAlloc4->Add (Vector(453.54408730173236,190.53534918268284, 0.0));
    positionAlloc4->Add (Vector(303.1114999559326,30.30577323700956, 0.0));
    positionAlloc4->Add (Vector(169.2898979673183,143.9723319533789, 0.0));
    positionAlloc4->Add (Vector(83.12412426801224,444.5435398449051, 0.0));
    positionAlloc4->Add (Vector(101.52993902993818,419.9436703325661, 0.0));
    positionAlloc4->Add (Vector(498.3002694199547,386.72664157022746, 0.0));
    positionAlloc4->Add (Vector(438.72256429051595,437.02482406425725, 0.0));
    positionAlloc4->Add (Vector(248.8189422549067,477.45197832609796, 0.0));
    positionAlloc4->Add (Vector(106.27315827407746,199.97480243785054, 0.0));
    positionAlloc4->Add (Vector(231.91607388645525,327.1510401126092, 0.0));
    positionAlloc4->Add (Vector(79.09993592315773,23.226425521868986, 0.0));
    positionAlloc4->Add (Vector(280.90643204195186,232.8353449760125, 0.0));
    positionAlloc4->Add (Vector(364.35318936641085,397.62691370954514, 0.0));
    positionAlloc4->Add (Vector(386.2450841310172,108.40734403526191, 0.0));
    positionAlloc4->Add (Vector(342.26975410842954,330.36864429625024, 0.0));
    positionAlloc4->Add (Vector(113.04929236677374,216.82180433259944, 0.0));
    positionAlloc4->Add (Vector(202.38516249112203,114.0125481555035, 0.0));
    positionAlloc4->Add (Vector(216.47739535868084,155.70452302918403, 0.0));
    positionAlloc4->Add (Vector(423.83224071879204,498.06309554951696, 0.0));
    positionAlloc4->Add (Vector(6.506333970812572,321.1264020582197, 0.0));
    positionAlloc4->Add (Vector(285.1920132236907,279.2469005449277, 0.0));
    positionAlloc4->Add (Vector(49.57676071080186,431.5473588734771, 0.0));
    positionAlloc4->Add (Vector(438.23777285628563,27.856805176567466, 0.0));
    positionAlloc4->Add (Vector(433.70781605037956,289.25989107006916, 0.0));
    positionAlloc4->Add (Vector(489.88486643080455,35.36856700817076, 0.0));
    positionAlloc4->Add (Vector(287.7680755555006,238.37630998325477, 0.0));
    positionAlloc4->Add (Vector(20.338739961134223,87.86953367208095, 0.0));
    positionAlloc4->Add (Vector(19.677294845383166,46.81641786567903, 0.0));
    positionAlloc4->Add (Vector(227.272331498311,6.947199494923728, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_5.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_ECC_flowmon/switch_ECC_5.flowmon", false, false);
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