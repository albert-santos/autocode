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
    positionAlloc->Add (Vector (500.0,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (500.0,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,500.0, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,500.0, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,500.0, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,500.0, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,55.55555555555556, 0.0));
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
    positionAlloc4->Add (Vector(73.97397971230963,386.21654468108824, 0.0));
    positionAlloc4->Add (Vector(26.06264660740648,272.56889762708374, 0.0));
    positionAlloc4->Add (Vector(292.4444219188383,87.52524334547374, 0.0));
    positionAlloc4->Add (Vector(150.67668641560218,420.9471284489644, 0.0));
    positionAlloc4->Add (Vector(475.9148650070235,128.8221040268599, 0.0));
    positionAlloc4->Add (Vector(290.3586057520901,491.73590805110865, 0.0));
    positionAlloc4->Add (Vector(381.17704648441696,474.4584818069499, 0.0));
    positionAlloc4->Add (Vector(171.78982296111712,155.66784565207803, 0.0));
    positionAlloc4->Add (Vector(64.83851702058857,80.51241095089651, 0.0));
    positionAlloc4->Add (Vector(46.58483788581247,148.37398272495355, 0.0));
    positionAlloc4->Add (Vector(444.7064436581144,75.94538097728804, 0.0));
    positionAlloc4->Add (Vector(214.7876613949441,339.652833742743, 0.0));
    positionAlloc4->Add (Vector(280.70580049009635,490.2831066737098, 0.0));
    positionAlloc4->Add (Vector(128.52130336558292,251.166552819497, 0.0));
    positionAlloc4->Add (Vector(152.01906091061306,64.14391998849001, 0.0));
    positionAlloc4->Add (Vector(46.75637354904588,7.27765979387196, 0.0));
    positionAlloc4->Add (Vector(432.65287618947076,452.30134061802227, 0.0));
    positionAlloc4->Add (Vector(56.058038832199344,108.81229905431944, 0.0));
    positionAlloc4->Add (Vector(86.78042509724176,369.0217076695178, 0.0));
    positionAlloc4->Add (Vector(295.8155835206922,125.2699514247646, 0.0));
    positionAlloc4->Add (Vector(213.08747757725266,391.47443275554855, 0.0));
    positionAlloc4->Add (Vector(72.21689084331162,419.4170948671261, 0.0));
    positionAlloc4->Add (Vector(338.6367600235006,260.2324652706134, 0.0));
    positionAlloc4->Add (Vector(175.12102709439864,401.3539117701509, 0.0));
    positionAlloc4->Add (Vector(320.7743433119731,488.22287216282336, 0.0));
    positionAlloc4->Add (Vector(235.228453077883,87.18712721505494, 0.0));
    positionAlloc4->Add (Vector(293.9479645685695,423.7132762191525, 0.0));
    positionAlloc4->Add (Vector(343.7769029774433,403.78718152525073, 0.0));
    positionAlloc4->Add (Vector(446.79159178170124,299.142408917668, 0.0));
    positionAlloc4->Add (Vector(232.36961649804593,100.11239445007459, 0.0));
    positionAlloc4->Add (Vector(388.08788151273495,4.6303962998561765, 0.0));
    positionAlloc4->Add (Vector(50.18522142365922,209.07197254412247, 0.0));
    positionAlloc4->Add (Vector(317.3906718880315,31.215591491331075, 0.0));
    positionAlloc4->Add (Vector(447.4360484828571,344.2888943696449, 0.0));
    positionAlloc4->Add (Vector(94.25631500138282,67.09235945104797, 0.0));
    positionAlloc4->Add (Vector(29.657370860523113,332.490333853807, 0.0));
    positionAlloc4->Add (Vector(230.0719656188142,353.1010557162062, 0.0));
    positionAlloc4->Add (Vector(306.68811610523636,293.33520219378994, 0.0));
    positionAlloc4->Add (Vector(96.7249781596588,8.224761080267374, 0.0));
    positionAlloc4->Add (Vector(362.63056817744484,101.55915826003664, 0.0));
    positionAlloc4->Add (Vector(52.33114901536323,234.95350045496028, 0.0));
    positionAlloc4->Add (Vector(494.02129098115967,113.80459293683131, 0.0));
    positionAlloc4->Add (Vector(354.886451593625,281.2203273416251, 0.0));
    positionAlloc4->Add (Vector(142.72930280683948,41.33942112731221, 0.0));
    positionAlloc4->Add (Vector(43.06403739969128,200.23181438399433, 0.0));
    positionAlloc4->Add (Vector(222.78469970556398,484.57681243523587, 0.0));
    positionAlloc4->Add (Vector(438.92306156191,82.64988661889294, 0.0));
    positionAlloc4->Add (Vector(327.1413341758842,144.18881354015016, 0.0));
    positionAlloc4->Add (Vector(341.3521161180637,29.214579224506267, 0.0));
    positionAlloc4->Add (Vector(359.27784886026024,221.5972660477466, 0.0));
    positionAlloc4->Add (Vector(135.10982569541136,51.17183034034789, 0.0));
    positionAlloc4->Add (Vector(354.78920528101906,368.5402137367286, 0.0));
    positionAlloc4->Add (Vector(130.6432924831129,316.1458580871053, 0.0));
    positionAlloc4->Add (Vector(371.11911935398894,461.31898100380766, 0.0));
    positionAlloc4->Add (Vector(198.35861679660906,108.7250304039814, 0.0));
    positionAlloc4->Add (Vector(247.9492128945166,299.76043443857304, 0.0));
    positionAlloc4->Add (Vector(418.0162572311068,350.6039480640036, 0.0));
    positionAlloc4->Add (Vector(267.9048412199766,209.90396764591102, 0.0));
    positionAlloc4->Add (Vector(167.19422679284855,134.2981283088928, 0.0));
    positionAlloc4->Add (Vector(235.07779160496577,64.25327869005176, 0.0));
    positionAlloc4->Add (Vector(316.17338296726297,248.39993814405554, 0.0));
    positionAlloc4->Add (Vector(47.405983065041625,329.3888748008474, 0.0));
    positionAlloc4->Add (Vector(131.02863610033373,230.7206357206713, 0.0));
    positionAlloc4->Add (Vector(460.79606994003683,219.10218442725133, 0.0));
    positionAlloc4->Add (Vector(218.33234238009314,413.80880484460096, 0.0));
    positionAlloc4->Add (Vector(130.4270297851698,405.1408938460427, 0.0));
    positionAlloc4->Add (Vector(289.4826904844621,425.42665928751416, 0.0));
    positionAlloc4->Add (Vector(47.59758237426814,169.18520570863188, 0.0));
    positionAlloc4->Add (Vector(25.217212684944702,382.88794460881337, 0.0));
    positionAlloc4->Add (Vector(257.67183131547745,223.42691994612306, 0.0));
    positionAlloc4->Add (Vector(316.78944495722135,50.59808892426215, 0.0));
    positionAlloc4->Add (Vector(77.52395354024088,130.93961020796112, 0.0));
    positionAlloc4->Add (Vector(294.82552035917195,441.10737608001665, 0.0));
    positionAlloc4->Add (Vector(45.40112094988519,124.40863222220506, 0.0));
    positionAlloc4->Add (Vector(109.36022935486206,110.82885472421444, 0.0));
    positionAlloc4->Add (Vector(254.18174574185525,210.57756191982364, 0.0));
    positionAlloc4->Add (Vector(117.58039722897668,377.19157271367345, 0.0));
    positionAlloc4->Add (Vector(435.2339244790807,417.0188429120742, 0.0));
    positionAlloc4->Add (Vector(118.90141675867766,48.51548329708039, 0.0));
    positionAlloc4->Add (Vector(334.0551315057972,34.867686590956126, 0.0));
    positionAlloc4->Add (Vector(218.1252460918962,4.738364168336451, 0.0));
    positionAlloc4->Add (Vector(350.97576280580455,416.7372882476111, 0.0));
    positionAlloc4->Add (Vector(378.9997162756992,96.46012493260176, 0.0));
    positionAlloc4->Add (Vector(119.830519333958,478.5025796535584, 0.0));
    positionAlloc4->Add (Vector(380.9763775730023,91.49919233275777, 0.0));
    positionAlloc4->Add (Vector(323.132193255017,483.7580256108621, 0.0));
    positionAlloc4->Add (Vector(67.73604677214179,254.1935260357525, 0.0));
    positionAlloc4->Add (Vector(288.8378546334225,171.19005243375358, 0.0));
    positionAlloc4->Add (Vector(457.5421022409352,315.7596875792048, 0.0));
    positionAlloc4->Add (Vector(96.90763886588871,304.2159217812978, 0.0));
    positionAlloc4->Add (Vector(201.17879042530868,36.918560733555154, 0.0));
    positionAlloc4->Add (Vector(269.1684869138469,225.51833168071482, 0.0));
    positionAlloc4->Add (Vector(188.46516276433474,32.20727779499716, 0.0));
    positionAlloc4->Add (Vector(363.1647103769414,228.02445263410743, 0.0));
    positionAlloc4->Add (Vector(225.53207645579997,314.97401154692784, 0.0));
    positionAlloc4->Add (Vector(103.71224044343197,80.39445648790134, 0.0));
    positionAlloc4->Add (Vector(41.13694614502034,25.646630087137744, 0.0));
    positionAlloc4->Add (Vector(235.70583884814488,273.6354814178583, 0.0));
    positionAlloc4->Add (Vector(96.54465895707281,497.7729218835499, 0.0));
    positionAlloc4->Add (Vector(58.48607375924764,412.3518203643388, 0.0));
    positionAlloc4->Add (Vector(100.02054736046856,431.11110657235804, 0.0));
    positionAlloc4->Add (Vector(275.41043757538915,57.9749546400809, 0.0));
    positionAlloc4->Add (Vector(1.6326227628042922,259.2932447615582, 0.0));
    positionAlloc4->Add (Vector(53.89258731167856,434.449159921279, 0.0));
    positionAlloc4->Add (Vector(146.90172807419373,74.96522887115364, 0.0));
    positionAlloc4->Add (Vector(4.9744335479886,322.771100699219, 0.0));
    positionAlloc4->Add (Vector(427.30623895441653,199.29450795999315, 0.0));
    positionAlloc4->Add (Vector(41.71540237756011,208.81465552525202, 0.0));
    positionAlloc4->Add (Vector(494.7660882436749,28.786286462081033, 0.0));
    positionAlloc4->Add (Vector(95.06091414771606,265.91315579591145, 0.0));
    positionAlloc4->Add (Vector(434.90105726224374,439.5988496647544, 0.0));
    positionAlloc4->Add (Vector(343.74865676737,171.1658491310802, 0.0));
    positionAlloc4->Add (Vector(103.09111677724658,274.0960981595863, 0.0));
    positionAlloc4->Add (Vector(18.47382363857425,80.78714461363296, 0.0));
    positionAlloc4->Add (Vector(356.5060764201909,445.2125759358876, 0.0));
    positionAlloc4->Add (Vector(360.81691815376536,270.3812735514397, 0.0));
    positionAlloc4->Add (Vector(23.997017325351642,149.45414975578296, 0.0));
    positionAlloc4->Add (Vector(402.4509641777137,242.7328751559137, 0.0));
    positionAlloc4->Add (Vector(252.86287210107318,234.1973412232637, 0.0));
    positionAlloc4->Add (Vector(418.5296118410208,121.3856419034326, 0.0));
    positionAlloc4->Add (Vector(117.4000939857116,298.2917238714797, 0.0));
    positionAlloc4->Add (Vector(241.4141878897388,245.48367087488216, 0.0));
    positionAlloc4->Add (Vector(440.7946725677778,99.29551293358146, 0.0));
    positionAlloc4->Add (Vector(468.15887303055393,74.04199408605517, 0.0));
    positionAlloc4->Add (Vector(33.55833655214868,232.3918189875747, 0.0));
    positionAlloc4->Add (Vector(211.84461170509766,302.49108584606796, 0.0));
    positionAlloc4->Add (Vector(180.7200336845044,223.2805092464058, 0.0));
    positionAlloc4->Add (Vector(122.4020163803628,314.58526248331793, 0.0));
    positionAlloc4->Add (Vector(278.1633281406902,161.5093885041131, 0.0));
    positionAlloc4->Add (Vector(131.29660430348494,295.55446632887515, 0.0));
    positionAlloc4->Add (Vector(384.392462332693,211.32900907291818, 0.0));
    positionAlloc4->Add (Vector(397.52157110760834,429.96154552013445, 0.0));
    positionAlloc4->Add (Vector(121.35377331240748,207.72748771708683, 0.0));
    positionAlloc4->Add (Vector(490.1372640127888,95.95818100712344, 0.0));
    positionAlloc4->Add (Vector(99.52058914835449,246.17007708285482, 0.0));
    positionAlloc4->Add (Vector(390.48274631382594,302.32988196913186, 0.0));
    positionAlloc4->Add (Vector(111.34356712767412,241.09047929715172, 0.0));
    positionAlloc4->Add (Vector(199.47683565536906,394.4216546836282, 0.0));
    positionAlloc4->Add (Vector(434.1775447926315,103.16097026994325, 0.0));
    positionAlloc4->Add (Vector(347.0826878855374,362.3029747042654, 0.0));
    positionAlloc4->Add (Vector(243.54479759017738,148.15704464468925, 0.0));
    positionAlloc4->Add (Vector(489.50156113878455,475.321636838439, 0.0));
    positionAlloc4->Add (Vector(399.5368575687593,428.8815908148923, 0.0));
    positionAlloc4->Add (Vector(183.61431961913317,30.730907213539105, 0.0));
    positionAlloc4->Add (Vector(382.34839707237563,107.86367692488724, 0.0));
    positionAlloc4->Add (Vector(162.18696191249748,104.43890858328663, 0.0));
    positionAlloc4->Add (Vector(2.8477892151850814,200.93704766558795, 0.0));
    positionAlloc4->Add (Vector(153.43730731113092,278.1175075644367, 0.0));
    positionAlloc4->Add (Vector(62.87750704584022,211.76059759915245, 0.0));
    positionAlloc4->Add (Vector(355.0947987269048,126.07907446874445, 0.0));
    positionAlloc4->Add (Vector(466.7194094818692,87.92196724587065, 0.0));
    positionAlloc4->Add (Vector(237.35734996220236,97.77053263160268, 0.0));
    positionAlloc4->Add (Vector(122.1017735304703,421.3918631606397, 0.0));
    positionAlloc4->Add (Vector(189.49659305928762,16.58060540242018, 0.0));
    positionAlloc4->Add (Vector(202.82864787898552,21.80520844407752, 0.0));
    positionAlloc4->Add (Vector(359.6136326561568,326.2863276657293, 0.0));
    positionAlloc4->Add (Vector(97.31931712054737,336.22451809567366, 0.0));
    positionAlloc4->Add (Vector(440.2999135625369,406.7655477294498, 0.0));
    positionAlloc4->Add (Vector(420.3904553091914,238.23691776750096, 0.0));
    positionAlloc4->Add (Vector(277.45535159950066,456.1089757325249, 0.0));
    positionAlloc4->Add (Vector(62.067838335482186,229.27220482418704, 0.0));
    positionAlloc4->Add (Vector(45.963848356791736,331.1019712392963, 0.0));
    positionAlloc4->Add (Vector(490.239641491828,125.47001062465202, 0.0));
    positionAlloc4->Add (Vector(369.11429143823125,95.15615274541756, 0.0));
    positionAlloc4->Add (Vector(118.67446423089444,214.52411705979745, 0.0));
    positionAlloc4->Add (Vector(107.48742871668115,215.35770192237536, 0.0));
    positionAlloc4->Add (Vector(134.7083783156038,149.15259098647778, 0.0));
    positionAlloc4->Add (Vector(107.69316384688027,191.92986026027387, 0.0));
    positionAlloc4->Add (Vector(246.45421388414024,124.24832076116022, 0.0));
    positionAlloc4->Add (Vector(354.24351690065697,249.4242303341109, 0.0));
    positionAlloc4->Add (Vector(232.42305997751035,477.74324931030395, 0.0));
    positionAlloc4->Add (Vector(199.18871055137217,307.3646536202771, 0.0));
    positionAlloc4->Add (Vector(179.2016642753141,265.8966590951793, 0.0));
    positionAlloc4->Add (Vector(295.7782966591889,475.9428028825226, 0.0));
    positionAlloc4->Add (Vector(217.65375366072865,304.37989869737197, 0.0));
    positionAlloc4->Add (Vector(370.9164021450892,108.2765456061886, 0.0));
    positionAlloc4->Add (Vector(359.6302141907618,425.5689596919163, 0.0));
    positionAlloc4->Add (Vector(481.5180291477908,129.75648160273406, 0.0));
    positionAlloc4->Add (Vector(176.63200798475776,218.93699196664957, 0.0));
    positionAlloc4->Add (Vector(328.38103332708613,396.09697936946566, 0.0));
    positionAlloc4->Add (Vector(217.19297131827804,151.25039119958973, 0.0));
    positionAlloc4->Add (Vector(132.59876594113084,240.78320055301123, 0.0));
    positionAlloc4->Add (Vector(461.1671402389845,293.1654393903188, 0.0));
    positionAlloc4->Add (Vector(370.7189574640886,170.08183739288296, 0.0));
    positionAlloc4->Add (Vector(91.25214902720408,488.00882791029716, 0.0));
    positionAlloc4->Add (Vector(256.2098773294574,192.82019658325817, 0.0));
    positionAlloc4->Add (Vector(157.20266942115606,416.26554268479236, 0.0));
    positionAlloc4->Add (Vector(43.615486096490464,46.51238210537312, 0.0));
    positionAlloc4->Add (Vector(124.71011539377008,129.56871529991477, 0.0));
    positionAlloc4->Add (Vector(332.49142948585467,485.9571777999964, 0.0));
    positionAlloc4->Add (Vector(348.4964344416001,29.935912991893133, 0.0));
    positionAlloc4->Add (Vector(58.73971036300718,194.56502730262397, 0.0));
    positionAlloc4->Add (Vector(34.705040445052184,376.610684917244, 0.0));
    positionAlloc4->Add (Vector(257.8583723195776,393.29567808230786, 0.0));
    positionAlloc4->Add (Vector(466.1831156639351,172.68388382898314, 0.0));
    positionAlloc4->Add (Vector(272.1057594995282,350.9761370316517, 0.0));
    positionAlloc4->Add (Vector(85.85275692324068,481.160866214935, 0.0));
    positionAlloc4->Add (Vector(134.30078334201107,290.9511194908813, 0.0));
    positionAlloc4->Add (Vector(51.09068908864239,209.6474809160797, 0.0));
    positionAlloc4->Add (Vector(144.25036648445928,252.12007922069319, 0.0));
    positionAlloc4->Add (Vector(83.93161899369073,245.55243377921022, 0.0));
    positionAlloc4->Add (Vector(321.70369397248106,63.216551693774214, 0.0));
    positionAlloc4->Add (Vector(497.83652952964627,127.45380882921525, 0.0));
    positionAlloc4->Add (Vector(307.59433886733837,29.55271823777822, 0.0));
    positionAlloc4->Add (Vector(230.72718113156358,273.0550106944482, 0.0));
    positionAlloc4->Add (Vector(401.7807960950513,363.43681050575344, 0.0));
    positionAlloc4->Add (Vector(54.51021904357217,348.9797974193469, 0.0));
    positionAlloc4->Add (Vector(237.97715012112675,480.4815369363734, 0.0));
    positionAlloc4->Add (Vector(46.72818633349296,389.2209301365922, 0.0));
    positionAlloc4->Add (Vector(92.23726692241985,256.44472818660586, 0.0));
    positionAlloc4->Add (Vector(371.7214711039459,185.54004453809526, 0.0));
    positionAlloc4->Add (Vector(41.16593479445962,366.8546659927584, 0.0));
    positionAlloc4->Add (Vector(282.30641346049396,127.41387113858066, 0.0));
    positionAlloc4->Add (Vector(216.26560644509362,33.497469639050095, 0.0));
    positionAlloc4->Add (Vector(403.4128092945502,261.5784470302129, 0.0));
    positionAlloc4->Add (Vector(50.83631097330954,371.22498701729114, 0.0));
    positionAlloc4->Add (Vector(56.5990173315406,232.83947432579026, 0.0));
    positionAlloc4->Add (Vector(284.1385436531143,490.1344660178944, 0.0));
    positionAlloc4->Add (Vector(362.55742301359186,455.11943560650127, 0.0));
    positionAlloc4->Add (Vector(308.82775594406706,319.9390465000264, 0.0));
    positionAlloc4->Add (Vector(182.2228845596753,251.08418845052392, 0.0));
    positionAlloc4->Add (Vector(338.5964022167846,40.86526395440831, 0.0));
    positionAlloc4->Add (Vector(496.6601968491591,422.77666534555516, 0.0));
    positionAlloc4->Add (Vector(330.2887152885226,277.25526672955544, 0.0));
    positionAlloc4->Add (Vector(128.29959685483706,461.96220079788924, 0.0));
    positionAlloc4->Add (Vector(435.1054370651817,285.8340301305531, 0.0));
    positionAlloc4->Add (Vector(82.5882740945379,162.05367832995975, 0.0));
    positionAlloc4->Add (Vector(90.97137751486517,382.07553257181036, 0.0));
    positionAlloc4->Add (Vector(343.5331948002455,340.4516875334669, 0.0));
    positionAlloc4->Add (Vector(278.51320460779317,193.06377637263256, 0.0));
    positionAlloc4->Add (Vector(403.07872010613363,403.39966205711585, 0.0));
    positionAlloc4->Add (Vector(434.5827074406109,234.71001306086248, 0.0));
    positionAlloc4->Add (Vector(233.38446485973103,152.48416638410973, 0.0));
    positionAlloc4->Add (Vector(351.7008579862737,208.72687776522235, 0.0));
    positionAlloc4->Add (Vector(292.5581160580431,80.23384310601716, 0.0));
    positionAlloc4->Add (Vector(493.36142521903116,472.0730329645817, 0.0));
    positionAlloc4->Add (Vector(397.89933907511454,107.9163054308962, 0.0));
    positionAlloc4->Add (Vector(340.1837767739466,2.3122242139083093, 0.0));
    positionAlloc4->Add (Vector(399.3736962546849,107.04343265501093, 0.0));
    positionAlloc4->Add (Vector(389.50728607357496,434.314595347914, 0.0));
    positionAlloc4->Add (Vector(498.05539403236077,267.0245775520459, 0.0));
    positionAlloc4->Add (Vector(351.68041084266616,205.6291951285972, 0.0));
    positionAlloc4->Add (Vector(61.184652937928185,43.34450378183319, 0.0));
    positionAlloc4->Add (Vector(73.20225633126853,333.0363698446093, 0.0));
    positionAlloc4->Add (Vector(59.67591423385199,222.36667691146616, 0.0));
    positionAlloc4->Add (Vector(412.4071752105195,290.8525028171993, 0.0));
    positionAlloc4->Add (Vector(204.8726386261418,204.10946319305583, 0.0));
    positionAlloc4->Add (Vector(490.32148496903477,230.20188086232002, 0.0));
    positionAlloc4->Add (Vector(12.787844577120511,154.21350950193036, 0.0));
    positionAlloc4->Add (Vector(2.2198126654972006,45.809754676776784, 0.0));
    positionAlloc4->Add (Vector(485.1858894264375,411.89109869403956, 0.0));
    positionAlloc4->Add (Vector(69.19658415382096,261.40118974364657, 0.0));
    positionAlloc4->Add (Vector(123.7458973000421,223.33800591668245, 0.0));
    positionAlloc4->Add (Vector(288.9805411453523,239.79796360664142, 0.0));
    positionAlloc4->Add (Vector(108.85287583527403,51.26002227661364, 0.0));
    positionAlloc4->Add (Vector(130.61435518110466,250.53469817215185, 0.0));
    positionAlloc4->Add (Vector(436.15429708936256,352.3328503553171, 0.0));
    positionAlloc4->Add (Vector(187.88041419907213,455.553351724413, 0.0));
    positionAlloc4->Add (Vector(300.26073570491616,368.9996835368004, 0.0));
    positionAlloc4->Add (Vector(247.41252676737398,368.57622243807276, 0.0));
    positionAlloc4->Add (Vector(474.30302884253155,458.5945175110866, 0.0));
    positionAlloc4->Add (Vector(398.9829078638153,490.58033754773936, 0.0));
    positionAlloc4->Add (Vector(248.4890497795354,417.78882087604194, 0.0));
    positionAlloc4->Add (Vector(424.42305089316307,294.0740542124017, 0.0));
    positionAlloc4->Add (Vector(218.81128531392184,294.9159215016586, 0.0));
    positionAlloc4->Add (Vector(155.33349256198025,330.7100033382605, 0.0));
    positionAlloc4->Add (Vector(480.2000111100719,100.46884636301112, 0.0));
    positionAlloc4->Add (Vector(166.9813477870485,79.91005065771884, 0.0));
    positionAlloc4->Add (Vector(133.65564325957658,66.63244764996368, 0.0));
    positionAlloc4->Add (Vector(414.19019282477063,335.9999679209135, 0.0));
    positionAlloc4->Add (Vector(234.55526836101097,473.6711267296803, 0.0));
    positionAlloc4->Add (Vector(474.0222527477173,187.9571461967927, 0.0));
    positionAlloc4->Add (Vector(165.5787195118658,163.21991410237314, 0.0));
    positionAlloc4->Add (Vector(280.122360518325,64.18995619671763, 0.0));
    positionAlloc4->Add (Vector(122.55020667237137,63.54999068002337, 0.0));
    positionAlloc4->Add (Vector(264.41170994483537,427.4200452624919, 0.0));
    positionAlloc4->Add (Vector(391.22689857423273,269.16942987179794, 0.0));
    positionAlloc4->Add (Vector(128.84742343624507,255.28327857103477, 0.0));
    positionAlloc4->Add (Vector(81.05250821351495,415.8834883963584, 0.0));
    positionAlloc4->Add (Vector(62.10866902933454,277.6961193535452, 0.0));
    positionAlloc4->Add (Vector(46.260112878519166,195.27867874462407, 0.0));
    positionAlloc4->Add (Vector(483.19409078604417,419.6229652873423, 0.0));
    positionAlloc4->Add (Vector(348.1478866643426,135.77525156712937, 0.0));
    positionAlloc4->Add (Vector(59.46018551060295,55.41903974005263, 0.0));
    positionAlloc4->Add (Vector(79.98881430499749,251.85275584457673, 0.0));
    positionAlloc4->Add (Vector(479.48678773833444,446.18204972774976, 0.0));
    positionAlloc4->Add (Vector(440.94695666002707,120.9456026977746, 0.0));
    positionAlloc4->Add (Vector(78.6439411427688,41.253364239040636, 0.0));
    positionAlloc4->Add (Vector(338.3875273037537,251.6328439849017, 0.0));
    positionAlloc4->Add (Vector(85.7536993629563,130.4589794805947, 0.0));
    positionAlloc4->Add (Vector(84.53736217900764,283.8062708484008, 0.0));
    positionAlloc4->Add (Vector(64.16286422560269,472.40664443496655, 0.0));
    positionAlloc4->Add (Vector(106.6994692576011,124.6194725171042, 0.0));
    positionAlloc4->Add (Vector(219.03759915804767,109.34454614883616, 0.0));
    positionAlloc4->Add (Vector(247.8356434651095,3.360662927243685, 0.0));
    positionAlloc4->Add (Vector(33.23418507793846,168.94370300351503, 0.0));
    positionAlloc4->Add (Vector(158.43706756461484,208.68423037948637, 0.0));
    positionAlloc4->Add (Vector(215.33985710096272,91.29370681801868, 0.0));
    positionAlloc4->Add (Vector(403.87801362111657,191.2838292124906, 0.0));
    positionAlloc4->Add (Vector(126.63184797379978,418.85639063808406, 0.0));
    positionAlloc4->Add (Vector(146.85254767209616,4.394524888183094, 0.0));
    positionAlloc4->Add (Vector(180.51821566063413,57.03496359420368, 0.0));
    positionAlloc4->Add (Vector(159.13782047999248,110.82117280364278, 0.0));
    positionAlloc4->Add (Vector(24.308187350705,409.6309222129109, 0.0));
    positionAlloc4->Add (Vector(176.62420789793615,351.231368682107, 0.0));
    positionAlloc4->Add (Vector(365.57088871561706,439.21663614316356, 0.0));
    positionAlloc4->Add (Vector(205.75594197879187,298.62928311022665, 0.0));
    positionAlloc4->Add (Vector(109.04980896406452,38.59764357994688, 0.0));
    positionAlloc4->Add (Vector(465.74808723685555,29.437447795631122, 0.0));
    positionAlloc4->Add (Vector(343.10027320981874,71.20277564580707, 0.0));
    positionAlloc4->Add (Vector(52.14704980476387,220.17187331153048, 0.0));
    positionAlloc4->Add (Vector(94.86280655213226,461.76383782121354, 0.0));
    positionAlloc4->Add (Vector(225.1619228580422,172.91185450915697, 0.0));
    positionAlloc4->Add (Vector(179.23172356377282,499.6876849202913, 0.0));
    positionAlloc4->Add (Vector(204.34006519962634,275.8843492816301, 0.0));
    positionAlloc4->Add (Vector(34.77485857014195,281.46549201630035, 0.0));
    positionAlloc4->Add (Vector(250.05183590734276,402.1474775421197, 0.0));
    positionAlloc4->Add (Vector(23.688912118021154,319.5191602969185, 0.0));
    positionAlloc4->Add (Vector(278.4160035654114,276.35496148975784, 0.0));
    positionAlloc4->Add (Vector(15.948631329538554,476.40489758671623, 0.0));
    positionAlloc4->Add (Vector(400.7528067533053,126.55741670647575, 0.0));
    positionAlloc4->Add (Vector(135.4120596959541,495.27526478841685, 0.0));
    positionAlloc4->Add (Vector(499.4902872071254,395.2536890875645, 0.0));
    positionAlloc4->Add (Vector(478.8046217138764,313.7541750498555, 0.0));
    positionAlloc4->Add (Vector(431.01552844591765,448.2814102718168, 0.0));
    positionAlloc4->Add (Vector(290.37518192355475,359.3368051336281, 0.0));
    positionAlloc4->Add (Vector(127.86793179673217,445.8673983032627, 0.0));
    positionAlloc4->Add (Vector(142.04130758438117,274.2385680603557, 0.0));
    positionAlloc4->Add (Vector(211.10607666369785,76.95898282591773, 0.0));
    positionAlloc4->Add (Vector(335.1701999510562,372.5329455048796, 0.0));
    positionAlloc4->Add (Vector(440.29845096595096,235.12029976434917, 0.0));
    positionAlloc4->Add (Vector(326.12782182139324,457.74838347180685, 0.0));
    positionAlloc4->Add (Vector(18.80352899740023,440.38445324256685, 0.0));
    positionAlloc4->Add (Vector(218.574305608659,54.75833106866646, 0.0));
    positionAlloc4->Add (Vector(360.87033059652333,451.8588752333612, 0.0));
    positionAlloc4->Add (Vector(26.015361895233514,337.6428032480499, 0.0));
    positionAlloc4->Add (Vector(162.7874097199159,161.07650935056432, 0.0));
    positionAlloc4->Add (Vector(236.17310633785797,469.49756522855137, 0.0));
    positionAlloc4->Add (Vector(44.06503415605145,9.426023127697302, 0.0));
    positionAlloc4->Add (Vector(122.23314640094112,336.6812196999265, 0.0));
    positionAlloc4->Add (Vector(136.23069608035144,415.46662416985544, 0.0));
    positionAlloc4->Add (Vector(301.82772590507597,332.18747200904966, 0.0));
    positionAlloc4->Add (Vector(240.71475825463884,418.90637695147973, 0.0));
    positionAlloc4->Add (Vector(254.8070577185748,497.7610131879095, 0.0));
    positionAlloc4->Add (Vector(449.0131859652526,60.417342026918455, 0.0));
    positionAlloc4->Add (Vector(290.555332817891,298.3131838071353, 0.0));
    positionAlloc4->Add (Vector(245.9989011967519,245.0242837902383, 0.0));
    positionAlloc4->Add (Vector(389.8417442990644,371.83278852472756, 0.0));
    positionAlloc4->Add (Vector(92.88335061520291,472.09667484579205, 0.0));
    positionAlloc4->Add (Vector(38.63363285403199,197.95124761656973, 0.0));
    positionAlloc4->Add (Vector(396.98994999037524,16.97330834857047, 0.0));
    positionAlloc4->Add (Vector(444.69961445238846,113.73928739815537, 0.0));
    positionAlloc4->Add (Vector(429.12013738223555,285.2909957892557, 0.0));
    positionAlloc4->Add (Vector(375.0626895163639,278.4891342947407, 0.0));
    positionAlloc4->Add (Vector(338.9570814305857,168.04993532864577, 0.0));
    positionAlloc4->Add (Vector(3.373905644321107,129.52977690603333, 0.0));
    positionAlloc4->Add (Vector(341.35443277816086,381.2285477481747, 0.0));
    positionAlloc4->Add (Vector(317.43781150074545,176.15493050590936, 0.0));
    positionAlloc4->Add (Vector(220.9854527325405,67.44575981338008, 0.0));
    positionAlloc4->Add (Vector(407.2777194088172,255.23339453324735, 0.0));
    positionAlloc4->Add (Vector(251.38685041487497,47.08883213770265, 0.0));
    positionAlloc4->Add (Vector(378.4891164534548,149.29605777786526, 0.0));
    positionAlloc4->Add (Vector(354.4033349648718,49.69180735313028, 0.0));
    positionAlloc4->Add (Vector(321.81267824051747,238.2843082266013, 0.0));
    positionAlloc4->Add (Vector(118.90433047539284,415.48477722197157, 0.0));
    positionAlloc4->Add (Vector(18.38831270760649,200.2517814866193, 0.0));
    positionAlloc4->Add (Vector(194.81477171311712,132.67198541557767, 0.0));
    positionAlloc4->Add (Vector(446.60124860001065,28.731691958395366, 0.0));
    positionAlloc4->Add (Vector(1.4707217081220136,200.29854410392417, 0.0));
    positionAlloc4->Add (Vector(88.51552729980399,200.87821510351634, 0.0));
    positionAlloc4->Add (Vector(365.3206754427353,388.916349856137, 0.0));
    positionAlloc4->Add (Vector(152.41124633566105,176.54885400041425, 0.0));
    positionAlloc4->Add (Vector(343.98945718738395,419.22002064006216, 0.0));
    positionAlloc4->Add (Vector(79.40501339130628,153.7817392852842, 0.0));
    positionAlloc4->Add (Vector(65.67089924464126,183.83681269329082, 0.0));
    positionAlloc4->Add (Vector(193.89119180327867,88.96573040738004, 0.0));
    positionAlloc4->Add (Vector(184.72334008343176,39.98154451064656, 0.0));
    positionAlloc4->Add (Vector(489.5733152883739,205.84452182653473, 0.0));
    positionAlloc4->Add (Vector(226.74157124925958,241.2649399486803, 0.0));
    positionAlloc4->Add (Vector(172.82638231161997,452.7313202919183, 0.0));
    positionAlloc4->Add (Vector(280.0739307620069,168.87320812366713, 0.0));
    positionAlloc4->Add (Vector(180.6622668691459,351.71607977733504, 0.0));
    positionAlloc4->Add (Vector(35.036461893010575,442.97748720639765, 0.0));
    positionAlloc4->Add (Vector(365.56736809004497,47.53099459702453, 0.0));
    positionAlloc4->Add (Vector(106.89210725921683,320.55251731381856, 0.0));
    positionAlloc4->Add (Vector(373.39138842556594,396.98523279697133, 0.0));
    positionAlloc4->Add (Vector(28.169863025996722,303.2011861371705, 0.0));
    positionAlloc4->Add (Vector(242.15268522715704,332.16063522491567, 0.0));
    positionAlloc4->Add (Vector(291.7080192990712,177.08488288242063, 0.0));
    positionAlloc4->Add (Vector(184.30127813549086,326.36503579219203, 0.0));
    positionAlloc4->Add (Vector(471.2440480790739,148.18136679165673, 0.0));
    positionAlloc4->Add (Vector(419.69113765481234,208.40459362852081, 0.0));
    positionAlloc4->Add (Vector(405.39081124445227,184.22278666186824, 0.0));
    positionAlloc4->Add (Vector(187.14791814209158,291.2438437279878, 0.0));
    positionAlloc4->Add (Vector(314.9358972637818,83.38556480171, 0.0));
    positionAlloc4->Add (Vector(264.61698826911686,241.3426258325002, 0.0));
    positionAlloc4->Add (Vector(98.81890310690672,154.81642831349734, 0.0));
    positionAlloc4->Add (Vector(434.4472222770536,141.70732894048538, 0.0));
    positionAlloc4->Add (Vector(420.93370443115964,6.602479111703863, 0.0));
    positionAlloc4->Add (Vector(226.53309219540475,491.60119086419223, 0.0));
    positionAlloc4->Add (Vector(24.80364386877548,73.308185917416, 0.0));
    positionAlloc4->Add (Vector(43.047328382005034,179.96610795890882, 0.0));
    positionAlloc4->Add (Vector(334.7817097777251,108.99538130250674, 0.0));
    positionAlloc4->Add (Vector(338.59013142028766,56.31464856149576, 0.0));
    positionAlloc4->Add (Vector(187.36594369555522,451.77734233605213, 0.0));
    positionAlloc4->Add (Vector(309.7953512092076,284.06006105779767, 0.0));
    positionAlloc4->Add (Vector(81.00697849485434,208.91346400184545, 0.0));
    positionAlloc4->Add (Vector(476.40180971143997,487.8270978523901, 0.0));
    positionAlloc4->Add (Vector(279.53327907879054,39.20539954989161, 0.0));
    positionAlloc4->Add (Vector(234.15615299712096,353.0967800148086, 0.0));
    positionAlloc4->Add (Vector(369.6350480464106,361.1511245091931, 0.0));
    positionAlloc4->Add (Vector(126.2475145487707,253.0104765969751, 0.0));
    positionAlloc4->Add (Vector(280.77314746480585,358.80617671139834, 0.0));
    positionAlloc4->Add (Vector(121.28565232702499,110.75348698959436, 0.0));
    positionAlloc4->Add (Vector(418.8230863159628,2.8465879647039394, 0.0));
    positionAlloc4->Add (Vector(243.45098644832836,267.9600186480317, 0.0));
    positionAlloc4->Add (Vector(343.4818514098243,444.7517830458456, 0.0));
    positionAlloc4->Add (Vector(281.5832140193667,398.09147633051157, 0.0));
    positionAlloc4->Add (Vector(173.27388910996376,324.0799629662421, 0.0));
    positionAlloc4->Add (Vector(19.49170826720492,107.73289523161989, 0.0));
    positionAlloc4->Add (Vector(147.87176976995937,429.98086980660776, 0.0));
    positionAlloc4->Add (Vector(76.11435410599971,35.227931774985734, 0.0));
    positionAlloc4->Add (Vector(204.15236466855546,187.40289950537647, 0.0));
    positionAlloc4->Add (Vector(441.7784585028234,121.79006076759109, 0.0));
    positionAlloc4->Add (Vector(469.06922432458185,492.36882412068155, 0.0));
    positionAlloc4->Add (Vector(336.7774603485536,442.41822613358863, 0.0));
    positionAlloc4->Add (Vector(394.53760108441907,233.23385179168554, 0.0));
    positionAlloc4->Add (Vector(106.58649549517868,217.94423736763514, 0.0));
    positionAlloc4->Add (Vector(156.8792150770056,307.8931119955987, 0.0));
    positionAlloc4->Add (Vector(114.99305237641111,259.05885012944964, 0.0));
    positionAlloc4->Add (Vector(420.91162183273786,373.8899966547793, 0.0));
    positionAlloc4->Add (Vector(464.866671604286,37.90419831792008, 0.0));
    positionAlloc4->Add (Vector(14.766342651900521,293.59212511246255, 0.0));
    positionAlloc4->Add (Vector(108.55590360231176,211.29823223637055, 0.0));
    positionAlloc4->Add (Vector(166.50533331444305,250.55507497048458, 0.0));
    positionAlloc4->Add (Vector(430.79840689910975,14.585215452510125, 0.0));
    positionAlloc4->Add (Vector(448.6372347523148,293.8673496802047, 0.0));
    positionAlloc4->Add (Vector(452.1883873825857,23.015350384473887, 0.0));
    positionAlloc4->Add (Vector(262.17758737692515,106.28612509229518, 0.0));
    positionAlloc4->Add (Vector(102.75455221583796,49.66144347608176, 0.0));
    positionAlloc4->Add (Vector(83.63578260981997,161.01120298140864, 0.0));
    positionAlloc4->Add (Vector(239.1390537751596,411.02695793500584, 0.0));
    positionAlloc4->Add (Vector(294.96999040507916,204.2900600158828, 0.0));
    positionAlloc4->Add (Vector(21.893087322857717,68.48564501518072, 0.0));
    positionAlloc4->Add (Vector(210.43177339553165,420.7394941421928, 0.0));
    positionAlloc4->Add (Vector(175.3875430681502,80.96206550918622, 0.0));
    positionAlloc4->Add (Vector(98.50293805335752,455.9195482496445, 0.0));
    positionAlloc4->Add (Vector(310.14524977711466,486.3686986104602, 0.0));
    positionAlloc4->Add (Vector(123.72279007107872,219.98017727489187, 0.0));
    positionAlloc4->Add (Vector(70.70843935068483,353.61630586032067, 0.0));
    positionAlloc4->Add (Vector(22.63501807937568,269.04257245514805, 0.0));
    positionAlloc4->Add (Vector(340.97331643535347,18.36184193665713, 0.0));
    positionAlloc4->Add (Vector(257.4996230186949,418.12722881086523, 0.0));
    positionAlloc4->Add (Vector(117.81312873978011,38.36699342577149, 0.0));
    positionAlloc4->Add (Vector(75.07542039348452,269.12345488591893, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_12.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_ECC_flowmon/switch_ECC_12.flowmon", false, false);
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