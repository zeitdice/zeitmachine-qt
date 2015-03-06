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
