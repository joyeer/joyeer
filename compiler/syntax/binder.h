#ifndef __joyeer_compiler_syntax_binder_h__
#define __joyeer_compiler_syntax_binder_h__

#include "compiler/AST/ast.h"

class Binder {
public:
    void bind(std::shared_ptr<SourceFile> sourceFile);
    
protected:
    void bind(std::shared_ptr<Node> node);
};

#endif
