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
	  uint16_t numberOfNodes = 680;
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
    positionAlloc->Add (Vector (400.0,270.0, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,140.0, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (270.0,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (270.0,270.0, 0.0));
    positionAlloc->Add (Vector (270.0,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,140.0, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,10.0, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,270.0, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,10.0, 0.0));
    positionAlloc->Add (Vector (140.0,400.0, 0.0));
    positionAlloc->Add (Vector (140.0,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (140.0,270.0, 0.0));
    positionAlloc->Add (Vector (140.0,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (140.0,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,10.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,270.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,140.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (10.0,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (10.0,183.33333333333334, 0.0));
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
    positionAlloc4->Add (Vector(293.4030928665265,33.227565127878876, 0.0));
    positionAlloc4->Add (Vector(16.525209271427155,108.31367810099302, 0.0));
    positionAlloc4->Add (Vector(84.96663150213269,158.49266958440649, 0.0));
    positionAlloc4->Add (Vector(384.5889404214799,42.60276101065257, 0.0));
    positionAlloc4->Add (Vector(184.57684850415572,206.18128331507353, 0.0));
    positionAlloc4->Add (Vector(65.48961041655765,174.66686717246017, 0.0));
    positionAlloc4->Add (Vector(302.31984616251844,353.1462270979229, 0.0));
    positionAlloc4->Add (Vector(22.181860699382838,303.94438719469775, 0.0));
    positionAlloc4->Add (Vector(227.9356835486324,20.983859017814545, 0.0));
    positionAlloc4->Add (Vector(360.8199793496026,56.92708278958727, 0.0));
    positionAlloc4->Add (Vector(234.7133470715581,141.6520848439624, 0.0));
    positionAlloc4->Add (Vector(364.17557572859175,314.0850972995846, 0.0));
    positionAlloc4->Add (Vector(375.6662524115648,92.93042331908143, 0.0));
    positionAlloc4->Add (Vector(374.36403367200717,180.4006049246204, 0.0));
    positionAlloc4->Add (Vector(370.5161306021794,311.7024211101696, 0.0));
    positionAlloc4->Add (Vector(272.6557538932776,170.01642380826786, 0.0));
    positionAlloc4->Add (Vector(111.98272194785517,225.84537641415764, 0.0));
    positionAlloc4->Add (Vector(224.78965762914163,229.40922403465783, 0.0));
    positionAlloc4->Add (Vector(4.964701604589861,329.53410129639076, 0.0));
    positionAlloc4->Add (Vector(43.98339018645476,389.8663909809181, 0.0));
    positionAlloc4->Add (Vector(239.71604910691414,309.2145337047779, 0.0));
    positionAlloc4->Add (Vector(78.16743245560494,356.82268850391165, 0.0));
    positionAlloc4->Add (Vector(275.5284328508459,52.9187617199419, 0.0));
    positionAlloc4->Add (Vector(389.0991666597446,302.2051257440031, 0.0));
    positionAlloc4->Add (Vector(86.64480962898718,40.52296031825957, 0.0));
    positionAlloc4->Add (Vector(180.61986555469565,116.01016820383433, 0.0));
    positionAlloc4->Add (Vector(192.70273432684886,134.60133605156716, 0.0));
    positionAlloc4->Add (Vector(213.2505617978035,128.17632166988724, 0.0));
    positionAlloc4->Add (Vector(316.10089048315285,16.718269822171415, 0.0));
    positionAlloc4->Add (Vector(283.8756539456634,382.2331796691595, 0.0));
    positionAlloc4->Add (Vector(313.0704491319315,9.175886762853613, 0.0));
    positionAlloc4->Add (Vector(149.74524566863522,119.02884924549694, 0.0));
    positionAlloc4->Add (Vector(62.790957395540346,52.56905750863772, 0.0));
    positionAlloc4->Add (Vector(190.20682995178794,303.9081887178756, 0.0));
    positionAlloc4->Add (Vector(399.44888410888535,167.67073859487036, 0.0));
    positionAlloc4->Add (Vector(10.659872455625097,397.1861291575097, 0.0));
    positionAlloc4->Add (Vector(54.203349577893476,204.73865072173706, 0.0));
    positionAlloc4->Add (Vector(378.05789160856693,344.45580402126865, 0.0));
    positionAlloc4->Add (Vector(53.90591475182407,90.10637077997883, 0.0));
    positionAlloc4->Add (Vector(204.48769459298433,306.31136443728457, 0.0));
    positionAlloc4->Add (Vector(65.49101548365014,325.64825012721974, 0.0));
    positionAlloc4->Add (Vector(134.81197180832774,256.79949489182974, 0.0));
    positionAlloc4->Add (Vector(87.69134928857821,196.84556572137683, 0.0));
    positionAlloc4->Add (Vector(328.44713791077595,6.800175498986993, 0.0));
    positionAlloc4->Add (Vector(130.19572884081794,231.86497255480796, 0.0));
    positionAlloc4->Add (Vector(79.8565840195947,85.5461137251618, 0.0));
    positionAlloc4->Add (Vector(74.76501532028811,142.2261874870542, 0.0));
    positionAlloc4->Add (Vector(278.1168317656267,371.8965596043871, 0.0));
    positionAlloc4->Add (Vector(183.20647137707135,108.54374435332188, 0.0));
    positionAlloc4->Add (Vector(3.95312974908264,227.91503767713084, 0.0));
    positionAlloc4->Add (Vector(258.45964033180934,179.0297205487115, 0.0));
    positionAlloc4->Add (Vector(95.3791690179111,379.4457018095474, 0.0));
    positionAlloc4->Add (Vector(387.4872438947927,300.49760658171556, 0.0));
    positionAlloc4->Add (Vector(236.7966688072351,63.45572706123925, 0.0));
    positionAlloc4->Add (Vector(264.8567961587575,352.6236773376673, 0.0));
    positionAlloc4->Add (Vector(192.67317689410558,3.5378159255137653, 0.0));
    positionAlloc4->Add (Vector(40.81652066010291,30.43501410347669, 0.0));
    positionAlloc4->Add (Vector(261.81534764126496,288.44223146726546, 0.0));
    positionAlloc4->Add (Vector(89.38171453127302,195.96813111923933, 0.0));
    positionAlloc4->Add (Vector(136.3512981236719,380.937999974925, 0.0));
    positionAlloc4->Add (Vector(74.60273389327784,42.56968226235984, 0.0));
    positionAlloc4->Add (Vector(17.92960750225334,2.563220904501229, 0.0));
    positionAlloc4->Add (Vector(222.3144538108183,33.409842456780666, 0.0));
    positionAlloc4->Add (Vector(62.14078933052036,269.4239995871511, 0.0));
    positionAlloc4->Add (Vector(158.755138412431,122.99890726554064, 0.0));
    positionAlloc4->Add (Vector(279.09517221590147,241.71902528690774, 0.0));
    positionAlloc4->Add (Vector(176.02013317044808,172.47379748306022, 0.0));
    positionAlloc4->Add (Vector(195.76885808067473,263.4022825849912, 0.0));
    positionAlloc4->Add (Vector(131.12571471439094,313.9861418433104, 0.0));
    positionAlloc4->Add (Vector(84.6403873311671,293.6938311750175, 0.0));
    positionAlloc4->Add (Vector(365.5576073677662,134.22906049236033, 0.0));
    positionAlloc4->Add (Vector(197.85814345491133,334.61616887263676, 0.0));
    positionAlloc4->Add (Vector(154.85525893305416,153.1503905962082, 0.0));
    positionAlloc4->Add (Vector(392.16352343449694,334.2923641910402, 0.0));
    positionAlloc4->Add (Vector(233.68612901337596,112.95564680899233, 0.0));
    positionAlloc4->Add (Vector(153.92631793310642,179.81113269388854, 0.0));
    positionAlloc4->Add (Vector(291.24409803099485,196.04469156827395, 0.0));
    positionAlloc4->Add (Vector(28.07819374767564,271.32885957471973, 0.0));
    positionAlloc4->Add (Vector(392.2683422325907,284.9305782814352, 0.0));
    positionAlloc4->Add (Vector(211.70413047565106,41.56295493351925, 0.0));
    positionAlloc4->Add (Vector(335.80420584995335,357.728752722495, 0.0));
    positionAlloc4->Add (Vector(163.51220334671547,291.1492131192864, 0.0));
    positionAlloc4->Add (Vector(260.5652195534423,389.941198467475, 0.0));
    positionAlloc4->Add (Vector(257.8130644524436,260.94470008817444, 0.0));
    positionAlloc4->Add (Vector(119.91617573831874,290.6350615941555, 0.0));
    positionAlloc4->Add (Vector(150.95644352029876,180.14474536777047, 0.0));
    positionAlloc4->Add (Vector(36.38613287854731,160.8222294891131, 0.0));
    positionAlloc4->Add (Vector(149.88550635429442,385.3811011696853, 0.0));
    positionAlloc4->Add (Vector(346.03147706302553,96.67164699881239, 0.0));
    positionAlloc4->Add (Vector(87.61609602773524,215.45887171173018, 0.0));
    positionAlloc4->Add (Vector(75.86187941484343,258.5298420624053, 0.0));
    positionAlloc4->Add (Vector(271.71543464263635,208.94766466529398, 0.0));
    positionAlloc4->Add (Vector(155.87448967651846,125.03841906683553, 0.0));
    positionAlloc4->Add (Vector(366.7009322003585,10.885968536185375, 0.0));
    positionAlloc4->Add (Vector(167.0535585227685,97.07735909938444, 0.0));
    positionAlloc4->Add (Vector(85.04460738962045,41.24853182948658, 0.0));
    positionAlloc4->Add (Vector(395.5092305020028,136.66556243106533, 0.0));
    positionAlloc4->Add (Vector(311.46293465167054,55.302522407081334, 0.0));
    positionAlloc4->Add (Vector(312.7568934616947,9.591392042399471, 0.0));
    positionAlloc4->Add (Vector(102.74306561073136,10.303374094638151, 0.0));
    positionAlloc4->Add (Vector(211.91509948385323,30.76969381833914, 0.0));
    positionAlloc4->Add (Vector(121.43658182501791,163.47131059042178, 0.0));
    positionAlloc4->Add (Vector(247.86068013920075,287.04505031145715, 0.0));
    positionAlloc4->Add (Vector(359.02040951521315,112.72337044780846, 0.0));
    positionAlloc4->Add (Vector(239.66456032345573,396.35793020993106, 0.0));
    positionAlloc4->Add (Vector(229.12354790111732,19.781951594863756, 0.0));
    positionAlloc4->Add (Vector(149.1252379619062,172.6441189994699, 0.0));
    positionAlloc4->Add (Vector(200.2891499521478,98.15693568656978, 0.0));
    positionAlloc4->Add (Vector(196.59380466505561,14.767530652786842, 0.0));
    positionAlloc4->Add (Vector(153.12269627032902,107.51612203944192, 0.0));
    positionAlloc4->Add (Vector(184.9399095572543,65.70908173152037, 0.0));
    positionAlloc4->Add (Vector(14.602744332039164,246.56884456409375, 0.0));
    positionAlloc4->Add (Vector(38.17663943688587,375.0760957464109, 0.0));
    positionAlloc4->Add (Vector(288.3324309667449,58.6622050347108, 0.0));
    positionAlloc4->Add (Vector(181.42771382959643,321.56574230550643, 0.0));
    positionAlloc4->Add (Vector(291.26052008015625,72.13892617995401, 0.0));
    positionAlloc4->Add (Vector(129.5746878571385,361.2449952856306, 0.0));
    positionAlloc4->Add (Vector(286.1812703584829,384.95641112036685, 0.0));
    positionAlloc4->Add (Vector(125.62523296204495,85.33710518371254, 0.0));
    positionAlloc4->Add (Vector(31.66682081682324,83.5184640828428, 0.0));
    positionAlloc4->Add (Vector(316.1571947754785,135.59953544929803, 0.0));
    positionAlloc4->Add (Vector(249.36586377042437,80.41117672545855, 0.0));
    positionAlloc4->Add (Vector(187.34036147542548,23.060577578382755, 0.0));
    positionAlloc4->Add (Vector(353.46366304239,161.43778805137254, 0.0));
    positionAlloc4->Add (Vector(61.47805409660636,240.25099721389645, 0.0));
    positionAlloc4->Add (Vector(24.24928443669141,160.33132863953665, 0.0));
    positionAlloc4->Add (Vector(193.54177596949387,132.70518522195687, 0.0));
    positionAlloc4->Add (Vector(327.9708831263344,219.12257749447028, 0.0));
    positionAlloc4->Add (Vector(135.62502331244426,266.5845670713475, 0.0));
    positionAlloc4->Add (Vector(343.5998780468425,192.27842083959058, 0.0));
    positionAlloc4->Add (Vector(263.8687228258262,148.38433129037273, 0.0));
    positionAlloc4->Add (Vector(223.20745224074165,204.48913690827996, 0.0));
    positionAlloc4->Add (Vector(19.33877552318983,397.13714333301004, 0.0));
    positionAlloc4->Add (Vector(61.13122719275794,69.8990216705592, 0.0));
    positionAlloc4->Add (Vector(108.38382687137221,83.95879883043338, 0.0));
    positionAlloc4->Add (Vector(191.6466776886872,216.671970001238, 0.0));
    positionAlloc4->Add (Vector(113.22868068879527,351.0220859717541, 0.0));
    positionAlloc4->Add (Vector(72.36946178456063,234.78344167554516, 0.0));
    positionAlloc4->Add (Vector(347.3492017718716,116.00624405777249, 0.0));
    positionAlloc4->Add (Vector(390.4974710635089,158.19410980458014, 0.0));
    positionAlloc4->Add (Vector(341.9714278539053,168.370617875666, 0.0));
    positionAlloc4->Add (Vector(169.65053372647546,172.87698054347564, 0.0));
    positionAlloc4->Add (Vector(354.4551996907926,230.2836681326192, 0.0));
    positionAlloc4->Add (Vector(212.36347797480354,240.70711283420917, 0.0));
    positionAlloc4->Add (Vector(186.78968396795156,198.78018532259287, 0.0));
    positionAlloc4->Add (Vector(151.02152437145145,367.249859169998, 0.0));
    positionAlloc4->Add (Vector(155.92671646826867,164.1378127642369, 0.0));
    positionAlloc4->Add (Vector(55.116538445747175,19.10236784567525, 0.0));
    positionAlloc4->Add (Vector(25.991262108608247,209.40180622302512, 0.0));
    positionAlloc4->Add (Vector(260.5652439582794,118.54683628316485, 0.0));
    positionAlloc4->Add (Vector(280.34169578110897,134.73469324705172, 0.0));
    positionAlloc4->Add (Vector(100.49870877270952,345.69709339606567, 0.0));
    positionAlloc4->Add (Vector(330.9025290646677,3.9454243953521484, 0.0));
    positionAlloc4->Add (Vector(89.02653842313026,184.12899630557718, 0.0));
    positionAlloc4->Add (Vector(221.9452870305494,35.46862865479512, 0.0));
    positionAlloc4->Add (Vector(388.3585242517444,141.8192730429924, 0.0));
    positionAlloc4->Add (Vector(346.30826193020687,353.83827701249845, 0.0));
    positionAlloc4->Add (Vector(73.21580715676106,323.2322669900092, 0.0));
    positionAlloc4->Add (Vector(0.9131283375369836,262.2933175355191, 0.0));
    positionAlloc4->Add (Vector(46.527737753347594,384.0857154074384, 0.0));
    positionAlloc4->Add (Vector(287.7495158878034,289.5087841261346, 0.0));
    positionAlloc4->Add (Vector(394.45617377014446,134.83926441322458, 0.0));
    positionAlloc4->Add (Vector(75.22996975385104,98.52559066966239, 0.0));
    positionAlloc4->Add (Vector(395.457763437159,191.04528643592263, 0.0));
    positionAlloc4->Add (Vector(154.15670573243773,367.23380892859296, 0.0));
    positionAlloc4->Add (Vector(21.306333606189476,70.89031934300482, 0.0));
    positionAlloc4->Add (Vector(8.649492423441618,375.19220561341314, 0.0));
    positionAlloc4->Add (Vector(139.8685937634451,263.1665898356412, 0.0));
    positionAlloc4->Add (Vector(238.82142627955858,151.64125977314936, 0.0));
    positionAlloc4->Add (Vector(390.7702855865846,174.62275521341977, 0.0));
    positionAlloc4->Add (Vector(174.09095143374978,164.51678953374764, 0.0));
    positionAlloc4->Add (Vector(395.97784666942874,72.84748354745054, 0.0));
    positionAlloc4->Add (Vector(379.7634891753668,182.71272489748415, 0.0));
    positionAlloc4->Add (Vector(318.334498793687,374.73736555356277, 0.0));
    positionAlloc4->Add (Vector(66.38409105620298,108.97234091186476, 0.0));
    positionAlloc4->Add (Vector(147.34612344940788,96.14101016567393, 0.0));
    positionAlloc4->Add (Vector(327.54298708163265,310.4799777429716, 0.0));
    positionAlloc4->Add (Vector(49.249343996526385,12.557714855642965, 0.0));
    positionAlloc4->Add (Vector(184.93466862900974,257.8777769408118, 0.0));
    positionAlloc4->Add (Vector(372.70114892190196,261.92538607866476, 0.0));
    positionAlloc4->Add (Vector(80.54051321083189,73.22866996470756, 0.0));
    positionAlloc4->Add (Vector(125.71498502726475,56.35883162349828, 0.0));
    positionAlloc4->Add (Vector(208.97785752247918,253.74004979747406, 0.0));
    positionAlloc4->Add (Vector(156.11280335731584,183.66933789745374, 0.0));
    positionAlloc4->Add (Vector(3.8864058263091916,67.52819069077103, 0.0));
    positionAlloc4->Add (Vector(387.1141541883279,263.1605431841566, 0.0));
    positionAlloc4->Add (Vector(113.74662515512415,247.89717814871693, 0.0));
    positionAlloc4->Add (Vector(345.1611807664964,389.7793456441283, 0.0));
    positionAlloc4->Add (Vector(16.939292332427414,255.96694526459567, 0.0));
    positionAlloc4->Add (Vector(54.67650590104496,290.57706923357887, 0.0));
    positionAlloc4->Add (Vector(152.15565272142425,355.03642038570547, 0.0));
    positionAlloc4->Add (Vector(344.2899178948417,286.15496775305235, 0.0));
    positionAlloc4->Add (Vector(188.5012796009456,53.24958983075465, 0.0));
    positionAlloc4->Add (Vector(53.613979123181906,297.91054509812244, 0.0));
    positionAlloc4->Add (Vector(298.61693423029413,232.0682584518819, 0.0));
    positionAlloc4->Add (Vector(123.98435230872384,186.33066345084796, 0.0));
    positionAlloc4->Add (Vector(45.942731063891664,128.9166279214313, 0.0));
    positionAlloc4->Add (Vector(154.00751759803745,210.7045515390146, 0.0));
    positionAlloc4->Add (Vector(340.7427032355568,19.363900833548755, 0.0));
    positionAlloc4->Add (Vector(355.8782620363496,96.28922323426221, 0.0));
    positionAlloc4->Add (Vector(135.12862729428073,352.5175167342755, 0.0));
    positionAlloc4->Add (Vector(305.3133285934208,316.8746904945827, 0.0));
    positionAlloc4->Add (Vector(89.91329666680747,355.1089330328124, 0.0));
    positionAlloc4->Add (Vector(341.91728931962734,291.4351457150211, 0.0));
    positionAlloc4->Add (Vector(137.48043615346126,223.49966327206428, 0.0));
    positionAlloc4->Add (Vector(49.76659142153981,4.3628238415600595, 0.0));
    positionAlloc4->Add (Vector(173.51330438062425,170.11283545661655, 0.0));
    positionAlloc4->Add (Vector(150.49764273396687,200.59547041892412, 0.0));
    positionAlloc4->Add (Vector(121.05697019584132,105.57123929514538, 0.0));
    positionAlloc4->Add (Vector(5.110181777553846,39.70174807445095, 0.0));
    positionAlloc4->Add (Vector(65.16158446506557,114.02803788688627, 0.0));
    positionAlloc4->Add (Vector(365.5324397263311,3.5036512684880172, 0.0));
    positionAlloc4->Add (Vector(134.72248459474434,394.75402336015605, 0.0));
    positionAlloc4->Add (Vector(27.665964052833303,222.2465180844902, 0.0));
    positionAlloc4->Add (Vector(130.9026796507757,144.76136013990111, 0.0));
    positionAlloc4->Add (Vector(136.71293177486623,91.7621848984952, 0.0));
    positionAlloc4->Add (Vector(140.61936476687214,155.20117463883838, 0.0));
    positionAlloc4->Add (Vector(166.27512447636215,68.88688693757499, 0.0));
    positionAlloc4->Add (Vector(36.24063976267813,333.5588434204024, 0.0));
    positionAlloc4->Add (Vector(361.62309412967136,187.8730189052988, 0.0));
    positionAlloc4->Add (Vector(393.09219800801213,279.0913441714637, 0.0));
    positionAlloc4->Add (Vector(279.371033592611,5.903109607647883, 0.0));
    positionAlloc4->Add (Vector(157.14906116257347,376.22498127116523, 0.0));
    positionAlloc4->Add (Vector(124.03028150309612,10.037084313710665, 0.0));
    positionAlloc4->Add (Vector(327.25956529423235,93.25424129076171, 0.0));
    positionAlloc4->Add (Vector(320.83273990707283,242.16609001714255, 0.0));
    positionAlloc4->Add (Vector(259.91000786479844,143.63893760318308, 0.0));
    positionAlloc4->Add (Vector(94.74290934071759,146.84497082925225, 0.0));
    positionAlloc4->Add (Vector(119.0213663189581,33.826566913778144, 0.0));
    positionAlloc4->Add (Vector(384.8249483413821,276.3831966536784, 0.0));
    positionAlloc4->Add (Vector(292.59065082250277,287.7856213248378, 0.0));
    positionAlloc4->Add (Vector(196.94656322333356,60.79281778945056, 0.0));
    positionAlloc4->Add (Vector(336.56599552446164,383.3349773707079, 0.0));
    positionAlloc4->Add (Vector(69.34651214100707,35.407432358932624, 0.0));
    positionAlloc4->Add (Vector(380.3861315175208,263.5933423871637, 0.0));
    positionAlloc4->Add (Vector(268.17835786242813,241.67678218808257, 0.0));
    positionAlloc4->Add (Vector(174.00081571000476,382.71235390918116, 0.0));
    positionAlloc4->Add (Vector(177.9752958652294,157.69745515836613, 0.0));
    positionAlloc4->Add (Vector(57.206996802548545,250.6927508351203, 0.0));
    positionAlloc4->Add (Vector(181.1603823070884,104.16302600987865, 0.0));
    positionAlloc4->Add (Vector(130.86773118642708,34.50500410274149, 0.0));
    positionAlloc4->Add (Vector(174.9632789541639,339.8376755082566, 0.0));
    positionAlloc4->Add (Vector(388.3376713079211,373.1020048936552, 0.0));
    positionAlloc4->Add (Vector(182.39344696769416,15.699498197311357, 0.0));
    positionAlloc4->Add (Vector(285.8507852950128,117.4426257219877, 0.0));
    positionAlloc4->Add (Vector(305.6949908322906,44.800353620411705, 0.0));
    positionAlloc4->Add (Vector(368.08811256631446,52.35935743883737, 0.0));
    positionAlloc4->Add (Vector(162.8889297191818,30.98291426555044, 0.0));
    positionAlloc4->Add (Vector(20.709194260721997,281.57092929252406, 0.0));
    positionAlloc4->Add (Vector(363.48188762444266,139.16734493286555, 0.0));
    positionAlloc4->Add (Vector(356.87184349685094,148.9374093174697, 0.0));
    positionAlloc4->Add (Vector(399.69751985281783,42.67650562637195, 0.0));
    positionAlloc4->Add (Vector(153.7053193990735,22.37688636753954, 0.0));
    positionAlloc4->Add (Vector(158.18229433629486,260.28841267529896, 0.0));
    positionAlloc4->Add (Vector(43.90930451902291,212.35720873295065, 0.0));
    positionAlloc4->Add (Vector(259.36951315234313,52.108076526600385, 0.0));
    positionAlloc4->Add (Vector(316.7936901243063,184.7812914349942, 0.0));
    positionAlloc4->Add (Vector(240.85893020206834,350.1587157798654, 0.0));
    positionAlloc4->Add (Vector(374.2480064396796,166.2096589884785, 0.0));
    positionAlloc4->Add (Vector(76.27050181449091,221.4071092099338, 0.0));
    positionAlloc4->Add (Vector(168.88825963688788,339.107649158198, 0.0));
    positionAlloc4->Add (Vector(46.40492394063291,397.10902587616783, 0.0));
    positionAlloc4->Add (Vector(184.95469516951997,270.71687706149476, 0.0));
    positionAlloc4->Add (Vector(126.25228751739188,105.35498744670986, 0.0));
    positionAlloc4->Add (Vector(4.730725674515668,298.8680235726497, 0.0));
    positionAlloc4->Add (Vector(73.37912659058516,153.94238243448498, 0.0));
    positionAlloc4->Add (Vector(285.27071769644016,341.7319241530215, 0.0));
    positionAlloc4->Add (Vector(295.11054024754947,265.4143934835256, 0.0));
    positionAlloc4->Add (Vector(172.25939335791142,101.5240538021685, 0.0));
    positionAlloc4->Add (Vector(142.50002010948668,305.096876290529, 0.0));
    positionAlloc4->Add (Vector(7.75257356377117,317.76581394682916, 0.0));
    positionAlloc4->Add (Vector(145.7194062726617,73.79474751863255, 0.0));
    positionAlloc4->Add (Vector(298.72567286712314,129.7568886368176, 0.0));
    positionAlloc4->Add (Vector(139.6311716134184,20.875584056862273, 0.0));
    positionAlloc4->Add (Vector(110.84128976382593,193.70166089474537, 0.0));
    positionAlloc4->Add (Vector(338.9788808187065,26.232504449053764, 0.0));
    positionAlloc4->Add (Vector(138.96643279945775,390.06068173786895, 0.0));
    positionAlloc4->Add (Vector(109.64642939057616,32.537213034054304, 0.0));
    positionAlloc4->Add (Vector(356.9722450554075,246.8279821854031, 0.0));
    positionAlloc4->Add (Vector(221.15828297276914,182.03759395377284, 0.0));
    positionAlloc4->Add (Vector(191.2569154665385,161.35162427783462, 0.0));
    positionAlloc4->Add (Vector(289.5701173872753,299.12787315597353, 0.0));
    positionAlloc4->Add (Vector(193.80193113091363,120.20095797699094, 0.0));
    positionAlloc4->Add (Vector(19.93706963473145,220.3143277995733, 0.0));
    positionAlloc4->Add (Vector(267.1445431871567,104.30436641245096, 0.0));
    positionAlloc4->Add (Vector(36.403991624426894,270.77448910131807, 0.0));
    positionAlloc4->Add (Vector(315.6506936483394,229.5981884513205, 0.0));
    positionAlloc4->Add (Vector(114.45383963670123,12.066409032153125, 0.0));
    positionAlloc4->Add (Vector(44.231850962612015,63.45311616718874, 0.0));
    positionAlloc4->Add (Vector(126.45622742171155,82.12842247150216, 0.0));
    positionAlloc4->Add (Vector(51.978253633414575,131.05431748418064, 0.0));
    positionAlloc4->Add (Vector(96.36877583633901,374.6183757879158, 0.0));
    positionAlloc4->Add (Vector(308.13606500377614,278.28907201125526, 0.0));
    positionAlloc4->Add (Vector(31.10695458357906,9.91547453345274, 0.0));
    positionAlloc4->Add (Vector(228.1764949389066,258.98251409985426, 0.0));
    positionAlloc4->Add (Vector(55.907701297706595,290.7633344643742, 0.0));
    positionAlloc4->Add (Vector(137.8650182647422,104.79487469201287, 0.0));
    positionAlloc4->Add (Vector(52.73504340855033,269.63793210879356, 0.0));
    positionAlloc4->Add (Vector(316.6003787263651,367.08581457411265, 0.0));
    positionAlloc4->Add (Vector(332.5944488288132,77.13335711058424, 0.0));
    positionAlloc4->Add (Vector(387.32280622140644,251.74643052745472, 0.0));
    positionAlloc4->Add (Vector(296.57468765313035,69.5530603184046, 0.0));
    positionAlloc4->Add (Vector(274.79806942475176,271.67515381266327, 0.0));
    positionAlloc4->Add (Vector(306.732679122136,210.2471837213633, 0.0));
    positionAlloc4->Add (Vector(7.056171351851326,9.500017224692447, 0.0));
    positionAlloc4->Add (Vector(190.979150183158,0.7852977698945107, 0.0));
    positionAlloc4->Add (Vector(43.39521554459598,146.76929427574836, 0.0));
    positionAlloc4->Add (Vector(119.07240897544243,29.10711267709498, 0.0));
    positionAlloc4->Add (Vector(216.12888627188,76.83493922530715, 0.0));
    positionAlloc4->Add (Vector(52.66088475416532,72.41118196371178, 0.0));
    positionAlloc4->Add (Vector(75.46435748849633,110.88128005961036, 0.0));
    positionAlloc4->Add (Vector(292.0369342279906,132.05887700021134, 0.0));
    positionAlloc4->Add (Vector(18.337176501847587,30.9071251147087, 0.0));
    positionAlloc4->Add (Vector(152.20416741746692,267.0963267215073, 0.0));
    positionAlloc4->Add (Vector(255.36027321640256,95.09812483849936, 0.0));
    positionAlloc4->Add (Vector(54.7145218483331,359.2371728257016, 0.0));
    positionAlloc4->Add (Vector(154.21232260241555,274.5646748499005, 0.0));
    positionAlloc4->Add (Vector(323.6781510552492,44.83405952577208, 0.0));
    positionAlloc4->Add (Vector(213.69186723245045,253.40782651736808, 0.0));
    positionAlloc4->Add (Vector(288.477578919869,383.1201743292576, 0.0));
    positionAlloc4->Add (Vector(18.162967910765815,263.59627113150987, 0.0));
    positionAlloc4->Add (Vector(332.306121363621,358.51308937663396, 0.0));
    positionAlloc4->Add (Vector(238.79662913625674,331.11103468107774, 0.0));
    positionAlloc4->Add (Vector(66.96695609303083,393.31243793921396, 0.0));
    positionAlloc4->Add (Vector(67.42501578342312,140.78285360653763, 0.0));
    positionAlloc4->Add (Vector(77.83616435610008,20.264792492793227, 0.0));
    positionAlloc4->Add (Vector(375.2477382672742,184.55784654144765, 0.0));
    positionAlloc4->Add (Vector(192.94786443155928,259.18355963943236, 0.0));
    positionAlloc4->Add (Vector(313.3707171269586,301.7942819933228, 0.0));
    positionAlloc4->Add (Vector(18.697894314344587,362.65319432551934, 0.0));
    positionAlloc4->Add (Vector(20.738562334889423,81.35148883977115, 0.0));
    positionAlloc4->Add (Vector(290.3253608476294,376.89610663731816, 0.0));
    positionAlloc4->Add (Vector(213.8288208049345,13.63432223070129, 0.0));
    positionAlloc4->Add (Vector(127.08130706275074,383.57053993492235, 0.0));
    positionAlloc4->Add (Vector(241.76047653797968,284.7692376192659, 0.0));
    positionAlloc4->Add (Vector(358.79190495871507,287.59697218853137, 0.0));
    positionAlloc4->Add (Vector(60.4331839488725,295.52201091314834, 0.0));
    positionAlloc4->Add (Vector(97.95948200986513,125.93443989000197, 0.0));
    positionAlloc4->Add (Vector(320.98469297748863,396.85485942676337, 0.0));
    positionAlloc4->Add (Vector(148.1658151497966,380.76021577850054, 0.0));
    positionAlloc4->Add (Vector(150.89480776061396,384.772994729826, 0.0));
    positionAlloc4->Add (Vector(173.4054869971161,374.39675702970504, 0.0));
    positionAlloc4->Add (Vector(159.05581194623807,48.85633999357415, 0.0));
    positionAlloc4->Add (Vector(320.853754916185,61.214453653716916, 0.0));
    positionAlloc4->Add (Vector(208.88612862204653,132.5658652090096, 0.0));
    positionAlloc4->Add (Vector(281.7012943106987,226.99064122478302, 0.0));
    positionAlloc4->Add (Vector(317.6061810500456,277.7960494130418, 0.0));
    positionAlloc4->Add (Vector(249.5295000793833,84.78366051710293, 0.0));
    positionAlloc4->Add (Vector(333.93211021905415,241.20705508326128, 0.0));
    positionAlloc4->Add (Vector(130.06960229966649,274.5851162442203, 0.0));
    positionAlloc4->Add (Vector(128.09504604134915,386.8425563559767, 0.0));
    positionAlloc4->Add (Vector(322.58330691034723,9.967374455110534, 0.0));
    positionAlloc4->Add (Vector(309.71132391014027,303.74594364598283, 0.0));
    positionAlloc4->Add (Vector(395.3051586003157,368.059972322465, 0.0));
    positionAlloc4->Add (Vector(45.458221074322005,350.7015632841859, 0.0));
    positionAlloc4->Add (Vector(15.869363075238807,253.16891348652337, 0.0));
    positionAlloc4->Add (Vector(337.72538458048155,239.54768677650097, 0.0));
    positionAlloc4->Add (Vector(309.3516789538814,109.16809120849855, 0.0));
    positionAlloc4->Add (Vector(23.563682477967163,355.0213596630106, 0.0));
    positionAlloc4->Add (Vector(370.7779499470992,173.20123658784036, 0.0));
    positionAlloc4->Add (Vector(186.13539090175334,165.19119014690253, 0.0));
    positionAlloc4->Add (Vector(273.72126539542677,172.86119724519548, 0.0));
    positionAlloc4->Add (Vector(366.481517436891,88.88563272017977, 0.0));
    positionAlloc4->Add (Vector(15.837016359373557,163.36747000921702, 0.0));
    positionAlloc4->Add (Vector(26.794096449386416,330.70829792388037, 0.0));
    positionAlloc4->Add (Vector(373.09500835272945,281.07850281133193, 0.0));
    positionAlloc4->Add (Vector(104.96954684772497,61.546909484777586, 0.0));
    positionAlloc4->Add (Vector(72.19437843455015,230.1015945457022, 0.0));
    positionAlloc4->Add (Vector(288.58940784955143,265.45438982968403, 0.0));
    positionAlloc4->Add (Vector(211.4462500744135,112.73114768270847, 0.0));
    positionAlloc4->Add (Vector(314.649495199727,38.223631533224676, 0.0));
    positionAlloc4->Add (Vector(240.94972485536763,18.055099402460772, 0.0));
    positionAlloc4->Add (Vector(45.81112637551872,181.0405130638561, 0.0));
    positionAlloc4->Add (Vector(56.50433349700572,253.72884940165807, 0.0));
    positionAlloc4->Add (Vector(363.27535195323696,300.1488319838249, 0.0));
    positionAlloc4->Add (Vector(258.6957312406722,250.2226289046179, 0.0));
    positionAlloc4->Add (Vector(295.9276925512387,66.4672741630707, 0.0));
    positionAlloc4->Add (Vector(335.3206021514982,386.2539719186252, 0.0));
    positionAlloc4->Add (Vector(136.5756391046139,231.5841189162963, 0.0));
    positionAlloc4->Add (Vector(65.25650508804301,320.4180663911818, 0.0));
    positionAlloc4->Add (Vector(368.6007101112879,307.99814623163746, 0.0));
    positionAlloc4->Add (Vector(331.1785472605214,241.66525747916393, 0.0));
    positionAlloc4->Add (Vector(175.57406011468166,1.4344219887715948, 0.0));
    positionAlloc4->Add (Vector(215.04554090829674,306.6714868854652, 0.0));
    positionAlloc4->Add (Vector(147.96021351818766,338.0177312966041, 0.0));
    positionAlloc4->Add (Vector(246.59768875612974,327.2333467535849, 0.0));
    positionAlloc4->Add (Vector(50.29965671451539,257.1587235659619, 0.0));
    positionAlloc4->Add (Vector(56.470152444206164,31.709285558555635, 0.0));
    positionAlloc4->Add (Vector(377.8323012283629,33.23546286106125, 0.0));
    positionAlloc4->Add (Vector(253.8302277058305,93.82599679915833, 0.0));
    positionAlloc4->Add (Vector(87.87141214019174,211.4864381368515, 0.0));
    positionAlloc4->Add (Vector(235.50118804229106,339.6228602632161, 0.0));
    positionAlloc4->Add (Vector(236.84924336333654,32.507184973068526, 0.0));
    positionAlloc4->Add (Vector(63.051384805975005,61.93058966563583, 0.0));
    positionAlloc4->Add (Vector(86.47108949298628,4.84043261238285, 0.0));
    positionAlloc4->Add (Vector(327.3199979149555,357.85606729058384, 0.0));
    positionAlloc4->Add (Vector(303.60793151334826,366.73226892174563, 0.0));
    positionAlloc4->Add (Vector(249.4416331319282,306.12670421305234, 0.0));
    positionAlloc4->Add (Vector(391.5718156207434,251.3326737145685, 0.0));
    positionAlloc4->Add (Vector(14.253006404557267,41.66732594543632, 0.0));
    positionAlloc4->Add (Vector(40.30884234774237,240.9800184513986, 0.0));
    positionAlloc4->Add (Vector(296.4535169535299,73.37830288205427, 0.0));
    positionAlloc4->Add (Vector(309.3097869584057,351.63995146120163, 0.0));
    positionAlloc4->Add (Vector(117.91548760333193,365.88012023247643, 0.0));
    positionAlloc4->Add (Vector(310.80724623615157,326.10439913229413, 0.0));
    positionAlloc4->Add (Vector(21.23944808874878,337.1957402426297, 0.0));
    positionAlloc4->Add (Vector(259.57292542002995,246.18103568998907, 0.0));
    positionAlloc4->Add (Vector(272.22410043353483,350.18532436513345, 0.0));
    positionAlloc4->Add (Vector(23.249120633469822,369.7523891223473, 0.0));
    positionAlloc4->Add (Vector(7.85127076159724,29.777238840575528, 0.0));
    positionAlloc4->Add (Vector(230.1315816650081,135.82489834433846, 0.0));
    positionAlloc4->Add (Vector(237.8296353842689,241.62085060697248, 0.0));
    positionAlloc4->Add (Vector(43.18679309600402,196.10383551575302, 0.0));
    positionAlloc4->Add (Vector(14.25453135309862,46.86163083700943, 0.0));
    positionAlloc4->Add (Vector(273.20003066686667,206.56023731669544, 0.0));
    positionAlloc4->Add (Vector(133.81048769749714,39.92837137848095, 0.0));
    positionAlloc4->Add (Vector(120.76363592372621,205.70374547754818, 0.0));
    positionAlloc4->Add (Vector(90.04282339360144,201.89646595228888, 0.0));
    positionAlloc4->Add (Vector(310.52856995517294,175.59832637212813, 0.0));
    positionAlloc4->Add (Vector(9.424442808813271,308.58384547556847, 0.0));
    positionAlloc4->Add (Vector(392.5265418935775,316.7772603612344, 0.0));
    positionAlloc4->Add (Vector(33.55217554833789,150.39596498874, 0.0));
    positionAlloc4->Add (Vector(32.75749678251931,42.9283830563361, 0.0));
    positionAlloc4->Add (Vector(111.98349905172135,12.075221639688039, 0.0));
    positionAlloc4->Add (Vector(71.94014138342291,148.82291052237036, 0.0));
    positionAlloc4->Add (Vector(302.65566978259767,164.44882885677416, 0.0));
    positionAlloc4->Add (Vector(238.11275282687598,295.71913907275496, 0.0));
    positionAlloc4->Add (Vector(61.42767203136037,352.4555765148829, 0.0));
    positionAlloc4->Add (Vector(17.267983494201246,91.39358239277028, 0.0));
    positionAlloc4->Add (Vector(238.64397771284897,220.74849696272375, 0.0));
    positionAlloc4->Add (Vector(97.90523364455139,182.34406121750544, 0.0));
    positionAlloc4->Add (Vector(52.40048698280195,134.3119839707712, 0.0));
    positionAlloc4->Add (Vector(106.40529773116101,38.57723873807317, 0.0));
    positionAlloc4->Add (Vector(223.96395326176722,182.9798575666282, 0.0));
    positionAlloc4->Add (Vector(194.2366699454691,330.7049347159119, 0.0));
    positionAlloc4->Add (Vector(85.95539637996681,62.035731811079216, 0.0));
    positionAlloc4->Add (Vector(316.64742346763154,46.038604647110006, 0.0));
    positionAlloc4->Add (Vector(185.08800769248478,389.8891427576339, 0.0));
    positionAlloc4->Add (Vector(30.620603189869968,299.4372870136936, 0.0));
    positionAlloc4->Add (Vector(231.69986044448353,240.7014072842981, 0.0));
    positionAlloc4->Add (Vector(306.4875763864479,75.69549883280686, 0.0));
    positionAlloc4->Add (Vector(373.39674437154076,15.33793930395908, 0.0));
    positionAlloc4->Add (Vector(246.6798392903741,315.8869495930366, 0.0));
    positionAlloc4->Add (Vector(252.00173152736815,55.64330252027623, 0.0));
    positionAlloc4->Add (Vector(142.39823189004022,295.51815739736327, 0.0));
    positionAlloc4->Add (Vector(379.5215611068707,301.18045744422363, 0.0));
    positionAlloc4->Add (Vector(135.3666881955848,133.6600090510876, 0.0));
    positionAlloc4->Add (Vector(268.9557442789622,95.00981509321203, 0.0));
    positionAlloc4->Add (Vector(204.25464742761284,133.511161307753, 0.0));
    positionAlloc4->Add (Vector(351.99354001813987,232.12521317578867, 0.0));
    positionAlloc4->Add (Vector(204.91551159504166,272.92198865902367, 0.0));
    positionAlloc4->Add (Vector(204.9945431086953,311.5275711694521, 0.0));
    positionAlloc4->Add (Vector(61.7892251430598,215.9331571893623, 0.0));
    positionAlloc4->Add (Vector(263.80373983428393,253.39236084141444, 0.0));
    positionAlloc4->Add (Vector(71.3644056776261,53.02924407325315, 0.0));
    positionAlloc4->Add (Vector(272.8238662482385,365.68504402114837, 0.0));
    positionAlloc4->Add (Vector(236.8033404613681,241.02364546485293, 0.0));
    positionAlloc4->Add (Vector(7.217867715722237,130.03882219032755, 0.0));
    positionAlloc4->Add (Vector(307.98019947907875,272.23575973909686, 0.0));
    positionAlloc4->Add (Vector(20.299288894403,369.7617747695931, 0.0));
    positionAlloc4->Add (Vector(264.4804047554985,385.29718136905694, 0.0));
    positionAlloc4->Add (Vector(370.1795817947447,222.06265820755902, 0.0));
    positionAlloc4->Add (Vector(130.55285903687883,291.430118522357, 0.0));
    positionAlloc4->Add (Vector(136.36018132698376,396.5213091674068, 0.0));
    positionAlloc4->Add (Vector(244.91009191850344,144.21424555168568, 0.0));
    positionAlloc4->Add (Vector(96.25169732204948,209.7785997337783, 0.0));
    positionAlloc4->Add (Vector(379.6149807103603,232.6807965177605, 0.0));
    positionAlloc4->Add (Vector(162.49584112027492,261.03320429908433, 0.0));
    positionAlloc4->Add (Vector(187.52482565145527,152.3599973009548, 0.0));
    positionAlloc4->Add (Vector(347.2332868332116,28.28241361100412, 0.0));
    positionAlloc4->Add (Vector(332.7377808947864,87.02500018531242, 0.0));
    positionAlloc4->Add (Vector(90.05990752256672,292.84301821158823, 0.0));
    positionAlloc4->Add (Vector(366.3329556161998,85.27522235969217, 0.0));
    positionAlloc4->Add (Vector(290.2667289924179,15.287294224070669, 0.0));
    positionAlloc4->Add (Vector(161.28650090945382,350.5861214834652, 0.0));
    positionAlloc4->Add (Vector(174.32885002359689,196.8709847118454, 0.0));
    positionAlloc4->Add (Vector(351.50950348871766,247.56408249855713, 0.0));
    positionAlloc4->Add (Vector(220.87099951559628,394.21649609407, 0.0));
    positionAlloc4->Add (Vector(25.27529048889674,38.1403410557172, 0.0));
    positionAlloc4->Add (Vector(221.77476474984084,362.828623038598, 0.0));
    positionAlloc4->Add (Vector(341.0881359906178,358.15621462276175, 0.0));
    positionAlloc4->Add (Vector(209.46921454108903,152.83439398176282, 0.0));
    positionAlloc4->Add (Vector(241.77426600953046,173.66843915643727, 0.0));
    positionAlloc4->Add (Vector(147.0520283498983,222.75103064228114, 0.0));
    positionAlloc4->Add (Vector(95.58887684298347,271.2134720058764, 0.0));
    positionAlloc4->Add (Vector(213.55873266574204,153.36806057230032, 0.0));
    positionAlloc4->Add (Vector(321.44258410282004,212.26165466175186, 0.0));
    positionAlloc4->Add (Vector(352.8232031934035,372.35847527605983, 0.0));
    positionAlloc4->Add (Vector(289.3969890852027,363.2194000041285, 0.0));
    positionAlloc4->Add (Vector(258.8151809626982,227.5832667458262, 0.0));
    positionAlloc4->Add (Vector(355.5861562633785,123.78286204081297, 0.0));
    positionAlloc4->Add (Vector(4.353012740970819,209.57637940980018, 0.0));
    positionAlloc4->Add (Vector(263.3989116926238,394.23449422926006, 0.0));
    positionAlloc4->Add (Vector(309.11469988942747,155.52964898252992, 0.0));
    positionAlloc4->Add (Vector(4.88651216863496,119.1855080836349, 0.0));
    positionAlloc4->Add (Vector(45.11910331663302,332.38148599126987, 0.0));
    positionAlloc4->Add (Vector(284.5129526549009,318.5148389160577, 0.0));
    positionAlloc4->Add (Vector(307.432403185283,390.020523704585, 0.0));
    positionAlloc4->Add (Vector(212.47680097147708,134.244115850183, 0.0));
    positionAlloc4->Add (Vector(317.5241920424064,208.31964913641437, 0.0));
    positionAlloc4->Add (Vector(227.27084146393182,2.6354676872185845, 0.0));
    positionAlloc4->Add (Vector(355.64296598167186,156.87563460408222, 0.0));
    positionAlloc4->Add (Vector(126.05082403332327,258.5234923901731, 0.0));
    positionAlloc4->Add (Vector(364.06809588427444,209.306140486053, 0.0));
    positionAlloc4->Add (Vector(232.34890879227947,334.15940336050784, 0.0));
    positionAlloc4->Add (Vector(215.02661272036093,50.62203463972352, 0.0));
    positionAlloc4->Add (Vector(313.73525916490246,34.32596903532969, 0.0));
    positionAlloc4->Add (Vector(66.68964310544965,102.53471191180998, 0.0));
    positionAlloc4->Add (Vector(293.1922754149971,101.72730988793681, 0.0));
    positionAlloc4->Add (Vector(21.373705728703385,179.48645767999434, 0.0));
    positionAlloc4->Add (Vector(69.77681047803163,244.2719043811738, 0.0));
    positionAlloc4->Add (Vector(73.10615517307491,142.7870375936969, 0.0));
    positionAlloc4->Add (Vector(285.68968054381355,162.6718247838589, 0.0));
    positionAlloc4->Add (Vector(7.721645507935682,303.57551245851204, 0.0));
    positionAlloc4->Add (Vector(17.765821898826893,395.9016353615689, 0.0));
    positionAlloc4->Add (Vector(100.84982811451306,3.7684834602932593, 0.0));
    positionAlloc4->Add (Vector(58.72595989711344,73.46097578621863, 0.0));
    positionAlloc4->Add (Vector(90.40036303122885,179.96245298447238, 0.0));
    positionAlloc4->Add (Vector(17.09993710104345,180.39583998951207, 0.0));
    positionAlloc4->Add (Vector(73.91094117520822,107.51311669301957, 0.0));
    positionAlloc4->Add (Vector(371.61588745255705,112.0456493913049, 0.0));
    positionAlloc4->Add (Vector(314.13066463086574,207.16998865892174, 0.0));
    positionAlloc4->Add (Vector(110.73315970210409,5.018708732920674, 0.0));
    positionAlloc4->Add (Vector(2.086614576497592,302.94382011052835, 0.0));
    positionAlloc4->Add (Vector(323.3177388870808,116.54264178761555, 0.0));
    positionAlloc4->Add (Vector(387.0482493737733,399.75673656984236, 0.0));
    positionAlloc4->Add (Vector(299.2757638090727,24.165725391412394, 0.0));
    positionAlloc4->Add (Vector(371.38330827431395,157.03646374504828, 0.0));
    positionAlloc4->Add (Vector(282.24428958129783,96.34997617946186, 0.0));
    positionAlloc4->Add (Vector(209.56830101467466,243.24341209636492, 0.0));
    positionAlloc4->Add (Vector(212.6128074836996,261.15528244026626, 0.0));
    positionAlloc4->Add (Vector(302.4937476405318,337.7610506241691, 0.0));
    positionAlloc4->Add (Vector(7.349446309247298,141.53779597693318, 0.0));
    positionAlloc4->Add (Vector(28.58531491455252,78.0159083140842, 0.0));
    positionAlloc4->Add (Vector(14.641464275646543,203.3785415480964, 0.0));
    positionAlloc4->Add (Vector(86.63934315524303,15.487247176730445, 0.0));
    positionAlloc4->Add (Vector(250.7715997284307,350.9176664362405, 0.0));
    positionAlloc4->Add (Vector(83.60394379410386,215.67341025225747, 0.0));
    positionAlloc4->Add (Vector(357.1242831309906,84.54770641920493, 0.0));
    positionAlloc4->Add (Vector(207.74946735751138,397.5930971102576, 0.0));
    positionAlloc4->Add (Vector(339.5473748664361,210.1118872290862, 0.0));
    positionAlloc4->Add (Vector(177.21541378627737,7.426995097524447, 0.0));
    positionAlloc4->Add (Vector(107.80276687306127,15.313295608615451, 0.0));
    positionAlloc4->Add (Vector(180.2225919834069,90.18363742846334, 0.0));
    positionAlloc4->Add (Vector(399.83095350530095,121.72828941507264, 0.0));
    positionAlloc4->Add (Vector(26.882627013759297,361.7569262956876, 0.0));
    positionAlloc4->Add (Vector(30.868649619266762,172.03826158317366, 0.0));
    positionAlloc4->Add (Vector(380.631423809625,180.06245038923618, 0.0));
    positionAlloc4->Add (Vector(168.43150746644721,291.613198511416, 0.0));
    positionAlloc4->Add (Vector(285.21907543413863,127.56075713242821, 0.0));
    positionAlloc4->Add (Vector(389.29293877010826,265.77304790787224, 0.0));
    positionAlloc4->Add (Vector(254.20929400869264,117.25053669293834, 0.0));
    positionAlloc4->Add (Vector(108.11501978118723,55.40417574864329, 0.0));
    positionAlloc4->Add (Vector(10.520755213986277,328.3492646481064, 0.0));
    positionAlloc4->Add (Vector(233.98190376210977,349.1832804218761, 0.0));
    positionAlloc4->Add (Vector(342.93471420961004,142.7395905951558, 0.0));
    positionAlloc4->Add (Vector(171.53857750148114,110.52823273393031, 0.0));
    positionAlloc4->Add (Vector(364.6665341369867,348.8156193721735, 0.0));
    positionAlloc4->Add (Vector(351.85227278387174,126.52270186086679, 0.0));
    positionAlloc4->Add (Vector(15.250898835661397,180.68830256939643, 0.0));
    positionAlloc4->Add (Vector(392.0595131010226,334.2746680469719, 0.0));
    positionAlloc4->Add (Vector(98.76828893273921,49.341222440076216, 0.0));
    positionAlloc4->Add (Vector(127.17861438383112,322.475849883277, 0.0));
    positionAlloc4->Add (Vector(264.1826591055164,391.9929473904607, 0.0));
    positionAlloc4->Add (Vector(195.19590632802982,177.63077105147644, 0.0));
    positionAlloc4->Add (Vector(41.964790739798374,356.66984949965126, 0.0));
    positionAlloc4->Add (Vector(270.6095022617913,256.1386966493128, 0.0));
    positionAlloc4->Add (Vector(62.591062473132595,37.467247541750346, 0.0));
    positionAlloc4->Add (Vector(205.6926789475518,28.50584347012952, 0.0));
    positionAlloc4->Add (Vector(336.6363270112191,341.76068211980686, 0.0));
    positionAlloc4->Add (Vector(27.699226127396813,202.96788676484843, 0.0));
    positionAlloc4->Add (Vector(152.634859980758,366.3568121374039, 0.0));
    positionAlloc4->Add (Vector(97.21817469781575,62.57865331500319, 0.0));
    positionAlloc4->Add (Vector(208.97603935124383,22.9120218365658, 0.0));
    positionAlloc4->Add (Vector(172.995043326372,108.07094404601787, 0.0));
    positionAlloc4->Add (Vector(216.85726694739634,161.72625998310383, 0.0));
    positionAlloc4->Add (Vector(310.0626421634961,180.0606776615973, 0.0));
    positionAlloc4->Add (Vector(155.90357983899418,1.4482608370282701, 0.0));
    positionAlloc4->Add (Vector(374.82697632379035,346.3533676531931, 0.0));
    positionAlloc4->Add (Vector(14.90154477560841,51.79685182330744, 0.0));
    positionAlloc4->Add (Vector(60.52703921932965,239.1815226963481, 0.0));
    positionAlloc4->Add (Vector(80.73288923875145,14.932172861086856, 0.0));
    positionAlloc4->Add (Vector(245.3252716073793,160.94502415948705, 0.0));
    positionAlloc4->Add (Vector(249.1244091759813,272.264170416543, 0.0));
    positionAlloc4->Add (Vector(256.05490960899164,324.6711483727595, 0.0));
    positionAlloc4->Add (Vector(304.3595421078553,321.77182457691214, 0.0));
    positionAlloc4->Add (Vector(360.0714560107078,293.7103280190013, 0.0));
    positionAlloc4->Add (Vector(212.47624546208237,175.11787676867095, 0.0));
    positionAlloc4->Add (Vector(73.7653482965242,98.3775325762036, 0.0));
    positionAlloc4->Add (Vector(359.7234460163233,48.298018615424354, 0.0));
    positionAlloc4->Add (Vector(353.4430777557264,357.72284650906704, 0.0));
    positionAlloc4->Add (Vector(204.17730490617996,185.88956282787385, 0.0));
    positionAlloc4->Add (Vector(190.31389759445108,17.7810217271698, 0.0));
    positionAlloc4->Add (Vector(215.21746216018,245.62575450315651, 0.0));
    positionAlloc4->Add (Vector(364.08198162101553,81.28665358084204, 0.0));
    positionAlloc4->Add (Vector(266.9312001967307,378.3328624526024, 0.0));
    positionAlloc4->Add (Vector(151.33898971231204,62.01609468762559, 0.0));
    positionAlloc4->Add (Vector(171.6812774721154,298.72257990975714, 0.0));
    positionAlloc4->Add (Vector(40.914189631155786,175.80775841743014, 0.0));
    positionAlloc4->Add (Vector(332.7694486587154,252.16259028468326, 0.0));
    positionAlloc4->Add (Vector(55.408816203874345,92.92574616245145, 0.0));
    positionAlloc4->Add (Vector(269.98728298407093,61.089033593548024, 0.0));
    positionAlloc4->Add (Vector(250.9245611094835,197.0228545913086, 0.0));
    positionAlloc4->Add (Vector(296.4559681479683,394.1303223816043, 0.0));
    positionAlloc4->Add (Vector(393.39164300859755,120.39356624558945, 0.0));
    positionAlloc4->Add (Vector(275.5986062657414,286.73542859148665, 0.0));
    positionAlloc4->Add (Vector(180.07724087597268,152.7977071367648, 0.0));
    positionAlloc4->Add (Vector(255.22682420049395,128.8883651401636, 0.0));
    positionAlloc4->Add (Vector(237.55838852320238,4.867030681140472, 0.0));
    positionAlloc4->Add (Vector(368.4208816845855,89.10163190054021, 0.0));
    positionAlloc4->Add (Vector(341.373953799172,61.98918349068987, 0.0));
    positionAlloc4->Add (Vector(356.73631847821014,262.5831330761893, 0.0));
    positionAlloc4->Add (Vector(95.89633442564364,174.1204937136887, 0.0));
    positionAlloc4->Add (Vector(22.22054550389272,232.55925937179418, 0.0));
    positionAlloc4->Add (Vector(389.6587129314901,148.30115185534888, 0.0));
    positionAlloc4->Add (Vector(234.6295593370256,395.9214867529406, 0.0));
    positionAlloc4->Add (Vector(72.24923479361247,206.0880695320368, 0.0));
    positionAlloc4->Add (Vector(152.7505676589187,391.51168622513603, 0.0));
    positionAlloc4->Add (Vector(136.6714355838157,183.2554913556615, 0.0));
    positionAlloc4->Add (Vector(178.75651876274503,233.27153061477594, 0.0));
    positionAlloc4->Add (Vector(178.6663333571645,9.058420748889917, 0.0));
    positionAlloc4->Add (Vector(285.161810900512,242.60435766219342, 0.0));
    positionAlloc4->Add (Vector(169.92573312584423,246.7319368515466, 0.0));
    positionAlloc4->Add (Vector(254.72755310551904,297.8382548994315, 0.0));
    positionAlloc4->Add (Vector(179.0581746971743,367.3206451503971, 0.0));
    positionAlloc4->Add (Vector(393.87697773850215,264.77895796097243, 0.0));
    positionAlloc4->Add (Vector(255.40588342672788,204.62490065416762, 0.0));
    positionAlloc4->Add (Vector(17.848904123210964,382.92210848529, 0.0));
    positionAlloc4->Add (Vector(50.3483418956673,9.948230495767385, 0.0));
    positionAlloc4->Add (Vector(142.68722492380076,42.52127512165988, 0.0));
    positionAlloc4->Add (Vector(26.858751929959766,273.6278614518032, 0.0));
    positionAlloc4->Add (Vector(334.75958813691466,326.17543182058677, 0.0));
    positionAlloc4->Add (Vector(215.50502015008047,231.28940535470468, 0.0));
    positionAlloc4->Add (Vector(226.91504712131493,58.56938592897305, 0.0));
    positionAlloc4->Add (Vector(189.61230403298993,103.83123522842479, 0.0));
    positionAlloc4->Add (Vector(60.89052687887122,196.08966393861945, 0.0));
    positionAlloc4->Add (Vector(266.6101101558767,248.84714883340374, 0.0));
    positionAlloc4->Add (Vector(263.1956306088425,305.43951618988257, 0.0));
    positionAlloc4->Add (Vector(230.32508559953038,65.42499985384826, 0.0));
    positionAlloc4->Add (Vector(235.94340312507822,346.603587057196, 0.0));
    positionAlloc4->Add (Vector(356.8299033488712,207.2213748031897, 0.0));
    positionAlloc4->Add (Vector(181.35927823441227,263.4146248755192, 0.0));
    positionAlloc4->Add (Vector(319.1843520599637,35.736629416029956, 0.0));
    positionAlloc4->Add (Vector(141.55267749975943,89.79586720118516, 0.0));
    positionAlloc4->Add (Vector(185.76543597387288,43.28232275932802, 0.0));
    positionAlloc4->Add (Vector(73.91103265836563,131.7244317489223, 0.0));
    positionAlloc4->Add (Vector(37.03953001485218,83.77829755505175, 0.0));
    positionAlloc4->Add (Vector(251.74829335412753,319.6474602206911, 0.0));
    positionAlloc4->Add (Vector(388.7018245760842,297.47180794397366, 0.0));
    positionAlloc4->Add (Vector(253.8437938788613,218.31586986236604, 0.0));
    positionAlloc4->Add (Vector(225.6217153216249,272.0537784792651, 0.0));
    positionAlloc4->Add (Vector(305.235718765529,137.46617661757998, 0.0));
    positionAlloc4->Add (Vector(195.19112444003306,230.1784012406448, 0.0));
    positionAlloc4->Add (Vector(64.47913418684746,155.86337753695193, 0.0));
    positionAlloc4->Add (Vector(332.4177157573322,274.10441055453737, 0.0));
    positionAlloc4->Add (Vector(233.6146654800019,364.5403507924207, 0.0));
    positionAlloc4->Add (Vector(199.74495234402946,149.11379322572893, 0.0));
    positionAlloc4->Add (Vector(3.513995926604485,61.333826311242, 0.0));
    positionAlloc4->Add (Vector(163.6287107910381,224.06671403337987, 0.0));
    positionAlloc4->Add (Vector(382.7443919630715,105.03776165788467, 0.0));
    positionAlloc4->Add (Vector(336.7410457000636,40.27169748731816, 0.0));
    positionAlloc4->Add (Vector(251.97556660805543,139.9769855523004, 0.0));
    positionAlloc4->Add (Vector(30.388997439836764,232.49612823976636, 0.0));
    positionAlloc4->Add (Vector(122.84337999620271,37.82719616251331, 0.0));
    positionAlloc4->Add (Vector(236.08914782652835,178.5995022117409, 0.0));
    positionAlloc4->Add (Vector(202.90632031902237,20.560922333694975, 0.0));
    positionAlloc4->Add (Vector(396.4620989838005,344.91953613064453, 0.0));
    positionAlloc4->Add (Vector(197.7237029565515,336.2254432425245, 0.0));
    positionAlloc4->Add (Vector(12.145854243393961,141.12322965862646, 0.0));
    positionAlloc4->Add (Vector(112.24827004143006,201.08310975113608, 0.0));
    positionAlloc4->Add (Vector(303.6887386628318,360.4766660918948, 0.0));
    positionAlloc4->Add (Vector(362.05649215247746,13.31226175488629, 0.0));
    positionAlloc4->Add (Vector(318.7919645682976,318.00555995348765, 0.0));
    positionAlloc4->Add (Vector(261.28645911353493,195.56312478590377, 0.0));
    positionAlloc4->Add (Vector(140.85117783656167,73.41148268437077, 0.0));
    positionAlloc4->Add (Vector(204.2695761715475,306.54612088179766, 0.0));
    positionAlloc4->Add (Vector(1.3258456713947986,176.85235213937082, 0.0));
    positionAlloc4->Add (Vector(297.50223525260566,96.48941230882953, 0.0));
    positionAlloc4->Add (Vector(89.5079208086567,375.1612598494106, 0.0));
    positionAlloc4->Add (Vector(180.38830891568676,150.2422262754072, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_19.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_19.flowmon", false, false);
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