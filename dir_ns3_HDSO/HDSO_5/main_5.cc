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
	  uint16_t numberOfRrhs = 9;
	//AUTOCODE NUMBEROFRRHS FIM
	//uint16_t numberOfNodes[19] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50,45,60,50};
	//uint16_t backNodes[19] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
	//uint16_t numberOfNodes[19] = {7,4,5,4,6,5,6,6,6,4,6,6,5,4,4,5,4,6,5};
	//uint16_t numberOfNodes[7] = {7,4,5,4,6,5,6};
	//AUTOCODE NUMBEROFUSERS INICIO
	  uint16_t numberOfNodes = 140;
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
    positionAlloc->Add (Vector (333.3333333333333,500.0, 0.0));
    positionAlloc->Add (Vector (333.3333333333333,166.66666666666666, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,333.3333333333333, 0.0));
    positionAlloc->Add (Vector (166.66666666666666,0.0, 0.0));
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
    positionAlloc4->Add (Vector(116.38760860150077,246.12240219933463, 0.0));
    positionAlloc4->Add (Vector(304.31079927120373,162.22789339452498, 0.0));
    positionAlloc4->Add (Vector(307.08542253497865,360.10252496330844, 0.0));
    positionAlloc4->Add (Vector(379.679453876645,29.032358498366385, 0.0));
    positionAlloc4->Add (Vector(495.53680397744614,332.86717076997155, 0.0));
    positionAlloc4->Add (Vector(27.55324760641542,494.48651638585875, 0.0));
    positionAlloc4->Add (Vector(179.0395558918405,182.06411143356377, 0.0));
    positionAlloc4->Add (Vector(212.40274982610464,222.0178337999509, 0.0));
    positionAlloc4->Add (Vector(41.26882562064399,332.12342496986076, 0.0));
    positionAlloc4->Add (Vector(106.89999649983494,294.0596688634507, 0.0));
    positionAlloc4->Add (Vector(123.81872367291041,42.23052497095053, 0.0));
    positionAlloc4->Add (Vector(426.27774261575905,270.9217090011439, 0.0));
    positionAlloc4->Add (Vector(425.42176274482574,321.6597613631499, 0.0));
    positionAlloc4->Add (Vector(198.01190368342796,129.2376786427366, 0.0));
    positionAlloc4->Add (Vector(14.91730183096468,42.875391262285234, 0.0));
    positionAlloc4->Add (Vector(182.7119284754013,26.30087849664603, 0.0));
    positionAlloc4->Add (Vector(297.8980701106112,482.7338806925533, 0.0));
    positionAlloc4->Add (Vector(317.96298878862365,435.6501918712423, 0.0));
    positionAlloc4->Add (Vector(451.73124544811344,314.05821565702996, 0.0));
    positionAlloc4->Add (Vector(358.40492913416955,493.12016145104263, 0.0));
    positionAlloc4->Add (Vector(5.6700628741055965,408.3305245082891, 0.0));
    positionAlloc4->Add (Vector(461.6251855886936,136.7449421594591, 0.0));
    positionAlloc4->Add (Vector(22.17469835382857,410.938839354011, 0.0));
    positionAlloc4->Add (Vector(246.88185829774707,108.39947767912118, 0.0));
    positionAlloc4->Add (Vector(330.2243535040141,184.333729249045, 0.0));
    positionAlloc4->Add (Vector(137.2992891738752,469.1092871704677, 0.0));
    positionAlloc4->Add (Vector(48.206417692852774,498.1739100168322, 0.0));
    positionAlloc4->Add (Vector(432.1033416004491,439.6694205546336, 0.0));
    positionAlloc4->Add (Vector(132.16590300617597,272.7853272054748, 0.0));
    positionAlloc4->Add (Vector(319.3078694873676,63.938239461674485, 0.0));
    positionAlloc4->Add (Vector(412.8859357521283,246.07341058634924, 0.0));
    positionAlloc4->Add (Vector(201.13669415886937,58.52443505242566, 0.0));
    positionAlloc4->Add (Vector(317.68080599225834,112.03035380169845, 0.0));
    positionAlloc4->Add (Vector(373.8079779398117,116.13937645967192, 0.0));
    positionAlloc4->Add (Vector(180.05951491537553,462.3054660644168, 0.0));
    positionAlloc4->Add (Vector(56.330006149723054,7.962534230346153, 0.0));
    positionAlloc4->Add (Vector(59.04247106574579,204.69199314701663, 0.0));
    positionAlloc4->Add (Vector(214.08799134831202,285.68465102275644, 0.0));
    positionAlloc4->Add (Vector(470.74368241625274,37.030163555106185, 0.0));
    positionAlloc4->Add (Vector(460.0126319971914,292.2819009516632, 0.0));
    positionAlloc4->Add (Vector(178.7696002136347,215.77196978058632, 0.0));
    positionAlloc4->Add (Vector(196.05824522299577,307.87689205670966, 0.0));
    positionAlloc4->Add (Vector(90.29295268422499,133.48832780992416, 0.0));
    positionAlloc4->Add (Vector(328.7219364313037,420.2296170748857, 0.0));
    positionAlloc4->Add (Vector(102.49720137506202,366.7659405095916, 0.0));
    positionAlloc4->Add (Vector(288.7516746117841,4.266017044854797, 0.0));
    positionAlloc4->Add (Vector(36.01737194684418,264.2979958276648, 0.0));
    positionAlloc4->Add (Vector(425.1060743281699,61.14138649636974, 0.0));
    positionAlloc4->Add (Vector(215.14698161538175,445.14533502925036, 0.0));
    positionAlloc4->Add (Vector(119.83525839507936,293.47252669991695, 0.0));
    positionAlloc4->Add (Vector(495.33261898086187,326.66108406738704, 0.0));
    positionAlloc4->Add (Vector(421.34416115958265,107.9283725777745, 0.0));
    positionAlloc4->Add (Vector(363.32941168592544,6.07299632502345, 0.0));
    positionAlloc4->Add (Vector(401.4405298013981,215.18233633962691, 0.0));
    positionAlloc4->Add (Vector(248.87743073449153,22.140775285005688, 0.0));
    positionAlloc4->Add (Vector(106.23913199212592,267.94817357901434, 0.0));
    positionAlloc4->Add (Vector(443.8060516284079,121.89099001133286, 0.0));
    positionAlloc4->Add (Vector(58.46355306165529,251.79998030735297, 0.0));
    positionAlloc4->Add (Vector(332.58301471057746,252.1094312910182, 0.0));
    positionAlloc4->Add (Vector(403.5446097411446,28.401673214264456, 0.0));
    positionAlloc4->Add (Vector(486.44557797762127,196.087039382974, 0.0));
    positionAlloc4->Add (Vector(75.94828536488363,98.6824750239263, 0.0));
    positionAlloc4->Add (Vector(242.3471352808475,153.0903926086188, 0.0));
    positionAlloc4->Add (Vector(43.38256958735948,419.6100483257741, 0.0));
    positionAlloc4->Add (Vector(113.04689191772171,220.46167875669965, 0.0));
    positionAlloc4->Add (Vector(139.73899719519773,496.8801237785132, 0.0));
    positionAlloc4->Add (Vector(344.90045873597467,427.39319400034987, 0.0));
    positionAlloc4->Add (Vector(200.98834148426664,300.99113797171304, 0.0));
    positionAlloc4->Add (Vector(185.37672662323607,411.69496285295867, 0.0));
    positionAlloc4->Add (Vector(54.95561443128777,440.73848589285905, 0.0));
    positionAlloc4->Add (Vector(403.4087888082833,277.38058485255027, 0.0));
    positionAlloc4->Add (Vector(325.03998169780306,220.12173593851136, 0.0));
    positionAlloc4->Add (Vector(422.501117436643,123.2830915437143, 0.0));
    positionAlloc4->Add (Vector(316.7138752910844,381.88903296751675, 0.0));
    positionAlloc4->Add (Vector(199.79994008886086,372.0805883458244, 0.0));
    positionAlloc4->Add (Vector(311.0687760879502,275.9280150440038, 0.0));
    positionAlloc4->Add (Vector(102.95847578970768,116.04705817863187, 0.0));
    positionAlloc4->Add (Vector(14.173308033312571,289.5522905066338, 0.0));
    positionAlloc4->Add (Vector(473.4059077944834,271.1488780219572, 0.0));
    positionAlloc4->Add (Vector(156.49173600087724,358.7206302889221, 0.0));
    positionAlloc4->Add (Vector(464.1629019230532,95.45169199094666, 0.0));
    positionAlloc4->Add (Vector(213.2476757363278,194.4702956462867, 0.0));
    positionAlloc4->Add (Vector(18.20737656985366,81.6034027858164, 0.0));
    positionAlloc4->Add (Vector(320.5930631475901,407.7678183363919, 0.0));
    positionAlloc4->Add (Vector(415.22357412830814,113.27600344413119, 0.0));
    positionAlloc4->Add (Vector(342.5033761954813,230.150173238446, 0.0));
    positionAlloc4->Add (Vector(353.6373989997705,142.50109170518655, 0.0));
    positionAlloc4->Add (Vector(361.8387415599238,408.29212144971, 0.0));
    positionAlloc4->Add (Vector(305.9147997730871,314.87170833362075, 0.0));
    positionAlloc4->Add (Vector(442.3565571966318,224.5831251769409, 0.0));
    positionAlloc4->Add (Vector(441.8016792724221,22.94166097951433, 0.0));
    positionAlloc4->Add (Vector(420.6495515558674,34.23118013181009, 0.0));
    positionAlloc4->Add (Vector(38.56756264386069,57.55887569312607, 0.0));
    positionAlloc4->Add (Vector(74.95779897026755,35.78337845294599, 0.0));
    positionAlloc4->Add (Vector(8.496332608472324,382.85409907033824, 0.0));
    positionAlloc4->Add (Vector(13.742426009569764,282.6143350360337, 0.0));
    positionAlloc4->Add (Vector(395.6216924366636,192.32811382575397, 0.0));
    positionAlloc4->Add (Vector(224.792519453749,429.79750566372655, 0.0));
    positionAlloc4->Add (Vector(267.9106025419909,77.9246747080417, 0.0));
    positionAlloc4->Add (Vector(37.50542277072599,335.98148471100615, 0.0));
    positionAlloc4->Add (Vector(48.93018753508538,62.37409581219422, 0.0));
    positionAlloc4->Add (Vector(441.79000454666397,360.0259693688519, 0.0));
    positionAlloc4->Add (Vector(117.54775737171269,153.1316209143902, 0.0));
    positionAlloc4->Add (Vector(286.9421438430635,166.12225354601907, 0.0));
    positionAlloc4->Add (Vector(291.2452387905961,299.57748939734336, 0.0));
    positionAlloc4->Add (Vector(441.2245134692415,212.58752524647912, 0.0));
    positionAlloc4->Add (Vector(319.0660899003273,258.9744809125728, 0.0));
    positionAlloc4->Add (Vector(38.80078662140757,123.56692743253706, 0.0));
    positionAlloc4->Add (Vector(361.17033464570903,286.9072412856575, 0.0));
    positionAlloc4->Add (Vector(336.3647578245302,57.95228330291796, 0.0));
    positionAlloc4->Add (Vector(13.78179893272552,441.3463537889787, 0.0));
    positionAlloc4->Add (Vector(219.33833994409375,322.15860899890544, 0.0));
    positionAlloc4->Add (Vector(193.78440392024416,24.302701272498062, 0.0));
    positionAlloc4->Add (Vector(197.710604248031,269.78280504515953, 0.0));
    positionAlloc4->Add (Vector(231.75161845802018,68.28417579281964, 0.0));
    positionAlloc4->Add (Vector(395.1278445592743,483.22409652291833, 0.0));
    positionAlloc4->Add (Vector(106.81815504203618,202.3049630929728, 0.0));
    positionAlloc4->Add (Vector(368.5921083010309,153.955145956779, 0.0));
    positionAlloc4->Add (Vector(349.5266187786538,246.413598651338, 0.0));
    positionAlloc4->Add (Vector(362.6664901372505,355.9729991806549, 0.0));
    positionAlloc4->Add (Vector(359.3642309492171,474.67139116282203, 0.0));
    positionAlloc4->Add (Vector(87.42652011538854,38.35775538428271, 0.0));
    positionAlloc4->Add (Vector(420.9666552242542,496.4107587563252, 0.0));
    positionAlloc4->Add (Vector(163.90151063727322,417.928942181965, 0.0));
    positionAlloc4->Add (Vector(333.54015043806794,471.39521775443154, 0.0));
    positionAlloc4->Add (Vector(211.1705618864447,28.052943336711323, 0.0));
    positionAlloc4->Add (Vector(80.80616919448636,421.5434326953094, 0.0));
    positionAlloc4->Add (Vector(231.91713102937038,85.44547271247299, 0.0));
    positionAlloc4->Add (Vector(114.32135853841025,272.1233382757292, 0.0));
    positionAlloc4->Add (Vector(33.875892301394316,28.563315766498178, 0.0));
    positionAlloc4->Add (Vector(106.45273423455909,64.07878279590035, 0.0));
    positionAlloc4->Add (Vector(81.73387812211442,131.27550935001508, 0.0));
    positionAlloc4->Add (Vector(77.90148690176302,357.7570943451213, 0.0));
    positionAlloc4->Add (Vector(221.04675195446072,42.444233864458006, 0.0));
    positionAlloc4->Add (Vector(278.9579070762613,305.3180307880471, 0.0));
    positionAlloc4->Add (Vector(221.92507350872003,352.83562739399474, 0.0));
    positionAlloc4->Add (Vector(394.0304618998596,114.41393109011533, 0.0));
    positionAlloc4->Add (Vector(420.51183147169667,218.78970922771308, 0.0));
    positionAlloc4->Add (Vector(231.01605839661366,107.41627865865262, 0.0));
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
	  flowmon->SerializeToXmlFile ("scratch/switch_SA_flowmon/switch_SA5.flowmon", false, false);
	 //FIM FLOW MONITOR
		Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (serverApps.Get (0));
		std::cout << "Bytes received by server 1: " << sink1->GetTotalRx () << " ("
							<< (8. * sink1->GetTotalRx ()) / 1000000 / simTime << " Mbps)"
							<< std::endl;
	 return 0;
	}