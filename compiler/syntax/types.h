#ifndef __joyeer_compiler_syntax_types_h__
#define __joyeer_compiler_syntax_types_h__

#include <string>

enum TypeKind {
    none =                  0,
    primary =               1 << 1,
    class_ =                1 << 2,
    method =                1 << 3,
    memberMethod =          1 << 4,
    
    
    int_ =      1 << 11,
    float_ =    1 << 12
};

class TypeDescriptor {
    typedef std::shared_ptr<TypeDescriptor> Pointer;
    
public:
    TypeDescriptor(TypeKind kind, const std::wstring& name);
    
protected:
    TypeKind kind;
    std::wstring name;
};


class MethodTypeDescriptor: TypeDescriptor {
    
};

class PrimaryTypeDescriptor : TypeDescriptor {
public:
    typedef std::shared_ptr<PrimaryTypeDescriptor> Pointer;
    
public:
    // pre-define primary type
    const static PrimaryTypeDescriptor::Pointer Int;
    
public:
    PrimaryTypeDescriptor(const std::wstring& name);
    
};


class ClassTypeDescriptor: TypeDescriptor {
public:
    typedef std::shared_ptr<ClassTypeDescriptor> Pointer;
    
    ClassTypeDescriptor(const std::wstring& name);
};


#endif
