TARGET = qwebos
TEMPLATE = lib
CONFIG += plugin warn_off

QT += opengl core-private gui-private opengl-private xml
QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms

#include(externalplugin.pri)

INCLUDEPATH += $$QT_SOURCE_TREE/src/3rdparty/harfbuzz/src
DEFINES += QT_NO_FONTCONFIG

CONFIG += link_pkgconfig
PKGCONFIG += glib-2.0 freetype2 LunaSysMgrIpc

SOURCES =   main.cpp \
            qwebosintegration.cpp \
            qweboswindow.cpp \
            qweboswindowsurface.cpp \
            qwebosscreen.cpp \
            qwebosfontdatabase.cpp \
            qwebosglcontext.cpp \
            qwebosnativeinterface.cpp \
            hybriscompositorclient.cpp \
            qwebosipcclient.cpp

HEADERS =   qwebosintegration.h \
            qweboswindow.h \
            qweboswindowsurface.h \
            qwebosscreen.h \
            qwebosfontdatabase.h \
            qwebosglcontext.h \
            qwebosnativeinterface.h \
            hybriscompositorclient.h \
            qwebosipcclient.h

# EGL convenience
INCLUDEPATH += eglconvenience/
SOURCES += eglconvenience/qeglconvenience.cpp \
           eglconvenience/qeglplatformcontext.cpp
HEADERS += eglconvenience/qeglconvenience.h \
           eglconvenience/qeglplatformcontext.h

# webOS clipboard
INCLUDEPATH += clipboards/
SOURCES += clipboards/qwebosclipboard.cpp
HEADERS += clipboards/qwebosclipboard.h

INCLUDEPATH += $$QT_BUILD_TREE/include
INCLUDEPATH += $$QT_BUILD_TREE/include/QtOpenGL
INCLUDEPATH += $$QT_BUILD_TREE/include/QtGui
INCLUDEPATH += $$QT_BUILD_TREE/include/QtCore
SOURCES += $$QT_SOURCE_TREE/src/gui/text/qfontengine_ft.cpp

INCLUDEPATH += $$(STAGING_INCDIR)/ime \
               $$(STAGING_INCDIR)/sysmgr-ipc

QMAKE_CXXFLAGS += -fno-rtti -fno-exceptions

QMAKE_CLEAN += libqwebos.so

STAGING_INCDIR = $$(STAGING_INCDIR)
isEmpty(STAGING_INCDIR):STAGING_INCDIR = $$(STAGING_DIR)/include

STAGING_LIBDIR = $$(STAGING_LIBDIR)
isEmpty(STAGING_LIBDIR):STAGING_LIBDIR = $$(STAGING_DIR)/lib

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
