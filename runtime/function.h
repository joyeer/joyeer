#ifndef __joyeer_runtime_function_h__
#define __joyeer_runtime_function_h__

#include <string>
#include <vector>

struct JrRuntimeContext;
struct JrFunction;

struct JrType {
    uint32_t kind;
    uint32_t index;
};

struct JrParameter {
    JrType type;
};


struct JrCode {
    typedef std::shared_ptr<JrCode> Pointer;
    
    uint32_t size;
    uint8_t* code;
    
    JrCode(uint32_t size);
    virtual ~JrCode();
};


struct JrNativeCode {
    typedef std::shared_ptr<JrNativeCode> Pointer;
    
    virtual void operator() (JrRuntimeContext *context, JrFunction* func);
};

#define JrFunction_Native       1
#define JrFunction_VM           2

struct JrFunction {
    typedef std::shared_ptr<JrFunction> Pointer;
public:
    const std::wstring name;
    
    uint8_t kind;
    // The maximun count of parameter count is 0xFF
    uint8_t paramCount;
    /**
     e.g. func add(value1:int, value2:float, descriptor:string)
     the descriptor will be [JrType_Int] [JrType_Float] [JrType_String] [RefIndexOfStringInDataSection]
     */
    std::vector<JrType> paramTypes;
    
    // Function's return type
    JrType  returnType;
    
    union {
        JrCode* code;
        JrNativeCode* nativeCode;
    };
    
public:
    ~JrFunction();
};

struct JrFunctionFrame {
    
};



#endif
