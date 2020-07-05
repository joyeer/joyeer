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

Binder::Binder(SymbolFactory::Pointer symFactory):
symFactory(symFactory),
context(std::make_shared<BindContext>(symFactory)) {
}

Node::Pointer Binder::bind(std::shared_ptr<Node> node) {
    switch (node->kind) {
        case SyntaxKind::sourceBlock:
            return bind(std::static_pointer_cast<SourceBlock>(node));
        case type:
            return bind(std::static_pointer_cast<TypeDecl>(node));
        case arrayType:
            break;
        case dictType:
            break;
        case pattern:
            return bind(std::static_pointer_cast<Pattern>(node));
        case importDecl:
            break;
        case constantDecl:
            return bind(std::static_pointer_cast<ConstDecl>(node));
        case varDecl:
            return bind(std::static_pointer_cast<VarDecl>(node));
        case letDecl:
            break;
        case funcDecl:
            return bind(std::static_pointer_cast<FuncDecl>(node));
        case constructorDecl:
            return bind(std::static_pointer_cast<ConstructorDecl>(node));
        case classDecl:
            return bind(std::static_pointer_cast<ClassDecl>(node));
        case parameterClause:
            return bind(std::static_pointer_cast<ParameterClause>(node));
        case codeBlock:
            return bind(std::static_pointer_cast<CodeBlock>(node));
        case forInStatement:
            break;
        case ifStatement:
            return bind(std::static_pointer_cast<IfStatement>(node));
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
            return bind(std::static_pointer_cast<ParenthesizedExpr>(node));
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
        case operatorExpr:
            return bind(std::static_pointer_cast<OperatorExpr>(node));
        case returnStatement:
            return bind(std::static_pointer_cast<ReturnStatement>(node));
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
    
    
    // visit sub nodes first;
    if(decl->prefix != nullptr) {
        decl->prefix = bind(decl->prefix);
    }
    
    std::vector<Node::Pointer> binaries;
    for(auto n: decl->binaries) {
        binaries.push_back(bind(n));
    }
    decl->binaries = binaries;
    
    
    // Flat the expr subnodes
    std::vector<Node::Pointer> nodes;
    nodes.push_back(decl->prefix);
    
    for(auto node: decl->binaries) {
        if(node->kind != binaryExpr) {
            Diagnostics::reportError(L"[Error] Except an binary expression");
            return decl;
        }
        
        auto bn = std::static_pointer_cast<BinaryExpr>(node);
        nodes.push_back(bn->op);
        nodes.push_back(bn->expr);
    }
    
    std::deque<Node::Pointer> temps;
    std::deque<OperatorExpr::Pointer> operators;
    
    
    for(auto iterator = nodes.begin(); iterator != nodes.end(); iterator ++ ) {
        auto n  = *iterator;
        if(n->kind != operatorExpr) {
            temps.push_back(n);
            continue;
        }
        
        auto opNode = std::static_pointer_cast<OperatorExpr>(n);
        if(Operators::getPriority(opNode->token->rawValue) == low) {
            operators.push_back(opNode);
            continue;
        }
        
        // for high priority operator
        if(temps.size() == 0) {
            Diagnostics::reportError(L"[Error] Except");
        }
        
        auto l = temps.back();
        temps.pop_back();
        iterator ++;
        if(iterator == nodes.end()) {
            Diagnostics::reportError(L"[Error] Except");
        }
        auto r = *iterator;
        
        auto expr = std::make_shared<Expr>(std::vector<Node::Pointer> {l, r, opNode});
        temps.push_back(expr);
    }
    
    // assembly all
    std::vector<Node::Pointer> result;
    while(temps.size() > 0 || operators.size() > 0 ) {
        if(temps.size() > operators.size() ) {
            auto t = temps.front();
            temps.pop_front();
            result.push_back(t);
    
            continue;
        }
        
        auto t = temps.front();
        auto o = operators.front();
        temps.pop_front();
        operators.pop_front();
        result.push_back(t);
        result.push_back(o);
        
    }
    
    decl->prefix = nullptr;
    decl->binaries.clear();
    decl->nodes = result;
    
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

Node::Pointer Binder::bind(OperatorExpr::Pointer decl) {
    return decl;
}

Node::Pointer Binder::bind(ParenthesizedExpr::Pointer decl) {
    if(decl->expr->kind == parenthesizedExpr) {
        auto n = std::static_pointer_cast<ParenthesizedExpr>(decl->expr);
        return bind(n);
    }
    
    decl->expr = bind(decl->expr);
    return decl;
}

Node::Pointer Binder::bind(IfStatement::Pointer decl) {
    decl->condition = bind(decl->condition);
    decl->ifCodeBlock = bind(decl->ifCodeBlock);
    if(decl->elseCodeBlock != nullptr) {
        decl->elseCodeBlock = bind(decl->elseCodeBlock);
    }
    return decl;
}

Node::Pointer Binder::bind(CodeBlock::Pointer decl) {
    
    SymTable::Pointer symtable = symFactory->createSymTable();
    decl->symbols = symtable;
    
    context->enter(symtable);
    
    std::vector<Node::Pointer> statements;
    for(auto s: decl->statements) {
        statements.push_back(bind(s));
    }
    
    context->leave(symtable);
    decl->statements = statements;
    return decl;
}

Node::Pointer Binder::bind(FuncDecl::Pointer decl) {
    SymTable::Pointer symtable = symFactory->createSymTable();
    auto scope = std::shared_ptr<Scope>(new Scope {
        .flag = ScopeFlag::funcScope
    });
    decl->symbols = symtable;
    decl->scope = scope;
    
    context->enter(scope);
    context->enter(symtable);
    
    decl->identifier = bind(decl->identifier);
    decl->parameterClause = bind(decl->parameterClause);
    if(decl->returnType != nullptr) {
        decl->returnType = bind(decl->returnType);
    }
    decl->codeBlock = bind(decl->codeBlock);
    
    context->leave(symtable);
    context->leave(scope);

    return decl;
}

Node::Pointer Binder::bind(ParameterClause::Pointer decl) {
    std::vector<Pattern::Pointer> parameters;
    for(auto parameter: decl->parameters) {
        parameters.push_back(std::static_pointer_cast<Pattern>(bind(parameter)));
    }
    
    for(auto parameter: decl->parameters) {
        auto name = parameter->identifier->token->rawValue;
        auto symbol = context->makeSymbol(parameter, name, SymbolFlag::paramSymbol);
        auto var = context->makeVar(parameter, name, false);
        decl->variables.push_back(var);
    }
    
    decl->parameters = parameters;
    return decl;
}

Node::Pointer Binder::bind(Pattern::Pointer decl) {
    decl->identifier = std::static_pointer_cast<IdentifierExpr>(bind(decl->identifier));
    decl->type = std::static_pointer_cast<TypeDecl>(bind(decl->type));
    return decl;
}

Node::Pointer Binder::bind(TypeDecl::Pointer decl) {
    decl->identifier = std::static_pointer_cast<IdentifierExpr>(bind(decl->identifier));
    return decl;
}

Node::Pointer Binder::bind(ReturnStatement::Pointer decl) {
    if(decl->expr != nullptr) {
        decl->expr = bind(decl->expr);
    }
    return decl;
}
