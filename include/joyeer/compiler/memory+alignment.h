//
// Created by Qing Xu on 2021/10/27.
//

#ifndef __joyeer_compiler_memory_alignment_h__
#define __joyeer_compiler_memory_alignment_h__

#include "joyeer/compiler/node.h"

// FileModule's variables will be stored in
struct FileModuleMemoryAlignment {
    using Ptr = std::shared_ptr<FileModuleMemoryAlignment>;

    void align(const FileModuleDecl::Ptr& decl);

private:
    void align(const StmtsBlock::Ptr& decl);

private:
    int base = 0;
};


#endif // __joyeer_compiler_memory_alignment_h__

