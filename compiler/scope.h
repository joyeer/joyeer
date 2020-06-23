#ifndef __joyeer_compiler_syntax_scope_h__
#define __joyeer_compiler_syntax_scope_h__

#include "types.h"

enum ScopeFlag {
    sourceScope,
    classScope,
    interClassScope,
    methodScope,
    fieldScope,
};

struct Scope {
    typedef std::shared_ptr<Scope> Pointer;
    
    ScopeFlag flog;
    TypeDescriptor::Pointer scopeType;
    
    Scope(ScopeFlag flag, TypeDescriptor::Pointer scopeType);
};

#endif
