TARGET = qtwebkitplugin
TEMPLATE = lib
CONFIG += plugin

OBJECTS_DIR = .obj
MOC_DIR = .moc

SOURCES += \
    WebKitPlatformPlugin.cpp

HEADERS += \
    WebKitPlatformPlugin.h \
    WebOSPlatformPlugin.h \
    qwebkitplatformplugin.h


STAGING_INCDIR = $$(STAGING_INCDIR)
isEmpty(STAGING_INCDIR):STAGING_INCDIR = $$(LUNA_STAGING)/include

#install headers
headers.path = $${STAGING_INCDIR}/WebKitSupplemental
headers.files += \
    WebOSPlatformPlugin.h \
    qwebkitplatformplugin.h

INSTALLS += headers
