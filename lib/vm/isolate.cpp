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
    // import string tables
    stringTable->import(compilerService->strings);

    ClassLoader classLoader(this, compilerService);
    classLoader.load(module);
}


void IsolateVM::register_(const ModuleType::Ptr &moduleType, Class *klass) {
    klass->slotID = static_cast<int>(classes.size());
    classes.push_back(klass);
    mapOfTypeAndClass[moduleType->address] = klass->slotID;
}

void IsolateVM::register_(const FuncType::Ptr &funcType, Method *method) {
    method->slotID = static_cast<int>(methods.size());
    methods.push_back(method);
    mapOfTypeAndMethod[funcType->address] = method->slotID;
}

void IsolateVM::import(ModuleClass *moduleClass) {
    gc->allocate(MemoryArea::Permanent, moduleClass);
}

const Class *IsolateVM::query(const ClassType::Ptr &classType) {
    auto klassSlotID = mapOfTypeAndClass[classType->address];
    return const_cast<Class *>(classes[klassSlotID]);
}

const Method *IsolateVM::query(const FuncType::Ptr &funcType) {
    if(!mapOfTypeAndMethod.contains(funcType->address)) {
        return nullptr;
    }
    auto funcSlotID = mapOfTypeAndMethod[funcType->address];
    return const_cast<Method*>(methods[funcSlotID]);
}


