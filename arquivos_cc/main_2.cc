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
	  uint16_t numberOfRrhs = 30;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 552;
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
    positionAlloc->Add (Vector (313.3333333333333,270.0, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (270.0,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (270.0,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (270.0,140.0, 0.0));
    positionAlloc->Add (Vector (270.0,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (270.0,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,140.0, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,270.0, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (140.0,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (140.0,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,140.0, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,270.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,96.66666666666667, 0.0));
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
    positionAlloc4->Add (Vector(212.7167600267554,312.686114158376, 0.0));
    positionAlloc4->Add (Vector(217.01609325791637,352.9944691609486, 0.0));
    positionAlloc4->Add (Vector(58.37156722864138,28.905902725949062, 0.0));
    positionAlloc4->Add (Vector(360.4267973691255,51.097325561437316, 0.0));
    positionAlloc4->Add (Vector(388.24747937357756,371.82015060493626, 0.0));
    positionAlloc4->Add (Vector(64.30565692938393,286.4953225918486, 0.0));
    positionAlloc4->Add (Vector(276.25563231295763,23.027047370042997, 0.0));
    positionAlloc4->Add (Vector(218.6738288363217,137.647734790399, 0.0));
    positionAlloc4->Add (Vector(60.29586364122328,97.50559095847247, 0.0));
    positionAlloc4->Add (Vector(281.4190928805714,366.90406363200344, 0.0));
    positionAlloc4->Add (Vector(351.6794242146382,67.52007603103745, 0.0));
    positionAlloc4->Add (Vector(300.90264192163727,247.10171184402716, 0.0));
    positionAlloc4->Add (Vector(72.29168544119591,277.72277556949143, 0.0));
    positionAlloc4->Add (Vector(273.22561904889875,26.248799584569937, 0.0));
    positionAlloc4->Add (Vector(90.71572826650596,163.2725611409129, 0.0));
    positionAlloc4->Add (Vector(339.0010409081608,136.58963200851946, 0.0));
    positionAlloc4->Add (Vector(145.22932944179928,91.25665371716889, 0.0));
    positionAlloc4->Add (Vector(184.17359738357337,51.12866464255394, 0.0));
    positionAlloc4->Add (Vector(58.069317168038246,133.8722131538373, 0.0));
    positionAlloc4->Add (Vector(258.6599926687162,242.10031420282482, 0.0));
    positionAlloc4->Add (Vector(261.0924803603819,196.2960465777653, 0.0));
    positionAlloc4->Add (Vector(383.6448070076809,221.33757978380828, 0.0));
    positionAlloc4->Add (Vector(137.3680372434689,140.24687572278145, 0.0));
    positionAlloc4->Add (Vector(220.85552302139257,227.25588707687697, 0.0));
    positionAlloc4->Add (Vector(332.3005467617703,61.36837646496494, 0.0));
    positionAlloc4->Add (Vector(92.37668332884965,114.55085261495297, 0.0));
    positionAlloc4->Add (Vector(238.34677824666045,58.199915688683525, 0.0));
    positionAlloc4->Add (Vector(27.252022957629762,220.50265957434064, 0.0));
    positionAlloc4->Add (Vector(153.31212405120502,72.12242433910374, 0.0));
    positionAlloc4->Add (Vector(180.35706260028,139.49070781905837, 0.0));
    positionAlloc4->Add (Vector(80.65270362265373,166.58502246924326, 0.0));
    positionAlloc4->Add (Vector(16.67497700811511,263.98788758555963, 0.0));
    positionAlloc4->Add (Vector(145.66811946141033,226.6582226751595, 0.0));
    positionAlloc4->Add (Vector(127.12702014023911,137.59327071439614, 0.0));
    positionAlloc4->Add (Vector(314.5668329119391,62.21143354944867, 0.0));
    positionAlloc4->Add (Vector(157.99482404070605,318.99839371733947, 0.0));
    positionAlloc4->Add (Vector(194.56982952307445,279.6586261651666, 0.0));
    positionAlloc4->Add (Vector(345.3793761330538,283.3803166061491, 0.0));
    positionAlloc4->Add (Vector(35.254065570173545,346.4309717494878, 0.0));
    positionAlloc4->Add (Vector(40.56831075251068,284.8979217589022, 0.0));
    positionAlloc4->Add (Vector(218.73378232632334,155.70997816358627, 0.0));
    positionAlloc4->Add (Vector(164.91157119142792,244.6176397801286, 0.0));
    positionAlloc4->Add (Vector(344.7623997275332,104.64431707316871, 0.0));
    positionAlloc4->Add (Vector(302.6464225184869,375.1750657239129, 0.0));
    positionAlloc4->Add (Vector(93.96088832084195,149.61868674682785, 0.0));
    positionAlloc4->Add (Vector(264.16477386825613,99.63603538077312, 0.0));
    positionAlloc4->Add (Vector(184.98978608601138,347.8456340484451, 0.0));
    positionAlloc4->Add (Vector(208.33933622151565,296.11745411208864, 0.0));
    positionAlloc4->Add (Vector(69.7284550079349,66.6887731706113, 0.0));
    positionAlloc4->Add (Vector(182.34912294780096,261.3004569045278, 0.0));
    positionAlloc4->Add (Vector(89.2953723761774,322.63403290420564, 0.0));
    positionAlloc4->Add (Vector(17.051872718631955,39.77610290195393, 0.0));
    positionAlloc4->Add (Vector(54.97741984969235,385.74911895444706, 0.0));
    positionAlloc4->Add (Vector(193.7880243794027,354.982031229091, 0.0));
    positionAlloc4->Add (Vector(298.19933229926585,161.36265133366913, 0.0));
    positionAlloc4->Add (Vector(46.11737019201083,83.23896399957889, 0.0));
    positionAlloc4->Add (Vector(343.09544580457623,258.2257633093731, 0.0));
    positionAlloc4->Add (Vector(255.3112533067297,57.20164380732906, 0.0));
    positionAlloc4->Add (Vector(14.392387063456757,143.35809785013004, 0.0));
    positionAlloc4->Add (Vector(144.88644221200445,322.8762302054078, 0.0));
    positionAlloc4->Add (Vector(115.01037600961146,134.64927220491964, 0.0));
    positionAlloc4->Add (Vector(6.035723053191644,12.877761547428213, 0.0));
    positionAlloc4->Add (Vector(106.362816419131,275.79794527106776, 0.0));
    positionAlloc4->Add (Vector(304.53408639683823,60.03938454081998, 0.0));
    positionAlloc4->Add (Vector(64.8979997915231,77.83749409525673, 0.0));
    positionAlloc4->Add (Vector(134.68843589802196,234.57176858570463, 0.0));
    positionAlloc4->Add (Vector(235.5634813224309,337.00728393662354, 0.0));
    positionAlloc4->Add (Vector(104.00758878267693,80.5567414854881, 0.0));
    positionAlloc4->Add (Vector(385.21293551387026,375.3047074009513, 0.0));
    positionAlloc4->Add (Vector(207.7190341881761,127.57434756696964, 0.0));
    positionAlloc4->Add (Vector(30.013055734616323,344.5409897932115, 0.0));
    positionAlloc4->Add (Vector(370.28164347609237,162.22295365560967, 0.0));
    positionAlloc4->Add (Vector(23.811442856028542,192.56018365961455, 0.0));
    positionAlloc4->Add (Vector(10.423973075357296,2.81868952403026, 0.0));
    positionAlloc4->Add (Vector(38.631613771365416,242.41411926758488, 0.0));
    positionAlloc4->Add (Vector(202.30533959457273,314.60030514397397, 0.0));
    positionAlloc4->Add (Vector(266.06734183287733,74.38438791853557, 0.0));
    positionAlloc4->Add (Vector(111.40166873522266,245.91275643331963, 0.0));
    positionAlloc4->Add (Vector(181.0234505545737,372.40127750580115, 0.0));
    positionAlloc4->Add (Vector(10.611340444179662,155.93549934999368, 0.0));
    positionAlloc4->Add (Vector(244.62944324856423,151.34153553409965, 0.0));
    positionAlloc4->Add (Vector(8.431869134204506,394.01822144418776, 0.0));
    positionAlloc4->Add (Vector(46.330698682499616,108.22079751898657, 0.0));
    positionAlloc4->Add (Vector(100.3846150864983,170.59717834248738, 0.0));
    positionAlloc4->Add (Vector(250.4835716548887,39.03304719043379, 0.0));
    positionAlloc4->Add (Vector(190.98443053586124,9.610693852323404, 0.0));
    positionAlloc4->Add (Vector(13.163486637013788,45.36440045233046, 0.0));
    positionAlloc4->Add (Vector(248.3809550321889,53.357612058095995, 0.0));
    positionAlloc4->Add (Vector(111.34373215772642,194.98852065033964, 0.0));
    positionAlloc4->Add (Vector(230.73172129280994,261.59717395109345, 0.0));
    positionAlloc4->Add (Vector(179.0113518365021,340.49506500059897, 0.0));
    positionAlloc4->Add (Vector(362.1509519838692,138.96557312024203, 0.0));
    positionAlloc4->Add (Vector(223.0351260684447,229.42443303083812, 0.0));
    positionAlloc4->Add (Vector(19.061881987871576,179.03165762025526, 0.0));
    positionAlloc4->Add (Vector(79.2929160840615,114.28659814852114, 0.0));
    positionAlloc4->Add (Vector(111.72813424653273,366.35148922692, 0.0));
    positionAlloc4->Add (Vector(58.6434807107636,244.98445145122994, 0.0));
    positionAlloc4->Add (Vector(333.5618709996933,279.31271971674505, 0.0));
    positionAlloc4->Add (Vector(308.5450028805848,228.65413457398054, 0.0));
    positionAlloc4->Add (Vector(369.4462985363043,271.23317364959297, 0.0));
    positionAlloc4->Add (Vector(372.2938932600591,107.61695490076741, 0.0));
    positionAlloc4->Add (Vector(280.92630191792296,250.00623727732844, 0.0));
    positionAlloc4->Add (Vector(114.99924081647505,382.89013694975733, 0.0));
    positionAlloc4->Add (Vector(127.2435009378467,306.5968578624919, 0.0));
    positionAlloc4->Add (Vector(278.75133057472203,362.32336668262093, 0.0));
    positionAlloc4->Add (Vector(153.64569119849065,58.69917229099308, 0.0));
    positionAlloc4->Add (Vector(393.2155934157667,239.37088895520594, 0.0));
    positionAlloc4->Add (Vector(219.95596818879247,6.168728937302337, 0.0));
    positionAlloc4->Add (Vector(265.4410526684374,34.533314726065626, 0.0));
    positionAlloc4->Add (Vector(159.38327861961605,150.8958270122038, 0.0));
    positionAlloc4->Add (Vector(297.9335492129971,118.29021376955247, 0.0));
    positionAlloc4->Add (Vector(163.21885413917147,20.134051863780698, 0.0));
    positionAlloc4->Add (Vector(250.54352128347347,249.4207204182233, 0.0));
    positionAlloc4->Add (Vector(341.6239179737258,108.33796553436663, 0.0));
    positionAlloc4->Add (Vector(76.50882069646805,154.81849304203433, 0.0));
    positionAlloc4->Add (Vector(110.02162042616467,43.22699406893493, 0.0));
    positionAlloc4->Add (Vector(217.04656474868816,353.6224849514744, 0.0));
    positionAlloc4->Add (Vector(141.96100434030936,261.41241306546374, 0.0));
    positionAlloc4->Add (Vector(388.85563484858085,297.0089809406839, 0.0));
    positionAlloc4->Add (Vector(265.2595565857267,176.55169238140033, 0.0));
    positionAlloc4->Add (Vector(354.3292468598886,226.11586814622734, 0.0));
    positionAlloc4->Add (Vector(110.18481066004165,203.76343257550494, 0.0));
    positionAlloc4->Add (Vector(256.6996723334387,149.63207509530244, 0.0));
    positionAlloc4->Add (Vector(277.2778233513552,234.99379174016366, 0.0));
    positionAlloc4->Add (Vector(351.1841674900627,13.853600535088129, 0.0));
    positionAlloc4->Add (Vector(330.0701322265678,12.793673750309553, 0.0));
    positionAlloc4->Add (Vector(215.57262132305374,216.8304618956492, 0.0));
    positionAlloc4->Add (Vector(33.608511919490084,331.8294873832596, 0.0));
    positionAlloc4->Add (Vector(360.83040060993784,158.30010462607225, 0.0));
    positionAlloc4->Add (Vector(343.5163025067859,369.20121630146616, 0.0));
    positionAlloc4->Add (Vector(297.03522307861874,290.84047536147074, 0.0));
    positionAlloc4->Add (Vector(261.8150138491326,341.70328393705864, 0.0));
    positionAlloc4->Add (Vector(343.44468122800504,379.0166204407297, 0.0));
    positionAlloc4->Add (Vector(210.76213841893758,301.5476286484852, 0.0));
    positionAlloc4->Add (Vector(53.12299383216441,303.0281436989685, 0.0));
    positionAlloc4->Add (Vector(371.9972355854005,250.13246196130203, 0.0));
    positionAlloc4->Add (Vector(107.30513165437455,318.69819023603367, 0.0));
    positionAlloc4->Add (Vector(122.1254125793227,5.009134754589306, 0.0));
    positionAlloc4->Add (Vector(208.66372536530875,284.92487825897166, 0.0));
    positionAlloc4->Add (Vector(251.20923144277035,121.50337366074746, 0.0));
    positionAlloc4->Add (Vector(183.08211507216768,301.8488901232527, 0.0));
    positionAlloc4->Add (Vector(369.2402730354655,298.4640563093684, 0.0));
    positionAlloc4->Add (Vector(385.20729583821384,201.59320375951313, 0.0));
    positionAlloc4->Add (Vector(127.59440258807561,48.76792555603507, 0.0));
    positionAlloc4->Add (Vector(165.47629447964053,85.88627826735222, 0.0));
    positionAlloc4->Add (Vector(147.43074790611038,117.09062483725208, 0.0));
    positionAlloc4->Add (Vector(301.3520292502343,112.97121222358903, 0.0));
    positionAlloc4->Add (Vector(361.014622568898,122.864307394038, 0.0));
    positionAlloc4->Add (Vector(254.04998443653528,352.0879908874332, 0.0));
    positionAlloc4->Add (Vector(77.54594386381872,198.04494502447128, 0.0));
    positionAlloc4->Add (Vector(19.04437148262201,62.54277147050065, 0.0));
    positionAlloc4->Add (Vector(297.1685001558788,136.36809965293685, 0.0));
    positionAlloc4->Add (Vector(80.63865376326844,131.00962233931565, 0.0));
    positionAlloc4->Add (Vector(229.9758909111066,323.6866334869378, 0.0));
    positionAlloc4->Add (Vector(120.87260016632166,146.69279266437667, 0.0));
    positionAlloc4->Add (Vector(217.26561900768152,246.29037144521493, 0.0));
    positionAlloc4->Add (Vector(348.1031795690393,218.78732370849633, 0.0));
    positionAlloc4->Add (Vector(1.9488191814424294,267.6631191188547, 0.0));
    positionAlloc4->Add (Vector(58.698951904311826,113.59329018647406, 0.0));
    positionAlloc4->Add (Vector(242.61417220661738,11.913177651824114, 0.0));
    positionAlloc4->Add (Vector(292.4550412795866,218.18846741948778, 0.0));
    positionAlloc4->Add (Vector(358.0550843214293,44.22297610217649, 0.0));
    positionAlloc4->Add (Vector(156.30922946827707,186.89542653329835, 0.0));
    positionAlloc4->Add (Vector(211.1861936618803,26.414233606654935, 0.0));
    positionAlloc4->Add (Vector(367.98143080594355,176.54069812633514, 0.0));
    positionAlloc4->Add (Vector(176.17622399028718,301.6700101820637, 0.0));
    positionAlloc4->Add (Vector(286.6352187178937,282.2321648482676, 0.0));
    positionAlloc4->Add (Vector(199.02575384202697,328.47078595709434, 0.0));
    positionAlloc4->Add (Vector(376.3313657526099,86.88790015640717, 0.0));
    positionAlloc4->Add (Vector(399.439635434592,97.151408647907, 0.0));
    positionAlloc4->Add (Vector(265.2627291527862,95.56398194808482, 0.0));
    positionAlloc4->Add (Vector(293.10794130454667,176.13513071857508, 0.0));
    positionAlloc4->Add (Vector(117.19915345134288,230.99627240541895, 0.0));
    positionAlloc4->Add (Vector(11.708848561245011,222.03470660886978, 0.0));
    positionAlloc4->Add (Vector(109.78964512142832,210.6863441434463, 0.0));
    positionAlloc4->Add (Vector(11.984701009619414,185.01162413688115, 0.0));
    positionAlloc4->Add (Vector(75.6553499755399,157.43336571874477, 0.0));
    positionAlloc4->Add (Vector(144.98612692001015,195.69791418405686, 0.0));
    positionAlloc4->Add (Vector(299.77698830086786,229.07531731696616, 0.0));
    positionAlloc4->Add (Vector(206.08587846482948,191.5293604741887, 0.0));
    positionAlloc4->Add (Vector(157.1839610536813,395.78085233450116, 0.0));
    positionAlloc4->Add (Vector(214.6299277361732,309.97815199909826, 0.0));
    positionAlloc4->Add (Vector(73.67111062435723,255.337332001355, 0.0));
    positionAlloc4->Add (Vector(7.4724023985501375,194.78009856958286, 0.0));
    positionAlloc4->Add (Vector(211.16751901229244,68.86502198237308, 0.0));
    positionAlloc4->Add (Vector(268.0361474991228,81.42415106476064, 0.0));
    positionAlloc4->Add (Vector(249.84944286225647,154.94420885779232, 0.0));
    positionAlloc4->Add (Vector(300.04969922759403,43.89400851426264, 0.0));
    positionAlloc4->Add (Vector(181.58106016260797,248.45967587745844, 0.0));
    positionAlloc4->Add (Vector(41.12734698172633,78.33369346999697, 0.0));
    positionAlloc4->Add (Vector(151.2434347132085,278.3986041781976, 0.0));
    positionAlloc4->Add (Vector(74.0173398547904,25.047920793566192, 0.0));
    positionAlloc4->Add (Vector(177.40153676157405,279.13666241993934, 0.0));
    positionAlloc4->Add (Vector(2.352392577851292,293.9768074735846, 0.0));
    positionAlloc4->Add (Vector(261.30322080978175,76.11339307161748, 0.0));
    positionAlloc4->Add (Vector(200.0373038387718,128.3395606403458, 0.0));
    positionAlloc4->Add (Vector(239.46555264257427,244.59979066384093, 0.0));
    positionAlloc4->Add (Vector(128.08266171451552,58.79125034197252, 0.0));
    positionAlloc4->Add (Vector(1.9905458845061563,77.80656554384073, 0.0));
    positionAlloc4->Add (Vector(232.97594475568903,171.33295921554256, 0.0));
    positionAlloc4->Add (Vector(198.49445558811166,248.18728024207618, 0.0));
    positionAlloc4->Add (Vector(199.19145306931534,264.00608941042043, 0.0));
    positionAlloc4->Add (Vector(111.84847177993956,169.40402124207318, 0.0));
    positionAlloc4->Add (Vector(34.98910487401741,159.90199507234578, 0.0));
    positionAlloc4->Add (Vector(224.5638734873926,293.5692545139506, 0.0));
    positionAlloc4->Add (Vector(1.0030238671692615,158.8911064935047, 0.0));
    positionAlloc4->Add (Vector(326.9529081588503,363.0578584399356, 0.0));
    positionAlloc4->Add (Vector(282.8750439136729,236.3602397961627, 0.0));
    positionAlloc4->Add (Vector(360.1795649544261,348.7942524763674, 0.0));
    positionAlloc4->Add (Vector(112.07705414923748,180.54474449296066, 0.0));
    positionAlloc4->Add (Vector(259.59288905945897,301.3303223577569, 0.0));
    positionAlloc4->Add (Vector(158.7952846689496,64.62961594243595, 0.0));
    positionAlloc4->Add (Vector(192.57835901629576,321.43785057241087, 0.0));
    positionAlloc4->Add (Vector(293.3464377875336,76.96715616549552, 0.0));
    positionAlloc4->Add (Vector(330.8801300557225,145.8093790223553, 0.0));
    positionAlloc4->Add (Vector(253.38433369429026,111.08509298936968, 0.0));
    positionAlloc4->Add (Vector(352.46732737156185,50.4313921588794, 0.0));
    positionAlloc4->Add (Vector(122.95259217113772,121.98625665682528, 0.0));
    positionAlloc4->Add (Vector(370.288214134504,312.47816027030643, 0.0));
    positionAlloc4->Add (Vector(272.44458818336386,94.79282578867934, 0.0));
    positionAlloc4->Add (Vector(287.73550688843017,342.9201333804923, 0.0));
    positionAlloc4->Add (Vector(167.97930176077162,241.11470489691845, 0.0));
    positionAlloc4->Add (Vector(111.50336347737482,125.85904271004473, 0.0));
    positionAlloc4->Add (Vector(320.25646971338244,265.910759404302, 0.0));
    positionAlloc4->Add (Vector(53.162495064730834,113.38497297122285, 0.0));
    positionAlloc4->Add (Vector(188.72762509503872,392.7297671301029, 0.0));
    positionAlloc4->Add (Vector(101.47432275507207,74.46226789466559, 0.0));
    positionAlloc4->Add (Vector(210.49186595665213,200.24782916923564, 0.0));
    positionAlloc4->Add (Vector(90.4380809730196,337.99525494333807, 0.0));
    positionAlloc4->Add (Vector(135.8163475185214,281.7988792992504, 0.0));
    positionAlloc4->Add (Vector(76.03482019197622,187.65377593335083, 0.0));
    positionAlloc4->Add (Vector(268.968309034573,207.1146895253658, 0.0));
    positionAlloc4->Add (Vector(108.78244498111181,120.40317723196252, 0.0));
    positionAlloc4->Add (Vector(54.882401395964564,169.7169497303248, 0.0));
    positionAlloc4->Add (Vector(41.10262554966573,309.0321066526176, 0.0));
    positionAlloc4->Add (Vector(189.18598074400524,169.13672521544524, 0.0));
    positionAlloc4->Add (Vector(150.8869150446026,169.77498649584612, 0.0));
    positionAlloc4->Add (Vector(165.43668712070573,55.72046852949071, 0.0));
    positionAlloc4->Add (Vector(270.5426541805114,92.68125658588784, 0.0));
    positionAlloc4->Add (Vector(89.45801821092712,13.102879898390851, 0.0));
    positionAlloc4->Add (Vector(340.5755627787963,41.85436469798343, 0.0));
    positionAlloc4->Add (Vector(15.8942746303405,59.02753507997902, 0.0));
    positionAlloc4->Add (Vector(91.29964184615437,69.80047502909761, 0.0));
    positionAlloc4->Add (Vector(215.79635637061986,299.71231485087674, 0.0));
    positionAlloc4->Add (Vector(297.3562049608156,94.88562305741905, 0.0));
    positionAlloc4->Add (Vector(96.06949353242666,74.59707530907411, 0.0));
    positionAlloc4->Add (Vector(341.5889659768704,238.33197682348063, 0.0));
    positionAlloc4->Add (Vector(198.1979925444772,329.7838443190734, 0.0));
    positionAlloc4->Add (Vector(128.94299565308427,99.3696985320796, 0.0));
    positionAlloc4->Add (Vector(29.006211218185342,69.08043525290996, 0.0));
    positionAlloc4->Add (Vector(296.5006941327436,379.38336172231743, 0.0));
    positionAlloc4->Add (Vector(168.14782752111404,170.4523121698849, 0.0));
    positionAlloc4->Add (Vector(11.811497976808383,361.3324707267502, 0.0));
    positionAlloc4->Add (Vector(251.18477079276107,121.60219600892125, 0.0));
    positionAlloc4->Add (Vector(206.84821278817066,92.84000873853402, 0.0));
    positionAlloc4->Add (Vector(256.31308178073846,320.532281281277, 0.0));
    positionAlloc4->Add (Vector(113.9728582888579,307.5411393151032, 0.0));
    positionAlloc4->Add (Vector(119.64493786433424,42.153869092015704, 0.0));
    positionAlloc4->Add (Vector(271.68518007828743,205.15005703599653, 0.0));
    positionAlloc4->Add (Vector(166.44030904725682,212.63338635836072, 0.0));
    positionAlloc4->Add (Vector(296.7856942713521,393.1523143683962, 0.0));
    positionAlloc4->Add (Vector(383.2754966859584,273.2683375416586, 0.0));
    positionAlloc4->Add (Vector(349.2333989097742,51.29237553819452, 0.0));
    positionAlloc4->Add (Vector(108.96034005175288,239.51556716368415, 0.0));
    positionAlloc4->Add (Vector(26.835784131542127,79.4593057682246, 0.0));
    positionAlloc4->Add (Vector(169.82396412088664,81.64899999773434, 0.0));
    positionAlloc4->Add (Vector(307.8901379828743,321.1465945729696, 0.0));
    positionAlloc4->Add (Vector(203.9593512572436,137.51524137473777, 0.0));
    positionAlloc4->Add (Vector(384.8312851388374,73.79824179607107, 0.0));
    positionAlloc4->Add (Vector(118.45196939836175,213.16902361266128, 0.0));
    positionAlloc4->Add (Vector(243.42617478942347,100.66383366092948, 0.0));
    positionAlloc4->Add (Vector(230.79113859100065,350.1554953802748, 0.0));
    positionAlloc4->Add (Vector(12.399183931302327,378.691307843181, 0.0));
    positionAlloc4->Add (Vector(73.65102247031933,331.4931406113207, 0.0));
    positionAlloc4->Add (Vector(318.59554685247826,179.00325559612554, 0.0));
    positionAlloc4->Add (Vector(224.71450424398944,294.684383077408, 0.0));
    positionAlloc4->Add (Vector(345.6576379056107,179.15956624811193, 0.0));
    positionAlloc4->Add (Vector(101.69294186294096,33.634541671971796, 0.0));
    positionAlloc4->Add (Vector(304.6919450770967,96.19372992123512, 0.0));
    positionAlloc4->Add (Vector(156.82634757215476,113.54700638936106, 0.0));
    positionAlloc4->Add (Vector(377.4037535644323,346.27440560253365, 0.0));
    positionAlloc4->Add (Vector(60.429807388572776,209.2034126937857, 0.0));
    positionAlloc4->Add (Vector(385.68972993971767,203.65576100654707, 0.0));
    positionAlloc4->Add (Vector(17.724662333241838,93.49374404660425, 0.0));
    positionAlloc4->Add (Vector(70.13303670853533,136.4587293006189, 0.0));
    positionAlloc4->Add (Vector(52.09686702910603,262.0805148898943, 0.0));
    positionAlloc4->Add (Vector(20.786275402144394,53.8185684047769, 0.0));
    positionAlloc4->Add (Vector(127.71909482984753,272.4816736468076, 0.0));
    positionAlloc4->Add (Vector(242.61773930018705,241.57909643187256, 0.0));
    positionAlloc4->Add (Vector(228.40350477482923,385.61445333563273, 0.0));
    positionAlloc4->Add (Vector(326.70158575070667,351.12056745387986, 0.0));
    positionAlloc4->Add (Vector(335.9362840790669,348.0409461230271, 0.0));
    positionAlloc4->Add (Vector(190.9751822368868,186.98395681814776, 0.0));
    positionAlloc4->Add (Vector(167.39872701567026,216.71962911309254, 0.0));
    positionAlloc4->Add (Vector(81.96885831225353,104.56828576233517, 0.0));
    positionAlloc4->Add (Vector(330.7074953620516,258.6199743004608, 0.0));
    positionAlloc4->Add (Vector(52.95259742509497,319.4481233112124, 0.0));
    positionAlloc4->Add (Vector(272.5439019565225,346.85571999963537, 0.0));
    positionAlloc4->Add (Vector(48.90449209200112,267.31559550032074, 0.0));
    positionAlloc4->Add (Vector(212.87261940866986,113.89848344525255, 0.0));
    positionAlloc4->Add (Vector(185.42283638068474,133.90053748546552, 0.0));
    positionAlloc4->Add (Vector(257.38500889632195,18.199286416241513, 0.0));
    positionAlloc4->Add (Vector(247.75686627870388,392.27961454231206, 0.0));
    positionAlloc4->Add (Vector(374.16804560282475,141.0717309006952, 0.0));
    positionAlloc4->Add (Vector(339.748440292262,180.1518569886881, 0.0));
    positionAlloc4->Add (Vector(87.66152230290322,171.5202771656355, 0.0));
    positionAlloc4->Add (Vector(320.0558737547951,107.57442311402596, 0.0));
    positionAlloc4->Add (Vector(362.50415838467273,322.84396066644246, 0.0));
    positionAlloc4->Add (Vector(392.99124740081754,68.83093273155838, 0.0));
    positionAlloc4->Add (Vector(273.0792171613005,5.367324222611813, 0.0));
    positionAlloc4->Add (Vector(281.5997993119554,188.1195699128701, 0.0));
    positionAlloc4->Add (Vector(376.7544984746821,347.96430930734175, 0.0));
    positionAlloc4->Add (Vector(170.16138623025503,324.28849344294804, 0.0));
    positionAlloc4->Add (Vector(170.2897647940465,240.80313538880054, 0.0));
    positionAlloc4->Add (Vector(143.64773556964428,30.124349054327126, 0.0));
    positionAlloc4->Add (Vector(223.4705644254526,339.0884056144043, 0.0));
    positionAlloc4->Add (Vector(60.49117094805921,20.99638554125356, 0.0));
    positionAlloc4->Add (Vector(77.4235830895242,45.58630458367454, 0.0));
    positionAlloc4->Add (Vector(229.55421075693897,176.12768303423726, 0.0));
    positionAlloc4->Add (Vector(184.86323867654284,205.05989870284705, 0.0));
    positionAlloc4->Add (Vector(337.9247276024814,238.1079122693025, 0.0));
    positionAlloc4->Add (Vector(75.43104139978713,176.34887992891635, 0.0));
    positionAlloc4->Add (Vector(155.84480635172207,362.08110637321613, 0.0));
    positionAlloc4->Add (Vector(32.48663437709332,399.365423436363, 0.0));
    positionAlloc4->Add (Vector(46.9507681252519,306.9694070965813, 0.0));
    positionAlloc4->Add (Vector(5.827492705332693,86.08383463777103, 0.0));
    positionAlloc4->Add (Vector(322.94610555092817,387.8152048399586, 0.0));
    positionAlloc4->Add (Vector(378.42201193902525,80.07442629572883, 0.0));
    positionAlloc4->Add (Vector(165.4680656805441,273.7238487325279, 0.0));
    positionAlloc4->Add (Vector(240.57336387951526,290.383538339526, 0.0));
    positionAlloc4->Add (Vector(186.5065896666294,358.25672103977865, 0.0));
    positionAlloc4->Add (Vector(392.1331540549311,88.9532774124703, 0.0));
    positionAlloc4->Add (Vector(315.2198204689229,241.14613501535894, 0.0));
    positionAlloc4->Add (Vector(263.42721662334475,72.47815278154465, 0.0));
    positionAlloc4->Add (Vector(181.35097060887935,210.28463653656385, 0.0));
    positionAlloc4->Add (Vector(187.02002122613584,266.77701727759006, 0.0));
    positionAlloc4->Add (Vector(306.7449505306168,253.71230110954937, 0.0));
    positionAlloc4->Add (Vector(186.52359568373654,196.4034116531717, 0.0));
    positionAlloc4->Add (Vector(247.20452973865784,92.39597946788156, 0.0));
    positionAlloc4->Add (Vector(62.39009321736897,215.87167806450975, 0.0));
    positionAlloc4->Add (Vector(78.77921556560668,392.4319028961189, 0.0));
    positionAlloc4->Add (Vector(148.9995065621438,247.61585389018833, 0.0));
    positionAlloc4->Add (Vector(369.0554778754685,17.66191841274498, 0.0));
    positionAlloc4->Add (Vector(15.03354059428128,59.00100215877417, 0.0));
    positionAlloc4->Add (Vector(328.08098652613995,199.7364405191338, 0.0));
    positionAlloc4->Add (Vector(60.27599689930847,338.2070702122826, 0.0));
    positionAlloc4->Add (Vector(226.29285865170533,363.5232129200228, 0.0));
    positionAlloc4->Add (Vector(281.17269284574854,73.97400894936067, 0.0));
    positionAlloc4->Add (Vector(163.17999561894698,349.8990479617454, 0.0));
    positionAlloc4->Add (Vector(247.8727968215451,180.44098484830994, 0.0));
    positionAlloc4->Add (Vector(325.3175381300223,323.0049086572128, 0.0));
    positionAlloc4->Add (Vector(209.6742458274645,263.008563081244, 0.0));
    positionAlloc4->Add (Vector(259.78672318189854,286.330781143272, 0.0));
    positionAlloc4->Add (Vector(232.3936290700078,101.19204508807536, 0.0));
    positionAlloc4->Add (Vector(222.0901274125378,37.9824279209966, 0.0));
    positionAlloc4->Add (Vector(308.052875039923,42.20283199893391, 0.0));
    positionAlloc4->Add (Vector(248.92501147664711,92.67787655794395, 0.0));
    positionAlloc4->Add (Vector(97.17885030832045,365.53755009864324, 0.0));
    positionAlloc4->Add (Vector(217.51499666553008,250.76640667213383, 0.0));
    positionAlloc4->Add (Vector(289.34441775772245,43.598013571857486, 0.0));
    positionAlloc4->Add (Vector(384.15885928800725,1.4219041857704173, 0.0));
    positionAlloc4->Add (Vector(100.48127333975465,262.3043071613464, 0.0));
    positionAlloc4->Add (Vector(158.25930890701954,308.6873929661833, 0.0));
    positionAlloc4->Add (Vector(220.23182360314482,246.1908284458631, 0.0));
    positionAlloc4->Add (Vector(233.8766452018103,213.48310185371054, 0.0));
    positionAlloc4->Add (Vector(184.71976407962458,146.10978448283748, 0.0));
    positionAlloc4->Add (Vector(295.19819856153634,289.6339379456296, 0.0));
    positionAlloc4->Add (Vector(94.20878264394771,118.37402075496453, 0.0));
    positionAlloc4->Add (Vector(4.635835763958074,151.9057732135979, 0.0));
    positionAlloc4->Add (Vector(323.4861392741159,360.13600180426477, 0.0));
    positionAlloc4->Add (Vector(133.44409153915907,381.68020297183557, 0.0));
    positionAlloc4->Add (Vector(391.15215734398635,100.3092626692009, 0.0));
    positionAlloc4->Add (Vector(308.3327322215321,168.2852796100767, 0.0));
    positionAlloc4->Add (Vector(214.40102992872485,360.3132458827664, 0.0));
    positionAlloc4->Add (Vector(323.36454264245674,52.27712975895522, 0.0));
    positionAlloc4->Add (Vector(347.0508838460695,339.9487525761329, 0.0));
    positionAlloc4->Add (Vector(334.7008199415325,220.82235248990511, 0.0));
    positionAlloc4->Add (Vector(140.64996254417704,367.211327634472, 0.0));
    positionAlloc4->Add (Vector(255.6739023472984,299.4081280359015, 0.0));
    positionAlloc4->Add (Vector(350.1615449117287,379.705773508916, 0.0));
    positionAlloc4->Add (Vector(191.16953968662375,221.8073993112668, 0.0));
    positionAlloc4->Add (Vector(374.11350800272027,345.43718196724103, 0.0));
    positionAlloc4->Add (Vector(166.62669160589897,192.86156886298699, 0.0));
    positionAlloc4->Add (Vector(263.6142638663533,151.53246746469273, 0.0));
    positionAlloc4->Add (Vector(375.4876693475724,339.050683695755, 0.0));
    positionAlloc4->Add (Vector(287.57579523593745,43.990388978128166, 0.0));
    positionAlloc4->Add (Vector(261.53607967850195,177.50791708988896, 0.0));
    positionAlloc4->Add (Vector(17.528603890858776,285.2655892639258, 0.0));
    positionAlloc4->Add (Vector(79.27969172807674,88.23763273893431, 0.0));
    positionAlloc4->Add (Vector(211.0486033825397,141.2969623932236, 0.0));
    positionAlloc4->Add (Vector(44.31817587838247,259.0211943265709, 0.0));
    positionAlloc4->Add (Vector(158.14018951787614,382.15599570286986, 0.0));
    positionAlloc4->Add (Vector(19.758553853325722,60.18201134706538, 0.0));
    positionAlloc4->Add (Vector(27.914391401683414,16.751028119998512, 0.0));
    positionAlloc4->Add (Vector(120.25291540949925,327.74335386240807, 0.0));
    positionAlloc4->Add (Vector(158.58507602104956,251.2092927020913, 0.0));
    positionAlloc4->Add (Vector(112.38221479614698,240.85865085461808, 0.0));
    positionAlloc4->Add (Vector(220.79801086167885,385.5663554738204, 0.0));
    positionAlloc4->Add (Vector(368.01816216388545,370.7364871608966, 0.0));
    positionAlloc4->Add (Vector(289.2373598664039,20.444361530446507, 0.0));
    positionAlloc4->Add (Vector(293.8340056201483,256.32304961834984, 0.0));
    positionAlloc4->Add (Vector(395.8827815146967,185.95505236472894, 0.0));
    positionAlloc4->Add (Vector(117.92480471939814,16.629693579666906, 0.0));
    positionAlloc4->Add (Vector(399.15371128996094,173.4758957619908, 0.0));
    positionAlloc4->Add (Vector(345.13559223144625,141.01302752202471, 0.0));
    positionAlloc4->Add (Vector(361.1667333897636,214.15235669220868, 0.0));
    positionAlloc4->Add (Vector(116.32246201307117,366.36631600398283, 0.0));
    positionAlloc4->Add (Vector(321.2832777312033,101.57354235109408, 0.0));
    positionAlloc4->Add (Vector(247.6853191732956,71.18893418806778, 0.0));
    positionAlloc4->Add (Vector(156.36748460058323,85.78594845332792, 0.0));
    positionAlloc4->Add (Vector(373.1055631859433,372.3419871039872, 0.0));
    positionAlloc4->Add (Vector(394.498712974518,11.297048065165072, 0.0));
    positionAlloc4->Add (Vector(206.15747725724853,159.9783695940527, 0.0));
    positionAlloc4->Add (Vector(213.45963043349235,295.7551149385528, 0.0));
    positionAlloc4->Add (Vector(33.27595547879909,310.79367681095215, 0.0));
    positionAlloc4->Add (Vector(319.86433500993553,200.95800235930335, 0.0));
    positionAlloc4->Add (Vector(99.7848826805551,87.70716413002431, 0.0));
    positionAlloc4->Add (Vector(103.68725026029479,130.73227394655004, 0.0));
    positionAlloc4->Add (Vector(226.22169415215242,196.96961895932782, 0.0));
    positionAlloc4->Add (Vector(13.057546385036956,290.06666353308333, 0.0));
    positionAlloc4->Add (Vector(247.64785330066422,394.1779659779322, 0.0));
    positionAlloc4->Add (Vector(103.15719377081595,373.77819701183387, 0.0));
    positionAlloc4->Add (Vector(183.4203243610004,372.25775686617936, 0.0));
    positionAlloc4->Add (Vector(324.2516413787606,343.99943663213793, 0.0));
    positionAlloc4->Add (Vector(124.51843298246685,319.5891361446372, 0.0));
    positionAlloc4->Add (Vector(17.380616685990713,379.429107350403, 0.0));
    positionAlloc4->Add (Vector(242.16880309081924,390.49530582415537, 0.0));
    positionAlloc4->Add (Vector(351.6265605433862,389.4075443345174, 0.0));
    positionAlloc4->Add (Vector(15.76460487377771,96.6233669363456, 0.0));
    positionAlloc4->Add (Vector(50.57235611587401,311.32035602705105, 0.0));
    positionAlloc4->Add (Vector(42.10501066511476,69.090985329152, 0.0));
    positionAlloc4->Add (Vector(36.163886277919445,51.59039365691043, 0.0));
    positionAlloc4->Add (Vector(361.174360848684,234.96800769503076, 0.0));
    positionAlloc4->Add (Vector(156.1058818398324,303.930158686439, 0.0));
    positionAlloc4->Add (Vector(284.2958279598345,296.95337188284066, 0.0));
    positionAlloc4->Add (Vector(96.03806166448403,252.6482981000641, 0.0));
    positionAlloc4->Add (Vector(84.68853393939449,332.0626104649197, 0.0));
    positionAlloc4->Add (Vector(300.18141868932963,300.4359229291224, 0.0));
    positionAlloc4->Add (Vector(85.78711233227705,375.76754836246374, 0.0));
    positionAlloc4->Add (Vector(376.999549690483,245.42001062405419, 0.0));
    positionAlloc4->Add (Vector(117.92835181478547,364.7052694791195, 0.0));
    positionAlloc4->Add (Vector(260.403043723398,148.4588737850918, 0.0));
    positionAlloc4->Add (Vector(250.17415132457316,103.2518597127484, 0.0));
    positionAlloc4->Add (Vector(285.5712129067845,132.04622087962971, 0.0));
    positionAlloc4->Add (Vector(394.6709779872331,26.628321252951714, 0.0));
    positionAlloc4->Add (Vector(99.24458562406993,338.2459848924582, 0.0));
    positionAlloc4->Add (Vector(92.58517421369912,170.72853977929424, 0.0));
    positionAlloc4->Add (Vector(260.15925299364795,237.71634143466576, 0.0));
    positionAlloc4->Add (Vector(131.6584007177199,211.20055865505046, 0.0));
    positionAlloc4->Add (Vector(282.37094670588766,357.9758080611697, 0.0));
    positionAlloc4->Add (Vector(80.98521426981424,341.69518916655306, 0.0));
    positionAlloc4->Add (Vector(221.37348249930628,33.92072866414115, 0.0));
    positionAlloc4->Add (Vector(191.895026819662,378.670671695859, 0.0));
    positionAlloc4->Add (Vector(47.589334869205935,360.2968860286932, 0.0));
    positionAlloc4->Add (Vector(316.74555023386864,276.96578486683023, 0.0));
    positionAlloc4->Add (Vector(67.09467402010864,203.39280537184638, 0.0));
    positionAlloc4->Add (Vector(272.82650337312595,94.58786053046215, 0.0));
    positionAlloc4->Add (Vector(124.40522715393661,219.3956358664416, 0.0));
    positionAlloc4->Add (Vector(61.63166873294297,358.03472345945937, 0.0));
    positionAlloc4->Add (Vector(159.48621745160335,386.6449970568568, 0.0));
    positionAlloc4->Add (Vector(264.1830910444399,231.26956736341526, 0.0));
    positionAlloc4->Add (Vector(338.42183969458773,371.0323770390174, 0.0));
    positionAlloc4->Add (Vector(242.97675740582144,260.3325026702788, 0.0));
    positionAlloc4->Add (Vector(124.64556538428187,129.39167879563013, 0.0));
    positionAlloc4->Add (Vector(259.960461235876,14.790027343743128, 0.0));
    positionAlloc4->Add (Vector(167.0529387256733,67.68567387747622, 0.0));
    positionAlloc4->Add (Vector(392.564186301645,243.80077030939864, 0.0));
    positionAlloc4->Add (Vector(11.084696048945997,34.15605564926825, 0.0));
    positionAlloc4->Add (Vector(145.74619194281118,356.909571710469, 0.0));
    positionAlloc4->Add (Vector(73.4957647504007,342.46369519494874, 0.0));
    positionAlloc4->Add (Vector(262.8161316098917,337.329285567809, 0.0));
    positionAlloc4->Add (Vector(0.4131730630938524,190.43564720084194, 0.0));
    positionAlloc4->Add (Vector(306.4753460811287,376.4053067275605, 0.0));
    positionAlloc4->Add (Vector(351.46439052632445,101.84030226423077, 0.0));
    positionAlloc4->Add (Vector(242.35854448714318,237.93885336316706, 0.0));
    positionAlloc4->Add (Vector(115.19475170990061,125.76688895629849, 0.0));
    positionAlloc4->Add (Vector(162.51537107532604,297.4996785404878, 0.0));
    positionAlloc4->Add (Vector(384.6290169738135,292.0330230961041, 0.0));
    positionAlloc4->Add (Vector(358.29164341148163,379.9385555520349, 0.0));
    positionAlloc4->Add (Vector(268.85776386273943,75.83496007663429, 0.0));
    positionAlloc4->Add (Vector(34.901009548792715,148.42557800647563, 0.0));
    positionAlloc4->Add (Vector(250.04842732219586,163.89250445360818, 0.0));
    positionAlloc4->Add (Vector(43.125560317836964,33.71322057462272, 0.0));
    positionAlloc4->Add (Vector(227.26101717964204,266.4752360893981, 0.0));
    positionAlloc4->Add (Vector(89.79739153438308,306.07358269340324, 0.0));
    positionAlloc4->Add (Vector(33.41368282469644,247.6539325679629, 0.0));
    positionAlloc4->Add (Vector(382.10490141527475,32.301563124831524, 0.0));
    positionAlloc4->Add (Vector(103.91667732176582,232.49413691646868, 0.0));
    positionAlloc4->Add (Vector(387.9482281378856,296.7758101824307, 0.0));
    positionAlloc4->Add (Vector(112.73803525729717,71.3979331909953, 0.0));
    positionAlloc4->Add (Vector(202.3117547159466,329.9434249540928, 0.0));
    positionAlloc4->Add (Vector(208.1254543453266,331.38725923418855, 0.0));
    positionAlloc4->Add (Vector(276.7935618096875,273.16274705194996, 0.0));
    positionAlloc4->Add (Vector(223.1301214859642,202.4524515312939, 0.0));
    positionAlloc4->Add (Vector(358.9993129211281,181.53198019600492, 0.0));
    positionAlloc4->Add (Vector(267.8586399502132,323.1115449260306, 0.0));
    positionAlloc4->Add (Vector(322.39397291586636,347.39524652110885, 0.0));
    positionAlloc4->Add (Vector(10.863531730344889,320.3437420520133, 0.0));
    positionAlloc4->Add (Vector(344.1826669347541,135.38055364730002, 0.0));
    positionAlloc4->Add (Vector(222.9072920299655,362.84362208814827, 0.0));
    positionAlloc4->Add (Vector(48.16035074308935,4.279601257669663, 0.0));
    positionAlloc4->Add (Vector(35.27545822860403,273.48413432865516, 0.0));
    positionAlloc4->Add (Vector(35.175473508241325,347.990472108643, 0.0));
    positionAlloc4->Add (Vector(116.92550481571348,78.37277859959686, 0.0));
    positionAlloc4->Add (Vector(58.31417248591082,97.95225550475344, 0.0));
    positionAlloc4->Add (Vector(128.65925824111108,312.08093484790345, 0.0));
    positionAlloc4->Add (Vector(269.79953766490297,220.5875343251106, 0.0));
    positionAlloc4->Add (Vector(106.20480698895433,6.177452777104353, 0.0));
    positionAlloc4->Add (Vector(63.21273398552507,5.912834696837166, 0.0));
    positionAlloc4->Add (Vector(187.21982194658912,251.9412086333691, 0.0));
    positionAlloc4->Add (Vector(339.9722140925676,173.8619176286694, 0.0));
    positionAlloc4->Add (Vector(115.704198193164,379.10562644064277, 0.0));
    positionAlloc4->Add (Vector(374.06124621695545,355.41684134249124, 0.0));
    positionAlloc4->Add (Vector(23.19032534910681,55.526607218269454, 0.0));
    positionAlloc4->Add (Vector(328.5786960905189,92.39542371265333, 0.0));
    positionAlloc4->Add (Vector(395.9533813348462,70.97181474730476, 0.0));
    positionAlloc4->Add (Vector(96.06888288855089,274.57736606894565, 0.0));
    positionAlloc4->Add (Vector(102.66439598119051,156.57214519370743, 0.0));
    positionAlloc4->Add (Vector(186.053577407231,322.6987243099454, 0.0));
    positionAlloc4->Add (Vector(23.742890135582996,197.0890404664133, 0.0));
    positionAlloc4->Add (Vector(264.75534713471905,173.53593086109038, 0.0));
    positionAlloc4->Add (Vector(57.27591268350514,252.93194986659864, 0.0));
    positionAlloc4->Add (Vector(193.72630921295425,387.078098384479, 0.0));
    positionAlloc4->Add (Vector(385.56710696959914,257.97666900488474, 0.0));
    positionAlloc4->Add (Vector(378.9503671815837,197.52986299643425, 0.0));
    positionAlloc4->Add (Vector(176.84044425070633,74.12705837507376, 0.0));
    positionAlloc4->Add (Vector(220.57262946934898,74.11569574742094, 0.0));
    positionAlloc4->Add (Vector(113.41780544146181,109.85369936954518, 0.0));
    positionAlloc4->Add (Vector(373.2623051292902,374.3012830768532, 0.0));
    positionAlloc4->Add (Vector(375.211417262541,74.15220985912092, 0.0));
    positionAlloc4->Add (Vector(374.8321539195857,321.93433840631786, 0.0));
    positionAlloc4->Add (Vector(114.81757846220346,53.28284550044078, 0.0));
    positionAlloc4->Add (Vector(253.283706322591,141.35773880065176, 0.0));
    positionAlloc4->Add (Vector(281.00921358456037,198.44958049994443, 0.0));
    positionAlloc4->Add (Vector(236.21860889742683,152.213630883856, 0.0));
    positionAlloc4->Add (Vector(117.8768166243617,107.86188046362058, 0.0));
    positionAlloc4->Add (Vector(111.3312523387608,255.9898276435649, 0.0));
    positionAlloc4->Add (Vector(196.0781248976927,18.19285826811763, 0.0));
    positionAlloc4->Add (Vector(19.015037691104197,4.068982504710261, 0.0));
    positionAlloc4->Add (Vector(101.13659283072604,89.53986813101014, 0.0));
    positionAlloc4->Add (Vector(393.65061755396675,3.949190143567405, 0.0));
    positionAlloc4->Add (Vector(206.4793844308276,345.2905926855398, 0.0));
    positionAlloc4->Add (Vector(251.84318268756525,13.772506122838246, 0.0));
    positionAlloc4->Add (Vector(349.32925361469285,97.85362628768324, 0.0));
    positionAlloc4->Add (Vector(276.4109366226817,229.5211476661412, 0.0));
    positionAlloc4->Add (Vector(66.12739619209593,78.87853858666772, 0.0));
    positionAlloc4->Add (Vector(177.73926297845088,33.509981538184206, 0.0));
    positionAlloc4->Add (Vector(138.04636591203248,63.96751543571328, 0.0));
    positionAlloc4->Add (Vector(252.79798018525486,84.25127136752097, 0.0));
    positionAlloc4->Add (Vector(62.4598257763179,351.88885740055235, 0.0));
    positionAlloc4->Add (Vector(356.2209145513244,369.00179890415734, 0.0));
    positionAlloc4->Add (Vector(113.84958758749417,362.9584568963989, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_2.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_2.flowmon", false, false);
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