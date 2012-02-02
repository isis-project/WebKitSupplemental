TARGET = qbsplugin
TEMPLATE = lib
CONFIG += plugin warn_off

include(externalplugin.pri)

INCLUDEPATH += $$QT_SOURCE_TREE/src/3rdparty/harfbuzz/src
DEFINES += QT_NO_FONTCONFIG

CONFIG += link_pkgconfig
PKGCONFIG += glib-2.0 freetype2

SOURCES =   main.cpp \
            qbsintegration.cpp \
            qbswindowsurface.cpp \
            qminimalwindowsurface.cpp 

HEADERS =   qbsintegration.h \
            qminimalwindowsurface.h \
            qbswindowsurface.h 

INCLUDEPATH += $$QT_BUILD_TREE/include
INCLUDEPATH += $$QT_BUILD_TREE/include/QtGui
INCLUDEPATH += $$QT_BUILD_TREE/include/QtCore
INCLUDEPATH += $$QT_SOURCE_TREE/src/plugins/platforms/fontdatabases/genericunix 
INCLUDEPATH += $$QT_SOURCE_TREE/src/plugins/platforms/fontdatabases/basicunix 
SOURCES += $$QT_SOURCE_TREE/src/gui/text/qfontengine_ft.cpp
SOURCES += $$QT_SOURCE_TREE/src/plugins/platforms/fontdatabases/basicunix/qbasicunixfontdatabase.cpp

QMAKE_CXXFLAGS += -fno-rtti -fno-exceptions

OBJECTS_DIR = .obj
MOC_DIR = .moc

QMAKE_CLEAN += libqbsplugin.so


STAGING_INCDIR = $$(STAGING_INCDIR)
isEmpty(STAGING_INCDIR):STAGING_INCDIR = $$(LUNA_STAGING)/include

STAGING_LIBDIR = $$(STAGING_LIBDIR)
isEmpty(STAGING_LIBDIR):STAGING_LIBDIR = $$(LUNA_STAGING)/lib

#install headers
headers.path = $${STAGING_INCDIR}/WebKitSupplemental
headers.files += \
	qbsinterface.h

INSTALLS += headers


