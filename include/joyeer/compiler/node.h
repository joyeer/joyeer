#ifndef __joyeer_compiler_AST_node_h__
#define __joyeer_compiler_AST_node_h__

#include <utility>
#include <sstream>
#include <fstream>

#include "joyeer/common/descriptor.h"
#include "joyeer/compiler/token.h"
#include "joyeer/compiler/symtable.h"
#include "joyeer/runtime/runtime.h"

enum class SyntaxKind {
    fileModule = 1L,

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
    memberAssignExpr,
    subscriptExpr,

    literalExpr,
    arrayLiteralExpr,
    dictLiteralExpr,
    operatorExpr

};

enum class NodeAccessFlag : int64_t {
    _default = 0,
    _public = 1 << 1,
    _private = 1 << 2,
    _static = 1 << 3,
};

// Lambda expr for update node's parent = this
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
    Symbol::Ptr symbol = nullptr;
    SymbolTable::Ptr symtable = nullptr;
    Node::Ptr parent = nullptr;

    JrType *type = nullptr;

    // return the name of Node, it will be used as symbol in some cases
    virtual std::string getSimpleName();

    // return the ClassDecl representing the node in which it was declared
    Node::Ptr getDeclaringClassDecl() const {
        Node::Ptr current = parent;
        while (current != nullptr) {
            if (current->kind == SyntaxKind::classDecl || current->kind == SyntaxKind::fileModule) {
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
    JrType *leftType;
    JrType *rightType;

    explicit OperatorExpr(Token::Ptr token);

    void recursiveUpdate() override { /* leave empty */ }
};

struct Type : Node {
    using Ptr = std::shared_ptr<Type>;

    Node::Ptr identifier; // identifierExpr

    explicit Type(Node::Ptr identifier);

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(identifier, NODE_UPDATE_ACTION_SET_PARENT_THIS(identifier))
    }

};

struct ArrayType : Node {
    using Ptr = std::shared_ptr<ArrayType>;

    Node::Ptr type;

    explicit ArrayType(Node::Ptr type);

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(type, NODE_UPDATE_ACTION_SET_PARENT_THIS(type))
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
    Node::Ptr type; // the type of pattern, optional nullptr

    Pattern(IdentifierExpr::Ptr identifier, Node::Ptr type);

    const std::string &getIdentifierName();

    std::string getSimpleName() override {
        return getIdentifierName();
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(identifier, NODE_UPDATE_ACTION_SET_PARENT_THIS(identifier))
        NODE_RECURSIVE_UPDATE(type, NODE_UPDATE_ACTION_SET_PARENT_THIS(type))
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

    Expr(std::vector<Node::Ptr> nodes) :
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
            op(op),
            expr(expr) {
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
            op(op),
            expr(expr) {
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
        NODE_RECURSIVE_UPDATE(op, NODE_UPDATE_ACTION_SET_PARENT_THIS(op))
    }
};

struct AssignmentExpr : Node {
    using Ptr = std::shared_ptr<AssignmentExpr>;

    Node::Ptr expr;

    // After binded, the identifier will be binded
    Node::Ptr left;

    explicit AssignmentExpr(Node::Ptr expr) :
            Node(SyntaxKind::assignmentExpr),
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

    FuncCallExpr(Node::Ptr expr, std::vector<ArguCallExpr::Ptr> arguments) :
            Node(SyntaxKind::funcCallExpr),
            identifier(expr),
            arguments(arguments) {
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
    Node::Ptr member;

    std::vector<ArguCallExpr::Ptr> arguments;

    MemberFuncCallExpr(Node::Ptr callee, Node::Ptr member, std::vector<ArguCallExpr::Ptr> arguments) :
            Node(SyntaxKind::memberFuncCallExpr),
            callee(callee),
            member(member),
            arguments(arguments) {
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(callee, NODE_UPDATE_ACTION_SET_PARENT_THIS(callee))
        NODE_RECURSIVE_UPDATE(member, NODE_UPDATE_ACTION_SET_PARENT_THIS(member))
        for (auto &argument: arguments) {
            NODE_RECURSIVE_UPDATE(argument, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(argument))
        }
    }
};

struct MemberAccessExpr : Node {
    using Ptr = std::shared_ptr<MemberAccessExpr>;

    Node::Ptr callee;
    Node::Ptr member;

    MemberAccessExpr(Node::Ptr callee, Node::Ptr member) :
            Node(SyntaxKind::memberAccessExpr),
            callee(callee),
            member(member) {
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
            literal(literal) {
    }

    void recursiveUpdate() override {
    }
};

struct ArrayLiteralExpr : Node {
    using Ptr = std::shared_ptr<ArrayLiteralExpr>;

    std::vector<Node::Ptr> items;

    explicit ArrayLiteralExpr(std::vector<Node::Ptr> items) :
            Node(SyntaxKind::arrayLiteralExpr),
            items(items) {
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

    explicit ParenthesizedExpr(std::shared_ptr<Node> expr) :
            Node(SyntaxKind::parenthesizedExpr),
            expr(expr) {
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
            identifier(identifier) {
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

struct FileImportStmt : Node {
    using Ptr = std::shared_ptr<FileImportStmt>;

    Token::Ptr stringLiteral;
    JrModuleClass *moduleClass;

    explicit FileImportStmt(Token::Ptr stringLiteral) :
            Node(SyntaxKind::fileimportStmt),
            stringLiteral(stringLiteral) {
    }

    const std::string getImportedFilename() {
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
            pattern(pattern),
            inExpr(inExpr),
            codeBlock(codeBlock) {
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
            expr(expr),
            codeBlock(codeBlock) {
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
            condition(condition),
            ifCodeBlock(ifCodeBlock),
            elseCodeBlock(elseCodeBlock) {
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
            expr(expr) {
    }


    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
    }
};

/********************************************************
 *  Decl Section
 *********************************************************/

struct DeclNode : public Node {
    using Ptr = std::shared_ptr<DeclNode>;
public:
    Descriptor::Ptr descriptor = nullptr;

    explicit DeclNode(SyntaxKind kind) : Node(kind) {}
};

enum class FuncType {
    staticInitializer = 1,  // class's static initializer
    constructor = 2,        // class constructor
    function = 3            // function
};

// Helper FuncType to SymbolFlag
static SymbolFlag funcTypeToSymbolFlag(FuncType type) {
    switch (type) {
        case FuncType::staticInitializer:
            return SymbolFlag::staticInitializer;
        case FuncType::constructor:
            return SymbolFlag::constructor;
        case FuncType::function:
            return SymbolFlag::funcSymbol;
    }
}

// Represent a Function in Ast tree.
struct FuncDecl : public DeclNode {
public:
    using Ptr = std::shared_ptr<FuncDecl>;

    FuncType type;
    Node::Ptr identifier = nullptr;               //IdentifierExpr
    Node::Ptr parameterClause;
    Node::Ptr codeBlock;
    Node::Ptr returnType = nullptr;

    struct {
        Descriptor::Ptr returnDescriptor = nullptr;
    };

    FuncDecl(Node::Ptr identifier, Node::Ptr parameterClause, Node::Ptr returnType, Node::Ptr codeBlock) :
            DeclNode(SyntaxKind::funcDecl),
            identifier(identifier),
            parameterClause(parameterClause),
            returnType(returnType),
            codeBlock(codeBlock) {
        symtable = std::make_shared<SymbolTable>();
    }

    // make FuncDecl as Static initializer
    static Ptr makeStaticInitializer(StmtsBlock::Ptr stmts) {
        auto decl = std::make_shared<FuncDecl>(nullptr, nullptr, nullptr, stmts);
        decl->type = FuncType::staticInitializer;
        return decl;
    }

    static Ptr makeConstructor(Node::Ptr parameterClause, StmtsBlock::Ptr stmts) {
        auto decl = std::make_shared<FuncDecl>(nullptr, parameterClause, nullptr, stmts);
        decl->type = FuncType::constructor;
        return decl;
    }

    // return func name
    std::string getSimpleName() override {
        std::stringstream ss;

        // basis name
        if (identifier != nullptr) {
            ss << identifier->getSimpleName();
        } else if (type == FuncType::constructor || type == FuncType::staticInitializer) {
            auto declaringClassDecl = getDeclaringClassDecl();
            ss << declaringClassDecl->getSimpleName();
        }

        // parameters
        ss << DescriptorConstants::ParenthesisOpen;
        if (parameterClause) {
            for (auto p: std::static_pointer_cast<ParameterClause>(parameterClause)->parameters) {
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


struct ClassDecl : public DeclNode {
    using Ptr = std::shared_ptr<ClassDecl>;

    Token::Ptr name = nullptr;
    StmtsBlock::Ptr members;

    std::vector<DeclNode::Ptr> staticFields;
    std::vector<DeclNode::Ptr> instanceFields;
    std::vector<FuncDecl::Ptr> staticMethods;
    std::vector<FuncDecl::Ptr> instanceMethods;
    std::vector<FuncDecl::Ptr> constructors;

    FuncDecl::Ptr staticConstructor;

    ClassDecl(Token::Ptr name, StmtsBlock::Ptr members) :
            DeclNode(SyntaxKind::classDecl),
            name(name),
            members(members),
            staticFields(),
            staticMethods(),
            instanceMethods(),
            instanceFields() {
        symtable = std::make_shared<SymbolTable>();
    }


    std::string getSimpleName() override {
        return name->rawValue;
    }

    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(members, NODE_UPDATE_ACTION_SET_PARENT_THIS(members))

        for (auto &field: staticFields) {
            NODE_RECURSIVE_UPDATE(field, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(field))
        }
        for (auto &field: instanceFields) {
            NODE_RECURSIVE_UPDATE(field, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(field))
        }
        for (auto &method: staticMethods) {
            NODE_RECURSIVE_UPDATE(method, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(method))
        }
        for (auto &method: instanceMethods) {
            NODE_RECURSIVE_UPDATE(method, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(method))
        }
        for (auto &constructor: constructors) {
            NODE_RECURSIVE_UPDATE(constructor, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(constructor))
        }
        NODE_RECURSIVE_UPDATE(staticConstructor, NODE_UPDATE_ACTION_SET_PARENT_THIS(staticConstructor))
    }
};

// Represent an FileModule in Ast tree, each xxx.joyeer file is a file module
class FileModuleDecl : public ClassDecl {
public:
    using Ptr = std::shared_ptr<FileModuleDecl>;

public:
    FileModuleDecl(const FileModuleDescriptor::Ptr& descriptor, const StmtsBlock::Ptr& block) :
            ClassDecl(nullptr, block) {
        this->members = block;
        kind = SyntaxKind::fileModule;
        this->descriptor = descriptor;
        assert(symtable != nullptr);
    }

    std::string filename;

    std::string getSimpleName() override {
        std::filesystem::path p = filename;
        return p.filename().replace_extension().string();
    }

    void recursiveUpdate() override {
        ClassDecl::recursiveUpdate();
    }
};


// `let` or `var` declaration
struct VarDecl : public DeclNode {
    using Ptr = std::shared_ptr<VarDecl>;

    NodeAccessFlag accessFlag;

    Pattern::Ptr pattern;
    Node::Ptr initializer;
    Descriptor::Ptr parentDescriptor;

    VarDecl(Pattern::Ptr pattern, std::shared_ptr<Node> initializer) :
            DeclNode(SyntaxKind::varDecl),
            pattern(pattern),
            initializer(initializer) {
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
