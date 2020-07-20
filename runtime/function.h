#ifndef __joyeer_runtime_function_h__
#define __joyeer_runtime_function_h__

#include <string>
#include "runtime.h"

struct JrRuntimeContext;
struct JrNativeCode;

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
    std::wstring name;
    
    JrFunctionKind kind;
    // The maximun count of parameter count is 0xFF
    /**
     e.g. func add(value1:int, value2:float, descriptor:string)
     the descriptor will be [JrType_Int] [JrType_Float] [JrType_String] [RefIndexOfStringInDataSection]
     */
    std::vector<JrType::Pointer> paramTypes;
    
    // Function's return type
    JrType::Pointer  returnType;
    
    // Variables
    std::vector<JrVar> localVars;
    
    std::vector<Instruction> instructions;
    
    union {
        JrCode* code;
        JrNativeCode* nativeCode;
    };
    
    // address index of this function in function table
    int addressOfFunc;
    
public:
    ~JrFunction();
    
    int totalSizeOfParams();
};

struct JrNativeCode {
    typedef std::shared_ptr<JrNativeCode> Pointer;
    
    virtual void operator() (JrRuntimeContext::Pointer context, JrFunction::Pointer func);
};


#endif
