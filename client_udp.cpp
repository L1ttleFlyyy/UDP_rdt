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
    }

    bool InitializeSocket() {
        if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
            cerr << "Failed to create socket" << endl;
            return false;
        }
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
        if (recvfrom(sockfd, buf, 3, 0, (struct sockaddr *) &remote_address, &socklen) < 0) {
            return false;
        } else {
            udp_segment = UDP_Segment(buf);
            return true;
        }
    }

    bool SetSendTimeout(int milliseconds)
    {
        timeval tv;
        tv.tv_sec = milliseconds/1000;
        tv.tv_usec = (milliseconds-tv.tv_sec)*1000;
        if(setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,&tv, sizeof(tv))<0){
            cerr<<"Timeout setting failed"<<endl;
            return false;
        }else return true;
    }

    bool SetRecvTimeout(int milliseconds)
    {
        timeval tv;
        tv.tv_sec = milliseconds/1000;
        tv.tv_usec = (milliseconds-tv.tv_sec)*1000;
        if(setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&tv, sizeof(tv))<0){
            cerr<<"Timeout setting failed"<<endl;
            return false;
        }else return true;
    }

    void Close()
    {
        close(sockfd);
    }

private:
    struct sockaddr_in remote_address;
    int sockfd;
    socklen_t socklen;
};

int main(int argc, char *argv[]) {

    UDP_Segment tmp = UDP_Segment(true, false, false, 1, 'a');
    Sender sender = Sender("127.0.0.1", 8000);
    if (!sender.InitializeSocket())
        return 1;

    if(!sender.SendOne(tmp))
        return 2;

    if(!sender.WaitOne(tmp))
        return 3;

    cout<<tmp.Data<<endl;
    sender.Close();
    return 0;

}
