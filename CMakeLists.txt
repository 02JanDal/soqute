cmake_minimum_required(VERSION 2.8.9)

project(soqute)
enable_testing()

include(FeatureSummary)

find_package(Qt5 REQUIRED Core Network Qml OPTIONAL_COMPONENTS Test)

set_package_properties(Qt5 PROPERTIES URL "http://qt-project.org/" PURPOSE "Required dependency for all of soqute")
set_package_properties(Qt5Core PROPERTIES URL "http://qt-project.org/" DESCRIPTION "Basic classes like strings, lists etc. for Qt")
set_package_properties(Qt5Qml PROPERTIES URL "http://qt-project.org/" DESCRIPTION "Parsing and execution of QML, a declarative Javascript/CSS like language" PURPOSE "Used for the Javascript engine")
set_package_properties(Qt5Network PROPERTIES URL "http://qt-project.org/" DESCRIPTION "The networking part of Qt" PURPOSE "Required for fetching metadata and downloading packages")
set_package_properties(Qt5Test PROPERTIES URL "http://qt-project.org/" DESCRIPTION "Qt Test is a framework for unit testing Qt based applications and libraries" PURPOSE "Needed for building unit tests")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wextra -Wall -Werror -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/dest)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/dest)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/dest)

option(SOQUTE_BUILD_PACKAGER "Set to ON to build the packager" OFF)
option(SOQUTE_BUILD_TESTS "Set to ON to build tests" OFF)
option(SOQUTE_INSTALL_BUILDSYSTEM_SCRIPTS "Set to ON to install scripts for buildsystems" OFF)
option(SOQUTE_BUILDSYSTEMS_ENABLE_TESTS "Enable buildsystem tests" OFF)

if(NOT Qt5Test_FOUND)
    set(SOQUTE_BUILD_TESTS OFF)
endif()

add_feature_info(Packager SOQUTE_BUILD_PACKAGER "The Packager is a small program that creates soqute compatible packages")
add_feature_info(Tests SOQUTE_BUILD_TESTS "Builds unit and black-box tests")
add_feature_info(Buildsystems SOQUTE_INSTALL_BUILDSYSTEM_SCRIPTS "Installs scripts for some common build systems for easily making sure that all required packages are available")
add_feature_info(BuildsystemTests SOQUTE_BUILDSYSTEMS_ENABLE_TESTS "Runs buildsystem tests. Only enable if you know what you do")

add_subdirectory(3rd-party)
add_subdirectory(soqute_core)
add_subdirectory(soqute_cmd)
if(SOQUTE_BUILD_TESTS)
    add_subdirectory(tests)
endif()
if(SOQUTE_BUILD_PACKAGER)
    add_subdirectory(Packager)
endif()
if(SOQUTE_INSTALL_BUILDSYSTEM_SCRIPTS)
    add_subdirectory(buildsystems)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
