DISTRO_TYPE ?= debug
TARGET_ARCH ?= x86
QMAKE       ?= qmake

# If not set by OE then use desktop staging locations
INCLUDE_DIR ?= $(LUNA_STAGING)/include
LIB_DIR     ?= $(LUNA_STAGING)/lib

STAGING_INCDIR ?= $(INCLUDE_DIR)
STAGING_LIBDIR ?= $(LIB_DIR)

.PHONY: all
all:
	$(MAKE) --directory=widgets
	$(MAKE) --directory=misc
	$(MAKE) --directory=qbsplugin
	$(MAKE) --directory=qtwebkitplugin

.PHONY: configure
configure:
	$(QMAKE) widgets/widgets.pro -o widgets/Makefile
	$(QMAKE) misc/misc.pro -o misc/Makefile
	$(QMAKE) qbsplugin/qbsplugin.pro -o qbsplugin/Makefile
	$(QMAKE) qtwebkitplugin/qtwebkitplugin.pro -o qtwebkitplugin/Makefile

.PHONY: stage
stage:
	$(MAKE) --directory=widgets install
	$(MAKE) --directory=misc install
	$(MAKE) --directory=qbsplugin install
	$(MAKE) --directory=qtwebkitplugin install

.PHONY: install
install:
	$(AT)install -d $(INSTALL_DIR)/usr/plugins/platforms
	$(AT)install -m 555 qbsplugin/libqbsplugin.so $(INSTALL_DIR)/usr/plugins/platforms/
	$(AT)install -d $(INSTALL_DIR)/usr/plugins/webkit/
	$(AT)install -m 555 qtwebkitplugin/libqtwebkitplugin.so $(INSTALL_DIR)/usr/plugins/webkit/

.PHONY: clean
clean:
	$(MAKE) --directory=widgets clean
	$(MAKE) --directory=misc clean
	$(MAKE) --directory=qbsplugin clean
	$(MAKE) --directory=qtwebkitplugin clean
