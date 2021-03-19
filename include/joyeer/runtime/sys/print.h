#ifndef __joyeer_runtime_sys_print_h__
#define __joyeer_runtime_sys_print_h__

#include "runtime/function.h"

struct JrFuncPrint : public JrNativeFunc {
    static JrFunction* Func;
    
    virtual void operator()(JrRuntimeContext* context, JrFunction* func);
    
    static void init();
};

struct JrFuncObjectPrint: public JrNativeFunc {
    static JrFunction* Func;
    
    virtual void operator()(JrRuntimeContext* context, JrFunction* func);
    
    static void init();
};

#endif


