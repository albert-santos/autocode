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
	  uint16_t numberOfRrhs = 8;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 324;
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
    positionAlloc->Add (Vector (500.0,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,0.0, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,500.0, 0.0));
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
    positionAlloc4->Add (Vector(265.3532252226852,145.18503772051628, 0.0));
    positionAlloc4->Add (Vector(367.7380417126456,286.66397511903955, 0.0));
    positionAlloc4->Add (Vector(484.1693891017971,88.99336997791413, 0.0));
    positionAlloc4->Add (Vector(216.15064928188565,424.20650812955, 0.0));
    positionAlloc4->Add (Vector(77.65361507074464,381.3874849848465, 0.0));
    positionAlloc4->Add (Vector(135.52594704440313,281.00604846095337, 0.0));
    positionAlloc4->Add (Vector(342.05084027570456,408.2528904099337, 0.0));
    positionAlloc4->Add (Vector(152.6446725937634,338.32168979908363, 0.0));
    positionAlloc4->Add (Vector(123.7294524725297,353.97427699491533, 0.0));
    positionAlloc4->Add (Vector(140.4160423858432,30.97176692335718, 0.0));
    positionAlloc4->Add (Vector(43.66383785179073,196.4565226709905, 0.0));
    positionAlloc4->Add (Vector(116.52934350305661,456.3721357186327, 0.0));
    positionAlloc4->Add (Vector(343.39423118117975,261.02276138299607, 0.0));
    positionAlloc4->Add (Vector(34.305299859106576,491.423558752477, 0.0));
    positionAlloc4->Add (Vector(351.99340008288175,28.630879042517044, 0.0));
    positionAlloc4->Add (Vector(480.26063263306185,93.97424423952843, 0.0));
    positionAlloc4->Add (Vector(176.4976259976543,331.7791373690106, 0.0));
    positionAlloc4->Add (Vector(184.97918612532555,238.13953414665778, 0.0));
    positionAlloc4->Add (Vector(347.1646447644515,30.288676353870837, 0.0));
    positionAlloc4->Add (Vector(496.87594125653374,459.6203405866858, 0.0));
    positionAlloc4->Add (Vector(191.88385593754626,394.973037474602, 0.0));
    positionAlloc4->Add (Vector(373.6150600993243,389.13536992281655, 0.0));
    positionAlloc4->Add (Vector(384.64240885483474,18.21698264911592, 0.0));
    positionAlloc4->Add (Vector(434.8276818053315,19.842812949764866, 0.0));
    positionAlloc4->Add (Vector(126.77796138573055,116.29904468532781, 0.0));
    positionAlloc4->Add (Vector(13.666978018709585,219.09532568932005, 0.0));
    positionAlloc4->Add (Vector(208.99776082208942,369.5986796413875, 0.0));
    positionAlloc4->Add (Vector(443.26149975997623,329.48263354509925, 0.0));
    positionAlloc4->Add (Vector(486.1088450180242,465.3161147620763, 0.0));
    positionAlloc4->Add (Vector(211.89722964048886,327.3633270019049, 0.0));
    positionAlloc4->Add (Vector(447.1923215150927,213.53188777898606, 0.0));
    positionAlloc4->Add (Vector(261.45260500571874,16.251895143192087, 0.0));
    positionAlloc4->Add (Vector(125.76615782479256,250.06817060020964, 0.0));
    positionAlloc4->Add (Vector(188.9956042187397,407.9840701984379, 0.0));
    positionAlloc4->Add (Vector(409.5645834647267,37.400670093282294, 0.0));
    positionAlloc4->Add (Vector(272.9124100423784,379.79096168633976, 0.0));
    positionAlloc4->Add (Vector(161.4804025616921,205.7894438258937, 0.0));
    positionAlloc4->Add (Vector(261.1656295866114,486.29790054752584, 0.0));
    positionAlloc4->Add (Vector(401.74992954471014,68.38074172640208, 0.0));
    positionAlloc4->Add (Vector(346.34051346860775,121.89705496711505, 0.0));
    positionAlloc4->Add (Vector(100.43080489502387,116.74949494871856, 0.0));
    positionAlloc4->Add (Vector(192.4557213352149,450.7806435576541, 0.0));
    positionAlloc4->Add (Vector(135.11440473323955,237.6484990696, 0.0));
    positionAlloc4->Add (Vector(123.4149630155671,156.60285416840318, 0.0));
    positionAlloc4->Add (Vector(122.90593914726867,393.4219117635288, 0.0));
    positionAlloc4->Add (Vector(318.31936874632606,179.46209357070057, 0.0));
    positionAlloc4->Add (Vector(223.9265837841734,119.1057737490474, 0.0));
    positionAlloc4->Add (Vector(156.55451658972254,450.78447269172256, 0.0));
    positionAlloc4->Add (Vector(152.10631079357313,74.85873343448735, 0.0));
    positionAlloc4->Add (Vector(77.26883532229594,280.86869417350385, 0.0));
    positionAlloc4->Add (Vector(388.6148978067584,392.01853620215775, 0.0));
    positionAlloc4->Add (Vector(391.695387153951,17.236912444719586, 0.0));
    positionAlloc4->Add (Vector(370.6079163048321,62.6869508689592, 0.0));
    positionAlloc4->Add (Vector(249.67810928552132,316.59981613919825, 0.0));
    positionAlloc4->Add (Vector(97.2990452768862,446.46836507753414, 0.0));
    positionAlloc4->Add (Vector(71.69755756389456,146.43948428976665, 0.0));
    positionAlloc4->Add (Vector(145.981255241358,82.40836256258844, 0.0));
    positionAlloc4->Add (Vector(342.49154421339455,413.52383447579643, 0.0));
    positionAlloc4->Add (Vector(95.50217307097503,188.83719204572392, 0.0));
    positionAlloc4->Add (Vector(457.30596103059025,28.95067930336259, 0.0));
    positionAlloc4->Add (Vector(224.37097496288183,88.59235156011125, 0.0));
    positionAlloc4->Add (Vector(272.18463469463165,177.08479001741495, 0.0));
    positionAlloc4->Add (Vector(479.479969073979,56.41622120647105, 0.0));
    positionAlloc4->Add (Vector(167.82403969873172,268.6798908467786, 0.0));
    positionAlloc4->Add (Vector(260.52095699116063,227.360317786022, 0.0));
    positionAlloc4->Add (Vector(483.92009911626417,300.18156063674905, 0.0));
    positionAlloc4->Add (Vector(37.896058607189474,486.9615892723476, 0.0));
    positionAlloc4->Add (Vector(313.8620425148022,45.220868201599096, 0.0));
    positionAlloc4->Add (Vector(311.30632517822573,5.804622774641944, 0.0));
    positionAlloc4->Add (Vector(203.1105364212793,369.5137299928063, 0.0));
    positionAlloc4->Add (Vector(139.43843798390253,142.29698501973064, 0.0));
    positionAlloc4->Add (Vector(349.39324747089285,355.47692937142665, 0.0));
    positionAlloc4->Add (Vector(199.625557990885,89.37903965120204, 0.0));
    positionAlloc4->Add (Vector(198.45311289587997,409.90597441507384, 0.0));
    positionAlloc4->Add (Vector(162.71424261377143,417.59284688620636, 0.0));
    positionAlloc4->Add (Vector(434.13242536919745,288.4037651386302, 0.0));
    positionAlloc4->Add (Vector(106.08563661061126,69.31476015839411, 0.0));
    positionAlloc4->Add (Vector(270.29923858791085,80.82617432352629, 0.0));
    positionAlloc4->Add (Vector(339.54644691034497,21.705017813788253, 0.0));
    positionAlloc4->Add (Vector(181.2270615992458,60.72234655528219, 0.0));
    positionAlloc4->Add (Vector(357.34862923503016,338.06429196445833, 0.0));
    positionAlloc4->Add (Vector(17.78250963217132,311.34958252489133, 0.0));
    positionAlloc4->Add (Vector(250.99009604679196,17.966416691179322, 0.0));
    positionAlloc4->Add (Vector(253.38991765394087,38.76072005633874, 0.0));
    positionAlloc4->Add (Vector(33.785158261387615,10.062287225906575, 0.0));
    positionAlloc4->Add (Vector(132.67366455187613,467.55967513469005, 0.0));
    positionAlloc4->Add (Vector(272.0943680220765,454.2059589284786, 0.0));
    positionAlloc4->Add (Vector(108.87486596251323,240.4738445572843, 0.0));
    positionAlloc4->Add (Vector(254.99868204426957,171.16482852588345, 0.0));
    positionAlloc4->Add (Vector(235.73116986047305,316.336136768063, 0.0));
    positionAlloc4->Add (Vector(471.834109664064,406.82976816533585, 0.0));
    positionAlloc4->Add (Vector(405.4065223701197,183.04446540220604, 0.0));
    positionAlloc4->Add (Vector(454.1273766644591,155.42276276604284, 0.0));
    positionAlloc4->Add (Vector(241.25695472363307,400.20993459633297, 0.0));
    positionAlloc4->Add (Vector(360.440639326628,75.65935578389139, 0.0));
    positionAlloc4->Add (Vector(427.87754751434693,77.61522801110642, 0.0));
    positionAlloc4->Add (Vector(42.85408196202511,301.31260572767997, 0.0));
    positionAlloc4->Add (Vector(357.80198197433685,490.99851169116704, 0.0));
    positionAlloc4->Add (Vector(296.25415255778745,479.35638000303686, 0.0));
    positionAlloc4->Add (Vector(169.91096180353233,347.11507872571997, 0.0));
    positionAlloc4->Add (Vector(336.6888120229476,353.81614201940596, 0.0));
    positionAlloc4->Add (Vector(444.8898756743359,167.1257797616591, 0.0));
    positionAlloc4->Add (Vector(284.93458744559365,282.3217640455948, 0.0));
    positionAlloc4->Add (Vector(86.74745154598295,184.07008249523145, 0.0));
    positionAlloc4->Add (Vector(245.32278625997694,207.29025842091858, 0.0));
    positionAlloc4->Add (Vector(269.38272026661497,304.9884744606892, 0.0));
    positionAlloc4->Add (Vector(466.9793917189926,195.9842742390223, 0.0));
    positionAlloc4->Add (Vector(491.3489894289018,274.71373595905715, 0.0));
    positionAlloc4->Add (Vector(440.4782471360811,204.61285556532698, 0.0));
    positionAlloc4->Add (Vector(106.72756507689785,146.97672167100822, 0.0));
    positionAlloc4->Add (Vector(454.84103863895575,316.3519603088166, 0.0));
    positionAlloc4->Add (Vector(23.804075408826286,422.1374095489451, 0.0));
    positionAlloc4->Add (Vector(469.96121925063716,419.85711193010104, 0.0));
    positionAlloc4->Add (Vector(201.2707510175769,251.92209418098886, 0.0));
    positionAlloc4->Add (Vector(309.83931084207387,264.1285116334426, 0.0));
    positionAlloc4->Add (Vector(229.47629066075203,93.22169794948232, 0.0));
    positionAlloc4->Add (Vector(125.92509856150275,321.6865256460998, 0.0));
    positionAlloc4->Add (Vector(140.39764457599742,20.4179321231332, 0.0));
    positionAlloc4->Add (Vector(226.97111816845654,264.25603093288674, 0.0));
    positionAlloc4->Add (Vector(335.8985277744197,256.44130682570346, 0.0));
    positionAlloc4->Add (Vector(215.2073847066781,150.19434053647186, 0.0));
    positionAlloc4->Add (Vector(38.936511063747126,284.61964943628925, 0.0));
    positionAlloc4->Add (Vector(217.55262461357378,218.7128076166161, 0.0));
    positionAlloc4->Add (Vector(148.1369494201641,276.66888765263434, 0.0));
    positionAlloc4->Add (Vector(417.9917451705652,216.53036513892647, 0.0));
    positionAlloc4->Add (Vector(304.82869253711857,371.3883376118925, 0.0));
    positionAlloc4->Add (Vector(291.0299519878685,77.50597030978639, 0.0));
    positionAlloc4->Add (Vector(75.31608598617073,270.91884589314884, 0.0));
    positionAlloc4->Add (Vector(388.56695677680085,94.8940646270784, 0.0));
    positionAlloc4->Add (Vector(330.9399430871621,169.68545069024483, 0.0));
    positionAlloc4->Add (Vector(194.30038550291178,190.92335281007155, 0.0));
    positionAlloc4->Add (Vector(198.86768011643164,246.9794470838602, 0.0));
    positionAlloc4->Add (Vector(227.60554040155273,316.0900326935094, 0.0));
    positionAlloc4->Add (Vector(454.6645027287911,181.1303914199473, 0.0));
    positionAlloc4->Add (Vector(393.39424606644167,75.18046236315278, 0.0));
    positionAlloc4->Add (Vector(21.787605672938305,230.05999860637965, 0.0));
    positionAlloc4->Add (Vector(235.3091292130869,236.5574631939244, 0.0));
    positionAlloc4->Add (Vector(126.50700750167726,196.2486899309141, 0.0));
    positionAlloc4->Add (Vector(85.8158888506389,466.68045703249686, 0.0));
    positionAlloc4->Add (Vector(285.33614467557857,479.78750019017536, 0.0));
    positionAlloc4->Add (Vector(330.3100970870031,216.8214078671084, 0.0));
    positionAlloc4->Add (Vector(155.59726680212526,184.5046792248151, 0.0));
    positionAlloc4->Add (Vector(429.14967496927846,106.32343950990946, 0.0));
    positionAlloc4->Add (Vector(477.72390264308274,79.31004181104944, 0.0));
    positionAlloc4->Add (Vector(496.7312451098745,406.776921949698, 0.0));
    positionAlloc4->Add (Vector(247.82804422241722,302.7869285271371, 0.0));
    positionAlloc4->Add (Vector(174.8513809756297,30.1621891848734, 0.0));
    positionAlloc4->Add (Vector(237.4960081130571,486.3726391443756, 0.0));
    positionAlloc4->Add (Vector(7.948624229742906,79.2151123417904, 0.0));
    positionAlloc4->Add (Vector(248.58815401184958,396.8536764613091, 0.0));
    positionAlloc4->Add (Vector(456.32896312678423,72.93367574122905, 0.0));
    positionAlloc4->Add (Vector(371.94145441415515,456.97586449523345, 0.0));
    positionAlloc4->Add (Vector(376.66423111495044,217.74512462163676, 0.0));
    positionAlloc4->Add (Vector(389.1496849596295,144.98311072273435, 0.0));
    positionAlloc4->Add (Vector(334.97797245356384,132.1256893559924, 0.0));
    positionAlloc4->Add (Vector(214.52543558524025,465.93079932809366, 0.0));
    positionAlloc4->Add (Vector(298.29204113213996,60.23737233107007, 0.0));
    positionAlloc4->Add (Vector(265.17376664503064,394.0127305485379, 0.0));
    positionAlloc4->Add (Vector(6.198548185371133,234.2536105706253, 0.0));
    positionAlloc4->Add (Vector(356.67747824378944,449.3766574066934, 0.0));
    positionAlloc4->Add (Vector(443.32830577438205,184.9686838420267, 0.0));
    positionAlloc4->Add (Vector(120.48471234584535,301.524906814308, 0.0));
    positionAlloc4->Add (Vector(293.81576528110753,291.82065117691945, 0.0));
    positionAlloc4->Add (Vector(88.76421787836314,167.1518945857875, 0.0));
    positionAlloc4->Add (Vector(32.36500323108083,442.05028546342965, 0.0));
    positionAlloc4->Add (Vector(114.34563517609465,118.0455293896393, 0.0));
    positionAlloc4->Add (Vector(20.309414195656128,257.95320265608467, 0.0));
    positionAlloc4->Add (Vector(272.0497555317378,453.00556504583216, 0.0));
    positionAlloc4->Add (Vector(290.5864203226247,130.23902083791006, 0.0));
    positionAlloc4->Add (Vector(21.56807601731259,84.74488781327133, 0.0));
    positionAlloc4->Add (Vector(160.1730928270358,264.5847805431481, 0.0));
    positionAlloc4->Add (Vector(496.2642650991428,461.7337294557942, 0.0));
    positionAlloc4->Add (Vector(143.59826820919707,351.8941896514494, 0.0));
    positionAlloc4->Add (Vector(424.90522430561987,283.12870308978887, 0.0));
    positionAlloc4->Add (Vector(271.33367362091184,323.2748642556006, 0.0));
    positionAlloc4->Add (Vector(221.2208321097829,468.80078703551334, 0.0));
    positionAlloc4->Add (Vector(22.248552762687723,73.96497402367885, 0.0));
    positionAlloc4->Add (Vector(465.24962116145207,84.2551364539793, 0.0));
    positionAlloc4->Add (Vector(102.59337736389296,307.6434121814148, 0.0));
    positionAlloc4->Add (Vector(138.19013878674775,312.94309417357977, 0.0));
    positionAlloc4->Add (Vector(192.507131900923,162.2246010947921, 0.0));
    positionAlloc4->Add (Vector(107.75869067394467,190.68060703125872, 0.0));
    positionAlloc4->Add (Vector(165.88355610393447,0.505223305274527, 0.0));
    positionAlloc4->Add (Vector(290.74716816820774,206.31283025500912, 0.0));
    positionAlloc4->Add (Vector(320.44468034008645,104.86055210658091, 0.0));
    positionAlloc4->Add (Vector(411.4411927599964,273.81126137275027, 0.0));
    positionAlloc4->Add (Vector(320.0330532888259,233.59478287772774, 0.0));
    positionAlloc4->Add (Vector(290.45705340448677,178.45963494849343, 0.0));
    positionAlloc4->Add (Vector(499.6760993029212,253.8415437217923, 0.0));
    positionAlloc4->Add (Vector(348.058807530346,135.75772238071033, 0.0));
    positionAlloc4->Add (Vector(171.34310190679548,217.53095639179858, 0.0));
    positionAlloc4->Add (Vector(81.68632352280397,476.6739880132826, 0.0));
    positionAlloc4->Add (Vector(74.28583064131116,24.95283097817652, 0.0));
    positionAlloc4->Add (Vector(374.49330261038045,216.3744934532248, 0.0));
    positionAlloc4->Add (Vector(114.29682495817639,417.91776177191906, 0.0));
    positionAlloc4->Add (Vector(211.09136615120033,302.414305542396, 0.0));
    positionAlloc4->Add (Vector(53.71699603228824,350.0830250694868, 0.0));
    positionAlloc4->Add (Vector(354.88777151766027,101.54902864660525, 0.0));
    positionAlloc4->Add (Vector(101.61175686672792,176.1017869765313, 0.0));
    positionAlloc4->Add (Vector(117.34501550313037,243.5052428557431, 0.0));
    positionAlloc4->Add (Vector(446.56211493819904,217.22791836020977, 0.0));
    positionAlloc4->Add (Vector(9.01028799794351,395.8842486085369, 0.0));
    positionAlloc4->Add (Vector(14.977198045540652,193.68408218147886, 0.0));
    positionAlloc4->Add (Vector(121.27257358140264,29.26930980193454, 0.0));
    positionAlloc4->Add (Vector(65.4473930250894,359.04612943303493, 0.0));
    positionAlloc4->Add (Vector(232.33873540750682,440.6927848998077, 0.0));
    positionAlloc4->Add (Vector(235.63390188181492,12.252693145339277, 0.0));
    positionAlloc4->Add (Vector(38.02709778333485,356.1023476492986, 0.0));
    positionAlloc4->Add (Vector(421.4692505567033,487.7334761280781, 0.0));
    positionAlloc4->Add (Vector(67.35183193086186,103.68878038800905, 0.0));
    positionAlloc4->Add (Vector(337.3034678091059,447.1993271603158, 0.0));
    positionAlloc4->Add (Vector(97.57910778258699,273.92432264306876, 0.0));
    positionAlloc4->Add (Vector(496.1321865163434,129.66554593805063, 0.0));
    positionAlloc4->Add (Vector(180.51595039954898,73.4363132896408, 0.0));
    positionAlloc4->Add (Vector(20.619374024419656,229.57028664238038, 0.0));
    positionAlloc4->Add (Vector(436.85629777513157,119.7894601928971, 0.0));
    positionAlloc4->Add (Vector(367.93866610284647,280.5071585541537, 0.0));
    positionAlloc4->Add (Vector(345.6396258349625,20.72030791091173, 0.0));
    positionAlloc4->Add (Vector(105.63573374025792,195.33743843526153, 0.0));
    positionAlloc4->Add (Vector(168.64367419627203,249.32889270452995, 0.0));
    positionAlloc4->Add (Vector(416.57077751606005,239.81915440424473, 0.0));
    positionAlloc4->Add (Vector(306.22638777579743,448.95681439768106, 0.0));
    positionAlloc4->Add (Vector(213.67820164755662,148.83645625566922, 0.0));
    positionAlloc4->Add (Vector(83.84011692449722,362.61191843986296, 0.0));
    positionAlloc4->Add (Vector(403.1191394452371,393.43136419115865, 0.0));
    positionAlloc4->Add (Vector(155.26866362693355,130.70385454490884, 0.0));
    positionAlloc4->Add (Vector(89.9832756737498,26.117669271293444, 0.0));
    positionAlloc4->Add (Vector(430.12060761508167,245.79115845491327, 0.0));
    positionAlloc4->Add (Vector(404.47274737028306,44.88086857728641, 0.0));
    positionAlloc4->Add (Vector(78.26186736696783,310.55621872483175, 0.0));
    positionAlloc4->Add (Vector(220.0133955222562,7.036203257718753, 0.0));
    positionAlloc4->Add (Vector(47.29334976526584,108.2785530595296, 0.0));
    positionAlloc4->Add (Vector(16.894645561783605,452.9116661855866, 0.0));
    positionAlloc4->Add (Vector(109.35150904398799,93.06157175342955, 0.0));
    positionAlloc4->Add (Vector(1.2288156430482755,188.3437297298471, 0.0));
    positionAlloc4->Add (Vector(166.40205991415223,386.123877323531, 0.0));
    positionAlloc4->Add (Vector(463.0235170254106,169.6230783100932, 0.0));
    positionAlloc4->Add (Vector(434.31122629294924,369.4876778652168, 0.0));
    positionAlloc4->Add (Vector(493.92418928934296,161.62036388651597, 0.0));
    positionAlloc4->Add (Vector(417.57058324128934,58.092543914242455, 0.0));
    positionAlloc4->Add (Vector(147.88270608635256,243.1984826468634, 0.0));
    positionAlloc4->Add (Vector(271.34589898333076,25.6072028857241, 0.0));
    positionAlloc4->Add (Vector(19.29333746556827,428.2799356684122, 0.0));
    positionAlloc4->Add (Vector(89.45041917125596,264.64510383241605, 0.0));
    positionAlloc4->Add (Vector(436.3904789815428,231.4124187398906, 0.0));
    positionAlloc4->Add (Vector(96.72548635724625,469.51068899561415, 0.0));
    positionAlloc4->Add (Vector(81.01015430986574,345.13076171556884, 0.0));
    positionAlloc4->Add (Vector(315.65076250438983,392.15676574959537, 0.0));
    positionAlloc4->Add (Vector(258.8293664583373,229.2625293950838, 0.0));
    positionAlloc4->Add (Vector(2.8190592600234043,433.1229191761648, 0.0));
    positionAlloc4->Add (Vector(308.5900298639489,62.583440050627495, 0.0));
    positionAlloc4->Add (Vector(211.57445097079224,309.18825025695224, 0.0));
    positionAlloc4->Add (Vector(451.4917077907241,224.6677127444678, 0.0));
    positionAlloc4->Add (Vector(317.4754629835086,160.82838327630793, 0.0));
    positionAlloc4->Add (Vector(260.97425142586286,455.0429989650177, 0.0));
    positionAlloc4->Add (Vector(460.331099888727,282.37155129485706, 0.0));
    positionAlloc4->Add (Vector(45.083487214046144,349.91017811096964, 0.0));
    positionAlloc4->Add (Vector(69.37152485140119,328.3182748572679, 0.0));
    positionAlloc4->Add (Vector(485.409138125451,318.71773425553874, 0.0));
    positionAlloc4->Add (Vector(38.88534879950345,424.91069198250915, 0.0));
    positionAlloc4->Add (Vector(393.89770036929997,76.45588827074712, 0.0));
    positionAlloc4->Add (Vector(411.02343121307825,204.54214782353685, 0.0));
    positionAlloc4->Add (Vector(183.53561031717157,165.93791645508298, 0.0));
    positionAlloc4->Add (Vector(392.37024511916894,196.81319861041302, 0.0));
    positionAlloc4->Add (Vector(463.1449065762058,221.7637480253591, 0.0));
    positionAlloc4->Add (Vector(426.21210751393676,192.9678674934879, 0.0));
    positionAlloc4->Add (Vector(492.97458496515037,178.56660322835864, 0.0));
    positionAlloc4->Add (Vector(415.6766993075146,469.9406253623216, 0.0));
    positionAlloc4->Add (Vector(446.28413447760045,219.20052014423436, 0.0));
    positionAlloc4->Add (Vector(13.01333347979272,486.3413214402837, 0.0));
    positionAlloc4->Add (Vector(108.10517234862665,164.02432676068818, 0.0));
    positionAlloc4->Add (Vector(268.12872292669226,199.53691556219925, 0.0));
    positionAlloc4->Add (Vector(134.154045313512,147.27848754459416, 0.0));
    positionAlloc4->Add (Vector(37.29478178829354,129.63169164840897, 0.0));
    positionAlloc4->Add (Vector(433.354382357644,234.157347698451, 0.0));
    positionAlloc4->Add (Vector(332.72294171103164,373.98135928335796, 0.0));
    positionAlloc4->Add (Vector(332.6199919919017,380.97602286182297, 0.0));
    positionAlloc4->Add (Vector(56.18630014542752,319.98565162096287, 0.0));
    positionAlloc4->Add (Vector(404.91938225843637,189.44727241821047, 0.0));
    positionAlloc4->Add (Vector(250.80035104209796,273.7855102572717, 0.0));
    positionAlloc4->Add (Vector(180.6939546158922,372.79222455570044, 0.0));
    positionAlloc4->Add (Vector(31.432273848615132,329.8663700790593, 0.0));
    positionAlloc4->Add (Vector(439.8398996530929,288.7664895898455, 0.0));
    positionAlloc4->Add (Vector(44.32462090353118,153.98041698518767, 0.0));
    positionAlloc4->Add (Vector(240.5383672610702,413.61869404620654, 0.0));
    positionAlloc4->Add (Vector(232.1814040971214,257.7626909902729, 0.0));
    positionAlloc4->Add (Vector(462.9707888349779,57.897059982429056, 0.0));
    positionAlloc4->Add (Vector(272.6465266413445,162.41735232158666, 0.0));
    positionAlloc4->Add (Vector(417.1535937140096,145.8915139370443, 0.0));
    positionAlloc4->Add (Vector(111.29587147558634,448.2007766831458, 0.0));
    positionAlloc4->Add (Vector(494.8661316470761,131.8691450390481, 0.0));
    positionAlloc4->Add (Vector(311.73102404418273,55.583311978441074, 0.0));
    positionAlloc4->Add (Vector(312.88652900461636,363.8178005866312, 0.0));
    positionAlloc4->Add (Vector(93.30927819129897,443.42123651885214, 0.0));
    positionAlloc4->Add (Vector(192.56060989867373,177.4097294893913, 0.0));
    positionAlloc4->Add (Vector(391.52896172082654,388.0107094005858, 0.0));
    positionAlloc4->Add (Vector(64.23083758116988,68.11924322194417, 0.0));
    positionAlloc4->Add (Vector(146.7745900484484,454.7313588104564, 0.0));
    positionAlloc4->Add (Vector(151.52054255637438,315.5290735062886, 0.0));
    positionAlloc4->Add (Vector(104.01908667032383,442.9019822678943, 0.0));
    positionAlloc4->Add (Vector(201.8873179868821,462.72864263841547, 0.0));
    positionAlloc4->Add (Vector(432.5685256210947,455.5752840722267, 0.0));
    positionAlloc4->Add (Vector(13.703977144576552,118.42842764445582, 0.0));
    positionAlloc4->Add (Vector(329.03659417106815,108.90699395589704, 0.0));
    positionAlloc4->Add (Vector(315.84305873785723,66.57045130072203, 0.0));
    positionAlloc4->Add (Vector(313.844518035939,50.658808718636486, 0.0));
    positionAlloc4->Add (Vector(36.42300427274553,288.0725332799754, 0.0));
    positionAlloc4->Add (Vector(307.18254537289806,262.64015982817523, 0.0));
    positionAlloc4->Add (Vector(67.7760682926784,386.73918704494264, 0.0));
    positionAlloc4->Add (Vector(482.6441348781644,417.6819850570104, 0.0));
    positionAlloc4->Add (Vector(23.962971378642006,387.7383058952514, 0.0));
    positionAlloc4->Add (Vector(40.004100070888946,136.10193721809844, 0.0));
    positionAlloc4->Add (Vector(467.14094355571154,397.3078598897194, 0.0));
    positionAlloc4->Add (Vector(202.71024441226078,238.57313867659514, 0.0));
    positionAlloc4->Add (Vector(192.44315362064413,97.94011703673344, 0.0));
    positionAlloc4->Add (Vector(41.717094284539236,118.2670724941049, 0.0));
    positionAlloc4->Add (Vector(24.954746664856366,280.457157745367, 0.0));
    positionAlloc4->Add (Vector(31.331452844272746,179.48181664519137, 0.0));
    positionAlloc4->Add (Vector(100.88505539845383,408.0113269612093, 0.0));
    positionAlloc4->Add (Vector(238.92534299865076,28.690516921774655, 0.0));
    positionAlloc4->Add (Vector(103.81685427725796,391.5585453697934, 0.0));
    positionAlloc4->Add (Vector(242.55867606726343,107.84592478092303, 0.0));
    positionAlloc4->Add (Vector(365.74647507717117,315.88881806430607, 0.0));
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
	  flowmon->SerializeToXmlFile ("scratch/switch_SA_flowmon/switch_SA1.flowmon", false, false);
	 //FIM FLOW MONITOR
		Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (serverApps.Get (0));
		std::cout << "Bytes received by server 1: " << sink1->GetTotalRx () << " ("
							<< (8. * sink1->GetTotalRx ()) / 1000000 / simTime << " Mbps)"
							<< std::endl;
	 return 0;
	}