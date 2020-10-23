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
    fileimportDecl,
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
    
    virtual std::wstring getName();
    // The type name of the Node
    virtual std::wstring getTypeName();
    
protected:
    Node(SyntaxKind kind);

};

struct IdentifierExpr: public Node {
    typedef std::shared_ptr<IdentifierExpr> Ptr;
    
    Token::Ptr token;
    
    IdentifierExpr(Token::Ptr token);
    
    // Return identifier's name
    virtual std::wstring getName();
    virtual std::wstring getTypeName();
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
    
    virtual std::wstring getTypeName();
};

struct DictType: Node {
    typedef std::shared_ptr<DictType> Ptr;
    
    Node::Ptr keyType;
    Node::Ptr valueType;
    
    DictType(Node::Ptr keyType, Node::Ptr valueType);
    
    virtual std::wstring getTypeName();
};

struct Pattern: public Node {
    typedef std::shared_ptr<Pattern> Ptr;
    
    IdentifierExpr::Ptr identifier; // the name of pattern
    Node::Ptr type; // the type of pattern, optinal nullptr
    
    Pattern(IdentifierExpr::Ptr identifier, Node::Ptr type);
    
    const std::wstring& getIdentifierName();

};

struct LetDecl: Node {
    typedef std::shared_ptr<LetDecl> Ptr;
    
    Pattern::Ptr pattern;
    Node::Ptr initializer;
    
    LetDecl(Pattern::Ptr pattern, std::shared_ptr<Node> initializer);
};

struct VarDecl: public Node {
    typedef std::shared_ptr<VarDecl> Ptr;
    
    Pattern::Ptr pattern;
    Node::Ptr initializer;
    
    VarDecl(Pattern::Ptr pattern, std::shared_ptr<Node> initializer);
};

struct ClassDecl: Node {
    typedef std::shared_ptr<ClassDecl> Ptr;
    
    Token::Ptr name;
    std::vector<Node::Ptr> members;
    
    ClassDecl(Token::Ptr name, std::vector<Node::Ptr> members);
    
    std::wstring getName();
};

struct ParameterClause: Node {
    typedef std::shared_ptr<ParameterClause> Ptr;
    
    std::vector<Pattern::Ptr> parameters;
    
    ParameterClause(std::vector<Pattern::Ptr> parameters);
};

struct FuncDecl: Node {
    typedef std::shared_ptr<FuncDecl> Ptr;
    
    Node::Ptr identifier;               //IdentifierExpr
    Node::Ptr parameterClause;
    Node::Ptr codeBlock;
    Node::Ptr returnType;

    FuncDecl(Node::Ptr identifier, Node::Ptr parameterClause, Node::Ptr returnType, Node::Ptr codeBlock);
    
    virtual std::wstring getTypeName();
};

struct ConstructorDecl: Node {
    typedef std::shared_ptr<ConstructorDecl> Ptr;
    
    Node::Ptr parameterClause;
    Node::Ptr codeBlock;
    
    ConstructorDecl(Node::Ptr parameterClause, Node::Ptr codeBlock);
    
    // return constructor's symbol name e.g. init(...)
    const std::wstring getName(JrType* ownerType);
};

struct Expr : Node {
    typedef std::shared_ptr<Expr> Ptr;
    
    Node::Ptr prefix;
    std::vector<Node::Ptr> binaries;
    
    // After binder, the 'binaries' and prefix will be merged into nodes 
    std::vector<Node::Ptr> nodes;
    
    Expr(Node::Ptr prefix, std::vector<Node::Ptr> binary);
    Expr(std::vector<Node::Ptr> nodes);
};

struct PostfixExpr: Node {
    typedef std::shared_ptr<PostfixExpr> Ptr;
    
    Node::Ptr  expr;
    OperatorExpr::Ptr op;

    PostfixExpr(Node::Ptr expr, OperatorExpr::Ptr op);
};

struct PrefixExpr: Node {
    typedef std::shared_ptr<PrefixExpr> Ptr;
    
    OperatorExpr::Ptr op;
    Node::Ptr expr; // postfix-expression
    
    PrefixExpr(OperatorExpr::Ptr op, Node::Ptr expr);
};

struct BinaryExpr: Node {
    typedef std::shared_ptr<BinaryExpr> Ptr;
    
    OperatorExpr::Ptr op;
    std::shared_ptr<Node> expr;
    
    BinaryExpr(OperatorExpr::Ptr op, Node::Ptr expr);
};

struct AssignmentExpr: Node {
    typedef std::shared_ptr<AssignmentExpr> Ptr;
    
    Node::Ptr expr;
    
    // After binded, the identifier will be binded 
    Node::Ptr left;
    AssignmentExpr(Node::Ptr expr);
};

struct ArguCallExpr: Node {
    typedef std::shared_ptr<ArguCallExpr> Ptr;
    
    IdentifierExpr::Ptr label;
    Node::Ptr expr;
    
    ArguCallExpr(IdentifierExpr::Ptr label, Node::Ptr expr);
};

struct FuncCallExpr: Node {
    typedef std::shared_ptr<FuncCallExpr> Ptr;
    
    Node::Ptr identifier;
    std::vector<ArguCallExpr::Ptr> arguments;
    
    FuncCallExpr(Node::Ptr expr, std::vector<ArguCallExpr::Ptr> arguments);
    
    virtual std::wstring getTypeName();
};

struct MemberFuncCallExpr: Node {
    typedef std::shared_ptr<MemberFuncCallExpr> Ptr;
    
    Node::Ptr parent;
    Node::Ptr member;
    
    std::vector<ArguCallExpr::Ptr> arguments;
    
    MemberFuncCallExpr(Node::Ptr parent, Node::Ptr member, std::vector<ArguCallExpr::Ptr> arguments);
    
    virtual std::wstring getTypeName();
};

struct MemberAccessExpr: Node {
    typedef std::shared_ptr<MemberAccessExpr> Ptr;
    
    Node::Ptr parent;
    Node::Ptr member;
    
    MemberAccessExpr(Node::Ptr parent, Node::Ptr member);
    
    virtual std::wstring getTypeName();
};

struct LiteralExpr : Node {
    typedef std::shared_ptr<LiteralExpr> Ptr;
    
    Token::Ptr literal;
    LiteralExpr(Token::Ptr literal);
};

struct ArrayLiteralExpr: Node {
    typedef std::shared_ptr<ArrayLiteralExpr> Ptr;
    
    std::vector<Node::Ptr> items;
    
    ArrayLiteralExpr(std::vector<Node::Ptr> items);
};

struct DictLiteralExpr: Node {
    typedef std::shared_ptr<DictLiteralExpr> Ptr;
    
    std::vector<std::tuple<Node::Ptr, Node::Ptr>> items;
    
    DictLiteralExpr(std::vector<std::tuple<Node::Ptr, Node::Ptr>> items);
};

struct ParenthesizedExpr: Node {
    typedef std::shared_ptr<ParenthesizedExpr> Ptr;
    
    std::shared_ptr<Node> expr;
    ParenthesizedExpr(std::shared_ptr<Node> expr);
};

struct SelfExpr: Node {
    typedef std::shared_ptr<SelfExpr> Ptr;
    
    IdentifierExpr::Ptr identifier;
    
    SelfExpr(IdentifierExpr::Ptr identifier);
};

struct SubscriptExpr: Node {
    typedef std::shared_ptr<SubscriptExpr> Ptr;
    
    Node::Ptr identifier;
    Node::Ptr indexExpr;
    
    SubscriptExpr(Node::Ptr identifier, Node::Ptr indexExpr);
};

struct FileImportDecl: Node {
    typedef std::shared_ptr<FileImportDecl> Ptr;
    
    Token::Ptr stringLiteral;
    JrModuleClass* moduleClass;
    
    FileImportDecl(Token::Ptr stringLiteral);
    
    const std::wstring getImportedFilename();
    
};

struct ModuleMemberAccessExpr: public Node {
    typedef std::shared_ptr<ModuleMemberAccessExpr> Ptr;
    
    JrModuleClass* moduleClass;
    Node::Ptr member;
    
    ModuleMemberAccessExpr(JrModuleClass* moduleClass, Node::Ptr member);
};

struct ModuleFuncCallExpr: public Node {
    typedef std::shared_ptr<ModuleFuncCallExpr> Ptr;
    
    JrModuleClass* moduleClass;
    Node::Ptr member;
    
    ModuleFuncCallExpr(JrModuleClass* moduleClass, Node::Ptr member);
};

struct SourceBlock: Node {
    typedef std::shared_ptr<SourceBlock> Ptr;
    
    std::vector<Node::Ptr> statements;
    
    std::wstring filename;

    SourceBlock(std::vector<Node::Ptr> statements);

    std::vector<FileImportDecl::Ptr> getFileImports();
    
    virtual std::wstring getName();
};

struct CodeBlock: Node {
    typedef std::shared_ptr<CodeBlock> Ptr;
    
    std::vector<Node::Ptr> statements;

    CodeBlock(std::vector<Node::Ptr> statements);

};

struct ForInStatement: Node {
    typedef std::shared_ptr<ForInStatement> Ptr;
    
    Node::Ptr pattern;
    Node::Ptr inExpr;
    Node::Ptr codeBlock;

    ForInStatement(Node::Ptr pattern, Node::Ptr inExpr, Node::Ptr codeBlock);
};

struct WhileStatement: Node {
    typedef std::shared_ptr<WhileStatement> Ptr;
    
    Node::Ptr expr;
    Node::Ptr codeBlock;
    
    WhileStatement(Node::Ptr expr, Node::Ptr codeBlock);
};

struct IfStatement: Node {
    typedef std::shared_ptr<IfStatement> Ptr;
    
    Node::Ptr condition;
    Node::Ptr ifCodeBlock;
    Node::Ptr elseCodeBlock;

    IfStatement(Node::Ptr condition, Node::Ptr ifCodeBlock, Node::Ptr elseCodeBlock);
};

struct ReturnStatement: Node {
    typedef std::shared_ptr<ReturnStatement> Ptr;
    
    Node::Ptr expr;
    
    ReturnStatement(Node::Ptr expr);
};

struct NodeVisitor {
protected:
    Node::Ptr visit(Node::Ptr node);
    virtual Node::Ptr visit(SourceBlock::Ptr decl) = 0;
    virtual Node::Ptr visit(ClassDecl::Ptr classDecl) = 0;
    virtual Node::Ptr visit(LetDecl::Ptr decl) = 0;
    virtual Node::Ptr visit(VarDecl::Ptr varDecl) = 0;
    virtual Node::Ptr visit(ConstructorDecl::Ptr decl) = 0;
    virtual Node::Ptr visit(Type::Ptr decl) = 0;
    virtual Node::Ptr visit(FuncCallExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(MemberFuncCallExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(ModuleMemberAccessExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(ModuleFuncCallExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(ArguCallExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(LiteralExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(PrefixExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(IdentifierExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(Expr::Ptr decl) = 0;
    virtual Node::Ptr visit(AssignmentExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(BinaryExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(OperatorExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(ParenthesizedExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(IfStatement::Ptr decl) = 0;
    virtual Node::Ptr visit(WhileStatement::Ptr decl) = 0;
    virtual Node::Ptr visit(CodeBlock::Ptr decl) = 0;
    virtual Node::Ptr visit(FuncDecl::Ptr decl) = 0;
    virtual Node::Ptr visit(ParameterClause::Ptr decl) = 0;
    virtual Node::Ptr visit(Pattern::Ptr decl) = 0;
    virtual Node::Ptr visit(ReturnStatement::Ptr decl) = 0;
    virtual Node::Ptr visit(SelfExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(ArrayLiteralExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(DictLiteralExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(MemberAccessExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(SubscriptExpr::Ptr decl) = 0;
    virtual Node::Ptr visit(ArrayType::Ptr decl) = 0;
    virtual Node::Ptr visit(FileImportDecl::Ptr decl) = 0;
    

};

// ASM node printer
struct NodeDebugPrinter {
    NodeDebugPrinter(const std::wstring filename);
    
    void print(Node::Ptr node);
    void close();
    
protected:
    void print(std::vector<Node::Ptr> nodes);
    void print(SymbolTable::Ptr symtable);
    void print(Symbol::Ptr symbol);
    void printTab();
    void incTab();
    void decTab();
    
private:
    int size = 0;
    std::wofstream output;
};

#endif
