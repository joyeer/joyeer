#include "joyeer/common/descriptor.h"
#include <sstream>
#include <filesystem>

FileModuleDescriptor::FileModuleDescriptor(const std::string& filename) {
    std::stringstream ss;
    std::filesystem::path path = filename;
    path.replace_extension("");
    ss << DescriptorType::FileModulePrefix << path.u8string() << DescriptorType::END ;
    rawDescriptor = ss.str();
}
