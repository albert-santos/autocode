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
	  uint16_t numberOfRrhs = 12;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 262;
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
    positionAlloc->Add (Vector (333.3333333333333,500.0, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,0.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,500.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,0.0, 0.0));
    positionAlloc->Add (Vector (0.0,500.0, 0.0));
    positionAlloc->Add (Vector (0.0,333.3333333333333, 0.0));
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
    positionAlloc4->Add (Vector(444.36823239094673,356.80663325420323, 0.0));
    positionAlloc4->Add (Vector(237.84860665583278,363.98468603643767, 0.0));
    positionAlloc4->Add (Vector(12.737174896834457,35.462576823312716, 0.0));
    positionAlloc4->Add (Vector(95.39362438629983,146.71847684743966, 0.0));
    positionAlloc4->Add (Vector(163.43799567002753,469.69226906023675, 0.0));
    positionAlloc4->Add (Vector(215.30656839540114,199.05744264460822, 0.0));
    positionAlloc4->Add (Vector(220.3620761929611,74.94015477184773, 0.0));
    positionAlloc4->Add (Vector(381.5764055279027,132.70359590247094, 0.0));
    positionAlloc4->Add (Vector(224.79626760655574,221.8160059050725, 0.0));
    positionAlloc4->Add (Vector(326.56856291809055,347.8445515907529, 0.0));
    positionAlloc4->Add (Vector(486.00797788530343,256.9147050093807, 0.0));
    positionAlloc4->Add (Vector(418.5558120235124,366.6556078208697, 0.0));
    positionAlloc4->Add (Vector(490.829033891576,236.3263316646796, 0.0));
    positionAlloc4->Add (Vector(455.59859288681156,31.933522403076896, 0.0));
    positionAlloc4->Add (Vector(345.65495660529683,415.731658293571, 0.0));
    positionAlloc4->Add (Vector(282.0930041608807,114.8032965419033, 0.0));
    positionAlloc4->Add (Vector(128.30988120594083,442.3118920717826, 0.0));
    positionAlloc4->Add (Vector(305.6041804962176,105.34626534071073, 0.0));
    positionAlloc4->Add (Vector(364.34720753424966,102.50095982456298, 0.0));
    positionAlloc4->Add (Vector(229.58084682606932,241.89720499359686, 0.0));
    positionAlloc4->Add (Vector(271.67885320912234,406.7674725629279, 0.0));
    positionAlloc4->Add (Vector(253.9481949677148,142.91412096696232, 0.0));
    positionAlloc4->Add (Vector(351.19175150225806,288.4173628297059, 0.0));
    positionAlloc4->Add (Vector(82.16088455520453,387.7444190537952, 0.0));
    positionAlloc4->Add (Vector(447.1771662591314,198.22226320519897, 0.0));
    positionAlloc4->Add (Vector(498.1967528255522,214.071115848179, 0.0));
    positionAlloc4->Add (Vector(454.6323587025104,279.1437800853189, 0.0));
    positionAlloc4->Add (Vector(446.75856999978726,245.83622871275574, 0.0));
    positionAlloc4->Add (Vector(274.3292985601388,258.1014131497931, 0.0));
    positionAlloc4->Add (Vector(331.87858465550903,415.11104475555845, 0.0));
    positionAlloc4->Add (Vector(105.07966240068367,382.48625479222244, 0.0));
    positionAlloc4->Add (Vector(17.137015681160882,201.52709400022277, 0.0));
    positionAlloc4->Add (Vector(95.97789461009998,242.39268794497104, 0.0));
    positionAlloc4->Add (Vector(309.30424568763243,196.34882624044036, 0.0));
    positionAlloc4->Add (Vector(75.80629946931089,282.8149109878119, 0.0));
    positionAlloc4->Add (Vector(45.66090574213666,124.03277281837948, 0.0));
    positionAlloc4->Add (Vector(191.89741055898008,131.99351932732966, 0.0));
    positionAlloc4->Add (Vector(25.904512896590237,358.6669720950587, 0.0));
    positionAlloc4->Add (Vector(463.98850530977944,317.70568637423526, 0.0));
    positionAlloc4->Add (Vector(185.37733561744096,269.71361788073807, 0.0));
    positionAlloc4->Add (Vector(45.51395216026305,333.0181762307857, 0.0));
    positionAlloc4->Add (Vector(325.7561679682465,405.5319203897771, 0.0));
    positionAlloc4->Add (Vector(58.717611868729705,122.14876452989238, 0.0));
    positionAlloc4->Add (Vector(53.127265493057294,48.24427381007512, 0.0));
    positionAlloc4->Add (Vector(362.5333356043448,343.47777328549347, 0.0));
    positionAlloc4->Add (Vector(42.54106445541228,303.8912322006146, 0.0));
    positionAlloc4->Add (Vector(393.12715354985824,268.5248566186152, 0.0));
    positionAlloc4->Add (Vector(52.3152146769163,50.123021297689476, 0.0));
    positionAlloc4->Add (Vector(406.31687132893023,320.2399806413669, 0.0));
    positionAlloc4->Add (Vector(7.867395898387142,467.1157793618058, 0.0));
    positionAlloc4->Add (Vector(341.19702650592114,253.03478055422323, 0.0));
    positionAlloc4->Add (Vector(53.25676008796365,188.28389406272538, 0.0));
    positionAlloc4->Add (Vector(384.66636320726195,422.0823518120144, 0.0));
    positionAlloc4->Add (Vector(6.131287922160444,316.4321316652864, 0.0));
    positionAlloc4->Add (Vector(40.32045565408565,495.8556473468713, 0.0));
    positionAlloc4->Add (Vector(292.8439317176789,222.91777881362373, 0.0));
    positionAlloc4->Add (Vector(426.65803917867174,426.7958167703082, 0.0));
    positionAlloc4->Add (Vector(114.97392939365409,462.57148784845594, 0.0));
    positionAlloc4->Add (Vector(373.99719629133085,318.5122720937901, 0.0));
    positionAlloc4->Add (Vector(17.47779741334188,295.6841020765212, 0.0));
    positionAlloc4->Add (Vector(370.98324481292735,105.24126074538748, 0.0));
    positionAlloc4->Add (Vector(132.27915823147234,125.83597484595738, 0.0));
    positionAlloc4->Add (Vector(87.92286804752524,252.2762385092428, 0.0));
    positionAlloc4->Add (Vector(69.70143602155099,379.36166424589595, 0.0));
    positionAlloc4->Add (Vector(160.7816755451033,399.71629022421905, 0.0));
    positionAlloc4->Add (Vector(364.97578459389166,92.76280855663089, 0.0));
    positionAlloc4->Add (Vector(5.416603036506206,476.54865257213834, 0.0));
    positionAlloc4->Add (Vector(222.0753271072205,35.01584228062976, 0.0));
    positionAlloc4->Add (Vector(104.10895852252217,277.016583838729, 0.0));
    positionAlloc4->Add (Vector(62.91718926208895,302.89719600437417, 0.0));
    positionAlloc4->Add (Vector(86.93587634479077,499.59267455498315, 0.0));
    positionAlloc4->Add (Vector(78.82705432770804,61.210300033315065, 0.0));
    positionAlloc4->Add (Vector(198.7743555414324,200.02580189600005, 0.0));
    positionAlloc4->Add (Vector(156.07442107900965,16.503418262639958, 0.0));
    positionAlloc4->Add (Vector(440.48459128028253,169.57785096745448, 0.0));
    positionAlloc4->Add (Vector(176.73023363344814,332.9409553518271, 0.0));
    positionAlloc4->Add (Vector(249.3180122481698,335.09225157538566, 0.0));
    positionAlloc4->Add (Vector(397.16497572829866,153.9429588728688, 0.0));
    positionAlloc4->Add (Vector(414.07228349032135,401.4767621152961, 0.0));
    positionAlloc4->Add (Vector(412.9354270199621,259.6204854264622, 0.0));
    positionAlloc4->Add (Vector(377.5578727833449,145.96894648663573, 0.0));
    positionAlloc4->Add (Vector(184.57045055932247,396.30553105361537, 0.0));
    positionAlloc4->Add (Vector(377.3776679071756,74.50229811210951, 0.0));
    positionAlloc4->Add (Vector(487.7670342918302,86.64015281573934, 0.0));
    positionAlloc4->Add (Vector(277.6302195264341,94.1049170522269, 0.0));
    positionAlloc4->Add (Vector(128.24929575295286,275.8742548322912, 0.0));
    positionAlloc4->Add (Vector(119.55655034720868,171.8391496810105, 0.0));
    positionAlloc4->Add (Vector(474.72303697958165,470.2250901378958, 0.0));
    positionAlloc4->Add (Vector(28.600227381625988,409.795848755716, 0.0));
    positionAlloc4->Add (Vector(31.408666437583776,262.0391276231833, 0.0));
    positionAlloc4->Add (Vector(155.5924832934979,84.28385467932625, 0.0));
    positionAlloc4->Add (Vector(387.5072847050765,90.86992778332609, 0.0));
    positionAlloc4->Add (Vector(102.79136138988292,495.0608481830833, 0.0));
    positionAlloc4->Add (Vector(355.3544936267713,454.04780575971625, 0.0));
    positionAlloc4->Add (Vector(421.2796120448379,378.0559278671135, 0.0));
    positionAlloc4->Add (Vector(424.5505110868125,349.9595576124419, 0.0));
    positionAlloc4->Add (Vector(462.176100592139,1.0422433100643058, 0.0));
    positionAlloc4->Add (Vector(458.4720196128732,208.44199430789095, 0.0));
    positionAlloc4->Add (Vector(222.36714583344408,456.1871133815691, 0.0));
    positionAlloc4->Add (Vector(0.5657353258125553,216.3440410824073, 0.0));
    positionAlloc4->Add (Vector(143.3832948369521,125.19345769008483, 0.0));
    positionAlloc4->Add (Vector(301.81858738027375,335.9215932285333, 0.0));
    positionAlloc4->Add (Vector(454.0451038762555,221.25842173345072, 0.0));
    positionAlloc4->Add (Vector(465.9570332153461,165.71515766666278, 0.0));
    positionAlloc4->Add (Vector(426.8247879565982,461.11407626853133, 0.0));
    positionAlloc4->Add (Vector(212.47750227397955,61.26227801952916, 0.0));
    positionAlloc4->Add (Vector(14.785608475573698,39.560414793702705, 0.0));
    positionAlloc4->Add (Vector(497.1501279195266,333.625326348444, 0.0));
    positionAlloc4->Add (Vector(377.70337482587877,0.7554109719901669, 0.0));
    positionAlloc4->Add (Vector(354.80223501458346,302.57526482281736, 0.0));
    positionAlloc4->Add (Vector(371.89225211011285,328.71041465839517, 0.0));
    positionAlloc4->Add (Vector(42.525520382404814,383.5303087967853, 0.0));
    positionAlloc4->Add (Vector(20.593346287357093,425.03641116938707, 0.0));
    positionAlloc4->Add (Vector(143.787255200918,87.26601596677358, 0.0));
    positionAlloc4->Add (Vector(309.6731642115851,162.85386529312657, 0.0));
    positionAlloc4->Add (Vector(415.2693215906341,203.9428183413755, 0.0));
    positionAlloc4->Add (Vector(171.78833378561092,107.39735088024239, 0.0));
    positionAlloc4->Add (Vector(64.98360049396202,357.4770656687761, 0.0));
    positionAlloc4->Add (Vector(475.4066443199142,478.5694741434574, 0.0));
    positionAlloc4->Add (Vector(28.669463087419523,71.12784075087009, 0.0));
    positionAlloc4->Add (Vector(489.93946784691394,18.15267234303536, 0.0));
    positionAlloc4->Add (Vector(78.42172838705197,79.61405533055415, 0.0));
    positionAlloc4->Add (Vector(390.6335334520015,356.2161848060286, 0.0));
    positionAlloc4->Add (Vector(182.3157436815886,128.34526074398102, 0.0));
    positionAlloc4->Add (Vector(27.50211145541187,383.25256295285993, 0.0));
    positionAlloc4->Add (Vector(145.77348330394528,85.21515263610335, 0.0));
    positionAlloc4->Add (Vector(80.83488776374031,344.7424780142451, 0.0));
    positionAlloc4->Add (Vector(336.795660376582,447.992896292983, 0.0));
    positionAlloc4->Add (Vector(236.40509062099662,465.88571089375614, 0.0));
    positionAlloc4->Add (Vector(425.76879886578416,243.89440480864667, 0.0));
    positionAlloc4->Add (Vector(21.413432618902885,257.5187612009633, 0.0));
    positionAlloc4->Add (Vector(79.14256643593359,186.6784586926447, 0.0));
    positionAlloc4->Add (Vector(240.10286675830804,174.4242937268825, 0.0));
    positionAlloc4->Add (Vector(276.2751043983307,175.70884079199024, 0.0));
    positionAlloc4->Add (Vector(293.9783081131959,14.643562054360238, 0.0));
    positionAlloc4->Add (Vector(476.81737266713515,256.05388902383044, 0.0));
    positionAlloc4->Add (Vector(324.87033187955495,42.219815968923605, 0.0));
    positionAlloc4->Add (Vector(383.50697921989035,358.7662613395341, 0.0));
    positionAlloc4->Add (Vector(136.45921376093435,400.25070919846974, 0.0));
    positionAlloc4->Add (Vector(82.37014743068166,184.56880246601915, 0.0));
    positionAlloc4->Add (Vector(344.1255338607038,125.63977135671661, 0.0));
    positionAlloc4->Add (Vector(171.56357454408877,349.6602544615077, 0.0));
    positionAlloc4->Add (Vector(204.32938786570242,197.1725554025373, 0.0));
    positionAlloc4->Add (Vector(176.0568442959299,477.50817791048274, 0.0));
    positionAlloc4->Add (Vector(88.2840716262927,379.61090913932463, 0.0));
    positionAlloc4->Add (Vector(129.87957995337896,254.0493222903627, 0.0));
    positionAlloc4->Add (Vector(237.6536696014095,48.15741319195149, 0.0));
    positionAlloc4->Add (Vector(367.0424592305315,181.79505198189077, 0.0));
    positionAlloc4->Add (Vector(457.15947782036426,424.5185808379611, 0.0));
    positionAlloc4->Add (Vector(360.86129129195405,360.11253473678164, 0.0));
    positionAlloc4->Add (Vector(339.3047909047574,170.8145010181171, 0.0));
    positionAlloc4->Add (Vector(49.21120801274109,34.62961271763104, 0.0));
    positionAlloc4->Add (Vector(387.9243614596207,404.5888690094199, 0.0));
    positionAlloc4->Add (Vector(350.21934449932394,246.96434827106157, 0.0));
    positionAlloc4->Add (Vector(453.85879090228417,42.354617125735686, 0.0));
    positionAlloc4->Add (Vector(457.85040332526495,116.22029514681714, 0.0));
    positionAlloc4->Add (Vector(152.92014561749411,218.67227118607002, 0.0));
    positionAlloc4->Add (Vector(342.5523492708596,364.2279262258626, 0.0));
    positionAlloc4->Add (Vector(435.1406079898953,148.94794890754443, 0.0));
    positionAlloc4->Add (Vector(457.85505440085393,42.01616523556906, 0.0));
    positionAlloc4->Add (Vector(12.831935424434782,456.7739570339565, 0.0));
    positionAlloc4->Add (Vector(96.31958379587053,124.1296303374444, 0.0));
    positionAlloc4->Add (Vector(220.63657013369158,43.130426404880126, 0.0));
    positionAlloc4->Add (Vector(425.9146556831521,419.20977863429016, 0.0));
    positionAlloc4->Add (Vector(453.77851284395206,332.3917602556691, 0.0));
    positionAlloc4->Add (Vector(223.48324086686532,425.2711063947802, 0.0));
    positionAlloc4->Add (Vector(308.0787026242718,134.45485089626246, 0.0));
    positionAlloc4->Add (Vector(411.59240492688156,379.0815850517526, 0.0));
    positionAlloc4->Add (Vector(344.03191839764014,224.74078848534907, 0.0));
    positionAlloc4->Add (Vector(25.00226601285277,462.125505091168, 0.0));
    positionAlloc4->Add (Vector(252.37389905840536,374.41105178200274, 0.0));
    positionAlloc4->Add (Vector(415.5646283812275,352.38702593871324, 0.0));
    positionAlloc4->Add (Vector(426.2034126394404,142.9949797599518, 0.0));
    positionAlloc4->Add (Vector(354.38302488565444,232.30979669097184, 0.0));
    positionAlloc4->Add (Vector(364.27914248029583,183.74619221697742, 0.0));
    positionAlloc4->Add (Vector(221.80999464037015,340.09120831876015, 0.0));
    positionAlloc4->Add (Vector(470.6627483724478,216.7750741941719, 0.0));
    positionAlloc4->Add (Vector(61.99362231617733,308.8354921127072, 0.0));
    positionAlloc4->Add (Vector(244.20521747180763,150.43341807010597, 0.0));
    positionAlloc4->Add (Vector(332.69131583048875,440.37404188712003, 0.0));
    positionAlloc4->Add (Vector(296.03307620097615,107.25796231793738, 0.0));
    positionAlloc4->Add (Vector(358.009480726621,79.63532857530863, 0.0));
    positionAlloc4->Add (Vector(288.083522336046,223.8789983284331, 0.0));
    positionAlloc4->Add (Vector(231.46921092210442,350.1834715093565, 0.0));
    positionAlloc4->Add (Vector(230.39414840663207,445.3380457230837, 0.0));
    positionAlloc4->Add (Vector(329.06655965432594,410.4119321853384, 0.0));
    positionAlloc4->Add (Vector(59.783587908131985,447.3046201730917, 0.0));
    positionAlloc4->Add (Vector(412.9430763140313,11.70152078625164, 0.0));
    positionAlloc4->Add (Vector(328.4160987097212,302.2777602543772, 0.0));
    positionAlloc4->Add (Vector(16.982422139244115,295.70767256885, 0.0));
    positionAlloc4->Add (Vector(434.95661874394705,8.85362168475934, 0.0));
    positionAlloc4->Add (Vector(246.84424033422846,494.13406387340865, 0.0));
    positionAlloc4->Add (Vector(442.79965107251405,363.5483987394375, 0.0));
    positionAlloc4->Add (Vector(154.38467339541677,255.28809371138505, 0.0));
    positionAlloc4->Add (Vector(179.68223313030092,155.92064294033386, 0.0));
    positionAlloc4->Add (Vector(447.4809256967734,273.73830264677844, 0.0));
    positionAlloc4->Add (Vector(469.8993170411874,159.5873071088396, 0.0));
    positionAlloc4->Add (Vector(193.6698224532963,427.3208626656758, 0.0));
    positionAlloc4->Add (Vector(44.826935454693796,333.5912836442041, 0.0));
    positionAlloc4->Add (Vector(387.3555457436444,145.89070642347096, 0.0));
    positionAlloc4->Add (Vector(218.0325313926853,176.05619041024306, 0.0));
    positionAlloc4->Add (Vector(12.668431117189627,45.688563894542675, 0.0));
    positionAlloc4->Add (Vector(333.89551567323167,221.1121093853974, 0.0));
    positionAlloc4->Add (Vector(135.46590161694462,360.68983560975, 0.0));
    positionAlloc4->Add (Vector(131.28350332273448,143.50071092835003, 0.0));
    positionAlloc4->Add (Vector(387.14033568523615,256.81033852931074, 0.0));
    positionAlloc4->Add (Vector(346.64721538347413,125.71248339489615, 0.0));
    positionAlloc4->Add (Vector(474.22258931403394,415.26474598586503, 0.0));
    positionAlloc4->Add (Vector(69.82012402691385,455.46703003843504, 0.0));
    positionAlloc4->Add (Vector(191.0987673972739,71.06562610788924, 0.0));
    positionAlloc4->Add (Vector(399.5000320674769,414.1510955083576, 0.0));
    positionAlloc4->Add (Vector(41.41931981738156,434.49555602558087, 0.0));
    positionAlloc4->Add (Vector(309.32985798387193,55.19173077997691, 0.0));
    positionAlloc4->Add (Vector(414.33796531130105,207.09325722514228, 0.0));
    positionAlloc4->Add (Vector(255.61355255564172,386.9467352408725, 0.0));
    positionAlloc4->Add (Vector(139.0950046206036,238.25026500314272, 0.0));
    positionAlloc4->Add (Vector(29.447610815478754,299.20427324010785, 0.0));
    positionAlloc4->Add (Vector(82.67271337944932,143.59456261659503, 0.0));
    positionAlloc4->Add (Vector(104.9234479859214,117.77328453403108, 0.0));
    positionAlloc4->Add (Vector(458.7572642087707,119.58925086405914, 0.0));
    positionAlloc4->Add (Vector(75.5212476293654,28.645156461808487, 0.0));
    positionAlloc4->Add (Vector(281.1964373579238,48.58611426139553, 0.0));
    positionAlloc4->Add (Vector(415.80807238079393,227.45173708757716, 0.0));
    positionAlloc4->Add (Vector(6.090520172389313,60.19707724735962, 0.0));
    positionAlloc4->Add (Vector(480.56950099436125,11.095861193610501, 0.0));
    positionAlloc4->Add (Vector(212.74374952500625,28.385257806513298, 0.0));
    positionAlloc4->Add (Vector(484.6413022531052,413.9486110486679, 0.0));
    positionAlloc4->Add (Vector(206.50188402346558,180.39394701407468, 0.0));
    positionAlloc4->Add (Vector(274.3427618943064,4.016207955745143, 0.0));
    positionAlloc4->Add (Vector(135.98399258610112,54.564008387638694, 0.0));
    positionAlloc4->Add (Vector(73.36515886345474,146.323316579118, 0.0));
    positionAlloc4->Add (Vector(64.01546719248414,295.3034699409027, 0.0));
    positionAlloc4->Add (Vector(366.1980398575449,272.6258010864278, 0.0));
    positionAlloc4->Add (Vector(482.99813659072174,30.431070595349997, 0.0));
    positionAlloc4->Add (Vector(191.07029526519077,208.66756046396569, 0.0));
    positionAlloc4->Add (Vector(321.2852111890119,472.30205557862007, 0.0));
    positionAlloc4->Add (Vector(461.8963746100334,10.658847748346357, 0.0));
    positionAlloc4->Add (Vector(211.73041470114785,231.9981418555845, 0.0));
    positionAlloc4->Add (Vector(331.57050171812824,485.20384370934323, 0.0));
    positionAlloc4->Add (Vector(171.13111380269115,223.9882165755443, 0.0));
    positionAlloc4->Add (Vector(28.664696328284666,94.05977022170892, 0.0));
    positionAlloc4->Add (Vector(270.74756269263247,338.59380796843726, 0.0));
    positionAlloc4->Add (Vector(124.28718863617772,98.97219479447344, 0.0));
    positionAlloc4->Add (Vector(342.3934768306657,256.2462102039658, 0.0));
    positionAlloc4->Add (Vector(202.782232044751,266.31035734702897, 0.0));
    positionAlloc4->Add (Vector(364.73336984118,179.92306220322274, 0.0));
    positionAlloc4->Add (Vector(142.28845783452172,35.88413762544468, 0.0));
    positionAlloc4->Add (Vector(204.65699341168803,149.60103240326293, 0.0));
    positionAlloc4->Add (Vector(96.05342543251066,120.99464783946712, 0.0));
    positionAlloc4->Add (Vector(270.36901317689495,185.8043903199188, 0.0));
    positionAlloc4->Add (Vector(159.13420681193284,78.52760849427548, 0.0));
    positionAlloc4->Add (Vector(350.68837417559104,325.67373342062353, 0.0));
    positionAlloc4->Add (Vector(167.1415577392449,280.683355307747, 0.0));
    positionAlloc4->Add (Vector(327.33013271622383,459.40340982336704, 0.0));
    positionAlloc4->Add (Vector(101.95700949046999,119.5620789172559, 0.0));
    positionAlloc4->Add (Vector(395.5324036373569,242.5860385441158, 0.0));
    positionAlloc4->Add (Vector(372.1009474037668,391.77452873745983, 0.0));
    positionAlloc4->Add (Vector(242.16902781667514,347.6725551580495, 0.0));
    positionAlloc4->Add (Vector(350.6594896046573,3.623148716974889, 0.0));
    positionAlloc4->Add (Vector(346.28218629597205,373.86585362103983, 0.0));
    positionAlloc4->Add (Vector(310.82647186487674,364.88753246150276, 0.0));
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
	  flowmon->SerializeToXmlFile ("scratch/switch_SA_flowmon/switch_SA15.flowmon", false, false);
	 //FIM FLOW MONITOR
		Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (serverApps.Get (0));
		std::cout << "Bytes received by server 1: " << sink1->GetTotalRx () << " ("
							<< (8. * sink1->GetTotalRx ()) / 1000000 / simTime << " Mbps)"
							<< std::endl;
	 return 0;
	}