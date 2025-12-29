cmake_minimum_required(VERSION 3.14)

# Fetch Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()
include(GoogleTest)

# Test executables
add_executable(test_vector
    unit/test_vector.cpp
)
target_link_libraries(test_vector
    PRIVATE
        orbat
        GTest::gtest_main
)
gtest_discover_tests(test_vector)

add_executable(test_matrix
    unit/test_matrix.cpp
)
target_link_libraries(test_matrix
    PRIVATE
        orbat
        GTest::gtest_main
)
gtest_discover_tests(test_matrix)
