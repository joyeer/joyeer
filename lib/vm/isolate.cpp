//
// Created by Qing Xu on 2021/11/11.
//

#include "joyeer/vm/isolate.h"
#include "joyeer/vm/interpreter.h"
#include "joyeer/vm/stdlib.h"

IsolateVM::IsolateVM() {
    gc = new GC();
}

IsolateVM::~IsolateVM() {
    delete gc;
}

void IsolateVM::run(const ModuleType::Ptr& module, CompilerService* compilerService) {
    ClassLoader classLoader(this, compilerService);

    // import string tables
    stringTable->import(compilerService->strings);

    // import methods
    for(const auto& type : compilerService->types) {
        if(type->kind == ValueType::Func) {
            auto funcType = std::static_pointer_cast<FuncType>(type);
            if(funcType->funcKind == C_Func) {
                if(type->address == static_cast<size_t>(BuildIns::Func_Print)) {
                    auto method = new Global_$_print();
                    methodTable->import(method, funcType);
                }
            } else {
                classLoader.compile(funcType);
            }

        }
    }

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

