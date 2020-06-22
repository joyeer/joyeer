#ifndef __joyeer_compiler_syntax_types_h__
#define __joyeer_compiler_syntax_types_h__

#include <string>
#include <unordered_map>
#include <vector>

class FieldTypeDescriptor;

enum TypeKind {
    noneType =                      0,
    sourceType =                    1 << 1,
    primaryType =                   1 << 2,
    classType =                     1 << 3,
    methodType =                    1 << 4,
    memberMethodType =              1 << 5,
    memberFieldType =               1 << 6,
    
    intType =                       1 << 11,
    floatType =                     1 << 12
};

class TypeDescriptor {
public:
    typedef std::shared_ptr<TypeDescriptor> Pointer;
    typedef std::weak_ptr<TypeDescriptor> WeakPointer;
    
public:
    TypeDescriptor(TypeKind kind, const std::wstring& name);
    
protected:
    TypeKind kind;
    std::wstring name;
};

class SourceFileTypeDescriptor: public TypeDescriptor {
public:
    typedef std::shared_ptr<SourceFileTypeDescriptor> Pointer;
    
    SourceFileTypeDescriptor(const std::wstring& name);
};

/// Primary data type. e.g. int, float
class PrimaryTypeDescriptor : public TypeDescriptor {
public:
    typedef std::shared_ptr<PrimaryTypeDescriptor> Pointer;
    
public:
    // pre-define primary type
    const static PrimaryTypeDescriptor::Pointer Int;
    
public:
    PrimaryTypeDescriptor(const std::wstring& name);
    
};

/// Field Type
class FieldTypeDescriptor: public TypeDescriptor {
public:
    typedef std::shared_ptr<FieldTypeDescriptor> Pointer;
public:
    FieldTypeDescriptor(const std::wstring& name);
    
    TypeDescriptor::WeakPointer type; // field's typeDescriptor
};

/// Method
class MethodTypeDescriptor: public TypeDescriptor {
    
};

/// Class type
class ClassTypeDescriptor: public TypeDescriptor {
public:
    typedef std::shared_ptr<ClassTypeDescriptor> Pointer;
    
    ClassTypeDescriptor(const std::wstring& name);
    
protected:
    std::vector<FieldTypeDescriptor::Pointer> fields;
};


/// TypeFactory is managing all types, creata and query types
class TypeFactory {
public:
    typedef std::shared_ptr<TypeFactory> Pointer;
public:
    TypeFactory();
    
    SourceFileTypeDescriptor::Pointer createSourceFileType(const std::wstring& name);
    
    ClassTypeDescriptor::Pointer createClassType(const std::wstring& name);
    
    FieldTypeDescriptor::Pointer createFieldType(const std::wstring& name);
    
private:
    std::unordered_map<std::wstring, TypeDescriptor::Pointer> types;
};

#endif
