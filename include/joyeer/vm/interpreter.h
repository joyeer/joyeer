//
// Created by Qing Xu on 2021/11/5.
//

#ifndef __joyeer_vm_interpreter_h__
#define __joyeer_vm_interpreter_h__

#include "joyeer/vm/isolate.h"
#include "joyeer/vm/frame.h"
#include <stack>

struct Bytecodes {
    JrByte *bytecodes;
    JrInt size;
};

#define FETCH_BYTECODE() \


struct BytecodeInterpreter {
    virtual void operator()(IsolateVM *isolateVM) {
    };
};

struct Interpreter {
    JrInt cp;
    Stack stack;
    Bytecodes *bytecodes;
    IsolateVM *isolateVM;

    std::stack<StackFrame*> stackFrameTraces;

    explicit Interpreter(IsolateVM *isolateVm) :
            isolateVM(isolateVm),
            cp(0),
            bytecodes(nullptr) {
    }

    void run() {
next_point:

return_point:
        return;
    }
};

#endif //__joyeer_vm_interpreter_h__
