linux {

    CONFIG(debug, debug|release) {

        LIBS += -lavfilter      \
                -lavformat      \
                -lavcodec       \
                -lswresample    \
                -lswscale       \
                -lavutil

    } else {

        INCLUDEPATH +=  deps/include

        QMAKE_LIBDIR += deps/lib \
                        deps/plugins/platforms

        LIBS += -lavfilter      \
                -lavformat      \
                -lavcodec       \
                -lswresample    \
                -lswscale       \
                -lavutil        \
                -lz             \
                -lx264          \
                -lQt5OpenGL     \
                -lQt5Widgets    \
                -lQt5Gui        \
                -lQt5Core       \
                -lqxcb

    }

}