#include "classes.h"

/*UDP_Segment Definition*/
//Generate UDP segment by setting;
UDP_Segment::UDP_Segment(bool SYN, bool ACK, bool FIN, uint SEQ_Number, char Data) {
    this->SYN = SYN;
    this->ACK = ACK;
    this->FIN = FIN;
    this->SEQ = SEQ_Number;
    this->Data = Data;
    if (FIN)
        Control_Byte = 1;
    else
        Control_Byte = 0;
    if (SYN)
        Control_Byte = Control_Byte | (uint8_t) 2;
    if (ACK)
        Control_Byte = Control_Byte | (uint8_t) 4;
    Raw_Data[0] = (char) Control_Byte;
    Raw_Data[1] = (char) SEQ;//Bits[7:0]
    Raw_Data[2] = (char) SEQ >> 8;//Bits[15:8]
    Raw_Data[3] = (char) SEQ >> 16;//Bits[23:16]
    Raw_Data[4] = (char) SEQ >> 24;//Bits[31:24]
    Raw_Data[5] = Data;
}

//Generate UDP segment by raw data;
UDP_Segment::UDP_Segment(char Raw_Data[6]) {
    for (int i = 0; i < 6; i++) {
        this->Raw_Data[i] = Raw_Data[i];
    }
    Control_Byte = (uint8_t) Raw_Data[0];
    SEQ = (uint) Raw_Data[1];//Bits[7:0]
    SEQ += ((uint) Raw_Data[2] << 8);//Bits[15:8]
    SEQ += ((uint) Raw_Data[3] << 16);//Bits[23:16]
    SEQ += ((uint) Raw_Data[4] << 24);//Bits[31:24]
    Data = Raw_Data[5];
    FIN = Control_Byte & (uint8_t) 1;
    SYN = Control_Byte & (uint8_t) 2;
    ACK = Control_Byte & (uint8_t) 4;
}


UDP_Segment::UDP_Segment() {
    Control_Byte = 0;
    SEQ = 0;
    Data = 0;
    SYN = false;
    ACK = false;
    FIN = false;
}

/*Sender Definition*/
Sender::Sender(string remote_address, uint16_t remote_port) {
    memset(&this->remote_address, 0, sizeof(sockaddr_in));
    this->remote_address.sin_addr.s_addr = inet_addr(remote_address.c_str());
    this->remote_address.sin_port = htons(remote_port);
    this->remote_address.sin_family = AF_INET;
    socklen = sizeof(struct sockaddr);
}

bool Sender::InitializeSocket() {
    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        cerr << "Failed to create socket" << endl;
        return false;
    }
    cout << "Socket Created" << endl;
    return true;
}

bool Sender::SendOne(UDP_Segment udp_segment) {
    if (sendto(sockfd, udp_segment.Raw_Data, 6, 0, (struct sockaddr *) &remote_address, socklen) < 0) {
        return false;
    } else
        return true;
}

bool Sender::WaitOne(UDP_Segment &udp_segment) {
    char buf[6];
    if (recvfrom(sockfd, buf, 6, 0, (struct sockaddr *) &remote_address, &socklen) < 0) {
        return false;
    } else {
        udp_segment = UDP_Segment(buf);
        return true;
    }
}

bool Sender::SetTimeout(int milliseconds) {
    timeval tv;
    tv.tv_sec = milliseconds / 1000;
    tv.tv_usec = (milliseconds - 1000 * tv.tv_sec) * 1000;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        cerr << "Timeout setting failed" << endl;
        return false;
    } else return true;
}

void Sender::Close() {
    close(sockfd);
}


/*Receiver Definition*/
Receiver::Receiver(string local_address, uint16_t remote_port) {
    memset(&this->local_address, 0, sizeof(sockaddr_in));
    this->local_address.sin_addr.s_addr = inet_addr(local_address.c_str());
    this->local_address.sin_port = htons(remote_port);
    this->local_address.sin_family = AF_INET;
    socklen = sizeof(struct sockaddr);
}

bool Receiver::InitializeSocket() {
    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        cerr << "socket error" << endl;
        return false;
    }
    if ((bind(sockfd, (sockaddr *) &local_address, socklen)) < 0) {
        cerr << "bind error" << endl;
        return false;
    }
    cout << "Socket Created" << endl;
    return true;
}

bool Receiver::SendOne(UDP_Segment udp_segment) {
    if (sendto(sockfd, udp_segment.Raw_Data, 6, 0, (struct sockaddr *) &remote_address, socklen) < 0) {
        return false;
    } else
        return true;
}

bool Receiver::WaitOne(UDP_Segment &udp_segment) {
    char buf[6];
    if (recvfrom(sockfd, buf, 6, 0, (struct sockaddr *) &remote_address, &socklen) < 0) {
        return false;
    } else {
        udp_segment = UDP_Segment(buf);
        return true;
    }
}

bool Receiver::SetTimeout(int milliseconds) {
    timeval tv;
    tv.tv_sec = milliseconds / 1000;
    tv.tv_usec = (milliseconds - 1000 * tv.tv_sec) * 1000;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        cerr << "Timeout setting failed" << endl;
        return false;
    } else return true;
}

void Receiver::Close() {
    close(sockfd);
}