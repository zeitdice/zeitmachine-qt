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

_exit() {
    INFO $"Usage: $0 {--all|x264|ffmpeg|qt5} {--dare|--download|--no-download|--configure|--make|--nomake}"
    exit 1
}

SKIP_X264=true
SKIP_FFMPEG=true
SKIP_QT5=true

case $1 in
  --all)
    SKIP_X264=false
    SKIP_FFMPEG=false
    SKIP_QT5=false
  ;;
  --x264)
    SKIP_X264=false
  ;;
  --ffmpeg)
    SKIP_FFMPEG=false
  ;;
  --qt5)
    SKIP_QT5=false
  ;;
  *)
    _exit
  ;;
esac

SKIP_DOWNLOAD=false
SKIP_CONFIGURE=false
SKIP_MAKE=false

case $2 in
  --dare)
  ;;
  --download)
    SKIP_CONFIGURE=true
    SKIP_MAKE=true
  ;;
  --no-download)
    SKIP_DOWNLOAD=true
  ;;
  --configure)
    SKIP_DOWNLOAD=true
    SKIP_MAKE=true
  ;;
  --make)
    SKIP_DOWNLOAD=true
    SKIP_CONFIGURE=true
  ;;
  --nomake)
    SKIP_MAKE=true
  ;;
  *)
    _exit
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

  if [ $SKIP_DOWNLOAD = false ]; then

    INFO "[x264] Downloading sources"
    curl $X264_SOURCE -o last_stable_x264.tar.bz2

    INFO "[x264] Extracting sources"
    tar -C . -xf last_stable_x264.tar.bz2

  fi

  cd x264-*

  if [ $SKIP_CONFIGURE = false ]; then

    INFO "[x264] Configuring"
    ./configure --disable-avs     \
                --disable-cli     \
                --disable-ffms    \
                --disable-gpac    \
                --disable-lavf    \
                --disable-lsmash  \
                --disable-opencl  \
                --disable-swscale \
                --enable-static   \
                --enable-strip    \
                --prefix=../../

  fi

  if [ $SKIP_MAKE = false ]; then

    INFO "[x264] Compiling"
    make -j$THREADS

    INFO "[x264] Installing"
    make install

    INFO "[x264] Cleaning"
    make clean

  fi

  cd ..

}

##### FFmpeg #####

_ffmpeg() {

   if [ $SKIP_DOWNLOAD = false ]; then

     INFO "[ffmpeg] Downloading sources"
     curl $FFMPEG_SOURCE -o ffmpeg-$FFMPEG_VERSION.tar.bz2

     INFO "[ffmpeg] Extracting sources"
     tar -C . -xf ffmpeg-$FFMPEG_VERSION.tar.bz2

  fi

  cd ffmpeg-*

  if [ $SKIP_CONFIGURE = false ]; then

    INFO "[ffmpeg] Configuring"
    ./configure --cc="gcc -Wl,--as-needed" \
                --enable-gpl \
                --enable-gray \
                --enable-libx264 \
                --enable-pthreads \
                --enable-runtime-cpudetect \
                --enable-static \
                --enable-stripping \
                --enable-zlib \
                --extra-cflags="-I../../include" \
                --extra-ldflags="-pthread -static-libgcc -L../../lib" \
                --extra-libs="-lx264" \
                --disable-avdevice \
                --disable-bzlib \
                --disable-doc \
                --disable-indevs \
                --disable-nonfree \
                --disable-outdevs \
                --disable-postproc \
                --disable-programs \
                --disable-swresample \
                --disable-version3 \
                --prefix=../../

  fi

  if [ $SKIP_MAKE = false ]; then

    INFO "[ffmpeg] Compiling"
    make -j$THREADS

    INFO "[ffmpeg] Installing"
    make install

    INFO "[ffmpeg] Cleaning"
    make clean

  fi

  cd ..

}

##### Qt5 #####

_qt5() {

  if [ $SKIP_DOWNLOAD = false ]; then

    INFO "[qt5] Downloading sources"
    curl $QT5_SOURCE -o qt-$QT5_VERSION.tar.gz

    INFO "[qt5] Extracting sources"
    tar -C . -xf qt-$QT5_VERSION.tar.gz

  fi

  cd qt-everywhere-*

  if [ $SKIP_CONFIGURE = false ]; then

    INFO "[qt5] Configuring"
    ./configure -confirm-license          \
                -fontconfig               \
                -glib                     \
                -gtkstyle                 \
                -icu                      \
                -nomake examples          \
                -nomake tests             \
                -nomake tools             \
                -no-cups                  \
                -no-qml-debug             \
                -no-sql-db2               \
                -no-sql-ibase             \
                -no-sql-mysql             \
                -no-sql-oci               \
                -no-sql-odbc              \
                -no-sql-psql              \
                -no-sql-sqlite            \
                -no-sql-sqlite2           \
                -no-sql-tds               \
                -opengl                   \
                -opensource               \
                -prefix ../../../         \
                -qt-libjpeg               \
                -qt-libpng                \
                -qt-zlib                  \
                -qt-xkbcommon             \
                -qt-pcre                  \
                -qt-harfbuzz              \
                -silent                   \
                -skip qtandroidextras     \
                -skip qtmacextras         \
                -skip qtx11extras         \
                -skip qtsvg               \
                -skip qtxmlpatterns       \
                -skip qtdeclarative       \
                -skip qtquickcontrols     \
                -skip qtmultimedia        \
                -skip qtwinextras         \
                -skip qtactiveqt          \
                -skip qtlocation          \
                -skip qtsensors           \
                -skip qtconnectivity      \
                -skip qtwebsockets        \
                -skip qtwebchannel        \
                -skip qtwebkit            \
                -skip qttools             \
                -skip qtwebkit-examples   \
                -skip qtimageformats      \
                -skip qtgraphicaleffects  \
                -skip qtscript            \
                -skip qtquick1            \
                -skip qtwayland           \
                -skip qtserialport        \
                -skip qtenginio           \
                -skip qtwebengine         \
                -skip qttranslations      \
                -skip qtdoc               \
                -static                   \
                -system-freetype          \
                -system-xcb               \
                -warnings-are-errors

  fi

  if [ $SKIP_MAKE = false ]; then

    INFO "[qt5] Compiling"
    make -j$THREADS

    INFO "[qt5] Installing"
    make install

    INFO "[qt5] Cleaning"
    make clean

  fi

  cd ..

}

##### Make it so #####

_init

if [ $SKIP_X264 = false ]; then
  _x264
fi

if [ $SKIP_FFMPEG = false ]; then
  _ffmpeg
fi

if [ $SKIP_QT5 = false ]; then
  _qt5
fi
