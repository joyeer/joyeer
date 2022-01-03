//
// Created by Qing Xu on 2022/1/2.
//

#include "joyeer/runtime/isolate+vm.h"


IsolateVM::IsolateVM() {
    gc = new GC();
}

IsolateVM::~IsolateVM() {
    delete gc;
}
