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
	  uint16_t numberOfRrhs = 16;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 231;
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
    positionAlloc->Add (Vector (356.6666666666667,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (270.0,400.0, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,140.0, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,10.0, 0.0));
    positionAlloc->Add (Vector (140.0,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (140.0,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (140.0,10.0, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,53.333333333333336, 0.0));
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
    positionAlloc4->Add (Vector(255.3873470098208,273.83685407531044, 0.0));
    positionAlloc4->Add (Vector(41.58624863910991,53.12405535383622, 0.0));
    positionAlloc4->Add (Vector(328.1827331573457,46.65699502320564, 0.0));
    positionAlloc4->Add (Vector(166.99213904996247,373.4620945237336, 0.0));
    positionAlloc4->Add (Vector(143.81586468098243,358.39373909931965, 0.0));
    positionAlloc4->Add (Vector(164.5944943422375,250.10998354407383, 0.0));
    positionAlloc4->Add (Vector(153.62194962655616,58.862558176442946, 0.0));
    positionAlloc4->Add (Vector(233.38502965507658,34.03512118955607, 0.0));
    positionAlloc4->Add (Vector(71.86559541554018,46.374183740834106, 0.0));
    positionAlloc4->Add (Vector(196.21949387267375,29.157460914897193, 0.0));
    positionAlloc4->Add (Vector(230.22914899163868,279.4141125504634, 0.0));
    positionAlloc4->Add (Vector(182.01781418420953,29.61078819710883, 0.0));
    positionAlloc4->Add (Vector(61.12673106646174,204.00632258220872, 0.0));
    positionAlloc4->Add (Vector(374.8979787547819,93.81420547074653, 0.0));
    positionAlloc4->Add (Vector(158.87125769309483,45.59553895724644, 0.0));
    positionAlloc4->Add (Vector(216.33995312615468,162.01174415514723, 0.0));
    positionAlloc4->Add (Vector(360.2368697474943,324.1100100169793, 0.0));
    positionAlloc4->Add (Vector(250.08591250679012,165.0376341516087, 0.0));
    positionAlloc4->Add (Vector(100.40271090110839,239.01257369820087, 0.0));
    positionAlloc4->Add (Vector(337.2720365763061,52.696165973280884, 0.0));
    positionAlloc4->Add (Vector(260.2342535823779,206.04160275230464, 0.0));
    positionAlloc4->Add (Vector(225.76704236653185,123.49335271096899, 0.0));
    positionAlloc4->Add (Vector(186.16145914149635,82.59085329124618, 0.0));
    positionAlloc4->Add (Vector(373.9342026204855,360.3304243341599, 0.0));
    positionAlloc4->Add (Vector(224.59988965857875,204.04357697046552, 0.0));
    positionAlloc4->Add (Vector(33.57060574308335,44.80627329782436, 0.0));
    positionAlloc4->Add (Vector(399.34328612202637,197.4801747081767, 0.0));
    positionAlloc4->Add (Vector(313.31794447948846,105.32579511953446, 0.0));
    positionAlloc4->Add (Vector(287.50161622056686,293.38134262364355, 0.0));
    positionAlloc4->Add (Vector(265.09520701751313,164.7785740812877, 0.0));
    positionAlloc4->Add (Vector(94.0323551928909,90.78150684855592, 0.0));
    positionAlloc4->Add (Vector(396.8114017301786,384.8919181786746, 0.0));
    positionAlloc4->Add (Vector(185.8050456024663,185.9210073661099, 0.0));
    positionAlloc4->Add (Vector(158.99659134878635,315.74968852356267, 0.0));
    positionAlloc4->Add (Vector(244.2835288202627,176.81920478225695, 0.0));
    positionAlloc4->Add (Vector(336.58388067033076,115.31158938354235, 0.0));
    positionAlloc4->Add (Vector(85.24314502061902,189.48880470428514, 0.0));
    positionAlloc4->Add (Vector(341.3151826915809,366.5316865167903, 0.0));
    positionAlloc4->Add (Vector(173.60922752774943,32.97034358577973, 0.0));
    positionAlloc4->Add (Vector(82.8572959314628,101.70411343428944, 0.0));
    positionAlloc4->Add (Vector(121.06200605623667,210.91833097286883, 0.0));
    positionAlloc4->Add (Vector(161.478645534239,294.65088348290067, 0.0));
    positionAlloc4->Add (Vector(388.9210350688506,306.9992196397419, 0.0));
    positionAlloc4->Add (Vector(120.78733284214329,98.5114232494146, 0.0));
    positionAlloc4->Add (Vector(159.40607044123266,15.567216010196727, 0.0));
    positionAlloc4->Add (Vector(165.57007768326676,246.2249895956363, 0.0));
    positionAlloc4->Add (Vector(317.5400233806514,199.43345718863816, 0.0));
    positionAlloc4->Add (Vector(25.43939955068111,365.1185661983481, 0.0));
    positionAlloc4->Add (Vector(287.6023849713989,121.05222536584863, 0.0));
    positionAlloc4->Add (Vector(82.90534462533587,354.2111271178798, 0.0));
    positionAlloc4->Add (Vector(287.451154288522,258.93325019096187, 0.0));
    positionAlloc4->Add (Vector(156.03615941351055,302.2362939283518, 0.0));
    positionAlloc4->Add (Vector(345.72886160936866,244.13848736665068, 0.0));
    positionAlloc4->Add (Vector(124.00799168413359,63.7883162337646, 0.0));
    positionAlloc4->Add (Vector(313.7437192163358,290.0185421741477, 0.0));
    positionAlloc4->Add (Vector(274.053311298425,399.06534450903416, 0.0));
    positionAlloc4->Add (Vector(328.40708541994206,68.33853604019878, 0.0));
    positionAlloc4->Add (Vector(166.35544268368446,345.00411875270845, 0.0));
    positionAlloc4->Add (Vector(198.01226407195122,118.60480035948875, 0.0));
    positionAlloc4->Add (Vector(18.00390632549562,273.0074530837945, 0.0));
    positionAlloc4->Add (Vector(379.58272952560554,154.16027981994205, 0.0));
    positionAlloc4->Add (Vector(115.44223847959812,215.5215860859657, 0.0));
    positionAlloc4->Add (Vector(99.73247371098246,151.59619041847688, 0.0));
    positionAlloc4->Add (Vector(234.4469792743455,308.7767380227517, 0.0));
    positionAlloc4->Add (Vector(378.1026315034421,200.36253795581328, 0.0));
    positionAlloc4->Add (Vector(17.5093070271239,369.27118360701434, 0.0));
    positionAlloc4->Add (Vector(96.62179605887448,224.53588311320738, 0.0));
    positionAlloc4->Add (Vector(283.58481328460465,380.51874152769966, 0.0));
    positionAlloc4->Add (Vector(198.59445925266363,305.04409278447395, 0.0));
    positionAlloc4->Add (Vector(358.55561393611396,364.0768651735635, 0.0));
    positionAlloc4->Add (Vector(151.1080812562951,305.2485642741514, 0.0));
    positionAlloc4->Add (Vector(223.54443559921918,274.5845503952141, 0.0));
    positionAlloc4->Add (Vector(47.78549832233643,376.0250055268671, 0.0));
    positionAlloc4->Add (Vector(163.56244428600868,8.046319233227006, 0.0));
    positionAlloc4->Add (Vector(201.06506812481152,37.61244384816109, 0.0));
    positionAlloc4->Add (Vector(21.444650751518026,85.4301247809091, 0.0));
    positionAlloc4->Add (Vector(22.381340353984047,244.5649201229445, 0.0));
    positionAlloc4->Add (Vector(214.014950925495,76.48577710359893, 0.0));
    positionAlloc4->Add (Vector(59.432734574788086,274.5360976784979, 0.0));
    positionAlloc4->Add (Vector(284.0751402340922,31.61267666675096, 0.0));
    positionAlloc4->Add (Vector(13.211607224615118,315.27055144548626, 0.0));
    positionAlloc4->Add (Vector(255.62128683479622,130.54855478048069, 0.0));
    positionAlloc4->Add (Vector(236.11057987746463,247.70363627461148, 0.0));
    positionAlloc4->Add (Vector(307.53534843309063,391.1690364575416, 0.0));
    positionAlloc4->Add (Vector(280.00245477911915,384.7569659767415, 0.0));
    positionAlloc4->Add (Vector(313.0542593091002,302.7461004830096, 0.0));
    positionAlloc4->Add (Vector(223.029840165007,145.5577930621774, 0.0));
    positionAlloc4->Add (Vector(292.1074886827467,201.7446818152013, 0.0));
    positionAlloc4->Add (Vector(359.85403448854595,256.4600892981999, 0.0));
    positionAlloc4->Add (Vector(6.098763676341079,25.95640185377195, 0.0));
    positionAlloc4->Add (Vector(356.3600826984804,110.47947591767655, 0.0));
    positionAlloc4->Add (Vector(314.75371348909505,198.10338654752366, 0.0));
    positionAlloc4->Add (Vector(375.8727993388429,100.7388174004483, 0.0));
    positionAlloc4->Add (Vector(278.4534651256758,225.62866568355463, 0.0));
    positionAlloc4->Add (Vector(356.01465538119646,263.2851559258129, 0.0));
    positionAlloc4->Add (Vector(340.6518457546405,60.60995239233735, 0.0));
    positionAlloc4->Add (Vector(309.55585459246265,328.13971920853635, 0.0));
    positionAlloc4->Add (Vector(0.24345692367964133,277.1541276917441, 0.0));
    positionAlloc4->Add (Vector(112.26302135418385,100.15811454323674, 0.0));
    positionAlloc4->Add (Vector(173.3873239329082,31.990564442045955, 0.0));
    positionAlloc4->Add (Vector(6.839289660690717,73.95795621893093, 0.0));
    positionAlloc4->Add (Vector(339.96181685971385,212.06180538628718, 0.0));
    positionAlloc4->Add (Vector(222.29650985502394,127.18922920962656, 0.0));
    positionAlloc4->Add (Vector(388.3622666568351,233.2403746898743, 0.0));
    positionAlloc4->Add (Vector(214.24282047273272,36.58990224328025, 0.0));
    positionAlloc4->Add (Vector(338.52017828031677,14.24010361397059, 0.0));
    positionAlloc4->Add (Vector(322.6660269845558,97.85611546684096, 0.0));
    positionAlloc4->Add (Vector(64.70736572760889,395.33251665119786, 0.0));
    positionAlloc4->Add (Vector(269.0214728897649,37.97819011988794, 0.0));
    positionAlloc4->Add (Vector(398.9846935033966,53.7482945705066, 0.0));
    positionAlloc4->Add (Vector(234.2492589252928,198.20651081024576, 0.0));
    positionAlloc4->Add (Vector(319.5004677241928,6.104981050132796, 0.0));
    positionAlloc4->Add (Vector(275.5956759962908,62.31555364358825, 0.0));
    positionAlloc4->Add (Vector(143.9235437600482,14.835253180273877, 0.0));
    positionAlloc4->Add (Vector(84.65427928719191,334.7838920549142, 0.0));
    positionAlloc4->Add (Vector(138.1555347576076,184.23594613009166, 0.0));
    positionAlloc4->Add (Vector(211.63963471283668,296.857929282243, 0.0));
    positionAlloc4->Add (Vector(147.60346365127157,388.87226873756987, 0.0));
    positionAlloc4->Add (Vector(133.43014400049213,8.643183659169384, 0.0));
    positionAlloc4->Add (Vector(353.51420759361474,262.1525584962633, 0.0));
    positionAlloc4->Add (Vector(152.99902851172317,129.31570056135325, 0.0));
    positionAlloc4->Add (Vector(298.99891515753853,56.20249799270085, 0.0));
    positionAlloc4->Add (Vector(386.6101116515747,79.31192798579207, 0.0));
    positionAlloc4->Add (Vector(103.6779821352471,280.3162426690694, 0.0));
    positionAlloc4->Add (Vector(329.353989903865,91.73507259173994, 0.0));
    positionAlloc4->Add (Vector(6.489744994586744,62.50039124116071, 0.0));
    positionAlloc4->Add (Vector(100.95170119719911,340.14173030172026, 0.0));
    positionAlloc4->Add (Vector(241.42617852329346,49.44515455953646, 0.0));
    positionAlloc4->Add (Vector(337.312512085301,178.95470388155266, 0.0));
    positionAlloc4->Add (Vector(232.47008111067876,218.2385303296536, 0.0));
    positionAlloc4->Add (Vector(22.028923594651452,248.03959597455062, 0.0));
    positionAlloc4->Add (Vector(269.1823163007435,213.6672010867063, 0.0));
    positionAlloc4->Add (Vector(188.60819555950292,215.87060048343557, 0.0));
    positionAlloc4->Add (Vector(215.30402834377833,17.53501921544394, 0.0));
    positionAlloc4->Add (Vector(226.58425432892523,240.73696451471687, 0.0));
    positionAlloc4->Add (Vector(3.5778419748548895,394.87047282717, 0.0));
    positionAlloc4->Add (Vector(103.14549439145048,118.82119820405003, 0.0));
    positionAlloc4->Add (Vector(112.49408382634533,102.20744701454541, 0.0));
    positionAlloc4->Add (Vector(101.84007557223893,300.4035303452914, 0.0));
    positionAlloc4->Add (Vector(72.38826296436707,388.8122683928293, 0.0));
    positionAlloc4->Add (Vector(5.817327830225194,16.80968821785647, 0.0));
    positionAlloc4->Add (Vector(309.17240691382017,239.2595059716372, 0.0));
    positionAlloc4->Add (Vector(37.338037440123,312.4801759959025, 0.0));
    positionAlloc4->Add (Vector(59.740324330260194,208.31859868203287, 0.0));
    positionAlloc4->Add (Vector(36.52909592148714,245.62993277560471, 0.0));
    positionAlloc4->Add (Vector(12.890655488972724,392.9084913401311, 0.0));
    positionAlloc4->Add (Vector(271.5021950045964,305.6808315998822, 0.0));
    positionAlloc4->Add (Vector(396.72046368271447,100.12666072911101, 0.0));
    positionAlloc4->Add (Vector(207.1021005398104,131.2650357700653, 0.0));
    positionAlloc4->Add (Vector(360.8829828189561,112.94017678984791, 0.0));
    positionAlloc4->Add (Vector(386.37163862475035,217.41278888014227, 0.0));
    positionAlloc4->Add (Vector(90.65259707700166,323.4712708278207, 0.0));
    positionAlloc4->Add (Vector(211.47947774455366,180.07414667630718, 0.0));
    positionAlloc4->Add (Vector(309.1998966484986,310.38685422749927, 0.0));
    positionAlloc4->Add (Vector(58.07419004471672,344.09048006814504, 0.0));
    positionAlloc4->Add (Vector(129.09093430982836,313.77496862853656, 0.0));
    positionAlloc4->Add (Vector(353.75381861087595,313.4877037180338, 0.0));
    positionAlloc4->Add (Vector(217.80300394219725,366.84434434533193, 0.0));
    positionAlloc4->Add (Vector(194.7684667336665,228.116760409286, 0.0));
    positionAlloc4->Add (Vector(205.4829010060331,195.73456537076203, 0.0));
    positionAlloc4->Add (Vector(48.930116913102054,373.12316258761854, 0.0));
    positionAlloc4->Add (Vector(0.05653208337075277,150.89608757012374, 0.0));
    positionAlloc4->Add (Vector(345.6124686962131,354.40510363526045, 0.0));
    positionAlloc4->Add (Vector(120.83485484744045,377.93058158435235, 0.0));
    positionAlloc4->Add (Vector(274.9790935488127,363.2683294117996, 0.0));
    positionAlloc4->Add (Vector(11.58290018208712,366.5936056008559, 0.0));
    positionAlloc4->Add (Vector(115.51135081595807,161.24111412862146, 0.0));
    positionAlloc4->Add (Vector(353.2123969594035,250.1471393917135, 0.0));
    positionAlloc4->Add (Vector(271.5418096424434,356.81800163428716, 0.0));
    positionAlloc4->Add (Vector(286.80948936179834,280.19127495180976, 0.0));
    positionAlloc4->Add (Vector(60.16933510609452,321.4000656429038, 0.0));
    positionAlloc4->Add (Vector(96.41978774780782,148.3759082040038, 0.0));
    positionAlloc4->Add (Vector(275.1785399470174,340.4690540454736, 0.0));
    positionAlloc4->Add (Vector(0.1572012768650044,98.32207439447602, 0.0));
    positionAlloc4->Add (Vector(159.05760741809317,134.29524392846582, 0.0));
    positionAlloc4->Add (Vector(31.684305201240328,334.15499440122375, 0.0));
    positionAlloc4->Add (Vector(350.43205963970996,293.5341733034572, 0.0));
    positionAlloc4->Add (Vector(369.3769110620389,94.47699742370096, 0.0));
    positionAlloc4->Add (Vector(193.25296380631428,228.03101174198636, 0.0));
    positionAlloc4->Add (Vector(212.8309265588822,188.64342778142964, 0.0));
    positionAlloc4->Add (Vector(222.90562978917157,125.3280940498037, 0.0));
    positionAlloc4->Add (Vector(94.96538522747535,19.80572477551692, 0.0));
    positionAlloc4->Add (Vector(176.56687998134254,375.52164145359217, 0.0));
    positionAlloc4->Add (Vector(375.89160844948265,363.3777173381792, 0.0));
    positionAlloc4->Add (Vector(392.24092762782686,360.1508717627266, 0.0));
    positionAlloc4->Add (Vector(20.135873682255134,68.74474346136293, 0.0));
    positionAlloc4->Add (Vector(79.86822764423258,323.4327921939133, 0.0));
    positionAlloc4->Add (Vector(163.85207402855352,270.8821055550128, 0.0));
    positionAlloc4->Add (Vector(244.13544460257134,248.1077218305745, 0.0));
    positionAlloc4->Add (Vector(191.1841956886144,73.17903450276893, 0.0));
    positionAlloc4->Add (Vector(55.05033050416137,3.2359693220332275, 0.0));
    positionAlloc4->Add (Vector(306.65007461806283,369.7756748884111, 0.0));
    positionAlloc4->Add (Vector(77.74960049350472,236.06657782643418, 0.0));
    positionAlloc4->Add (Vector(198.25186378032367,153.6415474031592, 0.0));
    positionAlloc4->Add (Vector(215.89757823640596,398.1058025839921, 0.0));
    positionAlloc4->Add (Vector(58.19608789048356,291.09131768678117, 0.0));
    positionAlloc4->Add (Vector(331.5243180152543,12.666276194647752, 0.0));
    positionAlloc4->Add (Vector(397.88309419173135,241.7069515131102, 0.0));
    positionAlloc4->Add (Vector(119.57427656410924,139.8802884195478, 0.0));
    positionAlloc4->Add (Vector(144.88133455239324,282.494101468807, 0.0));
    positionAlloc4->Add (Vector(396.3005397572139,130.86607032454776, 0.0));
    positionAlloc4->Add (Vector(149.35404230658412,228.4098504658787, 0.0));
    positionAlloc4->Add (Vector(137.88212381928108,85.4006661186883, 0.0));
    positionAlloc4->Add (Vector(168.76254100759013,266.10240712007715, 0.0));
    positionAlloc4->Add (Vector(50.49484515213525,101.82220313708932, 0.0));
    positionAlloc4->Add (Vector(45.584147039281355,350.6163030281846, 0.0));
    positionAlloc4->Add (Vector(41.73495992586829,143.6612861043002, 0.0));
    positionAlloc4->Add (Vector(147.72545823664362,351.15120756741914, 0.0));
    positionAlloc4->Add (Vector(305.9135840772316,28.354506284572388, 0.0));
    positionAlloc4->Add (Vector(56.576495063303156,74.4039957165565, 0.0));
    positionAlloc4->Add (Vector(80.41325948466861,222.4876688387917, 0.0));
    positionAlloc4->Add (Vector(73.84718571887427,217.76040980671473, 0.0));
    positionAlloc4->Add (Vector(203.967200190827,227.2276122311777, 0.0));
    positionAlloc4->Add (Vector(200.4600392499134,307.1449662701883, 0.0));
    positionAlloc4->Add (Vector(226.4521393963456,329.2947880405227, 0.0));
    positionAlloc4->Add (Vector(71.55388695069247,294.36753753875047, 0.0));
    positionAlloc4->Add (Vector(161.4491164965417,24.239924071628003, 0.0));
    positionAlloc4->Add (Vector(327.1185327209131,137.45867634223168, 0.0));
    positionAlloc4->Add (Vector(80.47105588246106,337.1152136257292, 0.0));
    positionAlloc4->Add (Vector(293.1297787685794,138.94106717226737, 0.0));
    positionAlloc4->Add (Vector(181.14453518200264,64.16001853107241, 0.0));
    positionAlloc4->Add (Vector(231.84571953614346,19.638992572362078, 0.0));
    positionAlloc4->Add (Vector(174.84954593829448,372.1329320025454, 0.0));
    positionAlloc4->Add (Vector(305.50996928457215,341.36916662167687, 0.0));
    positionAlloc4->Add (Vector(90.65479126714,133.9894969011731, 0.0));
    positionAlloc4->Add (Vector(191.26370200482646,317.4212754510209, 0.0));
    positionAlloc4->Add (Vector(19.421512827367238,346.7985588080116, 0.0));
    positionAlloc4->Add (Vector(126.19915300295213,257.4004761698132, 0.0));
    positionAlloc4->Add (Vector(41.46974482556227,259.7846023000728, 0.0));
    positionAlloc4->Add (Vector(34.63044747844486,204.2625850282985, 0.0));
    positionAlloc4->Add (Vector(267.63928493080397,311.82043525872047, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_6.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_6.flowmon", false, false);
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