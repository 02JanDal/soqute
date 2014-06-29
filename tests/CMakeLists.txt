include(CMakeParseArguments)

function(add_unit_test)
    set(options NO_TESTUTIL LINK_TO_CORE)
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

    if(AUT_LINK_TO_CORE)
        target_link_libraries(tst_${AUT_NAME} soqute_core)
    endif()

    add_test(tst_${AUT_NAME} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tst_${AUT_NAME} ${AUT_ARGUMENTS})
    set_tests_properties(tst_${AUT_NAME} PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR")
endfunction()

add_unit_test(NAME blackbox_install_remove)
add_unit_test(NAME util_core LINK_TO_CORE)

include_directories(${CMAKE_SOURCE_DIR}/soqute/core)

file(GLOB_RECURSE test_packages packages/*)
add_custom_target(tester_copy_data
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/test_meta.json ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_meta.json
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/packages ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/packages
    SOURCES test_meta.json ${test_packages}
)
add_dependencies(tst_blackbox_install_remove tester_copy_data)
