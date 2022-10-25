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
	  uint16_t numberOfRrhs = 36;
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
    positionAlloc->Add (Vector (356.6666666666667,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (356.6666666666667,10.0, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (313.3333333333333,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (270.0,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (270.0,270.0, 0.0));
    positionAlloc->Add (Vector (270.0,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (270.0,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (270.0,140.0, 0.0));
    positionAlloc->Add (Vector (270.0,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,270.0, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (226.66666666666666,53.333333333333336, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,270.0, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (183.33333333333334,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (140.0,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (140.0,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (140.0,226.66666666666666, 0.0));
    positionAlloc->Add (Vector (140.0,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (140.0,140.0, 0.0));
    positionAlloc->Add (Vector (140.0,96.66666666666667, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,270.0, 0.0));
    positionAlloc->Add (Vector (96.66666666666667,183.33333333333334, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,313.3333333333333, 0.0));
    positionAlloc->Add (Vector (53.333333333333336,270.0, 0.0));
    positionAlloc->Add (Vector (10.0,356.6666666666667, 0.0));
    positionAlloc->Add (Vector (10.0,226.66666666666666, 0.0));
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
    positionAlloc4->Add (Vector(178.82708839463527,97.82979605533045, 0.0));
    positionAlloc4->Add (Vector(210.96727364690486,35.85918443751774, 0.0));
    positionAlloc4->Add (Vector(19.41071756167525,243.60251576169395, 0.0));
    positionAlloc4->Add (Vector(207.77182693977744,284.6393164782778, 0.0));
    positionAlloc4->Add (Vector(114.9400935018574,333.01187489793665, 0.0));
    positionAlloc4->Add (Vector(153.44447820719967,66.23590747765826, 0.0));
    positionAlloc4->Add (Vector(50.09215898129034,222.31135628786447, 0.0));
    positionAlloc4->Add (Vector(337.3082373415243,325.7366326753652, 0.0));
    positionAlloc4->Add (Vector(345.7834579395143,389.11180256634975, 0.0));
    positionAlloc4->Add (Vector(221.54711351392612,88.58919492132715, 0.0));
    positionAlloc4->Add (Vector(267.6622145875208,331.5106545576549, 0.0));
    positionAlloc4->Add (Vector(162.55937709523405,311.63063234581637, 0.0));
    positionAlloc4->Add (Vector(381.244266794021,173.8544449782596, 0.0));
    positionAlloc4->Add (Vector(35.472537902581315,312.4409001950061, 0.0));
    positionAlloc4->Add (Vector(321.5855802094166,20.22073162955431, 0.0));
    positionAlloc4->Add (Vector(15.020752616796784,369.11597110637507, 0.0));
    positionAlloc4->Add (Vector(3.9750036080731466,7.0267861717806746, 0.0));
    positionAlloc4->Add (Vector(307.8899269224673,207.54876779583543, 0.0));
    positionAlloc4->Add (Vector(375.175901749356,261.6768198143624, 0.0));
    positionAlloc4->Add (Vector(322.42973821063595,106.65451663856933, 0.0));
    positionAlloc4->Add (Vector(225.91660431217537,300.0910755763141, 0.0));
    positionAlloc4->Add (Vector(190.0099603715455,391.6118594813548, 0.0));
    positionAlloc4->Add (Vector(45.6033020902439,308.20330550936967, 0.0));
    positionAlloc4->Add (Vector(333.10598873202457,389.9123961162635, 0.0));
    positionAlloc4->Add (Vector(267.3552589260991,165.64468305439271, 0.0));
    positionAlloc4->Add (Vector(360.6896103190103,341.65782106844216, 0.0));
    positionAlloc4->Add (Vector(259.19339396212615,115.25749521671736, 0.0));
    positionAlloc4->Add (Vector(90.67943967250214,318.0175090524559, 0.0));
    positionAlloc4->Add (Vector(285.80419599933464,286.86488423397304, 0.0));
    positionAlloc4->Add (Vector(313.6877534205197,341.4639906008342, 0.0));
    positionAlloc4->Add (Vector(399.43304037157515,167.69159356006801, 0.0));
    positionAlloc4->Add (Vector(114.35884019456068,280.90902871664053, 0.0));
    positionAlloc4->Add (Vector(94.11054096600809,0.36159329444678256, 0.0));
    positionAlloc4->Add (Vector(395.0023166190382,266.73511122658255, 0.0));
    positionAlloc4->Add (Vector(357.22136536443764,86.74777817314285, 0.0));
    positionAlloc4->Add (Vector(19.7696606424103,253.7802193781941, 0.0));
    positionAlloc4->Add (Vector(315.6169998834107,247.25757702439836, 0.0));
    positionAlloc4->Add (Vector(236.12698266701284,106.73775893123967, 0.0));
    positionAlloc4->Add (Vector(273.3237033826712,241.15154213891165, 0.0));
    positionAlloc4->Add (Vector(334.5814341468308,65.35564388794688, 0.0));
    positionAlloc4->Add (Vector(22.39002887370125,346.48810158548315, 0.0));
    positionAlloc4->Add (Vector(314.8208826527136,306.5790762225487, 0.0));
    positionAlloc4->Add (Vector(287.9956882554919,310.5952736664967, 0.0));
    positionAlloc4->Add (Vector(3.4919151653175273,72.36603174960044, 0.0));
    positionAlloc4->Add (Vector(180.25747079670145,239.4773684280491, 0.0));
    positionAlloc4->Add (Vector(277.6104889738295,227.3014155771313, 0.0));
    positionAlloc4->Add (Vector(145.4962730732178,130.41139304427597, 0.0));
    positionAlloc4->Add (Vector(62.812452788666874,359.4322049756163, 0.0));
    positionAlloc4->Add (Vector(120.59613472498407,141.37537186896984, 0.0));
    positionAlloc4->Add (Vector(195.5245888418281,354.29364294590704, 0.0));
    positionAlloc4->Add (Vector(228.2695595824059,341.70919338805396, 0.0));
    positionAlloc4->Add (Vector(284.80994601301273,198.11040626424884, 0.0));
    positionAlloc4->Add (Vector(111.39659909603981,383.82112886168505, 0.0));
    positionAlloc4->Add (Vector(382.9537241644785,377.017431656479, 0.0));
    positionAlloc4->Add (Vector(386.0741923395109,48.8726589581395, 0.0));
    positionAlloc4->Add (Vector(116.44528857294509,389.12473512922855, 0.0));
    positionAlloc4->Add (Vector(317.94575559345645,356.0626770345883, 0.0));
    positionAlloc4->Add (Vector(382.0339441847169,239.7599034682182, 0.0));
    positionAlloc4->Add (Vector(325.5847974707801,10.949389095523854, 0.0));
    positionAlloc4->Add (Vector(261.96167914978713,230.6179081878718, 0.0));
    positionAlloc4->Add (Vector(42.9232920940704,308.51931069798667, 0.0));
    positionAlloc4->Add (Vector(157.77310223551737,99.84714025971928, 0.0));
    positionAlloc4->Add (Vector(102.13362339366951,120.63506177904655, 0.0));
    positionAlloc4->Add (Vector(270.84366774330005,77.87263256101973, 0.0));
    positionAlloc4->Add (Vector(361.43959655961237,128.23915097733592, 0.0));
    positionAlloc4->Add (Vector(271.02297747870637,123.74940993566979, 0.0));
    positionAlloc4->Add (Vector(266.740590833445,330.1598192434098, 0.0));
    positionAlloc4->Add (Vector(364.0367679004932,350.1498623940613, 0.0));
    positionAlloc4->Add (Vector(379.821717635945,114.30163566622423, 0.0));
    positionAlloc4->Add (Vector(170.14389310780072,323.83208751322826, 0.0));
    positionAlloc4->Add (Vector(112.97077797861684,259.26000287973164, 0.0));
    positionAlloc4->Add (Vector(16.932607070299532,84.06267670854892, 0.0));
    positionAlloc4->Add (Vector(125.90641906721922,43.44079092228128, 0.0));
    positionAlloc4->Add (Vector(156.41673087775158,177.43282816783267, 0.0));
    positionAlloc4->Add (Vector(42.775891609309326,113.9026442790374, 0.0));
    positionAlloc4->Add (Vector(241.1659079546266,326.8284086745988, 0.0));
    positionAlloc4->Add (Vector(321.66272081930265,165.35193358623005, 0.0));
    positionAlloc4->Add (Vector(372.4776633393441,37.108348970004855, 0.0));
    positionAlloc4->Add (Vector(171.3576303036129,225.91944927495237, 0.0));
    positionAlloc4->Add (Vector(83.80918989610731,167.5010432583315, 0.0));
    positionAlloc4->Add (Vector(75.46952418534593,109.03713523481619, 0.0));
    positionAlloc4->Add (Vector(114.52094646998154,319.6735524098586, 0.0));
    positionAlloc4->Add (Vector(65.28214085774007,290.61861871649984, 0.0));
    positionAlloc4->Add (Vector(114.44754883197655,31.00798211472324, 0.0));
    positionAlloc4->Add (Vector(254.35929912482834,47.720133622146136, 0.0));
    positionAlloc4->Add (Vector(26.311935477245285,233.99525464109075, 0.0));
    positionAlloc4->Add (Vector(327.4425221283349,36.28379460377218, 0.0));
    positionAlloc4->Add (Vector(247.3673320356113,304.2927038350585, 0.0));
    positionAlloc4->Add (Vector(348.7264932780068,276.6764861055089, 0.0));
    positionAlloc4->Add (Vector(15.641082548579188,348.2353219256247, 0.0));
    positionAlloc4->Add (Vector(15.7972761507152,119.64279441524886, 0.0));
    positionAlloc4->Add (Vector(169.60196901063065,50.362218672096226, 0.0));
    positionAlloc4->Add (Vector(185.58626492812812,272.58874375633604, 0.0));
    positionAlloc4->Add (Vector(160.53868175065733,190.86794988911092, 0.0));
    positionAlloc4->Add (Vector(257.4960418399878,174.19059492232637, 0.0));
    positionAlloc4->Add (Vector(136.90991669176657,9.170916843903498, 0.0));
    positionAlloc4->Add (Vector(209.36488353510896,287.1678177993293, 0.0));
    positionAlloc4->Add (Vector(140.06773126569195,322.4895049328959, 0.0));
    positionAlloc4->Add (Vector(290.55391269250373,246.07704913270513, 0.0));
    positionAlloc4->Add (Vector(263.08243117853107,3.7410873358481034, 0.0));
    positionAlloc4->Add (Vector(255.45701425625893,131.19888106851195, 0.0));
    positionAlloc4->Add (Vector(346.166080026438,316.39491093203713, 0.0));
    positionAlloc4->Add (Vector(352.8369500712951,28.958400284533916, 0.0));
    positionAlloc4->Add (Vector(13.835243453798718,239.62733945597438, 0.0));
    positionAlloc4->Add (Vector(36.96840794445908,287.68366546169426, 0.0));
    positionAlloc4->Add (Vector(97.87821998167962,258.53253997033926, 0.0));
    positionAlloc4->Add (Vector(346.8498579146111,47.82424391486187, 0.0));
    positionAlloc4->Add (Vector(136.1786940614743,145.04630755448596, 0.0));
    positionAlloc4->Add (Vector(123.44920842717855,217.8291236143448, 0.0));
    positionAlloc4->Add (Vector(132.760417429547,155.54441957388008, 0.0));
    positionAlloc4->Add (Vector(96.48620177668445,342.32517350330954, 0.0));
    positionAlloc4->Add (Vector(124.89331520052195,310.3414231719877, 0.0));
    positionAlloc4->Add (Vector(246.4752655122707,392.4188028579965, 0.0));
    positionAlloc4->Add (Vector(85.6411266403839,308.241931240716, 0.0));
    positionAlloc4->Add (Vector(146.07209667566056,98.20344526555327, 0.0));
    positionAlloc4->Add (Vector(41.34221490635382,91.48580664437675, 0.0));
    positionAlloc4->Add (Vector(262.35157019255075,274.21227884493146, 0.0));
    positionAlloc4->Add (Vector(156.15888471837957,357.97428909078366, 0.0));
    positionAlloc4->Add (Vector(210.87052465104793,53.24128519614377, 0.0));
    positionAlloc4->Add (Vector(394.0288604832955,182.5726975843691, 0.0));
    positionAlloc4->Add (Vector(135.49317816840065,310.20218087465275, 0.0));
    positionAlloc4->Add (Vector(371.3773803307968,199.699968844482, 0.0));
    positionAlloc4->Add (Vector(133.24280703082678,160.64657342770977, 0.0));
    positionAlloc4->Add (Vector(215.2078469234752,351.61141291351623, 0.0));
    positionAlloc4->Add (Vector(139.21851340925272,272.0942451347279, 0.0));
    positionAlloc4->Add (Vector(272.05982967776555,32.1161320756445, 0.0));
    positionAlloc4->Add (Vector(18.88863211630447,311.64427107901867, 0.0));
    positionAlloc4->Add (Vector(207.97229381793656,139.93558202012704, 0.0));
    positionAlloc4->Add (Vector(370.2675038176553,57.03862253999175, 0.0));
    positionAlloc4->Add (Vector(156.22690433716494,356.41108121701603, 0.0));
    positionAlloc4->Add (Vector(150.12820752036217,228.74183088528275, 0.0));
    positionAlloc4->Add (Vector(63.49077015427533,129.92764501927488, 0.0));
    positionAlloc4->Add (Vector(211.86234296912798,226.23679543369607, 0.0));
    positionAlloc4->Add (Vector(213.83130782089745,97.47517466876646, 0.0));
    positionAlloc4->Add (Vector(65.6131302413557,399.497511192512, 0.0));
    positionAlloc4->Add (Vector(144.6724847420288,396.6697636153209, 0.0));
    positionAlloc4->Add (Vector(290.534822465052,122.13077736996128, 0.0));
    positionAlloc4->Add (Vector(68.07438543579711,59.66929585771177, 0.0));
    positionAlloc4->Add (Vector(378.0219957913276,84.53082251421216, 0.0));
    positionAlloc4->Add (Vector(125.95228478791265,234.17761508571306, 0.0));
    positionAlloc4->Add (Vector(393.2467759545783,349.1216171634683, 0.0));
    positionAlloc4->Add (Vector(190.84715998314934,112.72766714825724, 0.0));
    positionAlloc4->Add (Vector(152.83751448508173,326.12668802567686, 0.0));
    positionAlloc4->Add (Vector(93.13070460307635,288.5478430607377, 0.0));
    positionAlloc4->Add (Vector(197.50723339494533,273.0583355332912, 0.0));
    positionAlloc4->Add (Vector(54.253448409144056,203.11080966392447, 0.0));
    positionAlloc4->Add (Vector(32.26732299926658,384.3795064251069, 0.0));
    positionAlloc4->Add (Vector(172.1898889521988,202.10695953586065, 0.0));
    positionAlloc4->Add (Vector(94.94287574368067,207.6392284547811, 0.0));
    positionAlloc4->Add (Vector(180.59789980141417,337.7027471841468, 0.0));
    positionAlloc4->Add (Vector(133.3511598515765,349.9230842414555, 0.0));
    positionAlloc4->Add (Vector(352.76506565669644,95.33068186342648, 0.0));
    positionAlloc4->Add (Vector(368.9096568459052,381.1146467506846, 0.0));
    positionAlloc4->Add (Vector(23.805758156794266,196.11647859828184, 0.0));
    positionAlloc4->Add (Vector(138.98970595863744,185.27710482527388, 0.0));
    positionAlloc4->Add (Vector(373.91864529507495,204.90998134773176, 0.0));
    positionAlloc4->Add (Vector(149.51636616464833,310.32866286565957, 0.0));
    positionAlloc4->Add (Vector(201.5810615590856,137.69050221605806, 0.0));
    positionAlloc4->Add (Vector(219.3392751449666,29.193268373597768, 0.0));
    positionAlloc4->Add (Vector(235.05125658416102,98.44112853601939, 0.0));
    positionAlloc4->Add (Vector(230.9420788949706,307.3885504249055, 0.0));
    positionAlloc4->Add (Vector(249.2505385338705,283.73121008893304, 0.0));
    positionAlloc4->Add (Vector(377.80779004904764,3.8169323104719766, 0.0));
    positionAlloc4->Add (Vector(11.155890965453619,265.53750119901525, 0.0));
    positionAlloc4->Add (Vector(65.88995658643006,69.26005944739093, 0.0));
    positionAlloc4->Add (Vector(347.9217705591246,213.01500704435625, 0.0));
    positionAlloc4->Add (Vector(30.014590774514893,265.41460484511566, 0.0));
    positionAlloc4->Add (Vector(9.629476417679461,222.9478360092473, 0.0));
    positionAlloc4->Add (Vector(220.29800727098046,317.8015875089261, 0.0));
    positionAlloc4->Add (Vector(224.39282361535598,135.1552674632433, 0.0));
    positionAlloc4->Add (Vector(8.37999948184076,152.34538129602834, 0.0));
    positionAlloc4->Add (Vector(320.83604674858026,387.7266214866506, 0.0));
    positionAlloc4->Add (Vector(36.38835217619527,188.64637008642964, 0.0));
    positionAlloc4->Add (Vector(12.85761674528314,324.39342232455755, 0.0));
    positionAlloc4->Add (Vector(108.37115749047031,297.4945443427995, 0.0));
    positionAlloc4->Add (Vector(162.85236487968527,206.82775738953683, 0.0));
    positionAlloc4->Add (Vector(212.63362739764807,34.33018749139487, 0.0));
    positionAlloc4->Add (Vector(278.9845780611114,329.84406304220397, 0.0));
    positionAlloc4->Add (Vector(42.059548183224706,11.044421188848075, 0.0));
    positionAlloc4->Add (Vector(306.99654723825256,133.2458630365021, 0.0));
    positionAlloc4->Add (Vector(200.97703738514824,362.8738032626952, 0.0));
    positionAlloc4->Add (Vector(185.68920915282305,315.4622366122493, 0.0));
    positionAlloc4->Add (Vector(281.9852095364266,25.75841284723266, 0.0));
    positionAlloc4->Add (Vector(382.9344001866784,189.78713121530672, 0.0));
    positionAlloc4->Add (Vector(207.940592150626,327.1824426000924, 0.0));
    positionAlloc4->Add (Vector(313.7397465297491,152.53605301856373, 0.0));
    positionAlloc4->Add (Vector(147.01987043954094,334.572489251156, 0.0));
    positionAlloc4->Add (Vector(75.09985025456069,313.05448567842666, 0.0));
    positionAlloc4->Add (Vector(329.166614982119,200.56755359674887, 0.0));
    positionAlloc4->Add (Vector(10.302713066113478,276.4633128639987, 0.0));
    positionAlloc4->Add (Vector(317.4466834374533,323.04523989600125, 0.0));
    positionAlloc4->Add (Vector(129.88925947208853,307.26428553648157, 0.0));
    positionAlloc4->Add (Vector(328.52972496941584,261.4085012152724, 0.0));
    positionAlloc4->Add (Vector(350.34461297180127,62.0351316672302, 0.0));
    positionAlloc4->Add (Vector(188.99145582451834,296.14381908526593, 0.0));
    positionAlloc4->Add (Vector(325.1781456191643,309.7851249048352, 0.0));
    positionAlloc4->Add (Vector(147.56217586927303,297.35590242603536, 0.0));
    positionAlloc4->Add (Vector(279.80705639315534,56.60516934049986, 0.0));
    positionAlloc4->Add (Vector(374.9984487366,270.542258783687, 0.0));
    positionAlloc4->Add (Vector(85.25524496136798,196.96315727015673, 0.0));
    positionAlloc4->Add (Vector(181.9501945206894,215.3015274121211, 0.0));
    positionAlloc4->Add (Vector(110.00196766389632,180.13397997220358, 0.0));
    positionAlloc4->Add (Vector(85.6809865512397,284.3219483467037, 0.0));
    positionAlloc4->Add (Vector(102.96487954132449,276.91179711409217, 0.0));
    positionAlloc4->Add (Vector(134.85179907798815,167.48029421993635, 0.0));
    positionAlloc4->Add (Vector(289.21209885088484,209.03277707878863, 0.0));
    positionAlloc4->Add (Vector(369.73966426446896,199.33407550598056, 0.0));
    positionAlloc4->Add (Vector(37.672613952080745,214.09822137955138, 0.0));
    positionAlloc4->Add (Vector(372.0052641711136,290.5432317099258, 0.0));
    positionAlloc4->Add (Vector(134.76526684328852,309.3549363792625, 0.0));
    positionAlloc4->Add (Vector(235.45028313350335,140.87345856528395, 0.0));
    positionAlloc4->Add (Vector(199.8714017988894,208.397770720475, 0.0));
    positionAlloc4->Add (Vector(61.584720943775515,203.21998791765475, 0.0));
    positionAlloc4->Add (Vector(128.36093605565813,156.15814073385206, 0.0));
    positionAlloc4->Add (Vector(339.7759120279629,115.8594452216708, 0.0));
    positionAlloc4->Add (Vector(321.7744603690833,184.8004823699484, 0.0));
    positionAlloc4->Add (Vector(344.80510273164526,56.042332417444825, 0.0));
    positionAlloc4->Add (Vector(182.38743861561352,182.5497172883122, 0.0));
    positionAlloc4->Add (Vector(305.4231491944633,394.48088636541144, 0.0));
    positionAlloc4->Add (Vector(306.40317352176044,170.225258535442, 0.0));
    positionAlloc4->Add (Vector(173.67140213724363,103.63438273591919, 0.0));
    positionAlloc4->Add (Vector(53.627807591426716,222.83101052901208, 0.0));
    positionAlloc4->Add (Vector(255.65717493523042,257.80620004126047, 0.0));
    positionAlloc4->Add (Vector(320.4449577889648,343.02503575295987, 0.0));
    positionAlloc4->Add (Vector(355.5061997032856,357.05811825260736, 0.0));
    positionAlloc4->Add (Vector(47.92107679998963,312.2303463765513, 0.0));
    positionAlloc4->Add (Vector(380.97862479740155,287.02001879665585, 0.0));
    positionAlloc4->Add (Vector(34.3872909672903,382.5840994642498, 0.0));
    positionAlloc4->Add (Vector(188.4169082751689,365.62101048023305, 0.0));
    positionAlloc4->Add (Vector(31.998289897262254,63.78103518073712, 0.0));
    positionAlloc4->Add (Vector(387.61920743811453,292.2478767071511, 0.0));
    positionAlloc4->Add (Vector(221.13789763586107,391.9999879082381, 0.0));
    positionAlloc4->Add (Vector(333.61691215500275,369.01044804354524, 0.0));
    positionAlloc4->Add (Vector(209.60360435572247,56.21004553794946, 0.0));
    positionAlloc4->Add (Vector(193.55399190961694,118.86119308820224, 0.0));
    positionAlloc4->Add (Vector(293.30378487433745,55.225375266789676, 0.0));
    positionAlloc4->Add (Vector(119.16851401473525,98.74504857763475, 0.0));
    positionAlloc4->Add (Vector(375.1821239509082,87.0528205929983, 0.0));
    positionAlloc4->Add (Vector(274.11097510929847,145.568964528408, 0.0));
    positionAlloc4->Add (Vector(36.50514369303232,261.20658689013567, 0.0));
    positionAlloc4->Add (Vector(59.967391627217914,287.4677070893622, 0.0));
    positionAlloc4->Add (Vector(70.31056331569756,297.5554979636843, 0.0));
    positionAlloc4->Add (Vector(205.82060765820907,109.76382332255623, 0.0));
    positionAlloc4->Add (Vector(290.46122608375236,33.165182352741375, 0.0));
    positionAlloc4->Add (Vector(144.03982266377565,312.53721130913885, 0.0));
    positionAlloc4->Add (Vector(162.18002497603698,96.14060003242355, 0.0));
    positionAlloc4->Add (Vector(361.42863424479935,12.119371894556208, 0.0));
    positionAlloc4->Add (Vector(138.59173358636906,0.8523332331334732, 0.0));
    positionAlloc4->Add (Vector(121.16104127183154,81.36386875312836, 0.0));
    positionAlloc4->Add (Vector(131.03747200428245,88.3342733367293, 0.0));
    positionAlloc4->Add (Vector(22.237922771950824,117.82821739693956, 0.0));
    positionAlloc4->Add (Vector(35.61874059306467,271.7604647004853, 0.0));
    positionAlloc4->Add (Vector(220.3546399246203,298.47138759018964, 0.0));
    positionAlloc4->Add (Vector(1.116774515924579,38.242480741502625, 0.0));
    positionAlloc4->Add (Vector(100.26449703967857,205.65913680685327, 0.0));
    positionAlloc4->Add (Vector(22.641147615557912,181.24818217556697, 0.0));
    positionAlloc4->Add (Vector(244.2748684519636,27.975262307605988, 0.0));
    positionAlloc4->Add (Vector(298.8924119184075,197.87180238670237, 0.0));
    positionAlloc4->Add (Vector(27.189465681118065,162.83916482112554, 0.0));
    positionAlloc4->Add (Vector(268.49287940357607,25.781174881733104, 0.0));
    positionAlloc4->Add (Vector(379.75856465592176,190.73357298581914, 0.0));
    positionAlloc4->Add (Vector(92.34204405659435,340.0328098934931, 0.0));
    positionAlloc4->Add (Vector(377.47528761985177,354.76987552071836, 0.0));
    positionAlloc4->Add (Vector(165.55898335415776,130.09583575423048, 0.0));
    positionAlloc4->Add (Vector(217.36264237464334,391.68478787803036, 0.0));
    positionAlloc4->Add (Vector(159.9950680213761,231.2890847006743, 0.0));
    positionAlloc4->Add (Vector(131.94333886565417,237.6058933910847, 0.0));
    positionAlloc4->Add (Vector(206.83258999243864,272.33870657681115, 0.0));
    positionAlloc4->Add (Vector(352.6236314475319,87.91143800927857, 0.0));
    positionAlloc4->Add (Vector(108.97124279346625,71.838146619333, 0.0));
    positionAlloc4->Add (Vector(78.59313757017614,171.3833552226335, 0.0));
    positionAlloc4->Add (Vector(335.7040400141174,214.68819589644013, 0.0));
    positionAlloc4->Add (Vector(112.20458244544677,200.01119303681688, 0.0));
    positionAlloc4->Add (Vector(348.46953886880146,148.97908646691005, 0.0));
    positionAlloc4->Add (Vector(157.3494184510849,104.60156377645822, 0.0));
    positionAlloc4->Add (Vector(153.67651587986947,351.6458759000422, 0.0));
    positionAlloc4->Add (Vector(356.1557818778409,12.484036863911951, 0.0));
    positionAlloc4->Add (Vector(369.5168764031456,37.118045918869754, 0.0));
    positionAlloc4->Add (Vector(270.7350498900775,136.3438644643598, 0.0));
    positionAlloc4->Add (Vector(249.6889856457155,165.05381030241148, 0.0));
    positionAlloc4->Add (Vector(75.34624723468637,2.8703562636303914, 0.0));
    positionAlloc4->Add (Vector(143.71303484435197,360.4018417472187, 0.0));
    positionAlloc4->Add (Vector(219.45716428719336,44.03216536210493, 0.0));
    positionAlloc4->Add (Vector(182.3945910170449,282.88872752836227, 0.0));
    positionAlloc4->Add (Vector(15.495655621777615,85.64226174705345, 0.0));
    positionAlloc4->Add (Vector(157.38783540340228,201.8995762990426, 0.0));
    positionAlloc4->Add (Vector(82.58455261108107,349.53222871461577, 0.0));
    positionAlloc4->Add (Vector(93.028702096604,373.6659367219584, 0.0));
    positionAlloc4->Add (Vector(260.6205397206345,65.0531393037141, 0.0));
    positionAlloc4->Add (Vector(107.52647755811884,354.6726299471667, 0.0));
    positionAlloc4->Add (Vector(268.1166199126321,265.03812520090355, 0.0));
    positionAlloc4->Add (Vector(131.0881719718111,262.76219260807403, 0.0));
    positionAlloc4->Add (Vector(89.78974791066304,285.2461348629413, 0.0));
    positionAlloc4->Add (Vector(246.86788696604967,352.91641858206435, 0.0));
    positionAlloc4->Add (Vector(321.66367157248305,89.27762661187218, 0.0));
    positionAlloc4->Add (Vector(146.8928284846271,110.04931789998741, 0.0));
    positionAlloc4->Add (Vector(330.2983855725778,49.0594615735628, 0.0));
    positionAlloc4->Add (Vector(219.72533782085475,115.69550380673093, 0.0));
    positionAlloc4->Add (Vector(276.3627196626888,202.66356895606302, 0.0));
    positionAlloc4->Add (Vector(360.65371995017773,187.85679186745475, 0.0));
    positionAlloc4->Add (Vector(258.09582045380006,198.02526401915378, 0.0));
    positionAlloc4->Add (Vector(125.89705266122215,352.81766236244243, 0.0));
    positionAlloc4->Add (Vector(177.22308918244985,195.02789396244978, 0.0));
    positionAlloc4->Add (Vector(324.72926434658393,225.52933106209983, 0.0));
    positionAlloc4->Add (Vector(277.09149054031445,146.6634611151473, 0.0));
    positionAlloc4->Add (Vector(265.7917175988139,178.54801563451997, 0.0));
    positionAlloc4->Add (Vector(367.6394903410046,151.07727083632082, 0.0));
    positionAlloc4->Add (Vector(281.42380150824533,109.44905965468932, 0.0));
    positionAlloc4->Add (Vector(339.79143762892136,193.1965814216, 0.0));
    positionAlloc4->Add (Vector(384.89632614972004,156.27167633745088, 0.0));
    positionAlloc4->Add (Vector(11.528839139024516,1.8632408607115014, 0.0));
    positionAlloc4->Add (Vector(343.8200316845633,7.198161788472968, 0.0));
    positionAlloc4->Add (Vector(64.95420900061073,159.7201930079295, 0.0));
    positionAlloc4->Add (Vector(161.05343223781844,379.4401391753073, 0.0));
    positionAlloc4->Add (Vector(240.80044933802856,272.19530673994467, 0.0));
    positionAlloc4->Add (Vector(153.4930770062951,322.4207442699809, 0.0));
    positionAlloc4->Add (Vector(7.482325473675155,234.32837427074156, 0.0));
    positionAlloc4->Add (Vector(238.89796154138443,206.82191769600036, 0.0));
    positionAlloc4->Add (Vector(325.14563504353,244.52836977348147, 0.0));
    positionAlloc4->Add (Vector(211.83865083937735,187.02095000498838, 0.0));
    positionAlloc4->Add (Vector(219.6326200604228,147.90783587668724, 0.0));
    positionAlloc4->Add (Vector(5.42707997273153,93.97151211862833, 0.0));
    positionAlloc4->Add (Vector(287.015690690316,341.6067677361293, 0.0));
    positionAlloc4->Add (Vector(274.3810479935137,51.40606173850699, 0.0));
    positionAlloc4->Add (Vector(198.1150904455484,220.4495039835586, 0.0));
    positionAlloc4->Add (Vector(81.01652956691066,213.252010126486, 0.0));
    positionAlloc4->Add (Vector(235.3098191363639,115.25969045903604, 0.0));
    positionAlloc4->Add (Vector(31.088731191568098,59.35748440305204, 0.0));
    positionAlloc4->Add (Vector(143.41375318194244,47.543929202620205, 0.0));
    positionAlloc4->Add (Vector(126.44909813253125,14.064293714987786, 0.0));
    positionAlloc4->Add (Vector(288.369911629821,282.36608806953336, 0.0));
    positionAlloc4->Add (Vector(32.12255467924945,394.0011996635549, 0.0));
    positionAlloc4->Add (Vector(168.6917383784973,320.55875780535297, 0.0));
    positionAlloc4->Add (Vector(302.00320475355164,344.15968316984714, 0.0));
    positionAlloc4->Add (Vector(94.53990459909383,27.223065818212966, 0.0));
    positionAlloc4->Add (Vector(350.27958310420684,41.24810751432544, 0.0));
    positionAlloc4->Add (Vector(168.0091303696607,57.77688038301587, 0.0));
    positionAlloc4->Add (Vector(22.45595023030078,111.62284342252171, 0.0));
    positionAlloc4->Add (Vector(10.025676318845989,103.0186615726369, 0.0));
    positionAlloc4->Add (Vector(338.36432566728314,318.65068358098415, 0.0));
    positionAlloc4->Add (Vector(243.47840416971397,71.9459572457227, 0.0));
    positionAlloc4->Add (Vector(341.8291247740852,45.805589888368736, 0.0));
    positionAlloc4->Add (Vector(291.99830363364407,246.58005933056327, 0.0));
    positionAlloc4->Add (Vector(231.17955651840617,373.62124325022575, 0.0));
    positionAlloc4->Add (Vector(120.84216667488796,208.88806718504642, 0.0));
    positionAlloc4->Add (Vector(141.3764173034021,15.070418131522922, 0.0));
    positionAlloc4->Add (Vector(263.87988401418016,160.8252698962218, 0.0));
    positionAlloc4->Add (Vector(263.5553359531599,46.698191017594496, 0.0));
    positionAlloc4->Add (Vector(171.00615648207773,137.68939804376092, 0.0));
    positionAlloc4->Add (Vector(108.46154532268537,36.44418421811197, 0.0));
    positionAlloc4->Add (Vector(110.35412714055055,279.02859426970116, 0.0));
    positionAlloc4->Add (Vector(81.56713941116709,361.76777308313274, 0.0));
    positionAlloc4->Add (Vector(164.72895959176057,192.5812959644805, 0.0));
    positionAlloc4->Add (Vector(358.9647259400016,144.27907245417236, 0.0));
    positionAlloc4->Add (Vector(229.34330070853088,89.49108117216835, 0.0));
    positionAlloc4->Add (Vector(274.8758283512388,298.8069123418325, 0.0));
    positionAlloc4->Add (Vector(113.3009011073383,176.53009033462249, 0.0));
    positionAlloc4->Add (Vector(363.44412769618685,382.80925436461916, 0.0));
    positionAlloc4->Add (Vector(215.26573054412498,283.483360265756, 0.0));
    positionAlloc4->Add (Vector(64.73094295855088,298.7855110661274, 0.0));
    positionAlloc4->Add (Vector(166.1844354376639,329.35606061286546, 0.0));
    positionAlloc4->Add (Vector(330.7809506524287,254.2884087854671, 0.0));
    positionAlloc4->Add (Vector(146.81507913020408,51.23365546670202, 0.0));
    positionAlloc4->Add (Vector(64.12449416880813,349.79772911547184, 0.0));
    positionAlloc4->Add (Vector(257.0673487081337,119.24051450081498, 0.0));
    positionAlloc4->Add (Vector(364.0927550956188,12.051592935530175, 0.0));
    positionAlloc4->Add (Vector(355.59832056784,315.93583366292427, 0.0));
    positionAlloc4->Add (Vector(330.372669897058,226.66404552852887, 0.0));
    positionAlloc4->Add (Vector(54.41379322403294,313.66169991373005, 0.0));
    positionAlloc4->Add (Vector(343.62293872832794,83.74929419788381, 0.0));
    positionAlloc4->Add (Vector(90.51516043315573,286.7463530862077, 0.0));
    positionAlloc4->Add (Vector(55.16832049354803,213.84904808553102, 0.0));
    positionAlloc4->Add (Vector(2.023454043073336,375.6020154854961, 0.0));
    positionAlloc4->Add (Vector(23.71590494546045,220.5777342596945, 0.0));
    positionAlloc4->Add (Vector(275.9683343164757,137.88254909255855, 0.0));
    positionAlloc4->Add (Vector(269.2649164758957,191.6046782564897, 0.0));
    positionAlloc4->Add (Vector(275.12455055327285,23.893017822654585, 0.0));
    positionAlloc4->Add (Vector(280.8825077769028,332.67970842585714, 0.0));
    positionAlloc4->Add (Vector(79.23747934818954,345.30437013710457, 0.0));
    positionAlloc4->Add (Vector(307.06568935421626,96.1935215063463, 0.0));
    positionAlloc4->Add (Vector(214.69415049229994,29.88451195201156, 0.0));
    positionAlloc4->Add (Vector(197.5853420133693,188.1368108959732, 0.0));
    positionAlloc4->Add (Vector(296.0103585976754,95.46108511867182, 0.0));
    positionAlloc4->Add (Vector(215.82557286869033,383.41036931086654, 0.0));
    positionAlloc4->Add (Vector(303.5066236573808,382.274897441894, 0.0));
    positionAlloc4->Add (Vector(123.15847260291095,151.85942169440182, 0.0));
    positionAlloc4->Add (Vector(235.33792139347233,123.46736162986222, 0.0));
    positionAlloc4->Add (Vector(255.55985270344777,114.19027243977018, 0.0));
    positionAlloc4->Add (Vector(132.16847131215292,341.28544896565563, 0.0));
    positionAlloc4->Add (Vector(269.70265403071375,396.78969557313366, 0.0));
    positionAlloc4->Add (Vector(141.42350927980547,351.0308431456626, 0.0));
    positionAlloc4->Add (Vector(53.32969373492147,250.2892503145147, 0.0));
    positionAlloc4->Add (Vector(29.404848929548955,58.89102648684235, 0.0));
    positionAlloc4->Add (Vector(374.2556108329021,396.8159750150526, 0.0));
    positionAlloc4->Add (Vector(377.99774649266993,208.8433237278918, 0.0));
    positionAlloc4->Add (Vector(212.1669022704132,380.80407330493136, 0.0));
    positionAlloc4->Add (Vector(227.01925679921433,288.31385043180165, 0.0));
    positionAlloc4->Add (Vector(340.01117360124215,338.11167027559674, 0.0));
    positionAlloc4->Add (Vector(107.89930006956716,226.6091032159265, 0.0));
    positionAlloc4->Add (Vector(376.4565928252321,339.8128144648095, 0.0));
    positionAlloc4->Add (Vector(266.19122036857254,234.4194603367194, 0.0));
    positionAlloc4->Add (Vector(220.55097714215327,207.42597455782303, 0.0));
    positionAlloc4->Add (Vector(164.27042129284263,264.9422145467262, 0.0));
    positionAlloc4->Add (Vector(33.98347361757308,97.27908616012519, 0.0));
    positionAlloc4->Add (Vector(338.60007570809205,331.4787093005258, 0.0));
    positionAlloc4->Add (Vector(248.3639201477696,42.304225097885826, 0.0));
    positionAlloc4->Add (Vector(297.50886038149474,109.54388537000072, 0.0));
    positionAlloc4->Add (Vector(170.99876927550562,219.7345589601428, 0.0));
    positionAlloc4->Add (Vector(336.12820333611586,28.65814312064665, 0.0));
    positionAlloc4->Add (Vector(278.20534566283703,108.24337255874772, 0.0));
    positionAlloc4->Add (Vector(134.4821571378054,328.6204967328387, 0.0));
    positionAlloc4->Add (Vector(166.47727271874072,61.705648350456556, 0.0));
    positionAlloc4->Add (Vector(17.21424705512846,332.8321114111239, 0.0));
    positionAlloc4->Add (Vector(370.27292698713404,280.4524924274763, 0.0));
    positionAlloc4->Add (Vector(360.51099139685874,105.93475504491754, 0.0));
    positionAlloc4->Add (Vector(188.64682670292635,252.074366803444, 0.0));
    positionAlloc4->Add (Vector(308.24333010295936,145.06029594173984, 0.0));
    positionAlloc4->Add (Vector(98.18462725862803,276.32344751783927, 0.0));
    positionAlloc4->Add (Vector(178.93580801498982,251.75729569688437, 0.0));
    positionAlloc4->Add (Vector(314.30888994268514,225.40529475290361, 0.0));
    positionAlloc4->Add (Vector(379.41372511418285,118.43809379754254, 0.0));
    positionAlloc4->Add (Vector(103.24682424476133,139.2992257839869, 0.0));
    positionAlloc4->Add (Vector(70.95817612186868,187.78819991466693, 0.0));
    positionAlloc4->Add (Vector(312.3461760998242,210.74809791350458, 0.0));
    positionAlloc4->Add (Vector(155.81668497194633,188.8394610891615, 0.0));
    positionAlloc4->Add (Vector(30.572125826813323,385.6336129673248, 0.0));
    positionAlloc4->Add (Vector(375.5293463459567,236.9080122755159, 0.0));
    positionAlloc4->Add (Vector(366.48048928330434,348.9668263027767, 0.0));
    positionAlloc4->Add (Vector(55.44626154977323,58.85127645809902, 0.0));
    positionAlloc4->Add (Vector(202.73702138371,311.046215254241, 0.0));
    positionAlloc4->Add (Vector(69.33838833148114,31.106832243125027, 0.0));
    positionAlloc4->Add (Vector(30.126457923058368,145.81250744931694, 0.0));
    positionAlloc4->Add (Vector(360.64154747546667,283.99339555235224, 0.0));
    positionAlloc4->Add (Vector(52.51035562240176,134.37324278862616, 0.0));
    positionAlloc4->Add (Vector(393.8650912681001,241.04480308086974, 0.0));
    positionAlloc4->Add (Vector(204.46114357369015,392.57660369942226, 0.0));
    positionAlloc4->Add (Vector(380.28124859423974,254.67729943477053, 0.0));
    positionAlloc4->Add (Vector(140.1547447874192,322.7852151810128, 0.0));
    positionAlloc4->Add (Vector(55.96343181174341,376.3836252466931, 0.0));
    positionAlloc4->Add (Vector(234.37597950125863,39.0897163210155, 0.0));
    positionAlloc4->Add (Vector(30.61551665654263,373.23980159608436, 0.0));
    positionAlloc4->Add (Vector(60.29552497412496,215.69702375463834, 0.0));
    positionAlloc4->Add (Vector(160.50960196819918,249.62587700435637, 0.0));
    positionAlloc4->Add (Vector(133.47211053158983,334.7101475079629, 0.0));
    positionAlloc4->Add (Vector(333.1134866703215,352.0091825086185, 0.0));
    positionAlloc4->Add (Vector(153.81760070537362,14.05265525730095, 0.0));
    positionAlloc4->Add (Vector(168.77410583216093,266.9290313413575, 0.0));
    positionAlloc4->Add (Vector(217.39556501613552,65.60804973899197, 0.0));
    positionAlloc4->Add (Vector(182.7101594980207,164.6639623621245, 0.0));
    positionAlloc4->Add (Vector(50.97337921303726,98.37325320424198, 0.0));
    positionAlloc4->Add (Vector(49.128309069909726,157.72379973920582, 0.0));
    positionAlloc4->Add (Vector(289.58798115101996,166.5150562945232, 0.0));
    positionAlloc4->Add (Vector(364.0516021884047,126.59901850975177, 0.0));
    positionAlloc4->Add (Vector(167.4735047101839,217.65835554883623, 0.0));
    positionAlloc4->Add (Vector(67.90396377617674,363.3442424771748, 0.0));
    positionAlloc4->Add (Vector(40.97129251057399,358.47235301872826, 0.0));
    positionAlloc4->Add (Vector(280.46715510502884,357.45486216996494, 0.0));
    positionAlloc4->Add (Vector(134.18352729074928,228.18751420087912, 0.0));
    positionAlloc4->Add (Vector(269.5706802386814,332.3960996553068, 0.0));
    positionAlloc4->Add (Vector(236.0043241662024,2.937730739035471, 0.0));
    positionAlloc4->Add (Vector(7.761166617695681,69.33437170483359, 0.0));
    positionAlloc4->Add (Vector(14.97685637328745,84.64425311948798, 0.0));
    positionAlloc4->Add (Vector(274.1325953344773,338.15982627574607, 0.0));
    positionAlloc4->Add (Vector(5.01616499943407,326.84205434895273, 0.0));
    positionAlloc4->Add (Vector(10.681110656434134,112.68781245821864, 0.0));
    positionAlloc4->Add (Vector(109.21714946469451,194.11651377661582, 0.0));
    positionAlloc4->Add (Vector(338.2424170981152,384.65416950987134, 0.0));
    positionAlloc4->Add (Vector(33.90570569870172,373.82306788948495, 0.0));
    positionAlloc4->Add (Vector(4.731262601098329,372.41181097572843, 0.0));
    positionAlloc4->Add (Vector(286.47657439590864,183.96856722299302, 0.0));
    positionAlloc4->Add (Vector(374.3915255244586,350.1737718851598, 0.0));
    positionAlloc4->Add (Vector(311.8262132015133,338.2089955711166, 0.0));
    positionAlloc4->Add (Vector(144.58012557261935,358.3871044867713, 0.0));
    positionAlloc4->Add (Vector(332.9653656974972,65.9443874613943, 0.0));
    positionAlloc4->Add (Vector(189.83110112412663,86.2080950122333, 0.0));
    positionAlloc4->Add (Vector(104.00015248963959,106.46200892324997, 0.0));
    positionAlloc4->Add (Vector(361.8417474784182,183.89059753977594, 0.0));
    positionAlloc4->Add (Vector(385.37234717743394,321.15584413069763, 0.0));
    positionAlloc4->Add (Vector(16.07765157809755,213.87374317879858, 0.0));
    positionAlloc4->Add (Vector(387.8547341776451,132.16318360069158, 0.0));
    positionAlloc4->Add (Vector(77.35402219406299,267.14367270580766, 0.0));
    positionAlloc4->Add (Vector(74.79299406802365,137.4620399120586, 0.0));
    positionAlloc4->Add (Vector(142.12168451354117,356.813982880468, 0.0));
    positionAlloc4->Add (Vector(363.1747812093273,63.407568799133564, 0.0));
    positionAlloc4->Add (Vector(176.42966395697854,323.7170379369392, 0.0));
    positionAlloc4->Add (Vector(143.66357473592876,69.50280156278725, 0.0));
    positionAlloc4->Add (Vector(271.29229699564445,211.26830298918605, 0.0));
    positionAlloc4->Add (Vector(147.1517631979859,295.3248310871692, 0.0));
    positionAlloc4->Add (Vector(167.72500016642874,144.48026532665298, 0.0));
    positionAlloc4->Add (Vector(296.5501267329826,379.4296313609524, 0.0));
    positionAlloc4->Add (Vector(100.58904209009904,91.7078900108169, 0.0));
    positionAlloc4->Add (Vector(246.9909077789041,300.74015732199746, 0.0));
    positionAlloc4->Add (Vector(262.42456764412907,374.62176283899333, 0.0));
    positionAlloc4->Add (Vector(137.78440386034063,358.54873005656304, 0.0));
    positionAlloc4->Add (Vector(335.80990745666855,130.22224011911172, 0.0));
    positionAlloc4->Add (Vector(220.5041597130492,116.25038745903905, 0.0));
    positionAlloc4->Add (Vector(209.4536285595451,313.93454698659303, 0.0));
    positionAlloc4->Add (Vector(92.14074504332257,270.8632975867306, 0.0));
    positionAlloc4->Add (Vector(323.53293042370467,230.89072871924978, 0.0));
    positionAlloc4->Add (Vector(315.03043588751945,196.72913293286572, 0.0));
    positionAlloc4->Add (Vector(229.05691015604268,135.28091937690218, 0.0));
    positionAlloc4->Add (Vector(346.4739137129592,46.54862135202884, 0.0));
    positionAlloc4->Add (Vector(275.0558736177185,350.39361954686956, 0.0));
    positionAlloc4->Add (Vector(341.8184390126426,232.58784681215633, 0.0));
    positionAlloc4->Add (Vector(125.65203803406489,256.63332323289814, 0.0));
    positionAlloc4->Add (Vector(327.1434999514629,193.09531570124264, 0.0));
    positionAlloc4->Add (Vector(331.42236990773125,370.0216013862782, 0.0));
    positionAlloc4->Add (Vector(133.8848313901527,186.79309965131785, 0.0));
    positionAlloc4->Add (Vector(74.37943520816499,384.5224755699146, 0.0));
    positionAlloc4->Add (Vector(249.59268284757866,135.19769133008333, 0.0));
    positionAlloc4->Add (Vector(266.892803004453,355.1604131274453, 0.0));
    positionAlloc4->Add (Vector(204.67113616633293,329.2703181671003, 0.0));
    positionAlloc4->Add (Vector(339.1473620203537,291.06207728936704, 0.0));
    positionAlloc4->Add (Vector(33.32337791934572,48.967188732763404, 0.0));
    positionAlloc4->Add (Vector(269.12798799264806,148.16562688278947, 0.0));
    positionAlloc4->Add (Vector(5.727462348196033,378.19388880695146, 0.0));
    positionAlloc4->Add (Vector(366.2817098564643,200.46307825564585, 0.0));
    positionAlloc4->Add (Vector(357.3107092175948,168.0901623696792, 0.0));
    positionAlloc4->Add (Vector(285.57240112383624,232.0041628712009, 0.0));
    positionAlloc4->Add (Vector(83.76477031483138,359.3715846591226, 0.0));
    positionAlloc4->Add (Vector(234.37674665468947,176.12481189674605, 0.0));
    positionAlloc4->Add (Vector(285.42673612097707,378.83271626962306, 0.0));
    positionAlloc4->Add (Vector(317.19907176789854,252.74099102773556, 0.0));
    positionAlloc4->Add (Vector(188.70422140325925,270.7508433650354, 0.0));
    positionAlloc4->Add (Vector(105.00374077909478,269.92778455676125, 0.0));
    positionAlloc4->Add (Vector(373.47930360702355,68.34320680886945, 0.0));
    positionAlloc4->Add (Vector(152.66160860841524,251.65632649033597, 0.0));
    positionAlloc4->Add (Vector(178.94965847082284,5.058341489762608, 0.0));
    positionAlloc4->Add (Vector(249.75173048690579,104.8782971368865, 0.0));
    positionAlloc4->Add (Vector(318.7907743710249,202.49224849597468, 0.0));
    positionAlloc4->Add (Vector(310.41885491780545,201.09385476141614, 0.0));
    positionAlloc4->Add (Vector(257.7617477520394,11.578623578958114, 0.0));
    positionAlloc4->Add (Vector(188.4014384668278,379.8005047938934, 0.0));
    positionAlloc4->Add (Vector(80.3203042132961,12.272583440652518, 0.0));
    positionAlloc4->Add (Vector(1.3005099008269294,17.879761245311876, 0.0));
    positionAlloc4->Add (Vector(147.83257389817192,84.18095260655498, 0.0));
    positionAlloc4->Add (Vector(181.66859812983412,215.01127502623612, 0.0));
    positionAlloc4->Add (Vector(326.03157130635094,219.25396301444601, 0.0));
    positionAlloc4->Add (Vector(70.85809708502948,122.6881456069532, 0.0));
    positionAlloc4->Add (Vector(372.1567963092581,207.77593158048026, 0.0));
    positionAlloc4->Add (Vector(156.50849935498158,253.36654410763256, 0.0));
    positionAlloc4->Add (Vector(311.5135914952324,277.83136464824383, 0.0));
    positionAlloc4->Add (Vector(364.00041203209224,277.0363080293304, 0.0));
    positionAlloc4->Add (Vector(368.69516884734253,225.0100473580655, 0.0));
    positionAlloc4->Add (Vector(28.817147385184327,337.85946190210524, 0.0));
    positionAlloc4->Add (Vector(72.48881624131904,353.53421134017594, 0.0));
    positionAlloc4->Add (Vector(154.12051782008277,38.8846157355359, 0.0));
    positionAlloc4->Add (Vector(238.41458543310415,350.59456116029264, 0.0));
    positionAlloc4->Add (Vector(183.20420606623688,306.8042321009127, 0.0));
    positionAlloc4->Add (Vector(131.0590233540335,160.02894585921564, 0.0));
    positionAlloc4->Add (Vector(152.71783399758186,246.8000076808091, 0.0));
    positionAlloc4->Add (Vector(342.79578341638415,377.2996627356805, 0.0));
    positionAlloc4->Add (Vector(299.75965894458426,292.349457756084, 0.0));
    positionAlloc4->Add (Vector(13.925926273669775,45.771477678556984, 0.0));
    positionAlloc4->Add (Vector(247.49484100903842,4.53254921285966, 0.0));
    positionAlloc4->Add (Vector(263.2928140793935,303.9940474851534, 0.0));
    positionAlloc4->Add (Vector(248.37495046468572,152.07517546895085, 0.0));
    positionAlloc4->Add (Vector(351.5597807735733,336.4741102584644, 0.0));
    positionAlloc4->Add (Vector(328.3530463197817,270.86435479674265, 0.0));
    positionAlloc4->Add (Vector(123.89318987647596,376.5662895912993, 0.0));
    positionAlloc4->Add (Vector(92.83569196627819,56.59310219560978, 0.0));
    positionAlloc4->Add (Vector(30.51190380139195,393.98302051582885, 0.0));
    positionAlloc4->Add (Vector(63.01395402947043,313.40276896577495, 0.0));
    positionAlloc4->Add (Vector(240.61661773088792,167.9342468520847, 0.0));
    positionAlloc4->Add (Vector(167.89939173935352,369.7583186897989, 0.0));
    positionAlloc4->Add (Vector(124.40113994885924,43.38242609815972, 0.0));
    positionAlloc4->Add (Vector(210.92573588009316,242.59634046394817, 0.0));
    positionAlloc4->Add (Vector(391.47920200121894,80.72501237561313, 0.0));
    positionAlloc4->Add (Vector(125.39239254134667,28.909964885668636, 0.0));
    positionAlloc4->Add (Vector(116.19487127362369,66.40162890715912, 0.0));
    positionAlloc4->Add (Vector(266.12979449378497,321.71576541932603, 0.0));
    positionAlloc4->Add (Vector(173.4644074257925,385.9431938809504, 0.0));
    positionAlloc4->Add (Vector(186.3112273489947,17.680082931772432, 0.0));
    positionAlloc4->Add (Vector(208.38561480679328,92.00639970478717, 0.0));
    positionAlloc4->Add (Vector(192.36821798755716,10.592322508409069, 0.0));
    positionAlloc4->Add (Vector(252.73920649146348,229.17307345917428, 0.0));
    positionAlloc4->Add (Vector(291.43298750775875,128.40026292737338, 0.0));
    positionAlloc4->Add (Vector(329.03322511330464,342.7333660767229, 0.0));
    positionAlloc4->Add (Vector(118.37955501916694,74.38968833712644, 0.0));
    positionAlloc4->Add (Vector(350.32495675235,231.83766076055284, 0.0));
    positionAlloc4->Add (Vector(126.33186232692206,122.7148728051024, 0.0));
    positionAlloc4->Add (Vector(140.49846833982548,395.6257317958437, 0.0));
    positionAlloc4->Add (Vector(141.2075152791044,349.2407065503586, 0.0));
    positionAlloc4->Add (Vector(333.9954753965191,28.35629134307589, 0.0));
    positionAlloc4->Add (Vector(361.1393097338829,99.54823368806389, 0.0));
    positionAlloc4->Add (Vector(172.30115566959165,53.1058594194699, 0.0));
    positionAlloc4->Add (Vector(98.10040146873918,218.1933211158866, 0.0));
    positionAlloc4->Add (Vector(393.3854204976005,380.7102952496721, 0.0));
    positionAlloc4->Add (Vector(297.4052543115851,159.66174022195173, 0.0));
    positionAlloc4->Add (Vector(344.02100137098967,49.63658914024855, 0.0));
    positionAlloc4->Add (Vector(136.00050556663982,116.01343449651287, 0.0));
    positionAlloc4->Add (Vector(132.63912054047745,80.12164237312298, 0.0));
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
	  AnimationInterface anim ("animations/animation_SUI_17.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_SUI_flowmon/switch_SUI_17.flowmon", false, false);
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