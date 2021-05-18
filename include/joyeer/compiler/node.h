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
    constructorDecl,
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
    moduleMemberAccessExpr,     // access module's member variable
    moduleFuncCallExpr,         // access module's exported function
    subscriptExpr,
  
    literalExpr,
    arrayLiteralExpr,
    dictLiteralExpr,
    operatorExpr
    
};

struct Node {
    typedef std::shared_ptr<Node> Ptr;
    
    SyntaxKind kind;

    Symbol::Ptr symbol = nullptr;
    SymbolTable::Ptr symtable = nullptr;
    
    JrType* type = nullptr;
    
    virtual std::string getName();
    
    // The type name of the Node
    virtual std::string getTypeName();
    
    // detect if its a declaration node
    bool isDeclNode() {
        switch(kind) {
            case SyntaxKind::varDecl:
            case SyntaxKind::funcDecl:
            case SyntaxKind::constructorDecl:
            case SyntaxKind::classDecl:
                return true;
            default:
                return false;
        }
    }
    
protected:
    Node(SyntaxKind kind);

};

struct IdentifierExpr: public Node {
    typedef std::shared_ptr<IdentifierExpr> Ptr;
    
    Token::Ptr token;
    
    IdentifierExpr(Token::Ptr token);
    
    // Return identifier's name
    virtual std::string getName();
    virtual std::string getTypeName();
};

struct OperatorExpr: Node {
    typedef std::shared_ptr<OperatorExpr> Ptr;
    
    Token::Ptr token;
    OperatorPriority priority;
    JrType* leftType;
    JrType* rightType;
    OperatorExpr(Token::Ptr token);
};

struct Type: Node {
    typedef std::shared_ptr<Type> Ptr;
    
    Node::Ptr identifier; // identifierExpr
    
    Type(Node::Ptr identifier);
};

struct ArrayType: Node {
    typedef std::shared_ptr<ArrayType> Ptr;
    
    Node::Ptr type;
    
    ArrayType(Node::Ptr type);
    
    virtual std::string getTypeName();
};

struct DictType: Node {
    typedef std::shared_ptr<DictType> Ptr;
    
    Node::Ptr keyType;
    Node::Ptr valueType;
    
    DictType(Node::Ptr keyType, Node::Ptr valueType);
    
    virtual std::string getTypeName();
};

struct Pattern: public Node {
    typedef std::shared_ptr<Pattern> Ptr;
    
    IdentifierExpr::Ptr identifier; // the name of pattern
    Node::Ptr type; // the type of pattern, optinal nullptr
    
    Pattern(IdentifierExpr::Ptr identifier, Node::Ptr type);
    
    const std::string& getIdentifierName();

};

struct ParameterClause: Node {
    typedef std::shared_ptr<ParameterClause> Ptr;
    
    std::vector<Pattern::Ptr> parameters;
    
    ParameterClause(std::vector<Pattern::Ptr> parameters);
};


#endif
