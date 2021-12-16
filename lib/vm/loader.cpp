//
// Created by Qing Xu on 2021/11/7.
//

#include "joyeer/vm/bytecode.h"
#include "joyeer/vm/loader.h"
#include "joyeer/vm/types.h"

ModuleClass* ClassLoader::load(const ModuleType::Ptr& module) {
    auto moduleClass = new ModuleClass();
    // register ModuleClass with ModuleType
    isolateVM->classTable->register_(module, moduleClass);

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
    auto method = new VMethod(bytecodes);
    isolateVM->methodTable->import(method);
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
                auto typeVariable = std::static_pointer_cast<VariableType>(compilerService->getType(value));
                auto typeClass = std::static_pointer_cast<ClassType>(compilerService->getType(typeVariable->parent));
                auto klass = isolateVM->classTable->query(typeClass);
                writer.write(DEF_BYTECODE_2(OP_GETSTATIC, klass->slot, typeVariable->position));
            }
                break;
            case OP_PUTSTATIC: {
                auto typeVariable = std::static_pointer_cast<VariableType>(compilerService->getType(value));
                auto typeClass = std::static_pointer_cast<ClassType>(compilerService->getType(typeVariable->parent));
                auto klass = isolateVM->classTable->query(typeClass);
                writer.write(DEF_BYTECODE_2(OP_PUTSTATIC, klass->slot, typeVariable->position));
            }
                break;
            case OP_OLOAD:{
                writer.write(DEF_BYTECODE(OP_OLOAD, value));
            }
                break;
            case OP_INVOKE: {
                auto typeFunc = std::static_pointer_cast<FuncType>(compilerService->getType(value));
                auto method = isolateVM->methodTable->query(typeFunc);
                assert(method != nullptr);
                writer.write(DEF_BYTECODE(OP_INVOKE, method->slot));
            }
                break;
            case OP_RETURN:
                writer.write(DEF_BYTECODE(OP_RETURN, 0));
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
            default:
                assert(false);
        }
    }

    return writer.getBytecodes();
}

Field ClassLoader::compile(const VariableType::Ptr& variableType) {
    auto type = compilerService->getType(variableType->addressOfType);
    switch (type->kind) {
        case ValueType::Int:
            return Field(ValueType::Int);
        case ValueType::Bool:
            return Field(ValueType::Bool);
        default:
            assert(false);
    }
    assert(false);
}



