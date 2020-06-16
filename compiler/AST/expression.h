#ifndef __joyeer_compiler_AST_expression_h__
#define __joyeer_compiler_AST_expression_h__

struct Node;
#include "compiler/lexer/lexer.h"

struct Expr : Node {
protected:
    Expr(SyntaxKind kind);
};

struct PostfixExpr: Node {
    std::shared_ptr<Node>  expr;
    std::shared_ptr<Token> postfixOperator;

    PostfixExpr(std::shared_ptr<Node> expr, std::shared_ptr<Token> postfixOperator);
};

struct PrefixExpr: Node {
    std::shared_ptr<Token> prefixOperator;
    std::shared_ptr<Node> expr; // postfix-expression
    
    PrefixExpr(std::shared_ptr<Token> prefixOperator, std::shared_ptr<Node> expr);
};

struct BinaryExpr: Node {
    std::shared_ptr<Token> binaryOperator;
    std::shared_ptr<Node> expr;
    
    BinaryExpr(std::shared_ptr<Token> binaryOperator, std::shared_ptr<Node> expr);
};

struct AssignmentExpr: Node {
    std::shared_ptr<Node> expr;
    AssignmentExpr(std::shared_ptr<Node> expr);
};

struct FunctionCallExpr: Node {
    std::shared_ptr<Token> postfixOperator;
    std::vector<std::shared_ptr<Node>> parameters;

    FunctionCallExpr(std::vector<std::shared_ptr<Node>> parameters, std::shared_ptr<Token> postfixOperator);
};

struct LiteralExpr : Expr {
    std::shared_ptr<Token> literal;
    LiteralExpr(std::shared_ptr<Token> literal);
};

struct IdentifierExpr: Expr {
    std::shared_ptr<Token> identifier;
    IdentifierExpr(std::shared_ptr<Token> literal);
};

struct ParenthesizedExpr: Expr {
    std::shared_ptr<Expr> expr;
    ParenthesizedExpr(std::shared_ptr<Expr> expr);
};

struct Type: Node {
    std::shared_ptr<Token> identifier;
    bool isOptional;
    
    Type(std::shared_ptr<Token> identifier, bool isOptional);
};

#endif
