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

file(GLOB SRC_FILE tests/*.cpp)

foreach(src ${SRC_FILE})
    file(RELATIVE_PATH src_rel_path ${CMAKE_SOURCE_DIR}/tests ${src})
    string(REGEX REPLACE "[/_\\.]" "-" example_name ${src_rel_path})

    add_executable(${example_name} ${src})

    target_include_directories(${example_name} PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR})

    target_link_libraries(${example_name} PUBLIC)

    add_test(${example_name} ${example_name})
endforeach()
