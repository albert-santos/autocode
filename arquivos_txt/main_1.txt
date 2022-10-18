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
	  uint16_t numberOfRrhs = 50;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 720;
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
    positionAlloc->Add (Vector (388.8888888888889,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,0.0, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,500.0, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,500.0, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,500.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,55.55555555555556, 0.0));
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
    positionAlloc4->Add (Vector(112.90346121561757,485.66023463660554, 0.0));
    positionAlloc4->Add (Vector(221.90259918325927,50.79073797308309, 0.0));
    positionAlloc4->Add (Vector(445.25341620604473,190.9211983778806, 0.0));
    positionAlloc4->Add (Vector(425.97288564364777,422.12492978471414, 0.0));
    positionAlloc4->Add (Vector(55.622836283717504,379.03082906655425, 0.0));
    positionAlloc4->Add (Vector(251.50259752547737,321.67896232456775, 0.0));
    positionAlloc4->Add (Vector(238.47546124338896,139.11922121066516, 0.0));
    positionAlloc4->Add (Vector(178.00824301559325,476.83850923696093, 0.0));
    positionAlloc4->Add (Vector(27.849206356052314,146.9312427352721, 0.0));
    positionAlloc4->Add (Vector(2.8986964588444963,156.3405780777382, 0.0));
    positionAlloc4->Add (Vector(18.93129386021525,28.791752804086855, 0.0));
    positionAlloc4->Add (Vector(478.6974631633344,479.7353034248911, 0.0));
    positionAlloc4->Add (Vector(88.93237397567427,430.0333854381081, 0.0));
    positionAlloc4->Add (Vector(400.32982906461984,466.6011690933409, 0.0));
    positionAlloc4->Add (Vector(263.42593794591716,208.551209053429, 0.0));
    positionAlloc4->Add (Vector(44.49911041118898,92.45279513920418, 0.0));
    positionAlloc4->Add (Vector(386.95812497823425,167.13820249691312, 0.0));
    positionAlloc4->Add (Vector(302.60644756160644,495.20309844497336, 0.0));
    positionAlloc4->Add (Vector(355.1772891598047,227.82171232102698, 0.0));
    positionAlloc4->Add (Vector(259.01628079720433,186.3744010487428, 0.0));
    positionAlloc4->Add (Vector(475.9675837839625,443.9982469632323, 0.0));
    positionAlloc4->Add (Vector(488.4580809729251,261.7776523584464, 0.0));
    positionAlloc4->Add (Vector(153.94122190882337,464.0545392503631, 0.0));
    positionAlloc4->Add (Vector(398.2792937683555,162.3191611014983, 0.0));
    positionAlloc4->Add (Vector(385.50267897734113,164.86753962213047, 0.0));
    positionAlloc4->Add (Vector(14.48309308894924,442.3409471546472, 0.0));
    positionAlloc4->Add (Vector(230.76062662355727,140.6506362684057, 0.0));
    positionAlloc4->Add (Vector(362.8212727474345,12.962896392134937, 0.0));
    positionAlloc4->Add (Vector(126.91944117359361,455.50898684931326, 0.0));
    positionAlloc4->Add (Vector(173.3474406792891,176.82389353350254, 0.0));
    positionAlloc4->Add (Vector(342.5864984538498,243.77858374145904, 0.0));
    positionAlloc4->Add (Vector(47.38345822861456,420.8684130929248, 0.0));
    positionAlloc4->Add (Vector(7.371895449973465,485.91003648896304, 0.0));
    positionAlloc4->Add (Vector(35.27102237184959,36.73332718006167, 0.0));
    positionAlloc4->Add (Vector(499.2602535449029,69.82466684205174, 0.0));
    positionAlloc4->Add (Vector(448.021745787501,486.36017642641247, 0.0));
    positionAlloc4->Add (Vector(299.8701722360493,416.4840719185352, 0.0));
    positionAlloc4->Add (Vector(459.6187521280862,382.50257414907145, 0.0));
    positionAlloc4->Add (Vector(37.365722507380575,138.4032313002933, 0.0));
    positionAlloc4->Add (Vector(324.7915274392395,465.72350749645534, 0.0));
    positionAlloc4->Add (Vector(274.52672192525984,400.497277899178, 0.0));
    positionAlloc4->Add (Vector(383.9814391406558,26.546297726262548, 0.0));
    positionAlloc4->Add (Vector(138.60005239129924,267.4161162599057, 0.0));
    positionAlloc4->Add (Vector(259.14079016340617,2.6582664805516187, 0.0));
    positionAlloc4->Add (Vector(18.801939324800067,382.36331950024305, 0.0));
    positionAlloc4->Add (Vector(220.5033102044356,369.8059812537733, 0.0));
    positionAlloc4->Add (Vector(298.44426247298657,489.7950871797126, 0.0));
    positionAlloc4->Add (Vector(306.55616768350615,467.8681333040999, 0.0));
    positionAlloc4->Add (Vector(280.7456293996871,179.03995700646792, 0.0));
    positionAlloc4->Add (Vector(339.241226998962,343.6515746871949, 0.0));
    positionAlloc4->Add (Vector(195.3052493684529,348.1350415505515, 0.0));
    positionAlloc4->Add (Vector(277.95010556328776,268.384160971232, 0.0));
    positionAlloc4->Add (Vector(268.9938600698404,169.60209244254997, 0.0));
    positionAlloc4->Add (Vector(52.47683416496402,194.43670134736217, 0.0));
    positionAlloc4->Add (Vector(105.38547104350937,251.41219175920227, 0.0));
    positionAlloc4->Add (Vector(330.29917796429936,44.815349463731, 0.0));
    positionAlloc4->Add (Vector(489.33640257917347,118.19204933018779, 0.0));
    positionAlloc4->Add (Vector(131.5695119619371,35.80513972090227, 0.0));
    positionAlloc4->Add (Vector(431.308347252611,83.72092875326065, 0.0));
    positionAlloc4->Add (Vector(350.2665363097513,465.27990962492254, 0.0));
    positionAlloc4->Add (Vector(373.3287360615025,270.9855308857208, 0.0));
    positionAlloc4->Add (Vector(344.81969953977887,299.410445311711, 0.0));
    positionAlloc4->Add (Vector(108.98446503058395,462.88740081821993, 0.0));
    positionAlloc4->Add (Vector(225.32163433606723,167.2096963486363, 0.0));
    positionAlloc4->Add (Vector(202.0008045271895,68.05657712406105, 0.0));
    positionAlloc4->Add (Vector(196.94200392768823,344.47752411512573, 0.0));
    positionAlloc4->Add (Vector(395.3184967947994,322.5391995427132, 0.0));
    positionAlloc4->Add (Vector(196.41147249848123,196.4864989708082, 0.0));
    positionAlloc4->Add (Vector(302.15342183443613,479.40125781850026, 0.0));
    positionAlloc4->Add (Vector(268.50355842027034,182.99167333482714, 0.0));
    positionAlloc4->Add (Vector(393.6317237621307,50.11732155156617, 0.0));
    positionAlloc4->Add (Vector(241.86347559445292,294.9756491497019, 0.0));
    positionAlloc4->Add (Vector(273.22729535071534,408.2211987779689, 0.0));
    positionAlloc4->Add (Vector(173.68471018640642,390.11228984568186, 0.0));
    positionAlloc4->Add (Vector(491.29489465254267,456.86981242081583, 0.0));
    positionAlloc4->Add (Vector(92.49148277541491,422.1300025375492, 0.0));
    positionAlloc4->Add (Vector(115.45422524208959,87.4482544909611, 0.0));
    positionAlloc4->Add (Vector(339.18848612565995,251.59034527144354, 0.0));
    positionAlloc4->Add (Vector(336.2055494669629,4.191539762515795, 0.0));
    positionAlloc4->Add (Vector(370.8524001157777,307.5078624855244, 0.0));
    positionAlloc4->Add (Vector(39.670791479159476,151.42915235878928, 0.0));
    positionAlloc4->Add (Vector(63.35669192346138,122.13333160448792, 0.0));
    positionAlloc4->Add (Vector(214.39758017439885,191.05032127456977, 0.0));
    positionAlloc4->Add (Vector(126.94431047136983,295.69359113375845, 0.0));
    positionAlloc4->Add (Vector(229.97836553712918,381.93635903321496, 0.0));
    positionAlloc4->Add (Vector(33.74467153377114,440.2692987821634, 0.0));
    positionAlloc4->Add (Vector(319.17766037058647,163.78472205616256, 0.0));
    positionAlloc4->Add (Vector(121.44578748243534,311.623572593792, 0.0));
    positionAlloc4->Add (Vector(372.0679606847676,295.2602738235586, 0.0));
    positionAlloc4->Add (Vector(59.096314673519224,418.4988911067561, 0.0));
    positionAlloc4->Add (Vector(358.7974810973772,95.43497158615372, 0.0));
    positionAlloc4->Add (Vector(191.63172840264338,148.78735765693762, 0.0));
    positionAlloc4->Add (Vector(127.21905115721106,442.48325941903164, 0.0));
    positionAlloc4->Add (Vector(193.49461712814607,246.44043671566013, 0.0));
    positionAlloc4->Add (Vector(137.80155138042937,462.3704067691858, 0.0));
    positionAlloc4->Add (Vector(247.91319854260618,181.7443416424526, 0.0));
    positionAlloc4->Add (Vector(235.24883807610453,413.5680011661442, 0.0));
    positionAlloc4->Add (Vector(436.54718913482026,350.1447188167249, 0.0));
    positionAlloc4->Add (Vector(101.7169121850453,448.1612784020023, 0.0));
    positionAlloc4->Add (Vector(113.79886916112653,72.70181708561785, 0.0));
    positionAlloc4->Add (Vector(154.6231923293898,0.38219273909401164, 0.0));
    positionAlloc4->Add (Vector(444.72123773223944,291.703157304114, 0.0));
    positionAlloc4->Add (Vector(61.386064724709975,93.78494630861978, 0.0));
    positionAlloc4->Add (Vector(177.60469466982187,487.1063139475457, 0.0));
    positionAlloc4->Add (Vector(94.23419983320925,342.87844958627363, 0.0));
    positionAlloc4->Add (Vector(11.135755766402077,410.5609491094823, 0.0));
    positionAlloc4->Add (Vector(130.9998523596678,455.4030695433792, 0.0));
    positionAlloc4->Add (Vector(243.59695453671188,209.9967760051204, 0.0));
    positionAlloc4->Add (Vector(280.4743775609726,390.18176164463426, 0.0));
    positionAlloc4->Add (Vector(197.44113126840023,230.7898554811931, 0.0));
    positionAlloc4->Add (Vector(464.9778118266589,74.61801615487501, 0.0));
    positionAlloc4->Add (Vector(206.95992286470442,495.45781626272503, 0.0));
    positionAlloc4->Add (Vector(61.70942990736494,317.2593581459208, 0.0));
    positionAlloc4->Add (Vector(37.287035625739925,185.15284688694382, 0.0));
    positionAlloc4->Add (Vector(259.4021253667071,86.13985073856273, 0.0));
    positionAlloc4->Add (Vector(190.11824805863236,496.350436832409, 0.0));
    positionAlloc4->Add (Vector(168.37299935058365,495.9725059536405, 0.0));
    positionAlloc4->Add (Vector(98.91639232395966,214.66536130824616, 0.0));
    positionAlloc4->Add (Vector(478.15075487761607,294.9615635378849, 0.0));
    positionAlloc4->Add (Vector(410.9246873820053,314.1197985653289, 0.0));
    positionAlloc4->Add (Vector(317.6854807442267,423.8940218573708, 0.0));
    positionAlloc4->Add (Vector(172.16959772502693,75.6877849963995, 0.0));
    positionAlloc4->Add (Vector(424.88445500896535,333.559394349602, 0.0));
    positionAlloc4->Add (Vector(439.2939316023852,95.4196816342403, 0.0));
    positionAlloc4->Add (Vector(449.6104747829844,37.84844087915168, 0.0));
    positionAlloc4->Add (Vector(164.92228134094117,341.53718316828207, 0.0));
    positionAlloc4->Add (Vector(435.47280458052666,454.3726364291941, 0.0));
    positionAlloc4->Add (Vector(447.2162092137862,152.496987917608, 0.0));
    positionAlloc4->Add (Vector(100.3637440175455,264.77330523919744, 0.0));
    positionAlloc4->Add (Vector(328.53271231144475,290.5178287090451, 0.0));
    positionAlloc4->Add (Vector(214.22123797553672,432.3430723817672, 0.0));
    positionAlloc4->Add (Vector(165.19587318085138,421.74706303182774, 0.0));
    positionAlloc4->Add (Vector(364.69884912807635,362.45837360955954, 0.0));
    positionAlloc4->Add (Vector(99.29575152696313,12.73811788941015, 0.0));
    positionAlloc4->Add (Vector(117.53881834308515,230.80104044581103, 0.0));
    positionAlloc4->Add (Vector(245.5549806521481,44.08684062109053, 0.0));
    positionAlloc4->Add (Vector(299.8180042118584,344.4226295093325, 0.0));
    positionAlloc4->Add (Vector(96.92388375475358,410.3732493114691, 0.0));
    positionAlloc4->Add (Vector(57.51750602512312,186.70356586859765, 0.0));
    positionAlloc4->Add (Vector(213.8325498484821,49.72028145779461, 0.0));
    positionAlloc4->Add (Vector(306.7629498342158,380.00087617487446, 0.0));
    positionAlloc4->Add (Vector(160.2823269975801,407.7384645028131, 0.0));
    positionAlloc4->Add (Vector(88.33944653617071,195.5495615533812, 0.0));
    positionAlloc4->Add (Vector(452.05149295690245,116.88434630205296, 0.0));
    positionAlloc4->Add (Vector(380.40122815287805,390.52291109697853, 0.0));
    positionAlloc4->Add (Vector(309.3003712807682,75.4903666398229, 0.0));
    positionAlloc4->Add (Vector(340.82266880131147,348.5742643763781, 0.0));
    positionAlloc4->Add (Vector(224.36862941216097,352.15669919195483, 0.0));
    positionAlloc4->Add (Vector(222.83647820082803,336.23632526246496, 0.0));
    positionAlloc4->Add (Vector(40.03048863560865,240.84814151058853, 0.0));
    positionAlloc4->Add (Vector(403.0815814767041,375.82020204064526, 0.0));
    positionAlloc4->Add (Vector(485.5416598127319,376.53532793472743, 0.0));
    positionAlloc4->Add (Vector(347.9230669661632,35.97361008200056, 0.0));
    positionAlloc4->Add (Vector(122.22399968580544,292.2736330583149, 0.0));
    positionAlloc4->Add (Vector(152.64414967371664,185.82728836209122, 0.0));
    positionAlloc4->Add (Vector(293.9357531480932,132.06882210528482, 0.0));
    positionAlloc4->Add (Vector(253.55971177410225,227.34611377620345, 0.0));
    positionAlloc4->Add (Vector(146.6881392430283,464.4747309465578, 0.0));
    positionAlloc4->Add (Vector(498.7424963641647,291.01498817173086, 0.0));
    positionAlloc4->Add (Vector(151.67283585428453,10.687980544035359, 0.0));
    positionAlloc4->Add (Vector(394.1505948339845,6.048380007678933, 0.0));
    positionAlloc4->Add (Vector(18.034157284929965,100.09105820549347, 0.0));
    positionAlloc4->Add (Vector(63.3678545710043,294.9118240549446, 0.0));
    positionAlloc4->Add (Vector(360.6516720932854,220.17354341561912, 0.0));
    positionAlloc4->Add (Vector(160.4931872622266,61.47738577608924, 0.0));
    positionAlloc4->Add (Vector(411.2890762619355,439.7417617146585, 0.0));
    positionAlloc4->Add (Vector(427.59514195206765,302.3360261745939, 0.0));
    positionAlloc4->Add (Vector(458.8031738221966,294.1836664469384, 0.0));
    positionAlloc4->Add (Vector(3.6594846639319,164.12805182539043, 0.0));
    positionAlloc4->Add (Vector(96.27801047753421,376.46295748997113, 0.0));
    positionAlloc4->Add (Vector(474.10474909381264,267.3395285897944, 0.0));
    positionAlloc4->Add (Vector(29.588603592861197,375.1585922873429, 0.0));
    positionAlloc4->Add (Vector(299.4926444867099,307.8521436019744, 0.0));
    positionAlloc4->Add (Vector(172.83960664801273,297.81254906747483, 0.0));
    positionAlloc4->Add (Vector(301.0918269463554,481.78354359417096, 0.0));
    positionAlloc4->Add (Vector(80.92385706191757,422.43625911786955, 0.0));
    positionAlloc4->Add (Vector(119.72311845234019,65.70783026873028, 0.0));
    positionAlloc4->Add (Vector(113.50058260732898,297.9911928065069, 0.0));
    positionAlloc4->Add (Vector(283.84516505957464,57.55152048345169, 0.0));
    positionAlloc4->Add (Vector(141.0697242024303,441.87775409030047, 0.0));
    positionAlloc4->Add (Vector(312.4106470927495,427.5208606736757, 0.0));
    positionAlloc4->Add (Vector(299.2323465523182,263.63372824270846, 0.0));
    positionAlloc4->Add (Vector(440.88531154140367,243.37537240916907, 0.0));
    positionAlloc4->Add (Vector(101.69041641202553,413.11376774619566, 0.0));
    positionAlloc4->Add (Vector(32.2151277898034,448.49253475379123, 0.0));
    positionAlloc4->Add (Vector(436.34308245973097,210.83296742485464, 0.0));
    positionAlloc4->Add (Vector(124.1453644985493,147.53928762474794, 0.0));
    positionAlloc4->Add (Vector(309.1729346639427,265.643905240529, 0.0));
    positionAlloc4->Add (Vector(65.25533566963298,421.2229049782068, 0.0));
    positionAlloc4->Add (Vector(407.30959063082764,27.056830161999944, 0.0));
    positionAlloc4->Add (Vector(126.71952124989039,300.2444776372722, 0.0));
    positionAlloc4->Add (Vector(302.4634364871409,366.1874495896653, 0.0));
    positionAlloc4->Add (Vector(435.2124284115331,350.6901473166731, 0.0));
    positionAlloc4->Add (Vector(342.10902549161375,32.455792669564666, 0.0));
    positionAlloc4->Add (Vector(117.90731857375685,470.4139816006997, 0.0));
    positionAlloc4->Add (Vector(448.9253588338409,217.29748670425474, 0.0));
    positionAlloc4->Add (Vector(486.74369866259815,172.4306718581255, 0.0));
    positionAlloc4->Add (Vector(471.94631389189806,306.96700119235044, 0.0));
    positionAlloc4->Add (Vector(119.70552206887797,476.23689084662965, 0.0));
    positionAlloc4->Add (Vector(21.645469093079296,243.22037158519737, 0.0));
    positionAlloc4->Add (Vector(386.4428010835589,420.7110783979217, 0.0));
    positionAlloc4->Add (Vector(236.55597644293925,62.771554904361814, 0.0));
    positionAlloc4->Add (Vector(431.914572750726,40.81199135753405, 0.0));
    positionAlloc4->Add (Vector(31.6717217281699,210.82894301622545, 0.0));
    positionAlloc4->Add (Vector(25.4561917584436,117.53593204550728, 0.0));
    positionAlloc4->Add (Vector(411.50152032423546,475.88784651692583, 0.0));
    positionAlloc4->Add (Vector(436.53407422354974,328.10077387204944, 0.0));
    positionAlloc4->Add (Vector(227.02952802957566,7.701759895960447, 0.0));
    positionAlloc4->Add (Vector(216.17779738217203,423.85023411871884, 0.0));
    positionAlloc4->Add (Vector(256.0305137619594,308.73050600857806, 0.0));
    positionAlloc4->Add (Vector(26.156473218738373,302.5583902501831, 0.0));
    positionAlloc4->Add (Vector(96.70819950745918,167.8319005823249, 0.0));
    positionAlloc4->Add (Vector(19.929963317726283,67.39655898033952, 0.0));
    positionAlloc4->Add (Vector(383.8944888290865,268.00424736978187, 0.0));
    positionAlloc4->Add (Vector(335.1750159680842,215.2358913724875, 0.0));
    positionAlloc4->Add (Vector(18.556332246997208,346.9277056869499, 0.0));
    positionAlloc4->Add (Vector(20.737539788249435,160.91439998512226, 0.0));
    positionAlloc4->Add (Vector(234.73951427996286,105.38225123435268, 0.0));
    positionAlloc4->Add (Vector(375.85386045963344,381.31604772255537, 0.0));
    positionAlloc4->Add (Vector(162.90498330660242,444.91367072368746, 0.0));
    positionAlloc4->Add (Vector(162.98360365449292,387.2386330075398, 0.0));
    positionAlloc4->Add (Vector(155.8514588594907,334.39830296143487, 0.0));
    positionAlloc4->Add (Vector(277.33365539383334,487.2017344565013, 0.0));
    positionAlloc4->Add (Vector(263.9233980589797,169.70836200880473, 0.0));
    positionAlloc4->Add (Vector(406.25365208733587,141.90091747343715, 0.0));
    positionAlloc4->Add (Vector(437.06296779901413,475.567809209346, 0.0));
    positionAlloc4->Add (Vector(262.900984975735,38.01193972105344, 0.0));
    positionAlloc4->Add (Vector(193.00200551129777,285.77104636266614, 0.0));
    positionAlloc4->Add (Vector(435.0958731026888,118.43435177849926, 0.0));
    positionAlloc4->Add (Vector(71.3966414015002,47.05013724981971, 0.0));
    positionAlloc4->Add (Vector(110.06433701349061,335.7145855406367, 0.0));
    positionAlloc4->Add (Vector(164.18069504379505,273.78213524047675, 0.0));
    positionAlloc4->Add (Vector(126.79087468581417,136.90485207905428, 0.0));
    positionAlloc4->Add (Vector(223.31752291184526,438.51719463517594, 0.0));
    positionAlloc4->Add (Vector(269.8245550466753,447.6301828656104, 0.0));
    positionAlloc4->Add (Vector(468.62603106422443,244.7824805725843, 0.0));
    positionAlloc4->Add (Vector(237.0031449940273,174.58370647853866, 0.0));
    positionAlloc4->Add (Vector(19.864353341542408,35.166720807762616, 0.0));
    positionAlloc4->Add (Vector(349.81159119979196,459.129294566029, 0.0));
    positionAlloc4->Add (Vector(496.73023154992495,417.2432570090148, 0.0));
    positionAlloc4->Add (Vector(160.51315708365794,278.95599712188624, 0.0));
    positionAlloc4->Add (Vector(325.8946380050452,201.3284260874596, 0.0));
    positionAlloc4->Add (Vector(8.287941450467073,56.89817822370341, 0.0));
    positionAlloc4->Add (Vector(35.45920023519361,76.30199352113593, 0.0));
    positionAlloc4->Add (Vector(190.54074564672618,329.6724680976824, 0.0));
    positionAlloc4->Add (Vector(8.357794538503216,199.32952996118937, 0.0));
    positionAlloc4->Add (Vector(145.1489985309341,108.44574380589766, 0.0));
    positionAlloc4->Add (Vector(95.83786945494505,383.888596073637, 0.0));
    positionAlloc4->Add (Vector(199.6093026762269,398.48112598995004, 0.0));
    positionAlloc4->Add (Vector(71.80349893710464,41.601318217937774, 0.0));
    positionAlloc4->Add (Vector(249.96618554959997,388.5491146378915, 0.0));
    positionAlloc4->Add (Vector(54.6914570967923,336.520527108311, 0.0));
    positionAlloc4->Add (Vector(251.75475492419824,482.96653250986117, 0.0));
    positionAlloc4->Add (Vector(12.876741203291687,145.576970276214, 0.0));
    positionAlloc4->Add (Vector(36.00601758488497,209.06806686265787, 0.0));
    positionAlloc4->Add (Vector(204.54541595842994,134.4460111063229, 0.0));
    positionAlloc4->Add (Vector(442.99898558267563,263.8093861557807, 0.0));
    positionAlloc4->Add (Vector(238.5064972011316,224.43941835338276, 0.0));
    positionAlloc4->Add (Vector(71.11062653639333,363.4468766722813, 0.0));
    positionAlloc4->Add (Vector(75.39219918609629,184.69996280595313, 0.0));
    positionAlloc4->Add (Vector(247.42154209128182,264.26674508878256, 0.0));
    positionAlloc4->Add (Vector(354.2304350258586,388.73437500159463, 0.0));
    positionAlloc4->Add (Vector(437.45127560619045,225.9181247104353, 0.0));
    positionAlloc4->Add (Vector(315.4796001697652,490.4618378752907, 0.0));
    positionAlloc4->Add (Vector(285.5878695931167,90.23425449755779, 0.0));
    positionAlloc4->Add (Vector(258.3531798838061,15.548537727773926, 0.0));
    positionAlloc4->Add (Vector(497.52616116955556,29.181323073815825, 0.0));
    positionAlloc4->Add (Vector(278.20345783546264,188.18160834354376, 0.0));
    positionAlloc4->Add (Vector(21.767131878882683,475.37671403050115, 0.0));
    positionAlloc4->Add (Vector(122.75934009325373,455.5096198284841, 0.0));
    positionAlloc4->Add (Vector(444.5655445537414,177.476425574786, 0.0));
    positionAlloc4->Add (Vector(387.20506666801975,276.38379827494623, 0.0));
    positionAlloc4->Add (Vector(489.7416770899032,182.18903518411523, 0.0));
    positionAlloc4->Add (Vector(191.2337286354588,234.8945200431801, 0.0));
    positionAlloc4->Add (Vector(114.60251364603286,216.9416881058503, 0.0));
    positionAlloc4->Add (Vector(322.1160099395159,175.3282512160837, 0.0));
    positionAlloc4->Add (Vector(262.8347221754897,14.245424745783609, 0.0));
    positionAlloc4->Add (Vector(397.6505837028214,243.95046958981015, 0.0));
    positionAlloc4->Add (Vector(373.50241451497254,170.18964538273363, 0.0));
    positionAlloc4->Add (Vector(189.29029662530112,94.0971300149106, 0.0));
    positionAlloc4->Add (Vector(351.7596655525406,334.228083794741, 0.0));
    positionAlloc4->Add (Vector(142.31194050347108,30.518340559835544, 0.0));
    positionAlloc4->Add (Vector(311.8352054988509,23.996282024433658, 0.0));
    positionAlloc4->Add (Vector(211.37633082228047,53.75443809258385, 0.0));
    positionAlloc4->Add (Vector(26.497271333994764,99.74777306928068, 0.0));
    positionAlloc4->Add (Vector(32.36999172077443,495.67325436590454, 0.0));
    positionAlloc4->Add (Vector(289.6802701486165,462.352090768014, 0.0));
    positionAlloc4->Add (Vector(424.9849442579827,375.5605201859687, 0.0));
    positionAlloc4->Add (Vector(214.01873473413508,299.63487782901507, 0.0));
    positionAlloc4->Add (Vector(282.83105627196375,234.332918830626, 0.0));
    positionAlloc4->Add (Vector(75.86243551598682,319.5634243599525, 0.0));
    positionAlloc4->Add (Vector(474.14958703633255,322.5428201302218, 0.0));
    positionAlloc4->Add (Vector(235.29393974944284,493.2863897934408, 0.0));
    positionAlloc4->Add (Vector(316.7370063112105,62.37565003458961, 0.0));
    positionAlloc4->Add (Vector(429.82547678460054,335.79919045319497, 0.0));
    positionAlloc4->Add (Vector(254.80664710877932,148.47354289345228, 0.0));
    positionAlloc4->Add (Vector(303.50773039451946,72.73280513310998, 0.0));
    positionAlloc4->Add (Vector(400.16612288820085,213.3195791340718, 0.0));
    positionAlloc4->Add (Vector(130.9402020517601,422.2634716147395, 0.0));
    positionAlloc4->Add (Vector(80.75538524300596,171.22032056740528, 0.0));
    positionAlloc4->Add (Vector(214.46616701495148,248.64459295975817, 0.0));
    positionAlloc4->Add (Vector(91.97672077361102,231.41955935412994, 0.0));
    positionAlloc4->Add (Vector(257.08752176294144,437.66636317726955, 0.0));
    positionAlloc4->Add (Vector(84.97035710862887,74.72339946943124, 0.0));
    positionAlloc4->Add (Vector(306.7943472514178,324.15637808156225, 0.0));
    positionAlloc4->Add (Vector(315.8233134329698,353.65830134084973, 0.0));
    positionAlloc4->Add (Vector(177.6539558100611,28.82740362062819, 0.0));
    positionAlloc4->Add (Vector(212.32455796566984,185.96106083384362, 0.0));
    positionAlloc4->Add (Vector(346.18006081170444,113.64877045797478, 0.0));
    positionAlloc4->Add (Vector(344.2486589744594,126.69801724314983, 0.0));
    positionAlloc4->Add (Vector(358.385407073188,349.6538118484071, 0.0));
    positionAlloc4->Add (Vector(170.97641815122623,72.29886041904837, 0.0));
    positionAlloc4->Add (Vector(389.87214774129995,87.36815641370372, 0.0));
    positionAlloc4->Add (Vector(469.240945928367,171.51405491520327, 0.0));
    positionAlloc4->Add (Vector(6.8837562788583995,208.36198118534804, 0.0));
    positionAlloc4->Add (Vector(341.2268548834077,114.10286667524205, 0.0));
    positionAlloc4->Add (Vector(139.49544522848544,304.41558709689787, 0.0));
    positionAlloc4->Add (Vector(223.05860951721485,460.93415274190767, 0.0));
    positionAlloc4->Add (Vector(292.6742294691255,449.10252300856854, 0.0));
    positionAlloc4->Add (Vector(444.19321719769295,92.19222118388338, 0.0));
    positionAlloc4->Add (Vector(243.9607459026012,427.6782062976849, 0.0));
    positionAlloc4->Add (Vector(147.6443971999239,272.0606917926786, 0.0));
    positionAlloc4->Add (Vector(134.32506195311328,404.13900685269004, 0.0));
    positionAlloc4->Add (Vector(165.13846826254047,348.6844378065697, 0.0));
    positionAlloc4->Add (Vector(216.46925436708153,178.7555836652207, 0.0));
    positionAlloc4->Add (Vector(126.01654006541968,396.4278389884438, 0.0));
    positionAlloc4->Add (Vector(428.7422495417983,309.7933696842869, 0.0));
    positionAlloc4->Add (Vector(318.15306292799175,187.5455091969555, 0.0));
    positionAlloc4->Add (Vector(160.15966291101563,491.24547248744767, 0.0));
    positionAlloc4->Add (Vector(203.46099706467896,487.61117121865607, 0.0));
    positionAlloc4->Add (Vector(204.31861250663118,120.7093739827994, 0.0));
    positionAlloc4->Add (Vector(160.90998169395198,86.30231005989624, 0.0));
    positionAlloc4->Add (Vector(323.8853165937406,89.8971488464872, 0.0));
    positionAlloc4->Add (Vector(388.92423842958766,203.24143448854315, 0.0));
    positionAlloc4->Add (Vector(395.91051975207694,347.88210713591974, 0.0));
    positionAlloc4->Add (Vector(397.28917559237607,253.53996164206822, 0.0));
    positionAlloc4->Add (Vector(357.97284973119724,282.24916012043707, 0.0));
    positionAlloc4->Add (Vector(72.15770011452405,7.046147096221478, 0.0));
    positionAlloc4->Add (Vector(147.10784412212735,51.72977805820067, 0.0));
    positionAlloc4->Add (Vector(94.77585141696365,482.49446205545695, 0.0));
    positionAlloc4->Add (Vector(198.99531970396123,252.87100872540518, 0.0));
    positionAlloc4->Add (Vector(209.28901288204932,341.7324500831506, 0.0));
    positionAlloc4->Add (Vector(245.77257500846767,428.3512482588961, 0.0));
    positionAlloc4->Add (Vector(463.9244964125054,447.9849835033242, 0.0));
    positionAlloc4->Add (Vector(238.93100221611058,67.16930432784807, 0.0));
    positionAlloc4->Add (Vector(127.8725969029656,401.3447598166493, 0.0));
    positionAlloc4->Add (Vector(430.60581918339716,219.91540710428626, 0.0));
    positionAlloc4->Add (Vector(4.469551185622533,411.36500271096776, 0.0));
    positionAlloc4->Add (Vector(161.0647568073848,473.0761977283827, 0.0));
    positionAlloc4->Add (Vector(494.670028083319,38.85728516619324, 0.0));
    positionAlloc4->Add (Vector(418.98898102386465,3.9664380972615487, 0.0));
    positionAlloc4->Add (Vector(474.4181441401098,312.16835120133794, 0.0));
    positionAlloc4->Add (Vector(350.363018241269,270.1304701984499, 0.0));
    positionAlloc4->Add (Vector(423.9696962217381,125.81957117792642, 0.0));
    positionAlloc4->Add (Vector(258.9300240861609,97.23290476394226, 0.0));
    positionAlloc4->Add (Vector(286.63613577484233,471.3812877164746, 0.0));
    positionAlloc4->Add (Vector(421.44992009642493,33.90346717047687, 0.0));
    positionAlloc4->Add (Vector(252.35030404794716,249.02781241400373, 0.0));
    positionAlloc4->Add (Vector(302.73950051546734,383.38729361855104, 0.0));
    positionAlloc4->Add (Vector(216.49075348382408,454.65599432746836, 0.0));
    positionAlloc4->Add (Vector(328.1146658585657,65.92048751375107, 0.0));
    positionAlloc4->Add (Vector(392.2287093959616,228.4828502807752, 0.0));
    positionAlloc4->Add (Vector(438.6802373474063,223.84450007281427, 0.0));
    positionAlloc4->Add (Vector(157.3828436620082,313.42780108141733, 0.0));
    positionAlloc4->Add (Vector(110.74102786676437,233.6229119632769, 0.0));
    positionAlloc4->Add (Vector(172.84941058775115,337.4858024045526, 0.0));
    positionAlloc4->Add (Vector(414.067680365756,213.17407711811532, 0.0));
    positionAlloc4->Add (Vector(228.68009084549922,222.05855350830723, 0.0));
    positionAlloc4->Add (Vector(306.9355448706263,162.06926494872332, 0.0));
    positionAlloc4->Add (Vector(69.17734489647326,145.07223155227084, 0.0));
    positionAlloc4->Add (Vector(271.5832151190473,143.35582480321528, 0.0));
    positionAlloc4->Add (Vector(151.04836564504913,441.5142542819171, 0.0));
    positionAlloc4->Add (Vector(60.38359215461486,110.1419938968638, 0.0));
    positionAlloc4->Add (Vector(448.20830122842347,119.65785371875232, 0.0));
    positionAlloc4->Add (Vector(1.5564289125960529,387.4483233622989, 0.0));
    positionAlloc4->Add (Vector(200.9329619839187,348.65400963217047, 0.0));
    positionAlloc4->Add (Vector(375.3121595875877,306.6056057721299, 0.0));
    positionAlloc4->Add (Vector(187.4379717684297,385.1612626649788, 0.0));
    positionAlloc4->Add (Vector(353.4781226243805,390.26477843837137, 0.0));
    positionAlloc4->Add (Vector(364.425438759628,258.3443914607692, 0.0));
    positionAlloc4->Add (Vector(466.95130415744393,428.9485522689469, 0.0));
    positionAlloc4->Add (Vector(139.0425243064914,1.8968362046712928, 0.0));
    positionAlloc4->Add (Vector(358.46450084623496,493.6630874042133, 0.0));
    positionAlloc4->Add (Vector(201.61023605292462,379.3734655111683, 0.0));
    positionAlloc4->Add (Vector(430.1184987115576,279.4519055804692, 0.0));
    positionAlloc4->Add (Vector(430.9832788335367,9.887549758172131, 0.0));
    positionAlloc4->Add (Vector(361.45759677810685,88.27451713854151, 0.0));
    positionAlloc4->Add (Vector(108.30860416438969,379.59802465857234, 0.0));
    positionAlloc4->Add (Vector(74.96574245862797,275.5435870934859, 0.0));
    positionAlloc4->Add (Vector(263.9519892040633,370.0387715795358, 0.0));
    positionAlloc4->Add (Vector(393.5581448279171,94.80297390914826, 0.0));
    positionAlloc4->Add (Vector(286.2025562173246,22.422667294544063, 0.0));
    positionAlloc4->Add (Vector(492.8544118565939,88.58058221920867, 0.0));
    positionAlloc4->Add (Vector(393.7759198262536,53.29193839440055, 0.0));
    positionAlloc4->Add (Vector(178.53080444139596,54.03266434861842, 0.0));
    positionAlloc4->Add (Vector(253.96084997264373,130.77477591649355, 0.0));
    positionAlloc4->Add (Vector(103.2270294447165,38.09440451616059, 0.0));
    positionAlloc4->Add (Vector(247.05813294816252,66.08057097146907, 0.0));
    positionAlloc4->Add (Vector(185.07708877686912,265.34871923806975, 0.0));
    positionAlloc4->Add (Vector(40.051821903024646,452.31550146812145, 0.0));
    positionAlloc4->Add (Vector(197.5484344449866,150.0405229626782, 0.0));
    positionAlloc4->Add (Vector(417.63580124498293,96.12500841107385, 0.0));
    positionAlloc4->Add (Vector(391.09090841570264,28.07051983095765, 0.0));
    positionAlloc4->Add (Vector(472.6542199999749,212.78069513077347, 0.0));
    positionAlloc4->Add (Vector(147.74114282774852,365.8032356207544, 0.0));
    positionAlloc4->Add (Vector(319.10935531016673,298.5636514733245, 0.0));
    positionAlloc4->Add (Vector(300.47712405382,374.46418762291825, 0.0));
    positionAlloc4->Add (Vector(377.1529752881493,105.36619604166036, 0.0));
    positionAlloc4->Add (Vector(94.37253343859298,321.9478221558314, 0.0));
    positionAlloc4->Add (Vector(73.37574038214706,364.65945968056167, 0.0));
    positionAlloc4->Add (Vector(215.59087758018546,202.1009528875736, 0.0));
    positionAlloc4->Add (Vector(79.91682517204669,101.30106260983129, 0.0));
    positionAlloc4->Add (Vector(63.105051874679354,488.0217752338738, 0.0));
    positionAlloc4->Add (Vector(353.0259021679666,239.01565398264168, 0.0));
    positionAlloc4->Add (Vector(217.65297881476,461.7281075593876, 0.0));
    positionAlloc4->Add (Vector(152.2589939603101,417.42702381589925, 0.0));
    positionAlloc4->Add (Vector(355.6572590682434,276.9423055257071, 0.0));
    positionAlloc4->Add (Vector(103.81283090190718,458.6667815408153, 0.0));
    positionAlloc4->Add (Vector(40.264763564192954,115.85115524289603, 0.0));
    positionAlloc4->Add (Vector(337.2820118817053,399.8882684486931, 0.0));
    positionAlloc4->Add (Vector(13.313942345810737,55.46018451712031, 0.0));
    positionAlloc4->Add (Vector(71.25916888292232,188.54129960366706, 0.0));
    positionAlloc4->Add (Vector(107.05606841032423,99.47396171353334, 0.0));
    positionAlloc4->Add (Vector(372.2365765388661,110.69835045410053, 0.0));
    positionAlloc4->Add (Vector(154.32097381532554,366.62253386321373, 0.0));
    positionAlloc4->Add (Vector(106.84661881479057,383.64225421088327, 0.0));
    positionAlloc4->Add (Vector(403.8492045561047,176.7674313339952, 0.0));
    positionAlloc4->Add (Vector(386.899643622091,213.86866466803812, 0.0));
    positionAlloc4->Add (Vector(373.6604051659616,144.8013730934387, 0.0));
    positionAlloc4->Add (Vector(427.91418362696385,97.51874300435365, 0.0));
    positionAlloc4->Add (Vector(335.86489240871015,176.21481510820442, 0.0));
    positionAlloc4->Add (Vector(137.05988758342608,280.2649952680294, 0.0));
    positionAlloc4->Add (Vector(336.8492119493158,289.24978989460027, 0.0));
    positionAlloc4->Add (Vector(415.19978501866973,324.577098556239, 0.0));
    positionAlloc4->Add (Vector(242.2142227571878,429.6589030488, 0.0));
    positionAlloc4->Add (Vector(322.18054237849645,2.6199188257329253, 0.0));
    positionAlloc4->Add (Vector(224.62337679331878,457.39434337096714, 0.0));
    positionAlloc4->Add (Vector(336.234922326867,343.4041133284959, 0.0));
    positionAlloc4->Add (Vector(404.99435533120817,6.301883514938156, 0.0));
    positionAlloc4->Add (Vector(11.457565661190882,259.5411611059947, 0.0));
    positionAlloc4->Add (Vector(385.0749781560461,65.16789047717714, 0.0));
    positionAlloc4->Add (Vector(252.25958485049733,336.9245040084338, 0.0));
    positionAlloc4->Add (Vector(62.41127155583814,156.25068924602476, 0.0));
    positionAlloc4->Add (Vector(377.44572853026636,231.92889195014783, 0.0));
    positionAlloc4->Add (Vector(169.35401274083983,261.94806783306586, 0.0));
    positionAlloc4->Add (Vector(492.19172509675536,110.117113079578, 0.0));
    positionAlloc4->Add (Vector(379.1391302384981,159.9764190388392, 0.0));
    positionAlloc4->Add (Vector(68.83137047280158,5.444366901739494, 0.0));
    positionAlloc4->Add (Vector(234.3141713730546,228.70245804471583, 0.0));
    positionAlloc4->Add (Vector(246.03590074677922,365.0456055450522, 0.0));
    positionAlloc4->Add (Vector(6.568178461536123,176.5030427709124, 0.0));
    positionAlloc4->Add (Vector(334.36638510336604,238.44116676059667, 0.0));
    positionAlloc4->Add (Vector(412.1189198122489,316.3079083889986, 0.0));
    positionAlloc4->Add (Vector(486.3937959663276,383.73743443150653, 0.0));
    positionAlloc4->Add (Vector(487.67501713514326,83.99164659980745, 0.0));
    positionAlloc4->Add (Vector(225.86657434573766,187.71741658392887, 0.0));
    positionAlloc4->Add (Vector(46.78854790874038,32.68585752111319, 0.0));
    positionAlloc4->Add (Vector(297.80396680132276,16.21316265722855, 0.0));
    positionAlloc4->Add (Vector(487.9945341654525,450.78129233466063, 0.0));
    positionAlloc4->Add (Vector(408.41755439581095,297.1863119154889, 0.0));
    positionAlloc4->Add (Vector(285.35792577312816,498.37671260410497, 0.0));
    positionAlloc4->Add (Vector(259.50799203683937,87.36007505446125, 0.0));
    positionAlloc4->Add (Vector(332.05127767739674,248.62504668026563, 0.0));
    positionAlloc4->Add (Vector(28.48219974141636,114.40683548018794, 0.0));
    positionAlloc4->Add (Vector(287.10954442042987,174.49379861039444, 0.0));
    positionAlloc4->Add (Vector(350.9382648781552,120.17781242456942, 0.0));
    positionAlloc4->Add (Vector(408.1757559136071,92.56519913451532, 0.0));
    positionAlloc4->Add (Vector(303.54708098623024,206.79320750058068, 0.0));
    positionAlloc4->Add (Vector(157.1883080570252,87.38293817815578, 0.0));
    positionAlloc4->Add (Vector(43.74110477679544,113.65486814234438, 0.0));
    positionAlloc4->Add (Vector(460.1032571528302,113.95014497565847, 0.0));
    positionAlloc4->Add (Vector(468.43419065454685,422.1649742828566, 0.0));
    positionAlloc4->Add (Vector(185.6468806395272,465.9305583182703, 0.0));
    positionAlloc4->Add (Vector(213.23739425069797,123.6980183011927, 0.0));
    positionAlloc4->Add (Vector(258.6337992505649,458.2029265269489, 0.0));
    positionAlloc4->Add (Vector(288.3779674935454,155.72210706623142, 0.0));
    positionAlloc4->Add (Vector(145.04842584371292,4.67428898325789, 0.0));
    positionAlloc4->Add (Vector(226.42674997576873,314.15451876067857, 0.0));
    positionAlloc4->Add (Vector(279.2582754612589,345.11393408137394, 0.0));
    positionAlloc4->Add (Vector(162.99490588501266,429.7446604327929, 0.0));
    positionAlloc4->Add (Vector(13.643611758734409,261.02562362625537, 0.0));
    positionAlloc4->Add (Vector(358.4393318158446,445.50947714616944, 0.0));
    positionAlloc4->Add (Vector(469.4432386848567,15.474503135575358, 0.0));
    positionAlloc4->Add (Vector(216.79616363658226,256.1463461391807, 0.0));
    positionAlloc4->Add (Vector(455.3864155869187,183.2783219256172, 0.0));
    positionAlloc4->Add (Vector(266.8266724856057,157.2335055929248, 0.0));
    positionAlloc4->Add (Vector(491.2087246464714,133.3193510758176, 0.0));
    positionAlloc4->Add (Vector(409.5445893167654,155.293625897105, 0.0));
    positionAlloc4->Add (Vector(73.39618670862158,132.14874136345034, 0.0));
    positionAlloc4->Add (Vector(59.7285251668645,406.30529401641434, 0.0));
    positionAlloc4->Add (Vector(124.92534011251838,86.71508539356142, 0.0));
    positionAlloc4->Add (Vector(454.6466022721272,200.43686618556046, 0.0));
    positionAlloc4->Add (Vector(355.561229382908,365.90020457114764, 0.0));
    positionAlloc4->Add (Vector(452.9047480041509,494.97758917953684, 0.0));
    positionAlloc4->Add (Vector(93.815836451217,311.24795114996056, 0.0));
    positionAlloc4->Add (Vector(72.77911557919853,78.86503493526664, 0.0));
    positionAlloc4->Add (Vector(397.9834709711347,457.63715995739375, 0.0));
    positionAlloc4->Add (Vector(475.59689274510225,452.701016851389, 0.0));
    positionAlloc4->Add (Vector(269.31364128817347,1.5089783097981813, 0.0));
    positionAlloc4->Add (Vector(309.25678345604274,5.1821306676159695, 0.0));
    positionAlloc4->Add (Vector(386.93717136020933,62.82069056886941, 0.0));
    positionAlloc4->Add (Vector(452.3430801718115,18.530523977017932, 0.0));
    positionAlloc4->Add (Vector(401.5816710570848,201.1962599288679, 0.0));
    positionAlloc4->Add (Vector(239.03625335891547,237.001963281097, 0.0));
    positionAlloc4->Add (Vector(226.20241337322577,225.04337350038057, 0.0));
    positionAlloc4->Add (Vector(197.4252402303197,373.4692478037626, 0.0));
    positionAlloc4->Add (Vector(13.891674424334976,377.4307406952337, 0.0));
    positionAlloc4->Add (Vector(229.2919068542159,56.67096138379524, 0.0));
    positionAlloc4->Add (Vector(9.166321456503645,33.36249881967823, 0.0));
    positionAlloc4->Add (Vector(71.71152433512246,39.19344992747398, 0.0));
    positionAlloc4->Add (Vector(412.00493520615936,313.2480793121469, 0.0));
    positionAlloc4->Add (Vector(422.92574151383883,214.38438749972127, 0.0));
    positionAlloc4->Add (Vector(383.1056905378852,171.22705992175992, 0.0));
    positionAlloc4->Add (Vector(401.48161868686896,348.4002480600892, 0.0));
    positionAlloc4->Add (Vector(54.50938200953415,316.89338030595894, 0.0));
    positionAlloc4->Add (Vector(40.736982438630996,214.30941433189327, 0.0));
    positionAlloc4->Add (Vector(110.71636716174416,133.5360098421829, 0.0));
    positionAlloc4->Add (Vector(232.50074078175302,473.77447069754993, 0.0));
    positionAlloc4->Add (Vector(16.16153331178699,354.69362561564753, 0.0));
    positionAlloc4->Add (Vector(479.7171605308762,401.9804457493234, 0.0));
    positionAlloc4->Add (Vector(255.5707391485641,270.4759024810943, 0.0));
    positionAlloc4->Add (Vector(190.5080620311071,374.9291553630717, 0.0));
    positionAlloc4->Add (Vector(296.71622473231054,421.9202144655944, 0.0));
    positionAlloc4->Add (Vector(148.6656907536361,13.803005977251814, 0.0));
    positionAlloc4->Add (Vector(105.98008232183798,399.3021223801051, 0.0));
    positionAlloc4->Add (Vector(28.464719205547496,254.2581203051198, 0.0));
    positionAlloc4->Add (Vector(375.0862066489964,228.4876637178464, 0.0));
    positionAlloc4->Add (Vector(123.86645088117743,131.08654188645596, 0.0));
    positionAlloc4->Add (Vector(494.8758538318526,101.95311261656093, 0.0));
    positionAlloc4->Add (Vector(220.9521403742117,290.7887036721456, 0.0));
    positionAlloc4->Add (Vector(345.4710710330843,94.31840047765544, 0.0));
    positionAlloc4->Add (Vector(431.1731264190221,346.33885875367713, 0.0));
    positionAlloc4->Add (Vector(15.199844964394416,183.27154862187217, 0.0));
    positionAlloc4->Add (Vector(80.19758975982455,372.81859635374803, 0.0));
    positionAlloc4->Add (Vector(177.85258549194054,13.109283648186643, 0.0));
    positionAlloc4->Add (Vector(263.21296695276067,67.71395153608495, 0.0));
    positionAlloc4->Add (Vector(496.95651230523947,273.0354020714758, 0.0));
    positionAlloc4->Add (Vector(487.3701854275411,318.30220000094863, 0.0));
    positionAlloc4->Add (Vector(450.6542181746369,270.7796894436866, 0.0));
    positionAlloc4->Add (Vector(150.12416976695005,71.50291412904969, 0.0));
    positionAlloc4->Add (Vector(391.37470739208595,495.23589181192034, 0.0));
    positionAlloc4->Add (Vector(398.29433900364376,223.07311600172864, 0.0));
    positionAlloc4->Add (Vector(316.1516587150639,172.64873079554079, 0.0));
    positionAlloc4->Add (Vector(129.84919664917655,128.53464941107333, 0.0));
    positionAlloc4->Add (Vector(385.11936727748997,365.8643003697635, 0.0));
    positionAlloc4->Add (Vector(399.80763086801403,17.285609935817124, 0.0));
    positionAlloc4->Add (Vector(351.7584940763265,305.12040632331485, 0.0));
    positionAlloc4->Add (Vector(49.37170091017418,325.65216617049555, 0.0));
    positionAlloc4->Add (Vector(413.20349881467064,427.05772191826935, 0.0));
    positionAlloc4->Add (Vector(433.3041370832565,474.90299567681325, 0.0));
    positionAlloc4->Add (Vector(422.3548038010348,269.22029629727183, 0.0));
    positionAlloc4->Add (Vector(162.18122986309325,436.4611882000555, 0.0));
    positionAlloc4->Add (Vector(392.64801411080947,11.16521399590309, 0.0));
    positionAlloc4->Add (Vector(111.02456989996135,259.6759121830165, 0.0));
    positionAlloc4->Add (Vector(72.27277006232991,375.7639085676219, 0.0));
    positionAlloc4->Add (Vector(272.00505940696905,327.0686734853722, 0.0));
    positionAlloc4->Add (Vector(76.33582204658163,310.9031158702077, 0.0));
    positionAlloc4->Add (Vector(257.9285084842649,238.5704935248912, 0.0));
    positionAlloc4->Add (Vector(116.83306961962504,50.98362971289872, 0.0));
    positionAlloc4->Add (Vector(207.92435913376366,308.3425335776181, 0.0));
    positionAlloc4->Add (Vector(124.01677918861537,425.8359374681835, 0.0));
    positionAlloc4->Add (Vector(364.29283398729814,104.09721920439574, 0.0));
    positionAlloc4->Add (Vector(96.48136600955642,249.79208533944302, 0.0));
    positionAlloc4->Add (Vector(128.5591468910981,55.4456503720146, 0.0));
    positionAlloc4->Add (Vector(458.15277673417535,454.8177624757785, 0.0));
    positionAlloc4->Add (Vector(296.2559343828984,156.13491885095848, 0.0));
    positionAlloc4->Add (Vector(172.5860255776075,112.5199789070423, 0.0));
    positionAlloc4->Add (Vector(80.48571840780438,233.44001138165316, 0.0));
    positionAlloc4->Add (Vector(344.20662619297167,284.74720637268786, 0.0));
    positionAlloc4->Add (Vector(405.9293488602699,437.48082247794906, 0.0));
    positionAlloc4->Add (Vector(491.4085568652225,90.80642566538188, 0.0));
    positionAlloc4->Add (Vector(188.96208966404993,39.81303884912618, 0.0));
    positionAlloc4->Add (Vector(171.3444539766409,34.10467420835206, 0.0));
    positionAlloc4->Add (Vector(284.4289953523578,232.85493929599045, 0.0));
    positionAlloc4->Add (Vector(259.5058137320279,385.441955250944, 0.0));
    positionAlloc4->Add (Vector(286.24425859580316,366.5712626437556, 0.0));
    positionAlloc4->Add (Vector(263.8573231543733,401.080542002299, 0.0));
    positionAlloc4->Add (Vector(126.18335075229487,427.4489713487012, 0.0));
    positionAlloc4->Add (Vector(331.3599061876311,135.1041310148789, 0.0));
    positionAlloc4->Add (Vector(439.99914428171587,451.920977913186, 0.0));
    positionAlloc4->Add (Vector(171.6731198872115,398.81940197928327, 0.0));
    positionAlloc4->Add (Vector(99.28494421355543,213.3505046740164, 0.0));
    positionAlloc4->Add (Vector(486.90103896362615,338.90044571715555, 0.0));
    positionAlloc4->Add (Vector(310.9596839601507,215.12363541063218, 0.0));
    positionAlloc4->Add (Vector(200.7397917347703,158.69197297913846, 0.0));
    positionAlloc4->Add (Vector(191.05137601575862,205.40567496109279, 0.0));
    positionAlloc4->Add (Vector(335.3943953937936,225.43696820667458, 0.0));
    positionAlloc4->Add (Vector(432.2397150272999,161.34049841837322, 0.0));
    positionAlloc4->Add (Vector(377.54109423384006,349.0286242236515, 0.0));
    positionAlloc4->Add (Vector(375.01088185132994,429.15691821452117, 0.0));
    positionAlloc4->Add (Vector(142.78814084784696,293.41667032804145, 0.0));
    positionAlloc4->Add (Vector(287.35575235405116,314.0380991536752, 0.0));
    positionAlloc4->Add (Vector(302.3580504870265,414.42290399034647, 0.0));
    positionAlloc4->Add (Vector(188.29015737288722,486.89176917916126, 0.0));
    positionAlloc4->Add (Vector(0.20101244567871657,488.37957451552984, 0.0));
    positionAlloc4->Add (Vector(339.5344182827148,459.30088898876295, 0.0));
    positionAlloc4->Add (Vector(26.95463603687648,210.09684000056612, 0.0));
    positionAlloc4->Add (Vector(393.96461691275215,306.01558852246694, 0.0));
    positionAlloc4->Add (Vector(288.92860764226776,92.00510081373253, 0.0));
    positionAlloc4->Add (Vector(157.62240154768648,446.44435426257564, 0.0));
    positionAlloc4->Add (Vector(216.83817449472915,403.6802644424207, 0.0));
    positionAlloc4->Add (Vector(7.766637775417862,296.7407040983186, 0.0));
    positionAlloc4->Add (Vector(245.12676130996385,299.5551538229392, 0.0));
    positionAlloc4->Add (Vector(399.3017955760197,142.85942586707378, 0.0));
    positionAlloc4->Add (Vector(229.94013290840348,273.1734080101866, 0.0));
    positionAlloc4->Add (Vector(422.36722269611096,452.6961593543246, 0.0));
    positionAlloc4->Add (Vector(291.00708997354,439.415992205922, 0.0));
    positionAlloc4->Add (Vector(172.36832634164674,470.0537411666836, 0.0));
    positionAlloc4->Add (Vector(65.49842240545844,404.74534606329104, 0.0));
    positionAlloc4->Add (Vector(88.09812778752763,166.0317871834185, 0.0));
    positionAlloc4->Add (Vector(413.55773558536623,7.509490371061189, 0.0));
    positionAlloc4->Add (Vector(10.762402637098988,473.9851056088405, 0.0));
    positionAlloc4->Add (Vector(67.53957902336566,252.83108283844834, 0.0));
    positionAlloc4->Add (Vector(86.97783334023218,63.16475971981866, 0.0));
    positionAlloc4->Add (Vector(197.43780646177746,481.6312642203559, 0.0));
    positionAlloc4->Add (Vector(463.0907133532268,459.36671781680303, 0.0));
    positionAlloc4->Add (Vector(302.1552414599866,274.7739607542813, 0.0));
    positionAlloc4->Add (Vector(195.00385707062313,242.99533354845488, 0.0));
    positionAlloc4->Add (Vector(457.8031748831373,320.78310447316875, 0.0));
    positionAlloc4->Add (Vector(475.08805962353983,278.3265940975782, 0.0));
    positionAlloc4->Add (Vector(308.3891785008288,474.50816034380824, 0.0));
    positionAlloc4->Add (Vector(6.975786487798508,117.18104306071025, 0.0));
    positionAlloc4->Add (Vector(453.90792625176204,465.98603459841866, 0.0));
    positionAlloc4->Add (Vector(367.5329816443347,386.0890147716234, 0.0));
    positionAlloc4->Add (Vector(131.64838524355548,32.980545342011894, 0.0));
    positionAlloc4->Add (Vector(262.33515456186683,462.40398519967533, 0.0));
    positionAlloc4->Add (Vector(96.71714131166387,319.73044043997004, 0.0));
    positionAlloc4->Add (Vector(130.1575492892705,402.3772818716727, 0.0));
    positionAlloc4->Add (Vector(376.37777686940694,33.26824067705747, 0.0));
    positionAlloc4->Add (Vector(314.85875351078226,105.0870049574198, 0.0));
    positionAlloc4->Add (Vector(33.167417214220784,185.04209895705316, 0.0));
    positionAlloc4->Add (Vector(334.6164467265923,132.4597788314047, 0.0));
    positionAlloc4->Add (Vector(275.4741095589484,372.66721545325106, 0.0));
    positionAlloc4->Add (Vector(289.87260962289844,190.0705863117752, 0.0));
    positionAlloc4->Add (Vector(158.6812046610804,494.3080772062244, 0.0));
    positionAlloc4->Add (Vector(303.16473082666516,284.4257185432407, 0.0));
    positionAlloc4->Add (Vector(35.011071859611164,243.1725554685159, 0.0));
    positionAlloc4->Add (Vector(78.39569732304213,9.288101088704758, 0.0));
    positionAlloc4->Add (Vector(309.97785919068986,414.49044977508936, 0.0));
    positionAlloc4->Add (Vector(119.92387957379307,246.88485713436498, 0.0));
    positionAlloc4->Add (Vector(485.00999454415614,423.41440074501764, 0.0));
    positionAlloc4->Add (Vector(97.7147405546594,290.6794636366289, 0.0));
    positionAlloc4->Add (Vector(126.16287228516171,371.91292703754647, 0.0));
    positionAlloc4->Add (Vector(284.0502309599711,101.64661733049523, 0.0));
    positionAlloc4->Add (Vector(35.98713984474339,483.638165000136, 0.0));
    positionAlloc4->Add (Vector(279.85849102707124,6.277990079557927, 0.0));
    positionAlloc4->Add (Vector(112.85466930392735,356.49449019133834, 0.0));
    positionAlloc4->Add (Vector(22.275939272380864,53.74706455304646, 0.0));
    positionAlloc4->Add (Vector(29.95884475610583,60.67172787036868, 0.0));
    positionAlloc4->Add (Vector(278.12011699520946,68.22761283034251, 0.0));
    positionAlloc4->Add (Vector(431.77092727971433,373.5608213685923, 0.0));
    positionAlloc4->Add (Vector(403.55259780938957,193.93032203208588, 0.0));
    positionAlloc4->Add (Vector(400.31633634030817,116.48713692051021, 0.0));
    positionAlloc4->Add (Vector(280.515109627855,9.32364468647151, 0.0));
    positionAlloc4->Add (Vector(292.87963572914396,484.79787415983725, 0.0));
    positionAlloc4->Add (Vector(62.08665755995557,139.59183950556974, 0.0));
    positionAlloc4->Add (Vector(276.41098934288294,421.01544617980284, 0.0));
    positionAlloc4->Add (Vector(430.013974341444,269.4155321708264, 0.0));
    positionAlloc4->Add (Vector(14.153244010397303,123.10553380152295, 0.0));
    positionAlloc4->Add (Vector(9.940066919897795,13.10549343885964, 0.0));
    positionAlloc4->Add (Vector(355.7623793142359,62.13548098608235, 0.0));
    positionAlloc4->Add (Vector(376.93809423062316,461.51226777324166, 0.0));
    positionAlloc4->Add (Vector(33.000086361031244,377.7315263012332, 0.0));
    positionAlloc4->Add (Vector(466.29773151858177,348.4090807449501, 0.0));
    positionAlloc4->Add (Vector(86.17025417266427,68.56787481443888, 0.0));
    positionAlloc4->Add (Vector(345.448458758462,498.66142522574023, 0.0));
    positionAlloc4->Add (Vector(7.910621423278141,464.71861687188067, 0.0));
    positionAlloc4->Add (Vector(454.2675754598996,311.68005789590137, 0.0));
    positionAlloc4->Add (Vector(442.9710496553873,178.63488000124988, 0.0));
    positionAlloc4->Add (Vector(163.32245088604807,263.52905112880467, 0.0));
    positionAlloc4->Add (Vector(1.435163515579485,308.5724568103619, 0.0));
    positionAlloc4->Add (Vector(451.6897602811269,286.83974333614293, 0.0));
    positionAlloc4->Add (Vector(204.06840138064058,118.51349012151385, 0.0));
    positionAlloc4->Add (Vector(224.95606673997025,289.1948071935659, 0.0));
    positionAlloc4->Add (Vector(57.37298647668759,474.74462935353563, 0.0));
    positionAlloc4->Add (Vector(331.720748909224,310.8478601045609, 0.0));
    positionAlloc4->Add (Vector(214.04559493564744,482.4200235741928, 0.0));
    positionAlloc4->Add (Vector(134.96394588251303,447.9431090980334, 0.0));
    positionAlloc4->Add (Vector(375.4712170136686,174.44917098892125, 0.0));
    positionAlloc4->Add (Vector(441.6530456029049,311.8361035278044, 0.0));
    positionAlloc4->Add (Vector(375.40605156807777,362.99899267522574, 0.0));
    positionAlloc4->Add (Vector(463.754290198017,173.88292987275327, 0.0));
    positionAlloc4->Add (Vector(198.83841849276678,82.67709855846638, 0.0));
    positionAlloc4->Add (Vector(69.63817362537927,403.6956443547619, 0.0));
    positionAlloc4->Add (Vector(451.70095764394176,68.73735207311876, 0.0));
    positionAlloc4->Add (Vector(472.2973779954067,293.2775202509965, 0.0));
    positionAlloc4->Add (Vector(331.8973226098944,257.4445560291543, 0.0));
    positionAlloc4->Add (Vector(24.97672947304358,267.94820295775577, 0.0));
    positionAlloc4->Add (Vector(347.20007886387253,207.08963476345133, 0.0));
    positionAlloc4->Add (Vector(51.16721441391292,207.02799390978416, 0.0));
    positionAlloc4->Add (Vector(294.65276845164215,349.8791800104656, 0.0));
    positionAlloc4->Add (Vector(287.05880274600656,73.36428745290507, 0.0));
    positionAlloc4->Add (Vector(245.78657964016915,26.681272558540194, 0.0));
    positionAlloc4->Add (Vector(105.81405800002952,132.7733296861131, 0.0));
    positionAlloc4->Add (Vector(9.683478935148537,339.4177664699455, 0.0));
    positionAlloc4->Add (Vector(143.88766929317438,65.01428605913884, 0.0));
    positionAlloc4->Add (Vector(454.2977515465478,146.80707418683974, 0.0));
    positionAlloc4->Add (Vector(51.61300328882101,223.94676308795258, 0.0));
    positionAlloc4->Add (Vector(140.22199603220258,394.63966422574424, 0.0));
    positionAlloc4->Add (Vector(494.43054445324736,374.0828271899197, 0.0));
    positionAlloc4->Add (Vector(9.144138672095902,375.07215747248375, 0.0));
    positionAlloc4->Add (Vector(343.25046384079184,417.3128359486865, 0.0));
    positionAlloc4->Add (Vector(345.9385569752367,157.75781550303148, 0.0));
    positionAlloc4->Add (Vector(478.944765075251,266.5826424865085, 0.0));
    positionAlloc4->Add (Vector(49.17341691652505,210.55381250252609, 0.0));
    positionAlloc4->Add (Vector(84.91520978228445,439.0712517147066, 0.0));
    positionAlloc4->Add (Vector(42.522105684888956,19.527391616441182, 0.0));
    positionAlloc4->Add (Vector(438.19457614801917,447.3033317519237, 0.0));
    positionAlloc4->Add (Vector(156.71208907962142,346.16130783465707, 0.0));
    positionAlloc4->Add (Vector(400.37228433776835,484.1307878596988, 0.0));
    positionAlloc4->Add (Vector(70.19346929761689,99.05074454243939, 0.0));
    positionAlloc4->Add (Vector(208.65240118356348,279.34491422287584, 0.0));
    positionAlloc4->Add (Vector(13.693796598963083,335.2337550892011, 0.0));
    positionAlloc4->Add (Vector(102.22612486575872,439.0587181954727, 0.0));
    positionAlloc4->Add (Vector(209.5972572016474,342.60975019837974, 0.0));
    positionAlloc4->Add (Vector(198.38373711533498,269.40836700167847, 0.0));
    positionAlloc4->Add (Vector(93.13010568883546,172.78036352152387, 0.0));
    positionAlloc4->Add (Vector(73.37794540855653,46.1692973843989, 0.0));
    positionAlloc4->Add (Vector(0.05718740867244332,151.16628631591988, 0.0));
    positionAlloc4->Add (Vector(208.511002351287,360.16224672107904, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_1.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_ECC_flowmon/switch_ECC_1.flowmon", false, false);
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