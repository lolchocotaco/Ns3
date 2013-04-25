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
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Lab 1");

int main (int argc, char *argv[])
{

    // Set up Variables
    double latency = 2.0; // 2ms daly
    uint64_t dataRate = 5000000; //5Mbps data rate
    // double interval = 0.02;   //Idk what this is

    /******** Note quite sure what this does********/
    CommandLine cmd;
    cmd.AddValue("Latency", "P2P link Latency in Milliseconds",latency);
    cmd.AddValue("Rate","P2P data rate in bps", dataRate);
    cmd.Parse(argc,argv);
    /*******************************************/


    //Enables logging for UDP client and Server
    LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);


    //Create Nodes
    NodeContainer nodes;
    nodes.Create (2);

    //Create Channels
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate",DataRateValue( DataRate (dataRate) ));
    pointToPoint.SetChannelAttribute ("Delay", TimeValue( MilliSeconds (latency) ));


    NetDeviceContainer devices;
    devices = pointToPoint.Install (nodes);

    InternetStackHelper stack;
    stack.Install (nodes);

    Ipv4AddressHelper address;
    address.SetBase ("10.1.1.0", "255.255.255.0");

    Ipv4InterfaceContainer interfaces = address.Assign (devices);

    UdpEchoServerHelper echoServer (9);
    UdpEchoServerHelper echoServer2 (80);

    ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
    serverApps.Start (Seconds (1.0));
    serverApps.Stop (Seconds (12.0));

    ApplicationContainer serverApps2 = echoServer2.Install (nodes.Get (1));
    serverApps2.Start (Seconds (1.0));
    serverApps2.Stop (Seconds (12.0));

    UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
    echoClient.SetAttribute ("MaxPackets", UintegerValue (5));
    echoClient.SetAttribute ("Interval", TimeValue (Seconds (2.0)));
    echoClient.SetAttribute ("PacketSize", UintegerValue (512));

    UdpEchoClientHelper echoClient2 (interfaces.GetAddress (1), 80);
    echoClient2.SetAttribute ("MaxPackets", UintegerValue (5));
    echoClient2.SetAttribute ("Interval", TimeValue (Seconds (2)));
    echoClient2.SetAttribute ("PacketSize", UintegerValue (1024));

    ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
    clientApps.Start (Seconds (1.0));
    clientApps.Stop (Seconds (11.0));

    ApplicationContainer clientApps2 = echoClient2.Install (nodes.Get (0));
    clientApps2.Start (Seconds (1.0));
    clientApps2.Stop (Seconds (11.0));

    std::cout<<"======RUNNING SIMULATION========="<<std::endl;

    Simulator::Run ();
    Simulator::Destroy ();
    return 0;
}
