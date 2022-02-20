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
	  uint16_t numberOfRrhs = 12;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 270;
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
    positionAlloc4->Add (Vector(12.560429365774562,68.03354299238946, 0.0));
    positionAlloc4->Add (Vector(313.9703052457382,305.5323964034448, 0.0));
    positionAlloc4->Add (Vector(147.40805050537531,462.98393044248456, 0.0));
    positionAlloc4->Add (Vector(264.20669519024966,164.32195041957792, 0.0));
    positionAlloc4->Add (Vector(253.15044901821986,43.132894287007005, 0.0));
    positionAlloc4->Add (Vector(62.636736551675895,386.5783925888668, 0.0));
    positionAlloc4->Add (Vector(455.7954420149638,334.3130136765845, 0.0));
    positionAlloc4->Add (Vector(426.54157948871693,242.8642573395428, 0.0));
    positionAlloc4->Add (Vector(213.3760071612596,91.36236549578186, 0.0));
    positionAlloc4->Add (Vector(218.2170891409662,151.901621408149, 0.0));
    positionAlloc4->Add (Vector(301.4441959638177,363.58264418467684, 0.0));
    positionAlloc4->Add (Vector(59.145314716579854,160.08988388248747, 0.0));
    positionAlloc4->Add (Vector(94.26452021260795,468.3816691284975, 0.0));
    positionAlloc4->Add (Vector(277.25205202490196,417.15347790948687, 0.0));
    positionAlloc4->Add (Vector(188.63158525029706,137.95434010178624, 0.0));
    positionAlloc4->Add (Vector(289.79219483853103,433.7883805887202, 0.0));
    positionAlloc4->Add (Vector(180.54813692057448,358.85465854891606, 0.0));
    positionAlloc4->Add (Vector(85.90129553300125,163.70105523107352, 0.0));
    positionAlloc4->Add (Vector(489.8352695150328,10.557302569179082, 0.0));
    positionAlloc4->Add (Vector(397.764992453645,228.564613963532, 0.0));
    positionAlloc4->Add (Vector(94.9432000506058,16.556660541137745, 0.0));
    positionAlloc4->Add (Vector(180.6078489315438,99.77868636159315, 0.0));
    positionAlloc4->Add (Vector(398.5304576547809,117.67098785643554, 0.0));
    positionAlloc4->Add (Vector(465.4824782396283,215.449769170226, 0.0));
    positionAlloc4->Add (Vector(486.6086090259272,103.1896938535395, 0.0));
    positionAlloc4->Add (Vector(489.6847828057931,170.21940314693452, 0.0));
    positionAlloc4->Add (Vector(331.0783291403536,135.49961162765723, 0.0));
    positionAlloc4->Add (Vector(71.38347109948973,192.84393646455095, 0.0));
    positionAlloc4->Add (Vector(143.64593737598702,278.54163237190414, 0.0));
    positionAlloc4->Add (Vector(371.913003500348,301.8098526427072, 0.0));
    positionAlloc4->Add (Vector(47.52513581652812,485.0872431941547, 0.0));
    positionAlloc4->Add (Vector(411.33572689104125,440.47489852406363, 0.0));
    positionAlloc4->Add (Vector(304.5758125008966,216.43059308739714, 0.0));
    positionAlloc4->Add (Vector(444.7238091189368,161.6387283155804, 0.0));
    positionAlloc4->Add (Vector(196.55724711099631,420.91116754277743, 0.0));
    positionAlloc4->Add (Vector(261.0243192141691,480.87680287717285, 0.0));
    positionAlloc4->Add (Vector(147.7062569776975,55.2155727915844, 0.0));
    positionAlloc4->Add (Vector(36.4030046147803,80.98554680264608, 0.0));
    positionAlloc4->Add (Vector(265.03064471638163,483.00258308785925, 0.0));
    positionAlloc4->Add (Vector(206.77213598730526,351.99699912875525, 0.0));
    positionAlloc4->Add (Vector(143.74603561528582,23.487755636487417, 0.0));
    positionAlloc4->Add (Vector(178.2272201486413,272.0787078459802, 0.0));
    positionAlloc4->Add (Vector(376.22314494203937,231.44381574085455, 0.0));
    positionAlloc4->Add (Vector(477.835017838151,18.974489978750785, 0.0));
    positionAlloc4->Add (Vector(464.82813331130916,165.9829167168837, 0.0));
    positionAlloc4->Add (Vector(41.96090512552447,35.04283027229443, 0.0));
    positionAlloc4->Add (Vector(33.45004842933707,290.61746552527035, 0.0));
    positionAlloc4->Add (Vector(288.8149419227993,341.3715915612394, 0.0));
    positionAlloc4->Add (Vector(161.02026731092712,345.1840693106954, 0.0));
    positionAlloc4->Add (Vector(377.22319721747795,146.70825739480188, 0.0));
    positionAlloc4->Add (Vector(324.93070664542563,141.5885465634072, 0.0));
    positionAlloc4->Add (Vector(31.465293098051127,339.19099666380583, 0.0));
    positionAlloc4->Add (Vector(453.89870708858786,116.03087232061449, 0.0));
    positionAlloc4->Add (Vector(238.17405453969133,180.9822166484964, 0.0));
    positionAlloc4->Add (Vector(383.76957095462564,438.80272926507445, 0.0));
    positionAlloc4->Add (Vector(189.4986563141061,49.155906675061146, 0.0));
    positionAlloc4->Add (Vector(272.4559574892181,149.8601397694853, 0.0));
    positionAlloc4->Add (Vector(241.47005782505266,12.67042223302134, 0.0));
    positionAlloc4->Add (Vector(361.19195219782,46.94381310294704, 0.0));
    positionAlloc4->Add (Vector(494.67493081977017,191.73230749315334, 0.0));
    positionAlloc4->Add (Vector(307.5609539570547,367.80552374039223, 0.0));
    positionAlloc4->Add (Vector(51.81733801338417,246.36051379837897, 0.0));
    positionAlloc4->Add (Vector(20.613696826767725,409.68027005948045, 0.0));
    positionAlloc4->Add (Vector(3.3802549293449613,384.13870049510547, 0.0));
    positionAlloc4->Add (Vector(246.03138371359296,71.63868723256667, 0.0));
    positionAlloc4->Add (Vector(126.73515456827272,229.03895603895486, 0.0));
    positionAlloc4->Add (Vector(272.07958593780086,15.712966696008523, 0.0));
    positionAlloc4->Add (Vector(0.4880323700493516,157.19985118910512, 0.0));
    positionAlloc4->Add (Vector(119.18530215028721,317.58607476058194, 0.0));
    positionAlloc4->Add (Vector(433.85544955013256,489.55139925004454, 0.0));
    positionAlloc4->Add (Vector(88.65795689969997,25.81786658073959, 0.0));
    positionAlloc4->Add (Vector(346.54825241851677,341.32214737879536, 0.0));
    positionAlloc4->Add (Vector(256.12057399905706,401.9535797512737, 0.0));
    positionAlloc4->Add (Vector(481.9640859972584,406.49942590231797, 0.0));
    positionAlloc4->Add (Vector(101.94474392754715,102.95165462469235, 0.0));
    positionAlloc4->Add (Vector(29.560215990091677,4.405924942538442, 0.0));
    positionAlloc4->Add (Vector(455.72638571807937,175.05495710686642, 0.0));
    positionAlloc4->Add (Vector(417.0900323909242,327.2873739095436, 0.0));
    positionAlloc4->Add (Vector(297.8907405421672,151.58757275566293, 0.0));
    positionAlloc4->Add (Vector(390.93707290647427,472.14196504227556, 0.0));
    positionAlloc4->Add (Vector(35.981394520035515,111.21258023151803, 0.0));
    positionAlloc4->Add (Vector(159.58270378514823,84.41147109156144, 0.0));
    positionAlloc4->Add (Vector(114.33287037008722,362.7865290791883, 0.0));
    positionAlloc4->Add (Vector(449.4385521225797,315.92389996123234, 0.0));
    positionAlloc4->Add (Vector(121.73656862097498,451.5429157847985, 0.0));
    positionAlloc4->Add (Vector(142.7379122067036,371.3705453890828, 0.0));
    positionAlloc4->Add (Vector(199.5987254600951,74.41496057413616, 0.0));
    positionAlloc4->Add (Vector(2.6234458278563966,188.12732674714894, 0.0));
    positionAlloc4->Add (Vector(369.593948211944,166.31646101490426, 0.0));
    positionAlloc4->Add (Vector(48.34781395395965,96.48725531043223, 0.0));
    positionAlloc4->Add (Vector(158.0997375701973,482.7232661674305, 0.0));
    positionAlloc4->Add (Vector(212.3808609782784,190.5228846745104, 0.0));
    positionAlloc4->Add (Vector(1.2523284480897012,379.1940429440418, 0.0));
    positionAlloc4->Add (Vector(10.682429203241139,146.72496686674046, 0.0));
    positionAlloc4->Add (Vector(273.41333601400294,345.96181640036974, 0.0));
    positionAlloc4->Add (Vector(278.92455931525313,69.67201414939372, 0.0));
    positionAlloc4->Add (Vector(462.90674939465566,169.97118933698502, 0.0));
    positionAlloc4->Add (Vector(324.82885898473154,392.9991242432086, 0.0));
    positionAlloc4->Add (Vector(278.43217239789834,205.6474790814485, 0.0));
    positionAlloc4->Add (Vector(368.20189274023954,438.9334434074589, 0.0));
    positionAlloc4->Add (Vector(63.444039902650964,5.923956750828296, 0.0));
    positionAlloc4->Add (Vector(153.753463954578,337.5283694488803, 0.0));
    positionAlloc4->Add (Vector(345.5349112104064,279.0269539407869, 0.0));
    positionAlloc4->Add (Vector(104.56713817510693,313.3402530508603, 0.0));
    positionAlloc4->Add (Vector(343.60239617462844,447.26055175899313, 0.0));
    positionAlloc4->Add (Vector(252.66235297175004,426.53360062471455, 0.0));
    positionAlloc4->Add (Vector(262.280393751586,135.34105997411993, 0.0));
    positionAlloc4->Add (Vector(271.4465755336963,193.6292467177247, 0.0));
    positionAlloc4->Add (Vector(45.773881409877816,400.21763411622663, 0.0));
    positionAlloc4->Add (Vector(122.1676811797941,314.32402029115735, 0.0));
    positionAlloc4->Add (Vector(228.23439083600684,441.61607740344493, 0.0));
    positionAlloc4->Add (Vector(343.791712992803,151.94370473102208, 0.0));
    positionAlloc4->Add (Vector(406.97506277844127,416.3961788809055, 0.0));
    positionAlloc4->Add (Vector(112.18197585715522,411.1014019447979, 0.0));
    positionAlloc4->Add (Vector(28.23806691638203,252.12120707358753, 0.0));
    positionAlloc4->Add (Vector(263.6628734425491,422.31290550728966, 0.0));
    positionAlloc4->Add (Vector(159.04823812725604,160.57103279343292, 0.0));
    positionAlloc4->Add (Vector(267.5455848124365,380.09549921659254, 0.0));
    positionAlloc4->Add (Vector(448.85754529539923,126.27864896630592, 0.0));
    positionAlloc4->Add (Vector(262.7293996813712,485.40444813730966, 0.0));
    positionAlloc4->Add (Vector(95.19923345283526,219.68773827627953, 0.0));
    positionAlloc4->Add (Vector(8.119740592664348,28.451699652395412, 0.0));
    positionAlloc4->Add (Vector(29.507756942061093,362.17642205570604, 0.0));
    positionAlloc4->Add (Vector(295.1045408676125,257.30104975065564, 0.0));
    positionAlloc4->Add (Vector(270.08748506116086,129.6664195993495, 0.0));
    positionAlloc4->Add (Vector(88.52178335127131,6.517287566953966, 0.0));
    positionAlloc4->Add (Vector(342.0736308579712,456.6478081691129, 0.0));
    positionAlloc4->Add (Vector(134.09516040484115,371.28581420525813, 0.0));
    positionAlloc4->Add (Vector(212.84226585060767,493.9894129427871, 0.0));
    positionAlloc4->Add (Vector(3.7162561852688847,193.97429998325381, 0.0));
    positionAlloc4->Add (Vector(247.22387190275091,340.7613955283873, 0.0));
    positionAlloc4->Add (Vector(468.12023607716486,201.82264248512877, 0.0));
    positionAlloc4->Add (Vector(78.99813895517394,46.1782776722447, 0.0));
    positionAlloc4->Add (Vector(350.3046592561306,203.39597566552985, 0.0));
    positionAlloc4->Add (Vector(362.41551690567184,290.50209221430293, 0.0));
    positionAlloc4->Add (Vector(136.7458413918628,93.86683116089878, 0.0));
    positionAlloc4->Add (Vector(422.41301797344175,214.2190917087789, 0.0));
    positionAlloc4->Add (Vector(156.0241135760898,90.85985165200789, 0.0));
    positionAlloc4->Add (Vector(212.96560704988838,50.550310827225, 0.0));
    positionAlloc4->Add (Vector(497.078470823748,114.20963892172159, 0.0));
    positionAlloc4->Add (Vector(127.06929730049666,69.36569621518917, 0.0));
    positionAlloc4->Add (Vector(37.96934278931141,263.0258884521357, 0.0));
    positionAlloc4->Add (Vector(59.225736277216726,188.63187751134353, 0.0));
    positionAlloc4->Add (Vector(361.4699789324035,161.91376027805327, 0.0));
    positionAlloc4->Add (Vector(492.81799775808486,106.75036148209416, 0.0));
    positionAlloc4->Add (Vector(201.11402176138193,159.0901671126802, 0.0));
    positionAlloc4->Add (Vector(413.8687403079726,53.5235454864319, 0.0));
    positionAlloc4->Add (Vector(351.1245457030003,374.9014896172932, 0.0));
    positionAlloc4->Add (Vector(11.950562066447734,61.389025055679944, 0.0));
    positionAlloc4->Add (Vector(243.78019843712622,496.44279913936407, 0.0));
    positionAlloc4->Add (Vector(197.1079328513403,7.622819606890041, 0.0));
    positionAlloc4->Add (Vector(401.98074416741366,311.93053164073325, 0.0));
    positionAlloc4->Add (Vector(127.96204465241273,110.73573139576686, 0.0));
    positionAlloc4->Add (Vector(272.4254871643683,0.10608998239536316, 0.0));
    positionAlloc4->Add (Vector(431.3244920572154,142.55587513752238, 0.0));
    positionAlloc4->Add (Vector(3.6754956430092856,337.1153707308341, 0.0));
    positionAlloc4->Add (Vector(470.61180936757785,88.29990760691008, 0.0));
    positionAlloc4->Add (Vector(98.0413154317083,475.8742074736798, 0.0));
    positionAlloc4->Add (Vector(459.37917359057496,463.00760641934096, 0.0));
    positionAlloc4->Add (Vector(164.14730821284945,369.1653023432567, 0.0));
    positionAlloc4->Add (Vector(132.26566755852915,470.2833830256364, 0.0));
    positionAlloc4->Add (Vector(45.73332547813702,50.643782216310825, 0.0));
    positionAlloc4->Add (Vector(235.02705216915032,332.1422646667346, 0.0));
    positionAlloc4->Add (Vector(164.7746483197936,173.51532079510028, 0.0));
    positionAlloc4->Add (Vector(394.5465746297869,387.29442690973804, 0.0));
    positionAlloc4->Add (Vector(336.2166729919013,438.64497629870993, 0.0));
    positionAlloc4->Add (Vector(428.9140175357358,76.70374808124048, 0.0));
    positionAlloc4->Add (Vector(210.64182685217537,431.5817661102524, 0.0));
    positionAlloc4->Add (Vector(45.945300272056066,318.9026923422196, 0.0));
    positionAlloc4->Add (Vector(72.22673192365825,146.8134517258643, 0.0));
    positionAlloc4->Add (Vector(390.0894064224213,45.401253776826636, 0.0));
    positionAlloc4->Add (Vector(201.9860867202767,35.15882611050852, 0.0));
    positionAlloc4->Add (Vector(278.69796493908746,135.7031116388533, 0.0));
    positionAlloc4->Add (Vector(229.4425401364863,478.37253009498016, 0.0));
    positionAlloc4->Add (Vector(440.48793748048854,14.459178583927645, 0.0));
    positionAlloc4->Add (Vector(461.84173109994356,306.67968338424265, 0.0));
    positionAlloc4->Add (Vector(199.4167234196907,323.94094370194597, 0.0));
    positionAlloc4->Add (Vector(357.9442621826459,123.78277231071738, 0.0));
    positionAlloc4->Add (Vector(256.86325764501873,280.92969497969744, 0.0));
    positionAlloc4->Add (Vector(317.19010498038944,432.4746722016078, 0.0));
    positionAlloc4->Add (Vector(316.85206433883354,232.99347628522244, 0.0));
    positionAlloc4->Add (Vector(326.527556313639,46.53757552097254, 0.0));
    positionAlloc4->Add (Vector(236.47378129608498,11.527356635856078, 0.0));
    positionAlloc4->Add (Vector(239.61987100470384,194.48088853560557, 0.0));
    positionAlloc4->Add (Vector(94.22863821257138,367.8237648080129, 0.0));
    positionAlloc4->Add (Vector(498.81999960196845,403.79953563008075, 0.0));
    positionAlloc4->Add (Vector(234.0995758867936,388.87791981682597, 0.0));
    positionAlloc4->Add (Vector(342.1785990332191,240.41174130234577, 0.0));
    positionAlloc4->Add (Vector(273.3654648704274,58.6388593731707, 0.0));
    positionAlloc4->Add (Vector(278.57917829236436,36.2655376697808, 0.0));
    positionAlloc4->Add (Vector(319.61706788491404,44.09343272176236, 0.0));
    positionAlloc4->Add (Vector(41.470970772552626,468.8222288470675, 0.0));
    positionAlloc4->Add (Vector(479.37739286090647,192.99882795840873, 0.0));
    positionAlloc4->Add (Vector(473.0652645261135,429.3566252896342, 0.0));
    positionAlloc4->Add (Vector(281.8650920278914,9.957021324542625, 0.0));
    positionAlloc4->Add (Vector(301.03935183245034,126.53161718854078, 0.0));
    positionAlloc4->Add (Vector(159.15702257665544,143.28963089358678, 0.0));
    positionAlloc4->Add (Vector(350.1023997857447,146.72595013364204, 0.0));
    positionAlloc4->Add (Vector(287.3788867287625,287.2051001955563, 0.0));
    positionAlloc4->Add (Vector(256.3051538493483,431.81527550465137, 0.0));
    positionAlloc4->Add (Vector(447.5339749498887,423.55853617747675, 0.0));
    positionAlloc4->Add (Vector(170.84837947103009,476.94888702390267, 0.0));
    positionAlloc4->Add (Vector(189.39603843658682,307.669800652229, 0.0));
    positionAlloc4->Add (Vector(247.39730694575485,101.37902286449018, 0.0));
    positionAlloc4->Add (Vector(49.90251420073394,338.6569001170825, 0.0));
    positionAlloc4->Add (Vector(468.7153085942387,129.48071562542756, 0.0));
    positionAlloc4->Add (Vector(35.515964845532324,21.074986018286445, 0.0));
    positionAlloc4->Add (Vector(408.15383039813065,134.30436509943888, 0.0));
    positionAlloc4->Add (Vector(172.1096009033774,90.86367322062844, 0.0));
    positionAlloc4->Add (Vector(233.626681323908,5.884372546808681, 0.0));
    positionAlloc4->Add (Vector(325.0312913721341,162.85251406518003, 0.0));
    positionAlloc4->Add (Vector(348.38886286347525,182.17992026657691, 0.0));
    positionAlloc4->Add (Vector(440.46570610239496,192.35375279881146, 0.0));
    positionAlloc4->Add (Vector(91.92757631829824,222.96713379268007, 0.0));
    positionAlloc4->Add (Vector(157.1487095346854,17.408608686923177, 0.0));
    positionAlloc4->Add (Vector(193.46660974386415,81.50004868282562, 0.0));
    positionAlloc4->Add (Vector(470.48960988514403,319.39110122206483, 0.0));
    positionAlloc4->Add (Vector(38.94842122468556,382.5336163744818, 0.0));
    positionAlloc4->Add (Vector(476.7885435747384,236.40417204869718, 0.0));
    positionAlloc4->Add (Vector(403.9774886190528,162.57221738722228, 0.0));
    positionAlloc4->Add (Vector(143.87718457687527,111.65374654159116, 0.0));
    positionAlloc4->Add (Vector(210.9210443055329,485.569705383903, 0.0));
    positionAlloc4->Add (Vector(174.79699322419333,306.7751212824097, 0.0));
    positionAlloc4->Add (Vector(275.4707282003152,149.01790392989605, 0.0));
    positionAlloc4->Add (Vector(174.35111043089034,159.11670634695452, 0.0));
    positionAlloc4->Add (Vector(121.13720149276381,377.194538512441, 0.0));
    positionAlloc4->Add (Vector(190.69995982602933,442.74004288041255, 0.0));
    positionAlloc4->Add (Vector(440.61595470722096,295.1412236560868, 0.0));
    positionAlloc4->Add (Vector(487.8025587271382,209.70456089714517, 0.0));
    positionAlloc4->Add (Vector(149.96555672781352,471.03416280330663, 0.0));
    positionAlloc4->Add (Vector(68.21717992178927,394.2033084220572, 0.0));
    positionAlloc4->Add (Vector(168.8525537323587,255.3616934506549, 0.0));
    positionAlloc4->Add (Vector(246.70859885268953,375.70725847579365, 0.0));
    positionAlloc4->Add (Vector(81.37856694885448,22.103113654362737, 0.0));
    positionAlloc4->Add (Vector(426.2828198629802,445.5619696410791, 0.0));
    positionAlloc4->Add (Vector(112.22906164670803,386.30663331441554, 0.0));
    positionAlloc4->Add (Vector(85.53335826106706,370.2630686822105, 0.0));
    positionAlloc4->Add (Vector(420.56661929465554,9.245381713938238, 0.0));
    positionAlloc4->Add (Vector(153.13800625624796,473.0446244742707, 0.0));
    positionAlloc4->Add (Vector(99.13880673108899,160.60370677443714, 0.0));
    positionAlloc4->Add (Vector(29.436599827071642,492.4107112567966, 0.0));
    positionAlloc4->Add (Vector(312.09386054193635,16.760172000810257, 0.0));
    positionAlloc4->Add (Vector(312.8114819952905,237.98522539825439, 0.0));
    positionAlloc4->Add (Vector(476.65680363958495,488.15995957537314, 0.0));
    positionAlloc4->Add (Vector(51.642438110356906,118.58407796749992, 0.0));
    positionAlloc4->Add (Vector(115.29541438858149,228.18952120072262, 0.0));
    positionAlloc4->Add (Vector(158.6936104260397,190.0485644659358, 0.0));
    positionAlloc4->Add (Vector(329.00294125888996,114.69264224955306, 0.0));
    positionAlloc4->Add (Vector(315.07924396685536,400.0061770237304, 0.0));
    positionAlloc4->Add (Vector(474.1887375006741,390.83669042166747, 0.0));
    positionAlloc4->Add (Vector(397.7690355296161,196.7596479579616, 0.0));
    positionAlloc4->Add (Vector(215.23773528111101,159.60356586417694, 0.0));
    positionAlloc4->Add (Vector(462.78514549720654,464.7554790012306, 0.0));
    positionAlloc4->Add (Vector(484.7038456568862,128.96556671396092, 0.0));
    positionAlloc4->Add (Vector(50.13992292917852,237.21576904148029, 0.0));
    positionAlloc4->Add (Vector(399.72117193212733,52.327010022403755, 0.0));
    positionAlloc4->Add (Vector(283.7071207370883,290.328078803813, 0.0));
    positionAlloc4->Add (Vector(30.97522467308633,466.84024591309685, 0.0));
    positionAlloc4->Add (Vector(40.93794911451193,196.5396523993615, 0.0));
    positionAlloc4->Add (Vector(134.58500740023248,338.35378376227413, 0.0));
    positionAlloc4->Add (Vector(214.54135197956225,204.31292628582216, 0.0));
    positionAlloc4->Add (Vector(232.14722246313184,294.431269446226, 0.0));
    positionAlloc4->Add (Vector(373.3052470511294,450.84978232852205, 0.0));
    positionAlloc4->Add (Vector(124.460911956782,263.84279654536124, 0.0));
    positionAlloc4->Add (Vector(227.31079186323893,463.87473574330994, 0.0));
    positionAlloc4->Add (Vector(288.3624609313302,453.1424618457509, 0.0));
    positionAlloc4->Add (Vector(497.36851695199533,138.9998526537722, 0.0));
    positionAlloc4->Add (Vector(159.9094397450623,113.35576292856253, 0.0));
    positionAlloc4->Add (Vector(263.0322303002453,19.25804265436937, 0.0));
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
	  flowmon->SerializeToXmlFile ("scratch/switch_HDSO_flowmon/switch_HDSO20.flowmon", false, false);
	 //FIM FLOW MONITOR
		Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (serverApps.Get (0));
		std::cout << "Bytes received by server 1: " << sink1->GetTotalRx () << " ("
							<< (8. * sink1->GetTotalRx ()) / 1000000 / simTime << " Mbps)"
							<< std::endl;
	 return 0;
	}