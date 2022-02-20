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
	  uint16_t numberOfRrhs = 14;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 345;
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
    positionAlloc->Add (Vector (166.66666666666666,500.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,0.0, 0.0));
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
    positionAlloc4->Add (Vector(145.6354740403703,89.53033865224441, 0.0));
    positionAlloc4->Add (Vector(54.762086975682244,11.189632631778412, 0.0));
    positionAlloc4->Add (Vector(238.7853550265758,281.44141049585585, 0.0));
    positionAlloc4->Add (Vector(257.4109743943466,256.8761663092209, 0.0));
    positionAlloc4->Add (Vector(59.91218031909046,235.06223784033347, 0.0));
    positionAlloc4->Add (Vector(311.0816447696748,268.1255864488637, 0.0));
    positionAlloc4->Add (Vector(251.92962333304052,327.0620158138121, 0.0));
    positionAlloc4->Add (Vector(311.4297336903281,240.8010238572969, 0.0));
    positionAlloc4->Add (Vector(285.82831027375755,358.4801040760604, 0.0));
    positionAlloc4->Add (Vector(73.13848535877881,474.63469583295733, 0.0));
    positionAlloc4->Add (Vector(206.42421795944722,73.55894985061312, 0.0));
    positionAlloc4->Add (Vector(452.2891805991966,223.23349569659246, 0.0));
    positionAlloc4->Add (Vector(11.69604917446726,449.16606667246015, 0.0));
    positionAlloc4->Add (Vector(443.96198521862857,389.17553193598195, 0.0));
    positionAlloc4->Add (Vector(186.96797460411818,31.04909153392077, 0.0));
    positionAlloc4->Add (Vector(436.5378967559935,432.5461852769747, 0.0));
    positionAlloc4->Add (Vector(412.94530791241993,297.63105900490456, 0.0));
    positionAlloc4->Add (Vector(436.86041382660596,312.99432763911943, 0.0));
    positionAlloc4->Add (Vector(258.74881423045883,488.9749530134752, 0.0));
    positionAlloc4->Add (Vector(148.2517117085404,239.44410072543482, 0.0));
    positionAlloc4->Add (Vector(136.02295462719098,180.10042170444729, 0.0));
    positionAlloc4->Add (Vector(190.61512754394306,483.3234875859086, 0.0));
    positionAlloc4->Add (Vector(495.92622050566115,182.24451602671417, 0.0));
    positionAlloc4->Add (Vector(306.7836069735496,188.22094019102803, 0.0));
    positionAlloc4->Add (Vector(214.1681737723946,207.4009728052195, 0.0));
    positionAlloc4->Add (Vector(257.53414223655443,399.32985587419984, 0.0));
    positionAlloc4->Add (Vector(216.89152930386496,344.4729470268738, 0.0));
    positionAlloc4->Add (Vector(280.2751957019408,474.7328444898676, 0.0));
    positionAlloc4->Add (Vector(387.1010497663612,139.80355245857595, 0.0));
    positionAlloc4->Add (Vector(57.71682676064977,104.46178538961853, 0.0));
    positionAlloc4->Add (Vector(435.4260171752213,376.19979890873464, 0.0));
    positionAlloc4->Add (Vector(459.1377061003056,294.26187544428655, 0.0));
    positionAlloc4->Add (Vector(345.56653969479254,387.1045169792112, 0.0));
    positionAlloc4->Add (Vector(107.7403801521023,370.46879544961536, 0.0));
    positionAlloc4->Add (Vector(160.28872122824356,214.2713475612999, 0.0));
    positionAlloc4->Add (Vector(88.13549052287794,485.383211679941, 0.0));
    positionAlloc4->Add (Vector(368.94280713822013,318.0291386547613, 0.0));
    positionAlloc4->Add (Vector(311.48419318078226,49.70388315780782, 0.0));
    positionAlloc4->Add (Vector(295.9390057801092,441.35070775217787, 0.0));
    positionAlloc4->Add (Vector(411.99837477998864,201.24368379396674, 0.0));
    positionAlloc4->Add (Vector(337.497272184019,158.60624400369295, 0.0));
    positionAlloc4->Add (Vector(53.7770901913111,170.55087618727333, 0.0));
    positionAlloc4->Add (Vector(334.4600965519717,155.21548337688785, 0.0));
    positionAlloc4->Add (Vector(484.2260178383359,104.86476014479268, 0.0));
    positionAlloc4->Add (Vector(89.51515210426314,491.2497018176362, 0.0));
    positionAlloc4->Add (Vector(190.29435636562968,363.0746363681229, 0.0));
    positionAlloc4->Add (Vector(46.055118761904936,383.9291664539744, 0.0));
    positionAlloc4->Add (Vector(446.40551487082803,135.78070635811207, 0.0));
    positionAlloc4->Add (Vector(405.8800273767115,75.3501915499597, 0.0));
    positionAlloc4->Add (Vector(120.17025859230468,488.16403845136233, 0.0));
    positionAlloc4->Add (Vector(47.45853787064669,249.98481661506722, 0.0));
    positionAlloc4->Add (Vector(488.5347395298161,376.54232203377165, 0.0));
    positionAlloc4->Add (Vector(167.999572566654,185.4518677868662, 0.0));
    positionAlloc4->Add (Vector(287.98090814954963,47.9518216948519, 0.0));
    positionAlloc4->Add (Vector(83.45968919274254,496.3198916659932, 0.0));
    positionAlloc4->Add (Vector(159.18890766679516,286.9871254741148, 0.0));
    positionAlloc4->Add (Vector(86.80610683642986,380.8850754966563, 0.0));
    positionAlloc4->Add (Vector(64.56517572047649,442.77949367724557, 0.0));
    positionAlloc4->Add (Vector(23.351289720034195,362.14870171585255, 0.0));
    positionAlloc4->Add (Vector(345.33211741572,54.920689117571364, 0.0));
    positionAlloc4->Add (Vector(496.7245176389146,367.5485894525581, 0.0));
    positionAlloc4->Add (Vector(241.26808567219481,91.20334916928118, 0.0));
    positionAlloc4->Add (Vector(415.49404905587494,152.76796872810283, 0.0));
    positionAlloc4->Add (Vector(443.01193912934747,348.0352579145075, 0.0));
    positionAlloc4->Add (Vector(21.06465488164372,94.60823899826265, 0.0));
    positionAlloc4->Add (Vector(4.089892280668417,231.87833288891392, 0.0));
    positionAlloc4->Add (Vector(408.37055899578485,326.3489197110266, 0.0));
    positionAlloc4->Add (Vector(23.977869477482706,361.4661875824194, 0.0));
    positionAlloc4->Add (Vector(0.04126947712407292,430.4334139689734, 0.0));
    positionAlloc4->Add (Vector(391.1881998243993,415.3873925873966, 0.0));
    positionAlloc4->Add (Vector(18.104035921897133,187.33144255615682, 0.0));
    positionAlloc4->Add (Vector(79.6805882984541,72.32281674155772, 0.0));
    positionAlloc4->Add (Vector(310.5304962627324,41.58418344094211, 0.0));
    positionAlloc4->Add (Vector(347.1315224791987,211.97203624692307, 0.0));
    positionAlloc4->Add (Vector(384.1339196499722,434.8945870698911, 0.0));
    positionAlloc4->Add (Vector(437.3641642182055,229.8378963142934, 0.0));
    positionAlloc4->Add (Vector(302.5799878658416,478.89602567682795, 0.0));
    positionAlloc4->Add (Vector(148.7531072858172,492.9598258433545, 0.0));
    positionAlloc4->Add (Vector(492.6738846207981,89.60535435780636, 0.0));
    positionAlloc4->Add (Vector(86.0513534015569,3.5255160379443207, 0.0));
    positionAlloc4->Add (Vector(303.0712621321349,271.766226718976, 0.0));
    positionAlloc4->Add (Vector(3.9757504491717555,428.52572619262287, 0.0));
    positionAlloc4->Add (Vector(189.44770841707225,167.5801781107812, 0.0));
    positionAlloc4->Add (Vector(227.71875711266432,138.68899460095875, 0.0));
    positionAlloc4->Add (Vector(425.0188217573403,414.4630398207862, 0.0));
    positionAlloc4->Add (Vector(403.84324981359964,146.69199967917245, 0.0));
    positionAlloc4->Add (Vector(34.143659612269396,208.47139956901472, 0.0));
    positionAlloc4->Add (Vector(193.4056824132399,400.77563871342807, 0.0));
    positionAlloc4->Add (Vector(419.4672956565132,40.573220295990254, 0.0));
    positionAlloc4->Add (Vector(189.23868094923034,82.62776884724293, 0.0));
    positionAlloc4->Add (Vector(309.96862261320865,314.45844149060184, 0.0));
    positionAlloc4->Add (Vector(223.0877927808955,115.49445755682864, 0.0));
    positionAlloc4->Add (Vector(284.59988936634767,199.51904948710802, 0.0));
    positionAlloc4->Add (Vector(389.03961651635774,233.86063346042187, 0.0));
    positionAlloc4->Add (Vector(381.00966123142666,111.92843969884669, 0.0));
    positionAlloc4->Add (Vector(274.0196645989187,389.1166012971567, 0.0));
    positionAlloc4->Add (Vector(55.67932051529928,416.76831444581603, 0.0));
    positionAlloc4->Add (Vector(465.5807512965805,407.2260670921163, 0.0));
    positionAlloc4->Add (Vector(374.5904443803287,235.8928165738713, 0.0));
    positionAlloc4->Add (Vector(483.106999956347,163.10221902333205, 0.0));
    positionAlloc4->Add (Vector(173.48783037144506,386.8768789612458, 0.0));
    positionAlloc4->Add (Vector(42.361267381407664,372.6278284079677, 0.0));
    positionAlloc4->Add (Vector(368.57157228066677,443.56127765075854, 0.0));
    positionAlloc4->Add (Vector(283.89234802740503,391.78991214735464, 0.0));
    positionAlloc4->Add (Vector(14.75281052038474,420.4141215484732, 0.0));
    positionAlloc4->Add (Vector(302.3679065332601,236.8218998600175, 0.0));
    positionAlloc4->Add (Vector(220.46774719399392,302.6258993351642, 0.0));
    positionAlloc4->Add (Vector(99.02822652731031,82.78841512602348, 0.0));
    positionAlloc4->Add (Vector(360.7544224696649,118.6748292266418, 0.0));
    positionAlloc4->Add (Vector(126.0749254521351,413.24388720736897, 0.0));
    positionAlloc4->Add (Vector(240.47490226794443,251.18145647553163, 0.0));
    positionAlloc4->Add (Vector(247.33877965725753,373.21598844092506, 0.0));
    positionAlloc4->Add (Vector(177.87745429388306,308.0459453678129, 0.0));
    positionAlloc4->Add (Vector(136.6931110472833,445.57805956060906, 0.0));
    positionAlloc4->Add (Vector(366.5154833984678,227.4339340149566, 0.0));
    positionAlloc4->Add (Vector(19.772591068315027,90.1289836707963, 0.0));
    positionAlloc4->Add (Vector(273.1669837266976,268.42801530869343, 0.0));
    positionAlloc4->Add (Vector(278.8021652091407,51.926131043773914, 0.0));
    positionAlloc4->Add (Vector(251.1453391979112,493.77629066427596, 0.0));
    positionAlloc4->Add (Vector(23.9768390721668,276.5202891262313, 0.0));
    positionAlloc4->Add (Vector(447.9761184513543,167.52044079533857, 0.0));
    positionAlloc4->Add (Vector(398.721709464223,217.06093123389635, 0.0));
    positionAlloc4->Add (Vector(437.98683294456595,323.9245007417304, 0.0));
    positionAlloc4->Add (Vector(236.22098424333444,165.99713286087174, 0.0));
    positionAlloc4->Add (Vector(403.0476964049684,171.5086782604691, 0.0));
    positionAlloc4->Add (Vector(168.55232669511477,427.30983684293864, 0.0));
    positionAlloc4->Add (Vector(102.79328651228082,444.9568490004877, 0.0));
    positionAlloc4->Add (Vector(198.1193404646984,317.1144389728345, 0.0));
    positionAlloc4->Add (Vector(99.70602922903599,421.2422737672602, 0.0));
    positionAlloc4->Add (Vector(162.18308727818464,436.16608044346276, 0.0));
    positionAlloc4->Add (Vector(187.86928595948277,231.6457977260033, 0.0));
    positionAlloc4->Add (Vector(477.05684645601707,300.77659337533447, 0.0));
    positionAlloc4->Add (Vector(282.4514982147067,307.37656061565, 0.0));
    positionAlloc4->Add (Vector(193.83166913408905,479.93053446119734, 0.0));
    positionAlloc4->Add (Vector(28.104106078149947,198.13921678371688, 0.0));
    positionAlloc4->Add (Vector(420.999816631628,381.7691835917747, 0.0));
    positionAlloc4->Add (Vector(102.81525333328945,456.74740405810115, 0.0));
    positionAlloc4->Add (Vector(412.43996791195605,96.80572406703902, 0.0));
    positionAlloc4->Add (Vector(396.46302314172704,451.92012510639665, 0.0));
    positionAlloc4->Add (Vector(98.1401154269273,493.6526559865785, 0.0));
    positionAlloc4->Add (Vector(469.70233086387293,367.2764295448549, 0.0));
    positionAlloc4->Add (Vector(69.63223959180853,403.6665878412921, 0.0));
    positionAlloc4->Add (Vector(376.46140220318586,93.55299217879259, 0.0));
    positionAlloc4->Add (Vector(341.2948083670433,479.3777540541336, 0.0));
    positionAlloc4->Add (Vector(86.14281676034985,123.36659377282838, 0.0));
    positionAlloc4->Add (Vector(251.8341162998084,250.89409666974083, 0.0));
    positionAlloc4->Add (Vector(115.84643456048155,326.91164873112456, 0.0));
    positionAlloc4->Add (Vector(190.19534638172448,9.85435880468949, 0.0));
    positionAlloc4->Add (Vector(153.32307588045586,393.5371648281989, 0.0));
    positionAlloc4->Add (Vector(42.19955618501503,140.77150538651918, 0.0));
    positionAlloc4->Add (Vector(465.63031193807217,419.4833539376542, 0.0));
    positionAlloc4->Add (Vector(92.47583483380184,392.93131093128085, 0.0));
    positionAlloc4->Add (Vector(431.84086965668627,73.6838321670294, 0.0));
    positionAlloc4->Add (Vector(37.70889386486465,1.9221586670464808, 0.0));
    positionAlloc4->Add (Vector(183.9488740057389,468.98606967260787, 0.0));
    positionAlloc4->Add (Vector(491.0452462965278,166.3101492269246, 0.0));
    positionAlloc4->Add (Vector(187.83769441323318,423.0485682231464, 0.0));
    positionAlloc4->Add (Vector(372.5418709947911,394.78354349029655, 0.0));
    positionAlloc4->Add (Vector(133.85532489257173,265.3083702264742, 0.0));
    positionAlloc4->Add (Vector(404.85066079573977,472.5630980229398, 0.0));
    positionAlloc4->Add (Vector(179.1221549226454,309.50332657164785, 0.0));
    positionAlloc4->Add (Vector(101.25353387021013,278.04537245270643, 0.0));
    positionAlloc4->Add (Vector(390.18798719100573,201.428096841463, 0.0));
    positionAlloc4->Add (Vector(467.04731827747617,114.28239741455198, 0.0));
    positionAlloc4->Add (Vector(345.2157444971773,59.99071050242149, 0.0));
    positionAlloc4->Add (Vector(250.7781585920449,29.232041087295492, 0.0));
    positionAlloc4->Add (Vector(197.8549385282967,153.87286073970628, 0.0));
    positionAlloc4->Add (Vector(56.61715013182727,469.48174818859684, 0.0));
    positionAlloc4->Add (Vector(47.86555522118391,7.954822693557406, 0.0));
    positionAlloc4->Add (Vector(174.6653951923538,43.507395038319466, 0.0));
    positionAlloc4->Add (Vector(202.7321005548917,482.6211763727557, 0.0));
    positionAlloc4->Add (Vector(141.46904206737193,312.1990217848285, 0.0));
    positionAlloc4->Add (Vector(307.04847935207385,180.5547609341634, 0.0));
    positionAlloc4->Add (Vector(456.0645492352686,138.88795527371178, 0.0));
    positionAlloc4->Add (Vector(81.8360247484487,22.11375383713715, 0.0));
    positionAlloc4->Add (Vector(16.45254646227151,94.8102199604986, 0.0));
    positionAlloc4->Add (Vector(420.4608264897767,4.681444283998415, 0.0));
    positionAlloc4->Add (Vector(178.99798080326357,198.51999822019854, 0.0));
    positionAlloc4->Add (Vector(264.8174171020778,352.4286801467073, 0.0));
    positionAlloc4->Add (Vector(363.8482476875261,380.0937310994066, 0.0));
    positionAlloc4->Add (Vector(456.09815293981336,200.2026164764145, 0.0));
    positionAlloc4->Add (Vector(332.2301401112345,266.7274621293418, 0.0));
    positionAlloc4->Add (Vector(239.04445334882095,425.3699605306839, 0.0));
    positionAlloc4->Add (Vector(371.6231432620783,430.8761220424103, 0.0));
    positionAlloc4->Add (Vector(88.78194415275176,280.0404992860415, 0.0));
    positionAlloc4->Add (Vector(293.97185584723303,466.1193817189592, 0.0));
    positionAlloc4->Add (Vector(21.615565031410654,125.55461332032935, 0.0));
    positionAlloc4->Add (Vector(363.67755693587293,236.8107304617787, 0.0));
    positionAlloc4->Add (Vector(94.97625138358772,160.0980427340961, 0.0));
    positionAlloc4->Add (Vector(145.99842654401263,33.46395122376106, 0.0));
    positionAlloc4->Add (Vector(431.18469357221664,324.9724112157101, 0.0));
    positionAlloc4->Add (Vector(100.75003974166863,352.6498749561225, 0.0));
    positionAlloc4->Add (Vector(272.75289477675744,158.87215659553243, 0.0));
    positionAlloc4->Add (Vector(354.8237411467245,420.942751271098, 0.0));
    positionAlloc4->Add (Vector(130.89345027071013,216.2078373059435, 0.0));
    positionAlloc4->Add (Vector(271.88391927756487,369.8450809077067, 0.0));
    positionAlloc4->Add (Vector(189.93299209859737,130.62990162133215, 0.0));
    positionAlloc4->Add (Vector(164.9547105876467,70.56517751904978, 0.0));
    positionAlloc4->Add (Vector(365.349247262939,482.39072261910553, 0.0));
    positionAlloc4->Add (Vector(175.29753458427643,189.31939829581202, 0.0));
    positionAlloc4->Add (Vector(113.43805603892393,49.4214100037304, 0.0));
    positionAlloc4->Add (Vector(120.06771876545336,241.7950590570349, 0.0));
    positionAlloc4->Add (Vector(256.9643289133041,224.15770847074228, 0.0));
    positionAlloc4->Add (Vector(461.7867751782516,254.70777012179374, 0.0));
    positionAlloc4->Add (Vector(414.76069574786067,316.885262343961, 0.0));
    positionAlloc4->Add (Vector(499.86631686698945,323.55141410937335, 0.0));
    positionAlloc4->Add (Vector(359.1364426893448,372.65248412703227, 0.0));
    positionAlloc4->Add (Vector(15.49622039934384,94.81789735471396, 0.0));
    positionAlloc4->Add (Vector(167.4944406950042,32.958438493079775, 0.0));
    positionAlloc4->Add (Vector(410.8480547710828,87.34792690716914, 0.0));
    positionAlloc4->Add (Vector(139.2449950470997,118.2928971053293, 0.0));
    positionAlloc4->Add (Vector(287.26241802964773,483.45171086403565, 0.0));
    positionAlloc4->Add (Vector(470.1034185139592,179.0937251252217, 0.0));
    positionAlloc4->Add (Vector(477.75823313558357,372.99628008363516, 0.0));
    positionAlloc4->Add (Vector(46.005965506752844,314.8703744283456, 0.0));
    positionAlloc4->Add (Vector(460.129115280128,129.83229994808408, 0.0));
    positionAlloc4->Add (Vector(175.43162941031827,485.233002652418, 0.0));
    positionAlloc4->Add (Vector(49.25423069701252,157.00294692102474, 0.0));
    positionAlloc4->Add (Vector(20.449821762950126,262.8421781803826, 0.0));
    positionAlloc4->Add (Vector(274.5870984446351,326.35848283132896, 0.0));
    positionAlloc4->Add (Vector(327.25727613712905,375.10933132832065, 0.0));
    positionAlloc4->Add (Vector(96.09568201944013,41.43845523789263, 0.0));
    positionAlloc4->Add (Vector(224.10126212274744,121.32139823751315, 0.0));
    positionAlloc4->Add (Vector(498.9999618991737,193.64459653671378, 0.0));
    positionAlloc4->Add (Vector(424.51848777442143,360.05710098693646, 0.0));
    positionAlloc4->Add (Vector(447.35851950556423,224.57034777635488, 0.0));
    positionAlloc4->Add (Vector(332.5363944387139,434.0278589048077, 0.0));
    positionAlloc4->Add (Vector(221.1517314354482,352.0399513812476, 0.0));
    positionAlloc4->Add (Vector(314.07095352097343,289.18138502820517, 0.0));
    positionAlloc4->Add (Vector(258.5508168948155,5.629914857766649, 0.0));
    positionAlloc4->Add (Vector(8.967469321267242,14.900445038059761, 0.0));
    positionAlloc4->Add (Vector(310.8287967571877,66.0607283817114, 0.0));
    positionAlloc4->Add (Vector(134.27271699203436,146.74813820547962, 0.0));
    positionAlloc4->Add (Vector(494.71746245940426,119.94181133430176, 0.0));
    positionAlloc4->Add (Vector(149.11668954495616,240.9109749336557, 0.0));
    positionAlloc4->Add (Vector(302.80672345603557,164.44729549989552, 0.0));
    positionAlloc4->Add (Vector(475.21720141009365,16.27307011762147, 0.0));
    positionAlloc4->Add (Vector(269.78389494314405,423.0802720586572, 0.0));
    positionAlloc4->Add (Vector(113.45690788520169,463.13756903742285, 0.0));
    positionAlloc4->Add (Vector(158.70946280294874,252.75745001189824, 0.0));
    positionAlloc4->Add (Vector(126.12201760114927,438.4728947785114, 0.0));
    positionAlloc4->Add (Vector(38.61613745654996,285.8878704568434, 0.0));
    positionAlloc4->Add (Vector(100.99839602648886,460.64626601370503, 0.0));
    positionAlloc4->Add (Vector(334.17205605510975,182.6970363795684, 0.0));
    positionAlloc4->Add (Vector(494.04287712948,425.06567557784274, 0.0));
    positionAlloc4->Add (Vector(196.11568225817777,43.45824761907868, 0.0));
    positionAlloc4->Add (Vector(64.60210605275307,443.2861593833782, 0.0));
    positionAlloc4->Add (Vector(276.2882795651215,191.98208866560464, 0.0));
    positionAlloc4->Add (Vector(433.3468754494596,72.85227438951419, 0.0));
    positionAlloc4->Add (Vector(405.2153944895891,310.96749941592026, 0.0));
    positionAlloc4->Add (Vector(271.84790228542465,376.24870632521106, 0.0));
    positionAlloc4->Add (Vector(221.09503984362928,123.1468648996289, 0.0));
    positionAlloc4->Add (Vector(207.21759525664456,337.9067766955605, 0.0));
    positionAlloc4->Add (Vector(301.25276685686174,431.79585333343636, 0.0));
    positionAlloc4->Add (Vector(33.2718384601331,28.490196614158194, 0.0));
    positionAlloc4->Add (Vector(338.52192878013625,472.81843426434324, 0.0));
    positionAlloc4->Add (Vector(352.70715485747314,166.1139608605112, 0.0));
    positionAlloc4->Add (Vector(153.36383151223637,401.8344087266093, 0.0));
    positionAlloc4->Add (Vector(6.60396101083216,46.172389964658336, 0.0));
    positionAlloc4->Add (Vector(117.05484620884354,136.3476645913183, 0.0));
    positionAlloc4->Add (Vector(147.69150374865598,91.02719905864592, 0.0));
    positionAlloc4->Add (Vector(445.09269194902987,108.73998324788525, 0.0));
    positionAlloc4->Add (Vector(499.45682041906474,161.5382889569655, 0.0));
    positionAlloc4->Add (Vector(82.53944291336323,339.91854948312016, 0.0));
    positionAlloc4->Add (Vector(211.81606655126862,492.1145904878654, 0.0));
    positionAlloc4->Add (Vector(342.42366061638813,212.56943038691355, 0.0));
    positionAlloc4->Add (Vector(286.189701210075,1.652557814961375, 0.0));
    positionAlloc4->Add (Vector(267.1125016235891,297.20862226592806, 0.0));
    positionAlloc4->Add (Vector(415.79388450569104,115.15908502098532, 0.0));
    positionAlloc4->Add (Vector(118.31533210184858,456.2205887138014, 0.0));
    positionAlloc4->Add (Vector(124.14500045035692,432.2716952228451, 0.0));
    positionAlloc4->Add (Vector(316.63909288254,172.29451400318013, 0.0));
    positionAlloc4->Add (Vector(421.0051356874718,353.42026935671095, 0.0));
    positionAlloc4->Add (Vector(51.7634131204997,244.66955335468177, 0.0));
    positionAlloc4->Add (Vector(218.2638974316402,341.26964275210287, 0.0));
    positionAlloc4->Add (Vector(373.290131670075,353.52004280817124, 0.0));
    positionAlloc4->Add (Vector(250.50890799835284,337.9739147693315, 0.0));
    positionAlloc4->Add (Vector(407.87481994047744,386.55726230321136, 0.0));
    positionAlloc4->Add (Vector(218.4736738216521,489.37787796104925, 0.0));
    positionAlloc4->Add (Vector(335.9374131687578,223.94781825036404, 0.0));
    positionAlloc4->Add (Vector(39.56863678844469,136.61300022264882, 0.0));
    positionAlloc4->Add (Vector(340.9513703272522,267.0364055498608, 0.0));
    positionAlloc4->Add (Vector(448.71149659693344,100.77916101820705, 0.0));
    positionAlloc4->Add (Vector(244.3486304677913,234.38856087200517, 0.0));
    positionAlloc4->Add (Vector(468.7184917665236,435.22564121086106, 0.0));
    positionAlloc4->Add (Vector(344.8888435606762,491.61634381089544, 0.0));
    positionAlloc4->Add (Vector(276.9598381972151,123.76493535448962, 0.0));
    positionAlloc4->Add (Vector(202.83129138378402,309.89774591738785, 0.0));
    positionAlloc4->Add (Vector(197.31857284648052,357.87611782288207, 0.0));
    positionAlloc4->Add (Vector(416.0002273836186,235.39145309841146, 0.0));
    positionAlloc4->Add (Vector(162.57834160197936,308.73906281444675, 0.0));
    positionAlloc4->Add (Vector(126.2031122287518,191.8845103797869, 0.0));
    positionAlloc4->Add (Vector(380.98902459541995,322.34908741196375, 0.0));
    positionAlloc4->Add (Vector(164.4491780804399,95.37535506037648, 0.0));
    positionAlloc4->Add (Vector(392.5750016484024,467.3226276779502, 0.0));
    positionAlloc4->Add (Vector(398.21886923468276,144.71575338116793, 0.0));
    positionAlloc4->Add (Vector(470.5825837327374,395.15895105843634, 0.0));
    positionAlloc4->Add (Vector(236.0406150196946,8.13544099015906, 0.0));
    positionAlloc4->Add (Vector(22.212913579818416,416.82525215551107, 0.0));
    positionAlloc4->Add (Vector(322.3429082286569,453.92539051525955, 0.0));
    positionAlloc4->Add (Vector(279.4325015509064,222.7204399212333, 0.0));
    positionAlloc4->Add (Vector(6.786125235421936,259.7328088592589, 0.0));
    positionAlloc4->Add (Vector(178.3345797532982,151.55583316834574, 0.0));
    positionAlloc4->Add (Vector(406.68419280760145,191.08693339511547, 0.0));
    positionAlloc4->Add (Vector(31.177619980673697,370.71213071555076, 0.0));
    positionAlloc4->Add (Vector(361.4853122470557,179.59543623024527, 0.0));
    positionAlloc4->Add (Vector(401.6076589867514,25.07232692145134, 0.0));
    positionAlloc4->Add (Vector(91.14096196121014,362.6837382305041, 0.0));
    positionAlloc4->Add (Vector(120.5387729203693,11.265474830230826, 0.0));
    positionAlloc4->Add (Vector(53.65158566733391,480.98945434458267, 0.0));
    positionAlloc4->Add (Vector(182.57398960606696,270.50426518944926, 0.0));
    positionAlloc4->Add (Vector(457.7107742020904,191.8704611911643, 0.0));
    positionAlloc4->Add (Vector(296.6660560393091,64.7996961285307, 0.0));
    positionAlloc4->Add (Vector(191.65817623817298,253.62788191299447, 0.0));
    positionAlloc4->Add (Vector(257.8133796616746,415.16484190373524, 0.0));
    positionAlloc4->Add (Vector(11.109036308717569,379.36260347858354, 0.0));
    positionAlloc4->Add (Vector(277.8191891720504,493.01201148488104, 0.0));
    positionAlloc4->Add (Vector(52.89855418247191,470.8506746583001, 0.0));
    positionAlloc4->Add (Vector(126.11007248003853,82.3429683479015, 0.0));
    positionAlloc4->Add (Vector(479.4202639438273,338.30340284377667, 0.0));
    positionAlloc4->Add (Vector(364.47988934518526,403.3547591431547, 0.0));
    positionAlloc4->Add (Vector(30.415358726193997,36.05472758704903, 0.0));
    positionAlloc4->Add (Vector(268.92974488340434,36.746580864173374, 0.0));
    positionAlloc4->Add (Vector(464.06779528616823,392.9501091999276, 0.0));
    positionAlloc4->Add (Vector(417.3727993325332,278.8714843859664, 0.0));
    positionAlloc4->Add (Vector(352.66465788403724,244.91657417976913, 0.0));
    positionAlloc4->Add (Vector(27.532508045473246,411.7244362968194, 0.0));
    positionAlloc4->Add (Vector(492.4875029646811,332.398249692842, 0.0));
    positionAlloc4->Add (Vector(480.9591434249966,110.78640072489826, 0.0));
    positionAlloc4->Add (Vector(387.58689059882244,455.858036660063, 0.0));
    positionAlloc4->Add (Vector(476.42345943481325,137.4056358204732, 0.0));
    positionAlloc4->Add (Vector(248.63148419507547,174.55938206493832, 0.0));
    positionAlloc4->Add (Vector(61.21003041228151,326.2191184295509, 0.0));
    positionAlloc4->Add (Vector(90.0679264148666,153.30286552465762, 0.0));
    positionAlloc4->Add (Vector(95.37022572684312,346.0339240062846, 0.0));
    positionAlloc4->Add (Vector(391.108536256888,471.3230181992034, 0.0));
    positionAlloc4->Add (Vector(15.259518467846178,119.22785907137894, 0.0));
    positionAlloc4->Add (Vector(390.42802921174194,182.93996159454545, 0.0));
    positionAlloc4->Add (Vector(306.77430984654785,33.85069556698927, 0.0));
    positionAlloc4->Add (Vector(74.94629070140807,60.18264547985663, 0.0));
    positionAlloc4->Add (Vector(21.222273520276445,47.145225982824336, 0.0));
    positionAlloc4->Add (Vector(322.5236406219879,141.20923022409985, 0.0));
    positionAlloc4->Add (Vector(135.16982215907498,371.52541753955046, 0.0));
    positionAlloc4->Add (Vector(311.18409884450654,241.53023803230107, 0.0));
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
	  flowmon->SerializeToXmlFile ("scratch/switch_HDSO_flowmon/switch_HDSO23.flowmon", false, false);
	 //FIM FLOW MONITOR
		Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (serverApps.Get (0));
		std::cout << "Bytes received by server 1: " << sink1->GetTotalRx () << " ("
							<< (8. * sink1->GetTotalRx ()) / 1000000 / simTime << " Mbps)"
							<< std::endl;
	 return 0;
	}