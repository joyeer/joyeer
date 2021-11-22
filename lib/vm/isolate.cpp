//
// Created by Qing Xu on 2021/11/11.
//

#include "joyeer/vm/isolate.h"

IsolateVM::IsolateVM() {
    vCompiler = new VCompiler(this);
    gc = new GC();
}

IsolateVM::~IsolateVM() {
    delete vCompiler;
    delete gc;
}

void IsolateVM::run(JrFileModuleType::Ptr fileModule) {
    vCompiler->compile(fileModule);
}
