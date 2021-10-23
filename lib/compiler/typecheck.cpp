#include "joyeer/compiler/typecheck.h"
#include "joyeer/compiler/diagnostic.h"
#include "joyeer/compiler/typedef.h"
#include "joyeer/compiler/compiler+service.h"
#include <cassert>
#include <utility>

TypeChecker::TypeChecker(CompileContext::Ptr context):
context(std::move(context)) {
}

Node::Ptr TypeChecker::visit(const Node::Ptr& node) {
    return NodeVisitor::visit(node);
}

Node::Ptr TypeChecker::visit(FileModuleDecl::Ptr node) {
    
    context->visit(CompileStage::visitFileModule, node, [this, node](){
        processClassDecl(node);
    });
    
    return node;
}

Node::Ptr TypeChecker::visit(FuncDecl::Ptr node) {

    context->visit(CompileStage::visitFuncDecl, node, [this, node]() {
        auto funcDef = context->curFuncDef();
        context->visit(CompileStage::visitFuncParamDecl, [this, node]() {
            node->parameterClause = visit(node->parameterClause);
        });

        assert(funcDef->paramTypes.empty());
        if(node->returnType != nullptr) {
            node->returnType = visit(node->returnType);
            funcDef->returnType = typeOf(node->returnType);
        }

        // Binding function's type
        auto parameterClause = std::static_pointer_cast<ParameterClause>(node->parameterClause);
        for(const auto& parameter: parameterClause->parameters) {
            funcDef->paramTypes.push_back(parameter->typeDef);
            funcDef->localVars.push_back( std::make_shared<JrVariable>(parameter->typeDef, parameter->getIdentifierName()));
        }

        node->codeBlock = visit(node->codeBlock);

        // verify return statement
        verifyReturnStatement(std::static_pointer_cast<StmtsBlock>(node->codeBlock));
    });

    return node;
}

Node::Ptr TypeChecker::visit(FuncCallExpr::Ptr node) {
    
    Symbol::Ptr symbol = nullptr;
    
    if(node->identifier->kind == SyntaxKind::dictLiteralExpr) {
        node->identifier = visit(node->identifier);
        auto dictLiteral = std::static_pointer_cast<DictLiteralExpr>(node->identifier);
        if( dictLiteral->items.size() == 1) {
            auto item = dictLiteral->items[0];
            auto key = std::get<0>(item);
            auto value = std::get<1>(item);
//            if(key->symbol->flag == SymbolFlag::typeSymbol && key->symbol->flag == SymbolFlag::typeSymbol) {
//                auto dictType = std::make_shared<DictType>(key, value);
//                node->identifier = dictType;
//            }
            assert(false);
        }
    }
    
    if(node->identifier->kind == SyntaxKind::arrayLiteralExpr) {
        visit(node->identifier);
        auto arrayLiteral = std::static_pointer_cast<ArrayLiteralExpr>(node->identifier);
        if(arrayLiteral->items.size() == 1) {
            auto type = arrayLiteral->items[0];
//            if(type->symbol->flag == SymbolFlag::typeSymbol) {
//                auto arrayType = std::make_shared<ArrayType>(type);
//                node->identifier = arrayType;
//            }

            assert(false);
        }
    }
    
    auto name = node->getSimpleName();
    symbol = context->lookup(name);
    
    if(symbol == nullptr) {
        Diagnostics::reportError("[Error]Cannot find the function");
    }
    
//    node->symbol = symbol;
    for(const auto& argument: node->arguments) {
        visit(argument);
    }
    
    return node;
}



Node::Ptr TypeChecker::visit(MemberFuncCallExpr::Ptr node) {
    visit(node->callee);
    
    auto type = node->callee->typeDef;
    assert(type != nullptr);
    node->callee->typeDef = type;

    auto name = node->getSimpleName();
    auto symbol = context->lookup(name);
    
    if(symbol == nullptr) {
        Diagnostics::reportError("[Error]Cannot find the function");
    }
    
//    node->symbol = symbol;
    for(const auto& argument: node->arguments) {
        visit(argument);
    }
    
    return node;
}


Node::Ptr TypeChecker::visit(VarDecl::Ptr node) {
    context->visit(CompileStage::visitVarDecl, [this, node]() {
        visit(node->pattern);
    });
    
    if(node->pattern->type == nullptr) {
        // change the symbol to unfixed symbol
//        node->symbol->addressOfType = JrType::Any->addressOfType;
    }
    
    if(node->initializer != nullptr) {
        node->initializer = visit(node->initializer);
    }
    
    // Verify the type of expression
    if(node->initializer != nullptr) {
        auto rightType = typeOf(node->initializer);
        auto leftType = typeOf(node->pattern);
//        // the node doesn't use
//        if(node->pattern->type == nullptr && rightType != JrType::Nil) {
//            node->symbol->addressOfType = rightType->addressOfType;
//        }
    }
    
    return node;
}

Node::Ptr TypeChecker::visit(ParameterClause::Ptr node) {
    auto symtable = context->curSymTable();
    auto parameters = std::vector<Pattern::Ptr>();
    for(const auto& param: node->parameters) {
        parameters.push_back(std::static_pointer_cast<Pattern>(visit(param)));
    }
    node->parameters = parameters;
    return node;
}

Node::Ptr TypeChecker::visit(Pattern::Ptr node) {
    node->identifier = std::static_pointer_cast<IdentifierExpr>(visit(node->identifier));
    if(node->type != nullptr) {
        node->type = visit(node->type);
        // binding the identifier symbol's type to Pattern type's symbols' type
        auto type = typeOf(node->type);
//        if(node->type->symbol == nullptr) {
//            node->type->symbol = std::shared_ptr<Symbol>(new Symbol{
//                .flag = SymbolFlag::typeSymbol,
//                .type = type
//            });
//        }

//        node->identifier->symbol->addressOfType = type->addressOfType;
    }
    
    return node;
}

Node::Ptr TypeChecker::visit(IdentifierExpr::Ptr node) {
    auto name = node->getSimpleName();
    switch (context->curStage()) {
        case CompileStage::visitFileModule:
        case CompileStage::visitExpr:
        case CompileStage::visitCodeBlock:
        case CompileStage::visitMemberAccess: {
            auto symbol = context->lookup(name);
            if(symbol == nullptr) {
                Diagnostics::reportError("[Error] Cannot find variable");
            }
            assert(symbol != nullptr);
//            assert(node->symbol == nullptr);
//            node->symbol = symbol;
        }
            break;
        case CompileStage::visitAssignExpr:
        case CompileStage::visitFuncParamDecl:
        case CompileStage::visitVarDecl:
        case CompileStage::visitLetDecl:
            break;
        default:
            break;
    }
    return node;
}

ClassDecl::Ptr TypeChecker::processClassDecl(ClassDecl::Ptr decl) {
    // visit static fields
    visit(decl->members);
    return decl;
}

Node::Ptr TypeChecker::visit(Type::Ptr node) {
    auto symbol = context->lookup(node->identifier->getSimpleName());
    if(symbol == nullptr) {
        Diagnostics::reportError("[Error]Cannot find type");
    }
//    node->symbol = symbol;
    
    return node;
}

Node::Ptr TypeChecker::visit(StmtsBlock::Ptr node) {
    assert(node->symtable != nullptr);
    context->entry(node->symtable);
    context->visit(CompileStage::visitCodeBlock, [this, node]() {
        auto statements = std::vector<Node::Ptr>();
        for(const auto& statement: node->statements) {
            statements.push_back(visit(statement));
        }
        node->statements = statements;
    });
    context->leave(node->symtable);
    return node;
}

Node::Ptr TypeChecker::visit(ReturnStmt::Ptr node) {
    node->expr = visit(node->expr);
    return node;
}

Node::Ptr TypeChecker::visit(Expr::Ptr node) {
    context->visit(CompileStage::visitExpr, [this, node]() {
        auto nodes = std::vector<Node::Ptr>();
        for(const auto& n: node->nodes) {
            nodes.push_back(visit(n));
        }
        node->nodes = nodes;
        node->typeDef = typeOf(node);
    });
    
    return node;
}


Node::Ptr TypeChecker::visit(LiteralExpr::Ptr node) {
    return node;
}

Node::Ptr TypeChecker::visit(AssignExpr::Ptr node) {
    node->left = visit(node->left);
    node->expr = visit(node->expr);
    return node;
}

Node::Ptr TypeChecker::visit(ParenthesizedExpr::Ptr node) {
    node->expr = visit(node->expr);
    return node;
}

Node::Ptr TypeChecker::visit(IfStmt::Ptr node) {
    node->condition = visit(node->condition);
    
    context->visit(CompileStage::visitCodeBlock, [this, node](){
        node->ifCodeBlock = visit(node->ifCodeBlock);
    });
    
    if(node->elseCodeBlock != nullptr) {
        context->visit(CompileStage::visitCodeBlock, [this, node](){
            node->elseCodeBlock = visit(node->elseCodeBlock);
        });
    }
    
    return node;
}

Node::Ptr TypeChecker::visit(WhileStmt::Ptr node) {
    node->expr = visit(node->expr);
    
    context->visit(CompileStage::visitCodeBlock, [this, node]() {
        node->codeBlock = visit(node->codeBlock);
    });
    return node;
}

Node::Ptr TypeChecker::visit(ArguCallExpr::Ptr node) {
    node->expr = visit(node->expr);
    return node;
}

Node::Ptr TypeChecker::visit(ClassDecl::Ptr node) {
    context->entry(node->symtable);
    context->visit(CompileStage::visitClassDecl, [this, node]() {
        node->members = std::static_pointer_cast<StmtsBlock>(visit(node->members));
    });
    context->leave(node->symtable);
    return node;
}

Node::Ptr TypeChecker::visit(SelfExpr::Ptr node) {
    if(node->identifier != nullptr) {
        node->identifier = std::static_pointer_cast<IdentifierExpr>(visit(node->identifier));
    }
    return node;
}

Node::Ptr TypeChecker::visit(ArrayLiteralExpr::Ptr node) {
    auto items = std::vector<Node::Ptr>();
    for(const auto& item: node->items) {
        items.push_back(visit(item));
    }
    node->items = items;
    return node;
}

Node::Ptr TypeChecker::visit(DictLiteralExpr::Ptr node) {
    auto items = std::vector<std::tuple<Node::Ptr, Node::Ptr>>();
    for(auto item: node->items) {
        items.emplace_back(
            visit(std::get<0>(item)),
            visit(std::get<1>(item))
        );
    }
    node->items = items;
    return node;
}

Node::Ptr TypeChecker::visit(MemberAccessExpr::Ptr node) {
    node->callee = visit(node->callee);
    
//    auto type = Global::types[node->callee->symbol->addressOfType];
//    auto symtable = context->symtableOfType(type);

    context->visit(CompileStage::visitMemberAccess, [this, node](){
//        if(symtable != nullptr) {
//            // We will push access flags' symbols
//            context->entry(symtable);
//        }
        
        node->member = visit(node->member);
//        if(symtable != nullptr) {
//            context->leave(symtable);
//        }
    });
    return node;
}

Node::Ptr TypeChecker::visit(MemberAssignExpr::Ptr node) {
    return node;
}

Node::Ptr TypeChecker::visit(SubscriptExpr::Ptr node) {
    node->identifier = visit(node->identifier);
    node->indexExpr = visit(node->indexExpr);
    return node;
}

Node::Ptr TypeChecker::visit(ArrayType::Ptr node) {
    node->type = visit(node->type);
    return node;
}

Node::Ptr TypeChecker::visit(PrefixExpr::Ptr node) {
    node->expr = visit(node->expr);
    return node;
}

Node::Ptr TypeChecker::visit(FileImportStmt::Ptr node) {
    return node;
}

Node::Ptr TypeChecker::visit(BinaryExpr::Ptr decl) {
    assert(false);
}

Node::Ptr TypeChecker::visit(OperatorExpr::Ptr decl) {
    return decl;
}

JrTypeDef::Ptr TypeChecker::typeOf(const Node::Ptr& node) {
    switch (node->kind) {
        case SyntaxKind::identifierExpr:
            return typeOf(std::static_pointer_cast<IdentifierExpr>(node));
        case SyntaxKind::expr:
            return typeOf(std::static_pointer_cast<Expr>(node));
        case SyntaxKind::funcCallExpr:
            return typeOf(std::static_pointer_cast<FuncCallExpr>(node));
        case SyntaxKind::literalExpr:
            return typeOf(std::static_pointer_cast<LiteralExpr>(node));
        case SyntaxKind::parenthesizedExpr:
            return typeOf(std::static_pointer_cast<ParenthesizedExpr>(node));
        case SyntaxKind::selfExpr:
            return typeOf(std::static_pointer_cast<SelfExpr>(node));
        case SyntaxKind::pattern:
            return typeOf(std::static_pointer_cast<Pattern>(node));
        case SyntaxKind::type:
            return typeOf(std::static_pointer_cast<Type>(node));
        case SyntaxKind::arrayLiteralExpr:
            return typeOf(std::static_pointer_cast<ArrayLiteralExpr>(node));
        case SyntaxKind::dictLiteralExpr:
            return typeOf(std::static_pointer_cast<DictLiteralExpr>(node));
        case SyntaxKind::memberAccessExpr:
            return typeOf(std::static_pointer_cast<MemberAccessExpr>(node));
        case SyntaxKind::memberFuncCallExpr:
            return typeOf(std::static_pointer_cast<MemberFuncCallExpr>(node));
        case SyntaxKind::subscriptExpr:
            return typeOf(std::static_pointer_cast<SubscriptExpr>(node));
        case SyntaxKind::arrayType:
            return typeOf(std::static_pointer_cast<ArrayType>(node));
        case SyntaxKind::prefixExpr:
            return typeOf(std::static_pointer_cast<PrefixExpr>(node));
        default:
            assert(false);
    }
}

JrTypeDef::Ptr TypeChecker::typeOf(const IdentifierExpr::Ptr& node) {

    auto symbol = context->lookup(node->getSimpleName());
    switch(symbol->flag) {
        case SymbolFlag::var:
            return node->typeDef;
        case SymbolFlag::field: {
//            auto type = (JrObjectType*)(context->curType());
//            auto field = type->virtualFields[node->symbol->addressOfField];
//            return Global::types[field->addressOfField];
            assert(false);
        }
        default:
            assert(false);
    }
}

JrTypeDef::Ptr TypeChecker::typeOf(const Expr::Ptr& node) {
    assert(node->binaries.empty());
    assert(node->prefix == nullptr);
    
    if(node->typeDef != nullptr) {
        return node->typeDef;
    }
    
    std::stack<JrTypeDef::Ptr> stack;
    for (const auto &n: node->nodes)
        if (n->kind == SyntaxKind::operatorExpr) {
            auto leftType = stack.top();
            stack.pop();
            auto rightType = stack.top();
            stack.pop();
            if (leftType->type == rightType->type) {
                stack.push(leftType);
            } else {
                stack.push(leftType);
            }

        } else {
            auto type = typeOf(n);
            stack.push(type);
        };

    assert(stack.size() == 1);
    return stack.top();
}

JrTypeDef::Ptr TypeChecker::typeOf(const LiteralExpr::Ptr& node) {
    switch(node->literal->kind) {
        case booleanLiteral:
            return BuildIn::TypeDef::Bool;
        case decimalLiteral:
            return BuildIn::TypeDef::Int;
        case nilLiteral:
            return BuildIn::TypeDef::Nil;
        case stringLiteral:
            return BuildIn::TypeDef::String;
        default:
            assert(false);
    }
}

JrTypeDef::Ptr TypeChecker::typeOf(const FuncCallExpr::Ptr& node) {
    auto funcName = node->getSimpleName();
    auto symbol = context->lookup(funcName);
    assert(symbol->flag == SymbolFlag::func);
    return context->compiler->getTypeDefBy(symbol->address);
}

JrTypeDef::Ptr TypeChecker::typeOf(const MemberFuncCallExpr::Ptr& node) {
    auto funcName = node->getSimpleName();
    auto symbol = context->lookup(funcName);
    auto funcDef = std::static_pointer_cast<JrFuncTypeDef> (context->compiler->getTypeDefBy(symbol->address));
    return funcDef->returnType;
}

JrTypeDef::Ptr TypeChecker::typeOf(const ParenthesizedExpr::Ptr& node) {
    return typeOf(node->expr);
}

JrTypeDef::Ptr TypeChecker::typeOf(const SelfExpr::Ptr& node) {
    return typeOf(node->identifier);
}

JrTypeDef::Ptr TypeChecker::typeOf(const Pattern::Ptr& node) {
    if(node->type == nullptr) {
        return BuildIn::TypeDef::Any;
    }
    return typeOf(node->type);
}

JrTypeDef::Ptr TypeChecker::typeOf(const Type::Ptr& node) {
    return node->typeDef;
}

JrTypeDef::Ptr TypeChecker::typeOf(const DictLiteralExpr::Ptr& node) {
    assert(false);
}

JrTypeDef::Ptr TypeChecker::typeOf(const ArrayLiteralExpr::Ptr& node) {
    JrTypeDef::Ptr previousType = nullptr;
    for(const auto& item: node->items) {
        auto type = typeOf(item);
        if(previousType != nullptr) {
            if(type->type != previousType->type) {
                return BuildIn::TypeDef::Any;
            }
        }

        previousType = type;
    }
    
//    if(previousType->type == typeInt) {
//        return JrObjectArray::Type;
//    }
    assert(false);
    return previousType;
}

JrTypeDef::Ptr TypeChecker::typeOf(const MemberAccessExpr::Ptr& node) {
    return typeOf(node->member);
}

JrTypeDef::Ptr TypeChecker::typeOf(const SubscriptExpr::Ptr& node) {
    return typeOf(node->identifier);
}

JrTypeDef::Ptr TypeChecker::typeOf(const ArrayType::Ptr& node) {
//    auto type = typeOf(node->type);
//    if(type == JrPrimaryType::Int) {
//        return JrObjectArray::Type;
//    }
    
    assert(false);
}

JrTypeDef::Ptr TypeChecker::typeOf(const PrefixExpr::Ptr& node) {
    return typeOf(node->expr);
}

void TypeChecker::verifyReturnStatement(const StmtsBlock::Ptr& node) {
    verifyReturnStatement(node->statements);
}

void TypeChecker::verifyReturnStatement(std::vector<Node::Ptr>& statements) {
    if(statements.empty()) {
        statements.push_back(std::make_shared<ReturnStmt>(nullptr));
    }
    auto lastStatement = statements.back();
    auto returnType = returnTypeOf(lastStatement);
    if(returnType == nullptr) {
        statements.push_back(std::make_shared<ReturnStmt>(nullptr));
    }
}

JrTypeDef::Ptr TypeChecker::returnTypeOf(const StmtsBlock::Ptr& node) {
    if(node->statements.empty()) {
        return nullptr;
    }
    
    auto n = node->statements.back();
    return returnTypeOf(n);
}

JrTypeDef::Ptr TypeChecker::returnTypeOf(const IfStmt::Ptr& node) {
    auto ifBlock = returnTypeOf(node->ifCodeBlock);
    if(ifBlock == nullptr || node->elseCodeBlock == nullptr) {
        return ifBlock;
    }
    
    auto elseBlock = returnTypeOf(node->elseCodeBlock);
    if(elseBlock == nullptr) {
        return nullptr; // else block don't have return statement
    }
    assert(ifBlock->type == elseBlock->type);
    return ifBlock;
}

JrTypeDef::Ptr TypeChecker::returnTypeOf(const WhileStmt::Ptr& node) {
    return returnTypeOf(node->codeBlock);
}

JrTypeDef::Ptr TypeChecker::returnTypeOf(const FuncCallExpr::Ptr& node) {
    return node->typeDef;
}

JrTypeDef::Ptr TypeChecker::returnTypeOf(const Node::Ptr& node) {
    switch (node->kind) {
        case SyntaxKind::stmtsBlock:
            return returnTypeOf(std::static_pointer_cast<StmtsBlock>(node));
        case SyntaxKind::returnStmt: {
            auto returnStatement = std::static_pointer_cast<ReturnStmt>(node);
            if(returnStatement->expr == nullptr) {
                return BuildIn::TypeDef::Void;
            } else {
                return typeOf(returnStatement->expr);
            }
        }
        case SyntaxKind::ifStmt:
            return returnTypeOf(std::static_pointer_cast<IfStmt>(node));
        case SyntaxKind::funcCallExpr:
            return returnTypeOf(std::static_pointer_cast<FuncCallExpr>(node));
        case SyntaxKind::whileStmt:
            return returnTypeOf(std::static_pointer_cast<WhileStmt>(node));
        case SyntaxKind::assignExpr:
        case SyntaxKind::identifierExpr:
        case SyntaxKind::arrayLiteralExpr:
        case SyntaxKind::varDecl:
        case SyntaxKind::memberAccessExpr:
        case SyntaxKind::memberFuncCallExpr:
        case SyntaxKind::funcDecl:
        case SyntaxKind::fileimportStmt:
            return nullptr;
        default:
            assert(false);
    }
}
