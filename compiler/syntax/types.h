#ifndef __joyeer_compiler_syntax_types_h__
#define __joyeer_compiler_syntax_types_h__

#include <string>
#include <unordered_map>

enum TypeKind {
    noneType =                      0,
    primaryType =                   1 << 1,
    classType =                     1 << 2,
    methodType =                    1 << 3,
    memberMethodType =              1 << 4,
    
    
    intType =                       1 << 11,
    floatType =                     1 << 12
};

class TypeDescriptor {
public:
    typedef std::shared_ptr<TypeDescriptor> Pointer;
    
public:
    TypeDescriptor(TypeKind kind, const std::wstring& name);
    
protected:
    TypeKind kind;
    std::wstring name;
};


class MethodTypeDescriptor: public TypeDescriptor {
    
};

class PrimaryTypeDescriptor : public TypeDescriptor {
public:
    typedef std::shared_ptr<PrimaryTypeDescriptor> Pointer;
    
public:
    // pre-define primary type
    const static PrimaryTypeDescriptor::Pointer Int;
    
public:
    PrimaryTypeDescriptor(const std::wstring& name);
    
};


class ClassTypeDescriptor: public TypeDescriptor {
public:
    typedef std::shared_ptr<ClassTypeDescriptor> Pointer;
    
    ClassTypeDescriptor(const std::wstring& name);
};


/// TypeFactory is managing all types, creata and query types
class TypeFactory {
public:
    typedef std::shared_ptr<TypeFactory> Pointer;
public:
    TypeFactory();
    
    ClassTypeDescriptor::Pointer createClassType(const std::wstring& name);
    
private:
    std::unordered_map<std::wstring, TypeDescriptor::Pointer> types;
};

#endif
