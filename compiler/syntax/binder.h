#ifndef __joyeer_compiler_syntax_binder_h__
#define __joyeer_compiler_syntax_binder_h__

#include "compiler/AST/ast.h"
#include "syntax.h"
#include "symtable.h"
#include "types.h"


struct BindContext {
    std::stack<SymTable::Pointer> symbols;
    
    SymTable::Pointer currentSymTable();
    
    void enter(SymTable::Pointer symTable);
    
    void leave(SymTable::Pointer symTable);
};


// Bind all types and symbols during analyzing AST tree
class Binder {
public:
    Binder(SymbolFactory::Pointer symFactory, TypeFactory::Pointer typeFactory);
    
    void bind(SourceBlock::Pointer sourceBlock);
    
protected:
    // recursive bind down node
    void bind(Node::Pointer node);
    
    void bind(ClassDecl::Pointer classDecl);
    
    void bind(VarDecl::Pointer varDecl);
    
private:
    SymbolFactory::Pointer symFactory;
    TypeFactory::Pointer typeFactory;

    std::shared_ptr<BindContext> context;
};

#endif
