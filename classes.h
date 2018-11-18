//
// Created by Chang Xu on 11/15/18.
//

#ifndef CLASSES_H
#define CLASSES_H
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
using namespace std;


enum SocketStatus {
    Idle, Establishing, Transmitting, Finishing, Timeout
};

class UDP_Segment {
public:
    bool SYN;
    bool ACK;
    bool FIN;
    uint SEQ;
    char Data;
    char Raw_Data[6];

    UDP_Segment(bool SYN, bool ACK, bool FIN, uint SEQ_Number, char Data);

    explicit UDP_Segment(char Raw_Data[6]);

    UDP_Segment();

private:
    uint8_t Control_Byte;
};

class Sender
{
public:
    Sender(string remote_address, uint16_t remote_port);
    bool InitializeSocket();
    bool SendOne(UDP_Segment udp_segment);
    bool WaitOne(UDP_Segment &udp_segment);
    bool SetTimeout(int milliseconds);
    void Close();

private:
    struct sockaddr_in remote_address;
    int sockfd;
    socklen_t socklen;
};

class Receiver
{
public:
    Receiver(string local_address, uint16_t remote_port);
    bool InitializeSocket();
    bool SendOne(UDP_Segment udp_segment);
    bool WaitOne(UDP_Segment &udp_segment);
    bool SetTimeout(int milliseconds);
    void Close();
    struct sockaddr_in remote_address;
    struct sockaddr_in local_address;
private:
    int sockfd;
    socklen_t socklen;
};

#endif //CLASSES_H
