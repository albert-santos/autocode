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
	  uint16_t numberOfRrhs = 60;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 592;
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
    positionAlloc->Add (Vector (500.0,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,500.0, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,500.0, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,500.0, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,111.11111111111111, 0.0));
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
    positionAlloc->Add (Vector (111.11111111111111,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,500.0, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (0.0,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (0.0,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (0.0,111.11111111111111, 0.0));
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
    positionAlloc4->Add (Vector(223.5338604932941,122.28724506916305, 0.0));
    positionAlloc4->Add (Vector(263.7090920586311,44.82398054689718, 0.0));
    positionAlloc4->Add (Vector(24.263396952094062,304.5031447021174, 0.0));
    positionAlloc4->Add (Vector(259.7147836747218,355.79914559784726, 0.0));
    positionAlloc4->Add (Vector(143.67511687732176,416.2648436224208, 0.0));
    positionAlloc4->Add (Vector(191.80559775899957,82.79488434707282, 0.0));
    positionAlloc4->Add (Vector(62.61519872661292,277.8891953598306, 0.0));
    positionAlloc4->Add (Vector(421.6352966769054,407.17079084420646, 0.0));
    positionAlloc4->Add (Vector(432.2293224243928,486.3897532079372, 0.0));
    positionAlloc4->Add (Vector(276.93389189240764,110.73649365165895, 0.0));
    positionAlloc4->Add (Vector(334.577768234401,414.3883181970686, 0.0));
    positionAlloc4->Add (Vector(203.19922136904256,389.53829043227046, 0.0));
    positionAlloc4->Add (Vector(476.5553334925262,217.3180562228245, 0.0));
    positionAlloc4->Add (Vector(44.34067237822664,390.55112524375767, 0.0));
    positionAlloc4->Add (Vector(401.98197526177074,25.27591453694289, 0.0));
    positionAlloc4->Add (Vector(18.77594077099598,461.39496388296885, 0.0));
    positionAlloc4->Add (Vector(4.968754510091433,8.783482714725842, 0.0));
    positionAlloc4->Add (Vector(384.86240865308406,259.4359597447943, 0.0));
    positionAlloc4->Add (Vector(468.96987718669504,327.09602476795294, 0.0));
    positionAlloc4->Add (Vector(403.0371727632949,133.31814579821167, 0.0));
    positionAlloc4->Add (Vector(282.3957553902192,375.1138444703926, 0.0));
    positionAlloc4->Add (Vector(237.51245046443188,489.5148243516935, 0.0));
    positionAlloc4->Add (Vector(57.00412761280488,385.2541318867121, 0.0));
    positionAlloc4->Add (Vector(416.38248591503077,487.39049514532934, 0.0));
    positionAlloc4->Add (Vector(334.1940736576239,207.0558538179909, 0.0));
    positionAlloc4->Add (Vector(450.8620128987629,427.0722763355527, 0.0));
    positionAlloc4->Add (Vector(323.9917424526577,144.07186902089668, 0.0));
    positionAlloc4->Add (Vector(113.34929959062767,397.5218863155699, 0.0));
    positionAlloc4->Add (Vector(357.25524499916827,358.58110529246636, 0.0));
    positionAlloc4->Add (Vector(392.10969177564965,426.8299882510428, 0.0));
    positionAlloc4->Add (Vector(499.291300464469,209.614491950085, 0.0));
    positionAlloc4->Add (Vector(142.94855024320086,351.13628589580065, 0.0));
    positionAlloc4->Add (Vector(117.6381762075101,0.4519916180584782, 0.0));
    positionAlloc4->Add (Vector(493.7528957737977,333.4188890332282, 0.0));
    positionAlloc4->Add (Vector(446.52670670554704,108.43472271642857, 0.0));
    positionAlloc4->Add (Vector(24.71207580301288,317.2252742227426, 0.0));
    positionAlloc4->Add (Vector(394.5212498542634,309.07197128049796, 0.0));
    positionAlloc4->Add (Vector(295.1587283337661,133.42219866404957, 0.0));
    positionAlloc4->Add (Vector(341.65462922833905,301.43942767363956, 0.0));
    positionAlloc4->Add (Vector(418.2267926835385,81.6945548599336, 0.0));
    positionAlloc4->Add (Vector(27.987536092126565,433.11012698185397, 0.0));
    positionAlloc4->Add (Vector(393.52610331589193,383.2238452781858, 0.0));
    positionAlloc4->Add (Vector(359.99461031936494,388.24409208312096, 0.0));
    positionAlloc4->Add (Vector(4.36489395664691,90.45753968700055, 0.0));
    positionAlloc4->Add (Vector(225.32183849587685,299.34671053506133, 0.0));
    positionAlloc4->Add (Vector(347.01311121728685,284.12676947141415, 0.0));
    positionAlloc4->Add (Vector(181.87034134152225,163.01424130534497, 0.0));
    positionAlloc4->Add (Vector(78.51556598583359,449.29025621952036, 0.0));
    positionAlloc4->Add (Vector(150.74516840623008,176.71921483621227, 0.0));
    positionAlloc4->Add (Vector(244.4057360522851,442.86705368238376, 0.0));
    positionAlloc4->Add (Vector(285.33694947800734,427.13649173506747, 0.0));
    positionAlloc4->Add (Vector(356.0124325162659,247.63800783031104, 0.0));
    positionAlloc4->Add (Vector(139.24574887004977,479.77641107710633, 0.0));
    positionAlloc4->Add (Vector(478.69215520559817,471.27178957059874, 0.0));
    positionAlloc4->Add (Vector(482.5927404243886,61.09082369767438, 0.0));
    positionAlloc4->Add (Vector(145.55661071618135,486.40591891153565, 0.0));
    positionAlloc4->Add (Vector(397.43219449182055,445.07834629323537, 0.0));
    positionAlloc4->Add (Vector(477.5424302308962,299.6998793352728, 0.0));
    positionAlloc4->Add (Vector(406.9809968384751,13.686736369404818, 0.0));
    positionAlloc4->Add (Vector(327.4520989372339,288.2723852348397, 0.0));
    positionAlloc4->Add (Vector(53.654115117588,385.6491383724833, 0.0));
    positionAlloc4->Add (Vector(197.21637779439672,124.8089253246491, 0.0));
    positionAlloc4->Add (Vector(127.6670292420869,150.7938272238082, 0.0));
    positionAlloc4->Add (Vector(338.554584679125,97.34079070127466, 0.0));
    positionAlloc4->Add (Vector(451.79949569951543,160.29893872166988, 0.0));
    positionAlloc4->Add (Vector(338.778721848383,154.68676241958724, 0.0));
    positionAlloc4->Add (Vector(333.4257385418063,412.69977405426226, 0.0));
    positionAlloc4->Add (Vector(455.0459598756165,437.6873279925766, 0.0));
    positionAlloc4->Add (Vector(474.7771470449312,142.8770445827803, 0.0));
    positionAlloc4->Add (Vector(212.67986638475088,404.79010939153534, 0.0));
    positionAlloc4->Add (Vector(141.21347247327105,324.07500359966457, 0.0));
    positionAlloc4->Add (Vector(21.165758837874414,105.07834588568615, 0.0));
    positionAlloc4->Add (Vector(157.38302383402402,54.3009886528516, 0.0));
    positionAlloc4->Add (Vector(195.5209135971895,221.79103520979083, 0.0));
    positionAlloc4->Add (Vector(53.46986451163666,142.37830534879674, 0.0));
    positionAlloc4->Add (Vector(301.4573849432832,408.5355108432485, 0.0));
    positionAlloc4->Add (Vector(402.07840102412837,206.68991698278754, 0.0));
    positionAlloc4->Add (Vector(465.59707917418007,46.38543621250607, 0.0));
    positionAlloc4->Add (Vector(214.1970378795161,282.39931159369047, 0.0));
    positionAlloc4->Add (Vector(104.76148737013413,209.37630407291437, 0.0));
    positionAlloc4->Add (Vector(94.33690523168242,136.29641904352025, 0.0));
    positionAlloc4->Add (Vector(143.15118308747694,399.5919405123232, 0.0));
    positionAlloc4->Add (Vector(81.60267607217509,363.2732733956248, 0.0));
    positionAlloc4->Add (Vector(143.05943603997068,38.759977643404056, 0.0));
    positionAlloc4->Add (Vector(317.9491239060354,59.65016702768266, 0.0));
    positionAlloc4->Add (Vector(32.88991934655661,292.4940683013634, 0.0));
    positionAlloc4->Add (Vector(409.30315266041856,45.354743254715224, 0.0));
    positionAlloc4->Add (Vector(309.2091650445141,380.3658797938231, 0.0));
    positionAlloc4->Add (Vector(435.9081165975085,345.84560763188614, 0.0));
    positionAlloc4->Add (Vector(19.551353185723986,435.2941524070308, 0.0));
    positionAlloc4->Add (Vector(19.746595188394,149.55349301906108, 0.0));
    positionAlloc4->Add (Vector(212.00246126328832,62.95277334012028, 0.0));
    positionAlloc4->Add (Vector(231.98283116016017,340.73592969542005, 0.0));
    positionAlloc4->Add (Vector(200.67335218832167,238.58493736138863, 0.0));
    positionAlloc4->Add (Vector(321.87005229998476,217.73824365290795, 0.0));
    positionAlloc4->Add (Vector(171.13739586470822,11.463646054879373, 0.0));
    positionAlloc4->Add (Vector(261.7061044188862,358.95977224916163, 0.0));
    positionAlloc4->Add (Vector(175.08466408211493,403.11188116611993, 0.0));
    positionAlloc4->Add (Vector(363.19239086562965,307.5963114158814, 0.0));
    positionAlloc4->Add (Vector(328.8530389731638,4.676359169810129, 0.0));
    positionAlloc4->Add (Vector(319.32126782032367,163.99860133563993, 0.0));
    positionAlloc4->Add (Vector(432.70760003304747,395.4936386650464, 0.0));
    positionAlloc4->Add (Vector(441.0461875891189,36.19800035566739, 0.0));
    positionAlloc4->Add (Vector(17.294054317248396,299.534174319968, 0.0));
    positionAlloc4->Add (Vector(46.21050993057385,359.60458182711784, 0.0));
    positionAlloc4->Add (Vector(122.34777497709953,323.1656749629241, 0.0));
    positionAlloc4->Add (Vector(433.56232239326386,59.78030489357733, 0.0));
    positionAlloc4->Add (Vector(170.22336757684286,181.30788444310747, 0.0));
    positionAlloc4->Add (Vector(154.31151053397318,272.28640451793103, 0.0));
    positionAlloc4->Add (Vector(165.95052178693376,194.4305244673501, 0.0));
    positionAlloc4->Add (Vector(120.60775222085557,427.90646687913693, 0.0));
    positionAlloc4->Add (Vector(156.11664400065246,387.92677896498463, 0.0));
    positionAlloc4->Add (Vector(308.0940818903384,490.5235035724957, 0.0));
    positionAlloc4->Add (Vector(107.05140830047988,385.302414050895, 0.0));
    positionAlloc4->Add (Vector(182.59012084457572,122.75430658194159, 0.0));
    positionAlloc4->Add (Vector(51.67776863294227,114.35725830547095, 0.0));
    positionAlloc4->Add (Vector(327.93946274068844,342.7653485561643, 0.0));
    positionAlloc4->Add (Vector(195.19860589797443,447.4678613634795, 0.0));
    positionAlloc4->Add (Vector(263.5881558138099,66.55160649517971, 0.0));
    positionAlloc4->Add (Vector(492.53607560411933,228.2158719804614, 0.0));
    positionAlloc4->Add (Vector(169.3664727105008,387.75272609331597, 0.0));
    positionAlloc4->Add (Vector(464.221725413496,249.6249610556025, 0.0));
    positionAlloc4->Add (Vector(166.55350878853346,200.80821678463724, 0.0));
    positionAlloc4->Add (Vector(269.009808654344,439.5142661418953, 0.0));
    positionAlloc4->Add (Vector(174.0231417615659,340.1178064184098, 0.0));
    positionAlloc4->Add (Vector(340.074787097207,40.14516509455562, 0.0));
    positionAlloc4->Add (Vector(23.61079014538059,389.5553388487733, 0.0));
    positionAlloc4->Add (Vector(259.9653672724207,174.91947752515878, 0.0));
    positionAlloc4->Add (Vector(462.83437977206916,71.29827817498968, 0.0));
    positionAlloc4->Add (Vector(195.2836304214562,445.51385152127006, 0.0));
    positionAlloc4->Add (Vector(187.66025940045273,285.92728860660344, 0.0));
    positionAlloc4->Add (Vector(79.36346269284417,162.4095562740936, 0.0));
    positionAlloc4->Add (Vector(264.82792871141,282.7959942921201, 0.0));
    positionAlloc4->Add (Vector(267.28913477612184,121.84396833595807, 0.0));
    positionAlloc4->Add (Vector(82.01641280169464,499.37188899063995, 0.0));
    positionAlloc4->Add (Vector(180.840605927536,495.83720451915116, 0.0));
    positionAlloc4->Add (Vector(363.168528081315,152.66347171245158, 0.0));
    positionAlloc4->Add (Vector(85.0929817947464,74.58661982213971, 0.0));
    positionAlloc4->Add (Vector(472.52749473915947,105.6635281427652, 0.0));
    positionAlloc4->Add (Vector(157.4403559848908,292.72201885714134, 0.0));
    positionAlloc4->Add (Vector(491.55846994322286,436.40202145433534, 0.0));
    positionAlloc4->Add (Vector(238.55894997893668,140.90958393532154, 0.0));
    positionAlloc4->Add (Vector(191.04689310635214,407.658360032096, 0.0));
    positionAlloc4->Add (Vector(116.41338075384544,360.68480382592213, 0.0));
    positionAlloc4->Add (Vector(246.88404174368168,341.322919416614, 0.0));
    positionAlloc4->Add (Vector(67.81681051143008,253.88851207990558, 0.0));
    positionAlloc4->Add (Vector(40.33415374908322,480.47438303138364, 0.0));
    positionAlloc4->Add (Vector(215.2373611902485,252.63369941982583, 0.0));
    positionAlloc4->Add (Vector(118.67859467960085,259.54903556847637, 0.0));
    positionAlloc4->Add (Vector(225.7473747517677,422.1284339801835, 0.0));
    positionAlloc4->Add (Vector(166.68894981447065,437.4038553018194, 0.0));
    positionAlloc4->Add (Vector(440.95633207087053,119.1633523292831, 0.0));
    positionAlloc4->Add (Vector(461.1370710573815,476.3933084383557, 0.0));
    positionAlloc4->Add (Vector(29.757197695992833,245.14559824785232, 0.0));
    positionAlloc4->Add (Vector(173.7371324482968,231.59638103159236, 0.0));
    positionAlloc4->Add (Vector(467.39830661884366,256.1374766846647, 0.0));
    positionAlloc4->Add (Vector(186.8954577058104,387.91082858207443, 0.0));
    positionAlloc4->Add (Vector(251.976326948857,172.1131277700726, 0.0));
    positionAlloc4->Add (Vector(274.17409393120823,36.491585466997215, 0.0));
    positionAlloc4->Add (Vector(293.81407073020125,123.05141067002423, 0.0));
    positionAlloc4->Add (Vector(288.6775986187132,384.2356880311318, 0.0));
    positionAlloc4->Add (Vector(311.5631731673381,354.6640126111663, 0.0));
    positionAlloc4->Add (Vector(472.2597375613096,4.771165388089971, 0.0));
    positionAlloc4->Add (Vector(13.944863706817024,331.9218764987691, 0.0));
    positionAlloc4->Add (Vector(82.36244573303757,86.57507430923866, 0.0));
    positionAlloc4->Add (Vector(434.9022131989057,266.2687588054453, 0.0));
    positionAlloc4->Add (Vector(37.518238468143615,331.7682560563946, 0.0));
    positionAlloc4->Add (Vector(12.036845522099327,278.6847950115591, 0.0));
    positionAlloc4->Add (Vector(275.37250908872556,397.25198438615763, 0.0));
    positionAlloc4->Add (Vector(280.49102951919497,168.9440843290541, 0.0));
    positionAlloc4->Add (Vector(10.474999352300951,190.43172662003545, 0.0));
    positionAlloc4->Add (Vector(401.04505843572537,484.6582768583132, 0.0));
    positionAlloc4->Add (Vector(45.48544022024409,235.80796260803706, 0.0));
    positionAlloc4->Add (Vector(16.072020931603927,405.4917779056969, 0.0));
    positionAlloc4->Add (Vector(135.46394686308787,371.86818042849933, 0.0));
    positionAlloc4->Add (Vector(203.5654560996066,258.53469673692103, 0.0));
    positionAlloc4->Add (Vector(265.7920342470601,42.91273436424359, 0.0));
    positionAlloc4->Add (Vector(348.73072257638927,412.30507880275496, 0.0));
    positionAlloc4->Add (Vector(52.57443522903088,13.805526486060094, 0.0));
    positionAlloc4->Add (Vector(383.74568404781564,166.55732879562763, 0.0));
    positionAlloc4->Add (Vector(251.22129673143527,453.592254078369, 0.0));
    positionAlloc4->Add (Vector(232.11151144102882,394.32779576531163, 0.0));
    positionAlloc4->Add (Vector(352.4815119205333,32.198016059040825, 0.0));
    positionAlloc4->Add (Vector(478.6680002333479,237.2339140191334, 0.0));
    positionAlloc4->Add (Vector(259.92574018828253,408.97805325011547, 0.0));
    positionAlloc4->Add (Vector(392.17468316218634,190.67006627320464, 0.0));
    positionAlloc4->Add (Vector(183.7748380494262,418.215611563945, 0.0));
    positionAlloc4->Add (Vector(93.87481281820087,391.31810709803335, 0.0));
    positionAlloc4->Add (Vector(411.4582687276487,250.7094419959361, 0.0));
    positionAlloc4->Add (Vector(12.878391332641847,345.5791410799983, 0.0));
    positionAlloc4->Add (Vector(396.8083542968167,403.8065498700015, 0.0));
    positionAlloc4->Add (Vector(162.36157434011068,384.08035692060196, 0.0));
    positionAlloc4->Add (Vector(410.66215621176974,326.76062651909047, 0.0));
    positionAlloc4->Add (Vector(437.9307662147516,77.54391458403775, 0.0));
    positionAlloc4->Add (Vector(236.23931978064795,370.1797738565824, 0.0));
    positionAlloc4->Add (Vector(406.4726820239554,387.23140613104397, 0.0));
    positionAlloc4->Add (Vector(184.45271983659129,371.6948780325442, 0.0));
    positionAlloc4->Add (Vector(349.75882049144417,70.75646167562482, 0.0));
    positionAlloc4->Add (Vector(468.74806092075,338.1778234796087, 0.0));
    positionAlloc4->Add (Vector(106.56905620170998,246.2039465876959, 0.0));
    positionAlloc4->Add (Vector(227.43774315086173,269.12690926515137, 0.0));
    positionAlloc4->Add (Vector(137.5024595798704,225.16747496525446, 0.0));
    positionAlloc4->Add (Vector(107.10123318904962,355.40243543337965, 0.0));
    positionAlloc4->Add (Vector(128.7060994266556,346.13974639261517, 0.0));
    positionAlloc4->Add (Vector(168.5647488474852,209.35036777492044, 0.0));
    positionAlloc4->Add (Vector(361.51512356360604,261.29097134848575, 0.0));
    positionAlloc4->Add (Vector(462.1745803305862,249.1675943824757, 0.0));
    positionAlloc4->Add (Vector(47.09076744010093,267.62277672443923, 0.0));
    positionAlloc4->Add (Vector(465.00658021389194,363.1790396374073, 0.0));
    positionAlloc4->Add (Vector(168.45658355411064,386.6936704740781, 0.0));
    positionAlloc4->Add (Vector(294.3128539168792,176.09182320660494, 0.0));
    positionAlloc4->Add (Vector(249.83925224861176,260.49721340059375, 0.0));
    positionAlloc4->Add (Vector(76.9809011797194,254.02498489706844, 0.0));
    positionAlloc4->Add (Vector(160.45117006957267,195.19767591731508, 0.0));
    positionAlloc4->Add (Vector(424.7198900349536,144.8243065270885, 0.0));
    positionAlloc4->Add (Vector(402.2180754613541,231.00060296243552, 0.0));
    positionAlloc4->Add (Vector(431.0063784145566,70.05291552180604, 0.0));
    positionAlloc4->Add (Vector(227.98429826951687,228.18714661039024, 0.0));
    positionAlloc4->Add (Vector(381.77893649307913,493.10110795676434, 0.0));
    positionAlloc4->Add (Vector(383.0039669022005,212.7815731693025, 0.0));
    positionAlloc4->Add (Vector(217.08925267155453,129.542978419899, 0.0));
    positionAlloc4->Add (Vector(67.03475948928339,278.5387631612651, 0.0));
    positionAlloc4->Add (Vector(319.57146866903804,322.2577500515756, 0.0));
    positionAlloc4->Add (Vector(400.556197236206,428.7812946911998, 0.0));
    positionAlloc4->Add (Vector(444.38274962910697,446.3226478157592, 0.0));
    positionAlloc4->Add (Vector(59.90134599998703,390.28793297068916, 0.0));
    positionAlloc4->Add (Vector(476.22328099675195,358.7750234958198, 0.0));
    positionAlloc4->Add (Vector(42.98411370911287,478.23012433031226, 0.0));
    positionAlloc4->Add (Vector(235.52113534396113,457.0262631002913, 0.0));
    positionAlloc4->Add (Vector(39.99786237157782,79.72629397592141, 0.0));
    positionAlloc4->Add (Vector(484.52400929764315,365.3098458839388, 0.0));
    positionAlloc4->Add (Vector(276.4223720448263,489.99998488529764, 0.0));
    positionAlloc4->Add (Vector(417.0211401937534,461.2630600544315, 0.0));
    positionAlloc4->Add (Vector(262.0045054446531,70.26255692243683, 0.0));
    positionAlloc4->Add (Vector(241.94248988702117,148.57649136025282, 0.0));
    positionAlloc4->Add (Vector(366.6297310929218,69.0317190834871, 0.0));
    positionAlloc4->Add (Vector(148.96064251841906,123.43131072204343, 0.0));
    positionAlloc4->Add (Vector(468.9776549386352,108.81602574124788, 0.0));
    positionAlloc4->Add (Vector(342.6387188866231,181.96120566051, 0.0));
    positionAlloc4->Add (Vector(45.63142961629041,326.5082336126696, 0.0));
    positionAlloc4->Add (Vector(74.95923953402239,359.33463386170274, 0.0));
    positionAlloc4->Add (Vector(87.88820414462195,371.94437245460534, 0.0));
    positionAlloc4->Add (Vector(257.27575957276133,137.2047791531953, 0.0));
    positionAlloc4->Add (Vector(363.0765326046905,41.45647794092672, 0.0));
    positionAlloc4->Add (Vector(180.04977832971957,390.6715141364236, 0.0));
    positionAlloc4->Add (Vector(202.7250312200462,120.17575004052944, 0.0));
    positionAlloc4->Add (Vector(451.78579280599917,15.14921486819526, 0.0));
    positionAlloc4->Add (Vector(173.23966698296132,1.0654165414168415, 0.0));
    positionAlloc4->Add (Vector(151.4513015897894,101.70483594141044, 0.0));
    positionAlloc4->Add (Vector(163.79684000535306,110.41784167091163, 0.0));
    positionAlloc4->Add (Vector(27.79740346493853,147.28527174617446, 0.0));
    positionAlloc4->Add (Vector(44.52342574133083,339.7005808756066, 0.0));
    positionAlloc4->Add (Vector(275.44329990577535,373.0892344877371, 0.0));
    positionAlloc4->Add (Vector(1.3959681449057237,47.80310092687829, 0.0));
    positionAlloc4->Add (Vector(125.3306212995982,257.0739210085666, 0.0));
    positionAlloc4->Add (Vector(28.301434519447387,226.5602277194587, 0.0));
    positionAlloc4->Add (Vector(305.3435855649545,34.969077884507485, 0.0));
    positionAlloc4->Add (Vector(373.6155148980094,247.33975298337796, 0.0));
    positionAlloc4->Add (Vector(33.98683210139758,203.54895602640693, 0.0));
    positionAlloc4->Add (Vector(335.61609925447004,32.22646860216638, 0.0));
    positionAlloc4->Add (Vector(474.69820581990217,238.41696623227392, 0.0));
    positionAlloc4->Add (Vector(115.42755507074293,425.04101236686637, 0.0));
    positionAlloc4->Add (Vector(471.8441095248147,443.46234440089796, 0.0));
    positionAlloc4->Add (Vector(206.9487291926972,162.61979469278808, 0.0));
    positionAlloc4->Add (Vector(271.7033029683042,489.60598484753797, 0.0));
    positionAlloc4->Add (Vector(199.99383502672015,289.11135587584283, 0.0));
    positionAlloc4->Add (Vector(164.9291735820677,297.00736673885586, 0.0));
    positionAlloc4->Add (Vector(258.5407374905483,340.423383221014, 0.0));
    positionAlloc4->Add (Vector(440.7795393094149,109.8892975115982, 0.0));
    positionAlloc4->Add (Vector(136.2140534918328,89.79768327416626, 0.0));
    positionAlloc4->Add (Vector(98.24142196272018,214.2291940282919, 0.0));
    positionAlloc4->Add (Vector(419.63005001764674,268.36024487055016, 0.0));
    positionAlloc4->Add (Vector(140.25572805680846,250.0139912960211, 0.0));
    positionAlloc4->Add (Vector(435.58692358600183,186.22385808363757, 0.0));
    positionAlloc4->Add (Vector(196.68677306385612,130.75195472057277, 0.0));
    positionAlloc4->Add (Vector(192.09564484983682,439.55734487505276, 0.0));
    positionAlloc4->Add (Vector(445.1947273473011,15.60504607988994, 0.0));
    positionAlloc4->Add (Vector(461.89609550393203,46.397557398587196, 0.0));
    positionAlloc4->Add (Vector(338.41881236259684,170.42983058044976, 0.0));
    positionAlloc4->Add (Vector(312.1112320571444,206.31726287801433, 0.0));
    positionAlloc4->Add (Vector(94.18280904335796,3.5879453295379893, 0.0));
    positionAlloc4->Add (Vector(179.64129355543994,450.5023021840234, 0.0));
    positionAlloc4->Add (Vector(274.3214553589917,55.04020670263115, 0.0));
    positionAlloc4->Add (Vector(227.99323877130612,353.6109094104528, 0.0));
    positionAlloc4->Add (Vector(19.36956952722202,107.05282718381682, 0.0));
    positionAlloc4->Add (Vector(196.73479425425282,252.37447037380323, 0.0));
    positionAlloc4->Add (Vector(103.23069076385133,436.9152858932697, 0.0));
    positionAlloc4->Add (Vector(116.28587762075499,467.082420902448, 0.0));
    positionAlloc4->Add (Vector(325.7756746507931,81.31642412964263, 0.0));
    positionAlloc4->Add (Vector(134.40809694764854,443.3407874339583, 0.0));
    positionAlloc4->Add (Vector(335.14577489079005,331.2976565011295, 0.0));
    positionAlloc4->Add (Vector(163.86021496476388,328.4527407600925, 0.0));
    positionAlloc4->Add (Vector(112.23718488832878,356.5576685786766, 0.0));
    positionAlloc4->Add (Vector(308.5848587075621,441.14552322758044, 0.0));
    positionAlloc4->Add (Vector(402.0795894656038,111.59703326484022, 0.0));
    positionAlloc4->Add (Vector(183.61603560578388,137.56164737498426, 0.0));
    positionAlloc4->Add (Vector(412.87298196572226,61.3243269669535, 0.0));
    positionAlloc4->Add (Vector(274.65667227606843,144.61937975841366, 0.0));
    positionAlloc4->Add (Vector(345.453399578361,253.32946119507878, 0.0));
    positionAlloc4->Add (Vector(450.81714993772215,234.82098983431842, 0.0));
    positionAlloc4->Add (Vector(322.6197755672501,247.5315800239422, 0.0));
    positionAlloc4->Add (Vector(157.37131582652768,441.02207795305304, 0.0));
    positionAlloc4->Add (Vector(221.52886147806234,243.78486745306222, 0.0));
    positionAlloc4->Add (Vector(405.91158043323,281.9116638276248, 0.0));
    positionAlloc4->Add (Vector(346.36436317539307,183.3293263939341, 0.0));
    positionAlloc4->Add (Vector(332.23964699851734,223.18501954314996, 0.0));
    positionAlloc4->Add (Vector(459.54936292625575,188.84658854540103, 0.0));
    positionAlloc4->Add (Vector(351.7797518853066,136.81132456836164, 0.0));
    positionAlloc4->Add (Vector(424.7392970361517,241.495726777, 0.0));
    positionAlloc4->Add (Vector(481.12040768715,195.3395954218136, 0.0));
    positionAlloc4->Add (Vector(14.411048923780644,2.3290510758893768, 0.0));
    positionAlloc4->Add (Vector(429.77503960570414,8.997702235591209, 0.0));
    positionAlloc4->Add (Vector(81.19276125076341,199.65024125991187, 0.0));
    positionAlloc4->Add (Vector(201.31679029727306,474.30017396913416, 0.0));
    positionAlloc4->Add (Vector(301.0005616725357,340.2441334249308, 0.0));
    positionAlloc4->Add (Vector(191.86634625786886,403.02593033747615, 0.0));
    positionAlloc4->Add (Vector(9.352906842093944,292.91046783842694, 0.0));
    positionAlloc4->Add (Vector(298.62245192673055,258.52739712000044, 0.0));
    positionAlloc4->Add (Vector(406.4320438044125,305.66046221685184, 0.0));
    positionAlloc4->Add (Vector(264.7983135492217,233.77618750623546, 0.0));
    positionAlloc4->Add (Vector(274.5407750755285,184.88479484585906, 0.0));
    positionAlloc4->Add (Vector(6.7838499659144125,117.46439014828542, 0.0));
    positionAlloc4->Add (Vector(358.769613362895,427.00845967016164, 0.0));
    positionAlloc4->Add (Vector(342.9763099918921,64.25757717313374, 0.0));
    positionAlloc4->Add (Vector(247.6438630569355,275.56187997944824, 0.0));
    positionAlloc4->Add (Vector(101.27066195863831,266.5650126581075, 0.0));
    positionAlloc4->Add (Vector(294.13727392045485,144.07461307379504, 0.0));
    positionAlloc4->Add (Vector(38.86091398946012,74.19685550381504, 0.0));
    positionAlloc4->Add (Vector(179.26719147742804,59.42991150327526, 0.0));
    positionAlloc4->Add (Vector(158.06137266566407,17.580367143734733, 0.0));
    positionAlloc4->Add (Vector(360.46238953727624,352.95761008691665, 0.0));
    positionAlloc4->Add (Vector(40.15319334906181,492.50149957944365, 0.0));
    positionAlloc4->Add (Vector(210.86467297312163,400.6984472566912, 0.0));
    positionAlloc4->Add (Vector(377.50400594193957,430.1996039623089, 0.0));
    positionAlloc4->Add (Vector(118.17488074886728,34.02883227276621, 0.0));
    positionAlloc4->Add (Vector(437.8494788802586,51.5601343929068, 0.0));
    positionAlloc4->Add (Vector(210.01141296207587,72.22110047876984, 0.0));
    positionAlloc4->Add (Vector(28.069937787875975,139.52855427815214, 0.0));
    positionAlloc4->Add (Vector(12.532095398557486,128.77332696579614, 0.0));
    positionAlloc4->Add (Vector(422.9554070841039,398.3133544762302, 0.0));
    positionAlloc4->Add (Vector(304.3480052121424,89.93244655715338, 0.0));
    positionAlloc4->Add (Vector(427.28640596760647,57.25698736046092, 0.0));
    positionAlloc4->Add (Vector(364.9978795420551,308.2250741632041, 0.0));
    positionAlloc4->Add (Vector(288.9744456480077,467.02655406278217, 0.0));
    positionAlloc4->Add (Vector(151.05270834360996,261.11008398130804, 0.0));
    positionAlloc4->Add (Vector(176.7205216292526,18.838022664403653, 0.0));
    positionAlloc4->Add (Vector(329.8498550177252,201.03158737027726, 0.0));
    positionAlloc4->Add (Vector(329.4441699414499,58.37273877199311, 0.0));
    positionAlloc4->Add (Vector(213.75769560259715,172.11174755470117, 0.0));
    positionAlloc4->Add (Vector(135.57693165335672,45.55523027263997, 0.0));
    positionAlloc4->Add (Vector(137.9426589256882,348.7857428371264, 0.0));
    positionAlloc4->Add (Vector(101.95892426395886,452.20971635391595, 0.0));
    positionAlloc4->Add (Vector(205.91119948970072,240.72661995560063, 0.0));
    positionAlloc4->Add (Vector(448.705907425002,180.34884056771543, 0.0));
    positionAlloc4->Add (Vector(286.67912588566355,111.86385146521044, 0.0));
    positionAlloc4->Add (Vector(343.59478543904845,373.50864042729063, 0.0));
    positionAlloc4->Add (Vector(141.6261263841729,220.6626129182781, 0.0));
    positionAlloc4->Add (Vector(454.30515962023355,478.511567955774, 0.0));
    positionAlloc4->Add (Vector(269.0821631801562,354.354200332195, 0.0));
    positionAlloc4->Add (Vector(80.9136786981886,373.48188883265925, 0.0));
    positionAlloc4->Add (Vector(207.7305442970799,411.69507576608186, 0.0));
    positionAlloc4->Add (Vector(413.4761883155359,317.8605109818339, 0.0));
    positionAlloc4->Add (Vector(183.51884891275512,64.04206933337753, 0.0));
    positionAlloc4->Add (Vector(80.15561771101015,437.24716139433986, 0.0));
    positionAlloc4->Add (Vector(321.3341858851671,149.05064312601874, 0.0));
    positionAlloc4->Add (Vector(455.1159438695235,15.064491169412719, 0.0));
    positionAlloc4->Add (Vector(444.4979007098,394.91979207865535, 0.0));
    positionAlloc4->Add (Vector(412.96583737132255,283.3300569106611, 0.0));
    positionAlloc4->Add (Vector(68.01724153004119,392.07712489216254, 0.0));
    positionAlloc4->Add (Vector(429.5286734104099,104.68661774735477, 0.0));
    positionAlloc4->Add (Vector(113.14395054144467,358.4329413577596, 0.0));
    positionAlloc4->Add (Vector(68.96040061693503,267.31131010691377, 0.0));
    positionAlloc4->Add (Vector(2.52931755384167,469.50251935687015, 0.0));
    positionAlloc4->Add (Vector(29.644881181825568,275.7221678246181, 0.0));
    positionAlloc4->Add (Vector(344.9604178955946,172.35318636569818, 0.0));
    positionAlloc4->Add (Vector(336.5811455948696,239.50584782061213, 0.0));
    positionAlloc4->Add (Vector(343.9056881915911,29.866272278318228, 0.0));
    positionAlloc4->Add (Vector(351.10313472112847,415.84963553232143, 0.0));
    positionAlloc4->Add (Vector(99.04684918523692,431.6304626713807, 0.0));
    positionAlloc4->Add (Vector(383.8321116927703,120.24190188293288, 0.0));
    positionAlloc4->Add (Vector(268.3676881153749,37.35563994001445, 0.0));
    positionAlloc4->Add (Vector(246.98167751671164,235.17101361996652, 0.0));
    positionAlloc4->Add (Vector(370.01294824709424,119.32635639833977, 0.0));
    positionAlloc4->Add (Vector(269.7819660858629,479.2629616385832, 0.0));
    positionAlloc4->Add (Vector(379.383279571726,477.84362180236747, 0.0));
    positionAlloc4->Add (Vector(153.9480907536387,189.82427711800227, 0.0));
    positionAlloc4->Add (Vector(294.1724017418404,154.33420203732777, 0.0));
    positionAlloc4->Add (Vector(319.4498158793097,142.73784054971273, 0.0));
    positionAlloc4->Add (Vector(165.21058914019116,426.60681120706954, 0.0));
    positionAlloc4->Add (Vector(337.1283175383922,495.9871194664171, 0.0));
    positionAlloc4->Add (Vector(176.77938659975683,438.78855393207823, 0.0));
    positionAlloc4->Add (Vector(66.66211716865183,312.8615628931434, 0.0));
    positionAlloc4->Add (Vector(36.756061161936195,73.61378310855294, 0.0));
    positionAlloc4->Add (Vector(467.81951354112766,496.0199687688157, 0.0));
    positionAlloc4->Add (Vector(472.4971831158374,261.05415465986476, 0.0));
    positionAlloc4->Add (Vector(265.2086278380165,476.00509163116425, 0.0));
    positionAlloc4->Add (Vector(283.7740709990179,360.3923130397521, 0.0));
    positionAlloc4->Add (Vector(425.0139670015527,422.6395878444959, 0.0));
    positionAlloc4->Add (Vector(134.87412508695894,283.26137901990813, 0.0));
    positionAlloc4->Add (Vector(470.5707410315402,424.76601808101185, 0.0));
    positionAlloc4->Add (Vector(332.73902546071565,293.02432542089923, 0.0));
    positionAlloc4->Add (Vector(275.6887214276916,259.2824681972788, 0.0));
    positionAlloc4->Add (Vector(205.33802661605327,331.1777681834077, 0.0));
    positionAlloc4->Add (Vector(42.479342021966346,121.59885770015649, 0.0));
    positionAlloc4->Add (Vector(423.25009463511503,414.3483866256572, 0.0));
    positionAlloc4->Add (Vector(310.454900184712,52.88028137235729, 0.0));
    positionAlloc4->Add (Vector(371.8860754768684,136.9298567125009, 0.0));
    positionAlloc4->Add (Vector(213.74846159438204,274.6681987001785, 0.0));
    positionAlloc4->Add (Vector(420.1602541701448,35.82267890080831, 0.0));
    positionAlloc4->Add (Vector(347.75668207854625,135.30421569843466, 0.0));
    positionAlloc4->Add (Vector(168.1026964222567,410.7756209160484, 0.0));
    positionAlloc4->Add (Vector(208.09659089842592,77.1320604380707, 0.0));
    positionAlloc4->Add (Vector(21.517808818910577,416.04013926390485, 0.0));
    positionAlloc4->Add (Vector(462.84115873391755,350.56561553434534, 0.0));
    positionAlloc4->Add (Vector(450.63873924607344,132.4184438061469, 0.0));
    positionAlloc4->Add (Vector(235.80853337865793,315.09295850430505, 0.0));
    positionAlloc4->Add (Vector(385.3041626286992,181.3253699271748, 0.0));
    positionAlloc4->Add (Vector(122.73078407328502,345.40430939729913, 0.0));
    positionAlloc4->Add (Vector(223.6697600187373,314.69661962110547, 0.0));
    positionAlloc4->Add (Vector(392.8861124283564,281.75661844112955, 0.0));
    positionAlloc4->Add (Vector(474.2671563927286,148.04761724692816, 0.0));
    positionAlloc4->Add (Vector(129.05853030595165,174.12403222998364, 0.0));
    positionAlloc4->Add (Vector(88.69772015233585,234.73524989333367, 0.0));
    positionAlloc4->Add (Vector(390.43272012478025,263.4351223918807, 0.0));
    positionAlloc4->Add (Vector(194.7708562149329,236.04932636145188, 0.0));
    positionAlloc4->Add (Vector(38.21515728351665,482.042016209156, 0.0));
    positionAlloc4->Add (Vector(469.4116829324459,296.13501534439484, 0.0));
    positionAlloc4->Add (Vector(458.1006116041304,436.20853287847086, 0.0));
    positionAlloc4->Add (Vector(69.30782693721655,73.56409557262378, 0.0));
    positionAlloc4->Add (Vector(253.4212767296375,388.8077690678013, 0.0));
    positionAlloc4->Add (Vector(86.67298541435142,38.88354030390629, 0.0));
    positionAlloc4->Add (Vector(37.65807240382296,182.26563431164615, 0.0));
    positionAlloc4->Add (Vector(450.8019343443333,354.9917444404403, 0.0));
    positionAlloc4->Add (Vector(65.63794452800221,167.9665534857827, 0.0));
    positionAlloc4->Add (Vector(492.3313640851252,301.3060038510872, 0.0));
    positionAlloc4->Add (Vector(255.5764294671127,490.72075462427784, 0.0));
    positionAlloc4->Add (Vector(475.3515607427997,318.34662429346315, 0.0));
    positionAlloc4->Add (Vector(175.193430984274,403.481518976266, 0.0));
    positionAlloc4->Add (Vector(69.95428976467927,470.4795315583664, 0.0));
    positionAlloc4->Add (Vector(292.9699743765733,48.86214540126937, 0.0));
    positionAlloc4->Add (Vector(38.269395820678284,466.54975199510545, 0.0));
    positionAlloc4->Add (Vector(75.3694062176562,269.62127969329794, 0.0));
    positionAlloc4->Add (Vector(200.63700246024897,312.0323462554455, 0.0));
    positionAlloc4->Add (Vector(166.84013816448729,418.3876843849537, 0.0));
    positionAlloc4->Add (Vector(416.39185833790185,440.01147813577313, 0.0));
    positionAlloc4->Add (Vector(192.27200088171702,17.565819071626187, 0.0));
    positionAlloc4->Add (Vector(210.96763229020115,333.66128917669687, 0.0));
    positionAlloc4->Add (Vector(271.7444562701694,82.01006217373997, 0.0));
    positionAlloc4->Add (Vector(228.38769937252584,205.82995295265562, 0.0));
    positionAlloc4->Add (Vector(63.71672401629658,122.96656650530247, 0.0));
    positionAlloc4->Add (Vector(61.41038633738716,197.15474967400726, 0.0));
    positionAlloc4->Add (Vector(361.984976438775,208.143820368154, 0.0));
    positionAlloc4->Add (Vector(455.06450273550587,158.24877313718972, 0.0));
    positionAlloc4->Add (Vector(209.34188088772987,272.0729444360453, 0.0));
    positionAlloc4->Add (Vector(84.87995472022092,454.1803030964685, 0.0));
    positionAlloc4->Add (Vector(51.214115638217486,448.0904412734103, 0.0));
    positionAlloc4->Add (Vector(350.5839438812861,446.8185777124562, 0.0));
    positionAlloc4->Add (Vector(167.7294091134366,285.2343927510989, 0.0));
    positionAlloc4->Add (Vector(336.9633502983517,415.4951245691335, 0.0));
    positionAlloc4->Add (Vector(295.005405207753,3.672163423794339, 0.0));
    positionAlloc4->Add (Vector(9.7014582721196,86.66796463104198, 0.0));
    positionAlloc4->Add (Vector(18.721070466609312,105.80531639935997, 0.0));
    positionAlloc4->Add (Vector(342.6657441680966,422.69978284468255, 0.0));
    positionAlloc4->Add (Vector(6.2702062492925865,408.55256793619094, 0.0));
    positionAlloc4->Add (Vector(13.351388320542668,140.8597655727733, 0.0));
    positionAlloc4->Add (Vector(136.52143683086814,242.6456422207698, 0.0));
    positionAlloc4->Add (Vector(422.803021372644,480.81771188733916, 0.0));
    positionAlloc4->Add (Vector(42.38213212337716,467.2788348618562, 0.0));
    positionAlloc4->Add (Vector(5.914078251372912,465.51476371966055, 0.0));
    positionAlloc4->Add (Vector(358.09571799488583,229.96070902874126, 0.0));
    positionAlloc4->Add (Vector(467.9894069055732,437.7172148564497, 0.0));
    positionAlloc4->Add (Vector(389.7827665018916,422.76124446389576, 0.0));
    positionAlloc4->Add (Vector(180.72515696577418,447.9838806084641, 0.0));
    positionAlloc4->Add (Vector(416.2067071218715,82.43048432674289, 0.0));
    positionAlloc4->Add (Vector(237.28887640515828,107.76011876529162, 0.0));
    positionAlloc4->Add (Vector(130.0001906120495,133.07751115406248, 0.0));
    positionAlloc4->Add (Vector(452.30218434802276,229.86324692471993, 0.0));
    positionAlloc4->Add (Vector(481.71543397179244,401.44480516337205, 0.0));
    positionAlloc4->Add (Vector(20.097064472621938,267.3421789734982, 0.0));
    positionAlloc4->Add (Vector(484.81841772205644,165.20397950086445, 0.0));
    positionAlloc4->Add (Vector(96.69252774257875,333.9295908822596, 0.0));
    positionAlloc4->Add (Vector(93.49124258502955,171.82754989007327, 0.0));
    positionAlloc4->Add (Vector(177.65210564192645,446.01747860058504, 0.0));
    positionAlloc4->Add (Vector(453.9684765116591,79.25946099891695, 0.0));
    positionAlloc4->Add (Vector(220.53707994622317,404.64629742117404, 0.0));
    positionAlloc4->Add (Vector(179.57946841991097,86.87850195348406, 0.0));
    positionAlloc4->Add (Vector(339.11537124455555,264.0853787364826, 0.0));
    positionAlloc4->Add (Vector(183.93970399748238,369.1560388589615, 0.0));
    positionAlloc4->Add (Vector(209.6562502080359,180.60033165831624, 0.0));
    positionAlloc4->Add (Vector(370.68765841622826,474.28703920119045, 0.0));
    positionAlloc4->Add (Vector(125.7363026126238,114.63486251352111, 0.0));
    positionAlloc4->Add (Vector(308.7386347236301,375.9251966524969, 0.0));
    positionAlloc4->Add (Vector(328.03070955516137,468.2772035487417, 0.0));
    positionAlloc4->Add (Vector(172.23050482542578,448.18591257070386, 0.0));
    positionAlloc4->Add (Vector(419.7623843208357,162.77780014888964, 0.0));
    positionAlloc4->Add (Vector(275.63019964131155,145.3129843237988, 0.0));
    positionAlloc4->Add (Vector(261.81703569943136,392.4181837332413, 0.0));
    positionAlloc4->Add (Vector(115.17593130415321,338.57912198341324, 0.0));
    positionAlloc4->Add (Vector(404.41616302963087,288.6134108990622, 0.0));
    positionAlloc4->Add (Vector(393.78804485939935,245.91141616608215, 0.0));
    positionAlloc4->Add (Vector(286.3211376950533,169.10114922112774, 0.0));
    positionAlloc4->Add (Vector(433.092392141199,58.18577669003605, 0.0));
    positionAlloc4->Add (Vector(343.8198420221481,437.992024433587, 0.0));
    positionAlloc4->Add (Vector(427.2730487658033,290.7348085151954, 0.0));
    positionAlloc4->Add (Vector(157.0650475425811,320.7916540411226, 0.0));
    positionAlloc4->Add (Vector(408.9293749393286,241.3691446265533, 0.0));
    positionAlloc4->Add (Vector(414.2779623846641,462.52700173284774, 0.0));
    positionAlloc4->Add (Vector(167.35603923769088,233.49137456414732, 0.0));
    positionAlloc4->Add (Vector(92.97429401020624,480.65309446239326, 0.0));
    positionAlloc4->Add (Vector(311.9908535594733,168.99711416260416, 0.0));
    positionAlloc4->Add (Vector(333.6160037555663,443.9505164093066, 0.0));
    positionAlloc4->Add (Vector(255.83892020791615,411.5878977088754, 0.0));
    positionAlloc4->Add (Vector(423.9342025254421,363.8275966117088, 0.0));
    positionAlloc4->Add (Vector(41.65422239918215,61.20898591595425, 0.0));
    positionAlloc4->Add (Vector(336.4099849908101,185.20703360348685, 0.0));
    positionAlloc4->Add (Vector(7.159327935245042,472.74236100868933, 0.0));
    positionAlloc4->Add (Vector(457.85213732058037,250.5788478195573, 0.0));
    positionAlloc4->Add (Vector(446.6383865219935,210.112702962099, 0.0));
    positionAlloc4->Add (Vector(356.9655014047953,290.0052035890011, 0.0));
    positionAlloc4->Add (Vector(104.70596289353922,449.21448082390333, 0.0));
    positionAlloc4->Add (Vector(292.9709333183618,220.15601487093255, 0.0));
    positionAlloc4->Add (Vector(356.7834201512213,473.5408953370288, 0.0));
    positionAlloc4->Add (Vector(396.4988397098732,315.92623878466947, 0.0));
    positionAlloc4->Add (Vector(235.8802767540741,338.4385542062942, 0.0));
    positionAlloc4->Add (Vector(131.25467597386847,337.4097306959515, 0.0));
    positionAlloc4->Add (Vector(466.8491295087794,85.4290085110868, 0.0));
    positionAlloc4->Add (Vector(190.82701076051904,314.57040811291995, 0.0));
    positionAlloc4->Add (Vector(223.68707308852854,6.32292686220326, 0.0));
    positionAlloc4->Add (Vector(312.1896631086322,131.09787142110812, 0.0));
    positionAlloc4->Add (Vector(398.48846796378115,253.11531061996834, 0.0));
    positionAlloc4->Add (Vector(388.0235686472568,251.3673184517702, 0.0));
    positionAlloc4->Add (Vector(322.2021846900493,14.473279473697643, 0.0));
    positionAlloc4->Add (Vector(235.50179808353477,474.7506309923667, 0.0));
    positionAlloc4->Add (Vector(100.40038026662013,15.340729300815648, 0.0));
    positionAlloc4->Add (Vector(1.6256373760336618,22.349701556639843, 0.0));
    positionAlloc4->Add (Vector(184.7907173727149,105.22619075819372, 0.0));
    positionAlloc4->Add (Vector(227.08574766229268,268.76409378279516, 0.0));
    positionAlloc4->Add (Vector(407.5394641329387,274.06745376805753, 0.0));
    positionAlloc4->Add (Vector(88.57262135628685,153.3601820086915, 0.0));
    positionAlloc4->Add (Vector(465.19599538657263,259.7199144756003, 0.0));
    positionAlloc4->Add (Vector(195.63562419372698,316.7081801345407, 0.0));
    positionAlloc4->Add (Vector(389.3919893690405,347.2892058103048, 0.0));
    positionAlloc4->Add (Vector(455.0005150401153,346.295385036663, 0.0));
    positionAlloc4->Add (Vector(460.86896105917816,281.2625591975819, 0.0));
    positionAlloc4->Add (Vector(36.02143423148041,422.32432737763156, 0.0));
    positionAlloc4->Add (Vector(90.61102030164881,441.91776417521993, 0.0));
    positionAlloc4->Add (Vector(192.65064727510344,48.60576966941987, 0.0));
    positionAlloc4->Add (Vector(298.01823179138023,438.24320145036575, 0.0));
    positionAlloc4->Add (Vector(229.0052575827961,383.5052901261409, 0.0));
    positionAlloc4->Add (Vector(163.82377919254188,200.03618232401953, 0.0));
    positionAlloc4->Add (Vector(190.89729249697734,308.50000960101136, 0.0));
    positionAlloc4->Add (Vector(428.4947292704802,471.62457841960065, 0.0));
    positionAlloc4->Add (Vector(374.69957368073034,365.436822195105, 0.0));
    positionAlloc4->Add (Vector(17.40740784208722,57.21434709819623, 0.0));
    positionAlloc4->Add (Vector(309.368551261298,5.665686516074575, 0.0));
    positionAlloc4->Add (Vector(329.1160175992419,379.9925593564417, 0.0));
    positionAlloc4->Add (Vector(310.46868808085713,190.09396933618856, 0.0));
    positionAlloc4->Add (Vector(439.4497259669667,420.5926378230805, 0.0));
    positionAlloc4->Add (Vector(410.4413078997271,338.5804434959283, 0.0));
    positionAlloc4->Add (Vector(154.86648734559495,470.7078619891241, 0.0));
    positionAlloc4->Add (Vector(116.04461495784774,70.74137774451222, 0.0));
    positionAlloc4->Add (Vector(38.13987975173993,492.478775644786, 0.0));
    positionAlloc4->Add (Vector(78.76744253683803,391.75346120721866, 0.0));
    positionAlloc4->Add (Vector(300.77077216360993,209.9178085651059, 0.0));
    positionAlloc4->Add (Vector(209.8742396741919,462.1978983622486, 0.0));
    positionAlloc4->Add (Vector(155.50142493607405,54.22803262269965, 0.0));
    positionAlloc4->Add (Vector(263.6571698501165,303.2454255799352, 0.0));
    positionAlloc4->Add (Vector(489.34900250152367,100.90626546951643, 0.0));
    positionAlloc4->Add (Vector(156.74049067668332,36.1374561070858, 0.0));
    positionAlloc4->Add (Vector(145.24358909202962,83.0020361339489, 0.0));
    positionAlloc4->Add (Vector(332.6622431172312,402.14470677415756, 0.0));
    positionAlloc4->Add (Vector(216.83050928224063,482.42899235118796, 0.0));
    positionAlloc4->Add (Vector(232.88903418624335,22.100103664715544, 0.0));
    positionAlloc4->Add (Vector(260.4820185084916,115.00799963098396, 0.0));
    positionAlloc4->Add (Vector(240.46027248444645,13.240403135511336, 0.0));
    positionAlloc4->Add (Vector(315.92400811432935,286.46634182396787, 0.0));
    positionAlloc4->Add (Vector(364.29123438469844,160.5003286592167, 0.0));
    positionAlloc4->Add (Vector(411.2915313916308,428.4167075959037, 0.0));
    positionAlloc4->Add (Vector(147.97444377395868,92.98711042140806, 0.0));
    positionAlloc4->Add (Vector(437.90619594043744,289.79707595069107, 0.0));
    positionAlloc4->Add (Vector(157.91482790865257,153.39359100637802, 0.0));
    positionAlloc4->Add (Vector(175.62308542478183,494.5321647448047, 0.0));
    positionAlloc4->Add (Vector(176.5093940988805,436.5508831879482, 0.0));
    positionAlloc4->Add (Vector(417.4943442456489,35.44536417884486, 0.0));
    positionAlloc4->Add (Vector(451.42413716735365,124.43529211007987, 0.0));
    positionAlloc4->Add (Vector(215.37644458698958,66.38232427433738, 0.0));
    positionAlloc4->Add (Vector(122.62550183592397,272.74165139485825, 0.0));
    positionAlloc4->Add (Vector(491.7317756220006,475.8878690620901, 0.0));
    positionAlloc4->Add (Vector(371.75656788948135,199.57717527743966, 0.0));
    positionAlloc4->Add (Vector(430.0262517137371,62.04573642531069, 0.0));
    positionAlloc4->Add (Vector(170.00063195829978,145.0167931206411, 0.0));
    positionAlloc4->Add (Vector(165.79890067559683,100.15205296640373, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_17.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_ECC_flowmon/switch_ECC_17.flowmon", false, false);
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