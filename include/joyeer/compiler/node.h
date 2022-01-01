#ifndef __joyeer_compiler_AST_node_h__
#define __joyeer_compiler_AST_node_h__

#include <utility>
#include <sstream>
#include <fstream>

#include "joyeer/runtime/descriptor.h"
#include "joyeer/compiler/token.h"
#include "joyeer/compiler/symtable.h"
#include "joyeer/compiler/typedef.h"

enum class SyntaxKind {
    module = 1L,

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

    expr,
    assignExpr,
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

// Lambda expr for update node's parentTypeSlot = this
#define NODE_UPDATE_ACTION_SET_PARENT_THIS(node) [this]() { (node)->parent = shared_from_this(); }
#define NODE_UPDATE_ACTION_SET_PARENT_THIS_2(node) [this, node]() { (node)->parent = shared_from_this(); }

// recursive update the node
#define NODE_RECURSIVE_UPDATE(node, expr) \
    if((node) != nullptr) { \
        expr(); \
        (node)->recursiveUpdate(); \
    }


struct Node : std::enable_shared_from_this<Node> {
    using Ptr = std::shared_ptr<Node>;
    using VID = int;

    SyntaxKind kind;
    SymbolTable::Ptr symtable = nullptr;
    Node::Ptr parent = nullptr;

    // represent the Node's Type, only available in Expr Node
    int typeSlot = -1;

    // return the name of Node, it will be used as symbol in some cases
    virtual std::string getSimpleName();

    // return the ClassDecl representing the node in which it was declared
    Node::Ptr getDeclaringClassDecl() const {
        Node::Ptr current = parent;
        while (current != nullptr) {
            if (current->kind == SyntaxKind::classDecl || current->kind == SyntaxKind::module) {
                return current;
            }
            current = current->parent;
        }
        return nullptr;
    }

    // detect if its a declaration node
    bool isDeclNode() const {
        switch (kind) {
            case SyntaxKind::varDecl:
            case SyntaxKind::funcDecl:
            case SyntaxKind::classDecl:
            case SyntaxKind::module:
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

struct IdentifierExpr : public Node {
    using Ptr = std::shared_ptr<IdentifierExpr>;

    Token::Ptr token;

    explicit IdentifierExpr(Token::Ptr token) :
            Node(SyntaxKind::identifierExpr),
            token(std::move(token)) {
    }

    // Return identifier's name
    std::string getSimpleName() override {
        return token->rawValue;
    }

    void recursiveUpdate() override { /* leave empty */ }
};

struct OperatorExpr : Node {
    using Ptr = std::shared_ptr<OperatorExpr>;

    Token::Ptr token;
    OperatorPriority priority;
    Type *leftType;
    Type *rightType;

    explicit OperatorExpr(Token::Ptr token);

    void recursiveUpdate() override { /* leave empty */ }
};

struct TypeIdentifier : Node {
    using Ptr = std::shared_ptr<TypeIdentifier>;

    Node::Ptr identifier; // identifierExpr

    explicit TypeIdentifier(Node::Ptr identifier);

    std::string getSimpleName() override  {
        return identifier->getSimpleName();
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(identifier, NODE_UPDATE_ACTION_SET_PARENT_THIS(identifier))
    }

};

struct ArrayType : Node {
    using Ptr = std::shared_ptr<ArrayType>;

    Node::Ptr valueType;

    explicit ArrayType(Node::Ptr type);

    std::string getSimpleName() override {
        return "Array";
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(valueType, NODE_UPDATE_ACTION_SET_PARENT_THIS(valueType))
    }
};

struct DictType : Node {
    using Ptr = std::shared_ptr<DictType>;

    Node::Ptr keyType;
    Node::Ptr valueType;

    DictType(Node::Ptr keyType, Node::Ptr valueType);

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(keyType, NODE_UPDATE_ACTION_SET_PARENT_THIS(keyType))
        NODE_RECURSIVE_UPDATE(valueType, NODE_UPDATE_ACTION_SET_PARENT_THIS(valueType))
    }
};

struct Pattern : public Node {
    using Ptr = std::shared_ptr<Pattern>;

    IdentifierExpr::Ptr identifier; // the name of pattern
    Node::Ptr typeExpr; // the kind of pattern, optional nullptr

    Pattern(IdentifierExpr::Ptr identifier, Node::Ptr typeExpr);

    const std::string &getIdentifierName() const;

    std::string getSimpleName() override {
        return getIdentifierName();
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(identifier, NODE_UPDATE_ACTION_SET_PARENT_THIS(identifier))
        NODE_RECURSIVE_UPDATE(typeExpr, NODE_UPDATE_ACTION_SET_PARENT_THIS(typeExpr))
    }

};

struct ParameterClause : Node {
    using Ptr = std::shared_ptr<ParameterClause>;

    std::vector<Pattern::Ptr> parameters;

    explicit ParameterClause(std::vector<Pattern::Ptr> parameters);

    void recursiveUpdate() override {
        for (auto &param: parameters) {
            NODE_RECURSIVE_UPDATE(param, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(param))
        }
    }
};

/********************************************************
 *  Expression Section
 *********************************************************/

struct Expr : Node {
    using Ptr = std::shared_ptr<Expr>;

    Node::Ptr prefix;
    std::vector<Node::Ptr> binaries;

    // After binder, the 'binaries' and prefix will be merged into nodes
    std::vector<Node::Ptr> nodes;

    Expr(Node::Ptr prefix, std::vector<Node::Ptr> binaries) :
            Node(SyntaxKind::expr),
            prefix(std::move(prefix)),
            binaries(std::move(binaries)) {
    }

    explicit Expr(std::vector<Node::Ptr> nodes) :
            Node(SyntaxKind::expr),
            prefix(nullptr),
            binaries(),
            nodes(std::move(nodes)) {
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(prefix, NODE_UPDATE_ACTION_SET_PARENT_THIS(prefix))
        for (auto &binary: binaries) {
            NODE_RECURSIVE_UPDATE(binary, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(binary))
        }
        for (auto &node: nodes) {
            NODE_RECURSIVE_UPDATE(node, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(node))
        }
    }
};

struct PostfixExpr : Node {
    using Ptr = std::shared_ptr<PostfixExpr>;

    Node::Ptr expr;
    OperatorExpr::Ptr op;

    PostfixExpr(Node::Ptr expr, OperatorExpr::Ptr op) :
            Node(SyntaxKind::postfixExpr),
            expr(std::move(expr)),
            op(std::move(op)) {
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
        NODE_RECURSIVE_UPDATE(op, NODE_UPDATE_ACTION_SET_PARENT_THIS(op))
    }

};

struct PrefixExpr : Node {
    using Ptr = std::shared_ptr<PrefixExpr>;

    OperatorExpr::Ptr op;
    Node::Ptr expr; // postfix-expression

    PrefixExpr(OperatorExpr::Ptr op, Node::Ptr expr) :
            Node(SyntaxKind::prefixExpr),
            op(std::move(op)),
            expr(std::move(expr)) {
    }


    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
        NODE_RECURSIVE_UPDATE(op, NODE_UPDATE_ACTION_SET_PARENT_THIS(op))
    }
};

struct BinaryExpr : Node {
    using Ptr = std::shared_ptr<BinaryExpr>;

    OperatorExpr::Ptr op;
    Node::Ptr expr;

    BinaryExpr(OperatorExpr::Ptr op, Node::Ptr expr) :
            Node(SyntaxKind::binaryExpr),
            op(std::move(op)),
            expr(std::move(expr)) {
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
        NODE_RECURSIVE_UPDATE(op, NODE_UPDATE_ACTION_SET_PARENT_THIS(op))
    }
};

struct AssignExpr : Node {
    using Ptr = std::shared_ptr<AssignExpr>;

    Node::Ptr expr;

    // After bound, the identifier will be bound
    Node::Ptr left;

    explicit AssignExpr(Node::Ptr expr) :
            Node(SyntaxKind::assignExpr),
            expr(std::move(expr)) {
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
        NODE_RECURSIVE_UPDATE(left, NODE_UPDATE_ACTION_SET_PARENT_THIS(left))
    }
};

struct ArguCallExpr : Node {
    using Ptr = std::shared_ptr<ArguCallExpr>;
    IdentifierExpr::Ptr label;
    Node::Ptr expr;

    ArguCallExpr(IdentifierExpr::Ptr label, Node::Ptr expr) :
            Node(SyntaxKind::arguCallExpr),
            label(std::move(label)),
            expr(std::move(expr)) {
    }


    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
        NODE_RECURSIVE_UPDATE(label, NODE_UPDATE_ACTION_SET_PARENT_THIS(label))
    }
};

// represent a func call expression
// e.g. print(message: "hello world")
struct FuncCallExpr : Node {
    using Ptr = std::shared_ptr<FuncCallExpr>;

    Node::Ptr identifier;
    std::vector<ArguCallExpr::Ptr> arguments;
    int funcTypeSlot = -1;

    FuncCallExpr(Node::Ptr expr, std::vector<ArguCallExpr::Ptr> arguments) :
            Node(SyntaxKind::funcCallExpr),
            identifier(std::move(expr)),
            arguments(std::move(arguments)) {
    }

    /// get callee FuncDecl 's simple-name
    std::string getCalleeFuncSimpleName() const {
        auto ss = std::stringstream();
        ss << identifier->getSimpleName();
        ss << "(";
        for(auto& argument: arguments) {
            ss << argument->label->getSimpleName() << ":";
        }
        ss << ")";
        return ss.str();
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(identifier, NODE_UPDATE_ACTION_SET_PARENT_THIS(identifier))
        for (auto &argument: arguments) {
            NODE_RECURSIVE_UPDATE(argument, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(argument))
        }
    }

};

struct MemberFuncCallExpr : Node {
    using Ptr = std::shared_ptr<MemberFuncCallExpr>;

    Node::Ptr callee;
    Node::Ptr member; // FuncCallExpr

    int funcTypeSlot = -1;

    MemberFuncCallExpr(Node::Ptr callee, FuncCallExpr::Ptr funcCallExpr) :
            Node(SyntaxKind::memberFuncCallExpr),
            callee(std::move(callee)),
            member(std::move(funcCallExpr)) {
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(callee, NODE_UPDATE_ACTION_SET_PARENT_THIS(callee))
        NODE_RECURSIVE_UPDATE(member, NODE_UPDATE_ACTION_SET_PARENT_THIS(member))
    }
};

struct MemberAccessExpr : Node {
    using Ptr = std::shared_ptr<MemberAccessExpr>;

    Node::Ptr callee;
    Node::Ptr member;

    MemberAccessExpr(Node::Ptr callee, Node::Ptr member) :
            Node(SyntaxKind::memberAccessExpr),
            callee(std::move(callee)),
            member(std::move(member)) {
    }


    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(callee, NODE_UPDATE_ACTION_SET_PARENT_THIS(callee))
        NODE_RECURSIVE_UPDATE(member, NODE_UPDATE_ACTION_SET_PARENT_THIS(member))
    }

};

// assign expr to member, it's automatically generated by compiler
struct MemberAssignExpr : Node {
    using Ptr = std::shared_ptr<MemberAssignExpr>;

    Node::Ptr callee; // Member's owner class
    IdentifierExpr::Ptr member; // member self
    Node::Ptr expr; //  initializer expression

    MemberAssignExpr(Node::Ptr callee, IdentifierExpr::Ptr member, Node::Ptr expr) :
            Node(SyntaxKind::memberAssignExpr),
            callee(std::move(callee)),
            member(std::move(member)),
            expr(std::move(expr)) {}

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
        NODE_RECURSIVE_UPDATE(member, NODE_UPDATE_ACTION_SET_PARENT_THIS(member))
    }
};

struct LiteralExpr : Node {
    using Ptr = std::shared_ptr<LiteralExpr>;

    Token::Ptr literal;

    explicit LiteralExpr(Token::Ptr literal) :
            Node(SyntaxKind::literalExpr),
            literal(std::move(literal)) {
    }

    void recursiveUpdate() override {
    }
};

struct ArrayLiteralExpr : Node {
    using Ptr = std::shared_ptr<ArrayLiteralExpr>;

    std::vector<Node::Ptr> items;

    explicit ArrayLiteralExpr(std::vector<Node::Ptr> items) :
            Node(SyntaxKind::arrayLiteralExpr),
            items(std::move(items)) {
    }

    void recursiveUpdate() override {
        for (auto &item: items) {
            NODE_RECURSIVE_UPDATE(item, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(item))
        }
    }
};

struct DictLiteralExpr : Node {
    using Ptr = std::shared_ptr<DictLiteralExpr>;

    std::vector<std::tuple<Node::Ptr, Node::Ptr>> items;

    explicit DictLiteralExpr(std::vector<std::tuple<Node::Ptr, Node::Ptr>> items) :
            Node(SyntaxKind::dictLiteralExpr),
            items(std::move(items)) {
    }

    void recursiveUpdate() override {
        for (auto &item: items) {
            auto key = std::get<0>(item);
            auto value = std::get<1>(item);
            NODE_RECURSIVE_UPDATE(key, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(key))
            NODE_RECURSIVE_UPDATE(value, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(value))
        }
    }
};

struct ParenthesizedExpr : Node {
    using Ptr = std::shared_ptr<ParenthesizedExpr>;

    Node::Ptr expr;

    explicit ParenthesizedExpr(Node::Ptr expr) :
            Node(SyntaxKind::parenthesizedExpr),
            expr(std::move(expr)) {
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
    }
};

struct SelfExpr : Node {
    using Ptr = std::shared_ptr<SelfExpr>;

    IdentifierExpr::Ptr identifier;

    explicit SelfExpr(IdentifierExpr::Ptr identifier) :
            Node(SyntaxKind::selfExpr),
            identifier(std::move(identifier)) {
    }


    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(identifier, NODE_UPDATE_ACTION_SET_PARENT_THIS(identifier))
    }
};

struct SubscriptExpr : Node {
    using Ptr = std::shared_ptr<SubscriptExpr>;

    Node::Ptr identifier;
    Node::Ptr indexExpr;

    SubscriptExpr(Node::Ptr identifier, Node::Ptr indexExpr) :
            Node(SyntaxKind::subscriptExpr),
            identifier(std::move(identifier)),
            indexExpr(std::move(indexExpr)) {
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(identifier, NODE_UPDATE_ACTION_SET_PARENT_THIS(identifier))
        NODE_RECURSIVE_UPDATE(indexExpr, NODE_UPDATE_ACTION_SET_PARENT_THIS(indexExpr))
    }

};

/********************************************************
 *  Statement Section
 *********************************************************/

struct ImportStmt : Node {
    using Ptr = std::shared_ptr<ImportStmt>;

    Token::Ptr stringLiteral;

    explicit ImportStmt(Token::Ptr stringLiteral) :
            Node(SyntaxKind::importStmt),
            stringLiteral(std::move(stringLiteral)) {
    }

    std::string getImportedFilename() const {
        return stringLiteral->rawValue;
    }

    void recursiveUpdate() override {}
};

// StmtsBlock represent an { ... } code block
struct StmtsBlock : Node {
    using Ptr = std::shared_ptr<StmtsBlock>;

    std::vector<Node::Ptr> statements;

    explicit StmtsBlock(std::vector<Node::Ptr> statements) :
            Node(SyntaxKind::stmtsBlock),
            statements(std::move(statements)) {
        symtable = std::make_shared<SymbolTable>();
    }

    void recursiveUpdate() override {
        for (auto &statement: statements) {
            NODE_RECURSIVE_UPDATE(statement, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(statement))
        }
    }
};

// For In statement
struct ForInStmt : Node {
    using Ptr = std::shared_ptr<ForInStmt>;

    Node::Ptr pattern;
    Node::Ptr inExpr;
    Node::Ptr codeBlock;

    ForInStmt(Node::Ptr pattern, Node::Ptr inExpr, Node::Ptr codeBlock) :
            Node(SyntaxKind::forInStmt),
            pattern(std::move(pattern)),
            inExpr(std::move(inExpr)),
            codeBlock(std::move(codeBlock)) {
    }


    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(pattern, NODE_UPDATE_ACTION_SET_PARENT_THIS(pattern))
        NODE_RECURSIVE_UPDATE(inExpr, NODE_UPDATE_ACTION_SET_PARENT_THIS(inExpr))
        NODE_RECURSIVE_UPDATE(codeBlock, NODE_UPDATE_ACTION_SET_PARENT_THIS(codeBlock))
    }
};

struct WhileStmt : Node {
    using Ptr = std::shared_ptr<WhileStmt>;

    Node::Ptr expr;
    Node::Ptr codeBlock;

    WhileStmt(Node::Ptr expr, Node::Ptr codeBlock) :
            Node(SyntaxKind::whileStmt),
            expr(std::move(expr)),
            codeBlock(std::move(codeBlock)) {
    }


    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
        NODE_RECURSIVE_UPDATE(codeBlock, NODE_UPDATE_ACTION_SET_PARENT_THIS(codeBlock))
    }
};

struct IfStmt : Node {
    using Ptr = std::shared_ptr<IfStmt>;

    Node::Ptr condition;
    Node::Ptr ifCodeBlock;
    Node::Ptr elseCodeBlock;

    IfStmt(Node::Ptr condition, Node::Ptr ifCodeBlock, Node::Ptr elseCodeBlock) :
            Node(SyntaxKind::ifStmt),
            condition(std::move(condition)),
            ifCodeBlock(std::move(ifCodeBlock)),
            elseCodeBlock(std::move(elseCodeBlock)) {
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(condition, NODE_UPDATE_ACTION_SET_PARENT_THIS(condition))
        NODE_RECURSIVE_UPDATE(ifCodeBlock, NODE_UPDATE_ACTION_SET_PARENT_THIS(ifCodeBlock))
        NODE_RECURSIVE_UPDATE(elseCodeBlock, NODE_UPDATE_ACTION_SET_PARENT_THIS(elseCodeBlock))
    }
};

struct ReturnStmt : Node {
    using Ptr = std::shared_ptr<ReturnStmt>;

    Node::Ptr expr;

    explicit ReturnStmt(Node::Ptr expr) :
            Node(SyntaxKind::returnStmt),
            expr(std::move(expr)) {
    }


    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
    }
};

/********************************************************
 *  Decl Section
 *********************************************************/

// Represent a Function in Ast tree.
struct FuncDecl : public Node {
public:
    using Ptr = std::shared_ptr<FuncDecl>;

    Node::Ptr identifier = nullptr;               //IdentifierExpr
    Node::Ptr parameterClause;
    Node::Ptr codeBlock;
    Node::Ptr returnType = nullptr;

    FuncDecl(Node::Ptr identifier, Node::Ptr parameterClause, Node::Ptr returnType, Node::Ptr codeBlock) :
            Node(SyntaxKind::funcDecl),
            identifier(std::move(identifier)),
            parameterClause(std::move(parameterClause)),
            returnType(std::move(returnType)),
            codeBlock(std::move(codeBlock)) {
        symtable = std::make_shared<SymbolTable>();
    }

    // create a Constructor FuncDecl
    static Ptr makeConstructor(const Node::Ptr& parameterClause, const StmtsBlock::Ptr& stmts) {
        auto decl = std::make_shared<FuncDecl>(nullptr, parameterClause, nullptr, stmts);
        return decl;
    }

    // return func name
    std::string getSimpleName() override {
        std::stringstream ss;

        // basis name
        if (identifier != nullptr) {
            ss << identifier->getSimpleName();
        }

        // parameters
        ss << DescriptorConstants::ParenthesisOpen;
        if (parameterClause) {
            for (const auto& p: std::static_pointer_cast<ParameterClause>(parameterClause)->parameters) {
                ss << p->getSimpleName() << DescriptorConstants::Colon;
            }
        }
        ss << DescriptorConstants::ParenthesisClose;

        return ss.str();
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(identifier, NODE_UPDATE_ACTION_SET_PARENT_THIS(identifier))
        NODE_RECURSIVE_UPDATE(parameterClause, NODE_UPDATE_ACTION_SET_PARENT_THIS(parameterClause))
        NODE_RECURSIVE_UPDATE(codeBlock, NODE_UPDATE_ACTION_SET_PARENT_THIS(codeBlock))
        NODE_RECURSIVE_UPDATE(returnType, NODE_UPDATE_ACTION_SET_PARENT_THIS(returnType))
    }
};


struct ClassDecl : public Node {
    using Ptr = std::shared_ptr<ClassDecl>;

    Token::Ptr name = nullptr;
    StmtsBlock::Ptr members;

    ClassDecl(Token::Ptr name, StmtsBlock::Ptr members) :
            Node(SyntaxKind::classDecl),
            name(std::move(name)),
            members(std::move(members)) {
        symtable = std::make_shared<SymbolTable>();
    }

    std::string getSimpleName() override {
        return name->rawValue;
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(members, NODE_UPDATE_ACTION_SET_PARENT_THIS(members))
    }
};

// Represent an Module in Ast tree, each xxx.joyeer file is a file module
class ModuleDecl : public StmtsBlock {
public:
    using Ptr = std::shared_ptr<ModuleDecl>;

public:
    ModuleDecl(const ModuleDescriptor::Ptr& descriptor, std::vector<Node::Ptr>& statements) :
            StmtsBlock(statements) {
        kind = SyntaxKind::module;
        assert(symtable != nullptr);
    }

    std::string filename;

    std::string getSimpleName() override {
        std::filesystem::path p = filename;
        return p.filename().replace_extension().string();
    }

    void recursiveUpdate() override {
        StmtsBlock::recursiveUpdate();
    }
};


// `let` or `var` declaration
struct VarDecl : public Node {
    using Ptr = std::shared_ptr<VarDecl>;

    Pattern::Ptr pattern;
    Node::Ptr initializer;

    VarDecl(Pattern::Ptr pattern, Node::Ptr initializer) :
            Node(SyntaxKind::varDecl),
            pattern(std::move(pattern)),
            initializer(std::move(initializer)) {
    }

    std::string getSimpleName() override {
        return pattern->getSimpleName();
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(pattern, NODE_UPDATE_ACTION_SET_PARENT_THIS(pattern))
        NODE_RECURSIVE_UPDATE(initializer, NODE_UPDATE_ACTION_SET_PARENT_THIS(initializer))
    }

};

#endif
