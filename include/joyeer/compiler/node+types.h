#ifndef __joyeer_compiler_node_types_h__
#define __joyeer_compiler_node_types_h__

#include "joyeer/compiler/node.h"


struct DeclNode : public Node {
    using Ptr = std::shared_ptr<DeclNode>;
public:
    Descriptor::Ptr descriptor = nullptr;
    DeclNode(SyntaxKind kind): Node(kind) {}
    
    // update self descriptor
    virtual void updateDescriptor() { }
};

// Represent a Constructor of Class in AST tree
struct ConstructorDecl: public DeclNode {
    typedef std::shared_ptr<ConstructorDecl> Ptr;
    
    Node::Ptr parameterClause;
    Node::Ptr codeBlock;
    
    ConstructorDecl(Node::Ptr parameterClause, Node::Ptr codeBlock);
    
    // return constructor's symbol name e.g. init(...)
    const std::string getName(JrType* ownerType);
};

struct ClassDecl: public DeclNode {
    using Ptr = std::shared_ptr<ClassDecl>;
    
    Token::Ptr name = nullptr;
    std::vector<Node::Ptr> members;
    
    ClassDecl(Token::Ptr name, std::vector<Node::Ptr> members);
    
    std::string getName();
};

// Reprensent an FileModule in Ast tree, each xxx.joyeer file is a file module
class FileModuleDecl: public DeclNode {
public:
    using Ptr = std::shared_ptr<FileModuleDecl>;

public:
    FileModuleDecl(FileModuleDescriptor::Ptr descriptor, std::vector<Node::Ptr> statements);
    
    std::vector<Node::Ptr> statements;
    // the default initializer function of the filemodule
    ConstructorDecl::Ptr defaultInitializer;
    
    std::string filename;

    std::vector<FileImportStatement::Ptr> getFileImports();
    
    virtual std::string getName();
    // get the top level declarations
    std::vector<Node::Ptr> getTopLevelDecls();
};


// Represent a Function in Ast tree.
struct FuncDecl: public DeclNode {
public:
    using Ptr = std::shared_ptr<FuncDecl>;
    
    Node::Ptr identifier;               //IdentifierExpr
    Node::Ptr parameterClause;
    Node::Ptr codeBlock;
    Node::Ptr returnType;

    FuncDecl(Node::Ptr identifier, Node::Ptr parameterClause, Node::Ptr returnType, Node::Ptr codeBlock);
    
    virtual std::string getTypeName();
};


struct VarDecl: public DeclNode {
    using Ptr = std::shared_ptr<VarDecl>;
    
    Pattern::Ptr pattern;
    Node::Ptr initializer;
    Descriptor::Ptr parentDescriptor;
    
    VarDecl(Pattern::Ptr pattern, std::shared_ptr<Node> initializer);
    
};

#endif
