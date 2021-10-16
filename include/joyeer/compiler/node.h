#ifndef __joyeer_compiler_AST_node_h__
#define __joyeer_compiler_AST_node_h__

#include <utility>

#include "joyeer/common/descriptor.h"
#include "joyeer/compiler/token.h"
#include "joyeer/compiler/symtable.h"
#include "joyeer/runtime/runtime.h"

enum class SyntaxKind {
    fileModule = 1L,

    type,
    arrayType,
    dictType,
    pattern,

    varDecl,
    funcDecl,
    classDecl,

    parameterClause,

    stmtsBlock,
    forInStmt,
    ifStmt,
    whileStmt,
    returnStmt,
    importStmt,
    fileimportStmt,

    expr,
    assignmentExpr,
    binaryExpr,
    selfExpr,
    postfixExpr,
    prefixExpr,
    identifierExpr,
    parenthesizedExpr,
    arguCallExpr,
    funcCallExpr,
    memberFuncCallExpr,
    memberAccessExpr,
    memberAssignExpr,
    subscriptExpr,

    literalExpr,
    arrayLiteralExpr,
    dictLiteralExpr,
    operatorExpr

};

enum class NodeAccessFlag : int64_t {
    _default = 0,
    _public = 1 << 1,
    _private = 1 << 2,
    _static = 1 << 3,
};

// Lambda expr for update node's parent = this
#define NODE_UPDATE_ACTION_SET_PARENT_THIS(node) [this]() { node->parent = shared_from_this(); }
#define NODE_UPDATE_ACTION_SET_PARENT_THIS_2(node) [this, node]() { node->parent = shared_from_this(); }

// recursive update the node
#define NODE_RECURSIVE_UPDATE(node, expr) \
    if((node) != nullptr) { \
        expr(); \
        (node)->recursiveUpdate(); \
    }


struct Node : std::enable_shared_from_this<Node> {
    using Ptr = std::shared_ptr<Node>;
    using VID = int;

    SyntaxKind kind;
    Symbol::Ptr symbol = nullptr;
    SymbolTable::Ptr symtable = nullptr;
    Node::Ptr parent = nullptr;

    JrType *type = nullptr;

    // return the name of Node, it will be used as symbol in some cases
    virtual std::string getSimpleName();

    // return the ClassDecl representing the node in which it was declared
    Node::Ptr getDeclaringClassDecl() const {
        Node::Ptr current = parent;
        while (current != nullptr) {
            if (current->kind == SyntaxKind::classDecl || current->kind == SyntaxKind::fileModule) {
                return current;
            }
            current = current->parent;
        }
        return nullptr;
    }

    // detect if its a declaration node
    bool isDeclNode() const {
        switch (kind) {
            case SyntaxKind::varDecl:
            case SyntaxKind::funcDecl:
            case SyntaxKind::classDecl:
                return true;
            default:
                return false;
        }
    }

    // recursive update the children node
    virtual void recursiveUpdate() = 0;

protected:
    explicit Node(SyntaxKind kind);

    VID vid = -1;
};

struct IdentifierExpr : public Node {
    using Ptr = std::shared_ptr<IdentifierExpr>;

    Token::Ptr token;

    explicit IdentifierExpr(Token::Ptr token) :
            Node(SyntaxKind::identifierExpr),
            token(token) {
    }

    // Return identifier's name
    std::string getSimpleName() override {
        return token->rawValue;
    }

    void recursiveUpdate() override { /* leave empty */ }
};

struct OperatorExpr : Node {
    using Ptr = std::shared_ptr<OperatorExpr>;

    Token::Ptr token;
    OperatorPriority priority;
    JrType *leftType;
    JrType *rightType;

    explicit OperatorExpr(Token::Ptr token);

    void recursiveUpdate() override { /* leave empty */ }
};

struct Type : Node {
    using Ptr = std::shared_ptr<Type>;

    Node::Ptr identifier; // identifierExpr

    explicit Type(Node::Ptr identifier);

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(identifier, NODE_UPDATE_ACTION_SET_PARENT_THIS(identifier))
    }

};

struct ArrayType : Node {
    using Ptr = std::shared_ptr<ArrayType>;

    Node::Ptr type;

    explicit ArrayType(Node::Ptr type);

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(type, NODE_UPDATE_ACTION_SET_PARENT_THIS(type))
    }
};

struct DictType : Node {
    using Ptr = std::shared_ptr<DictType>;

    Node::Ptr keyType;
    Node::Ptr valueType;

    DictType(Node::Ptr keyType, Node::Ptr valueType);

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(keyType, NODE_UPDATE_ACTION_SET_PARENT_THIS(keyType))
        NODE_RECURSIVE_UPDATE(valueType, NODE_UPDATE_ACTION_SET_PARENT_THIS(valueType))
    }
};

struct Pattern : public Node {
    using Ptr = std::shared_ptr<Pattern>;

    IdentifierExpr::Ptr identifier; // the name of pattern
    Node::Ptr type; // the type of pattern, optional nullptr

    Pattern(IdentifierExpr::Ptr identifier, Node::Ptr type);

    const std::string &getIdentifierName();

    std::string getSimpleName() override {
        return getIdentifierName();
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(identifier, NODE_UPDATE_ACTION_SET_PARENT_THIS(identifier))
        NODE_RECURSIVE_UPDATE(type, NODE_UPDATE_ACTION_SET_PARENT_THIS(type))
    }

};

struct ParameterClause : Node {
    using Ptr = std::shared_ptr<ParameterClause>;

    std::vector<Pattern::Ptr> parameters;

    explicit ParameterClause(std::vector<Pattern::Ptr> parameters);

    void recursiveUpdate() override {
        for (auto &param: parameters) {
            NODE_RECURSIVE_UPDATE(param, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(param))
        }
    }
};

/********************************************************
 *  Expression Section
 *********************************************************/

struct Expr : Node {
    using Ptr = std::shared_ptr<Expr>;

    Node::Ptr prefix;
    std::vector<Node::Ptr> binaries;

    // After binder, the 'binaries' and prefix will be merged into nodes
    std::vector<Node::Ptr> nodes;

    Expr(Node::Ptr prefix, std::vector<Node::Ptr> binaries) :
            Node(SyntaxKind::expr),
            prefix(std::move(prefix)),
            binaries(std::move(binaries)) {
    }

    Expr(std::vector<Node::Ptr> nodes) :
            Node(SyntaxKind::expr),
            prefix(nullptr),
            binaries(),
            nodes(std::move(nodes)) {
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(prefix, NODE_UPDATE_ACTION_SET_PARENT_THIS(prefix))
        for (auto &binary: binaries) {
            NODE_RECURSIVE_UPDATE(binary, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(binary))
        }
        for (auto &node: nodes) {
            NODE_RECURSIVE_UPDATE(node, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(node))
        }
    }
};

struct PostfixExpr : Node {
    using Ptr = std::shared_ptr<PostfixExpr>;

    Node::Ptr expr;
    OperatorExpr::Ptr op;

    PostfixExpr(Node::Ptr expr, OperatorExpr::Ptr op) :
            Node(SyntaxKind::postfixExpr),
            expr(std::move(expr)),
            op(std::move(op)) {
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
        NODE_RECURSIVE_UPDATE(op, NODE_UPDATE_ACTION_SET_PARENT_THIS(op))
    }

};

struct PrefixExpr : Node {
    using Ptr = std::shared_ptr<PrefixExpr>;

    OperatorExpr::Ptr op;
    Node::Ptr expr; // postfix-expression

    PrefixExpr(OperatorExpr::Ptr op, Node::Ptr expr) :
            Node(SyntaxKind::prefixExpr),
            op(op),
            expr(expr) {
    }


    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
        NODE_RECURSIVE_UPDATE(op, NODE_UPDATE_ACTION_SET_PARENT_THIS(op))
    }
};

struct BinaryExpr : Node {
    using Ptr = std::shared_ptr<BinaryExpr>;

    OperatorExpr::Ptr op;
    Node::Ptr expr;

    BinaryExpr(OperatorExpr::Ptr op, Node::Ptr expr) :
            Node(SyntaxKind::binaryExpr),
            op(op),
            expr(expr) {
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
        NODE_RECURSIVE_UPDATE(op, NODE_UPDATE_ACTION_SET_PARENT_THIS(op))
    }
};

struct AssignmentExpr : Node {
    using Ptr = std::shared_ptr<AssignmentExpr>;

    Node::Ptr expr;

    // After binded, the identifier will be binded
    Node::Ptr left;

    explicit AssignmentExpr(Node::Ptr expr) :
            Node(SyntaxKind::assignmentExpr),
            expr(expr) {
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
        NODE_RECURSIVE_UPDATE(left, NODE_UPDATE_ACTION_SET_PARENT_THIS(left))
    }
};

struct ArguCallExpr : Node {
    using Ptr = std::shared_ptr<ArguCallExpr>;
    IdentifierExpr::Ptr label;
    Node::Ptr expr;

    ArguCallExpr(IdentifierExpr::Ptr label, Node::Ptr expr) :
            Node(SyntaxKind::arguCallExpr),
            label(label),
            expr(expr) {
    }


    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
        NODE_RECURSIVE_UPDATE(label, NODE_UPDATE_ACTION_SET_PARENT_THIS(label))
    }
};

// represent a func call expression
// e.g. print(message: "hello world")
struct FuncCallExpr : Node {
    using Ptr = std::shared_ptr<FuncCallExpr>;

    Node::Ptr identifier;
    std::vector<ArguCallExpr::Ptr> arguments;

    FuncCallExpr(Node::Ptr expr, std::vector<ArguCallExpr::Ptr> arguments) :
            Node(SyntaxKind::funcCallExpr),
            identifier(expr),
            arguments(arguments) {
    }


    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(identifier, NODE_UPDATE_ACTION_SET_PARENT_THIS(identifier))
        for (auto &argument: arguments) {
            NODE_RECURSIVE_UPDATE(argument, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(argument))
        }
    }

};

struct MemberFuncCallExpr : Node {
    using Ptr = std::shared_ptr<MemberFuncCallExpr>;

    Node::Ptr callee;
    Node::Ptr member;

    std::vector<ArguCallExpr::Ptr> arguments;

    MemberFuncCallExpr(Node::Ptr callee, Node::Ptr member, std::vector<ArguCallExpr::Ptr> arguments) :
            Node(SyntaxKind::memberFuncCallExpr),
            callee(callee),
            member(member),
            arguments(arguments) {
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(callee, NODE_UPDATE_ACTION_SET_PARENT_THIS(callee))
        NODE_RECURSIVE_UPDATE(member, NODE_UPDATE_ACTION_SET_PARENT_THIS(member))
        for (auto &argument: arguments) {
            NODE_RECURSIVE_UPDATE(argument, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(argument))
        }
    }
};

struct MemberAccessExpr : Node {
    using Ptr = std::shared_ptr<MemberAccessExpr>;

    Node::Ptr callee;
    Node::Ptr member;

    MemberAccessExpr(Node::Ptr callee, Node::Ptr member) :
            Node(SyntaxKind::memberAccessExpr),
            callee(callee),
            member(member) {
    }


    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(callee, NODE_UPDATE_ACTION_SET_PARENT_THIS(callee))
        NODE_RECURSIVE_UPDATE(member, NODE_UPDATE_ACTION_SET_PARENT_THIS(member))
    }

};

// assign expr to member, it's automatically generated by compiler
struct MemberAssignExpr : Node {
    using Ptr = std::shared_ptr<MemberAssignExpr>;

    Node::Ptr callee; // Member's owner class
    IdentifierExpr::Ptr member; // member self
    Node::Ptr expr; //  initializer expression

    MemberAssignExpr(Node::Ptr callee, IdentifierExpr::Ptr member, Node::Ptr expr) :
            Node(SyntaxKind::memberAssignExpr),
            callee(std::move(callee)),
            member(std::move(member)),
            expr(std::move(expr)) {}

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
        NODE_RECURSIVE_UPDATE(member, NODE_UPDATE_ACTION_SET_PARENT_THIS(member))
    }
};

struct LiteralExpr : Node {
    using Ptr = std::shared_ptr<LiteralExpr>;

    Token::Ptr literal;

    explicit LiteralExpr(Token::Ptr literal) :
            Node(SyntaxKind::literalExpr),
            literal(literal) {
    }

    void recursiveUpdate() override {
    }
};

struct ArrayLiteralExpr : Node {
    using Ptr = std::shared_ptr<ArrayLiteralExpr>;

    std::vector<Node::Ptr> items;

    explicit ArrayLiteralExpr(std::vector<Node::Ptr> items) :
            Node(SyntaxKind::arrayLiteralExpr),
            items(items) {
    }


    void recursiveUpdate() override {
        for (auto &item: items) {
            NODE_RECURSIVE_UPDATE(item, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(item))
        }
    }

};

struct DictLiteralExpr : Node {
    using Ptr = std::shared_ptr<DictLiteralExpr>;

    std::vector<std::tuple<Node::Ptr, Node::Ptr>> items;

    explicit DictLiteralExpr(std::vector<std::tuple<Node::Ptr, Node::Ptr>> items) :
            Node(SyntaxKind::dictLiteralExpr),
            items(items) {
    }

    void recursiveUpdate() override {
        for (auto &item: items) {
            auto key = std::get<0>(item);
            auto value = std::get<1>(item);
            NODE_RECURSIVE_UPDATE(key, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(key))
            NODE_RECURSIVE_UPDATE(value, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(value))
        }
    }
};

struct ParenthesizedExpr : Node {
    using Ptr = std::shared_ptr<ParenthesizedExpr>;

    Node::Ptr expr;

    explicit ParenthesizedExpr(std::shared_ptr<Node> expr) :
            Node(SyntaxKind::parenthesizedExpr),
            expr(expr) {
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
    }
};

struct SelfExpr : Node {
    using Ptr = std::shared_ptr<SelfExpr>;

    IdentifierExpr::Ptr identifier;

    explicit SelfExpr(IdentifierExpr::Ptr identifier):
            Node(SyntaxKind::selfExpr),
            identifier(identifier) {
    }


    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(identifier, NODE_UPDATE_ACTION_SET_PARENT_THIS(identifier))
    }
};

struct SubscriptExpr : Node {
    using Ptr = std::shared_ptr<SubscriptExpr>;

    Node::Ptr identifier;
    Node::Ptr indexExpr;

    SubscriptExpr(Node::Ptr identifier, Node::Ptr indexExpr):
            Node(SyntaxKind::subscriptExpr),
            identifier(identifier),
            indexExpr(indexExpr) {
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(identifier, NODE_UPDATE_ACTION_SET_PARENT_THIS(identifier))
        NODE_RECURSIVE_UPDATE(indexExpr, NODE_UPDATE_ACTION_SET_PARENT_THIS(indexExpr))
    }

};

#endif
    
