//
// Created by Qing Xu on 2021/11/7.
//

#include "joyeer/vm/bytecode.h"
#include "joyeer/vm/loader.h"

void ClassLoader::load(const ModuleType::Ptr& module) {
    auto moduleClass = new ModuleClass();
    // register ModuleClass with ModuleType
    isolateVM->register_(module, moduleClass);

    auto variables = module->getVariables();
    for(auto const& variable: variables ) {
        auto field = compile(variable);
        moduleClass->staticFields.push_back(field);
    }

    isolateVM->import(moduleClass);
    // static memory allocation
    compile(module->instructions);
}


void ClassLoader::compile(const std::vector<Instruction> &instructions) {
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
                auto klass = isolateVM->query(typeClass);
                writer.write(DEF_BYTECODE_2(OP_GETSTATIC, klass->slotID, typeVariable->position));
            }
                break;
            case OP_PUTSTATIC: {
                writer.write(DEF_BYTECODE(OP_PUTSTATIC, value));
            }
                break;
            case OP_OLOAD:{
                writer.write(DEF_BYTECODE(OP_OLOAD, value));
            }
                break;
            case OP_INVOKE: {
                auto typeFunc = std::static_pointer_cast<FuncType>(compilerService->getType(value));
                auto method = isolateVM->methodTable->query(typeFunc);
                if(method == nullptr) {
                    assert(false);
                }
                writer.write(DEF_BYTECODE(OP_INVOKE, method->slotID));
            }
                break;
            default:
                assert(false);
        }
    }
}

Field ClassLoader::compile(const VariableType::Ptr& variableType) {
    auto type = compilerService->getType(variableType->addressOfType);
    switch (type->kind) {
        case ValueType::Int:
            return Field(ValueType::Int);
        default:
            assert(false);
    }
    assert(false);
}


