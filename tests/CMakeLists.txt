cmake_minimum_required(VERSION 3.15)

# Test executable
add_executable(test_transpiler
    test_main.cpp
    test_type_mapping.cpp
    test_codegen.cpp
)

target_include_directories(test_transpiler PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(test_transpiler
    # Link against transpiler library components
)

# Add tests to CTest
add_test(NAME TypeMappingTests COMMAND test_transpiler --test-type-mapping)
add_test(NAME CodegenTests COMMAND test_transpiler --test-codegen)
