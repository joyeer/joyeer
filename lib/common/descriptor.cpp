#include "joyeer/common/descriptor.h"
#include <sstream>
#include <filesystem>

PatternDescriptor::PatternDescriptor(Descriptor::Ptr label, Descriptor::Ptr type) {
    std::stringstream ss;
    ss << label->getRawDescriptor() << DescriptorConstants::Colon << type->getRawDescriptor();
    ss << DescriptorConstants::END;
    
    rawDescriptor = ss.str();
}

FunctionDescriptor::FunctionDescriptor(Descriptor::Ptr parent, const std::string& funcName, std::vector<PatternDescriptor::Ptr> parameters) {
    std::stringstream ss;
    ss << parent->getRawDescriptor() << DescriptorConstants::Separator << DescriptorConstants::FuncPrefix << funcName;
    
    ss << DescriptorConstants::ParenthesisOpen;
    for(auto parameter : parameters) {
        ss << parameter->getRawDescriptor();
    }
    ss << DescriptorConstants::ParenthesisClose;
    ss << DescriptorConstants::END;
    
    rawDescriptor = ss.str();
}

FileModuleDescriptor::FileModuleDescriptor(const std::string& filename) {
    std::stringstream ss;
    std::filesystem::path path = filename;
    path.replace_extension("");
    ss << DescriptorConstants::FileModulePrefix << path.u8string() << DescriptorConstants::END ;
    
    rawDescriptor = ss.str();
}

FileModuleInitializerDescriptor::FileModuleInitializerDescriptor(FileModuleDescriptor::Ptr parent):
filemodule(parent),
FunctionDescriptor(parent, std::string { DescriptorConstants::FileModuleInitializer }, {}){
    
}
