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
	  uint16_t numberOfNodes = 205;
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
    positionAlloc->Add (Vector (500.0,0.0, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,500.0, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,500.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,0.0, 0.0));
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
    positionAlloc4->Add (Vector(496.38643174356486,181.77163014136184, 0.0));
    positionAlloc4->Add (Vector(391.85411029482793,417.5181389589595, 0.0));
    positionAlloc4->Add (Vector(135.09485741595722,287.07328532130435, 0.0));
    positionAlloc4->Add (Vector(485.54652785198124,185.2731455933027, 0.0));
    positionAlloc4->Add (Vector(350.7981876276903,42.36373769305568, 0.0));
    positionAlloc4->Add (Vector(395.23126212990456,357.295115984325, 0.0));
    positionAlloc4->Add (Vector(221.02336077357597,18.79175753626755, 0.0));
    positionAlloc4->Add (Vector(375.7357121779027,130.6804743334579, 0.0));
    positionAlloc4->Add (Vector(127.5942137514034,52.60145543595068, 0.0));
    positionAlloc4->Add (Vector(408.20543394592363,183.79968654903078, 0.0));
    positionAlloc4->Add (Vector(139.37867871681274,359.80174105320884, 0.0));
    positionAlloc4->Add (Vector(467.82003303670166,254.6866502901682, 0.0));
    positionAlloc4->Add (Vector(436.6849610200408,149.93283775810724, 0.0));
    positionAlloc4->Add (Vector(317.1281752164962,474.59849844649307, 0.0));
    positionAlloc4->Add (Vector(57.436803916618985,183.0247692685388, 0.0));
    positionAlloc4->Add (Vector(223.38589872678483,498.36899790189756, 0.0));
    positionAlloc4->Add (Vector(234.68361714670283,185.94187802487943, 0.0));
    positionAlloc4->Add (Vector(335.78460185688226,281.4086373531101, 0.0));
    positionAlloc4->Add (Vector(218.3853201512137,25.363829004545092, 0.0));
    positionAlloc4->Add (Vector(99.22089189324062,113.10456370348737, 0.0));
    positionAlloc4->Add (Vector(275.89919290957846,107.59372394407218, 0.0));
    positionAlloc4->Add (Vector(245.6513821578901,473.1179976138551, 0.0));
    positionAlloc4->Add (Vector(491.0404848896185,381.8074808788902, 0.0));
    positionAlloc4->Add (Vector(447.9302877455933,192.73871711300777, 0.0));
    positionAlloc4->Add (Vector(384.77490266011716,49.98722823397317, 0.0));
    positionAlloc4->Add (Vector(459.50632431414687,371.01651535356, 0.0));
    positionAlloc4->Add (Vector(5.718028398166542,472.42607135899163, 0.0));
    positionAlloc4->Add (Vector(396.31952485313,263.9143196423724, 0.0));
    positionAlloc4->Add (Vector(207.2048821580111,444.5371881535134, 0.0));
    positionAlloc4->Add (Vector(168.51878053095936,107.70816424155011, 0.0));
    positionAlloc4->Add (Vector(448.1221829182996,404.1649717386085, 0.0));
    positionAlloc4->Add (Vector(452.47336906601305,324.17402008288104, 0.0));
    positionAlloc4->Add (Vector(352.73610366774693,473.98559079215255, 0.0));
    positionAlloc4->Add (Vector(408.46516282818703,495.7707837179865, 0.0));
    positionAlloc4->Add (Vector(221.83635149241314,169.12102239593878, 0.0));
    positionAlloc4->Add (Vector(71.7314138495857,350.8216017662171, 0.0));
    positionAlloc4->Add (Vector(310.8574594545692,360.3423138439067, 0.0));
    positionAlloc4->Add (Vector(242.47305573615824,241.63992229724568, 0.0));
    positionAlloc4->Add (Vector(109.58961480869972,387.70692325981844, 0.0));
    positionAlloc4->Add (Vector(199.98175461979943,80.93721798036974, 0.0));
    positionAlloc4->Add (Vector(491.5669167347135,314.3845487354331, 0.0));
    positionAlloc4->Add (Vector(454.2864742798003,260.5602642919657, 0.0));
    positionAlloc4->Add (Vector(126.68623091565829,473.3987138073811, 0.0));
    positionAlloc4->Add (Vector(483.99175314793274,399.660978382018, 0.0));
    positionAlloc4->Add (Vector(243.87766606300627,234.25492886299065, 0.0));
    positionAlloc4->Add (Vector(62.03258663594718,395.65566031816195, 0.0));
    positionAlloc4->Add (Vector(64.05438402422719,243.51918798507504, 0.0));
    positionAlloc4->Add (Vector(216.28511611514966,12.54182664793041, 0.0));
    positionAlloc4->Add (Vector(347.5996766779357,238.71615444484783, 0.0));
    positionAlloc4->Add (Vector(83.27630485588328,171.9608880089431, 0.0));
    positionAlloc4->Add (Vector(150.46228854638426,285.25625825252285, 0.0));
    positionAlloc4->Add (Vector(256.56692184315324,440.92899006379963, 0.0));
    positionAlloc4->Add (Vector(98.99190627216741,321.475585168581, 0.0));
    positionAlloc4->Add (Vector(88.63690954415048,216.399853445185, 0.0));
    positionAlloc4->Add (Vector(423.3478054832489,149.0526848507362, 0.0));
    positionAlloc4->Add (Vector(293.3033449345338,144.2555126639552, 0.0));
    positionAlloc4->Add (Vector(469.9062008663341,236.46136732888957, 0.0));
    positionAlloc4->Add (Vector(442.8944509800723,273.3824775421833, 0.0));
    positionAlloc4->Add (Vector(440.58333749432,45.52085796110428, 0.0));
    positionAlloc4->Add (Vector(55.4068181914536,302.261518789088, 0.0));
    positionAlloc4->Add (Vector(348.58994255429377,48.05727718283348, 0.0));
    positionAlloc4->Add (Vector(1.2459525926797888,303.2663274067201, 0.0));
    positionAlloc4->Add (Vector(49.904863167415215,420.89465382984883, 0.0));
    positionAlloc4->Add (Vector(326.68008496262735,143.51833120334734, 0.0));
    positionAlloc4->Add (Vector(232.72934462795592,449.71360384188614, 0.0));
    positionAlloc4->Add (Vector(285.47523083219556,184.4811075685514, 0.0));
    positionAlloc4->Add (Vector(218.25752736376995,474.72009088407754, 0.0));
    positionAlloc4->Add (Vector(105.85397193441398,408.61837432347266, 0.0));
    positionAlloc4->Add (Vector(382.5426722885975,106.90569001116978, 0.0));
    positionAlloc4->Add (Vector(281.43160412976175,480.5135904282508, 0.0));
    positionAlloc4->Add (Vector(448.75451113753724,22.436384168891934, 0.0));
    positionAlloc4->Add (Vector(331.95481505547616,214.45695499620777, 0.0));
    positionAlloc4->Add (Vector(221.78682639916008,223.7619242155708, 0.0));
    positionAlloc4->Add (Vector(267.94361905870187,312.98188613609494, 0.0));
    positionAlloc4->Add (Vector(141.822667198158,467.4378436802373, 0.0));
    positionAlloc4->Add (Vector(72.61206610162513,61.15833701232526, 0.0));
    positionAlloc4->Add (Vector(33.17951165552158,335.01841464850116, 0.0));
    positionAlloc4->Add (Vector(227.27291755759072,96.28899671784386, 0.0));
    positionAlloc4->Add (Vector(234.01920053301257,239.73755819014596, 0.0));
    positionAlloc4->Add (Vector(45.268537557367985,383.818852807707, 0.0));
    positionAlloc4->Add (Vector(44.0518364045604,136.4781315121522, 0.0));
    positionAlloc4->Add (Vector(497.9660164768573,3.7719297262266416, 0.0));
    positionAlloc4->Add (Vector(491.3147004932063,97.26474946744068, 0.0));
    positionAlloc4->Add (Vector(463.8659627003217,455.121870860823, 0.0));
    positionAlloc4->Add (Vector(192.613027811323,296.22209268012705, 0.0));
    positionAlloc4->Add (Vector(403.91103917857833,262.68598599045384, 0.0));
    positionAlloc4->Add (Vector(315.87530556273305,150.53032700676562, 0.0));
    positionAlloc4->Add (Vector(56.288443073466034,88.2229821215646, 0.0));
    positionAlloc4->Add (Vector(373.5072123221906,291.66954606962383, 0.0));
    positionAlloc4->Add (Vector(278.18442243979865,459.5628323418229, 0.0));
    positionAlloc4->Add (Vector(223.78656587757905,252.83689123405162, 0.0));
    positionAlloc4->Add (Vector(310.4455949943175,241.34058390721393, 0.0));
    positionAlloc4->Add (Vector(341.9466060303499,40.75453606103635, 0.0));
    positionAlloc4->Add (Vector(256.59491166294725,226.92913211761194, 0.0));
    positionAlloc4->Add (Vector(496.3733285910971,455.93846429333973, 0.0));
    positionAlloc4->Add (Vector(31.26003658899257,211.29708026349454, 0.0));
    positionAlloc4->Add (Vector(293.8470042595159,471.8557881074334, 0.0));
    positionAlloc4->Add (Vector(16.508021282172823,176.01203399767724, 0.0));
    positionAlloc4->Add (Vector(271.501207632057,19.717859576948328, 0.0));
    positionAlloc4->Add (Vector(86.09097110630476,470.6928405591934, 0.0));
    positionAlloc4->Add (Vector(81.37318747907536,436.28838932475156, 0.0));
    positionAlloc4->Add (Vector(395.76785464562306,135.06762176279048, 0.0));
    positionAlloc4->Add (Vector(277.66243452630414,164.369379302094, 0.0));
    positionAlloc4->Add (Vector(228.8592187961298,78.6670984511329, 0.0));
    positionAlloc4->Add (Vector(262.9722465944972,280.91613405712445, 0.0));
    positionAlloc4->Add (Vector(107.42245374470532,11.788418405094092, 0.0));
    positionAlloc4->Add (Vector(6.599467977646212,277.7801478951939, 0.0));
    positionAlloc4->Add (Vector(331.87925251033727,250.9869434247877, 0.0));
    positionAlloc4->Add (Vector(477.58264533767334,7.997166728596794, 0.0));
    positionAlloc4->Add (Vector(437.5984904099793,494.6809539580833, 0.0));
    positionAlloc4->Add (Vector(200.92087539509535,288.19119952818494, 0.0));
    positionAlloc4->Add (Vector(124.95496389571753,425.0366471848626, 0.0));
    positionAlloc4->Add (Vector(83.18263250981467,90.6420984156448, 0.0));
    positionAlloc4->Add (Vector(78.51133230885787,195.93213518386122, 0.0));
    positionAlloc4->Add (Vector(321.33880035048696,234.39850955255025, 0.0));
    positionAlloc4->Add (Vector(349.5077268371953,458.92881409740943, 0.0));
    positionAlloc4->Add (Vector(90.68888448343421,278.3480077480541, 0.0));
    positionAlloc4->Add (Vector(378.6071127744969,326.1203048393311, 0.0));
    positionAlloc4->Add (Vector(485.1034732199577,366.5167180813823, 0.0));
    positionAlloc4->Add (Vector(106.27676925223534,410.1603913142805, 0.0));
    positionAlloc4->Add (Vector(257.623386676612,450.0792847519825, 0.0));
    positionAlloc4->Add (Vector(53.60280906848674,224.44433598336965, 0.0));
    positionAlloc4->Add (Vector(21.876501538053205,317.99854957487844, 0.0));
    positionAlloc4->Add (Vector(224.39442535519123,175.64510689448238, 0.0));
    positionAlloc4->Add (Vector(247.1273131392141,82.40936689774514, 0.0));
    positionAlloc4->Add (Vector(42.79605484111848,0.012780833686332205, 0.0));
    positionAlloc4->Add (Vector(304.8956421954569,47.129848429803836, 0.0));
    positionAlloc4->Add (Vector(368.583691277849,189.54182205309823, 0.0));
    positionAlloc4->Add (Vector(268.63792262222034,433.6826695087663, 0.0));
    positionAlloc4->Add (Vector(382.69758154991786,60.917181385016896, 0.0));
    positionAlloc4->Add (Vector(6.351274402270068,481.43160246456404, 0.0));
    positionAlloc4->Add (Vector(172.78499145828485,493.3157431368067, 0.0));
    positionAlloc4->Add (Vector(363.6526374997141,254.92801448899073, 0.0));
    positionAlloc4->Add (Vector(21.165369174393266,269.86099184342385, 0.0));
    positionAlloc4->Add (Vector(485.9722601579468,350.91006306541414, 0.0));
    positionAlloc4->Add (Vector(325.7952643134966,363.5186973561523, 0.0));
    positionAlloc4->Add (Vector(133.04505716820697,333.4659980732542, 0.0));
    positionAlloc4->Add (Vector(91.17642172015178,292.8400697991635, 0.0));
    positionAlloc4->Add (Vector(108.62020804157912,331.2054840205603, 0.0));
    positionAlloc4->Add (Vector(371.6608409328358,106.25613549424517, 0.0));
    positionAlloc4->Add (Vector(73.48893657461525,83.40523790930438, 0.0));
    positionAlloc4->Add (Vector(285.01914399189155,294.14986864011416, 0.0));
    positionAlloc4->Add (Vector(327.5354563575363,315.0503686307667, 0.0));
    positionAlloc4->Add (Vector(348.39832273174596,455.7364606404348, 0.0));
    positionAlloc4->Add (Vector(481.79862349073926,3.1348982685409643, 0.0));
    positionAlloc4->Add (Vector(462.7639417094684,24.262511034673363, 0.0));
    positionAlloc4->Add (Vector(411.2231083335361,429.14973291301305, 0.0));
    positionAlloc4->Add (Vector(316.6159978239791,449.62410439051405, 0.0));
    positionAlloc4->Add (Vector(93.36180597623873,206.99466310846975, 0.0));
    positionAlloc4->Add (Vector(112.2501189418012,409.24314186911533, 0.0));
    positionAlloc4->Add (Vector(280.4467256313875,203.2910570601012, 0.0));
    positionAlloc4->Add (Vector(78.70947706542908,73.3075289165252, 0.0));
    positionAlloc4->Add (Vector(275.920613710541,14.419437311226025, 0.0));
    positionAlloc4->Add (Vector(79.86874710957915,324.9895202502354, 0.0));
    positionAlloc4->Add (Vector(349.26443282914147,441.87886938156737, 0.0));
    positionAlloc4->Add (Vector(74.61031368603999,218.75479985038044, 0.0));
    positionAlloc4->Add (Vector(199.4458523454773,355.51561129743095, 0.0));
    positionAlloc4->Add (Vector(311.6133134024321,159.84151138646223, 0.0));
    positionAlloc4->Add (Vector(276.5866578373269,132.29751838859067, 0.0));
    positionAlloc4->Add (Vector(266.58522754938275,237.64420577427381, 0.0));
    positionAlloc4->Add (Vector(192.17009893969134,245.49696928533615, 0.0));
    positionAlloc4->Add (Vector(329.8114139331045,361.5893344163142, 0.0));
    positionAlloc4->Add (Vector(150.42095745937584,214.46683862564092, 0.0));
    positionAlloc4->Add (Vector(80.21923689281756,309.0672294430758, 0.0));
    positionAlloc4->Add (Vector(486.38454666718843,226.4550368119156, 0.0));
    positionAlloc4->Add (Vector(271.3805283839097,49.38762820210702, 0.0));
    positionAlloc4->Add (Vector(274.51321789642543,140.86085857181868, 0.0));
    positionAlloc4->Add (Vector(123.37678462607826,287.49313691360294, 0.0));
    positionAlloc4->Add (Vector(256.8880184271235,262.9670932743584, 0.0));
    positionAlloc4->Add (Vector(102.04997254274056,277.4359309799947, 0.0));
    positionAlloc4->Add (Vector(249.43341709799367,392.1092855049583, 0.0));
    positionAlloc4->Add (Vector(468.7034901167551,362.95683663100084, 0.0));
    positionAlloc4->Add (Vector(64.99537168778835,150.19755139078035, 0.0));
    positionAlloc4->Add (Vector(451.5031956163253,135.7138323731661, 0.0));
    positionAlloc4->Add (Vector(327.37089931609637,488.2058362027522, 0.0));
    positionAlloc4->Add (Vector(383.9654314329088,373.29323091478125, 0.0));
    positionAlloc4->Add (Vector(260.2168841846274,35.48242814493124, 0.0));
    positionAlloc4->Add (Vector(394.56382921621594,276.8207648911836, 0.0));
    positionAlloc4->Add (Vector(141.95985140683536,495.4153660128622, 0.0));
    positionAlloc4->Add (Vector(289.43017103268346,359.0702318671078, 0.0));
    positionAlloc4->Add (Vector(262.2229646037138,264.96115912887353, 0.0));
    positionAlloc4->Add (Vector(314.2841386345292,223.83702025086876, 0.0));
    positionAlloc4->Add (Vector(160.21339772197163,27.733012949442916, 0.0));
    positionAlloc4->Add (Vector(348.72408170581383,340.7338193826944, 0.0));
    positionAlloc4->Add (Vector(140.35392644570655,136.59791071050736, 0.0));
    positionAlloc4->Add (Vector(498.7719133369999,227.22316969775136, 0.0));
    positionAlloc4->Add (Vector(106.5815110257623,401.0329728791167, 0.0));
    positionAlloc4->Add (Vector(365.4642619099684,170.26054884114228, 0.0));
    positionAlloc4->Add (Vector(90.1072483946525,13.462993444583681, 0.0));
    positionAlloc4->Add (Vector(39.90668600469699,315.7370125971946, 0.0));
    positionAlloc4->Add (Vector(237.40869952059452,492.9775438336197, 0.0));
    positionAlloc4->Add (Vector(476.7883202293363,321.1213261405397, 0.0));
    positionAlloc4->Add (Vector(216.94350754154036,256.942248640749, 0.0));
    positionAlloc4->Add (Vector(139.90035828571297,445.59998525530784, 0.0));
    positionAlloc4->Add (Vector(22.964297090124152,363.99330467421225, 0.0));
    positionAlloc4->Add (Vector(36.83387353203166,3.361463654779351, 0.0));
    positionAlloc4->Add (Vector(360.4857966511324,190.9431619969954, 0.0));
    positionAlloc4->Add (Vector(251.69383579099352,480.3209684330543, 0.0));
    positionAlloc4->Add (Vector(399.8801971535045,344.42483958709477, 0.0));
    positionAlloc4->Add (Vector(9.668810991397214,485.8313818050456, 0.0));
    positionAlloc4->Add (Vector(116.50820044975634,159.85760606438882, 0.0));
    positionAlloc4->Add (Vector(443.59935652749886,408.10218994875333, 0.0));
    positionAlloc4->Add (Vector(186.16284323938353,424.99703465168625, 0.0));
    positionAlloc4->Add (Vector(222.02905552920083,204.0332946821994, 0.0));
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
	  flowmon->SerializeToXmlFile ("scratch/switch_SA_flowmon/switch_SA12.flowmon", false, false);
	 //FIM FLOW MONITOR
		Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (serverApps.Get (0));
		std::cout << "Bytes received by server 1: " << sink1->GetTotalRx () << " ("
							<< (8. * sink1->GetTotalRx ()) / 1000000 / simTime << " Mbps)"
							<< std::endl;
	 return 0;
	}