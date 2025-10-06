cmake_minimum_required(VERSION 3.16)
project(fontconfig_viewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)
pkg_check_modules(FONTCONFIG REQUIRED fontconfig)

# Include directories
include_directories(${GTK4_INCLUDE_DIRS})
include_directories(${FONTCONFIG_INCLUDE_DIRS})

# Add executable
add_executable(fontconfig_viewer main.cpp fontconfig_viewer.cpp)

# Link libraries
target_link_libraries(fontconfig_viewer 
    ${GTK4_LIBRARIES} 
    ${FONTCONFIG_LIBRARIES}
)

# Compiler flags
target_compile_options(fontconfig_viewer PRIVATE ${GTK4_CFLAGS_OTHER})
target_compile_options(fontconfig_viewer PRIVATE ${FONTCONFIG_CFLAGS_OTHER})
