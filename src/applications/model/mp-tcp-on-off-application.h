#ifndef MP_TCP_ON_OFF_APPLICATION_H
#define MP_TCP_ON_OFF_APPLICATION_H

#include "ns3/address.h"
#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"
#include "ns3/mp-tcp-socket-base.h"

namespace ns3 {

    class Address;

    class Socket;

    class MpTcpSocketBase;

    /**
     * The MpTcpOnOffApplication takes all logic from the MpTcpBulkSendApplication
     * and adds the OnOff logic, which is similar to the ns3 OnOffApplication
     *
     * The documentation of MpTcpBulkSendApplication is available at
     * src/applications/model/mp-tcp-bulk-send-application.h
     */

    class MpTcpOnOffApplication : public Application {
    public:
        /**
         * \brief Get the type ID.
         * \return the object TypeId
         */
        static TypeId GetTypeId(void);

        MpTcpOnOffApplication();

        virtual ~MpTcpOnOffApplication();

        /**
         * \brief Set the upper bound for the total number of bytes to send.
         *
         * Once this bound is reached, no more application bytes are sent. If the
         * application is stopped during the simulation and restarted, the
         * total number of bytes sent is not reset; however, the maxBytes
         * bound is still effective and the application will continue sending
         * up to maxBytes. The value zero for maxBytes means that
         * there is no upper bound; i.e. data is sent until the application
         * or simulation is stopped.
         *
         * \param maxBytes the upper bound of bytes to send
         */
        void SetMaxBytes(uint32_t maxBytes);

        /**
         * \brief Get the socket this application is attached to.
         * \return pointer to associated socket
         */
        Ptr <Socket> GetSocket(void) const;

        void SetBuffer(uint32_t buffSize);

    protected:
        virtual void DoDispose(void);

    private:
        // inherited from Application base class.
        virtual void StartApplication(void);    // Called at time specified by Start
        virtual void StopApplication(void);     // Called at time specified by Stop

        /**
         * \brief Send data until the L4 transmission buffer is full.
         */
        void SendData();

    public:
        Ptr <MpTcpSocketBase> m_socket;
        Address m_peer;         //!< Peer address
        bool m_connected;    //!< True if connected
        uint32_t m_sendSize;     //!< Size of data to send each time
        uint32_t m_maxBytes;     //!< Limit total number of bytes sent
        uint32_t m_totBytes;     //!< Total bytes sent so far
        TypeId m_tid;          //!< The type of protocol to use.
// uint8_t       *m_data;         // Application Buffer
// uint32_t       m_bufferSize;   // Application buffer size
        uint32_t m_dupack;
        uint32_t m_flowId;
        uint32_t m_subflows;
        uint32_t m_simTime;
        std::string m_flowType;
        std::string m_outputFileName;
        TracedCallback <Ptr<const Packet> > m_txTrace; // Traced Callback: sent packets

        //===================== On Off application implementation ===================//

        double m_lastSent;         //! time at which last packet has been sent in seconds
        uint64_t m_dataRate;       //! app data rate in bps
        bool m_isOn;               //! On/Off phase
        double m_start;            //! time at which app starts in seconds
        double m_stop;             //! time at which the app stops in seconds
        double m_onTime;           //! average duration of the ON phase in seconds
        double m_offTime;          //! average duration of the ON phase in seconds
        bool m_isUncoupled;        //! if app is using separate TCP connections



    private:
        /**
         * \brief Connection Succeeded (called by Socket through a callback)
         * \param socket the connected socket
         */
        void ConnectionSucceeded(Ptr <Socket> socket);

        /**
         * \brief Connection Failed (called by Socket through a callback)
         * \param socket the connected socket
         */
        void ConnectionFailed(Ptr <Socket> socket);

        /**
         * \brief Send more data as soon as some has been transmitted.
         */
        void DataSend(Ptr <Socket>, uint32_t); // for socket's SetSendCallback
        void HandlePeerError(Ptr <Socket> socket);

        void HandlePeerClose(Ptr <Socket> socket);

        // =================== On Off application implementation ============//

        /**
         * Creates On Off sending sequence
         */
        void CreateOnOffSequence(void);

        /**
         * Starts the Off phase
         */
        void turnOff(void);

        /**
         * Starts the ON phase
         */
        void turnOn(void);
    };

} // namespace ns3

#endif /* MPTCP_ON_OFF_APPLICATION_H */
