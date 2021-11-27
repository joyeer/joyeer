//
// Created by Qing Xu on 2021/11/11.
//

#include "joyeer/vm/isolate.h"


IsolateVM::IsolateVM() {
    classLoader = new ClassLoader(this);
    gc = new GC();
}

IsolateVM::~IsolateVM() {
    delete classLoader;
    delete gc;
}

void IsolateVM::run(const ModuleType::Ptr& module, CompilerService* compilerService) {
    classLoader->compile(module, compilerService);
}
