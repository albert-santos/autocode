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
	  uint16_t numberOfNodes = 712;
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
    positionAlloc->Add (Vector (444.44444444444446,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (444.44444444444446,0.0, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (388.8888888888889,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (277.77777777777777,0.0, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,277.77777777777777, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (222.22222222222223,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,111.11111111111111, 0.0));
    positionAlloc->Add (Vector (111.11111111111111,55.55555555555556, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,444.44444444444446, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,388.8888888888889, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,222.22222222222223, 0.0));
    positionAlloc->Add (Vector (55.55555555555556,166.66666666666666, 0.0));
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
    positionAlloc4->Add (Vector(440.87019099973475,100.25101040968082, 0.0));
    positionAlloc4->Add (Vector(9.212198046297082,12.256619243304845, 0.0));
    positionAlloc4->Add (Vector(52.2882657741559,442.9901817884288, 0.0));
    positionAlloc4->Add (Vector(169.11223659317193,410.8313161445351, 0.0));
    positionAlloc4->Add (Vector(293.9027421358291,199.01807243001295, 0.0));
    positionAlloc4->Add (Vector(63.346783774345774,456.22893261815335, 0.0));
    positionAlloc4->Add (Vector(164.97736281293257,402.069380639088, 0.0));
    positionAlloc4->Add (Vector(462.9607330837482,453.3773965504923, 0.0));
    positionAlloc4->Add (Vector(256.1159957371579,324.26443503066173, 0.0));
    positionAlloc4->Add (Vector(458.5263285244634,208.16330398457356, 0.0));
    positionAlloc4->Add (Vector(127.39172004618172,447.093555013425, 0.0));
    positionAlloc4->Add (Vector(142.96591970459215,338.958741331171, 0.0));
    positionAlloc4->Add (Vector(360.3925678968094,255.2460801065441, 0.0));
    positionAlloc4->Add (Vector(334.7640909812857,49.657988169488746, 0.0));
    positionAlloc4->Add (Vector(109.00176348030143,495.7610100903118, 0.0));
    positionAlloc4->Add (Vector(257.0543485741784,120.01773973245894, 0.0));
    positionAlloc4->Add (Vector(209.5347300389379,29.237490921184495, 0.0));
    positionAlloc4->Add (Vector(115.24404757369561,485.1077574989451, 0.0));
    positionAlloc4->Add (Vector(219.57663313215576,85.63565738040252, 0.0));
    positionAlloc4->Add (Vector(71.7386800333536,59.291337503225236, 0.0));
    positionAlloc4->Add (Vector(371.96568086995643,152.72971879467627, 0.0));
    positionAlloc4->Add (Vector(260.4383545685848,389.7682828848653, 0.0));
    positionAlloc4->Add (Vector(231.2184058529051,359.200373159021, 0.0));
    positionAlloc4->Add (Vector(62.177856834366,86.35161082679244, 0.0));
    positionAlloc4->Add (Vector(475.83629104609514,278.1339353941715, 0.0));
    positionAlloc4->Add (Vector(181.3791444169337,188.93675866889964, 0.0));
    positionAlloc4->Add (Vector(498.6571737783451,429.86046196800714, 0.0));
    positionAlloc4->Add (Vector(239.20024151370495,429.76192035990806, 0.0));
    positionAlloc4->Add (Vector(497.1499300047565,398.88109024379577, 0.0));
    positionAlloc4->Add (Vector(250.28440576792883,24.156206221379538, 0.0));
    positionAlloc4->Add (Vector(154.35355629202385,291.5339486558791, 0.0));
    positionAlloc4->Add (Vector(233.76717798131068,179.74265318348253, 0.0));
    positionAlloc4->Add (Vector(428.8886386179692,33.1296945047192, 0.0));
    positionAlloc4->Add (Vector(161.5655489439819,435.97917589135756, 0.0));
    positionAlloc4->Add (Vector(52.323864444850656,131.40813997733432, 0.0));
    positionAlloc4->Add (Vector(356.7108974189258,423.10685640912146, 0.0));
    positionAlloc4->Add (Vector(305.0062725201216,122.13971316817069, 0.0));
    positionAlloc4->Add (Vector(378.66703765769523,59.68227950620758, 0.0));
    positionAlloc4->Add (Vector(362.8786291765562,485.36250947391363, 0.0));
    positionAlloc4->Add (Vector(271.38755183560744,120.62873604669144, 0.0));
    positionAlloc4->Add (Vector(469.16908770116635,27.03026003644149, 0.0));
    positionAlloc4->Add (Vector(460.94631292153827,384.20497671837865, 0.0));
    positionAlloc4->Add (Vector(18.768976342259936,145.97172695719357, 0.0));
    positionAlloc4->Add (Vector(33.69774139315973,436.5979690054165, 0.0));
    positionAlloc4->Add (Vector(71.2252903660061,200.94203233977265, 0.0));
    positionAlloc4->Add (Vector(168.82969127650972,182.04348733530844, 0.0));
    positionAlloc4->Add (Vector(211.40463164731798,319.5969405054184, 0.0));
    positionAlloc4->Add (Vector(468.0302140489343,112.62352459277946, 0.0));
    positionAlloc4->Add (Vector(203.87358546721524,218.3832074477338, 0.0));
    positionAlloc4->Add (Vector(327.3009587561118,406.74383978056215, 0.0));
    positionAlloc4->Add (Vector(396.0237784960482,101.08665142572725, 0.0));
    positionAlloc4->Add (Vector(261.7375990468691,407.29426499199707, 0.0));
    positionAlloc4->Add (Vector(380.42654922094715,154.02575573671717, 0.0));
    positionAlloc4->Add (Vector(366.35684074351883,161.6294357426241, 0.0));
    positionAlloc4->Add (Vector(432.71370009420724,384.31813135503444, 0.0));
    positionAlloc4->Add (Vector(167.36745360161987,374.9522319582106, 0.0));
    positionAlloc4->Add (Vector(275.45298157216234,13.99175705621042, 0.0));
    positionAlloc4->Add (Vector(282.3324093683777,448.0114997641725, 0.0));
    positionAlloc4->Add (Vector(407.62100719549295,287.775085352488, 0.0));
    positionAlloc4->Add (Vector(268.97735477825887,362.6282197672029, 0.0));
    positionAlloc4->Add (Vector(300.30494401516586,402.65157431100556, 0.0));
    positionAlloc4->Add (Vector(21.926406391708785,263.39515341275285, 0.0));
    positionAlloc4->Add (Vector(426.74483264756446,288.30473435400467, 0.0));
    positionAlloc4->Add (Vector(475.71746637172834,388.7596186875928, 0.0));
    positionAlloc4->Add (Vector(98.55519373575855,372.97081223285414, 0.0));
    positionAlloc4->Add (Vector(192.68078559824698,328.23463124352486, 0.0));
    positionAlloc4->Add (Vector(126.95892615694993,49.86531271507938, 0.0));
    positionAlloc4->Add (Vector(48.45670893061832,30.21006760438566, 0.0));
    positionAlloc4->Add (Vector(258.27508659400985,77.97284704610846, 0.0));
    positionAlloc4->Add (Vector(186.04270416795754,425.39396836644323, 0.0));
    positionAlloc4->Add (Vector(127.06471513012302,280.3907472811849, 0.0));
    positionAlloc4->Add (Vector(148.25007754850822,424.3191366939588, 0.0));
    positionAlloc4->Add (Vector(120.01998892275778,39.21540351399716, 0.0));
    positionAlloc4->Add (Vector(440.8829852462865,13.87099491020477, 0.0));
    positionAlloc4->Add (Vector(1.2588327866008275,281.6334268756992, 0.0));
    positionAlloc4->Add (Vector(427.1177197029127,290.0424659453642, 0.0));
    positionAlloc4->Add (Vector(418.45122457819156,294.1492006356588, 0.0));
    positionAlloc4->Add (Vector(118.70447858428213,1.1709515398082848, 0.0));
    positionAlloc4->Add (Vector(6.211671220367865,183.28441293689224, 0.0));
    positionAlloc4->Add (Vector(203.53176884290573,222.13989181409966, 0.0));
    positionAlloc4->Add (Vector(285.129827565998,310.7648499946038, 0.0));
    positionAlloc4->Add (Vector(194.6019732408699,102.37238542694915, 0.0));
    positionAlloc4->Add (Vector(78.96897090335835,156.47569565941976, 0.0));
    positionAlloc4->Add (Vector(433.76325756909023,124.65009755114932, 0.0));
    positionAlloc4->Add (Vector(283.4894204964829,383.0388453428955, 0.0));
    positionAlloc4->Add (Vector(254.48481795141063,359.6533295900256, 0.0));
    positionAlloc4->Add (Vector(193.98378923694625,297.88887393580455, 0.0));
    positionAlloc4->Add (Vector(301.7491625589074,173.28632872266286, 0.0));
    positionAlloc4->Add (Vector(41.20553702318519,308.30694451977877, 0.0));
    positionAlloc4->Add (Vector(54.83412820672401,78.76889242639817, 0.0));
    positionAlloc4->Add (Vector(490.5860496866336,171.46732413107884, 0.0));
    positionAlloc4->Add (Vector(401.1836872630405,201.8957007912972, 0.0));
    positionAlloc4->Add (Vector(154.79546303889745,331.249038677933, 0.0));
    positionAlloc4->Add (Vector(139.40517081680258,480.44486858681734, 0.0));
    positionAlloc4->Add (Vector(431.53222750004375,490.04174571252906, 0.0));
    positionAlloc4->Add (Vector(212.27953808082512,126.64841380475283, 0.0));
    positionAlloc4->Add (Vector(439.202050214532,324.9463487696519, 0.0));
    positionAlloc4->Add (Vector(454.1144847522009,10.742415719332598, 0.0));
    positionAlloc4->Add (Vector(222.7644057991392,234.98081767158408, 0.0));
    positionAlloc4->Add (Vector(127.40789207087572,175.1302398741257, 0.0));
    positionAlloc4->Add (Vector(393.613028877977,112.02916897561438, 0.0));
    positionAlloc4->Add (Vector(327.56159471166626,165.85397101039885, 0.0));
    positionAlloc4->Add (Vector(333.4801768844854,397.0591674834325, 0.0));
    positionAlloc4->Add (Vector(207.12440686341643,277.37621397991455, 0.0));
    positionAlloc4->Add (Vector(51.166172865015405,40.58649452914798, 0.0));
    positionAlloc4->Add (Vector(498.7371292306079,405.7354367383981, 0.0));
    positionAlloc4->Add (Vector(271.93270360971184,464.8835757095353, 0.0));
    positionAlloc4->Add (Vector(495.46606849360853,339.9104729929001, 0.0));
    positionAlloc4->Add (Vector(181.06824885017792,48.5010133755629, 0.0));
    positionAlloc4->Add (Vector(88.80448482346654,84.09929261980525, 0.0));
    positionAlloc4->Add (Vector(189.663536140089,458.431515051009, 0.0));
    positionAlloc4->Add (Vector(487.6379033508565,488.8928735259139, 0.0));
    positionAlloc4->Add (Vector(286.7943376655348,440.19603788512524, 0.0));
    positionAlloc4->Add (Vector(466.52819510387815,33.38437364542429, 0.0));
    positionAlloc4->Add (Vector(149.51650828253239,456.66884898919784, 0.0));
    positionAlloc4->Add (Vector(354.16173813206865,248.2464947820097, 0.0));
    positionAlloc4->Add (Vector(121.89110261027658,366.21958537599943, 0.0));
    positionAlloc4->Add (Vector(380.75295713754474,465.13259775650334, 0.0));
    positionAlloc4->Add (Vector(8.390738500869633,268.239355544346, 0.0));
    positionAlloc4->Add (Vector(246.27664967697032,77.52111678786677, 0.0));
    positionAlloc4->Add (Vector(472.7407106398519,169.29770343321988, 0.0));
    positionAlloc4->Add (Vector(279.31922045681563,276.3779827877814, 0.0));
    positionAlloc4->Add (Vector(367.8812309553728,284.4471447503631, 0.0));
    positionAlloc4->Add (Vector(356.4892947065495,293.04384219934644, 0.0));
    positionAlloc4->Add (Vector(221.90746952582296,399.4461981099187, 0.0));
    positionAlloc4->Add (Vector(49.02500994826286,261.5483829904846, 0.0));
    positionAlloc4->Add (Vector(266.6320650687612,457.52462767885913, 0.0));
    positionAlloc4->Add (Vector(189.5953237259029,391.66780824895733, 0.0));
    positionAlloc4->Add (Vector(8.792751294929735,346.20577311599766, 0.0));
    positionAlloc4->Add (Vector(174.8739118964292,365.87426709704397, 0.0));
    positionAlloc4->Add (Vector(103.15079685927175,408.7259782090402, 0.0));
    positionAlloc4->Add (Vector(126.69510305928256,464.8955740486964, 0.0));
    positionAlloc4->Add (Vector(325.6916721312529,52.661065739853576, 0.0));
    positionAlloc4->Add (Vector(226.71537664046753,166.39970402836514, 0.0));
    positionAlloc4->Add (Vector(339.4942090994046,266.2802636926491, 0.0));
    positionAlloc4->Add (Vector(121.49523007016327,470.5105311792893, 0.0));
    positionAlloc4->Add (Vector(198.16766294349696,489.69122967586594, 0.0));
    positionAlloc4->Add (Vector(320.0551861929755,380.72347013453754, 0.0));
    positionAlloc4->Add (Vector(161.100777924381,454.52079873046046, 0.0));
    positionAlloc4->Add (Vector(234.77523014269448,261.01288654084925, 0.0));
    positionAlloc4->Add (Vector(39.12819578600829,181.07725701166245, 0.0));
    positionAlloc4->Add (Vector(56.663957354211256,99.28070257747589, 0.0));
    positionAlloc4->Add (Vector(383.3542556023196,473.4272419966055, 0.0));
    positionAlloc4->Add (Vector(382.95986584212824,363.50729991541834, 0.0));
    positionAlloc4->Add (Vector(364.38514020413015,54.57492014911236, 0.0));
    positionAlloc4->Add (Vector(431.5915806324902,168.5191172709798, 0.0));
    positionAlloc4->Add (Vector(249.5702187319192,197.4376658236137, 0.0));
    positionAlloc4->Add (Vector(492.43474275749844,291.88020386958243, 0.0));
    positionAlloc4->Add (Vector(482.8282600980736,249.53932314874393, 0.0));
    positionAlloc4->Add (Vector(277.6255318617978,51.134402869081, 0.0));
    positionAlloc4->Add (Vector(451.72888684611206,7.628633160314147, 0.0));
    positionAlloc4->Add (Vector(309.68329157433146,45.877860000647864, 0.0));
    positionAlloc4->Add (Vector(340.2255959310958,225.58913841596456, 0.0));
    positionAlloc4->Add (Vector(424.30994857900026,190.9500729597024, 0.0));
    positionAlloc4->Add (Vector(491.9883991760062,270.811109876224, 0.0));
    positionAlloc4->Add (Vector(44.64287431417435,433.42834275522864, 0.0));
    positionAlloc4->Add (Vector(379.7259839359286,463.2684128131625, 0.0));
    positionAlloc4->Add (Vector(401.1646704432544,478.11869501095174, 0.0));
    positionAlloc4->Add (Vector(70.43375682074637,156.8900187092463, 0.0));
    positionAlloc4->Add (Vector(430.8580147770136,496.8911434295375, 0.0));
    positionAlloc4->Add (Vector(147.47246532678466,92.02071353570246, 0.0));
    positionAlloc4->Add (Vector(378.8747671495766,207.2799729974829, 0.0));
    positionAlloc4->Add (Vector(194.25940684243915,178.45793524315684, 0.0));
    positionAlloc4->Add (Vector(325.8288589555493,111.54418851732633, 0.0));
    positionAlloc4->Add (Vector(317.53308717361676,177.7054468911054, 0.0));
    positionAlloc4->Add (Vector(342.15928098333245,287.8120469674144, 0.0));
    positionAlloc4->Add (Vector(255.32974191445408,68.65368806395838, 0.0));
    positionAlloc4->Add (Vector(138.74426773970146,241.60235250482458, 0.0));
    positionAlloc4->Add (Vector(484.5866772846691,338.4384169677662, 0.0));
    positionAlloc4->Add (Vector(253.64601614410054,321.77140239543576, 0.0));
    positionAlloc4->Add (Vector(109.61298621925769,124.52100607006989, 0.0));
    positionAlloc4->Add (Vector(294.0080616128482,194.6099658154088, 0.0));
    positionAlloc4->Add (Vector(110.31068421287993,19.431600280491622, 0.0));
    positionAlloc4->Add (Vector(337.11774606266357,202.45471110023533, 0.0));
    positionAlloc4->Add (Vector(278.3740991412727,268.8645587105543, 0.0));
    positionAlloc4->Add (Vector(260.46160225931936,339.6096451475428, 0.0));
    positionAlloc4->Add (Vector(263.8740383749715,22.70726099974335, 0.0));
    positionAlloc4->Add (Vector(473.4283543884675,287.1781684477962, 0.0));
    positionAlloc4->Add (Vector(90.3832721499847,480.37892320977, 0.0));
    positionAlloc4->Add (Vector(450.24875068653483,346.04256607312334, 0.0));
    positionAlloc4->Add (Vector(64.91583387804323,357.61435965188593, 0.0));
    positionAlloc4->Add (Vector(304.3243606016001,137.3034856830234, 0.0));
    positionAlloc4->Add (Vector(257.9874468795543,99.45109097310583, 0.0));
    positionAlloc4->Add (Vector(62.38253441983665,413.8609915488867, 0.0));
    positionAlloc4->Add (Vector(8.887416272245263,87.79730654570783, 0.0));
    positionAlloc4->Add (Vector(330.8063262978878,355.0246485645882, 0.0));
    positionAlloc4->Add (Vector(9.19622820412208,467.81515582864375, 0.0));
    positionAlloc4->Add (Vector(387.0986743000086,329.73602713734255, 0.0));
    positionAlloc4->Add (Vector(488.1030065029896,415.1659257848408, 0.0));
    positionAlloc4->Add (Vector(411.4838937532228,45.60364792838162, 0.0));
    positionAlloc4->Add (Vector(372.6497408635041,117.5515117683933, 0.0));
    positionAlloc4->Add (Vector(344.3819314526772,414.37263337574365, 0.0));
    positionAlloc4->Add (Vector(442.19528188331356,222.20811558931803, 0.0));
    positionAlloc4->Add (Vector(17.82806099237383,195.88787438580968, 0.0));
    positionAlloc4->Add (Vector(292.0617507892407,14.887310105830942, 0.0));
    positionAlloc4->Add (Vector(236.18196484284798,219.38466871730205, 0.0));
    positionAlloc4->Add (Vector(237.8336262391575,407.08555228052234, 0.0));
    positionAlloc4->Add (Vector(351.24624225749284,401.8981172900679, 0.0));
    positionAlloc4->Add (Vector(394.3767246497006,108.68108765134204, 0.0));
    positionAlloc4->Add (Vector(167.23990245021602,68.85629763304169, 0.0));
    positionAlloc4->Add (Vector(98.99866928538175,384.78544822383543, 0.0));
    positionAlloc4->Add (Vector(104.49506004809584,258.13547262586343, 0.0));
    positionAlloc4->Add (Vector(321.26030662658576,191.30714549194184, 0.0));
    positionAlloc4->Add (Vector(123.1800900868219,49.074574348722756, 0.0));
    positionAlloc4->Add (Vector(125.10875976750413,52.65372955725761, 0.0));
    positionAlloc4->Add (Vector(117.88325077697425,40.488244477303596, 0.0));
    positionAlloc4->Add (Vector(14.205288232784829,432.4596379709949, 0.0));
    positionAlloc4->Add (Vector(404.37084107497617,374.312539484903, 0.0));
    positionAlloc4->Add (Vector(216.07856495670435,335.0448934587242, 0.0));
    positionAlloc4->Add (Vector(320.2132654184846,78.2884906716792, 0.0));
    positionAlloc4->Add (Vector(106.00656511795836,115.43964778891919, 0.0));
    positionAlloc4->Add (Vector(348.5619708816941,413.99775952051647, 0.0));
    positionAlloc4->Add (Vector(117.84876220008483,356.926737216732, 0.0));
    positionAlloc4->Add (Vector(354.81005795993013,183.30012967350308, 0.0));
    positionAlloc4->Add (Vector(471.8649189413614,197.0766751361658, 0.0));
    positionAlloc4->Add (Vector(179.4046239035408,69.03370827710303, 0.0));
    positionAlloc4->Add (Vector(437.58818527418885,257.25557216060787, 0.0));
    positionAlloc4->Add (Vector(214.40310851452531,267.5185266110255, 0.0));
    positionAlloc4->Add (Vector(79.48260393213458,67.20701914183208, 0.0));
    positionAlloc4->Add (Vector(465.14798860879625,465.5202564438867, 0.0));
    positionAlloc4->Add (Vector(316.4663255797427,308.8110154922289, 0.0));
    positionAlloc4->Add (Vector(340.54395014448147,463.54621813253806, 0.0));
    positionAlloc4->Add (Vector(91.46399996391425,176.88948633458855, 0.0));
    positionAlloc4->Add (Vector(246.4861530881181,69.62092923197899, 0.0));
    positionAlloc4->Add (Vector(317.3140853901114,349.7573034301039, 0.0));
    positionAlloc4->Add (Vector(412.29856400660964,348.8373399545547, 0.0));
    positionAlloc4->Add (Vector(231.71707453668267,315.8828629876368, 0.0));
    positionAlloc4->Add (Vector(112.64347019243608,18.526597680392943, 0.0));
    positionAlloc4->Add (Vector(8.252521950987246,470.1753070839976, 0.0));
    positionAlloc4->Add (Vector(222.24551353190773,256.9120440850113, 0.0));
    positionAlloc4->Add (Vector(276.22503445861565,49.601854230840736, 0.0));
    positionAlloc4->Add (Vector(62.90476888644964,319.49721295828164, 0.0));
    positionAlloc4->Add (Vector(293.76655754137164,487.00199041467715, 0.0));
    positionAlloc4->Add (Vector(131.1188671724508,422.3337295040497, 0.0));
    positionAlloc4->Add (Vector(94.4234722611984,136.5308643759432, 0.0));
    positionAlloc4->Add (Vector(100.30376480160474,68.82828878105995, 0.0));
    positionAlloc4->Add (Vector(489.8554190808441,35.69610420212077, 0.0));
    positionAlloc4->Add (Vector(407.36708024702943,217.81823390306414, 0.0));
    positionAlloc4->Add (Vector(135.34608678548386,76.84967652376562, 0.0));
    positionAlloc4->Add (Vector(130.54483801134853,141.88678176413694, 0.0));
    positionAlloc4->Add (Vector(114.66708948115057,335.34016899057906, 0.0));
    positionAlloc4->Add (Vector(60.083135708390415,59.962383068419335, 0.0));
    positionAlloc4->Add (Vector(414.52823452682406,56.57283357316128, 0.0));
    positionAlloc4->Add (Vector(296.7658939344662,300.7666585862759, 0.0));
    positionAlloc4->Add (Vector(218.54584770940878,298.34422817678694, 0.0));
    positionAlloc4->Add (Vector(9.694975987797172,94.42177743213654, 0.0));
    positionAlloc4->Add (Vector(430.95370483734644,54.12042630246749, 0.0));
    positionAlloc4->Add (Vector(200.4642816434189,149.0875798605532, 0.0));
    positionAlloc4->Add (Vector(78.05327727306477,55.37984070829477, 0.0));
    positionAlloc4->Add (Vector(280.6802412554803,289.49092842225565, 0.0));
    positionAlloc4->Add (Vector(44.99638878839052,476.282023935818, 0.0));
    positionAlloc4->Add (Vector(35.41138445676412,184.2479488542216, 0.0));
    positionAlloc4->Add (Vector(9.261226494408358,496.7915559051115, 0.0));
    positionAlloc4->Add (Vector(175.11026082835102,278.1775031436445, 0.0));
    positionAlloc4->Add (Vector(498.9012160776706,456.06256966262396, 0.0));
    positionAlloc4->Add (Vector(9.873009846151358,127.41861429019092, 0.0));
    positionAlloc4->Add (Vector(239.44222748324523,253.83788616788067, 0.0));
    positionAlloc4->Add (Vector(338.3671901173092,117.99406415389635, 0.0));
    positionAlloc4->Add (Vector(16.911789146931255,33.58436072571164, 0.0));
    positionAlloc4->Add (Vector(85.59724793777652,61.51162528331766, 0.0));
    positionAlloc4->Add (Vector(126.78023875215521,32.9623541665392, 0.0));
    positionAlloc4->Add (Vector(345.3006943841513,37.47189152880248, 0.0));
    positionAlloc4->Add (Vector(117.0780444763177,240.1115711387448, 0.0));
    positionAlloc4->Add (Vector(29.099203240458284,161.10589063194962, 0.0));
    positionAlloc4->Add (Vector(201.55542870524252,298.8593186127245, 0.0));
    positionAlloc4->Add (Vector(254.69873188808756,226.04595213319823, 0.0));
    positionAlloc4->Add (Vector(354.1505151699102,147.83068716480935, 0.0));
    positionAlloc4->Add (Vector(145.03010281489338,415.4896432277803, 0.0));
    positionAlloc4->Add (Vector(345.20700352221354,406.5047933449315, 0.0));
    positionAlloc4->Add (Vector(473.8725471685837,90.3826489571805, 0.0));
    positionAlloc4->Add (Vector(302.9597899114214,228.88991164366195, 0.0));
    positionAlloc4->Add (Vector(182.86951721912808,92.01944549559649, 0.0));
    positionAlloc4->Add (Vector(133.84449184213838,131.45476877675654, 0.0));
    positionAlloc4->Add (Vector(0.4460647373451532,85.68456004175695, 0.0));
    positionAlloc4->Add (Vector(202.58024026612102,299.7466568427716, 0.0));
    positionAlloc4->Add (Vector(492.97945098480227,469.01832921478837, 0.0));
    positionAlloc4->Add (Vector(427.94046517433867,259.0615291497779, 0.0));
    positionAlloc4->Add (Vector(226.2329352974411,171.99227154442735, 0.0));
    positionAlloc4->Add (Vector(499.27799392059336,88.25444955576877, 0.0));
    positionAlloc4->Add (Vector(209.9365299645809,71.9343320362395, 0.0));
    positionAlloc4->Add (Vector(331.6260661685787,162.00331688528945, 0.0));
    positionAlloc4->Add (Vector(47.53502285020972,219.14869021575578, 0.0));
    positionAlloc4->Add (Vector(338.268161519143,484.969220959683, 0.0));
    positionAlloc4->Add (Vector(330.2217335021973,425.73355441690484, 0.0));
    positionAlloc4->Add (Vector(121.72415572518635,142.49737768668956, 0.0));
    positionAlloc4->Add (Vector(367.4635072512229,118.04888454198125, 0.0));
    positionAlloc4->Add (Vector(267.2119817731482,462.17071085001396, 0.0));
    positionAlloc4->Add (Vector(241.18683900021344,289.76854439961625, 0.0));
    positionAlloc4->Add (Vector(373.7799454612338,153.40118926478706, 0.0));
    positionAlloc4->Add (Vector(184.7200148558532,176.13105177677852, 0.0));
    positionAlloc4->Add (Vector(305.5236863417128,179.78539437786912, 0.0));
    positionAlloc4->Add (Vector(92.1815395610946,267.0701504962353, 0.0));
    positionAlloc4->Add (Vector(424.0314438436008,373.1841115899494, 0.0));
    positionAlloc4->Add (Vector(312.5296965715864,212.25083367796333, 0.0));
    positionAlloc4->Add (Vector(80.41345350967377,8.965688044876519, 0.0));
    positionAlloc4->Add (Vector(233.82335206339417,420.8738798980636, 0.0));
    positionAlloc4->Add (Vector(495.18759178422295,424.46216936187835, 0.0));
    positionAlloc4->Add (Vector(246.71626439623913,434.7955354251366, 0.0));
    positionAlloc4->Add (Vector(104.66185943040097,298.0073489427554, 0.0));
    positionAlloc4->Add (Vector(71.79191110753457,325.6150715579741, 0.0));
    positionAlloc4->Add (Vector(249.079158780026,467.7375576300681, 0.0));
    positionAlloc4->Add (Vector(12.486897175514544,415.5548909131926, 0.0));
    positionAlloc4->Add (Vector(367.2546129723556,498.7866539532365, 0.0));
    positionAlloc4->Add (Vector(322.6284244036351,64.98077242497341, 0.0));
    positionAlloc4->Add (Vector(463.3832013513043,387.5594133746914, 0.0));
    positionAlloc4->Add (Vector(204.5481584025205,308.5280695242751, 0.0));
    positionAlloc4->Add (Vector(368.3568085474775,490.5565129989109, 0.0));
    positionAlloc4->Add (Vector(19.39281329267839,412.381841789981, 0.0));
    positionAlloc4->Add (Vector(309.0991682607601,492.9971285340349, 0.0));
    positionAlloc4->Add (Vector(171.173129005025,352.73391438114174, 0.0));
    positionAlloc4->Add (Vector(213.164591698837,224.87383900159568, 0.0));
    positionAlloc4->Add (Vector(326.10993861062235,29.259380169550543, 0.0));
    positionAlloc4->Add (Vector(91.5017677120471,330.112443923903, 0.0));
    positionAlloc4->Add (Vector(203.1674853796106,76.68553496264241, 0.0));
    positionAlloc4->Add (Vector(34.706569136089605,419.78750136341813, 0.0));
    positionAlloc4->Add (Vector(196.0713145901591,279.84860795299846, 0.0));
    positionAlloc4->Add (Vector(16.186919280449132,374.0487570143878, 0.0));
    positionAlloc4->Add (Vector(253.53560401448212,77.11007997155662, 0.0));
    positionAlloc4->Add (Vector(318.7111698901552,80.20297569922053, 0.0));
    positionAlloc4->Add (Vector(320.0309520749576,298.9053095171138, 0.0));
    positionAlloc4->Add (Vector(21.26237937742165,182.3948851941221, 0.0));
    positionAlloc4->Add (Vector(31.140744313302328,480.6822276789792, 0.0));
    positionAlloc4->Add (Vector(152.95104588919452,406.59310182089445, 0.0));
    positionAlloc4->Add (Vector(380.5307970710842,25.614546579681097, 0.0));
    positionAlloc4->Add (Vector(293.84164828483426,402.8604674505457, 0.0));
    positionAlloc4->Add (Vector(367.61358939917216,178.645037030198, 0.0));
    positionAlloc4->Add (Vector(150.6980750084817,15.337649887467942, 0.0));
    positionAlloc4->Add (Vector(324.15700696009134,117.55052104029367, 0.0));
    positionAlloc4->Add (Vector(135.22682975434142,82.26245910300989, 0.0));
    positionAlloc4->Add (Vector(189.34240803114417,408.7426534125668, 0.0));
    positionAlloc4->Add (Vector(59.94579389254101,4.081525894555849, 0.0));
    positionAlloc4->Add (Vector(469.0193519155988,309.8898968530255, 0.0));
    positionAlloc4->Add (Vector(396.93459106219063,260.74002687593355, 0.0));
    positionAlloc4->Add (Vector(390.30376632147,191.87565653143847, 0.0));
    positionAlloc4->Add (Vector(76.97832906990737,382.0026194184424, 0.0));
    positionAlloc4->Add (Vector(160.35726591604794,147.4258472906276, 0.0));
    positionAlloc4->Add (Vector(436.96812568941033,429.3440970790453, 0.0));
    positionAlloc4->Add (Vector(292.30944007837644,55.63919075860247, 0.0));
    positionAlloc4->Add (Vector(340.28323733467954,221.7462075544251, 0.0));
    positionAlloc4->Add (Vector(209.0557800990837,107.4144206185243, 0.0));
    positionAlloc4->Add (Vector(152.48460283171366,228.84832174627178, 0.0));
    positionAlloc4->Add (Vector(219.33463595904757,393.07090692723585, 0.0));
    positionAlloc4->Add (Vector(189.61878764463137,103.44918122477526, 0.0));
    positionAlloc4->Add (Vector(207.27760229537307,339.7500297938539, 0.0));
    positionAlloc4->Add (Vector(492.09272388885466,66.3589699349993, 0.0));
    positionAlloc4->Add (Vector(322.51604540827583,253.6808279524392, 0.0));
    positionAlloc4->Add (Vector(182.8433581432306,397.3656383168388, 0.0));
    positionAlloc4->Add (Vector(20.098533932019325,95.37920457269644, 0.0));
    positionAlloc4->Add (Vector(195.8646722254088,475.99381171831106, 0.0));
    positionAlloc4->Add (Vector(377.85961450867796,371.4600733419419, 0.0));
    positionAlloc4->Add (Vector(443.9898688218831,195.9213743580175, 0.0));
    positionAlloc4->Add (Vector(399.2982724594646,399.7142180812262, 0.0));
    positionAlloc4->Add (Vector(422.10967749157214,27.615939530222956, 0.0));
    positionAlloc4->Add (Vector(68.5310691783535,311.4151696093808, 0.0));
    positionAlloc4->Add (Vector(358.3885611296254,385.02172817854375, 0.0));
    positionAlloc4->Add (Vector(447.66509819750166,382.4567057701049, 0.0));
    positionAlloc4->Add (Vector(38.72591587389229,22.22629028553741, 0.0));
    positionAlloc4->Add (Vector(130.12013849411287,265.6078969780229, 0.0));
    positionAlloc4->Add (Vector(0.6196402781432075,171.1936299738101, 0.0));
    positionAlloc4->Add (Vector(39.21543766003438,412.26244973862293, 0.0));
    positionAlloc4->Add (Vector(439.5587573886376,474.7552956145866, 0.0));
    positionAlloc4->Add (Vector(464.33557890421315,199.3364831081606, 0.0));
    positionAlloc4->Add (Vector(316.60121798247314,107.54225315581789, 0.0));
    positionAlloc4->Add (Vector(453.25791556369666,317.8398220074136, 0.0));
    positionAlloc4->Add (Vector(205.02293481712968,19.07263343349075, 0.0));
    positionAlloc4->Add (Vector(32.495878513564556,15.120161664423815, 0.0));
    positionAlloc4->Add (Vector(59.96797257601661,270.1085118379305, 0.0));
    positionAlloc4->Add (Vector(63.52691169559566,55.90666765574398, 0.0));
    positionAlloc4->Add (Vector(307.8614533733268,404.6013015546248, 0.0));
    positionAlloc4->Add (Vector(482.40307629827174,16.648664091635233, 0.0));
    positionAlloc4->Add (Vector(94.09759999999945,475.3867713753266, 0.0));
    positionAlloc4->Add (Vector(365.77992644771297,130.9328560090019, 0.0));
    positionAlloc4->Add (Vector(426.01687014151935,160.54556052005742, 0.0));
    positionAlloc4->Add (Vector(125.87215732899232,112.47325464560404, 0.0));
    positionAlloc4->Add (Vector(462.0351509583553,86.54482951942394, 0.0));
    positionAlloc4->Add (Vector(268.856440711801,188.59862870841155, 0.0));
    positionAlloc4->Add (Vector(388.3703764715976,281.3193686083453, 0.0));
    positionAlloc4->Add (Vector(97.73546902893393,401.5921307690058, 0.0));
    positionAlloc4->Add (Vector(447.67232205096127,460.2761035914869, 0.0));
    positionAlloc4->Add (Vector(400.6381117035588,78.77985099266577, 0.0));
    positionAlloc4->Add (Vector(145.94588090464305,476.4268505627799, 0.0));
    positionAlloc4->Add (Vector(447.49057629902103,188.55465432793372, 0.0));
    positionAlloc4->Add (Vector(200.4412013138942,409.6967931857369, 0.0));
    positionAlloc4->Add (Vector(113.2762720182634,344.3533791781369, 0.0));
    positionAlloc4->Add (Vector(278.1619271004743,102.86093623116871, 0.0));
    positionAlloc4->Add (Vector(400.1039660872717,465.4325237554431, 0.0));
    positionAlloc4->Add (Vector(225.03210439373433,199.12655693985204, 0.0));
    positionAlloc4->Add (Vector(209.21081367321042,390.96088486629804, 0.0));
    positionAlloc4->Add (Vector(271.87006190883636,160.2136974961248, 0.0));
    positionAlloc4->Add (Vector(104.59143089084944,225.10812410461733, 0.0));
    positionAlloc4->Add (Vector(72.06841207730346,395.71885744348174, 0.0));
    positionAlloc4->Add (Vector(480.9028552830061,171.22340237211316, 0.0));
    positionAlloc4->Add (Vector(250.23936372844912,36.063445214571445, 0.0));
    positionAlloc4->Add (Vector(41.61963029043636,482.3526104314964, 0.0));
    positionAlloc4->Add (Vector(475.4024453429548,471.1730688982747, 0.0));
    positionAlloc4->Add (Vector(376.5297314330393,327.437697403654, 0.0));
    positionAlloc4->Add (Vector(52.13478178911563,6.283270845996003, 0.0));
    positionAlloc4->Add (Vector(357.0748208125759,177.85686040919003, 0.0));
    positionAlloc4->Add (Vector(414.47039793579233,439.6496622783597, 0.0));
    positionAlloc4->Add (Vector(295.2024364539048,25.16416101360297, 0.0));
    positionAlloc4->Add (Vector(90.29349358400052,133.14015386553297, 0.0));
    positionAlloc4->Add (Vector(428.04711116519496,77.77060335746316, 0.0));
    positionAlloc4->Add (Vector(357.369025255803,333.06988734401915, 0.0));
    positionAlloc4->Add (Vector(378.0177291228703,149.9849827521282, 0.0));
    positionAlloc4->Add (Vector(353.5714511031486,312.4611970879904, 0.0));
    positionAlloc4->Add (Vector(253.29492957765527,102.55794188142464, 0.0));
    positionAlloc4->Add (Vector(34.28090905114173,488.58196977928105, 0.0));
    positionAlloc4->Add (Vector(432.27248179002737,314.7831816032681, 0.0));
    positionAlloc4->Add (Vector(45.118084205877764,193.94110823880206, 0.0));
    positionAlloc4->Add (Vector(286.4837580563731,405.8047441396107, 0.0));
    positionAlloc4->Add (Vector(82.04775426865451,329.53815745844116, 0.0));
    positionAlloc4->Add (Vector(304.77801671605033,75.8841232579816, 0.0));
    positionAlloc4->Add (Vector(205.07493616715865,482.28867120429555, 0.0));
    positionAlloc4->Add (Vector(82.30038387444122,24.748235226064676, 0.0));
    positionAlloc4->Add (Vector(475.04758705111163,231.57224280825704, 0.0));
    positionAlloc4->Add (Vector(207.57773228152092,370.93881813836805, 0.0));
    positionAlloc4->Add (Vector(288.58473719409017,309.2684343484228, 0.0));
    positionAlloc4->Add (Vector(15.391773722706393,275.19345921479015, 0.0));
    positionAlloc4->Add (Vector(190.3210104050327,41.335736070643726, 0.0));
    positionAlloc4->Add (Vector(25.046676481631224,444.1793755293948, 0.0));
    positionAlloc4->Add (Vector(74.6238435538707,30.086946874970877, 0.0));
    positionAlloc4->Add (Vector(165.99474502261512,3.223121638720683, 0.0));
    positionAlloc4->Add (Vector(468.469210563083,273.2785443932444, 0.0));
    positionAlloc4->Add (Vector(194.91432100567235,304.6636346883086, 0.0));
    positionAlloc4->Add (Vector(246.94355048516337,269.6099779640359, 0.0));
    positionAlloc4->Add (Vector(110.90799396067801,311.54370193294386, 0.0));
    positionAlloc4->Add (Vector(66.10890633293116,256.68656344798626, 0.0));
    positionAlloc4->Add (Vector(448.8588856233579,341.7318564543644, 0.0));
    positionAlloc4->Add (Vector(414.5459603335532,308.202911131592, 0.0));
    positionAlloc4->Add (Vector(439.8819444415313,254.83437066360088, 0.0));
    positionAlloc4->Add (Vector(202.41961750152714,178.84451360029146, 0.0));
    positionAlloc4->Add (Vector(386.9976685731897,292.18701295246956, 0.0));
    positionAlloc4->Add (Vector(252.2452468036445,297.2535405049258, 0.0));
    positionAlloc4->Add (Vector(454.5258381771409,275.58067240653065, 0.0));
    positionAlloc4->Add (Vector(468.3487064339761,148.73071033782298, 0.0));
    positionAlloc4->Add (Vector(140.78131172628693,469.077863437378, 0.0));
    positionAlloc4->Add (Vector(51.27745054854249,423.23597203904376, 0.0));
    positionAlloc4->Add (Vector(292.6896731709557,461.7733862788931, 0.0));
    positionAlloc4->Add (Vector(487.98245019155047,239.29954907509466, 0.0));
    positionAlloc4->Add (Vector(342.5804991002208,194.08740062938867, 0.0));
    positionAlloc4->Add (Vector(198.0443327711797,174.01086402799103, 0.0));
    positionAlloc4->Add (Vector(484.72263603082973,104.75801207214263, 0.0));
    positionAlloc4->Add (Vector(19.990461253704904,377.2385709285356, 0.0));
    positionAlloc4->Add (Vector(499.31552004615685,26.647742170330613, 0.0));
    positionAlloc4->Add (Vector(287.88854533655206,435.4630341722058, 0.0));
    positionAlloc4->Add (Vector(32.8674701971653,236.53183018081714, 0.0));
    positionAlloc4->Add (Vector(366.5971944349424,255.05775928295927, 0.0));
    positionAlloc4->Add (Vector(250.87334014736408,465.0130121501466, 0.0));
    positionAlloc4->Add (Vector(172.5008098957343,25.214216337995644, 0.0));
    positionAlloc4->Add (Vector(259.93156286183495,373.45292458541803, 0.0));
    positionAlloc4->Add (Vector(240.91782934482558,467.1818909129247, 0.0));
    positionAlloc4->Add (Vector(226.82618468377973,465.0116837774712, 0.0));
    positionAlloc4->Add (Vector(378.96490245923826,451.42808135198067, 0.0));
    positionAlloc4->Add (Vector(276.410105386361,31.78737902155526, 0.0));
    positionAlloc4->Add (Vector(42.494946028884875,374.84351636654384, 0.0));
    positionAlloc4->Add (Vector(130.2401156435007,57.406353091009, 0.0));
    positionAlloc4->Add (Vector(88.5030948067953,487.0064562201393, 0.0));
    positionAlloc4->Add (Vector(112.43235872259055,389.8274153687946, 0.0));
    positionAlloc4->Add (Vector(80.10075766483627,335.05445121182663, 0.0));
    positionAlloc4->Add (Vector(338.2234087277938,87.83737785078455, 0.0));
    positionAlloc4->Add (Vector(444.9918977198315,167.16194328356426, 0.0));
    positionAlloc4->Add (Vector(443.67208659695353,144.3539130361013, 0.0));
    positionAlloc4->Add (Vector(305.6891202294627,75.19889006106462, 0.0));
    positionAlloc4->Add (Vector(304.885934561744,250.69352564625436, 0.0));
    positionAlloc4->Add (Vector(214.6910713290278,13.534914842251222, 0.0));
    positionAlloc4->Add (Vector(284.31870897870255,122.79329704000058, 0.0));
    positionAlloc4->Add (Vector(278.4617392097155,198.5399990531957, 0.0));
    positionAlloc4->Add (Vector(106.5484598561487,382.9604499379315, 0.0));
    positionAlloc4->Add (Vector(410.4514083093867,63.78254382399956, 0.0));
    positionAlloc4->Add (Vector(385.69475859724264,109.51782309369784, 0.0));
    positionAlloc4->Add (Vector(362.4944364475525,155.51457974921723, 0.0));
    positionAlloc4->Add (Vector(440.06107444175296,10.277191612234727, 0.0));
    positionAlloc4->Add (Vector(381.22846763329545,196.17745716232727, 0.0));
    positionAlloc4->Add (Vector(65.73180419114577,457.70132391172757, 0.0));
    positionAlloc4->Add (Vector(57.059997769043356,89.6287305677635, 0.0));
    positionAlloc4->Add (Vector(348.31895563569316,168.04146220436334, 0.0));
    positionAlloc4->Add (Vector(253.51135142818464,41.348726695871754, 0.0));
    positionAlloc4->Add (Vector(26.033196740837262,26.61715739876008, 0.0));
    positionAlloc4->Add (Vector(329.8738080569997,24.822482445210404, 0.0));
    positionAlloc4->Add (Vector(456.65693126447263,228.00330978201822, 0.0));
    positionAlloc4->Add (Vector(355.43356162205407,213.60791581814308, 0.0));
    positionAlloc4->Add (Vector(319.1307852198414,359.8289212459769, 0.0));
    positionAlloc4->Add (Vector(250.95393349404037,23.553903433097602, 0.0));
    positionAlloc4->Add (Vector(214.65995510627982,272.12244769756495, 0.0));
    positionAlloc4->Add (Vector(155.32699393105497,356.6112429173747, 0.0));
    positionAlloc4->Add (Vector(215.22985517155752,314.5367649703607, 0.0));
    positionAlloc4->Add (Vector(56.18606690869776,272.9011372298905, 0.0));
    positionAlloc4->Add (Vector(447.75638416118045,11.911284539127154, 0.0));
    positionAlloc4->Add (Vector(459.45067464011265,127.11366126724177, 0.0));
    positionAlloc4->Add (Vector(199.57593197756896,185.62338168980818, 0.0));
    positionAlloc4->Add (Vector(380.60013833023856,490.93287048243485, 0.0));
    positionAlloc4->Add (Vector(194.57338012105535,467.66757212621746, 0.0));
    positionAlloc4->Add (Vector(326.0063386854396,389.17154558475414, 0.0));
    positionAlloc4->Add (Vector(494.38881978858575,458.7756603354941, 0.0));
    positionAlloc4->Add (Vector(330.7839056508917,483.1352819667927, 0.0));
    positionAlloc4->Add (Vector(19.45546307863022,13.784253510925382, 0.0));
    positionAlloc4->Add (Vector(455.0170908163379,161.84637820156522, 0.0));
    positionAlloc4->Add (Vector(75.41555011794382,320.2329409858325, 0.0));
    positionAlloc4->Add (Vector(191.6687473118146,65.13713742433502, 0.0));
    positionAlloc4->Add (Vector(350.5754740195009,322.87950693212287, 0.0));
    positionAlloc4->Add (Vector(276.3053094369396,291.76563821096863, 0.0));
    positionAlloc4->Add (Vector(356.05901528158296,254.4801518054447, 0.0));
    positionAlloc4->Add (Vector(70.13330346269487,237.47596534305188, 0.0));
    positionAlloc4->Add (Vector(185.6113340951433,368.94941778374357, 0.0));
    positionAlloc4->Add (Vector(167.35084770484698,470.11729124868174, 0.0));
    positionAlloc4->Add (Vector(420.7549424500267,365.10564221802025, 0.0));
    positionAlloc4->Add (Vector(158.4515281436788,95.59804680984834, 0.0));
    positionAlloc4->Add (Vector(135.808524938772,136.0227138811649, 0.0));
    positionAlloc4->Add (Vector(417.5949683185792,496.92289023377054, 0.0));
    positionAlloc4->Add (Vector(63.82104294398688,229.03158338126045, 0.0));
    positionAlloc4->Add (Vector(132.2747679566139,171.75528404446683, 0.0));
    positionAlloc4->Add (Vector(448.37879735798987,37.586237214458805, 0.0));
    positionAlloc4->Add (Vector(413.86145597139364,6.023716699813719, 0.0));
    positionAlloc4->Add (Vector(116.71379630402862,74.54059997465006, 0.0));
    positionAlloc4->Add (Vector(426.8942674083726,427.99804962049126, 0.0));
    positionAlloc4->Add (Vector(33.118587857642325,330.58310793070103, 0.0));
    positionAlloc4->Add (Vector(198.22964789012508,154.84903492218788, 0.0));
    positionAlloc4->Add (Vector(158.47293985867955,227.28918832491567, 0.0));
    positionAlloc4->Add (Vector(409.72044608602033,72.51598839554241, 0.0));
    positionAlloc4->Add (Vector(333.6732977071749,252.19000223994576, 0.0));
    positionAlloc4->Add (Vector(449.64984103133895,318.67326405714266, 0.0));
    positionAlloc4->Add (Vector(354.49946501680483,366.43338271400347, 0.0));
    positionAlloc4->Add (Vector(402.8389678164352,104.10254046231127, 0.0));
    positionAlloc4->Add (Vector(329.31109874803235,227.6920293757805, 0.0));
    positionAlloc4->Add (Vector(208.87154167210437,350.2728344298545, 0.0));
    positionAlloc4->Add (Vector(368.705301869349,346.4082286438368, 0.0));
    positionAlloc4->Add (Vector(183.960361211692,372.35804573405824, 0.0));
    positionAlloc4->Add (Vector(163.47536071716416,398.7559620593207, 0.0));
    positionAlloc4->Add (Vector(265.5773288414538,449.3641290249795, 0.0));
    positionAlloc4->Add (Vector(34.034802014962615,283.4547114765906, 0.0));
    positionAlloc4->Add (Vector(296.828765725558,117.11752776415285, 0.0));
    positionAlloc4->Add (Vector(325.9767039231677,47.74529043041292, 0.0));
    positionAlloc4->Add (Vector(28.874810850974765,220.00153672535362, 0.0));
    positionAlloc4->Add (Vector(467.0563124907495,401.5071225010117, 0.0));
    positionAlloc4->Add (Vector(305.86709696544966,175.89715991967137, 0.0));
    positionAlloc4->Add (Vector(105.96321215736943,252.28398420972675, 0.0));
    positionAlloc4->Add (Vector(231.4861038095244,70.42385006547703, 0.0));
    positionAlloc4->Add (Vector(278.5879999204265,131.5501816587392, 0.0));
    positionAlloc4->Add (Vector(60.19237310680242,155.72501399404965, 0.0));
    positionAlloc4->Add (Vector(227.1805351190278,22.776637427144863, 0.0));
    positionAlloc4->Add (Vector(234.44189317565045,432.71453699779664, 0.0));
    positionAlloc4->Add (Vector(0.3265713312189389,402.6236512445276, 0.0));
    positionAlloc4->Add (Vector(441.9757481887238,26.310268556966832, 0.0));
    positionAlloc4->Add (Vector(138.03196224619245,97.80179783231519, 0.0));
    positionAlloc4->Add (Vector(429.64771595687,385.4834127795324, 0.0));
    positionAlloc4->Add (Vector(233.95606645877902,98.77920976788273, 0.0));
    positionAlloc4->Add (Vector(342.55720913288405,181.29081731251185, 0.0));
    positionAlloc4->Add (Vector(119.54981466358855,451.14808326502725, 0.0));
    positionAlloc4->Add (Vector(403.8653974331404,450.93222612213737, 0.0));
    positionAlloc4->Add (Vector(201.68944241608426,308.20155114072793, 0.0));
    positionAlloc4->Add (Vector(83.04822326916917,231.9312989632309, 0.0));
    positionAlloc4->Add (Vector(467.82189802941764,456.40235314119906, 0.0));
    positionAlloc4->Add (Vector(66.92473014273448,281.23749262933086, 0.0));
    positionAlloc4->Add (Vector(117.10826322037381,156.5278388911633, 0.0));
    positionAlloc4->Add (Vector(67.48981548062116,403.442678192527, 0.0));
    positionAlloc4->Add (Vector(41.357050267415985,167.8969983299325, 0.0));
    positionAlloc4->Add (Vector(250.2668634772523,481.6912004563841, 0.0));
    positionAlloc4->Add (Vector(203.53465528987462,295.0064934441982, 0.0));
    positionAlloc4->Add (Vector(138.81861792074884,125.32973380772839, 0.0));
    positionAlloc4->Add (Vector(161.66603784061638,428.58688687224446, 0.0));
    positionAlloc4->Add (Vector(408.06547970847834,111.74307847808717, 0.0));
    positionAlloc4->Add (Vector(83.98018883268405,209.65237829991023, 0.0));
    positionAlloc4->Add (Vector(155.27014980886656,95.08314531229888, 0.0));
    positionAlloc4->Add (Vector(295.076830885501,193.41729462043278, 0.0));
    positionAlloc4->Add (Vector(448.07410316253987,55.86237534662586, 0.0));
    positionAlloc4->Add (Vector(87.47626595130886,325.80749235881683, 0.0));
    positionAlloc4->Add (Vector(32.80374642478551,168.35810222715446, 0.0));
    positionAlloc4->Add (Vector(383.0908763772994,67.68753982443798, 0.0));
    positionAlloc4->Add (Vector(228.54325832242867,375.5917002420614, 0.0));
    positionAlloc4->Add (Vector(47.59299244120363,182.21125233934794, 0.0));
    positionAlloc4->Add (Vector(147.4670177317043,271.786872842483, 0.0));
    positionAlloc4->Add (Vector(200.06276974642407,384.3118577564123, 0.0));
    positionAlloc4->Add (Vector(68.6618479007708,210.99173861640685, 0.0));
    positionAlloc4->Add (Vector(39.60733015681983,401.9262540534018, 0.0));
    positionAlloc4->Add (Vector(295.6047258140598,376.635201604883, 0.0));
    positionAlloc4->Add (Vector(201.0651592757776,92.29658803478091, 0.0));
    positionAlloc4->Add (Vector(4.393782872153862,381.8254785049637, 0.0));
    positionAlloc4->Add (Vector(239.8415795753005,392.16379749382696, 0.0));
    positionAlloc4->Add (Vector(247.177914350717,314.1951912756299, 0.0));
    positionAlloc4->Add (Vector(463.6858353361933,443.7668792929, 0.0));
    positionAlloc4->Add (Vector(218.81255845402808,456.289635100978, 0.0));
    positionAlloc4->Add (Vector(68.00969591553313,197.66806606260263, 0.0));
    positionAlloc4->Add (Vector(227.7512365723421,301.71392846180504, 0.0));
    positionAlloc4->Add (Vector(181.26682618745116,151.65139169045494, 0.0));
    positionAlloc4->Add (Vector(441.65526729614766,230.4205750470778, 0.0));
    positionAlloc4->Add (Vector(133.03184910856226,409.0962085353422, 0.0));
    positionAlloc4->Add (Vector(176.93948326583637,193.75639444443544, 0.0));
    positionAlloc4->Add (Vector(37.17850781066273,238.13413204819784, 0.0));
    positionAlloc4->Add (Vector(193.37862627773757,296.24923944590364, 0.0));
    positionAlloc4->Add (Vector(405.1970640769778,192.3549242167683, 0.0));
    positionAlloc4->Add (Vector(408.9047162883617,401.4070161162196, 0.0));
    positionAlloc4->Add (Vector(344.4350145089353,114.72322280905989, 0.0));
    positionAlloc4->Add (Vector(338.7542579470933,407.16394594147323, 0.0));
    positionAlloc4->Add (Vector(241.33012440529777,144.71702611834564, 0.0));
    positionAlloc4->Add (Vector(173.65954833366138,346.1326374508112, 0.0));
    positionAlloc4->Add (Vector(491.7403876170368,376.50708387090964, 0.0));
    positionAlloc4->Add (Vector(444.09000715895803,114.89885494501428, 0.0));
    positionAlloc4->Add (Vector(109.6918671986667,402.9411267778516, 0.0));
    positionAlloc4->Add (Vector(451.4117183252147,397.27349464255434, 0.0));
    positionAlloc4->Add (Vector(142.2919556953664,356.461226382511, 0.0));
    positionAlloc4->Add (Vector(288.71015149676356,125.484601575024, 0.0));
    positionAlloc4->Add (Vector(237.8894397358229,196.63611863308344, 0.0));
    positionAlloc4->Add (Vector(127.40779259758061,67.10635541026599, 0.0));
    positionAlloc4->Add (Vector(269.4673823059699,390.1468894633517, 0.0));
    positionAlloc4->Add (Vector(363.66173008783113,447.48916024136855, 0.0));
    positionAlloc4->Add (Vector(463.3792658834177,310.6744564639822, 0.0));
    positionAlloc4->Add (Vector(38.33775960119973,74.84511796097787, 0.0));
    positionAlloc4->Add (Vector(291.8219545482226,2.4458842409390558, 0.0));
    positionAlloc4->Add (Vector(219.18038323796029,210.2468872540829, 0.0));
    positionAlloc4->Add (Vector(474.31707715761223,233.74488680323492, 0.0));
    positionAlloc4->Add (Vector(333.8222514716579,188.90424255332837, 0.0));
    positionAlloc4->Add (Vector(480.6738093003676,215.6753170247534, 0.0));
    positionAlloc4->Add (Vector(479.09470181609004,256.20744636296774, 0.0));
    positionAlloc4->Add (Vector(432.63854865395336,226.99529733688627, 0.0));
    positionAlloc4->Add (Vector(277.44083688498523,370.0924511149932, 0.0));
    positionAlloc4->Add (Vector(56.30318960428543,66.52640440497515, 0.0));
    positionAlloc4->Add (Vector(92.47906438875097,323.77856326567775, 0.0));
    positionAlloc4->Add (Vector(441.8530210864824,36.012917092064946, 0.0));
    positionAlloc4->Add (Vector(347.8343724552612,98.85797916827488, 0.0));
    positionAlloc4->Add (Vector(286.1374678172882,237.88416705921728, 0.0));
    positionAlloc4->Add (Vector(452.21585863000416,274.3149028655249, 0.0));
    positionAlloc4->Add (Vector(263.1804110335596,398.89954637881783, 0.0));
    positionAlloc4->Add (Vector(371.5356980929669,164.82643401730567, 0.0));
    positionAlloc4->Add (Vector(7.549666132081412,4.131213875250839, 0.0));
    positionAlloc4->Add (Vector(194.15361946550675,291.71236461536796, 0.0));
    positionAlloc4->Add (Vector(460.0132230375291,482.1413697247957, 0.0));
    positionAlloc4->Add (Vector(29.19262685327456,461.01203186469405, 0.0));
    positionAlloc4->Add (Vector(381.8420631239839,343.05994061840624, 0.0));
    positionAlloc4->Add (Vector(199.46142609053814,331.630742830102, 0.0));
    positionAlloc4->Add (Vector(260.37979901448057,344.0539416616578, 0.0));
    positionAlloc4->Add (Vector(461.31501987711766,426.40183755018256, 0.0));
    positionAlloc4->Add (Vector(90.42770536108952,356.8557392410748, 0.0));
    positionAlloc4->Add (Vector(35.06635929527624,422.47169422173306, 0.0));
    positionAlloc4->Add (Vector(431.17143535665264,158.33824151186536, 0.0));
    positionAlloc4->Add (Vector(74.98202898999656,465.3639142685713, 0.0));
    positionAlloc4->Add (Vector(292.42217259090194,254.76490996040408, 0.0));
    positionAlloc4->Add (Vector(400.4888353588661,19.51438756146534, 0.0));
    positionAlloc4->Add (Vector(180.7814301094564,349.0918552680569, 0.0));
    positionAlloc4->Add (Vector(88.22214320905842,343.96318446216986, 0.0));
    positionAlloc4->Add (Vector(353.08634130202006,79.17944702402701, 0.0));
    positionAlloc4->Add (Vector(301.5177999648848,211.357049065241, 0.0));
    positionAlloc4->Add (Vector(164.67650718746611,25.412414212621304, 0.0));
    positionAlloc4->Add (Vector(201.39447121975525,386.58361495037695, 0.0));
    positionAlloc4->Add (Vector(59.593917211983424,259.0133287559433, 0.0));
    positionAlloc4->Add (Vector(206.16932347329265,227.03423363647673, 0.0));
    positionAlloc4->Add (Vector(391.64732326760287,363.70965696667616, 0.0));
    positionAlloc4->Add (Vector(54.113631386037085,458.1159537462986, 0.0));
    positionAlloc4->Add (Vector(350.0223034831176,114.87910855750837, 0.0));
    positionAlloc4->Add (Vector(40.524581257304796,53.509521391640114, 0.0));
    positionAlloc4->Add (Vector(286.9488731847441,378.345486559416, 0.0));
    positionAlloc4->Add (Vector(476.8711865936278,141.84125855816194, 0.0));
    positionAlloc4->Add (Vector(233.148294649807,364.4092752150063, 0.0));
    positionAlloc4->Add (Vector(99.39945701020308,488.3793872940766, 0.0));
    positionAlloc4->Add (Vector(317.3695784167432,365.04017468509215, 0.0));
    positionAlloc4->Add (Vector(25.84961600004182,496.39806758554744, 0.0));
    positionAlloc4->Add (Vector(489.08742963312716,179.82675327794684, 0.0));
    positionAlloc4->Add (Vector(325.0619189662006,493.62399142026766, 0.0));
    positionAlloc4->Add (Vector(482.0134060051301,280.3241250177549, 0.0));
    positionAlloc4->Add (Vector(273.37961917098187,203.7036182033873, 0.0));
    positionAlloc4->Add (Vector(78.13299973898485,28.04937486485293, 0.0));
    positionAlloc4->Add (Vector(477.25940990428677,406.3175868141837, 0.0));
    positionAlloc4->Add (Vector(174.86998478774717,399.63377135723454, 0.0));
    positionAlloc4->Add (Vector(391.3140849544878,106.37952062293992, 0.0));
    positionAlloc4->Add (Vector(129.26407291036878,26.02805015634113, 0.0));
    positionAlloc4->Add (Vector(266.1900657805856,306.17444317724335, 0.0));
    positionAlloc4->Add (Vector(134.02339867645773,149.08860315385024, 0.0));
    positionAlloc4->Add (Vector(363.0661006738598,461.2536988930545, 0.0));
    positionAlloc4->Add (Vector(406.5634113296164,363.37542929356874, 0.0));
    positionAlloc4->Add (Vector(345.88913426751395,457.113922302522, 0.0));
    positionAlloc4->Add (Vector(136.1824684339681,488.9104403175286, 0.0));
    positionAlloc4->Add (Vector(267.30304115495966,255.1642272994033, 0.0));
    positionAlloc4->Add (Vector(329.76751247002636,369.3737573527862, 0.0));
    positionAlloc4->Add (Vector(38.951486342848064,188.60354439557747, 0.0));
    positionAlloc4->Add (Vector(370.99180107807285,291.9116092435852, 0.0));
    positionAlloc4->Add (Vector(236.7891201128377,475.6949701700595, 0.0));
    positionAlloc4->Add (Vector(355.2268864861784,196.41900518383798, 0.0));
    positionAlloc4->Add (Vector(141.18209913693124,405.0095825251358, 0.0));
    positionAlloc4->Add (Vector(150.81297411394817,225.3594283507932, 0.0));
    positionAlloc4->Add (Vector(227.31044812377843,273.0203388150139, 0.0));
    positionAlloc4->Add (Vector(221.33390487513188,44.557395197095396, 0.0));
    positionAlloc4->Add (Vector(18.959938883924277,303.53528052598955, 0.0));
    positionAlloc4->Add (Vector(210.1403848269412,497.7076418692362, 0.0));
    positionAlloc4->Add (Vector(196.93557217945084,116.64959263506164, 0.0));
    positionAlloc4->Add (Vector(200.56477425726143,66.73429719086866, 0.0));
    positionAlloc4->Add (Vector(112.98018700654045,423.8217376143091, 0.0));
    positionAlloc4->Add (Vector(446.72798800216674,287.31410818650613, 0.0));
    positionAlloc4->Add (Vector(170.86865959560825,425.80377906743144, 0.0));
    positionAlloc4->Add (Vector(162.77776406112332,5.2168431420628885, 0.0));
    positionAlloc4->Add (Vector(474.7744410093688,190.2968193610196, 0.0));
    positionAlloc4->Add (Vector(184.2552945238991,134.43314693919172, 0.0));
    positionAlloc4->Add (Vector(158.92017776411475,193.59279943044183, 0.0));
    positionAlloc4->Add (Vector(467.4743114910985,173.61017778323296, 0.0));
    positionAlloc4->Add (Vector(312.0279677294387,194.80276401485392, 0.0));
    positionAlloc4->Add (Vector(424.4981221246817,331.60937721298683, 0.0));
    positionAlloc4->Add (Vector(295.3499445163065,330.62647607170106, 0.0));
    positionAlloc4->Add (Vector(244.50554677262863,444.5766385566612, 0.0));
    positionAlloc4->Add (Vector(350.44823421561676,253.89802708684527, 0.0));
    positionAlloc4->Add (Vector(49.47505670668162,212.0703867024742, 0.0));
    positionAlloc4->Add (Vector(323.65924621075794,82.49764994463199, 0.0));
    positionAlloc4->Add (Vector(11.648967641770035,172.09377496402172, 0.0));
    positionAlloc4->Add (Vector(427.54462045807,55.90303831143573, 0.0));
    positionAlloc4->Add (Vector(243.4527318373086,212.57965572945602, 0.0));
    positionAlloc4->Add (Vector(128.30711791337873,220.98492618418592, 0.0));
    positionAlloc4->Add (Vector(484.70899847980564,171.0471669029688, 0.0));
    positionAlloc4->Add (Vector(211.6479252605285,398.77536476238214, 0.0));
    positionAlloc4->Add (Vector(232.5683480413206,253.83454810768237, 0.0));
    positionAlloc4->Add (Vector(159.4359868612859,323.6573912088281, 0.0));
    positionAlloc4->Add (Vector(93.83918137562713,398.8801582764585, 0.0));
    positionAlloc4->Add (Vector(247.6080655477813,482.7540993710379, 0.0));
    positionAlloc4->Add (Vector(281.6138671281195,387.20447321342516, 0.0));
    positionAlloc4->Add (Vector(123.60406768624271,1.0689389800692672, 0.0));
    positionAlloc4->Add (Vector(465.49316535169095,158.23388630481884, 0.0));
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
	  AnimationInterface anim ("animations/animation_ECC_18.xml");
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
	  flowmon->SerializeToXmlFile ("flowmon-results/switch_ECC_flowmon/switch_ECC_18.flowmon", false, false);
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