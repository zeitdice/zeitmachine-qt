mac {

    ICON = zeitmachine.icns

    INCLUDEPATH += ../zeitmachine-qt/dependencies/installed/ffmpeg-3.3.3/include

    QMAKE_LIBDIR += ../zeitmachine-qt/dependencies/installed/ffmpeg-3.3.3/lib

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
