#ifndef __joyeer_compiler_AST_node_h__
#define __joyeer_compiler_AST_node_h__

#include <memory>

enum SyntaxKind {
    importDecl,
    constantDecl,
    varDecl,
    letDecl,
    funcDecl,
    classDecl,

    codeBlock,
    forInStatement,

    postfixExpr,
    prefixExpr,
    identifierExpr,
    parenthesizedExpr,
    functionCallExpr,
  
    literalExpr,
    arrayLiteralExpr,
    dictLiteralExpr,
    
    assignmentOperator,
    binaryOperator
};

struct Node {
  SyntaxKind kind;
  
protected:
  Node(SyntaxKind kind);

};


#endif
