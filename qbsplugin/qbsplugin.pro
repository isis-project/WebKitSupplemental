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
SOURCES += $$QT_SOURCE_TREE/src/gui/text/qfontengine_ft.cpp

QMAKE_CXXFLAGS += -fno-rtti -fno-exceptions

OBJECTS_DIR = .obj
MOC_DIR = .moc

QMAKE_CLEAN += libqbsplugin.so


STAGING_INCDIR = $$(STAGING_INCDIR)
isEmpty(STAGING_INCDIR):STAGING_INCDIR = $$(STAGING_DIR)/include

STAGING_LIBDIR = $$(STAGING_LIBDIR)
isEmpty(STAGING_LIBDIR):STAGING_LIBDIR = $$(STAGING_DIR)/lib

STAGING_FONTSDIR = $$(STAGING_FONTSDIR)
isEmpty(STAGING_FONTSDIR):STAGING_FONTSDIR = $$(STAGING_LIBDIR)/fonts

#install headers
headers.path = $${STAGING_INCDIR}/WebKitSupplemental
headers.files += \
    qbsinterface.h

INSTALLS += headers

#install lib
target.path = $$(STAGING_DIR)/plugins/platforms
INSTALLS += target

#install fonts
fonts.path = $${STAGING_FONTSDIR}
fonts.files += \
    fonts/Roboto-Regular.ttf \
    fonts/Roboto-Bold.ttf \
    fonts/Roboto-Italic.ttf \
    fonts/Roboto-BoldItalic.ttf \
    fonts/DroidSerif-Regular.ttf \
    fonts/DroidSerif-Bold.ttf \
    fonts/DroidSerif-Italic.ttf \
    fonts/DroidSerif-BoldItalic.ttf \
    fonts/DroidSans.ttf \
    fonts/DroidSans-Bold.ttf \
    fonts/DroidSansMono.ttf \
    fonts/DroidNaskh-Regular.ttf \
    fonts/DroidSansEthiopic-Regular.ttf \
    fonts/DroidSansHebrew-Regular.ttf \
    fonts/DroidSansHebrew-Bold.ttf \
    fonts/DroidSansThai.ttf \
    fonts/DroidSansArmenian.ttf \
    fonts/DroidSansGeorgian.ttf \
    fonts/Lohit-Devanagari.ttf \
    fonts/Lohit-Bengali.ttf \
    fonts/Lohit-Tamil.ttf \
    fonts/DroidSansJapanese.ttf \
    fonts/NanumGothic.ttf \
    fonts/NanumMyeongjo.ttf \
    fonts/DroidSansFallback.ttf \
    fonts/system_fonts.xml \
    fonts/fallback_fonts.xml

INSTALLS += fonts

