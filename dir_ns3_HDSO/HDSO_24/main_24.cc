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
	  uint16_t numberOfRrhs = 15;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 360;
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
    positionAlloc->Add (Vector (333.3333333333333,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,0.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,500.0, 0.0));
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
    positionAlloc4->Add (Vector(429.38421168088786,490.1908165442285, 0.0));
    positionAlloc4->Add (Vector(403.57770033799795,253.97776097130836, 0.0));
    positionAlloc4->Add (Vector(470.83390115880377,394.111087925391, 0.0));
    positionAlloc4->Add (Vector(437.38838095867214,15.5386961935694, 0.0));
    positionAlloc4->Add (Vector(203.19965229195515,383.0060335532095, 0.0));
    positionAlloc4->Add (Vector(425.46610977272684,487.4232758201134, 0.0));
    positionAlloc4->Add (Vector(145.50733967782458,105.77981547024918, 0.0));
    positionAlloc4->Add (Vector(496.74812816844906,291.6989143628269, 0.0));
    positionAlloc4->Add (Vector(206.61370346725093,164.8717555398342, 0.0));
    positionAlloc4->Add (Vector(319.45967211428706,442.53598816007445, 0.0));
    positionAlloc4->Add (Vector(297.40414822531034,479.76912200498583, 0.0));
    positionAlloc4->Add (Vector(473.8350881382836,185.816280468669, 0.0));
    positionAlloc4->Add (Vector(282.78015186166806,181.92946477196037, 0.0));
    positionAlloc4->Add (Vector(406.8424812147747,253.72365474369778, 0.0));
    positionAlloc4->Add (Vector(386.35204393828957,164.95300777138684, 0.0));
    positionAlloc4->Add (Vector(6.218837807554001,35.95524222433993, 0.0));
    positionAlloc4->Add (Vector(303.83795649737743,138.76394274929217, 0.0));
    positionAlloc4->Add (Vector(422.1443424932251,152.69515357087448, 0.0));
    positionAlloc4->Add (Vector(416.66371760159274,353.0787307836821, 0.0));
    positionAlloc4->Add (Vector(185.41879705169939,247.19290028605033, 0.0));
    positionAlloc4->Add (Vector(466.824909969288,317.0068454517618, 0.0));
    positionAlloc4->Add (Vector(492.26052172651646,177.16567916354558, 0.0));
    positionAlloc4->Add (Vector(313.42723522648845,2.5750203682974315, 0.0));
    positionAlloc4->Add (Vector(112.53925605538522,194.29960267582135, 0.0));
    positionAlloc4->Add (Vector(196.04725593573414,352.3558508931086, 0.0));
    positionAlloc4->Add (Vector(323.52415886312735,212.4415716109337, 0.0));
    positionAlloc4->Add (Vector(466.7761439634847,292.31539487027595, 0.0));
    positionAlloc4->Add (Vector(63.57365831550138,370.21298161330765, 0.0));
    positionAlloc4->Add (Vector(329.5542765727256,321.5188780267488, 0.0));
    positionAlloc4->Add (Vector(194.46861797586556,477.470236656182, 0.0));
    positionAlloc4->Add (Vector(196.68770115012867,242.7707137316672, 0.0));
    positionAlloc4->Add (Vector(67.9076543792988,440.1784656455742, 0.0));
    positionAlloc4->Add (Vector(63.78197960471061,101.93358288628285, 0.0));
    positionAlloc4->Add (Vector(46.12730457513348,432.32673065733036, 0.0));
    positionAlloc4->Add (Vector(461.5671252606803,386.65221645132186, 0.0));
    positionAlloc4->Add (Vector(221.37123885087294,24.44983798539352, 0.0));
    positionAlloc4->Add (Vector(235.47533971582118,273.4369732010347, 0.0));
    positionAlloc4->Add (Vector(242.79409396095897,81.06417389188975, 0.0));
    positionAlloc4->Add (Vector(106.92630752377919,71.23568704030536, 0.0));
    positionAlloc4->Add (Vector(254.98258410583475,187.98286690898846, 0.0));
    positionAlloc4->Add (Vector(154.38251046376456,223.53244306343268, 0.0));
    positionAlloc4->Add (Vector(231.3910059298075,194.26038945366682, 0.0));
    positionAlloc4->Add (Vector(18.704846486558512,332.2578145970248, 0.0));
    positionAlloc4->Add (Vector(360.97599461516234,392.82636528509755, 0.0));
    positionAlloc4->Add (Vector(377.6087092359913,30.375806993511723, 0.0));
    positionAlloc4->Add (Vector(76.42656317657077,445.0023788120588, 0.0));
    positionAlloc4->Add (Vector(428.6434059185059,323.9774053817704, 0.0));
    positionAlloc4->Add (Vector(115.97471587233349,61.962644822798396, 0.0));
    positionAlloc4->Add (Vector(150.50235837292902,156.22960237580708, 0.0));
    positionAlloc4->Add (Vector(29.53967832277582,129.82833099845115, 0.0));
    positionAlloc4->Add (Vector(79.06195959990086,335.57287582109825, 0.0));
    positionAlloc4->Add (Vector(185.4078409945521,364.26470595917147, 0.0));
    positionAlloc4->Add (Vector(195.7570272980681,233.258649057421, 0.0));
    positionAlloc4->Add (Vector(174.558130864903,456.35798954839913, 0.0));
    positionAlloc4->Add (Vector(29.610129704327804,24.756228021345027, 0.0));
    positionAlloc4->Add (Vector(390.1465890277781,360.2654709062763, 0.0));
    positionAlloc4->Add (Vector(281.6357243770606,67.16488238429518, 0.0));
    positionAlloc4->Add (Vector(18.416095887230032,106.92381850617211, 0.0));
    positionAlloc4->Add (Vector(438.0030349432247,20.646589589842, 0.0));
    positionAlloc4->Add (Vector(147.30623954531774,215.2193241739146, 0.0));
    positionAlloc4->Add (Vector(306.34499392241554,66.82463429880514, 0.0));
    positionAlloc4->Add (Vector(208.15914190302377,363.5179677498423, 0.0));
    positionAlloc4->Add (Vector(239.23268692280985,8.55757958660014, 0.0));
    positionAlloc4->Add (Vector(186.78725616336112,376.2731917733487, 0.0));
    positionAlloc4->Add (Vector(172.18167042860995,476.20888891753026, 0.0));
    positionAlloc4->Add (Vector(434.8251898508153,248.50143643378968, 0.0));
    positionAlloc4->Add (Vector(101.87773026055014,331.4967861682846, 0.0));
    positionAlloc4->Add (Vector(239.84166353863594,362.5633894599914, 0.0));
    positionAlloc4->Add (Vector(106.9368334731079,24.020815535394757, 0.0));
    positionAlloc4->Add (Vector(375.7929132560949,262.27525145604153, 0.0));
    positionAlloc4->Add (Vector(302.95666470214314,149.598119012403, 0.0));
    positionAlloc4->Add (Vector(90.47074085016321,226.00700673416412, 0.0));
    positionAlloc4->Add (Vector(431.9593890911885,105.6572245881044, 0.0));
    positionAlloc4->Add (Vector(307.40817095640466,367.07997377423936, 0.0));
    positionAlloc4->Add (Vector(231.5519555211824,261.0579776558504, 0.0));
    positionAlloc4->Add (Vector(11.024942460273135,205.04107943119638, 0.0));
    positionAlloc4->Add (Vector(390.541537897949,451.732393355915, 0.0));
    positionAlloc4->Add (Vector(51.65392929161533,92.51802946514486, 0.0));
    positionAlloc4->Add (Vector(379.0904968086338,404.4764270962121, 0.0));
    positionAlloc4->Add (Vector(469.2614648009861,245.38518989924685, 0.0));
    positionAlloc4->Add (Vector(9.04346189494898,355.42725388154406, 0.0));
    positionAlloc4->Add (Vector(300.8822544314138,429.39771208339283, 0.0));
    positionAlloc4->Add (Vector(13.941702988168492,210.35613791956797, 0.0));
    positionAlloc4->Add (Vector(362.7417693734106,457.39969034447927, 0.0));
    positionAlloc4->Add (Vector(344.95154498533867,304.05921615552154, 0.0));
    positionAlloc4->Add (Vector(21.514677380254142,129.77423605817523, 0.0));
    positionAlloc4->Add (Vector(109.41947319070172,243.12496544511865, 0.0));
    positionAlloc4->Add (Vector(99.21164501532841,350.07862492883555, 0.0));
    positionAlloc4->Add (Vector(409.252934382855,126.07651912008933, 0.0));
    positionAlloc4->Add (Vector(57.83277472782073,208.01880217694517, 0.0));
    positionAlloc4->Add (Vector(315.2518288447546,455.1848743101484, 0.0));
    positionAlloc4->Add (Vector(366.3668612207281,42.6652616199083, 0.0));
    positionAlloc4->Add (Vector(150.9959086071356,453.85129636744415, 0.0));
    positionAlloc4->Add (Vector(33.18078995379686,124.84797341061987, 0.0));
    positionAlloc4->Add (Vector(129.12290526805754,116.65978203980737, 0.0));
    positionAlloc4->Add (Vector(7.127643337486889,160.8550831532396, 0.0));
    positionAlloc4->Add (Vector(57.351009575915654,292.44729285940366, 0.0));
    positionAlloc4->Add (Vector(333.0752647827426,225.79708747373266, 0.0));
    positionAlloc4->Add (Vector(414.6148268490465,421.638747642946, 0.0));
    positionAlloc4->Add (Vector(154.15687698641278,86.2028099225231, 0.0));
    positionAlloc4->Add (Vector(454.980767687071,72.9850103301991, 0.0));
    positionAlloc4->Add (Vector(0.9158272957447977,321.49960083850266, 0.0));
    positionAlloc4->Add (Vector(484.70061705536557,113.70816970456305, 0.0));
    positionAlloc4->Add (Vector(119.03409566761275,298.2110713510072, 0.0));
    positionAlloc4->Add (Vector(262.1925139499368,145.8070017337827, 0.0));
    positionAlloc4->Add (Vector(259.77325871282534,208.6362708591818, 0.0));
    positionAlloc4->Add (Vector(349.9660294713504,456.57812150967095, 0.0));
    positionAlloc4->Add (Vector(303.28250262072805,240.53455457205115, 0.0));
    positionAlloc4->Add (Vector(72.91121896137031,183.47120910110775, 0.0));
    positionAlloc4->Add (Vector(282.4390772467873,82.20801324726523, 0.0));
    positionAlloc4->Add (Vector(293.26687734983824,265.87053768796983, 0.0));
    positionAlloc4->Add (Vector(407.3784080093088,182.43139737167542, 0.0));
    positionAlloc4->Add (Vector(271.02036593995865,365.98968700702926, 0.0));
    positionAlloc4->Add (Vector(162.99041337085228,9.60341376369045, 0.0));
    positionAlloc4->Add (Vector(142.28369287894054,18.801273433428577, 0.0));
    positionAlloc4->Add (Vector(3.399021183760398,135.50267025679824, 0.0));
    positionAlloc4->Add (Vector(468.24852876591376,191.8225070486919, 0.0));
    positionAlloc4->Add (Vector(253.45794154336753,390.5685832889616, 0.0));
    positionAlloc4->Add (Vector(405.81819721590125,174.4841179123177, 0.0));
    positionAlloc4->Add (Vector(160.81999826205197,194.4300647096502, 0.0));
    positionAlloc4->Add (Vector(365.87615686739844,357.54147942029033, 0.0));
    positionAlloc4->Add (Vector(48.630752963496946,183.25995100601884, 0.0));
    positionAlloc4->Add (Vector(332.0313992266018,475.88184970533484, 0.0));
    positionAlloc4->Add (Vector(65.2406385343502,110.15441826092581, 0.0));
    positionAlloc4->Add (Vector(198.16804051405808,26.577895269028208, 0.0));
    positionAlloc4->Add (Vector(199.06048263213054,474.9096124784592, 0.0));
    positionAlloc4->Add (Vector(84.56256407519514,404.1179104497231, 0.0));
    positionAlloc4->Add (Vector(444.54442882913725,91.42856076554051, 0.0));
    positionAlloc4->Add (Vector(385.90378598652114,1.170101878079588, 0.0));
    positionAlloc4->Add (Vector(459.2694938389867,100.15376570767265, 0.0));
    positionAlloc4->Add (Vector(263.5622208958967,26.33562745629492, 0.0));
    positionAlloc4->Add (Vector(384.95581779942535,394.22709946709614, 0.0));
    positionAlloc4->Add (Vector(176.87139977581006,332.48595241723757, 0.0));
    positionAlloc4->Add (Vector(257.2099942552022,447.4595234030262, 0.0));
    positionAlloc4->Add (Vector(81.03230207979689,173.988902895021, 0.0));
    positionAlloc4->Add (Vector(383.833723051836,378.56961375966466, 0.0));
    positionAlloc4->Add (Vector(438.2184156405024,141.81741633774115, 0.0));
    positionAlloc4->Add (Vector(388.9793819967121,407.1757802630967, 0.0));
    positionAlloc4->Add (Vector(206.88286396848204,227.36194877463467, 0.0));
    positionAlloc4->Add (Vector(288.59571976118156,73.05085626135921, 0.0));
    positionAlloc4->Add (Vector(434.9379775439261,441.5867284969378, 0.0));
    positionAlloc4->Add (Vector(396.7767140322112,411.8318693798016, 0.0));
    positionAlloc4->Add (Vector(289.57292880134366,46.84146041570797, 0.0));
    positionAlloc4->Add (Vector(167.54348357168087,345.002897856941, 0.0));
    positionAlloc4->Add (Vector(225.77863541213628,32.17791600445396, 0.0));
    positionAlloc4->Add (Vector(412.72441242747374,252.75090648309705, 0.0));
    positionAlloc4->Add (Vector(497.83541195303786,68.67765160664163, 0.0));
    positionAlloc4->Add (Vector(171.03819023434153,92.43956936789161, 0.0));
    positionAlloc4->Add (Vector(106.77928712263596,359.848241408437, 0.0));
    positionAlloc4->Add (Vector(440.1096223139334,179.40378377935394, 0.0));
    positionAlloc4->Add (Vector(467.40115579898594,133.34243791822627, 0.0));
    positionAlloc4->Add (Vector(100.56265516396967,445.17257732813545, 0.0));
    positionAlloc4->Add (Vector(430.443274119,227.87872074584848, 0.0));
    positionAlloc4->Add (Vector(362.6067101291868,144.49289338526216, 0.0));
    positionAlloc4->Add (Vector(23.27164118956887,96.53044518530912, 0.0));
    positionAlloc4->Add (Vector(268.60498325674484,364.47423892997494, 0.0));
    positionAlloc4->Add (Vector(185.68908731477669,472.45738874331875, 0.0));
    positionAlloc4->Add (Vector(195.37033548913118,462.29209057691907, 0.0));
    positionAlloc4->Add (Vector(350.67973649541204,186.55010010212325, 0.0));
    positionAlloc4->Add (Vector(442.1527861679693,259.4385902480784, 0.0));
    positionAlloc4->Add (Vector(51.995215607672115,426.07143562174247, 0.0));
    positionAlloc4->Add (Vector(376.1862435482816,437.3660386355263, 0.0));
    positionAlloc4->Add (Vector(47.458040161648455,251.32892676024343, 0.0));
    positionAlloc4->Add (Vector(435.98242992236095,490.71936268772333, 0.0));
    positionAlloc4->Add (Vector(413.3773020562396,297.7781944447164, 0.0));
    positionAlloc4->Add (Vector(168.4969046629799,365.07601390834566, 0.0));
    positionAlloc4->Add (Vector(487.891205387548,392.04750671587965, 0.0));
    positionAlloc4->Add (Vector(136.92559126049713,260.2917329691286, 0.0));
    positionAlloc4->Add (Vector(130.21261612306628,373.0206355108763, 0.0));
    positionAlloc4->Add (Vector(315.0214794225543,375.38427331662524, 0.0));
    positionAlloc4->Add (Vector(193.56065120983297,439.33077429831883, 0.0));
    positionAlloc4->Add (Vector(192.1385629524039,61.22993757619388, 0.0));
    positionAlloc4->Add (Vector(206.84009410733395,84.49516139627583, 0.0));
    positionAlloc4->Add (Vector(484.5154320085466,97.73304648949465, 0.0));
    positionAlloc4->Add (Vector(34.724868464117876,468.1326427862184, 0.0));
    positionAlloc4->Add (Vector(37.73173087231962,161.19445684339124, 0.0));
    positionAlloc4->Add (Vector(305.8583780670345,73.76126354114282, 0.0));
    positionAlloc4->Add (Vector(36.53068881842619,54.02680971775348, 0.0));
    positionAlloc4->Add (Vector(155.88879890805273,111.9168472243462, 0.0));
    positionAlloc4->Add (Vector(263.20647289421845,441.66656825788044, 0.0));
    positionAlloc4->Add (Vector(319.7672981058327,281.679569892279, 0.0));
    positionAlloc4->Add (Vector(56.4043924804854,204.9658920367518, 0.0));
    positionAlloc4->Add (Vector(406.1836255560461,131.9347550139364, 0.0));
    positionAlloc4->Add (Vector(131.26786228059072,250.22290485081356, 0.0));
    positionAlloc4->Add (Vector(266.8140771572465,2.5354546594995586, 0.0));
    positionAlloc4->Add (Vector(414.74521936910435,279.59681121433044, 0.0));
    positionAlloc4->Add (Vector(227.5085694864061,94.53766342529774, 0.0));
    positionAlloc4->Add (Vector(250.7439293817008,427.3104468368563, 0.0));
    positionAlloc4->Add (Vector(102.43322239882396,300.84370786549005, 0.0));
    positionAlloc4->Add (Vector(59.24494843971573,178.02132710789598, 0.0));
    positionAlloc4->Add (Vector(382.1715104991547,460.505357892387, 0.0));
    positionAlloc4->Add (Vector(192.9337022491947,405.04457938248447, 0.0));
    positionAlloc4->Add (Vector(372.16076809268924,214.2051428047308, 0.0));
    positionAlloc4->Add (Vector(16.805247850514903,388.2187965561642, 0.0));
    positionAlloc4->Add (Vector(236.1733882263965,312.18599464412665, 0.0));
    positionAlloc4->Add (Vector(428.5705020138048,347.4588829303864, 0.0));
    positionAlloc4->Add (Vector(254.1851516825318,313.40411746012427, 0.0));
    positionAlloc4->Add (Vector(377.9389263966054,335.50696543608416, 0.0));
    positionAlloc4->Add (Vector(10.510276532439411,477.62199257095756, 0.0));
    positionAlloc4->Add (Vector(330.5104761514363,284.4057422240584, 0.0));
    positionAlloc4->Add (Vector(341.80646292687373,409.4303036491447, 0.0));
    positionAlloc4->Add (Vector(97.72722113439136,477.20326081736766, 0.0));
    positionAlloc4->Add (Vector(47.52064421832919,168.74368287348574, 0.0));
    positionAlloc4->Add (Vector(435.8159258792955,140.01144776280378, 0.0));
    positionAlloc4->Add (Vector(359.52228039527256,153.0597925712466, 0.0));
    positionAlloc4->Add (Vector(120.64237827512426,19.83931390001381, 0.0));
    positionAlloc4->Add (Vector(270.7166417103146,265.22582667561437, 0.0));
    positionAlloc4->Add (Vector(253.6332796389516,319.90495186749627, 0.0));
    positionAlloc4->Add (Vector(201.55631963880143,386.1855986037974, 0.0));
    positionAlloc4->Add (Vector(452.23556132578204,400.0443487033327, 0.0));
    positionAlloc4->Add (Vector(420.53409845167454,453.73456961990286, 0.0));
    positionAlloc4->Add (Vector(366.2539381095775,39.90421331494165, 0.0));
    positionAlloc4->Add (Vector(173.10573737763252,303.29241793713925, 0.0));
    positionAlloc4->Add (Vector(290.28441661710326,375.23498386009646, 0.0));
    positionAlloc4->Add (Vector(410.7937138699992,339.3541103159086, 0.0));
    positionAlloc4->Add (Vector(240.9455116337307,482.07402165530976, 0.0));
    positionAlloc4->Add (Vector(330.7916144870224,483.3985991543896, 0.0));
    positionAlloc4->Add (Vector(191.37555657076183,490.61776010576295, 0.0));
    positionAlloc4->Add (Vector(142.3394059715073,63.11605982454049, 0.0));
    positionAlloc4->Add (Vector(269.04041461657556,294.185380208226, 0.0));
    positionAlloc4->Add (Vector(292.3611329625691,243.7266885706119, 0.0));
    positionAlloc4->Add (Vector(423.99063697423594,275.9892898449781, 0.0));
    positionAlloc4->Add (Vector(494.70660241127985,311.4678016484076, 0.0));
    positionAlloc4->Add (Vector(295.97245933670735,55.05888962975114, 0.0));
    positionAlloc4->Add (Vector(454.60677494943167,230.03591307697258, 0.0));
    positionAlloc4->Add (Vector(98.50806797932388,474.1234433743456, 0.0));
    positionAlloc4->Add (Vector(283.94628760362224,335.5995961067408, 0.0));
    positionAlloc4->Add (Vector(244.4066988181325,401.0880291591853, 0.0));
    positionAlloc4->Add (Vector(493.8245865260454,88.22619806812615, 0.0));
    positionAlloc4->Add (Vector(277.6266997694975,125.06112679289538, 0.0));
    positionAlloc4->Add (Vector(278.1336433852022,253.9515021426594, 0.0));
    positionAlloc4->Add (Vector(102.62154503412468,198.318270413935, 0.0));
    positionAlloc4->Add (Vector(165.62183877845226,329.37049918878296, 0.0));
    positionAlloc4->Add (Vector(432.63093834349525,0.9782345232173251, 0.0));
    positionAlloc4->Add (Vector(18.20415975068429,33.96688462940583, 0.0));
    positionAlloc4->Add (Vector(369.21020599820287,363.24876043244774, 0.0));
    positionAlloc4->Add (Vector(263.9105985649693,411.7899144086787, 0.0));
    positionAlloc4->Add (Vector(60.97603362753373,487.57955441727853, 0.0));
    positionAlloc4->Add (Vector(234.21524711168868,259.332797895659, 0.0));
    positionAlloc4->Add (Vector(288.70592566154534,21.788380444122957, 0.0));
    positionAlloc4->Add (Vector(267.94922111435557,157.01475742235615, 0.0));
    positionAlloc4->Add (Vector(217.52848862873068,336.7524587272862, 0.0));
    positionAlloc4->Add (Vector(256.42491444575353,219.93426398113291, 0.0));
    positionAlloc4->Add (Vector(60.19808555690492,181.51149824789005, 0.0));
    positionAlloc4->Add (Vector(22.4725693788283,243.42374203659588, 0.0));
    positionAlloc4->Add (Vector(299.5791904877099,99.78166833570218, 0.0));
    positionAlloc4->Add (Vector(340.10692670497986,499.1325401107867, 0.0));
    positionAlloc4->Add (Vector(285.21774138996085,463.22132521835965, 0.0));
    positionAlloc4->Add (Vector(134.19432695629268,80.81173124300194, 0.0));
    positionAlloc4->Add (Vector(489.03609144562324,331.7244446227846, 0.0));
    positionAlloc4->Add (Vector(25.456438184069786,13.819692153896668, 0.0));
    positionAlloc4->Add (Vector(62.06042915079502,347.4110049900777, 0.0));
    positionAlloc4->Add (Vector(192.64182542221124,218.472667185675, 0.0));
    positionAlloc4->Add (Vector(436.6635631873165,297.6459976303695, 0.0));
    positionAlloc4->Add (Vector(495.6855091427708,499.3756071989796, 0.0));
    positionAlloc4->Add (Vector(410.5624891965747,247.94417797365614, 0.0));
    positionAlloc4->Add (Vector(44.265078441928004,456.3467350332797, 0.0));
    positionAlloc4->Add (Vector(280.8581012527309,233.96917483482721, 0.0));
    positionAlloc4->Add (Vector(457.2404663819152,403.0993246099837, 0.0));
    positionAlloc4->Add (Vector(363.80547056851225,328.89315410260923, 0.0));
    positionAlloc4->Add (Vector(69.97245627115085,324.5806926829901, 0.0));
    positionAlloc4->Add (Vector(354.93791456237255,378.06099168916523, 0.0));
    positionAlloc4->Add (Vector(217.94200347733317,402.9661958527424, 0.0));
    positionAlloc4->Add (Vector(486.2827414842779,134.04200983788616, 0.0));
    positionAlloc4->Add (Vector(481.1088073624654,292.59470996918554, 0.0));
    positionAlloc4->Add (Vector(65.0559167334055,123.32983747917264, 0.0));
    positionAlloc4->Add (Vector(95.01330855159196,319.4861937206483, 0.0));
    positionAlloc4->Add (Vector(175.51834438784385,240.4048432763662, 0.0));
    positionAlloc4->Add (Vector(436.8520393279134,229.140611057893, 0.0));
    positionAlloc4->Add (Vector(156.4533283307779,67.91050906552366, 0.0));
    positionAlloc4->Add (Vector(56.45776564365057,150.42001243310636, 0.0));
    positionAlloc4->Add (Vector(48.64705245658302,493.29206459529297, 0.0));
    positionAlloc4->Add (Vector(35.24531739900455,313.239442517124, 0.0));
    positionAlloc4->Add (Vector(337.3338152512107,321.9651935384049, 0.0));
    positionAlloc4->Add (Vector(310.8393143735037,483.70153436452495, 0.0));
    positionAlloc4->Add (Vector(419.2032072892362,83.19054537354442, 0.0));
    positionAlloc4->Add (Vector(34.13092984972843,0.8464664009844025, 0.0));
    positionAlloc4->Add (Vector(323.7859999604861,133.52163596947562, 0.0));
    positionAlloc4->Add (Vector(81.60519222953388,497.53941501860174, 0.0));
    positionAlloc4->Add (Vector(239.50035401551102,295.4733598655103, 0.0));
    positionAlloc4->Add (Vector(317.1436981523917,239.73505003013028, 0.0));
    positionAlloc4->Add (Vector(376.3389204911058,18.824286799566604, 0.0));
    positionAlloc4->Add (Vector(368.8166726065586,332.2510918068183, 0.0));
    positionAlloc4->Add (Vector(193.68778723167483,233.8288025773116, 0.0));
    positionAlloc4->Add (Vector(316.31128469446656,474.1809568627028, 0.0));
    positionAlloc4->Add (Vector(105.72305883931432,468.6084423066112, 0.0));
    positionAlloc4->Add (Vector(257.83485704934105,18.06693349818894, 0.0));
    positionAlloc4->Add (Vector(98.03180735390615,482.0549924082624, 0.0));
    positionAlloc4->Add (Vector(125.64306539468373,2.1721050476595782, 0.0));
    positionAlloc4->Add (Vector(266.6799067178174,124.98029867164823, 0.0));
    positionAlloc4->Add (Vector(71.35843388410123,216.98074305735736, 0.0));
    positionAlloc4->Add (Vector(258.4507791167916,495.2092055587404, 0.0));
    positionAlloc4->Add (Vector(475.3366460234201,420.2999696218924, 0.0));
    positionAlloc4->Add (Vector(430.05470503687366,380.6858024886616, 0.0));
    positionAlloc4->Add (Vector(202.6776129519483,189.18394054593108, 0.0));
    positionAlloc4->Add (Vector(181.18121871878512,485.25371022341284, 0.0));
    positionAlloc4->Add (Vector(350.53682946650827,222.53891422372152, 0.0));
    positionAlloc4->Add (Vector(116.6118899615724,218.5368544352238, 0.0));
    positionAlloc4->Add (Vector(86.36268462106645,10.471548338557678, 0.0));
    positionAlloc4->Add (Vector(115.39411914970948,94.24172140015324, 0.0));
    positionAlloc4->Add (Vector(57.171385641987094,294.2038970356378, 0.0));
    positionAlloc4->Add (Vector(135.10213457366626,88.90463833548678, 0.0));
    positionAlloc4->Add (Vector(282.09757374668857,13.004924334185153, 0.0));
    positionAlloc4->Add (Vector(93.17291017880325,104.33472825733087, 0.0));
    positionAlloc4->Add (Vector(105.11236012537356,354.6689985394767, 0.0));
    positionAlloc4->Add (Vector(131.38057848642904,197.21894968886312, 0.0));
    positionAlloc4->Add (Vector(132.96181778119043,115.63551876028866, 0.0));
    positionAlloc4->Add (Vector(286.48814607443165,209.5982000816538, 0.0));
    positionAlloc4->Add (Vector(43.61209686962214,172.9873173034211, 0.0));
    positionAlloc4->Add (Vector(326.0161941165713,207.7716088729326, 0.0));
    positionAlloc4->Add (Vector(425.05848442501406,30.640316943065294, 0.0));
    positionAlloc4->Add (Vector(105.79228146194703,204.8174757733146, 0.0));
    positionAlloc4->Add (Vector(141.02417615625123,109.58266804935323, 0.0));
    positionAlloc4->Add (Vector(255.7944204897188,480.502887864611, 0.0));
    positionAlloc4->Add (Vector(241.53951071516923,134.54512230072575, 0.0));
    positionAlloc4->Add (Vector(347.23282257564574,335.6037265969181, 0.0));
    positionAlloc4->Add (Vector(10.854541443545141,359.6415238362741, 0.0));
    positionAlloc4->Add (Vector(476.69149445819517,47.85360713814063, 0.0));
    positionAlloc4->Add (Vector(199.29638625143232,394.4297536447718, 0.0));
    positionAlloc4->Add (Vector(197.4535761978715,60.79339545812273, 0.0));
    positionAlloc4->Add (Vector(285.41726561314886,354.5293707856779, 0.0));
    positionAlloc4->Add (Vector(31.618800462562934,148.60252207139897, 0.0));
    positionAlloc4->Add (Vector(157.74556924974127,338.9633046109814, 0.0));
    positionAlloc4->Add (Vector(90.5472733682674,467.98989401901883, 0.0));
    positionAlloc4->Add (Vector(227.38583180907247,457.0256503682017, 0.0));
    positionAlloc4->Add (Vector(139.24594724239375,241.19242773954198, 0.0));
    positionAlloc4->Add (Vector(221.02875613339123,342.32707105785545, 0.0));
    positionAlloc4->Add (Vector(235.82111294423913,170.69402852569144, 0.0));
    positionAlloc4->Add (Vector(399.323886686338,377.43611632699185, 0.0));
    positionAlloc4->Add (Vector(355.01460500990805,349.6073594789773, 0.0));
    positionAlloc4->Add (Vector(191.036795538971,356.9997504555689, 0.0));
    positionAlloc4->Add (Vector(85.07960161722261,13.853407746697632, 0.0));
    positionAlloc4->Add (Vector(184.75818931641152,437.83763173020265, 0.0));
    positionAlloc4->Add (Vector(109.7075357696985,29.746754899757143, 0.0));
    positionAlloc4->Add (Vector(274.97088791292725,357.551192128751, 0.0));
    positionAlloc4->Add (Vector(263.67164015239865,106.06000434264307, 0.0));
    positionAlloc4->Add (Vector(376.1047901787715,4.534499116431001, 0.0));
    positionAlloc4->Add (Vector(432.8326754610974,394.57396742780446, 0.0));
    positionAlloc4->Add (Vector(359.7559410292167,331.1599943068458, 0.0));
    positionAlloc4->Add (Vector(152.07263606208616,213.69075837150498, 0.0));
    positionAlloc4->Add (Vector(236.28259209151724,224.11325471582649, 0.0));
    positionAlloc4->Add (Vector(223.67053506671496,454.838285159908, 0.0));
    positionAlloc4->Add (Vector(435.56609683396726,400.70643332443206, 0.0));
    positionAlloc4->Add (Vector(128.31134029734775,395.27864469490225, 0.0));
    positionAlloc4->Add (Vector(464.3672134013332,22.26230776820953, 0.0));
    positionAlloc4->Add (Vector(4.516473325338721,461.65483144987974, 0.0));
    positionAlloc4->Add (Vector(68.24139020817816,375.7325697432639, 0.0));
    positionAlloc4->Add (Vector(344.07797278164054,64.785001590415, 0.0));
    positionAlloc4->Add (Vector(407.24875304085873,5.862513113116197, 0.0));
    positionAlloc4->Add (Vector(133.57542256376308,111.97973007250694, 0.0));
    positionAlloc4->Add (Vector(284.0569149163452,123.06580420099684, 0.0));
    positionAlloc4->Add (Vector(305.6068972393489,298.9616108290003, 0.0));
    positionAlloc4->Add (Vector(386.355716629262,108.36207983975748, 0.0));
    positionAlloc4->Add (Vector(79.3825636990264,351.5659899137228, 0.0));
    positionAlloc4->Add (Vector(211.92302073321002,156.24152848793372, 0.0));
    positionAlloc4->Add (Vector(204.54939383491154,398.370299633885, 0.0));
    positionAlloc4->Add (Vector(466.42646621291516,411.5028420366627, 0.0));
    positionAlloc4->Add (Vector(291.629549765978,206.09186793347877, 0.0));
    positionAlloc4->Add (Vector(312.43193753487935,196.92962623109557, 0.0));
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
	  flowmon->SerializeToXmlFile ("scratch/switch_HDSO_flowmon/switch_HDSO24.flowmon", false, false);
	 //FIM FLOW MONITOR
		Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (serverApps.Get (0));
		std::cout << "Bytes received by server 1: " << sink1->GetTotalRx () << " ("
							<< (8. * sink1->GetTotalRx ()) / 1000000 / simTime << " Mbps)"
							<< std::endl;
	 return 0;
	}