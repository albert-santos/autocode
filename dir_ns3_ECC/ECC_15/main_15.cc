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
	  uint16_t numberOfRrhs = 62;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 584;
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
    positionAlloc->Add (Vector (500.0,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (500.0,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (500.0,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (500.0,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,500.0, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,0.0, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,0.0, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,0.0, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,0.0, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,500.0, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,0.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (0.0,55.55555555555556, 0.0));
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
    positionAlloc4->Add (Vector(254.6382221278129,424.60719856412237, 0.0));
    positionAlloc4->Add (Vector(198.79202289661873,127.79216485309702, 0.0));
    positionAlloc4->Add (Vector(54.28817284794457,473.6385864887473, 0.0));
    positionAlloc4->Add (Vector(246.5794329366293,459.3438149658387, 0.0));
    positionAlloc4->Add (Vector(406.72010717223054,41.21620049631686, 0.0));
    positionAlloc4->Add (Vector(213.2930474283342,419.5310702412852, 0.0));
    positionAlloc4->Add (Vector(49.063650857855855,154.0803471346937, 0.0));
    positionAlloc4->Add (Vector(128.13749753547165,295.3810843022639, 0.0));
    positionAlloc4->Add (Vector(309.6330711038434,325.49057521235625, 0.0));
    positionAlloc4->Add (Vector(373.34035603399093,469.5323130460874, 0.0));
    positionAlloc4->Add (Vector(134.72405197816522,53.41787210577109, 0.0));
    positionAlloc4->Add (Vector(175.49875403336657,86.3403103149098, 0.0));
    positionAlloc4->Add (Vector(354.8672279759286,451.4458096210105, 0.0));
    positionAlloc4->Add (Vector(412.0014709758501,69.41645456558737, 0.0));
    positionAlloc4->Add (Vector(34.817234804578604,242.98693760631247, 0.0));
    positionAlloc4->Add (Vector(197.44680619674583,199.21834586890353, 0.0));
    positionAlloc4->Add (Vector(437.83323111361835,12.763608114829294, 0.0));
    positionAlloc4->Add (Vector(138.5578544187262,282.3764374711506, 0.0));
    positionAlloc4->Add (Vector(483.89528272432426,28.705762291978797, 0.0));
    positionAlloc4->Add (Vector(86.02733537120294,381.76978725957764, 0.0));
    positionAlloc4->Add (Vector(150.5340762108437,228.5964406986371, 0.0));
    positionAlloc4->Add (Vector(414.38865580781896,303.09241463846706, 0.0));
    positionAlloc4->Add (Vector(272.89602841808244,13.64246307605088, 0.0));
    positionAlloc4->Add (Vector(17.55777641789197,119.39371085774286, 0.0));
    positionAlloc4->Add (Vector(390.2549118470776,478.75869171762326, 0.0));
    positionAlloc4->Add (Vector(338.51589328105507,134.40357734950155, 0.0));
    positionAlloc4->Add (Vector(149.28828210660444,437.7577322950982, 0.0));
    positionAlloc4->Add (Vector(270.6910298771477,227.0449586290386, 0.0));
    positionAlloc4->Add (Vector(115.51952992350762,53.56741911524365, 0.0));
    positionAlloc4->Add (Vector(129.46645484870473,339.45333866309176, 0.0));
    positionAlloc4->Add (Vector(264.5431742686829,117.03151214364233, 0.0));
    positionAlloc4->Add (Vector(249.3328652768268,260.34227590756205, 0.0));
    positionAlloc4->Add (Vector(101.01298335458392,325.5214542636995, 0.0));
    positionAlloc4->Add (Vector(53.44326459910725,172.13887103732333, 0.0));
    positionAlloc4->Add (Vector(327.26755893365714,410.9919700941124, 0.0));
    positionAlloc4->Add (Vector(4.018246144452176,493.9740667175476, 0.0));
    positionAlloc4->Add (Vector(464.77541228950014,239.1656183683642, 0.0));
    positionAlloc4->Add (Vector(353.08750765413583,9.454025747059614, 0.0));
    positionAlloc4->Add (Vector(470.4743412498056,52.86566248791824, 0.0));
    positionAlloc4->Add (Vector(232.3996603381844,11.012878715738307, 0.0));
    positionAlloc4->Add (Vector(110.80423449551274,352.02697339855587, 0.0));
    positionAlloc4->Add (Vector(339.9065939459309,41.3833656971786, 0.0));
    positionAlloc4->Add (Vector(52.535075592716254,41.580972193165586, 0.0));
    positionAlloc4->Add (Vector(186.1726047731822,161.74102577414723, 0.0));
    positionAlloc4->Add (Vector(460.55357801974156,1.7391759919620497, 0.0));
    positionAlloc4->Add (Vector(362.8335172280389,365.23293678088487, 0.0));
    positionAlloc4->Add (Vector(153.9102497105429,61.751038628893085, 0.0));
    positionAlloc4->Add (Vector(258.0160919090245,69.14954220323216, 0.0));
    positionAlloc4->Add (Vector(19.72561621016755,2.554445203291733, 0.0));
    positionAlloc4->Add (Vector(27.358045611785677,258.29050641942393, 0.0));
    positionAlloc4->Add (Vector(80.9035419017905,379.67565160071325, 0.0));
    positionAlloc4->Add (Vector(126.53236048277367,199.66562777520508, 0.0));
    positionAlloc4->Add (Vector(409.6817957714628,49.86766704558326, 0.0));
    positionAlloc4->Add (Vector(29.769959762647012,280.9235359146055, 0.0));
    positionAlloc4->Add (Vector(66.03830365435876,331.8307411905895, 0.0));
    positionAlloc4->Add (Vector(400.5880945617563,369.5262623156105, 0.0));
    positionAlloc4->Add (Vector(36.45112861400157,287.7109233452209, 0.0));
    positionAlloc4->Add (Vector(313.7174929094133,185.80874165075767, 0.0));
    positionAlloc4->Add (Vector(55.008104873763784,152.7739780075168, 0.0));
    positionAlloc4->Add (Vector(92.22301517588122,86.76613108028913, 0.0));
    positionAlloc4->Add (Vector(368.15631936828106,265.62781189834743, 0.0));
    positionAlloc4->Add (Vector(376.8102821493151,67.56169159258485, 0.0));
    positionAlloc4->Add (Vector(378.4916425862624,397.8373695562636, 0.0));
    positionAlloc4->Add (Vector(2.429475500946632,234.0441234650552, 0.0));
    positionAlloc4->Add (Vector(115.52181288612984,420.885866508479, 0.0));
    positionAlloc4->Add (Vector(487.3070881514868,48.017401212128604, 0.0));
    positionAlloc4->Add (Vector(435.40578011429574,12.100378781246313, 0.0));
    positionAlloc4->Add (Vector(288.62160413743493,171.4807439394387, 0.0));
    positionAlloc4->Add (Vector(242.88028159861346,314.94060523973053, 0.0));
    positionAlloc4->Add (Vector(335.4208534707551,239.2250185778057, 0.0));
    positionAlloc4->Add (Vector(378.8496706253084,80.5321522076668, 0.0));
    positionAlloc4->Add (Vector(298.5322622326339,339.5411738445256, 0.0));
    positionAlloc4->Add (Vector(59.60059563205755,117.79795582402458, 0.0));
    positionAlloc4->Add (Vector(159.27029879584242,487.60056750712386, 0.0));
    positionAlloc4->Add (Vector(440.5244432046621,103.39349005309911, 0.0));
    positionAlloc4->Add (Vector(235.13728066281115,235.28766572852066, 0.0));
    positionAlloc4->Add (Vector(331.6319522304582,21.93021854366539, 0.0));
    positionAlloc4->Add (Vector(118.95436756308442,314.4613448911985, 0.0));
    positionAlloc4->Add (Vector(458.92792411810115,356.34281969885615, 0.0));
    positionAlloc4->Add (Vector(242.48256647535692,460.6102011482572, 0.0));
    positionAlloc4->Add (Vector(409.0566891960518,499.3594079807635, 0.0));
    positionAlloc4->Add (Vector(152.37384884167636,395.0800767264241, 0.0));
    positionAlloc4->Add (Vector(121.74846643563015,25.95423105242267, 0.0));
    positionAlloc4->Add (Vector(184.7024030952366,108.37937394923358, 0.0));
    positionAlloc4->Add (Vector(196.4590651763088,341.8765181963853, 0.0));
    positionAlloc4->Add (Vector(250.09822839879152,1.1248240468140214, 0.0));
    positionAlloc4->Add (Vector(39.038267059794016,255.76828519856502, 0.0));
    positionAlloc4->Add (Vector(376.66203047052846,159.70819585932617, 0.0));
    positionAlloc4->Add (Vector(86.28071063997406,143.71986284882814, 0.0));
    positionAlloc4->Add (Vector(415.47780764197483,45.74627346087684, 0.0));
    positionAlloc4->Add (Vector(213.21968541212084,69.47298832594623, 0.0));
    positionAlloc4->Add (Vector(291.2213039687629,62.698556707126095, 0.0));
    positionAlloc4->Add (Vector(320.3433829946312,258.9983928063372, 0.0));
    positionAlloc4->Add (Vector(318.0553844773695,67.93679329176771, 0.0));
    positionAlloc4->Add (Vector(483.07829154864214,468.13921194111106, 0.0));
    positionAlloc4->Add (Vector(494.58533691370377,225.49208291724054, 0.0));
    positionAlloc4->Add (Vector(117.72245965828404,448.9067459223592, 0.0));
    positionAlloc4->Add (Vector(471.8293609822945,125.17398348977899, 0.0));
    positionAlloc4->Add (Vector(276.28172848288347,281.2037522069616, 0.0));
    positionAlloc4->Add (Vector(320.4973485771602,368.2140733301819, 0.0));
    positionAlloc4->Add (Vector(229.48996409954003,61.61714156235665, 0.0));
    positionAlloc4->Add (Vector(474.38018293610384,389.55417654534017, 0.0));
    positionAlloc4->Add (Vector(17.422437085732824,140.18337229397244, 0.0));
    positionAlloc4->Add (Vector(420.9145868023059,434.06944385232373, 0.0));
    positionAlloc4->Add (Vector(309.1651647726442,230.4981699002555, 0.0));
    positionAlloc4->Add (Vector(256.9687621126541,402.2511772543922, 0.0));
    positionAlloc4->Add (Vector(129.19455528002953,404.758085491509, 0.0));
    positionAlloc4->Add (Vector(18.63171118545942,389.1508801075578, 0.0));
    positionAlloc4->Add (Vector(409.8581379561693,296.06562288754276, 0.0));
    positionAlloc4->Add (Vector(490.77215004032104,159.12770330831367, 0.0));
    positionAlloc4->Add (Vector(414.3312603073987,429.22603536438515, 0.0));
    positionAlloc4->Add (Vector(209.78330727325013,458.388644771798, 0.0));
    positionAlloc4->Add (Vector(231.11054382358077,66.49128892968015, 0.0));
    positionAlloc4->Add (Vector(373.68066380075766,341.9933146668441, 0.0));
    positionAlloc4->Add (Vector(436.4484687874326,388.39006321216687, 0.0));
    positionAlloc4->Add (Vector(357.1534732502853,494.0087716020402, 0.0));
    positionAlloc4->Add (Vector(346.8943325429115,420.416711762477, 0.0));
    positionAlloc4->Add (Vector(147.11772917988696,483.0391500183356, 0.0));
    positionAlloc4->Add (Vector(105.2581634700523,244.8352741059997, 0.0));
    positionAlloc4->Add (Vector(190.82223947663292,332.376947395101, 0.0));
    positionAlloc4->Add (Vector(347.9670174367275,479.3280431224668, 0.0));
    positionAlloc4->Add (Vector(429.6707935224315,19.085496650353306, 0.0));
    positionAlloc4->Add (Vector(220.1160113488785,443.4141399750603, 0.0));
    positionAlloc4->Add (Vector(43.60932682584695,208.42718191798693, 0.0));
    positionAlloc4->Add (Vector(383.58892141096686,244.74458476569095, 0.0));
    positionAlloc4->Add (Vector(287.7304499740752,222.1515966952994, 0.0));
    positionAlloc4->Add (Vector(56.696426751248865,280.6398426301725, 0.0));
    positionAlloc4->Add (Vector(488.7384423285508,170.63687051482563, 0.0));
    positionAlloc4->Add (Vector(244.26637099164174,428.77262038750564, 0.0));
    positionAlloc4->Add (Vector(428.20579073442036,471.8047380855041, 0.0));
    positionAlloc4->Add (Vector(425.67477336819684,95.01580379929331, 0.0));
    positionAlloc4->Add (Vector(94.2591078593577,404.3937525127852, 0.0));
    positionAlloc4->Add (Vector(193.6977489387808,38.60910509884319, 0.0));
    positionAlloc4->Add (Vector(109.2150335300529,120.8969061348349, 0.0));
    positionAlloc4->Add (Vector(225.24634689851575,22.5894919523304, 0.0));
    positionAlloc4->Add (Vector(61.409044268268374,81.05796574813546, 0.0));
    positionAlloc4->Add (Vector(15.29432206654019,285.65559203013646, 0.0));
    positionAlloc4->Add (Vector(431.50008502853115,123.52929989906114, 0.0));
    positionAlloc4->Add (Vector(244.83895783176413,90.62661872089556, 0.0));
    positionAlloc4->Add (Vector(324.0502626519092,220.744216360244, 0.0));
    positionAlloc4->Add (Vector(395.11656386777514,4.9325641522273695, 0.0));
    positionAlloc4->Add (Vector(158.4067268243815,457.74642248674667, 0.0));
    positionAlloc4->Add (Vector(132.24155380531693,434.9565555615083, 0.0));
    positionAlloc4->Add (Vector(343.5508747384207,480.77015264634775, 0.0));
    positionAlloc4->Add (Vector(114.33040605741863,477.6522200691797, 0.0));
    positionAlloc4->Add (Vector(76.29870031265129,166.23855270379002, 0.0));
    positionAlloc4->Add (Vector(317.2170531216684,191.15159308224995, 0.0));
    positionAlloc4->Add (Vector(39.69910021736711,380.7956648225899, 0.0));
    positionAlloc4->Add (Vector(473.16434376545345,367.47033223342856, 0.0));
    positionAlloc4->Add (Vector(493.4307018970161,276.7852385082637, 0.0));
    positionAlloc4->Add (Vector(236.6662293710854,202.4199245545305, 0.0));
    positionAlloc4->Add (Vector(116.15930109270973,376.49478708339024, 0.0));
    positionAlloc4->Add (Vector(28.707763942770193,487.1919364622936, 0.0));
    positionAlloc4->Add (Vector(460.222050755986,244.99889406388542, 0.0));
    positionAlloc4->Add (Vector(433.88046862343515,31.72568962469624, 0.0));
    positionAlloc4->Add (Vector(100.06816880775999,21.571684658109657, 0.0));
    positionAlloc4->Add (Vector(163.45869483747026,17.44973908579439, 0.0));
    positionAlloc4->Add (Vector(397.4431710469375,29.417349339355916, 0.0));
    positionAlloc4->Add (Vector(478.77172015458694,369.81122727567487, 0.0));
    positionAlloc4->Add (Vector(204.61241333888853,20.435136054525405, 0.0));
    positionAlloc4->Add (Vector(33.135760146961566,77.11628123064762, 0.0));
    positionAlloc4->Add (Vector(233.11778866997057,167.3656362887715, 0.0));
    positionAlloc4->Add (Vector(362.4133236209788,241.13166829815324, 0.0));
    positionAlloc4->Add (Vector(434.5423766134478,453.0251477115606, 0.0));
    positionAlloc4->Add (Vector(306.44989421885236,283.5356727857266, 0.0));
    positionAlloc4->Add (Vector(154.50809489775165,124.97805854333477, 0.0));
    positionAlloc4->Add (Vector(41.36979714104888,23.440113998688894, 0.0));
    positionAlloc4->Add (Vector(278.4265024090344,433.0878146356558, 0.0));
    positionAlloc4->Add (Vector(447.6652062819338,237.5333933711935, 0.0));
    positionAlloc4->Add (Vector(472.69638123866525,469.42705259914845, 0.0));
    positionAlloc4->Add (Vector(415.1986219838837,265.5926731439687, 0.0));
    positionAlloc4->Add (Vector(13.017098021974427,5.080820228185012, 0.0));
    positionAlloc4->Add (Vector(322.3740890505002,230.42803867554673, 0.0));
    positionAlloc4->Add (Vector(332.8947846369808,180.6962763364658, 0.0));
    positionAlloc4->Add (Vector(49.704154032142455,386.7278792029437, 0.0));
    positionAlloc4->Add (Vector(458.7124673543327,43.91530471304528, 0.0));
    positionAlloc4->Add (Vector(329.2159557407916,289.76952599846896, 0.0));
    positionAlloc4->Add (Vector(128.15588018718339,463.4858338705238, 0.0));
    positionAlloc4->Add (Vector(433.43780526063006,271.1300379081237, 0.0));
    positionAlloc4->Add (Vector(276.5203239112859,382.57896529257727, 0.0));
    positionAlloc4->Add (Vector(329.55494114540335,41.259704557648604, 0.0));
    positionAlloc4->Add (Vector(87.60052278847974,188.28730953308025, 0.0));
    positionAlloc4->Add (Vector(363.09149436463446,270.60613135901787, 0.0));
    positionAlloc4->Add (Vector(142.17184874078887,68.23560236503434, 0.0));
    positionAlloc4->Add (Vector(215.01689116373745,285.37460996267447, 0.0));
    positionAlloc4->Add (Vector(403.31903010524013,154.38447005218214, 0.0));
    positionAlloc4->Add (Vector(311.9103879686041,375.4844134724319, 0.0));
    positionAlloc4->Add (Vector(363.7365988522562,82.75264684488809, 0.0));
    positionAlloc4->Add (Vector(475.3135063714807,87.83859012634288, 0.0));
    positionAlloc4->Add (Vector(159.1924771347903,227.16671186469216, 0.0));
    positionAlloc4->Add (Vector(398.79316636635684,382.31202516403886, 0.0));
    positionAlloc4->Add (Vector(412.49881927797463,81.96833697123957, 0.0));
    positionAlloc4->Add (Vector(419.885482150326,451.582385904691, 0.0));
    positionAlloc4->Add (Vector(175.32474251279373,0.19672515530083645, 0.0));
    positionAlloc4->Add (Vector(78.19326516452585,433.1763607063499, 0.0));
    positionAlloc4->Add (Vector(488.7886429707596,146.1008746899949, 0.0));
    positionAlloc4->Add (Vector(412.68272611852757,165.89918317636327, 0.0));
    positionAlloc4->Add (Vector(358.2600057549464,326.16379941895593, 0.0));
    positionAlloc4->Add (Vector(401.82999941980046,62.19659442197201, 0.0));
    positionAlloc4->Add (Vector(369.6801230274172,297.24335892703584, 0.0));
    positionAlloc4->Add (Vector(340.5853732101473,448.102665103157, 0.0));
    positionAlloc4->Add (Vector(323.8609067624796,286.6292272592773, 0.0));
    positionAlloc4->Add (Vector(21.266004297555263,480.05590292030627, 0.0));
    positionAlloc4->Add (Vector(161.96999721100212,32.98521370573554, 0.0));
    positionAlloc4->Add (Vector(77.92000338371419,499.1946060575441, 0.0));
    positionAlloc4->Add (Vector(117.52552423412821,321.6110605353873, 0.0));
    positionAlloc4->Add (Vector(485.4650634915478,26.644913385296565, 0.0));
    positionAlloc4->Add (Vector(142.6938763359022,132.08757203883397, 0.0));
    positionAlloc4->Add (Vector(136.8167879433806,30.822741596038206, 0.0));
    positionAlloc4->Add (Vector(294.5751307279062,245.89225822956584, 0.0));
    positionAlloc4->Add (Vector(331.91421620898416,411.90377971741424, 0.0));
    positionAlloc4->Add (Vector(79.28397625434802,122.6563798238946, 0.0));
    positionAlloc4->Add (Vector(349.595324762026,201.55800445896222, 0.0));
    positionAlloc4->Add (Vector(222.6921200607911,319.43843471845395, 0.0));
    positionAlloc4->Add (Vector(328.258844865688,151.97935303402133, 0.0));
    positionAlloc4->Add (Vector(395.65702882079046,431.9292717963336, 0.0));
    positionAlloc4->Add (Vector(415.84301407014715,446.2129471268348, 0.0));
    positionAlloc4->Add (Vector(414.5960922382948,16.947691684809307, 0.0));
    positionAlloc4->Add (Vector(457.4810693261226,257.6721298482893, 0.0));
    positionAlloc4->Add (Vector(291.58914439257177,19.59346288049507, 0.0));
    positionAlloc4->Add (Vector(150.259524035192,383.23772836917874, 0.0));
    positionAlloc4->Add (Vector(262.8114733368048,252.02334197575038, 0.0));
    positionAlloc4->Add (Vector(334.0973946864548,117.0304983035258, 0.0));
    positionAlloc4->Add (Vector(79.7160895505607,196.97416074188573, 0.0));
    positionAlloc4->Add (Vector(421.71248244463953,485.1146781971739, 0.0));
    positionAlloc4->Add (Vector(11.50908727241906,435.9524596295376, 0.0));
    positionAlloc4->Add (Vector(247.2490667160394,29.113858589579543, 0.0));
    positionAlloc4->Add (Vector(163.93631913411343,131.87874870639615, 0.0));
    positionAlloc4->Add (Vector(49.46289891608541,434.26330291304896, 0.0));
    positionAlloc4->Add (Vector(365.1191103296776,78.05849746160642, 0.0));
    positionAlloc4->Add (Vector(434.3151090861338,413.9379614031246, 0.0));
    positionAlloc4->Add (Vector(360.42370865256544,56.11838935021818, 0.0));
    positionAlloc4->Add (Vector(361.3727125111601,387.15674435397347, 0.0));
    positionAlloc4->Add (Vector(175.52045620694085,249.75282660507114, 0.0));
    positionAlloc4->Add (Vector(267.03423257732754,103.33179370873601, 0.0));
    positionAlloc4->Add (Vector(227.6077808673962,72.28386673814263, 0.0));
    positionAlloc4->Add (Vector(254.8596235825642,407.03785287333255, 0.0));
    positionAlloc4->Add (Vector(291.60630483906965,314.9802536479216, 0.0));
    positionAlloc4->Add (Vector(186.13408419011353,34.839749816583335, 0.0));
    positionAlloc4->Add (Vector(431.69728805589403,98.24718139522327, 0.0));
    positionAlloc4->Add (Vector(234.41961792348587,331.44526544572716, 0.0));
    positionAlloc4->Add (Vector(203.5832330337019,132.03904077627172, 0.0));
    positionAlloc4->Add (Vector(316.2235489269981,320.53728922852235, 0.0));
    positionAlloc4->Add (Vector(420.88701878360575,77.8034181147475, 0.0));
    positionAlloc4->Add (Vector(476.8741402528773,253.5196441644592, 0.0));
    positionAlloc4->Add (Vector(52.715178813446784,216.79009455664712, 0.0));
    positionAlloc4->Add (Vector(207.0678582808118,186.92748546721583, 0.0));
    positionAlloc4->Add (Vector(401.6899614488514,262.0110351336129, 0.0));
    positionAlloc4->Add (Vector(499.7275769892641,299.12542180594204, 0.0));
    positionAlloc4->Add (Vector(31.581966502193247,363.207146407181, 0.0));
    positionAlloc4->Add (Vector(190.49787168153003,269.82110086803283, 0.0));
    positionAlloc4->Add (Vector(87.85199708141566,461.1788155993493, 0.0));
    positionAlloc4->Add (Vector(179.92889386898904,483.4709940709111, 0.0));
    positionAlloc4->Add (Vector(160.16563889286323,347.3323109680913, 0.0));
    positionAlloc4->Add (Vector(416.75023294330913,208.05384299229107, 0.0));
    positionAlloc4->Add (Vector(117.79927284680441,392.26520785180185, 0.0));
    positionAlloc4->Add (Vector(66.94710921328235,420.84074977742563, 0.0));
    positionAlloc4->Add (Vector(343.07964688115896,466.9134782012376, 0.0));
    positionAlloc4->Add (Vector(7.535207671836963,208.18207893737213, 0.0));
    positionAlloc4->Add (Vector(497.9826463860361,366.0287769692212, 0.0));
    positionAlloc4->Add (Vector(328.1962389885715,313.85674727146215, 0.0));
    positionAlloc4->Add (Vector(174.548728983505,225.93825324784777, 0.0));
    positionAlloc4->Add (Vector(447.0227794148627,261.67283895750177, 0.0));
    positionAlloc4->Add (Vector(305.54825045366175,127.98948459315723, 0.0));
    positionAlloc4->Add (Vector(382.67696407728545,213.93938936817602, 0.0));
    positionAlloc4->Add (Vector(494.90965629528125,66.53277292931065, 0.0));
    positionAlloc4->Add (Vector(245.92589503417673,366.967919285435, 0.0));
    positionAlloc4->Add (Vector(203.3377823360337,341.4951332078646, 0.0));
    positionAlloc4->Add (Vector(171.6596467166881,223.8385563408648, 0.0));
    positionAlloc4->Add (Vector(212.10702281471393,107.5579954692421, 0.0));
    positionAlloc4->Add (Vector(250.9714517336406,462.49814156908104, 0.0));
    positionAlloc4->Add (Vector(437.08507121439806,418.4603990617691, 0.0));
    positionAlloc4->Add (Vector(41.767560974189266,458.6972961390585, 0.0));
    positionAlloc4->Add (Vector(25.762602683759294,109.83683669910916, 0.0));
    positionAlloc4->Add (Vector(191.36294638551894,55.507584869218306, 0.0));
    positionAlloc4->Add (Vector(293.46382522792555,81.74986055325118, 0.0));
    positionAlloc4->Add (Vector(338.1244467736208,33.97300417236865, 0.0));
    positionAlloc4->Add (Vector(290.45837472178613,413.9452451022877, 0.0));
    positionAlloc4->Add (Vector(465.46715749084433,463.28231510719326, 0.0));
    positionAlloc4->Add (Vector(424.1099796804919,27.12664619511862, 0.0));
    positionAlloc4->Add (Vector(161.04611854856643,143.57961832965432, 0.0));
    positionAlloc4->Add (Vector(255.41954879940437,170.64569178246836, 0.0));
    positionAlloc4->Add (Vector(192.83512910226946,66.63931374616288, 0.0));
    positionAlloc4->Add (Vector(303.1640032884731,200.23852862641894, 0.0));
    positionAlloc4->Add (Vector(226.50980815288878,64.663591031688, 0.0));
    positionAlloc4->Add (Vector(371.1904090304782,48.625715405398005, 0.0));
    positionAlloc4->Add (Vector(217.11365816888727,255.0567440090421, 0.0));
    positionAlloc4->Add (Vector(394.10253576293564,169.77463684236238, 0.0));
    positionAlloc4->Add (Vector(279.6031103289418,480.5715010054463, 0.0));
    positionAlloc4->Add (Vector(206.47081002596957,42.29299442168294, 0.0));
    positionAlloc4->Add (Vector(188.1745637542944,143.72434661094147, 0.0));
    positionAlloc4->Add (Vector(381.24186696037697,242.13817287722466, 0.0));
    positionAlloc4->Add (Vector(213.64680590077896,475.4782453480088, 0.0));
    positionAlloc4->Add (Vector(11.308339571914017,57.29515991338891, 0.0));
    positionAlloc4->Add (Vector(12.481694592422599,131.44220008422496, 0.0));
    positionAlloc4->Add (Vector(438.8259313843722,11.046922125190484, 0.0));
    positionAlloc4->Add (Vector(270.3526445927095,39.72049544305351, 0.0));
    positionAlloc4->Add (Vector(491.5261005354054,119.04022863663654, 0.0));
    positionAlloc4->Add (Vector(62.717856594736,350.4280690041217, 0.0));
    positionAlloc4->Add (Vector(237.52280651885778,369.9885556168082, 0.0));
    positionAlloc4->Add (Vector(59.92987849511566,436.2033125562382, 0.0));
    positionAlloc4->Add (Vector(34.44286758097226,179.18125495927072, 0.0));
    positionAlloc4->Add (Vector(173.85950795962518,288.13389792569666, 0.0));
    positionAlloc4->Add (Vector(219.4778077706086,475.76333854933364, 0.0));
    positionAlloc4->Add (Vector(53.00173213679027,167.11638582294813, 0.0));
    positionAlloc4->Add (Vector(130.07479608483396,35.85838251913381, 0.0));
    positionAlloc4->Add (Vector(382.5100347176231,122.26876602342313, 0.0));
    positionAlloc4->Add (Vector(446.1513681082252,407.1609532874569, 0.0));
    positionAlloc4->Add (Vector(386.1751857395686,55.91340860312849, 0.0));
    positionAlloc4->Add (Vector(155.1345060943357,306.92944467073653, 0.0));
    positionAlloc4->Add (Vector(448.55091490220053,193.83079907275786, 0.0));
    positionAlloc4->Add (Vector(463.8547821883641,7.893966439582734, 0.0));
    positionAlloc4->Add (Vector(18.560761109419456,171.4764250253537, 0.0));
    positionAlloc4->Add (Vector(209.1606641219451,231.92731717350674, 0.0));
    positionAlloc4->Add (Vector(404.7293039424571,193.5209677378854, 0.0));
    positionAlloc4->Add (Vector(459.0845367634244,486.7923989404355, 0.0));
    positionAlloc4->Add (Vector(304.4988325893971,67.71482830162235, 0.0));
    positionAlloc4->Add (Vector(387.65477599710044,116.48580662843882, 0.0));
    positionAlloc4->Add (Vector(201.97357667089418,415.64360864824863, 0.0));
    positionAlloc4->Add (Vector(266.1633963587174,40.87821078393294, 0.0));
    positionAlloc4->Add (Vector(238.68251783743605,74.28176752269877, 0.0));
    positionAlloc4->Add (Vector(447.6767988994924,98.38030915761253, 0.0));
    positionAlloc4->Add (Vector(61.49109602270675,138.12674871936514, 0.0));
    positionAlloc4->Add (Vector(196.39125336317326,497.17436125682707, 0.0));
    positionAlloc4->Add (Vector(482.0493946895026,461.45399569654, 0.0));
    positionAlloc4->Add (Vector(441.91343460838317,55.59021035228251, 0.0));
    positionAlloc4->Add (Vector(101.57282437803033,260.61825657364545, 0.0));
    positionAlloc4->Add (Vector(178.5055108332958,38.88693577931751, 0.0));
    positionAlloc4->Add (Vector(70.00458181307268,176.9549473828187, 0.0));
    positionAlloc4->Add (Vector(132.45931197019894,53.80358714296746, 0.0));
    positionAlloc4->Add (Vector(229.7507677087302,273.991865787602, 0.0));
    positionAlloc4->Add (Vector(389.2618925351697,293.57818402911374, 0.0));
    positionAlloc4->Add (Vector(397.8027283100575,2.228963119573879, 0.0));
    positionAlloc4->Add (Vector(76.61975655203979,221.95250662936132, 0.0));
    positionAlloc4->Add (Vector(400.8417747923014,150.8397722928896, 0.0));
    positionAlloc4->Add (Vector(20.07423181775314,381.5369945503797, 0.0));
    positionAlloc4->Add (Vector(124.79444556489311,73.51702930023124, 0.0));
    positionAlloc4->Add (Vector(255.50704599717406,430.34155317947767, 0.0));
    positionAlloc4->Add (Vector(387.1206682063046,369.7209940328606, 0.0));
    positionAlloc4->Add (Vector(0.7196192084530928,52.436662204882964, 0.0));
    positionAlloc4->Add (Vector(413.84932974565083,239.2855562008705, 0.0));
    positionAlloc4->Add (Vector(6.737011842234686,375.57377085654286, 0.0));
    positionAlloc4->Add (Vector(355.55721097278274,335.8353486017916, 0.0));
    positionAlloc4->Add (Vector(374.8206942060058,26.725756389914814, 0.0));
    positionAlloc4->Add (Vector(108.75359458557843,153.04201461916546, 0.0));
    positionAlloc4->Add (Vector(103.9452515894449,484.9986387980622, 0.0));
    positionAlloc4->Add (Vector(330.1920720768297,312.56261168363545, 0.0));
    positionAlloc4->Add (Vector(316.12992351146016,290.9942905957548, 0.0));
    positionAlloc4->Add (Vector(400.5482446921983,277.61722381049844, 0.0));
    positionAlloc4->Add (Vector(173.7012606733304,127.41409810731047, 0.0));
    positionAlloc4->Add (Vector(157.96592313600388,310.6534423288033, 0.0));
    positionAlloc4->Add (Vector(149.2928427107701,363.6440516342271, 0.0));
    positionAlloc4->Add (Vector(13.201932737087308,363.4896527498864, 0.0));
    positionAlloc4->Add (Vector(334.9724444999202,43.041725789805916, 0.0));
    positionAlloc4->Add (Vector(317.2653924309139,0.02356459220576035, 0.0));
    positionAlloc4->Add (Vector(182.29635158733143,229.4028690538326, 0.0));
    positionAlloc4->Add (Vector(25.91959604165528,122.18813290525165, 0.0));
    positionAlloc4->Add (Vector(74.29251064925141,71.4118977319082, 0.0));
    positionAlloc4->Add (Vector(232.90843575177567,27.07691123678191, 0.0));
    positionAlloc4->Add (Vector(445.357597849233,31.74426562490018, 0.0));
    positionAlloc4->Add (Vector(387.6443583415156,5.146440060681934, 0.0));
    positionAlloc4->Add (Vector(377.4524416093739,176.54032240515866, 0.0));
    positionAlloc4->Add (Vector(308.0343359900574,314.8593514728394, 0.0));
    positionAlloc4->Add (Vector(238.91163443282304,42.71719099932469, 0.0));
    positionAlloc4->Add (Vector(273.2751892355339,465.9540534241356, 0.0));
    positionAlloc4->Add (Vector(302.7043564262721,1.0543451321929354, 0.0));
    positionAlloc4->Add (Vector(486.5670211287357,167.77698630393834, 0.0));
    positionAlloc4->Add (Vector(252.36582091454102,109.4013122440402, 0.0));
    positionAlloc4->Add (Vector(381.80827937156954,217.9595312192816, 0.0));
    positionAlloc4->Add (Vector(390.5745228660994,51.78932960712102, 0.0));
    positionAlloc4->Add (Vector(111.25390884095488,256.5443049472824, 0.0));
    positionAlloc4->Add (Vector(248.16861981478345,94.25204709686302, 0.0));
    positionAlloc4->Add (Vector(88.35984862870838,436.9366673354935, 0.0));
    positionAlloc4->Add (Vector(429.63667059892435,497.3666983138365, 0.0));
    positionAlloc4->Add (Vector(131.55256598313852,348.43521095177226, 0.0));
    positionAlloc4->Add (Vector(251.8857361859569,257.57961753523847, 0.0));
    positionAlloc4->Add (Vector(468.40064533511134,455.2491562889021, 0.0));
    positionAlloc4->Add (Vector(103.55805023405684,314.07589955295333, 0.0));
    positionAlloc4->Add (Vector(411.4260355836249,301.63780425594274, 0.0));
    positionAlloc4->Add (Vector(238.7903332409476,389.45691392858913, 0.0));
    positionAlloc4->Add (Vector(320.21852140696404,228.9370176263143, 0.0));
    positionAlloc4->Add (Vector(62.98638555611535,435.027301746301, 0.0));
    positionAlloc4->Add (Vector(184.8462213297634,176.16418866824617, 0.0));
    positionAlloc4->Add (Vector(3.851294315089182,68.1222678101585, 0.0));
    positionAlloc4->Add (Vector(107.41423869359107,176.7036379260818, 0.0));
    positionAlloc4->Add (Vector(125.05743007745635,0.7905879261351223, 0.0));
    positionAlloc4->Add (Vector(440.6016267871304,174.6878113259432, 0.0));
    positionAlloc4->Add (Vector(431.95407493851235,499.1788807401988, 0.0));
    positionAlloc4->Add (Vector(357.7839111376181,428.84224118149626, 0.0));
    positionAlloc4->Add (Vector(271.18878008988816,345.85083653112076, 0.0));
    positionAlloc4->Add (Vector(319.79091437415155,98.57469651382877, 0.0));
    positionAlloc4->Add (Vector(178.96552836945943,387.72348898993624, 0.0));
    positionAlloc4->Add (Vector(128.9201713272581,451.9573251027207, 0.0));
    positionAlloc4->Add (Vector(301.2982104470961,9.262789008738226, 0.0));
    positionAlloc4->Add (Vector(72.82629917862954,236.1431673061738, 0.0));
    positionAlloc4->Add (Vector(318.18846700355056,360.7354906080134, 0.0));
    positionAlloc4->Add (Vector(104.36149667203497,123.59509653337503, 0.0));
    positionAlloc4->Add (Vector(273.893660311411,163.69127650084687, 0.0));
    positionAlloc4->Add (Vector(458.3163140569555,489.06214720160466, 0.0));
    positionAlloc4->Add (Vector(240.28951579053165,465.7217968176274, 0.0));
    positionAlloc4->Add (Vector(479.37629992129393,193.60893136447237, 0.0));
    positionAlloc4->Add (Vector(366.3359893387782,373.7952138752667, 0.0));
    positionAlloc4->Add (Vector(361.2255982712777,190.73412552133695, 0.0));
    positionAlloc4->Add (Vector(292.6798061983139,215.66081287331696, 0.0));
    positionAlloc4->Add (Vector(235.75897880742923,227.657236570235, 0.0));
    positionAlloc4->Add (Vector(127.83695855133081,321.06573872757724, 0.0));
    positionAlloc4->Add (Vector(120.00725529799861,5.892016225588881, 0.0));
    positionAlloc4->Add (Vector(435.28871370939004,445.3487916601068, 0.0));
    positionAlloc4->Add (Vector(197.58893311747005,466.1481022136645, 0.0));
    positionAlloc4->Add (Vector(50.146401337359926,410.7436347541879, 0.0));
    positionAlloc4->Add (Vector(402.02026666194365,354.4380959667606, 0.0));
    positionAlloc4->Add (Vector(209.9453836169567,121.61534807209185, 0.0));
    positionAlloc4->Add (Vector(494.9536241496843,363.76972863671386, 0.0));
    positionAlloc4->Add (Vector(142.03529690619936,327.8771958985873, 0.0));
    positionAlloc4->Add (Vector(161.13897420603297,168.4105574385959, 0.0));
    positionAlloc4->Add (Vector(254.4106831512792,480.60270690381753, 0.0));
    positionAlloc4->Add (Vector(139.15978766913594,337.09526275184385, 0.0));
    positionAlloc4->Add (Vector(12.00960717437799,61.32141468074897, 0.0));
    positionAlloc4->Add (Vector(262.80673617079265,326.2416356293306, 0.0));
    positionAlloc4->Add (Vector(229.06724255269216,473.228608762335, 0.0));
    positionAlloc4->Add (Vector(283.74817854241064,260.9386409668567, 0.0));
    positionAlloc4->Add (Vector(109.7167356333027,55.405138033733145, 0.0));
    positionAlloc4->Add (Vector(182.51262200826406,102.41257099670109, 0.0));
    positionAlloc4->Add (Vector(58.24721826625417,242.40680064242724, 0.0));
    positionAlloc4->Add (Vector(264.9439193619043,398.66899058230393, 0.0));
    positionAlloc4->Add (Vector(478.0895772906151,433.1343962092437, 0.0));
    positionAlloc4->Add (Vector(231.420340589393,412.36039331555236, 0.0));
    positionAlloc4->Add (Vector(389.0501474239837,155.90582659052242, 0.0));
    positionAlloc4->Add (Vector(109.34080214266228,196.0167387711587, 0.0));
    positionAlloc4->Add (Vector(193.6517739670615,387.62936879219194, 0.0));
    positionAlloc4->Add (Vector(0.5575521769907721,349.81447730323816, 0.0));
    positionAlloc4->Add (Vector(403.1225801397263,372.61660204438454, 0.0));
    positionAlloc4->Add (Vector(447.9991343207393,254.27151435465, 0.0));
    positionAlloc4->Add (Vector(251.35070678652156,300.173026506111, 0.0));
    positionAlloc4->Add (Vector(330.80911159962307,354.57444034587127, 0.0));
    positionAlloc4->Add (Vector(63.30804789246164,247.040640787907, 0.0));
    positionAlloc4->Add (Vector(413.2360521666834,372.48695399033295, 0.0));
    positionAlloc4->Add (Vector(465.6891482362506,229.48279897551694, 0.0));
    positionAlloc4->Add (Vector(177.98269729917877,212.6094114317385, 0.0));
    positionAlloc4->Add (Vector(374.2572631487344,495.6930640271121, 0.0));
    positionAlloc4->Add (Vector(493.7710912650954,115.73594436921303, 0.0));
    positionAlloc4->Add (Vector(332.2321695909318,208.57219798626818, 0.0));
    positionAlloc4->Add (Vector(484.98356265753154,289.11927067226463, 0.0));
    positionAlloc4->Add (Vector(261.87705545145593,282.54562688543746, 0.0));
    positionAlloc4->Add (Vector(254.58914589920983,404.49304485866645, 0.0));
    positionAlloc4->Add (Vector(343.55048994305383,10.11513150653548, 0.0));
    positionAlloc4->Add (Vector(350.7691104645109,281.47518318318356, 0.0));
    positionAlloc4->Add (Vector(291.5601138798954,263.32698420944234, 0.0));
    positionAlloc4->Add (Vector(136.35606020280727,36.78040002844535, 0.0));
    positionAlloc4->Add (Vector(242.1226643955573,79.19867143679681, 0.0));
    positionAlloc4->Add (Vector(43.64103956399873,303.798859083544, 0.0));
    positionAlloc4->Add (Vector(35.78650608801775,93.02335624012976, 0.0));
    positionAlloc4->Add (Vector(277.5161490885763,278.02328198486975, 0.0));
    positionAlloc4->Add (Vector(263.38927330155934,123.92995987059608, 0.0));
    positionAlloc4->Add (Vector(288.39894536589486,254.84405709344492, 0.0));
    positionAlloc4->Add (Vector(489.9435568196619,279.62879111642945, 0.0));
    positionAlloc4->Add (Vector(431.86387126804823,369.2251186352647, 0.0));
    positionAlloc4->Add (Vector(192.78445469464512,437.057202103057, 0.0));
    positionAlloc4->Add (Vector(161.60390940106916,365.8624131829309, 0.0));
    positionAlloc4->Add (Vector(76.67577123738334,222.74275561877172, 0.0));
    positionAlloc4->Add (Vector(157.2949148711043,205.27453014393433, 0.0));
    positionAlloc4->Add (Vector(293.78981323795847,216.47025443975298, 0.0));
    positionAlloc4->Add (Vector(5.402462101954386,280.7996139635153, 0.0));
    positionAlloc4->Add (Vector(495.0678184589698,231.94788107467613, 0.0));
    positionAlloc4->Add (Vector(349.2898389416733,223.98683599736057, 0.0));
    positionAlloc4->Add (Vector(220.45801458491687,351.1300083981125, 0.0));
    positionAlloc4->Add (Vector(353.31477377204635,428.99535873650046, 0.0));
    positionAlloc4->Add (Vector(232.42072610884546,389.7547363849125, 0.0));
    positionAlloc4->Add (Vector(150.16556835241403,271.77256927967346, 0.0));
    positionAlloc4->Add (Vector(215.43853630831933,279.4892194206642, 0.0));
    positionAlloc4->Add (Vector(154.23489978657838,16.64206280084285, 0.0));
    positionAlloc4->Add (Vector(123.85128338704027,339.504254853, 0.0));
    positionAlloc4->Add (Vector(428.32711238379954,406.5506434809638, 0.0));
    positionAlloc4->Add (Vector(36.96052925655824,222.01406880277779, 0.0));
    positionAlloc4->Add (Vector(323.90876745599405,320.4258160092176, 0.0));
    positionAlloc4->Add (Vector(374.84414316038203,90.850236711227, 0.0));
    positionAlloc4->Add (Vector(390.9309477429178,111.11812392613662, 0.0));
    positionAlloc4->Add (Vector(303.9275597596775,331.0974410612977, 0.0));
    positionAlloc4->Add (Vector(317.68545885845896,207.22591477081397, 0.0));
    positionAlloc4->Add (Vector(430.46769856866797,181.42836871915446, 0.0));
    positionAlloc4->Add (Vector(389.0239723609933,427.6696641492796, 0.0));
    positionAlloc4->Add (Vector(472.5656619076513,13.96443769266742, 0.0));
    positionAlloc4->Add (Vector(308.7910869467251,170.55746675724387, 0.0));
    positionAlloc4->Add (Vector(179.56446869271875,248.29369231293603, 0.0));
    positionAlloc4->Add (Vector(10.396235144567667,485.02856169960114, 0.0));
    positionAlloc4->Add (Vector(249.9758483481016,185.97078997699197, 0.0));
    positionAlloc4->Add (Vector(9.836750437250075,376.86354707109933, 0.0));
    positionAlloc4->Add (Vector(201.2026244196955,179.8179828700608, 0.0));
    positionAlloc4->Add (Vector(159.86935813479332,335.86628880091246, 0.0));
    positionAlloc4->Add (Vector(194.57031655000318,94.3513879483207, 0.0));
    positionAlloc4->Add (Vector(473.3384068848617,150.87759537081828, 0.0));
    positionAlloc4->Add (Vector(101.90066568711592,152.20529019078478, 0.0));
    positionAlloc4->Add (Vector(151.33602724350837,183.9897903544368, 0.0));
    positionAlloc4->Add (Vector(354.83451715019214,256.631093250915, 0.0));
    positionAlloc4->Add (Vector(365.7486939677491,260.8919231273768, 0.0));
    positionAlloc4->Add (Vector(22.63078205156388,248.13525907132512, 0.0));
    positionAlloc4->Add (Vector(125.53851915257319,255.15783172841407, 0.0));
    positionAlloc4->Add (Vector(228.4943164823777,220.22669061792578, 0.0));
    positionAlloc4->Add (Vector(373.89368116241604,426.4183113708672, 0.0));
    positionAlloc4->Add (Vector(397.9363277738871,147.28945417145889, 0.0));
    positionAlloc4->Add (Vector(214.2477689892221,331.74328180292486, 0.0));
    positionAlloc4->Add (Vector(391.8138613690967,372.43657928817663, 0.0));
    positionAlloc4->Add (Vector(247.31476364459903,57.804846539290246, 0.0));
    positionAlloc4->Add (Vector(304.32955160994743,270.60631551485886, 0.0));
    positionAlloc4->Add (Vector(143.75796811968598,488.61707628218016, 0.0));
    positionAlloc4->Add (Vector(452.43464711416175,140.75287112691188, 0.0));
    positionAlloc4->Add (Vector(34.86678709779073,406.535744763734, 0.0));
    positionAlloc4->Add (Vector(12.197535685064764,251.82390470169224, 0.0));
    positionAlloc4->Add (Vector(470.39820150794156,197.6642697966612, 0.0));
    positionAlloc4->Add (Vector(307.0152877078427,44.482186657782975, 0.0));
    positionAlloc4->Add (Vector(368.4349990651858,142.30694850940645, 0.0));
    positionAlloc4->Add (Vector(59.19794929831984,57.08745859297626, 0.0));
    positionAlloc4->Add (Vector(264.1124411049028,146.26241534255468, 0.0));
    positionAlloc4->Add (Vector(442.7502631788638,493.1442046502694, 0.0));
    positionAlloc4->Add (Vector(192.54526267541783,456.0905939538661, 0.0));
    positionAlloc4->Add (Vector(471.91126676256056,370.93276463863697, 0.0));
    positionAlloc4->Add (Vector(171.8088704600223,185.71970258062538, 0.0));
    positionAlloc4->Add (Vector(142.91717975526157,65.38291107747874, 0.0));
    positionAlloc4->Add (Vector(396.23191406198083,122.72181599002636, 0.0));
    positionAlloc4->Add (Vector(38.53232926924277,295.50341938751376, 0.0));
    positionAlloc4->Add (Vector(367.1985186009185,316.9669093052375, 0.0));
    positionAlloc4->Add (Vector(14.049367524199441,11.875560747522051, 0.0));
    positionAlloc4->Add (Vector(85.97876745671174,355.73582297600115, 0.0));
    positionAlloc4->Add (Vector(113.68156284197129,53.02832814949498, 0.0));
    positionAlloc4->Add (Vector(434.40596007763725,332.47780615785973, 0.0));
    positionAlloc4->Add (Vector(108.92477647486187,423.3729666004807, 0.0));
    positionAlloc4->Add (Vector(382.7091394089856,259.47424592713566, 0.0));
    positionAlloc4->Add (Vector(144.98258228227283,346.0492344577852, 0.0));
    positionAlloc4->Add (Vector(490.62840907703156,225.6117399134877, 0.0));
    positionAlloc4->Add (Vector(273.09678016410476,127.03153022258756, 0.0));
    positionAlloc4->Add (Vector(158.72542941029877,293.82106845238604, 0.0));
    positionAlloc4->Add (Vector(432.36358393140813,286.43398980777795, 0.0));
    positionAlloc4->Add (Vector(142.82370696820746,79.7075786781764, 0.0));
    positionAlloc4->Add (Vector(16.993357318730805,77.80338267503511, 0.0));
    positionAlloc4->Add (Vector(169.70875866552214,224.75184728509916, 0.0));
    positionAlloc4->Add (Vector(418.85122056901895,181.09292469890838, 0.0));
    positionAlloc4->Add (Vector(343.21826218688807,498.5947994371406, 0.0));
    positionAlloc4->Add (Vector(436.1745524273388,46.193770729681674, 0.0));
    positionAlloc4->Add (Vector(133.60312165043558,339.32104045866566, 0.0));
    positionAlloc4->Add (Vector(468.49659744148977,29.523254467802573, 0.0));
    positionAlloc4->Add (Vector(478.0801241577055,14.094371609848888, 0.0));
    positionAlloc4->Add (Vector(227.1757438763987,78.03115185608462, 0.0));
    positionAlloc4->Add (Vector(52.24727860241002,4.9731635798958, 0.0));
    positionAlloc4->Add (Vector(55.435616058719326,478.96871053816045, 0.0));
    positionAlloc4->Add (Vector(465.2101677613279,227.68326750819483, 0.0));
    positionAlloc4->Add (Vector(463.2350921266966,325.86782831054654, 0.0));
    positionAlloc4->Add (Vector(35.785983433097854,371.0232528238417, 0.0));
    positionAlloc4->Add (Vector(248.80107002233632,485.55513670847427, 0.0));
    positionAlloc4->Add (Vector(130.54759987796112,301.78862562775333, 0.0));
    positionAlloc4->Add (Vector(440.9717252240056,368.91633056602024, 0.0));
    positionAlloc4->Add (Vector(271.8156724344435,463.95463838947984, 0.0));
    positionAlloc4->Add (Vector(449.51368937050296,313.3186490763102, 0.0));
    positionAlloc4->Add (Vector(42.0192265656229,104.11181815095233, 0.0));
    positionAlloc4->Add (Vector(361.2275064615328,124.88566198683215, 0.0));
    positionAlloc4->Add (Vector(301.1935269833064,226.57874168443044, 0.0));
    positionAlloc4->Add (Vector(262.65249840707907,183.10417447490667, 0.0));
    positionAlloc4->Add (Vector(219.56655300183974,151.9366153698486, 0.0));
    positionAlloc4->Add (Vector(377.9965478029308,281.31724605858494, 0.0));
    positionAlloc4->Add (Vector(464.8987641728594,78.51697501426858, 0.0));
    positionAlloc4->Add (Vector(466.4571420643704,435.5342385706111, 0.0));
    positionAlloc4->Add (Vector(420.78192767391386,65.10558000961485, 0.0));
    positionAlloc4->Add (Vector(318.30580259795704,99.39914012015188, 0.0));
    positionAlloc4->Add (Vector(192.13969651021355,284.0524779908342, 0.0));
    positionAlloc4->Add (Vector(53.76049244019493,30.543734971564984, 0.0));
    positionAlloc4->Add (Vector(335.29168936914186,422.6684524488166, 0.0));
    positionAlloc4->Add (Vector(56.50774157786592,108.20372464602657, 0.0));
    positionAlloc4->Add (Vector(342.29875165203043,138.49739519091008, 0.0));
    positionAlloc4->Add (Vector(14.338727286009046,353.6853672090066, 0.0));
    positionAlloc4->Add (Vector(97.33495236940021,344.0250336859596, 0.0));
    positionAlloc4->Add (Vector(317.7479305884687,77.67852780856744, 0.0));
    positionAlloc4->Add (Vector(346.99202577543036,219.58637019794836, 0.0));
    positionAlloc4->Add (Vector(322.5253217085926,437.65539623769376, 0.0));
    positionAlloc4->Add (Vector(237.14932044933772,462.3586988169031, 0.0));
    positionAlloc4->Add (Vector(265.67363186184633,277.9225062658885, 0.0));
    positionAlloc4->Add (Vector(307.00851121229283,402.8885787882784, 0.0));
    positionAlloc4->Add (Vector(223.56864499342788,294.2843151887007, 0.0));
    positionAlloc4->Add (Vector(467.4162234706866,263.9058496803066, 0.0));
    positionAlloc4->Add (Vector(257.3923602283387,191.61611628931186, 0.0));
    positionAlloc4->Add (Vector(244.28093593296524,129.4283739719319, 0.0));
    positionAlloc4->Add (Vector(11.053621698791204,184.69124059995534, 0.0));
    positionAlloc4->Add (Vector(296.98111116577735,392.05079697716684, 0.0));
    positionAlloc4->Add (Vector(267.0571369418836,403.675439223922, 0.0));
    positionAlloc4->Add (Vector(124.967453923501,192.94914226350224, 0.0));
    positionAlloc4->Add (Vector(133.98506233044466,466.98646071564156, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_15.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_ECC_flowmon/switch_ECC_15.flowmon", false, false);
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