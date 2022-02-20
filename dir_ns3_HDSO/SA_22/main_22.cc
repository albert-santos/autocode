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
	  uint16_t numberOfNodes = 291;
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
    positionAlloc->Add (Vector (333.3333333333333,0.0, 0.0));
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
    positionAlloc4->Add (Vector(136.66444835158947,285.34739640557154, 0.0));
    positionAlloc4->Add (Vector(427.9505940958017,203.6984856797227, 0.0));
    positionAlloc4->Add (Vector(415.7448339126583,476.5217994232141, 0.0));
    positionAlloc4->Add (Vector(319.56932953999603,493.3334784421559, 0.0));
    positionAlloc4->Add (Vector(476.3849645348063,437.69307522246675, 0.0));
    positionAlloc4->Add (Vector(393.78747956134094,118.37137552060823, 0.0));
    positionAlloc4->Add (Vector(474.81111620972115,386.1228636698359, 0.0));
    positionAlloc4->Add (Vector(410.2229488609297,157.79343371071013, 0.0));
    positionAlloc4->Add (Vector(70.80406793241184,453.68733441871575, 0.0));
    positionAlloc4->Add (Vector(364.0221416513719,127.66494224640552, 0.0));
    positionAlloc4->Add (Vector(457.3072399880156,491.6815331341474, 0.0));
    positionAlloc4->Add (Vector(475.6993352941154,219.09813827323848, 0.0));
    positionAlloc4->Add (Vector(307.27657655638234,189.95407699681016, 0.0));
    positionAlloc4->Add (Vector(289.8992520318046,103.92340705698821, 0.0));
    positionAlloc4->Add (Vector(136.71414315318614,293.16378860237, 0.0));
    positionAlloc4->Add (Vector(383.53005353848584,188.4362448959492, 0.0));
    positionAlloc4->Add (Vector(451.8334615133069,129.26085081201128, 0.0));
    positionAlloc4->Add (Vector(169.44548691149697,223.4169545885995, 0.0));
    positionAlloc4->Add (Vector(20.821283200601115,245.6223239329524, 0.0));
    positionAlloc4->Add (Vector(331.0180826277555,316.2130624484918, 0.0));
    positionAlloc4->Add (Vector(377.87297219252287,285.087751819325, 0.0));
    positionAlloc4->Add (Vector(453.5317597474575,316.14273512519895, 0.0));
    positionAlloc4->Add (Vector(314.5552544870931,57.332261953674966, 0.0));
    positionAlloc4->Add (Vector(447.1622332199386,365.6974863493651, 0.0));
    positionAlloc4->Add (Vector(156.0746986751222,143.17429764129153, 0.0));
    positionAlloc4->Add (Vector(56.80135242757256,260.60770310756635, 0.0));
    positionAlloc4->Add (Vector(4.465147382922197,224.38452985243268, 0.0));
    positionAlloc4->Add (Vector(225.23521211769366,165.1991866261061, 0.0));
    positionAlloc4->Add (Vector(198.49274459932104,330.02526170566074, 0.0));
    positionAlloc4->Add (Vector(481.8356278620344,241.27337387393405, 0.0));
    positionAlloc4->Add (Vector(148.0839280978037,224.81156451500866, 0.0));
    positionAlloc4->Add (Vector(297.137933795272,288.82674574562975, 0.0));
    positionAlloc4->Add (Vector(143.53154201919156,262.4359883482388, 0.0));
    positionAlloc4->Add (Vector(220.4577287419144,228.20155161566385, 0.0));
    positionAlloc4->Add (Vector(359.72152664676327,88.04830291194304, 0.0));
    positionAlloc4->Add (Vector(466.35797779845404,83.51957681718942, 0.0));
    positionAlloc4->Add (Vector(23.336592340430673,72.01619896221683, 0.0));
    positionAlloc4->Add (Vector(279.1792604912718,290.83991568130966, 0.0));
    positionAlloc4->Add (Vector(434.6480335987621,260.52764888978106, 0.0));
    positionAlloc4->Add (Vector(225.22636086839094,401.78010253310964, 0.0));
    positionAlloc4->Add (Vector(194.01482562635454,495.9433218535824, 0.0));
    positionAlloc4->Add (Vector(174.80148578098425,445.52743969614505, 0.0));
    positionAlloc4->Add (Vector(386.01357075172126,200.28665692926785, 0.0));
    positionAlloc4->Add (Vector(31.351033054593692,200.52643967596296, 0.0));
    positionAlloc4->Add (Vector(1.0167767067246758,340.8779299282086, 0.0));
    positionAlloc4->Add (Vector(190.35740772060623,81.93534125030894, 0.0));
    positionAlloc4->Add (Vector(339.3588038242812,246.14743965465925, 0.0));
    positionAlloc4->Add (Vector(440.34332743556735,313.8993713388406, 0.0));
    positionAlloc4->Add (Vector(28.46323929198463,58.18647873372274, 0.0));
    positionAlloc4->Add (Vector(328.7913055881138,215.97754523847746, 0.0));
    positionAlloc4->Add (Vector(376.27057058323544,336.94500844860454, 0.0));
    positionAlloc4->Add (Vector(341.69255834377526,353.8188325373179, 0.0));
    positionAlloc4->Add (Vector(316.68307027475436,89.98251843641219, 0.0));
    positionAlloc4->Add (Vector(31.815536652215705,424.79083571576746, 0.0));
    positionAlloc4->Add (Vector(127.97668045425937,210.52780500150953, 0.0));
    positionAlloc4->Add (Vector(186.10689127989326,378.7720995341435, 0.0));
    positionAlloc4->Add (Vector(474.41074818431287,182.95446313479496, 0.0));
    positionAlloc4->Add (Vector(428.6561426711447,100.1405851789029, 0.0));
    positionAlloc4->Add (Vector(477.9923517054447,208.76773617984844, 0.0));
    positionAlloc4->Add (Vector(231.12676406887078,291.8814727065799, 0.0));
    positionAlloc4->Add (Vector(321.5526901202766,107.2684239784793, 0.0));
    positionAlloc4->Add (Vector(482.9678237490294,397.48825358948517, 0.0));
    positionAlloc4->Add (Vector(454.87423165289454,236.68418588609592, 0.0));
    positionAlloc4->Add (Vector(339.115017623375,221.83302794490683, 0.0));
    positionAlloc4->Add (Vector(435.7487461797703,110.92192626929936, 0.0));
    positionAlloc4->Add (Vector(234.92170702230493,76.25649676773305, 0.0));
    positionAlloc4->Add (Vector(185.30375496223422,331.7500922697152, 0.0));
    positionAlloc4->Add (Vector(317.5628023673653,459.59325281891614, 0.0));
    positionAlloc4->Add (Vector(238.10440691950146,391.7721203506796, 0.0));
    positionAlloc4->Add (Vector(128.07338974165023,380.8314123620711, 0.0));
    positionAlloc4->Add (Vector(9.627483305197416,72.24577517889492, 0.0));
    positionAlloc4->Add (Vector(328.99566494510015,449.4603439483312, 0.0));
    positionAlloc4->Add (Vector(265.32567520499606,32.58701092990923, 0.0));
    positionAlloc4->Add (Vector(216.11238887830487,412.37874474810155, 0.0));
    positionAlloc4->Add (Vector(414.3772884557965,181.08766950501888, 0.0));
    positionAlloc4->Add (Vector(294.5825014452114,304.57775889406804, 0.0));
    positionAlloc4->Add (Vector(373.7911398947778,378.06715007827347, 0.0));
    positionAlloc4->Add (Vector(417.68657587230774,380.5654722693726, 0.0));
    positionAlloc4->Add (Vector(286.39323698434106,168.00100820584535, 0.0));
    positionAlloc4->Add (Vector(483.4805569905586,34.06635646423206, 0.0));
    positionAlloc4->Add (Vector(31.789712883014797,281.64925659939615, 0.0));
    positionAlloc4->Add (Vector(103.16960872274007,458.30171460317956, 0.0));
    positionAlloc4->Add (Vector(484.4201671452929,400.7829022411438, 0.0));
    positionAlloc4->Add (Vector(349.2882073939132,19.057627214386365, 0.0));
    positionAlloc4->Add (Vector(328.19703011522824,187.6508611440349, 0.0));
    positionAlloc4->Add (Vector(482.57497863338114,50.3917936030146, 0.0));
    positionAlloc4->Add (Vector(350.10893200059706,423.44353301371785, 0.0));
    positionAlloc4->Add (Vector(208.5358108935385,334.3042398940777, 0.0));
    positionAlloc4->Add (Vector(404.3145519927189,289.13149157115146, 0.0));
    positionAlloc4->Add (Vector(469.9131435186252,322.28342837447155, 0.0));
    positionAlloc4->Add (Vector(101.5046742710628,43.43996114415255, 0.0));
    positionAlloc4->Add (Vector(95.42601057271321,463.6569720978116, 0.0));
    positionAlloc4->Add (Vector(494.3479934023644,308.5242808212421, 0.0));
    positionAlloc4->Add (Vector(312.48434173857584,195.67098904936282, 0.0));
    positionAlloc4->Add (Vector(425.0205828056471,341.6499182914296, 0.0));
    positionAlloc4->Add (Vector(64.40860082356642,432.09384937120484, 0.0));
    positionAlloc4->Add (Vector(455.70045006100673,466.26342489990674, 0.0));
    positionAlloc4->Add (Vector(290.55829859497,139.85206957682234, 0.0));
    positionAlloc4->Add (Vector(354.23503322385494,21.980705961286652, 0.0));
    positionAlloc4->Add (Vector(144.88498736814958,127.17027227810141, 0.0));
    positionAlloc4->Add (Vector(83.47426767635501,467.0596587645968, 0.0));
    positionAlloc4->Add (Vector(332.25689805957654,216.17137130801189, 0.0));
    positionAlloc4->Add (Vector(435.50848600415884,176.5029581135175, 0.0));
    positionAlloc4->Add (Vector(375.88817129056446,411.28209727686345, 0.0));
    positionAlloc4->Add (Vector(196.31589236979906,467.61950204071906, 0.0));
    positionAlloc4->Add (Vector(74.9692164789773,323.905570507986, 0.0));
    positionAlloc4->Add (Vector(484.90673879947025,206.67556389107432, 0.0));
    positionAlloc4->Add (Vector(353.0052639796496,66.78523615804954, 0.0));
    positionAlloc4->Add (Vector(97.06690643121763,89.06218169472518, 0.0));
    positionAlloc4->Add (Vector(18.55036372032587,245.1659171320439, 0.0));
    positionAlloc4->Add (Vector(8.302532003858222,139.48129843742012, 0.0));
    positionAlloc4->Add (Vector(232.83667242458063,25.859158464727695, 0.0));
    positionAlloc4->Add (Vector(495.4326062413662,374.55138932457965, 0.0));
    positionAlloc4->Add (Vector(11.826234115840384,133.26083994051234, 0.0));
    positionAlloc4->Add (Vector(127.58777300759655,148.36314229281615, 0.0));
    positionAlloc4->Add (Vector(165.14634981127412,406.3237785065186, 0.0));
    positionAlloc4->Add (Vector(197.73655779635473,273.2476773068573, 0.0));
    positionAlloc4->Add (Vector(84.45311132332705,225.24226021423948, 0.0));
    positionAlloc4->Add (Vector(347.8600953659067,290.2327885188857, 0.0));
    positionAlloc4->Add (Vector(440.9478870536925,75.71522356689331, 0.0));
    positionAlloc4->Add (Vector(172.36690825072824,351.2960405402832, 0.0));
    positionAlloc4->Add (Vector(254.13738129495738,341.69416913896197, 0.0));
    positionAlloc4->Add (Vector(282.7283993182569,331.7620351434676, 0.0));
    positionAlloc4->Add (Vector(186.45571011445216,38.24102785048644, 0.0));
    positionAlloc4->Add (Vector(284.88687846968185,436.6847935396783, 0.0));
    positionAlloc4->Add (Vector(100.96408208232488,449.4030663599007, 0.0));
    positionAlloc4->Add (Vector(457.97493497427797,416.2516607910326, 0.0));
    positionAlloc4->Add (Vector(85.84682746287581,172.54566338369415, 0.0));
    positionAlloc4->Add (Vector(160.68330245550504,73.94955261533282, 0.0));
    positionAlloc4->Add (Vector(372.3654301630325,212.53204613484132, 0.0));
    positionAlloc4->Add (Vector(248.2546302453999,98.89246267831497, 0.0));
    positionAlloc4->Add (Vector(48.57867679674788,468.67947762687834, 0.0));
    positionAlloc4->Add (Vector(354.11007123440777,42.351274761027454, 0.0));
    positionAlloc4->Add (Vector(478.79912034057094,345.0477573545212, 0.0));
    positionAlloc4->Add (Vector(318.21429472883625,403.9206686459399, 0.0));
    positionAlloc4->Add (Vector(321.32641733853916,381.4930802033686, 0.0));
    positionAlloc4->Add (Vector(112.5012004270029,147.90236663031763, 0.0));
    positionAlloc4->Add (Vector(402.7788594001004,290.8404262603748, 0.0));
    positionAlloc4->Add (Vector(442.497932093772,357.99999383367873, 0.0));
    positionAlloc4->Add (Vector(149.49237586978893,306.1252166282104, 0.0));
    positionAlloc4->Add (Vector(478.44365302495805,127.3874946907459, 0.0));
    positionAlloc4->Add (Vector(22.472123917247956,2.0689461155886657, 0.0));
    positionAlloc4->Add (Vector(111.54211464511543,30.465423789881598, 0.0));
    positionAlloc4->Add (Vector(313.46146635142327,393.1550280721596, 0.0));
    positionAlloc4->Add (Vector(406.90956035257034,59.0286105553583, 0.0));
    positionAlloc4->Add (Vector(27.717878545548803,448.9692750042512, 0.0));
    positionAlloc4->Add (Vector(339.586644644416,295.7607325224069, 0.0));
    positionAlloc4->Add (Vector(267.09313686285725,252.5963339248068, 0.0));
    positionAlloc4->Add (Vector(13.596458990286331,381.71746816150994, 0.0));
    positionAlloc4->Add (Vector(440.0768691561049,440.64728035340454, 0.0));
    positionAlloc4->Add (Vector(374.6524554482288,67.1480485832579, 0.0));
    positionAlloc4->Add (Vector(228.21497155768787,407.68937507311824, 0.0));
    positionAlloc4->Add (Vector(406.8602589450413,498.4154838344836, 0.0));
    positionAlloc4->Add (Vector(16.600588156534645,53.27731478561276, 0.0));
    positionAlloc4->Add (Vector(388.9666634312337,190.400563491521, 0.0));
    positionAlloc4->Add (Vector(196.1692216571983,0.588334015620473, 0.0));
    positionAlloc4->Add (Vector(110.96790231913356,2.4851191885402746, 0.0));
    positionAlloc4->Add (Vector(377.88346529002183,403.18375334378914, 0.0));
    positionAlloc4->Add (Vector(223.52161096505068,195.68331975562376, 0.0));
    positionAlloc4->Add (Vector(28.13990749788814,215.52104325080396, 0.0));
    positionAlloc4->Add (Vector(231.3487325799171,42.590569007238784, 0.0));
    positionAlloc4->Add (Vector(384.65753259766,26.066753654660545, 0.0));
    positionAlloc4->Add (Vector(294.3152408451453,436.4073476068019, 0.0));
    positionAlloc4->Add (Vector(1.6785761292349433,448.1201381413377, 0.0));
    positionAlloc4->Add (Vector(343.74017208318475,431.6375349917967, 0.0));
    positionAlloc4->Add (Vector(7.289307705070581,441.56780261433863, 0.0));
    positionAlloc4->Add (Vector(213.761571119791,40.11418580176146, 0.0));
    positionAlloc4->Add (Vector(246.89335388185262,335.5408611530011, 0.0));
    positionAlloc4->Add (Vector(343.5015130995204,192.25242606679538, 0.0));
    positionAlloc4->Add (Vector(137.3851711127972,380.2900049899267, 0.0));
    positionAlloc4->Add (Vector(363.7298534079999,303.65132766552375, 0.0));
    positionAlloc4->Add (Vector(450.0992693783973,252.8972085902847, 0.0));
    positionAlloc4->Add (Vector(179.50554656878737,351.6447069512291, 0.0));
    positionAlloc4->Add (Vector(53.02441497068211,449.01865035427727, 0.0));
    positionAlloc4->Add (Vector(97.95353376388044,357.50806421793493, 0.0));
    positionAlloc4->Add (Vector(156.41273564052327,486.8538383011331, 0.0));
    positionAlloc4->Add (Vector(278.10674178266044,225.51515735168064, 0.0));
    positionAlloc4->Add (Vector(439.18204969946856,485.7289837578914, 0.0));
    positionAlloc4->Add (Vector(109.19089946645948,139.26163448589702, 0.0));
    positionAlloc4->Add (Vector(19.333342811043597,200.84318477943015, 0.0));
    positionAlloc4->Add (Vector(15.636840245879247,333.409431971794, 0.0));
    positionAlloc4->Add (Vector(440.3163734991492,61.315254682784925, 0.0));
    positionAlloc4->Add (Vector(334.3267648024713,79.79546770675233, 0.0));
    positionAlloc4->Add (Vector(265.95456412519127,336.7597454682692, 0.0));
    positionAlloc4->Add (Vector(392.87991383897133,252.01423164928534, 0.0));
    positionAlloc4->Add (Vector(82.25752719753655,430.98900016845977, 0.0));
    positionAlloc4->Add (Vector(397.89068049895906,298.5521370853739, 0.0));
    positionAlloc4->Add (Vector(307.8592125113693,421.9073071558107, 0.0));
    positionAlloc4->Add (Vector(13.825493426467993,168.39787680948547, 0.0));
    positionAlloc4->Add (Vector(325.18467516016256,52.57724397107455, 0.0));
    positionAlloc4->Add (Vector(398.12726746257226,371.8979858066562, 0.0));
    positionAlloc4->Add (Vector(222.7349970784286,484.73124624855353, 0.0));
    positionAlloc4->Add (Vector(443.97817740806,5.463582438289039, 0.0));
    positionAlloc4->Add (Vector(195.88739498680613,416.1802003603629, 0.0));
    positionAlloc4->Add (Vector(163.0481993149443,251.14712798447601, 0.0));
    positionAlloc4->Add (Vector(66.30024981467442,136.75503828045987, 0.0));
    positionAlloc4->Add (Vector(150.64904537199004,188.55040509218352, 0.0));
    positionAlloc4->Add (Vector(271.94966589201965,3.8730344903394887, 0.0));
    positionAlloc4->Add (Vector(129.71119005378173,152.01187464562273, 0.0));
    positionAlloc4->Add (Vector(227.3832767445837,44.59522575553881, 0.0));
    positionAlloc4->Add (Vector(85.61136463237729,32.88872494959227, 0.0));
    positionAlloc4->Add (Vector(432.4276427141245,19.868087198685547, 0.0));
    positionAlloc4->Add (Vector(180.7615151791763,133.77185157846012, 0.0));
    positionAlloc4->Add (Vector(406.81295277097905,34.209652427519664, 0.0));
    positionAlloc4->Add (Vector(174.20877379140194,42.19655762559932, 0.0));
    positionAlloc4->Add (Vector(60.7245934081142,36.71611458897506, 0.0));
    positionAlloc4->Add (Vector(64.70795374629812,222.25980852783172, 0.0));
    positionAlloc4->Add (Vector(43.61686436977408,282.18410472091284, 0.0));
    positionAlloc4->Add (Vector(203.27695052547418,400.70159155043666, 0.0));
    positionAlloc4->Add (Vector(374.2772494955434,220.52659554620234, 0.0));
    positionAlloc4->Add (Vector(460.8204219990011,141.7869107478283, 0.0));
    positionAlloc4->Add (Vector(257.9034475798841,362.7614989406633, 0.0));
    positionAlloc4->Add (Vector(143.54333364338996,113.29364887369331, 0.0));
    positionAlloc4->Add (Vector(359.9032210631084,139.7320198651575, 0.0));
    positionAlloc4->Add (Vector(211.99014266602035,305.4628508524598, 0.0));
    positionAlloc4->Add (Vector(176.9136268605544,255.81940759029715, 0.0));
    positionAlloc4->Add (Vector(92.57022062529069,99.07511001657177, 0.0));
    positionAlloc4->Add (Vector(74.21897944694605,443.7347584422588, 0.0));
    positionAlloc4->Add (Vector(423.7551212710762,19.477324494317994, 0.0));
    positionAlloc4->Add (Vector(165.1535169356797,107.8092085001654, 0.0));
    positionAlloc4->Add (Vector(428.59155527308127,274.1335062692292, 0.0));
    positionAlloc4->Add (Vector(257.05395963581225,168.7405463046124, 0.0));
    positionAlloc4->Add (Vector(106.88420077213118,356.36614108881207, 0.0));
    positionAlloc4->Add (Vector(179.81044172204741,360.15679434630437, 0.0));
    positionAlloc4->Add (Vector(127.5786222476239,101.75051601363172, 0.0));
    positionAlloc4->Add (Vector(214.24196409394463,338.21046000258224, 0.0));
    positionAlloc4->Add (Vector(275.299657127447,330.4332107553644, 0.0));
    positionAlloc4->Add (Vector(76.4243928995581,458.68849677556017, 0.0));
    positionAlloc4->Add (Vector(497.0127194780164,188.55416000673742, 0.0));
    positionAlloc4->Add (Vector(417.23255552535693,171.34448437182436, 0.0));
    positionAlloc4->Add (Vector(51.9256000017202,295.4757507186181, 0.0));
    positionAlloc4->Add (Vector(247.44992956304114,469.1380451112178, 0.0));
    positionAlloc4->Add (Vector(2.9015097448135685,412.09325449132047, 0.0));
    positionAlloc4->Add (Vector(420.7464675471508,498.1849099859447, 0.0));
    positionAlloc4->Add (Vector(0.935109170529913,152.4370215595229, 0.0));
    positionAlloc4->Add (Vector(43.25565723625857,50.91943928702092, 0.0));
    positionAlloc4->Add (Vector(323.840961988009,32.42722487518756, 0.0));
    positionAlloc4->Add (Vector(327.3156226751138,237.66457840335892, 0.0));
    positionAlloc4->Add (Vector(158.16818626395894,250.70384637509318, 0.0));
    positionAlloc4->Add (Vector(102.19606581203821,437.8543259741433, 0.0));
    positionAlloc4->Add (Vector(37.696314799126604,64.05712931064988, 0.0));
    positionAlloc4->Add (Vector(172.41103894060734,71.70053367575619, 0.0));
    positionAlloc4->Add (Vector(228.51884387624415,96.60804095238524, 0.0));
    positionAlloc4->Add (Vector(107.7938003381968,5.840991344535085, 0.0));
    positionAlloc4->Add (Vector(127.29894380791113,387.58417125089454, 0.0));
    positionAlloc4->Add (Vector(81.17947029264222,340.17979238566994, 0.0));
    positionAlloc4->Add (Vector(211.0545161891742,337.4336199649345, 0.0));
    positionAlloc4->Add (Vector(268.1730477648552,0.6965805850427942, 0.0));
    positionAlloc4->Add (Vector(449.9860788923046,63.198253332467644, 0.0));
    positionAlloc4->Add (Vector(314.4758189472357,204.29768954474898, 0.0));
    positionAlloc4->Add (Vector(431.70760069113567,10.267629245743425, 0.0));
    positionAlloc4->Add (Vector(306.71414193827457,267.0557099478004, 0.0));
    positionAlloc4->Add (Vector(58.81204634012127,38.29632599656435, 0.0));
    positionAlloc4->Add (Vector(127.11130670737259,119.85282640367274, 0.0));
    positionAlloc4->Add (Vector(356.1374271761511,182.04984747730134, 0.0));
    positionAlloc4->Add (Vector(440.45571489305183,267.862062179788, 0.0));
    positionAlloc4->Add (Vector(473.8214473711251,409.03752973092236, 0.0));
    positionAlloc4->Add (Vector(394.7096149798368,409.39978600208, 0.0));
    positionAlloc4->Add (Vector(52.726577543783684,326.0290748068563, 0.0));
    positionAlloc4->Add (Vector(79.82288696393769,47.5130526631507, 0.0));
    positionAlloc4->Add (Vector(306.6152986984585,256.76063711642774, 0.0));
    positionAlloc4->Add (Vector(351.78073741476123,440.5661216252963, 0.0));
    positionAlloc4->Add (Vector(182.60039672833324,160.05494017396614, 0.0));
    positionAlloc4->Add (Vector(312.7499229814373,348.2146923362381, 0.0));
    positionAlloc4->Add (Vector(115.6638062659352,301.24806161537384, 0.0));
    positionAlloc4->Add (Vector(304.91493046215646,242.68382429295326, 0.0));
    positionAlloc4->Add (Vector(329.8550500187088,432.25004952735463, 0.0));
    positionAlloc4->Add (Vector(4.038902628174745,493.54730462761745, 0.0));
    positionAlloc4->Add (Vector(431.4068113981689,389.66519663519534, 0.0));
    positionAlloc4->Add (Vector(460.7248855110813,385.2808547927672, 0.0));
    positionAlloc4->Add (Vector(257.39152110566187,81.76852317484362, 0.0));
    positionAlloc4->Add (Vector(339.14056910631547,252.18943285651847, 0.0));
    positionAlloc4->Add (Vector(387.87570224666473,270.0095720301653, 0.0));
    positionAlloc4->Add (Vector(470.3179627153829,385.42502077174225, 0.0));
    positionAlloc4->Add (Vector(219.87659706581297,269.2077696128298, 0.0));
    positionAlloc4->Add (Vector(123.5241286016916,119.67058610077707, 0.0));
    positionAlloc4->Add (Vector(283.9032268723199,33.7232993738058, 0.0));
    positionAlloc4->Add (Vector(231.99092987244697,360.02194800521045, 0.0));
    positionAlloc4->Add (Vector(282.25129963462274,65.08177001336368, 0.0));
    positionAlloc4->Add (Vector(278.31299455817356,478.0078981605475, 0.0));
    positionAlloc4->Add (Vector(10.552206642304274,432.8084091328545, 0.0));
    positionAlloc4->Add (Vector(424.94183866945957,114.70955012894474, 0.0));
    positionAlloc4->Add (Vector(175.19898043410487,136.26282672176697, 0.0));
    positionAlloc4->Add (Vector(299.9422841067801,469.17977751353254, 0.0));
    positionAlloc4->Add (Vector(86.57415846528733,97.34762497141448, 0.0));
    positionAlloc4->Add (Vector(341.69021759469166,286.96242562706215, 0.0));
    positionAlloc4->Add (Vector(154.15980345563025,233.83547797573794, 0.0));
    positionAlloc4->Add (Vector(283.9901860070855,386.55208458365155, 0.0));
    positionAlloc4->Add (Vector(136.1113487047774,106.2808178593263, 0.0));
    positionAlloc4->Add (Vector(466.32491869449177,497.83635192787455, 0.0));
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
	  flowmon->SerializeToXmlFile ("scratch/switch_SA_flowmon/switch_SA22.flowmon", false, false);
	 //FIM FLOW MONITOR
		Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (serverApps.Get (0));
		std::cout << "Bytes received by server 1: " << sink1->GetTotalRx () << " ("
							<< (8. * sink1->GetTotalRx ()) / 1000000 / simTime << " Mbps)"
							<< std::endl;
	 return 0;
	}