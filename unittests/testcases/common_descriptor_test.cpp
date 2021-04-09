#include "joyeer/common/descriptor.h"
#include "gtest/gtest.h"


TEST(DescriptorTest, FileModileDescritpor) {
    auto descriptor = FileModuleDescriptor(std::string("foo.joyeer"));
    auto rawDescriptor = descriptor.getRawDescriptor();
    EXPECT_EQ(rawDescriptor, "#foo;");

    auto descriptor2 = FileModuleDescriptor(std::string("bar/foo.joyeer"));
    auto rawDescriptor2 = descriptor2.getRawDescriptor();
    EXPECT_EQ(rawDescriptor2, "#bar/foo;");
    
}
