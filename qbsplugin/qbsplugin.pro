TARGET = qbsplugin
TEMPLATE = lib
CONFIG += plugin warn_off
QT += xml

include(externalplugin.pri)

INCLUDEPATH += $$QT_SOURCE_TREE/src/3rdparty/harfbuzz/src
DEFINES += QT_NO_FONTCONFIG
DEFINES += Q_WS_QPA

CONFIG += link_pkgconfig
PKGCONFIG += glib-2.0 freetype2

SOURCES +=  main.cpp \
            qbsintegration.cpp \
            qbswindowsurface.cpp \
            qminimalwindowsurface.cpp \
            qwebosfontdatabase.cpp

HEADERS +=  qbsintegration.h \
            qminimalwindowsurface.h \
            qbswindowsurface.h \
            qwebosfontdatabase.h

INCLUDEPATH += $$QT_BUILD_TREE/include
INCLUDEPATH += $$QT_BUILD_TREE/include/QtGui
INCLUDEPATH += $$QT_BUILD_TREE/include/QtCore
INCLUDEPATH += $$QT_BUILD_TREE/include/QtXml
contains(QT_VERSION, "^5.*") {
    INCLUDEPATH += $$QT_BUILD_TREE/include/QtGui/5.0.0/QtGui
}
SOURCES += $$QT_SOURCE_TREE/src/gui/text/qfontengine_ft.cpp

QMAKE_CXXFLAGS += -fno-rtti -fno-exceptions

OBJECTS_DIR = .obj
MOC_DIR = .moc

QMAKE_CLEAN += libqbsplugin.so


STAGING_INCDIR = $$(STAGING_INCDIR)
isEmpty(STAGING_INCDIR):STAGING_INCDIR = $$(STAGING_DIR)/include

STAGING_LIBDIR = $$(STAGING_LIBDIR)
isEmpty(STAGING_LIBDIR):STAGING_LIBDIR = $$(STAGING_DIR)/lib

#install headers
headers.path = $${STAGING_INCDIR}/WebKitSupplemental
headers.files += \
    qbsinterface.h

INSTALLS += headers

#install lib
target.path = $$(STAGING_DIR)/plugins/platforms
INSTALLS += target
