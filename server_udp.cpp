#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include "classes.h"

using namespace std;



int main(int argc, char *argv[]) {
    int server_sockfd;
    int len;
    struct sockaddr_in receiver_addr;   //服务器网络地址结构体
    struct sockaddr_in sender_addr; //客户端网络地址结构体
    int sin_size;
    char buf[3];  //数据传送的缓冲区
    memset(&receiver_addr, 0, sizeof(receiver_addr)); //数据初始化--清零
    receiver_addr.sin_family = AF_INET; //设置为IP通信
    receiver_addr.sin_addr.s_addr = INADDR_ANY;//服务器IP地址--允许连接到所有本地地址上
    receiver_addr.sin_port = htons(8000); //服务器端口号


    /*创建服务器端套接字--IPv4协议，面向无连接通信，UDP协议*/
    if ((server_sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        cerr << "socket error" << endl;
        return -1;
    }

    /*将套接字绑定到服务器的网络地址上*/
    if (bind(server_sockfd, (struct sockaddr *) &receiver_addr, sizeof(struct sockaddr)) < 0) {
        cerr << "bind error" << endl;
        return -2;
    }
    sin_size = sizeof(struct sockaddr_in);
    cout << "waiting for a packet at port:" << ntohs(receiver_addr.sin_port) << endl;

    /*接收客户端的数据并将其发送给客户端--recvfrom是无连接的*/
    if ((len = recvfrom(server_sockfd, buf, 3, 0, (struct sockaddr *) &sender_addr, (socklen_t *) &sin_size)) <
        0) {
        cerr << "receieve error" << endl;
        return -3;
    }
    cout << "received packet from:" << inet_ntoa(sender_addr.sin_addr) << ":" << ntohs(sender_addr.sin_port) << endl;
    //buf[len] = '\0';
    UDP_Segment tmp = UDP_Segment(buf);
    cout << "contents:" << tmp.Data << endl;

    /*关闭套接字*/
    close(server_sockfd);

    return 0;
}
