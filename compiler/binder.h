#ifndef __joyeer_compiler_syntax_binder_h__
#define __joyeer_compiler_syntax_binder_h__


#include "node.h"
#include "types.h"
#include "scope.h"

struct BindContext {
public:
    std::stack<SymTable::Pointer> symbols;
    
    // current semantic analyzing scope, e.g. source level, class level, method level etc.
    std::stack<Scope::Pointer> scopes;
    
public:
    Symbol::Pointer makeSymbol(Node::Pointer node, const std::wstring& name, SymbolFlag flag);
    
    // return the current parsing symbol tables
    SymTable::Pointer currentSymTable();
    
    // return the current parsing scope
    Scope::Pointer currentScope();

    void enter(SymTable::Pointer table);
    void leave(SymTable::Pointer table);
    
    // enter or leave the parsing scope
    void enterScope(ScopeFlag flag, TypeDescriptor::Pointer scopeType);
    void leaveScope(ScopeFlag flag, TypeDescriptor::Pointer scopeType);
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
    
    void bind(ConstructorDecl::Pointer decl);
    
    void bind(Type::Pointer decl);
    
    void bind(FuncCallExpr::Pointer decl);
    
private:
    SymbolFactory::Pointer symFactory;
    TypeFactory::Pointer typeFactory;
    
    std::shared_ptr<BindContext> context;
};

#endif
