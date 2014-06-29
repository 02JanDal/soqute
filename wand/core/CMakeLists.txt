set(SRCS
)

include_directories(${KARCHIVE_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/common)

add_library(wand_core SHARED ${SRCS})
qt5_use_modules(wand_core Core Network Qml)
target_link_libraries(wand_core karchive common_lib)
