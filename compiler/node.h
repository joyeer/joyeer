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
    
    assignmentExpr,
    binaryExpr
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
    
    Pattern::Pointer pattern;
    std::shared_ptr<Node> initializer;
    
    std::shared_ptr<Var> variable;

    ConstDecl(Pattern::Pointer pattern, std::shared_ptr<Node> initializer);
};

struct VarDecl: public Node {
    typedef std::shared_ptr<VarDecl> Pointer;
    
    Pattern::Pointer pattern;
    std::shared_ptr<Node> initializer;
    
    std::shared_ptr<Var> variable;

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
    
    std::vector<std::shared_ptr<Node>> parameters;
    
    ParameterClause(std::vector<std::shared_ptr<Node>> parameters);
};

struct FuncDecl: Node {
    typedef std::shared_ptr<FuncDecl> Pointer;
    
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
    typedef std::shared_ptr<Expr> Pointer;
    
    std::shared_ptr<Node> prefix;
    std::shared_ptr<Node> binary;
    
    Expr(std::shared_ptr<Node> prefix, std::shared_ptr<Node> binary);
};

struct PostfixExpr: Node {
    typedef std::shared_ptr<PostfixExpr> Pointer;
    
    std::shared_ptr<Node>  expr;
    std::shared_ptr<Token> postfixOperator;

    PostfixExpr(std::shared_ptr<Node> expr, std::shared_ptr<Token> postfixOperator);
};

struct PrefixExpr: Node {
    typedef std::shared_ptr<PrefixExpr> Pointer;
    
    std::shared_ptr<Token> prefixOperator;
    std::shared_ptr<Node> expr; // postfix-expression
    
    PrefixExpr(std::shared_ptr<Token> prefixOperator, std::shared_ptr<Node> expr);
};

struct BinaryExpr: Node {
    typedef std::shared_ptr<BinaryExpr> Pointer;
    
    std::shared_ptr<Token> binaryOperator;
    std::shared_ptr<Node> expr;
    
    BinaryExpr(std::shared_ptr<Token> binaryOperator, std::shared_ptr<Node> expr);
};

struct AssignmentExpr: Node {
    typedef std::shared_ptr<AssignmentExpr> Pointer;
    
    std::shared_ptr<Node> expr;
    AssignmentExpr(std::shared_ptr<Node> expr);
};

struct ArguCallExpr: Node {
    typedef std::shared_ptr<ArguCallExpr> Pointer;
    
    Token::Pointer label;
    std::shared_ptr<Node> expr;
    
    ArguCallExpr(Token::Pointer label, std::shared_ptr<Node> expr);
};

struct FuncCallExpr: Node {
    typedef std::shared_ptr<FuncCallExpr> Pointer;
    
    Token::Pointer identifier;
    std::vector<ArguCallExpr::Pointer> arguments;
    
    // Function's symbol
    Symbol::Pointer symbol;
    
    FuncCallExpr(Token::Pointer identifier, std::vector<ArguCallExpr::Pointer> arguments);
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

struct IdentifierExpr: Node {
    typedef std::shared_ptr<IdentifierExpr> Pointer;
    
    std::shared_ptr<Token> identifier;
    IdentifierExpr(std::shared_ptr<Token> literal);
};

struct ParenthesizedExpr: Node {
    typedef std::shared_ptr<ParenthesizedExpr> Pointer;
    
    std::shared_ptr<Node> expr;
    ParenthesizedExpr(std::shared_ptr<Node> expr);
};

struct SelfExpr: Node {
    typedef std::shared_ptr<SelfExpr> Pointer;
    
    std::shared_ptr<Token> identifier;
    
    SelfExpr(std::shared_ptr<Token> identifier);
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
