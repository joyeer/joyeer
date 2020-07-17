#include "interpreter.h"
#include "buildin.h"
#include <iostream>

JrInterpreter::JrInterpreter(JrRuntimeContext::Pointer context):
context(context) {
}


void JrInterpreter::run(JrFunction::Pointer function) {
    
    auto frame = prepareStackFrame(function);
    context->stack->push(frame);
    
    std::wcout << L">[call][function]:" << function->name << std::endl;
    
    pointer = function->instructions.begin();
    end = function->instructions.end();
    
    while(pointer != end) {
        auto instruction = *pointer;
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
            case OP_IMUL:
                exec_imul(instruction);
                break;
            case OP_IADD:
                exec_iadd(instruction);
                break;
            case OP_ISUB:
                exec_isub(instruction);
                break;
            case OP_IDIV:
                exec_idiv(instruction);
                break;
            case OP_IREM:
                exec_irem(instruction);
                break;
            case OP_IFLE:
                exec_ifle(instruction);
                break;
            case OP_GOTO:
                exec_goto(instruction);
                break;
            case OP_IRETURN:
                exec_ireturn(instruction);
                return;
            default:
                break;
        } 
        pointer ++;
        
    }
    
}

JrFunctionFrame::Pointer JrInterpreter::prepareStackFrame(JrFunction::Pointer func) {
    auto frame = std::make_shared<JrFunctionFrame>();
    frame->addressOfFunc = func->addressOfFunc;
    
    uint8_t* baseAddress = context->stack->pointer;
    uint8_t* address = baseAddress - (4 * func->paramTypes.size());
    frame->startAddress = address ;
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
    auto addressOfVariable = context->stack->topFrame()->addressOfVariables[variableIndex];
    context->stack->storeValueForVariable(addressOfVariable, value);
}

void JrInterpreter::exec_iload(const Instruction &instruction) {
    auto variableIndex = instruction.value;
    auto addressOfVarible = context->stack->topFrame()->addressOfVariables[variableIndex];
    auto value = context->stack->intValueOfVariable(addressOfVarible);
    context->stack->push4(value);
}

void JrInterpreter::exec_invoke(const Instruction &instruction) {
    auto func = Global::functions[instruction.value];
    if(func->kind == JrFunction_Native) {
        (*func->nativeCode)(context, func);
    } else if(func->kind == JrFunction_VM) {
        JrInterpreter interpreter(context);
        interpreter.run(func);
    }
}

void JrInterpreter::exec_iadd(const Instruction &instruction) {
    auto value1 = context->stack->pop4();
    auto value2 = context->stack->pop4();
    context->stack->push4(value1 + value2);
}

void JrInterpreter::exec_imul(const Instruction &instruction) {
    auto value1 = context->stack->pop4();
    auto value2 = context->stack->pop4();
    context->stack->push4(value1 * value2);
}

void JrInterpreter::exec_isub(const Instruction &instruction) {
    auto value1 = context->stack->pop4();
    auto value2 = context->stack->pop4();
    context->stack->push4(value2 - value1 );
}

void JrInterpreter::exec_idiv(const Instruction &instrunction) {
    auto value1 = context->stack->pop4();
    auto value2 = context->stack->pop4();
    context->stack->push4(value2 / value1);
}

void JrInterpreter::exec_irem(const Instruction &instrunction) {
    auto value1 = context->stack->pop4();
    auto value2 = context->stack->pop4();
    
    context->stack->push4(value2 % value1);
}

void JrInterpreter::exec_ifle(const Instruction &instrunction) {
    auto value1 = context->stack->pop4();
    if(value1 <= 0) {
        pointer += instrunction.value;
    }
}

void JrInterpreter::exec_goto(const Instruction &instruction) {
    auto value1 = context->stack->pop4();
    pointer += instruction.value;
}

void JrInterpreter::exec_ireturn(const Instruction &instruction) {
    auto value = context->stack->pop4();
    context->stack->pop(context->stack->topFrame());
    context->stack->push4(value);
}
