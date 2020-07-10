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
    context->entry(node->symtable);
    context->visit(visitSourceBlock, [this, node](){
        for(auto statement: node->statements) {
            verify(statement);
        }
    });
    
    context->leave(node->symtable);
}

void TypeChecker::verify(FuncDecl::Pointer node) {
    context->entry(node->symtable);
    
    auto function = Global::functions[node->symbol->addressOfFunc];
    context->visit(visitFuncParamDecl, [this, node]() {
        auto parameterClause = std::static_pointer_cast<ParameterClause>(node->parameterClause);
        verify(parameterClause);
        
    });
    
    
    assert(function->paramTypes.size() == 0);
    // Binding function's type
    auto parameterClause = std::static_pointer_cast<ParameterClause>(node->parameterClause);
    for(auto parameter: parameterClause->parameters) {
        auto symbol = parameter->type->symbol;
        
        //
        assert((symbol->flag & typeSymbol) == typeSymbol);
        function->paramTypes.push_back(Global::types[symbol->addressOfType]);
    }
    
    
    verify(node->codeBlock);
    
    context->leave(node->symtable);
}

void TypeChecker::verify(FuncCallExpr::Pointer node) {
    auto name = node->getFunctionName();
    
    auto symbol = context->lookup(name);
    if(symbol == nullptr) {
        Diagnostics::reportError(L"[Error]Cannot find the function");
    }
    
    node->symbol = symbol;
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
}

void TypeChecker::verify(ParameterClause::Pointer node) {
    auto symtable = context->curSymTable();
    for(auto param: node->parameters) {
        verify(param);
    }
    
    int i = 0;
    for(auto param: node->parameters) {
        param->identifier->symbol->index = i;
        i ++;
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
        case visitFuncParamDecl: { // visit function delcaration's parameter part

            auto symtable = context->curSymTable();

            // In current symtable, we find same name symbol, report it as error
            if(symtable->find(name) != nullptr) {
                Diagnostics::reportError(L"[Error] duplicate function variable name");
            }

            auto symbol = std::shared_ptr<Symbol>(new Symbol{
                .name = name,
                .flag = SymbolFlag::immutableVarSymbol
            });

            symtable->insert(symbol);
            node->symbol = symbol;
        }
            break;
        case visitExpr:
        case visitCodeBlock: {
            auto symbol = context->lookup(name);
            assert(symbol != nullptr);
            node->symbol = symbol;
            
        }
            break;
        case visitAssignExpr:
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

JrType::Pointer TypeChecker::typeOf(Node::Pointer node) {
    switch (node->kind) {
        case identifierExpr:
            break;
        default:
            break;
    }
}

JrType::Pointer TypeChecker::typeOf(IdentifierExpr::Pointer node) {
    switch(node->symbol->flag) {
        case varSymbol:
            break;
        default:
            break;
    }
}
