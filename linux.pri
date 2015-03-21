linux {

    CONFIG(debug, debug|release) {

        INCLUDEPATH +=  /usr/include/ffmpeg

        LIBS += -lavfilter      \
                -lavformat      \
                -lavcodec       \
                -lswresample    \
                -lswscale       \
                -lavutil

    } else {

        INCLUDEPATH +=  ../qtapp/dependencies/installed/ffmpeg-2.6.1/include                       \
                        ../qtapp/dependencies/installed/qt-everywhere-opensource-src-5.4.1/include \
                        ../qtapp/dependencies/installed/x264-snapshot-20141218-2245-stable/include

        QMAKE_LIBDIR += ../qtapp/dependencies/installed/ffmpeg-2.6.1/lib                                            \
                        ../qtapp/dependencies/installed/qt-everywhere-opensource-src-5.4.1/lib                      \
                        ../qtapp/dependencies/installed/qt-everywhere-opensource-src-5.4.1/plugins/platforms        \
                        ../qtapp/dependencies/installed/qt-everywhere-opensource-src-5.4.1/plugins/platformthemes   \
                        ../qtapp/dependencies/installed/x264-snapshot-20141218-2245-stable/lib

        LIBS += -static-libgcc      \
                -static-libstdc++   \
                -lavfilter          \
                -lavformat          \
                -lavcodec           \
                -lswscale           \
                -lavutil            \
                -lx264              \
                -lz                 \
                -lQt5OpenGL         \
                -lQt5Widgets        \
                -lQt5Gui            \
                -lQt5Core           \
                -lqxcb              \
                -lqgtk2

    }

}
