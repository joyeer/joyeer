#include "typecheck.h"


void TypeChecker::verify(Node::Pointer node) {
    switch (node->kind) {
        case sourceBlock:
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
