#ifndef __joyeer_runtime_runtime_h__
#define __joyeer_runtime_runtime_h__

#include <stdint.h>
#include <vector>
#include <string>
#include "instruction.h"
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
    int index;
};

struct JrFunctionFrame {
    
    typedef std::shared_ptr<JrFunctionFrame> Pointer;
    
    // the index of the function in function tables
    int addressOfFunc;
    
    // the address of this frame in stack
    uint8_t* startAddress;
    uint8_t* endAddress;
    
    // the addresses of variable in stack
    std::vector<uint8_t*> addressOfVariables;
};


// Runtime stack for VM
struct JrRuntimeStack {
    const static JrInt Size = 1024 * 1024 * 4;
    
    JrRuntimeStack();
    
    uint8_t* pointer;
    uint8_t data[JrRuntimeStack::Size];
    
    // push the FunctionFrame into stack
    void push(JrFunctionFrame::Pointer frame);
    void push4(uint32_t value);
    
    // store the variable value at address at stack
    void storeValueForVariable(uint8_t* addressOfVariable, int value);
    // get the int value of the variable
    int intValueOfVariable(uint8_t* addressOfVariable);
    
    uint32_t pop4();
};

// The runtime context for VM
struct JrRuntimeContext {
    JrRuntimeContext();
    ~JrRuntimeContext();
    
    JrRuntimeStack *stack;
    
    JrFunctionFrame::Pointer frame;
    
};


#endif
