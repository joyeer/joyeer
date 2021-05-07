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
    
    virtual std::string getTypeName();
};

struct MemberFuncCallExpr: Node {
    typedef std::shared_ptr<MemberFuncCallExpr> Ptr;
    
    Node::Ptr parent;
    Node::Ptr member;
    
    std::vector<ArguCallExpr::Ptr> arguments;
    
    MemberFuncCallExpr(Node::Ptr parent, Node::Ptr member, std::vector<ArguCallExpr::Ptr> arguments);
    
    virtual std::string getTypeName();
};

struct MemberAccessExpr: Node {
    typedef std::shared_ptr<MemberAccessExpr> Ptr;
    
    Node::Ptr parent;
    Node::Ptr member;
    
    MemberAccessExpr(Node::Ptr parent, Node::Ptr member);
    
    virtual std::string getTypeName();
};

struct StaticAccessExpr: Node {
    using Ptr = std::shared_ptr<StaticAccessExpr>;
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

struct FileImportStmt: Node {
    typedef std::shared_ptr<FileImportStmt> Ptr;
    
    Token::Ptr stringLiteral;
    JrModuleClass* moduleClass;
    
    FileImportStmt(Token::Ptr stringLiteral);
    
    const std::string getImportedFilename();
    
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

// StmtsBlock represent an { ... } code block
struct StmtsBlock: Node {
    typedef std::shared_ptr<StmtsBlock> Ptr;
    
    std::vector<Node::Ptr> statements;

    StmtsBlock(std::vector<Node::Ptr> statements);

};

// For In statement
struct ForInStmt: Node {
    typedef std::shared_ptr<ForInStmt> Ptr;
    
    Node::Ptr pattern;
    Node::Ptr inExpr;
    Node::Ptr codeBlock;

    ForInStmt(Node::Ptr pattern, Node::Ptr inExpr, Node::Ptr codeBlock);
};

struct WhileStmt: Node {
    typedef std::shared_ptr<WhileStmt> Ptr;
    
    Node::Ptr expr;
    Node::Ptr codeBlock;
    
    WhileStmt(Node::Ptr expr, Node::Ptr codeBlock);
};

struct IfStmt: Node {
    typedef std::shared_ptr<IfStmt> Ptr;
    
    Node::Ptr condition;
    Node::Ptr ifCodeBlock;
    Node::Ptr elseCodeBlock;

    IfStmt(Node::Ptr condition, Node::Ptr ifCodeBlock, Node::Ptr elseCodeBlock);
};

struct ReturnStmt: Node {
    typedef std::shared_ptr<ReturnStmt> Ptr;
    
    Node::Ptr expr;
    
    ReturnStmt(Node::Ptr expr);
};


#endif
