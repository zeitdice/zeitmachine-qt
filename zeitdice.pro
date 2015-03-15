# zeitdice qtapp

QT += core      \
      gui       \
      widgets   \
      opengl

TARGET = zeitdice

TEMPLATE = app

VERSION = 0.4.0

CONFIG += c++11

HEADERS  += src/glvideowidget.h \
            src/mainwindow.h \
            src/zeitengine.h \
            src/settingsdialog.h \
            src/version.h

SOURCES +=  src/main.cpp \
            src/glvideowidget.cpp \
            src/mainwindow.cpp \
            src/zeitengine.cpp \
            src/settingsdialog.cpp

FORMS    += forms/mainwindow.ui \
            forms/settingsdialog.ui

RESOURCES = zeitdice.qrc # menu icons and about image

include(win.pri)
include(mac.pri)
include(linux.pri)
