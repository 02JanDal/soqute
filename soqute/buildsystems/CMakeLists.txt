set(SCRIPTS
    soqute.cmake
)

add_custom_target(buildscripts_dummy_target
    SOURCES ${SCRIPTS}
)

if(SOQUTE_BUILDSYSTEMS_ENABLE_TESTS)
    add_subdirectory(cmake_test)
endif()
