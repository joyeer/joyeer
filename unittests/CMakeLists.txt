include(ExternalProject)

# Download and install GoogleTest
ExternalProject_Add(
    gtest
    URL https://github.com/google/googletest/archive/master.zip
    DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}/gtest/download
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/gtest/install
    CMAKE_ARGS -D CMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)

# Get GTest source and binary directories from CMake project
ExternalProject_Get_Property(gtest source_dir binary_dir install_dir)

# Create a libgtest target to be used as a dependency by test programs
add_library(libgtest IMPORTED STATIC GLOBAL)

add_dependencies(libgtest gtest)

# Set libgtest properties
set_target_properties(libgtest PROPERTIES
    "IMPORTED_LOCATION" "${install_dir}/lib/libgtest.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

# Create a libgmock target to be used as a dependency by test programs
add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gtest)

# Set libgmock properties
set_target_properties(libgmock PROPERTIES
    "IMPORTED_LOCATION" "${install_dir}/lib/libgmock.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

include_directories("${source_dir}/googletest/include"
                    "${source_dir}/googlemock/include")

add_subdirectory(testcases)
