#ifndef __joyeer_runtime_array_h__
#define __joyeer_runtime_array_h__

#include "function.h"
#include "runtime.h"

struct JrArray: JrObject {
    const static JrObjectType::Pointer Type;
    
    std::vector<JrInt> slots;
    
    static void init();
};

struct JrArray_Size : public JrNativeCode {
    static JrFunction::Pointer Func;
    virtual void operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func);
};

struct JrArray_Append: public JrNativeCode {
    static JrFunction::Pointer Func;
    virtual void operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func);
};

#endif

