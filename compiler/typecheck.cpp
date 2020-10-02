#include "typecheck.h"
#include "runtime/buildin.h"
#include "runtime/sys/array.h"
#include "runtime/sys/map.h"
#include "runtime/sys/string.h"
#include "diagnostic.h"
#include <cassert>

TypeChecker::TypeChecker(CompileContext::Ptr context):
context(context) {
}

void TypeChecker::verify(Node::Ptr node) {
    switch (node->kind) {
        case sourceBlock:
            verify(std::static_pointer_cast<SourceBlock>(node));
            break;
        case type:
            verify(std::static_pointer_cast<Type>(node));
            break;
        case letDecl:
            verify(std::static_pointer_cast<LetDecl>(node));
            break;
        case varDecl:
            verify(std::static_pointer_cast<VarDecl>(node));
            break;
        case funcDecl:
            verify(std::static_pointer_cast<FuncDecl>(node));
            break;
        case constructorDecl:
            verify(std::static_pointer_cast<ConstructorDecl>(node));
            break;
        case classDecl:
            verify(std::static_pointer_cast<ClassDecl>(node));
            break;
        case parameterClause:
            verify(std::static_pointer_cast<ParameterClause>(node));
            break;
        case codeBlock:
            verify(std::static_pointer_cast<CodeBlock>(node));
            break;
        case ifStatement:
            verify(std::static_pointer_cast<IfStatement>(node));
            break;
        case whileStatement:
            verify(std::static_pointer_cast<WhileStatement>(node));
            break;
        case expr:
            verify(std::static_pointer_cast<Expr>(node));
            break;
        case selfExpr:
            verify(std::static_pointer_cast<SelfExpr>(node));
            break;
        case identifierExpr:
            verify(std::static_pointer_cast<IdentifierExpr>(node));
            break;
        case parenthesizedExpr:
            verify(std::static_pointer_cast<ParenthesizedExpr>(node));
            break;
        case arguCallExpr:
            verify(std::static_pointer_cast<ArguCallExpr>(node));
            break;
        case funcCallExpr:
            verify(std::static_pointer_cast<FuncCallExpr>(node));
            break;
        case memberAccessExpr:
            verify(std::static_pointer_cast<MemberAccessExpr>(node));
            break;
        case memberFuncCallExpr:
            verify(std::static_pointer_cast<MemberFuncCallExpr>(node));
            break;
        case literalExpr:
            verify(std::static_pointer_cast<LiteralExpr>(node));
            break;
        case arrayLiteralExpr:
            verify(std::static_pointer_cast<ArrayLiteralExpr>(node));
            break;
        case dictLiteralExpr:
            verify(std::static_pointer_cast<DictLiteralExpr>(node));
            break;
        case assignmentExpr:
            verify(std::static_pointer_cast<AssignmentExpr>(node));
            break;
        case returnStatement:
            verify(std::static_pointer_cast<ReturnStatement>(node));
            break;
        case subscriptExpr:
            verify(std::static_pointer_cast<SubscriptExpr>(node));
            break;
        case arrayType:
            verify(std::static_pointer_cast<ArrayType>(node));
            break;
        case prefixExpr:
            verify(std::static_pointer_cast<PrefixExpr>(node));
            break;
        case operatorExpr:
            // ignore it
            break;
        case fileimportDecl:
            verify(std::static_pointer_cast<FileImportDecl>(node));
            break;
        default:
            assert(false);
    }
}

void TypeChecker::verify(SourceBlock::Ptr node) {
    
    assert(node->symbol->flag == moduleSymbol);
    auto moduleType = (JrModuleType*)(Global::types[node->symbol->addressOfType]);
    // 
    assert(moduleType->constructors.size() == 1);
    auto constructor = Global::functions[moduleType->constructors.back()];
    assert(constructor != nullptr);
    context->entry(node->symtable);
    context->entry(moduleType);
    context->entry(constructor);
    context->visit(visitSourceBlock, [this, node](){
        for(auto statement: node->statements) {
            verify(statement);
        }
    });
    
    // For Module's default consturctor
    constructor->localVars.push_back(JrVar {
        .name = L"self",
        .type = moduleType,
        .addressOfVariable = static_cast<int>(constructor->localVars.size())
    });

    verifyReturnStatement(node);
    context->leave(constructor);
    context->leave(moduleType);
    context->leave(node->symtable);
}

void TypeChecker::verify(FuncDecl::Ptr node) {
    context->entry(node->symtable);
    
    auto function = Global::functions[node->symbol->addressOfFunc];
    context->entry(function);
    
    context->visit(visitFuncParamDecl, [this, node]() {
        auto parameterClause = std::static_pointer_cast<ParameterClause>(node->parameterClause);
        verify(parameterClause);
        
    });
    
    assert(function->paramTypes.size() == 0);
    if(node->returnType != nullptr) {
        verify(node->returnType);
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
    verify(node->codeBlock);
    
    // verify return
    auto codeblock = std::static_pointer_cast<CodeBlock>(node->codeBlock);
    verifyReturnStatement(codeblock);
    
    context->leave(function);
    context->leave(node->symtable);
}

void TypeChecker::verify(ConstructorDecl::Ptr node) {
    auto function = Global::functions[node->symbol->addressOfFunc];
    
    auto ownerType = context->curType();
    context->entry(node->symtable);
    context->entry(function);
    
    context->visit(visitFuncParamDecl, [this, node]() {
        verify(node->parameterClause);
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
        .name = L"self",
        .type = ownerType,
        .addressOfVariable = static_cast<int>(function->localVars.size())
    });
    verify(node->codeBlock);
    auto codeblock = std::static_pointer_cast<CodeBlock>(node->codeBlock);
    verifyReturnStatement(codeblock);
    
    context->leave(function);
    context->leave(node->symtable);

}

void TypeChecker::verify(FuncCallExpr::Ptr node) {
    
    Symbol::Ptr symbol = nullptr;
    
    if(node->identifier->kind == dictLiteralExpr) {
        verify(node->identifier);
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
    
    if(node->identifier->kind == arrayLiteralExpr) {
        verify(node->identifier);
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
        Diagnostics::reportError(L"[Error]Cannot find the function");
    }
    
    node->symbol = symbol;
    for(auto argument: node->arguments) {
        verify(argument);
    }
}



void TypeChecker::verify(MemberFuncCallExpr::Ptr node) {
    verify(node->parent);
    
    auto type = Global::types[node->parent->symbol->addressOfType];
    assert(type != nullptr);
    node->parent->type = type;
    auto symtable = context->symtableOfType(type);
    node->parent->symtable = symtable;
    
    auto name = node->getTypeName();
    auto symbol = context->lookup(name);
    
    if(symbol == nullptr) {
        Diagnostics::reportError(L"[Error]Cannot find the function");
    }
    
    node->symbol = symbol;
    for(auto argument: node->arguments) {
        verify(argument);
    }
}


void TypeChecker::verify(VarDecl::Ptr node) {
    context->visit(visitVarDecl, [this, node]() {
        verify(node->pattern);
    });
    
    if(node->pattern->type == nullptr) {
        // change the symbol to unfixed symbol
        node->symbol->isMutable = true;
        node->symbol->addressOfType = JrType::Any->addressOfType;
    }
    
    if(node->initializer != nullptr) {
        verify(node->initializer);
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
    if(stage == visitSourceBlock || stage == visitClassDecl) {
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
}

void TypeChecker::verify(LetDecl::Ptr node) {
    context->visit(visitLetDecl, [this, node]() {
        verify(node->pattern);
    });
    
    if(node->pattern->type == nullptr) {
        // change the symbol to unfixed symbol
        node->symbol->flag = varSymbol;
        node->symbol->isMutable = false;
        node->symbol->addressOfType = JrType::Any->addressOfType;
    }
    
    if(node->initializer != nullptr) {
        verify(node->initializer);
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

}

void TypeChecker::verify(ParameterClause::Ptr node) {
    auto symtable = context->curSymTable();
    for(auto param: node->parameters) {
        verify(param);
    }
}

void TypeChecker::verify(Pattern::Ptr node) {
    verify(node->identifier);
    if(node->type != nullptr) {
        verify(node->type);
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
}

void TypeChecker::verify(IdentifierExpr::Ptr node) {
    auto name = node->getName();
    switch (context->curStage()) {
        case visitSourceBlock:
        case visitExpr:
        case visitCodeBlock:
        case visitMemberAccess: {
            auto symbol = context->lookup(name);
            if(symbol == nullptr) {
                Diagnostics::reportError(L"[Error] Cannot find variable");
            }
            assert(symbol != nullptr);
            assert(node->symbol == nullptr);
            node->symbol = symbol;
        }
            break;
        case visitAssignExpr:
        case visitFuncParamDecl:
        case visitVarDecl:
        case visitLetDecl:
            break;
        default:
            break;
    }
    
}

void TypeChecker::verify(Type::Ptr node) {
    auto symbol = context->lookup(node->identifier->getName());
    if(symbol == nullptr) {
        Diagnostics::reportError(L"[Error]Cannot find type");
    }
    node->symbol = symbol;
}

void TypeChecker::verify(CodeBlock::Ptr node) {
    assert(node->symtable != nullptr);
    context->entry(node->symtable);
    context->visit(visitCodeBlock, [this, node]() {
        for(auto statement: node->statements) {
            verify(statement);
        }
    });
    context->leave(node->symtable);
}

void TypeChecker::verify(ReturnStatement::Ptr node) {
    verify(node->expr);
}

void TypeChecker::verify(Expr::Ptr node) {
    context->visit(visitExpr, [this, node]() {
        for(auto n: node->nodes) {
            verify(n);
        }
        node->type = typeOf(node);
    });
}


void TypeChecker::verify(LiteralExpr::Ptr node) {
    
}

void TypeChecker::verify(AssignmentExpr::Ptr node) {
    
    verify(node->left);
    verify(node->expr);
}

void TypeChecker::verify(ParenthesizedExpr::Ptr node) {
    verify(node->expr);
}

void TypeChecker::verify(IfStatement::Ptr node) {
    verify(node->condition);
    
    context->visit(visitCodeBlock, [this, node](){
        verify(node->ifCodeBlock);
    });
    
    if(node->elseCodeBlock != nullptr) {
        context->visit(visitCodeBlock, [this, node](){
            verify(node->elseCodeBlock);
        });
    }
}

void TypeChecker::verify(WhileStatement::Ptr node) {
    verify(node->expr);
    
    context->visit(visitCodeBlock, [this, node]() {
        verify(node->codeBlock);
    });
}

void TypeChecker::verify(ArguCallExpr::Ptr node) {
    verify(node->expr);
}

void TypeChecker::verify(ClassDecl::Ptr node) {
    context->entry(node->symtable);
    auto type = Global::types[node->symbol->addressOfType];
    context->entry(type);
    context->visit(CompileStage::visitClassDecl, [this, node]() {
        for(auto member: node->members) {
            verify(member);
        }
    });
    
    context->leave(type);
    context->leave(node->symtable);
}

void TypeChecker::verify(SelfExpr::Ptr node) {
    if(node->identifier != nullptr) {
        verify(node->identifier);
    }
}

void TypeChecker::verify(ArrayLiteralExpr::Ptr node) {
    for(auto item: node->items) {
        verify(item);
    }
}

void TypeChecker::verify(DictLiteralExpr::Ptr node) {
    for(auto item: node->items) {
        verify(std::get<0>(item));
        verify(std::get<1>(item));
    }
}

void TypeChecker::verify(MemberAccessExpr::Ptr node) {
    verify(node->parent);
    
    auto type = Global::types[node->parent->symbol->addressOfType];
    auto symtable = context->symtableOfType(type);
        
    
    context->visit(visitMemberAccess, [this, node, symtable](){
        if(symtable != nullptr) {
            // We will push access flags's symbols
            context->entry(symtable);
        }
        
        verify(node->member);
        if(symtable != nullptr) {
            context->leave(symtable);
        }
    });
}

void TypeChecker::verify(SubscriptExpr::Ptr node) {
    verify(node->identifier);
    verify(node->indexExpr);
}

void TypeChecker::verify(ArrayType::Ptr node) {
    verify(node->type);
}

void TypeChecker::verify(PrefixExpr::Ptr node) {
    verify(node->expr);
}

void TypeChecker::verify(FileImportDecl::Ptr node) {    
}

JrType* TypeChecker::typeOf(Node::Ptr node) {
    switch (node->kind) {
        case identifierExpr:
            return typeOf(std::static_pointer_cast<IdentifierExpr>(node));
        case expr:
            return typeOf(std::static_pointer_cast<Expr>(node));
        case funcCallExpr:
            return typeOf(std::static_pointer_cast<FuncCallExpr>(node));
        case literalExpr:
            return typeOf(std::static_pointer_cast<LiteralExpr>(node));
        case parenthesizedExpr:
            return typeOf(std::static_pointer_cast<ParenthesizedExpr>(node));
        case selfExpr:
            return typeOf(std::static_pointer_cast<SelfExpr>(node));
        case pattern:
            return typeOf(std::static_pointer_cast<Pattern>(node));
        case type:
            return typeOf(std::static_pointer_cast<Type>(node));
        case arrayLiteralExpr:
            return typeOf(std::static_pointer_cast<ArrayLiteralExpr>(node));
        case dictLiteralExpr:
            return typeOf(std::static_pointer_cast<DictLiteralExpr>(node));
        case memberAccessExpr:
            return typeOf(std::static_pointer_cast<MemberAccessExpr>(node));
        case memberFuncCallExpr:
            return typeOf(std::static_pointer_cast<MemberFuncCallExpr>(node));
        case subscriptExpr:
            return typeOf(std::static_pointer_cast<SubscriptExpr>(node));
        case arrayType:
            return typeOf(std::static_pointer_cast<ArrayType>(node));
        case prefixExpr:
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
        if(n->kind == operatorExpr) {
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

void TypeChecker::verifyReturnStatement(SourceBlock::Ptr node) {
    verifyReturnStatement(node->statements);
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
        case codeBlock:
            return returnTypeOf(std::static_pointer_cast<CodeBlock>(node));
        case returnStatement: {
            auto returnStatement = std::static_pointer_cast<ReturnStatement>(node);
            if(returnStatement->expr == nullptr) {
                return JrType::Void;
            } else {
                return typeOf(returnStatement->expr);
            }
        }
        case ifStatement:
            return returnTypeOf(std::static_pointer_cast<IfStatement>(node));
        case funcCallExpr:
            return returnTypeOf(std::static_pointer_cast<FuncCallExpr>(node));
        case whileStatement:
            return returnTypeOf(std::static_pointer_cast<WhileStatement>(node));
        case assignmentExpr:
        case identifierExpr:
        case arrayLiteralExpr:
        case varDecl:
        case memberAccessExpr:
        case memberFuncCallExpr:
        case funcDecl:
            return nullptr;
        default:
            assert(false);
    }
}
