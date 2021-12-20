#include "joyeer/compiler/typecheck.h"
#include "joyeer/common/diagnostic.h"
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

        // visit parameters decls
        context->visit(CompileStage::visitFuncParamDecl, decl->parameterClause, [this, decl]() {
            decl->parameterClause = visit(decl->parameterClause);
        });

        assert(funcType->paramTypes.empty());
        if(decl->returnType != nullptr) {
            decl->returnType = visit(decl->returnType);
            funcType->returnType = typeOf(decl->returnType);
        }

        // Binding function's kind
        auto parameterClause = std::static_pointer_cast<ParameterClause>(decl->parameterClause);
        for(const auto& parameter: parameterClause->parameters) {
            funcType->paramTypes.push_back(parameter->type);
        }


        decl->codeBlock = visit(decl->codeBlock);

        // verify return statement
        verifyReturnStatement(std::static_pointer_cast<StmtsBlock>(decl->codeBlock));
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
    
//    node->symbol = symbol;
    for(const auto& argument: node->arguments) {
        visit(argument);
    }
    
    return node;
}



Node::Ptr TypeChecker::visit(const MemberFuncCallExpr::Ptr& node) {
    visit(node->callee);
    
    auto type = node->callee->type;
    assert(type != nullptr);
    node->callee->type = type;

    auto name = node->getSimpleName();
    auto symbol = context->lookup(name);
    
    if(symbol == nullptr) {
        Diagnostics::reportError("[Error]Cannot find the function");
    }

    for(const auto& argument: node->arguments) {
        visit(argument);
    }
    
    return node;
}

Node::Ptr TypeChecker::visit(const VarDecl::Ptr& decl) {

    auto symtable = context->curSymTable();
    auto symbol = context->lookup(decl->getSimpleName());

    assert(decl->getType() == nullptr);
    if(decl->initializer != nullptr) {
        decl->initializer = visit(decl->initializer);
    }

    context->visit(CompileStage::visitVarDecl, decl, [this, decl]() {
        visit(decl->pattern);
    });

    // if the pattern specify the Types, VarDecl will follow the pattern
    if(decl->pattern->getType() != nullptr) {
        decl->type = decl->pattern->getType();
    } else {
        // follow the initializer expression's typedef
        decl->type = decl->initializer->getType();
    }

    assert(decl->getType() != nullptr);
    // assign the decl for VariableType;
    auto variableType = std::static_pointer_cast<VariableType>(context->compiler->getType(symbol->address));
    assert(variableType->kind == ValueType::Var);
    variableType->addressOfType = decl->getType()->address;

    return decl;
}

Node::Ptr TypeChecker::visit(const ParameterClause::Ptr& node) {
    auto symtable = context->curSymTable();
    auto parameters = std::vector<Pattern::Ptr>();
    for(const auto& param: node->parameters) {
        parameters.push_back(std::static_pointer_cast<Pattern>(visit(param)));
    }
    node->parameters = parameters;
    return node;
}

Node::Ptr TypeChecker::visit(const Pattern::Ptr& node) {
    node->identifier = std::static_pointer_cast<IdentifierExpr>(visit(node->identifier));
    if(node->typeExpr != nullptr) {
        node->typeExpr = visit(node->typeExpr);
    }

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
        }
            break;
        case CompileStage::visitAssignExpr:
        case CompileStage::visitFuncParamDecl:
        case CompileStage::visitVarDecl:
        case CompileStage::visitLetDecl:
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
        node->type = context->compiler->getType(symbol->address);
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

Node::Ptr TypeChecker::visit(const ReturnStmt::Ptr& node) {
    node->expr = visit(node->expr);
    return node;
}

Node::Ptr TypeChecker::visit(const Expr::Ptr& node) {
    context->visit(CompileStage::visitExpr, node, [this, node]() {
        auto nodes = std::vector<Node::Ptr>();
        for(const auto& n: node->nodes) {
            nodes.push_back(visit(n));
        }
        node->nodes = nodes;
        node->type = typeOf(node);
    });
    
    return node;
}


Node::Ptr TypeChecker::visit(const LiteralExpr::Ptr& node) {
    switch(node->literal->kind) {
        case TokenKind::decimalLiteral:
            node->type = context->compiler->getPrimaryType(ValueType::Int);
            break;
        case TokenKind::stringLiteral:
            node->type = context->compiler->getPrimaryType(ValueType::String);
            break;
        case TokenKind::booleanLiteral:
            node->type = context->compiler->getPrimaryType(ValueType::Bool);
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
    return node;
}

Node::Ptr TypeChecker::visit(const ArrayType::Ptr& node) {
    node->type = visit(node->type);
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

Type::Ptr TypeChecker::typeOf(const Node::Ptr& node) {
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

Type::Ptr TypeChecker::typeOf(const IdentifierExpr::Ptr& node) {

    auto symbol = context->lookup(node->getSimpleName());
    switch(symbol->flag) {
        case SymbolFlag::var:
            return node->type;
        case SymbolFlag::field: {
//            auto kind = (JrObjectType*)(context->curType());
//            auto field = kind->virtualFields[node->symbol->addressOfField];
//            return Global::types[field->addressOfField];
            assert(false);
        }
        default:
            assert(false);
    }
}

Type::Ptr TypeChecker::typeOf(const Expr::Ptr& node) {
    assert(node->binaries.empty());
    assert(node->prefix == nullptr);
    
    if(node->type != nullptr) {
        return node->type;
    }
    
    std::stack<Type::Ptr> stack;
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

Type::Ptr TypeChecker::typeOf(const LiteralExpr::Ptr& node) {
    switch(node->literal->kind) {
        case booleanLiteral:
            return context->compiler->getPrimaryType(ValueType::Bool);
        case decimalLiteral:
            return context->compiler->getPrimaryType(ValueType::Int);
        case nilLiteral:
            return context->compiler->getPrimaryType(ValueType::Nil);
        case stringLiteral:
            return context->compiler->getPrimaryType(ValueType::String);
        default:
            assert(false);
    }
}

Type::Ptr TypeChecker::typeOf(const FuncCallExpr::Ptr& node) {
    auto funcName = node->getSimpleName();
    auto symbol = context->lookup(funcName);
    assert(symbol->flag == SymbolFlag::func);
    return context->compiler->getType(symbol->address);
}

Type::Ptr TypeChecker::typeOf(const MemberFuncCallExpr::Ptr& node) {
    auto funcName = node->getSimpleName();
    auto symbol = context->lookup(funcName);
    auto funcDef = std::static_pointer_cast<FuncType> (context->compiler->getType(symbol->address));
    return funcDef->returnType;
}

Type::Ptr TypeChecker::typeOf(const ParenthesizedExpr::Ptr& node) {
    return typeOf(node->expr);
}

Type::Ptr TypeChecker::typeOf(const SelfExpr::Ptr& node) {
    return typeOf(node->identifier);
}

Type::Ptr TypeChecker::typeOf(const Pattern::Ptr& node) {
    if(node->type == nullptr) {
        return context->compiler->getPrimaryType(ValueType::Any);
    }
    return typeOf(node->typeExpr);
}

Type::Ptr TypeChecker::typeOf(const TypeIdentifier::Ptr& node) {
    return node->type;
}

Type::Ptr TypeChecker::typeOf(const DictLiteralExpr::Ptr& node) {
    assert(false);
}

Type::Ptr TypeChecker::typeOf(const ArrayLiteralExpr::Ptr& node) {
    Type::Ptr previousType = nullptr;
    for(const auto& item: node->items) {
        auto type = typeOf(item);
        if(previousType != nullptr) {
            if(type->kind != previousType->kind) {
                return context->compiler->getPrimaryType(ValueType::Any);
            }
        }

        previousType = type;
    }

    assert(false);
    return previousType;
}

Type::Ptr TypeChecker::typeOf(const MemberAccessExpr::Ptr& node) {
    return typeOf(node->member);
}

Type::Ptr TypeChecker::typeOf(const SubscriptExpr::Ptr& node) {
    return typeOf(node->identifier);
}

Type::Ptr TypeChecker::typeOf(const ArrayType::Ptr& node) {
//    auto kind = typeOf(node->kind);
//    if(kind == JrPrimaryType::Int) {
//        return JrObjectArray::Type;
//    }
    
    assert(false);
}

Type::Ptr TypeChecker::typeOf(const PrefixExpr::Ptr& node) {
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

Type::Ptr TypeChecker::returnTypeOf(const StmtsBlock::Ptr& node) {
    if(node->statements.empty()) {
        return nullptr;
    }
    
    auto n = node->statements.back();
    return returnTypeOf(n);
}

Type::Ptr TypeChecker::returnTypeOf(const IfStmt::Ptr& node) {
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

Type::Ptr TypeChecker::returnTypeOf(const WhileStmt::Ptr& node) {
    return returnTypeOf(node->codeBlock);
}

Type::Ptr TypeChecker::returnTypeOf(const FuncCallExpr::Ptr& node) {
    return node->type;
}

Type::Ptr TypeChecker::returnTypeOf(const Node::Ptr& node) {
    switch (node->kind) {
        case SyntaxKind::stmtsBlock:
            return returnTypeOf(std::static_pointer_cast<StmtsBlock>(node));
        case SyntaxKind::returnStmt: {
            auto returnStatement = std::static_pointer_cast<ReturnStmt>(node);
            if(returnStatement->expr == nullptr) {
                return context->compiler->getPrimaryType(ValueType::Void);;
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
