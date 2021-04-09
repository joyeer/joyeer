#ifndef __joyeer_compiler_node_types_h__
#define __joyeer_compiler_node_types_h__

#include "node.h"

struct FileModuleNode: Node {
    using Ptr = std::shared_ptr<FileModuleNode>;
    
    std::vector<Node::Ptr> statements;
    
    std::string filename;

    FileModuleNode(std::vector<Node::Ptr> statements);

    std::vector<FileImportDecl::Ptr> getFileImports();
    
    virtual std::string getName();
};



#endif
