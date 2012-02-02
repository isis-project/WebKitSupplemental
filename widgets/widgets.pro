TARGET = WebKitWidgets
TEMPLATE = lib
CONFIG += staticlib

CONFIG += warn_on
CONFIG += building-libs
CONFIG += depend_includepath

DISTRO_TYPE = $$(DISTRO_TYPE)
contains(DISTRO_TYPE, release) {
    CONFIG -= debug
}

HEADERS += \
    phrasePickerWidget.h

SOURCES += \
    phrasePickerWidget.cpp

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
	phrasePickerWidget.h

INSTALLS += headers

#install lib
target.path = $$STAGING_LIBDIR
INSTALLS += target
