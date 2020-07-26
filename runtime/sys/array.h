#ifndef __joyeer_runtime_sys_array_h__
#define __joyeer_runtime_sys_array_h__

#include "runtime/function.h"
#include "runtime/runtime.h"

struct JrObjectIntArray: JrObject {
    const static JrObjectType::Pointer Type;
    
    std::vector<JrInt> slots;
    
    static void init();
};

struct JrObjectIntArray_Size : public JrNativeFunc {
    static JrFunction::Pointer Func;
    virtual void operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func);
};

struct JrObjectIntArray_Append: public JrNativeFunc {
    static JrFunction::Pointer Func;
    virtual void operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func);
};


struct JrObjectObjectArray: JrObject {
    
};

#endif

