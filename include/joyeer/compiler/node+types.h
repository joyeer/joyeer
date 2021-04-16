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

    std::vector<FileImportDecl::Ptr> getFileImports();
    
    virtual std::string getName();
    // get the top level declarations
    std::vector<Node::Ptr> getTopLevelDecls();
};



#endif
