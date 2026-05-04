LIBS+=-luser32
LIBS+=-lshell32

INCLUDEPATH += $$PWD/include
HEADERS += \
    $$PWD/include/WECore/metadata/wevent.h \
    $$PWD/include/WECore/metadata/wmetadata.h \
    $$PWD/include/WECore/plugin/wplugindata.h
SOURCES += \
    $$PWD/src/plugin/wplugindata.cpp


