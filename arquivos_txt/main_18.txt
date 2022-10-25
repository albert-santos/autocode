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
	  uint16_t numberOfRrhs = 42;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 712;
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
    positionAlloc->Add (Vector (356.6666666666667,270.0, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,140.0, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,10.0, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,140.0, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (270.0,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (270.0,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (270.0,140.0, 0.0));
    positionAlloc->Add (Vector (270.0,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,140.0, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,270.0, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,140.0, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,10.0, 0.0));
    positionAlloc->Add (Vector (140.0,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (140.0,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (140.0,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (140.0,140.0, 0.0));
    positionAlloc->Add (Vector (140.0,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,270.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,140.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (10.0,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (10.0,140.0, 0.0));
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
    positionAlloc4->Add (Vector(352.6961527997878,80.20080832774465, 0.0));
    positionAlloc4->Add (Vector(7.369758437037666,9.805295394643876, 0.0));
    positionAlloc4->Add (Vector(41.830612619324725,354.392145430743, 0.0));
    positionAlloc4->Add (Vector(135.28978927453755,328.6650529156281, 0.0));
    positionAlloc4->Add (Vector(235.1221937086633,159.21445794401038, 0.0));
    positionAlloc4->Add (Vector(50.67742701947662,364.9831460945227, 0.0));
    positionAlloc4->Add (Vector(131.98189025034605,321.6555045112704, 0.0));
    positionAlloc4->Add (Vector(370.3685864669985,362.70191724039387, 0.0));
    positionAlloc4->Add (Vector(204.8927965897263,259.4115480245294, 0.0));
    positionAlloc4->Add (Vector(366.8210628195707,166.53064318765885, 0.0));
    positionAlloc4->Add (Vector(101.91337603694537,357.67484401073995, 0.0));
    positionAlloc4->Add (Vector(114.37273576367373,271.1669930649368, 0.0));
    positionAlloc4->Add (Vector(288.3140543174475,204.19686408523526, 0.0));
    positionAlloc4->Add (Vector(267.81127278502856,39.726390535590994, 0.0));
    positionAlloc4->Add (Vector(87.20141078424115,396.6088080722495, 0.0));
    positionAlloc4->Add (Vector(205.64347885934274,96.01419178596716, 0.0));
    positionAlloc4->Add (Vector(167.62778403115033,23.389992736947598, 0.0));
    positionAlloc4->Add (Vector(92.19523805895649,388.08620599915605, 0.0));
    positionAlloc4->Add (Vector(175.6613065057246,68.50852590432201, 0.0));
    positionAlloc4->Add (Vector(57.390944026682874,47.433070002580195, 0.0));
    positionAlloc4->Add (Vector(297.5725446959651,122.18377503574102, 0.0));
    positionAlloc4->Add (Vector(208.35068365486785,311.81462630789224, 0.0));
    positionAlloc4->Add (Vector(184.97472468232408,287.36029852721686, 0.0));
    positionAlloc4->Add (Vector(49.7422854674928,69.08128866143394, 0.0));
    positionAlloc4->Add (Vector(380.6690328368761,222.5071483153372, 0.0));
    positionAlloc4->Add (Vector(145.10331553354695,151.1494069351197, 0.0));
    positionAlloc4->Add (Vector(398.9257390226761,343.8883695744057, 0.0));
    positionAlloc4->Add (Vector(191.36019321096396,343.80953628792645, 0.0));
    positionAlloc4->Add (Vector(397.7199440038052,319.1048721950366, 0.0));
    positionAlloc4->Add (Vector(200.22752461434305,19.324964977103633, 0.0));
    positionAlloc4->Add (Vector(123.48284503361909,233.2271589247033, 0.0));
    positionAlloc4->Add (Vector(187.01374238504854,143.79412254678604, 0.0));
    positionAlloc4->Add (Vector(343.11091089437537,26.50375560377536, 0.0));
    positionAlloc4->Add (Vector(129.25243915518553,348.78334071308603, 0.0));
    positionAlloc4->Add (Vector(41.859091555880525,105.12651198186744, 0.0));
    positionAlloc4->Add (Vector(285.36871793514064,338.4854851272971, 0.0));
    positionAlloc4->Add (Vector(244.0050180160973,97.71177053453654, 0.0));
    positionAlloc4->Add (Vector(302.9336301261562,47.74582360496606, 0.0));
    positionAlloc4->Add (Vector(290.302903341245,388.29000757913093, 0.0));
    positionAlloc4->Add (Vector(217.11004146848595,96.50298883735316, 0.0));
    positionAlloc4->Add (Vector(375.3352701609331,21.62420802915319, 0.0));
    positionAlloc4->Add (Vector(368.7570503372306,307.3639813747029, 0.0));
    positionAlloc4->Add (Vector(15.015181073807948,116.77738156575485, 0.0));
    positionAlloc4->Add (Vector(26.958193114527784,349.2783752043332, 0.0));
    positionAlloc4->Add (Vector(56.980232292804885,160.75362587181812, 0.0));
    positionAlloc4->Add (Vector(135.06375302120776,145.63478986824677, 0.0));
    positionAlloc4->Add (Vector(169.1237053178544,255.6775524043347, 0.0));
    positionAlloc4->Add (Vector(374.4241712391474,90.09881967422358, 0.0));
    positionAlloc4->Add (Vector(163.09886837377218,174.70656595818704, 0.0));
    positionAlloc4->Add (Vector(261.84076700488947,325.39507182444976, 0.0));
    positionAlloc4->Add (Vector(316.81902279683857,80.8693211405818, 0.0));
    positionAlloc4->Add (Vector(209.39007923749529,325.83541199359763, 0.0));
    positionAlloc4->Add (Vector(304.3412393767577,123.22060458937374, 0.0));
    positionAlloc4->Add (Vector(293.08547259481503,129.30354859409928, 0.0));
    positionAlloc4->Add (Vector(346.17096007536577,307.4545050840276, 0.0));
    positionAlloc4->Add (Vector(133.8939628812959,299.9617855665685, 0.0));
    positionAlloc4->Add (Vector(220.3623852577299,11.193405644968335, 0.0));
    positionAlloc4->Add (Vector(225.86592749470213,358.40919981133806, 0.0));
    positionAlloc4->Add (Vector(326.09680575639436,230.22006828199042, 0.0));
    positionAlloc4->Add (Vector(215.1818838226071,290.1025758137623, 0.0));
    positionAlloc4->Add (Vector(240.2439552121327,322.12125944880444, 0.0));
    positionAlloc4->Add (Vector(17.54112511336703,210.71612273020227, 0.0));
    positionAlloc4->Add (Vector(341.3958661180516,230.64378748320374, 0.0));
    positionAlloc4->Add (Vector(380.5739730973827,311.00769495007427, 0.0));
    positionAlloc4->Add (Vector(78.84415498860685,298.37664978628334, 0.0));
    positionAlloc4->Add (Vector(154.14462847859758,262.5877049948199, 0.0));
    positionAlloc4->Add (Vector(101.56714092555994,39.892250172063505, 0.0));
    positionAlloc4->Add (Vector(38.765367144494654,24.16805408350853, 0.0));
    positionAlloc4->Add (Vector(206.62006927520787,62.37827763688677, 0.0));
    positionAlloc4->Add (Vector(148.83416333436602,340.3151746931546, 0.0));
    positionAlloc4->Add (Vector(101.65177210409841,224.3125978249479, 0.0));
    positionAlloc4->Add (Vector(118.60006203880657,339.45530935516706, 0.0));
    positionAlloc4->Add (Vector(96.01599113820623,31.372322811197726, 0.0));
    positionAlloc4->Add (Vector(352.7063881970292,11.096795928163816, 0.0));
    positionAlloc4->Add (Vector(1.007066229280662,225.30674150055933, 0.0));
    positionAlloc4->Add (Vector(341.6941757623302,232.03397275629135, 0.0));
    positionAlloc4->Add (Vector(334.76097966255327,235.31936050852704, 0.0));
    positionAlloc4->Add (Vector(94.9635828674257,0.9367612318466279, 0.0));
    positionAlloc4->Add (Vector(4.969336976294292,146.6275303495138, 0.0));
    positionAlloc4->Add (Vector(162.8254150743246,177.7119134512797, 0.0));
    positionAlloc4->Add (Vector(228.10386205279843,248.61187999568304, 0.0));
    positionAlloc4->Add (Vector(155.68157859269593,81.89790834155932, 0.0));
    positionAlloc4->Add (Vector(63.175176722686686,125.18055652753581, 0.0));
    positionAlloc4->Add (Vector(347.0106060552722,99.72007804091945, 0.0));
    positionAlloc4->Add (Vector(226.79153639718632,306.4310762743164, 0.0));
    positionAlloc4->Add (Vector(203.5878543611285,287.7226636720205, 0.0));
    positionAlloc4->Add (Vector(155.187031389557,238.31109914864362, 0.0));
    positionAlloc4->Add (Vector(241.39933004712594,138.6290629781303, 0.0));
    positionAlloc4->Add (Vector(32.96442961854815,246.64555561582299, 0.0));
    positionAlloc4->Add (Vector(43.867302565379205,63.01511394111854, 0.0));
    positionAlloc4->Add (Vector(392.4688397493069,137.17385930486307, 0.0));
    positionAlloc4->Add (Vector(320.9469498104324,161.51656063303776, 0.0));
    positionAlloc4->Add (Vector(123.83637043111797,264.99923094234646, 0.0));
    positionAlloc4->Add (Vector(111.52413665344207,384.3558948694539, 0.0));
    positionAlloc4->Add (Vector(345.225782000035,392.0333965700232, 0.0));
    positionAlloc4->Add (Vector(169.82363046466008,101.31873104380227, 0.0));
    positionAlloc4->Add (Vector(351.3616401716256,259.9570790157215, 0.0));
    positionAlloc4->Add (Vector(363.2915878017607,8.593932575466079, 0.0));
    positionAlloc4->Add (Vector(178.21152463931136,187.98465413726726, 0.0));
    positionAlloc4->Add (Vector(101.92631365670057,140.10419189930053, 0.0));
    positionAlloc4->Add (Vector(314.8904231023816,89.62333518049151, 0.0));
    positionAlloc4->Add (Vector(262.049275769333,132.68317680831908, 0.0));
    positionAlloc4->Add (Vector(266.7841415075883,317.64733398674593, 0.0));
    positionAlloc4->Add (Vector(165.69952549073315,221.90097118393166, 0.0));
    positionAlloc4->Add (Vector(40.932938292012324,32.469195623318384, 0.0));
    positionAlloc4->Add (Vector(398.9897033844863,324.5883493907185, 0.0));
    positionAlloc4->Add (Vector(217.54616288776947,371.90686056762826, 0.0));
    positionAlloc4->Add (Vector(396.37285479488685,271.9283783943201, 0.0));
    positionAlloc4->Add (Vector(144.85459908014232,38.80081070045031, 0.0));
    positionAlloc4->Add (Vector(71.04358785877322,67.27943409584421, 0.0));
    positionAlloc4->Add (Vector(151.73082891207122,366.7452120408072, 0.0));
    positionAlloc4->Add (Vector(390.1103226806852,391.1142988207311, 0.0));
    positionAlloc4->Add (Vector(229.43547013242784,352.1568303081002, 0.0));
    positionAlloc4->Add (Vector(373.2225560831025,26.707498916339432, 0.0));
    positionAlloc4->Add (Vector(119.6132066260259,365.33507919135826, 0.0));
    positionAlloc4->Add (Vector(283.3293905056549,198.59719582560777, 0.0));
    positionAlloc4->Add (Vector(97.51288208822126,292.97566830079955, 0.0));
    positionAlloc4->Add (Vector(304.6023657100358,372.10607820520266, 0.0));
    positionAlloc4->Add (Vector(6.712590800695706,214.5914844354768, 0.0));
    positionAlloc4->Add (Vector(197.02131974157626,62.01689343029342, 0.0));
    positionAlloc4->Add (Vector(378.1925685118815,135.4381627465759, 0.0));
    positionAlloc4->Add (Vector(223.4553763654525,221.10238623022508, 0.0));
    positionAlloc4->Add (Vector(294.3049847642983,227.5577158002905, 0.0));
    positionAlloc4->Add (Vector(285.1914357652396,234.43507375947715, 0.0));
    positionAlloc4->Add (Vector(177.52597562065836,319.556958487935, 0.0));
    positionAlloc4->Add (Vector(39.220007958610296,209.23870639238768, 0.0));
    positionAlloc4->Add (Vector(213.305652055009,366.01970214308733, 0.0));
    positionAlloc4->Add (Vector(151.6762589807223,313.33424659916585, 0.0));
    positionAlloc4->Add (Vector(7.034201035943788,276.96461849279814, 0.0));
    positionAlloc4->Add (Vector(139.89912951714336,292.69941367763516, 0.0));
    positionAlloc4->Add (Vector(82.5206374874174,326.98078256723215, 0.0));
    positionAlloc4->Add (Vector(101.35608244742605,371.91645923895715, 0.0));
    positionAlloc4->Add (Vector(260.5533377050024,42.12885259188286, 0.0));
    positionAlloc4->Add (Vector(181.37230131237402,133.1197632226921, 0.0));
    positionAlloc4->Add (Vector(271.59536727952366,213.0242109541193, 0.0));
    positionAlloc4->Add (Vector(97.19618405613062,376.4084249434314, 0.0));
    positionAlloc4->Add (Vector(158.53413035479758,391.75298374069274, 0.0));
    positionAlloc4->Add (Vector(256.04414895438043,304.57877610763006, 0.0));
    positionAlloc4->Add (Vector(128.88062233950478,363.61663898436836, 0.0));
    positionAlloc4->Add (Vector(187.82018411415558,208.8103092326794, 0.0));
    positionAlloc4->Add (Vector(31.302556628806634,144.86180560932996, 0.0));
    positionAlloc4->Add (Vector(45.33116588336901,79.42456206198072, 0.0));
    positionAlloc4->Add (Vector(306.6834044818557,378.7417935972844, 0.0));
    positionAlloc4->Add (Vector(306.36789267370256,290.80583993233466, 0.0));
    positionAlloc4->Add (Vector(291.50811216330413,43.65993611928989, 0.0));
    positionAlloc4->Add (Vector(345.2732645059922,134.81529381678382, 0.0));
    positionAlloc4->Add (Vector(199.65617498553536,157.95013265889096, 0.0));
    positionAlloc4->Add (Vector(393.9477942059988,233.50416309566592, 0.0));
    positionAlloc4->Add (Vector(386.26260807845887,199.63145851899515, 0.0));
    positionAlloc4->Add (Vector(222.10042548943827,40.9075222952648, 0.0));
    positionAlloc4->Add (Vector(361.3831094768897,6.102906528251317, 0.0));
    positionAlloc4->Add (Vector(247.74663325946514,36.702288000518294, 0.0));
    positionAlloc4->Add (Vector(272.1804767448766,180.47131073277166, 0.0));
    positionAlloc4->Add (Vector(339.44795886320026,152.76005836776193, 0.0));
    positionAlloc4->Add (Vector(393.590719340805,216.64888790097922, 0.0));
    positionAlloc4->Add (Vector(35.714299451339485,346.7426742041829, 0.0));
    positionAlloc4->Add (Vector(303.7807871487429,370.61473025053, 0.0));
    positionAlloc4->Add (Vector(320.9317363546036,382.4949560087614, 0.0));
    positionAlloc4->Add (Vector(56.3470054565971,125.51201496739704, 0.0));
    positionAlloc4->Add (Vector(344.6864118216109,397.51291474363, 0.0));
    positionAlloc4->Add (Vector(117.97797226142772,73.61657082856196, 0.0));
    positionAlloc4->Add (Vector(303.09981371966126,165.82397839798634, 0.0));
    positionAlloc4->Add (Vector(155.40752547395132,142.76634819452548, 0.0));
    positionAlloc4->Add (Vector(260.66308716443945,89.23535081386107, 0.0));
    positionAlloc4->Add (Vector(254.0264697388934,142.16435751288432, 0.0));
    positionAlloc4->Add (Vector(273.72742478666595,230.24963757393152, 0.0));
    positionAlloc4->Add (Vector(204.26379353156327,54.922950451166706, 0.0));
    positionAlloc4->Add (Vector(110.99541419176116,193.28188200385966, 0.0));
    positionAlloc4->Add (Vector(387.6693418277353,270.750733574213, 0.0));
    positionAlloc4->Add (Vector(202.91681291528045,257.4171219163486, 0.0));
    positionAlloc4->Add (Vector(87.69038897540615,99.61680485605591, 0.0));
    positionAlloc4->Add (Vector(235.20644929027856,155.68797265232703, 0.0));
    positionAlloc4->Add (Vector(88.24854737030394,15.545280224393299, 0.0));
    positionAlloc4->Add (Vector(269.6941968501309,161.96376888018827, 0.0));
    positionAlloc4->Add (Vector(222.69927931301817,215.09164696844346, 0.0));
    positionAlloc4->Add (Vector(208.36928180745548,271.68771611803425, 0.0));
    positionAlloc4->Add (Vector(211.09923069997723,18.165808799794682, 0.0));
    positionAlloc4->Add (Vector(378.742683510774,229.74253475823696, 0.0));
    positionAlloc4->Add (Vector(72.30661771998777,384.303138567816, 0.0));
    positionAlloc4->Add (Vector(360.1990005492279,276.8340528584987, 0.0));
    positionAlloc4->Add (Vector(51.93266710243458,286.09148772150877, 0.0));
    positionAlloc4->Add (Vector(243.4594884812801,109.84278854641873, 0.0));
    positionAlloc4->Add (Vector(206.38995750364342,79.56087277848466, 0.0));
    positionAlloc4->Add (Vector(49.90602753586933,331.0887932391094, 0.0));
    positionAlloc4->Add (Vector(7.109933017796211,70.23784523656627, 0.0));
    positionAlloc4->Add (Vector(264.6450610383102,284.0197188516705, 0.0));
    positionAlloc4->Add (Vector(7.356982563297665,374.252124662915, 0.0));
    positionAlloc4->Add (Vector(309.6789394400069,263.78882170987407, 0.0));
    positionAlloc4->Add (Vector(390.4824052023917,332.13274062787264, 0.0));
    positionAlloc4->Add (Vector(329.18711500257825,36.4829183427053, 0.0));
    positionAlloc4->Add (Vector(298.1197926908033,94.04120941471463, 0.0));
    positionAlloc4->Add (Vector(275.50554516214174,331.4981067005949, 0.0));
    positionAlloc4->Add (Vector(353.7562255066508,177.76649247145443, 0.0));
    positionAlloc4->Add (Vector(14.262448793899063,156.71029950864775, 0.0));
    positionAlloc4->Add (Vector(233.64940063139255,11.909848084664754, 0.0));
    positionAlloc4->Add (Vector(188.9455718742784,175.50773497384165, 0.0));
    positionAlloc4->Add (Vector(190.266900991326,325.66844182441787, 0.0));
    positionAlloc4->Add (Vector(280.9969938059943,321.51849383205433, 0.0));
    positionAlloc4->Add (Vector(315.50137971976045,86.94487012107363, 0.0));
    positionAlloc4->Add (Vector(133.79192196017283,55.085038106433345, 0.0));
    positionAlloc4->Add (Vector(79.1989354283054,307.8283585790683, 0.0));
    positionAlloc4->Add (Vector(83.59604803847667,206.50837810069075, 0.0));
    positionAlloc4->Add (Vector(257.00824530126863,153.04571639355348, 0.0));
    positionAlloc4->Add (Vector(98.54407206945753,39.25965947897821, 0.0));
    positionAlloc4->Add (Vector(100.0870078140033,42.12298364580609, 0.0));
    positionAlloc4->Add (Vector(94.3066006215794,32.390595581842874, 0.0));
    positionAlloc4->Add (Vector(11.364230586227864,345.96771037679594, 0.0));
    positionAlloc4->Add (Vector(323.49667285998095,299.4500315879224, 0.0));
    positionAlloc4->Add (Vector(172.8628519653635,268.0359147669793, 0.0));
    positionAlloc4->Add (Vector(256.1706123347877,62.63079253734336, 0.0));
    positionAlloc4->Add (Vector(84.8052520943667,92.35171823113535, 0.0));
    positionAlloc4->Add (Vector(278.8495767053553,331.1982076164132, 0.0));
    positionAlloc4->Add (Vector(94.27900976006786,285.5413897733856, 0.0));
    positionAlloc4->Add (Vector(283.8480463679441,146.64010373880245, 0.0));
    positionAlloc4->Add (Vector(377.4919351530891,157.66134010893262, 0.0));
    positionAlloc4->Add (Vector(143.52369912283262,55.22696662168243, 0.0));
    positionAlloc4->Add (Vector(350.0705482193511,205.80445772848628, 0.0));
    positionAlloc4->Add (Vector(171.52248681162027,214.0148212888204, 0.0));
    positionAlloc4->Add (Vector(63.58608314570766,53.765615313465666, 0.0));
    positionAlloc4->Add (Vector(372.118390887037,372.4162051551093, 0.0));
    positionAlloc4->Add (Vector(253.17306046379414,247.04881239378312, 0.0));
    positionAlloc4->Add (Vector(272.4351601155852,370.83697450603046, 0.0));
    positionAlloc4->Add (Vector(73.1711999711314,141.51158906767085, 0.0));
    positionAlloc4->Add (Vector(197.18892247049448,55.69674338558319, 0.0));
    positionAlloc4->Add (Vector(253.85126831208913,279.8058427440831, 0.0));
    positionAlloc4->Add (Vector(329.8388512052877,279.0698719636437, 0.0));
    positionAlloc4->Add (Vector(185.37365962934612,252.70629039010947, 0.0));
    positionAlloc4->Add (Vector(90.11477615394887,14.821278144314354, 0.0));
    positionAlloc4->Add (Vector(6.602017560789797,376.1402456671981, 0.0));
    positionAlloc4->Add (Vector(177.7964108255262,205.52963526800903, 0.0));
    positionAlloc4->Add (Vector(220.9800275668925,39.68148338467259, 0.0));
    positionAlloc4->Add (Vector(50.32381510915971,255.59777036662533, 0.0));
    positionAlloc4->Add (Vector(235.01324603309732,389.60159233174176, 0.0));
    positionAlloc4->Add (Vector(104.89509373796065,337.86698360323976, 0.0));
    positionAlloc4->Add (Vector(75.53877780895873,109.22469150075456, 0.0));
    positionAlloc4->Add (Vector(80.24301184128379,55.06263102484796, 0.0));
    positionAlloc4->Add (Vector(391.8843352646753,28.556883361696617, 0.0));
    positionAlloc4->Add (Vector(325.89366419762354,174.2545871224513, 0.0));
    positionAlloc4->Add (Vector(108.2768694283871,61.479741219012496, 0.0));
    positionAlloc4->Add (Vector(104.43587040907883,113.50942541130955, 0.0));
    positionAlloc4->Add (Vector(91.73367158492044,268.2721351924632, 0.0));
    positionAlloc4->Add (Vector(48.06650856671233,47.969906454735465, 0.0));
    positionAlloc4->Add (Vector(331.6225876214593,45.258266858529026, 0.0));
    positionAlloc4->Add (Vector(237.41271514757295,240.61332686902074, 0.0));
    positionAlloc4->Add (Vector(174.83667816752703,238.67538254142957, 0.0));
    positionAlloc4->Add (Vector(7.755980790237738,75.53742194570923, 0.0));
    positionAlloc4->Add (Vector(344.7629638698771,43.296341041973996, 0.0));
    positionAlloc4->Add (Vector(160.37142531473512,119.27006388844256, 0.0));
    positionAlloc4->Add (Vector(62.44262181845181,44.30387256663582, 0.0));
    positionAlloc4->Add (Vector(224.54419300438423,231.5927427378045, 0.0));
    positionAlloc4->Add (Vector(35.99711103071242,381.0256191486544, 0.0));
    positionAlloc4->Add (Vector(28.32910756541129,147.39835908337727, 0.0));
    positionAlloc4->Add (Vector(7.408981195526687,397.4332447240892, 0.0));
    positionAlloc4->Add (Vector(140.0882086626808,222.54200251491562, 0.0));
    positionAlloc4->Add (Vector(399.12097286213645,364.8500557300992, 0.0));
    positionAlloc4->Add (Vector(7.8984078769210875,101.93489143215272, 0.0));
    positionAlloc4->Add (Vector(191.5537819865962,203.07030893430453, 0.0));
    positionAlloc4->Add (Vector(270.69375209384737,94.39525132311708, 0.0));
    positionAlloc4->Add (Vector(13.529431317545004,26.867488580569308, 0.0));
    positionAlloc4->Add (Vector(68.4777983502212,49.20930022665413, 0.0));
    positionAlloc4->Add (Vector(101.42419100172417,26.369883333231357, 0.0));
    positionAlloc4->Add (Vector(276.24055550732106,29.977513223041985, 0.0));
    positionAlloc4->Add (Vector(93.66243558105415,192.08925691099586, 0.0));
    positionAlloc4->Add (Vector(23.279362592366624,128.88471250555972, 0.0));
    positionAlloc4->Add (Vector(161.244342964194,239.08745489017963, 0.0));
    positionAlloc4->Add (Vector(203.75898551047004,180.83676170655858, 0.0));
    positionAlloc4->Add (Vector(283.32041213592817,118.26454973184748, 0.0));
    positionAlloc4->Add (Vector(116.02408225191469,332.3917145822242, 0.0));
    positionAlloc4->Add (Vector(276.16560281777083,325.2038346759452, 0.0));
    positionAlloc4->Add (Vector(379.0980377348669,72.3061191657444, 0.0));
    positionAlloc4->Add (Vector(242.36783192913714,183.11192931492957, 0.0));
    positionAlloc4->Add (Vector(146.29561377530246,73.61555639647719, 0.0));
    positionAlloc4->Add (Vector(107.0755934737107,105.16381502140524, 0.0));
    positionAlloc4->Add (Vector(0.35685178987612254,68.54764803340557, 0.0));
    positionAlloc4->Add (Vector(162.06419221289684,239.79732547421725, 0.0));
    positionAlloc4->Add (Vector(394.3835607878418,375.2146633718307, 0.0));
    positionAlloc4->Add (Vector(342.3523721394709,207.24922331982233, 0.0));
    positionAlloc4->Add (Vector(180.98634823795288,137.5938172355419, 0.0));
    positionAlloc4->Add (Vector(399.4223951364747,70.60355964461502, 0.0));
    positionAlloc4->Add (Vector(167.9492239716647,57.5474656289916, 0.0));
    positionAlloc4->Add (Vector(265.300852934863,129.60265350823158, 0.0));
    positionAlloc4->Add (Vector(38.02801828016777,175.3189521726046, 0.0));
    positionAlloc4->Add (Vector(270.6145292153144,387.9753767677464, 0.0));
    positionAlloc4->Add (Vector(264.17738680175785,340.5868435335239, 0.0));
    positionAlloc4->Add (Vector(97.37932458014909,113.99790214935166, 0.0));
    positionAlloc4->Add (Vector(293.9708058009783,94.43910763358501, 0.0));
    positionAlloc4->Add (Vector(213.76958541851855,369.73656868001115, 0.0));
    positionAlloc4->Add (Vector(192.94947120017073,231.814835519693, 0.0));
    positionAlloc4->Add (Vector(299.02395636898706,122.72095141182966, 0.0));
    positionAlloc4->Add (Vector(147.77601188468256,140.90484142142282, 0.0));
    positionAlloc4->Add (Vector(244.41894907337024,143.82831550229528, 0.0));
    positionAlloc4->Add (Vector(73.74523164887567,213.65612039698826, 0.0));
    positionAlloc4->Add (Vector(339.22515507488066,298.5472892719595, 0.0));
    positionAlloc4->Add (Vector(250.02375725726912,169.80066694237067, 0.0));
    positionAlloc4->Add (Vector(64.33076280773902,7.172550435901215, 0.0));
    positionAlloc4->Add (Vector(187.05868165071533,336.6991039184509, 0.0));
    positionAlloc4->Add (Vector(396.15007342737835,339.5697354895027, 0.0));
    positionAlloc4->Add (Vector(197.37301151699128,347.8364283401093, 0.0));
    positionAlloc4->Add (Vector(83.72948754432076,238.40587915420434, 0.0));
    positionAlloc4->Add (Vector(57.43352888602766,260.49205724637926, 0.0));
    positionAlloc4->Add (Vector(199.2633270240208,374.1900461040545, 0.0));
    positionAlloc4->Add (Vector(9.989517740411635,332.4439127305541, 0.0));
    positionAlloc4->Add (Vector(293.80369037788444,399.0293231625892, 0.0));
    positionAlloc4->Add (Vector(258.10273952290805,51.984617939978726, 0.0));
    positionAlloc4->Add (Vector(370.70656108104345,310.0475306997531, 0.0));
    positionAlloc4->Add (Vector(163.6385267220164,246.8224556194201, 0.0));
    positionAlloc4->Add (Vector(294.685446837982,392.4452103991287, 0.0));
    positionAlloc4->Add (Vector(15.514250634142712,329.9054734319848, 0.0));
    positionAlloc4->Add (Vector(247.27933460860808,394.39770282722793, 0.0));
    positionAlloc4->Add (Vector(136.93850320402,282.1871315049134, 0.0));
    positionAlloc4->Add (Vector(170.5316733590696,179.89907120127654, 0.0));
    positionAlloc4->Add (Vector(260.8879508884979,23.407504135640433, 0.0));
    positionAlloc4->Add (Vector(73.20141416963767,264.0899551391224, 0.0));
    positionAlloc4->Add (Vector(162.5339883036885,61.34842797011393, 0.0));
    positionAlloc4->Add (Vector(27.765255308871684,335.8300010907345, 0.0));
    positionAlloc4->Add (Vector(156.85705167212726,223.8788863623988, 0.0));
    positionAlloc4->Add (Vector(12.949535424359304,299.2390056115102, 0.0));
    positionAlloc4->Add (Vector(202.82848321158568,61.68806397724529, 0.0));
    positionAlloc4->Add (Vector(254.96893591212415,64.16238055937642, 0.0));
    positionAlloc4->Add (Vector(256.0247616599661,239.12424761369104, 0.0));
    positionAlloc4->Add (Vector(17.009903501937316,145.9159081552977, 0.0));
    positionAlloc4->Add (Vector(24.91259545064186,384.5457821431833, 0.0));
    positionAlloc4->Add (Vector(122.3608367113556,325.27448145671553, 0.0));
    positionAlloc4->Add (Vector(304.42463765686733,20.491637263744877, 0.0));
    positionAlloc4->Add (Vector(235.07331862786742,322.2883739604365, 0.0));
    positionAlloc4->Add (Vector(294.0908715193377,142.91602962415843, 0.0));
    positionAlloc4->Add (Vector(120.55846000678537,12.270119909974353, 0.0));
    positionAlloc4->Add (Vector(259.32560556807306,94.04041683223494, 0.0));
    positionAlloc4->Add (Vector(108.18146380347314,65.80996728240791, 0.0));
    positionAlloc4->Add (Vector(151.47392642491533,326.99412273005345, 0.0));
    positionAlloc4->Add (Vector(47.95663511403281,3.265220715644679, 0.0));
    positionAlloc4->Add (Vector(375.21548153247903,247.9119174824204, 0.0));
    positionAlloc4->Add (Vector(317.54767284975253,208.59202150074685, 0.0));
    positionAlloc4->Add (Vector(312.24301305717603,153.50052522515077, 0.0));
    positionAlloc4->Add (Vector(61.582663255925894,305.60209553475397, 0.0));
    positionAlloc4->Add (Vector(128.28581273283834,117.94067783250206, 0.0));
    positionAlloc4->Add (Vector(349.5745005515283,343.4752776632362, 0.0));
    positionAlloc4->Add (Vector(233.84755206270117,44.51135260688197, 0.0));
    positionAlloc4->Add (Vector(272.2265898677436,177.39696604354006, 0.0));
    positionAlloc4->Add (Vector(167.24462407926697,85.93153649481944, 0.0));
    positionAlloc4->Add (Vector(121.98768226537094,183.07865739701742, 0.0));
    positionAlloc4->Add (Vector(175.46770876723804,314.45672554178867, 0.0));
    positionAlloc4->Add (Vector(151.6950301157051,82.7593449798202, 0.0));
    positionAlloc4->Add (Vector(165.82208183629845,271.80002383508315, 0.0));
    positionAlloc4->Add (Vector(393.67417911108373,53.087175947999434, 0.0));
    positionAlloc4->Add (Vector(258.0128363266207,202.94466236195134, 0.0));
    positionAlloc4->Add (Vector(146.2746865145845,317.89251065347105, 0.0));
    positionAlloc4->Add (Vector(16.07882714561546,76.30336365815715, 0.0));
    positionAlloc4->Add (Vector(156.69173778032706,380.79504937464884, 0.0));
    positionAlloc4->Add (Vector(302.28769160694236,297.1680586735535, 0.0));
    positionAlloc4->Add (Vector(355.1918950575065,156.737099486414, 0.0));
    positionAlloc4->Add (Vector(319.43861796757164,319.77137446498097, 0.0));
    positionAlloc4->Add (Vector(337.6877419932577,22.092751624178362, 0.0));
    positionAlloc4->Add (Vector(54.824855342682795,249.13213568750464, 0.0));
    positionAlloc4->Add (Vector(286.7108489037003,308.017382542835, 0.0));
    positionAlloc4->Add (Vector(358.13207855800135,305.96536461608395, 0.0));
    positionAlloc4->Add (Vector(30.980732699113833,17.78103222842993, 0.0));
    positionAlloc4->Add (Vector(104.09611079529029,212.4863175824183, 0.0));
    positionAlloc4->Add (Vector(0.49571222251456604,136.95490397904808, 0.0));
    positionAlloc4->Add (Vector(31.372350128027506,329.80995979089835, 0.0));
    positionAlloc4->Add (Vector(351.6470059109101,379.8042364916693, 0.0));
    positionAlloc4->Add (Vector(371.46846312337055,159.46918648652849, 0.0));
    positionAlloc4->Add (Vector(253.2809743859785,86.0338025246543, 0.0));
    positionAlloc4->Add (Vector(362.6063324509573,254.2718576059309, 0.0));
    positionAlloc4->Add (Vector(164.01834785370374,15.258106746792599, 0.0));
    positionAlloc4->Add (Vector(25.996702810851644,12.096129331539052, 0.0));
    positionAlloc4->Add (Vector(47.97437806081328,216.0868094703444, 0.0));
    positionAlloc4->Add (Vector(50.821529356476525,44.725334124595186, 0.0));
    positionAlloc4->Add (Vector(246.28916269866147,323.68104124369984, 0.0));
    positionAlloc4->Add (Vector(385.92246103861737,13.318931273308188, 0.0));
    positionAlloc4->Add (Vector(75.27807999999956,380.30941710026127, 0.0));
    positionAlloc4->Add (Vector(292.62394115817034,104.74628480720152, 0.0));
    positionAlloc4->Add (Vector(340.8134961132155,128.43644841604595, 0.0));
    positionAlloc4->Add (Vector(100.69772586319385,89.97860371648324, 0.0));
    positionAlloc4->Add (Vector(369.6281207666842,69.23586361553915, 0.0));
    positionAlloc4->Add (Vector(215.0851525694408,150.87890296672924, 0.0));
    positionAlloc4->Add (Vector(310.69630117727803,225.05549488667626, 0.0));
    positionAlloc4->Add (Vector(78.18837522314715,321.2737046152046, 0.0));
    positionAlloc4->Add (Vector(358.13785764076897,368.2208828731895, 0.0));
    positionAlloc4->Add (Vector(320.51048936284707,63.023880794132616, 0.0));
    positionAlloc4->Add (Vector(116.75670472371445,381.1414804502239, 0.0));
    positionAlloc4->Add (Vector(357.9924610392168,150.843723462347, 0.0));
    positionAlloc4->Add (Vector(160.35296105111536,327.7574345485896, 0.0));
    positionAlloc4->Add (Vector(90.62101761461072,275.4827033425095, 0.0));
    positionAlloc4->Add (Vector(222.52954168037945,82.28874898493497, 0.0));
    positionAlloc4->Add (Vector(320.08317286981736,372.3460190043545, 0.0));
    positionAlloc4->Add (Vector(180.02568351498746,159.30124555188164, 0.0));
    positionAlloc4->Add (Vector(167.36865093856835,312.7687078930384, 0.0));
    positionAlloc4->Add (Vector(217.49604952706906,128.17095799689983, 0.0));
    positionAlloc4->Add (Vector(83.67314471267954,180.08649928369385, 0.0));
    positionAlloc4->Add (Vector(57.65472966184277,316.5750859547854, 0.0));
    positionAlloc4->Add (Vector(384.72228422640484,136.97872189769052, 0.0));
    positionAlloc4->Add (Vector(200.1914909827593,28.850756171657153, 0.0));
    positionAlloc4->Add (Vector(33.29570423234909,385.8820883451971, 0.0));
    positionAlloc4->Add (Vector(380.32195627436386,376.93845511861974, 0.0));
    positionAlloc4->Add (Vector(301.2237851464314,261.9501579229232, 0.0));
    positionAlloc4->Add (Vector(41.7078254312925,5.026616676796802, 0.0));
    positionAlloc4->Add (Vector(285.6598566500607,142.28548832735203, 0.0));
    positionAlloc4->Add (Vector(331.57631834863383,351.7197298226878, 0.0));
    positionAlloc4->Add (Vector(236.16194916312386,20.131328810882376, 0.0));
    positionAlloc4->Add (Vector(72.2347948672004,106.51212309242636, 0.0));
    positionAlloc4->Add (Vector(342.43768893215594,62.21648268597053, 0.0));
    positionAlloc4->Add (Vector(285.8952202046424,266.4559098752153, 0.0));
    positionAlloc4->Add (Vector(302.4141832982962,119.98798620170254, 0.0));
    positionAlloc4->Add (Vector(282.8571608825189,249.96895767039234, 0.0));
    positionAlloc4->Add (Vector(202.63594366212422,82.0463535051397, 0.0));
    positionAlloc4->Add (Vector(27.424727240913384,390.8655758234248, 0.0));
    positionAlloc4->Add (Vector(345.8179854320219,251.82654528261446, 0.0));
    positionAlloc4->Add (Vector(36.09446736470221,155.15288659104164, 0.0));
    positionAlloc4->Add (Vector(229.1870064450985,324.64379531168856, 0.0));
    positionAlloc4->Add (Vector(65.63820341492361,263.63052596675294, 0.0));
    positionAlloc4->Add (Vector(243.82241337284026,60.70729860638529, 0.0));
    positionAlloc4->Add (Vector(164.05994893372693,385.83093696343644, 0.0));
    positionAlloc4->Add (Vector(65.84030709955297,19.798588180851738, 0.0));
    positionAlloc4->Add (Vector(380.03806964088926,185.25779424660564, 0.0));
    positionAlloc4->Add (Vector(166.06218582521674,296.7510545106944, 0.0));
    positionAlloc4->Add (Vector(230.86778975527213,247.4147474787382, 0.0));
    positionAlloc4->Add (Vector(12.313418978165114,220.15476737183212, 0.0));
    positionAlloc4->Add (Vector(152.25680832402614,33.06858885651498, 0.0));
    positionAlloc4->Add (Vector(20.03734118530498,355.3435004235158, 0.0));
    positionAlloc4->Add (Vector(59.699074843096554,24.069557499976703, 0.0));
    positionAlloc4->Add (Vector(132.79579601809212,2.5784973109765463, 0.0));
    positionAlloc4->Add (Vector(374.7753684504664,218.62283551459552, 0.0));
    positionAlloc4->Add (Vector(155.93145680453787,243.73090775064688, 0.0));
    positionAlloc4->Add (Vector(197.55484038813069,215.6879823712287, 0.0));
    positionAlloc4->Add (Vector(88.72639516854241,249.23496154635507, 0.0));
    positionAlloc4->Add (Vector(52.88712506634492,205.349250758389, 0.0));
    positionAlloc4->Add (Vector(359.0871084986863,273.3854851634915, 0.0));
    positionAlloc4->Add (Vector(331.6367682668425,246.56232890527357, 0.0));
    positionAlloc4->Add (Vector(351.90555555322504,203.8674965308807, 0.0));
    positionAlloc4->Add (Vector(161.93569400122172,143.07561088023317, 0.0));
    positionAlloc4->Add (Vector(309.59813485855176,233.74961036197567, 0.0));
    positionAlloc4->Add (Vector(201.79619744291563,237.80283240394064, 0.0));
    positionAlloc4->Add (Vector(363.6206705417127,220.46453792522453, 0.0));
    positionAlloc4->Add (Vector(374.67896514718086,118.98456827025838, 0.0));
    positionAlloc4->Add (Vector(112.62504938102956,375.2622907499024, 0.0));
    positionAlloc4->Add (Vector(41.02196043883399,338.588777631235, 0.0));
    positionAlloc4->Add (Vector(234.15173853676455,369.41870902311445, 0.0));
    positionAlloc4->Add (Vector(390.3859601532404,191.43963926007572, 0.0));
    positionAlloc4->Add (Vector(274.0643992801766,155.26992050351095, 0.0));
    positionAlloc4->Add (Vector(158.43546621694378,139.20869122239284, 0.0));
    positionAlloc4->Add (Vector(387.7781088246638,83.80640965771411, 0.0));
    positionAlloc4->Add (Vector(15.992369002963924,301.79085674282845, 0.0));
    positionAlloc4->Add (Vector(399.4524160369255,21.31819373626449, 0.0));
    positionAlloc4->Add (Vector(230.31083626924163,348.3704273377647, 0.0));
    positionAlloc4->Add (Vector(26.293976157732235,189.2254641446537, 0.0));
    positionAlloc4->Add (Vector(293.2777555479539,204.0462074263674, 0.0));
    positionAlloc4->Add (Vector(200.69867211789125,372.0104097201173, 0.0));
    positionAlloc4->Add (Vector(138.00064791658744,20.171373070396513, 0.0));
    positionAlloc4->Add (Vector(207.94525028946796,298.7623396683344, 0.0));
    positionAlloc4->Add (Vector(192.73426347586047,373.7455127303398, 0.0));
    positionAlloc4->Add (Vector(181.4609477470238,372.00934702197696, 0.0));
    positionAlloc4->Add (Vector(303.1719219673906,361.14246508158453, 0.0));
    positionAlloc4->Add (Vector(221.12808430908882,25.42990321724421, 0.0));
    positionAlloc4->Add (Vector(33.995956823107896,299.87481309323505, 0.0));
    positionAlloc4->Add (Vector(104.19209251480055,45.9250824728072, 0.0));
    positionAlloc4->Add (Vector(70.80247584543625,389.60516497611144, 0.0));
    positionAlloc4->Add (Vector(89.94588697807244,311.8619322950357, 0.0));
    positionAlloc4->Add (Vector(64.08060613186902,268.04356096946134, 0.0));
    positionAlloc4->Add (Vector(270.5787269822351,70.26990228062763, 0.0));
    positionAlloc4->Add (Vector(355.9935181758652,133.7295546268514, 0.0));
    positionAlloc4->Add (Vector(354.9376692775628,115.48313042888103, 0.0));
    positionAlloc4->Add (Vector(244.55129618357012,60.159112048851696, 0.0));
    positionAlloc4->Add (Vector(243.9087476493952,200.5548205170035, 0.0));
    positionAlloc4->Add (Vector(171.75285706322225,10.827931873800978, 0.0));
    positionAlloc4->Add (Vector(227.45496718296204,98.23463763200047, 0.0));
    positionAlloc4->Add (Vector(222.7693913677724,158.83199924255655, 0.0));
    positionAlloc4->Add (Vector(85.23876788491896,306.36835995034517, 0.0));
    positionAlloc4->Add (Vector(328.36112664750937,51.02603505919965, 0.0));
    positionAlloc4->Add (Vector(308.5558068777941,87.61425847495828, 0.0));
    positionAlloc4->Add (Vector(289.995549158042,124.41166379937378, 0.0));
    positionAlloc4->Add (Vector(352.0488595534024,8.221753289787781, 0.0));
    positionAlloc4->Add (Vector(304.9827741066363,156.9419657298618, 0.0));
    positionAlloc4->Add (Vector(52.58544335291661,366.16105912938207, 0.0));
    positionAlloc4->Add (Vector(45.647998215234686,71.70298445421079, 0.0));
    positionAlloc4->Add (Vector(278.65516450855455,134.43316976349067, 0.0));
    positionAlloc4->Add (Vector(202.8090811425477,33.0789813566974, 0.0));
    positionAlloc4->Add (Vector(20.82655739266981,21.293725919008068, 0.0));
    positionAlloc4->Add (Vector(263.89904644559977,19.85798595616832, 0.0));
    positionAlloc4->Add (Vector(365.32554501157813,182.40264782561457, 0.0));
    positionAlloc4->Add (Vector(284.34684929764325,170.88633265451446, 0.0));
    positionAlloc4->Add (Vector(255.30462817587312,287.8631369967815, 0.0));
    positionAlloc4->Add (Vector(200.7631467952323,18.84312274647808, 0.0));
    positionAlloc4->Add (Vector(171.72796408502387,217.69795815805196, 0.0));
    positionAlloc4->Add (Vector(124.26159514484398,285.2889943338997, 0.0));
    positionAlloc4->Add (Vector(172.18388413724603,251.62941197628857, 0.0));
    positionAlloc4->Add (Vector(44.94885352695821,218.32090978391236, 0.0));
    positionAlloc4->Add (Vector(358.2051073289444,9.529027631301723, 0.0));
    positionAlloc4->Add (Vector(367.56053971209013,101.69092901379342, 0.0));
    positionAlloc4->Add (Vector(159.66074558205517,148.49870535184655, 0.0));
    positionAlloc4->Add (Vector(304.4801106641909,392.74629638594786, 0.0));
    positionAlloc4->Add (Vector(155.65870409684427,374.134057700974, 0.0));
    positionAlloc4->Add (Vector(260.8050709483516,311.33723646780334, 0.0));
    positionAlloc4->Add (Vector(395.5110558308686,367.0205282683953, 0.0));
    positionAlloc4->Add (Vector(264.6271245207134,386.5082255734342, 0.0));
    positionAlloc4->Add (Vector(15.564370462904176,11.027402808740305, 0.0));
    positionAlloc4->Add (Vector(364.01367265307033,129.47710256125217, 0.0));
    positionAlloc4->Add (Vector(60.33244009435506,256.186352788666, 0.0));
    positionAlloc4->Add (Vector(153.33499784945167,52.10970993946802, 0.0));
    positionAlloc4->Add (Vector(280.4603792156007,258.3036055456983, 0.0));
    positionAlloc4->Add (Vector(221.04424754955167,233.4125105687749, 0.0));
    positionAlloc4->Add (Vector(284.8472122252664,203.58412144435576, 0.0));
    positionAlloc4->Add (Vector(56.10664277015589,189.9807722744415, 0.0));
    positionAlloc4->Add (Vector(148.48906727611464,295.1595342269949, 0.0));
    positionAlloc4->Add (Vector(133.88067816387758,376.0938329989454, 0.0));
    positionAlloc4->Add (Vector(336.6039539600214,292.0845137744162, 0.0));
    positionAlloc4->Add (Vector(126.76122251494304,76.47843744787868, 0.0));
    positionAlloc4->Add (Vector(108.64681995101759,108.8181711049319, 0.0));
    positionAlloc4->Add (Vector(334.0759746548634,397.53831218701646, 0.0));
    positionAlloc4->Add (Vector(51.05683435518951,183.22526670500835, 0.0));
    positionAlloc4->Add (Vector(105.81981436529114,137.40422723557347, 0.0));
    positionAlloc4->Add (Vector(358.7030378863919,30.068989771567047, 0.0));
    positionAlloc4->Add (Vector(331.0891647771149,4.818973359850975, 0.0));
    positionAlloc4->Add (Vector(93.3710370432229,59.63247997972005, 0.0));
    positionAlloc4->Add (Vector(341.5154139266981,342.398439696393, 0.0));
    positionAlloc4->Add (Vector(26.49487028611386,264.46648634456085, 0.0));
    positionAlloc4->Add (Vector(158.58371831210007,123.8792279377503, 0.0));
    positionAlloc4->Add (Vector(126.77835188694364,181.83135065993255, 0.0));
    positionAlloc4->Add (Vector(327.77635686881626,58.012790716433926, 0.0));
    positionAlloc4->Add (Vector(266.9386381657399,201.75200179195662, 0.0));
    positionAlloc4->Add (Vector(359.71987282507115,254.9386112457141, 0.0));
    positionAlloc4->Add (Vector(283.5995720134439,293.1467061712028, 0.0));
    positionAlloc4->Add (Vector(322.27117425314816,83.28203236984902, 0.0));
    positionAlloc4->Add (Vector(263.44887899842587,182.15362350062438, 0.0));
    positionAlloc4->Add (Vector(167.0972333376835,280.2182675438836, 0.0));
    positionAlloc4->Add (Vector(294.9642414954792,277.12658291506943, 0.0));
    positionAlloc4->Add (Vector(147.16828896935363,297.8864365872466, 0.0));
    positionAlloc4->Add (Vector(130.78028857373135,319.0047696474566, 0.0));
    positionAlloc4->Add (Vector(212.46186307316304,359.4913032199836, 0.0));
    positionAlloc4->Add (Vector(27.227841611970092,226.7637691812725, 0.0));
    positionAlloc4->Add (Vector(237.4630125804464,93.69402221132228, 0.0));
    positionAlloc4->Add (Vector(260.7813631385342,38.19623234433034, 0.0));
    positionAlloc4->Add (Vector(23.09984868077981,176.0012293802829, 0.0));
    positionAlloc4->Add (Vector(373.64504999259964,321.2056980008093, 0.0));
    positionAlloc4->Add (Vector(244.69367757235975,140.7177279357371, 0.0));
    positionAlloc4->Add (Vector(84.77056972589554,201.8271873677814, 0.0));
    positionAlloc4->Add (Vector(185.18888304761953,56.33908005238162, 0.0));
    positionAlloc4->Add (Vector(222.8703999363412,105.24014532699138, 0.0));
    positionAlloc4->Add (Vector(48.153898485441935,124.58001119523972, 0.0));
    positionAlloc4->Add (Vector(181.74442809522225,18.22130994171589, 0.0));
    positionAlloc4->Add (Vector(187.55351454052035,346.1716295982373, 0.0));
    positionAlloc4->Add (Vector(0.2612570649751511,322.09892099562205, 0.0));
    positionAlloc4->Add (Vector(353.58059855097906,21.048214845573465, 0.0));
    positionAlloc4->Add (Vector(110.42556979695397,78.24143826585215, 0.0));
    positionAlloc4->Add (Vector(343.71817276549604,308.38673022362593, 0.0));
    positionAlloc4->Add (Vector(187.16485316702324,79.02336781430618, 0.0));
    positionAlloc4->Add (Vector(274.04576730630725,145.03265385000947, 0.0));
    positionAlloc4->Add (Vector(95.63985173087084,360.9184666120218, 0.0));
    positionAlloc4->Add (Vector(323.09231794651237,360.7457808977099, 0.0));
    positionAlloc4->Add (Vector(161.35155393286743,246.56124091258235, 0.0));
    positionAlloc4->Add (Vector(66.43857861533533,185.54503917058472, 0.0));
    positionAlloc4->Add (Vector(374.2575184235341,365.12188251295925, 0.0));
    positionAlloc4->Add (Vector(53.53978411418758,224.98999410346468, 0.0));
    positionAlloc4->Add (Vector(93.68661057629906,125.22227111293063, 0.0));
    positionAlloc4->Add (Vector(53.99185238449693,322.7541425540216, 0.0));
    positionAlloc4->Add (Vector(33.08564021393279,134.317598663946, 0.0));
    positionAlloc4->Add (Vector(200.21349078180185,385.35296036510726, 0.0));
    positionAlloc4->Add (Vector(162.8277242318997,236.00519475535856, 0.0));
    positionAlloc4->Add (Vector(111.05489433659908,100.2637870461827, 0.0));
    positionAlloc4->Add (Vector(129.3328302724931,342.86950949779555, 0.0));
    positionAlloc4->Add (Vector(326.45238376678265,89.39446278246974, 0.0));
    positionAlloc4->Add (Vector(67.18415106614724,167.7219026399282, 0.0));
    positionAlloc4->Add (Vector(124.21611984709324,76.06651624983911, 0.0));
    positionAlloc4->Add (Vector(236.06146470840082,154.73383569634623, 0.0));
    positionAlloc4->Add (Vector(358.4592825300319,44.68990027730069, 0.0));
    positionAlloc4->Add (Vector(69.98101276104708,260.64599388705346, 0.0));
    positionAlloc4->Add (Vector(26.242997139828404,134.6864817817236, 0.0));
    positionAlloc4->Add (Vector(306.47270110183956,54.15003185955038, 0.0));
    positionAlloc4->Add (Vector(182.83460665794294,300.4733601936491, 0.0));
    positionAlloc4->Add (Vector(38.07439395296291,145.76900187147834, 0.0));
    positionAlloc4->Add (Vector(117.97361418536343,217.42949827398638, 0.0));
    positionAlloc4->Add (Vector(160.05021579713926,307.44948620512986, 0.0));
    positionAlloc4->Add (Vector(54.92947832061663,168.7933908931255, 0.0));
    positionAlloc4->Add (Vector(31.685864125455865,321.54100324272144, 0.0));
    positionAlloc4->Add (Vector(236.48378065124786,301.3081612839064, 0.0));
    positionAlloc4->Add (Vector(160.85212742062208,73.83727042782473, 0.0));
    positionAlloc4->Add (Vector(3.5150262977230895,305.460382803971, 0.0));
    positionAlloc4->Add (Vector(191.8732636602404,313.73103799506157, 0.0));
    positionAlloc4->Add (Vector(197.7423314805736,251.3561530205039, 0.0));
    positionAlloc4->Add (Vector(370.94866826895463,355.01350343432, 0.0));
    positionAlloc4->Add (Vector(175.05004676322247,365.0317080807824, 0.0));
    positionAlloc4->Add (Vector(54.407756732426506,158.13445285008208, 0.0));
    positionAlloc4->Add (Vector(182.20098925787366,241.37114276944405, 0.0));
    positionAlloc4->Add (Vector(145.01346094996092,121.32111335236395, 0.0));
    positionAlloc4->Add (Vector(353.32421383691815,184.33646003766225, 0.0));
    positionAlloc4->Add (Vector(106.4254792868498,327.27696682827377, 0.0));
    positionAlloc4->Add (Vector(141.5515866126691,155.00511555554834, 0.0));
    positionAlloc4->Add (Vector(29.742806248530187,190.50730563855828, 0.0));
    positionAlloc4->Add (Vector(154.70290102219005,236.9993915567229, 0.0));
    positionAlloc4->Add (Vector(324.15765126158226,153.88393937341465, 0.0));
    positionAlloc4->Add (Vector(327.1237730306894,321.12561289297565, 0.0));
    positionAlloc4->Add (Vector(275.5480116071483,91.7785782472479, 0.0));
    positionAlloc4->Add (Vector(271.00340635767463,325.73115675317854, 0.0));
    positionAlloc4->Add (Vector(193.06409952423823,115.77362089467651, 0.0));
    positionAlloc4->Add (Vector(138.9276386669291,276.906109960649, 0.0));
    positionAlloc4->Add (Vector(393.3923100936295,301.2056670967277, 0.0));
    positionAlloc4->Add (Vector(355.2720057271664,91.91908395601143, 0.0));
    positionAlloc4->Add (Vector(87.75349375893336,322.35290142228126, 0.0));
    positionAlloc4->Add (Vector(361.1293746601718,317.81879571404346, 0.0));
    positionAlloc4->Add (Vector(113.83356455629313,285.1689811060088, 0.0));
    positionAlloc4->Add (Vector(230.96812119741082,100.3876812600192, 0.0));
    positionAlloc4->Add (Vector(190.3115517886583,157.30889490646675, 0.0));
    positionAlloc4->Add (Vector(101.9262340780645,53.685084328212795, 0.0));
    positionAlloc4->Add (Vector(215.5739058447759,312.1175115706814, 0.0));
    positionAlloc4->Add (Vector(290.9293840702649,357.99132819309483, 0.0));
    positionAlloc4->Add (Vector(370.7034127067342,248.53956517118579, 0.0));
    positionAlloc4->Add (Vector(30.670207680959784,59.8760943687823, 0.0));
    positionAlloc4->Add (Vector(233.4575636385781,1.9567073927512446, 0.0));
    positionAlloc4->Add (Vector(175.34430659036823,168.19750980326634, 0.0));
    positionAlloc4->Add (Vector(379.45366172608976,186.99590944258793, 0.0));
    positionAlloc4->Add (Vector(267.0578011773264,151.1233940426627, 0.0));
    positionAlloc4->Add (Vector(384.5390474402941,172.5402536198027, 0.0));
    positionAlloc4->Add (Vector(383.275761452872,204.96595709037416, 0.0));
    positionAlloc4->Add (Vector(346.1108389231627,181.59623786950903, 0.0));
    positionAlloc4->Add (Vector(221.95266950798816,296.07396089199455, 0.0));
    positionAlloc4->Add (Vector(45.04255168342834,53.22112352398012, 0.0));
    positionAlloc4->Add (Vector(73.98325151100079,259.0228506125422, 0.0));
    positionAlloc4->Add (Vector(353.4824168691859,28.810333673651954, 0.0));
    positionAlloc4->Add (Vector(278.2674979642089,79.08638333461991, 0.0));
    positionAlloc4->Add (Vector(228.90997425383057,190.3073336473738, 0.0));
    positionAlloc4->Add (Vector(361.77268690400336,219.4519222924199, 0.0));
    positionAlloc4->Add (Vector(210.54432882684767,319.1196371030543, 0.0));
    positionAlloc4->Add (Vector(297.22855847437353,131.86114721384453, 0.0));
    positionAlloc4->Add (Vector(6.039732905665129,3.304971100200671, 0.0));
    positionAlloc4->Add (Vector(155.3228955724054,233.36989169229437, 0.0));
    positionAlloc4->Add (Vector(368.01057843002326,385.7130957798366, 0.0));
    positionAlloc4->Add (Vector(23.354101482619647,368.80962549175524, 0.0));
    positionAlloc4->Add (Vector(305.4736504991871,274.447952494725, 0.0));
    positionAlloc4->Add (Vector(159.5691408724305,265.30459426408163, 0.0));
    positionAlloc4->Add (Vector(208.30383921158443,275.2431533293263, 0.0));
    positionAlloc4->Add (Vector(369.0520159016941,341.12147004014605, 0.0));
    positionAlloc4->Add (Vector(72.34216428887161,285.48459139285984, 0.0));
    positionAlloc4->Add (Vector(28.05308743622099,337.97735537738646, 0.0));
    positionAlloc4->Add (Vector(344.9371482853221,126.6705932094923, 0.0));
    positionAlloc4->Add (Vector(59.98562319199725,372.29113141485703, 0.0));
    positionAlloc4->Add (Vector(233.93773807272154,203.81192796832326, 0.0));
    positionAlloc4->Add (Vector(320.3910682870929,15.611510049172272, 0.0));
    positionAlloc4->Add (Vector(144.62514408756513,279.27348421444555, 0.0));
    positionAlloc4->Add (Vector(70.57771456724673,275.17054756973585, 0.0));
    positionAlloc4->Add (Vector(282.469073041616,63.34355761922161, 0.0));
    positionAlloc4->Add (Vector(241.2142399719078,169.0856392521928, 0.0));
    positionAlloc4->Add (Vector(131.7412057499729,20.329931370097043, 0.0));
    positionAlloc4->Add (Vector(161.11557697580417,309.26689196030156, 0.0));
    positionAlloc4->Add (Vector(47.67513376958674,207.21066300475462, 0.0));
    positionAlloc4->Add (Vector(164.9354587786341,181.62738690918138, 0.0));
    positionAlloc4->Add (Vector(313.3178586140823,290.96772557334094, 0.0));
    positionAlloc4->Add (Vector(43.29090510882967,366.4927629970389, 0.0));
    positionAlloc4->Add (Vector(280.01784278649404,91.90328684600671, 0.0));
    positionAlloc4->Add (Vector(32.41966500584383,42.80761711331209, 0.0));
    positionAlloc4->Add (Vector(229.5590985477953,302.67638924753277, 0.0));
    positionAlloc4->Add (Vector(381.4969492749022,113.47300684652954, 0.0));
    positionAlloc4->Add (Vector(186.5186357198456,291.527420172005, 0.0));
    positionAlloc4->Add (Vector(79.51956560816247,390.7035098352613, 0.0));
    positionAlloc4->Add (Vector(253.89566273339454,292.0321397480737, 0.0));
    positionAlloc4->Add (Vector(20.679692800033457,397.1184540684379, 0.0));
    positionAlloc4->Add (Vector(391.26994370650175,143.86140262235747, 0.0));
    positionAlloc4->Add (Vector(260.0495351729605,394.89919313621414, 0.0));
    positionAlloc4->Add (Vector(385.61072480410405,224.25930001420392, 0.0));
    positionAlloc4->Add (Vector(218.70369533678547,162.96289456270983, 0.0));
    positionAlloc4->Add (Vector(62.50639979118788,22.439499891882342, 0.0));
    positionAlloc4->Add (Vector(381.8075279234294,325.05406945134695, 0.0));
    positionAlloc4->Add (Vector(139.89598783019775,319.70701708578764, 0.0));
    positionAlloc4->Add (Vector(313.0512679635902,85.10361649835194, 0.0));
    positionAlloc4->Add (Vector(103.41125832829503,20.822440125072905, 0.0));
    positionAlloc4->Add (Vector(212.95205262446845,244.93955454179468, 0.0));
    positionAlloc4->Add (Vector(107.21871894116619,119.2708825230802, 0.0));
    positionAlloc4->Add (Vector(290.45288053908786,369.0029591144436, 0.0));
    positionAlloc4->Add (Vector(325.2507290636931,290.70034343485503, 0.0));
    positionAlloc4->Add (Vector(276.7113074140112,365.69113784201755, 0.0));
    positionAlloc4->Add (Vector(108.94597474717446,391.12835225402284, 0.0));
    positionAlloc4->Add (Vector(213.84243292396775,204.13138183952265, 0.0));
    positionAlloc4->Add (Vector(263.81400997602105,295.4990058822289, 0.0));
    positionAlloc4->Add (Vector(31.16118907427845,150.88283551646197, 0.0));
    positionAlloc4->Add (Vector(296.7934408624583,233.52928739486813, 0.0));
    positionAlloc4->Add (Vector(189.43129609027017,380.5559761360476, 0.0));
    positionAlloc4->Add (Vector(284.1815091889427,157.1352041470704, 0.0));
    positionAlloc4->Add (Vector(112.945679309545,324.0076660201086, 0.0));
    positionAlloc4->Add (Vector(120.65037929115854,180.28754268063457, 0.0));
    positionAlloc4->Add (Vector(181.84835849902274,218.4162710520111, 0.0));
    positionAlloc4->Add (Vector(177.0671239001055,35.64591615767632, 0.0));
    positionAlloc4->Add (Vector(15.167951107139421,242.82822442079163, 0.0));
    positionAlloc4->Add (Vector(168.11230786155295,398.16611349538897, 0.0));
    positionAlloc4->Add (Vector(157.54845774356068,93.31967410804931, 0.0));
    positionAlloc4->Add (Vector(160.45181940580915,53.38743775269492, 0.0));
    positionAlloc4->Add (Vector(90.38414960523235,339.05739009144725, 0.0));
    positionAlloc4->Add (Vector(357.3823904017334,229.85128654920493, 0.0));
    positionAlloc4->Add (Vector(136.6949276764866,340.64302325394516, 0.0));
    positionAlloc4->Add (Vector(130.22221124889865,4.17347451365031, 0.0));
    positionAlloc4->Add (Vector(379.81955280749503,152.2374554888157, 0.0));
    positionAlloc4->Add (Vector(147.40423561911928,107.54651755135339, 0.0));
    positionAlloc4->Add (Vector(127.13614221129181,154.87423954435346, 0.0));
    positionAlloc4->Add (Vector(373.9794491928788,138.88814222658635, 0.0));
    positionAlloc4->Add (Vector(249.622374183551,155.84221121188313, 0.0));
    positionAlloc4->Add (Vector(339.59849769974534,265.28750177038944, 0.0));
    positionAlloc4->Add (Vector(236.2799556130452,264.50118085736085, 0.0));
    positionAlloc4->Add (Vector(195.60443741810292,355.66131084532896, 0.0));
    positionAlloc4->Add (Vector(280.3585873724934,203.11842166947622, 0.0));
    positionAlloc4->Add (Vector(39.5800453653453,169.65630936197934, 0.0));
    positionAlloc4->Add (Vector(258.92739696860633,65.99811995570559, 0.0));
    positionAlloc4->Add (Vector(9.319174113416029,137.67501997121738, 0.0));
    positionAlloc4->Add (Vector(342.035696366456,44.72243064914858, 0.0));
    positionAlloc4->Add (Vector(194.76218546984688,170.06372458356483, 0.0));
    positionAlloc4->Add (Vector(102.64569433070298,176.78794094734874, 0.0));
    positionAlloc4->Add (Vector(387.7671987838445,136.83773352237503, 0.0));
    positionAlloc4->Add (Vector(169.3183402084228,319.0202918099057, 0.0));
    positionAlloc4->Add (Vector(186.05467843305647,203.06763848614588, 0.0));
    positionAlloc4->Add (Vector(127.54878948902872,258.92591296706246, 0.0));
    positionAlloc4->Add (Vector(75.0713451005017,319.10412662116676, 0.0));
    positionAlloc4->Add (Vector(198.08645243822505,386.2032794968303, 0.0));
    positionAlloc4->Add (Vector(225.29109370249563,309.76357857074015, 0.0));
    positionAlloc4->Add (Vector(98.88325414899417,0.8551511840554138, 0.0));
    positionAlloc4->Add (Vector(372.3945322813528,126.58710904385507, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_18.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_18.flowmon", false, false);
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