#include "classes.h"

using namespace std;



enum SocketStatus{Establishing,Transmitting,Finishing,Timeout};

int main(int argc, char *argv[]) {

/*    UDP_Segment tmp = UDP_Segment(true, false, false, 1, 'a');
    Sender sender = Sender("127.0.0.1", 8000);
    if (!sender.InitializeSocket())
        return 1;

    if(!sender.SendOne(tmp))
        return 2;

    if(!sender.WaitOne(tmp))
        return 3;

    cout<<tmp.Data<<endl;
    sender.Close();
    return 0;*/

    UDP_Segment segment;
    UDP_Segment ACK;
    string buffer = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int len = (int)buffer.length();
    Sender sender = Sender("127.0.0.1", 8000);
    if(!sender.InitializeSocket())
    {
        return  1;
    }
    sender.SetRecvTimeout(200);//timeout 200msec
    int cnt;
    cnt = 0;
    SocketStatus status = Establishing;
    while(true)
    {
        switch(status)
        {
            case Establishing:{
                segment = UDP_Segment(true,false,false,0,'s');
                cout<<"Establishing connection with receiver...(SYN)"<<endl;
                sender.SendOne(segment);
                if(sender.WaitOne(ACK))
                {
                    if(ACK.SYN)
                    {
                        cnt = 0;
                        status = Transmitting;
                    }
                }
                else
                    cnt++;
                if(cnt==25)
                {
                    status = Timeout;
                }
                continue;
            }
            case Transmitting:{
                if(ACK.SEQ<len) {
                    segment = UDP_Segment(false, false, false, ACK.SEQ + (uint8_t) 1, buffer[ACK.SEQ]);
                    cout<<"Sending Character: \""<<buffer[ACK.SEQ]
                    <<"\" with SEQ #"<<(ACK.SEQ+1)<<endl;
                    sender.SendOne(segment);
                    if(sender.WaitOne(ACK))
                    {
                        cnt=0;
                    }
                    else
                        cnt++;
                    if(cnt==25)
                        status = Timeout;
                }else {
                    cnt=0;
                    status = Finishing;
                }
                continue;
            }
            case Finishing:{
                if(cnt<25){
                    segment = UDP_Segment(false,false,true,ACK.SEQ+(uint8_t)1,'e');
                    cout<<"Sending FIN with SEQ #"<<(ACK.SEQ+1)<<endl;
                    sender.SendOne(segment);
                    if(sender.WaitOne(ACK)){
                        if(ACK.FIN)
                            cout<<"ACK with FIN received"<<endl;
                        return 0;
                    }
                    else
                        cnt++;
                }
                else{
                    status=Timeout;
                }
                continue;
            }
            case Timeout:{
                cout<<"Connection Timeout, release port to Idle..."<<endl;
                return 3;
            }
            default:{
                cerr<<"Fatal error: undefined socket status"<<endl;
                return 2;
            }
        }
    }
}
