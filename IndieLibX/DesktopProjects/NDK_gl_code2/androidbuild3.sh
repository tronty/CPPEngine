#!/bin/bash
source ../../make.cnf

CURDIR0=`pwd -P`
CURDIR="$CURDIR0/../../Libraries/MVC/NDK"
cd "$CURDIR"
#SDLPATH="$( cd "$(dirname "$0")/.." ; pwd -P )"
SDLPATH="$CURDIR0/../../Libraries/MVC/NDK"

APP="$1"
APP="GL2JNIActivity"
BUILDPATH="$SDLPATH"

export NDK_PROJECT_PATH="$BUILDPATH"

APK="$BUILDPATH/bin/$APP-debug.apk"

#NDKBUILD=`which ndk-build`
#if [ -z "$NDKBUILD" ];then
#    echo "Could not find the ndk-build utility, install Android's NDK and add it to the path."
#    exit 1
#fi
NDKBUILD='/home/tronty/android-ndk-r9d/ndk-build'

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
echo "androidbuild3.sh:1"
cd $CURDIR0/../../Libraries/MVC/NDK/jni
#$NDKBUILD V=1 -j $NCPUS $NDKARGS | tee $CURDIR0/../../NDKbuild.log
##$NDKBUILD  NDK_APPLICATION_MK=/home/tronty/IndieLibX/Libraries/MVC/NDK/jni/Application.mk V=1 -j $NCPUS $NDKARGS | tee $CURDIR0/../../NDKbuild.log
#cd ~/android-ndk-r9d
if [ -n "$NDKBUILD" ]; then
$NDKBUILD
fi
echo "androidbuild3.sh:2"
if [ -n "$ANT" ]; then
$ANT debug
fi
#echo "androidbuild3.sh:3"

cd $CURDIR

APK="$BUILDPATH/bin/$APP-debug.apk"

if [ -f "$APK" ]; then
    echo "Your APK is ready at $APK"
    echo "To install to your device: "
    cd  $BUILDPATH
    $ANT debug install
    exit 0
fi

echo "4:There was an error building the APK"
exit 1
















