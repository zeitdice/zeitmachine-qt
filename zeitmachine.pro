# zeitmachine

QT += core      \
      gui       \
      widgets   \
      opengl

TARGET = zeitmachine

TEMPLATE = app

VERSION = 0.6.1

CONFIG += c++11

HEADERS  += src/glvideowidget.h \
            src/mainwindow.h \
            src/zeitengine.h \
            src/settingsdialog.h \
            src/version.h \
            src/aboutdialog.h

SOURCES +=  src/main.cpp \
            src/glvideowidget.cpp \
            src/mainwindow.cpp \
            src/zeitengine.cpp \
            src/settingsdialog.cpp \
            src/aboutdialog.cpp

FORMS    += forms/mainwindow.ui \
            forms/settingsdialog.ui \
            forms/aboutdialog.ui

RESOURCES = zeitmachine.qrc # menu icons and about image

include(win.pri)
include(mac.pri)
include(linux.pri)
