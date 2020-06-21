#include "binder.h"

Binder::Binder(SymbolFactoryPtr symFactory):
symFactory(symFactory) {
    
}

void Binder::bind(std::shared_ptr<Node> node) {
    switch (node->kind) {
        case SyntaxKind::sourceFile:
            bind(std::static_pointer_cast<SourceFile>(node));
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
            break;
        case letDecl:
            break;
        case funcDecl:
            break;
        case initializerDecl:
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
            break;
        case memberExpr:
            break;
        case literalExpr:
            break;
        case arrayLiteralExpr:
            break;
        case dictLiteralExpr:
            break;
        case assignmentOperator:
            break;
        case binaryOperator:
            break;
    }
}


void Binder::bind(std::shared_ptr<SourceFile> sourceFile) {
    symFactory->createSymTable(sourceFile);
    for(auto& decl : sourceFile->decls) {
        bind(decl);
    }
}


