#ifndef __joyeer_runtime_interpreter_h__
#define __joyeer_runtime_interpreter_h__

#include "instruction.h"
#include "runtime.h"

struct JrInterpreter {
public:
    JrInterpreter(JrRuntimeContext* context);
    void run(std::vector<Instruction> instructions);
    
private:
    void exec_invoke(const Instruction& instruction);
    
    JrRuntimeContext* context;
};

#endif
