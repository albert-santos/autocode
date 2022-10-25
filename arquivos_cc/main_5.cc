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
	  uint16_t numberOfRrhs = 18;
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
    positionAlloc->Add (Vector (356.6666666666667,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,400.0, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (270.0,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (270.0,140.0, 0.0));
    positionAlloc->Add (Vector (270.0,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,270.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (10.0,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (10.0,183.33333333333334, 0.0));
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
    positionAlloc4->Add (Vector(282.8220714699255,133.20339076639658, 0.0));
    positionAlloc4->Add (Vector(230.45954021469802,85.1656950602492, 0.0));
    positionAlloc4->Add (Vector(56.73215761512904,320.95550495505955, 0.0));
    positionAlloc4->Add (Vector(29.449326846593316,213.71801084906613, 0.0));
    positionAlloc4->Add (Vector(265.2658427475033,197.68669037209517, 0.0));
    positionAlloc4->Add (Vector(295.62369944450194,389.3278385215397, 0.0));
    positionAlloc4->Add (Vector(0.39112326358172744,283.24521582344454, 0.0));
    positionAlloc4->Add (Vector(33.52049076815153,205.20024561191542, 0.0));
    positionAlloc4->Add (Vector(323.65312947772054,100.96309422448226, 0.0));
    positionAlloc4->Add (Vector(304.20249339197676,92.64611362947099, 0.0));
    positionAlloc4->Add (Vector(40.230029789058804,179.24383614936565, 0.0));
    positionAlloc4->Add (Vector(285.5287874318876,55.63458421979166, 0.0));
    positionAlloc4->Add (Vector(360.427974140506,171.430852793743, 0.0));
    positionAlloc4->Add (Vector(160.20981201924633,282.67150968912154, 0.0));
    positionAlloc4->Add (Vector(52.25368164581718,115.81613841457545, 0.0));
    positionAlloc4->Add (Vector(95.64846317603299,210.18782909293003, 0.0));
    positionAlloc4->Add (Vector(77.99697113659465,259.9640516965121, 0.0));
    positionAlloc4->Add (Vector(58.20478110128269,374.6419270751422, 0.0));
    positionAlloc4->Add (Vector(126.06011544793749,47.85379829020955, 0.0));
    positionAlloc4->Add (Vector(90.24213271678327,216.20642680754077, 0.0));
    positionAlloc4->Add (Vector(173.12391022105408,342.24925259167685, 0.0));
    positionAlloc4->Add (Vector(365.853592543263,327.83414227271135, 0.0));
    positionAlloc4->Add (Vector(215.2191227373884,335.9904472313139, 0.0));
    positionAlloc4->Add (Vector(355.7279374255118,230.85733485462504, 0.0));
    positionAlloc4->Add (Vector(91.51458157915444,398.1161165260169, 0.0));
    positionAlloc4->Add (Vector(308.8489834524732,206.5395475039313, 0.0));
    positionAlloc4->Add (Vector(81.2741180836646,213.0657324511879, 0.0));
    positionAlloc4->Add (Vector(316.1956541453415,379.6047062625583, 0.0));
    positionAlloc4->Add (Vector(127.42931735191956,312.25850844040514, 0.0));
    positionAlloc4->Add (Vector(291.5622036300009,162.2880589161988, 0.0));
    positionAlloc4->Add (Vector(279.6430628091702,68.34315784953743, 0.0));
    positionAlloc4->Add (Vector(269.94886526865287,375.3909650909687, 0.0));
    positionAlloc4->Add (Vector(275.3506894539514,371.3353517455059, 0.0));
    positionAlloc4->Add (Vector(367.4701161537514,147.25430940685888, 0.0));
    positionAlloc4->Add (Vector(260.9163927787097,48.0900015643674, 0.0));
    positionAlloc4->Add (Vector(126.72106158509497,245.18060829022076, 0.0));
    positionAlloc4->Add (Vector(318.1232252156175,319.63638416073337, 0.0));
    positionAlloc4->Add (Vector(82.60477992022493,320.0611518274351, 0.0));
    positionAlloc4->Add (Vector(355.9010166943807,292.20775466932616, 0.0));
    positionAlloc4->Add (Vector(141.31538263775582,218.09869775812493, 0.0));
    positionAlloc4->Add (Vector(166.7710337489591,207.89396780172441, 0.0));
    positionAlloc4->Add (Vector(390.1632455238851,154.2063284803974, 0.0));
    positionAlloc4->Add (Vector(321.0899848031897,279.2591911690061, 0.0));
    positionAlloc4->Add (Vector(161.21712618782564,80.61227710571205, 0.0));
    positionAlloc4->Add (Vector(28.450854431039254,186.95279229020602, 0.0));
    positionAlloc4->Add (Vector(176.21457876254047,160.1641028662067, 0.0));
    positionAlloc4->Add (Vector(333.10880194619574,234.22293707622305, 0.0));
    positionAlloc4->Add (Vector(240.09960844747638,337.40737299374916, 0.0));
    positionAlloc4->Add (Vector(268.07918700336086,176.06942973594295, 0.0));
    positionAlloc4->Add (Vector(134.37025362792028,113.00631704094299, 0.0));
    positionAlloc4->Add (Vector(179.3976452013474,79.76400481875321, 0.0));
    positionAlloc4->Add (Vector(164.20876240929059,7.9788261234662095, 0.0));
    positionAlloc4->Add (Vector(367.31140888900376,169.5478691581969, 0.0));
    positionAlloc4->Add (Vector(175.86111264536078,380.1089414005985, 0.0));
    positionAlloc4->Add (Vector(215.08024493791663,391.95711882463075, 0.0));
    positionAlloc4->Add (Vector(233.14332572899846,316.6934249935311, 0.0));
    positionAlloc4->Add (Vector(54.307647223147896,280.17736992098065, 0.0));
    positionAlloc4->Add (Vector(195.62662607165163,384.1069970375385, 0.0));
    positionAlloc4->Add (Vector(290.4608050196903,389.92399983666724, 0.0));
    positionAlloc4->Add (Vector(162.13875200124554,44.943599234274735, 0.0));
    positionAlloc4->Add (Vector(12.198546042744995,53.23287286221445, 0.0));
    positionAlloc4->Add (Vector(171.92202070794792,343.7404034925477, 0.0));
    positionAlloc4->Add (Vector(354.6984049492044,77.80451404916478, 0.0));
    positionAlloc4->Add (Vector(209.1607511990512,287.86285238490956, 0.0));
    positionAlloc4->Add (Vector(357.9356806720044,295.88382202531494, 0.0));
    positionAlloc4->Add (Vector(346.4350078347928,212.96051270051439, 0.0));
    positionAlloc4->Add (Vector(266.0949453576871,161.6243927834271, 0.0));
    positionAlloc4->Add (Vector(299.7810061587546,223.31280391820428, 0.0));
    positionAlloc4->Add (Vector(42.406645197762494,275.78408856810285, 0.0));
    positionAlloc4->Add (Vector(271.0480288336566,115.32538598925109, 0.0));
    positionAlloc4->Add (Vector(12.264286056834539,304.31220070346086, 0.0));
    positionAlloc4->Add (Vector(261.8737427825326,67.6040384791405, 0.0));
    positionAlloc4->Add (Vector(220.4572410903863,362.1705350161947, 0.0));
    positionAlloc4->Add (Vector(312.67947591199265,386.30450359501964, 0.0));
    positionAlloc4->Add (Vector(285.360334407866,352.24867839700806, 0.0));
    positionAlloc4->Add (Vector(98.35370316386106,219.36201108808825, 0.0));
    positionAlloc4->Add (Vector(132.1285664382031,379.58725685734856, 0.0));
    positionAlloc4->Add (Vector(225.47083543713663,375.92072406779477, 0.0));
    positionAlloc4->Add (Vector(171.02235597172108,169.61296580022426, 0.0));
    positionAlloc4->Add (Vector(23.629608698373914,181.22880480286562, 0.0));
    positionAlloc4->Add (Vector(280.61349693404037,336.3993339028349, 0.0));
    positionAlloc4->Add (Vector(161.86771441094808,198.35931617534786, 0.0));
    positionAlloc4->Add (Vector(93.24211927365202,143.57440161505878, 0.0));
    positionAlloc4->Add (Vector(319.7950946841134,124.49409970947501, 0.0));
    positionAlloc4->Add (Vector(177.24049551527975,394.7938595592642, 0.0));
    positionAlloc4->Add (Vector(296.4235675312591,366.8339455508314, 0.0));
    positionAlloc4->Add (Vector(169.2800232968225,119.74105503849329, 0.0));
    positionAlloc4->Add (Vector(303.73192941516385,170.78804846186614, 0.0));
    positionAlloc4->Add (Vector(293.3704044218616,138.26973507630527, 0.0));
    positionAlloc4->Add (Vector(2.5971890330120484,236.2061465683803, 0.0));
    positionAlloc4->Add (Vector(144.03707521211206,241.86960815279517, 0.0));
    positionAlloc4->Add (Vector(61.83022333994557,296.86901250219125, 0.0));
    positionAlloc4->Add (Vector(258.64547166400035,225.6557332160955, 0.0));
    positionAlloc4->Add (Vector(112.49971655303912,79.57097415107883, 0.0));
    positionAlloc4->Add (Vector(312.7794380137403,343.4869344830376, 0.0));
    positionAlloc4->Add (Vector(392.74162720350046,134.77957172686757, 0.0));
    positionAlloc4->Add (Vector(198.678206377022,351.5580176670676, 0.0));
    positionAlloc4->Add (Vector(41.58743329653327,139.9774250844101, 0.0));
    positionAlloc4->Add (Vector(355.2684592130964,341.5701637100762, 0.0));
    positionAlloc4->Add (Vector(393.92510371449015,187.3815025872484, 0.0));
    positionAlloc4->Add (Vector(111.69266531577344,81.10917016575372, 0.0));
    positionAlloc4->Add (Vector(357.6989486349911,26.29441654717226, 0.0));
    positionAlloc4->Add (Vector(72.9826097374164,193.11499126307461, 0.0));
    positionAlloc4->Add (Vector(167.7342145228181,376.465022850463, 0.0));
    positionAlloc4->Add (Vector(145.00742172952465,323.44801753726745, 0.0));
    positionAlloc4->Add (Vector(94.05453809342728,202.12234220587274, 0.0));
    positionAlloc4->Add (Vector(290.94081801371374,54.384545331925516, 0.0));
    positionAlloc4->Add (Vector(170.10539173029778,353.45496716815506, 0.0));
    positionAlloc4->Add (Vector(214.24757188597806,214.3142393046138, 0.0));
    positionAlloc4->Add (Vector(30.69378489836172,112.71621853754752, 0.0));
    positionAlloc4->Add (Vector(357.45143732521825,2.5236164157313112, 0.0));
    positionAlloc4->Add (Vector(190.4644238816378,292.1600541647206, 0.0));
    positionAlloc4->Add (Vector(16.01738396323862,132.70028699391437, 0.0));
    positionAlloc4->Add (Vector(275.1784052029959,136.03740719550177, 0.0));
    positionAlloc4->Add (Vector(276.19175103091635,369.08260963843503, 0.0));
    positionAlloc4->Add (Vector(81.54302863813587,139.84232610558936, 0.0));
    positionAlloc4->Add (Vector(137.4797311753677,362.09795209971895, 0.0));
    positionAlloc4->Add (Vector(42.423000356336125,350.91284940892365, 0.0));
    positionAlloc4->Add (Vector(116.27316108280974,176.05862378330946, 0.0));
    positionAlloc4->Add (Vector(325.7349967688574,379.3192955127172, 0.0));
    positionAlloc4->Add (Vector(368.64712153321284,324.30867316720366, 0.0));
    positionAlloc4->Add (Vector(134.64000265817356,306.63776178562273, 0.0));
    positionAlloc4->Add (Vector(304.6793985230755,383.8414590939949, 0.0));
    positionAlloc4->Add (Vector(171.2877527510144,325.37970196769794, 0.0));
    positionAlloc4->Add (Vector(191.08721809833216,325.6877173482723, 0.0));
    positionAlloc4->Add (Vector(293.5943846069965,382.7662534144946, 0.0));
    positionAlloc4->Add (Vector(367.47453597533763,75.83942139814405, 0.0));
    positionAlloc4->Add (Vector(155.34108508707564,70.54989799397973, 0.0));
    positionAlloc4->Add (Vector(11.929640409508746,84.20877511638811, 0.0));
    positionAlloc4->Add (Vector(356.4882407365167,356.51659519168646, 0.0));
    positionAlloc4->Add (Vector(156.1202383258413,127.82587714831197, 0.0));
    positionAlloc4->Add (Vector(37.676296477835834,53.62733013877765, 0.0));
    positionAlloc4->Add (Vector(334.16631329605644,257.90830818449473, 0.0));
    positionAlloc4->Add (Vector(87.62082088862164,207.40243263993108, 0.0));
    positionAlloc4->Add (Vector(233.7880818727391,92.84897090122253, 0.0));
    positionAlloc4->Add (Vector(227.25069337968463,104.2549521782446, 0.0));
    positionAlloc4->Add (Vector(209.45781148549236,116.56947497795325, 0.0));
    positionAlloc4->Add (Vector(52.123272469535564,330.28000033393124, 0.0));
    positionAlloc4->Add (Vector(51.66639854867046,206.01319654907465, 0.0));
    positionAlloc4->Add (Vector(267.78248335618343,81.4782674623435, 0.0));
    positionAlloc4->Add (Vector(293.6829628599624,348.8731587000156, 0.0));
    positionAlloc4->Add (Vector(371.9003974434034,74.12768123277628, 0.0));
    positionAlloc4->Add (Vector(102.0906475167366,207.14750680923788, 0.0));
    positionAlloc4->Add (Vector(279.28837835035034,223.54085377372965, 0.0));
    positionAlloc4->Add (Vector(106.21956063685354,305.16332062862796, 0.0));
    positionAlloc4->Add (Vector(51.589728457612516,47.52833605687541, 0.0));
    positionAlloc4->Add (Vector(37.81770570228633,220.54999427527628, 0.0));
    positionAlloc4->Add (Vector(357.59106600654127,257.7643189590618, 0.0));
    positionAlloc4->Add (Vector(234.10969832818628,13.318707854584844, 0.0));
    positionAlloc4->Add (Vector(158.3999044803312,73.54562567497874, 0.0));
    positionAlloc4->Add (Vector(214.5854254048274,246.60912745563266, 0.0));
    positionAlloc4->Add (Vector(177.3239820960343,147.6789163141898, 0.0));
    positionAlloc4->Add (Vector(395.3314439144832,7.106499428404867, 0.0));
    positionAlloc4->Add (Vector(86.05027712465758,349.3573661637424, 0.0));
    positionAlloc4->Add (Vector(18.560331896463644,50.323209119150604, 0.0));
    positionAlloc4->Add (Vector(218.1825891622126,90.79984406621446, 0.0));
    positionAlloc4->Add (Vector(361.83315500949044,210.30298567170527, 0.0));
    positionAlloc4->Add (Vector(22.230182525894946,174.854444497208, 0.0));
    positionAlloc4->Add (Vector(108.57976324736423,96.39276655666885, 0.0));
    positionAlloc4->Add (Vector(191.67003274676415,80.82748831464568, 0.0));
    positionAlloc4->Add (Vector(130.08718043157162,364.2535557174299, 0.0));
    positionAlloc4->Add (Vector(381.7579327394273,353.26186216081055, 0.0));
    positionAlloc4->Add (Vector(9.19126871303848,113.5889941794388, 0.0));
    positionAlloc4->Add (Vector(318.95290959231454,258.9870955821018, 0.0));
    positionAlloc4->Add (Vector(135.3133663890087,187.92144741876973, 0.0));
    positionAlloc4->Add (Vector(274.82933842310786,301.11247699557475, 0.0));
    positionAlloc4->Add (Vector(91.94127565400798,1.8746958425551075, 0.0));
    positionAlloc4->Add (Vector(134.39880047422184,129.78173095186293, 0.0));
    positionAlloc4->Add (Vector(24.55930356345175,215.22340734895687, 0.0));
    positionAlloc4->Add (Vector(74.07746065666214,217.03791242656757, 0.0));
    positionAlloc4->Add (Vector(326.4511275099994,61.698426967848576, 0.0));
    positionAlloc4->Add (Vector(335.66438346228625,274.3651223101229, 0.0));
    positionAlloc4->Add (Vector(228.9559416834253,136.9157615942779, 0.0));
    positionAlloc4->Add (Vector(201.10938220260527,111.41844516410018, 0.0));
    positionAlloc4->Add (Vector(245.91318586060032,114.22285808406514, 0.0));
    positionAlloc4->Add (Vector(143.8137981279231,192.15191868084247, 0.0));
    positionAlloc4->Add (Vector(262.3222165808572,74.28723959003705, 0.0));
    positionAlloc4->Add (Vector(168.13169983226217,200.2527892445726, 0.0));
    positionAlloc4->Add (Vector(41.256894474370355,242.84274216581588, 0.0));
    positionAlloc4->Add (Vector(376.83866309902027,267.9979619321633, 0.0));
    positionAlloc4->Add (Vector(224.93101034531574,182.09272787001737, 0.0));
    positionAlloc4->Add (Vector(163.95944148362832,58.34503330496559, 0.0));
    positionAlloc4->Add (Vector(56.61081242261896,75.74783621306219, 0.0));
    positionAlloc4->Add (Vector(64.09213267670357,204.0176945651981, 0.0));
    positionAlloc4->Add (Vector(182.99171450719393,307.45384171049557, 0.0));
    positionAlloc4->Add (Vector(108.4104490649028,201.23187937903793, 0.0));
    positionAlloc4->Add (Vector(156.80386397188138,17.90232677220782, 0.0));
    positionAlloc4->Add (Vector(170.8936569718666,38.7233159852725, 0.0));
    positionAlloc4->Add (Vector(66.8136552108726,272.0778130868706, 0.0));
    positionAlloc4->Add (Vector(169.11192237064716,43.30409263811492, 0.0));
    positionAlloc4->Add (Vector(18.898123474344874,321.9809738826648, 0.0));
    positionAlloc4->Add (Vector(154.79762863298507,24.994191346641294, 0.0));
    positionAlloc4->Add (Vector(392.97298180768155,263.32180059950855, 0.0));
    positionAlloc4->Add (Vector(62.37232438603763,14.196151694504033, 0.0));
    positionAlloc4->Add (Vector(21.335251701939217,71.71352296614005, 0.0));
    positionAlloc4->Add (Vector(61.70823763545856,45.0718152534038, 0.0));
    positionAlloc4->Add (Vector(6.625028689715018,219.8714993498962, 0.0));
    positionAlloc4->Add (Vector(148.69622511862536,146.13379680797354, 0.0));
    positionAlloc4->Add (Vector(334.48245532396885,341.09567971514616, 0.0));
    positionAlloc4->Add (Vector(251.05452089503234,124.32673089497675, 0.0));
    positionAlloc4->Add (Vector(3.1234676375357484,100.3617233835135, 0.0));
    positionAlloc4->Add (Vector(332.3304587221251,88.16271438613458, 0.0));
    positionAlloc4->Add (Vector(78.23331289994941,272.3306035638695, 0.0));
    positionAlloc4->Add (Vector(119.3576114403316,196.14466347980724, 0.0));
    positionAlloc4->Add (Vector(42.566837260610455,289.2483258403516, 0.0));
    positionAlloc4->Add (Vector(28.350403035095304,170.02049331363088, 0.0));
    positionAlloc4->Add (Vector(170.94787102693348,90.87014179842879, 0.0));
    positionAlloc4->Add (Vector(134.04267920619915,100.9299736009837, 0.0));
    positionAlloc4->Add (Vector(224.4303562477063,224.83912518407334, 0.0));
    positionAlloc4->Add (Vector(218.01555099432542,399.0190938179581, 0.0));
    positionAlloc4->Add (Vector(119.46519585468192,347.3989200666935, 0.0));
    positionAlloc4->Add (Vector(10.56078220973986,215.86012883979828, 0.0));
    positionAlloc4->Add (Vector(43.198689063711,286.5510710244395, 0.0));
    positionAlloc4->Add (Vector(111.09482790800836,27.86894429184974, 0.0));
    positionAlloc4->Add (Vector(234.6170409695924,52.90843875249234, 0.0));
    positionAlloc4->Add (Vector(236.43131426054978,191.87668474313492, 0.0));
    positionAlloc4->Add (Vector(77.53404505149395,65.1467101169843, 0.0));
    positionAlloc4->Add (Vector(298.3198088940667,318.80264442532166, 0.0));
    positionAlloc4->Add (Vector(377.90907974706505,12.194510474238651, 0.0));
    positionAlloc4->Add (Vector(373.14973065583865,159.18613668030997, 0.0));
    positionAlloc4->Add (Vector(151.6342511312515,218.60993708779404, 0.0));
    positionAlloc4->Add (Vector(390.71588316622467,342.05505600481854, 0.0));
    positionAlloc4->Add (Vector(196.9378042141305,23.641356381949173, 0.0));
    positionAlloc4->Add (Vector(178.27696219007535,237.0958544035167, 0.0));
    positionAlloc4->Add (Vector(235.92603523912095,162.2525512226704, 0.0));
    positionAlloc4->Add (Vector(231.45165982002845,165.64201914654402, 0.0));
    positionAlloc4->Add (Vector(304.79299918172893,214.2275191267328, 0.0));
    positionAlloc4->Add (Vector(64.5848121447278,10.797917169754578, 0.0));
    positionAlloc4->Add (Vector(354.96038584095817,254.95097601733212, 0.0));
    positionAlloc4->Add (Vector(225.84259614734145,224.8244975946323, 0.0));
    positionAlloc4->Add (Vector(102.08250122256688,119.03335465621412, 0.0));
    positionAlloc4->Add (Vector(314.5837742680763,11.18073961591799, 0.0));
    positionAlloc4->Add (Vector(316.98470463362827,29.580433583752395, 0.0));
    positionAlloc4->Add (Vector(74.21281977964287,22.6047101950976, 0.0));
    positionAlloc4->Add (Vector(239.09466399084587,10.97918268063296, 0.0));
    positionAlloc4->Add (Vector(391.51327301022906,344.14857879605677, 0.0));
    positionAlloc4->Add (Vector(396.4848414322277,141.1246940426401, 0.0));
    positionAlloc4->Add (Vector(295.6420333313777,140.16267794288927, 0.0));
    positionAlloc4->Add (Vector(390.4976742351298,71.28448284968067, 0.0));
    positionAlloc4->Add (Vector(283.65618179405584,228.12202964785487, 0.0));
    positionAlloc4->Add (Vector(374.39176100597524,173.62759427252445, 0.0));
    positionAlloc4->Add (Vector(58.05874558280779,190.01181902813678, 0.0));
    positionAlloc4->Add (Vector(110.57456054615167,19.617391546203677, 0.0));
    positionAlloc4->Add (Vector(36.972370749347625,49.39800643421162, 0.0));
    positionAlloc4->Add (Vector(272.5579391505398,106.75658477016245, 0.0));
    positionAlloc4->Add (Vector(51.09521879176455,357.4237496287394, 0.0));
    positionAlloc4->Add (Vector(162.16514637574414,121.89425451526131, 0.0));
    positionAlloc4->Add (Vector(105.3589891304481,164.24040367821195, 0.0));
    positionAlloc4->Add (Vector(29.81354047340168,323.07631812079825, 0.0));
    positionAlloc4->Add (Vector(97.19517811678564,125.11163302306163, 0.0));
    positionAlloc4->Add (Vector(233.2875524000557,53.69976399189018, 0.0));
    positionAlloc4->Add (Vector(31.406806663097562,149.14786844447764, 0.0));
    positionAlloc4->Add (Vector(141.92169754165204,217.200625931524, 0.0));
    positionAlloc4->Add (Vector(144.4647753925781,111.91415690824171, 0.0));
    positionAlloc4->Add (Vector(61.144851758331555,144.20654279728308, 0.0));
    positionAlloc4->Add (Vector(51.700563230995876,354.40807757776207, 0.0));
    positionAlloc4->Add (Vector(394.32712814508733,375.13265400756165, 0.0));
    positionAlloc4->Add (Vector(332.9916842044827,375.65840797466745, 0.0));
    positionAlloc4->Add (Vector(384.2072161490125,8.555796806386073, 0.0));
    positionAlloc4->Add (Vector(186.88925057244137,294.08229737301286, 0.0));
    positionAlloc4->Add (Vector(260.7284285305035,48.752810870430174, 0.0));
    positionAlloc4->Add (Vector(114.22431248581249,185.4175725144773, 0.0));
    positionAlloc4->Add (Vector(121.6087483472455,266.0308203926778, 0.0));
    positionAlloc4->Add (Vector(394.48204198924793,195.56741296844598, 0.0));
    positionAlloc4->Add (Vector(246.23341470910148,3.0193344905861785, 0.0));
    positionAlloc4->Add (Vector(102.8764327367401,310.5365995137606, 0.0));
    positionAlloc4->Add (Vector(392.4607952584394,160.30338859548792, 0.0));
    positionAlloc4->Add (Vector(33.06463406176361,208.54804971814227, 0.0));
    positionAlloc4->Add (Vector(318.68750358164675,111.93823358924102, 0.0));
    positionAlloc4->Add (Vector(154.79591936981006,183.92820278926877, 0.0));
    positionAlloc4->Add (Vector(71.7438177751748,117.93550755715465, 0.0));
    positionAlloc4->Add (Vector(256.676252995586,0.6017880741725268, 0.0));
    positionAlloc4->Add (Vector(368.42072476995645,151.07965602578219, 0.0));
    positionAlloc4->Add (Vector(293.1511386707716,191.59717313127257, 0.0));
    positionAlloc4->Add (Vector(293.4072535621438,391.34931567425645, 0.0));
    positionAlloc4->Add (Vector(69.92961864190565,136.7916806065481, 0.0));
    positionAlloc4->Add (Vector(328.6843755329068,2.781967015785236, 0.0));
    positionAlloc4->Add (Vector(326.0390590554227,292.24386753405173, 0.0));
    positionAlloc4->Add (Vector(301.5232833180388,374.6468672743009, 0.0));
    positionAlloc4->Add (Vector(276.63088285845936,94.32727811131288, 0.0));
    positionAlloc4->Add (Vector(186.67020417806012,258.1591728540357, 0.0));
    positionAlloc4->Add (Vector(281.6592520029274,213.0504667561238, 0.0));
    positionAlloc4->Add (Vector(315.9932683608425,284.218974061184, 0.0));
    positionAlloc4->Add (Vector(362.83526984138587,152.42827934614627, 0.0));
    positionAlloc4->Add (Vector(242.48919996474604,24.244618589607647, 0.0));
    positionAlloc4->Add (Vector(135.43191837385464,115.1778655627031, 0.0));
    positionAlloc4->Add (Vector(66.49929941440979,355.6348318759241, 0.0));
    positionAlloc4->Add (Vector(81.22395122395054,335.9549362660529, 0.0));
    positionAlloc4->Add (Vector(398.64021553596376,309.38131325618195, 0.0));
    positionAlloc4->Add (Vector(350.9780514324127,349.6198592514058, 0.0));
    positionAlloc4->Add (Vector(199.05515380392535,381.9615826608784, 0.0));
    positionAlloc4->Add (Vector(85.01852661926196,159.97984195028044, 0.0));
    positionAlloc4->Add (Vector(185.5328591091642,261.72083209008736, 0.0));
    positionAlloc4->Add (Vector(63.27994873852618,18.58114041749519, 0.0));
    positionAlloc4->Add (Vector(224.7251456335615,186.26827598081, 0.0));
    positionAlloc4->Add (Vector(291.4825514931287,318.1015309676361, 0.0));
    positionAlloc4->Add (Vector(308.9960673048138,86.72587522820953, 0.0));
    positionAlloc4->Add (Vector(273.81580328674363,264.2949154370002, 0.0));
    positionAlloc4->Add (Vector(90.439433893419,173.45744346607955, 0.0));
    positionAlloc4->Add (Vector(161.90812999289764,91.21003852440279, 0.0));
    positionAlloc4->Add (Vector(173.18191628694467,124.56361842334722, 0.0));
    positionAlloc4->Add (Vector(339.06579257503364,398.45047643961357, 0.0));
    positionAlloc4->Add (Vector(5.205067176650058,256.90112164657575, 0.0));
    positionAlloc4->Add (Vector(228.15361057895257,223.3975204359422, 0.0));
    positionAlloc4->Add (Vector(39.66140856864149,345.2378870987817, 0.0));
    positionAlloc4->Add (Vector(350.5902182850285,22.285444141253976, 0.0));
    positionAlloc4->Add (Vector(346.96625284030364,231.40791285605533, 0.0));
    positionAlloc4->Add (Vector(391.90789314464365,28.294853606536606, 0.0));
    positionAlloc4->Add (Vector(230.2144604444005,190.7010479866038, 0.0));
    positionAlloc4->Add (Vector(16.270991968907378,70.29562693766476, 0.0));
    positionAlloc4->Add (Vector(15.741835876306531,37.453134292543226, 0.0));
    positionAlloc4->Add (Vector(181.8178651986488,5.5577595959389825, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_5.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_5.flowmon", false, false);
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