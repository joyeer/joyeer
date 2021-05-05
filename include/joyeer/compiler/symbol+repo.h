#ifndef __joyeer_compiler_symtable_repo_h__
#define __joyeer_compiler_symtable_repo_h__

#include "joyeer/compiler/node+types.h"

class NodeRepository {
public:
    using Ptr = std::shared_ptr<NodeRepository>;
    
public:
    int store(DeclNode::Ptr decl) {
        auto descriptorString = decl->descriptor->getRawDescriptor();
        if(descriptors.contains(descriptorString)) {
            return descriptors[descriptorString];
        }
        int pos = nodes.size();
        nodes.push_back(decl);
        return pos;
    }

    Node::Ptr get(int position) {
        return nodes[position];
    }
    
private:
    std::unordered_map<std::string, int> descriptors;
    std::vector<DeclNode::Ptr> nodes;
};

#endif
