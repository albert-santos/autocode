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

/* ifndef - caso não esteja definido
  Trata-se de um mecanismo do C++ para evitar um biblioteca ou arquivo seja declarado mais de uma vez. Caso haja mais de uma declaração ocorrerá erro
  Dessa forma, todo o código abaixo de ifndef só será incluso caso ainda não tenha sido definido
*/
#ifndef MY_CONTROLLER_H
#define MY_CONTROLLER_H

// Importa o módulo ofswitch13 
#include <ns3/ofswitch13-module.h>

namespace ns3 {

/**
 * \ingroup ofswitch13
 * \brief An Learning OpenFlow 1.3 controller (works as L2 switch)
 */
//Declaração da classe MyController
class MyController : public OFSwitch13Controller
{
public:
  MyController ();          //!< Default constructor
  virtual ~MyController (); //!< Dummy destructor.

  /**
   * Register this type.
   * \return The object TypeId.
   */
  static TypeId GetTypeId (void);

  /** Destructor implementation */
  virtual void DoDispose ();

  /*
    Metodo de alocação, este método toma como argumento dois objetos do tipo map,
    relacionando imsi a IP e imsi a cellid, respectivamente.
    O objetivo desta função é relacionar UE, ENB e BBU,
    através da construção de um map relacionando ip a bbu antes de começar o tráfego de dados.
  */
  virtual void Allocation (std::map <uint64_t, Ipv4Address> mymap,std::map <uint64_t, uint64_t> mymap2);

  /* Método que atualiza a alocação dos usuários caso haja handover
  */
  virtual void Update (std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /* Método que indica quando ocorre handover informando IMSI do UE, Id da célula a qual estava conectado, RNTI e célula alvo
  */
  virtual void NotifyHandoverStartUe (std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti, uint16_t targetCellId);
  
  /**
   * Handle packet-in messages sent from switch to this controller. Look for L2
   * switching information, update the structures and send a packet-out back.
   *
   * \param msg The packet-in message.
   * \param swtch The switch information.
   * \param xid Transaction id.
   * \return 0 if everything's ok, otherwise an error number.
   */
  ofl_err HandlePacketIn (
    struct ofl_msg_packet_in *msg, Ptr<const RemoteSwitch> swtch,
    uint32_t xid);

  /**
   * Handle flow removed messages sent from switch to this controller. Look for
   * L2 switching information and removes associated entry.
   *
   * \param msg The flow removed message.
   * \param swtch The switch information.
   * \param xid Transaction id.
   * \return 0 if everything's ok, otherwise an error number.
   */
  ofl_err HandleFlowRemoved (
    struct ofl_msg_flow_removed *msg, Ptr<const RemoteSwitch> swtch,
    uint32_t xid);
  std::map <uint64_t, Ipv4Address> m_mymap;
  std::map <uint64_t, uint64_t> m_mymap2;
  std::map <Ipv4Address, uint64_t> mymap3;

protected:
  // Inherited from OFSwitch13Controller
  void HandshakeSuccessful (Ptr<const RemoteSwitch> swtch);

private:
  /** Map saving <IPv4 address / MAC address> */
  typedef std::map<Ipv4Address, Mac48Address> IpMacMap_t;
  IpMacMap_t m_arpTable; //!< ARP resolution table.
  Ipv4Address ExtractIpv4Address (uint32_t oxm_of, struct ofl_match* match);
  
  /**
   * \name L2 switching structures
   */
  //\{
  /** L2SwitchingTable: map MacAddress to port */
  typedef std::map<Mac48Address, uint32_t> L2Table_t;

  /** Map datapathID to L2SwitchingTable */
  typedef std::map<uint64_t, L2Table_t> DatapathMap_t;

  /** Switching information for all dapataths */
  DatapathMap_t m_learnedInfo;
  //\}
};

} // namespace ns3
#endif /* OFSWITCH13_LEARNING_CONTROLLER_H */
