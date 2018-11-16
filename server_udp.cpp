#include "classes.h"

using namespace std;

class Receiver {
public:
    Receiver(string local_address, uint16_t remote_port) {
        memset(&this->local_address, 0, sizeof(sockaddr_in));
        this->local_address.sin_addr.s_addr = inet_addr(local_address.c_str());
        this->local_address.sin_port = htons(remote_port);
        this->local_address.sin_family = AF_INET;
        socklen = sizeof(struct sockaddr);
    }

    bool InitializeSocket() {
        if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
            cerr << "socket error" << endl;
            return false;
        }
        if((bind(sockfd,(sockaddr*)&local_address,socklen))<0)
        {
            cerr<<"bind error"<<endl;
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

    void Close()
    {
        close(sockfd);
    }

private:
    struct sockaddr_in remote_address;
    struct sockaddr_in local_address;
    int sockfd;
    socklen_t socklen;
};

int main(int argc, char *argv[]) {
/*    int server_sockfd;
    int len;
    struct sockaddr_in receiver_addr;
    struct sockaddr_in sender_addr;
    int sin_size;
    char buf[3];
    memset(&receiver_addr, 0, sizeof(receiver_addr));
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_addr.s_addr = INADDR_ANY;
    receiver_addr.sin_port = htons(8000);


    if ((server_sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        cerr << "socket error" << endl;
        return -1;
    }

    if (bind(server_sockfd, (struct sockaddr *) &receiver_addr, sizeof(struct sockaddr)) < 0) {
        cerr << "bind error" << endl;
        return -2;
    }
    sin_size = sizeof(struct sockaddr_in);
    cout << "waiting for a packet at port:" << ntohs(receiver_addr.sin_port) << endl;

    if ((len = recvfrom(server_sockfd, buf, 3, 0, (struct sockaddr *) &sender_addr, (socklen_t *) &sin_size)) <
        0) {
        cerr << "receieve error" << endl;
        return -3;
    }
    cout << "received packet from:" << inet_ntoa(sender_addr.sin_addr) << ":" << ntohs(sender_addr.sin_port) << endl;
    //buf[len] = '\0';
    UDP_Segment tmp = UDP_Segment(buf);
    cout << "contents:" << tmp.Data << endl;
    close(server_sockfd);

    return 0;*/
    Receiver reciever = Receiver("127.0.0.1",8000);
    char buf[3]{7,0,'z'};
    UDP_Segment tmp;
    if(!reciever.InitializeSocket())
        return 1;
    if(!reciever.WaitOne(tmp))
        return 2;
    cout<<tmp.Data<<endl;
    if(!reciever.SendOne(UDP_Segment(buf)))
        return 3;
    reciever.Close();
    return 0;
}
