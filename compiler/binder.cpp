#include "binder.h"
#include "diagnostic.h"
#include "runtime/buildin.h"
#include "runtime/sys/module.h"
#include <cassert>


/////////////////////////////////////////////////////////////////
// Binder
/////////////////////////////////////////////////////////////////

Binder::Binder(CompileContext::Ptr context):
context(context) {
}

Node::Ptr Binder::bind(Node::Ptr node) {
    switch (node->kind) {
        case SyntaxKind::sourceBlock:
            return bind(std::static_pointer_cast<SourceBlock>(node));
        case type:
            return bind(std::static_pointer_cast<Type>(node));
        case pattern:
            return bind(std::static_pointer_cast<Pattern>(node));
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
        case ifStatement:
            return bind(std::static_pointer_cast<IfStatement>(node));
        case whileStatement:
            return bind(std::static_pointer_cast<WhileStatement>(node));
        case expr:
            return bind(std::static_pointer_cast<Expr>(node));
        case selfExpr:
            return bind(std::static_pointer_cast<SelfExpr>(node));
        case prefixExpr:
            return bind(std::static_pointer_cast<PrefixExpr>(node));
        case identifierExpr:
            return bind(std::static_pointer_cast<IdentifierExpr>(node));
        case parenthesizedExpr:
            return bind(std::static_pointer_cast<ParenthesizedExpr>(node));
        case arguCallExpr:
            return bind(std::static_pointer_cast<ArguCallExpr>(node));
        case funcCallExpr:
            return bind(std::static_pointer_cast<FuncCallExpr>(node));
        case memberAccessExpr:
            return bind(std::static_pointer_cast<MemberAccessExpr>(node));
        case SyntaxKind::memberFuncCallExpr:
            return bind(std::static_pointer_cast<MemberFuncCallExpr>(node));
        case literalExpr:
            return bind(std::static_pointer_cast<LiteralExpr>(node));
        case arrayLiteralExpr:
            return bind(std::static_pointer_cast<ArrayLiteralExpr>(node));
        case dictLiteralExpr:
            return bind(std::static_pointer_cast<DictLiteralExpr>(node));
        case assignmentExpr:
            return bind(std::static_pointer_cast<AssignmentExpr>(node));
        case binaryExpr:
            return bind(std::static_pointer_cast<BinaryExpr>(node));
        case operatorExpr:
            return bind(std::static_pointer_cast<OperatorExpr>(node));
        case returnStatement:
            return bind(std::static_pointer_cast<ReturnStatement>(node));
        case subscriptExpr:
            return bind(std::static_pointer_cast<SubscriptExpr>(node));
        case arrayType:
            return bind(std::static_pointer_cast<ArrayType>(node));
        case fileimportDecl:
            return bind(std::static_pointer_cast<FileImportDecl>(node));
        default:
            assert(false);
    }
}


SourceBlock::Ptr Binder::bind(SourceBlock::Ptr sourceBlock) {
    
    // Module class self
    auto moduleClass = new JrModuleClass(L"Module@__FILE__@" + sourceBlock->filename);
    Global::registerObjectType(moduleClass);
    
    // Module constructor function
    auto function = new JrFunction {
        .name = L"Module@__MAIN__@" + sourceBlock->filename,
        .kind = jrFuncConstructor,
    };
    function->paramTypes.push_back(moduleClass);
    Global::registerFunction(function, moduleClass);
    
    auto symbol = Symbol::Ptr(new Symbol {
        .name = moduleClass->name,
        .flag = moduleSymbol,
        .addressOfType = moduleClass->addressOfType
    });
    moduleClass->constructors.push_back(function->addressOfFunc);
    
    sourceBlock->symbol = symbol;
    sourceBlock->symtable = context->initializeSymTable();
    // The module class's symbols table is exporting
    context->exportedSymbols = sourceBlock->symtable;
    
    context->entry(moduleClass);
    context->visit(visitSourceBlock, [sourceBlock, this]() {
        auto nodes = std::vector<Node::Ptr>();
        for(auto& statement : sourceBlock->statements) {
            nodes.push_back(bind(statement));
        }
        sourceBlock->statements = nodes;
    });
    context->leave(moduleClass);
    context->finalizeSymTable();
    
    
    return sourceBlock;
}

Node::Ptr Binder::bind(FuncDecl::Ptr decl) {
    auto type = context->curType();
    auto symtable = context->curSymTable();
    auto function = new JrFunction {
        .name = decl->getTypeName(),
        .kind = jrFuncVM
    };
    
    Global::registerFunction(function, type);
    
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
    
    // If the parsing stage is visitClassDecl or visitSourceBlock, we will register function into target type
    if(type != nullptr && (context->curStage() == visitClassDecl || context->curStage() == visitSourceBlock)) {
        // if the function inside of class declaration, we will register it as virtual functions
        auto classType = (JrObjectType*)type;
        if(type->name == function->name) {
            classType->constructors.push_back(function->addressOfFunc);
        } else {
            classType->virtualFunctions.push_back(function->addressOfFunc);
        }
        
    }
    
    decl->symtable = context->initializeSymTable();
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
    context->finalizeSymTable();
    
    return decl;
}

Node::Ptr Binder::bind(ConstructorDecl::Ptr decl) {
    auto symtable = context->curSymTable();
    auto function = new JrFunction();
    auto type = context->curType();
    function->name = decl->getName(type);
    function->kind = jrFuncConstructor;
    function->returnType = type;
    if(symtable->find(function->name) != nullptr) {
        Diagnostics::reportError(L"[Error] Dupliate constructor name");
    }
    
    // register this functin in global function tables
    Global::registerFunction(function, type);
    auto symbol = std::shared_ptr<Symbol>(new Symbol {
        .name = function->name,
        .flag = constructorSymbol,
        .addressOfFunc = function->addressOfFunc
    });
    symtable->insert(symbol);
    decl->symbol = symbol;
    
    context->initializeSymTable();
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
    
    context->finalizeSymTable();
    
    return decl;

}

Node::Ptr Binder::bind(ClassDecl::Ptr decl) {
    
    auto symtable = context->curSymTable();
    auto name = decl->getName();
    
    if(symtable->find(name) != nullptr) {
        Diagnostics::reportError(L"[Error] duplicate class name");
    }
    
    auto symbol = Symbol::Ptr(new Symbol {
        .flag = typeSymbol,
        .name = name
    });
    symtable->insert(symbol);
    
    auto objectType = new JrObjectType(name);
    
    Global::registerObjectType(objectType);
    symbol->addressOfType = objectType->addressOfType;
    decl->symbol = symbol;
    
    context->initializeSymTable();
    decl->symtable = context->curSymTable();
    context->entry(objectType);
    bool hasCustomizedConstructor = false;
    context->visit(visitClassDecl, [this, decl, symtable, &hasCustomizedConstructor]() {
        std::vector<Node::Ptr> result;
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
        auto defaultConstructor = new JrFunction();
        defaultConstructor->name = name + L"()";
        defaultConstructor->kind = jrFuncVM;
        defaultConstructor->returnType = objectType;
        defaultConstructor->kind = jrFuncConstructor;
        defaultConstructor->paramTypes.push_back(objectType);
        defaultConstructor->instructions.push_back(Instruction {
            .opcode = OP_RETURN
        });
        Global::registerFunction(defaultConstructor, objectType);
        
        auto symbol = Symbol::Ptr(new Symbol{
            .name = defaultConstructor->name,
            .flag = constructorSymbol,
            .addressOfFunc = defaultConstructor->addressOfFunc
        });
        symtable->insert(symbol);
        objectType->constructors.push_back(defaultConstructor->addressOfFunc);
    }
    
    // assocated type with type's symbol 
    context->associate(objectType, context->curSymTable());
    context->leave(objectType);
    context->finalizeSymTable();
    
    return decl;
}

Node::Ptr Binder::bind(VarDecl::Ptr decl) {
    
    auto pattern = decl->pattern;
    auto symtable = context->curSymTable();
    auto name = pattern->identifier->getName();
    
    if(symtable->find(name) != nullptr) {
        Diagnostics::reportError(L"[Error] duplicate variable name");
    }
    
    // double check the domplciate
    auto stage = context->curStage();
    auto symbolFlag = (stage == visitClassDecl || stage == visitSourceBlock) ? fieldSymbol : varSymbol;
    
    auto symbol = std::shared_ptr<Symbol>(new Symbol{
        .name = name,
        .flag = symbolFlag
    });
    symbol->isMutable = true;
    symtable->insert(symbol);
    decl->symbol = symbol;
    decl->pattern->identifier->symbol = symbol;
    
    if(decl->initializer != nullptr) {
        decl->initializer = bind(decl->initializer);
    }
    
    if(stage == visitClassDecl || stage == visitSourceBlock) {
        // If var decl is a field, let's register it in type's field list
        auto ownerType = (JrObjectType*)context->curType();
        auto field = JrFieldType::Ptr(new JrFieldType {
            .name = name
        });
        ownerType->registerField(field);
        symbol->addressOfField = field->addressOfField;
    }
    
    return decl;
}

Node::Ptr Binder::bind(LetDecl::Ptr decl) {
    assert(false);
    auto pattern = decl->pattern;
    auto name = pattern->getIdentifierName();
    
    auto table = context->curSymTable();
    if(table->find(name) != nullptr) {
        Diagnostics::reportError(L"[Error] duplicate variable name");
        return nullptr;
    }
    
    auto symbol = std::shared_ptr<Symbol>(new Symbol{
        .name = name,
        .flag = SymbolFlag::varSymbol,
        .isMutable = false
    });
    
    
    table->insert(symbol);
    decl->symbol = symbol;
    
    if(decl->initializer != nullptr) {
        decl->initializer = bind(decl->initializer);
    }
    return decl;
}

Node::Ptr Binder::bind(FuncCallExpr::Ptr decl) {
    
    if(decl->identifier->kind == memberAccessExpr) {
        // Transfer to MemberF
        auto memberAccessExpr = std::static_pointer_cast<MemberAccessExpr>(decl->identifier);
        auto memberFuncCallExpr = std::make_shared<MemberFuncCallExpr>(memberAccessExpr->parent, memberAccessExpr->member, decl->arguments);
        return bind(memberFuncCallExpr);
    }
    
    // go down to bind argument
    std::vector<ArguCallExpr::Ptr> argus;
    for(auto& paramter: decl->arguments) {
        argus.push_back(std::static_pointer_cast<ArguCallExpr>(bind(paramter)));
    }
    decl->arguments = argus;
    
    return decl;
}

Node::Ptr Binder::bind(MemberFuncCallExpr::Ptr decl) {
    decl->parent = bind(decl->parent);
    
    std::vector<ArguCallExpr::Ptr> argus;
    for(auto &parameter: decl->arguments) {
        argus.push_back(std::static_pointer_cast<ArguCallExpr>(bind(parameter)));
    }
    decl->arguments = argus;
    return decl;
}

Node::Ptr Binder::bind(ArguCallExpr::Ptr decl) {
    decl->expr = bind(decl->expr);
    
    return decl;
}

Node::Ptr Binder::bind(LiteralExpr::Ptr decl) {
    return decl;
}

Node::Ptr Binder::bind(PrefixExpr::Ptr decl) {
    decl->expr = bind(decl->expr);
    return decl;
}

Node::Ptr Binder::bind(IdentifierExpr::Ptr decl) {
    auto name = decl->getName();
    
    switch (context->curStage()) {
        case visitFuncParamDecl: {
            // verify the func delcaration's parameter duplicate name
            auto table = context->curSymTable();
            if(table->find(name) != nullptr) {
                Diagnostics::reportError(L"[Error] duplicate variable declaration in fucntion");
                return nullptr;
            }
            
            auto symbol = std::shared_ptr<Symbol>(new Symbol {
                .flag = SymbolFlag::varSymbol,
                .name = name,
                .isMutable = false
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

Node::Ptr Binder::bind(Expr::Ptr decl) {
    
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
        
        if(decl->prefix->kind == memberAccessExpr) {
            auto memberAccessExpr = std::static_pointer_cast<MemberAccessExpr>(bind(decl->prefix));
            auto assignmentExpr = std::static_pointer_cast<AssignmentExpr>(bind(decl->binaries[0]));
            assignmentExpr->left = memberAccessExpr;
            return assignmentExpr;
        }
        
        if(decl->prefix->kind == subscriptExpr) {
            auto subscriptExpr = std::static_pointer_cast<SubscriptExpr>(bind(decl->prefix));
            auto assignmentExpr = std::static_pointer_cast<AssignmentExpr>(bind(decl->binaries[0]));
            assignmentExpr->left = subscriptExpr;
            return assignmentExpr;
        }
        Diagnostics::reportError(L"[Error] left of assignment expression must be a variable");
        return decl;
        
    }
    
    
    // visit sub nodes first;
    if(decl->prefix != nullptr) {
        decl->prefix = bind(decl->prefix);
    }
    
    std::vector<Node::Ptr> binaries;
    for(auto n: decl->binaries) {
        binaries.push_back(bind(n));
    }
    decl->binaries = binaries;
    
    
    // Flat the expr subnodes
    std::vector<Node::Ptr> nodes;
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
    
    std::deque<Node::Ptr> temps;
    std::deque<OperatorExpr::Ptr> operators;
    
    
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
        
        auto expr = std::make_shared<Expr>(std::vector<Node::Ptr> {l, r, opNode});
        temps.push_back(expr);
    }
    
    // assembly all
    std::vector<Node::Ptr> result;
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

Node::Ptr Binder::bind(AssignmentExpr::Ptr decl) {
    decl->expr = bind(decl->expr);
    return decl;
}

Node::Ptr Binder::bind(BinaryExpr::Ptr decl) {
    decl->expr = bind(decl->expr);
    return decl;
}

Node::Ptr Binder::bind(OperatorExpr::Ptr decl) {
    return decl;
}

Node::Ptr Binder::bind(ParenthesizedExpr::Ptr decl) {
    if(decl->expr->kind == parenthesizedExpr) {
        auto n = std::static_pointer_cast<ParenthesizedExpr>(decl->expr);
        return bind(n);
    }
    
    decl->expr = bind(decl->expr);
    return decl;
}

Node::Ptr Binder::bind(IfStatement::Ptr decl) {
    decl->condition = bind(decl->condition);
    decl->ifCodeBlock = bind(decl->ifCodeBlock);
    if(decl->elseCodeBlock != nullptr) {
        decl->elseCodeBlock = bind(decl->elseCodeBlock);
    }
    return decl;
}

Node::Ptr Binder::bind(WhileStatement::Ptr decl) {
    decl->expr = bind(decl->expr);
    decl->codeBlock = bind(decl->codeBlock);
    return decl;
}

Node::Ptr Binder::bind(CodeBlock::Ptr decl) {
    
    context->initializeSymTable();
    auto table = context->curSymTable();
    decl->symtable = table;
    
    // start to process code block
    context->visit(visitCodeBlock, [decl, this]() {
        std::vector<Node::Ptr> statements;
        for(auto s: decl->statements) {
            statements.push_back(bind(s));
        }
        decl->statements = statements;
    });
    
    context->finalizeSymTable();
    
    return decl;
}


Node::Ptr Binder::bind(ParameterClause::Ptr decl) {
    std::vector<Pattern::Ptr> parameters;
    for(auto parameter: decl->parameters) {
        parameters.push_back(std::static_pointer_cast<Pattern>(bind(parameter)));
    }
    
    decl->parameters = parameters;
    return decl;
}

Node::Ptr Binder::bind(Pattern::Ptr decl) {
    decl->identifier = std::static_pointer_cast<IdentifierExpr>(bind(decl->identifier));
    decl->type = std::static_pointer_cast<Type>(bind(decl->type));
    return decl;
}

Node::Ptr Binder::bind(Type::Ptr decl) {
    return decl;
}

Node::Ptr Binder::bind(ReturnStatement::Ptr decl) {
    if(decl->expr != nullptr) {
        decl->expr = bind(decl->expr);
    }
    return decl;
}

Node::Ptr Binder::bind(SelfExpr::Ptr decl) {
    assert(decl->identifier != nullptr);
    
    decl->identifier = std::static_pointer_cast<IdentifierExpr>(bind(decl->identifier));
    return decl;
}

Node::Ptr Binder::bind(ArrayLiteralExpr::Ptr decl) {
    std::vector<Node::Ptr> result;
    for(auto item: decl->items) {
        result.push_back(bind(item));
    }
    decl->items = result;
    return decl;
}

Node::Ptr Binder::bind(DictLiteralExpr::Ptr decl) {
    std::vector<std::tuple<Node::Ptr, Node::Ptr>> result;
    for(auto item: decl->items) {
        auto keyItem = bind(std::get<0>(item));
        auto valueItem = bind(std::get<1>(item));
        result.push_back(std::make_tuple(keyItem , valueItem));
    }
    decl->items = result;
    return decl;
}

Node::Ptr Binder::bind(MemberAccessExpr::Ptr decl) {
    decl->parent = bind(decl->parent);
    decl->member = bind(decl->member);
    return decl;
}

Node::Ptr Binder::bind(SubscriptExpr::Ptr decl) {
    decl->identifier = bind(decl->identifier);
    decl->indexExpr = bind(decl->indexExpr);
    
    return decl;
}

Node::Ptr Binder::bind(ArrayType::Ptr decl) {
    decl->type = bind(decl->type);
    return decl;
}

Node::Ptr Binder::bind(FileImportDecl::Ptr decl) {
    if(context->curStage() != visitSourceBlock) {
        Diagnostics::reportError(Diagnostics::errorFileImportShouldAtTopOfSourceFile);
        return nullptr;
    }
    
    return decl;
}
