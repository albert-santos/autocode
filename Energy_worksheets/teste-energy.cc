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
 */

#include "ns3/core-module.h"
#include "ns3/basic-energy-source.h"
#include "ns3/wifi-radio-energy-model.h"
#include "ns3/basic-energy-source-helper.h"
#include "ns3/wifi-radio-energy-model-helper.h"
#include "ns3/energy-source-container.h"
#include "ns3/device-energy-model-container.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/simulator.h"
#include "ns3/double.h"
#include "ns3/config.h"
#include "ns3/string.h"
#include "ns3/yans-wifi-helper.h"
#include <cmath>
#include "ns3/simple-device-energy-model.h"
#include "ns3/simple-device-energy-model.h"


#include <ns3/buildings-helper.h>
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/energy-module.h"
#include "lte-energy-model-helper.h"

#include "UsersByBBU.cc"

#include <list>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("LteEnergy");


// Trace function for remaining energy at node.
void
RemainingEnergy (double oldValue, double remainingEnergy)
{
  NS_LOG_UNCOND (Simulator::Now ().GetSeconds ()
                 << "s Current remaining energy = " << remainingEnergy << "J");
}

/// Trace function for total energy consumption at node.
void
TotalEnergy (double oldValue, double totalEnergy)
{
  NS_LOG_UNCOND (Simulator::Now ().GetSeconds ()
                 << "s Total energy consumed by radio = " << totalEnergy << "J");
}


int 
main (int argc, char *argv[])
{
  NS_LOG_UNCOND ("LteEnergy");

  // Teste

  // double a {5};
  // double b {10};

  // double result1 = a + b;

  // std::cout << "result1: " << result1 << std::endl;

  int number_of_BBUs = 20;

  NodeContainer c;
  NodeContainer ueNodes;
  c.Create (number_of_BBUs);     // create nodes
  ueNodes.Create(6);
  std::cout << "Criando nós" << std::endl;

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  std::cout << "Criando lteHelper" << std::endl;

    // Install Mobility Model
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (c);
  BuildingsHelper::Install (c);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (ueNodes);
  BuildingsHelper::Install (ueNodes);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  std::cout << "Instalando mobilidade no nó Lte" << std::endl;

  // Create Devices and install them in the Nodes (eNB)
  NetDeviceContainer enbLteDevs;
  NetDeviceContainer ueLteDevs;
  enbLteDevs = lteHelper->InstallEnbDevice (c);
  ueLteDevs = lteHelper->InstallUeDevice (ueNodes);
  std::cout << "Instalando netdevice nos nós Lte" << std::endl;

  // Attach a UE to a eNB
  lteHelper->Attach (ueLteDevs, enbLteDevs.Get (0));

  // Activate a data radio bearer
  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
  EpsBearer bearer (q);
  lteHelper->ActivateDataRadioBearer (ueLteDevs, bearer);
  lteHelper->EnableTraces ();



  /** Energy **/
  /***************************************************************************/

  BasicEnergySourceHelper basicSourceHelper;
  std::cout << "Criando basicSourceHelper" << std::endl;

  DoubleValue source_initial_energy = 10000;

  basicSourceHelper.Set ("BasicEnergySourceInitialEnergyJ", source_initial_energy );
  std::cout << "Configurando a energia inicial da fonte para " << " 10000 J" << std::endl;

    /* device energy model */
  LteEnergyModelHelper lte_energy_helper;
  std::cout <<"Criando helper lte energy" << std::endl;

  EnergySourceContainer sources = basicSourceHelper.Install (c);
  std::cout << "Instalando a fonte nos nós"  << std::endl;




  /** modificando a corrente de acordo com a quantidade de usuários **/
  DeviceEnergyModelContainer container2;

  //Hora 23
  // int RRH_by_BBU [number_of_BBUs] = {6, 4,	2,	5,	2,	10,	4,	7,	4,	6,	4,	5,	5,	7,	5, 5, 4, 4, 7, 4};
  // int users_by_BBU [number_of_BBUs] = {16,	20,	13,	18,	12,	41,	18,	11,	17,	19,	12,	21,	22,	18,	20,	23,	18,	27,	21,	14};

  //Hora 5
  int RRH_by_BBU [number_of_BBUs] = {5,	6,	6,	6,	8,	3,	4,	4,	4,	4,	5,	3,	6,	3,	7,	4,	3,	3,	7,	9};
  int users_by_BBU [number_of_BBUs] = {5,	4,	11,	3,	6,	6,	3,	4,	6,	5,	3,	3,	3,	6,	9,	4,	5,	5,	4,	3};
  
  for (int i{}; i < number_of_BBUs; i++){

    double RRH_power = 20 * RRH_by_BBU[i];
    double Users_power = 3.06 * users_by_BBU[i];
    double BBU_power = 10;
    double total_power = RRH_power + Users_power + BBU_power;

    double voltagem = 3; // Voltagem padrão das fontes

    DoubleValue current = (total_power / voltagem );
    lte_energy_helper.Set ("TxCurrentA", current);

    std::cout << "teste " << i << std::endl;
    auto device = enbLteDevs.Get(i);
    auto source = sources.Get(i);
    
    //verifica se os ponteiros são nulos
    NS_ASSERT (device != NULL);
    NS_ASSERT (source != NULL);
    // std::cout << "tipo de enbDevs " << device << device->GetInstanceTypeId () << std::endl;
    // std::cout << "tipo de enbDevs " << source << source->GetInstanceTypeId () << std::endl;

    // check if device is LteDevice
    std::string deviceName = device->GetInstanceTypeId ().GetName ();
    // std::cout << "Tipo de dispositivo: " << deviceName << std::endl;
      if (deviceName.compare ("ns3::LteEnbNetDevice") != 0)
    {
      NS_FATAL_ERROR ("NetDevice type is not LteEnbNetDevice!");
    }

    Ptr<Node> node = device->GetNode ();
    // std::cout << "Nó do dispositivo: " << node << std::endl;

    DeviceEnergyModelContainer model = lte_energy_helper.Install (device, source);
    std::cout << "Gerando modelo a partir de ponteiros " << std::endl;

    container2.Add (model);
    std::cout << "Adicionando o contêiner " << std::endl;
  }
  /***************************************************************************/



  /** connect trace sources **/
  /***************************************************************************/
  // all sources are connected to node 1
  // energy source
  Ptr<BasicEnergySource> basicSourcePtr = DynamicCast<BasicEnergySource> (sources.Get (0));
  basicSourcePtr->TraceConnectWithoutContext ("RemainingEnergy", MakeCallback (&RemainingEnergy));
  // device energy model
  Ptr<DeviceEnergyModel> basicRadioModelPtr =
    basicSourcePtr->FindDeviceEnergyModels ("ns3::LteEnergyModel").Get (0);
  NS_ASSERT (basicRadioModelPtr != NULL);
  basicRadioModelPtr->TraceConnectWithoutContext ("TotalEnergyConsumption", MakeCallback (&TotalEnergy));
  /***************************************************************************/






  // /** modificando a corrente de acordo com a quantidade de usuários **/
  // // /***************************************************************************/
  // // uint16_t UsersByRrhs[16] = {70,45,50,45,60,55,65,60,65,45,60,65,50,45,45,50};
  // int i {0};
  // for (DeviceEnergyModelContainer::Iterator iter = deviceModels.Begin (); iter != deviceModels.End (); iter ++)
  //   {
  //     double currentA = i * 0.0174;
  //     (*iter)->SetTxCurrentA (currentA);
  //     NS_LOG_UNCOND ("Modificando a corrente para dispositivo " << i << " para: " << (*iter)->GetTxCurrentA());
  //     // NS_LOG_UNCOND ("End of simulation (" << Simulator::Now ().GetSeconds ()
  //     //                << "s) Total energy consumed by radio = " << energyConsumed << "J");
      
  //     // NS_ASSERT (energyConsumed <= 0.1);
  //   }
  // // /***************************************************************************/






  // Delta necessário para calcular a energia do último segundo da simulação
  // Sem o Delta não é realizado o cálculo para o segundo 11 se tempo de simulação for 11s
  double delta = 0.000002;
  double stop_time = 11 + delta;
  Simulator::Stop (Seconds (stop_time));

  Simulator::Run ();

    //inicializando em zero a variável que armazenará o consumo total da BBU pool
    double total_energy_consumed = {};

    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << " Quantidade de energia consumida por cada BBU " << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;
    for (DeviceEnergyModelContainer::Iterator iter = container2.Begin (); iter != container2.End (); iter ++)
    {
      double energyConsumed = (*iter)->GetTotalEnergyConsumption ();
      NS_LOG_UNCOND ("End of simulation (" << Simulator::Now ().GetSeconds ()
                     << "s) Total energy consumed by BBU = " << energyConsumed << "J");
      
      total_energy_consumed += energyConsumed;
    }


    std::cout << "-------------------------------------------------" << std::endl;
    NS_LOG_UNCOND ("Energy consumida pela BBU pool: " << total_energy_consumed << "J");
    std::cout << "-------------------------------------------------" << std::endl;



    //** Mostra quantidade restante de energia em cada BBU
    // int count = 1;
    // for (EnergySourceContainer::Iterator iter = sources.Begin (); iter != sources.End (); iter ++)
    // {
    //   double remain_energy = (*iter)->GetRemainingEnergy();
    //   NS_LOG_UNCOND ("End of simulation (" << Simulator::Now ().GetSeconds ()
    //                  << "s) Remain energy BBU " << count << " = " << remain_energy << "J");
    //   count++;
    // }

  Simulator::Destroy ();

  return 0;
}
