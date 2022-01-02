//
// Created by Qing Xu on 2021/11/8.
//

#ifndef __joyeer_vm_isolate_h__
#define __joyeer_vm_isolate_h__

#include "joyeer/vm/gc.h"
#include "joyeer/vm/stdlib.h"
#include "joyeer/runtime/res+table.h"
#include "joyeer/compiler/compiler+service.h"

struct ArrayClass;

struct IsolateVM {

    StringResTable* strings = nullptr;
    TypeResTable* types = nullptr;

    GC* gc;

    explicit IsolateVM(CompilerService* compiler) ;
    ~IsolateVM();

    void run(const ModuleType* module);

private:
    friend class Interpreter;
    friend class Array_$$_get;
    friend class Array_$$_size;

    CompilerService* compiler;

    // initialize the std library
    void initStdlib();

    // std library classes
    Global_$_print* printFunc = nullptr;
    ArrayClass* arrayClass = nullptr;
    Array_$$_get* arrayGetFunc = nullptr;
};


#endif //__joyeer_vm_isolate_h__
