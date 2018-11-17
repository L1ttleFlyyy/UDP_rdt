#include <queue>
#include "classes.h"

using namespace std;

enum SocketStatus {
    Idle, Establishing, Transmitting, Finishing, Timeout
};

int main(int argc, char *argv[]) {

    Receiver receiver = Receiver("127.0.0.1", 8000);
    if (!receiver.InitializeSocket()) {
        receiver.Close();//Initialize Failed;
        return 1;
    }
    queue<char> buffer;//FIFO buffer
    UDP_Segment segment = UDP_Segment();
    UDP_Segment ACK;
    int cnt = 0;
    enum SocketStatus status = Idle;//Set socket status to Idle;

    /*Finite State Machine*/
    while (true) {
        switch (status) {

            case Idle: {
                receiver.SetTimeout(0);
                cout << "Receiver is ready at " << inet_ntoa(receiver.local_address.sin_addr)
                     << ": " << ntohs(receiver.local_address.sin_port) << endl;
                receiver.WaitOne(segment);
                if (segment.SYN) {
                    receiver.SetTimeout(5000);//timeout 5s;
                    cout << "SYN received from " << inet_ntoa(receiver.remote_address.sin_addr)
                         << ": " << ntohs(receiver.remote_address.sin_port) << endl;
                    status = Establishing;
                }
                continue;
            }

            case Establishing: {
                ACK = UDP_Segment(true, true, false, 0, 'a');
                cout << "Sending ACK with SEQ #0" << endl;
                receiver.SendOne(ACK);
                if (receiver.WaitOne(segment)) {
                    if (segment.SEQ > 0) {
                        buffer.push(segment.Data);
                        status = Transmitting;
                    }
                } else {
                    status = Timeout;
                }
                continue;
            }

            case Transmitting: {
                cout << "Data received: \"" << segment.Data << "\" with SEQ #" << segment.SEQ << endl;
                cout << "Sending ACK with SEQ #" << ACK.SEQ << endl;
                ACK = UDP_Segment(false, true, false, segment.SEQ, 'a');
                receiver.SendOne(ACK);
                if (receiver.WaitOne(segment)) {
                    if (segment.FIN) {
                        cout << "FIN received, connection terminating..." << endl;
                        cnt = 0;
                        status = Finishing;
                    } else if (segment.SEQ > buffer.size())
                        buffer.push(segment.Data);

                } else
                    status = Timeout;
                continue;
            }

            case Finishing: {
                if (cnt < 25) {
                    if (cnt == 0) {
                        receiver.SetTimeout(200);//200msec timeout for FIN
                        ACK = UDP_Segment(false, true, true, segment.SEQ, 'a');
                        cout << "Sending FIN with SEQ#" << ACK.SEQ << endl;
                        receiver.SendOne(ACK);
                        cnt++;
                        continue;
                    }

                    if (receiver.WaitOne(segment) && segment.FIN) {
                        cout << "Sending FIN with SEQ#" << ACK.SEQ << endl;
                        receiver.SendOne(ACK);
                    }
                    cnt++;
                } else {
                    cout << "Reception Complete:" << endl;
                    cout<<"\"";
                    while (!buffer.empty()) {
                        cout<<buffer.front();
                        buffer.pop();
                    }
                    cout<<"\""<<endl;
                    status = Idle;
                }
                continue;
            }

            case Timeout: {
                cout << "Connection Timeout, release port to Idle..." << endl;
                status = Idle;
                continue;
            }

            default: {
                cerr << "Fatal error: undefined socket status" << endl;
                return 2;
            }

        }
    }
}
