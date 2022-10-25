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
	  uint16_t numberOfRrhs = 40;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 488;
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
    positionAlloc->Add (Vector (400.0,10.0, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,270.0, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,10.0, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,140.0, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (270.0,270.0, 0.0));
    positionAlloc->Add (Vector (270.0,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (270.0,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (270.0,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,10.0, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,270.0, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (140.0,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (140.0,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (140.0,270.0, 0.0));
    positionAlloc->Add (Vector (140.0,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (140.0,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (140.0,10.0, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,270.0, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,270.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (10.0,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (10.0,270.0, 0.0));
    positionAlloc->Add (Vector (10.0,140.0, 0.0));
    positionAlloc->Add (Vector (10.0,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (10.0,10.0, 0.0));
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
    positionAlloc4->Add (Vector(224.690844366949,73.15513127112432, 0.0));
    positionAlloc4->Add (Vector(291.5570345729516,95.151483392999, 0.0));
    positionAlloc4->Add (Vector(144.14061124988947,91.79349688641638, 0.0));
    positionAlloc4->Add (Vector(105.8813129197393,200.4832346592516, 0.0));
    positionAlloc4->Add (Vector(284.8238188991592,371.0112084080613, 0.0));
    positionAlloc4->Add (Vector(123.61394603578107,59.581654525115766, 0.0));
    positionAlloc4->Add (Vector(356.18835170173713,68.47701866461335, 0.0));
    positionAlloc4->Add (Vector(333.7820899995859,153.72999944677807, 0.0));
    positionAlloc4->Add (Vector(348.59945211286873,220.98718740879715, 0.0));
    positionAlloc4->Add (Vector(268.003423064563,248.78148211445085, 0.0));
    positionAlloc4->Add (Vector(161.61059877330288,232.5825966081866, 0.0));
    positionAlloc4->Add (Vector(337.3983774167529,16.67227729068923, 0.0));
    positionAlloc4->Add (Vector(337.14278217288313,272.1375154722776, 0.0));
    positionAlloc4->Add (Vector(54.23712339764091,398.18817779032105, 0.0));
    positionAlloc4->Add (Vector(371.9393185121365,60.32923594367947, 0.0));
    positionAlloc4->Add (Vector(367.64873793961556,67.64417763970738, 0.0));
    positionAlloc4->Add (Vector(339.30705612878154,282.0728285229529, 0.0));
    positionAlloc4->Add (Vector(315.1162618419625,81.71880711054094, 0.0));
    positionAlloc4->Add (Vector(393.12907675710085,157.87258091885005, 0.0));
    positionAlloc4->Add (Vector(125.75838081294526,315.87150104014705, 0.0));
    positionAlloc4->Add (Vector(67.88173434447904,112.03203027810873, 0.0));
    positionAlloc4->Add (Vector(134.153778965801,31.10376462850306, 0.0));
    positionAlloc4->Add (Vector(357.6581293230681,108.29477681649405, 0.0));
    positionAlloc4->Add (Vector(124.04804267293513,356.36534783366284, 0.0));
    positionAlloc4->Add (Vector(306.2340693838825,36.221191723758615, 0.0));
    positionAlloc4->Add (Vector(49.77931142155887,136.27921271472582, 0.0));
    positionAlloc4->Add (Vector(373.28901803107726,106.9842667921379, 0.0));
    positionAlloc4->Add (Vector(346.03243994427186,13.141385593048671, 0.0));
    positionAlloc4->Add (Vector(80.95310713004152,194.80913330428797, 0.0));
    positionAlloc4->Add (Vector(68.78601915596559,263.2819923714883, 0.0));
    positionAlloc4->Add (Vector(223.322442577045,17.39605343679851, 0.0));
    positionAlloc4->Add (Vector(45.75057529698614,53.541039231362134, 0.0));
    positionAlloc4->Add (Vector(127.72884566261165,53.440824378343166, 0.0));
    positionAlloc4->Add (Vector(197.8117273767288,279.09379813996554, 0.0));
    positionAlloc4->Add (Vector(279.1397470124656,45.305340410662694, 0.0));
    positionAlloc4->Add (Vector(294.0550225374541,226.95655137894812, 0.0));
    positionAlloc4->Add (Vector(111.40065184866134,344.1892340972961, 0.0));
    positionAlloc4->Add (Vector(129.24348004042324,362.5192208930785, 0.0));
    positionAlloc4->Add (Vector(160.09972135071445,158.36694046438558, 0.0));
    positionAlloc4->Add (Vector(206.95169244160644,330.192165865113, 0.0));
    positionAlloc4->Add (Vector(151.9752123245111,68.38028206456465, 0.0));
    positionAlloc4->Add (Vector(133.68253423119944,382.09901614287645, 0.0));
    positionAlloc4->Add (Vector(27.57526671867896,33.77408486447497, 0.0));
    positionAlloc4->Add (Vector(122.27496466950427,329.606404216935, 0.0));
    positionAlloc4->Add (Vector(268.2821365131867,279.32667154716904, 0.0));
    positionAlloc4->Add (Vector(285.3818753337656,126.96010521084196, 0.0));
    positionAlloc4->Add (Vector(218.87756776519137,259.83850986980485, 0.0));
    positionAlloc4->Add (Vector(300.7148708944098,267.5697504689943, 0.0));
    positionAlloc4->Add (Vector(308.5602540512594,308.73946668069556, 0.0));
    positionAlloc4->Add (Vector(85.77088607998245,250.96209027331597, 0.0));
    positionAlloc4->Add (Vector(105.80374221502127,289.45873912863635, 0.0));
    positionAlloc4->Add (Vector(301.0568011610387,333.52153190062916, 0.0));
    positionAlloc4->Add (Vector(27.70509260289544,26.659564019167846, 0.0));
    positionAlloc4->Add (Vector(58.93743181628386,2.2920823420524705, 0.0));
    positionAlloc4->Add (Vector(159.0392856145066,101.26759732660031, 0.0));
    positionAlloc4->Add (Vector(348.57480381581547,10.488488450344224, 0.0));
    positionAlloc4->Add (Vector(394.45764872914606,225.74795418966661, 0.0));
    positionAlloc4->Add (Vector(345.62708425891276,310.008602967009, 0.0));
    positionAlloc4->Add (Vector(391.7302531744183,10.558509652954662, 0.0));
    positionAlloc4->Add (Vector(9.350449779192616,138.69430983075594, 0.0));
    positionAlloc4->Add (Vector(144.87201299276364,358.06469965735624, 0.0));
    positionAlloc4->Add (Vector(73.9237993695621,186.9270655984669, 0.0));
    positionAlloc4->Add (Vector(92.2607286478966,378.5132881946366, 0.0));
    positionAlloc4->Add (Vector(57.51839434275534,289.43516938669865, 0.0));
    positionAlloc4->Add (Vector(213.8478436848585,390.91783112975406, 0.0));
    positionAlloc4->Add (Vector(122.47340612243866,11.360822336689491, 0.0));
    positionAlloc4->Add (Vector(210.1040148405493,99.75072960818383, 0.0));
    positionAlloc4->Add (Vector(137.30698594704137,176.0519715916418, 0.0));
    positionAlloc4->Add (Vector(266.27289969312585,293.78812913389123, 0.0));
    positionAlloc4->Add (Vector(228.83619224070958,246.70952131677336, 0.0));
    positionAlloc4->Add (Vector(168.5577202303707,228.9712808933139, 0.0));
    positionAlloc4->Add (Vector(250.2372180762343,184.25733078928096, 0.0));
    positionAlloc4->Add (Vector(22.648525448027222,198.80073541440203, 0.0));
    positionAlloc4->Add (Vector(329.0863236701358,175.6710811336166, 0.0));
    positionAlloc4->Add (Vector(2.002216695680703,105.84706627241216, 0.0));
    positionAlloc4->Add (Vector(236.12923083811103,236.73691599794805, 0.0));
    positionAlloc4->Add (Vector(261.8573351400372,137.03923121195652, 0.0));
    positionAlloc4->Add (Vector(182.94559754616358,349.26923097834595, 0.0));
    positionAlloc4->Add (Vector(174.9380854933097,108.72951363170715, 0.0));
    positionAlloc4->Add (Vector(283.43505503883176,268.8418840815282, 0.0));
    positionAlloc4->Add (Vector(369.85750194331763,347.009241225377, 0.0));
    positionAlloc4->Add (Vector(313.65945098405723,199.86505316773275, 0.0));
    positionAlloc4->Add (Vector(298.21129791466325,287.04869889796083, 0.0));
    positionAlloc4->Add (Vector(124.66604682487126,103.07426475494306, 0.0));
    positionAlloc4->Add (Vector(42.99215907792879,194.4503722104912, 0.0));
    positionAlloc4->Add (Vector(33.32271212940938,206.04479502593574, 0.0));
    positionAlloc4->Add (Vector(62.21695979615936,294.85347734269436, 0.0));
    positionAlloc4->Add (Vector(340.92963677916,40.337915624936294, 0.0));
    positionAlloc4->Add (Vector(361.4891115626493,18.834167129302728, 0.0));
    positionAlloc4->Add (Vector(375.46060961669616,297.0874126584052, 0.0));
    positionAlloc4->Add (Vector(119.48492701047466,247.72283435118595, 0.0));
    positionAlloc4->Add (Vector(41.091550713621274,240.07572568379305, 0.0));
    positionAlloc4->Add (Vector(108.25772128969788,68.11148556306637, 0.0));
    positionAlloc4->Add (Vector(354.99835121663807,21.537754272995755, 0.0));
    positionAlloc4->Add (Vector(164.52707492152481,173.0996103469988, 0.0));
    positionAlloc4->Add (Vector(30.14566797443785,269.25800271686364, 0.0));
    positionAlloc4->Add (Vector(310.72420511085386,90.72222556618632, 0.0));
    positionAlloc4->Add (Vector(113.4361182646093,192.4330278586153, 0.0));
    positionAlloc4->Add (Vector(299.5794796959052,236.9542608767679, 0.0));
    positionAlloc4->Add (Vector(251.46036539335904,65.54432575279274, 0.0));
    positionAlloc4->Add (Vector(360.97394070440913,317.9896201727854, 0.0));
    positionAlloc4->Add (Vector(120.40039732835739,286.10901107597925, 0.0));
    positionAlloc4->Add (Vector(313.00757418814476,154.38293559769795, 0.0));
    positionAlloc4->Add (Vector(46.0778902418526,150.10286698110647, 0.0));
    positionAlloc4->Add (Vector(122.8861824687557,170.52567403282944, 0.0));
    positionAlloc4->Add (Vector(82.78995204844621,68.96426341856467, 0.0));
    positionAlloc4->Add (Vector(387.9877539890681,136.20230389410023, 0.0));
    positionAlloc4->Add (Vector(96.561315076077,88.0441686851229, 0.0));
    positionAlloc4->Add (Vector(329.48394685945823,386.78582499625367, 0.0));
    positionAlloc4->Add (Vector(399.4254515375735,195.84805063480366, 0.0));
    positionAlloc4->Add (Vector(211.12245089920606,5.0884572377581705, 0.0));
    positionAlloc4->Add (Vector(7.311440599518981,28.35393508262336, 0.0));
    positionAlloc4->Add (Vector(190.167121037735,360.58592488383437, 0.0));
    positionAlloc4->Add (Vector(8.531287317355574,82.66857936046095, 0.0));
    positionAlloc4->Add (Vector(271.8206322657871,58.7720601112101, 0.0));
    positionAlloc4->Add (Vector(150.36863503112468,300.6130892383023, 0.0));
    positionAlloc4->Add (Vector(305.4574542378297,115.80754118354713, 0.0));
    positionAlloc4->Add (Vector(286.02153990358113,176.37593746344686, 0.0));
    positionAlloc4->Add (Vector(393.4577658357334,81.11719218787572, 0.0));
    positionAlloc4->Add (Vector(371.81021980716184,325.9705725720442, 0.0));
    positionAlloc4->Add (Vector(33.03970823275164,38.13706418164613, 0.0));
    positionAlloc4->Add (Vector(34.26882047733009,295.4799065668344, 0.0));
    positionAlloc4->Add (Vector(339.8206394673894,178.34870501014294, 0.0));
    positionAlloc4->Add (Vector(231.4182031414712,87.51789001029393, 0.0));
    positionAlloc4->Add (Vector(305.5133936947519,399.0875407287147, 0.0));
    positionAlloc4->Add (Vector(13.055066479961575,79.19485827703969, 0.0));
    positionAlloc4->Add (Vector(398.0610436213975,12.77426723202204, 0.0));
    positionAlloc4->Add (Vector(63.48784660302811,296.7642030187495, 0.0));
    positionAlloc4->Add (Vector(177.88087845529432,151.73201354115432, 0.0));
    positionAlloc4->Add (Vector(327.04555905656144,65.67146752881858, 0.0));
    positionAlloc4->Add (Vector(144.14069693287965,217.15353510248656, 0.0));
    positionAlloc4->Add (Vector(66.72273275912137,352.007918460852, 0.0));
    positionAlloc4->Add (Vector(326.7209333733514,89.39856296859641, 0.0));
    positionAlloc4->Add (Vector(305.3557989549095,282.68986851089426, 0.0));
    positionAlloc4->Add (Vector(64.33752592928283,245.3781557151198, 0.0));
    positionAlloc4->Add (Vector(157.16391125096507,292.34694432561105, 0.0));
    positionAlloc4->Add (Vector(69.56924840541099,357.4743349907532, 0.0));
    positionAlloc4->Add (Vector(142.5662924793071,19.146818850500757, 0.0));
    positionAlloc4->Add (Vector(7.9402281949660924,396.5681922629844, 0.0));
    positionAlloc4->Add (Vector(180.5752385211432,225.17935808637867, 0.0));
    positionAlloc4->Add (Vector(103.5295713018788,53.34865459959892, 0.0));
    positionAlloc4->Add (Vector(251.9179503505018,366.8657290552615, 0.0));
    positionAlloc4->Add (Vector(249.51797695492618,393.1955285728416, 0.0));
    positionAlloc4->Add (Vector(330.94636865311855,135.53045110660779, 0.0));
    positionAlloc4->Add (Vector(347.5564227883113,43.597103579407644, 0.0));
    positionAlloc4->Add (Vector(116.35517706871426,236.0120616877071, 0.0));
    positionAlloc4->Add (Vector(44.94221886338994,383.94559655123527, 0.0));
    positionAlloc4->Add (Vector(169.13781870727684,194.14338895848832, 0.0));
    positionAlloc4->Add (Vector(182.92404248596497,380.37974595433843, 0.0));
    positionAlloc4->Add (Vector(46.44577201711675,203.742009627777, 0.0));
    positionAlloc4->Add (Vector(259.1267249006671,72.6500468125502, 0.0));
    positionAlloc4->Add (Vector(281.8439345534678,304.0595560565518, 0.0));
    positionAlloc4->Add (Vector(61.747590400797804,56.7848086914168, 0.0));
    positionAlloc4->Add (Vector(252.92490532147235,276.3145137209415, 0.0));
    positionAlloc4->Add (Vector(140.71143733180116,62.37829900867591, 0.0));
    positionAlloc4->Add (Vector(281.94231393683685,194.5676676330941, 0.0));
    positionAlloc4->Add (Vector(92.8907333261649,310.6118513706496, 0.0));
    positionAlloc4->Add (Vector(261.39118486107213,382.3815531287005, 0.0));
    positionAlloc4->Add (Vector(371.62506059040277,357.92793256231107, 0.0));
    positionAlloc4->Add (Vector(278.45991428316796,175.98129915997288, 0.0));
    positionAlloc4->Add (Vector(42.53001119536073,166.15134619812775, 0.0));
    positionAlloc4->Add (Vector(326.67943205208246,354.9505032315533, 0.0));
    positionAlloc4->Add (Vector(54.662034493120885,107.27717118175507, 0.0));
    positionAlloc4->Add (Vector(281.1076842441875,84.14317033050666, 0.0));
    positionAlloc4->Add (Vector(115.74711631177115,181.35194130695908, 0.0));
    positionAlloc4->Add (Vector(386.8389599887526,192.87822646858643, 0.0));
    positionAlloc4->Add (Vector(381.3601410660083,24.177968722865593, 0.0));
    positionAlloc4->Add (Vector(86.76324983621515,230.30938029848863, 0.0));
    positionAlloc4->Add (Vector(315.3877329213498,2.445227358541402, 0.0));
    positionAlloc4->Add (Vector(388.93292630871196,258.9083536409011, 0.0));
    positionAlloc4->Add (Vector(323.9621943755942,163.63986066389225, 0.0));
    positionAlloc4->Add (Vector(123.81808025793117,251.00491790107955, 0.0));
    positionAlloc4->Add (Vector(208.35971604430497,274.8806872895657, 0.0));
    positionAlloc4->Add (Vector(121.74628978716653,162.0271257468691, 0.0));
    positionAlloc4->Add (Vector(11.632503670790184,300.2656914109267, 0.0));
    positionAlloc4->Add (Vector(41.28605505882214,326.924887628821, 0.0));
    positionAlloc4->Add (Vector(125.0001605107863,8.669718520234415, 0.0));
    positionAlloc4->Add (Vector(49.24328347742377,234.777106636215, 0.0));
    positionAlloc4->Add (Vector(186.47873371751697,91.25765707681003, 0.0));
    positionAlloc4->Add (Vector(93.9369223075035,182.49625962467655, 0.0));
    positionAlloc4->Add (Vector(241.1089713592375,80.05736516002666, 0.0));
    positionAlloc4->Add (Vector(190.82715607894372,40.19940895725882, 0.0));
    positionAlloc4->Add (Vector(365.063034517227,126.93012132732267, 0.0));
    positionAlloc4->Add (Vector(164.4101788982374,364.6548536760779, 0.0));
    positionAlloc4->Add (Vector(62.02520100328157,259.8303870534775, 0.0));
    positionAlloc4->Add (Vector(184.77373227378294,232.2890125845968, 0.0));
    positionAlloc4->Add (Vector(374.631248478984,188.45354245804472, 0.0));
    positionAlloc4->Add (Vector(172.84021009497573,164.03598162112382, 0.0));
    positionAlloc4->Add (Vector(377.07818906558,291.4125756566549, 0.0));
    positionAlloc4->Add (Vector(97.448575852833,69.69044047752769, 0.0));
    positionAlloc4->Add (Vector(222.0195528597615,2.966425202040579, 0.0));
    positionAlloc4->Add (Vector(36.427725934429624,33.379808643828966, 0.0));
    positionAlloc4->Add (Vector(182.92274569990522,244.51012858362003, 0.0));
    positionAlloc4->Add (Vector(80.8952367902247,361.85582645044406, 0.0));
    positionAlloc4->Add (Vector(310.83922757451654,206.66622863854207, 0.0));
    positionAlloc4->Add (Vector(332.59186496475974,79.77537434240122, 0.0));
    positionAlloc4->Add (Vector(197.7445286320616,66.86986833514999, 0.0));
    positionAlloc4->Add (Vector(91.8637435549377,234.7636007629638, 0.0));
    positionAlloc4->Add (Vector(145.6332925925127,82.16722016034925, 0.0));
    positionAlloc4->Add (Vector(314.9945288571678,156.5411959065991, 0.0));
    positionAlloc4->Add (Vector(173.23999160966144,110.59785532589102, 0.0));
    positionAlloc4->Add (Vector(190.24768067826616,14.618482901374374, 0.0));
    positionAlloc4->Add (Vector(227.68676106040706,24.376663641085152, 0.0));
    positionAlloc4->Add (Vector(350.80663709221926,397.766001743308, 0.0));
    positionAlloc4->Add (Vector(198.71260755032102,34.868375667240414, 0.0));
    positionAlloc4->Add (Vector(14.346839231110131,310.53150910214515, 0.0));
    positionAlloc4->Add (Vector(282.25787124550396,41.999839099278404, 0.0));
    positionAlloc4->Add (Vector(73.72940512003723,72.20855052894981, 0.0));
    positionAlloc4->Add (Vector(89.99595237376532,272.898258545371, 0.0));
    positionAlloc4->Add (Vector(339.6429869384129,26.712707041526507, 0.0));
    positionAlloc4->Add (Vector(262.3656597638415,379.62463072117873, 0.0));
    positionAlloc4->Add (Vector(201.50820705646245,36.079793337189116, 0.0));
    positionAlloc4->Add (Vector(138.01948847847777,0.47685290500107946, 0.0));
    positionAlloc4->Add (Vector(137.08965487488084,320.5935799409707, 0.0));
    positionAlloc4->Add (Vector(116.73670309925419,220.63157249449944, 0.0));
    positionAlloc4->Add (Vector(3.367606381435362,137.03751325891878, 0.0));
    positionAlloc4->Add (Vector(111.29615253072531,10.877950222031085, 0.0));
    positionAlloc4->Add (Vector(284.4810751768267,199.15370246315348, 0.0));
    positionAlloc4->Add (Vector(272.25176340709635,227.36467628976524, 0.0));
    positionAlloc4->Add (Vector(13.694591463102368,343.8023779389821, 0.0));
    positionAlloc4->Add (Vector(381.02013646193996,279.42029468363046, 0.0));
    positionAlloc4->Add (Vector(266.4390551644472,290.34213055838467, 0.0));
    positionAlloc4->Add (Vector(212.36607594428074,323.04997863217835, 0.0));
    positionAlloc4->Add (Vector(354.8027954466933,318.5119833571919, 0.0));
    positionAlloc4->Add (Vector(395.8429396471362,359.25994819624606, 0.0));
    positionAlloc4->Add (Vector(301.8098641784283,245.24181136312788, 0.0));
    positionAlloc4->Add (Vector(343.2725422832108,43.3060939259843, 0.0));
    positionAlloc4->Add (Vector(17.392112670851567,258.4902897829313, 0.0));
    positionAlloc4->Add (Vector(283.18166480549746,75.27350537608997, 0.0));
    positionAlloc4->Add (Vector(283.1737145515275,135.09040293421992, 0.0));
    positionAlloc4->Add (Vector(44.91686131655683,214.8188882785172, 0.0));
    positionAlloc4->Add (Vector(291.75108279233365,135.02516762125873, 0.0));
    positionAlloc4->Add (Vector(108.89690237865901,188.94858246395785, 0.0));
    positionAlloc4->Add (Vector(8.599390808674867,69.68056373497818, 0.0));
    positionAlloc4->Add (Vector(307.27630654454913,356.7412587227912, 0.0));
    positionAlloc4->Add (Vector(171.86238516640825,59.47853916716901, 0.0));
    positionAlloc4->Add (Vector(382.55005250160184,3.703163050743008, 0.0));
    positionAlloc4->Add (Vector(340.4782080860761,176.00058157364634, 0.0));
    positionAlloc4->Add (Vector(355.11097269777747,80.9975341550246, 0.0));
    positionAlloc4->Add (Vector(76.15395240913867,266.7212011243856, 0.0));
    positionAlloc4->Add (Vector(293.247610201925,391.18979763360045, 0.0));
    positionAlloc4->Add (Vector(178.20971298026248,189.04731494599335, 0.0));
    positionAlloc4->Add (Vector(353.8299053685089,397.857791171485, 0.0));
    positionAlloc4->Add (Vector(353.97382131898837,395.07953207659284, 0.0));
    positionAlloc4->Add (Vector(46.119643642346155,270.41124368423823, 0.0));
    positionAlloc4->Add (Vector(170.4618643151564,286.82071837709515, 0.0));
    positionAlloc4->Add (Vector(117.8517405707233,349.54092018176414, 0.0));
    positionAlloc4->Add (Vector(254.56280008912785,295.3076023391395, 0.0));
    positionAlloc4->Add (Vector(134.33465330656594,285.75869202565826, 0.0));
    positionAlloc4->Add (Vector(161.4642883789898,24.87402915355692, 0.0));
    positionAlloc4->Add (Vector(253.1428713306832,0.8365596760822935, 0.0));
    positionAlloc4->Add (Vector(232.23032700078602,315.7287976338592, 0.0));
    positionAlloc4->Add (Vector(376.099796102923,106.58207947091726, 0.0));
    positionAlloc4->Add (Vector(16.233490101459846,261.6838282712007, 0.0));
    positionAlloc4->Add (Vector(5.5043785530824785,345.8842462111218, 0.0));
    positionAlloc4->Add (Vector(22.94638809402949,335.4805398518577, 0.0));
    positionAlloc4->Add (Vector(233.63721108441644,123.76454491768878, 0.0));
    positionAlloc4->Add (Vector(231.1049535290963,191.54841303796326, 0.0));
    positionAlloc4->Add (Vector(161.70429140952987,224.9044648391773, 0.0));
    positionAlloc4->Add (Vector(58.065683650302404,256.88466775621566, 0.0));
    positionAlloc4->Add (Vector(88.41466684338246,86.25448800143576, 0.0));
    positionAlloc4->Add (Vector(236.88516642090502,163.64073532197474, 0.0));
    positionAlloc4->Add (Vector(206.93828016973325,379.1244429510337, 0.0));
    positionAlloc4->Add (Vector(261.80636600597893,120.518308205739, 0.0));
    positionAlloc4->Add (Vector(192.82697291214822,264.8583063317462, 0.0));
    positionAlloc4->Add (Vector(158.75693357454796,75.31813148422968, 0.0));
    positionAlloc4->Add (Vector(254.05491796787805,377.5766114476745, 0.0));
    positionAlloc4->Add (Vector(233.26652055857343,72.68563974886142, 0.0));
    positionAlloc4->Add (Vector(250.28239194287795,30.334251207469798, 0.0));
    positionAlloc4->Add (Vector(300.5797600115703,226.0877392032782, 0.0));
    positionAlloc4->Add (Vector(361.9514328116015,251.91920367921924, 0.0));
    positionAlloc4->Add (Vector(394.90635775113464,87.77244565574276, 0.0));
    positionAlloc4->Add (Vector(251.67784307378201,86.57489514811148, 0.0));
    positionAlloc4->Add (Vector(154.3171294371387,299.46398705008335, 0.0));
    positionAlloc4->Add (Vector(61.58570131139109,331.06493322790914, 0.0));
    positionAlloc4->Add (Vector(137.9025062000195,303.8085149460985, 0.0));
    positionAlloc4->Add (Vector(69.28159372878669,97.32762080960353, 0.0));
    positionAlloc4->Add (Vector(315.6992329405282,217.27446777800247, 0.0));
    positionAlloc4->Add (Vector(19.75706124558223,15.139888422568948, 0.0));
    positionAlloc4->Add (Vector(58.22698295542148,305.1947973350579, 0.0));
    positionAlloc4->Add (Vector(384.22972575891157,31.58655888038031, 0.0));
    positionAlloc4->Add (Vector(63.30579889401924,92.72768535433826, 0.0));
    positionAlloc4->Add (Vector(260.3661847924589,7.533409848958028, 0.0));
    positionAlloc4->Add (Vector(247.45061087506474,59.335179769103604, 0.0));
    positionAlloc4->Add (Vector(6.27860087200296,133.5838761901462, 0.0));
    positionAlloc4->Add (Vector(85.90800903523896,144.3192557711998, 0.0));
    positionAlloc4->Add (Vector(354.87467771578525,217.04584878881468, 0.0));
    positionAlloc4->Add (Vector(239.81407035666362,36.90501034238301, 0.0));
    positionAlloc4->Add (Vector(23.26245555082922,259.1869688139279, 0.0));
    positionAlloc4->Add (Vector(239.73991020541195,81.73191065975125, 0.0));
    positionAlloc4->Add (Vector(220.2320840642897,347.1736891551214, 0.0));
    positionAlloc4->Add (Vector(152.21554879832624,32.410862213742185, 0.0));
    positionAlloc4->Add (Vector(102.01033003750446,140.94243191607148, 0.0));
    positionAlloc4->Add (Vector(322.2460453633209,194.96737041882298, 0.0));
    positionAlloc4->Add (Vector(50.095915046679316,287.0210393788358, 0.0));
    positionAlloc4->Add (Vector(346.5968081539046,379.22524907523234, 0.0));
    positionAlloc4->Add (Vector(325.76731229020265,353.4703513159966, 0.0));
    positionAlloc4->Add (Vector(235.63570986470444,291.38564784690925, 0.0));
    positionAlloc4->Add (Vector(239.1825635576244,10.986946099652029, 0.0));
    positionAlloc4->Add (Vector(15.466225272055656,34.53754580662513, 0.0));
    positionAlloc4->Add (Vector(130.86429762487734,356.76107824032937, 0.0));
    positionAlloc4->Add (Vector(269.7661044915669,311.0725580318366, 0.0));
    positionAlloc4->Add (Vector(190.10211725576283,255.10444954866153, 0.0));
    positionAlloc4->Add (Vector(196.64891745605897,396.53163878439005, 0.0));
    positionAlloc4->Add (Vector(120.59507435784917,18.17739375629848, 0.0));
    positionAlloc4->Add (Vector(136.2520362268854,375.6764061920089, 0.0));
    positionAlloc4->Add (Vector(99.95727472371385,176.07683865039684, 0.0));
    positionAlloc4->Add (Vector(369.0443857819481,194.84425129615738, 0.0));
    positionAlloc4->Add (Vector(300.3119778114109,4.363768719230832, 0.0));
    positionAlloc4->Add (Vector(349.1910707784119,161.67102276861183, 0.0));
    positionAlloc4->Add (Vector(229.03459694129626,1.782403835207802, 0.0));
    positionAlloc4->Add (Vector(294.9311660462618,43.95026990126407, 0.0));
    positionAlloc4->Add (Vector(365.57322178094046,325.014882243269, 0.0));
    positionAlloc4->Add (Vector(374.9231335549027,313.941600072651, 0.0));
    positionAlloc4->Add (Vector(158.02167304268227,29.066726887024252, 0.0));
    positionAlloc4->Add (Vector(144.75521872602002,128.08582380990723, 0.0));
    positionAlloc4->Add (Vector(210.55028335486335,46.400170607335546, 0.0));
    positionAlloc4->Add (Vector(213.15591334284804,355.84396417053904, 0.0));
    positionAlloc4->Add (Vector(347.619142621062,5.392764123954263, 0.0));
    positionAlloc4->Add (Vector(198.4534527539457,195.45067038229095, 0.0));
    positionAlloc4->Add (Vector(207.69372020004923,45.57581496005936, 0.0));
    positionAlloc4->Add (Vector(100.27760020222436,335.6735520396363, 0.0));
    positionAlloc4->Add (Vector(189.4206569308087,166.2890187674289, 0.0));
    positionAlloc4->Add (Vector(93.99070464084929,308.45913752677336, 0.0));
    positionAlloc4->Add (Vector(285.2330510506322,396.4510152675222, 0.0));
    positionAlloc4->Add (Vector(40.77763109919217,211.41584041198493, 0.0));
    positionAlloc4->Add (Vector(15.79066947309995,23.14285947426855, 0.0));
    positionAlloc4->Add (Vector(324.43645559886727,203.63728102726597, 0.0));
    positionAlloc4->Add (Vector(96.79701303453024,384.6483199696618, 0.0));
    positionAlloc4->Add (Vector(23.609414996797717,377.8261398257335, 0.0));
    positionAlloc4->Add (Vector(36.62267941345894,139.18870876218742, 0.0));
    positionAlloc4->Add (Vector(372.1509817979587,265.71802720269136, 0.0));
    positionAlloc4->Add (Vector(64.10081998191366,72.95605328524357, 0.0));
    positionAlloc4->Add (Vector(164.85305329733723,287.91351114047944, 0.0));
    positionAlloc4->Add (Vector(301.3538995172229,228.03133448175558, 0.0));
    positionAlloc4->Add (Vector(331.4560847404876,9.24899759317328, 0.0));
    positionAlloc4->Add (Vector(136.99477684380065,326.40521058547193, 0.0));
    positionAlloc4->Add (Vector(234.63039919532048,255.714713628772, 0.0));
    positionAlloc4->Add (Vector(264.5699327670208,181.07269560495845, 0.0));
    positionAlloc4->Add (Vector(320.45912086295084,284.4597492402629, 0.0));
    positionAlloc4->Add (Vector(160.31589878911717,287.1855774374867, 0.0));
    positionAlloc4->Add (Vector(361.03076046133526,317.13164079122663, 0.0));
    positionAlloc4->Add (Vector(169.8092182144642,128.2991459712185, 0.0));
    positionAlloc4->Add (Vector(377.16774109480735,79.0910652776133, 0.0));
    positionAlloc4->Add (Vector(25.960961859985197,47.47945587560665, 0.0));
    positionAlloc4->Add (Vector(325.3422156687373,376.9474500873687, 0.0));
    positionAlloc4->Add (Vector(378.94886247635856,154.22846538113558, 0.0));
    positionAlloc4->Add (Vector(174.4000006522676,112.32715399518031, 0.0));
    positionAlloc4->Add (Vector(13.572103185226725,134.96133151055992, 0.0));
    positionAlloc4->Add (Vector(174.58379408509472,327.6789575741042, 0.0));
    positionAlloc4->Add (Vector(276.0906285464936,113.92053394294538, 0.0));
    positionAlloc4->Add (Vector(235.89968591492217,124.90561514675016, 0.0));
    positionAlloc4->Add (Vector(370.83366955655845,310.42123137103744, 0.0));
    positionAlloc4->Add (Vector(100.46477077885574,394.5038792457104, 0.0));
    positionAlloc4->Add (Vector(154.03852651012687,304.8439879417068, 0.0));
    positionAlloc4->Add (Vector(245.4438553612149,301.5942851682229, 0.0));
    positionAlloc4->Add (Vector(344.3124602390204,180.96669126701747, 0.0));
    positionAlloc4->Add (Vector(92.94893455880171,302.60638250641773, 0.0));
    positionAlloc4->Add (Vector(202.60015442840742,314.6357791647594, 0.0));
    positionAlloc4->Add (Vector(316.2952177690273,61.694658127346045, 0.0));
    positionAlloc4->Add (Vector(100.46635411594514,50.64476036484691, 0.0));
    positionAlloc4->Add (Vector(77.21111858315486,264.51170444854586, 0.0));
    positionAlloc4->Add (Vector(374.24957427967877,127.78459572219813, 0.0));
    positionAlloc4->Add (Vector(174.76805023521095,4.191511727895181, 0.0));
    positionAlloc4->Add (Vector(29.801202310883124,374.24783203214315, 0.0));
    positionAlloc4->Add (Vector(308.8127789006333,162.95229066436184, 0.0));
    positionAlloc4->Add (Vector(287.9936794022326,88.70407596747127, 0.0));
    positionAlloc4->Add (Vector(234.29145785628535,157.49454884826667, 0.0));
    positionAlloc4->Add (Vector(29.375472536590628,347.13349888629284, 0.0));
    positionAlloc4->Add (Vector(305.46162401410857,101.01238792204414, 0.0));
    positionAlloc4->Add (Vector(140.77108994065074,69.15856744077243, 0.0));
    positionAlloc4->Add (Vector(284.5726864903823,29.319138628612464, 0.0));
    positionAlloc4->Add (Vector(52.091734813369285,105.43556238874939, 0.0));
    positionAlloc4->Add (Vector(17.193285293548,232.16233195966404, 0.0));
    positionAlloc4->Add (Vector(130.6390450025377,274.50661123235574, 0.0));
    positionAlloc4->Add (Vector(1.760534992582219,129.59216650950518, 0.0));
    positionAlloc4->Add (Vector(194.028075149619,276.49131329265384, 0.0));
    positionAlloc4->Add (Vector(119.97813235230561,171.8622288427658, 0.0));
    positionAlloc4->Add (Vector(198.4187088473706,118.38523263692582, 0.0));
    positionAlloc4->Add (Vector(318.3244561528424,327.86582443422094, 0.0));
    positionAlloc4->Add (Vector(324.1726678040632,12.81641357318497, 0.0));
    positionAlloc4->Add (Vector(302.5677936493897,201.7422176949597, 0.0));
    positionAlloc4->Add (Vector(281.0881405167312,352.16721977845157, 0.0));
    positionAlloc4->Add (Vector(245.08938861041796,348.5935345755074, 0.0));
    positionAlloc4->Add (Vector(184.27222163787405,26.28805259303819, 0.0));
    positionAlloc4->Add (Vector(51.51048706139343,292.66389303988103, 0.0));
    positionAlloc4->Add (Vector(264.2360506696466,98.37979745121292, 0.0));
    positionAlloc4->Add (Vector(385.65378185244066,255.56242879984202, 0.0));
    positionAlloc4->Add (Vector(284.2702405250772,380.7398204102571, 0.0));
    positionAlloc4->Add (Vector(248.3494734453179,345.4143299623812, 0.0));
    positionAlloc4->Add (Vector(218.91206927837018,11.404432895582861, 0.0));
    positionAlloc4->Add (Vector(113.4810497048158,384.50501423446394, 0.0));
    positionAlloc4->Add (Vector(77.50617476647044,329.62048628096846, 0.0));
    positionAlloc4->Add (Vector(326.5381956120861,215.77638671753405, 0.0));
    positionAlloc4->Add (Vector(14.181474614013911,183.7013320836121, 0.0));
    positionAlloc4->Add (Vector(369.2335689038794,111.82521933824367, 0.0));
    positionAlloc4->Add (Vector(176.55091165648895,328.7439931658823, 0.0));
    positionAlloc4->Add (Vector(305.99679431493985,385.95977815282527, 0.0));
    positionAlloc4->Add (Vector(121.67590161977824,55.295126180617956, 0.0));
    positionAlloc4->Add (Vector(4.546673747836749,232.34895039100613, 0.0));
    positionAlloc4->Add (Vector(221.5330001408858,264.90509136325124, 0.0));
    positionAlloc4->Add (Vector(364.68669462932684,341.4980015358871, 0.0));
    positionAlloc4->Add (Vector(325.13795314638617,190.93332979687892, 0.0));
    positionAlloc4->Add (Vector(96.18887600118472,331.08378788952535, 0.0));
    positionAlloc4->Add (Vector(283.7882422405003,94.0896747062057, 0.0));
    positionAlloc4->Add (Vector(344.40627764714145,399.04099835996595, 0.0));
    positionAlloc4->Add (Vector(244.63811061810276,336.5924795600245, 0.0));
    positionAlloc4->Add (Vector(291.4801365100403,75.3744811815436, 0.0));
    positionAlloc4->Add (Vector(65.81833266216192,113.70189579418724, 0.0));
    positionAlloc4->Add (Vector(268.6909449861185,260.64565848440134, 0.0));
    positionAlloc4->Add (Vector(134.5031788759261,322.8284821640921, 0.0));
    positionAlloc4->Add (Vector(145.47876527374282,368.34399496777485, 0.0));
    positionAlloc4->Add (Vector(397.01127775004375,224.2599347895204, 0.0));
    positionAlloc4->Add (Vector(311.66116862764284,342.94069089470736, 0.0));
    positionAlloc4->Add (Vector(306.9889207576423,163.48602986855983, 0.0));
    positionAlloc4->Add (Vector(210.23330471495626,48.553399773827046, 0.0));
    positionAlloc4->Add (Vector(176.72805220878848,296.1423450570478, 0.0));
    positionAlloc4->Add (Vector(61.19406880588638,7.437492358215092, 0.0));
    positionAlloc4->Add (Vector(370.12366590285876,208.8997527710001, 0.0));
    positionAlloc4->Add (Vector(60.91031662084796,288.60625216139323, 0.0));
    positionAlloc4->Add (Vector(140.98576384276095,12.45006108451494, 0.0));
    positionAlloc4->Add (Vector(56.638268656912814,291.98638456807225, 0.0));
    positionAlloc4->Add (Vector(216.79303313599644,386.7186471639244, 0.0));
    positionAlloc4->Add (Vector(212.54819324323452,220.71491593002492, 0.0));
    positionAlloc4->Add (Vector(21.087756263336168,11.887168034571571, 0.0));
    positionAlloc4->Add (Vector(337.60932166284397,352.7945869787429, 0.0));
    positionAlloc4->Add (Vector(257.5397194598823,38.8242514421631, 0.0));
    positionAlloc4->Add (Vector(55.62677072694062,328.08332341836166, 0.0));
    positionAlloc4->Add (Vector(149.33770231652903,295.71690543937797, 0.0));
    positionAlloc4->Add (Vector(248.0770194400878,15.605947356831829, 0.0));
    positionAlloc4->Add (Vector(4.440114583801114,192.94154647776475, 0.0));
    positionAlloc4->Add (Vector(55.63658230845348,217.72368596468255, 0.0));
    positionAlloc4->Add (Vector(113.99038030688584,265.19641867342824, 0.0));
    positionAlloc4->Add (Vector(10.760695931452835,151.41986904632665, 0.0));
    positionAlloc4->Add (Vector(234.7854612818042,352.0977459735081, 0.0));
    positionAlloc4->Add (Vector(33.805480303245176,394.6796810062079, 0.0));
    positionAlloc4->Add (Vector(318.8320850495571,92.26700079058946, 0.0));
    positionAlloc4->Add (Vector(324.57645296565227,223.7252607227631, 0.0));
    positionAlloc4->Add (Vector(139.51313323333122,169.94740160395287, 0.0));
    positionAlloc4->Add (Vector(380.5229553294561,358.65499041899704, 0.0));
    positionAlloc4->Add (Vector(316.5556507798409,37.08861277987792, 0.0));
    positionAlloc4->Add (Vector(232.41286897752227,7.768074408040837, 0.0));
    positionAlloc4->Add (Vector(191.38539885933105,298.2991709240012, 0.0));
    positionAlloc4->Add (Vector(306.0458182421353,289.86401359239153, 0.0));
    positionAlloc4->Add (Vector(88.49130802797629,292.0146943265068, 0.0));
    positionAlloc4->Add (Vector(92.27981501516376,4.206949224269163, 0.0));
    positionAlloc4->Add (Vector(293.5567664490132,169.9404466327259, 0.0));
    positionAlloc4->Add (Vector(371.10307000085965,238.54228387713064, 0.0));
    positionAlloc4->Add (Vector(185.92761549835356,287.82188894329266, 0.0));
    positionAlloc4->Add (Vector(378.2993666575447,42.11204544044045, 0.0));
    positionAlloc4->Add (Vector(0.03157428471864954,128.22943886558443, 0.0));
    positionAlloc4->Add (Vector(166.4807144319844,12.158420420690863, 0.0));
    positionAlloc4->Add (Vector(290.5013875962463,243.4220020855257, 0.0));
    positionAlloc4->Add (Vector(249.14198885090678,100.9162758957038, 0.0));
    positionAlloc4->Add (Vector(399.3990418313275,34.48334531377686, 0.0));
    positionAlloc4->Add (Vector(398.1191634396878,219.3822346023473, 0.0));
    positionAlloc4->Add (Vector(89.52378555578085,94.33967188147956, 0.0));
    positionAlloc4->Add (Vector(273.10698532277746,115.53122278458136, 0.0));
    positionAlloc4->Add (Vector(260.16705831756127,20.65174773758769, 0.0));
    positionAlloc4->Add (Vector(2.019396451224198,329.3207480517072, 0.0));
    positionAlloc4->Add (Vector(194.11657721836968,373.38281840799806, 0.0));
    positionAlloc4->Add (Vector(226.00797606493387,191.5168080924676, 0.0));
    positionAlloc4->Add (Vector(138.99417914999395,230.90774364737143, 0.0));
    positionAlloc4->Add (Vector(217.37374120896237,77.37905202465191, 0.0));
    positionAlloc4->Add (Vector(206.5008557313394,346.1666831836045, 0.0));
    positionAlloc4->Add (Vector(316.4745743655904,32.22319148609509, 0.0));
    positionAlloc4->Add (Vector(263.92477381801666,391.93124808126953, 0.0));
    positionAlloc4->Add (Vector(163.31479258144617,240.62727553838434, 0.0));
    positionAlloc4->Add (Vector(352.1887929099249,55.044857419143376, 0.0));
    positionAlloc4->Add (Vector(263.28420532665797,158.858865160806, 0.0));
    positionAlloc4->Add (Vector(377.6815910490976,340.38897924357394, 0.0));
    positionAlloc4->Add (Vector(308.36068835463806,329.47793170094116, 0.0));
    positionAlloc4->Add (Vector(306.1119647953471,207.67908416782, 0.0));
    positionAlloc4->Add (Vector(154.31823799091507,173.60875445971212, 0.0));
    positionAlloc4->Add (Vector(250.20850168177816,21.866464684160913, 0.0));
    positionAlloc4->Add (Vector(28.561573966097107,276.423330863892, 0.0));
    positionAlloc4->Add (Vector(255.52487255741644,239.06312270482832, 0.0));
    positionAlloc4->Add (Vector(319.29103558099206,136.71134991423287, 0.0));
    positionAlloc4->Add (Vector(200.59931974366506,323.39494329812885, 0.0));
    positionAlloc4->Add (Vector(138.1100129878248,45.72821986309097, 0.0));
    positionAlloc4->Add (Vector(369.0595209392687,357.113202482178, 0.0));
    positionAlloc4->Add (Vector(203.65329503398857,339.2057065365277, 0.0));
    positionAlloc4->Add (Vector(240.76332719540616,399.70062449570554, 0.0));
    positionAlloc4->Add (Vector(211.8625564176779,308.69288733797333, 0.0));
    positionAlloc4->Add (Vector(201.93571680658403,335.9428141387995, 0.0));
    positionAlloc4->Add (Vector(245.06555007511554,338.2174157220455, 0.0));
    positionAlloc4->Add (Vector(302.47008335282356,290.43100698435074, 0.0));
    positionAlloc4->Add (Vector(79.17280161432912,392.11837117244136, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_16.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_16.flowmon", false, false);
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