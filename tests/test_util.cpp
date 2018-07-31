//
// Created by lml on 2018/7/5.
//

#include <iostream>
#include <sstream>
#include "test_util.h"

void start_test(const test_case * test,int len){
    int fail = 0; //失败个数
    for(int i=0;i<len;i++){
        try{
            std::cout << "# Case " << i + 1 << " -> "  << test[i].name << std::endl << "====================================" << std::endl;
            test[i].func();
            std::cout << "====================================" << std::endl;
            std::cout << "Result:" << "Pass" << std::endl << std::endl << std::endl;
        }catch (std::string s){
            fail++;
            std::cerr << s.c_str() << std::endl;
            std::cout << "====================================" << std::endl;
            std::cout << "Result:" << "Fail" << std::endl << std::endl << std::endl;
        }catch (const char * s){
            fail++;
            std::cerr << s << std::endl;
            std::cout << "====================================" << std::endl;
            std::cout << "Result:" << "Fail" << std::endl << std::endl << std::endl;
        }

    }
    std::cout << "Total:" << len - fail << "/" << len << std::endl;
}

void assert_equal(int expect,int actual){
    std::stringstream ss;
    if(expect != actual){
        ss << "expect:[" << expect << "] But got:[" << actual << "]";
        throw ss.str();
    }
}
void assert_equal(bool expect,bool actual){
    std::stringstream ss;
    if(expect != actual){
        ss << "expect:[" << expect << "] But got:[" << actual << "]";
        throw ss.str();
    }
}

void assert_equal(uint8_t expect,uint8_t actual){
    std::stringstream ss;
    if(expect != actual){
        ss << "expect:[" << std::hex << expect << "] But got:[" << std::hex << actual << "]";
        throw ss.str();
    }
}
