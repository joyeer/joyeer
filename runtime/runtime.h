#ifndef __joyeer_runtime_runtime_h__
#define __joyeer_runtime_runtime_h__

#include <stack>
#include <string>
#include "object.h"
#include "instruction.h"


typedef std::function<JrObject*(JrPtr)> JrObjectInitializer;
typedef std::function<void(JrObject*)> JrObjectFinalizer;

struct JrType {
    static JrType* Any;
    static JrType* Void;
    static JrType* Nil;
    
    JrTypeKind kind;
    std::wstring name;
    
    // index to the type tables
    int addressOfType;
    
    JrType(JrTypeKind kind, const std::wstring& name);
    
    virtual JrInt size();
    
    // initialize the object after malloc memory data
    JrObjectInitializer initializer = nullptr;
    // finialize the object before release memory
    JrObjectFinalizer finalizer = nullptr;
};

struct JrFieldType {
    typedef std::shared_ptr<JrFieldType> Pointer;
    
    std::wstring name;
    JrType* type;
    JrInt addressOfField = -1;
};

struct JrPrimaryType: public JrType {
    
    static JrPrimaryType* Int;
    static JrPrimaryType* Float;
    static JrPrimaryType* Boolean;
    
    JrPrimaryType(JrTypeKind kind, const std::wstring& name);
    
    virtual JrInt size();
};
 
struct JrObjectType: public JrType {
    
    std::vector<JrInt> constructors;
    std::vector<JrInt> virtualFunctions;
    std::vector<JrInt> staticFunctions;
    
    std::vector<JrFieldType::Pointer> virtualFields;
    std::vector<JrFieldType::Pointer> staticFields;
    
    void registerField(JrFieldType::Pointer field);
    
    virtual JrInt size();
    JrObjectType(const std::wstring& name,
                 JrObjectInitializer initializer = nullptr,
                 JrObjectFinalizer finalizer = nullptr);
};

// Speical type for module class
struct JrModuleType: public JrObjectType {
    JrModuleType(const std::wstring& name);
};

struct JrVar {
    typedef std::shared_ptr<JrVar> Pointer;
    JrType* type;
    std::wstring name;
    
    int addressOfVariable;
    int addressOfObject;
};

struct JrFunctionFrame {
    
    typedef std::shared_ptr<JrFunctionFrame> Pointer;
    
    // the index of the function in function tables
    JrInt addressOfFunc;
    
    // the address of this frame in stack
    JrInt startAddress;
    JrInt endAddress;
    
    // the addresses of variable in stack
    std::vector<JrInt> addressOfVariables;
};


// Runtime stack for VM
struct JrRuntimeStack {
    typedef std::shared_ptr<JrRuntimeStack> Pointer;
    const static JrInt Size = 1024 * 1024 * 4;
    
    JrRuntimeStack();
    
    std::stack<JrFunctionFrame::Pointer> frames;
    JrInt pointer;
    JrByte data[JrRuntimeStack::Size];
    
    // push the FunctionFrame into stack
    void startFuncCall(JrFunctionFrame::Pointer frame);
    void endFuncCall(JrFunctionFrame::Pointer frame);
    
    JrFunctionFrame::Pointer topFrame();
    
    JrValueHold top();
    
    void push(JrValueHold value);
    JrValueHold pop();
    
    // store the variable value at address at stack
    void storeValueForVariable(JrInt addressOfVariable, JrValueHold value);
    // get the int value of the variable
    JrValueHold intValueOfVariable(JrInt addressOfVariable);
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
