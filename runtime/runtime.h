#ifndef __joyeer_runtime_runtime_h__
#define __joyeer_runtime_runtime_h__

#include <stack>
#include <string>
#include "object.h"
#include "instruction.h"

struct JrType {
    typedef std::shared_ptr<JrType> Pointer;
    const static JrType::Pointer Any;
    const static JrType::Pointer Void;
    const static JrType::Pointer Nil;
    
    int kind;
    std::wstring name;
    
    // index to the type tables
    int addressOfType;
    
    int size = sizeof(JrPtr);
};

struct JrFieldType {
    typedef std::shared_ptr<JrFieldType> Pointer;
    
    std::wstring name;
    JrType::Pointer type;
    JrInt addressOfField = -1;
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
    
    std::vector<JrInt> constructors;
    std::vector<JrInt> virtualFunctions;
    std::vector<JrInt> staticFunctions;
    
    std::vector<JrFieldType::Pointer> virtualFields;
    std::vector<JrFieldType::Pointer> staticFields;
    
    void registerField(JrFieldType::Pointer field);
    
};

// Speical type for module class
struct JrModuleType: public JrObjectType {
    
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
