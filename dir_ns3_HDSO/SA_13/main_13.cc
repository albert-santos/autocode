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
	  uint16_t numberOfRrhs = 13;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 313;
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
    positionAlloc->Add (Vector (500.0,0.0, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,500.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,333.3333333333333, 0.0));
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
    positionAlloc4->Add (Vector(243.2178981057298,483.1852729449435, 0.0));
    positionAlloc4->Add (Vector(269.1053532917322,169.5456735905611, 0.0));
    positionAlloc4->Add (Vector(86.07628010543966,54.668580290648364, 0.0));
    positionAlloc4->Add (Vector(261.3478453076311,437.5062949104095, 0.0));
    positionAlloc4->Add (Vector(480.45255472793696,126.95165914060603, 0.0));
    positionAlloc4->Add (Vector(364.6381721903892,59.736100027683314, 0.0));
    positionAlloc4->Add (Vector(408.5449605435242,491.4858247152865, 0.0));
    positionAlloc4->Add (Vector(460.6731756628615,331.61593305792314, 0.0));
    positionAlloc4->Add (Vector(422.2194957644989,380.33448788936244, 0.0));
    positionAlloc4->Add (Vector(357.3175928153556,372.85605020131266, 0.0));
    positionAlloc4->Add (Vector(21.3964715981787,55.56959370820047, 0.0));
    positionAlloc4->Add (Vector(485.66856680382955,451.3150525813522, 0.0));
    positionAlloc4->Add (Vector(347.405564712603,260.66751962201425, 0.0));
    positionAlloc4->Add (Vector(490.4539357375508,199.5912971991285, 0.0));
    positionAlloc4->Add (Vector(342.85687735150225,188.93429620199447, 0.0));
    positionAlloc4->Add (Vector(187.74342430253736,121.0789388025899, 0.0));
    positionAlloc4->Add (Vector(242.32822396347657,404.4038059400331, 0.0));
    positionAlloc4->Add (Vector(113.33951320480179,429.0161890368951, 0.0));
    positionAlloc4->Add (Vector(425.1734551939154,326.4744092148911, 0.0));
    positionAlloc4->Add (Vector(210.4992867880467,263.15570326172457, 0.0));
    positionAlloc4->Add (Vector(438.3704580324221,219.33841614009486, 0.0));
    positionAlloc4->Add (Vector(23.734785978802353,270.4900113343661, 0.0));
    positionAlloc4->Add (Vector(257.18932737528837,19.513672646452708, 0.0));
    positionAlloc4->Add (Vector(73.18402825407195,215.658698850627, 0.0));
    positionAlloc4->Add (Vector(323.47150952684405,256.30116663476866, 0.0));
    positionAlloc4->Add (Vector(55.905232506237525,195.9979674928083, 0.0));
    positionAlloc4->Add (Vector(174.57749137027074,74.34424275541379, 0.0));
    positionAlloc4->Add (Vector(130.30480660747605,159.32851236466678, 0.0));
    positionAlloc4->Add (Vector(473.3279277250108,481.11512482006077, 0.0));
    positionAlloc4->Add (Vector(365.9808410984657,343.44017815401367, 0.0));
    positionAlloc4->Add (Vector(433.41397297105806,150.56163589827875, 0.0));
    positionAlloc4->Add (Vector(341.5954377100238,270.98834012623325, 0.0));
    positionAlloc4->Add (Vector(177.0554345996052,384.5473305075985, 0.0));
    positionAlloc4->Add (Vector(140.27670207873788,431.5837111225992, 0.0));
    positionAlloc4->Add (Vector(103.7407796779381,350.50228582075096, 0.0));
    positionAlloc4->Add (Vector(302.76264587736,64.9458128586362, 0.0));
    positionAlloc4->Add (Vector(315.9096114638249,369.85689478652404, 0.0));
    positionAlloc4->Add (Vector(189.795049710409,249.21968587029718, 0.0));
    positionAlloc4->Add (Vector(97.53571028363916,362.3622052807667, 0.0));
    positionAlloc4->Add (Vector(394.1096249510719,158.18158233927588, 0.0));
    positionAlloc4->Add (Vector(321.5865058383211,245.85048383726559, 0.0));
    positionAlloc4->Add (Vector(28.51024419632048,260.79295411775786, 0.0));
    positionAlloc4->Add (Vector(410.36794201137957,339.917393922249, 0.0));
    positionAlloc4->Add (Vector(156.82113854708413,383.62591474835506, 0.0));
    positionAlloc4->Add (Vector(286.67995543560886,138.9440703813125, 0.0));
    positionAlloc4->Add (Vector(14.498942153916616,30.442539767989995, 0.0));
    positionAlloc4->Add (Vector(30.66277607223772,178.8202496380808, 0.0));
    positionAlloc4->Add (Vector(65.28331842909473,1.7827260627352737, 0.0));
    positionAlloc4->Add (Vector(445.58137412096426,107.02491579003876, 0.0));
    positionAlloc4->Add (Vector(353.9456862044286,351.4682444582515, 0.0));
    positionAlloc4->Add (Vector(389.69139565429975,494.16677447620265, 0.0));
    positionAlloc4->Add (Vector(124.34760116934157,395.6961610018424, 0.0));
    positionAlloc4->Add (Vector(261.17183655512736,199.30216435431842, 0.0));
    positionAlloc4->Add (Vector(459.5992151682676,353.2956702327891, 0.0));
    positionAlloc4->Add (Vector(385.6908299616493,327.08102626192755, 0.0));
    positionAlloc4->Add (Vector(214.20627911702806,364.3978703264423, 0.0));
    positionAlloc4->Add (Vector(336.40225160917214,168.05373479941244, 0.0));
    positionAlloc4->Add (Vector(193.5104275918591,125.87384852518207, 0.0));
    positionAlloc4->Add (Vector(469.0868429103644,435.389189183198, 0.0));
    positionAlloc4->Add (Vector(127.56999204300212,219.7702649383601, 0.0));
    positionAlloc4->Add (Vector(36.51859703142263,272.83213082518034, 0.0));
    positionAlloc4->Add (Vector(406.79922183538156,104.97414122286163, 0.0));
    positionAlloc4->Add (Vector(331.193918077636,486.0438996198631, 0.0));
    positionAlloc4->Add (Vector(126.26316712482816,351.880843200619, 0.0));
    positionAlloc4->Add (Vector(66.16364863099462,187.6725054893615, 0.0));
    positionAlloc4->Add (Vector(116.49196875048484,134.6303673375379, 0.0));
    positionAlloc4->Add (Vector(182.61021233687487,394.42611233089497, 0.0));
    positionAlloc4->Add (Vector(453.45869306682164,65.76933225897874, 0.0));
    positionAlloc4->Add (Vector(356.7602250083388,437.18010545730345, 0.0));
    positionAlloc4->Add (Vector(475.64643443240544,285.9983525651573, 0.0));
    positionAlloc4->Add (Vector(238.95205068417198,103.80304771801529, 0.0));
    positionAlloc4->Add (Vector(171.08387901124783,127.20533084252588, 0.0));
    positionAlloc4->Add (Vector(383.8095168357919,316.37119155319806, 0.0));
    positionAlloc4->Add (Vector(349.57096811741087,96.44223804763979, 0.0));
    positionAlloc4->Add (Vector(167.23716236023907,247.1763681324684, 0.0));
    positionAlloc4->Add (Vector(424.2654218371377,351.97791724511626, 0.0));
    positionAlloc4->Add (Vector(295.98352823154244,412.5734668794202, 0.0));
    positionAlloc4->Add (Vector(95.1285904199074,356.0718530477542, 0.0));
    positionAlloc4->Add (Vector(214.42415042936545,87.81075846858354, 0.0));
    positionAlloc4->Add (Vector(491.4832503476423,19.931134526492144, 0.0));
    positionAlloc4->Add (Vector(0.7443021538445782,327.51846083988585, 0.0));
    positionAlloc4->Add (Vector(248.33800967696158,415.68587579410445, 0.0));
    positionAlloc4->Add (Vector(26.45463483427185,408.48016791635445, 0.0));
    positionAlloc4->Add (Vector(163.51845338626575,2.9570672483250626, 0.0));
    positionAlloc4->Add (Vector(96.64430305237276,404.56058607641273, 0.0));
    positionAlloc4->Add (Vector(31.387290372541898,7.892236888534354, 0.0));
    positionAlloc4->Add (Vector(207.76385430296958,168.31008691880274, 0.0));
    positionAlloc4->Add (Vector(157.88046152072798,210.55330095142838, 0.0));
    positionAlloc4->Add (Vector(45.695482213582885,452.18448372974547, 0.0));
    positionAlloc4->Add (Vector(66.86603525593654,61.18427986683278, 0.0));
    positionAlloc4->Add (Vector(281.7864953401977,439.06453570990755, 0.0));
    positionAlloc4->Add (Vector(197.3893848651971,184.81705091032092, 0.0));
    positionAlloc4->Add (Vector(354.4637412713623,376.4860967886773, 0.0));
    positionAlloc4->Add (Vector(39.57488513576679,378.7596567389081, 0.0));
    positionAlloc4->Add (Vector(111.07589480949137,256.069811220723, 0.0));
    positionAlloc4->Add (Vector(12.995776399403358,125.89892397242258, 0.0));
    positionAlloc4->Add (Vector(484.30542821696656,325.7349407399199, 0.0));
    positionAlloc4->Add (Vector(306.95058285231534,393.4159475699663, 0.0));
    positionAlloc4->Add (Vector(305.3674734798173,74.47783899018701, 0.0));
    positionAlloc4->Add (Vector(270.9891171665062,247.16191175257163, 0.0));
    positionAlloc4->Add (Vector(368.7549134941182,482.009102388547, 0.0));
    positionAlloc4->Add (Vector(60.835318059428154,180.04256613797455, 0.0));
    positionAlloc4->Add (Vector(288.98203646551525,70.8542536752006, 0.0));
    positionAlloc4->Add (Vector(119.04505032872781,179.11243090078798, 0.0));
    positionAlloc4->Add (Vector(304.54086276757863,155.91980742916655, 0.0));
    positionAlloc4->Add (Vector(83.70115400395156,474.1925042331095, 0.0));
    positionAlloc4->Add (Vector(285.8980289875162,187.54550165176954, 0.0));
    positionAlloc4->Add (Vector(476.0435685407695,210.1010247671753, 0.0));
    positionAlloc4->Add (Vector(445.87894673111913,364.61067346172797, 0.0));
    positionAlloc4->Add (Vector(364.84123815609473,256.1338267616174, 0.0));
    positionAlloc4->Add (Vector(282.901971540903,182.65449482060237, 0.0));
    positionAlloc4->Add (Vector(432.2194286221484,419.7908494715684, 0.0));
    positionAlloc4->Add (Vector(380.42264627578595,389.8252297643322, 0.0));
    positionAlloc4->Add (Vector(464.2075416017756,99.03633570141118, 0.0));
    positionAlloc4->Add (Vector(168.67688674292552,440.349814583554, 0.0));
    positionAlloc4->Add (Vector(90.14025340730369,396.28144846857276, 0.0));
    positionAlloc4->Add (Vector(75.06708346856395,124.19177309743806, 0.0));
    positionAlloc4->Add (Vector(277.9285825950677,243.39324914201242, 0.0));
    positionAlloc4->Add (Vector(258.25803820285466,410.1310180705956, 0.0));
    positionAlloc4->Add (Vector(314.710370350112,56.78080714323897, 0.0));
    positionAlloc4->Add (Vector(184.8508844511903,85.22167544850795, 0.0));
    positionAlloc4->Add (Vector(184.76832566103286,483.93707587138584, 0.0));
    positionAlloc4->Add (Vector(46.94218430318969,120.45465206560723, 0.0));
    positionAlloc4->Add (Vector(337.1516076384189,288.71698398223987, 0.0));
    positionAlloc4->Add (Vector(68.30526128917359,160.4122510614171, 0.0));
    positionAlloc4->Add (Vector(330.14511164192925,216.04980936009628, 0.0));
    positionAlloc4->Add (Vector(363.7153221691049,36.25561124426224, 0.0));
    positionAlloc4->Add (Vector(260.9717935949724,125.7422937745214, 0.0));
    positionAlloc4->Add (Vector(10.927013328399626,384.53114045284406, 0.0));
    positionAlloc4->Add (Vector(135.20557656051753,146.09137682774355, 0.0));
    positionAlloc4->Add (Vector(326.5151899732922,223.58276382459252, 0.0));
    positionAlloc4->Add (Vector(455.3753875424409,137.93407602473638, 0.0));
    positionAlloc4->Add (Vector(405.5036004001249,119.55076537432285, 0.0));
    positionAlloc4->Add (Vector(76.09623703436374,145.5232957165597, 0.0));
    positionAlloc4->Add (Vector(62.37637378851091,36.15014335658506, 0.0));
    positionAlloc4->Add (Vector(476.6348253958206,154.24057130971775, 0.0));
    positionAlloc4->Add (Vector(227.32601767936578,130.83652942560175, 0.0));
    positionAlloc4->Add (Vector(313.5912197549738,386.903964406003, 0.0));
    positionAlloc4->Add (Vector(316.9584902024513,353.1322459256285, 0.0));
    positionAlloc4->Add (Vector(25.008899989031274,356.00473241362164, 0.0));
    positionAlloc4->Add (Vector(293.7996000619367,78.50881906054718, 0.0));
    positionAlloc4->Add (Vector(455.5773656243413,473.2609151637701, 0.0));
    positionAlloc4->Add (Vector(197.5823372351138,25.219377634487948, 0.0));
    positionAlloc4->Add (Vector(476.4747070238897,85.17794215047181, 0.0));
    positionAlloc4->Add (Vector(98.42178985920059,443.42653250259275, 0.0));
    positionAlloc4->Add (Vector(222.8930938382161,226.52517415342638, 0.0));
    positionAlloc4->Add (Vector(332.8831887280302,419.2689809195516, 0.0));
    positionAlloc4->Add (Vector(479.3407081891781,473.46067175014133, 0.0));
    positionAlloc4->Add (Vector(111.76750530866359,435.2648397864897, 0.0));
    positionAlloc4->Add (Vector(215.11035527010802,13.908394780761169, 0.0));
    positionAlloc4->Add (Vector(410.3403728791403,123.98492554050694, 0.0));
    positionAlloc4->Add (Vector(137.90952181171795,232.0631243175454, 0.0));
    positionAlloc4->Add (Vector(262.3216823667487,458.6266231437892, 0.0));
    positionAlloc4->Add (Vector(73.26128135811445,298.3838558131612, 0.0));
    positionAlloc4->Add (Vector(330.84227149449094,82.35115608935345, 0.0));
    positionAlloc4->Add (Vector(156.13116793005938,292.9235264989294, 0.0));
    positionAlloc4->Add (Vector(322.6957245050611,99.75453832111236, 0.0));
    positionAlloc4->Add (Vector(253.24895526702463,176.62289640495592, 0.0));
    positionAlloc4->Add (Vector(56.51491804820141,333.4728760956642, 0.0));
    positionAlloc4->Add (Vector(456.98013299898076,456.60239118654874, 0.0));
    positionAlloc4->Add (Vector(267.02457287478353,342.1741940444137, 0.0));
    positionAlloc4->Add (Vector(79.59767184244681,28.828840332930707, 0.0));
    positionAlloc4->Add (Vector(356.03714553221454,130.9125803438241, 0.0));
    positionAlloc4->Add (Vector(138.5285589167729,410.75394453339385, 0.0));
    positionAlloc4->Add (Vector(287.2731979720372,355.48590432193373, 0.0));
    positionAlloc4->Add (Vector(394.2401948577769,478.4343037769568, 0.0));
    positionAlloc4->Add (Vector(329.74334420304524,137.99660412382485, 0.0));
    positionAlloc4->Add (Vector(316.95697210024963,203.62760554474374, 0.0));
    positionAlloc4->Add (Vector(143.91761540055015,221.49552254862792, 0.0));
    positionAlloc4->Add (Vector(358.18680708486863,352.66423752332383, 0.0));
    positionAlloc4->Add (Vector(388.8760285021528,36.45882142396273, 0.0));
    positionAlloc4->Add (Vector(243.14336081630816,350.64109172611126, 0.0));
    positionAlloc4->Add (Vector(65.86363519591815,339.0183729567333, 0.0));
    positionAlloc4->Add (Vector(201.79243299706988,213.17253903940113, 0.0));
    positionAlloc4->Add (Vector(286.5265647687996,132.52076425463366, 0.0));
    positionAlloc4->Add (Vector(217.34295973903312,254.22287271564227, 0.0));
    positionAlloc4->Add (Vector(346.545717029239,165.1396781920077, 0.0));
    positionAlloc4->Add (Vector(202.57828773882719,430.8590206331816, 0.0));
    positionAlloc4->Add (Vector(377.15177557195943,57.1150719971909, 0.0));
    positionAlloc4->Add (Vector(63.6374726023482,479.3439088745132, 0.0));
    positionAlloc4->Add (Vector(162.96167849068732,346.98209946942126, 0.0));
    positionAlloc4->Add (Vector(303.2902191647111,369.7549493164667, 0.0));
    positionAlloc4->Add (Vector(130.35673299461757,465.5061852928418, 0.0));
    positionAlloc4->Add (Vector(58.28924591757545,285.62520192389377, 0.0));
    positionAlloc4->Add (Vector(92.99062218503357,202.0322289099482, 0.0));
    positionAlloc4->Add (Vector(298.036045150538,486.34347585664307, 0.0));
    positionAlloc4->Add (Vector(377.66578447841323,181.46570009689182, 0.0));
    positionAlloc4->Add (Vector(248.2259738751566,241.39351689095585, 0.0));
    positionAlloc4->Add (Vector(446.68779180559744,333.306504097132, 0.0));
    positionAlloc4->Add (Vector(392.16346524402,446.0299204127263, 0.0));
    positionAlloc4->Add (Vector(262.4252615235831,232.7407718804339, 0.0));
    positionAlloc4->Add (Vector(65.7874929420435,71.11383068406352, 0.0));
    positionAlloc4->Add (Vector(397.7553000552806,394.92673445447764, 0.0));
    positionAlloc4->Add (Vector(106.72851842421393,143.72392549430575, 0.0));
    positionAlloc4->Add (Vector(349.81797793973357,202.897773004935, 0.0));
    positionAlloc4->Add (Vector(348.45139368182413,193.3553011269521, 0.0));
    positionAlloc4->Add (Vector(292.45683547013607,14.81914550945862, 0.0));
    positionAlloc4->Add (Vector(187.82068744914815,254.21283471071965, 0.0));
    positionAlloc4->Add (Vector(180.23958252686873,202.17471096398987, 0.0));
    positionAlloc4->Add (Vector(11.91857460239082,181.9546014134318, 0.0));
    positionAlloc4->Add (Vector(26.183651578808586,360.84988334637444, 0.0));
    positionAlloc4->Add (Vector(449.8855917574287,338.2531895507822, 0.0));
    positionAlloc4->Add (Vector(278.7788735267755,57.73435476104177, 0.0));
    positionAlloc4->Add (Vector(459.34944604484997,405.28898862938405, 0.0));
    positionAlloc4->Add (Vector(266.86276516750144,138.5919877187392, 0.0));
    positionAlloc4->Add (Vector(348.7804629084106,122.843293848942, 0.0));
    positionAlloc4->Add (Vector(482.01150417376573,377.9630866737225, 0.0));
    positionAlloc4->Add (Vector(47.78568877048656,311.9784075757899, 0.0));
    positionAlloc4->Add (Vector(212.4047877694702,359.3032217818559, 0.0));
    positionAlloc4->Add (Vector(366.96885732108075,379.11771524937626, 0.0));
    positionAlloc4->Add (Vector(70.74848362024488,492.1122685812031, 0.0));
    positionAlloc4->Add (Vector(312.9966588564261,135.05959784003724, 0.0));
    positionAlloc4->Add (Vector(170.87551721222744,109.5188777982044, 0.0));
    positionAlloc4->Add (Vector(290.8506526174986,252.80014469909156, 0.0));
    positionAlloc4->Add (Vector(493.5981236385722,494.7569901942128, 0.0));
    positionAlloc4->Add (Vector(383.29706301979047,401.9163118422186, 0.0));
    positionAlloc4->Add (Vector(463.0957334155735,203.88148636345605, 0.0));
    positionAlloc4->Add (Vector(458.6268922275375,86.61785266861222, 0.0));
    positionAlloc4->Add (Vector(333.11715405215676,61.73643065357687, 0.0));
    positionAlloc4->Add (Vector(15.78487863943706,259.64057122651343, 0.0));
    positionAlloc4->Add (Vector(342.0539443574055,224.48021207487463, 0.0));
    positionAlloc4->Add (Vector(290.4290779738864,321.4149209904342, 0.0));
    positionAlloc4->Add (Vector(275.95272812655634,105.2555823320831, 0.0));
    positionAlloc4->Add (Vector(95.24737882783113,394.09019918907944, 0.0));
    positionAlloc4->Add (Vector(285.7870603992902,132.67545376273117, 0.0));
    positionAlloc4->Add (Vector(116.63524907548717,342.32087480561347, 0.0));
    positionAlloc4->Add (Vector(394.4782481937859,356.97390301994426, 0.0));
    positionAlloc4->Add (Vector(362.01435166213355,325.53665992369105, 0.0));
    positionAlloc4->Add (Vector(494.7802462958941,266.1070568934961, 0.0));
    positionAlloc4->Add (Vector(340.4206760986085,450.19891413157393, 0.0));
    positionAlloc4->Add (Vector(243.95104653977057,265.2769590577812, 0.0));
    positionAlloc4->Add (Vector(240.8822395268913,429.5072498033892, 0.0));
    positionAlloc4->Add (Vector(457.4269924520307,475.6499769549183, 0.0));
    positionAlloc4->Add (Vector(77.18966172840302,410.37865798334576, 0.0));
    positionAlloc4->Add (Vector(140.69257512622247,312.27381630493403, 0.0));
    positionAlloc4->Add (Vector(35.578830681997076,476.01672422396734, 0.0));
    positionAlloc4->Add (Vector(491.25552390418767,335.5921988377072, 0.0));
    positionAlloc4->Add (Vector(75.53890431489363,333.78425647578393, 0.0));
    positionAlloc4->Add (Vector(382.23502630042583,155.32828886204675, 0.0));
    positionAlloc4->Add (Vector(266.032369500002,295.6992982791686, 0.0));
    positionAlloc4->Add (Vector(369.1244458727609,14.729569405640797, 0.0));
    positionAlloc4->Add (Vector(166.77268172947169,289.0232911015195, 0.0));
    positionAlloc4->Add (Vector(494.53015470488265,16.54124174394961, 0.0));
    positionAlloc4->Add (Vector(361.9132862423586,381.3244423951685, 0.0));
    positionAlloc4->Add (Vector(330.92011170700164,238.42011414124408, 0.0));
    positionAlloc4->Add (Vector(399.41200104794734,321.8129608495864, 0.0));
    positionAlloc4->Add (Vector(456.6303768699044,66.30829839180097, 0.0));
    positionAlloc4->Add (Vector(128.40195917027486,58.46967312045276, 0.0));
    positionAlloc4->Add (Vector(326.68078319275276,152.4548336438261, 0.0));
    positionAlloc4->Add (Vector(100.95117996386166,238.04544495885636, 0.0));
    positionAlloc4->Add (Vector(209.6087893863099,322.3929916083658, 0.0));
    positionAlloc4->Add (Vector(58.08837809289052,405.12096522661096, 0.0));
    positionAlloc4->Add (Vector(84.47846128516274,460.19295834826045, 0.0));
    positionAlloc4->Add (Vector(198.6698753739065,245.28916412501016, 0.0));
    positionAlloc4->Add (Vector(268.6221842832401,29.089355555538653, 0.0));
    positionAlloc4->Add (Vector(112.28418282166814,399.5486403728829, 0.0));
    positionAlloc4->Add (Vector(183.24042144215392,242.6086509744591, 0.0));
    positionAlloc4->Add (Vector(467.7962313908419,151.60763441238484, 0.0));
    positionAlloc4->Add (Vector(380.65478409830797,55.03716132203124, 0.0));
    positionAlloc4->Add (Vector(287.3487911322712,472.89485192559175, 0.0));
    positionAlloc4->Add (Vector(457.59349719274314,199.405891880787, 0.0));
    positionAlloc4->Add (Vector(488.46151759984497,397.3773540276179, 0.0));
    positionAlloc4->Add (Vector(388.2232680496864,83.05233595317391, 0.0));
    positionAlloc4->Add (Vector(406.0498795854636,217.2048120457945, 0.0));
    positionAlloc4->Add (Vector(342.931619687692,380.6094625566416, 0.0));
    positionAlloc4->Add (Vector(188.82616055751006,348.87698395019896, 0.0));
    positionAlloc4->Add (Vector(169.77558162982575,489.5430961434612, 0.0));
    positionAlloc4->Add (Vector(187.97451932798543,2.563142315930911, 0.0));
    positionAlloc4->Add (Vector(358.20304823009,380.6986184057671, 0.0));
    positionAlloc4->Add (Vector(309.2518959312217,401.21577666612126, 0.0));
    positionAlloc4->Add (Vector(372.73933512477345,182.33915509190462, 0.0));
    positionAlloc4->Add (Vector(237.86305116440087,220.83772859369273, 0.0));
    positionAlloc4->Add (Vector(351.9853061104166,286.01305305383846, 0.0));
    positionAlloc4->Add (Vector(177.892079363963,169.50619299380253, 0.0));
    positionAlloc4->Add (Vector(342.0976305507218,108.35562105125246, 0.0));
    positionAlloc4->Add (Vector(203.1432818082501,273.1498502896236, 0.0));
    positionAlloc4->Add (Vector(97.38876897801035,469.3781500792004, 0.0));
    positionAlloc4->Add (Vector(255.45330402955668,141.15567684524737, 0.0));
    positionAlloc4->Add (Vector(471.9469866599663,184.62376937950475, 0.0));
    positionAlloc4->Add (Vector(223.33273160517402,168.67833057683728, 0.0));
    positionAlloc4->Add (Vector(420.3553661409084,311.6914530418635, 0.0));
    positionAlloc4->Add (Vector(212.33083836607096,493.34204020712957, 0.0));
    positionAlloc4->Add (Vector(250.10985194188683,280.12831110132686, 0.0));
    positionAlloc4->Add (Vector(422.58622972559016,184.49467997282798, 0.0));
    positionAlloc4->Add (Vector(469.6696087430957,276.3795684757397, 0.0));
    positionAlloc4->Add (Vector(242.05860449407757,265.4038335399053, 0.0));
    positionAlloc4->Add (Vector(92.16701028248059,157.72969868359039, 0.0));
    positionAlloc4->Add (Vector(42.061698543757245,158.16838242179554, 0.0));
    positionAlloc4->Add (Vector(418.9365756412328,20.038456273491157, 0.0));
    positionAlloc4->Add (Vector(260.91792030964734,106.30866570389297, 0.0));
    positionAlloc4->Add (Vector(349.4798775825939,422.4655308115918, 0.0));
    positionAlloc4->Add (Vector(79.26341761892897,233.34414691920662, 0.0));
    positionAlloc4->Add (Vector(377.48781097434227,299.12234119722376, 0.0));
    positionAlloc4->Add (Vector(110.09167022366006,102.64059412332993, 0.0));
    positionAlloc4->Add (Vector(57.073542137780265,496.6621611407997, 0.0));
    positionAlloc4->Add (Vector(212.08719600627902,480.68760823301704, 0.0));
    positionAlloc4->Add (Vector(144.98438824943804,402.73877205399685, 0.0));
    positionAlloc4->Add (Vector(119.47527592277896,398.13173158809286, 0.0));
    positionAlloc4->Add (Vector(495.5076354825583,376.36281381001925, 0.0));
    positionAlloc4->Add (Vector(290.2275533599975,347.76831591417897, 0.0));
    positionAlloc4->Add (Vector(98.79639380746646,288.5839934106585, 0.0));
    positionAlloc4->Add (Vector(49.53000947070652,39.15571153194125, 0.0));
    positionAlloc4->Add (Vector(316.9047343352392,214.69229339235528, 0.0));
    positionAlloc4->Add (Vector(401.3946538341706,46.55821965763113, 0.0));
    positionAlloc4->Add (Vector(268.75146410166155,490.86016432259373, 0.0));
    positionAlloc4->Add (Vector(269.89546865620184,494.431999881803, 0.0));
    positionAlloc4->Add (Vector(193.7736870287025,370.0148272470232, 0.0));
    positionAlloc4->Add (Vector(109.97120105111274,335.4230132204671, 0.0));
    positionAlloc4->Add (Vector(30.353124863309098,155.7395368390856, 0.0));
    positionAlloc4->Add (Vector(324.35015257790667,44.07355584550887, 0.0));
    positionAlloc4->Add (Vector(202.23026039278037,233.5755911338268, 0.0));
    positionAlloc4->Add (Vector(219.39321440446403,477.9634743032206, 0.0));
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
	  flowmon->SerializeToXmlFile ("scratch/switch_SA_flowmon/switch_SA13.flowmon", false, false);
	 //FIM FLOW MONITOR
		Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (serverApps.Get (0));
		std::cout << "Bytes received by server 1: " << sink1->GetTotalRx () << " ("
							<< (8. * sink1->GetTotalRx ()) / 1000000 / simTime << " Mbps)"
							<< std::endl;
	 return 0;
	}