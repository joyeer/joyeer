#ifndef __joyeer_compiler_syntax_binder_h__
#define __joyeer_compiler_syntax_binder_h__

#include "compiler/AST/ast.h"
#include "syntax.h"
#include "symtable.h"

class Binder {
public:
    Binder(SymbolFactoryPtr symFactory);
    
    void bind(std::shared_ptr<Node> node);
    
protected:
    void bind(std::shared_ptr<SourceFile> sourceFile);
    void bind(ClassDecl::Pointer classDecl);
    
private:
    SymbolFactoryPtr symFactory;
};

#endif
