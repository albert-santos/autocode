/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Luciano Chaves <luciano@lrc.ic.unicamp.br>
 */
#ifdef NS3_OFSWITCH13
#include "my-controller.h"
#include <ns3/network-module.h>
#include <ns3/internet-module.h>
NS_LOG_COMPONENT_DEFINE ("MyController");
namespace ns3 {
NS_OBJECT_ENSURE_REGISTERED (MyController);
/********** Public methods ***********/
MyController::MyController ()
{
  NS_LOG_FUNCTION (this);
}
MyController::~MyController ()
{
  NS_LOG_FUNCTION (this);
}
TypeId
MyController::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MyController")
    .SetParent<OFSwitch13Controller> ()
    .SetGroupName ("OFSwitch13")
    .AddConstructor<MyController> ()
  ;
  return tid;
}
void
MyController::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_learnedInfo.clear ();
  OFSwitch13Controller::DoDispose ();
}

ofl_err
MyController::HandlePacketIn (
  struct ofl_msg_packet_in *msg, Ptr<const RemoteSwitch> swtch,
  uint32_t xid)
{
  NS_LOG_FUNCTION (this << swtch << xid);
  //PORT 2==INPUT PORT
  //PORT 1==OUTPUT PORT
  //uint32_t outPort = OFPP_FLOOD;
  static uint64_t swt1=1;
  uint16_t bbu=0;
  bool access_bbu1= true;
  bool access_bbu2= true;
  bool access_bbu3= true;
  bool access_bbu4= true;
  bool access_bbu5= true;
  bool access_bbu6= true;

  uint64_t swt2=2;
  //static int prio = 100;
  uint64_t dpId = swtch->GetDpId ();
  //enum ofp_packet_in_reason reason = msg->reason;
  static uint32_t connectionCounter = 0;
  //static uint32_t aux = 3;
  char *msgStr =
    ofl_structs_match_to_string ((struct ofl_match_header*)msg->match, 0);
  NS_LOG_DEBUG ("Packet in match: " << msgStr);
  free (msgStr);
  // Get Ethernet frame type
  uint16_t ethType;
  struct ofl_match_tlv *tlv;
  tlv = oxm_match_lookup (OXM_OF_ETH_TYPE, (struct ofl_match*)msg->match);
  memcpy (&ethType, tlv->value, OXM_LENGTH (OXM_OF_ETH_TYPE));
  Ipv4Address srcIp, dstIp;

  uint32_t inPort;
  size_t portLen = OXM_LENGTH (OXM_OF_IN_PORT); // (Always 4 bytes)
  struct ofl_match_tlv *input =
  oxm_match_lookup (OXM_OF_IN_PORT, (struct ofl_match*)msg->match);
  memcpy (&inPort, input->value, portLen);

  if (ethType == Ipv4L3Protocol::PROT_NUMBER)
        srcIp = ExtractIpv4Address (OXM_OF_IPV4_SRC, (struct ofl_match*)msg->match);
        dstIp = ExtractIpv4Address (OXM_OF_IPV4_DST, (struct ofl_match*)msg->match);
        //std::cout<<srcIp<<std::endl;
        //std::cout<<dstIp<<std::endl;
      }
   if (ethType == ArpL3Protocol::PROT_NUMBER)
        {
          // ARP packet
          std::cout<<"-------------------------arp-----------------------------"<<std::endl;
        }

   if(connect_bbu_1>=200){
    access_bbu1=false;
  if(connect_bbu_2>=200){
    access_bbu2=false;
  }
  if(connect_bbu_3>=200){
    access_bbu3=false;
  }
  if(connect_bbu_4>=200){
    access_bbu4=false;
  }
  if(connect_bbu_5>=200){
    access_bbu5=false;
  }
  if(connect_bbu_6>=200){
    access_bbu6=false;
  }

  /*
	Não-balanceado
	BBU 1 = 7,12= 65+65 = 130
	BBU 2 = 3,6 = 50+55 = 105
	BBU 3 = 1,2 = 70+45 = 115
	BBU 4 = 13,14,17,18 = 60+45+45+50 = 200
	BBU 5 = 4,5,8,9 = 45+60+60+65 = 230
	BBU 6 = 10,11,15,16,19 = 50+45+60+45+50 = 250
	Balanceado
	BBU 1 = 170s_status_with_JasmineModel.xls', number_of_bbus = 6)
	BBU 4 = 160
	BBU 5 = 175
	BBU 6 = 180
  */


  // if(connectionCounter<130 && access_bbu1){
  //   connect_bbu_1++;
  //   bbu=1;
  // }else if(connectionCounter<235 && access_bbu2){
  //   connect_bbu_2++;
  //   bbu=2;
  // }else if(connectionCounter<350 && access_bbu3){
  //   connect_bbu_3++;
  //   bbu=3;
  // }else if(connectionCounter<550 && access_bbu4){
  //   connect_bbu_4++;
  //   bbu=4;
  // }else if(connectionCounter<780 && access_bbu5){
  //   connect_bbu_5++;
  //   bbu=5;
  // }else if(connectionCounter<=1030 && access_bbu6){
  //   connect_bbu_6++;
  //   bbu=6;
  // }else{
  // 	std::cout<<"-------------------------limite alcançado------------------------------------"<<std::endl;
  //   bbu=10;
  // }


if(connectionCounter<170 && access_bbu1){
    connect_bbu_1++;
    bbu=1;
  }else if(connectionCounter<350 && access_bbu2){
    connect_bbu_2++;
    bbu=2;
  }else if(connectionCounter<515 && access_bbu3){
    connect_bbu_3++;
    bbu=3;
  }else if(connectionCounter<675 && access_bbu4){
    connect_bbu_4++;
    bbu=4;
  }else if(connectionCounter<850 && access_bbu5){
    connect_bbu_5++;
    bbu=5;
  }else if(connectionCounter<=1030 && access_bbu6){
    connect_bbu_6++;
    bbu=6;
  }else{
  	std::cout<<"-------------------------limite alcançado------------------------------------"<<std::endl;
    bbu=10;}


//#####################Cenário com cargas diferentes########### 
  //  if(connectionCounter<130 && access_bbu1){
  //   connect_bbu_1++;
  //   bbu=1;
  // }else if(connectionCounter<235 && access_bbu2){
  //   connect_bbu_2++;
  //   bbu=2;
  // }else if(connectionCounter<350 && access_bbu3){
  //   connect_bbu_3++;
  //   bbu=3;
  // }else if(connectionCounter<550 && access_bbu4){
  //   connect_bbu_4++;
  //   bbu=4;
  // }else if(connectionCounter<780 && access_bbu5){
  //   connect_bbu_5++;
  //   bbu=5;
  // }else if(connectionCounter<=1030 && access_bbu6){
  //   connect_bbu_6++;
  //   bbu=6;
  // }else{
  // 	std::cout<<"-------------------------limite alcançado------------------------------------"<<std::endl;
  //   bbu=10;
  // }
  std::cout<<"shit "<<std::endl;
  if(dpId==1 && !(bbu==10)){
  connectionCounter++;
  std::ostringstream flowCmd;
  flowCmd << "flow-mod cmd=add,prio=100,table=0, eth_type=0x0800,"
          << "ip_src=" << srcIp
          << ",in_port=7 apply:output=" << bbu;
   DpctlExecute (swt1, flowCmd.str ());
  std::ostringstream flowCmd2;
  flowCmd2 << "flow-mod cmd=add,prio=100,table=0, eth_type=0x0800,"
          << "ip_dst=" << srcIp
          << ",in_port=1 apply:output=" << bbu+1;
   DpctlExecute (swt2, flowCmd2.str ());
   //std::cout<<"connect:"<<connectionCounter<<std::endl;
 }
 if(dpId==2){
 	// std::cout<<"----------------------------------switch2-----------------------------"<<std::endl;
  //std::cout<<"dpId 2, connect:"<<connectionCounter<<std::endl;
  //std::cout<<"aux:"<<aux<<std::endl;
  // std::ostringstream flowCmd;
  // flowCmd << "flow-mod cmd=add,prio=100,table=0, eth_type=0x0800,"
  //         << "ip_dst=" << dstIp
  //         << " apply:output=" << bbu+1;
  //  DpctlExecute (swt2, flowCmd.str ());
   //std::cout<<"dpId 2, bbu:"<<bbu<<std::endl;
 }
  // if(dpId==2){
  //   DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x800,ip_dst=7.0.0.2,"
  //               "in_port=1 apply:output=2");
  //   DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x806,arp_tpa=7.0.0.2,"
  //               "in_port=1 apply:output=2");
  //   DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x800,ip_dst=7.0.0.3,"
  //               "in_port=1 apply:output=3");
  //   DpctlExecute (swtch, "flow-mod cmd=add,table=0, eth_type=0x806,arp_tpa=7.0.0.3,"
  //               "in_port=1 apply:output=3");
  //  }
//Se utilizando TCP, descomentar essas linhas!!
  // All handlers must free the message when everything is ok
 // Lets send the packet out to switch.
  //     struct ofl_msg_packet_out reply;
  //     reply.header.tzype = OFPT_PACKET_OUT;
  //     reply.buffer_id = msg->buffer_id;
  //     reply.in_port = inPort;
  //     reply.data_length = 0;
  //     reply.data = 0;

  //     if (msg->buffer_id == NO_BUFFER)
  //       {
  //         // No packet buffer. Send data back to switch
  //         reply.data_length = msg->data_length;
  //         reply.data = msg->data;
  //       }

  //     // Create output action
  //     struct ofl_action_output *a =
  //       (struct ofl_action_output*)xmalloc (sizeof (struct ofl_action_output));
  //     a->header.type = OFPAT_OUTPUT;
  //     a->port = outPort;
  //     a->max_len = 0;

  //     reply.actions_num = 1;
  //     reply.actions = (struct ofl_action_header**)&a;

  //     SendToSwitch (swtch, (struct ofl_msg_header*)&reply, xid);
  //     free (a);

  // ofl_msg_free ((struct ofl_msg_header*)msg, 0);
  return 0;
}
Ipv4Address
MyController::ExtractIpv4Address (uint32_t oxm_of, struct ofl_match* match)
{
  switch (oxm_of)
    {
    case OXM_OF_ARP_SPA:
    case OXM_OF_ARP_TPA:
    case OXM_OF_IPV4_DST:
    case OXM_OF_IPV4_SRC:
      {
        uint32_t ip;
        int size = OXM_LENGTH (oxm_of);
        struct ofl_match_tlv *tlv = oxm_match_lookup (oxm_of, match);
        memcpy (&ip, tlv->value, size);
        return Ipv4Address (ntohl (ip));
      }
    default:
      NS_FATAL_ERROR ("Invalid IP field.");
}

ofl_err
MyController::HandleFlowRemoved (
  struct ofl_msg_flow_removed *msg, Ptr<const RemoteSwitch> swtch,
  uint32_t xid)
{
  NS_LOG_FUNCTION (this << swtch << xid);

  NS_LOG_DEBUG ( "Flow entry expired. Removing from L2 switch table.");
  uint64_t dpId = swtch->GetDpId ();
  DatapathMap_t::iterator it = m_learnedInfo.find (dpId);
  if (it != m_learnedInfo.end ())
    {
      Mac48Address mac48;
      struct ofl_match_tlv *ethSrc =
        oxm_match_lookup (OXM_OF_ETH_DST, (struct ofl_match*)msg->stats->match);
      mac48.CopyFrom (ethSrc->value);

      L2Table_t *l2Table = &it->second;
      L2Table_t::iterator itSrc = l2Table->find (mac48);
      if (itSrc != l2Table->end ())
        {
          l2Table->erase (itSrc);
        }
    }

  // All handlers must free the message when everything is ok
  ofl_msg_free_flow_removed (msg, true, 0);
  return 0;
}

void MyController::NotifyHandoverStartUe (std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti, uint16_t targetCellId)
{
  std::cout << Simulator::Now ().GetSeconds () << " " << context
            << " UE IMSI " << imsi
            << ": previously connected to CellId " << cellid
            << " with RNTI " << rnti
            << ", doing handover to CellId " << targetCellId
            << std::endl;
}

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


  static uint32_t connect_bbu_1 = 0;
  static uint32_t connect_bbu_2 = 0;
  static uint32_t connect_bbu_3 = 0;
  static uint32_t connect_bbu_4 = 0;
  static uint32_t connect_bbu_5 = 0;
  static uint32_t connect_bbu_6 = 0;
  
  

    {


    }
  }





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
        mymap3[m_mymap[imsi]]= 3;
        std::cout<<"ip: "<<m_mymap[imsi]<<" associado à BBU: 3 (Handover)"<<std::endl; 
        break;
      case 2:
        mymap3[m_mymap[imsi]]= 3;
        std::cout<<"ip: "<<m_mymap[imsi]<<" associado à BBU: 3 (Handover)"<<std::endl; 
        break;
      case 3:
        mymap3[m_mymap[imsi]]= 2;
        std::cout<<"ip: "<<m_mymap[imsi]<<" associado à BBU: 2 (Handover)"<<std::endl; 
        break;
      case 4:
        mymap3[m_mymap[imsi]]= 5;
        std::cout<<"ip: "<<m_mymap[imsi]<<" associado à BBU: 5 (Handover)"<<std::endl; 
        break;
      case 5:
        mymap3[m_mymap[imsi]]= 5;
        std::cout<<"ip: "<<m_mymap[imsi]<<" associado à BBU: 5 (Handover)"<<std::endl; 
        break;
      case 6:
        mymap3[m_mymap[imsi]]= 2;
        std::cout<<"ip: "<<m_mymap[imsi]<<" associado à BBU: 2 (Handover)"<<std::endl; 
        break;
      case 7:
        mymap3[m_mymap[imsi]]= 1;
        std::cout<<"ip: "<<m_mymap[imsi]<<" associado à BBU: 1 (Handover)"<<std::endl; 
        break;
      case 8:
        mymap3[m_mymap[imsi]]= 5;
        std::cout<<"ip: "<<m_mymap[imsi]<<" associado à BBU: 5 (Handover)"<<std::endl; 
        break;
      case 9:
        mymap3[m_mymap[imsi]]= 5;
        std::cout<<"ip: "<<m_mymap[imsi]<<" associado à BBU: 5 (Handover)"<<std::endl; 
        break;
      case 10:
        mymap3[m_mymap[imsi]]= 6;
        std::cout<<"ip: "<<m_mymap[imsi]<<" associado à BBU: 6 (Handover)"<<std::endl; 
        break;
      case 11:
        mymap3[m_mymap[imsi]]= 6;
        std::cout<<"ip: "<<m_mymap[imsi]<<" associado à BBU: 6 (Handover)"<<std::endl; 
        break;
      case 12:
        mymap3[m_mymap[imsi]]= 1;
        std::cout<<"ip: "<<m_mymap[imsi]<<" associado à BBU: 1 (Handover)"<<std::endl; 
        break;
      case 13:
        mymap3[m_mymap[imsi]]= 4;
        std::cout<<"ip: "<<m_mymap[imsi]<<" associado à BBU: 4 (Handover)"<<std::endl; 
        break;
      case 14://AUTOCODE UPDATE FIM
      case 15:
        mymap3[m_mymap[imsi]]= 6;
        std::cout<<"ip: "<<m_mymap[imsi]<<" associado à BBU: 6 (Handover)"<<std::endl; 
        break;
      case 16:
        mymap3[m_mymap[imsi]]= 6;
        std::cout<<"ip: "<<m_mymap[imsi]<<" associado à BBU: 6 (Handover)"<<std::endl; 
        break;
      case 17:
        mymap3[m_mymap[imsi]]= 4;
        std::cout<<"ip: "<<m_mymap[imsi]<<" associado à BBU: 4 (Handover)"<<std::endl; 
        break;
      case 18:
        mymap3[m_mymap[imsi]]= 4;
        std::cout<<"ip: "<<m_mymap[imsi]<<" associado à BBU: 4 (Handover)"<<std::endl; 
        break;
      case 19:
        mymap3[m_mymap[imsi]]= 6;
        std::cout<<"ip: "<<m_mymap[imsi]<<" associado à BBU: 6 (Handover)"<<std::endl; 
        break;
      default:
        mymap3[m_mymap[imsi]]= 7;
        std::cout<<"ip: "<<m_mymap[imsi]<<" não associado"<<std::endl; 
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

	uint32_t connect_RRH_1 = 0;
	uint32_t connect_RRH_2 = 0;
	uint32_t connect_RRH_3 = 0;
	uint32_t connect_RRH_4 = 0;
	uint32_t connect_RRH_5 = 0;
	uint32_t connect_RRH_6 = 0;
	uint32_t connect_RRH_7 = 0;
	uint32_t connect_RRH_8 = 0;
	uint32_t connect_RRH_9 = 0;
	uint32_t connect_RRH_10 = 0;
	uint32_t connect_RRH_11 = 0;
	uint32_t connect_RRH_12 = 0;
	uint32_t connect_RRH_13 = 0;
	uint32_t connect_RRH_14 = 0;
	uint32_t connect_RRH_15 = 0;
	uint32_t connect_RRH_16 = 0;
	uint32_t connect_RRH_17 = 0;
	uint32_t connect_RRH_18 = 0;
	uint32_t connect_RRH_19 = 0;
	uint32_t connect_RRH_20 = 0;
	uint32_t connect_RRH_21 = 0;
	uint32_t connect_RRH_22 = 0;
	uint32_t connect_RRH_23 = 0;
	uint32_t connect_RRH_24 = 0;
	uint32_t connect_RRH_25 = 0;

	uint32_t connect_bbu_1 = 0;
	uint32_t connect_bbu_2 = 0;
	uint32_t connect_bbu_3 = 0;
	uint32_t connect_bbu_4 = 0;
	uint32_t connect_bbu_5 = 0;
	uint32_t connect_bbu_6 = 0;

	for (std::map<uint64_t, uint64_t >::iterator it=m_mymap2.begin(); it!=m_mymap2.end(); ++it) 
	{

		//Associando cada antena a uma BBU de acordo com a alocação teste

		switch (it->second)
		{
			case 1:
				connect_RRH_1++;
				mymap3[m_mymap[it->first]]= 4;

	std::cout <<"BBU 6: " << connect_bbu_6 << " usuários" << std::endl;
	std::cout <<"BBU 5: " << connect_bbu_5 << " usuários" << std::endl;
	std::cout <<"BBU 4: " << connect_bbu_4 << " usuários" << std::endl;
	std::cout <<"BBU 3: " << connect_bbu_3 << " usuários" << std::endl;
	std::cout <<"BBU 2: " << connect_bbu_2 << " usuários" << std::endl;
	std::cout <<"BBU 1: " << connect_bbu_1 << " usuários" << std::endl;
	std::cout << " " << std::endl;

	std::cout <<"RRH 25: " << connect_RRH_25 << " usuários << std::endl";
	std::cout <<"RRH 24: " << connect_RRH_24 << " usuários << std::endl";
	std::cout <<"RRH 23: " << connect_RRH_23 << " usuários << std::endl";
	std::cout <<"RRH 22: " << connect_RRH_22 << " usuários << std::endl";
	std::cout <<"RRH 21: " << connect_RRH_21 << " usuários << std::endl";
	std::cout <<"RRH 20: " << connect_RRH_20 << " usuários << std::endl";
	std::cout <<"RRH 19: " << connect_RRH_19 << " usuários << std::endl";
	std::cout <<"RRH 18: " << connect_RRH_18 << " usuários << std::endl";
	std::cout <<"RRH 17: " << connect_RRH_17 << " usuários << std::endl";
	std::cout <<"RRH 16: " << connect_RRH_16 << " usuários << std::endl";
	std::cout <<"RRH 15: " << connect_RRH_15 << " usuários << std::endl";
	std::cout <<"RRH 14: " << connect_RRH_14 << " usuários << std::endl";
	std::cout <<"RRH 13: " << connect_RRH_13 << " usuários << std::endl";
	std::cout <<"RRH 12: " << connect_RRH_12 << " usuários << std::endl";
	std::cout <<"RRH 11: " << connect_RRH_11 << " usuários << std::endl";
	std::cout <<"RRH 10: " << connect_RRH_10 << " usuários << std::endl";
	std::cout <<"RRH 9: " << connect_RRH_9 << " usuários << std::endl";
	std::cout <<"RRH 8: " << connect_RRH_8 << " usuários << std::endl";
	std::cout <<"RRH 7: " << connect_RRH_7 << " usuários << std::endl";
	std::cout <<"RRH 6: " << connect_RRH_6 << " usuários << std::endl";
	std::cout <<"RRH 5: " << connect_RRH_5 << " usuários << std::endl";
	std::cout <<"RRH 4: " << connect_RRH_4 << " usuários << std::endl";
	std::cout <<"RRH 3: " << connect_RRH_3 << " usuários << std::endl";
	std::cout <<"RRH 2: " << connect_RRH_2 << " usuários << std::endl";
	std::cout <<"RRH 1: " << connect_RRH_1 << " usuários << std::endl";
	std::cout <<" Qtd de Usuários por RRH"<< std::endl;
	std::cout << " " << std::endl;

		std::cout <<"ip address: "<< it->first << " bbu: " << it->second << std::endl; 
	for (std::map<Ipv4Address, uint64_t >::iterator it=mymap3.begin(); it!=mymap3.end(); ++it)
	std::cout <<" MAPA 3: IP X BBU"<< std::endl;

		std::cout <<"imsi: "<< it->first << " connected to cellid: " << it->second << std::endl; 
	for (std::map<uint64_t, uint64_t>::iterator it=m_mymap2.begin(); it!=m_mymap2.end(); ++it)
	std::cout <<" MAPA 2: IMSI X RRH_ID"<< std::endl;

		std::cout <<"imsi: "<< it->first << " ip address: " << it->second << std::endl;
	for (std::map<uint64_t, Ipv4Address>::iterator it=m_mymap.begin(); it!=m_mymap.end(); ++it)
	std::cout <<" MAPA 1: IMSI X IP" << std::endl;

	}
		}
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" não associado"<<std::endl;
				mymap3[m_mymap[it->first]]= 7;
			default:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 5"<<std::endl;
				connect_bbu_5++;
				mymap3[m_mymap[it->first]]= 5;
				connect_RRH_25++;
			case 25:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 1"<<std::endl;
				connect_bbu_1++;
				mymap3[m_mymap[it->first]]= 1;
				connect_RRH_24++;
			case 24:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 2"<<std::endl;
				connect_bbu_2++;
				mymap3[m_mymap[it->first]]= 2;
				connect_RRH_23++;
			case 23:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 2"<<std::endl;
				connect_bbu_2++;
				mymap3[m_mymap[it->first]]= 2;
				connect_RRH_22++;
			case 22:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 6"<<std::endl;
				connect_bbu_6++;
				mymap3[m_mymap[it->first]]= 6;
				connect_RRH_21++;
			case 21:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 2"<<std::endl;
				connect_bbu_2++;
				mymap3[m_mymap[it->first]]= 2;
				connect_RRH_20++;
			case 20:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 2"<<std::endl;
				connect_bbu_2++;
				mymap3[m_mymap[it->first]]= 2;
				connect_RRH_19++;
			case 19:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 2"<<std::endl;
				connect_bbu_2++;
				mymap3[m_mymap[it->first]]= 2;
				connect_RRH_18++;
			case 18:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 3"<<std::endl;
				connect_bbu_3++;
				mymap3[m_mymap[it->first]]= 3;
				connect_RRH_17++;
			case 17:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 3"<<std::endl;
				connect_bbu_3++;
				mymap3[m_mymap[it->first]]= 3;
				connect_RRH_16++;
			case 16:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 6"<<std::endl;
				connect_bbu_6++;
				mymap3[m_mymap[it->first]]= 6;
				connect_RRH_15++;
			case 15:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 1"<<std::endl;
				connect_bbu_1++;
				mymap3[m_mymap[it->first]]= 1;
				connect_RRH_14++;
			case 14:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 2"<<std::endl;
				connect_bbu_2++;
				mymap3[m_mymap[it->first]]= 2;
				connect_RRH_13++;
			case 13:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 2"<<std::endl;
				connect_bbu_2++;
				mymap3[m_mymap[it->first]]= 2;
				connect_RRH_12++;
			case 12:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 2"<<std::endl;
				connect_bbu_2++;
				mymap3[m_mymap[it->first]]= 2;
				connect_RRH_11++;
			case 11:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 6"<<std::endl;
				connect_bbu_6++;
				mymap3[m_mymap[it->first]]= 6;
				connect_RRH_10++;
			case 10:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 3"<<std::endl;
				connect_bbu_3++;
				mymap3[m_mymap[it->first]]= 3;
				connect_RRH_9++;
			case 9:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 5"<<std::endl;
				connect_bbu_5++;
				mymap3[m_mymap[it->first]]= 5;
				connect_RRH_8++;
			case 8:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 3"<<std::endl;
				connect_bbu_3++;
				mymap3[m_mymap[it->first]]= 3;
				connect_RRH_7++;
			case 7:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 4"<<std::endl;
				connect_bbu_4++;
				mymap3[m_mymap[it->first]]= 4;
				connect_RRH_6++;
			case 6:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 3"<<std::endl;
				connect_bbu_3++;
				mymap3[m_mymap[it->first]]= 3;
				connect_RRH_5++;
			case 5:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 5"<<std::endl;
				connect_bbu_5++;
				mymap3[m_mymap[it->first]]= 5;
				connect_RRH_4++;
			case 4:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 2"<<std::endl;
				connect_bbu_2++;
				mymap3[m_mymap[it->first]]= 2;
				connect_RRH_3++;
			case 3:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 1"<<std::endl;
				connect_bbu_1++;
				mymap3[m_mymap[it->first]]= 1;
				connect_RRH_2++;
			case 2:
				break;
				//std::cout<<"ip: "<<m_mymap[it->first]<<" associado à BBU: 4"<<std::endl;
				connect_bbu_4++;
