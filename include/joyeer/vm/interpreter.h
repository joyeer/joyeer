//
// Created by Qing Xu on 2021/11/5.
//

#ifndef __joyeer_vm_interpreter_h__
#define __joyeer_vm_interpreter_h__

#include "joyeer/vm/isolate.h"
#include "joyeer/vm/frame.h"
#include "joyeer/vm/bytecode.h"

constexpr int kStackMaxSize = 1024 * 1024 * 4; // 4M intptr_t;

struct Executor;

struct Arguments {
    explicit Arguments(Executor* executor);
    Value getArgument(Slot slot);

private:
    Executor* executor;
};

struct Executor {

    intptr_t fp = 0;
    intptr_t sp = 0;
    IsolateVM* isolateVM;

    explicit Executor(IsolateVM* isolateVM);

    // execute a Module
    void execute(const ModuleClass* module);

    // execute a Method
    void execute(const Method* method);

private:
    friend class Interpreter;
    friend class Arguments;
    friend class Array_$$_get;

    FramePtr getCurrentFrame() const;
    void invoke(const CMethod* method);
    void invoke(const VMethod* method);

    void push(Slot frame, int size);
    void pop(Slot frame);

    // Push Value into Stack
    void push(Value value);
    Value pop();

    std::vector<Slot> frames {};
    char stack[kStackMaxSize] {};
};


#endif //__joyeer_vm_interpreter_h__
