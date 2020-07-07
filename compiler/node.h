#ifndef __joyeer_compiler_AST_node_h__
#define __joyeer_compiler_AST_node_h__

#include "token.h"
#include "symtable.h"
#include "context.h"

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
    returnStatement,

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
    
    Token::Pointer token;
    Symbol::Pointer symbol;
    
    IdentifierExpr(Token::Pointer token);
    
    // Return identifier's name
    const std::wstring& getName();
};

struct OperatorExpr: Node {
    typedef std::shared_ptr<OperatorExpr> Pointer;
    
    Token::Pointer token;
    OperatorPriority priority;
    OperatorExpr(Token::Pointer token);
};

struct TypeDecl: Node {
    typedef std::shared_ptr<TypeDecl> Pointer;
    
    IdentifierExpr::Pointer identifier;
    bool isOptional;
    
    TypeDecl(IdentifierExpr::Pointer identifier, bool isOptional);
    
};

struct Pattern: public Node {
    typedef std::shared_ptr<Pattern> Pointer;
    
    IdentifierExpr::Pointer identifier;
    TypeDecl::Pointer type;
    
    Pattern(IdentifierExpr::Pointer identifier, TypeDecl::Pointer type);
    
    const std::wstring& getIdentifierName();

};

struct ConstDecl: Node {
    typedef std::shared_ptr<ConstDecl> Pointer;
    
    Pattern::Pointer pattern;
    Node::Pointer initializer;
    
    Symbol::Pointer symbol;

    ConstDecl(Pattern::Pointer pattern, std::shared_ptr<Node> initializer);
};

struct VarDecl: public Node {
    typedef std::shared_ptr<VarDecl> Pointer;
    
    Pattern::Pointer pattern;
    Node::Pointer initializer;
    
    // enrich
    Symbol::Pointer symbol;

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
    
    std::vector<Pattern::Pointer> parameters;
    
    ParameterClause(std::vector<Pattern::Pointer> parameters);
};

struct FuncDecl: Node {
    typedef std::shared_ptr<FuncDecl> Pointer;
    
    Node::Pointer identifier;               //IdentifierExpr
    Node::Pointer parameterClause;
    Node::Pointer codeBlock;
    Node::Pointer returnType;
    
    // Additional information
    SymbolTable::Pointer symtable;
    
    // Symbol of function declaration
    Symbol::Pointer symbol;
    
    FuncDecl(Node::Pointer identifier, Node::Pointer parameterClause, Node::Pointer returnType, Node::Pointer codeBlock);
    
    const std::wstring getFuncName();
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
    
    // After binder, the 'binaries' and prefix will be merged into nodes 
    std::vector<Node::Pointer> nodes;
    
    Expr(Node::Pointer prefix, std::vector<Node::Pointer> binary);
    Expr(std::vector<Node::Pointer> nodes);
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
    
    OperatorExpr::Pointer op;
    std::shared_ptr<Node> expr;
    
    BinaryExpr(OperatorExpr::Pointer op, std::shared_ptr<Node> expr);
};

struct AssignmentExpr: Node {
    typedef std::shared_ptr<AssignmentExpr> Pointer;
    
    Node::Pointer expr;
    
    // After binded, the identifier will be binded 
    IdentifierExpr::Pointer identifier;
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
    
    std::wstring getFunctionName();
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
    
    SymbolTable::Pointer symtable; // source symbol
    
    SourceBlock(std::vector<Node::Pointer> statements);
};

struct CodeBlock: Node {
    typedef std::shared_ptr<CodeBlock> Pointer;
    
    std::vector<Node::Pointer> statements;

    SymbolTable::Pointer symbols;
    CodeBlock(std::vector<Node::Pointer> statements);

};

struct ForInStatement: Node {
    typedef std::shared_ptr<ForInStatement> Pointer;
    
    Node::Pointer pattern;
    Node::Pointer inExpr;
    Node::Pointer codeBlock;

    ForInStatement(Node::Pointer pattern, Node::Pointer inExpr, Node::Pointer codeBlock);
};

struct IfStatement: Node {
    typedef std::shared_ptr<IfStatement> Pointer;
    
    Node::Pointer condition;
    Node::Pointer ifCodeBlock;
    Node::Pointer elseCodeBlock;

    IfStatement(Node::Pointer condition, Node::Pointer ifCodeBlock, Node::Pointer elseCodeBlock);
};

struct ReturnStatement: Node {
    typedef std::shared_ptr<ReturnStatement> Pointer;
    
    Node::Pointer expr;
    
    ReturnStatement(Node::Pointer expr);
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
