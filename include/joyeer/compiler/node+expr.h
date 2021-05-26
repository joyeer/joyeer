#ifndef __joyeer_compiler_node_expr_h__
#define __joyeer_compiler_node_expr_h__

#include "joyeer/compiler/node.h"


struct Expr : Node {
    typedef std::shared_ptr<Expr> Ptr;
    
    Node::Ptr prefix;
    std::vector<Node::Ptr> binaries;
    
    // After binder, the 'binaries' and prefix will be merged into nodes
    std::vector<Node::Ptr> nodes;
    
    Expr(Node::Ptr prefix, std::vector<Node::Ptr> binary);
    Expr(std::vector<Node::Ptr> nodes);
};

struct PostfixExpr: Node {
    typedef std::shared_ptr<PostfixExpr> Ptr;
    
    Node::Ptr  expr;
    OperatorExpr::Ptr op;

    PostfixExpr(Node::Ptr expr, OperatorExpr::Ptr op);
};

struct PrefixExpr: Node {
    typedef std::shared_ptr<PrefixExpr> Ptr;
    
    OperatorExpr::Ptr op;
    Node::Ptr expr; // postfix-expression
    
    PrefixExpr(OperatorExpr::Ptr op, Node::Ptr expr);
};

struct BinaryExpr: Node {
    typedef std::shared_ptr<BinaryExpr> Ptr;
    
    OperatorExpr::Ptr op;
    std::shared_ptr<Node> expr;
    
    BinaryExpr(OperatorExpr::Ptr op, Node::Ptr expr);
};

struct AssignmentExpr: Node {
    typedef std::shared_ptr<AssignmentExpr> Ptr;
    
    Node::Ptr expr;
    
    // After binded, the identifier will be binded
    Node::Ptr left;
    AssignmentExpr(Node::Ptr expr);
};

struct ArguCallExpr: Node {
    typedef std::shared_ptr<ArguCallExpr> Ptr;
    
    IdentifierExpr::Ptr label;
    Node::Ptr expr;
    
    ArguCallExpr(IdentifierExpr::Ptr label, Node::Ptr expr);
};

struct FuncCallExpr: Node {
    typedef std::shared_ptr<FuncCallExpr> Ptr;
    
    Node::Ptr identifier;
    std::vector<ArguCallExpr::Ptr> arguments;
    
    FuncCallExpr(Node::Ptr expr, std::vector<ArguCallExpr::Ptr> arguments);
    
    virtual std::string getTypeName();
};

struct MemberFuncCallExpr: Node {
    typedef std::shared_ptr<MemberFuncCallExpr> Ptr;
    
    Node::Ptr callee;
    Node::Ptr member;
    
    std::vector<ArguCallExpr::Ptr> arguments;
    
    MemberFuncCallExpr(Node::Ptr callee, Node::Ptr member, std::vector<ArguCallExpr::Ptr> arguments);
    
    virtual std::string getTypeName();
};

struct MemberAccessExpr: Node {
    typedef std::shared_ptr<MemberAccessExpr> Ptr;
    
    Node::Ptr callee;
    Node::Ptr member;
    
    MemberAccessExpr(Node::Ptr callee, Node::Ptr member);
    
    virtual std::string getTypeName();
};

struct StaticAccessExpr: Node {
    using Ptr = std::shared_ptr<StaticAccessExpr>;
};

struct LiteralExpr : Node {
    typedef std::shared_ptr<LiteralExpr> Ptr;
    
    Token::Ptr literal;
    LiteralExpr(Token::Ptr literal);
};

struct ArrayLiteralExpr: Node {
    typedef std::shared_ptr<ArrayLiteralExpr> Ptr;
    
    std::vector<Node::Ptr> items;
    
    ArrayLiteralExpr(std::vector<Node::Ptr> items);
};

struct DictLiteralExpr: Node {
    typedef std::shared_ptr<DictLiteralExpr> Ptr;
    
    std::vector<std::tuple<Node::Ptr, Node::Ptr>> items;
    
    DictLiteralExpr(std::vector<std::tuple<Node::Ptr, Node::Ptr>> items);
};

struct ParenthesizedExpr: Node {
    typedef std::shared_ptr<ParenthesizedExpr> Ptr;
    
    std::shared_ptr<Node> expr;
    ParenthesizedExpr(std::shared_ptr<Node> expr);
};

struct SelfExpr: Node {
    typedef std::shared_ptr<SelfExpr> Ptr;
    
    IdentifierExpr::Ptr identifier;
    
    SelfExpr(IdentifierExpr::Ptr identifier);
};

struct SubscriptExpr: Node {
    typedef std::shared_ptr<SubscriptExpr> Ptr;
    
    Node::Ptr identifier;
    Node::Ptr indexExpr;
    
    SubscriptExpr(Node::Ptr identifier, Node::Ptr indexExpr);
};

#endif
