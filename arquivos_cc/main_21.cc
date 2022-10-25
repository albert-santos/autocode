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
	  uint16_t numberOfRrhs = 26;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 576;
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
    positionAlloc->Add (Vector (356.6666666666667,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,140.0, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (270.0,400.0, 0.0));
    positionAlloc->Add (Vector (270.0,270.0, 0.0));
    positionAlloc->Add (Vector (270.0,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (270.0,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (270.0,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (140.0,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (140.0,270.0, 0.0));
    positionAlloc->Add (Vector (140.0,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (140.0,140.0, 0.0));
    positionAlloc->Add (Vector (140.0,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,140.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (10.0,270.0, 0.0));
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
    positionAlloc4->Add (Vector(291.5570314378873,200.52628797034097, 0.0));
    positionAlloc4->Add (Vector(114.31579432450133,351.86147944649974, 0.0));
    positionAlloc4->Add (Vector(253.28875138713678,65.9593389175444, 0.0));
    positionAlloc4->Add (Vector(241.65542670699645,267.4464646622495, 0.0));
    positionAlloc4->Add (Vector(252.9721863837788,367.6295177140897, 0.0));
    positionAlloc4->Add (Vector(114.9863469766693,255.20979557903826, 0.0));
    positionAlloc4->Add (Vector(313.18168015297965,250.2015797858106, 0.0));
    positionAlloc4->Add (Vector(264.57221324094576,135.0273320705184, 0.0));
    positionAlloc4->Add (Vector(174.5300692761956,303.3232361006839, 0.0));
    positionAlloc4->Add (Vector(36.322566219431664,361.4991379718645, 0.0));
    positionAlloc4->Add (Vector(335.0696521609685,391.0482008966002, 0.0));
    positionAlloc4->Add (Vector(167.79607803562348,20.920469688618894, 0.0));
    positionAlloc4->Add (Vector(118.08185596829163,244.28343880779244, 0.0));
    positionAlloc4->Add (Vector(83.73282670259331,314.35865926929745, 0.0));
    positionAlloc4->Add (Vector(343.0226502971158,64.0462518957801, 0.0));
    positionAlloc4->Add (Vector(185.87242870238416,186.39136050878298, 0.0));
    positionAlloc4->Add (Vector(96.63127652671055,21.08142323317641, 0.0));
    positionAlloc4->Add (Vector(294.5036483165046,382.02500585714006, 0.0));
    positionAlloc4->Add (Vector(128.30793687679974,350.8316989098552, 0.0));
    positionAlloc4->Add (Vector(100.46453871621233,244.77555839756428, 0.0));
    positionAlloc4->Add (Vector(220.60014696863539,329.68022233324933, 0.0));
    positionAlloc4->Add (Vector(139.60099628754134,118.48968975684909, 0.0));
    positionAlloc4->Add (Vector(201.01096199884907,203.73793117536175, 0.0));
    positionAlloc4->Add (Vector(221.6169888316728,248.91516842416465, 0.0));
    positionAlloc4->Add (Vector(21.854503998538142,329.9096247199463, 0.0));
    positionAlloc4->Add (Vector(177.6562662832359,285.21130771903626, 0.0));
    positionAlloc4->Add (Vector(122.3959684339063,239.80255684716707, 0.0));
    positionAlloc4->Add (Vector(218.4072284145976,190.84537645789345, 0.0));
    positionAlloc4->Add (Vector(245.813046052981,51.552642538695224, 0.0));
    positionAlloc4->Add (Vector(128.30310169598567,381.01500222491217, 0.0));
    positionAlloc4->Add (Vector(361.0049006152083,222.0908925698965, 0.0));
    positionAlloc4->Add (Vector(376.2500439362727,94.76608413121879, 0.0));
    positionAlloc4->Add (Vector(393.73598378459934,177.95611639803656, 0.0));
    positionAlloc4->Add (Vector(199.50748587108427,201.8368213638245, 0.0));
    positionAlloc4->Add (Vector(248.9100933719869,241.8954779566343, 0.0));
    positionAlloc4->Add (Vector(316.7618895464185,384.343659575098, 0.0));
    positionAlloc4->Add (Vector(284.4815401264551,243.06297636456398, 0.0));
    positionAlloc4->Add (Vector(183.60574050672489,314.02869170210835, 0.0));
    positionAlloc4->Add (Vector(280.8311684063439,226.29324559994063, 0.0));
    positionAlloc4->Add (Vector(202.8432027862955,52.88461569616376, 0.0));
    positionAlloc4->Add (Vector(3.368900615298198,286.0586964235379, 0.0));
    positionAlloc4->Add (Vector(163.4235628134575,306.64610927197026, 0.0));
    positionAlloc4->Add (Vector(282.4601318494559,109.8112091942451, 0.0));
    positionAlloc4->Add (Vector(362.3049789445584,195.27739106910437, 0.0));
    positionAlloc4->Add (Vector(130.25384405677843,2.3478127438004925, 0.0));
    positionAlloc4->Add (Vector(354.6904317227934,333.0261593507377, 0.0));
    positionAlloc4->Add (Vector(284.3528313118214,103.53448153543417, 0.0));
    positionAlloc4->Add (Vector(235.9687159945991,6.869181462642837, 0.0));
    positionAlloc4->Add (Vector(293.5641505485621,99.80622262893588, 0.0));
    positionAlloc4->Add (Vector(43.71837797399816,306.02690180993335, 0.0));
    positionAlloc4->Add (Vector(192.23914542111257,221.99131939782805, 0.0));
    positionAlloc4->Add (Vector(289.49837569523436,379.34978808852213, 0.0));
    positionAlloc4->Add (Vector(315.7894620065563,278.5302167273386, 0.0));
    positionAlloc4->Add (Vector(161.16754767921347,312.71033966910676, 0.0));
    positionAlloc4->Add (Vector(224.44183070607644,312.3492608697549, 0.0));
    positionAlloc4->Add (Vector(105.97877948161849,307.90648451194147, 0.0));
    positionAlloc4->Add (Vector(295.12251867439784,338.19928704460784, 0.0));
    positionAlloc4->Add (Vector(135.69251772205405,301.7132479051654, 0.0));
    positionAlloc4->Add (Vector(372.689759656886,229.37568938162286, 0.0));
    positionAlloc4->Add (Vector(270.2422201455945,220.9691751882519, 0.0));
    positionAlloc4->Add (Vector(364.59112287587317,134.79656120289363, 0.0));
    positionAlloc4->Add (Vector(162.95289384932073,386.8021897071891, 0.0));
    positionAlloc4->Add (Vector(150.67115294462124,67.66692648265776, 0.0));
    positionAlloc4->Add (Vector(143.89717336378786,82.63471784835178, 0.0));
    positionAlloc4->Add (Vector(175.47934258840849,44.12450923968208, 0.0));
    positionAlloc4->Add (Vector(90.5233967150477,335.3298494725811, 0.0));
    positionAlloc4->Add (Vector(320.8865746134686,396.46537615388735, 0.0));
    positionAlloc4->Add (Vector(199.93209971501057,178.41782571962509, 0.0));
    positionAlloc4->Add (Vector(212.27406908838176,338.6159846376331, 0.0));
    positionAlloc4->Add (Vector(154.3146313330811,169.9224247030404, 0.0));
    positionAlloc4->Add (Vector(385.3244150730831,70.68886709515114, 0.0));
    positionAlloc4->Add (Vector(183.24708521106166,314.5759387662501, 0.0));
    positionAlloc4->Add (Vector(279.786553100125,307.3467066826426, 0.0));
    positionAlloc4->Add (Vector(165.99587154607997,63.07253968433195, 0.0));
    positionAlloc4->Add (Vector(92.0411730289775,148.17639331978162, 0.0));
    positionAlloc4->Add (Vector(125.11382730726707,132.2174770599824, 0.0));
    positionAlloc4->Add (Vector(223.8892846067861,172.08579884637268, 0.0));
    positionAlloc4->Add (Vector(13.435768025582773,221.2330850473906, 0.0));
    positionAlloc4->Add (Vector(128.01912065919785,365.5360261191915, 0.0));
    positionAlloc4->Add (Vector(19.068123005502446,158.36364929494184, 0.0));
    positionAlloc4->Add (Vector(378.011667034819,172.47871367722126, 0.0));
    positionAlloc4->Add (Vector(336.1842713999738,293.2670123143592, 0.0));
    positionAlloc4->Add (Vector(374.1524399242872,129.59327107040525, 0.0));
    positionAlloc4->Add (Vector(81.19949924060083,44.89777536514992, 0.0));
    positionAlloc4->Add (Vector(360.5853200692821,208.3914975825655, 0.0));
    positionAlloc4->Add (Vector(119.17647980663837,152.50683179312077, 0.0));
    positionAlloc4->Add (Vector(254.36375102795031,142.4511559071922, 0.0));
    positionAlloc4->Add (Vector(142.32357336857854,333.56880909783797, 0.0));
    positionAlloc4->Add (Vector(79.69650355173692,59.225756866214766, 0.0));
    positionAlloc4->Add (Vector(173.1874748290655,325.9454407813484, 0.0));
    positionAlloc4->Add (Vector(87.83170547348038,118.73035463373718, 0.0));
    positionAlloc4->Add (Vector(109.99115136560663,150.47210388824675, 0.0));
    positionAlloc4->Add (Vector(230.16003071514004,291.902672976282, 0.0));
    positionAlloc4->Add (Vector(148.59923843659476,79.33266459495928, 0.0));
    positionAlloc4->Add (Vector(0.4501409986155558,328.5613385122122, 0.0));
    positionAlloc4->Add (Vector(239.35321720309247,106.2468090232969, 0.0));
    positionAlloc4->Add (Vector(241.43635426511696,326.36828973936645, 0.0));
    positionAlloc4->Add (Vector(102.83486408967084,282.6690876953203, 0.0));
    positionAlloc4->Add (Vector(208.79490860359718,296.5394027236985, 0.0));
    positionAlloc4->Add (Vector(61.16904032288155,241.19833731092183, 0.0));
    positionAlloc4->Add (Vector(363.61707838216836,316.1382236327702, 0.0));
    positionAlloc4->Add (Vector(392.2238348290386,284.5140444250554, 0.0));
    positionAlloc4->Add (Vector(172.4883662509597,112.07055820561762, 0.0));
    positionAlloc4->Add (Vector(57.712444157717925,323.0401307461631, 0.0));
    positionAlloc4->Add (Vector(382.11451722130477,219.54291715427203, 0.0));
    positionAlloc4->Add (Vector(139.1708107919508,237.78406564531284, 0.0));
    positionAlloc4->Add (Vector(201.74985279880056,281.86408385147035, 0.0));
    positionAlloc4->Add (Vector(178.26541362602546,367.34063003658395, 0.0));
    positionAlloc4->Add (Vector(158.52467706876507,280.0505663719734, 0.0));
    positionAlloc4->Add (Vector(251.69785707941523,390.245359859696, 0.0));
    positionAlloc4->Add (Vector(189.21904014206712,15.971565353891393, 0.0));
    positionAlloc4->Add (Vector(395.90871626373956,377.97092519227647, 0.0));
    positionAlloc4->Add (Vector(250.38027346624983,272.7156799390387, 0.0));
    positionAlloc4->Add (Vector(235.16562719552803,24.537884505207195, 0.0));
    positionAlloc4->Add (Vector(274.33078387363304,65.75983350750288, 0.0));
    positionAlloc4->Add (Vector(0.7348168730610283,202.66148837808547, 0.0));
    positionAlloc4->Add (Vector(194.60009526397545,222.6791817140676, 0.0));
    positionAlloc4->Add (Vector(109.83258255048791,246.75245643579152, 0.0));
    positionAlloc4->Add (Vector(273.23628680667554,144.81590211924313, 0.0));
    positionAlloc4->Add (Vector(122.52724926051349,368.2709462787792, 0.0));
    positionAlloc4->Add (Vector(352.10308120145504,52.26656773703829, 0.0));
    positionAlloc4->Add (Vector(233.86649277156496,99.13150068594558, 0.0));
    positionAlloc4->Add (Vector(49.24046869701728,69.54054113522604, 0.0));
    positionAlloc4->Add (Vector(22.53316076325089,85.01262678776263, 0.0));
    positionAlloc4->Add (Vector(237.86154333219548,287.68103308251176, 0.0));
    positionAlloc4->Add (Vector(15.389900921606925,20.518674811674664, 0.0));
    positionAlloc4->Add (Vector(305.5419178768869,47.2237228859024, 0.0));
    positionAlloc4->Add (Vector(396.54822368014845,51.67499246784231, 0.0));
    positionAlloc4->Add (Vector(216.55037561780603,333.4501678814459, 0.0));
    positionAlloc4->Add (Vector(276.14959388237776,322.76568597558736, 0.0));
    positionAlloc4->Add (Vector(332.29686489609895,246.60800301218288, 0.0));
    positionAlloc4->Add (Vector(38.495118961156514,254.8310275776096, 0.0));
    positionAlloc4->Add (Vector(314.5095661281658,52.854599951525174, 0.0));
    positionAlloc4->Add (Vector(272.48307270400574,257.39983418134193, 0.0));
    positionAlloc4->Add (Vector(234.95233191549758,389.68675860239887, 0.0));
    positionAlloc4->Add (Vector(254.7121574156001,300.75547374773276, 0.0));
    positionAlloc4->Add (Vector(350.2239811009294,157.30317289296622, 0.0));
    positionAlloc4->Add (Vector(59.42854176172525,69.13773625159219, 0.0));
    positionAlloc4->Add (Vector(312.8215071413142,179.9159080338667, 0.0));
    positionAlloc4->Add (Vector(19.89416748417332,5.562767969490245, 0.0));
    positionAlloc4->Add (Vector(291.22348975626545,57.97467831400995, 0.0));
    positionAlloc4->Add (Vector(389.71243758332383,5.530084353941156, 0.0));
    positionAlloc4->Add (Vector(337.53095163748804,38.20925645409745, 0.0));
    positionAlloc4->Add (Vector(348.9791766728336,209.9778937212494, 0.0));
    positionAlloc4->Add (Vector(93.43862908430526,153.6003709220354, 0.0));
    positionAlloc4->Add (Vector(149.69365736370955,203.17842772767548, 0.0));
    positionAlloc4->Add (Vector(167.86589998207552,304.6708569947772, 0.0));
    positionAlloc4->Add (Vector(341.1370957211478,375.5808927058892, 0.0));
    positionAlloc4->Add (Vector(121.9992382963389,208.83511740191244, 0.0));
    positionAlloc4->Add (Vector(182.14639067416311,26.48383506723566, 0.0));
    positionAlloc4->Add (Vector(371.88396704030833,299.02692286445733, 0.0));
    positionAlloc4->Add (Vector(18.431939626173488,280.3965572191746, 0.0));
    positionAlloc4->Add (Vector(307.62491730067075,201.429028282387, 0.0));
    positionAlloc4->Add (Vector(42.44898856311971,56.82851510761991, 0.0));
    positionAlloc4->Add (Vector(243.70129269311772,211.56807326752323, 0.0));
    positionAlloc4->Add (Vector(52.15112949926826,379.8574029998557, 0.0));
    positionAlloc4->Add (Vector(139.95802808771086,30.508120940565277, 0.0));
    positionAlloc4->Add (Vector(299.0179380680117,274.3810738783455, 0.0));
    positionAlloc4->Add (Vector(225.1972889782576,2.318495887884753, 0.0));
    positionAlloc4->Add (Vector(333.82884576082853,31.395247273539304, 0.0));
    positionAlloc4->Add (Vector(215.8158232893058,271.245494180533, 0.0));
    positionAlloc4->Add (Vector(14.906921050164934,174.9111720327154, 0.0));
    positionAlloc4->Add (Vector(339.3464358662132,258.89066124540375, 0.0));
    positionAlloc4->Add (Vector(170.47149047909448,75.8488849025369, 0.0));
    positionAlloc4->Add (Vector(207.3068716727375,374.40022542318206, 0.0));
    positionAlloc4->Add (Vector(312.63959625631037,376.08451155678716, 0.0));
    positionAlloc4->Add (Vector(38.07784529957825,367.1634383006052, 0.0));
    positionAlloc4->Add (Vector(367.82832787281944,14.186793538390718, 0.0));
    positionAlloc4->Add (Vector(132.90531917528332,399.93368904317276, 0.0));
    positionAlloc4->Add (Vector(199.48919991276628,90.81498278352349, 0.0));
    positionAlloc4->Add (Vector(245.1602590629844,5.94671197698764, 0.0));
    positionAlloc4->Add (Vector(5.215031894622468,263.90359329631167, 0.0));
    positionAlloc4->Add (Vector(138.0211073315719,194.4885922329806, 0.0));
    positionAlloc4->Add (Vector(326.9946960347551,128.54808703637696, 0.0));
    positionAlloc4->Add (Vector(55.32269927985269,396.6852503877759, 0.0));
    positionAlloc4->Add (Vector(117.50957855829327,34.58183220185411, 0.0));
    positionAlloc4->Add (Vector(399.6993072868217,352.5787001934989, 0.0));
    positionAlloc4->Add (Vector(105.32534748947837,298.02833470025894, 0.0));
    positionAlloc4->Add (Vector(302.26917530563423,28.78361077145657, 0.0));
    positionAlloc4->Add (Vector(270.2650782977924,264.80309169036656, 0.0));
    positionAlloc4->Add (Vector(102.56540464059563,45.05294976538927, 0.0));
    positionAlloc4->Add (Vector(358.66758313381916,110.2159936555057, 0.0));
    positionAlloc4->Add (Vector(156.86583502038417,93.73056761826781, 0.0));
    positionAlloc4->Add (Vector(57.07290099049547,283.06942891994765, 0.0));
    positionAlloc4->Add (Vector(58.46816487204522,392.9509172440116, 0.0));
    positionAlloc4->Add (Vector(42.52390114873994,128.61579532182833, 0.0));
    positionAlloc4->Add (Vector(216.76645038075938,30.85681801725544, 0.0));
    positionAlloc4->Add (Vector(126.0544004535618,269.3829309686767, 0.0));
    positionAlloc4->Add (Vector(134.26116248470996,355.8857480506598, 0.0));
    positionAlloc4->Add (Vector(95.39558728128767,344.7695034280368, 0.0));
    positionAlloc4->Add (Vector(278.0788999268391,348.809175580647, 0.0));
    positionAlloc4->Add (Vector(351.10719810683776,232.1733209074171, 0.0));
    positionAlloc4->Add (Vector(310.49474923005624,355.2328552539106, 0.0));
    positionAlloc4->Add (Vector(393.2673887967053,93.06669913144754, 0.0));
    positionAlloc4->Add (Vector(221.8832755139549,383.8457066237348, 0.0));
    positionAlloc4->Add (Vector(392.45954376442677,218.63482619376305, 0.0));
    positionAlloc4->Add (Vector(350.5529621562914,269.93922750914635, 0.0));
    positionAlloc4->Add (Vector(285.64074705991254,323.482174107908, 0.0));
    positionAlloc4->Add (Vector(114.7500886310643,289.93802497750886, 0.0));
    positionAlloc4->Add (Vector(218.82730569688852,357.08440016937175, 0.0));
    positionAlloc4->Add (Vector(8.245880582491028,155.6520116072472, 0.0));
    positionAlloc4->Add (Vector(327.720451678566,363.033155253838, 0.0));
    positionAlloc4->Add (Vector(146.1986573053505,64.38224825379363, 0.0));
    positionAlloc4->Add (Vector(266.70775222645784,399.08467399095144, 0.0));
    positionAlloc4->Add (Vector(282.9502671676413,197.91866227580073, 0.0));
    positionAlloc4->Add (Vector(346.30266761051655,362.17297135484523, 0.0));
    positionAlloc4->Add (Vector(34.122680351316426,126.74808144954066, 0.0));
    positionAlloc4->Add (Vector(280.9528314067244,356.0098471793565, 0.0));
    positionAlloc4->Add (Vector(392.6813627794107,138.96405708578988, 0.0));
    positionAlloc4->Add (Vector(221.72213046711212,77.86708628511296, 0.0));
    positionAlloc4->Add (Vector(296.19356862461865,308.0071196180891, 0.0));
    positionAlloc4->Add (Vector(125.95394615170754,41.14114931242443, 0.0));
    positionAlloc4->Add (Vector(217.15422196108557,386.5098688287988, 0.0));
    positionAlloc4->Add (Vector(337.08519486071623,127.96398128952724, 0.0));
    positionAlloc4->Add (Vector(383.7027725865765,21.755201448127927, 0.0));
    positionAlloc4->Add (Vector(184.78430340534175,348.7526134253134, 0.0));
    positionAlloc4->Add (Vector(205.4658835920878,194.76233265362092, 0.0));
    positionAlloc4->Add (Vector(218.39168781963164,185.1149113948455, 0.0));
    positionAlloc4->Add (Vector(343.8168270440408,37.38836206183449, 0.0));
    positionAlloc4->Add (Vector(378.6931742063634,312.6894755913598, 0.0));
    positionAlloc4->Add (Vector(270.33923705344495,259.9034612101316, 0.0));
    positionAlloc4->Add (Vector(183.62845660390838,345.09032368222165, 0.0));
    positionAlloc4->Add (Vector(255.3189224864893,49.26975763344878, 0.0));
    positionAlloc4->Add (Vector(36.740424506090186,190.59024137813333, 0.0));
    positionAlloc4->Add (Vector(362.22429661111437,20.80339984978501, 0.0));
    positionAlloc4->Add (Vector(370.02753181245646,310.95826249606824, 0.0));
    positionAlloc4->Add (Vector(137.7911424802829,0.786750083456278, 0.0));
    positionAlloc4->Add (Vector(63.40782826652607,388.3342158457064, 0.0));
    positionAlloc4->Add (Vector(76.70244264800372,40.60208712149374, 0.0));
    positionAlloc4->Add (Vector(322.6935522310063,100.85014813997084, 0.0));
    positionAlloc4->Add (Vector(141.4746281081566,105.81703846262602, 0.0));
    positionAlloc4->Add (Vector(238.5522007368453,77.49303644341418, 0.0));
    positionAlloc4->Add (Vector(132.04087207580196,200.37504144635312, 0.0));
    positionAlloc4->Add (Vector(57.9612985237365,288.6580468829536, 0.0));
    positionAlloc4->Add (Vector(96.8419039800518,82.83979921878904, 0.0));
    positionAlloc4->Add (Vector(322.45904751405175,339.9574140402274, 0.0));
    positionAlloc4->Add (Vector(34.71196473852456,190.11022434543415, 0.0));
    positionAlloc4->Add (Vector(119.30170212130848,189.5321731511059, 0.0));
    positionAlloc4->Add (Vector(100.27762348614763,306.81677956186303, 0.0));
    positionAlloc4->Add (Vector(319.15992967858307,327.2660142776424, 0.0));
    positionAlloc4->Add (Vector(53.63301537963068,51.324554642775276, 0.0));
    positionAlloc4->Add (Vector(110.29995882935388,5.113242055079681, 0.0));
    positionAlloc4->Add (Vector(136.48537790585945,261.5772279964073, 0.0));
    positionAlloc4->Add (Vector(363.00570299276467,230.92647800053695, 0.0));
    positionAlloc4->Add (Vector(346.43436561551954,322.97003999360174, 0.0));
    positionAlloc4->Add (Vector(168.76319608169928,237.98078917287194, 0.0));
    positionAlloc4->Add (Vector(342.41782892829417,52.560433633295254, 0.0));
    positionAlloc4->Add (Vector(50.65168698467719,35.80951384240292, 0.0));
    positionAlloc4->Add (Vector(39.330881726295175,340.9387115573201, 0.0));
    positionAlloc4->Add (Vector(152.2698978337786,169.34441678351243, 0.0));
    positionAlloc4->Add (Vector(50.394765256875075,153.15267407539523, 0.0));
    positionAlloc4->Add (Vector(84.14791755602553,324.7315367597638, 0.0));
    positionAlloc4->Add (Vector(243.1413204427293,213.10315088897806, 0.0));
    positionAlloc4->Add (Vector(13.83632299391726,70.32949998328428, 0.0));
    positionAlloc4->Add (Vector(280.60737438537905,374.8189610253449, 0.0));
    positionAlloc4->Add (Vector(280.39579398427554,107.79422687468822, 0.0));
    positionAlloc4->Add (Vector(350.3005702229104,222.3312575146977, 0.0));
    positionAlloc4->Add (Vector(247.67205146589512,306.89189690458477, 0.0));
    positionAlloc4->Add (Vector(161.6201188682431,32.54051002896481, 0.0));
    positionAlloc4->Add (Vector(17.789054261886285,25.117161611582794, 0.0));
    positionAlloc4->Add (Vector(372.5457515735418,342.9966444532747, 0.0));
    positionAlloc4->Add (Vector(174.1464916505048,370.4014055768182, 0.0));
    positionAlloc4->Add (Vector(148.44782337841923,323.11324611488874, 0.0));
    positionAlloc4->Add (Vector(50.13885432434999,91.69176933731835, 0.0));
    positionAlloc4->Add (Vector(130.7325088678176,302.44247976081374, 0.0));
    positionAlloc4->Add (Vector(307.3981969500197,206.90435042574165, 0.0));
    positionAlloc4->Add (Vector(255.531223736426,79.28410068915532, 0.0));
    positionAlloc4->Add (Vector(368.67132002424364,316.19730490489866, 0.0));
    positionAlloc4->Add (Vector(111.9534498805212,219.73754896157308, 0.0));
    positionAlloc4->Add (Vector(123.50523966549649,192.05979698343052, 0.0));
    positionAlloc4->Add (Vector(60.05998820710161,379.6582750773728, 0.0));
    positionAlloc4->Add (Vector(150.89928788057776,132.79664798544758, 0.0));
    positionAlloc4->Add (Vector(55.132699326095704,100.3150293551088, 0.0));
    positionAlloc4->Add (Vector(239.36335568707185,369.19069611158346, 0.0));
    positionAlloc4->Add (Vector(236.01015648868784,230.04724444578756, 0.0));
    positionAlloc4->Add (Vector(198.1882593183617,154.0834043400183, 0.0));
    positionAlloc4->Add (Vector(296.53090171037843,187.6772339616509, 0.0));
    positionAlloc4->Add (Vector(102.59262905328966,201.35434148005663, 0.0));
    positionAlloc4->Add (Vector(342.0865751383272,276.1137890893396, 0.0));
    positionAlloc4->Add (Vector(294.5785963523076,290.583825066513, 0.0));
    positionAlloc4->Add (Vector(377.8475242278032,333.4328426100911, 0.0));
    positionAlloc4->Add (Vector(290.0034790006037,233.50831398816604, 0.0));
    positionAlloc4->Add (Vector(4.07063496155744,168.759830409771, 0.0));
    positionAlloc4->Add (Vector(128.28242821609086,36.31082558797587, 0.0));
    positionAlloc4->Add (Vector(119.54092706596317,273.97618780941957, 0.0));
    positionAlloc4->Add (Vector(106.10925085815248,291.11384311096595, 0.0));
    positionAlloc4->Add (Vector(236.4931762298886,213.5937644144268, 0.0));
    positionAlloc4->Add (Vector(4.515545813491739,347.24564083704047, 0.0));
    positionAlloc4->Add (Vector(20.590093071383066,367.8780800701438, 0.0));
    positionAlloc4->Add (Vector(144.54496202853616,337.35675890394396, 0.0));
    positionAlloc4->Add (Vector(1.7980122724569725,27.148687348305423, 0.0));
    positionAlloc4->Add (Vector(66.0441306881784,217.12335135005185, 0.0));
    positionAlloc4->Add (Vector(377.8030059799183,208.22812926578987, 0.0));
    positionAlloc4->Add (Vector(124.92616519311252,291.93903530551364, 0.0));
    positionAlloc4->Add (Vector(157.28246375617098,201.29663149221574, 0.0));
    positionAlloc4->Add (Vector(350.22230266102866,25.592872810598077, 0.0));
    positionAlloc4->Add (Vector(86.86145090738276,192.05817329889248, 0.0));
    positionAlloc4->Add (Vector(283.91551652336256,397.41216860275273, 0.0));
    positionAlloc4->Add (Vector(81.3245163995549,93.71185756015028, 0.0));
    positionAlloc4->Add (Vector(333.2808794209483,195.5339138232642, 0.0));
    positionAlloc4->Add (Vector(332.3819809081656,46.508697567126276, 0.0));
    positionAlloc4->Add (Vector(129.57747199030902,270.77698192268593, 0.0));
    positionAlloc4->Add (Vector(316.42277463304146,228.58590712756214, 0.0));
    positionAlloc4->Add (Vector(11.875335488737404,217.70198684279646, 0.0));
    positionAlloc4->Add (Vector(343.81123216671114,316.06296050672194, 0.0));
    positionAlloc4->Add (Vector(247.21757199786526,310.2901250074577, 0.0));
    positionAlloc4->Add (Vector(122.46256413280459,128.41882798572465, 0.0));
    positionAlloc4->Add (Vector(320.02268950674653,218.79796647423203, 0.0));
    positionAlloc4->Add (Vector(203.6376802894626,120.45327104533689, 0.0));
    positionAlloc4->Add (Vector(129.76567697362293,250.8907284610757, 0.0));
    positionAlloc4->Add (Vector(316.54498961177126,249.9395322429135, 0.0));
    positionAlloc4->Add (Vector(368.28517286456656,358.71829284708963, 0.0));
    positionAlloc4->Add (Vector(191.96764491396516,120.49191283635264, 0.0));
    positionAlloc4->Add (Vector(89.16609279817038,4.842320742341766, 0.0));
    positionAlloc4->Add (Vector(222.07781983725238,321.34671946005994, 0.0));
    positionAlloc4->Add (Vector(168.28354923177486,23.8646519220286, 0.0));
    positionAlloc4->Add (Vector(275.55206888807464,69.62856290778726, 0.0));
    positionAlloc4->Add (Vector(55.718162971954754,116.75172609067394, 0.0));
    positionAlloc4->Add (Vector(353.63328232542955,302.54141996075396, 0.0));
    positionAlloc4->Add (Vector(119.85639643644679,27.029378238160405, 0.0));
    positionAlloc4->Add (Vector(45.29868237096686,373.464497592927, 0.0));
    positionAlloc4->Add (Vector(20.61737629530338,396.65379530254586, 0.0));
    positionAlloc4->Add (Vector(298.23211837721277,377.5011342234132, 0.0));
    positionAlloc4->Add (Vector(326.3717400817937,10.381749660572615, 0.0));
    positionAlloc4->Add (Vector(247.8805889447345,336.5336954628952, 0.0));
    positionAlloc4->Add (Vector(193.59178644150484,228.50334976194162, 0.0));
    positionAlloc4->Add (Vector(331.4738966556078,151.81254155661313, 0.0));
    positionAlloc4->Add (Vector(99.84766460652996,50.288136681491835, 0.0));
    positionAlloc4->Add (Vector(256.3707675625224,222.18419690756969, 0.0));
    positionAlloc4->Add (Vector(290.7712178814207,380.9387099618067, 0.0));
    positionAlloc4->Add (Vector(21.831241659942346,247.40781393804977, 0.0));
    positionAlloc4->Add (Vector(14.849624880657997,281.61874978534684, 0.0));
    positionAlloc4->Add (Vector(271.0131164199668,314.5553533383254, 0.0));
    positionAlloc4->Add (Vector(140.70790385775794,97.88323156464314, 0.0));
    positionAlloc4->Add (Vector(83.8676838702055,319.7105735287832, 0.0));
    positionAlloc4->Add (Vector(129.26158926423415,309.7310336705342, 0.0));
    positionAlloc4->Add (Vector(19.058378004588494,338.9064084711765, 0.0));
    positionAlloc4->Add (Vector(135.26843203698658,275.1063305127607, 0.0));
    positionAlloc4->Add (Vector(40.54915019368765,293.9636563797296, 0.0));
    positionAlloc4->Add (Vector(39.26986288751877,296.50002287853334, 0.0));
    positionAlloc4->Add (Vector(138.2314167237138,39.52268017313938, 0.0));
    positionAlloc4->Add (Vector(379.1840668742812,352.7312839191173, 0.0));
    positionAlloc4->Add (Vector(2.033592808620499,315.96980022323135, 0.0));
    positionAlloc4->Add (Vector(165.73734964124353,29.183793689922098, 0.0));
    positionAlloc4->Add (Vector(272.7353502843204,395.64605293020344, 0.0));
    positionAlloc4->Add (Vector(344.21499183890626,145.77684873668323, 0.0));
    positionAlloc4->Add (Vector(78.26401002137425,162.96361886307133, 0.0));
    positionAlloc4->Add (Vector(48.6593530447923,379.3751652819179, 0.0));
    positionAlloc4->Add (Vector(267.40942799295516,319.5237898424473, 0.0));
    positionAlloc4->Add (Vector(324.57043398274664,256.39974093125886, 0.0));
    positionAlloc4->Add (Vector(357.7570833890236,358.3460124540501, 0.0));
    positionAlloc4->Add (Vector(214.26955193447927,127.27649599500643, 0.0));
    positionAlloc4->Add (Vector(132.70847062194346,134.5971338714149, 0.0));
    positionAlloc4->Add (Vector(205.72525027397873,398.460211141786, 0.0));
    positionAlloc4->Add (Vector(314.85195093682717,305.6107694314237, 0.0));
    positionAlloc4->Add (Vector(114.17973240238854,309.29009599270694, 0.0));
    positionAlloc4->Add (Vector(347.25562386787385,115.13850133993917, 0.0));
    positionAlloc4->Add (Vector(9.691610960501418,157.0435309849911, 0.0));
    positionAlloc4->Add (Vector(109.29697463716951,373.6844103611392, 0.0));
    positionAlloc4->Add (Vector(39.01598725856745,113.17147559118297, 0.0));
    positionAlloc4->Add (Vector(102.6092783498342,217.215686558692, 0.0));
    positionAlloc4->Add (Vector(231.75450817913114,346.39492687754046, 0.0));
    positionAlloc4->Add (Vector(329.0782383037344,167.91305166754552, 0.0));
    positionAlloc4->Add (Vector(311.35018104183365,22.432414251656763, 0.0));
    positionAlloc4->Add (Vector(274.50441397034297,243.57156461457015, 0.0));
    positionAlloc4->Add (Vector(92.77299332119316,209.44609556166913, 0.0));
    positionAlloc4->Add (Vector(121.99584308736232,88.82830382972533, 0.0));
    positionAlloc4->Add (Vector(362.12685442845685,267.83323104203413, 0.0));
    positionAlloc4->Add (Vector(248.01588965217766,54.38635169145467, 0.0));
    positionAlloc4->Add (Vector(253.2958898465107,104.23134323773193, 0.0));
    positionAlloc4->Add (Vector(71.43177671923513,72.95681416482557, 0.0));
    positionAlloc4->Add (Vector(232.67775300535715,132.32551925787445, 0.0));
    positionAlloc4->Add (Vector(148.48928889803616,292.63823760615423, 0.0));
    positionAlloc4->Add (Vector(398.59891459309057,293.6967843477059, 0.0));
    positionAlloc4->Add (Vector(257.1291060578428,2.927238271634458, 0.0));
    positionAlloc4->Add (Vector(109.0457322398851,347.7731253421241, 0.0));
    positionAlloc4->Add (Vector(100.1436525357982,306.61543268022007, 0.0));
    positionAlloc4->Add (Vector(323.51876054441647,361.0764455942522, 0.0));
    positionAlloc4->Add (Vector(338.0467425077985,311.1049243028229, 0.0));
    positionAlloc4->Add (Vector(237.98519782387478,27.630367411681434, 0.0));
    positionAlloc4->Add (Vector(285.8814447234705,53.56129100799247, 0.0));
    positionAlloc4->Add (Vector(103.38502955567242,234.7179602451678, 0.0));
    positionAlloc4->Add (Vector(240.27831136952798,192.1050588371164, 0.0));
    positionAlloc4->Add (Vector(175.06226630613278,367.6889969969666, 0.0));
    positionAlloc4->Add (Vector(124.9809254337051,231.74453133665008, 0.0));
    positionAlloc4->Add (Vector(21.67794087646393,279.36702910326403, 0.0));
    positionAlloc4->Add (Vector(269.8319618777539,76.24244047534728, 0.0));
    positionAlloc4->Add (Vector(3.3046239504249453,330.31019875964984, 0.0));
    positionAlloc4->Add (Vector(279.82014326487376,76.71235053464787, 0.0));
    positionAlloc4->Add (Vector(160.72026650001362,153.3981283507391, 0.0));
    positionAlloc4->Add (Vector(210.77465785740307,377.73098787818543, 0.0));
    positionAlloc4->Add (Vector(394.1866390630246,142.6892712588033, 0.0));
    positionAlloc4->Add (Vector(175.37248151280878,114.61603023144238, 0.0));
    positionAlloc4->Add (Vector(339.7797191808419,261.1947657400089, 0.0));
    positionAlloc4->Add (Vector(198.16807264912165,356.6992978705818, 0.0));
    positionAlloc4->Add (Vector(379.5030364829671,328.17020949299615, 0.0));
    positionAlloc4->Add (Vector(275.33072844780065,263.45116849669625, 0.0));
    positionAlloc4->Add (Vector(124.6254791741113,252.14955390115756, 0.0));
    positionAlloc4->Add (Vector(388.92504335609914,378.54232043949423, 0.0));
    positionAlloc4->Add (Vector(397.26638934727214,331.7258195109976, 0.0));
    positionAlloc4->Add (Vector(63.683391189431006,127.38843051565713, 0.0));
    positionAlloc4->Add (Vector(116.74368240587066,65.9886948610485, 0.0));
    positionAlloc4->Add (Vector(51.397540366231894,370.5417062770842, 0.0));
    positionAlloc4->Add (Vector(399.89330263776617,143.4928693061842, 0.0));
    positionAlloc4->Add (Vector(175.936977807982,65.94106624360032, 0.0));
    positionAlloc4->Add (Vector(314.5424667019712,177.65254342039646, 0.0));
    positionAlloc4->Add (Vector(69.59099703064906,82.24647522967162, 0.0));
    positionAlloc4->Add (Vector(358.59404566756,127.18888149479879, 0.0));
    positionAlloc4->Add (Vector(340.7424311314494,307.20969838157566, 0.0));
    positionAlloc4->Add (Vector(45.157017998740656,351.7209769521208, 0.0));
    positionAlloc4->Add (Vector(215.3293213212818,351.7364885337069, 0.0));
    positionAlloc4->Add (Vector(352.1566070339236,118.1809622139105, 0.0));
    positionAlloc4->Add (Vector(134.68238696059817,155.0818168581224, 0.0));
    positionAlloc4->Add (Vector(170.1170714664547,91.56507118449694, 0.0));
    positionAlloc4->Add (Vector(95.65835472987261,223.77853477911657, 0.0));
    positionAlloc4->Add (Vector(304.95923184905774,387.6753899792044, 0.0));
    positionAlloc4->Add (Vector(136.23987274593375,203.37367173370865, 0.0));
    positionAlloc4->Add (Vector(98.39013026119297,109.51543892261766, 0.0));
    positionAlloc4->Add (Vector(160.9541552559247,137.02270589688922, 0.0));
    positionAlloc4->Add (Vector(186.25253037693477,279.47092173689174, 0.0));
    positionAlloc4->Add (Vector(195.75626068482484,214.75132869270385, 0.0));
    positionAlloc4->Add (Vector(205.45311497230873,79.45823420933267, 0.0));
    positionAlloc4->Add (Vector(83.23030262211626,18.212973365239815, 0.0));
    positionAlloc4->Add (Vector(77.58072497767503,161.98943575504535, 0.0));
    positionAlloc4->Add (Vector(170.5421592441575,376.3306604456931, 0.0));
    positionAlloc4->Add (Vector(1.926787143474007,96.23901216123154, 0.0));
    positionAlloc4->Add (Vector(17.065657698097823,330.45936454519, 0.0));
    positionAlloc4->Add (Vector(211.3738836516623,167.56545242936346, 0.0));
    positionAlloc4->Add (Vector(315.71976356488676,144.59268772881563, 0.0));
    positionAlloc4->Add (Vector(208.59735227731932,160.26148966845756, 0.0));
    positionAlloc4->Add (Vector(136.3997724866076,173.86896822355453, 0.0));
    positionAlloc4->Add (Vector(15.862141656327555,62.061059430193, 0.0));
    positionAlloc4->Add (Vector(142.9434756631963,77.64995130457488, 0.0));
    positionAlloc4->Add (Vector(189.4915351055522,146.15939282422335, 0.0));
    positionAlloc4->Add (Vector(173.27395315707867,339.63620844771214, 0.0));
    positionAlloc4->Add (Vector(201.7559426188721,115.1834407344055, 0.0));
    positionAlloc4->Add (Vector(393.6765448807966,85.22865501344823, 0.0));
    positionAlloc4->Add (Vector(263.0852214474562,257.0686184332826, 0.0));
    positionAlloc4->Add (Vector(125.8428949533922,382.21601499761294, 0.0));
    positionAlloc4->Add (Vector(159.07038627454443,87.46507419505303, 0.0));
    positionAlloc4->Add (Vector(79.93878826962013,341.6386097263292, 0.0));
    positionAlloc4->Add (Vector(126.58697192882613,286.97368681846024, 0.0));
    positionAlloc4->Add (Vector(376.77082071838817,120.72577955976693, 0.0));
    positionAlloc4->Add (Vector(327.9070846664054,231.65246525613364, 0.0));
    positionAlloc4->Add (Vector(203.72606503613412,50.66587742319268, 0.0));
    positionAlloc4->Add (Vector(266.6550179868865,59.0523637760985, 0.0));
    positionAlloc4->Add (Vector(116.65060403152236,164.01953321902377, 0.0));
    positionAlloc4->Add (Vector(350.30925950640676,96.73707044616084, 0.0));
    positionAlloc4->Add (Vector(44.01854070861222,125.51569373028278, 0.0));
    positionAlloc4->Add (Vector(368.26593811398914,314.6705534618961, 0.0));
    positionAlloc4->Add (Vector(21.137244315013692,190.8569286690962, 0.0));
    positionAlloc4->Add (Vector(197.6680622390964,183.22627917846307, 0.0));
    positionAlloc4->Add (Vector(271.94933851576997,200.24713066464952, 0.0));
    positionAlloc4->Add (Vector(262.51469816431484,113.83789321570634, 0.0));
    positionAlloc4->Add (Vector(54.40012876385288,135.88877195355576, 0.0));
    positionAlloc4->Add (Vector(69.15577094813337,19.489186898436817, 0.0));
    positionAlloc4->Add (Vector(228.01873545203347,244.6682242664699, 0.0));
    positionAlloc4->Add (Vector(103.86761345060194,113.90774992317718, 0.0));
    positionAlloc4->Add (Vector(102.72402280281301,0.0758011244962109, 0.0));
    positionAlloc4->Add (Vector(235.70739807186573,355.39464316027136, 0.0));
    positionAlloc4->Add (Vector(138.3576375694745,384.1465889017504, 0.0));
    positionAlloc4->Add (Vector(125.00241602548866,0.09924387657651579, 0.0));
    positionAlloc4->Add (Vector(55.48640003409284,116.10951154734596, 0.0));
    positionAlloc4->Add (Vector(93.72374557976029,76.43750499401803, 0.0));
    positionAlloc4->Add (Vector(59.719020463874,292.57418432726195, 0.0));
    positionAlloc4->Add (Vector(225.6775912736618,198.3318806882466, 0.0));
    positionAlloc4->Add (Vector(362.9292483511424,80.12100157513666, 0.0));
    positionAlloc4->Add (Vector(295.11010301346624,374.7847754727903, 0.0));
    positionAlloc4->Add (Vector(154.3790404867661,261.01024216308673, 0.0));
    positionAlloc4->Add (Vector(119.10625527197523,386.5240468408014, 0.0));
    positionAlloc4->Add (Vector(252.25535087383096,381.77446822080674, 0.0));
    positionAlloc4->Add (Vector(48.13434304595279,182.93275285381387, 0.0));
    positionAlloc4->Add (Vector(2.264254222083384,276.0850606674522, 0.0));
    positionAlloc4->Add (Vector(64.55510455278679,1.1321056956400621, 0.0));
    positionAlloc4->Add (Vector(267.2413839945653,283.08560185590477, 0.0));
    positionAlloc4->Add (Vector(205.15249426325846,350.8428654653201, 0.0));
    positionAlloc4->Add (Vector(316.8098153316339,69.13740148858354, 0.0));
    positionAlloc4->Add (Vector(64.7603733113848,234.9810710356632, 0.0));
    positionAlloc4->Add (Vector(364.51884055183103,344.71294322119263, 0.0));
    positionAlloc4->Add (Vector(218.90928607035232,8.59326117855983, 0.0));
    positionAlloc4->Add (Vector(176.13631677583803,389.9515713772225, 0.0));
    positionAlloc4->Add (Vector(178.52293496090817,211.65702992706034, 0.0));
    positionAlloc4->Add (Vector(110.52943225965817,158.06374688224537, 0.0));
    positionAlloc4->Add (Vector(316.7640110683692,382.51353740771015, 0.0));
    positionAlloc4->Add (Vector(194.32810188213617,146.86603354747922, 0.0));
    positionAlloc4->Add (Vector(176.77807795994744,225.66406145728695, 0.0));
    positionAlloc4->Add (Vector(61.05629657568783,249.29221962076406, 0.0));
    positionAlloc4->Add (Vector(391.60114707828797,110.47059502657478, 0.0));
    positionAlloc4->Add (Vector(109.74233704506311,52.680860940982875, 0.0));
    positionAlloc4->Add (Vector(62.10561804290209,40.89360124786366, 0.0));
    positionAlloc4->Add (Vector(12.071417819062846,372.2984620829743, 0.0));
    positionAlloc4->Add (Vector(320.0579433719372,316.6230898964578, 0.0));
    positionAlloc4->Add (Vector(7.3156582584323,363.56896126028937, 0.0));
    positionAlloc4->Add (Vector(149.98801643312146,37.130720193186484, 0.0));
    positionAlloc4->Add (Vector(391.67691147682353,382.390621853385, 0.0));
    positionAlloc4->Add (Vector(389.8120214726225,330.7433546115506, 0.0));
    positionAlloc4->Add (Vector(267.26208424522093,203.03915319721236, 0.0));
    positionAlloc4->Add (Vector(253.7847780783641,152.60708722930048, 0.0));
    positionAlloc4->Add (Vector(42.171689912144,253.26641688549486, 0.0));
    positionAlloc4->Add (Vector(221.19963911533782,191.11669205414668, 0.0));
    positionAlloc4->Add (Vector(221.68376141126393,201.20065674433474, 0.0));
    positionAlloc4->Add (Vector(288.2566588735993,177.80618294572258, 0.0));
    positionAlloc4->Add (Vector(334.7665920991715,220.2682859662578, 0.0));
    positionAlloc4->Add (Vector(85.09980969379063,26.85786156858132, 0.0));
    positionAlloc4->Add (Vector(259.6819794794356,342.2840713385341, 0.0));
    positionAlloc4->Add (Vector(117.21877590499838,364.75697817196095, 0.0));
    positionAlloc4->Add (Vector(45.87452895903792,192.57616950862322, 0.0));
    positionAlloc4->Add (Vector(78.96609724419096,249.3338498225487, 0.0));
    positionAlloc4->Add (Vector(280.6978767628295,197.23763786907784, 0.0));
    positionAlloc4->Add (Vector(255.016517672772,350.4343391805781, 0.0));
    positionAlloc4->Add (Vector(203.19032109659423,133.72351603176247, 0.0));
    positionAlloc4->Add (Vector(287.4421692191262,202.24895567433947, 0.0));
    positionAlloc4->Add (Vector(83.53805589328323,92.68212746828333, 0.0));
    positionAlloc4->Add (Vector(302.103920749787,27.494283391528242, 0.0));
    positionAlloc4->Add (Vector(236.94278946428943,154.25536431901716, 0.0));
    positionAlloc4->Add (Vector(190.06708861485842,109.21038878803574, 0.0));
    positionAlloc4->Add (Vector(276.9455475793971,73.96062945240001, 0.0));
    positionAlloc4->Add (Vector(263.99435597805285,382.88901694080124, 0.0));
    positionAlloc4->Add (Vector(365.86088809862076,231.3766846122488, 0.0));
    positionAlloc4->Add (Vector(383.64175728162235,149.30431890331394, 0.0));
    positionAlloc4->Add (Vector(198.83070046649087,283.5373139880285, 0.0));
    positionAlloc4->Add (Vector(293.2686845257615,93.37745356788662, 0.0));
    positionAlloc4->Add (Vector(70.87179083330533,126.36511845859752, 0.0));
    positionAlloc4->Add (Vector(25.1073394520088,340.7439268162042, 0.0));
    positionAlloc4->Add (Vector(240.02945999133348,31.189195163963078, 0.0));
    positionAlloc4->Add (Vector(338.5639951974554,397.55472239531366, 0.0));
    positionAlloc4->Add (Vector(193.35310597621316,368.97326039866954, 0.0));
    positionAlloc4->Add (Vector(258.05703238828437,148.57308222105067, 0.0));
    positionAlloc4->Add (Vector(62.62783960893952,288.28272331193796, 0.0));
    positionAlloc4->Add (Vector(375.6171023938681,222.56885639026308, 0.0));
    positionAlloc4->Add (Vector(29.1138073234265,271.05127080444345, 0.0));
    positionAlloc4->Add (Vector(139.23292087783216,324.7462733739791, 0.0));
    positionAlloc4->Add (Vector(142.87151850272477,52.07719145268119, 0.0));
    positionAlloc4->Add (Vector(1.8981469275577645,97.75621002902591, 0.0));
    positionAlloc4->Add (Vector(108.5880282795233,164.44872912072256, 0.0));
    positionAlloc4->Add (Vector(189.69184876285277,158.15598768607316, 0.0));
    positionAlloc4->Add (Vector(237.9039128014443,37.899094727332105, 0.0));
    positionAlloc4->Add (Vector(85.5860513987003,150.1308268614326, 0.0));
    positionAlloc4->Add (Vector(92.0295244809966,7.123384119138265, 0.0));
    positionAlloc4->Add (Vector(341.88453337094506,375.6874406216222, 0.0));
    positionAlloc4->Add (Vector(144.73265872542493,367.186688870028, 0.0));
    positionAlloc4->Add (Vector(181.19232507909607,286.8361037117538, 0.0));
    positionAlloc4->Add (Vector(244.921331814857,355.5826043128377, 0.0));
    positionAlloc4->Add (Vector(307.41122298634343,225.30817112853373, 0.0));
    positionAlloc4->Add (Vector(269.76395751676336,99.59575551952615, 0.0));
    positionAlloc4->Add (Vector(89.20027613776607,383.21577745108374, 0.0));
    positionAlloc4->Add (Vector(194.66160099264823,340.0438727609967, 0.0));
    positionAlloc4->Add (Vector(168.81403853041124,274.04046751736564, 0.0));
    positionAlloc4->Add (Vector(339.3529479919862,221.68443071110994, 0.0));
    positionAlloc4->Add (Vector(44.16417522204847,387.5328487199019, 0.0));
    positionAlloc4->Add (Vector(86.33546060422756,247.14491327760277, 0.0));
    positionAlloc4->Add (Vector(288.3828425341439,36.548804677179405, 0.0));
    positionAlloc4->Add (Vector(197.75330631490164,330.28033032054714, 0.0));
    positionAlloc4->Add (Vector(224.95964015864067,338.8713809295805, 0.0));
    positionAlloc4->Add (Vector(225.01788079737094,278.61384005399265, 0.0));
    positionAlloc4->Add (Vector(79.07290887527041,12.077015328955776, 0.0));
    positionAlloc4->Add (Vector(147.150960482718,96.48953166142395, 0.0));
    positionAlloc4->Add (Vector(327.4708050458009,333.8482425949338, 0.0));
    positionAlloc4->Add (Vector(219.22628012807473,228.74742131546432, 0.0));
    positionAlloc4->Add (Vector(169.97317812791394,315.94751730434893, 0.0));
    positionAlloc4->Add (Vector(302.31832250966926,354.8207045303989, 0.0));
    positionAlloc4->Add (Vector(237.174726575316,375.21280315417056, 0.0));
    positionAlloc4->Add (Vector(374.48941955195954,270.82584927503433, 0.0));
    positionAlloc4->Add (Vector(250.3441220931614,54.37773201663427, 0.0));
    positionAlloc4->Add (Vector(359.68800039724874,207.38598047339843, 0.0));
    positionAlloc4->Add (Vector(273.31065877197375,61.00747049632762, 0.0));
    positionAlloc4->Add (Vector(278.2670415262263,84.68524988676167, 0.0));
    positionAlloc4->Add (Vector(301.3653016148754,101.08952007897183, 0.0));
    positionAlloc4->Add (Vector(348.10036225719523,354.29057028724174, 0.0));
    positionAlloc4->Add (Vector(158.99029244648912,301.24696917465576, 0.0));
    positionAlloc4->Add (Vector(113.3303400539976,196.7087859373075, 0.0));
    positionAlloc4->Add (Vector(394.1814090922227,169.42648246616127, 0.0));
    positionAlloc4->Add (Vector(43.94603258487808,22.89495945450866, 0.0));
    positionAlloc4->Add (Vector(270.5234131755459,46.92902703425275, 0.0));
    positionAlloc4->Add (Vector(39.223962932816825,121.84085072159014, 0.0));
    positionAlloc4->Add (Vector(45.999950586391236,229.88379018807333, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_21.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_21.flowmon", false, false);
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