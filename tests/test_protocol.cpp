//
// Created by lml on 2018/7/11.
//

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include "test_util.h"
#include "../src/core/modules/easn/easn_package.h"

#define SERVER_PORT 40625

void test_connect(){
    int fd = socket(AF_INET,SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);  ///服务器端口
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  ///服务器ip

    //连接服务器，成功返回0，错误返回-1
    int err = connect(fd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if ( err  < 0)
    {
        throw "connect fail!";
    }
    //不授权的话10秒后应该断开连接了
    sleep(12);
    close(fd);
}

void test_auth(){
    int fd = socket(AF_INET,SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);  ///服务器端口
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  ///服务器ip

    //连接服务器，成功返回0，错误返回-1
    int err = connect(fd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if ( err  < 0)
    {
        throw "connect fail!";
    }
    std::string str = "{\"dd\":\"1-2378d8fh8a7d19suda9f\",\"appid\":\"xxxxxxxxxxxxxxxxxxx\",\"timestamp\":123456789412,\"sign\":\"xxxxxxxxxxx\"}";
    easn::packet_t pt;
    memset(pt.buf,0, sizeof(pt.buf));

    uint64_t data_pointer = reinterpret_cast<uint64_t>(&pt.info.data);
    uint64_t header_pointer = reinterpret_cast<uint64_t>(&pt.info.header);
    static const size_t crc_length = 2; //2字节
    size_t size = (data_pointer - header_pointer) + str.size() + crc_length;

    pt.info.type = 0x01; //授权
    pt.info.length = htons(size); //长度
    pt.info.index = htonl(1);//index
    pt.buf[0] = 0xaa;
    pt.buf[1] = 0x55;

    memcpy(&pt.info.data,str.c_str(),str.size());

    easn::fill_crc(pt.buf,size);

    printf("发送报文:");
    for(int i=0;i < size ; i++)
        printf("%x ",pt.buf[i]);
    printf("\n");

    write(fd,pt.buf,size);

    fd_set set;
    FD_ZERO(&set);
    FD_SET(fd, &set);
    struct timeval tv;
    tv.tv_sec = 10;//超时时间
    tv.tv_usec = 0;
    if (select(fd + 1, NULL, &set, NULL, &tv) > 0) {
        unsigned char buf[512];
        int len = read(fd,buf,512);

        if(len > 0){
            printf("返回报文:");
            for(int i=0;i < len ; i++)
                printf("%x ",buf[i]);
            printf("\n");
            close(fd);
        }else{
            close(fd);
            throw "认证超时";
        }
    }else{
        close(fd);
        throw "认证超时";
    }


}

int main(){

    const test_case test[] = {
//            {"test_connect",&test_connect },
            {"test_auth",&test_auth },
    };

    start_test(test , sizeof(test) / sizeof(test_case));


    return 0;
}
