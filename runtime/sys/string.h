#ifndef __joyeer_runtime_sys_string_h__
#define __joyeer_runtime_sys_string_h__

#include "runtime/function.h"
#include "runtime/runtime.h"
#include <sstream>
struct JrObjectString: JrObject {
    static JrObjectType* Type;
    
    std::wstring* content = nullptr;
    
    JrObjectString();
    ~JrObjectString();
    
    static void init();
};

struct JrObjectStringBuilder: JrObject {
    static JrObjectType* Type;
    static JrFunction::Pointer Constructor;
    
    JrObjectStringBuilder();
    ~JrObjectStringBuilder();
    
    std::wstringstream* stringstream;
    
    static void init();
};

struct JrObjectStringBuilder_Append: public JrNativeFunc {
    static JrFunction::Pointer Func;
    virtual void operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func);
};

struct JrObjectStringBuilder_toString: public JrNativeFunc {
    static JrFunction::Pointer Func;
    virtual void operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func);
};
#endif