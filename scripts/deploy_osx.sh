#!/usr/bin/env bash

RELEASE_DIR="/Users/simonrepp/zeitdice/build-zeitdice-Desktop_Qt_5_6_0_clang_64bit-Debug"
QT_REDIST_BIN_DIR="/Users/simonrepp/Qt/5.6/clang_64/bin"

# Deploy with macdeployqt
$QT_REDIST_BIN_DIR/macdeployqt $RELEASE_DIR/zeitdice.app

# Clean up some stuff
rm -rf $RELEASE_DIR/zeitdice.app/Contents/PlugIns/imageformats
rm -rf $RELEASE_DIR/zeitdice.app/Contents/PlugIns/printsupport
