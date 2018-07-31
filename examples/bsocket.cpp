//
// Created by lml on 2018/6/29.
//

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <arpa/inet.h>


int create_client_socket() {
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        //创建socket失败
        std::cerr << "socket创建失败:" << errno << " " << strerror(errno) << std::endl;
        return sockfd;
    }
    struct sockaddr_in ser_sockaddr;
    bzero(&ser_sockaddr, sizeof(ser_sockaddr));
    ser_sockaddr.sin_family = AF_INET;
    ser_sockaddr.sin_port = htons(8888);
    ser_sockaddr.sin_addr.s_addr = inet_addr("10.10.2.35");

//    if(set_block(sockfd) < 0){
//        std::cerr << "socket设置阻塞失败:" << errno << " " << strerror(errno) << std::endl;
//    }

    connect(sockfd, (struct sockaddr *)&ser_sockaddr, sizeof(ser_sockaddr));

    return sockfd;

}


int main(){

    char buf[ 1024 * 16 ] = "helloworld";
    int socket = create_client_socket();
    write(socket, buf, strlen(buf));
    std::cout << "正在转发数据" << std::endl;
    memset(buf,0, sizeof(buf));
    int ret = read(socket, buf, sizeof(buf));
    std::cout << "转发数据完成" << std::endl;

    return 0;

}