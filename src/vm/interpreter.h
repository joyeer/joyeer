//
// Created by Qing Xu on 2021/11/5.
//

#ifndef __joyeer_vm_interpreter_h__
#define __joyeer_vm_interpreter_h__

#include "isolate.h"
#include "frame.h"
#include "runtime/bytecode.h"
#include "runtime/executor.h"



struct InterpretedExecutor : public Executor {

    explicit InterpretedExecutor(IsolateVM* vm);

    // execute a Module
    void execute(const ModuleClass* module);

    // execute a Method
    void execute(const Function* method);

private:
    friend class Interpreter;
    friend class Arguments;

    FramePtr getCurrentFrame() const;

    void invokeVFunction(const Function* method);
    void invokeCFunction(const Function* method);

    void push(Slot frame, int size);
    void pop(Slot frame);

    // Push Value into Stack
    void push(Value value);
    Value pop();
    Value top() const;

};


#endif //__joyeer_vm_interpreter_h__
