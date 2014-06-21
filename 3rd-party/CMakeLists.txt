set(KARCHIVE_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/karchive-qt4/src")

set(KARCHIVE_SRCS
    ${KARCHIVE_SRC_DIR}/karchive.h
    ${KARCHIVE_SRC_DIR}/karchive.cpp
    ${KARCHIVE_SRC_DIR}/kar.h
    ${KARCHIVE_SRC_DIR}/kar.cpp
    ${KARCHIVE_SRC_DIR}/kcompressiondevice.h
    ${KARCHIVE_SRC_DIR}/kcompressiondevice.cpp
    ${KARCHIVE_SRC_DIR}/kfilterbase.h
    ${KARCHIVE_SRC_DIR}/kfilterbase.cpp
    ${KARCHIVE_SRC_DIR}/kfilterdev.h
    ${KARCHIVE_SRC_DIR}/kfilterdev.cpp
    ${KARCHIVE_SRC_DIR}/kgzipfilter.h
    ${KARCHIVE_SRC_DIR}/kgzipfilter.cpp
    ${KARCHIVE_SRC_DIR}/klimitediodevice_p.h
    ${KARCHIVE_SRC_DIR}/klimitediodevice.cpp
    ${KARCHIVE_SRC_DIR}/knonefilter.h
    ${KARCHIVE_SRC_DIR}/knonefilter.cpp
    ${KARCHIVE_SRC_DIR}/ktar.h
    ${KARCHIVE_SRC_DIR}/ktar.cpp
    ${KARCHIVE_SRC_DIR}/kzip.h
    ${KARCHIVE_SRC_DIR}/kzip.cpp
    ${KARCHIVE_SRC_DIR}/karchive_qt4_export.h
)

set(KARCHIVE_INCLUDE_DIR ${KARCHIVE_SRC_DIR} PARENT_SCOPE)

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIR})
set_package_properties(ZLIB PROPERTIES
  URL "http://www.zlib.net"
  DESCRIPTION "Support for gzip compressed files and data streams"
  PURPOSE "Required by the core KDE libraries and some criti cal kioslaves"
)

include(${KARCHIVE_SRC_DIR}/ConfigureChecks.cmake)

if(BZIP2_FOUND)
    include_directories(${BZIP2_INCLUDE_DIR})
    set(KARCHIVE_SRCS
        ${KARCHIVE_SRCS}
        ${KARCHIVE_SRC_DIR}/kbzip2filter.h
        ${KARCHIVE_SRC_DIR}/kbzip2filter.cpp
    )
    set(KARCHIVE_LIBS ${KARCHIVE_LIBS} ${BZIP2_LIBRARIES})
endif()

if(LIBLZMA_FOUND)
    include_directories(${LIBLZMA_INCLUDE_DIRS})
    set(KARCHIVE_SRCS
        ${KARCHIVE_SRCS}
        ${KARCHIVE_SRC_DIR}/kxzfilter.h
        ${KARCHIVE_SRC_DIR}/kxzfilter.cpp
        ${KARCHIVE_SRC_DIR}/k7zip.h
        ${KARCHIVE_SRC_DIR}/k7zip.cpp
    )
    set(KARCHIVE_LIBS ${KARCHIVE_LIBS} ${LIBLZMA_LIBRARIES})
endif()

configure_file(${KARCHIVE_SRC_DIR}/config-compression.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-compression.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${KARCHIVE_SRC_DIR})

add_library(karchive STATIC ${KARCHIVE_SRCS})
qt5_use_modules(karchive Core)
target_link_libraries(karchive ${KARCHIVE_LIBS} z)
