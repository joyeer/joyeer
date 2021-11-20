#include "joyeer/compiler/binder.h"
#include "joyeer/compiler/sourcefile.h"
#include "joyeer/compiler/compiler+service.h"
#include "joyeer/common/diagnostic.h"

#include <cassert>
#include <utility>


/////////////////////////////////////////////////////////////////
// Binder
/////////////////////////////////////////////////////////////////

Binder::Binder(CompileContext::Ptr context):
        NodeVisitor(),
        context(std::move(context)) {
}

Node::Ptr Binder::visit(const Node::Ptr& node) {
    return NodeVisitor::visit(node);
}

Node::Ptr Binder::visit(const FileModuleDecl::Ptr& fileModule) {
    // register FileModule

    auto fileModuleDef = std::make_shared<JrFileModuleType>(fileModule->getSimpleName());
    context->compiler->declare(fileModuleDef);
    fileModule->type = fileModuleDef;

    fileModule->recursiveUpdate();

    context->visit(CompileStage::visitFileModule, fileModule, [fileModule, this]() {
        visit(fileModule->members);
    });
    
    return fileModule;
}

Node::Ptr Binder::visit(const ClassDecl::Ptr& decl) {
    
    auto symtable = context->curSymTable();
    auto name = decl->getSimpleName();
    
    if(symtable->find(name) != nullptr) {
        Diagnostics::reportError("[Error] duplicate class name");
    }
    
    auto symbol = Symbol::Ptr(new Symbol {
        .flag = SymbolFlag::class_,
        .name = name
    });
    symtable->insert(symbol);

    auto objectType = JrClassType::create(name);

    decl->symtable = context->curSymTable();
    bool hasCustomizedConstructor = false;
    context->visit(CompileStage::visitClassDecl, decl, [this, decl, symtable, &hasCustomizedConstructor]() {
        decl->members = std::static_pointer_cast<StmtsBlock>(visit(decl->members));
    });

    return decl;
}

Node::Ptr Binder::visit(const FuncDecl::Ptr& decl) {
    auto symtable = context->curSymTable();

    auto funcSimpleName = decl->getSimpleName();
    auto declaringClassDecl = decl->getDeclaringClassDecl();
    assert(declaringClassDecl != nullptr);
    
    // check if the function name duplicated
    if(symtable->find(funcSimpleName) != nullptr) {
        Diagnostics::reportError("[Error] Duplicate function name");
        return nullptr;
    }

    // define FuncTypeDef
    auto funcDef = std::make_shared<JrFuncType>(funcSimpleName);
    context->compiler->declare(funcDef);

    // prepare the symbol, register the symbol into parent
    auto symbol = Symbol::make(SymbolFlag::func, funcSimpleName, funcDef->address);
    symtable->insert(symbol);
    
    // visit func declaration
    context->visit(CompileStage::visitFuncDecl, decl, [this, decl]() {
        // start to process function name
        context->visit(CompileStage::visitFuncNameDecl, decl->identifier, [this, decl]() {
            if(decl->identifier != nullptr) {
                decl->identifier = visit(decl->identifier);
            }
        });
        
        // start to process function parameters
        context->visit(CompileStage::visitFuncParamDecl, decl->parameterClause, [this, decl]() {
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


Node::Ptr Binder::visit(const VarDecl::Ptr& decl) {

    if(decl->initializer != nullptr) {
        decl->initializer = visit(decl->initializer);
    }

    auto pattern = decl->pattern;
    auto symtable = context->curSymTable();
    auto name = pattern->identifier->getSimpleName();
    
    if(symtable->find(name) != nullptr) {
        Diagnostics::reportError("[Error] duplicate variable name");
    }

    // declare the JrVariableType
    auto varDef = std::make_shared<JrVariableType>(name);
    context->compiler->declare(varDef);

    // if the closest declaration type, is the FileModuleDef/ClassDef,
    // the variable will be treated as field in Symbol
    auto declType = context->curDeclTypeDef();
    auto flag = SymbolFlag::var;
    switch (declType->kind) {
        case JrTypeKind::FileModule:
            flag = SymbolFlag::field;
            varDef->markAsStatic();
            varDef->parent = declType->address; // parent address
            break;
        default:
            assert(false);
    }

    auto symbol = Symbol::make(flag, name, varDef->address);
    symtable->insert(symbol);

    // double-check to complicate
    auto stage = context->curStage();
    switch (stage) {
        case CompileStage::visitCodeBlock: {
            auto blockDef = context->curBlockDef();
            assert(blockDef);
            blockDef->localVars.push_back(varDef);
        }
            break;
        default:
            assert(false);
    }

    return decl;
}

Node::Ptr Binder::visit(const FuncCallExpr::Ptr& decl) {
    
    if(decl->identifier->kind == SyntaxKind::memberAccessExpr) {
        // Transfer to MemberF
        auto memberAccessExpr = std::static_pointer_cast<MemberAccessExpr>(decl->identifier);
        auto memberFuncCallExpr = std::make_shared<MemberFuncCallExpr>(memberAccessExpr->callee, memberAccessExpr->member, decl->arguments);
        return visit(memberFuncCallExpr);
    }

    auto callleeSimpleName = decl->getCalleeFuncSimpleName();
    auto symbol = context->lookup(callleeSimpleName);
    if(symbol == nullptr) {
        Diagnostics::reportError(ErrorLevel::failure, "[TODO] cannot find func");
        return decl;
    }
    decl->type = context->compiler->getTypeDefBy(symbol->address);

    // go down to bind argument
    std::vector<ArguCallExpr::Ptr> argus;
    for(auto& paramter: decl->arguments) {
        argus.push_back(std::static_pointer_cast<ArguCallExpr>(visit(paramter)));
    }
    decl->arguments = argus;
    
    return decl;
}

Node::Ptr Binder::visit(const MemberFuncCallExpr::Ptr& decl) {
    decl->callee = visit(decl->callee);
    
    std::vector<ArguCallExpr::Ptr> argus;
    for(auto &parameter: decl->arguments) {
        argus.push_back(std::static_pointer_cast<ArguCallExpr>(visit(parameter)));
    }
    decl->arguments = argus;
    return decl;
}

Node::Ptr Binder::visit(const ArguCallExpr::Ptr& decl) {
    decl->expr = visit(decl->expr);
    
    return decl;
}

Node::Ptr Binder::visit(const LiteralExpr::Ptr& decl) {
    return decl;
}

Node::Ptr Binder::visit(const PrefixExpr::Ptr& decl) {
    decl->expr = visit(decl->expr);
    return decl;
}

Node::Ptr Binder::visit(const IdentifierExpr::Ptr& decl) {
    auto name = decl->getSimpleName();
    
    switch (context->curStage()) {
        case CompileStage::visitFuncParamDecl: {
            // verify the func declaration's parameter duplicate name
            auto table = context->curSymTable();
            if(table->find(name) != nullptr) {
                Diagnostics::reportError("[Error] duplicate variable declaration in fucntion");
                return nullptr;
            }
            
            auto symbol = std::shared_ptr<Symbol>(new Symbol {
                .flag = SymbolFlag::var,
                .name = name
            });
            table->insert(symbol);
            return decl;
        }
        default: {
            return decl;
        }
    }
}

Node::Ptr Binder::visit(const Expr::Ptr& decl) {
    
    // If binary is assignment
    if(decl->binaries.size() == 1 && decl->binaries[0]->kind == SyntaxKind::assignExpr) {
        if(decl->prefix->kind == SyntaxKind::identifierExpr) {
            auto identifier = std::static_pointer_cast<IdentifierExpr>(visit(decl->prefix));
            auto assignmentExpr = std::static_pointer_cast<AssignExpr>(visit(decl->binaries[0]));
            assignmentExpr->left = identifier;
            return assignmentExpr;
        }
        
        if(decl->prefix->kind == SyntaxKind::selfExpr) {
            auto selfExpr = std::static_pointer_cast<SelfExpr>(visit(decl->prefix));
            auto assignmentExpr = std::static_pointer_cast<AssignExpr>(visit(decl->binaries[0]));
            assignmentExpr->left = selfExpr;
            return assignmentExpr;
        }
        
        if(decl->prefix->kind == SyntaxKind::memberAccessExpr) {
            auto memberAccessExpr = std::static_pointer_cast<MemberAccessExpr>(visit(decl->prefix));
            auto assignmentExpr = std::static_pointer_cast<AssignExpr>(visit(decl->binaries[0]));
            assignmentExpr->left = memberAccessExpr;
            return assignmentExpr;
        }
        
        if(decl->prefix->kind == SyntaxKind::subscriptExpr) {
            auto subscriptExpr = std::static_pointer_cast<SubscriptExpr>(visit(decl->prefix));
            auto assignmentExpr = std::static_pointer_cast<AssignExpr>(visit(decl->binaries[0]));
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
    for(const auto& n: decl->binaries) {
        binaries.push_back(visit(n));
    }
    decl->binaries = binaries;
    
    
    // Flat the expr sub-nodes
    std::vector<Node::Ptr> nodes;
    nodes.push_back(decl->prefix);
    
    for(const auto& node: decl->binaries) {
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

Node::Ptr Binder::visit(const AssignExpr::Ptr& decl) {
    decl->expr = visit(decl->expr);
    return decl;
}

Node::Ptr Binder::visit(const BinaryExpr::Ptr& decl) {
    decl->expr = visit(decl->expr);
    return decl;
}

Node::Ptr Binder::visit(const OperatorExpr::Ptr& decl) {
    return decl;
}

Node::Ptr Binder::visit(const ParenthesizedExpr::Ptr& decl) {
    if(decl->expr->kind == SyntaxKind::parenthesizedExpr) {
        auto n = std::static_pointer_cast<ParenthesizedExpr>(decl->expr);
        return visit(n);
    }
    
    decl->expr = visit(decl->expr);
    return decl;
}

Node::Ptr Binder::visit(const IfStmt::Ptr& decl) {
    decl->condition = visit(decl->condition);
    decl->ifCodeBlock = std::static_pointer_cast<StmtsBlock>(visit(decl->ifCodeBlock));
    if(decl->elseCodeBlock != nullptr) {
        decl->elseCodeBlock = std::static_pointer_cast<StmtsBlock>(visit(decl->elseCodeBlock));
    }
    return decl;
}

Node::Ptr Binder::visit(const WhileStmt::Ptr& decl) {
    decl->expr = visit(decl->expr);
    decl->codeBlock = visit(decl->codeBlock);
    return decl;
}

Node::Ptr Binder::visit(const StmtsBlock::Ptr& decl) {

    // generate a JrBlockType
    auto blockDef = std::make_shared<JrBlockType>();
    context->compiler->declare(blockDef);
    decl->type = blockDef;

    // check parent's type, assign the BlockTypeDef to Parent
    auto typeDef = context->curTypeDef();
    switch (typeDef->kind) {
        case JrTypeKind::FileModule: {
            auto moduleDef = std::static_pointer_cast<JrFileModuleType>(typeDef);
            moduleDef->block = blockDef;
        }
            break;
        default:
            assert(false);
    }

    // start to process code block
    context->visit(CompileStage::visitCodeBlock, decl,[decl, this]() {
        std::vector<Node::Ptr> statements;
        for(const auto& s: decl->statements) {
            statements.push_back(visit(s));
        }
        decl->statements = statements;
    });
    
    return decl;
}


Node::Ptr Binder::visit(const ParameterClause::Ptr& decl) {
    std::vector<Pattern::Ptr> parameters;
    for(const auto& parameter: decl->parameters) {
        parameters.push_back(std::static_pointer_cast<Pattern>(visit(parameter)));
    }
    
    decl->parameters = parameters;
    return decl;
}

Node::Ptr Binder::visit(const Pattern::Ptr& decl) {
    decl->identifier = std::static_pointer_cast<IdentifierExpr>(visit(decl->identifier));
    decl->typeNode = std::static_pointer_cast<Type>(visit(decl->typeNode));
    return decl;
}

Node::Ptr Binder::visit(const Type::Ptr& decl) {
    return decl;
}

Node::Ptr Binder::visit(const ReturnStmt::Ptr& decl) {
    if(decl->expr != nullptr) {
        decl->expr = visit(decl->expr);
    }
    return decl;
}

Node::Ptr Binder::visit(const SelfExpr::Ptr& decl) {
    assert(decl->identifier != nullptr);
    
    decl->identifier = std::static_pointer_cast<IdentifierExpr>(visit(decl->identifier));
    return decl;
}

Node::Ptr Binder::visit(const ArrayLiteralExpr::Ptr& decl) {
    std::vector<Node::Ptr> result;
    for(const auto& item: decl->items) {
        result.push_back(visit(item));
    }
    decl->items = result;
    return decl;
}

Node::Ptr Binder::visit(const DictLiteralExpr::Ptr& decl) {
    std::vector<std::tuple<Node::Ptr, Node::Ptr>> result;
    for(auto item: decl->items) {
        auto keyItem = visit(std::get<0>(item));
        auto valueItem = visit(std::get<1>(item));
        result.emplace_back(keyItem , valueItem);
    }
    decl->items = result;
    return decl;
}

Node::Ptr Binder::visit(const MemberAccessExpr::Ptr& decl) {
    decl->callee = visit(decl->callee);
    decl->member = visit(decl->member);
    return decl;
}

Node::Ptr Binder::visit(const MemberAssignExpr::Ptr& decl) {
    return decl;
}

Node::Ptr Binder::visit(const SubscriptExpr::Ptr& decl) {
    decl->identifier = visit(decl->identifier);
    decl->indexExpr = visit(decl->indexExpr);
    
    return decl;
}

Node::Ptr Binder::visit(const ArrayType::Ptr& decl) {
    decl->type = visit(decl->type);
    return decl;
}

Node::Ptr Binder::visit(const FileImportStmt::Ptr& decl) {
    if(context->curStage() != CompileStage::visitFileModule) {
        Diagnostics::reportError(Diagnostics::errorFileImportShouldAtTopOfSourceFile);
        return nullptr;
    }
    
    auto moduleName = decl->stringLiteral->rawValue;
    auto importedSourceFile = importDelegate(context, moduleName);
    
    return decl;
}
