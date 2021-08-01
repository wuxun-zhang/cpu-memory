if(SRC_cmake_included)
    return()
endif()
set(SRC_cmake_included true)

FILE(GLOB_RECURSE SRC_FILES *.cpp)

add_library(MemoryLib STATIC ${SRC_FILES})
target_include_directories(MemoryLib PUBLIC
    ${CMAKE_SOURCE_DIR}/include)
