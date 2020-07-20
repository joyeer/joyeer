#include "binder.h"
#include "diagnostic.h"
#include "runtime/buildin.h"
#include <cassert>


/////////////////////////////////////////////////////////////////
// Binder
/////////////////////////////////////////////////////////////////

Binder::Binder(CompileContext::Pointer context):
context(context) {
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
        case letDecl:
            return bind(std::static_pointer_cast<LetDecl>(node));
        case varDecl:
            return bind(std::static_pointer_cast<VarDecl>(node));
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
            return bind(std::static_pointer_cast<SelfExpr>(node));
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
    
    auto function = std::make_shared<JrFunction>();
    function->name = L"__FILE__";
    function->kind = jrFuncVM;
    Global::registerFunction(function);
    auto symbol = std::shared_ptr<Symbol>(new Symbol {
        .name = function->name,
        .flag = funcSymbol,
        .addressOfFunc = function->addressOfFunc
    });
    
    sourceBlock->symbol = symbol;
    context->initializeScope(ScopeFlag::sourceScope);
    sourceBlock->symtable = context->curSymTable();
    
    
    context->visit(visitSourceBlock, [sourceBlock, this]() {
        auto nodes = std::vector<Node::Pointer>();
        for(auto& statement : sourceBlock->statements) {
            nodes.push_back(bind(statement));
        }
        sourceBlock->statements = nodes;
    });
    
    
    context->finalizeScope(ScopeFlag::sourceScope);
    
    return sourceBlock;
}

Node::Pointer Binder::bind(FuncDecl::Pointer decl) {
    
    auto symtable = context->curSymTable();
    auto function = std::make_shared<JrFunction>();
    function->name = decl->getFuncName();
    function->kind = jrFuncVM;
    
    Global::registerFunction(function);
    
    if(symtable->find(function->name) != nullptr) {
        Diagnostics::reportError(L"[Error] Dupliate function name");
    }
    
    auto symbol = std::shared_ptr<Symbol>(new Symbol {
        .name = function->name,
        .flag = funcSymbol,
        .addressOfFunc = function->addressOfFunc
    });
    symtable->insert(symbol);
    decl->symbol = symbol;
    
    
    context->initializeScope(ScopeFlag::funcScope);
    // visit func decleration
    context->visit(visitFuncDecl, [this, decl]() {
        
        // start to process function name
        context->visit(visitFuncNameDecl, [this, decl]() {
            decl->identifier = bind(decl->identifier);
        });
        
        // start to process function parameters
        context->visit(visitFuncParamDecl, [this, decl]() {
            decl->parameterClause = bind(decl->parameterClause);
        });
        
        if(decl->returnType != nullptr) {
            decl->returnType = bind(decl->returnType);
        }
        
        decl->codeBlock = bind(decl->codeBlock);
        
    });
    // Start to Bind sub
    decl->symtable = context->curSymTable();
    
    context->finalizeScope(ScopeFlag::funcScope);
    
    return decl;
}

Node::Pointer Binder::bind(ConstructorDecl::Pointer decl) {
    auto symtable = context->curSymTable();
    auto function = std::make_shared<JrFunction>();
    auto type = context->curType();
    function->name = decl->getName(type);
    function->kind = jrFuncConstructor;
    
    if(symtable->find(function->name) != nullptr) {
        Diagnostics::reportError(L"[Error] Dupliate constructor name");
    }
    
    // register this functin in global function tables
    Global::registerFunction(function);
    auto symbol = std::shared_ptr<Symbol>(new Symbol {
        .name = function->name,
        .flag = constructorSymbol,
        .addressOfFunc = function->addressOfFunc
    });
    symtable->insert(symbol);
    decl->symbol = symbol;
    
    context->initializeScope(ScopeFlag::funcScope);
    // visit func decleration
    context->visit(visitFuncDecl, [this, decl]() {
        
        // start to process function parameters
        context->visit(visitFuncParamDecl, [this, decl]() {
            decl->parameterClause = bind(decl->parameterClause);
        });
        
        decl->codeBlock = bind(decl->codeBlock);
        
    });
    // Start to Bind sub
    decl->symtable = context->curSymTable();
    
    context->finalizeScope(ScopeFlag::funcScope);
    
    return decl;

}

Node::Pointer Binder::bind(ClassDecl::Pointer decl) {
    
    auto symtable = context->curSymTable();
    auto name = decl->getName();
    
    if(symtable->find(name) != nullptr) {
        Diagnostics::reportError(L"[Error] duplicate class name");
    }
    
    auto symbol = Symbol::Pointer(new Symbol {
        .flag = typeSymbol,
        .name = name
    });
    symtable->insert(symbol);
    
    auto objectType = JrObjectType::Pointer(new JrObjectType {{
            .name = name,
            .kind = JrType_Object
        }
    });
    
    Global::registerObjectType(objectType);
    symbol->addressOfType = objectType->addressOfType;
    decl->symbol = symbol;
    
    context->initializeScope(classScope);
    decl->symtable = context->curSymTable();
    context->entry(objectType);
    bool hasCustomizedConstructor = false;
    context->visit(visitClassDecl, [this, decl, symtable, &hasCustomizedConstructor]() {
        std::vector<Node::Pointer> result;
        for(auto member: decl->members) {
            result.push_back(bind(member));
            if(member->kind == constructorDecl) {
                // if its constructor, we should register it in parent's symbol table
                auto cdecl = std::static_pointer_cast<ConstructorDecl>(member);
                symtable->insert(cdecl->symbol);
                hasCustomizedConstructor = true;
            }
        }
        decl->members = result;
    });
    
    if(hasCustomizedConstructor == false) {
        // if has no customize constructors , we will bind an default constructor
        auto defaultConstructor = std::make_shared<JrFunction>();
        defaultConstructor->name = name + L"()";
        defaultConstructor->kind = jrFuncVM;
        defaultConstructor->returnType = objectType;
        
        Global::registerFunction(defaultConstructor);
        
        auto symbol = Symbol::Pointer(new Symbol{
            .name = defaultConstructor->name,
            .flag = constructorSymbol,
            .addressOfFunc = defaultConstructor->addressOfFunc
        });
        symtable->insert(symbol);
    }
    context->leave(objectType);
    context->finalizeScope(classScope);
    
    return decl;
}

Node::Pointer Binder::bind(VarDecl::Pointer decl) {
    
    auto pattern = decl->pattern;
    auto symtable = context->curSymTable();
    auto name = pattern->identifier->getName();
    
    if(symtable->find(name) != nullptr) {
        Diagnostics::reportError(L"[Error] duplicate variable name");
    }
     
    // double check the domplciate
    auto symbolFlag = context->curStage() == visitClassDecl ? mutableVarSymbol : mutableSymbol;
    auto symbol = std::shared_ptr<Symbol>(new Symbol{
        .name = name,
        .flag = symbolFlag
    });
    
    symtable->insert(symbol);
    decl->symbol = symbol;
    decl->pattern->identifier->symbol = symbol;
    
    if(decl->initializer != nullptr) {
        decl->initializer = bind(decl->initializer);
    }
    
    if(context->curStage() == visitClassDecl) {
        auto type = std::static_pointer_cast<JrObjectType>(context->curType());
        auto field = JrField::Pointer(new JrField {
            .type = type,
            .name = name
        });
        type->registerField(field);
        symbol->addressOfField = field->addressOfField;
    }
    
    return decl;
}

Node::Pointer Binder::bind(LetDecl::Pointer decl) {
    auto pattern = decl->pattern;
    auto name = pattern->getIdentifierName();
    
    auto table = context->curSymTable();
    if(table->find(name) != nullptr) {
        Diagnostics::reportError(L"[Error] duplicate variable name");
    }
    
    auto symbol = std::shared_ptr<Symbol>(new Symbol{
        .name = name,
        .flag = SymbolFlag::immutableVarSymbol
    });
    
    table->insert(symbol);
    decl->symbol = symbol;
    
    if(decl->initializer != nullptr) {
        decl->initializer = bind(decl->initializer);
    }
    return decl;
}



Node::Pointer Binder::bind(FuncCallExpr::Pointer decl) {
    // go down to bind argument
    std::vector<ArguCallExpr::Pointer> argus;
    for(auto& paramter: decl->arguments) {
        argus.push_back(std::static_pointer_cast<ArguCallExpr>(bind(paramter)));
    }
    decl->arguments = argus;
    
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
    auto name = decl->getName();
    auto table = context->curSymTable();
    switch (context->curStage()) {
        case visitFuncParamDecl: {
            if(table->find(name) != nullptr) {
                Diagnostics::reportError(L"[Error] duplicate variable declaration in fucntion");
            }
            
            auto symbol = std::shared_ptr<Symbol>(new Symbol {
                .flag = SymbolFlag::immutableVarSymbol,
                .name = name
            });
            table->insert(symbol);
            decl->symbol = symbol;
            return decl;
        }
        default: {
            return decl;
        }
    }
}

Node::Pointer Binder::bind(Expr::Pointer decl) {
    
    // If binary is assignment
    if(decl->binaries.size() == 1 && decl->binaries[0]->kind == assignmentExpr) {
        if(decl->prefix->kind == identifierExpr) {
            auto identifier = std::static_pointer_cast<IdentifierExpr>(bind(decl->prefix));
            auto assignmentExpr = std::static_pointer_cast<AssignmentExpr>(bind(decl->binaries[0]));
            assignmentExpr->left = identifier;
            return assignmentExpr;
        }
        
        if(decl->prefix->kind == selfExpr) {
            auto selfExpr = std::static_pointer_cast<SelfExpr>(bind(decl->prefix));
            auto assignmentExpr = std::static_pointer_cast<AssignmentExpr>(bind(decl->binaries[0]));
            assignmentExpr->left = selfExpr;
            return assignmentExpr;
        }
        
        Diagnostics::reportError(L"[Error] left of assignment expression must be a variable");
        return decl;
        
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
    
    context->initializeScope(ScopeFlag::codeBlockScope);
    auto table = context->curSymTable();
    decl->symtable = table;
    
    // start to process code block
    context->visit(visitCodeBlock, [decl, this]() {
        std::vector<Node::Pointer> statements;
        for(auto s: decl->statements) {
            statements.push_back(bind(s));
        }
        decl->statements = statements;
    });
    
    context->finalizeScope(codeBlockScope);
    
    return decl;
}


Node::Pointer Binder::bind(ParameterClause::Pointer decl) {
    std::vector<Pattern::Pointer> parameters;
    for(auto parameter: decl->parameters) {
        parameters.push_back(std::static_pointer_cast<Pattern>(bind(parameter)));
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
    return decl;
}

Node::Pointer Binder::bind(ReturnStatement::Pointer decl) {
    if(decl->expr != nullptr) {
        decl->expr = bind(decl->expr);
    }
    return decl;
}

Node::Pointer Binder::bind(SelfExpr::Pointer decl) {
    assert(decl->identifier != nullptr);
    
    decl->identifier = std::static_pointer_cast<IdentifierExpr>(bind(decl->identifier));
    return decl;
}
