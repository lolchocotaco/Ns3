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

NS_LOG_COMPONENT_DEFINE ("Proj 1");

int main (int argc, char *argv[])
{

    // Set up Variables
    double latency = 2.0; // 2ms daly
    uint64_t dataRate = 5000000; //5Mbps data rate
    double interval = 0.02;   //Inter Packet Arrival Time

    /******** Note quite sure what this does********/
    CommandLine cmd;
    cmd.AddValue("latency", "P2P link Latency in Milliseconds",latency);
//    cmd.AddValue("Rate","P2P data rate in bps", dataRate);
//    cmd.addValue("Interval", "UDP client Packet inverval", interval);
    cmd.Parse(argc,argv);
    /*******************************************/


    //Enables logging for UDP client and Server
    LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);


    //Create Nodes
    NS_LOG_INFO("Create Nodes");
    NodeContainer nodes;
    nodes.Create (2);

    //Create Channels
    NS_LOG_INFO("Create Channels");
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate",DataRateValue( DataRate (dataRate) ));
    pointToPoint.SetChannelAttribute ("Delay", TimeValue( MilliSeconds (latency) ));
//    NetDeviceContainer devices = pointToPoint.Install(nodes.Get(0), nodes.Get(1) );  Good for specific connections

    //Installs all nodes. 
    NS_LOG_INFO("Install Nodes");
    NetDeviceContainer devices;
    devices = pointToPoint.Install (nodes);

    InternetStackHelper stack;
    stack.Install (nodes);

    Ipv4AddressHelper address;
    address.SetBase ("10.1.1.0", "255.255.255.0");

    Ipv4InterfaceContainer interfaces = address.Assign (devices);
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();


    //Create Applications 
    //nodes.Get(1) == Server
    NS_LOG_INFO("Create Applications");
    uint16_t port1 = 9;
    uint16_t port2 = 80;
    UdpEchoServerHelper echoServer (port1);
    UdpEchoServerHelper echoServer2 (port2);

    ApplicationContainer serverApps;
    serverApps = echoServer.Install (nodes.Get(1));    
    serverApps = echoServer2.Install(nodes.Get(1));
    serverApps.Start (Seconds (1.0));
    serverApps.Stop (Seconds (12.0));


    //Client application to send from node 0 to 1
    uint32_t maxPacketSize = 1024;
    Time interPacketTime = Seconds(interval);
    uint32_t maxPacketCount = 5;

    UdpEchoClientHelper echoClient (interfaces.GetAddress (1), port1);
    UdpEchoClientHelper echoClient2 (interfaces.GetAddress (1), port2);

    echoClient.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
    echoClient.SetAttribute ("Interval", TimeValue (interPacketTime));
    echoClient.SetAttribute ("PacketSize", UintegerValue (maxPacketSize));

    echoClient2.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
    echoClient2.SetAttribute ("Interval", TimeValue (interPacketTime));
    echoClient2.SetAttribute ("PacketSize", UintegerValue (maxPacketSize));

    ApplicationContainer clientApps;
    clientApps = echoClient.Install (nodes.Get (0));
    clientApps = echoClient2.Install(nodes.Get (0));
    clientApps.Start (Seconds (1.0));
    clientApps.Stop (Seconds (11.0));

    //Tracing Probably not needed
    AsciiTraceHelper ascii;
    pointToPoint.EnableAscii(ascii.CreateFileStream ("./scratch/lab1Res/lab1Trace.tr") ,devices);
    pointToPoint.EnablePcap("proj1",devices,false);

    //Calculating ThroughPut
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();



    NS_LOG_INFO("Run Simulation");
    std::cout<<"======LATENCY ="<<latency<<"=========="<<std::endl;

    Simulator::Stop (Seconds(12.0));
    Simulator::Run ();
    
    monitor->CheckForLostPackets ();

    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i){
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
        if ((t.sourceAddress=="10.1.1.1" && t.destinationAddress == "10.1.1.2")){
            std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
            std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
            std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
            std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/1024  << " Mbps\n";
        }
    }

    monitor->SerializeToXmlFile("./scratch/lab1Res/lab1.flowmon", true, true);
    Simulator::Destroy ();
    return 0;
}
