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
	  uint16_t numberOfRrhs = 24;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 455;
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
    positionAlloc->Add (Vector (400.0,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (270.0,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (270.0,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (270.0,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (270.0,10.0, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,400.0, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,140.0, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,140.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,53.333333333333336, 0.0));
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
    positionAlloc4->Add (Vector(59.1791837698477,308.9732357448706, 0.0));
    positionAlloc4->Add (Vector(20.850117285925187,218.055118101667, 0.0));
    positionAlloc4->Add (Vector(233.95553753507065,70.02019467637899, 0.0));
    positionAlloc4->Add (Vector(120.54134913248174,336.75770275917154, 0.0));
    positionAlloc4->Add (Vector(380.73189200561876,103.05768322148792, 0.0));
    positionAlloc4->Add (Vector(232.28688460167209,393.38872644088696, 0.0));
    positionAlloc4->Add (Vector(304.94163718753356,379.56678544555996, 0.0));
    positionAlloc4->Add (Vector(137.4318583688937,124.53427652166242, 0.0));
    positionAlloc4->Add (Vector(51.870813616470855,64.40992876071721, 0.0));
    positionAlloc4->Add (Vector(37.267870308649975,118.69918617996285, 0.0));
    positionAlloc4->Add (Vector(355.7651549264915,60.75630478183043, 0.0));
    positionAlloc4->Add (Vector(171.8301291159553,271.7222669941944, 0.0));
    positionAlloc4->Add (Vector(224.56464039207708,392.22648533896785, 0.0));
    positionAlloc4->Add (Vector(102.81704269246634,200.9332422555976, 0.0));
    positionAlloc4->Add (Vector(121.61524872849046,51.315135990792, 0.0));
    positionAlloc4->Add (Vector(37.40509883923671,5.822127835097568, 0.0));
    positionAlloc4->Add (Vector(346.12230095157656,361.8410724944178, 0.0));
    positionAlloc4->Add (Vector(44.846431065759475,87.04983924345555, 0.0));
    positionAlloc4->Add (Vector(69.42434007779342,295.21736613561427, 0.0));
    positionAlloc4->Add (Vector(236.65246681655375,100.21596113981168, 0.0));
    positionAlloc4->Add (Vector(170.4699820618021,313.17954620443885, 0.0));
    positionAlloc4->Add (Vector(57.7735126746493,335.5336758937009, 0.0));
    positionAlloc4->Add (Vector(270.90940801880043,208.1859722164907, 0.0));
    positionAlloc4->Add (Vector(140.09682167551892,321.0831294161207, 0.0));
    positionAlloc4->Add (Vector(256.6194746495785,390.57829773025867, 0.0));
    positionAlloc4->Add (Vector(188.18276246230639,69.74970177204396, 0.0));
    positionAlloc4->Add (Vector(235.15837165485559,338.970620975322, 0.0));
    positionAlloc4->Add (Vector(275.02152238195464,323.0297452202006, 0.0));
    positionAlloc4->Add (Vector(357.433273425361,239.31392713413436, 0.0));
    positionAlloc4->Add (Vector(185.89569319843676,80.08991556005967, 0.0));
    positionAlloc4->Add (Vector(310.470305210188,3.704317039884941, 0.0));
    positionAlloc4->Add (Vector(40.14817713892738,167.25757803529797, 0.0));
    positionAlloc4->Add (Vector(253.91253751042518,24.972473193064857, 0.0));
    positionAlloc4->Add (Vector(357.94883878628565,275.4311154957159, 0.0));
    positionAlloc4->Add (Vector(75.40505200110626,53.673887560838374, 0.0));
    positionAlloc4->Add (Vector(23.72589668841849,265.9922670830456, 0.0));
    positionAlloc4->Add (Vector(184.05757249505137,282.48084457296494, 0.0));
    positionAlloc4->Add (Vector(245.35049288418912,234.66816175503195, 0.0));
    positionAlloc4->Add (Vector(77.37998252772704,6.5798088642138985, 0.0));
    positionAlloc4->Add (Vector(290.1044545419559,81.24732660802931, 0.0));
    positionAlloc4->Add (Vector(41.86491921229059,187.96280036396823, 0.0));
    positionAlloc4->Add (Vector(395.2170327849277,91.04367434946505, 0.0));
    positionAlloc4->Add (Vector(283.9091612749,224.97626187330005, 0.0));
    positionAlloc4->Add (Vector(114.18344224547158,33.07153690184976, 0.0));
    positionAlloc4->Add (Vector(34.451229919753025,160.18545150719547, 0.0));
    positionAlloc4->Add (Vector(178.22775976445118,387.6614499481887, 0.0));
    positionAlloc4->Add (Vector(351.13844924952804,66.11990929511435, 0.0));
    positionAlloc4->Add (Vector(261.71306734070737,115.35105083212014, 0.0));
    positionAlloc4->Add (Vector(273.08169289445095,23.37166337960501, 0.0));
    positionAlloc4->Add (Vector(287.42227908820814,177.2778128381973, 0.0));
    positionAlloc4->Add (Vector(108.08786055632909,40.93746427227831, 0.0));
    positionAlloc4->Add (Vector(283.8313642248152,294.8321709893829, 0.0));
    positionAlloc4->Add (Vector(104.51463398649032,252.9166864696842, 0.0));
    positionAlloc4->Add (Vector(296.89529548319115,369.05518480304613, 0.0));
    positionAlloc4->Add (Vector(158.68689343728724,86.98002432318512, 0.0));
    positionAlloc4->Add (Vector(198.3593703156133,239.80834755085843, 0.0));
    positionAlloc4->Add (Vector(334.41300578488546,280.4831584512029, 0.0));
    positionAlloc4->Add (Vector(214.3238729759813,167.9231741167288, 0.0));
    positionAlloc4->Add (Vector(133.75538143427886,107.43850264711425, 0.0));
    positionAlloc4->Add (Vector(188.06223328397263,51.40262295204141, 0.0));
    positionAlloc4->Add (Vector(252.93870637381036,198.71995051524442, 0.0));
    positionAlloc4->Add (Vector(37.9247864520333,263.51109984067796, 0.0));
    positionAlloc4->Add (Vector(104.82290888026698,184.57650857653704, 0.0));
    positionAlloc4->Add (Vector(368.63685595202946,175.28174754180105, 0.0));
    positionAlloc4->Add (Vector(174.66587390407452,331.0470438756808, 0.0));
    positionAlloc4->Add (Vector(104.34162382813584,324.11271507683415, 0.0));
    positionAlloc4->Add (Vector(231.5861523875697,340.34132743001135, 0.0));
    positionAlloc4->Add (Vector(38.07806589941451,135.3481645669055, 0.0));
    positionAlloc4->Add (Vector(20.17377014795576,306.3103556870507, 0.0));
    positionAlloc4->Add (Vector(206.13746505238194,178.74153595689845, 0.0));
    positionAlloc4->Add (Vector(253.43155596577708,40.47847113940972, 0.0));
    positionAlloc4->Add (Vector(62.019162832192706,104.75168816636891, 0.0));
    positionAlloc4->Add (Vector(235.86041628733753,352.8859008640133, 0.0));
    positionAlloc4->Add (Vector(36.32089675990815,99.52690577776404, 0.0));
    positionAlloc4->Add (Vector(87.48818348388964,88.66308377937156, 0.0));
    positionAlloc4->Add (Vector(203.3453965934842,168.4620495358589, 0.0));
    positionAlloc4->Add (Vector(94.06431778318134,301.75325817093875, 0.0));
    positionAlloc4->Add (Vector(348.1871395832646,333.61507432965936, 0.0));
    positionAlloc4->Add (Vector(95.12113340694212,38.81238663766431, 0.0));
    positionAlloc4->Add (Vector(267.2441052046378,27.8941492727649, 0.0));
    positionAlloc4->Add (Vector(174.50019687351696,3.7906913346691606, 0.0));
    positionAlloc4->Add (Vector(280.78061024464364,333.38983059808885, 0.0));
    positionAlloc4->Add (Vector(303.19977302055935,77.1680999460814, 0.0));
    positionAlloc4->Add (Vector(95.8644154671664,382.80206372284675, 0.0));
    positionAlloc4->Add (Vector(304.7811020584018,73.19935386620622, 0.0));
    positionAlloc4->Add (Vector(258.5057546040136,387.0064204886896, 0.0));
    positionAlloc4->Add (Vector(54.18883741771343,203.35482082860202, 0.0));
    positionAlloc4->Add (Vector(231.07028370673802,136.95204194700287, 0.0));
    positionAlloc4->Add (Vector(366.03368179274815,252.60775006336385, 0.0));
    positionAlloc4->Add (Vector(77.52611109271098,243.37273742503825, 0.0));
    positionAlloc4->Add (Vector(160.94303234024693,29.53484858684412, 0.0));
    positionAlloc4->Add (Vector(215.3347895310775,180.41466534457186, 0.0));
    positionAlloc4->Add (Vector(150.77213021146778,25.765822235997724, 0.0));
    positionAlloc4->Add (Vector(290.53176830155314,182.41956210728594, 0.0));
    positionAlloc4->Add (Vector(180.42566116464,251.97920923754228, 0.0));
    positionAlloc4->Add (Vector(82.96979235474558,64.31556519032107, 0.0));
    positionAlloc4->Add (Vector(32.90955691601627,20.517304069710196, 0.0));
    positionAlloc4->Add (Vector(188.56467107851591,218.90838513428662, 0.0));
    positionAlloc4->Add (Vector(77.23572716565825,398.21833750683993, 0.0));
    positionAlloc4->Add (Vector(46.78885900739811,329.881456291471, 0.0));
    positionAlloc4->Add (Vector(80.01643788837485,344.8888852578864, 0.0));
    positionAlloc4->Add (Vector(220.3283500603113,46.37996371206472, 0.0));
    positionAlloc4->Add (Vector(1.3060982102434338,207.43459580924656, 0.0));
    positionAlloc4->Add (Vector(43.114069849342854,347.55932793702317, 0.0));
    positionAlloc4->Add (Vector(117.52138245935497,59.97218309692292, 0.0));
    positionAlloc4->Add (Vector(3.9795468383908794,258.21688055937517, 0.0));
    positionAlloc4->Add (Vector(341.8449911635332,159.43560636799452, 0.0));
    positionAlloc4->Add (Vector(33.372321902048085,167.05172442020162, 0.0));
    positionAlloc4->Add (Vector(395.8128705949399,23.029029169664827, 0.0));
    positionAlloc4->Add (Vector(76.04873131817284,212.73052463672917, 0.0));
    positionAlloc4->Add (Vector(347.920845809795,351.67907973180354, 0.0));
    positionAlloc4->Add (Vector(274.998925413896,136.93267930486414, 0.0));
    positionAlloc4->Add (Vector(82.47289342179727,219.27687852766903, 0.0));
    positionAlloc4->Add (Vector(14.7790589108594,64.62971569090637, 0.0));
    positionAlloc4->Add (Vector(285.2048611361527,356.1700607487101, 0.0));
    positionAlloc4->Add (Vector(288.6535345230123,216.3050188411517, 0.0));
    positionAlloc4->Add (Vector(19.197613860281315,119.56331980462637, 0.0));
    positionAlloc4->Add (Vector(321.960771342171,194.18630012473096, 0.0));
    positionAlloc4->Add (Vector(202.29029768085854,187.35787297861094, 0.0));
    positionAlloc4->Add (Vector(334.8236894728167,97.10851352274608, 0.0));
    positionAlloc4->Add (Vector(93.92007518856929,238.63337909718373, 0.0));
    positionAlloc4->Add (Vector(193.13135031179104,196.38693669990573, 0.0));
    positionAlloc4->Add (Vector(352.63573805422226,79.43641034686517, 0.0));
    positionAlloc4->Add (Vector(374.5270984244431,59.23359526884413, 0.0));
    positionAlloc4->Add (Vector(26.846669241718946,185.91345519005978, 0.0));
    positionAlloc4->Add (Vector(169.47568936407814,241.99286867685439, 0.0));
    positionAlloc4->Add (Vector(144.5760269476035,178.62440739712463, 0.0));
    positionAlloc4->Add (Vector(97.92161310429024,251.66820998665435, 0.0));
    positionAlloc4->Add (Vector(222.53066251255217,129.2075108032905, 0.0));
    positionAlloc4->Add (Vector(105.03728344278795,236.44357306310013, 0.0));
    positionAlloc4->Add (Vector(307.5139698661544,169.06320725833456, 0.0));
    positionAlloc4->Add (Vector(318.01725688608667,343.96923641610755, 0.0));
    positionAlloc4->Add (Vector(97.08301864992599,166.18199017366945, 0.0));
    positionAlloc4->Add (Vector(392.1098112102311,76.76654480569876, 0.0));
    positionAlloc4->Add (Vector(79.6164713186836,196.93606166628385, 0.0));
    positionAlloc4->Add (Vector(312.3861970510608,241.8639055753055, 0.0));
    positionAlloc4->Add (Vector(89.07485370213931,192.87238343772137, 0.0));
    positionAlloc4->Add (Vector(159.58146852429525,315.5373237469026, 0.0));
    positionAlloc4->Add (Vector(347.3420358341052,82.52877621595461, 0.0));
    positionAlloc4->Add (Vector(277.6661503084299,289.84237976341234, 0.0));
    positionAlloc4->Add (Vector(194.8358380721419,118.5256357157514, 0.0));
    positionAlloc4->Add (Vector(391.60124891102765,380.25730947075124, 0.0));
    positionAlloc4->Add (Vector(319.6294860550074,343.1052726519138, 0.0));
    positionAlloc4->Add (Vector(146.89145569530652,24.584725770831284, 0.0));
    positionAlloc4->Add (Vector(305.8787176579005,86.2909415399098, 0.0));
    positionAlloc4->Add (Vector(129.74956952999798,83.55112686662932, 0.0));
    positionAlloc4->Add (Vector(2.278231372148065,160.74963813247035, 0.0));
    positionAlloc4->Add (Vector(122.74984584890474,222.49400605154938, 0.0));
    positionAlloc4->Add (Vector(50.30200563667218,169.40847807932195, 0.0));
    positionAlloc4->Add (Vector(284.0758389815238,100.86325957499555, 0.0));
    positionAlloc4->Add (Vector(373.3755275854953,70.33757379669652, 0.0));
    positionAlloc4->Add (Vector(189.8858799697619,78.21642610528214, 0.0));
    positionAlloc4->Add (Vector(97.68141882437624,337.1134905285117, 0.0));
    positionAlloc4->Add (Vector(151.5972744474301,13.264484321936143, 0.0));
    positionAlloc4->Add (Vector(162.26291830318843,17.444166755262014, 0.0));
    positionAlloc4->Add (Vector(287.69090612492545,261.0290621325834, 0.0));
    positionAlloc4->Add (Vector(77.85545369643789,268.9796144765389, 0.0));
    positionAlloc4->Add (Vector(352.2399308500295,325.4124381835598, 0.0));
    positionAlloc4->Add (Vector(336.3123642473531,190.58953421400076, 0.0));
    positionAlloc4->Add (Vector(221.9642812796005,364.8871805860199, 0.0));
    positionAlloc4->Add (Vector(49.65427066838575,183.41776385934963, 0.0));
    positionAlloc4->Add (Vector(36.77107868543339,264.88157699143704, 0.0));
    positionAlloc4->Add (Vector(392.1917131934624,100.37600849972162, 0.0));
    positionAlloc4->Add (Vector(295.291433150585,76.12492219633404, 0.0));
    positionAlloc4->Add (Vector(94.93957138471556,171.61929364783796, 0.0));
    positionAlloc4->Add (Vector(85.98994297334492,172.28616153790028, 0.0));
    positionAlloc4->Add (Vector(107.76670265248302,119.32207278918222, 0.0));
    positionAlloc4->Add (Vector(86.15453107750422,153.5438882082191, 0.0));
    positionAlloc4->Add (Vector(197.1633711073122,99.39865660892818, 0.0));
    positionAlloc4->Add (Vector(283.3948135205256,199.5393842672887, 0.0));
    positionAlloc4->Add (Vector(185.93844798200826,382.19459944824314, 0.0));
    positionAlloc4->Add (Vector(159.35096844109773,245.89172289622172, 0.0));
    positionAlloc4->Add (Vector(143.36133142025128,212.71732727614344, 0.0));
    positionAlloc4->Add (Vector(236.62263732735113,380.7542423060181, 0.0));
    positionAlloc4->Add (Vector(174.1230029285829,243.50391895789758, 0.0));
    positionAlloc4->Add (Vector(296.73312171607137,86.62123648495088, 0.0));
    positionAlloc4->Add (Vector(287.7041713526094,340.45516775353303, 0.0));
    positionAlloc4->Add (Vector(385.21442331823266,103.80518528218725, 0.0));
    positionAlloc4->Add (Vector(141.3056063878062,175.14959357331966, 0.0));
    positionAlloc4->Add (Vector(262.7048266616689,316.87758349557254, 0.0));
    positionAlloc4->Add (Vector(173.7543770546224,121.00031295967177, 0.0));
    positionAlloc4->Add (Vector(106.07901275290467,192.626560442409, 0.0));
    positionAlloc4->Add (Vector(368.93371219118757,234.53235151225505, 0.0));
    positionAlloc4->Add (Vector(296.57516597127085,136.06546991430636, 0.0));
    positionAlloc4->Add (Vector(73.00171922176327,390.4070623282377, 0.0));
    positionAlloc4->Add (Vector(204.96790186356594,154.25615726660652, 0.0));
    positionAlloc4->Add (Vector(125.76213553692486,333.0124341478339, 0.0));
    positionAlloc4->Add (Vector(34.892388877192374,37.20990568429849, 0.0));
    positionAlloc4->Add (Vector(99.76809231501606,103.65497223993181, 0.0));
    positionAlloc4->Add (Vector(265.9931435886838,388.7657422399971, 0.0));
    positionAlloc4->Add (Vector(278.79714755328007,23.948730393514506, 0.0));
    positionAlloc4->Add (Vector(46.991768290405744,155.65202184209917, 0.0));
    positionAlloc4->Add (Vector(27.764032356041746,301.2885479337952, 0.0));
    positionAlloc4->Add (Vector(206.28669785566208,314.6365424658463, 0.0));
    positionAlloc4->Add (Vector(372.94649253114807,138.14710706318652, 0.0));
    positionAlloc4->Add (Vector(217.68460759962255,280.7809096253214, 0.0));
    positionAlloc4->Add (Vector(68.68220553859254,384.928692971948, 0.0));
    positionAlloc4->Add (Vector(107.44062667360885,232.76089559270505, 0.0));
    positionAlloc4->Add (Vector(40.87255127091392,167.71798473286375, 0.0));
    positionAlloc4->Add (Vector(115.40029318756741,201.69606337655455, 0.0));
    positionAlloc4->Add (Vector(67.14529519495258,196.44194702336816, 0.0));
    positionAlloc4->Add (Vector(257.3629551779848,50.57324135501937, 0.0));
    positionAlloc4->Add (Vector(398.269223623717,101.9630470633722, 0.0));
    positionAlloc4->Add (Vector(246.0754710938707,23.642174590222574, 0.0));
    positionAlloc4->Add (Vector(184.58174490525087,218.44400855555855, 0.0));
    positionAlloc4->Add (Vector(321.42463687604106,290.74944840460273, 0.0));
    positionAlloc4->Add (Vector(43.60817523485774,279.18383793547747, 0.0));
    positionAlloc4->Add (Vector(190.3817200969014,384.3852295490987, 0.0));
    positionAlloc4->Add (Vector(37.38254906679437,311.3767441092738, 0.0));
    positionAlloc4->Add (Vector(73.78981353793588,205.15578254928468, 0.0));
    positionAlloc4->Add (Vector(297.3771768831567,148.4320356304762, 0.0));
    positionAlloc4->Add (Vector(32.932747835567696,293.48373279420673, 0.0));
    positionAlloc4->Add (Vector(225.84513076839517,101.93109691086453, 0.0));
    positionAlloc4->Add (Vector(173.0124851560749,26.797975711240074, 0.0));
    positionAlloc4->Add (Vector(322.73024743564014,209.2627576241703, 0.0));
    positionAlloc4->Add (Vector(40.669048778647635,296.9799896138329, 0.0));
    positionAlloc4->Add (Vector(45.27921386523248,186.2715794606322, 0.0));
    positionAlloc4->Add (Vector(227.31083492249144,392.1075728143155, 0.0));
    positionAlloc4->Add (Vector(290.0459384108735,364.095548485201, 0.0));
    positionAlloc4->Add (Vector(247.06220475525367,255.95123720002113, 0.0));
    positionAlloc4->Add (Vector(145.77830764774023,200.86735076041916, 0.0));
    positionAlloc4->Add (Vector(270.8771217734277,32.69221116352665, 0.0));
    positionAlloc4->Add (Vector(397.3281574793273,338.2213322764441, 0.0));
    positionAlloc4->Add (Vector(264.2309722308181,221.80421338364437, 0.0));
    positionAlloc4->Add (Vector(102.63967748386963,369.5697606383114, 0.0));
    positionAlloc4->Add (Vector(348.08434965214536,228.6672241044425, 0.0));
    positionAlloc4->Add (Vector(66.07061927563032,129.6429426639678, 0.0));
    positionAlloc4->Add (Vector(72.77710201189214,305.6604260574483, 0.0));
    positionAlloc4->Add (Vector(274.8265558401964,272.3613500267735, 0.0));
    positionAlloc4->Add (Vector(222.81056368623453,154.45102109810605, 0.0));
    positionAlloc4->Add (Vector(322.46297608490687,322.71972964569267, 0.0));
    positionAlloc4->Add (Vector(347.6661659524887,187.76801044868998, 0.0));
    positionAlloc4->Add (Vector(186.7075718877848,121.98733310728778, 0.0));
    positionAlloc4->Add (Vector(281.36068638901895,166.9815022121779, 0.0));
    positionAlloc4->Add (Vector(234.0464928464345,64.18707448481373, 0.0));
    positionAlloc4->Add (Vector(394.6891401752249,377.6584263716654, 0.0));
    positionAlloc4->Add (Vector(318.31947126009163,86.33304434471695, 0.0));
    positionAlloc4->Add (Vector(272.1470214191572,1.8497793711266475, 0.0));
    positionAlloc4->Add (Vector(319.49895700374793,85.63474612400874, 0.0));
    positionAlloc4->Add (Vector(311.6058288588599,347.4516762783312, 0.0));
    positionAlloc4->Add (Vector(398.4443152258886,213.61966204163673, 0.0));
    positionAlloc4->Add (Vector(281.34432867413295,164.50335610287775, 0.0));
    positionAlloc4->Add (Vector(48.947722350342545,34.67560302546655, 0.0));
    positionAlloc4->Add (Vector(58.56180506501482,266.42909587568744, 0.0));
    positionAlloc4->Add (Vector(47.740731387081595,177.89334152917294, 0.0));
    positionAlloc4->Add (Vector(329.9257401684156,232.68200225375946, 0.0));
    positionAlloc4->Add (Vector(163.89811090091345,163.28757055444467, 0.0));
    positionAlloc4->Add (Vector(392.2571879752278,184.161504689856, 0.0));
    positionAlloc4->Add (Vector(10.230275661696409,123.37080760154429, 0.0));
    positionAlloc4->Add (Vector(1.7758501323977605,36.64780374142143, 0.0));
    positionAlloc4->Add (Vector(388.14871154114996,329.51287895523166, 0.0));
    positionAlloc4->Add (Vector(55.35726732305677,209.12095179491726, 0.0));
    positionAlloc4->Add (Vector(98.99671784003368,178.67040473334598, 0.0));
    positionAlloc4->Add (Vector(231.18443291628182,191.8383708853131, 0.0));
    positionAlloc4->Add (Vector(87.08230066821923,41.008017821290906, 0.0));
    positionAlloc4->Add (Vector(104.49148414488371,200.42775853772147, 0.0));
    positionAlloc4->Add (Vector(348.92343767149003,281.86628028425366, 0.0));
    positionAlloc4->Add (Vector(150.3043313592577,364.4426813795304, 0.0));
    positionAlloc4->Add (Vector(240.20858856393295,295.1997468294403, 0.0));
    positionAlloc4->Add (Vector(197.93002141389917,294.8609779504582, 0.0));
    positionAlloc4->Add (Vector(379.44242307402527,366.87561400886926, 0.0));
    positionAlloc4->Add (Vector(319.18632629105224,392.4642700381915, 0.0));
    positionAlloc4->Add (Vector(198.79123982362833,334.23105670083356, 0.0));
    positionAlloc4->Add (Vector(339.53844071453045,235.25924336992134, 0.0));
    positionAlloc4->Add (Vector(175.04902825113749,235.93273720132692, 0.0));
    positionAlloc4->Add (Vector(124.2667940495842,264.5680026706084, 0.0));
    positionAlloc4->Add (Vector(384.1600088880575,80.3750770904089, 0.0));
    positionAlloc4->Add (Vector(133.58507822963878,63.92804052617507, 0.0));
    positionAlloc4->Add (Vector(106.92451460766125,53.30595811997094, 0.0));
    positionAlloc4->Add (Vector(331.35215425981653,268.79997433673077, 0.0));
    positionAlloc4->Add (Vector(187.64421468880877,378.9369013837442, 0.0));
    positionAlloc4->Add (Vector(379.21780219817384,150.36571695743416, 0.0));
    positionAlloc4->Add (Vector(132.46297560949264,130.5759312818985, 0.0));
    positionAlloc4->Add (Vector(224.09788841466002,51.35196495737411, 0.0));
    positionAlloc4->Add (Vector(98.0401653378971,50.8399925440187, 0.0));
    positionAlloc4->Add (Vector(211.5293679558683,341.9360362099935, 0.0));
    positionAlloc4->Add (Vector(312.9815188593862,215.33554389743836, 0.0));
    positionAlloc4->Add (Vector(103.07793874899605,204.22662285682782, 0.0));
    positionAlloc4->Add (Vector(64.84200657081196,332.70679071708673, 0.0));
    positionAlloc4->Add (Vector(49.68693522346763,222.15689548283618, 0.0));
    positionAlloc4->Add (Vector(37.00809030281533,156.22294299569924, 0.0));
    positionAlloc4->Add (Vector(386.5552726288354,335.6983722298739, 0.0));
    positionAlloc4->Add (Vector(278.5183093314741,108.62020125370351, 0.0));
    positionAlloc4->Add (Vector(47.56814840848236,44.33523179204211, 0.0));
    positionAlloc4->Add (Vector(63.99105144399799,201.4822046756614, 0.0));
    positionAlloc4->Add (Vector(383.58943019066754,356.9456397821998, 0.0));
    positionAlloc4->Add (Vector(352.75756532802166,96.75648215821968, 0.0));
    positionAlloc4->Add (Vector(62.915152914215035,33.00269139123251, 0.0));
    positionAlloc4->Add (Vector(270.71002184300295,201.30627518792136, 0.0));
    positionAlloc4->Add (Vector(68.60295949036504,104.36718358447577, 0.0));
    positionAlloc4->Add (Vector(67.62988974320612,227.04501667872066, 0.0));
    positionAlloc4->Add (Vector(51.33029138048215,377.92531554797324, 0.0));
    positionAlloc4->Add (Vector(85.35957540608088,99.69557801368336, 0.0));
    positionAlloc4->Add (Vector(175.23007932643813,87.47563691906892, 0.0));
    positionAlloc4->Add (Vector(198.2685147720876,2.688530341794948, 0.0));
    positionAlloc4->Add (Vector(26.58734806235077,135.15496240281203, 0.0));
    positionAlloc4->Add (Vector(126.74965405169188,166.94738430358908, 0.0));
    positionAlloc4->Add (Vector(172.27188568077017,73.03496545441494, 0.0));
    positionAlloc4->Add (Vector(323.1024108968933,153.02706336999248, 0.0));
    positionAlloc4->Add (Vector(101.30547837903983,335.08511251046724, 0.0));
    positionAlloc4->Add (Vector(117.48203813767692,3.5156199105464747, 0.0));
    positionAlloc4->Add (Vector(144.4145725285073,45.62797087536295, 0.0));
    positionAlloc4->Add (Vector(127.31025638399399,88.65693824291424, 0.0));
    positionAlloc4->Add (Vector(19.446549880564,327.7047377703287, 0.0));
    positionAlloc4->Add (Vector(141.29936631834892,280.9850949456856, 0.0));
    positionAlloc4->Add (Vector(292.4567109724936,351.3733089145308, 0.0));
    positionAlloc4->Add (Vector(164.6047535830335,238.9034264881813, 0.0));
    positionAlloc4->Add (Vector(87.23984717125161,30.878114863957506, 0.0));
    positionAlloc4->Add (Vector(372.59846978948445,23.5499582365049, 0.0));
    positionAlloc4->Add (Vector(274.480218567855,56.96222051664566, 0.0));
    positionAlloc4->Add (Vector(41.71763984381109,176.1374986492244, 0.0));
    positionAlloc4->Add (Vector(75.8902452417058,369.41107025697085, 0.0));
    positionAlloc4->Add (Vector(180.12953828643376,138.32948360732559, 0.0));
    positionAlloc4->Add (Vector(143.38537885101826,399.750147936233, 0.0));
    positionAlloc4->Add (Vector(163.47205215970106,220.70747942530406, 0.0));
    positionAlloc4->Add (Vector(27.819886856113563,225.17239361304027, 0.0));
    positionAlloc4->Add (Vector(200.0414687258742,321.71798203369576, 0.0));
    positionAlloc4->Add (Vector(18.951129694416924,255.6153282375348, 0.0));
    positionAlloc4->Add (Vector(222.73280285232912,221.0839691918063, 0.0));
    positionAlloc4->Add (Vector(12.758905063630843,381.12391806937296, 0.0));
    positionAlloc4->Add (Vector(320.60224540264426,101.24593336518059, 0.0));
    positionAlloc4->Add (Vector(108.32964775676328,396.22021183073343, 0.0));
    positionAlloc4->Add (Vector(399.5922297657003,316.20295127005164, 0.0));
    positionAlloc4->Add (Vector(383.04369737110113,251.00334003988442, 0.0));
    positionAlloc4->Add (Vector(344.8124227567341,358.62512821745344, 0.0));
    positionAlloc4->Add (Vector(232.30014553884382,287.4694441069025, 0.0));
    positionAlloc4->Add (Vector(102.29434543738574,356.69391864261013, 0.0));
    positionAlloc4->Add (Vector(113.63304606750492,219.39085444828456, 0.0));
    positionAlloc4->Add (Vector(168.88486133095827,61.56718626073419, 0.0));
    positionAlloc4->Add (Vector(268.13615996084496,298.02635640390366, 0.0));
    positionAlloc4->Add (Vector(352.23876077276077,188.09623981147934, 0.0));
    positionAlloc4->Add (Vector(260.90225745711456,366.19870677744547, 0.0));
    positionAlloc4->Add (Vector(15.042823197920185,352.3075625940535, 0.0));
    positionAlloc4->Add (Vector(174.85944448692717,43.80666485493316, 0.0));
    positionAlloc4->Add (Vector(288.6962644772187,361.4871001866889, 0.0));
    positionAlloc4->Add (Vector(20.81228951618681,270.11424259843994, 0.0));
    positionAlloc4->Add (Vector(130.2299277759327,128.86120748045147, 0.0));
    positionAlloc4->Add (Vector(188.9384850702864,375.59805218284106, 0.0));
    positionAlloc4->Add (Vector(35.252027324841166,7.540818502157842, 0.0));
    positionAlloc4->Add (Vector(97.7865171207529,269.3449757599412, 0.0));
    positionAlloc4->Add (Vector(108.98455686428115,332.3732993358843, 0.0));
    positionAlloc4->Add (Vector(241.46218072406077,265.7499776072397, 0.0));
    positionAlloc4->Add (Vector(192.57180660371108,335.1251015611838, 0.0));
    positionAlloc4->Add (Vector(203.84564617485984,398.2088105503276, 0.0));
    positionAlloc4->Add (Vector(359.21054877220206,48.333873621534764, 0.0));
    positionAlloc4->Add (Vector(232.4442662543128,238.65054704570827, 0.0));
    positionAlloc4->Add (Vector(196.7991209574015,196.0194270321906, 0.0));
    positionAlloc4->Add (Vector(311.8733954392515,297.466230819782, 0.0));
    positionAlloc4->Add (Vector(74.30668049216234,377.67733987663365, 0.0));
    positionAlloc4->Add (Vector(30.906906283225588,158.3609980932558, 0.0));
    positionAlloc4->Add (Vector(317.5919599923002,13.578646678856376, 0.0));
    positionAlloc4->Add (Vector(355.7596915619108,90.99142991852429, 0.0));
    positionAlloc4->Add (Vector(343.2961099057884,228.23279663140457, 0.0));
    positionAlloc4->Add (Vector(300.05015161309115,222.79130743579256, 0.0));
    positionAlloc4->Add (Vector(271.16566514446856,134.4399482629166, 0.0));
    positionAlloc4->Add (Vector(2.6991245154568855,103.62382152482668, 0.0));
    positionAlloc4->Add (Vector(273.0835462225287,304.98283819853975, 0.0));
    positionAlloc4->Add (Vector(253.95024920059637,140.92394440472748, 0.0));
    positionAlloc4->Add (Vector(176.7883621860324,53.95660785070406, 0.0));
    positionAlloc4->Add (Vector(325.82217552705373,204.18671562659787, 0.0));
    positionAlloc4->Add (Vector(201.10948033189996,37.671065710162125, 0.0));
    positionAlloc4->Add (Vector(302.79129316276385,119.4368462222922, 0.0));
    positionAlloc4->Add (Vector(283.5226679718974,39.753445882504224, 0.0));
    positionAlloc4->Add (Vector(257.45014259241395,190.62744658128105, 0.0));
    positionAlloc4->Add (Vector(95.12346438031427,332.38782177757724, 0.0));
    positionAlloc4->Add (Vector(14.710650166085193,160.20142518929546, 0.0));
    positionAlloc4->Add (Vector(155.8518173704937,106.13758833246214, 0.0));
    positionAlloc4->Add (Vector(357.28099888000855,22.98535356671629, 0.0));
    positionAlloc4->Add (Vector(1.176577366497611,160.23883528313934, 0.0));
    positionAlloc4->Add (Vector(70.8124218398432,160.70257208281308, 0.0));
    positionAlloc4->Add (Vector(292.25654035418825,311.1330798849096, 0.0));
    positionAlloc4->Add (Vector(121.92899706852884,141.2390832003314, 0.0));
    positionAlloc4->Add (Vector(275.19156574990717,335.3760165120497, 0.0));
    positionAlloc4->Add (Vector(63.52401071304503,123.02539142822737, 0.0));
    positionAlloc4->Add (Vector(52.53671939571301,147.06945015463268, 0.0));
    positionAlloc4->Add (Vector(155.11295344262294,71.17258432590403, 0.0));
    positionAlloc4->Add (Vector(147.77867206674543,31.98523560851725, 0.0));
    positionAlloc4->Add (Vector(391.65865223069915,164.67561746122777, 0.0));
    positionAlloc4->Add (Vector(181.39325699940767,193.01195195894425, 0.0));
    positionAlloc4->Add (Vector(138.26110584929597,362.1850562335346, 0.0));
    positionAlloc4->Add (Vector(224.05914460960554,135.0985664989337, 0.0));
    positionAlloc4->Add (Vector(144.52981349531672,281.372863821868, 0.0));
    positionAlloc4->Add (Vector(28.02916951440846,354.38198976511813, 0.0));
    positionAlloc4->Add (Vector(292.453894472036,38.02479567761963, 0.0));
    positionAlloc4->Add (Vector(85.51368580737346,256.44201385105487, 0.0));
    positionAlloc4->Add (Vector(298.71311074045275,317.5881862375771, 0.0));
    positionAlloc4->Add (Vector(22.53589042079738,242.5609489097364, 0.0));
    positionAlloc4->Add (Vector(193.72214818172563,265.72850817993253, 0.0));
    positionAlloc4->Add (Vector(233.36641543925697,141.6679063059365, 0.0));
    positionAlloc4->Add (Vector(147.44102250839268,261.09202863375367, 0.0));
    positionAlloc4->Add (Vector(376.9952384632591,118.54509343332538, 0.0));
    positionAlloc4->Add (Vector(335.7529101238499,166.72367490281664, 0.0));
    positionAlloc4->Add (Vector(324.3126489955618,147.37822932949462, 0.0));
    positionAlloc4->Add (Vector(149.71833451367326,232.99507498239024, 0.0));
    positionAlloc4->Add (Vector(251.94871781102543,66.708451841368, 0.0));
    positionAlloc4->Add (Vector(211.69359061529346,193.07410066600016, 0.0));
    positionAlloc4->Add (Vector(79.05512248552537,123.85314265079788, 0.0));
    positionAlloc4->Add (Vector(347.55777782164284,113.3658631523883, 0.0));
    positionAlloc4->Add (Vector(336.7469635449277,5.28198328936309, 0.0));
    positionAlloc4->Add (Vector(181.2264737563238,393.2809526913538, 0.0));
    positionAlloc4->Add (Vector(19.842915095020388,58.6465487339328, 0.0));
    positionAlloc4->Add (Vector(34.437862705604026,143.97288636712707, 0.0));
    positionAlloc4->Add (Vector(267.82536782218006,87.19630504200539, 0.0));
    positionAlloc4->Add (Vector(270.8721051362301,45.05171884919661, 0.0));
    positionAlloc4->Add (Vector(149.89275495644415,361.4218738688417, 0.0));
    positionAlloc4->Add (Vector(247.83628096736606,227.24804884623813, 0.0));
    positionAlloc4->Add (Vector(64.80558279588347,167.13077120147636, 0.0));
    positionAlloc4->Add (Vector(381.121447769152,390.2616782819121, 0.0));
    positionAlloc4->Add (Vector(223.62662326303246,31.364319639913283, 0.0));
    positionAlloc4->Add (Vector(187.32492239769675,282.4774240118469, 0.0));
    positionAlloc4->Add (Vector(295.70803843712844,288.9208996073545, 0.0));
    positionAlloc4->Add (Vector(100.99801163901657,202.4083812775801, 0.0));
    positionAlloc4->Add (Vector(224.61851797184468,287.0449413691187, 0.0));
    positionAlloc4->Add (Vector(97.02852186161999,88.60278959167549, 0.0));
    positionAlloc4->Add (Vector(335.05846905277025,2.2772703717631515, 0.0));
    positionAlloc4->Add (Vector(194.76078915866267,214.36801491842536, 0.0));
    positionAlloc4->Add (Vector(274.78548112785944,355.8014264366765, 0.0));
    positionAlloc4->Add (Vector(225.26657121549337,318.47318106440923, 0.0));
    positionAlloc4->Add (Vector(138.619111287971,259.26397037299364, 0.0));
    positionAlloc4->Add (Vector(15.593366613763937,86.18631618529591, 0.0));
    positionAlloc4->Add (Vector(118.29741581596748,343.98469584528624, 0.0));
    positionAlloc4->Add (Vector(60.89148328479976,28.182345419988586, 0.0));
    positionAlloc4->Add (Vector(163.32189173484437,149.92231960430118, 0.0));
    positionAlloc4->Add (Vector(353.42276680225876,97.43204861407287, 0.0));
    positionAlloc4->Add (Vector(375.2553794596655,393.89505929654524, 0.0));
    positionAlloc4->Add (Vector(269.4219682788428,353.9345809068709, 0.0));
    positionAlloc4->Add (Vector(315.63008086753524,186.58708143334843, 0.0));
    positionAlloc4->Add (Vector(85.26919639614295,174.3553898941081, 0.0));
    positionAlloc4->Add (Vector(125.50337206160447,246.31448959647898, 0.0));
    positionAlloc4->Add (Vector(91.99444190112888,207.2470801035597, 0.0));
    positionAlloc4->Add (Vector(336.72929746619025,299.1119973238234, 0.0));
    positionAlloc4->Add (Vector(371.8933372834288,30.32335865433606, 0.0));
    positionAlloc4->Add (Vector(11.813074121520417,234.87370008997007, 0.0));
    positionAlloc4->Add (Vector(86.8447228818494,169.03858578909646, 0.0));
    positionAlloc4->Add (Vector(133.20426665155446,200.44405997638765, 0.0));
    positionAlloc4->Add (Vector(344.6387255192878,11.6681723620081, 0.0));
    positionAlloc4->Add (Vector(358.9097878018518,235.0938797441638, 0.0));
    positionAlloc4->Add (Vector(361.75070990606855,18.41228030757911, 0.0));
    positionAlloc4->Add (Vector(209.7420699015401,85.02890007383614, 0.0));
    positionAlloc4->Add (Vector(82.20364177267037,39.72915478086541, 0.0));
    positionAlloc4->Add (Vector(66.90862608785596,128.8089623851269, 0.0));
    positionAlloc4->Add (Vector(191.31124302012768,328.82156634800464, 0.0));
    positionAlloc4->Add (Vector(235.97599232406333,163.43204801270622, 0.0));
    positionAlloc4->Add (Vector(17.514469858286176,54.788516012144584, 0.0));
    positionAlloc4->Add (Vector(168.3454187164253,336.5915953137542, 0.0));
    positionAlloc4->Add (Vector(140.31003445452015,64.76965240734897, 0.0));
    positionAlloc4->Add (Vector(78.80235044268602,364.73563859971557, 0.0));
    positionAlloc4->Add (Vector(248.11619982169174,389.0949588883682, 0.0));
    positionAlloc4->Add (Vector(98.97823205686298,175.9841418199135, 0.0));
    positionAlloc4->Add (Vector(56.566751480547865,282.8930446882565, 0.0));
    positionAlloc4->Add (Vector(18.108014463500545,215.23405796411845, 0.0));
    positionAlloc4->Add (Vector(272.77865314828273,14.689473549325704, 0.0));
    positionAlloc4->Add (Vector(205.9996984149559,334.5017830486922, 0.0));
    positionAlloc4->Add (Vector(94.25050299182409,30.693594740617193, 0.0));
    positionAlloc4->Add (Vector(60.06033631478762,215.29876390873514, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_12.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_12.flowmon", false, false);
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