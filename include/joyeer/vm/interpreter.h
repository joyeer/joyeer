//
// Created by Qing Xu on 2021/11/5.
//

#ifndef __joyeer_vm_interpreter_h__
#define __joyeer_vm_interpreter_h__

#include "joyeer/vm/isolate.h"
#include "joyeer/vm/frame.h"
#include "joyeer/vm/bytecode.h"

struct BytecodeInterpreter {
    virtual void operator()(IsolateVM *isolateVM) = 0;
};

struct Stack {
public:
    struct StackSlot {
    };
};

struct Interpreter {
    uint64_t cp;
    Stack stack;
    Bytecodes *bytecodes;
    IsolateVM *isolateVM;

    explicit Interpreter(IsolateVM *isolateVm) :
            isolateVM(isolateVm),
            cp(0),
            bytecodes(nullptr) {
    }

    // entry a Module
    void entry(ModuleClass* moduleClass);
    void entry(Method* method);

    [[maybe_unused]] void run();
};

#endif //__joyeer_vm_interpreter_h__
