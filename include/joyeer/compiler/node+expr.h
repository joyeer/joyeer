#ifndef __joyeer_compiler_node_expr_h__
#define __joyeer_compiler_node_expr_h__

#include "joyeer/compiler/node.h"


struct Expr : Node {
    using Ptr = std::shared_ptr<Expr>;
    
    Node::Ptr prefix;
    std::vector<Node::Ptr> binaries;
    
    // After binder, the 'binaries' and prefix will be merged into nodes
    std::vector<Node::Ptr> nodes;
    
    Expr(Node::Ptr prefix, std::vector<Node::Ptr> binary);
    Expr(std::vector<Node::Ptr> nodes);
    
    virtual void recursiveUpdate() {
        NODE_RECURSIVE_UPDATE(prefix, NODE_UPDATE_ACTION_SET_PARENT_THIS(prefix))
        for(auto& binary: binaries) {
            NODE_RECURSIVE_UPDATE(binary, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(binary))
        }
        for(auto& node: nodes) {
            NODE_RECURSIVE_UPDATE(node, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(node))
        }
    }
};

struct PostfixExpr: Node {
    using Ptr = std::shared_ptr<PostfixExpr>;
    
    Node::Ptr  expr;
    OperatorExpr::Ptr op;

    PostfixExpr(Node::Ptr expr, OperatorExpr::Ptr op);
    
    virtual void recursiveUpdate() {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
        NODE_RECURSIVE_UPDATE(op, NODE_UPDATE_ACTION_SET_PARENT_THIS(op))
    }
    
};

struct PrefixExpr: Node {
    using Ptr = std::shared_ptr<PrefixExpr>;
    
    OperatorExpr::Ptr op;
    Node::Ptr expr; // postfix-expression
    
    PrefixExpr(OperatorExpr::Ptr op, Node::Ptr expr);
    virtual void recursiveUpdate() {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
        NODE_RECURSIVE_UPDATE(op, NODE_UPDATE_ACTION_SET_PARENT_THIS(op))
    }
};

struct BinaryExpr: Node {
    using Ptr = std::shared_ptr<BinaryExpr>;
    
    OperatorExpr::Ptr op;
    Node::Ptr expr;
    
    BinaryExpr(OperatorExpr::Ptr op, Node::Ptr expr);
    
    virtual void recursiveUpdate() {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
        NODE_RECURSIVE_UPDATE(op, NODE_UPDATE_ACTION_SET_PARENT_THIS(op))
    }
};

struct AssignmentExpr: Node {
    using Ptr = std::shared_ptr<AssignmentExpr>;
    
    Node::Ptr expr;
    
    // After binded, the identifier will be binded
    Node::Ptr left;
    AssignmentExpr(Node::Ptr expr);
    
    virtual void recursiveUpdate() {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
        NODE_RECURSIVE_UPDATE(left, NODE_UPDATE_ACTION_SET_PARENT_THIS(left))
    }
};

struct ArguCallExpr: Node {
    using Ptr = std::shared_ptr<ArguCallExpr>;
    
    IdentifierExpr::Ptr label;
    Node::Ptr expr;
    
    ArguCallExpr(IdentifierExpr::Ptr label, Node::Ptr expr);
    
    virtual void recursiveUpdate() {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
        NODE_RECURSIVE_UPDATE(label, NODE_UPDATE_ACTION_SET_PARENT_THIS(label))
    }
};

struct FuncCallExpr: Node {
    using Ptr = std::shared_ptr<FuncCallExpr>;
    
    Node::Ptr identifier;
    std::vector<ArguCallExpr::Ptr> arguments;
    
    FuncCallExpr(Node::Ptr expr, std::vector<ArguCallExpr::Ptr> arguments);
    
    virtual std::string getTypeName();
    
    virtual void recursiveUpdate() {
        NODE_RECURSIVE_UPDATE(identifier, NODE_UPDATE_ACTION_SET_PARENT_THIS(identifier))
        for(auto& argu: arguments) {
            NODE_RECURSIVE_UPDATE(argu, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(argu))
        }
    }

};

struct MemberFuncCallExpr: Node {
    using Ptr = std::shared_ptr<MemberFuncCallExpr>;
    
    Node::Ptr callee;
    Node::Ptr member;
    
    std::vector<ArguCallExpr::Ptr> arguments;
    
    MemberFuncCallExpr(Node::Ptr callee, Node::Ptr member, std::vector<ArguCallExpr::Ptr> arguments);
    
    virtual std::string getTypeName();
    
    virtual void recursiveUpdate() {
        NODE_RECURSIVE_UPDATE(callee, NODE_UPDATE_ACTION_SET_PARENT_THIS(callee))
        NODE_RECURSIVE_UPDATE(member, NODE_UPDATE_ACTION_SET_PARENT_THIS(member))
        for(auto& argu: arguments) {
            NODE_RECURSIVE_UPDATE(argu, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(argu))
        }
    }
};

struct MemberAccessExpr: Node {
    using Ptr = std::shared_ptr<MemberAccessExpr>;
    
    Node::Ptr callee;
    Node::Ptr member;
    
    MemberAccessExpr(Node::Ptr callee, Node::Ptr member);
    
    virtual std::string getTypeName();
    virtual void recursiveUpdate() {
        NODE_RECURSIVE_UPDATE(callee, NODE_UPDATE_ACTION_SET_PARENT_THIS(callee))
        NODE_RECURSIVE_UPDATE(member, NODE_UPDATE_ACTION_SET_PARENT_THIS(member))
    }

};

struct StaticAccessExpr: Node {
    using Ptr = std::shared_ptr<StaticAccessExpr>;
};

struct LiteralExpr : Node {
    using Ptr = std::shared_ptr<LiteralExpr>;
    
    Token::Ptr literal;
    LiteralExpr(Token::Ptr literal);
    
    virtual void recursiveUpdate() {}
};

struct ArrayLiteralExpr: Node {
    using Ptr = std::shared_ptr<ArrayLiteralExpr>;
    
    std::vector<Node::Ptr> items;
    
    ArrayLiteralExpr(std::vector<Node::Ptr> items);
    
    virtual void recursiveUpdate() {
        for(auto& item: items) {
            NODE_RECURSIVE_UPDATE(item, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(item))
        }
    }

};

struct DictLiteralExpr: Node {
    using Ptr = std::shared_ptr<DictLiteralExpr>;
    
    std::vector<std::tuple<Node::Ptr, Node::Ptr>> items;
    
    DictLiteralExpr(std::vector<std::tuple<Node::Ptr, Node::Ptr>> items);
    
    virtual void recursiveUpdate() {
        for(auto& item: items) {
            auto key = std::get<0>(item);
            auto value = std::get<1>(item);
            NODE_RECURSIVE_UPDATE(key, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(key))
            NODE_RECURSIVE_UPDATE(value, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(value))
        }
    }
};

struct ParenthesizedExpr: Node {
    using Ptr = std::shared_ptr<ParenthesizedExpr>;
    
    Node::Ptr expr;
    ParenthesizedExpr(std::shared_ptr<Node> expr);
    
    virtual void recursiveUpdate() {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
    }
};

struct SelfExpr: Node {
    using Ptr = std::shared_ptr<SelfExpr>;
    
    IdentifierExpr::Ptr identifier;
    
    SelfExpr(IdentifierExpr::Ptr identifier);
    
    virtual void recursiveUpdate() {
        NODE_RECURSIVE_UPDATE(identifier, NODE_UPDATE_ACTION_SET_PARENT_THIS(identifier))
    }
};

struct SubscriptExpr: Node {
    using Ptr = std::shared_ptr<SubscriptExpr>;
    
    Node::Ptr identifier;
    Node::Ptr indexExpr;
    
    SubscriptExpr(Node::Ptr identifier, Node::Ptr indexExpr);
    
    virtual void recursiveUpdate() {
        NODE_RECURSIVE_UPDATE(identifier, NODE_UPDATE_ACTION_SET_PARENT_THIS(identifier))
        NODE_RECURSIVE_UPDATE(indexExpr, NODE_UPDATE_ACTION_SET_PARENT_THIS(indexExpr))
    }

};

#endif
