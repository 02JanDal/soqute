# some common things for all projects

QMAKE_CFLAGS_DEBUG += -Wextra -Werror
QMAKE_CFLAGS_RELEASE += -Wextra -Werror
QMAKE_CXXFLAGS += -Wextra -Werror

strip.commands = strip $$OUT_PWD/$$TARGET
strip.target = strip_files

QMAKE_EXTRA_TARGETS += strip
CONFIG(release, debug|release):QMAKE_POST_LINK += make strip_files
