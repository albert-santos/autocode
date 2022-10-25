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
    positionAlloc->Add (Vector (313.3333333333333,270.0, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,140.0, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,10.0, 0.0));
    positionAlloc->Add (Vector (270.0,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (270.0,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (270.0,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,140.0, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,10.0, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,140.0, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (140.0,400.0, 0.0));
    positionAlloc->Add (Vector (140.0,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (140.0,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (140.0,270.0, 0.0));
    positionAlloc->Add (Vector (140.0,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,270.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,140.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (10.0,140.0, 0.0));
    positionAlloc->Add (Vector (10.0,96.66666666666667, 0.0));
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
    positionAlloc4->Add (Vector(90.32276897249405,388.5281877092844, 0.0));
    positionAlloc4->Add (Vector(177.52207934660743,40.632590378466475, 0.0));
    positionAlloc4->Add (Vector(356.2027329648358,152.73695870230446, 0.0));
    positionAlloc4->Add (Vector(340.7783085149182,337.69994382777134, 0.0));
    positionAlloc4->Add (Vector(44.498269026974,303.2246632532434, 0.0));
    positionAlloc4->Add (Vector(201.2020780203819,257.3431698596542, 0.0));
    positionAlloc4->Add (Vector(190.78036899471118,111.29537696853214, 0.0));
    positionAlloc4->Add (Vector(142.40659441247462,381.47080738956873, 0.0));
    positionAlloc4->Add (Vector(22.27936508484185,117.54499418821767, 0.0));
    positionAlloc4->Add (Vector(2.318957167075597,125.07246246219057, 0.0));
    positionAlloc4->Add (Vector(15.145035088172198,23.033402243269485, 0.0));
    positionAlloc4->Add (Vector(382.9579705306675,383.78824273991285, 0.0));
    positionAlloc4->Add (Vector(71.14589918053942,344.0267083504865, 0.0));
    positionAlloc4->Add (Vector(320.26386325169585,373.2809352746727, 0.0));
    positionAlloc4->Add (Vector(210.74075035673374,166.8409672427432, 0.0));
    positionAlloc4->Add (Vector(35.59928832895118,73.96223611136334, 0.0));
    positionAlloc4->Add (Vector(309.5664999825874,133.7105619975305, 0.0));
    positionAlloc4->Add (Vector(242.08515804928516,396.16247875597867, 0.0));
    positionAlloc4->Add (Vector(284.14183132784376,182.25736985682158, 0.0));
    positionAlloc4->Add (Vector(207.21302463776348,149.09952083899424, 0.0));
    positionAlloc4->Add (Vector(380.77406702716996,355.19859757058583, 0.0));
    positionAlloc4->Add (Vector(390.7664647783401,209.42212188675714, 0.0));
    positionAlloc4->Add (Vector(123.1529775270587,371.2436314002905, 0.0));
    positionAlloc4->Add (Vector(318.62343501468445,129.85532888119863, 0.0));
    positionAlloc4->Add (Vector(308.40214318187293,131.89403169770438, 0.0));
    positionAlloc4->Add (Vector(11.586474471159391,353.87275772371777, 0.0));
    positionAlloc4->Add (Vector(184.60850129884582,112.52050901472455, 0.0));
    positionAlloc4->Add (Vector(290.25701819794756,10.37031711370795, 0.0));
    positionAlloc4->Add (Vector(101.53555293887489,364.4071894794506, 0.0));
    positionAlloc4->Add (Vector(138.67795254343127,141.45911482680202, 0.0));
    positionAlloc4->Add (Vector(274.06919876307984,195.02286699316724, 0.0));
    positionAlloc4->Add (Vector(37.90676658289165,336.69473047433985, 0.0));
    positionAlloc4->Add (Vector(5.897516359978772,388.72802919117044, 0.0));
    positionAlloc4->Add (Vector(28.21681789747967,29.386661744049334, 0.0));
    positionAlloc4->Add (Vector(399.40820283592234,55.85973347364139, 0.0));
    positionAlloc4->Add (Vector(358.4173966300008,389.08814114112994, 0.0));
    positionAlloc4->Add (Vector(239.89613778883947,333.1872575348282, 0.0));
    positionAlloc4->Add (Vector(367.69500170246897,306.00205931925717, 0.0));
    positionAlloc4->Add (Vector(29.892578005904458,110.72258504023465, 0.0));
    positionAlloc4->Add (Vector(259.8332219513916,372.5788059971643, 0.0));
    positionAlloc4->Add (Vector(219.62137754020787,320.39782231934237, 0.0));
    positionAlloc4->Add (Vector(307.1851513125246,21.237038181010035, 0.0));
    positionAlloc4->Add (Vector(110.88004191303939,213.9328930079246, 0.0));
    positionAlloc4->Add (Vector(207.31263213072495,2.126613184441295, 0.0));
    positionAlloc4->Add (Vector(15.041551459840052,305.89065560019446, 0.0));
    positionAlloc4->Add (Vector(176.40264816354846,295.84478500301867, 0.0));
    positionAlloc4->Add (Vector(238.75540997838925,391.83606974377005, 0.0));
    positionAlloc4->Add (Vector(245.2449341468049,374.2945066432799, 0.0));
    positionAlloc4->Add (Vector(224.5965035197497,143.23196560517434, 0.0));
    positionAlloc4->Add (Vector(271.3929815991696,274.92125974975596, 0.0));
    positionAlloc4->Add (Vector(156.2441994947623,278.5080332404412, 0.0));
    positionAlloc4->Add (Vector(222.3600844506302,214.7073287769856, 0.0));
    positionAlloc4->Add (Vector(215.19508805587236,135.68167395403998, 0.0));
    positionAlloc4->Add (Vector(41.98146733197121,155.54936107788976, 0.0));
    positionAlloc4->Add (Vector(84.30837683480749,201.12975340736182, 0.0));
    positionAlloc4->Add (Vector(264.2393423714395,35.852279570984805, 0.0));
    positionAlloc4->Add (Vector(391.4691220633388,94.55363946415022, 0.0));
    positionAlloc4->Add (Vector(105.25560956954969,28.644111776721815, 0.0));
    positionAlloc4->Add (Vector(345.04667780208877,66.97674300260852, 0.0));
    positionAlloc4->Add (Vector(280.21322904780106,372.22392769993803, 0.0));
    positionAlloc4->Add (Vector(298.662988849202,216.7884247085766, 0.0));
    positionAlloc4->Add (Vector(275.8557596318231,239.52835624936876, 0.0));
    positionAlloc4->Add (Vector(87.18757202446716,370.30992065457593, 0.0));
    positionAlloc4->Add (Vector(180.25730746885378,133.76775707890906, 0.0));
    positionAlloc4->Add (Vector(161.6006436217516,54.44526169924884, 0.0));
    positionAlloc4->Add (Vector(157.5536031421506,275.5820192921006, 0.0));
    positionAlloc4->Add (Vector(316.2547974358395,258.03135963417054, 0.0));
    positionAlloc4->Add (Vector(157.129177998785,157.18919917664658, 0.0));
    positionAlloc4->Add (Vector(241.72273746754888,383.52100625480017, 0.0));
    positionAlloc4->Add (Vector(214.80284673621628,146.3933386678617, 0.0));
    positionAlloc4->Add (Vector(314.9053790097046,40.09385724125294, 0.0));
    positionAlloc4->Add (Vector(193.49078047556233,235.9805193197615, 0.0));
    positionAlloc4->Add (Vector(218.58183628057225,326.5769590223751, 0.0));
    positionAlloc4->Add (Vector(138.94776814912512,312.0898318765455, 0.0));
    positionAlloc4->Add (Vector(393.03591572203413,365.49584993665263, 0.0));
    positionAlloc4->Add (Vector(73.99318622033194,337.70400203003936, 0.0));
    positionAlloc4->Add (Vector(92.36338019367167,69.95860359276888, 0.0));
    positionAlloc4->Add (Vector(271.35078890052796,201.27227621715483, 0.0));
    positionAlloc4->Add (Vector(268.9644395735703,3.3532318100126357, 0.0));
    positionAlloc4->Add (Vector(296.68192009262214,246.00628998841952, 0.0));
    positionAlloc4->Add (Vector(31.736633183327577,121.14332188703143, 0.0));
    positionAlloc4->Add (Vector(50.68535353876911,97.70666528359033, 0.0));
    positionAlloc4->Add (Vector(171.51806413951908,152.84025701965584, 0.0));
    positionAlloc4->Add (Vector(101.55544837709587,236.55487290700674, 0.0));
    positionAlloc4->Add (Vector(183.98269242970335,305.54908722657194, 0.0));
    positionAlloc4->Add (Vector(26.995737227016914,352.2154390257307, 0.0));
    positionAlloc4->Add (Vector(255.34212829646918,131.02777764493004, 0.0));
    positionAlloc4->Add (Vector(97.15662998594827,249.29885807503362, 0.0));
    positionAlloc4->Add (Vector(297.6543685478141,236.20821905884685, 0.0));
    positionAlloc4->Add (Vector(47.27705173881538,334.7991128854049, 0.0));
    positionAlloc4->Add (Vector(287.03798487790175,76.34797726892297, 0.0));
    positionAlloc4->Add (Vector(153.3053827221147,119.02988612555009, 0.0));
    positionAlloc4->Add (Vector(101.77524092576884,353.98660753522535, 0.0));
    positionAlloc4->Add (Vector(154.79569370251687,197.1523493725281, 0.0));
    positionAlloc4->Add (Vector(110.2412411043435,369.89632541534866, 0.0));
    positionAlloc4->Add (Vector(198.33055883408494,145.3954733139621, 0.0));
    positionAlloc4->Add (Vector(188.19907046088363,330.8544009329153, 0.0));
    positionAlloc4->Add (Vector(349.23775130785623,280.1157750533799, 0.0));
    positionAlloc4->Add (Vector(81.37352974803625,358.52902272160185, 0.0));
    positionAlloc4->Add (Vector(91.03909532890123,58.161453668494275, 0.0));
    positionAlloc4->Add (Vector(123.69855386351185,0.3057541912752093, 0.0));
    positionAlloc4->Add (Vector(355.77699018579153,233.36252584329117, 0.0));
    positionAlloc4->Add (Vector(49.10885177976798,75.02795704689582, 0.0));
    positionAlloc4->Add (Vector(142.0837557358575,389.6850511580365, 0.0));
    positionAlloc4->Add (Vector(75.3873598665674,274.3027596690189, 0.0));
    positionAlloc4->Add (Vector(8.908604613121662,328.44875928758586, 0.0));
    positionAlloc4->Add (Vector(104.79988188773426,364.3224556347034, 0.0));
    positionAlloc4->Add (Vector(194.8775636293695,167.99742080409632, 0.0));
    positionAlloc4->Add (Vector(224.37950204877808,312.1454093157074, 0.0));
    positionAlloc4->Add (Vector(157.9529050147202,184.63188438495445, 0.0));
    positionAlloc4->Add (Vector(371.9822494613271,59.69441292390001, 0.0));
    positionAlloc4->Add (Vector(165.56793829176354,396.36625301018006, 0.0));
    positionAlloc4->Add (Vector(49.36754392589195,253.80748651673665, 0.0));
    positionAlloc4->Add (Vector(29.829628500591944,148.12227750955506, 0.0));
    positionAlloc4->Add (Vector(207.52170029336568,68.91188059085019, 0.0));
    positionAlloc4->Add (Vector(152.0945984469059,397.08034946592716, 0.0));
    positionAlloc4->Add (Vector(134.69839948046692,396.7780047629124, 0.0));
    positionAlloc4->Add (Vector(79.13311385916772,171.73228904659692, 0.0));
    positionAlloc4->Add (Vector(382.52060390209283,235.9692508303079, 0.0));
    positionAlloc4->Add (Vector(328.73974990560424,251.29583885226313, 0.0));
    positionAlloc4->Add (Vector(254.14838459538137,339.11521748589666, 0.0));
    positionAlloc4->Add (Vector(137.73567818002155,60.5502279971196, 0.0));
    positionAlloc4->Add (Vector(339.90756400717225,266.8475154796816, 0.0));
    positionAlloc4->Add (Vector(351.4351452819082,76.33574530739224, 0.0));
    positionAlloc4->Add (Vector(359.6883798263875,30.278752703321345, 0.0));
    positionAlloc4->Add (Vector(131.93782507275293,273.22974653462563, 0.0));
    positionAlloc4->Add (Vector(348.37824366442135,363.49810914335524, 0.0));
    positionAlloc4->Add (Vector(357.7729673710289,121.99759033408641, 0.0));
    positionAlloc4->Add (Vector(80.2909952140364,211.81864419135798, 0.0));
    positionAlloc4->Add (Vector(262.8261698491558,232.41426296723606, 0.0));
    positionAlloc4->Add (Vector(171.37699038042936,345.87445790541375, 0.0));
    positionAlloc4->Add (Vector(132.1566985446811,337.3976504254622, 0.0));
    positionAlloc4->Add (Vector(291.75907930246103,289.9666988876477, 0.0));
    positionAlloc4->Add (Vector(79.4366012215705,10.19049431152812, 0.0));
    positionAlloc4->Add (Vector(94.03105467446812,184.64083235664881, 0.0));
    positionAlloc4->Add (Vector(196.44398452171848,35.269472496872424, 0.0));
    positionAlloc4->Add (Vector(239.8544033694867,275.538103607466, 0.0));
    positionAlloc4->Add (Vector(77.53910700380287,328.2985994491753, 0.0));
    positionAlloc4->Add (Vector(46.0140048200985,149.36285269487811, 0.0));
    positionAlloc4->Add (Vector(171.0660398787857,39.77622516623569, 0.0));
    positionAlloc4->Add (Vector(245.41035986737265,304.00070093989956, 0.0));
    positionAlloc4->Add (Vector(128.22586159806409,326.1907716022505, 0.0));
    positionAlloc4->Add (Vector(70.67155722893656,156.43964924270497, 0.0));
    positionAlloc4->Add (Vector(361.641194365522,93.50747704164237, 0.0));
    positionAlloc4->Add (Vector(304.3209825223024,312.4183288775828, 0.0));
    positionAlloc4->Add (Vector(247.44029702461455,60.39229331185831, 0.0));
    positionAlloc4->Add (Vector(272.65813504104915,278.8594115011025, 0.0));
    positionAlloc4->Add (Vector(179.49490352972876,281.7253593535638, 0.0));
    positionAlloc4->Add (Vector(178.26918256066241,268.98906020997197, 0.0));
    positionAlloc4->Add (Vector(32.02439090848692,192.67851320847083, 0.0));
    positionAlloc4->Add (Vector(322.4652651813633,300.6561616325162, 0.0));
    positionAlloc4->Add (Vector(388.4333278501855,301.22826234778194, 0.0));
    positionAlloc4->Add (Vector(278.3384535729306,28.778888065600448, 0.0));
    positionAlloc4->Add (Vector(97.77919974864434,233.8189064466519, 0.0));
    positionAlloc4->Add (Vector(122.11531973897331,148.66183068967297, 0.0));
    positionAlloc4->Add (Vector(235.14860251847458,105.65505768422784, 0.0));
    positionAlloc4->Add (Vector(202.8477694192818,181.87689102096277, 0.0));
    positionAlloc4->Add (Vector(117.35051139442265,371.5797847572463, 0.0));
    positionAlloc4->Add (Vector(398.9939970913318,232.8119905373847, 0.0));
    positionAlloc4->Add (Vector(121.33826868342763,8.550384435228287, 0.0));
    positionAlloc4->Add (Vector(315.32047586718755,4.838704006143146, 0.0));
    positionAlloc4->Add (Vector(14.427325827943971,80.07284656439478, 0.0));
    positionAlloc4->Add (Vector(50.69428365680344,235.92945924395568, 0.0));
    positionAlloc4->Add (Vector(288.52133767462834,176.1388347324953, 0.0));
    positionAlloc4->Add (Vector(128.3945498097813,49.1819086208714, 0.0));
    positionAlloc4->Add (Vector(329.0312610095484,351.79340937172674, 0.0));
    positionAlloc4->Add (Vector(342.07611356165415,241.8688209396751, 0.0));
    positionAlloc4->Add (Vector(367.04253905775727,235.34693315755072, 0.0));
    positionAlloc4->Add (Vector(2.92758773114552,131.30244146031234, 0.0));
    positionAlloc4->Add (Vector(77.02240838202736,301.1703659919769, 0.0));
    positionAlloc4->Add (Vector(379.2837992750501,213.87162287183548, 0.0));
    positionAlloc4->Add (Vector(23.670882874288957,300.12687382987434, 0.0));
    positionAlloc4->Add (Vector(239.5941155893679,246.28171488157952, 0.0));
    positionAlloc4->Add (Vector(138.2716853184102,238.25003925397988, 0.0));
    positionAlloc4->Add (Vector(240.8734615570843,385.4268348753368, 0.0));
    positionAlloc4->Add (Vector(64.73908564953406,337.9490072942956, 0.0));
    positionAlloc4->Add (Vector(95.77849476187214,52.56626421498423, 0.0));
    positionAlloc4->Add (Vector(90.80046608586318,238.3929542452055, 0.0));
    positionAlloc4->Add (Vector(227.0761320476597,46.04121638676135, 0.0));
    positionAlloc4->Add (Vector(112.85577936194424,353.50220327224037, 0.0));
    positionAlloc4->Add (Vector(249.9285176741996,342.01668853894057, 0.0));
    positionAlloc4->Add (Vector(239.38587724185453,210.90698259416678, 0.0));
    positionAlloc4->Add (Vector(352.7082492331229,194.70029792733524, 0.0));
    positionAlloc4->Add (Vector(81.35233312962042,330.49101419695654, 0.0));
    positionAlloc4->Add (Vector(25.77210223184272,358.794027803033, 0.0));
    positionAlloc4->Add (Vector(349.07446596778476,168.6663739398837, 0.0));
    positionAlloc4->Add (Vector(99.31629159883943,118.03143009979836, 0.0));
    positionAlloc4->Add (Vector(247.3383477311542,212.5151241924232, 0.0));
    positionAlloc4->Add (Vector(52.20426853570639,336.9783239825654, 0.0));
    positionAlloc4->Add (Vector(325.8476725046621,21.645464129599958, 0.0));
    positionAlloc4->Add (Vector(101.37561699991231,240.19558210981776, 0.0));
    positionAlloc4->Add (Vector(241.97074918971268,292.9499596717322, 0.0));
    positionAlloc4->Add (Vector(348.1699427292265,280.55211785333853, 0.0));
    positionAlloc4->Add (Vector(273.687220393291,25.964634135651732, 0.0));
    positionAlloc4->Add (Vector(94.32585485900549,376.33118528055974, 0.0));
    positionAlloc4->Add (Vector(359.1402870670727,173.8379893634038, 0.0));
    positionAlloc4->Add (Vector(389.39495893007853,137.9445374865004, 0.0));
    positionAlloc4->Add (Vector(377.55705111351847,245.57360095388034, 0.0));
    positionAlloc4->Add (Vector(95.76441765510238,380.9895126773037, 0.0));
    positionAlloc4->Add (Vector(17.316375274463436,194.5762972681579, 0.0));
    positionAlloc4->Add (Vector(309.1542408668471,336.56886271833736, 0.0));
    positionAlloc4->Add (Vector(189.2447811543514,50.21724392348945, 0.0));
    positionAlloc4->Add (Vector(345.5316582005808,32.64959308602724, 0.0));
    positionAlloc4->Add (Vector(25.33737738253592,168.66315441298036, 0.0));
    positionAlloc4->Add (Vector(20.364953406754882,94.02874563640582, 0.0));
    positionAlloc4->Add (Vector(329.2012162593884,380.7102772135407, 0.0));
    positionAlloc4->Add (Vector(349.2272593788398,262.48061909763953, 0.0));
    positionAlloc4->Add (Vector(181.62362242366052,6.161407916768358, 0.0));
    positionAlloc4->Add (Vector(172.9422379057376,339.0801872949751, 0.0));
    positionAlloc4->Add (Vector(204.8244110095675,246.98440480686244, 0.0));
    positionAlloc4->Add (Vector(20.9251785749907,242.0467122001465, 0.0));
    positionAlloc4->Add (Vector(77.36655960596734,134.2655204658599, 0.0));
    positionAlloc4->Add (Vector(15.943970654181028,53.91724718427162, 0.0));
    positionAlloc4->Add (Vector(307.1155910632692,214.40339789582546, 0.0));
    positionAlloc4->Add (Vector(268.1400127744673,172.18871309799, 0.0));
    positionAlloc4->Add (Vector(14.845065797597767,277.5421645495599, 0.0));
    positionAlloc4->Add (Vector(16.59003183059955,128.7315199880978, 0.0));
    positionAlloc4->Add (Vector(187.7916114239703,84.30580098748214, 0.0));
    positionAlloc4->Add (Vector(300.6830883677067,305.0528381780443, 0.0));
    positionAlloc4->Add (Vector(130.32398664528193,355.93093657894997, 0.0));
    positionAlloc4->Add (Vector(130.38688292359436,309.79090640603187, 0.0));
    positionAlloc4->Add (Vector(124.68116708759256,267.51864236914787, 0.0));
    positionAlloc4->Add (Vector(221.86692431506668,389.761387565201, 0.0));
    positionAlloc4->Add (Vector(211.1387184471838,135.7666896070438, 0.0));
    positionAlloc4->Add (Vector(325.0029216698687,113.52073397874972, 0.0));
    positionAlloc4->Add (Vector(349.6503742392113,380.45424736747685, 0.0));
    positionAlloc4->Add (Vector(210.320787980588,30.409551776842747, 0.0));
    positionAlloc4->Add (Vector(154.4016044090382,228.61683709013292, 0.0));
    positionAlloc4->Add (Vector(348.0766984821511,94.74748142279941, 0.0));
    positionAlloc4->Add (Vector(57.11731312120016,37.64010979985577, 0.0));
    positionAlloc4->Add (Vector(88.05146961079248,268.57166843250934, 0.0));
    positionAlloc4->Add (Vector(131.34455603503605,219.0257081923814, 0.0));
    positionAlloc4->Add (Vector(101.43269974865134,109.52388166324343, 0.0));
    positionAlloc4->Add (Vector(178.6540183294762,350.8137557081408, 0.0));
    positionAlloc4->Add (Vector(215.85964403734025,358.10414629248834, 0.0));
    positionAlloc4->Add (Vector(374.9008248513796,195.82598445806744, 0.0));
    positionAlloc4->Add (Vector(189.60251599522184,139.66696518283092, 0.0));
    positionAlloc4->Add (Vector(15.891482673233925,28.133376646210095, 0.0));
    positionAlloc4->Add (Vector(279.84927295983357,367.30343565282317, 0.0));
    positionAlloc4->Add (Vector(397.38418523994,333.79460560721185, 0.0));
    positionAlloc4->Add (Vector(128.41052566692633,223.164797697509, 0.0));
    positionAlloc4->Add (Vector(260.71571040403614,161.06274086996768, 0.0));
    positionAlloc4->Add (Vector(6.630353160373659,45.51854257896273, 0.0));
    positionAlloc4->Add (Vector(28.36736018815489,61.04159481690874, 0.0));
    positionAlloc4->Add (Vector(152.43259651738094,263.7379744781459, 0.0));
    positionAlloc4->Add (Vector(6.686235630802573,159.46362396895148, 0.0));
    positionAlloc4->Add (Vector(116.11919882474729,86.75659504471813, 0.0));
    positionAlloc4->Add (Vector(76.67029556395603,307.1108768589096, 0.0));
    positionAlloc4->Add (Vector(159.68744214098152,318.78490079196007, 0.0));
    positionAlloc4->Add (Vector(57.44279914968371,33.28105457435022, 0.0));
    positionAlloc4->Add (Vector(199.97294843967998,310.8392917103132, 0.0));
    positionAlloc4->Add (Vector(43.753165677433834,269.2164216866488, 0.0));
    positionAlloc4->Add (Vector(201.4038039393586,386.37322600788895, 0.0));
    positionAlloc4->Add (Vector(10.301392962633349,116.46157622097122, 0.0));
    positionAlloc4->Add (Vector(28.80481406790798,167.2544534901263, 0.0));
    positionAlloc4->Add (Vector(163.63633276674398,107.55680888505835, 0.0));
    positionAlloc4->Add (Vector(354.3991884661405,211.04750892462457, 0.0));
    positionAlloc4->Add (Vector(190.8051977609053,179.5515346827062, 0.0));
    positionAlloc4->Add (Vector(56.88850122911467,290.75750133782503, 0.0));
    positionAlloc4->Add (Vector(60.31375934887704,147.7599702447625, 0.0));
    positionAlloc4->Add (Vector(197.93723367302545,211.41339607102606, 0.0));
    positionAlloc4->Add (Vector(283.3843480206869,310.9875000012757, 0.0));
    positionAlloc4->Add (Vector(349.96102048495237,180.73449976834826, 0.0));
    positionAlloc4->Add (Vector(252.38368013581217,392.3694703002326, 0.0));
    positionAlloc4->Add (Vector(228.47029567449337,72.18740359804623, 0.0));
    positionAlloc4->Add (Vector(206.68254390704485,12.43883018221914, 0.0));
    positionAlloc4->Add (Vector(398.02092893564446,23.34505845905266, 0.0));
    positionAlloc4->Add (Vector(222.5627662683701,150.545286674835, 0.0));
    positionAlloc4->Add (Vector(17.413705503106147,380.3013712244009, 0.0));
    positionAlloc4->Add (Vector(98.20747207460299,364.4076958627873, 0.0));
    positionAlloc4->Add (Vector(355.6524356429931,141.9811404598288, 0.0));
    positionAlloc4->Add (Vector(309.7640533344158,221.107038619957, 0.0));
    positionAlloc4->Add (Vector(391.7933416719225,145.75122814729218, 0.0));
    positionAlloc4->Add (Vector(152.98698290836703,187.9156160345441, 0.0));
    positionAlloc4->Add (Vector(91.68201091682629,173.55335048468024, 0.0));
    positionAlloc4->Add (Vector(257.69280795161274,140.26260097286695, 0.0));
    positionAlloc4->Add (Vector(210.26777774039175,11.396339796626886, 0.0));
    positionAlloc4->Add (Vector(318.1204669622571,195.16037567184813, 0.0));
    positionAlloc4->Add (Vector(298.80193161197803,136.1517163061869, 0.0));
    positionAlloc4->Add (Vector(151.4322373002409,75.27770401192848, 0.0));
    positionAlloc4->Add (Vector(281.40773244203245,267.3824670357928, 0.0));
    positionAlloc4->Add (Vector(113.84955240277685,24.414672447868433, 0.0));
    positionAlloc4->Add (Vector(249.46816439908073,19.197025619546928, 0.0));
    positionAlloc4->Add (Vector(169.10106465782437,43.003550474067076, 0.0));
    positionAlloc4->Add (Vector(21.197817067195814,79.79821845542455, 0.0));
    positionAlloc4->Add (Vector(25.895993376619543,396.5386034927236, 0.0));
    positionAlloc4->Add (Vector(231.74421611889323,369.88167261441123, 0.0));
    positionAlloc4->Add (Vector(339.98795540638616,300.448416148775, 0.0));
    positionAlloc4->Add (Vector(171.21498778730805,239.70790226321205, 0.0));
    positionAlloc4->Add (Vector(226.264845017571,187.4663350645008, 0.0));
    positionAlloc4->Add (Vector(60.68994841278945,255.650739487962, 0.0));
    positionAlloc4->Add (Vector(379.319669629066,258.0342561041774, 0.0));
    positionAlloc4->Add (Vector(188.23515179955427,394.6291118347526, 0.0));
    positionAlloc4->Add (Vector(253.3896050489684,49.90052002767169, 0.0));
    positionAlloc4->Add (Vector(343.8603814276804,268.63935236255594, 0.0));
    positionAlloc4->Add (Vector(203.84531768702345,118.77883431476181, 0.0));
    positionAlloc4->Add (Vector(242.80618431561555,58.18624410648798, 0.0));
    positionAlloc4->Add (Vector(320.1328983105607,170.65566330725744, 0.0));
    positionAlloc4->Add (Vector(104.75216164140808,337.81077729179157, 0.0));
    positionAlloc4->Add (Vector(64.60430819440477,136.9762564539242, 0.0));
    positionAlloc4->Add (Vector(171.57293361196116,198.91567436780653, 0.0));
    positionAlloc4->Add (Vector(73.58137661888881,185.13564748330396, 0.0));
    positionAlloc4->Add (Vector(205.67001741035313,350.13309054181565, 0.0));
    positionAlloc4->Add (Vector(67.97628568690311,59.77871957554499, 0.0));
    positionAlloc4->Add (Vector(245.43547780113423,259.3251024652498, 0.0));
    positionAlloc4->Add (Vector(252.65865074637586,282.9266410726798, 0.0));
    positionAlloc4->Add (Vector(142.12316464804888,23.06192289650255, 0.0));
    positionAlloc4->Add (Vector(169.85964637253588,148.7688486670749, 0.0));
    positionAlloc4->Add (Vector(276.94404864936354,90.91901636637982, 0.0));
    positionAlloc4->Add (Vector(275.3989271795675,101.35841379451986, 0.0));
    positionAlloc4->Add (Vector(286.7083256585504,279.72304947872567, 0.0));
    positionAlloc4->Add (Vector(136.78113452098097,57.8390883352387, 0.0));
    positionAlloc4->Add (Vector(311.89771819303996,69.89452513096298, 0.0));
    positionAlloc4->Add (Vector(375.3927567426936,137.21124393216263, 0.0));
    positionAlloc4->Add (Vector(5.50700502308672,166.68958494827842, 0.0));
    positionAlloc4->Add (Vector(272.98148390672617,91.28229334019365, 0.0));
    positionAlloc4->Add (Vector(111.59635618278836,243.5324696775183, 0.0));
    positionAlloc4->Add (Vector(178.44688761377188,368.74732219352614, 0.0));
    positionAlloc4->Add (Vector(234.13938357530043,359.28201840685483, 0.0));
    positionAlloc4->Add (Vector(355.35457375815434,73.7537769471067, 0.0));
    positionAlloc4->Add (Vector(195.16859672208096,342.1425650381479, 0.0));
    positionAlloc4->Add (Vector(118.11551775993911,217.64855343414288, 0.0));
    positionAlloc4->Add (Vector(107.46004956249062,323.31120548215205, 0.0));
    positionAlloc4->Add (Vector(132.11077461003237,278.9475502452558, 0.0));
    positionAlloc4->Add (Vector(173.17540349366521,143.00446693217657, 0.0));
    positionAlloc4->Add (Vector(100.81323205233575,317.142271190755, 0.0));
    positionAlloc4->Add (Vector(342.99379963343864,247.83469574742952, 0.0));
    positionAlloc4->Add (Vector(254.52245034239343,150.0364073575644, 0.0));
    positionAlloc4->Add (Vector(128.1277303288125,392.99637798995815, 0.0));
    positionAlloc4->Add (Vector(162.76879765174317,390.0889369749248, 0.0));
    positionAlloc4->Add (Vector(163.45489000530495,96.56749918623953, 0.0));
    positionAlloc4->Add (Vector(128.7279853551616,69.041848047917, 0.0));
    positionAlloc4->Add (Vector(259.1082532749925,71.91771907718976, 0.0));
    positionAlloc4->Add (Vector(311.13939074367016,162.59314759083452, 0.0));
    positionAlloc4->Add (Vector(316.7284158016615,278.3056857087358, 0.0));
    positionAlloc4->Add (Vector(317.83134047390087,202.83196931365458, 0.0));
    positionAlloc4->Add (Vector(286.3782797849578,225.79932809634963, 0.0));
    positionAlloc4->Add (Vector(57.726160091619235,5.636917676977182, 0.0));
    positionAlloc4->Add (Vector(117.68627529770188,41.383822446560536, 0.0));
    positionAlloc4->Add (Vector(75.82068113357093,385.99556964436556, 0.0));
    positionAlloc4->Add (Vector(159.19625576316898,202.29680698032416, 0.0));
    positionAlloc4->Add (Vector(167.43121030563947,273.38596006652045, 0.0));
    positionAlloc4->Add (Vector(196.61806000677413,342.68099860711686, 0.0));
    positionAlloc4->Add (Vector(371.1395971300043,358.3879868026594, 0.0));
    positionAlloc4->Add (Vector(191.14480177288846,53.735443462278454, 0.0));
    positionAlloc4->Add (Vector(102.29807752237248,321.0758078533194, 0.0));
    positionAlloc4->Add (Vector(344.4846553467178,175.932325683429, 0.0));
    positionAlloc4->Add (Vector(3.5756409484980267,329.09200216877423, 0.0));
    positionAlloc4->Add (Vector(128.85180544590784,378.4609581827061, 0.0));
    positionAlloc4->Add (Vector(395.7360224666552,31.085828132954596, 0.0));
    positionAlloc4->Add (Vector(335.19118481909175,3.173150477809239, 0.0));
    positionAlloc4->Add (Vector(379.5345153120878,249.73468096107035, 0.0));
    positionAlloc4->Add (Vector(280.29041459301516,216.10437615875995, 0.0));
    positionAlloc4->Add (Vector(339.17575697739045,100.65565694234112, 0.0));
    positionAlloc4->Add (Vector(207.14401926892876,77.7863238111538, 0.0));
    positionAlloc4->Add (Vector(229.30890861987385,377.1050301731797, 0.0));
    positionAlloc4->Add (Vector(337.15993607713995,27.1227737363815, 0.0));
    positionAlloc4->Add (Vector(201.88024323835774,199.22224993120298, 0.0));
    positionAlloc4->Add (Vector(242.19160041237387,306.70983489484087, 0.0));
    positionAlloc4->Add (Vector(173.19260278705926,363.7247954619747, 0.0));
    positionAlloc4->Add (Vector(262.4917326868526,52.73639001100086, 0.0));
    positionAlloc4->Add (Vector(313.78296751676925,182.78628022462016, 0.0));
    positionAlloc4->Add (Vector(350.94418987792506,179.0756000582514, 0.0));
    positionAlloc4->Add (Vector(125.90627492960658,250.74224086513385, 0.0));
    positionAlloc4->Add (Vector(88.5928222934115,186.8983295706215, 0.0));
    positionAlloc4->Add (Vector(138.27952847020094,269.9886419236421, 0.0));
    positionAlloc4->Add (Vector(331.25414429260485,170.53926169449224, 0.0));
    positionAlloc4->Add (Vector(182.94407267639937,177.64684280664576, 0.0));
    positionAlloc4->Add (Vector(245.54843589650105,129.65541195897865, 0.0));
    positionAlloc4->Add (Vector(55.341875917178605,116.05778524181667, 0.0));
    positionAlloc4->Add (Vector(217.26657209523782,114.68465984257223, 0.0));
    positionAlloc4->Add (Vector(120.83869251603932,353.21140342553366, 0.0));
    positionAlloc4->Add (Vector(48.30687372369189,88.11359511749104, 0.0));
    positionAlloc4->Add (Vector(358.56664098273876,95.72628297500185, 0.0));
    positionAlloc4->Add (Vector(1.2451431300768423,309.9586586898391, 0.0));
    positionAlloc4->Add (Vector(160.74636958713495,278.9232077057364, 0.0));
    positionAlloc4->Add (Vector(300.2497276700701,245.2844846177039, 0.0));
    positionAlloc4->Add (Vector(149.95037741474374,308.129010131983, 0.0));
    positionAlloc4->Add (Vector(282.7824980995044,312.2118227506971, 0.0));
    positionAlloc4->Add (Vector(291.5403510077024,206.67551316861537, 0.0));
    positionAlloc4->Add (Vector(373.5610433259551,343.15884181515753, 0.0));
    positionAlloc4->Add (Vector(111.23401944519311,1.5174689637370342, 0.0));
    positionAlloc4->Add (Vector(286.77160067698793,394.9304699233706, 0.0));
    positionAlloc4->Add (Vector(161.28818884233968,303.49877240893466, 0.0));
    positionAlloc4->Add (Vector(344.0947989692461,223.56152446437534, 0.0));
    positionAlloc4->Add (Vector(344.78662306682935,7.910039806537705, 0.0));
    positionAlloc4->Add (Vector(289.1660774224855,70.6196137108332, 0.0));
    positionAlloc4->Add (Vector(86.64688333151176,303.6784197268579, 0.0));
    positionAlloc4->Add (Vector(59.97259396690238,220.4348696747887, 0.0));
    positionAlloc4->Add (Vector(211.16159136325066,296.03101726362866, 0.0));
    positionAlloc4->Add (Vector(314.84651586233366,75.84237912731862, 0.0));
    positionAlloc4->Add (Vector(228.96204497385972,17.93813383563525, 0.0));
    positionAlloc4->Add (Vector(394.2835294852751,70.86446577536694, 0.0));
    positionAlloc4->Add (Vector(315.0207358610029,42.63355071552044, 0.0));
    positionAlloc4->Add (Vector(142.82464355311677,43.226131478894736, 0.0));
    positionAlloc4->Add (Vector(203.168679978115,104.61982073319484, 0.0));
    positionAlloc4->Add (Vector(82.5816235557732,30.47552361292847, 0.0));
    positionAlloc4->Add (Vector(197.64650635853002,52.86445677717526, 0.0));
    positionAlloc4->Add (Vector(148.0616710214953,212.2789753904558, 0.0));
    positionAlloc4->Add (Vector(32.04145752241971,361.8524011744972, 0.0));
    positionAlloc4->Add (Vector(158.0387475559893,120.03241837014258, 0.0));
    positionAlloc4->Add (Vector(334.1086409959864,76.90000672885908, 0.0));
    positionAlloc4->Add (Vector(312.8727267325621,22.45641586476612, 0.0));
    positionAlloc4->Add (Vector(378.1233759999799,170.22455610461878, 0.0));
    positionAlloc4->Add (Vector(118.19291426219883,292.64258849660354, 0.0));
    positionAlloc4->Add (Vector(255.28748424813335,238.85092117865963, 0.0));
    positionAlloc4->Add (Vector(240.381699243056,299.57135009833456, 0.0));
    positionAlloc4->Add (Vector(301.7223802305195,84.29295683332829, 0.0));
    positionAlloc4->Add (Vector(75.49802675087437,257.5582577246651, 0.0));
    positionAlloc4->Add (Vector(58.70059230571765,291.72756774444935, 0.0));
    positionAlloc4->Add (Vector(172.47270206414836,161.6807623100589, 0.0));
    positionAlloc4->Add (Vector(63.93346013763734,81.04085008786504, 0.0));
    positionAlloc4->Add (Vector(50.48404149974348,390.417420187099, 0.0));
    positionAlloc4->Add (Vector(282.4207217343733,191.21252318611334, 0.0));
    positionAlloc4->Add (Vector(174.122383051808,369.3824860475101, 0.0));
    positionAlloc4->Add (Vector(121.8071951682481,333.9416190527194, 0.0));
    positionAlloc4->Add (Vector(284.52580725459467,221.55384442056572, 0.0));
    positionAlloc4->Add (Vector(83.05026472152575,366.93342523265227, 0.0));
    positionAlloc4->Add (Vector(32.21181085135436,92.68092419431682, 0.0));
    positionAlloc4->Add (Vector(269.8256095053642,319.9106147589545, 0.0));
    positionAlloc4->Add (Vector(10.65115387664859,44.36814761369625, 0.0));
    positionAlloc4->Add (Vector(57.00733510633786,150.83303968293365, 0.0));
    positionAlloc4->Add (Vector(85.64485472825938,79.57916937082668, 0.0));
    positionAlloc4->Add (Vector(297.78926123109284,88.55868036328043, 0.0));
    positionAlloc4->Add (Vector(123.45677905226044,293.298027090571, 0.0));
    positionAlloc4->Add (Vector(85.47729505183246,306.9138033687066, 0.0));
    positionAlloc4->Add (Vector(323.0793636448838,141.41394506719615, 0.0));
    positionAlloc4->Add (Vector(309.5197148976728,171.09493173443047, 0.0));
    positionAlloc4->Add (Vector(298.92832413276926,115.84109847475096, 0.0));
    positionAlloc4->Add (Vector(342.3313469015711,78.01499440348292, 0.0));
    positionAlloc4->Add (Vector(268.69191392696814,140.97185208656353, 0.0));
    positionAlloc4->Add (Vector(109.64791006674086,224.21199621442352, 0.0));
    positionAlloc4->Add (Vector(269.47936955945266,231.3998319156802, 0.0));
    positionAlloc4->Add (Vector(332.1598280149358,259.6616788449912, 0.0));
    positionAlloc4->Add (Vector(193.77137820575024,343.72712243904, 0.0));
    positionAlloc4->Add (Vector(257.7444339027972,2.0959350605863403, 0.0));
    positionAlloc4->Add (Vector(179.69870143465502,365.9154746967737, 0.0));
    positionAlloc4->Add (Vector(268.98793786149355,274.7232906627967, 0.0));
    positionAlloc4->Add (Vector(323.99548426496654,5.041506811950525, 0.0));
    positionAlloc4->Add (Vector(9.166052528952706,207.63292888479577, 0.0));
    positionAlloc4->Add (Vector(308.05998252483687,52.13431238174171, 0.0));
    positionAlloc4->Add (Vector(201.80766788039784,269.5396032067471, 0.0));
    positionAlloc4->Add (Vector(49.92901724467052,125.0005513968198, 0.0));
    positionAlloc4->Add (Vector(301.9565828242131,185.54311356011826, 0.0));
    positionAlloc4->Add (Vector(135.48321019267186,209.55845426645269, 0.0));
    positionAlloc4->Add (Vector(393.7533800774043,88.0936904636624, 0.0));
    positionAlloc4->Add (Vector(303.31130419079847,127.98113523107135, 0.0));
    positionAlloc4->Add (Vector(55.065096378241265,4.355493521391596, 0.0));
    positionAlloc4->Add (Vector(187.45133709844367,182.96196643577267, 0.0));
    positionAlloc4->Add (Vector(196.82872059742337,292.03648443604175, 0.0));
    positionAlloc4->Add (Vector(5.254542769228898,141.2024342167299, 0.0));
    positionAlloc4->Add (Vector(267.4931080826928,190.75293340847733, 0.0));
    positionAlloc4->Add (Vector(329.6951358497991,253.0463267111989, 0.0));
    positionAlloc4->Add (Vector(389.11503677306206,306.9899475452052, 0.0));
    positionAlloc4->Add (Vector(390.1400137081146,67.19331727984597, 0.0));
    positionAlloc4->Add (Vector(180.69325947659013,150.1739332671431, 0.0));
    positionAlloc4->Add (Vector(37.4308383269923,26.148686016890554, 0.0));
    positionAlloc4->Add (Vector(238.24317344105822,12.970530125782842, 0.0));
    positionAlloc4->Add (Vector(390.395627332362,360.6250338677285, 0.0));
    positionAlloc4->Add (Vector(326.73404351664874,237.7490495323911, 0.0));
    positionAlloc4->Add (Vector(228.28634061850255,398.701370083284, 0.0));
    positionAlloc4->Add (Vector(207.6063936294715,69.888060043569, 0.0));
    positionAlloc4->Add (Vector(265.6410221419174,198.9000373442125, 0.0));
    positionAlloc4->Add (Vector(22.78575979313309,91.52546838415034, 0.0));
    positionAlloc4->Add (Vector(229.6876355363439,139.59503888831554, 0.0));
    positionAlloc4->Add (Vector(280.75061190252416,96.14224993965554, 0.0));
    positionAlloc4->Add (Vector(326.54060473088566,74.05215930761227, 0.0));
    positionAlloc4->Add (Vector(242.8376647889842,165.43456600046454, 0.0));
    positionAlloc4->Add (Vector(125.75064644562018,69.90635054252463, 0.0));
    positionAlloc4->Add (Vector(34.99288382143635,90.9238945138755, 0.0));
    positionAlloc4->Add (Vector(368.0826057222642,91.16011598052678, 0.0));
    positionAlloc4->Add (Vector(374.74735252363746,337.7319794262853, 0.0));
    positionAlloc4->Add (Vector(148.51750451162178,372.74444665461624, 0.0));
    positionAlloc4->Add (Vector(170.5899154005584,98.95841464095416, 0.0));
    positionAlloc4->Add (Vector(206.9070394004519,366.5623412215591, 0.0));
    positionAlloc4->Add (Vector(230.7023739948363,124.57768565298512, 0.0));
    positionAlloc4->Add (Vector(116.03874067497033,3.7394311866063124, 0.0));
    positionAlloc4->Add (Vector(181.14139998061498,251.32361500854284, 0.0));
    positionAlloc4->Add (Vector(223.40662036900713,276.09114726509915, 0.0));
    positionAlloc4->Add (Vector(130.3959247080101,343.79572834623434, 0.0));
    positionAlloc4->Add (Vector(10.914889406987527,208.8204989010043, 0.0));
    positionAlloc4->Add (Vector(286.7514654526757,356.40758171693557, 0.0));
    positionAlloc4->Add (Vector(375.55459094788534,12.379602508460286, 0.0));
    positionAlloc4->Add (Vector(173.4369309092658,204.91707691134457, 0.0));
    positionAlloc4->Add (Vector(364.309132469535,146.62265754049378, 0.0));
    positionAlloc4->Add (Vector(213.46133798848456,125.78680447433985, 0.0));
    positionAlloc4->Add (Vector(392.96697971717714,106.65548086065408, 0.0));
    positionAlloc4->Add (Vector(327.63567145341233,124.234900717684, 0.0));
    positionAlloc4->Add (Vector(58.71694936689727,105.71899309076028, 0.0));
    positionAlloc4->Add (Vector(47.7828201334916,325.04423521313146, 0.0));
    positionAlloc4->Add (Vector(99.9402720900147,69.37206831484914, 0.0));
    positionAlloc4->Add (Vector(363.7172818177018,160.34949294844836, 0.0));
    positionAlloc4->Add (Vector(284.4489835063264,292.7201636569181, 0.0));
    positionAlloc4->Add (Vector(362.3237984033207,395.98207134362946, 0.0));
    positionAlloc4->Add (Vector(75.0526691609736,248.99836091996846, 0.0));
    positionAlloc4->Add (Vector(58.223292463358824,63.092027948213314, 0.0));
    positionAlloc4->Add (Vector(318.38677677690777,366.109727965915, 0.0));
    positionAlloc4->Add (Vector(380.4775141960818,362.16081348111123, 0.0));
    positionAlloc4->Add (Vector(215.45091303053877,1.207182647838545, 0.0));
    positionAlloc4->Add (Vector(247.4054267648342,4.145704534092776, 0.0));
    positionAlloc4->Add (Vector(309.5497370881675,50.25655245509553, 0.0));
    positionAlloc4->Add (Vector(361.8744641374492,14.824419181614346, 0.0));
    positionAlloc4->Add (Vector(321.26533684566783,160.95700794309434, 0.0));
    positionAlloc4->Add (Vector(191.22900268713238,189.6015706248776, 0.0));
    positionAlloc4->Add (Vector(180.96193069858063,180.03469880030445, 0.0));
    positionAlloc4->Add (Vector(157.94019218425578,298.77539824301005, 0.0));
    positionAlloc4->Add (Vector(11.113339539467981,301.94459255618693, 0.0));
    positionAlloc4->Add (Vector(183.43352548337273,45.33676910703619, 0.0));
    positionAlloc4->Add (Vector(7.333057165202916,26.689999055742586, 0.0));
    positionAlloc4->Add (Vector(57.36921946809797,31.354759941979182, 0.0));
    positionAlloc4->Add (Vector(329.6039481649275,250.5984634497175, 0.0));
    positionAlloc4->Add (Vector(338.34059321107105,171.50750999977703, 0.0));
    positionAlloc4->Add (Vector(306.4845524303082,136.98164793740793, 0.0));
    positionAlloc4->Add (Vector(321.18529494949513,278.7201984480714, 0.0));
    positionAlloc4->Add (Vector(43.60750560762732,253.51470424476713, 0.0));
    positionAlloc4->Add (Vector(32.5895859509048,171.4475314655146, 0.0));
    positionAlloc4->Add (Vector(88.57309372939532,106.82880787374631, 0.0));
    positionAlloc4->Add (Vector(186.00059262540242,379.01957655803994, 0.0));
    positionAlloc4->Add (Vector(12.929226649429593,283.75490049251806, 0.0));
    positionAlloc4->Add (Vector(383.77372842470095,321.5843565994587, 0.0));
    positionAlloc4->Add (Vector(204.45659131885128,216.38072198487546, 0.0));
    positionAlloc4->Add (Vector(152.40644962488568,299.9433242904574, 0.0));
    positionAlloc4->Add (Vector(237.37297978584843,337.53617157247555, 0.0));
    positionAlloc4->Add (Vector(118.93255260290888,11.042404781801451, 0.0));
    positionAlloc4->Add (Vector(84.78406585747038,319.4416979040841, 0.0));
    positionAlloc4->Add (Vector(22.771775364437996,203.40649624409585, 0.0));
    positionAlloc4->Add (Vector(300.0689653191971,182.79013097427713, 0.0));
    positionAlloc4->Add (Vector(99.09316070494194,104.86923350916477, 0.0));
    positionAlloc4->Add (Vector(395.9006830654821,81.56249009324874, 0.0));
    positionAlloc4->Add (Vector(176.76171229936935,232.63096293771648, 0.0));
    positionAlloc4->Add (Vector(276.3768568264674,75.45472038212435, 0.0));
    positionAlloc4->Add (Vector(344.9385011352176,277.07108700294174, 0.0));
    positionAlloc4->Add (Vector(12.159875971515532,146.61723889749774, 0.0));
    positionAlloc4->Add (Vector(64.15807180785964,298.2548770829984, 0.0));
    positionAlloc4->Add (Vector(142.28206839355244,10.487426918549314, 0.0));
    positionAlloc4->Add (Vector(210.57037356220852,54.17116122886796, 0.0));
    positionAlloc4->Add (Vector(397.56520984419154,218.42832165718065, 0.0));
    positionAlloc4->Add (Vector(389.8961483420329,254.6417600007589, 0.0));
    positionAlloc4->Add (Vector(360.52337453970955,216.6237515549493, 0.0));
    positionAlloc4->Add (Vector(120.09933581356003,57.20233130323975, 0.0));
    positionAlloc4->Add (Vector(313.09976591366876,396.1887134495363, 0.0));
    positionAlloc4->Add (Vector(318.635471202915,178.4584928013829, 0.0));
    positionAlloc4->Add (Vector(252.92132697205113,138.11898463643263, 0.0));
    positionAlloc4->Add (Vector(103.87935731934124,102.82771952885868, 0.0));
    positionAlloc4->Add (Vector(308.09549382199197,292.69144029581076, 0.0));
    positionAlloc4->Add (Vector(319.8461046944112,13.8284879486537, 0.0));
    positionAlloc4->Add (Vector(281.4067952610612,244.09632505865187, 0.0));
    positionAlloc4->Add (Vector(39.49736072813934,260.5217329363964, 0.0));
    positionAlloc4->Add (Vector(330.56279905173653,341.6461775346155, 0.0));
    positionAlloc4->Add (Vector(346.6433096666052,379.9223965414506, 0.0));
    positionAlloc4->Add (Vector(337.88384304082786,215.37623703781748, 0.0));
    positionAlloc4->Add (Vector(129.7449838904746,349.16895056004444, 0.0));
    positionAlloc4->Add (Vector(314.11841128864756,8.932171196722472, 0.0));
    positionAlloc4->Add (Vector(88.81965591996908,207.7407297464132, 0.0));
    positionAlloc4->Add (Vector(57.81821604986392,300.61112685409756, 0.0));
    positionAlloc4->Add (Vector(217.60404752557525,261.6549387882977, 0.0));
    positionAlloc4->Add (Vector(61.068657637265304,248.7224926961662, 0.0));
    positionAlloc4->Add (Vector(206.34280678741192,190.85639481991294, 0.0));
    positionAlloc4->Add (Vector(93.46645569570002,40.78690377031897, 0.0));
    positionAlloc4->Add (Vector(166.33948730701093,246.67402686209448, 0.0));
    positionAlloc4->Add (Vector(99.2134233508923,340.6687499745468, 0.0));
    positionAlloc4->Add (Vector(291.43426718983847,83.2777753635166, 0.0));
    positionAlloc4->Add (Vector(77.18509280764513,199.83366827155442, 0.0));
    positionAlloc4->Add (Vector(102.84731751287848,44.35652029761168, 0.0));
    positionAlloc4->Add (Vector(366.5222213873403,363.85420998062284, 0.0));
    positionAlloc4->Add (Vector(237.0047475063187,124.90793508076679, 0.0));
    positionAlloc4->Add (Vector(138.068820462086,90.01598312563384, 0.0));
    positionAlloc4->Add (Vector(64.3885747262435,186.75200910532251, 0.0));
    positionAlloc4->Add (Vector(275.36530095437735,227.79776509815028, 0.0));
    positionAlloc4->Add (Vector(324.7434790882159,349.98465798235924, 0.0));
    positionAlloc4->Add (Vector(393.126845492178,72.6451405323055, 0.0));
    positionAlloc4->Add (Vector(151.16967173123993,31.850431079300947, 0.0));
    positionAlloc4->Add (Vector(137.0755631813127,27.283739366681647, 0.0));
    positionAlloc4->Add (Vector(227.54319628188622,186.28395143679234, 0.0));
    positionAlloc4->Add (Vector(207.60465098562233,308.3535642007552, 0.0));
    positionAlloc4->Add (Vector(228.99540687664253,293.25701011500445, 0.0));
    positionAlloc4->Add (Vector(211.08585852349867,320.8644336018392, 0.0));
    positionAlloc4->Add (Vector(100.9466806018359,341.959177078961, 0.0));
    positionAlloc4->Add (Vector(265.0879249501049,108.08330481190312, 0.0));
    positionAlloc4->Add (Vector(351.9993154253727,361.53678233054876, 0.0));
    positionAlloc4->Add (Vector(137.33849590976922,319.05552158342664, 0.0));
    positionAlloc4->Add (Vector(79.42795537084434,170.68040373921312, 0.0));
    positionAlloc4->Add (Vector(389.5208311709009,271.12035657372445, 0.0));
    positionAlloc4->Add (Vector(248.7677471681206,172.09890832850573, 0.0));
    positionAlloc4->Add (Vector(160.59183338781625,126.95357838331076, 0.0));
    positionAlloc4->Add (Vector(152.8411008126069,164.32453996887423, 0.0));
    positionAlloc4->Add (Vector(268.31551631503487,180.34957456533965, 0.0));
    positionAlloc4->Add (Vector(345.7917720218399,129.07239873469857, 0.0));
    positionAlloc4->Add (Vector(302.03287538707207,279.22289937892117, 0.0));
    positionAlloc4->Add (Vector(300.008705481064,343.3255345716169, 0.0));
    positionAlloc4->Add (Vector(114.23051267827758,234.73333626243317, 0.0));
    positionAlloc4->Add (Vector(229.88460188324092,251.23047932294017, 0.0));
    positionAlloc4->Add (Vector(241.8864403896212,331.5383231922772, 0.0));
    positionAlloc4->Add (Vector(150.6321258983098,389.51341534332903, 0.0));
    positionAlloc4->Add (Vector(0.16080995654297325,390.70365961242385, 0.0));
    positionAlloc4->Add (Vector(271.62753462617184,367.44071119101034, 0.0));
    positionAlloc4->Add (Vector(21.56370882950118,168.0774720004529, 0.0));
    positionAlloc4->Add (Vector(315.1716935302017,244.81247081797358, 0.0));
    positionAlloc4->Add (Vector(231.1428861138142,73.60408065098602, 0.0));
    positionAlloc4->Add (Vector(126.09792123814918,357.1554834100605, 0.0));
    positionAlloc4->Add (Vector(173.47053959578332,322.94421155393655, 0.0));
    positionAlloc4->Add (Vector(6.213310220334289,237.39256327865488, 0.0));
    positionAlloc4->Add (Vector(196.1014090479711,239.64412305835134, 0.0));
    positionAlloc4->Add (Vector(319.44143646081574,114.28754069365903, 0.0));
    positionAlloc4->Add (Vector(183.9521063267228,218.5387264081493, 0.0));
    positionAlloc4->Add (Vector(337.89377815688874,362.1569274834597, 0.0));
    positionAlloc4->Add (Vector(232.805671978832,351.5327937647376, 0.0));
    positionAlloc4->Add (Vector(137.8946610733174,376.04299293334685, 0.0));
    positionAlloc4->Add (Vector(52.398737924366756,323.7962768506328, 0.0));
    positionAlloc4->Add (Vector(70.4785022300221,132.82542974673478, 0.0));
    positionAlloc4->Add (Vector(330.846188468293,6.0075922968489515, 0.0));
    positionAlloc4->Add (Vector(8.609922109679191,379.1880844870724, 0.0));
    positionAlloc4->Add (Vector(54.03166321869253,202.2648662707587, 0.0));
    positionAlloc4->Add (Vector(69.58226667218574,50.53180777585493, 0.0));
    positionAlloc4->Add (Vector(157.95024516942195,385.3050113762847, 0.0));
    positionAlloc4->Add (Vector(370.4725706825815,367.4933742534424, 0.0));
    positionAlloc4->Add (Vector(241.7241931679893,219.81916860342503, 0.0));
    positionAlloc4->Add (Vector(156.00308565649848,194.3962668387639, 0.0));
    positionAlloc4->Add (Vector(366.2425399065098,256.626483578535, 0.0));
    positionAlloc4->Add (Vector(380.07044769883186,222.66127527806256, 0.0));
    positionAlloc4->Add (Vector(246.71134280066303,379.60652827504657, 0.0));
    positionAlloc4->Add (Vector(5.580629190238806,93.7448344485682, 0.0));
    positionAlloc4->Add (Vector(363.1263410014096,372.7888276787349, 0.0));
    positionAlloc4->Add (Vector(294.0263853154678,308.87121181729873, 0.0));
    positionAlloc4->Add (Vector(105.31870819484439,26.384436273609513, 0.0));
    positionAlloc4->Add (Vector(209.86812364949347,369.9231881597403, 0.0));
    positionAlloc4->Add (Vector(77.37371304933109,255.78435235197605, 0.0));
    positionAlloc4->Add (Vector(104.12603943141639,321.90182549733817, 0.0));
    positionAlloc4->Add (Vector(301.10222149552555,26.614592541645976, 0.0));
    positionAlloc4->Add (Vector(251.8870028086258,84.06960396593584, 0.0));
    positionAlloc4->Add (Vector(26.533933771376628,148.03367916564252, 0.0));
    positionAlloc4->Add (Vector(267.69315738127386,105.96782306512375, 0.0));
    positionAlloc4->Add (Vector(220.37928764715872,298.13377236260084, 0.0));
    positionAlloc4->Add (Vector(231.89808769831876,152.05646904942017, 0.0));
    positionAlloc4->Add (Vector(126.94496372886431,395.4464617649796, 0.0));
    positionAlloc4->Add (Vector(242.53178466133215,227.54057483459255, 0.0));
    positionAlloc4->Add (Vector(28.008857487688932,194.53804437481273, 0.0));
    positionAlloc4->Add (Vector(62.71655785843371,7.430480870963807, 0.0));
    positionAlloc4->Add (Vector(247.9822873525519,331.59235982007147, 0.0));
    positionAlloc4->Add (Vector(95.93910365903446,197.507885707492, 0.0));
    positionAlloc4->Add (Vector(388.00799563532496,338.7315205960141, 0.0));
    positionAlloc4->Add (Vector(78.17179244372751,232.5435709093031, 0.0));
    positionAlloc4->Add (Vector(100.93029782812937,297.5303416300372, 0.0));
    positionAlloc4->Add (Vector(227.24018476797684,81.31729386439619, 0.0));
    positionAlloc4->Add (Vector(28.78971187579471,386.9105320001088, 0.0));
    positionAlloc4->Add (Vector(223.88679282165697,5.0223920636463415, 0.0));
    positionAlloc4->Add (Vector(90.28373544314188,285.19559215307066, 0.0));
    positionAlloc4->Add (Vector(17.820751417904688,42.99765164243716, 0.0));
    positionAlloc4->Add (Vector(23.967075804884665,48.537382296294936, 0.0));
    positionAlloc4->Add (Vector(222.49609359616755,54.58209026427401, 0.0));
    positionAlloc4->Add (Vector(345.4167418237715,298.84865709487383, 0.0));
    positionAlloc4->Add (Vector(322.84207824751167,155.1442576256687, 0.0));
    positionAlloc4->Add (Vector(320.25306907224655,93.18970953640817, 0.0));
    positionAlloc4->Add (Vector(224.41208770228397,7.4589157491772085, 0.0));
    positionAlloc4->Add (Vector(234.30370858331514,387.83829932786983, 0.0));
    positionAlloc4->Add (Vector(49.66932604796446,111.6734716044558, 0.0));
    positionAlloc4->Add (Vector(221.12879147430635,336.8123569438423, 0.0));
    positionAlloc4->Add (Vector(344.0111794731552,215.53242573666114, 0.0));
    positionAlloc4->Add (Vector(11.322595208317843,98.48442704121835, 0.0));
    positionAlloc4->Add (Vector(7.952053535918235,10.484394751087711, 0.0));
    positionAlloc4->Add (Vector(284.6099034513887,49.70838478886588, 0.0));
    positionAlloc4->Add (Vector(301.55047538449855,369.20981421859335, 0.0));
    positionAlloc4->Add (Vector(26.400069088824996,302.18522104098656, 0.0));
    positionAlloc4->Add (Vector(373.03818521486545,278.7272645959601, 0.0));
    positionAlloc4->Add (Vector(68.93620333813142,54.854299851551104, 0.0));
    positionAlloc4->Add (Vector(276.3587670067696,398.9291401805922, 0.0));
    positionAlloc4->Add (Vector(6.328497138622513,371.77489349750454, 0.0));
    positionAlloc4->Add (Vector(363.41406036791966,249.3440463167211, 0.0));
    positionAlloc4->Add (Vector(354.3768397243098,142.9079040009999, 0.0));
    positionAlloc4->Add (Vector(130.65796070883846,210.8232409030437, 0.0));
    positionAlloc4->Add (Vector(1.148130812463588,246.85796544828955, 0.0));
    positionAlloc4->Add (Vector(361.3518082249015,229.47179466891436, 0.0));
    positionAlloc4->Add (Vector(163.25472110451247,94.81079209721108, 0.0));
    positionAlloc4->Add (Vector(179.96485339197622,231.3558457548527, 0.0));
    positionAlloc4->Add (Vector(45.898389181350076,379.7957034828285, 0.0));
    positionAlloc4->Add (Vector(265.3765991273792,248.6782880836487, 0.0));
    positionAlloc4->Add (Vector(171.23647594851798,385.93601885935425, 0.0));
    positionAlloc4->Add (Vector(107.97115670601043,358.35448727842675, 0.0));
    positionAlloc4->Add (Vector(300.3769736109349,139.559336791137, 0.0));
    positionAlloc4->Add (Vector(353.32243648232395,249.46888282224356, 0.0));
    positionAlloc4->Add (Vector(300.3248412544622,290.3991941401806, 0.0));
    positionAlloc4->Add (Vector(371.00343215841355,139.10634389820262, 0.0));
    positionAlloc4->Add (Vector(159.07073479421342,66.14167884677312, 0.0));
    positionAlloc4->Add (Vector(55.71053890030342,322.95651548380954, 0.0));
    positionAlloc4->Add (Vector(361.3607661151534,54.98988165849501, 0.0));
    positionAlloc4->Add (Vector(377.8379023963253,234.62201620079716, 0.0));
    positionAlloc4->Add (Vector(265.5178580879155,205.95564482332344, 0.0));
    positionAlloc4->Add (Vector(19.981383578434865,214.35856236620464, 0.0));
    positionAlloc4->Add (Vector(277.76006309109806,165.67170781076106, 0.0));
    positionAlloc4->Add (Vector(40.93377153113033,165.62239512782733, 0.0));
    positionAlloc4->Add (Vector(235.7222147613137,279.90334400837247, 0.0));
    positionAlloc4->Add (Vector(229.64704219680522,58.69142996232406, 0.0));
    positionAlloc4->Add (Vector(196.62926371213533,21.345018046832152, 0.0));
    positionAlloc4->Add (Vector(84.65124640002361,106.21866374889049, 0.0));
    positionAlloc4->Add (Vector(7.74678314811883,271.5342131759564, 0.0));
    positionAlloc4->Add (Vector(115.1101354345395,52.011428847311066, 0.0));
    positionAlloc4->Add (Vector(363.4382012372382,117.4456593494718, 0.0));
    positionAlloc4->Add (Vector(41.29040263105681,179.15741047036207, 0.0));
    positionAlloc4->Add (Vector(112.17759682576207,315.7117313805954, 0.0));
    positionAlloc4->Add (Vector(395.54443556259787,299.26626175193576, 0.0));
    positionAlloc4->Add (Vector(7.315310937676722,300.057725977987, 0.0));
    positionAlloc4->Add (Vector(274.6003710726335,333.85026875894914, 0.0));
    positionAlloc4->Add (Vector(276.7508455801893,126.20625240242518, 0.0));
    positionAlloc4->Add (Vector(383.1558120602008,213.26611398920681, 0.0));
    positionAlloc4->Add (Vector(39.33873353322004,168.44305000202087, 0.0));
    positionAlloc4->Add (Vector(67.93216782582756,351.2570013717652, 0.0));
    positionAlloc4->Add (Vector(34.017684547911166,15.621913293152945, 0.0));
    positionAlloc4->Add (Vector(350.5556609184153,357.8426654015389, 0.0));
    positionAlloc4->Add (Vector(125.36967126369714,276.92904626772565, 0.0));
    positionAlloc4->Add (Vector(320.29782747021466,387.304630287759, 0.0));
    positionAlloc4->Add (Vector(56.15477543809351,79.24059563395151, 0.0));
    positionAlloc4->Add (Vector(166.92192094685078,223.47593137830066, 0.0));
    positionAlloc4->Add (Vector(10.955037279170465,268.1870040713609, 0.0));
    positionAlloc4->Add (Vector(81.78089989260697,351.2469745563782, 0.0));
    positionAlloc4->Add (Vector(167.67780576131793,274.08780015870377, 0.0));
    positionAlloc4->Add (Vector(158.70698969226797,215.52669360134277, 0.0));
    positionAlloc4->Add (Vector(74.50408455106836,138.2242908172191, 0.0));
    positionAlloc4->Add (Vector(58.70235632684522,36.935437907519116, 0.0));
    positionAlloc4->Add (Vector(0.045749926937954655,120.9330290527359, 0.0));
    positionAlloc4->Add (Vector(166.8088018810296,288.1297973768632, 0.0));
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