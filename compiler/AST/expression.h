#ifndef __joyeer_compiler_AST_expression_h__
#define __joyeer_compiler_AST_expression_h__

struct Node;
#include "compiler/lexer/lexer.h"

struct Expr : Node {

    std::shared_ptr<Node> prefix;
    std::shared_ptr<Node> binary;
    
    Expr(std::shared_ptr<Node> prefix, std::shared_ptr<Node> binary);
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

struct ArguCallExpr: Node {
    std::shared_ptr<Token> identifier;
    std::shared_ptr<Node> expr;
    
    ArguCallExpr(std::shared_ptr<Token> identifier, std::shared_ptr<Node> expr);
};

struct FunctionCallExpr: Node {
    std::shared_ptr<Token> identifier;
    std::vector<std::shared_ptr<Node>> parameters;

    FunctionCallExpr(std::shared_ptr<Token> identifier, std::vector<std::shared_ptr<Node>> parameters);
};

struct MemberExpr: Node {
    std::shared_ptr<Node> parent;
    std::shared_ptr<Node> member;
    
    MemberExpr(std::shared_ptr<Node> parent, std::shared_ptr<Node> member);
};

struct LiteralExpr : Node {
    std::shared_ptr<Token> literal;
    LiteralExpr(std::shared_ptr<Token> literal);
};

struct IdentifierExpr: Node {
    std::shared_ptr<Token> identifier;
    IdentifierExpr(std::shared_ptr<Token> literal);
};

struct ParenthesizedExpr: Node {
    std::shared_ptr<Node> expr;
    ParenthesizedExpr(std::shared_ptr<Node> expr);
};

struct SelfExpr: Node {
    std::shared_ptr<Token> identifier;
    
    SelfExpr(std::shared_ptr<Token> identifier);
};

struct Type: Node {
    typedef std::shared_ptr<Type> Pointer;
    
    std::shared_ptr<Token> identifier;
    bool isOptional;
    
    Type(std::shared_ptr<Token> identifier, bool isOptional);
};

#endif
