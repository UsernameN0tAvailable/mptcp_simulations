#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/ipv4-nix-vector-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/packet-sink.h"
#include "ns3/simulator.h"
#include "ns3/tcp-l4-protocol.h"
#include "ns3/random-variable-stream.h"
#include "ns3/node.h"
#include "ns3/global-value.h"
#include "ns3/enum.h"
#include "ns3/config-store-module.h"
#include "ns3/mobility-module.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("MpTcpOnOff");

//======================== Helper Methods ==================//

Ipv4AddressHelper ipv4;

/**
 * Assigns IP address to nodes
 *
 * @param a the second byte of the IP address
 * @param ndc NetDeviceContainer (Node) to which the
 * IP address is assigned
 * @return IP address interface
 */
Ipv4InterfaceContainer assignIp(int a, NetDeviceContainer ndc) {

    std::stringstream ss;
    ss << "10.1." << a << ".0";
    std::string s = ss.str();

    char *tab2 = new char[s.length() + 1];
    strcpy(tab2, s.c_str());
    ipv4.SetBase(tab2, "255.255.255.0");

    return ipv4.Assign(ndc);
}

/**
 * Stops the OFF phase Starts the ON Phase
 * on a connection between 2 nodes
 *
 * @param e1 node where the connection starts
 * @param e2 node where the connection ends
 * @param ber Bit Error Ratio to apply
 */
void onPhase(int e1, int e2, double ber) {
    // set error unit
    std::stringstream ss1;
    ss1 << "/NodeList/" << e1 << "/$ns3::Node/DeviceList/" << e2
        << "/$ns3::PointToPointNetDevice/ReceiveErrorModel/$ns3::RateErrorModel/ErrorUnit";
    Config::Set(ss1.str(), EnumValue(RateErrorModel::ERROR_UNIT_BIT));

    // set packet error ratio
    std::stringstream ss2;
    ss2 << "/NodeList/" << e1 << "/$ns3::Node/DeviceList/" << e2
        << "/$ns3::PointToPointNetDevice/ReceiveErrorModel/$ns3::RateErrorModel/ErrorRate";
    Config::Set(ss2.str(), DoubleValue(ber));
}

/**
 * Stops the ON Phase and starts the OFF Phase
 * on a connection between 2 nodes
 *
 * @param e1 node where the connection starts
 * @param e2 node where the connection ends
 * @param per Packet Error Ratio to apply
 */
void offPhase(int e1, int e2, double per) {
    std::stringstream ss1;
    ss1 << "/NodeList/" << e1 << "/$ns3::Node/DeviceList/" << e2
        << "/$ns3::PointToPointNetDevice/ReceiveErrorModel/$ns3::RateErrorModel/ErrorUnit";
    Config::Set(ss1.str(), EnumValue(RateErrorModel::ERROR_UNIT_PACKET));

    std::stringstream ss2;
    ss2 << "/NodeList/" << e1 << "/$ns3::Node/DeviceList/" << e2
        << "/$ns3::PointToPointNetDevice/ReceiveErrorModel/$ns3::RateErrorModel/ErrorRate";
    Config::Set(ss2.str(), DoubleValue(per));
}

/**
 * Creates ON/OFF sequence for ON/OFF Error Model
 * and for ON/OFF plus BER Error Model using the
 * NS3 Simulator::Schedule instance
 *
 * @param e1 node where the connection start
 * @param e1 node where the connection ends
 * @param onRV ON phase duration distribution
 * @param offRV OFF phase duration distribution
 * @param per Packet Error Ratio to apply to connection
 * @param ber Bit Error Ratio to apply to connection
 * @param stopSeconds time at which the simulation ends
 */
void
createSequence(int e1, int e2, double stopSeconds, Ptr <NormalRandomVariable> onRV, Ptr <NormalRandomVariable> offRV,
               double ber, double per) {

    double sum = 0;

    for (uint32_t i = 0; sum < stopSeconds; i++) {
        // Start OFF phase
        sum += onRV->GetValue();
        Simulator::Schedule(Seconds(sum), &offPhase, e1, e2, per);
        // Start ON Phase
        sum += offRV->GetValue();
        Simulator::Schedule(Seconds(sum), &onPhase, e1, e2, ber);
    }
}
//================= End Of Helper Methods ==================//

//================= Simulation Setup and Execution ==================//

int main(int argc, char *argv[]) {

    /*
     * Default Socket and Buffer Setups
     */

    // Set Flow Routing Strategy
    Config::SetDefault("ns3::Ipv4GlobalRouting::FlowEcmpRouting", BooleanValue(true));
    // Set MPTCP packet size
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(1000));
    // Set Buffer queuing mode
    Config::SetDefault("ns3::DropTailQueue::Mode", StringValue("QUEUE_MODE_PACKETS"));
    Config::SetDefault("ns3::DropTailQueue::MaxPackets", UintegerValue(100));
    // Set MPTCP Socket
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(MpTcpSocketBase::GetTypeId()));

    /*
     * Log this Component in order to extract throughput data
     */
    LogComponentEnable("MpTcpOnOff", LOG_ERROR );

    /*
     * Network Attributes
     */
    // Number of satellite Nodes
    uint32_t nSatelliteNodes = 6;
    // Number of dishes in the first layer of dishes
    uint32_t nDishes1 = 4;
    // Number of dishes in the second layer of dishes
    uint32_t nDishes2 = 4;
    // Number of routers in the first layer of routers
    uint32_t nRouters1 = 6;
    uint32_t nRouters2 = 6;
    uint32_t nTerminals1 = 5;
    uint32_t nTerminals2 = 5;
    std::string satsDelay = "275ms";
    std::string satsDataRate = "20Mbps";

    /*
     * Initialize Application attributes
     * Ideally this attributes are overridden
     * using options in the command line at
     * execution
     */
    double appStart = 0;
    double appStop = 10.0;

    /*
     * Error Model Attributes.
     * Ideally overridden by command
     * line options
     */
    double ber = 0.00001; // bit error ratio during ON phase or overall BER for BER error model
    double per = 0.01;    // packet error ratio during OFF phase
    double meanONDuration = 6; // mean On phase duration
    /*
     * MODE 1 := BER Error Model
     * MODE 2 := ON/OFF Error Model
     * MODE 3 := ON/OFF plus BER Error Model
     */
    int mode = 1;

    /*
     * Number of Subflows
     * Ideally overridden by command line
     * options
     */
    uint64_t nSF = 1;

    // Command Line Arguments
    CommandLine cmd;
    cmd.AddValue("ber", "Bit Error Ratio at satellite links", ber);
    cmd.AddValue("per", "Packet Error Ratio during the ON phase", per);
    cmd.AddValue("duration", "Simulation Duration in seconds", appStop);
    cmd.AddValue("nSF", "Number of subflows per MpTcp Socket", nSF);
    cmd.AddValue("meanONDuration", "Mean Duration in seconds of the ON Mode", meanONDuration);
    cmd.AddValue("mode", "1 -> BER, 2 -> ON/OFF, 3, -> BER + ON/OFF", mode);
    cmd.Parse(argc, argv);

    /*
     * Set the number of subflows after command line has been parsed
     */
    Config::SetDefault("ns3::MpTcpSocketBase::MaxSubflows", UintegerValue(nSF));

    /*
     * Set the time in seconds when the packet sink stops
     * We let 10 seconds in order to let the last packets
     * arrive at the sink and let the subflows close on
     * their own
     */
    double sinkStop = appStop + 10.0;

    /*
     *  Channel Settings
     */

    // Satellite - Dishes Channel Characteristics
    std::string upSatDataRate = satsDataRate;
    std::string upSatDelay = satsDelay;
    std::string downSatDataRate = satsDataRate;
    std::string downSatDelay = satsDelay;

    // Router - Dishes Char.
    std::string r1d1DataRate = "500Mbps";
    std::string r1d1Delay = "10ms";
    std::string d2r2DataRate = "500Mbps";
    std::string d2r2Delay = "10ms";

    // Terminal - Routers Char.
    std::string t1r1DataRate = "500Mbps";
    std::string t1r1Delay = "10ms";
    std::string r2t2DataRate = "500Mbps";
    std::string r2t2Delay = "10ms";

    // Define MPTCP port at host
    uint16_t mpTcpPort = 10;

    // Initialize Internet Stack and Routing Protocols
    InternetStackHelper internet;

    /*
     * helpers and Node containers
     *
     * */
    PointToPointHelper p2p;
    NodeContainer allNodes;

    // Create terminal Nodes on the left
    NodeContainer terminal1Nodes;
    terminal1Nodes.Create(nTerminals1);
    allNodes.Add(terminal1Nodes);

    // Create routers nodes on the left
    NodeContainer router1Nodes;
    router1Nodes.Create(nRouters1);
    allNodes.Add(router1Nodes);

    // Create dish nodes on the left
    NodeContainer dishes1Nodes;
    dishes1Nodes.Create(nDishes1);
    allNodes.Add(dishes1Nodes);

    // Create Satellite Nodes
    NodeContainer satNodes;
    satNodes.Create(nSatelliteNodes);
    allNodes.Add(satNodes);

    // Create dish nodes on the right
    NodeContainer dishes2Nodes;
    dishes2Nodes.Create(nDishes2);
    allNodes.Add(dishes2Nodes);

    // Create router nodes on the right
    NodeContainer router2Nodes;
    router2Nodes.Create(nRouters2);
    allNodes.Add(router2Nodes);

    // Create terminal Nodes on the right
    NodeContainer terminal2Nodes;
    terminal2Nodes.Create(nTerminals2);
    allNodes.Add(terminal2Nodes);

    /*
     * If ON/OFF Error Model
     * we set the overall BER to be 0
     * will be changed at run time accordingly
     */
    if (mode == 2) {
        ber = 0;
    }

    /*
     * Aplly the ON/OFF Error Model and BER plus ON/OFF Error Model
     */
    if (mode != 1) {
        /*
         * Create Normal Distributions to extract the ON and the OFF phase durations
         */
        Ptr <NormalRandomVariable> onRV = CreateObject<NormalRandomVariable>();
        onRV->SetAttribute("Mean", DoubleValue(meanONDuration));
        onRV->SetAttribute("Variance", DoubleValue(0.5));

        Ptr <NormalRandomVariable> offRV = CreateObject<NormalRandomVariable>();
        offRV->SetAttribute("Mean", DoubleValue(1));
        offRV->SetAttribute("Variance", DoubleValue(0.1));

        /*
         * Create sequence for every dish-to-satellite connection
         * Indices come from run-time NS3 variables found in the
         * output-attributes.txt in the root directory
         * The indices are simulation specific!
         */
        for (int e1 = 11; e1 < 25; e1++) {
            if (e1 < 15) {
                for (int e2 = 7; e2 < 13; e2++) {
                    createSequence(e1, e2, sinkStop, onRV, offRV, ber, per);
                }
            } else if (e1 < 21) {
                for (int e2 = 1; e2 < 8; e2++) {
                    createSequence(e1, e2, sinkStop, onRV, offRV, ber, per);
                }

            } else {
                for (int e2 = 1; e2 < 6; e2++) {
                    createSequence(e1, e2, sinkStop, onRV, offRV, ber, per);
                }
            }
        }
    }

    // Install internet stack to all nodes
    internet.Install(allNodes);


    /*
     * Connect nodes and assign IP addresses to the nodes
     */
    int connectionsCount = nTerminals1 * nRouters1 + nRouters1 * nDishes1
                           + nDishes1 * nSatelliteNodes + nSatelliteNodes * nDishes2
                           + nDishes2 * nRouters2 + nRouters2 * nTerminals2;

    Ipv4InterfaceContainer allIpv4Interfaces[connectionsCount];
    int count = 1;

    // connect terminal to routers
    for (uint32_t i = 0; i < nTerminals1; i++) {
        for (uint32_t j = 0; j < nRouters1; j++) {
            p2p.SetDeviceAttribute("DataRate", StringValue(t1r1DataRate));
            p2p.SetChannelAttribute("Delay", StringValue(t1r1Delay));
            allIpv4Interfaces[count - 1] = assignIp(count,
                                                    p2p.Install(terminal1Nodes.Get(i), router1Nodes.Get(j)));
            count++;
        }
    }

    // connect routers to dishes
    for (uint32_t i = 0; i < nRouters1; i++) {
        for (uint32_t j = 0; j < nDishes1; j++) {
            p2p.SetDeviceAttribute("DataRate", StringValue(r1d1DataRate));
            p2p.SetChannelAttribute("Delay", StringValue(r1d1Delay));
            allIpv4Interfaces[count - 1] = assignIp(count,
                                                    p2p.Install(router1Nodes.Get(i), dishes1Nodes.Get(j)));
            count++;
        }
    }

    // connect dishes to satellites on the left
    for (uint32_t i = 0; i < nDishes1; i++) {
        for (uint32_t j = 0; j < nSatelliteNodes; j++) {
            p2p.SetDeviceAttribute("DataRate", StringValue(upSatDataRate));
            p2p.SetChannelAttribute("Delay", StringValue(upSatDelay));

            NetDeviceContainer dds = p2p.Install(dishes1Nodes.Get(i), satNodes.Get(j));

            // Apply Error Model
            Ptr <RateErrorModel> rem = CreateObject<RateErrorModel>();
            Ptr <UniformRandomVariable> uv = CreateObject<UniformRandomVariable>();
            rem->SetRandomVariable(uv);
            rem->SetRate(ber);
            rem->SetUnit(RateErrorModel::ERROR_UNIT_BIT);
            Ptr <RateErrorModel> rem1 = CreateObject<RateErrorModel>();
            Ptr <UniformRandomVariable> uv1 = CreateObject<UniformRandomVariable>();
            rem1->SetRandomVariable(uv1);
            rem1->SetRate(ber);
            rem1->SetUnit(RateErrorModel::ERROR_UNIT_BIT);

            dds.Get(0)->SetAttribute("ReceiveErrorModel", PointerValue(rem));
            dds.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(rem1));

            allIpv4Interfaces[count - 1] = assignIp(count, dds);
            count++;
        }
    }

    // connects satellite to dishes on the right
    for (uint32_t i = 0; i < nSatelliteNodes; i++) {
        for (uint32_t j = 0; j < nDishes2; j++) {
            p2p.SetDeviceAttribute("DataRate", StringValue(downSatDataRate));
            p2p.SetChannelAttribute("Delay", StringValue(downSatDelay));
            NetDeviceContainer dds = p2p.Install(satNodes.Get(i), dishes2Nodes.Get(j));

            // Apply the Error Model
            Ptr <RateErrorModel> rem = CreateObject<RateErrorModel>();
            Ptr <UniformRandomVariable> uv = CreateObject<UniformRandomVariable>();
            rem->SetRandomVariable(uv);
            rem->SetRate(ber);
            rem->SetUnit(RateErrorModel::ERROR_UNIT_BIT);

            Ptr <RateErrorModel> rem1 = CreateObject<RateErrorModel>();
            Ptr <UniformRandomVariable> uv1 = CreateObject<UniformRandomVariable>();
            rem1->SetRandomVariable(uv1);
            rem1->SetRate(ber);
            rem1->SetUnit(RateErrorModel::ERROR_UNIT_BIT);
            dds.Get(0)->SetAttribute("ReceiveErrorModel", PointerValue(rem));
            dds.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(rem1));

            allIpv4Interfaces[count - 1] = assignIp(count, dds);
            count++;
        }
    }

    // connect dishes to routers
    for (uint32_t i = 0; i < nDishes2; i++) {
        for (uint32_t j = 0; j < nRouters2; j++) {
            p2p.SetDeviceAttribute("DataRate", StringValue(d2r2DataRate));
            p2p.SetChannelAttribute("Delay", StringValue(d2r2Delay));
            allIpv4Interfaces[count - 1] = assignIp(count,
                                                    p2p.Install(dishes2Nodes.Get(i), router2Nodes.Get(j)));
            count++;
        }
    }

    // connect routers to terminals
    for (uint32_t i = 0; i < nRouters2; i++) {
        for (uint32_t j = 0; j < nTerminals2; j++) {
            p2p.SetDeviceAttribute("DataRate", StringValue(r2t2DataRate));
            p2p.SetChannelAttribute("Delay", StringValue(r2t2Delay));
            allIpv4Interfaces[count - 1] = assignIp(count,
                                                    p2p.Install(router2Nodes.Get(i), terminal2Nodes.Get(j)));
            count++;
        }
    }

    // Create router nodes, initialize routing database and set up the routing
    // tables in the nodes.
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // All send apps
    ApplicationContainer allSendApps[nTerminals1];
    for(uint32_t index = 0; index < nTerminals1; index++){
        MpTcpOnOffHelper source("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address(allIpv4Interfaces[connectionsCount - (index +1)].GetAddress(1)), mpTcpPort));
        source.SetAttribute("MaxBytes", UintegerValue(0));
        source.SetAttribute("DataRate", UintegerValue(20000000));
        source.SetAttribute("Start",DoubleValue(appStart));
        source.SetAttribute("Stop",DoubleValue(appStop));
        source.SetAttribute("OnTime",DoubleValue(0.5));
        source.SetAttribute("OffTime",DoubleValue(2.2));

        allSendApps[index] = source.Install(terminal1Nodes.Get(index));
        allSendApps[index].Start(Seconds(appStart));
        allSendApps[index].Stop(Seconds(appStop));
    }

    // All sinks
    ApplicationContainer allsinkApps[nTerminals1];
    for(uint32_t index = 0; index < nTerminals2; index++){
        MpTcpPacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), mpTcpPort));
        allsinkApps[index] = sink.Install(terminal2Nodes.Get(index));
        allsinkApps[index].Start(Seconds(appStart));
        allsinkApps[index].Stop(Seconds(sinkStop));
    }

    /*
     * Setup at Simulation Start
     */
    Config::SetDefault("ns3::ConfigStore::Filename",
                       StringValue("output-attributes.txt"));
    Config::SetDefault("ns3::ConfigStore::FileFormat", StringValue("RawText"));
    Config::SetDefault("ns3::ConfigStore::Mode", StringValue("Save"));
    ConfigStore outputConfig2;
    outputConfig2.ConfigureDefaults();
    outputConfig2.ConfigureAttributes();

    /*
     *
     * Flow Monitor
     *
     */
    Ptr <FlowMonitor> flowMonitor;
    FlowMonitorHelper flowHelper;
    flowMonitor = flowHelper.InstallAll();

    /*
     * Run Simulation
     */
    Simulator::Stop(Seconds(sinkStop));
    Simulator::Run();
    Simulator::Destroy();

    /*
     * Throughput Computation
     */
    uint32_t totData = 0;
    double totTime = 0;


    for (uint32_t i = 0; i < nTerminals2; i++) {

        uint64_t sinkData = (DynamicCast<MpTcpPacketSink>(allsinkApps[i].Get(0)))->GetTotalRx();
        double lastRx = (DynamicCast<MpTcpPacketSink>(allsinkApps[i].Get(0)))->GetLastRxTime();
        totData += sinkData;
        totTime += lastRx;
    }

    double outputData = (totData / (totTime / nTerminals1)) * 8;

    std::stringstream ss1;
    ss1 << outputData;
    std::string outputstream = ss1.str();

    /*
     * Output Throughput in bps
     * NS_LOG_ERROR in order to distinguish the output
     * from other NS_LOG outputs that might occur
     */
    NS_LOG_ERROR(outputstream);

    return 0;
}
