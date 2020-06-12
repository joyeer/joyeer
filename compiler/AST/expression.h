#ifndef __joyeer_compiler_AST_expression_h__
#define __joyeer_compiler_AST_expression_h__

struct Node;
#include "compiler/lexer/lexer.h"

struct Expr : Node {
    Expr(SyntaxKind kind);
};

struct LiteralExpr : Expr {
    std::shared_ptr<Token> literal;
    LiteralExpr(std::shared_ptr<Token> literal);
};



#endif
