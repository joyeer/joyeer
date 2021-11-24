//
// Created by Qing Xu on 2021/11/7.
//

#ifndef __joyeer_vm_compiler_h__
#define __joyeer_vm_compiler_h__

#include "joyeer/compiler/compiler+service.h"
#include "joyeer/vm/types.h"

struct IsolateVM;

// VM compiler, compiling the Joyeer Compiler's file into interpreter accepted binary code

class VCompiler {
public:
    explicit VCompiler(IsolateVM* isolateVM): isolateVM(isolateVM) {}

    void compile(const ModuleType::Ptr& module, CompilerService* compilerService);

private:
    void compile(const std::vector<Instruction>& instructions);
    Field compile(const VariableType::Ptr& variableType);
    
private:
    IsolateVM* isolateVM;
    CompilerService* compilerService;
};

#endif //__joyeer_vm_compiler_h__
