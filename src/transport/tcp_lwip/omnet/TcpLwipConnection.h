//
// Copyright (C) 2010 Zoltan Bojthe
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//


#ifndef __TCP_LWIP_CONNECTION_H
#define __TCP_LWIP_CONNECTION_H

#include <omnetpp.h>

#include "IPvXAddress.h"
#include "lwip/tcp.h"
#include "TCPSegment.h"
#include "TCPCommand_m.h"

// forward declarations:
class TCP_lwip;
class TCPConnectInfo;
class TcpLwipReceiveQueue;
class TcpLwipSendQueue;
class INetStack;
class INetStreamSocket;
class TCPStatusInfo;

/**
 *
 */
class INET_API TcpLwipConnection
{
  protected:
    class Stats
    {
      public:
        Stats();
        ~Stats();
        void recordSend(const TCPSegment &tcpsegP);
        void recordReceive(const TCPSegment &tcpsegP);

      protected:
        // statistics
        cOutVector sndWndVector;   // snd_wnd
        cOutVector sndSeqVector;   // sent seqNo
        cOutVector sndAckVector;   // sent ackNo
        cOutVector sndSacksVector;  // number of sent Sacks

        cOutVector rcvWndVector;   // rcv_wnd
        cOutVector rcvSeqVector;   // received seqNo
        cOutVector rcvAdvVector;   // current advertised window (=rcv_avd)
        cOutVector rcvAckVector;   // received ackNo (= snd_una)
        cOutVector rcvSacksVector;  // number of received Sacks

        cOutVector unackedVector;  // number of bytes unacknowledged

        cOutVector dupAcksVector;   // current number of received dupAcks
        cOutVector pipeVector;      // current sender's estimate of bytes outstanding in the network
        cOutVector rcvOooSegVector; // number of received out-of-order segments

        cOutVector sackedBytesVector;        // current number of received sacked bytes
        cOutVector tcpRcvQueueBytesVector;   // current amount of used bytes in tcp receive queue
        cOutVector tcpRcvQueueDropsVector;   // number of drops in tcp receive queue
    };
    // prevent copy constructor:
    TcpLwipConnection(const TcpLwipConnection&);

  public:
    TcpLwipConnection(TCP_lwip &tcpLwipP, int connIdP, int gateIndexP, TCPDataTransferMode dataTransferModeP);

    TcpLwipConnection(TcpLwipConnection &tcpLwipConnectionP, int connIdP, LwipTcpLayer::tcp_pcb *pcbP);

    ~TcpLwipConnection();

    void sendEstablishedMsg();

    static const char* indicationName(int code);

    void sendIndicationToApp(int code);

    void listen(TCPOpenCommand &tcpCommand);

    void connect(TCPOpenCommand &tcpCommand);

    void process_CLOSE();

    void process_ABORT();

    void process_SEND(cPacket *msgP);

    void process_READ(TCPReadCommand &tcpCommandP);

    void fillStatusInfo(TCPStatusInfo &statusInfo);

    void notifyAboutSending(const TCPSegment& tcpsegP);

    int send_data(void *data, int len);

    void do_SEND();

    long getReceiveBufferSize() { return receiveBufferSizeM; }
    bool isExplicitReadsEnabled() { return explicitReadsEnabledM; }

    INetStreamSocket* getSocket();

    /** Utility: send available received data to App */
    void sendToApp(cMessage* msgP);

    /** Utility: send available received data to App */
    void sendDataToApp();

    /** Utility: send sent byte count and length of send queue to App */
    void dataSent(unsigned int sentBytesP);

    void initStats();

  public:
    int connIdM;
    int appGateIndexM;
    LwipTcpLayer::tcp_pcb *pcbM;
    TcpLwipSendQueue *sendQueueM;
    TcpLwipReceiveQueue *receiveQueueM;
    TCP_lwip &tcpLwipM;

  protected:
    long int totalSentM;
    bool isListenerM;
    bool onCloseM;

    bool explicitReadsEnabledM;      // when enabled: TCP send up only TCPDataArrivedInfo notification when received some data, and send the packet only after a READ msg.
    bool sendNotificationsEnabledM;  // when enabled: when TCP sent some data to partner, it's send up a TCPDataSentInfo notification.
    bool sendingObjectUpAtFirstByteEnabledM; // when enabled: when TCPDataTransferMode is "object", then TCP send up object at first byte of msg
    long receiveBufferSizeM;         // max size of used receive queue in TCP layer (SO_RCVBUF). only valid when explicitReadsEnabled is true
    ulong readBytesM;                // bytecount of last READ

    Stats * statsM;
};

#endif
