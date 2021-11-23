#include "joyeer/common/descriptor.h"
#include <sstream>
#include <filesystem>

PatternDescriptor::PatternDescriptor(Descriptor::Ptr label, Descriptor::Ptr type) {
    // full descriptor name
    std::stringstream ss;
    ss << label->getRawDescriptor() << DescriptorConstants::Colon << type->getRawDescriptor();
    ss << DescriptorConstants::END;
    rawDescriptor = ss.str();
}

FunctionDescriptor::FunctionDescriptor(Descriptor::Ptr parent, const std::string& funcName, std::vector<PatternDescriptor::Ptr> parameters) {
    // full descriptor name
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

ModuleDescriptor::ModuleDescriptor(const std::string& filename) {
    std::stringstream ss;
    std::filesystem::path path = filename;
    path.replace_extension("");
    ss << DescriptorConstants::ModulePrefix << path << DescriptorConstants::END ;
    
    rawDescriptor = ss.str();
}

ModuleInitializerDescriptor::ModuleInitializerDescriptor(ModuleDescriptor::Ptr parent):
        module(parent),
        FunctionDescriptor(parent, std::string { DescriptorConstants::ModuleInitializer }, {}){
    
}
