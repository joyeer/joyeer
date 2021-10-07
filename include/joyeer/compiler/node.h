#ifndef __joyeer_compiler_AST_node_h__
#define __joyeer_compiler_AST_node_h__

#include "joyeer/common/descriptor.h"
#include "joyeer/compiler/token.h"
#include "joyeer/compiler/symtable.h"
#include "joyeer/runtime/runtime.h"

enum class SyntaxKind {
    filemodule = 1L,

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

enum class NodeAccessFlag: int64_t {
    _default    = 0,
    _public     = 1 << 1,
    _private    = 1 << 2,
    _static     = 1 << 3,
};

// Lambda expr for update node's parent = this
#define NODE_UPDATE_ACTION_SET_PARENT_THIS(node) [this]() { node->parent = shared_from_this(); }
#define NODE_UPDATE_ACTION_SET_PARENT_THIS_2(node) [this, node]() { node->parent = shared_from_this(); }

// recursive update the node
#define NODE_RECURSIVE_UPDATE(node , expr) \
    if(node != nullptr) { \
        expr(); \
        node->recursiveUpdate(); \
    }


struct Node: std::enable_shared_from_this<Node> {
    using Ptr = std::shared_ptr<Node>;
    using VID = int;
    
    SyntaxKind kind;
    Symbol::Ptr symbol = nullptr;
    SymbolTable::Ptr symtable = nullptr;
    Node::Ptr parent = nullptr;
    
    JrType* type = nullptr;
    
    // return the name of Node, it will be used as symbol in some cases
    virtual std::string getSimpleName();
    
    // return the ClassDecl representing the node in which it was declared
    Node::Ptr getDeclaringClassDecl() const {
        Node::Ptr current = parent;
        while(current != nullptr) {
            if(current->kind == SyntaxKind::classDecl || current->kind == SyntaxKind::filemodule) {
                return current;
            }
            current = current->parent;
        }
        return nullptr;
    }
    
    // detect if its a declaration node
    bool isDeclNode() const {
        switch(kind) {
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

struct IdentifierExpr: public Node {
    using Ptr = std::shared_ptr<IdentifierExpr>;
    
    Token::Ptr token;
    
    explicit IdentifierExpr(Token::Ptr token);
    
    // Return identifier's name
    std::string getSimpleName() override;
    void recursiveUpdate() override { /* leave empty */ }
};

struct OperatorExpr: Node {
    using Ptr = std::shared_ptr<OperatorExpr>;
    
    Token::Ptr token;
    OperatorPriority priority;
    JrType* leftType;
    JrType* rightType;
    explicit OperatorExpr(Token::Ptr token);
    
    void recursiveUpdate() override { /* leave empty */ }
};

struct Type: Node {
    using Ptr = std::shared_ptr<Type>;
    
    Node::Ptr identifier; // identifierExpr
    
    explicit Type(Node::Ptr identifier);
    
    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(identifier, NODE_UPDATE_ACTION_SET_PARENT_THIS(identifier))
    }

};

struct ArrayType: Node {
    using Ptr = std::shared_ptr<ArrayType>;
    
    Node::Ptr type;
    
    explicit ArrayType(Node::Ptr type);
    
    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(type, NODE_UPDATE_ACTION_SET_PARENT_THIS(type))
    }
};

struct DictType: Node {
    using Ptr = std::shared_ptr<DictType>;
    
    Node::Ptr keyType;
    Node::Ptr valueType;
    
    DictType(Node::Ptr keyType, Node::Ptr valueType);

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(keyType, NODE_UPDATE_ACTION_SET_PARENT_THIS(keyType))
        NODE_RECURSIVE_UPDATE(valueType, NODE_UPDATE_ACTION_SET_PARENT_THIS(valueType))
    }
};

struct Pattern: public Node {
    using Ptr = std::shared_ptr<Pattern>;
    
    IdentifierExpr::Ptr identifier; // the name of pattern
    Node::Ptr type; // the type of pattern, optinal nullptr
    
    Pattern(IdentifierExpr::Ptr identifier, Node::Ptr type);
    
    const std::string& getIdentifierName();
    
    std::string getSimpleName() override {
        return getIdentifierName();
    }
    
    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(identifier, NODE_UPDATE_ACTION_SET_PARENT_THIS(identifier))
        NODE_RECURSIVE_UPDATE(type, NODE_UPDATE_ACTION_SET_PARENT_THIS(type))
    }

};

struct ParameterClause: Node {
    using Ptr = std::shared_ptr<ParameterClause>;
    
    std::vector<Pattern::Ptr> parameters;
    
    explicit ParameterClause(std::vector<Pattern::Ptr> parameters);
    
    void recursiveUpdate() override {
        for(auto& param: parameters) {
            NODE_RECURSIVE_UPDATE(param, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(param))
        }
    }
};


#endif
    
