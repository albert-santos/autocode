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
	  uint16_t numberOfRrhs = 80;
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
    positionAlloc->Add (Vector (500.0,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (500.0,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (500.0,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (500.0,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (500.0,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,0.0, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,500.0, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,0.0, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,0.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,500.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,0.0, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,0.0, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,500.0, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (0.0,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (0.0,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (0.0,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (0.0,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (0.0,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (0.0,111.11111111111111, 0.0));
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
    positionAlloc4->Add (Vector(366.75386608315813,41.534456409848595, 0.0));
    positionAlloc4->Add (Vector(20.656511589283944,135.39209762624128, 0.0));
    positionAlloc4->Add (Vector(106.20828937766585,198.1158369805081, 0.0));
    positionAlloc4->Add (Vector(480.7361755268498,53.25345126331571, 0.0));
    positionAlloc4->Add (Vector(230.72106063019464,257.7266041438419, 0.0));
    positionAlloc4->Add (Vector(81.86201302069706,218.33358396557523, 0.0));
    positionAlloc4->Add (Vector(377.8998077031481,441.43278387240366, 0.0));
    positionAlloc4->Add (Vector(27.727325874228548,379.9304839933722, 0.0));
    positionAlloc4->Add (Vector(284.9196044357905,26.22982377226818, 0.0));
    positionAlloc4->Add (Vector(451.0249741870033,71.15885348698409, 0.0));
    positionAlloc4->Add (Vector(293.39168383944764,177.06510605495302, 0.0));
    positionAlloc4->Add (Vector(455.21946966073966,392.60637162448074, 0.0));
    positionAlloc4->Add (Vector(469.582815514456,116.16302914885179, 0.0));
    positionAlloc4->Add (Vector(467.95504209000893,225.50075615577552, 0.0));
    positionAlloc4->Add (Vector(463.1451632527242,389.62802638771205, 0.0));
    positionAlloc4->Add (Vector(340.81969236659705,212.5205297603348, 0.0));
    positionAlloc4->Add (Vector(139.97840243481897,282.30672051769704, 0.0));
    positionAlloc4->Add (Vector(280.98707203642704,286.7615300433223, 0.0));
    positionAlloc4->Add (Vector(6.205877005737326,411.9176266204884, 0.0));
    positionAlloc4->Add (Vector(54.97923773306845,487.3329887261476, 0.0));
    positionAlloc4->Add (Vector(299.64506138364266,386.5181671309724, 0.0));
    positionAlloc4->Add (Vector(97.70929056950617,446.02836062988956, 0.0));
    positionAlloc4->Add (Vector(344.41054106355733,66.14845214992738, 0.0));
    positionAlloc4->Add (Vector(486.3739583246807,377.75640718000386, 0.0));
    positionAlloc4->Add (Vector(108.30601203623397,50.653700397824466, 0.0));
    positionAlloc4->Add (Vector(225.77483194336955,145.0127102547929, 0.0));
    positionAlloc4->Add (Vector(240.87841790856106,168.25167006445895, 0.0));
    positionAlloc4->Add (Vector(266.56320224725437,160.22040208735905, 0.0));
    positionAlloc4->Add (Vector(395.12611310394107,20.89783727771427, 0.0));
    positionAlloc4->Add (Vector(354.8445674320792,477.79147458644934, 0.0));
    positionAlloc4->Add (Vector(391.3380614149144,11.469858453567017, 0.0));
    positionAlloc4->Add (Vector(187.18155708579403,148.78606155687118, 0.0));
    positionAlloc4->Add (Vector(78.48869674442544,65.71132188579715, 0.0));
    positionAlloc4->Add (Vector(237.7585374397349,379.88523589734444, 0.0));
    positionAlloc4->Add (Vector(499.31110513610673,209.58842324358795, 0.0));
    positionAlloc4->Add (Vector(13.324840569531371,496.4826614468871, 0.0));
    positionAlloc4->Add (Vector(67.75418697236685,255.92331340217135, 0.0));
    positionAlloc4->Add (Vector(472.57236451070867,430.56975502658577, 0.0));
    positionAlloc4->Add (Vector(67.38239343978009,112.63296347497354, 0.0));
    positionAlloc4->Add (Vector(255.6096182412304,382.8892055466057, 0.0));
    positionAlloc4->Add (Vector(81.86376935456269,407.0603126590247, 0.0));
    positionAlloc4->Add (Vector(168.51496476040967,320.99936861478716, 0.0));
    positionAlloc4->Add (Vector(109.61418661072275,246.05695715172104, 0.0));
    positionAlloc4->Add (Vector(410.5589223884699,8.500219373733742, 0.0));
    positionAlloc4->Add (Vector(162.74466105102243,289.83121569350993, 0.0));
    positionAlloc4->Add (Vector(99.82073002449337,106.93264215645226, 0.0));
    positionAlloc4->Add (Vector(93.45626915036014,177.78273435881775, 0.0));
    positionAlloc4->Add (Vector(347.64603970703337,464.87069950548386, 0.0));
    positionAlloc4->Add (Vector(229.0080892213392,135.67968044165235, 0.0));
    positionAlloc4->Add (Vector(4.9414121863533005,284.89379709641355, 0.0));
    positionAlloc4->Add (Vector(323.0745504147617,223.7871506858894, 0.0));
    positionAlloc4->Add (Vector(119.22396127238888,474.3071272619342, 0.0));
    positionAlloc4->Add (Vector(484.3590548684909,375.6220082271444, 0.0));
    positionAlloc4->Add (Vector(295.9958360090439,79.31965882654907, 0.0));
    positionAlloc4->Add (Vector(331.07099519844684,440.7795966720841, 0.0));
    positionAlloc4->Add (Vector(240.84147111763198,4.422269906892207, 0.0));
    positionAlloc4->Add (Vector(51.020650825128634,38.04376762934586, 0.0));
    positionAlloc4->Add (Vector(327.2691845515812,360.5527893340818, 0.0));
    positionAlloc4->Add (Vector(111.72714316409127,244.96016389904918, 0.0));
    positionAlloc4->Add (Vector(170.43912265458988,476.1724999686562, 0.0));
    positionAlloc4->Add (Vector(93.25341736659732,53.212102827949806, 0.0));
    positionAlloc4->Add (Vector(22.412009377816677,3.204026130626536, 0.0));
    positionAlloc4->Add (Vector(277.8930672635229,41.76230307097584, 0.0));
    positionAlloc4->Add (Vector(77.67598666315045,336.77999948393887, 0.0));
    positionAlloc4->Add (Vector(198.44392301553876,153.7486340819258, 0.0));
    positionAlloc4->Add (Vector(348.86896526987687,302.1487816086347, 0.0));
    positionAlloc4->Add (Vector(220.0251664630601,215.5922468538253, 0.0));
    positionAlloc4->Add (Vector(244.7110726008434,329.25285323123904, 0.0));
    positionAlloc4->Add (Vector(163.90714339298867,392.482677304138, 0.0));
    positionAlloc4->Add (Vector(105.80048416395887,367.11728896877196, 0.0));
    positionAlloc4->Add (Vector(456.94700920970774,167.78632561545044, 0.0));
    positionAlloc4->Add (Vector(247.32267931863916,418.270211090796, 0.0));
    positionAlloc4->Add (Vector(193.5690736663177,191.43798824526027, 0.0));
    positionAlloc4->Add (Vector(490.2044042931212,417.8654552388003, 0.0));
    positionAlloc4->Add (Vector(292.10766126671996,141.19455851124042, 0.0));
    positionAlloc4->Add (Vector(192.40789741638304,224.76391586736065, 0.0));
    positionAlloc4->Add (Vector(364.05512253874355,245.05586446034243, 0.0));
    positionAlloc4->Add (Vector(35.09774218459455,339.16107446839965, 0.0));
    positionAlloc4->Add (Vector(490.33542779073844,356.16322285179405, 0.0));
    positionAlloc4->Add (Vector(264.63016309456384,51.953693666899056, 0.0));
    positionAlloc4->Add (Vector(419.7552573124417,447.1609409031187, 0.0));
    positionAlloc4->Add (Vector(204.39025418339435,363.93651639910803, 0.0));
    positionAlloc4->Add (Vector(325.70652444180286,487.42649808434373, 0.0));
    positionAlloc4->Add (Vector(322.2663305655545,326.18087511021804, 0.0));
    positionAlloc4->Add (Vector(149.89521967289843,363.29382699269433, 0.0));
    positionAlloc4->Add (Vector(188.69555440037345,225.1809317097131, 0.0));
    positionAlloc4->Add (Vector(45.48266609818413,201.02778686139138, 0.0));
    positionAlloc4->Add (Vector(187.356882942868,481.7263764621066, 0.0));
    positionAlloc4->Add (Vector(432.53934632878196,120.83955874851549, 0.0));
    positionAlloc4->Add (Vector(109.52012003466905,269.3235896396627, 0.0));
    positionAlloc4->Add (Vector(94.8273492685543,323.1623025780066, 0.0));
    positionAlloc4->Add (Vector(339.64429330329546,261.1845808316175, 0.0));
    positionAlloc4->Add (Vector(194.84311209564808,156.29802383354442, 0.0));
    positionAlloc4->Add (Vector(458.37616525044814,13.607460670231719, 0.0));
    positionAlloc4->Add (Vector(208.81694815346063,121.34669887423055, 0.0));
    positionAlloc4->Add (Vector(106.30575923702557,51.560664786858226, 0.0));
    positionAlloc4->Add (Vector(494.3865381275035,170.83195303883164, 0.0));
    positionAlloc4->Add (Vector(389.3286683145882,69.12815300885167, 0.0));
    positionAlloc4->Add (Vector(390.9461168271184,11.989240052999339, 0.0));
    positionAlloc4->Add (Vector(128.4288320134142,12.87921761829769, 0.0));
    positionAlloc4->Add (Vector(264.89387435481655,38.462117272923926, 0.0));
    positionAlloc4->Add (Vector(151.79572728127238,204.33913823802723, 0.0));
    positionAlloc4->Add (Vector(309.82585017400095,358.8063128893214, 0.0));
    positionAlloc4->Add (Vector(448.7755118940164,140.9042130597606, 0.0));
    positionAlloc4->Add (Vector(299.58070040431966,495.4474127624138, 0.0));
    positionAlloc4->Add (Vector(286.40443487639664,24.727439493579695, 0.0));
    positionAlloc4->Add (Vector(186.40654745238277,215.80514874933738, 0.0));
    positionAlloc4->Add (Vector(250.36143744018474,122.69616960821223, 0.0));
    positionAlloc4->Add (Vector(245.74225583131954,18.459413315983554, 0.0));
    positionAlloc4->Add (Vector(191.40337033791127,134.3951525493024, 0.0));
    positionAlloc4->Add (Vector(231.1748869465679,82.13635216440046, 0.0));
    positionAlloc4->Add (Vector(18.253430415048953,308.2110557051172, 0.0));
    positionAlloc4->Add (Vector(47.720799296107344,468.84511968301365, 0.0));
    positionAlloc4->Add (Vector(360.41553870843114,73.3277562933885, 0.0));
    positionAlloc4->Add (Vector(226.7846422869955,401.95717788188307, 0.0));
    positionAlloc4->Add (Vector(364.07565010019533,90.17365772494252, 0.0));
    positionAlloc4->Add (Vector(161.96835982142315,451.5562441070382, 0.0));
    positionAlloc4->Add (Vector(357.7265879481036,481.19551390045854, 0.0));
    positionAlloc4->Add (Vector(157.03154120255618,106.67138147964067, 0.0));
    positionAlloc4->Add (Vector(39.583526021029044,104.3980801035535, 0.0));
    positionAlloc4->Add (Vector(395.19649346934807,169.49941931162255, 0.0));
    positionAlloc4->Add (Vector(311.70732971303045,100.51397090682318, 0.0));
    positionAlloc4->Add (Vector(234.17545184428184,28.825721972978446, 0.0));
    positionAlloc4->Add (Vector(441.8295788029875,201.79723506421564, 0.0));
    positionAlloc4->Add (Vector(76.84756762075796,300.3137465173706, 0.0));
    positionAlloc4->Add (Vector(30.31160554586426,200.4141607994208, 0.0));
    positionAlloc4->Add (Vector(241.92721996186734,165.88148152744608, 0.0));
    positionAlloc4->Add (Vector(409.963603907918,273.9032218680878, 0.0));
    positionAlloc4->Add (Vector(169.5312791405553,333.23070883918444, 0.0));
    positionAlloc4->Add (Vector(429.49984755855314,240.34802604948823, 0.0));
    positionAlloc4->Add (Vector(329.83590353228277,185.4804141129659, 0.0));
    positionAlloc4->Add (Vector(279.009315300927,255.61142113534996, 0.0));
    positionAlloc4->Add (Vector(24.17346940398729,496.4214291662626, 0.0));
    positionAlloc4->Add (Vector(76.41403399094743,87.37377708819899, 0.0));
    positionAlloc4->Add (Vector(135.47978358921526,104.94849853804172, 0.0));
    positionAlloc4->Add (Vector(239.558347110859,270.8399625015475, 0.0));
    positionAlloc4->Add (Vector(141.53585086099406,438.7776074646926, 0.0));
    positionAlloc4->Add (Vector(90.46182723070079,293.47930209443143, 0.0));
    positionAlloc4->Add (Vector(434.1865022148395,145.0078050722156, 0.0));
    positionAlloc4->Add (Vector(488.1218388293861,197.74263725572516, 0.0));
    positionAlloc4->Add (Vector(427.4642848173816,210.46327234458252, 0.0));
    positionAlloc4->Add (Vector(212.0631671580943,216.09622567934454, 0.0));
    positionAlloc4->Add (Vector(443.0689996134908,287.854585165774, 0.0));
    positionAlloc4->Add (Vector(265.4543474685044,300.8838910427615, 0.0));
    positionAlloc4->Add (Vector(233.48710495993947,248.47523165324108, 0.0));
    positionAlloc4->Add (Vector(188.7769054643143,459.0623239624975, 0.0));
    positionAlloc4->Add (Vector(194.90839558533585,205.17226595529615, 0.0));
    positionAlloc4->Add (Vector(68.89567305718397,23.87795980709406, 0.0));
    positionAlloc4->Add (Vector(32.48907763576031,261.7522577787814, 0.0));
    positionAlloc4->Add (Vector(325.7065549478492,148.18354535395605, 0.0));
    positionAlloc4->Add (Vector(350.4271197263862,168.41836655881465, 0.0));
    positionAlloc4->Add (Vector(125.6233859658869,432.1213667450821, 0.0));
    positionAlloc4->Add (Vector(413.62816133083464,4.9317804941901855, 0.0));
    positionAlloc4->Add (Vector(111.28317302891283,230.16124538197147, 0.0));
    positionAlloc4->Add (Vector(277.43160878818674,44.3357858184939, 0.0));
    positionAlloc4->Add (Vector(485.44815531468055,177.2740913037405, 0.0));
    positionAlloc4->Add (Vector(432.8853274127586,442.2978462656231, 0.0));
    positionAlloc4->Add (Vector(91.51975894595132,404.04033373751145, 0.0));
    positionAlloc4->Add (Vector(1.1414104219212295,327.8666469193989, 0.0));
    positionAlloc4->Add (Vector(58.1596721916845,480.10714425929797, 0.0));
    positionAlloc4->Add (Vector(359.68689485975426,361.8859801576682, 0.0));
    positionAlloc4->Add (Vector(493.0702172126806,168.54908051653072, 0.0));
    positionAlloc4->Add (Vector(94.0374621923138,123.156988337078, 0.0));
    positionAlloc4->Add (Vector(494.3222042964487,238.80660804490327, 0.0));
    positionAlloc4->Add (Vector(192.69588216554718,459.0422611607412, 0.0));
    positionAlloc4->Add (Vector(26.632917007736843,88.61289917875604, 0.0));
    positionAlloc4->Add (Vector(10.811865529302022,468.99025701676646, 0.0));
    positionAlloc4->Add (Vector(174.83574220430637,328.9582372945515, 0.0));
    positionAlloc4->Add (Vector(298.52678284944824,189.55157471643668, 0.0));
    positionAlloc4->Add (Vector(488.4628569832307,218.2784440167747, 0.0));
    positionAlloc4->Add (Vector(217.61368929218722,205.64598691718456, 0.0));
    positionAlloc4->Add (Vector(494.97230833678594,91.05935443431318, 0.0));
    positionAlloc4->Add (Vector(474.7043614692085,228.3909061218552, 0.0));
    positionAlloc4->Add (Vector(397.9181234921087,468.42170694195346, 0.0));
    positionAlloc4->Add (Vector(82.98011382025372,136.21542613983095, 0.0));
    positionAlloc4->Add (Vector(184.18265431175985,120.17626270709242, 0.0));
    positionAlloc4->Add (Vector(409.4287338520408,388.09997217871444, 0.0));
    positionAlloc4->Add (Vector(61.56167999565798,15.697143569553706, 0.0));
    positionAlloc4->Add (Vector(231.16833578626216,322.3472211760148, 0.0));
    positionAlloc4->Add (Vector(465.87643615237744,327.40673259833096, 0.0));
    positionAlloc4->Add (Vector(100.67564151353986,91.53583745588445, 0.0));
    positionAlloc4->Add (Vector(157.14373128408093,70.44853952937285, 0.0));
    positionAlloc4->Add (Vector(261.222321903099,317.1750622468426, 0.0));
    positionAlloc4->Add (Vector(195.14100419664481,229.58667237181717, 0.0));
    positionAlloc4->Add (Vector(4.85800728288649,84.41023836346379, 0.0));
    positionAlloc4->Add (Vector(483.8926927354099,328.95067898019573, 0.0));
    positionAlloc4->Add (Vector(142.18328144390517,309.87147268589615, 0.0));
    positionAlloc4->Add (Vector(431.4514759581205,487.2241820551603, 0.0));
    positionAlloc4->Add (Vector(21.174115415534267,319.9586815807446, 0.0));
    positionAlloc4->Add (Vector(68.3456323763062,363.2213365419736, 0.0));
    positionAlloc4->Add (Vector(190.1945659017803,443.79552548213184, 0.0));
    positionAlloc4->Add (Vector(430.36239736855214,357.6937096913154, 0.0));
    positionAlloc4->Add (Vector(235.62659950118197,66.56198728844332, 0.0));
    positionAlloc4->Add (Vector(67.01747390397739,372.38818137265304, 0.0));
    positionAlloc4->Add (Vector(373.2711677878677,290.08532306485233, 0.0));
    positionAlloc4->Add (Vector(154.9804403859048,232.91332931355996, 0.0));
    positionAlloc4->Add (Vector(57.428413829864574,161.14578490178917, 0.0));
    positionAlloc4->Add (Vector(192.50939699754682,263.3806894237682, 0.0));
    positionAlloc4->Add (Vector(425.928379044446,24.20487604193594, 0.0));
    positionAlloc4->Add (Vector(444.847827545437,120.36152904282777, 0.0));
    positionAlloc4->Add (Vector(168.91078411785094,440.64689591784435, 0.0));
    positionAlloc4->Add (Vector(381.64166074177604,396.09336311822835, 0.0));
    positionAlloc4->Add (Vector(112.39162083350934,443.8861662910155, 0.0));
    positionAlloc4->Add (Vector(427.3966116495342,364.2939321437764, 0.0));
    positionAlloc4->Add (Vector(171.85054519182657,279.3745790900803, 0.0));
    positionAlloc4->Add (Vector(62.20823927692476,5.453529801950074, 0.0));
    positionAlloc4->Add (Vector(216.89163047578032,212.64104432077067, 0.0));
    positionAlloc4->Add (Vector(188.12205341745857,250.74433802365513, 0.0));
    positionAlloc4->Add (Vector(151.32121274480164,131.9640491189317, 0.0));
    positionAlloc4->Add (Vector(6.387727221942308,49.62718509306369, 0.0));
    positionAlloc4->Add (Vector(81.45198058133197,142.53504735860784, 0.0));
    positionAlloc4->Add (Vector(456.9155496579139,4.3795640856100215, 0.0));
    positionAlloc4->Add (Vector(168.40310574343042,493.44252920019505, 0.0));
    positionAlloc4->Add (Vector(34.58245506604163,277.80814760561276, 0.0));
    positionAlloc4->Add (Vector(163.6283495634696,180.95170017487638, 0.0));
    positionAlloc4->Add (Vector(170.8911647185828,114.70273112311902, 0.0));
    positionAlloc4->Add (Vector(175.77420595859016,194.00146829854796, 0.0));
    positionAlloc4->Add (Vector(207.8439055954527,86.10860867196874, 0.0));
    positionAlloc4->Add (Vector(45.300799703347664,416.948554275503, 0.0));
    positionAlloc4->Add (Vector(452.0288676620892,234.8412736316235, 0.0));
    positionAlloc4->Add (Vector(491.36524751001514,348.86418021432956, 0.0));
    positionAlloc4->Add (Vector(349.21379199076375,7.378887009559854, 0.0));
    positionAlloc4->Add (Vector(196.43632645321685,470.28122658895654, 0.0));
    positionAlloc4->Add (Vector(155.03785187887016,12.54635539213833, 0.0));
    positionAlloc4->Add (Vector(409.0744566177905,116.56780161345215, 0.0));
    positionAlloc4->Add (Vector(401.040924883841,302.7076125214282, 0.0));
    positionAlloc4->Add (Vector(324.88750983099806,179.54867200397885, 0.0));
    positionAlloc4->Add (Vector(118.42863667589698,183.5562135365653, 0.0));
    positionAlloc4->Add (Vector(148.77670789869762,42.28320864222268, 0.0));
    positionAlloc4->Add (Vector(481.03118542672763,345.478995817098, 0.0));
    positionAlloc4->Add (Vector(365.73831352812846,359.73202665604725, 0.0));
    positionAlloc4->Add (Vector(246.18320402916694,75.9910222368132, 0.0));
    positionAlloc4->Add (Vector(420.7074944055771,479.16872171338485, 0.0));
    positionAlloc4->Add (Vector(86.68314017625883,44.25929044866578, 0.0));
    positionAlloc4->Add (Vector(475.482664396901,329.4916779839546, 0.0));
    positionAlloc4->Add (Vector(335.22294732803516,302.0959777351032, 0.0));
    positionAlloc4->Add (Vector(217.50101963750595,478.3904423864764, 0.0));
    positionAlloc4->Add (Vector(222.46911983153677,197.12181894795765, 0.0));
    positionAlloc4->Add (Vector(71.50874600318569,313.36593854390037, 0.0));
    positionAlloc4->Add (Vector(226.45047788386051,130.2037825123483, 0.0));
    positionAlloc4->Add (Vector(163.58466398303383,43.13125512842686, 0.0));
    positionAlloc4->Add (Vector(218.70409869270492,424.79709438532075, 0.0));
    positionAlloc4->Add (Vector(485.42208913490134,466.37750611706895, 0.0));
    positionAlloc4->Add (Vector(227.99180870961771,19.624372746639196, 0.0));
    positionAlloc4->Add (Vector(357.31348161876605,146.80328215248463, 0.0));
    positionAlloc4->Add (Vector(382.11873854036327,56.00044202551463, 0.0));
    positionAlloc4->Add (Vector(460.1101407078931,65.44919679854672, 0.0));
    positionAlloc4->Add (Vector(203.61116214897723,38.72864283193805, 0.0));
    positionAlloc4->Add (Vector(25.886492825902497,351.96366161565504, 0.0));
    positionAlloc4->Add (Vector(454.3523595305534,173.95918116608195, 0.0));
    positionAlloc4->Add (Vector(446.0898043710637,186.17176164683713, 0.0));
    positionAlloc4->Add (Vector(499.6218998160223,53.34563203296494, 0.0));
    positionAlloc4->Add (Vector(192.13164924884185,27.971107959424423, 0.0));
    positionAlloc4->Add (Vector(197.72786792036857,325.36051584412365, 0.0));
    positionAlloc4->Add (Vector(54.886630648778635,265.44651091618834, 0.0));
    positionAlloc4->Add (Vector(324.21189144042893,65.13509565825049, 0.0));
    positionAlloc4->Add (Vector(395.99211265538287,230.97661429374273, 0.0));
    positionAlloc4->Add (Vector(301.0736627525854,437.69839472483176, 0.0));
    positionAlloc4->Add (Vector(467.8100080495995,207.76207373559814, 0.0));
    positionAlloc4->Add (Vector(95.33812726811364,276.75888651241723, 0.0));
    positionAlloc4->Add (Vector(211.11032454610984,423.8845614477475, 0.0));
    positionAlloc4->Add (Vector(58.006154925791144,496.38628234520974, 0.0));
    positionAlloc4->Add (Vector(231.19336896189995,338.3960963268685, 0.0));
    positionAlloc4->Add (Vector(157.81535939673986,131.69373430838732, 0.0));
    positionAlloc4->Add (Vector(5.913407093144585,373.5850294658121, 0.0));
    positionAlloc4->Add (Vector(91.72390823823146,192.42797804310624, 0.0));
    positionAlloc4->Add (Vector(356.58839712055016,427.1649051912769, 0.0));
    positionAlloc4->Add (Vector(368.88817530943686,331.76799185440706, 0.0));
    positionAlloc4->Add (Vector(215.32424169738928,126.90506725271061, 0.0));
    positionAlloc4->Add (Vector(178.12502513685834,381.3710953631612, 0.0));
    positionAlloc4->Add (Vector(9.690716954713963,397.20726743353646, 0.0));
    positionAlloc4->Add (Vector(182.14925784082715,92.24343439829069, 0.0));
    positionAlloc4->Add (Vector(373.40709108390394,162.19611079602197, 0.0));
    positionAlloc4->Add (Vector(174.53896451677298,26.09448007107784, 0.0));
    positionAlloc4->Add (Vector(138.5516122047824,242.12707611843172, 0.0));
    positionAlloc4->Add (Vector(423.72360102338314,32.7906305613172, 0.0));
    positionAlloc4->Add (Vector(173.70804099932218,487.57585217233617, 0.0));
    positionAlloc4->Add (Vector(137.0580367382202,40.67151629256788, 0.0));
    positionAlloc4->Add (Vector(446.2153063192593,308.5349777317539, 0.0));
    positionAlloc4->Add (Vector(276.4478537159614,227.54699244221604, 0.0));
    positionAlloc4->Add (Vector(239.07114433317312,201.68953034729324, 0.0));
    positionAlloc4->Add (Vector(361.96264673409405,373.9098414449669, 0.0));
    positionAlloc4->Add (Vector(242.25241391364204,150.25119747123867, 0.0));
    positionAlloc4->Add (Vector(24.921337043414315,275.39290974946664, 0.0));
    positionAlloc4->Add (Vector(333.9306789839459,130.3804580155637, 0.0));
    positionAlloc4->Add (Vector(45.504989530533614,338.46811137664764, 0.0));
    positionAlloc4->Add (Vector(394.5633670604243,286.9977355641507, 0.0));
    positionAlloc4->Add (Vector(143.06729954587655,15.083011290191406, 0.0));
    positionAlloc4->Add (Vector(55.289813703265025,79.31639520898592, 0.0));
    positionAlloc4->Add (Vector(158.07028427713942,102.6605280893777, 0.0));
    positionAlloc4->Add (Vector(64.97281704176821,163.8178968552258, 0.0));
    positionAlloc4->Add (Vector(120.46096979542375,468.27296973489473, 0.0));
    positionAlloc4->Add (Vector(385.1700812547202,347.861340014069, 0.0));
    positionAlloc4->Add (Vector(38.88369322947383,12.394343166815924, 0.0));
    positionAlloc4->Add (Vector(285.2206186736332,323.7281426248178, 0.0));
    positionAlloc4->Add (Vector(69.88462662213324,363.45416808046775, 0.0));
    positionAlloc4->Add (Vector(172.33127283092776,130.9935933650161, 0.0));
    positionAlloc4->Add (Vector(65.9188042606879,337.04741513599197, 0.0));
    positionAlloc4->Add (Vector(395.75047340795635,458.85726821764075, 0.0));
    positionAlloc4->Add (Vector(415.7430610360165,96.4166963882303, 0.0));
    positionAlloc4->Add (Vector(484.15350777675803,314.6830381593184, 0.0));
    positionAlloc4->Add (Vector(370.7183595664129,86.94132539800576, 0.0));
    positionAlloc4->Add (Vector(343.4975867809397,339.5939422658291, 0.0));
    positionAlloc4->Add (Vector(383.41584890267,262.80897965170413, 0.0));
    positionAlloc4->Add (Vector(8.820214189814157,11.87502153086556, 0.0));
    positionAlloc4->Add (Vector(238.7239377289475,0.9816222123681384, 0.0));
    positionAlloc4->Add (Vector(54.24401943074497,183.46161784468546, 0.0));
    positionAlloc4->Add (Vector(148.84051121930304,36.38389084636873, 0.0));
    positionAlloc4->Add (Vector(270.16110783985,96.04367403163394, 0.0));
    positionAlloc4->Add (Vector(65.82610594270666,90.51397745463974, 0.0));
    positionAlloc4->Add (Vector(94.33044686062043,138.60160007451293, 0.0));
    positionAlloc4->Add (Vector(365.04616778498826,165.0735962502642, 0.0));
    positionAlloc4->Add (Vector(22.921470627309482,38.63390639338588, 0.0));
    positionAlloc4->Add (Vector(190.25520927183365,333.87040840188416, 0.0));
    positionAlloc4->Add (Vector(319.2003415205032,118.8726560481242, 0.0));
    positionAlloc4->Add (Vector(68.39315231041637,449.046466032127, 0.0));
    positionAlloc4->Add (Vector(192.76540325301943,343.2058435623756, 0.0));
    positionAlloc4->Add (Vector(404.5976888190615,56.042574407215106, 0.0));
    positionAlloc4->Add (Vector(267.1148340405631,316.7597831467101, 0.0));
    positionAlloc4->Add (Vector(360.5969736498363,478.90021791157204, 0.0));
    positionAlloc4->Add (Vector(22.70370988845727,329.49533891438733, 0.0));
    positionAlloc4->Add (Vector(415.38265170452627,448.1413617207924, 0.0));
    positionAlloc4->Add (Vector(298.49578642032094,413.8887933513472, 0.0));
    positionAlloc4->Add (Vector(83.70869511628854,491.6405474240175, 0.0));
    positionAlloc4->Add (Vector(84.2812697292789,175.97856700817204, 0.0));
    positionAlloc4->Add (Vector(97.2952054451251,25.33099061599153, 0.0));
    positionAlloc4->Add (Vector(469.05967283409274,230.69730817680954, 0.0));
    positionAlloc4->Add (Vector(241.1848305394491,323.97944954929045, 0.0));
    positionAlloc4->Add (Vector(391.71339640869826,377.24285249165354, 0.0));
    positionAlloc4->Add (Vector(23.372367892930733,453.31649290689916, 0.0));
    positionAlloc4->Add (Vector(25.923202918611775,101.68936104971394, 0.0));
    positionAlloc4->Add (Vector(362.9067010595367,471.1201332966477, 0.0));
    positionAlloc4->Add (Vector(267.28602600616813,17.04290278837661, 0.0));
    positionAlloc4->Add (Vector(158.85163382843842,479.4631749186529, 0.0));
    positionAlloc4->Add (Vector(302.2005956724746,355.96154702408234, 0.0));
    positionAlloc4->Add (Vector(448.48988119839385,359.49621523566424, 0.0));
    positionAlloc4->Add (Vector(75.54147993609061,369.40251364143546, 0.0));
    positionAlloc4->Add (Vector(122.44935251233142,157.41804986250247, 0.0));
    positionAlloc4->Add (Vector(401.2308662218608,496.06857428345427, 0.0));
    positionAlloc4->Add (Vector(185.20726893724577,475.9502697231257, 0.0));
    positionAlloc4->Add (Vector(188.61850970076745,480.9662434122825, 0.0));
    positionAlloc4->Add (Vector(216.7568587463951,467.9959462871313, 0.0));
    positionAlloc4->Add (Vector(198.81976493279757,61.070424991967684, 0.0));
    positionAlloc4->Add (Vector(401.06719364523127,76.51806706714615, 0.0));
    positionAlloc4->Add (Vector(261.1076607775581,165.707331511262, 0.0));
    positionAlloc4->Add (Vector(352.12661788837335,283.7383015309788, 0.0));
    positionAlloc4->Add (Vector(397.00772631255705,347.2450617663023, 0.0));
    positionAlloc4->Add (Vector(311.9118750992291,105.97957564637866, 0.0));
    positionAlloc4->Add (Vector(417.4151377738177,301.5088188540766, 0.0));
    positionAlloc4->Add (Vector(162.5870028745831,343.2313953052753, 0.0));
    positionAlloc4->Add (Vector(160.11880755168644,483.55319544497087, 0.0));
    positionAlloc4->Add (Vector(403.22913363793407,12.459218068888166, 0.0));
    positionAlloc4->Add (Vector(387.1391548876753,379.68242955747854, 0.0));
    positionAlloc4->Add (Vector(494.1314482503946,460.07496540308125, 0.0));
    positionAlloc4->Add (Vector(56.8227763429025,438.3769541052324, 0.0));
    positionAlloc4->Add (Vector(19.836703844048508,316.46114185815424, 0.0));
    positionAlloc4->Add (Vector(422.15673072560196,299.4346084706262, 0.0));
    positionAlloc4->Add (Vector(386.68959869235175,136.4601140106232, 0.0));
    positionAlloc4->Add (Vector(29.454603097458953,443.77669957876327, 0.0));
    positionAlloc4->Add (Vector(463.472437433874,216.50154573480046, 0.0));
    positionAlloc4->Add (Vector(232.66923862719167,206.48898768362815, 0.0));
    positionAlloc4->Add (Vector(342.1515817442835,216.07649655649436, 0.0));
    positionAlloc4->Add (Vector(458.10189679611375,111.10704090022472, 0.0));
    positionAlloc4->Add (Vector(19.796270449216948,204.20933751152126, 0.0));
    positionAlloc4->Add (Vector(33.49262056173302,413.38537240485044, 0.0));
    positionAlloc4->Add (Vector(466.3687604409118,351.3481285141649, 0.0));
    positionAlloc4->Add (Vector(131.21193355965622,76.93363685597198, 0.0));
    positionAlloc4->Add (Vector(90.24297304318767,287.62699318212776, 0.0));
    positionAlloc4->Add (Vector(360.7367598119393,331.81798728710504, 0.0));
    positionAlloc4->Add (Vector(264.3078125930169,140.91393460338557, 0.0));
    positionAlloc4->Add (Vector(393.3118689996588,47.779539416530845, 0.0));
    positionAlloc4->Add (Vector(301.18715606920955,22.568874253075965, 0.0));
    positionAlloc4->Add (Vector(57.263907969398396,226.30064132982014, 0.0));
    positionAlloc4->Add (Vector(70.63041687125715,317.1610617520726, 0.0));
    positionAlloc4->Add (Vector(454.09418994154623,375.1860399797811, 0.0));
    positionAlloc4->Add (Vector(323.3696640508403,312.77828613077236, 0.0));
    positionAlloc4->Add (Vector(369.90961568904834,83.08409270383838, 0.0));
    positionAlloc4->Add (Vector(419.1507526893727,482.8174648982815, 0.0));
    positionAlloc4->Add (Vector(170.7195488807674,289.4801486453704, 0.0));
    positionAlloc4->Add (Vector(81.57063136005377,400.52258298897726, 0.0));
    positionAlloc4->Add (Vector(460.75088763910986,384.99768278954684, 0.0));
    positionAlloc4->Add (Vector(413.9731840756518,302.0815718489549, 0.0));
    positionAlloc4->Add (Vector(219.46757514335206,1.7930274859644935, 0.0));
    positionAlloc4->Add (Vector(268.80692613537093,383.33935860683147, 0.0));
    positionAlloc4->Add (Vector(184.9502668977346,422.5221641207551, 0.0));
    positionAlloc4->Add (Vector(308.24711094516215,409.0416834419811, 0.0));
    positionAlloc4->Add (Vector(62.874570893144245,321.4484044574524, 0.0));
    positionAlloc4->Add (Vector(70.58769055525771,39.63660694819454, 0.0));
    positionAlloc4->Add (Vector(472.29037653545356,41.54432857632656, 0.0));
    positionAlloc4->Add (Vector(317.28778463228815,117.28249599894791, 0.0));
    positionAlloc4->Add (Vector(109.83926517523967,264.35804767106436, 0.0));
    positionAlloc4->Add (Vector(294.37648505286387,424.52857532902016, 0.0));
    positionAlloc4->Add (Vector(296.0615542041707,40.633981216335656, 0.0));
    positionAlloc4->Add (Vector(78.81423100746876,77.41323708204479, 0.0));
    positionAlloc4->Add (Vector(108.08886186623285,6.050540765478562, 0.0));
    positionAlloc4->Add (Vector(409.1499973936943,447.32008411322977, 0.0));
    positionAlloc4->Add (Vector(379.5099143916853,458.4153361521821, 0.0));
    positionAlloc4->Add (Vector(311.80204141491026,382.6583802663154, 0.0));
    positionAlloc4->Add (Vector(489.4647695259292,314.1658421432106, 0.0));
    positionAlloc4->Add (Vector(17.816258005696582,52.0841574317954, 0.0));
    positionAlloc4->Add (Vector(50.38605293467796,301.22502306424826, 0.0));
    positionAlloc4->Add (Vector(370.5668961919124,91.72287860256783, 0.0));
    positionAlloc4->Add (Vector(386.6372336980071,439.54993932650206, 0.0));
    positionAlloc4->Add (Vector(147.3943595041649,457.35015029059554, 0.0));
    positionAlloc4->Add (Vector(388.50905779518945,407.6304989153677, 0.0));
    positionAlloc4->Add (Vector(26.549310110935977,421.4946753032872, 0.0));
    positionAlloc4->Add (Vector(324.4661567750374,307.72629461248636, 0.0));
    positionAlloc4->Add (Vector(340.28012554191855,437.73165545641683, 0.0));
    positionAlloc4->Add (Vector(29.06140079183728,462.19048640293414, 0.0));
    positionAlloc4->Add (Vector(9.81408845199655,37.221548550719405, 0.0));
    positionAlloc4->Add (Vector(287.66447708126015,169.78112293042307, 0.0));
    positionAlloc4->Add (Vector(297.28704423033616,302.0260632587156, 0.0));
    positionAlloc4->Add (Vector(53.98349137000502,245.1297943946913, 0.0));
    positionAlloc4->Add (Vector(17.818164191373278,58.57703854626178, 0.0));
    positionAlloc4->Add (Vector(341.50003833358335,258.2002966458693, 0.0));
    positionAlloc4->Add (Vector(167.26310962187142,49.910464223101194, 0.0));
    positionAlloc4->Add (Vector(150.95454490465775,257.1296818469352, 0.0));
    positionAlloc4->Add (Vector(112.55352924200179,252.3705824403611, 0.0));
    positionAlloc4->Add (Vector(388.16071244396613,219.49790796516015, 0.0));
    positionAlloc4->Add (Vector(11.780553511016588,385.72980684446054, 0.0));
    positionAlloc4->Add (Vector(490.6581773669718,395.97157545154306, 0.0));
    positionAlloc4->Add (Vector(41.940219435422364,187.994956235925, 0.0));
    positionAlloc4->Add (Vector(40.94687097814914,53.66047882042013, 0.0));
    positionAlloc4->Add (Vector(139.9793738146517,15.094027049610048, 0.0));
    positionAlloc4->Add (Vector(89.92517672927863,186.02863815296294, 0.0));
    positionAlloc4->Add (Vector(378.3195872282471,205.5610360709677, 0.0));
    positionAlloc4->Add (Vector(297.64094103359497,369.64892384094367, 0.0));
    positionAlloc4->Add (Vector(76.78459003920047,440.5694706436036, 0.0));
    positionAlloc4->Add (Vector(21.584979367751554,114.24197799096287, 0.0));
    positionAlloc4->Add (Vector(298.3049721410612,275.9356212034047, 0.0));
    positionAlloc4->Add (Vector(122.38154205568924,227.9300765218818, 0.0));
    positionAlloc4->Add (Vector(65.50060872850244,167.889979963464, 0.0));
    positionAlloc4->Add (Vector(133.00662216395125,48.22154842259147, 0.0));
    positionAlloc4->Add (Vector(279.954941577209,228.72482195828525, 0.0));
    positionAlloc4->Add (Vector(242.79583743183636,413.38116839488987, 0.0));
    positionAlloc4->Add (Vector(107.4442454749585,77.54466476384903, 0.0));
    positionAlloc4->Add (Vector(395.8092793345394,57.548255808887504, 0.0));
    positionAlloc4->Add (Vector(231.36000961560597,487.3614284470424, 0.0));
    positionAlloc4->Add (Vector(38.27575398733746,374.296608767117, 0.0));
    positionAlloc4->Add (Vector(289.62482555560445,300.8767591053726, 0.0));
    positionAlloc4->Add (Vector(383.1094704830599,94.61937354100857, 0.0));
    positionAlloc4->Add (Vector(466.74593046442595,19.17242412994885, 0.0));
    positionAlloc4->Add (Vector(308.3497991129676,394.85868699129577, 0.0));
    positionAlloc4->Add (Vector(315.0021644092102,69.55412815034528, 0.0));
    positionAlloc4->Add (Vector(177.99778986255026,369.39769674670407, 0.0));
    positionAlloc4->Add (Vector(474.40195138358837,376.47557180527957, 0.0));
    positionAlloc4->Add (Vector(169.20836024448099,167.07501131385948, 0.0));
    positionAlloc4->Add (Vector(336.1946803487028,118.76226886651503, 0.0));
    positionAlloc4->Add (Vector(255.31830928451603,166.88895163469124, 0.0));
    positionAlloc4->Add (Vector(439.9919250226749,290.15651646973583, 0.0));
    positionAlloc4->Add (Vector(256.14438949380207,341.15248582377956, 0.0));
    positionAlloc4->Add (Vector(256.2431788858691,389.4094639618151, 0.0));
    positionAlloc4->Add (Vector(77.23653142882475,269.91644648670285, 0.0));
    positionAlloc4->Add (Vector(329.7546747928549,316.74045105176805, 0.0));
    positionAlloc4->Add (Vector(89.20550709703262,66.28655509156644, 0.0));
    positionAlloc4->Add (Vector(341.02983281029816,457.10630502643545, 0.0));
    positionAlloc4->Add (Vector(296.0041755767101,301.27955683106615, 0.0));
    positionAlloc4->Add (Vector(9.022334644652796,162.54852773790947, 0.0));
    positionAlloc4->Add (Vector(384.9752493488485,340.2946996738711, 0.0));
    positionAlloc4->Add (Vector(25.37411111800375,462.2022184619914, 0.0));
    positionAlloc4->Add (Vector(330.60050594437314,481.6214767113212, 0.0));
    positionAlloc4->Add (Vector(462.7244772434309,277.5783227594488, 0.0));
    positionAlloc4->Add (Vector(163.19107379609855,364.2876481529463, 0.0));
    positionAlloc4->Add (Vector(170.45022665872972,495.65163645925855, 0.0));
    positionAlloc4->Add (Vector(306.1376148981293,180.26780693960708, 0.0));
    positionAlloc4->Add (Vector(120.31462165256185,262.2232496672229, 0.0));
    positionAlloc4->Add (Vector(474.5187258879504,290.85099564720065, 0.0));
    positionAlloc4->Add (Vector(203.11980140034365,326.29150537385544, 0.0));
    positionAlloc4->Add (Vector(234.4060320643191,190.4499966261935, 0.0));
    positionAlloc4->Add (Vector(434.0416085415145,35.35301701375515, 0.0));
    positionAlloc4->Add (Vector(415.922226118483,108.78125023164054, 0.0));
    positionAlloc4->Add (Vector(112.5748844032084,366.0537727644853, 0.0));
    positionAlloc4->Add (Vector(457.91619452024975,106.5940279496152, 0.0));
    positionAlloc4->Add (Vector(362.8334112405223,19.109117780088337, 0.0));
    positionAlloc4->Add (Vector(201.60812613681728,438.23265185433155, 0.0));
    positionAlloc4->Add (Vector(217.9110625294961,246.08873088980675, 0.0));
    positionAlloc4->Add (Vector(439.38687936089707,309.4551031231964, 0.0));
    positionAlloc4->Add (Vector(276.0887493944953,492.77062011758755, 0.0));
    positionAlloc4->Add (Vector(31.594113111120926,47.6754263196465, 0.0));
    positionAlloc4->Add (Vector(277.21845593730103,453.53577879824746, 0.0));
    positionAlloc4->Add (Vector(426.3601699882723,447.6952682784522, 0.0));
    positionAlloc4->Add (Vector(261.83651817636127,191.0429924772035, 0.0));
    positionAlloc4->Add (Vector(302.21783251191306,217.0855489455466, 0.0));
    positionAlloc4->Add (Vector(183.81503543737287,278.4387883028514, 0.0));
    positionAlloc4->Add (Vector(119.48609605372934,339.0168400073454, 0.0));
    positionAlloc4->Add (Vector(266.94841583217755,191.7100757153754, 0.0));
    positionAlloc4->Add (Vector(401.80323012852506,265.3270683271898, 0.0));
    positionAlloc4->Add (Vector(441.0290039917544,465.44809409507474, 0.0));
    positionAlloc4->Add (Vector(361.7462363565034,454.02425000516064, 0.0));
    positionAlloc4->Add (Vector(323.5189762033728,284.4790834322827, 0.0));
    positionAlloc4->Add (Vector(444.48269532922313,154.7285775510162, 0.0));
    positionAlloc4->Add (Vector(5.441265926213523,261.9704742622502, 0.0));
    positionAlloc4->Add (Vector(329.2486396157798,492.79311778657507, 0.0));
    positionAlloc4->Add (Vector(386.3933748617843,194.4120612281624, 0.0));
    positionAlloc4->Add (Vector(6.1081402107936995,148.98188510454364, 0.0));
    positionAlloc4->Add (Vector(56.398879145791284,415.47685748908737, 0.0));
    positionAlloc4->Add (Vector(355.64119081862617,398.1435486450722, 0.0));
    positionAlloc4->Add (Vector(384.2905039816038,487.5256546307312, 0.0));
    positionAlloc4->Add (Vector(265.59600121434636,167.80514481272874, 0.0));
    positionAlloc4->Add (Vector(396.905240053008,260.39956142051795, 0.0));
    positionAlloc4->Add (Vector(284.08855182991476,3.2943346090232306, 0.0));
    positionAlloc4->Add (Vector(444.55370747708986,196.0945432551028, 0.0));
    positionAlloc4->Add (Vector(157.56353004165408,323.15436548771635, 0.0));
    positionAlloc4->Add (Vector(455.08511985534307,261.6326756075663, 0.0));
    positionAlloc4->Add (Vector(290.4361359903493,417.6992542006348, 0.0));
    positionAlloc4->Add (Vector(268.78326590045117,63.277543299654404, 0.0));
    positionAlloc4->Add (Vector(392.16907395612805,42.90746129416212, 0.0));
    positionAlloc4->Add (Vector(83.36205388181206,128.16838988976247, 0.0));
    positionAlloc4->Add (Vector(366.49034426874636,127.15913735992102, 0.0));
    positionAlloc4->Add (Vector(26.71713216087923,224.3580720999929, 0.0));
    positionAlloc4->Add (Vector(87.22101309753954,305.3398804764673, 0.0));
    positionAlloc4->Add (Vector(91.38269396634364,178.48379699212114, 0.0));
    positionAlloc4->Add (Vector(357.11210067976697,203.33978097982364, 0.0));
    positionAlloc4->Add (Vector(9.652056884919602,379.46939057314006, 0.0));
    positionAlloc4->Add (Vector(22.207277373533618,494.8770442019611, 0.0));
    positionAlloc4->Add (Vector(126.06228514314132,4.710604325366575, 0.0));
    positionAlloc4->Add (Vector(73.4074498713918,91.82621973277327, 0.0));
    positionAlloc4->Add (Vector(113.00045378903606,224.95306623059048, 0.0));
    positionAlloc4->Add (Vector(21.37492137630431,225.4947999868901, 0.0));
    positionAlloc4->Add (Vector(92.38867646901028,134.39139586627448, 0.0));
    positionAlloc4->Add (Vector(464.5198593156963,140.05706173913111, 0.0));
    positionAlloc4->Add (Vector(392.6633307885822,258.9624858236522, 0.0));
    positionAlloc4->Add (Vector(138.4164496276301,6.273385916150842, 0.0));
    positionAlloc4->Add (Vector(2.60826822062199,378.67977513816044, 0.0));
    positionAlloc4->Add (Vector(404.147173608851,145.67830223451944, 0.0));
    positionAlloc4->Add (Vector(483.8103117172166,499.69592071230295, 0.0));
    positionAlloc4->Add (Vector(374.0947047613409,30.207156739265496, 0.0));
    positionAlloc4->Add (Vector(464.22913534289245,196.29557968131033, 0.0));
    positionAlloc4->Add (Vector(352.80536197662235,120.43747022432733, 0.0));
    positionAlloc4->Add (Vector(261.96037626834334,304.05426512045614, 0.0));
    positionAlloc4->Add (Vector(265.76600935462454,326.44410305033284, 0.0));
    positionAlloc4->Add (Vector(378.1171845506647,422.20131328021137, 0.0));
    positionAlloc4->Add (Vector(9.186807886559123,176.9222449711665, 0.0));
    positionAlloc4->Add (Vector(35.731643643190644,97.51988539260526, 0.0));
    positionAlloc4->Add (Vector(18.30183034455818,254.22317693512048, 0.0));
    positionAlloc4->Add (Vector(108.29917894405378,19.359058970913058, 0.0));
    positionAlloc4->Add (Vector(313.46449966053837,438.64708304530063, 0.0));
    positionAlloc4->Add (Vector(104.50492974262981,269.59176281532183, 0.0));
    positionAlloc4->Add (Vector(446.4053539137383,105.68463302400616, 0.0));
    positionAlloc4->Add (Vector(259.6868341968892,496.991371387822, 0.0));
    positionAlloc4->Add (Vector(424.43421858304515,262.63985903635773, 0.0));
    positionAlloc4->Add (Vector(221.51926723284672,9.283743871905559, 0.0));
    positionAlloc4->Add (Vector(134.7534585913266,19.141619510769313, 0.0));
    positionAlloc4->Add (Vector(225.27823997925861,112.72954678557917, 0.0));
    positionAlloc4->Add (Vector(499.7886918816262,152.1603617688408, 0.0));
    positionAlloc4->Add (Vector(33.60328376719912,452.1961578696095, 0.0));
    positionAlloc4->Add (Vector(38.58581202408345,215.04782697896707, 0.0));
    positionAlloc4->Add (Vector(475.7892797620313,225.0780629865452, 0.0));
    positionAlloc4->Add (Vector(210.53938433305902,364.51649813927, 0.0));
    positionAlloc4->Add (Vector(356.52384429267323,159.45094641553524, 0.0));
    positionAlloc4->Add (Vector(486.61617346263535,332.2163098848403, 0.0));
    positionAlloc4->Add (Vector(317.7616175108658,146.56317086617292, 0.0));
    positionAlloc4->Add (Vector(135.14377472648403,69.25521968580412, 0.0));
    positionAlloc4->Add (Vector(13.150944017482846,410.436580810133, 0.0));
    positionAlloc4->Add (Vector(292.4773797026372,436.4791005273451, 0.0));
    positionAlloc4->Add (Vector(428.6683927620125,178.42448824394475, 0.0));
    positionAlloc4->Add (Vector(214.42322187685141,138.16029091741288, 0.0));
    positionAlloc4->Add (Vector(455.8331676712334,436.0195242152169, 0.0));
    positionAlloc4->Add (Vector(439.8153409798397,158.15337732608347, 0.0));
    positionAlloc4->Add (Vector(19.063623544576746,225.86037821174554, 0.0));
    positionAlloc4->Add (Vector(490.0743913762783,417.84333505871484, 0.0));
    positionAlloc4->Add (Vector(123.46036116592401,61.67652805009527, 0.0));
    positionAlloc4->Add (Vector(158.9732679797889,403.0948123540963, 0.0));
    positionAlloc4->Add (Vector(330.2283238818955,489.99118423807585, 0.0));
    positionAlloc4->Add (Vector(243.9948829100373,222.03846381434556, 0.0));
    positionAlloc4->Add (Vector(52.45598842474797,445.837311874564, 0.0));
    positionAlloc4->Add (Vector(338.2618778272392,320.173370811641, 0.0));
    positionAlloc4->Add (Vector(78.23882809141574,46.83405942718794, 0.0));
    positionAlloc4->Add (Vector(257.11584868443975,35.6323043376619, 0.0));
    positionAlloc4->Add (Vector(420.79540876402393,427.2008526497586, 0.0));
    positionAlloc4->Add (Vector(34.62403265924602,253.70985845606054, 0.0));
    positionAlloc4->Add (Vector(190.7935749759475,457.9460151717549, 0.0));
    positionAlloc4->Add (Vector(121.5227183722697,78.22331664375398, 0.0));
    positionAlloc4->Add (Vector(261.2200491890548,28.64002729570725, 0.0));
    positionAlloc4->Add (Vector(216.243804157965,135.08868005752234, 0.0));
    positionAlloc4->Add (Vector(271.07158368424547,202.1578249788798, 0.0));
    positionAlloc4->Add (Vector(387.5783027043701,225.07584707699664, 0.0));
    positionAlloc4->Add (Vector(194.87947479874273,1.8103260462853377, 0.0));
    positionAlloc4->Add (Vector(468.53372040473795,432.94170956649134, 0.0));
    positionAlloc4->Add (Vector(18.626930969510514,64.7460647791343, 0.0));
    positionAlloc4->Add (Vector(75.65879902416206,298.97690337043514, 0.0));
    positionAlloc4->Add (Vector(100.91611154843932,18.665216076358572, 0.0));
    positionAlloc4->Add (Vector(306.65658950922415,201.18128019935884, 0.0));
    positionAlloc4->Add (Vector(311.40551146997666,340.33021302067874, 0.0));
    positionAlloc4->Add (Vector(320.06863701123956,405.83893546594936, 0.0));
    positionAlloc4->Add (Vector(380.44942763481913,402.21478072114013, 0.0));
    positionAlloc4->Add (Vector(450.0893200133847,367.1379100237516, 0.0));
    positionAlloc4->Add (Vector(265.59530682760294,218.89734596083872, 0.0));
    positionAlloc4->Add (Vector(92.20668537065524,122.97191572025451, 0.0));
    positionAlloc4->Add (Vector(449.65430752040413,60.37252326928044, 0.0));
    positionAlloc4->Add (Vector(441.803847194658,447.1535581363338, 0.0));
    positionAlloc4->Add (Vector(255.22163113272495,232.36195353484229, 0.0));
    positionAlloc4->Add (Vector(237.89237199306385,22.226277158962247, 0.0));
    positionAlloc4->Add (Vector(269.021827700225,307.03219312894566, 0.0));
    positionAlloc4->Add (Vector(455.10247702626936,101.60831697605255, 0.0));
    positionAlloc4->Add (Vector(333.66400024591337,472.91607806575297, 0.0));
    positionAlloc4->Add (Vector(189.17373714039005,77.52011835953199, 0.0));
    positionAlloc4->Add (Vector(214.60159684014423,373.4032248871964, 0.0));
    positionAlloc4->Add (Vector(51.14273703894473,219.7596980217877, 0.0));
    positionAlloc4->Add (Vector(415.9618108233943,315.20323785585407, 0.0));
    positionAlloc4->Add (Vector(69.26102025484293,116.1571827030643, 0.0));
    positionAlloc4->Add (Vector(337.4841037300887,76.36129199193503, 0.0));
    positionAlloc4->Add (Vector(313.6557013868544,246.27856823913575, 0.0));
    positionAlloc4->Add (Vector(370.56996018496034,492.6629029770054, 0.0));
    positionAlloc4->Add (Vector(491.73955376074696,150.49195780698682, 0.0));
    positionAlloc4->Add (Vector(344.49825783217676,358.4192857393583, 0.0));
    positionAlloc4->Add (Vector(225.09655109496583,190.99713392095603, 0.0));
    positionAlloc4->Add (Vector(319.03353025061745,161.1104564252045, 0.0));
    positionAlloc4->Add (Vector(296.947985654003,6.083788351425589, 0.0));
    positionAlloc4->Add (Vector(460.52610210573187,111.37703987567527, 0.0));
    positionAlloc4->Add (Vector(426.717442248965,77.48647936336233, 0.0));
    positionAlloc4->Add (Vector(445.9203980977627,328.22891634523666, 0.0));
    positionAlloc4->Add (Vector(119.87041803205456,217.65061714211086, 0.0));
    positionAlloc4->Add (Vector(27.7756818798659,290.6990742147427, 0.0));
    positionAlloc4->Add (Vector(487.0733911643626,185.37643981918612, 0.0));
    positionAlloc4->Add (Vector(293.286949171282,494.9018584411758, 0.0));
    positionAlloc4->Add (Vector(90.31154349201559,257.610086915046, 0.0));
    positionAlloc4->Add (Vector(190.93820957364838,489.38960778142007, 0.0));
    positionAlloc4->Add (Vector(170.83929447976965,229.06936419457685, 0.0));
    positionAlloc4->Add (Vector(223.4456484534313,291.5894132684699, 0.0));
    positionAlloc4->Add (Vector(223.3329166964556,11.323025936112396, 0.0));
    positionAlloc4->Add (Vector(356.45226362564006,303.2554470777418, 0.0));
    positionAlloc4->Add (Vector(212.40716640730528,308.41492106443326, 0.0));
    positionAlloc4->Add (Vector(318.40944138189883,372.29781862428933, 0.0));
    positionAlloc4->Add (Vector(223.8227183714679,459.15080643799644, 0.0));
    positionAlloc4->Add (Vector(492.34622217312767,330.9736974512155, 0.0));
    positionAlloc4->Add (Vector(319.2573542834098,255.78112581770952, 0.0));
    positionAlloc4->Add (Vector(22.311130154013703,478.6526356066125, 0.0));
    positionAlloc4->Add (Vector(62.93542736958413,12.43528811970923, 0.0));
    positionAlloc4->Add (Vector(178.35903115475094,53.15159390207486, 0.0));
    positionAlloc4->Add (Vector(33.57343991244971,342.034826814754, 0.0));
    positionAlloc4->Add (Vector(418.44948517114335,407.71928977573344, 0.0));
    positionAlloc4->Add (Vector(269.3812751876006,289.11175669338087, 0.0));
    positionAlloc4->Add (Vector(283.64380890164364,73.21173241121632, 0.0));
    positionAlloc4->Add (Vector(237.0153800412374,129.789044035531, 0.0));
    positionAlloc4->Add (Vector(76.11315859858902,245.11207992327434, 0.0));
    positionAlloc4->Add (Vector(333.2626376948459,311.0589360417547, 0.0));
    positionAlloc4->Add (Vector(328.99453826105315,381.7993952373532, 0.0));
    positionAlloc4->Add (Vector(287.906356999413,81.78124981731033, 0.0));
    positionAlloc4->Add (Vector(294.92925390634775,433.25448382149506, 0.0));
    positionAlloc4->Add (Vector(446.03737918608897,259.0267185039871, 0.0));
    positionAlloc4->Add (Vector(226.69909779301534,329.268281094399, 0.0));
    positionAlloc4->Add (Vector(398.98044007495463,44.67078677003744, 0.0));
    positionAlloc4->Add (Vector(176.9408468746993,112.24483400148144, 0.0));
    positionAlloc4->Add (Vector(232.2067949673411,54.102903449160024, 0.0));
    positionAlloc4->Add (Vector(92.38879082295703,164.65553968615288, 0.0));
    positionAlloc4->Add (Vector(46.299412518565234,104.72287194381468, 0.0));
    positionAlloc4->Add (Vector(314.6853666926594,399.55932527586384, 0.0));
    positionAlloc4->Add (Vector(485.87728072010526,371.83975992996704, 0.0));
    positionAlloc4->Add (Vector(317.30474234857667,272.89483732795753, 0.0));
    positionAlloc4->Add (Vector(282.0271441520311,340.06722309908133, 0.0));
    positionAlloc4->Add (Vector(381.5446484569112,171.83272077197498, 0.0));
    positionAlloc4->Add (Vector(243.98890555004132,287.723001550806, 0.0));
    positionAlloc4->Add (Vector(80.59891773355932,194.8292219211899, 0.0));
    positionAlloc4->Add (Vector(415.5221446966652,342.63051319317174, 0.0));
    positionAlloc4->Add (Vector(292.01833185000237,455.6754384905259, 0.0));
    positionAlloc4->Add (Vector(249.68119043003685,186.39224153216117, 0.0));
    positionAlloc4->Add (Vector(4.392494908255607,76.6672828890525, 0.0));
    positionAlloc4->Add (Vector(204.5358884887976,280.08339254172483, 0.0));
    positionAlloc4->Add (Vector(478.4304899538394,131.29720207235584, 0.0));
    positionAlloc4->Add (Vector(420.92630712507946,50.3396218591477, 0.0));
    positionAlloc4->Add (Vector(314.9694582600693,174.9712319403755, 0.0));
    positionAlloc4->Add (Vector(37.98624679979595,290.62016029970795, 0.0));
    positionAlloc4->Add (Vector(153.5542249952534,47.283995203141636, 0.0));
    positionAlloc4->Add (Vector(295.1114347831604,223.24937776467613, 0.0));
    positionAlloc4->Add (Vector(253.63290039877796,25.70115291711872, 0.0));
    positionAlloc4->Add (Vector(495.5776237297506,431.14942016330565, 0.0));
    positionAlloc4->Add (Vector(247.15462869568938,420.2818040531556, 0.0));
    positionAlloc4->Add (Vector(15.182317804242452,176.40403707328306, 0.0));
    positionAlloc4->Add (Vector(140.31033755178757,251.3538871889201, 0.0));
    positionAlloc4->Add (Vector(379.61092332853974,450.59583261486847, 0.0));
    positionAlloc4->Add (Vector(452.5706151905968,16.640327193607863, 0.0));
    positionAlloc4->Add (Vector(398.489955710372,397.5069499418596, 0.0));
    positionAlloc4->Add (Vector(326.60807389191865,244.4539059823797, 0.0));
    positionAlloc4->Add (Vector(176.0639722957021,91.76435335546346, 0.0));
    positionAlloc4->Add (Vector(255.33697021443436,383.18265110224706, 0.0));
    positionAlloc4->Add (Vector(1.6573070892434982,221.06544017421353, 0.0));
    positionAlloc4->Add (Vector(371.8777940657571,120.61176538603691, 0.0));
    positionAlloc4->Add (Vector(111.88490101082088,468.95157481176324, 0.0));
    positionAlloc4->Add (Vector(225.48538614460844,187.80278284425899, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_19.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_ECC_flowmon/switch_ECC_19.flowmon", false, false);
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