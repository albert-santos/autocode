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
	  uint16_t numberOfRrhs = 7;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 331;
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
    positionAlloc->Add (Vector (500.0,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,0.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,500.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,333.3333333333333, 0.0));
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
    positionAlloc4->Add (Vector(474.30228542952966,152.7626329406341, 0.0));
    positionAlloc4->Add (Vector(428.36307141878484,158.79441889402622, 0.0));
    positionAlloc4->Add (Vector(480.3441500552785,378.03162208885533, 0.0));
    positionAlloc4->Add (Vector(331.6634364327606,338.51951480197306, 0.0));
    positionAlloc4->Add (Vector(455.89046933695914,71.99129620812289, 0.0));
    positionAlloc4->Add (Vector(227.84542977278016,461.4219083297613, 0.0));
    positionAlloc4->Add (Vector(165.7524063372277,223.54702910424302, 0.0));
    positionAlloc4->Add (Vector(33.56601776890605,383.0086766162912, 0.0));
    positionAlloc4->Add (Vector(90.85349514666014,237.33867845946304, 0.0));
    positionAlloc4->Add (Vector(432.831876373356,55.72778309671256, 0.0));
    positionAlloc4->Add (Vector(406.5219130378152,321.0127918497305, 0.0));
    positionAlloc4->Add (Vector(420.84306947207756,348.3853585626217, 0.0));
    positionAlloc4->Add (Vector(467.46473788646136,492.1891531264912, 0.0));
    positionAlloc4->Add (Vector(56.22117210824668,471.08767873252225, 0.0));
    positionAlloc4->Add (Vector(456.87162531923764,234.23240548505396, 0.0));
    positionAlloc4->Add (Vector(393.423534208485,489.52779901677417, 0.0));
    positionAlloc4->Add (Vector(29.502661707599522,341.0832888593928, 0.0));
    positionAlloc4->Add (Vector(185.40047642361162,164.17917169719186, 0.0));
    positionAlloc4->Add (Vector(318.79045457775027,150.1638848365604, 0.0));
    positionAlloc4->Add (Vector(346.42961918509053,235.7335734706356, 0.0));
    positionAlloc4->Add (Vector(461.6551119942032,348.6330517934858, 0.0));
    positionAlloc4->Add (Vector(333.4940217073988,46.20667763321157, 0.0));
    positionAlloc4->Add (Vector(408.066245800139,396.21507730530953, 0.0));
    positionAlloc4->Add (Vector(92.49408735423509,120.48151321371387, 0.0));
    positionAlloc4->Add (Vector(434.4648216408706,202.32679976906886, 0.0));
    positionAlloc4->Add (Vector(25.069696355904615,326.0603381889204, 0.0));
    positionAlloc4->Add (Vector(10.666015955934249,42.22397161563174, 0.0));
    positionAlloc4->Add (Vector(281.39417645921577,487.618587485224, 0.0));
    positionAlloc4->Add (Vector(411.29792616686785,117.49727310774777, 0.0));
    positionAlloc4->Add (Vector(137.8760213439724,172.690624078009, 0.0));
    positionAlloc4->Add (Vector(349.79920027434315,326.7064338508274, 0.0));
    positionAlloc4->Add (Vector(212.18921386132956,291.3456375113044, 0.0));
    positionAlloc4->Add (Vector(149.745439618777,45.82946036866203, 0.0));
    positionAlloc4->Add (Vector(350.2257055799365,292.16743818524424, 0.0));
    positionAlloc4->Add (Vector(75.14720991315727,201.89450005409466, 0.0));
    positionAlloc4->Add (Vector(203.8126809914577,250.99410521785254, 0.0));
    positionAlloc4->Add (Vector(391.71808393462504,251.16699518669733, 0.0));
    positionAlloc4->Add (Vector(187.0696369139363,480.2402669315281, 0.0));
    positionAlloc4->Add (Vector(345.1814048717561,185.87358423783513, 0.0));
    positionAlloc4->Add (Vector(450.84461568692404,68.42214987932805, 0.0));
    positionAlloc4->Add (Vector(177.33756130241306,489.4275004635366, 0.0));
    positionAlloc4->Add (Vector(200.21214570542816,105.17118017547772, 0.0));
    positionAlloc4->Add (Vector(229.00347414423078,353.814703752871, 0.0));
    positionAlloc4->Add (Vector(175.380616400096,449.10229698157406, 0.0));
    positionAlloc4->Add (Vector(79.82370264671646,494.8393386937111, 0.0));
    positionAlloc4->Add (Vector(346.00102055668697,267.1439313904038, 0.0));
    positionAlloc4->Add (Vector(217.49906953683302,169.1431430987869, 0.0));
    positionAlloc4->Add (Vector(313.7178815035422,486.59548672474597, 0.0));
    positionAlloc4->Add (Vector(245.02991982195294,51.368370072565284, 0.0));
    positionAlloc4->Add (Vector(8.736373664539443,296.19356913569567, 0.0));
    positionAlloc4->Add (Vector(453.6927553510157,382.22063030146046, 0.0));
    positionAlloc4->Add (Vector(349.06602324413916,257.75547539052775, 0.0));
    positionAlloc4->Add (Vector(80.52162443909106,242.3369575691085, 0.0));
    positionAlloc4->Add (Vector(372.6949857765133,360.5024633250498, 0.0));
    positionAlloc4->Add (Vector(134.04988487017167,127.80226435921504, 0.0));
    positionAlloc4->Add (Vector(32.38147945908965,16.073420459727707, 0.0));
    positionAlloc4->Add (Vector(372.6446324126008,42.64934563329936, 0.0));
    positionAlloc4->Add (Vector(144.978793217674,50.56566232018095, 0.0));
    positionAlloc4->Add (Vector(284.0623373340385,333.8985628364677, 0.0));
    positionAlloc4->Add (Vector(414.37095899340767,267.31816426616587, 0.0));
    positionAlloc4->Add (Vector(400.01344738051097,479.22245289262537, 0.0));
    positionAlloc4->Add (Vector(478.88379068759053,459.83492328605644, 0.0));
    positionAlloc4->Add (Vector(421.02006572385284,342.2683228802496, 0.0));
    positionAlloc4->Add (Vector(231.90159611303585,14.080876250605057, 0.0));
    positionAlloc4->Add (Vector(41.15683791029484,272.25424365392746, 0.0));
    positionAlloc4->Add (Vector(226.66734009208238,14.701582029261584, 0.0));
    positionAlloc4->Add (Vector(389.5128370439067,4.288043384405027, 0.0));
    positionAlloc4->Add (Vector(311.5172081910528,386.77595433961966, 0.0));
    positionAlloc4->Add (Vector(495.15292306621996,24.526984347981728, 0.0));
    positionAlloc4->Add (Vector(162.571147388472,180.46926991011807, 0.0));
    positionAlloc4->Add (Vector(484.8978982726643,370.7364999739544, 0.0));
    positionAlloc4->Add (Vector(142.78544882211318,483.3864985608815, 0.0));
    positionAlloc4->Add (Vector(191.240079050104,100.40524738122353, 0.0));
    positionAlloc4->Add (Vector(183.16716321811737,488.5764720111358, 0.0));
    positionAlloc4->Add (Vector(228.6318059539767,407.9366220585879, 0.0));
    positionAlloc4->Add (Vector(128.37185837163906,177.56115537682078, 0.0));
    positionAlloc4->Add (Vector(343.49802488668365,289.0987550451279, 0.0));
    positionAlloc4->Add (Vector(213.6019086150507,124.25654429759403, 0.0));
    positionAlloc4->Add (Vector(311.4879421267479,106.70729751500613, 0.0));
    positionAlloc4->Add (Vector(137.87244353271345,4.196441477567204, 0.0));
    positionAlloc4->Add (Vector(289.6994148700007,63.93825858082624, 0.0));
    positionAlloc4->Add (Vector(332.73302645827107,394.0216043591423, 0.0));
    positionAlloc4->Add (Vector(220.48797033991374,122.53150066113211, 0.0));
    positionAlloc4->Add (Vector(80.35788621257811,310.1865876309975, 0.0));
    positionAlloc4->Add (Vector(438.6705894612765,323.77095422829035, 0.0));
    positionAlloc4->Add (Vector(432.8950845221513,226.13690829502264, 0.0));
    positionAlloc4->Add (Vector(139.99136484324237,229.35639006559373, 0.0));
    positionAlloc4->Add (Vector(273.4250784470924,2.6522141417180123, 0.0));
    positionAlloc4->Add (Vector(119.61186677665886,274.5415744559214, 0.0));
    positionAlloc4->Add (Vector(236.05963525642275,113.34578915318077, 0.0));
    positionAlloc4->Add (Vector(310.41969057816993,397.5160371388741, 0.0));
    positionAlloc4->Add (Vector(391.88044405079717,115.74753731166254, 0.0));
    positionAlloc4->Add (Vector(68.24444610079166,311.597330525738, 0.0));
    positionAlloc4->Add (Vector(239.23616823128015,381.1127471766196, 0.0));
    positionAlloc4->Add (Vector(196.9028966650846,478.20194281072315, 0.0));
    positionAlloc4->Add (Vector(212.1504346653925,307.482189976336, 0.0));
    positionAlloc4->Add (Vector(381.2119279480649,235.44050256397503, 0.0));
    positionAlloc4->Add (Vector(171.83895234705037,380.1569831515342, 0.0));
    positionAlloc4->Add (Vector(155.4028014555605,458.4228373710031, 0.0));
    positionAlloc4->Add (Vector(50.11539476219512,387.72470944621614, 0.0));
    positionAlloc4->Add (Vector(262.2449679273807,113.0808453353766, 0.0));
    positionAlloc4->Add (Vector(268.5750385371479,88.93339817342205, 0.0));
    positionAlloc4->Add (Vector(382.6613486868183,269.51050149721374, 0.0));
    positionAlloc4->Add (Vector(302.3365299421573,230.38434957768374, 0.0));
    positionAlloc4->Add (Vector(143.07414866909085,95.25443711845327, 0.0));
    positionAlloc4->Add (Vector(279.05325495923097,312.40397568855724, 0.0));
    positionAlloc4->Add (Vector(71.77268576221546,10.99060431047716, 0.0));
    positionAlloc4->Add (Vector(120.29216489856476,300.5415891066261, 0.0));
    positionAlloc4->Add (Vector(162.76505603036262,3.549778074749377, 0.0));
    positionAlloc4->Add (Vector(96.53143570733397,444.5350764636276, 0.0));
    positionAlloc4->Add (Vector(122.73930954696432,307.88601114041467, 0.0));
    positionAlloc4->Add (Vector(84.56888817750391,25.981154336749356, 0.0));
    positionAlloc4->Add (Vector(391.2450407564416,188.50777041858018, 0.0));
    positionAlloc4->Add (Vector(98.32966002112703,38.30715254240713, 0.0));
    positionAlloc4->Add (Vector(446.0274117047538,399.0551728281393, 0.0));
    positionAlloc4->Add (Vector(476.9819950866356,323.4152284842317, 0.0));
    positionAlloc4->Add (Vector(109.81885612170922,175.55044258539738, 0.0));
    positionAlloc4->Add (Vector(171.74617349338024,153.08229595428736, 0.0));
    positionAlloc4->Add (Vector(437.6055599670019,132.10341502399132, 0.0));
    positionAlloc4->Add (Vector(493.83663210582887,198.83432163653137, 0.0));
    positionAlloc4->Add (Vector(105.21115666823638,243.22133789365634, 0.0));
    positionAlloc4->Add (Vector(267.2670113235724,282.6521379507109, 0.0));
    positionAlloc4->Add (Vector(309.10505287935723,18.842810138498145, 0.0));
    positionAlloc4->Add (Vector(362.4353281452574,269.8292247517864, 0.0));
    positionAlloc4->Add (Vector(350.32865381216203,312.86045042950497, 0.0));
    positionAlloc4->Add (Vector(156.61620135370035,270.3067193368899, 0.0));
    positionAlloc4->Add (Vector(397.2032747593107,244.88576733507227, 0.0));
    positionAlloc4->Add (Vector(12.488622649575454,125.23986072070537, 0.0));
    positionAlloc4->Add (Vector(224.79597617973636,188.3871498734674, 0.0));
    positionAlloc4->Add (Vector(333.83837455150723,346.2713707618172, 0.0));
    positionAlloc4->Add (Vector(224.04246396930867,279.8154631556411, 0.0));
    positionAlloc4->Add (Vector(116.41501456136116,381.20613835248196, 0.0));
    positionAlloc4->Add (Vector(493.6484800034981,252.29323464139415, 0.0));
    positionAlloc4->Add (Vector(399.17614898063334,474.21601653448886, 0.0));
    positionAlloc4->Add (Vector(369.81461918211477,125.03476204190068, 0.0));
    positionAlloc4->Add (Vector(310.88570609585054,433.71795749464127, 0.0));
    positionAlloc4->Add (Vector(76.08112763074799,357.5781392171474, 0.0));
    positionAlloc4->Add (Vector(392.52651781803814,298.2988996015447, 0.0));
    positionAlloc4->Add (Vector(157.82682484246973,396.92588582577895, 0.0));
    positionAlloc4->Add (Vector(39.92669078803973,294.1495960514176, 0.0));
    positionAlloc4->Add (Vector(262.0121607240551,110.53977553500354, 0.0));
    positionAlloc4->Add (Vector(239.77921594979,316.8461500198587, 0.0));
    positionAlloc4->Add (Vector(188.31160246179124,374.64614747302403, 0.0));
    positionAlloc4->Add (Vector(232.95122459352564,409.5468970125463, 0.0));
    positionAlloc4->Add (Vector(201.08467686487225,124.64293062303061, 0.0));
    positionAlloc4->Add (Vector(64.45800682296715,60.629146853697804, 0.0));
    positionAlloc4->Add (Vector(63.836346842212734,100.6718460204813, 0.0));
    positionAlloc4->Add (Vector(99.72660995404348,376.4776856313418, 0.0));
    positionAlloc4->Add (Vector(168.59269093998265,294.54895728244105, 0.0));
    positionAlloc4->Add (Vector(345.0754175285134,485.306625282575, 0.0));
    positionAlloc4->Add (Vector(479.2400058021785,397.9955546023628, 0.0));
    positionAlloc4->Add (Vector(68.08328494265791,102.09380656007671, 0.0));
    positionAlloc4->Add (Vector(494.685878822283,377.32921979122017, 0.0));
    positionAlloc4->Add (Vector(148.29290217786507,465.28246916570174, 0.0));
    positionAlloc4->Add (Vector(76.3570518242827,15.598636790459363, 0.0));
    positionAlloc4->Add (Vector(480.2283966531324,95.99314964859367, 0.0));
    positionAlloc4->Add (Vector(46.20174788917403,8.619608402682166, 0.0));
    positionAlloc4->Add (Vector(303.3163173798401,131.99855467162936, 0.0));
    positionAlloc4->Add (Vector(361.66946248004825,114.11693717170817, 0.0));
    positionAlloc4->Add (Vector(9.188916316032358,152.84477448551726, 0.0));
    positionAlloc4->Add (Vector(221.84499307465722,249.9484286930308, 0.0));
    positionAlloc4->Add (Vector(484.84040959639424,462.780129975332, 0.0));
    positionAlloc4->Add (Vector(56.495231333109906,19.570009295424683, 0.0));
    positionAlloc4->Add (Vector(385.68214390044875,436.8819016752079, 0.0));
    positionAlloc4->Add (Vector(355.2626724950539,362.32598794163476, 0.0));
    positionAlloc4->Add (Vector(133.650345213353,250.19164855196448, 0.0));
    positionAlloc4->Add (Vector(66.00379000380573,427.7202984766568, 0.0));
    positionAlloc4->Add (Vector(308.1213882160607,438.857278531571, 0.0));
    positionAlloc4->Add (Vector(133.11760010024508,170.87373312922793, 0.0));
    positionAlloc4->Add (Vector(317.90920298591533,345.0665085465075, 0.0));
    positionAlloc4->Add (Vector(377.53663564741674,27.809874593449745, 0.0));
    positionAlloc4->Add (Vector(339.4095095633759,130.73964362254918, 0.0));
    positionAlloc4->Add (Vector(497.2289485946257,153.09703764374567, 0.0));
    positionAlloc4->Add (Vector(179.11307052516955,149.4379031082405, 0.0));
    positionAlloc4->Add (Vector(130.3077228243788,205.61218680135428, 0.0));
    positionAlloc4->Add (Vector(472.26070515067227,66.75957720829123, 0.0));
    positionAlloc4->Add (Vector(281.1053667562143,79.77824454309452, 0.0));
    positionAlloc4->Add (Vector(488.8807288563859,261.62198675323685, 0.0));
    positionAlloc4->Add (Vector(219.74677064160036,360.16641967424226, 0.0));
    positionAlloc4->Add (Vector(23.02009946047906,326.94633388608105, 0.0));
    positionAlloc4->Add (Vector(71.42938860661707,280.2658826276765, 0.0));
    positionAlloc4->Add (Vector(192.65752340954018,486.20247809042075, 0.0));
    positionAlloc4->Add (Vector(389.717487999255,299.963738627365, 0.0));
    positionAlloc4->Add (Vector(51.42560795187756,287.69076786545276, 0.0));
    positionAlloc4->Add (Vector(189.291470680383,257.03162714045993, 0.0));
    positionAlloc4->Add (Vector(378.95955320656594,346.08591796809327, 0.0));
    positionAlloc4->Add (Vector(0.4895645680838001,111.17886779604657, 0.0));
    positionAlloc4->Add (Vector(86.53559299214686,181.83519091139942, 0.0));
    positionAlloc4->Add (Vector(489.13904196697933,69.53164198650641, 0.0));
    positionAlloc4->Add (Vector(256.80450709214017,332.8232237544164, 0.0));
    positionAlloc4->Add (Vector(450.3765107472614,466.03204441323294, 0.0));
    positionAlloc4->Add (Vector(255.32714779458183,336.1215146480276, 0.0));
    positionAlloc4->Add (Vector(127.24046336935041,432.90295049968745, 0.0));
    positionAlloc4->Add (Vector(361.1110570577572,189.9361819246382, 0.0));
    positionAlloc4->Add (Vector(231.70189293860128,236.80508283478625, 0.0));
    positionAlloc4->Add (Vector(124.83376149269753,160.78175102671665, 0.0));
    positionAlloc4->Add (Vector(109.56270238806914,72.49826169655005, 0.0));
    positionAlloc4->Add (Vector(482.89181533709024,243.31908944558444, 0.0));
    positionAlloc4->Add (Vector(44.89966253722793,10.973804468655036, 0.0));
    positionAlloc4->Add (Vector(374.4416229227323,84.55296951226838, 0.0));
    positionAlloc4->Add (Vector(201.9947058379109,433.46007225030354, 0.0));
    positionAlloc4->Add (Vector(177.08059353774908,134.86988542177315, 0.0));
    positionAlloc4->Add (Vector(330.55455529849803,379.7282565805987, 0.0));
    positionAlloc4->Add (Vector(57.76970045948649,277.3460239702482, 0.0));
    positionAlloc4->Add (Vector(378.1445202239027,132.2803286047629, 0.0));
    positionAlloc4->Add (Vector(436.15232945237926,264.4393254065946, 0.0));
    positionAlloc4->Add (Vector(485.15173412574404,435.7199976726115, 0.0));
    positionAlloc4->Add (Vector(451.85021352807115,397.26993688258716, 0.0));
    positionAlloc4->Add (Vector(181.16752403680158,409.9179397001021, 0.0));
    positionAlloc4->Add (Vector(240.59789410082266,172.2680304406599, 0.0));
    positionAlloc4->Add (Vector(95.42386284081694,143.56663755647858, 0.0));
    positionAlloc4->Add (Vector(281.03613845871774,54.896389801079025, 0.0));
    positionAlloc4->Add (Vector(439.1930328012821,367.151805349383, 0.0));
    positionAlloc4->Add (Vector(140.72631053168377,110.04782279899572, 0.0));
    positionAlloc4->Add (Vector(429.8844794160698,495.28510837558946, 0.0));
    positionAlloc4->Add (Vector(414.5686831906248,183.1628913587874, 0.0));
    positionAlloc4->Add (Vector(138.8610107068558,341.9424996761191, 0.0));
    positionAlloc4->Add (Vector(378.14019524687797,79.9111650133058, 0.0));
    positionAlloc4->Add (Vector(384.0702270768883,187.32131836441428, 0.0));
    positionAlloc4->Add (Vector(252.88191800950716,452.35996725212436, 0.0));
    positionAlloc4->Add (Vector(412.4790505417177,117.15915512821745, 0.0));
    positionAlloc4->Add (Vector(293.91620311705606,312.69413218877145, 0.0));
    positionAlloc4->Add (Vector(367.41659602389035,343.3110415088391, 0.0));
    positionAlloc4->Add (Vector(421.8615998261098,428.5768343405588, 0.0));
    positionAlloc4->Add (Vector(152.5204071650904,21.135219381909877, 0.0));
    positionAlloc4->Add (Vector(219.6619368977235,140.12434054531943, 0.0));
    positionAlloc4->Add (Vector(194.04729467141323,2.251230335691856, 0.0));
    positionAlloc4->Add (Vector(176.77030032377388,369.499910807635, 0.0));
    positionAlloc4->Add (Vector(409.1277318866587,194.55909596671773, 0.0));
    positionAlloc4->Add (Vector(228.76968381546493,11.241754333359832, 0.0));
    positionAlloc4->Add (Vector(412.5215530822951,11.343412360956206, 0.0));
    positionAlloc4->Add (Vector(445.4244226987801,82.0674035106298, 0.0));
    positionAlloc4->Add (Vector(323.09631627248973,404.65246078364083, 0.0));
    positionAlloc4->Add (Vector(424.2874305455729,286.49916313241755, 0.0));
    positionAlloc4->Add (Vector(482.65998600809337,15.110655704662745, 0.0));
    positionAlloc4->Add (Vector(240.01717975544872,57.72111805807928, 0.0));
    positionAlloc4->Add (Vector(378.4488891687319,447.74408060353153, 0.0));
    positionAlloc4->Add (Vector(427.16099061567843,159.0353468129168, 0.0));
    positionAlloc4->Add (Vector(343.7921157007056,366.1900791343849, 0.0));
    positionAlloc4->Add (Vector(1.2085792337545587,465.1290780555696, 0.0));
    positionAlloc4->Add (Vector(477.3939220065703,132.05195162658111, 0.0));
    positionAlloc4->Add (Vector(392.22285433210385,147.3192820840995, 0.0));
    positionAlloc4->Add (Vector(360.42318939158764,248.40429099618893, 0.0));
    positionAlloc4->Add (Vector(274.9541536501529,164.47846245567067, 0.0));
    positionAlloc4->Add (Vector(97.70303768354881,229.48278026613482, 0.0));
    positionAlloc4->Add (Vector(392.98234365500633,250.82845195091434, 0.0));
    positionAlloc4->Add (Vector(200.65076083561334,14.294640809738468, 0.0));
    positionAlloc4->Add (Vector(182.85693110883477,400.1442053144105, 0.0));
    positionAlloc4->Add (Vector(89.6899411265602,399.6203083367163, 0.0));
    positionAlloc4->Add (Vector(253.17903305026834,428.17487381153126, 0.0));
    positionAlloc4->Add (Vector(181.10958795127624,373.26720142624816, 0.0));
    positionAlloc4->Add (Vector(64.22771021633844,451.2275478453395, 0.0));
    positionAlloc4->Add (Vector(337.59485826634653,49.002889956807905, 0.0));
    positionAlloc4->Add (Vector(329.00966842628503,218.16282962972343, 0.0));
    positionAlloc4->Add (Vector(403.60881834024445,24.039559916671006, 0.0));
    positionAlloc4->Add (Vector(338.3422166482459,10.750663836439022, 0.0));
    positionAlloc4->Add (Vector(23.178194319251723,244.52059620869454, 0.0));
    positionAlloc4->Add (Vector(281.9631883573113,422.5309184128838, 0.0));
    positionAlloc4->Add (Vector(450.7924026045515,259.8334706262584, 0.0));
    positionAlloc4->Add (Vector(106.59348668010065,309.7062345588235, 0.0));
    positionAlloc4->Add (Vector(429.88732263024247,212.11589299807298, 0.0));
    positionAlloc4->Add (Vector(151.7497900208743,291.1350330511523, 0.0));
    positionAlloc4->Add (Vector(145.46097115602367,177.58997019510443, 0.0));
    positionAlloc4->Add (Vector(100.2167289775368,381.51729983994613, 0.0));
    positionAlloc4->Add (Vector(253.86582152019454,26.40190562058642, 0.0));
    positionAlloc4->Add (Vector(398.2684971852217,344.2578146606229, 0.0));
    positionAlloc4->Add (Vector(251.43142089237463,215.6089833062378, 0.0));
    positionAlloc4->Add (Vector(170.29172487808725,91.12948312367203, 0.0));
    positionAlloc4->Add (Vector(227.1774670986879,480.17323433859735, 0.0));
    positionAlloc4->Add (Vector(346.3439060829431,55.063680337940234, 0.0));
    positionAlloc4->Add (Vector(477.613246711357,278.21324190634033, 0.0));
    positionAlloc4->Add (Vector(413.1811306563392,214.0219479163073, 0.0));
    positionAlloc4->Add (Vector(429.3148081779741,205.77291192347707, 0.0));
    positionAlloc4->Add (Vector(483.48749866822584,260.12500185988483, 0.0));
    positionAlloc4->Add (Vector(443.0945917770347,255.697480716859, 0.0));
    positionAlloc4->Add (Vector(369.6139942489779,320.6091610492082, 0.0));
    positionAlloc4->Add (Vector(407.7199821966097,367.8292762209771, 0.0));
    positionAlloc4->Add (Vector(239.44956437714782,270.41057552749766, 0.0));
    positionAlloc4->Add (Vector(128.96284447340355,193.35233545790288, 0.0));
    positionAlloc4->Add (Vector(124.76111460747514,85.4227760982288, 0.0));
    positionAlloc4->Add (Vector(341.9551788885118,371.89874635662466, 0.0));
    positionAlloc4->Add (Vector(73.27429352537695,23.949751804347997, 0.0));
    positionAlloc4->Add (Vector(240.58172042907623,178.17103630525017, 0.0));
    positionAlloc4->Add (Vector(215.06338130825765,399.7198561160994, 0.0));
    positionAlloc4->Add (Vector(365.5509056795247,207.63520676940078, 0.0));
    positionAlloc4->Add (Vector(207.78388614863036,425.1747501306061, 0.0));
    positionAlloc4->Add (Vector(139.23389131581004,308.2668950129926, 0.0));
    positionAlloc4->Add (Vector(75.228227288345,36.58534714173117, 0.0));
    positionAlloc4->Add (Vector(247.21993765002816,433.6916738952236, 0.0));
    positionAlloc4->Add (Vector(346.3582707412023,60.171246659259104, 0.0));
    positionAlloc4->Add (Vector(455.670741011183,407.6523635772958, 0.0));
    positionAlloc4->Add (Vector(51.07264298688513,36.15020153050364, 0.0));
    positionAlloc4->Add (Vector(270.6055212821683,428.0868505388253, 0.0));
    positionAlloc4->Add (Vector(300.2099818839239,295.4919507266246, 0.0));
    positionAlloc4->Add (Vector(12.481518823227677,129.53614499404537, 0.0));
    positionAlloc4->Add (Vector(417.470086084639,61.9506279026954, 0.0));
    positionAlloc4->Add (Vector(421.20143820124395,5.166240773815501, 0.0));
    positionAlloc4->Add (Vector(366.32311164373755,338.6979335512155, 0.0));
    positionAlloc4->Add (Vector(220.67630465285708,486.14612522385397, 0.0));
    positionAlloc4->Add (Vector(497.24791156036673,380.4027299365678, 0.0));
    positionAlloc4->Add (Vector(191.0807966964892,335.3194144495442, 0.0));
    positionAlloc4->Add (Vector(329.88663844522057,456.92941487384826, 0.0));
    positionAlloc4->Add (Vector(374.9010956228573,335.4789932222706, 0.0));
    positionAlloc4->Add (Vector(344.8214832916072,365.98019994974317, 0.0));
    positionAlloc4->Add (Vector(494.24191347139254,122.7546508398515, 0.0));
    positionAlloc4->Add (Vector(184.0206541940816,394.28344398469875, 0.0));
    positionAlloc4->Add (Vector(83.1968885342459,291.16755124862607, 0.0));
    positionAlloc4->Add (Vector(356.5655386813128,202.579856898907, 0.0));
    positionAlloc4->Add (Vector(97.20896103656446,468.7594352805075, 0.0));
    positionAlloc4->Add (Vector(349.46827727014113,83.28152169644132, 0.0));
    positionAlloc4->Add (Vector(178.0074690477062,363.4434438869993, 0.0));
    positionAlloc4->Add (Vector(93.07763366181592,133.73691414028227, 0.0));
    positionAlloc4->Add (Vector(497.7815983302297,229.09690062204496, 0.0));
    positionAlloc4->Add (Vector(83.04574255856522,493.74707316622136, 0.0));
    positionAlloc4->Add (Vector(175.25119632939905,349.9795224648466, 0.0));
    positionAlloc4->Add (Vector(80.49101519478941,384.8161339938102, 0.0));
    positionAlloc4->Add (Vector(28.670870092500024,105.73012439294239, 0.0));
    positionAlloc4->Add (Vector(437.607669476189,31.11117203801006, 0.0));
    positionAlloc4->Add (Vector(35.687986867064204,40.64837818288319, 0.0));
    positionAlloc4->Add (Vector(357.74926346162863,198.69206638177283, 0.0));
    positionAlloc4->Add (Vector(294.31057582254346,44.23609981147264, 0.0));
    positionAlloc4->Add (Vector(315.1499745071596,229.98663044323757, 0.0));
    positionAlloc4->Add (Vector(256.61522381744624,288.37785032083775, 0.0));
    positionAlloc4->Add (Vector(199.35357757956297,277.46929032019796, 0.0));
    positionAlloc4->Add (Vector(213.70976715733315,64.98815397731595, 0.0));
    positionAlloc4->Add (Vector(146.88551291102624,387.58490381904966, 0.0));
    positionAlloc4->Add (Vector(148.2237667971108,45.79583731609904, 0.0));
    positionAlloc4->Add (Vector(132.07012680975134,89.010605955403, 0.0));
    positionAlloc4->Add (Vector(117.84057502659451,75.66609828795661, 0.0));
    positionAlloc4->Add (Vector(468.73450963268726,122.5233559145793, 0.0));
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
	  flowmon->SerializeToXmlFile ("scratch/switch_SA_flowmon/switch_SA14.flowmon", false, false);
	 //FIM FLOW MONITOR
		Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (serverApps.Get (0));
		std::cout << "Bytes received by server 1: " << sink1->GetTotalRx () << " ("
							<< (8. * sink1->GetTotalRx ()) / 1000000 / simTime << " Mbps)"
							<< std::endl;
	 return 0;
	}