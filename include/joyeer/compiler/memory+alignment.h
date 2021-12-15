//
// Created by Qing Xu on 2021/10/27.
//

#ifndef __joyeer_compiler_memory_alignment_h__
#define __joyeer_compiler_memory_alignment_h__

#include "joyeer/compiler/node.h"

// Module's variables will be stored in
struct ModuleMemoryAlignment {
    using Ptr = std::shared_ptr<ModuleMemoryAlignment>;

    void align(const ModuleDecl::Ptr& decl);

private:
    void align(const StmtsBlock::Ptr& decl);

private:
    int base = 0;
};


#endif // __joyeer_compiler_memory_alignment_h__

