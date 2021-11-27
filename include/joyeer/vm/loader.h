//
// Created by Qing Xu on 2021/11/7.
//

#ifndef __joyeer_vm_loader_h__
#define __joyeer_vm_loader_h__

#include "joyeer/compiler/compiler+service.h"
#include "joyeer/vm/types.h"

struct IsolateVM;

// VM loader, compiling the Joyeer Compiler's file into interpreter accepted binary code

class ClassLoader {
public:
    explicit ClassLoader(IsolateVM* isolateVM): isolateVM(isolateVM) {}

    void compile(const ModuleType::Ptr& module, CompilerService* pService);

private:
    void compile(const std::vector<Instruction>& instructions);
    Field compile(const VariableType::Ptr& variableType);
    
private:
    IsolateVM* isolateVM;
    CompilerService* compilerService;
};


#endif //__joyeer_vm_loader_h__
