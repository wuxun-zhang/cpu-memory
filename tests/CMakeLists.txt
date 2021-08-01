file(GLOB SRC_FILE *.cpp)

foreach(src ${SRC_FILE})
    file(RELATIVE_PATH src_rel_path ${CMAKE_SOURCE_DIR}/tests ${src})
    string(REGEX REPLACE "[/_\\.]" "-" example_name ${src_rel_path})

    add_executable(${example_name} ${src})
    target_link_libraries(${example_name} PUBLIC
        MemoryLib)

    target_include_directories(${example_name} PUBLIC
        ${CMAKE_SOURCE_DIR}/include)

    add_test(${example_name} ${example_name})
endforeach()