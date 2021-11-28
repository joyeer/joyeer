//
// Created by Qing Xu on 2021/11/11.
//

#include "joyeer/vm/isolate.h"


IsolateVM::IsolateVM() {
    gc = new GC();
}

IsolateVM::~IsolateVM() {
    delete gc;
}

void IsolateVM::run(const ModuleType::Ptr& module, CompilerService* compilerService) {

    ClassLoader classLoader(this, compilerService);
    classLoader.load(module);
}


void IsolateVM::register_(const ModuleType::Ptr &moduleType, const ModuleClass *moduleClass) {

}

void IsolateVM::import(ModuleClass *moduleClass) {
    gc->allocate(MemoryArea::Permanent, moduleClass);
}
