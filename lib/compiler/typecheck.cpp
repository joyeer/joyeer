#include "joyeer/compiler/typecheck.h"
#include "joyeer/runtime/diagnostic.h"
#include "joyeer/runtime/types.h"
#include "joyeer/compiler/compiler+service.h"
#include <cassert>
#include <utility>

TypeChecker::TypeChecker(CompileContext::Ptr context):
context(std::move(context)) {
    compiler = this->context->compiler;
}

Node::Ptr TypeChecker::visit(const Node::Ptr& node) {
    return NodeVisitor::visit(node);
}

Node::Ptr TypeChecker::visit(const ModuleDecl::Ptr& node) {
    
    context->visit(CompileStage::visitModule, node, [this, node](){
        auto statements = std::vector<Node::Ptr>();
        for(const auto& statement: node->statements) {
            statements.push_back(visit(statement));
        }
        node->statements =  statements;
    });
    
    return node;
}

Node::Ptr TypeChecker::visit(const FuncDecl::Ptr& decl) {

    context->visit(CompileStage::visitFuncDecl, decl, [this, decl]() {
        auto funcType = context->curFuncType();

        if(decl->returnType == nullptr) {
            // no return
            funcType->returnTypeSlot = compiler->getType(ValueType::Void)->slot;
        } else {
            decl->returnType = visit(decl->returnType);
            funcType->returnTypeSlot = decl->returnType->typeSlot;
            assert(funcType->returnTypeSlot != -1);
        }

        // visit parameters decls
        context->visit(CompileStage::visitFuncParamDecl, decl->parameterClause, [this, decl]() {
            decl->parameterClause = visit(decl->parameterClause);
        });

        assert(funcType->paramCount == 0);

        auto parameterClause = std::static_pointer_cast<ParameterClause>(decl->parameterClause);
        for(const auto& parameter: parameterClause->parameters) {
            auto parameterName = parameter->getSimpleName();
            auto variable = new Variable(parameterName);
            funcType->paramCount ++;
            funcType->localVars.push_back(variable);
        }

        decl->codeBlock = visit(decl->codeBlock);



    });

    return decl;
}

Node::Ptr TypeChecker::visit(const FuncCallExpr::Ptr& node) {
    
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
//            if(kind->symbol->flag == SymbolFlag::typeSymbol) {
//                auto arrayType = std::make_shared<ArrayType>(kind);
//                node->identifier = arrayType;
//            }

            assert(false);
        }
    }

    auto name = node->getCalleeFuncSimpleName();
    symbol = context->lookup(name);

    if(symbol == nullptr) {
        Diagnostics::reportError("[Error]Cannot find the function");
    }

    node->funcTypeSlot = symbol->typeSlot;
    auto funcType = (Function*)compiler->getType(node->funcTypeSlot);
    assert(funcType != nullptr && funcType->returnTypeSlot != -1);
    node->typeSlot = funcType->returnTypeSlot;

    for(const auto& argument: node->arguments) {
        visit(argument);
    }

    return node;
}



Node::Ptr TypeChecker::visit(const MemberFuncCallExpr::Ptr& node) {
    visit(node->callee);
    
    auto typeSlot = node->callee->typeSlot;
    assert(typeSlot != -1);

    auto symtable = compiler->getExportingSymbolTable(typeSlot);

    auto funcCallExpr = std::static_pointer_cast<FuncCallExpr>( node->member);
    auto name = funcCallExpr->getCalleeFuncSimpleName();

    auto symbol = symtable->find(name);

    if(symbol == nullptr) {
        Diagnostics::reportError("[Error]Cannot find the function");
    }

    auto funcType = (Function*)(compiler->getType(symbol->typeSlot));
    assert(funcType != nullptr && funcType->returnTypeSlot);
    node->typeSlot = funcType->returnTypeSlot;
    funcCallExpr->funcTypeSlot = funcType->slot;
    node->funcTypeSlot = funcCallExpr->funcTypeSlot;

    for(const auto& argument: funcCallExpr->arguments) {
        visit(argument);
    }

    return node;
}

Node::Ptr TypeChecker::visit(const VarDecl::Ptr& decl) {

    auto simpleName = decl->getSimpleName();
    auto symtable = context->curSymTable();
    auto symbol = context->lookup(simpleName);

    assert(decl->typeSlot == -1);
    if(decl->initializer != nullptr) {
        decl->initializer = visit(decl->initializer);
    }

    context->visit(CompileStage::visitVarDecl, decl, [this, decl]() {
        decl->pattern = std::static_pointer_cast<Pattern>(visit(decl->pattern));
    });

    // if the pattern specify the Types, VarDecl will follow the pattern
    auto patternType = compiler->getType(decl->pattern->typeSlot);
    if(patternType->kind == ValueType::Unspecified) {
        decl->typeSlot = decl->initializer->typeSlot;
    } else {
        // follow the initializer expression's typedef
        decl->typeSlot = decl->pattern->typeSlot;
    }

    assert(decl->typeSlot != -1);
    // Update the Variable's type base on declaration
    symbol->typeSlot = decl->typeSlot;

    auto declType = context->curDeclType();
    auto variableType = new Variable(simpleName);
    variableType->typeSlot = symbol->typeSlot;
    variableType->parentSlot = declType->slot;
    switch (declType->kind) {
        case ValueType::Func: {
            auto funcType = (Function*)declType;
            symbol->locationInParent = funcType->paramCount + funcType->getLocalVarCount();
            funcType->localVars.push_back(variableType);
        }
            break;
        case ValueType::Module: {
            auto moduleType = (ModuleClass*)declType;
            symbol->locationInParent = moduleType->staticFields.size();
            moduleType->staticFields.push_back(variableType);
        }
            break;
        default:
            assert(false);
    }

    return decl;
}

Node::Ptr TypeChecker::visit(const ParameterClause::Ptr& node) {

    auto parameters = std::vector<Pattern::Ptr>();
    for(const auto& param: node->parameters) {
        parameters.push_back(std::static_pointer_cast<Pattern>(visit(param)));
    }
    node->parameters = parameters;

    // enrich the symbol's type
    for(const auto& param: node->parameters) {
        auto parameterName = param->getSimpleName();
        auto symbol = context->lookup(parameterName);
        assert(symbol != nullptr);
        assert(param->typeSlot != -1);
        symbol->typeSlot = param->typeSlot;
    }
    return node;
}

Node::Ptr TypeChecker::visit(const Pattern::Ptr& node) {
    node->identifier = std::static_pointer_cast<IdentifierExpr>(visit(node->identifier));
    if(node->typeExpr != nullptr) {
        node->typeExpr = visit(node->typeExpr);
        node->typeSlot = node->typeExpr->typeSlot;
    } else {
        node->typeSlot = compiler->getType(ValueType::Unspecified)->slot;
    }

    assert(node->typeSlot != -1);
    return node;
}

Node::Ptr TypeChecker::visit(const IdentifierExpr::Ptr& node) {
    auto name = node->getSimpleName();
    switch (context->curStage()) {
        case CompileStage::visitModule:
        case CompileStage::visitExpr:
        case CompileStage::visitCodeBlock:
        case CompileStage::visitMemberAccess: {
            auto symbol = context->lookup(name);
            if(symbol == nullptr) {
                Diagnostics::reportError(ErrorLevel::failure, "[TODO][Error] Cannot find variable");
            }
            assert(symbol->typeSlot != -1);
            assert(node->typeSlot == -1);
            node->typeSlot =symbol->typeSlot;
        }
            break;
        case CompileStage::visitAssignExpr:
        case CompileStage::visitFuncParamDecl:
        case CompileStage::visitVarDecl:
            // nothing to do
            break;
        default:
            break;
    }
    return node;
}

Node::Ptr TypeChecker::visit(const TypeIdentifier::Ptr& node) {
    auto simpleName = node->getSimpleName();
    auto symbol = context->lookup(simpleName);
    if(symbol == nullptr) {
        Diagnostics::reportError("[Error]Cannot find type");
    } else {
        node->typeSlot = symbol->typeSlot;
    }

    return node;
}

Node::Ptr TypeChecker::visit(const StmtsBlock::Ptr& node) {
    assert(node->symtable != nullptr);

    context->visit(CompileStage::visitCodeBlock, node, [this, node]() {
        auto statements = std::vector<Node::Ptr>();
        for(const auto& statement: node->statements) {
            statements.push_back(visit(statement));
        }
        node->statements = statements;
    });
    return node;
}

Node::Ptr TypeChecker::visit(const ReturnStmt::Ptr& decl) {
    if(decl->expr != nullptr) {
        decl->expr = visit(decl->expr);
    }

    return decl;
}

Node::Ptr TypeChecker::visit(const Expr::Ptr& node) {
    context->visit(CompileStage::visitExpr, node, [this, node]() {
        auto nodes = std::vector<Node::Ptr>();
        for(const auto& n: node->nodes) {
            nodes.push_back(visit(n));
        }
        node->nodes = nodes;
        node->typeSlot = typeOf(node)->slot;
    });
    
    return node;
}


Node::Ptr TypeChecker::visit(const LiteralExpr::Ptr& node) {
    switch(node->literal->kind) {
        case TokenKind::decimalLiteral:
            node->typeSlot = compiler->getType(ValueType::Int)->slot;
            break;
        case TokenKind::stringLiteral:
            node->typeSlot = compiler->getType(ValueType::String)->slot;
            break;
        case TokenKind::booleanLiteral:
            node->typeSlot = compiler->getType(ValueType::Bool)->slot;
            break;
        default:
            assert(false);
    }
    return node;
}

Node::Ptr TypeChecker::visit(const AssignExpr::Ptr& node) {
    node->left = visit(node->left);
    node->expr = visit(node->expr);
    return node;
}

Node::Ptr TypeChecker::visit(const ParenthesizedExpr::Ptr& node) {
    node->expr = visit(node->expr);
    return node;
}

Node::Ptr TypeChecker::visit(const IfStmt::Ptr& node) {
    node->condition = visit(node->condition);
    
    context->visit(CompileStage::visitCodeBlock, node->ifCodeBlock, [this, node](){
        node->ifCodeBlock = visit(node->ifCodeBlock);
    });
    
    if(node->elseCodeBlock != nullptr) {
        context->visit(CompileStage::visitCodeBlock, node->elseCodeBlock, [this, node](){
            node->elseCodeBlock = visit(node->elseCodeBlock);
        });
    }
    
    return node;
}

Node::Ptr TypeChecker::visit(const WhileStmt::Ptr& node) {
    node->expr = visit(node->expr);
    
    context->visit(CompileStage::visitCodeBlock, node->codeBlock, [this, node]() {
        node->codeBlock = visit(node->codeBlock);
    });
    return node;
}

Node::Ptr TypeChecker::visit(const ArguCallExpr::Ptr& node) {
    node->expr = visit(node->expr);
    return node;
}

Node::Ptr TypeChecker::visit(const ClassDecl::Ptr& node) {
    context->visit(CompileStage::visitClassDecl, node->members, [this, node]() {
        node->members = std::static_pointer_cast<StmtsBlock>(visit(node->members));
    });
    return node;
}

Node::Ptr TypeChecker::visit(const SelfExpr::Ptr& node) {
    if(node->identifier != nullptr) {
        node->identifier = std::static_pointer_cast<IdentifierExpr>(visit(node->identifier));
    }
    return node;
}

Node::Ptr TypeChecker::visit(const ArrayLiteralExpr::Ptr& node) {
    node->typeSlot = compiler->getType(BuildIns::Object_Array)->slot;
    auto items = std::vector<Node::Ptr>();
    for(const auto& item: node->items) {
        items.push_back(visit(item));
    }
    node->items = items;
    return node;
}

Node::Ptr TypeChecker::visit(const DictLiteralExpr::Ptr& node) {
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

Node::Ptr TypeChecker::visit(const MemberAccessExpr::Ptr& node) {
    node->callee = visit(node->callee);

    context->visit(CompileStage::visitMemberAccess, node->member, [this, node](){

        node->member = visit(node->member);
    });
    return node;
}

Node::Ptr TypeChecker::visit(const MemberAssignExpr::Ptr& node) {
    return node;
}

Node::Ptr TypeChecker::visit(const SubscriptExpr::Ptr& node) {
    node->identifier = visit(node->identifier);
    node->indexExpr = visit(node->indexExpr);

    auto simpleName = node->identifier->getSimpleName();
    auto symbol = context->lookup(simpleName);
    assert(symbol->typeSlot == compiler->getType(BuildIns::Object_Array)->slot);

    node->typeSlot = compiler->getType(ValueType::Any)->slot;
    return node;
}

Node::Ptr TypeChecker::visit(const ArrayType::Ptr& node) {
    node->valueType = visit(node->valueType);
    node->typeSlot = compiler->getType(BuildIns::Object_Array)->slot;
    return node;
}

Node::Ptr TypeChecker::visit(const PrefixExpr::Ptr& node) {
    node->expr = visit(node->expr);
    return node;
}

Node::Ptr TypeChecker::visit(const ImportStmt::Ptr& node) {
    return node;
}

Node::Ptr TypeChecker::visit(const  BinaryExpr::Ptr& decl) {
    assert(false);
}

Node::Ptr TypeChecker::visit(const OperatorExpr::Ptr& decl) {
    return decl;
}

Type* TypeChecker::typeOf(const Node::Ptr& node) {
    switch (node->kind) {
        case SyntaxKind::identifierExpr:
            assert(node->typeSlot != -1);
            return compiler->getType(node->typeSlot);
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
            return typeOf(std::static_pointer_cast<TypeIdentifier>(node));
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

Type* TypeChecker::typeOf(const Expr::Ptr& node) {
    assert(node->binaries.empty());
    assert(node->prefix == nullptr);
    
    if(node->typeSlot != -1) {
        return compiler->getType(node->typeSlot);
    }
    
    std::stack<Type*> stack;
    for (const auto &n: node->nodes)
        if (n->kind == SyntaxKind::operatorExpr) {
            auto leftType = stack.top();
            stack.pop();
            auto rightType = stack.top();
            stack.pop();
            if (leftType->kind == rightType->kind) {
                stack.push(leftType);
            } else {
                stack.push(leftType);
            }

        } else {
            auto type = typeOf(n);
            assert(type != nullptr);
            stack.push(type);
        };

    assert(stack.size() == 1);
    return stack.top();
}

Type* TypeChecker::typeOf(const LiteralExpr::Ptr& node) {
    switch(node->literal->kind) {
        case booleanLiteral:
            return context->compiler->getType(ValueType::Bool);
        case decimalLiteral:
            return context->compiler->getType(ValueType::Int);
        case nilLiteral:
            return context->compiler->getType(ValueType::Nil);
        case stringLiteral:
            return context->compiler->getType(ValueType::String);
        default:
            assert(false);
    }
}

Type* TypeChecker::typeOf(const FuncCallExpr::Ptr& node) {
    auto funcName = node->getSimpleName();
    auto symbol = context->lookup(funcName);
    assert(symbol->flag == SymbolFlag::func);
    return compiler->getType(symbol->typeSlot);
}

Type* TypeChecker::typeOf(const MemberFuncCallExpr::Ptr& node) {
    assert(node->funcTypeSlot != -1);
    auto funcType = (Function*)( compiler->getType(node->funcTypeSlot));
    auto type = compiler->getType(funcType->returnTypeSlot);
    assert(type != nullptr);
    return type;
}

Type* TypeChecker::typeOf(const ParenthesizedExpr::Ptr& node) {
    return typeOf(node->expr);
}

Type* TypeChecker::typeOf(const SelfExpr::Ptr& node) {
    return typeOf(node->identifier);
}

Type* TypeChecker::typeOf(const Pattern::Ptr& node) {
    if(node->typeSlot == -1) {
        return compiler->getType(ValueType::Any);
    }
    return typeOf(node->typeExpr);
}

Type* TypeChecker::typeOf(const TypeIdentifier::Ptr& node) {
    assert(node->typeSlot != -1);
    return compiler->getType(node->typeSlot);
}

Type* TypeChecker::typeOf(const DictLiteralExpr::Ptr& node) {
    assert(false);
}

Type* TypeChecker::typeOf(const ArrayLiteralExpr::Ptr& node) {
    Type* previousType = nullptr;
    for(const auto& item: node->items) {
        auto type = typeOf(item);
        if(previousType != nullptr) {
            if(type->kind != previousType->kind) {
                return context->compiler->getType(ValueType::Any);
            }
        }

        previousType = type;
    }

    assert(false);
    return previousType;
}

Type* TypeChecker::typeOf(const MemberAccessExpr::Ptr& node) {
    return typeOf(node->member);
}

Type* TypeChecker::typeOf(const SubscriptExpr::Ptr& node) {
    return typeOf(node->identifier);
}

Type* TypeChecker::typeOf(const ArrayType::Ptr& node) {
//    auto kind = typeOf(node->kind);
//    if(kind == JrPrimaryType::Int) {
//        return JrObjectArray::Type;
//    }
    
    assert(false);
}

Type* TypeChecker::typeOf(const PrefixExpr::Ptr& node) {
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

Type* TypeChecker::returnTypeOf(const StmtsBlock::Ptr& node) {
    if(node->statements.empty()) {
        return nullptr;
    }
    
    auto n = node->statements.back();
    return returnTypeOf(n);
}

Type* TypeChecker::returnTypeOf(const IfStmt::Ptr& node) {
    auto ifBlock = returnTypeOf(node->ifCodeBlock);
    if(ifBlock == nullptr || node->elseCodeBlock == nullptr) {
        return ifBlock;
    }
    
    auto elseBlock = returnTypeOf(node->elseCodeBlock);
    if(elseBlock == nullptr) {
        return nullptr; // else block don't have return statement
    }
    assert(ifBlock->kind == elseBlock->kind);
    return ifBlock;
}

Type* TypeChecker::returnTypeOf(const WhileStmt::Ptr& node) {
    return returnTypeOf(node->codeBlock);
}

Type* TypeChecker::returnTypeOf(const FuncCallExpr::Ptr& node) {
    assert(node->typeSlot !=  -1);
    return compiler->getType(node->typeSlot);
}

Type* TypeChecker::returnTypeOf(const Node::Ptr& node) {
    switch (node->kind) {
        case SyntaxKind::stmtsBlock:
            return returnTypeOf(std::static_pointer_cast<StmtsBlock>(node));
        case SyntaxKind::returnStmt: {
            auto returnStatement = std::static_pointer_cast<ReturnStmt>(node);
            if(returnStatement->expr == nullptr) {
                return context->compiler->getType(ValueType::Void);;
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
        case SyntaxKind::importStmt:
            return nullptr;
        default:
            assert(false);
    }
}
