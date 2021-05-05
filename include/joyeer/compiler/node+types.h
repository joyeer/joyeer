#ifndef __joyeer_compiler_node_types_h__
#define __joyeer_compiler_node_types_h__

#include "joyeer/compiler/node.h"


struct DeclNode : public Node {
    using Ptr = std::shared_ptr<DeclNode>;
public:
    Descriptor::Ptr descriptor = nullptr;
    
    DeclNode(SyntaxKind kind): Node(kind) {}
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

// Reprensent an FileModule in Ast tree, each xxx.joyeer file is a file module
class FileModuleNode: public DeclNode {
public:
    using Ptr = std::shared_ptr<FileModuleNode>;

public:
    FileModuleNode(FileModuleDescriptor::Ptr descriptor, std::vector<Node::Ptr> statements);
    
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

#endif
