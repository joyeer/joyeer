#ifndef __joyeer_runtime_function_h__
#define __joyeer_runtime_function_h__

#include <string>
#include "runtime.h"

struct JrRuntimeContext;
struct JrNativeFunc;

enum JrFunctionKind  {
    jrFuncNative                    = 1,
    jrFuncVM                        = 1 << 1,
    jrFuncConstructor               = jrFuncVM | ( 1 << 2)
};


struct JrFunction {
    
public:
    ~JrFunction();
    int totalSizeOfParams();
    
    JrFunctionKind kind;
    std::string name;
    
    // all types for param, for object class, the last param is self
    std::vector<JrType*> paramTypes;
    
    // Function's return type
    JrType*  returnType;
    
    // Local Variables
    std::vector<JrVar> localVars;
    
    std::vector<Instruction> instructions;
    
    union {
        JrNativeFunc* nativeCode;
    };
    
    // address index of this function in function table
    int addressOfFunc;
    // address index of owner type in global type table
    int addressOfOwnerType;
};

struct JrNativeFunc {
    virtual void operator() (JrRuntimeContext* context, JrFunction* func);
};


#endif
