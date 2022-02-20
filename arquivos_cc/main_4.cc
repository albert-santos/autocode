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

	// std::map <uint64_t, Ipv4Address> mymap;
	// std::map <uint64_t, uint64_t> mymap2;
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
	  uint16_t numberOfRrhs = 12;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 172;
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
    positionAlloc->Add (Vector (500.0,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (500.0,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (500.0,0.0, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,500.0, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,500.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,166.66666666666666, 0.0));
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
    positionAlloc4->Add (Vector(289.4515280278366,96.33675606181713, 0.0));
    positionAlloc4->Add (Vector(9.624795610406633,155.848100609928, 0.0));
    positionAlloc4->Add (Vector(473.1828837284055,74.65610369628978, 0.0));
    positionAlloc4->Add (Vector(25.55001725261663,165.34218241993264, 0.0));
    positionAlloc4->Add (Vector(260.77501093683065,25.348318333163867, 0.0));
    positionAlloc4->Add (Vector(56.549827445675106,232.99372338282782, 0.0));
    positionAlloc4->Add (Vector(119.82052151456213,88.45601685718368, 0.0));
    positionAlloc4->Add (Vector(415.5715789703392,7.436301368594855, 0.0));
    positionAlloc4->Add (Vector(228.7976302555367,335.21624962621064, 0.0));
    positionAlloc4->Add (Vector(9.773768789473781,406.77670491484787, 0.0));
    positionAlloc4->Add (Vector(98.33076388524775,246.49175657653544, 0.0));
    positionAlloc4->Add (Vector(447.5942899276613,419.206903382555, 0.0));
    positionAlloc4->Add (Vector(265.03346913384684,349.6088854938342, 0.0));
    positionAlloc4->Add (Vector(27.289761990813343,274.3630521473029, 0.0));
    positionAlloc4->Add (Vector(256.3545848243833,248.98445471429233, 0.0));
    positionAlloc4->Add (Vector(17.78445669189238,378.5116209065139, 0.0));
    positionAlloc4->Add (Vector(327.0391004208361,85.8707461525588, 0.0));
    positionAlloc4->Add (Vector(123.50030870918211,101.85885327425748, 0.0));
    positionAlloc4->Add (Vector(137.09265348989157,258.38088928450173, 0.0));
    positionAlloc4->Add (Vector(394.0572919984027,413.41016767337123, 0.0));
    positionAlloc4->Add (Vector(9.548685740709217,476.7137151227739, 0.0));
    positionAlloc4->Add (Vector(176.91487254682642,233.46580563701374, 0.0));
    positionAlloc4->Add (Vector(38.10150780874161,191.781798376995, 0.0));
    positionAlloc4->Add (Vector(298.9085828898939,12.411958442404558, 0.0));
    positionAlloc4->Add (Vector(41.494790855434005,122.85146749370568, 0.0));
    positionAlloc4->Add (Vector(340.1801833215603,199.69480357778824, 0.0));
    positionAlloc4->Add (Vector(265.4427302481811,365.78923574566187, 0.0));
    positionAlloc4->Add (Vector(276.0110395722431,281.90586150793837, 0.0));
    positionAlloc4->Add (Vector(145.3072378036271,145.05261968962313, 0.0));
    positionAlloc4->Add (Vector(299.5997453134865,400.538156479823, 0.0));
    positionAlloc4->Add (Vector(490.4804397566623,270.2629805405182, 0.0));
    positionAlloc4->Add (Vector(429.83064851693723,46.51318555616846, 0.0));
    positionAlloc4->Add (Vector(164.855709376644,410.7479862683691, 0.0));
    positionAlloc4->Add (Vector(206.22195874213173,374.9887067237426, 0.0));
    positionAlloc4->Add (Vector(56.28458491533844,139.95262245640194, 0.0));
    positionAlloc4->Add (Vector(445.9774499296132,303.701362886099, 0.0));
    positionAlloc4->Add (Vector(345.92164738759817,39.8511512840134, 0.0));
    positionAlloc4->Add (Vector(313.5209609910157,4.152352304108097, 0.0));
    positionAlloc4->Add (Vector(361.0365990426058,284.09596341416244, 0.0));
    positionAlloc4->Add (Vector(444.39995291088957,98.23830039190695, 0.0));
    positionAlloc4->Add (Vector(49.008738181018195,117.62310264993725, 0.0));
    positionAlloc4->Add (Vector(403.36043749239775,294.8725189228027, 0.0));
    positionAlloc4->Add (Vector(209.6044624046558,50.61945304986787, 0.0));
    positionAlloc4->Add (Vector(398.1662704309386,97.82941340636347, 0.0));
    positionAlloc4->Add (Vector(78.19719857892792,353.2313915467906, 0.0));
    positionAlloc4->Add (Vector(256.10281554066995,191.41351841003467, 0.0));
    positionAlloc4->Add (Vector(318.18320501689243,39.45894830266805, 0.0));
    positionAlloc4->Add (Vector(254.0399675046334,243.41173666515226, 0.0));
    positionAlloc4->Add (Vector(236.65488255338062,16.475083175320815, 0.0));
    positionAlloc4->Add (Vector(102.8701439334968,461.11795581739557, 0.0));
    positionAlloc4->Add (Vector(499.41662125708575,66.63448378967817, 0.0));
    positionAlloc4->Add (Vector(23.888881719926314,177.43399314965208, 0.0));
    positionAlloc4->Add (Vector(181.17699763157825,43.55549617755011, 0.0));
    positionAlloc4->Add (Vector(393.13327432209604,244.0154825500016, 0.0));
    positionAlloc4->Add (Vector(208.26400033927754,352.17792757131497, 0.0));
    positionAlloc4->Add (Vector(292.82338456905086,372.47130224610487, 0.0));
    positionAlloc4->Add (Vector(135.42809177221693,451.597294095631, 0.0));
    positionAlloc4->Add (Vector(140.55672366028293,438.48077491897305, 0.0));
    positionAlloc4->Add (Vector(68.7871029140028,48.03991859702927, 0.0));
    positionAlloc4->Add (Vector(278.3121909211187,373.0483281821238, 0.0));
    positionAlloc4->Add (Vector(177.66173770067306,298.2536045766659, 0.0));
    positionAlloc4->Add (Vector(352.47707569334307,419.11731022371987, 0.0));
    positionAlloc4->Add (Vector(390.9782525839189,375.68061311978704, 0.0));
    positionAlloc4->Add (Vector(335.9832941596548,79.30495483390249, 0.0));
    positionAlloc4->Add (Vector(310.45833240795525,81.46644469602971, 0.0));
    positionAlloc4->Add (Vector(318.13029733772066,472.9873584194016, 0.0));
    positionAlloc4->Add (Vector(308.8245342274998,209.7447545408757, 0.0));
    positionAlloc4->Add (Vector(461.38323778127784,44.06631826409879, 0.0));
    positionAlloc4->Add (Vector(146.82483657936135,495.7834566037177, 0.0));
    positionAlloc4->Add (Vector(117.46344813436855,102.19837038433899, 0.0));
    positionAlloc4->Add (Vector(394.373963195789,313.3890760395894, 0.0));
    positionAlloc4->Add (Vector(323.2775572155083,242.12957590058392, 0.0));
    positionAlloc4->Add (Vector(15.674401983093055,24.602531294636464, 0.0));
    positionAlloc4->Add (Vector(434.06596506752896,344.11312420512735, 0.0));
    positionAlloc4->Add (Vector(453.68786415272336,339.60838702462763, 0.0));
    positionAlloc4->Add (Vector(65.63057367135977,227.59949310971993, 0.0));
    positionAlloc4->Add (Vector(88.75722109117584,341.37374284368786, 0.0));
    positionAlloc4->Add (Vector(64.08541811527697,104.82627249003635, 0.0));
    positionAlloc4->Add (Vector(133.5974614149078,191.2140697450111, 0.0));
    positionAlloc4->Add (Vector(202.30150329833597,411.0130990996165, 0.0));
    positionAlloc4->Add (Vector(448.77155820030316,212.97562813654864, 0.0));
    positionAlloc4->Add (Vector(471.3841095913975,202.44424317336657, 0.0));
    positionAlloc4->Add (Vector(250.64404173649874,453.60196119201277, 0.0));
    positionAlloc4->Add (Vector(434.0172247265236,80.08059979662463, 0.0));
    positionAlloc4->Add (Vector(75.56086267789996,177.68956609730603, 0.0));
    positionAlloc4->Add (Vector(368.2838882506834,483.1974293360757, 0.0));
    positionAlloc4->Add (Vector(402.76230237404224,299.550912483104, 0.0));
    positionAlloc4->Add (Vector(243.07216696480876,461.51974479629484, 0.0));
    positionAlloc4->Add (Vector(382.43185913607834,31.13408727671324, 0.0));
    positionAlloc4->Add (Vector(130.76260147319212,319.35961981030647, 0.0));
    positionAlloc4->Add (Vector(462.47134512184465,421.2503838230382, 0.0));
    positionAlloc4->Add (Vector(185.17713088002668,324.7092105693772, 0.0));
    positionAlloc4->Add (Vector(148.38709808840545,105.67495257325365, 0.0));
    positionAlloc4->Add (Vector(204.60504207221348,244.00811834596175, 0.0));
    positionAlloc4->Add (Vector(27.066455800756838,37.085622643961145, 0.0));
    positionAlloc4->Add (Vector(275.07378982079115,118.27345912834753, 0.0));
    positionAlloc4->Add (Vector(396.4457140427082,444.5963855126863, 0.0));
    positionAlloc4->Add (Vector(414.1035225159615,461.8618224228628, 0.0));
    positionAlloc4->Add (Vector(257.0972160467593,24.749477016797027, 0.0));
    positionAlloc4->Add (Vector(193.79173068111928,14.63977375621478, 0.0));
    positionAlloc4->Add (Vector(182.62411896059416,73.5506786619497, 0.0));
    positionAlloc4->Add (Vector(322.7359621310291,33.99283722128504, 0.0));
    positionAlloc4->Add (Vector(438.89845942808324,28.489740800949793, 0.0));
    positionAlloc4->Add (Vector(105.36622489408326,462.51044317402614, 0.0));
    positionAlloc4->Add (Vector(314.11936535827147,415.75013059472167, 0.0));
    positionAlloc4->Add (Vector(103.23397090676917,365.2482459576544, 0.0));
    positionAlloc4->Add (Vector(159.25423585844234,295.0539381049118, 0.0));
    positionAlloc4->Add (Vector(366.13257320525963,481.2307520045733, 0.0));
    positionAlloc4->Add (Vector(68.79198148007853,426.56192914127445, 0.0));
    positionAlloc4->Add (Vector(116.54556667409565,232.07899063571136, 0.0));
    positionAlloc4->Add (Vector(24.26852520712369,7.208877635640154, 0.0));
    positionAlloc4->Add (Vector(238.08594347425765,258.1049641364715, 0.0));
    positionAlloc4->Add (Vector(85.51211235151452,76.30487176175527, 0.0));
    positionAlloc4->Add (Vector(498.1222752216701,80.65171171971214, 0.0));
    positionAlloc4->Add (Vector(270.38729219225297,17.29641799952869, 0.0));
    positionAlloc4->Add (Vector(124.47632739573055,346.7273688981114, 0.0));
    positionAlloc4->Add (Vector(142.0583598177526,373.14141774717524, 0.0));
    positionAlloc4->Add (Vector(183.3620988239394,0.6879676066479656, 0.0));
    positionAlloc4->Add (Vector(323.47996830220904,205.62885485577854, 0.0));
    positionAlloc4->Add (Vector(159.33999203153516,98.50531254381694, 0.0));
    positionAlloc4->Add (Vector(131.0497572622426,378.88600648885824, 0.0));
    positionAlloc4->Add (Vector(113.72987700054682,37.426777227345056, 0.0));
    positionAlloc4->Add (Vector(121.62075044261955,202.9866918021479, 0.0));
    positionAlloc4->Add (Vector(492.5565758507454,69.51140190221922, 0.0));
    positionAlloc4->Add (Vector(407.0895883260011,165.13522225503803, 0.0));
    positionAlloc4->Add (Vector(243.04912404841807,420.92844541230875, 0.0));
    positionAlloc4->Add (Vector(477.53153623794094,207.35626190101593, 0.0));
    positionAlloc4->Add (Vector(106.10022386150747,387.5866761705865, 0.0));
    positionAlloc4->Add (Vector(135.86770957303563,369.169424575896, 0.0));
    positionAlloc4->Add (Vector(337.19376802227947,494.5094666494722, 0.0));
    positionAlloc4->Add (Vector(210.26368366866777,84.42442795186771, 0.0));
    positionAlloc4->Add (Vector(271.77742498236785,390.11293211491636, 0.0));
    positionAlloc4->Add (Vector(134.12815040525217,25.013597944793243, 0.0));
    positionAlloc4->Add (Vector(340.8577096576312,472.80350278108455, 0.0));
    positionAlloc4->Add (Vector(336.14379387465743,281.96776445816846, 0.0));
    positionAlloc4->Add (Vector(108.94527316652142,29.806245342185676, 0.0));
    positionAlloc4->Add (Vector(454.7783229325433,282.13744305389883, 0.0));
    positionAlloc4->Add (Vector(294.763158418124,4.600460865619215, 0.0));
    positionAlloc4->Add (Vector(495.500688303417,256.2217581106334, 0.0));
    positionAlloc4->Add (Vector(171.3300835346689,12.796040480884452, 0.0));
    positionAlloc4->Add (Vector(378.853464913113,378.8051788734549, 0.0));
    positionAlloc4->Add (Vector(268.79398191283786,256.4129140486572, 0.0));
    positionAlloc4->Add (Vector(472.0623908403887,308.0644557599179, 0.0));
    positionAlloc4->Add (Vector(160.41938856509137,342.15818338565987, 0.0));
    positionAlloc4->Add (Vector(316.955784730754,113.58147849554007, 0.0));
    positionAlloc4->Add (Vector(217.98562321957172,294.1499775521622, 0.0));
    positionAlloc4->Add (Vector(231.34505554817136,144.08776918611426, 0.0));
    positionAlloc4->Add (Vector(275.6510157560721,145.06231417839666, 0.0));
    positionAlloc4->Add (Vector(329.3170066647314,8.634898043485995, 0.0));
    positionAlloc4->Add (Vector(303.2803286121375,191.81241920619613, 0.0));
    positionAlloc4->Add (Vector(266.3929070718121,362.5112121856948, 0.0));
    positionAlloc4->Add (Vector(321.63196061985127,492.2577788465204, 0.0));
    positionAlloc4->Add (Vector(290.3122542780383,297.9975301535052, 0.0));
    positionAlloc4->Add (Vector(122.30366413143828,10.97962883785103, 0.0));
    positionAlloc4->Add (Vector(151.0040820802176,158.8913762485989, 0.0));
    positionAlloc4->Add (Vector(363.85315399355176,246.60951743110988, 0.0));
    positionAlloc4->Add (Vector(45.79906750237717,148.22284920444739, 0.0));
    positionAlloc4->Add (Vector(354.2694777975546,49.30974187586745, 0.0));
    positionAlloc4->Add (Vector(205.77861959940924,192.63050871821707, 0.0));
    positionAlloc4->Add (Vector(369.27239369639585,73.74842698100487, 0.0));
    positionAlloc4->Add (Vector(477.4289522983526,132.91086176182083, 0.0));
    positionAlloc4->Add (Vector(279.14785950567625,423.429381063295, 0.0));
    positionAlloc4->Add (Vector(315.6598149725843,87.62761872564478, 0.0));
    positionAlloc4->Add (Vector(50.60301469159767,143.28077312194264, 0.0));
    positionAlloc4->Add (Vector(91.83092999777209,354.16552720005075, 0.0));
    positionAlloc4->Add (Vector(477.31173570028517,278.55480626200597, 0.0));
    positionAlloc4->Add (Vector(431.2533236611447,242.28890861693304, 0.0));
    positionAlloc4->Add (Vector(160.71626603701438,470.88273219887225, 0.0));
    positionAlloc4->Add (Vector(60.065455900670585,261.8608097488119, 0.0));
    positionAlloc4->Add (Vector(190.49176171597676,244.3506182366142, 0.0));
    positionAlloc4->Add (Vector(5.420671967974188,392.80989070393736, 0.0));
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
	  flowmon->SerializeToXmlFile ("scratch/switch_HDSO_flowmon/switch_HDSO4.flowmon", false, false);
	 //FIM FLOW MONITOR
		Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (serverApps.Get (0));
		std::cout << "Bytes received by server 1: " << sink1->GetTotalRx () << " ("
							<< (8. * sink1->GetTotalRx ()) / 1000000 / simTime << " Mbps)"
							<< std::endl;
	 return 0;
	}