#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include "classes.h"

using namespace std;

class Sender {
public:
    Sender(string remote_address, uint16_t remote_port) {
        memset(&this->remote_address, 0, sizeof(sockaddr_in));
        this->remote_address.sin_addr.s_addr = inet_addr(remote_address.c_str());
        this->remote_address.sin_port = htons(remote_port);
        this->remote_address.sin_family = AF_INET;
        socklen = sizeof(struct sockaddr);
        timeout.tv_sec = 0;
        timeout.tv_usec = 200000;// 200msec
    }
    bool InitializeSocket() {
        if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
            cerr << "socket error" << endl;
            return false;
        } else if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
            cerr << "Timeout setting failed" << endl;
            return false;
        } else
            cout << "Socket Created" << endl;
        return true;
    }

    bool SendOne(UDP_Segment udp_segment) {
        if (sendto(sockfd, udp_segment.Raw_Data, 3, 0, (struct sockaddr *)&remote_address,socklen) < 0) {
            return false;
        } else
            return true;
    }

    bool WaitOne(UDP_Segment &udp_segment) {
        char buf[3];
        if (recvfrom(sockfd, buf, 3, 0, (struct sockaddr *) &remote_address, (socklen_t *) &socklen) < 0) {
            return false;
        } else {
            udp_segment = UDP_Segment(buf);
            return true;
        }
    }

    void Close()
    {
        close(sockfd);
    }

private:
    timeval timeout;
    struct sockaddr_in remote_address;
    int sockfd;
    int socklen;



};

int main(int argc, char *argv[]) {
/*
    int client_sockfd;
    int len;
    struct sockaddr_in remote_addr;
    int sin_size;
    char buf[BUFSIZ];
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    remote_addr.sin_port = htons(8000);

    if ((client_sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        cerr << "socket error" << endl;
        return 1;
    }
    strcpy(buf, "This is a test message");
    cout << "sending: " << buf << endl;
    sin_size = sizeof(struct sockaddr_in);

    if ((len = sendto(client_sockfd, buf, strlen(buf), 0, (struct sockaddr *) &remote_addr, sizeof(struct sockaddr))) <
        0) {
        cerr << "sending error" << endl;
        return 1;
    }

    close(client_sockfd);
*/

    UDP_Segment tmp = UDP_Segment(true, false, false, 1, 'a');
    Sender sender = Sender("127.0.0.1", 8000);
    if (sender.InitializeSocket()) {
        if(sender.SendOne(tmp))
            return 0;
        else
            return -1;
    } else
        return -1;
}
