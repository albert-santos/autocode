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
    positionAlloc->Add (Vector (500.0,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (500.0,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (500.0,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (500.0,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,500.0, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,500.0, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,0.0, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,55.55555555555556, 0.0));
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
    positionAlloc->Add (Vector (222.22222222222223,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,0.0, 0.0));
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
    positionAlloc->Add (Vector (55.55555555555556,500.0, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (0.0,500.0, 0.0));
    positionAlloc->Add (Vector (0.0,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (0.0,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (0.0,166.66666666666666, 0.0));
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
    positionAlloc4->Add (Vector(440.3193245962028,321.74022134366584, 0.0));
    positionAlloc4->Add (Vector(296.6650082279741,69.99816773934813, 0.0));
    positionAlloc4->Add (Vector(126.3370912194961,17.389822594719107, 0.0));
    positionAlloc4->Add (Vector(461.10671615579946,380.0063332394759, 0.0));
    positionAlloc4->Add (Vector(464.58920347078714,41.47026816978905, 0.0));
    positionAlloc4->Add (Vector(67.66601290670432,408.6722764216306, 0.0));
    positionAlloc4->Add (Vector(336.92750872381026,363.5678337891292, 0.0));
    positionAlloc4->Add (Vector(371.09780613644017,495.2944220643756, 0.0));
    positionAlloc4->Add (Vector(365.9892605560285,228.16668616800766, 0.0));
    positionAlloc4->Add (Vector(391.8654874498024,459.95117036356726, 0.0));
    positionAlloc4->Add (Vector(458.06206640632445,335.6983257157202, 0.0));
    positionAlloc4->Add (Vector(146.52279151941383,322.17957129622783, 0.0));
    positionAlloc4->Add (Vector(119.904003794141,221.16802695994764, 0.0));
    positionAlloc4->Add (Vector(404.8030094336657,189.3191332433934, 0.0));
    positionAlloc4->Add (Vector(404.41544045930556,285.21637879927766, 0.0));
    positionAlloc4->Add (Vector(379.6642466432854,486.62308160118323, 0.0));
    positionAlloc4->Add (Vector(52.20780001745201,108.81372340064421, 0.0));
    positionAlloc4->Add (Vector(204.56134189546293,434.53422201629184, 0.0));
    positionAlloc4->Add (Vector(90.70002926495063,258.0046582647075, 0.0));
    positionAlloc4->Add (Vector(465.673264919675,314.02802908714165, 0.0));
    positionAlloc4->Add (Vector(114.1555510736662,246.76338037868118, 0.0));
    positionAlloc4->Add (Vector(308.5838420886214,112.44996167742244, 0.0));
    positionAlloc4->Add (Vector(420.63397038880083,489.1652708867389, 0.0));
    positionAlloc4->Add (Vector(36.423224950621446,355.38101497446564, 0.0));
    positionAlloc4->Add (Vector(1.4605580424569276,142.1817754590416, 0.0));
    positionAlloc4->Add (Vector(161.70706118299654,330.7503434734759, 0.0));
    positionAlloc4->Add (Vector(172.1137430397681,36.57897576184821, 0.0));
    positionAlloc4->Add (Vector(232.65247339392565,47.62937196502071, 0.0));
    positionAlloc4->Add (Vector(157.02579577507885,167.19513212754373, 0.0));
    positionAlloc4->Add (Vector(37.00877971293587,94.34268931124373, 0.0));
    positionAlloc4->Add (Vector(359.4727758395227,385.2140696322937, 0.0));
    positionAlloc4->Add (Vector(413.0185672819538,462.42713395176327, 0.0));
    positionAlloc4->Add (Vector(456.29995831360395,341.9596560109303, 0.0));
    positionAlloc4->Add (Vector(418.09794907904,173.3200623532477, 0.0));
    positionAlloc4->Add (Vector(48.20526998685898,272.0525410219491, 0.0));
    positionAlloc4->Add (Vector(89.03621307330117,113.8841048413644, 0.0));
    positionAlloc4->Add (Vector(129.5893119034288,91.25124823287823, 0.0));
    positionAlloc4->Add (Vector(185.90851671103454,83.0629790153199, 0.0));
    positionAlloc4->Add (Vector(4.177503821415851,328.7280746067528, 0.0));
    positionAlloc4->Add (Vector(124.9778508918204,94.05598206176657, 0.0));
    positionAlloc4->Add (Vector(441.1936328787907,139.67752326177563, 0.0));
    positionAlloc4->Add (Vector(417.6202322326069,342.06414806502016, 0.0));
    positionAlloc4->Add (Vector(247.26737340079728,293.67969145801425, 0.0));
    positionAlloc4->Add (Vector(103.5297281172044,275.75905586180926, 0.0));
    positionAlloc4->Add (Vector(162.553060969588,344.4121994800832, 0.0));
    positionAlloc4->Add (Vector(464.5945495973283,211.96548366558116, 0.0));
    positionAlloc4->Add (Vector(29.88120809708128,376.0262221831405, 0.0));
    positionAlloc4->Add (Vector(438.05266720161774,137.19231408155113, 0.0));
    positionAlloc4->Add (Vector(141.5949000031958,15.732442403489033, 0.0));
    positionAlloc4->Add (Vector(481.0704143060104,128.21781906875768, 0.0));
    positionAlloc4->Add (Vector(118.3982084766827,268.18182874210754, 0.0));
    positionAlloc4->Add (Vector(258.31275688187185,203.09805185672357, 0.0));
    positionAlloc4->Add (Vector(83.9749845643371,247.81694042561048, 0.0));
    positionAlloc4->Add (Vector(443.7115989229337,107.41574402917381, 0.0));
    positionAlloc4->Add (Vector(284.2906963689813,224.76635341961438, 0.0));
    positionAlloc4->Add (Vector(223.52823722917574,79.30920829517075, 0.0));
    positionAlloc4->Add (Vector(129.63302767012053,141.8842988643476, 0.0));
    positionAlloc4->Add (Vector(366.14455032491674,474.8664099493792, 0.0));
    positionAlloc4->Add (Vector(266.6910350902925,63.31994735625529, 0.0));
    positionAlloc4->Add (Vector(463.8249457729356,482.207287382464, 0.0));
    positionAlloc4->Add (Vector(16.202363132828303,480.18528942854954, 0.0));
    positionAlloc4->Add (Vector(262.5527740252953,253.65586463819244, 0.0));
    positionAlloc4->Add (Vector(41.95584945310099,278.7910757864966, 0.0));
    positionAlloc4->Add (Vector(415.30522269028796,361.57766908453823, 0.0));
    positionAlloc4->Add (Vector(63.68715469818548,37.39036349225383, 0.0));
    positionAlloc4->Add (Vector(120.4470643415339,272.6173833365565, 0.0));
    positionAlloc4->Add (Vector(460.1152181760437,266.5504263338516, 0.0));
    positionAlloc4->Add (Vector(287.80679983634167,291.73654760188316, 0.0));
    positionAlloc4->Add (Vector(68.8166310902128,287.63668480975633, 0.0));
    positionAlloc4->Add (Vector(187.13081580602352,273.9270192179274, 0.0));
    positionAlloc4->Add (Vector(455.5157018026195,372.21027918280953, 0.0));
    positionAlloc4->Add (Vector(120.38654790495157,448.09858691929793, 0.0));
    positionAlloc4->Add (Vector(411.29746861087165,474.54686181973807, 0.0));
    positionAlloc4->Add (Vector(113.31915603989185,270.81897023500994, 0.0));
    positionAlloc4->Add (Vector(395.0106855988423,104.68754161980614, 0.0));
    positionAlloc4->Add (Vector(243.81921926181278,19.18028541004302, 0.0));
    positionAlloc4->Add (Vector(302.0620211038328,393.99075301179926, 0.0));
    positionAlloc4->Add (Vector(126.88690092073507,345.06721736886516, 0.0));
    positionAlloc4->Add (Vector(131.2901133983175,255.17386043980895, 0.0));
    positionAlloc4->Add (Vector(434.20858368758985,274.389399902982, 0.0));
    positionAlloc4->Add (Vector(62.336992941409484,447.5732142676615, 0.0));
    positionAlloc4->Add (Vector(300.3935981730157,26.316758579527257, 0.0));
    positionAlloc4->Add (Vector(297.7530222471955,499.14085567622607, 0.0));
    positionAlloc4->Add (Vector(491.7653465876733,29.804522851416326, 0.0));
    positionAlloc4->Add (Vector(10.31824857401098,113.7901120032212, 0.0));
    positionAlloc4->Add (Vector(235.5630780477037,385.7760833293919, 0.0));
    positionAlloc4->Add (Vector(176.76211636754218,135.8355034891611, 0.0));
    positionAlloc4->Add (Vector(143.1042173221441,71.72437798735143, 0.0));
    positionAlloc4->Add (Vector(233.66482769580426,2.65649719788813, 0.0));
    positionAlloc4->Add (Vector(70.22955831000577,256.3622334632075, 0.0));
    positionAlloc4->Add (Vector(163.7565853659238,291.1929207937837, 0.0));
    positionAlloc4->Add (Vector(64.54555655824822,453.5090398747875, 0.0));
    positionAlloc4->Add (Vector(486.6929213487431,428.88557502324375, 0.0));
    positionAlloc4->Add (Vector(287.04282071984983,453.4171441337296, 0.0));
    positionAlloc4->Add (Vector(469.91055316651165,94.69573879695164, 0.0));
    positionAlloc4->Add (Vector(303.26511770719173,38.10328877925223, 0.0));
    positionAlloc4->Add (Vector(313.36119643923195,367.41918698550285, 0.0));
    positionAlloc4->Add (Vector(62.19939075190622,173.83847157681697, 0.0));
    positionAlloc4->Add (Vector(446.4098718657922,170.8482947825139, 0.0));
    positionAlloc4->Add (Vector(101.80611010562856,319.38401054509285, 0.0));
    positionAlloc4->Add (Vector(142.29267915534876,180.76636941135638, 0.0));
    positionAlloc4->Add (Vector(307.51951067933453,484.39808770182555, 0.0));
    positionAlloc4->Add (Vector(393.4086074246728,96.38660971780166, 0.0));
    positionAlloc4->Add (Vector(141.93012122063408,470.5408324988286, 0.0));
    positionAlloc4->Add (Vector(195.98809987484444,325.71287796643344, 0.0));
    positionAlloc4->Add (Vector(105.34473515047237,445.86688546659843, 0.0));
    positionAlloc4->Add (Vector(419.81059891171304,481.0470274692895, 0.0));
    positionAlloc4->Add (Vector(148.8192843251775,141.75229037611848, 0.0));
    positionAlloc4->Add (Vector(222.26009307473475,237.37441369723544, 0.0));
    positionAlloc4->Add (Vector(496.04974417544514,101.43668774300319, 0.0));
    positionAlloc4->Add (Vector(290.07825366986043,485.58562139998105, 0.0));
    positionAlloc4->Add (Vector(405.31649922343,217.9522785684525, 0.0));
    positionAlloc4->Add (Vector(274.84173658675724,194.22969135561934, 0.0));
    positionAlloc4->Add (Vector(194.93381923621712,211.3348464371247, 0.0));
    positionAlloc4->Add (Vector(10.095970271090993,381.5667369312162, 0.0));
    positionAlloc4->Add (Vector(17.31743700582267,109.4021962995979, 0.0));
    positionAlloc4->Add (Vector(217.44788105352686,306.6497238866815, 0.0));
    positionAlloc4->Add (Vector(329.6995261007345,192.17997081878562, 0.0));
    positionAlloc4->Add (Vector(243.01754285582456,176.96421051578304, 0.0));
    positionAlloc4->Add (Vector(483.755934309476,289.0932386110048, 0.0));
    positionAlloc4->Add (Vector(79.85298368189736,472.6209732107472, 0.0));
    positionAlloc4->Add (Vector(14.970449407924246,423.6902439386126, 0.0));
    positionAlloc4->Add (Vector(421.2885376595954,82.37241420031182, 0.0));
    positionAlloc4->Add (Vector(135.50818491606347,382.2822431427777, 0.0));
    positionAlloc4->Add (Vector(190.8151651399182,125.89853189512267, 0.0));
    positionAlloc4->Add (Vector(456.98204676782683,484.772802656574, 0.0));
    positionAlloc4->Add (Vector(221.91057559812643,374.6776993452804, 0.0));
    positionAlloc4->Add (Vector(106.3350544797168,179.0731438336217, 0.0));
    positionAlloc4->Add (Vector(50.45552767353506,286.3656986579863, 0.0));
    positionAlloc4->Add (Vector(269.78470547064137,363.0173154408809, 0.0));
    positionAlloc4->Add (Vector(462.5194938960259,121.28022923211684, 0.0));
    positionAlloc4->Add (Vector(5.70962165754807,98.97836499182017, 0.0));
    positionAlloc4->Add (Vector(289.25362736669723,405.11953873678107, 0.0));
    positionAlloc4->Add (Vector(84.78814256049405,488.9795422727443, 0.0));
    positionAlloc4->Add (Vector(493.5882203745626,423.623357509237, 0.0));
    positionAlloc4->Add (Vector(284.3366119996666,385.7892309034176, 0.0));
    positionAlloc4->Add (Vector(69.60272420435099,18.177466742849756, 0.0));
    positionAlloc4->Add (Vector(82.22406143990835,13.041371992771833, 0.0));
    positionAlloc4->Add (Vector(253.9787139432843,452.25907420808875, 0.0));
    positionAlloc4->Add (Vector(100.05487905463228,278.42458935646835, 0.0));
    positionAlloc4->Add (Vector(188.41579562200235,157.67028968019093, 0.0));
    positionAlloc4->Add (Vector(89.45244375013544,370.26074038599006, 0.0));
    positionAlloc4->Add (Vector(398.968722346084,376.4319816918143, 0.0));
    positionAlloc4->Add (Vector(123.54530759695992,155.62552339459984, 0.0));
    positionAlloc4->Add (Vector(33.87680621716071,35.42450236289585, 0.0));
    positionAlloc4->Add (Vector(499.87689866183587,482.2161401041149, 0.0));
    positionAlloc4->Add (Vector(336.2132750649979,87.80471650206229, 0.0));
    positionAlloc4->Add (Vector(196.9918746009698,71.73911624444251, 0.0));
    positionAlloc4->Add (Vector(437.8977829641922,119.38361519275476, 0.0));
    positionAlloc4->Add (Vector(219.4157512696928,197.8548417173548, 0.0));
    positionAlloc4->Add (Vector(410.2992866717148,340.8037739598201, 0.0));
    positionAlloc4->Add (Vector(20.538650720054786,63.04060401931966, 0.0));
    positionAlloc4->Add (Vector(13.570128171807971,281.6475673858897, 0.0));
    positionAlloc4->Add (Vector(142.4511423629568,361.94117118068016, 0.0));
    positionAlloc4->Add (Vector(441.1768447769254,382.00204974019533, 0.0));
    positionAlloc4->Add (Vector(142.35484023594714,210.00311514262043, 0.0));
    positionAlloc4->Add (Vector(342.9717635390882,393.6088099571068, 0.0));
    positionAlloc4->Add (Vector(258.03932976555495,167.1203506334172, 0.0));
    positionAlloc4->Add (Vector(381.65987431349714,58.36237211513562, 0.0));
    positionAlloc4->Add (Vector(249.02672770394258,409.96255269979554, 0.0));
    positionAlloc4->Add (Vector(97.89932094413433,432.0458405283056, 0.0));
    positionAlloc4->Add (Vector(462.08892175507935,177.1588800201655, 0.0));
    positionAlloc4->Add (Vector(107.89755885085157,18.424640774878263, 0.0));
    positionAlloc4->Add (Vector(179.67249008855762,180.65460528510363, 0.0));
    positionAlloc4->Add (Vector(258.7643984283071,73.09609595373901, 0.0));
    positionAlloc4->Add (Vector(247.10254892574824,270.7646607508462, 0.0));
    positionAlloc4->Add (Vector(405.306445630257,158.21039115934565, 0.0));
    positionAlloc4->Add (Vector(196.96545982273435,30.26769348870345, 0.0));
    positionAlloc4->Add (Vector(457.00182336359654,368.2052040690868, 0.0));
    positionAlloc4->Add (Vector(371.0243277119964,278.80769030478876, 0.0));
    positionAlloc4->Add (Vector(474.16636904744274,214.65953279486516, 0.0));
    positionAlloc4->Add (Vector(168.0971820708516,474.9259512811266, 0.0));
    positionAlloc4->Add (Vector(179.668682200202,27.739417771067643, 0.0));
    positionAlloc4->Add (Vector(254.33875492060227,255.06148073213308, 0.0));
    positionAlloc4->Add (Vector(69.09943964220533,292.51594297258174, 0.0));
    positionAlloc4->Add (Vector(56.59681170284653,384.3028456573105, 0.0));
    positionAlloc4->Add (Vector(283.0211801703048,155.30401764047846, 0.0));
    positionAlloc4->Add (Vector(47.48161871346379,282.11319794262414, 0.0));
    positionAlloc4->Add (Vector(14.789530179608711,43.63532654803032, 0.0));
    positionAlloc4->Add (Vector(435.93494282491844,347.1461649718532, 0.0));
    positionAlloc4->Add (Vector(486.56730685475463,187.84372204158305, 0.0));
    positionAlloc4->Add (Vector(334.3388302458784,25.037118924303936, 0.0));
    positionAlloc4->Add (Vector(134.76805777775496,125.00101983141387, 0.0));
    positionAlloc4->Add (Vector(251.146211632786,32.65752817716494, 0.0));
    positionAlloc4->Add (Vector(263.19459578271665,206.1718054698421, 0.0));
    positionAlloc4->Add (Vector(390.6235759572949,57.98587484298895, 0.0));
    positionAlloc4->Add (Vector(127.5480387249589,328.16974087978207, 0.0));
    positionAlloc4->Add (Vector(285.4007059122435,465.26280212517423, 0.0));
    positionAlloc4->Add (Vector(224.21175852340426,286.3407442237064, 0.0));
    positionAlloc4->Add (Vector(415.212277739974,107.98964239835752, 0.0));
    positionAlloc4->Add (Vector(260.082282073305,487.77050553573116, 0.0));
    positionAlloc4->Add (Vector(336.24576607123856,105.96443138971745, 0.0));
    positionAlloc4->Add (Vector(402.67209614834223,214.15083753902042, 0.0));
    positionAlloc4->Add (Vector(53.57986706274392,476.8413528523534, 0.0));
    positionAlloc4->Add (Vector(288.37714000285064,43.679205360639294, 0.0));
    positionAlloc4->Add (Vector(194.76863601704113,477.18128640017886, 0.0));
    positionAlloc4->Add (Vector(53.11312084711489,149.48801284902274, 0.0));
    positionAlloc4->Add (Vector(313.4612628772586,322.20741091291313, 0.0));
    positionAlloc4->Add (Vector(489.94414780031576,75.73272367634199, 0.0));
    positionAlloc4->Add (Vector(24.124340143839294,294.21132914081545, 0.0));
    positionAlloc4->Add (Vector(422.5755869375655,245.78805322704306, 0.0));
    positionAlloc4->Add (Vector(460.0056233745104,245.57879891684505, 0.0));
    positionAlloc4->Add (Vector(103.79773010054294,166.54496695595756, 0.0));
    positionAlloc4->Add (Vector(453.24802128178106,288.9928236378556, 0.0));
    positionAlloc4->Add (Vector(301.39835486604613,421.694389842648, 0.0));
    positionAlloc4->Add (Vector(456.255249159058,355.2039221335061, 0.0));
    positionAlloc4->Add (Vector(310.92163364466006,220.853302576409, 0.0));
    positionAlloc4->Add (Vector(462.01602386431364,240.64199397330677, 0.0));
    positionAlloc4->Add (Vector(418.0797711222095,294.14739944607686, 0.0));
    positionAlloc4->Add (Vector(220.50477616126253,289.3378725302196, 0.0));
    positionAlloc4->Add (Vector(186.3479427272116,91.25426268744746, 0.0));
    positionAlloc4->Add (Vector(86.87266551117479,85.7676312760769, 0.0));
    positionAlloc4->Add (Vector(9.389535969884843,435.35387318008526, 0.0));
    positionAlloc4->Add (Vector(141.9157061671781,70.95069293739581, 0.0));
    positionAlloc4->Add (Vector(32.005493488434844,493.4050942113884, 0.0));
    positionAlloc4->Add (Vector(290.40868458658764,47.17302870125067, 0.0));
    positionAlloc4->Add (Vector(233.0220368639953,271.5695560826406, 0.0));
    positionAlloc4->Add (Vector(96.34190963547034,419.5764545002675, 0.0));
    positionAlloc4->Add (Vector(107.44773158488819,110.7553308745709, 0.0));
    positionAlloc4->Add (Vector(11.88133681688397,144.2854865316659, 0.0));
    positionAlloc4->Add (Vector(52.42247942682398,221.9696686012067, 0.0));
    positionAlloc4->Add (Vector(203.4246423712111,25.089170322576628, 0.0));
    positionAlloc4->Add (Vector(467.9980189031336,308.43453076629004, 0.0));
    positionAlloc4->Add (Vector(303.48107006123723,141.225625218986, 0.0));
    positionAlloc4->Add (Vector(390.50497740602935,255.11907940718586, 0.0));
    positionAlloc4->Add (Vector(312.1354794148996,411.4541609398578, 0.0));
    positionAlloc4->Add (Vector(41.74561065188659,489.7471681090074, 0.0));
    positionAlloc4->Add (Vector(241.94228723299844,37.83542227825271, 0.0));
    positionAlloc4->Add (Vector(5.445239576647943,146.2864077009582, 0.0));
    positionAlloc4->Add (Vector(300.74240677631315,65.40437245420638, 0.0));
    positionAlloc4->Add (Vector(363.88246466364075,404.9675673480796, 0.0));
    positionAlloc4->Add (Vector(98.40845828890494,197.94631109919447, 0.0));
    positionAlloc4->Add (Vector(83.16366104138667,339.6418141375497, 0.0));
    positionAlloc4->Add (Vector(255.0828997565193,147.12727306221223, 0.0));
    positionAlloc4->Add (Vector(273.7291728240069,27.58497161379786, 0.0));
    positionAlloc4->Add (Vector(154.79584386064843,421.3917501011851, 0.0));
    positionAlloc4->Add (Vector(355.16584116528554,10.750182502726524, 0.0));
    positionAlloc4->Add (Vector(246.96825204066823,147.2254435059309, 0.0));
    positionAlloc4->Add (Vector(48.98064592010343,221.71193275329304, 0.0));
    positionAlloc4->Add (Vector(56.090962811163436,39.46787606851865, 0.0));
    positionAlloc4->Add (Vector(241.9578215735767,100.99299477497559, 0.0));
    positionAlloc4->Add (Vector(460.0946287504351,72.47910046712674, 0.0));
    positionAlloc4->Add (Vector(422.57559960113446,126.78156912860238, 0.0));
    positionAlloc4->Add (Vector(279.10352110962776,197.60552325326398, 0.0));
    positionAlloc4->Add (Vector(2.7456963252123767,135.01686929020025, 0.0));
    positionAlloc4->Add (Vector(171.25392703608173,142.7068312162785, 0.0));
    positionAlloc4->Add (Vector(181.7757516838912,354.34356449550035, 0.0));
    positionAlloc4->Add (Vector(32.68013614559312,426.5236571529194, 0.0));
    positionAlloc4->Add (Vector(15.15957196782719,187.17791035562908, 0.0));
    positionAlloc4->Add (Vector(414.2022549227784,11.144628368293663, 0.0));
    positionAlloc4->Add (Vector(2.8042517457261273,172.3922912289012, 0.0));
    positionAlloc4->Add (Vector(90.41885322206112,147.05780142857054, 0.0));
    positionAlloc4->Add (Vector(342.4962600370362,103.77078695021474, 0.0));
    positionAlloc4->Add (Vector(6.347605743590668,179.61543040947254, 0.0));
    positionAlloc4->Add (Vector(151.03952426092576,274.8802236876956, 0.0));
    positionAlloc4->Add (Vector(183.72168300456326,174.62067411282644, 0.0));
    positionAlloc4->Add (Vector(97.70584330111676,96.95247032021715, 0.0));
    positionAlloc4->Add (Vector(225.19023173892177,367.70036401590363, 0.0));
    positionAlloc4->Add (Vector(373.36042654108616,433.9113045976113, 0.0));
    positionAlloc4->Add (Vector(55.63913261566006,157.08182397111486, 0.0));
    positionAlloc4->Add (Vector(40.8846606480252,53.300417316112515, 0.0));
    positionAlloc4->Add (Vector(437.6862019160617,156.3083740601658, 0.0));
    positionAlloc4->Add (Vector(26.307091009795947,490.27456634880207, 0.0));
    positionAlloc4->Add (Vector(192.7875001774383,232.57204723590553, 0.0));
    positionAlloc4->Add (Vector(5.8735095060063465,272.5789710796035, 0.0));
    positionAlloc4->Add (Vector(154.71132081072696,183.17914767743747, 0.0));
    positionAlloc4->Add (Vector(459.361773344073,40.45819385520433, 0.0));
    positionAlloc4->Add (Vector(346.31161705550375,269.711297812042, 0.0));
    positionAlloc4->Add (Vector(264.7774338969798,469.7710184543054, 0.0));
    positionAlloc4->Add (Vector(77.46955236457148,284.65930311417645, 0.0));
    positionAlloc4->Add (Vector(458.99002585175396,361.8801137711851, 0.0));
    positionAlloc4->Add (Vector(488.84298095709465,222.0686021397366, 0.0));
    positionAlloc4->Add (Vector(381.75115523168984,492.74003457058984, 0.0));
    positionAlloc4->Add (Vector(198.8432497289369,315.355691894191, 0.0));
    positionAlloc4->Add (Vector(279.29387190366464,85.14714280009228, 0.0));
    positionAlloc4->Add (Vector(179.30476174665378,463.96605635121244, 0.0));
    positionAlloc4->Add (Vector(410.52724244469886,2.984944730120398, 0.0));
    positionAlloc4->Add (Vector(69.5029498571758,390.60645593743635, 0.0));
    positionAlloc4->Add (Vector(140.16415826741962,210.3866904910846, 0.0));
    positionAlloc4->Add (Vector(492.18984403795986,241.17107703779794, 0.0));
    positionAlloc4->Add (Vector(176.35420749866864,116.5124935199468, 0.0));
    positionAlloc4->Add (Vector(402.0284914687776,47.63174946611692, 0.0));
    positionAlloc4->Add (Vector(412.9707938195585,18.451983878394916, 0.0));
    positionAlloc4->Add (Vector(433.3363015334429,383.9645510642066, 0.0));
    positionAlloc4->Add (Vector(56.65642322767284,188.52606803189397, 0.0));
    positionAlloc4->Add (Vector(347.0301328734247,471.1587901302152, 0.0));
    positionAlloc4->Add (Vector(145.7881645429514,243.41519409895267, 0.0));
    positionAlloc4->Add (Vector(249.62935535137697,335.4554303539859, 0.0));
    positionAlloc4->Add (Vector(76.59217675890001,156.63702551905882, 0.0));
    positionAlloc4->Add (Vector(193.26207355263992,140.2419391456861, 0.0));
    positionAlloc4->Add (Vector(164.0224518855593,33.88986295323948, 0.0));
    positionAlloc4->Add (Vector(80.29886791416246,115.31020729910458, 0.0));
    positionAlloc4->Add (Vector(70.15654572429186,89.87272351440978, 0.0));
    positionAlloc4->Add (Vector(221.4259287291228,404.0516267510961, 0.0));
    positionAlloc4->Add (Vector(69.87584865649698,45.86588829904714, 0.0));
    positionAlloc4->Add (Vector(391.49522425963914,418.95020259920375, 0.0));
    positionAlloc4->Add (Vector(212.73997003434104,409.5793189759873, 0.0));
    positionAlloc4->Add (Vector(44.52961448088028,472.3553788873863, 0.0));
    positionAlloc4->Add (Vector(390.7432161341358,49.68863744330859, 0.0));
    positionAlloc4->Add (Vector(280.8038512760652,289.36073320994177, 0.0));
    positionAlloc4->Add (Vector(461.20923500813416,62.98517479342019, 0.0));
    positionAlloc4->Add (Vector(51.884545519425565,498.69335178648464, 0.0));
    positionAlloc4->Add (Vector(225.6822400329107,447.84305840196436, 0.0));
    positionAlloc4->Add (Vector(29.249104070759035,59.58894537308823, 0.0));
    positionAlloc4->Add (Vector(101.51558626248375,11.974916062885088, 0.0));
    positionAlloc4->Add (Vector(396.0947549514587,58.94688115593727, 0.0));
    positionAlloc4->Add (Vector(278.42162846298993,91.08637124044544, 0.0));
    positionAlloc4->Add (Vector(274.82062474430944,190.2827349452685, 0.0));
    positionAlloc4->Add (Vector(430.1449693129535,30.282279107085586, 0.0));
    positionAlloc4->Add (Vector(157.70874762079546,440.26786804993554, 0.0));
    positionAlloc4->Add (Vector(263.63346295758237,311.322462763181, 0.0));
    positionAlloc4->Add (Vector(499.3134514152171,460.65396982030853, 0.0));
    positionAlloc4->Add (Vector(112.02322430987749,376.3727323041986, 0.0));
    positionAlloc4->Add (Vector(92.48702390447167,361.16265021581177, 0.0));
    positionAlloc4->Add (Vector(466.9419724433715,324.3271393065111, 0.0));
    positionAlloc4->Add (Vector(62.34980547596219,427.8539534757308, 0.0));
    positionAlloc4->Add (Vector(283.6027476335704,55.484137097201845, 0.0));
    positionAlloc4->Add (Vector(364.931263989218,392.1776023153344, 0.0));
    positionAlloc4->Add (Vector(364.5686112450578,137.55520313388786, 0.0));
    positionAlloc4->Add (Vector(457.67731625592035,156.27926269506332, 0.0));
    positionAlloc4->Add (Vector(438.5101507820025,30.231930316131695, 0.0));
    positionAlloc4->Add (Vector(211.113153093247,253.94966267161712, 0.0));
    positionAlloc4->Add (Vector(176.55237487480142,288.42829869231264, 0.0));
    positionAlloc4->Add (Vector(492.3052987780495,77.16551262759558, 0.0));
    positionAlloc4->Add (Vector(215.43828933868946,479.438162983734, 0.0));
    positionAlloc4->Add (Vector(87.49585298280438,254.77769473174655, 0.0));
    positionAlloc4->Add (Vector(457.2230396828597,31.581096608604643, 0.0));
    positionAlloc4->Add (Vector(6.552553798196192,394.6214069432005, 0.0));
    positionAlloc4->Add (Vector(439.96642764198884,256.63126662747067, 0.0));
    positionAlloc4->Add (Vector(497.2096934592641,417.82437466639965, 0.0));
    positionAlloc4->Add (Vector(63.118770809822756,241.1054267170574, 0.0));
    positionAlloc4->Add (Vector(95.14961135685051,74.79803659211665, 0.0));
    positionAlloc4->Add (Vector(424.46663757921885,219.30725924043836, 0.0));
    positionAlloc4->Add (Vector(243.04565974530018,23.26522006124099, 0.0));
    positionAlloc4->Add (Vector(94.82917730209873,138.83728319371463, 0.0));
    positionAlloc4->Add (Vector(208.95425482398383,277.71826710892014, 0.0));
    positionAlloc4->Add (Vector(174.85546972199646,367.9073144152322, 0.0));
    positionAlloc4->Add (Vector(28.048375353193766,328.7606164750549, 0.0));
    positionAlloc4->Add (Vector(28.617137110052482,331.05944752538073, 0.0));
    positionAlloc4->Add (Vector(137.18049919370807,362.62718621622605, 0.0));
    positionAlloc4->Add (Vector(81.07753588004807,332.88250430201566, 0.0));
    positionAlloc4->Add (Vector(138.65518221496976,389.1516832227942, 0.0));
    positionAlloc4->Add (Vector(43.00845492447697,456.62738858598, 0.0));
    positionAlloc4->Add (Vector(202.59278826800985,91.90057446149646, 0.0));
    positionAlloc4->Add (Vector(463.9721038585321,122.22896401209677, 0.0));
    positionAlloc4->Add (Vector(172.20633645749228,148.1218306439337, 0.0));
    positionAlloc4->Add (Vector(20.638036220292587,489.5797247483484, 0.0));
    positionAlloc4->Add (Vector(111.0040684308366,282.4043084443729, 0.0));
    positionAlloc4->Add (Vector(421.5075866951475,102.33029612077554, 0.0));
    positionAlloc4->Add (Vector(47.80251545023584,454.4667659569221, 0.0));
    positionAlloc4->Add (Vector(89.09355068202618,232.83208835516862, 0.0));
    positionAlloc4->Add (Vector(86.46654995208736,253.76871245819714, 0.0));
    positionAlloc4->Add (Vector(330.73926900942274,322.22423257602674, 0.0));
    positionAlloc4->Add (Vector(272.23803795494183,263.66945579022587, 0.0));
    positionAlloc4->Add (Vector(271.23182866583994,236.930584305049, 0.0));
    positionAlloc4->Add (Vector(38.41834619393164,465.2940696807717, 0.0));
    positionAlloc4->Add (Vector(453.6126770388975,295.23554329985006, 0.0));
    positionAlloc4->Add (Vector(494.4255183493443,160.78446047887212, 0.0));
    positionAlloc4->Add (Vector(309.0844670256495,337.7437071613823, 0.0));
    positionAlloc4->Add (Vector(102.02152890440635,56.81917349987692, 0.0));
    positionAlloc4->Add (Vector(226.8382346727445,447.80851736011203, 0.0));
    positionAlloc4->Add (Vector(323.03153334313544,136.8546207027646, 0.0));
    positionAlloc4->Add (Vector(278.97167167350693,11.954691399149286, 0.0));
    positionAlloc4->Add (Vector(44.77396913857468,462.55401524337594, 0.0));
    positionAlloc4->Add (Vector(384.4259302421407,266.2486896624322, 0.0));
    positionAlloc4->Add (Vector(170.88723934684313,422.1399778317021, 0.0));
    positionAlloc4->Add (Vector(35.02281368593779,188.16116754270152, 0.0));
    positionAlloc4->Add (Vector(371.2880727045126,438.74397899420273, 0.0));
    positionAlloc4->Add (Vector(275.57561845916496,253.99261315483352, 0.0));
    positionAlloc4->Add (Vector(98.58538317063947,392.1652029317595, 0.0));
    positionAlloc4->Add (Vector(61.39097998778564,104.3637086495619, 0.0));
    positionAlloc4->Add (Vector(375.32707589428287,352.5724947656441, 0.0));
    positionAlloc4->Add (Vector(459.4580793225478,119.10905847304943, 0.0));
    positionAlloc4->Add (Vector(305.0187345226994,140.16010183143908, 0.0));
    positionAlloc4->Add (Vector(239.96037748296,43.46454955859258, 0.0));
    positionAlloc4->Add (Vector(355.7293493939108,216.47774047255396, 0.0));
    positionAlloc4->Add (Vector(116.123028521265,297.47795719812376, 0.0));
    positionAlloc4->Add (Vector(411.10354939316926,29.036570025688434, 0.0));
    positionAlloc4->Add (Vector(79.88480505579221,59.3280164489553, 0.0));
    positionAlloc4->Add (Vector(218.22023312200227,445.431581162852, 0.0));
    positionAlloc4->Add (Vector(353.0070115314778,75.52658388807775, 0.0));
    positionAlloc4->Add (Vector(440.47271252449013,261.8984707779496, 0.0));
    positionAlloc4->Add (Vector(146.09850558893095,246.28440226593628, 0.0));
    positionAlloc4->Add (Vector(60.511092551317645,345.26918728953234, 0.0));
    positionAlloc4->Add (Vector(99.46622352104362,3.6776528137933973, 0.0));
    positionAlloc4->Add (Vector(97.38845874710222,71.06665961035719, 0.0));
    positionAlloc4->Add (Vector(401.5459796411919,245.7767975351725, 0.0));
    positionAlloc4->Add (Vector(487.9151460781924,20.016864394942267, 0.0));
    positionAlloc4->Add (Vector(404.38595841472727,357.81900478817226, 0.0));
    positionAlloc4->Add (Vector(206.96304108083407,65.57261694855853, 0.0));
    positionAlloc4->Add (Vector(72.76269789381983,296.74027769043346, 0.0));
    positionAlloc4->Add (Vector(326.19399214573076,40.61978834968366, 0.0));
    positionAlloc4->Add (Vector(138.47023571633414,251.83777524981133, 0.0));
    positionAlloc4->Add (Vector(259.2761226992195,406.32333087370347, 0.0));
    positionAlloc4->Add (Vector(372.5954056640333,108.85967568610167, 0.0));
    positionAlloc4->Add (Vector(375.45769940804814,265.76964544079937, 0.0));
    positionAlloc4->Add (Vector(410.35521130027143,389.50627248720104, 0.0));
    positionAlloc4->Add (Vector(178.11412222807166,297.96855739496266, 0.0));
    positionAlloc4->Add (Vector(350.82680586331526,341.8470716870832, 0.0));
    positionAlloc4->Add (Vector(139.41518010495392,465.6746175194648, 0.0));
    positionAlloc4->Add (Vector(294.4683341812092,33.50238971558323, 0.0));
    positionAlloc4->Add (Vector(207.508321065343,28.09208098841076, 0.0));
    positionAlloc4->Add (Vector(148.5432441358216,239.28159407439688, 0.0));
    positionAlloc4->Add (Vector(50.9695720279279,118.69012030318792, 0.0));
    positionAlloc4->Add (Vector(219.11772629047417,178.07253537095403, 0.0));
    positionAlloc4->Add (Vector(3.603387087165899,475.44441097067045, 0.0));
    positionAlloc4->Add (Vector(143.04322189487507,275.1822369936694, 0.0));
    positionAlloc4->Add (Vector(296.48290629118134,350.863521355786, 0.0));
    positionAlloc4->Add (Vector(179.38866402783694,362.56135637213083, 0.0));
    positionAlloc4->Add (Vector(242.40719594441356,477.08089409376095, 0.0));
    positionAlloc4->Add (Vector(148.09732957484607,359.3712507926998, 0.0));
    positionAlloc4->Add (Vector(60.62487728482979,304.11585468219454, 0.0));
    positionAlloc4->Add (Vector(476.94823122845764,24.494642952417212, 0.0));
    positionAlloc4->Add (Vector(167.98540113352095,330.6983169472515, 0.0));
    positionAlloc4->Add (Vector(44.63777901621818,264.75978869222945, 0.0));
    positionAlloc4->Add (Vector(212.44986035651635,49.4998322589762, 0.0));
    positionAlloc4->Add (Vector(303.62635757003943,364.79683579719256, 0.0));
    positionAlloc4->Add (Vector(66.75369448177997,32.48327183033833, 0.0));
    positionAlloc4->Add (Vector(287.6680173980365,124.02380291585202, 0.0));
    positionAlloc4->Add (Vector(425.07880021664994,306.2020541314927, 0.0));
    positionAlloc4->Add (Vector(175.34008146148756,103.29534831917059, 0.0));
    positionAlloc4->Add (Vector(499.0918084969341,209.92886159596074, 0.0));
    positionAlloc4->Add (Vector(340.8817987119573,64.63091610729177, 0.0));
    positionAlloc4->Add (Vector(495.36298517555656,122.14497057044387, 0.0));
    positionAlloc4->Add (Vector(78.09190729031762,323.3794867062923, 0.0));
    positionAlloc4->Add (Vector(481.9433204683732,348.9677097977136, 0.0));
    positionAlloc4->Add (Vector(489.6368785108341,475.3273535777166, 0.0));
    positionAlloc4->Add (Vector(495.7153366505204,470.87579032658624, 0.0));
    positionAlloc4->Add (Vector(227.5089689898569,429.1944755310533, 0.0));
    positionAlloc4->Add (Vector(382.24815482970274,421.2851463372456, 0.0));
    positionAlloc4->Add (Vector(377.5592973269745,142.64347269461103, 0.0));
    positionAlloc4->Add (Vector(142.53515162774082,20.17023826649228, 0.0));
    positionAlloc4->Add (Vector(164.8718112933702,82.12104831687644, 0.0));
    positionAlloc4->Add (Vector(181.08609999032672,390.02837037771735, 0.0));
    positionAlloc4->Add (Vector(310.0558889211641,95.17713510888726, 0.0));
    positionAlloc4->Add (Vector(338.5784342772379,483.9539981977021, 0.0));
    positionAlloc4->Add (Vector(0.6921407938462742,221.49340457493943, 0.0));
    positionAlloc4->Add (Vector(430.80493582194975,80.88750579420001, 0.0));
    positionAlloc4->Add (Vector(257.50084663513087,151.97778202972233, 0.0));
    positionAlloc4->Add (Vector(499.5812704215652,38.548522757402004, 0.0));
    positionAlloc4->Add (Vector(427.86075512258003,282.5963304841081, 0.0));
    positionAlloc4->Add (Vector(235.5713487058705,114.63482518430213, 0.0));
    positionAlloc4->Add (Vector(52.90727308928034,342.3721386032462, 0.0));
    positionAlloc4->Add (Vector(249.25040955022308,263.65222986684205, 0.0));
    positionAlloc4->Add (Vector(274.7836579537829,353.37198127333414, 0.0));
    positionAlloc4->Add (Vector(162.21493525953213,14.790339044132628, 0.0));
    positionAlloc4->Add (Vector(40.702631464880454,164.15217004870058, 0.0));
    positionAlloc4->Add (Vector(175.16271703987252,182.19286573519787, 0.0));
    positionAlloc4->Add (Vector(134.69891645124395,355.37507550064896, 0.0));
    positionAlloc4->Add (Vector(319.002976438878,453.96207643535456, 0.0));
    positionAlloc4->Add (Vector(79.17357260367497,295.25454296416586, 0.0));
    positionAlloc4->Add (Vector(261.7881736591648,465.7668851068655, 0.0));
    positionAlloc4->Add (Vector(310.25546818722813,246.0535968283068, 0.0));
    positionAlloc4->Add (Vector(490.0014194280326,416.6653846397413, 0.0));
    positionAlloc4->Add (Vector(103.4449754625481,370.4858168701219, 0.0));
    positionAlloc4->Add (Vector(167.48313769449635,281.8917926108714, 0.0));
    positionAlloc4->Add (Vector(88.51169943608417,394.6292511996265, 0.0));
    positionAlloc4->Add (Vector(377.3445342726908,475.07920818041015, 0.0));
    positionAlloc4->Add (Vector(353.1944647622327,288.9607321277682, 0.0));
    positionAlloc4->Add (Vector(203.6744114749991,248.11224767416877, 0.0));
    positionAlloc4->Add (Vector(436.2989664967592,403.8292092897027, 0.0));
    positionAlloc4->Add (Vector(117.62094933784012,282.1472814196944, 0.0));
    positionAlloc4->Add (Vector(290.5595104939391,312.46964808460837, 0.0));
    positionAlloc4->Add (Vector(120.55110894132231,36.58169027265051, 0.0));
    positionAlloc4->Add (Vector(152.1224307716148,145.20393903594208, 0.0));
    positionAlloc4->Add (Vector(363.90769892337164,246.0215571757945, 0.0));
    positionAlloc4->Add (Vector(407.5165576614109,131.42100927773504, 0.0));
    positionAlloc4->Add (Vector(192.5948253218745,279.99886751779906, 0.0));
    positionAlloc4->Add (Vector(264.7201568347565,110.65601971402312, 0.0));
    positionAlloc4->Add (Vector(299.5275917384968,35.27792912146993, 0.0));
    positionAlloc4->Add (Vector(452.16789811165927,227.36389836275083, 0.0));
    positionAlloc4->Add (Vector(250.044308536849,43.739770055914285, 0.0));
    positionAlloc4->Add (Vector(384.51070298597483,207.81357731792127, 0.0));
    positionAlloc4->Add (Vector(255.29914239173846,223.50626011626977, 0.0));
    positionAlloc4->Add (Vector(356.21699105878946,374.91503657676634, 0.0));
    positionAlloc4->Add (Vector(473.9731812943834,242.49160035694067, 0.0));
    positionAlloc4->Add (Vector(247.4159134489581,475.277725282819, 0.0));
    positionAlloc4->Add (Vector(334.9494403272433,191.41048719122762, 0.0));
    positionAlloc4->Add (Vector(206.3057731108371,402.0432095536903, 0.0));
    positionAlloc4->Add (Vector(63.975036583884645,376.88043657245504, 0.0));
    positionAlloc4->Add (Vector(188.119864994935,19.51848764842107, 0.0));
    positionAlloc4->Add (Vector(94.45111810528405,210.00400978911566, 0.0));
    positionAlloc4->Add (Vector(114.85436820955557,286.04871899120997, 0.0));
    positionAlloc4->Add (Vector(151.27815567091014,424.2714794999111, 0.0));
    positionAlloc4->Add (Vector(127.46835077537033,460.2096384268144, 0.0));
    positionAlloc4->Add (Vector(116.50617984815142,310.9631047910314, 0.0));
    positionAlloc4->Add (Vector(114.68271986767647,346.8012337471853, 0.0));
    positionAlloc4->Add (Vector(376.1954430426973,132.47721090455667, 0.0));
    positionAlloc4->Add (Vector(286.87424992854955,451.37100528165934, 0.0));
    positionAlloc4->Add (Vector(221.25137523625028,309.72284363598527, 0.0));
    positionAlloc4->Add (Vector(315.72823138693826,385.09242634395787, 0.0));
    positionAlloc4->Add (Vector(360.79382544502863,149.2094498080609, 0.0));
    positionAlloc4->Add (Vector(390.22788580372674,460.1321040058189, 0.0));
    positionAlloc4->Add (Vector(13.781228727608063,87.23457188931583, 0.0));
    positionAlloc4->Add (Vector(129.84985633206475,87.10645177913506, 0.0));
    positionAlloc4->Add (Vector(116.83260433390363,35.076987566343696, 0.0));
    positionAlloc4->Add (Vector(477.54939090316736,127.83838264007474, 0.0));
    positionAlloc4->Add (Vector(257.8520957532722,443.682000090876, 0.0));
    positionAlloc4->Add (Vector(246.1983267886379,205.95300697854395, 0.0));
    positionAlloc4->Add (Vector(84.16710829141016,307.21860530770726, 0.0));
    positionAlloc4->Add (Vector(149.1388605807398,40.708687500335685, 0.0));
    positionAlloc4->Add (Vector(129.059996357699,137.80944817718992, 0.0));
    positionAlloc4->Add (Vector(461.8137878732717,422.6346307125232, 0.0));
    positionAlloc4->Add (Vector(50.52624595382571,181.17859940036553, 0.0));
    positionAlloc4->Add (Vector(454.5392666706091,125.48820965741564, 0.0));
    positionAlloc4->Add (Vector(124.36410022785881,470.9762528617189, 0.0));
    positionAlloc4->Add (Vector(462.89821393475125,490.31538168314086, 0.0));
    positionAlloc4->Add (Vector(172.62517714230856,319.0402663790688, 0.0));
    positionAlloc4->Add (Vector(136.1540108155934,464.0389977264469, 0.0));
    positionAlloc4->Add (Vector(314.4485748183565,347.0123294722279, 0.0));
    positionAlloc4->Add (Vector(387.33624595455547,91.2201432685979, 0.0));
    positionAlloc4->Add (Vector(256.2902425930565,337.118650749492, 0.0));
    positionAlloc4->Add (Vector(183.21703587281078,287.670716008964, 0.0));
    positionAlloc4->Add (Vector(480.0685473680504,144.94781807015522, 0.0));
    positionAlloc4->Add (Vector(270.8729744326573,233.5676102267491, 0.0));
    positionAlloc4->Add (Vector(402.1707981036334,315.59086436277164, 0.0));
    positionAlloc4->Add (Vector(275.8393092253112,403.95270958496155, 0.0));
    positionAlloc4->Add (Vector(84.49244880313212,30.408268233331594, 0.0));
    positionAlloc4->Add (Vector(51.34382548607708,34.400129225799546, 0.0));
    positionAlloc4->Add (Vector(99.6333426627547,93.08427104159233, 0.0));
    positionAlloc4->Add (Vector(228.98606703969926,286.3790290309662, 0.0));
    positionAlloc4->Add (Vector(338.9870193408312,24.31453242454451, 0.0));
    positionAlloc4->Add (Vector(27.200254611931097,144.54299738366538, 0.0));
    positionAlloc4->Add (Vector(52.689068348774185,228.73271321593603, 0.0));
    positionAlloc4->Add (Vector(491.09529281710223,23.837038647584187, 0.0));
    positionAlloc4->Add (Vector(67.2529793668727,3.8409795449355078, 0.0));
    positionAlloc4->Add (Vector(328.48893395429656,40.75049035161526, 0.0));
    positionAlloc4->Add (Vector(201.87286263647164,441.64764194993086, 0.0));
    positionAlloc4->Add (Vector(388.27865320081594,125.2149492095831, 0.0));
    positionAlloc4->Add (Vector(356.3615374109138,97.4844667402026, 0.0));
    positionAlloc4->Add (Vector(382.21037471814327,32.835755482149175, 0.0));
    positionAlloc4->Add (Vector(158.2457668641517,410.53455649373706, 0.0));
    positionAlloc4->Add (Vector(26.568886213969133,358.06670353941405, 0.0));
    positionAlloc4->Add (Vector(209.39944859360688,120.52596019835082, 0.0));
    positionAlloc4->Add (Vector(271.3018635611679,4.683242638785556, 0.0));
    positionAlloc4->Add (Vector(97.04591716289562,499.36038339645773, 0.0));
    positionAlloc4->Add (Vector(84.37513248647049,390.6962791608891, 0.0));
    positionAlloc4->Add (Vector(298.46262955429927,219.49754849086582, 0.0));
    positionAlloc4->Add (Vector(389.74308104205693,405.96143443462034, 0.0));
    positionAlloc4->Add (Vector(428.3624801678914,337.50308994592905, 0.0));
    positionAlloc4->Add (Vector(198.74154865245103,372.28713414333254, 0.0));
    positionAlloc4->Add (Vector(336.6154109362213,490.74903065092695, 0.0));
    positionAlloc4->Add (Vector(136.22481052948888,202.9107444273105, 0.0));
    positionAlloc4->Add (Vector(395.4341433597816,136.99482229845734, 0.0));
    positionAlloc4->Add (Vector(281.6470196703107,213.9017052454742, 0.0));
    positionAlloc4->Add (Vector(16.098043630943526,50.76272393481174, 0.0));
    positionAlloc4->Add (Vector(13.5085412170442,243.60511902442954, 0.0));
    positionAlloc4->Add (Vector(375.0781602574083,21.03660131724261, 0.0));
    positionAlloc4->Add (Vector(495.5465338513877,416.8695315250271, 0.0));
    positionAlloc4->Add (Vector(79.38917806708545,137.93230477760144, 0.0));
    positionAlloc4->Add (Vector(331.37133438064984,471.0633179201876, 0.0));
    positionAlloc4->Add (Vector(146.83403814668776,93.09055219364454, 0.0));
    positionAlloc4->Add (Vector(367.3994998118658,252.78739056931943, 0.0));
    positionAlloc4->Add (Vector(13.506368900987386,262.9326920899454, 0.0));
    positionAlloc4->Add (Vector(400.1179728170383,19.587579039273106, 0.0));
    positionAlloc4->Add (Vector(344.1259456409445,32.40541899514193, 0.0));
    positionAlloc4->Add (Vector(239.354078737973,360.2447507825077, 0.0));
    positionAlloc4->Add (Vector(327.59212473749847,260.7268231006441, 0.0));
    positionAlloc4->Add (Vector(336.57408936282945,15.026985066238508, 0.0));
    positionAlloc4->Add (Vector(137.21829780248356,155.0654188563738, 0.0));
    positionAlloc4->Add (Vector(211.74213153336484,234.22394938387419, 0.0));
    positionAlloc4->Add (Vector(116.82368896371675,82.96925624101308, 0.0));
    positionAlloc4->Add (Vector(30.67472544167521,382.2498808968903, 0.0));
    positionAlloc4->Add (Vector(198.9794865973643,306.4398988808648, 0.0));
    positionAlloc4->Add (Vector(276.5382277599942,203.20762186623315, 0.0));
    positionAlloc4->Add (Vector(182.33184240594363,336.37476481175406, 0.0));
    positionAlloc4->Add (Vector(342.1153631119268,410.69565607034286, 0.0));
    positionAlloc4->Add (Vector(239.67759834196255,337.32939641002775, 0.0));
    positionAlloc4->Add (Vector(282.7670309672928,134.45147828953318, 0.0));
    positionAlloc4->Add (Vector(100.13696146998925,386.8317071381735, 0.0));
    positionAlloc4->Add (Vector(423.88736316292324,95.18113635443004, 0.0));
    positionAlloc4->Add (Vector(404.8451519028192,399.7922610895627, 0.0));
    positionAlloc4->Add (Vector(271.6826374568175,268.5533244605001, 0.0));
    positionAlloc4->Add (Vector(361.60990335766877,344.54936449399275, 0.0));
    positionAlloc4->Add (Vector(199.77891741290483,116.81093272362075, 0.0));
    positionAlloc4->Add (Vector(409.4785317305114,472.9461317848447, 0.0));
    positionAlloc4->Add (Vector(173.48813542143532,449.97210113864816, 0.0));
    positionAlloc4->Add (Vector(336.5235878486128,266.8454477061194, 0.0));
    positionAlloc4->Add (Vector(444.02468864060575,381.0565325474024, 0.0));
    positionAlloc4->Add (Vector(42.18691476915942,207.9872809717754, 0.0));
    positionAlloc4->Add (Vector(25.575018312953944,495.1231178438306, 0.0));
    positionAlloc4->Add (Vector(494.8470281120864,404.1800657880457, 0.0));
    positionAlloc4->Add (Vector(339.7697207523285,102.28907394337666, 0.0));
    positionAlloc4->Add (Vector(304.6915894074907,457.07990243048096, 0.0));
    positionAlloc4->Add (Vector(219.69447254937873,352.3098928621153, 0.0));
    positionAlloc4->Add (Vector(23.02153350336711,395.8881192758206, 0.0));
    positionAlloc4->Add (Vector(391.2007163071957,457.23208728772903, 0.0));
    positionAlloc4->Add (Vector(5.742047847380805,272.4724140795936, 0.0));
    positionAlloc4->Add (Vector(212.39829871537196,236.30176341200686, 0.0));
    positionAlloc4->Add (Vector(276.6579764600603,103.26940957060448, 0.0));
    positionAlloc4->Add (Vector(34.120282394542066,325.13743016780865, 0.0));
    positionAlloc4->Add (Vector(407.61288903320167,454.68184459120954, 0.0));
    positionAlloc4->Add (Vector(12.988671313080125,45.255434110933464, 0.0));
    positionAlloc4->Add (Vector(223.76439512011248,414.9713889332497, 0.0));
    positionAlloc4->Add (Vector(55.46036179604036,484.4106469911858, 0.0));
    positionAlloc4->Add (Vector(383.5731602269886,114.05388569330194, 0.0));
    positionAlloc4->Add (Vector(274.69454107052564,59.99711432454025, 0.0));
    positionAlloc4->Add (Vector(496.3797854771444,86.88471743241367, 0.0));
    positionAlloc4->Add (Vector(315.55354029831085,445.3299857318176, 0.0));
    positionAlloc4->Add (Vector(315.77166437668564,386.41304259451454, 0.0));
    positionAlloc4->Add (Vector(346.39807572586517,407.49578842511585, 0.0));
    positionAlloc4->Add (Vector(114.60014103562716,45.06690246356515, 0.0));
    positionAlloc4->Add (Vector(267.4665014748787,425.10436085973663, 0.0));
    positionAlloc4->Add (Vector(6.938481489826065,399.45941529832396, 0.0));
    positionAlloc4->Add (Vector(62.105783808933445,333.7405687143931, 0.0));
    positionAlloc4->Add (Vector(481.4706811866038,231.88526781283946, 0.0));
    positionAlloc4->Add (Vector(18.51755586876974,122.21742743952952, 0.0));
    positionAlloc4->Add (Vector(473.38398741892354,174.69583078858398, 0.0));
    positionAlloc4->Add (Vector(224.825043869387,0.9836566579046746, 0.0));
    positionAlloc4->Add (Vector(466.9631372487001,406.33233337916727, 0.0));
    positionAlloc4->Add (Vector(189.20259343000794,377.9321444872403, 0.0));
    positionAlloc4->Add (Vector(474.43096030076936,494.96686838349, 0.0));
    positionAlloc4->Add (Vector(363.9388814619563,218.44723510477766, 0.0));
    positionAlloc4->Add (Vector(342.97614541076814,287.1266306579622, 0.0));
    positionAlloc4->Add (Vector(360.7236413414856,91.99623862897954, 0.0));
    positionAlloc4->Add (Vector(417.9480192938791,172.29638629327215, 0.0));
    positionAlloc4->Add (Vector(480.68443148701925,171.02870295213285, 0.0));
    positionAlloc4->Add (Vector(134.66867364446833,145.1000318701089, 0.0));
    positionAlloc4->Add (Vector(87.43519691362378,243.5839143600269, 0.0));
    positionAlloc4->Add (Vector(401.83786749742706,38.82372134440681, 0.0));
    positionAlloc4->Add (Vector(107.46289254702346,467.74515650962286, 0.0));
    positionAlloc4->Add (Vector(129.52462211601633,298.54273560761175, 0.0));
    positionAlloc4->Add (Vector(328.58652869603236,153.19937260077106, 0.0));
    positionAlloc4->Add (Vector(443.30330238516706,313.420428301876, 0.0));
    positionAlloc4->Add (Vector(74.06588208240144,116.69889255257453, 0.0));
    positionAlloc4->Add (Vector(334.8109544183742,92.69597070345343, 0.0));
    positionAlloc4->Add (Vector(300.74496783710134,280.13398517673556, 0.0));
    positionAlloc4->Add (Vector(310.6025604397122,391.00943826971115, 0.0));
    positionAlloc4->Add (Vector(91.13020451290465,283.5492373980579, 0.0));
    positionAlloc4->Add (Vector(379.66623967256197,20.54269218063315, 0.0));
    positionAlloc4->Add (Vector(404.3585477850348,269.065901151712, 0.0));
    positionAlloc4->Add (Vector(27.332190312058025,370.8031629211196, 0.0));
    positionAlloc4->Add (Vector(254.24996195436006,211.14652611285013, 0.0));
    positionAlloc4->Add (Vector(404.7303746231321,136.28050018159993, 0.0));
    positionAlloc4->Add (Vector(110.42951856469124,348.39838570293995, 0.0));
    positionAlloc4->Add (Vector(67.93384140621389,380.7622380313513, 0.0));
    positionAlloc4->Add (Vector(292.6592561093765,192.46341292698042, 0.0));
    positionAlloc4->Add (Vector(495.3866902737584,180.95904032771008, 0.0));
    positionAlloc4->Add (Vector(442.658522734264,478.57888167493576, 0.0));
    positionAlloc4->Add (Vector(46.23612592545051,241.74295391239664, 0.0));
    positionAlloc4->Add (Vector(293.9028721903605,445.94561093098275, 0.0));
    positionAlloc4->Add (Vector(61.539025894538355,63.175234982387366, 0.0));
    positionAlloc4->Add (Vector(436.76626367372376,123.85937635396627, 0.0));
    positionAlloc4->Add (Vector(389.00915770525745,9.53415245561484, 0.0));
    positionAlloc4->Add (Vector(428.2686974082643,144.48691841826772, 0.0));
    positionAlloc4->Add (Vector(395.29629382313334,333.9492002442775, 0.0));
    positionAlloc4->Add (Vector(237.0952537879844,476.23956707000224, 0.0));
    positionAlloc4->Add (Vector(336.26074276779497,98.94719008030833, 0.0));
    positionAlloc4->Add (Vector(124.46952355822988,80.29484110176787, 0.0));
    positionAlloc4->Add (Vector(360.52038196986194,45.421394892167676, 0.0));
    positionAlloc4->Add (Vector(227.50425645863697,192.01169887897407, 0.0));
    positionAlloc4->Add (Vector(21.308560066602812,300.51670192426434, 0.0));
    positionAlloc4->Add (Vector(149.61858295375401,326.5335614405656, 0.0));
    positionAlloc4->Add (Vector(203.0858018074402,147.3228633622361, 0.0));
    positionAlloc4->Add (Vector(370.77196798917197,193.33095990441635, 0.0));
    positionAlloc4->Add (Vector(498.1516750641728,401.8356857678908, 0.0));
    positionAlloc4->Add (Vector(73.02192878965108,302.40369300401176, 0.0));
    positionAlloc4->Add (Vector(417.2060017803478,175.61864773379742, 0.0));
    positionAlloc4->Add (Vector(405.96133747338223,441.4527165787587, 0.0));
    positionAlloc4->Add (Vector(288.36453528980906,124.03513284240569, 0.0));
    positionAlloc4->Add (Vector(197.08012808470775,162.62412065287845, 0.0));
    positionAlloc4->Add (Vector(70.46607529832677,456.55187919762517, 0.0));
    positionAlloc4->Add (Vector(219.25082367653292,384.47050758737515, 0.0));
    positionAlloc4->Add (Vector(474.81628372697935,108.94887850722206, 0.0));
    positionAlloc4->Add (Vector(121.88350832991578,282.8482752477212, 0.0));
    positionAlloc4->Add (Vector(150.88320692713282,225.09926428293235, 0.0));
    positionAlloc4->Add (Vector(145.9041068393933,249.2878883557083, 0.0));
    positionAlloc4->Add (Vector(253.25642391975904,381.4812876635383, 0.0));
    positionAlloc4->Add (Vector(349.4384825077726,37.981333518544105, 0.0));
    positionAlloc4->Add (Vector(465.14827460014453,362.63789068464706, 0.0));
    positionAlloc4->Add (Vector(361.57556699900624,297.9700799427366, 0.0));
    positionAlloc4->Add (Vector(368.73374222026627,78.50997087593508, 0.0));
    positionAlloc4->Add (Vector(117.82000524111203,134.84511484330807, 0.0));
    positionAlloc4->Add (Vector(229.86660477650773,73.38240430288434, 0.0));
    positionAlloc4->Add (Vector(442.07080092469226,227.1814998203831, 0.0));
    positionAlloc4->Add (Vector(384.30888305394257,385.7756318404629, 0.0));
    positionAlloc4->Add (Vector(392.62839186848254,372.5861470396121, 0.0));
    positionAlloc4->Add (Vector(370.8162049601942,267.23711174912165, 0.0));
    positionAlloc4->Add (Vector(94.36087821624866,264.9848178003151, 0.0));
    positionAlloc4->Add (Vector(361.7562103744258,469.6712740039524, 0.0));
    positionAlloc4->Add (Vector(154.3334925123851,242.78604669578402, 0.0));
    positionAlloc4->Add (Vector(150.07750479827965,342.4588966665647, 0.0));
    positionAlloc4->Add (Vector(268.60183100520254,125.61223818651645, 0.0));
    positionAlloc4->Add (Vector(316.3423080702413,164.71431299251367, 0.0));
    positionAlloc4->Add (Vector(431.37863731073173,22.467533101834526, 0.0));
    positionAlloc4->Add (Vector(360.57263799983673,186.63168678325493, 0.0));
    positionAlloc4->Add (Vector(275.73067507842745,315.5100073423044, 0.0));
    positionAlloc4->Add (Vector(229.81251069121777,291.3532170698079, 0.0));
    positionAlloc4->Add (Vector(11.869854248216406,405.7097384335234, 0.0));
    positionAlloc4->Add (Vector(486.49517791415946,183.91096722521806, 0.0));
    positionAlloc4->Add (Vector(136.73896639111777,342.9165640722559, 0.0));
    positionAlloc4->Add (Vector(389.8457637974606,115.23784070201914, 0.0));
    positionAlloc4->Add (Vector(145.5413448594986,491.3213990336398, 0.0));
    positionAlloc4->Add (Vector(306.91751592455034,96.54246028819603, 0.0));
    positionAlloc4->Add (Vector(4.855213339247655,146.35853753285028, 0.0));
    positionAlloc4->Add (Vector(301.8197441479809,127.24775007024208, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_13.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_ECC_flowmon/switch_ECC_13.flowmon", false, false);
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