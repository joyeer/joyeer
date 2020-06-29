#include "binder.h"
#include "diagnostic.h"
#include "runtime/buildin.h"
#include <cassert>

/////////////////////////////////////////////////////////////////
// BindContext
/////////////////////////////////////////////////////////////////

BindContext::BindContext(SymbolFactory::Pointer factory):
factory(factory) {
    symbols.push_back(factory->getGlobalSymbolTable());
}

Symbol::Pointer BindContext::findSymbol(const std::wstring &name) {
    for(auto symtable : symbols) {
        auto result = symtable->find(name);
        if(result != nullptr) {
            return result;
        }
    }
    return nullptr;
}

SymTable::Pointer BindContext::currentSymTable() {
    return symbols.back();
}

Scope::Pointer BindContext::currentScope() {
    return scopes.back();
}

void BindContext::enter(SymTable::Pointer table) {
    symbols.push_back(table);
}

void BindContext::leave(SymTable::Pointer table) {
    assert(table == symbols.back());
    symbols.pop_back();
}

void BindContext::enter(Scope::Pointer scope) {
    scopes.push_back(scope);
}
 
void BindContext::leave(Scope::Pointer scope) {
    assert(scopes.back() == scope);
    scopes.pop_back();
}

Symbol::Pointer BindContext::makeSymbol(Node::Pointer node, const std::wstring &name, SymbolFlag flag) {
    auto table = currentSymTable();
    
    if(table->find(name) !=  nullptr) {
        Diagnostics::reportError(L"[Error]Duplicate symbols");
    }
    
    auto symbol = std::make_shared<Symbol>(flag, name);
    table->insert(symbol);
    
    return symbol;
}

Var::Pointer BindContext::makeVar(Node::Pointer, const std::wstring &name, bool isMutable) {
    auto scope = currentScope();
    
    auto var = std::shared_ptr<Var>(new Var {
        .name = name,
        .isMutable = isMutable
    });
    scope->insert(var);
    
    return var;
}

/////////////////////////////////////////////////////////////////
// Binder
/////////////////////////////////////////////////////////////////

Binder::Binder(SymbolFactory::Pointer symFactory, TypeFactory::Pointer typeFactory):
symFactory(symFactory),
typeFactory(typeFactory),
context(std::make_shared<BindContext>(symFactory)) {
}

void Binder::bind(std::shared_ptr<Node> node) {
    switch (node->kind) {
        case SyntaxKind::sourceBlock:
            bind(std::static_pointer_cast<SourceBlock>(node));
            break;
        case type:
            break;
        case arrayType:
            break;
        case dictType:
            break;
        case pattern:
            break;
        case importDecl:
            break;
        case constantDecl:
            bind(std::static_pointer_cast<ConstDecl>(node));
            break;
        case varDecl:
            bind(std::static_pointer_cast<VarDecl>(node));
            break;
        case letDecl:
            break;
        case funcDecl:
            break;
        case constructorDecl:
            bind(std::static_pointer_cast<ConstructorDecl>(node));
            break;
        case classDecl:
            bind(std::static_pointer_cast<ClassDecl>(node));
            break;
        case parameterClause:
            break;
        case codeBlock:
            break;
        case forInStatement:
            break;
        case ifStatement:
            break;
        case expr:
            break;
        case selfExpr:
            break;
        case postfixExpr:
            break;
        case prefixExpr:
            bind(std::static_pointer_cast<PrefixExpr>(node));
            break;
        case identifierExpr:
            bind(std::static_pointer_cast<IdentifierExpr>(node));
            break;
        case parenthesizedExpr:
            break;
        case arguCallExpr:
            bind(std::static_pointer_cast<ArguCallExpr>(node));
            break;
        case functionCallExpr:
            bind(std::static_pointer_cast<FuncCallExpr>(node));
            break;
        case memberExpr:
            break;
        case literalExpr:
            bind(std::static_pointer_cast<LiteralExpr>(node));
            break;
        case arrayLiteralExpr:
            break;
        case dictLiteralExpr:
            break;
        case assignmentOperator:
            break;
        case binaryOperator:
            break;
    }
}


void Binder::bind(SourceBlock::Pointer sourceBlock) {
    SymTable::Pointer symtable = symFactory->createSymTable();
    auto scope = std::shared_ptr<Scope>(new Scope {
        .flag = ScopeFlag::sourceScope
    });
    sourceBlock->symbols = symtable;
    sourceBlock->scope = scope;
    
    context->enter(scope);
    context->enter(symtable);
    
    for(auto& statement : sourceBlock->statements) {
        bind(statement);
    }
    
    context->leave(symtable);
    context->leave(scope);
}

void Binder::bind(ClassDecl::Pointer classDecl) {
    
    // TODO: double the parent scope is Source File
    
}

void Binder::bind(VarDecl::Pointer decl) {
    
    auto pattern = decl->pattern;
    
    auto symbol = context->makeSymbol(decl, pattern->identifier->rawValue, SymbolFlag::varSymbol);
    auto variable  = context->makeVar(decl, pattern->identifier->rawValue, false);
    
    decl->variable = variable;
    
    if(decl->initializer != nullptr) {
        bind(decl->initializer);
    }
}

void Binder::bind(ConstDecl::Pointer decl) {
    auto pattern = decl->pattern;
    context->makeSymbol(decl, pattern->identifier->rawValue, SymbolFlag::constSymbol);
    auto variable = context->makeVar(decl, pattern->identifier->rawValue, true);
    
    decl->variable = variable;
    
    if(decl->initializer != nullptr) {
        bind(decl->initializer);
    }
}

void Binder::bind(ConstructorDecl::Pointer decl) {
    Scope::Pointer scope = context->currentScope();
}

void Binder::bind(FuncCallExpr::Pointer decl) {
    // go down to bind argument
    for(auto& paramter: decl->arguments) {
        bind(paramter);
    }
    
    std::wstring name = decl->identifier->rawValue;
    name += L"(";
    for(auto& argument: decl->arguments) {
        name += argument->label->rawValue;
        name += L":";
    }
    name += L")";
    
    auto symbol = context->findSymbol(name);
    if(symbol == nullptr) {
        Diagnostics::reportError(L"Cannot find function");
    }
    
    decl->symbol = symbol;
}

void Binder::bind(ArguCallExpr::Pointer decl) {
    bind(decl->expr);
}

void Binder::bind(LiteralExpr::Pointer decl) {
}

void Binder::bind(PrefixExpr::Pointer decl) {
    bind(decl->expr);
}

void Binder::bind(IdentifierExpr::Pointer decl) {
    if(context->findSymbol(decl->identifier->rawValue) == nullptr) {
        Diagnostics::reportError(L"[Error] cannot find symbol");
    }
    
}
