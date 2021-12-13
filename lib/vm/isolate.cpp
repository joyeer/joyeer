//
// Created by Qing Xu on 2021/11/11.
//

#include "joyeer/vm/isolate.h"
#include "joyeer/vm/interpreter.h"

IsolateVM::IsolateVM() {
    gc = new GC();
}

IsolateVM::~IsolateVM() {
    delete gc;
}

void IsolateVM::run(const ModuleType::Ptr& module, CompilerService* compilerService) {
    // import string tables
    stringTable->import(compilerService->strings);
    methodTable->import(compilerService);

    ClassLoader classLoader(this, compilerService);
    auto moduleClass = classLoader.load(module);
    run(moduleClass);
}

void IsolateVM::run(const ModuleClass* moduleClass) {
    Executor executor(this);
    executor.execute(moduleClass);
}


void IsolateVM::import(ModuleClass *moduleClass) {
    moduleClass->staticArea = gc->allocate(MemoryArea::Permanent, moduleClass);
}

