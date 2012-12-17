TEMPLATE=subdirs
SUBDIRS=misc qtwebkitplugin widgets
!contains(QT_VERSION, "^5.*") {
 SUBDIRS += qbsplugin
}
