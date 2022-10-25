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
	  uint16_t numberOfNodes = 448;
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
    positionAlloc->Add (Vector (400.0,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (400.0,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,270.0, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (270.0,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (270.0,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,270.0, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,140.0, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,140.0, 0.0));
    positionAlloc->Add (Vector (140.0,270.0, 0.0));
    positionAlloc->Add (Vector (140.0,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (140.0,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,270.0, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,140.0, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,10.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,270.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,140.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,10.0, 0.0));
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
    positionAlloc4->Add (Vector(287.62455802260956,364.07148302901777, 0.0));
    positionAlloc4->Add (Vector(111.34599144897722,103.79692340313925, 0.0));
    positionAlloc4->Add (Vector(155.9421563324436,230.17082284474978, 0.0));
    positionAlloc4->Add (Vector(86.0838640653308,148.76920456170916, 0.0));
    positionAlloc4->Add (Vector(107.82680733878172,140.81736107806276, 0.0));
    positionAlloc4->Add (Vector(168.85062712633342,365.42787466461004, 0.0));
    positionAlloc4->Add (Vector(151.6490411101461,175.23257497114182, 0.0));
    positionAlloc4->Add (Vector(107.68941340068578,69.22268767287379, 0.0));
    positionAlloc4->Add (Vector(188.6079753394331,287.0675923108188, 0.0));
    positionAlloc4->Add (Vector(152.47544549861522,253.0850158475014, 0.0));
    positionAlloc4->Add (Vector(37.49467042985688,127.19241267148509, 0.0));
    positionAlloc4->Add (Vector(68.64427769876968,117.93400196834965, 0.0));
    positionAlloc4->Add (Vector(293.0004460433815,371.4987722911024, 0.0));
    positionAlloc4->Add (Vector(165.59116650774698,351.65687426569434, 0.0));
    positionAlloc4->Add (Vector(399.32670011378787,325.8022431769403, 0.0));
    positionAlloc4->Add (Vector(242.95061557954995,114.98914968093219, 0.0));
    positionAlloc4->Add (Vector(310.80765974416704,117.86593959954779, 0.0));
    positionAlloc4->Add (Vector(236.9149543063655,287.9182837495679, 0.0));
    positionAlloc4->Add (Vector(326.30823381973136,111.20653266751282, 0.0));
    positionAlloc4->Add (Vector(355.1920359760703,38.310411719925156, 0.0));
    positionAlloc4->Add (Vector(226.06939268076474,283.0239791328893, 0.0));
    positionAlloc4->Add (Vector(341.23420495595593,46.9510416403792, 0.0));
    positionAlloc4->Add (Vector(353.7536467103486,372.3473185018233, 0.0));
    positionAlloc4->Add (Vector(309.4911933609175,31.409165933641425, 0.0));
    positionAlloc4->Add (Vector(17.316064349562986,144.59503385669515, 0.0));
    positionAlloc4->Add (Vector(282.5052978205265,264.53845375262785, 0.0));
    positionAlloc4->Add (Vector(276.76689171071394,107.20303274292817, 0.0));
    positionAlloc4->Add (Vector(98.37693408645562,392.6486914492067, 0.0));
    positionAlloc4->Add (Vector(19.937023253808395,32.172573587818846, 0.0));
    positionAlloc4->Add (Vector(283.4842031780053,195.3336472481795, 0.0));
    positionAlloc4->Add (Vector(363.99584867297926,177.43681544813347, 0.0));
    positionAlloc4->Add (Vector(325.6543258895281,324.919246147379, 0.0));
    positionAlloc4->Add (Vector(63.744082491678626,178.12544104614608, 0.0));
    positionAlloc4->Add (Vector(39.415604256221215,54.238818956941245, 0.0));
    positionAlloc4->Add (Vector(108.72122099864852,0.8001202433726817, 0.0));
    positionAlloc4->Add (Vector(304.63692475160207,113.93334884732269, 0.0));
    positionAlloc4->Add (Vector(43.24295423540416,28.449991809135298, 0.0));
    positionAlloc4->Add (Vector(351.3280798723779,332.2810276962271, 0.0));
    positionAlloc4->Add (Vector(94.19910451996545,269.4266791904128, 0.0));
    positionAlloc4->Add (Vector(105.2988235600758,0.3146166118545679, 0.0));
    positionAlloc4->Add (Vector(276.5953694215879,212.80550003095266, 0.0));
    positionAlloc4->Add (Vector(248.8866545255735,82.59198367505265, 0.0));
    positionAlloc4->Add (Vector(246.11948858643294,337.7562730573435, 0.0));
    positionAlloc4->Add (Vector(310.43245494675904,228.50159081696697, 0.0));
    positionAlloc4->Add (Vector(334.52091095416165,352.67636774157694, 0.0));
    positionAlloc4->Add (Vector(333.69439579853963,217.76726440129502, 0.0));
    positionAlloc4->Add (Vector(42.60986319623723,374.6468092245948, 0.0));
    positionAlloc4->Add (Vector(386.21703183693546,209.19523000549148, 0.0));
    positionAlloc4->Add (Vector(335.98150868881316,306.34039691597576, 0.0));
    positionAlloc4->Add (Vector(225.4825021145823,95.43423837703351, 0.0));
    positionAlloc4->Add (Vector(39.3385515793681,144.21052199858434, 0.0));
    positionAlloc4->Add (Vector(178.3098105098901,300.36102386582667, 0.0));
    positionAlloc4->Add (Vector(325.4106317940001,150.51767346275926, 0.0));
    positionAlloc4->Add (Vector(44.72969590064313,212.41894183488884, 0.0));
    positionAlloc4->Add (Vector(321.2000158986004,398.4565749453079, 0.0));
    positionAlloc4->Add (Vector(387.2805752978798,114.84935101151814, 0.0));
    positionAlloc4->Add (Vector(149.8599917094083,325.53599695783817, 0.0));
    positionAlloc4->Add (Vector(391.8958483607497,199.71080438533525, 0.0));
    positionAlloc4->Add (Vector(49.02134596454526,210.64373428252296, 0.0));
    positionAlloc4->Add (Vector(1.2582881857489792,175.30121300839446, 0.0));
    positionAlloc4->Add (Vector(267.8281794160945,274.93738956673883, 0.0));
    positionAlloc4->Add (Vector(37.00134326024158,258.35335496705693, 0.0));
    positionAlloc4->Add (Vector(123.20378832835584,95.32356336272909, 0.0));
    positionAlloc4->Add (Vector(185.46948448508041,161.9393886090891, 0.0));
    positionAlloc4->Add (Vector(234.23431763960826,288.5537106588918, 0.0));
    positionAlloc4->Add (Vector(98.39198309577392,221.9569524303381, 0.0));
    positionAlloc4->Add (Vector(316.9251487082991,287.6661191013419, 0.0));
    positionAlloc4->Add (Vector(6.363984637025011,99.5592567930319, 0.0));
    positionAlloc4->Add (Vector(306.27444750830153,391.47752759001816, 0.0));
    positionAlloc4->Add (Vector(89.14555690142434,77.75800820188917, 0.0));
    positionAlloc4->Add (Vector(304.97647286383176,239.0479147959905, 0.0));
    positionAlloc4->Add (Vector(118.47068736123009,273.1011464657773, 0.0));
    positionAlloc4->Add (Vector(127.96602481754182,114.49911713047408, 0.0));
    positionAlloc4->Add (Vector(224.98646587040528,164.9570432881188, 0.0));
    positionAlloc4->Add (Vector(313.71680093985043,261.91967502416713, 0.0));
    positionAlloc4->Add (Vector(232.88601685572283,291.58826775044446, 0.0));
    positionAlloc4->Add (Vector(170.9968378242122,160.42732217379844, 0.0));
    positionAlloc4->Add (Vector(327.98776889803355,187.1272191261487, 0.0));
    positionAlloc4->Add (Vector(351.9742304887695,328.54881661731537, 0.0));
    positionAlloc4->Add (Vector(37.4456993855961,26.239406454017278, 0.0));
    positionAlloc4->Add (Vector(184.9341616540463,384.2626781147986, 0.0));
    positionAlloc4->Add (Vector(54.1417563448293,12.255479236347888, 0.0));
    positionAlloc4->Add (Vector(9.712570129871434,327.26989813246615, 0.0));
    positionAlloc4->Add (Vector(350.6743226386296,224.59459261307174, 0.0));
    positionAlloc4->Add (Vector(215.108211292521,2.693882797681324, 0.0));
    positionAlloc4->Add (Vector(202.77215493133065,139.4158461222791, 0.0));
    positionAlloc4->Add (Vector(326.36266913512895,238.80647511382884, 0.0));
    positionAlloc4->Add (Vector(399.10494622752816,210.23911528712773, 0.0));
    positionAlloc4->Add (Vector(103.93359029619509,133.51898272948665, 0.0));
    positionAlloc4->Add (Vector(366.66004483237305,224.43952234905464, 0.0));
    positionAlloc4->Add (Vector(325.8000683106418,368.9289923411586, 0.0));
    positionAlloc4->Add (Vector(85.10361442725683,260.5461447991472, 0.0));
    positionAlloc4->Add (Vector(250.11637434233873,240.38623815923592, 0.0));
    positionAlloc4->Add (Vector(362.98928109392,313.0323912178394, 0.0));
    positionAlloc4->Add (Vector(254.73439013862006,276.9670013514411, 0.0));
    positionAlloc4->Add (Vector(81.89403459690281,252.8769501167303, 0.0));
    positionAlloc4->Add (Vector(352.273447162806,177.24456013071338, 0.0));
    positionAlloc4->Add (Vector(226.18223863621677,359.81236635442843, 0.0));
    positionAlloc4->Add (Vector(131.98929152220776,257.67827701528654, 0.0));
    positionAlloc4->Add (Vector(62.70064400137252,301.80680304095455, 0.0));
    positionAlloc4->Add (Vector(202.6328750027708,78.38680581700861, 0.0));
    positionAlloc4->Add (Vector(223.31463137906073,157.96805364289358, 0.0));
    positionAlloc4->Add (Vector(230.18373644952322,95.46008117157197, 0.0));
    positionAlloc4->Add (Vector(125.59373121877857,192.6499722882967, 0.0));
    positionAlloc4->Add (Vector(166.25640220816865,8.976701743471915, 0.0));
    positionAlloc4->Add (Vector(239.8857462463972,3.807886486623202, 0.0));
    positionAlloc4->Add (Vector(252.63750605236424,385.465084085228, 0.0));
    positionAlloc4->Add (Vector(111.52670217153941,353.04080100122803, 0.0));
    positionAlloc4->Add (Vector(206.52641827181967,62.231159196829644, 0.0));
    positionAlloc4->Add (Vector(324.84622718325846,41.25871303338569, 0.0));
    positionAlloc4->Add (Vector(56.34319875974674,38.543420744497325, 0.0));
    positionAlloc4->Add (Vector(234.83925395321995,22.06083400752954, 0.0));
    positionAlloc4->Add (Vector(375.7927696833818,150.659085498532, 0.0));
    positionAlloc4->Add (Vector(75.28776624251515,69.29770860326916, 0.0));
    positionAlloc4->Add (Vector(128.1417160602449,220.69518824868305, 0.0));
    positionAlloc4->Add (Vector(107.86810655613932,141.9277394886595, 0.0));
    positionAlloc4->Add (Vector(186.61739654949625,351.69262789777247, 0.0));
    positionAlloc4->Add (Vector(132.4057736263179,263.8839215355198, 0.0));
    positionAlloc4->Add (Vector(146.7347768633531,38.8530726413141, 0.0));
    positionAlloc4->Add (Vector(88.21191789748949,146.87693036130622, 0.0));
    positionAlloc4->Add (Vector(198.19897389884537,349.07708919394884, 0.0));
    positionAlloc4->Add (Vector(140.8621440386127,364.5740561295169, 0.0));
    positionAlloc4->Add (Vector(40.58434312960162,314.5077289394374, 0.0));
    positionAlloc4->Add (Vector(383.4296844078683,290.86617397077174, 0.0));
    positionAlloc4->Add (Vector(122.10458862632527,28.54258997784647, 0.0));
    positionAlloc4->Add (Vector(121.13140769305626,35.507082431656926, 0.0));
    positionAlloc4->Add (Vector(94.22649460096815,247.27273673617822, 0.0));
    positionAlloc4->Add (Vector(246.1888565500411,325.54782512379404, 0.0));
    positionAlloc4->Add (Vector(210.3044749088277,260.11590768120215, 0.0));
    positionAlloc4->Add (Vector(137.67854357210726,28.139151874083133, 0.0));
    positionAlloc4->Add (Vector(255.39118216061897,48.88495129479331, 0.0));
    positionAlloc4->Add (Vector(44.78217773663245,204.50915745370474, 0.0));
    positionAlloc4->Add (Vector(61.605271335180454,151.1254189857767, 0.0));
    positionAlloc4->Add (Vector(76.83026920182292,90.50063543482621, 0.0));
    positionAlloc4->Add (Vector(224.16047243391276,239.10627415073571, 0.0));
    positionAlloc4->Add (Vector(394.5610302549668,338.4966922351127, 0.0));
    positionAlloc4->Add (Vector(285.71820555155136,118.12558644075968, 0.0));
    positionAlloc4->Add (Vector(318.8879229434953,5.4177387356510565, 0.0));
    positionAlloc4->Add (Vector(218.6045997456912,263.86558216996843, 0.0));
    positionAlloc4->Add (Vector(191.34873242867943,141.0969750004296, 0.0));
    positionAlloc4->Add (Vector(91.13035475623903,12.297127339878822, 0.0));
    positionAlloc4->Add (Vector(395.7058738943201,312.1827742297365, 0.0));
    positionAlloc4->Add (Vector(269.94823366580005,28.770785923685782, 0.0));
    positionAlloc4->Add (Vector(93.14163889759838,301.9231404401845, 0.0));
    positionAlloc4->Add (Vector(49.07241674360252,213.54475372136665, 0.0));
    positionAlloc4->Add (Vector(168.36923380595198,154.46020474823743, 0.0));
    positionAlloc4->Add (Vector(258.4121028286291,242.03965366220507, 0.0));
    positionAlloc4->Add (Vector(121.38903918461001,167.7329192667545, 0.0));
    positionAlloc4->Add (Vector(44.47078076792241,346.1456423129519, 0.0));
    positionAlloc4->Add (Vector(350.75347992346065,127.86004474482104, 0.0));
    positionAlloc4->Add (Vector(395.00756348177663,286.3327876290375, 0.0));
    positionAlloc4->Add (Vector(352.73339230071866,349.4677477427754, 0.0));
    positionAlloc4->Add (Vector(233.7077620910323,134.28604310224125, 0.0));
    positionAlloc4->Add (Vector(1.094322418160143,25.344204549614126, 0.0));
    positionAlloc4->Add (Vector(389.3010207068273,295.03540396667245, 0.0));
    positionAlloc4->Add (Vector(9.561511746487739,323.4256601897554, 0.0));
    positionAlloc4->Add (Vector(395.6302742031788,127.38284792547302, 0.0));
    positionAlloc4->Add (Vector(275.4220799032592,10.53915154799312, 0.0));
    positionAlloc4->Add (Vector(380.71031130283745,225.11221987576997, 0.0));
    positionAlloc4->Add (Vector(51.49264183861906,308.77085842921934, 0.0));
    positionAlloc4->Add (Vector(386.0751967172171,261.0861473702781, 0.0));
    positionAlloc4->Add (Vector(393.8319115842824,75.76720881750151, 0.0));
    positionAlloc4->Add (Vector(110.23728576412672,246.50428933194414, 0.0));
    positionAlloc4->Add (Vector(46.97677449103583,353.5644263745397, 0.0));
    positionAlloc4->Add (Vector(207.00507685704775,165.4392644516428, 0.0));
    positionAlloc4->Add (Vector(175.17676991084952,317.52180284657004, 0.0));
    positionAlloc4->Add (Vector(306.4899227671676,283.5057023725045, 0.0));
    positionAlloc4->Add (Vector(42.68563534221514,73.58477590106767, 0.0));
    positionAlloc4->Add (Vector(383.83848101236765,95.5256533185568, 0.0));
    positionAlloc4->Add (Vector(87.04212142364925,99.79282043944914, 0.0));
    positionAlloc4->Add (Vector(382.40783345237355,225.8440396274847, 0.0));
    positionAlloc4->Add (Vector(127.458745912153,92.50571795417501, 0.0));
    positionAlloc4->Add (Vector(331.2801034829795,229.62990207862038, 0.0));
    positionAlloc4->Add (Vector(154.94732279952382,368.3652015219009, 0.0));
    positionAlloc4->Add (Vector(118.87511767869103,6.649642613162365, 0.0));
    positionAlloc4->Add (Vector(70.53521630794629,167.5340572209971, 0.0));
    positionAlloc4->Add (Vector(234.33135377116844,152.0284821115165, 0.0));
    positionAlloc4->Add (Vector(325.9978638823939,114.15341297981239, 0.0));
    positionAlloc4->Add (Vector(43.8166868865074,391.60357502967605, 0.0));
    positionAlloc4->Add (Vector(266.03951133432827,89.94999198694629, 0.0));
    positionAlloc4->Add (Vector(307.1947917931223,197.9088535334109, 0.0));
    positionAlloc4->Add (Vector(390.34797502516545,185.7108270176973, 0.0));
    positionAlloc4->Add (Vector(196.99226156524415,351.12380059198006, 0.0));
    positionAlloc4->Add (Vector(223.1315955048236,284.9123998410009, 0.0));
    positionAlloc4->Add (Vector(4.588982009162423,66.61505744461604, 0.0));
    positionAlloc4->Add (Vector(102.88589938138215,326.581068130991, 0.0));
    positionAlloc4->Add (Vector(109.20353492476708,278.33938254377836, 0.0));
    positionAlloc4->Add (Vector(280.8277856915393,247.5721692389956, 0.0));
    positionAlloc4->Add (Vector(103.48868300264292,368.65476746245605, 0.0));
    positionAlloc4->Add (Vector(200.3936049575762,291.8493514272721, 0.0));
    positionAlloc4->Add (Vector(337.904545137454,166.64765942884733, 0.0));
    positionAlloc4->Add (Vector(340.58750675081535,110.7692225875971, 0.0));
    positionAlloc4->Add (Vector(149.44388757056242,241.4054466685276, 0.0));
    positionAlloc4->Add (Vector(205.66330710247507,199.2667299978049, 0.0));
    positionAlloc4->Add (Vector(42.167898575726866,280.71493860476636, 0.0));
    positionAlloc4->Add (Vector(335.57797918929947,25.12959208690777, 0.0));
    positionAlloc4->Add (Vector(1.9508177928906356,130.13253190758417, 0.0));
    positionAlloc4->Add (Vector(269.4012058742744,162.32816239961804, 0.0));
    positionAlloc4->Add (Vector(62.31093024620513,229.71334174035326, 0.0));
    positionAlloc4->Add (Vector(374.17080493656346,335.5379336792731, 0.0));
    positionAlloc4->Add (Vector(368.27023910364886,171.334085503889, 0.0));
    positionAlloc4->Add (Vector(397.29471378621827,102.30041078075986, 0.0));
    positionAlloc4->Add (Vector(145.47376314111392,112.97498929401799, 0.0));
    positionAlloc4->Add (Vector(336.87164754680396,169.8450840378411, 0.0));
    positionAlloc4->Add (Vector(121.79409762093232,344.99455988172036, 0.0));
    positionAlloc4->Add (Vector(375.4202617144101,388.30005922126463, 0.0));
    positionAlloc4->Add (Vector(353.52703599900474,72.0934100928888, 0.0));
    positionAlloc4->Add (Vector(291.6986457781581,126.26165271326433, 0.0));
    positionAlloc4->Add (Vector(196.62287391261577,81.75474651914594, 0.0));
    positionAlloc4->Add (Vector(1.0527184502012954,310.48231498415237, 0.0));
    positionAlloc4->Add (Vector(38.727364277284025,282.77977921545363, 0.0));
    positionAlloc4->Add (Vector(62.628969839350866,287.51002113946, 0.0));
    positionAlloc4->Add (Vector(273.9695987619227,296.312725207834, 0.0));
    positionAlloc4->Add (Vector(341.18793993987714,311.3304874720808, 0.0));
    positionAlloc4->Add (Vector(266.72319805550467,4.441333347969145, 0.0));
    positionAlloc4->Add (Vector(380.83643428513597,267.909922530896, 0.0));
    positionAlloc4->Add (Vector(258.43628859837565,108.83720571730974, 0.0));
    positionAlloc4->Add (Vector(384.3189224082579,222.63666364084074, 0.0));
    positionAlloc4->Add (Vector(76.543551014288,390.9822257572065, 0.0));
    positionAlloc4->Add (Vector(317.3938696668134,14.351659586406695, 0.0));
    positionAlloc4->Add (Vector(67.39078865857886,211.63488418210017, 0.0));
    positionAlloc4->Add (Vector(85.6587699122965,175.81564086215647, 0.0));
    positionAlloc4->Add (Vector(387.0435590050185,334.4704892541434, 0.0));
    positionAlloc4->Add (Vector(216.44835444358134,181.37412578789264, 0.0));
    positionAlloc4->Add (Vector(388.8082487161269,85.75916167032562, 0.0));
    positionAlloc4->Add (Vector(392.44342522545077,174.6251086724573, 0.0));
    positionAlloc4->Add (Vector(195.9231138840353,398.2308847385585, 0.0));
    positionAlloc4->Add (Vector(123.03543566992099,198.9793114338573, 0.0));
    positionAlloc4->Add (Vector(99.90390171468152,44.86019328843871, 0.0));
    positionAlloc4->Add (Vector(244.2530067470583,85.16583620606544, 0.0));
    positionAlloc4->Add (Vector(116.75808841098632,178.38112589957112, 0.0));
    positionAlloc4->Add (Vector(125.97160895704143,146.74372743647348, 0.0));
    positionAlloc4->Add (Vector(229.45754302816766,56.51173305640067, 0.0));
    positionAlloc4->Add (Vector(395.52875388691984,310.9917724839764, 0.0));
    positionAlloc4->Add (Vector(122.85684370834451,350.71081597518446, 0.0));
    positionAlloc4->Add (Vector(255.57912814737267,226.25258759069357, 0.0));
    positionAlloc4->Add (Vector(261.07716573380924,336.75920313069514, 0.0));
    positionAlloc4->Add (Vector(249.67188930556438,105.93466207047832, 0.0));
    positionAlloc4->Add (Vector(211.862928228017,114.79350109882982, 0.0));
    positionAlloc4->Add (Vector(219.02982394182908,301.86786978994587, 0.0));
    positionAlloc4->Add (Vector(33.118375435338976,122.20426451441244, 0.0));
    positionAlloc4->Add (Vector(278.06082589107615,83.08721602519506, 0.0));
    positionAlloc4->Add (Vector(127.87630954909179,264.335655582402, 0.0));
    positionAlloc4->Add (Vector(44.03060313880718,29.567555164508885, 0.0));
    positionAlloc4->Add (Vector(244.92893547384904,126.15847151205726, 0.0));
    positionAlloc4->Add (Vector(302.29714231849215,376.3389679604183, 0.0));
    positionAlloc4->Add (Vector(153.96365194159313,228.5284600234942, 0.0));
    positionAlloc4->Add (Vector(8.674161810440761,308.4609333015115, 0.0));
    positionAlloc4->Add (Vector(214.40960667536442,134.45017717493246, 0.0));
    positionAlloc4->Add (Vector(130.2161190395958,34.33972044811742, 0.0));
    positionAlloc4->Add (Vector(385.2966259654682,303.6216048865341, 0.0));
    positionAlloc4->Add (Vector(155.69550189638557,213.36139890640573, 0.0));
    positionAlloc4->Add (Vector(32.857600812772645,362.5340278321343, 0.0));
    positionAlloc4->Add (Vector(283.83568627124333,47.51108575939402, 0.0));
    positionAlloc4->Add (Vector(84.11802870104039,256.5699130214486, 0.0));
    positionAlloc4->Add (Vector(2.4841068762064378,172.62489811216065, 0.0));
    positionAlloc4->Add (Vector(76.01980144667593,118.42531576729303, 0.0));
    positionAlloc4->Add (Vector(311.12824436015376,371.9920545385426, 0.0));
    positionAlloc4->Add (Vector(86.41014050064283,113.57737513849871, 0.0));
    positionAlloc4->Add (Vector(172.84283651988258,276.1736079417255, 0.0));
    positionAlloc4->Add (Vector(317.9869698771693,273.9026819914306, 0.0));
    positionAlloc4->Add (Vector(221.5007250441181,340.7905032259526, 0.0));
    positionAlloc4->Add (Vector(156.4607042140113,120.11937174858525, 0.0));
    positionAlloc4->Add (Vector(274.28662889029465,380.8250156218133, 0.0));
    positionAlloc4->Add (Vector(90.41766835011767,0.4459321009626649, 0.0));
    positionAlloc4->Add (Vector(15.111110322458599,165.82515187695796, 0.0));
    positionAlloc4->Add (Vector(277.8983969346647,125.28867805875441, 0.0));
    positionAlloc4->Add (Vector(211.70237074708677,264.0567238494387, 0.0));
    positionAlloc4->Add (Vector(33.55814099975119,43.423482243487044, 0.0));
    positionAlloc4->Add (Vector(213.7599347618203,329.42062239870415, 0.0));
    positionAlloc4->Add (Vector(125.61018002035871,16.80990403444578, 0.0));
    positionAlloc4->Add (Vector(195.28357723528194,127.17435643004613, 0.0));
    positionAlloc4->Add (Vector(109.10729466591525,310.82819717695355, 0.0));
    positionAlloc4->Add (Vector(15.831164590147662,315.25608265747326, 0.0));
    positionAlloc4->Add (Vector(227.86142267362956,97.98034204593375, 0.0));
    positionAlloc4->Add (Vector(191.7085661134358,35.29206040003832, 0.0));
    positionAlloc4->Add (Vector(292.9082939025074,369.80405999364103, 0.0));
    positionAlloc4->Add (Vector(200.7172472252678,180.95049911114063, 0.0));
    positionAlloc4->Add (Vector(143.56445671554275,247.5880434960569, 0.0));
    positionAlloc4->Add (Vector(386.5645662609242,225.91449410665447, 0.0));
    positionAlloc4->Add (Vector(152.08264209819177,252.6977634005566, 0.0));
    positionAlloc4->Add (Vector(81.15956024913697,330.41261513816755, 0.0));
    positionAlloc4->Add (Vector(245.04642165982165,262.8660169454068, 0.0));
    positionAlloc4->Add (Vector(130.71673013235105,150.70275476805855, 0.0));
    positionAlloc4->Add (Vector(10.216033950316294,294.50290297861363, 0.0));
    positionAlloc4->Add (Vector(260.79032830829675,95.40640511960521, 0.0));
    positionAlloc4->Add (Vector(266.933513999279,237.57445735350782, 0.0));
    positionAlloc4->Add (Vector(199.58698979998402,330.9776365644303, 0.0));
    positionAlloc4->Add (Vector(371.2828876508704,89.75370641759382, 0.0));
    positionAlloc4->Add (Vector(319.5304356224329,324.3987677686107, 0.0));
    positionAlloc4->Add (Vector(347.511547822008,60.99869670448217, 0.0));
    positionAlloc4->Add (Vector(144.71830436286456,394.1267208341292, 0.0));
    positionAlloc4->Add (Vector(232.6749077475699,9.938344145492506, 0.0));
    positionAlloc4->Add (Vector(153.88642717015023,36.41634761839288, 0.0));
    positionAlloc4->Add (Vector(228.97232677082906,333.7518659162067, 0.0));
    positionAlloc4->Add (Vector(2.9359567014575205,229.25288187357674, 0.0));
    positionAlloc4->Add (Vector(20.116374411481488,385.1935012351416, 0.0));
    positionAlloc4->Add (Vector(357.37065494011176,331.2720876292499, 0.0));
    positionAlloc4->Add (Vector(233.95303950820824,185.0952721077301, 0.0));
    positionAlloc4->Add (Vector(302.46795535509415,317.502698889957, 0.0));
    positionAlloc4->Add (Vector(398.8555197467683,51.83832690805592, 0.0));
    positionAlloc4->Add (Vector(203.94958905137437,204.63195586589302, 0.0));
    positionAlloc4->Add (Vector(75.83668762910594,185.36846175618055, 0.0));
    positionAlloc4->Add (Vector(120.06873329441939,68.39493957720735, 0.0));
    positionAlloc4->Add (Vector(230.78414837304337,100.66278940005797, 0.0));
    positionAlloc4->Add (Vector(43.747062202741475,69.41705873700062, 0.0));
    positionAlloc4->Add (Vector(354.8484706629226,238.18646758781713, 0.0));
    positionAlloc4->Add (Vector(7.345054601028966,372.5621869059417, 0.0));
    positionAlloc4->Add (Vector(73.70282686337872,94.7234134776957, 0.0));
    positionAlloc4->Add (Vector(356.2896765974052,343.4860462907781, 0.0));
    positionAlloc4->Add (Vector(65.98890715958099,290.89417645731083, 0.0));
    positionAlloc4->Add (Vector(60.60562474084925,388.513324693032, 0.0));
    positionAlloc4->Add (Vector(230.18073378599294,123.11691133388423, 0.0));
    positionAlloc4->Add (Vector(114.31317334230488,3.8830386299266273, 0.0));
    positionAlloc4->Add (Vector(102.98331352306982,260.6736126962674, 0.0));
    positionAlloc4->Add (Vector(355.07325219188994,14.092508261343806, 0.0));
    positionAlloc4->Add (Vector(68.73190384273084,4.503957396875791, 0.0));
    positionAlloc4->Add (Vector(205.2049174640071,273.59968564678303, 0.0));
    positionAlloc4->Add (Vector(372.39594613838875,53.51132309723656, 0.0));
    positionAlloc4->Add (Vector(34.66898906116405,237.84155524465035, 0.0));
    positionAlloc4->Add (Vector(379.346116075609,65.17959943287379, 0.0));
    positionAlloc4->Add (Vector(35.58364438442139,130.68050317184063, 0.0));
    positionAlloc4->Add (Vector(293.37908002750635,290.48072391808034, 0.0));
    positionAlloc4->Add (Vector(104.41989256187463,13.232737363006253, 0.0));
    positionAlloc4->Add (Vector(95.14227489648795,303.27224936525096, 0.0));
    positionAlloc4->Add (Vector(76.13471788687565,352.9990139434437, 0.0));
    positionAlloc4->Add (Vector(96.3304838286216,93.16618735353268, 0.0));
    positionAlloc4->Add (Vector(377.1231940301139,321.09302006302045, 0.0));
    positionAlloc4->Add (Vector(131.24412597254724,298.11674914320616, 0.0));
    positionAlloc4->Add (Vector(271.82235781326017,14.029447722031563, 0.0));
    positionAlloc4->Add (Vector(155.08205271256395,119.0195729508949, 0.0));
    positionAlloc4->Add (Vector(104.15927900631421,123.5491009265024, 0.0));
    positionAlloc4->Add (Vector(90.91095256606354,89.68113570237452, 0.0));
    positionAlloc4->Add (Vector(312.93522534490404,217.95348267047783, 0.0));
    positionAlloc4->Add (Vector(127.4701425201309,48.253118142958584, 0.0));
    positionAlloc4->Add (Vector(181.43001726833518,39.136831338227964, 0.0));
    positionAlloc4->Add (Vector(305.8123761641894,204.085533522431, 0.0));
    positionAlloc4->Add (Vector(55.14368010564286,366.9584747433698, 0.0));
    positionAlloc4->Add (Vector(79.85822826066489,166.035955048545, 0.0));
    positionAlloc4->Add (Vector(343.7493313203428,231.32895498062047, 0.0));
    positionAlloc4->Add (Vector(110.07721224421067,295.36474824184273, 0.0));
    positionAlloc4->Add (Vector(34.67150645604256,77.20001502970119, 0.0));
    positionAlloc4->Add (Vector(374.52338397827725,361.35148004434114, 0.0));
    positionAlloc4->Add (Vector(269.84274872672967,70.51045119337265, 0.0));
    positionAlloc4->Add (Vector(278.15901438862096,346.79533184309753, 0.0));
    positionAlloc4->Add (Vector(16.007114269057475,105.8726526867416, 0.0));
    positionAlloc4->Add (Vector(270.1790062712079,318.20108411946603, 0.0));
    positionAlloc4->Add (Vector(314.0661051809227,305.890478738167, 0.0));
    positionAlloc4->Add (Vector(282.3239204644195,391.65099638328684, 0.0));
    positionAlloc4->Add (Vector(41.261887899622664,9.80899482185289, 0.0));
    positionAlloc4->Add (Vector(349.29816792638746,213.50443654911325, 0.0));
    positionAlloc4->Add (Vector(73.89926093971346,172.38946149034976, 0.0));
    positionAlloc4->Add (Vector(140.10420228288112,300.29131694209, 0.0));
    positionAlloc4->Add (Vector(30.732913909872337,98.12045764799709, 0.0));
    positionAlloc4->Add (Vector(371.70927285615875,160.83126952022462, 0.0));
    positionAlloc4->Add (Vector(356.12201088969647,161.80575199214312, 0.0));
    positionAlloc4->Add (Vector(201.3227134398649,21.113542447428202, 0.0));
    positionAlloc4->Add (Vector(14.144767796525581,326.9406917136053, 0.0));
    positionAlloc4->Add (Vector(397.19254305499135,129.66452127249397, 0.0));
    positionAlloc4->Add (Vector(88.14676046147794,194.13498158735223, 0.0));
    positionAlloc4->Add (Vector(218.17563573672177,272.99123171917773, 0.0));
    positionAlloc4->Add (Vector(197.8587392427636,126.01083558742702, 0.0));
    positionAlloc4->Add (Vector(100.7961848442954,135.79495771871697, 0.0));
    positionAlloc4->Add (Vector(241.27182169301017,350.91990568860456, 0.0));
    positionAlloc4->Add (Vector(286.39457891161874,298.95309386169737, 0.0));
    positionAlloc4->Add (Vector(71.56533454633865,91.16776472510209, 0.0));
    positionAlloc4->Add (Vector(376.33968374985756,391.8391485671365, 0.0));
    positionAlloc4->Add (Vector(326.91081044510025,287.022881236382, 0.0));
    positionAlloc4->Add (Vector(158.50374691307755,179.17839140590846, 0.0));
    positionAlloc4->Add (Vector(294.658881901955,323.73518760227205, 0.0));
    positionAlloc4->Add (Vector(191.64022969029762,256.92905545938856, 0.0));
    positionAlloc4->Add (Vector(140.42620913928468,374.86963548871245, 0.0));
    positionAlloc4->Add (Vector(290.46903532305095,381.7362818496845, 0.0));
    positionAlloc4->Add (Vector(149.54742882613687,349.7951186484708, 0.0));
    positionAlloc4->Add (Vector(188.94937688609593,239.7023495515054, 0.0));
    positionAlloc4->Add (Vector(272.88825618958026,124.26507842554236, 0.0));
    positionAlloc4->Add (Vector(8.520452584552629,384.2017891020399, 0.0));
    positionAlloc4->Add (Vector(367.6903564081094,100.58338089277363, 0.0));
    positionAlloc4->Add (Vector(73.5582396327533,281.6122782145337, 0.0));
    positionAlloc4->Add (Vector(350.2121260152491,32.494624972561994, 0.0));
    positionAlloc4->Add (Vector(109.95473193010379,72.23042106674278, 0.0));
    positionAlloc4->Add (Vector(85.99059231667106,13.146102352766631, 0.0));
    positionAlloc4->Add (Vector(51.7458828909422,153.52876474416948, 0.0));
    positionAlloc4->Add (Vector(212.45787775732353,131.03755950638876, 0.0));
    positionAlloc4->Add (Vector(316.03820845836,205.9101713492904, 0.0));
    positionAlloc4->Add (Vector(377.2497322913018,246.95292520998606, 0.0));
    positionAlloc4->Add (Vector(361.309080348134,153.2703401724373, 0.0));
    positionAlloc4->Add (Vector(340.78185792171934,76.08883572487342, 0.0));
    positionAlloc4->Add (Vector(158.18548652300456,265.57843257312027, 0.0));
    positionAlloc4->Add (Vector(123.21479482181252,234.78488289187166, 0.0));
    positionAlloc4->Add (Vector(251.53662743887645,47.58615864880178, 0.0));
    positionAlloc4->Add (Vector(296.33075127917164,225.0691546372913, 0.0));
    positionAlloc4->Add (Vector(72.59495902309214,255.62114371791012, 0.0));
    positionAlloc4->Add (Vector(77.7159928344747,351.45201063308815, 0.0));
    positionAlloc4->Add (Vector(264.96367227649915,195.23217820852966, 0.0));
    positionAlloc4->Add (Vector(316.01046024107416,48.88556403754247, 0.0));
    positionAlloc4->Add (Vector(47.8733108769946,196.03579843505332, 0.0));
    positionAlloc4->Add (Vector(106.81612397131808,399.950315029705, 0.0));
    positionAlloc4->Add (Vector(331.1212957836889,4.590205042325346, 0.0));
    positionAlloc4->Add (Vector(336.36313258141297,337.0670890380131, 0.0));
    positionAlloc4->Add (Vector(75.58503729832458,187.5355798568724, 0.0));
    positionAlloc4->Add (Vector(378.25371399455656,153.56569679141518, 0.0));
    positionAlloc4->Add (Vector(64.12929279535481,194.69557430310132, 0.0));
    positionAlloc4->Add (Vector(128.9466700537678,244.0820234985481, 0.0));
    positionAlloc4->Add (Vector(247.97827754465197,140.52732814327703, 0.0));
    positionAlloc4->Add (Vector(265.6603262576301,121.23227037722648, 0.0));
    positionAlloc4->Add (Vector(261.7877404909737,370.1044751435065, 0.0));
    positionAlloc4->Add (Vector(345.76650467933916,167.48347002286636, 0.0));
    positionAlloc4->Add (Vector(394.8732328265498,224.95778238287127, 0.0));
    positionAlloc4->Add (Vector(246.0886766033818,280.4832367674529, 0.0));
    positionAlloc4->Add (Vector(314.1475799475463,247.99688132809558, 0.0));
    positionAlloc4->Add (Vector(237.6577311566158,50.112778474459496, 0.0));
    positionAlloc4->Add (Vector(71.74000665465408,165.42415191031674, 0.0));
    positionAlloc4->Add (Vector(292.9443103478231,293.78777413997375, 0.0));
    positionAlloc4->Add (Vector(59.66380356807552,52.74867629137643, 0.0));
    positionAlloc4->Add (Vector(237.5015433421774,116.34397512418823, 0.0));
    positionAlloc4->Add (Vector(377.0393742432687,198.9159168466654, 0.0));
    positionAlloc4->Add (Vector(208.7124836936234,315.39165334940606, 0.0));
    positionAlloc4->Add (Vector(113.58035451561439,34.380891030967135, 0.0));
    positionAlloc4->Add (Vector(110.91801613728403,139.50519341537438, 0.0));
    positionAlloc4->Add (Vector(204.79969226556918,0.7854581530080118, 0.0));
    positionAlloc4->Add (Vector(210.86407028890454,115.23183523430194, 0.0));
    positionAlloc4->Add (Vector(338.84516643287776,201.35104385462122, 0.0));
    positionAlloc4->Add (Vector(192.59432487466088,76.81250641245771, 0.0));
    positionAlloc4->Add (Vector(295.12295104159364,109.05291242900321, 0.0));
    positionAlloc4->Add (Vector(1.706007362842854,18.772337290493056, 0.0));
    positionAlloc4->Add (Vector(335.36116255159163,127.9666801671942, 0.0));
    positionAlloc4->Add (Vector(191.65571150516544,325.6295013502661, 0.0));
    positionAlloc4->Add (Vector(291.52597277806154,140.9557769932147, 0.0));
    positionAlloc4->Add (Vector(184.83367719909688,302.36043901512204, 0.0));
    positionAlloc4->Add (Vector(6.420883666824828,61.082372472470595, 0.0));
    positionAlloc4->Add (Vector(103.27546560299359,14.896102244313925, 0.0));
    positionAlloc4->Add (Vector(82.96262592772959,298.912128423689, 0.0));
    positionAlloc4->Add (Vector(127.17790153893355,380.3171336310882, 0.0));
    positionAlloc4->Add (Vector(38.42519950397567,47.49216204503743, 0.0));
    positionAlloc4->Add (Vector(160.8375624372774,110.88803319147304, 0.0));
    positionAlloc4->Add (Vector(42.297686052020865,150.42283021892624, 0.0));
    positionAlloc4->Add (Vector(347.2954285763556,6.007768701369631, 0.0));
    positionAlloc4->Add (Vector(230.02806679548559,85.9130143539435, 0.0));
    positionAlloc4->Add (Vector(344.4045474455902,51.80037837715834, 0.0));
    positionAlloc4->Add (Vector(371.2912073000404,315.97911855321615, 0.0));
    positionAlloc4->Add (Vector(205.32013978716265,139.30267270029879, 0.0));
    positionAlloc4->Add (Vector(156.17585469504655,115.60443327510788, 0.0));
    positionAlloc4->Add (Vector(109.56723729182198,42.042827632819794, 0.0));
    positionAlloc4->Add (Vector(388.2968038085578,36.70153337647659, 0.0));
    positionAlloc4->Add (Vector(168.59640417123924,222.04177200337537, 0.0));
    positionAlloc4->Add (Vector(74.96990829014058,21.905650426609302, 0.0));
    positionAlloc4->Add (Vector(75.38151476450463,345.244504066534, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_3.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_3.flowmon", false, false);
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