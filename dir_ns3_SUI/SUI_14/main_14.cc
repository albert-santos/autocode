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
	  uint16_t numberOfNodes = 736;
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
    positionAlloc->Add (Vector (356.6666666666667,270.0, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,140.0, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,270.0, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,140.0, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (270.0,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (270.0,270.0, 0.0));
    positionAlloc->Add (Vector (270.0,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (270.0,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,140.0, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,10.0, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (140.0,270.0, 0.0));
    positionAlloc->Add (Vector (140.0,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (140.0,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,140.0, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (10.0,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (10.0,183.33333333333334, 0.0));
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
    positionAlloc4->Add (Vector(280.6262052997969,241.37295511872435, 0.0));
    positionAlloc4->Add (Vector(231.61065226677104,231.04732070693217, 0.0));
    positionAlloc4->Add (Vector(329.8735194271236,96.33867975113945, 0.0));
    positionAlloc4->Add (Vector(315.8031930725934,282.7793918849145, 0.0));
    positionAlloc4->Add (Vector(333.2345419217151,197.9123047591155, 0.0));
    positionAlloc4->Add (Vector(76.35877953498857,69.00112226233483, 0.0));
    positionAlloc4->Add (Vector(293.1826035855074,14.875037953709835, 0.0));
    positionAlloc4->Add (Vector(320.4210728215934,327.21248589928547, 0.0));
    positionAlloc4->Add (Vector(363.0405590558856,249.3269135897215, 0.0));
    positionAlloc4->Add (Vector(79.48935748729724,239.13168414197625, 0.0));
    positionAlloc4->Add (Vector(347.84241748278816,141.33936336739978, 0.0));
    positionAlloc4->Add (Vector(261.0939330821171,206.10678000818763, 0.0));
    positionAlloc4->Add (Vector(248.4611966439977,137.85798230793696, 0.0));
    positionAlloc4->Add (Vector(323.09120265695867,284.2878276714542, 0.0));
    positionAlloc4->Add (Vector(157.3565926386527,99.47495826842747, 0.0));
    positionAlloc4->Add (Vector(6.653005040859705,321.5594488080272, 0.0));
    positionAlloc4->Add (Vector(229.13854826745848,56.176475482923124, 0.0));
    positionAlloc4->Add (Vector(168.12030913132753,82.84349485195834, 0.0));
    positionAlloc4->Add (Vector(114.95721332081308,79.01272353784417, 0.0));
    positionAlloc4->Add (Vector(22.359236272921645,174.0007754809808, 0.0));
    positionAlloc4->Add (Vector(8.34598708342229,181.51406638813938, 0.0));
    positionAlloc4->Add (Vector(143.36484392715087,0.9971446351348501, 0.0));
    positionAlloc4->Add (Vector(217.02477221907213,71.73703662797428, 0.0));
    positionAlloc4->Add (Vector(209.14794256010288,127.47183827342599, 0.0));
    positionAlloc4->Add (Vector(112.05783627015168,119.98392033554043, 0.0));
    positionAlloc4->Add (Vector(3.6647437126302584,149.77412415500626, 0.0));
    positionAlloc4->Add (Vector(203.62865763545793,245.46981879851745, 0.0));
    positionAlloc4->Add (Vector(127.78055697285437,174.1181861182204, 0.0));
    positionAlloc4->Add (Vector(334.982867883261,344.17352359630814, 0.0));
    positionAlloc4->Add (Vector(105.93479847793921,274.6890299647241, 0.0));
    positionAlloc4->Add (Vector(2.0087188958608504,353.2768425489037, 0.0));
    positionAlloc4->Add (Vector(189.7610597436933,342.42368394356487, 0.0));
    positionAlloc4->Add (Vector(187.43749207082368,376.0330067102501, 0.0));
    positionAlloc4->Add (Vector(145.93506510754773,296.96326248129924, 0.0));
    positionAlloc4->Add (Vector(90.49779909193401,344.15850105425034, 0.0));
    positionAlloc4->Add (Vector(225.67720009333993,347.6586269883519, 0.0));
    positionAlloc4->Add (Vector(259.4094339132504,358.3271222819562, 0.0));
    positionAlloc4->Add (Vector(289.5220653600302,278.2102550802241, 0.0));
    positionAlloc4->Add (Vector(212.34608946818,13.462895526479324, 0.0));
    positionAlloc4->Add (Vector(7.23485522653422,196.22842932219652, 0.0));
    positionAlloc4->Add (Vector(314.7352741901369,371.81051418617415, 0.0));
    positionAlloc4->Add (Vector(115.55397230113718,348.4107068086927, 0.0));
    positionAlloc4->Add (Vector(187.85767318649684,64.39861567950204, 0.0));
    positionAlloc4->Add (Vector(302.9854492177643,216.28256889500128, 0.0));
    positionAlloc4->Add (Vector(188.6555332681925,206.33908877867944, 0.0));
    positionAlloc4->Add (Vector(374.9380333912476,203.5251343900944, 0.0));
    positionAlloc4->Add (Vector(129.88852468924384,324.25056040513465, 0.0));
    positionAlloc4->Add (Vector(184.2141098576479,187.48400737612454, 0.0));
    positionAlloc4->Add (Vector(264.5769669841355,112.95643069270862, 0.0));
    positionAlloc4->Add (Vector(395.5368410253322,185.73623734057202, 0.0));
    positionAlloc4->Add (Vector(14.741893075569923,328.5138887523806, 0.0));
    positionAlloc4->Add (Vector(318.65589152904016,86.70450212598544, 0.0));
    positionAlloc4->Add (Vector(57.56969563868437,100.17575073945922, 0.0));
    positionAlloc4->Add (Vector(302.9295052322123,50.3569001102711, 0.0));
    positionAlloc4->Add (Vector(51.10686564673239,291.99725306166084, 0.0));
    positionAlloc4->Add (Vector(147.3012381672147,28.232601027359117, 0.0));
    positionAlloc4->Add (Vector(334.0665840323007,276.55552147306486, 0.0));
    positionAlloc4->Add (Vector(109.640400539333,368.9339199677642, 0.0));
    positionAlloc4->Add (Vector(83.70769154041452,114.82979272771452, 0.0));
    positionAlloc4->Add (Vector(263.1010277760025,11.022694272430833, 0.0));
    positionAlloc4->Add (Vector(276.0116416392134,347.9260986241214, 0.0));
    positionAlloc4->Add (Vector(135.81327332419986,102.23545358790295, 0.0));
    positionAlloc4->Add (Vector(109.78521256962317,369.00353897569016, 0.0));
    positionAlloc4->Add (Vector(125.90531754645879,341.090180164915, 0.0));
    positionAlloc4->Add (Vector(291.01031093902395,145.7811414829258, 0.0));
    positionAlloc4->Add (Vector(342.6016478095789,13.701291325870235, 0.0));
    positionAlloc4->Add (Vector(220.6319378726208,156.46217761410216, 0.0));
    positionAlloc4->Add (Vector(6.811392310404063,292.2534646854957, 0.0));
    positionAlloc4->Add (Vector(140.51820525046548,383.8294467062951, 0.0));
    positionAlloc4->Add (Vector(126.24427333437,213.7996229650771, 0.0));
    positionAlloc4->Add (Vector(53.30497165879344,290.0842493216154, 0.0));
    positionAlloc4->Add (Vector(388.1638305234666,337.3358495639718, 0.0));
    positionAlloc4->Add (Vector(63.671865252250996,316.1417243604135, 0.0));
    positionAlloc4->Add (Vector(341.0038081738267,273.7448859186558, 0.0));
    positionAlloc4->Add (Vector(161.12381322209455,298.03255284895766, 0.0));
    positionAlloc4->Add (Vector(367.5185825052187,392.17714187319234, 0.0));
    positionAlloc4->Add (Vector(361.5036438762429,184.30350349822788, 0.0));
    positionAlloc4->Add (Vector(152.3054179422097,180.17709588302816, 0.0));
    positionAlloc4->Add (Vector(345.09575724758855,320.47404147741065, 0.0));
    positionAlloc4->Add (Vector(282.5307341505538,43.96785591052903, 0.0));
    positionAlloc4->Add (Vector(246.69262647968247,68.32486765167101, 0.0));
    positionAlloc4->Add (Vector(294.36288884229003,268.43127954466104, 0.0));
    positionAlloc4->Add (Vector(8.87721943548283,120.83325552604332, 0.0));
    positionAlloc4->Add (Vector(218.42860405481895,144.4135850288004, 0.0));
    positionAlloc4->Add (Vector(89.5741022188639,104.01166254936771, 0.0));
    positionAlloc4->Add (Vector(154.3979298587511,145.30651296219, 0.0));
    positionAlloc4->Add (Vector(57.178553404026204,290.78496957033377, 0.0));
    positionAlloc4->Add (Vector(101.71243977790598,144.88549955704917, 0.0));
    positionAlloc4->Add (Vector(138.91237199344073,74.7525070539032, 0.0));
    positionAlloc4->Add (Vector(0.548127049453484,387.11314027149047, 0.0));
    positionAlloc4->Add (Vector(141.2265310438808,388.6154600077109, 0.0));
    positionAlloc4->Add (Vector(134.74562776769397,169.45840918206488, 0.0));
    positionAlloc4->Add (Vector(203.5994690773483,93.69544689307952, 0.0));
    positionAlloc4->Add (Vector(399.5218480493509,82.21565665255733, 0.0));
    positionAlloc4->Add (Vector(72.35578192634304,28.638323724284433, 0.0));
    positionAlloc4->Add (Vector(357.25056677770414,83.59943554368047, 0.0));
    positionAlloc4->Add (Vector(184.54188441945215,132.62209550549676, 0.0));
    positionAlloc4->Add (Vector(239.71437520911317,105.31174377428867, 0.0));
    positionAlloc4->Add (Vector(369.6945632192043,10.938254024089256, 0.0));
    positionAlloc4->Add (Vector(129.3411313895054,361.24765398605734, 0.0));
    positionAlloc4->Add (Vector(99.11340232484851,284.2933271383068, 0.0));
    positionAlloc4->Add (Vector(229.2950230774356,228.8073290571539, 0.0));
    positionAlloc4->Add (Vector(168.08194123719454,282.94899435973247, 0.0));
    positionAlloc4->Add (Vector(359.1038357517122,381.7707268038429, 0.0));
    positionAlloc4->Add (Vector(250.8717596117879,123.58420087852168, 0.0));
    positionAlloc4->Add (Vector(211.69349447126544,187.5024206166562, 0.0));
    positionAlloc4->Add (Vector(20.61117553373304,122.91702190295459, 0.0));
    positionAlloc4->Add (Vector(254.27423970950537,183.28854442592143, 0.0));
    positionAlloc4->Add (Vector(65.0637671384954,303.53681980777435, 0.0));
    positionAlloc4->Add (Vector(43.6134468462229,348.25660511605446, 0.0));
    positionAlloc4->Add (Vector(215.17304828078863,174.06260697380148, 0.0));
    positionAlloc4->Add (Vector(150.9899078385586,288.01978821197235, 0.0));
    positionAlloc4->Add (Vector(56.06107497980837,365.7968898708715, 0.0));
    positionAlloc4->Add (Vector(3.23757685428534,153.32113004702902, 0.0));
    positionAlloc4->Add (Vector(202.28437893927503,380.05004829692945, 0.0));
    positionAlloc4->Add (Vector(22.374652523211846,56.41021342612316, 0.0));
    positionAlloc4->Add (Vector(19.33937751930772,226.03188567369634, 0.0));
    positionAlloc4->Add (Vector(156.2453176444047,276.8973435247578, 0.0));
    positionAlloc4->Add (Vector(110.10541106197316,42.29526799945682, 0.0));
    positionAlloc4->Add (Vector(246.9283434162902,186.6461161679464, 0.0));
    positionAlloc4->Add (Vector(243.07391590937192,309.6458927189223, 0.0));
    positionAlloc4->Add (Vector(139.4801711383305,85.24398862597455, 0.0));
    positionAlloc4->Add (Vector(275.16092160729073,270.8531951851684, 0.0));
    positionAlloc4->Add (Vector(199.95659168381542,217.76681227922637, 0.0));
    positionAlloc4->Add (Vector(383.42781607830625,200.55335802412495, 0.0));
    positionAlloc4->Add (Vector(221.98123702472378,32.48883989807787, 0.0));
    positionAlloc4->Add (Vector(62.0357576712629,244.63480307349732, 0.0));
    positionAlloc4->Add (Vector(353.8999436376643,375.9924889775138, 0.0));
    positionAlloc4->Add (Vector(248.26505913482632,142.35542018690862, 0.0));
    positionAlloc4->Add (Vector(294.54995003404355,174.00014804528067, 0.0));
    positionAlloc4->Add (Vector(242.7883661675343,208.1505071066604, 0.0));
    positionAlloc4->Add (Vector(155.99543188858885,339.44154071250097, 0.0));
    positionAlloc4->Add (Vector(204.06730342826273,226.40519690435, 0.0));
    positionAlloc4->Add (Vector(261.42126869906946,79.49483876663814, 0.0));
    positionAlloc4->Add (Vector(168.49074616611085,233.74075800415704, 0.0));
    positionAlloc4->Add (Vector(102.16532683246902,185.40788907195483, 0.0));
    positionAlloc4->Add (Vector(51.82246478582289,111.78635232230731, 0.0));
    positionAlloc4->Add (Vector(276.2851381054145,45.118637261535575, 0.0));
    positionAlloc4->Add (Vector(9.168131491456943,300.4033138974111, 0.0));
    positionAlloc4->Add (Vector(186.58138270418263,85.20151502086519, 0.0));
    positionAlloc4->Add (Vector(7.363913317510207,139.7107977199757, 0.0));
    positionAlloc4->Add (Vector(370.13004022720935,321.6230722880637, 0.0));
    positionAlloc4->Add (Vector(168.3807593557578,249.15614342555267, 0.0));
    positionAlloc4->Add (Vector(102.54030200601063,139.52537861489222, 0.0));
    positionAlloc4->Add (Vector(397.8243157382976,287.33470421759665, 0.0));
    positionAlloc4->Add (Vector(213.7673648239615,345.0330397343707, 0.0));
    positionAlloc4->Add (Vector(365.4677303547039,68.73246833911236, 0.0));
    positionAlloc4->Add (Vector(271.6625409077905,188.6431756216557, 0.0));
    positionAlloc4->Add (Vector(234.93947062342966,88.7632946396732, 0.0));
    positionAlloc4->Add (Vector(158.18829022197943,55.09628952123138, 0.0));
    positionAlloc4->Add (Vector(200.47617745348262,58.20113649402599, 0.0));
    positionAlloc4->Add (Vector(301.0274542284412,303.1467382064021, 0.0));
    positionAlloc4->Add (Vector(364.5162436039439,107.96611069181665, 0.0));
    positionAlloc4->Add (Vector(240.8787633819872,317.3614703954029, 0.0));
    positionAlloc4->Add (Vector(333.63643426788616,128.0599202568368, 0.0));
    positionAlloc4->Add (Vector(131.467822625146,104.29303915182193, 0.0));
    positionAlloc4->Add (Vector(38.59970171304181,275.4946839255793, 0.0));
    positionAlloc4->Add (Vector(28.29862061920143,379.26290585773745, 0.0));
    positionAlloc4->Add (Vector(178.5598487073489,159.65381384219546, 0.0));
    positionAlloc4->Add (Vector(363.0215214002045,241.588834291588, 0.0));
    positionAlloc4->Add (Vector(269.400428341666,229.0143670207325, 0.0));
    positionAlloc4->Add (Vector(356.33384562151457,274.1251792867667, 0.0));
    positionAlloc4->Add (Vector(83.5323358277332,279.77521409469, 0.0));
    positionAlloc4->Add (Vector(222.27326707404035,267.1024380599097, 0.0));
    positionAlloc4->Add (Vector(105.1092504670589,210.60352946662647, 0.0));
    positionAlloc4->Add (Vector(102.42677664637205,366.5172387343033, 0.0));
    positionAlloc4->Add (Vector(262.49613617530406,361.6129079732107, 0.0));
    positionAlloc4->Add (Vector(263.30217497154786,241.15734349391062, 0.0));
    positionAlloc4->Add (Vector(59.59068370915701,265.3150088660492, 0.0));
    positionAlloc4->Add (Vector(77.26572711482747,6.272903627675497, 0.0));
    positionAlloc4->Add (Vector(40.78192544939516,232.92393086725855, 0.0));
    positionAlloc4->Add (Vector(21.194557572895924,377.6292166313135, 0.0));
    positionAlloc4->Add (Vector(373.0226633218054,263.28757494133964, 0.0));
    positionAlloc4->Add (Vector(255.3461530886071,76.55100400849308, 0.0));
    positionAlloc4->Add (Vector(339.65212036824835,244.15674111712667, 0.0));
    positionAlloc4->Add (Vector(133.9573103372569,248.94430136881329, 0.0));
    positionAlloc4->Add (Vector(294.42771725884734,67.27596797249431, 0.0));
    positionAlloc4->Add (Vector(5.8740424860044715,387.60980387676307, 0.0));
    positionAlloc4->Add (Vector(30.126456089072562,30.464376567908367, 0.0));
    positionAlloc4->Add (Vector(299.8269294957383,64.40290601198146, 0.0));
    positionAlloc4->Add (Vector(280.1831341150431,133.15433212331763, 0.0));
    positionAlloc4->Add (Vector(246.89105670090376,205.47107289957017, 0.0));
    positionAlloc4->Add (Vector(231.4655108734472,328.89597530364676, 0.0));
    positionAlloc4->Add (Vector(376.00843155915675,346.79491697079635, 0.0));
    positionAlloc4->Add (Vector(283.6994747407379,12.45676042143704, 0.0));
    positionAlloc4->Add (Vector(24.306942383218466,9.981320253388892, 0.0));
    positionAlloc4->Add (Vector(365.15332869766866,356.91133255754744, 0.0));
    positionAlloc4->Add (Vector(251.52031898135186,166.020432281051, 0.0));
    positionAlloc4->Add (Vector(340.13004901355316,157.82874918386355, 0.0));
    positionAlloc4->Add (Vector(116.73595968530938,338.970751324549, 0.0));
    positionAlloc4->Add (Vector(200.35510420256068,389.8818809812874, 0.0));
    positionAlloc4->Add (Vector(15.733974480345658,177.0786728603137, 0.0));
    positionAlloc4->Add (Vector(319.50721146915095,350.26880972552476, 0.0));
    positionAlloc4->Add (Vector(78.10391221825408,222.4131635345024, 0.0));
    positionAlloc4->Add (Vector(154.61628521800526,111.73296927130502, 0.0));
    positionAlloc4->Add (Vector(197.12966123832985,225.11594474303865, 0.0));
    positionAlloc4->Add (Vector(320.92230518311874,381.6810661415522, 0.0));
    positionAlloc4->Add (Vector(305.7403895423562,92.76239019138384, 0.0));
    positionAlloc4->Add (Vector(72.99266977556194,240.3285849939011, 0.0));
    positionAlloc4->Add (Vector(371.1335500785733,14.833740523632954, 0.0));
    positionAlloc4->Add (Vector(269.8635825169093,9.07350756025389, 0.0));
    positionAlloc4->Add (Vector(231.273539111141,276.4122237568078, 0.0));
    positionAlloc4->Add (Vector(189.23717864232347,30.690429653022466, 0.0));
    positionAlloc4->Add (Vector(12.510688360993072,34.70031944940426, 0.0));
    positionAlloc4->Add (Vector(105.76281378365789,7.965581230399144, 0.0));
    positionAlloc4->Add (Vector(324.9750564619187,133.90731573509598, 0.0));
    positionAlloc4->Add (Vector(78.27827811483958,5.97098705230783, 0.0));
    positionAlloc4->Add (Vector(165.7876616157112,19.97784268883196, 0.0));
    positionAlloc4->Add (Vector(20.183895832004062,22.150409763049872, 0.0));
    positionAlloc4->Add (Vector(310.0491336988021,269.86554730854976, 0.0));
    positionAlloc4->Add (Vector(312.9761756773073,317.0866579595038, 0.0));
    positionAlloc4->Add (Vector(85.81412864680584,126.58715683390609, 0.0));
    positionAlloc4->Add (Vector(160.0526056942118,46.34479300551986, 0.0));
    positionAlloc4->Add (Vector(392.15416500128424,199.02812382975185, 0.0));
    positionAlloc4->Add (Vector(138.56772226855819,262.9025713801376, 0.0));
    positionAlloc4->Add (Vector(313.75878619302637,136.13400587374903, 0.0));
    positionAlloc4->Add (Vector(149.4086830527075,10.168485028577434, 0.0));
    positionAlloc4->Add (Vector(151.6139107822695,58.54480293452147, 0.0));
    positionAlloc4->Add (Vector(317.05027044817285,91.94618338808081, 0.0));
    positionAlloc4->Add (Vector(397.29836020764697,33.57690549418311, 0.0));
    positionAlloc4->Add (Vector(298.1842371060804,54.77496320886752, 0.0));
    positionAlloc4->Add (Vector(241.67269210529184,154.35229625085972, 0.0));
    positionAlloc4->Add (Vector(200.28352331215538,318.106543528569, 0.0));
    positionAlloc4->Add (Vector(282.4125646862497,108.53002456408962, 0.0));
    positionAlloc4->Add (Vector(252.59805457113217,345.48804067865274, 0.0));
    positionAlloc4->Add (Vector(116.47237246995252,121.70809231340343, 0.0));
    positionAlloc4->Add (Vector(384.3497760289084,348.0069721938042, 0.0));
    positionAlloc4->Add (Vector(33.433379929972595,363.93437298380326, 0.0));
    positionAlloc4->Add (Vector(27.721394287069636,153.47258005136615, 0.0));
    positionAlloc4->Add (Vector(313.236059498803,265.21921436247374, 0.0));
    positionAlloc4->Add (Vector(318.55124865360483,269.7578042797731, 0.0));
    positionAlloc4->Add (Vector(231.85279456432121,55.7351676553488, 0.0));
    positionAlloc4->Add (Vector(197.58073301773499,86.14414095982158, 0.0));
    positionAlloc4->Add (Vector(318.5996941634279,381.61519928342085, 0.0));
    positionAlloc4->Add (Vector(214.59655780721135,144.65922031696422, 0.0));
    positionAlloc4->Add (Vector(329.0219722227249,383.3376599881949, 0.0));
    positionAlloc4->Add (Vector(35.07076778727658,170.32451266603312, 0.0));
    positionAlloc4->Add (Vector(20.82281957893595,112.87069979449744, 0.0));
    positionAlloc4->Add (Vector(186.06702299451473,197.85145375308252, 0.0));
    positionAlloc4->Add (Vector(279.11242985061114,170.81923767114858, 0.0));
    positionAlloc4->Add (Vector(117.43284297529554,357.4650843879364, 0.0));
    positionAlloc4->Add (Vector(167.25063925039075,244.87354569008355, 0.0));
    positionAlloc4->Add (Vector(180.61031779016793,165.128752859756, 0.0));
    positionAlloc4->Add (Vector(329.73701367084607,265.8349818903714, 0.0));
    positionAlloc4->Add (Vector(274.24665076694004,314.6066233990985, 0.0));
    positionAlloc4->Add (Vector(42.640564972369035,305.9587860567091, 0.0));
    positionAlloc4->Add (Vector(308.056571203762,176.97927507157297, 0.0));
    positionAlloc4->Add (Vector(258.42104247406553,210.2131124720136, 0.0));
    positionAlloc4->Add (Vector(328.30412846487536,208.54104037792393, 0.0));
    positionAlloc4->Add (Vector(323.19394813373964,16.195424206657183, 0.0));
    positionAlloc4->Add (Vector(18.797438330822658,349.5955571595362, 0.0));
    positionAlloc4->Add (Vector(283.17098826262696,379.2068871025068, 0.0));
    positionAlloc4->Add (Vector(305.3440975477211,345.6160851975804, 0.0));
    positionAlloc4->Add (Vector(27.770395623875732,183.2110442420817, 0.0));
    positionAlloc4->Add (Vector(318.6685404429404,19.06062827732842, 0.0));
    positionAlloc4->Add (Vector(239.28377315942814,6.432350446131174, 0.0));
    positionAlloc4->Add (Vector(169.00951899925522,236.27223326533917, 0.0));
    positionAlloc4->Add (Vector(66.96304918517156,304.3530695597185, 0.0));
    positionAlloc4->Add (Vector(228.12184867087387,371.33084353416405, 0.0));
    positionAlloc4->Add (Vector(156.726513698948,101.12736685646597, 0.0));
    positionAlloc4->Add (Vector(154.04626739606962,207.82731415344907, 0.0));
    positionAlloc4->Add (Vector(231.45622661558875,250.8330674908341, 0.0));
    positionAlloc4->Add (Vector(349.6014404133481,199.6892706958155, 0.0));
    positionAlloc4->Add (Vector(107.09508550397939,305.2998364174961, 0.0));
    positionAlloc4->Add (Vector(231.46389060866807,33.53233878307309, 0.0));
    positionAlloc4->Add (Vector(277.6962648763996,278.10251300689765, 0.0));
    positionAlloc4->Add (Vector(181.86501524025695,332.5387350894024, 0.0));
    positionAlloc4->Add (Vector(353.8854492275371,46.66619338604274, 0.0));
    positionAlloc4->Add (Vector(8.303261625711222,65.73168272458632, 0.0));
    positionAlloc4->Add (Vector(385.4479866622637,397.7238487272316, 0.0));
    positionAlloc4->Add (Vector(360.30044986055515,292.7170465207156, 0.0));
    positionAlloc4->Add (Vector(395.7536394204219,394.7262395296667, 0.0));
    positionAlloc4->Add (Vector(242.24365249741479,40.91494818118999, 0.0));
    positionAlloc4->Add (Vector(245.47034573635682,375.798632664054, 0.0));
    positionAlloc4->Add (Vector(298.9008745540191,345.02582595806876, 0.0));
    positionAlloc4->Add (Vector(310.46395385373285,115.22712401515545, 0.0));
    positionAlloc4->Add (Vector(59.213759715208084,363.9876475814958, 0.0));
    positionAlloc4->Add (Vector(105.83491065312872,68.0497897866585, 0.0));
    positionAlloc4->Add (Vector(377.28927333142906,266.14529304218104, 0.0));
    positionAlloc4->Add (Vector(70.5805221960877,96.2096045663348, 0.0));
    positionAlloc4->Add (Vector(348.03459500328296,312.6185597205305, 0.0));
    positionAlloc4->Add (Vector(273.1710668059842,277.4545238690997, 0.0));
    positionAlloc4->Add (Vector(16.102161565518447,153.7655658139879, 0.0));
    positionAlloc4->Add (Vector(108.43426075014291,258.8756715111219, 0.0));
    positionAlloc4->Add (Vector(159.68772853271122,106.5021298017752, 0.0));
    positionAlloc4->Add (Vector(190.87865863359337,311.98680545102036, 0.0));
    positionAlloc4->Add (Vector(354.24876340753406,355.2951040559612, 0.0));
    positionAlloc4->Add (Vector(22.607742036557354,341.61080442716985, 0.0));
    positionAlloc4->Add (Vector(271.59237341535527,165.7684651945928, 0.0));
    positionAlloc4->Add (Vector(71.63280673619643,183.4965388923635, 0.0));
    positionAlloc4->Add (Vector(360.2047092762463,127.3111028729864, 0.0));
    positionAlloc4->Add (Vector(177.9055633451134,332.7589794726794, 0.0));
    positionAlloc4->Add (Vector(264.2075575098434,117.56451945229074, 0.0));
    positionAlloc4->Add (Vector(301.6112703919545,62.0043192777457, 0.0));
    positionAlloc4->Add (Vector(300.1181477861216,182.0286437754318, 0.0));
    positionAlloc4->Add (Vector(73.43798179713717,292.8450215791735, 0.0));
    positionAlloc4->Add (Vector(110.89072154626342,276.8294623991723, 0.0));
    positionAlloc4->Add (Vector(139.06438551579092,173.4505059093047, 0.0));
    positionAlloc4->Add (Vector(248.81762758124512,377.5751053671179, 0.0));
    positionAlloc4->Add (Vector(94.67773009374514,25.129492100454918, 0.0));
    positionAlloc4->Add (Vector(294.1511531124497,324.87936120443106, 0.0));
    positionAlloc4->Add (Vector(306.19878418776483,123.26910867280789, 0.0));
    positionAlloc4->Add (Vector(43.21746252801897,305.1175110572888, 0.0));
    positionAlloc4->Add (Vector(118.49249130865873,23.813477950510364, 0.0));
    positionAlloc4->Add (Vector(71.67301472584651,277.86563700199997, 0.0));
    positionAlloc4->Add (Vector(398.627578468477,346.04323589649937, 0.0));
    positionAlloc4->Add (Vector(234.6393892440825,346.1180675968758, 0.0));
    positionAlloc4->Add (Vector(373.68502948797675,290.1916214492311, 0.0));
    positionAlloc4->Add (Vector(82.52995388612612,237.7362364104851, 0.0));
    positionAlloc4->Add (Vector(235.36490082501075,223.5059125840325, 0.0));
    positionAlloc4->Add (Vector(26.431424213501977,303.16018457068026, 0.0));
    positionAlloc4->Add (Vector(194.31156290637753,64.53868047808334, 0.0));
    positionAlloc4->Add (Vector(20.74066956903442,304.72866705967033, 0.0));
    positionAlloc4->Add (Vector(156.3705168542914,109.15058528341244, 0.0));
    positionAlloc4->Add (Vector(308.8918713640816,177.20136407891061, 0.0));
    positionAlloc4->Add (Vector(33.92978619907581,187.05873180864447, 0.0));
    positionAlloc4->Add (Vector(144.75211692380375,208.263585787458, 0.0));
    positionAlloc4->Add (Vector(227.86129796614495,27.327456303974973, 0.0));
    positionAlloc4->Add (Vector(209.09466892597885,163.08454408721857, 0.0));
    positionAlloc4->Add (Vector(376.6392532066996,128.03428371306617, 0.0));
    positionAlloc4->Add (Vector(21.534672369440997,222.57369465820872, 0.0));
    positionAlloc4->Add (Vector(128.23370053661924,253.18879856547022, 0.0));
    positionAlloc4->Add (Vector(3.6198903069534882,288.18905718216865, 0.0));
    positionAlloc4->Add (Vector(245.5687847583313,296.1034993757653, 0.0));
    positionAlloc4->Add (Vector(36.510544941137056,294.23268428929606, 0.0));
    positionAlloc4->Add (Vector(122.9044608367115,297.1498215983693, 0.0));
    positionAlloc4->Add (Vector(140.941392504765,62.25617260894802, 0.0));
    positionAlloc4->Add (Vector(204.62729564204878,346.2328759496599, 0.0));
    positionAlloc4->Add (Vector(179.31823628680337,50.54116886655269, 0.0));
    positionAlloc4->Add (Vector(145.65916976508794,100.2185033833285, 0.0));
    positionAlloc4->Add (Vector(385.49901132192497,45.72972906372335, 0.0));
    positionAlloc4->Add (Vector(225.67018073629788,393.60851883259687, 0.0));
    positionAlloc4->Add (Vector(337.1669388980393,215.84520920699853, 0.0));
    positionAlloc4->Add (Vector(354.2657420979999,151.72397357454784, 0.0));
    positionAlloc4->Add (Vector(350.851553999085,95.89660088289263, 0.0));
    positionAlloc4->Add (Vector(339.83913452162915,20.844943367849833, 0.0));
    positionAlloc4->Add (Vector(351.2558245196301,52.61067025695301, 0.0));
    positionAlloc4->Add (Vector(333.66332535647075,281.97230269130824, 0.0));
    positionAlloc4->Add (Vector(225.43358411640702,113.66716064521923, 0.0));
    positionAlloc4->Add (Vector(336.1068551134485,214.86304579242832, 0.0));
    positionAlloc4->Add (Vector(114.42199840238159,121.72116748053811, 0.0));
    positionAlloc4->Add (Vector(181.31499732321865,91.37236725256814, 0.0));
    positionAlloc4->Add (Vector(275.9821073724888,318.0063137695678, 0.0));
    positionAlloc4->Add (Vector(59.77831966113656,305.2108730193948, 0.0));
    positionAlloc4->Add (Vector(236.84262519232462,357.80463511366537, 0.0));
    positionAlloc4->Add (Vector(80.66649805830086,287.78079498874393, 0.0));
    positionAlloc4->Add (Vector(335.940223764022,347.8900533692649, 0.0));
    positionAlloc4->Add (Vector(146.50681112063978,116.29956068809099, 0.0));
    positionAlloc4->Add (Vector(202.98410331341213,386.9609735873718, 0.0));
    positionAlloc4->Add (Vector(233.62932062185942,377.6026002121787, 0.0));
    positionAlloc4->Add (Vector(272.09455839037855,75.76990151534338, 0.0));
    positionAlloc4->Add (Vector(174.90726105555444,345.28049826985426, 0.0));
    positionAlloc4->Add (Vector(203.34836609518698,33.893352129560924, 0.0));
    positionAlloc4->Add (Vector(325.15007204819284,198.34172425925752, 0.0));
    positionAlloc4->Add (Vector(267.3197653968086,88.58614276670154, 0.0));
    positionAlloc4->Add (Vector(366.3288898686601,285.56204670179335, 0.0));
    positionAlloc4->Add (Vector(243.48798854480384,149.0475348377632, 0.0));
    positionAlloc4->Add (Vector(285.6158113983683,236.38685643379313, 0.0));
    positionAlloc4->Add (Vector(343.4037585921884,58.560942777993574, 0.0));
    positionAlloc4->Add (Vector(152.4133277519706,218.31619587094804, 0.0));
    positionAlloc4->Add (Vector(327.6780379721279,42.43272192122478, 0.0));
    positionAlloc4->Add (Vector(303.0877086877498,284.4923979718574, 0.0));
    positionAlloc4->Add (Vector(374.55175328966146,353.3006388402752, 0.0));
    positionAlloc4->Add (Vector(193.84905353773215,236.2356541538383, 0.0));
    positionAlloc4->Add (Vector(227.4672793711566,359.38451117265475, 0.0));
    positionAlloc4->Add (Vector(324.99381779608257,161.26870344086308, 0.0));
    positionAlloc4->Add (Vector(179.88856918613513,224.59993750409643, 0.0));
    positionAlloc4->Add (Vector(209.49798130392688,356.454057508882, 0.0));
    positionAlloc4->Add (Vector(209.39145264336992,357.5601100162748, 0.0));
    positionAlloc4->Add (Vector(105.08799679941023,325.2023159103792, 0.0));
    positionAlloc4->Add (Vector(0.11689941223966827,233.04650229756635, 0.0));
    positionAlloc4->Add (Vector(361.84947763455784,348.6412699237277, 0.0));
    positionAlloc4->Add (Vector(298.0561692535167,391.07873273851095, 0.0));
    positionAlloc4->Add (Vector(232.9445425571464,122.88357909941175, 0.0));
    positionAlloc4->Add (Vector(243.7496126203663,354.4266668337534, 0.0));
    positionAlloc4->Add (Vector(288.229976430974,52.20629869358153, 0.0));
    positionAlloc4->Add (Vector(158.5982558294774,279.73196309484405, 0.0));
    positionAlloc4->Add (Vector(280.37951304952225,89.34023100050746, 0.0));
    positionAlloc4->Add (Vector(129.35537601314343,276.6761733784764, 0.0));
    positionAlloc4->Add (Vector(165.1664476084671,86.33926921206245, 0.0));
    positionAlloc4->Add (Vector(239.30004342428498,343.21868909218125, 0.0));
    positionAlloc4->Add (Vector(72.58436964710029,105.82469891258172, 0.0));
    positionAlloc4->Add (Vector(278.04820882143446,240.96261990072225, 0.0));
    positionAlloc4->Add (Vector(90.13045656236885,170.29753000915474, 0.0));
    positionAlloc4->Add (Vector(135.198913331501,120.70364786341852, 0.0));
    positionAlloc4->Add (Vector(65.56940818455263,396.18430493224776, 0.0));
    positionAlloc4->Add (Vector(299.6021006043688,181.07033400214482, 0.0));
    positionAlloc4->Add (Vector(177.88152118664996,110.84823967071817, 0.0));
    positionAlloc4->Add (Vector(144.64547401490177,130.14999044467635, 0.0));
    positionAlloc4->Add (Vector(134.30180255183149,328.06828178961894, 0.0));
    positionAlloc4->Add (Vector(363.1924725037847,118.75485119328641, 0.0));
    positionAlloc4->Add (Vector(280.30568658461635,398.2413496156875, 0.0));
    positionAlloc4->Add (Vector(203.44602085423534,199.86279160739522, 0.0));
    positionAlloc4->Add (Vector(376.5784286467342,26.058901682985436, 0.0));
    positionAlloc4->Add (Vector(302.3183827137575,319.86024479833173, 0.0));
    positionAlloc4->Add (Vector(279.7649715677026,272.40239725827735, 0.0));
    positionAlloc4->Add (Vector(326.19691676749324,54.40092850329701, 0.0));
    positionAlloc4->Add (Vector(276.92081865137925,350.6547087348065, 0.0));
    positionAlloc4->Add (Vector(119.27131582641803,372.5927754661435, 0.0));
    positionAlloc4->Add (Vector(346.04565767805855,251.89982615037084, 0.0));
    positionAlloc4->Add (Vector(320.95859830708184,260.4060232716851, 0.0));
    positionAlloc4->Add (Vector(381.1865615042793,44.930789443451545, 0.0));
    positionAlloc4->Add (Vector(95.71420867522096,29.231890192456866, 0.0));
    positionAlloc4->Add (Vector(235.86475490581512,334.11047239667556, 0.0));
    positionAlloc4->Add (Vector(218.80799436633615,351.3562142112789, 0.0));
    positionAlloc4->Add (Vector(392.0949528351058,83.09188730502095, 0.0));
    positionAlloc4->Add (Vector(98.81736554849705,215.10145656484045, 0.0));
    positionAlloc4->Add (Vector(103.60297228383234,43.571213618772475, 0.0));
    positionAlloc4->Add (Vector(376.7999099585969,117.32478326301599, 0.0));
    positionAlloc4->Add (Vector(342.6637112934394,159.36244485481174, 0.0));
    positionAlloc4->Add (Vector(179.37473748510766,191.89044060606108, 0.0));
    positionAlloc4->Add (Vector(375.95373004637685,362.7157443820454, 0.0));
    positionAlloc4->Add (Vector(353.77327368388575,229.96099606438452, 0.0));
    positionAlloc4->Add (Vector(338.37869295708043,290.880316416971, 0.0));
    positionAlloc4->Add (Vector(152.70093061935492,306.78805475698033, 0.0));
    positionAlloc4->Add (Vector(30.0837130851356,147.21142404905572, 0.0));
    positionAlloc4->Add (Vector(13.331281981972376,8.473481349308098, 0.0));
    positionAlloc4->Add (Vector(19.715428919697196,248.35836491385965, 0.0));
    positionAlloc4->Add (Vector(43.75804837250321,211.87093411576484, 0.0));
    positionAlloc4->Add (Vector(187.0911887936402,53.84831875705256, 0.0));
    positionAlloc4->Add (Vector(294.3051876385737,353.53748924649494, 0.0));
    positionAlloc4->Add (Vector(100.00515655442443,237.1397822504492, 0.0));
    positionAlloc4->Add (Vector(348.4929279494564,329.8825290872244, 0.0));
    positionAlloc4->Add (Vector(270.12419279603444,75.99748218261686, 0.0));
    positionAlloc4->Add (Vector(364.1906988607631,147.67631773319954, 0.0));
    positionAlloc4->Add (Vector(64.38451042110765,321.1430951507226, 0.0));
    positionAlloc4->Add (Vector(317.55317566597296,17.574341164728892, 0.0));
    positionAlloc4->Add (Vector(46.11373999816757,308.9757516003796, 0.0));
    positionAlloc4->Add (Vector(93.6862133560425,119.83566874180798, 0.0));
    positionAlloc4->Add (Vector(149.2879732126895,184.1252536949137, 0.0));
    positionAlloc4->Add (Vector(150.38327511020046,250.7607836213206, 0.0));
    positionAlloc4->Add (Vector(99.96689083984708,204.9178293909734, 0.0));
    positionAlloc4->Add (Vector(230.9396111891404,22.776797057878227, 0.0));
    positionAlloc4->Add (Vector(187.6576554540184,139.91987843138833, 0.0));
    positionAlloc4->Add (Vector(86.70618215441009,38.92926543050348, 0.0));
    positionAlloc4->Add (Vector(264.2893207399943,104.31524756321436, 0.0));
    positionAlloc4->Add (Vector(175.71686029099328,349.89056576317284, 0.0));
    positionAlloc4->Add (Vector(23.540256642686064,214.3451254955365, 0.0));
    positionAlloc4->Add (Vector(260.2079965577251,182.13600803529704, 0.0));
    positionAlloc4->Add (Vector(252.97895466958414,68.75590071065561, 0.0));
    positionAlloc4->Add (Vector(270.1993471348494,338.0653920179196, 0.0));
    positionAlloc4->Add (Vector(250.18879953925688,303.2447287058804, 0.0));
    positionAlloc4->Add (Vector(359.2774458663582,350.9055564868313, 0.0));
    positionAlloc4->Add (Vector(176.4953523541775,343.02158788475833, 0.0));
    positionAlloc4->Add (Vector(345.83046444844615,51.24535436811817, 0.0));
    positionAlloc4->Add (Vector(374.6136199308495,247.683551784493, 0.0));
    positionAlloc4->Add (Vector(178.60912553398006,94.2548594305518, 0.0));
    positionAlloc4->Add (Vector(216.88749822166926,342.14411536049465, 0.0));
    positionAlloc4->Add (Vector(13.598332240219513,6.148698059260216, 0.0));
    positionAlloc4->Add (Vector(71.6953228249888,102.35137214068577, 0.0));
    positionAlloc4->Add (Vector(205.416310466773,365.4863391133566, 0.0));
    positionAlloc4->Add (Vector(99.14810902850402,44.73655226445725, 0.0));
    positionAlloc4->Add (Vector(222.34690489145387,367.2276793281571, 0.0));
    positionAlloc4->Add (Vector(283.886297143796,122.2175173411419, 0.0));
    positionAlloc4->Add (Vector(240.03415384310935,115.53841083550154, 0.0));
    positionAlloc4->Add (Vector(320.83058461311765,1.0170457720365622, 0.0));
    positionAlloc4->Add (Vector(362.2380462549059,394.7513288443335, 0.0));
    positionAlloc4->Add (Vector(213.55387838346118,315.5326196879558, 0.0));
    positionAlloc4->Add (Vector(296.98853949184223,168.28181210132578, 0.0));
    positionAlloc4->Add (Vector(263.4855894946123,5.909527574042972, 0.0));
    positionAlloc4->Add (Vector(229.44723356296174,18.646240727327967, 0.0));
    positionAlloc4->Add (Vector(176.30175443090116,384.52522879798005, 0.0));
    positionAlloc4->Add (Vector(134.38364179672715,387.5496665509418, 0.0));
    positionAlloc4->Add (Vector(32.204593133257916,88.4780727819828, 0.0));
    positionAlloc4->Add (Vector(133.1771993568199,210.83271215149497, 0.0));
    positionAlloc4->Add (Vector(204.83772606965124,186.4417038221593, 0.0));
    positionAlloc4->Add (Vector(329.33937922367613,115.24625131655468, 0.0));
    positionAlloc4->Add (Vector(397.09744747337623,170.7042918321982, 0.0));
    positionAlloc4->Add (Vector(31.421371175398072,134.75376834250983, 0.0));
    positionAlloc4->Add (Vector(382.3265560659867,398.2393694038823, 0.0));
    positionAlloc4->Add (Vector(390.1867132197043,61.234410737773224, 0.0));
    positionAlloc4->Add (Vector(299.9075097576622,209.3181515485161, 0.0));
    positionAlloc4->Add (Vector(350.2756354994537,277.60841284080766, 0.0));
    positionAlloc4->Add (Vector(258.0008138811549,2.9135363762300415, 0.0));
    positionAlloc4->Add (Vector(31.499087117815705,169.44935285364164, 0.0));
    positionAlloc4->Add (Vector(250.07525876076627,322.1602924714723, 0.0));
    positionAlloc4->Add (Vector(192.27899592129862,114.68111342077596, 0.0));
    positionAlloc4->Add (Vector(162.03452433852394,239.91031787827683, 0.0));
    positionAlloc4->Add (Vector(117.43823055293245,196.49469673376024, 0.0));
    positionAlloc4->Add (Vector(392.05268676241604,219.7049066726585, 0.0));
    positionAlloc4->Add (Vector(387.5641883991358,236.31475793143042, 0.0));
    positionAlloc4->Add (Vector(261.4431452810708,350.33507538963886, 0.0));
    positionAlloc4->Add (Vector(367.2066699819012,340.2581237820944, 0.0));
    positionAlloc4->Add (Vector(218.65180421581312,105.12896619714832, 0.0));
    positionAlloc4->Add (Vector(139.01360340231622,43.01301241599531, 0.0));
    positionAlloc4->Add (Vector(291.90262038957906,158.24381920561495, 0.0));
    positionAlloc4->Add (Vector(29.497638485499866,268.90722312699427, 0.0));
    positionAlloc4->Add (Vector(352.0661916760655,214.5399352374766, 0.0));
    positionAlloc4->Add (Vector(112.17053634957969,135.7022813340861, 0.0));
    positionAlloc4->Add (Vector(149.9587774203699,178.8373885194035, 0.0));
    positionAlloc4->Add (Vector(306.675717872009,232.84100918165657, 0.0));
    positionAlloc4->Add (Vector(288.7115359808889,271.42584517234053, 0.0));
    positionAlloc4->Add (Vector(181.09088970897048,316.73095939524234, 0.0));
    positionAlloc4->Add (Vector(300.5462867482745,31.954821043334647, 0.0));
    positionAlloc4->Add (Vector(66.93190695086595,238.30542064779468, 0.0));
    positionAlloc4->Add (Vector(215.58066751076703,391.48966557205836, 0.0));
    positionAlloc4->Add (Vector(349.1487657493982,69.18796355143608, 0.0));
    positionAlloc4->Add (Vector(40.93741087437439,22.072843987654966, 0.0));
    positionAlloc4->Add (Vector(148.82421794574677,93.54267290387494, 0.0));
    positionAlloc4->Add (Vector(166.51773444681538,213.75854924695412, 0.0));
    positionAlloc4->Add (Vector(91.16631203886216,362.142650301075, 0.0));
    positionAlloc4->Add (Vector(297.4988695898755,131.5458262885274, 0.0));
    positionAlloc4->Add (Vector(98.83143773162067,101.7319432874805, 0.0));
    positionAlloc4->Add (Vector(235.94257558378322,149.42733421660316, 0.0));
    positionAlloc4->Add (Vector(324.766296207763,61.844481715410595, 0.0));
    positionAlloc4->Add (Vector(280.08378486119574,180.92123001285265, 0.0));
    positionAlloc4->Add (Vector(147.74564069142082,147.6707536282054, 0.0));
    positionAlloc4->Add (Vector(231.8119463685798,307.5364616746057, 0.0));
    positionAlloc4->Add (Vector(305.40639417639005,278.1483712270469, 0.0));
    positionAlloc4->Add (Vector(151.43610069982861,76.56709207284216, 0.0));
    positionAlloc4->Add (Vector(194.82799827925885,36.71903972495429, 0.0));
    positionAlloc4->Add (Vector(278.62869096079515,341.56117845392436, 0.0));
    positionAlloc4->Add (Vector(284.1098243442616,344.45138233961325, 0.0));
    positionAlloc4->Add (Vector(294.5631223782697,180.7030574226929, 0.0));
    positionAlloc4->Add (Vector(347.46689011855057,49.917357143893206, 0.0));
    positionAlloc4->Add (Vector(219.6998642547464,18.01198758469149, 0.0));
    positionAlloc4->Add (Vector(227.17678141133723,252.35595697886262, 0.0));
    positionAlloc4->Add (Vector(190.20273853379584,364.84929317978845, 0.0));
    positionAlloc4->Add (Vector(136.85365328250606,72.11978861602182, 0.0));
    positionAlloc4->Add (Vector(100.33185898673796,201.60974799826076, 0.0));
    positionAlloc4->Add (Vector(251.15748001303925,286.31907882540565, 0.0));
    positionAlloc4->Add (Vector(103.24221982391771,279.2028330267912, 0.0));
    positionAlloc4->Add (Vector(67.49178890483721,318.5750673042055, 0.0));
    positionAlloc4->Add (Vector(47.84722838425664,261.7042926503555, 0.0));
    positionAlloc4->Add (Vector(70.42479246676999,373.72083214624615, 0.0));
    positionAlloc4->Add (Vector(203.09642137894545,140.99974888651525, 0.0));
    positionAlloc4->Add (Vector(146.90063287740202,14.35799427574671, 0.0));
    positionAlloc4->Add (Vector(309.60728400166647,393.42211509908407, 0.0));
    positionAlloc4->Add (Vector(313.53531220741354,172.81548281692048, 0.0));
    positionAlloc4->Add (Vector(344.06917476648204,374.91096450726207, 0.0));
    positionAlloc4->Add (Vector(250.36481952683977,343.59378187093967, 0.0));
    positionAlloc4->Add (Vector(194.58776338073537,2.28999717058489, 0.0));
    positionAlloc4->Add (Vector(309.275325270942,43.98901467559342, 0.0));
    positionAlloc4->Add (Vector(47.7100059521173,295.8245972818175, 0.0));
    positionAlloc4->Add (Vector(270.90996956564004,307.3358513700192, 0.0));
    positionAlloc4->Add (Vector(100.38984207588103,26.47430907329675, 0.0));
    positionAlloc4->Add (Vector(270.460708772485,241.4899236218679, 0.0));
    positionAlloc4->Add (Vector(56.61065245469854,247.57172370555773, 0.0));
    positionAlloc4->Add (Vector(194.86245100211698,6.2361305754176755, 0.0));
    positionAlloc4->Add (Vector(180.94125776262052,27.88947282283449, 0.0));
    positionAlloc4->Add (Vector(224.90151035282636,34.58265512169278, 0.0));
    positionAlloc4->Add (Vector(395.34647601373325,257.4142336082742, 0.0));
    positionAlloc4->Add (Vector(195.4251173418623,169.48483334737875, 0.0));
    positionAlloc4->Add (Vector(11.148800246347168,186.83477248052444, 0.0));
    positionAlloc4->Add (Vector(70.32906565179351,69.05201823093176, 0.0));
    positionAlloc4->Add (Vector(262.9706322457023,112.90748508041095, 0.0));
    positionAlloc4->Add (Vector(99.5519230853103,106.20614583888765, 0.0));
    positionAlloc4->Add (Vector(371.36483094941116,367.6454458200249, 0.0));
    positionAlloc4->Add (Vector(256.3641224673799,225.45912459964458, 0.0));
    positionAlloc4->Add (Vector(322.0750128955145,13.4256616249111, 0.0));
    positionAlloc4->Add (Vector(230.55753489632545,6.149522507445404, 0.0));
    positionAlloc4->Add (Vector(355.1839142133339,115.25146848592303, 0.0));
    positionAlloc4->Add (Vector(314.8984002429406,159.577246627908, 0.0));
    positionAlloc4->Add (Vector(364.2116141987642,96.8998740510306, 0.0));
    positionAlloc4->Add (Vector(296.91154069552255,104.17075994500125, 0.0));
    positionAlloc4->Add (Vector(24.591062598442548,124.21884754878336, 0.0));
    positionAlloc4->Add (Vector(307.12562799937774,334.37205497611654, 0.0));
    positionAlloc4->Add (Vector(304.2162660741795,156.88310326863228, 0.0));
    positionAlloc4->Add (Vector(281.92204529936396,377.25609695020745, 0.0));
    positionAlloc4->Add (Vector(40.190745465925474,58.7893508685398, 0.0));
    positionAlloc4->Add (Vector(141.72408401107165,35.90966789062308, 0.0));
    positionAlloc4->Add (Vector(28.991061508294447,359.6033079355701, 0.0));
    positionAlloc4->Add (Vector(130.30852735136347,284.2274709625522, 0.0));
    positionAlloc4->Add (Vector(142.46456446766643,31.903626729984722, 0.0));
    positionAlloc4->Add (Vector(247.49050275216257,18.39908028559769, 0.0));
    positionAlloc4->Add (Vector(281.9126889810064,235.55870679510863, 0.0));
    positionAlloc4->Add (Vector(370.68363189665024,275.1733377123614, 0.0));
    positionAlloc4->Add (Vector(95.62963690402113,83.1659217413725, 0.0));
    positionAlloc4->Add (Vector(386.86962853937405,199.54638133152943, 0.0));
    positionAlloc4->Add (Vector(102.27470767287947,188.31699145655585, 0.0));
    positionAlloc4->Add (Vector(149.6286133798679,136.58438548090942, 0.0));
    positionAlloc4->Add (Vector(358.03293826086485,370.57912501501613, 0.0));
    positionAlloc4->Add (Vector(33.42984925638683,303.7483026362237, 0.0));
    positionAlloc4->Add (Vector(351.0741326047436,103.20440854139856, 0.0));
    positionAlloc4->Add (Vector(313.86829683716206,133.87210242524898, 0.0));
    positionAlloc4->Add (Vector(208.5246235355097,285.9550942865844, 0.0));
    positionAlloc4->Add (Vector(54.819234718829165,363.4953854544074, 0.0));
    positionAlloc4->Add (Vector(155.40432259986622,384.5935512397192, 0.0));
    positionAlloc4->Add (Vector(383.03338583109104,351.9003186230273, 0.0));
    positionAlloc4->Add (Vector(232.1720543429884,323.02577102442933, 0.0));
    positionAlloc4->Add (Vector(329.39225179192937,173.23813008213457, 0.0));
    positionAlloc4->Add (Vector(272.55931906389463,323.4841392166108, 0.0));
    positionAlloc4->Add (Vector(11.60691116663477,48.78642386473473, 0.0));
    positionAlloc4->Add (Vector(362.87079625228154,223.04112606606364, 0.0));
    positionAlloc4->Add (Vector(125.14295205656198,85.7505847014667, 0.0));
    positionAlloc4->Add (Vector(198.69175455765688,274.1536081884349, 0.0));
    positionAlloc4->Add (Vector(211.70584806559017,263.210741343026, 0.0));
    positionAlloc4->Add (Vector(160.13413843154768,223.30424848668434, 0.0));
    positionAlloc4->Add (Vector(98.22518981606683,186.99984456275897, 0.0));
    positionAlloc4->Add (Vector(312.2971146936082,394.6065822820978, 0.0));
    positionAlloc4->Add (Vector(96.66391122694704,143.8696965912715, 0.0));
    positionAlloc4->Add (Vector(1.8002171461299454,279.9395057732168, 0.0));
    positionAlloc4->Add (Vector(341.3809157952111,305.7317764319236, 0.0));
    positionAlloc4->Add (Vector(106.13789701702503,18.8708794656252, 0.0));
    positionAlloc4->Add (Vector(246.94014041171437,318.17504897934504, 0.0));
    positionAlloc4->Add (Vector(77.81444570672366,323.5411105799327, 0.0));
    positionAlloc4->Add (Vector(249.7715047358306,95.22768286551097, 0.0));
    positionAlloc4->Add (Vector(135.57341766488813,6.45538133258432, 0.0));
    positionAlloc4->Add (Vector(123.0121902618344,262.9697550935399, 0.0));
    positionAlloc4->Add (Vector(180.93805705184582,352.9395528527887, 0.0));
    positionAlloc4->Add (Vector(41.40669533025623,77.19278473240064, 0.0));
    positionAlloc4->Add (Vector(49.09690263649065,368.65318040270176, 0.0));
    positionAlloc4->Add (Vector(110.38692736068958,358.2339691966845, 0.0));
    positionAlloc4->Add (Vector(129.63771587340435,288.01346644254676, 0.0));
    positionAlloc4->Add (Vector(200.36452152024543,186.778404603892, 0.0));
    positionAlloc4->Add (Vector(76.44728549035955,96.35382509573769, 0.0));
    positionAlloc4->Add (Vector(265.78477152039386,268.469594700648, 0.0));
    positionAlloc4->Add (Vector(391.88741097021256,238.5107696335508, 0.0));
    positionAlloc4->Add (Vector(66.16588385735893,205.21335612860216, 0.0));
    positionAlloc4->Add (Vector(96.65892522860067,132.97609361486255, 0.0));
    positionAlloc4->Add (Vector(396.5066220516263,348.2857267719141, 0.0));
    positionAlloc4->Add (Vector(132.72206140916575,55.35328507996717, 0.0));
    positionAlloc4->Add (Vector(225.94501715105145,113.95351366419742, 0.0));
    positionAlloc4->Add (Vector(19.188981521919544,390.9841911182569, 0.0));
    positionAlloc4->Add (Vector(356.195058934576,54.92518821880887, 0.0));
    positionAlloc4->Add (Vector(187.300299787205,121.66595326136247, 0.0));
    positionAlloc4->Add (Vector(374.2983990030698,51.72637428553158, 0.0));
    positionAlloc4->Add (Vector(314.86217745794244,391.39075965355704, 0.0));
    positionAlloc4->Add (Vector(349.7159791263876,324.0931656718214, 0.0));
    positionAlloc4->Add (Vector(80.8178221992331,86.00348308588059, 0.0));
    positionAlloc4->Add (Vector(28.04499715442801,322.82412581872444, 0.0));
    positionAlloc4->Add (Vector(291.41690344819125,231.3775162015046, 0.0));
    positionAlloc4->Add (Vector(199.3943793146368,168.70331524821563, 0.0));
    positionAlloc4->Add (Vector(306.9902004411695,359.01325253315224, 0.0));
    positionAlloc4->Add (Vector(352.1961060196663,260.7569988308296, 0.0));
    positionAlloc4->Add (Vector(157.63342011350886,326.05810680545403, 0.0));
    positionAlloc4->Add (Vector(91.6964797105368,367.5784745754582, 0.0));
    positionAlloc4->Add (Vector(284.9264698367629,138.1723980981061, 0.0));
    positionAlloc4->Add (Vector(215.26530322297756,206.211194409725, 0.0));
    positionAlloc4->Add (Vector(204.6761570374502,168.41327092614705, 0.0));
    positionAlloc4->Add (Vector(182.87335570664527,83.82550896100871, 0.0));
    positionAlloc4->Add (Vector(231.5577515764496,166.26155242315238, 0.0));
    positionAlloc4->Add (Vector(186.3148147922024,103.5901583400348, 0.0));
    positionAlloc4->Add (Vector(265.0464833078503,155.7741350514447, 0.0));
    positionAlloc4->Add (Vector(135.21158866424744,104.33861391753241, 0.0));
    positionAlloc4->Add (Vector(181.42573800403835,101.1352443526441, 0.0));
    positionAlloc4->Add (Vector(78.58825235054425,140.21004833351455, 0.0));
    positionAlloc4->Add (Vector(199.35745300137432,241.85303438768068, 0.0));
    positionAlloc4->Add (Vector(53.793356810585145,14.010003804767956, 0.0));
    positionAlloc4->Add (Vector(39.165957771417226,128.18395033348048, 0.0));
    positionAlloc4->Add (Vector(177.33020720458336,82.50920733259677, 0.0));
    positionAlloc4->Add (Vector(149.40519680487813,85.29706409190423, 0.0));
    positionAlloc4->Add (Vector(263.161379219797,53.396099877563195, 0.0));
    positionAlloc4->Add (Vector(110.58302003724583,363.76881409235654, 0.0));
    positionAlloc4->Add (Vector(186.88337952510662,247.62556755334256, 0.0));
    positionAlloc4->Add (Vector(373.05461315021955,166.89144432431368, 0.0));
    positionAlloc4->Add (Vector(230.0484471013172,13.330107027159244, 0.0));
    positionAlloc4->Add (Vector(293.29806783271306,337.36433157317424, 0.0));
    positionAlloc4->Add (Vector(241.42383526060115,271.4662159501001, 0.0));
    positionAlloc4->Add (Vector(246.13669612212118,261.13854929872485, 0.0));
    positionAlloc4->Add (Vector(0.6094954748862236,191.85479171572206, 0.0));
    positionAlloc4->Add (Vector(370.497825630818,93.66527313783371, 0.0));
    positionAlloc4->Add (Vector(30.63156312207811,333.4688085697738, 0.0));
    positionAlloc4->Add (Vector(265.55196631657185,142.26864322004567, 0.0));
    positionAlloc4->Add (Vector(221.86069358587406,285.7163099165262, 0.0));
    positionAlloc4->Add (Vector(206.5551805340287,137.68251274081584, 0.0));
    positionAlloc4->Add (Vector(54.29817145324844,178.39894038762242, 0.0));
    positionAlloc4->Add (Vector(117.04607443241422,205.3662559500482, 0.0));
    positionAlloc4->Add (Vector(38.258536067076236,184.0721756826594, 0.0));
    positionAlloc4->Add (Vector(154.6467926524501,306.23904754771746, 0.0));
    positionAlloc4->Add (Vector(113.9386074618863,267.1892226279209, 0.0));
    positionAlloc4->Add (Vector(39.94319460704854,92.05505516607593, 0.0));
    positionAlloc4->Add (Vector(279.47373128747483,370.1208461241392, 0.0));
    positionAlloc4->Add (Vector(145.7681348768192,79.38929630920173, 0.0));
    positionAlloc4->Add (Vector(121.44984260143303,193.21939328616517, 0.0));
    positionAlloc4->Add (Vector(286.9139515818965,17.580996278359294, 0.0));
    positionAlloc4->Add (Vector(374.8607610943437,94.00156854251618, 0.0));
    positionAlloc4->Add (Vector(308.4894850791168,341.48861045764846, 0.0));
    positionAlloc4->Add (Vector(117.37413046068363,350.990180605238, 0.0));
    positionAlloc4->Add (Vector(275.7799349325654,270.7491710037899, 0.0));
    positionAlloc4->Add (Vector(10.163397305027688,179.3098914079692, 0.0));
    positionAlloc4->Add (Vector(244.13135990624863,196.81711864207628, 0.0));
    positionAlloc4->Add (Vector(364.07675735910874,168.0902646137942, 0.0));
    positionAlloc4->Add (Vector(271.1435546683762,300.0576558873559, 0.0));
    positionAlloc4->Add (Vector(2.4791340352632307,65.14089690643456, 0.0));
    positionAlloc4->Add (Vector(64.93816524897306,162.00386171372605, 0.0));
    positionAlloc4->Add (Vector(28.387933314169047,315.2570860928875, 0.0));
    positionAlloc4->Add (Vector(90.76541342781277,313.3492096988802, 0.0));
    positionAlloc4->Add (Vector(282.57840641910025,264.84375852480025, 0.0));
    positionAlloc4->Add (Vector(149.57244945150404,249.23197570921977, 0.0));
    positionAlloc4->Add (Vector(54.43950346648458,275.2225629200357, 0.0));
    positionAlloc4->Add (Vector(385.2651079458513,104.78052453817956, 0.0));
    positionAlloc4->Add (Vector(57.38877277465999,16.495118655815723, 0.0));
    positionAlloc4->Add (Vector(70.73366033109019,177.2013552294817, 0.0));
    positionAlloc4->Add (Vector(193.91645138826954,191.38954238033597, 0.0));
    positionAlloc4->Add (Vector(17.452831510515754,333.36473864007104, 0.0));
    positionAlloc4->Add (Vector(319.39486928587075,163.69358107832844, 0.0));
    positionAlloc4->Add (Vector(232.18487436575916,185.8644318308202, 0.0));
    positionAlloc4->Add (Vector(198.56256288205287,216.38522493329853, 0.0));
    positionAlloc4->Add (Vector(242.79519186905492,178.52448880203417, 0.0));
    positionAlloc4->Add (Vector(203.73514118234698,102.88245377745602, 0.0));
    positionAlloc4->Add (Vector(348.8618106115536,159.83862942959698, 0.0));
    positionAlloc4->Add (Vector(356.79458012529295,328.55394041422875, 0.0));
    positionAlloc4->Add (Vector(36.36452531956724,185.37938947556643, 0.0));
    positionAlloc4->Add (Vector(21.459759901266693,123.73207117976665, 0.0));
    positionAlloc4->Add (Vector(179.15410178172465,234.81086618674607, 0.0));
    positionAlloc4->Add (Vector(66.77136071933991,244.07331649932385, 0.0));
    positionAlloc4->Add (Vector(149.1055080805307,389.5548831688185, 0.0));
    positionAlloc4->Add (Vector(60.473875425787014,105.4071539318016, 0.0));
    positionAlloc4->Add (Vector(76.50438236382429,107.75645603807646, 0.0));
    positionAlloc4->Add (Vector(37.33093895705264,396.3730042633876, 0.0));
    positionAlloc4->Add (Vector(365.6227469147506,314.5380594958688, 0.0));
    positionAlloc4->Add (Vector(230.96878171857617,194.23229165005088, 0.0));
    positionAlloc4->Add (Vector(181.01977803591183,220.33354873407305, 0.0));
    positionAlloc4->Add (Vector(375.4104659620402,348.94046048900844, 0.0));
    positionAlloc4->Add (Vector(292.4742991127271,357.86761222897934, 0.0));
    positionAlloc4->Add (Vector(150.52364735301427,267.6331404430572, 0.0));
    positionAlloc4->Add (Vector(171.60092805595872,339.80794998172763, 0.0));
    positionAlloc4->Add (Vector(104.82613052953482,123.01481249195905, 0.0));
    positionAlloc4->Add (Vector(78.72417292246246,364.1563660801953, 0.0));
    positionAlloc4->Add (Vector(4.524713084348875,331.1320797249905, 0.0));
    positionAlloc4->Add (Vector(264.5204040200131,103.34536709374635, 0.0));
    positionAlloc4->Add (Vector(30.206087403620742,369.8795105436279, 0.0));
    positionAlloc4->Add (Vector(396.98148248768416,229.3244788686692, 0.0));
    positionAlloc4->Add (Vector(350.2880040957635,50.53163446821798, 0.0));
    positionAlloc4->Add (Vector(85.04701489599836,232.22447840237345, 0.0));
    positionAlloc4->Add (Vector(284.5849734111871,203.806947222131, 0.0));
    positionAlloc4->Add (Vector(106.71472565233127,119.88263888718946, 0.0));
    positionAlloc4->Add (Vector(244.50032492112666,80.71918076523525, 0.0));
    positionAlloc4->Add (Vector(167.60494093174736,378.78293937369375, 0.0));
    positionAlloc4->Add (Vector(192.2424013509307,152.61148567033644, 0.0));
    positionAlloc4->Add (Vector(76.50911245245511,240.71425996029765, 0.0));
    positionAlloc4->Add (Vector(199.4147441700155,96.71650865878361, 0.0));
    positionAlloc4->Add (Vector(160.1347904059038,79.74245698391016, 0.0));
    positionAlloc4->Add (Vector(121.81629351581526,34.861019556921, 0.0));
    positionAlloc4->Add (Vector(287.41779251043107,18.73687892814995, 0.0));
    positionAlloc4->Add (Vector(362.2145348048593,193.6108681106683, 0.0));
    positionAlloc4->Add (Vector(243.4994132712496,301.616091334044, 0.0));
    positionAlloc4->Add (Vector(89.5648965329265,22.014287163995228, 0.0));
    positionAlloc4->Add (Vector(38.45256441081011,352.58961448051, 0.0));
    positionAlloc4->Add (Vector(98.61519138932806,35.55248336934862, 0.0));
    positionAlloc4->Add (Vector(353.3179383295562,296.3880463176554, 0.0));
    positionAlloc4->Add (Vector(98.627077648332,399.3556681366046, 0.0));
    positionAlloc4->Add (Vector(116.10274646024821,158.04729399040133, 0.0));
    positionAlloc4->Add (Vector(249.8052670084503,210.6609563275629, 0.0));
    positionAlloc4->Add (Vector(16.47573344182898,77.76493948332535, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_14.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_14.flowmon", false, false);
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