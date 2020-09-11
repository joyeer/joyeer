#ifndef __joyeer_runtime_sys_array_h__
#define __joyeer_runtime_sys_array_h__

#include "runtime/function.h"
#include "runtime/runtime.h"

struct JrObjectArray: JrObject {
    static JrObjectType* Type;
    static JrFunction* Constructor;
    
    std::vector<JrValueHold>* slots;
    
    JrObjectArray();
    ~JrObjectArray();
    
    static void init();
};

struct JrObjectArray_Size : public JrNativeFunc {
    static JrFunction* Func;
    virtual void operator()(JrRuntimeContext* context, JrFunction* func);
};

struct JrObjectArray_Append: public JrNativeFunc {
    static JrFunction* Func;
    virtual void operator()(JrRuntimeContext* context, JrFunction* func);
};

struct JrObjectArray_Get: public JrNativeFunc {
    static JrFunction* Func;
    virtual void operator()(JrRuntimeContext* context, JrFunction* func);
};

struct JrObjectArray_Set: public JrNativeFunc {
    static JrFunction* Func;
    virtual void operator()(JrRuntimeContext* context, JrFunction* func);
};

#endif

