mac {

    ICON = zeitmachine.icns

    INCLUDEPATH += ../zeitmachine/dependencies/installed/ffmpeg-3.0.1/include

    QMAKE_LIBDIR += ../zeitmachine/dependencies/installed/ffmpeg-3.0.1/lib

    LIBS += -lavfilter      \
            -lavformat      \
            -lavcodec       \
            -lavutil        \
            -lswscale

    # In the .plist file, you can define some variables, e.g., @EXECUTABLE@,
    # which qmake will replace with the actual executable name. Other variables
    # include @ICON@, @TYPEINFO@, @LIBRARY@, and @SHORT_VERSION@.

    # QMAKE_INFO_PLIST = Info.plist

}
