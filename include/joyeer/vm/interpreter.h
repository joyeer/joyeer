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

constexpr int kStackMaxSize = 1024 * 1024 * 4; // 4M intptr_t;

struct Executor {

    intptr_t fp = 0;
    intptr_t sp = 0;
    IsolateVM* isolateVM;

    explicit Executor(IsolateVM* isolateVM);

    // execute a Module
    void execute(const ModuleClass* moduleClass);

    // execute a Method
    void execute(const Method* method);
private:

    friend class Interpreter;

    void execute(const CMethod* method);
    void execute(const VMethod* method);

    void push(Slot frame, int size);
    void pop(Slot frame);

    // Push Value into Stack
    void push(Value value);

    std::vector<Slot> frames {};
    char stack[kStackMaxSize] {};
};


#endif //__joyeer_vm_interpreter_h__
