#ifndef __joyeer_runtime_sys_map_h__
#define __joyeer_runtime_sys_map_h__

#include "joyeer/runtime/function.h"
#include "joyeer/runtime/runtime.h"
#include <unordered_map>


struct JrValueHoldHasher {
    std::size_t operator()(const JrValueHold& hold) const;
};

struct JrObjectMap : public JrObject {
    static JrObjectType* Type;
    static JrFunction* Constructor;
    
    JrObjectMap();
    ~JrObjectMap();
    
    std::unordered_map<JrValueHold, JrValueHold, JrValueHoldHasher> maps;
    
    static void init();
};


struct JrObjectMap_Insert : public JrNativeFunc {
    static JrFunction* Func;
    virtual void operator()(JrRuntimeContext* context, JrFunction* func);
};


struct JrObjectMap_Get: public JrNativeFunc {
    static JrFunction* Func;
    virtual void operator()(JrRuntimeContext* context, JrFunction* func);
};

#endif
