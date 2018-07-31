//
// Created by lml on 2018/7/4.
//

#ifndef PROJECT_PACKAGE_H
#define PROJECT_PACKAGE_H

#include <stdint-gcc.h>
#include <iostream>

static const int MAX_SIZE = 1024 * 16;
namespace easn{

    typedef enum {
        TYPE_INIT = 0x01,
        TYPE_REPORT,
        TYPE_SWITCH,
        TYPE_IP_CHANGE,
    } wf_cmd_type;

    typedef enum {
        SUCCESS = 0,
        CRC_ERROR,
    } wf_return_code;

    #pragma pack (1)
    typedef union {
        unsigned char buf[MAX_SIZE];
        struct{
            uint16_t header;
            uint8_t type;
            uint32_t index;
            uint16_t length;
            uint8_t data;
        }info;
    } packet_t;

    typedef union {
        unsigned char buf[13] = {0xaa,0x55,/*type*/0x00,/*index*/0x00,0x00,0x00,0x00,/*length*/0x00,0x0d,0x00,0x00,0x00,0x00};
        struct{
            uint16_t header;
            uint8_t type;
            uint32_t index;
            uint16_t length;
            uint16_t code;
            uint16_t crc;
        }info;
    } ack_t;

    #pragma pack ()

    //数据包类型转字符串
    std::string type_to_string(uint8_t);

    //打印数据包
    void debug_print_packet(packet_t &);

    //查找头部位置（数据头必须以0xaa 0x55 开头）
    int find_header_position(unsigned char * buf,int pos,int length);

    //数据包最小值
    size_t packet_min_size();

    //数据包大小
    size_t sizeof_packet(packet_t &);

    //内容大小
    size_t sizeof_data(packet_t &);

    //index
    uint32_t indexof_packet(packet_t &);


    //填充数据包
    void fill_ack_packet(ack_t & dest,uint8_t &type,uint32_t &index,uint16_t code);
    //填充数据包2
    void fill_ack_packet(ack_t & dest,packet_t & src,uint16_t code);

    //填充crc
    void fill_crc(unsigned char * buf ,int size);

    //校验crc
    bool check_crc(unsigned char * buf ,int size);

}



#endif //PROJECT_PACKAGE_H
