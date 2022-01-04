//
// Created by Qing Xu on 2022/1/4.
//

#include "joyeer/runtime/loader.h"

ClassLoader::ClassLoader(IsolateVM *vm):
    vm(vm) {
}

void ClassLoader::load(Class *klass) {
    klass->staticArea = vm->gc->allocate(MemoryArea::Permanent, klass);
}