#ifndef __joyeer_compiler_node_types_h__
#define __joyeer_compiler_node_types_h__

#include "joyeer/compiler/node.h"
#include "joyeer/compiler/node+stmt.h"

struct DeclNode : public Node {
    using Ptr = std::shared_ptr<DeclNode>;
public:
    Descriptor::Ptr descriptor = nullptr;
    DeclNode(SyntaxKind kind): Node(kind) {}
    
    // update self descriptor
    virtual void updateDescriptor() { }
};


// Represent a Function in Ast tree.
struct FuncDecl: public DeclNode {
public:
    using Ptr = std::shared_ptr<FuncDecl>;
    
    Node::Ptr identifier = nullptr;               //IdentifierExpr
    Node::Ptr parameterClause;
    Node::Ptr codeBlock;
    Node::Ptr returnType;
    bool isConstructor = false;

    FuncDecl(Node::Ptr identifier, Node::Ptr parameterClause, Node::Ptr returnType, Node::Ptr codeBlock);
    
    static Ptr makeConstructor(Node::Ptr parameterClause, StmtsBlock::Ptr stmts) {
        auto decl = std::make_shared<FuncDecl>(nullptr, parameterClause, nullptr, stmts);
        decl->isConstructor = true;
        return decl;
    }
    
    virtual std::string getTypeName();
};


struct ClassDecl: public DeclNode {
    using Ptr = std::shared_ptr<ClassDecl>;
    
    Token::Ptr name = nullptr;
    std::vector<Node::Ptr> members;
    
    std::vector<DeclNode::Ptr> staticFields;
    std::vector<DeclNode::Ptr> instanceFields;
    std::vector<FuncDecl::Ptr> staticMethods;
    std::vector<FuncDecl::Ptr> instanceMethods;
    std::vector<FuncDecl::Ptr> constructors;
    
    FuncDecl::Ptr defaultConstructor;
    
    ClassDecl(Token::Ptr name, std::vector<Node::Ptr> members);
    
    std::string getName();
};

// Reprensent an FileModule in Ast tree, each xxx.joyeer file is a file module
class FileModuleDecl: public ClassDecl {
public:
    using Ptr = std::shared_ptr<FileModuleDecl>;

public:
    FileModuleDecl(FileModuleDescriptor::Ptr descriptor, StmtsBlock::Ptr block);
    
    StmtsBlock::Ptr block;
    
    std::string filename;

    std::vector<FileImportStmt::Ptr> getFileImports();
    
    virtual std::string getName();
    // get the top level declarations
    std::vector<Node::Ptr> getTopLevelDecls();
};

// `let` or `var` declaration
struct VarDecl: public DeclNode {
    using Ptr = std::shared_ptr<VarDecl>;
    
    // `let` mutable = false, `var` mutable = true
    bool _mutable = true;
    Pattern::Ptr pattern;
    Node::Ptr initializer;
    Descriptor::Ptr parentDescriptor;
    
    VarDecl(Pattern::Ptr pattern, std::shared_ptr<Node> initializer);
    
};

#endif
