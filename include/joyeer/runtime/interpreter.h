#ifndef __joyeer_runtime_interpreter_h__
#define __joyeer_runtime_interpreter_h__

#include "instruction.h"
#include "runtime.h"
#include "function.h"

struct JrInterpreter {
public:
    JrInterpreter(JrRuntimeContext* context);
    
    void run(JrModuleClass* moduleType);
    void run(JrFunction* function, int objectRef);
    
private:
    std::vector<Instruction>::const_iterator pointer;
    std::vector<Instruction>::const_iterator end;
    
    // prepare the stack frame
    JrFunctionFrame::Ptr prepareStackFrame(JrFunction* func);
    
    void exec_invoke(const Instruction& instruction);
    void exec_istore(const Instruction& instruction);
    
    void exec_iload(const Instruction& instruction);
    void exec_oload(const Instruction& instruction);
    
    void exec_iadd(const Instruction& instruction);
    void exec_imul(const Instruction& instruction);
    void exec_isub(const Instruction& instruction);
    void exec_idiv(const Instruction& instruction);
    void exec_irem(const Instruction& instruction);
    void exec_icmp_g(const Instruction& instruction);
    void exec_icmp_ge(const Instruction& instruction);
    void exec_icmp_l(const Instruction& instruction);
    void exec_icmp_le(const Instruction& instruction);
    void exec_icmp_ne(const Instruction& instruction);
    void exec_icmp_eq(const Instruction& instruction);
    void exec_ifle(const Instruction& instruction);
    void exec_ineg(const Instruction& instruction);
    void exec_iand(const Instruction& instruction);
    
    void exec_goto(const Instruction& instruction);
    void exec_ireturn(const Instruction& instruction);
    void exec_return(const Instruction& instruction);
    void exec_oreturn(const Instruction& instruction);
        
    void exec_oconst_nil(const Instruction& instruction);
    void exec_new(const Instruction& instruction);
    void exec_putfield(const Instruction& instruction);
    void exec_getfield(const Instruction& instruction);
    void exec_onewarray(const Instruction& instruction);
    
    void exec_sconst(const Instruction& instruction);
    void exec_dup(const Instruction& instruction);
    
    JrRuntimeContext* context;
};

#endif