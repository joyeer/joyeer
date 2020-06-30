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

Node::Pointer Binder::bind(std::shared_ptr<Node> node) {
    switch (node->kind) {
        case SyntaxKind::sourceBlock:
            return bind(std::static_pointer_cast<SourceBlock>(node));
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
            return bind(std::static_pointer_cast<ConstDecl>(node));
        case varDecl:
            return bind(std::static_pointer_cast<VarDecl>(node));
        case letDecl:
            break;
        case funcDecl:
            break;
        case constructorDecl:
            return bind(std::static_pointer_cast<ConstructorDecl>(node));
        case classDecl:
            return bind(std::static_pointer_cast<ClassDecl>(node));
        case parameterClause:
            break;
        case codeBlock:
            break;
        case forInStatement:
            break;
        case ifStatement:
            break;
        case expr:
            return bind(std::static_pointer_cast<Expr>(node));
        case selfExpr:
            break;
        case postfixExpr:
            break;
        case prefixExpr:
            return bind(std::static_pointer_cast<PrefixExpr>(node));
        case identifierExpr:
            return bind(std::static_pointer_cast<IdentifierExpr>(node));
        case parenthesizedExpr:
            break;
        case arguCallExpr:
            return bind(std::static_pointer_cast<ArguCallExpr>(node));
        case functionCallExpr:
            return bind(std::static_pointer_cast<FuncCallExpr>(node));
        case memberExpr:
            break;
        case literalExpr:
            return bind(std::static_pointer_cast<LiteralExpr>(node));
        case arrayLiteralExpr:
            break;
        case dictLiteralExpr:
            break;
        case assignmentExpr:
            return bind(std::static_pointer_cast<AssignmentExpr>(node));
        case binaryExpr:
            return bind(std::static_pointer_cast<BinaryExpr>(node));
    }
}


SourceBlock::Pointer Binder::bind(SourceBlock::Pointer sourceBlock) {
    SymTable::Pointer symtable = symFactory->createSymTable();
    auto scope = std::shared_ptr<Scope>(new Scope {
        .flag = ScopeFlag::sourceScope
    });
    sourceBlock->symbols = symtable;
    sourceBlock->scope = scope;
    
    context->enter(scope);
    context->enter(symtable);
    
    auto nodes = std::vector<Node::Pointer>();
    for(auto& statement : sourceBlock->statements) {
        nodes.push_back(bind(statement));
    }
    sourceBlock->statements = nodes;
    
    context->leave(symtable);
    context->leave(scope);
    
    return sourceBlock;
}

Node::Pointer Binder::bind(ClassDecl::Pointer classDecl) {
    
    // TODO: double the parent scope is Source File
    return classDecl;
}

Node::Pointer Binder::bind(VarDecl::Pointer decl) {
    
    auto pattern = decl->pattern;
    
    auto symbol = context->makeSymbol(decl, pattern->identifier->token->rawValue, SymbolFlag::varSymbol);
    auto variable  = context->makeVar(decl, pattern->identifier->token->rawValue, false);
    
    decl->variable = variable;
    
    if(decl->initializer != nullptr) {
        decl->initializer = bind(decl->initializer);
    }
    
    return decl;
}

Node::Pointer Binder::bind(ConstDecl::Pointer decl) {
    auto pattern = decl->pattern;
    context->makeSymbol(decl, pattern->identifier->token->rawValue, SymbolFlag::constSymbol);
    auto variable = context->makeVar(decl, pattern->identifier->token->rawValue, true);
    
    decl->variable = variable;
    
    if(decl->initializer != nullptr) {
        decl->initializer = bind(decl->initializer);
    }
    return decl;
}

Node::Pointer Binder::bind(ConstructorDecl::Pointer decl) {
    Scope::Pointer scope = context->currentScope();
    return decl;
}

Node::Pointer Binder::bind(FuncCallExpr::Pointer decl) {
    // go down to bind argument
    std::vector<ArguCallExpr::Pointer> argus;
    for(auto& paramter: decl->arguments) {
        argus.push_back(std::static_pointer_cast<ArguCallExpr>(bind(paramter)));
    }
    decl->arguments = argus;
    
    std::wstring name = decl->identifier->token->rawValue;
    name += L"(";
    for(auto& argument: decl->arguments) {
        name += argument->label->token->rawValue;
        name += L":";
    }
    name += L")";
    
    auto symbol = context->findSymbol(name);
    if(symbol == nullptr) {
        Diagnostics::reportError(L"Cannot find function");
    }
    
    decl->symbol = symbol;
    return decl;
}

Node::Pointer Binder::bind(ArguCallExpr::Pointer decl) {
    decl->expr = bind(decl->expr);
    
    return decl;
}

Node::Pointer Binder::bind(LiteralExpr::Pointer decl) {
    return decl;
}

Node::Pointer Binder::bind(PrefixExpr::Pointer decl) {
    decl->expr = bind(decl->expr);
    return decl;
}

Node::Pointer Binder::bind(IdentifierExpr::Pointer decl) {
    if(context->findSymbol(decl->token->rawValue) == nullptr) {
        Diagnostics::reportError(L"[Error] cannot find symbol");
    }
    
    auto var = context->currentScope()->find(decl->token->rawValue);
    decl->varRef = var;
    return decl;
}

Node::Pointer Binder::bind(Expr::Pointer decl) {
    // If binary is assignment
    if(decl->binaries.size() == 1 && decl->binaries[0]->kind == assignmentExpr) {
        if(decl->prefix->kind != identifierExpr) {
            Diagnostics::reportError(L"[Error] left of assignment expression must be a variable");
            return decl;
        }

        auto identifier = std::static_pointer_cast<IdentifierExpr>(bind(decl->prefix));
        auto assignmentExpr = std::static_pointer_cast<AssignmentExpr>(bind(decl->binaries[0]));
        assignmentExpr->identifier = identifier;
        return assignmentExpr;
    }
    
    // Order of the operations
    
    decl->prefix = bind(decl->prefix);
    std::vector<Node::Pointer> binaries;
    for(auto bin: decl->binaries) {
        binaries.push_back(bind(bin));
    }
    decl->binaries = binaries;
    return decl;
    
}

Node::Pointer Binder::bind(AssignmentExpr::Pointer decl) {
    decl->expr = bind(decl->expr);
    return decl;
}

Node::Pointer Binder::bind(BinaryExpr::Pointer decl) {
    decl->expr = bind(decl->expr);
    return decl;
}
