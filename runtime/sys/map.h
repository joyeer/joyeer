#ifndef __joyeer_runtime_sys_map_h__
#define __joyeer_runtime_sys_map_h__

#include "runtime/function.h"
#include "runtime/runtime.h"
#include <unordered_map>


struct JrValueHoldHasher {
    std::size_t operator()(const JrValueHold& hold) const;
};

struct JrObjectMap : public JrObject {
    static JrObjectType* Type;
    static JrFunction::Pointer Constructor;
    
    JrObjectMap();
    ~JrObjectMap();
    
    std::unordered_map<JrValueHold, JrValueHold, JrValueHoldHasher> maps;
    
    static void init();
};


struct JrObjectMap_Insert : public JrNativeFunc {
    static JrFunction::Pointer Func;
    virtual void operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func);
};


struct JrObjectMap_Get: public JrNativeFunc {
    static JrFunction::Pointer Func;
    virtual void operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func);
};

#endif
