#
# CMake file for automatically downloading and installing required packages using soqute
#
# Usage:
#
#    soqute_ensure_available([FAIL_DIRECT]
#                            [PLATFORM <platform>]
#                            [VERSION <version>]
#                            [REPOSITORY <repository>]
#                            [PACKAGES <package1> [<package2> ...]]
#                            )
#
#    If FAIL_DIRECT is set errors will invoke a FATAL_ERROR, thus terminating CMake. PLATFORM can be used to override the platform, none
#    given or empty means use default. VERSION can be used to set a default version for all packages, none given or empty means use latest.
#    Both PLATFORM and VERSION can be overriden on a per-package basis. REPOSITORY can be used to tell soqute to use an extra repository,
#    the given repository will be searched before system-defined repositories. PACKAGES is a list of packages to install, in the format
#           <name>[/<version>][#<platform>]
#    , where <name> is required and <version> and <platform> are optional. They can be used to override the default (either as given using
#    PLATFORM resp. VERSION or if they aren't given either; the current platform resp. the latest version).
#
#
# TODO:
#    * Integrate with find_package
#

include(CMakeParseArguments)

find_program(SOQUTE_EXECUTABLE soqute_cmd)

function(soqute_ensure_available)
    # setup parsing and parse the arguments
    set(options FAIL_DIRECT)
    set(oneValueArgs PLATFORM VERSION REPOSITORY __OUTDIR)
    set(multiValueArgs PACKAGES)
    cmake_parse_arguments(SEA "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # don't go any further unless we found soqute_cmd
    if(SOQUTE_EXECUTABLE STREQUAL "SOQUTE_EXECUTABLE-NOTFOUND")
        set(message "Couldn't find the soqute_cmd executable. Ensure it's either in your PATH or in CMAKE_PREFIX_PATH")
        if(${SEA_FAIL_DIRECT})
            message(FATAL_ERROR ${message})
        else()
            message(WARNING ${message})
        endif()
        return()
    endif()

    # make sure buildsystem installation is enabled
    execute_process(
        COMMAND ${SOQUTE_EXECUTABLE} buildsystem --defaultplatform="${SEA_PLATFORM}" --defaultversion="${SEA_VERSION}" --repository="${SEA_REPOSITORY}" --outdir="${SEA___OUTDIR}" "${SEA_PACKAGES}"
        WORKING_DIRECTORY ${CMAKE_ROOT}
        RESULT_VARIABLE test_result
        ERROR_VARIABLE test_error_output
    )

    if(${test_result} EQUAL 0)
        message(STATUS "soqute: Successfully installed ${test_error_output}")
        return()
    elseif(${test_result} EQUAL 1)
        set(message "soqute: Error installing packages: ${test_error_output}")
    elseif(${test_result} EQUAL 2)
        set(message "soqute: You need to enable buildsystem installations in soqute. Try ${SOQUTE_EXECUTABLE} config -s allow-build-system-install true")
    endif()
    if(${SEA_FAIL_DIRECT})
        message(FATAL_ERROR ${message})
    else()
        message(WARNING ${message})
    endif()
endfunction()
