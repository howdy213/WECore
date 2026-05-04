# @file WECore.pro
# @brief Qt project file for the WECore shared library.
#
# Builds the core framework library (wecore) with all plugin, widget,
# service, and configuration components.
#
# @author howdy213
# @date 2026-05-01
# @version 2.0.0
#
# Copyright 2025-2026 howdy213
# Licensed under the Apache License, Version 2.0

TARGET = wecore
TEMPLATE = lib

# Export the WE_EXPORT macro when building the library.
DEFINES += WE_LIBRARY

LIBS += -luser32 -lshell32

win32 {
    CONFIG(debug, debug|release) {
        DESTDIR = ../../src/debug
    } else {
        DESTDIR = ../../src/release
    }
}

INCLUDEPATH += include
include(WECore.pri)

HEADERS += \
    include/WECore/def/wedef.h \
    include/WECore/file/wpath.h           \
    include/WECore/file/wshellexecute.h   \
    include/WECore/metadata/wmetadata.h \
    include/WECore/metadata/wmetadocument.h \
    include/WECore/plugin/wplugin.h \
    include/WECore/plugin/wplugininterface.h \
    include/WECore/plugin/wpluginmanager.h \
    include/WECore/plugin/wpluginmessage.h \
    include/WECore/plugin/wpluginproxy.h \
    include/WECore/plugin/wvirtualplugin.h \
    include/WECore/service/wserviceregistry.h \
    include/WECore/service/wserviceproxy.h \
    include/WECore/we/we.h \
    include/WECore/we/webase.h \
    include/WECore/we/webasedata.h \
    include/WECore/we/weclass.h \
    include/WECore/widget/wwidgetmanager.h

SOURCES += \
    src/metadata/wmetadocument.cpp \
    src/file/wpath.cpp \
    src/file/wshellexecute.cpp \
    src/plugin/wplugin.cpp \
    src/plugin/wpluginmanager.cpp \
    src/plugin/wpluginproxy.cpp \
    src/plugin/wvirtualplugin.cpp \
    src/service/wserviceregistry.cpp \
    src/service/wserviceproxy.cpp \
    src/we/we.cpp \
    src/we/webase.cpp \
    src/we/webasedata.cpp \
    src/we/weclass.cpp \
    src/widget/wwidgetmanager.cpp