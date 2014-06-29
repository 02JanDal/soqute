include(CMakeParseArguments)

function(add_unit_test)
    set(options NO_TESTUTIL)
    set(oneValueArgs NAME)
    set(multiValueArgs QT ARGUMENTS)
    cmake_parse_arguments(AUT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(AUT_SOURCES "tst_${AUT_NAME}.cpp")
    if(NOT AUT_NO_TESTUTIL)
        list(APPEND AUT_SOURCES testutil.h testutil.cpp)
    endif()

    list(APPEND AUT_QT Core Test)

    add_executable(tst_${AUT_NAME} ${AUT_SOURCES})
    qt5_use_modules(tst_${AUT_NAME} ${AUT_QT})

    add_test(tst_${AUT_NAME} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tst_${AUT_NAME} ${AUT_ARGUMENTS})
    set_tests_properties(tst_${AUT_NAME} PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR")
endfunction()

add_unit_test(NAME blackbox_install_remove)

file(GLOB_RECURSE test_packages packages/*)
add_custom_target(tester_copy_data
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/test_meta.json ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_meta.json
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/packages ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/packages
    SOURCES test_meta.json ${test_packages}
)
add_dependencies(tst_blackbox_install_remove tester_copy_data)
