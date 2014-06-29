set(SRCS
    main.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/wand/core)

add_executable(wand_cmd ${SRCS})
qt5_use_modules(wand_cmd Core Network)
target_link_libraries(wand_cmd wand_core)
