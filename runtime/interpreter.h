#ifndef __joyeer_runtime_interpreter_h__
#define __joyeer_runtime_interpreter_h__

#include "instruction.h"
#include "runtime.h"
#include "function.h"

struct JrInterpreter {
public:
    JrInterpreter(JrRuntimeContext* context);
    
    void run(JrFunction::Pointer function);
    
private:
    
    // prepare the stack frame
    JrFunctionFrame::Pointer prepareStackFrame(JrFunction::Pointer func);
    
    void exec_invoke(const Instruction& instruction);
    void exec_istore(const Instruction& instruction);
    void exec_iload(const Instruction& instruction);
    
    JrRuntimeContext* context;
};

#endif
