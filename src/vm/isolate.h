//
// Created by Qing Xu on 2021/11/8.
//

#ifndef __joyeer_vm_isolate_h__
#define __joyeer_vm_isolate_h__

#include "runtime/gc.h"
#include "runtime/res+table.h"
#include "runtime/isolate+vm.h"
#include "compiler/compiler+service.h"

struct InterpretedIsolatedVM : public IsolateVM {
    void run(ModuleClass* module);
};

#endif //__joyeer_vm_isolate_h__
