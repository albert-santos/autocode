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
	  uint16_t numberOfNodes = 600;
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
    positionAlloc->Add (Vector (356.6666666666667,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,140.0, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (270.0,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (270.0,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,270.0, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,140.0, 0.0));
    positionAlloc->Add (Vector (140.0,270.0, 0.0));
    positionAlloc->Add (Vector (140.0,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,140.0, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,270.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,140.0, 0.0));
    positionAlloc->Add (Vector (10.0,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (10.0,313.3333333333333, 0.0));
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
    positionAlloc4->Add (Vector(7.583657491446916,80.17986827859937, 0.0));
    positionAlloc4->Add (Vector(313.1700094199214,175.11743284403352, 0.0));
    positionAlloc4->Add (Vector(159.4137576864874,4.673094845145931, 0.0));
    positionAlloc4->Add (Vector(280.55976165600714,193.80254707650707, 0.0));
    positionAlloc4->Add (Vector(101.6496793971414,114.20537377964943, 0.0));
    positionAlloc4->Add (Vector(184.37997918505943,161.59355234813358, 0.0));
    positionAlloc4->Add (Vector(382.24231179006705,120.85834346483057, 0.0));
    positionAlloc4->Add (Vector(155.89955602691975,354.3242052003284, 0.0));
    positionAlloc4->Add (Vector(295.82901417848535,343.15112472850615, 0.0));
    positionAlloc4->Add (Vector(149.91191313413594,244.06147431016194, 0.0));
    positionAlloc4->Add (Vector(203.50561790949317,186.738037269972, 0.0));
    positionAlloc4->Add (Vector(268.43164551475337,127.51489680389105, 0.0));
    positionAlloc4->Add (Vector(64.12887200869504,324.8168801173395, 0.0));
    positionAlloc4->Add (Vector(332.1273028729208,340.03188651153147, 0.0));
    positionAlloc4->Add (Vector(58.03672048138684,283.13988500303157, 0.0));
    positionAlloc4->Add (Vector(231.0279614196319,262.68887191788474, 0.0));
    positionAlloc4->Add (Vector(160.25188682595837,299.5077351991255, 0.0));
    positionAlloc4->Add (Vector(391.1841533058994,118.80766624397539, 0.0));
    positionAlloc4->Add (Vector(314.408816893502,259.9269696339124, 0.0));
    positionAlloc4->Add (Vector(269.7453374361865,383.8344370227441, 0.0));
    positionAlloc4->Add (Vector(369.23678311249773,395.64795050718305, 0.0));
    positionAlloc4->Add (Vector(384.34468362851004,40.69553844833096, 0.0));
    positionAlloc4->Add (Vector(71.54298600370245,363.8048805181113, 0.0));
    positionAlloc4->Add (Vector(234.54213493998398,237.30513278117127, 0.0));
    positionAlloc4->Add (Vector(318.81960216768607,343.95264655210565, 0.0));
    positionAlloc4->Add (Vector(57.63245254435398,371.0848528323683, 0.0));
    positionAlloc4->Add (Vector(367.5722473647262,327.79688527730906, 0.0));
    positionAlloc4->Add (Vector(250.31577839606248,114.29876980797187, 0.0));
    positionAlloc4->Add (Vector(355.92997134438707,223.9529413936409, 0.0));
    positionAlloc4->Add (Vector(394.018372544866,31.781253635192108, 0.0));
    positionAlloc4->Add (Vector(343.1938501946892,354.8621617900335, 0.0));
    positionAlloc4->Add (Vector(325.5375304501648,79.69994895479249, 0.0));
    positionAlloc4->Add (Vector(82.10125412051772,104.58423576896716, 0.0));
    positionAlloc4->Add (Vector(100.66916175601905,113.54541305156363, 0.0));
    positionAlloc4->Add (Vector(206.7528519554283,22.918290976452216, 0.0));
    positionAlloc4->Add (Vector(252.51148872460098,205.53871514474386, 0.0));
    positionAlloc4->Add (Vector(362.428169973156,395.5893826274059, 0.0));
    positionAlloc4->Add (Vector(328.42572475453875,123.46657066599236, 0.0));
    positionAlloc4->Add (Vector(177.88665782168368,65.32812545251083, 0.0));
    positionAlloc4->Add (Vector(173.78200203637402,124.1746302157499, 0.0));
    positionAlloc4->Add (Vector(93.04065518519411,105.20569633819777, 0.0));
    positionAlloc4->Add (Vector(302.16657630482666,265.1108247591494, 0.0));
    positionAlloc4->Add (Vector(321.9065240362033,293.93785284870484, 0.0));
    positionAlloc4->Add (Vector(361.5019415663132,93.25777953373056, 0.0));
    positionAlloc4->Add (Vector(289.0062098598194,309.15819596264805, 0.0));
    positionAlloc4->Add (Vector(203.42405936309996,1.0940523466409946, 0.0));
    positionAlloc4->Add (Vector(232.0101729784954,55.8686457631286, 0.0));
    positionAlloc4->Add (Vector(387.5673890957845,324.5961218829615, 0.0));
    positionAlloc4->Add (Vector(107.60791111058813,73.80349275850394, 0.0));
    positionAlloc4->Add (Vector(257.5038225329538,101.58848343492824, 0.0));
    positionAlloc4->Add (Vector(145.50304423242855,237.14005495148794, 0.0));
    positionAlloc4->Add (Vector(160.35331949993972,393.6983370168638, 0.0));
    positionAlloc4->Add (Vector(119.94491980587844,334.3376976032213, 0.0));
    positionAlloc4->Add (Vector(254.35392325043517,97.51014792580767, 0.0));
    positionAlloc4->Add (Vector(41.53785631920353,32.28275201452222, 0.0));
    positionAlloc4->Add (Vector(136.90229726841005,156.8133056430852, 0.0));
    positionAlloc4->Add (Vector(383.3829670806232,49.887813030798476, 0.0));
    positionAlloc4->Add (Vector(319.43842594217335,309.6141496580032, 0.0));
    positionAlloc4->Add (Vector(10.496469948035125,332.31583986424755, 0.0));
    positionAlloc4->Add (Vector(205.48315025670814,317.6083229128869, 0.0));
    positionAlloc4->Add (Vector(165.79887839374047,235.04271594940212, 0.0));
    positionAlloc4->Add (Vector(234.61796122628124,349.6729403755925, 0.0));
    positionAlloc4->Add (Vector(338.3321543611691,61.69786582679109, 0.0));
    positionAlloc4->Add (Vector(218.373740590284,175.02313405522818, 0.0));
    positionAlloc4->Add (Vector(12.165972693939509,3.4258320365995854, 0.0));
    positionAlloc4->Add (Vector(289.9977869568893,235.23730468073646, 0.0));
    positionAlloc4->Add (Vector(79.13932065903757,144.80205552987698, 0.0));
    positionAlloc4->Add (Vector(198.15514866207852,393.44157676922305, 0.0));
    positionAlloc4->Add (Vector(6.0219981718451,346.1864614892286, 0.0));
    positionAlloc4->Add (Vector(96.87696306046827,234.9604189292331, 0.0));
    positionAlloc4->Add (Vector(209.79039913827253,171.76120118326108, 0.0));
    positionAlloc4->Add (Vector(194.3869373784335,112.19037191112795, 0.0));
    positionAlloc4->Add (Vector(24.632292787605437,351.10876384088186, 0.0));
    positionAlloc4->Add (Vector(125.06325188154186,375.29742525125465, 0.0));
    positionAlloc4->Add (Vector(315.34379651625045,322.4492443605955, 0.0));
    positionAlloc4->Add (Vector(307.80950242472386,278.37136663846337, 0.0));
    positionAlloc4->Add (Vector(110.45848575197846,199.2933717898388, 0.0));
    positionAlloc4->Add (Vector(263.4736852281212,130.86937399690143, 0.0));
    positionAlloc4->Add (Vector(213.1597162677755,15.886544904763289, 0.0));
    positionAlloc4->Add (Vector(68.17579216384271,67.05617026741733, 0.0));
    positionAlloc4->Add (Vector(147.65753301287018,296.74419835898533, 0.0));
    positionAlloc4->Add (Vector(224.63588690266567,228.14911771251184, 0.0));
    positionAlloc4->Add (Vector(256.2537129516397,238.0426437425899, 0.0));
    positionAlloc4->Add (Vector(215.76049367010276,111.37280961553486, 0.0));
    positionAlloc4->Add (Vector(387.7586724795615,136.3893121596359, 0.0));
    positionAlloc4->Add (Vector(74.851661047042,243.18764916974987, 0.0));
    positionAlloc4->Add (Vector(227.2190879423879,66.25972492813128, 0.0));
    positionAlloc4->Add (Vector(313.410987672201,34.78915407816912, 0.0));
    positionAlloc4->Add (Vector(138.90163038256085,331.9140543618821, 0.0));
    positionAlloc4->Add (Vector(198.3110947631728,131.34526910261118, 0.0));
    positionAlloc4->Add (Vector(156.58103630950012,56.53557464359933, 0.0));
    positionAlloc4->Add (Vector(314.661990294872,344.72058663950963, 0.0));
    positionAlloc4->Add (Vector(163.78577591031674,94.57739529502254, 0.0));
    positionAlloc4->Add (Vector(232.0130204395336,201.15082819700058, 0.0));
    positionAlloc4->Add (Vector(117.94340071047115,225.4301471892239, 0.0));
    positionAlloc4->Add (Vector(389.8824386667269,186.80655060722913, 0.0));
    positionAlloc4->Add (Vector(163.96505622137218,326.44876748419784, 0.0));
    positionAlloc4->Add (Vector(304.64666843966313,158.90856358063377, 0.0));
    positionAlloc4->Add (Vector(374.66781189897154,358.0799080529402, 0.0));
    positionAlloc4->Add (Vector(75.68206177728811,127.37081550418719, 0.0));
    positionAlloc4->Add (Vector(173.0325522494228,250.73893797323464, 0.0));
    positionAlloc4->Add (Vector(239.30742691020694,182.90030441120905, 0.0));
    positionAlloc4->Add (Vector(22.737244035259565,335.287441203211, 0.0));
    positionAlloc4->Add (Vector(333.80127011523746,356.20689408976534, 0.0));
    positionAlloc4->Add (Vector(19.109831501019457,316.9588327668325, 0.0));
    positionAlloc4->Add (Vector(327.2109001684107,297.6683189479293, 0.0));
    positionAlloc4->Add (Vector(266.59559595751216,28.350270533853994, 0.0));
    positionAlloc4->Add (Vector(79.94895834510886,110.21711575581423, 0.0));
    positionAlloc4->Add (Vector(98.04937073110396,126.89935468783978, 0.0));
    positionAlloc4->Add (Vector(71.3451231414565,72.74356789801297, 0.0));
    positionAlloc4->Add (Vector(174.5116767715757,3.1258302723786713, 0.0));
    positionAlloc4->Add (Vector(301.23970616126024,90.07826293615686, 0.0));
    positionAlloc4->Add (Vector(302.4655038806148,106.94022793782887, 0.0));
    positionAlloc4->Add (Vector(92.81614452259785,219.19742743859746, 0.0));
    positionAlloc4->Add (Vector(349.65064355008946,374.4806412755876, 0.0));
    positionAlloc4->Add (Vector(120.07916450625311,88.57140870831999, 0.0));
    positionAlloc4->Add (Vector(44.51069856331982,242.62483806478764, 0.0));
    positionAlloc4->Add (Vector(234.40160808816316,144.30836699899635, 0.0));
    positionAlloc4->Add (Vector(180.8780808356518,331.6040573930485, 0.0));
    positionAlloc4->Add (Vector(378.45120496837626,0.4045468455679657, 0.0));
    positionAlloc4->Add (Vector(224.61882239981028,150.5991915060995, 0.0));
    positionAlloc4->Add (Vector(237.6878914860411,145.4152074887357, 0.0));
    positionAlloc4->Add (Vector(178.9592547214864,106.1685203631868, 0.0));
    positionAlloc4->Add (Vector(164.62587804706007,150.14571550895596, 0.0));
    positionAlloc4->Add (Vector(113.85261557445823,391.32587337946137, 0.0));
    positionAlloc4->Add (Vector(224.1665087225583,39.08179358704382, 0.0));
    positionAlloc4->Add (Vector(53.48029344387988,55.023252004661714, 0.0));
    positionAlloc4->Add (Vector(151.5659656157127,282.48436747833654, 0.0));
    positionAlloc4->Add (Vector(31.033052443770526,79.11952592075369, 0.0));
    positionAlloc4->Add (Vector(105.06319634255541,44.46477952010284, 0.0));
    positionAlloc4->Add (Vector(43.07190513218857,223.52140064062408, 0.0));
    positionAlloc4->Add (Vector(89.75077227000745,335.76519480218246, 0.0));
    positionAlloc4->Add (Vector(39.59615815893041,220.27260665500808, 0.0));
    positionAlloc4->Add (Vector(270.88043986021165,319.2484558154651, 0.0));
    positionAlloc4->Add (Vector(377.05578682269055,64.95566645343138, 0.0));
    positionAlloc4->Add (Vector(263.4171206936136,200.7233476428398, 0.0));
    positionAlloc4->Add (Vector(215.11953908272253,31.984884588162288, 0.0));
    positionAlloc4->Add (Vector(245.09088526410548,241.18216306253962, 0.0));
    positionAlloc4->Add (Vector(57.43279736203015,223.93284945226384, 0.0));
    positionAlloc4->Add (Vector(225.40207519770576,295.0632416553428, 0.0));
    positionAlloc4->Add (Vector(347.66240792146056,343.426050721487, 0.0));
    positionAlloc4->Add (Vector(27.00633742319738,15.912190633297296, 0.0));
    positionAlloc4->Add (Vector(77.28549868618599,102.95272553607884, 0.0));
    positionAlloc4->Add (Vector(82.56511214922244,296.2295725824598, 0.0));
    positionAlloc4->Add (Vector(83.80725481990994,270.8293278152356, 0.0));
    positionAlloc4->Add (Vector(132.09755638077652,82.17951547982278, 0.0));
    positionAlloc4->Add (Vector(262.01914441159573,17.644616971152026, 0.0));
    positionAlloc4->Add (Vector(251.64131044888202,157.72806915097507, 0.0));
    positionAlloc4->Add (Vector(27.42147643561088,301.44638523323226, 0.0));
    positionAlloc4->Add (Vector(0.8414549572325569,122.35987734960348, 0.0));
    positionAlloc4->Add (Vector(310.57040981883773,369.945234879305, 0.0));
    positionAlloc4->Add (Vector(102.9580880444131,38.353573776287185, 0.0));
    positionAlloc4->Add (Vector(354.5494945969522,376.1216666315701, 0.0));
    positionAlloc4->Add (Vector(52.62335688478834,138.08468810435795, 0.0));
    positionAlloc4->Add (Vector(356.5479914635104,69.56714705867722, 0.0));
    positionAlloc4->Add (Vector(70.47189122792012,63.2929214800086, 0.0));
    positionAlloc4->Add (Vector(233.48980300277867,373.34555670319224, 0.0));
    positionAlloc4->Add (Vector(228.5346674178245,308.9623281850595, 0.0));
    positionAlloc4->Add (Vector(196.50394006138603,295.75844744070395, 0.0));
    positionAlloc4->Add (Vector(373.06036605788773,267.69343750986764, 0.0));
    positionAlloc4->Add (Vector(251.51016765608162,252.61646105393268, 0.0));
    positionAlloc4->Add (Vector(304.38837863961703,10.691017446753737, 0.0));
    positionAlloc4->Add (Vector(58.100963196177034,375.56599685314853, 0.0));
    positionAlloc4->Add (Vector(148.11505572097917,56.75320348796129, 0.0));
    positionAlloc4->Add (Vector(373.00873453647966,314.02567409462006, 0.0));
    positionAlloc4->Add (Vector(46.64443762104491,185.88000336632874, 0.0));
    positionAlloc4->Add (Vector(142.56641326223257,10.082032395100837, 0.0));
    positionAlloc4->Add (Vector(191.6298003343743,188.286070454962, 0.0));
    positionAlloc4->Add (Vector(54.05952475171634,181.45471476040868, 0.0));
    positionAlloc4->Add (Vector(153.76466778772658,240.77013581818426, 0.0));
    positionAlloc4->Add (Vector(92.44405641409142,338.81305130970236, 0.0));
    positionAlloc4->Add (Vector(89.34469376217456,135.84058810635952, 0.0));
    positionAlloc4->Add (Vector(341.9443264018952,257.6480904654741, 0.0));
    positionAlloc4->Add (Vector(156.55247732508553,326.23705346634443, 0.0));
    positionAlloc4->Add (Vector(49.70465509207598,109.29612540721143, 0.0));
    positionAlloc4->Add (Vector(70.46255878862335,276.3254462312404, 0.0));
    positionAlloc4->Add (Vector(308.8556213467658,189.37009577494388, 0.0));
    positionAlloc4->Add (Vector(143.66281337682807,278.7692818952335, 0.0));
    positionAlloc4->Add (Vector(97.47138661544987,84.82841239882526, 0.0));
    positionAlloc4->Add (Vector(154.5091380566649,277.72811348233176, 0.0));
    positionAlloc4->Add (Vector(353.19341529082755,6.947551287580467, 0.0));
    positionAlloc4->Add (Vector(273.7635468415096,283.760196186058, 0.0));
    positionAlloc4->Add (Vector(117.2859220878764,332.1884410560074, 0.0));
    positionAlloc4->Add (Vector(370.2449283415018,192.6300107476102, 0.0));
    positionAlloc4->Add (Vector(330.08557835019326,72.02258264290067, 0.0));
    positionAlloc4->Add (Vector(220.23834798164378,20.603305226358117, 0.0));
    positionAlloc4->Add (Vector(262.70052292993944,337.7948659075803, 0.0));
    positionAlloc4->Add (Vector(396.9788811088911,82.73316433438565, 0.0));
    positionAlloc4->Add (Vector(287.63643706124856,308.22309118746574, 0.0));
    positionAlloc4->Add (Vector(224.06258771294105,332.16033244202123, 0.0));
    positionAlloc4->Add (Vector(67.60780008677494,207.54800581782226, 0.0));
    positionAlloc4->Add (Vector(306.3349945644226,361.46449912764353, 0.0));
    positionAlloc4->Add (Vector(246.507264041442,158.39782100514083, 0.0));
    positionAlloc4->Add (Vector(94.0523873553739,333.26947315637074, 0.0));
    positionAlloc4->Add (Vector(345.80421161960544,349.35557165229, 0.0));
    positionAlloc4->Add (Vector(49.373034338569745,148.71202901038606, 0.0));
    positionAlloc4->Add (Vector(94.4926165535093,72.21401218629731, 0.0));
    positionAlloc4->Add (Vector(295.30216464892726,31.125916809085385, 0.0));
    positionAlloc4->Add (Vector(292.5864296544828,57.217222478417185, 0.0));
    positionAlloc4->Add (Vector(52.81504424800283,86.1407795968839, 0.0));
    positionAlloc4->Add (Vector(373.52722109717115,145.95690765707695, 0.0));
    positionAlloc4->Add (Vector(104.80409725315809,273.24112452000435, 0.0));
    positionAlloc4->Add (Vector(317.5177768962298,176.34300312812462, 0.0));
    positionAlloc4->Add (Vector(312.24512331138123,222.40603271654845, 0.0));
    positionAlloc4->Add (Vector(227.60807592024616,21.955375731416726, 0.0));
    positionAlloc4->Add (Vector(279.74855517725587,316.39543729539764, 0.0));
    positionAlloc4->Add (Vector(98.81297869888508,258.34836842007707, 0.0));
    positionAlloc4->Add (Vector(376.0662750120935,293.5966147942469, 0.0));
    positionAlloc4->Add (Vector(304.4139007046763,393.5446847924945, 0.0));
    positionAlloc4->Add (Vector(186.46111973892067,187.8843657400381, 0.0));
    positionAlloc4->Add (Vector(250.93854716448493,318.8217342405711, 0.0));
    positionAlloc4->Add (Vector(19.608860784791737,146.61575877745045, 0.0));
    positionAlloc4->Add (Vector(44.81246503470322,344.5600939649866, 0.0));
    positionAlloc4->Add (Vector(188.21412013202462,313.777176856439, 0.0));
    positionAlloc4->Add (Vector(53.048112028732895,1.624077005594149, 0.0));
    positionAlloc4->Add (Vector(239.67641914641584,264.0274874383403, 0.0));
    positionAlloc4->Add (Vector(278.1382379385086,120.30601663701184, 0.0));
    positionAlloc4->Add (Vector(164.10553092382366,219.67089093865337, 0.0));
    positionAlloc4->Add (Vector(374.5688187404032,170.11614012759827, 0.0));
    positionAlloc4->Add (Vector(352.6354181634892,80.83283813534501, 0.0));
    positionAlloc4->Add (Vector(395.72479830535286,30.00617957631877, 0.0));
    positionAlloc4->Add (Vector(132.91321771702061,2.664381545383243, 0.0));
    positionAlloc4->Add (Vector(7.962028466695292,318.6822744955569, 0.0));
    positionAlloc4->Add (Vector(392.1072902781949,349.6380131903229, 0.0));
    positionAlloc4->Add (Vector(393.6391636620739,286.5230976241146, 0.0));
    positionAlloc4->Add (Vector(141.42637724409067,46.773439764585234, 0.0));
    positionAlloc4->Add (Vector(272.54659043542347,85.5918506117277, 0.0));
    positionAlloc4->Add (Vector(241.25616362076437,253.06167881742755, 0.0));
    positionAlloc4->Add (Vector(269.644714539302,75.75042364702273, 0.0));
    positionAlloc4->Add (Vector(307.292087151622,210.66954915688768, 0.0));
    positionAlloc4->Add (Vector(12.225084634065508,371.8966257858422, 0.0));
    positionAlloc4->Add (Vector(77.83603419097047,156.91452066886802, 0.0));
    positionAlloc4->Add (Vector(58.676713472099266,236.17464910198058, 0.0));
    positionAlloc4->Add (Vector(34.27170174491509,236.74234540445772, 0.0));
    positionAlloc4->Add (Vector(67.54340538589743,291.3826329522079, 0.0));
    positionAlloc4->Add (Vector(86.80915379453205,70.7567706829249, 0.0));
    positionAlloc4->Add (Vector(271.1537913965184,244.17358535638846, 0.0));
    positionAlloc4->Add (Vector(344.61104903199436,122.46148985371707, 0.0));
    positionAlloc4->Add (Vector(35.558902771523115,277.981793440633, 0.0));
    positionAlloc4->Add (Vector(258.8925939122887,242.17839301808291, 0.0));
    positionAlloc4->Add (Vector(194.77355424544385,190.73723172337594, 0.0));
    positionAlloc4->Add (Vector(92.44750380681785,344.6424525180083, 0.0));
    positionAlloc4->Add (Vector(322.40724347611456,395.234361164478, 0.0));
    positionAlloc4->Add (Vector(167.9548090750767,50.02315652515197, 0.0));
    positionAlloc4->Add (Vector(158.4895156566362,235.28874624113345, 0.0));
    positionAlloc4->Add (Vector(328.115904525527,67.28287038372018, 0.0));
    positionAlloc4->Add (Vector(319.25033826497196,209.85480724439128, 0.0));
    positionAlloc4->Add (Vector(335.88997403347054,256.44411861925016, 0.0));
    positionAlloc4->Add (Vector(54.024178235950956,193.23273387992086, 0.0));
    positionAlloc4->Add (Vector(183.92136730881313,382.7368712704756, 0.0));
    positionAlloc4->Add (Vector(64.74858795254606,68.88004195179538, 0.0));
    positionAlloc4->Add (Vector(242.33451857253425,47.93927529485123, 0.0));
    positionAlloc4->Add (Vector(303.63565751716743,68.34171287042237, 0.0));
    positionAlloc4->Add (Vector(8.163699250435963,62.0037505197105, 0.0));
    positionAlloc4->Add (Vector(141.15758323198517,134.5419987477069, 0.0));
    positionAlloc4->Add (Vector(65.45418072087989,137.43622566062504, 0.0));
    positionAlloc4->Add (Vector(110.98864764912699,159.5025899374146, 0.0));
    positionAlloc4->Add (Vector(179.95209684420627,378.5222720560031, 0.0));
    positionAlloc4->Add (Vector(101.89787385524029,216.73399726191778, 0.0));
    positionAlloc4->Add (Vector(82.64749647872294,20.69125534905063, 0.0));
    positionAlloc4->Add (Vector(28.974870444808644,369.0272652150314, 0.0));
    positionAlloc4->Add (Vector(224.78140611477042,62.924629514142836, 0.0));
    positionAlloc4->Add (Vector(243.54676150499714,261.9870320256215, 0.0));
    positionAlloc4->Add (Vector(385.35374299340697,349.37394335110605, 0.0));
    positionAlloc4->Add (Vector(337.8389778564464,86.54670555989057, 0.0));
    positionAlloc4->Add (Vector(7.276839147455805,190.17097907572355, 0.0));
    positionAlloc4->Add (Vector(277.2255895289785,182.57717645327972, 0.0));
    positionAlloc4->Add (Vector(332.98448483653914,69.48216636528373, 0.0));
    positionAlloc4->Add (Vector(249.0009024601646,250.70995018878298, 0.0));
    positionAlloc4->Add (Vector(176.95970543216944,185.13702550261524, 0.0));
    positionAlloc4->Add (Vector(0.7451628854793224,337.9889385102603, 0.0));
    positionAlloc4->Add (Vector(365.03021813034553,317.4712778819182, 0.0));
    positionAlloc4->Add (Vector(390.4404182532138,190.67190260975465, 0.0));
    positionAlloc4->Add (Vector(86.46619748340756,338.9181454907028, 0.0));
    positionAlloc4->Add (Vector(44.26563791606317,123.10780584356311, 0.0));
    positionAlloc4->Add (Vector(246.69419675033305,273.5494988479577, 0.0));
    positionAlloc4->Add (Vector(103.41655428937435,133.24296523641888, 0.0));
    positionAlloc4->Add (Vector(25.656727039412928,240.62379473872792, 0.0));
    positionAlloc4->Add (Vector(231.62094812701253,286.3803127486243, 0.0));
    positionAlloc4->Add (Vector(381.6752923407729,256.21842711026386, 0.0));
    positionAlloc4->Add (Vector(130.19017875545666,169.69611521439876, 0.0));
    positionAlloc4->Add (Vector(167.60133930915532,15.670660703605721, 0.0));
    positionAlloc4->Add (Vector(45.200173300337724,337.4026771444819, 0.0));
    positionAlloc4->Add (Vector(144.1493674313266,205.3855187707816, 0.0));
    positionAlloc4->Add (Vector(284.61024600856894,128.51862331692865, 0.0));
    positionAlloc4->Add (Vector(184.86725934770996,380.53224953828584, 0.0));
    positionAlloc4->Add (Vector(190.74331505219973,124.82936565267741, 0.0));
    positionAlloc4->Add (Vector(292.1273984308883,17.273799939818836, 0.0));
    positionAlloc4->Add (Vector(21.348177533084645,68.68807470943854, 0.0));
    positionAlloc4->Add (Vector(242.93125187046192,44.92177318920483, 0.0));
    positionAlloc4->Add (Vector(281.6435874720192,261.81757692798004, 0.0));
    positionAlloc4->Add (Vector(247.6857422535133,228.04560405729273, 0.0));
    positionAlloc4->Add (Vector(234.80399766632365,263.81553312863315, 0.0));
    positionAlloc4->Add (Vector(71.89223319257061,396.1021397044815, 0.0));
    positionAlloc4->Add (Vector(38.89080477707947,267.44433109078057, 0.0));
    positionAlloc4->Add (Vector(111.87172982056657,140.80725788354988, 0.0));
    positionAlloc4->Add (Vector(297.5912985546607,366.8586971018929, 0.0));
    positionAlloc4->Add (Vector(143.11797707966477,115.22547304960162, 0.0));
    positionAlloc4->Add (Vector(209.84500907456112,210.01644510811067, 0.0));
    positionAlloc4->Add (Vector(119.32335096936728,157.69503191820894, 0.0));
    positionAlloc4->Add (Vector(372.8132938449437,126.23889248226075, 0.0));
    positionAlloc4->Add (Vector(150.07298496303005,242.37347407183685, 0.0));
    positionAlloc4->Add (Vector(293.4380936785725,58.11447256271358, 0.0));
    positionAlloc4->Add (Vector(361.53206888545765,141.58379111231926, 0.0));
    positionAlloc4->Add (Vector(349.43507165169876,270.8138379800777, 0.0));
    positionAlloc4->Add (Vector(253.9678675367037,304.5184501310468, 0.0));
    positionAlloc4->Add (Vector(108.12902508986255,321.7504144263051, 0.0));
    positionAlloc4->Add (Vector(213.10204401454084,18.331268854885963, 0.0));
    positionAlloc4->Add (Vector(217.11300567100923,192.30742515248792, 0.0));
    positionAlloc4->Add (Vector(121.55921086827597,342.9644672891603, 0.0));
    positionAlloc4->Add (Vector(70.38184412971727,382.554441146549, 0.0));
    positionAlloc4->Add (Vector(48.864802816653395,361.7985321843144, 0.0));
    positionAlloc4->Add (Vector(1.149697780853698,228.0480150426535, 0.0));
    positionAlloc4->Add (Vector(286.19168127088363,227.23924097579246, 0.0));
    positionAlloc4->Add (Vector(77.16396570484659,88.82447819214919, 0.0));
    positionAlloc4->Add (Vector(340.88936873233035,108.99981822771517, 0.0));
    positionAlloc4->Add (Vector(100.6267970431391,174.27117220159693, 0.0));
    positionAlloc4->Add (Vector(95.25125446038336,300.1715186693007, 0.0));
    positionAlloc4->Add (Vector(379.4423258847646,233.9200638164117, 0.0));
    positionAlloc4->Add (Vector(236.9419116221144,336.8987636756067, 0.0));
    positionAlloc4->Add (Vector(313.23516084571617,183.80586339112645, 0.0));
    positionAlloc4->Add (Vector(146.35967139022497,358.33968885460405, 0.0));
    positionAlloc4->Add (Vector(382.69345379435407,264.74165959166396, 0.0));
    positionAlloc4->Add (Vector(252.75395975745286,104.0847242304582, 0.0));
    positionAlloc4->Add (Vector(52.911680902786664,7.4662195659775055, 0.0));
    positionAlloc4->Add (Vector(277.30337562149236,219.64822459711235, 0.0));
    positionAlloc4->Add (Vector(377.95904169710104,222.33276420859292, 0.0));
    positionAlloc4->Add (Vector(348.54087923294867,89.86754953262101, 0.0));
    positionAlloc4->Add (Vector(76.06929359844656,356.4973154831483, 0.0));
    positionAlloc4->Add (Vector(64.15612908127164,232.30831371575752, 0.0));
    positionAlloc4->Add (Vector(14.377154544685933,149.2198563502492, 0.0));
    positionAlloc4->Add (Vector(0.2858257321819657,164.93945556871034, 0.0));
    positionAlloc4->Add (Vector(109.17893106952464,165.05046825180645, 0.0));
    positionAlloc4->Add (Vector(325.4720154312985,119.80666898650578, 0.0));
    positionAlloc4->Add (Vector(178.07966524912865,22.90420032943956, 0.0));
    positionAlloc4->Add (Vector(359.36467251648054,167.75355919778033, 0.0));
    positionAlloc4->Add (Vector(41.92363470665268,297.7723517626691, 0.0));
    positionAlloc4->Add (Vector(235.1449172419146,341.2986588486874, 0.0));
    positionAlloc4->Add (Vector(10.366775319724075,294.47563011756654, 0.0));
    positionAlloc4->Add (Vector(230.19892795742894,43.37796395569251, 0.0));
    positionAlloc4->Add (Vector(324.2475811122925,1.5112119512206146, 0.0));
    positionAlloc4->Add (Vector(125.68971946714372,279.20345935617024, 0.0));
    positionAlloc4->Add (Vector(222.6493739049986,256.8379810968667, 0.0));
    positionAlloc4->Add (Vector(231.9342813317923,45.207653639499455, 0.0));
    positionAlloc4->Add (Vector(209.48141571094925,251.47344048768852, 0.0));
    positionAlloc4->Add (Vector(270.84148018628554,180.11778953562398, 0.0));
    positionAlloc4->Add (Vector(40.50202060854113,157.34892370003553, 0.0));
    positionAlloc4->Add (Vector(77.38407770470106,209.36210369808396, 0.0));
    positionAlloc4->Add (Vector(197.6608603907431,108.12821387502409, 0.0));
    positionAlloc4->Add (Vector(197.5389215761989,145.55439913259235, 0.0));
    positionAlloc4->Add (Vector(317.4830265281269,226.07519136481238, 0.0));
    positionAlloc4->Add (Vector(284.124168253992,22.106063574885138, 0.0));
    positionAlloc4->Add (Vector(3.1251933196160486,49.919022874266574, 0.0));
    positionAlloc4->Add (Vector(148.29547188413522,372.8737138875342, 0.0));
    positionAlloc4->Add (Vector(65.90139188415525,351.9179361571266, 0.0));
    positionAlloc4->Add (Vector(257.8665793688362,210.5795340962548, 0.0));
    positionAlloc4->Add (Vector(210.13343608244998,179.04734041569438, 0.0));
    positionAlloc4->Add (Vector(60.67708890286214,175.90951913243777, 0.0));
    positionAlloc4->Add (Vector(63.482241688380284,268.8398948790051, 0.0));
    positionAlloc4->Add (Vector(318.725484032738,210.47692588839405, 0.0));
    positionAlloc4->Add (Vector(2.845718157396826,243.7047798351502, 0.0));
    positionAlloc4->Add (Vector(282.1240903686575,252.26169924508804, 0.0));
    positionAlloc4->Add (Vector(33.070813567986114,348.4168800310351, 0.0));
    positionAlloc4->Add (Vector(366.8139594964655,82.65393199023853, 0.0));
    positionAlloc4->Add (Vector(79.65241608494482,227.5579986864953, 0.0));
    positionAlloc4->Add (Vector(97.56733226472609,70.62160493247625, 0.0));
    positionAlloc4->Add (Vector(243.18465285958752,19.751380224940227, 0.0));
    positionAlloc4->Add (Vector(192.3954314035238,275.3512538169359, 0.0));
    positionAlloc4->Add (Vector(253.1503662363158,68.20221600844386, 0.0));
    positionAlloc4->Add (Vector(190.40864965496013,155.05799586576222, 0.0));
    positionAlloc4->Add (Vector(63.71636243183549,276.4784201234849, 0.0));
    positionAlloc4->Add (Vector(298.24270951116057,127.21420211922987, 0.0));
    positionAlloc4->Add (Vector(311.34729451929604,265.5262681079643, 0.0));
    positionAlloc4->Add (Vector(253.14385907480306,173.54113301126674, 0.0));
    positionAlloc4->Add (Vector(317.1043246513544,355.4262744393056, 0.0));
    positionAlloc4->Add (Vector(386.0941202874128,250.89411123582028, 0.0));
    positionAlloc4->Add (Vector(99.23231787542632,9.138118508164705, 0.0));
    positionAlloc4->Add (Vector(275.52546947893023,361.1895763282187, 0.0));
    positionAlloc4->Add (Vector(74.76261327069787,187.22456760164704, 0.0));
    positionAlloc4->Add (Vector(251.27768856388525,69.41698839876804, 0.0));
    positionAlloc4->Add (Vector(257.69569513886324,308.58425582544, 0.0));
    positionAlloc4->Add (Vector(87.92986303261353,339.7156720998659, 0.0));
    positionAlloc4->Add (Vector(9.615585953422645,344.0133399164086, 0.0));
    positionAlloc4->Add (Vector(372.4963872194612,45.04338736880307, 0.0));
    positionAlloc4->Add (Vector(19.4440214788556,73.98567392478377, 0.0));
    positionAlloc4->Add (Vector(243.2697504831438,251.2881403455968, 0.0));
    positionAlloc4->Add (Vector(107.00712885830845,44.36993245335073, 0.0));
    positionAlloc4->Add (Vector(3.317486547846338,307.45772563465266, 0.0));
    positionAlloc4->Add (Vector(157.25682042532813,36.67047952844804, 0.0));
    positionAlloc4->Add (Vector(79.62925930855862,371.07169967797466, 0.0));
    positionAlloc4->Add (Vector(336.1209812177576,327.81649401044695, 0.0));
    positionAlloc4->Add (Vector(156.1377943960434,281.01821961728035, 0.0));
    positionAlloc4->Add (Vector(308.9105834614877,252.89644595592026, 0.0));
    positionAlloc4->Add (Vector(319.7535357850205,234.99928073510995, 0.0));
    positionAlloc4->Add (Vector(110.40493314280182,335.9901597320493, 0.0));
    positionAlloc4->Add (Vector(200.326633307589,332.0781180759207, 0.0));
    positionAlloc4->Add (Vector(35.390059649069315,328.40917872572527, 0.0));
    positionAlloc4->Add (Vector(281.3008437889651,358.59326106165287, 0.0));
    positionAlloc4->Add (Vector(258.2035286866802,58.20254720149567, 0.0));
    positionAlloc4->Add (Vector(195.8099759910583,124.98179766547115, 0.0));
    positionAlloc4->Add (Vector(301.6646315001218,27.750212149368547, 0.0));
    positionAlloc4->Add (Vector(197.86484288691236,391.54592451605856, 0.0));
    positionAlloc4->Add (Vector(109.34530735613039,100.95302666076944, 0.0));
    positionAlloc4->Add (Vector(355.1873711710784,254.92005093326705, 0.0));
    positionAlloc4->Add (Vector(77.00612924483812,383.1066032483761, 0.0));
    positionAlloc4->Add (Vector(205.41600589992171,136.42680251837197, 0.0));
    positionAlloc4->Add (Vector(62.01105285296875,329.9801329013921, 0.0));
    positionAlloc4->Add (Vector(294.69538829879525,293.50890650232355, 0.0));
    positionAlloc4->Add (Vector(116.52597649126494,169.33334276722297, 0.0));
    positionAlloc4->Add (Vector(158.9311326218831,264.8986179339594, 0.0));
    positionAlloc4->Add (Vector(75.45608929983199,244.45758874309877, 0.0));
    positionAlloc4->Add (Vector(361.8467435674527,334.6948749326655, 0.0));
    positionAlloc4->Add (Vector(374.374018726812,337.6145190210575, 0.0));
    positionAlloc4->Add (Vector(331.41302954171664,365.41095783257157, 0.0));
    positionAlloc4->Add (Vector(395.9667365024518,78.68637385394837, 0.0));
    positionAlloc4->Add (Vector(362.15045729005453,28.70185861068957, 0.0));
    positionAlloc4->Add (Vector(293.949418144623,112.76707420120786, 0.0));
    positionAlloc4->Add (Vector(121.90188970617287,263.3039602342275, 0.0));
    positionAlloc4->Add (Vector(254.11446613937105,55.81758990254353, 0.0));
    positionAlloc4->Add (Vector(148.31880362380926,374.90423763496005, 0.0));
    positionAlloc4->Add (Vector(323.5812825806536,285.6679234541847, 0.0));
    positionAlloc4->Add (Vector(145.04624188689897,118.33904593155218, 0.0));
    positionAlloc4->Add (Vector(220.6990401205805,348.7655202212971, 0.0));
    positionAlloc4->Add (Vector(338.4526264238082,291.7884241891998, 0.0));
    positionAlloc4->Add (Vector(22.27977431250001,55.92082688959259, 0.0));
    positionAlloc4->Add (Vector(135.43107396078312,107.20590500279496, 0.0));
    positionAlloc4->Add (Vector(387.10634592875164,0.9049376639326034, 0.0));
    positionAlloc4->Add (Vector(384.0883933894638,391.00778210250337, 0.0));
    positionAlloc4->Add (Vector(361.31133384325165,263.4105014411259, 0.0));
    positionAlloc4->Add (Vector(249.0235508183756,332.7902015903425, 0.0));
    positionAlloc4->Add (Vector(145.7718195382019,20.04178766213358, 0.0));
    positionAlloc4->Add (Vector(7.046944689550871,94.97620566091314, 0.0));
    positionAlloc4->Add (Vector(234.30279843000307,366.8122599167046, 0.0));
    positionAlloc4->Add (Vector(365.3375650343093,396.23355155902425, 0.0));
    positionAlloc4->Add (Vector(109.40927208879062,57.73109377699459, 0.0));
    positionAlloc4->Add (Vector(264.96750048101705,297.12771152829976, 0.0));
    positionAlloc4->Add (Vector(286.2134469771004,217.83274087195866, 0.0));
    positionAlloc4->Add (Vector(303.7922133460316,318.83467655238655, 0.0));
    positionAlloc4->Add (Vector(358.93916662390507,355.9715485408805, 0.0));
    positionAlloc4->Add (Vector(395.2305531606556,215.82653518853556, 0.0));
    positionAlloc4->Add (Vector(101.85117346915789,397.72826678005237, 0.0));
    positionAlloc4->Add (Vector(383.47528165956265,362.17312932774905, 0.0));
    positionAlloc4->Add (Vector(103.85761820741659,366.55015542265505, 0.0));
    positionAlloc4->Add (Vector(163.35944079437675,281.45118625158443, 0.0));
    positionAlloc4->Add (Vector(44.29410790297909,114.89438451692666, 0.0));
    positionAlloc4->Add (Vector(187.2155222508167,91.34541440572445, 0.0));
    positionAlloc4->Add (Vector(28.50050293357085,26.359848847677412, 0.0));
    positionAlloc4->Add (Vector(142.16424186974538,61.47488753781833, 0.0));
    positionAlloc4->Add (Vector(219.82081609368706,356.1898962862873, 0.0));
    positionAlloc4->Add (Vector(219.75673556243316,65.14297380005742, 0.0));
    positionAlloc4->Add (Vector(372.0608745686786,315.39477661542605, 0.0));
    positionAlloc4->Add (Vector(217.17723529850178,308.5618250191057, 0.0));
    positionAlloc4->Add (Vector(298.6277116293346,320.9083239424111, 0.0));
    positionAlloc4->Add (Vector(183.63898065853584,361.7969017475462, 0.0));
    positionAlloc4->Add (Vector(27.344834167467937,316.48110540381623, 0.0));
    positionAlloc4->Add (Vector(311.57456609379034,161.1779574783458, 0.0));
    positionAlloc4->Add (Vector(36.97244526193284,137.9731905989279, 0.0));
    positionAlloc4->Add (Vector(289.76446092780554,34.49253286859442, 0.0));
    positionAlloc4->Add (Vector(263.47273039196466,326.27249584661814, 0.0));
    positionAlloc4->Add (Vector(334.4120425023808,82.39969785692396, 0.0));
    positionAlloc4->Add (Vector(99.72421036493829,217.30278151974466, 0.0));
    positionAlloc4->Add (Vector(85.0669438750732,318.40381344794486, 0.0));
    positionAlloc4->Add (Vector(295.0233698531734,343.63894220723563, 0.0));
    positionAlloc4->Add (Vector(297.9067554377539,316.41749333568157, 0.0));
    positionAlloc4->Add (Vector(112.04253488646265,196.60031134160909, 0.0));
    positionAlloc4->Add (Vector(215.84982668600622,249.18610114791946, 0.0));
    positionAlloc4->Add (Vector(193.53051668233854,47.82869589825607, 0.0));
    positionAlloc4->Add (Vector(206.8543890614407,176.31978639862655, 0.0));
    positionAlloc4->Add (Vector(307.35518639798744,233.464963717915, 0.0));
    positionAlloc4->Add (Vector(366.5411725243679,168.7596260738565, 0.0));
    positionAlloc4->Add (Vector(73.31955065179075,248.28067284105987, 0.0));
    positionAlloc4->Add (Vector(370.2283323562754,126.82281860171915, 0.0));
    positionAlloc4->Add (Vector(60.033056622997584,205.85060846059062, 0.0));
    positionAlloc4->Add (Vector(129.61529463926595,210.82681953062146, 0.0));
    positionAlloc4->Add (Vector(180.7188747266812,45.217503890228805, 0.0));
    positionAlloc4->Add (Vector(36.26855877538415,373.2128732125853, 0.0));
    positionAlloc4->Add (Vector(203.9901535039538,217.55526714125915, 0.0));
    positionAlloc4->Add (Vector(62.7165747408128,151.42346343593385, 0.0));
    positionAlloc4->Add (Vector(317.0491896550635,245.8319976501229, 0.0));
    positionAlloc4->Add (Vector(198.8592902134726,25.191036734691963, 0.0));
    positionAlloc4->Add (Vector(75.25406183866718,230.5639706303539, 0.0));
    positionAlloc4->Add (Vector(393.4559822421198,56.548962398357894, 0.0));
    positionAlloc4->Add (Vector(361.8642701493527,356.8218738783284, 0.0));
    positionAlloc4->Add (Vector(54.35922348247888,272.09788936527366, 0.0));
    positionAlloc4->Add (Vector(128.29895159399962,153.40824705401363, 0.0));
    positionAlloc4->Add (Vector(65.55479242584256,228.76444157084973, 0.0));
    positionAlloc4->Add (Vector(135.44132840462012,83.56618103260925, 0.0));
    positionAlloc4->Add (Vector(146.8648482261792,27.116805235998065, 0.0));
    positionAlloc4->Add (Vector(27.50034385940556,89.30184290761285, 0.0));
    positionAlloc4->Add (Vector(377.4463807605523,92.54594073174052, 0.0));
    positionAlloc4->Add (Vector(299.32248386416904,155.9403531160973, 0.0));
    positionAlloc4->Add (Vector(324.56766836254315,151.16911364678964, 0.0));
    positionAlloc4->Add (Vector(118.91980097708723,283.56221738872006, 0.0));
    positionAlloc4->Add (Vector(337.87763001998303,220.20406863584535, 0.0));
    positionAlloc4->Add (Vector(71.06095415813401,193.29041176248194, 0.0));
    positionAlloc4->Add (Vector(300.99022470832887,204.6761190160515, 0.0));
    positionAlloc4->Add (Vector(70.15659787403959,321.55042083951577, 0.0));
    positionAlloc4->Add (Vector(314.45334064176393,49.22920988144304, 0.0));
    positionAlloc4->Add (Vector(158.43773992047744,158.68788903466168, 0.0));
    positionAlloc4->Add (Vector(342.5867295996863,49.23558647806412, 0.0));
    positionAlloc4->Add (Vector(243.3539087054609,54.87833717066937, 0.0));
    positionAlloc4->Add (Vector(271.5464175779989,91.40682064916983, 0.0));
    positionAlloc4->Add (Vector(124.37666503608047,351.4042846145945, 0.0));
    positionAlloc4->Add (Vector(367.70769038343195,205.54554553645792, 0.0));
    positionAlloc4->Add (Vector(154.630686642068,322.7802954202063, 0.0));
    positionAlloc4->Add (Vector(373.5931120598482,19.303854951409562, 0.0));
    positionAlloc4->Add (Vector(170.4226876531492,213.8638054763931, 0.0));
    positionAlloc4->Add (Vector(358.0585261286416,341.2592399571254, 0.0));
    positionAlloc4->Add (Vector(261.1729178394755,292.2523062725565, 0.0));
    positionAlloc4->Add (Vector(322.97264355204106,128.3348514982212, 0.0));
    positionAlloc4->Add (Vector(296.8506551871404,102.03832931553248, 0.0));
    positionAlloc4->Add (Vector(160.38236830764268,204.90600805397804, 0.0));
    positionAlloc4->Add (Vector(223.31458746763525,191.88270286433658, 0.0));
    positionAlloc4->Add (Vector(10.589276602392594,167.95793385886603, 0.0));
    positionAlloc4->Add (Vector(240.80261372319308,358.62867043928253, 0.0));
    positionAlloc4->Add (Vector(386.79966846190814,245.23650331629875, 0.0));
    positionAlloc4->Add (Vector(75.54772253154609,81.23009738022455, 0.0));
    positionAlloc4->Add (Vector(188.85552426248745,369.4864590569293, 0.0));
    positionAlloc4->Add (Vector(162.03525281943274,10.737347379403683, 0.0));
    positionAlloc4->Add (Vector(279.6622980649181,79.39238507589965, 0.0));
    positionAlloc4->Add (Vector(103.32342030368382,229.47346994130703, 0.0));
    positionAlloc4->Add (Vector(95.23219674521366,201.69429166619207, 0.0));
    positionAlloc4->Add (Vector(257.7523269682286,195.03579369803091, 0.0));
    positionAlloc4->Add (Vector(132.6175985813797,174.3584178831619, 0.0));
    positionAlloc4->Add (Vector(308.08300237077754,0.8662460215654644, 0.0));
    positionAlloc4->Add (Vector(205.44451667331,90.79420573216446, 0.0));
    positionAlloc4->Add (Vector(110.6840900418649,265.960890662726, 0.0));
    positionAlloc4->Add (Vector(183.73225383037126,287.99166534188043, 0.0));
    positionAlloc4->Add (Vector(316.9558077280854,280.39499993564255, 0.0));
    positionAlloc4->Add (Vector(219.72701651793335,146.26808645054888, 0.0));
    positionAlloc4->Add (Vector(133.504912455731,387.65857909710735, 0.0));
    positionAlloc4->Add (Vector(374.2986027864216,56.07042545625478, 0.0));
    positionAlloc4->Add (Vector(137.97748164883106,31.279959886375465, 0.0));
    positionAlloc4->Add (Vector(389.9280900438247,255.0143373250061, 0.0));
    positionAlloc4->Add (Vector(198.02511652471492,370.11977967774965, 0.0));
    positionAlloc4->Add (Vector(141.3039900618944,96.92012669542311, 0.0));
    positionAlloc4->Add (Vector(75.64562370593526,354.0200968446087, 0.0));
    positionAlloc4->Add (Vector(92.08848628819912,46.852705644663615, 0.0));
    positionAlloc4->Add (Vector(15.43601606155618,28.929850916937696, 0.0));
    positionAlloc4->Add (Vector(221.8682180983157,10.39510869714464, 0.0));
    positionAlloc4->Add (Vector(361.7509268965669,339.3434066382103, 0.0));
    positionAlloc4->Add (Vector(100.13625524079029,186.9695347436234, 0.0));
    positionAlloc4->Add (Vector(373.26604533296944,300.2067559127563, 0.0));
    positionAlloc4->Add (Vector(324.30203252435126,263.8510882064252, 0.0));
    positionAlloc4->Add (Vector(72.6427489619133,360.1029246095289, 0.0));
    positionAlloc4->Add (Vector(338.3010556440573,276.8279548205741, 0.0));
    positionAlloc4->Add (Vector(320.37527659227834,50.61970914435152, 0.0));
    positionAlloc4->Add (Vector(222.5959525120852,194.18796322215294, 0.0));
    positionAlloc4->Add (Vector(99.78697214854702,226.33502409725352, 0.0));
    positionAlloc4->Add (Vector(78.38027821066014,239.01300509966288, 0.0));
    positionAlloc4->Add (Vector(333.70266052483703,316.5296612098275, 0.0));
    positionAlloc4->Add (Vector(373.183862849765,8.442842988547916, 0.0));
    positionAlloc4->Add (Vector(167.7154307010204,334.5193332103061, 0.0));
    positionAlloc4->Add (Vector(149.11353604166982,178.6051519594031, 0.0));
    positionAlloc4->Add (Vector(289.2576306084049,87.15212622183483, 0.0));
    positionAlloc4->Add (Vector(39.32125213114408,282.53099707246594, 0.0));
    positionAlloc4->Add (Vector(202.58957542911853,63.5117456138159, 0.0));
    positionAlloc4->Add (Vector(344.0583686275143,232.97920803432558, 0.0));
    positionAlloc4->Add (Vector(309.77926864935824,103.25669364254226, 0.0));
    positionAlloc4->Add (Vector(367.92046877517623,208.54391547983408, 0.0));
    positionAlloc4->Add (Vector(274.8404082220746,378.3504885564188, 0.0));
    positionAlloc4->Add (Vector(90.63576436241574,184.12350584042355, 0.0));
    positionAlloc4->Add (Vector(344.1734197020424,11.377633322166858, 0.0));
    positionAlloc4->Add (Vector(19.772862362338863,208.356917708978, 0.0));
    positionAlloc4->Add (Vector(152.4176901790667,132.61320710295328, 0.0));
    positionAlloc4->Add (Vector(138.26921458417277,107.99279008333235, 0.0));
    positionAlloc4->Add (Vector(76.66263722995797,394.3608023796433, 0.0));
    positionAlloc4->Add (Vector(89.28113831200007,107.41321230558873, 0.0));
    positionAlloc4->Add (Vector(296.01946550805883,80.25485450618466, 0.0));
    positionAlloc4->Add (Vector(399.4731181958068,329.23232460985713, 0.0));
    positionAlloc4->Add (Vector(214.48517604440914,26.313607911128845, 0.0));
    positionAlloc4->Add (Vector(64.18449874292445,320.96775595015276, 0.0));
    positionAlloc4->Add (Vector(188.0042458486947,298.3115703037028, 0.0));
    positionAlloc4->Add (Vector(352.7293312112001,259.32805413264396, 0.0));
    positionAlloc4->Add (Vector(72.39815391712376,183.9043396298921, 0.0));
    positionAlloc4->Add (Vector(167.42301934969336,140.9986206559734, 0.0));
    positionAlloc4->Add (Vector(327.5713653288148,157.44545696590487, 0.0));
    positionAlloc4->Add (Vector(340.77832057177494,158.6841701123954, 0.0));
    positionAlloc4->Add (Vector(237.8787227974409,160.71852394714017, 0.0));
    positionAlloc4->Add (Vector(307.4353329580772,241.83256703912463, 0.0));
    positionAlloc4->Add (Vector(172.9663220278114,174.84099534990136, 0.0));
    positionAlloc4->Add (Vector(170.9600214859821,310.3393503177358, 0.0));
    positionAlloc4->Add (Vector(284.2320613535834,85.79734697943451, 0.0));
    positionAlloc4->Add (Vector(362.927976386007,386.646451450801, 0.0));
    positionAlloc4->Add (Vector(225.06658136914655,203.71640650906113, 0.0));
    positionAlloc4->Add (Vector(312.3824791784361,81.9434884373342, 0.0));
    positionAlloc4->Add (Vector(228.57291804608954,182.81037746886088, 0.0));
    positionAlloc4->Add (Vector(183.3179783002546,350.45323781237, 0.0));
    positionAlloc4->Add (Vector(67.31031034545461,336.97097532947515, 0.0));
    positionAlloc4->Add (Vector(340.965317886363,75.59148692609759, 0.0));
    positionAlloc4->Add (Vector(106.29054715025794,331.4033203984626, 0.0));
    positionAlloc4->Add (Vector(268.62255260748685,0.791452464765996, 0.0));
    positionAlloc4->Add (Vector(387.0021759961647,82.0826728077046, 0.0));
    positionAlloc4->Add (Vector(45.43603881091354,181.71058594832536, 0.0));
    positionAlloc4->Add (Vector(54.74691733462635,383.5435782953057, 0.0));
    positionAlloc4->Add (Vector(340.76004230019277,333.25186357153365, 0.0));
    positionAlloc4->Add (Vector(7.072161135110111,287.7136635613461, 0.0));
    positionAlloc4->Add (Vector(156.5085460929766,275.1388995976621, 0.0));
    positionAlloc4->Add (Vector(17.095875846583255,247.69752452268915, 0.0));
    positionAlloc4->Add (Vector(384.45859671765544,316.18889057731667, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_20.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_20.flowmon", false, false);
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