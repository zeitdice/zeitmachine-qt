#!/usr/bin/env bash

SCRIPTS=$PWD
ASSETS_DIR="$SCRIPTS/../assets"
RELEASE_DIR="/Users/simonrepp/zeitdice/build-zeitmachine-Desktop_Qt_5_9_1_clang_64bit-Debug"
QT_REDIST_BIN_DIR="/Users/simonrepp/Qt/5.9.1/clang_64/bin"

# Deploy with macdeployqt
$QT_REDIST_BIN_DIR/macdeployqt $RELEASE_DIR/zeitmachine.app

# Clean up some stuff
rm -rf $RELEASE_DIR/zeitmachine.app/Contents/PlugIns/imageformats
rm -rf $RELEASE_DIR/zeitmachine.app/Contents/PlugIns/printsupport

cp -rf $ASSETS_DIR/Info.plist $RELEASE_DIR/zeitmachine.app/Contents/Info.plist
