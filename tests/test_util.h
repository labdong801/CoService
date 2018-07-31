//
// Created by lml on 2018/7/5.
//

#ifndef PROJECT_TEST_UTIL_H
#define PROJECT_TEST_UTIL_H

typedef struct{
    const char * name;
    void (*func)() ;
} test_case;

void start_test(const test_case * test,int size);

void assert_equal(uint8_t expect,uint8_t actual);
void assert_equal(int expect,int actual);
void assert_equal(bool expect,bool actual);


#endif //PROJECT_TEST_UTIL_H
