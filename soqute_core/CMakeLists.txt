set(SRCS
    soqute_core_global.h

    configurationhandler.h
    configurationhandler.cpp
    dependencycalculator.h
    dependencycalculator.cpp
    package.h
    package.cpp
    packagematcher.h
    packagematcher.cpp
    abstractinstaller.h
    abstractinstaller.cpp
    installedpackages.h
    installedpackages.cpp
    util_core.h
    util_core.cpp
    downloader.h
    downloader.cpp
    loadmetadata.h
    loadmetadata.cpp
    jsinstaller.h
    jsinstaller.cpp
    remover.h
    remover.cpp
    json.h
    json.cpp
    filesystem.h
    filesystem.cpp
    cache.h
    cache.cpp
    exception.h
    actor.h
    actor.cpp
)

include_directories(${KARCHIVE_INCLUDE_DIR})

add_definitions(-DSOQUTE_CORE_LIBRARY)

add_library(soqute_core SHARED ${SRCS})
qt5_use_modules(soqute_core Core Network Qml)
target_link_libraries(soqute_core karchive)

add_custom_target(ide_extra_files SOURCES ../status.txt)
