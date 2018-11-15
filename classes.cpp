#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include "classes.h"


UDP_Segment::UDP_Segment(bool SYN, bool ACK, bool FIN, uint8_t SEQ_Number, char Data) {
    this->SYN = SYN;
    this->ACK = ACK;
    this->FIN = FIN;
    this->SEQ = (char) SEQ_Number;
    this->Data = Data;
    if (SYN)
        Control_Byte = 1;
    else
        Control_Byte = 0;
    if (ACK)
        Control_Byte = Control_Byte & (uint8_t) 2;
    if (FIN)
        Control_Byte = Control_Byte & (uint8_t) 4;
    Raw_Data[0] = (char) Control_Byte;
    Raw_Data[1] = (char) SEQ;
    Raw_Data[2] = Data;
}

UDP_Segment::UDP_Segment(char Raw_Data[3]) {
    for (int i = 0; i < 3; i++) {
        this->Raw_Data[i] = Raw_Data[i];
    }
    Control_Byte = (uint8_t) Raw_Data[0];
    SEQ = (uint8_t) Raw_Data[1];
    Data = Raw_Data[2];
    SYN = Control_Byte & (uint8_t) 1;
    ACK = Control_Byte & (uint8_t) 2;
    FIN = Control_Byte & (uint8_t) 4;
}
