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
	  uint16_t numberOfRrhs = 38;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 576;
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
    positionAlloc->Add (Vector (444.44444444444446,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,500.0, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,0.0, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (0.0,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (0.0,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (0.0,222.22222222222223, 0.0));
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
    positionAlloc4->Add (Vector(364.4462892973591,250.6578599629262, 0.0));
    positionAlloc4->Add (Vector(142.89474290562666,439.82684930812474, 0.0));
    positionAlloc4->Add (Vector(316.610939233921,82.44917364693049, 0.0));
    positionAlloc4->Add (Vector(302.0692833837456,334.30808082781186, 0.0));
    positionAlloc4->Add (Vector(316.2152329797235,459.5368971426121, 0.0));
    positionAlloc4->Add (Vector(143.73293372083663,319.0122444737978, 0.0));
    positionAlloc4->Add (Vector(391.47710019122457,312.75197473226325, 0.0));
    positionAlloc4->Add (Vector(330.71526655118214,168.78416508814803, 0.0));
    positionAlloc4->Add (Vector(218.1625865952445,379.15404512585485, 0.0));
    positionAlloc4->Add (Vector(45.40320777428958,451.8739224648307, 0.0));
    positionAlloc4->Add (Vector(418.83706520121063,488.8102511207502, 0.0));
    positionAlloc4->Add (Vector(209.74509754452936,26.150587110773614, 0.0));
    positionAlloc4->Add (Vector(147.60231996036455,305.35429850974054, 0.0));
    positionAlloc4->Add (Vector(104.66603337824165,392.9483240866218, 0.0));
    positionAlloc4->Add (Vector(428.77831287139475,80.05781486972514, 0.0));
    positionAlloc4->Add (Vector(232.34053587798022,232.98920063597873, 0.0));
    positionAlloc4->Add (Vector(120.78909565838819,26.351779041470515, 0.0));
    positionAlloc4->Add (Vector(368.1295603956308,477.5312573214251, 0.0));
    positionAlloc4->Add (Vector(160.38492109599966,438.53962363731904, 0.0));
    positionAlloc4->Add (Vector(125.5806733952654,305.96944799695535, 0.0));
    positionAlloc4->Add (Vector(275.75018371079426,412.10027791656165, 0.0));
    positionAlloc4->Add (Vector(174.5012453594267,148.11211219606136, 0.0));
    positionAlloc4->Add (Vector(251.26370249856134,254.6724139692022, 0.0));
    positionAlloc4->Add (Vector(277.021236039591,311.14396053020585, 0.0));
    positionAlloc4->Add (Vector(27.31812999817268,412.3870308999328, 0.0));
    positionAlloc4->Add (Vector(222.07033285404486,356.51413464879533, 0.0));
    positionAlloc4->Add (Vector(152.99496054238287,299.75319605895885, 0.0));
    positionAlloc4->Add (Vector(273.009035518247,238.55672057236683, 0.0));
    positionAlloc4->Add (Vector(307.26630756622626,64.44080317336903, 0.0));
    positionAlloc4->Add (Vector(160.37887711998212,476.2687527811402, 0.0));
    positionAlloc4->Add (Vector(451.2561257690104,277.61361571237063, 0.0));
    positionAlloc4->Add (Vector(470.31255492034086,118.45760516402348, 0.0));
    positionAlloc4->Add (Vector(492.16997973074916,222.4451454975457, 0.0));
    positionAlloc4->Add (Vector(249.38435733885532,252.29602670478062, 0.0));
    positionAlloc4->Add (Vector(311.1376167149836,302.36934744579287, 0.0));
    positionAlloc4->Add (Vector(395.95236193302316,480.42957446887254, 0.0));
    positionAlloc4->Add (Vector(355.60192515806887,303.82872045570497, 0.0));
    positionAlloc4->Add (Vector(229.50717563340612,392.5358646276354, 0.0));
    positionAlloc4->Add (Vector(351.0389605079299,282.8665569999258, 0.0));
    positionAlloc4->Add (Vector(253.55400348286938,66.1057696202047, 0.0));
    positionAlloc4->Add (Vector(4.211125769122748,357.5733705294224, 0.0));
    positionAlloc4->Add (Vector(204.27945351682186,383.3076365899628, 0.0));
    positionAlloc4->Add (Vector(353.07516481181983,137.26401149280636, 0.0));
    positionAlloc4->Add (Vector(452.881223680698,244.09673883638044, 0.0));
    positionAlloc4->Add (Vector(162.81730507097302,2.9347659297506157, 0.0));
    positionAlloc4->Add (Vector(443.3630396534918,416.28269918842216, 0.0));
    positionAlloc4->Add (Vector(355.4410391397767,129.4181019192927, 0.0));
    positionAlloc4->Add (Vector(294.9608949932489,8.586476828303546, 0.0));
    positionAlloc4->Add (Vector(366.95518818570264,124.75777828616985, 0.0));
    positionAlloc4->Add (Vector(54.647972467497695,382.5336272624167, 0.0));
    positionAlloc4->Add (Vector(240.29893177639073,277.48914924728507, 0.0));
    positionAlloc4->Add (Vector(361.8729696190429,474.18723511065264, 0.0));
    positionAlloc4->Add (Vector(394.73682750819535,348.1627709091732, 0.0));
    positionAlloc4->Add (Vector(201.45943459901682,390.88792458638346, 0.0));
    positionAlloc4->Add (Vector(280.5522883825956,390.4365760871936, 0.0));
    positionAlloc4->Add (Vector(132.47347435202312,384.88310563992684, 0.0));
    positionAlloc4->Add (Vector(368.9031483429973,422.7491088057598, 0.0));
    positionAlloc4->Add (Vector(169.61564715256756,377.14155988145677, 0.0));
    positionAlloc4->Add (Vector(465.86219957110745,286.71961172702856, 0.0));
    positionAlloc4->Add (Vector(337.8027751819931,276.2114689853149, 0.0));
    positionAlloc4->Add (Vector(455.7389035948415,168.49570150361703, 0.0));
    positionAlloc4->Add (Vector(203.6911173116509,483.5027371339864, 0.0));
    positionAlloc4->Add (Vector(188.33894118077654,84.5836581033222, 0.0));
    positionAlloc4->Add (Vector(179.87146670473481,103.29339731043974, 0.0));
    positionAlloc4->Add (Vector(219.34917823551058,55.15563654960259, 0.0));
    positionAlloc4->Add (Vector(113.15424589380963,419.1623118407263, 0.0));
    positionAlloc4->Add (Vector(401.10821826683576,495.58172019235917, 0.0));
    positionAlloc4->Add (Vector(249.9151246437632,223.02228214953135, 0.0));
    positionAlloc4->Add (Vector(265.3425863604772,423.2699807970414, 0.0));
    positionAlloc4->Add (Vector(192.8932891663514,212.40303087880054, 0.0));
    positionAlloc4->Add (Vector(481.65551884135385,88.36108386893893, 0.0));
    positionAlloc4->Add (Vector(229.05885651382707,393.21992345781257, 0.0));
    positionAlloc4->Add (Vector(349.73319137515625,384.18338335330327, 0.0));
    positionAlloc4->Add (Vector(207.49483943259995,78.84067460541495, 0.0));
    positionAlloc4->Add (Vector(115.05146628622187,185.22049164972702, 0.0));
    positionAlloc4->Add (Vector(156.39228413408384,165.271846324978, 0.0));
    positionAlloc4->Add (Vector(279.8616057584826,215.10724855796587, 0.0));
    positionAlloc4->Add (Vector(16.794710031978465,276.54135630923827, 0.0));
    positionAlloc4->Add (Vector(160.0239008239973,456.9200326489894, 0.0));
    positionAlloc4->Add (Vector(23.835153756878057,197.9545616186773, 0.0));
    positionAlloc4->Add (Vector(472.5145837935238,215.5983920965266, 0.0));
    positionAlloc4->Add (Vector(420.23033924996724,366.583765392949, 0.0));
    positionAlloc4->Add (Vector(467.69054990535903,161.99158883800658, 0.0));
    positionAlloc4->Add (Vector(101.49937405075104,56.12221920643739, 0.0));
    positionAlloc4->Add (Vector(450.7316500866026,260.48937197820686, 0.0));
    positionAlloc4->Add (Vector(148.97059975829796,190.63353974140097, 0.0));
    positionAlloc4->Add (Vector(317.9546887849379,178.06394488399025, 0.0));
    positionAlloc4->Add (Vector(177.9044667107232,416.96101137229743, 0.0));
    positionAlloc4->Add (Vector(99.62062943967115,74.03219608276845, 0.0));
    positionAlloc4->Add (Vector(216.4843435363319,407.4318009766855, 0.0));
    positionAlloc4->Add (Vector(109.78963184185048,148.41294329217146, 0.0));
    positionAlloc4->Add (Vector(137.4889392070083,188.09012986030842, 0.0));
    positionAlloc4->Add (Vector(287.70003839392507,364.8783412203525, 0.0));
    positionAlloc4->Add (Vector(185.74904804574345,99.16583074369912, 0.0));
    positionAlloc4->Add (Vector(0.5626762482694447,410.7016731402653, 0.0));
    positionAlloc4->Add (Vector(299.1915215038656,132.80851127912112, 0.0));
    positionAlloc4->Add (Vector(301.7954428313962,407.96036217420806, 0.0));
    positionAlloc4->Add (Vector(128.54358011208856,353.3363596191504, 0.0));
    positionAlloc4->Add (Vector(260.9936357544965,370.6742534046232, 0.0));
    positionAlloc4->Add (Vector(76.46130040360194,301.4979216386523, 0.0));
    positionAlloc4->Add (Vector(454.52134797771043,395.1727795409627, 0.0));
    positionAlloc4->Add (Vector(490.27979353629826,355.64255553131926, 0.0));
    positionAlloc4->Add (Vector(215.61045781369964,140.08819775702202, 0.0));
    positionAlloc4->Add (Vector(72.1405551971474,403.80016343270387, 0.0));
    positionAlloc4->Add (Vector(477.6431465266309,274.42864644284003, 0.0));
    positionAlloc4->Add (Vector(173.9635134899385,297.23008205664104, 0.0));
    positionAlloc4->Add (Vector(252.1873159985007,352.330104814338, 0.0));
    positionAlloc4->Add (Vector(222.83176703253181,459.17578754573, 0.0));
    positionAlloc4->Add (Vector(198.15584633595634,350.0632079649667, 0.0));
    positionAlloc4->Add (Vector(314.62232134926904,487.80669982462, 0.0));
    positionAlloc4->Add (Vector(236.5238001775839,19.96445669236424, 0.0));
    positionAlloc4->Add (Vector(494.8858953296745,472.4636564903456, 0.0));
    positionAlloc4->Add (Vector(312.97534183281226,340.89459992379835, 0.0));
    positionAlloc4->Add (Vector(293.95703399441004,30.672355631508996, 0.0));
    positionAlloc4->Add (Vector(342.9134798420413,82.1997918843786, 0.0));
    positionAlloc4->Add (Vector(0.9185210913262853,253.32686047260682, 0.0));
    positionAlloc4->Add (Vector(243.2501190799693,278.3489771425845, 0.0));
    positionAlloc4->Add (Vector(137.29072818810988,308.4405705447394, 0.0));
    positionAlloc4->Add (Vector(341.54535850834446,181.0198776490539, 0.0));
    positionAlloc4->Add (Vector(153.15906157564186,460.33868284847404, 0.0));
    positionAlloc4->Add (Vector(440.1288515018188,65.33320967129785, 0.0));
    positionAlloc4->Add (Vector(292.3331159644562,123.91437585743198, 0.0));
    positionAlloc4->Add (Vector(61.5505858712716,86.92567641903254, 0.0));
    positionAlloc4->Add (Vector(28.166450954063617,106.26578348470328, 0.0));
    positionAlloc4->Add (Vector(297.32692916524434,359.6012913531397, 0.0));
    positionAlloc4->Add (Vector(19.237376152008657,25.64834351459333, 0.0));
    positionAlloc4->Add (Vector(381.9273973461086,59.029653607377995, 0.0));
    positionAlloc4->Add (Vector(495.68527960018554,64.59374058480289, 0.0));
    positionAlloc4->Add (Vector(270.68796952225756,416.81270985180737, 0.0));
    positionAlloc4->Add (Vector(345.18699235297225,403.4571074694842, 0.0));
    positionAlloc4->Add (Vector(415.37108112012373,308.2600037652286, 0.0));
    positionAlloc4->Add (Vector(48.118898701445644,318.538784472012, 0.0));
    positionAlloc4->Add (Vector(393.13695766020726,66.06824993940647, 0.0));
    positionAlloc4->Add (Vector(340.60384088000717,321.7497927266774, 0.0));
    positionAlloc4->Add (Vector(293.690414894372,487.1084482529986, 0.0));
    positionAlloc4->Add (Vector(318.39019676950016,375.9443421846659, 0.0));
    positionAlloc4->Add (Vector(437.77997637616176,196.62896611620778, 0.0));
    positionAlloc4->Add (Vector(74.28567720215656,86.42217031449023, 0.0));
    positionAlloc4->Add (Vector(391.02688392664277,224.8948850423334, 0.0));
    positionAlloc4->Add (Vector(24.86770935521665,6.953459961862807, 0.0));
    positionAlloc4->Add (Vector(364.0293621953318,72.46834789251244, 0.0));
    positionAlloc4->Add (Vector(487.14054697915475,6.912605442426445, 0.0));
    positionAlloc4->Add (Vector(421.91368954686004,47.76157056762182, 0.0));
    positionAlloc4->Add (Vector(436.223970841042,262.47236715156174, 0.0));
    positionAlloc4->Add (Vector(116.79828635538159,192.00046365254425, 0.0));
    positionAlloc4->Add (Vector(187.11707170463694,253.97303465959436, 0.0));
    positionAlloc4->Add (Vector(209.83237497759438,380.8385712434715, 0.0));
    positionAlloc4->Add (Vector(426.42136965143476,469.4761158823615, 0.0));
    positionAlloc4->Add (Vector(152.49904787042362,261.04389675239054, 0.0));
    positionAlloc4->Add (Vector(227.6829883427039,33.10479383404458, 0.0));
    positionAlloc4->Add (Vector(464.8549588003854,373.78365358057164, 0.0));
    positionAlloc4->Add (Vector(23.03992453271686,350.49569652396826, 0.0));
    positionAlloc4->Add (Vector(384.5311466258384,251.78628535298375, 0.0));
    positionAlloc4->Add (Vector(53.06123570389965,71.03564388452487, 0.0));
    positionAlloc4->Add (Vector(304.6266158663972,264.46009158440404, 0.0));
    positionAlloc4->Add (Vector(65.18891187408532,474.8217537498196, 0.0));
    positionAlloc4->Add (Vector(174.94753510963855,38.135151175706596, 0.0));
    positionAlloc4->Add (Vector(373.77242258501457,342.9763423479319, 0.0));
    positionAlloc4->Add (Vector(281.49661122282197,2.898119859855941, 0.0));
    positionAlloc4->Add (Vector(417.2860572010357,39.244059091924136, 0.0));
    positionAlloc4->Add (Vector(269.76977911163226,339.0568677256662, 0.0));
    positionAlloc4->Add (Vector(18.633651312706167,218.63896504089425, 0.0));
    positionAlloc4->Add (Vector(424.18304483276654,323.61332655675466, 0.0));
    positionAlloc4->Add (Vector(213.08936309886812,94.81110612817112, 0.0));
    positionAlloc4->Add (Vector(259.1335895909219,468.0002817789776, 0.0));
    positionAlloc4->Add (Vector(390.799495320388,470.1056394459839, 0.0));
    positionAlloc4->Add (Vector(47.597306624472814,458.95429787575654, 0.0));
    positionAlloc4->Add (Vector(459.78540984102426,17.7334919229884, 0.0));
    positionAlloc4->Add (Vector(166.13164896910416,499.91711130396595, 0.0));
    positionAlloc4->Add (Vector(249.36149989095784,113.51872847940436, 0.0));
    positionAlloc4->Add (Vector(306.4503238287305,7.43338997123455, 0.0));
    positionAlloc4->Add (Vector(6.518789868278086,329.8794916203896, 0.0));
    positionAlloc4->Add (Vector(172.52638416446487,243.11074029122577, 0.0));
    positionAlloc4->Add (Vector(408.7433700434439,160.68510879547122, 0.0));
    positionAlloc4->Add (Vector(69.15337409981586,495.8565629847199, 0.0));
    positionAlloc4->Add (Vector(146.88697319786658,43.227290252317644, 0.0));
    positionAlloc4->Add (Vector(499.62413410852713,440.7233752418736, 0.0));
    positionAlloc4->Add (Vector(131.65668436184797,372.5354183753237, 0.0));
    positionAlloc4->Add (Vector(377.83646913204285,35.97951346432071, 0.0));
    positionAlloc4->Add (Vector(337.8313478722405,331.0038646129582, 0.0));
    positionAlloc4->Add (Vector(128.20675580074453,56.316187206736586, 0.0));
    positionAlloc4->Add (Vector(448.33447891727394,137.7699920693821, 0.0));
    positionAlloc4->Add (Vector(196.0822937754802,117.16320952283476, 0.0));
    positionAlloc4->Add (Vector(71.34112623811933,353.8367861499346, 0.0));
    positionAlloc4->Add (Vector(73.08520609005653,491.1886465550145, 0.0));
    positionAlloc4->Add (Vector(53.154876435924926,160.7697441522854, 0.0));
    positionAlloc4->Add (Vector(270.9580629759492,38.5710225215693, 0.0));
    positionAlloc4->Add (Vector(157.56800056695224,336.72866371084586, 0.0));
    positionAlloc4->Add (Vector(167.82645310588745,444.8571850633248, 0.0));
    positionAlloc4->Add (Vector(119.2444841016096,430.961879285046, 0.0));
    positionAlloc4->Add (Vector(347.59862490854886,436.0114694758087, 0.0));
    positionAlloc4->Add (Vector(438.8839976335472,290.2166511342714, 0.0));
    positionAlloc4->Add (Vector(388.11843653757035,444.0410690673882, 0.0));
    positionAlloc4->Add (Vector(491.58423599588156,116.33337391430942, 0.0));
    positionAlloc4->Add (Vector(277.35409439244364,479.8071332796685, 0.0));
    positionAlloc4->Add (Vector(490.5744297055334,273.2935327422038, 0.0));
    positionAlloc4->Add (Vector(438.19120269536427,337.42403438643294, 0.0));
    positionAlloc4->Add (Vector(357.05093382489065,404.352717634885, 0.0));
    positionAlloc4->Add (Vector(143.43761078883037,362.42253122188606, 0.0));
    positionAlloc4->Add (Vector(273.53413212111064,446.35550021171474, 0.0));
    positionAlloc4->Add (Vector(10.307350728113784,194.565014509059, 0.0));
    positionAlloc4->Add (Vector(409.65056459820755,453.7914440672975, 0.0));
    positionAlloc4->Add (Vector(182.74832163168813,80.47781031724205, 0.0));
    positionAlloc4->Add (Vector(333.3846902830723,498.8558424886893, 0.0));
    positionAlloc4->Add (Vector(353.6878339595516,247.3983278447509, 0.0));
    positionAlloc4->Add (Vector(432.8783345131457,452.7162141935565, 0.0));
    positionAlloc4->Add (Vector(42.653350439145534,158.43510181192582, 0.0));
    positionAlloc4->Add (Vector(351.19103925840545,445.0123089741956, 0.0));
    positionAlloc4->Add (Vector(490.8517034742634,173.70507135723733, 0.0));
    positionAlloc4->Add (Vector(277.15266308389016,97.3338578563912, 0.0));
    positionAlloc4->Add (Vector(370.2419607807733,385.0088995226114, 0.0));
    positionAlloc4->Add (Vector(157.44243268963442,51.42643664053054, 0.0));
    positionAlloc4->Add (Vector(271.44277745135696,483.1373360359985, 0.0));
    positionAlloc4->Add (Vector(421.3564935758953,159.95497661190905, 0.0));
    positionAlloc4->Add (Vector(479.6284657332206,27.194001810159907, 0.0));
    positionAlloc4->Add (Vector(230.9803792566772,435.9407667816418, 0.0));
    positionAlloc4->Add (Vector(256.83235449010976,243.45291581702617, 0.0));
    positionAlloc4->Add (Vector(272.9896097745396,231.3936392435569, 0.0));
    positionAlloc4->Add (Vector(429.771033805051,46.73545257729311, 0.0));
    positionAlloc4->Add (Vector(473.36646775795424,390.86184448919977, 0.0));
    positionAlloc4->Add (Vector(337.9240463168062,324.8793265126645, 0.0));
    positionAlloc4->Add (Vector(229.53557075488544,431.3629046027771, 0.0));
    positionAlloc4->Add (Vector(319.14865310811166,61.58719704181098, 0.0));
    positionAlloc4->Add (Vector(45.925530632612734,238.2378017226667, 0.0));
    positionAlloc4->Add (Vector(452.78037076389296,26.00424981223126, 0.0));
    positionAlloc4->Add (Vector(462.53441476557055,388.6978281200853, 0.0));
    positionAlloc4->Add (Vector(172.23892810035363,0.9834376043203474, 0.0));
    positionAlloc4->Add (Vector(79.25978533315758,485.417769807133, 0.0));
    positionAlloc4->Add (Vector(95.87805331000465,50.75260890186717, 0.0));
    positionAlloc4->Add (Vector(403.3669402887579,126.06268517496355, 0.0));
    positionAlloc4->Add (Vector(176.84328513519577,132.27129807828254, 0.0));
    positionAlloc4->Add (Vector(298.1902509210566,96.86629555426774, 0.0));
    positionAlloc4->Add (Vector(165.05109009475245,250.4688018079414, 0.0));
    positionAlloc4->Add (Vector(72.45162315467063,360.822558603692, 0.0));
    positionAlloc4->Add (Vector(121.05237997506474,103.5497490234863, 0.0));
    positionAlloc4->Add (Vector(403.07380939256467,424.9467675502842, 0.0));
    positionAlloc4->Add (Vector(43.389955923155696,237.6377804317927, 0.0));
    positionAlloc4->Add (Vector(149.12712765163562,236.91521643888237, 0.0));
    positionAlloc4->Add (Vector(125.34702935768455,383.5209744523288, 0.0));
    positionAlloc4->Add (Vector(398.9499120982288,409.082517847053, 0.0));
    positionAlloc4->Add (Vector(67.04126922453835,64.15569330346909, 0.0));
    positionAlloc4->Add (Vector(137.87494853669236,6.391552568849601, 0.0));
    positionAlloc4->Add (Vector(170.60672238232434,326.9715349955091, 0.0));
    positionAlloc4->Add (Vector(453.75712874095586,288.65809750067115, 0.0));
    positionAlloc4->Add (Vector(433.0429570193994,403.71254999200215, 0.0));
    positionAlloc4->Add (Vector(210.95399510212408,297.4759864660899, 0.0));
    positionAlloc4->Add (Vector(428.0222861603677,65.70054204161907, 0.0));
    positionAlloc4->Add (Vector(63.31460873084649,44.76189230300365, 0.0));
    positionAlloc4->Add (Vector(49.163602157868965,426.17338944665016, 0.0));
    positionAlloc4->Add (Vector(190.33737229222325,211.68052097939056, 0.0));
    positionAlloc4->Add (Vector(62.993456571093844,191.44084259424403, 0.0));
    positionAlloc4->Add (Vector(105.18489694503191,405.9144209497048, 0.0));
    positionAlloc4->Add (Vector(303.9266505534116,266.37893861122257, 0.0));
    positionAlloc4->Add (Vector(17.295403742396577,87.91187497910535, 0.0));
    positionAlloc4->Add (Vector(350.75921798172385,468.52370128168116, 0.0));
    positionAlloc4->Add (Vector(350.4947424803444,134.7427835933603, 0.0));
    positionAlloc4->Add (Vector(437.87571277863793,277.9140718933721, 0.0));
    positionAlloc4->Add (Vector(309.5900643323689,383.61487113073093, 0.0));
    positionAlloc4->Add (Vector(202.0251485853039,40.67563753620601, 0.0));
    positionAlloc4->Add (Vector(22.236317827357855,31.39645201447849, 0.0));
    positionAlloc4->Add (Vector(465.68218946692724,428.7458055665934, 0.0));
    positionAlloc4->Add (Vector(217.683114563131,463.00175697102276, 0.0));
    positionAlloc4->Add (Vector(185.55977922302404,403.8915576436109, 0.0));
    positionAlloc4->Add (Vector(62.67356790543749,114.61471167164794, 0.0));
    positionAlloc4->Add (Vector(163.415636084772,378.0530997010172, 0.0));
    positionAlloc4->Add (Vector(384.2477461875247,258.63043803217704, 0.0));
    positionAlloc4->Add (Vector(319.4140296705325,99.10512586144415, 0.0));
    positionAlloc4->Add (Vector(460.83915003030455,395.2466311311233, 0.0));
    positionAlloc4->Add (Vector(139.9418123506515,274.67193620196633, 0.0));
    positionAlloc4->Add (Vector(154.38154958187062,240.07474622928814, 0.0));
    positionAlloc4->Add (Vector(75.074985258877,474.57284384671607, 0.0));
    positionAlloc4->Add (Vector(188.6241098507222,165.99580998180946, 0.0));
    positionAlloc4->Add (Vector(68.91587415761963,125.39378669388601, 0.0));
    positionAlloc4->Add (Vector(299.20419460883977,461.4883701394793, 0.0));
    positionAlloc4->Add (Vector(295.0126956108598,287.55905555723444, 0.0));
    positionAlloc4->Add (Vector(247.73532414795213,192.60425542502284, 0.0));
    positionAlloc4->Add (Vector(370.6636271379731,234.59654245206363, 0.0));
    positionAlloc4->Add (Vector(128.24078631661206,251.69292685007076, 0.0));
    positionAlloc4->Add (Vector(427.60821892290903,345.1422363616745, 0.0));
    positionAlloc4->Add (Vector(368.2232454403845,363.2297813331412, 0.0));
    positionAlloc4->Add (Vector(472.309405284754,416.79105326261384, 0.0));
    positionAlloc4->Add (Vector(362.5043487507546,291.88539248520755, 0.0));
    positionAlloc4->Add (Vector(5.0882937019467995,210.94978801221376, 0.0));
    positionAlloc4->Add (Vector(160.35303527011357,45.38853198496984, 0.0));
    positionAlloc4->Add (Vector(149.42615883245398,342.47023476177446, 0.0));
    positionAlloc4->Add (Vector(132.6365635726906,363.89230388870743, 0.0));
    positionAlloc4->Add (Vector(295.61647028736076,266.9922055180335, 0.0));
    positionAlloc4->Add (Vector(5.6444322668646745,434.0570510463006, 0.0));
    positionAlloc4->Add (Vector(25.73761633922883,459.8476000876798, 0.0));
    positionAlloc4->Add (Vector(180.68120253567022,421.69594862992994, 0.0));
    positionAlloc4->Add (Vector(2.2475153405712156,33.93585918538178, 0.0));
    positionAlloc4->Add (Vector(82.555163360223,271.40418918756484, 0.0));
    positionAlloc4->Add (Vector(472.25375747489784,260.2851615822374, 0.0));
    positionAlloc4->Add (Vector(156.15770649139066,364.92379413189207, 0.0));
    positionAlloc4->Add (Vector(196.60307969521372,251.62078936526967, 0.0));
    positionAlloc4->Add (Vector(437.7778783262858,31.991091013247598, 0.0));
    positionAlloc4->Add (Vector(108.57681363422844,240.07271662361563, 0.0));
    positionAlloc4->Add (Vector(354.89439565420315,496.7652107534409, 0.0));
    positionAlloc4->Add (Vector(101.65564549944361,117.13982195018785, 0.0));
    positionAlloc4->Add (Vector(416.6010992761853,244.41739227908022, 0.0));
    positionAlloc4->Add (Vector(415.477476135207,58.13587195890785, 0.0));
    positionAlloc4->Add (Vector(161.97183998788628,338.47122740335743, 0.0));
    positionAlloc4->Add (Vector(395.52846829130186,285.73238390945266, 0.0));
    positionAlloc4->Add (Vector(14.844169360921756,272.1274835534956, 0.0));
    positionAlloc4->Add (Vector(429.7640402083889,395.07870063340243, 0.0));
    positionAlloc4->Add (Vector(309.02196499733157,387.8626562593221, 0.0));
    positionAlloc4->Add (Vector(153.07820516600574,160.52353498215584, 0.0));
    positionAlloc4->Add (Vector(400.02836188343315,273.49745809279, 0.0));
    positionAlloc4->Add (Vector(254.54710036182826,150.56658880667112, 0.0));
    positionAlloc4->Add (Vector(162.20709621702866,313.6134105763446, 0.0));
    positionAlloc4->Add (Vector(395.6812370147141,312.42441530364187, 0.0));
    positionAlloc4->Add (Vector(460.3564660807082,448.397866058862, 0.0));
    positionAlloc4->Add (Vector(239.95955614245645,150.6148910454408, 0.0));
    positionAlloc4->Add (Vector(111.45761599771298,6.052900927927207, 0.0));
    positionAlloc4->Add (Vector(277.5972747965655,401.6833993250749, 0.0));
    positionAlloc4->Add (Vector(210.3544365397186,29.830814902535753, 0.0));
    positionAlloc4->Add (Vector(344.4400861100933,87.03570363473406, 0.0));
    positionAlloc4->Add (Vector(69.64770371494345,145.93965761334243, 0.0));
    positionAlloc4->Add (Vector(442.0416029067869,378.17677495094244, 0.0));
    positionAlloc4->Add (Vector(149.8204955455585,33.786722797700506, 0.0));
    positionAlloc4->Add (Vector(56.62335296370857,466.8306219911587, 0.0));
    positionAlloc4->Add (Vector(25.771720369129227,495.8172441281823, 0.0));
    positionAlloc4->Add (Vector(372.7901479715159,471.87641777926655, 0.0));
    positionAlloc4->Add (Vector(407.9646751022421,12.977187075715769, 0.0));
    positionAlloc4->Add (Vector(309.85073618091815,420.667119328619, 0.0));
    positionAlloc4->Add (Vector(241.98973305188105,285.62918720242703, 0.0));
    positionAlloc4->Add (Vector(414.34237081950977,189.76567694576642, 0.0));
    positionAlloc4->Add (Vector(124.80958075816245,62.860170851864794, 0.0));
    positionAlloc4->Add (Vector(320.463459453153,277.7302461344621, 0.0));
    positionAlloc4->Add (Vector(363.4640223517759,476.17338745225834, 0.0));
    positionAlloc4->Add (Vector(27.289052074927934,309.2597674225622, 0.0));
    positionAlloc4->Add (Vector(18.562031100822495,352.02343723168354, 0.0));
    positionAlloc4->Add (Vector(338.7663955249585,393.19419167290675, 0.0));
    positionAlloc4->Add (Vector(175.8848798221974,122.35403945580391, 0.0));
    positionAlloc4->Add (Vector(104.83460483775686,399.63821691097894, 0.0));
    positionAlloc4->Add (Vector(161.57698658029267,387.1637920881678, 0.0));
    positionAlloc4->Add (Vector(23.822972505735617,423.6330105889707, 0.0));
    positionAlloc4->Add (Vector(169.0855400462332,343.88291314095085, 0.0));
    positionAlloc4->Add (Vector(50.68643774210957,367.45457047466203, 0.0));
    positionAlloc4->Add (Vector(49.08732860939846,370.6250285981667, 0.0));
    positionAlloc4->Add (Vector(172.78927090464225,49.40335021642422, 0.0));
    positionAlloc4->Add (Vector(473.9800835928515,440.9141048988966, 0.0));
    positionAlloc4->Add (Vector(2.541991010775624,394.9622502790392, 0.0));
    positionAlloc4->Add (Vector(207.17168705155441,36.479742112402626, 0.0));
    positionAlloc4->Add (Vector(340.91918785540054,494.5575661627543, 0.0));
    positionAlloc4->Add (Vector(430.26873979863285,182.22106092085406, 0.0));
    positionAlloc4->Add (Vector(97.83001252671781,203.70452357883917, 0.0));
    positionAlloc4->Add (Vector(60.82419130599037,474.2189566023973, 0.0));
    positionAlloc4->Add (Vector(334.26178499119396,399.4047373030591, 0.0));
    positionAlloc4->Add (Vector(405.71304247843324,320.4996761640736, 0.0));
    positionAlloc4->Add (Vector(447.1963542362795,447.93251556756263, 0.0));
    positionAlloc4->Add (Vector(267.8369399180991,159.09561999375805, 0.0));
    positionAlloc4->Add (Vector(165.88558827742932,168.24641733926865, 0.0));
    positionAlloc4->Add (Vector(257.1565628424734,498.07526392723247, 0.0));
    positionAlloc4->Add (Vector(393.564938671034,382.01346178927963, 0.0));
    positionAlloc4->Add (Vector(142.7246655029857,386.6126199908837, 0.0));
    positionAlloc4->Add (Vector(434.06952983484234,143.92312667492396, 0.0));
    positionAlloc4->Add (Vector(12.114513700626773,196.30441373123887, 0.0));
    positionAlloc4->Add (Vector(136.6212182964619,467.105512951424, 0.0));
    positionAlloc4->Add (Vector(48.769984073209315,141.46434448897872, 0.0));
    positionAlloc4->Add (Vector(128.26159793729275,271.519608198365, 0.0));
    positionAlloc4->Add (Vector(289.69313522391394,432.99365859692557, 0.0));
    positionAlloc4->Add (Vector(411.347797879668,209.89131458443188, 0.0));
    positionAlloc4->Add (Vector(389.18772630229205,28.040517814570954, 0.0));
    positionAlloc4->Add (Vector(343.1305174629287,304.4644557682127, 0.0));
    positionAlloc4->Add (Vector(115.96624165149144,261.80761945208644, 0.0));
    positionAlloc4->Add (Vector(152.49480385920288,111.03537978715666, 0.0));
    positionAlloc4->Add (Vector(452.6585680355711,334.7915388025426, 0.0));
    positionAlloc4->Add (Vector(310.01986206522207,67.98293961431834, 0.0));
    positionAlloc4->Add (Vector(316.61986230813835,130.2891790471649, 0.0));
    positionAlloc4->Add (Vector(89.28972089904391,91.19601770603197, 0.0));
    positionAlloc4->Add (Vector(290.8471912566964,165.40689907234307, 0.0));
    positionAlloc4->Add (Vector(185.6116111225452,365.7977970076928, 0.0));
    positionAlloc4->Add (Vector(498.2486432413632,367.1209804346324, 0.0));
    positionAlloc4->Add (Vector(321.4113825723035,3.6590478395430726, 0.0));
    positionAlloc4->Add (Vector(136.30716529985636,434.7164066776551, 0.0));
    positionAlloc4->Add (Vector(125.17956566974775,383.2692908502751, 0.0));
    positionAlloc4->Add (Vector(404.39845068052057,451.34555699281526, 0.0));
    positionAlloc4->Add (Vector(422.55842813474817,388.8811553785286, 0.0));
    positionAlloc4->Add (Vector(297.48149727984344,34.537959264601795, 0.0));
    positionAlloc4->Add (Vector(357.3518059043381,66.9516137599906, 0.0));
    positionAlloc4->Add (Vector(129.23128694459052,293.39745030645975, 0.0));
    positionAlloc4->Add (Vector(300.34788921190994,240.1313235463955, 0.0));
    positionAlloc4->Add (Vector(218.827832882666,459.61124624620817, 0.0));
    positionAlloc4->Add (Vector(156.22615679213138,289.68066417081263, 0.0));
    positionAlloc4->Add (Vector(27.097426095579912,349.20878637908004, 0.0));
    positionAlloc4->Add (Vector(337.2899523471924,95.30305059418409, 0.0));
    positionAlloc4->Add (Vector(4.130779938031182,412.88774844956225, 0.0));
    positionAlloc4->Add (Vector(349.7751790810922,95.89043816830983, 0.0));
    positionAlloc4->Add (Vector(200.90033312501703,191.7476604384239, 0.0));
    positionAlloc4->Add (Vector(263.4683223217538,472.1637348477318, 0.0));
    positionAlloc4->Add (Vector(492.73329882878073,178.36158907350412, 0.0));
    positionAlloc4->Add (Vector(219.21560189101098,143.27003778930296, 0.0));
    positionAlloc4->Add (Vector(424.7246489760523,326.4934571750111, 0.0));
    positionAlloc4->Add (Vector(247.71009081140204,445.87412233822727, 0.0));
    positionAlloc4->Add (Vector(474.3787956037089,410.2127618662452, 0.0));
    positionAlloc4->Add (Vector(344.1634105597508,329.3139606208703, 0.0));
    positionAlloc4->Add (Vector(155.78184896763915,315.18694237644695, 0.0));
    positionAlloc4->Add (Vector(486.1563041951239,473.1779005493678, 0.0));
    positionAlloc4->Add (Vector(496.5829866840902,414.657274388747, 0.0));
    positionAlloc4->Add (Vector(79.60423898678876,159.23553814457142, 0.0));
    positionAlloc4->Add (Vector(145.92960300733833,82.48586857631062, 0.0));
    positionAlloc4->Add (Vector(64.24692545778987,463.1771328463553, 0.0));
    positionAlloc4->Add (Vector(499.8666282972077,179.36608663273023, 0.0));
    positionAlloc4->Add (Vector(219.9212222599775,82.4263328045004, 0.0));
    positionAlloc4->Add (Vector(393.17808337746396,222.0656792754956, 0.0));
    positionAlloc4->Add (Vector(86.98874628831132,102.80809403708952, 0.0));
    positionAlloc4->Add (Vector(448.24255708444997,158.98610186849848, 0.0));
    positionAlloc4->Add (Vector(425.92803891431174,384.0121229769696, 0.0));
    positionAlloc4->Add (Vector(56.44627249842582,439.651221190151, 0.0));
    positionAlloc4->Add (Vector(269.16165165160226,439.67061066713364, 0.0));
    positionAlloc4->Add (Vector(440.1957587924045,147.72620276738812, 0.0));
    positionAlloc4->Add (Vector(168.3529837007477,193.85227107265302, 0.0));
    positionAlloc4->Add (Vector(212.64633933306837,114.45633898062118, 0.0));
    positionAlloc4->Add (Vector(119.57294341234076,279.7231684738957, 0.0));
    positionAlloc4->Add (Vector(381.19903981132217,484.59423747400547, 0.0));
    positionAlloc4->Add (Vector(170.29984093241717,254.2170896671358, 0.0));
    positionAlloc4->Add (Vector(122.9876628264912,136.89429865327207, 0.0));
    positionAlloc4->Add (Vector(201.19269406990585,171.27838237111155, 0.0));
    positionAlloc4->Add (Vector(232.81566297116845,349.3386521711147, 0.0));
    positionAlloc4->Add (Vector(244.69532585603105,268.43916086587984, 0.0));
    positionAlloc4->Add (Vector(256.8163937153859,99.32279276166584, 0.0));
    positionAlloc4->Add (Vector(104.03787827764532,22.76621670654977, 0.0));
    positionAlloc4->Add (Vector(96.97590622209378,202.4867946938067, 0.0));
    positionAlloc4->Add (Vector(213.17769905519685,470.4133255571164, 0.0));
    positionAlloc4->Add (Vector(2.4084839293425087,120.29876520153942, 0.0));
    positionAlloc4->Add (Vector(21.332072122622282,413.0742056814875, 0.0));
    positionAlloc4->Add (Vector(264.2173545645779,209.4568155367043, 0.0));
    positionAlloc4->Add (Vector(394.6497044561084,180.74085966101956, 0.0));
    positionAlloc4->Add (Vector(260.74669034664913,200.32686208557195, 0.0));
    positionAlloc4->Add (Vector(170.4997156082595,217.33621027944315, 0.0));
    positionAlloc4->Add (Vector(19.827677070409443,77.57632428774124, 0.0));
    positionAlloc4->Add (Vector(178.67934457899537,97.0624391307186, 0.0));
    positionAlloc4->Add (Vector(236.86441888194025,182.69924103027918, 0.0));
    positionAlloc4->Add (Vector(216.59244144634832,424.5452605596402, 0.0));
    positionAlloc4->Add (Vector(252.19492827359014,143.97930091800686, 0.0));
    positionAlloc4->Add (Vector(492.0956811009957,106.53581876681028, 0.0));
    positionAlloc4->Add (Vector(328.85652680932026,321.33577304160326, 0.0));
    positionAlloc4->Add (Vector(157.30361869174027,477.7700187470162, 0.0));
    positionAlloc4->Add (Vector(198.83798284318055,109.33134274381628, 0.0));
    positionAlloc4->Add (Vector(99.92348533702517,427.04826215791155, 0.0));
    positionAlloc4->Add (Vector(158.23371491103268,358.7171085230753, 0.0));
    positionAlloc4->Add (Vector(470.96352589798516,150.90722444970868, 0.0));
    positionAlloc4->Add (Vector(409.8838558330068,289.56558157016707, 0.0));
    positionAlloc4->Add (Vector(254.65758129516763,63.33234677899085, 0.0));
    positionAlloc4->Add (Vector(333.3187724836081,73.81545472012313, 0.0));
    positionAlloc4->Add (Vector(145.81325503940295,205.02441652377973, 0.0));
    positionAlloc4->Add (Vector(437.88657438300845,120.92133805770105, 0.0));
    positionAlloc4->Add (Vector(55.023175885765276,156.89461716285348, 0.0));
    positionAlloc4->Add (Vector(460.3324226424864,393.33819182737005, 0.0));
    positionAlloc4->Add (Vector(26.421555393767115,238.57116083637027, 0.0));
    positionAlloc4->Add (Vector(247.08507779887051,229.03284897307884, 0.0));
    positionAlloc4->Add (Vector(339.9366731447124,250.3089133308119, 0.0));
    positionAlloc4->Add (Vector(328.1433727053935,142.29736651963293, 0.0));
    positionAlloc4->Add (Vector(68.0001609548161,169.8609649419447, 0.0));
    positionAlloc4->Add (Vector(86.4447136851667,24.36148362304602, 0.0));
    positionAlloc4->Add (Vector(285.02341931504185,305.8352803330874, 0.0));
    positionAlloc4->Add (Vector(129.83451681325243,142.38468740397147, 0.0));
    positionAlloc4->Add (Vector(128.40502850351626,0.09475140562026363, 0.0));
    positionAlloc4->Add (Vector(294.63424758983217,444.2433039503392, 0.0));
    positionAlloc4->Add (Vector(172.94704696184309,480.183236127188, 0.0));
    positionAlloc4->Add (Vector(156.2530200318608,0.12405484572064474, 0.0));
    positionAlloc4->Add (Vector(69.35800004261606,145.13688943418245, 0.0));
    positionAlloc4->Add (Vector(117.15468197470035,95.54688124252253, 0.0));
    positionAlloc4->Add (Vector(74.6487755798425,365.7177304090774, 0.0));
    positionAlloc4->Add (Vector(282.09698909207725,247.91485086030823, 0.0));
    positionAlloc4->Add (Vector(453.661560438928,100.15125196892083, 0.0));
    positionAlloc4->Add (Vector(368.8876287668328,468.4809693409879, 0.0));
    positionAlloc4->Add (Vector(192.9738006084576,326.26280270385837, 0.0));
    positionAlloc4->Add (Vector(148.88281908996902,483.1550585510018, 0.0));
    positionAlloc4->Add (Vector(315.3191885922887,477.2180852760085, 0.0));
    positionAlloc4->Add (Vector(60.16792880744099,228.66594106726734, 0.0));
    positionAlloc4->Add (Vector(2.83031777760423,345.1063258343152, 0.0));
    positionAlloc4->Add (Vector(80.69388069098349,1.4151321195500777, 0.0));
    positionAlloc4->Add (Vector(334.0517299932066,353.85700231988096, 0.0));
    positionAlloc4->Add (Vector(256.44061782907306,438.5535818316501, 0.0));
    positionAlloc4->Add (Vector(396.0122691645424,86.42175186072943, 0.0));
    positionAlloc4->Add (Vector(80.950466639231,293.72633879457896, 0.0));
    positionAlloc4->Add (Vector(455.6485506897888,430.8911790264908, 0.0));
    positionAlloc4->Add (Vector(273.6366075879404,10.741576473199787, 0.0));
    positionAlloc4->Add (Vector(220.17039596979754,487.4394642215281, 0.0));
    positionAlloc4->Add (Vector(223.15366870113525,264.5712874088254, 0.0));
    positionAlloc4->Add (Vector(138.1617903245727,197.5796836028067, 0.0));
    positionAlloc4->Add (Vector(395.9550138354615,478.1419217596377, 0.0));
    positionAlloc4->Add (Vector(242.9101273526702,183.58254193434902, 0.0));
    positionAlloc4->Add (Vector(220.9725974499343,282.08007682160866, 0.0));
    positionAlloc4->Add (Vector(76.32037071960978,311.61527452595504, 0.0));
    positionAlloc4->Add (Vector(489.50143384785997,138.0882437832185, 0.0));
    positionAlloc4->Add (Vector(137.1779213063289,65.8510761762286, 0.0));
    positionAlloc4->Add (Vector(77.6320225536276,51.11700155982957, 0.0));
    positionAlloc4->Add (Vector(15.089272273828557,465.3730776037179, 0.0));
    positionAlloc4->Add (Vector(400.07242921492156,395.77886237057226, 0.0));
    positionAlloc4->Add (Vector(9.144572823040376,454.4612015753617, 0.0));
    positionAlloc4->Add (Vector(187.4850205414018,46.4134002414831, 0.0));
    positionAlloc4->Add (Vector(489.5961393460294,477.9882773167313, 0.0));
    positionAlloc4->Add (Vector(487.26502684077815,413.42919326443825, 0.0));
    positionAlloc4->Add (Vector(334.0776053065262,253.79894149651545, 0.0));
    positionAlloc4->Add (Vector(317.23097259795514,190.7588590366256, 0.0));
    positionAlloc4->Add (Vector(52.714612390179994,316.5830211068686, 0.0));
    positionAlloc4->Add (Vector(276.4995488941723,238.89586506768333, 0.0));
    positionAlloc4->Add (Vector(277.1047017640799,251.50082093041843, 0.0));
    positionAlloc4->Add (Vector(360.3208235919991,222.2577286821532, 0.0));
    positionAlloc4->Add (Vector(418.4582401239644,275.3353574578222, 0.0));
    positionAlloc4->Add (Vector(106.37476211723829,33.57232696072665, 0.0));
    positionAlloc4->Add (Vector(324.60247434929454,427.8550891731676, 0.0));
    positionAlloc4->Add (Vector(146.523469881248,455.9462227149512, 0.0));
    positionAlloc4->Add (Vector(57.3431611987974,240.72021188577904, 0.0));
    positionAlloc4->Add (Vector(98.70762155523872,311.6673122781859, 0.0));
    positionAlloc4->Add (Vector(350.8723459535368,246.5470473363473, 0.0));
    positionAlloc4->Add (Vector(318.77064709096504,438.0429239757226, 0.0));
    positionAlloc4->Add (Vector(253.98790137074278,167.15439503970308, 0.0));
    positionAlloc4->Add (Vector(359.30271152390776,252.81119459292432, 0.0));
    positionAlloc4->Add (Vector(104.42256986660404,115.85265933535416, 0.0));
    positionAlloc4->Add (Vector(377.62990093723374,34.3678542394103, 0.0));
    positionAlloc4->Add (Vector(296.1784868303618,192.81920539877146, 0.0));
    positionAlloc4->Add (Vector(237.58386076857303,136.51298598504468, 0.0));
    positionAlloc4->Add (Vector(346.18193447424636,92.45078681550001, 0.0));
    positionAlloc4->Add (Vector(329.9929449725661,478.61127117600154, 0.0));
    positionAlloc4->Add (Vector(457.32611012327595,289.220855765311, 0.0));
    positionAlloc4->Add (Vector(479.5521966020279,186.63039862914243, 0.0));
    positionAlloc4->Add (Vector(248.53837558311358,354.4216424850356, 0.0));
    positionAlloc4->Add (Vector(366.5858556572019,116.72181695985829, 0.0));
    positionAlloc4->Add (Vector(88.58973854163166,157.9563980732469, 0.0));
    positionAlloc4->Add (Vector(31.384174315011006,425.9299085202552, 0.0));
    positionAlloc4->Add (Vector(300.03682498916686,38.98649395495385, 0.0));
    positionAlloc4->Add (Vector(423.20499399681927,496.9434029941421, 0.0));
    positionAlloc4->Add (Vector(241.69138247026643,461.21657549833697, 0.0));
    positionAlloc4->Add (Vector(322.5712904853554,185.71635277631333, 0.0));
    positionAlloc4->Add (Vector(78.2847995111744,360.3534041399225, 0.0));
    positionAlloc4->Add (Vector(469.52137799233515,278.21107048782886, 0.0));
    positionAlloc4->Add (Vector(36.392259154283124,338.8140885055543, 0.0));
    positionAlloc4->Add (Vector(174.04115109729017,405.9328417174738, 0.0));
    positionAlloc4->Add (Vector(178.58939812840597,65.09648931585149, 0.0));
    positionAlloc4->Add (Vector(2.3726836594472056,122.19526253628239, 0.0));
    positionAlloc4->Add (Vector(135.73503534940411,205.56091140090322, 0.0));
    positionAlloc4->Add (Vector(237.11481095356595,197.69498460759144, 0.0));
    positionAlloc4->Add (Vector(297.3798910018054,47.373868409165134, 0.0));
    positionAlloc4->Add (Vector(106.98256424837538,187.66353357679077, 0.0));
    positionAlloc4->Add (Vector(115.03690560124575,8.904230148922831, 0.0));
    positionAlloc4->Add (Vector(427.3556667136814,469.6093007770278, 0.0));
    positionAlloc4->Add (Vector(180.91582340678113,458.98336108753494, 0.0));
    positionAlloc4->Add (Vector(226.49040634887007,358.5451296396923, 0.0));
    positionAlloc4->Add (Vector(306.15166476857127,444.4782553910471, 0.0));
    positionAlloc4->Add (Vector(384.2640287329293,281.6352139106672, 0.0));
    positionAlloc4->Add (Vector(337.2049468959542,124.49469439940769, 0.0));
    positionAlloc4->Add (Vector(111.50034517220759,479.0197218138547, 0.0));
    positionAlloc4->Add (Vector(243.3270012408103,425.05484095124586, 0.0));
    positionAlloc4->Add (Vector(211.01754816301406,342.55058439670705, 0.0));
    positionAlloc4->Add (Vector(424.1911849899828,277.1055383888874, 0.0));
    positionAlloc4->Add (Vector(55.205219027560595,484.4160608998774, 0.0));
    positionAlloc4->Add (Vector(107.91932575528446,308.9311415970035, 0.0));
    positionAlloc4->Add (Vector(360.4785531676799,45.68600584647425, 0.0));
    positionAlloc4->Add (Vector(247.19163289362706,412.8504129006839, 0.0));
    positionAlloc4->Add (Vector(281.1995501983008,423.5892261619756, 0.0));
    positionAlloc4->Add (Vector(281.27235099671367,348.2673000674908, 0.0));
    positionAlloc4->Add (Vector(98.84113609408801,15.09626916119472, 0.0));
    positionAlloc4->Add (Vector(183.9387006033975,120.61191457677994, 0.0));
    positionAlloc4->Add (Vector(409.3385063072511,417.3103032436673, 0.0));
    positionAlloc4->Add (Vector(274.03285016009346,285.9342766443304, 0.0));
    positionAlloc4->Add (Vector(212.46647265989245,394.93439663043614, 0.0));
    positionAlloc4->Add (Vector(377.89790313708653,443.52588066299865, 0.0));
    positionAlloc4->Add (Vector(296.468408219145,469.0160039427132, 0.0));
    positionAlloc4->Add (Vector(468.11177443994944,338.5323115937929, 0.0));
    positionAlloc4->Add (Vector(312.9301526164517,67.97216502079284, 0.0));
    positionAlloc4->Add (Vector(449.61000049656093,259.2324755917481, 0.0));
    positionAlloc4->Add (Vector(341.6383234649672,76.25933812040952, 0.0));
    positionAlloc4->Add (Vector(347.8338019077829,105.8565623584521, 0.0));
    positionAlloc4->Add (Vector(376.70662701859425,126.36190009871478, 0.0));
    positionAlloc4->Add (Vector(435.12545282149404,442.86321285905217, 0.0));
    positionAlloc4->Add (Vector(198.7378655581114,376.5587114683197, 0.0));
    positionAlloc4->Add (Vector(141.662925067497,245.8859824216344, 0.0));
    positionAlloc4->Add (Vector(492.7267613652783,211.7831030827016, 0.0));
    positionAlloc4->Add (Vector(54.9325407310976,28.61869931813582, 0.0));
    positionAlloc4->Add (Vector(338.15426646943234,58.66128379281593, 0.0));
    positionAlloc4->Add (Vector(49.02995366602103,152.30106340198768, 0.0));
    positionAlloc4->Add (Vector(57.499938232989045,287.3547377350917, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_21.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_ECC_flowmon/switch_ECC_21.flowmon", false, false);
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