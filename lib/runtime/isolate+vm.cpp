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

void IsolateVM::bootstrap() {
    arrayClass = static_cast<ArrayClass *>((*types)[(int) BuildIns::Object_Array]);
}