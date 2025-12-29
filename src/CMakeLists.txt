cmake_minimum_required(VERSION 3.14)

# CLI executable
add_executable(orbat-cli
    cli/main.cpp
)

target_link_libraries(orbat-cli
    PRIVATE
        orbat
)

target_include_directories(orbat-cli
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Install the CLI executable
install(TARGETS orbat-cli
    RUNTIME DESTINATION bin
)
