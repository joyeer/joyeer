#ifndef __joyeer_runtime_sys_string_h__
#define __joyeer_runtime_sys_string_h__

#include "runtime/function.h"
#include "runtime/runtime.h"

struct JrObjectString: JrObject {
    static JrObjectType* Type;
    
    
    std::wstring content;
    
    JrObjectString();
    ~JrObjectString();
    
    static void init();
};

struct JrObjectStringBuilder: JrObject {
    static JrObjectType* Type;

};

#endif
