#ifndef __joyeer_runtime_function_h__
#define __joyeer_runtime_function_h__

#include <string>
#include "runtime.h"

struct JrRuntimeContext;
struct JrNativeFunc;

struct JrCode {
    typedef std::shared_ptr<JrCode> Pointer;
    
    uint32_t size;
    uint8_t* code;
    
    JrCode(uint32_t size);
    virtual ~JrCode();
};

enum JrFunctionKind  {
    jrFuncNative                    = 1,
    jrFuncVM                        = 1 << 1,
    jrFuncConstructor               = jrFuncVM | ( 1 << 2)
};

struct JrFunction {
    typedef std::shared_ptr<JrFunction> Pointer;
    
public:
    ~JrFunction();
    int totalSizeOfParams();
    
    JrFunctionKind kind;
    std::wstring name;
    
    // all types for param, for object class, the last param is self
    std::vector<JrType*> paramTypes;
    
    // Function's return type
    JrType*  returnType;
    
    // Local Variables
    std::vector<JrVar> localVars;
    
    std::vector<Instruction> instructions;
    
    union {
        JrCode* code;
        JrNativeFunc* nativeCode;
    };
    
    // address index of this function in function table
    int addressOfFunc;
    // address index of owner type in global type table
    int addressOfOwnerType;
};

struct JrNativeFunc {
    typedef std::shared_ptr<JrNativeFunc> Pointer;
    
    virtual void operator() (JrRuntimeContext::Pointer context, JrFunction::Pointer func);
};


#endif
