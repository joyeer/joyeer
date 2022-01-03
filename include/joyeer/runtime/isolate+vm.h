//
// Created by Qing Xu on 2022/1/2.
//

#ifndef __joyeer_isolate_vm_h__
#define __joyeer_isolate_vm_h__

#include "joyeer/runtime/res+table.h"
#include "joyeer/runtime/gc.h"
#include "joyeer/runtime/sys.h"

struct IsolateVM {

    IsolateVM();
    virtual ~IsolateVM();

    StringResTable* strings = nullptr;
    TypeResTable* types = nullptr;
    GC* gc;

    // std library classes
    Global_$_print* printFunc = nullptr;
    ArrayClass* arrayClass = nullptr;
    Array_$$_get* arrayGetFunc = nullptr;

    // initialize the std library
    void initStdlib();


};


#endif //__joyeer_isolate_vm_h__
