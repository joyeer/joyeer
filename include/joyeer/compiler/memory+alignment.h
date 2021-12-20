//
// Created by Qing Xu on 2021/10/27.
//

#ifndef __joyeer_compiler_memory_alignment_h__
#define __joyeer_compiler_memory_alignment_h__

#include "joyeer/compiler/node.h"

// Adjust the variables be stored in right positions
struct VariablePositionAlignment {
    using Ptr = std::shared_ptr<VariablePositionAlignment>;

    // align Module
    void align(const ModuleDecl::Ptr& decl);

    // align Function
    void align(const FuncDecl::Ptr& decl);

private:
    void align(const StmtsBlock::Ptr& decl);

private:
    int base = 0;
};


#endif // __joyeer_compiler_memory_alignment_h__

