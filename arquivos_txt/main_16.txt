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
	  uint16_t numberOfRrhs = 5;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 219;
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
    positionAlloc->Add (Vector (333.3333333333333,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,166.66666666666666, 0.0));
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
    positionAlloc4->Add (Vector(185.3538768986726,38.90511449841716, 0.0));
    positionAlloc4->Add (Vector(47.53783197797695,446.0779686706011, 0.0));
    positionAlloc4->Add (Vector(336.8503079711746,29.347646735637277, 0.0));
    positionAlloc4->Add (Vector(351.7448441755201,480.4059544683813, 0.0));
    positionAlloc4->Add (Vector(318.42665211809145,152.20768511351272, 0.0));
    positionAlloc4->Add (Vector(210.11260366568334,201.76121006598314, 0.0));
    positionAlloc4->Add (Vector(123.22913237129573,149.6021295295783, 0.0));
    positionAlloc4->Add (Vector(433.4936728145892,426.9491785915132, 0.0));
    positionAlloc4->Add (Vector(44.06008555315643,59.833597544929226, 0.0));
    positionAlloc4->Add (Vector(429.76420547656096,60.36513644185554, 0.0));
    positionAlloc4->Add (Vector(117.98564796781041,57.287073816434386, 0.0));
    positionAlloc4->Add (Vector(373.93175454027454,306.2147094222553, 0.0));
    positionAlloc4->Add (Vector(27.8785684044322,185.37476976923907, 0.0));
    positionAlloc4->Add (Vector(96.36386440932048,147.5797442083588, 0.0));
    positionAlloc4->Add (Vector(138.70127592553328,139.89922780187058, 0.0));
    positionAlloc4->Add (Vector(228.08297126853222,239.76712603184052, 0.0));
    positionAlloc4->Add (Vector(356.1866211010154,419.2615458967892, 0.0));
    positionAlloc4->Add (Vector(166.9403797055427,410.83672821785217, 0.0));
    positionAlloc4->Add (Vector(161.9105193184248,10.265144963759854, 0.0));
    positionAlloc4->Add (Vector(280.9973842218323,481.79645631345545, 0.0));
    positionAlloc4->Add (Vector(230.83280856495324,270.43953958814, 0.0));
    positionAlloc4->Add (Vector(285.49188334887907,207.5075533708564, 0.0));
    positionAlloc4->Add (Vector(423.82836238124787,223.36246550352735, 0.0));
    positionAlloc4->Add (Vector(386.8296680227145,293.21292483470774, 0.0));
    positionAlloc4->Add (Vector(480.9947378960911,399.5686729355167, 0.0));
    positionAlloc4->Add (Vector(481.74569665406733,295.8775215622745, 0.0));
    positionAlloc4->Add (Vector(238.39896696076417,428.7960420730814, 0.0));
    positionAlloc4->Add (Vector(170.3852766397188,409.62862980156353, 0.0));
    positionAlloc4->Add (Vector(88.02648540445989,121.87300351323555, 0.0));
    positionAlloc4->Add (Vector(183.37989439814496,48.514350093280036, 0.0));
    positionAlloc4->Add (Vector(371.7619666699945,455.96199911283566, 0.0));
    positionAlloc4->Add (Vector(487.23012292355224,293.1535245925153, 0.0));
    positionAlloc4->Add (Vector(428.10876805471355,160.49874064069593, 0.0));
    positionAlloc4->Add (Vector(147.78998130664877,478.37371549451115, 0.0));
    positionAlloc4->Add (Vector(312.9736296811247,337.74780983779226, 0.0));
    positionAlloc4->Add (Vector(78.939742430719,188.81925065360454, 0.0));
    positionAlloc4->Add (Vector(96.95820508876895,346.72000723401885, 0.0));
    positionAlloc4->Add (Vector(79.1811823148118,160.9781560717786, 0.0));
    positionAlloc4->Add (Vector(239.70279680488622,190.71880715292056, 0.0));
    positionAlloc4->Add (Vector(62.15700770337135,342.65326684297673, 0.0));
    positionAlloc4->Add (Vector(133.30475691058152,148.60643203732022, 0.0));
    positionAlloc4->Add (Vector(282.6532803597994,316.050456599376, 0.0));
    positionAlloc4->Add (Vector(221.0959112913783,83.3052636894494, 0.0));
    positionAlloc4->Add (Vector(335.0753974555657,434.4115578641192, 0.0));
    positionAlloc4->Add (Vector(73.280504763103,438.58020251188833, 0.0));
    positionAlloc4->Add (Vector(389.0813204823434,245.9063269151483, 0.0));
    positionAlloc4->Add (Vector(35.449701294316185,88.47397687051523, 0.0));
    positionAlloc4->Add (Vector(91.38360555513908,399.0804315140187, 0.0));
    positionAlloc4->Add (Vector(150.87721041048673,14.845423370679068, 0.0));
    positionAlloc4->Add (Vector(79.90625430371156,372.65753960688517, 0.0));
    positionAlloc4->Add (Vector(471.06126179049016,496.3324310663347, 0.0));
    positionAlloc4->Add (Vector(255.0705475449986,412.8300271982002, 0.0));
    positionAlloc4->Add (Vector(45.30088760415718,494.9404094856171, 0.0));
    positionAlloc4->Add (Vector(275.13905137244524,124.09391269675824, 0.0));
    positionAlloc4->Add (Vector(202.27818026720863,276.51452163412716, 0.0));
    positionAlloc4->Add (Vector(390.82368352421247,447.03893448700774, 0.0));
    positionAlloc4->Add (Vector(136.2224275786113,81.45821997396129, 0.0));
    positionAlloc4->Add (Vector(492.3381414410264,208.79250552697798, 0.0));
    positionAlloc4->Add (Vector(174.42809464215492,494.5162685513124, 0.0));
    positionAlloc4->Add (Vector(253.49688834112678,125.1244752251534, 0.0));
    positionAlloc4->Add (Vector(213.35066796037938,262.83282370984637, 0.0));
    positionAlloc4->Add (Vector(250.07752293625342,377.51722547478784, 0.0));
    positionAlloc4->Add (Vector(196.58749470189213,195.05145621960008, 0.0));
    positionAlloc4->Add (Vector(321.26966695385636,178.8198585671989, 0.0));
    positionAlloc4->Add (Vector(417.4814389122013,488.8234535434577, 0.0));
    positionAlloc4->Add (Vector(429.28856283088896,268.67188125002957, 0.0));
    positionAlloc4->Add (Vector(84.90545068665256,101.15552177339254, 0.0));
    positionAlloc4->Add (Vector(128.5476827800449,38.89509741736485, 0.0));
    positionAlloc4->Add (Vector(256.57378159235554,97.15836356261931, 0.0));
    positionAlloc4->Add (Vector(264.76037274660985,127.04441684301099, 0.0));
    positionAlloc4->Add (Vector(129.3679293427037,332.5579011872567, 0.0));
    positionAlloc4->Add (Vector(317.36561821356156,93.26592821670887, 0.0));
    positionAlloc4->Add (Vector(433.0569713673675,99.47257940931459, 0.0));
    positionAlloc4->Add (Vector(252.83335731925183,479.06580383827963, 0.0));
    positionAlloc4->Add (Vector(365.1975961802807,451.67225851944016, 0.0));
    positionAlloc4->Add (Vector(51.00543947783343,248.37158929155544, 0.0));
    positionAlloc4->Add (Vector(72.89023795071398,189.5577138460911, 0.0));
    positionAlloc4->Add (Vector(155.70586391245234,333.4631859992202, 0.0));
    positionAlloc4->Add (Vector(228.17296654241014,458.75779230346416, 0.0));
    positionAlloc4->Add (Vector(18.40001708056699,260.006774451691, 0.0));
    positionAlloc4->Add (Vector(187.6123173045034,199.57472838319208, 0.0));
    positionAlloc4->Add (Vector(284.87540682126513,414.8889508995886, 0.0));
    positionAlloc4->Add (Vector(294.9864619277563,494.04027739617766, 0.0));
    positionAlloc4->Add (Vector(466.36679504168336,367.4730199499775, 0.0));
    positionAlloc4->Add (Vector(258.17838880560294,468.8041762227403, 0.0));
    positionAlloc4->Add (Vector(37.33796992609217,415.8761788573418, 0.0));
    positionAlloc4->Add (Vector(449.16267425245803,30.03824272357819, 0.0));
    positionAlloc4->Add (Vector(331.78161225081766,159.8146645980923, 0.0));
    positionAlloc4->Add (Vector(358.20183234882415,129.92295909314393, 0.0));
    positionAlloc4->Add (Vector(93.25819929027173,463.98682900267676, 0.0));
    positionAlloc4->Add (Vector(497.92344543054156,381.98540994730547, 0.0));
    positionAlloc4->Add (Vector(1.7734724369548038,323.98292059746404, 0.0));
    positionAlloc4->Add (Vector(152.86242303552422,185.68578077749663, 0.0));
    positionAlloc4->Add (Vector(310.2305671752303,301.9712810861802, 0.0));
    positionAlloc4->Add (Vector(254.09001404397912,197.34252786728212, 0.0));
    positionAlloc4->Add (Vector(488.08035578009094,196.40612782394746, 0.0));
    positionAlloc4->Add (Vector(210.0044294566867,131.20506949812443, 0.0));
    positionAlloc4->Add (Vector(406.29674493056916,251.15885917295722, 0.0));
    positionAlloc4->Add (Vector(328.91030306583934,155.09320363357833, 0.0));
    positionAlloc4->Add (Vector(437.18876104481,432.3488218771693, 0.0));
    positionAlloc4->Add (Vector(492.5015809342972,193.212878200461, 0.0));
    positionAlloc4->Add (Vector(329.6586507014205,170.62875299209546, 0.0));
    positionAlloc4->Add (Vector(36.87932971592339,355.86403837018713, 0.0));
    positionAlloc4->Add (Vector(255.82843839155794,195.9489418258785, 0.0));
    positionAlloc4->Add (Vector(257.1125742689711,297.1293826838573, 0.0));
    positionAlloc4->Add (Vector(455.1265900045717,270.22023137484774, 0.0));
    positionAlloc4->Add (Vector(403.7015729960571,404.72042924803003, 0.0));
    positionAlloc4->Add (Vector(388.5353135966744,32.68493056223165, 0.0));
    positionAlloc4->Add (Vector(109.83185256037432,263.9041882281894, 0.0));
    positionAlloc4->Add (Vector(465.0039308330387,12.64379949297334, 0.0));
    positionAlloc4->Add (Vector(169.8194240977302,335.2994913587088, 0.0));
    positionAlloc4->Add (Vector(4.089142163589354,126.97852564807799, 0.0));
    positionAlloc4->Add (Vector(125.46359258649814,146.59218240264343, 0.0));
    positionAlloc4->Add (Vector(120.70167797506309,99.48797962792183, 0.0));
    positionAlloc4->Add (Vector(425.3712967005304,436.46922542990046, 0.0));
    positionAlloc4->Add (Vector(199.11622649816368,152.07326915232926, 0.0));
    positionAlloc4->Add (Vector(225.80979638324055,67.7583308281301, 0.0));
    positionAlloc4->Add (Vector(9.328289570064818,46.108221810631676, 0.0));
    positionAlloc4->Add (Vector(16.954776626544987,302.59995198889015, 0.0));
    positionAlloc4->Add (Vector(321.2819435356345,141.90410918177005, 0.0));
    positionAlloc4->Add (Vector(383.8486465817266,259.745685288609, 0.0));
    positionAlloc4->Add (Vector(311.32756495901407,65.94280976837268, 0.0));
    positionAlloc4->Add (Vector(320.6915535676086,435.8278139487946, 0.0));
    positionAlloc4->Add (Vector(250.67661039456817,355.2815993111341, 0.0));
    positionAlloc4->Add (Vector(374.9074040470894,290.2413036263668, 0.0));
    positionAlloc4->Add (Vector(477.25862708662515,498.30193178041446, 0.0));
    positionAlloc4->Add (Vector(406.5922764395694,424.6239725700504, 0.0));
    positionAlloc4->Add (Vector(472.17414704819157,368.667145868605, 0.0));
    positionAlloc4->Add (Vector(286.8969234884138,289.7936813519695, 0.0));
    positionAlloc4->Add (Vector(291.5569791802353,337.3843934941905, 0.0));
    positionAlloc4->Add (Vector(126.48941627503257,95.61510699063925, 0.0));
    positionAlloc4->Add (Vector(64.34334277441644,472.67896375568984, 0.0));
    positionAlloc4->Add (Vector(464.7668710340396,185.48929390406872, 0.0));
    positionAlloc4->Add (Vector(220.5600321674116,498.9871101027031, 0.0));
    positionAlloc4->Add (Vector(294.0183747549983,304.54094720512086, 0.0));
    positionAlloc4->Add (Vector(17.383663980037547,124.69257964040354, 0.0));
    positionAlloc4->Add (Vector(435.7192539454814,250.10339044695905, 0.0));
    positionAlloc4->Add (Vector(464.5131452428471,149.26414122226146, 0.0));
    positionAlloc4->Add (Vector(302.3908245129258,123.14504967249663, 0.0));
    positionAlloc4->Add (Vector(196.86001576749234,286.837137758481, 0.0));
    positionAlloc4->Add (Vector(455.40953943991326,8.840836090457993, 0.0));
    positionAlloc4->Add (Vector(196.62643151876398,90.84055065533603, 0.0));
    positionAlloc4->Add (Vector(39.95834059304237,446.65705823279137, 0.0));
    positionAlloc4->Add (Vector(249.6621124734364,401.7901554988409, 0.0));
    positionAlloc4->Add (Vector(31.443278740830305,193.1375853861364, 0.0));
    positionAlloc4->Add (Vector(201.92220221611694,346.9703349812601, 0.0));
    positionAlloc4->Add (Vector(270.48731462450127,411.7491650208373, 0.0));
    positionAlloc4->Add (Vector(77.02402176334871,357.72161839105985, 0.0));
    positionAlloc4->Add (Vector(54.511667121826704,46.716569997111044, 0.0));
    positionAlloc4->Add (Vector(254.1028609472233,146.17643495003597, 0.0));
    positionAlloc4->Add (Vector(314.2895893134599,147.0842929060589, 0.0));
    positionAlloc4->Add (Vector(130.44592911529656,306.9859583388468, 0.0));
    positionAlloc4->Add (Vector(401.6183167729892,238.41668712603308, 0.0));
    positionAlloc4->Add (Vector(486.587074520706,63.305454001852446, 0.0));
    positionAlloc4->Add (Vector(17.648933239213516,77.48556044442412, 0.0));
    positionAlloc4->Add (Vector(36.2752755612728,298.61582402598356, 0.0));
    positionAlloc4->Add (Vector(410.9450977700307,36.50615178381622, 0.0));
    positionAlloc4->Add (Vector(91.82680319370378,133.2690582172308, 0.0));
    positionAlloc4->Add (Vector(127.89137861061556,299.5673401571703, 0.0));
    positionAlloc4->Add (Vector(142.12991972478568,56.46120949728895, 0.0));
    positionAlloc4->Add (Vector(78.40303246544872,171.74174526064718, 0.0));
    positionAlloc4->Add (Vector(190.23359081682318,145.0913656944311, 0.0));
    positionAlloc4->Add (Vector(234.2801626044867,233.84588643203602, 0.0));
    positionAlloc4->Add (Vector(39.67546248987114,435.26588435771185, 0.0));
    positionAlloc4->Add (Vector(433.10681144163357,499.3046499742961, 0.0));
    positionAlloc4->Add (Vector(462.0416036848524,395.5938531960423, 0.0));
    positionAlloc4->Add (Vector(121.17415694324951,246.3538808150188, 0.0));
    positionAlloc4->Add (Vector(472.64612159183497,402.035634165945, 0.0));
    positionAlloc4->Add (Vector(440.5723416905339,447.2190908947065, 0.0));
    positionAlloc4->Add (Vector(124.09449629672021,205.2606162645516, 0.0));
    positionAlloc4->Add (Vector(269.7588541511927,40.30062422653869, 0.0));
    positionAlloc4->Add (Vector(107.39538616983218,105.56190298626467, 0.0));
    positionAlloc4->Add (Vector(113.29717626077812,464.6064322267427, 0.0));
    positionAlloc4->Add (Vector(64.82991233732317,273.90895636924114, 0.0));
    positionAlloc4->Add (Vector(339.4726805912477,436.3340911787882, 0.0));
    positionAlloc4->Add (Vector(480.50614814008003,445.32642469353124, 0.0));
    positionAlloc4->Add (Vector(238.9075761855799,230.44284414076384, 0.0));
    positionAlloc4->Add (Vector(59.094215845315986,153.29878891260145, 0.0));
    positionAlloc4->Add (Vector(292.84499819257377,36.83331935895323, 0.0));
    positionAlloc4->Add (Vector(78.49922765425133,67.37119407121867, 0.0));
    positionAlloc4->Add (Vector(317.3569778839168,5.956485244538956, 0.0));
    positionAlloc4->Add (Vector(227.20154752402027,80.50740767275666, 0.0));
    positionAlloc4->Add (Vector(128.40687229397167,300.94175261508025, 0.0));
    positionAlloc4->Add (Vector(372.6997834819343,344.2929763439058, 0.0));
    positionAlloc4->Add (Vector(349.50816038446516,92.31683777948373, 0.0));
    positionAlloc4->Add (Vector(19.057687081580397,44.7637152805086, 0.0));
    positionAlloc4->Add (Vector(67.04503157222086,238.16516078673266, 0.0));
    positionAlloc4->Add (Vector(320.42973926496654,438.9581123291157, 0.0));
    positionAlloc4->Add (Vector(426.8353894712912,253.47956517912544, 0.0));
    positionAlloc4->Add (Vector(0.22509718194640982,414.6736058250563, 0.0));
    positionAlloc4->Add (Vector(420.0011261066018,309.31373329626416, 0.0));
    positionAlloc4->Add (Vector(52.53211104818018,52.59519206116864, 0.0));
    positionAlloc4->Add (Vector(194.97374352514186,7.895452113243495, 0.0));
    positionAlloc4->Add (Vector(467.1845799687882,172.20001904041405, 0.0));
    positionAlloc4->Add (Vector(50.35454705000536,335.3178242039074, 0.0));
    positionAlloc4->Add (Vector(484.41258403489445,433.38856916835783, 0.0));
    positionAlloc4->Add (Vector(87.09165237833449,416.93057082329636, 0.0));
    positionAlloc4->Add (Vector(339.6082634063667,100.93955528091763, 0.0));
    positionAlloc4->Add (Vector(124.96875099292892,222.26841404511123, 0.0));
    positionAlloc4->Add (Vector(343.2600908089413,111.16503522934396, 0.0));
    positionAlloc4->Add (Vector(286.65895193665267,20.70538504364294, 0.0));
    positionAlloc4->Add (Vector(187.2172662011752,164.44788659823405, 0.0));
    positionAlloc4->Add (Vector(191.49647613302645,479.1874101240463, 0.0));
    positionAlloc4->Add (Vector(493.9649470099598,429.6909998121771, 0.0));
    positionAlloc4->Add (Vector(475.23891879265983,319.9318373472178, 0.0));
    positionAlloc4->Add (Vector(249.51657492678248,387.1084716680521, 0.0));
    positionAlloc4->Add (Vector(94.208122215857,78.96623078230502, 0.0));
    positionAlloc4->Add (Vector(492.62475941961685,16.94801856498168, 0.0));
    positionAlloc4->Add (Vector(85.84213752739994,498.1265667564164, 0.0));
    positionAlloc4->Add (Vector(395.59155142816047,55.998709413722814, 0.0));
    positionAlloc4->Add (Vector(68.1074770330175,370.15529105517305, 0.0));
    positionAlloc4->Add (Vector(57.14127444209627,405.46938892203093, 0.0));
    positionAlloc4->Add (Vector(109.1223406171552,73.09562466921716, 0.0));
    positionAlloc4->Add (Vector(379.83940281150285,132.27308995343785, 0.0));
    positionAlloc4->Add (Vector(25.99594685378004,39.18897849011965, 0.0));
    positionAlloc4->Add (Vector(300.3312458247164,71.20364272326884, 0.0));
    positionAlloc4->Add (Vector(414.37201050435795,334.64385260127347, 0.0));
    positionAlloc4->Add (Vector(406.53747394898886,341.5825691164114, 0.0));
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
	  flowmon->SerializeToXmlFile ("scratch/switch_SA_flowmon/switch_SA16.flowmon", false, false);
	 //FIM FLOW MONITOR
		Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (serverApps.Get (0));
		std::cout << "Bytes received by server 1: " << sink1->GetTotalRx () << " ("
							<< (8. * sink1->GetTotalRx ()) / 1000000 / simTime << " Mbps)"
							<< std::endl;
	 return 0;
	}