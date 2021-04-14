#ifndef __joyeer_compiler_symtable_repo_h__
#define __joyeer_compiler_symtable_repo_h__


#include "joyeer/compiler/node+types.h"

class NodeRepository {

public:
    int register(Node::Ptr node) {
        int pos = repos.size();
        repos.push_back(node);
        return pos
    }

    Node::Ptr get(int position) {
        return repos[position];
    }

private:
    std::vector<Node::Ptr> repos;
}

#endif
