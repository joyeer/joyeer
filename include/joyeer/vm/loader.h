//
// Created by Qing Xu on 2021/11/7.
//

#ifndef __joyeer_vm_loader_h__
#define __joyeer_vm_loader_h__

#include "joyeer/compiler/compiler+service.h"
#include "joyeer/vm/types.h"
#include "joyeer/vm/isolate.h"


struct IsolateVM;

// VM loader, compiling the Joyeer Compiler's file into interpreter accepted binary code

class ClassLoader {
public:
    ClassLoader(IsolateVM* isolateVM, CompilerService* pService):
        isolateVM(isolateVM),
        compilerService(pService){
    }

    ModuleClass* load(const ModuleType::Ptr& module);

private:
    Method* compile(const ModuleType::Ptr& module);
    Bytecodes* compile(const std::vector<Instruction>& instructions);
    Field compile(const VariableType::Ptr& variableType);
    
private:
    IsolateVM* isolateVM;
    CompilerService* compilerService;
};



#endif //__joyeer_vm_loader_h__