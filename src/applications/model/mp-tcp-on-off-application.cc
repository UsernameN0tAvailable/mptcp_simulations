#include "ns3/log.h"
#include "ns3/address.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/tcp-socket-factory.h"
#include "mp-tcp-on-off-application.h"
#include "ns3/string.h"
#include "ns3/random-variable-stream.h"
#include "ns3/mp-tcp-typedefs.h"


NS_LOG_COMPONENT_DEFINE ("MpTcpOnOffApplication");

namespace ns3 {

    NS_OBJECT_ENSURE_REGISTERED (MpTcpOnOffApplication);

    TypeId
    MpTcpOnOffApplication::GetTypeId(void) {
        static TypeId tid = TypeId("ns3::MpTcpOnOffApplication")
                .SetParent<Application>()
                .AddConstructor<MpTcpOnOffApplication>()
                .AddAttribute("SendSize",
                              "The amount of data to send each time from application buffer to socket buffer.",
                              UintegerValue(140000), //512
                              MakeUintegerAccessor(&MpTcpOnOffApplication::m_sendSize),
                              MakeUintegerChecker<uint32_t>(1))
//    .AddAttribute ("BufferSize", "The size of the application buffer.",
//                    UintegerValue(10),
//                    MakeUintegerAccessor(&MpTcpOnOffApplication::m_bufferSize),
//                    MakeUintegerChecker<uint32_t>(1))
                .AddAttribute("Remote", "The address of the destination",
                              AddressValue(),
                              MakeAddressAccessor(&MpTcpOnOffApplication::m_peer),
                              MakeAddressChecker())
                .AddAttribute("MaxBytes",
                              "The total number of bytes to send. "
                              "Once these bytes are sent, "
                              "no data  is sent again. The value zero means "
                              "that there is no limit.",
                              UintegerValue(0), // 1 MB default of data to send
                              MakeUintegerAccessor(&MpTcpOnOffApplication::m_maxBytes),
                              MakeUintegerChecker<uint32_t>())
                .AddAttribute("FlowId",
                              "Unique Id for each flow installed per node",
                              UintegerValue(0),
                              MakeUintegerAccessor(&MpTcpOnOffApplication::m_flowId),
                              MakeUintegerChecker<uint32_t>())
                .AddAttribute("MaxSubflows",
                              "Number of MPTCP subflows",
                              UintegerValue(8),
                              MakeUintegerAccessor(&MpTcpOnOffApplication::m_subflows),
                              MakeUintegerChecker<uint32_t>())
                .AddAttribute("SimTime",
                              "Simulation Time for this application it should be smaller than stop time",
                              UintegerValue(20),
                              MakeUintegerAccessor(&MpTcpOnOffApplication::m_simTime),
                              MakeUintegerChecker<uint32_t>())
                .AddAttribute("DupAck",
                              "Dupack threshold -- used only for MMPTCP and PacketScatter",
                              UintegerValue(0),
                              MakeUintegerAccessor(&MpTcpOnOffApplication::m_dupack),
                              MakeUintegerChecker<uint32_t>())
                .AddAttribute("FlowType",
                              "The Type of the Flow: Large or Short ",
                              StringValue("Large"),
                              MakeStringAccessor(&MpTcpOnOffApplication::m_flowType),
                              MakeStringChecker())
                .AddAttribute("OutputFileName",
                              "Output file name",
                              StringValue("NULL"),
                              MakeStringAccessor(&MpTcpOnOffApplication::m_outputFileName),
                              MakeStringChecker())
                .AddAttribute("Protocol", "The type of protocol to use.",
                              TypeIdValue(TcpSocketFactory::GetTypeId()),
                              MakeTypeIdAccessor(&MpTcpOnOffApplication::m_tid),
                              MakeTypeIdChecker())
                .AddAttribute("DataRate", "Sending Data Rate in bits per second", UintegerValue(20000000),
                              MakeUintegerAccessor(&MpTcpOnOffApplication::m_dataRate),
                              MakeUintegerChecker<uint64_t>())
                .AddAttribute("Start", "when the app starts",
                              DoubleValue(0),
                              MakeDoubleAccessor(&MpTcpOnOffApplication::m_start),
                              MakeDoubleChecker<double>())
                .AddAttribute("Stop", "when the app starts",
                              DoubleValue(0),
                              MakeDoubleAccessor(&MpTcpOnOffApplication::m_stop),
                              MakeDoubleChecker<double>())
                .AddAttribute("OnTime", "when the app is not active in seconds",
                              DoubleValue(1),
                              MakeDoubleAccessor(&MpTcpOnOffApplication::m_onTime),
                              MakeDoubleChecker<double>())
                .AddAttribute("OffTime", "when the app is not active in seconds",
                              DoubleValue(1),
                              MakeDoubleAccessor(&MpTcpOnOffApplication::m_offTime),
                              MakeDoubleChecker<double>())
                .AddAttribute("IsUncoupled", "when the app is not active in seconds",
                              BooleanValue(false),
                              MakeBooleanAccessor(&MpTcpOnOffApplication::m_isUncoupled),
                              MakeBooleanChecker())
                .AddTraceSource("Tx", "A new packet is created and is sent",
                                MakeTraceSourceAccessor(&MpTcpOnOffApplication::m_txTrace));
        return tid;
    }


    MpTcpOnOffApplication::MpTcpOnOffApplication()
            : m_socket(0),
              m_connected(false),
              m_totBytes(0),
              m_lastSent(0.0000001), // nothing happens before that
              m_isOn(true),
              m_isUncoupled(false){
        NS_LOG_FUNCTION(this);
//  m_data = new uint8_t[m_bufferSize];
        //memset(m_data, 0, m_sendSize*2);
    }

    MpTcpOnOffApplication::~MpTcpOnOffApplication() {
        NS_LOG_FUNCTION(this);
//  delete [] m_data;
//  m_data = 0;
    }

    void
    MpTcpOnOffApplication::SetBuffer(uint32_t buffSize) {
        NS_LOG_FUNCTION_NOARGS();
//  delete [] m_data;
//  m_data = 0;
//  m_data = new uint8_t[buffSize];
        //memset(m_data, 0, m_bufferSize);
    }

    void
    MpTcpOnOffApplication::SetMaxBytes(uint32_t maxBytes) {
        NS_LOG_FUNCTION(this << maxBytes);
        m_maxBytes = maxBytes;
    }

    Ptr <Socket>
    MpTcpOnOffApplication::GetSocket(void) const {
        NS_LOG_FUNCTION(this);
        return m_socket;
    }

    void
    MpTcpOnOffApplication::DoDispose(void) {
        NS_LOG_FUNCTION(this);

        m_socket = 0;
        Application::DoDispose(); // chain up
    }

// Application Methods
    void MpTcpOnOffApplication::StartApplication(void) // Called at time specified by Start
    {

        CreateOnOffSequence(); // creates on and off sequence

        NS_LOG_FUNCTION(this);
        //NS_LOG_UNCOND(Simulator::Now().GetSeconds() << " StartApplication -> Node-FlowId: {" << GetNode()->GetId() <<"-" << m_flowId<< "} MaxBytes: " << m_maxBytes << " F-Type: " << m_flowType << " S-Time: " << m_simTime);
        // Create the socket if not already
        if (!m_socket) {


            //m_socket = CreateObject<MpTcpSocketBase>(GetNode()); //m_socket = Socket::CreateSocket (GetNode (), m_tid);
            m_socket = DynamicCast<MpTcpSocketBase>(Socket::CreateSocket(GetNode(), m_tid));

            m_socket->Bind();
            //m_socket->SetMaxSubFlowNumber(m_subflows);
            m_socket->SetFlowType(m_flowType);
            m_socket->SetOutputFileName(m_outputFileName);
            int result = m_socket->Connect(m_peer);
            if (result == 0) {
                m_socket->SetFlowId(m_flowId);
                m_socket->SetDupAckThresh(m_dupack);
                m_socket->SetConnectCallback(MakeCallback(&MpTcpOnOffApplication::ConnectionSucceeded, this),
                                             MakeCallback(&MpTcpOnOffApplication::ConnectionFailed, this));
                m_socket->SetDataSentCallback(MakeCallback(&MpTcpOnOffApplication::DataSend, this));
                m_socket->SetCloseCallbacks(
                        MakeCallback(&MpTcpOnOffApplication::HandlePeerClose, this),
                        MakeCallback(&MpTcpOnOffApplication::HandlePeerError, this));

                //m_socket->SetSendCallback(MakeCallback(&MpTcpOnOffApplication::DataSend, this));
            } else {
                NS_LOG_UNCOND("Connection is failed");
            }
        }
        if (m_connected) {
            SendData();
        }
    }

    void MpTcpOnOffApplication::StopApplication(void) // Called at time specified by Stop
    {
        NS_LOG_FUNCTION(this);
        //NS_LOG_UNCOND(Simulator::Now().GetSeconds() << " ["<<m_node->GetId() << "] Application STOP");

        if (m_socket != 0) {
            m_socket->Close();
            m_connected = false;
        } else {
            NS_LOG_WARN("MpTcpOnOffApplication found null socket to close in StopApplication");
        }
    }

    void
    MpTcpOnOffApplication::HandlePeerClose(Ptr <Socket> socket) {
        //StopApplication();
        NS_LOG_UNCOND("*** [" << m_node->GetId() << "] HandlePeerError is called -> connection is false");
        m_connected = false;
        NS_LOG_FUNCTION(this << socket);
    }

    void
    MpTcpOnOffApplication::HandlePeerError(Ptr <Socket> socket) {
        //StopApplication();
        NS_LOG_UNCOND("*** [" << m_node->GetId() << "] HandlePeerError is called -> connection is false");
        m_connected = false;
        NS_LOG_FUNCTION(this << socket);
    }

// Private helpers
    void MpTcpOnOffApplication::SendData(void) {

        while (((m_maxBytes == 0 && m_socket->GetTxAvailable())
                || (m_totBytes < m_maxBytes && m_socket->GetTxAvailable()))
               && (((m_totBytes * 8) / m_lastSent) <= m_dataRate)
               && m_isOn && m_connected) { // Time to send more new data into MPTCP socket buffer

            /*
             * Determine amount of data to be sent in order to maintain the set data rate (m_dataRate)
             */
            uint32_t toSend = (((m_dataRate / 8) * Simulator::Now().GetSeconds()) - m_totBytes);

            if (m_maxBytes > 0) {
                uint32_t tmp = std::min(m_sendSize, m_maxBytes - m_totBytes);
                toSend = std::min(tmp, m_socket->GetTxAvailable());
            } else {
                toSend = std::min(m_sendSize, m_socket->GetTxAvailable());
            }
            int actual = m_socket->FillBuffer(toSend); // TODO Change m_totalBytes to toSend
            m_totBytes += actual;
            NS_LOG_DEBUG("toSend: " << toSend << " actual: " << actual << " totalByte: " << m_totBytes);
            m_socket->SendBufferedData();


            m_lastSent = Simulator::Now().GetSeconds();
        }
        if (m_totBytes == m_maxBytes && m_connected) {
            m_socket->Close();
            m_connected = false;
        }
    }

    void MpTcpOnOffApplication::ConnectionSucceeded(Ptr <Socket> socket) {
        NS_LOG_FUNCTION(this << socket);
        NS_LOG_LOGIC("MpTcpOnOffApplication Connection succeeded");
        m_connected = true;
        SendData();
    }

    void MpTcpOnOffApplication::ConnectionFailed(Ptr <Socket> socket) {
        NS_LOG_FUNCTION(this << socket);
        NS_LOG_LOGIC("MpTcpOnOffApplication, Connection Failed");
    }

    void MpTcpOnOffApplication::DataSend(Ptr <Socket>, uint32_t) {
        NS_LOG_FUNCTION(this);

        if (m_connected) { // Only send new data if the connection has completed
            Simulator::ScheduleNow(&MpTcpOnOffApplication::SendData, this);
        }
    }

    //============== ON OFF application implementation ==============//

    /**
     *  Creates On Off sequence
     */
    void MpTcpOnOffApplication::CreateOnOffSequence() {

        double time = m_start + 2;
        int loopIndex = 0;

        // Creates distribution for the Off phase duration
        Ptr <NormalRandomVariable> offRand = CreateObject<NormalRandomVariable>();
        offRand->SetAttribute("Mean", DoubleValue(m_offTime));
        offRand->SetAttribute("Variance", DoubleValue(0.1));

        // Creates distribution for the On phase duration
        Ptr <NormalRandomVariable> onRand = CreateObject<NormalRandomVariable>();
        onRand->SetAttribute("Mean", DoubleValue(m_onTime));
        onRand->SetAttribute("Variance", DoubleValue(0.1));

        // Creates sequence and schedules it with ns3 Simulator::Scheduler
        while (time < m_stop) {
            if (loopIndex % 2 == 0) {
                time += std::abs(onRand->GetValue()); // might turn up to be negative
                Simulator::Schedule(Seconds(time), &MpTcpOnOffApplication::turnOff, this);
            } else {
                time += std::abs(offRand->GetValue()); // might turn up to be negative
                Simulator::Schedule(Seconds(time), &MpTcpOnOffApplication::turnOn, this);
                Simulator::Schedule(Seconds(time),&MpTcpOnOffApplication::SendData, this);
            }
            loopIndex++;
        }
    }

    void MpTcpOnOffApplication::turnOff() {
        m_isOn = false;
    }

    void MpTcpOnOffApplication::turnOn() {
        m_isOn = true;
    }

} // Namespace ns3
