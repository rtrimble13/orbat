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

# Copy test data files
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

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

add_executable(test_expected_returns
    unit/test_expected_returns.cpp
)
target_link_libraries(test_expected_returns
    PRIVATE
        orbat
        GTest::gtest_main
)
gtest_discover_tests(test_expected_returns)

add_executable(test_covariance_matrix
    unit/test_covariance_matrix.cpp
)
target_link_libraries(test_covariance_matrix
    PRIVATE
        orbat
        GTest::gtest_main
)
gtest_discover_tests(test_covariance_matrix)

add_executable(test_constraint
    unit/test_constraint.cpp
)
target_link_libraries(test_constraint
    PRIVATE
        orbat
        GTest::gtest_main
)
gtest_discover_tests(test_constraint)

add_executable(test_markowitz
    unit/test_markowitz.cpp
)
target_link_libraries(test_markowitz
    PRIVATE
        orbat
        GTest::gtest_main
)
gtest_discover_tests(test_markowitz)

add_executable(test_efficient_frontier
    unit/test_efficient_frontier.cpp
)
target_link_libraries(test_efficient_frontier
    PRIVATE
        orbat
        GTest::gtest_main
)
gtest_discover_tests(test_efficient_frontier)

add_executable(test_black_litterman
    unit/test_black_litterman.cpp
)
target_link_libraries(test_black_litterman
    PRIVATE
        orbat
        GTest::gtest_main
)
gtest_discover_tests(test_black_litterman)
