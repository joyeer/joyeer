//
// Created by Qing Xu on 2021/11/7.
//

#include "joyeer/vm/interpreter.h"

struct Interpreter {
    intptr_t cp = 0;
    Executor* executor;
    Bytecodes* bytecodes;

    explicit Interpreter(Executor* executor, Bytecodes* bytecodes) :
            executor(executor),
            bytecodes(bytecodes) {
    }


    [[maybe_unused]] void run() {
        auto bytecode = *(int *)(bytecodes->bytecodes + cp);
        printf("");
    }
};


Executor::Executor(IsolateVM *isolateVM):
isolateVM(isolateVM) {
}

void Executor::execute(const ModuleClass *moduleClass) {
    auto savedFP = sp;
    auto frame = &stack[sp];
    ModuleEntryFrame::set(frame, {.intValue = 0}, moduleClass->slotID);

    push(savedFP, ModuleEntryFrame::size());
    auto method = (*isolateVM->methodTable)[moduleClass->initializerSlot];
    execute(method);
    pop(savedFP);
}

void Executor::execute(const Method *method) {
    auto savedFP = sp;
    auto frame = &stack[sp];

    push(savedFP, FuncCallFrame::size());

    switch (method->kind) {
        case MethodKind::C_Method:
            execute(dynamic_cast<const CMethod*>(method));
            break;
        case MethodKind::VM_Method:
            execute(dynamic_cast<const VMethod*>(method));
            break;
        default:
            assert(false);
    }

    pop(savedFP);
}

void Executor::execute(const VMethod *method) {
    Interpreter interpreter(this, method->bytecodes);
    interpreter.run();
}

void  Executor::execute(const CMethod *method) {
    Arguments arguments;
    method->operator() (isolateVM, &arguments);
}

void Executor::push(Slot frame, int size) {
    frames.push_back(frame);
    sp += size;
}

void Executor::pop(Slot frame) {
    assert(frames.back() == frame);
    frames.pop_back();
    sp = frame;
}
