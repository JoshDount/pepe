cmake_minimum_required(VERSION 3.20)
project(RouteTransitSimulator VERSION 1.0.0 LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific settings
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20 /EHsc /W4")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Wextra -Wpedantic")
endif()

# Find required packages
find_package(Threads REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

# Core library - all data structures and algorithms
add_subdirectory(core)

# I/O module - data loaders and parsers
add_subdirectory(io)

# Engine module - routing and traffic simulation
add_subdirectory(engine)

# Visualization module
add_subdirectory(viz)

# Benchmarking module
add_subdirectory(bench)

# CLI application
add_subdirectory(cli)

# Tests
enable_testing()
add_subdirectory(tests)

# Documentation (optional - only if docs directory has implementation)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/docs/CMakeLists.txt")
    add_subdirectory(docs)
endif()