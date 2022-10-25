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
	  uint16_t numberOfRrhs = 40;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 768;
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
    positionAlloc->Add (Vector (400.0,140.0, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,10.0, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,140.0, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,10.0, 0.0));
    positionAlloc->Add (Vector (270.0,270.0, 0.0));
    positionAlloc->Add (Vector (270.0,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (270.0,140.0, 0.0));
    positionAlloc->Add (Vector (270.0,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,400.0, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,140.0, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (140.0,270.0, 0.0));
    positionAlloc->Add (Vector (140.0,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (140.0,140.0, 0.0));
    positionAlloc->Add (Vector (140.0,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,10.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,270.0, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (10.0,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (10.0,140.0, 0.0));
    positionAlloc->Add (Vector (10.0,96.66666666666667, 0.0));
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
    positionAlloc4->Add (Vector(117.13753941790661,164.13847872723224, 0.0));
    positionAlloc4->Add (Vector(377.6457300853472,303.1283015771631, 0.0));
    positionAlloc4->Add (Vector(307.7191586410913,26.35908386661989, 0.0));
    positionAlloc4->Add (Vector(376.8234703147973,300.5343583234468, 0.0));
    positionAlloc4->Add (Vector(332.52325009185176,324.8944951235177, 0.0));
    positionAlloc4->Add (Vector(181.59660254284472,234.6093269089226, 0.0));
    positionAlloc4->Add (Vector(40.75113743843551,190.4871988040806, 0.0));
    positionAlloc4->Add (Vector(12.451438477930488,353.23426704682504, 0.0));
    positionAlloc4->Add (Vector(398.8119306846559,293.1842081658768, 0.0));
    positionAlloc4->Add (Vector(394.9519234435379,384.92638800416006, 0.0));
    positionAlloc4->Add (Vector(331.4334846160791,293.1272086960227, 0.0));
    positionAlloc4->Add (Vector(116.84840340484955,93.25013140201928, 0.0));
    positionAlloc4->Add (Vector(281.6803034868737,59.18171119860869, 0.0));
    positionAlloc4->Add (Vector(158.32185416470205,120.5740699976253, 0.0));
    positionAlloc4->Add (Vector(114.76471933910899,359.2907226801205, 0.0));
    positionAlloc4->Add (Vector(170.3463765622365,37.38225143703047, 0.0));
    positionAlloc4->Add (Vector(380.064419460918,347.09156131946185, 0.0));
    positionAlloc4->Add (Vector(40.99129311461387,208.5844101464721, 0.0));
    positionAlloc4->Add (Vector(109.80876599668944,220.59804078975387, 0.0));
    positionAlloc4->Add (Vector(125.87606110490897,240.1539719519881, 0.0));
    positionAlloc4->Add (Vector(394.94285245927676,225.7977057096448, 0.0));
    positionAlloc4->Add (Vector(132.76370229129788,36.03286653014379, 0.0));
    positionAlloc4->Add (Vector(267.90910144980285,338.30437979672786, 0.0));
    positionAlloc4->Add (Vector(307.72254727355266,369.5544069394181, 0.0));
    positionAlloc4->Add (Vector(240.27860329574617,346.8941140269557, 0.0));
    positionAlloc4->Add (Vector(375.4861016537649,115.82959206783525, 0.0));
    positionAlloc4->Add (Vector(50.550974245043534,290.9897061508156, 0.0));
    positionAlloc4->Add (Vector(138.21826754527328,22.343519210397478, 0.0));
    positionAlloc4->Add (Vector(120.15350346167195,335.217388772026, 0.0));
    positionAlloc4->Add (Vector(380.36914282868656,34.78258856347627, 0.0));
    positionAlloc4->Add (Vector(332.2274935616056,277.27791056418994, 0.0));
    positionAlloc4->Add (Vector(120.1614857228544,366.3028911160265, 0.0));
    positionAlloc4->Add (Vector(212.19326687132957,166.33053267500108, 0.0));
    positionAlloc4->Add (Vector(328.74398319832864,294.026408244834, 0.0));
    positionAlloc4->Add (Vector(88.94638157231935,12.941233679436914, 0.0));
    positionAlloc4->Add (Vector(176.15637271808916,182.60053313658238, 0.0));
    positionAlloc4->Add (Vector(172.1865529694764,3.193413914179555, 0.0));
    positionAlloc4->Add (Vector(116.64342219708108,74.36337427738331, 0.0));
    positionAlloc4->Add (Vector(216.92353929527303,235.4543466096356, 0.0));
    positionAlloc4->Add (Vector(233.42157584914304,379.39157663873584, 0.0));
    positionAlloc4->Add (Vector(145.10169445244935,165.0270484543684, 0.0));
    positionAlloc4->Add (Vector(140.14742849022977,191.81165748691961, 0.0));
    positionAlloc4->Add (Vector(246.5537722922691,54.819067647455235, 0.0));
    positionAlloc4->Add (Vector(349.2843838168878,311.49536669650786, 0.0));
    positionAlloc4->Add (Vector(17.517742840398487,196.29683383303015, 0.0));
    positionAlloc4->Add (Vector(30.79417503443289,163.7137687688482, 0.0));
    positionAlloc4->Add (Vector(116.25488543963426,97.77988923568634, 0.0));
    positionAlloc4->Add (Vector(236.76559128957842,389.70502671356894, 0.0));
    positionAlloc4->Add (Vector(141.13355513457552,146.6020669560976, 0.0));
    positionAlloc4->Add (Vector(264.034187456625,223.87031157260827, 0.0));
    positionAlloc4->Add (Vector(236.13882018368466,205.86373821398757, 0.0));
    positionAlloc4->Add (Vector(237.43577650754796,188.33069402685118, 0.0));
    positionAlloc4->Add (Vector(120.12790469409916,133.39118118532735, 0.0));
    positionAlloc4->Add (Vector(167.9776803692141,272.5109114204982, 0.0));
    positionAlloc4->Add (Vector(107.56926690779704,325.36966423886315, 0.0));
    positionAlloc4->Add (Vector(194.61940451619267,70.92094422470664, 0.0));
    positionAlloc4->Add (Vector(289.0181343143381,137.68052311684548, 0.0));
    positionAlloc4->Add (Vector(134.78086634951754,28.976607273026865, 0.0));
    positionAlloc4->Add (Vector(178.86149010042664,256.87021825675663, 0.0));
    positionAlloc4->Add (Vector(396.21335197072204,206.70487149680872, 0.0));
    positionAlloc4->Add (Vector(61.123824323745744,181.26844935199333, 0.0));
    positionAlloc4->Add (Vector(295.5496599260159,299.30178965261865, 0.0));
    positionAlloc4->Add (Vector(146.42691600014257,127.64187828433195, 0.0));
    positionAlloc4->Add (Vector(296.88714133351766,237.9930266632853, 0.0));
    positionAlloc4->Add (Vector(73.18284113456008,191.9597036987389, 0.0));
    positionAlloc4->Add (Vector(214.58839262371643,168.4386445679157, 0.0));
    positionAlloc4->Add (Vector(387.92763199985257,315.3818585344535, 0.0));
    positionAlloc4->Add (Vector(329.5595867590353,126.06064771577716, 0.0));
    positionAlloc4->Add (Vector(59.12895984348836,358.6512159625068, 0.0));
    positionAlloc4->Add (Vector(45.05491039295406,13.595462620176324, 0.0));
    positionAlloc4->Add (Vector(86.86867478980864,236.1274154369203, 0.0));
    positionAlloc4->Add (Vector(332.4856657560174,345.9155354708023, 0.0));
    positionAlloc4->Add (Vector(7.7387201000342465,220.46768346200105, 0.0));
    positionAlloc4->Add (Vector(277.69239483478606,136.91252003465073, 0.0));
    positionAlloc4->Add (Vector(335.62798990816754,9.235960811976751, 0.0));
    positionAlloc4->Add (Vector(277.69281129709367,276.5155613360018, 0.0));
    positionAlloc4->Add (Vector(150.2820603148761,302.9819095428995, 0.0));
    positionAlloc4->Add (Vector(223.62847490365425,370.8289559912719, 0.0));
    positionAlloc4->Add (Vector(166.06275134384157,315.14915288822675, 0.0));
    positionAlloc4->Add (Vector(213.91357123085984,201.94466498399728, 0.0));
    positionAlloc4->Add (Vector(374.09874081701605,344.41765950399775, 0.0));
    positionAlloc4->Add (Vector(92.2396530567747,170.66882742462374, 0.0));
    positionAlloc4->Add (Vector(31.529505904635037,65.35965936094472, 0.0));
    positionAlloc4->Add (Vector(232.75595163233083,122.89392243519113, 0.0));
    positionAlloc4->Add (Vector(261.87965704328985,321.04546192129436, 0.0));
    positionAlloc4->Add (Vector(357.746141425278,108.21364790729882, 0.0));
    positionAlloc4->Add (Vector(135.25234605980944,250.46949495128666, 0.0));
    positionAlloc4->Add (Vector(189.8561533169148,146.06035290631496, 0.0));
    positionAlloc4->Add (Vector(91.56165221103159,374.63724454928024, 0.0));
    positionAlloc4->Add (Vector(179.15374096560538,237.69641032828153, 0.0));
    positionAlloc4->Add (Vector(108.54263641800537,238.48851550577743, 0.0));
    positionAlloc4->Add (Vector(389.4307133105435,254.47828435475753, 0.0));
    positionAlloc4->Add (Vector(120.36932768063089,47.403431007586235, 0.0));
    positionAlloc4->Add (Vector(289.52902092461625,22.78790850256911, 0.0));
    positionAlloc4->Add (Vector(159.48206144295455,211.67909392853312, 0.0));
    positionAlloc4->Add (Vector(44.792521631640845,385.8574075201372, 0.0));
    positionAlloc4->Add (Vector(223.4771707248705,249.1780637531059, 0.0));
    positionAlloc4->Add (Vector(273.73537624873563,154.20101440467016, 0.0));
    positionAlloc4->Add (Vector(237.97695713860617,353.913385153127, 0.0));
    positionAlloc4->Add (Vector(289.5898662076688,110.0318245210845, 0.0));
    positionAlloc4->Add (Vector(69.54474815942677,175.29970894804393, 0.0));
    positionAlloc4->Add (Vector(305.983977879984,134.1329170951158, 0.0));
    positionAlloc4->Add (Vector(163.6381651115167,207.8376070079402, 0.0));
    positionAlloc4->Add (Vector(371.04927913270967,20.978047691014325, 0.0));
    positionAlloc4->Add (Vector(218.48686830583145,21.562904459258768, 0.0));
    positionAlloc4->Add (Vector(213.68637901464945,316.6163522867125, 0.0));
    positionAlloc4->Add (Vector(117.05103226264266,119.91783372943247, 0.0));
    positionAlloc4->Add (Vector(247.6660155332777,44.843830557787086, 0.0));
    positionAlloc4->Add (Vector(89.72703723720747,349.9310210083383, 0.0));
    positionAlloc4->Add (Vector(306.8764946132473,274.9648683454912, 0.0));
    positionAlloc4->Add (Vector(238.33523022477726,274.11051579919564, 0.0));
    positionAlloc4->Add (Vector(119.77143692685405,66.68543230511928, 0.0));
    positionAlloc4->Add (Vector(53.951451767300455,253.57402633106875, 0.0));
    positionAlloc4->Add (Vector(149.5750124169573,52.8438917536858, 0.0));
    positionAlloc4->Add (Vector(377.7597399848716,331.25063884219156, 0.0));
    positionAlloc4->Add (Vector(159.51436065937213,241.24626844529922, 0.0));
    positionAlloc4->Add (Vector(100.93342291905797,191.3635538094887, 0.0));
    positionAlloc4->Add (Vector(228.21884624781225,209.3825751282352, 0.0));
    positionAlloc4->Add (Vector(14.296609050625753,35.96911912649872, 0.0));
    positionAlloc4->Add (Vector(330.29370583888175,102.20173474275857, 0.0));
    positionAlloc4->Add (Vector(254.9556586803814,384.85280307920834, 0.0));
    positionAlloc4->Add (Vector(13.912852402902853,130.0341585002391, 0.0));
    positionAlloc4->Add (Vector(9.49845957687998,46.01853208605875, 0.0));
    positionAlloc4->Add (Vector(283.6967466923849,35.12699635942704, 0.0));
    positionAlloc4->Add (Vector(146.39142279926727,246.8772616882815, 0.0));
    positionAlloc4->Add (Vector(230.11501292064466,176.18559030266806, 0.0));
    positionAlloc4->Add (Vector(349.19364609798623,334.727935418634, 0.0));
    positionAlloc4->Add (Vector(176.05533073996696,377.5141021098243, 0.0));
    positionAlloc4->Add (Vector(60.94573051312886,36.111570548960216, 0.0));
    positionAlloc4->Add (Vector(3.0531751691746045,117.0948834527187, 0.0));
    positionAlloc4->Add (Vector(168.94044847234562,42.567573677116584, 0.0));
    positionAlloc4->Add (Vector(333.58493731238246,258.5082167332235, 0.0));
    positionAlloc4->Add (Vector(121.62638336497164,153.827419772233, 0.0));
    positionAlloc4->Add (Vector(196.04497227563513,364.5249482617567, 0.0));
    positionAlloc4->Add (Vector(292.81029882847275,189.21379044819133, 0.0));
    positionAlloc4->Add (Vector(292.1494752037654,157.794857715761, 0.0));
    positionAlloc4->Add (Vector(374.0751616168894,78.20003714000059, 0.0));
    positionAlloc4->Add (Vector(205.19177526743198,308.2572281749724, 0.0));
    positionAlloc4->Add (Vector(96.4115721646972,106.01106887427588, 0.0));
    positionAlloc4->Add (Vector(57.03964666405472,221.20228477150928, 0.0));
    positionAlloc4->Add (Vector(174.19706953223985,317.38225557821636, 0.0));
    positionAlloc4->Add (Vector(273.6098112469813,222.39854953641554, 0.0));
    positionAlloc4->Add (Vector(343.6798026676028,139.72116546823923, 0.0));
    positionAlloc4->Add (Vector(297.6562006258467,30.25601517413188, 0.0));
    positionAlloc4->Add (Vector(186.72859497463037,24.93152164378416, 0.0));
    positionAlloc4->Add (Vector(20.990299555046875,234.37344124264538, 0.0));
    positionAlloc4->Add (Vector(90.65872307596683,9.10666933692319, 0.0));
    positionAlloc4->Add (Vector(217.7877279854017,304.1649505874026, 0.0));
    positionAlloc4->Add (Vector(109.26843280380854,330.32969503247034, 0.0));
    positionAlloc4->Add (Vector(129.31595876672813,151.3805598897526, 0.0));
    positionAlloc4->Add (Vector(380.217847382247,134.9993829953434, 0.0));
    positionAlloc4->Add (Vector(27.970908928557403,191.10448709374168, 0.0));
    positionAlloc4->Add (Vector(283.03519234398,0.32639342168461205, 0.0));
    positionAlloc4->Add (Vector(39.52589365220041,276.91477710549117, 0.0));
    positionAlloc4->Add (Vector(155.43323404433403,145.43355505170283, 0.0));
    positionAlloc4->Add (Vector(23.175410312412392,195.66116098028056, 0.0));
    positionAlloc4->Add (Vector(5.833779713469678,253.28405464919186, 0.0));
    positionAlloc4->Add (Vector(133.54409879670865,98.379790684454, 0.0));
    positionAlloc4->Add (Vector(190.71387234120948,230.2555339143144, 0.0));
    positionAlloc4->Add (Vector(381.0732243865181,147.73539204765353, 0.0));
    positionAlloc4->Add (Vector(294.00306022314896,133.5351674748747, 0.0));
    positionAlloc4->Add (Vector(379.92078426640643,395.926643255823, 0.0));
    positionAlloc4->Add (Vector(382.7423452760035,143.97948369609063, 0.0));
    positionAlloc4->Add (Vector(188.8033896390516,360.6456557468708, 0.0));
    positionAlloc4->Add (Vector(252.80083888695893,278.0910790896377, 0.0));
    positionAlloc4->Add (Vector(350.67325940521596,140.51171064506755, 0.0));
    positionAlloc4->Add (Vector(67.60074460734403,27.627927772649173, 0.0));
    positionAlloc4->Add (Vector(58.92780413490692,267.11833063978713, 0.0));
    positionAlloc4->Add (Vector(30.097982861341865,136.58458190735126, 0.0));
    positionAlloc4->Add (Vector(148.63820411105058,112.65253257004329, 0.0));
    positionAlloc4->Add (Vector(142.86869344062575,246.1400021346079, 0.0));
    positionAlloc4->Add (Vector(392.57504661186624,329.12343172660354, 0.0));
    positionAlloc4->Add (Vector(288.17173887552116,193.0768150504528, 0.0));
    positionAlloc4->Add (Vector(31.207994102678825,266.05634234533613, 0.0));
    positionAlloc4->Add (Vector(298.7917767448421,320.1167622235985, 0.0));
    positionAlloc4->Add (Vector(71.61356849121363,65.47089786511613, 0.0));
    positionAlloc4->Add (Vector(27.33660287617301,126.39087204709708, 0.0));
    positionAlloc4->Add (Vector(182.67376503469984,83.43142914643718, 0.0));
    positionAlloc4->Add (Vector(309.1493030721354,382.9890743765514, 0.0));
    positionAlloc4->Add (Vector(77.27376164947759,271.9872402848865, 0.0));
    positionAlloc4->Add (Vector(68.81344888619898,321.3763782489325, 0.0));
    positionAlloc4->Add (Vector(144.10293389665503,144.0511083011005, 0.0));
    positionAlloc4->Add (Vector(38.051285501730135,247.18346326201276, 0.0));
    positionAlloc4->Add (Vector(101.37191064931716,70.19900052453889, 0.0));
    positionAlloc4->Add (Vector(311.0287353863464,387.4859218684223, 0.0));
    positionAlloc4->Add (Vector(253.6550692500894,207.6656088508338, 0.0));
    positionAlloc4->Add (Vector(43.321604861288286,135.46789512657966, 0.0));
    positionAlloc4->Add (Vector(254.66315756556202,156.36991735981755, 0.0));
    positionAlloc4->Add (Vector(176.07967469074887,70.31590121566586, 0.0));
    positionAlloc4->Add (Vector(160.9035977631094,292.4607662077074, 0.0));
    positionAlloc4->Add (Vector(122.17127593599079,199.72379160854751, 0.0));
    positionAlloc4->Add (Vector(160.7573736621162,269.28359917604865, 0.0));
    positionAlloc4->Add (Vector(322.5858279158909,335.8377583920629, 0.0));
    positionAlloc4->Add (Vector(252.90200395374728,112.32827667940968, 0.0));
    positionAlloc4->Add (Vector(385.24463374900915,235.56315949353112, 0.0));
    positionAlloc4->Add (Vector(240.59997316713338,265.6400734899832, 0.0));
    positionAlloc4->Add (Vector(288.3836723057625,136.2302197111127, 0.0));
    positionAlloc4->Add (Vector(396.2411468341089,96.89362288077672, 0.0));
    positionAlloc4->Add (Vector(371.22637651301966,227.03876506587903, 0.0));
    positionAlloc4->Add (Vector(341.4521679610878,57.6116302725608, 0.0));
    positionAlloc4->Add (Vector(262.0749924489589,33.096635327754775, 0.0));
    positionAlloc4->Add (Vector(26.74300583672813,263.7698065463683, 0.0));
    positionAlloc4->Add (Vector(44.98941515274524,246.7964692306138, 0.0));
    positionAlloc4->Add (Vector(80.39368887952274,230.07005934720047, 0.0));
    positionAlloc4->Add (Vector(370.53901485155615,391.2308030897814, 0.0));
    positionAlloc4->Add (Vector(395.3459286077539,49.676356406077545, 0.0));
    positionAlloc4->Add (Vector(24.127413047411615,34.22834180291026, 0.0));
    positionAlloc4->Add (Vector(186.80726811559944,194.1630726737716, 0.0));
    positionAlloc4->Add (Vector(180.00955195317601,185.48767087107646, 0.0));
    positionAlloc4->Add (Vector(341.04393950179883,277.20898720280593, 0.0));
    positionAlloc4->Add (Vector(68.26867205308855,366.72115625009064, 0.0));
    positionAlloc4->Add (Vector(233.6080779467565,372.95047959444724, 0.0));
    positionAlloc4->Add (Vector(266.93645968727577,397.43228452895374, 0.0));
    positionAlloc4->Add (Vector(398.59845218177304,376.92797912434213, 0.0));
    positionAlloc4->Add (Vector(2.0830863354884155,207.87277124418316, 0.0));
    positionAlloc4->Add (Vector(369.6874032318283,328.06200271541417, 0.0));
    positionAlloc4->Add (Vector(57.26251370784969,282.15739083580263, 0.0));
    positionAlloc4->Add (Vector(219.31167813706244,215.84061823061984, 0.0));
    positionAlloc4->Add (Vector(143.97070955262592,255.92545637701468, 0.0));
    positionAlloc4->Add (Vector(118.956699704135,194.03920326185425, 0.0));
    positionAlloc4->Add (Vector(78.59499450175393,258.8040488894366, 0.0));
    positionAlloc4->Add (Vector(277.289678337643,291.45999201507783, 0.0));
    positionAlloc4->Add (Vector(319.60683830607036,20.47912454370806, 0.0));
    positionAlloc4->Add (Vector(31.46523955459708,130.52591461512174, 0.0));
    positionAlloc4->Add (Vector(276.17449047435224,118.28962953049214, 0.0));
    positionAlloc4->Add (Vector(136.79007356694987,233.86732934019489, 0.0));
    positionAlloc4->Add (Vector(239.1840981749212,117.96973643608393, 0.0));
    positionAlloc4->Add (Vector(248.8033463436548,380.4829181547263, 0.0));
    positionAlloc4->Add (Vector(298.7326295531652,15.44304647578203, 0.0));
    positionAlloc4->Add (Vector(87.83223537113028,157.12294541788782, 0.0));
    positionAlloc4->Add (Vector(309.2144604343682,50.84969713891989, 0.0));
    positionAlloc4->Add (Vector(362.5224516819177,320.766997351312, 0.0));
    positionAlloc4->Add (Vector(5.453384536181538,337.61415267307143, 0.0));
    positionAlloc4->Add (Vector(74.10906562910364,77.81611998266733, 0.0));
    positionAlloc4->Add (Vector(239.28406967776505,230.89756579299362, 0.0));
    positionAlloc4->Add (Vector(266.38130047441786,207.4269423707955, 0.0));
    positionAlloc4->Add (Vector(92.37223425853469,180.6981999910459, 0.0));
    positionAlloc4->Add (Vector(255.00558669459377,216.39309852662416, 0.0));
    positionAlloc4->Add (Vector(258.22256267160253,228.00876481588895, 0.0));
    positionAlloc4->Add (Vector(352.71910483096894,78.47300037417999, 0.0));
    positionAlloc4->Add (Vector(82.14968151979822,0.7199839672083375, 0.0));
    positionAlloc4->Add (Vector(241.4390042634101,95.30246304662246, 0.0));
    positionAlloc4->Add (Vector(137.7366465111137,251.4833237679687, 0.0));
    positionAlloc4->Add (Vector(211.6508451126243,226.6625859573998, 0.0));
    positionAlloc4->Add (Vector(174.68807006826444,132.62309849643995, 0.0));
    positionAlloc4->Add (Vector(306.96324124225345,354.1834347575446, 0.0));
    positionAlloc4->Add (Vector(106.36642734255321,171.87860334328425, 0.0));
    positionAlloc4->Add (Vector(118.75730314683986,332.8482024762139, 0.0));
    positionAlloc4->Add (Vector(169.3680713148384,323.42425157974156, 0.0));
    positionAlloc4->Add (Vector(8.056527308935957,137.3108101860377, 0.0));
    positionAlloc4->Add (Vector(42.53539166455407,170.32924852699978, 0.0));
    positionAlloc4->Add (Vector(177.90411873896974,264.401509693226, 0.0));
    positionAlloc4->Add (Vector(44.985622755469734,43.523201287434254, 0.0));
    positionAlloc4->Add (Vector(232.80691947368172,385.57225983107907, 0.0));
    positionAlloc4->Add (Vector(349.0965634490097,375.6706269916716, 0.0));
    positionAlloc4->Add (Vector(92.70008778252476,192.02827844033115, 0.0));
    positionAlloc4->Add (Vector(90.13472131726826,335.3178135232353, 0.0));
    positionAlloc4->Add (Vector(301.5728140650342,144.48526253438635, 0.0));
    positionAlloc4->Add (Vector(176.98291548970064,399.7733152578071, 0.0));
    positionAlloc4->Add (Vector(234.6330132912895,228.91414742607256, 0.0));
    positionAlloc4->Add (Vector(273.79574267258374,26.087734525577353, 0.0));
    positionAlloc4->Add (Vector(331.4832650589018,167.14598994249633, 0.0));
    positionAlloc4->Add (Vector(31.740338648072218,75.5602872921731, 0.0));
    positionAlloc4->Add (Vector(110.6738665216278,252.20085319546365, 0.0));
    positionAlloc4->Add (Vector(116.92514229609246,355.1905628269589, 0.0));
    positionAlloc4->Add (Vector(343.72767449429887,99.14407755078511, 0.0));
    positionAlloc4->Add (Vector(177.44097200574907,19.600314247381867, 0.0));
    positionAlloc4->Add (Vector(180.32498137037956,304.2809628121034, 0.0));
    positionAlloc4->Add (Vector(79.02731822097122,21.51861147725316, 0.0));
    positionAlloc4->Add (Vector(205.42340074527488,34.8149062086446, 0.0));
    positionAlloc4->Add (Vector(313.02215705560434,238.587704446523, 0.0));
    positionAlloc4->Add (Vector(162.72498735407157,112.27868221898886, 0.0));
    positionAlloc4->Add (Vector(276.55031969537697,44.46213057369421, 0.0));
    positionAlloc4->Add (Vector(111.47206987684561,387.1636117551532, 0.0));
    positionAlloc4->Add (Vector(150.6822681417276,145.08099641930875, 0.0));
    positionAlloc4->Add (Vector(56.57874510512486,179.31228136902374, 0.0));
    positionAlloc4->Add (Vector(52.9445768279317,24.521004507370627, 0.0));
    positionAlloc4->Add (Vector(38.996816896526724,293.9143606132879, 0.0));
    positionAlloc4->Add (Vector(55.87577789042846,108.48189842123107, 0.0));
    positionAlloc4->Add (Vector(14.625382048413638,62.9077597599041, 0.0));
    positionAlloc4->Add (Vector(218.91844308494748,154.0105648710261, 0.0));
    positionAlloc4->Add (Vector(288.1638638532263,178.00294102735074, 0.0));
    positionAlloc4->Add (Vector(88.19445517022548,286.0929723372815, 0.0));
    positionAlloc4->Add (Vector(311.0443192400923,361.5731367704665, 0.0));
    positionAlloc4->Add (Vector(354.89128292003284,6.212343604672732, 0.0));
    positionAlloc4->Add (Vector(287.3200001838466,331.58867259819027, 0.0));
    positionAlloc4->Add (Vector(136.18017979757332,243.97176778523658, 0.0));
    positionAlloc4->Add (Vector(283.5103004307629,378.9857875212178, 0.0));
    positionAlloc4->Add (Vector(288.9362509093729,364.9892804286417, 0.0));
    positionAlloc4->Add (Vector(219.50441825910306,155.07713921548066, 0.0));
    positionAlloc4->Add (Vector(346.6953266289327,5.0691005745259865, 0.0));
    positionAlloc4->Add (Vector(142.10199230665904,399.22873212751415, 0.0));
    positionAlloc4->Add (Vector(248.7125145427913,382.98138239886305, 0.0));
    positionAlloc4->Add (Vector(348.3501380591151,77.62541499303967, 0.0));
    positionAlloc4->Add (Vector(39.37329748972482,347.5013206997143, 0.0));
    positionAlloc4->Add (Vector(362.8289697063827,376.4162042734968, 0.0));
    positionAlloc4->Add (Vector(91.4353770445103,112.96976361053459, 0.0));
    positionAlloc4->Add (Vector(343.6167198147922,178.17275047741325, 0.0));
    positionAlloc4->Add (Vector(190.53740713907422,150.11856732283852, 0.0));
    positionAlloc4->Add (Vector(237.92872137965256,201.25280430598184, 0.0));
    positionAlloc4->Add (Vector(72.94945135675945,96.91338946196484, 0.0));
    positionAlloc4->Add (Vector(329.6936899507144,233.87124576397608, 0.0));
    positionAlloc4->Add (Vector(123.87719673277728,186.50653590761706, 0.0));
    positionAlloc4->Add (Vector(307.6462788395469,44.16815405419605, 0.0));
    positionAlloc4->Add (Vector(310.69975884713585,34.86170754083422, 0.0));
    positionAlloc4->Add (Vector(396.6305826173351,157.68444810032807, 0.0));
    positionAlloc4->Add (Vector(74.52812095575231,170.98271116142504, 0.0));
    positionAlloc4->Add (Vector(116.25196092325653,298.242686739063, 0.0));
    positionAlloc4->Add (Vector(135.14853579293225,379.79724998699373, 0.0));
    positionAlloc4->Add (Vector(277.8326144179078,117.3160480512077, 0.0));
    positionAlloc4->Add (Vector(92.12403518001157,125.02369034571488, 0.0));
    positionAlloc4->Add (Vector(194.20519462589638,72.3700055503309, 0.0));
    positionAlloc4->Add (Vector(24.913588256272632,107.40847490154542, 0.0));
    positionAlloc4->Add (Vector(14.13927333079692,147.73397697704888, 0.0));
    positionAlloc4->Add (Vector(132.77741088208978,399.6482397573722, 0.0));
    positionAlloc4->Add (Vector(294.6910215290879,242.85688813622025, 0.0));
    positionAlloc4->Add (Vector(376.9838127524118,299.0076544629606, 0.0));
    positionAlloc4->Add (Vector(154.20710945220196,350.5125251363947, 0.0));
    positionAlloc4->Add (Vector(382.0388265763357,238.64225573526397, 0.0));
    positionAlloc4->Add (Vector(319.6720097438469,316.1194425252094, 0.0));
    positionAlloc4->Add (Vector(106.39063136319402,133.88134747690108, 0.0));
    positionAlloc4->Add (Vector(396.43439680526353,142.40107885098934, 0.0));
    positionAlloc4->Add (Vector(363.9678942864997,125.92226991142965, 0.0));
    positionAlloc4->Add (Vector(205.50975900855718,398.26255377996176, 0.0));
    positionAlloc4->Add (Vector(392.25025268242973,161.86664401787309, 0.0));
    positionAlloc4->Add (Vector(172.81383273757123,224.6308732073394, 0.0));
    positionAlloc4->Add (Vector(323.2169428426647,251.8089443525478, 0.0));
    positionAlloc4->Add (Vector(118.60210475570811,272.76696750896815, 0.0));
    positionAlloc4->Add (Vector(167.55806815493003,68.79071413950398, 0.0));
    positionAlloc4->Add (Vector(5.359737525187613,95.49631916559052, 0.0));
    positionAlloc4->Add (Vector(123.21747519696711,188.7774973005618, 0.0));
    positionAlloc4->Add (Vector(247.64665649264853,165.37848907520774, 0.0));
    positionAlloc4->Add (Vector(359.76966063363983,151.22700303605083, 0.0));
    positionAlloc4->Add (Vector(373.1763901046202,205.63562775213566, 0.0));
    positionAlloc4->Add (Vector(44.21865015066131,238.87413843606762, 0.0));
    positionAlloc4->Add (Vector(239.07393032451506,296.8873732181689, 0.0));
    positionAlloc4->Add (Vector(80.25077659712699,302.2978851616303, 0.0));
    positionAlloc4->Add (Vector(211.4880710748033,318.4811824113326, 0.0));
    positionAlloc4->Add (Vector(94.97448366400465,277.82693737533947, 0.0));
    positionAlloc4->Add (Vector(42.41175338495977,19.42822694592028, 0.0));
    positionAlloc4->Add (Vector(229.06686727913072,201.87632754188604, 0.0));
    positionAlloc4->Add (Vector(288.013870941929,266.4723844016545, 0.0));
    positionAlloc4->Add (Vector(370.0314942732235,208.24775410422677, 0.0));
    positionAlloc4->Add (Vector(356.15846931616744,317.5137154189797, 0.0));
    positionAlloc4->Add (Vector(306.0236326273624,88.0029183351105, 0.0));
    positionAlloc4->Add (Vector(337.0114171214272,302.59630891867806, 0.0));
    positionAlloc4->Add (Vector(386.61626844228334,359.4168701501392, 0.0));
    positionAlloc4->Add (Vector(46.9680086011258,85.53668428840884, 0.0));
    positionAlloc4->Add (Vector(183.4532874876784,320.48023991261647, 0.0));
    positionAlloc4->Add (Vector(288.0113827331608,332.3939787744025, 0.0));
    positionAlloc4->Add (Vector(313.1200989833314,198.28565208494476, 0.0));
    positionAlloc4->Add (Vector(289.8497696460663,388.14000459399784, 0.0));
    positionAlloc4->Add (Vector(237.8501342508642,149.3083333776038, 0.0));
    positionAlloc4->Add (Vector(273.4215806995411,311.38549306730334, 0.0));
    positionAlloc4->Add (Vector(46.997444081263765,132.11196465471994, 0.0));
    positionAlloc4->Add (Vector(200.99510072876603,154.76523044675594, 0.0));
    positionAlloc4->Add (Vector(8.919266552722371,96.45820133445015, 0.0));
    positionAlloc4->Add (Vector(305.32657249941326,337.3650767398528, 0.0));
    positionAlloc4->Add (Vector(216.6844245962937,228.27990714478136, 0.0));
    positionAlloc4->Add (Vector(304.3116448664004,177.64194225659492, 0.0));
    positionAlloc4->Add (Vector(26.512600342808046,91.65729248410459, 0.0));
    positionAlloc4->Add (Vector(348.7431258248344,399.785649873039, 0.0));
    positionAlloc4->Add (Vector(74.63642900873828,50.50049340713758, 0.0));
    positionAlloc4->Add (Vector(274.16367504178424,202.35956071194033, 0.0));
    positionAlloc4->Add (Vector(345.8299534221278,90.40147241837846, 0.0));
    positionAlloc4->Add (Vector(227.4788741088969,396.1897173612084, 0.0));
    positionAlloc4->Add (Vector(376.05570959542666,263.76798568558826, 0.0));
    positionAlloc4->Add (Vector(356.63223002855466,150.75792363569312, 0.0));
    positionAlloc4->Add (Vector(86.56209482813546,77.94500504393133, 0.0));
    positionAlloc4->Add (Vector(117.49641839585428,10.817006579212185, 0.0));
    positionAlloc4->Add (Vector(287.3862029020658,78.82852170314925, 0.0));
    positionAlloc4->Add (Vector(189.1082174978391,177.64241499291978, 0.0));
    positionAlloc4->Add (Vector(155.36636252893405,63.59312085878357, 0.0));
    positionAlloc4->Add (Vector(68.85807626103579,319.97033263758857, 0.0));
    positionAlloc4->Add (Vector(265.97222497997615,147.34483457500113, 0.0));
    positionAlloc4->Add (Vector(1.4403750063747278,251.8491413699288, 0.0));
    positionAlloc4->Add (Vector(219.0816467259185,294.73464794706916, 0.0));
    positionAlloc4->Add (Vector(90.1394845837852,25.391892862851996, 0.0));
    positionAlloc4->Add (Vector(272.33826024883484,34.24257117889162, 0.0));
    positionAlloc4->Add (Vector(347.88431565930307,347.20227582034136, 0.0));
    positionAlloc4->Add (Vector(341.30073860104477,388.38040022524547, 0.0));
    positionAlloc4->Add (Vector(176.0974554720344,181.76655603114807, 0.0));
    positionAlloc4->Add (Vector(276.8785486789176,1.7952036925763082, 0.0));
    positionAlloc4->Add (Vector(396.9041242666664,221.44416303831616, 0.0));
    positionAlloc4->Add (Vector(183.15913036101006,9.953483019727516, 0.0));
    positionAlloc4->Add (Vector(389.2412745222297,138.89437615711006, 0.0));
    positionAlloc4->Add (Vector(203.33969142859831,381.1030289054562, 0.0));
    positionAlloc4->Add (Vector(281.4524186199387,318.61295515673015, 0.0));
    positionAlloc4->Add (Vector(339.90328506353546,2.0347760166558704, 0.0));
    positionAlloc4->Add (Vector(10.158005297405426,237.4731352983414, 0.0));
    positionAlloc4->Add (Vector(294.4010456583936,276.02966939706783, 0.0));
    positionAlloc4->Add (Vector(21.70102210539855,374.1262193213529, 0.0));
    positionAlloc4->Add (Vector(342.7672011487996,70.18864736078339, 0.0));
    positionAlloc4->Add (Vector(307.6197768965556,0.8790381970106065, 0.0));
    positionAlloc4->Add (Vector(81.6051176078438,25.298004386209037, 0.0));
    positionAlloc4->Add (Vector(148.0887160990831,131.6105016232084, 0.0));
    positionAlloc4->Add (Vector(1.3607921347107865,169.30095833623017, 0.0));
    positionAlloc4->Add (Vector(350.4802753553595,218.84968609443246, 0.0));
    positionAlloc4->Add (Vector(80.88257007903547,9.227573490857965, 0.0));
    positionAlloc4->Add (Vector(30.21818985104354,195.64058269182127, 0.0));
    positionAlloc4->Add (Vector(317.54335605605706,132.62146415546638, 0.0));
    positionAlloc4->Add (Vector(14.470515021967456,254.51826293798362, 0.0));
    positionAlloc4->Add (Vector(301.7865030752074,245.2952059155394, 0.0));
    positionAlloc4->Add (Vector(99.78640187244565,192.37300396741136, 0.0));
    positionAlloc4->Add (Vector(4.98855515012262,76.946200722676, 0.0));
    positionAlloc4->Add (Vector(54.63748996036597,322.5443010017202, 0.0));
    positionAlloc4->Add (Vector(245.8506534623632,321.4008865051361, 0.0));
    positionAlloc4->Add (Vector(165.61839834080251,184.91492133338966, 0.0));
    positionAlloc4->Add (Vector(264.06118223800047,256.389646494649, 0.0));
    positionAlloc4->Add (Vector(327.96447498614225,18.53505107612614, 0.0));
    positionAlloc4->Add (Vector(221.57966358839553,399.19223961698435, 0.0));
    positionAlloc4->Add (Vector(295.6087013272993,170.27749655086632, 0.0));
    positionAlloc4->Add (Vector(344.27561043041027,289.6128886857008, 0.0));
    positionAlloc4->Add (Vector(310.46936402770194,247.16837821464702, 0.0));
    positionAlloc4->Add (Vector(98.17823049622692,62.39614938709819, 0.0));
    positionAlloc4->Add (Vector(368.27919688888966,386.5332136828853, 0.0));
    positionAlloc4->Add (Vector(349.75306888125965,214.00561400152586, 0.0));
    positionAlloc4->Add (Vector(367.76784889302075,25.194067561475553, 0.0));
    positionAlloc4->Add (Vector(204.11915736049028,378.8328664366362, 0.0));
    positionAlloc4->Add (Vector(352.1059493985897,13.186472551525474, 0.0));
    positionAlloc4->Add (Vector(166.377742933269,158.51220671542623, 0.0));
    positionAlloc4->Add (Vector(394.9198871785185,227.76134032875163, 0.0));
    positionAlloc4->Add (Vector(157.60301593212313,78.7025129249666, 0.0));
    positionAlloc4->Add (Vector(8.026323772206867,160.0873895134494, 0.0));
    positionAlloc4->Add (Vector(366.50241483190536,295.9734557815489, 0.0));
    positionAlloc4->Add (Vector(230.85199166788945,28.38514510528851, 0.0));
    positionAlloc4->Add (Vector(51.0562808622189,283.1959423968478, 0.0));
    positionAlloc4->Add (Vector(341.9288317730983,91.14058613973177, 0.0));
    positionAlloc4->Add (Vector(139.5893176350325,59.54711326204682, 0.0));
    positionAlloc4->Add (Vector(82.09031841935803,195.98073709163413, 0.0));
    positionAlloc4->Add (Vector(399.292503213284,166.0779285514023, 0.0));
    positionAlloc4->Add (Vector(297.2574708184888,322.70516600809, 0.0));
    positionAlloc4->Add (Vector(203.7460897363115,108.71445553089262, 0.0));
    positionAlloc4->Add (Vector(214.39652221948072,191.65113056128553, 0.0));
    positionAlloc4->Add (Vector(393.1319701336029,383.2579253572493, 0.0));
    positionAlloc4->Add (Vector(300.4213421543394,75.28955387448684, 0.0));
    positionAlloc4->Add (Vector(20.951177292238434,244.97961872588058, 0.0));
    positionAlloc4->Add (Vector(319.9067258606193,179.9372280733228, 0.0));
    positionAlloc4->Add (Vector(6.09093020830076,252.40852316980832, 0.0));
    positionAlloc4->Add (Vector(20.499014452162577,24.313514014206294, 0.0));
    positionAlloc4->Add (Vector(379.8771411135676,395.70701329678235, 0.0));
    positionAlloc4->Add (Vector(91.1437063839366,131.81622105726478, 0.0));
    positionAlloc4->Add (Vector(317.3564423794891,305.6346642647472, 0.0));
    positionAlloc4->Add (Vector(114.66399698885455,157.79312742835367, 0.0));
    positionAlloc4->Add (Vector(326.69055725786177,390.1980707420743, 0.0));
    positionAlloc4->Add (Vector(382.58492756078306,225.46924158504194, 0.0));
    positionAlloc4->Add (Vector(226.4425145401129,78.85279753057053, 0.0));
    positionAlloc4->Add (Vector(314.1337173925711,301.8234963040991, 0.0));
    positionAlloc4->Add (Vector(208.18168270012922,247.74603778137953, 0.0));
    positionAlloc4->Add (Vector(229.06052253495938,165.3015363447595, 0.0));
    positionAlloc4->Add (Vector(181.04988283273653,243.53729529551487, 0.0));
    positionAlloc4->Add (Vector(152.0418243874016,34.995473231667475, 0.0));
    positionAlloc4->Add (Vector(181.02953504330358,242.85642837883069, 0.0));
    positionAlloc4->Add (Vector(53.303006426754074,181.48515154413852, 0.0));
    positionAlloc4->Add (Vector(233.09975619622864,71.45076599248638, 0.0));
    positionAlloc4->Add (Vector(32.301594240744436,230.9970441327436, 0.0));
    positionAlloc4->Add (Vector(270.71862060372473,234.6737745014016, 0.0));
    positionAlloc4->Add (Vector(60.36971899708834,89.26701571607141, 0.0));
    positionAlloc4->Add (Vector(326.9561627369912,245.9084534629714, 0.0));
    positionAlloc4->Add (Vector(328.03484574566323,356.66070847921765, 0.0));
    positionAlloc4->Add (Vector(283.98421684749474,254.5237257752042, 0.0));
    positionAlloc4->Add (Vector(65.54838220465871,35.352137567271534, 0.0));
    positionAlloc4->Add (Vector(289.5700805108909,3.876417647776531, 0.0));
    positionAlloc4->Add (Vector(380.96431191333664,103.63268663114731, 0.0));
    positionAlloc4->Add (Vector(385.0279737493444,387.4658498385134, 0.0));
    positionAlloc4->Add (Vector(21.886163472003783,29.747098887562373, 0.0));
    positionAlloc4->Add (Vector(354.1405011616339,48.66829269404667, 0.0));
    positionAlloc4->Add (Vector(349.02572076317335,146.33954113699215, 0.0));
    positionAlloc4->Add (Vector(345.61094670779954,265.2547530432578, 0.0));
    positionAlloc4->Add (Vector(210.49748481014853,9.202031976874103, 0.0));
    positionAlloc4->Add (Vector(20.44092335900065,76.70230148963984, 0.0));
    positionAlloc4->Add (Vector(35.76593344137824,223.98610425000015, 0.0));
    positionAlloc4->Add (Vector(60.75235169831315,163.10026312004888, 0.0));
    positionAlloc4->Add (Vector(207.68603078060752,336.32276759936184, 0.0));
    positionAlloc4->Add (Vector(192.9541904308086,190.93418494780772, 0.0));
    positionAlloc4->Add (Vector(70.90847618123992,120.45507603086772, 0.0));
    positionAlloc4->Add (Vector(173.04165381417153,345.52428666892376, 0.0));
    positionAlloc4->Add (Vector(139.33644395086122,279.8921549515662, 0.0));
    positionAlloc4->Add (Vector(237.88820765085143,127.12135246536982, 0.0));
    positionAlloc4->Add (Vector(266.21086355958136,267.105667668613, 0.0));
    positionAlloc4->Add (Vector(209.91756732032337,167.5551678647517, 0.0));
    positionAlloc4->Add (Vector(215.16350212964298,372.8124060572037, 0.0));
    positionAlloc4->Add (Vector(222.1495634822615,227.27105567681429, 0.0));
    positionAlloc4->Add (Vector(238.51548410983492,153.81592229028428, 0.0));
    positionAlloc4->Add (Vector(86.16463494157061,339.8284797115316, 0.0));
    positionAlloc4->Add (Vector(189.42859219982364,392.9499778555112, 0.0));
    positionAlloc4->Add (Vector(215.35414895695354,26.437116489683767, 0.0));
    positionAlloc4->Add (Vector(220.02246794619737,250.699326177957, 0.0));
    positionAlloc4->Add (Vector(380.0785826526021,109.81742709710676, 0.0));
    positionAlloc4->Add (Vector(54.985849796043325,367.3061871853497, 0.0));
    positionAlloc4->Add (Vector(361.24610269191874,239.65223533581758, 0.0));
    positionAlloc4->Add (Vector(274.38915981938294,281.25574304740815, 0.0));
    positionAlloc4->Add (Vector(204.41678812207923,331.6180096406852, 0.0));
    positionAlloc4->Add (Vector(221.7825766197753,244.05414728086637, 0.0));
    positionAlloc4->Add (Vector(121.23223846226807,75.90089030627966, 0.0));
    positionAlloc4->Add (Vector(216.24978564139545,340.30350912253016, 0.0));
    positionAlloc4->Add (Vector(110.42354231193539,163.07978519771063, 0.0));
    positionAlloc4->Add (Vector(220.25828033801633,99.69670355122608, 0.0));
    positionAlloc4->Add (Vector(163.79824036559364,393.50597693539424, 0.0));
    positionAlloc4->Add (Vector(396.19134444916835,82.95626287212725, 0.0));
    positionAlloc4->Add (Vector(138.42077627435754,135.98850502278128, 0.0));
    positionAlloc4->Add (Vector(225.51944705777936,131.5221565461481, 0.0));
    positionAlloc4->Add (Vector(73.5465349372574,247.31197423197156, 0.0));
    positionAlloc4->Add (Vector(310.51286192306986,231.5538446665018, 0.0));
    positionAlloc4->Add (Vector(146.84968959545697,64.85075633528577, 0.0));
    positionAlloc4->Add (Vector(269.4928557775142,212.3101037345636, 0.0));
    positionAlloc4->Add (Vector(246.2925737326153,68.76364294106847, 0.0));
    positionAlloc4->Add (Vector(160.859104591957,109.1810534939349, 0.0));
    positionAlloc4->Add (Vector(358.5837311781563,322.18805482567217, 0.0));
    positionAlloc4->Add (Vector(268.84096883906886,6.647139941625602, 0.0));
    positionAlloc4->Add (Vector(94.2775826593496,360.8226650299164, 0.0));
    positionAlloc4->Add (Vector(62.282059450460196,240.460684857266, 0.0));
    positionAlloc4->Add (Vector(87.2101670768731,143.78352976549746, 0.0));
    positionAlloc4->Add (Vector(144.79586429840157,37.66634530666968, 0.0));
    positionAlloc4->Add (Vector(377.2134270128198,85.33407408878482, 0.0));
    positionAlloc4->Add (Vector(332.1882354551303,138.15631790081704, 0.0));
    positionAlloc4->Add (Vector(195.65927758553713,84.78993803282928, 0.0));
    positionAlloc4->Add (Vector(318.9161282483432,177.53759882462057, 0.0));
    positionAlloc4->Add (Vector(241.73510666186542,312.69157639903113, 0.0));
    positionAlloc4->Add (Vector(357.27500890768636,53.78249334380052, 0.0));
    positionAlloc4->Add (Vector(131.13679060273537,105.76917524394207, 0.0));
    positionAlloc4->Add (Vector(314.06143716544136,348.2542878748429, 0.0));
    positionAlloc4->Add (Vector(289.70083479158774,353.20715201111943, 0.0));
    positionAlloc4->Add (Vector(40.09854028721276,88.92837816031145, 0.0));
    positionAlloc4->Add (Vector(166.5622112617935,209.47766583740636, 0.0));
    positionAlloc4->Add (Vector(395.07844451447147,112.21625132161508, 0.0));
    positionAlloc4->Add (Vector(285.7521934330595,218.1692100725423, 0.0));
    positionAlloc4->Add (Vector(254.09973062275685,45.47698853362236, 0.0));
    positionAlloc4->Add (Vector(92.84361411777753,348.24047045117834, 0.0));
    positionAlloc4->Add (Vector(310.46607481858615,214.40904499700667, 0.0));
    positionAlloc4->Add (Vector(254.74656768000182,54.905334998769064, 0.0));
    positionAlloc4->Add (Vector(34.29018193279614,210.02777434405928, 0.0));
    positionAlloc4->Add (Vector(339.86762360558293,76.83777652501914, 0.0));
    positionAlloc4->Add (Vector(354.41783603072463,348.8508169506773, 0.0));
    positionAlloc4->Add (Vector(96.87090101138125,15.402111182875178, 0.0));
    positionAlloc4->Add (Vector(54.16401743518162,10.323459694967951, 0.0));
    positionAlloc4->Add (Vector(194.57301183790952,340.4568017384275, 0.0));
    positionAlloc4->Add (Vector(305.097636400215,298.0763978758413, 0.0));
    positionAlloc4->Add (Vector(254.82467750116595,364.3577111010997, 0.0));
    positionAlloc4->Add (Vector(375.0520116164473,154.78836601568221, 0.0));
    positionAlloc4->Add (Vector(129.80610466186096,38.83285383770363, 0.0));
    positionAlloc4->Add (Vector(375.79406075762927,227.15213410917096, 0.0));
    positionAlloc4->Add (Vector(217.40232679197754,12.534213299581243, 0.0));
    positionAlloc4->Add (Vector(137.7569104220159,60.73159268517729, 0.0));
    positionAlloc4->Add (Vector(237.7427925521844,386.2736665742847, 0.0));
    positionAlloc4->Add (Vector(171.48058883928076,47.27790710291329, 0.0));
    positionAlloc4->Add (Vector(204.87767224048437,297.0385347248872, 0.0));
    positionAlloc4->Add (Vector(319.0900386195103,180.9029251368128, 0.0));
    positionAlloc4->Add (Vector(266.8276242923248,369.5634480253141, 0.0));
    positionAlloc4->Add (Vector(199.5452822363288,128.4255126545968, 0.0));
    positionAlloc4->Add (Vector(391.855269928622,58.49395340723187, 0.0));
    positionAlloc4->Add (Vector(299.2660963232254,55.616362933357834, 0.0));
    positionAlloc4->Add (Vector(124.10307809020264,388.80096561223263, 0.0));
    positionAlloc4->Add (Vector(70.42297318018873,196.42573170239564, 0.0));
    positionAlloc4->Add (Vector(141.9754320416387,266.9734641338447, 0.0));
    positionAlloc4->Add (Vector(138.96180577489173,71.09711492566615, 0.0));
    positionAlloc4->Add (Vector(289.17720338167896,219.31642126167796, 0.0));
    positionAlloc4->Add (Vector(64.30906965806598,337.12704859903704, 0.0));
    positionAlloc4->Add (Vector(84.23397942352663,342.3505539020061, 0.0));
    positionAlloc4->Add (Vector(142.22496256838687,268.53581188649895, 0.0));
    positionAlloc4->Add (Vector(181.46257779917886,11.324899375036646, 0.0));
    positionAlloc4->Add (Vector(369.05970704654163,211.82354514165974, 0.0));
    positionAlloc4->Add (Vector(245.18566274485738,302.97382835574604, 0.0));
    positionAlloc4->Add (Vector(160.9626139180467,323.5843228153118, 0.0));
    positionAlloc4->Add (Vector(22.742146993217325,38.487837192184756, 0.0));
    positionAlloc4->Add (Vector(271.22060507341183,130.8063548014063, 0.0));
    positionAlloc4->Add (Vector(386.40073432892467,171.96713722464318, 0.0));
    positionAlloc4->Add (Vector(93.75814901112194,7.8971374455270915, 0.0));
    positionAlloc4->Add (Vector(143.34381928717522,216.84338268291435, 0.0));
    positionAlloc4->Add (Vector(387.17280355430296,52.08711248718143, 0.0));
    positionAlloc4->Add (Vector(133.2538102299418,315.25773649475025, 0.0));
    positionAlloc4->Add (Vector(390.15524771160995,161.22234079268765, 0.0));
    positionAlloc4->Add (Vector(104.32484584693111,71.07360865685068, 0.0));
    positionAlloc4->Add (Vector(153.5250160626532,6.562987066828674, 0.0));
    positionAlloc4->Add (Vector(24.24001255772592,107.81725826992918, 0.0));
    positionAlloc4->Add (Vector(394.45664927727125,387.27364016177364, 0.0));
    positionAlloc4->Add (Vector(366.7308203088609,313.28267234364256, 0.0));
    positionAlloc4->Add (Vector(100.94581550132627,14.498888106201546, 0.0));
    positionAlloc4->Add (Vector(317.1327103557609,25.12203093523988, 0.0));
    positionAlloc4->Add (Vector(234.68169367163676,172.79316630129046, 0.0));
    positionAlloc4->Add (Vector(250.13989869257412,388.27836022274016, 0.0));
    positionAlloc4->Add (Vector(288.414695530294,53.46994372957759, 0.0));
    positionAlloc4->Add (Vector(201.26095361242568,89.2446637283292, 0.0));
    positionAlloc4->Add (Vector(335.43190085361385,209.35018705333528, 0.0));
    positionAlloc4->Add (Vector(2.2647944584215374,72.58737700251463, 0.0));
    positionAlloc4->Add (Vector(323.14076539744417,321.30124616774134, 0.0));
    positionAlloc4->Add (Vector(187.84540913087375,221.24560262106786, 0.0));
    positionAlloc4->Add (Vector(115.56473895307104,2.2571288122936295, 0.0));
    positionAlloc4->Add (Vector(195.46768521701532,302.898958708594, 0.0));
    positionAlloc4->Add (Vector(266.21164477674466,211.4753250030561, 0.0));
    positionAlloc4->Add (Vector(293.80132291827977,89.74732019811866, 0.0));
    positionAlloc4->Add (Vector(78.54898781549586,78.49002400969387, 0.0));
    positionAlloc4->Add (Vector(281.11483216269033,193.5668933514012, 0.0));
    positionAlloc4->Add (Vector(331.89278506501967,25.847548661885213, 0.0));
    positionAlloc4->Add (Vector(363.85332800022906,26.52843772340563, 0.0));
    positionAlloc4->Add (Vector(356.06973203158196,84.4560231556403, 0.0));
    positionAlloc4->Add (Vector(52.02078356675366,222.72507001146283, 0.0));
    positionAlloc4->Add (Vector(309.22900557301267,8.1949931149782, 0.0));
    positionAlloc4->Add (Vector(39.89303242230879,26.749687756474927, 0.0));
    positionAlloc4->Add (Vector(153.42759208717897,366.4618272672337, 0.0));
    positionAlloc4->Add (Vector(181.54985899222206,375.4818030703048, 0.0));
    positionAlloc4->Add (Vector(257.23531820508026,106.80221846261762, 0.0));
    positionAlloc4->Add (Vector(218.15243106798684,149.7969316519612, 0.0));
    positionAlloc4->Add (Vector(321.52988198339307,278.06928004016004, 0.0));
    positionAlloc4->Add (Vector(47.29417520504278,357.84993874000173, 0.0));
    positionAlloc4->Add (Vector(179.6455383138107,281.81665232259314, 0.0));
    positionAlloc4->Add (Vector(42.30593176723217,192.25278258158497, 0.0));
    positionAlloc4->Add (Vector(65.48901113288976,44.30771091625809, 0.0));
    positionAlloc4->Add (Vector(54.37586080846928,270.41158371098743, 0.0));
    positionAlloc4->Add (Vector(287.9455637030665,198.97580853816638, 0.0));
    positionAlloc4->Add (Vector(16.92062658260678,15.573371230570698, 0.0));
    positionAlloc4->Add (Vector(136.24076841363316,388.3841069282066, 0.0));
    positionAlloc4->Add (Vector(343.07273650976555,32.946330739391705, 0.0));
    positionAlloc4->Add (Vector(197.80551310602755,33.13061904111554, 0.0));
    positionAlloc4->Add (Vector(80.72947945440374,365.4481481413549, 0.0));
    positionAlloc4->Add (Vector(344.6189260412021,264.64009364098996, 0.0));
    positionAlloc4->Add (Vector(121.40715043346302,34.54490717376801, 0.0));
    positionAlloc4->Add (Vector(237.37715514013865,45.823809958520066, 0.0));
    positionAlloc4->Add (Vector(92.23350902677313,162.06921342465895, 0.0));
    positionAlloc4->Add (Vector(360.45570360373426,372.8063646760454, 0.0));
    positionAlloc4->Add (Vector(286.74154594080454,303.06736190085115, 0.0));
    positionAlloc4->Add (Vector(106.41378891131468,106.69733111836935, 0.0));
    positionAlloc4->Add (Vector(397.19222228507095,157.04911700203533, 0.0));
    positionAlloc4->Add (Vector(246.4584276930409,92.81115652087406, 0.0));
    positionAlloc4->Add (Vector(398.5223038726792,289.3464094023543, 0.0));
    positionAlloc4->Add (Vector(80.7016171610107,350.5298970717221, 0.0));
    positionAlloc4->Add (Vector(287.0914966977411,223.00809575063982, 0.0));
    positionAlloc4->Add (Vector(132.79123618572265,164.40988785515515, 0.0));
    positionAlloc4->Add (Vector(15.014416933536223,160.51212797335265, 0.0));
    positionAlloc4->Add (Vector(208.9613523212368,5.455689674651998, 0.0));
    positionAlloc4->Add (Vector(20.478911490906306,196.8521671728236, 0.0));
    positionAlloc4->Add (Vector(232.3569791277988,397.9469645790458, 0.0));
    positionAlloc4->Add (Vector(395.9940579780265,368.0081039578055, 0.0));
    positionAlloc4->Add (Vector(359.06942219933353,311.5295641430474, 0.0));
    positionAlloc4->Add (Vector(19.47282217031634,371.1874265841618, 0.0));
    positionAlloc4->Add (Vector(267.8105441492039,223.1541174044883, 0.0));
    positionAlloc4->Add (Vector(168.4270334277139,287.6003882343705, 0.0));
    positionAlloc4->Add (Vector(215.1563627829993,366.79096662410177, 0.0));
    positionAlloc4->Add (Vector(61.83254993180864,182.74358099136813, 0.0));
    positionAlloc4->Add (Vector(379.84886243531787,30.347491529590354, 0.0));
    positionAlloc4->Add (Vector(130.58628085417334,382.3841037368242, 0.0));
    positionAlloc4->Add (Vector(91.29286418459385,399.68254493430277, 0.0));
    positionAlloc4->Add (Vector(66.87311840561333,110.48190469915218, 0.0));
    positionAlloc4->Add (Vector(92.96506783563689,115.12013964997978, 0.0));
    positionAlloc4->Add (Vector(77.49902465466683,380.1678127539939, 0.0));
    positionAlloc4->Add (Vector(109.96106328971398,226.31554964823218, 0.0));
    positionAlloc4->Add (Vector(343.7238967093441,23.188348038065065, 0.0));
    positionAlloc4->Add (Vector(130.07669193409333,2.6534274653299494, 0.0));
    positionAlloc4->Add (Vector(153.25680346589365,61.87827608052805, 0.0));
    positionAlloc4->Add (Vector(24.73953166413221,363.73838287534136, 0.0));
    positionAlloc4->Add (Vector(32.07834795635005,201.6274626382652, 0.0));
    positionAlloc4->Add (Vector(317.01033557051903,282.731857493004, 0.0));
    positionAlloc4->Add (Vector(157.30223174420254,51.99786128716983, 0.0));
    positionAlloc4->Add (Vector(225.77503260599744,361.6426713105205, 0.0));
    positionAlloc4->Add (Vector(85.30600141399165,186.2919283616967, 0.0));
    positionAlloc4->Add (Vector(43.679025975751486,144.9382110704696, 0.0));
    positionAlloc4->Add (Vector(147.11457204170156,34.49951104642612, 0.0));
    positionAlloc4->Add (Vector(372.92445131350746,221.6102855435214, 0.0));
    positionAlloc4->Add (Vector(202.30781434527182,138.01390196199196, 0.0));
    positionAlloc4->Add (Vector(312.1458540950754,365.13415606413076, 0.0));
    positionAlloc4->Add (Vector(51.34377680681026,79.81911103062905, 0.0));
    positionAlloc4->Add (Vector(133.76122173386565,55.18811606082528, 0.0));
    positionAlloc4->Add (Vector(166.80887433552255,342.59404079886326, 0.0));
    positionAlloc4->Add (Vector(134.1488842440155,91.645760442051, 0.0));
    positionAlloc4->Add (Vector(317.0178846942481,154.3088001055791, 0.0));
    positionAlloc4->Add (Vector(194.10008967058675,285.75473618739085, 0.0));
    positionAlloc4->Add (Vector(311.94889321994384,371.5365171464649, 0.0));
    positionAlloc4->Add (Vector(289.8419619547731,132.66882361228843, 0.0));
    positionAlloc4->Add (Vector(305.27346815668557,209.1839921218568, 0.0));
    positionAlloc4->Add (Vector(356.0028083162321,45.21785281897106, 0.0));
    positionAlloc4->Add (Vector(58.18242505895989,220.67770179259549, 0.0));
    positionAlloc4->Add (Vector(76.66199638655668,258.07227289143617, 0.0));
    positionAlloc4->Add (Vector(348.45640913934363,383.5248943498837, 0.0));
    positionAlloc4->Add (Vector(345.67131259829,130.55761507538358, 0.0));
    positionAlloc4->Add (Vector(32.69318589797323,316.0808875464599, 0.0));
    positionAlloc4->Add (Vector(176.73502809155175,31.406978116277884, 0.0));
    positionAlloc4->Add (Vector(73.37360250445984,399.81574789476986, 0.0));
    positionAlloc4->Add (Vector(270.0083366628459,382.4199981603248, 0.0));
    positionAlloc4->Add (Vector(328.5508529492242,233.20292834800088, 0.0));
    positionAlloc4->Add (Vector(303.1019830039833,282.8931074475024, 0.0));
    positionAlloc4->Add (Vector(256.8990204330507,296.9660863431256, 0.0));
    positionAlloc4->Add (Vector(9.83572081111479,275.6085324311395, 0.0));
    positionAlloc4->Add (Vector(363.82560378069854,218.28956012885, 0.0));
    positionAlloc4->Add (Vector(116.27166142499728,396.9235679920006, 0.0));
    positionAlloc4->Add (Vector(160.81239567867897,246.80986715600457, 0.0));
    positionAlloc4->Add (Vector(295.9690840413117,58.21804367482044, 0.0));
    positionAlloc4->Add (Vector(242.94871279128083,139.85405105701463, 0.0));
    positionAlloc4->Add (Vector(339.33736619966857,334.6834048778346, 0.0));
    positionAlloc4->Add (Vector(264.21771126726094,168.62464374534713, 0.0));
    positionAlloc4->Add (Vector(237.78924267047148,114.45009598791671, 0.0));
    positionAlloc4->Add (Vector(271.05067990425886,329.770965696969, 0.0));
    positionAlloc4->Add (Vector(301.2625450813876,252.92466444790716, 0.0));
    positionAlloc4->Add (Vector(165.42391727714886,28.975939117084025, 0.0));
    positionAlloc4->Add (Vector(93.87375609316044,111.8811101559698, 0.0));
    positionAlloc4->Add (Vector(117.98926457046792,366.0303514329434, 0.0));
    positionAlloc4->Add (Vector(97.77656310449055,242.78537223972424, 0.0));
    positionAlloc4->Add (Vector(255.34738518708906,52.23336852339262, 0.0));
    positionAlloc4->Add (Vector(153.09550075602976,77.59928725084033, 0.0));
    positionAlloc4->Add (Vector(172.64950943155645,12.232800661947563, 0.0));
    positionAlloc4->Add (Vector(275.26464293659274,7.776906300098929, 0.0));
    positionAlloc4->Add (Vector(195.69536334308953,341.54766599770807, 0.0));
    positionAlloc4->Add (Vector(355.96517456093693,92.41701111313549, 0.0));
    positionAlloc4->Add (Vector(114.76822464595587,101.18300711772004, 0.0));
    positionAlloc4->Add (Vector(266.95348466084636,395.2490584007556, 0.0));
    positionAlloc4->Add (Vector(121.39181813393817,163.48869333495855, 0.0));
    positionAlloc4->Add (Vector(315.7201670556003,243.3635146328107, 0.0));
    positionAlloc4->Add (Vector(100.13243252698061,68.5383282026045, 0.0));
    positionAlloc4->Add (Vector(75.42204925495675,273.1935247700054, 0.0));
    positionAlloc4->Add (Vector(14.991771406183485,73.53487248109762, 0.0));
    positionAlloc4->Add (Vector(278.6014806956531,162.70557917322455, 0.0));
    positionAlloc4->Add (Vector(383.181525126382,389.952254185792, 0.0));
    positionAlloc4->Add (Vector(94.67046707871552,356.7725147441736, 0.0));
    positionAlloc4->Add (Vector(14.03146914368767,281.85455559305507, 0.0));
    positionAlloc4->Add (Vector(371.55776536171857,337.2332567494616, 0.0));
    positionAlloc4->Add (Vector(46.385180055656775,21.373517408939023, 0.0));
    positionAlloc4->Add (Vector(155.49411596439091,73.06596005947492, 0.0));
    positionAlloc4->Add (Vector(179.73895206324312,189.61282855561782, 0.0));
    positionAlloc4->Add (Vector(345.17134874857567,273.0102304170917, 0.0));
    positionAlloc4->Add (Vector(163.77053598529835,354.8747014214045, 0.0));
    positionAlloc4->Add (Vector(360.94331445472346,394.80394327208, 0.0));
    positionAlloc4->Add (Vector(171.9167326282855,113.2862029279928, 0.0));
    positionAlloc4->Add (Vector(251.42063751419275,176.80156642415028, 0.0));
    positionAlloc4->Add (Vector(247.47980157238803,342.82873922241185, 0.0));
    positionAlloc4->Add (Vector(34.0762287560231,291.3731030299777, 0.0));
    positionAlloc4->Add (Vector(339.7186687076426,150.5213296508736, 0.0));
    positionAlloc4->Add (Vector(153.28130619195775,395.22193663582135, 0.0));
    positionAlloc4->Add (Vector(179.26920335345562,317.5852515847902, 0.0));
    positionAlloc4->Add (Vector(175.2859019055327,100.55221933522608, 0.0));
    positionAlloc4->Add (Vector(356.4298514019194,313.1291793221329, 0.0));
    positionAlloc4->Add (Vector(214.49147813095055,107.89736805348338, 0.0));
    positionAlloc4->Add (Vector(157.03458629559012,337.6853827839954, 0.0));
    positionAlloc4->Add (Vector(9.925351878238331,126.17501027369, 0.0));
    positionAlloc4->Add (Vector(132.61902425831119,139.782412755854, 0.0));
    positionAlloc4->Add (Vector(386.70976319349097,289.481090628183, 0.0));
    positionAlloc4->Add (Vector(117.26401881714783,0.9151531424958836, 0.0));
    positionAlloc4->Add (Vector(292.36386314553175,367.92985611842994, 0.0));
    positionAlloc4->Add (Vector(282.210303195177,353.2011160758477, 0.0));
    positionAlloc4->Add (Vector(158.9734641362505,328.1309117938466, 0.0));
    positionAlloc4->Add (Vector(264.368164468202,310.9505466494836, 0.0));
    positionAlloc4->Add (Vector(151.97271819745853,12.69656517141624, 0.0));
    positionAlloc4->Add (Vector(17.229243183422362,377.46883100490686, 0.0));
    positionAlloc4->Add (Vector(238.4744798815186,197.08781386635826, 0.0));
    positionAlloc4->Add (Vector(227.2973496502129,337.4114585399348, 0.0));
    positionAlloc4->Add (Vector(368.5030235390294,354.36594144942484, 0.0));
    positionAlloc4->Add (Vector(103.11924999426974,336.93313670717146, 0.0));
    positionAlloc4->Add (Vector(124.591784681897,138.69557411651135, 0.0));
    positionAlloc4->Add (Vector(262.0802389010862,50.82798044691494, 0.0));
    positionAlloc4->Add (Vector(172.76930565348306,375.4528026205248, 0.0));
    positionAlloc4->Add (Vector(198.13415484315198,174.24560644655563, 0.0));
    positionAlloc4->Add (Vector(195.6492583864395,272.035175214977, 0.0));
    positionAlloc4->Add (Vector(33.00373513856285,16.180355250019353, 0.0));
    positionAlloc4->Add (Vector(352.1443352820109,108.87925676994263, 0.0));
    positionAlloc4->Add (Vector(156.17080455476483,55.651713378723635, 0.0));
    positionAlloc4->Add (Vector(217.53069841680158,365.23748357901036, 0.0));
    positionAlloc4->Add (Vector(250.17570323860414,335.4104374345279, 0.0));
    positionAlloc4->Add (Vector(90.9674445165737,378.2616353701092, 0.0));
    positionAlloc4->Add (Vector(214.06662103425825,265.829202085867, 0.0));
    positionAlloc4->Add (Vector(320.13851701584906,85.09016571731442, 0.0));
    positionAlloc4->Add (Vector(313.63420882638746,268.4023609132805, 0.0));
    positionAlloc4->Add (Vector(180.12369679118882,135.3722749202197, 0.0));
    positionAlloc4->Add (Vector(306.891813416507,262.1855424842646, 0.0));
    positionAlloc4->Add (Vector(234.48752324724126,88.25372847045321, 0.0));
    positionAlloc4->Add (Vector(278.5566888228764,140.6043703939945, 0.0));
    positionAlloc4->Add (Vector(350.6072515783404,254.71904093903564, 0.0));
    positionAlloc4->Add (Vector(340.9265133550706,15.512963988813988, 0.0));
    positionAlloc4->Add (Vector(173.00880663362918,278.41755576597285, 0.0));
    positionAlloc4->Add (Vector(161.23951018718387,274.71539310472144, 0.0));
    positionAlloc4->Add (Vector(299.8483534150077,92.19059492890516, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_23.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_23.flowmon", false, false);
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