#ifndef __compiler_ast_decl_h__
#define __compiler_ast_decl_h__

#include "node.h"
#include "compiler/lexer/lexer.h"

struct Decl : Node {

};

struct VarDecl: Decl {
  std::shared_ptr<Token> identifier;
};

struct ConstDecl: Decl {
  std::shared_ptr<Token> identifier;

};

struct AssignmentOperator: Node {
  AssignmentOperator();
};

struct BinaryOperator: Node {
  
  std::shared_ptr<Token> op;
  BinaryOperator(std::shared_ptr<Token> operatorToken);
};

#endif
