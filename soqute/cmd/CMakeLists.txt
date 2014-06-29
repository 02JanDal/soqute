set(SRCS
    main.cpp

    basecommand.h
    basecommand.cpp
    searchcommand.h
    searchcommand.cpp
    configcommand.h
    configcommand.cpp
    showcommand.h
    showcommand.cpp
    installcommand.h
    installcommand.cpp
    installcommandbase.h
    installcommandbase.cpp
    util_cmd.h
    util_cmd.cpp
    textstream.h
    textstream.cpp
    removecommand.h
    removecommand.cpp
    updatecommand.h
    updatecommand.cpp
    buildsystemcommand.h
    buildsystemcommand.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/soqute/core)
include_directories(${CMAKE_SOURCE_DIR}/common)

add_executable(soqute_cmd ${SRCS})
qt5_use_modules(soqute_cmd Core Network)
target_link_libraries(soqute_cmd soqute_core)
