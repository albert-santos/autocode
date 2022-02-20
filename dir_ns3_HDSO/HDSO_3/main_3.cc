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
	  uint16_t numberOfRrhs = 11;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 201;
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
    positionAlloc->Add (Vector (333.3333333333333,0.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,500.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (0.0,500.0, 0.0));
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
    positionAlloc4->Add (Vector(133.19166245322705,335.5820884086484, 0.0));
    positionAlloc4->Add (Vector(205.55585961886075,362.93202501872594, 0.0));
    positionAlloc4->Add (Vector(121.79613840138842,234.34195357531794, 0.0));
    positionAlloc4->Add (Vector(234.7117914763418,184.27468999532886, 0.0));
    positionAlloc4->Add (Vector(139.80562126584311,246.09017060861066, 0.0));
    positionAlloc4->Add (Vector(224.49854356398248,40.90151022571603, 0.0));
    positionAlloc4->Add (Vector(91.98390040154192,216.51521348170078, 0.0));
    positionAlloc4->Add (Vector(44.73225971623418,471.71622463704443, 0.0));
    positionAlloc4->Add (Vector(387.5066054900995,451.01228250320685, 0.0));
    positionAlloc4->Add (Vector(91.92737933223272,253.3075898999404, 0.0));
    positionAlloc4->Add (Vector(441.36212972003955,392.91038775710274, 0.0));
    positionAlloc4->Add (Vector(114.9428628193978,322.6017538601368, 0.0));
    positionAlloc4->Add (Vector(146.41818958301545,439.16779048846377, 0.0));
    positionAlloc4->Add (Vector(281.3057224563737,58.17724254413192, 0.0));
    positionAlloc4->Add (Vector(188.2486233286824,312.26644606312664, 0.0));
    positionAlloc4->Add (Vector(198.74988844130874,327.3673680852131, 0.0));
    positionAlloc4->Add (Vector(329.74247681844776,461.1078942471495, 0.0));
    positionAlloc4->Add (Vector(6.051774272050226,114.67666732285025, 0.0));
    positionAlloc4->Add (Vector(11.700225347285931,431.3167869832296, 0.0));
    positionAlloc4->Add (Vector(101.09936861395968,111.07629225263926, 0.0));
    positionAlloc4->Add (Vector(400.92723919484166,3.3786217564635845, 0.0));
    positionAlloc4->Add (Vector(413.8890380815017,143.38614787318537, 0.0));
    positionAlloc4->Add (Vector(311.7179507929058,10.61251250705203, 0.0));
    positionAlloc4->Add (Vector(335.18289899561614,80.1307912688189, 0.0));
    positionAlloc4->Add (Vector(432.2520342665703,68.22264071964607, 0.0));
    positionAlloc4->Add (Vector(225.94211160831122,381.0589834495653, 0.0));
    positionAlloc4->Add (Vector(254.28770036458886,2.1079580383938645, 0.0));
    positionAlloc4->Add (Vector(457.26559359239985,491.12926083884224, 0.0));
    positionAlloc4->Add (Vector(343.7028274267289,363.0420525407764, 0.0));
    positionAlloc4->Add (Vector(147.33852811511815,367.07506645956437, 0.0));
    positionAlloc4->Add (Vector(255.71652492600566,22.75944385172218, 0.0));
    positionAlloc4->Add (Vector(17.92393756249683,205.93447925285213, 0.0));
    positionAlloc4->Add (Vector(134.97979142362192,317.67032914246124, 0.0));
    positionAlloc4->Add (Vector(405.6183665015148,453.5772803732292, 0.0));
    positionAlloc4->Add (Vector(103.91405665986831,335.4722443632291, 0.0));
    positionAlloc4->Add (Vector(366.39024634099894,397.8178794264246, 0.0));
    positionAlloc4->Add (Vector(87.38735030222028,263.60759003143653, 0.0));
    positionAlloc4->Add (Vector(477.50777704550734,375.48994060344296, 0.0));
    positionAlloc4->Add (Vector(467.6065366396389,78.0021561691675, 0.0));
    positionAlloc4->Add (Vector(424.24102228942945,201.127226731962, 0.0));
    positionAlloc4->Add (Vector(121.45208118622608,460.329544964184, 0.0));
    positionAlloc4->Add (Vector(485.0036466399708,258.9365235944088, 0.0));
    positionAlloc4->Add (Vector(326.0240835392015,273.587228517255, 0.0));
    positionAlloc4->Add (Vector(231.26768864418597,352.5700981295229, 0.0));
    positionAlloc4->Add (Vector(331.06849754559147,486.15487990883827, 0.0));
    positionAlloc4->Add (Vector(177.43421866089687,466.9762558653788, 0.0));
    positionAlloc4->Add (Vector(108.74806026313144,56.08769662174057, 0.0));
    positionAlloc4->Add (Vector(434.2951476420234,498.9213643294915, 0.0));
    positionAlloc4->Add (Vector(166.54795378472437,177.0959435562182, 0.0));
    positionAlloc4->Add (Vector(77.19589498929614,240.97484549481646, 0.0));
    positionAlloc4->Add (Vector(467.47542879758566,464.45865295345374, 0.0));
    positionAlloc4->Add (Vector(35.637366761069764,50.367715885016985, 0.0));
    positionAlloc4->Add (Vector(117.50891229099297,421.5987291616756, 0.0));
    positionAlloc4->Add (Vector(300.0379717070116,127.44549820364509, 0.0));
    positionAlloc4->Add (Vector(80.8826161320505,129.08330910898448, 0.0));
    positionAlloc4->Add (Vector(5.361395464361207,427.9036377369716, 0.0));
    positionAlloc4->Add (Vector(336.23418639743073,79.367214393746, 0.0));
    positionAlloc4->Add (Vector(349.33336202708637,170.27159142551352, 0.0));
    positionAlloc4->Add (Vector(209.54294415774015,461.517573331491, 0.0));
    positionAlloc4->Add (Vector(431.83135675641984,143.18627258640964, 0.0));
    positionAlloc4->Add (Vector(408.1823328102197,220.32237499315093, 0.0));
    positionAlloc4->Add (Vector(166.93890153457923,60.00155543565799, 0.0));
    positionAlloc4->Add (Vector(145.3356471491637,151.9234640077523, 0.0));
    positionAlloc4->Add (Vector(49.83373985953271,459.59419914405447, 0.0));
    positionAlloc4->Add (Vector(369.4029931720127,28.500361947123633, 0.0));
    positionAlloc4->Add (Vector(59.833637739294176,259.97322396063254, 0.0));
    positionAlloc4->Add (Vector(36.10373158800995,197.31007057115906, 0.0));
    positionAlloc4->Add (Vector(487.4088696362892,86.06847744308921, 0.0));
    positionAlloc4->Add (Vector(272.75237809845106,270.8506215500747, 0.0));
    positionAlloc4->Add (Vector(458.9811537130739,269.8975204011496, 0.0));
    positionAlloc4->Add (Vector(252.21963758485504,45.330449069566626, 0.0));
    positionAlloc4->Add (Vector(448.87274656958397,130.07850145871726, 0.0));
    positionAlloc4->Add (Vector(255.0690247441872,219.7746666432686, 0.0));
    positionAlloc4->Add (Vector(277.7050064326616,248.19213666209828, 0.0));
    positionAlloc4->Add (Vector(173.33260842605412,101.42013310372356, 0.0));
    positionAlloc4->Add (Vector(82.37879978750045,481.1788157740992, 0.0));
    positionAlloc4->Add (Vector(426.291365096457,148.3924614480384, 0.0));
    positionAlloc4->Add (Vector(28.29762174850353,21.166522826353063, 0.0));
    positionAlloc4->Add (Vector(108.08950714046483,152.65164015591432, 0.0));
    positionAlloc4->Add (Vector(162.15996186646598,377.0582335486579, 0.0));
    positionAlloc4->Add (Vector(274.56989858104276,169.3706130181629, 0.0));
    positionAlloc4->Add (Vector(354.90256047248886,243.2537637184508, 0.0));
    positionAlloc4->Add (Vector(486.81657581631055,367.2269405858576, 0.0));
    positionAlloc4->Add (Vector(94.37160588366118,418.29170537992945, 0.0));
    positionAlloc4->Add (Vector(304.30440271691685,217.42747301034953, 0.0));
    positionAlloc4->Add (Vector(278.3406861476286,362.36393556651007, 0.0));
    positionAlloc4->Add (Vector(419.09778620161154,384.07926803736865, 0.0));
    positionAlloc4->Add (Vector(375.6403455306588,257.7874822055874, 0.0));
    positionAlloc4->Add (Vector(426.59675818167716,144.9197895608997, 0.0));
    positionAlloc4->Add (Vector(236.76249566929081,237.58167632635303, 0.0));
    positionAlloc4->Add (Vector(167.32619564522133,221.0558299488905, 0.0));
    positionAlloc4->Add (Vector(56.297384417972495,341.9881250204877, 0.0));
    positionAlloc4->Add (Vector(201.084961605527,125.33779527971456, 0.0));
    positionAlloc4->Add (Vector(20.456813208667047,20.778035650317108, 0.0));
    positionAlloc4->Add (Vector(128.46195570006586,192.2717045522272, 0.0));
    positionAlloc4->Add (Vector(308.5927939120207,322.1423530936929, 0.0));
    positionAlloc4->Add (Vector(298.39549124425764,435.2460270401716, 0.0));
    positionAlloc4->Add (Vector(23.648609637295813,442.0682650998285, 0.0));
    positionAlloc4->Add (Vector(153.57507563831646,92.30233110845198, 0.0));
    positionAlloc4->Add (Vector(220.8598069399856,87.24244179405721, 0.0));
    positionAlloc4->Add (Vector(337.19695640129464,398.42695014764746, 0.0));
    positionAlloc4->Add (Vector(289.1633560362538,348.96443823966985, 0.0));
    positionAlloc4->Add (Vector(394.25332344855116,16.473640000113974, 0.0));
    positionAlloc4->Add (Vector(338.6097224754917,153.60100788542786, 0.0));
    positionAlloc4->Add (Vector(61.751288566917474,316.7076124360426, 0.0));
    positionAlloc4->Add (Vector(22.177068017350255,210.41094750791305, 0.0));
    positionAlloc4->Add (Vector(440.2919513599155,139.67584375929005, 0.0));
    positionAlloc4->Add (Vector(342.0725202302308,192.13789907022883, 0.0));
    positionAlloc4->Add (Vector(141.3003579501067,331.6277790280085, 0.0));
    positionAlloc4->Add (Vector(363.2851994811972,77.5267097877722, 0.0));
    positionAlloc4->Add (Vector(191.29065728816647,33.18988858712185, 0.0));
    positionAlloc4->Add (Vector(84.57766422678314,428.25571223494717, 0.0));
    positionAlloc4->Add (Vector(68.77078377509349,123.54938231731249, 0.0));
    positionAlloc4->Add (Vector(496.7354795568046,178.91678999645632, 0.0));
    positionAlloc4->Add (Vector(282.7292611275382,130.1558155065961, 0.0));
    positionAlloc4->Add (Vector(427.23312630779054,236.9602009175655, 0.0));
    positionAlloc4->Add (Vector(237.71731548488023,207.3154114848984, 0.0));
    positionAlloc4->Add (Vector(296.925568636982,387.49200239971685, 0.0));
    positionAlloc4->Add (Vector(25.034462303892614,143.0012341414047, 0.0));
    positionAlloc4->Add (Vector(206.73987897104334,51.44211737968407, 0.0));
    positionAlloc4->Add (Vector(431.06875023472924,390.3765833913137, 0.0));
    positionAlloc4->Add (Vector(483.64755294956274,45.69195162276557, 0.0));
    positionAlloc4->Add (Vector(439.1237158832099,303.89686257502746, 0.0));
    positionAlloc4->Add (Vector(491.7715621786218,300.556692674312, 0.0));
    positionAlloc4->Add (Vector(405.78272986039656,306.12265686348206, 0.0));
    positionAlloc4->Add (Vector(37.01322713009764,230.40263882776057, 0.0));
    positionAlloc4->Add (Vector(116.93209708353142,18.740821649062877, 0.0));
    positionAlloc4->Add (Vector(190.8998800206767,89.39624437369176, 0.0));
    positionAlloc4->Add (Vector(475.53170781871455,322.07155990784173, 0.0));
    positionAlloc4->Add (Vector(479.37481793869097,256.3138513991659, 0.0));
    positionAlloc4->Add (Vector(50.47295694958143,231.3975633064374, 0.0));
    positionAlloc4->Add (Vector(367.9598920749667,84.19500954592507, 0.0));
    positionAlloc4->Add (Vector(105.02112732709939,330.0710947813934, 0.0));
    positionAlloc4->Add (Vector(130.9438945087589,258.0522085796553, 0.0));
    positionAlloc4->Add (Vector(124.89682465113766,317.62056649117943, 0.0));
    positionAlloc4->Add (Vector(333.57790153695527,441.97842737277034, 0.0));
    positionAlloc4->Add (Vector(473.9773183159716,93.11952930021012, 0.0));
    positionAlloc4->Add (Vector(12.357727895143555,20.65347058726219, 0.0));
    positionAlloc4->Add (Vector(235.7832490620137,458.6500992814482, 0.0));
    positionAlloc4->Add (Vector(41.36032978219784,9.889007150905183, 0.0));
    positionAlloc4->Add (Vector(472.7558893852978,441.4556286813718, 0.0));
    positionAlloc4->Add (Vector(459.0398991880186,12.214726684455169, 0.0));
    positionAlloc4->Add (Vector(305.0585377844612,469.8763740127027, 0.0));
    positionAlloc4->Add (Vector(313.4086898078012,323.96952796743074, 0.0));
    positionAlloc4->Add (Vector(199.92729524679248,499.7354495516569, 0.0));
    positionAlloc4->Add (Vector(250.22150558960632,37.36286595026067, 0.0));
    positionAlloc4->Add (Vector(203.4615348034502,354.90416848111005, 0.0));
    positionAlloc4->Add (Vector(341.3344065967151,378.65935539170994, 0.0));
    positionAlloc4->Add (Vector(346.45825697519575,16.02088390275913, 0.0));
    positionAlloc4->Add (Vector(292.00514722874226,203.09868197256154, 0.0));
    positionAlloc4->Add (Vector(14.063894338121774,69.92773112767142, 0.0));
    positionAlloc4->Add (Vector(98.7598550553539,129.0252318031125, 0.0));
    positionAlloc4->Add (Vector(232.2483801285044,390.44200575426515, 0.0));
    positionAlloc4->Add (Vector(201.39002888385406,214.01479149471984, 0.0));
    positionAlloc4->Add (Vector(443.34599763518673,66.14619127968402, 0.0));
    positionAlloc4->Add (Vector(210.8642200313236,435.6285634884713, 0.0));
    positionAlloc4->Add (Vector(339.338531933817,469.68547393278664, 0.0));
    positionAlloc4->Add (Vector(196.6179824964921,98.79757001115507, 0.0));
    positionAlloc4->Add (Vector(278.32444039854346,146.33193151656226, 0.0));
    positionAlloc4->Add (Vector(276.290670961945,394.8428396028188, 0.0));
    positionAlloc4->Add (Vector(280.37948624461336,194.85946399497894, 0.0));
    positionAlloc4->Add (Vector(481.95835454275584,322.6256005105578, 0.0));
    positionAlloc4->Add (Vector(97.43565332363934,207.66208371886975, 0.0));
    positionAlloc4->Add (Vector(330.3420180533507,311.12690845231214, 0.0));
    positionAlloc4->Add (Vector(82.78749398905822,200.4886840355144, 0.0));
    positionAlloc4->Add (Vector(304.61156322481025,229.73075305518324, 0.0));
    positionAlloc4->Add (Vector(46.25502184664742,494.96758766556206, 0.0));
    positionAlloc4->Add (Vector(236.40140582479972,414.165174652904, 0.0));
    positionAlloc4->Add (Vector(75.70380734920623,81.55912261462944, 0.0));
    positionAlloc4->Add (Vector(278.28698226944675,421.2946775256447, 0.0));
    positionAlloc4->Add (Vector(93.41081953696457,43.70696612869573, 0.0));
    positionAlloc4->Add (Vector(154.47748487011432,105.66757707598629, 0.0));
    positionAlloc4->Add (Vector(461.1553326969298,217.33533599463618, 0.0));
    positionAlloc4->Add (Vector(440.1278439159259,192.47241381063972, 0.0));
    positionAlloc4->Add (Vector(156.6639603957094,157.8836600375851, 0.0));
    positionAlloc4->Add (Vector(236.63878736805262,12.234022984049798, 0.0));
    positionAlloc4->Add (Vector(206.01166305750672,322.57545052183497, 0.0));
    positionAlloc4->Add (Vector(333.2494928042083,446.2482668057489, 0.0));
    positionAlloc4->Add (Vector(137.899717446049,379.4087235471952, 0.0));
    positionAlloc4->Add (Vector(178.50700828496778,62.86629026095275, 0.0));
    positionAlloc4->Add (Vector(318.81057635845565,79.5537021576479, 0.0));
    positionAlloc4->Add (Vector(9.776500999046423,307.86746251942884, 0.0));
    positionAlloc4->Add (Vector(167.57296231196068,196.6936712073708, 0.0));
    positionAlloc4->Add (Vector(78.98525056928152,309.07839539523513, 0.0));
    positionAlloc4->Add (Vector(168.28748205513173,339.4792976122522, 0.0));
    positionAlloc4->Add (Vector(456.5149554417519,215.42437671150145, 0.0));
    positionAlloc4->Add (Vector(376.72628039955185,391.6002890184564, 0.0));
    positionAlloc4->Add (Vector(437.25227754362527,161.68943289576177, 0.0));
    positionAlloc4->Add (Vector(113.54927070055426,499.4527733275509, 0.0));
    positionAlloc4->Add (Vector(38.26487593603234,61.89663213963087, 0.0));
    positionAlloc4->Add (Vector(337.7915460367438,121.15248689531583, 0.0));
    positionAlloc4->Add (Vector(404.57885286783255,165.32885084521732, 0.0));
    positionAlloc4->Add (Vector(159.02143687582392,432.0134327463539, 0.0));
    positionAlloc4->Add (Vector(340.7489470117325,0.37510027620318853, 0.0));
    positionAlloc4->Add (Vector(207.61234711524247,263.0228717896344, 0.0));
    positionAlloc4->Add (Vector(316.44517045280855,393.53988957471745, 0.0));
    positionAlloc4->Add (Vector(287.66059397879343,333.1485928006873, 0.0));
    positionAlloc4->Add (Vector(271.15191550762927,307.5873569019921, 0.0));
    positionAlloc4->Add (Vector(277.5308119158408,350.9315799104152, 0.0));
    positionAlloc4->Add (Vector(126.06224639468216,156.79369647861313, 0.0));
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
	  flowmon->SerializeToXmlFile ("scratch/switch_SA_flowmon/switch_SA3.flowmon", false, false);
	 //FIM FLOW MONITOR
		Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (serverApps.Get (0));
		std::cout << "Bytes received by server 1: " << sink1->GetTotalRx () << " ("
							<< (8. * sink1->GetTotalRx ()) / 1000000 / simTime << " Mbps)"
							<< std::endl;
	 return 0;
	}