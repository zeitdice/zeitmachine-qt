#!/usr/bin/env bash

if [[ $# -eq 0 ]] ; then
  echo 'No version number supplied - exiting.'
  exit 1
fi

SCRIPTS_DIR=$PWD
PROJECT_DIR="$SCRIPTS_DIR/.."
ZEITDICE_DIR="$PROJECT_DIR/.."
HOME_DIR="$ZEITDICE_DIR/.."
ASSETS_DIR="$PROJECT_DIR/assets"
BUILDS_DIR="$PROJECT_DIR/builds"
BUILD_ZIP="$BUILDS_DIR/zeitmachine-macos-$1.zip"
BUILD_DIR="$ZEITDICE_DIR/build-zeitmachine-Desktop_Qt_5_9_1_clang_64bit-Release"
QT_REDIST_BIN_DIR="$HOME_DIR/Qt/5.9.1/clang_64/bin"

# Deploy with macdeployqt
$QT_REDIST_BIN_DIR/macdeployqt $BUILD_DIR/zeitmachine.app

# Clean up some stuff
rm -rf $BUILD_DIR/zeitmachine.app/Contents/PlugIns/imageformats
rm -rf $BUILD_DIR/zeitmachine.app/Contents/PlugIns/printsupport

cp -rf $ASSETS_DIR/Info.plist $BUILD_DIR/zeitmachine.app/Contents/Info.plist


rm $BUILD_ZIP
cd $BUILD_DIR
zip -r -y $BUILD_ZIP zeitmachine.app
