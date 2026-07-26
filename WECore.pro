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
QT += widgets
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
    include/WECore/config/WConfig.h \
    include/WECore/config/WConfigDataAction.h \
    include/WECore/config/WConfigDataArray.h \
    include/WECore/config/WConfigDataBase.h \
    include/WECore/config/WConfigDataBool.h \
    include/WECore/config/WConfigDataDef.h \
    include/WECore/config/WConfigDataDouble.h \
    include/WECore/config/WConfigDataInt.h \
    include/WECore/config/WConfigDataObject.h \
    include/WECore/config/WConfigDataSelect.h \
    include/WECore/config/WConfigDataString.h \
    include/WECore/config/WConfigDef.h \
    include/WECore/config/WConfigDocument.h \
    include/WECore/config/WConfigEditorAction.h \
    include/WECore/config/WConfigEditorArray.h \
    include/WECore/config/WConfigEditorBase.h \
    include/WECore/config/WConfigEditorBool.h \
    include/WECore/config/WConfigEditorDef.h \
    include/WECore/config/WConfigEditorDouble.h \
    include/WECore/config/WConfigEditorInt.h \
    include/WECore/config/WConfigEditorObject.h \
    include/WECore/config/WConfigEditorSelect.h \
    include/WECore/config/WConfigEditorString.h \
    include/WECore/config/WConfigItemInfo.h \
    include/WECore/config/WConfigItemWidget.h \
    include/WECore/config/WConfigRef.h \
    include/WECore/config/WConfigTemplate.h \
    include/WECore/config/WConfigViewer.h \
    include/WECore/config/WConfigWidget.h \
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
    src/config/WConfig.cpp \
    src/config/WConfigDataAction.cpp \
    src/config/WConfigDataArray.cpp \
    src/config/WConfigDataBase.cpp \
    src/config/WConfigDataBool.cpp \
    src/config/WConfigDataDouble.cpp \
    src/config/WConfigDataInt.cpp \
    src/config/WConfigDataObject.cpp \
    src/config/WConfigDataSelect.cpp \
    src/config/WConfigDataString.cpp \
    src/config/WConfigDef.cpp \
    src/config/WConfigDocument.cpp \
    src/config/WConfigEditorAction.cpp \
    src/config/WConfigEditorArray.cpp \
    src/config/WConfigEditorBase.cpp \
    src/config/WConfigEditorBool.cpp \
    src/config/WConfigEditorDouble.cpp \
    src/config/WConfigEditorInt.cpp \
    src/config/WConfigEditorObject.cpp \
    src/config/WConfigEditorSelect.cpp \
    src/config/WConfigEditorString.cpp \
    src/config/WConfigItemWidget.cpp \
    src/config/WConfigRef.cpp \
    src/config/WConfigTemplate.cpp \
    src/config/WConfigViewer.cpp \
    src/config/WConfigWidget.cpp \
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