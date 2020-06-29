#ifndef __joyeer_compiler_syntax_binder_h__
#define __joyeer_compiler_syntax_binder_h__


#include "node.h"
#include "types.h"
#include "scope.h"

struct BindContext {
public:
    BindContext(SymbolFactory::Pointer factory);
    
    Symbol::Pointer findSymbol(const std::wstring& name);
    
    Symbol::Pointer makeSymbol(Node::Pointer node, const std::wstring& name, SymbolFlag flag);
    
    // declare an variable in current scope
    Var::Pointer makeVar(Node::Pointer node, const std::wstring& name, bool isMutable);
    
    // return the current parsing symbol tables
    SymTable::Pointer currentSymTable();
    
    // return the current parsing scope
    Scope::Pointer currentScope();

    // Symbol tables entry & leave
    void enter(SymTable::Pointer table);
    void leave(SymTable::Pointer table);
    
    // enter or leave the parsing scope
    void enter(Scope::Pointer scope);
    void leave(Scope::Pointer scope);
    
private:
    SymbolFactory::Pointer factory;
    
    std::vector<SymTable::Pointer> symbols;
    // current semantic analyzing scope, e.g. source level, class level, method level etc.
    std::vector<Scope::Pointer> scopes;
    
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
    
    void bind(ConstDecl::Pointer decl);
    
    void bind(VarDecl::Pointer varDecl);
    
    void bind(ConstructorDecl::Pointer decl);
    
    void bind(Type::Pointer decl);
    
    void bind(FuncCallExpr::Pointer decl);
    
    void bind(ArguCallExpr::Pointer decl);
    
    void bind(LiteralExpr::Pointer decl);
    
    void bind(PrefixExpr::Pointer decl);
    
    void bind(IdentifierExpr::Pointer decl);
private:
    SymbolFactory::Pointer symFactory;
    TypeFactory::Pointer typeFactory;
    
    std::shared_ptr<BindContext> context;
};

#endif
