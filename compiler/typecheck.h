#ifndef __joyeer_compiler_typecheck_h__
#define __joyeer_compiler_typecheck_h__

#include "node.h"

class TypeChecker {
public:
    void verify(Node::Pointer node);
    
};

#endif
