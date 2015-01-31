#-------------------------------------------------
#
# zeitdice QMakefile
#
#-------------------------------------------------

QT += core gui widgets opengl

TARGET = zeitdice
TEMPLATE = app

HEADERS  += \
    glvideowidget.h \
    mainwindow.h \
    zeitengine.h \
    settingsdialog.h

SOURCES += \
    main.cpp \
    glvideowidget.cpp \
    mainwindow.cpp \
    zeitengine.cpp \
    settingsdialog.cpp

FORMS    += \
    mainwindow.ui \
    settingsdialog.ui

RESOURCES = zeitdice.qrc # menu icons and about image

QMAKE_CXXFLAGS += \
    -I/usr/include/ffmpeg/ \ # Fedora 21 ffmpeg headers
    -I/opt/local/include/    # OS X ffmpeg headers

LIBS += \
    # -L/usr/local/lib/ \ # Fedora ffmpeg libraries
    /usr/local/lib/libavfilter.a \
    /usr/local/lib/libavformat.a \
    /usr/local/lib/libavcodec.a \
    /usr/local/lib/libswresample.a \
    /usr/local/lib/libswscale.a \
    /usr/local/lib/libavutil.a \
    /usr/local/lib/liblzma.a \
    /usr/local/lib/libz.a
    #-L/opt/local/lib \ # OS X ffmpeg shared libraries
    # -lavcodec -lavfilter -lavformat -lavutil -lswscale ### generic linkage disabled


OTHER_FILES += \
    .gitignore \
    splash.png \
    README.md
