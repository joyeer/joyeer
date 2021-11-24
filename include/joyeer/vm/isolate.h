//
// Created by Qing Xu on 2021/11/8.
//

#ifndef __joyeer_vm_isolate_h__
#define __joyeer_vm_isolate_h__

#include "joyeer/vm/compiler.h"
#include "joyeer/vm/res+table.h"
#include "joyeer/vm/gc.h"
#include "joyeer/vm/types.h"

struct IsolateVM {

    explicit IsolateVM() ;
    ~IsolateVM();

    void run(ModuleType::Ptr fileModule, CompilerService* compilerService);

private:
    friend class VCompiler;

    VCompiler *vCompiler;
    GC* gc;

    StringTable* stringTable { new StringTable() };
    ClassResTable* classTable { new ClassResTable() };
    MethodResTable* funcTable { new MethodResTable() };
};

#endif //__joyeer_vm_isolate_h__
