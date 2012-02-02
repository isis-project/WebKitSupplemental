TARGET = WebKitMisc
TEMPLATE = lib
CONFIG += staticlib

QT += webkit
CONFIG += warn_on
CONFIG += building-libs
CONFIG += depend_includepath

DISTRO_TYPE = $$(DISTRO_TYPE)
contains(DISTRO_TYPE, release) {
    CONFIG -= debug
}

HEADERS += \
    weboswebpage.h

SOURCES += \
    weboswebpage.cpp \
    weboswebsettings.cpp \
    webosmisc.cpp

OBJECTS_DIR = .obj
MOC_DIR = .moc

QMAKE_CLEAN += $(TARGET)

STAGING_INCDIR = $$(STAGING_INCDIR)
isEmpty(STAGING_INCDIR):STAGING_INCDIR = $$(LUNA_STAGING)/include

STAGING_LIBDIR = $$(STAGING_LIBDIR)
isEmpty(STAGING_LIBDIR):STAGING_LIBDIR = $$(LUNA_STAGING)/lib

#install headers
headers.path = $${STAGING_INCDIR}/WebKitSupplemental
headers.files += \
    weboswebpage.h \
    weboswebsettings.h\
    webosmisc.h

INSTALLS += headers

#install lib
target.path = $$STAGING_LIBDIR
INSTALLS += target
