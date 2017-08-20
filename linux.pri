linux {

    INCLUDEPATH +=  ../zeitmachine-qt/dependencies/installed/x264-snapshot-20170816-2245-stable/include \
                    ../zeitmachine-qt/dependencies/installed/ffmpeg-3.3.3/include

    QMAKE_LIBDIR += /usr/lib/nvidia-375 \
                    ../zeitmachine-qt/dependencies/installed/x264-snapshot-20170816-2245-stable/lib \
                    ../zeitmachine-qt/dependencies/installed/ffmpeg-3.3.3/lib

    LIBS += -lavfilter      \
            -lavformat      \
            -lavcodec       \
            -lswscale       \
            -lavutil        \
            -lx264

}
