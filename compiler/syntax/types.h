#ifndef __joyeer_compiler_syntax_types_h__
#define __joyeer_compiler_syntax_types_h__

#include <string>

enum TypeKind {
    NONE = 0,
    CLASS = 1 << 1,
};

class TypeRef {
    TypeKind kind;
    std::wstring name;
    
};


#endif
