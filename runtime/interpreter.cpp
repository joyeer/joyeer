#include "interpreter.h"
#include "buildin.h"

JrInterpreter::JrInterpreter(JrRuntimeContext* context):
context(context) {
}


void JrInterpreter::run(JrFunction::Pointer function) {
    
    auto frame = prepareStackFrame(function);
    context->stack->push(frame);
    context->frame = frame;
    
    for(auto instruction: function->instructions) {
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
                exec_istore(instruction);
                break;
            case OP_ILOAD:
                exec_iload(instruction);
                break;
            default:
                break;
        }
    }
}

JrFunctionFrame::Pointer JrInterpreter::prepareStackFrame(JrFunction::Pointer func) {
    auto frame = std::make_shared<JrFunctionFrame>();
    frame->addressOfFunc = func->addressOfFunc;
    
    uint8_t* baseAddress = context->stack->pointer;
    uint8_t* address = baseAddress;
    frame->startAddress = address;
    for(auto var : func->localVars) {
        frame->addressOfVariables.push_back(address);
        // TODO: Update with sizeof variable
        address += 4;
    }
    frame->endAddress = address;
    
    return frame;
}

void JrInterpreter::exec_istore(const Instruction &instruction) {
    auto value = context->stack->pop4();
    auto variableIndex = instruction.value;
    auto addressOfVariable = context->frame->addressOfVariables[variableIndex];
    context->stack->storeValueForVariable(addressOfVariable, value);
}

void JrInterpreter::exec_iload(const Instruction &instruction) {
    auto variableIndex = instruction.value;
    auto addressOfVarible = context->frame->addressOfVariables[variableIndex];
    auto value = context->stack->intValueOfVariable(addressOfVarible);
    context->stack->push4(value);
}

void JrInterpreter::exec_invoke(const Instruction &instruction) {
    auto func = Global::funcTable[instruction.value];
    if(func->kind == JrFunction_Native) {
        (*func->nativeCode)(context, func.get());
    }
}

