#include "interpreter.h"
#include "buildin.h"
#include "gc.h"
#include "runtime/sys/array.h"
#include <cassert>
#include <iostream>

JrInterpreter::JrInterpreter(JrRuntimeContext::Pointer context):
context(context) {
}

void JrInterpreter::run(JrModuleType* module) {
    auto objectRef = context->gc->alloc(module);
    assert(module->constructors.size() == 1);
    auto mainFunc = Global::functions[module->constructors.back()];
    context->stack->push(objectRef);
    run(mainFunc, objectRef);
}

void JrInterpreter::run(JrFunction::Pointer function, int objectRef) {
    
    auto frame = prepareStackFrame(function);
    context->stack->startFuncCall(frame);
    
    std::wcout << std::wstring(context->stack->frames.size() - 1, L'-') << L"$[function][entry] " << function->name << std::endl;
    
    pointer = function->instructions.begin();
    end = function->instructions.end();
    JrInstructionDebugPrinter printer;
    while(pointer != end) {
        auto instruction = *pointer;
        std::wcout << std::wstring(context->stack->frames.size() , L'-') << L"[stack:" << context->stack->pointer << L"] #" << pointer - function->instructions.begin() << L" " << printer.print(instruction) << std::endl;
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
            case OP_GETFIELD:
                exec_getfield(instruction);
                break;
            case OP_ONEWARRAY:
                exec_onewarray(instruction);
                break;
            case OP_ICMP_G:
                exec_icmp_g(instruction);
                break;
            case OP_ICMP_L:
                exec_icmp_l(instruction);
                break;
            case OP_INEG:
                exec_ineg(instruction);
                break;
            case OP_IAND:
                exec_iand(instruction);
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
    
    JrInt baseAddress = context->stack->pointer;
    JrInt address = baseAddress - func->totalSizeOfParams();
    frame->startAddress = address ;
    for(auto var : func->localVars) {
        frame->addressOfVariables.push_back(address);
        address += 8;
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
    std::wcout << func->name << std::endl;
    if((func->kind & jrFuncNative) == jrFuncNative) {
        (*func->nativeCode)(context, func);
    } else if((func->kind & jrFuncVM) == jrFuncVM) {
        JrInterpreter interpreter(context);
        interpreter.run(func, -1);
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

void JrInterpreter::exec_ineg(const Instruction& instruction) {
    auto value = context->stack->pop();
    value = -value;
    context->stack->push(value);
}

void JrInterpreter::exec_iand(const Instruction &instruction) {
    auto rightValue = context->stack->pop();
    auto leftValue = context->stack->pop();
    
    context->stack->push(leftValue > 0 && rightValue > 0);
}

void JrInterpreter::exec_ifle(const Instruction &instrunction) {
    auto value1 = context->stack->pop();
    if(value1 <= 0) {
        pointer += instrunction.value;
    }
}

void JrInterpreter::exec_goto(const Instruction &instruction) {
    pointer += instruction.value;
}

void JrInterpreter::exec_ireturn(const Instruction &instruction) {
    auto value = context->stack->pop();
    context->stack->endFuncCall(context->stack->topFrame());
    context->stack->push(value);
}

void JrInterpreter::exec_return(const Instruction &instruction) {
    context->stack->endFuncCall(context->stack->topFrame());
}

void JrInterpreter::exec_oconst_nil(const Instruction &instruction) {
    assert(false);
    context->stack->push(0);
}

void JrInterpreter::exec_new(const Instruction &instruction) {
    auto function = Global::functions[instruction.value];
    assert(function->kind == jrFuncConstructor);
    
    auto returnType = function->returnType;
    
    assert(returnType->kind == typeObject);
    auto objectType = (JrObjectType*)returnType;
    auto objectRef = context->gc->alloc(objectType);
    context->stack->push(objectRef);
    
    JrInterpreter interpreter(context);
    interpreter.run(function, objectRef);
}

void JrInterpreter::exec_putfield(const Instruction &instruction) {
    auto frame = context->stack->topFrame();
    auto addressOfField = instruction.value;
    auto objectRef = context->stack->pop();
    auto valueRef = context->stack->pop();
    
    auto object = context->gc->get(objectRef);
    assert(object != nullptr);
    object->setField(valueRef, addressOfField);
}

void JrInterpreter::exec_getfield(const Instruction &instruction) {
    auto frame = context->stack->topFrame();
    auto addressOfField = instruction.value;
    
    auto objectRef = context->stack->pop();
    auto object = context->gc->get(objectRef);
    auto fieldObjCRef = object->getFieldAsObjectRef(addressOfField);
    context->stack->push(fieldObjCRef);
}

void JrInterpreter::exec_onewarray(const Instruction &instruction) {
    auto objectCount = context->stack->pop();
    
    std::vector<JrInt> objects;
    for(auto i = 0 ; i < objectCount ; i ++ ) {
        objects.push_back(context->stack->pop());
    }
    
    auto objectRef = context->gc->alloc((JrObjectType*)JrObjectIntArray::Type);
    
    auto arrayObject = (JrObjectIntArray*)context->gc->get(objectRef);
    for(auto iterator = objects.rbegin(); iterator != objects.rend(); iterator ++) {
        arrayObject->slots->push_back(*iterator);
    }
    context->stack->push(objectRef);
}

void JrInterpreter::exec_icmp_g(const Instruction &instrunction) {
    auto rightValue = context->stack->pop();
    auto leftValue = context->stack->pop();
    
    context->stack->push(leftValue > rightValue);
}

void JrInterpreter::exec_icmp_l(const Instruction &instruction) {
    auto rightValue = context->stack->pop();
    auto leftValue = context->stack->pop();
    
    context->stack->push(leftValue < rightValue);
}
