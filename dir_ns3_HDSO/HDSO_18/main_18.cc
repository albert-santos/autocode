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
	  uint16_t numberOfNodes = 320;
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
    positionAlloc->Add (Vector (500.0,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (500.0,0.0, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,500.0, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,166.66666666666666, 0.0));
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
    positionAlloc4->Add (Vector(398.5730593036677,373.6862325648809, 0.0));
    positionAlloc4->Add (Vector(450.27637684153694,463.41192890304296, 0.0));
    positionAlloc4->Add (Vector(253.81434202344565,431.42910237185754, 0.0));
    positionAlloc4->Add (Vector(420.37522063041223,147.98953033211487, 0.0));
    positionAlloc4->Add (Vector(390.6019669838493,428.31483014326193, 0.0));
    positionAlloc4->Add (Vector(335.4007570966173,296.4048747199269, 0.0));
    positionAlloc4->Add (Vector(347.3532130441416,33.64450063180552, 0.0));
    positionAlloc4->Add (Vector(38.74198251673211,68.79348909337607, 0.0));
    positionAlloc4->Add (Vector(5.716268763917953,290.75301222869336, 0.0));
    positionAlloc4->Add (Vector(108.62639050426476,432.59808265311403, 0.0));
    positionAlloc4->Add (Vector(147.8748380438344,172.35351549590362, 0.0));
    positionAlloc4->Add (Vector(468.8103592915644,274.5526913973243, 0.0));
    positionAlloc4->Add (Vector(141.9361715163779,245.2523930821316, 0.0));
    positionAlloc4->Add (Vector(481.32884168665333,284.0301542362792, 0.0));
    positionAlloc4->Add (Vector(483.7130625331092,104.07423434998236, 0.0));
    positionAlloc4->Add (Vector(82.2664825530196,205.66743044069486, 0.0));
    positionAlloc4->Add (Vector(430.5411692764772,83.6999800341372, 0.0));
    positionAlloc4->Add (Vector(316.5753312097358,89.47866022899248, 0.0));
    positionAlloc4->Add (Vector(262.08392712759985,364.2056103839684, 0.0));
    positionAlloc4->Add (Vector(397.0727730497912,478.1482605786496, 0.0));
    positionAlloc4->Add (Vector(167.4385578374763,376.667748158615, 0.0));
    positionAlloc4->Add (Vector(128.63930299476976,258.2930672353013, 0.0));
    positionAlloc4->Add (Vector(422.3331486771072,193.9769759343592, 0.0));
    positionAlloc4->Add (Vector(95.04369101732112,388.18317200767444, 0.0));
    positionAlloc4->Add (Vector(369.6041940366312,68.14492361505592, 0.0));
    positionAlloc4->Add (Vector(398.4615112864864,72.34760596117823, 0.0));
    positionAlloc4->Add (Vector(495.4933685832477,271.7450744050419, 0.0));
    positionAlloc4->Add (Vector(241.60145521580418,355.83009255334156, 0.0));
    positionAlloc4->Add (Vector(159.71191622904269,383.65448539488284, 0.0));
    positionAlloc4->Add (Vector(306.04052211288257,341.4192006032434, 0.0));
    positionAlloc4->Add (Vector(134.86708691357012,317.3102172476544, 0.0));
    positionAlloc4->Add (Vector(15.606194462815859,464.6609287852203, 0.0));
    positionAlloc4->Add (Vector(365.3451462977762,314.9498168061402, 0.0));
    positionAlloc4->Add (Vector(473.48470676668575,261.061702786285, 0.0));
    positionAlloc4->Add (Vector(206.4593799380154,266.3373643178744, 0.0));
    positionAlloc4->Add (Vector(480.57721741630735,1.0509546620998678, 0.0));
    positionAlloc4->Add (Vector(305.3018234741791,469.2242254734224, 0.0));
    positionAlloc4->Add (Vector(249.92208202391168,355.3619854150108, 0.0));
    positionAlloc4->Add (Vector(155.04821305824623,135.03135657226517, 0.0));
    positionAlloc4->Add (Vector(180.90662387649763,97.73528287436534, 0.0));
    positionAlloc4->Add (Vector(244.90382545937595,191.20032396411102, 0.0));
    positionAlloc4->Add (Vector(22.630583758339096,110.08251975141248, 0.0));
    positionAlloc4->Add (Vector(94.00264191476715,264.02814013217545, 0.0));
    positionAlloc4->Add (Vector(371.121645076871,283.9665966793135, 0.0));
    positionAlloc4->Add (Vector(412.7578515322624,186.88980692250013, 0.0));
    positionAlloc4->Add (Vector(374.7431306661025,446.60091484333117, 0.0));
    positionAlloc4->Add (Vector(337.1512699907735,327.40350976566003, 0.0));
    positionAlloc4->Add (Vector(191.05557330915968,250.03284597604315, 0.0));
    positionAlloc4->Add (Vector(380.75545493172774,364.57427761155554, 0.0));
    positionAlloc4->Add (Vector(347.9183126329605,433.2601208249681, 0.0));
    positionAlloc4->Add (Vector(176.7417545480691,365.71083535452317, 0.0));
    positionAlloc4->Add (Vector(261.1451904492931,216.31132919274253, 0.0));
    positionAlloc4->Add (Vector(105.45772531952863,277.5815229142021, 0.0));
    positionAlloc4->Add (Vector(232.67136386242504,152.22670979052117, 0.0));
    positionAlloc4->Add (Vector(92.64945018702802,245.36578413014692, 0.0));
    positionAlloc4->Add (Vector(392.67934469050056,484.37203691947826, 0.0));
    positionAlloc4->Add (Vector(200.70894320022038,115.16414832490035, 0.0));
    positionAlloc4->Add (Vector(185.23425293415002,330.06822242999596, 0.0));
    positionAlloc4->Add (Vector(330.56870417457304,466.3756297832757, 0.0));
    positionAlloc4->Add (Vector(300.3339941077833,494.0288257488748, 0.0));
    positionAlloc4->Add (Vector(344.360596553501,225.48731700559904, 0.0));
    positionAlloc4->Add (Vector(37.7864243349077,111.93071164304868, 0.0));
    positionAlloc4->Add (Vector(372.73476973196125,460.8302053929209, 0.0));
    positionAlloc4->Add (Vector(100.15509908440767,372.47083000696324, 0.0));
    positionAlloc4->Add (Vector(172.7115897662017,260.9616941581784, 0.0));
    positionAlloc4->Add (Vector(353.6638536766561,341.41639268673543, 0.0));
    positionAlloc4->Add (Vector(382.1401905413372,295.75870085464715, 0.0));
    positionAlloc4->Add (Vector(460.12362754687894,369.02456222240966, 0.0));
    positionAlloc4->Add (Vector(336.2095913386228,332.0201454329189, 0.0));
    positionAlloc4->Add (Vector(212.44548874109876,336.3659759226108, 0.0));
    positionAlloc4->Add (Vector(297.6670085691923,69.21309568095008, 0.0));
    positionAlloc4->Add (Vector(167.68812137801254,282.0556301927121, 0.0));
    positionAlloc4->Add (Vector(227.70896293803284,256.60125009145634, 0.0));
    positionAlloc4->Add (Vector(66.71132617448056,306.6637094720477, 0.0));
    positionAlloc4->Add (Vector(252.25498900700106,55.40364417423421, 0.0));
    positionAlloc4->Add (Vector(32.77137048314188,178.59520280282138, 0.0));
    positionAlloc4->Add (Vector(39.14791234378562,314.5327075311517, 0.0));
    positionAlloc4->Add (Vector(273.32708168262207,151.2684954176134, 0.0));
    positionAlloc4->Add (Vector(76.5641566466827,102.82771042433913, 0.0));
    positionAlloc4->Add (Vector(136.55968733340623,217.38334967071026, 0.0));
    positionAlloc4->Add (Vector(40.85803640734703,221.23483942310835, 0.0));
    positionAlloc4->Add (Vector(267.7744944320474,110.068870621232, 0.0));
    positionAlloc4->Add (Vector(456.5972789483208,167.31096596876623, 0.0));
    positionAlloc4->Add (Vector(195.63203092166293,62.854106466663175, 0.0));
    positionAlloc4->Add (Vector(242.39439510907712,63.70478253818934, 0.0));
    positionAlloc4->Add (Vector(336.5358523182687,55.69411095446791, 0.0));
    positionAlloc4->Add (Vector(273.7636212802427,309.4535475154724, 0.0));
    positionAlloc4->Add (Vector(229.58220950900227,20.376266636684516, 0.0));
    positionAlloc4->Add (Vector(80.70566610959479,312.12501787432063, 0.0));
    positionAlloc4->Add (Vector(332.8642153060434,378.707136573876, 0.0));
    positionAlloc4->Add (Vector(52.175082580187826,89.89148795142926, 0.0));
    positionAlloc4->Add (Vector(35.78703454642568,64.92023159661042, 0.0));
    positionAlloc4->Add (Vector(398.31071750432096,117.98384279133545, 0.0));
    positionAlloc4->Add (Vector(136.037354688736,413.47115196851837, 0.0));
    positionAlloc4->Add (Vector(140.01491486492645,17.608419961709863, 0.0));
    positionAlloc4->Add (Vector(0.85782530949291,453.49430240854593, 0.0));
    positionAlloc4->Add (Vector(47.852247975698425,274.5118965662721, 0.0));
    positionAlloc4->Add (Vector(206.79780768590012,193.965729524792, 0.0));
    positionAlloc4->Add (Vector(438.17211650369535,216.97080330943768, 0.0));
    positionAlloc4->Add (Vector(282.554926991016,80.07715563616208, 0.0));
    positionAlloc4->Add (Vector(30.20355388410234,325.8041019170249, 0.0));
    positionAlloc4->Add (Vector(174.594422506632,405.8954381784081, 0.0));
    positionAlloc4->Add (Vector(252.15483946156846,164.20598273373105, 0.0));
    positionAlloc4->Add (Vector(217.97096557397595,73.7377553226618, 0.0));
    positionAlloc4->Add (Vector(477.7050804908998,488.67562253582594, 0.0));
    positionAlloc4->Add (Vector(394.6446399529905,142.4979518851634, 0.0));
    positionAlloc4->Add (Vector(45.20700428160917,197.23803197617312, 0.0));
    positionAlloc4->Add (Vector(142.37292407582797,55.514270329290405, 0.0));
    positionAlloc4->Add (Vector(122.77529821382221,492.23191385240057, 0.0));
    positionAlloc4->Add (Vector(330.7958092406613,441.60269376346724, 0.0));
    positionAlloc4->Add (Vector(418.22349391245865,194.15499956562482, 0.0));
    positionAlloc4->Add (Vector(179.15723814897117,497.91943701424157, 0.0));
    positionAlloc4->Add (Vector(374.06543646360865,401.0391670462823, 0.0));
    positionAlloc4->Add (Vector(239.0465035589056,0.691466747501368, 0.0));
    positionAlloc4->Add (Vector(408.25646873811854,174.79158664376592, 0.0));
    positionAlloc4->Add (Vector(174.23315699271197,98.4610898590354, 0.0));
    positionAlloc4->Add (Vector(80.8726836606376,438.70533119205805, 0.0));
    positionAlloc4->Add (Vector(330.4231505445961,161.09752787727615, 0.0));
    positionAlloc4->Add (Vector(288.80493595747515,336.9744261826265, 0.0));
    positionAlloc4->Add (Vector(230.03506553505625,163.65273819394406, 0.0));
    positionAlloc4->Add (Vector(48.88484763347717,183.05404473048847, 0.0));
    positionAlloc4->Add (Vector(346.70278938479333,389.4707097514779, 0.0));
    positionAlloc4->Add (Vector(198.91855124176487,47.165084810790184, 0.0));
    positionAlloc4->Add (Vector(294.3981904445649,250.394642342106, 0.0));
    positionAlloc4->Add (Vector(478.54527806530314,67.81898297864835, 0.0));
    positionAlloc4->Add (Vector(152.95418008338495,65.19726107109042, 0.0));
    positionAlloc4->Add (Vector(213.33097668576957,340.48719202877663, 0.0));
    positionAlloc4->Add (Vector(240.60485486411721,280.45304801673603, 0.0));
    positionAlloc4->Add (Vector(368.449035775542,421.73438222991354, 0.0));
    positionAlloc4->Add (Vector(423.76777191697414,67.74190495585769, 0.0));
    positionAlloc4->Add (Vector(384.51773814118513,499.9744008758194, 0.0));
    positionAlloc4->Add (Vector(158.76601036283967,35.15154847028845, 0.0));
    positionAlloc4->Add (Vector(386.60215924441206,415.57284078663446, 0.0));
    positionAlloc4->Add (Vector(332.45673107208927,231.60526532810994, 0.0));
    positionAlloc4->Add (Vector(103.49082557841332,8.129323968141088, 0.0));
    positionAlloc4->Add (Vector(99.26931882321388,35.538928986379446, 0.0));
    positionAlloc4->Add (Vector(55.68767355253923,468.48589368938923, 0.0));
    positionAlloc4->Add (Vector(462.2355861033208,280.7649160646799, 0.0));
    positionAlloc4->Add (Vector(42.925025799885994,49.83593947802228, 0.0));
    positionAlloc4->Add (Vector(31.834957759614923,271.93928516937797, 0.0));
    positionAlloc4->Add (Vector(172.7732559509706,108.69791107277288, 0.0));
    positionAlloc4->Add (Vector(5.792393514753414,263.0679496815236, 0.0));
    positionAlloc4->Add (Vector(225.82468009080642,79.82272139420854, 0.0));
    positionAlloc4->Add (Vector(80.55350085401264,175.691569511613, 0.0));
    positionAlloc4->Add (Vector(152.95016635483273,207.24236032082683, 0.0));
    positionAlloc4->Add (Vector(143.88118758767888,459.5876790202722, 0.0));
    positionAlloc4->Add (Vector(366.10349025407334,67.232139295267, 0.0));
    positionAlloc4->Add (Vector(172.47763737078643,459.9671844003068, 0.0));
    positionAlloc4->Add (Vector(83.01218823771539,352.50416281497826, 0.0));
    positionAlloc4->Add (Vector(132.65979951252334,48.797708755958844, 0.0));
    positionAlloc4->Add (Vector(149.79994096409334,351.58295518292863, 0.0));
    positionAlloc4->Add (Vector(11.913807216973826,266.4126470218616, 0.0));
    positionAlloc4->Add (Vector(54.88199888448231,34.1321938338342, 0.0));
    positionAlloc4->Add (Vector(447.00247293428794,114.34940344888788, 0.0));
    positionAlloc4->Add (Vector(125.81589217306305,2.6755930135380357, 0.0));
    positionAlloc4->Add (Vector(105.43397018507832,5.724313107898116, 0.0));
    positionAlloc4->Add (Vector(174.35026570243633,443.53337157630983, 0.0));
    positionAlloc4->Add (Vector(102.45690087851922,282.7036749102899, 0.0));
    positionAlloc4->Add (Vector(146.4590412334892,139.54611169570714, 0.0));
    positionAlloc4->Add (Vector(267.244193862158,306.7460089779438, 0.0));
    positionAlloc4->Add (Vector(433.2852401298098,211.33089997163185, 0.0));
    positionAlloc4->Add (Vector(181.87770145734765,182.3609011036435, 0.0));
    positionAlloc4->Add (Vector(123.02896144096064,249.9860595072611, 0.0));
    positionAlloc4->Add (Vector(74.751356547136,265.76104667344356, 0.0));
    positionAlloc4->Add (Vector(233.15127033575845,262.177546378354, 0.0));
    positionAlloc4->Add (Vector(382.363959725479,498.56471031691405, 0.0));
    positionAlloc4->Add (Vector(323.3200360790999,406.3320499132696, 0.0));
    positionAlloc4->Add (Vector(319.0099169628254,212.7030144134297, 0.0));
    positionAlloc4->Add (Vector(258.8334287798576,99.67645028130111, 0.0));
    positionAlloc4->Add (Vector(454.9943166552417,7.115216586322138, 0.0));
    positionAlloc4->Add (Vector(469.37703678646074,280.24606269542744, 0.0));
    positionAlloc4->Add (Vector(401.9034479108036,396.9977023690159, 0.0));
    positionAlloc4->Add (Vector(216.62999596293187,434.0095725986091, 0.0));
    positionAlloc4->Add (Vector(372.0608922204082,406.50398450714465, 0.0));
    positionAlloc4->Add (Vector(145.29558565242263,432.2908056597532, 0.0));
    positionAlloc4->Add (Vector(373.4960360817368,262.6323368756241, 0.0));
    positionAlloc4->Add (Vector(62.346266153622985,83.17410427418164, 0.0));
    positionAlloc4->Add (Vector(179.57861341017474,476.44489261304494, 0.0));
    positionAlloc4->Add (Vector(373.42242373368595,55.07759624407365, 0.0));
    positionAlloc4->Add (Vector(65.68514150811866,467.6523045303614, 0.0));
    positionAlloc4->Add (Vector(367.63898829404053,303.22106311791504, 0.0));
    positionAlloc4->Add (Vector(398.2056452537284,489.0178180276625, 0.0));
    positionAlloc4->Add (Vector(104.98620549242088,491.20474118034355, 0.0));
    positionAlloc4->Add (Vector(113.1034623680029,99.09060496510624, 0.0));
    positionAlloc4->Add (Vector(372.16408877519933,101.8032607592596, 0.0));
    positionAlloc4->Add (Vector(211.2074837706201,303.57383595912313, 0.0));
    positionAlloc4->Add (Vector(154.4034521895774,385.46253485631024, 0.0));
    positionAlloc4->Add (Vector(333.4914307816709,51.78426523514301, 0.0));
    positionAlloc4->Add (Vector(479.9994883201111,284.10133454428774, 0.0));
    positionAlloc4->Add (Vector(322.43113745599527,203.44438939894877, 0.0));
    positionAlloc4->Add (Vector(463.9198549315233,254.4734400166757, 0.0));
    positionAlloc4->Add (Vector(288.1224536425393,120.52838145751338, 0.0));
    positionAlloc4->Add (Vector(415.0394480386782,427.7473480134806, 0.0));
    positionAlloc4->Add (Vector(379.4913057630497,2.3454628916113607, 0.0));
    positionAlloc4->Add (Vector(277.87451255503254,160.19591225300073, 0.0));
    positionAlloc4->Add (Vector(380.4619821478323,145.02177585339098, 0.0));
    positionAlloc4->Add (Vector(213.4914253256144,339.4729039120816, 0.0));
    positionAlloc4->Add (Vector(40.7779774655595,106.55669370201248, 0.0));
    positionAlloc4->Add (Vector(140.56778370859996,269.1121584935323, 0.0));
    positionAlloc4->Add (Vector(313.7477343871386,341.614880365961, 0.0));
    positionAlloc4->Add (Vector(267.9431528843914,45.09627247712872, 0.0));
    positionAlloc4->Add (Vector(87.26973018061118,55.225557791752365, 0.0));
    positionAlloc4->Add (Vector(281.3407660208792,475.82166089751126, 0.0));
    positionAlloc4->Add (Vector(88.1621797965475,92.0452765232746, 0.0));
    positionAlloc4->Add (Vector(156.36334840219513,487.057201192331, 0.0));
    positionAlloc4->Add (Vector(159.7223759327075,174.1071645889788, 0.0));
    positionAlloc4->Add (Vector(436.2629343128291,199.29948450172196, 0.0));
    positionAlloc4->Add (Vector(412.97809402873827,384.36424098497247, 0.0));
    positionAlloc4->Add (Vector(483.72244762028737,242.4881945725994, 0.0));
    positionAlloc4->Add (Vector(249.2141328496248,71.76448598152419, 0.0));
    positionAlloc4->Add (Vector(296.134145746186,143.63352898232117, 0.0));
    positionAlloc4->Add (Vector(163.51616886001398,408.92893827695303, 0.0));
    positionAlloc4->Add (Vector(70.4729535093882,440.41153004582605, 0.0));
    positionAlloc4->Add (Vector(283.12404517615624,113.39272138459965, 0.0));
    positionAlloc4->Add (Vector(359.46179458573306,385.98471706276035, 0.0));
    positionAlloc4->Add (Vector(298.41551254516213,96.77674168850636, 0.0));
    positionAlloc4->Add (Vector(169.45616055203934,156.16401114929212, 0.0));
    positionAlloc4->Add (Vector(245.60940085499556,236.3519451356163, 0.0));
    positionAlloc4->Add (Vector(320.5473368927528,490.2720503188709, 0.0));
    positionAlloc4->Add (Vector(297.6963743906783,285.8401880712207, 0.0));
    positionAlloc4->Add (Vector(152.11415723529132,335.9038115412617, 0.0));
    positionAlloc4->Add (Vector(427.306677670933,288.2971428144457, 0.0));
    positionAlloc4->Add (Vector(297.8343150063149,31.813440634076095, 0.0));
    positionAlloc4->Add (Vector(459.8076405597942,296.73839616866184, 0.0));
    positionAlloc4->Add (Vector(233.19484431155053,112.2152201016689, 0.0));
    positionAlloc4->Add (Vector(37.174160241976004,458.5951956705106, 0.0));
    positionAlloc4->Add (Vector(487.15065742318546,154.5273746261922, 0.0));
    positionAlloc4->Add (Vector(478.21554500730394,443.6184233928649, 0.0));
    positionAlloc4->Add (Vector(411.65372446801075,420.0575068928212, 0.0));
    positionAlloc4->Add (Vector(116.49460443776782,167.8864695895098, 0.0));
    positionAlloc4->Add (Vector(449.13065684394377,8.936254849138125, 0.0));
    positionAlloc4->Add (Vector(237.4964422439007,345.3065320271547, 0.0));
    positionAlloc4->Add (Vector(129.19328693801714,48.34821671355522, 0.0));
    positionAlloc4->Add (Vector(70.98749481478522,365.6136250838112, 0.0));
    positionAlloc4->Add (Vector(89.79007567287522,264.42496806968586, 0.0));
    positionAlloc4->Add (Vector(344.13758557682365,353.72906231005504, 0.0));
    positionAlloc4->Add (Vector(436.5500060126127,447.2107395315743, 0.0));
    positionAlloc4->Add (Vector(66.55528572969594,326.01435273602675, 0.0));
    positionAlloc4->Add (Vector(17.70475197104082,395.1366610263636, 0.0));
    positionAlloc4->Add (Vector(491.5427661731605,435.9714505523615, 0.0));
    positionAlloc4->Add (Vector(274.37478100912165,9.865560845927268, 0.0));
    positionAlloc4->Add (Vector(14.966280804646848,260.56674241693156, 0.0));
    positionAlloc4->Add (Vector(108.04427844035824,153.81279582047534, 0.0));
    positionAlloc4->Add (Vector(10.574319989476999,71.4196570365005, 0.0));
    positionAlloc4->Add (Vector(435.45461975605673,422.26710210898295, 0.0));
    positionAlloc4->Add (Vector(196.80160749943565,496.72350001917135, 0.0));
    positionAlloc4->Add (Vector(239.29405045106145,165.38618235596863, 0.0));
    positionAlloc4->Add (Vector(129.74059267399429,376.99405196328894, 0.0));
    positionAlloc4->Add (Vector(44.840402525613,457.3990859790817, 0.0));
    positionAlloc4->Add (Vector(175.22038439891645,203.0757594872075, 0.0));
    positionAlloc4->Add (Vector(108.22744969876236,436.3516819478047, 0.0));
    positionAlloc4->Add (Vector(498.16467157733524,400.94526081049867, 0.0));
    positionAlloc4->Add (Vector(81.26781380034664,496.9745423353478, 0.0));
    positionAlloc4->Add (Vector(404.96189583641194,381.8277610892287, 0.0));
    positionAlloc4->Add (Vector(55.62958131094475,162.15342320272623, 0.0));
    positionAlloc4->Add (Vector(199.85802128266295,166.28976276891848, 0.0));
    positionAlloc4->Add (Vector(449.1998608196219,238.50817490405268, 0.0));
    positionAlloc4->Add (Vector(410.1184822865673,264.72893909388074, 0.0));
    positionAlloc4->Add (Vector(392.11954235961144,411.7776537549434, 0.0));
    positionAlloc4->Add (Vector(32.291149519134585,71.06997549251737, 0.0));
    positionAlloc4->Add (Vector(493.59983739438513,352.1866714102893, 0.0));
    positionAlloc4->Add (Vector(463.90438352475934,451.52721229482404, 0.0));
    positionAlloc4->Add (Vector(76.27130923663323,259.61398565606333, 0.0));
    positionAlloc4->Add (Vector(283.74053327211595,230.43356528627677, 0.0));
    positionAlloc4->Add (Vector(330.1835725701731,167.32401658903188, 0.0));
    positionAlloc4->Add (Vector(40.11706281262756,484.581169639109, 0.0));
    positionAlloc4->Add (Vector(68.94813260926813,379.9497938647714, 0.0));
    positionAlloc4->Add (Vector(425.6539337767706,497.48052556082075, 0.0));
    positionAlloc4->Add (Vector(228.02353222299564,62.879981663292504, 0.0));
    positionAlloc4->Add (Vector(24.094218515915532,291.76250809666834, 0.0));
    positionAlloc4->Add (Vector(8.213135221274538,253.9154258454941, 0.0));
    positionAlloc4->Add (Vector(219.91866265720773,279.4797990505825, 0.0));
    positionAlloc4->Add (Vector(103.58510904397534,97.1757462616707, 0.0));
    positionAlloc4->Add (Vector(207.08266507610122,335.68023644221057, 0.0));
    positionAlloc4->Add (Vector(466.73557244836314,66.10834407351851, 0.0));
    positionAlloc4->Add (Vector(173.07887023569353,366.6977934558533, 0.0));
    positionAlloc4->Add (Vector(218.1762632093882,276.2444023744056, 0.0));
    positionAlloc4->Add (Vector(276.41259154374956,121.1295491521313, 0.0));
    positionAlloc4->Add (Vector(27.605412215960857,456.85425697136185, 0.0));
    positionAlloc4->Add (Vector(225.59749488156,321.26834591196115, 0.0));
    positionAlloc4->Add (Vector(395.8590502519644,300.056789218682, 0.0));
    positionAlloc4->Add (Vector(24.527338418607936,223.82366545216547, 0.0));
    positionAlloc4->Add (Vector(461.9489786108163,142.12964511354565, 0.0));
    positionAlloc4->Add (Vector(392.56176272087254,439.850226940677, 0.0));
    positionAlloc4->Add (Vector(498.0944909970757,96.88387958000271, 0.0));
    positionAlloc4->Add (Vector(454.48126564552325,315.45899133096935, 0.0));
    positionAlloc4->Add (Vector(423.4953041913135,481.15856534747877, 0.0));
    positionAlloc4->Add (Vector(476.93643523915404,397.2751263428499, 0.0));
    positionAlloc4->Add (Vector(455.4646915024124,46.76747578938512, 0.0));
    positionAlloc4->Add (Vector(175.14079959835922,334.6715230300497, 0.0));
    positionAlloc4->Add (Vector(305.3331832381835,92.09064006718476, 0.0));
    positionAlloc4->Add (Vector(208.70604417404064,406.8465869017324, 0.0));
    positionAlloc4->Add (Vector(56.75870238533032,438.30552968454964, 0.0));
    positionAlloc4->Add (Vector(107.78777130555395,32.06480981658899, 0.0));
    positionAlloc4->Add (Vector(137.74471080599506,418.3017127038008, 0.0));
    positionAlloc4->Add (Vector(409.8067984916236,385.07048128399384, 0.0));
    positionAlloc4->Add (Vector(314.7978855809202,410.91217324774965, 0.0));
    positionAlloc4->Add (Vector(336.3981136731662,24.299808893476882, 0.0));
    positionAlloc4->Add (Vector(96.35873871082784,197.17053569659353, 0.0));
    positionAlloc4->Add (Vector(261.89264062680775,107.63311008116605, 0.0));
    positionAlloc4->Add (Vector(225.7560865243372,155.001492297404, 0.0));
    positionAlloc4->Add (Vector(337.3257011565393,193.3965616878931, 0.0));
    positionAlloc4->Add (Vector(58.92882646807429,152.49882761449706, 0.0));
    positionAlloc4->Add (Vector(69.07254273612656,207.85301849287728, 0.0));
    positionAlloc4->Add (Vector(48.491019305104686,288.74881913957995, 0.0));
    positionAlloc4->Add (Vector(325.5953338972937,402.19217701254973, 0.0));
    positionAlloc4->Add (Vector(245.25622397442044,312.4280900218327, 0.0));
    positionAlloc4->Add (Vector(400.33167980679934,426.98066760297024, 0.0));
    positionAlloc4->Add (Vector(499.93923336351844,238.20625228556924, 0.0));
    positionAlloc4->Add (Vector(330.44699851786214,103.08867773367703, 0.0));
    positionAlloc4->Add (Vector(265.31013403423117,238.5657508188779, 0.0));
    positionAlloc4->Add (Vector(67.62372782013193,397.00145419447233, 0.0));
    positionAlloc4->Add (Vector(244.94067049933525,299.83325763910176, 0.0));
    positionAlloc4->Add (Vector(314.97589385934396,393.5748287489508, 0.0));
    positionAlloc4->Add (Vector(22.151974449826195,276.0621849207015, 0.0));
    positionAlloc4->Add (Vector(185.8892016180919,153.88905316909862, 0.0));
    positionAlloc4->Add (Vector(82.91985743262175,147.99681283388793, 0.0));
    positionAlloc4->Add (Vector(347.15691324471555,3.146750076055138, 0.0));
    positionAlloc4->Add (Vector(176.996067412885,207.32656805178888, 0.0));
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
	  flowmon->SerializeToXmlFile ("scratch/switch_HDSO_flowmon/switch_HDSO18.flowmon", false, false);
	 //FIM FLOW MONITOR
		Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (serverApps.Get (0));
		std::cout << "Bytes received by server 1: " << sink1->GetTotalRx () << " ("
							<< (8. * sink1->GetTotalRx ()) / 1000000 / simTime << " Mbps)"
							<< std::endl;
	 return 0;
	}