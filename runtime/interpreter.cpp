#include "interpreter.h"
#include "buildin.h"
#include "gc.h"
#include <cassert>
#include <iostream>

JrInterpreter::JrInterpreter(JrRuntimeContext::Pointer context):
context(context) {
}


void JrInterpreter::run(JrFunction::Pointer function) {
    
    auto frame = prepareStackFrame(function);
    context->stack->push(frame);
    
    std::wcout << std::wstring(context->stack->frames.size() - 1, L'-') << L"$[function][entry] " << function->name << std::endl;
    
    pointer = function->instructions.begin();
    end = function->instructions.end();
    JrInstructionDebugPrinter printer;
    while(pointer != end) {
        auto instruction = *pointer;
        std::wcout << std::wstring(context->stack->frames.size() , L'-') << printer.print(instruction) << std::endl;
        switch(instruction.opcode) {
            case OP_ICONST:
                context->stack->push(instruction.value);
                break;
            case OP_OCONST_NIL:
                exec_oconst_nil(instruction);
                break;
            case OP_SCONST:
                context->stack->push(instruction.value);
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
                goto exit_label;; // finish the method's loop
            case OP_RETURN:
                exec_return(instruction);
                goto exit_label;; // finish the method's loop
            case OP_OLOAD:
                exec_oload(instruction);
                break;
            case OP_NEW:
                exec_new(instruction);
                break;
            case OP_PUTFIELD:
                exec_putfield(instruction);
                break;
            default:
                assert(false);
        } 
        pointer ++;
    }
    
exit_label:
    std::wcout << std::wstring(context->stack->frames.size(), L'-') << L"$[function][leave] " << function->name << std::endl;
}

JrFunctionFrame::Pointer JrInterpreter::prepareStackFrame(JrFunction::Pointer func) {
    auto frame = std::make_shared<JrFunctionFrame>();
    frame->addressOfFunc = func->addressOfFunc;
    
    uint8_t* baseAddress = context->stack->pointer;
    uint8_t* address = baseAddress - func->totalSizeOfParams();
    frame->startAddress = address ;
    for(auto var : func->localVars) {
        frame->addressOfVariables.push_back(address);
        // TODO: Update with sizeof variable
        address += var.type->size;
    }
    frame->endAddress = address;
    return frame;
}

void JrInterpreter::exec_istore(const Instruction &instruction) {
    auto value = context->stack->pop();
    auto variableIndex = instruction.value;
    auto addressOfVariable = context->stack->topFrame()->addressOfVariables[variableIndex];
    context->stack->storeValueForVariable(addressOfVariable, value);
}

void JrInterpreter::exec_iload(const Instruction &instruction) {
    auto variableIndex = instruction.value;
    auto addressOfVarible = context->stack->topFrame()->addressOfVariables[variableIndex];
    auto value = context->stack->intValueOfVariable(addressOfVarible);
    context->stack->push(value);
}

void JrInterpreter::exec_oload(const Instruction &instruction) {
    auto variableIndex = instruction.value;
    auto addressOfVariable = context->stack->topFrame()->addressOfVariables[variableIndex];
    auto value = context->stack->intValueOfVariable(addressOfVariable);
    context->stack->push(value);
}

void JrInterpreter::exec_invoke(const Instruction &instruction) {
    auto func = Global::functions[instruction.value];
    if((func->kind & jrFuncNative) == jrFuncNative) {
        (*func->nativeCode)(context, func);
    } else if((func->kind & jrFuncVM) == jrFuncVM) {
        JrInterpreter interpreter(context);
        interpreter.run(func);
    }
}

void JrInterpreter::exec_iadd(const Instruction &instruction) {
    auto value1 = context->stack->pop();
    auto value2 = context->stack->pop();
    context->stack->push(value1 + value2);
}

void JrInterpreter::exec_imul(const Instruction &instruction) {
    auto value1 = context->stack->pop();
    auto value2 = context->stack->pop();
    context->stack->push(value1 * value2);
}

void JrInterpreter::exec_isub(const Instruction &instruction) {
    auto value1 = context->stack->pop();
    auto value2 = context->stack->pop();
    context->stack->push(value2 - value1 );
}

void JrInterpreter::exec_idiv(const Instruction &instrunction) {
    auto value1 = context->stack->pop();
    auto value2 = context->stack->pop();
    context->stack->push(value2 / value1);
}

void JrInterpreter::exec_irem(const Instruction &instrunction) {
    auto value1 = context->stack->pop();
    auto value2 = context->stack->pop();
    
    context->stack->push(value2 % value1);
}

void JrInterpreter::exec_ifle(const Instruction &instrunction) {
    auto value1 = context->stack->pop();
    if(value1 <= 0) {
        pointer += instrunction.value;
    }
}

void JrInterpreter::exec_goto(const Instruction &instruction) {
    auto value1 = context->stack->pop();
    pointer += instruction.value;
}

void JrInterpreter::exec_ireturn(const Instruction &instruction) {
    auto value = context->stack->pop();
    context->stack->pop(context->stack->topFrame());
    context->stack->push(value);
}

void JrInterpreter::exec_return(const Instruction &instruction) {
    context->stack->pop(context->stack->topFrame());
}

void JrInterpreter::exec_oconst_nil(const Instruction &instruction) {
    context->stack->push(0);
}

void JrInterpreter::exec_new(const Instruction &instruction) {
    auto function = Global::functions[instruction.value];
    assert(function->kind == jrFuncConstructor);
    
    auto returnType = function->returnType;
    
    assert(returnType->kind == JrType_Object);
    auto objectType = std::static_pointer_cast<JrObjectType>(returnType);
    auto objectRef = context->gc->alloc(objectType);
    auto frame = context->stack->topFrame();
    auto addressOfVariable = frame->addressOfVariables[function->paramTypes.size() - 1];
    context->stack->storeValueForVariable(addressOfVariable, objectRef);
    
    JrInterpreter interpreter(context);
    interpreter.run(function);
}

void JrInterpreter::exec_putfield(const Instruction &instruction) {
    auto frame = context->stack->topFrame();
}
