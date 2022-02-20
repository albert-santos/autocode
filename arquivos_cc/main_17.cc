//-----------------------------------------------------Include
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

	std::map <uint64_t, Ipv4Address> mymap;
	std::map <uint64_t, uint64_t> mymap2;
//-----------------------------------------------------Funções

	void CallMap (Ptr<PointToPointEpcHelper>  epcHelper){
		mymap = epcHelper->RecvMymap();
		// for (std::map<uint64_t, Ipv4Address >::iterator it=mymap.begin(); it!=mymap.end(); ++it)
		// std::cout <<"imsi: "<< it->first << " ip address: " << it->second << '\n';
	}

	void SendToController(Ptr<MyController> MyCtrl){
		MyCtrl->Allocation(mymap,mymap2);
		//for (std::map<uint64_t, Ipv4Address>::iterator it=mymap.begin(); it!=mymap.end(); ++it)
		//std::cout <<"imsi: "<< it->first << " ip address: " << it->second << '\n';
	}
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
	  uint16_t numberOfRrhs = 13;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 266;
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
	double simTime = 20.0;
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
	Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
	Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
	lteHelper->SetEpcHelper (epcHelper);
	epcHelper->SetAttribute ("S1uLinkMtu", UintegerValue(7000));
	epcHelper->SetAttribute ("S1uLinkDelay", TimeValue(Seconds(0.00015)));
	epcHelper->SetAttribute ("S1uLinkDataRate", DataRateValue (DataRate ("70Gb/s")));
	lteHelper->SetSchedulerType ("ns3::RrFfMacScheduler");
	//lteHelper->SetHandoverAlgorithmType ("ns3::NoOpHandoverAlgorithm"); // disable automatic handover
	
	lteHelper->SetHandoverAlgorithmType ("ns3::A2A4RsrqHandoverAlgorithm");
	lteHelper->SetHandoverAlgorithmAttribute ("ServingCellThreshold",
                                          UintegerValue (30));
	lteHelper->SetHandoverAlgorithmAttribute ("NeighbourCellOffset",
                                          UintegerValue (1));


	ConfigStore inputConfig;
	inputConfig.ConfigureDefaults();

	GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));
	Config::SetDefault ("ns3::LteEnbRrc::SrsPeriodicity", UintegerValue (160));
	//Config::SetDefault ("ns3::OFSwitch13Device::PipelineCapacity", DataRateValue (DataRate ("100Gb/s")));
	lteHelper->SetAttribute("PathlossModel", StringValue("ns3::FriisPropagationLossModel"));

	// parse again so you can override default values from the command line
	cmd.Parse(argc, argv);

	Ptr<Node> pgw = epcHelper->GetPgwNode ();
	//Mac48Address::Allocate ();
	// Create a single RemoteHost
//-----------------------------------------------------Nós
	NodeContainer remoteHostContainer;
	NodeContainer mainswitchC;
	NodeContainer topswitchC;
	mainswitchC.Create (1);
	topswitchC.Create (1);
	Ptr<Node> topswitch = topswitchC.Get(0);
	Ptr<Node> mainswitch = mainswitchC.Get(0);
	NodeContainer midswitch;
	midswitch.Create (6);
	remoteHostContainer.Create (1);
	Ptr<Node> remoteHost = remoteHostContainer.Get (0);
	InternetStackHelper internet;
	internet.Install (remoteHostContainer);
//-----------------------------------------------------Links
	CsmaHelper csmaHelper;
	csmaHelper.SetChannelAttribute ("DataRate", DataRateValue (DataRate ("1Gbps")));//600Gbps
	csmaHelper.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (15)));//10
	csmaHelper.SetDeviceAttribute ("Mtu", UintegerValue (9000));
	csmaHelper.SetQueue ("ns3::DropTailQueue","MaxPackets", UintegerValue (65536));
	NetDeviceContainer hostDevices;
	NetDeviceContainer switchPorts[8];
	NodeContainer pair (remoteHost, topswitch);
	NetDeviceContainer link = csmaHelper.Install (pair);
	hostDevices.Add (link.Get (0));
	switchPorts[0].Add (link.Get (1));
	pair = NodeContainer(topswitch,midswitch.Get(0));
	link = csmaHelper.Install (pair);
	switchPorts[0].Add (link.Get (0));
	switchPorts[1].Add (link.Get (1));
	pair =NodeContainer(midswitch.Get(0), mainswitch);
	link = csmaHelper.Install (pair);
	switchPorts[1].Add (link.Get (0));
	switchPorts[3].Add (link.Get (1));
	pair = NodeContainer(topswitch,midswitch.Get(1));
	link = csmaHelper.Install (pair);
	switchPorts[0].Add (link.Get (0));
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

	// Create the controller node
	Ptr<Node> controllerNode = CreateObject<Node> ();
	Ptr<Node> controllerNode2 = CreateObject<Node> ();

	//Ptr<OFSwitch13InternalHelper> of13Helper = CreateObject<OFSwitch13InternalHelper> ();
	//Ptr<OFSwitch13LearningController> learnCtrl = CreateObject<OFSwitch13LearningController> ();
	OFSwitch13DeviceContainer ofSwitchDevices;
	//of13Helper->InstallController (controllerNode, learnCtrl);

	// Configure the OpenFlow network domain
	Ptr<OFSwitch13InternalHelper> ofMyCtrlHelper = CreateObject<OFSwitch13InternalHelper> ();
	Ptr<MyController> MyCtrl = CreateObject<MyController> ();
	ofMyCtrlHelper->InstallController (controllerNode2, MyCtrl);

	ofSwitchDevices.Add (ofMyCtrlHelper->InstallSwitch (mainswitch, switchPorts[3]));
	ofSwitchDevices.Add (ofMyCtrlHelper->InstallSwitch (topswitch, switchPorts[0]));
	ofSwitchDevices.Add (ofMyCtrlHelper->InstallSwitch (midswitch.Get(0), switchPorts[1]));
	ofSwitchDevices.Add (ofMyCtrlHelper->InstallSwitch (midswitch.Get(1), switchPorts[2]));
	ofSwitchDevices.Add (ofMyCtrlHelper->InstallSwitch (midswitch.Get(2), switchPorts[4]));
	ofSwitchDevices.Add (ofMyCtrlHelper->InstallSwitch (midswitch.Get(3), switchPorts[5]));
	ofSwitchDevices.Add (ofMyCtrlHelper->InstallSwitch (midswitch.Get(4), switchPorts[6]));
	ofSwitchDevices.Add (ofMyCtrlHelper->InstallSwitch (midswitch.Get(5), switchPorts[7]));
	ofMyCtrlHelper->CreateOpenFlowChannels ();

	//of13Helper->CreateOpenFlowChannels ();

	Ipv4AddressHelper ipv4h;
	ipv4h.SetBase ("1.0.0.0", "255.255.255.0","0.0.0.2");
	Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (hostDevices);
	Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (0);
	Ipv4Address pgwAddr = internetIpIfaces.GetAddress (1);

	Ipv4StaticRoutingHelper ipv4RoutingHelper;
	Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
	remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"),pgwAddr, 1,1);
	//remoteHostStaticRouting->SetDefaultRoute (internetIpIfaces.GetAddress(1), 1,1);

	NodeContainer enbNodes;
	enbNodes.Create(numberOfRrhs);
//-----------------------------------------------------Mobilidade
	Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
	// for (uint16_t i = 0; i < numberOfRrhs; i++)
	// 	{
	// 		positionAlloc->Add (Vector(50,distance * i , 0));
	// 	}
	MobilityHelper mobility;
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

	// mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
    //                              "MinX", DoubleValue (0.0),
    //                              "MinY", DoubleValue (0.0),
    //                              "DeltaX", DoubleValue (distance/4),
    //                              "DeltaY", DoubleValue (distance/4),
    //                              "GridWidth", UintegerValue (4),
    //                              "LayoutType", StringValue ("RowFirst"));
	
	positionAlloc->Add (Vector (  300,  300, 0)); //Macro no centro do cenário
	//AUTOCODE SMALLS INICIO
    positionAlloc->Add (Vector (500.0,500.0, 0.0));
    positionAlloc->Add (Vector (500.0,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (500.0,0.0, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,0.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,500.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,0.0, 0.0));
    positionAlloc->Add (Vector (0.0,500.0, 0.0));
    positionAlloc->Add (Vector (0.0,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (0.0,166.66666666666666, 0.0));
	//AUTOCODE SMALLS FIM
	mobility.SetPositionAllocator(positionAlloc);
	mobility.Install(enbNodes);

	Ptr<ListPositionAllocator> positionAlloc2 = CreateObject<ListPositionAllocator> ();
	positionAlloc2->Add (Vector (  0,  10, 0));  // Server
	positionAlloc2->Add (Vector (  10, 10, 0));  // top
	positionAlloc2->Add (Vector ( 25, 10, 0));  // mid1
	positionAlloc2->Add (Vector ( 25, 20, 0));  // mid2
	positionAlloc2->Add (Vector ( 25, 30, 0));  // mid3
	positionAlloc2->Add (Vector ( 25, 40, 0));  // mid4
	positionAlloc2->Add (Vector ( 25, 50, 0));  // mid5
	positionAlloc2->Add (Vector ( 25, 60, 0));  // mid6
	positionAlloc2->Add (Vector (40, 10, 0));  // main
	MobilityHelper mobility2;
	mobility2.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility2.SetPositionAllocator(positionAlloc2);
	mobility2.Install(NodeContainer(remoteHostContainer,topswitch, midswitch, mainswitch));
	Ptr<ListPositionAllocator> positionAlloc3 = CreateObject<ListPositionAllocator> ();
	positionAlloc3->Add (Vector (  10,  20, 0));  // ctrl1
	positionAlloc3->Add (Vector (  40, 20, 0));  // ctrl2
	positionAlloc3->Add (Vector ( 50, 10, 0));  // pgw
	MobilityHelper mobility3;
	mobility3.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility3.SetPositionAllocator(positionAlloc3);
	mobility3.Install(NodeContainer( controllerNode,controllerNode2, pgw));
	// Install LTE Devices to the nodes
	NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);


	//--------------------------------------------Nós moveis
	NodeContainer ueNodes;
	MobilityHelper uesMobility;
	//double yvalue=0;

	//yvalue= i*distance;
	ueNodes.Create(numberOfNodes);
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
	
	Ptr<ListPositionAllocator> positionAlloc4 = CreateObject<ListPositionAllocator> ();
	//AUTOCODE USERS INICIO
    positionAlloc4->Add (Vector(364.0882917015389,496.49015742783445, 0.0));
    positionAlloc4->Add (Vector(109.60685498890554,436.8959112429031, 0.0));
    positionAlloc4->Add (Vector(168.6027173439123,114.97325591851765, 0.0));
    positionAlloc4->Add (Vector(291.357414955988,278.6037441439445, 0.0));
    positionAlloc4->Add (Vector(287.33669445550174,199.60520315948642, 0.0));
    positionAlloc4->Add (Vector(281.20960095221756,358.6499376373861, 0.0));
    positionAlloc4->Add (Vector(7.938192396490573,20.484539743490238, 0.0));
    positionAlloc4->Add (Vector(145.64073402277373,388.17932538912413, 0.0));
    positionAlloc4->Add (Vector(103.79395048129092,322.2912086888942, 0.0));
    positionAlloc4->Add (Vector(145.6489546411287,375.79787202268994, 0.0));
    positionAlloc4->Add (Vector(337.79621370036705,133.8870116813482, 0.0));
    positionAlloc4->Add (Vector(111.36682012458293,382.0501145323874, 0.0));
    positionAlloc4->Add (Vector(259.88725162900414,90.76025580765295, 0.0));
    positionAlloc4->Add (Vector(0.12561005531708247,61.116076034867405, 0.0));
    positionAlloc4->Add (Vector(293.24638375107594,409.3223916324253, 0.0));
    positionAlloc4->Add (Vector(193.66077646082124,448.344044100902, 0.0));
    positionAlloc4->Add (Vector(440.8241814763583,90.88875934998624, 0.0));
    positionAlloc4->Add (Vector(481.6332932062055,205.3615325840228, 0.0));
    positionAlloc4->Add (Vector(6.655664416859053,125.87648950385095, 0.0));
    positionAlloc4->Add (Vector(339.51020060939675,476.6852425951885, 0.0));
    positionAlloc4->Add (Vector(406.17552367538815,23.64661611060531, 0.0));
    positionAlloc4->Add (Vector(83.43840244215566,165.9336826127198, 0.0));
    positionAlloc4->Add (Vector(182.34488564146716,21.430469769122105, 0.0));
    positionAlloc4->Add (Vector(495.435113222758,24.001111182751778, 0.0));
    positionAlloc4->Add (Vector(137.01893726930848,449.5377791659447, 0.0));
    positionAlloc4->Add (Vector(240.89916412711437,90.08965743864805, 0.0));
    positionAlloc4->Add (Vector(362.2930411034837,95.76527464428636, 0.0));
    positionAlloc4->Add (Vector(443.25121002486895,255.70906030556208, 0.0));
    positionAlloc4->Add (Vector(211.56921149857672,171.96473587687822, 0.0));
    positionAlloc4->Add (Vector(223.78749753894823,247.76778799851817, 0.0));
    positionAlloc4->Add (Vector(5.977957795394895,306.9297619884796, 0.0));
    positionAlloc4->Add (Vector(110.94533412909114,94.25705611382223, 0.0));
    positionAlloc4->Add (Vector(406.61235927747765,406.21804370700903, 0.0));
    positionAlloc4->Add (Vector(226.99590465981578,171.22454529229685, 0.0));
    positionAlloc4->Add (Vector(339.66246723509397,439.62620601828166, 0.0));
    positionAlloc4->Add (Vector(124.79868675261713,432.02782105409875, 0.0));
    positionAlloc4->Add (Vector(174.91578849141547,421.51151959674434, 0.0));
    positionAlloc4->Add (Vector(402.51522171658144,207.3148292397008, 0.0));
    positionAlloc4->Add (Vector(129.94567349053386,278.0972010939038, 0.0));
    positionAlloc4->Add (Vector(338.31304732645555,112.62424908942504, 0.0));
    positionAlloc4->Add (Vector(279.63392886808265,123.3726869008287, 0.0));
    positionAlloc4->Add (Vector(47.10685373887213,157.54075248904448, 0.0));
    positionAlloc4->Add (Vector(47.921274799838656,113.42456409595553, 0.0));
    positionAlloc4->Add (Vector(42.02415432895113,401.6272644769099, 0.0));
    positionAlloc4->Add (Vector(268.8336193312812,493.97813986820637, 0.0));
    positionAlloc4->Add (Vector(252.0670619428757,217.25726573299124, 0.0));
    positionAlloc4->Add (Vector(197.93882992446942,327.22886150415985, 0.0));
    positionAlloc4->Add (Vector(432.1426443685562,200.46208032508568, 0.0));
    positionAlloc4->Add (Vector(63.42662661591264,18.114851578151068, 0.0));
    positionAlloc4->Add (Vector(400.8870989492258,103.6525978638116, 0.0));
    positionAlloc4->Add (Vector(444.649600300825,440.46117152150885, 0.0));
    positionAlloc4->Add (Vector(107.78429741465234,227.46783497174738, 0.0));
    positionAlloc4->Add (Vector(421.8679783181645,452.1465497523651, 0.0));
    positionAlloc4->Add (Vector(415.21449627044825,165.82119099115354, 0.0));
    positionAlloc4->Add (Vector(478.64029826540724,296.2873396101481, 0.0));
    positionAlloc4->Add (Vector(444.94991002831034,479.93127477253154, 0.0));
    positionAlloc4->Add (Vector(210.8155954107769,483.31723070826257, 0.0));
    positionAlloc4->Add (Vector(383.48956452862706,366.46829980466543, 0.0));
    positionAlloc4->Add (Vector(144.69834036091356,432.6371004317831, 0.0));
    positionAlloc4->Add (Vector(116.9234540164148,351.50919303937485, 0.0));
    positionAlloc4->Add (Vector(171.5429210812257,108.75250964015676, 0.0));
    positionAlloc4->Add (Vector(440.0348996180317,159.46106453536012, 0.0));
    positionAlloc4->Add (Vector(81.48368670672518,2.293942420002082, 0.0));
    positionAlloc4->Add (Vector(445.2563369202518,56.41467126490068, 0.0));
    positionAlloc4->Add (Vector(436.3103840812136,12.68735189719622, 0.0));
    positionAlloc4->Add (Vector(175.17128039114937,474.92339081077404, 0.0));
    positionAlloc4->Add (Vector(383.5606427276984,226.1629329732295, 0.0));
    positionAlloc4->Add (Vector(437.69891498887205,93.5896503174724, 0.0));
    positionAlloc4->Add (Vector(118.10377712160208,20.235804889295828, 0.0));
    positionAlloc4->Add (Vector(5.109690783326181,159.89148320437207, 0.0));
    positionAlloc4->Add (Vector(80.77059675059395,138.7167015241534, 0.0));
    positionAlloc4->Add (Vector(175.28169244531793,369.79911988621114, 0.0));
    positionAlloc4->Add (Vector(198.51771283820452,272.9923808408219, 0.0));
    positionAlloc4->Add (Vector(386.81377395298216,212.71519196732746, 0.0));
    positionAlloc4->Add (Vector(207.72854184103247,283.43006019525995, 0.0));
    positionAlloc4->Add (Vector(67.83911583260621,157.60630807847136, 0.0));
    positionAlloc4->Add (Vector(388.91435152094607,223.10300685245727, 0.0));
    positionAlloc4->Add (Vector(41.708568445731345,394.2815507344954, 0.0));
    positionAlloc4->Add (Vector(401.86878054839053,422.1217329277798, 0.0));
    positionAlloc4->Add (Vector(387.2817059379135,92.46204110984108, 0.0));
    positionAlloc4->Add (Vector(382.9972955340241,123.90213914885018, 0.0));
    positionAlloc4->Add (Vector(275.3460838393795,409.35056735531174, 0.0));
    positionAlloc4->Add (Vector(198.33015939727454,32.3116417116166, 0.0));
    positionAlloc4->Add (Vector(211.098206495889,177.61975675495052, 0.0));
    positionAlloc4->Add (Vector(483.54307754542947,465.76792021914014, 0.0));
    positionAlloc4->Add (Vector(36.8689101567492,43.99728762752025, 0.0));
    positionAlloc4->Add (Vector(57.07869340075522,39.22079818466867, 0.0));
    positionAlloc4->Add (Vector(491.5945172716408,430.5177943340822, 0.0));
    positionAlloc4->Add (Vector(358.0366442223696,16.252209019777975, 0.0));
    positionAlloc4->Add (Vector(330.7624660157292,259.13145571155064, 0.0));
    positionAlloc4->Add (Vector(191.50595402586274,430.42073391797845, 0.0));
    positionAlloc4->Add (Vector(322.2414380594537,121.54881615381625, 0.0));
    positionAlloc4->Add (Vector(73.06548417784053,68.51749786623435, 0.0));
    positionAlloc4->Add (Vector(40.245092190212276,209.81135709568304, 0.0));
    positionAlloc4->Add (Vector(293.38219040796895,365.018249575064, 0.0));
    positionAlloc4->Add (Vector(371.2259210640162,62.61995439532514, 0.0));
    positionAlloc4->Add (Vector(101.90598536240635,158.30169044679505, 0.0));
    positionAlloc4->Add (Vector(296.95711750041994,125.94297313534075, 0.0));
    positionAlloc4->Add (Vector(58.07406813867433,9.969117559036956, 0.0));
    positionAlloc4->Add (Vector(22.580593684330797,263.5512223599675, 0.0));
    positionAlloc4->Add (Vector(91.7377727549124,406.9954729026986, 0.0));
    positionAlloc4->Add (Vector(190.2500854341136,137.83562558973, 0.0));
    positionAlloc4->Add (Vector(123.38152733958901,54.065624730845144, 0.0));
    positionAlloc4->Add (Vector(140.81904824777448,444.81456132221854, 0.0));
    positionAlloc4->Add (Vector(160.92946148620902,206.4616016183643, 0.0));
    positionAlloc4->Add (Vector(440.68492579923276,346.1612137436624, 0.0));
    positionAlloc4->Add (Vector(55.38200810808208,471.70639861390276, 0.0));
    positionAlloc4->Add (Vector(255.13865640948256,239.79671997878415, 0.0));
    positionAlloc4->Add (Vector(137.67975643908048,33.49774954867429, 0.0));
    positionAlloc4->Add (Vector(453.3995502708547,84.15635325266192, 0.0));
    positionAlloc4->Add (Vector(4.403392576856014,376.8703445670869, 0.0));
    positionAlloc4->Add (Vector(93.23861343363492,379.0955633671809, 0.0));
    positionAlloc4->Add (Vector(263.16671067173525,62.962353427359005, 0.0));
    positionAlloc4->Add (Vector(18.485518023609426,364.79113502486035, 0.0));
    positionAlloc4->Add (Vector(60.24468575862063,293.0070712890123, 0.0));
    positionAlloc4->Add (Vector(293.15630159450035,499.6651048185733, 0.0));
    positionAlloc4->Add (Vector(278.8854055282305,353.1920564225005, 0.0));
    positionAlloc4->Add (Vector(236.54864832446327,397.3116251635836, 0.0));
    positionAlloc4->Add (Vector(285.0342261895156,52.9763486478797, 0.0));
    positionAlloc4->Add (Vector(417.3386129204383,151.7375811673592, 0.0));
    positionAlloc4->Add (Vector(459.9992313767778,37.167529388758446, 0.0));
    positionAlloc4->Add (Vector(186.91810964403376,62.63642520202928, 0.0));
    positionAlloc4->Add (Vector(327.57413299845166,99.20445881146017, 0.0));
    positionAlloc4->Add (Vector(425.2463588864595,129.7751342420328, 0.0));
    positionAlloc4->Add (Vector(252.6567555397423,374.5863805177432, 0.0));
    positionAlloc4->Add (Vector(302.3488366922687,8.401709528333189, 0.0));
    positionAlloc4->Add (Vector(286.8088377342564,489.5983545297427, 0.0));
    positionAlloc4->Add (Vector(412.52898348918217,60.39410004799506, 0.0));
    positionAlloc4->Add (Vector(472.7880093967931,263.19985641653284, 0.0));
    positionAlloc4->Add (Vector(434.14396754804466,269.92276119889823, 0.0));
    positionAlloc4->Add (Vector(402.0354597684426,482.51620796828627, 0.0));
    positionAlloc4->Add (Vector(388.94692172773176,352.47728805264455, 0.0));
    positionAlloc4->Add (Vector(188.15770417392287,393.6519526909069, 0.0));
    positionAlloc4->Add (Vector(119.32596228054648,115.38114975017733, 0.0));
    positionAlloc4->Add (Vector(482.99435219195806,492.51433438178253, 0.0));
    positionAlloc4->Add (Vector(115.65733385106685,142.49344876829977, 0.0));
    positionAlloc4->Add (Vector(263.8997585556758,20.61622516905065, 0.0));
    positionAlloc4->Add (Vector(422.0803588583355,225.4802257268695, 0.0));
    positionAlloc4->Add (Vector(85.53530988633507,243.75547879376074, 0.0));
    positionAlloc4->Add (Vector(53.011500097368604,103.69904178478112, 0.0));
    positionAlloc4->Add (Vector(145.70881520324903,442.04444178232586, 0.0));
    positionAlloc4->Add (Vector(285.0580281812682,83.88862552902965, 0.0));
    positionAlloc4->Add (Vector(435.6353346302986,290.23633315540826, 0.0));
    positionAlloc4->Add (Vector(236.6627766378729,484.54865627011594, 0.0));
    positionAlloc4->Add (Vector(489.77427705495427,260.38421192883453, 0.0));
    positionAlloc4->Add (Vector(10.751896640746384,321.62537828521675, 0.0));
    positionAlloc4->Add (Vector(196.90848446927606,208.6016030019146, 0.0));
    positionAlloc4->Add (Vector(437.71214722899447,227.75052259862827, 0.0));
    positionAlloc4->Add (Vector(257.78707407549615,352.8962120856949, 0.0));
    positionAlloc4->Add (Vector(294.071932134705,360.6944425370123, 0.0));
    positionAlloc4->Add (Vector(111.11251232329766,404.2119473502434, 0.0));
    positionAlloc4->Add (Vector(455.7933675602118,27.693478833203944, 0.0));
    positionAlloc4->Add (Vector(45.56117392275394,444.54283295967394, 0.0));
    positionAlloc4->Add (Vector(259.71978197337154,275.06220635763066, 0.0));
    positionAlloc4->Add (Vector(141.46115824204853,450.8491032908526, 0.0));
    positionAlloc4->Add (Vector(38.309193145749155,108.45091330539208, 0.0));
    positionAlloc4->Add (Vector(427.41853773559137,258.45745389555424, 0.0));
    positionAlloc4->Add (Vector(281.2107498412992,454.60245475742494, 0.0));
    positionAlloc4->Add (Vector(253.3875426921911,327.06227302548893, 0.0));
    positionAlloc4->Add (Vector(112.50417505099952,273.9389617713622, 0.0));
    positionAlloc4->Add (Vector(234.13485187949584,490.8398626653444, 0.0));
    positionAlloc4->Add (Vector(7.559961670207449,110.38210206104704, 0.0));
    positionAlloc4->Add (Vector(51.785959645310186,293.7647438114995, 0.0));
    positionAlloc4->Add (Vector(5.547703245843305,91.06036646781862, 0.0));
    positionAlloc4->Add (Vector(152.01450504467985,55.31106465605845, 0.0));
    positionAlloc4->Add (Vector(462.00399971074177,485.5335006733774, 0.0));
    positionAlloc4->Add (Vector(362.1530202976264,407.3965636225711, 0.0));
    positionAlloc4->Add (Vector(92.12530878993475,257.7009171982814, 0.0));
    positionAlloc4->Add (Vector(82.01531358954544,400.8516518022963, 0.0));
    positionAlloc4->Add (Vector(295.7047518010798,131.10119494781196, 0.0));
    positionAlloc4->Add (Vector(87.89362281259561,28.144178900676998, 0.0));
    positionAlloc4->Add (Vector(381.51711887991684,294.4616161963998, 0.0));
    positionAlloc4->Add (Vector(343.0558468092475,144.67950299718913, 0.0));
    positionAlloc4->Add (Vector(106.88133978458848,497.28242625131327, 0.0));
    positionAlloc4->Add (Vector(479.02848229793955,291.2288426834621, 0.0));
    positionAlloc4->Add (Vector(455.53211106553437,108.45114006965406, 0.0));
    positionAlloc4->Add (Vector(153.45193976723786,132.14849690277504, 0.0));
    positionAlloc4->Add (Vector(436.0230936527773,25.68854741564197, 0.0));
    positionAlloc4->Add (Vector(403.76145499181484,208.4194856222925, 0.0));
    positionAlloc4->Add (Vector(115.9592052948843,393.4431426364224, 0.0));
    positionAlloc4->Add (Vector(270.7818036602261,144.65432820556313, 0.0));
    positionAlloc4->Add (Vector(201.8371529830368,265.768124243511, 0.0));
    positionAlloc4->Add (Vector(126.14335131679616,1.2529249528702158, 0.0));
    positionAlloc4->Add (Vector(388.3088055875332,220.10445782685, 0.0));
    positionAlloc4->Add (Vector(468.11131168794856,9.478160458336937, 0.0));
    positionAlloc4->Add (Vector(139.08539653217213,56.61103459986305, 0.0));
    positionAlloc4->Add (Vector(77.55672444766392,339.99174336784796, 0.0));
    positionAlloc4->Add (Vector(475.9267294459525,168.48136949292746, 0.0));
    positionAlloc4->Add (Vector(321.2149057433306,400.6101423403058, 0.0));
    positionAlloc4->Add (Vector(423.64487790064624,471.9593520596447, 0.0));
    positionAlloc4->Add (Vector(29.86401679317574,155.85603688931815, 0.0));
    positionAlloc4->Add (Vector(233.13799235811516,345.0235810313675, 0.0));
    positionAlloc4->Add (Vector(488.2831051724953,199.16686271833257, 0.0));
    positionAlloc4->Add (Vector(263.8261479354261,253.8077167326756, 0.0));
    positionAlloc4->Add (Vector(194.74026384381915,446.28061616533654, 0.0));
    positionAlloc4->Add (Vector(208.2903729645046,267.13400830063523, 0.0));
    positionAlloc4->Add (Vector(497.7935303053157,218.2055237831924, 0.0));
    positionAlloc4->Add (Vector(29.68001002558601,151.6905853245541, 0.0));
    positionAlloc4->Add (Vector(337.5854244890418,18.940576070381354, 0.0));
    positionAlloc4->Add (Vector(454.30901266474586,328.12624895076306, 0.0));
    positionAlloc4->Add (Vector(253.94535666441755,270.14358639153284, 0.0));
    positionAlloc4->Add (Vector(114.78343522022271,75.63525408012079, 0.0));
    positionAlloc4->Add (Vector(127.95748792676082,140.31119861797103, 0.0));
    positionAlloc4->Add (Vector(63.444423097819616,243.43803863507168, 0.0));
    positionAlloc4->Add (Vector(180.8128499845978,326.7481826572901, 0.0));
    positionAlloc4->Add (Vector(344.31020333999453,125.89575505740585, 0.0));
    positionAlloc4->Add (Vector(157.41955813927632,370.9376818008953, 0.0));
    positionAlloc4->Add (Vector(60.031557125902694,455.40370516697163, 0.0));
    positionAlloc4->Add (Vector(374.1252004404274,39.03184107929103, 0.0));
    positionAlloc4->Add (Vector(171.09841016143812,4.11678203882987, 0.0));
    positionAlloc4->Add (Vector(354.1520369632329,224.10626874055285, 0.0));
    positionAlloc4->Add (Vector(331.2165360165607,24.476416833251967, 0.0));
    positionAlloc4->Add (Vector(478.21231297836465,262.34407153157906, 0.0));
    positionAlloc4->Add (Vector(167.61031469652409,323.27669787446007, 0.0));
    positionAlloc4->Add (Vector(451.0846585966326,356.9552031017188, 0.0));
    positionAlloc4->Add (Vector(445.1691987821764,267.11723068701264, 0.0));
    positionAlloc4->Add (Vector(62.63892931471887,77.06903245758478, 0.0));
    positionAlloc4->Add (Vector(55.37028572140146,357.93453643494774, 0.0));
    positionAlloc4->Add (Vector(387.55198221514917,79.30311414735452, 0.0));
    positionAlloc4->Add (Vector(347.4533147663073,48.98846442298838, 0.0));
    positionAlloc4->Add (Vector(103.90376932641348,27.70333659790297, 0.0));
    positionAlloc4->Add (Vector(142.3966496110477,125.63740740846214, 0.0));
    positionAlloc4->Add (Vector(31.744695514242526,491.3525865579266, 0.0));
    positionAlloc4->Add (Vector(82.29777473525951,211.9315436411519, 0.0));
    positionAlloc4->Add (Vector(380.5772095338453,56.03640861027043, 0.0));
    positionAlloc4->Add (Vector(243.58280728725745,315.0437330794849, 0.0));
    positionAlloc4->Add (Vector(359.9175338775683,122.35723621760991, 0.0));
    positionAlloc4->Add (Vector(147.64360634488722,123.4390234256909, 0.0));
    positionAlloc4->Add (Vector(252.39260823393235,168.04561867869876, 0.0));
    positionAlloc4->Add (Vector(258.87155496720845,446.66932580228143, 0.0));
    positionAlloc4->Add (Vector(129.42600876905868,415.534587541921, 0.0));
    positionAlloc4->Add (Vector(362.9610637400264,478.855887945326, 0.0));
    positionAlloc4->Add (Vector(191.90026057212762,210.65345720114948, 0.0));
    positionAlloc4->Add (Vector(300.11968481850806,29.47437456317481, 0.0));
    positionAlloc4->Add (Vector(307.0715332778229,185.71208625598896, 0.0));
    positionAlloc4->Add (Vector(309.6709241494949,68.64968866995468, 0.0));
    positionAlloc4->Add (Vector(395.7154490892713,482.1979386354256, 0.0));
    positionAlloc4->Add (Vector(386.8126033349107,249.77223673943277, 0.0));
    positionAlloc4->Add (Vector(233.96914843162043,171.12947821954188, 0.0));
    positionAlloc4->Add (Vector(417.05164059737155,43.0114409993701, 0.0));
    positionAlloc4->Add (Vector(300.4895619533302,317.74473089486884, 0.0));
    positionAlloc4->Add (Vector(498.14355212579983,245.14369416224025, 0.0));
    positionAlloc4->Add (Vector(382.9411430861177,441.87956430651894, 0.0));
    positionAlloc4->Add (Vector(33.111754356132394,212.11453479540467, 0.0));
    positionAlloc4->Add (Vector(43.21942696533848,55.771547678016894, 0.0));
    positionAlloc4->Add (Vector(202.3368146695893,8.176767580889754, 0.0));
    positionAlloc4->Add (Vector(365.2285606418734,213.69807521910593, 0.0));
    positionAlloc4->Add (Vector(319.3442673876787,413.01694862474267, 0.0));
    positionAlloc4->Add (Vector(11.955990006866424,82.75281687795605, 0.0));
    positionAlloc4->Add (Vector(342.9393592322488,390.6274095086417, 0.0));
    positionAlloc4->Add (Vector(243.69511397860393,150.15912175042945, 0.0));
    positionAlloc4->Add (Vector(131.20044898746636,17.835759720505383, 0.0));
    positionAlloc4->Add (Vector(485.0543609153277,121.47364832533297, 0.0));
    positionAlloc4->Add (Vector(477.7050169218872,41.42445847808246, 0.0));
    positionAlloc4->Add (Vector(223.52305638154712,105.63405708174372, 0.0));
    positionAlloc4->Add (Vector(91.7892012656431,197.5651329687304, 0.0));
    positionAlloc4->Add (Vector(297.0450173254759,21.556623299556765, 0.0));
    positionAlloc4->Add (Vector(431.7357048249892,486.3775240645692, 0.0));
    positionAlloc4->Add (Vector(324.57040505996696,4.053417310261375, 0.0));
    positionAlloc4->Add (Vector(351.03546378570843,320.862405778384, 0.0));
    positionAlloc4->Add (Vector(125.07509086620122,222.12682348194585, 0.0));
    positionAlloc4->Add (Vector(222.076567543098,499.082107648127, 0.0));
    positionAlloc4->Add (Vector(111.40260783820288,403.3113885710795, 0.0));
    positionAlloc4->Add (Vector(28.076832541769015,367.7814630182146, 0.0));
    positionAlloc4->Add (Vector(158.31774840605473,297.4466640706114, 0.0));
	//AUTOCODE USERS FIM

	uesMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	uesMobility.SetPositionAllocator(positionAlloc4);
	uesMobility.Install (ueNodes);

	NetDeviceContainer ueLteDevs;
	
	ueLteDevs = lteHelper->InstallUeDevice (ueNodes);
//-----------------------------------------------------Routing
	internet.Install (ueNodes);
	
	Ipv4InterfaceContainer ueIpIface; 

	ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));

	
	for (uint32_t u = 0; u < numberOfNodes; u++)
		{
		Ptr<Node> ueNode_sg = ueNodes.Get (u);
		Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode_sg->GetObject<Ipv4> ());
		ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
		}
	 // for (int i = 0; i < numberOfRrhs; i++)
	 // {
		// for (uint16_t u = 0; u < numberOfNodes[i]; u++)
		// 	{
		// 		lteHelper->Attach (ueLteDevs[i].Get(u), enbLteDevs.Get(i));
		// 	}
	 // }

		lteHelper->Attach (ueLteDevs);
//-----------------------------------------------------Bearer
	for (uint32_t u = 0; u < numberOfNodes; u++)
		{
			Ptr<NetDevice> ueDevice = ueLteDevs.Get (u);
			GbrQosInformation qos;
			qos.gbrDl = 132;  // bit/s, considering IP, UDP, RLC, PDCP header size
			qos.gbrUl = 132;
			qos.mbrDl = qos.gbrDl;
			qos.mbrUl = qos.gbrUl;

			enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
			EpsBearer bearer (q, qos);
			bearer.arp.preemptionCapability = true;
			bearer.arp.preemptionVulnerability = true;
			lteHelper->ActivateDedicatedEpsBearer (ueDevice, bearer, EpcTft::Default ());
		}
//-----------------------------------------------------Aplicação
		 //uint16_t dlPort = 900;
		 uint16_t ulPort = 900;
		 ApplicationContainer clientApps;
		 ApplicationContainer serverApps;
		 Ptr<UniformRandomVariable> startTimeSeconds = CreateObject<UniformRandomVariable> ();
		 startTimeSeconds->SetAttribute ("Min", DoubleValue (2.0));
		 startTimeSeconds->SetAttribute ("Max", DoubleValue (2.020));
			
			//++ulPort;
			//++dlPort;
			//PacketSinkHelper dlPacketSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
			PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
			//PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), otherPort));
			//serverApps.Add (dlPacketSinkHelper.Install (ueNodes[u]));
			serverApps.Add (ulPacketSinkHelper.Install (remoteHost));
			//serverApps.Add (packetSinkHelper.Install (ueNodes.Get(u))); 
			//BulkSendHelper dlClient ("ns3::TcpSocketFactory", InetSocketAddress (ueIpIface.GetAddress (u), dlPort));
			//dlClient.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
			//dlClient.SetAttribute ("MaxPackets", UintegerValue(1000000));
			
			//BulkSendHelper ulClient ("ns3::TcpSocketFactory", InetSocketAddress (remoteHostAddr, ulPort));
			UdpClientHelper ulClient (remoteHostAddr, ulPort);
			ulClient.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
			ulClient.SetAttribute ("MaxPackets", UintegerValue(1000000));

			//UdpClientHelper client (ueIpIface.GetAddress (u), otherPort);
			//client.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
			//client.SetAttribute ("MaxPackets", UintegerValue(1000000));

			//clientApps.Add (dlClient.Install (remoteHost));
			clientApps.Add (ulClient.Install (ueNodes));
			Time startTime = Seconds (startTimeSeconds->GetValue ());
			serverApps.Start (startTime);
			clientApps.Start (startTime);
		 clientApps.Stop (Seconds (simTime));
		 

		 Ptr<FlowMonitor> flowmon;
		 FlowMonitorHelper flowmonHelper;
		 flowmon = flowmonHelper.InstallAll (); 
		 if (trace)
			{
				ofMyCtrlHelper->EnableOpenFlowPcap ("openflow");
				ofMyCtrlHelper->EnableDatapathStats ("switch-stats");
				csmaHelper.EnablePcap ("mainswitch", mainswitchC);
				csmaHelper.EnablePcap ("topswitch", topswitchC);
				csmaHelper.EnablePcap ("midswitch", midswitch);
				csmaHelper.EnablePcap ("server", hostDevices);
				//lteHelper->EnableTraces ();
				lteHelper->EnablePhyTraces ();
				lteHelper->EnableMacTraces ();
				lteHelper->EnableRlcTraces ();
				lteHelper->EnablePdcpTraces ();
			}
		 
		 AnimationInterface anim ("animation.xml");
		 Simulator::Stop(Seconds(simTime));
		 // AnimationInterface anim ("cran.xml");
		 anim.SetMaxPktsPerTraceFile (999999);
//-----------------------------------------------------Handover
	// Add X2 inteface
	lteHelper->AddX2Interface (enbNodes);
	// X2-based Handover
	//lteHelper->HandoverRequest (Seconds (3.0), ueLteDevs[1].Get (0), enbLteDevs.Get (1), enbLteDevs.Get (2));

		//Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/ConnectionEstablished",
		//               MakeCallback (&MapearUE));
		Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/ConnectionEstablished",
									 MakeCallback (&NotifyConnectionEstablishedUe));
		Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverEndOk",
									 MakeCallback (&MyController::Update, MyCtrl));
//-----------------------------------------------------Schedule/Callbacks
		Simulator::Schedule(Seconds(0.5),&CallMap, epcHelper);
		Simulator::Schedule(Seconds(1.5),&SendToController, MyCtrl);
		// std::map <uint64_t, Ipv4Address> mymap = epcHelper->RecvMymap();
		// for (std::map<uint64_t, Ipv4Address >::iterator it=mymap.begin(); it!=mymap.end(); ++it)
		// std::cout << it->first << " => " << it->second << '\n';

	 Simulator::Run ();
	 Simulator::Destroy();
	 //INICIO FLOW MONITOR
	  flowmon->SerializeToXmlFile ("scratch/switch_HDSO_flowmon/switch_HDSO17.flowmon", false, false);
	 //FIM FLOW MONITOR
		Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (serverApps.Get (0));
		std::cout << "Bytes received by server 1: " << sink1->GetTotalRx () << " ("
							<< (8. * sink1->GetTotalRx ()) / 1000000 / simTime << " Mbps)"
							<< std::endl;
	 return 0;
	}