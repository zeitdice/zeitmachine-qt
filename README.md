# Zeitmachine

Qt 5.9.1, FFmpeg 3.3.3, x264

## Dependencies Windows 10

- zeranoe ffmpeg shared + dev

## Dependencies OS X El Capitan

- XCode and Command Line Tools
- pkg-info (compile from source --with-internal-glib and install)
- yasm (compile from source and install)

## Ubuntu 17.10

### Dependencies

- FFMPEG runtime + dev packages
- mesa-common-dev
- build-essential, yasm

### Build steps

- Run `scripts/build_deps_linux.sh` in a shell (first time setup only)
- Open and configure project in Qt Creator (first time setup only)
- Hit `Build` and observe what errors come up
- In case of build failure:
  - Install dependencies and/or make changes to `linux.pri`
  - Build again and repeat previous step if necessary
- When the build succeeds:
  - Edit `scripts/deploy_linux.sh` with correct paths
  - Run `scripts/build_deps_linux.sh` in a shell
  -
