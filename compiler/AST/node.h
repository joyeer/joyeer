#ifndef __joyeer_compiler_AST_node_h__
#define __joyeer_compiler_AST_node_h__


enum StmtKind {
  importDecl,
  varDecl,
  letDecl,
  funcDecl,
  classDecl,
  expression
};


struct Node {
  StmtKind kind;
};


#endif
