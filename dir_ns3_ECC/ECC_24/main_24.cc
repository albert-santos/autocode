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
	  uint16_t numberOfRrhs = 64;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 800;
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
    positionAlloc->Add (Vector (500.0,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,333.3333333333333, 0.0));
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
    positionAlloc->Add (Vector (333.3333333333333,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,166.66666666666666, 0.0));
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
    positionAlloc->Add (Vector (111.11111111111111,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,500.0, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (0.0,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (0.0,166.66666666666666, 0.0));
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
    positionAlloc4->Add (Vector(339.02078325984417,402.6300138556628, 0.0));
    positionAlloc4->Add (Vector(397.8197705853255,70.66822681318175, 0.0));
    positionAlloc4->Add (Vector(75.13162959026515,494.92014122575324, 0.0));
    positionAlloc4->Add (Vector(27.930319903172574,24.217294936788146, 0.0));
    positionAlloc4->Add (Vector(147.8880784384186,24.096004155896523, 0.0));
    positionAlloc4->Add (Vector(249.90057358018737,384.2602073115852, 0.0));
    positionAlloc4->Add (Vector(267.2924288354461,36.66196410268208, 0.0));
    positionAlloc4->Add (Vector(469.4001523982373,212.1407116941591, 0.0));
    positionAlloc4->Add (Vector(364.906297708045,355.40095361209427, 0.0));
    positionAlloc4->Add (Vector(415.1077862980681,354.26394114307834, 0.0));
    positionAlloc4->Add (Vector(52.97456773327208,34.68180713401991, 0.0));
    positionAlloc4->Add (Vector(90.09581290452068,299.76390431552284, 0.0));
    positionAlloc4->Add (Vector(418.5189552030034,445.7526813801033, 0.0));
    positionAlloc4->Add (Vector(420.24877637394223,45.568155433328094, 0.0));
    positionAlloc4->Add (Vector(51.729497844987705,481.1731770603639, 0.0));
    positionAlloc4->Add (Vector(455.4293682895087,22.10558327245138, 0.0));
    positionAlloc4->Add (Vector(408.57284513036177,151.51381606070908, 0.0));
    positionAlloc4->Add (Vector(94.26955099940237,227.43590512503425, 0.0));
    positionAlloc4->Add (Vector(484.660876591358,150.9947860856875, 0.0));
    positionAlloc4->Add (Vector(457.5314348058568,346.04883687516855, 0.0));
    positionAlloc4->Add (Vector(131.2164961425598,439.2699660549772, 0.0));
    positionAlloc4->Add (Vector(498.9340156871229,485.7371222384417, 0.0));
    positionAlloc4->Add (Vector(410.6734607689249,59.33880627398464, 0.0));
    positionAlloc4->Add (Vector(188.13651650941853,73.82462554097097, 0.0));
    positionAlloc4->Add (Vector(290.6467735294034,58.9336854166162, 0.0));
    positionAlloc4->Add (Vector(154.39291403152683,249.0448410934617, 0.0));
    positionAlloc4->Add (Vector(430.41602976782235,194.8709493375524, 0.0));
    positionAlloc4->Add (Vector(415.6864637342114,171.8225578428522, 0.0));
    positionAlloc4->Add (Vector(306.38793345101345,242.18262547472423, 0.0));
    positionAlloc4->Add (Vector(87.28095837943512,415.67006973975646, 0.0));
    positionAlloc4->Add (Vector(145.75443458993396,287.79380392072636, 0.0));
    positionAlloc4->Add (Vector(152.9928448150707,285.7362137177262, 0.0));
    positionAlloc4->Add (Vector(71.52790037562528,481.36401682687597, 0.0));
    positionAlloc4->Add (Vector(386.90359680093957,358.25013715491883, 0.0));
    positionAlloc4->Add (Vector(173.00099226377574,402.87832616691475, 0.0));
    positionAlloc4->Add (Vector(243.8341559833243,424.5934419134051, 0.0));
    positionAlloc4->Add (Vector(220.13875796067362,102.30205138069299, 0.0));
    positionAlloc4->Add (Vector(465.85731436444314,469.30726529365006, 0.0));
    positionAlloc4->Add (Vector(400.45083961019145,65.71100481606612, 0.0));
    positionAlloc4->Add (Vector(490.61693684064556,157.25889506222458, 0.0));
    positionAlloc4->Add (Vector(334.8621286635607,142.64371938246552, 0.0));
    positionAlloc4->Add (Vector(8.859303933388796,174.6123448257057, 0.0));
    positionAlloc4->Add (Vector(340.4110646334149,421.85960219237273, 0.0));
    positionAlloc4->Add (Vector(115.02238522585812,89.20210128795603, 0.0));
    positionAlloc4->Add (Vector(229.6661123617943,462.75282624644024, 0.0));
    positionAlloc4->Add (Vector(168.85887152890032,337.4852208628828, 0.0));
    positionAlloc4->Add (Vector(445.104417094976,93.55771097225701, 0.0));
    positionAlloc4->Add (Vector(117.6883170559136,178.53148448539235, 0.0));
    positionAlloc4->Add (Vector(114.64921916503823,135.80872711293745, 0.0));
    positionAlloc4->Add (Vector(350.55147453191324,438.7388198511254, 0.0));
    positionAlloc4->Add (Vector(262.5925419167107,478.5707461710288, 0.0));
    positionAlloc4->Add (Vector(209.35040185781546,253.98501621097225, 0.0));
    positionAlloc4->Add (Vector(214.52369672422495,479.45888730695697, 0.0));
    positionAlloc4->Add (Vector(159.17415592395434,131.73496211529235, 0.0));
    positionAlloc4->Add (Vector(357.97273636048385,491.0112984179607, 0.0));
    positionAlloc4->Add (Vector(412.1216005558707,317.3159211791185, 0.0));
    positionAlloc4->Add (Vector(186.10005675815805,73.13520599052032, 0.0));
    positionAlloc4->Add (Vector(106.19440457852986,447.26694354436364, 0.0));
    positionAlloc4->Add (Vector(229.91337043163884,436.52600526920764, 0.0));
    positionAlloc4->Add (Vector(152.50192284068376,329.6440286697375, 0.0));
    positionAlloc4->Add (Vector(88.93700414408184,122.85052542246872, 0.0));
    positionAlloc4->Add (Vector(337.3551137285449,267.98152296334393, 0.0));
    positionAlloc4->Add (Vector(87.3994174124822,296.4678269065873, 0.0));
    positionAlloc4->Add (Vector(178.60921820344623,496.66532209492334, 0.0));
    positionAlloc4->Add (Vector(169.14252671271385,364.50965597134723, 0.0));
    positionAlloc4->Add (Vector(442.42354797199704,354.6378174312542, 0.0));
    positionAlloc4->Add (Vector(72.19218472624777,91.98645706669245, 0.0));
    positionAlloc4->Add (Vector(87.34786327434063,244.41350942575824, 0.0));
    positionAlloc4->Add (Vector(112.16789010013245,29.39202834035537, 0.0));
    positionAlloc4->Add (Vector(412.59491516926056,154.4189705870081, 0.0));
    positionAlloc4->Add (Vector(32.98055214500889,173.90962019990823, 0.0));
    positionAlloc4->Add (Vector(188.4856854075323,405.3056190379362, 0.0));
    positionAlloc4->Add (Vector(227.7446283579449,438.8223040420556, 0.0));
    positionAlloc4->Add (Vector(183.77600371088909,302.4928888482652, 0.0));
    positionAlloc4->Add (Vector(105.66960297590778,324.18225186908614, 0.0));
    positionAlloc4->Add (Vector(49.14407050863628,229.86751614931345, 0.0));
    positionAlloc4->Add (Vector(293.5709570936939,441.67314440519556, 0.0));
    positionAlloc4->Add (Vector(284.5094773758023,355.2190364104122, 0.0));
    positionAlloc4->Add (Vector(287.0130490176725,208.7385251882236, 0.0));
    positionAlloc4->Add (Vector(445.69736781190284,444.64575875127343, 0.0));
    positionAlloc4->Add (Vector(52.62042338742734,481.1131065482727, 0.0));
    positionAlloc4->Add (Vector(99.38885511867652,83.43603683939659, 0.0));
    positionAlloc4->Add (Vector(491.5476710055901,236.9601989342494, 0.0));
    positionAlloc4->Add (Vector(322.3227937506287,433.72393461414185, 0.0));
    positionAlloc4->Add (Vector(282.0904835873275,274.5081239413365, 0.0));
    positionAlloc4->Add (Vector(266.3985277973601,43.08207839605077, 0.0));
    positionAlloc4->Add (Vector(89.35880461105123,169.9992787526239, 0.0));
    positionAlloc4->Add (Vector(35.565426494726104,134.70751495792038, 0.0));
    positionAlloc4->Add (Vector(129.08306963518268,447.41128840612504, 0.0));
    positionAlloc4->Add (Vector(199.48178797095628,395.9990924934168, 0.0));
    positionAlloc4->Add (Vector(301.88418423007914,227.84772025591954, 0.0));
    positionAlloc4->Add (Vector(4.4621362784666445,129.76040152705093, 0.0));
    positionAlloc4->Add (Vector(223.5585702891273,416.6976779143996, 0.0));
    positionAlloc4->Add (Vector(448.1207414361195,141.83337348994147, 0.0));
    positionAlloc4->Add (Vector(271.4733802442981,231.68169359771613, 0.0));
    positionAlloc4->Add (Vector(27.981126615640118,353.50607460238956, 0.0));
    positionAlloc4->Add (Vector(382.6569650353206,369.96491876247455, 0.0));
    positionAlloc4->Add (Vector(98.87788037855222,375.7445464189745, 0.0));
    positionAlloc4->Add (Vector(480.81845754185167,85.50930351126829, 0.0));
    positionAlloc4->Add (Vector(67.55316413835372,59.2377009396049, 0.0));
    positionAlloc4->Add (Vector(130.13027730781747,233.73854107760678, 0.0));
    positionAlloc4->Add (Vector(162.4482756708075,87.56878967456616, 0.0));
    positionAlloc4->Add (Vector(409.5928283880605,59.02391905878823, 0.0));
    positionAlloc4->Add (Vector(3.6928527716433845,156.6174076200604, 0.0));
    positionAlloc4->Add (Vector(166.57137322451626,126.62922898842183, 0.0));
    positionAlloc4->Add (Vector(142.13403817177254,154.6543037331522, 0.0));
    positionAlloc4->Add (Vector(323.7263439540817,186.0932017131871, 0.0));
    positionAlloc4->Add (Vector(236.06970290749308,19.457717362803717, 0.0));
    positionAlloc4->Add (Vector(162.6659086476369,466.80374726731975, 0.0));
    positionAlloc4->Add (Vector(101.14808997028402,179.1603651150022, 0.0));
    positionAlloc4->Add (Vector(353.3249076109889,183.29548415967702, 0.0));
    positionAlloc4->Add (Vector(196.4133985980981,178.9560762261472, 0.0));
    positionAlloc4->Add (Vector(149.8098346226917,114.79482448319511, 0.0));
    positionAlloc4->Add (Vector(284.86957743001636,437.30459927762126, 0.0));
    positionAlloc4->Add (Vector(230.3568808452893,423.182052186039, 0.0));
    positionAlloc4->Add (Vector(187.37034696561545,436.94956878212724, 0.0));
    positionAlloc4->Add (Vector(94.70603000515948,400.7218436015567, 0.0));
    positionAlloc4->Add (Vector(38.566257713001704,69.20060899458097, 0.0));
    positionAlloc4->Add (Vector(177.83872500396984,94.71522555366185, 0.0));
    positionAlloc4->Add (Vector(19.100264421319068,370.8026411045753, 0.0));
    positionAlloc4->Add (Vector(117.77620082369977,225.98536934450286, 0.0));
    positionAlloc4->Add (Vector(349.0488935239382,398.6789895236503, 0.0));
    positionAlloc4->Add (Vector(384.42218427609345,244.607208752949, 0.0));
    positionAlloc4->Add (Vector(216.67628177858163,445.66416651045745, 0.0));
    positionAlloc4->Add (Vector(1.0000367194756277,284.2148185482518, 0.0));
    positionAlloc4->Add (Vector(445.0454607809719,156.1841861842579, 0.0));
    positionAlloc4->Add (Vector(258.4317200190759,30.94123394256193, 0.0));
    positionAlloc4->Add (Vector(160.28341699637073,131.79145032937683, 0.0));
    positionAlloc4->Add (Vector(240.8156307717165,314.1785101032913, 0.0));
    positionAlloc4->Add (Vector(41.96710496208439,327.67300693166356, 0.0));
    positionAlloc4->Add (Vector(220.94776520491232,327.6172651148866, 0.0));
    positionAlloc4->Add (Vector(216.78647446731702,138.66351786498916, 0.0));
    positionAlloc4->Add (Vector(396.1273372995718,241.6881329591576, 0.0));
    positionAlloc4->Add (Vector(260.31658924187195,344.0656625815066, 0.0));
    positionAlloc4->Add (Vector(495.05882586320746,437.1267904281089, 0.0));
    positionAlloc4->Add (Vector(46.27059353662211,170.57614766963698, 0.0));
    positionAlloc4->Add (Vector(222.07450383556082,116.75049386827891, 0.0));
    positionAlloc4->Add (Vector(29.350403056540408,353.9540293152027, 0.0));
    positionAlloc4->Add (Vector(184.23448667490132,477.7581421025459, 0.0));
    positionAlloc4->Add (Vector(302.5915613152973,380.85977560314575, 0.0));
    positionAlloc4->Add (Vector(235.52852526920793,312.79042255928545, 0.0));
    positionAlloc4->Add (Vector(18.768876193580862,326.8666117234581, 0.0));
    positionAlloc4->Add (Vector(99.23367088775842,380.4216096176041, 0.0));
    positionAlloc4->Add (Vector(146.83527440607307,477.49958841347035, 0.0));
    positionAlloc4->Add (Vector(68.96110923098792,461.70064008712063, 0.0));
    positionAlloc4->Add (Vector(151.15594484439865,403.70548756170217, 0.0));
    positionAlloc4->Add (Vector(388.9037569823966,218.19247742115456, 0.0));
    positionAlloc4->Add (Vector(443.2199337038374,304.4720064589884, 0.0));
    positionAlloc4->Add (Vector(348.45738859818647,490.01772469293417, 0.0));
    positionAlloc4->Add (Vector(323.33176776442457,160.35627403735498, 0.0));
    positionAlloc4->Add (Vector(448.2778232463288,19.19692449914434, 0.0));
    positionAlloc4->Add (Vector(24.613672647038342,3.041942994002622, 0.0));
    positionAlloc4->Add (Vector(340.9297634359695,11.311802874958865, 0.0));
    positionAlloc4->Add (Vector(130.90142006255567,225.88940679760495, 0.0));
    positionAlloc4->Add (Vector(339.28398870385195,351.9776483880385, 0.0));
    positionAlloc4->Add (Vector(487.72143694503546,374.6259267270067, 0.0));
    positionAlloc4->Add (Vector(341.8500401721895,357.0950234420448, 0.0));
    positionAlloc4->Add (Vector(50.01755989715212,139.85944633913527, 0.0));
    positionAlloc4->Add (Vector(305.24829291321885,465.7046431091003, 0.0));
    positionAlloc4->Add (Vector(242.91382888392639,150.47481081190406, 0.0));
    positionAlloc4->Add (Vector(183.799310381501,380.07983224832736, 0.0));
    positionAlloc4->Add (Vector(278.38855317766644,2.931055582071629, 0.0));
    positionAlloc4->Add (Vector(450.3608863035077,172.14301291108492, 0.0));
    positionAlloc4->Add (Vector(388.69359866357183,218.0997304648905, 0.0));
    positionAlloc4->Add (Vector(352.2044713339107,404.4678002262838, 0.0));
    positionAlloc4->Add (Vector(254.91129902283416,172.61950806680915, 0.0));
    positionAlloc4->Add (Vector(494.51192988877676,335.74515090922495, 0.0));
    positionAlloc4->Add (Vector(221.8401414155563,394.3801950655485, 0.0));
    positionAlloc4->Add (Vector(195.80294487266582,34.53514284205617, 0.0));
    positionAlloc4->Add (Vector(358.65199461487884,423.7995332557302, 0.0));
    positionAlloc4->Add (Vector(298.50304694010146,31.73990011236438, 0.0));
    positionAlloc4->Add (Vector(325.0337081534855,151.53134826486885, 0.0));
    positionAlloc4->Add (Vector(212.58973686453785,98.15990981735523, 0.0));
    positionAlloc4->Add (Vector(242.3297490911886,42.92676573537718, 0.0));
    positionAlloc4->Add (Vector(367.77893645167757,376.6669658726396, 0.0));
    positionAlloc4->Add (Vector(408.2265751934434,138.2658177106374, 0.0));
    positionAlloc4->Add (Vector(90.94612469454671,84.2214782840412, 0.0));
    positionAlloc4->Add (Vector(461.8676293408317,442.7952108569349, 0.0));
    positionAlloc4->Add (Vector(53.66608987990429,240.76836721874727, 0.0));
    positionAlloc4->Add (Vector(471.53562006464387,436.56810829352963, 0.0));
    positionAlloc4->Add (Vector(459.553595708848,83.02496662077252, 0.0));
    positionAlloc4->Add (Vector(316.3635163891963,147.2610890184197, 0.0));
    positionAlloc4->Add (Vector(454.36490725282147,253.37582153821648, 0.0));
    positionAlloc4->Add (Vector(32.84163396774037,427.7463001722895, 0.0));
    positionAlloc4->Add (Vector(187.33892105220428,173.525607222127, 0.0));
    positionAlloc4->Add (Vector(255.25490013598957,464.64549233013395, 0.0));
    positionAlloc4->Add (Vector(53.831996951380525,258.7078086569844, 0.0));
    positionAlloc4->Add (Vector(495.34078108034566,417.64118091222986, 0.0));
    positionAlloc4->Add (Vector(271.68546808724335,330.92006147853454, 0.0));
    positionAlloc4->Add (Vector(80.58435814387799,477.32448766506155, 0.0));
    positionAlloc4->Add (Vector(172.35234349540906,410.6378431334922, 0.0));
    positionAlloc4->Add (Vector(411.7094576118932,119.88844574072455, 0.0));
    positionAlloc4->Add (Vector(280.7851458314399,12.669193974013893, 0.0));
    positionAlloc4->Add (Vector(65.53379453379593,476.3444175559889, 0.0));
    positionAlloc4->Add (Vector(93.19608423507914,492.1004253387059, 0.0));
    positionAlloc4->Add (Vector(147.71314775366483,191.28343341434967, 0.0));
    positionAlloc4->Add (Vector(105.95200001670469,390.8921187442794, 0.0));
    positionAlloc4->Add (Vector(249.6291227932193,195.4769359579417, 0.0));
    positionAlloc4->Add (Vector(346.2353368463611,412.12276968996366, 0.0));
    positionAlloc4->Add (Vector(154.4559837665509,210.16055151544595, 0.0));
    positionAlloc4->Add (Vector(482.35712081426203,287.76512198279033, 0.0));
    positionAlloc4->Add (Vector(114.27639838389948,70.7641370652814, 0.0));
    positionAlloc4->Add (Vector(304.7642507511992,0.6559639373550086, 0.0));
    positionAlloc4->Add (Vector(38.998251239731715,432.7962171010749, 0.0));
    positionAlloc4->Add (Vector(177.84046218023846,164.68166531158718, 0.0));
    positionAlloc4->Add (Vector(144.00001713761844,115.99338593554626, 0.0));
    positionAlloc4->Add (Vector(222.656300808979,115.55284223093571, 0.0));
    positionAlloc4->Add (Vector(471.13820323407543,105.62696611221573, 0.0));
    positionAlloc4->Add (Vector(497.7972697997315,282.5021125449, 0.0));
    positionAlloc4->Add (Vector(63.36191644542988,422.44373452723676, 0.0));
    positionAlloc4->Add (Vector(351.1988682602385,355.06617333934634, 0.0));
    positionAlloc4->Add (Vector(472.4551542685557,482.4694337404467, 0.0));
    positionAlloc4->Add (Vector(106.15415639127885,120.66724442581639, 0.0));
    positionAlloc4->Add (Vector(212.1489132645852,1.372951508342657, 0.0));
    positionAlloc4->Add (Vector(496.2839126396132,320.6668874054505, 0.0));
    positionAlloc4->Add (Vector(295.3153837252471,218.98122370737954, 0.0));
    positionAlloc4->Add (Vector(122.3064772715347,37.16462788882507, 0.0));
    positionAlloc4->Add (Vector(152.18606160471975,326.1332195314378, 0.0));
    positionAlloc4->Add (Vector(80.71572387573484,71.61868998317011, 0.0));
    positionAlloc4->Add (Vector(165.08291839615137,233.22409834537007, 0.0));
    positionAlloc4->Add (Vector(226.41051132698337,328.2964839863523, 0.0));
    positionAlloc4->Add (Vector(260.7654518664154,60.258327733542615, 0.0));
    positionAlloc4->Add (Vector(263.79066809152124,51.45427389564633, 0.0));
    positionAlloc4->Add (Vector(417.9231775765902,302.48862974740143, 0.0));
    positionAlloc4->Add (Vector(45.08659556409111,465.0724194202418, 0.0));
    positionAlloc4->Add (Vector(144.56958206358956,178.460935175642, 0.0));
    positionAlloc4->Add (Vector(287.91440374477463,314.74583505059536, 0.0));
    positionAlloc4->Add (Vector(18.248445221940525,360.4760069594359, 0.0));
    positionAlloc4->Add (Vector(445.98346144218044,257.3656611821377, 0.0));
    positionAlloc4->Add (Vector(456.21535024361674,189.41990352220657, 0.0));
    positionAlloc4->Add (Vector(90.01098541452251,292.20279788137185, 0.0));
    positionAlloc4->Add (Vector(312.4189094897111,108.30456137880957, 0.0));
    positionAlloc4->Add (Vector(426.49598891584236,460.05476182824066, 0.0));
    positionAlloc4->Add (Vector(388.46641674123816,406.3319608080806, 0.0));
    positionAlloc4->Add (Vector(125.8671432865734,11.443552287377145, 0.0));
    positionAlloc4->Add (Vector(130.36086381932387,497.2517453944479, 0.0));
    positionAlloc4->Add (Vector(132.448240616919,370.46130333861424, 0.0));
    positionAlloc4->Add (Vector(382.8083667129603,19.81025707974554, 0.0));
    positionAlloc4->Add (Vector(165.22899578732188,317.5499159815925, 0.0));
    positionAlloc4->Add (Vector(211.02746031730402,172.97037332993025, 0.0));
    positionAlloc4->Add (Vector(228.63349372569357,412.416328476635, 0.0));
    positionAlloc4->Add (Vector(366.7646650502288,208.39666378040917, 0.0));
    positionAlloc4->Add (Vector(479.07980260672986,68.09049453636761, 0.0));
    positionAlloc4->Add (Vector(217.74211385938574,407.3007135251655, 0.0));
    positionAlloc4->Add (Vector(366.5561167843666,346.7523507067387, 0.0));
    positionAlloc4->Add (Vector(241.9856560679081,150.26691700821902, 0.0));
    positionAlloc4->Add (Vector(181.1402468767906,301.0627132599613, 0.0));
    positionAlloc4->Add (Vector(410.49390781973386,260.8114578413971, 0.0));
    positionAlloc4->Add (Vector(136.76512366825904,128.29169877618784, 0.0));
    positionAlloc4->Add (Vector(124.28639922324703,455.9713564544135, 0.0));
    positionAlloc4->Add (Vector(162.8211318359481,397.3535026172773, 0.0));
    positionAlloc4->Add (Vector(71.90633180995287,57.9664004553187, 0.0));
    positionAlloc4->Add (Vector(398.38104530513056,221.97434968539466, 0.0));
    positionAlloc4->Add (Vector(65.61304839944421,195.2006917976944, 0.0));
    positionAlloc4->Add (Vector(71.79861212494532,53.48268189506633, 0.0));
    positionAlloc4->Add (Vector(251.1751588039811,17.989642924664484, 0.0));
    positionAlloc4->Add (Vector(104.83484509809699,281.34975740359044, 0.0));
    positionAlloc4->Add (Vector(492.53732350128416,379.67331002833606, 0.0));
    positionAlloc4->Add (Vector(48.577827514335596,343.3483191051617, 0.0));
    positionAlloc4->Add (Vector(145.14129131230808,366.105607475713, 0.0));
    positionAlloc4->Add (Vector(259.2663328369586,68.3323629059977, 0.0));
    positionAlloc4->Add (Vector(477.51744878846483,37.95460775664877, 0.0));
    positionAlloc4->Add (Vector(317.3786331422306,246.71286932367948, 0.0));
    positionAlloc4->Add (Vector(388.5278783715927,195.63547670884722, 0.0));
    positionAlloc4->Add (Vector(333.0098613136478,277.27773933566385, 0.0));
    positionAlloc4->Add (Vector(187.46090354847823,320.1522284078747, 0.0));
    positionAlloc4->Add (Vector(376.03445186355054,446.5125662045195, 0.0));
    positionAlloc4->Add (Vector(94.11276146861225,59.42078483878821, 0.0));
    positionAlloc4->Add (Vector(202.0223594839697,454.01224379493266, 0.0));
    positionAlloc4->Add (Vector(362.32322886952505,352.37612319568666, 0.0));
    positionAlloc4->Add (Vector(251.24081714495833,51.71211851280211, 0.0));
    positionAlloc4->Add (Vector(378.65265683849185,137.12707485801107, 0.0));
    positionAlloc4->Add (Vector(263.35842571174294,241.85922451730502, 0.0));
    positionAlloc4->Add (Vector(95.94976716404075,195.8306119136518, 0.0));
    positionAlloc4->Add (Vector(157.9195376589776,160.0776861035596, 0.0));
    positionAlloc4->Add (Vector(470.24136727830074,155.6026794542546, 0.0));
    positionAlloc4->Add (Vector(349.9718908774951,143.6665487891569, 0.0));
    positionAlloc4->Add (Vector(471.91587753514403,256.4910748655583, 0.0));
    positionAlloc4->Add (Vector(241.9298738522368,78.1967578771845, 0.0));
    positionAlloc4->Add (Vector(159.10440428421722,118.00949458194194, 0.0));
    positionAlloc4->Add (Vector(297.9041346198393,49.96073019066849, 0.0));
    positionAlloc4->Add (Vector(205.63796348576568,22.03627614622694, 0.0));
    positionAlloc4->Add (Vector(51.483137874447415,330.096709931242, 0.0));
    positionAlloc4->Add (Vector(484.52002594789366,264.1501425201273, 0.0));
    positionAlloc4->Add (Vector(409.5404046080492,423.47818401854886, 0.0));
    positionAlloc4->Add (Vector(443.54518882674483,193.61054690666825, 0.0));
    positionAlloc4->Add (Vector(485.7042170756967,120.61298940509413, 0.0));
    positionAlloc4->Add (Vector(247.37502035703878,232.01079969604515, 0.0));
    positionAlloc4->Add (Vector(396.6511368589583,12.714464676280679, 0.0));
    positionAlloc4->Add (Vector(362.0866287045519,465.256141867636, 0.0));
    positionAlloc4->Add (Vector(263.17296762206877,101.6241519863576, 0.0));
    positionAlloc4->Add (Vector(339.62424278200155,439.4297822886418, 0.0));
    positionAlloc4->Add (Vector(376.71814615476626,420.00718875465026, 0.0));
    positionAlloc4->Add (Vector(147.46275329177777,279.201120100784, 0.0));
    positionAlloc4->Add (Vector(27.969397165201805,166.70685857916524, 0.0));
    positionAlloc4->Add (Vector(377.85803143563464,242.11439352611242, 0.0));
    positionAlloc4->Add (Vector(361.0784619902717,65.14776036356734, 0.0));
    positionAlloc4->Add (Vector(383.4790023456385,100.57057166872634, 0.0));
    positionAlloc4->Add (Vector(225.9342938297945,315.5787135792569, 0.0));
    positionAlloc4->Add (Vector(14.06648875315597,388.3772272419771, 0.0));
    positionAlloc4->Add (Vector(100.93605080399226,314.9970752804671, 0.0));
    positionAlloc4->Add (Vector(48.747167822679124,120.53956999915916, 0.0));
    positionAlloc4->Add (Vector(267.63811952917723,416.91053993868536, 0.0));
    positionAlloc4->Add (Vector(351.68043735312534,34.78547896605566, 0.0));
    positionAlloc4->Add (Vector(70.82883878011053,65.21791229681112, 0.0));
    positionAlloc4->Add (Vector(130.62271083588521,30.217258164577267, 0.0));
    positionAlloc4->Add (Vector(435.7020605104312,482.50278866936424, 0.0));
    positionAlloc4->Add (Vector(291.3116086181491,378.70154365016765, 0.0));
    positionAlloc4->Add (Vector(456.546678476812,146.21995297821593, 0.0));
    positionAlloc4->Add (Vector(338.05707627006296,167.08243541839735, 0.0));
    positionAlloc4->Add (Vector(373.73137137072945,326.42475148539745, 0.0));
    positionAlloc4->Add (Vector(267.9739333891474,460.8745164229891, 0.0));
    positionAlloc4->Add (Vector(197.1403795860467,197.9370045084756, 0.0));
    positionAlloc4->Add (Vector(421.69548571894137,343.9188652025474, 0.0));
    positionAlloc4->Add (Vector(157.52992033345353,29.112565402626245, 0.0));
    positionAlloc4->Add (Vector(104.61275803573811,446.70681956492564, 0.0));
    positionAlloc4->Add (Vector(420.9465365893736,369.16109973369737, 0.0));
    positionAlloc4->Add (Vector(331.87385428287575,427.30069690893544, 0.0));
    positionAlloc4->Add (Vector(197.47466811789087,77.44583738806898, 0.0));
    positionAlloc4->Add (Vector(393.03097084274884,191.4751147539192, 0.0));
    positionAlloc4->Add (Vector(84.1662315846648,5.757759005796059, 0.0));
    positionAlloc4->Add (Vector(305.363143252114,428.73693389982304, 0.0));
    positionAlloc4->Add (Vector(143.63116426509526,49.9119130013716, 0.0));
    positionAlloc4->Add (Vector(154.40471841797228,367.92954089848826, 0.0));
    positionAlloc4->Add (Vector(388.0300527992362,11.488502996138905, 0.0));
    positionAlloc4->Add (Vector(421.55608168409293,94.10242953399411, 0.0));
    positionAlloc4->Add (Vector(343.3489279148837,104.11668998268465, 0.0));
    positionAlloc4->Add (Vector(457.99435093483135,114.05964038890531, 0.0));
    positionAlloc4->Add (Vector(349.72320438330144,135.27775191300856, 0.0));
    positionAlloc4->Add (Vector(199.5258916591397,444.33462079356167, 0.0));
    positionAlloc4->Add (Vector(132.86469037366956,328.83565434217803, 0.0));
    positionAlloc4->Add (Vector(425.75438542561227,481.9054232909253, 0.0));
    positionAlloc4->Add (Vector(271.5328722727124,79.06039855244995, 0.0));
    positionAlloc4->Add (Vector(169.09646968690322,74.69568825480333, 0.0));
    positionAlloc4->Add (Vector(89.18581883937881,314.25493272297564, 0.0));
    positionAlloc4->Add (Vector(391.64128303318125,5.057515864133332, 0.0));
    positionAlloc4->Add (Vector(495.3216597154283,190.30169746206593, 0.0));
    positionAlloc4->Add (Vector(100.82409553878857,327.1532616414828, 0.0));
    positionAlloc4->Add (Vector(194.30916000847043,252.47126893905065, 0.0));
    positionAlloc4->Add (Vector(155.53429583107976,178.05661732704786, 0.0));
    positionAlloc4->Add (Vector(320.55240728447,46.940830141361964, 0.0));
    positionAlloc4->Add (Vector(282.169447669049,451.00399954787866, 0.0));
    positionAlloc4->Add (Vector(173.17162981659828,62.06206825684396, 0.0));
    positionAlloc4->Add (Vector(129.12462240061,169.93894693057078, 0.0));
    positionAlloc4->Add (Vector(233.15135556797156,313.74239294033345, 0.0));
    positionAlloc4->Add (Vector(441.600888981024,281.12521979938094, 0.0));
    positionAlloc4->Add (Vector(476.74125971819274,203.74870084649427, 0.0));
    positionAlloc4->Add (Vector(137.58080610441849,200.8196128905413, 0.0));
    positionAlloc4->Add (Vector(419.5462241541564,464.26038209817455, 0.0));
    positionAlloc4->Add (Vector(171.43174958156183,235.44413541103114, 0.0));
    positionAlloc4->Add (Vector(346.79763520121264,64.19203866175977, 0.0));
    positionAlloc4->Add (Vector(69.16236641360912,414.4825692014691, 0.0));
    positionAlloc4->Add (Vector(111.75702584267272,467.8563436431987, 0.0));
    positionAlloc4->Add (Vector(67.24186845225066,92.7111798488906, 0.0));
    positionAlloc4->Add (Vector(137.19045540398895,307.9047001670669, 0.0));
    positionAlloc4->Add (Vector(469.6859405751079,255.7319660476412, 0.0));
    positionAlloc4->Add (Vector(307.2222897558189,235.58651875519055, 0.0));
    positionAlloc4->Add (Vector(198.19474402053189,263.73967306946116, 0.0));
    positionAlloc4->Add (Vector(120.47633941444053,112.71609609573562, 0.0));
    positionAlloc4->Add (Vector(358.7080856844218,342.1098301289753, 0.0));
    positionAlloc4->Add (Vector(321.05851090894197,371.0004963634681, 0.0));
    positionAlloc4->Add (Vector(266.5301402673753,141.31326018855745, 0.0));
    positionAlloc4->Add (Vector(117.03121366421621,75.70479013605296, 0.0));
    positionAlloc4->Add (Vector(331.55978335330127,166.38899591462953, 0.0));
    positionAlloc4->Add (Vector(207.23978819186206,229.17006317176143, 0.0));
    positionAlloc4->Add (Vector(13.120147304347018,151.93478931544962, 0.0));
    positionAlloc4->Add (Vector(437.9439814209082,26.812901715844497, 0.0));
    positionAlloc4->Add (Vector(116.58229797748199,278.37569204039386, 0.0));
    positionAlloc4->Add (Vector(256.63345474329924,362.5924057084978, 0.0));
    positionAlloc4->Add (Vector(115.58784279031681,192.76457400829332, 0.0));
    positionAlloc4->Add (Vector(208.76968372285754,203.61056936594656, 0.0));
    positionAlloc4->Add (Vector(75.81215301098653,248.52002534471535, 0.0));
    positionAlloc4->Add (Vector(227.50439812515904,394.4450952864095, 0.0));
    positionAlloc4->Add (Vector(1.3802113165907737,414.0013937427279, 0.0));
    positionAlloc4->Add (Vector(196.087599745653,379.72517837371214, 0.0));
    positionAlloc4->Add (Vector(241.61314512066284,472.1166188910645, 0.0));
    positionAlloc4->Add (Vector(442.14540173473597,266.9498632392574, 0.0));
    positionAlloc4->Add (Vector(162.20792022352487,494.60855747391764, 0.0));
    positionAlloc4->Add (Vector(260.2367708465392,392.3105253856097, 0.0));
    positionAlloc4->Add (Vector(164.15388308727924,230.18769715541825, 0.0));
    positionAlloc4->Add (Vector(96.57827648398404,6.87480211753988, 0.0));
    positionAlloc4->Add (Vector(304.39486678175206,17.00480792041581, 0.0));
    positionAlloc4->Add (Vector(297.9880969386398,295.6794433240297, 0.0));
    positionAlloc4->Add (Vector(93.56173518014882,232.16547444775136, 0.0));
    positionAlloc4->Add (Vector(125.05252551920964,181.56566577471233, 0.0));
    positionAlloc4->Add (Vector(216.09968915512695,370.57763248897487, 0.0));
    positionAlloc4->Add (Vector(209.22455747524853,294.0276857440098, 0.0));
    positionAlloc4->Add (Vector(326.19726719856396,85.31662839130244, 0.0));
    positionAlloc4->Add (Vector(367.8433517798157,350.22444355952325, 0.0));
    positionAlloc4->Add (Vector(274.09163366038194,344.20990293618405, 0.0));
    positionAlloc4->Add (Vector(241.48867445181997,411.8626012617078, 0.0));
    positionAlloc4->Add (Vector(88.81976634176641,62.54017400854267, 0.0));
    positionAlloc4->Add (Vector(157.47869194009223,154.52222662294096, 0.0));
    positionAlloc4->Add (Vector(65.23372650932569,385.2461708777883, 0.0));
    positionAlloc4->Add (Vector(192.9772226362188,379.82878458268027, 0.0));
    positionAlloc4->Add (Vector(272.71273553172114,482.9888586031948, 0.0));
    positionAlloc4->Add (Vector(176.24596280540615,260.0675116713345, 0.0));
    positionAlloc4->Add (Vector(170.97929888512664,365.3483215711605, 0.0));
    positionAlloc4->Add (Vector(41.64400570257387,266.04357733902356, 0.0));
    positionAlloc4->Add (Vector(169.37381651046752,257.4632889331643, 0.0));
    positionAlloc4->Add (Vector(252.14941622720437,402.0467531699046, 0.0));
    positionAlloc4->Add (Vector(342.7428383951442,127.9498655130929, 0.0));
    positionAlloc4->Add (Vector(120.75227465240368,323.77163898256373, 0.0));
    positionAlloc4->Add (Vector(188.62422440157732,347.79957675569176, 0.0));
    positionAlloc4->Add (Vector(12.617936583810607,431.09592536168543, 0.0));
    positionAlloc4->Add (Vector(436.87647181862883,59.17692255669782, 0.0));
    positionAlloc4->Add (Vector(245.20033789943167,136.91076819296495, 0.0));
    positionAlloc4->Add (Vector(296.4331449832186,248.76837175861777, 0.0));
    positionAlloc4->Add (Vector(485.57633931733767,182.6902996133271, 0.0));
    positionAlloc4->Add (Vector(456.05183548867944,277.13988521851394, 0.0));
    positionAlloc4->Add (Vector(496.166654769514,313.1691165927484, 0.0));
    positionAlloc4->Add (Vector(387.4370954786302,286.39082257926106, 0.0));
    positionAlloc4->Add (Vector(153.9463856381551,60.7665500493616, 0.0));
    positionAlloc4->Add (Vector(215.86609703955696,210.95882475655353, 0.0));
    positionAlloc4->Add (Vector(221.6341478718744,491.65918786723313, 0.0));
    positionAlloc4->Add (Vector(145.0442738232406,482.0435715357712, 0.0));
    positionAlloc4->Add (Vector(351.22698267985265,298.8425391512293, 0.0));
    positionAlloc4->Add (Vector(80.12096559122533,232.60470639936608, 0.0));
    positionAlloc4->Add (Vector(276.99382923301994,221.17193605389724, 0.0));
    positionAlloc4->Add (Vector(320.0524503764853,280.718185723368, 0.0));
    positionAlloc4->Add (Vector(386.156832168877,186.23490684767395, 0.0));
    positionAlloc4->Add (Vector(103.78293141062872,178.8260835655745, 0.0));
    positionAlloc4->Add (Vector(277.0833184058626,183.52195008959248, 0.0));
    positionAlloc4->Add (Vector(128.67690738168196,116.48189019410098, 0.0));
    positionAlloc4->Add (Vector(450.16242701410357,33.31410357009229, 0.0));
    positionAlloc4->Add (Vector(494.1320149807825,92.68797705701893, 0.0));
    positionAlloc4->Add (Vector(331.61693115350664,491.6114452336985, 0.0));
    positionAlloc4->Add (Vector(318.2263540734764,392.0500037339133, 0.0));
    positionAlloc4->Add (Vector(7.0338798412538095,261.1505672552823, 0.0));
    positionAlloc4->Add (Vector(334.5946985960318,388.6656945614611, 0.0));
    positionAlloc4->Add (Vector(9.726514646152262,137.08573609246216, 0.0));
    positionAlloc4->Add (Vector(358.0222611476034,76.72983883491335, 0.0));
    positionAlloc4->Add (Vector(491.6911079680583,278.19243242740293, 0.0));
    positionAlloc4->Add (Vector(90.81457724526348,449.51739296502456, 0.0));
    positionAlloc4->Add (Vector(370.37425965943333,30.53824967363322, 0.0));
    positionAlloc4->Add (Vector(325.5667155880205,480.1751955039278, 0.0));
    positionAlloc4->Add (Vector(333.05238973309946,215.29631685531058, 0.0));
    positionAlloc4->Add (Vector(330.1016872742485,403.04608177031815, 0.0));
    positionAlloc4->Add (Vector(472.68181232822747,75.30322585901101, 0.0));
    positionAlloc4->Add (Vector(129.7532114183233,360.00847541671635, 0.0));
    positionAlloc4->Add (Vector(14.204228642653083,381.44491866448413, 0.0));
    positionAlloc4->Add (Vector(327.38877766256064,470.6114203947416, 0.0));
    positionAlloc4->Add (Vector(436.0780426828044,42.32756184707414, 0.0));
    positionAlloc4->Add (Vector(109.70506784601037,94.42810579455407, 0.0));
    positionAlloc4->Add (Vector(139.1483479708782,494.7084348239058, 0.0));
    positionAlloc4->Add (Vector(349.3598986314028,127.04921930913576, 0.0));
    positionAlloc4->Add (Vector(333.4199901940419,251.17787751245984, 0.0));
    positionAlloc4->Add (Vector(165.28431762925362,320.43866654529756, 0.0));
    positionAlloc4->Add (Vector(126.71051338815815,342.35898208890455, 0.0));
    positionAlloc4->Add (Vector(58.70027386832799,264.3383214592427, 0.0));
    positionAlloc4->Add (Vector(138.20760691215804,133.4046952249186, 0.0));
    positionAlloc4->Add (Vector(239.19650330104386,400.04596843810515, 0.0));
    positionAlloc4->Add (Vector(348.2630989409234,313.33073851267363, 0.0));
    positionAlloc4->Add (Vector(230.32387764915342,430.736779444878, 0.0));
    positionAlloc4->Add (Vector(314.3785191061767,42.2569203108083, 0.0));
    positionAlloc4->Add (Vector(177.74299701085272,318.25479780175567, 0.0));
    positionAlloc4->Add (Vector(49.37663203303666,466.0841565500646, 0.0));
    positionAlloc4->Add (Vector(494.51049623426667,349.91808448133, 0.0));
    positionAlloc4->Add (Vector(402.20696607474747,8.300476482541619, 0.0));
    positionAlloc4->Add (Vector(23.666061138640227,415.29388592071564, 0.0));
    positionAlloc4->Add (Vector(168.4391803101853,412.75937573363626, 0.0));
    positionAlloc4->Add (Vector(162.62696204037402,24.91919644588969, 0.0));
    positionAlloc4->Add (Vector(111.91567524098161,134.97097636134208, 0.0));
    positionAlloc4->Add (Vector(256.16779465196987,12.63101975858194, 0.0));
    positionAlloc4->Add (Vector(268.2384056173412,31.72310032918141, 0.0));
    positionAlloc4->Add (Vector(107.50947103817587,120.45257127523212, 0.0));
    positionAlloc4->Add (Vector(92.04000187910727,285.2221637361095, 0.0));
    positionAlloc4->Add (Vector(158.82138060730534,158.69689473697275, 0.0));
    positionAlloc4->Add (Vector(334.6222111343746,96.89579546248368, 0.0));
    positionAlloc4->Add (Vector(98.58788080313768,496.6547546175411, 0.0));
    positionAlloc4->Add (Vector(282.15831913989297,67.0929114896292, 0.0));
    positionAlloc4->Add (Vector(456.35720121401033,165.21624225777504, 0.0));
    positionAlloc4->Add (Vector(238.0797748191142,367.6231151695276, 0.0));
    positionAlloc4->Add (Vector(71.44906628840114,27.92864902700115, 0.0));
    positionAlloc4->Add (Vector(119.04701241566407,399.7769576628665, 0.0));
    positionAlloc4->Add (Vector(340.48478641694084,237.2622272321823, 0.0));
    positionAlloc4->Add (Vector(111.0281859511829,59.92643853794699, 0.0));
    positionAlloc4->Add (Vector(159.86005021563653,213.21630639404952, 0.0));
    positionAlloc4->Add (Vector(17.517284534455335,47.46020963530279, 0.0));
    positionAlloc4->Add (Vector(442.0165745975696,431.29986374494337, 0.0));
    positionAlloc4->Add (Vector(24.615397569833387,82.00323254396335, 0.0));
    positionAlloc4->Add (Vector(308.9238050884556,230.59953778529734, 0.0));
    positionAlloc4->Add (Vector(273.1758827328601,385.6830315589288, 0.0));
    positionAlloc4->Add (Vector(192.07406922702592,144.57988983906844, 0.0));
    positionAlloc4->Add (Vector(172.23254323075577,74.79828049451592, 0.0));
    positionAlloc4->Add (Vector(50.84902446922423,177.25818331902266, 0.0));
    positionAlloc4->Add (Vector(377.26416772156483,63.812043933124066, 0.0));
    positionAlloc4->Add (Vector(63.30229596384024,49.518429297046694, 0.0));
    positionAlloc4->Add (Vector(416.8120827920799,259.2770091093136, 0.0));
    positionAlloc4->Add (Vector(50.94896233640933,109.61163482791386, 0.0));
    positionAlloc4->Add (Vector(369.0154235073231,42.284981826936466, 0.0));
    positionAlloc4->Add (Vector(15.01748849771456,55.29519958491408, 0.0));
    positionAlloc4->Add (Vector(62.677835734493925,60.133770567983916, 0.0));
    positionAlloc4->Add (Vector(253.3253409975142,469.945499696142, 0.0));
    positionAlloc4->Add (Vector(439.4257707386592,306.57039406167047, 0.0));
    positionAlloc4->Add (Vector(438.26925492872505,255.30924848332194, 0.0));
    positionAlloc4->Add (Vector(214.89918048319367,136.83678727331895, 0.0));
    positionAlloc4->Add (Vector(422.72376675436794,252.8506659964559, 0.0));
    positionAlloc4->Add (Vector(448.5639377942062,217.9091059251014, 0.0));
    positionAlloc4->Add (Vector(350.5258569453839,174.37486098334287, 0.0));
    positionAlloc4->Add (Vector(188.9090502786368,195.88584146477683, 0.0));
    positionAlloc4->Add (Vector(246.38919189181308,189.25353067147228, 0.0));
    positionAlloc4->Add (Vector(210.3932155448558,274.29870511776176, 0.0));
    positionAlloc4->Add (Vector(72.40835991347572,409.6406728738206, 0.0));
    positionAlloc4->Add (Vector(254.6055358143224,77.06772538204359, 0.0));
    positionAlloc4->Add (Vector(429.064037915354,380.2027085119907, 0.0));
    positionAlloc4->Add (Vector(451.78101100939676,274.94705565748336, 0.0));
    positionAlloc4->Add (Vector(320.48947098322674,245.17352276612476, 0.0));
    positionAlloc4->Add (Vector(82.16101158473948,225.99103604814118, 0.0));
    positionAlloc4->Add (Vector(48.62872892894643,91.93812261086975, 0.0));
    positionAlloc4->Add (Vector(416.7632681126702,333.4229670367262, 0.0));
    positionAlloc4->Add (Vector(97.4427814598131,198.48162097687268, 0.0));
    positionAlloc4->Add (Vector(347.5187191845994,155.08661927269557, 0.0));
    positionAlloc4->Add (Vector(330.9844744325538,156.73934292618225, 0.0));
    positionAlloc4->Add (Vector(288.17723987141517,147.1156617619712, 0.0));
    positionAlloc4->Add (Vector(153.48083975244725,147.6230685754281, 0.0));
    positionAlloc4->Add (Vector(273.66105175864647,493.36511903300845, 0.0));
    positionAlloc4->Add (Vector(489.94738248118534,27.918152552883267, 0.0));
    positionAlloc4->Add (Vector(95.2441052640543,452.3375211626857, 0.0));
    positionAlloc4->Add (Vector(57.14496705651495,256.6720909023874, 0.0));
    positionAlloc4->Add (Vector(169.19582038123875,231.69605684106608, 0.0));
    positionAlloc4->Add (Vector(338.69838602304424,434.4214845781037, 0.0));
    positionAlloc4->Add (Vector(419.0777941792021,222.64648514925452, 0.0));
    positionAlloc4->Add (Vector(57.788704130085414,39.863137986219485, 0.0));
    positionAlloc4->Add (Vector(300.5198210455875,95.751081374442, 0.0));
    positionAlloc4->Add (Vector(392.8844813386788,461.2834981488468, 0.0));
    positionAlloc4->Add (Vector(233.1403067217636,57.80581143390218, 0.0));
    positionAlloc4->Add (Vector(157.79110326382732,256.39508547601304, 0.0));
    positionAlloc4->Add (Vector(244.1413020182357,402.6788134823789, 0.0));
    positionAlloc4->Add (Vector(51.14453252078249,310.62004353807026, 0.0));
    positionAlloc4->Add (Vector(440.49982593101635,175.61374247249117, 0.0));
    positionAlloc4->Add (Vector(239.87886253021296,458.12881408272693, 0.0));
    positionAlloc4->Add (Vector(99.88211108825857,29.53917364505004, 0.0));
    positionAlloc4->Add (Vector(316.96806385009353,109.83362831718263, 0.0));
    positionAlloc4->Add (Vector(414.8826328703356,52.27117202616671, 0.0));
    positionAlloc4->Add (Vector(205.27617288732998,348.53955997956376, 0.0));
    positionAlloc4->Add (Vector(208.63608409171252,336.604821810773, 0.0));
    positionAlloc4->Add (Vector(437.8921743823888,127.42384595202533, 0.0));
    positionAlloc4->Add (Vector(342.10920817842253,328.78914681948646, 0.0));
    positionAlloc4->Add (Vector(495.6816145590211,44.93190744596254, 0.0));
    positionAlloc4->Add (Vector(295.00801715969646,143.501996937138, 0.0));
    positionAlloc4->Add (Vector(43.45427782839323,55.2223455577438, 0.0));
    positionAlloc4->Add (Vector(159.88709021501884,154.4519057316316, 0.0));
    positionAlloc4->Add (Vector(57.09319737768864,334.6011222783796, 0.0));
    positionAlloc4->Add (Vector(316.55197713648204,430.464054088163, 0.0));
    positionAlloc4->Add (Vector(56.77195324891243,72.62202845804339, 0.0));
    positionAlloc4->Add (Vector(288.8649653730747,457.0402603117527, 0.0));
    positionAlloc4->Add (Vector(355.2431905350087,427.16279256224885, 0.0));
    positionAlloc4->Add (Vector(365.8250410454635,185.54207357523566, 0.0));
    positionAlloc4->Add (Vector(493.33760100001837,379.18043147958423, 0.0));
    positionAlloc4->Add (Vector(480.6365628227314,131.869871148159, 0.0));
    positionAlloc4->Add (Vector(298.5405025971738,87.59355322652112, 0.0));
    positionAlloc4->Add (Vector(190.34264471758271,489.53980658476826, 0.0));
    positionAlloc4->Add (Vector(5.252080499480782,457.3535031065794, 0.0));
    positionAlloc4->Add (Vector(229.22870561148488,38.200469749219366, 0.0));
    positionAlloc4->Add (Vector(200.63533346253604,261.54211569886985, 0.0));
    positionAlloc4->Add (Vector(7.561797201420417,316.0745638664042, 0.0));
    positionAlloc4->Add (Vector(317.73200522542726,48.71779390065484, 0.0));
    positionAlloc4->Add (Vector(397.5401858546412,41.92371604366363, 0.0));
    positionAlloc4->Add (Vector(9.640137305701513,181.38157746717616, 0.0));
    positionAlloc4->Add (Vector(76.02421162434058,42.96975311701612, 0.0));
    positionAlloc4->Add (Vector(120.19713947017846,417.61246403056236, 0.0));
    positionAlloc4->Add (Vector(188.54588893693509,381.08175582100716, 0.0));
    positionAlloc4->Add (Vector(193.65652883523876,134.7005559157556, 0.0));
    positionAlloc4->Add (Vector(156.53260359402793,284.77562793608126, 0.0));
    positionAlloc4->Add (Vector(225.53196746210435,55.522980920039764, 0.0));
    positionAlloc4->Add (Vector(167.3180858649877,326.2352578827448, 0.0));
    positionAlloc4->Add (Vector(325.83609615474853,282.9513221594182, 0.0));
    positionAlloc4->Add (Vector(71.82752473784937,133.5992057394581, 0.0));
    positionAlloc4->Add (Vector(304.3948056402136,116.04785299311904, 0.0));
    positionAlloc4->Add (Vector(210.63980048660997,395.3764719957652, 0.0));
    positionAlloc4->Add (Vector(411.7842926093259,270.39303944978946, 0.0));
    positionAlloc4->Add (Vector(52.301179548660095,475.69500243507395, 0.0));
    positionAlloc4->Add (Vector(299.5927759751285,30.012072215275243, 0.0));
    positionAlloc4->Add (Vector(183.27409989648635,170.65588789919244, 0.0));
    positionAlloc4->Add (Vector(413.4298204465406,437.13264407963646, 0.0));
    positionAlloc4->Add (Vector(87.91144714874466,111.15008759581502, 0.0));
    positionAlloc4->Add (Vector(101.99091039189473,87.32884720249018, 0.0));
    positionAlloc4->Add (Vector(177.39336046284276,151.4248306787317, 0.0));
    positionAlloc4->Add (Vector(310.506746651573,266.9688110103755, 0.0));
    positionAlloc4->Add (Vector(26.8184184880374,0.6044367986241528, 0.0));
    positionAlloc4->Add (Vector(118.60733418963343,154.4157616359108, 0.0));
    positionAlloc4->Add (Vector(190.49699194970665,23.06235939786072, 0.0));
    positionAlloc4->Add (Vector(348.38561204682327,336.1612571741664, 0.0));
    positionAlloc4->Add (Vector(146.00655050515476,456.49471700788285, 0.0));
    positionAlloc4->Add (Vector(345.7875352782942,206.54463317842308, 0.0));
    positionAlloc4->Add (Vector(451.49134764561774,274.9065092829822, 0.0));
    positionAlloc4->Add (Vector(349.3109186263777,233.4960148177666, 0.0));
    positionAlloc4->Add (Vector(170.09045876182566,370.9841711753153, 0.0));
    positionAlloc4->Add (Vector(84.27377092902022,108.7531753993472, 0.0));
    positionAlloc4->Add (Vector(229.491543591961,378.3462564135237, 0.0));
    positionAlloc4->Add (Vector(17.127118010729745,412.33742681081105, 0.0));
    positionAlloc4->Add (Vector(354.49866441001984,171.0671258122065, 0.0));
    positionAlloc4->Add (Vector(257.93988084391657,344.44778205040706, 0.0));
    positionAlloc4->Add (Vector(93.11674389408952,306.9564377583122, 0.0));
    positionAlloc4->Add (Vector(380.540573945556,219.01575717889938, 0.0));
    positionAlloc4->Add (Vector(92.95355405445261,171.23527480216111, 0.0));
    positionAlloc4->Add (Vector(308.5414077554549,388.7089452881762, 0.0));
    positionAlloc4->Add (Vector(117.26405056993677,99.2712463262837, 0.0));
    positionAlloc4->Add (Vector(24.6841706154457,232.941704811638, 0.0));
    positionAlloc4->Add (Vector(394.3961811629037,125.1671013135654, 0.0));
    positionAlloc4->Add (Vector(477.12986123291137,9.276561679540606, 0.0));
    positionAlloc4->Add (Vector(2.9790687677920813,111.13985441609209, 0.0));
    positionAlloc4->Add (Vector(194.57186262398142,46.16396013508262, 0.0));
    positionAlloc4->Add (Vector(63.863988457719955,109.3561038766751, 0.0));
    positionAlloc4->Add (Vector(207.48692352559723,440.12096417072615, 0.0));
    positionAlloc4->Add (Vector(183.47092788341706,469.17214317821805, 0.0));
    positionAlloc4->Add (Vector(371.13511656081727,298.3802844399783, 0.0));
    positionAlloc4->Add (Vector(15.970024759293933,370.2768763823137, 0.0));
    positionAlloc4->Add (Vector(291.3041090712028,353.17340888789437, 0.0));
    positionAlloc4->Add (Vector(336.8184543083708,403.20396190450435, 0.0));
    positionAlloc4->Add (Vector(350.5384028819882,377.6984211626289, 0.0));
    positionAlloc4->Add (Vector(316.5691204015734,53.95844204276196, 0.0));
    positionAlloc4->Add (Vector(295.44261239253126,432.2864565488282, 0.0));
    positionAlloc4->Add (Vector(490.10529770579035,285.2385813151882, 0.0));
    positionAlloc4->Add (Vector(456.90102146125474,444.81639280515185, 0.0));
    positionAlloc4->Add (Vector(48.86249369865775,359.263011542837, 0.0));
    positionAlloc4->Add (Vector(460.37037325877714,253.3221965809661, 0.0));
    positionAlloc4->Add (Vector(220.35905300484126,179.98741768319033, 0.0));
    positionAlloc4->Add (Vector(292.16961600599177,197.7056036161081, 0.0));
    positionAlloc4->Add (Vector(36.527066116455785,448.06812293353664, 0.0));
    positionAlloc4->Add (Vector(278.86663352663624,325.8255306024645, 0.0));
    positionAlloc4->Add (Vector(323.2130431820025,414.6525122272876, 0.0));
    positionAlloc4->Add (Vector(321.6416599844745,151.87405324421732, 0.0));
    positionAlloc4->Add (Vector(266.327837849173,381.95314924957586, 0.0));
    positionAlloc4->Add (Vector(323.1327332708377,290.9756959664833, 0.0));
    positionAlloc4->Add (Vector(128.42891069707514,192.15254784018094, 0.0));
    positionAlloc4->Add (Vector(432.4372312222605,475.6652921924313, 0.0));
    positionAlloc4->Add (Vector(158.62336319500088,84.99130412559325, 0.0));
    positionAlloc4->Add (Vector(272.8652886725872,361.5267191982771, 0.0));
    positionAlloc4->Add (Vector(165.01226095455047,32.018564406702254, 0.0));
    positionAlloc4->Add (Vector(334.1304888914279,182.50247782177965, 0.0));
    positionAlloc4->Add (Vector(172.48630945505,464.3834693546608, 0.0));
    positionAlloc4->Add (Vector(168.98416681107224,102.84651293788288, 0.0));
    positionAlloc4->Add (Vector(423.6762455861499,63.67104797246925, 0.0));
    positionAlloc4->Add (Vector(316.35865691136325,237.9542591668809, 0.0));
    positionAlloc4->Add (Vector(406.9152340169772,403.4318036138974, 0.0));
    positionAlloc4->Add (Vector(175.3332008455002,240.04117286573302, 0.0));
    positionAlloc4->Add (Vector(245.4919945004454,123.69761880403502, 0.0));
    positionAlloc4->Add (Vector(366.1535737775081,239.4726359645864, 0.0));
    positionAlloc4->Add (Vector(395.44959687396545,305.68662902286104, 0.0));
    positionAlloc4->Add (Vector(192.5897894925599,97.82302782713259, 0.0));
    positionAlloc4->Add (Vector(139.34620052870622,478.0305200806783, 0.0));
    positionAlloc4->Add (Vector(319.7521475390519,289.8119185828897, 0.0));
    positionAlloc4->Add (Vector(336.4680122954939,168.3883388180356, 0.0));
    positionAlloc4->Add (Vector(162.95166148448448,417.80471868435677, 0.0));
    positionAlloc4->Add (Vector(240.45839717591411,284.32546181600605, 0.0));
    positionAlloc4->Add (Vector(351.76415315040094,311.84254056377534, 0.0));
    positionAlloc4->Add (Vector(22.572978628676132,161.75733100391182, 0.0));
    positionAlloc4->Add (Vector(308.5906965339811,199.6952192245347, 0.0));
    positionAlloc4->Add (Vector(217.8000247591747,356.94606137758626, 0.0));
    positionAlloc4->Add (Vector(361.3707709243574,18.670152445842415, 0.0));
    positionAlloc4->Add (Vector(234.7674455661234,238.83039853264592, 0.0));
    positionAlloc4->Add (Vector(281.27202818296234,19.73164764280583, 0.0));
    positionAlloc4->Add (Vector(208.5356829584606,29.732388695755674, 0.0));
    positionAlloc4->Add (Vector(383.9867753317324,107.52273432861936, 0.0));
    positionAlloc4->Add (Vector(402.53133991272534,428.48905963669085, 0.0));
    positionAlloc4->Add (Vector(44.976286342444105,240.2552760203206, 0.0));
    positionAlloc4->Add (Vector(415.47909318106605,60.57994904098274, 0.0));
    positionAlloc4->Add (Vector(187.1097573889454,179.9031930471955, 0.0));
    positionAlloc4->Add (Vector(129.28423760232488,342.365110042652, 0.0));
    positionAlloc4->Add (Vector(86.71871175485457,465.71245683810366, 0.0));
    positionAlloc4->Add (Vector(229.6860081061094,340.53187934288843, 0.0));
    positionAlloc4->Add (Vector(367.7077734271818,243.97387898336947, 0.0));
    positionAlloc4->Add (Vector(167.29882897677163,128.87487112636148, 0.0));
    positionAlloc4->Add (Vector(24.064931990335214,155.02786335457287, 0.0));
    positionAlloc4->Add (Vector(34.684097731912054,427.4452553053834, 0.0));
    positionAlloc4->Add (Vector(119.32584615410529,148.10830997920948, 0.0));
    positionAlloc4->Add (Vector(474.2781118937435,384.0264898784885, 0.0));
    positionAlloc4->Add (Vector(225.97271193320273,252.65453323965653, 0.0));
    positionAlloc4->Add (Vector(394.8882172929421,122.72362628310685, 0.0));
    positionAlloc4->Add (Vector(458.0995449904607,119.36313527948006, 0.0));
    positionAlloc4->Add (Vector(283.08347517284096,454.6908456920304, 0.0));
    positionAlloc4->Add (Vector(268.05697453652647,417.79005111659006, 0.0));
    positionAlloc4->Add (Vector(122.56091372712535,252.5392660556934, 0.0));
    positionAlloc4->Add (Vector(151.58517453615872,257.21470317669, 0.0));
    positionAlloc4->Add (Vector(217.6046380850749,483.01652145162797, 0.0));
    positionAlloc4->Add (Vector(388.31748165610816,371.1553496052596, 0.0));
    positionAlloc4->Add (Vector(249.91053520171903,81.91781453314512, 0.0));
    positionAlloc4->Add (Vector(1.4698821447454158,490.8847043310247, 0.0));
    positionAlloc4->Add (Vector(348.8950673280473,463.1199645266093, 0.0));
    positionAlloc4->Add (Vector(439.2611404092607,61.13394056049276, 0.0));
    positionAlloc4->Add (Vector(228.28718201465608,337.14042599222887, 0.0));
    positionAlloc4->Add (Vector(211.14925127927975,307.9696411243957, 0.0));
    positionAlloc4->Add (Vector(195.60776616942417,202.80656542585595, 0.0));
    positionAlloc4->Add (Vector(268.0619276408943,346.3155078824836, 0.0));
    positionAlloc4->Add (Vector(489.80553271061007,121.18824604524936, 0.0));
    positionAlloc4->Add (Vector(481.36016385924404,417.8989704367115, 0.0));
    positionAlloc4->Add (Vector(373.1475053967925,141.6925245983523, 0.0));
    positionAlloc4->Add (Vector(100.35108448073294,114.21434789182516, 0.0));
    positionAlloc4->Add (Vector(297.4679189534562,157.29413302301214, 0.0));
    positionAlloc4->Add (Vector(469.11261348322904,476.2462999683014, 0.0));
    positionAlloc4->Add (Vector(474.38866380421643,461.6550460479674, 0.0));
    positionAlloc4->Add (Vector(3.055341764143571,76.06610357628846, 0.0));
    positionAlloc4->Add (Vector(33.11028972062419,142.52882218564483, 0.0));
    positionAlloc4->Add (Vector(118.62027394335739,258.1408024979307, 0.0));
    positionAlloc4->Add (Vector(11.137700490254087,42.06611099086199, 0.0));
    positionAlloc4->Add (Vector(347.9750584503714,19.69788345386686, 0.0));
    positionAlloc4->Add (Vector(20.300919099648375,417.69531902449387, 0.0));
    positionAlloc4->Add (Vector(272.8538234350614,263.40900053343455, 0.0));
    positionAlloc4->Add (Vector(472.7050408574293,150.4374951102332, 0.0));
    positionAlloc4->Add (Vector(400.4892204670127,358.8257471050879, 0.0));
    positionAlloc4->Add (Vector(442.0645690432233,128.27389222190195, 0.0));
    positionAlloc4->Add (Vector(442.3111682673529,34.65616226347773, 0.0));
    positionAlloc4->Add (Vector(247.2137661648695,353.6648315452999, 0.0));
    positionAlloc4->Add (Vector(42.24567817059349,66.87668057408541, 0.0));
    positionAlloc4->Add (Vector(375.69882951923694,224.2033111742558, 0.0));
    positionAlloc4->Add (Vector(317.99866835715324,478.42162677823575, 0.0));
    positionAlloc4->Add (Vector(441.94998343014265,251.84274920841648, 0.0));
    positionAlloc4->Add (Vector(40.838957287602504,471.57201895188354, 0.0));
    positionAlloc4->Add (Vector(370.22018255350866,378.96924422883836, 0.0));
    positionAlloc4->Add (Vector(251.8646829123581,221.18663478547873, 0.0));
    positionAlloc4->Add (Vector(272.14001827135,325.27679901611583, 0.0));
    positionAlloc4->Add (Vector(25.72022663364104,286.4472214885678, 0.0));
    positionAlloc4->Add (Vector(453.82133267579326,45.9317745448693, 0.0));
    positionAlloc4->Add (Vector(400.2685204876279,382.0377610264015, 0.0));
    positionAlloc4->Add (Vector(467.3914114696333,26.09762208475669, 0.0));
    positionAlloc4->Add (Vector(406.79618449534814,464.42571540902384, 0.0));
    positionAlloc4->Add (Vector(309.8691610433279,101.38569911019945, 0.0));
    positionAlloc4->Add (Vector(305.47923106667105,340.94851638342385, 0.0));
    positionAlloc4->Add (Vector(271.8056881882649,242.04215405332042, 0.0));
    positionAlloc4->Add (Vector(336.9040967635993,15.075079163323736, 0.0));
    positionAlloc4->Add (Vector(277.75453856764597,347.9443934934836, 0.0));
    positionAlloc4->Add (Vector(62.563721876497226,304.8986497862848, 0.0));
    positionAlloc4->Add (Vector(376.97572527039875,279.6828817668884, 0.0));
    positionAlloc4->Add (Vector(48.47754891190165,83.85762161083976, 0.0));
    positionAlloc4->Add (Vector(330.0556678194866,164.50100815307212, 0.0));
    positionAlloc4->Add (Vector(131.21956868797136,432.9307021716023, 0.0));
    positionAlloc4->Add (Vector(370.23772300842785,282.3559339404362, 0.0));
    positionAlloc4->Add (Vector(474.3747683682918,307.31068544832624, 0.0));
    positionAlloc4->Add (Vector(135.32320536093638,255.1214650265906, 0.0));
    positionAlloc4->Add (Vector(24.01425729524309,104.81622420874508, 0.0));
    positionAlloc4->Add (Vector(227.06142999485346,46.80611193605322, 0.0));
    positionAlloc4->Add (Vector(136.0523450024294,482.7324923069989, 0.0));
    positionAlloc4->Add (Vector(323.43489581644144,398.10059949605454, 0.0));
    positionAlloc4->Add (Vector(466.5626042884886,89.82891398498921, 0.0));
    positionAlloc4->Add (Vector(121.00572033223517,469.59559053877473, 0.0));
    positionAlloc4->Add (Vector(445.93907271473626,23.764222571032832, 0.0));
    positionAlloc4->Add (Vector(290.1542783482772,450.32574712490185, 0.0));
    positionAlloc4->Add (Vector(389.9689076177411,358.0844183561505, 0.0));
    positionAlloc4->Add (Vector(385.7260607761765,367.07980153673157, 0.0));
    positionAlloc4->Add (Vector(370.34638598278553,463.6365352236953, 0.0));
    positionAlloc4->Add (Vector(486.1606842727992,283.3295875551934, 0.0));
    positionAlloc4->Add (Vector(478.9772323995055,134.35841805989824, 0.0));
    positionAlloc4->Add (Vector(53.64896050697476,320.9947327786568, 0.0));
    positionAlloc4->Add (Vector(488.24957707937665,13.697176031706949, 0.0));
    positionAlloc4->Add (Vector(7.4764657661872125,221.02434288390012, 0.0));
    positionAlloc4->Add (Vector(275.40588961518273,246.56999447552369, 0.0));
    positionAlloc4->Add (Vector(342.7957128638346,442.5819321533943, 0.0));
    positionAlloc4->Add (Vector(132.53348157890366,122.32013893456983, 0.0));
    positionAlloc4->Add (Vector(106.6037184694395,200.58874261994674, 0.0));
    positionAlloc4->Add (Vector(322.1430510327253,459.19370124961154, 0.0));
    positionAlloc4->Add (Vector(289.2810680619592,40.05758266513121, 0.0));
    positionAlloc4->Add (Vector(201.1522894982578,115.29666449004961, 0.0));
    positionAlloc4->Add (Vector(270.9285427871286,280.73903213013637, 0.0));
    positionAlloc4->Add (Vector(95.70836804574606,488.4430603331636, 0.0));
    positionAlloc4->Add (Vector(472.5816486265843,56.72106385226539, 0.0));
    positionAlloc4->Add (Vector(444.33070207960606,463.2840334176427, 0.0));
    positionAlloc4->Add (Vector(490.1903689365774,0.5962628947762116, 0.0));
    positionAlloc4->Add (Vector(301.0323846266938,496.5986322976747, 0.0));
    positionAlloc4->Add (Vector(121.76922466355528,474.40136962128946, 0.0));
    positionAlloc4->Add (Vector(277.73955669820305,165.90476574806956, 0.0));
    positionAlloc4->Add (Vector(297.7740862738305,286.8583302839887, 0.0));
    positionAlloc4->Add (Vector(469.61019674345226,196.0278532330777, 0.0));
    positionAlloc4->Add (Vector(188.09782518878643,421.8246928044152, 0.0));
    positionAlloc4->Add (Vector(219.77069980090386,464.0448665226328, 0.0));
    positionAlloc4->Add (Vector(108.78668273403458,73.6477125265026, 0.0));
    positionAlloc4->Add (Vector(473.2869952170676,259.1300968131357, 0.0));
    positionAlloc4->Add (Vector(187.09374855370388,301.7973482069078, 0.0));
    positionAlloc4->Add (Vector(417.3173570452751,53.889848465074664, 0.0));
    positionAlloc4->Add (Vector(351.536702045281,485.08695059531436, 0.0));
    positionAlloc4->Add (Vector(8.738975342550582,407.799876022881, 0.0));
    positionAlloc4->Add (Vector(465.30015491256984,241.25826010628643, 0.0));
    positionAlloc4->Add (Vector(347.9342028770738,350.54620510818694, 0.0));
    positionAlloc4->Add (Vector(427.8170485051989,135.03811069782302, 0.0));
    positionAlloc4->Add (Vector(55.41118383399207,398.3871747347381, 0.0));
    positionAlloc4->Add (Vector(370.0150708218796,368.0163535513748, 0.0));
    positionAlloc4->Add (Vector(26.088573756380683,186.48508624058869, 0.0));
    positionAlloc4->Add (Vector(27.74155273155554,425.7409143192318, 0.0));
    positionAlloc4->Add (Vector(115.78719962034367,302.332303795797, 0.0));
    positionAlloc4->Add (Vector(461.0900865137673,85.76695418163766, 0.0));
    positionAlloc4->Add (Vector(37.38071591404351,346.3207737511312, 0.0));
    positionAlloc4->Add (Vector(145.19328482952503,314.84627572141426, 0.0));
    positionAlloc4->Add (Vector(176.9815186846309,424.6656077796981, 0.0));
    positionAlloc4->Add (Vector(445.11425874814284,60.858432394142014, 0.0));
    positionAlloc4->Add (Vector(166.49356003417813,487.026958931752, 0.0));
    positionAlloc4->Add (Vector(445.66317608522445,254.41495466187698, 0.0));
    positionAlloc4->Add (Vector(393.38830153057404,7.19536791687625, 0.0));
    positionAlloc4->Add (Vector(142.19053521972725,90.91258643712314, 0.0));
    positionAlloc4->Add (Vector(299.073952667067,278.60681916524544, 0.0));
    positionAlloc4->Add (Vector(24.53877651381142,348.69490249837406, 0.0));
    positionAlloc4->Add (Vector(213.87782447849264,347.72798011662996, 0.0));
    positionAlloc4->Add (Vector(416.3286625426296,447.48328015782334, 0.0));
    positionAlloc4->Add (Vector(138.5512726238506,443.80606916330066, 0.0));
    positionAlloc4->Add (Vector(122.5715426285397,302.39634405726474, 0.0));
    positionAlloc4->Add (Vector(413.0075501731185,220.82211814699914, 0.0));
    positionAlloc4->Add (Vector(264.49140179226157,31.280221716980694, 0.0));
    positionAlloc4->Add (Vector(90.82327489131131,210.7338784743561, 0.0));
    positionAlloc4->Add (Vector(122.46761260881468,321.6180958265871, 0.0));
    positionAlloc4->Add (Vector(242.91713462155224,97.65078323195209, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_24.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_ECC_flowmon/switch_ECC_24.flowmon", false, false);
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