KARCHIVE_SRC_DIR = $$PWD/karchive-qt4/src

HEADERS += \
$$KARCHIVE_SRC_DIR/k7zip.h \
$$KARCHIVE_SRC_DIR/karchive.h \
$$KARCHIVE_SRC_DIR/kar.h \
$$KARCHIVE_SRC_DIR/kbzip2filter.h \
$$KARCHIVE_SRC_DIR/kcompressiondevice.h \
$$KARCHIVE_SRC_DIR/kfilterbase.h \
$$KARCHIVE_SRC_DIR/kfilterdev.h \
$$KARCHIVE_SRC_DIR/kgzipfilter.h \
$$KARCHIVE_SRC_DIR/klimitediodevice_p.h  \
$$KARCHIVE_SRC_DIR/knonefilter.h \
$$KARCHIVE_SRC_DIR/ktar.h \
$$KARCHIVE_SRC_DIR/kxzfilter.h  \
$$KARCHIVE_SRC_DIR/kzip.h \
$$KARCHIVE_SRC_DIR/karchive_qt4_export.h


SOURCES += \
$$KARCHIVE_SRC_DIR/k7zip.cpp \
$$KARCHIVE_SRC_DIR/karchive.cpp \
$$KARCHIVE_SRC_DIR/kar.cpp \
$$KARCHIVE_SRC_DIR/kbzip2filter.cpp \
$$KARCHIVE_SRC_DIR/kcompressiondevice.cpp \
$$KARCHIVE_SRC_DIR/kfilterbase.cpp \
$$KARCHIVE_SRC_DIR/kfilterdev.cpp \
$$KARCHIVE_SRC_DIR/kgzipfilter.cpp \
$$KARCHIVE_SRC_DIR/klimitediodevice.cpp \
$$KARCHIVE_SRC_DIR/knonefilter.cpp \
$$KARCHIVE_SRC_DIR/ktar.cpp \
$$KARCHIVE_SRC_DIR/kxzfilter.cpp \
$$KARCHIVE_SRC_DIR/kzip.cpp

INCLUDEPATH += $$KARCHIVE_SRC_DIR
LIBS += -llzma -lz
