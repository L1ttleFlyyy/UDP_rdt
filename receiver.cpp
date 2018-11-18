#include <queue>
#include "classes.h"

int main(int argc, char *argv[]) {

    Receiver receiver = Receiver("10.0.0.2", 11714);
    if (!receiver.InitializeSocket()) {
        receiver.Close();//Initialize Failed;
        return 1;
    }
    queue<char> buffer;//FIFO queue buffer
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
                if (segment.SYN) {//When SYN received
                    receiver.SetTimeout(5000);//timeout 5s;
                    cout << "SYN received from " << inet_ntoa(receiver.remote_address.sin_addr)
                         << ": " << ntohs(receiver.remote_address.sin_port) << endl;
                    status = Establishing;
                }
                continue;
            }

            case Establishing: {
                ACK = UDP_Segment(true, true, false, segment.SEQ, 'a');
                cout << "Sending ACK with SEQ #" << segment.SEQ << endl;
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
                cout << "Sending ACK with SEQ #" << segment.SEQ << endl;
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
                if (cnt < 25) {//Wait for 25 times
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
                } else {//5 sec elapsed
                    cout << "Reception Complete:" << endl;
                    cout << "\"";
                    while (!buffer.empty()) {
                        cout << buffer.front();
                        buffer.pop();//Print FIFO queue
                    }
                    cout << "\"" << endl;
                    status = Idle;
                }
                continue;
            }

            case Timeout: {//If the port doesn't receive any packet in 5 sec, back to Idle 
                cout << "Connection Timeout, release port to Idle..." << endl;
                while (!buffer.empty())
                    buffer.pop();
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
