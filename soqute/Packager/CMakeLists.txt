set(SRCS
    main.cpp
    filegetters.cpp
    filegetters.h
)

include_directories(${KARCHIVE_INCLUDE_DIR})

add_executable(Packager ${SRCS})
qt5_use_modules(Packager Core)
target_link_libraries(Packager karchive)
