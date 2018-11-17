#include "classes.h"

using namespace std;

enum SocketStatus{Idle,Establishing,Transmitting,Finishing,Timeout};

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
    enum SocketStatus status = Idle;
    Receiver receiver = Receiver("127.0.0.1",8000);
    UDP_Segment segment = UDP_Segment();
    UDP_Segment ACK;
    int cnt =0;
    if(!receiver.InitializeSocket())
        return 1;
    while(true)
    {
        switch (status){
            case Idle:{
                receiver.SetTimeout(0);
                cout<<"Receiver is ready at "<<inet_ntoa(receiver.local_address.sin_addr)
                <<": "<<ntohs(receiver.local_address.sin_port)<<endl;
                receiver.WaitOne(segment);
                if(segment.SYN)
                {
                    receiver.SetTimeout(5000);//timeout 5s;
                    cout<<"SYN received from "<<inet_ntoa(receiver.remote_address.sin_addr)
                    <<": "<<ntohs(receiver.remote_address.sin_port)<<endl;
                    status = Establishing;
                }
                continue;
            }
            case Establishing:
            {
                ACK = UDP_Segment(true,true,false,0,'a');
                cout<<"Sending ACK with SEQ #0"<<endl;
                receiver.SendOne(ACK);
                if(receiver.WaitOne(segment))
                {
                    if(segment.SEQ>0)
                    {
                        status = Transmitting;
                    }
                } else{
                    status = Timeout;
                }
                continue;
            }
            case Transmitting:{
                cout<<"Data received: \""<<segment.Data<<"\" with SEQ #"<<(int)segment.SEQ<<endl;
                ACK = UDP_Segment(false,true,false,segment.SEQ,'a');
                cout<<"Sending ACK with SEQ #"<<(int)ACK.SEQ<<endl;
                receiver.SendOne(ACK);
                if(receiver.WaitOne(segment))
                {
                    if(segment.FIN)
                    {
                        cout<<"FIN received, connection terminating..."<<endl;
                        receiver.SetTimeout(200);//200ms timeout for FIN
                        ACK = UDP_Segment(false,true,true,segment.ACK,'a');
                        cout << "Sending FIN" << endl;
                        receiver.SendOne(ACK);
                        cnt = 0;
                        status = Finishing;
                    }
                }
                else
                    status = Timeout;
                continue;
            }
            case Finishing:{
                if(cnt<25) {
                    if (receiver.WaitOne(segment)) {
                        cout << "Sending FIN" << endl;
                        receiver.SendOne(ACK);
                    }
                    cnt++;
                }else
                    status = Idle;
                continue;
            }
            case Timeout:{
                cout<<"Connection Timeout, release port to Idle..."<<endl;
                status = Idle;
                continue;
            }
            default:{
                cerr<<"Fatal error: undefined socket status"<<endl;
                return 2;
            }
        }
    }
}
