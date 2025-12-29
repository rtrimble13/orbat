cmake_minimum_required(VERSION 3.14)

project(orbat
    VERSION 0.1.0
    DESCRIPTION "Modern C++ portfolio optimization library"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add compiler warnings
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Build options
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_CLI "Build command-line interface" OFF)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Library target (header-only for now since we only have template/inline code)
add_library(orbat INTERFACE)
target_include_directories(orbat INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(orbat INTERFACE cxx_std_20)

# Add subdirectories
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_CLI)
    add_subdirectory(src)
endif()

# Installation rules
install(DIRECTORY include/ DESTINATION include)
install(TARGETS orbat EXPORT orbatTargets)
install(EXPORT orbatTargets
    FILE orbatTargets.cmake
    NAMESPACE orbat::
    DESTINATION lib/cmake/orbat
)
