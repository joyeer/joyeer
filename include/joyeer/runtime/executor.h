//
// Created by Qing Xu on 2022/1/4.
//

#ifndef __joyeer_runtime_executor_h__
#define __joyeer_runtime_executor_h__

#include "joyeer/runtime/isolate+vm.h"

constexpr int kStackMaxSize = 1024 * 1024 * 4; // 4M intptr_t;

struct StackFrameSnapshot {
    AddressPtr fp = 0;
    AddressPtr sp = 0;
};

struct Executor {

    AddressPtr fp = 0;
    AddressPtr sp = 0;
    IsolateVM *vm = nullptr;

    std::vector<Slot> frames {};
    char stack[kStackMaxSize] {};

    // return current stack frame numbers
    size_t getStackFrameCount() const;

    // return stack frame snapshot
    StackFrameSnapshot getStackFrameSnapshot(int index) const;

};

#endif //__joyeer_runtime_executor_h__
