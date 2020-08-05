#ifndef __joyeer_runtime_sys_module_h__
#define __joyeer_runtime_sys_module_h__

#include "runtime/function.h"
#include "runtime/runtime.h"

struct JrModule: JrObject {
    const static JrObjectType* Type;
    
    static void init();
};

#endif
