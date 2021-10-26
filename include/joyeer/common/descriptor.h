#ifndef __joyeer_common_descriptor_h__
#define __joyeer_common_descriptor_h__


#include <string>
#include <vector>

//  interpretation of a Type
struct DescriptorConstants {
    static constexpr char Bool = 'B';
    static constexpr char Int = 'L';
    static constexpr char UInt = 'l';
    static constexpr char Int8 = 'D';
    static constexpr char UInt8 = 'd';
    static constexpr char Int16 = 'H';
    static constexpr char UInt16 = 'h';
    static constexpr char Int32 = 'K';
    static constexpr char UInt32 = 'k';
    static constexpr char Int128 = 'X';
    static constexpr char UInt128 = 'x';
    static constexpr char Float = 'F';
    static constexpr char Double = 'O';
    static constexpr char Float128 = 'W';
    static constexpr char String = 'S';
    static constexpr char Void = 'V';

    // descriptor prefix of class/interface/func/file module/array/variable
    static constexpr char ClassPrefix = '^';
    static constexpr char InterfacePrefix = '~';
    static constexpr char FuncPrefix = '&';
    static constexpr char ArrayPrefix = '[';
    static constexpr char FileModulePrefix = '#';
    static constexpr char VariablePrefix = '@';

    // punctuation
    static constexpr char Colon = ':' ;
    static constexpr char ParenthesisOpen = '(';
    static constexpr char ParenthesisClose = ')';
    static constexpr char Separator = '/';
    
    // constants
    static constexpr std::string_view FileModuleInitializer = "fileModule-initializer";
    static constexpr std::string_view Constructor = "initializer";
    
    static constexpr char END = ';' ;
};

class Descriptor {
public:
    using Ptr = std::shared_ptr<Descriptor>;
    virtual const std::string getRawDescriptor() const { return rawDescriptor; }
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

DECLARE_DESCRIPTOR(Bool,        DescriptorConstants::Bool)
DECLARE_DESCRIPTOR(Int,         DescriptorConstants::Int)
DECLARE_DESCRIPTOR(UInt,        DescriptorConstants::UInt)
DECLARE_DESCRIPTOR(Int8,        DescriptorConstants::Int8)
DECLARE_DESCRIPTOR(UInt8,       DescriptorConstants::UInt8)
DECLARE_DESCRIPTOR(Int16,       DescriptorConstants::Int16)
DECLARE_DESCRIPTOR(UInt16,      DescriptorConstants::UInt16)
DECLARE_DESCRIPTOR(Int32,       DescriptorConstants::Int32)
DECLARE_DESCRIPTOR(UInt32,      DescriptorConstants::UInt32)
DECLARE_DESCRIPTOR(Int128,      DescriptorConstants::Int128)
DECLARE_DESCRIPTOR(UInt128,     DescriptorConstants::UInt128)
DECLARE_DESCRIPTOR(Float,       DescriptorConstants::Float)
DECLARE_DESCRIPTOR(Double,      DescriptorConstants::Double)
DECLARE_DESCRIPTOR(Float128,    DescriptorConstants::Float128)
DECLARE_DESCRIPTOR(String,      DescriptorConstants::String)
DECLARE_DESCRIPTOR(Void,        DescriptorConstants::Void)

// Represent an Variable
class VariableDescriptor: public Descriptor {
public:
    VariableDescriptor(Descriptor::Ptr parent, const std::string& name);
    
    Descriptor::Ptr parentDescriptor;
    Descriptor::Ptr typeDescriptor = nullptr;
};

// Pattern descriptor, e.g  foo: Int
class PatternDescriptor: public Descriptor {
public:
    using Ptr = std::shared_ptr<PatternDescriptor>;
public:
    PatternDescriptor(Descriptor::Ptr label, Descriptor::Ptr type);
    
    Descriptor::Ptr label;
    Descriptor::Ptr type;
};

// Function descriptor
class FunctionDescriptor: public Descriptor {
public:
    FunctionDescriptor(Descriptor::Ptr parent, const std::string& funcName, std::vector<PatternDescriptor::Ptr> parameters);
};


class ClassDescriptor: public Descriptor {

};

// File FileModule descriptor, e.g.
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

// FileModule initializer function descriptor
// foo.joyeer, FileModuleInitializer
class FileModuleInitializerDescriptor: public FunctionDescriptor {
public:
    using Ptr = std::shared_ptr<FileModuleInitializerDescriptor>();
public:
    FileModuleInitializerDescriptor(FileModuleDescriptor::Ptr parent);
    
    FileModuleDescriptor::Ptr filemodule;
};


#endif
