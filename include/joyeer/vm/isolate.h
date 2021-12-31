//
// Created by Qing Xu on 2021/11/8.
//

#ifndef __joyeer_vm_isolate_h__
#define __joyeer_vm_isolate_h__

#include "joyeer/vm/loader.h"
#include "joyeer/vm/res+table.h"
#include "joyeer/vm/gc.h"
#include "joyeer/vm/stdlib.h"

struct ArrayClass;

struct IsolateVM {

    StringTable* strings {new StringTable() };
    ClassResTable* classes {new ClassResTable() };
    MethodResTable* methods {new MethodResTable() };

    GC* gc;

    explicit IsolateVM() ;
    ~IsolateVM();

    void run(const ModuleType::Ptr& module, CompilerService* compilerService);
    void run(const ModuleClass* moduleClass);

    // import ModuleClass, prepare the memory for module class
    void import(ModuleClass* moduleClass);

private:
    friend class Interpreter;
    friend class Array_$$_get;
    friend class Array_$$_size;

    // initialize the std library
    void initStdlib(CompilerService* compiler);

    // std library classes
    Global_$_print* printFunc = nullptr;
    ArrayClass* arrayClass = nullptr;
    Array_$$_get* arrayGetFunc = nullptr;
};


#endif //__joyeer_vm_isolate_h__
