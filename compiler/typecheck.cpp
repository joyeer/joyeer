#include "typecheck.h"
#include "runtime/buildin.h"
#include "diagnostic.h"

TypeChecker::TypeChecker(CompileContext::Pointer context):
context(context) {
}

void TypeChecker::verify(Node::Pointer node) {
    switch (node->kind) {
        case sourceBlock:
            verify(std::static_pointer_cast<SourceBlock>(node));
            break;
        case type:
            break;
        case arrayType:
            break;
        case dictType:
            break;
        case pattern:
            break;
        case importDecl:
            break;
        case constantDecl:
            break;
        case varDecl:
            verify(std::static_pointer_cast<VarDecl>(node));
            break;
        case letDecl:
            break;
        case funcDecl:
            verify(std::static_pointer_cast<FuncDecl>(node));
            break;
        case constructorDecl:
            break;
        case classDecl:
            break;
        case parameterClause:
            break;
        case codeBlock:
            break;
        case forInStatement:
            break;
        case ifStatement:
            break;
        case expr:
            break;
        case selfExpr:
            break;
        case postfixExpr:
            break;
        case prefixExpr:
            break;
        case identifierExpr:
            break;
        case parenthesizedExpr:
            break;
        case arguCallExpr:
            break;
        case functionCallExpr:
            verify(std::static_pointer_cast<FuncCallExpr>(node));
            break;
        case memberExpr:
            break;
        case literalExpr:
            break;
        case arrayLiteralExpr:
            break;
        case dictLiteralExpr:
            break;
        case assignmentExpr:
            break;
        case binaryExpr:
            break;
        case operatorExpr:
            break;
        case returnStatement:
            break;
    }
}

void TypeChecker::verify(SourceBlock::Pointer node) {
    context->entry(node->symtable);
    for(auto statement: node->statements) {
        verify(statement);
    }
    context->leave(node->symtable);
}

void TypeChecker::verify(FuncDecl::Pointer node) {
    context->entry(node->symtable);
    
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
    verify(node->initializer);
}
