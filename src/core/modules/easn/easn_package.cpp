//
// Created by lml on 2018/7/4.
//

#include <cstdio>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <iomanip>
#include "easn_package.h"
#include "../../crc16.h"
#include "../../wf_log.h"

using namespace wf;
using namespace easn;

std::string easn::type_to_string(uint8_t type) {
    switch (type){
        case TYPE_INIT:
            return "AUTH";
        case TYPE_REPORT:
            return "REPORT";
        case TYPE_SWITCH:
            return "SWITCH";
        case TYPE_IP_CHANGE:
            return "CHANGE-IP";
    }
}

void easn::fill_ack_packet(ack_t & ack,uint8_t &type,uint32_t &index,uint16_t code){
    ack.info.type = type;
    ack.info.index = index;
    ack.info.code = htons(code) ;
    fill_crc(ack.buf,ntohs(ack.info.length));
}

void easn::fill_ack_packet(ack_t & dest,packet_t & src,uint16_t code){
    dest.info.type = src.info.type;
    dest.info.index = src.info.index;
    dest.info.code =  htons(code);
    fill_crc(dest.buf,ntohs(dest.info.length));
}

void easn::fill_crc(unsigned char * buf ,int size){
    uint16_t crc = crc16_compute(buf,size - 2);
    buf[size - 1] = crc & 0xff;
    buf[size - 2] = (crc >> 8) & 0xff;
}

bool easn::check_crc(unsigned char * buf ,int size){
    uint16_t crc = crc16_compute(buf,size - 2);
    unsigned char low = crc & 0xff;
    unsigned char high = (crc >> 8) & 0xff;

    if(low == buf[size-1] &&
            high == buf[size-2])
        return true;
    else
        return false;
}

size_t easn::sizeof_packet(packet_t & p){
    uint16_t size = ntohs(p.info.length);
    if(size > MAX_SIZE)
        size = MAX_SIZE;
    return size;
}

size_t easn::packet_min_size(){
    return sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint8_t) + 2 /*CRC16*/;
}

size_t easn::sizeof_data(packet_t & p) {
    uint64_t data_pointer = (uint64_t)&p.info.data;
    uint64_t header_pointer = (uint64_t)&p.info.header;
    static const size_t crc_length = 2; //2字节
    size_t size = sizeof_packet(p) - (data_pointer - header_pointer) - crc_length;
    if(size > 0)
        return size;
    return 0;
}

uint32_t easn::indexof_packet(packet_t & p) {
    return ntohl(p.info.index);
}

void easn::debug_print_packet(packet_t & p){
    for(int i = 0; i< sizeof_packet(p); i++)
        printf("%x ",p.buf[i]);
    printf("\n");

}

int easn::find_header_position(unsigned char *buf, int pos, int length) {
    for(int i=pos;i<(length-1);i++){
        if((buf[i] == 0xaa) && (buf[i+1] == 0x55)){
            return i;
        }
    }
    return -1; //没有找到
}