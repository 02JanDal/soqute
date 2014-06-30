cmake_minimum_required(VERSION 2.8.9)

project(soqute)
enable_testing()

include(FeatureSummary)
include(CMakeDependentOption)

find_package(Qt5 REQUIRED Core Network Qml OPTIONAL_COMPONENTS Test)

set_package_properties(Qt5 PROPERTIES URL "http://qt-project.org/" PURPOSE "Required dependency for all of soqute")
set_package_properties(Qt5Core PROPERTIES URL "http://qt-project.org/" DESCRIPTION "Basic classes like strings, lists etc. for Qt")
set_package_properties(Qt5Qml PROPERTIES URL "http://qt-project.org/" DESCRIPTION "Parsing and execution of QML, a declarative Javascript/CSS like language" PURPOSE "Used for the Javascript engine")
set_package_properties(Qt5Network PROPERTIES URL "http://qt-project.org/" DESCRIPTION "The networking part of Qt" PURPOSE "Required for fetching metadata and downloading packages")
set_package_properties(Qt5Test PROPERTIES URL "http://qt-project.org/" DESCRIPTION "Qt Test is a framework for unit testing Qt based applications and libraries" PURPOSE "Needed for building unit tests")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wextra -Wall -Werror -std=c++11 -fPIC")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11 -fPIC")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/dest)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/dest)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/dest)

option(SOQUTE_BUILD_SOQUTE "Decides whether or not to build soqute" ON)
cmake_dependent_option(SOQUTE_BUILD_PACKAGER "Set to ON to build the packager" OFF "SOQUTE_BUILD_SOQUTE" OFF)
cmake_dependent_option(SOQUTE_BUILD_TESTS "Set to ON to build tests" OFF "SOQUTE_BUILD_SOQUTE" OFF)
cmake_dependent_option(SOQUTE_INSTALL_BUILDSYSTEM_SCRIPTS "Set to ON to install scripts for buildsystems" OFF "SOQUTE_BUILD_SOQUTE" OFF)
cmake_dependent_option(SOQUTE_BUILDSYSTEMS_ENABLE_TESTS "Enable buildsystem tests" OFF "SOQUTE_BUILD_SOQUTE" OFF)
option(SOQUTE_BUILD_WAND "Decides whether or not to build wand" ON)

if(NOT Qt5Test_FOUND)
    set(SOQUTE_BUILD_TESTS OFF)
endif()

add_feature_info(soqute SOQUTE_BUILD_SOQUTE "Builds soqute, the package installer")
add_feature_info(wand SOQUTE_BUILD_WAND "Builds wand, the source builder")
add_feature_info(Packager SOQUTE_BUILD_PACKAGER "The Packager is a small program that creates soqute compatible packages")
add_feature_info(Tests SOQUTE_BUILD_TESTS "Builds unit and black-box tests")
add_feature_info(Buildsystems SOQUTE_INSTALL_BUILDSYSTEM_SCRIPTS "Installs scripts for some common build systems for easily making sure that all required packages are available")
add_feature_info(BuildsystemTests SOQUTE_BUILDSYSTEMS_ENABLE_TESTS "Runs buildsystem tests. Only enable if you know what you do")

add_subdirectory(3rd-party)
add_subdirectory(common)
if(SOQUTE_BUILD_SOQUTE)
    add_subdirectory(soqute/core)
    add_subdirectory(soqute/cmd)
endif()
if(SOQUTE_BUILD_WAND)
    add_subdirectory(wand/core)
    add_subdirectory(wand/cmd)
endif()
if(SOQUTE_BUILD_TESTS)
    add_subdirectory(tests)
endif()
if(SOQUTE_BUILD_PACKAGER)
    add_subdirectory(soqute/Packager)
endif()
if(SOQUTE_INSTALL_BUILDSYSTEM_SCRIPTS)
    add_subdirectory(soqute/buildsystems)
endif()

add_custom_target(ide_extra_files SOURCES status.txt)

add_custom_target(static_analyzers)

# krazy2
find_program(KRAZY2_EXECUTABLE krazy2)
if(KRAZY2_EXECUTABLE)
    file(GLOB_RECURSE soqute_hdrs ${CMAKE_SOURCE_DIR}/soqute/*.h)
    file(GLOB_RECURSE soqute_srcs ${CMAKE_SOURCE_DIR}/soqute/*.cpp)
    file(GLOB_RECURSE wand_hdrs ${CMAKE_SOURCE_DIR}/wand/*.h)
    file(GLOB_RECURSE wand_srcs ${CMAKE_SOURCE_DIR}/wand/*.cpp)
    file(GLOB_RECURSE cmake_files ${CMAKE_SOURCE_DIR}/*CMakeLists.txt)
    add_custom_target(krazy2
        COMMAND ${KRAZY2_EXECUTABLE} --brief ${cmake_files}
        COMMAND ${KRAZY2_EXECUTABLE} --brief --check-sets c++,qt5 ${soqute_hdrs} ${soqute_srcs} ${wand_hdrs} ${wand_srcs}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running static code analyzer (krazy2)"
    )
    add_dependencies(static_analyzers krazy2)
endif()

# cppcheck
find_program(CPPCHECK_EXECUTABLE cppcheck)
if(CPPCHECK_EXECUTABLE)
    set(cppcheck_flags "-I${CMAKE_CURRENT_BINARY_DIR}")
    get_directory_property(inc_dirs INCLUDE_DIRECTORIES)
    foreach(inc_dir ${inc_dirs})
        set(cppcheck_flags ${cppcheck_flags} "-I{inc_dir}")
    endforeach()
    get_directory_property(defs COMPILE_DEFINITIONS)
    foreach(def ${defs})
        set(cppcheck_flags ${cppcheck_flags} "-D{def}")
    endforeach()
    add_custom_target(cppcheck
        COMMAND ${CPPCHECK_EXECUTABLE} --enable=all --std=c++11 ${cppcheck_flags} -I${CMAKE_SOURCE_DIR}/common -I${CMAKE_SOURCE_DIR}/soqute/core ${CMAKE_SOURCE_DIR}/wand/core -i${CMAKE_SOURCE_DIR}/3rd-party -i${CMAKE_SOURCE_DIR}/build -i${CMAKE_SOURCE_DIR}/clangbuild ${CMAKE_SOURCE_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running static code analyzer (cppcheck)"
    )
    add_dependencies(static_analyzers cppcheck)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
