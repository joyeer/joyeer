//
// Created by Qing Xu on 2022/1/4.
//

#ifndef __joyeer_runtime_executor_h__
#define __joyeer_runtime_executor_h__

#include "joyeer/runtime/isolate+vm.h"

constexpr int kStackMaxSize = 1024 * 1024 * 4; // 4M intptr_t;

struct Executor {

    intptr_t fp = 0;
    intptr_t sp = 0;
    IsolateVM *isolateVM;

    std::vector<Slot> frames {};
    char stack[kStackMaxSize] {};

};

#endif //__joyeer_runtime_executor_h__
