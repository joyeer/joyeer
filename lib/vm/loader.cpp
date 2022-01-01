//
// Created by Qing Xu on 2021/11/7.
//

#include "joyeer/runtime/bytecode.h"
#include "joyeer/vm/loader.h"
#include "joyeer/vm/types.h"

ModuleClass* ClassLoader::load(const ModuleType::Ptr& module) {
    auto moduleClass = new ModuleClass();
    // register ModuleClass with ModuleType
    isolateVM->classes->import((const ClassType::Ptr &) module, moduleClass);

    auto variables = module->getVariables();
    for(auto const& variable: variables ) {
        auto field = compile(variable);
        moduleClass->staticFields.push_back(field);
    }

    isolateVM->import(moduleClass);
    // static memory allocation
    auto moduleInitializer = compile(module);

    moduleClass->initializerSlot = moduleInitializer->slot;
    return moduleClass;
}

Method *ClassLoader::compile(const ModuleType::Ptr& module) {
    auto bytecodes = compile(module->instructions);
    auto method = new VMethod(bytecodes, 0, module->getVariables().size());
    isolateVM->methods->import(method, module);
    return method;
}

Method* ClassLoader::compile(const FuncType::Ptr &funcType){
    auto bytecodes = compile(funcType->instructions);
    auto paramCount = funcType->paramTypes.size();
    auto localVarCount = paramCount + funcType->getLocalVarCount();
    auto method = new VMethod(bytecodes, paramCount, localVarCount);

    isolateVM->methods->import(method, funcType);
    return method;
}

Bytecodes* ClassLoader::compile(const std::vector<Instruction> &instructions) {
    BytecodeWriter writer{};
    auto result = std::vector<Instruction>();
    for(auto const& instruction : instructions) {
        auto opcode = instruction.opcode;
        auto value = instruction.value;
        switch (opcode) {
            case OP_SCONST:{
;                writer.write(DEF_BYTECODE(OP_SCONST, value));
            }
                break;
            case OP_ICONST: {
                writer.write(DEF_BYTECODE(OP_ICONST, value));
            }
                break;
            case OP_GETSTATIC:{
                auto typeSlot = instruction.pair.val1;
                auto location = instruction.pair.val2;
                auto typeClass = std::static_pointer_cast<ClassType>(compilerService->getType(typeSlot));
                auto klass = isolateVM->classes->query(typeClass);
                writer.write(DEF_BYTECODE_2(OP_GETSTATIC, klass->slot, location));
            }
                break;
            case OP_PUTSTATIC: {
                auto typeSlot = instruction.pair.val1;
                auto location = instruction.pair.val2;
                auto typeClass = std::static_pointer_cast<ClassType>(compilerService->getType(typeSlot));
                auto klass = isolateVM->classes->query(typeClass);
                writer.write(DEF_BYTECODE_2(OP_PUTSTATIC, klass->slot, location));
            }
                break;
            case OP_OLOAD:{
                writer.write(DEF_BYTECODE(OP_OLOAD, value));
            }
                break;
            case OP_INVOKE: {
                auto typeFunc = std::static_pointer_cast<FuncType>(compilerService->getType(value));
                auto method = isolateVM->methods->query(typeFunc);
                assert(method != nullptr);
                writer.write(DEF_BYTECODE(OP_INVOKE, method->slot));
            }
                break;
            case OP_ONEWARRAY:
                writer.write(DEF_BYTECODE(OP_ONEWARRAY, value));
                break;
            case OP_RETURN:
                writer.write(DEF_BYTECODE(OP_RETURN, 0));
                break;
            case OP_IRETURN:
                writer.write(DEF_BYTECODE(OP_IRETURN, -1));
                break;
            case OP_IFLE:
                writer.write(DEF_BYTECODE(OP_IFLE, value));
                break;
            case OP_GOTO:
                writer.write(DEF_BYTECODE(OP_GOTO, value));
                break;
            case OP_IFNE:
                writer.write(DEF_BYTECODE(OP_IFNE, value));
                break;
            case OP_IDIV:
                writer.write(DEF_BYTECODE(OP_IDIV, -1));
                break;
            case OP_IREM:
                writer.write(DEF_BYTECODE(OP_IREM, -1));
                break;
            case OP_IMUL:
                writer.write(DEF_BYTECODE(OP_IMUL, -1));
                break;
            case OP_ISUB:
                writer.write(DEF_BYTECODE(OP_ISUB, -1));
                break;
            case OP_IADD:
                writer.write(DEF_BYTECODE(OP_IADD, -1));
                break;
            case OP_ISTORE:
                writer.write(DEF_BYTECODE(OP_ISTORE, value));
                break;
            case OP_ILOAD:
                writer.write(DEF_BYTECODE(OP_ILOAD, value));
                break;
            case OP_ICMP_G:
                writer.write(DEF_BYTECODE(OP_ICMP_G, value));
                break;
            case OP_ICMP_L:
                writer.write(DEF_BYTECODE(OP_ICMP_L, value));
                break;
            case OP_IAND:
                writer.write(DEF_BYTECODE(OP_IAND, value));
                break;
            default:
                assert(false);
        }
    }

    return writer.getBytecodes();
}

Field ClassLoader::compile(const Variable::Ptr& variable) {
    auto type = compilerService->getType(variable->typeSlot);
    switch (type->kind) {
        case ValueType::Int:
            return Field(ValueType::Int);
        case ValueType::Bool:
            return Field(ValueType::Bool);
        case ValueType::Class:
            return Field(ValueType::Class);
        case ValueType::Any:
            return Field(ValueType::Any);
        default:
            assert(false);
    }
    assert(false);
}



