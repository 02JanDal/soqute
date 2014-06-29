set(SRCS
    actor.h
    actor.cpp
    exception.h
    json.h
    json.cpp
    filesystem.h
    filesystem.cpp
)

add_library(common_lib STATIC ${SRCS})
qt5_use_modules(common_lib Core)
