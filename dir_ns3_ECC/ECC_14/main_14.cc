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
	  uint16_t numberOfRrhs = 78;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 736;
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
    positionAlloc->Add (Vector (500.0,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (500.0,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (500.0,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,0.0, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,500.0, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,0.0, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,0.0, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,500.0, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,0.0, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,55.55555555555556, 0.0));
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
    positionAlloc->Add (Vector (111.11111111111111,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,0.0, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (0.0,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (0.0,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (0.0,166.66666666666666, 0.0));
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
    positionAlloc4->Add (Vector(350.7827566247461,301.71619389840544, 0.0));
    positionAlloc4->Add (Vector(289.5133153334638,288.8091508836652, 0.0));
    positionAlloc4->Add (Vector(412.34189928390447,120.42334968892432, 0.0));
    positionAlloc4->Add (Vector(394.7539913407417,353.47423985614313, 0.0));
    positionAlloc4->Add (Vector(416.54317740214384,247.39038094889438, 0.0));
    positionAlloc4->Add (Vector(95.44847441873571,86.25140282791854, 0.0));
    positionAlloc4->Add (Vector(366.4782544818843,18.593797442137294, 0.0));
    positionAlloc4->Add (Vector(400.52634102699176,409.0156073741068, 0.0));
    positionAlloc4->Add (Vector(453.80069881985696,311.6586419871519, 0.0));
    positionAlloc4->Add (Vector(99.36169685912155,298.9146051774703, 0.0));
    positionAlloc4->Add (Vector(434.8030218534852,176.67420420924972, 0.0));
    positionAlloc4->Add (Vector(326.3674163526464,257.6334750102345, 0.0));
    positionAlloc4->Add (Vector(310.57649580499714,172.32247788492123, 0.0));
    positionAlloc4->Add (Vector(403.8640033211983,355.3597845893177, 0.0));
    positionAlloc4->Add (Vector(196.69574079831585,124.34369783553434, 0.0));
    positionAlloc4->Add (Vector(8.316256301074631,401.949311010034, 0.0));
    positionAlloc4->Add (Vector(286.4231853343231,70.22059435365391, 0.0));
    positionAlloc4->Add (Vector(210.15038641415939,103.55436856494794, 0.0));
    positionAlloc4->Add (Vector(143.69651665101634,98.76590442230521, 0.0));
    positionAlloc4->Add (Vector(27.949045341152058,217.500969351226, 0.0));
    positionAlloc4->Add (Vector(10.432483854277862,226.89258298517422, 0.0));
    positionAlloc4->Add (Vector(179.2060549089386,1.2464307939185626, 0.0));
    positionAlloc4->Add (Vector(271.28096527384014,89.67129578496785, 0.0));
    positionAlloc4->Add (Vector(261.4349282001286,159.33979784178248, 0.0));
    positionAlloc4->Add (Vector(140.0722953376896,149.97990041942555, 0.0));
    positionAlloc4->Add (Vector(4.580929640787823,187.21765519375782, 0.0));
    positionAlloc4->Add (Vector(254.5358220443224,306.8372734981468, 0.0));
    positionAlloc4->Add (Vector(159.72569621606797,217.64773264777548, 0.0));
    positionAlloc4->Add (Vector(418.72858485407625,430.21690449538517, 0.0));
    positionAlloc4->Add (Vector(132.41849809742402,343.3612874559051, 0.0));
    positionAlloc4->Add (Vector(2.510898619826063,441.5960531861296, 0.0));
    positionAlloc4->Add (Vector(237.20132467961662,428.02960492945607, 0.0));
    positionAlloc4->Add (Vector(234.2968650885296,470.0412583878126, 0.0));
    positionAlloc4->Add (Vector(182.41883138443467,371.204078101624, 0.0));
    positionAlloc4->Add (Vector(113.12224886491751,430.1981263178129, 0.0));
    positionAlloc4->Add (Vector(282.0965001166749,434.5732837354399, 0.0));
    positionAlloc4->Add (Vector(324.26179239156295,447.90890285244524, 0.0));
    positionAlloc4->Add (Vector(361.90258170003773,347.7628188502801, 0.0));
    positionAlloc4->Add (Vector(265.432611835225,16.828619408099154, 0.0));
    positionAlloc4->Add (Vector(9.043569033167776,245.28553665274566, 0.0));
    positionAlloc4->Add (Vector(393.4190927376711,464.7631427327177, 0.0));
    positionAlloc4->Add (Vector(144.44246537642147,435.5133835108658, 0.0));
    positionAlloc4->Add (Vector(234.82209148312106,80.49826959937756, 0.0));
    positionAlloc4->Add (Vector(378.7318115222054,270.3532111187516, 0.0));
    positionAlloc4->Add (Vector(235.81941658524065,257.9238609733493, 0.0));
    positionAlloc4->Add (Vector(468.67254173905945,254.40641798761797, 0.0));
    positionAlloc4->Add (Vector(162.3606558615548,405.3132005064183, 0.0));
    positionAlloc4->Add (Vector(230.2676373220599,234.35500922015567, 0.0));
    positionAlloc4->Add (Vector(330.72120873016934,141.19553836588577, 0.0));
    positionAlloc4->Add (Vector(494.42105128166526,232.17029667571504, 0.0));
    positionAlloc4->Add (Vector(18.427366344462403,410.6423609404757, 0.0));
    positionAlloc4->Add (Vector(398.31986441130016,108.3806276574818, 0.0));
    positionAlloc4->Add (Vector(71.96211954835546,125.21968842432402, 0.0));
    positionAlloc4->Add (Vector(378.66188154026537,62.94612513783887, 0.0));
    positionAlloc4->Add (Vector(63.88358205841549,364.99656632707604, 0.0));
    positionAlloc4->Add (Vector(184.12654770901838,35.2907512841989, 0.0));
    positionAlloc4->Add (Vector(417.58323004037584,345.69440184133106, 0.0));
    positionAlloc4->Add (Vector(137.05050067416624,461.1673999597053, 0.0));
    positionAlloc4->Add (Vector(104.63461442551814,143.53724090964315, 0.0));
    positionAlloc4->Add (Vector(328.8762847200031,13.778367840538541, 0.0));
    positionAlloc4->Add (Vector(345.0145520490167,434.9076232801517, 0.0));
    positionAlloc4->Add (Vector(169.7665916552498,127.79431698487869, 0.0));
    positionAlloc4->Add (Vector(137.23151571202897,461.25442371961265, 0.0));
    positionAlloc4->Add (Vector(157.38164693307348,426.36272520614375, 0.0));
    positionAlloc4->Add (Vector(363.7628886737799,182.22642685365724, 0.0));
    positionAlloc4->Add (Vector(428.2520597619736,17.126614157337794, 0.0));
    positionAlloc4->Add (Vector(275.789922340776,195.57772201762768, 0.0));
    positionAlloc4->Add (Vector(8.514240388005078,365.3168308568697, 0.0));
    positionAlloc4->Add (Vector(175.64775656308186,479.78680838286886, 0.0));
    positionAlloc4->Add (Vector(157.8053416679625,267.2495287063464, 0.0));
    positionAlloc4->Add (Vector(66.6312145734918,362.6053116520193, 0.0));
    positionAlloc4->Add (Vector(485.2047881543332,421.66981195496476, 0.0));
    positionAlloc4->Add (Vector(79.58983156531374,395.17715545051686, 0.0));
    positionAlloc4->Add (Vector(426.2547602172834,342.1811073983198, 0.0));
    positionAlloc4->Add (Vector(201.4047665276182,372.5406910611971, 0.0));
    positionAlloc4->Add (Vector(459.39822813152335,490.2214273414904, 0.0));
    positionAlloc4->Add (Vector(451.87955484530363,230.37937937278485, 0.0));
    positionAlloc4->Add (Vector(190.38177242776212,225.2213698537852, 0.0));
    positionAlloc4->Add (Vector(431.3696965594857,400.59255184676334, 0.0));
    positionAlloc4->Add (Vector(353.1634176881923,54.959819888161284, 0.0));
    positionAlloc4->Add (Vector(308.3657830996031,85.40608456458875, 0.0));
    positionAlloc4->Add (Vector(367.9536110528626,335.53909943082635, 0.0));
    positionAlloc4->Add (Vector(11.096524294353538,151.04156940755414, 0.0));
    positionAlloc4->Add (Vector(273.03575506852366,180.51698128600052, 0.0));
    positionAlloc4->Add (Vector(111.96762777357988,130.01457818670963, 0.0));
    positionAlloc4->Add (Vector(192.99741232343888,181.6331412027375, 0.0));
    positionAlloc4->Add (Vector(71.47319175503274,363.4812119629172, 0.0));
    positionAlloc4->Add (Vector(127.14054972238247,181.10687444631145, 0.0));
    positionAlloc4->Add (Vector(173.6404649918009,93.440633817379, 0.0));
    positionAlloc4->Add (Vector(0.685158811816855,483.8914253393631, 0.0));
    positionAlloc4->Add (Vector(176.533163804851,485.76932500963864, 0.0));
    positionAlloc4->Add (Vector(168.43203470961748,211.8230114775811, 0.0));
    positionAlloc4->Add (Vector(254.49933634668537,117.1193086163494, 0.0));
    positionAlloc4->Add (Vector(499.4023100616887,102.76957081569665, 0.0));
    positionAlloc4->Add (Vector(90.44472740792881,35.79790465535554, 0.0));
    positionAlloc4->Add (Vector(446.56320847213016,104.4992944296006, 0.0));
    positionAlloc4->Add (Vector(230.67735552431517,165.77761938187098, 0.0));
    positionAlloc4->Add (Vector(299.6429690113915,131.63967971786084, 0.0));
    positionAlloc4->Add (Vector(462.11820402400537,13.672817530111569, 0.0));
    positionAlloc4->Add (Vector(161.67641423688173,451.5595674825717, 0.0));
    positionAlloc4->Add (Vector(123.89175290606063,355.3666589228835, 0.0));
    positionAlloc4->Add (Vector(286.6187788467945,286.0091613214424, 0.0));
    positionAlloc4->Add (Vector(210.1024265464932,353.68624294966554, 0.0));
    positionAlloc4->Add (Vector(448.8797946896403,477.21340850480357, 0.0));
    positionAlloc4->Add (Vector(313.5896995147349,154.4802510981521, 0.0));
    positionAlloc4->Add (Vector(264.6168680890818,234.37802577082024, 0.0));
    positionAlloc4->Add (Vector(25.7639694171663,153.64627737869324, 0.0));
    positionAlloc4->Add (Vector(317.84279963688175,229.1106805324018, 0.0));
    positionAlloc4->Add (Vector(81.32970892311924,379.42102475971797, 0.0));
    positionAlloc4->Add (Vector(54.51680855777863,435.3207563950681, 0.0));
    positionAlloc4->Add (Vector(268.9663103509858,217.57825871725183, 0.0));
    positionAlloc4->Add (Vector(188.73738479819824,360.02473526496544, 0.0));
    positionAlloc4->Add (Vector(70.07634372476046,457.24611233858934, 0.0));
    positionAlloc4->Add (Vector(4.046971067856675,191.65141255878626, 0.0));
    positionAlloc4->Add (Vector(252.8554736740938,475.0625603711618, 0.0));
    positionAlloc4->Add (Vector(27.96831565401481,70.51276678265394, 0.0));
    positionAlloc4->Add (Vector(24.17422189913465,282.5398570921204, 0.0));
    positionAlloc4->Add (Vector(195.3066470555059,346.12167940594725, 0.0));
    positionAlloc4->Add (Vector(137.63176382746644,52.869084999321025, 0.0));
    positionAlloc4->Add (Vector(308.6604292703627,233.30764520993296, 0.0));
    positionAlloc4->Add (Vector(303.8423948867149,387.0573658986529, 0.0));
    positionAlloc4->Add (Vector(174.3502139229131,106.5549857824682, 0.0));
    positionAlloc4->Add (Vector(343.9511520091134,338.56649398146044, 0.0));
    positionAlloc4->Add (Vector(249.9457396047693,272.20851534903295, 0.0));
    positionAlloc4->Add (Vector(479.28477009788276,250.69169753015618, 0.0));
    positionAlloc4->Add (Vector(277.47654628090476,40.611049872597334, 0.0));
    positionAlloc4->Add (Vector(77.54469708907862,305.79350384187165, 0.0));
    positionAlloc4->Add (Vector(442.37492954708034,469.9906112218922, 0.0));
    positionAlloc4->Add (Vector(310.3313239185329,177.94427523363575, 0.0));
    positionAlloc4->Add (Vector(368.18743754255445,217.50018505660086, 0.0));
    positionAlloc4->Add (Vector(303.48545770941786,260.1881338833255, 0.0));
    positionAlloc4->Add (Vector(194.99428986073607,424.3019258906262, 0.0));
    positionAlloc4->Add (Vector(255.08412928532843,283.0064961304375, 0.0));
    positionAlloc4->Add (Vector(326.77658587383684,99.36854845829768, 0.0));
    positionAlloc4->Add (Vector(210.61343270763854,292.17594750519635, 0.0));
    positionAlloc4->Add (Vector(127.70665854058628,231.75986133994354, 0.0));
    positionAlloc4->Add (Vector(64.77808098227861,139.73294040288414, 0.0));
    positionAlloc4->Add (Vector(345.3564226317681,56.39829657691947, 0.0));
    positionAlloc4->Add (Vector(11.46016436432118,375.50414237176386, 0.0));
    positionAlloc4->Add (Vector(233.22672838022828,106.50189377608149, 0.0));
    positionAlloc4->Add (Vector(9.20489164688776,174.6384971499696, 0.0));
    positionAlloc4->Add (Vector(462.6625502840117,402.02884036007964, 0.0));
    positionAlloc4->Add (Vector(210.47594919469725,311.4451792819408, 0.0));
    positionAlloc4->Add (Vector(128.17537750751328,174.40672326861528, 0.0));
    positionAlloc4->Add (Vector(497.28039467287203,359.16838027199583, 0.0));
    positionAlloc4->Add (Vector(267.2092060299519,431.29129966796336, 0.0));
    positionAlloc4->Add (Vector(456.83466294337995,85.91558542389043, 0.0));
    positionAlloc4->Add (Vector(339.57817613473816,235.80396952706963, 0.0));
    positionAlloc4->Add (Vector(293.6743382792871,110.95411829959151, 0.0));
    positionAlloc4->Add (Vector(197.73536277747428,68.87036190153923, 0.0));
    positionAlloc4->Add (Vector(250.5952218168533,72.75142061753247, 0.0));
    positionAlloc4->Add (Vector(376.2843177855515,378.9334227580026, 0.0));
    positionAlloc4->Add (Vector(455.64530450492987,134.95763836477082, 0.0));
    positionAlloc4->Add (Vector(301.098454227484,396.7018379942536, 0.0));
    positionAlloc4->Add (Vector(417.04554283485766,160.074900321046, 0.0));
    positionAlloc4->Add (Vector(164.3347782814325,130.3662989397774, 0.0));
    positionAlloc4->Add (Vector(48.24962714130226,344.36835490697416, 0.0));
    positionAlloc4->Add (Vector(35.37327577400179,474.07863232217187, 0.0));
    positionAlloc4->Add (Vector(223.19981088418612,199.5672673027443, 0.0));
    positionAlloc4->Add (Vector(453.77690175025566,301.986042864485, 0.0));
    positionAlloc4->Add (Vector(336.7505354270825,286.2679587759156, 0.0));
    positionAlloc4->Add (Vector(445.4173070268932,342.6564741084584, 0.0));
    positionAlloc4->Add (Vector(104.41541978466651,349.7190176183625, 0.0));
    positionAlloc4->Add (Vector(277.84158384255045,333.8780475748871, 0.0));
    positionAlloc4->Add (Vector(131.38656308382363,263.2544118332831, 0.0));
    positionAlloc4->Add (Vector(128.03347080796507,458.14654841787916, 0.0));
    positionAlloc4->Add (Vector(328.1201702191301,452.01613496651345, 0.0));
    positionAlloc4->Add (Vector(329.1277187144348,301.4466793673883, 0.0));
    positionAlloc4->Add (Vector(74.48835463644626,331.64376108256147, 0.0));
    positionAlloc4->Add (Vector(96.58215889353433,7.841129534594371, 0.0));
    positionAlloc4->Add (Vector(50.977406811743954,291.1549135840732, 0.0));
    positionAlloc4->Add (Vector(26.493196966119903,472.0365207891419, 0.0));
    positionAlloc4->Add (Vector(466.27832915225673,329.10946867667457, 0.0));
    positionAlloc4->Add (Vector(319.1826913607589,95.68875501061636, 0.0));
    positionAlloc4->Add (Vector(424.56515046031046,305.19592639640837, 0.0));
    positionAlloc4->Add (Vector(167.4466379215711,311.1803767110166, 0.0));
    positionAlloc4->Add (Vector(368.03464657355914,84.09495996561789, 0.0));
    positionAlloc4->Add (Vector(7.342553107505589,484.51225484595386, 0.0));
    positionAlloc4->Add (Vector(37.658070111340706,38.08047070988546, 0.0));
    positionAlloc4->Add (Vector(374.7836618696728,80.50363251497683, 0.0));
    positionAlloc4->Add (Vector(350.22891764380387,166.44291515414704, 0.0));
    positionAlloc4->Add (Vector(308.6138208761297,256.8388411244627, 0.0));
    positionAlloc4->Add (Vector(289.331888591809,411.11996912955846, 0.0));
    positionAlloc4->Add (Vector(470.01053944894596,433.4936462134955, 0.0));
    positionAlloc4->Add (Vector(354.62434342592235,15.5709505267963, 0.0));
    positionAlloc4->Add (Vector(30.383677979023084,12.476650316736116, 0.0));
    positionAlloc4->Add (Vector(456.44166087208583,446.1391656969343, 0.0));
    positionAlloc4->Add (Vector(314.40039872668984,207.52554035131377, 0.0));
    positionAlloc4->Add (Vector(425.16256126694145,197.28593647982945, 0.0));
    positionAlloc4->Add (Vector(145.91994960663672,423.7134391556862, 0.0));
    positionAlloc4->Add (Vector(250.44388025320086,487.35235122660924, 0.0));
    positionAlloc4->Add (Vector(19.667468100432075,221.34834107539214, 0.0));
    positionAlloc4->Add (Vector(399.38401433643867,437.836012156906, 0.0));
    positionAlloc4->Add (Vector(97.6298902728176,278.016454418128, 0.0));
    positionAlloc4->Add (Vector(193.27035652250657,139.66621158913128, 0.0));
    positionAlloc4->Add (Vector(246.4120765479123,281.3949309287983, 0.0));
    positionAlloc4->Add (Vector(401.15288147889845,477.1013326769403, 0.0));
    positionAlloc4->Add (Vector(382.17548692794526,115.9529877392298, 0.0));
    positionAlloc4->Add (Vector(91.24083721945242,300.41073124237636, 0.0));
    positionAlloc4->Add (Vector(463.91693759821663,18.542175654541193, 0.0));
    positionAlloc4->Add (Vector(337.32947814613664,11.341884450317362, 0.0));
    positionAlloc4->Add (Vector(289.09192388892626,345.51527969600977, 0.0));
    positionAlloc4->Add (Vector(236.54647330290436,38.36303706627808, 0.0));
    positionAlloc4->Add (Vector(15.63836045124134,43.375399311755324, 0.0));
    positionAlloc4->Add (Vector(132.20351722957236,9.95697653799893, 0.0));
    positionAlloc4->Add (Vector(406.2188205773984,167.38414466886996, 0.0));
    positionAlloc4->Add (Vector(97.84784764354949,7.463733815384788, 0.0));
    positionAlloc4->Add (Vector(207.23457701963898,24.97230336103995, 0.0));
    positionAlloc4->Add (Vector(25.229869790005075,27.68801220381234, 0.0));
    positionAlloc4->Add (Vector(387.5614171235026,337.33193413568716, 0.0));
    positionAlloc4->Add (Vector(391.22021959663414,396.3583224493797, 0.0));
    positionAlloc4->Add (Vector(107.2676608085073,158.2339460423826, 0.0));
    positionAlloc4->Add (Vector(200.06575711776475,57.93099125689982, 0.0));
    positionAlloc4->Add (Vector(490.19270625160533,248.7851547871898, 0.0));
    positionAlloc4->Add (Vector(173.20965283569774,328.628214225172, 0.0));
    positionAlloc4->Add (Vector(392.19848274128293,170.16750734218627, 0.0));
    positionAlloc4->Add (Vector(186.76085381588436,12.710606285721793, 0.0));
    positionAlloc4->Add (Vector(189.5173884778369,73.18100366815183, 0.0));
    positionAlloc4->Add (Vector(396.31283806021605,114.93272923510101, 0.0));
    positionAlloc4->Add (Vector(496.6229502595587,41.971131867728886, 0.0));
    positionAlloc4->Add (Vector(372.73029638260044,68.4687040110844, 0.0));
    positionAlloc4->Add (Vector(302.0908651316148,192.94037031357465, 0.0));
    positionAlloc4->Add (Vector(250.35440414019422,397.6331794107112, 0.0));
    positionAlloc4->Add (Vector(353.0157058578121,135.66253070511203, 0.0));
    positionAlloc4->Add (Vector(315.7475682139152,431.8600508483159, 0.0));
    positionAlloc4->Add (Vector(145.59046558744066,152.13511539175428, 0.0));
    positionAlloc4->Add (Vector(480.43722003613556,435.0087152422553, 0.0));
    positionAlloc4->Add (Vector(41.791724912465746,454.91796622975403, 0.0));
    positionAlloc4->Add (Vector(34.65174285883704,191.84072506420767, 0.0));
    positionAlloc4->Add (Vector(391.5450743735037,331.52401795309214, 0.0));
    positionAlloc4->Add (Vector(398.18906081700607,337.19725534971633, 0.0));
    positionAlloc4->Add (Vector(289.8159932054015,69.668959569186, 0.0));
    positionAlloc4->Add (Vector(246.97591627216875,107.680176199777, 0.0));
    positionAlloc4->Add (Vector(398.2496177042849,477.018999104276, 0.0));
    positionAlloc4->Add (Vector(268.2456972590142,180.8240253962053, 0.0));
    positionAlloc4->Add (Vector(411.2774652784061,479.1720749852436, 0.0));
    positionAlloc4->Add (Vector(43.83845973409572,212.90564083254137, 0.0));
    positionAlloc4->Add (Vector(26.028524473669933,141.0883747431218, 0.0));
    positionAlloc4->Add (Vector(232.5837787431434,247.31431719135315, 0.0));
    positionAlloc4->Add (Vector(348.8905373132639,213.52404708893573, 0.0));
    positionAlloc4->Add (Vector(146.79105371911945,446.8313554849205, 0.0));
    positionAlloc4->Add (Vector(209.06329906298842,306.0919321126044, 0.0));
    positionAlloc4->Add (Vector(225.76289723770992,206.410941074695, 0.0));
    positionAlloc4->Add (Vector(412.1712670885576,332.29372736296426, 0.0));
    positionAlloc4->Add (Vector(342.80831345867506,393.2582792488731, 0.0));
    positionAlloc4->Add (Vector(53.30070621546129,382.44848257088637, 0.0));
    positionAlloc4->Add (Vector(385.0707140047025,221.22409383946624, 0.0));
    positionAlloc4->Add (Vector(323.0263030925819,262.766390590017, 0.0));
    positionAlloc4->Add (Vector(410.38016058109423,260.67630047240493, 0.0));
    positionAlloc4->Add (Vector(403.99243516717456,20.244280258321478, 0.0));
    positionAlloc4->Add (Vector(23.496797913528322,436.99444644942025, 0.0));
    positionAlloc4->Add (Vector(353.9637353282837,474.0086088781335, 0.0));
    positionAlloc4->Add (Vector(381.68012193465137,432.0201064969755, 0.0));
    positionAlloc4->Add (Vector(34.712994529844664,229.01380530260212, 0.0));
    positionAlloc4->Add (Vector(398.3356755536755,23.82578534666052, 0.0));
    positionAlloc4->Add (Vector(299.10471644928515,8.040438057663968, 0.0));
    positionAlloc4->Add (Vector(211.26189874906902,295.34029158167397, 0.0));
    positionAlloc4->Add (Vector(83.70381148146444,380.4413369496481, 0.0));
    positionAlloc4->Add (Vector(285.1523108385923,464.1635544177051, 0.0));
    positionAlloc4->Add (Vector(195.90814212368502,126.40920857058246, 0.0));
    positionAlloc4->Add (Vector(192.55783424508704,259.7841426918113, 0.0));
    positionAlloc4->Add (Vector(289.32028326948597,313.54133436354266, 0.0));
    positionAlloc4->Add (Vector(437.00180051668514,249.61158836976938, 0.0));
    positionAlloc4->Add (Vector(133.86885687997423,381.62479552187017, 0.0));
    positionAlloc4->Add (Vector(289.3298632608351,41.91542347884136, 0.0));
    positionAlloc4->Add (Vector(347.1203310954995,347.62814125862207, 0.0));
    positionAlloc4->Add (Vector(227.3312690503212,415.673418861753, 0.0));
    positionAlloc4->Add (Vector(442.35681153442135,58.33274173255343, 0.0));
    positionAlloc4->Add (Vector(10.379077032139028,82.16460340573289, 0.0));
    positionAlloc4->Add (Vector(481.80998332782957,497.1548109090395, 0.0));
    positionAlloc4->Add (Vector(450.3755623256939,365.8963081508945, 0.0));
    positionAlloc4->Add (Vector(494.6920492755273,493.4077994120834, 0.0));
    positionAlloc4->Add (Vector(302.8045656217685,51.14368522648749, 0.0));
    positionAlloc4->Add (Vector(306.83793217044604,469.7482908300675, 0.0));
    positionAlloc4->Add (Vector(373.62609319252385,431.28228244758594, 0.0));
    positionAlloc4->Add (Vector(388.079942317166,144.0339050189443, 0.0));
    positionAlloc4->Add (Vector(74.0171996440101,454.9845594768698, 0.0));
    positionAlloc4->Add (Vector(132.2936383164109,85.06223723332312, 0.0));
    positionAlloc4->Add (Vector(471.61159166428627,332.6816163027263, 0.0));
    positionAlloc4->Add (Vector(88.22565274510963,120.2620057079185, 0.0));
    positionAlloc4->Add (Vector(435.0432437541037,390.7731996506632, 0.0));
    positionAlloc4->Add (Vector(341.4638335074802,346.8181548363746, 0.0));
    positionAlloc4->Add (Vector(20.127701956898058,192.20695726748488, 0.0));
    positionAlloc4->Add (Vector(135.54282593767863,323.59458938890236, 0.0));
    positionAlloc4->Add (Vector(199.60966066588904,133.127662252219, 0.0));
    positionAlloc4->Add (Vector(238.5983232919917,389.9835068137754, 0.0));
    positionAlloc4->Add (Vector(442.8109542594176,444.1188800699515, 0.0));
    positionAlloc4->Add (Vector(28.259677545696693,427.01350553396236, 0.0));
    positionAlloc4->Add (Vector(339.4904667691941,207.21058149324102, 0.0));
    positionAlloc4->Add (Vector(89.54100842024553,229.3706736154544, 0.0));
    positionAlloc4->Add (Vector(450.2558865953079,159.13887859123298, 0.0));
    positionAlloc4->Add (Vector(222.38195418139173,415.9487243408493, 0.0));
    positionAlloc4->Add (Vector(330.25944688730425,146.95564931536342, 0.0));
    positionAlloc4->Add (Vector(377.0140879899431,77.50539909718212, 0.0));
    positionAlloc4->Add (Vector(375.14768473265195,227.53580471928976, 0.0));
    positionAlloc4->Add (Vector(91.79747724642145,366.0562769739668, 0.0));
    positionAlloc4->Add (Vector(138.61340193282928,346.0368279989654, 0.0));
    positionAlloc4->Add (Vector(173.83048189473865,216.81313238663085, 0.0));
    positionAlloc4->Add (Vector(311.0220344765564,471.96888170889736, 0.0));
    positionAlloc4->Add (Vector(118.34716261718143,31.41186512556865, 0.0));
    positionAlloc4->Add (Vector(367.68894139056215,406.0992015055389, 0.0));
    positionAlloc4->Add (Vector(382.74848023470605,154.08638584100987, 0.0));
    positionAlloc4->Add (Vector(54.02182816002371,381.39688882161107, 0.0));
    positionAlloc4->Add (Vector(148.11561413582342,29.766847438137955, 0.0));
    positionAlloc4->Add (Vector(89.59126840730814,347.3320462525, 0.0));
    positionAlloc4->Add (Vector(498.28447308559623,432.55404487062424, 0.0));
    positionAlloc4->Add (Vector(293.29923655510316,432.64758449609474, 0.0));
    positionAlloc4->Add (Vector(467.1062868599709,362.7395268115389, 0.0));
    positionAlloc4->Add (Vector(103.16244235765765,297.17029551310634, 0.0));
    positionAlloc4->Add (Vector(294.20612603126347,279.38239073004064, 0.0));
    positionAlloc4->Add (Vector(33.03928026687747,378.9502307133503, 0.0));
    positionAlloc4->Add (Vector(242.88945363297194,80.67335059760417, 0.0));
    positionAlloc4->Add (Vector(25.925836961293026,380.9108338245879, 0.0));
    positionAlloc4->Add (Vector(195.46314606786424,136.43823160426555, 0.0));
    positionAlloc4->Add (Vector(386.11483920510204,221.50170509863827, 0.0));
    positionAlloc4->Add (Vector(42.41223274884476,233.8234147608056, 0.0));
    positionAlloc4->Add (Vector(180.94014615475467,260.3294822343225, 0.0));
    positionAlloc4->Add (Vector(284.8266224576812,34.15932037996872, 0.0));
    positionAlloc4->Add (Vector(261.36833615747355,203.8556801090232, 0.0));
    positionAlloc4->Add (Vector(470.7990665083745,160.04285464133272, 0.0));
    positionAlloc4->Add (Vector(26.91834046180125,278.21711832276094, 0.0));
    positionAlloc4->Add (Vector(160.29212567077406,316.48599820683773, 0.0));
    positionAlloc4->Add (Vector(4.52486288369186,360.2363214777108, 0.0));
    positionAlloc4->Add (Vector(306.96098094791415,370.1293742197067, 0.0));
    positionAlloc4->Add (Vector(45.63818117642132,367.7908553616201, 0.0));
    positionAlloc4->Add (Vector(153.63057604588937,371.4372769979616, 0.0));
    positionAlloc4->Add (Vector(176.17674063095623,77.82021576118503, 0.0));
    positionAlloc4->Add (Vector(255.78411955256098,432.79109493707483, 0.0));
    positionAlloc4->Add (Vector(224.1477953585042,63.17646108319086, 0.0));
    positionAlloc4->Add (Vector(182.0739622063599,125.27312922916062, 0.0));
    positionAlloc4->Add (Vector(481.87376415240624,57.16216132965418, 0.0));
    positionAlloc4->Add (Vector(282.08772592037235,492.0106485407461, 0.0));
    positionAlloc4->Add (Vector(421.45867362254916,269.8065115087482, 0.0));
    positionAlloc4->Add (Vector(442.8321776224999,189.6549669681848, 0.0));
    positionAlloc4->Add (Vector(438.56444249885624,119.87075110361577, 0.0));
    positionAlloc4->Add (Vector(424.79891815203644,26.056179209812292, 0.0));
    positionAlloc4->Add (Vector(439.0697806495376,65.76333782119126, 0.0));
    positionAlloc4->Add (Vector(417.07915669558844,352.4653783641353, 0.0));
    positionAlloc4->Add (Vector(281.7919801455088,142.08395080652403, 0.0));
    positionAlloc4->Add (Vector(420.1335688918106,268.5788072405354, 0.0));
    positionAlloc4->Add (Vector(143.02749800297698,152.15145935067264, 0.0));
    positionAlloc4->Add (Vector(226.64374665402332,114.21545906571018, 0.0));
    positionAlloc4->Add (Vector(344.9776342156111,397.5078922119597, 0.0));
    positionAlloc4->Add (Vector(74.7228995764207,381.5135912742435, 0.0));
    positionAlloc4->Add (Vector(296.05328149040577,447.2557938920817, 0.0));
    positionAlloc4->Add (Vector(100.83312257287608,359.72599373592993, 0.0));
    positionAlloc4->Add (Vector(419.92527970502755,434.86256671158105, 0.0));
    positionAlloc4->Add (Vector(183.13351390079973,145.37445086011374, 0.0));
    positionAlloc4->Add (Vector(253.7301291417652,483.70121698421474, 0.0));
    positionAlloc4->Add (Vector(292.03665077732427,472.0032502652233, 0.0));
    positionAlloc4->Add (Vector(340.1181979879732,94.71237689417922, 0.0));
    positionAlloc4->Add (Vector(218.63407631944304,431.6006228373178, 0.0));
    positionAlloc4->Add (Vector(254.1854576189837,42.36669016195116, 0.0));
    positionAlloc4->Add (Vector(406.43759006024106,247.9271553240719, 0.0));
    positionAlloc4->Add (Vector(334.1497067460107,110.73267845837692, 0.0));
    positionAlloc4->Add (Vector(457.9111123358251,356.9525583772417, 0.0));
    positionAlloc4->Add (Vector(304.3599856810048,186.30941854720402, 0.0));
    positionAlloc4->Add (Vector(357.0197642479603,295.4835705422414, 0.0));
    positionAlloc4->Add (Vector(429.25469824023554,73.20117847249196, 0.0));
    positionAlloc4->Add (Vector(190.51665968996323,272.8952448386851, 0.0));
    positionAlloc4->Add (Vector(409.5975474651599,53.04090240153098, 0.0));
    positionAlloc4->Add (Vector(378.8596358596872,355.6154974648217, 0.0));
    positionAlloc4->Add (Vector(468.1896916120768,441.62579855034403, 0.0));
    positionAlloc4->Add (Vector(242.3113169221652,295.2945676922979, 0.0));
    positionAlloc4->Add (Vector(284.33409921394576,449.23063896581846, 0.0));
    positionAlloc4->Add (Vector(406.2422722451032,201.58587930107885, 0.0));
    positionAlloc4->Add (Vector(224.8607114826689,280.74992188012055, 0.0));
    positionAlloc4->Add (Vector(261.8724766299086,445.5675718861025, 0.0));
    positionAlloc4->Add (Vector(261.73931580421237,446.95013752034345, 0.0));
    positionAlloc4->Add (Vector(131.3599959992628,406.502894887974, 0.0));
    positionAlloc4->Add (Vector(0.14612426529958533,291.30812787195794, 0.0));
    positionAlloc4->Add (Vector(452.31184704319725,435.80158740465964, 0.0));
    positionAlloc4->Add (Vector(372.5702115668959,488.8484159231387, 0.0));
    positionAlloc4->Add (Vector(291.180678196433,153.60447387426467, 0.0));
    positionAlloc4->Add (Vector(304.6870157754579,443.03333354219177, 0.0));
    positionAlloc4->Add (Vector(360.28747053871746,65.25787336697691, 0.0));
    positionAlloc4->Add (Vector(198.24781978684675,349.6649538685551, 0.0));
    positionAlloc4->Add (Vector(350.4743913119028,111.67528875063432, 0.0));
    positionAlloc4->Add (Vector(161.6942200164293,345.84521672309546, 0.0));
    positionAlloc4->Add (Vector(206.4580595105839,107.92408651507806, 0.0));
    positionAlloc4->Add (Vector(299.12505428035627,429.02336136522655, 0.0));
    positionAlloc4->Add (Vector(90.73046205887536,132.28087364072715, 0.0));
    positionAlloc4->Add (Vector(347.56026102679306,301.2032748759028, 0.0));
    positionAlloc4->Add (Vector(112.66307070296105,212.87191251144344, 0.0));
    positionAlloc4->Add (Vector(168.99864166437624,150.87955982927315, 0.0));
    positionAlloc4->Add (Vector(81.9617602306908,495.2303811653097, 0.0));
    positionAlloc4->Add (Vector(374.50262575546094,226.337917502681, 0.0));
    positionAlloc4->Add (Vector(222.35190148331247,138.56029958839773, 0.0));
    positionAlloc4->Add (Vector(180.8068425186272,162.68748805584542, 0.0));
    positionAlloc4->Add (Vector(167.87725318978934,410.0853522370237, 0.0));
    positionAlloc4->Add (Vector(453.99059062973083,148.44356399160802, 0.0));
    positionAlloc4->Add (Vector(350.3821082307704,497.8016870196094, 0.0));
    positionAlloc4->Add (Vector(254.30752606779416,249.82848950924404, 0.0));
    positionAlloc4->Add (Vector(470.72303580841776,32.573627103731795, 0.0));
    positionAlloc4->Add (Vector(377.8979783921969,399.82530599791465, 0.0));
    positionAlloc4->Add (Vector(349.7062144596283,340.5029965728467, 0.0));
    positionAlloc4->Add (Vector(407.7461459593665,68.00116062912126, 0.0));
    positionAlloc4->Add (Vector(346.15102331422406,438.3183859185081, 0.0));
    positionAlloc4->Add (Vector(149.08914478302253,465.7409693326794, 0.0));
    positionAlloc4->Add (Vector(432.5570720975732,314.87478268796355, 0.0));
    positionAlloc4->Add (Vector(401.1982478838523,325.5075290896064, 0.0));
    positionAlloc4->Add (Vector(476.48320188034916,56.16348680431443, 0.0));
    positionAlloc4->Add (Vector(119.6427608440262,36.53986274057108, 0.0));
    positionAlloc4->Add (Vector(294.83094363226894,417.63809049584444, 0.0));
    positionAlloc4->Add (Vector(273.5099929579202,439.1952677640986, 0.0));
    positionAlloc4->Add (Vector(490.1186910438823,103.86485913127619, 0.0));
    positionAlloc4->Add (Vector(123.52170693562131,268.8768207060506, 0.0));
    positionAlloc4->Add (Vector(129.50371535479044,54.464017023465594, 0.0));
    positionAlloc4->Add (Vector(470.9998874482461,146.65597907876997, 0.0));
    positionAlloc4->Add (Vector(428.3296391167992,199.20305606851468, 0.0));
    positionAlloc4->Add (Vector(224.21842185638457,239.86305075757636, 0.0));
    positionAlloc4->Add (Vector(469.9421625579711,453.3946804775568, 0.0));
    positionAlloc4->Add (Vector(442.21659210485717,287.45124508048065, 0.0));
    positionAlloc4->Add (Vector(422.9733661963505,363.6003955212138, 0.0));
    positionAlloc4->Add (Vector(190.87616327419366,383.48506844622545, 0.0));
    positionAlloc4->Add (Vector(37.6046413564195,184.01428006131965, 0.0));
    positionAlloc4->Add (Vector(16.66410247746547,10.591851686635124, 0.0));
    positionAlloc4->Add (Vector(24.644286149621497,310.4479561423246, 0.0));
    positionAlloc4->Add (Vector(54.69756046562901,264.83866764470605, 0.0));
    positionAlloc4->Add (Vector(233.86398599205026,67.3103984463157, 0.0));
    positionAlloc4->Add (Vector(367.8814845482172,441.9218615581187, 0.0));
    positionAlloc4->Add (Vector(125.00644569303054,296.4247278130615, 0.0));
    positionAlloc4->Add (Vector(435.61615993682045,412.3531613590305, 0.0));
    positionAlloc4->Add (Vector(337.655240995043,94.99685272827108, 0.0));
    positionAlloc4->Add (Vector(455.23837357595386,184.59539716649942, 0.0));
    positionAlloc4->Add (Vector(80.48063802638455,401.4288689384033, 0.0));
    positionAlloc4->Add (Vector(396.9414695824662,21.967926455911112, 0.0));
    positionAlloc4->Add (Vector(57.64217499770946,386.21968950047454, 0.0));
    positionAlloc4->Add (Vector(117.10776669505313,149.79458592725996, 0.0));
    positionAlloc4->Add (Vector(186.60996651586186,230.15656711864213, 0.0));
    positionAlloc4->Add (Vector(187.9790938877506,313.45097952665077, 0.0));
    positionAlloc4->Add (Vector(124.95861354980886,256.14728673871673, 0.0));
    positionAlloc4->Add (Vector(288.6745139864255,28.470996322347787, 0.0));
    positionAlloc4->Add (Vector(234.572069317523,174.89984803923542, 0.0));
    positionAlloc4->Add (Vector(108.3827276930126,48.66158178812935, 0.0));
    positionAlloc4->Add (Vector(330.36165092499294,130.39405945401793, 0.0));
    positionAlloc4->Add (Vector(219.6460753637416,437.36320720396606, 0.0));
    positionAlloc4->Add (Vector(29.42532080335758,267.93140686942064, 0.0));
    positionAlloc4->Add (Vector(325.25999569715634,227.67001004412128, 0.0));
    positionAlloc4->Add (Vector(316.2236933369802,85.9448758883195, 0.0));
    positionAlloc4->Add (Vector(337.74918391856176,422.58174002239946, 0.0));
    positionAlloc4->Add (Vector(312.7359994240711,379.05591088235053, 0.0));
    positionAlloc4->Add (Vector(449.0968073329478,438.6319456085391, 0.0));
    positionAlloc4->Add (Vector(220.61919044272187,428.77698485594794, 0.0));
    positionAlloc4->Add (Vector(432.28808056055766,64.05669296014771, 0.0));
    positionAlloc4->Add (Vector(468.26702491356184,309.60443973061626, 0.0));
    positionAlloc4->Add (Vector(223.26140691747509,117.81857428818977, 0.0));
    positionAlloc4->Add (Vector(271.1093727770866,427.68014420061826, 0.0));
    positionAlloc4->Add (Vector(16.997915300274393,7.68587257407527, 0.0));
    positionAlloc4->Add (Vector(89.61915353123601,127.93921517585721, 0.0));
    positionAlloc4->Add (Vector(256.77038808346623,456.85792389169575, 0.0));
    positionAlloc4->Add (Vector(123.93513628563002,55.92069033057157, 0.0));
    positionAlloc4->Add (Vector(277.93363111431734,459.03459916019636, 0.0));
    positionAlloc4->Add (Vector(354.857871429745,152.7718966764274, 0.0));
    positionAlloc4->Add (Vector(300.04269230388667,144.42301354437691, 0.0));
    positionAlloc4->Add (Vector(401.0382307663971,1.2713072150457028, 0.0));
    positionAlloc4->Add (Vector(452.7975578186324,493.4391610554169, 0.0));
    positionAlloc4->Add (Vector(266.9423479793265,394.4157746099447, 0.0));
    positionAlloc4->Add (Vector(371.2356743648028,210.35226512665722, 0.0));
    positionAlloc4->Add (Vector(329.35698686826544,7.386909467553715, 0.0));
    positionAlloc4->Add (Vector(286.80904195370215,23.30780090915996, 0.0));
    positionAlloc4->Add (Vector(220.37719303862647,480.65653599747503, 0.0));
    positionAlloc4->Add (Vector(167.97955224590893,484.43708318867726, 0.0));
    positionAlloc4->Add (Vector(40.2557414165724,110.5975909774785, 0.0));
    positionAlloc4->Add (Vector(166.4714991960249,263.5408901893687, 0.0));
    positionAlloc4->Add (Vector(256.047157587064,233.05212977769912, 0.0));
    positionAlloc4->Add (Vector(411.6742240295951,144.05781414569336, 0.0));
    positionAlloc4->Add (Vector(496.3718093417203,213.38036479024774, 0.0));
    positionAlloc4->Add (Vector(39.27671396924759,168.4422104281373, 0.0));
    positionAlloc4->Add (Vector(477.9081950824834,497.7992117548529, 0.0));
    positionAlloc4->Add (Vector(487.73339152463035,76.54301342221653, 0.0));
    positionAlloc4->Add (Vector(374.88438719707773,261.64768943564513, 0.0));
    positionAlloc4->Add (Vector(437.84454437431714,347.0105160510096, 0.0));
    positionAlloc4->Add (Vector(322.50101735144364,3.641920470287552, 0.0));
    positionAlloc4->Add (Vector(39.37385889726963,211.81169106705207, 0.0));
    positionAlloc4->Add (Vector(312.59407345095786,402.7003655893404, 0.0));
    positionAlloc4->Add (Vector(240.3487449016233,143.35139177596994, 0.0));
    positionAlloc4->Add (Vector(202.54315542315493,299.88789734784604, 0.0));
    positionAlloc4->Add (Vector(146.79778819116558,245.61837091720028, 0.0));
    positionAlloc4->Add (Vector(490.0658584530201,274.6311333408231, 0.0));
    positionAlloc4->Add (Vector(484.4552354989197,295.393447414288, 0.0));
    positionAlloc4->Add (Vector(326.80393160133855,437.9188442370486, 0.0));
    positionAlloc4->Add (Vector(459.00833747737653,425.322654727618, 0.0));
    positionAlloc4->Add (Vector(273.3147552697664,131.4112077464354, 0.0));
    positionAlloc4->Add (Vector(173.76700425289528,53.766265519994136, 0.0));
    positionAlloc4->Add (Vector(364.87827548697385,197.8047740070187, 0.0));
    positionAlloc4->Add (Vector(36.872048106874836,336.13402890874283, 0.0));
    positionAlloc4->Add (Vector(440.0827395950819,268.1749190468458, 0.0));
    positionAlloc4->Add (Vector(140.21317043697462,169.62785166760764, 0.0));
    positionAlloc4->Add (Vector(187.44847177546237,223.54673564925437, 0.0));
    positionAlloc4->Add (Vector(383.34464734001125,291.05126147707074, 0.0));
    positionAlloc4->Add (Vector(360.8894199761112,339.2823064654257, 0.0));
    positionAlloc4->Add (Vector(226.36361213621308,395.9136992440529, 0.0));
    positionAlloc4->Add (Vector(375.6828584353431,39.94352630416831, 0.0));
    positionAlloc4->Add (Vector(83.66488368858244,297.8817758097433, 0.0));
    positionAlloc4->Add (Vector(269.4758343884588,489.3620819650729, 0.0));
    positionAlloc4->Add (Vector(436.43595718674777,86.48495443929511, 0.0));
    positionAlloc4->Add (Vector(51.17176359296799,27.591054984568707, 0.0));
    positionAlloc4->Add (Vector(186.03027243218347,116.92834112984369, 0.0));
    positionAlloc4->Add (Vector(208.14716805851924,267.19818655869267, 0.0));
    positionAlloc4->Add (Vector(113.95789004857771,452.67831287634374, 0.0));
    positionAlloc4->Add (Vector(371.87358698734437,164.43228286065926, 0.0));
    positionAlloc4->Add (Vector(123.53929716452583,127.16492910935062, 0.0));
    positionAlloc4->Add (Vector(294.928219479729,186.78416777075395, 0.0));
    positionAlloc4->Add (Vector(405.9578702597038,77.30560214426325, 0.0));
    positionAlloc4->Add (Vector(350.10473107649466,226.15153751606582, 0.0));
    positionAlloc4->Add (Vector(184.68205086427602,184.58844203525675, 0.0));
    positionAlloc4->Add (Vector(289.76493296072476,384.4205770932571, 0.0));
    positionAlloc4->Add (Vector(381.75799272048755,347.6854640338086, 0.0));
    positionAlloc4->Add (Vector(189.29512587478575,95.7088650910527, 0.0));
    positionAlloc4->Add (Vector(243.53499784907356,45.89879965619287, 0.0));
    positionAlloc4->Add (Vector(348.28586370099396,426.95147306740546, 0.0));
    positionAlloc4->Add (Vector(355.137280430327,430.56422792451656, 0.0));
    positionAlloc4->Add (Vector(368.20390297283717,225.8788217783661, 0.0));
    positionAlloc4->Add (Vector(434.3336126481882,62.3966964298665, 0.0));
    positionAlloc4->Add (Vector(274.624830318433,22.514984480864364, 0.0));
    positionAlloc4->Add (Vector(283.97097676417155,315.44494622357826, 0.0));
    positionAlloc4->Add (Vector(237.7534231672448,456.0616164747356, 0.0));
    positionAlloc4->Add (Vector(171.0670666031326,90.14973577002728, 0.0));
    positionAlloc4->Add (Vector(125.41482373342244,252.01218499782595, 0.0));
    positionAlloc4->Add (Vector(313.9468500162991,357.8988485317571, 0.0));
    positionAlloc4->Add (Vector(129.05277477989713,349.003541283489, 0.0));
    positionAlloc4->Add (Vector(84.3647361310465,398.2188341302569, 0.0));
    positionAlloc4->Add (Vector(59.809035480320794,327.13036581294443, 0.0));
    positionAlloc4->Add (Vector(88.03099058346248,467.15104018280766, 0.0));
    positionAlloc4->Add (Vector(253.87052672368182,176.24968610814406, 0.0));
    positionAlloc4->Add (Vector(183.62579109675252,17.94749284468339, 0.0));
    positionAlloc4->Add (Vector(387.00910500208306,491.7776438738551, 0.0));
    positionAlloc4->Add (Vector(391.91914025926695,216.0193535211506, 0.0));
    positionAlloc4->Add (Vector(430.08646845810256,468.63870563407755, 0.0));
    positionAlloc4->Add (Vector(312.9560244085497,429.4922273386746, 0.0));
    positionAlloc4->Add (Vector(243.2347042259192,2.8624964632311123, 0.0));
    positionAlloc4->Add (Vector(386.5941565886775,54.986268344491776, 0.0));
    positionAlloc4->Add (Vector(59.63750744014662,369.78074660227185, 0.0));
    positionAlloc4->Add (Vector(338.6374619570501,384.169814212524, 0.0));
    positionAlloc4->Add (Vector(125.4873025948513,33.09288634162094, 0.0));
    positionAlloc4->Add (Vector(338.0758859656063,301.8624045273349, 0.0));
    positionAlloc4->Add (Vector(70.76331556837317,309.46465463194716, 0.0));
    positionAlloc4->Add (Vector(243.57806375264624,7.795163219272094, 0.0));
    positionAlloc4->Add (Vector(226.17657220327564,34.86184102854312, 0.0));
    positionAlloc4->Add (Vector(281.126887941033,43.22831890211598, 0.0));
    positionAlloc4->Add (Vector(494.1830950171666,321.7677920103427, 0.0));
    positionAlloc4->Add (Vector(244.28139667732785,211.85604168422344, 0.0));
    positionAlloc4->Add (Vector(13.93600030793396,233.54346560065554, 0.0));
    positionAlloc4->Add (Vector(87.91133206474188,86.3150227886647, 0.0));
    positionAlloc4->Add (Vector(328.71329030712786,141.13435635051368, 0.0));
    positionAlloc4->Add (Vector(124.43990385663788,132.75768229860958, 0.0));
    positionAlloc4->Add (Vector(464.206038686764,459.5568072750312, 0.0));
    positionAlloc4->Add (Vector(320.4551530842249,281.82390574955576, 0.0));
    positionAlloc4->Add (Vector(402.5937661193931,16.782077031138876, 0.0));
    positionAlloc4->Add (Vector(288.1969186204068,7.686903134306755, 0.0));
    positionAlloc4->Add (Vector(443.9798927666674,144.0643356074038, 0.0));
    positionAlloc4->Add (Vector(393.6230003036758,199.471558284885, 0.0));
    positionAlloc4->Add (Vector(455.26451774845526,121.12484256378825, 0.0));
    positionAlloc4->Add (Vector(371.13942586940317,130.21344993125155, 0.0));
    positionAlloc4->Add (Vector(30.738828248053185,155.2735594359792, 0.0));
    positionAlloc4->Add (Vector(383.90703499922216,417.96506872014567, 0.0));
    positionAlloc4->Add (Vector(380.27033259272434,196.10387908579037, 0.0));
    positionAlloc4->Add (Vector(352.4025566242049,471.5701211877593, 0.0));
    positionAlloc4->Add (Vector(50.23843183240684,73.48668858567476, 0.0));
    positionAlloc4->Add (Vector(177.15510501383957,44.88708486327886, 0.0));
    positionAlloc4->Add (Vector(36.238826885368056,449.5041349194627, 0.0));
    positionAlloc4->Add (Vector(162.88565918920435,355.28433870319026, 0.0));
    positionAlloc4->Add (Vector(178.08070558458306,39.8795334124809, 0.0));
    positionAlloc4->Add (Vector(309.3631284402032,22.998850356997114, 0.0));
    positionAlloc4->Add (Vector(352.39086122625804,294.4483834938858, 0.0));
    positionAlloc4->Add (Vector(463.3545398708128,343.96667214045175, 0.0));
    positionAlloc4->Add (Vector(119.53704613002641,103.95740217671562, 0.0));
    positionAlloc4->Add (Vector(483.5870356742176,249.43297666441177, 0.0));
    positionAlloc4->Add (Vector(127.84338459109934,235.3962393206948, 0.0));
    positionAlloc4->Add (Vector(187.03576672483484,170.73048185113677, 0.0));
    positionAlloc4->Add (Vector(447.5411728260811,463.2239062687702, 0.0));
    positionAlloc4->Add (Vector(41.787311570483546,379.6853782952796, 0.0));
    positionAlloc4->Add (Vector(438.8426657559295,129.0055106767482, 0.0));
    positionAlloc4->Add (Vector(392.33537104645256,167.34012803156122, 0.0));
    positionAlloc4->Add (Vector(260.6557794193871,357.44386785823053, 0.0));
    positionAlloc4->Add (Vector(68.52404339853646,454.36923181800927, 0.0));
    positionAlloc4->Add (Vector(194.25540324983277,480.74193904964903, 0.0));
    positionAlloc4->Add (Vector(478.7917322888638,439.87539827878413, 0.0));
    positionAlloc4->Add (Vector(290.2150679287355,403.7822137805366, 0.0));
    positionAlloc4->Add (Vector(411.7403147399117,216.54766260266823, 0.0));
    positionAlloc4->Add (Vector(340.6991488298683,404.3551740207635, 0.0));
    positionAlloc4->Add (Vector(14.508638958293464,60.98302983091841, 0.0));
    positionAlloc4->Add (Vector(453.58849531535196,278.80140758257954, 0.0));
    positionAlloc4->Add (Vector(156.4286900707025,107.18823087683337, 0.0));
    positionAlloc4->Add (Vector(248.36469319707112,342.6920102355436, 0.0));
    positionAlloc4->Add (Vector(264.6323100819877,329.0134266787825, 0.0));
    positionAlloc4->Add (Vector(200.16767303943462,279.1303106083554, 0.0));
    positionAlloc4->Add (Vector(122.78148727008353,233.74980570344871, 0.0));
    positionAlloc4->Add (Vector(390.3713933670103,493.25822785262227, 0.0));
    positionAlloc4->Add (Vector(120.8298890336838,179.83712073908936, 0.0));
    positionAlloc4->Add (Vector(2.250271432662432,349.924382216521, 0.0));
    positionAlloc4->Add (Vector(426.7261447440139,382.1647205399045, 0.0));
    positionAlloc4->Add (Vector(132.6723712712813,23.5885993320315, 0.0));
    positionAlloc4->Add (Vector(308.67517551464294,397.7188112241813, 0.0));
    positionAlloc4->Add (Vector(97.26805713340458,404.4263882249159, 0.0));
    positionAlloc4->Add (Vector(312.21438091978825,119.03460358188872, 0.0));
    positionAlloc4->Add (Vector(169.46677208111015,8.069226665730401, 0.0));
    positionAlloc4->Add (Vector(153.76523782729302,328.71219386692485, 0.0));
    positionAlloc4->Add (Vector(226.17257131480727,441.1744410659859, 0.0));
    positionAlloc4->Add (Vector(51.75836916282029,96.49098091550078, 0.0));
    positionAlloc4->Add (Vector(61.37112829561331,460.8164755033772, 0.0));
    positionAlloc4->Add (Vector(137.98365920086198,447.7924614958556, 0.0));
    positionAlloc4->Add (Vector(162.04714484175543,360.01683305318346, 0.0));
    positionAlloc4->Add (Vector(250.45565190030678,233.47300575486503, 0.0));
    positionAlloc4->Add (Vector(95.55910686294943,120.44228136967212, 0.0));
    positionAlloc4->Add (Vector(332.2309644004923,335.58699337581, 0.0));
    positionAlloc4->Add (Vector(489.85926371276565,298.1384620419385, 0.0));
    positionAlloc4->Add (Vector(82.70735482169866,256.5166951607527, 0.0));
    positionAlloc4->Add (Vector(120.82365653575083,166.2201170185782, 0.0));
    positionAlloc4->Add (Vector(495.63327756453293,435.35715846489256, 0.0));
    positionAlloc4->Add (Vector(165.9025767614572,69.19160634995897, 0.0));
    positionAlloc4->Add (Vector(282.4312714388143,142.44189208024676, 0.0));
    positionAlloc4->Add (Vector(23.986226902399434,488.7302388978211, 0.0));
    positionAlloc4->Add (Vector(445.24382366822005,68.6564852735111, 0.0));
    positionAlloc4->Add (Vector(234.12537473400624,152.08244157670308, 0.0));
    positionAlloc4->Add (Vector(467.87299875383724,64.65796785691447, 0.0));
    positionAlloc4->Add (Vector(393.5777218224281,489.2384495669463, 0.0));
    positionAlloc4->Add (Vector(437.14497390798454,405.1164570897767, 0.0));
    positionAlloc4->Add (Vector(101.02227774904138,107.50435385735074, 0.0));
    positionAlloc4->Add (Vector(35.05624644303501,403.53015727340556, 0.0));
    positionAlloc4->Add (Vector(364.2711293102391,289.22189525188077, 0.0));
    positionAlloc4->Add (Vector(249.24297414329598,210.87914406026954, 0.0));
    positionAlloc4->Add (Vector(383.73775055146194,448.7665656664403, 0.0));
    positionAlloc4->Add (Vector(440.24513252458286,325.946248538537, 0.0));
    positionAlloc4->Add (Vector(197.0417751418861,407.57263350681757, 0.0));
    positionAlloc4->Add (Vector(114.620599638171,459.4730932193227, 0.0));
    positionAlloc4->Add (Vector(356.15808729595364,172.7154976226326, 0.0));
    positionAlloc4->Add (Vector(269.081629028722,257.7639930121562, 0.0));
    positionAlloc4->Add (Vector(255.84519629681273,210.5165886576838, 0.0));
    positionAlloc4->Add (Vector(228.5916946333066,104.78188620126089, 0.0));
    positionAlloc4->Add (Vector(289.44718947056197,207.82694052894047, 0.0));
    positionAlloc4->Add (Vector(232.89351849025303,129.4876979250435, 0.0));
    positionAlloc4->Add (Vector(331.3081041348128,194.71766881430585, 0.0));
    positionAlloc4->Add (Vector(169.01448583030933,130.42326739691552, 0.0));
    positionAlloc4->Add (Vector(226.78217250504795,126.41905544080512, 0.0));
    positionAlloc4->Add (Vector(98.23531543818032,175.2625604168932, 0.0));
    positionAlloc4->Add (Vector(249.1968162517179,302.3162929846008, 0.0));
    positionAlloc4->Add (Vector(67.24169601323143,17.512504755959945, 0.0));
    positionAlloc4->Add (Vector(48.95744721427153,160.2299379168506, 0.0));
    positionAlloc4->Add (Vector(221.66275900572919,103.13650916574596, 0.0));
    positionAlloc4->Add (Vector(186.75649600609768,106.62133011488028, 0.0));
    positionAlloc4->Add (Vector(328.95172402474634,66.745124846954, 0.0));
    positionAlloc4->Add (Vector(138.22877504655727,454.7110176154457, 0.0));
    positionAlloc4->Add (Vector(233.60422440638328,309.5319594416782, 0.0));
    positionAlloc4->Add (Vector(466.31826643777447,208.61430540539212, 0.0));
    positionAlloc4->Add (Vector(287.5605588766465,16.662633783949055, 0.0));
    positionAlloc4->Add (Vector(366.6225847908913,421.7054144664678, 0.0));
    positionAlloc4->Add (Vector(301.77979407575145,339.33276993762513, 0.0));
    positionAlloc4->Add (Vector(307.6708701526515,326.4231866234061, 0.0));
    positionAlloc4->Add (Vector(0.7618693436077795,239.81848964465257, 0.0));
    positionAlloc4->Add (Vector(463.12228203852254,117.08159142229214, 0.0));
    positionAlloc4->Add (Vector(38.289453902597636,416.8360107122173, 0.0));
    positionAlloc4->Add (Vector(331.9399578957148,177.83580402505712, 0.0));
    positionAlloc4->Add (Vector(277.3258669823426,357.1453873956578, 0.0));
    positionAlloc4->Add (Vector(258.1939756675359,172.1031409260198, 0.0));
    positionAlloc4->Add (Vector(67.87271431656056,222.998675484528, 0.0));
    positionAlloc4->Add (Vector(146.30759304051776,256.70781993756026, 0.0));
    positionAlloc4->Add (Vector(47.82317008384529,230.09021960332427, 0.0));
    positionAlloc4->Add (Vector(193.3084908155626,382.79880943464684, 0.0));
    positionAlloc4->Add (Vector(142.4232593273579,333.98652828490117, 0.0));
    positionAlloc4->Add (Vector(49.92899325881067,115.06881895759491, 0.0));
    positionAlloc4->Add (Vector(349.34216410934357,462.65105765517393, 0.0));
    positionAlloc4->Add (Vector(182.210168596024,99.23662038650217, 0.0));
    positionAlloc4->Add (Vector(151.8123032517913,241.52424160770647, 0.0));
    positionAlloc4->Add (Vector(358.6424394773706,21.97624534794912, 0.0));
    positionAlloc4->Add (Vector(468.5759513679296,117.50196067814522, 0.0));
    positionAlloc4->Add (Vector(385.611856348896,426.86076307206054, 0.0));
    positionAlloc4->Add (Vector(146.71766307585455,438.73772575654743, 0.0));
    positionAlloc4->Add (Vector(344.72491866570675,338.4364637547374, 0.0));
    positionAlloc4->Add (Vector(12.70424663128461,224.13736425996152, 0.0));
    positionAlloc4->Add (Vector(305.1641998828108,246.02139830259534, 0.0));
    positionAlloc4->Add (Vector(455.09594669888594,210.11283076724274, 0.0));
    positionAlloc4->Add (Vector(338.92944333547024,375.07206985919487, 0.0));
    positionAlloc4->Add (Vector(3.0989175440790384,81.42612113304321, 0.0));
    positionAlloc4->Add (Vector(81.17270656121633,202.50482714215755, 0.0));
    positionAlloc4->Add (Vector(35.484916642711305,394.0713576161094, 0.0));
    positionAlloc4->Add (Vector(113.45676678476596,391.6865121236002, 0.0));
    positionAlloc4->Add (Vector(353.2230080238753,331.0546981560003, 0.0));
    positionAlloc4->Add (Vector(186.96556181438007,311.5399696365247, 0.0));
    positionAlloc4->Add (Vector(68.04937933310573,344.02820365004465, 0.0));
    positionAlloc4->Add (Vector(481.5813849323141,130.97565567272446, 0.0));
    positionAlloc4->Add (Vector(71.73596596832499,20.618898319769652, 0.0));
    positionAlloc4->Add (Vector(88.41707541386273,221.50169403685211, 0.0));
    positionAlloc4->Add (Vector(242.39556423533693,239.23692797541995, 0.0));
    positionAlloc4->Add (Vector(21.81603938814469,416.70592330008884, 0.0));
    positionAlloc4->Add (Vector(399.24358660733844,204.61697634791054, 0.0));
    positionAlloc4->Add (Vector(290.23109295719894,232.33053978852524, 0.0));
    positionAlloc4->Add (Vector(248.20320360256608,270.48153116662314, 0.0));
    positionAlloc4->Add (Vector(303.49398983631863,223.1556110025427, 0.0));
    positionAlloc4->Add (Vector(254.66892647793372,128.60306722182003, 0.0));
    positionAlloc4->Add (Vector(436.077263264442,199.79828678699624, 0.0));
    positionAlloc4->Add (Vector(445.9932251566162,410.692425517786, 0.0));
    positionAlloc4->Add (Vector(45.45565664945905,231.72423684445803, 0.0));
    positionAlloc4->Add (Vector(26.82469987658337,154.6650889747083, 0.0));
    positionAlloc4->Add (Vector(223.9426272271558,293.5135827334326, 0.0));
    positionAlloc4->Add (Vector(83.46420089917488,305.0916456241548, 0.0));
    positionAlloc4->Add (Vector(186.3818851006634,486.9436039610231, 0.0));
    positionAlloc4->Add (Vector(75.59234428223377,131.758942414752, 0.0));
    positionAlloc4->Add (Vector(95.63047795478036,134.6955700475956, 0.0));
    positionAlloc4->Add (Vector(46.6636736963158,495.4662553292345, 0.0));
    positionAlloc4->Add (Vector(457.02843364343823,393.17257436983607, 0.0));
    positionAlloc4->Add (Vector(288.7109771482202,242.79036456256358, 0.0));
    positionAlloc4->Add (Vector(226.27472254488978,275.41693591759133, 0.0));
    positionAlloc4->Add (Vector(469.26308245255024,436.1755756112605, 0.0));
    positionAlloc4->Add (Vector(365.5928738909089,447.3345152862242, 0.0));
    positionAlloc4->Add (Vector(188.15455919126782,334.5414255538215, 0.0));
    positionAlloc4->Add (Vector(214.5011600699484,424.7599374771595, 0.0));
    positionAlloc4->Add (Vector(131.03266316191852,153.7685156149488, 0.0));
    positionAlloc4->Add (Vector(98.40521615307807,455.19545760024414, 0.0));
    positionAlloc4->Add (Vector(5.6558913554360934,413.9150996562381, 0.0));
    positionAlloc4->Add (Vector(330.65050502501634,129.18170886718295, 0.0));
    positionAlloc4->Add (Vector(37.757609254525924,462.34938817953486, 0.0));
    positionAlloc4->Add (Vector(496.22685310960526,286.6555985858365, 0.0));
    positionAlloc4->Add (Vector(437.86000511970434,63.16454308527247, 0.0));
    positionAlloc4->Add (Vector(106.30876861999793,290.28059800296677, 0.0));
    positionAlloc4->Add (Vector(355.73121676398387,254.75868402766372, 0.0));
    positionAlloc4->Add (Vector(133.3934070654141,149.85329860898682, 0.0));
    positionAlloc4->Add (Vector(305.6254061514083,100.89897595654406, 0.0));
    positionAlloc4->Add (Vector(209.5061761646842,473.4786742171172, 0.0));
    positionAlloc4->Add (Vector(240.30300168866336,190.76435708792056, 0.0));
    positionAlloc4->Add (Vector(95.63639056556889,300.8928249503721, 0.0));
    positionAlloc4->Add (Vector(249.26843021251938,120.89563582347951, 0.0));
    positionAlloc4->Add (Vector(200.16848800737975,99.6780712298877, 0.0));
    positionAlloc4->Add (Vector(152.27036689476907,43.57627444615125, 0.0));
    positionAlloc4->Add (Vector(359.27224063803885,23.421098660187432, 0.0));
    positionAlloc4->Add (Vector(452.7681685060741,242.01358513833537, 0.0));
    positionAlloc4->Add (Vector(304.374266589062,377.020114167555, 0.0));
    positionAlloc4->Add (Vector(111.95612066615811,27.517858954994033, 0.0));
    positionAlloc4->Add (Vector(48.06570551351263,440.73701810063756, 0.0));
    positionAlloc4->Add (Vector(123.26898923666008,44.44060421168577, 0.0));
    positionAlloc4->Add (Vector(441.6474229119453,370.4850578970693, 0.0));
    positionAlloc4->Add (Vector(123.28384706041501,499.1945851707558, 0.0));
    positionAlloc4->Add (Vector(145.12843307531026,197.55911748800165, 0.0));
    positionAlloc4->Add (Vector(312.2565837605629,263.32619540945365, 0.0));
    positionAlloc4->Add (Vector(20.59466680228622,97.20617435415669, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_14.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_ECC_flowmon/switch_ECC_14.flowmon", false, false);
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