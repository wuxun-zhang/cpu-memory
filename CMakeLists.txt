cmake_minimum_required(VERSION 3.9)
message(STATUS "cmake verson: $CMAKE_VERSION")

project(cpu_memory LANGUAGES C CXX VERSION 0.1)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "Setting CMake_BUILD_TYPE to Release by default.")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

set(CMAKE_CXX_STANDARD 14)

enable_testing()

#add_subdirectory(src)
add_subdirectory(tests)
