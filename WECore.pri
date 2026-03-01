LIBS+=-luser32
LIBS+=-lshell32

INCLUDEPATH += $$PWD
HEADERS +=                        \
    $$PWD/WFile/wpath.h           \
    $$PWD/WFile/wshellexecute.h   \
    $$PWD/WConfig/wconfig.h       \
    $$PWD/WPlugin/wplugindata.h
SOURCES +=                        \
    $$PWD/WFile/wpath.cpp         \
    $$PWD/WFile/wshellexecute.cpp \
    $$PWD/WPlugin/wplugindata.cpp
