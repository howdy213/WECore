LIBS+=-luser32
LIBS+=-lshell32

INCLUDEPATH += $$PWD/include
HEADERS += \
    $$PWD/include/WECore/metadata/wevent.h \
    $$PWD/include/WECore/metadata/wmetadata.h \
    $$PWD/include/WECore/plugin/wplugindata.h \
    $$PWD/include/WECore/utils/flowlayout.h \
    $$PWD/include/WECore/utils/sidebar.h
SOURCES += \
    $$PWD/src/plugin/wplugindata.cpp \
    $$PWD/src/utils/flowlayout.cpp \
    $$PWD/src/utils/sidebar.cpp


