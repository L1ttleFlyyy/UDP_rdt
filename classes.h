//
// Created by mininet on 11/15/18.
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

class UDP_Segment {
public:
    bool SYN;
    bool ACK;
    bool FIN;
    uint8_t SEQ;
    char Data;
    char Raw_Data[3];

    UDP_Segment(bool SYN, bool ACK, bool FIN, uint8_t SEQ_Number, char Data);

    UDP_Segment(char Raw_Data[3]);

    UDP_Segment();

private:
    uint8_t Control_Byte;
};
#endif //CLASSES_H
