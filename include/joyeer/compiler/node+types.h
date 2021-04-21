#ifndef __joyeer_compiler_node_types_h__
#define __joyeer_compiler_node_types_h__

#include "joyeer/compiler/node.h"

// Reprensent an FileModule in Ast tree, each xxx.joyeer file is a file module
class FileModuleNode: public Node {
public:
    using Ptr = std::shared_ptr<FileModuleNode>;

public:
    FileModuleNode(FileModuleDescriptor::Ptr descriptor, std::vector<Node::Ptr> statements);
    
    std::vector<Node::Ptr> statements;
    
    std::string filename;

    std::vector<FileImportStatement::Ptr> getFileImports();
    
    virtual std::string getName();
    // get the top level declarations
    std::vector<Node::Ptr> getTopLevelDecls();
};


// Represent a Function in Ast tree.
struct FuncDecl: Node {
public:
    using Ptr = std::shared_ptr<FuncDecl>;
    
    Node::Ptr identifier;               //IdentifierExpr
    Node::Ptr parameterClause;
    Node::Ptr codeBlock;
    Node::Ptr returnType;

    FuncDecl(Node::Ptr identifier, Node::Ptr parameterClause, Node::Ptr returnType, Node::Ptr codeBlock);
    
    virtual std::string getTypeName();
};

// Represent a Constructor of Class in AST tree
struct ConstructorDecl: Node {
    typedef std::shared_ptr<ConstructorDecl> Ptr;
    
    Node::Ptr parameterClause;
    Node::Ptr codeBlock;
    
    ConstructorDecl(Node::Ptr parameterClause, Node::Ptr codeBlock);
    
    // return constructor's symbol name e.g. init(...)
    const std::string getName(JrType* ownerType);
};




#endif
