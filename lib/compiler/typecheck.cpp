#include "joyeer/compiler/typecheck.h"
#include "joyeer/compiler/diagnostic.h"
#include "joyeer/runtime/buildin.h"
#include "joyeer/runtime/sys/array.h"
#include "joyeer/runtime/sys/map.h"
#include "joyeer/runtime/sys/string.h"
#include <cassert>

TypeChecker::TypeChecker(CompileContext::Ptr context):
context(context) {
}

Node::Ptr TypeChecker::visit(Node::Ptr node) {
    return NodeVisitor::visit(node);
}

Node::Ptr TypeChecker::visit(FileModuleDecl::Ptr node) {
    
    assert(node->symbol->flag == moduleSymbol);
    auto moduleClass = (JrModuleClass*)(Global::types[node->symbol->addressOfType]);
    assert(moduleClass->constructors.size() == 1);
    
    auto constructor = Global::functions[moduleClass->constructors.back()];
    assert(constructor != nullptr);
    
    context->entry(node->symtable);
    context->entry(moduleClass);
    context->entry(constructor);
    
    context->visit(CompileStage::visitSourceBlock, [this, node](){
        auto statements = std::vector<Node::Ptr>();
        for(auto statement: node->block->statements) {
            statements.push_back(visit(statement));
        }
        node->block->statements = statements;
    });
    
    // For Module's default consturctor
    constructor->localVars.push_back(JrVar {
        .name = Keywords::SELF,
        .type = moduleClass,
        .addressOfVariable = static_cast<int>(constructor->localVars.size())
    });

    verifyReturnStatement(node);
    context->leave(constructor);
    context->leave(moduleClass);
    context->leave(node->symtable);
    return node;
}

Node::Ptr TypeChecker::visit(FuncDecl::Ptr node) {
    context->entry(node->symtable);
    
    auto function = Global::functions[node->symbol->addressOfFunc];
    context->entry(function);
    
    context->visit(CompileStage::visitFuncParamDecl, [this, node]() {
        node->parameterClause = visit(node->parameterClause);
    });
    
    assert(function->paramTypes.size() == 0);
    if(node->returnType != nullptr) {
        node->returnType = visit(node->returnType);
        function->returnType = typeOf(node->returnType);
    }
    
    // Binding function's type
    auto parameterClause = std::static_pointer_cast<ParameterClause>(node->parameterClause);
    for(auto parameter: parameterClause->parameters) {
        auto symbol = parameter->type->symbol;
        
        assert((symbol->flag & typeSymbol) == typeSymbol);
        auto type = Global::types[symbol->addressOfType];
        function->paramTypes.push_back(type);
        
        // register function's parameter as variable in function
        auto addressOfVariable = (int)function->localVars.size();
        parameter->identifier->symbol->addressOfVariable = addressOfVariable;

        assert(type != nullptr);
        function->localVars.push_back(JrVar{
            .name = parameter->getIdentifierName(),
            .type = type,
            .addressOfVariable = addressOfVariable
        });
    }
    
    node->codeBlock = visit(node->codeBlock);
    
    // verify return statement
    verifyReturnStatement(std::static_pointer_cast<CodeBlock>(node->codeBlock));
    
    context->leave(function);
    context->leave(node->symtable);
    return node;
}

Node::Ptr TypeChecker::visit(ConstructorDecl::Ptr node) {
    auto function = Global::functions[node->symbol->addressOfFunc];
    
    auto ownerType = context->curType();
    context->entry(node->symtable);
    context->entry(function);
    
    context->visit(CompileStage::visitFuncParamDecl, [this, node]() {
        node->parameterClause = visit(node->parameterClause);
    });
    
    assert(function->paramTypes.size() == 0);
    
    // Binding function's type
    auto parameterClause = std::static_pointer_cast<ParameterClause>(node->parameterClause);
    for(auto parameter: parameterClause->parameters) {
        auto symbol = parameter->type->symbol;
        
        assert((symbol->flag & typeSymbol) == typeSymbol);
        auto type = Global::types[symbol->addressOfType];
        function->paramTypes.push_back(type);
        
        // register function's parameter as variable in function
        auto addressOfVariable = (int)function->localVars.size();
        parameter->identifier->symbol->addressOfVariable = addressOfVariable;

        assert(type != nullptr);
        function->localVars.push_back(JrVar{
            .name = parameter->getIdentifierName(),
            .type = type,
            .addressOfVariable = addressOfVariable
        });
    }
    
    // the last parameter is self type
    function->paramTypes.push_back(ownerType);
    assert(ownerType != nullptr);
    function->localVars.push_back(JrVar {
        .name = Keywords::SELF,
        .type = ownerType,
        .addressOfVariable = static_cast<int>(function->localVars.size())
    });
    node->codeBlock = visit(node->codeBlock);
    auto codeblock = std::static_pointer_cast<CodeBlock>(node->codeBlock);
    verifyReturnStatement(codeblock);
    
    context->leave(function);
    context->leave(node->symtable);
    
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
            if(key->symbol->flag == typeSymbol && key->symbol->flag == typeSymbol) {
                auto dictType = std::make_shared<DictType>(key, value);
                node->identifier = dictType;
            }
        }
    }
    
    if(node->identifier->kind == SyntaxKind::arrayLiteralExpr) {
        visit(node->identifier);
        auto arrayLiteral = std::static_pointer_cast<ArrayLiteralExpr>(node->identifier);
        if(arrayLiteral->items.size() == 1) {
            auto type = arrayLiteral->items[0];
            if(type->symbol->flag == typeSymbol) {
                auto arrayType = std::make_shared<ArrayType>(type);
                node->identifier = arrayType;
            }
        }
    }
    
    auto name = node->getTypeName();
    symbol = context->lookup(name);
    
    if(symbol == nullptr) {
        Diagnostics::reportError("[Error]Cannot find the function");
    }
    
    node->symbol = symbol;
    for(auto argument: node->arguments) {
        visit(argument);
    }
    
    return node;
}



Node::Ptr TypeChecker::visit(MemberFuncCallExpr::Ptr node) {
    visit(node->parent);
    
    auto type = Global::types[node->parent->symbol->addressOfType];
    assert(type != nullptr);
    node->parent->type = type;
    auto symtable = context->symtableOfType(type);
    node->parent->symtable = symtable;
    
    auto name = node->getTypeName();
    auto symbol = context->lookup(name);
    
    if(symbol == nullptr) {
        Diagnostics::reportError("[Error]Cannot find the function");
    }
    
    node->symbol = symbol;
    for(auto argument: node->arguments) {
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
        node->symbol->isMutable = true;
        node->symbol->addressOfType = JrType::Any->addressOfType;
    }
    
    if(node->initializer != nullptr) {
        node->initializer = visit(node->initializer);
    }
    
    // Verify the type of expression
    if(node->initializer != nullptr) {
        auto rightType = typeOf(node->initializer);
        auto leftType = typeOf(node->pattern);
        // the node doesn't use
        if(node->pattern->type == nullptr && rightType != JrType::Nil) {
            node->symbol->addressOfType = rightType->addressOfType;
        }
        auto function = context->curFunction();
    }
    
    
    auto stage = context->curStage();
    if(stage == CompileStage::visitSourceBlock || stage == CompileStage::visitClassDecl) {
        auto objectType = (JrObjectType*)(context->curType());
        auto fieldType = objectType->virtualFields[node->symbol->addressOfField];
        assert(fieldType->type == nullptr);
        fieldType->type = Global::types[node->symbol->addressOfType];
    } else {
        // declare the local variable in function
        auto function = context->curFunction();
        auto addressOfVariable = (int)function->localVars.size();
        node->symbol->addressOfVariable = addressOfVariable;
        
        assert(Global::types[node->symbol->addressOfType] != nullptr);
        
        function->localVars.push_back(JrVar {
            .name = node->pattern->getIdentifierName(),
            .type = Global::types[node->symbol->addressOfType],
            .addressOfVariable = addressOfVariable
        });
    }
    
    return node;
}

Node::Ptr TypeChecker::visit(LetDecl::Ptr node) {
    context->visit(CompileStage::visitLetDecl, [this, node]() {
        node->pattern = std::static_pointer_cast<Pattern>(visit(node->pattern));
    });
    
    if(node->pattern->type == nullptr) {
        // change the symbol to unfixed symbol
        node->symbol->flag = varSymbol;
        node->symbol->isMutable = false;
        node->symbol->addressOfType = JrType::Any->addressOfType;
    }
    
    if(node->initializer != nullptr) {
        node->initializer = visit(node->initializer);
    }
    
    // declare the local variable in function
    auto function = context->curFunction();
    auto addressOfVariable = (int)function->localVars.size();
    node->symbol->addressOfVariable = addressOfVariable;
    
    assert(Global::types[node->symbol->addressOfType] != nullptr);
    function->localVars.push_back(JrVar {
        .name = node->pattern->getIdentifierName(),
        .type = Global::types[node->symbol->addressOfType],
        .addressOfVariable = addressOfVariable
    });

    return node;
}

Node::Ptr TypeChecker::visit(ParameterClause::Ptr node) {
    auto symtable = context->curSymTable();
    auto parameters = std::vector<Pattern::Ptr>();
    for(auto param: node->parameters) {
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
        if(node->type->symbol == nullptr) {
            node->type->symbol = std::shared_ptr<Symbol>(new Symbol{
                .addressOfType = type->addressOfType,
                .flag = typeSymbol
            });
        }
        node->identifier->symbol->addressOfType = type->addressOfType;
    }
    
    return node;
}

Node::Ptr TypeChecker::visit(IdentifierExpr::Ptr node) {
    auto name = node->getName();
    switch (context->curStage()) {
        case CompileStage::visitSourceBlock:
        case CompileStage::visitExpr:
        case CompileStage::visitCodeBlock:
        case CompileStage::visitMemberAccess: {
            auto symbol = context->lookup(name);
            if(symbol == nullptr) {
                Diagnostics::reportError("[Error] Cannot find variable");
            }
            assert(symbol != nullptr);
            assert(node->symbol == nullptr);
            node->symbol = symbol;
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

Node::Ptr TypeChecker::visit(Type::Ptr node) {
    auto symbol = context->lookup(node->identifier->getName());
    if(symbol == nullptr) {
        Diagnostics::reportError("[Error]Cannot find type");
    }
    node->symbol = symbol;
    
    return node;
}

Node::Ptr TypeChecker::visit(CodeBlock::Ptr node) {
    assert(node->symtable != nullptr);
    context->entry(node->symtable);
    context->visit(CompileStage::visitCodeBlock, [this, node]() {
        auto statements = std::vector<Node::Ptr>();
        for(auto statement: node->statements) {
            statements.push_back(visit(statement));
        }
        node->statements = statements;
    });
    context->leave(node->symtable);
    return node;
}

Node::Ptr TypeChecker::visit(ReturnStatement::Ptr node) {
    node->expr = visit(node->expr);
    return node;
}

Node::Ptr TypeChecker::visit(Expr::Ptr node) {
    context->visit(CompileStage::visitExpr, [this, node]() {
        auto nodes = std::vector<Node::Ptr>();
        for(auto n: node->nodes) {
            nodes.push_back(visit(n));
        }
        node->nodes = nodes;
        node->type = typeOf(node);
    });
    
    return node;
}


Node::Ptr TypeChecker::visit(LiteralExpr::Ptr node) {
    return node;
}

Node::Ptr TypeChecker::visit(AssignmentExpr::Ptr node) {
    node->left = visit(node->left);
    node->expr = visit(node->expr);
    return node;
}

Node::Ptr TypeChecker::visit(ParenthesizedExpr::Ptr node) {
    node->expr = visit(node->expr);
    return node;
}

Node::Ptr TypeChecker::visit(IfStatement::Ptr node) {
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

Node::Ptr TypeChecker::visit(WhileStatement::Ptr node) {
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
    auto type = Global::types[node->symbol->addressOfType];
    context->entry(type);
    context->visit(CompileStage::visitClassDecl, [this, node]() {
        auto members = std::vector<Node::Ptr>();
        for(auto member: node->members) {
            members.push_back(visit(member));
        }
        node->members = members;
    });
    
    context->leave(type);
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
    for(auto item: node->items) {
        items.push_back(visit(item));
    }
    node->items = items;
    return node;
}

Node::Ptr TypeChecker::visit(DictLiteralExpr::Ptr node) {
    auto items = std::vector<std::tuple<Node::Ptr, Node::Ptr>>();
    for(auto item: node->items) {
        items.push_back({
            visit(std::get<0>(item)),
            visit(std::get<1>(item))
        });
    }
    node->items = items;
    return node;
}

Node::Ptr TypeChecker::visit(MemberAccessExpr::Ptr node) {
    node->parent = visit(node->parent);
    
    auto type = Global::types[node->parent->symbol->addressOfType];
    auto symtable = context->symtableOfType(type);
    
    context->visit(CompileStage::visitMemberAccess, [this, node, symtable](){
        if(symtable != nullptr) {
            // We will push access flags's symbols
            context->entry(symtable);
        }
        
        node->member = visit(node->member);
        if(symtable != nullptr) {
            context->leave(symtable);
        }
    });
    return node;
}

Node::Ptr TypeChecker::visit(ModuleMemberAccessExpr::Ptr node) {
    assert(false);
}

Node::Ptr TypeChecker::visit(ModuleFuncCallExpr::Ptr node) {
    assert(false);
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

Node::Ptr TypeChecker::visit(FileImportStatement::Ptr node) {
    return node;
}

Node::Ptr TypeChecker::visit(BinaryExpr::Ptr decl) {
    assert(false);
}

Node::Ptr TypeChecker::visit(OperatorExpr::Ptr decl) {
    return decl;
}

JrType* TypeChecker::typeOf(Node::Ptr node) {
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

JrType* TypeChecker::typeOf(IdentifierExpr::Ptr node) {
    
    switch(node->symbol->flag) {
        case varSymbol:
            return Global::types[node->symbol->addressOfType];
        case fieldSymbol: {
            auto type = (JrObjectType*)(context->curType());
            auto field = type->virtualFields[node->symbol->addressOfField];
            return Global::types[field->addressOfField];
        }
        default:
            assert(false);
    }
}

JrType* TypeChecker::typeOf(Expr::Ptr node) {
    assert(node->binaries.size() == 0);
    assert(node->prefix == nullptr);
    
    if(node->type != nullptr) {
        return node->type;
    }
    
    std::stack<JrType*> stack;
    for(auto n: node->nodes) {
        if(n->kind == SyntaxKind::operatorExpr) {
            auto leftType = stack.top();
            stack.pop();
            auto rightType = stack.top();
            stack.pop();
            if(leftType->kind == rightType->kind) {
                stack.push(leftType);
            } else {
                stack.push(leftType);
            }
            
        } else {
            auto type = typeOf(n);
            stack.push(type);
        }
    }
    
    assert(stack.size() == 1);
    return stack.top();
}

JrType* TypeChecker::typeOf(LiteralExpr::Ptr node) {
    switch(node->literal->kind) {
        case booleanLiteral:
            return JrPrimaryType::Boolean;
        case decimalLiteral:
            return JrPrimaryType::Int;
        case nilLiteral:
            return JrPrimaryType::Nil;
        case stringLiteral:
            return JrObjectString::Type;
        default:
            assert(false);
    }
}

JrType* TypeChecker::typeOf(FuncCallExpr::Ptr node) {
    auto funcName = node->getTypeName();
    auto symbol = context->lookup(funcName);
    auto function = Global::functions[node->symbol->addressOfFunc];
    return function->returnType;
}

JrType* TypeChecker::typeOf(MemberFuncCallExpr::Ptr node) {
    auto funcName = node->getTypeName();
    auto symmbol = context->lookup(funcName);
    auto function = Global::functions[node->symbol->addressOfFunc];
    return function->returnType;
}

JrType* TypeChecker::typeOf(ParenthesizedExpr::Ptr node) {
    return typeOf(node->expr);
}

JrType* TypeChecker::typeOf(SelfExpr::Ptr node) {
    return typeOf(node->identifier);
}

JrType* TypeChecker::typeOf(Pattern::Ptr node) {
    if(node->type == nullptr) {
        return JrType::Any;
    }
    return typeOf(node->type);
}

JrType* TypeChecker::typeOf(Type::Ptr node) {
    assert(node->symbol->flag == typeSymbol);
    return Global::types[node->symbol->addressOfType];
}

JrType* TypeChecker::typeOf(DictLiteralExpr::Ptr node) {
    return JrObjectMap::Type;
}

JrType* TypeChecker::typeOf(ArrayLiteralExpr::Ptr node) {
    JrType* previousType = nullptr;
    for(auto item: node->items) {
        auto type = typeOf(item);
        if(previousType != nullptr) {
            if(type->kind != previousType->kind) {
                return JrType::Any;
            }
        }
        
        previousType = type;
    }
    
    if(previousType->kind == typeInt) {
        return JrObjectArray::Type;
    }
    assert(false);
    return previousType;
}

JrType* TypeChecker::typeOf(MemberAccessExpr::Ptr node) {
    return typeOf(node->member);
}

JrType* TypeChecker::typeOf(SubscriptExpr::Ptr node) {
    return typeOf(node->identifier);
}

JrType* TypeChecker::typeOf(ArrayType::Ptr node) {
    auto type = typeOf(node->type);
    if(type == JrPrimaryType::Int) {
        return JrObjectArray::Type;
    }
    
    assert(false);
}

JrType* TypeChecker::typeOf(PrefixExpr::Ptr node) {
    return typeOf(node->expr);
}

void TypeChecker::verifyReturnStatement(FileModuleDecl::Ptr node) {
    verifyReturnStatement(node->block->statements);
}

void TypeChecker::verifyReturnStatement(CodeBlock::Ptr node) {
    verifyReturnStatement(node->statements);
}

void TypeChecker::verifyReturnStatement(std::vector<Node::Ptr>& statements) {
    if(statements.size() == 0) {
        statements.push_back(std::make_shared<ReturnStatement>(nullptr));
    }
    auto lastStatement = statements.back();
    auto returnType = returnTypeOf(lastStatement);
    if(returnType == nullptr) {
        statements.push_back(std::make_shared<ReturnStatement>(nullptr));
    }
}

JrType* TypeChecker::returnTypeOf(CodeBlock::Ptr node) {
    if(node->statements.size() == 0) {
        return nullptr;
    }
    
    auto n = node->statements.back();
    return returnTypeOf(n);
}

JrType* TypeChecker::returnTypeOf(IfStatement::Ptr node) {
    JrType* ifBlock = returnTypeOf(node->ifCodeBlock);
    if(ifBlock == nullptr || node->elseCodeBlock == nullptr) {
        return ifBlock;
    }
    
    JrType* elseBlock = returnTypeOf(node->elseCodeBlock);
    if(elseBlock == nullptr) {
        return nullptr; // else block don't have return statement
    }
    assert(ifBlock->kind == elseBlock->kind);
    return ifBlock;
}

JrType* TypeChecker::returnTypeOf(WhileStatement::Ptr node) {
    return returnTypeOf(node->codeBlock);
}

JrType* TypeChecker::returnTypeOf(FuncCallExpr::Ptr node) {
    auto function = Global::functions[node->symbol->addressOfFunc];
    assert(function != nullptr);
    return function->returnType;
}

JrType* TypeChecker::returnTypeOf(Node::Ptr node) {
    switch (node->kind) {
        case SyntaxKind::codeBlock:
            return returnTypeOf(std::static_pointer_cast<CodeBlock>(node));
        case SyntaxKind::returnStatement: {
            auto returnStatement = std::static_pointer_cast<ReturnStatement>(node);
            if(returnStatement->expr == nullptr) {
                return JrType::Void;
            } else {
                return typeOf(returnStatement->expr);
            }
        }
        case SyntaxKind::ifStatement:
            return returnTypeOf(std::static_pointer_cast<IfStatement>(node));
        case SyntaxKind::funcCallExpr:
            return returnTypeOf(std::static_pointer_cast<FuncCallExpr>(node));
        case SyntaxKind::whileStatement:
            return returnTypeOf(std::static_pointer_cast<WhileStatement>(node));
        case SyntaxKind::assignmentExpr:
        case SyntaxKind::identifierExpr:
        case SyntaxKind::arrayLiteralExpr:
        case SyntaxKind::varDecl:
        case SyntaxKind::memberAccessExpr:
        case SyntaxKind::memberFuncCallExpr:
        case SyntaxKind::funcDecl:
        case SyntaxKind::fileimportStatement:
            return nullptr;
        default:
            assert(false);
    }
}
