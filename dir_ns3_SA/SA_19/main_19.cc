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
	  uint16_t numberOfRrhs = 7;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 306;
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
    positionAlloc4->Add (Vector(288.8849435471541,51.085177163049, 0.0));
    positionAlloc4->Add (Vector(356.7212324289553,83.20843745107564, 0.0));
    positionAlloc4->Add (Vector(470.8843867652458,47.943630582649554, 0.0));
    positionAlloc4->Add (Vector(39.15024026070163,302.2755713682887, 0.0));
    positionAlloc4->Add (Vector(351.9843390590018,146.42129781875224, 0.0));
    positionAlloc4->Add (Vector(65.16600300401882,349.6590593704399, 0.0));
    positionAlloc4->Add (Vector(230.69525695636577,360.7008812685311, 0.0));
    positionAlloc4->Add (Vector(221.4241025462387,8.0742281353815, 0.0));
    positionAlloc4->Add (Vector(428.68176814395645,43.84024050161223, 0.0));
    positionAlloc4->Add (Vector(159.03795746277842,343.84917560437395, 0.0));
    positionAlloc4->Add (Vector(413.3438419375076,359.74820086082156, 0.0));
    positionAlloc4->Add (Vector(45.38337924811658,304.5653885962961, 0.0));
    positionAlloc4->Add (Vector(177.87326953687145,258.9621983487707, 0.0));
    positionAlloc4->Add (Vector(360.1015618407545,37.94485263047942, 0.0));
    positionAlloc4->Add (Vector(439.8464304494571,319.509199425916, 0.0));
    positionAlloc4->Add (Vector(240.87223112902845,24.394753972323713, 0.0));
    positionAlloc4->Add (Vector(184.04669759154913,199.48005045181827, 0.0));
    positionAlloc4->Add (Vector(467.91921789879973,250.82450284743902, 0.0));
    positionAlloc4->Add (Vector(106.50246895451303,177.2889411340831, 0.0));
    positionAlloc4->Add (Vector(213.4392569373782,77.22525706026678, 0.0));
    positionAlloc4->Add (Vector(122.42448164129289,470.751506734615, 0.0));
    positionAlloc4->Add (Vector(6.7827302098955595,34.11391039654976, 0.0));
    positionAlloc4->Add (Vector(343.4627889855311,229.18422210282213, 0.0));
    positionAlloc4->Add (Vector(121.25849013875167,103.8531818598839, 0.0));
    positionAlloc4->Add (Vector(447.5669054911346,87.56836878833285, 0.0));
    positionAlloc4->Add (Vector(427.8778914895402,138.01368139659542, 0.0));
    positionAlloc4->Add (Vector(42.64892456868363,59.283428500147615, 0.0));
    positionAlloc4->Add (Vector(378.2815270884202,377.8264264225571, 0.0));
    positionAlloc4->Add (Vector(52.79793323703169,499.30224902683057, 0.0));
    positionAlloc4->Add (Vector(65.73853957168008,301.13452632465544, 0.0));
    positionAlloc4->Add (Vector(306.4614146108256,57.69870829355167, 0.0));
    positionAlloc4->Add (Vector(428.52217959223344,141.29699457878903, 0.0));
    positionAlloc4->Add (Vector(101.48963286249207,79.81690069983388, 0.0));
    positionAlloc4->Add (Vector(185.16029583062,408.53637000875165, 0.0));
    positionAlloc4->Add (Vector(230.19808827804673,424.8925475523462, 0.0));
    positionAlloc4->Add (Vector(473.25232691257327,154.7126929227184, 0.0));
    positionAlloc4->Add (Vector(277.5921374619947,438.35369374346277, 0.0));
    positionAlloc4->Add (Vector(66.9294038855151,415.27555185088926, 0.0));
    positionAlloc4->Add (Vector(367.84734937180605,412.4096902633253, 0.0));
    positionAlloc4->Add (Vector(309.96255101426385,5.59224575652334, 0.0));
    positionAlloc4->Add (Vector(8.06242477179725,145.2447685009704, 0.0));
    positionAlloc4->Add (Vector(412.80095410192047,131.08550179113877, 0.0));
    positionAlloc4->Add (Vector(121.22149986072517,203.54136890023966, 0.0));
    positionAlloc4->Add (Vector(11.861984738888776,428.0182028078003, 0.0));
    positionAlloc4->Add (Vector(395.6997560215992,317.2635594363931, 0.0));
    positionAlloc4->Add (Vector(334.29181118913385,336.88015405283954, 0.0));
    positionAlloc4->Add (Vector(210.2813420988361,218.3175142687987, 0.0));
    positionAlloc4->Add (Vector(242.04233038964045,387.0433651284499, 0.0));
    positionAlloc4->Add (Vector(228.46258306844553,70.70993114344148, 0.0));
    positionAlloc4->Add (Vector(453.5219271171357,94.84576785738702, 0.0));
    positionAlloc4->Add (Vector(237.69224987898036,448.367387683304, 0.0));
    positionAlloc4->Add (Vector(3.62265019534308,338.51279558873057, 0.0));
    positionAlloc4->Add (Vector(21.761099300822373,282.5919257887106, 0.0));
    positionAlloc4->Add (Vector(475.10347654798665,422.3318629526981, 0.0));
    positionAlloc4->Add (Vector(397.926082808059,326.9472466038041, 0.0));
    positionAlloc4->Add (Vector(125.53839088733426,175.84290823414477, 0.0));
    positionAlloc4->Add (Vector(152.4901424435988,0.42976345859796483, 0.0));
    positionAlloc4->Add (Vector(226.74515685897757,277.3524716963065, 0.0));
    positionAlloc4->Add (Vector(244.88539869051888,416.4775724712962, 0.0));
    positionAlloc4->Add (Vector(112.25954680050876,92.28643608548055, 0.0));
    positionAlloc4->Add (Vector(59.356725310637515,227.19449031357664, 0.0));
    positionAlloc4->Add (Vector(477.1734341838308,171.0176224056877, 0.0));
    positionAlloc4->Add (Vector(436.07647919219283,394.886646155067, 0.0));
    positionAlloc4->Add (Vector(152.78997187940175,268.57564839776074, 0.0));
    positionAlloc4->Add (Vector(157.72515805584203,390.81750802135394, 0.0));
    positionAlloc4->Add (Vector(432.0493138282669,117.83595323461616, 0.0));
    positionAlloc4->Add (Vector(242.12467108987858,294.3123959090786, 0.0));
    positionAlloc4->Add (Vector(123.5513764443682,299.0026832726381, 0.0));
    positionAlloc4->Add (Vector(440.01802021797823,213.74054834523844, 0.0));
    positionAlloc4->Add (Vector(456.0798059966275,379.5755056729738, 0.0));
    positionAlloc4->Add (Vector(496.6018885813617,382.36843367032776, 0.0));
    positionAlloc4->Add (Vector(422.2663491003973,106.05983357058363, 0.0));
    positionAlloc4->Add (Vector(287.6619685936706,161.6013067094638, 0.0));
    positionAlloc4->Add (Vector(18.052597420948004,269.56831635957667, 0.0));
    positionAlloc4->Add (Vector(421.75210690562346,88.95418500847579, 0.0));
    positionAlloc4->Add (Vector(405.71554842942805,79.7743993610206, 0.0));
    positionAlloc4->Add (Vector(398.63425525054134,76.83891235831724, 0.0));
    positionAlloc4->Add (Vector(239.35403326763216,449.43853562936766, 0.0));
    positionAlloc4->Add (Vector(409.49177322440175,63.85048437733221, 0.0));
    positionAlloc4->Add (Vector(264.9353031110905,34.869687100884065, 0.0));
    positionAlloc4->Add (Vector(71.18661883598747,215.39045726689343, 0.0));
    positionAlloc4->Add (Vector(187.70411248250068,136.3598090493721, 0.0));
    positionAlloc4->Add (Vector(111.95277507304007,127.97635906092991, 0.0));
    positionAlloc4->Add (Vector(321.4676844759593,242.98105331169873, 0.0));
    positionAlloc4->Add (Vector(206.5271286502296,279.4350815217018, 0.0));
    positionAlloc4->Add (Vector(261.49848570554417,273.14330951514836, 0.0));
    positionAlloc4->Add (Vector(444.88149482551574,374.7599826401586, 0.0));
    positionAlloc4->Add (Vector(265.8362232415121,165.64835363598084, 0.0));
    positionAlloc4->Add (Vector(126.94498137277228,422.2736751778036, 0.0));
    positionAlloc4->Add (Vector(370.5553782504386,47.914133681085545, 0.0));
    positionAlloc4->Add (Vector(389.53955076696343,299.1038045943302, 0.0));
    positionAlloc4->Add (Vector(450.6939195395239,296.3963719652997, 0.0));
    positionAlloc4->Add (Vector(18.249159727660114,439.8420156644332, 0.0));
    positionAlloc4->Add (Vector(2.055546751952464,418.52095792066535, 0.0));
    positionAlloc4->Add (Vector(480.60167593796945,458.2190533470902, 0.0));
    positionAlloc4->Add (Vector(428.6430592411125,311.2566904613742, 0.0));
    positionAlloc4->Add (Vector(334.98715335716116,63.66055929402387, 0.0));
    positionAlloc4->Add (Vector(194.53720256128148,220.28539486095494, 0.0));
    positionAlloc4->Add (Vector(304.9933579910238,200.01865271317075, 0.0));
    positionAlloc4->Add (Vector(99.68362474651605,195.53109717994943, 0.0));
    positionAlloc4->Add (Vector(386.0234296161532,376.1286449018555, 0.0));
    positionAlloc4->Add (Vector(103.04541948354412,313.8348522684906, 0.0));
    positionAlloc4->Add (Vector(352.7669909519823,381.5196713926136, 0.0));
    positionAlloc4->Add (Vector(284.3648045331095,135.19946107322585, 0.0));
    positionAlloc4->Add (Vector(231.6146867448341,272.53995759799227, 0.0));
    positionAlloc4->Add (Vector(429.3542120401426,12.051062328056862, 0.0));
    positionAlloc4->Add (Vector(266.9741158467089,134.93204797118747, 0.0));
    positionAlloc4->Add (Vector(205.8589932728851,342.540730326661, 0.0));
    positionAlloc4->Add (Vector(409.125438657117,106.19676055736366, 0.0));
    positionAlloc4->Add (Vector(318.50411693132037,417.3807823729209, 0.0));
    positionAlloc4->Add (Vector(469.87923173376754,72.01120920074405, 0.0));
    positionAlloc4->Add (Vector(239.68871940317433,376.34355510855016, 0.0));
    positionAlloc4->Add (Vector(145.9145736538089,487.2244705322528, 0.0));
    positionAlloc4->Add (Vector(298.477205261046,395.4106337457637, 0.0));
    positionAlloc4->Add (Vector(117.00138062131438,497.7836769442433, 0.0));
    positionAlloc4->Add (Vector(94.39780482696702,260.1594663329558, 0.0));
    positionAlloc4->Add (Vector(39.958855819985985,416.9211686799222, 0.0));
    positionAlloc4->Add (Vector(27.644118150017604,29.67527728435887, 0.0));
    positionAlloc4->Add (Vector(499.8895094681481,228.21646540903805, 0.0));
    positionAlloc4->Add (Vector(121.49836136920644,4.7678522103922205, 0.0));
    positionAlloc4->Add (Vector(155.22734158333589,287.64322733577046, 0.0));
    positionAlloc4->Add (Vector(465.77223173109667,245.3800632761368, 0.0));
    positionAlloc4->Add (Vector(210.46726092295953,484.03106847766094, 0.0));
    positionAlloc4->Add (Vector(195.01488764072522,439.4177968791342, 0.0));
    positionAlloc4->Add (Vector(72.17900908142838,207.62123489858848, 0.0));
    positionAlloc4->Add (Vector(81.91529552342186,215.47819024963343, 0.0));
    positionAlloc4->Add (Vector(215.98344193727752,151.40333954463392, 0.0));
    positionAlloc4->Add (Vector(433.93463162243137,171.58450955568915, 0.0));
    positionAlloc4->Add (Vector(475.1425615526954,491.07719829964293, 0.0));
    positionAlloc4->Add (Vector(197.0384968678144,163.85201772912555, 0.0));
    positionAlloc4->Add (Vector(192.452765063772,497.1778622011528, 0.0));
    positionAlloc4->Add (Vector(117.59919410572556,449.46779194945276, 0.0));
    positionAlloc4->Add (Vector(66.60233295289753,178.53374711879738, 0.0));
    positionAlloc4->Add (Vector(164.58482021100107,24.64689129011427, 0.0));
    positionAlloc4->Add (Vector(302.8853053329937,108.36519239401893, 0.0));
    positionAlloc4->Add (Vector(81.20634414740874,316.2757165447586, 0.0));
    positionAlloc4->Add (Vector(51.44928031025559,452.01923476328335, 0.0));
    positionAlloc4->Add (Vector(345.7219754766069,447.14517386953634, 0.0));
    positionAlloc4->Add (Vector(174.26298341944053,355.12119087901857, 0.0));
    positionAlloc4->Add (Vector(498.0176076343443,90.88095449702166, 0.0));
    positionAlloc4->Add (Vector(121.57730309682418,40.622809428483734, 0.0));
    positionAlloc4->Add (Vector(294.79386010719867,240.9195284648913, 0.0));
    positionAlloc4->Add (Vector(111.28751952783999,53.24228039530626, 0.0));
    positionAlloc4->Add (Vector(382.81714045051837,0.10186100844300494, 0.0));
    positionAlloc4->Add (Vector(29.035393250107955,50.51736315658245, 0.0));
    positionAlloc4->Add (Vector(171.98876639763438,249.70409757035756, 0.0));
    positionAlloc4->Add (Vector(376.24163613067736,132.80118768567894, 0.0));
    positionAlloc4->Add (Vector(265.90727482792806,137.9892337858705, 0.0));
    positionAlloc4->Add (Vector(445.7203861678721,333.16329130824977, 0.0));
    positionAlloc4->Add (Vector(360.76085018679936,296.9304165400281, 0.0));
    positionAlloc4->Add (Vector(268.2452022389908,265.6093236660668, 0.0));
    positionAlloc4->Add (Vector(42.64519448738918,45.44859637821713, 0.0));
    positionAlloc4->Add (Vector(30.527904162517604,148.10395956480133, 0.0));
    positionAlloc4->Add (Vector(33.094473987709705,211.71154975241052, 0.0));
    positionAlloc4->Add (Vector(498.2719909445811,473.597551899364, 0.0));
    positionAlloc4->Add (Vector(252.1715961611034,46.12010804737293, 0.0));
    positionAlloc4->Add (Vector(105.90016127360452,10.415265568797905, 0.0));
    positionAlloc4->Add (Vector(418.72614728924736,93.90141246235339, 0.0));
    positionAlloc4->Add (Vector(442.91820878247995,133.51302713268421, 0.0));
    positionAlloc4->Add (Vector(218.33976283013263,337.57478341738863, 0.0));
    positionAlloc4->Add (Vector(345.87981184127784,171.75437843242648, 0.0));
    positionAlloc4->Add (Vector(51.99611353595179,383.99662046022235, 0.0));
    positionAlloc4->Add (Vector(361.3804879913601,367.3366291662638, 0.0));
    positionAlloc4->Add (Vector(475.49772773780904,284.325748127662, 0.0));
    positionAlloc4->Add (Vector(181.9239035916889,143.07525684706317, 0.0));
    positionAlloc4->Add (Vector(121.21659682263086,390.41166446135, 0.0));
    positionAlloc4->Add (Vector(291.196739684261,155.9619475244931, 0.0));
    positionAlloc4->Add (Vector(469.4854990413878,355.20379559918297, 0.0));
    positionAlloc4->Add (Vector(387.51576238011575,484.4931000500159, 0.0));
    positionAlloc4->Add (Vector(412.34269513754964,423.2034510329792, 0.0));
    positionAlloc4->Add (Vector(336.17369774662376,157.66401612930525, 0.0));
    positionAlloc4->Add (Vector(263.5053404738954,259.0549812396257, 0.0));
    positionAlloc4->Add (Vector(91.34345318895865,250.9524142404289, 0.0));
    positionAlloc4->Add (Vector(464.53609995801264,257.80018278578564, 0.0));
    positionAlloc4->Add (Vector(446.12365230092433,167.19737831588176, 0.0));
    positionAlloc4->Add (Vector(484.5700038803044,118.60355933488658, 0.0));
    positionAlloc4->Add (Vector(365.7059789124106,167.79651734521406, 0.0));
    positionAlloc4->Add (Vector(223.5250829359471,137.10201396260751, 0.0));
    positionAlloc4->Add (Vector(15.81470518162542,161.1115160048192, 0.0));
    positionAlloc4->Add (Vector(355.39523413303226,429.6394434443014, 0.0));
    positionAlloc4->Add (Vector(34.09465552453117,469.75066615526964, 0.0));
    positionAlloc4->Add (Vector(353.3341304156216,446.1862765570565, 0.0));
    positionAlloc4->Add (Vector(491.98415980511373,272.4223431689218, 0.0));
    positionAlloc4->Add (Vector(398.8389371543228,436.32299636497766, 0.0));
    positionAlloc4->Add (Vector(186.0327185573492,148.6392929301053, 0.0));
    positionAlloc4->Add (Vector(184.36438137539307,342.77158982673006, 0.0));
    positionAlloc4->Add (Vector(279.9632378973211,132.99734282254605, 0.0));
    positionAlloc4->Add (Vector(234.42576460145193,447.02097532483305, 0.0));
    positionAlloc4->Add (Vector(61.23848398312959,300.1349244080867, 0.0));
    positionAlloc4->Add (Vector(390.5632798562832,496.4409673829288, 0.0));
    positionAlloc4->Add (Vector(1.0852605761192957,16.03932469130498, 0.0));
    positionAlloc4->Add (Vector(3.714950646147741,96.73576457972594, 0.0));
    positionAlloc4->Add (Vector(47.30160308832476,245.14296160752818, 0.0));
    positionAlloc4->Add (Vector(392.7636165629041,221.80067889997457, 0.0));
    positionAlloc4->Add (Vector(229.40301866635176,436.1523528726454, 0.0));
    positionAlloc4->Add (Vector(114.02803422036972,475.0275269336313, 0.0));
    positionAlloc4->Add (Vector(284.1982849724619,268.0366966285009, 0.0));
    positionAlloc4->Add (Vector(174.8288039180098,278.12081919582926, 0.0));
    positionAlloc4->Add (Vector(269.2530327923046,47.42190886438502, 0.0));
    positionAlloc4->Add (Vector(469.48200285273623,374.79126208612826, 0.0));
    positionAlloc4->Add (Vector(159.69679578097734,111.65761095070748, 0.0));
    positionAlloc4->Add (Vector(381.18921508115886,197.62828567896057, 0.0));
    positionAlloc4->Add (Vector(21.997007647824308,190.15929531145937, 0.0));
    positionAlloc4->Add (Vector(282.20523487359446,49.38937672834803, 0.0));
    positionAlloc4->Add (Vector(468.4109084310709,77.04473499479498, 0.0));
    positionAlloc4->Add (Vector(84.6084763594912,242.69440522051744, 0.0));
    positionAlloc4->Add (Vector(447.6851861196104,306.4586101065386, 0.0));
    positionAlloc4->Add (Vector(376.3159704695837,85.29299877872154, 0.0));
    positionAlloc4->Add (Vector(167.5594067269972,37.64285608631474, 0.0));
    positionAlloc4->Add (Vector(212.69115414319862,80.20486736105204, 0.0));
    positionAlloc4->Add (Vector(356.74294978312344,326.6567362652711, 0.0));
    positionAlloc4->Add (Vector(114.1238936059692,330.755113649103, 0.0));
    positionAlloc4->Add (Vector(367.8198027403148,269.71929113510225, 0.0));
    positionAlloc4->Add (Vector(199.64327770880956,4.464839708018086, 0.0));
    positionAlloc4->Add (Vector(411.6605321476349,296.31485561333017, 0.0));
    positionAlloc4->Add (Vector(426.5155792832709,336.3905161970685, 0.0));
    positionAlloc4->Add (Vector(370.8208545322724,215.56421502912332, 0.0));
    positionAlloc4->Add (Vector(132.82134721041066,166.3274202812155, 0.0));
    positionAlloc4->Add (Vector(19.035977658343196,67.91077318107803, 0.0));
    positionAlloc4->Add (Vector(194.8151123033534,324.94755527920665, 0.0));
    positionAlloc4->Add (Vector(350.3590930705069,90.76307187139288, 0.0));
    positionAlloc4->Add (Vector(405.93382521504685,414.7703917066581, 0.0));
    positionAlloc4->Add (Vector(106.12066842197432,204.69095052237296, 0.0));
    positionAlloc4->Add (Vector(308.418613485134,293.75610645533385, 0.0));
    positionAlloc4->Add (Vector(33.78997059392241,290.05238144842684, 0.0));
    positionAlloc4->Add (Vector(304.8014122143444,149.3602877935179, 0.0));
    positionAlloc4->Add (Vector(441.0063130121429,301.371413080675, 0.0));
    positionAlloc4->Add (Vector(224.4984758843741,159.24447784261707, 0.0));
    positionAlloc4->Add (Vector(337.72541160738615,409.63347284143333, 0.0));
    positionAlloc4->Add (Vector(56.30524781778862,253.1489263541804, 0.0));
    positionAlloc4->Add (Vector(4.578635235196082,165.0152792216027, 0.0));
    positionAlloc4->Add (Vector(376.11926320399317,497.56647371459246, 0.0));
    positionAlloc4->Add (Vector(453.5328653541756,74.91625620465936, 0.0));
    positionAlloc4->Add (Vector(498.73914398966923,321.920477966252, 0.0));
    positionAlloc4->Add (Vector(327.00084673689395,232.7918655839496, 0.0));
    positionAlloc4->Add (Vector(69.9415804230955,178.1686938964564, 0.0));
    positionAlloc4->Add (Vector(281.2061845656515,106.59891586348552, 0.0));
    positionAlloc4->Add (Vector(279.88619297234777,307.5108618628316, 0.0));
    positionAlloc4->Add (Vector(311.1798014144972,449.5734001480146, 0.0));
    positionAlloc4->Add (Vector(332.38215891998067,475.1757061477728, 0.0));
    positionAlloc4->Add (Vector(60.37555538090367,406.35634657970877, 0.0));
    positionAlloc4->Add (Vector(67.96430708432821,392.25825903417535, 0.0));
    positionAlloc4->Add (Vector(452.38681406055866,312.78688495584737, 0.0));
    positionAlloc4->Add (Vector(136.73886933577322,450.79069361197605, 0.0));
    positionAlloc4->Add (Vector(240.44086304988477,62.05925338498158, 0.0));
    positionAlloc4->Add (Vector(24.41276229426559,94.74901468401592, 0.0));
    positionAlloc4->Add (Vector(342.973614545434,380.5561181752802, 0.0));
    positionAlloc4->Add (Vector(226.21820906877255,150.61408774609953, 0.0));
    positionAlloc4->Add (Vector(276.58097688942513,474.301119940887, 0.0));
    positionAlloc4->Add (Vector(282.52986921173766,466.52377405664464, 0.0));
    positionAlloc4->Add (Vector(208.1458753457096,303.4805709811305, 0.0));
    positionAlloc4->Add (Vector(159.4791440757602,467.4904504462224, 0.0));
    positionAlloc4->Add (Vector(460.7604969994157,312.2632131950911, 0.0));
    positionAlloc4->Add (Vector(238.51726996611345,192.54708931735144, 0.0));
    positionAlloc4->Add (Vector(27.8643178953934,329.8436018826134, 0.0));
    positionAlloc4->Add (Vector(250.64723742200695,68.4881388041747, 0.0));
    positionAlloc4->Add (Vector(354.4713798196731,47.17739278561195, 0.0));
    positionAlloc4->Add (Vector(169.7621102370252,101.09473713107941, 0.0));
    positionAlloc4->Add (Vector(388.2227151785251,368.4816410852427, 0.0));
    positionAlloc4->Add (Vector(132.47082184265946,370.3697146341235, 0.0));
    positionAlloc4->Add (Vector(24.158597780915613,304.3563994336732, 0.0));
    positionAlloc4->Add (Vector(337.94471252162873,467.46252473926864, 0.0));
    positionAlloc4->Add (Vector(131.08717960632222,448.4030062466486, 0.0));
    positionAlloc4->Add (Vector(36.91501660711333,413.5817259360374, 0.0));
    positionAlloc4->Add (Vector(482.0518287842171,90.1423061930981, 0.0));
    positionAlloc4->Add (Vector(170.1511188790365,316.52888053683944, 0.0));
    positionAlloc4->Add (Vector(448.66929779021405,146.10861398707365, 0.0));
    positionAlloc4->Add (Vector(490.808458859558,391.4632462592021, 0.0));
    positionAlloc4->Add (Vector(426.27752666811284,430.9952026742343, 0.0));
    positionAlloc4->Add (Vector(296.7933784008399,464.6426831008442, 0.0));
    positionAlloc4->Add (Vector(302.16449918570953,289.89007920915003, 0.0));
    positionAlloc4->Add (Vector(389.170054771865,404.73860583471196, 0.0));
    positionAlloc4->Add (Vector(348.06560168450153,457.6712107288939, 0.0));
    positionAlloc4->Add (Vector(69.55283744414947,337.87498839137675, 0.0));
    positionAlloc4->Add (Vector(26.846664043792046,439.1392270089167, 0.0));
    positionAlloc4->Add (Vector(197.15269233532206,259.9115309429581, 0.0));
    positionAlloc4->Add (Vector(285.79010881326667,429.32416155280606, 0.0));
    positionAlloc4->Add (Vector(282.00778132100976,218.54358785930677, 0.0));
    positionAlloc4->Add (Vector(391.3675680414927,173.13917413174747, 0.0));
    positionAlloc4->Add (Vector(276.7492095969521,163.15509102749866, 0.0));
    positionAlloc4->Add (Vector(94.35076788728009,316.31755114082966, 0.0));
    positionAlloc4->Add (Vector(78.7427084099167,248.25517528607983, 0.0));
    positionAlloc4->Add (Vector(431.7063871606905,403.9453380977324, 0.0));
    positionAlloc4->Add (Vector(177.4646815806596,117.5198613753931, 0.0));
    positionAlloc4->Add (Vector(28.734588069181388,343.05821519238765, 0.0));
    positionAlloc4->Add (Vector(289.78071932891356,178.02355962961963, 0.0));
    positionAlloc4->Add (Vector(389.18078045959567,402.41607400063367, 0.0));
    positionAlloc4->Add (Vector(283.50096688870786,402.9519990639885, 0.0));
    positionAlloc4->Add (Vector(93.89387765899554,228.8137816142486, 0.0));
    positionAlloc4->Add (Vector(74.29505809290704,421.96770999787196, 0.0));
    positionAlloc4->Add (Vector(38.13561102689478,265.28635085970984, 0.0));
    positionAlloc4->Add (Vector(103.591967934623,163.41894850210164, 0.0));
    positionAlloc4->Add (Vector(280.12398757916907,469.2310148668935, 0.0));
    positionAlloc4->Add (Vector(281.46076950196147,146.78535489474564, 0.0));
    positionAlloc4->Add (Vector(497.6511178254781,114.80959312666845, 0.0));
    positionAlloc4->Add (Vector(70.24053899382682,463.04447897113477, 0.0));
    positionAlloc4->Add (Vector(166.7603980755181,338.7218700191286, 0.0));
    positionAlloc4->Add (Vector(427.01684041126873,395.26522824263833, 0.0));
    positionAlloc4->Add (Vector(466.32909950936255,242.86402380134038, 0.0));
    positionAlloc4->Add (Vector(183.16666336733638,294.5141900612068, 0.0));
    positionAlloc4->Add (Vector(119.24052097887427,292.84409648771384, 0.0));
    positionAlloc4->Add (Vector(224.27375997592648,225.9711531068234, 0.0));
    positionAlloc4->Add (Vector(168.9284593903987,458.8742400664197, 0.0));
    positionAlloc4->Add (Vector(242.1506443985869,366.7236900024403, 0.0));
    positionAlloc4->Add (Vector(499.2873905743051,129.36208158326846, 0.0));
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
	  flowmon->SerializeToXmlFile ("scratch/switch_SA_flowmon/switch_SA19.flowmon", false, false);
	 //FIM FLOW MONITOR
		Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (serverApps.Get (0));
		std::cout << "Bytes received by server 1: " << sink1->GetTotalRx () << " ("
							<< (8. * sink1->GetTotalRx ()) / 1000000 / simTime << " Mbps)"
							<< std::endl;
	 return 0;
	}