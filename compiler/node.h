#ifndef __joyeer_compiler_AST_node_h__
#define __joyeer_compiler_AST_node_h__

#include "token.h"
#include "symtable.h"

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
    
    assignmentOperator,
    binaryOperator
};

struct Node {
    typedef std::shared_ptr<Node> Pointer;
    
    SyntaxKind kind;
    
protected:
    Node(SyntaxKind kind);

};

struct Type: Node {
    typedef std::shared_ptr<Type> Pointer;
    
    std::shared_ptr<Token> identifier;
    bool isOptional;
    
    Type(std::shared_ptr<Token> identifier, bool isOptional);
};

struct Pattern: Node {
    typedef std::shared_ptr<Pattern> Pointer;
    
    std::shared_ptr<Token> identifier;
    Type::Pointer type;
    
    Pattern(std::shared_ptr<Token> identifier, Type::Pointer type);
    
};

struct ConstDecl: Node {
    typedef std::shared_ptr<ConstDecl> Pointer;
    
    std::shared_ptr<Node> pattern;
    std::shared_ptr<Node> initializer;

    ConstDecl(std::shared_ptr<Node> pattern, std::shared_ptr<Node> initializer);
};

struct VarDecl: public Node {
    typedef std::shared_ptr<VarDecl> Pointer;
    
    Pattern::Pointer pattern;
    std::shared_ptr<Node> initializer;

    VarDecl(Pattern::Pointer pattern, std::shared_ptr<Node> initializer);
};

struct ClassDecl: Node {
    typedef std::shared_ptr<ClassDecl> Pointer;
    
    std::shared_ptr<Token> name;
    std::vector<std::shared_ptr<Node>> members;

    ClassDecl(std::shared_ptr<Token> name, std::vector<std::shared_ptr<Node>> members);
};

struct ParameterClause: Node {
    std::vector<std::shared_ptr<Node>> parameters;
    
    ParameterClause(std::vector<std::shared_ptr<Node>> parameters);
};

struct FuncDecl: Node {
    std::shared_ptr<Token> name;
    std::shared_ptr<Node> parameterClause;
    std::shared_ptr<Node> codeBlock;

    FuncDecl(std::shared_ptr<Token> name, std::shared_ptr<Node> parameterClause, std::shared_ptr<Node> codeBlock);
};

struct ConstructorDecl: Node {
    typedef std::shared_ptr<ConstructorDecl> Pointer;
    
    std::shared_ptr<Node> parameterClause;
    std::shared_ptr<Node> codeBlock;
    
    ConstructorDecl(std::shared_ptr<Node> parameterClause, std::shared_ptr<Node> codeBlock);
};

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

struct FuncCallExpr: Node {
    typedef std::shared_ptr<FuncCallExpr> Pointer;
    
    Token::Pointer identifier;
    std::vector<std::shared_ptr<Node>> parameters;
    
    // Function's symbol
    Symbol::Pointer symbol;
    
    FuncCallExpr(std::shared_ptr<Token> identifier, std::vector<std::shared_ptr<Node>> parameters);
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


struct SourceBlock: Node {
    typedef std::shared_ptr<SourceBlock> Pointer;
    std::vector<std::shared_ptr<Node>> statements;

    SourceBlock(std::vector<std::shared_ptr<Node>> statements);
};

struct CodeBlock: Node {
    typedef std::shared_ptr<CodeBlock> Pointer;
    
    std::vector<std::shared_ptr<Node>> statements;

    CodeBlock(std::vector<std::shared_ptr<Node>> statements);

};

struct ForInStatement: Node {
    typedef std::shared_ptr<ForInStatement> Pointer;
    
    std::shared_ptr<Node> pattern;
    std::shared_ptr<Node> inExpr;
    std::shared_ptr<Node> codeBlock;

    ForInStatement(std::shared_ptr<Node> pattern, std::shared_ptr<Node> inExpr, std::shared_ptr<Node> codeBlock);
};

struct IfStatement: Node {
    typedef std::shared_ptr<IfStatement> Pointer;
    
    std::shared_ptr<Node> condition;
    std::shared_ptr<Node> ifCodeBlock;
    std::shared_ptr<Node> elseCodeBlock;

    IfStatement(std::shared_ptr<Node> condition, std::shared_ptr<Node> ifCodeBlock, std::shared_ptr<Node> elseCodeBlock);
};



#endif
