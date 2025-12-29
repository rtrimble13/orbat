cmake_minimum_required(VERSION 3.14)

# Linear algebra demo
add_executable(linear_algebra_demo
    linear_algebra_demo.cpp
)
target_link_libraries(linear_algebra_demo
    PRIVATE
        orbat
)
