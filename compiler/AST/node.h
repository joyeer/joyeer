#ifndef __joyeer_compiler_AST_node_h__
#define __joyeer_compiler_AST_node_h__

#include <memory>

enum SyntaxKind {
  importDecl,
  varDecl,
  letDecl,
  funcDecl,
  classDecl,

  expression,
  literalExpr,
  
  assignmentOperator,
  binaryOperator
};

struct Node {
  SyntaxKind kind;
  
protected:
  Node(SyntaxKind kind);

};


#endif
