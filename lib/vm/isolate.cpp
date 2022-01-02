//
// Created by Qing Xu on 2021/11/11.
//

#include "joyeer/vm/isolate.h"
#include "joyeer/vm/interpreter.h"
#include "joyeer/vm/stdlib.h"


IsolateVM::IsolateVM(CompilerService* compilerService) {
    gc = new GC();
}

IsolateVM::~IsolateVM() {
    delete gc;
}

void IsolateVM::run(const ModuleType* module) {
    initStdlib();
//    module->staticArea = gc->allocate(MemoryArea::Permanent, module);
}

//void IsolateVM::import(ModuleType *module) {
//    module->staticArea = gc->allocate(MemoryArea::Permanent, module);
//}

//void IsolateVM::initStdlib(CompilerService* compiler) {
//
//    assert(printFunc == nullptr);
//    printFunc = new Global_$_print();
//    methods->import(printFunc, compiler->getType(BuildIns::Func_Print));
//
//
//    assert(arrayClass == nullptr);
//    arrayClass = new ArrayClass();
//    auto arrayType = std::static_pointer_cast<ClassType>(compiler->getType(BuildIns::Object_Array));
//    classes->import(arrayType, arrayClass);
//
//    arrayGetFunc = new Array_$$_get();
//    methods->import(arrayGetFunc, compiler->getType(BuildIns::Object_Array_Func_get));
//
//    auto arraySizeFunc = new Array_$$_size();
//    methods->import(arraySizeFunc, compiler->getType(BuildIns::Object_Array_Func_size));
//
//    auto arraySetFunc = new Array_$$_set();
//    methods->import(arraySetFunc, compiler->getType(BuildIns::Object_Array_Func_set));
//
//}

