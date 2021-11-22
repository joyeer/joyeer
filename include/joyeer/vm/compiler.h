//
// Created by Qing Xu on 2021/11/7.
//

#ifndef __joyeer_vm_compiler_h__
#define __joyeer_vm_compiler_h__

#include "joyeer/compiler/node.h"
#include "joyeer/vm/types.h"


struct IsolateVM;

// VM compiler, compiling the Joyeer Compiler's file into interpreter accepted binary code

class VCompiler {
public:
    explicit VCompiler(IsolateVM* isolateVM): isolateVM(isolateVM) {}

    void compile(JrFileModuleType::Ptr fileModule);
    void compile(const std::vector<Instruction>& instructions);
    
private:
    IsolateVM* isolateVM;
};

#endif //__joyeer_vm_compiler_h__
