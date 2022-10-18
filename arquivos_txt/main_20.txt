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
	  uint16_t numberOfRrhs = 42;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 600;
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
  	Config::SetDefault ("ns3::LteEnbNetDevice::DlEarfcn", UintegerValue (3100));
  	Config::SetDefault ("ns3::LteEnbNetDevice::UlEarfcn", UintegerValue (21100));
  	Config::SetDefault ("ns3::LteUeNetDevice::DlEarfcn", UintegerValue (3100));
  	// Potência de transmissão da eNb em dBm
  	Config::SetDefault ("ns3::LteEnbPhy::TxPower", UintegerValue (33));

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
    positionAlloc->Add (Vector (444.44444444444446,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,0.0, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,500.0, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,166.66666666666666, 0.0));
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
    positionAlloc4->Add (Vector(9.479571864308644,100.22483534824921, 0.0));
    positionAlloc4->Add (Vector(391.46251177490177,218.8967910550419, 0.0));
    positionAlloc4->Add (Vector(199.26719710810926,5.841368556432414, 0.0));
    positionAlloc4->Add (Vector(350.6997020700089,242.25318384563383, 0.0));
    positionAlloc4->Add (Vector(127.06209924642675,142.75671722456178, 0.0));
    positionAlloc4->Add (Vector(230.47497398132427,201.99194043516698, 0.0));
    positionAlloc4->Add (Vector(477.8028897375838,151.0729293310382, 0.0));
    positionAlloc4->Add (Vector(194.8744450336497,442.9052565004105, 0.0));
    positionAlloc4->Add (Vector(369.7862677231067,428.93890591063274, 0.0));
    positionAlloc4->Add (Vector(187.38989141766993,305.0768428877024, 0.0));
    positionAlloc4->Add (Vector(254.38202238686648,233.42254658746504, 0.0));
    positionAlloc4->Add (Vector(335.5395568934417,159.3936210048638, 0.0));
    positionAlloc4->Add (Vector(80.16109001086879,406.02110014667437, 0.0));
    positionAlloc4->Add (Vector(415.159128591151,425.0398581394144, 0.0));
    positionAlloc4->Add (Vector(72.54590060173355,353.92485625378947, 0.0));
    positionAlloc4->Add (Vector(288.7849517745399,328.3610898973559, 0.0));
    positionAlloc4->Add (Vector(200.31485853244797,374.3846689989069, 0.0));
    positionAlloc4->Add (Vector(488.9801916323742,148.50958280496923, 0.0));
    positionAlloc4->Add (Vector(393.0110211168775,324.90871204239045, 0.0));
    positionAlloc4->Add (Vector(337.1816717952331,479.7930462784301, 0.0));
    positionAlloc4->Add (Vector(461.54597889062217,494.5599381339788, 0.0));
    positionAlloc4->Add (Vector(480.43085453563754,50.8694230604137, 0.0));
    positionAlloc4->Add (Vector(89.42873250462807,454.7561006476392, 0.0));
    positionAlloc4->Add (Vector(293.17766867497994,296.6314159764641, 0.0));
    positionAlloc4->Add (Vector(398.5245027096076,429.9408081901321, 0.0));
    positionAlloc4->Add (Vector(72.04056568044248,463.85606604046035, 0.0));
    positionAlloc4->Add (Vector(459.4653092059077,409.7461065966363, 0.0));
    positionAlloc4->Add (Vector(312.8947229950781,142.87346225996484, 0.0));
    positionAlloc4->Add (Vector(444.9124641804839,279.94117674205114, 0.0));
    positionAlloc4->Add (Vector(492.5229656810825,39.72656704399014, 0.0));
    positionAlloc4->Add (Vector(428.99231274336154,443.5777022375418, 0.0));
    positionAlloc4->Add (Vector(406.92191306270604,99.62493619349061, 0.0));
    positionAlloc4->Add (Vector(102.62656765064715,130.73029471120896, 0.0));
    positionAlloc4->Add (Vector(125.83645219502382,141.93176631445453, 0.0));
    positionAlloc4->Add (Vector(258.44106494428536,28.64786372056527, 0.0));
    positionAlloc4->Add (Vector(315.63936090575123,256.9233939309298, 0.0));
    positionAlloc4->Add (Vector(453.035212466445,494.4867282842573, 0.0));
    positionAlloc4->Add (Vector(410.53215594317345,154.33321333249046, 0.0));
    positionAlloc4->Add (Vector(222.3583222771046,81.66015681563854, 0.0));
    positionAlloc4->Add (Vector(217.22750254546753,155.21828776968738, 0.0));
    positionAlloc4->Add (Vector(116.30081898149264,131.5071204227472, 0.0));
    positionAlloc4->Add (Vector(377.7082203810333,331.38853094893676, 0.0));
    positionAlloc4->Add (Vector(402.38315504525406,367.42231606088103, 0.0));
    positionAlloc4->Add (Vector(451.8774269578915,116.5722244171632, 0.0));
    positionAlloc4->Add (Vector(361.25776232477426,386.44774495331006, 0.0));
    positionAlloc4->Add (Vector(254.28007420387493,1.3675654333012432, 0.0));
    positionAlloc4->Add (Vector(290.01271622311924,69.83580720391075, 0.0));
    positionAlloc4->Add (Vector(484.45923636973066,405.7451523537018, 0.0));
    positionAlloc4->Add (Vector(134.50988888823517,92.25436594812992, 0.0));
    positionAlloc4->Add (Vector(321.87977816619224,126.98560429366029, 0.0));
    positionAlloc4->Add (Vector(181.8788052905357,296.42506868935993, 0.0));
    positionAlloc4->Add (Vector(200.44164937492465,492.1229212710797, 0.0));
    positionAlloc4->Add (Vector(149.93114975734807,417.9221220040266, 0.0));
    positionAlloc4->Add (Vector(317.94240406304397,121.8876849072596, 0.0));
    positionAlloc4->Add (Vector(51.922320399004406,40.353440018152774, 0.0));
    positionAlloc4->Add (Vector(171.12787158551257,196.0166320538565, 0.0));
    positionAlloc4->Add (Vector(479.228708850779,62.3597662884981, 0.0));
    positionAlloc4->Add (Vector(399.2980324277167,387.017687072504, 0.0));
    positionAlloc4->Add (Vector(13.120587435043907,415.3947998303094, 0.0));
    positionAlloc4->Add (Vector(256.85393782088516,397.0104036411086, 0.0));
    positionAlloc4->Add (Vector(207.2485979921756,293.8033949367527, 0.0));
    positionAlloc4->Add (Vector(293.2724515328515,437.0911754694906, 0.0));
    positionAlloc4->Add (Vector(422.9151929514614,77.12233228348886, 0.0));
    positionAlloc4->Add (Vector(272.96717573785503,218.77891756903523, 0.0));
    positionAlloc4->Add (Vector(15.207465867424386,4.282290045749482, 0.0));
    positionAlloc4->Add (Vector(362.49723369611166,294.04663085092056, 0.0));
    positionAlloc4->Add (Vector(98.92415082379696,181.00256941234622, 0.0));
    positionAlloc4->Add (Vector(247.69393582759812,491.8019709615288, 0.0));
    positionAlloc4->Add (Vector(7.527497714806374,432.7330768615357, 0.0));
    positionAlloc4->Add (Vector(121.09620382558533,293.70052366154135, 0.0));
    positionAlloc4->Add (Vector(262.2379989228407,214.70150147907634, 0.0));
    positionAlloc4->Add (Vector(242.98367172304185,140.23796488890994, 0.0));
    positionAlloc4->Add (Vector(30.790365984506796,438.88595480110234, 0.0));
    positionAlloc4->Add (Vector(156.3290648519273,469.1217815640683, 0.0));
    positionAlloc4->Add (Vector(394.17974564531306,403.0615554507444, 0.0));
    positionAlloc4->Add (Vector(384.7618780309048,347.9642082980792, 0.0));
    positionAlloc4->Add (Vector(138.07310718997306,249.1167147372985, 0.0));
    positionAlloc4->Add (Vector(329.3421065351515,163.5867174961268, 0.0));
    positionAlloc4->Add (Vector(266.4496453347194,19.85818113095411, 0.0));
    positionAlloc4->Add (Vector(85.21974020480339,83.82021283427166, 0.0));
    positionAlloc4->Add (Vector(184.5719162660877,370.9302479487316, 0.0));
    positionAlloc4->Add (Vector(280.79485862833207,285.1863971406398, 0.0));
    positionAlloc4->Add (Vector(320.31714118954966,297.5533046782374, 0.0));
    positionAlloc4->Add (Vector(269.7006170876284,139.21601201941857, 0.0));
    positionAlloc4->Add (Vector(484.6983405994519,170.48664019954484, 0.0));
    positionAlloc4->Add (Vector(93.5645763088025,303.98456146218734, 0.0));
    positionAlloc4->Add (Vector(284.02385992798486,82.8246561601641, 0.0));
    positionAlloc4->Add (Vector(391.7637345902512,43.486442597711395, 0.0));
    positionAlloc4->Add (Vector(173.62703797820106,414.89256795235264, 0.0));
    positionAlloc4->Add (Vector(247.888868453966,164.18158637826397, 0.0));
    positionAlloc4->Add (Vector(195.72629538687514,70.66946830449916, 0.0));
    positionAlloc4->Add (Vector(393.32748786859,430.90073329938707, 0.0));
    positionAlloc4->Add (Vector(204.73221988789592,118.22174411877818, 0.0));
    positionAlloc4->Add (Vector(290.016275549417,251.4385352462507, 0.0));
    positionAlloc4->Add (Vector(147.42925088808894,281.7876839865299, 0.0));
    positionAlloc4->Add (Vector(487.35304833340865,233.50818825903642, 0.0));
    positionAlloc4->Add (Vector(204.95632027671522,408.0609593552473, 0.0));
    positionAlloc4->Add (Vector(380.80833554957894,198.6357044757922, 0.0));
    positionAlloc4->Add (Vector(468.3347648737144,447.59988506617526, 0.0));
    positionAlloc4->Add (Vector(94.60257722161015,159.21351938023398, 0.0));
    positionAlloc4->Add (Vector(216.2906903117785,313.4236724665433, 0.0));
    positionAlloc4->Add (Vector(299.1342836377587,228.6253805140113, 0.0));
    positionAlloc4->Add (Vector(28.421555044074452,419.1093015040138, 0.0));
    positionAlloc4->Add (Vector(417.2515876440469,445.2586176122067, 0.0));
    positionAlloc4->Add (Vector(23.887289376274325,396.19854095854055, 0.0));
    positionAlloc4->Add (Vector(409.01362521051345,372.08539868491164, 0.0));
    positionAlloc4->Add (Vector(333.2444949468902,35.437838167317494, 0.0));
    positionAlloc4->Add (Vector(99.93619793138609,137.7713946947678, 0.0));
    positionAlloc4->Add (Vector(122.56171341387994,158.6241933597997, 0.0));
    positionAlloc4->Add (Vector(89.18140392682061,90.92945987251622, 0.0));
    positionAlloc4->Add (Vector(218.13959596446963,3.907287840473339, 0.0));
    positionAlloc4->Add (Vector(376.5496327015753,112.59782867019608, 0.0));
    positionAlloc4->Add (Vector(378.08187985076853,133.6752849222861, 0.0));
    positionAlloc4->Add (Vector(116.02018065324732,273.99678429824684, 0.0));
    positionAlloc4->Add (Vector(437.0633044376118,468.1008015944845, 0.0));
    positionAlloc4->Add (Vector(150.0989556328164,110.71426088539998, 0.0));
    positionAlloc4->Add (Vector(55.63837320414977,303.2810475809846, 0.0));
    positionAlloc4->Add (Vector(293.0020101102039,180.38545874874546, 0.0));
    positionAlloc4->Add (Vector(226.09760104456473,414.50507174131064, 0.0));
    positionAlloc4->Add (Vector(473.06400621047027,0.5056835569599571, 0.0));
    positionAlloc4->Add (Vector(280.77352799976285,188.24898938262436, 0.0));
    positionAlloc4->Add (Vector(297.1098643575514,181.7690093609196, 0.0));
    positionAlloc4->Add (Vector(223.699068401858,132.7106504539835, 0.0));
    positionAlloc4->Add (Vector(205.78234755882508,187.68214438619495, 0.0));
    positionAlloc4->Add (Vector(142.31576946807277,489.1573417243267, 0.0));
    positionAlloc4->Add (Vector(280.2081359031979,48.85224198380478, 0.0));
    positionAlloc4->Add (Vector(66.85036680484984,68.77906500582715, 0.0));
    positionAlloc4->Add (Vector(189.45745701964088,353.1054593479207, 0.0));
    positionAlloc4->Add (Vector(38.79131555471316,98.89940740094211, 0.0));
    positionAlloc4->Add (Vector(131.32899542819428,55.58097440012855, 0.0));
    positionAlloc4->Add (Vector(53.839881415235716,279.4017508007801, 0.0));
    positionAlloc4->Add (Vector(112.18846533750931,419.7064935027281, 0.0));
    positionAlloc4->Add (Vector(49.495197698663006,275.3407583187601, 0.0));
    positionAlloc4->Add (Vector(338.6005498252646,399.0605697693314, 0.0));
    positionAlloc4->Add (Vector(471.3197335283632,81.19458306678922, 0.0));
    positionAlloc4->Add (Vector(329.271400867017,250.90418455354973, 0.0));
    positionAlloc4->Add (Vector(268.89942385340316,39.98110573520286, 0.0));
    positionAlloc4->Add (Vector(306.36360658013183,301.4777038281745, 0.0));
    positionAlloc4->Add (Vector(71.7909967025377,279.9160618153298, 0.0));
    positionAlloc4->Add (Vector(281.7525939971322,368.8290520691785, 0.0));
    positionAlloc4->Add (Vector(434.57800990182574,429.2825634018588, 0.0));
    positionAlloc4->Add (Vector(33.757921778996725,19.89023829162162, 0.0));
    positionAlloc4->Add (Vector(96.6068733577325,128.69090692009854, 0.0));
    positionAlloc4->Add (Vector(103.20639018652805,370.28696572807473, 0.0));
    positionAlloc4->Add (Vector(104.75906852488743,338.5366597690445, 0.0));
    positionAlloc4->Add (Vector(165.12194547597065,102.72439434977848, 0.0));
    positionAlloc4->Add (Vector(327.5239305144946,22.055771213940034, 0.0));
    positionAlloc4->Add (Vector(314.55163806110255,197.16008643871885, 0.0));
    positionAlloc4->Add (Vector(34.2768455445136,376.8079815415403, 0.0));
    positionAlloc4->Add (Vector(1.0518186965406962,152.94984668700434, 0.0));
    positionAlloc4->Add (Vector(388.2130122735472,462.43154359913126, 0.0));
    positionAlloc4->Add (Vector(128.69761005551638,47.94196722035898, 0.0));
    positionAlloc4->Add (Vector(443.1868682461903,470.1520832894626, 0.0));
    positionAlloc4->Add (Vector(65.77919610598542,172.60586013044744, 0.0));
    positionAlloc4->Add (Vector(445.684989329388,86.95893382334651, 0.0));
    positionAlloc4->Add (Vector(88.08986403490016,79.11615185001075, 0.0));
    positionAlloc4->Add (Vector(291.86225375347334,466.6819458789903, 0.0));
    positionAlloc4->Add (Vector(285.6683342722806,386.20291023132444, 0.0));
    positionAlloc4->Add (Vector(245.62992507673255,369.6980593008799, 0.0));
    positionAlloc4->Add (Vector(466.32545757235965,334.61679688733454, 0.0));
    positionAlloc4->Add (Vector(314.38770957010206,315.7705763174158, 0.0));
    positionAlloc4->Add (Vector(380.48547329952123,13.36377180844217, 0.0));
    positionAlloc4->Add (Vector(72.62620399522129,469.4574960664357, 0.0));
    positionAlloc4->Add (Vector(185.14381965122396,70.94150435995161, 0.0));
    positionAlloc4->Add (Vector(466.26091817059955,392.5320926182751, 0.0));
    positionAlloc4->Add (Vector(58.30554702630614,232.3500042079109, 0.0));
    positionAlloc4->Add (Vector(178.20801657779072,12.602540493876047, 0.0));
    positionAlloc4->Add (Vector(239.53725041796787,235.35758806870248, 0.0));
    positionAlloc4->Add (Vector(67.57440593964542,226.81839345051085, 0.0));
    positionAlloc4->Add (Vector(192.20583473465825,300.96266977273035, 0.0));
    positionAlloc4->Add (Vector(115.55507051761427,423.516314137128, 0.0));
    positionAlloc4->Add (Vector(111.68086720271819,169.8007351329494, 0.0));
    positionAlloc4->Add (Vector(427.43040800236895,322.0601130818427, 0.0));
    positionAlloc4->Add (Vector(195.69059665635692,407.79631683293053, 0.0));
    positionAlloc4->Add (Vector(62.13081886509497,136.6201567590143, 0.0));
    positionAlloc4->Add (Vector(88.0781984857792,345.40680778905045, 0.0));
    positionAlloc4->Add (Vector(386.0695266834573,236.71261971867986, 0.0));
    positionAlloc4->Add (Vector(179.5785167210351,348.4616023690419, 0.0));
    positionAlloc4->Add (Vector(121.83923326931234,106.03551549853158, 0.0));
    positionAlloc4->Add (Vector(193.13642257083114,347.1601418529147, 0.0));
    positionAlloc4->Add (Vector(441.4917691135344,8.684439109475584, 0.0));
    positionAlloc4->Add (Vector(342.204433551887,354.70024523257257, 0.0));
    positionAlloc4->Add (Vector(146.6074026098455,415.2355513200092, 0.0));
    positionAlloc4->Add (Vector(462.8061604268772,240.78751343451276, 0.0));
    positionAlloc4->Add (Vector(412.60697293774155,90.02822830362584, 0.0));
    positionAlloc4->Add (Vector(275.29793497705475,25.754131532947643, 0.0));
    positionAlloc4->Add (Vector(328.3756536624243,422.24358238447536, 0.0));
    positionAlloc4->Add (Vector(496.22360138611384,103.41645541798205, 0.0));
    positionAlloc4->Add (Vector(359.54554632656067,385.27886398433213, 0.0));
    positionAlloc4->Add (Vector(280.0782346411763,415.2004155525266, 0.0));
    positionAlloc4->Add (Vector(84.50975010846868,259.4350072722778, 0.0));
    positionAlloc4->Add (Vector(382.9187432055282,451.83062390955445, 0.0));
    positionAlloc4->Add (Vector(308.13408005180247,197.99727625642603, 0.0));
    positionAlloc4->Add (Vector(117.56548419421736,416.58684144546345, 0.0));
    positionAlloc4->Add (Vector(432.2552645245068,436.69446456536247, 0.0));
    positionAlloc4->Add (Vector(61.71629292321218,185.89003626298256, 0.0));
    positionAlloc4->Add (Vector(118.11577069188661,90.26751523287163, 0.0));
    positionAlloc4->Add (Vector(369.12770581115905,38.90739601135673, 0.0));
    positionAlloc4->Add (Vector(365.7330370681035,71.52152809802148, 0.0));
    positionAlloc4->Add (Vector(66.01880531000354,107.67597449610489, 0.0));
    positionAlloc4->Add (Vector(466.90902637146394,182.44613457134616, 0.0));
    positionAlloc4->Add (Vector(131.0051215664476,341.55140565000545, 0.0));
    positionAlloc4->Add (Vector(396.89722112028727,220.42875391015576, 0.0));
    positionAlloc4->Add (Vector(390.3064041392265,278.00754089568557, 0.0));
    positionAlloc4->Add (Vector(284.5100949003077,27.444219664270907, 0.0));
    positionAlloc4->Add (Vector(349.68569397156983,395.49429661924705, 0.0));
    positionAlloc4->Add (Vector(123.51622337360635,322.93546052509635, 0.0));
    positionAlloc4->Add (Vector(470.0828437651169,366.99576849280857, 0.0));
    positionAlloc4->Add (Vector(380.51737588084535,491.9308559906181, 0.0));
    positionAlloc4->Add (Vector(233.07639967365085,234.85545717504763, 0.0));
    positionAlloc4->Add (Vector(313.6731839556062,398.5271678007139, 0.0));
    positionAlloc4->Add (Vector(24.511075980989673,183.26969847181306, 0.0));
    positionAlloc4->Add (Vector(56.01558129337902,430.7001174562333, 0.0));
    positionAlloc4->Add (Vector(235.26765016503077,392.2214710705487, 0.0));
    positionAlloc4->Add (Vector(66.31014003591612,2.030096256992686, 0.0));
    positionAlloc4->Add (Vector(299.5955239330198,330.0343592979254, 0.0));
    positionAlloc4->Add (Vector(347.67279742313576,150.3825207962648, 0.0));
    positionAlloc4->Add (Vector(205.13191365477957,274.58861367331673, 0.0));
    positionAlloc4->Add (Vector(468.211023425504,212.64517515949782, 0.0));
    positionAlloc4->Add (Vector(440.7942727043615,101.04104766918127, 0.0));
    positionAlloc4->Add (Vector(494.65599788169106,37.507724470398465, 0.0));
    positionAlloc4->Add (Vector(166.14152214627575,3.330476931729054, 0.0));
    positionAlloc4->Add (Vector(9.952535583369116,398.3528431194461, 0.0));
    positionAlloc4->Add (Vector(490.13411284774367,437.04751648790364, 0.0));
    positionAlloc4->Add (Vector(492.0489545775923,358.15387203014325, 0.0));
    positionAlloc4->Add (Vector(176.78297155511336,58.466799705731546, 0.0));
    positionAlloc4->Add (Vector(340.68323804427934,106.98981326465962, 0.0));
    positionAlloc4->Add (Vector(301.57020452595543,316.3270985217844, 0.0));
    positionAlloc4->Add (Vector(337.0558931741275,94.68802955877842, 0.0));
    positionAlloc4->Add (Vector(384.1151089395275,263.33693644610963, 0.0));
    positionAlloc4->Add (Vector(15.281355792581886,464.87078223230276, 0.0));
    positionAlloc4->Add (Vector(97.2950427387131,196.14315083608503, 0.0));
    positionAlloc4->Add (Vector(73.34589184012408,295.21831137747574, 0.0));
    positionAlloc4->Add (Vector(42.83962718114387,295.92793175557216, 0.0));
    positionAlloc4->Add (Vector(84.42925673237178,364.2282911902599, 0.0));
    positionAlloc4->Add (Vector(108.51144224316506,88.44596335365613, 0.0));
    positionAlloc4->Add (Vector(338.94223924564795,305.2169816954856, 0.0));
    positionAlloc4->Add (Vector(430.763811289993,153.07686231714635, 0.0));
    positionAlloc4->Add (Vector(44.448628464403896,347.47724180079126, 0.0));
    positionAlloc4->Add (Vector(323.6157423903609,302.7229912726037, 0.0));
    positionAlloc4->Add (Vector(243.4669428068048,238.4215396542199, 0.0));
    positionAlloc4->Add (Vector(115.55937975852231,430.80306564751044, 0.0));
    positionAlloc4->Add (Vector(403.00905434514317,494.0429514555975, 0.0));
    positionAlloc4->Add (Vector(209.94351134384587,62.52894565643996, 0.0));
    positionAlloc4->Add (Vector(198.11189457079524,294.1109328014168, 0.0));
    positionAlloc4->Add (Vector(410.1448806569088,84.10358797965023, 0.0));
    positionAlloc4->Add (Vector(399.06292283121496,262.3185090554891, 0.0));
    positionAlloc4->Add (Vector(419.86246754183816,320.55514827406273, 0.0));
    positionAlloc4->Add (Vector(67.5302227949387,241.54091734990106, 0.0));
    positionAlloc4->Add (Vector(229.90170913601642,478.4210890880945, 0.0));
    positionAlloc4->Add (Vector(80.93573494068256,86.10005243974422, 0.0));
    positionAlloc4->Add (Vector(302.9181482156678,59.92409411856403, 0.0));
    positionAlloc4->Add (Vector(379.5445718964593,85.42714108802795, 0.0));
    positionAlloc4->Add (Vector(10.204624063044953,77.50468814963813, 0.0));
    positionAlloc4->Add (Vector(176.44697903998147,168.17749843463363, 0.0));
    positionAlloc4->Add (Vector(81.81772590109986,171.79528207578133, 0.0));
    positionAlloc4->Add (Vector(138.73580956140873,199.37823742176823, 0.0));
    positionAlloc4->Add (Vector(224.94012105525783,473.15284007000383, 0.0));
    positionAlloc4->Add (Vector(127.37234231905038,270.91749657739723, 0.0));
    positionAlloc4->Add (Vector(103.30937059840367,25.864069186313287, 0.0));
    positionAlloc4->Add (Vector(36.2185880560108,461.2840815187892, 0.0));
    positionAlloc4->Add (Vector(280.97675764346303,78.65578689267855, 0.0));
    positionAlloc4->Add (Vector(304.4334518812464,327.4837900320269, 0.0));
    positionAlloc4->Add (Vector(481.6921787417587,436.7174291888826, 0.0));
    positionAlloc4->Add (Vector(422.298722320558,108.18338194986322, 0.0));
    positionAlloc4->Add (Vector(9.096048934319755,237.71372384465445, 0.0));
    positionAlloc4->Add (Vector(346.5319869112231,228.22147056659963, 0.0));
    positionAlloc4->Add (Vector(416.23060604567394,86.85270795660466, 0.0));
    positionAlloc4->Add (Vector(311.25112807520577,313.3874377359787, 0.0));
    positionAlloc4->Add (Vector(221.19963179021178,231.42128187826904, 0.0));
    positionAlloc4->Add (Vector(0.931453606849153,422.4861731378254, 0.0));
    positionAlloc4->Add (Vector(456.2877726629319,396.83909735239774, 0.0));
    positionAlloc4->Add (Vector(488.05052281651723,238.33987826219334, 0.0));
    positionAlloc4->Add (Vector(108.08274685425945,423.64768186337847, 0.0));
    positionAlloc4->Add (Vector(55.332047395078966,153.8847573044539, 0.0));
    positionAlloc4->Add (Vector(308.3677459379163,341.9368735599471, 0.0));
    positionAlloc4->Add (Vector(129.27069286171795,166.5537065455236, 0.0));
    positionAlloc4->Add (Vector(32.070908799266164,300.7797434234099, 0.0));
    positionAlloc4->Add (Vector(289.52618515876566,357.9753909357804, 0.0));
    positionAlloc4->Add (Vector(477.09411542596615,320.2730338878298, 0.0));
    positionAlloc4->Add (Vector(162.7377234443208,212.12014401799846, 0.0));
    positionAlloc4->Add (Vector(209.50167413644417,19.588325879507153, 0.0));
    positionAlloc4->Add (Vector(56.50021662542215,421.7533464306024, 0.0));
    positionAlloc4->Add (Vector(180.18670928915824,256.73189846347697, 0.0));
    positionAlloc4->Add (Vector(355.7628075107111,160.6482791461608, 0.0));
    positionAlloc4->Add (Vector(231.08407418463744,475.6653119228573, 0.0));
    positionAlloc4->Add (Vector(238.42914381524966,156.03670706584677, 0.0));
    positionAlloc4->Add (Vector(365.1592480386104,21.592249924773544, 0.0));
    positionAlloc4->Add (Vector(26.685221916355808,85.86009338679817, 0.0));
    positionAlloc4->Add (Vector(303.6640648380774,56.152216486506035, 0.0));
    positionAlloc4->Add (Vector(352.05448434002403,327.27197115997507, 0.0));
    positionAlloc4->Add (Vector(309.60717781689164,285.0570050716159, 0.0));
    positionAlloc4->Add (Vector(293.50499708290454,329.76941641079145, 0.0));
    positionAlloc4->Add (Vector(89.86529149071326,495.12767463060186, 0.0));
    positionAlloc4->Add (Vector(48.61350597134933,334.3054138634757, 0.0));
    positionAlloc4->Add (Vector(139.8396622757082,176.00907235443736, 0.0));
    positionAlloc4->Add (Vector(371.98912319332584,458.57337137736613, 0.0));
    positionAlloc4->Add (Vector(178.89747134958094,144.03184131200203, 0.0));
    positionAlloc4->Add (Vector(262.3062613432014,262.5205563851383, 0.0));
    positionAlloc4->Add (Vector(149.1541887117091,197.11878989776116, 0.0));
    positionAlloc4->Add (Vector(466.0166173061797,157.79861560282592, 0.0));
    positionAlloc4->Add (Vector(187.59123120378757,302.96684258979604, 0.0));
    positionAlloc4->Add (Vector(366.7976170982156,72.64309070339198, 0.0));
    positionAlloc4->Add (Vector(451.91508610682206,176.9797388903991, 0.0));
    positionAlloc4->Add (Vector(436.7938395646234,338.5172974750971, 0.0));
    positionAlloc4->Add (Vector(317.4598344208796,380.64806266380845, 0.0));
    positionAlloc4->Add (Vector(135.1612813623282,402.18801803288136, 0.0));
    positionAlloc4->Add (Vector(266.37755501817605,22.914086068607453, 0.0));
    positionAlloc4->Add (Vector(271.39125708876156,240.38428144060993, 0.0));
    positionAlloc4->Add (Vector(151.94901358534497,428.70558411145043, 0.0));
    positionAlloc4->Add (Vector(87.97730516214658,478.19305143318627, 0.0));
    positionAlloc4->Add (Vector(61.08100352081674,452.248165230393, 0.0));
    positionAlloc4->Add (Vector(1.4371222260671224,285.0600188033169, 0.0));
    positionAlloc4->Add (Vector(357.7396015886045,284.04905121974053, 0.0));
    positionAlloc4->Add (Vector(96.45495713105822,111.03059774018648, 0.0));
    positionAlloc4->Add (Vector(426.11171091541297,136.24977278464397, 0.0));
    positionAlloc4->Add (Vector(125.78349630392388,217.83896525199614, 0.0));
    positionAlloc4->Add (Vector(119.0640680754792,375.2143983366259, 0.0));
    positionAlloc4->Add (Vector(474.30290735595577,292.40007977051465, 0.0));
    positionAlloc4->Add (Vector(296.177389527643,421.12345459450836, 0.0));
    positionAlloc4->Add (Vector(391.5439510571452,229.75732923890806, 0.0));
    positionAlloc4->Add (Vector(182.9495892377812,447.92461106825505, 0.0));
    positionAlloc4->Add (Vector(478.3668172429426,330.9270744895799, 0.0));
    positionAlloc4->Add (Vector(315.9424496968161,130.10590528807276, 0.0));
    positionAlloc4->Add (Vector(66.13960112848332,9.332774457471881, 0.0));
    positionAlloc4->Add (Vector(346.6292195268654,274.56028074639045, 0.0));
    positionAlloc4->Add (Vector(472.4488021213763,277.91595526074116, 0.0));
    positionAlloc4->Add (Vector(435.67609904118586,112.33443691577627, 0.0));
    positionAlloc4->Add (Vector(95.0866169980582,445.62164435393544, 0.0));
    positionAlloc4->Add (Vector(80.19516135158955,290.3853921446969, 0.0));
    positionAlloc4->Add (Vector(17.971443180857417,186.52482043781148, 0.0));
    positionAlloc4->Add (Vector(0.35728216522745715,206.17431946088794, 0.0));
    positionAlloc4->Add (Vector(136.47366383690579,206.31308531475807, 0.0));
    positionAlloc4->Add (Vector(406.84001928912306,149.75833623313224, 0.0));
    positionAlloc4->Add (Vector(222.5995815614108,28.63025041179945, 0.0));
    positionAlloc4->Add (Vector(449.20584064560063,209.69194899722544, 0.0));
    positionAlloc4->Add (Vector(52.40454338331585,372.21543970333636, 0.0));
    positionAlloc4->Add (Vector(293.9311465523933,426.62332356085926, 0.0));
    positionAlloc4->Add (Vector(12.958469149655095,368.09453764695814, 0.0));
    positionAlloc4->Add (Vector(287.7486599467862,54.22245494461564, 0.0));
    positionAlloc4->Add (Vector(405.3094763903656,1.8890149390257682, 0.0));
    positionAlloc4->Add (Vector(157.11214933392964,349.0043241952128, 0.0));
    positionAlloc4->Add (Vector(278.3117173812483,321.0474763710834, 0.0));
    positionAlloc4->Add (Vector(289.9178516647404,56.50956704937432, 0.0));
    positionAlloc4->Add (Vector(261.85176963868656,314.34180060961063, 0.0));
    positionAlloc4->Add (Vector(338.55185023285696,225.14723691952997, 0.0));
    positionAlloc4->Add (Vector(50.62752576067642,196.6861546250444, 0.0));
    positionAlloc4->Add (Vector(96.73009713087633,261.70262962260495, 0.0));
    positionAlloc4->Add (Vector(247.07607548842887,135.1602673437801, 0.0));
    positionAlloc4->Add (Vector(246.9236519702486,181.94299891574045, 0.0));
    positionAlloc4->Add (Vector(396.8537831601586,282.59398920601546, 0.0));
    positionAlloc4->Add (Vector(355.15521031749006,27.632579468606423, 0.0));
    positionAlloc4->Add (Vector(3.906491649520061,62.39877859283322, 0.0));
    positionAlloc4->Add (Vector(185.36933985516902,466.0921423594178, 0.0));
    positionAlloc4->Add (Vector(82.37673985519406,439.89742019640823, 0.0));
    positionAlloc4->Add (Vector(322.33322421104526,263.2244176203185, 0.0));
    positionAlloc4->Add (Vector(262.66679510306244,223.80917551961798, 0.0));
    positionAlloc4->Add (Vector(75.84636112857768,219.8868989155472, 0.0));
    positionAlloc4->Add (Vector(79.35280211047535,336.04986859875635, 0.0));
    positionAlloc4->Add (Vector(398.4068550409225,263.09615736049255, 0.0));
    positionAlloc4->Add (Vector(3.5571476967460325,304.63097479393775, 0.0));
    positionAlloc4->Add (Vector(352.6551129608219,315.32712405636005, 0.0));
    positionAlloc4->Add (Vector(41.33851695998264,435.52110003879386, 0.0));
    positionAlloc4->Add (Vector(458.51744937058186,103.31741498779817, 0.0));
    positionAlloc4->Add (Vector(99.56552010618103,284.4474983581191, 0.0));
    positionAlloc4->Add (Vector(121.95916533090761,88.27700616559531, 0.0));
    positionAlloc4->Add (Vector(303.9808160744844,24.689225281175286, 0.0));
    positionAlloc4->Add (Vector(240.49428925440475,344.1890672711699, 0.0));
    positionAlloc4->Add (Vector(316.43795779539477,85.25277001055481, 0.0));
    positionAlloc4->Add (Vector(238.01081206870018,193.82249483220275, 0.0));
    positionAlloc4->Add (Vector(79.64545303979436,345.5980251543561, 0.0));
    positionAlloc4->Add (Vector(372.8033868889507,159.01775264903733, 0.0));
    positionAlloc4->Add (Vector(389.18411814912,331.9078351349554, 0.0));
    positionAlloc4->Add (Vector(316.4298238435038,216.92641626408343, 0.0));
    positionAlloc4->Add (Vector(396.38040581419295,444.28284304913194, 0.0));
    positionAlloc4->Add (Vector(482.617650359266,313.61763904477533, 0.0));
    positionAlloc4->Add (Vector(124.04039734428291,11.422648135205883, 0.0));
    positionAlloc4->Add (Vector(344.4068368486628,451.4869704102734, 0.0));
    positionAlloc4->Add (Vector(93.45326658837233,234.03070950205878, 0.0));
    positionAlloc4->Add (Vector(314.09711070485656,86.77123549846006, 0.0));
    positionAlloc4->Add (Vector(322.119618923579,385.7303197818, 0.0));
    positionAlloc4->Add (Vector(109.9123287907669,424.6445901248324, 0.0));
    positionAlloc4->Add (Vector(12.019482441778306,430.0166748955108, 0.0));
    positionAlloc4->Add (Vector(465.62048402432646,56.30423421100383, 0.0));
    positionAlloc4->Add (Vector(24.3050268485695,92.48209240597971, 0.0));
    positionAlloc4->Add (Vector(304.08718810392975,314.11017543199597, 0.0));
    positionAlloc4->Add (Vector(133.75891107288555,55.462415566688406, 0.0));
    positionAlloc4->Add (Vector(4.146858184807922,384.32215704331577, 0.0));
    positionAlloc4->Add (Vector(196.57102553166018,45.83809941056005, 0.0));
    positionAlloc4->Add (Vector(99.53657413569827,463.83962459746834, 0.0));
    positionAlloc4->Add (Vector(420.1512265221971,409.77061751305865, 0.0));
    positionAlloc4->Add (Vector(195.17224299505426,351.27277452160047, 0.0));
    positionAlloc4->Add (Vector(386.13822932685963,316.1205574449003, 0.0));
    positionAlloc4->Add (Vector(399.6919197312757,293.74910091888745, 0.0));
    positionAlloc4->Add (Vector(138.00616642850227,419.9876996650616, 0.0));
    positionAlloc4->Add (Vector(250.40829163448623,415.09764759490093, 0.0));
    positionAlloc4->Add (Vector(44.23757456133664,410.5114734071566, 0.0));
    positionAlloc4->Add (Vector(351.6260547362064,448.24157632706607, 0.0));
    positionAlloc4->Add (Vector(322.7544108583503,72.75318400186958, 0.0));
    positionAlloc4->Add (Vector(244.7624699888229,156.22724708183893, 0.0));
    positionAlloc4->Add (Vector(377.0807893751522,34.687765186710685, 0.0));
    positionAlloc4->Add (Vector(247.33105360864045,489.43240564507323, 0.0));
    positionAlloc4->Add (Vector(136.68163419516299,126.1912833259618, 0.0));
    positionAlloc4->Add (Vector(443.98421396384794,318.65006366658383, 0.0));
    positionAlloc4->Add (Vector(96.25766155604765,478.8832540604701, 0.0));
    positionAlloc4->Add (Vector(256.77000737490215,170.53350314796495, 0.0));
    positionAlloc4->Add (Vector(77.51381606621094,412.47516612674013, 0.0));
    positionAlloc4->Add (Vector(368.36923537349406,366.8861331279045, 0.0));
    positionAlloc4->Add (Vector(145.65747061408118,211.6666784590287, 0.0));
    positionAlloc4->Add (Vector(198.66391577735388,331.1232724174492, 0.0));
    positionAlloc4->Add (Vector(94.32011162478999,305.57198592887346, 0.0));
    positionAlloc4->Add (Vector(452.3084294593159,418.3685936658319, 0.0));
    positionAlloc4->Add (Vector(467.967523408515,422.0181487763219, 0.0));
    positionAlloc4->Add (Vector(414.2662869271458,456.7636972907145, 0.0));
    positionAlloc4->Add (Vector(494.95842062806474,98.35796731743545, 0.0));
    positionAlloc4->Add (Vector(452.68807161256814,35.87732326336196, 0.0));
    positionAlloc4->Add (Vector(367.43677268077874,140.95884275150982, 0.0));
    positionAlloc4->Add (Vector(152.3773621327161,329.1299502927844, 0.0));
    positionAlloc4->Add (Vector(317.6430826742138,69.77198737817942, 0.0));
    positionAlloc4->Add (Vector(185.3985045297616,468.63029704370007, 0.0));
    positionAlloc4->Add (Vector(404.476603225817,357.0849043177309, 0.0));
    positionAlloc4->Add (Vector(181.30780235862372,147.92380741444023, 0.0));
    positionAlloc4->Add (Vector(275.87380015072563,435.95690027662135, 0.0));
    positionAlloc4->Add (Vector(423.06578302976027,364.73553023649976, 0.0));
    positionAlloc4->Add (Vector(27.849717890625016,69.90103361199074, 0.0));
    positionAlloc4->Add (Vector(169.2888424509789,134.00738125349372, 0.0));
    positionAlloc4->Add (Vector(483.88293241093953,1.1311720799157543, 0.0));
    positionAlloc4->Add (Vector(480.1104917368298,488.75972762812927, 0.0));
    positionAlloc4->Add (Vector(451.6391673040646,329.2631268014074, 0.0));
    positionAlloc4->Add (Vector(311.27943852296954,415.98775198792816, 0.0));
    positionAlloc4->Add (Vector(182.21477442275236,25.052234577666976, 0.0));
    positionAlloc4->Add (Vector(8.808680861938589,118.72025707614142, 0.0));
    positionAlloc4->Add (Vector(292.87849803750385,458.5153248958807, 0.0));
    positionAlloc4->Add (Vector(456.67195629288665,495.2919394487803, 0.0));
    positionAlloc4->Add (Vector(136.76159011098827,72.16386722124324, 0.0));
    positionAlloc4->Add (Vector(331.2093756012713,371.40963941037467, 0.0));
    positionAlloc4->Add (Vector(357.76680872137547,272.2909260899483, 0.0));
    positionAlloc4->Add (Vector(379.74026668253947,398.54334569048314, 0.0));
    positionAlloc4->Add (Vector(448.67395827988133,444.96443567610066, 0.0));
    positionAlloc4->Add (Vector(494.03819145081945,269.7831689856695, 0.0));
    positionAlloc4->Add (Vector(127.31396683644735,497.16033347506544, 0.0));
    positionAlloc4->Add (Vector(479.3441020744533,452.71641165968634, 0.0));
    positionAlloc4->Add (Vector(129.82202275927074,458.1876942783188, 0.0));
    positionAlloc4->Add (Vector(204.19930099297096,351.81398281448054, 0.0));
    positionAlloc4->Add (Vector(55.36763487872387,143.61798064615832, 0.0));
    positionAlloc4->Add (Vector(234.01940281352086,114.18176800715557, 0.0));
    positionAlloc4->Add (Vector(35.62562866696356,32.94981105959677, 0.0));
    positionAlloc4->Add (Vector(177.70530233718173,76.84360942227292, 0.0));
    positionAlloc4->Add (Vector(274.7760201171088,445.23737035785916, 0.0));
    positionAlloc4->Add (Vector(274.6959194530414,81.42871725007178, 0.0));
    positionAlloc4->Add (Vector(465.0760932108483,394.24347076928257, 0.0));
    positionAlloc4->Add (Vector(271.4715441231272,385.70228127388214, 0.0));
    positionAlloc4->Add (Vector(373.2846395366683,401.1354049280139, 0.0));
    positionAlloc4->Add (Vector(229.54872582316983,452.24612718443274, 0.0));
    positionAlloc4->Add (Vector(34.18104270933492,395.6013817547703, 0.0));
    positionAlloc4->Add (Vector(389.46820761723797,201.47244684793225, 0.0));
    positionAlloc4->Add (Vector(46.21555657741605,172.46648824865989, 0.0));
    positionAlloc4->Add (Vector(362.20557615975696,43.11566608574302, 0.0));
    positionAlloc4->Add (Vector(329.3409129899558,407.84061980827266, 0.0));
    positionAlloc4->Add (Vector(418.01505312797605,102.99962232115494, 0.0));
    positionAlloc4->Add (Vector(124.65526295617285,271.62847689968083, 0.0));
    positionAlloc4->Add (Vector(106.33367984384151,398.0047668099311, 0.0));
    positionAlloc4->Add (Vector(368.7792123164667,429.54867775904455, 0.0));
    positionAlloc4->Add (Vector(372.38344429719234,395.52186666960193, 0.0));
    positionAlloc4->Add (Vector(140.05316860807832,245.75038917701136, 0.0));
    positionAlloc4->Add (Vector(269.8122833575078,311.4826264348993, 0.0));
    positionAlloc4->Add (Vector(241.91314585292318,59.785869872820086, 0.0));
    positionAlloc4->Add (Vector(258.5679863268009,220.39973299828318, 0.0));
    positionAlloc4->Add (Vector(384.1939829974843,291.83120464739375, 0.0));
    positionAlloc4->Add (Vector(458.1764656554599,210.94953259232062, 0.0));
    positionAlloc4->Add (Vector(91.64943831473843,310.35084105132484, 0.0));
    positionAlloc4->Add (Vector(462.78541544534426,158.52852325214894, 0.0));
    positionAlloc4->Add (Vector(75.04132077874698,257.3132605757383, 0.0));
    positionAlloc4->Add (Vector(162.01911829908244,263.53352441327684, 0.0));
    positionAlloc4->Add (Vector(225.8985934083515,56.52187986278601, 0.0));
    positionAlloc4->Add (Vector(45.33569846923019,466.5160915157316, 0.0));
    positionAlloc4->Add (Vector(254.98769187994225,271.9440839265739, 0.0));
    positionAlloc4->Add (Vector(78.395718426016,189.27932929491732, 0.0));
    positionAlloc4->Add (Vector(396.3114870688294,307.28999706265364, 0.0));
    positionAlloc4->Add (Vector(248.57411276684078,31.488795918364954, 0.0));
    positionAlloc4->Add (Vector(94.06757729833399,288.20496328794235, 0.0));
    positionAlloc4->Add (Vector(491.81997780264976,70.68620299794736, 0.0));
    positionAlloc4->Add (Vector(452.3303376866909,446.0273423479105, 0.0));
    positionAlloc4->Add (Vector(67.9490293530986,340.12236170659213, 0.0));
    positionAlloc4->Add (Vector(160.37368949249952,191.76030881751706, 0.0));
    positionAlloc4->Add (Vector(81.94349053230322,285.95555196356213, 0.0));
    positionAlloc4->Add (Vector(169.30166050577517,104.45772629076156, 0.0));
    positionAlloc4->Add (Vector(183.58106028272397,33.89600654499758, 0.0));
    positionAlloc4->Add (Vector(34.375429824256955,111.62730363451607, 0.0));
    positionAlloc4->Add (Vector(471.80797595069043,115.68242591467565, 0.0));
    positionAlloc4->Add (Vector(374.1531048302113,194.92544139512162, 0.0));
    positionAlloc4->Add (Vector(405.70958545317893,188.96139205848706, 0.0));
    positionAlloc4->Add (Vector(148.64975122135903,354.45277173590006, 0.0));
    positionAlloc4->Add (Vector(422.34703752497876,275.2550857948067, 0.0));
    positionAlloc4->Add (Vector(88.82619269766751,241.61301470310244, 0.0));
    positionAlloc4->Add (Vector(376.2377808854111,255.84514877006436, 0.0));
    positionAlloc4->Add (Vector(87.69574734254948,401.9380260493947, 0.0));
    positionAlloc4->Add (Vector(393.06667580220494,61.53651235180379, 0.0));
    positionAlloc4->Add (Vector(198.04717490059681,198.3598612933271, 0.0));
    positionAlloc4->Add (Vector(428.2334119996078,61.54448309758015, 0.0));
    positionAlloc4->Add (Vector(304.1923858818261,68.5979214633367, 0.0));
    positionAlloc4->Add (Vector(339.43302197249864,114.25852581146229, 0.0));
    positionAlloc4->Add (Vector(155.4708312951006,439.2553557682431, 0.0));
    positionAlloc4->Add (Vector(459.6346129792899,256.9319319205724, 0.0));
    positionAlloc4->Add (Vector(193.288358302585,403.4753692752579, 0.0));
    positionAlloc4->Add (Vector(466.9913900748102,24.12981868926195, 0.0));
    positionAlloc4->Add (Vector(213.0283595664365,267.3297568454914, 0.0));
    positionAlloc4->Add (Vector(447.57315766080194,426.5740499464067, 0.0));
    positionAlloc4->Add (Vector(326.4661472993443,365.31538284069563, 0.0));
    positionAlloc4->Add (Vector(403.71580444005133,160.41856437277653, 0.0));
    positionAlloc4->Add (Vector(371.06331898392546,127.54791164441559, 0.0));
    positionAlloc4->Add (Vector(200.47796038455334,256.13251006747254, 0.0));
    positionAlloc4->Add (Vector(279.1432343345441,239.85337858042072, 0.0));
    positionAlloc4->Add (Vector(13.236595752990743,209.94741732358253, 0.0));
    positionAlloc4->Add (Vector(301.0032671539913,448.2858380491032, 0.0));
    positionAlloc4->Add (Vector(483.4995855773852,306.5456291453734, 0.0));
    positionAlloc4->Add (Vector(94.43465316443262,101.53762172528069, 0.0));
    positionAlloc4->Add (Vector(236.06940532810933,461.85807382116155, 0.0));
    positionAlloc4->Add (Vector(202.5440660242909,13.421684224254605, 0.0));
    positionAlloc4->Add (Vector(349.57787258114763,99.24048134487457, 0.0));
    positionAlloc4->Add (Vector(129.15427537960477,286.8418374266338, 0.0));
    positionAlloc4->Add (Vector(119.04024593151708,252.1178645827401, 0.0));
    positionAlloc4->Add (Vector(322.19040871028574,243.79474212253865, 0.0));
    positionAlloc4->Add (Vector(165.7719982267246,217.94802235395238, 0.0));
    positionAlloc4->Add (Vector(385.10375296347195,1.0828075269568305, 0.0));
    positionAlloc4->Add (Vector(256.8056458416375,113.49275716520557, 0.0));
    positionAlloc4->Add (Vector(138.35511255233112,332.4511133284075, 0.0));
    positionAlloc4->Add (Vector(229.6653172879641,359.98958167735054, 0.0));
    positionAlloc4->Add (Vector(396.19475966010674,350.4937499195532, 0.0));
    positionAlloc4->Add (Vector(274.65877064741665,182.83510806318608, 0.0));
    positionAlloc4->Add (Vector(166.88114056966376,484.5732238713842, 0.0));
    positionAlloc4->Add (Vector(467.873253483027,70.08803182031848, 0.0));
    positionAlloc4->Add (Vector(172.47185206103882,39.09994985796933, 0.0));
    positionAlloc4->Add (Vector(487.4101125547808,318.7679216562576, 0.0));
    positionAlloc4->Add (Vector(247.53139565589365,462.64972459718706, 0.0));
    positionAlloc4->Add (Vector(176.62998757736804,121.1501583692789, 0.0));
    positionAlloc4->Add (Vector(94.55702963241907,442.52512105576085, 0.0));
    positionAlloc4->Add (Vector(115.1106078602489,58.56588205582952, 0.0));
    positionAlloc4->Add (Vector(19.295020076945224,36.162313646172116, 0.0));
    positionAlloc4->Add (Vector(277.33527262289465,12.993885871430798, 0.0));
    positionAlloc4->Add (Vector(452.1886586207086,424.1792582977629, 0.0));
    positionAlloc4->Add (Vector(125.17031905098786,233.71191842952928, 0.0));
    positionAlloc4->Add (Vector(466.5825566662118,375.2584448909454, 0.0));
    positionAlloc4->Add (Vector(405.3775406554391,329.8138602580315, 0.0));
    positionAlloc4->Add (Vector(90.80343620239162,450.1286557619111, 0.0));
    positionAlloc4->Add (Vector(422.87631955507163,346.0349435257176, 0.0));
    positionAlloc4->Add (Vector(400.4690957403479,63.2746364304394, 0.0));
    positionAlloc4->Add (Vector(278.2449406401065,242.73495402769117, 0.0));
    positionAlloc4->Add (Vector(124.73371518568376,282.9187801215669, 0.0));
    positionAlloc4->Add (Vector(97.97534776332517,298.7662563745786, 0.0));
    positionAlloc4->Add (Vector(417.1283256560463,395.6620765122844, 0.0));
    positionAlloc4->Add (Vector(466.4798285622062,10.553553735684895, 0.0));
    positionAlloc4->Add (Vector(209.6442883762755,418.14916651288263, 0.0));
    positionAlloc4->Add (Vector(186.39192005208727,223.25643994925386, 0.0));
    positionAlloc4->Add (Vector(361.5720382605061,108.94015777729355, 0.0));
    positionAlloc4->Add (Vector(49.1515651639301,353.16374634058246, 0.0));
    positionAlloc4->Add (Vector(253.23696928639816,79.38968201726986, 0.0));
    positionAlloc4->Add (Vector(430.0729607843929,291.22401004290697, 0.0));
    positionAlloc4->Add (Vector(387.2240858116978,129.07086705317784, 0.0));
    positionAlloc4->Add (Vector(459.90058596897035,260.6798943497926, 0.0));
    positionAlloc4->Add (Vector(343.5505102775932,472.9381106955235, 0.0));
    positionAlloc4->Add (Vector(113.29470545301967,230.15438230052942, 0.0));
    positionAlloc4->Add (Vector(430.216774627553,14.222041652708572, 0.0));
    positionAlloc4->Add (Vector(24.716077952923577,260.4461471362225, 0.0));
    positionAlloc4->Add (Vector(190.5221127238334,165.7665088786916, 0.0));
    positionAlloc4->Add (Vector(172.83651823021594,134.99098760416544, 0.0));
    positionAlloc4->Add (Vector(95.82829653744746,492.9510029745541, 0.0));
    positionAlloc4->Add (Vector(111.60142289000008,134.26651538198593, 0.0));
    positionAlloc4->Add (Vector(370.02433188507354,100.31856813273082, 0.0));
    positionAlloc4->Add (Vector(499.3413977447586,411.5404057623214, 0.0));
    positionAlloc4->Add (Vector(268.1064700555114,32.892009888911055, 0.0));
    positionAlloc4->Add (Vector(80.23062342865556,401.209694937691, 0.0));
    positionAlloc4->Add (Vector(235.00530731086837,372.8894628796285, 0.0));
    positionAlloc4->Add (Vector(440.9116640140001,324.16006766580495, 0.0));
    positionAlloc4->Add (Vector(90.4976923964047,229.88042453736512, 0.0));
    positionAlloc4->Add (Vector(209.2787741871167,176.24827581996678, 0.0));
    positionAlloc4->Add (Vector(409.4642066610185,196.8068212073811, 0.0));
    positionAlloc4->Add (Vector(425.97290071471866,198.35521264049427, 0.0));
    positionAlloc4->Add (Vector(297.34840349680115,200.8981549339252, 0.0));
    positionAlloc4->Add (Vector(384.2941661975965,302.29070879890577, 0.0));
    positionAlloc4->Add (Vector(216.20790253476423,218.5512441873767, 0.0));
    positionAlloc4->Add (Vector(213.70002685747764,387.92418789716976, 0.0));
    positionAlloc4->Add (Vector(355.2900766919792,107.24668372429313, 0.0));
    positionAlloc4->Add (Vector(453.6599704825087,483.3080643135013, 0.0));
    positionAlloc4->Add (Vector(281.33322671143316,254.6455081363264, 0.0));
    positionAlloc4->Add (Vector(390.47809897304506,102.42936054666774, 0.0));
    positionAlloc4->Add (Vector(285.7161475576119,228.5129718360761, 0.0));
    positionAlloc4->Add (Vector(229.14747287531821,438.0665472654625, 0.0));
    positionAlloc4->Add (Vector(84.13788793181826,421.2137191618439, 0.0));
    positionAlloc4->Add (Vector(426.2066473579537,94.48935865762198, 0.0));
    positionAlloc4->Add (Vector(132.8631839378224,414.2541504980782, 0.0));
    positionAlloc4->Add (Vector(335.77819075935855,0.9893155809574949, 0.0));
    positionAlloc4->Add (Vector(483.75271999520584,102.60334100963075, 0.0));
    positionAlloc4->Add (Vector(56.79504851364192,227.1382324354067, 0.0));
    positionAlloc4->Add (Vector(68.43364666828295,479.42947286913204, 0.0));
    positionAlloc4->Add (Vector(425.95005287524094,416.5648294644171, 0.0));
    positionAlloc4->Add (Vector(8.840201418887638,359.64207945168266, 0.0));
    positionAlloc4->Add (Vector(195.63568261622072,343.9236244970776, 0.0));
    positionAlloc4->Add (Vector(21.369844808229065,309.62190565336147, 0.0));
    positionAlloc4->Add (Vector(480.5732458970693,395.23611322164584, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_20.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_ECC_flowmon/switch_ECC_20.flowmon", false, false);
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