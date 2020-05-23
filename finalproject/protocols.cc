/**
@file  protocols.cc
@author  Wesley Banghart, Christopher Eichstedt, Chantelle Marquez-Suarez, Liliana Pacheco
@brief  A simulation for Routing Protocols: AODV and DSDV. This code was developed using a given framework from the NS-3 repository. Here
	are the original files for reference: https://www.nsnam.org/doxygen/aodv_8cc_source.html and 
	https://www.nsnam.org/doxygen/dsdv-manet_8cc_source.html
**/
//--------------------------------------------------------------------------------------
//Included Libraries
//--------------------------------------------------------------------------------------
#include <iostream>
#include <cmath>
#include "ns3/aodv-module.h"
#include "ns3/dsdv-module.h"
#include "ns3/applications-module.h"
#include "ns3/config-store-module.h"
#include "ns3/dsdv-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/v4ping-helper.h"
#include "ns3/yans-wifi-helper.h"

using namespace ns3;

//--------------------------------------------------------------------------------------
//Custom Classes
//--------------------------------------------------------------------------------------
class AodvExample
{
public:
  AodvExample ();
  bool Configure (int argc, char **argv);
  void Run ();

private:

  uint32_t size;
  uint32_t nodeRM;
  double duration;
  bool pcap;

  NodeContainer nodes;
  NetDeviceContainer devices;
  Ipv4InterfaceContainer interfaces;

private:
  void CreateNodes ();
  void CreateDevices ();
  void InstallInternetStack ();
  void InstallApplications ();
};

class DsdvExample
{
public:
  DsdvExample ();
  bool Configure (int argc, char **argv);
  void Run ();

private:

  uint32_t size;
  uint32_t nodeRM;
  double duration;
  bool pcap;

  NodeContainer nodes;
  NetDeviceContainer devices;
  Ipv4InterfaceContainer interfaces;

private:
  void CreateNodes ();
  void CreateDevices ();
  void InstallInternetStack ();
  void InstallApplications ();
};

//--------------------------------------------------------------------------------------
//Main Function
//--------------------------------------------------------------------------------------
/**
@param  int argc, char **argv
@brief  The main driver for the simulation. It takes in a command line argument for the amount of nodes simulated. Then prompts the user
	for which routing protocol they wish to use.
**/
int main (int argc, char **argv)
{
  char select;
  std::cout << "Enter 'a' for AODV, or 'd' for DSDV" << std::endl;
  std::cin >> select;

  if(select == 'a')
  {
  	std::cout << "Initiating AODV test..." << std::endl;
  	AodvExample test;
  	if (!test.Configure (argc, argv))
    	  NS_FATAL_ERROR ("Configuration failed. Aborted.");

    	test.Run ();
  	return 0;
  }
  else if(select == 'd')
  {
  	std::cout << "Initiating DSDV test..." << std::endl;
  	DsdvExample test;
  	if (!test.Configure (argc, argv))
    	  NS_FATAL_ERROR ("Configuration failed. Aborted.");

    	test.Run ();
  	return 0;
  }
  else
  {
  	std::cout << "ERROR: Incorrect selection, please run again." << std::endl;
  	return 0;
  }
}

//--------------------------------------------------------------------------------------
//Class Functions (AODV)
//--------------------------------------------------------------------------------------
/**
@param  none
@brief  The AODV protocol constructor.
**/
AodvExample::AodvExample () :
  size (5),
  nodeRM (1),
  duration (50),
  pcap (false)

{
}

/**
@param  int argc, char **argv
@brief  The configuration function for AODV protocol. Currently not used in this simulation.
**/
bool AodvExample::Configure (int argc, char **argv)
{
  //Enable AODV logs by default. Comment this if too noisy
  //LogComponentEnable("AodvRoutingProtocol", LOG_LEVEL_ALL);

  CommandLine cmd;

  cmd.AddValue ("pcap", "Write PCAP traces.", pcap);
  cmd.AddValue ("size", "Number of nodes.", size);
  cmd.AddValue ("time", "Simulation time, s.", duration);
  cmd.AddValue ("fail", "Number of Nodes to move away.", nodeRM);

  cmd.Parse (argc, argv);

  if(size == 45)
  {
    SeedManager::SetSeed (8765);
  }
  else
  {
    SeedManager::SetSeed (1234);
  }

  return true;
}

/**
@param  none
@brief  The run function for AODV protocol. Simulates the process by calling additional functions.
**/
void AodvExample::Run ()
{
  //Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", UintegerValue (1));
  CreateNodes ();
  CreateDevices ();
  InstallInternetStack ();
  InstallApplications ();

  std::cout << "Starting simulation" << std::endl;

  Simulator::Stop (Seconds (duration));
  Simulator::Run ();
  Simulator::Destroy ();
}

/**
@param  none
@brief  The create node function for AODV protocol. It creates the amount of nodes specified in the command line. Then puts them at a
	set distance.
**/
void AodvExample::CreateNodes ()
{
  std::cout << "Creating " << (unsigned)size << " nodes "<< std::endl;
  std::cout << "Nodes will be 175m apart."<< std::endl;
  nodes.Create (size);

  for (uint32_t i = 0; i < size; i++){
    std::ostringstream os;
    os << "node(" << i << ")";
    Names::Add (os.str (), nodes.Get (i));
  }

  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("175.0"),
                                 "Y", StringValue ("175.0"),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=125]"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);
}

/**
@param  none
@brief  The create devices function for AODV protocol. It creates and initializes all simulated devices.
**/
void AodvExample::CreateDevices ()
{
  WifiMacHelper wifiMac;
  wifiMac.SetType ("ns3::AdhocWifiMac");
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  wifiPhy.SetChannel (wifiChannel.Create ());
  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("OfdmRate6Mbps"), "RtsCtsThreshold", UintegerValue (0));
  devices = wifi.Install (wifiPhy, wifiMac, nodes);

  if (pcap)
  {
    wifiPhy.EnablePcapAll (std::string ("aodv"));
  }
}

/**
@param  none
@brief  The install internet stack function for AODV protocol. It creates the simulated network.
**/
void AodvExample::InstallInternetStack ()
{
  AodvHelper aodv;
  InternetStackHelper stack;
  stack.SetRoutingHelper (aodv);
  stack.Install (nodes);
  Ipv4AddressHelper address;
  address.SetBase ("10.0.0.0", "255.0.0.0");
  interfaces = address.Assign (devices);

  Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("aodv.routes", std::ios::out);
  aodv.PrintRoutingTableAllAt (Seconds (8), routingStream);
}

/**
@param  none
@brief  The install applications function for AODV protocol. It creates simulated data sinks and sources.
**/
void AodvExample::InstallApplications ()
{
  V4PingHelper ping (interfaces.GetAddress (size - 1));
  ping.SetAttribute ("Verbose", BooleanValue (true));

  ApplicationContainer p = ping.Install (nodes.Get (size/2));
  p.Start (Seconds (0));
  p.Stop (Seconds (duration) - Seconds (0.001));

  uint32_t numbers[size-2];
  int count = 0;
  for(uint32_t i = 0; i < size-2; i++, count++)
  {
    if(i == (size/2))
    {
      count++;
    }
    numbers[i]=count;
  }

  for(uint32_t i = 0; i < size-2; i++)
  {
    int k = rand() % (size-2);
    uint32_t temp = numbers[i];
    numbers[i] = numbers[k];
    numbers[k] = temp;
  }

  for (uint32_t i = 0; i < nodeRM; i++){
    Ptr<Node> node = nodes.Get (numbers[i]);
    Ptr<MobilityModel> mob = node->GetObject<MobilityModel> ();
    Simulator::Schedule (Seconds (duration/2), &MobilityModel::SetPosition, mob, Vector (rand()%10000, rand()%10000, rand()%10000));
  }
}

//--------------------------------------------------------------------------------------
//Class Functions (DSDV)
//--------------------------------------------------------------------------------------
/**
@param  none
@brief  The DSDV protocol constructor.
**/
DsdvExample::DsdvExample () :
  size (5),
  nodeRM (1),
  duration (50),
  pcap (false)
{
}

/**
@param  int argc, char **argv
@brief  The configuration function for DSDV protocol. Currently not used in this simulation.
**/
bool DsdvExample::Configure (int argc, char **argv)
{
  //Enable DSDV logs by default. Comment this if too noisy
  //LogComponentEnable("DsdvRoutingProtocol", LOG_LEVEL_ALL);

  CommandLine cmd;

  cmd.AddValue ("pcap", "Write PCAP traces.", pcap);
  cmd.AddValue ("size", "Number of nodes.", size);
  cmd.AddValue ("time", "Simulation time, s.", duration);
  cmd.AddValue ("fail", "Number of Nodes to move away.", nodeRM);

  cmd.Parse (argc, argv);
  if(size == 45)
  {
    SeedManager::SetSeed (8765);
  }
  else
  {
    SeedManager::SetSeed (1234);
  }
  return true;
}

/**
@param  none
@brief  The run function for DSDV protocol. Simulates the process by calling additional functions.
**/
void DsdvExample::Run ()
{
  //Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", UintegerValue (1));
  CreateNodes ();
  CreateDevices ();
  InstallInternetStack ();
  InstallApplications ();

  std::cout << "Starting simulation" << std::endl;

  Simulator::Stop (Seconds (duration));
  Simulator::Run ();
  Simulator::Destroy ();
}

/**
@param  none
@brief  The create node function for DSDV protocol. It creates the amount of nodes specified in the command line. Then puts them at a
	set distance.
**/
void DsdvExample::CreateNodes ()
{
  std::cout << "Creating " << (unsigned)size << " nodes "<< std::endl;
  std::cout << "Nodes will be 175m apart."<< std::endl;
  nodes.Create (size);

  for (uint32_t i = 0; i < size; i++){
    std::ostringstream os;
    os << "node(" << i << ")";
    Names::Add (os.str (), nodes.Get (i));
  }

  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("175.0"),
                                 "Y", StringValue ("175.0"),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=125]"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);
}

/**
@param  none
@brief  The create devices function for DSDV protocol. It creates and initializes all simulated devices.
**/
void DsdvExample::CreateDevices ()
{
  WifiMacHelper wifiMac;
  wifiMac.SetType ("ns3::AdhocWifiMac");
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  wifiPhy.SetChannel (wifiChannel.Create ());
  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("OfdmRate6Mbps"), "RtsCtsThreshold", UintegerValue (0));
  devices = wifi.Install (wifiPhy, wifiMac, nodes);

  if (pcap)
  {
    wifiPhy.EnablePcapAll (std::string ("dsdv"));
  }
}

/**
@param  none
@brief  The install internet stack function for DSDV protocol. It creates the simulated network.
**/
void DsdvExample::InstallInternetStack ()
{
  DsdvHelper dsdv;
  InternetStackHelper stack;
  stack.SetRoutingHelper (dsdv);
  stack.Install (nodes);
  Ipv4AddressHelper address;
  address.SetBase ("10.0.0.0", "255.0.0.0");
  interfaces = address.Assign (devices);

  Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("dsdv.routes", std::ios::out);
  dsdv.PrintRoutingTableAllAt (Seconds (8), routingStream);
}

/**
@param  none
@brief  The install applications function for DSDV protocol. It creates simulated data sinks and sources.
**/
void DsdvExample::InstallApplications ()
{
  V4PingHelper ping (interfaces.GetAddress (size - 1));
  ping.SetAttribute ("Verbose", BooleanValue (true));

  ApplicationContainer p = ping.Install (nodes.Get (size/2));
  p.Start (Seconds (0));
  p.Stop (Seconds (duration) - Seconds (0.001));

  uint32_t numbers[size-2];
  int count = 0;
  for(uint32_t i = 0; i < size-2; i++, count++)
  {
    if(i == (size/2))
    {
      count++;
    }
    numbers[i]=count;
  }

  for(uint32_t i = 0; i < size-2; i++)
  {
    int k = rand() % (size-2);
    uint32_t temp = numbers[i];
    numbers[i] = numbers[k];
    numbers[k] = temp;
  }

  for (uint32_t i = 0; i < nodeRM; i++)
  {
    Ptr<Node> node = nodes.Get (numbers[i]);
    Ptr<MobilityModel> mob = node->GetObject<MobilityModel> ();
    Simulator::Schedule (Seconds (duration/2), &MobilityModel::SetPosition, mob, Vector (rand()%10000, rand()%10000, rand()%10000));
  }
}
