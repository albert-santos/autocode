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
	  uint16_t numberOfNodes = 259;
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
    positionAlloc->Add (Vector (333.3333333333333,500.0, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,0.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,500.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,0.0, 0.0));
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
    positionAlloc4->Add (Vector(317.56455015337644,467.05447522022075, 0.0));
    positionAlloc4->Add (Vector(287.44541619596026,151.54354200978298, 0.0));
    positionAlloc4->Add (Vector(463.1766159857117,275.04566953066364, 0.0));
    positionAlloc4->Add (Vector(135.29568449862035,485.6517236146126, 0.0));
    positionAlloc4->Add (Vector(199.9653715449874,459.5369461826691, 0.0));
    positionAlloc4->Add (Vector(281.1350673634821,270.58165688838153, 0.0));
    positionAlloc4->Add (Vector(460.9251657631288,223.556843743123, 0.0));
    positionAlloc4->Add (Vector(431.59449697230616,407.79788805373835, 0.0));
    positionAlloc4->Add (Vector(405.64857958957117,238.70566711435427, 0.0));
    positionAlloc4->Add (Vector(292.539860102583,419.01897605941025, 0.0));
    positionAlloc4->Add (Vector(141.3308292930684,303.4106176100245, 0.0));
    positionAlloc4->Add (Vector(361.7007242504504,80.4499520105697, 0.0));
    positionAlloc4->Add (Vector(106.50945872399292,0.8344447713052316, 0.0));
    positionAlloc4->Add (Vector(423.3092047959466,278.4259749211966, 0.0));
    positionAlloc4->Add (Vector(240.20780719941615,97.88595516040182, 0.0));
    positionAlloc4->Add (Vector(389.03669079794054,96.98840430839417, 0.0));
    positionAlloc4->Add (Vector(195.96090331634204,211.6886730698191, 0.0));
    positionAlloc4->Add (Vector(424.50719065359255,155.22541231965343, 0.0));
    positionAlloc4->Add (Vector(55.292384295186906,438.8964763600226, 0.0));
    positionAlloc4->Add (Vector(140.58393511637246,70.19002761408576, 0.0));
    positionAlloc4->Add (Vector(337.9281808963954,56.97911869695721, 0.0));
    positionAlloc4->Add (Vector(153.70414507998754,122.22934926592404, 0.0));
    positionAlloc4->Add (Vector(65.78350390532012,393.8351857330507, 0.0));
    positionAlloc4->Add (Vector(461.0492462007271,414.4002807056223, 0.0));
    positionAlloc4->Add (Vector(486.7503196387758,146.70684465119544, 0.0));
    positionAlloc4->Add (Vector(215.77175974952416,479.55388453734435, 0.0));
    positionAlloc4->Add (Vector(115.42571009628416,265.47542541693343, 0.0));
    positionAlloc4->Add (Vector(68.15965431733045,417.7470411271551, 0.0));
    positionAlloc4->Add (Vector(135.80880753445317,185.78137254705084, 0.0));
    positionAlloc4->Add (Vector(194.1723728017508,299.9707695722194, 0.0));
    positionAlloc4->Add (Vector(265.8625351587488,384.48779898516517, 0.0));
    positionAlloc4->Add (Vector(496.38977248145017,110.0625150586012, 0.0));
    positionAlloc4->Add (Vector(96.60983736410734,233.1599879046884, 0.0));
    positionAlloc4->Add (Vector(204.39631693647704,471.5950893361803, 0.0));
    positionAlloc4->Add (Vector(86.60523824588506,376.0583370085738, 0.0));
    positionAlloc4->Add (Vector(238.93708306347517,423.40210587552895, 0.0));
    positionAlloc4->Add (Vector(212.20059825899133,485.7769380754745, 0.0));
    positionAlloc4->Add (Vector(138.8354867430161,72.40352891169383, 0.0));
    positionAlloc4->Add (Vector(116.3241773736159,431.39515512817104, 0.0));
    positionAlloc4->Add (Vector(9.058994771865503,75.83948992891892, 0.0));
    positionAlloc4->Add (Vector(123.50989495478815,285.7716989854771, 0.0));
    positionAlloc4->Add (Vector(335.2288438891521,126.15173283168652, 0.0));
    positionAlloc4->Add (Vector(259.74862587451287,82.6675856931362, 0.0));
    positionAlloc4->Add (Vector(272.6544248035929,131.9894249215821, 0.0));
    positionAlloc4->Add (Vector(100.24287047725889,118.87050999754788, 0.0));
    positionAlloc4->Add (Vector(380.69391039582973,443.43310515971655, 0.0));
    positionAlloc4->Add (Vector(340.3886695668126,187.3113132990069, 0.0));
    positionAlloc4->Add (Vector(252.95422919590877,89.07795538416514, 0.0));
    positionAlloc4->Add (Vector(213.64814463379184,113.47132295903539, 0.0));
    positionAlloc4->Add (Vector(450.1464840669372,0.8289396536135807, 0.0));
    positionAlloc4->Add (Vector(263.3658167453627,252.0491800918219, 0.0));
    positionAlloc4->Add (Vector(475.37658686098814,430.2247253625899, 0.0));
    positionAlloc4->Add (Vector(108.73906333523087,154.95683646853908, 0.0));
    positionAlloc4->Add (Vector(221.0362922245161,60.31520991969569, 0.0));
    positionAlloc4->Add (Vector(445.6018729164849,58.91258733115839, 0.0));
    positionAlloc4->Add (Vector(427.8112206658954,380.41866986177644, 0.0));
    positionAlloc4->Add (Vector(61.009248233667435,21.046362956257724, 0.0));
    positionAlloc4->Add (Vector(385.46240514965336,145.7944158111818, 0.0));
    positionAlloc4->Add (Vector(498.59822005283013,72.96596814228856, 0.0));
    positionAlloc4->Add (Vector(424.8502604292261,82.95337860915208, 0.0));
    positionAlloc4->Add (Vector(95.7169175938255,423.3606738297488, 0.0));
    positionAlloc4->Add (Vector(466.456896057886,429.90176991071013, 0.0));
    positionAlloc4->Add (Vector(465.44342284861466,8.050416645377645, 0.0));
    positionAlloc4->Add (Vector(376.86796101750866,402.68746636807236, 0.0));
    positionAlloc4->Add (Vector(257.4517368867942,199.1830101167824, 0.0));
    positionAlloc4->Add (Vector(66.78943850897751,113.01473156635939, 0.0));
    positionAlloc4->Add (Vector(388.67955441975994,488.63410946736985, 0.0));
    positionAlloc4->Add (Vector(270.46456095585984,475.0495011544734, 0.0));
    positionAlloc4->Add (Vector(147.57436424434144,76.81646941075704, 0.0));
    positionAlloc4->Add (Vector(479.60902571440863,308.4888363594084, 0.0));
    positionAlloc4->Add (Vector(409.53587572002584,241.03816948290267, 0.0));
    positionAlloc4->Add (Vector(406.9839630477985,9.246827759441967, 0.0));
    positionAlloc4->Add (Vector(110.31617691714906,315.7499537686957, 0.0));
    positionAlloc4->Add (Vector(45.89798475177304,184.27180472557225, 0.0));
    positionAlloc4->Add (Vector(383.6907173354487,208.10901203454983, 0.0));
    positionAlloc4->Add (Vector(395.31844706496133,416.05579130327675, 0.0));
    positionAlloc4->Add (Vector(160.43473805783148,28.597133742115922, 0.0));
    positionAlloc4->Add (Vector(404.10889508392927,178.21083335934622, 0.0));
    positionAlloc4->Add (Vector(5.817063479183426,479.31299349764544, 0.0));
    positionAlloc4->Add (Vector(41.546810921189945,195.44011480269629, 0.0));
    positionAlloc4->Add (Vector(199.27835408502048,240.99758038778063, 0.0));
    positionAlloc4->Add (Vector(107.90380875961347,297.5771575772169, 0.0));
    positionAlloc4->Add (Vector(431.25795148044307,114.34628382198197, 0.0));
    positionAlloc4->Add (Vector(19.40594135005025,291.4590992779091, 0.0));
    positionAlloc4->Add (Vector(17.718004286624065,224.43526271762843, 0.0));
    positionAlloc4->Add (Vector(45.573577481657026,305.27793833900176, 0.0));
    positionAlloc4->Add (Vector(443.8324243683152,26.537242334177147, 0.0));
    positionAlloc4->Add (Vector(391.8066209592345,114.45240156676229, 0.0));
    positionAlloc4->Add (Vector(80.74056004429065,44.627610479494265, 0.0));
    positionAlloc4->Add (Vector(419.0561842488243,365.4919839333847, 0.0));
    positionAlloc4->Add (Vector(134.40590473932724,36.35095435810282, 0.0));
    positionAlloc4->Add (Vector(178.1384871558311,60.01659428481576, 0.0));
    positionAlloc4->Add (Vector(426.2086628340851,192.65118534908078, 0.0));
    positionAlloc4->Add (Vector(408.6030538994571,343.8827729506282, 0.0));
    positionAlloc4->Add (Vector(283.43438207900454,19.59531152991817, 0.0));
    positionAlloc4->Add (Vector(82.43601906501897,363.2993217163773, 0.0));
    positionAlloc4->Add (Vector(21.852524552036677,144.6237861174019, 0.0));
    positionAlloc4->Add (Vector(342.5352440387144,308.6969562707632, 0.0));
    positionAlloc4->Add (Vector(273.85545133998573,273.2057360015153, 0.0));
    positionAlloc4->Add (Vector(437.49253913232644,38.363592054504736, 0.0));
    positionAlloc4->Add (Vector(236.9515651680793,223.5678410929975, 0.0));
    positionAlloc4->Add (Vector(257.55215586246044,454.05060594585626, 0.0));
    positionAlloc4->Add (Vector(343.6846165742555,207.48754706942873, 0.0));
    positionAlloc4->Add (Vector(424.2458978998293,443.798489080966, 0.0));
    positionAlloc4->Add (Vector(122.09351697855352,287.41184407387925, 0.0));
    positionAlloc4->Add (Vector(464.3545301436212,99.43597690435773, 0.0));
    positionAlloc4->Add (Vector(498.0984945178928,140.38416864094287, 0.0));
    positionAlloc4->Add (Vector(117.38841884914758,455.0734699622118, 0.0));
    positionAlloc4->Add (Vector(247.56407682199512,180.24733996932963, 0.0));
    positionAlloc4->Add (Vector(342.22387598978486,159.67928033471568, 0.0));
    positionAlloc4->Add (Vector(449.9857491170728,89.84032982589673, 0.0));
    positionAlloc4->Add (Vector(91.68662465644861,286.5643174211839, 0.0));
    positionAlloc4->Add (Vector(149.72034184473048,91.7287814318291, 0.0));
    positionAlloc4->Add (Vector(276.3218807035153,404.49757431134, 0.0));
    positionAlloc4->Add (Vector(393.83337392102607,217.11745641263892, 0.0));
    positionAlloc4->Add (Vector(61.270870151997336,345.59785975617245, 0.0));
    positionAlloc4->Add (Vector(472.6877948854274,461.43768965347516, 0.0));
    positionAlloc4->Add (Vector(326.0565640098617,398.6023446118517, 0.0));
    positionAlloc4->Add (Vector(334.6865723760877,36.17835974341593, 0.0));
    positionAlloc4->Add (Vector(348.45702165594486,304.42995898314837, 0.0));
    positionAlloc4->Add (Vector(392.8393215173719,441.8395536932465, 0.0));
    positionAlloc4->Add (Vector(255.66856727753233,442.6185616284864, 0.0));
    positionAlloc4->Add (Vector(24.556966487752018,78.15700008513038, 0.0));
    positionAlloc4->Add (Vector(387.75334760296215,322.52473172599605, 0.0));
    positionAlloc4->Add (Vector(98.48007805262881,21.16765965727646, 0.0));
    positionAlloc4->Add (Vector(468.94107994542134,414.21992883182986, 0.0));
    positionAlloc4->Add (Vector(305.46423342364596,452.0605942676322, 0.0));
    positionAlloc4->Add (Vector(232.55956402553585,88.36509468940423, 0.0));
    positionAlloc4->Add (Vector(190.44168661027823,325.70067188296036, 0.0));
    positionAlloc4->Add (Vector(285.9164592662018,178.32566096084568, 0.0));
    positionAlloc4->Add (Vector(20.411915607358832,106.23841921498628, 0.0));
    positionAlloc4->Add (Vector(357.2175639037989,121.80551149670654, 0.0));
    positionAlloc4->Add (Vector(478.92909959188484,154.19117834074027, 0.0));
    positionAlloc4->Add (Vector(346.13754821735347,44.13211477140899, 0.0));
    positionAlloc4->Add (Vector(130.7615342099641,387.17657238715867, 0.0));
    positionAlloc4->Add (Vector(280.23780352605553,115.00556593767752, 0.0));
    positionAlloc4->Add (Vector(11.09404116740198,198.79673685677994, 0.0));
    positionAlloc4->Add (Vector(91.60696716985345,319.7200980494877, 0.0));
    positionAlloc4->Add (Vector(122.47685169626976,111.67476247325031, 0.0));
    positionAlloc4->Add (Vector(320.0834206288096,425.1968330016348, 0.0));
    positionAlloc4->Add (Vector(360.3310604637667,156.6619807224511, 0.0));
    positionAlloc4->Add (Vector(105.34651222805303,21.76260598889146, 0.0));
    positionAlloc4->Add (Vector(254.20715552488466,238.04120437400377, 0.0));
    positionAlloc4->Add (Vector(466.1975429632746,201.29814154530973, 0.0));
    positionAlloc4->Add (Vector(445.48228085236184,194.28196164807855, 0.0));
    positionAlloc4->Add (Vector(2.1953129239616342,413.4515775208668, 0.0));
    positionAlloc4->Add (Vector(143.41086972531548,238.71665558731826, 0.0));
    positionAlloc4->Add (Vector(153.92671983569784,43.51282915006449, 0.0));
    positionAlloc4->Add (Vector(305.0220696255652,451.13367770741564, 0.0));
    positionAlloc4->Add (Vector(278.3344961242995,438.509734635001, 0.0));
    positionAlloc4->Add (Vector(226.9559531531233,18.022536219739983, 0.0));
    positionAlloc4->Add (Vector(377.9564507347947,76.48103891793157, 0.0));
    positionAlloc4->Add (Vector(21.4496512574765,324.8269602086682, 0.0));
    positionAlloc4->Add (Vector(108.36651198469599,374.18349992078697, 0.0));
    positionAlloc4->Add (Vector(293.41345456743886,191.01015049960364, 0.0));
    positionAlloc4->Add (Vector(65.27204277719967,108.98814980256888, 0.0));
    positionAlloc4->Add (Vector(290.25407971410874,54.84684296179398, 0.0));
    positionAlloc4->Add (Vector(111.38361026714777,406.1381376741261, 0.0));
    positionAlloc4->Add (Vector(224.81252731354877,58.16866753611849, 0.0));
    positionAlloc4->Add (Vector(279.64108090265177,113.34859636206096, 0.0));
    positionAlloc4->Add (Vector(182.97662704587557,218.16552302688524, 0.0));
    positionAlloc4->Add (Vector(188.68668550980172,202.2409351100204, 0.0));
    positionAlloc4->Add (Vector(413.1692272771456,148.16018489209853, 0.0));
    positionAlloc4->Add (Vector(140.34169789040706,0.37074248126084397, 0.0));
    positionAlloc4->Add (Vector(18.460541410858955,137.7797393296248, 0.0));
    positionAlloc4->Add (Vector(190.08788234753865,243.72814264077957, 0.0));
    positionAlloc4->Add (Vector(48.41871305252931,175.2516484984077, 0.0));
    positionAlloc4->Add (Vector(316.601081332052,112.20277780238308, 0.0));
    positionAlloc4->Add (Vector(397.8802994797551,54.314018170836086, 0.0));
    positionAlloc4->Add (Vector(484.8982836868051,22.473626877886566, 0.0));
    positionAlloc4->Add (Vector(171.23532891687,338.6742317641982, 0.0));
    positionAlloc4->Add (Vector(117.81567627311928,283.50432083380616, 0.0));
    positionAlloc4->Add (Vector(377.90035883274214,130.59446418616733, 0.0));
    positionAlloc4->Add (Vector(38.45300284316933,240.26674969033834, 0.0));
    positionAlloc4->Add (Vector(41.239300027462455,391.05470373592675, 0.0));
    positionAlloc4->Add (Vector(335.05148775497406,409.47306766908065, 0.0));
    positionAlloc4->Add (Vector(316.04513297299945,242.45370811626583, 0.0));
    positionAlloc4->Add (Vector(234.38360866666753,382.2564548983328, 0.0));
    positionAlloc4->Add (Vector(154.21214642140419,39.49156348672989, 0.0));
    positionAlloc4->Add (Vector(498.4976265115452,359.2833861354634, 0.0));
    positionAlloc4->Add (Vector(388.33739698800883,156.41819499600084, 0.0));
    positionAlloc4->Add (Vector(463.11140959995055,486.33548736834973, 0.0));
    positionAlloc4->Add (Vector(299.9473925460949,32.411853174876214, 0.0));
    positionAlloc4->Add (Vector(36.55366480105482,205.67882857877723, 0.0));
    positionAlloc4->Add (Vector(58.865800473036636,330.2903268978251, 0.0));
    positionAlloc4->Add (Vector(358.1566050763416,437.55947324632893, 0.0));
    positionAlloc4->Add (Vector(250.88936457157857,174.27649310125776, 0.0));
    positionAlloc4->Add (Vector(319.13563361131537,263.9196107674472, 0.0));
    positionAlloc4->Add (Vector(142.5151684467969,325.72591185841276, 0.0));
    positionAlloc4->Add (Vector(363.7488488100238,7.3973966762502785, 0.0));
    positionAlloc4->Add (Vector(153.76052728410022,277.166749692788, 0.0));
    positionAlloc4->Add (Vector(211.5302768406995,316.76192164594, 0.0));
    positionAlloc4->Add (Vector(311.7527606008702,80.52956459404187, 0.0));
    positionAlloc4->Add (Vector(325.3088670672441,155.9289893262138, 0.0));
    positionAlloc4->Add (Vector(272.2517603692276,483.5739922507366, 0.0));
    positionAlloc4->Add (Vector(431.54749741318534,162.79866884588301, 0.0));
    positionAlloc4->Add (Vector(380.36822621749064,282.3432204382118, 0.0));
    positionAlloc4->Add (Vector(479.51303316955665,417.02422531852847, 0.0));
    positionAlloc4->Add (Vector(195.20449743512265,387.3803555459422, 0.0));
    positionAlloc4->Add (Vector(347.0688293437178,21.11115284108267, 0.0));
    positionAlloc4->Add (Vector(105.11231911186836,70.10342616386129, 0.0));
    positionAlloc4->Add (Vector(369.7071321567377,0.6230211547822995, 0.0));
    positionAlloc4->Add (Vector(144.56570405515535,311.410553234055, 0.0));
    positionAlloc4->Add (Vector(228.43897349008614,127.61110164877154, 0.0));
    positionAlloc4->Add (Vector(304.01355834151826,312.09717793800286, 0.0));
    positionAlloc4->Add (Vector(344.87107636841824,356.83969392260315, 0.0));
    positionAlloc4->Add (Vector(257.5964585831086,133.34226544084927, 0.0));
    positionAlloc4->Add (Vector(81.62526460250491,169.36913036735345, 0.0));
    positionAlloc4->Add (Vector(240.62156812541647,32.76992786290983, 0.0));
    positionAlloc4->Add (Vector(90.38113690817545,372.5139818662363, 0.0));
    positionAlloc4->Add (Vector(72.83433537492645,119.88593459553998, 0.0));
    positionAlloc4->Add (Vector(473.7115583228937,478.9892856613423, 0.0));
    positionAlloc4->Add (Vector(288.63002531901196,192.25680580187014, 0.0));
    positionAlloc4->Add (Vector(220.6145941022813,460.5054184626336, 0.0));
    positionAlloc4->Add (Vector(398.6150067972159,341.30828435712385, 0.0));
    positionAlloc4->Add (Vector(131.91001431108774,246.9319637666379, 0.0));
    positionAlloc4->Add (Vector(447.44812134156285,20.83460105591567, 0.0));
    positionAlloc4->Add (Vector(16.928136861672993,316.0702178032236, 0.0));
    positionAlloc4->Add (Vector(442.8270431429201,192.17090099205697, 0.0));
    positionAlloc4->Add (Vector(328.4503201834474,168.13912281484096, 0.0));
    positionAlloc4->Add (Vector(351.6670988918134,323.0804666223644, 0.0));
    positionAlloc4->Add (Vector(264.3338473996963,158.9125704715359, 0.0));
    positionAlloc4->Add (Vector(243.3883633665687,124.80779555008104, 0.0));
    positionAlloc4->Add (Vector(151.4689527907172,223.58440954936754, 0.0));
    positionAlloc4->Add (Vector(374.3217166033612,282.3380440830362, 0.0));
    positionAlloc4->Add (Vector(158.21728322732815,48.994945999762784, 0.0));
    positionAlloc4->Add (Vector(284.09520413299964,38.91736873078738, 0.0));
    positionAlloc4->Add (Vector(233.12693229387,266.95351030422876, 0.0));
    positionAlloc4->Add (Vector(346.25916100814624,0.7184704123017749, 0.0));
    positionAlloc4->Add (Vector(197.67562596098327,135.66307462485943, 0.0));
    positionAlloc4->Add (Vector(350.5718271680408,208.22420099990035, 0.0));
    positionAlloc4->Add (Vector(367.88151751001914,235.94843008586847, 0.0));
    positionAlloc4->Add (Vector(201.78946230110327,180.09323104908802, 0.0));
    positionAlloc4->Add (Vector(419.9014694826809,448.98236402153725, 0.0));
    positionAlloc4->Add (Vector(429.6759744177754,215.97862518729104, 0.0));
    positionAlloc4->Add (Vector(48.713801194807836,146.86280651848605, 0.0));
    positionAlloc4->Add (Vector(264.35682575752236,26.116378801680774, 0.0));
    positionAlloc4->Add (Vector(91.6653190454656,409.825581882739, 0.0));
    positionAlloc4->Add (Vector(15.763126940004035,191.2853998069596, 0.0));
    positionAlloc4->Add (Vector(254.7725198535814,471.0968408026151, 0.0));
    positionAlloc4->Add (Vector(328.9713383094896,283.92602417254636, 0.0));
    positionAlloc4->Add (Vector(290.72167237650103,224.7158186074512, 0.0));
    positionAlloc4->Add (Vector(421.14677063746313,468.80777409148726, 0.0));
    positionAlloc4->Add (Vector(438.2141737318739,99.36847157283391, 0.0));
    positionAlloc4->Add (Vector(32.439105472303666,190.65161990670254, 0.0));
    positionAlloc4->Add (Vector(228.2494962787671,247.63759963647564, 0.0));
    positionAlloc4->Add (Vector(0.35575725981767947,382.2618370851071, 0.0));
    positionAlloc4->Add (Vector(205.49619688402925,55.21401182523456, 0.0));
    positionAlloc4->Add (Vector(202.99926971683567,4.430195729548947, 0.0));
    positionAlloc4->Add (Vector(328.7843513692827,488.1680477064925, 0.0));
    positionAlloc4->Add (Vector(47.943321566206286,484.84011212765444, 0.0));
    positionAlloc4->Add (Vector(87.77106099924103,321.9557325232044, 0.0));
    positionAlloc4->Add (Vector(474.4569681665739,441.1173932364191, 0.0));
    positionAlloc4->Add (Vector(361.29358964412125,325.9145914966199, 0.0));
    positionAlloc4->Add (Vector(339.52302547896454,96.44951417952868, 0.0));
    positionAlloc4->Add (Vector(108.14634250719529,173.09121720582777, 0.0));
    positionAlloc4->Add (Vector(431.9581937813553,455.0634228112678, 0.0));
    positionAlloc4->Add (Vector(411.7625598050923,168.7265350408258, 0.0));
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
	  flowmon->SerializeToXmlFile ("scratch/switch_SA_flowmon/switch_SA21.flowmon", false, false);
	 //FIM FLOW MONITOR
		Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (serverApps.Get (0));
		std::cout << "Bytes received by server 1: " << sink1->GetTotalRx () << " ("
							<< (8. * sink1->GetTotalRx ()) / 1000000 / simTime << " Mbps)"
							<< std::endl;
	 return 0;
	}