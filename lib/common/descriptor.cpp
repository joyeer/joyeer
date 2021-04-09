#include "joyeer/common/descriptor.h"
#include <sstream>
#include <filesystem>

FileModuleDescriptor::FileModuleDescriptor(const std::string& filename) {
    std::stringstream ss;
    ss << DescriptorType::FileModulePrefix << filename << DescriptorType::END ;
    rawDescriptor = ss.str();
}
