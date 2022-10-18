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
	  uint16_t numberOfRrhs = 32;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 488;
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
    positionAlloc->Add (Vector (444.44444444444446,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,0.0, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,0.0, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,0.0, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,55.55555555555556, 0.0));
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
    positionAlloc4->Add (Vector(280.8635554586862,91.4439140889054, 0.0));
    positionAlloc4->Add (Vector(364.4462932161895,118.93935424124874, 0.0));
    positionAlloc4->Add (Vector(180.17576406236185,114.74187110802048, 0.0));
    positionAlloc4->Add (Vector(132.35164114967412,250.6040433240645, 0.0));
    positionAlloc4->Add (Vector(356.02977362394904,463.7640105100766, 0.0));
    positionAlloc4->Add (Vector(154.51743254472632,74.47706815639471, 0.0));
    positionAlloc4->Add (Vector(445.2354396271714,85.59627333076669, 0.0));
    positionAlloc4->Add (Vector(417.22761249948235,192.16249930847258, 0.0));
    positionAlloc4->Add (Vector(435.7493151410859,276.2339842609964, 0.0));
    positionAlloc4->Add (Vector(335.0042788307037,310.9768526430636, 0.0));
    positionAlloc4->Add (Vector(202.0132484666286,290.7282457602333, 0.0));
    positionAlloc4->Add (Vector(421.74797177094115,20.84034661336154, 0.0));
    positionAlloc4->Add (Vector(421.42847771610394,340.171894340347, 0.0));
    positionAlloc4->Add (Vector(67.79640424705113,497.7352222379013, 0.0));
    positionAlloc4->Add (Vector(464.9241481401706,75.41154492959934, 0.0));
    positionAlloc4->Add (Vector(459.56092242451945,84.55522204963422, 0.0));
    positionAlloc4->Add (Vector(424.13382016097694,352.5910356536911, 0.0));
    positionAlloc4->Add (Vector(393.89532730245315,102.14850888817617, 0.0));
    positionAlloc4->Add (Vector(491.4113459463761,197.34072614856257, 0.0));
    positionAlloc4->Add (Vector(157.19797601618157,394.83937630018386, 0.0));
    positionAlloc4->Add (Vector(84.8521679305988,140.04003784763592, 0.0));
    positionAlloc4->Add (Vector(167.69222370725123,38.87970578562883, 0.0));
    positionAlloc4->Add (Vector(447.0726616538352,135.36847102061756, 0.0));
    positionAlloc4->Add (Vector(155.0600533411689,445.45668479207853, 0.0));
    positionAlloc4->Add (Vector(382.79258672985316,45.276489654698274, 0.0));
    positionAlloc4->Add (Vector(62.224139276948584,170.34901589340728, 0.0));
    positionAlloc4->Add (Vector(466.61127253884655,133.73033349017237, 0.0));
    positionAlloc4->Add (Vector(432.5405499303398,16.426731991310838, 0.0));
    positionAlloc4->Add (Vector(101.19138391255188,243.51141663035997, 0.0));
    positionAlloc4->Add (Vector(85.98252394495698,329.10249046436036, 0.0));
    positionAlloc4->Add (Vector(279.1530532213062,21.745066795998135, 0.0));
    positionAlloc4->Add (Vector(57.18821912123268,66.92629903920266, 0.0));
    positionAlloc4->Add (Vector(159.66105707826455,66.80103047292896, 0.0));
    positionAlloc4->Add (Vector(247.264659220911,348.8672476749569, 0.0));
    positionAlloc4->Add (Vector(348.924683765582,56.631675513328375, 0.0));
    positionAlloc4->Add (Vector(367.5687781718176,283.6956892236852, 0.0));
    positionAlloc4->Add (Vector(139.25081481082668,430.2365426216201, 0.0));
    positionAlloc4->Add (Vector(161.55435005052905,453.1490261163481, 0.0));
    positionAlloc4->Add (Vector(200.12465168839304,197.95867558048198, 0.0));
    positionAlloc4->Add (Vector(258.68961555200804,412.7402073313913, 0.0));
    positionAlloc4->Add (Vector(189.96901540563888,85.47535258070582, 0.0));
    positionAlloc4->Add (Vector(167.10316778899931,477.6237701785956, 0.0));
    positionAlloc4->Add (Vector(34.4690833983487,42.21760608059372, 0.0));
    positionAlloc4->Add (Vector(152.84370583688033,412.00800527116877, 0.0));
    positionAlloc4->Add (Vector(335.35267064148337,349.1583394339613, 0.0));
    positionAlloc4->Add (Vector(356.72734416720704,158.70013151355244, 0.0));
    positionAlloc4->Add (Vector(273.5969597064892,324.7981373372561, 0.0));
    positionAlloc4->Add (Vector(375.8935886180122,334.4621880862429, 0.0));
    positionAlloc4->Add (Vector(385.7003175640742,385.92433335086946, 0.0));
    positionAlloc4->Add (Vector(107.21360759997806,313.70261284164496, 0.0));
    positionAlloc4->Add (Vector(132.2546777687766,361.8234239107955, 0.0));
    positionAlloc4->Add (Vector(376.3210014512984,416.9019148757864, 0.0));
    positionAlloc4->Add (Vector(34.631365753619306,33.32445502395981, 0.0));
    positionAlloc4->Add (Vector(73.67178977035482,2.865102927565588, 0.0));
    positionAlloc4->Add (Vector(198.79910701813324,126.58449665825039, 0.0));
    positionAlloc4->Add (Vector(435.71850476976937,13.11061056293028, 0.0));
    positionAlloc4->Add (Vector(493.0720609114326,282.1849427370833, 0.0));
    positionAlloc4->Add (Vector(432.033855323641,387.5107537087613, 0.0));
    positionAlloc4->Add (Vector(489.66281646802287,13.198137066193327, 0.0));
    positionAlloc4->Add (Vector(11.68806222399077,173.36788728844493, 0.0));
    positionAlloc4->Add (Vector(181.09001624095455,447.58087457169535, 0.0));
    positionAlloc4->Add (Vector(92.40474921195263,233.65883199808363, 0.0));
    positionAlloc4->Add (Vector(115.32591080987076,473.1416102432957, 0.0));
    positionAlloc4->Add (Vector(71.89799292844418,361.7939617333733, 0.0));
    positionAlloc4->Add (Vector(267.3098046060731,488.64728891219255, 0.0));
    positionAlloc4->Add (Vector(153.09175765304832,14.201027920861865, 0.0));
    positionAlloc4->Add (Vector(262.6300185506866,124.68841201022978, 0.0));
    positionAlloc4->Add (Vector(171.6337324338017,220.06496448955227, 0.0));
    positionAlloc4->Add (Vector(332.8411246164073,367.235161417364, 0.0));
    positionAlloc4->Add (Vector(286.045240300887,308.3869016459667, 0.0));
    positionAlloc4->Add (Vector(210.69715028796338,286.2141011166424, 0.0));
    positionAlloc4->Add (Vector(312.7965225952929,230.3216634866012, 0.0));
    positionAlloc4->Add (Vector(28.31065681003403,248.50091926800255, 0.0));
    positionAlloc4->Add (Vector(411.35790458766974,219.58885141702078, 0.0));
    positionAlloc4->Add (Vector(2.502770869600879,132.3088328405152, 0.0));
    positionAlloc4->Add (Vector(295.16153854763877,295.921144997435, 0.0));
    positionAlloc4->Add (Vector(327.3216689250465,171.29903901494563, 0.0));
    positionAlloc4->Add (Vector(228.68199693270446,436.5865387229324, 0.0));
    positionAlloc4->Add (Vector(218.67260686663715,135.91189203963395, 0.0));
    positionAlloc4->Add (Vector(354.2938187985397,336.0523551019102, 0.0));
    positionAlloc4->Add (Vector(462.32187742914704,433.7615515317213, 0.0));
    positionAlloc4->Add (Vector(392.07431373007154,249.83131645966594, 0.0));
    positionAlloc4->Add (Vector(372.76412239332905,358.8108736224511, 0.0));
    positionAlloc4->Add (Vector(155.83255853108906,128.84283094367882, 0.0));
    positionAlloc4->Add (Vector(53.740198847410994,243.062965263114, 0.0));
    positionAlloc4->Add (Vector(41.65339016176173,257.55599378241965, 0.0));
    positionAlloc4->Add (Vector(77.7711997451992,368.56684667836794, 0.0));
    positionAlloc4->Add (Vector(426.16204597394994,50.42239453117037, 0.0));
    positionAlloc4->Add (Vector(451.8613894533117,23.542708911628406, 0.0));
    positionAlloc4->Add (Vector(469.3257620208702,371.35926582300647, 0.0));
    positionAlloc4->Add (Vector(149.35615876309333,309.65354293898247, 0.0));
    positionAlloc4->Add (Vector(51.36443839202659,300.0946571047413, 0.0));
    positionAlloc4->Add (Vector(135.32215161212235,85.13935695383296, 0.0));
    positionAlloc4->Add (Vector(443.7479390207976,26.922192841244698, 0.0));
    positionAlloc4->Add (Vector(205.658843651906,216.3745129337485, 0.0));
    positionAlloc4->Add (Vector(37.68208496804731,336.5725033960796, 0.0));
    positionAlloc4->Add (Vector(388.4052563885673,113.4027819577329, 0.0));
    positionAlloc4->Add (Vector(141.79514783076164,240.54128482326914, 0.0));
    positionAlloc4->Add (Vector(374.47434961988154,296.19282609595984, 0.0));
    positionAlloc4->Add (Vector(314.3254567416988,81.93040719099093, 0.0));
    positionAlloc4->Add (Vector(451.2174258805114,397.48702521598176, 0.0));
    positionAlloc4->Add (Vector(150.50049666044674,357.63626384497405, 0.0));
    positionAlloc4->Add (Vector(391.2594677351809,192.97866949712244, 0.0));
    positionAlloc4->Add (Vector(57.59736280231576,187.62858372638308, 0.0));
    positionAlloc4->Add (Vector(153.60772808594464,213.15709254103683, 0.0));
    positionAlloc4->Add (Vector(103.48744006055777,86.20532927320585, 0.0));
    positionAlloc4->Add (Vector(484.9846924863351,170.25287986762527, 0.0));
    positionAlloc4->Add (Vector(120.70164384509624,110.05521085640363, 0.0));
    positionAlloc4->Add (Vector(411.8549335743228,483.48228124531704, 0.0));
    positionAlloc4->Add (Vector(499.2818144219669,244.81006329350458, 0.0));
    positionAlloc4->Add (Vector(263.9030636240076,6.360571547197713, 0.0));
    positionAlloc4->Add (Vector(9.139300749398727,35.4424188532792, 0.0));
    positionAlloc4->Add (Vector(237.70890129716875,450.7324061047929, 0.0));
    positionAlloc4->Add (Vector(10.664109146694468,103.33572420057618, 0.0));
    positionAlloc4->Add (Vector(339.7757903322339,73.46507513901263, 0.0));
    positionAlloc4->Add (Vector(187.96079378890585,375.76636154787786, 0.0));
    positionAlloc4->Add (Vector(381.8218177972871,144.7594264794339, 0.0));
    positionAlloc4->Add (Vector(357.5269248794764,220.4699218293086, 0.0));
    positionAlloc4->Add (Vector(491.8222072946668,101.39649023484465, 0.0));
    positionAlloc4->Add (Vector(464.7627747589523,407.46321571505524, 0.0));
    positionAlloc4->Add (Vector(41.29963529093955,47.67133022705766, 0.0));
    positionAlloc4->Add (Vector(42.83602559666261,369.349883208543, 0.0));
    positionAlloc4->Add (Vector(424.77579933423675,222.93588126267866, 0.0));
    positionAlloc4->Add (Vector(289.272753926839,109.39736251286742, 0.0));
    positionAlloc4->Add (Vector(381.8917421184399,498.8594259108934, 0.0));
    positionAlloc4->Add (Vector(16.318833099951966,98.99357284629961, 0.0));
    positionAlloc4->Add (Vector(497.57630452674687,15.967834040027551, 0.0));
    positionAlloc4->Add (Vector(79.35980825378513,370.95525377343694, 0.0));
    positionAlloc4->Add (Vector(222.3510980691179,189.6650169264429, 0.0));
    positionAlloc4->Add (Vector(408.8069488207018,82.08933441102323, 0.0));
    positionAlloc4->Add (Vector(180.17587116609957,271.4419188781082, 0.0));
    positionAlloc4->Add (Vector(83.40341594890171,440.009898076065, 0.0));
    positionAlloc4->Add (Vector(408.40116671668926,111.74820371074551, 0.0));
    positionAlloc4->Add (Vector(381.6947486936369,353.3623356386179, 0.0));
    positionAlloc4->Add (Vector(80.42190741160354,306.72269464389973, 0.0));
    positionAlloc4->Add (Vector(196.45488906370633,365.4336804070138, 0.0));
    positionAlloc4->Add (Vector(86.96156050676373,446.8429187384415, 0.0));
    positionAlloc4->Add (Vector(178.20786559913387,23.933523563125945, 0.0));
    positionAlloc4->Add (Vector(9.925285243707616,495.7102403287305, 0.0));
    positionAlloc4->Add (Vector(225.719048151429,281.47419760797334, 0.0));
    positionAlloc4->Add (Vector(129.4119641273485,66.68581824949865, 0.0));
    positionAlloc4->Add (Vector(314.89743793812727,458.5821613190769, 0.0));
    positionAlloc4->Add (Vector(311.89747119365774,491.494410716052, 0.0));
    positionAlloc4->Add (Vector(413.68296081639824,169.41306388325972, 0.0));
    positionAlloc4->Add (Vector(434.44552848538916,54.49637947425956, 0.0));
    positionAlloc4->Add (Vector(145.44397133589283,295.01507710963386, 0.0));
    positionAlloc4->Add (Vector(56.177773579237424,479.9319956890441, 0.0));
    positionAlloc4->Add (Vector(211.42227338409603,242.67923619811037, 0.0));
    positionAlloc4->Add (Vector(228.6550531074562,475.47468244292304, 0.0));
    positionAlloc4->Add (Vector(58.05721502139593,254.67751203472127, 0.0));
    positionAlloc4->Add (Vector(323.9084061258339,90.81255851568775, 0.0));
    positionAlloc4->Add (Vector(352.3049181918348,380.07444507068976, 0.0));
    positionAlloc4->Add (Vector(77.18448800099725,70.981010864271, 0.0));
    positionAlloc4->Add (Vector(316.15613165184044,345.39314215117685, 0.0));
    positionAlloc4->Add (Vector(175.88929666475144,77.97287376084489, 0.0));
    positionAlloc4->Add (Vector(352.42789242104607,243.2095845413676, 0.0));
    positionAlloc4->Add (Vector(116.11341665770614,388.26481421331204, 0.0));
    positionAlloc4->Add (Vector(326.7389810763402,477.9769414108756, 0.0));
    positionAlloc4->Add (Vector(464.53132573800343,447.40991570288884, 0.0));
    positionAlloc4->Add (Vector(348.0748928539599,219.9766239499661, 0.0));
    positionAlloc4->Add (Vector(53.16251399420091,207.6891827476597, 0.0));
    positionAlloc4->Add (Vector(408.34929006510305,443.6881290394416, 0.0));
    positionAlloc4->Add (Vector(68.3275431164011,134.09646397719382, 0.0));
    positionAlloc4->Add (Vector(351.3846053052344,105.17896291313333, 0.0));
    positionAlloc4->Add (Vector(144.68389538971394,226.68992663369886, 0.0));
    positionAlloc4->Add (Vector(483.54869998594074,241.09778308573306, 0.0));
    positionAlloc4->Add (Vector(476.7001763325104,30.222460903581993, 0.0));
    positionAlloc4->Add (Vector(108.45406229526894,287.8867253731108, 0.0));
    positionAlloc4->Add (Vector(394.2346661516873,3.0565341981767524, 0.0));
    positionAlloc4->Add (Vector(486.16615788588996,323.6354420511264, 0.0));
    positionAlloc4->Add (Vector(404.9527429694928,204.5498258298653, 0.0));
    positionAlloc4->Add (Vector(154.77260032241398,313.7561473763494, 0.0));
    positionAlloc4->Add (Vector(260.4496450553812,343.6008591119571, 0.0));
    positionAlloc4->Add (Vector(152.18286223395816,202.5339071835864, 0.0));
    positionAlloc4->Add (Vector(14.54062958848773,375.33211426365835, 0.0));
    positionAlloc4->Add (Vector(51.607568823527686,408.6561095360263, 0.0));
    positionAlloc4->Add (Vector(156.25020063848288,10.837148150293018, 0.0));
    positionAlloc4->Add (Vector(61.55410434677972,293.4713832952687, 0.0));
    positionAlloc4->Add (Vector(233.09841714689622,114.07207134601255, 0.0));
    positionAlloc4->Add (Vector(117.42115288437938,228.1203245308457, 0.0));
    positionAlloc4->Add (Vector(301.3862141990469,100.07170645003333, 0.0));
    positionAlloc4->Add (Vector(238.53394509867965,50.24926119657353, 0.0));
    positionAlloc4->Add (Vector(456.3287931465337,158.66265165915334, 0.0));
    positionAlloc4->Add (Vector(205.51272362279676,455.81856709509736, 0.0));
    positionAlloc4->Add (Vector(77.53150125410197,324.78798381684686, 0.0));
    positionAlloc4->Add (Vector(230.96716534222867,290.361265730746, 0.0));
    positionAlloc4->Add (Vector(468.28906059873,235.56692807255592, 0.0));
    positionAlloc4->Add (Vector(216.05026261871967,205.04497702640478, 0.0));
    positionAlloc4->Add (Vector(471.34773633197494,364.2657195708186, 0.0));
    positionAlloc4->Add (Vector(121.81071981604124,87.1130505969096, 0.0));
    positionAlloc4->Add (Vector(277.5244410747019,3.708031502550724, 0.0));
    positionAlloc4->Add (Vector(45.53465741803703,41.72476080478621, 0.0));
    positionAlloc4->Add (Vector(228.65343212488153,305.637660729525, 0.0));
    positionAlloc4->Add (Vector(101.11904598778088,452.31978306305507, 0.0));
    positionAlloc4->Add (Vector(388.5490344681457,258.3327857981776, 0.0));
    positionAlloc4->Add (Vector(415.73983120594966,99.71921792800154, 0.0));
    positionAlloc4->Add (Vector(247.180660790077,83.58733541893749, 0.0));
    positionAlloc4->Add (Vector(114.82967944367212,293.4545009537047, 0.0));
    positionAlloc4->Add (Vector(182.04161574064088,102.70902520043657, 0.0));
    positionAlloc4->Add (Vector(393.7431610714597,195.6764948832489, 0.0));
    positionAlloc4->Add (Vector(216.54998951207682,138.24731915736376, 0.0));
    positionAlloc4->Add (Vector(237.8096008478327,18.27310362671797, 0.0));
    positionAlloc4->Add (Vector(284.60845132550884,30.470829551356438, 0.0));
    positionAlloc4->Add (Vector(438.5082963652741,497.207502179135, 0.0));
    positionAlloc4->Add (Vector(248.3907594379013,43.585469584050514, 0.0));
    positionAlloc4->Add (Vector(17.933549038887662,388.16438637768147, 0.0));
    positionAlloc4->Add (Vector(352.8223390568799,52.499798874098005, 0.0));
    positionAlloc4->Add (Vector(92.16175640004653,90.26068816118726, 0.0));
    positionAlloc4->Add (Vector(112.49494046720665,341.1228231817137, 0.0));
    positionAlloc4->Add (Vector(424.55373367301615,33.39088380190813, 0.0));
    positionAlloc4->Add (Vector(327.9570747048019,474.53078840147344, 0.0));
    positionAlloc4->Add (Vector(251.88525882057806,45.09974167148639, 0.0));
    positionAlloc4->Add (Vector(172.52436059809722,0.5960661312513493, 0.0));
    positionAlloc4->Add (Vector(171.36206859360104,400.7419749262134, 0.0));
    positionAlloc4->Add (Vector(145.92087887406774,275.78946561812427, 0.0));
    positionAlloc4->Add (Vector(4.209507976794202,171.29689157364848, 0.0));
    positionAlloc4->Add (Vector(139.12019066340665,13.597437777538858, 0.0));
    positionAlloc4->Add (Vector(355.6013439710334,248.94212807894183, 0.0));
    positionAlloc4->Add (Vector(340.31470425887045,284.20584536220656, 0.0));
    positionAlloc4->Add (Vector(17.11823932887796,429.7529724237276, 0.0));
    positionAlloc4->Add (Vector(476.2751705774249,349.2753683545381, 0.0));
    positionAlloc4->Add (Vector(333.048818955559,362.9276631979808, 0.0));
    positionAlloc4->Add (Vector(265.4575949303509,403.81247329022295, 0.0));
    positionAlloc4->Add (Vector(443.50349430836667,398.1399791964899, 0.0));
    positionAlloc4->Add (Vector(494.8036745589202,449.0749352453076, 0.0));
    positionAlloc4->Add (Vector(377.2623302230354,306.55226420390983, 0.0));
    positionAlloc4->Add (Vector(429.09067785401345,54.132617407480375, 0.0));
    positionAlloc4->Add (Vector(21.740140838564457,323.11286222866414, 0.0));
    positionAlloc4->Add (Vector(353.97708100687186,94.09188172011245, 0.0));
    positionAlloc4->Add (Vector(353.96714318940934,168.8630036677749, 0.0));
    positionAlloc4->Add (Vector(56.14607664569604,268.52361034814646, 0.0));
    positionAlloc4->Add (Vector(364.6888534904171,168.78145952657343, 0.0));
    positionAlloc4->Add (Vector(136.12112797332375,236.1857280799473, 0.0));
    positionAlloc4->Add (Vector(10.749238510843584,87.10070466872271, 0.0));
    positionAlloc4->Add (Vector(384.09538318068644,445.926573403489, 0.0));
    positionAlloc4->Add (Vector(214.8279814580103,74.34817395896125, 0.0));
    positionAlloc4->Add (Vector(478.18756562700224,4.62895381342876, 0.0));
    positionAlloc4->Add (Vector(425.59776010759515,220.00072696705791, 0.0));
    positionAlloc4->Add (Vector(443.88871587222184,101.24691769378074, 0.0));
    positionAlloc4->Add (Vector(95.19244051142334,333.40150140548195, 0.0));
    positionAlloc4->Add (Vector(366.55951275240625,488.98724704200055, 0.0));
    positionAlloc4->Add (Vector(222.76214122532812,236.30914368249168, 0.0));
    positionAlloc4->Add (Vector(442.28738171063617,497.3222389643563, 0.0));
    positionAlloc4->Add (Vector(442.4672766487355,493.84941509574105, 0.0));
    positionAlloc4->Add (Vector(57.64955455293269,338.0140546052978, 0.0));
    positionAlloc4->Add (Vector(213.0773303939455,358.525897971369, 0.0));
    positionAlloc4->Add (Vector(147.3146757134041,436.92615022720514, 0.0));
    positionAlloc4->Add (Vector(318.2035001114098,369.1345029239244, 0.0));
    positionAlloc4->Add (Vector(167.91831663320744,357.19836503207284, 0.0));
    positionAlloc4->Add (Vector(201.83036047373724,31.09253644194615, 0.0));
    positionAlloc4->Add (Vector(316.428589163354,1.045699595102867, 0.0));
    positionAlloc4->Add (Vector(290.2879087509825,394.66099704232397, 0.0));
    positionAlloc4->Add (Vector(470.1247451286538,133.2275993386466, 0.0));
    positionAlloc4->Add (Vector(20.29186262682481,327.10478533900084, 0.0));
    positionAlloc4->Add (Vector(6.880473191353098,432.3553077639023, 0.0));
    positionAlloc4->Add (Vector(28.682985117536862,419.3506748148221, 0.0));
    positionAlloc4->Add (Vector(292.0465138555206,154.70568114711097, 0.0));
    positionAlloc4->Add (Vector(288.88119191137037,239.4355162974541, 0.0));
    positionAlloc4->Add (Vector(202.13036426191232,281.13058104897164, 0.0));
    positionAlloc4->Add (Vector(72.582104562878,321.1058346952696, 0.0));
    positionAlloc4->Add (Vector(110.51833355422808,107.8181100017947, 0.0));
    positionAlloc4->Add (Vector(296.1064580261313,204.55091915246842, 0.0));
    positionAlloc4->Add (Vector(258.67285021216657,473.9055536887921, 0.0));
    positionAlloc4->Add (Vector(327.25795750747363,150.64788525717375, 0.0));
    positionAlloc4->Add (Vector(241.03371614018525,331.07288291468274, 0.0));
    positionAlloc4->Add (Vector(198.44616696818497,94.14766435528709, 0.0));
    positionAlloc4->Add (Vector(317.56864745984757,471.9707643095931, 0.0));
    positionAlloc4->Add (Vector(291.5831506982168,90.85704968607678, 0.0));
    positionAlloc4->Add (Vector(312.8529899285974,37.91781400933725, 0.0));
    positionAlloc4->Add (Vector(375.7247000144628,282.60967400409777, 0.0));
    positionAlloc4->Add (Vector(452.43929101450186,314.899004599024, 0.0));
    positionAlloc4->Add (Vector(493.6329471889183,109.71555706967845, 0.0));
    positionAlloc4->Add (Vector(314.5973038422275,108.21861893513935, 0.0));
    positionAlloc4->Add (Vector(192.89641179642337,374.32998381260415, 0.0));
    positionAlloc4->Add (Vector(76.98212663923887,413.83116653488645, 0.0));
    positionAlloc4->Add (Vector(172.3781327500244,379.76064368262314, 0.0));
    positionAlloc4->Add (Vector(86.60199216098336,121.65952601200442, 0.0));
    positionAlloc4->Add (Vector(394.62404117566024,271.5930847225031, 0.0));
    positionAlloc4->Add (Vector(24.696326556977787,18.924860528211184, 0.0));
    positionAlloc4->Add (Vector(72.78372869427685,381.4934966688224, 0.0));
    positionAlloc4->Add (Vector(480.2871571986395,39.48319860047539, 0.0));
    positionAlloc4->Add (Vector(79.13224861752404,115.90960669292284, 0.0));
    positionAlloc4->Add (Vector(325.4577309905736,9.416762311197536, 0.0));
    positionAlloc4->Add (Vector(309.3132635938309,74.1689747113795, 0.0));
    positionAlloc4->Add (Vector(7.8482510900037,166.97984523768278, 0.0));
    positionAlloc4->Add (Vector(107.38501129404871,180.39906971399978, 0.0));
    positionAlloc4->Add (Vector(443.5933471447316,271.30731098601836, 0.0));
    positionAlloc4->Add (Vector(299.7675879458295,46.13126292797875, 0.0));
    positionAlloc4->Add (Vector(29.078069438536524,323.98371101740986, 0.0));
    positionAlloc4->Add (Vector(299.6748877567649,102.16488832468906, 0.0));
    positionAlloc4->Add (Vector(275.29010508036214,433.9671114439018, 0.0));
    positionAlloc4->Add (Vector(190.2694359979078,40.513577767177736, 0.0));
    positionAlloc4->Add (Vector(127.51291254688057,176.17803989508934, 0.0));
    positionAlloc4->Add (Vector(402.80755670415107,243.70921302352872, 0.0));
    positionAlloc4->Add (Vector(62.61989380834915,358.77629922354475, 0.0));
    positionAlloc4->Add (Vector(433.2460101923807,474.03156134404037, 0.0));
    positionAlloc4->Add (Vector(407.2091403627533,441.83793914499574, 0.0));
    positionAlloc4->Add (Vector(294.54463733088056,364.2320598086366, 0.0));
    positionAlloc4->Add (Vector(298.9782044470305,13.733682624565036, 0.0));
    positionAlloc4->Add (Vector(19.33278159006957,43.17193225828142, 0.0));
    positionAlloc4->Add (Vector(163.58037203109666,445.9513478004117, 0.0));
    positionAlloc4->Add (Vector(337.20763061445865,388.84069753979577, 0.0));
    positionAlloc4->Add (Vector(237.62764656970353,318.8805619358269, 0.0));
    positionAlloc4->Add (Vector(245.8111468200737,495.66454848048755, 0.0));
    positionAlloc4->Add (Vector(150.74384294731146,22.721742195373096, 0.0));
    positionAlloc4->Add (Vector(170.31504528360674,469.59550774001116, 0.0));
    positionAlloc4->Add (Vector(124.94659340464231,220.09604831299606, 0.0));
    positionAlloc4->Add (Vector(461.30548222743516,243.5553141201967, 0.0));
    positionAlloc4->Add (Vector(375.38997226426363,5.45471089903854, 0.0));
    positionAlloc4->Add (Vector(436.4888384730149,202.0887784607648, 0.0));
    positionAlloc4->Add (Vector(286.29324617662036,2.2280047940097525, 0.0));
    positionAlloc4->Add (Vector(368.66395755782725,54.93783737658009, 0.0));
    positionAlloc4->Add (Vector(456.9665272261756,406.2686028040863, 0.0));
    positionAlloc4->Add (Vector(468.65391694362836,392.4270000908137, 0.0));
    positionAlloc4->Add (Vector(197.52709130335282,36.33340860878032, 0.0));
    positionAlloc4->Add (Vector(180.94402340752504,160.10727976238402, 0.0));
    positionAlloc4->Add (Vector(263.1878541935792,58.00021325916943, 0.0));
    positionAlloc4->Add (Vector(266.44489167856005,444.8049552131738, 0.0));
    positionAlloc4->Add (Vector(434.5239282763275,6.74095515494283, 0.0));
    positionAlloc4->Add (Vector(248.06681594243213,244.3133379778637, 0.0));
    positionAlloc4->Add (Vector(259.61715025006157,56.9697687000742, 0.0));
    positionAlloc4->Add (Vector(125.34700025278045,419.5919400495454, 0.0));
    positionAlloc4->Add (Vector(236.77582116351087,207.8612734592861, 0.0));
    positionAlloc4->Add (Vector(117.48838080106161,385.5739219084667, 0.0));
    positionAlloc4->Add (Vector(356.54131381329023,495.5637690844028, 0.0));
    positionAlloc4->Add (Vector(50.97203887399021,264.26980051498117, 0.0));
    positionAlloc4->Add (Vector(19.738336841374938,28.928574342835688, 0.0));
    positionAlloc4->Add (Vector(405.5455694985841,254.54660128408247, 0.0));
    positionAlloc4->Add (Vector(120.99626629316279,480.8103999620773, 0.0));
    positionAlloc4->Add (Vector(29.511768745997145,472.28267478216685, 0.0));
    positionAlloc4->Add (Vector(45.77834926682367,173.9858859527343, 0.0));
    positionAlloc4->Add (Vector(465.18872724744836,332.14753400336417, 0.0));
    positionAlloc4->Add (Vector(80.12602497739208,91.19506660655446, 0.0));
    positionAlloc4->Add (Vector(206.06631662167152,359.8918889255993, 0.0));
    positionAlloc4->Add (Vector(376.69237439652863,285.0391681021945, 0.0));
    positionAlloc4->Add (Vector(414.32010592560954,11.5612469914666, 0.0));
    positionAlloc4->Add (Vector(171.24347105475078,408.00651323183996, 0.0));
    positionAlloc4->Add (Vector(293.2879989941506,319.643392035965, 0.0));
    positionAlloc4->Add (Vector(330.71241595877603,226.3408695061981, 0.0));
    positionAlloc4->Add (Vector(400.57390107868855,355.57468655032864, 0.0));
    positionAlloc4->Add (Vector(200.39487348639645,358.9819717968584, 0.0));
    positionAlloc4->Add (Vector(451.2884505766691,396.4145509890333, 0.0));
    positionAlloc4->Add (Vector(212.26152276808025,160.37393246402314, 0.0));
    positionAlloc4->Add (Vector(471.4596763685092,98.86383159701661, 0.0));
    positionAlloc4->Add (Vector(32.4512023249815,59.34931984450831, 0.0));
    positionAlloc4->Add (Vector(406.6777695859217,471.18431260921085, 0.0));
    positionAlloc4->Add (Vector(473.68607809544824,192.78558172641945, 0.0));
    positionAlloc4->Add (Vector(218.0000008153345,140.4089424939754, 0.0));
    positionAlloc4->Add (Vector(16.965128981533407,168.7016643881999, 0.0));
    positionAlloc4->Add (Vector(218.2297426063684,409.5986969676302, 0.0));
    positionAlloc4->Add (Vector(345.113285683117,142.40066742868174, 0.0));
    positionAlloc4->Add (Vector(294.87460739365275,156.1320189334377, 0.0));
    positionAlloc4->Add (Vector(463.5420869456981,388.0265392137968, 0.0));
    positionAlloc4->Add (Vector(125.58096347356967,493.12984905713796, 0.0));
    positionAlloc4->Add (Vector(192.5481581376586,381.05498492713355, 0.0));
    positionAlloc4->Add (Vector(306.8048192015186,376.9928564602786, 0.0));
    positionAlloc4->Add (Vector(430.3905752987755,226.2083640837718, 0.0));
    positionAlloc4->Add (Vector(116.18616819850214,378.2579781330222, 0.0));
    positionAlloc4->Add (Vector(253.25019303550928,393.2947239559493, 0.0));
    positionAlloc4->Add (Vector(395.3690222112841,77.11832265918255, 0.0));
    positionAlloc4->Add (Vector(125.58294264493142,63.30595045605863, 0.0));
    positionAlloc4->Add (Vector(96.51389822894357,330.6396305606823, 0.0));
    positionAlloc4->Add (Vector(467.81196784959843,159.73074465274766, 0.0));
    positionAlloc4->Add (Vector(218.4600627940137,5.239389659868976, 0.0));
    positionAlloc4->Add (Vector(37.2515028886039,467.80979004017894, 0.0));
    positionAlloc4->Add (Vector(386.0159736257917,203.69036333045233, 0.0));
    positionAlloc4->Add (Vector(359.99209925279075,110.88009495933909, 0.0));
    positionAlloc4->Add (Vector(292.8643223203567,196.86818606033336, 0.0));
    positionAlloc4->Add (Vector(36.71934067073829,433.9168736078661, 0.0));
    positionAlloc4->Add (Vector(381.82703001763576,126.26548490255519, 0.0));
    positionAlloc4->Add (Vector(175.9638624258134,86.44820930096553, 0.0));
    positionAlloc4->Add (Vector(355.71585811297786,36.64892328576558, 0.0));
    positionAlloc4->Add (Vector(65.1146685167116,131.79445298593672, 0.0));
    positionAlloc4->Add (Vector(21.491606616934998,290.2029149495801, 0.0));
    positionAlloc4->Add (Vector(163.2988062531721,343.13326404044466, 0.0));
    positionAlloc4->Add (Vector(2.2006687407277736,161.99020813688148, 0.0));
    positionAlloc4->Add (Vector(242.53509393702376,345.61414161581735, 0.0));
    positionAlloc4->Add (Vector(149.972665440382,214.82778605345726, 0.0));
    positionAlloc4->Add (Vector(248.02338605921327,147.98154079615728, 0.0));
    positionAlloc4->Add (Vector(397.905570191053,409.83228054277623, 0.0));
    positionAlloc4->Add (Vector(405.215834755079,16.020516966481214, 0.0));
    positionAlloc4->Add (Vector(378.2097420617372,252.17777211869964, 0.0));
    positionAlloc4->Add (Vector(351.360175645914,440.20902472306443, 0.0));
    positionAlloc4->Add (Vector(306.36173576302247,435.7419182193842, 0.0));
    positionAlloc4->Add (Vector(230.34027704734257,32.86006574129774, 0.0));
    positionAlloc4->Add (Vector(64.38810882674179,365.8298662998513, 0.0));
    positionAlloc4->Add (Vector(330.29506333705825,122.97474681401616, 0.0));
    positionAlloc4->Add (Vector(482.06722731555084,319.4530359998025, 0.0));
    positionAlloc4->Add (Vector(355.33780065634653,475.9247755128214, 0.0));
    positionAlloc4->Add (Vector(310.43684180664735,431.76791245297653, 0.0));
    positionAlloc4->Add (Vector(273.64008659796275,14.255541119478575, 0.0));
    positionAlloc4->Add (Vector(141.85131213101977,480.6312677930799, 0.0));
    positionAlloc4->Add (Vector(96.88271845808805,412.02560785121057, 0.0));
    positionAlloc4->Add (Vector(408.17274451510764,269.7204833969176, 0.0));
    positionAlloc4->Add (Vector(17.72684326751739,229.62666510451513, 0.0));
    positionAlloc4->Add (Vector(461.54196112984926,139.78152417280458, 0.0));
    positionAlloc4->Add (Vector(220.6886395706112,410.9299914573528, 0.0));
    positionAlloc4->Add (Vector(382.4959928936748,482.4497226910316, 0.0));
    positionAlloc4->Add (Vector(152.0948770247228,69.11890772577244, 0.0));
    positionAlloc4->Add (Vector(5.6833421847959364,290.43618798875764, 0.0));
    positionAlloc4->Add (Vector(276.91625017610727,331.1313642040641, 0.0));
    positionAlloc4->Add (Vector(455.85836828665856,426.87250191985885, 0.0));
    positionAlloc4->Add (Vector(406.42244143298274,238.66666224609867, 0.0));
    positionAlloc4->Add (Vector(120.23609500148092,413.8547348619067, 0.0));
    positionAlloc4->Add (Vector(354.7353028006254,117.61209338275714, 0.0));
    positionAlloc4->Add (Vector(430.5078470589268,498.8012479499574, 0.0));
    positionAlloc4->Add (Vector(305.79763827262843,420.74059945003063, 0.0));
    positionAlloc4->Add (Vector(364.3501706375504,94.21810147692949, 0.0));
    positionAlloc4->Add (Vector(82.2729158277024,142.12736974273403, 0.0));
    positionAlloc4->Add (Vector(335.86368123264816,325.8070731055017, 0.0));
    positionAlloc4->Add (Vector(168.12897359490765,403.5356027051152, 0.0));
    positionAlloc4->Add (Vector(181.8484565921785,460.42999370971853, 0.0));
    positionAlloc4->Add (Vector(496.2640971875547,280.3249184869005, 0.0));
    positionAlloc4->Add (Vector(389.57646078455355,428.6758636183842, 0.0));
    positionAlloc4->Add (Vector(383.73615094705286,204.3575373356998, 0.0));
    positionAlloc4->Add (Vector(262.79163089369536,60.691749717283805, 0.0));
    positionAlloc4->Add (Vector(220.91006526098562,370.17793132130976, 0.0));
    positionAlloc4->Add (Vector(76.49258600735797,9.296865447768866, 0.0));
    positionAlloc4->Add (Vector(462.6545823785734,261.12469096375014, 0.0));
    positionAlloc4->Add (Vector(76.13789577605995,360.75781520174155, 0.0));
    positionAlloc4->Add (Vector(176.2322048034512,15.562576355643676, 0.0));
    positionAlloc4->Add (Vector(70.79783582114102,364.9829807100903, 0.0));
    positionAlloc4->Add (Vector(270.99129141999555,483.3983089549055, 0.0));
    positionAlloc4->Add (Vector(265.68524155404316,275.89364491253116, 0.0));
    positionAlloc4->Add (Vector(26.359695329170208,14.858960043214465, 0.0));
    positionAlloc4->Add (Vector(422.01165207855496,440.9932337234287, 0.0));
    positionAlloc4->Add (Vector(321.92464932485285,48.53031430270388, 0.0));
    positionAlloc4->Add (Vector(69.53346340867577,410.1041542729521, 0.0));
    positionAlloc4->Add (Vector(186.67212789566128,369.6461317992225, 0.0));
    positionAlloc4->Add (Vector(310.09627430010977,19.507434196039785, 0.0));
    positionAlloc4->Add (Vector(5.5501432297513915,241.17693309720596, 0.0));
    positionAlloc4->Add (Vector(69.54572788556685,272.1546074558532, 0.0));
    positionAlloc4->Add (Vector(142.4879753836073,331.49552334178526, 0.0));
    positionAlloc4->Add (Vector(13.450869914316044,189.27483630790832, 0.0));
    positionAlloc4->Add (Vector(293.48182660225524,440.12218246688514, 0.0));
    positionAlloc4->Add (Vector(42.256850379056466,493.34960125775984, 0.0));
    positionAlloc4->Add (Vector(398.54010631194643,115.33375098823684, 0.0));
    positionAlloc4->Add (Vector(405.7205662070653,279.65657590345387, 0.0));
    positionAlloc4->Add (Vector(174.391416541664,212.4342520049411, 0.0));
    positionAlloc4->Add (Vector(475.6536941618201,448.31873802374633, 0.0));
    positionAlloc4->Add (Vector(395.69456347480116,46.360765974847396, 0.0));
    positionAlloc4->Add (Vector(290.5160862219028,9.710093010051047, 0.0));
    positionAlloc4->Add (Vector(239.2317485741638,372.87396365500155, 0.0));
    positionAlloc4->Add (Vector(382.5572728026691,362.33001699048947, 0.0));
    positionAlloc4->Add (Vector(110.61413503497036,365.01836790813354, 0.0));
    positionAlloc4->Add (Vector(115.3497687689547,5.258686530336454, 0.0));
    positionAlloc4->Add (Vector(366.9459580612665,212.42555829090736, 0.0));
    positionAlloc4->Add (Vector(463.87883750107454,298.1778548464133, 0.0));
    positionAlloc4->Add (Vector(232.40951937294196,359.77736117911587, 0.0));
    positionAlloc4->Add (Vector(472.87420832193084,52.64005680055056, 0.0));
    positionAlloc4->Add (Vector(0.03946785589831192,160.28679858198052, 0.0));
    positionAlloc4->Add (Vector(208.10089303998052,15.198025525863578, 0.0));
    positionAlloc4->Add (Vector(363.12673449530786,304.2775026069071, 0.0));
    positionAlloc4->Add (Vector(311.42748606363347,126.14534486962975, 0.0));
    positionAlloc4->Add (Vector(499.2488022891594,43.10418164222107, 0.0));
    positionAlloc4->Add (Vector(497.6489542996098,274.22779325293413, 0.0));
    positionAlloc4->Add (Vector(111.90473194472605,117.92458985184945, 0.0));
    positionAlloc4->Add (Vector(341.38373165347184,144.4140284807267, 0.0));
    positionAlloc4->Add (Vector(325.2088228969516,25.81468467198461, 0.0));
    positionAlloc4->Add (Vector(2.5242455640302475,411.65093506463404, 0.0));
    positionAlloc4->Add (Vector(242.6457215229621,466.72852300999756, 0.0));
    positionAlloc4->Add (Vector(282.50997008116735,239.39601011558455, 0.0));
    positionAlloc4->Add (Vector(173.74272393749246,288.6346795592143, 0.0));
    positionAlloc4->Add (Vector(271.717176511203,96.72381503081489, 0.0));
    positionAlloc4->Add (Vector(258.1260696641743,432.70835397950566, 0.0));
    positionAlloc4->Add (Vector(395.593217956988,40.27898935761887, 0.0));
    positionAlloc4->Add (Vector(329.90596727252085,489.9140601015869, 0.0));
    positionAlloc4->Add (Vector(204.1434907268077,300.78409442298044, 0.0));
    positionAlloc4->Add (Vector(440.23599113740613,68.80607177392922, 0.0));
    positionAlloc4->Add (Vector(329.10525665832245,198.5735814510075, 0.0));
    positionAlloc4->Add (Vector(472.101988811372,425.4862240544674, 0.0));
    positionAlloc4->Add (Vector(385.4508604432976,411.84741462617643, 0.0));
    positionAlloc4->Add (Vector(382.6399559941839,259.59885520977497, 0.0));
    positionAlloc4->Add (Vector(192.89779748864382,217.01094307464015, 0.0));
    positionAlloc4->Add (Vector(312.7606271022227,27.333080855201143, 0.0));
    positionAlloc4->Add (Vector(35.701967457621386,345.529163579865, 0.0));
    positionAlloc4->Add (Vector(319.40609069677055,298.8289033810354, 0.0));
    positionAlloc4->Add (Vector(399.11379447624006,170.8891873927911, 0.0));
    positionAlloc4->Add (Vector(250.74914967958134,404.2436791226611, 0.0));
    positionAlloc4->Add (Vector(172.63751623478103,57.16027482886371, 0.0));
    positionAlloc4->Add (Vector(461.32440117408584,446.3915031027225, 0.0));
    positionAlloc4->Add (Vector(254.5666187924857,424.00713317065964, 0.0));
    positionAlloc4->Add (Vector(300.9541589942577,499.62578061963194, 0.0));
    positionAlloc4->Add (Vector(264.82819552209736,385.8661091724667, 0.0));
    positionAlloc4->Add (Vector(252.41964600823002,419.92851767349936, 0.0));
    positionAlloc4->Add (Vector(306.33193759389445,422.7717696525569, 0.0));
    positionAlloc4->Add (Vector(378.0876041910294,363.0387587304385, 0.0));
    positionAlloc4->Add (Vector(98.96600201791139,490.1479639655517, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_16.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_ECC_flowmon/switch_ECC_16.flowmon", false, false);
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