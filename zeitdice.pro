#-------------------------------------------------
#
# zeitdice QMakefile
#
#-------------------------------------------------

QT += core gui widgets opengl

TARGET = zeitdice

TEMPLATE = app

VERSION = 0.1.0

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

win {

    # Coming up as well

    RC_ICONS = zeitdice.ico

}

mac {

    ICON = zeitdice.icns

    # In the .plist file, you can define some variables, e.g., @EXECUTABLE@,
    # which qmake will replace with the actual executable name. Other variables
    # include @ICON@, @TYPEINFO@, @LIBRARY@, and @SHORT_VERSION@.

     #QMAKE_INFO_PLIST = Info.plist

    # Preliminary non-generic build configuration on Michael's OS X

    QMAKE_CXXFLAGS += \
        -I/Users/mschwanzer/Dropbox/WORK/ZEITDICE/qtapp-buildtool/installs/include

    LIBS += \
        -L/Users/mschwanzer/Dropbox/WORK/ZEITDICE/qtapp-buildtool/installs/lib \
        -lavfilter -lavformat -lavcodec -lswresample -lswscale -lavutil -lz -lx264 \
        -lQt5OpenGL -lQt5Widgets -lQt5Gui -lQt5Core \
        -L/Users/mschwanzer/Dropbox/WORK/ZEITDICE/qtapp-buildtool/installs/plugins/platforms -lqcocoa \
        -framework CoreFoundation -framework CoreVideo -framework VideoDecodeAcceleration

}

linux {

    # Preliminary non-generic build configuration on Simon's Fedora 21

    QMAKE_CXXFLAGS += \
        -I/home/simonrepp/zeitdice/qtapp-buildtool/installs/include

    LIBS += \
        -L/home/simonrepp/zeitdice/qtapp-buildtool/installs/lib \
        -lavfilter -lavformat -lavcodec -lswresample -lswscale -lavutil -lz -lx264 \
        -L/home/simonrepp/zeitdice/qtapp-buildtool/installs/plugins/platforms -lqxcb

}

OTHER_FILES += \
    .gitignore \
    splash.png \
    README.md
