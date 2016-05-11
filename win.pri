win32 {

   RC_ICONS = assets/zeitmachine.ico

   INCLUDEPATH += $$PWD/dependencies/ffmpeg-20160418-git-13406b6-win64-dev/include

   LIBS += -L$$PWD/dependencies/ffmpeg-20160418-git-13406b6-win64-dev/lib \
           -lavfilter -lavformat -lavcodec -lswscale -lavutil

}
