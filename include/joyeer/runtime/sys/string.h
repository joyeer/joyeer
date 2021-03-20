#ifndef __joyeer_runtime_sys_string_h__
#define __joyeer_runtime_sys_string_h__

#include "joyeer/runtime/function.h"
#include "joyeer/runtime/runtime.h"
#include <sstream>

struct JrObjectString: public JrObject {
    static JrObjectType* Type;
    
    std::wstring* content = nullptr;
    
    JrObjectString();
    ~JrObjectString();
    
    static void init();
};

struct JrObjectStringBuilder: public JrObject {
    static JrObjectType* Type;
    static JrFunction* Constructor;
    
    JrObjectStringBuilder();
    ~JrObjectStringBuilder();
    
    std::wstringstream* stringstream;
    
    static void init();
};

struct JrObjectStringBuilder_Append: public JrNativeFunc {
    static JrFunction* Func;
    virtual void operator()(JrRuntimeContext* context, JrFunction* func);
};

struct JrObjectStringBuilder_toString: public JrNativeFunc {
    static JrFunction* Func;
    virtual void operator()(JrRuntimeContext* context, JrFunction* func);
};
#endif
