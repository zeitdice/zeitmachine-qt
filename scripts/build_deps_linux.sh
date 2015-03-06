#!/usr/bin/env bash
#
# A shell script to build and install zeitdice qt app dependencies on linux*

X264_SOURCE="ftp://ftp.videolan.org/pub/videolan/x264/snapshots/last_stable_x264.tar.bz2"

FFMPEG_VERSION="2.2.13"
FFMPEG_SOURCE=( "http://ffmpeg.org/releases/ffmpeg-$FFMPEG_VERSION.tar.gz" )

QT5_VERSION="5.4.1"
QT5_SOURCE=( "http://ftp.fau.de/qtproject/archive/qt/5.4/$QT5_VERSION/single/qt-everywhere-opensource-src-$QT5_VERSION.tar.gz" )

##### Generic Helpers #####

BLACK=$(tput setaf 0)
RED=$(tput setaf 1)
GREEN=$(tput setaf 2)
YELLOW=$(tput setaf 3)
LIME_YELLOW=$(tput setaf 190)
POWDER_BLUE=$(tput setaf 153)
BLUE=$(tput setaf 4)
MAGENTA=$(tput setaf 5)
CYAN=$(tput setaf 6)
WHITE=$(tput setaf 7)
BRIGHT=$(tput bold)
NORMAL=$(tput sgr0)
BLINK=$(tput blink)
REVERSE=$(tput smso)
UNDERLINE=$(tput smul)

_echo() {
  if [ "X$1" = "X-n" ]; then
     shift; printf "%s" "$@"
  else
     printf "%s\n" "$@"
  fi
}

ERROR() {
  _echo "${BRIGHT}${RED}ERROR! ${NORMAL}${RED}$@${NORMAL}"
}

WARNING() {
  _echo "${BRIGHT}${YELLOW}WARNING! ${NORMAL}${YELLOW}$@${NORMAL}"
}

INFO() {
  _echo "${GREEN}$@${NORMAL}"
}

PRINT() {
  _echo "$@"
}

##### Parse commandline arguments #####

X264_SKIP=true
FFMPEG_SKIP=true
QT5_SKIP=true

case $1 in
  --all)
    X264_SKIP=false
    FFMPEG_SKIP=false
    QT5_SKIP=false
  ;;
  --x264)
    X264_SKIP=false
  ;;
  --ffmpeg)
    FFMPEG_SKIP=false
  ;;
  --qt5)
    QT5_SKIP=false
  ;;
  *)
    echo $"Usage: $0 {--all|x264|ffmpeg|qt5}"
    exit 1
  ;;
esac

##### Initialize #####

_init() {

  THREADS=$(nproc)
  mkdir -p ../deps/src
  cd ../deps/src/

}


##### libx264 #####

_x264() {

  INFO "[x264] Downloading sources"
  curl $X264_SOURCE -o last_stable_x264.tar.bz2

  INFO "[x264] Extracting sources"
  tar -C . -xf last_stable_x264.tar.bz2

  cd x264-*

  INFO "[x264] Configuring"
  ./configure --disable-avs \
              --disable-cli \
              --disable-ffms \
              --disable-gpac \
              --disable-lavf \
              --disable-opencl \
              --disable-swscale \
              --enable-static \
              --prefix=../../

  INFO "[x264] Compiling"
  make -j$THREADS

  INFO "[x264] Installing"
  make install

  INFO "[x264] Cleaning"
  make clean

  cd ..

}

##### FFmpeg #####

_ffmpeg() {

  INFO "[ffmpeg] Downloading sources"
  curl $FFMPEG_SOURCE -o ffmpeg-$FFMPEG_VERSION.tar.bz2

  INFO "[ffmpeg] Extracting sources"
  tar -C . -xf ffmpeg-$FFMPEG_VERSION.tar.bz2

  cd ffmpeg-*

  INFO "[ffmpeg] Configuring"
  ./configure --cc="gcc -Wl,--as-needed" \
              --enable-avfilter \
              --enable-gpl \
              --enable-gray \
              --enable-libx264 \
              --enable-pthreads \
              --enable-runtime-cpudetect \
              --enable-static \
              --enable-stripping \
              --enable-zlib \
              --extra-cflags="-I../../include" \
              --extra-ldflags="-pthread -static-libgcc -L../../lib -lx264" \
              --disable-bzlib \
              --disable-doc \
              --disable-ffplay \
              --disable-ffprobe \
              --disable-ffserver \
              --disable-indev=alsa \
              --disable-indev=jack \
              --disable-indev=lavfi \
              --disable-indev=sdl \
              --disable-libgsm \
              --disable-libspeex \
              --disable-libfaac \
              --disable-librtmp \
              --disable-libopencore-amrnb \
              --disable-libopencore-amrwb \
              --disable-libdc1394 \
              --disable-nonfree \
              --disable-outdev=alsa \
              --disable-outdev=sdl \
              --disable-outdev=xv \
              --disable-postproc \
              --disable-vaapi \
              --disable-vdpau \
              --disable-version3 \
              --prefix=../../

  INFO "[ffmpeg] Compiling"
  make -j$THREADS

  INFO "[ffmpeg] Installing"
  make install

  INFO "[ffmpeg] Cleaning"
  make clean

  cd ..

}

##### Qt5 #####

_qt5() {

 INFO "[qt5] Downloading sources"
 curl $QT5_SOURCE -o qt-$QT5_VERSION.tar.gz

 INFO "[qt5] Extracting sources"
 tar -C . -xf qt-$QT5_VERSION.tar.gz

  cd qt-everywhere-*

  INFO "[qt5] Configuring"
  ./configure -confirm-license \
              -nomake examples \
              -nomake tests \
              -nomake tools \
              -no-qml-debug \
              -no-sql-db2 \
              -no-sql-ibase \
              -no-sql-mysql \
              -no-sql-oci \
              -no-sql-odbc \
              -no-sql-psql \
              -no-sql-sqlite \
              -no-sql-sqlite2 \
              -no-sql-tds \
              -no-gtkstyle \
              -opensource \
              -prefix ../../../ \
              -qt-xcb \
              -qt-libjpeg \
              -qt-libpng \
              -qt-zlib \
              -qt-xkbcommon \
              -qt-freetype \
              -qt-pcre \
              -qt-harfbuzz \
              -silent \
              -skip qtandroidextras \
              -skip qtmacextras \
              -skip qtx11extras \
              -skip qtsvg \
              -skip qtxmlpatterns \
              -skip qtdeclarative \
              -skip qtquickcontrols \
              -skip qtmultimedia \
              -skip qtwinextras \
              -skip qtactiveqt \
              -skip qtlocation \
              -skip qtsensors \
              -skip qtconnectivity \
              -skip qtwebsockets \
              -skip qtwebchannel \
              -skip qtwebkit \
              -skip qttools \
              -skip qtwebkit-examples \
              -skip qtimageformats \
              -skip qtgraphicaleffects \
              -skip qtscript \
              -skip qtquick1 \
              -skip qtwayland \
              -skip qtserialport \
              -skip qtenginio \
              -skip qtwebengine \
              -skip qttranslations \
              -skip qtdoc \
              -static


  INFO "[qt5] Compiling"
  make -j$THREADS

  INFO "[qt5] Installing"
  make install

  INFO "[qt5] Cleaning"
  make clean

  cd ..

}

##### Make it so #####

_init

if [ $X264_SKIP = false ]; then
  _x264
fi

if [ $FFMPEG_SKIP = false ]; then
  _ffmpeg
fi

if [ $QT5_SKIP = false ]; then
  _qt5
fi
