#ifndef __joyeer_runtime_sys_print_h__
#define __joyeer_runtime_sys_print_h__

#include "runtime/function.h"

struct JrFuncPrint : public JrNativeFunc {
    static JrFunction::Pointer Func;
    
    virtual void operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func);
    
    static void init();
};


#endif


