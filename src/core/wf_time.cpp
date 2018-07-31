//
// Created by lml on 2018/7/10.
//

#include "wf_time.h"
#include <stdio.h>
#include <sys/time.h>
using namespace wf;

uint64_t time::timestamp() {
    struct timeval tv;
    gettimeofday(&tv,NULL);    //该函数在sys/time.h头文件中
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}