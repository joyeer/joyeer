#include "joyeer/compiler/binder.h"
#include "joyeer/compiler/diagnostic.h"
#include "joyeer/compiler/sourcefile.h"
#include "joyeer/compiler/compiler+service.h"
#include "joyeer/runtime/buildin.h"
#include "joyeer/runtime/sys/module.h"

#include <cassert>


/////////////////////////////////////////////////////////////////
// Binder
/////////////////////////////////////////////////////////////////

Binder::Binder(CompileContext::Ptr context):
NodeVisitor(),
context(context) {
}

Node::Ptr Binder::visit(Node::Ptr node) {
    return NodeVisitor::visit(node);
}

Node::Ptr Binder::visit(FileModuleDecl::Ptr filemodule) {
    // register FileModule
    context->compiler->declare(filemodule);
    
    filemodule = normalizeAndPrepareDefaultStaticConstructorForFileModule(filemodule);
    filemodule->recursiveUpdate();
    
    filemodule->symbol = Symbol::make(SymbolFlag::fileModuleSymbol, filemodule->getSimpleName());
    
    context->visit(CompileStage::visitFileModule, filemodule, [filemodule, this]() {
        // visit static fields
        auto staticFields = std::vector<DeclNode::Ptr>();
        for(auto& fieldStatement : filemodule->staticFields) {
            staticFields.push_back(std::static_pointer_cast<DeclNode>(visit(fieldStatement)));
        }
        filemodule->staticFields = staticFields;
        
        // visit instance fields
        auto instanceFields = std::vector<DeclNode::Ptr>();
        for(auto& instanceField : filemodule->instanceFields) {
            instanceFields.push_back(std::static_pointer_cast<DeclNode>(visit(instanceField)));
        }
        filemodule->instanceFields = instanceFields;
        
        // visit constructors
        auto constructors = std::vector<FuncDecl::Ptr>();
        for(auto& constructor: filemodule->constructors) {
            constructors.push_back(std::static_pointer_cast<FuncDecl>(visit(constructor)));
        }
        filemodule->constructors = constructors;
        
        // visit static methods
        auto staticMethods = std::vector<FuncDecl::Ptr>();
        for(auto& method: filemodule->staticMethods) {
            staticMethods.push_back(std::static_pointer_cast<FuncDecl>(visit(method)));
        }
        filemodule->staticMethods = staticMethods;
        
        // visit instance methods
        auto instanceMethods = std::vector<FuncDecl::Ptr>();
        for(auto& method: filemodule->instanceMethods) {
            instanceMethods.push_back(std::static_pointer_cast<FuncDecl>(visit(method)));
        }
        filemodule->instanceMethods = instanceMethods;
    });
    
    return filemodule;
}

Node::Ptr Binder::visit(ClassDecl::Ptr decl) {
    
    auto symtable = context->curSymTable();
    auto name = decl->getSimpleName();
    
    if(symtable->find(name) != nullptr) {
        Diagnostics::reportError("[Error] duplicate class name");
    }
    
    auto symbol = Symbol::Ptr(new Symbol {
        .flag = SymbolFlag::typeSymbol,
        .name = name
    });
    symtable->insert(symbol);
    
    auto objectType = new JrObjectType(name);
    
    Global::registerObjectType(objectType);
    symbol->addressOfType = objectType->addressOfType;
    decl->symbol = symbol;
    
    decl->symtable = context->curSymTable();
    context->entry(objectType);
    bool hasCustomizedConstructor = false;
    context->visit(CompileStage::visitClassDecl, decl, [this, decl, symtable, &hasCustomizedConstructor]() {
        decl->members = std::static_pointer_cast<StmtsBlock>(visit(decl->members));
    });
    
    if(hasCustomizedConstructor == false) {
        // if has no customize constructors , we will bind an default constructor
        auto defaultConstructor = new JrFunction();
        defaultConstructor->name = name + "()";
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
            .flag = SymbolFlag::funcSymbol,
            .addressOfFunc = defaultConstructor->addressOfFunc
        });
        symtable->insert(symbol);
        objectType->constructors.push_back(defaultConstructor->addressOfFunc);
    }
    
    // assocated type with type's symbol
    context->associate(objectType, context->curSymTable());
    context->leave(objectType);
    
    return decl;
}

Node::Ptr Binder::visit(FuncDecl::Ptr decl) {
    assert(decl->descriptor != nullptr);
    auto symtable = context->curSymTable();

    auto funcSimpleName = decl->getSimpleName();
    auto declaringClassDecl = decl->getDeclaringClassDecl();
    assert(declaringClassDecl != nullptr);
    
    // check if the function name duplicated
    if(symtable->find(funcSimpleName) != nullptr) {
        Diagnostics::reportError("[Error] Dupliate function name");
    }

    // prepare the symbol, register the symbol into parent
    decl->symbol = Symbol::make(funcTypeToSymbolFlag(decl->type), funcSimpleName);
    symtable->insert(decl->symbol);
    
    // visit func decleration
    context->visit(CompileStage::visitFuncDecl, decl, [this, decl]() {
        // start to process function name
        context->visit(CompileStage::visitFuncNameDecl, [this, decl]() {
            if(decl->identifier != nullptr) {
                decl->identifier = visit(decl->identifier);
            }
        });
        
        // start to process function parameters
        context->visit(CompileStage::visitFuncParamDecl, [this, decl]() {
            if(decl->parameterClause != nullptr) {
                decl->parameterClause = visit(decl->parameterClause);
            }
        });
        
        if(decl->returnType != nullptr) {
            decl->returnType = visit(decl->returnType);
        }
        
        decl->codeBlock = visit(decl->codeBlock);
        
    });
    
    return decl;
}


Node::Ptr Binder::visit(VarDecl::Ptr decl) {
    
    auto pattern = decl->pattern;
    auto symtable = context->curSymTable();
    auto name = pattern->identifier->getSimpleName();
    
    if(symtable->find(name) != nullptr) {
        Diagnostics::reportError("[Error] duplicate variable name");
    }
    
    // double check the domplciate
    auto stage = context->curStage();
    auto symbolFlag = (stage == CompileStage::visitClassDecl || stage == CompileStage::visitFileModule) ? SymbolFlag::fieldSymbol : SymbolFlag::varSymbol;
    
    auto symbol = std::shared_ptr<Symbol>(new Symbol{
        .name = name,
        .flag = symbolFlag
    });
    symtable->insert(symbol);
    decl->symbol = symbol;
    decl->pattern->identifier->symbol = symbol;
    
    if(decl->initializer != nullptr) {
        decl->initializer = visit(decl->initializer);
    }
    
    return decl;
}

Node::Ptr Binder::visit(FuncCallExpr::Ptr decl) {
    
    if(decl->identifier->kind == SyntaxKind::memberAccessExpr) {
        // Transfer to MemberF
        auto memberAccessExpr = std::static_pointer_cast<MemberAccessExpr>(decl->identifier);
        auto memberFuncCallExpr = std::make_shared<MemberFuncCallExpr>(memberAccessExpr->callee, memberAccessExpr->member, decl->arguments);
        return visit(memberFuncCallExpr);
    }
    
    // go down to bind argument
    std::vector<ArguCallExpr::Ptr> argus;
    for(auto& paramter: decl->arguments) {
        argus.push_back(std::static_pointer_cast<ArguCallExpr>(visit(paramter)));
    }
    decl->arguments = argus;
    
    return decl;
}

Node::Ptr Binder::visit(MemberFuncCallExpr::Ptr decl) {
    decl->callee = visit(decl->callee);
    
    std::vector<ArguCallExpr::Ptr> argus;
    for(auto &parameter: decl->arguments) {
        argus.push_back(std::static_pointer_cast<ArguCallExpr>(visit(parameter)));
    }
    decl->arguments = argus;
    return decl;
}

Node::Ptr Binder::visit(ArguCallExpr::Ptr decl) {
    decl->expr = visit(decl->expr);
    
    return decl;
}

Node::Ptr Binder::visit(LiteralExpr::Ptr decl) {
    return decl;
}

Node::Ptr Binder::visit(PrefixExpr::Ptr decl) {
    decl->expr = visit(decl->expr);
    return decl;
}

Node::Ptr Binder::visit(IdentifierExpr::Ptr decl) {
    auto name = decl->getSimpleName();
    
    switch (context->curStage()) {
        case CompileStage::visitFuncParamDecl: {
            // verify the func delcaration's parameter duplicate name
            auto table = context->curSymTable();
            if(table->find(name) != nullptr) {
                Diagnostics::reportError("[Error] duplicate variable declaration in fucntion");
                return nullptr;
            }
            
            auto symbol = std::shared_ptr<Symbol>(new Symbol {
                .flag = SymbolFlag::varSymbol,
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

Node::Ptr Binder::visit(Expr::Ptr decl) {
    
    // If binary is assignment
    if(decl->binaries.size() == 1 && decl->binaries[0]->kind == SyntaxKind::assignmentExpr) {
        if(decl->prefix->kind == SyntaxKind::identifierExpr) {
            auto identifier = std::static_pointer_cast<IdentifierExpr>(visit(decl->prefix));
            auto assignmentExpr = std::static_pointer_cast<AssignmentExpr>(visit(decl->binaries[0]));
            assignmentExpr->left = identifier;
            return assignmentExpr;
        }
        
        if(decl->prefix->kind == SyntaxKind::selfExpr) {
            auto selfExpr = std::static_pointer_cast<SelfExpr>(visit(decl->prefix));
            auto assignmentExpr = std::static_pointer_cast<AssignmentExpr>(visit(decl->binaries[0]));
            assignmentExpr->left = selfExpr;
            return assignmentExpr;
        }
        
        if(decl->prefix->kind == SyntaxKind::memberAccessExpr) {
            auto memberAccessExpr = std::static_pointer_cast<MemberAccessExpr>(visit(decl->prefix));
            auto assignmentExpr = std::static_pointer_cast<AssignmentExpr>(visit(decl->binaries[0]));
            assignmentExpr->left = memberAccessExpr;
            return assignmentExpr;
        }
        
        if(decl->prefix->kind == SyntaxKind::subscriptExpr) {
            auto subscriptExpr = std::static_pointer_cast<SubscriptExpr>(visit(decl->prefix));
            auto assignmentExpr = std::static_pointer_cast<AssignmentExpr>(visit(decl->binaries[0]));
            assignmentExpr->left = subscriptExpr;
            return assignmentExpr;
        }
        Diagnostics::reportError("[Error] left of assignment expression must be a variable");
        return decl;
        
    }
    
    
    // visit sub nodes first;
    if(decl->prefix != nullptr) {
        decl->prefix = visit(decl->prefix);
    }
    
    std::vector<Node::Ptr> binaries;
    for(auto n: decl->binaries) {
        binaries.push_back(visit(n));
    }
    decl->binaries = binaries;
    
    
    // Flat the expr subnodes
    std::vector<Node::Ptr> nodes;
    nodes.push_back(decl->prefix);
    
    for(auto node: decl->binaries) {
        if(node->kind != SyntaxKind::binaryExpr) {
            Diagnostics::reportError("[Error] Except an binary expression");
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
        if(n->kind != SyntaxKind::operatorExpr) {
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
            Diagnostics::reportError("[Error] Except");
        }
        
        auto l = temps.back();
        temps.pop_back();
        iterator ++;
        if(iterator == nodes.end()) {
            Diagnostics::reportError("[Error] Except");
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

Node::Ptr Binder::visit(AssignmentExpr::Ptr decl) {
    decl->expr = visit(decl->expr);
    return decl;
}

Node::Ptr Binder::visit(BinaryExpr::Ptr decl) {
    decl->expr = visit(decl->expr);
    return decl;
}

Node::Ptr Binder::visit(OperatorExpr::Ptr decl) {
    return decl;
}

Node::Ptr Binder::visit(ParenthesizedExpr::Ptr decl) {
    if(decl->expr->kind == SyntaxKind::parenthesizedExpr) {
        auto n = std::static_pointer_cast<ParenthesizedExpr>(decl->expr);
        return visit(n);
    }
    
    decl->expr = visit(decl->expr);
    return decl;
}

Node::Ptr Binder::visit(IfStmt::Ptr decl) {
    decl->condition = visit(decl->condition);
    decl->ifCodeBlock = visit(decl->ifCodeBlock);
    if(decl->elseCodeBlock != nullptr) {
        decl->elseCodeBlock = visit(decl->elseCodeBlock);
    }
    return decl;
}

Node::Ptr Binder::visit(WhileStmt::Ptr decl) {
    decl->expr = visit(decl->expr);
    decl->codeBlock = visit(decl->codeBlock);
    return decl;
}

Node::Ptr Binder::visit(StmtsBlock::Ptr decl) {
    
    auto table = context->curSymTable();
    decl->symtable = table;
    
    // start to process code block
    context->visit(CompileStage::visitCodeBlock, [decl, this]() {
        std::vector<Node::Ptr> statements;
        for(auto s: decl->statements) {
            statements.push_back(visit(s));
        }
        decl->statements = statements;
    });
    
    return decl;
}


Node::Ptr Binder::visit(ParameterClause::Ptr decl) {
    std::vector<Pattern::Ptr> parameters;
    for(auto parameter: decl->parameters) {
        parameters.push_back(std::static_pointer_cast<Pattern>(visit(parameter)));
    }
    
    decl->parameters = parameters;
    return decl;
}

Node::Ptr Binder::visit(Pattern::Ptr decl) {
    decl->identifier = std::static_pointer_cast<IdentifierExpr>(visit(decl->identifier));
    decl->type = std::static_pointer_cast<Type>(visit(decl->type));
    return decl;
}

Node::Ptr Binder::visit(Type::Ptr decl) {
    return decl;
}

Node::Ptr Binder::visit(ReturnStmt::Ptr decl) {
    if(decl->expr != nullptr) {
        decl->expr = visit(decl->expr);
    }
    return decl;
}

Node::Ptr Binder::visit(SelfExpr::Ptr decl) {
    assert(decl->identifier != nullptr);
    
    decl->identifier = std::static_pointer_cast<IdentifierExpr>(visit(decl->identifier));
    return decl;
}

Node::Ptr Binder::visit(ArrayLiteralExpr::Ptr decl) {
    std::vector<Node::Ptr> result;
    for(auto item: decl->items) {
        result.push_back(visit(item));
    }
    decl->items = result;
    return decl;
}

Node::Ptr Binder::visit(DictLiteralExpr::Ptr decl) {
    std::vector<std::tuple<Node::Ptr, Node::Ptr>> result;
    for(auto item: decl->items) {
        auto keyItem = visit(std::get<0>(item));
        auto valueItem = visit(std::get<1>(item));
        result.push_back(std::make_tuple(keyItem , valueItem));
    }
    decl->items = result;
    return decl;
}

Node::Ptr Binder::visit(MemberAccessExpr::Ptr decl) {
    decl->callee = visit(decl->callee);
    decl->member = visit(decl->member);
    return decl;
}

Node::Ptr Binder::visit(SubscriptExpr::Ptr decl) {
    decl->identifier = visit(decl->identifier);
    decl->indexExpr = visit(decl->indexExpr);
    
    return decl;
}

Node::Ptr Binder::visit(ArrayType::Ptr decl) {
    decl->type = visit(decl->type);
    return decl;
}

Node::Ptr Binder::visit(FileImportStmt::Ptr decl) {
    if(context->curStage() != CompileStage::visitFileModule) {
        Diagnostics::reportError(Diagnostics::errorFileImportShouldAtTopOfSourceFile);
        return nullptr;
    }
    
    auto moduleName = decl->stringLiteral->rawValue;
    auto importedSourceFile = importDelegate(context, moduleName);
    
    context->importSymbolTableOfModule(importedSourceFile->exportedSymbolTable);

    return decl;
}

FileModuleDecl::Ptr  Binder::normalizeAndPrepareDefaultStaticConstructorForFileModule(FileModuleDecl::Ptr filemodule) {
    
    // declaration inside of the module
    auto declarations = std::vector<DeclNode::Ptr>();
    // statements & expression inside of the module
    auto statements = std::vector<Node::Ptr>();
    
    for(auto statement: filemodule->members->statements) {
        if(statement->kind == SyntaxKind::varDecl ) {
            auto varDecl = std::static_pointer_cast<VarDecl>(statement);
            varDecl->accessFlag = NodeAccessFlag::_static;
            
            // separate the VarDecl declaration and value initlizer expr
            auto expr = varDecl->initializer;
            auto identifierExpr = varDecl->pattern->identifier;
            
            auto memberAssignExpr = std::make_shared<MemberAssignExpr>(identifierExpr, expr);
            varDecl->initializer = nullptr;
            declarations.push_back(varDecl);
            statements.push_back(memberAssignExpr);
            
            continue;
        }
        
        if(statement->isDeclNode()) {
            declarations.push_back(std::static_pointer_cast<DeclNode>(statement));
        } else {
            statements.push_back(statement);
        }
    }
    
    // prepare for FileModule initializer
    auto moduleStaticInitializer = FuncDecl::makeStaticInitializer(std::make_shared<StmtsBlock>(statements));
    
    // preapre for filemodule initializer's descriptor
    moduleStaticInitializer->descriptor = std::make_shared<FileModuleInitializerDescriptor>(std::static_pointer_cast<FileModuleDescriptor>(filemodule->descriptor));
    
    filemodule->staticConstructor = moduleStaticInitializer;
    
    // clear the code block
    filemodule->members = nullptr;
    filemodule->staticFields = declarations;
    
    // register filemodule initializer in compile service
    context->compiler->declare(moduleStaticInitializer);
    return filemodule;
}
