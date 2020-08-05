#ifndef __joyeer_runtime_interpreter_h__
#define __joyeer_runtime_interpreter_h__

#include "instruction.h"
#include "runtime.h"
#include "function.h"

struct JrInterpreter {
public:
    JrInterpreter(JrRuntimeContext::Pointer context);
    
    void run(JrModuleType::Pointer module);
    void run(JrFunction::Pointer function, int objectRef);
    
private:
    std::vector<Instruction>::const_iterator pointer;
    std::vector<Instruction>::const_iterator end;
    
    // prepare the stack frame
    JrFunctionFrame::Pointer prepareStackFrame(JrFunction::Pointer func);
    
    void exec_invoke(const Instruction& instruction);
    void exec_istore(const Instruction& instruction);
    
    void exec_iload(const Instruction& instruction);
    void exec_oload(const Instruction& instruction);
    
    void exec_iadd(const Instruction& instruction);
    void exec_imul(const Instruction& instruction);
    void exec_isub(const Instruction& instruction);
    void exec_idiv(const Instruction& instrunction);
    void exec_irem(const Instruction& instrunction);
    
    void exec_ifle(const Instruction& instrunction);
    
    void exec_goto(const Instruction& instruction);
    void exec_ireturn(const Instruction& instruction);
    void exec_return(const Instruction& instruction);
        
    void exec_oconst_nil(const Instruction& instruction);
    void exec_new(const Instruction& instruction);
    void exec_putfield(const Instruction& instruction);
    void exec_onewarray(const Instruction& instruction);
    
    JrRuntimeContext::Pointer context;
};

#endif
