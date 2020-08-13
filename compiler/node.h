#ifndef __joyeer_compiler_AST_node_h__
#define __joyeer_compiler_AST_node_h__

#include "token.h"
#include "symtable.h"
#include "context.h"
#include <fstream>

enum SyntaxKind {
    sourceBlock = 1L,

    type,
    arrayType,
    dictType,
    pattern,
    
    importDecl,
    letDecl,
    varDecl,
    funcDecl,
    constructorDecl,
    classDecl,
    
    parameterClause,

    codeBlock,
    forInStatement,
    ifStatement,
    whileStatement,
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
    funcCallExpr,
    memberAccessExpr,
    subscriptExpr,
  
    literalExpr,
    arrayLiteralExpr,
    dictLiteralExpr,
    operatorExpr
    
};

struct Node {
    typedef std::shared_ptr<Node> Pointer;
    
    SyntaxKind kind;

    Symbol::Pointer symbol;
    SymbolTable::Pointer symtable;
    JrType* type;
    
    virtual std::wstring getName();
    
protected:
    Node(SyntaxKind kind);

};

struct IdentifierExpr: public Node {
    typedef std::shared_ptr<IdentifierExpr> Pointer;
    
    Token::Pointer token;
    
    IdentifierExpr(Token::Pointer token);
    
    // Return identifier's name
    virtual std::wstring getName();
};

struct OperatorExpr: Node {
    typedef std::shared_ptr<OperatorExpr> Pointer;
    
    Token::Pointer token;
    OperatorPriority priority;
    OperatorExpr(Token::Pointer token);
};

struct Type: Node {
    typedef std::shared_ptr<Type> Pointer;
    
    Node::Pointer identifier; // identifierExpr
    
    Type(Node::Pointer identifier);
};

struct ArrayType: Node {
    typedef std::shared_ptr<ArrayType> Pointer;
    
    Node::Pointer type;
    
    ArrayType(Node::Pointer type);
};

struct Pattern: public Node {
    typedef std::shared_ptr<Pattern> Pointer;
    
    IdentifierExpr::Pointer identifier; // the name of pattern
    Node::Pointer type; // the type of pattern, optinal nullptr
    
    Pattern(IdentifierExpr::Pointer identifier, Node::Pointer type);
    
    const std::wstring& getIdentifierName();

};

struct LetDecl: Node {
    typedef std::shared_ptr<LetDecl> Pointer;
    
    Pattern::Pointer pattern;
    Node::Pointer initializer;
    
    LetDecl(Pattern::Pointer pattern, std::shared_ptr<Node> initializer);
};

struct VarDecl: public Node {
    typedef std::shared_ptr<VarDecl> Pointer;
    
    Pattern::Pointer pattern;
    Node::Pointer initializer;
    
    VarDecl(Pattern::Pointer pattern, std::shared_ptr<Node> initializer);
};

struct ClassDecl: Node {
    typedef std::shared_ptr<ClassDecl> Pointer;
    
    Token::Pointer name;
    std::vector<Node::Pointer> members;
    
    ClassDecl(Token::Pointer name, std::vector<Node::Pointer> members);
    
    std::wstring getName();
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

    FuncDecl(Node::Pointer identifier, Node::Pointer parameterClause, Node::Pointer returnType, Node::Pointer codeBlock);
    
    const std::wstring getFuncName();
};

struct ConstructorDecl: Node {
    typedef std::shared_ptr<ConstructorDecl> Pointer;
    
    Node::Pointer parameterClause;
    Node::Pointer codeBlock;
    
    ConstructorDecl(Node::Pointer parameterClause, Node::Pointer codeBlock);
    
    // return constructor's symbol name e.g. init(...)
    const std::wstring getName(JrType* ownerType);
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
    Node::Pointer left;
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
    
    Node::Pointer identifier;
    std::vector<ArguCallExpr::Pointer> arguments;
    
    JrType* ownerType = nullptr;
    
    FuncCallExpr(Node::Pointer expr, std::vector<ArguCallExpr::Pointer> arguments);
    
    std::wstring getFunctionName();
};

struct MemberAccessExpr: Node {
    typedef std::shared_ptr<MemberAccessExpr> Pointer;
    
    Node::Pointer parent;
    Node::Pointer member;
    
    MemberAccessExpr(Node::Pointer parent, Node::Pointer member);
};

struct LiteralExpr : Node {
    typedef std::shared_ptr<LiteralExpr> Pointer;
    
    Token::Pointer literal;
    LiteralExpr(Token::Pointer literal);
};

struct ArrayLiteralExpr: Node {
    typedef std::shared_ptr<ArrayLiteralExpr> Pointer;
    
    std::vector<Node::Pointer> items;
    
    ArrayLiteralExpr(std::vector<Node::Pointer> items);
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

struct SubscriptExpr: Node {
    typedef std::shared_ptr<SubscriptExpr> Pointer;
    
    Node::Pointer identifier;
    Node::Pointer indexExpr;
    
    SubscriptExpr(Node::Pointer identifier, Node::Pointer indexExpr);
};

struct SourceBlock: Node {
    typedef std::shared_ptr<SourceBlock> Pointer;
    
    std::vector<Node::Pointer> statements;
    
    std::wstring filename;

    SourceBlock(std::vector<Node::Pointer> statements);
    
    virtual std::wstring getName();
};

struct CodeBlock: Node {
    typedef std::shared_ptr<CodeBlock> Pointer;
    
    std::vector<Node::Pointer> statements;

    CodeBlock(std::vector<Node::Pointer> statements);

};

struct ForInStatement: Node {
    typedef std::shared_ptr<ForInStatement> Pointer;
    
    Node::Pointer pattern;
    Node::Pointer inExpr;
    Node::Pointer codeBlock;

    ForInStatement(Node::Pointer pattern, Node::Pointer inExpr, Node::Pointer codeBlock);
};

struct WhileStatement: Node {
    typedef std::shared_ptr<WhileStatement> Pointer;
    
    Node::Pointer expr;
    Node::Pointer codeBlock;
    
    WhileStatement(Node::Pointer expr, Node::Pointer codeBlock);
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


// ASM node printer
struct NodeDebugPrinter {
    NodeDebugPrinter(const std::wstring filename);
    
    void print(Node::Pointer node);
    void close();
    
protected:
    void print(std::vector<Node::Pointer> nodes);
    void print(SymbolTable::Pointer symtable);
    void print(Symbol::Pointer symbol);
    void printTab();
    void incTab();
    void decTab();
    
private:
    int size = 0;
    std::wofstream output;
};

#endif
