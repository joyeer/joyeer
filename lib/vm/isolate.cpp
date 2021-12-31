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
    initStdlib(compilerService);
    ClassLoader classLoader(this, compilerService);

    // import string tables
    strings->import(compilerService->strings);

    // import methods
    for(const auto& type : compilerService->types) {
        if(type->kind == ValueType::Func) {
            auto funcType = std::static_pointer_cast<FuncType>(type);
            if(funcType->funcKind == VM_Func) {
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

void IsolateVM::initStdlib(CompilerService* compiler) {

    assert(printFunc == nullptr);
    printFunc = new Global_$_print();
    methods->import(printFunc, compiler->getType(BuildIns::Func_Print));


    assert(arrayClass == nullptr);
    arrayClass = new ArrayClass();
    auto arrayType = std::static_pointer_cast<ClassType>(compiler->getType(BuildIns::Object_Array));
    classes->import(arrayType, arrayClass);

    arrayGetFunc = new Array_$$_get();
    methods->import(arrayGetFunc, compiler->getType(BuildIns::Object_Array_Func_get));

    auto arraySizeFunc = new Array_$$_size();
    methods->import(arraySizeFunc, compiler->getType(BuildIns::Object_Array_Func_size));



}

