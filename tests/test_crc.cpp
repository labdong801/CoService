//
// Created by lml on 2018/7/4.
//

#include <iostream>
#include "test_util.h"
#include "../src/core/modules/easn/easn_package.h"

//测试计算crc
void test_crc_cal(){
    unsigned char data[] = {0xAA,0x55,0x01,0x00,0x0D,0x00,0x00,0x00,0x02,0x00,0x01,0x00,0x00};
    int size = sizeof(data);
    easn::fill_crc(data, size);

    assert_equal(true,(data[size-1] == 0x81) && (data[size-2] == 0xdb));
}

//测试计算crc
void test_check_crc(){
    unsigned char data[] = {0xAA,0x55,0x01,0x00,0x0D,0x00,0x00,0x00,0x02,0x00,0x01,0xDB,0x81};
    int size = sizeof(data);
    assert_equal(true,easn::check_crc(data, size));
}

//测试计算crc
void test_check_crc2(){
    unsigned char data[] = {0xAA,0x55,0x01,0x00,0x0D,0x00,0x00,0x00,0x02,0x00,0x01,0xDB,0x82};
    int size = sizeof(data);
    assert_equal(false,easn::check_crc(data, size));
}

int main(){

    const test_case test[] = {
            {"test_crc_cal",&test_crc_cal },
            {"test_check_crc2",&test_check_crc2 },
            {"test_check_crc",&test_check_crc },

    };

    start_test(test , sizeof(test) / sizeof(test_case));

    return 0;
}