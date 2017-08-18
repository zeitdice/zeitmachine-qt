win32 {

   RC_ICONS = assets/zeitmachine.ico

   INCLUDEPATH += $$PWD/dependencies/ffmpeg-3.3.3-win64-dev/include

   LIBS += -L$$PWD/dependencies/ffmpeg-3.3.3-win64-dev/lib \
           -lavfilter -lavformat -lavcodec -lswscale -lavutil

}
