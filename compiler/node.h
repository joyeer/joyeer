#ifndef __joyeer_compiler_AST_node_h__
#define __joyeer_compiler_AST_node_h__

#include "token.h"
#include "symtable.h"
#include "scope.h"

enum SyntaxKind {
    sourceBlock = 1L,

    type,
    arrayType,
    dictType,
    pattern,
    
    importDecl,
    constantDecl,
    varDecl,
    letDecl,
    funcDecl,
    constructorDecl,
    classDecl,
    
    parameterClause,

    codeBlock,
    forInStatement,
    ifStatement,

    expr,
    assignmentExpr,
    binaryExpr,
    selfExpr,
    postfixExpr,
    prefixExpr,
    identifierExpr,
    parenthesizedExpr,
    arguCallExpr,
    functionCallExpr,
    memberExpr,
  
    literalExpr,
    arrayLiteralExpr,
    dictLiteralExpr,
    operatorExpr
    
};

struct Node {
    typedef std::shared_ptr<Node> Pointer;
    
    SyntaxKind kind;
    
protected:
    Node(SyntaxKind kind);

};

struct IdentifierExpr: public Node {
    typedef std::shared_ptr<IdentifierExpr> Pointer;
    
    Var::Pointer   varRef;
    Token::Pointer token;
    
    IdentifierExpr(Token::Pointer token);
};

struct OperatorExpr: Node {
    typedef std::shared_ptr<OperatorExpr> Pointer;
    
    Token::Pointer token;
    OperatorExpr(Token::Pointer token);
};

struct Type: Node {
    typedef std::shared_ptr<Type> Pointer;
    
    IdentifierExpr::Pointer identifier;
    bool isOptional;
    
    Type(IdentifierExpr::Pointer identifier, bool isOptional);
};

struct Pattern: public Node {
    typedef std::shared_ptr<Pattern> Pointer;
    
    IdentifierExpr::Pointer identifier;
    Type::Pointer type;
    
    Pattern(IdentifierExpr::Pointer identifier, Type::Pointer type);

};

struct ConstDecl: Node {
    typedef std::shared_ptr<ConstDecl> Pointer;
    
    Pattern::Pointer pattern;
    Node::Pointer initializer;
    
    Var::Pointer variable;

    ConstDecl(Pattern::Pointer pattern, std::shared_ptr<Node> initializer);
};

struct VarDecl: public Node {
    typedef std::shared_ptr<VarDecl> Pointer;
    
    Pattern::Pointer pattern;
    Node::Pointer initializer;
    
    // enrich
    Var::Pointer variable;

    VarDecl(Pattern::Pointer pattern, std::shared_ptr<Node> initializer);
};

struct ClassDecl: Node {
    typedef std::shared_ptr<ClassDecl> Pointer;
    
    std::shared_ptr<Token> name;
    std::vector<std::shared_ptr<Node>> members;

    ClassDecl(std::shared_ptr<Token> name, std::vector<std::shared_ptr<Node>> members);
};

struct ParameterClause: Node {
    typedef std::shared_ptr<ParameterClause> Pointer;
    
    std::vector<Node::Pointer> parameters;
    
    ParameterClause(std::vector<Node::Pointer> parameters);
};

struct FuncDecl: Node {
    typedef std::shared_ptr<FuncDecl> Pointer;
    
    IdentifierExpr::Pointer identifier;
    Node::Pointer parameterClause;
    Node::Pointer codeBlock;

    FuncDecl(IdentifierExpr::Pointer identifier, Node::Pointer parameterClause, Node::Pointer codeBlock);
};

struct ConstructorDecl: Node {
    typedef std::shared_ptr<ConstructorDecl> Pointer;
    
    Node::Pointer parameterClause;
    Node::Pointer codeBlock;
    
    ConstructorDecl(Node::Pointer parameterClause, Node::Pointer codeBlock);
};

struct Expr : Node {
    typedef std::shared_ptr<Expr> Pointer;
    
    Node::Pointer prefix;
    std::vector<Node::Pointer> binaries;
    
    Expr(Node::Pointer prefix, std::vector<Node::Pointer> binary);
};

struct PostfixExpr: Node {
    typedef std::shared_ptr<PostfixExpr> Pointer;
    
    Node::Pointer  expr;
    OperatorExpr::Pointer op;

    PostfixExpr(Node::Pointer expr, OperatorExpr::Pointer op);
};

struct PrefixExpr: Node {
    typedef std::shared_ptr<PrefixExpr> Pointer;
    
    OperatorExpr::Pointer op;
    Node::Pointer expr; // postfix-expression
    
    PrefixExpr(OperatorExpr::Pointer op, Node::Pointer expr);
};

struct BinaryExpr: Node {
    typedef std::shared_ptr<BinaryExpr> Pointer;
    
    std::shared_ptr<Token> binaryOperator;
    std::shared_ptr<Node> expr;
    
    BinaryExpr(std::shared_ptr<Token> binaryOperator, std::shared_ptr<Node> expr);
};

struct AssignmentExpr: Node {
    typedef std::shared_ptr<AssignmentExpr> Pointer;
    
    IdentifierExpr::Pointer identifier;
    Node::Pointer expr;
    AssignmentExpr(std::shared_ptr<Node> expr);
};

struct ArguCallExpr: Node {
    typedef std::shared_ptr<ArguCallExpr> Pointer;
    
    IdentifierExpr::Pointer label;
    Node::Pointer expr;
    
    ArguCallExpr(IdentifierExpr::Pointer label, Node::Pointer expr);
};

struct FuncCallExpr: Node {
    typedef std::shared_ptr<FuncCallExpr> Pointer;
    
    IdentifierExpr::Pointer identifier;
    std::vector<ArguCallExpr::Pointer> arguments;
    
    // Function's symbol
    Symbol::Pointer symbol;
    
    FuncCallExpr(IdentifierExpr::Pointer identifier, std::vector<ArguCallExpr::Pointer> arguments);
};

struct MemberExpr: Node {
    typedef std::shared_ptr<MemberExpr> Pointer;
    
    std::shared_ptr<Node> parent;
    std::shared_ptr<Node> member;
    
    MemberExpr(std::shared_ptr<Node> parent, std::shared_ptr<Node> member);
};

struct LiteralExpr : Node {
    typedef std::shared_ptr<LiteralExpr> Pointer;
    
    Token::Pointer literal;
    LiteralExpr(Token::Pointer literal);
};


struct ParenthesizedExpr: Node {
    typedef std::shared_ptr<ParenthesizedExpr> Pointer;
    
    std::shared_ptr<Node> expr;
    ParenthesizedExpr(std::shared_ptr<Node> expr);
};

struct SelfExpr: Node {
    typedef std::shared_ptr<SelfExpr> Pointer;
    
    IdentifierExpr::Pointer identifier;
    
    SelfExpr(IdentifierExpr::Pointer identifier);
};

struct SourceBlock: Node {
    typedef std::shared_ptr<SourceBlock> Pointer;
    
    std::vector<Node::Pointer> statements;
    
    SymTable::Pointer symbols;
    Scope::Pointer scope;
    
    SourceBlock(std::vector<Node::Pointer> statements);
};

struct CodeBlock: Node {
    typedef std::shared_ptr<CodeBlock> Pointer;
    
    std::vector<std::shared_ptr<Node>> statements;

    CodeBlock(std::vector<std::shared_ptr<Node>> statements);

};

struct ForInStatement: Node {
    typedef std::shared_ptr<ForInStatement> Pointer;
    
    Node::Pointer pattern;
    std::shared_ptr<Node> inExpr;
    std::shared_ptr<Node> codeBlock;

    ForInStatement(std::shared_ptr<Node> pattern, std::shared_ptr<Node> inExpr, std::shared_ptr<Node> codeBlock);
};

struct IfStatement: Node {
    typedef std::shared_ptr<IfStatement> Pointer;
    
    Node::Pointer condition;
    Node::Pointer ifCodeBlock;
    Node::Pointer elseCodeBlock;

    IfStatement(std::shared_ptr<Node> condition, std::shared_ptr<Node> ifCodeBlock, std::shared_ptr<Node> elseCodeBlock);
};


struct NodeDebugPrinter {
    void print(Node::Pointer node);
    void print(std::vector<Node::Pointer> nodes);
    void printTab();
    void incTab();
    void decTab();
    
    int size = 0;
};

#endif
