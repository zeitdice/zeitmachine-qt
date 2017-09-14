#!/bin/bash

SCRIPTS_DIR=$PWD
PROJECT_DIR="$SCRIPTS_DIR/.."
ZEITDICE_DIR="$PROJECT_DIR/.."
HOME_DIR="$ZEITDICE_DIR/.."
BUILD_DIR="$ZEITDICE_DIR/build-zeitmachine-Desktop_Qt_5_9_1_GCC_64bit-Debug/"
QT_LIB_DIR="$HOME_DIR/Qt/5.9.1/gcc_64/lib"
FFMPEG_LIB_DIR="$PROJECT_DIR/dependencies/installed/ffmpeg-3.3.3/lib"
X264_LIB_DIR="$PROJECT_DIR/dependencies/installed/x264-snapshot-20170816-2245-stable/lib"

cp $SCRIPTS_DIR/zeitmachine.sh $BUILD_DIR

cp /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.21 $BUILD_DIR
cp /lib/x86_64-linux-gnu/libgcc_s.so.1 $BUILD_DIR
cp /lib/x86_64-linux-gnu/libc.so.6 $BUILD_DIR

cp $QT_LIB_DIR/libQt5OpenGL.so.5 $BUILD_DIR
cp $QT_LIB_DIR/libQt5Widgets.so.5 $BUILD_DIR
cp $QT_LIB_DIR/libQt5Gui.so.5 $BUILD_DIR
cp $QT_LIB_DIR/libQt5Core.so.5 $BUILD_DIR

cp $QT_LIB_DIR/libicui18n.so.56 $BUILD_DIR
cp $QT_LIB_DIR/libicuuc.so.56 $BUILD_DIR
cp $QT_LIB_DIR/libicudata.so.56 $BUILD_DIR

cp $FFMPEG_LIB_DIR/libavfilter.so.6 $BUILD_DIR
cp $FFMPEG_LIB_DIR/libavformat.so.57 $BUILD_DIR
cp $FFMPEG_LIB_DIR/libavcodec.so.57 $BUILD_DIR
cp $FFMPEG_LIB_DIR/libswscale.so.4 $BUILD_DIR
cp $FFMPEG_LIB_DIR/libavutil.so.55 $BUILD_DIR

cp $X264_LIB_DIR/libx264.so.148 $BUILD_DIR
