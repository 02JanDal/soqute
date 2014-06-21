add_executable(tester main.cpp)
qt5_use_modules(tester Core)

add_custom_target(tester_copy_data
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/test_meta.json ${CMAKE_CURRENT_BINARY_DIR}/test_meta.json
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/packages ${CMAKE_CURRENT_BINARY_DIR}/packages
    SOURCES test_meta.json
)
add_dependencies(tester tester_copy_data)
