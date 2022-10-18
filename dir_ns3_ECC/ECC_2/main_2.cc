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
	  uint16_t numberOfNodes = 552;
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
    positionAlloc->Add (Vector (500.0,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,111.11111111111111, 0.0));
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
    positionAlloc4->Add (Vector(265.89595003344425,390.85764269796994, 0.0));
    positionAlloc4->Add (Vector(271.27011657239547,441.2430864511858, 0.0));
    positionAlloc4->Add (Vector(72.96445903580174,36.13237840743633, 0.0));
    positionAlloc4->Add (Vector(450.5334967114069,63.87165695179664, 0.0));
    positionAlloc4->Add (Vector(485.30934921697195,464.77518825617034, 0.0));
    positionAlloc4->Add (Vector(80.38207116172991,358.11915323981077, 0.0));
    positionAlloc4->Add (Vector(345.31954039119705,28.783809212553745, 0.0));
    positionAlloc4->Add (Vector(273.3422860454021,172.05966848799875, 0.0));
    positionAlloc4->Add (Vector(75.3698295515291,121.88198869809058, 0.0));
    positionAlloc4->Add (Vector(351.77386610071426,458.63007954000426, 0.0));
    positionAlloc4->Add (Vector(439.5992802682978,84.4000950387968, 0.0));
    positionAlloc4->Add (Vector(376.1283024020466,308.87713980503395, 0.0));
    positionAlloc4->Add (Vector(90.36460680149489,347.1534694618643, 0.0));
    positionAlloc4->Add (Vector(341.53202381112345,32.810999480712425, 0.0));
    positionAlloc4->Add (Vector(113.39466033313245,204.0907014261411, 0.0));
    positionAlloc4->Add (Vector(423.751301135201,170.7370400106493, 0.0));
    positionAlloc4->Add (Vector(181.53666180224909,114.07081714646111, 0.0));
    positionAlloc4->Add (Vector(230.21699672946673,63.91083080319243, 0.0));
    positionAlloc4->Add (Vector(72.58664646004782,167.3402664422966, 0.0));
    positionAlloc4->Add (Vector(323.3249908358953,302.625392753531, 0.0));
    positionAlloc4->Add (Vector(326.36560045047736,245.37005822220664, 0.0));
    positionAlloc4->Add (Vector(479.5560087596011,276.67197472976034, 0.0));
    positionAlloc4->Add (Vector(171.71004655433615,175.3085946534768, 0.0));
    positionAlloc4->Add (Vector(276.0694037767407,284.0698588460962, 0.0));
    positionAlloc4->Add (Vector(415.37568345221285,76.71047058120617, 0.0));
    positionAlloc4->Add (Vector(115.47085416106206,143.1885657686912, 0.0));
    positionAlloc4->Add (Vector(297.9334728083256,72.7498946108544, 0.0));
    positionAlloc4->Add (Vector(34.06502869703721,275.6283244679258, 0.0));
    positionAlloc4->Add (Vector(191.64015506400628,90.15303042387967, 0.0));
    positionAlloc4->Add (Vector(225.44632825034998,174.36338477382296, 0.0));
    positionAlloc4->Add (Vector(100.81587952831717,208.23127808655406, 0.0));
    positionAlloc4->Add (Vector(20.843721260143887,329.98485948194957, 0.0));
    positionAlloc4->Add (Vector(182.0851493267629,283.3227783439494, 0.0));
    positionAlloc4->Add (Vector(158.9087751752989,171.99158839299517, 0.0));
    positionAlloc4->Add (Vector(393.20854113992385,77.76429193681084, 0.0));
    positionAlloc4->Add (Vector(197.49353005088255,398.74799214667433, 0.0));
    positionAlloc4->Add (Vector(243.21228690384305,349.57328270645826, 0.0));
    positionAlloc4->Add (Vector(431.7242201663173,354.22539575768633, 0.0));
    positionAlloc4->Add (Vector(44.06758196271693,433.0387146868598, 0.0));
    positionAlloc4->Add (Vector(50.710388440638354,356.1224021986278, 0.0));
    positionAlloc4->Add (Vector(273.4172279079042,194.63747270448283, 0.0));
    positionAlloc4->Add (Vector(206.1394639892849,305.77204972516074, 0.0));
    positionAlloc4->Add (Vector(430.9529996594165,130.8053963414609, 0.0));
    positionAlloc4->Add (Vector(378.3080281481086,468.9688321548911, 0.0));
    positionAlloc4->Add (Vector(117.45111040105243,187.02335843353478, 0.0));
    positionAlloc4->Add (Vector(330.20596733532017,124.54504422596641, 0.0));
    positionAlloc4->Add (Vector(231.23723260751422,434.8070425605564, 0.0));
    positionAlloc4->Add (Vector(260.4241702768946,370.14681764011084, 0.0));
    positionAlloc4->Add (Vector(87.16056875991862,83.36096646326413, 0.0));
    positionAlloc4->Add (Vector(227.9364036847512,326.6255711306597, 0.0));
    positionAlloc4->Add (Vector(111.61921547022175,403.292541130257, 0.0));
    positionAlloc4->Add (Vector(21.314840898289944,49.720128627442406, 0.0));
    positionAlloc4->Add (Vector(68.72177481211544,482.1863986930588, 0.0));
    positionAlloc4->Add (Vector(242.23503047425336,443.72753903636374, 0.0));
    positionAlloc4->Add (Vector(372.74916537408234,201.70331416708643, 0.0));
    positionAlloc4->Add (Vector(57.64671274001354,104.04870499947361, 0.0));
    positionAlloc4->Add (Vector(428.8693072557203,322.78220413671636, 0.0));
    positionAlloc4->Add (Vector(319.13906663341214,71.50205475916133, 0.0));
    positionAlloc4->Add (Vector(17.990483829320947,179.19762231266256, 0.0));
    positionAlloc4->Add (Vector(181.10805276500557,403.5952877567598, 0.0));
    positionAlloc4->Add (Vector(143.76297001201434,168.31159025614954, 0.0));
    positionAlloc4->Add (Vector(7.544653816489555,16.097201934285266, 0.0));
    positionAlloc4->Add (Vector(132.95352052391374,344.7474315888347, 0.0));
    positionAlloc4->Add (Vector(380.6676079960478,75.04923067602498, 0.0));
    positionAlloc4->Add (Vector(81.12249973940389,97.29686761907091, 0.0));
    positionAlloc4->Add (Vector(168.36054487252744,293.2147107321308, 0.0));
    positionAlloc4->Add (Vector(294.4543516530386,421.25910492077946, 0.0));
    positionAlloc4->Add (Vector(130.00948597834616,100.69592685686013, 0.0));
    positionAlloc4->Add (Vector(481.5161693923378,469.1308842511892, 0.0));
    positionAlloc4->Add (Vector(259.6487927352201,159.46793445871205, 0.0));
    positionAlloc4->Add (Vector(37.516319668270405,430.6762372415144, 0.0));
    positionAlloc4->Add (Vector(462.85205434511545,202.7786920695121, 0.0));
    positionAlloc4->Add (Vector(29.76430357003568,240.7002295745182, 0.0));
    positionAlloc4->Add (Vector(13.02996634419662,3.523361905037825, 0.0));
    positionAlloc4->Add (Vector(48.28951721420677,303.0176490844811, 0.0));
    positionAlloc4->Add (Vector(252.8816744932159,393.25038142996743, 0.0));
    positionAlloc4->Add (Vector(332.5841772910967,92.98048489816946, 0.0));
    positionAlloc4->Add (Vector(139.25208591902833,307.39094554164956, 0.0));
    positionAlloc4->Add (Vector(226.27931319321715,465.5015968822514, 0.0));
    positionAlloc4->Add (Vector(13.264175555224577,194.9193741874921, 0.0));
    positionAlloc4->Add (Vector(305.7868040607053,189.17691941762453, 0.0));
    positionAlloc4->Add (Vector(10.539836417755632,492.5227768052347, 0.0));
    positionAlloc4->Add (Vector(57.91337335312452,135.2759968987332, 0.0));
    positionAlloc4->Add (Vector(125.48076885812287,213.24647292810923, 0.0));
    positionAlloc4->Add (Vector(313.10446456861087,48.79130898804224, 0.0));
    positionAlloc4->Add (Vector(238.73053816982653,12.013367315404256, 0.0));
    positionAlloc4->Add (Vector(16.454358296267234,56.70550056541307, 0.0));
    positionAlloc4->Add (Vector(310.4761937902361,66.69701507261999, 0.0));
    positionAlloc4->Add (Vector(139.17966519715802,243.73565081292452, 0.0));
    positionAlloc4->Add (Vector(288.4146516160124,326.99646743886683, 0.0));
    positionAlloc4->Add (Vector(223.76418979562763,425.6188312507487, 0.0));
    positionAlloc4->Add (Vector(452.6886899798365,173.70696640030252, 0.0));
    positionAlloc4->Add (Vector(278.79390758555587,286.7805412885476, 0.0));
    positionAlloc4->Add (Vector(23.82735248483947,223.78957202531907, 0.0));
    positionAlloc4->Add (Vector(99.11614510507688,142.8582476856514, 0.0));
    positionAlloc4->Add (Vector(139.66016780816594,457.9393615336501, 0.0));
    positionAlloc4->Add (Vector(73.3043508884545,306.23056431403745, 0.0));
    positionAlloc4->Add (Vector(416.9523387496166,349.1408996459313, 0.0));
    positionAlloc4->Add (Vector(385.68125360073094,285.81766821747567, 0.0));
    positionAlloc4->Add (Vector(461.80787317038033,339.0414670619912, 0.0));
    positionAlloc4->Add (Vector(465.3673665750739,134.52119362595926, 0.0));
    positionAlloc4->Add (Vector(351.15787739740375,312.50779659666057, 0.0));
    positionAlloc4->Add (Vector(143.7490510205938,478.61267118719667, 0.0));
    positionAlloc4->Add (Vector(159.0543761723084,383.24607232811485, 0.0));
    positionAlloc4->Add (Vector(348.4391632184026,452.90420835327615, 0.0));
    positionAlloc4->Add (Vector(192.05711399811332,73.37396536374136, 0.0));
    positionAlloc4->Add (Vector(491.5194917697084,299.2136111940074, 0.0));
    positionAlloc4->Add (Vector(274.94496023599055,7.710911171627921, 0.0));
    positionAlloc4->Add (Vector(331.8013158355468,43.16664340758203, 0.0));
    positionAlloc4->Add (Vector(199.22909827452006,188.61978376525474, 0.0));
    positionAlloc4->Add (Vector(372.4169365162464,147.8627672119406, 0.0));
    positionAlloc4->Add (Vector(204.02356767396435,25.167564829725876, 0.0));
    positionAlloc4->Add (Vector(313.1794016043418,311.7759005227791, 0.0));
    positionAlloc4->Add (Vector(427.02989746715724,135.4224569179583, 0.0));
    positionAlloc4->Add (Vector(95.63602587058506,193.52311630254292, 0.0));
    positionAlloc4->Add (Vector(137.52702553270584,54.033742586168664, 0.0));
    positionAlloc4->Add (Vector(271.3082059358602,442.028106189343, 0.0));
    positionAlloc4->Add (Vector(177.4512554253867,326.7655163318297, 0.0));
    positionAlloc4->Add (Vector(486.06954356072606,371.26122617585486, 0.0));
    positionAlloc4->Add (Vector(331.5744457321584,220.68961547675042, 0.0));
    positionAlloc4->Add (Vector(442.91155857486075,282.64483518278416, 0.0));
    positionAlloc4->Add (Vector(137.73101332505206,254.70429071938116, 0.0));
    positionAlloc4->Add (Vector(320.87459041679836,187.04009386912807, 0.0));
    positionAlloc4->Add (Vector(346.597279189194,293.7422396752046, 0.0));
    positionAlloc4->Add (Vector(438.98020936257836,17.31700066886016, 0.0));
    positionAlloc4->Add (Vector(412.5876652832098,15.992092187886941, 0.0));
    positionAlloc4->Add (Vector(269.46577665381716,271.0380773695615, 0.0));
    positionAlloc4->Add (Vector(42.0106398993626,414.78685922907454, 0.0));
    positionAlloc4->Add (Vector(451.0380007624223,197.87513078259033, 0.0));
    positionAlloc4->Add (Vector(429.3953781334824,461.5015203768327, 0.0));
    positionAlloc4->Add (Vector(371.2940288482734,363.55059420183846, 0.0));
    positionAlloc4->Add (Vector(327.26876731141573,427.1291049213233, 0.0));
    positionAlloc4->Add (Vector(429.3058515350063,473.7707755509121, 0.0));
    positionAlloc4->Add (Vector(263.452673023672,376.93453581060646, 0.0));
    positionAlloc4->Add (Vector(66.40374229020551,378.78517962371063, 0.0));
    positionAlloc4->Add (Vector(464.9965444817507,312.66557745162754, 0.0));
    positionAlloc4->Add (Vector(134.13141456796816,398.37273779504204, 0.0));
    positionAlloc4->Add (Vector(152.6567657241534,6.261418443236632, 0.0));
    positionAlloc4->Add (Vector(260.82965670663594,356.1560978237146, 0.0));
    positionAlloc4->Add (Vector(314.01153930346294,151.87921707593432, 0.0));
    positionAlloc4->Add (Vector(228.8526438402096,377.3111126540659, 0.0));
    positionAlloc4->Add (Vector(461.55034129433193,373.08007038671053, 0.0));
    positionAlloc4->Add (Vector(481.50911979776726,251.9915046993914, 0.0));
    positionAlloc4->Add (Vector(159.49300323509453,60.95990694504383, 0.0));
    positionAlloc4->Add (Vector(206.84536809955068,107.35784783419028, 0.0));
    positionAlloc4->Add (Vector(184.28843488263797,146.3632810465651, 0.0));
    positionAlloc4->Add (Vector(376.6900365627929,141.2140152794863, 0.0));
    positionAlloc4->Add (Vector(451.26827821112255,153.5803842425475, 0.0));
    positionAlloc4->Add (Vector(317.5624805456691,440.1099886092915, 0.0));
    positionAlloc4->Add (Vector(96.9324298297734,247.5561812805891, 0.0));
    positionAlloc4->Add (Vector(23.805464353277518,78.1784643381258, 0.0));
    positionAlloc4->Add (Vector(371.4606251948485,170.46012456617103, 0.0));
    positionAlloc4->Add (Vector(100.79831720408555,163.76202792414458, 0.0));
    positionAlloc4->Add (Vector(287.4698636388833,404.60829185867226, 0.0));
    positionAlloc4->Add (Vector(151.0907502079021,183.36599083047085, 0.0));
    positionAlloc4->Add (Vector(271.5820237596019,307.8629643065187, 0.0));
    positionAlloc4->Add (Vector(435.1289744612991,273.4841546356204, 0.0));
    positionAlloc4->Add (Vector(2.4360239768030367,334.5788988985684, 0.0));
    positionAlloc4->Add (Vector(73.37368988038978,141.99161273309258, 0.0));
    positionAlloc4->Add (Vector(303.2677152582717,14.891472064780142, 0.0));
    positionAlloc4->Add (Vector(365.5688015994833,272.7355842743597, 0.0));
    positionAlloc4->Add (Vector(447.56885540178666,55.27872012772062, 0.0));
    positionAlloc4->Add (Vector(195.38653683534633,233.61928316662295, 0.0));
    positionAlloc4->Add (Vector(263.9827420773504,33.01779200831867, 0.0));
    positionAlloc4->Add (Vector(459.9767885074294,220.67587265791894, 0.0));
    positionAlloc4->Add (Vector(220.220279987859,377.0875127275797, 0.0));
    positionAlloc4->Add (Vector(358.2940233973671,352.7902060603345, 0.0));
    positionAlloc4->Add (Vector(248.78219230253373,410.58848244636795, 0.0));
    positionAlloc4->Add (Vector(470.4142071907624,108.60987519550896, 0.0));
    positionAlloc4->Add (Vector(499.29954429324,121.43926080988376, 0.0));
    positionAlloc4->Add (Vector(331.5784114409828,119.45497743510603, 0.0));
    positionAlloc4->Add (Vector(366.38492663068337,220.16891339821888, 0.0));
    positionAlloc4->Add (Vector(146.4989418141786,288.7453405067737, 0.0));
    positionAlloc4->Add (Vector(14.636060701556264,277.54338326108723, 0.0));
    positionAlloc4->Add (Vector(137.2370564017854,263.35793017930786, 0.0));
    positionAlloc4->Add (Vector(14.980876262024267,231.26453017110143, 0.0));
    positionAlloc4->Add (Vector(94.56918746942489,196.79170714843096, 0.0));
    positionAlloc4->Add (Vector(181.2326586500127,244.62239273007108, 0.0));
    positionAlloc4->Add (Vector(374.7212353760848,286.3441466462077, 0.0));
    positionAlloc4->Add (Vector(257.60734808103683,239.41170059273585, 0.0));
    positionAlloc4->Add (Vector(196.4799513171016,494.7260654181265, 0.0));
    positionAlloc4->Add (Vector(268.28740967021645,387.47268999887285, 0.0));
    positionAlloc4->Add (Vector(92.08888828044654,319.1716650016937, 0.0));
    positionAlloc4->Add (Vector(9.340502998187672,243.47512321197857, 0.0));
    positionAlloc4->Add (Vector(263.95939876536556,86.08127747796634, 0.0));
    positionAlloc4->Add (Vector(335.0451843739035,101.7801888309508, 0.0));
    positionAlloc4->Add (Vector(312.31180357782057,193.6802610722404, 0.0));
    positionAlloc4->Add (Vector(375.0621240344925,54.8675106428283, 0.0));
    positionAlloc4->Add (Vector(226.97632520325993,310.57459484682306, 0.0));
    positionAlloc4->Add (Vector(51.40918372715792,97.91711683749621, 0.0));
    positionAlloc4->Add (Vector(189.05429339151064,347.99825522274705, 0.0));
    positionAlloc4->Add (Vector(92.521674818488,31.30990099195774, 0.0));
    positionAlloc4->Add (Vector(221.75192095196755,348.92082802492416, 0.0));
    positionAlloc4->Add (Vector(2.9404907223141152,367.47100934198073, 0.0));
    positionAlloc4->Add (Vector(326.6290260122272,95.14174133952186, 0.0));
    positionAlloc4->Add (Vector(250.04662979846475,160.42445080043228, 0.0));
    positionAlloc4->Add (Vector(299.33194080321783,305.7497383298012, 0.0));
    positionAlloc4->Add (Vector(160.1033271431444,73.48906292746565, 0.0));
    positionAlloc4->Add (Vector(2.4881823556326954,97.25820692980092, 0.0));
    positionAlloc4->Add (Vector(291.2199309446113,214.16619901942818, 0.0));
    positionAlloc4->Add (Vector(248.11806948513959,310.23410030259527, 0.0));
    positionAlloc4->Add (Vector(248.98931633664418,330.0076117630255, 0.0));
    positionAlloc4->Add (Vector(139.81058972492443,211.7550265525915, 0.0));
    positionAlloc4->Add (Vector(43.73638109252176,199.87749384043224, 0.0));
    positionAlloc4->Add (Vector(280.7048418592408,366.9615681424383, 0.0));
    positionAlloc4->Add (Vector(1.2537798339615769,198.61388311688088, 0.0));
    positionAlloc4->Add (Vector(408.6911351985628,453.8223230499195, 0.0));
    positionAlloc4->Add (Vector(353.59380489209116,295.4502997452034, 0.0));
    positionAlloc4->Add (Vector(450.22445619303267,435.9928155954592, 0.0));
    positionAlloc4->Add (Vector(140.09631768654685,225.6809306162008, 0.0));
    positionAlloc4->Add (Vector(324.4911113243237,376.6629029471962, 0.0));
    positionAlloc4->Add (Vector(198.49410583618698,80.78701992804494, 0.0));
    positionAlloc4->Add (Vector(240.7229487703697,401.7973132155136, 0.0));
    positionAlloc4->Add (Vector(366.683047234417,96.2089452068694, 0.0));
    positionAlloc4->Add (Vector(413.6001625696531,182.26172377794413, 0.0));
    positionAlloc4->Add (Vector(316.7304171178628,138.8563662367121, 0.0));
    positionAlloc4->Add (Vector(440.5841592144523,63.039240198599245, 0.0));
    positionAlloc4->Add (Vector(153.69074021392214,152.4828208210316, 0.0));
    positionAlloc4->Add (Vector(462.86026766813,390.59770033788305, 0.0));
    positionAlloc4->Add (Vector(340.55573522920486,118.49103223584918, 0.0));
    positionAlloc4->Add (Vector(359.66938361053775,428.65016672561535, 0.0));
    positionAlloc4->Add (Vector(209.97412720096455,301.393381121148, 0.0));
    positionAlloc4->Add (Vector(139.37920434671852,157.32380338755593, 0.0));
    positionAlloc4->Add (Vector(400.320587141728,332.38844925537745, 0.0));
    positionAlloc4->Add (Vector(66.45311883091354,141.73121621402856, 0.0));
    positionAlloc4->Add (Vector(235.90953136879838,490.91220891262867, 0.0));
    positionAlloc4->Add (Vector(126.84290344384009,93.07783486833199, 0.0));
    positionAlloc4->Add (Vector(263.1148324458152,250.30978646154455, 0.0));
    positionAlloc4->Add (Vector(113.0476012162745,422.4940686791726, 0.0));
    positionAlloc4->Add (Vector(169.77043439815176,352.24859912406305, 0.0));
    positionAlloc4->Add (Vector(95.04352523997028,234.56721991668854, 0.0));
    positionAlloc4->Add (Vector(336.21038629321623,258.89336190670724, 0.0));
    positionAlloc4->Add (Vector(135.97805622638975,150.50397153995314, 0.0));
    positionAlloc4->Add (Vector(68.6030017449557,212.14618716290602, 0.0));
    positionAlloc4->Add (Vector(51.37828193708216,386.290133315772, 0.0));
    positionAlloc4->Add (Vector(236.48247593000653,211.42090651930656, 0.0));
    positionAlloc4->Add (Vector(188.60864380575327,212.21873311980767, 0.0));
    positionAlloc4->Add (Vector(206.79585890088214,69.65058566186339, 0.0));
    positionAlloc4->Add (Vector(338.17831772563926,115.8515707323598, 0.0));
    positionAlloc4->Add (Vector(111.8225227636589,16.378599872988563, 0.0));
    positionAlloc4->Add (Vector(425.7194534734954,52.31795587247928, 0.0));
    positionAlloc4->Add (Vector(19.867843287925623,73.78441884997378, 0.0));
    positionAlloc4->Add (Vector(114.12455230769297,87.25059378637201, 0.0));
    positionAlloc4->Add (Vector(269.74544546327485,374.6403935635959, 0.0));
    positionAlloc4->Add (Vector(371.6952562010195,118.60702882177382, 0.0));
    positionAlloc4->Add (Vector(120.08686691553334,93.24634413634264, 0.0));
    positionAlloc4->Add (Vector(426.98620747108794,297.9149710293508, 0.0));
    positionAlloc4->Add (Vector(247.74749068059648,412.22980539884173, 0.0));
    positionAlloc4->Add (Vector(161.17874456635533,124.2121231650995, 0.0));
    positionAlloc4->Add (Vector(36.25776402273168,86.35054406613746, 0.0));
    positionAlloc4->Add (Vector(370.62586766592955,474.22920215289685, 0.0));
    positionAlloc4->Add (Vector(210.18478440139253,213.0653902123561, 0.0));
    positionAlloc4->Add (Vector(14.764372471010478,451.66558840843777, 0.0));
    positionAlloc4->Add (Vector(313.98096349095135,152.00274501115157, 0.0));
    positionAlloc4->Add (Vector(258.56026598521333,116.05001092316752, 0.0));
    positionAlloc4->Add (Vector(320.3913522259231,400.6653516015963, 0.0));
    positionAlloc4->Add (Vector(142.46607286107238,384.42642414387893, 0.0));
    positionAlloc4->Add (Vector(149.55617233041778,52.692336365019635, 0.0));
    positionAlloc4->Add (Vector(339.6064750978593,256.4375712949957, 0.0));
    positionAlloc4->Add (Vector(208.05038630907103,265.7917329479509, 0.0));
    positionAlloc4->Add (Vector(370.9821178391901,491.4403929604953, 0.0));
    positionAlloc4->Add (Vector(479.094370857448,341.58542192707324, 0.0));
    positionAlloc4->Add (Vector(436.5417486372178,64.11546942274316, 0.0));
    positionAlloc4->Add (Vector(136.2004250646911,299.3944589546052, 0.0));
    positionAlloc4->Add (Vector(33.54473016442766,99.32413221028075, 0.0));
    positionAlloc4->Add (Vector(212.2799551511083,102.06124999716792, 0.0));
    positionAlloc4->Add (Vector(384.86267247859286,401.43324321621196, 0.0));
    positionAlloc4->Add (Vector(254.9491890715545,171.8940517184222, 0.0));
    positionAlloc4->Add (Vector(481.03910642354674,92.24780224508883, 0.0));
    positionAlloc4->Add (Vector(148.0649617479522,266.4612795158266, 0.0));
    positionAlloc4->Add (Vector(304.28271848677934,125.82979207616185, 0.0));
    positionAlloc4->Add (Vector(288.48892323875083,437.6943692253435, 0.0));
    positionAlloc4->Add (Vector(15.498979914127908,473.3641348039762, 0.0));
    positionAlloc4->Add (Vector(92.06377808789917,414.3664257641509, 0.0));
    positionAlloc4->Add (Vector(398.2444335655978,223.75406949515693, 0.0));
    positionAlloc4->Add (Vector(280.8931303049868,368.35547884676, 0.0));
    positionAlloc4->Add (Vector(432.0720473820134,223.9494578101399, 0.0));
    positionAlloc4->Add (Vector(127.1161773286762,42.04317708996474, 0.0));
    positionAlloc4->Add (Vector(380.8649313463709,120.2421624015439, 0.0));
    positionAlloc4->Add (Vector(196.03293446519342,141.93375798670132, 0.0));
    positionAlloc4->Add (Vector(471.75469195554035,432.84300700316703, 0.0));
    positionAlloc4->Add (Vector(75.53725923571596,261.5042658672321, 0.0));
    positionAlloc4->Add (Vector(482.11216242464707,254.56970125818384, 0.0));
    positionAlloc4->Add (Vector(22.155827916552294,116.8671800582553, 0.0));
    positionAlloc4->Add (Vector(87.66629588566916,170.57341162577362, 0.0));
    positionAlloc4->Add (Vector(65.12108378638254,327.60064361236783, 0.0));
    positionAlloc4->Add (Vector(25.982844252680493,67.27321050597112, 0.0));
    positionAlloc4->Add (Vector(159.64886853730943,340.60209205850947, 0.0));
    positionAlloc4->Add (Vector(303.27217412523385,301.97387053984073, 0.0));
    positionAlloc4->Add (Vector(285.5043809685365,482.0180666695409, 0.0));
    positionAlloc4->Add (Vector(408.37698218838335,438.9007093173498, 0.0));
    positionAlloc4->Add (Vector(419.9203550988337,435.0511826537838, 0.0));
    positionAlloc4->Add (Vector(238.7189777961085,233.72994602268471, 0.0));
    positionAlloc4->Add (Vector(209.24840876958783,270.89953639136564, 0.0));
    positionAlloc4->Add (Vector(102.46107289031691,130.71035720291897, 0.0));
    positionAlloc4->Add (Vector(413.38436920256447,323.27496787557595, 0.0));
    positionAlloc4->Add (Vector(66.19074678136872,399.3101541390155, 0.0));
    positionAlloc4->Add (Vector(340.6798774456531,433.56964999954425, 0.0));
    positionAlloc4->Add (Vector(61.1306151150014,334.1444943754009, 0.0));
    positionAlloc4->Add (Vector(266.09077426083735,142.3731043065657, 0.0));
    positionAlloc4->Add (Vector(231.7785454758559,167.3756718568319, 0.0));
    positionAlloc4->Add (Vector(321.73126112040245,22.74910802030189, 0.0));
    positionAlloc4->Add (Vector(309.69608284837983,490.3495181778901, 0.0));
    positionAlloc4->Add (Vector(467.7100570035309,176.339663625869, 0.0));
    positionAlloc4->Add (Vector(424.68555036532746,225.1898212358601, 0.0));
    positionAlloc4->Add (Vector(109.57690287862903,214.40034645704438, 0.0));
    positionAlloc4->Add (Vector(400.06984219349386,134.46802889253246, 0.0));
    positionAlloc4->Add (Vector(453.13019798084093,403.55495083305306, 0.0));
    positionAlloc4->Add (Vector(491.23905925102196,86.03866591444798, 0.0));
    positionAlloc4->Add (Vector(341.3490214516257,6.709155278264767, 0.0));
    positionAlloc4->Add (Vector(351.99974913994424,235.14946239108764, 0.0));
    positionAlloc4->Add (Vector(470.9431230933526,434.9553866341772, 0.0));
    positionAlloc4->Add (Vector(212.7017327878188,405.360616803685, 0.0));
    positionAlloc4->Add (Vector(212.86220599255813,301.0039192360007, 0.0));
    positionAlloc4->Add (Vector(179.55966946205538,37.65543631790891, 0.0));
    positionAlloc4->Add (Vector(279.3382055318158,423.8605070180053, 0.0));
    positionAlloc4->Add (Vector(75.613963685074,26.24548192656695, 0.0));
    positionAlloc4->Add (Vector(96.77947886190525,56.98288072959318, 0.0));
    positionAlloc4->Add (Vector(286.9427634461737,220.1596037927966, 0.0));
    positionAlloc4->Add (Vector(231.07904834567856,256.3248733785588, 0.0));
    positionAlloc4->Add (Vector(422.4059095031017,297.6348903366281, 0.0));
    positionAlloc4->Add (Vector(94.28880174973392,220.43609991114545, 0.0));
    positionAlloc4->Add (Vector(194.80600793965257,452.6013829665202, 0.0));
    positionAlloc4->Add (Vector(40.608292971366645,499.20677929545377, 0.0));
    positionAlloc4->Add (Vector(58.68846015656487,383.7117588707266, 0.0));
    positionAlloc4->Add (Vector(7.284365881665867,107.60479329721379, 0.0));
    positionAlloc4->Add (Vector(403.6826319386602,484.7690060499482, 0.0));
    positionAlloc4->Add (Vector(473.0275149237816,100.09303286966103, 0.0));
    positionAlloc4->Add (Vector(206.83508210068013,342.15481091565994, 0.0));
    positionAlloc4->Add (Vector(300.71670484939403,362.9794229244075, 0.0));
    positionAlloc4->Add (Vector(233.13323708328676,447.8209012997233, 0.0));
    positionAlloc4->Add (Vector(490.16644256866385,111.19159676558787, 0.0));
    positionAlloc4->Add (Vector(394.02477558615357,301.43266876919864, 0.0));
    positionAlloc4->Add (Vector(329.28402077918093,90.59769097693082, 0.0));
    positionAlloc4->Add (Vector(226.6887132610992,262.8557956707048, 0.0));
    positionAlloc4->Add (Vector(233.77502653266978,333.4712715969876, 0.0));
    positionAlloc4->Add (Vector(383.431188163271,317.1403763869367, 0.0));
    positionAlloc4->Add (Vector(233.1544946046707,245.50426456646463, 0.0));
    positionAlloc4->Add (Vector(309.0056621733223,115.49497433485195, 0.0));
    positionAlloc4->Add (Vector(77.98761652171122,269.8395975806372, 0.0));
    positionAlloc4->Add (Vector(98.47401945700834,490.5398786201486, 0.0));
    positionAlloc4->Add (Vector(186.24938320267975,309.5198173627354, 0.0));
    positionAlloc4->Add (Vector(461.3193473443356,22.077398015931227, 0.0));
    positionAlloc4->Add (Vector(18.7919257428516,73.75125269846772, 0.0));
    positionAlloc4->Add (Vector(410.1012331576749,249.67055064891724, 0.0));
    positionAlloc4->Add (Vector(75.34499612413559,422.75883776535323, 0.0));
    positionAlloc4->Add (Vector(282.86607331463165,454.40401615002844, 0.0));
    positionAlloc4->Add (Vector(351.4658660571857,92.46751118670082, 0.0));
    positionAlloc4->Add (Vector(203.97499452368373,437.3738099521817, 0.0));
    positionAlloc4->Add (Vector(309.8409960269314,225.55123106038744, 0.0));
    positionAlloc4->Add (Vector(406.64692266252786,403.756135821516, 0.0));
    positionAlloc4->Add (Vector(262.0928072843306,328.76070385155504, 0.0));
    positionAlloc4->Add (Vector(324.73340397737314,357.91347642909, 0.0));
    positionAlloc4->Add (Vector(290.4920363375098,126.4900563600942, 0.0));
    positionAlloc4->Add (Vector(277.6126592656722,47.47803490124575, 0.0));
    positionAlloc4->Add (Vector(385.06609379990374,52.753539998667385, 0.0));
    positionAlloc4->Add (Vector(311.1562643458089,115.84734569742994, 0.0));
    positionAlloc4->Add (Vector(121.47356288540057,456.9219376233041, 0.0));
    positionAlloc4->Add (Vector(271.8937458319126,313.4580083401673, 0.0));
    positionAlloc4->Add (Vector(361.6805221971531,54.497516964821855, 0.0));
    positionAlloc4->Add (Vector(480.198574110009,1.7773802322130217, 0.0));
    positionAlloc4->Add (Vector(125.60159167469331,327.88038395168303, 0.0));
    positionAlloc4->Add (Vector(197.82413613377443,385.8592412077292, 0.0));
    positionAlloc4->Add (Vector(275.289779503931,307.7385355573289, 0.0));
    positionAlloc4->Add (Vector(292.34580650226286,266.85387731713814, 0.0));
    positionAlloc4->Add (Vector(230.89970509953073,182.63723060354687, 0.0));
    positionAlloc4->Add (Vector(368.99774820192044,362.04242243203703, 0.0));
    positionAlloc4->Add (Vector(117.76097830493465,147.96752594370565, 0.0));
    positionAlloc4->Add (Vector(5.794794704947592,189.88221651699737, 0.0));
    positionAlloc4->Add (Vector(404.35767409264486,450.1700022553309, 0.0));
    positionAlloc4->Add (Vector(166.80511442394885,477.10025371479446, 0.0));
    positionAlloc4->Add (Vector(488.9401966799829,125.38657833650113, 0.0));
    positionAlloc4->Add (Vector(385.41591527691514,210.3565995125959, 0.0));
    positionAlloc4->Add (Vector(268.00128741090606,450.391557353458, 0.0));
    positionAlloc4->Add (Vector(404.2056783030709,65.34641219869403, 0.0));
    positionAlloc4->Add (Vector(433.81360480758684,424.9359407201662, 0.0));
    positionAlloc4->Add (Vector(418.3760249269156,276.02794061238137, 0.0));
    positionAlloc4->Add (Vector(175.81245318022133,459.01415954309, 0.0));
    positionAlloc4->Add (Vector(319.59237793412296,374.26016004487684, 0.0));
    positionAlloc4->Add (Vector(437.70193113966087,474.63221688614505, 0.0));
    positionAlloc4->Add (Vector(238.9619246082797,277.2592491390835, 0.0));
    positionAlloc4->Add (Vector(467.6418850034003,431.7964774590513, 0.0));
    positionAlloc4->Add (Vector(208.28336450737373,241.07696107873372, 0.0));
    positionAlloc4->Add (Vector(329.51782983294163,189.41558433086593, 0.0));
    positionAlloc4->Add (Vector(469.3595866844655,423.81335461969377, 0.0));
    positionAlloc4->Add (Vector(359.4697440449218,54.98798622266021, 0.0));
    positionAlloc4->Add (Vector(326.9200995981275,221.8848963623612, 0.0));
    positionAlloc4->Add (Vector(21.910754863573466,356.58198657990727, 0.0));
    positionAlloc4->Add (Vector(99.09961466009592,110.29704092366788, 0.0));
    positionAlloc4->Add (Vector(263.81075422817463,176.62120299152951, 0.0));
    positionAlloc4->Add (Vector(55.39771984797809,323.7764929082136, 0.0));
    positionAlloc4->Add (Vector(197.67523689734517,477.6949946285873, 0.0));
    positionAlloc4->Add (Vector(24.698192316657153,75.22751418383173, 0.0));
    positionAlloc4->Add (Vector(34.892989252104265,20.93878514999814, 0.0));
    positionAlloc4->Add (Vector(150.31614426187406,409.6791923280101, 0.0));
    positionAlloc4->Add (Vector(198.23134502631194,314.01161587761413, 0.0));
    positionAlloc4->Add (Vector(140.47776849518374,301.0733135682726, 0.0));
    positionAlloc4->Add (Vector(275.9975135770986,481.95794434227554, 0.0));
    positionAlloc4->Add (Vector(460.02270270485684,463.42060895112076, 0.0));
    positionAlloc4->Add (Vector(361.5466998330049,25.555451913058135, 0.0));
    positionAlloc4->Add (Vector(367.29250702518533,320.4038120229373, 0.0));
    positionAlloc4->Add (Vector(494.8534768933709,232.4438154559112, 0.0));
    positionAlloc4->Add (Vector(147.40600589924767,20.78711697458363, 0.0));
    positionAlloc4->Add (Vector(498.9421391124512,216.8448697024885, 0.0));
    positionAlloc4->Add (Vector(431.4194902893078,176.26628440253089, 0.0));
    positionAlloc4->Add (Vector(451.4584167372045,267.69044586526087, 0.0));
    positionAlloc4->Add (Vector(145.40307751633895,457.9578950049785, 0.0));
    positionAlloc4->Add (Vector(401.6040971640042,126.9669279388676, 0.0));
    positionAlloc4->Add (Vector(309.6066489666195,88.98616773508472, 0.0));
    positionAlloc4->Add (Vector(195.45935575072903,107.2324355666599, 0.0));
    positionAlloc4->Add (Vector(466.38195398242914,465.427483879984, 0.0));
    positionAlloc4->Add (Vector(493.1233912181475,14.12131008145634, 0.0));
    positionAlloc4->Add (Vector(257.6968465715607,199.97296199256587, 0.0));
    positionAlloc4->Add (Vector(266.8245380418655,369.693893673191, 0.0));
    positionAlloc4->Add (Vector(41.59494434849886,388.4920960136902, 0.0));
    positionAlloc4->Add (Vector(399.8304187624194,251.1975029491292, 0.0));
    positionAlloc4->Add (Vector(124.73110335069387,109.63395516253038, 0.0));
    positionAlloc4->Add (Vector(129.60906282536848,163.41534243318756, 0.0));
    positionAlloc4->Add (Vector(282.77711769019055,246.2120236991598, 0.0));
    positionAlloc4->Add (Vector(16.321932981296193,362.58332941635416, 0.0));
    positionAlloc4->Add (Vector(309.55981662583025,492.7224574724153, 0.0));
    positionAlloc4->Add (Vector(128.94649221351995,467.2227462647923, 0.0));
    positionAlloc4->Add (Vector(229.27540545125052,465.3221960827242, 0.0));
    positionAlloc4->Add (Vector(405.3145517234508,429.9992957901724, 0.0));
    positionAlloc4->Add (Vector(155.64804122808357,399.4864201807965, 0.0));
    positionAlloc4->Add (Vector(21.725770857488392,474.2863841880038, 0.0));
    positionAlloc4->Add (Vector(302.71100386352407,488.1191322801942, 0.0));
    positionAlloc4->Add (Vector(439.5332006792327,486.7594304181468, 0.0));
    positionAlloc4->Add (Vector(19.70575609222214,120.77920867043201, 0.0));
    positionAlloc4->Add (Vector(63.215445144842505,389.1504450338138, 0.0));
    positionAlloc4->Add (Vector(52.631263331393455,86.36373166144001, 0.0));
    positionAlloc4->Add (Vector(45.2048578473993,64.48799207113804, 0.0));
    positionAlloc4->Add (Vector(451.46795106085494,293.71000961878843, 0.0));
    positionAlloc4->Add (Vector(195.13235229979048,379.9126983580487, 0.0));
    positionAlloc4->Add (Vector(355.36978494979314,371.1917148535508, 0.0));
    positionAlloc4->Add (Vector(120.04757708060504,315.8103726250801, 0.0));
    positionAlloc4->Add (Vector(105.8606674242431,415.07826308114966, 0.0));
    positionAlloc4->Add (Vector(375.226773361662,375.54490366140305, 0.0));
    positionAlloc4->Add (Vector(107.23389041534631,469.7094354530797, 0.0));
    positionAlloc4->Add (Vector(471.24943711310374,306.77501328006775, 0.0));
    positionAlloc4->Add (Vector(147.41043976848184,455.8815868488994, 0.0));
    positionAlloc4->Add (Vector(325.5038046542476,185.57359223136476, 0.0));
    positionAlloc4->Add (Vector(312.71768915571647,129.06482464093548, 0.0));
    positionAlloc4->Add (Vector(356.96401613348064,165.05777609953714, 0.0));
    positionAlloc4->Add (Vector(493.33872248404134,33.28540156618964, 0.0));
    positionAlloc4->Add (Vector(124.05573203008741,422.8074811155727, 0.0));
    positionAlloc4->Add (Vector(115.7314677671239,213.4106747241178, 0.0));
    positionAlloc4->Add (Vector(325.1990662420599,297.1454267933322, 0.0));
    positionAlloc4->Add (Vector(164.57300089714988,264.0006983188131, 0.0));
    positionAlloc4->Add (Vector(352.96368338235953,447.4697600764621, 0.0));
    positionAlloc4->Add (Vector(101.2315178372678,427.11898645819133, 0.0));
    positionAlloc4->Add (Vector(276.71685312413285,42.40091083017644, 0.0));
    positionAlloc4->Add (Vector(239.8687835245775,473.3383396198238, 0.0));
    positionAlloc4->Add (Vector(59.48666858650742,450.3711075358665, 0.0));
    positionAlloc4->Add (Vector(395.9319377923358,346.2072310835378, 0.0));
    positionAlloc4->Add (Vector(83.8683425251358,254.24100671480798, 0.0));
    positionAlloc4->Add (Vector(341.0331292164074,118.23482566307769, 0.0));
    positionAlloc4->Add (Vector(155.50653394242076,274.244544833052, 0.0));
    positionAlloc4->Add (Vector(77.03958591617871,447.54340432432423, 0.0));
    positionAlloc4->Add (Vector(199.3577718145042,483.306246321071, 0.0));
    positionAlloc4->Add (Vector(330.22886380554985,289.0869592042691, 0.0));
    positionAlloc4->Add (Vector(423.0272996182347,463.7904712987717, 0.0));
    positionAlloc4->Add (Vector(303.7209467572768,325.41562833784855, 0.0));
    positionAlloc4->Add (Vector(155.80695673035234,161.73959849453766, 0.0));
    positionAlloc4->Add (Vector(324.950576544845,18.48753417967891, 0.0));
    positionAlloc4->Add (Vector(208.81617340709164,84.60709234684526, 0.0));
    positionAlloc4->Add (Vector(490.70523287705623,304.7509628867483, 0.0));
    positionAlloc4->Add (Vector(13.855870061182497,42.695069561585306, 0.0));
    positionAlloc4->Add (Vector(182.18273992851397,446.1369646380862, 0.0));
    positionAlloc4->Add (Vector(91.86970593800086,428.0796189936859, 0.0));
    positionAlloc4->Add (Vector(328.52016451236466,421.66160695976123, 0.0));
    positionAlloc4->Add (Vector(0.5164663288673155,238.04455900105242, 0.0));
    positionAlloc4->Add (Vector(383.09418260141086,470.50663340945067, 0.0));
    positionAlloc4->Add (Vector(439.33048815790556,127.30037783028847, 0.0));
    positionAlloc4->Add (Vector(302.94818060892896,297.4235667039588, 0.0));
    positionAlloc4->Add (Vector(143.99343963737576,157.2086111953731, 0.0));
    positionAlloc4->Add (Vector(203.14421384415755,371.87459817560983, 0.0));
    positionAlloc4->Add (Vector(480.7862712172669,365.0412788701301, 0.0));
    positionAlloc4->Add (Vector(447.8645542643521,474.9231944400437, 0.0));
    positionAlloc4->Add (Vector(336.07220482842433,94.79370009579286, 0.0));
    positionAlloc4->Add (Vector(43.6262619359909,185.53197250809455, 0.0));
    positionAlloc4->Add (Vector(312.5605341527448,204.86563056701024, 0.0));
    positionAlloc4->Add (Vector(53.90695039729621,42.141525718278395, 0.0));
    positionAlloc4->Add (Vector(284.0762714745525,333.0940451117476, 0.0));
    positionAlloc4->Add (Vector(112.24673941797886,382.59197836675406, 0.0));
    positionAlloc4->Add (Vector(41.76710353087054,309.5674157099536, 0.0));
    positionAlloc4->Add (Vector(477.63112676909344,40.376953906039404, 0.0));
    positionAlloc4->Add (Vector(129.89584665220727,290.61767114558586, 0.0));
    positionAlloc4->Add (Vector(484.935285172357,370.9697627280384, 0.0));
    positionAlloc4->Add (Vector(140.92254407162147,89.24741648874412, 0.0));
    positionAlloc4->Add (Vector(252.88969339493323,412.429281192616, 0.0));
    positionAlloc4->Add (Vector(260.1568179316582,414.23407404273564, 0.0));
    positionAlloc4->Add (Vector(345.9919522621094,341.4534338149374, 0.0));
    positionAlloc4->Add (Vector(278.9126518574552,253.06556441411738, 0.0));
    positionAlloc4->Add (Vector(448.74914115141013,226.91497524500613, 0.0));
    positionAlloc4->Add (Vector(334.82329993776654,403.88943115753824, 0.0));
    positionAlloc4->Add (Vector(402.992466144833,434.24405815138607, 0.0));
    positionAlloc4->Add (Vector(13.57941466293111,400.42967756501656, 0.0));
    positionAlloc4->Add (Vector(430.2283336684426,169.225692059125, 0.0));
    positionAlloc4->Add (Vector(278.63411503745687,453.55452761018535, 0.0));
    positionAlloc4->Add (Vector(60.20043842886169,5.349501572087078, 0.0));
    positionAlloc4->Add (Vector(44.09432278575503,341.8551679108189, 0.0));
    positionAlloc4->Add (Vector(43.96934188530166,434.98809013580376, 0.0));
    positionAlloc4->Add (Vector(146.15688101964187,97.96597324949607, 0.0));
    positionAlloc4->Add (Vector(72.89271560738852,122.4403193809418, 0.0));
    positionAlloc4->Add (Vector(160.82407280138884,390.1011685598793, 0.0));
    positionAlloc4->Add (Vector(337.2494220811287,275.7344179063883, 0.0));
    positionAlloc4->Add (Vector(132.7560087361929,7.721815971380441, 0.0));
    positionAlloc4->Add (Vector(79.01591748190634,7.391043371046457, 0.0));
    positionAlloc4->Add (Vector(234.0247774332364,314.92651079171134, 0.0));
    positionAlloc4->Add (Vector(424.9652676157095,217.32739703583675, 0.0));
    positionAlloc4->Add (Vector(144.630247741455,473.88203305080344, 0.0));
    positionAlloc4->Add (Vector(467.57655777119436,444.271051678114, 0.0));
    positionAlloc4->Add (Vector(28.987906686383514,69.40825902283682, 0.0));
    positionAlloc4->Add (Vector(410.72337011314863,115.49427964081666, 0.0));
    positionAlloc4->Add (Vector(494.94172666855775,88.71476843413095, 0.0));
    positionAlloc4->Add (Vector(120.08610361068861,343.22170758618205, 0.0));
    positionAlloc4->Add (Vector(128.33049497648813,195.7151814921343, 0.0));
    positionAlloc4->Add (Vector(232.56697175903872,403.3734053874318, 0.0));
    positionAlloc4->Add (Vector(29.678612669478742,246.36130058301663, 0.0));
    positionAlloc4->Add (Vector(330.9441839183988,216.91991357636297, 0.0));
    positionAlloc4->Add (Vector(71.59489085438142,316.1649373332483, 0.0));
    positionAlloc4->Add (Vector(242.1578865161928,483.84762298059877, 0.0));
    positionAlloc4->Add (Vector(481.9588837119989,322.4708362561059, 0.0));
    positionAlloc4->Add (Vector(473.68795897697964,246.91232874554282, 0.0));
    positionAlloc4->Add (Vector(221.05055531338292,92.6588229688422, 0.0));
    positionAlloc4->Add (Vector(275.7157868366862,92.64461968427618, 0.0));
    positionAlloc4->Add (Vector(141.77225680182727,137.31712421193149, 0.0));
    positionAlloc4->Add (Vector(466.5778814116128,467.87660384606653, 0.0));
    positionAlloc4->Add (Vector(469.0142715781762,92.69026232390115, 0.0));
    positionAlloc4->Add (Vector(468.5401923994821,402.4179230078973, 0.0));
    positionAlloc4->Add (Vector(143.52197307775432,66.60355687555098, 0.0));
    positionAlloc4->Add (Vector(316.6046329032388,176.6971735008147, 0.0));
    positionAlloc4->Add (Vector(351.26151698070043,248.06197562493054, 0.0));
    positionAlloc4->Add (Vector(295.27326112178355,190.26703860482002, 0.0));
    positionAlloc4->Add (Vector(147.3460207804521,134.82735057952573, 0.0));
    positionAlloc4->Add (Vector(139.164065423451,319.98728455445615, 0.0));
    positionAlloc4->Add (Vector(245.09765612211586,22.741072835147037, 0.0));
    positionAlloc4->Add (Vector(23.768797113880247,5.086228130887827, 0.0));
    positionAlloc4->Add (Vector(126.42074103840756,111.92483516376267, 0.0));
    positionAlloc4->Add (Vector(492.0632719424584,4.9364876794592565, 0.0));
    positionAlloc4->Add (Vector(258.0992305385345,431.6132408569248, 0.0));
    positionAlloc4->Add (Vector(314.80397835945655,17.215632653547807, 0.0));
    positionAlloc4->Add (Vector(436.66156701836604,122.31703285960404, 0.0));
    positionAlloc4->Add (Vector(345.5136707783521,286.9014345826765, 0.0));
    positionAlloc4->Add (Vector(82.65924524011992,98.59817323333463, 0.0));
    positionAlloc4->Add (Vector(222.17407872306362,41.88747692273026, 0.0));
    positionAlloc4->Add (Vector(172.5579573900406,79.95939429464161, 0.0));
    positionAlloc4->Add (Vector(315.99747523156856,105.31408920940122, 0.0));
    positionAlloc4->Add (Vector(78.07478222039738,439.8610717506904, 0.0));
    positionAlloc4->Add (Vector(445.27614318915556,461.25224863019673, 0.0));
    positionAlloc4->Add (Vector(142.3119844843677,453.69807112049864, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_2.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_ECC_flowmon/switch_ECC_2.flowmon", false, false);
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