#ifndef __joyeer_runtime_sys_module_h__
#define __joyeer_runtime_sys_module_h__

#include "joyeer/runtime/function.h"
#include "joyeer/runtime/runtime.h"

struct JrModule: JrObject {
    static void init();
};

struct JrModule_Import: public JrNativeFunc {
    
};

#endif
