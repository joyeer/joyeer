//
// Created by Qing Xu on 2021/11/8.
//

#ifndef __joyeer_vm_isolate_h__
#define __joyeer_vm_isolate_h__

#include "joyeer/vm/loader.h"
#include "joyeer/vm/res+table.h"
#include "joyeer/vm/gc.h"
#include "joyeer/vm/types.h"

struct ArrayClass;

struct IsolateVM {

    StringTable* stringTable { new StringTable() };
    ClassResTable* classTable { new ClassResTable() };
    MethodResTable* methodTable { new MethodResTable() };
    GC* gc;

    explicit IsolateVM() ;
    ~IsolateVM();

    void run(const ModuleType::Ptr& module, CompilerService* compilerService);
    void run(const ModuleClass* moduleClass);

    // import ModuleClass, prepare the memory for module class
    void import(ModuleClass* moduleClass);

private:
    friend class Interpreter;

    // initialize the std library
    void initStdlib(CompilerService* compiler);

    ArrayClass* arrayClass;
};


#endif //__joyeer_vm_isolate_h__
