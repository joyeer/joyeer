#include "typecheck.h"
#include "runtime/buildin.h"
#include "diagnostic.h"
#include <cassert>

TypeChecker::TypeChecker(CompileContext::Pointer context):
context(context) {
}

void TypeChecker::verify(Node::Pointer node) {
    switch (node->kind) {
        case sourceBlock:
            verify(std::static_pointer_cast<SourceBlock>(node));
            break;
        case type:
            verify(std::static_pointer_cast<TypeDecl>(node));
            break;
        case arrayType:
            break;
        case dictType:
            break;
        case pattern:
            break;
        case importDecl:
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
            break;
        case classDecl:
            break;
        case parameterClause:
            verify(std::static_pointer_cast<ParameterClause>(node));
            break;
        case codeBlock:
            verify(std::static_pointer_cast<CodeBlock>(node));
            break;
        case forInStatement:
            break;
        case ifStatement:
            verify(std::static_pointer_cast<IfStatement>(node));
            break;
        case expr:
            verify(std::static_pointer_cast<Expr>(node));
            break;
        case selfExpr:
            break;
        case postfixExpr:
            break;
        case prefixExpr:
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
        case functionCallExpr:
            verify(std::static_pointer_cast<FuncCallExpr>(node));
            break;
        case memberExpr:
            break;
        case literalExpr:
            verify(std::static_pointer_cast<LiteralExpr>(node));
            break;
        case arrayLiteralExpr:
            break;
        case dictLiteralExpr:
            break;
        case assignmentExpr:
            verify(std::static_pointer_cast<AssignmentExpr>(node));
            break;
        case binaryExpr:
            break;
        case operatorExpr:
            break;
        case returnStatement:
            verify(std::static_pointer_cast<ReturnStatement>(node));
            break;
    }
}

void TypeChecker::verify(SourceBlock::Pointer node) {
    
    assert(node->symbol->flag == funcSymbol);
    auto function = Global::functions[node->symbol->addressOfFunc];
    assert(function != nullptr);
    context->entry(node->symtable);
    context->entry(function);
    
    context->visit(visitSourceBlock, [this, node](){
        for(auto statement: node->statements) {
            verify(statement);
        }
    });
    
    context->leave(function);
    context->leave(node->symtable);
}

void TypeChecker::verify(FuncDecl::Pointer node) {
    context->entry(node->symtable);
    
    auto function = Global::functions[node->symbol->addressOfFunc];
    context->entry(function);
    
    context->visit(visitFuncParamDecl, [this, node]() {
        auto parameterClause = std::static_pointer_cast<ParameterClause>(node->parameterClause);
        verify(parameterClause);
        
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

        function->localVars.push_back(JrVar{
            .type = type,
            .index = addressOfVariable
        });
        
    }
    
    
    
    verify(node->codeBlock);
    context->leave(function);
    context->leave(node->symtable);
}

void TypeChecker::verify(FuncCallExpr::Pointer node) {
    auto name = node->getFunctionName();
    
    auto symbol = context->lookup(name);
    if(symbol == nullptr) {
        Diagnostics::reportError(L"[Error]Cannot find the function");
    }
    
    node->symbol = symbol;
    
    for(auto argument: node->arguments) {
        verify(argument);
    }
}

void TypeChecker::verify(VarDecl::Pointer node) {
    context->visit(visitVarDecl, [this, node]() {
        verify(node->pattern);
    });
    
    if(node->pattern->type == nullptr) {
        // change the symbol to unfixed symbol
        node->symbol->flag = unfixedMutableVarSymbol;
    }
    
    if(node->initializer != nullptr) {
        verify(node->initializer);
    }
    
    // Verify the type of expression
    if(node->initializer != nullptr) {
        auto rightType = typeOf(node->initializer);
    }
    
    // declare the local variable in function
    auto function = context->curFunction();
    auto addressOfVariable = (int)function->localVars.size();
    node->symbol->addressOfVariable = addressOfVariable;
    function->localVars.push_back(JrVar {
        .type = Global::types[node->symbol->addressOfType],
        .index = addressOfVariable
    });
    
    
}

void TypeChecker::verify(LetDecl::Pointer node) {
    context->visit(visitLetDecl, [this, node]() {
        verify(node->pattern);
    });
    
    if(node->pattern->type == nullptr) {
        // change the symbol to unfixed symbol
        node->symbol->flag = unfixedImmutableVarSymbol;
    }
    
    if(node->initializer != nullptr) {
        verify(node->initializer);
    }
    
    // declare the local variable in function
    auto function = context->curFunction();
    auto addressOfVariable = (int)function->localVars.size();
    node->symbol->addressOfVariable = addressOfVariable;
    function->localVars.push_back(JrVar {
        .type = Global::types[node->symbol->addressOfType],
        .index = addressOfVariable
    });

}

void TypeChecker::verify(ParameterClause::Pointer node) {
    auto symtable = context->curSymTable();
    for(auto param: node->parameters) {
        verify(param);
    }
}

void TypeChecker::verify(Pattern::Pointer node) {
    verify(node->identifier);
    if(node->type != nullptr) {
        verify(node->type);
        // binding the identifier symbol's type to Pattern type's symbols' type
        node->identifier->symbol->addressOfType = node->type->symbol->addressOfType;
    }
}

void TypeChecker::verify(IdentifierExpr::Pointer node) {
    auto name = node->getName();
    switch (context->curStage()) {
        case visitSourceBlock:
        case visitExpr:
        case visitCodeBlock: {
            auto symbol = context->lookup(name);
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

void TypeChecker::verify(TypeDecl::Pointer node) {
    auto symbol = context->lookup(node->identifier->getName());
    node->symbol = symbol;
}

void TypeChecker::verify(CodeBlock::Pointer node) {
    assert(node->symbols != nullptr);
    context->entry(node->symbols);
    context->visit(visitCodeBlock, [this, node]() {
        for(auto statement: node->statements) {
            verify(statement);
        }
    });
    context->leave(node->symbols);
}

void TypeChecker::verify(ReturnStatement::Pointer node) {
    verify(node->expr);
}

void TypeChecker::verify(Expr::Pointer node) {
    context->visit(visitExpr, [this, node]() {
        for(auto n: node->nodes) {
            verify(n);
        }
    });
    
}


void TypeChecker::verify(LiteralExpr::Pointer node) {
    
}

void TypeChecker::verify(AssignmentExpr::Pointer node) {
    
    verify(node->identifier);
    verify(node->expr);
    
    auto leftType = typeOf(node->identifier);
    auto rightType = typeOf(node->expr);
    
}

void TypeChecker::verify(ParenthesizedExpr::Pointer node) {
    verify(node->expr);
}

void TypeChecker::verify(IfStatement::Pointer node) {
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

void TypeChecker::verify(ArguCallExpr::Pointer node) {
    verify(node->expr);
}

JrType::Pointer TypeChecker::typeOf(Node::Pointer node) {
    switch (node->kind) {
        case identifierExpr:
            return typeOf(std::static_pointer_cast<IdentifierExpr>(node));
        case expr:
            return typeOf(std::static_pointer_cast<Expr>(node));
        case functionCallExpr:
            return typeOf(std::static_pointer_cast<FuncCallExpr>(node));
        case literalExpr:
            return typeOf(std::static_pointer_cast<LiteralExpr>(node));
        case parenthesizedExpr:
            return typeOf(std::static_pointer_cast<ParenthesizedExpr>(node));
        default:
            assert(false);
    }
}

JrType::Pointer TypeChecker::typeOf(IdentifierExpr::Pointer node) {
    
    switch(node->symbol->flag) {
        case varSymbol:
        case mutableVarSymbol:
        case immutableVarSymbol:
        case unfixedMutableVarSymbol:
        case unfixedImmutableVarSymbol:
            return Global::types[node->symbol->addressOfType];
        default:
            assert(false);
    }
}

JrType::Pointer TypeChecker::typeOf(Expr::Pointer node) {
    assert(node->binaries.size() == 0);
    assert(node->prefix == nullptr);
    
    std::stack<JrType::Pointer> stack;
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
                Diagnostics::reportError(L"type should be same");
            }
            //TODO double check
            
        } else {
            auto type = typeOf(n);
            stack.push(type);
        }
    }
    
    assert(stack.size() == 1);
    return stack.top();
}

JrType::Pointer TypeChecker::typeOf(LiteralExpr::Pointer node) {
    switch(node->literal->kind) {
        case booleanLiteral:
            return JrPrimaryType::Boolean;
        case decimalLiteral:
            return JrPrimaryType::Int;
        default:
            assert(false);
    }
}

JrType::Pointer TypeChecker::typeOf(FuncCallExpr::Pointer node) {
    auto function = Global::functions[node->symbol->addressOfFunc];
    return function->returnType;
}

JrType::Pointer TypeChecker::typeOf(ParenthesizedExpr::Pointer node) {
    return typeOf(node->expr);
}
