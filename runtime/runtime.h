#ifndef __joyeer_runtime_runtime_h__
#define __joyeer_runtime_runtime_h__

#include <stdint.h>
#include <vector>
#include <string>

#define JrType_Void         0

#define JrType_Int          1
#define JrType_Float        2
#define JrType_Double       3
#define JrType_Long         4
#define JrType_String       8

#define JrType_Object       10
 
typedef int64_t     JrInt;
typedef int64_t     JrInt64;
typedef int32_t     JrInt32;
typedef int16_t     JrInt16;

typedef uint64_t    JrUInt;
typedef uint64_t    JrUInt64;
typedef uint32_t    JrUInt32;
typedef uint16_t    JrUInt16;
typedef uint8_t     JrUInt8;

struct JrType {
    uint32_t kind;
    uint32_t index;
};

struct JrVar {
    JrType type;
    std::wstring name;
    union {
        int32_t int32Value;
    };
};

struct JrFunctionFrame {
    std::vector<JrVar> vars;
};

// Runtime stack for VM
struct JrRuntimeStack {
    const static JrInt Size = 1024 * 1024 * 4;
    
    JrRuntimeStack();
    
    uint8_t* pointer;
    uint8_t data[JrRuntimeStack::Size];
    
    // push the FunctionFrame into stack
    void push(const JrFunctionFrame& frame);
    
    void push4(uint32_t value);
    uint32_t pop4();
};

// The runtime context for VM
struct JrRuntimeContext {
    JrRuntimeContext();
    ~JrRuntimeContext();
    
    JrRuntimeStack *stack;
};


#endif
