### eqmake4 was here ###
CONFIG -= debug_and_release debug
CONFIG += release

TEMPLATE = app
VERSION=0.2
TARGET = meego-handset-email

SUBDIRS = translations
OBJECTS_DIR = .obj
MOC_DIR = .moc
MGEN_OUTDIR = .gen

PKGCONFIG += qmfmessageserver qmfclient
QT += dbus network
CONFIG += qt meegotouch mobility  link_pkgconfig
MOBILITY += contacts

LIBS += -lseaside

# Paths
M_PREFIX = /usr
M_INSTALL_BIN = $$INSTALL_ROOT$$M_PREFIX/bin/
M_INSTALL_DATA = $$INSTALL_ROOT$$M_PREFIX/share/
M_THEME_DIR = $$M_INSTALL_DATA/themes/

DEFINES += THEMEDIR=\\\"\"$$M_PREFIX/share/themes/base/meegotouch/$$TARGET/\"\\\"

target.path = $$M_INSTALL_BIN

STYLE_HEADERS += messageitemstyle.h

SOURCES += main.cpp \
    accountinfo.cpp \
    accountitem.cpp \
    accountsetuppage.cpp \
    composepage.cpp \
    contactpage.cpp \
    conversationpage.cpp \
    editexistingaccountpage.cpp \
    emailapplication.cpp \
    emaildbusadaptor.cpp \
    emailsettingspage.cpp \
    emailserviceconfiguration.cpp \
    emailutils.cpp \
    folderitem.cpp \
    folderpage.cpp \
    mailmanager.cpp \
    manualaccounteditpage.cpp \
    messageitem.cpp \
    messageitemcreator.cpp \
    messageitemmodel.cpp \
    messagelistpage.cpp \
    reviewaccountpage.cpp \
    servercontroller.cpp \

HEADERS += emailapplication.h \
    accountinfo.h \
    accountitem.h \
    accountsetuppage.h \
    composepage.h \
    contactpage.h \
    conversationpage.h \
    editexistingaccountpage.h \
    emaildbusadaptor.h \
    emailsettingspage.h \
    emailserviceconfiguration.h \
    emailutils.h \
    folderitem.h \
    folderpage.h \
    mailmanager.h \
    manualaccounteditpage.h \
    messageitem.h \
    messageitemcreator.h \
    messageitemmodel.h \
    messageitemstyle.h \
    messagelistpage.h \
    reviewaccountpage.h \
    servercontroller.h \
    $$STYLE_HEADERS

INSTALLS += target
