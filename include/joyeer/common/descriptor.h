#ifndef __joyeer_common_descriptor_h__
#define __joyeer_common_descriptor_h__


#include <string>
#include <vector>

//  interpretation of a Type
struct DescriptorType {
    static const char Bool = 'B';
    static const char Int = 'L';
    static const char UInt = 'l';
    static const char Int8 = 'D';
    static const char UInt8 = 'd';
    static const char Int16 = 'H';
    static const char UInt16 = 'h';
    static const char Int32 = 'K';
    static const char UInt32 = 'k';
    static const char Int128 = 'X';
    static const char UInt128 = 'x';
    static const char Float = 'F';
    static const char Double = 'O';
    static const char Float128 = 'W';
    static const char String = 'S';
    static const char Void = 'V';

    // descriptor prefix of class/interface/func/file module/array/variable
    static const char ClassPrefix = '^';
    static const char InterfacePrefix = '~';
    static const char FuncPrefix = '&';
    static const char ArrayPrefix = '[';
    static const char FileModulePrefix = '#';
    static const char VariablePrefix = '@';

    static const char END = ';' ;
};

class Descriptor {
public:
    using Ptr = std::shared_ptr<Descriptor>;
    const std::string& getRawDescriptor() const { return rawDescriptor; }
protected:
    std::string rawDescriptor;
};

#define DECLARE_DESCRIPTOR(name, type) \
class name##Descriptor : public Descriptor { \
public: \
    using Ptr = std::shared_ptr<name##Descriptor>; \
    name##Descriptor() { \
        rawDescriptor = type; \
    } \
};

DECLARE_DESCRIPTOR(Bool,        DescriptorType::Bool)
DECLARE_DESCRIPTOR(Int,         DescriptorType::Int)
DECLARE_DESCRIPTOR(UInt,        DescriptorType::UInt)
DECLARE_DESCRIPTOR(Int8,        DescriptorType::Int8)
DECLARE_DESCRIPTOR(UInt8,       DescriptorType::UInt8)
DECLARE_DESCRIPTOR(Int16,       DescriptorType::Int16)
DECLARE_DESCRIPTOR(UInt16,      DescriptorType::UInt16)
DECLARE_DESCRIPTOR(Int32,       DescriptorType::Int32)
DECLARE_DESCRIPTOR(UInt32,      DescriptorType::UInt32)
DECLARE_DESCRIPTOR(Int128,      DescriptorType::Int128)
DECLARE_DESCRIPTOR(UInt128,     DescriptorType::UInt128)
DECLARE_DESCRIPTOR(Float,       DescriptorType::Float)
DECLARE_DESCRIPTOR(Double,      DescriptorType::Double)
DECLARE_DESCRIPTOR(Float128,    DescriptorType::Float128)
DECLARE_DESCRIPTOR(String,      DescriptorType::String)
DECLARE_DESCRIPTOR(Void,        DescriptorType::Void)

// Represent an Variable
class VariableDescriptor: public Descriptor {
public:
    VariableDescriptor(Descriptor::Ptr parent, const std::string& name);
    
    Descriptor::Ptr parentDescriptor;
    Descriptor::Ptr typeDescriptor = nullptr;
};

class FunctionDescriptor: public Descriptor {
public:
    FunctionDescriptor(const std::string& filename);
};

class ClassDescriptor: public Descriptor {

};

// File Module descriptor, e.g.
// foo.joyeer, descriptor: #foo;
// bar/foo.joyeer, descriptor: #bar/foo;
class FileModuleDescriptor: public Descriptor {
public:
    using Ptr = std::shared_ptr<FileModuleDescriptor>;
public:
    FileModuleDescriptor(const std::string& filename);
    
    FunctionDescriptor::Ptr constructor;
    std::vector<VariableDescriptor::Ptr> variables;
    std::vector<FunctionDescriptor::Ptr> functions;
    std::vector<ClassDescriptor::Ptr> classes;
};

#endif
