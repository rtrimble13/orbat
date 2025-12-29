cmake_minimum_required(VERSION 3.14)

# Copy test data files for examples to use
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../tests/data
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Linear algebra demo
add_executable(linear_algebra_demo
    linear_algebra_demo.cpp
)
target_link_libraries(linear_algebra_demo
    PRIVATE
        orbat
)

# CMA inputs demo
add_executable(cma_inputs_demo
    cma_inputs_demo.cpp
)
target_link_libraries(cma_inputs_demo
    PRIVATE
        orbat
)

# Constraints demo
add_executable(constraints_demo
    constraints_demo.cpp
)
target_link_libraries(constraints_demo
    PRIVATE
        orbat
)
