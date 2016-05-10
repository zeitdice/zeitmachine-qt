linux {

    INCLUDEPATH +=  ../zeitmachine/dependencies/installed/x264-snapshot-20141218-2245-stable/include \
                    ../zeitmachine/dependencies/installed/ffmpeg-3.0.1/include

    QMAKE_LIBDIR += /usr/lib/nvidia-361 \
                    ../zeitmachine/dependencies/installed/x264-snapshot-20141218-2245-stable/lib \
                    ../zeitmachine/dependencies/installed/ffmpeg-3.0.1/lib

    LIBS += -lavfilter      \
            -lavformat      \
            -lavcodec       \
            -lswscale       \
            -lavutil        \
            -lx264

}
