#include "classes.h"

int main(int argc, char *argv[]) {

    string buffer;//where data temporally saved
    int len;//buffer length
    do {//input check
        cout << "Please input at least 20 characters:" << endl;
        cin >> buffer;
    } while ((len = buffer.length()) < 20);

    Sender sender = Sender("10.0.0.2", 11714);
    if (!sender.InitializeSocket()) {
        sender.Close();
        return 1;
    }

    sender.SetTimeout(200);//timeout 200msec
    int cnt = 0;
    UDP_Segment segment;
    UDP_Segment ACK;
    SocketStatus status = Establishing;

    /*Finite State Machine*/
    while (true) {
        switch (status) {
            case Establishing: {
                segment = UDP_Segment(true, false, false, 0, 's');
                cout << "Establishing connection with receiver...(SYN)" << endl;
                sender.SendOne(segment);
                if (sender.WaitOne(ACK) && ACK.ACK) {
                    if (ACK.SYN) {//SYN ACK received
                        cnt = 0;
                        status = Transmitting;
                    }
                } else
                    cnt++;
                if (cnt == 25) {//Only try 25 times retransimission then abort process
                    status = Timeout;
                }
                continue;
            }
            case Transmitting: {
                if (ACK.SEQ < len) {
                    segment = UDP_Segment(false, false, false, ACK.SEQ + (uint8_t) 1, buffer[ACK.SEQ]);
                    cout << "Sending Character: \"" << buffer[ACK.SEQ]
                         << "\" with SEQ #" << (ACK.SEQ + 1) << endl;
                    sender.SendOne(segment);
                    if (sender.WaitOne(ACK) && ACK.ACK) {
                        cnt = 0;
                    } else {
                        cnt++;
                        cout << "Window timeout..." << endl;
                    }
                    if (cnt == 25)//At most retransmit 25 times
                        status = Timeout;
                } else {
                    cnt = 0;
                    status = Finishing;
                }
                continue;
            }
            case Finishing: {
                if (cnt < 25) {//Only retransmit at most 25 times
                    segment = UDP_Segment(false, false, true, ACK.SEQ + 1, 'e');
                    cout << "Sending FIN with SEQ #" << (ACK.SEQ + 1) << endl;
                    sender.SendOne(segment);
                    if (sender.WaitOne(ACK)) {
                        if (ACK.FIN && ACK.ACK) {//FIN ACK received
                            cout << "ACK with FIN received" << endl;
                            sender.Close();
                            return 0;
                        }
                    } else
                        cnt++;
                } else {
                    status = Timeout;
                }
                continue;
            }
            case Timeout: {
                cout << "Connection Timeout, process abort..." << endl;
                sender.Close();
                return 3;
            }
            default: {
                cerr << "Fatal error: undefined socket status" << endl;
                return 2;
            }
        }
    }
}
