#ifndef __joyeer_runtime_runtime_h__
#define __joyeer_runtime_runtime_h__

#include <stdint.h>
#include <vector>
#include <stack>
#include <string>
#include "instruction.h"

typedef int64_t     JrInt;
typedef int64_t     JrInt64;
typedef int32_t     JrInt32;
typedef int16_t     JrInt16;

typedef uint64_t    JrUInt;
typedef uint64_t    JrUInt64;
typedef uint32_t    JrUInt32;
typedef uint16_t    JrUInt16;
typedef uint8_t     JrUInt8;

#define JrType_Void         0
#define JrType_Int          1
#define JrType_Float        2
#define JrType_Double       3
#define JrType_Long         4
#define JrType_Boolean      5
#define JrType_String       7

#define JrType_Any          8
#define JrType_Nil          9
#define JrType_Object       10



struct JrType {
    typedef std::shared_ptr<JrType> Pointer;
    const static JrType::Pointer Any;
    const static JrType::Pointer Void;
    const static JrType::Pointer Nil;
    
    int kind;
    std::wstring name;
    
    // index to the type tables
    int addressOfType;
    
    int size = sizeof(void*);
};

struct JrField {
    typedef std::shared_ptr<JrField> Pointer;
    
    std::wstring name;
    JrType::Pointer type;
    int addressOfField = -1;
};

struct JrPrimaryType: public JrType {
    typedef std::shared_ptr<JrPrimaryType> Pointer;
    
    const static JrPrimaryType::Pointer Int;
    const static JrPrimaryType::Pointer Int8;
    const static JrPrimaryType::Pointer Int16;
    const static JrPrimaryType::Pointer Int32;
    const static JrPrimaryType::Pointer Int64;
    
    const static JrPrimaryType::Pointer Float;
    const static JrPrimaryType::Pointer Boolean;
};
 
struct JrObjectType: public JrType {
    typedef std::shared_ptr<JrObjectType> Pointer;
    
    std::vector<int> virtualFunctions;
    std::vector<int> staticFunctions;
    
    std::vector<JrField::Pointer> virtualFields;
    std::vector<JrField::Pointer> staticFields;
    
    void registerField(JrField::Pointer field);
    
};

struct JrVar {
    typedef std::shared_ptr<JrVar> Pointer;
    JrType::Pointer type;
    std::wstring name;
    
    int addressOfVariable;
    int addressOfObject;
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
    typedef std::shared_ptr<JrRuntimeStack> Pointer;
    const static JrInt Size = 1024 * 1024 * 4;
    
    JrRuntimeStack();
    
    std::stack<JrFunctionFrame::Pointer> frames;
    uint8_t* pointer;
    uint8_t data[JrRuntimeStack::Size];
    
    // push the FunctionFrame into stack
    void push(JrFunctionFrame::Pointer frame);
    void pop(JrFunctionFrame::Pointer frame);
    
    JrFunctionFrame::Pointer topFrame();
    
    void push(JrInt value);
    JrInt pop();
    
    void restore(uint8_t* address);
    
    // store the variable value at address at stack
    void storeValueForVariable(uint8_t* addressOfVariable, JrInt value);
    // get the int value of the variable
    JrInt intValueOfVariable(uint8_t* addressOfVariable);
    
    
    
};

struct JrGC;
// The runtime context for VM
struct JrRuntimeContext {
    typedef std::shared_ptr<JrRuntimeContext> Pointer;
    
    JrRuntimeContext();
    ~JrRuntimeContext();
    
    JrRuntimeStack::Pointer stack;
    JrGC* gc;
};

#endif
