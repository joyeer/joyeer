#include "interpreter.h"
#include "buildin.h"

JrInterpreter::JrInterpreter(JrRuntimeContext* context):
context(context) {
}

void JrInterpreter::run(std::vector<Instruction> instructions) {
    for(auto instruction: instructions) {
        switch(instruction.opcode) {
            case OP_ICONST:
                context->stack->push4(instruction.value);
                break;
            case OP_SCONST:
                context->stack->push4(instruction.value);
                break;
            case OP_INVOKE:
                exec_invoke(instruction);
                break;
            case OP_ISTORE:
                break;
            case OP_ILOAD:
                break;
            default:
                break;
        }
    }
}

void JrInterpreter::exec_invoke(const Instruction &instruction) {
    auto func = Global::funcTable[instruction.value];
    if(func->kind == JrFunction_Native) {
        (*func->nativeCode)(context, func.get());
    }
}
