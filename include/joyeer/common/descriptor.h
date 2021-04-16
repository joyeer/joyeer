#ifndef __joyeer_common_descriptor_h__
#define __joyeer_common_descriptor_h__


#include <string>

//  interpretation of a Type
struct DescriptorType {
    static const char Bool = 'B';
    static const char Int = 'L';
    static const char Int64 = 'L';
    static const char UInt = 'l';
    static const char UInt64 = 'l';
    static const char Int32 = 'K';
    static const char UInt32 = 'k';
    static const char Int16 = 'H';
    static const char UInt16 = 'h';
    static const char Int8 = 'D';
    static const char UInt8 = 'd';
    static const char Int128 = 'X';
    static const char UInt128 = 'x';
    static const char Float = 'F';
    static const char Double = 'D';
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

// File Module descriptor, e.g.
// foo.joyeer, descriptor: #foo;
// bar/foo.joyeer, descriptor: #bar/foo;
class FileModuleDescriptor: public Descriptor {
public:
    using Ptr = std::shared_ptr<FileModuleDescriptor>;
public:
    FileModuleDescriptor(const std::string& filename);
};

class VariableDescriptor: public Descriptor {
public:
    VariableDescriptor(const std::string& filename);
};

class FunctionDescriptor: public Descriptor {
public:
    FunctionDescriptor(const std::string& filename);
};

class ClassDescriptor: public Descriptor {

};

#endif
