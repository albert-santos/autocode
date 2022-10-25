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
    positionAlloc->Add (Vector (500.0,500.0, 0.0));
    positionAlloc->Add (Vector (500.0,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (500.0,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (500.0,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (500.0,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (500.0,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,277.77777777777777, 0.0));
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
    positionAlloc->Add (Vector (388.8888888888889,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,0.0, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,500.0, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,222.22222222222223, 0.0));
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
    positionAlloc->Add (Vector (166.66666666666666,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,55.55555555555556, 0.0));
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
    positionAlloc->Add (Vector (55.55555555555556,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (0.0,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (0.0,333.3333333333333, 0.0));
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
    positionAlloc4->Add (Vector(146.42192427238325,205.1730984090403, 0.0));
    positionAlloc4->Add (Vector(472.057162606684,378.91037697145384, 0.0));
    positionAlloc4->Add (Vector(384.64894830136416,32.948854833274865, 0.0));
    positionAlloc4->Add (Vector(471.02933789349663,375.66794790430845, 0.0));
    positionAlloc4->Add (Vector(415.6540626148147,406.1181189043971, 0.0));
    positionAlloc4->Add (Vector(226.9957531785559,293.2616586361533, 0.0));
    positionAlloc4->Add (Vector(50.938921798044376,238.10899850510074, 0.0));
    positionAlloc4->Add (Vector(15.56429809741311,441.54283380853127, 0.0));
    positionAlloc4->Add (Vector(498.5149133558199,366.48026020734596, 0.0));
    positionAlloc4->Add (Vector(493.6899043044224,481.1579850052001, 0.0));
    positionAlloc4->Add (Vector(414.29185577009883,366.40901087002834, 0.0));
    positionAlloc4->Add (Vector(146.06050425606193,116.5626642525241, 0.0));
    positionAlloc4->Add (Vector(352.10037935859214,73.97713899826086, 0.0));
    positionAlloc4->Add (Vector(197.90231770587758,150.71758749703162, 0.0));
    positionAlloc4->Add (Vector(143.45589917388622,449.11340335015063, 0.0));
    positionAlloc4->Add (Vector(212.9329707027956,46.72781429628808, 0.0));
    positionAlloc4->Add (Vector(475.08052432614744,433.8644516493273, 0.0));
    positionAlloc4->Add (Vector(51.23911639326734,260.7305126830901, 0.0));
    positionAlloc4->Add (Vector(137.26095749586182,275.7475509871923, 0.0));
    positionAlloc4->Add (Vector(157.3450763811362,300.19246493998514, 0.0));
    positionAlloc4->Add (Vector(493.67856557409596,282.247132137056, 0.0));
    positionAlloc4->Add (Vector(165.95462786412236,45.04108316267974, 0.0));
    positionAlloc4->Add (Vector(334.88637681225356,422.88047474590985, 0.0));
    positionAlloc4->Add (Vector(384.65318409194083,461.94300867427256, 0.0));
    positionAlloc4->Add (Vector(300.3482541196827,433.61764253369466, 0.0));
    positionAlloc4->Add (Vector(469.35762706720607,144.78699008479407, 0.0));
    positionAlloc4->Add (Vector(63.18871780630442,363.73713268851947, 0.0));
    positionAlloc4->Add (Vector(172.77283443159158,27.929399012996846, 0.0));
    positionAlloc4->Add (Vector(150.19187932708994,419.0217359650325, 0.0));
    positionAlloc4->Add (Vector(475.4614285358582,43.47823570434534, 0.0));
    positionAlloc4->Add (Vector(415.284366952007,346.59738820523745, 0.0));
    positionAlloc4->Add (Vector(150.201857153568,457.8786138950331, 0.0));
    positionAlloc4->Add (Vector(265.24158358916196,207.91316584375136, 0.0));
    positionAlloc4->Add (Vector(410.9299789979108,367.53301030604246, 0.0));
    positionAlloc4->Add (Vector(111.18297696539919,16.176542099296142, 0.0));
    positionAlloc4->Add (Vector(220.19546589761146,228.250666420728, 0.0));
    positionAlloc4->Add (Vector(215.23319121184548,3.9917673927244435, 0.0));
    positionAlloc4->Add (Vector(145.80427774635135,92.95421784672914, 0.0));
    positionAlloc4->Add (Vector(271.1544241190913,294.31793326204445, 0.0));
    positionAlloc4->Add (Vector(291.7769698114288,474.23947079841975, 0.0));
    positionAlloc4->Add (Vector(181.37711806556166,206.28381056796053, 0.0));
    positionAlloc4->Add (Vector(175.1842856127872,239.76457185864953, 0.0));
    positionAlloc4->Add (Vector(308.1922153653364,68.52383455931904, 0.0));
    positionAlloc4->Add (Vector(436.6054797711097,389.36920837063485, 0.0));
    positionAlloc4->Add (Vector(21.897178550498108,245.3710422912877, 0.0));
    positionAlloc4->Add (Vector(38.492718793041114,204.64221096106027, 0.0));
    positionAlloc4->Add (Vector(145.31860679954283,122.22486154460793, 0.0));
    positionAlloc4->Add (Vector(295.95698911197303,487.1312833919612, 0.0));
    positionAlloc4->Add (Vector(176.41694391821937,183.25258369512198, 0.0));
    positionAlloc4->Add (Vector(330.0427343207812,279.8378894657603, 0.0));
    positionAlloc4->Add (Vector(295.17352522960584,257.32967276748445, 0.0));
    positionAlloc4->Add (Vector(296.79472063443495,235.41336753356396, 0.0));
    positionAlloc4->Add (Vector(150.15988086762394,166.7389764816592, 0.0));
    positionAlloc4->Add (Vector(209.97210046151764,340.6386392756228, 0.0));
    positionAlloc4->Add (Vector(134.46158363474632,406.71208029857894, 0.0));
    positionAlloc4->Add (Vector(243.27425564524086,88.65118028088331, 0.0));
    positionAlloc4->Add (Vector(361.27266789292264,172.10065389605683, 0.0));
    positionAlloc4->Add (Vector(168.47608293689692,36.22075909128358, 0.0));
    positionAlloc4->Add (Vector(223.57686262553327,321.0877728209458, 0.0));
    positionAlloc4->Add (Vector(495.26668996340254,258.3810893710109, 0.0));
    positionAlloc4->Add (Vector(76.40478040468219,226.58556168999166, 0.0));
    positionAlloc4->Add (Vector(369.4370749075199,374.12723706577333, 0.0));
    positionAlloc4->Add (Vector(183.0336450001782,159.55234785541495, 0.0));
    positionAlloc4->Add (Vector(371.10892666689705,297.49128332910664, 0.0));
    positionAlloc4->Add (Vector(91.4785514182001,239.94962962342365, 0.0));
    positionAlloc4->Add (Vector(268.23549077964554,210.54830570989463, 0.0));
    positionAlloc4->Add (Vector(484.90953999981565,394.22732316806685, 0.0));
    positionAlloc4->Add (Vector(411.9494834487941,157.57580964472146, 0.0));
    positionAlloc4->Add (Vector(73.91119980436045,448.3140199531335, 0.0));
    positionAlloc4->Add (Vector(56.318637991192574,16.994328275220404, 0.0));
    positionAlloc4->Add (Vector(108.5858434872608,295.1592692961504, 0.0));
    positionAlloc4->Add (Vector(415.60708219502175,432.39441933850287, 0.0));
    positionAlloc4->Add (Vector(9.673400125042807,275.5846043275013, 0.0));
    positionAlloc4->Add (Vector(347.1154935434826,171.1406500433134, 0.0));
    positionAlloc4->Add (Vector(419.53498738520943,11.54495101497094, 0.0));
    positionAlloc4->Add (Vector(347.1160141213671,345.64445167000224, 0.0));
    positionAlloc4->Add (Vector(187.85257539359512,378.72738692862436, 0.0));
    positionAlloc4->Add (Vector(279.5355936295678,463.53619498908995, 0.0));
    positionAlloc4->Add (Vector(207.57843917980196,393.9364411102834, 0.0));
    positionAlloc4->Add (Vector(267.39196403857477,252.4308312299966, 0.0));
    positionAlloc4->Add (Vector(467.62342602127006,430.52207437999715, 0.0));
    positionAlloc4->Add (Vector(115.29956632096838,213.33603428077967, 0.0));
    positionAlloc4->Add (Vector(39.41188238079379,81.69957420118091, 0.0));
    positionAlloc4->Add (Vector(290.94493954041354,153.6174030439889, 0.0));
    positionAlloc4->Add (Vector(327.3495713041123,401.30682740161797, 0.0));
    positionAlloc4->Add (Vector(447.18267678159754,135.2670598841235, 0.0));
    positionAlloc4->Add (Vector(169.06543257476181,313.08686868910837, 0.0));
    positionAlloc4->Add (Vector(237.32019164614348,182.5754411328937, 0.0));
    positionAlloc4->Add (Vector(114.45206526378948,468.29655568660036, 0.0));
    positionAlloc4->Add (Vector(223.94217620700672,297.1205129103519, 0.0));
    positionAlloc4->Add (Vector(135.67829552250672,298.1106443822218, 0.0));
    positionAlloc4->Add (Vector(486.78839163817946,318.0978554434469, 0.0));
    positionAlloc4->Add (Vector(150.4616596007886,59.2542887594828, 0.0));
    positionAlloc4->Add (Vector(361.9112761557703,28.484885628211387, 0.0));
    positionAlloc4->Add (Vector(199.35257680369318,264.5988674106664, 0.0));
    positionAlloc4->Add (Vector(55.99065203955106,482.32175940017146, 0.0));
    positionAlloc4->Add (Vector(279.3464634060881,311.47257969138235, 0.0));
    positionAlloc4->Add (Vector(342.1692203109195,192.7512680058377, 0.0));
    positionAlloc4->Add (Vector(297.4711964232577,442.3917314414088, 0.0));
    positionAlloc4->Add (Vector(361.987332759586,137.53978065135564, 0.0));
    positionAlloc4->Add (Vector(86.93093519928347,219.1246361850549, 0.0));
    positionAlloc4->Add (Vector(382.4799723499801,167.66614636889477, 0.0));
    positionAlloc4->Add (Vector(204.5477063893959,259.79700875992523, 0.0));
    positionAlloc4->Add (Vector(463.8115989158871,26.222559613767903, 0.0));
    positionAlloc4->Add (Vector(273.10858538228933,26.95363057407346, 0.0));
    positionAlloc4->Add (Vector(267.1079737683118,395.77044035839066, 0.0));
    positionAlloc4->Add (Vector(146.31379032830333,149.8972921617906, 0.0));
    positionAlloc4->Add (Vector(309.5825194165971,56.05478819723386, 0.0));
    positionAlloc4->Add (Vector(112.15879654650934,437.4137762604229, 0.0));
    positionAlloc4->Add (Vector(383.59561826655914,343.706085431864, 0.0));
    positionAlloc4->Add (Vector(297.91903778097156,342.6381447489946, 0.0));
    positionAlloc4->Add (Vector(149.71429615856758,83.3567903813991, 0.0));
    positionAlloc4->Add (Vector(67.43931470912557,316.96753291383595, 0.0));
    positionAlloc4->Add (Vector(186.96876552119662,66.05486469210724, 0.0));
    positionAlloc4->Add (Vector(472.1996749810895,414.0632985527394, 0.0));
    positionAlloc4->Add (Vector(199.39295082421515,301.557835556624, 0.0));
    positionAlloc4->Add (Vector(126.16677864882247,239.2044422618609, 0.0));
    positionAlloc4->Add (Vector(285.2735578097653,261.728218910294, 0.0));
    positionAlloc4->Add (Vector(17.87076131328219,44.96139890812339, 0.0));
    positionAlloc4->Add (Vector(412.86713229860214,127.75216842844821, 0.0));
    positionAlloc4->Add (Vector(318.69457335047673,481.06600384901043, 0.0));
    positionAlloc4->Add (Vector(17.391065503628568,162.54269812529887, 0.0));
    positionAlloc4->Add (Vector(11.873074471099976,57.52316510757344, 0.0));
    positionAlloc4->Add (Vector(354.6209333654811,43.908745449283806, 0.0));
    positionAlloc4->Add (Vector(182.9892784990841,308.59657711035186, 0.0));
    positionAlloc4->Add (Vector(287.64376615080585,220.23198787833508, 0.0));
    positionAlloc4->Add (Vector(436.4920576224828,418.4099192732925, 0.0));
    positionAlloc4->Add (Vector(220.0691634249587,471.8926276372803, 0.0));
    positionAlloc4->Add (Vector(76.18216314141108,45.13946318620027, 0.0));
    positionAlloc4->Add (Vector(3.8164689614682556,146.36860431589838, 0.0));
    positionAlloc4->Add (Vector(211.175560590432,53.209467096395734, 0.0));
    positionAlloc4->Add (Vector(416.9811716404781,323.1352709165294, 0.0));
    positionAlloc4->Add (Vector(152.03297920621455,192.28427471529125, 0.0));
    positionAlloc4->Add (Vector(245.05621534454392,455.65618532719594, 0.0));
    positionAlloc4->Add (Vector(366.0128735355909,236.51723806023915, 0.0));
    positionAlloc4->Add (Vector(365.18684400470676,197.24357214470123, 0.0));
    positionAlloc4->Add (Vector(467.5939520211117,97.75004642500073, 0.0));
    positionAlloc4->Add (Vector(256.48971908428996,385.3215352187155, 0.0));
    positionAlloc4->Add (Vector(120.5144652058715,132.51383609284483, 0.0));
    positionAlloc4->Add (Vector(71.29955833006841,276.50285596438664, 0.0));
    positionAlloc4->Add (Vector(217.74633691529982,396.7278194727705, 0.0));
    positionAlloc4->Add (Vector(342.0122640587266,277.9981869205194, 0.0));
    positionAlloc4->Add (Vector(429.59975333450353,174.65145683529903, 0.0));
    positionAlloc4->Add (Vector(372.07025078230834,37.82001896766485, 0.0));
    positionAlloc4->Add (Vector(233.41074371828796,31.1644020547302, 0.0));
    positionAlloc4->Add (Vector(26.237874443808597,292.9668015533067, 0.0));
    positionAlloc4->Add (Vector(113.32340384495853,11.383336671153987, 0.0));
    positionAlloc4->Add (Vector(272.2346599817521,380.2061882342532, 0.0));
    positionAlloc4->Add (Vector(136.58554100476067,412.9121187905879, 0.0));
    positionAlloc4->Add (Vector(161.64494845841014,189.22569986219074, 0.0));
    positionAlloc4->Add (Vector(475.2723092278087,168.74922874417925, 0.0));
    positionAlloc4->Add (Vector(34.96363616069675,238.8806088671771, 0.0));
    positionAlloc4->Add (Vector(353.793990429975,0.40799177710576506, 0.0));
    positionAlloc4->Add (Vector(49.407367065250504,346.14347138186395, 0.0));
    positionAlloc4->Add (Vector(194.29154255541752,181.79194381462855, 0.0));
    positionAlloc4->Add (Vector(28.96926289051549,244.57645122535072, 0.0));
    positionAlloc4->Add (Vector(7.292224641837097,316.60506831148984, 0.0));
    positionAlloc4->Add (Vector(166.93012349588582,122.9747383555675, 0.0));
    positionAlloc4->Add (Vector(238.39234042651185,287.819417392893, 0.0));
    positionAlloc4->Add (Vector(476.34153048314766,184.66924005956693, 0.0));
    positionAlloc4->Add (Vector(367.50382527893623,166.91895934359334, 0.0));
    positionAlloc4->Add (Vector(474.900980333008,494.9083040697788, 0.0));
    positionAlloc4->Add (Vector(478.42793159500434,179.9743546201133, 0.0));
    positionAlloc4->Add (Vector(236.0042370488145,450.80706968358845, 0.0));
    positionAlloc4->Add (Vector(316.0010486086987,347.6138488620471, 0.0));
    positionAlloc4->Add (Vector(438.34157425652,175.63963830633443, 0.0));
    positionAlloc4->Add (Vector(84.50093075918002,34.534909715811466, 0.0));
    positionAlloc4->Add (Vector(73.65975516863365,333.8979132997339, 0.0));
    positionAlloc4->Add (Vector(37.62247857667733,170.7307273841891, 0.0));
    positionAlloc4->Add (Vector(185.79775513881324,140.8156657125541, 0.0));
    positionAlloc4->Add (Vector(178.5858668007822,307.6750026682599, 0.0));
    positionAlloc4->Add (Vector(490.7188082648328,411.4042896582544, 0.0));
    positionAlloc4->Add (Vector(360.21467359440146,241.346018813066, 0.0));
    positionAlloc4->Add (Vector(39.009992628348535,332.57042793167017, 0.0));
    positionAlloc4->Add (Vector(373.4897209310526,400.14595277949815, 0.0));
    positionAlloc4->Add (Vector(89.51696061401704,81.83862233139516, 0.0));
    positionAlloc4->Add (Vector(34.17075359521626,157.98859005887132, 0.0));
    positionAlloc4->Add (Vector(228.34220629337483,104.28928643304647, 0.0));
    positionAlloc4->Add (Vector(386.43662884016925,478.73634297068924, 0.0));
    positionAlloc4->Add (Vector(96.59220206184699,339.9840503561082, 0.0));
    positionAlloc4->Add (Vector(86.01681110774872,401.7204728111656, 0.0));
    positionAlloc4->Add (Vector(180.1286673708188,180.06388537637562, 0.0));
    positionAlloc4->Add (Vector(47.56410687716267,308.97932907751596, 0.0));
    positionAlloc4->Add (Vector(126.71488831164646,87.74875065567362, 0.0));
    positionAlloc4->Add (Vector(388.785919232933,484.35740233552787, 0.0));
    positionAlloc4->Add (Vector(317.06883656261175,259.5820110635422, 0.0));
    positionAlloc4->Add (Vector(54.15200607661036,169.33486890822456, 0.0));
    positionAlloc4->Add (Vector(318.3289469569525,195.46239669977194, 0.0));
    positionAlloc4->Add (Vector(220.0995933634361,87.89487651958233, 0.0));
    positionAlloc4->Add (Vector(201.12949720388673,365.57595775963426, 0.0));
    positionAlloc4->Add (Vector(152.7140949199885,249.6547395106844, 0.0));
    positionAlloc4->Add (Vector(200.94671707764527,336.6044989700608, 0.0));
    positionAlloc4->Add (Vector(403.2322848948636,419.7971979900786, 0.0));
    positionAlloc4->Add (Vector(316.1275049421841,140.4103458492621, 0.0));
    positionAlloc4->Add (Vector(481.55579218626144,294.4539493669139, 0.0));
    positionAlloc4->Add (Vector(300.74996645891673,332.05009186247895, 0.0));
    positionAlloc4->Add (Vector(360.4795903822031,170.28777463889088, 0.0));
    positionAlloc4->Add (Vector(495.3014335426361,121.1170286009709, 0.0));
    positionAlloc4->Add (Vector(464.0329706412746,283.7984563323488, 0.0));
    positionAlloc4->Add (Vector(426.81520995135975,72.014537840701, 0.0));
    positionAlloc4->Add (Vector(327.5937405611986,41.37079415969347, 0.0));
    positionAlloc4->Add (Vector(33.42875729591016,329.71225818296034, 0.0));
    positionAlloc4->Add (Vector(56.23676894093155,308.4955865382673, 0.0));
    positionAlloc4->Add (Vector(100.49211109940343,287.58757418400063, 0.0));
    positionAlloc4->Add (Vector(463.1737685644452,489.0385038622268, 0.0));
    positionAlloc4->Add (Vector(494.18241075969235,62.095445507596935, 0.0));
    positionAlloc4->Add (Vector(30.15926630926452,42.78542725363782, 0.0));
    positionAlloc4->Add (Vector(233.5090851444993,242.70384084221453, 0.0));
    positionAlloc4->Add (Vector(225.01193994147002,231.85958858884558, 0.0));
    positionAlloc4->Add (Vector(426.30492437724854,346.5112340035074, 0.0));
    positionAlloc4->Add (Vector(85.33584006636069,458.4014453126133, 0.0));
    positionAlloc4->Add (Vector(292.01009743344565,466.18809949305904, 0.0));
    positionAlloc4->Add (Vector(333.67057460909473,496.79035566119217, 0.0));
    positionAlloc4->Add (Vector(498.24806522721633,471.1599739054277, 0.0));
    positionAlloc4->Add (Vector(2.6038579193605194,259.84096405522894, 0.0));
    positionAlloc4->Add (Vector(462.10925403978536,410.0775033942677, 0.0));
    positionAlloc4->Add (Vector(71.57814213481211,352.6967385447533, 0.0));
    positionAlloc4->Add (Vector(274.13959767132803,269.8007727882748, 0.0));
    positionAlloc4->Add (Vector(179.9633869407824,319.90682047126836, 0.0));
    positionAlloc4->Add (Vector(148.69587463016876,242.54900407731782, 0.0));
    positionAlloc4->Add (Vector(98.24374312719242,323.5050611117958, 0.0));
    positionAlloc4->Add (Vector(346.61209792205375,364.32499001884725, 0.0));
    positionAlloc4->Add (Vector(399.50854788258795,25.598905679635074, 0.0));
    positionAlloc4->Add (Vector(39.33154944324635,163.15739326890215, 0.0));
    positionAlloc4->Add (Vector(345.2181130929403,147.86203691311516, 0.0));
    positionAlloc4->Add (Vector(170.98759195868735,292.3341616752436, 0.0));
    positionAlloc4->Add (Vector(298.9801227186515,147.4621705451049, 0.0));
    positionAlloc4->Add (Vector(311.0041829295685,475.6036476934078, 0.0));
    positionAlloc4->Add (Vector(373.41578694145653,19.303808094727536, 0.0));
    positionAlloc4->Add (Vector(109.79029421391284,196.4036817723598, 0.0));
    positionAlloc4->Add (Vector(386.51807554296033,63.56212142364986, 0.0));
    positionAlloc4->Add (Vector(453.15306460239714,400.95874668914, 0.0));
    positionAlloc4->Add (Vector(6.816730670226923,422.0176908413393, 0.0));
    positionAlloc4->Add (Vector(92.63633203637956,97.27014997833416, 0.0));
    positionAlloc4->Add (Vector(299.1050870972063,288.621957241242, 0.0));
    positionAlloc4->Add (Vector(332.97662559302233,259.28367796349437, 0.0));
    positionAlloc4->Add (Vector(115.46529282316837,225.87274998880736, 0.0));
    positionAlloc4->Add (Vector(318.75698336824223,270.4913731582802, 0.0));
    positionAlloc4->Add (Vector(322.77820333950314,285.0109560198612, 0.0));
    positionAlloc4->Add (Vector(440.8988810387112,98.09125046772499, 0.0));
    positionAlloc4->Add (Vector(102.68710189974777,0.8999799590104218, 0.0));
    positionAlloc4->Add (Vector(301.79875532926263,119.12807880827808, 0.0));
    positionAlloc4->Add (Vector(172.17080813889214,314.3541547099609, 0.0));
    positionAlloc4->Add (Vector(264.5635563907804,283.3282324467498, 0.0));
    positionAlloc4->Add (Vector(218.36008758533055,165.77887312054995, 0.0));
    positionAlloc4->Add (Vector(383.7040515528168,442.72929344693074, 0.0));
    positionAlloc4->Add (Vector(132.95803417819153,214.8482541791053, 0.0));
    positionAlloc4->Add (Vector(148.44662893354982,416.06025309526734, 0.0));
    positionAlloc4->Add (Vector(211.710089143548,404.2803144746769, 0.0));
    positionAlloc4->Add (Vector(10.070659136169946,171.63851273254716, 0.0));
    positionAlloc4->Add (Vector(53.16923958069258,212.91156065874972, 0.0));
    positionAlloc4->Add (Vector(222.38014842371217,330.5018871165325, 0.0));
    positionAlloc4->Add (Vector(56.23202844433717,54.40400160929282, 0.0));
    positionAlloc4->Add (Vector(291.00864934210216,481.9653247888489, 0.0));
    positionAlloc4->Add (Vector(436.3707043112621,469.5882837395895, 0.0));
    positionAlloc4->Add (Vector(115.87510972815596,240.03534805041394, 0.0));
    positionAlloc4->Add (Vector(112.66840164658531,419.14726690404416, 0.0));
    positionAlloc4->Add (Vector(376.96601758129276,180.60657816798292, 0.0));
    positionAlloc4->Add (Vector(221.2286443621258,499.71664407225893, 0.0));
    positionAlloc4->Add (Vector(293.29126661411186,286.1426842825907, 0.0));
    positionAlloc4->Add (Vector(342.2446783407297,32.60966815697169, 0.0));
    positionAlloc4->Add (Vector(414.35408132362727,208.93248742812042, 0.0));
    positionAlloc4->Add (Vector(39.675423310090274,94.45035911521637, 0.0));
    positionAlloc4->Add (Vector(138.34233315203477,315.25106649432956, 0.0));
    positionAlloc4->Add (Vector(146.15642787011558,443.9882035336986, 0.0));
    positionAlloc4->Add (Vector(429.6595931178736,123.9300969384814, 0.0));
    positionAlloc4->Add (Vector(221.80121500718636,24.500392809227336, 0.0));
    positionAlloc4->Add (Vector(225.40622671297444,380.35120351512927, 0.0));
    positionAlloc4->Add (Vector(98.78414777621403,26.898264346566446, 0.0));
    positionAlloc4->Add (Vector(256.7792509315936,43.51863276080575, 0.0));
    positionAlloc4->Add (Vector(391.2776963195054,298.23463055815375, 0.0));
    positionAlloc4->Add (Vector(203.40623419258947,140.34835277373608, 0.0));
    positionAlloc4->Add (Vector(345.6878996192212,55.57766321711777, 0.0));
    positionAlloc4->Add (Vector(139.340087346057,483.9545146939415, 0.0));
    positionAlloc4->Add (Vector(188.3528351771595,181.35124552413595, 0.0));
    positionAlloc4->Add (Vector(70.72343138140607,224.14035171127966, 0.0));
    positionAlloc4->Add (Vector(66.18072103491463,30.651255634213282, 0.0));
    positionAlloc4->Add (Vector(48.7460211206584,367.3929507666099, 0.0));
    positionAlloc4->Add (Vector(69.84472236303557,135.60237302653883, 0.0));
    positionAlloc4->Add (Vector(18.281727560517048,78.63469969988013, 0.0));
    positionAlloc4->Add (Vector(273.64805385618433,192.51320608878265, 0.0));
    positionAlloc4->Add (Vector(360.2048298165329,222.50367628418843, 0.0));
    positionAlloc4->Add (Vector(110.24306896278185,357.61621542160185, 0.0));
    positionAlloc4->Add (Vector(388.80539905011534,451.96642096308307, 0.0));
    positionAlloc4->Add (Vector(443.61410365004105,7.765429505840915, 0.0));
    positionAlloc4->Add (Vector(359.15000022980826,414.4858407477379, 0.0));
    positionAlloc4->Add (Vector(170.22522474696666,304.9647097315457, 0.0));
    positionAlloc4->Add (Vector(354.38787553845367,473.73223440152225, 0.0));
    positionAlloc4->Add (Vector(361.1703136367162,456.23660053580204, 0.0));
    positionAlloc4->Add (Vector(274.38052282387883,193.84642401935082, 0.0));
    positionAlloc4->Add (Vector(433.3691582861659,6.336375718157483, 0.0));
    positionAlloc4->Add (Vector(177.62749038332382,499.0359151593927, 0.0));
    positionAlloc4->Add (Vector(310.89064317848914,478.7267279985788, 0.0));
    positionAlloc4->Add (Vector(435.43767257389385,97.0317687412996, 0.0));
    positionAlloc4->Add (Vector(49.21662186215603,434.3766508746429, 0.0));
    positionAlloc4->Add (Vector(453.53621213297833,470.520255341871, 0.0));
    positionAlloc4->Add (Vector(114.29422130563788,141.21220451316825, 0.0));
    positionAlloc4->Add (Vector(429.5208997684902,222.71593809676654, 0.0));
    positionAlloc4->Add (Vector(238.1717589238428,187.64820915354812, 0.0));
    positionAlloc4->Add (Vector(297.4109017245657,251.5660053824773, 0.0));
    positionAlloc4->Add (Vector(91.18681419594932,121.14173682745604, 0.0));
    positionAlloc4->Add (Vector(412.117112438393,292.3390572049701, 0.0));
    positionAlloc4->Add (Vector(154.84649591597162,233.13316988452132, 0.0));
    positionAlloc4->Add (Vector(384.5578485494336,55.21019256774507, 0.0));
    positionAlloc4->Add (Vector(388.3746985589198,43.577134426042775, 0.0));
    positionAlloc4->Add (Vector(495.78822827166886,197.10556012541008, 0.0));
    positionAlloc4->Add (Vector(93.1601511946904,213.7283889517813, 0.0));
    positionAlloc4->Add (Vector(145.31495115407068,372.8033584238288, 0.0));
    positionAlloc4->Add (Vector(168.9356697411653,474.7465624837422, 0.0));
    positionAlloc4->Add (Vector(347.2907680223847,146.6450600640096, 0.0));
    positionAlloc4->Add (Vector(115.15504397501446,156.2796129321436, 0.0));
    positionAlloc4->Add (Vector(242.75649328237049,90.46250693791363, 0.0));
    positionAlloc4->Add (Vector(31.141985320340794,134.26059362693178, 0.0));
    positionAlloc4->Add (Vector(17.67409166349615,184.6674712213111, 0.0));
    positionAlloc4->Add (Vector(165.97176360261224,499.5602996967153, 0.0));
    positionAlloc4->Add (Vector(368.3637769113599,303.57111017027535, 0.0));
    positionAlloc4->Add (Vector(471.22976594051477,373.75956807870074, 0.0));
    positionAlloc4->Add (Vector(192.75888681525245,438.14065642049337, 0.0));
    positionAlloc4->Add (Vector(477.54853322041964,298.30281966907995, 0.0));
    positionAlloc4->Add (Vector(399.5900121798086,395.14930315651173, 0.0));
    positionAlloc4->Add (Vector(132.98828920399254,167.35168434612635, 0.0));
    positionAlloc4->Add (Vector(495.5429960065794,178.00134856373668, 0.0));
    positionAlloc4->Add (Vector(454.9598678581246,157.40283738928707, 0.0));
    positionAlloc4->Add (Vector(256.8871987606965,497.8281922249522, 0.0));
    positionAlloc4->Add (Vector(490.31281585303714,202.33330502234136, 0.0));
    positionAlloc4->Add (Vector(216.01729092196405,280.78859150917424, 0.0));
    positionAlloc4->Add (Vector(404.0211785533309,314.76118044068477, 0.0));
    positionAlloc4->Add (Vector(148.25263094463514,340.9587093862102, 0.0));
    positionAlloc4->Add (Vector(209.44758519366252,85.98839267437997, 0.0));
    positionAlloc4->Add (Vector(6.699671906484516,119.37039895698814, 0.0));
    positionAlloc4->Add (Vector(154.02184399620887,235.97187162570228, 0.0));
    positionAlloc4->Add (Vector(309.55832061581066,206.72311134400968, 0.0));
    positionAlloc4->Add (Vector(449.7120757920498,189.03375379506355, 0.0));
    positionAlloc4->Add (Vector(466.47048763077527,257.0445346901696, 0.0));
    positionAlloc4->Add (Vector(55.273312688326634,298.5926730450845, 0.0));
    positionAlloc4->Add (Vector(298.8424129056438,371.10921652271105, 0.0));
    positionAlloc4->Add (Vector(100.31347074640873,377.8723564520379, 0.0));
    positionAlloc4->Add (Vector(264.3600888435041,398.10147801416576, 0.0));
    positionAlloc4->Add (Vector(118.71810458000581,347.2836717191743, 0.0));
    positionAlloc4->Add (Vector(53.01469173119971,24.285283682400348, 0.0));
    positionAlloc4->Add (Vector(286.33358409891343,252.34540942735757, 0.0));
    positionAlloc4->Add (Vector(360.01733867741126,333.09048050206815, 0.0));
    positionAlloc4->Add (Vector(462.5393678415293,260.30969263028345, 0.0));
    positionAlloc4->Add (Vector(445.1980866452093,396.8921442737247, 0.0));
    positionAlloc4->Add (Vector(382.52954078420305,110.00364791888812, 0.0));
    positionAlloc4->Add (Vector(421.264271401784,378.24538614834756, 0.0));
    positionAlloc4->Add (Vector(483.27033555285414,449.27108768767397, 0.0));
    positionAlloc4->Add (Vector(58.710010751407246,106.92085536051104, 0.0));
    positionAlloc4->Add (Vector(229.31660935959803,400.6002998907706, 0.0));
    positionAlloc4->Add (Vector(360.01422841645103,415.49247346800314, 0.0));
    positionAlloc4->Add (Vector(391.4001237291642,247.85706510618095, 0.0));
    positionAlloc4->Add (Vector(362.3122120575828,485.1750057424973, 0.0));
    positionAlloc4->Add (Vector(297.3126678135802,186.63541672200472, 0.0));
    positionAlloc4->Add (Vector(341.77697587442634,389.23186633412917, 0.0));
    positionAlloc4->Add (Vector(58.7468051015797,165.1399558183999, 0.0));
    positionAlloc4->Add (Vector(251.24387591095754,193.45653805844492, 0.0));
    positionAlloc4->Add (Vector(11.149083190902964,120.5727516680627, 0.0));
    positionAlloc4->Add (Vector(381.65821562426663,421.706345924816, 0.0));
    positionAlloc4->Add (Vector(270.8555307453671,285.3498839309767, 0.0));
    positionAlloc4->Add (Vector(380.3895560830005,222.05242782074365, 0.0));
    positionAlloc4->Add (Vector(33.14075042851006,114.57161560513075, 0.0));
    positionAlloc4->Add (Vector(435.928907281043,499.73206234129873, 0.0));
    positionAlloc4->Add (Vector(93.29553626092284,63.12561675892198, 0.0));
    positionAlloc4->Add (Vector(342.7045938022303,252.94945088992543, 0.0));
    positionAlloc4->Add (Vector(432.28744177765975,113.00184052297307, 0.0));
    positionAlloc4->Add (Vector(284.3485926361211,495.23714670151054, 0.0));
    positionAlloc4->Add (Vector(470.0696369942833,329.7099821069853, 0.0));
    positionAlloc4->Add (Vector(445.7902875356933,188.4474045446164, 0.0));
    positionAlloc4->Add (Vector(108.20261853516932,97.43125630491417, 0.0));
    positionAlloc4->Add (Vector(146.87052299481783,13.52125822401523, 0.0));
    positionAlloc4->Add (Vector(359.2327536275822,98.53565212893656, 0.0));
    positionAlloc4->Add (Vector(236.38527187229886,222.05301874114974, 0.0));
    positionAlloc4->Add (Vector(194.20795316116758,79.49140107347947, 0.0));
    positionAlloc4->Add (Vector(86.07259532629475,399.9629157969857, 0.0));
    positionAlloc4->Add (Vector(332.46528122497017,184.1810432187514, 0.0));
    positionAlloc4->Add (Vector(1.8004687579684098,314.811426712411, 0.0));
    positionAlloc4->Add (Vector(273.85205840739815,368.41830993383644, 0.0));
    positionAlloc4->Add (Vector(112.6743557297315,31.739866078564994, 0.0));
    positionAlloc4->Add (Vector(340.42282531104354,42.80321397361453, 0.0));
    positionAlloc4->Add (Vector(434.85539457412887,434.00284477542664, 0.0));
    positionAlloc4->Add (Vector(426.625923251306,485.47550028155683, 0.0));
    positionAlloc4->Add (Vector(220.121819340043,227.2081950389351, 0.0));
    positionAlloc4->Add (Vector(346.098185848647,2.2440046157203852, 0.0));
    positionAlloc4->Add (Vector(496.130155333333,276.8052037978952, 0.0));
    positionAlloc4->Add (Vector(228.94891295126257,12.441853774659394, 0.0));
    positionAlloc4->Add (Vector(486.55159315278706,173.61797019638757, 0.0));
    positionAlloc4->Add (Vector(254.17461428574788,476.3787861318202, 0.0));
    positionAlloc4->Add (Vector(351.81552327492346,398.2661939459127, 0.0));
    positionAlloc4->Add (Vector(424.87910632941936,2.543470020819838, 0.0));
    positionAlloc4->Add (Vector(12.697506621756782,296.8414191229267, 0.0));
    positionAlloc4->Add (Vector(368.00130707299206,345.03708674633475, 0.0));
    positionAlloc4->Add (Vector(27.126277631748184,467.6577741516911, 0.0));
    positionAlloc4->Add (Vector(428.4590014359995,87.73580920097923, 0.0));
    positionAlloc4->Add (Vector(384.52472112069455,1.098797746263258, 0.0));
    positionAlloc4->Add (Vector(102.00639700980474,31.622505482761298, 0.0));
    positionAlloc4->Add (Vector(185.11089512385388,164.5131270290105, 0.0));
    positionAlloc4->Add (Vector(1.7009901683884832,211.6261979202877, 0.0));
    positionAlloc4->Add (Vector(438.10034419419935,273.56210761804056, 0.0));
    positionAlloc4->Add (Vector(101.10321259879434,11.534466863572456, 0.0));
    positionAlloc4->Add (Vector(37.77273731380443,244.5507283647766, 0.0));
    positionAlloc4->Add (Vector(396.92919507007133,165.77683019433297, 0.0));
    positionAlloc4->Add (Vector(18.08814377745932,318.14782867247953, 0.0));
    positionAlloc4->Add (Vector(377.2331288440093,306.6190073944243, 0.0));
    positionAlloc4->Add (Vector(124.73300234055706,240.4662549592642, 0.0));
    positionAlloc4->Add (Vector(6.235693937653275,96.182750903345, 0.0));
    positionAlloc4->Add (Vector(68.29686245045747,403.18037625215027, 0.0));
    positionAlloc4->Add (Vector(307.313316827954,401.7511081314201, 0.0));
    positionAlloc4->Add (Vector(207.02299792600314,231.14365166673707, 0.0));
    positionAlloc4->Add (Vector(330.0764777975006,320.48705811831127, 0.0));
    positionAlloc4->Add (Vector(409.9555937326778,23.168813845157676, 0.0));
    positionAlloc4->Add (Vector(276.9745794854944,498.9902995212305, 0.0));
    positionAlloc4->Add (Vector(369.51087665912405,212.84687068858287, 0.0));
    positionAlloc4->Add (Vector(430.34451303801285,362.01611085712597, 0.0));
    positionAlloc4->Add (Vector(388.08670503462747,308.9604727683088, 0.0));
    positionAlloc4->Add (Vector(122.72278812028364,77.99518673387273, 0.0));
    positionAlloc4->Add (Vector(460.34899611111206,483.16651710360657, 0.0));
    positionAlloc4->Add (Vector(437.19133610157456,267.5070175019073, 0.0));
    positionAlloc4->Add (Vector(459.70981111627594,31.49258445184444, 0.0));
    positionAlloc4->Add (Vector(255.14894670061284,473.54108304579523, 0.0));
    positionAlloc4->Add (Vector(440.1324367482371,16.483090689406843, 0.0));
    positionAlloc4->Add (Vector(207.97217866658625,198.1402583942828, 0.0));
    positionAlloc4->Add (Vector(493.6498589731481,284.7016754109396, 0.0));
    positionAlloc4->Add (Vector(197.0037699151539,98.37814115620824, 0.0));
    positionAlloc4->Add (Vector(10.032904715258583,200.10923689181175, 0.0));
    positionAlloc4->Add (Vector(458.12801853988174,369.9668197269362, 0.0));
    positionAlloc4->Add (Vector(288.5649895848618,35.48143138161064, 0.0));
    positionAlloc4->Add (Vector(63.820351077773616,353.99492799605974, 0.0));
    positionAlloc4->Add (Vector(427.4110397163729,113.92573267466472, 0.0));
    positionAlloc4->Add (Vector(174.48664704379064,74.43389157755853, 0.0));
    positionAlloc4->Add (Vector(102.61289802419753,244.97592136454267, 0.0));
    positionAlloc4->Add (Vector(499.11562901660506,207.59741068925285, 0.0));
    positionAlloc4->Add (Vector(371.571838523111,403.38145751011245, 0.0));
    positionAlloc4->Add (Vector(254.68261217038935,135.8930694136158, 0.0));
    positionAlloc4->Add (Vector(267.9956527743509,239.5639132016069, 0.0));
    positionAlloc4->Add (Vector(491.41496266700364,479.0724066965616, 0.0));
    positionAlloc4->Add (Vector(375.52667769292424,94.11194234310855, 0.0));
    positionAlloc4->Add (Vector(26.18897161529804,306.2245234073507, 0.0));
    positionAlloc4->Add (Vector(399.88340732577416,224.9215350916535, 0.0));
    positionAlloc4->Add (Vector(7.61366276037595,315.5106539622604, 0.0));
    positionAlloc4->Add (Vector(25.623768065203222,30.391892517757867, 0.0));
    positionAlloc4->Add (Vector(474.8464263919594,494.6337666209779, 0.0));
    positionAlloc4->Add (Vector(113.92963297992075,164.77027632158098, 0.0));
    positionAlloc4->Add (Vector(396.6955529743614,382.043330330934, 0.0));
    positionAlloc4->Add (Vector(143.32999623606818,197.2414092854421, 0.0));
    positionAlloc4->Add (Vector(408.36319657232724,487.7475884275929, 0.0));
    positionAlloc4->Add (Vector(478.23115945097885,281.8365519813024, 0.0));
    positionAlloc4->Add (Vector(283.05314317514114,98.56599691321316, 0.0));
    positionAlloc4->Add (Vector(392.6671467407139,377.2793703801239, 0.0));
    positionAlloc4->Add (Vector(260.2271033751615,309.6825472267244, 0.0));
    positionAlloc4->Add (Vector(286.32565316869926,206.62692043094938, 0.0));
    positionAlloc4->Add (Vector(226.31235354092067,304.4216191193936, 0.0));
    positionAlloc4->Add (Vector(190.05228048425198,43.74434153958434, 0.0));
    positionAlloc4->Add (Vector(226.2869188041295,303.57053547353837, 0.0));
    positionAlloc4->Add (Vector(66.6287580334426,226.85643943017314, 0.0));
    positionAlloc4->Add (Vector(291.3746952452858,89.31345749060799, 0.0));
    positionAlloc4->Add (Vector(40.376992800930545,288.7463051659295, 0.0));
    positionAlloc4->Add (Vector(338.3982757546559,293.34221812675196, 0.0));
    positionAlloc4->Add (Vector(75.46214874636043,111.58376964508926, 0.0));
    positionAlloc4->Add (Vector(408.695203421239,307.3855668287143, 0.0));
    positionAlloc4->Add (Vector(410.04355718207904,445.8258855990221, 0.0));
    positionAlloc4->Add (Vector(354.9802710593684,318.1546572190053, 0.0));
    positionAlloc4->Add (Vector(81.93547775582338,44.19017195908942, 0.0));
    positionAlloc4->Add (Vector(361.96260063861365,4.845522059720664, 0.0));
    positionAlloc4->Add (Vector(476.2053898916708,129.54085828893415, 0.0));
    positionAlloc4->Add (Vector(481.2849671866805,484.3323122981418, 0.0));
    positionAlloc4->Add (Vector(27.35770434000473,37.18387360945297, 0.0));
    positionAlloc4->Add (Vector(442.6756264520423,60.83536586755833, 0.0));
    positionAlloc4->Add (Vector(436.28215095396666,182.92442642124018, 0.0));
    positionAlloc4->Add (Vector(432.0136833847494,331.5684413040723, 0.0));
    positionAlloc4->Add (Vector(263.12185601268567,11.502539971092629, 0.0));
    positionAlloc4->Add (Vector(25.551154198750815,95.87787686204979, 0.0));
    positionAlloc4->Add (Vector(44.7074168017228,279.9826303125002, 0.0));
    positionAlloc4->Add (Vector(75.94043962289143,203.8753289000611, 0.0));
    positionAlloc4->Add (Vector(259.6075384757594,420.40345949920226, 0.0));
    positionAlloc4->Add (Vector(241.19273803851075,238.66773118475965, 0.0));
    positionAlloc4->Add (Vector(88.6355952265499,150.56884503858464, 0.0));
    positionAlloc4->Add (Vector(216.3020672677144,431.9053583361547, 0.0));
    positionAlloc4->Add (Vector(174.17055493857652,349.8651936894578, 0.0));
    positionAlloc4->Add (Vector(297.3602595635643,158.90169058171227, 0.0));
    positionAlloc4->Add (Vector(332.7635794494767,333.8820845857663, 0.0));
    positionAlloc4->Add (Vector(262.39695915040426,209.4439598309396, 0.0));
    positionAlloc4->Add (Vector(268.95437766205373,466.0155075715046, 0.0));
    positionAlloc4->Add (Vector(277.6869543528269,284.08881959601786, 0.0));
    positionAlloc4->Add (Vector(298.14435513729364,192.26990286285533, 0.0));
    positionAlloc4->Add (Vector(107.70579367696325,424.78559963941456, 0.0));
    positionAlloc4->Add (Vector(236.78574024977956,491.187472319389, 0.0));
    positionAlloc4->Add (Vector(269.1926861961919,33.0463956121047, 0.0));
    positionAlloc4->Add (Vector(275.0280849327467,313.3741577224463, 0.0));
    positionAlloc4->Add (Vector(475.09822831575264,137.27178387138343, 0.0));
    positionAlloc4->Add (Vector(68.73231224505416,459.1327339816871, 0.0));
    positionAlloc4->Add (Vector(451.55762836489845,299.56529416977196, 0.0));
    positionAlloc4->Add (Vector(342.9864497742287,351.5696788092602, 0.0));
    positionAlloc4->Add (Vector(255.52098515259902,414.5225120508565, 0.0));
    positionAlloc4->Add (Vector(277.22822077471915,305.067684101083, 0.0));
    positionAlloc4->Add (Vector(151.54029807783508,94.87611288284958, 0.0));
    positionAlloc4->Add (Vector(270.31223205174433,425.3793864031627, 0.0));
    positionAlloc4->Add (Vector(138.02942788991922,203.84973149713832, 0.0));
    positionAlloc4->Add (Vector(275.3228504225204,124.6208794390326, 0.0));
    positionAlloc4->Add (Vector(204.74780045699202,491.88247116924276, 0.0));
    positionAlloc4->Add (Vector(495.23918056146044,103.69532859015906, 0.0));
    positionAlloc4->Add (Vector(173.02597034294692,169.9856312784766, 0.0));
    positionAlloc4->Add (Vector(281.8993088222242,164.4026956826851, 0.0));
    positionAlloc4->Add (Vector(91.93316867157175,309.13996778996443, 0.0));
    positionAlloc4->Add (Vector(388.1410774038373,289.44230583312725, 0.0));
    positionAlloc4->Add (Vector(183.5621119943212,81.06344541910721, 0.0));
    positionAlloc4->Add (Vector(336.86606972189276,265.3876296682045, 0.0));
    positionAlloc4->Add (Vector(307.8657171657691,85.95455367633559, 0.0));
    positionAlloc4->Add (Vector(201.07388073994625,136.47631686741863, 0.0));
    positionAlloc4->Add (Vector(448.2296639726954,402.73506853209017, 0.0));
    positionAlloc4->Add (Vector(336.0512110488361,8.308924927032002, 0.0));
    positionAlloc4->Add (Vector(117.846978324187,451.0283312873954, 0.0));
    positionAlloc4->Add (Vector(77.85257431307524,300.5758560715825, 0.0));
    positionAlloc4->Add (Vector(109.01270884609137,179.72941220687184, 0.0));
    positionAlloc4->Add (Vector(180.99483037300195,47.0829316333371, 0.0));
    positionAlloc4->Add (Vector(471.51678376602473,106.66759261098102, 0.0));
    positionAlloc4->Add (Vector(415.2352943189129,172.6953973760213, 0.0));
    positionAlloc4->Add (Vector(244.5740969819214,105.98742254103661, 0.0));
    positionAlloc4->Add (Vector(398.645160310429,221.9219985307757, 0.0));
    positionAlloc4->Add (Vector(302.1688833273318,390.86447049878893, 0.0));
    positionAlloc4->Add (Vector(446.593761134608,67.22811667975066, 0.0));
    positionAlloc4->Add (Vector(163.9209882534192,132.2114690549276, 0.0));
    positionAlloc4->Add (Vector(392.57679645680173,435.3178598435536, 0.0));
    positionAlloc4->Add (Vector(362.1260434894847,441.5089400138993, 0.0));
    positionAlloc4->Add (Vector(50.123175359015946,111.1604727003893, 0.0));
    positionAlloc4->Add (Vector(208.20276407724185,261.84708229675795, 0.0));
    positionAlloc4->Add (Vector(493.84805564308937,140.27031415201884, 0.0));
    positionAlloc4->Add (Vector(357.1902417913244,272.7115125906779, 0.0));
    positionAlloc4->Add (Vector(317.62466327844606,56.846235667027955, 0.0));
    positionAlloc4->Add (Vector(116.05451764722191,435.30058806397295, 0.0));
    positionAlloc4->Add (Vector(388.08259352323273,268.01130624625836, 0.0));
    positionAlloc4->Add (Vector(318.4332096000023,68.63166874846132, 0.0));
    positionAlloc4->Add (Vector(42.862727415995174,262.5347179300741, 0.0));
    positionAlloc4->Add (Vector(424.83452950697864,96.04722065627391, 0.0));
    positionAlloc4->Add (Vector(443.0222950384058,436.0635211883466, 0.0));
    positionAlloc4->Add (Vector(121.08862626422656,19.25263897859397, 0.0));
    positionAlloc4->Add (Vector(67.70502179397702,12.90432461870994, 0.0));
    positionAlloc4->Add (Vector(243.21626479738688,425.57100217303434, 0.0));
    positionAlloc4->Add (Vector(381.3720455002687,372.59549734480163, 0.0));
    positionAlloc4->Add (Vector(318.5308468764574,455.4471388763746, 0.0));
    positionAlloc4->Add (Vector(468.8150145205592,193.4854575196028, 0.0));
    positionAlloc4->Add (Vector(162.25763082732618,48.54106729712954, 0.0));
    positionAlloc4->Add (Vector(469.7425759470366,283.9401676364637, 0.0));
    positionAlloc4->Add (Vector(271.7529084899719,15.667766624476553, 0.0));
    positionAlloc4->Add (Vector(172.19613802751988,75.91449085647162, 0.0));
    positionAlloc4->Add (Vector(297.1784906902305,482.8420832178559, 0.0));
    positionAlloc4->Add (Vector(214.35073604910093,59.09738387864161, 0.0));
    positionAlloc4->Add (Vector(256.09709030060543,371.29816840610897, 0.0));
    positionAlloc4->Add (Vector(398.86254827438796,226.128656421016, 0.0));
    positionAlloc4->Add (Vector(333.53453036540606,461.9543100316426, 0.0));
    positionAlloc4->Add (Vector(249.43160279541098,160.53189081824598, 0.0));
    positionAlloc4->Add (Vector(489.8190874107775,73.11744175903983, 0.0));
    positionAlloc4->Add (Vector(374.08262040403173,69.52045366669729, 0.0));
    positionAlloc4->Add (Vector(155.1288476127533,486.0012070152908, 0.0));
    positionAlloc4->Add (Vector(88.02871647523591,245.53216462799455, 0.0));
    positionAlloc4->Add (Vector(177.46929005204836,333.71683016730583, 0.0));
    positionAlloc4->Add (Vector(173.70225721861465,88.87139365708269, 0.0));
    positionAlloc4->Add (Vector(361.4715042270987,274.1455265770974, 0.0));
    positionAlloc4->Add (Vector(80.38633707258246,421.4088107487963, 0.0));
    positionAlloc4->Add (Vector(105.2924742794083,427.93819237750756, 0.0));
    positionAlloc4->Add (Vector(177.78120321048357,335.66976485812364, 0.0));
    positionAlloc4->Add (Vector(226.8282222489736,14.156124218795807, 0.0));
    positionAlloc4->Add (Vector(461.32463380817705,264.77943142707466, 0.0));
    positionAlloc4->Add (Vector(306.4820784310717,378.71728544468255, 0.0));
    positionAlloc4->Add (Vector(201.2032673975584,404.48040351913977, 0.0));
    positionAlloc4->Add (Vector(28.42768374152166,48.10979649023095, 0.0));
    positionAlloc4->Add (Vector(339.0257563417648,163.50794350175784, 0.0));
    positionAlloc4->Add (Vector(483.00091791115585,214.958921530804, 0.0));
    positionAlloc4->Add (Vector(117.19768626390243,9.871421806908865, 0.0));
    positionAlloc4->Add (Vector(179.179774108969,271.05422835364294, 0.0));
    positionAlloc4->Add (Vector(483.96600444287867,65.1088906089768, 0.0));
    positionAlloc4->Add (Vector(166.56726278742724,394.0721706184378, 0.0));
    positionAlloc4->Add (Vector(487.69405963951243,201.52792599085956, 0.0));
    positionAlloc4->Add (Vector(130.4060573086639,88.84201082106335, 0.0));
    positionAlloc4->Add (Vector(191.9062700783165,8.203733833535843, 0.0));
    positionAlloc4->Add (Vector(30.300015697157402,134.77157283741147, 0.0));
    positionAlloc4->Add (Vector(493.07081159658907,484.0920502022171, 0.0));
    positionAlloc4->Add (Vector(458.4135253860762,391.6033404295532, 0.0));
    positionAlloc4->Add (Vector(126.18226937665783,18.12361013275193, 0.0));
    positionAlloc4->Add (Vector(396.4158879447011,31.40253866904985, 0.0));
    positionAlloc4->Add (Vector(293.35211708954597,215.99145787661305, 0.0));
    positionAlloc4->Add (Vector(312.67487336571764,485.3479502784252, 0.0));
    positionAlloc4->Add (Vector(360.5183694128675,66.83742966197198, 0.0));
    positionAlloc4->Add (Vector(251.57619201553211,111.5558296604115, 0.0));
    positionAlloc4->Add (Vector(419.28987606701736,261.6877338166691, 0.0));
    positionAlloc4->Add (Vector(2.8309930730269217,90.73422125314329, 0.0));
    positionAlloc4->Add (Vector(403.9259567468052,401.6265577096767, 0.0));
    positionAlloc4->Add (Vector(234.8067614135922,276.5570032763348, 0.0));
    positionAlloc4->Add (Vector(144.4559236913388,2.821411015367037, 0.0));
    positionAlloc4->Add (Vector(244.33460652126914,378.6236983857425, 0.0));
    positionAlloc4->Add (Vector(332.7645559709308,264.34415625382013, 0.0));
    positionAlloc4->Add (Vector(367.25165364784965,112.18415024764833, 0.0));
    positionAlloc4->Add (Vector(98.18623476936983,98.11253001211733, 0.0));
    positionAlloc4->Add (Vector(351.3935402033629,241.9586166892515, 0.0));
    positionAlloc4->Add (Vector(414.86598133127455,32.309435827356516, 0.0));
    positionAlloc4->Add (Vector(454.81666000028633,33.16054715425704, 0.0));
    positionAlloc4->Add (Vector(445.0871650394775,105.57002894455036, 0.0));
    positionAlloc4->Add (Vector(65.02597945844208,278.4063375143285, 0.0));
    positionAlloc4->Add (Vector(386.5362569662658,10.24374139372275, 0.0));
    positionAlloc4->Add (Vector(49.866290527885994,33.43710969559366, 0.0));
    positionAlloc4->Add (Vector(191.7844901089737,458.0772840840421, 0.0));
    positionAlloc4->Add (Vector(226.93732374027758,469.352253837881, 0.0));
    positionAlloc4->Add (Vector(321.5441477563503,133.502773078272, 0.0));
    positionAlloc4->Add (Vector(272.69053883498356,187.24616456495147, 0.0));
    positionAlloc4->Add (Vector(401.91235247924135,347.58660005020005, 0.0));
    positionAlloc4->Add (Vector(59.117719006303474,447.3124234250022, 0.0));
    positionAlloc4->Add (Vector(224.55692289226337,352.27081540324144, 0.0));
    positionAlloc4->Add (Vector(52.882414709040205,240.3159782269812, 0.0));
    positionAlloc4->Add (Vector(81.86126391611221,55.38463864532262, 0.0));
    positionAlloc4->Add (Vector(67.9698260105866,338.01447963873426, 0.0));
    positionAlloc4->Add (Vector(359.9319546288331,248.71976067270796, 0.0));
    positionAlloc4->Add (Vector(21.150783228258476,19.46671403821337, 0.0));
    positionAlloc4->Add (Vector(170.30096051704146,485.4801336602583, 0.0));
    positionAlloc4->Add (Vector(428.840920637207,41.18291342423963, 0.0));
    positionAlloc4->Add (Vector(247.25689138253443,41.413273801394425, 0.0));
    positionAlloc4->Add (Vector(100.91184931800467,456.81018517669366, 0.0));
    positionAlloc4->Add (Vector(430.7736575515026,330.80011705123746, 0.0));
    positionAlloc4->Add (Vector(151.75893804182877,43.181133967210016, 0.0));
    positionAlloc4->Add (Vector(296.7214439251733,57.27976244815009, 0.0));
    positionAlloc4->Add (Vector(115.29188628346643,202.58651678082367, 0.0));
    positionAlloc4->Add (Vector(450.56962950466783,466.00795584505676, 0.0));
    positionAlloc4->Add (Vector(358.42693242600564,378.83420237606396, 0.0));
    positionAlloc4->Add (Vector(133.01723613914334,133.37166389796167, 0.0));
    positionAlloc4->Add (Vector(496.4902778563387,196.31139625254417, 0.0));
    positionAlloc4->Add (Vector(308.07303461630113,116.01394565109257, 0.0));
    positionAlloc4->Add (Vector(498.1528798408489,361.6830117529429, 0.0));
    positionAlloc4->Add (Vector(100.87702145126337,438.16237133965257, 0.0));
    positionAlloc4->Add (Vector(358.8643708721764,278.76011968829977, 0.0));
    positionAlloc4->Add (Vector(165.9890452321533,205.51235981894393, 0.0));
    positionAlloc4->Add (Vector(18.768021166920278,200.64015996669082, 0.0));
    positionAlloc4->Add (Vector(261.201690401546,6.819612093314998, 0.0));
    positionAlloc4->Add (Vector(25.59863936363288,246.0652089660295, 0.0));
    positionAlloc4->Add (Vector(290.4462239097485,497.4337057238073, 0.0));
    positionAlloc4->Add (Vector(494.99257247253314,460.01012994725687, 0.0));
    positionAlloc4->Add (Vector(448.8367777491669,389.41195517880925, 0.0));
    positionAlloc4->Add (Vector(24.341027712895425,463.9842832302023, 0.0));
    positionAlloc4->Add (Vector(334.76318018650494,278.9426467556104, 0.0));
    positionAlloc4->Add (Vector(210.53379178464237,359.50048529296316, 0.0));
    positionAlloc4->Add (Vector(268.94545347874913,458.48870828012724, 0.0));
    positionAlloc4->Add (Vector(77.29068741476081,228.42947623921017, 0.0));
    positionAlloc4->Add (Vector(474.81107804414734,37.93436441198794, 0.0));
    positionAlloc4->Add (Vector(163.23285106771667,477.9801296710303, 0.0));
    positionAlloc4->Add (Vector(114.11608023074233,499.6031811678785, 0.0));
    positionAlloc4->Add (Vector(83.59139800701665,138.10238087394023, 0.0));
    positionAlloc4->Add (Vector(116.20633479454612,143.90017456247472, 0.0));
    positionAlloc4->Add (Vector(96.87378081833353,475.2097659424924, 0.0));
    positionAlloc4->Add (Vector(137.45132911214247,282.8944370602902, 0.0));
    positionAlloc4->Add (Vector(429.65487088668016,28.985435047581333, 0.0));
    positionAlloc4->Add (Vector(162.5958649176167,3.316784331662437, 0.0));
    positionAlloc4->Add (Vector(191.57100433236707,77.34784510066007, 0.0));
    positionAlloc4->Add (Vector(30.924414580165262,454.6729785941767, 0.0));
    positionAlloc4->Add (Vector(40.097934945437565,252.0343282978315, 0.0));
    positionAlloc4->Add (Vector(396.26291946314876,353.414821866255, 0.0));
    positionAlloc4->Add (Vector(196.6277896802532,64.9973266089623, 0.0));
    positionAlloc4->Add (Vector(282.2187907574968,452.0533391381506, 0.0));
    positionAlloc4->Add (Vector(106.63250176748956,232.86491045212088, 0.0));
    positionAlloc4->Add (Vector(54.598782469689354,181.172763838087, 0.0));
    positionAlloc4->Add (Vector(183.89321505212698,43.124388808032656, 0.0));
    positionAlloc4->Add (Vector(466.1555641418843,277.0128569294018, 0.0));
    positionAlloc4->Add (Vector(252.88476793158975,172.51737745248997, 0.0));
    positionAlloc4->Add (Vector(390.1823176188442,456.41769508016347, 0.0));
    positionAlloc4->Add (Vector(64.17972100851283,99.7738887882863, 0.0));
    positionAlloc4->Add (Vector(167.20152716733205,68.9851450760316, 0.0));
    positionAlloc4->Add (Vector(208.51109291940318,428.2425509985791, 0.0));
    positionAlloc4->Add (Vector(167.68610530501937,114.55720055256374, 0.0));
    positionAlloc4->Add (Vector(396.2723558678101,192.88600013197387, 0.0));
    positionAlloc4->Add (Vector(242.62511208823346,357.19342023423854, 0.0));
    positionAlloc4->Add (Vector(389.93611652492984,464.4206464330811, 0.0));
    positionAlloc4->Add (Vector(362.30245244346645,165.83602951536054, 0.0));
    positionAlloc4->Add (Vector(381.591835195857,261.47999015232097, 0.0));
    positionAlloc4->Add (Vector(445.00351039529016,56.52231602371383, 0.0));
    positionAlloc4->Add (Vector(72.72803132369987,275.8471272407443, 0.0));
    positionAlloc4->Add (Vector(95.82749548319586,322.59034111429526, 0.0));
    positionAlloc4->Add (Vector(435.57051142417953,479.4061179373546, 0.0));
    positionAlloc4->Add (Vector(432.08914074786253,163.1970188442295, 0.0));
    positionAlloc4->Add (Vector(40.86648237246654,395.10110943307484, 0.0));
    positionAlloc4->Add (Vector(220.9187851144397,39.258722645347355, 0.0));
    positionAlloc4->Add (Vector(91.7170031305748,499.7696848684623, 0.0));
    positionAlloc4->Add (Vector(337.5104208285574,478.02499770040595, 0.0));
    positionAlloc4->Add (Vector(410.68856618653024,291.5036604350011, 0.0));
    positionAlloc4->Add (Vector(378.87747875497917,353.616384309378, 0.0));
    positionAlloc4->Add (Vector(321.1237755413133,371.207607928907, 0.0));
    positionAlloc4->Add (Vector(12.294651013893487,344.5106655389244, 0.0));
    positionAlloc4->Add (Vector(454.78200472587315,272.8619501610625, 0.0));
    positionAlloc4->Add (Vector(145.3395767812466,496.15445999000076, 0.0));
    positionAlloc4->Add (Vector(201.0154945983487,308.5123339450057, 0.0));
    positionAlloc4->Add (Vector(369.96135505163966,72.77255459352556, 0.0));
    positionAlloc4->Add (Vector(303.68589098910104,174.8175638212683, 0.0));
    positionAlloc4->Add (Vector(424.1717077495857,418.3542560972933, 0.0));
    positionAlloc4->Add (Vector(330.2721390840762,210.78080468168392, 0.0));
    positionAlloc4->Add (Vector(297.2365533380893,143.0626199848959, 0.0));
    positionAlloc4->Add (Vector(338.8133498803236,412.2137071212112, 0.0));
    positionAlloc4->Add (Vector(376.5781813517345,316.15583055988395, 0.0));
    positionAlloc4->Add (Vector(206.7798965964361,36.219923896355034, 0.0));
    positionAlloc4->Add (Vector(117.34219511645055,139.85138769496226, 0.0));
    positionAlloc4->Add (Vector(147.4865807130849,457.53793929117927, 0.0));
    positionAlloc4->Add (Vector(122.22070388061319,303.48171529965526, 0.0));
    positionAlloc4->Add (Vector(319.1842314838613,65.29171065424077, 0.0));
    positionAlloc4->Add (Vector(191.36937594503723,96.99910906355042, 0.0));
    positionAlloc4->Add (Vector(215.81188678944557,15.291000827434454, 0.0));
    positionAlloc4->Add (Vector(344.080803670741,9.721132875123661, 0.0));
    positionAlloc4->Add (Vector(244.6192041788619,426.93458249713507, 0.0));
    positionAlloc4->Add (Vector(444.95646820117116,115.52126389141937, 0.0));
    positionAlloc4->Add (Vector(143.46028080744483,126.47875889715004, 0.0));
    positionAlloc4->Add (Vector(333.691855826058,494.06132300094447, 0.0));
    positionAlloc4->Add (Vector(151.73977266742273,204.36086666869818, 0.0));
    positionAlloc4->Add (Vector(394.6502088195004,304.2043932910134, 0.0));
    positionAlloc4->Add (Vector(125.16554065872576,85.67291025325562, 0.0));
    positionAlloc4->Add (Vector(94.27756156869593,341.4919059625068, 0.0));
    positionAlloc4->Add (Vector(18.739714257729357,91.91859060137203, 0.0));
    positionAlloc4->Add (Vector(348.25185086956634,203.3819739665307, 0.0));
    positionAlloc4->Add (Vector(478.9769064079775,487.44031773224, 0.0));
    positionAlloc4->Add (Vector(118.33808384839439,445.9656434302169, 0.0));
    positionAlloc4->Add (Vector(17.539336429609587,352.3181944913188, 0.0));
    positionAlloc4->Add (Vector(464.44720670214815,421.541570936827, 0.0));
    positionAlloc4->Add (Vector(57.98147506957096,26.716896761173782, 0.0));
    positionAlloc4->Add (Vector(194.36764495548863,91.33245007434365, 0.0));
    positionAlloc4->Add (Vector(224.6736900790539,237.0160356945223, 0.0));
    positionAlloc4->Add (Vector(431.46418593571957,341.2627880213646, 0.0));
    positionAlloc4->Add (Vector(204.7131699816229,443.5933767767556, 0.0));
    positionAlloc4->Add (Vector(451.17914306840436,493.5049290901, 0.0));
    positionAlloc4->Add (Vector(214.8959157853569,141.607753659991, 0.0));
    positionAlloc4->Add (Vector(314.2757968927409,221.00195803018784, 0.0));
    positionAlloc4->Add (Vector(309.34975196548504,428.53592402801485, 0.0));
    positionAlloc4->Add (Vector(42.59528594502887,364.2163787874721, 0.0));
    positionAlloc4->Add (Vector(424.64833588455326,188.15166206359203, 0.0));
    positionAlloc4->Add (Vector(191.60163273994718,494.0274207947767, 0.0));
    positionAlloc4->Add (Vector(224.08650419181953,396.9815644809878, 0.0));
    positionAlloc4->Add (Vector(219.10737738191588,125.6902741690326, 0.0));
    positionAlloc4->Add (Vector(445.53731425239926,391.4114741526662, 0.0));
    positionAlloc4->Add (Vector(268.1143476636882,134.87171006685423, 0.0));
    positionAlloc4->Add (Vector(196.29323286948764,422.1067284799943, 0.0));
    positionAlloc4->Add (Vector(12.406689847797914,157.7187628421125, 0.0));
    positionAlloc4->Add (Vector(165.77378032288897,174.7280159448175, 0.0));
    positionAlloc4->Add (Vector(483.38720399186377,361.85136328522873, 0.0));
    positionAlloc4->Add (Vector(146.58002352143478,1.1439414281198546, 0.0));
    positionAlloc4->Add (Vector(365.4548289319147,459.9123201480374, 0.0));
    positionAlloc4->Add (Vector(352.76287899397124,441.5013950948096, 0.0));
    positionAlloc4->Add (Vector(198.7168301703131,410.1636397423083, 0.0));
    positionAlloc4->Add (Vector(330.46020558525254,388.6881833118545, 0.0));
    positionAlloc4->Add (Vector(189.96589774682315,15.8707064642703, 0.0));
    positionAlloc4->Add (Vector(21.53655397927795,471.8360387561336, 0.0));
    positionAlloc4->Add (Vector(298.0930998518983,246.35976733294785, 0.0));
    positionAlloc4->Add (Vector(284.1216870627661,421.7643231749185, 0.0));
    positionAlloc4->Add (Vector(460.6287794237868,442.95742681178103, 0.0));
    positionAlloc4->Add (Vector(128.89906249283717,421.16642088396435, 0.0));
    positionAlloc4->Add (Vector(155.73973085237125,173.36946764563922, 0.0));
    positionAlloc4->Add (Vector(327.60029862635776,63.53497555864368, 0.0));
    positionAlloc4->Add (Vector(215.9616320668538,469.316003275656, 0.0));
    positionAlloc4->Add (Vector(247.66769355393998,217.80700805819453, 0.0));
    positionAlloc4->Add (Vector(244.56157298304936,340.0439690187213, 0.0));
    positionAlloc4->Add (Vector(41.25466892320356,20.22544406252419, 0.0));
    positionAlloc4->Add (Vector(440.1804191025136,136.0990709624283, 0.0));
    positionAlloc4->Add (Vector(195.21350569345603,69.56464172340453, 0.0));
    positionAlloc4->Add (Vector(271.913373021002,456.5468544737629, 0.0));
    positionAlloc4->Add (Vector(312.7196290482552,419.2630467931599, 0.0));
    positionAlloc4->Add (Vector(113.70930564571712,472.82704421263657, 0.0));
    positionAlloc4->Add (Vector(267.5832762928228,332.2865026073337, 0.0));
    positionAlloc4->Add (Vector(400.1731462698113,106.36270714664303, 0.0));
    positionAlloc4->Add (Vector(392.04276103298434,335.50295114160065, 0.0));
    positionAlloc4->Add (Vector(225.15462098898604,169.21534365027463, 0.0));
    positionAlloc4->Add (Vector(383.6147667706337,327.7319281053307, 0.0));
    positionAlloc4->Add (Vector(293.10940405905154,110.31716058806651, 0.0));
    positionAlloc4->Add (Vector(348.1958610285955,175.7554629924931, 0.0));
    positionAlloc4->Add (Vector(438.25906447292556,318.39880117379454, 0.0));
    positionAlloc4->Add (Vector(426.1581416938383,19.391204986017485, 0.0));
    positionAlloc4->Add (Vector(216.26100829203648,348.02194470746605, 0.0));
    positionAlloc4->Add (Vector(201.54938773397984,343.3942413809018, 0.0));
    positionAlloc4->Add (Vector(374.81044176875963,115.23824366113145, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_23.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_ECC_flowmon/switch_ECC_23.flowmon", false, false);
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