#!/usr/bin/env bash
#
# A shell script to build and install zeitdice qt app dependencies on OS X

SCRIPTS=$PWD

DEPS="$SCRIPTS/../dependencies"
INST="$DEPS/installed"
SRC="$DEPS/sources"

VERSION_FFMPEG="3.0.1"
VERSION_QT="5.4.1"
VERSION_X264="20141218-2245-stable"

THREADS=$(nproc)

INST_FFMPEG="$INST/ffmpeg-$VERSION_FFMPEG"
INST_QT="$INST/qt-everywhere-opensource-src-$VERSION_QT"
INST_X264="$INST/x264-snapshot-$VERSION_X264"

SRC_FFMPEG="$SRC/ffmpeg-$VERSION_FFMPEG"
SRC_QT="$SRC/qt-everywhere-opensource-src-$VERSION_QT"
SRC_X264="$SRC/x264-snapshot-$VERSION_X264"

TAR_FFMPEG="$SRC_FFMPEG.tar.bz2"
TAR_QT="$SRC_QT.tar.gz"
TAR_X264="$SRC_X264.tar.bz2"

URL_FFMPEG="http://ffmpeg.org/releases/ffmpeg-$VERSION_FFMPEG.tar.bz2"
URL_QT="http://ftp.fau.de/qtproject/archive/qt/5.4/$VERSION_QT/single/qt-everywhere-opensource-src-$VERSION_QT.tar.gz"
URL_X264="ftp://ftp.videolan.org/pub/videolan/x264/snapshots/x264-snapshot-$VERSION_X264.tar.bz2"

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
  _echo "${RED}$@${NORMAL}"
}

WARNING() {
  _echo "${YELLOW}$@${NORMAL}"
}

INFO() {
  _echo "${GREEN}$@${NORMAL}"
}

##### Parse commandline arguments #####

SKIP_X264=true
SKIP_FFMPEG=true
SKIP_QT=true

case $1 in
  --x264)
    SKIP_X264=false
  ;;
  --force-x264)
    rm -rf $INST_X264
    SKIP_X264=false
  ;;
  --ffmpeg)
    SKIP_X264=false
    SKIP_FFMPEG=false
  ;;
  --force-ffmpeg)
    rm -rf $INST_FFMPEG
    SKIP_X264=false
    SKIP_FFMPEG=false
  ;;
  --qt)
    SKIP_QT=false
  ;;
  --force-qt)
    rm -rf $INST_QT
    SKIP_QT=false
  ;;
  --help)
    INFO $"Optional flags:
    --x264            Build only x264
    --force-x264      Force rebuilding x264

    --ffmpeg          Build only ffmpeg
    --force-ffmpeg    Force rebuilding ffmpeg

    --qt              Build only qt
    --force-qt        Force rebuilding qt\n"
    exit 1
  ;;
  *)
    SKIP_X264=false
    SKIP_FFMPEG=false
    SKIP_QT=false
  ;;
esac

##### libx264 #####

if [ $SKIP_X264 = false ]; then

  if [ -d $INST_X264 ]; then

    WARNING "[x264] Found installed version $VERSION_X264 - skipping build"

  else

    INFO "[x264] Building version $VERSION_X264"

    mkdir -p $INST_X264

    if [ ! -d $SRC_X264 ]; then

      mkdir -p $SRC

      INFO "[x264] Downloading sources from $URL_X264"
      curl $URL_X264 -o $TAR_X264

      INFO "[x264] Unpacking archive $TAR_X264"
      tar -C $SRC -xf $TAR_X264

      INFO "[x264] Removing archive $TAR_X264"
      rm $TAR_X264

    fi

    cd $SRC_X264

    INFO "[x264] Configuring"
    ./configure --disable-avs       \
                --disable-cli       \
                --disable-ffms      \
                --disable-gpac      \
                --disable-lavf      \
                --disable-lsmash    \
                --disable-opencl    \
                --disable-swscale   \
                --enable-shared     \
                --enable-strip      \
                --prefix=$INST_X264

    INFO "[x264] Compiling"
    make -j$THREADS

    INFO "[x264] Installing to $INST_X264"
    make install

    INFO "[x264] Cleaning"
    make clean

    cd $SCRIPTS

  fi

fi

##### FFmpeg #####

if [ $SKIP_FFMPEG = false ]; then

  if [ -d $INST_FFMPEG ]; then

    WARNING "[ffmpeg] Found installed version $VERSION_FFMPEG - skipping build"

  else

    INFO "[ffmpeg] Building version $VERSION_FFMPEG"

    mkdir -p $INST_FFMPEG

    if [ ! -d $SRC_FFMPEG ]; then

      mkdir -p $SRC

      INFO "[ffmpeg] Downloading sources from $URL_FFMPEG"
      curl $URL_FFMPEG -o $TAR_FFMPEG

      INFO "[ffmpeg] Unpacking archive $TAR_FFMPEG"
      tar -C $SRC -xf $TAR_FFMPEG

      INFO "[ffmpeg] Removing archive $TAR_FFMPEG"
      rm $TAR_FFMPEG

    fi

    cd $SRC_FFMPEG

    INFO "[ffmpeg] Configuring"
    ./configure --enable-gpl \
                --enable-gray \
                --enable-libx264 \
                --enable-pthreads \
                --enable-runtime-cpudetect \
                --enable-shared \
		            --enable-stripping \
                --enable-zlib \
                --extra-cflags="-I$INST_X264/include" \
                --extra-ldflags="-pthread -L$INST_X264/lib" \
                --extra-libs="-lx264" \
                --disable-static \
                --disable-avdevice \
                --disable-bzlib \
                --disable-doc \
                --disable-indevs \
                --disable-nonfree \
                --disable-outdevs \
                --disable-postproc \
                --disable-programs \
                --disable-static \
                --disable-swresample \
                --disable-version3 \
                --prefix=$INST_FFMPEG

                # --cc="gcc -Wl,--as-needed" \
                # --disable-librtmp \
                # --disable-libopencore-amrwb \
                # --disable-libopencore-amrnb \
                # --disable-libdc1394 \
                # --disable-libspeex \
                # --disable-libfaac \
                # --disable-libgsm \
                # --disable-vaapi \
                # --disable-vdpau \

    INFO "[ffmpeg] Compiling"
    make -j$THREADS

    INFO "[ffmpeg] Installing to $INST_FFMPEG"
    make install

    INFO "[ffmpeg] Cleaning"
    make clean

    cd $SCRIPTS

  fi

fi

##### Qt #####

if [ $SKIP_QT = false ]; then

  if [ -d $INST_QT ]; then

    WARNING "[qt] Found installed version $VERSION_QT - skipping build"

  else

    INFO "[qt] Building version $VERSION_QT"

    mkdir -p $INST_QT

    if [ ! -d $SRC_QT ]; then

      mkdir -p $SRC

      INFO "[qt] Downloading sources from $URL_QT"
      curl $URL_QT -o $TAR_QT

      INFO "[qt] Unpacking archive $TAR_QT"
      tar -C $SRC -xf $TAR_QT

      INFO "[qt] Removing archive $TAR_QT"
      rm $TAR_QT

    fi

    cd $SRC_QT

    INFO "[qt] Configuring"
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
                -prefix $INST_QT          \
                -qt-harfbuzz              \
                -qt-libjpeg               \
                -qt-libpng                \
                -qt-pcre                  \
                -qt-xcb                   \
                -qt-xkbcommon             \
                -qt-zlib                  \
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
                -warnings-are-errors

    INFO "[qt] Compiling"
    make -j$THREADS

    INFO "[qt] Installing to $INST_QT"
    make install

    INFO "[qt] Cleaning"
    make clean

    cd $SCRIPTS

  fi

fi
