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
	  uint16_t numberOfRrhs = 38;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 696;
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
    positionAlloc->Add (Vector (356.6666666666667,270.0, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,10.0, 0.0));
    positionAlloc->Add (Vector (270.0,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,270.0, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,10.0, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,10.0, 0.0));
    positionAlloc->Add (Vector (140.0,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (140.0,270.0, 0.0));
    positionAlloc->Add (Vector (140.0,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (140.0,140.0, 0.0));
    positionAlloc->Add (Vector (140.0,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,270.0, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,10.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,140.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,10.0, 0.0));
    positionAlloc->Add (Vector (10.0,96.66666666666667, 0.0));
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
    positionAlloc4->Add (Vector(352.25545967696223,257.39217707493265, 0.0));
    positionAlloc4->Add (Vector(237.33200658237928,55.99853419147851, 0.0));
    positionAlloc4->Add (Vector(101.06967297559689,13.911858075775285, 0.0));
    positionAlloc4->Add (Vector(368.88537292463957,304.0050665915807, 0.0));
    positionAlloc4->Add (Vector(371.67136277662973,33.176214535831235, 0.0));
    positionAlloc4->Add (Vector(54.13281032536346,326.93782113730447, 0.0));
    positionAlloc4->Add (Vector(269.5420069790482,290.8542670313034, 0.0));
    positionAlloc4->Add (Vector(296.87824490915216,396.2355376515005, 0.0));
    positionAlloc4->Add (Vector(292.79140844482276,182.53334893440615, 0.0));
    positionAlloc4->Add (Vector(313.4923899598419,367.9609362908538, 0.0));
    positionAlloc4->Add (Vector(366.44965312505957,268.5586605725762, 0.0));
    positionAlloc4->Add (Vector(117.21823321553106,257.74365703698226, 0.0));
    positionAlloc4->Add (Vector(95.92320303531281,176.9344215679581, 0.0));
    positionAlloc4->Add (Vector(323.8424075469326,151.4553065947147, 0.0));
    positionAlloc4->Add (Vector(323.53235236744445,228.1731030394221, 0.0));
    positionAlloc4->Add (Vector(303.73139731462834,389.2984652809466, 0.0));
    positionAlloc4->Add (Vector(41.76624001396161,87.05097872051537, 0.0));
    positionAlloc4->Add (Vector(163.64907351637035,347.62737761303345, 0.0));
    positionAlloc4->Add (Vector(72.5600234119605,206.403726611766, 0.0));
    positionAlloc4->Add (Vector(372.53861193574,251.22242326971335, 0.0));
    positionAlloc4->Add (Vector(91.32444085893296,197.41070430294494, 0.0));
    positionAlloc4->Add (Vector(246.8670736708971,89.95996934193795, 0.0));
    positionAlloc4->Add (Vector(336.50717631104067,391.3322167093911, 0.0));
    positionAlloc4->Add (Vector(29.138579960497157,284.3048119795725, 0.0));
    positionAlloc4->Add (Vector(1.168446433965542,113.74542036723328, 0.0));
    positionAlloc4->Add (Vector(129.36564894639724,264.6002747787807, 0.0));
    positionAlloc4->Add (Vector(137.6909944318145,29.263180609478567, 0.0));
    positionAlloc4->Add (Vector(186.1219787151405,38.103497572016565, 0.0));
    positionAlloc4->Add (Vector(125.62063662006308,133.75610570203497, 0.0));
    positionAlloc4->Add (Vector(29.607023770348697,75.47415144899499, 0.0));
    positionAlloc4->Add (Vector(287.57822067161817,308.17125570583494, 0.0));
    positionAlloc4->Add (Vector(330.41485382556306,369.9417071614106, 0.0));
    positionAlloc4->Add (Vector(365.0399666508831,273.56772480874423, 0.0));
    positionAlloc4->Add (Vector(334.478359263232,138.65604988259815, 0.0));
    positionAlloc4->Add (Vector(38.56421598948718,217.64203281755928, 0.0));
    positionAlloc4->Add (Vector(71.22897045864094,91.10728387309153, 0.0));
    positionAlloc4->Add (Vector(103.67144952274305,73.00099858630259, 0.0));
    positionAlloc4->Add (Vector(148.72681336882764,66.45038321225591, 0.0));
    positionAlloc4->Add (Vector(3.342003057132681,262.98245968540226, 0.0));
    positionAlloc4->Add (Vector(99.98228071345632,75.24478564941326, 0.0));
    positionAlloc4->Add (Vector(352.95490630303254,111.7420186094205, 0.0));
    positionAlloc4->Add (Vector(334.0961857860855,273.65131845201614, 0.0));
    positionAlloc4->Add (Vector(197.81389872063784,234.9437531664114, 0.0));
    positionAlloc4->Add (Vector(82.82378249376352,220.6072446894474, 0.0));
    positionAlloc4->Add (Vector(130.04244877567038,275.52975958406654, 0.0));
    positionAlloc4->Add (Vector(371.67563967786265,169.57238693246492, 0.0));
    positionAlloc4->Add (Vector(23.904966477665024,300.82097774651237, 0.0));
    positionAlloc4->Add (Vector(350.4421337612942,109.75385126524091, 0.0));
    positionAlloc4->Add (Vector(113.27592000255665,12.585953922791226, 0.0));
    positionAlloc4->Add (Vector(384.8563314448083,102.57425525500614, 0.0));
    positionAlloc4->Add (Vector(94.71856678134615,214.54546299368604, 0.0));
    positionAlloc4->Add (Vector(206.65020550549747,162.47844148537888, 0.0));
    positionAlloc4->Add (Vector(67.17998765146969,198.25355234048837, 0.0));
    positionAlloc4->Add (Vector(354.96927913834696,85.93259522333905, 0.0));
    positionAlloc4->Add (Vector(227.43255709518505,179.8130827356915, 0.0));
    positionAlloc4->Add (Vector(178.8225897833406,63.44736663613659, 0.0));
    positionAlloc4->Add (Vector(103.70642213609642,113.50743909147809, 0.0));
    positionAlloc4->Add (Vector(292.91564025993335,379.8931279595033, 0.0));
    positionAlloc4->Add (Vector(213.35282807223396,50.655957885004234, 0.0));
    positionAlloc4->Add (Vector(371.0599566183485,385.76582990597126, 0.0));
    positionAlloc4->Add (Vector(12.961890506262641,384.14823154283965, 0.0));
    positionAlloc4->Add (Vector(210.04221922023623,202.92469171055396, 0.0));
    positionAlloc4->Add (Vector(33.56467956248079,223.03286062919727, 0.0));
    positionAlloc4->Add (Vector(332.24417815223035,289.2621352676306, 0.0));
    positionAlloc4->Add (Vector(50.94972375854838,29.912290793803066, 0.0));
    positionAlloc4->Add (Vector(96.35765147322712,218.0939066692452, 0.0));
    positionAlloc4->Add (Vector(368.09217454083495,213.24034106708126, 0.0));
    positionAlloc4->Add (Vector(230.24543986907332,233.38923808150653, 0.0));
    positionAlloc4->Add (Vector(55.05330487217024,230.10934784780508, 0.0));
    positionAlloc4->Add (Vector(149.70465264481882,219.1416153743419, 0.0));
    positionAlloc4->Add (Vector(364.41256144209564,297.7682233462476, 0.0));
    positionAlloc4->Add (Vector(96.30923832396125,358.4788695354383, 0.0));
    positionAlloc4->Add (Vector(329.0379748886973,379.63748945579044, 0.0));
    positionAlloc4->Add (Vector(90.65532483191348,216.65517618800797, 0.0));
    positionAlloc4->Add (Vector(316.00854847907385,83.75003329584491, 0.0));
    positionAlloc4->Add (Vector(195.05537540945022,15.344228328034415, 0.0));
    positionAlloc4->Add (Vector(241.64961688306627,315.1926024094394, 0.0));
    positionAlloc4->Add (Vector(101.50952073658806,276.0537738950921, 0.0));
    positionAlloc4->Add (Vector(105.03209071865398,204.13908835184716, 0.0));
    positionAlloc4->Add (Vector(347.3668669500719,219.5115199223856, 0.0));
    positionAlloc4->Add (Vector(49.86959435312759,358.0585714141292, 0.0));
    positionAlloc4->Add (Vector(240.31487853841256,21.053406863621802, 0.0));
    positionAlloc4->Add (Vector(238.20241779775637,399.31268454098085, 0.0));
    positionAlloc4->Add (Vector(393.4122772701387,23.84361828113306, 0.0));
    positionAlloc4->Add (Vector(8.254598859208784,91.03208960257696, 0.0));
    positionAlloc4->Add (Vector(188.45046243816296,308.62086666351354, 0.0));
    positionAlloc4->Add (Vector(141.40969309403374,108.6684027913289, 0.0));
    positionAlloc4->Add (Vector(114.48337385771529,57.37950238988114, 0.0));
    positionAlloc4->Add (Vector(186.9318621566434,2.125197758310504, 0.0));
    positionAlloc4->Add (Vector(56.18364664800461,205.08978677056598, 0.0));
    positionAlloc4->Add (Vector(131.00526829273903,232.95433663502695, 0.0));
    positionAlloc4->Add (Vector(51.63644524659858,362.80723189983, 0.0));
    positionAlloc4->Add (Vector(389.3543370789945,343.108460018595, 0.0));
    positionAlloc4->Add (Vector(229.63425657587985,362.7337153069837, 0.0));
    positionAlloc4->Add (Vector(375.9284425332093,75.75659103756132, 0.0));
    positionAlloc4->Add (Vector(242.61209416575338,30.482631023401787, 0.0));
    positionAlloc4->Add (Vector(250.68895715138555,293.9353495884023, 0.0));
    positionAlloc4->Add (Vector(49.75951260152498,139.07077726145357, 0.0));
    positionAlloc4->Add (Vector(357.12789749263374,136.67863582601112, 0.0));
    positionAlloc4->Add (Vector(81.44488808450285,255.50720843607428, 0.0));
    positionAlloc4->Add (Vector(113.83414332427901,144.61309552908511, 0.0));
    positionAlloc4->Add (Vector(246.01560854346764,387.5184701614604, 0.0));
    positionAlloc4->Add (Vector(314.7268859397383,77.10928777424132, 0.0));
    positionAlloc4->Add (Vector(113.54409697650726,376.43266599906286, 0.0));
    positionAlloc4->Add (Vector(156.79047989987555,260.5703023731468, 0.0));
    positionAlloc4->Add (Vector(84.27578812037791,356.69350837327875, 0.0));
    positionAlloc4->Add (Vector(335.8484791293704,384.8376219754316, 0.0));
    positionAlloc4->Add (Vector(119.055427460142,113.40183230089478, 0.0));
    positionAlloc4->Add (Vector(177.8080744597878,189.89953095778836, 0.0));
    positionAlloc4->Add (Vector(396.8397953403561,81.14935019440254, 0.0));
    positionAlloc4->Add (Vector(232.06260293588835,388.4684971199849, 0.0));
    positionAlloc4->Add (Vector(324.253199378744,174.361822854762, 0.0));
    positionAlloc4->Add (Vector(219.8733892694058,155.38375308449548, 0.0));
    positionAlloc4->Add (Vector(155.9470553889737,169.06787714969974, 0.0));
    positionAlloc4->Add (Vector(8.076776216872794,305.253389544973, 0.0));
    positionAlloc4->Add (Vector(13.853949604658133,87.52175703967833, 0.0));
    positionAlloc4->Add (Vector(173.9583048428215,245.3197791093452, 0.0));
    positionAlloc4->Add (Vector(263.7596208805876,153.7439766550285, 0.0));
    positionAlloc4->Add (Vector(194.41403428465964,141.57136841262644, 0.0));
    positionAlloc4->Add (Vector(387.0047474475808,231.27459088880383, 0.0));
    positionAlloc4->Add (Vector(63.88238694551789,378.09677856859776, 0.0));
    positionAlloc4->Add (Vector(11.976359526339397,338.9521951508901, 0.0));
    positionAlloc4->Add (Vector(337.0308301276763,65.89793136024946, 0.0));
    positionAlloc4->Add (Vector(108.40654793285078,305.82579451422214, 0.0));
    positionAlloc4->Add (Vector(152.65213211193455,100.71882551609815, 0.0));
    positionAlloc4->Add (Vector(365.58563741426144,387.8182421252592, 0.0));
    positionAlloc4->Add (Vector(177.52846047850116,299.7421594762243, 0.0));
    positionAlloc4->Add (Vector(85.06804358377344,143.25851506689736, 0.0));
    positionAlloc4->Add (Vector(40.36442213882805,229.09255892638902, 0.0));
    positionAlloc4->Add (Vector(215.8277643765131,290.41385235270474, 0.0));
    positionAlloc4->Add (Vector(370.0155951168207,97.02418338569348, 0.0));
    positionAlloc4->Add (Vector(4.567697326038456,79.18269199345613, 0.0));
    positionAlloc4->Add (Vector(231.4029018933578,324.0956309894249, 0.0));
    positionAlloc4->Add (Vector(67.83051404839524,391.18363381819546, 0.0));
    positionAlloc4->Add (Vector(394.87057629965005,338.89868600738964, 0.0));
    positionAlloc4->Add (Vector(227.46928959973326,308.6313847227341, 0.0));
    positionAlloc4->Add (Vector(55.682179363480785,14.541973394279806, 0.0));
    positionAlloc4->Add (Vector(65.77924915192668,10.433097594217466, 0.0));
    positionAlloc4->Add (Vector(203.18297115462744,361.807259366471, 0.0));
    positionAlloc4->Add (Vector(80.04390324370583,222.73967148517468, 0.0));
    positionAlloc4->Add (Vector(150.73263649760187,126.13623174415274, 0.0));
    positionAlloc4->Add (Vector(71.56195500010836,296.20859230879205, 0.0));
    positionAlloc4->Add (Vector(319.1749778768672,301.14558535345145, 0.0));
    positionAlloc4->Add (Vector(98.83624607756794,124.50041871567987, 0.0));
    positionAlloc4->Add (Vector(27.101444973728572,28.33960189031668, 0.0));
    positionAlloc4->Add (Vector(399.9015189294687,385.77291208329194, 0.0));
    positionAlloc4->Add (Vector(268.9706200519983,70.24377320164983, 0.0));
    positionAlloc4->Add (Vector(157.59349968077584,57.39129299555401, 0.0));
    positionAlloc4->Add (Vector(350.3182263713538,95.5068921542038, 0.0));
    positionAlloc4->Add (Vector(175.53260101575427,158.28387337388384, 0.0));
    positionAlloc4->Add (Vector(328.23942933737186,272.6430191678561, 0.0));
    positionAlloc4->Add (Vector(16.430920576043828,50.432483215455726, 0.0));
    positionAlloc4->Add (Vector(10.856102537446377,225.31805390871176, 0.0));
    positionAlloc4->Add (Vector(113.96091389036545,289.5529369445441, 0.0));
    positionAlloc4->Add (Vector(352.94147582154034,305.6016397921563, 0.0));
    positionAlloc4->Add (Vector(113.8838721887577,168.00249211409636, 0.0));
    positionAlloc4->Add (Vector(274.37741083127054,314.8870479656855, 0.0));
    positionAlloc4->Add (Vector(206.43146381244395,133.69628050673376, 0.0));
    positionAlloc4->Add (Vector(305.3278994507977,46.689897692108495, 0.0));
    positionAlloc4->Add (Vector(199.22138216315406,327.97004215983645, 0.0));
    positionAlloc4->Add (Vector(78.31945675530747,345.63667242264444, 0.0));
    positionAlloc4->Add (Vector(369.6711374040635,141.7271040161324, 0.0));
    positionAlloc4->Add (Vector(86.31804708068125,14.739712619902612, 0.0));
    positionAlloc4->Add (Vector(143.7379920708461,144.5236842280829, 0.0));
    positionAlloc4->Add (Vector(207.0115187426457,58.4768767629912, 0.0));
    positionAlloc4->Add (Vector(197.6820391405986,216.61172860067697, 0.0));
    positionAlloc4->Add (Vector(324.24515650420557,126.56831292747653, 0.0));
    positionAlloc4->Add (Vector(157.57236785818748,24.21415479096276, 0.0));
    positionAlloc4->Add (Vector(365.6014586908773,294.5641632552694, 0.0));
    positionAlloc4->Add (Vector(296.81946216959716,223.046152243831, 0.0));
    positionAlloc4->Add (Vector(379.3330952379542,171.72762623589213, 0.0));
    positionAlloc4->Add (Vector(134.4777456566813,379.94076102490124, 0.0));
    positionAlloc4->Add (Vector(143.7349457601616,22.191534216854116, 0.0));
    positionAlloc4->Add (Vector(203.47100393648182,204.04918458570646, 0.0));
    positionAlloc4->Add (Vector(55.27955171376426,234.0127543780654, 0.0));
    positionAlloc4->Add (Vector(45.277449362277224,307.4422765258484, 0.0));
    positionAlloc4->Add (Vector(226.4169441362438,124.24321411238277, 0.0));
    positionAlloc4->Add (Vector(37.98529497077104,225.6905583540993, 0.0));
    positionAlloc4->Add (Vector(11.83162414368697,34.90826123842425, 0.0));
    positionAlloc4->Add (Vector(348.7479542599348,277.71693197748255, 0.0));
    positionAlloc4->Add (Vector(389.25384548380373,150.27497763326645, 0.0));
    positionAlloc4->Add (Vector(267.47106419670274,20.02969513944315, 0.0));
    positionAlloc4->Add (Vector(107.81444622220397,100.0008158651311, 0.0));
    positionAlloc4->Add (Vector(200.9169693062288,26.126022541731952, 0.0));
    positionAlloc4->Add (Vector(210.55567662617332,164.93744437587367, 0.0));
    positionAlloc4->Add (Vector(312.49886076583596,46.38869987439116, 0.0));
    positionAlloc4->Add (Vector(102.03843097996712,262.5357927038257, 0.0));
    positionAlloc4->Add (Vector(228.3205647297948,372.21024170013936, 0.0));
    positionAlloc4->Add (Vector(179.3694068187234,229.07259537896513, 0.0));
    positionAlloc4->Add (Vector(332.16982219197916,86.39171391868601, 0.0));
    positionAlloc4->Add (Vector(208.06582565864403,390.2164044285849, 0.0));
    positionAlloc4->Add (Vector(268.99661285699085,84.77154511177396, 0.0));
    positionAlloc4->Add (Vector(322.1376769186738,171.32067003121634, 0.0));
    positionAlloc4->Add (Vector(42.86389365019514,381.47308228188274, 0.0));
    positionAlloc4->Add (Vector(230.70171200228052,34.94336428851143, 0.0));
    positionAlloc4->Add (Vector(155.8149088136329,381.7450291201431, 0.0));
    positionAlloc4->Add (Vector(42.490496677691915,119.59041027921819, 0.0));
    positionAlloc4->Add (Vector(250.7690103018069,257.7659287303305, 0.0));
    positionAlloc4->Add (Vector(391.9553182402526,60.586178941073584, 0.0));
    positionAlloc4->Add (Vector(19.29947211507144,235.36906331265234, 0.0));
    positionAlloc4->Add (Vector(338.0604695500524,196.63044258163444, 0.0));
    positionAlloc4->Add (Vector(368.0044986996083,196.46303913347603, 0.0));
    positionAlloc4->Add (Vector(83.03818408043435,133.23597356476603, 0.0));
    positionAlloc4->Add (Vector(362.59841702542485,231.19425891028447, 0.0));
    positionAlloc4->Add (Vector(241.11868389283694,337.35551187411835, 0.0));
    positionAlloc4->Add (Vector(365.0041993272464,284.1631377068049, 0.0));
    positionAlloc4->Add (Vector(248.73730691572806,176.6826420611272, 0.0));
    positionAlloc4->Add (Vector(369.6128190914509,192.5135951786454, 0.0));
    positionAlloc4->Add (Vector(334.4638168977676,235.3179195568615, 0.0));
    positionAlloc4->Add (Vector(176.40382092901,231.47029802417566, 0.0));
    positionAlloc4->Add (Vector(149.0783541817693,73.00341014995797, 0.0));
    positionAlloc4->Add (Vector(69.49813240893982,68.61410502086152, 0.0));
    positionAlloc4->Add (Vector(7.511628775907875,348.28309854406825, 0.0));
    positionAlloc4->Add (Vector(113.53256493374246,56.760554349916646, 0.0));
    positionAlloc4->Add (Vector(25.604394790747875,394.7240753691107, 0.0));
    positionAlloc4->Add (Vector(232.3269476692701,37.73842296100054, 0.0));
    positionAlloc4->Add (Vector(186.41762949119624,217.25564486611248, 0.0));
    positionAlloc4->Add (Vector(77.07352770837628,335.661163600214, 0.0));
    positionAlloc4->Add (Vector(85.95818526791055,88.60426469965672, 0.0));
    positionAlloc4->Add (Vector(9.505069453507176,115.42838922533272, 0.0));
    positionAlloc4->Add (Vector(41.93798354145919,177.57573488096537, 0.0));
    positionAlloc4->Add (Vector(162.73971389696888,20.0713362580613, 0.0));
    positionAlloc4->Add (Vector(374.3984151225069,246.747624613032, 0.0));
    positionAlloc4->Add (Vector(242.7848560489898,112.98050017518881, 0.0));
    positionAlloc4->Add (Vector(312.4039819248235,204.0952635257487, 0.0));
    positionAlloc4->Add (Vector(249.70838353191965,329.1633287518862, 0.0));
    positionAlloc4->Add (Vector(33.39648852150927,391.79773448720596, 0.0));
    positionAlloc4->Add (Vector(193.55382978639875,30.268337822602163, 0.0));
    positionAlloc4->Add (Vector(4.356191661318354,117.02912616076655, 0.0));
    positionAlloc4->Add (Vector(240.59392542105053,52.32349796336511, 0.0));
    positionAlloc4->Add (Vector(291.10597173091264,323.97405387846374, 0.0));
    positionAlloc4->Add (Vector(78.72676663112395,158.35704887935557, 0.0));
    positionAlloc4->Add (Vector(66.53092883310934,271.7134513100398, 0.0));
    positionAlloc4->Add (Vector(204.06631980521547,117.70181844976979, 0.0));
    positionAlloc4->Add (Vector(218.98333825920554,22.06797729103829, 0.0));
    positionAlloc4->Add (Vector(123.83667508851875,337.11340008094805, 0.0));
    positionAlloc4->Add (Vector(284.13267293222845,8.60014600218122, 0.0));
    positionAlloc4->Add (Vector(197.5746016325346,117.78035480474473, 0.0));
    positionAlloc4->Add (Vector(39.18451673608274,177.36954620263444, 0.0));
    positionAlloc4->Add (Vector(44.87277024893075,31.57430085481492, 0.0));
    positionAlloc4->Add (Vector(193.56625725886136,80.79439581998047, 0.0));
    positionAlloc4->Add (Vector(368.0757030003481,57.9832803737014, 0.0));
    positionAlloc4->Add (Vector(338.0604796809076,101.42525530288191, 0.0));
    positionAlloc4->Add (Vector(223.2828168877022,158.08441860261118, 0.0));
    positionAlloc4->Add (Vector(2.1965570601699014,108.01349543216018, 0.0));
    positionAlloc4->Add (Vector(137.00314162886536,114.16546497302278, 0.0));
    positionAlloc4->Add (Vector(145.42060134711298,283.4748515964003, 0.0));
    positionAlloc4->Add (Vector(26.144108916474494,341.21892572233554, 0.0));
    positionAlloc4->Add (Vector(12.127657574261752,149.74232828450326, 0.0));
    positionAlloc4->Add (Vector(331.3618039382227,8.91570269463493, 0.0));
    positionAlloc4->Add (Vector(2.243401396580902,137.91383298312095, 0.0));
    positionAlloc4->Add (Vector(72.33508257764889,117.64624114285644, 0.0));
    positionAlloc4->Add (Vector(273.99700802962894,83.0166295601718, 0.0));
    positionAlloc4->Add (Vector(5.078084594872534,143.692344327578, 0.0));
    positionAlloc4->Add (Vector(120.83161940874061,219.90417895015648, 0.0));
    positionAlloc4->Add (Vector(146.9773464036506,139.69653929026117, 0.0));
    positionAlloc4->Add (Vector(78.16467464089341,77.56197625617372, 0.0));
    positionAlloc4->Add (Vector(180.1521853911374,294.1602912127229, 0.0));
    positionAlloc4->Add (Vector(298.68834123286894,347.1290436780891, 0.0));
    positionAlloc4->Add (Vector(44.511306092528045,125.66545917689189, 0.0));
    positionAlloc4->Add (Vector(32.70772851842017,42.64033385289001, 0.0));
    positionAlloc4->Add (Vector(350.14896153284934,125.04669924813263, 0.0));
    positionAlloc4->Add (Vector(21.04567280783676,392.2196530790417, 0.0));
    positionAlloc4->Add (Vector(154.23000014195063,186.0576377887244, 0.0));
    positionAlloc4->Add (Vector(4.6988076048050775,218.0631768636828, 0.0));
    positionAlloc4->Add (Vector(123.76905664858158,146.54331814194998, 0.0));
    positionAlloc4->Add (Vector(367.48941867525843,32.366555084163465, 0.0));
    positionAlloc4->Add (Vector(277.049293644403,215.7690382496336, 0.0));
    positionAlloc4->Add (Vector(211.82194711758382,375.8168147634443, 0.0));
    positionAlloc4->Add (Vector(61.97564189165718,227.72744249134118, 0.0));
    positionAlloc4->Add (Vector(367.19202068140316,289.5040910169481, 0.0));
    positionAlloc4->Add (Vector(391.07438476567575,177.6548817117893, 0.0));
    positionAlloc4->Add (Vector(305.40092418535187,394.19202765647185, 0.0));
    positionAlloc4->Add (Vector(159.07459978314952,252.2845535153528, 0.0));
    positionAlloc4->Add (Vector(223.4350975229317,68.11771424007382, 0.0));
    positionAlloc4->Add (Vector(143.443809397323,371.17284508097, 0.0));
    positionAlloc4->Add (Vector(328.4217939557591,2.3879557840963184, 0.0));
    positionAlloc4->Add (Vector(55.60235988574065,312.4851647499491, 0.0));
    positionAlloc4->Add (Vector(112.1313266139357,168.30935239286768, 0.0));
    positionAlloc4->Add (Vector(393.7518752303679,192.93686163023835, 0.0));
    positionAlloc4->Add (Vector(141.0833659989349,93.20999481595744, 0.0));
    positionAlloc4->Add (Vector(321.622793175022,38.10539957289354, 0.0));
    positionAlloc4->Add (Vector(330.3766350556468,14.761587102715934, 0.0));
    positionAlloc4->Add (Vector(346.6690412267543,307.1716408513653, 0.0));
    positionAlloc4->Add (Vector(45.325138582138266,150.8208544255152, 0.0));
    positionAlloc4->Add (Vector(277.6241062987398,376.92703210417216, 0.0));
    positionAlloc4->Add (Vector(116.63053163436112,194.73215527916216, 0.0));
    positionAlloc4->Add (Vector(199.7034842811016,268.3643442831887, 0.0));
    positionAlloc4->Add (Vector(61.273741407120006,125.30962041524707, 0.0));
    positionAlloc4->Add (Vector(154.60965884211194,112.19355131654889, 0.0));
    positionAlloc4->Add (Vector(131.21796150844744,27.111890362591584, 0.0));
    positionAlloc4->Add (Vector(64.23909433132997,92.24816583928366, 0.0));
    positionAlloc4->Add (Vector(56.12523657943349,71.89817881152783, 0.0));
    positionAlloc4->Add (Vector(177.14074298329825,323.24130140087686, 0.0));
    positionAlloc4->Add (Vector(55.90067892519759,36.69271063923772, 0.0));
    positionAlloc4->Add (Vector(313.19617940771127,335.160162079363, 0.0));
    positionAlloc4->Add (Vector(170.19197602747283,327.66345518078987, 0.0));
    positionAlloc4->Add (Vector(35.62369158470422,377.884303109909, 0.0));
    positionAlloc4->Add (Vector(312.59457290730865,39.75090995464687, 0.0));
    positionAlloc4->Add (Vector(224.64308102085212,231.4885865679534, 0.0));
    positionAlloc4->Add (Vector(368.96738800650735,50.38813983473616, 0.0));
    positionAlloc4->Add (Vector(41.507636415540446,398.95468142918776, 0.0));
    positionAlloc4->Add (Vector(180.54579202632857,358.2744467215715, 0.0));
    positionAlloc4->Add (Vector(23.399283256607227,47.671156298470585, 0.0));
    positionAlloc4->Add (Vector(81.212469009987,9.57993285030807, 0.0));
    positionAlloc4->Add (Vector(316.8758039611669,47.157504924749816, 0.0));
    positionAlloc4->Add (Vector(222.73730277039198,72.86909699235635, 0.0));
    positionAlloc4->Add (Vector(219.85649979544758,152.2261879562148, 0.0));
    positionAlloc4->Add (Vector(344.1159754503628,24.22582328566847, 0.0));
    positionAlloc4->Add (Vector(126.16699809663636,352.21429443994845, 0.0));
    positionAlloc4->Add (Vector(210.9067703660659,249.05797021054482, 0.0));
    positionAlloc4->Add (Vector(399.4507611321737,368.52317585624684, 0.0));
    positionAlloc4->Add (Vector(89.618579447902,301.09818584335886, 0.0));
    positionAlloc4->Add (Vector(73.98961912357733,288.9301201726494, 0.0));
    positionAlloc4->Add (Vector(373.55357795469723,259.4617114452089, 0.0));
    positionAlloc4->Add (Vector(49.879844380769754,342.28316278058463, 0.0));
    positionAlloc4->Add (Vector(226.88219810685632,44.38730967776148, 0.0));
    positionAlloc4->Add (Vector(291.9450111913744,313.7420818522675, 0.0));
    positionAlloc4->Add (Vector(291.6548889960462,110.04416250711029, 0.0));
    positionAlloc4->Add (Vector(366.1418530047363,125.02341015605066, 0.0));
    positionAlloc4->Add (Vector(350.808120625602,24.185544252905355, 0.0));
    positionAlloc4->Add (Vector(168.89052247459762,203.1597301372937, 0.0));
    positionAlloc4->Add (Vector(141.24189989984112,230.7426389538501, 0.0));
    positionAlloc4->Add (Vector(393.84423902243964,61.732410102076464, 0.0));
    positionAlloc4->Add (Vector(172.35063147095158,383.5505303869872, 0.0));
    positionAlloc4->Add (Vector(69.9966823862435,203.82215578539723, 0.0));
    positionAlloc4->Add (Vector(365.7784317462877,25.264877286883713, 0.0));
    positionAlloc4->Add (Vector(5.242043038556954,315.6971255545604, 0.0));
    positionAlloc4->Add (Vector(351.97314211359105,205.30501330197652, 0.0));
    positionAlloc4->Add (Vector(397.76775476741125,334.2594997331197, 0.0));
    positionAlloc4->Add (Vector(50.49501664785821,192.88434137364595, 0.0));
    positionAlloc4->Add (Vector(76.11968908548042,59.838429273693315, 0.0));
    positionAlloc4->Add (Vector(339.5733100633751,175.4458073923507, 0.0));
    positionAlloc4->Add (Vector(194.43652779624014,18.61217604899279, 0.0));
    positionAlloc4->Add (Vector(75.86334184167897,111.06982655497171, 0.0));
    positionAlloc4->Add (Vector(167.16340385918707,222.17461368713614, 0.0));
    positionAlloc4->Add (Vector(139.88437577759717,294.3258515321857, 0.0));
    positionAlloc4->Add (Vector(22.43870028255501,263.00849318004396, 0.0));
    positionAlloc4->Add (Vector(22.893709688041987,264.8475580203046, 0.0));
    positionAlloc4->Add (Vector(109.74439935496645,290.10174897298083, 0.0));
    positionAlloc4->Add (Vector(64.86202870403845,266.3060034416125, 0.0));
    positionAlloc4->Add (Vector(110.9241457719758,311.32134657823536, 0.0));
    positionAlloc4->Add (Vector(34.40676393958157,365.30191086878403, 0.0));
    positionAlloc4->Add (Vector(162.0742306144079,73.52045956919717, 0.0));
    positionAlloc4->Add (Vector(371.1776830868257,97.7831712096774, 0.0));
    positionAlloc4->Add (Vector(137.76506916599382,118.49746451514696, 0.0));
    positionAlloc4->Add (Vector(16.51042897623407,391.6637797986787, 0.0));
    positionAlloc4->Add (Vector(88.80325474466927,225.9234467554983, 0.0));
    positionAlloc4->Add (Vector(337.206069356118,81.86423689662044, 0.0));
    positionAlloc4->Add (Vector(38.24201236018867,363.5734127655377, 0.0));
    positionAlloc4->Add (Vector(71.27484054562095,186.26567068413488, 0.0));
    positionAlloc4->Add (Vector(69.17323996166988,203.01496996655771, 0.0));
    positionAlloc4->Add (Vector(264.5914152075382,257.77938606082137, 0.0));
    positionAlloc4->Add (Vector(217.79043036395348,210.9355646321807, 0.0));
    positionAlloc4->Add (Vector(216.98546293267196,189.54446744403919, 0.0));
    positionAlloc4->Add (Vector(30.73467695514531,372.23525574461735, 0.0));
    positionAlloc4->Add (Vector(362.890141631118,236.18843463988006, 0.0));
    positionAlloc4->Add (Vector(395.5404146794755,128.6275683830977, 0.0));
    positionAlloc4->Add (Vector(247.26757362051956,270.19496572910583, 0.0));
    positionAlloc4->Add (Vector(81.61722312352508,45.45533879990153, 0.0));
    positionAlloc4->Add (Vector(181.4705877381956,358.2468138880896, 0.0));
    positionAlloc4->Add (Vector(258.42522667450834,109.48369656221169, 0.0));
    positionAlloc4->Add (Vector(223.17733733880556,9.56375311931943, 0.0));
    positionAlloc4->Add (Vector(35.81917531085974,370.04321219470074, 0.0));
    positionAlloc4->Add (Vector(307.54074419371256,212.99895172994576, 0.0));
    positionAlloc4->Add (Vector(136.7097914774745,337.71198226536166, 0.0));
    positionAlloc4->Add (Vector(28.018250948750236,150.5289340341612, 0.0));
    positionAlloc4->Add (Vector(297.0304581636101,350.9951831953622, 0.0));
    positionAlloc4->Add (Vector(220.46049476733197,203.1940905238668, 0.0));
    positionAlloc4->Add (Vector(78.86830653651158,313.7321623454076, 0.0));
    positionAlloc4->Add (Vector(49.112783990228515,83.49096691964952, 0.0));
    positionAlloc4->Add (Vector(300.26166071542633,282.05799581251523, 0.0));
    positionAlloc4->Add (Vector(367.56646345803824,95.28724677843954, 0.0));
    positionAlloc4->Add (Vector(244.01498761815952,112.12808146515125, 0.0));
    positionAlloc4->Add (Vector(191.968301986368,34.771639646874064, 0.0));
    positionAlloc4->Add (Vector(284.58347951512866,173.18219237804317, 0.0));
    positionAlloc4->Add (Vector(92.89842281701199,237.98236575849901, 0.0));
    positionAlloc4->Add (Vector(328.88283951453536,23.22925602055075, 0.0));
    positionAlloc4->Add (Vector(63.90784404463376,47.46241315916424, 0.0));
    positionAlloc4->Add (Vector(174.57618649760184,356.34526493028164, 0.0));
    positionAlloc4->Add (Vector(282.4056092251823,60.42126711046221, 0.0));
    positionAlloc4->Add (Vector(352.37817001959206,209.51877662235972, 0.0));
    positionAlloc4->Add (Vector(116.87880447114476,197.02752181274903, 0.0));
    positionAlloc4->Add (Vector(48.40887404105412,276.2153498316259, 0.0));
    positionAlloc4->Add (Vector(79.57297881683489,2.942122251034718, 0.0));
    positionAlloc4->Add (Vector(77.91076699768178,56.853327688285745, 0.0));
    positionAlloc4->Add (Vector(321.2367837129535,196.62143802813802, 0.0));
    positionAlloc4->Add (Vector(390.3321168625539,16.013491515953817, 0.0));
    positionAlloc4->Add (Vector(323.5087667317818,286.2552038305378, 0.0));
    positionAlloc4->Add (Vector(165.57043286466725,52.45809355884683, 0.0));
    positionAlloc4->Add (Vector(58.21015831505587,237.3922221523468, 0.0));
    positionAlloc4->Add (Vector(260.9551937165846,32.49583067974693, 0.0));
    positionAlloc4->Add (Vector(110.77618857306733,201.47022019984905, 0.0));
    positionAlloc4->Add (Vector(207.4208981593756,325.0586646989628, 0.0));
    positionAlloc4->Add (Vector(298.07632453122665,87.08774054888133, 0.0));
    positionAlloc4->Add (Vector(300.3661595264385,212.61571635263948, 0.0));
    positionAlloc4->Add (Vector(328.28416904021714,311.6050179897608, 0.0));
    positionAlloc4->Add (Vector(142.49129778245734,238.37484591597013, 0.0));
    positionAlloc4->Add (Vector(280.6614446906522,273.4776573496666, 0.0));
    positionAlloc4->Add (Vector(111.53214408396312,372.5396940155718, 0.0));
    positionAlloc4->Add (Vector(235.57466734496734,26.801911772466582, 0.0));
    positionAlloc4->Add (Vector(166.0066568522744,22.47366479072861, 0.0));
    positionAlloc4->Add (Vector(118.83459530865727,191.4252752595175, 0.0));
    positionAlloc4->Add (Vector(40.77565762234232,94.95209624255034, 0.0));
    positionAlloc4->Add (Vector(175.29418103237933,142.45802829676322, 0.0));
    positionAlloc4->Add (Vector(2.8827096697327192,380.3555287765364, 0.0));
    positionAlloc4->Add (Vector(114.43457751590005,220.1457895949355, 0.0));
    positionAlloc4->Add (Vector(237.1863250329451,280.6908170846288, 0.0));
    positionAlloc4->Add (Vector(143.51093122226953,290.0490850977047, 0.0));
    positionAlloc4->Add (Vector(193.92575675553084,381.66471527500875, 0.0));
    positionAlloc4->Add (Vector(118.47786365987685,287.49700063415986, 0.0));
    positionAlloc4->Add (Vector(48.49990182786384,243.29268374575562, 0.0));
    positionAlloc4->Add (Vector(381.5585849827661,19.59571436193377, 0.0));
    positionAlloc4->Add (Vector(134.38832090681677,264.5586535578012, 0.0));
    positionAlloc4->Add (Vector(35.71022321297455,211.80783095378354, 0.0));
    positionAlloc4->Add (Vector(169.95988828521308,39.599865807180954, 0.0));
    positionAlloc4->Add (Vector(242.90108605603155,291.83746863775406, 0.0));
    positionAlloc4->Add (Vector(53.40295558542398,25.986617464270665, 0.0));
    positionAlloc4->Add (Vector(230.1344139184292,99.21904233268162, 0.0));
    positionAlloc4->Add (Vector(340.06304017331996,244.96164330519412, 0.0));
    positionAlloc4->Add (Vector(140.27206516919003,82.63627865533647, 0.0));
    positionAlloc4->Add (Vector(399.2734467975473,167.9430892767686, 0.0));
    positionAlloc4->Add (Vector(272.7054389695658,51.704732885833415, 0.0));
    positionAlloc4->Add (Vector(396.29038814044526,97.71597645635511, 0.0));
    positionAlloc4->Add (Vector(62.4735258322541,258.70358936503385, 0.0));
    positionAlloc4->Add (Vector(385.55465637469854,279.1741678381709, 0.0));
    positionAlloc4->Add (Vector(391.70950280866725,380.2618828621733, 0.0));
    positionAlloc4->Add (Vector(396.5722693204163,376.700632261269, 0.0));
    positionAlloc4->Add (Vector(182.00717519188552,343.3555804248426, 0.0));
    positionAlloc4->Add (Vector(305.7985238637622,337.02811706979645, 0.0));
    positionAlloc4->Add (Vector(302.0474378615796,114.11477815568882, 0.0));
    positionAlloc4->Add (Vector(114.02812130219266,16.136190613193826, 0.0));
    positionAlloc4->Add (Vector(131.89744903469617,65.69683865350116, 0.0));
    positionAlloc4->Add (Vector(144.86887999226136,312.02269630217387, 0.0));
    positionAlloc4->Add (Vector(248.0447111369313,76.1417080871098, 0.0));
    positionAlloc4->Add (Vector(270.8627474217903,387.1631985581617, 0.0));
    positionAlloc4->Add (Vector(0.5537126350770194,177.19472365995153, 0.0));
    positionAlloc4->Add (Vector(344.6439486575598,64.71000463536001, 0.0));
    positionAlloc4->Add (Vector(206.0006773081047,121.58222562377787, 0.0));
    positionAlloc4->Add (Vector(399.66501633725215,30.838818205921605, 0.0));
    positionAlloc4->Add (Vector(342.288604098064,226.07706438728647, 0.0));
    positionAlloc4->Add (Vector(188.4570789646964,91.70786014744169, 0.0));
    positionAlloc4->Add (Vector(42.32581847142427,273.8977108825969, 0.0));
    positionAlloc4->Add (Vector(199.4003276401785,210.92178389347364, 0.0));
    positionAlloc4->Add (Vector(219.82692636302633,282.6975850186673, 0.0));
    positionAlloc4->Add (Vector(129.7719482076257,11.832271235306102, 0.0));
    positionAlloc4->Add (Vector(32.56210517190436,131.32173603896047, 0.0));
    positionAlloc4->Add (Vector(140.130173631898,145.7542925881583, 0.0));
    positionAlloc4->Add (Vector(107.75913316099515,284.30006040051916, 0.0));
    positionAlloc4->Add (Vector(255.20238115110243,363.16966114828364, 0.0));
    positionAlloc4->Add (Vector(63.33885808293998,236.2036343713327, 0.0));
    positionAlloc4->Add (Vector(209.43053892733184,372.6135080854924, 0.0));
    positionAlloc4->Add (Vector(248.20437454978253,196.84287746264545, 0.0));
    positionAlloc4->Add (Vector(392.0011355424261,333.3323077117931, 0.0));
    positionAlloc4->Add (Vector(82.75598037003849,296.3886534960975, 0.0));
    positionAlloc4->Add (Vector(133.98651015559707,225.5134340886971, 0.0));
    positionAlloc4->Add (Vector(70.80935954886733,315.7034009597012, 0.0));
    positionAlloc4->Add (Vector(301.8756274181526,380.0633665443281, 0.0));
    positionAlloc4->Add (Vector(282.55557180978616,231.16858570221459, 0.0));
    positionAlloc4->Add (Vector(162.93952917999928,198.48979813933502, 0.0));
    positionAlloc4->Add (Vector(349.03917319740737,323.0633674317622, 0.0));
    positionAlloc4->Add (Vector(94.09675947027209,225.71782513575553, 0.0));
    positionAlloc4->Add (Vector(232.4476083951513,249.9757184676867, 0.0));
    positionAlloc4->Add (Vector(96.44088715305786,29.265352218120412, 0.0));
    positionAlloc4->Add (Vector(121.69794461729184,116.16315122875366, 0.0));
    positionAlloc4->Add (Vector(291.12615913869735,196.8172457406356, 0.0));
    positionAlloc4->Add (Vector(326.01324612912873,105.13680742218803, 0.0));
    positionAlloc4->Add (Vector(154.07586025749958,223.99909401423926, 0.0));
    positionAlloc4->Add (Vector(211.77612546780517,88.52481577121848, 0.0));
    positionAlloc4->Add (Vector(239.62207339079745,28.222343297175946, 0.0));
    positionAlloc4->Add (Vector(361.7343184893274,181.89111869020067, 0.0));
    positionAlloc4->Add (Vector(200.0354468294792,34.99181604473143, 0.0));
    positionAlloc4->Add (Vector(307.6085623887799,166.25086185433702, 0.0));
    positionAlloc4->Add (Vector(204.23931391339076,178.80500809301583, 0.0));
    positionAlloc4->Add (Vector(284.97359284703157,299.93202926141305, 0.0));
    positionAlloc4->Add (Vector(379.1785450355067,193.99328028555254, 0.0));
    positionAlloc4->Add (Vector(197.93273075916647,380.2221802262552, 0.0));
    positionAlloc4->Add (Vector(267.9595522617946,153.1283897529821, 0.0));
    positionAlloc4->Add (Vector(165.04461848866967,321.6345676429523, 0.0));
    positionAlloc4->Add (Vector(51.18002926710772,301.504349257964, 0.0));
    positionAlloc4->Add (Vector(150.495891995948,15.614790118736854, 0.0));
    positionAlloc4->Add (Vector(75.56089448422725,168.0032078312925, 0.0));
    positionAlloc4->Add (Vector(91.88349456764446,228.83897519296798, 0.0));
    positionAlloc4->Add (Vector(121.0225245367281,339.4171835999289, 0.0));
    positionAlloc4->Add (Vector(101.97468062029627,368.1677107414516, 0.0));
    positionAlloc4->Add (Vector(93.20494387852114,248.7704838328251, 0.0));
    positionAlloc4->Add (Vector(91.74617589414117,277.44098699774827, 0.0));
    positionAlloc4->Add (Vector(300.95635443415784,105.98176872364533, 0.0));
    positionAlloc4->Add (Vector(229.49939994283963,361.0968042253275, 0.0));
    positionAlloc4->Add (Vector(177.00110018900023,247.77827490878823, 0.0));
    positionAlloc4->Add (Vector(252.58258510955062,308.07394107516626, 0.0));
    positionAlloc4->Add (Vector(288.6350603560229,119.36755984644871, 0.0));
    positionAlloc4->Add (Vector(312.1823086429814,368.1056832046552, 0.0));
    positionAlloc4->Add (Vector(11.024982982086451,69.78765751145266, 0.0));
    positionAlloc4->Add (Vector(103.8798850656518,69.68516142330805, 0.0));
    positionAlloc4->Add (Vector(93.4660834671229,28.06159005307496, 0.0));
    positionAlloc4->Add (Vector(382.0395127225339,102.2707061120598, 0.0));
    positionAlloc4->Add (Vector(206.28167660261772,354.9456000727008, 0.0));
    positionAlloc4->Add (Vector(196.95866143091033,164.76240558283516, 0.0));
    positionAlloc4->Add (Vector(67.33368663312814,245.7748842461658, 0.0));
    positionAlloc4->Add (Vector(119.31108846459182,32.56695000026855, 0.0));
    positionAlloc4->Add (Vector(103.24799708615919,110.24755854175194, 0.0));
    positionAlloc4->Add (Vector(369.4510302986173,338.10770457001854, 0.0));
    positionAlloc4->Add (Vector(40.42099676306057,144.94287952029242, 0.0));
    positionAlloc4->Add (Vector(363.6314133364873,100.39056772593251, 0.0));
    positionAlloc4->Add (Vector(99.49128018228706,376.7810022893751, 0.0));
    positionAlloc4->Add (Vector(370.31857114780104,392.2523053465127, 0.0));
    positionAlloc4->Add (Vector(138.10014171384685,255.23221310325505, 0.0));
    positionAlloc4->Add (Vector(108.92320865247473,371.23119818115754, 0.0));
    positionAlloc4->Add (Vector(251.55885985468524,277.60986357778233, 0.0));
    positionAlloc4->Add (Vector(309.86899676364436,72.97611461487831, 0.0));
    positionAlloc4->Add (Vector(205.0321940744452,269.6949205995936, 0.0));
    positionAlloc4->Add (Vector(146.57362869824863,230.1365728071712, 0.0));
    positionAlloc4->Add (Vector(384.05483789444037,115.95825445612418, 0.0));
    positionAlloc4->Add (Vector(216.69837954612584,186.85408818139928, 0.0));
    positionAlloc4->Add (Vector(321.7366384829067,252.4726914902173, 0.0));
    positionAlloc4->Add (Vector(220.67144738024896,323.16216766796924, 0.0));
    positionAlloc4->Add (Vector(67.59395904250569,24.326614586665272, 0.0));
    positionAlloc4->Add (Vector(41.07506038886166,27.52010338063964, 0.0));
    positionAlloc4->Add (Vector(79.70667413020377,74.46741683327387, 0.0));
    positionAlloc4->Add (Vector(183.1888536317594,229.10322322477293, 0.0));
    positionAlloc4->Add (Vector(271.18961547266497,19.45162593963561, 0.0));
    positionAlloc4->Add (Vector(21.760203689544877,115.6343979069323, 0.0));
    positionAlloc4->Add (Vector(42.151254679019345,182.98617057274882, 0.0));
    positionAlloc4->Add (Vector(392.8762342536818,19.069630918067347, 0.0));
    positionAlloc4->Add (Vector(53.802383493498155,3.072783635948406, 0.0));
    positionAlloc4->Add (Vector(262.79114716343724,32.600392281292216, 0.0));
    positionAlloc4->Add (Vector(161.4982901091773,353.31811355994466, 0.0));
    positionAlloc4->Add (Vector(310.62292256065274,100.17195936766647, 0.0));
    positionAlloc4->Add (Vector(285.0892299287311,77.98757339216209, 0.0));
    positionAlloc4->Add (Vector(305.7682997745146,26.268604385719343, 0.0));
    positionAlloc4->Add (Vector(126.59661349132136,328.4276451949896, 0.0));
    positionAlloc4->Add (Vector(21.255108971175307,286.45336283153125, 0.0));
    positionAlloc4->Add (Vector(167.5195588748855,96.42076815868066, 0.0));
    positionAlloc4->Add (Vector(217.04149084893433,3.746594111028445, 0.0));
    positionAlloc4->Add (Vector(77.6367337303165,399.48830671716615, 0.0));
    positionAlloc4->Add (Vector(67.50010598917639,312.5570233287113, 0.0));
    positionAlloc4->Add (Vector(238.7701036434394,175.59803879269268, 0.0));
    positionAlloc4->Add (Vector(311.7944648336455,324.76914754769626, 0.0));
    positionAlloc4->Add (Vector(342.68998413431314,270.00247195674325, 0.0));
    positionAlloc4->Add (Vector(158.99323892196082,297.82970731466605, 0.0));
    positionAlloc4->Add (Vector(269.292328748977,392.5992245207416, 0.0));
    positionAlloc4->Add (Vector(108.9798484235911,162.3285955418484, 0.0));
    positionAlloc4->Add (Vector(316.3473146878253,109.59585783876587, 0.0));
    positionAlloc4->Add (Vector(225.3176157362486,171.12136419637935, 0.0));
    positionAlloc4->Add (Vector(12.87843490475482,40.610179147849394, 0.0));
    positionAlloc4->Add (Vector(10.80683297363536,194.88409521954364, 0.0));
    positionAlloc4->Add (Vector(300.06252820592664,16.829281053794087, 0.0));
    positionAlloc4->Add (Vector(396.4372270811102,333.49562522002174, 0.0));
    positionAlloc4->Add (Vector(63.51134245366836,110.34584382208115, 0.0));
    positionAlloc4->Add (Vector(265.0970675045199,376.8506543361501, 0.0));
    positionAlloc4->Add (Vector(117.46723051735022,74.47244175491564, 0.0));
    positionAlloc4->Add (Vector(293.9195998494926,202.22991245545555, 0.0));
    positionAlloc4->Add (Vector(10.80509512078991,210.34615367195633, 0.0));
    positionAlloc4->Add (Vector(320.0943782536307,15.670063231418485, 0.0));
    positionAlloc4->Add (Vector(275.3007565127556,25.924335196113546, 0.0));
    positionAlloc4->Add (Vector(191.4832629903784,288.1958006260062, 0.0));
    positionAlloc4->Add (Vector(262.0736997899988,208.58145848051527, 0.0));
    positionAlloc4->Add (Vector(269.2592714902636,12.021588052990806, 0.0));
    positionAlloc4->Add (Vector(109.77463824198685,124.05233508509906, 0.0));
    positionAlloc4->Add (Vector(169.39370522669188,187.37915950709936, 0.0));
    positionAlloc4->Add (Vector(93.4589511709734,66.37540499281047, 0.0));
    positionAlloc4->Add (Vector(24.53978035334017,305.7999047175123, 0.0));
    positionAlloc4->Add (Vector(159.18358927789146,245.15191910469184, 0.0));
    positionAlloc4->Add (Vector(221.23058220799535,162.56609749298653, 0.0));
    positionAlloc4->Add (Vector(145.8654739247549,269.09981184940324, 0.0));
    positionAlloc4->Add (Vector(273.69229048954145,328.5565248562743, 0.0));
    positionAlloc4->Add (Vector(191.74207867357006,269.8635171280222, 0.0));
    positionAlloc4->Add (Vector(226.21362477383423,107.56118263162655, 0.0));
    positionAlloc4->Add (Vector(80.1095691759914,309.4653657105388, 0.0));
    positionAlloc4->Add (Vector(339.1098905303386,76.14490908354404, 0.0));
    positionAlloc4->Add (Vector(323.8761215222554,319.8338088716502, 0.0));
    positionAlloc4->Add (Vector(217.346109965454,214.8426595684001, 0.0));
    positionAlloc4->Add (Vector(289.28792268613506,275.6394915951942, 0.0));
    positionAlloc4->Add (Vector(159.82313393032385,93.4487461788966, 0.0));
    positionAlloc4->Add (Vector(327.58282538440915,378.35690542787574, 0.0));
    positionAlloc4->Add (Vector(138.79050833714825,359.97768091091854, 0.0));
    positionAlloc4->Add (Vector(269.2188702788902,213.47635816489552, 0.0));
    positionAlloc4->Add (Vector(355.2197509124846,304.8452260379219, 0.0));
    positionAlloc4->Add (Vector(33.749531815327536,166.3898247774203, 0.0));
    positionAlloc4->Add (Vector(20.460014650363156,396.09849427506447, 0.0));
    positionAlloc4->Add (Vector(395.8776224896691,323.3440526304365, 0.0));
    positionAlloc4->Add (Vector(271.8157766018628,81.83125915470133, 0.0));
    positionAlloc4->Add (Vector(243.75327152599255,365.66392194438475, 0.0));
    positionAlloc4->Add (Vector(175.75557803950298,281.8479142896922, 0.0));
    positionAlloc4->Add (Vector(18.417226802693687,316.71049542065646, 0.0));
    positionAlloc4->Add (Vector(312.9605730457566,365.7856698301832, 0.0));
    positionAlloc4->Add (Vector(4.593638277904644,217.9779312636749, 0.0));
    positionAlloc4->Add (Vector(169.91863897229757,189.0414107296055, 0.0));
    positionAlloc4->Add (Vector(221.32638116804824,82.6155276564836, 0.0));
    positionAlloc4->Add (Vector(27.296225915633656,260.1099441342469, 0.0));
    positionAlloc4->Add (Vector(326.0903112265613,363.74547567296764, 0.0));
    positionAlloc4->Add (Vector(10.3909370504641,36.204347288746774, 0.0));
    positionAlloc4->Add (Vector(179.01151609609,331.9771111465998, 0.0));
    positionAlloc4->Add (Vector(44.368289436832285,387.5285175929486, 0.0));
    positionAlloc4->Add (Vector(306.8585281815909,91.24310855464155, 0.0));
    positionAlloc4->Add (Vector(219.75563285642053,47.99769145963219, 0.0));
    positionAlloc4->Add (Vector(397.1038283817155,69.50777394593092, 0.0));
    positionAlloc4->Add (Vector(252.44283223864866,356.26398858545406, 0.0));
    positionAlloc4->Add (Vector(252.6173315013485,309.1304340756116, 0.0));
    positionAlloc4->Add (Vector(277.11846058069216,325.9966307400927, 0.0));
    positionAlloc4->Add (Vector(91.68011282850172,36.05352197085212, 0.0));
    positionAlloc4->Add (Vector(213.97320117990296,340.0834886877893, 0.0));
    positionAlloc4->Add (Vector(5.550785191860852,319.56753223865917, 0.0));
    positionAlloc4->Add (Vector(49.684627047146755,266.99245497151446, 0.0));
    positionAlloc4->Add (Vector(385.176544949283,185.50821425027158, 0.0));
    positionAlloc4->Add (Vector(14.814044695015793,97.77394195162361, 0.0));
    positionAlloc4->Add (Vector(378.7071899351388,139.7566646308672, 0.0));
    positionAlloc4->Add (Vector(179.8600350955096,0.7869253263237397, 0.0));
    positionAlloc4->Add (Vector(373.5705097989601,325.06586670333377, 0.0));
    positionAlloc4->Add (Vector(151.36207474400635,302.34571558979223, 0.0));
    positionAlloc4->Add (Vector(379.5447682406155,395.973494706792, 0.0));
    positionAlloc4->Add (Vector(291.1511051695651,174.7577880838221, 0.0));
    positionAlloc4->Add (Vector(274.38091632861455,229.7013045263698, 0.0));
    positionAlloc4->Add (Vector(288.57891307318846,73.59699090318364, 0.0));
    positionAlloc4->Add (Vector(334.35841543510327,137.83710903461773, 0.0));
    positionAlloc4->Add (Vector(384.5475451896154,136.8229623617063, 0.0));
    positionAlloc4->Add (Vector(107.73493891557466,116.08002549608712, 0.0));
    positionAlloc4->Add (Vector(69.94815753089902,194.86713148802153, 0.0));
    positionAlloc4->Add (Vector(321.47029399794167,31.05897707552545, 0.0));
    positionAlloc4->Add (Vector(85.97031403761876,374.1961252076983, 0.0));
    positionAlloc4->Add (Vector(103.61969769281308,238.8341884860894, 0.0));
    positionAlloc4->Add (Vector(262.86922295682587,122.55949808061683, 0.0));
    positionAlloc4->Add (Vector(354.64264190813367,250.7363426415008, 0.0));
    positionAlloc4->Add (Vector(59.252705665921155,93.35911404205963, 0.0));
    positionAlloc4->Add (Vector(267.8487635346994,74.15677656276274, 0.0));
    positionAlloc4->Add (Vector(240.59597426968108,224.10718814138843, 0.0));
    positionAlloc4->Add (Vector(248.48204835176975,312.8075506157689, 0.0));
    positionAlloc4->Add (Vector(72.90416361032372,226.83938991844633, 0.0));
    positionAlloc4->Add (Vector(303.7329917380496,16.434153744506517, 0.0));
    positionAlloc4->Add (Vector(323.4868382280278,215.25272092136962, 0.0));
    positionAlloc4->Add (Vector(21.86575224964642,296.64253033689573, 0.0));
    positionAlloc4->Add (Vector(203.39996956348804,168.9172208902801, 0.0));
    positionAlloc4->Add (Vector(323.78429969850566,109.02440014527994, 0.0));
    positionAlloc4->Add (Vector(88.34361485175299,278.718708562352, 0.0));
    positionAlloc4->Add (Vector(54.34707312497111,304.60979042508103, 0.0));
    positionAlloc4->Add (Vector(234.1274048875012,153.97073034158436, 0.0));
    positionAlloc4->Add (Vector(396.3093522190067,144.76723226216808, 0.0));
    positionAlloc4->Add (Vector(354.12681818741123,382.8631053399486, 0.0));
    positionAlloc4->Add (Vector(36.98890074036041,193.3943631299173, 0.0));
    positionAlloc4->Add (Vector(235.12229775228838,356.75648874478617, 0.0));
    positionAlloc4->Add (Vector(49.231220715630684,50.54018798590989, 0.0));
    positionAlloc4->Add (Vector(349.41301093897897,99.08750108317301, 0.0));
    positionAlloc4->Add (Vector(311.20732616420594,7.627321964491873, 0.0));
    positionAlloc4->Add (Vector(342.61495792661145,115.58953473461418, 0.0));
    positionAlloc4->Add (Vector(316.2370350585067,267.159360195422, 0.0));
    positionAlloc4->Add (Vector(189.6762030303875,380.9916536560018, 0.0));
    positionAlloc4->Add (Vector(269.0085942142359,79.15775206424667, 0.0));
    positionAlloc4->Add (Vector(99.57561884658391,64.23587288141431, 0.0));
    positionAlloc4->Add (Vector(288.41630557588957,36.33711591373414, 0.0));
    positionAlloc4->Add (Vector(182.00340516690957,153.60935910317926, 0.0));
    positionAlloc4->Add (Vector(17.046848053282247,240.41336153941145, 0.0));
    positionAlloc4->Add (Vector(119.69486636300321,261.2268491524525, 0.0));
    positionAlloc4->Add (Vector(162.46864144595213,117.85829068978887, 0.0));
    positionAlloc4->Add (Vector(296.61757439133754,154.66476792353308, 0.0));
    positionAlloc4->Add (Vector(398.52134005133826,321.4685486143126, 0.0));
    positionAlloc4->Add (Vector(58.41754303172087,241.92295440320942, 0.0));
    positionAlloc4->Add (Vector(333.76480142427823,140.49491818703794, 0.0));
    positionAlloc4->Add (Vector(324.76906997870583,353.162173263007, 0.0));
    positionAlloc4->Add (Vector(230.69162823184723,99.22810627392455, 0.0));
    positionAlloc4->Add (Vector(157.6641024677662,130.09929652230278, 0.0));
    positionAlloc4->Add (Vector(56.37286023866142,365.24150335810015, 0.0));
    positionAlloc4->Add (Vector(175.40065894122634,307.57640606990014, 0.0));
    positionAlloc4->Add (Vector(379.8530269815835,87.15910280577765, 0.0));
    positionAlloc4->Add (Vector(97.50680666393264,226.27862019817692, 0.0));
    positionAlloc4->Add (Vector(120.70656554170624,180.0794114263459, 0.0));
    positionAlloc4->Add (Vector(116.72328547151464,199.43031068456665, 0.0));
    positionAlloc4->Add (Vector(202.60513913580724,305.1850301308306, 0.0));
    positionAlloc4->Add (Vector(279.55078600621806,30.385066814835284, 0.0));
    positionAlloc4->Add (Vector(372.1186196801156,290.1103125477177, 0.0));
    positionAlloc4->Add (Vector(289.260453599205,238.37606395418928, 0.0));
    positionAlloc4->Add (Vector(294.986993776213,62.80797670074807, 0.0));
    positionAlloc4->Add (Vector(94.25600419288962,107.87609187464646, 0.0));
    positionAlloc4->Add (Vector(183.89328382120618,58.70592344230747, 0.0));
    positionAlloc4->Add (Vector(353.6566407397538,181.74519985630647, 0.0));
    positionAlloc4->Add (Vector(307.44710644315404,308.6205054723703, 0.0));
    positionAlloc4->Add (Vector(314.10271349478603,298.0689176316897, 0.0));
    positionAlloc4->Add (Vector(296.6529639681554,213.7896893992973, 0.0));
    positionAlloc4->Add (Vector(75.48870257299893,211.9878542402521, 0.0));
    positionAlloc4->Add (Vector(289.40496829954066,375.73701920316194, 0.0));
    positionAlloc4->Add (Vector(123.46679400990807,194.22883735662722, 0.0));
    positionAlloc4->Add (Vector(120.06200383862371,273.96711733325174, 0.0));
    positionAlloc4->Add (Vector(214.88146480416202,100.48979054921317, 0.0));
    positionAlloc4->Add (Vector(253.07384645619302,131.7714503940109, 0.0));
    positionAlloc4->Add (Vector(345.1029098485854,17.974026481467618, 0.0));
    positionAlloc4->Add (Vector(288.4581103998694,149.30534942660393, 0.0));
    positionAlloc4->Add (Vector(220.58454006274198,252.4080058738435, 0.0));
    positionAlloc4->Add (Vector(183.85000855297423,233.08257365584632, 0.0));
    positionAlloc4->Add (Vector(9.495883398573124,324.56779074681873, 0.0));
    positionAlloc4->Add (Vector(389.19614233132756,147.12877378017444, 0.0));
    positionAlloc4->Add (Vector(109.39117311289421,274.3332512578047, 0.0));
    positionAlloc4->Add (Vector(311.8766110379685,92.19027256161532, 0.0));
    positionAlloc4->Add (Vector(116.43307588759887,393.0571192269118, 0.0));
    positionAlloc4->Add (Vector(245.53401273964027,77.23396823055681, 0.0));
    positionAlloc4->Add (Vector(3.884170671398124,117.08683002628023, 0.0));
    positionAlloc4->Add (Vector(241.45579531838473,101.79820005619366, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_13.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_13.flowmon", false, false);
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