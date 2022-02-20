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
	  uint16_t numberOfNodes = 248;
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
    positionAlloc->Add (Vector (333.3333333333333,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,0.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,500.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (0.0,500.0, 0.0));
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
    positionAlloc4->Add (Vector(411.8422015114826,197.640369024204, 0.0));
    positionAlloc4->Add (Vector(199.46192499544574,100.56911373619542, 0.0));
    positionAlloc4->Add (Vector(322.01742160351495,167.5410060391574, 0.0));
    positionAlloc4->Add (Vector(159.76974584016372,423.87651975041814, 0.0));
    positionAlloc4->Add (Vector(81.17777186523695,51.69432978803079, 0.0));
    positionAlloc4->Add (Vector(294.23226816598174,395.8718674363153, 0.0));
    positionAlloc4->Add (Vector(177.7243509798092,224.11895314742114, 0.0));
    positionAlloc4->Add (Vector(179.3156527811658,435.68146460516954, 0.0));
    positionAlloc4->Add (Vector(67.31874261946719,313.72970552939574, 0.0));
    positionAlloc4->Add (Vector(184.48377569032755,161.05583685855146, 0.0));
    positionAlloc4->Add (Vector(16.17583251411925,359.5484548607867, 0.0));
    positionAlloc4->Add (Vector(412.9995868321615,273.0090399383493, 0.0));
    positionAlloc4->Add (Vector(459.69502518761385,351.8937874667331, 0.0));
    positionAlloc4->Add (Vector(412.80428953062744,321.8110339212713, 0.0));
    positionAlloc4->Add (Vector(258.15200316983913,200.7264063946798, 0.0));
    positionAlloc4->Add (Vector(338.0542602196921,411.39706622059487, 0.0));
    positionAlloc4->Add (Vector(166.20043904730363,276.9529171639844, 0.0));
    positionAlloc4->Add (Vector(158.72365031092912,90.36008743906604, 0.0));
    positionAlloc4->Add (Vector(368.7845012243978,61.703287627502604, 0.0));
    positionAlloc4->Add (Vector(255.26451840329779,258.8902739854316, 0.0));
    positionAlloc4->Add (Vector(297.0769397101096,251.2926128015897, 0.0));
    positionAlloc4->Add (Vector(317.30867884553066,343.2412611516626, 0.0));
    positionAlloc4->Add (Vector(401.6301136573703,74.86638229836079, 0.0));
    positionAlloc4->Add (Vector(315.2252833106564,483.53242984866506, 0.0));
    positionAlloc4->Add (Vector(170.29040919007403,68.3038517974029, 0.0));
    positionAlloc4->Add (Vector(214.4344749025354,234.27521364856918, 0.0));
    positionAlloc4->Add (Vector(365.3236794371156,1.0320788363333278, 0.0));
    positionAlloc4->Add (Vector(171.1819083448576,96.23947165290237, 0.0));
    positionAlloc4->Add (Vector(222.48351923525573,114.76850520278231, 0.0));
    positionAlloc4->Add (Vector(69.49555978078547,65.73128004765094, 0.0));
    positionAlloc4->Add (Vector(57.902341993509296,349.92724258005927, 0.0));
    positionAlloc4->Add (Vector(411.461532561952,464.40425479833965, 0.0));
    positionAlloc4->Add (Vector(494.5380754758145,271.46937450796054, 0.0));
    positionAlloc4->Add (Vector(117.32812841116808,317.3635670799851, 0.0));
    positionAlloc4->Add (Vector(446.9775656582233,142.86965288967147, 0.0));
    positionAlloc4->Add (Vector(63.66367786247057,17.9286682787147, 0.0));
    positionAlloc4->Add (Vector(227.99102861201715,484.76148834294816, 0.0));
    positionAlloc4->Add (Vector(408.804222750975,169.27184151325835, 0.0));
    positionAlloc4->Add (Vector(413.19877484019906,292.8706658982826, 0.0));
    positionAlloc4->Add (Vector(421.71745872710767,366.54295621856863, 0.0));
    positionAlloc4->Add (Vector(132.2646994553386,437.680428877115, 0.0));
    positionAlloc4->Add (Vector(392.48304117363074,42.81936201126357, 0.0));
    positionAlloc4->Add (Vector(118.07118076711149,332.4276533029571, 0.0));
    positionAlloc4->Add (Vector(356.785825491133,121.51836739837263, 0.0));
    positionAlloc4->Add (Vector(95.52516087764357,491.95728026997654, 0.0));
    positionAlloc4->Add (Vector(161.2715274776431,465.24975374494204, 0.0));
    positionAlloc4->Add (Vector(442.7897457454308,374.6592621508157, 0.0));
    positionAlloc4->Add (Vector(110.3988260129492,416.89510181826176, 0.0));
    positionAlloc4->Add (Vector(31.890432407897297,20.73824976744737, 0.0));
    positionAlloc4->Add (Vector(328.5227821044388,391.86167700265685, 0.0));
    positionAlloc4->Add (Vector(496.7435773410796,348.5230706492136, 0.0));
    positionAlloc4->Add (Vector(306.31537902306496,396.6071915765566, 0.0));
    positionAlloc4->Add (Vector(349.1049585924202,352.2445708816923, 0.0));
    positionAlloc4->Add (Vector(220.62135778357995,464.25784583258326, 0.0));
    positionAlloc4->Add (Vector(394.9045016422641,0.21700089174292447, 0.0));
    positionAlloc4->Add (Vector(48.11137080429817,438.77130400213747, 0.0));
    positionAlloc4->Add (Vector(210.31743174162364,381.7759039764944, 0.0));
    positionAlloc4->Add (Vector(225.92898063443585,104.75509199759414, 0.0));
    positionAlloc4->Add (Vector(56.30143229044232,118.54093610947348, 0.0));
    positionAlloc4->Add (Vector(205.10604966240786,389.85637875030176, 0.0));
    positionAlloc4->Add (Vector(466.46402868008965,93.19464276779283, 0.0));
    positionAlloc4->Add (Vector(12.517639371863453,483.35343950332907, 0.0));
    positionAlloc4->Add (Vector(218.9737940904596,321.2892786185383, 0.0));
    positionAlloc4->Add (Vector(486.2893656837125,275.13088579864984, 0.0));
    positionAlloc4->Add (Vector(406.87044320421535,427.8117776729743, 0.0));
    positionAlloc4->Add (Vector(179.24983720739525,54.754373173015736, 0.0));
    positionAlloc4->Add (Vector(319.01651368827584,352.8102694501415, 0.0));
    positionAlloc4->Add (Vector(32.928071885929,294.0040530091226, 0.0));
    positionAlloc4->Add (Vector(489.44020321404196,186.75689595575483, 0.0));
    positionAlloc4->Add (Vector(21.63257963826526,316.3976009987254, 0.0));
    positionAlloc4->Add (Vector(417.33436563199047,476.84710550208166, 0.0));
    positionAlloc4->Add (Vector(449.0874254238904,147.7247112382018, 0.0));
    positionAlloc4->Add (Vector(171.71438005140044,180.1526702524568, 0.0));
    positionAlloc4->Add (Vector(410.94640562384444,42.39407687414387, 0.0));
    positionAlloc4->Add (Vector(244.59754245243786,144.89736896363337, 0.0));
    positionAlloc4->Add (Vector(312.3540730693393,67.8487855162519, 0.0));
    positionAlloc4->Add (Vector(407.67411899840715,413.1207300391782, 0.0));
    positionAlloc4->Add (Vector(433.0520418982647,101.90185015126862, 0.0));
    positionAlloc4->Add (Vector(378.2072406311274,140.64883667822048, 0.0));
    positionAlloc4->Add (Vector(479.8815371935463,153.34054682270792, 0.0));
    positionAlloc4->Add (Vector(71.35290963463959,45.717576914677814, 0.0));
    positionAlloc4->Add (Vector(61.296638931364775,190.79528924421146, 0.0));
    positionAlloc4->Add (Vector(401.3072839086294,83.18502484608592, 0.0));
    positionAlloc4->Add (Vector(286.6571656760121,183.8003105899947, 0.0));
    positionAlloc4->Add (Vector(177.73445926138527,495.01058409656605, 0.0));
    positionAlloc4->Add (Vector(169.2917640116255,349.71359230064206, 0.0));
    positionAlloc4->Add (Vector(173.1549933628283,399.890603324162, 0.0));
    positionAlloc4->Add (Vector(340.55150996911215,411.4301845524601, 0.0));
    positionAlloc4->Add (Vector(256.16281352172047,62.60125605159678, 0.0));
    positionAlloc4->Add (Vector(300.0287748785809,484.2846547271306, 0.0));
    positionAlloc4->Add (Vector(143.8541765618,482.58648886549526, 0.0));
    positionAlloc4->Add (Vector(91.69151824801396,478.5390624040678, 0.0));
    positionAlloc4->Add (Vector(456.72877249939364,290.59552704218646, 0.0));
    positionAlloc4->Add (Vector(50.98689128246608,42.6488467085474, 0.0));
    positionAlloc4->Add (Vector(453.30522491765043,180.38578215706585, 0.0));
    positionAlloc4->Add (Vector(86.54237077104094,62.43767319239246, 0.0));
    positionAlloc4->Add (Vector(339.12481815784287,66.11520624905332, 0.0));
    positionAlloc4->Add (Vector(79.61943936355587,136.94386760066624, 0.0));
    positionAlloc4->Add (Vector(21.696111688766006,179.99147996408323, 0.0));
    positionAlloc4->Add (Vector(241.32960666886888,143.4565192267616, 0.0));
    positionAlloc4->Add (Vector(49.709916023566116,134.63055640671695, 0.0));
    positionAlloc4->Add (Vector(425.41707862244425,93.37376943394493, 0.0));
    positionAlloc4->Add (Vector(74.77181877002454,192.2406385380256, 0.0));
    positionAlloc4->Add (Vector(18.854908946262185,64.18374821640704, 0.0));
    positionAlloc4->Add (Vector(277.3922351284764,187.5224179297189, 0.0));
    positionAlloc4->Add (Vector(214.33727183343444,399.86155384509357, 0.0));
    positionAlloc4->Add (Vector(40.15618441960633,456.3501847487859, 0.0));
    positionAlloc4->Add (Vector(371.83450445240976,159.9038546810163, 0.0));
    positionAlloc4->Add (Vector(363.39015824460137,119.94476911375668, 0.0));
    positionAlloc4->Add (Vector(43.44087585576345,241.66586825714703, 0.0));
    positionAlloc4->Add (Vector(57.982380014568456,394.89060750341355, 0.0));
    positionAlloc4->Add (Vector(18.029590442394884,213.12560034352157, 0.0));
    positionAlloc4->Add (Vector(329.967259478922,10.559157561113143, 0.0));
    positionAlloc4->Add (Vector(424.6610934831447,68.89738251822075, 0.0));
    positionAlloc4->Add (Vector(485.3552998139985,243.12127001100137, 0.0));
    positionAlloc4->Add (Vector(464.44764949363145,210.74531033920425, 0.0));
    positionAlloc4->Add (Vector(35.05241690250849,330.7168936434853, 0.0));
    positionAlloc4->Add (Vector(48.1042276845588,71.40559204017138, 0.0));
    positionAlloc4->Add (Vector(302.8165401285839,58.57321300950707, 0.0));
    positionAlloc4->Add (Vector(252.2298139033209,414.74296527818325, 0.0));
    positionAlloc4->Add (Vector(265.411017538752,72.5224657262385, 0.0));
    positionAlloc4->Add (Vector(102.9678046641238,334.4491413741645, 0.0));
    positionAlloc4->Add (Vector(28.306829005629417,204.0258715470877, 0.0));
    positionAlloc4->Add (Vector(244.49109526343332,156.30876128401295, 0.0));
    positionAlloc4->Add (Vector(262.10674897818706,78.66558614566522, 0.0));
    positionAlloc4->Add (Vector(416.79631890788744,419.15580844972135, 0.0));
    positionAlloc4->Add (Vector(498.8580012246296,216.90074581177782, 0.0));
    positionAlloc4->Add (Vector(0.5168392642411757,220.00428667441614, 0.0));
    positionAlloc4->Add (Vector(231.13632628587183,189.80330834611487, 0.0));
    positionAlloc4->Add (Vector(286.83248215597604,112.77539969316908, 0.0));
    positionAlloc4->Add (Vector(321.3147834397622,204.04118099745594, 0.0));
    positionAlloc4->Add (Vector(219.5240722097651,262.2950458961656, 0.0));
    positionAlloc4->Add (Vector(240.15091673824767,54.10616571442167, 0.0));
    positionAlloc4->Add (Vector(246.4873516857066,463.4426426295236, 0.0));
    positionAlloc4->Add (Vector(366.90735357127653,297.3446001187428, 0.0));
    positionAlloc4->Add (Vector(370.36434170659163,400.56904002410545, 0.0));
    positionAlloc4->Add (Vector(194.71591643428272,496.46081499000627, 0.0));
    positionAlloc4->Add (Vector(186.90052666858404,488.8644933945032, 0.0));
    positionAlloc4->Add (Vector(419.6129770521411,170.85603180108944, 0.0));
    positionAlloc4->Add (Vector(177.09886990533678,187.6122611173393, 0.0));
    positionAlloc4->Add (Vector(264.773374943939,272.98962920019125, 0.0));
    positionAlloc4->Add (Vector(248.11512092415606,293.7167447395169, 0.0));
    positionAlloc4->Add (Vector(104.18582887739753,92.2538180502272, 0.0));
    positionAlloc4->Add (Vector(471.3572809398784,149.27239296099881, 0.0));
    positionAlloc4->Add (Vector(402.55593057352905,300.7211866638838, 0.0));
    positionAlloc4->Add (Vector(74.74156777275043,325.63246133616485, 0.0));
    positionAlloc4->Add (Vector(369.71173841992834,190.5179745837709, 0.0));
    positionAlloc4->Add (Vector(103.36121264405485,305.97921200560137, 0.0));
    positionAlloc4->Add (Vector(337.27587177142993,216.4614514158591, 0.0));
    positionAlloc4->Add (Vector(368.83396621435725,458.18623066587764, 0.0));
    positionAlloc4->Add (Vector(492.24593118418755,278.97280549363353, 0.0));
    positionAlloc4->Add (Vector(4.966004925041068,383.7997121012819, 0.0));
    positionAlloc4->Add (Vector(410.84791551515923,326.77532418405724, 0.0));
    positionAlloc4->Add (Vector(181.80339180426435,210.39521610742855, 0.0));
    positionAlloc4->Add (Vector(90.83233653025829,206.775766857861, 0.0));
    positionAlloc4->Add (Vector(312.78243103848183,23.322747620222238, 0.0));
    positionAlloc4->Add (Vector(157.88478059298706,103.25893495842448, 0.0));
    positionAlloc4->Add (Vector(315.985478629837,11.314799694073008, 0.0));
    positionAlloc4->Add (Vector(78.47820863698257,43.109727753353255, 0.0));
    positionAlloc4->Add (Vector(110.66648796257384,282.36942030307983, 0.0));
    positionAlloc4->Add (Vector(22.133005227465972,441.2727326724187, 0.0));
    positionAlloc4->Add (Vector(433.82335369884504,186.12402722232306, 0.0));
    positionAlloc4->Add (Vector(480.22799444545086,384.7084007855832, 0.0));
    positionAlloc4->Add (Vector(426.970703943901,397.426904547696, 0.0));
    positionAlloc4->Add (Vector(175.91597460766982,146.61915268308624, 0.0));
    positionAlloc4->Add (Vector(21.20756666611007,433.45443782915635, 0.0));
    positionAlloc4->Add (Vector(200.52531100858363,21.16821164863608, 0.0));
    positionAlloc4->Add (Vector(183.66339679482107,342.82301659571266, 0.0));
    positionAlloc4->Add (Vector(461.96775875043414,302.73312918961125, 0.0));
    positionAlloc4->Add (Vector(209.10814718159693,429.7179250979788, 0.0));
    positionAlloc4->Add (Vector(366.2736873116043,141.62935565121916, 0.0));
    positionAlloc4->Add (Vector(246.4713819327679,255.67052167221544, 0.0));
    positionAlloc4->Add (Vector(253.51210624328652,324.10559255560037, 0.0));
    positionAlloc4->Add (Vector(208.9769047937719,94.43094656879703, 0.0));
    positionAlloc4->Add (Vector(425.11044952121017,4.6111412453356255, 0.0));
    positionAlloc4->Add (Vector(172.06629892236015,98.79690209040336, 0.0));
    positionAlloc4->Add (Vector(324.0286818937857,275.3621620188417, 0.0));
    positionAlloc4->Add (Vector(471.9452478481531,461.58269513005513, 0.0));
    positionAlloc4->Add (Vector(330.6017436685497,375.5644856004676, 0.0));
    positionAlloc4->Add (Vector(388.80873833457923,180.4691012444959, 0.0));
    positionAlloc4->Add (Vector(362.35928802446955,179.1550458616585, 0.0));
    positionAlloc4->Add (Vector(369.65568597537293,411.91335398775743, 0.0));
    positionAlloc4->Add (Vector(0.8286941261089198,260.8822789002154, 0.0));
    positionAlloc4->Add (Vector(287.7477088570319,334.6234819648021, 0.0));
    positionAlloc4->Add (Vector(51.788196469142235,358.08646225008056, 0.0));
    positionAlloc4->Add (Vector(405.5037876641969,129.60769003013544, 0.0));
    positionAlloc4->Add (Vector(439.4594234126034,374.29684376606986, 0.0));
    positionAlloc4->Add (Vector(425.86244674895676,406.63315952089744, 0.0));
    positionAlloc4->Add (Vector(409.24359165060145,148.73381537633728, 0.0));
    positionAlloc4->Add (Vector(169.7279648785287,316.6802534352251, 0.0));
    positionAlloc4->Add (Vector(371.84544660343084,111.05584996154255, 0.0));
    positionAlloc4->Add (Vector(28.68523821693103,375.55994274922085, 0.0));
    positionAlloc4->Add (Vector(219.93852037082468,88.66379576415612, 0.0));
    positionAlloc4->Add (Vector(419.87561258217465,369.0230726837653, 0.0));
    positionAlloc4->Add (Vector(271.0166488208431,218.99423878994506, 0.0));
    positionAlloc4->Add (Vector(187.91296204586982,346.10199811863447, 0.0));
    positionAlloc4->Add (Vector(39.83508668282859,205.2332726365263, 0.0));
    positionAlloc4->Add (Vector(86.26681637582017,8.783275794906121, 0.0));
    positionAlloc4->Add (Vector(475.36930984338187,32.58827335223719, 0.0));
    positionAlloc4->Add (Vector(420.60887017927166,392.0142256967692, 0.0));
    positionAlloc4->Add (Vector(450.6647813392955,261.49750356081915, 0.0));
    positionAlloc4->Add (Vector(386.1140469270335,110.89770146645527, 0.0));
    positionAlloc4->Add (Vector(7.452173211339453,420.80327964111274, 0.0));
    positionAlloc4->Add (Vector(496.8347777891862,7.625579278384365, 0.0));
    positionAlloc4->Add (Vector(113.83363471153434,467.29945182039205, 0.0));
    positionAlloc4->Add (Vector(470.85551576010886,276.8729032842118, 0.0));
    positionAlloc4->Add (Vector(335.50872490762816,427.41205141753875, 0.0));
    positionAlloc4->Add (Vector(75.31290854249994,152.49967176025103, 0.0));
    positionAlloc4->Add (Vector(11.580302064485126,53.5104724539493, 0.0));
    positionAlloc4->Add (Vector(388.98016691917076,383.8300494652073, 0.0));
    positionAlloc4->Add (Vector(400.77653683926496,238.62042524298565, 0.0));
    positionAlloc4->Add (Vector(480.2910179598795,192.03752191532064, 0.0));
    positionAlloc4->Add (Vector(331.2592515239273,381.11117533576953, 0.0));
    positionAlloc4->Add (Vector(226.2225331738525,410.15250245576146, 0.0));
    positionAlloc4->Add (Vector(464.2052282501418,191.78088216781552, 0.0));
    positionAlloc4->Add (Vector(398.99483400060257,63.91633893903015, 0.0));
    positionAlloc4->Add (Vector(133.03073046233666,11.577422862229735, 0.0));
    positionAlloc4->Add (Vector(410.50970479426496,169.05828509607173, 0.0));
    positionAlloc4->Add (Vector(471.76214573989535,348.1310894247636, 0.0));
    positionAlloc4->Add (Vector(377.36213156343973,251.1947266707696, 0.0));
    positionAlloc4->Add (Vector(13.376000929665565,150.61550034691757, 0.0));
    positionAlloc4->Add (Vector(124.88299412126209,418.5780199043179, 0.0));
    positionAlloc4->Add (Vector(390.773769365769,185.2412438179998, 0.0));
    positionAlloc4->Add (Vector(234.64708502605052,126.86473143724037, 0.0));
    positionAlloc4->Add (Vector(94.7573805144929,282.07325810411623, 0.0));
    positionAlloc4->Add (Vector(157.18568001692097,9.256654951186494, 0.0));
    positionAlloc4->Add (Vector(117.6589289003735,80.8348546231541, 0.0));
    positionAlloc4->Add (Vector(97.36957716192212,154.44985806940238, 0.0));
    positionAlloc4->Add (Vector(91.24003298670263,450.12051544474065, 0.0));
    positionAlloc4->Add (Vector(164.87089551228152,446.5718242656458, 0.0));
    positionAlloc4->Add (Vector(338.1480915361336,413.2367823805291, 0.0));
    positionAlloc4->Add (Vector(156.68748766461803,458.71597340852674, 0.0));
    positionAlloc4->Add (Vector(222.12149023755757,108.84836106404482, 0.0));
    positionAlloc4->Add (Vector(252.7363304847424,4.618320237966711, 0.0));
    positionAlloc4->Add (Vector(316.3331794323507,230.66956634492126, 0.0));
    positionAlloc4->Add (Vector(68.94587118856543,122.40270724984053, 0.0));
    positionAlloc4->Add (Vector(177.84471437249388,43.14811770574861, 0.0));
    positionAlloc4->Add (Vector(215.3695343610984,332.0814019958552, 0.0));
    positionAlloc4->Add (Vector(444.6215525538061,178.96133815036143, 0.0));
    positionAlloc4->Add (Vector(364.39026333532166,102.11127313004064, 0.0));
    positionAlloc4->Add (Vector(97.2494287442664,329.99843710881595, 0.0));
    positionAlloc4->Add (Vector(253.14662385727888,284.81801739839165, 0.0));
    positionAlloc4->Add (Vector(403.6289421721798,401.8767047892131, 0.0));
    positionAlloc4->Add (Vector(47.375814945421254,411.9824263828503, 0.0));
    positionAlloc4->Add (Vector(310.6671956727157,291.5105528889195, 0.0));
    positionAlloc4->Add (Vector(197.2089925776898,365.6769776740668, 0.0));
    positionAlloc4->Add (Vector(390.67625481266344,81.86960124198272, 0.0));
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
	  flowmon->SerializeToXmlFile ("scratch/switch_HDSO_flowmon/switch_HDSO2.flowmon", false, false);
	 //FIM FLOW MONITOR
		Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (serverApps.Get (0));
		std::cout << "Bytes received by server 1: " << sink1->GetTotalRx () << " ("
							<< (8. * sink1->GetTotalRx ()) / 1000000 / simTime << " Mbps)"
							<< std::endl;
	 return 0;
	}