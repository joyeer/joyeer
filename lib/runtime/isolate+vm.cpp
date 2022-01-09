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
    dictClass = static_cast<DictClass*>((*types)[(int) BuildIns::Object_Dict]);
    dictEntryClass = static_cast<DictEntry*>((*types)[(int) BuildIns::Object_DictEntry]);
}