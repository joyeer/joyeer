//
// Created by Qing Xu on 2021/11/4.
//

#ifndef __joyeer_vm_object_h__
#define __joyeer_vm_object_h__

#include <cstdint>
#include "joyeer/vm/types.h"


struct JrObject {
    JrClass* klass;
    JrByte data[];
};

// calculate a given JrClass size
static size_t calculateClassSize(JrClass* klass);


struct FileModuleLoader {
    void load(IsolateVM* isolateVM, JrFileModuleClass* fileModule);
};

#endif //__joyeer_vm_object_h__
