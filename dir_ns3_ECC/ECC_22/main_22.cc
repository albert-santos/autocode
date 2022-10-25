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
	  uint16_t numberOfRrhs = 68;
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
    positionAlloc->Add (Vector (500.0,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (500.0,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (500.0,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (500.0,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,500.0, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,0.0, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,500.0, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,0.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,500.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,500.0, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,0.0, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,0.0, 0.0));
    positionAlloc->Add (Vector (0.0,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (0.0,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (0.0,111.11111111111111, 0.0));
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
    positionAlloc4->Add (Vector(106.79109609257736,250.8404517081776, 0.0));
    positionAlloc4->Add (Vector(361.3723600607782,305.8483613581191, 0.0));
    positionAlloc4->Add (Vector(256.7900256173353,402.70721534142183, 0.0));
    positionAlloc4->Add (Vector(378.43682392282875,44.8079882333442, 0.0));
    positionAlloc4->Add (Vector(323.4827994279573,359.5438518406504, 0.0));
    positionAlloc4->Add (Vector(208.96052541977284,69.22710379357805, 0.0));
    positionAlloc4->Add (Vector(72.85906921898932,223.55268086657694, 0.0));
    positionAlloc4->Add (Vector(300.7042540998241,266.9030359564756, 0.0));
    positionAlloc4->Add (Vector(61.83313573438326,81.60103250351192, 0.0));
    positionAlloc4->Add (Vector(176.56315611934386,348.8233061322668, 0.0));
    positionAlloc4->Add (Vector(305.0256756361734,430.29589342905496, 0.0));
    positionAlloc4->Add (Vector(353.9753919811712,52.707900791437446, 0.0));
    positionAlloc4->Add (Vector(20.417456592318807,176.90577446992594, 0.0));
    positionAlloc4->Add (Vector(335.79715045519816,254.68462030653956, 0.0));
    positionAlloc4->Add (Vector(74.2908857525737,147.92323923950823, 0.0));
    positionAlloc4->Add (Vector(278.1677571124419,354.97958848277983, 0.0));
    positionAlloc4->Add (Vector(315.90021468394264,56.63921724975557, 0.0));
    positionAlloc4->Add (Vector(417.75576243881954,320.4717475601262, 0.0));
    positionAlloc4->Add (Vector(414.04692849812665,324.89163500570106, 0.0));
    positionAlloc4->Add (Vector(5.063106365565284,127.191123136008, 0.0));
    positionAlloc4->Add (Vector(187.52251595057484,51.98911612382712, 0.0));
    positionAlloc4->Add (Vector(97.90660769250337,387.18823816990715, 0.0));
    positionAlloc4->Add (Vector(383.55622917926144,191.1268955583431, 0.0));
    positionAlloc4->Add (Vector(10.440236129495851,170.09088686343387, 0.0));
    positionAlloc4->Add (Vector(142.26240986943478,436.7873632923569, 0.0));
    positionAlloc4->Add (Vector(347.0271924057184,268.93768462210926, 0.0));
    positionAlloc4->Add (Vector(120.3374890957769,269.84400601182426, 0.0));
    positionAlloc4->Add (Vector(137.70799289825226,155.90488176713936, 0.0));
    positionAlloc4->Add (Vector(89.04401903374021,91.47171026704432, 0.0));
    positionAlloc4->Add (Vector(438.3598313031284,372.9403677495147, 0.0));
    positionAlloc4->Add (Vector(250.07913767370405,293.0670733417139, 0.0));
    positionAlloc4->Add (Vector(395.65778172552194,219.29058424604742, 0.0));
    positionAlloc4->Add (Vector(417.13546470539217,186.22168871963302, 0.0));
    positionAlloc4->Add (Vector(130.57607029449397,295.60287120415126, 0.0));
    positionAlloc4->Add (Vector(472.3932711117488,491.08053906328985, 0.0));
    positionAlloc4->Add (Vector(137.77813472748252,198.39916663867768, 0.0));
    positionAlloc4->Add (Vector(0.8046457632410742,7.40575588715936, 0.0));
    positionAlloc4->Add (Vector(36.504999245915606,413.1328934170856, 0.0));
    positionAlloc4->Add (Vector(83.7107191029794,405.4181190775172, 0.0));
    positionAlloc4->Add (Vector(474.32066454772206,196.3906487469718, 0.0));
    positionAlloc4->Add (Vector(488.1292866562427,135.4777881483568, 0.0));
    positionAlloc4->Add (Vector(99.23109478313819,174.8212825594076, 0.0));
    positionAlloc4->Add (Vector(343.7540103344591,69.61896207173446, 0.0));
    positionAlloc4->Add (Vector(407.3012432656343,33.152122213207235, 0.0));
    positionAlloc4->Add (Vector(283.53866046654764,225.53248608350867, 0.0));
    positionAlloc4->Add (Vector(259.75402614298,474.3183841636794, 0.0));
    positionAlloc4->Add (Vector(202.20576814410651,212.76899275123785, 0.0));
    positionAlloc4->Add (Vector(213.60356626708082,487.2550219050073, 0.0));
    positionAlloc4->Add (Vector(107.91070588187135,187.9722301416406, 0.0));
    positionAlloc4->Add (Vector(183.83866626428164,186.34454728135236, 0.0));
    positionAlloc4->Add (Vector(147.12241181695495,144.03812989374237, 0.0));
    positionAlloc4->Add (Vector(450.30185634535474,314.8083129223562, 0.0));
    positionAlloc4->Add (Vector(293.1906070111677,213.4024420075148, 0.0));
    positionAlloc4->Add (Vector(47.860823729157175,7.331274304280044, 0.0));
    positionAlloc4->Add (Vector(417.7917677339071,496.81879871495744, 0.0));
    positionAlloc4->Add (Vector(447.1757338276396,125.84314467262281, 0.0));
    positionAlloc4->Add (Vector(92.96317147209932,444.69027606679094, 0.0));
    positionAlloc4->Add (Vector(395.11616005045744,113.21232774420965, 0.0));
    positionAlloc4->Add (Vector(117.18420816563874,479.7531756978324, 0.0));
    positionAlloc4->Add (Vector(112.46413761138513,302.2401696840444, 0.0));
    positionAlloc4->Add (Vector(108.90279561807475,128.8456619300431, 0.0));
    positionAlloc4->Add (Vector(93.26922862882498,208.47524900933513, 0.0));
    positionAlloc4->Add (Vector(444.18009314207126,213.07574823500443, 0.0));
    positionAlloc4->Add (Vector(67.1049286727321,311.93204159375904, 0.0));
    positionAlloc4->Add (Vector(241.2943970202111,71.40554975016528, 0.0));
    positionAlloc4->Add (Vector(322.35336380794655,58.59516685435118, 0.0));
    positionAlloc4->Add (Vector(160.5913323859882,236.90598103091153, 0.0));
    positionAlloc4->Add (Vector(328.49528606079014,204.23443414490856, 0.0));
    positionAlloc4->Add (Vector(139.48736477913943,318.7700207369698, 0.0));
    positionAlloc4->Add (Vector(62.49930023678368,185.55102899202015, 0.0));
    positionAlloc4->Add (Vector(120.89179513305936,236.90444300671615, 0.0));
    positionAlloc4->Add (Vector(370.351123364474,158.9698309504659, 0.0));
    positionAlloc4->Add (Vector(470.63040757693864,174.5448466867287, 0.0));
    positionAlloc4->Add (Vector(134.30172539915463,10.412227932275364, 0.0));
    positionAlloc4->Add (Vector(239.55292465017396,138.1686113635986, 0.0));
    positionAlloc4->Add (Vector(292.635199002279,269.4315492843427, 0.0));
    positionAlloc4->Add (Vector(371.54005463389694,492.27889297763716, 0.0));
    positionAlloc4->Add (Vector(321.4643730199121,59.45875437199616, 0.0));
    positionAlloc4->Add (Vector(376.4645023970209,452.85759498558787, 0.0));
    positionAlloc4->Add (Vector(455.5154844713116,481.1516826685382, 0.0));
    positionAlloc4->Add (Vector(439.6396068199615,456.614205259839, 0.0));
    positionAlloc4->Add (Vector(120.00196042842171,90.03266211601208, 0.0));
    positionAlloc4->Add (Vector(77.80849831796255,117.85903483549343, 0.0));
    positionAlloc4->Add (Vector(228.4868963605952,491.00067545398485, 0.0));
    positionAlloc4->Add (Vector(471.6088813986978,177.22928506966718, 0.0));
    positionAlloc4->Add (Vector(117.5082015777244,345.07993339507794, 0.0));
    positionAlloc4->Add (Vector(281.44091539345106,172.59845656934704, 0.0));
    positionAlloc4->Add (Vector(387.9009182620514,101.44225251045597, 0.0));
    positionAlloc4->Add (Vector(43.43054924653161,486.85815268240805, 0.0));
    positionAlloc4->Add (Vector(187.13991794371358,203.8688336360218, 0.0));
    positionAlloc4->Add (Vector(249.65844200483133,52.9581197339829, 0.0));
    positionAlloc4->Add (Vector(20.93529910688474,341.9435706972587, 0.0));
    positionAlloc4->Add (Vector(437.371166237431,291.60382525711077, 0.0));
    positionAlloc4->Add (Vector(482.8976607502134,437.8165602337323, 0.0));
    positionAlloc4->Add (Vector(16.21892515480833,373.82888445321504, 0.0));
    positionAlloc4->Add (Vector(312.4101888014601,71.63424522112727, 0.0));
    positionAlloc4->Add (Vector(137.54139271312422,35.49129591066402, 0.0));
    positionAlloc4->Add (Vector(21.645731655666467,53.41931927443433, 0.0));
    positionAlloc4->Add (Vector(64.17622189594502,30.574125142342034, 0.0));
    positionAlloc4->Add (Vector(487.4895124000985,192.86027005347017, 0.0));
    positionAlloc4->Add (Vector(333.23252015305206,389.542659908102, 0.0));
    positionAlloc4->Add (Vector(241.489198004742,228.44714643521215, 0.0));
    positionAlloc4->Add (Vector(214.23006662577689,363.4616543994628, 0.0));
    positionAlloc4->Add (Vector(63.01213577325054,119.86436614706625, 0.0));
    positionAlloc4->Add (Vector(357.2535255393349,75.95899199312383, 0.0));
    positionAlloc4->Add (Vector(444.102904348641,86.08922493203869, 0.0));
    positionAlloc4->Add (Vector(58.376915110138484,148.25264477885776, 0.0));
    positionAlloc4->Add (Vector(276.40546871947475,441.4302590153904, 0.0));
    positionAlloc4->Add (Vector(330.044461006373,167.39746092325026, 0.0));
    positionAlloc4->Add (Vector(45.64112035007567,392.82130334116, 0.0));
    positionAlloc4->Add (Vector(22.322615697628056,65.70759666020997, 0.0));
    positionAlloc4->Add (Vector(97.7886622055475,192.16946694496457, 0.0));
    positionAlloc4->Add (Vector(476.3225729550882,150.2336027149275, 0.0));
    positionAlloc4->Add (Vector(401.77937069972313,473.7355905556707, 0.0));
    positionAlloc4->Add (Vector(262.4120677996832,388.2262260828067, 0.0));
    positionAlloc4->Add (Vector(80.13284357184192,140.90326874573745, 0.0));
    positionAlloc4->Add (Vector(213.0924074721856,446.361564632377, 0.0));
    positionAlloc4->Add (Vector(372.1291135688055,100.20085602440959, 0.0));
    positionAlloc4->Add (Vector(120.34191449742355,269.23709129415414, 0.0));
    positionAlloc4->Add (Vector(21.165443851525357,24.693478902555132, 0.0));
    positionAlloc4->Add (Vector(490.3927464644682,152.00162037077402, 0.0));
    positionAlloc4->Add (Vector(474.65890880093417,290.04524286113923, 0.0));
    positionAlloc4->Add (Vector(43.760235498638444,47.8607186426111, 0.0));
    positionAlloc4->Add (Vector(181.05966352366238,27.637403360533387, 0.0));
    positionAlloc4->Add (Vector(40.971981936414046,379.57356912585635, 0.0));
    positionAlloc4->Add (Vector(297.6603996312986,334.0809146757524, 0.0));
    positionAlloc4->Add (Vector(316.71585993354796,46.519154732346635, 0.0));
    positionAlloc4->Add (Vector(73.22852863435925,163.7289000266346, 0.0));
    positionAlloc4->Add (Vector(66.29795333123923,155.42249139996656, 0.0));
    positionAlloc4->Add (Vector(263.6700057794258,272.37695694282667, 0.0));
    positionAlloc4->Add (Vector(188.66657839573077,386.4520756340739, 0.0));
    positionAlloc4->Add (Vector(139.36392664978092,450.9252298855483, 0.0));
    positionAlloc4->Add (Vector(76.65407065154223,422.99343930359663, 0.0));
    positionAlloc4->Add (Vector(375.510880063192,211.12853458233434, 0.0));
    positionAlloc4->Add (Vector(229.0904645319763,34.49646430600822, 0.0));
    positionAlloc4->Add (Vector(273.4239821375605,471.4885635738639, 0.0));
    positionAlloc4->Add (Vector(341.5435206557968,302.602953762549, 0.0));
    positionAlloc4->Add (Vector(83.96242569819078,279.12938448938803, 0.0));
    positionAlloc4->Add (Vector(283.8010343034541,339.6813083953511, 0.0));
    positionAlloc4->Add (Vector(328.63980602569575,454.8405375194498, 0.0));
    positionAlloc4->Add (Vector(303.3887790734794,426.21437925475266, 0.0));
    positionAlloc4->Add (Vector(350.4851117719595,205.76040357123188, 0.0));
    positionAlloc4->Add (Vector(320.45273720123606,452.5035771828624, 0.0));
    positionAlloc4->Add (Vector(358.73076268179636,307.4390370561383, 0.0));
    positionAlloc4->Add (Vector(9.29473127768815,165.50839757880908, 0.0));
    positionAlloc4->Add (Vector(260.93548761018377,356.9383791027271, 0.0));
    positionAlloc4->Add (Vector(359.18012641584056,137.5280822988077, 0.0));
    positionAlloc4->Add (Vector(120.68138160488773,333.84262836702743, 0.0));
    positionAlloc4->Add (Vector(212.18901261683342,412.1236612778544, 0.0));
    positionAlloc4->Add (Vector(66.40441312035506,173.61612376171465, 0.0));
    positionAlloc4->Add (Vector(223.14224120460284,231.00171308755034, 0.0));
    positionAlloc4->Add (Vector(136.44890836528506,65.34713857727903, 0.0));
    positionAlloc4->Add (Vector(227.1046044152275,492.2518423229116, 0.0));
    positionAlloc4->Add (Vector(3.4333590072513354,206.88796660873905, 0.0));
    positionAlloc4->Add (Vector(245.53835010351304,111.20470259378185, 0.0));
    positionAlloc4->Add (Vector(381.46020749228023,86.77478213125606, 0.0));
    positionAlloc4->Add (Vector(26.504064988742748,473.3653082305904, 0.0));
    positionAlloc4->Add (Vector(101.67572790302532,365.27364653728495, 0.0));
    positionAlloc4->Add (Vector(237.40565621548083,345.6369247628105, 0.0));
    positionAlloc4->Add (Vector(263.6838143997189,82.78867423440678, 0.0));
    positionAlloc4->Add (Vector(146.86001184486196,165.867613650212, 0.0));
    positionAlloc4->Add (Vector(293.42717134879337,54.41701040246005, 0.0));
    positionAlloc4->Add (Vector(259.7804202817673,206.90123624266405, 0.0));
    positionAlloc4->Add (Vector(16.89487745944063,425.02094216171525, 0.0));
    positionAlloc4->Add (Vector(65.26342293731958,219.90641692786278, 0.0));
    positionAlloc4->Add (Vector(158.79680255120937,340.9987433846753, 0.0));
    positionAlloc4->Add (Vector(290.37569181805384,157.8397360452789, 0.0));
    positionAlloc4->Add (Vector(380.56529990003594,415.19449782637537, 0.0));
    positionAlloc4->Add (Vector(91.45600555276218,210.1610270201702, 0.0));
    positionAlloc4->Add (Vector(184.43958498671753,101.19079590583313, 0.0));
    positionAlloc4->Add (Vector(350.63594497875823,54.388157381379585, 0.0));
    positionAlloc4->Add (Vector(222.3977327385791,17.062730490999435, 0.0));
    positionAlloc4->Add (Vector(288.9571888906678,66.22124914519617, 0.0));
    positionAlloc4->Add (Vector(278.52455328505056,453.0482546995407, 0.0));
    positionAlloc4->Add (Vector(127.30548645638423,398.5142036538953, 0.0));
    positionAlloc4->Add (Vector(97.6499568082329,287.73728339937026, 0.0));
    positionAlloc4->Add (Vector(448.33251420307886,463.6147202191331, 0.0));
    positionAlloc4->Add (Vector(85.021689320742,180.49110411506308, 0.0));
    positionAlloc4->Add (Vector(246.4201942213249,199.98954084486954, 0.0));
    positionAlloc4->Add (Vector(416.39514637432785,499.3968125448227, 0.0));
    positionAlloc4->Add (Vector(439.44337820284363,473.76748204051444, 0.0));
    positionAlloc4->Add (Vector(402.90101815010433,270.79370373730086, 0.0));
    positionAlloc4->Add (Vector(348.2640551270009,149.82137092936497, 0.0));
    positionAlloc4->Add (Vector(226.6760989626833,22.594481576699298, 0.0));
    positionAlloc4->Add (Vector(26.173039775675868,87.47231462085014, 0.0));
    positionAlloc4->Add (Vector(363.48980016044516,250.72397994355916, 0.0));
    positionAlloc4->Add (Vector(257.6861728677701,412.472269356607, 0.0));
    positionAlloc4->Add (Vector(234.0581549166913,228.91545207564374, 0.0));
    positionAlloc4->Add (Vector(369.7920030718174,146.84302105064228, 0.0));
    positionAlloc4->Add (Vector(392.5183140346095,37.23110726787493, 0.0));
    positionAlloc4->Add (Vector(478.98674139610574,18.7258987461037, 0.0));
    positionAlloc4->Add (Vector(378.38392475304084,442.0215588830387, 0.0));
    positionAlloc4->Add (Vector(77.24816074620949,360.74988677701964, 0.0));
    positionAlloc4->Add (Vector(180.11058303391098,169.64246292408342, 0.0));
    positionAlloc4->Add (Vector(88.61182109392924,358.2647555004704, 0.0));
    positionAlloc4->Add (Vector(23.348103020450427,110.85883415679615, 0.0));
    positionAlloc4->Add (Vector(431.6060427590204,25.063148082111187, 0.0));
    positionAlloc4->Add (Vector(178.87127743416332,259.5927627071123, 0.0));
    positionAlloc4->Add (Vector(236.1826233332507,317.7604209533273, 0.0));
    positionAlloc4->Add (Vector(147.27097445187837,285.1216677106673, 0.0));
    positionAlloc4->Add (Vector(244.01151167652384,393.77818250955556, 0.0));
    positionAlloc4->Add (Vector(432.7217364592592,404.61042004078575, 0.0));
    positionAlloc4->Add (Vector(132.77808967536131,454.412756410442, 0.0));
    positionAlloc4->Add (Vector(370.75173724215676,467.06099914728065, 0.0));
    positionAlloc4->Add (Vector(65.11968686869079,268.7030427667205, 0.0));
    positionAlloc4->Add (Vector(363.2703392092435,280.68027143709986, 0.0));
    positionAlloc4->Add (Vector(60.7826871799172,160.64121439839496, 0.0));
    positionAlloc4->Add (Vector(374.0100340339118,181.92289489432477, 0.0));
    positionAlloc4->Add (Vector(122.03242936330611,125.04952664007679, 0.0));
    positionAlloc4->Add (Vector(257.9284595233972,130.96875108273454, 0.0));
    positionAlloc4->Add (Vector(415.2074097201996,48.58787447655932, 0.0));
    positionAlloc4->Add (Vector(96.53986563106653,53.723014265624144, 0.0));
    positionAlloc4->Add (Vector(301.67687412274626,129.02058613768637, 0.0));
    positionAlloc4->Add (Vector(99.09299632442375,301.94604798175715, 0.0));
    positionAlloc4->Add (Vector(355.5120862790952,325.8441279332283, 0.0));
    positionAlloc4->Add (Vector(213.35404665164916,178.21221519903497, 0.0));
    positionAlloc4->Add (Vector(497.1346433916319,294.07321545304177, 0.0));
    positionAlloc4->Add (Vector(186.75674220535282,485.13380225726866, 0.0));
    positionAlloc4->Add (Vector(474.8474324085744,36.16868859515665, 0.0));
    positionAlloc4->Add (Vector(221.47085733938914,184.46164152443512, 0.0));
    positionAlloc4->Add (Vector(458.67364562369136,227.41657945963811, 0.0));
    positionAlloc4->Add (Vector(379.2480154878065,290.5490902025867, 0.0));
    positionAlloc4->Add (Vector(132.7752518695618,85.64857626915007, 0.0));
    positionAlloc4->Add (Vector(456.24473058132855,115.85242229926735, 0.0));
    positionAlloc4->Add (Vector(421.2643650124869,465.16965892014156, 0.0));
    positionAlloc4->Add (Vector(375.3237704903113,255.31624305651545, 0.0));
    positionAlloc4->Add (Vector(245.2214282064953,130.5661027134196, 0.0));
    positionAlloc4->Add (Vector(354.0930730085835,369.6075862159176, 0.0));
    positionAlloc4->Add (Vector(429.1163885557579,303.12536757876416, 0.0));
    positionAlloc4->Add (Vector(119.8333170625051,492.88814404005336, 0.0));
    positionAlloc4->Add (Vector(138.74933694818887,130.58914731443667, 0.0));
    positionAlloc4->Add (Vector(301.0605181943095,412.5238969930713, 0.0));
    positionAlloc4->Add (Vector(246.18927366957922,75.39597463839553, 0.0));
    positionAlloc4->Add (Vector(142.236344184057,404.06212870465225, 0.0));
    positionAlloc4->Add (Vector(196.8523398611508,266.6594664243752, 0.0));
    positionAlloc4->Add (Vector(94.86922763017408,0.17495295642755426, 0.0));
    positionAlloc4->Add (Vector(62.5222712835547,2.521432511149224, 0.0));
    positionAlloc4->Add (Vector(272.81987312188625,127.64947579346581, 0.0));
    positionAlloc4->Add (Vector(135.72388417462,424.14229270048776, 0.0));
    positionAlloc4->Add (Vector(132.20529273696235,211.62628801968503, 0.0));
    positionAlloc4->Add (Vector(51.21557543016764,138.55540520355402, 0.0));
    positionAlloc4->Add (Vector(224.59422812787432,176.25056336123512, 0.0));
    positionAlloc4->Add (Vector(94.58697222662838,356.96600362190446, 0.0));
    positionAlloc4->Add (Vector(340.4696540354823,350.1605697120015, 0.0));
    positionAlloc4->Add (Vector(374.7723944279468,6.039471687554753, 0.0));
    positionAlloc4->Add (Vector(421.910380798745,70.77680366383838, 0.0));
    positionAlloc4->Add (Vector(136.44875167211123,212.69579059833237, 0.0));
    positionAlloc4->Add (Vector(181.54525933342364,62.782337782162855, 0.0));
    positionAlloc4->Add (Vector(113.06567850738836,165.65747475405368, 0.0));
    positionAlloc4->Add (Vector(34.88482602073434,361.1386971060025, 0.0));
    positionAlloc4->Add (Vector(187.69263478800647,424.12356987866974, 0.0));
    positionAlloc4->Add (Vector(382.89907374768484,278.26248459155244, 0.0));
    positionAlloc4->Add (Vector(480.8911740110081,275.7183150804328, 0.0));
    positionAlloc4->Add (Vector(354.9243712113633,40.55408161518426, 0.0));
    positionAlloc4->Add (Vector(484.0307346418345,313.11236015610416, 0.0));
    positionAlloc4->Add (Vector(113.14814930325096,413.1525655998083, 0.0));
    positionAlloc4->Add (Vector(63.41446567207337,24.321714697308483, 0.0));
    positionAlloc4->Add (Vector(459.2360479410614,66.66355890434367, 0.0));
    positionAlloc4->Add (Vector(450.2005449897489,262.2210896688157, 0.0));
    positionAlloc4->Add (Vector(333.86550570281787,284.56095138089535, 0.0));
    positionAlloc4->Add (Vector(179.0839752598165,127.01076472109717, 0.0));
    positionAlloc4->Add (Vector(290.52227321365353,373.06354382335707, 0.0));
    positionAlloc4->Add (Vector(420.5212890506184,1.9879885889817484, 0.0));
    positionAlloc4->Add (Vector(324.3406602954109,117.24730026352587, 0.0));
    positionAlloc4->Add (Vector(465.1845436349786,288.4671603648658, 0.0));
    positionAlloc4->Add (Vector(348.53601416947185,25.69759255328774, 0.0));
    positionAlloc4->Add (Vector(53.06275933531423,139.5372817650053, 0.0));
    positionAlloc4->Add (Vector(64.28739985735443,370.5837706831704, 0.0));
    positionAlloc4->Add (Vector(465.82092573145803,296.61532372278384, 0.0));
    positionAlloc4->Add (Vector(222.5280648919446,243.28511405873655, 0.0));
    positionAlloc4->Add (Vector(38.0365185462897,86.23686693964333, 0.0));
    positionAlloc4->Add (Vector(350.7158759666698,112.99265204160253, 0.0));
    positionAlloc4->Add (Vector(116.13585613535093,296.9216211088015, 0.0));
    positionAlloc4->Add (Vector(95.73495744548177,442.374736519838, 0.0));
    positionAlloc4->Add (Vector(393.5202044803969,202.13683897850788, 0.0));
    positionAlloc4->Add (Vector(331.4513952773605,317.6535584286793, 0.0));
    positionAlloc4->Add (Vector(31.453171580445584,227.59536382226457, 0.0));
    positionAlloc4->Add (Vector(239.54979425315187,282.22928348571776, 0.0));
    positionAlloc4->Add (Vector(484.1330898763479,134.16780299764835, 0.0));
    positionAlloc4->Add (Vector(270.870362865542,142.22310662836713, 0.0));
    positionAlloc4->Add (Vector(477.4640421566547,100.68252151965939, 0.0));
    positionAlloc4->Add (Vector(458.3953542043029,288.0449449802009, 0.0));
    positionAlloc4->Add (Vector(268.13387786592995,429.54123357051213, 0.0));
    positionAlloc4->Add (Vector(395.1734802902966,152.4353097584978, 0.0));
    positionAlloc4->Add (Vector(304.55532340718094,481.9663579274885, 0.0));
    positionAlloc4->Add (Vector(404.8090791988395,499.74511862695374, 0.0));
    positionAlloc4->Add (Vector(114.95028907933019,29.359905413450626, 0.0));
    positionAlloc4->Add (Vector(130.17398641643769,82.45896724921087, 0.0));
    positionAlloc4->Add (Vector(331.4085498179314,319.3056386255791, 0.0));
    positionAlloc4->Add (Vector(142.30009675563898,173.0271704105508, 0.0));
    positionAlloc4->Add (Vector(325.4150626074203,322.4713143328248, 0.0));
    positionAlloc4->Add (Vector(57.93995962945936,159.41234573060527, 0.0));
    positionAlloc4->Add (Vector(192.84267300836504,19.71723198957531, 0.0));
    positionAlloc4->Add (Vector(187.19461726348663,48.89816244438, 0.0));
    positionAlloc4->Add (Vector(46.60940775539213,219.55775287293406, 0.0));
    positionAlloc4->Add (Vector(213.04441429818255,245.20662522626458, 0.0));
    positionAlloc4->Add (Vector(28.453424554460927,164.53255416403533, 0.0));
    positionAlloc4->Add (Vector(259.531372345154,331.2566029372275, 0.0));
    positionAlloc4->Add (Vector(118.85928874366964,5.7802246864255835, 0.0));
    positionAlloc4->Add (Vector(411.4435535520699,343.3036237098676, 0.0));
    positionAlloc4->Add (Vector(45.20109269576206,82.57352392920991, 0.0));
    positionAlloc4->Add (Vector(189.45613493213887,473.0642891207637, 0.0));
    positionAlloc4->Add (Vector(428.55589339093274,394.010488528554, 0.0));
    positionAlloc4->Add (Vector(16.816064408744612,164.21591023366088, 0.0));
    positionAlloc4->Add (Vector(497.57330079580504,318.5130609872388, 0.0));
    positionAlloc4->Add (Vector(315.80836534438856,442.1873355150434, 0.0));
    positionAlloc4->Add (Vector(392.13392302682246,118.18006365459077, 0.0));
    positionAlloc4->Add (Vector(47.29297839861374,141.3400877785982, 0.0));
    positionAlloc4->Add (Vector(354.1208707313071,301.5363399168762, 0.0));
    positionAlloc4->Add (Vector(102.25451483258053,144.1069718545595, 0.0));
    positionAlloc4->Add (Vector(480.854929813134,300.6458667631826, 0.0));
    positionAlloc4->Add (Vector(402.3711359590431,317.01873823302265, 0.0));
    positionAlloc4->Add (Vector(225.86009671060748,275.304214962658, 0.0));
    positionAlloc4->Add (Vector(279.18763794070196,398.97365411994787, 0.0));
    positionAlloc4->Add (Vector(93.29177319489979,329.51812047480894, 0.0));
    positionAlloc4->Add (Vector(233.29278329813553,268.4110931821133, 0.0));
    positionAlloc4->Add (Vector(189.94478684490718,229.45809883957807, 0.0));
    positionAlloc4->Add (Vector(347.15520415003505,365.2671603041337, 0.0));
    positionAlloc4->Add (Vector(452.98371626885245,402.59935077181615, 0.0));
    positionAlloc4->Add (Vector(245.27793247406666,286.777955927538, 0.0));
    positionAlloc4->Add (Vector(341.17123057534513,129.49942506127883, 0.0));
    positionAlloc4->Add (Vector(78.4695483636736,12.172077370728285, 0.0));
    positionAlloc4->Add (Vector(221.11860464609757,436.5506533527636, 0.0));
    positionAlloc4->Add (Vector(442.62455288933114,395.89357104854884, 0.0));
    positionAlloc4->Add (Vector(180.3444799786999,210.53748064566753, 0.0));
    positionAlloc4->Add (Vector(83.60827832995454,162.03150709761005, 0.0));
    positionAlloc4->Add (Vector(304.1715886215698,184.87440561695618, 0.0));
    positionAlloc4->Add (Vector(208.52477313024116,82.12801547785209, 0.0));
    positionAlloc4->Add (Vector(386.7681244786737,181.2783177882951, 0.0));
    positionAlloc4->Add (Vector(10.853626018355822,207.65346549298653, 0.0));
    positionAlloc4->Add (Vector(403.17696474046204,345.0674398309, 0.0));
    positionAlloc4->Add (Vector(252.80251119541413,73.14021932657904, 0.0));
    positionAlloc4->Add (Vector(474.5607669486548,433.8072178293071, 0.0));
    positionAlloc4->Add (Vector(70.12820198823039,436.77883755567507, 0.0));
    positionAlloc4->Add (Vector(354.93632653216895,458.4629350589112, 0.0));
    positionAlloc4->Add (Vector(232.19150076228217,41.55173316760258, 0.0));
    positionAlloc4->Add (Vector(402.28244802758564,223.74910534134818, 0.0));
    positionAlloc4->Add (Vector(426.8447913724811,447.8710190474046, 0.0));
    positionAlloc4->Add (Vector(29.25706102251602,28.580385547855634, 0.0));
    positionAlloc4->Add (Vector(320.1865010644291,44.17593449832097, 0.0));
    positionAlloc4->Add (Vector(216.4761691950692,36.4846215454746, 0.0));
    positionAlloc4->Add (Vector(281.30988429669753,399.5932626474624, 0.0));
    positionAlloc4->Add (Vector(308.64796432801876,471.2517041989347, 0.0));
    positionAlloc4->Add (Vector(103.9068816310475,265.3919326612802, 0.0));
    positionAlloc4->Add (Vector(215.73286131047576,104.07927118135623, 0.0));
    positionAlloc4->Add (Vector(142.37098544881184,433.24325531939735, 0.0));
    positionAlloc4->Add (Vector(149.6603010359102,356.240873254804, 0.0));
    positionAlloc4->Add (Vector(493.24622326721243,217.45245432635446, 0.0));
    positionAlloc4->Add (Vector(351.1023785664331,127.83579883692065, 0.0));
    positionAlloc4->Add (Vector(242.00186761549213,435.9192643789656, 0.0));
    positionAlloc4->Add (Vector(148.369761915651,444.73803425653614, 0.0));
    positionAlloc4->Add (Vector(110.97905784384032,277.05239693133564, 0.0));
    positionAlloc4->Add (Vector(482.54679462716297,130.86524446637532, 0.0));
    positionAlloc4->Add (Vector(158.57511440938964,442.6519375357984, 0.0));
    positionAlloc4->Add (Vector(241.80507187975354,217.87954363703054, 0.0));
    positionAlloc4->Add (Vector(82.47151058769781,62.85025782728715, 0.0));
    positionAlloc4->Add (Vector(155.16893135822107,473.7450318988142, 0.0));
    positionAlloc4->Add (Vector(306.7923023140501,285.0210876883798, 0.0));
    positionAlloc4->Add (Vector(36.899581915265486,435.8983004166559, 0.0));
    positionAlloc4->Add (Vector(16.79276549284786,154.8532386609372, 0.0));
    positionAlloc4->Add (Vector(99.1780650687541,365.1874871224964, 0.0));
    positionAlloc4->Add (Vector(245.19113994345042,107.76299174508169, 0.0));
    positionAlloc4->Add (Vector(417.92297902261083,211.5992721583383, 0.0));
    positionAlloc4->Add (Vector(449.88478744473423,6.119611508466849, 0.0));
    positionAlloc4->Add (Vector(270.5418734823494,254.41445426619458, 0.0));
    positionAlloc4->Add (Vector(229.67459983197918,273.84298758781364, 0.0));
    positionAlloc4->Add (Vector(287.982249162826,269.5245634312628, 0.0));
    positionAlloc4->Add (Vector(360.7657038264302,474.70449979565046, 0.0));
    positionAlloc4->Add (Vector(167.22482371266582,287.2115355665073, 0.0));
    positionAlloc4->Add (Vector(499.95004807310835,330.2449066200243, 0.0));
    positionAlloc4->Add (Vector(360.6583069657896,404.10145024483086, 0.0));
    positionAlloc4->Add (Vector(218.92226715971884,366.52212166874705, 0.0));
    positionAlloc4->Add (Vector(266.64480360144125,219.79432274053434, 0.0));
    positionAlloc4->Add (Vector(243.94810858845233,22.827209382884238, 0.0));
    positionAlloc4->Add (Vector(307.2662851511635,256.4159763160224, 0.0));
    positionAlloc4->Add (Vector(74.32553553639737,384.94377118058276, 0.0));
    positionAlloc4->Add (Vector(424.2538841963147,248.5928450443758, 0.0));
    positionAlloc4->Add (Vector(23.411948747450673,55.69125873295266, 0.0));
    positionAlloc4->Add (Vector(340.4961242210767,394.60862362469425, 0.0));
    positionAlloc4->Add (Vector(410.0954443984683,342.0543813369196, 0.0));
    positionAlloc4->Add (Vector(465.86620845551045,54.35331059138343, 0.0));
    positionAlloc4->Add (Vector(486.99935389370916,380.7885621641937, 0.0));
    positionAlloc4->Add (Vector(451.66594286140713,274.42710492850534, 0.0));
    positionAlloc4->Add (Vector(31.421554523702877,458.0796421107044, 0.0));
    positionAlloc4->Add (Vector(208.71303752988524,68.91489105738768, 0.0));
    positionAlloc4->Add (Vector(403.11872706298516,243.18918587537357, 0.0));
    positionAlloc4->Add (Vector(167.8043181535302,134.78810447409555, 0.0));
    positionAlloc4->Add (Vector(365.98497775084843,124.7491032140185, 0.0));
    positionAlloc4->Add (Vector(373.4318035214071,432.9876004846791, 0.0));
    positionAlloc4->Add (Vector(432.6625513034228,422.2961314086736, 0.0));
    positionAlloc4->Add (Vector(455.0901662093833,88.28808208594808, 0.0));
    positionAlloc4->Add (Vector(232.75969595890805,347.36027507920346, 0.0));
    positionAlloc4->Add (Vector(156.84772596599777,224.9861390573208, 0.0));
    positionAlloc4->Add (Vector(405.65798254185603,326.4782834715595, 0.0));
    positionAlloc4->Add (Vector(492.1139806360794,77.12099296844855, 0.0));
    positionAlloc4->Add (Vector(135.78937343313035,336.884332085397, 0.0));
    positionAlloc4->Add (Vector(361.35970329706157,434.5787410380614, 0.0));
    positionAlloc4->Add (Vector(331.1470393449491,3.5189095345634636, 0.0));
    positionAlloc4->Add (Vector(35.888598285704454,226.02201130496192, 0.0));
    positionAlloc4->Add (Vector(173.57167339656132,54.65533194933636, 0.0));
    positionAlloc4->Add (Vector(393.00758369869294,384.23138683455005, 0.0));
    positionAlloc4->Add (Vector(55.05737157225721,201.24466265258783, 0.0));
    positionAlloc4->Add (Vector(199.31301914264253,269.40155001825565, 0.0));
    positionAlloc4->Add (Vector(145.76857728391246,33.62049899009023, 0.0));
    positionAlloc4->Add (Vector(412.74994083121754,46.810398681211836, 0.0));
    positionAlloc4->Add (Vector(469.4851954704103,142.58741490006128, 0.0));
    positionAlloc4->Add (Vector(151.94603943513346,191.306654879201, 0.0));
    positionAlloc4->Add (Vector(410.9874539916606,396.0422877037189, 0.0));
    positionAlloc4->Add (Vector(81.04516079947332,315.5289774501336, 0.0));
    positionAlloc4->Add (Vector(463.1141687175558,249.95266097336244, 0.0));
    positionAlloc4->Add (Vector(338.15234015099196,464.2186678181149, 0.0));
    positionAlloc4->Add (Vector(172.45552102275346,258.79877431128705, 0.0));
    positionAlloc4->Add (Vector(113.36840101310763,498.45688639822487, 0.0));
    positionAlloc4->Add (Vector(309.6587313750901,218.41578672352225, 0.0));
    positionAlloc4->Add (Vector(346.73674325905966,415.20619356739587, 0.0));
    positionAlloc4->Add (Vector(0.7954652197036394,329.03100912564713, 0.0));
    positionAlloc4->Add (Vector(226.49801405968506,19.528952394936073, 0.0));
    positionAlloc4->Add (Vector(497.6624033435172,399.75771671936315, 0.0));
    positionAlloc4->Add (Vector(404.43954510978773,344.74725980345454, 0.0));
    positionAlloc4->Add (Vector(86.63161529100033,81.90421083556598, 0.0));
    positionAlloc4->Add (Vector(484.1580244190294,35.9144693284893, 0.0));
    positionAlloc4->Add (Vector(415.4716451978026,218.13256868778342, 0.0));
    positionAlloc4->Add (Vector(57.22127265189631,261.03497306384395, 0.0));
    positionAlloc4->Add (Vector(204.20696411819895,2.4711656926337255, 0.0));
    positionAlloc4->Add (Vector(199.9731312217462,462.85930782480017, 0.0));
    positionAlloc4->Add (Vector(34.688241877685435,146.57296152836435, 0.0));
    positionAlloc4->Add (Vector(94.90125799103694,197.3930505350716, 0.0));
    positionAlloc4->Add (Vector(48.62721330423941,243.11290113828287, 0.0));
    positionAlloc4->Add (Vector(311.6419683802658,446.21222582780706, 0.0));
    positionAlloc4->Add (Vector(239.85143331679825,424.5047647943038, 0.0));
    positionAlloc4->Add (Vector(142.70304553385137,487.4877215844639, 0.0));
    positionAlloc4->Add (Vector(54.909085620179596,206.33441554485472, 0.0));
    positionAlloc4->Add (Vector(380.5311262502015,322.51916475098545, 0.0));
    positionAlloc4->Add (Vector(26.352977742676753,364.4458889694779, 0.0));
    positionAlloc4->Add (Vector(496.95169493038395,36.18851874128281, 0.0));
    positionAlloc4->Add (Vector(410.2999508609595,498.62225913515994, 0.0));
    positionAlloc4->Add (Vector(378.3885455143802,36.028835184365114, 0.0));
    positionAlloc4->Add (Vector(253.69064812323515,243.48769267322547, 0.0));
    positionAlloc4->Add (Vector(338.253787259086,314.944877789726, 0.0));
    positionAlloc4->Add (Vector(189.5995621939765,198.7879283786006, 0.0));
    positionAlloc4->Add (Vector(260.5935502439099,81.86579621711704, 0.0));
    positionAlloc4->Add (Vector(474.3208920841688,232.69433166799746, 0.0));
    positionAlloc4->Add (Vector(132.57794797168555,432.8200094354042, 0.0));
    positionAlloc4->Add (Vector(78.75463930799448,159.57471750151313, 0.0));
    positionAlloc4->Add (Vector(447.09246405806607,133.34733069906656, 0.0));
    positionAlloc4->Add (Vector(255.95439337207392,150.98158555616592, 0.0));
    positionAlloc4->Add (Vector(116.04805326202106,223.4606001759925, 0.0));
    positionAlloc4->Add (Vector(24.300238408708317,337.7397408448211, 0.0));
    positionAlloc4->Add (Vector(381.7246439235018,67.46604347352842, 0.0));
    positionAlloc4->Add (Vector(204.23726515175827,52.40693742478886, 0.0));
    positionAlloc4->Add (Vector(8.674749318904663,450.9885718939294, 0.0));
    positionAlloc4->Add (Vector(320.6989273041068,104.31071008634673, 0.0));
    positionAlloc4->Add (Vector(496.33536217667523,335.289266735518, 0.0));
    positionAlloc4->Add (Vector(86.8753854383278,199.3525865150007, 0.0));
    positionAlloc4->Add (Vector(428.08800368361267,476.4772238498029, 0.0));
    positionAlloc4->Add (Vector(131.20335155334527,256.28805535835403, 0.0));
    positionAlloc4->Add (Vector(154.69705042633115,426.74832196854595, 0.0));
    positionAlloc4->Add (Vector(420.1393087021702,16.16654681032076, 0.0));
    positionAlloc4->Add (Vector(85.27359132687207,3.5195165105472714, 0.0));
    positionAlloc4->Add (Vector(446.55151123279404,272.6890769056329, 0.0));
    positionAlloc4->Add (Vector(496.715422286693,482.469098391034, 0.0));
    positionAlloc4->Add (Vector(163.62211735349013,267.37360989958205, 0.0));
    positionAlloc4->Add (Vector(6.6568293099334985,282.2321660728283, 0.0));
    positionAlloc4->Add (Vector(266.31828433457497,299.8898697640614, 0.0));
    positionAlloc4->Add (Vector(133.60272293663772,336.4944293915193, 0.0));
    positionAlloc4->Add (Vector(465.11197540122197,156.72658062579342, 0.0));
    positionAlloc4->Add (Vector(169.833870475236,337.72074181527495, 0.0));
    positionAlloc4->Add (Vector(121.52628782794712,485.06015731769793, 0.0));
    positionAlloc4->Add (Vector(47.04501628280627,230.27083991646984, 0.0));
    positionAlloc4->Add (Vector(289.1653336375087,297.2701764747151, 0.0));
    positionAlloc4->Add (Vector(343.86100034468575,394.7545827531966, 0.0));
    positionAlloc4->Add (Vector(183.3873115804157,190.0068643181757, 0.0));
    positionAlloc4->Add (Vector(264.7876680628191,224.9167537736509, 0.0));
    positionAlloc4->Add (Vector(369.84754412581356,355.74899420161546, 0.0));
    positionAlloc4->Add (Vector(92.0353472828685,424.9996592639621, 0.0));
    positionAlloc4->Add (Vector(135.15599420071766,231.50005318461393, 0.0));
    positionAlloc4->Add (Vector(166.27158270042597,495.378801349582, 0.0));
    positionAlloc4->Add (Vector(408.5442497624218,252.02795435770236, 0.0));
    positionAlloc4->Add (Vector(280.01189568925713,289.4090045640734, 0.0));
    positionAlloc4->Add (Vector(392.7237205708488,159.7785311830675, 0.0));
    positionAlloc4->Add (Vector(491.4599342795121,76.09436102772726, 0.0));
    positionAlloc4->Add (Vector(403.6181928635274,409.07297003684477, 0.0));
    positionAlloc4->Add (Vector(103.51188325702931,254.00604318142234, 0.0));
    positionAlloc4->Add (Vector(316.9211012618496,79.47534091991898, 0.0));
    positionAlloc4->Add (Vector(80.07921805915986,21.35506419930444, 0.0));
    positionAlloc4->Add (Vector(471.5280022079804,182.12090422119726, 0.0));
    positionAlloc4->Add (Vector(29.62884354629658,100.0846116516923, 0.0));
    positionAlloc4->Add (Vector(48.83884099078356,78.62544566083368, 0.0));
    positionAlloc4->Add (Vector(128.46946321763258,488.65467591998697, 0.0));
    positionAlloc4->Add (Vector(327.27104267987073,132.79955780508018, 0.0));
    positionAlloc4->Add (Vector(67.51605947836397,458.5399469885773, 0.0));
    positionAlloc4->Add (Vector(101.16441413537642,38.88364235680341, 0.0));
    positionAlloc4->Add (Vector(382.1755077275513,472.6664567603186, 0.0));
    positionAlloc4->Add (Vector(264.83997759882936,158.9752850712081, 0.0));
    positionAlloc4->Add (Vector(1.4186488705815181,92.62364591713384, 0.0));
    positionAlloc4->Add (Vector(207.89578603041937,54.325151640944256, 0.0));
    positionAlloc4->Add (Vector(456.91251286125896,54.434936745748786, 0.0));
    positionAlloc4->Add (Vector(152.4953144309665,355.87902595096364, 0.0));
    positionAlloc4->Add (Vector(63.092256107423026,65.86047180299393, 0.0));
    positionAlloc4->Add (Vector(120.99076199625091,59.16392217311828, 0.0));
    positionAlloc4->Add (Vector(462.7201970963709,126.78902416980847, 0.0));
    positionAlloc4->Add (Vector(382.71983133828826,214.31082579599743, 0.0));
    positionAlloc4->Add (Vector(402.9697129774497,296.8744113869306, 0.0));
    positionAlloc4->Add (Vector(94.69188375728399,353.16226448582324, 0.0));
    positionAlloc4->Add (Vector(50.29588962574283,365.1052234529776, 0.0));
    positionAlloc4->Add (Vector(9.483457440191634,270.1677429235514, 0.0));
    positionAlloc4->Add (Vector(351.48140202025917,370.1043693013823, 0.0));
    positionAlloc4->Add (Vector(86.94910070369698,365.8720886260004, 0.0));
    positionAlloc4->Add (Vector(94.17902670835981,441.7317551674825, 0.0));
    positionAlloc4->Add (Vector(252.02600842314354,238.1935932237798, 0.0));
    positionAlloc4->Add (Vector(447.2556151574396,491.25664285411773, 0.0));
    positionAlloc4->Add (Vector(58.539395893881405,397.17059574935786, 0.0));
    positionAlloc4->Add (Vector(261.6680002030809,372.037759376593, 0.0));
    positionAlloc4->Add (Vector(17.168344926953573,264.86337255499745, 0.0));
    positionAlloc4->Add (Vector(99.60345286636823,387.0738528449557, 0.0));
    positionAlloc4->Add (Vector(197.11136336536006,473.1641792429081, 0.0));
    positionAlloc4->Add (Vector(422.46084555280726,125.70887842288059, 0.0));
    positionAlloc4->Add (Vector(321.29168096966487,347.4437012336845, 0.0));
    positionAlloc4->Add (Vector(173.4354732399369,9.744375738368948, 0.0));
    positionAlloc4->Add (Vector(101.59568395195973,204.7018824503728, 0.0));
    positionAlloc4->Add (Vector(112.71304094022838,367.6742002665841, 0.0));
    positionAlloc4->Add (Vector(131.0138175865353,258.5497441013118, 0.0));
    positionAlloc4->Add (Vector(344.4606710084123,378.02753790909384, 0.0));
    positionAlloc4->Add (Vector(245.5242582578403,367.7421318697078, 0.0));
    positionAlloc4->Add (Vector(169.35307798561888,487.3681444154002, 0.0));
    positionAlloc4->Add (Vector(14.266093990042972,294.42297420856596, 0.0));
    positionAlloc4->Add (Vector(412.2066944152359,395.4485153485984, 0.0));
    positionAlloc4->Add (Vector(329.45030085955136,408.63002680732467, 0.0));
    positionAlloc4->Add (Vector(260.52204200576176,97.36108066476106, 0.0));
    positionAlloc4->Add (Vector(421.39475973466,95.86185397804003, 0.0));
    positionAlloc4->Add (Vector(453.9878243484983,103.87716236465833, 0.0));
    positionAlloc4->Add (Vector(57.36977600392129,116.6578854890472, 0.0));
    positionAlloc4->Add (Vector(480.49656895578596,258.8025646570954, 0.0));
    positionAlloc4->Add (Vector(376.94458950589296,299.687068232296, 0.0));
    positionAlloc4->Add (Vector(384.34723944743456,334.66293458183463, 0.0));
    positionAlloc4->Add (Vector(182.61277607485977,414.37982635663985, 0.0));
    positionAlloc4->Add (Vector(446.0292460552259,345.628245500091, 0.0));
    positionAlloc4->Add (Vector(447.8107163457962,102.98844896431547, 0.0));
    positionAlloc4->Add (Vector(295.7052880730029,233.61792701744193, 0.0));
    positionAlloc4->Add (Vector(43.79771012713229,20.968199572391434, 0.0));
    positionAlloc4->Add (Vector(200.61722852755432,435.4209183411535, 0.0));
    positionAlloc4->Add (Vector(0.4839718688732786,305.8491395443803, 0.0));
    positionAlloc4->Add (Vector(211.40921299253895,393.76112733409974, 0.0));
    positionAlloc4->Add (Vector(400.83459791770804,112.9847958838786, 0.0));
    positionAlloc4->Add (Vector(397.52313264446764,462.3279371327824, 0.0));
    positionAlloc4->Add (Vector(406.26550978763305,434.5567813239256, 0.0));
    positionAlloc4->Add (Vector(67.00895010649872,334.75226731595535, 0.0));
    positionAlloc4->Add (Vector(470.528962693961,216.15617880032235, 0.0));
    positionAlloc4->Add (Vector(363.59640635316487,117.94907699331515, 0.0));
    positionAlloc4->Add (Vector(334.64461186342083,446.7632683153497, 0.0));
    positionAlloc4->Add (Vector(328.991208691316,478.9667320567017, 0.0));
    positionAlloc4->Add (Vector(260.93180153039174,197.88508311910462, 0.0));
    positionAlloc4->Add (Vector(352.2808713562124,241.4646922202892, 0.0));
    positionAlloc4->Add (Vector(286.74005410732605,324.445523451758, 0.0));
    positionAlloc4->Add (Vector(186.10454398437244,130.66502748019215, 0.0));
    positionAlloc4->Add (Vector(85.68810436764335,275.20839799835807, 0.0));
    positionAlloc4->Add (Vector(451.60720183109567,290.71463142463955, 0.0));
    positionAlloc4->Add (Vector(367.70599638600595,408.6375610720706, 0.0));
    positionAlloc4->Add (Vector(169.5215291777572,235.6792151627793, 0.0));
    positionAlloc4->Add (Vector(46.12645495519163,139.3484362238642, 0.0));
    positionAlloc4->Add (Vector(431.3779498811419,244.9193288318716, 0.0));
    positionAlloc4->Add (Vector(375.9743954754177,374.31765458768905, 0.0));
    positionAlloc4->Add (Vector(336.75673298743135,451.4163693298864, 0.0));
    positionAlloc4->Add (Vector(270.25468697689837,407.8287800477072, 0.0));
    positionAlloc4->Add (Vector(491.9440335951515,469.34888449981696, 0.0));
    positionAlloc4->Add (Vector(187.71725535666366,374.0322605044723, 0.0));
    positionAlloc4->Add (Vector(114.21788865063698,74.55763803384258, 0.0));
    positionAlloc4->Add (Vector(390.55630609140223,88.80475572276092, 0.0));
    positionAlloc4->Add (Vector(66.89886777985072,38.80362703380968, 0.0));
    positionAlloc4->Add (Vector(118.15478214708975,432.21692274241144, 0.0));
    positionAlloc4->Add (Vector(390.8037127721978,376.40089349638856, 0.0));
    positionAlloc4->Add (Vector(338.99966164886445,436.20538735657476, 0.0));
    positionAlloc4->Add (Vector(317.4706610361388,162.63228412039376, 0.0));
    positionAlloc4->Add (Vector(186.94094596203598,14.28483636868999, 0.0));
    positionAlloc4->Add (Vector(226.52263700986492,89.62918700195121, 0.0));
    positionAlloc4->Add (Vector(452.4682106256983,456.51324811822116, 0.0));
    positionAlloc4->Add (Vector(462.4254487443922,2.7854643206394747, 0.0));
    positionAlloc4->Add (Vector(386.0949234298398,256.7166776665287, 0.0));
    positionAlloc4->Add (Vector(481.60473704748665,85.85783269136643, 0.0));
    positionAlloc4->Add (Vector(369.85147172575944,120.94230052899724, 0.0));
    positionAlloc4->Add (Vector(219.54161796924336,166.20621385957517, 0.0));
    positionAlloc4->Add (Vector(297.9848610538215,92.85403910920486, 0.0));
    positionAlloc4->Add (Vector(296.22865599451404,80.13332693373049, 0.0));
    positionAlloc4->Add (Vector(435.0754157694938,254.85180846171406, 0.0));
    positionAlloc4->Add (Vector(461.3864129945579,17.177117709153812, 0.0));
    positionAlloc4->Add (Vector(295.15554641437944,311.32426315044745, 0.0));
    positionAlloc4->Add (Vector(187.08400162975153,434.3000798504353, 0.0));
    positionAlloc4->Add (Vector(321.6703720021064,263.37672790165954, 0.0));
    positionAlloc4->Add (Vector(323.7565141558977,246.1057700783081, 0.0));
    positionAlloc4->Add (Vector(316.1209524017698,393.1069774229652, 0.0));
    positionAlloc4->Add (Vector(404.6234399560977,122.0813265850439, 0.0));
    positionAlloc4->Add (Vector(263.1435323201172,324.0180576573662, 0.0));
    positionAlloc4->Add (Vector(370.10844627200527,264.2132843786499, 0.0));
    positionAlloc4->Add (Vector(93.95215510905497,131.65476259441812, 0.0));
    positionAlloc4->Add (Vector(61.731701816127526,429.63447295155373, 0.0));
    positionAlloc4->Add (Vector(220.2252213035592,301.29813732085427, 0.0));
    positionAlloc4->Add (Vector(88.6420048663778,42.173387787157736, 0.0));
    positionAlloc4->Add (Vector(369.91582088237845,216.15105439052263, 0.0));
    positionAlloc4->Add (Vector(300.2208439575479,73.19709197643253, 0.0));
    positionAlloc4->Add (Vector(231.21948805026037,50.466027685441674, 0.0));
    positionAlloc4->Add (Vector(29.724893330376258,384.0486822921283, 0.0));
    positionAlloc4->Add (Vector(310.0480691340171,88.52958385744108, 0.0));
    positionAlloc4->Add (Vector(59.60677772543327,186.95609692757154, 0.0));
    positionAlloc4->Add (Vector(394.0761436271881,16.302665975148344, 0.0));
    positionAlloc4->Add (Vector(419.06493630971494,23.931220761279626, 0.0));
    positionAlloc4->Add (Vector(52.25715997969116,333.2775179801803, 0.0));
    positionAlloc4->Add (Vector(92.18470138069712,353.08875411401675, 0.0));
    positionAlloc4->Add (Vector(237.74368349181756,401.0921459886617, 0.0));
    positionAlloc4->Add (Vector(232.19765766457735,472.78771746520965, 0.0));
    positionAlloc4->Add (Vector(35.43721315947579,45.03779665323188, 0.0));
    positionAlloc4->Add (Vector(422.7280653005467,179.9563913721753, 0.0));
    positionAlloc4->Add (Vector(146.5674083222187,412.7957948942328, 0.0));
    positionAlloc4->Add (Vector(105.92106999013151,477.96158845524707, 0.0));
    positionAlloc4->Add (Vector(9.944202256221557,37.82657606943934, 0.0));
    positionAlloc4->Add (Vector(268.99739226583773,228.8737024054604, 0.0));
    positionAlloc4->Add (Vector(424.1087555633969,197.14010736998455, 0.0));
    positionAlloc4->Add (Vector(386.1160919533018,96.10123590780884, 0.0));
    positionAlloc4->Add (Vector(353.1243813630181,311.11997838499025, 0.0));
    positionAlloc4->Add (Vector(378.7794859288618,336.1383068358092, 0.0));
    positionAlloc4->Add (Vector(436.6254595685019,72.13040340713495, 0.0));
    positionAlloc4->Add (Vector(298.8020685205791,16.927379997388524, 0.0));
    positionAlloc4->Add (Vector(383.42192467701057,317.1642959722241, 0.0));
    positionAlloc4->Add (Vector(108.92751337156947,192.66316366768194, 0.0));
    positionAlloc4->Add (Vector(284.37484219211416,141.98843623059244, 0.0));
    positionAlloc4->Add (Vector(256.4955999739039,233.96400645566118, 0.0));
    positionAlloc4->Add (Vector(365.94646066993425,176.6378215295728, 0.0));
    positionAlloc4->Add (Vector(144.03264821659678,429.0834241648424, 0.0));
    positionAlloc4->Add (Vector(269.18347214413023,75.7570488171816, 0.0));
    positionAlloc4->Add (Vector(327.91660552992283,14.134078405764638, 0.0));
    positionAlloc4->Add (Vector(336.872538440243,411.57701130068216, 0.0));
    positionAlloc4->Add (Vector(55.545976032468026,268.6692282600094, 0.0));
    positionAlloc4->Add (Vector(32.336248498560096,270.41166640428406, 0.0));
    positionAlloc4->Add (Vector(490.39941447653626,238.0022349723694, 0.0));
    positionAlloc4->Add (Vector(401.9506180754722,150.74761048331843, 0.0));
    positionAlloc4->Add (Vector(245.14669231712134,60.865660430606596, 0.0));
    positionAlloc4->Add (Vector(354.502013534737,310.81209053643016, 0.0));
    positionAlloc4->Add (Vector(132.07085274485257,18.336384053119993, 0.0));
    positionAlloc4->Add (Vector(127.8880655147862,344.95240513786064, 0.0));
    positionAlloc4->Add (Vector(197.88720741226751,445.3663873471539, 0.0));
    positionAlloc4->Add (Vector(225.3226446350585,323.7497419845239, 0.0));
    positionAlloc4->Add (Vector(50.72871936303092,126.01868206818328, 0.0));
    positionAlloc4->Add (Vector(273.3265980206965,362.3522957380097, 0.0));
    positionAlloc4->Add (Vector(356.32628280935853,200.67810132535334, 0.0));
    positionAlloc4->Add (Vector(12.842789794856369,444.7193548978924, 0.0));
    positionAlloc4->Add (Vector(407.5062614309555,224.02932808595045, 0.0));
    positionAlloc4->Add (Vector(305.4610941987057,364.9606971751195, 0.0));
    positionAlloc4->Add (Vector(87.71536575077393,236.59496670094043, 0.0));
    positionAlloc4->Add (Vector(296.5921914210638,16.71112544961595, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_22.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_ECC_flowmon/switch_ECC_22.flowmon", false, false);
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