#!/bin/bash
source ../../make.cnf

CURDIR0=`pwd -P`
CURDIR="$CURDIR0/../../Libraries/SDL2-$sdl2version/build-scripts"
cd "$CURDIR"
#SDLPATH="$( cd "$(dirname "$0")/.." ; pwd -P )"
SDLPATH="$CURDIR0/../../Libraries/SDL2-$sdl2version"

APP="$1"
BUILDPATH="$SDLPATH/build/$APP"

export NDK_PROJECT_PATH="$BUILDPATH"

APK="$BUILDPATH/bin/$APP-debug.apk"

NDKBUILD=`which ndk-build`
if [ -z "$NDKBUILD" ];then
    echo "Could not find the ndk-build utility, install Android's NDK and add it to the path."
    exit 1
fi

ANT=`which ant`
if [ -z "$ANT" ];then
    echo "Could not find the ant utility, install ant and add it to the path."
    exit 1
fi

NCPUS="1"
case "$OSTYPE" in
    darwin*)
        NCPU=`sysctl -n hw.ncpu`
        ;; 
    linux*)
        if [ -n `which nproc` ]; then
            NCPUS=`nproc`
        fi  
        ;;
  *);;
esac

cd $NDK_PROJECT_PATH
$NDKBUILD V=1 -j $NCPUS $NDKARGS | tee $CURDIR0/../../NDKbuild.log
#$NDKBUILD V=1 -j $NCPUS $NDKARGS | tee $CURDIR0/../../NDKbuild.log
$ANT debug

cd $CURDIR

APK="$BUILDPATH/bin/$APP-debug.apk"

if [ -f "$APK" ]; then
    echo "Your APK is ready at $APK"
    echo "To install to your device: "
    cd  $BUILDPATH
    $ANT debug install
    exit 0
fi

echo "2:There was an error building the APK"
exit 1

