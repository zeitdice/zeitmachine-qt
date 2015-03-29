# zeitdice qtapp

QT += core      \
      gui       \
      widgets   \
      opengl

TARGET = zeitdice

TEMPLATE = app

VERSION = 0.5.0

CONFIG += c++11

HEADERS  += source/glvideowidget.h \
            source/mainwindow.h \
            source/zeitengine.h \
            source/settingsdialog.h \
            source/version.h \
            source/aboutdialog.h

SOURCES +=  source/main.cpp \
            source/glvideowidget.cpp \
            source/mainwindow.cpp \
            source/zeitengine.cpp \
            source/settingsdialog.cpp \
            source/aboutdialog.cpp

FORMS    += forms/mainwindow.ui \
            forms/settingsdialog.ui \
            forms/aboutdialog.ui

RESOURCES = zeitdice.qrc # menu icons and about image

include(win.pri)
include(mac.pri)
include(linux.pri)
