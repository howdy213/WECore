QT -= gui

TARGET = wecore
TEMPLATE = lib

DEFINES += WE_LIBRARY

LIBS += -luser32 -lshell32

win32 {
    CONFIG(debug, debug|release) {
        DESTDIR  = ../../src/debug
    } else {
        DESTDIR  = ../../src/release
    }
}

include(WECore.pri)
HEADERS +=                      \
    wconfig.h                   \
    wdef.h                      \
    wfile.h                     \
    wplugin.h                   \
    WE/we.h                     \
    WE/webase.h                 \
    WE/webasedata.h             \
    WE/weclass.h                \
    WDef/colordef.h             \
    WDef/wedef.h                \
    WConfig/wconfigdocument.h   \
    WPlugin/wplugin.h           \
    WPlugin/wplugininterface.h  \
    WPlugin/wpluginmanager.h    \
    WPlugin/wpluginmetadata.h   \
    WPlugin/wpluginproxy.h      \
    WPlugin/wvirtualplugin.h    \
    WPlugin/wwidget.h           \
    WPlugin/wwidgetmanager.h
SOURCES +=                      \
    WE/weclass.cpp              \
    WE/webase.cpp               \
    WE/webasedata.cpp           \
    WConfig/wconfigdocument.cpp \
    WE/we.cpp                   \
    WPlugin/wplugin.cpp         \
    WPlugin/wpluginmanager.cpp  \
    WPlugin/wvirtualplugin.cpp  \
    WPlugin/wwidget.cpp         \
    WPlugin/wwidgetmanager.cpp  \
    WPlugin/wpluginproxy.cpp
