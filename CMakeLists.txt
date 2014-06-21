cmake_minimum_required(VERSION 2.8.9)

project(soqute)

include(FeatureSummary)

find_package(Qt5 REQUIRED Core Network Qml)

set_package_properties(Qt5 PROPERTIES URL "http://qt-project.org/" PURPOSE "Required dependency for all of soqute")
set_package_properties(Qt5Core PROPERTIES URL "http://qt-project.org/" DESCRIPTION "Basic classes like strings, lists etc. for Qt")
set_package_properties(Qt5Qml PROPERTIES URL "http://qt-project.org/" DESCRIPTION "Parsing and execution of QML, a declarative Javascript/CSS like language" PURPOSE "Used for the Javascript engine")
set_package_properties(Qt5Network PROPERTIES URL "http://qt-project.org/" DESCRIPTION "The networking part of Qt" PURPOSE "Required for fetching metadata and downloading packages")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wextra -Werror -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/dest)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/dest)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/dest)

option(KARCHIVE_BUILD_PACKAGER "Set to ON to build the packager" OFF)
option(KARCHIVE_BUILD_TESTER "Set to ON to build the blackbox tester" OFF)
add_feature_info(Packager KARCHIVE_BUILD_PACKAGER "The Packager is a small program that creates soqute compatible packages")
add_feature_info(Tester KARCHIVE_BUILD_TESTER "The tester is a black-box testing program that goes through installing, updating and removing of packages")

add_subdirectory(3rd-party)
add_subdirectory(soqute_core)
add_subdirectory(soqute_cmd)
if(KARCHIVE_BUILD_TESTER)
    add_subdirectory(tester)
endif()
if(KARCHIVE_BUILD_PACKAGER)
    add_subdirectory(Packager)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
