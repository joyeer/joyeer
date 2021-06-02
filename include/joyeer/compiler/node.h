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
    subscriptExpr,
  
    literalExpr,
    arrayLiteralExpr,
    dictLiteralExpr,
    operatorExpr
    
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
    
    SyntaxKind kind;
    Symbol::Ptr symbol = nullptr;
    SymbolTable::Ptr symtable = nullptr;
    Node::Ptr parent = nullptr;
    
    JrType* type = nullptr;
    
    // return the name of Node, it will be used as symbol in some cases
    virtual std::string getSimpleName();
    
    // return the ClassDecl representing the node in which it was declared
    Node::Ptr getDeclaringClassDecl() {
        Node::Ptr current = parent;
        while(current != nullptr) {
            if(current->kind == SyntaxKind::classDecl || current->kind == SyntaxKind::filemodule) {
                return current;
            }
            current = current->parent;
        }
        return nullptr;
    }
    
    // The type name of the Node
    virtual std::string getTypeName();
    
    // detect if its a declaration node
    bool isDeclNode() {
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
    Node(SyntaxKind kind);
    int vid = -1;

};

struct IdentifierExpr: public Node {
    typedef std::shared_ptr<IdentifierExpr> Ptr;
    
    Token::Ptr token;
    
    IdentifierExpr(Token::Ptr token);
    
    // Return identifier's name
    virtual std::string getSimpleName();
    virtual std::string getTypeName();
    virtual void recursiveUpdate() { /* leave empty */ }
};

struct OperatorExpr: Node {
    typedef std::shared_ptr<OperatorExpr> Ptr;
    
    Token::Ptr token;
    OperatorPriority priority;
    JrType* leftType;
    JrType* rightType;
    OperatorExpr(Token::Ptr token);
    
    virtual void recursiveUpdate() { /* leave empty */ }
};

struct Type: Node {
    typedef std::shared_ptr<Type> Ptr;
    
    Node::Ptr identifier; // identifierExpr
    
    Type(Node::Ptr identifier);
    
    virtual void recursiveUpdate() {
        NODE_RECURSIVE_UPDATE(identifier, NODE_UPDATE_ACTION_SET_PARENT_THIS(identifier))
    }

};

struct ArrayType: Node {
    typedef std::shared_ptr<ArrayType> Ptr;
    
    Node::Ptr type;
    
    ArrayType(Node::Ptr type);
    
    virtual std::string getTypeName();
    
    virtual void recursiveUpdate() {
        NODE_RECURSIVE_UPDATE(type, NODE_UPDATE_ACTION_SET_PARENT_THIS(type))
    }
};

struct DictType: Node {
    typedef std::shared_ptr<DictType> Ptr;
    
    Node::Ptr keyType;
    Node::Ptr valueType;
    
    DictType(Node::Ptr keyType, Node::Ptr valueType);
    
    virtual std::string getTypeName();
    
    virtual void recursiveUpdate() {
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
    
    virtual std::string getSimpleName() {
        return getIdentifierName();
    }
    
    virtual void recursiveUpdate() {
        NODE_RECURSIVE_UPDATE(identifier, NODE_UPDATE_ACTION_SET_PARENT_THIS(identifier))
        NODE_RECURSIVE_UPDATE(type, NODE_UPDATE_ACTION_SET_PARENT_THIS(type))
    }

};

struct ParameterClause: Node {
    using Ptr = std::shared_ptr<ParameterClause>;
    
    std::vector<Pattern::Ptr> parameters;
    
    ParameterClause(std::vector<Pattern::Ptr> parameters);
    
    virtual void recursiveUpdate() {
        for(auto& param: parameters) {
            NODE_RECURSIVE_UPDATE(param, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(param))
        }
    }
};


#endif
    
