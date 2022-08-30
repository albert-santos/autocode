void
MyController::Allocation (std::map <uint64_t, Ipv4Address> mymap,std::map <uint64_t, uint64_t> mymap2)
{
  /*
  Metodo de alocação, este método toma como argumento dois objetos do tipo map,
  relacionando imsi a IP e imsi a cellid, respectivamente.
  O objetivo desta função é relacionar UE, ENB e BBU,
  através da construção de um map relacionando ip a bbu antes de começar o tráfego de dados.
  Alocação de teste:
  BBU 1 = 7,12
  BBU 2 = 3,6
  BBU 3 = 1,2
  BBU 4 = 13,14,17,18
  BBU 5 = 4,5,8,9
  BBU 6 = 10,11,15,16,19
*/
  static uint64_t swt1=1;
  static uint64_t swt2=2;
  m_mymap = mymap;
  m_mymap2 = mymap2;
  //AUTOCODE ALLOCATION INICIO
  
  



	uint32_t connect_RRH_1 = 0;
	uint32_t connect_RRH_2 = 0;
	uint32_t connect_RRH_3 = 0;

	uint32_t connect_bbu_1 = 0;
	uint32_t connect_bbu_2 = 0;
	uint32_t connect_bbu_3 = 0;
	uint32_t connect_bbu_4 = 0;
	uint32_t connect_bbu_5 = 0;
	uint32_t connect_bbu_6 = 0;

	for (std::map<uint64_t, uint64_t >::iterator it=m_mymap2.begin(); it!=m_mymap2.end(); ++it) 
	{

		//Associando cada antena a uma BBU de acordo com a aloca��o teste

		switch (it->second)
		{
			case 1:
				connect_RRH_1++;
				mymap3[m_mymap[it->first]]= 2;
				connect_bbu_2++;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado � BBU: 2"<<std::endl;
				break;
			case 2:
				connect_RRH_2++;
				mymap3[m_mymap[it->first]]= 6;
				connect_bbu_6++;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado � BBU: 6"<<std::endl;
				break;
			case 3:
				connect_RRH_3++;
				mymap3[m_mymap[it->first]]= 1;
				connect_bbu_1++;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado � BBU: 1"<<std::endl;
				break;
			default:
				mymap3[m_mymap[it->first]]= 7;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" n�o associado"<<std::endl;
				break;
		}
	}

	std::cout <<" MAPA 1: IMSI X IP" << std::endl;
	for (std::map<uint64_t, Ipv4Address>::iterator it=m_mymap.begin(); it!=m_mymap.end(); ++it)
		std::cout <<"imsi: "<< it->first << " ip address: " << it->second << std::endl;

	std::cout <<" MAPA 2: IMSI X RRH_ID"<< std::endl;
	for (std::map<uint64_t, uint64_t>::iterator it=m_mymap2.begin(); it!=m_mymap2.end(); ++it)
		std::cout <<"imsi: "<< it->first << " connected to cellid: " << it->second << std::endl; 

	std::cout <<" MAPA 3: IP X BBU"<< std::endl;
	for (std::map<Ipv4Address, uint64_t >::iterator it=mymap3.begin(); it!=mymap3.end(); ++it)
		std::cout <<"ip address: "<< it->first << " bbu: " << it->second << std::endl; 

	std::cout << " " << std::endl;
	std::cout <<" Qtd de Usu�rios por RRH"<< std::endl;
	std::cout <<"RRH 1: " << connect_RRH_1 << " usu�rios << std::endl";
	std::cout <<"RRH 2: " << connect_RRH_2 << " usu�rios << std::endl";
	std::cout <<"RRH 3: " << connect_RRH_3 << " usu�rios << std::endl";

	std::cout << " " << std::endl;
	std::cout <<"BBU 1: " << connect_bbu_1 << " usu�rios" << std::endl;
	std::cout <<"BBU 2: " << connect_bbu_2 << " usu�rios" << std::endl;
	std::cout <<"BBU 3: " << connect_bbu_3 << " usu�rios" << std::endl;
	std::cout <<"BBU 4: " << connect_bbu_4 << " usu�rios" << std::endl;
	std::cout <<"BBU 5: " << connect_bbu_5 << " usu�rios" << std::endl;
	std::cout <<"BBU 6: " << connect_bbu_6 << " usu�rios" << std::endl;

  //AUTOCODE ALLOCATION FIM

  for (std::map<Ipv4Address, uint64_t >::iterator it2=mymap3.begin(); it2!=mymap3.end(); ++it2)
  {
    std::ostringstream flowCmd;
    flowCmd << "flow-mod cmd=add,prio=100,table=0, eth_type=0x0800,"
            << "ip_src=" << it2->first
           << ",in_port=7 apply:output=" << it2->second;
    DpctlExecute (swt1, flowCmd.str ());
    std::ostringstream flowCmd2;
    flowCmd2 << "flow-mod cmd=add,prio=100,table=0, eth_type=0x0800,"
           << "ip_dst=" << it2->first
           << ",in_port=1 apply:output=2";
    DpctlExecute (swt2, flowCmd2.str ());
  }

}
void
MyController::Update (std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti)
{ 
  static uint64_t swt1=1;
  static uint64_t swt2=2;
  static uint64_t prio=100;
  prio++;

  std::cout << Simulator::Now ().GetSeconds () << " "
            << " UE IMSI " << imsi
            << ", realizou handover para rrh:  " << cellid
            << std::endl;

  //AUTOCODE UPDATE INICIO


	switch (cellid)
		{
			case 1:
				mymap3[m_mymap[imsi]]= 2;
				//std::cout<<"ip: "<<m_mymap[imsi]<<" associado � BBU: 2 (Handover)"<<std::endl;
				break;
			case 2:
				mymap3[m_mymap[imsi]]= 6;
				//std::cout<<"ip: "<<m_mymap[imsi]<<" associado � BBU: 6 (Handover)"<<std::endl;
				break;
			case 3:
				mymap3[m_mymap[imsi]]= 1;
				//std::cout<<"ip: "<<m_mymap[imsi]<<" associado � BBU: 1 (Handover)"<<std::endl;
				break;
			default:
				mymap3[m_mymap[imsi]]= 7;
				std::cout<<"ip: "<<m_mymap[imsi]<<" n�o associado"<<std::endl;
				break;
		}
    //AUTOCODE UPDATE FIM

  for (std::map<Ipv4Address, uint64_t >::iterator it2=mymap3.begin(); it2!=mymap3.end(); ++it2)
  {
    std::ostringstream flowCmd;
    flowCmd << "flow-mod cmd=add,prio="<<prio<<",table=0, eth_type=0x0800,"
            << "ip_src=" << it2->first
           << ",in_port=7 apply:output=" << it2->second;
    DpctlExecute (swt1, flowCmd.str ());
    std::ostringstream flowCmd2;
    flowCmd2 << "flow-mod cmd=add,prio="<<prio<<",table=0, eth_type=0x0800,"
           << "ip_dst=" << it2->first
           << ",in_port=1 apply:output=2";
    DpctlExecute (swt2, flowCmd2.str ());
  }

}
/********** Private methods **********/
void
MyController::HandshakeSuccessful (
  Ptr<const RemoteSwitch> swtch)
{
  NS_LOG_FUNCTION (this << swtch);

  // After a successfull handshake, let's install the table-miss entry, setting
  // to 128 bytes the maximum amount of data from a packet that should be sent
  // to the controller.
  DpctlExecute (swtch, "flow-mod cmd=add,table=0,prio=0 "
                "apply:output=ctrl:128");
  // Configure te switch to buffer packets and send only the first 128 bytes of
  // each packet sent to the controller when not using an output action to the
  // OFPP_CONTROLLER logical port.
  DpctlExecute (swtch, "set-config miss=128");

  // Create an empty L2SwitchingTable and insert it into m_learnedInfo
  L2Table_t l2Table;
  uint64_t dpId = swtch->GetDpId ();
  //dpctl unix:/tmp/ofd flow-mod cmd=add,table=0 eth_type=0x800,ip_src=192.168.0.1,ip_dst=172.40.56.101/24 apply:nw_dec,output=3
  //dpctl unix:/tmp/ofd flow-mod table=0,cmd=add  eth_type=0x806,arp_sha=00:00:00:00:00:01,arp_tha=00:00:00:00:00:02,
  //arp_spa=192.168.0.1,arp_tpa=192.168.0.2,arp_op=1 apply:output=3
  //Topswitch==2
  //mainswitch==1
  if(dpId==1){
    //std::cout<<"dpId1"<<std::endl;
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x800,ip_src=1.0.0.3,"
                "in_port=7 apply:output=1");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x806,arp_spa=1.0.0.3,"
                "in_port=7 apply:output=1");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, "
                "in_port=1 apply:output=7");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, "
                "in_port=2 apply:output=7");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, "
                "in_port=3 apply:output=7");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, "
                "in_port=4 apply:output=7");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, "
                "in_port=5 apply:output=7");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, "
                "in_port=6 apply:output=7");
   }
  if(dpId==2){
    //std::cout<<"dpId2"<<std::endl;
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x800,ip_dst=1.0.0.3,"
                "in_port=1 apply:output=2");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x806,arp_tpa=1.0.0.3,"
                "in_port=1 apply:output=2");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, "
                "in_port=2 apply:output=1");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, "
                "in_port=3 apply:output=1");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, "
                "in_port=4 apply:output=1");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, "
                "in_port=5 apply:output=1");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, "
                "in_port=6 apply:output=1");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, "
                "in_port=7 apply:output=1");
   }
   if(dpId==3){
    //std::cout<<"dpId3"<<std::endl;
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x800,"
                "in_port=1 apply:output=2");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x800,"
                "in_port=2 apply:output=1");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x806,"
                "in_port=1 apply:output=2");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x806,"
                "in_port=2 apply:output=1");
   }
   if(dpId==4){
    //std::cout<<"dpId4"<<std::endl;
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x800,"
                "in_port=1 apply:output=2");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x800,"
                "in_port=2 apply:output=1");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x806,"
                "in_port=1 apply:output=2");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x806,"
                "in_port=2 apply:output=1");
   }
   if(dpId==5){
    //std::cout<<"dpId5"<<std::endl;
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x800,"
                "in_port=1 apply:output=2");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x800,"
                "in_port=2 apply:output=1");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x806,"
                "in_port=1 apply:output=2");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x806,"
                "in_port=2 apply:output=1");
   }
   if(dpId==6){
    //std::cout<<"dpId6"<<std::endl;
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x800,"
                "in_port=1 apply:output=2");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x800,"
                "in_port=2 apply:output=1");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x806,"
                "in_port=1 apply:output=2");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x806,"
                "in_port=2 apply:output=1");
   }
   if(dpId==7){
    //std::cout<<"dpId7"<<std::endl;
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x800,"
                "in_port=1 apply:output=2");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x800,"
                "in_port=2 apply:output=1");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x806,"
                "in_port=1 apply:output=2");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x806,"
                "in_port=2 apply:output=1");
   }
   if(dpId==8){
    //std::cout<<"dpId8"<<std::endl;
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x800,"
                "in_port=1 apply:output=2");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x800,"
                "in_port=2 apply:output=1");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x806,"
                "in_port=1 apply:output=2");
    DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x806,"
                "in_port=2 apply:output=1");
   }
  std::pair <DatapathMap_t::iterator, bool> ret;
  ret =  m_learnedInfo.insert (std::pair<uint64_t, L2Table_t> (dpId, l2Table));
  if (ret.second == false)
    {
      NS_LOG_ERROR ("Table exists for this datapath.");
    }
}

} // namespace ns3
#endif // NS3_OFSWITCH13
