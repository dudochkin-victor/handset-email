### eqmake4 was here ###
CONFIG -= debug_and_release debug
CONFIG += release

TEMPLATE = subdirs
VERSION=0.2
TARGET = meego-handset-email

# Paths
M_PREFIX = /usr
M_INSTALL_BIN = $$INSTALL_ROOT$$M_PREFIX/bin/
M_INSTALL_DATA = $$INSTALL_ROOT$$M_PREFIX/share/
M_THEME_DIR = $$M_INSTALL_DATA/themes/

SUBDIRS = src translations

# Themes
basetheme.files = theme/base/*
basetheme.path = $$M_THEME_DIR/base/meegotouch/$$TARGET/

# Desktop
desktop_entry.files = email.desktop
desktop_entry.path = $$M_INSTALL_DATA/applications

# DBUS Service
dbus_service.files = email.service
dbus_service.path = $$M_INSTALL_DATA/dbus-1/services/

MAKE_CLEAN += $$OBJECTS_DIR/*.o
MAKE_DISTCLEAN += \
        $$MOC_DIR/* $$MOC_DIR \
        $$OBJECTS_DIR/* $$OBJECTS_DIR \
        $$MGEN_OUTDIR/* $$MGEN_OUTDIR

INSTALLS += desktop_entry \
            dbus_service \
            basetheme
