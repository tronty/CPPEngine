#!/bin/bash
source ../../make.cnf

NDK1()
{
#echo "mainphoneproject2:$mainphoneproject"
#exit 0
#rm -fr $BUILDPATH/jni

#mkdir -p $BUILDPATH/jni/Libraries
#mkdir -p $BUILDPATH/jni/libs

ln -s $CURDIR/../../../Samples $BUILDPATH/jni
ln -s $CURDIR/../../../Dependencies $BUILDPATH/jni
ln -s $CURDIR/../../../../$NDK $BUILDPATH/jni

ln -s $CURDIR/../Libraries/BoostWorkaround $BUILDPATH/jni/Libraries
}

NDK()
{
#rm -fr $BUILDPATH/jni

#mkdir -p $BUILDPATH/jni/Libraries
#mkdir -p $BUILDPATH/jni/libs

ln -s $CURDIR/../../../Samples $BUILDPATH/jni
ln -s $CURDIR/../../../Dependencies $BUILDPATH/jni
ln -s $CURDIR/../../../../$NDK $BUILDPATH/jni
}

#echo "\$0=$0"
#echo "\$1=$1"
#echo "\$2=$2"
#echo "\$3=$3"
#exit 0

SOURCES=()
MKSOURCES=""
CURDIR0=`pwd -P`
CURDIR="$CURDIR0/../../Libraries/SDL2-$sdl2version/build-scripts"
cd "$CURDIR"

# Fetch sources
if [[ $# -ge 2 ]]; then
    for src in ${@:2}
    do
        SOURCES+=($src)
        MKSOURCES="$MKSOURCES $(basename $src)"
    done
else
    if [ -n "$1" ]; then
        while read src
        do
            SOURCES+=($src)
            MKSOURCES="$MKSOURCES $(basename $src)"
        done
    fi
fi

if [ -z "$1" ] || [ -z "$SOURCES" ]; then
    echo "Usage: androidbuild.sh com.yourcompany.yourapp < sources.list"
    echo "Usage: androidbuild.sh com.yourcompany.yourapp source1.c source2.c ...sourceN.c"
    echo "To copy SDL source instead of symlinking: COPYSOURCE=1 androidbuild.sh ... "
    echo "You can pass additional arguments to ndk-build with the NDKARGS variable: NDKARGS=\"-s\" androidbuild.sh ..."
    exit 1
fi

#SDLPATH="$( cd "$(dirname "$0")/.." ; pwd -P )"
SDLPATH="$CURDIR0/../../Libraries/SDL2-$sdl2version"

cp $CURDIR0/android_fopen.h $SDLPATH/include
cp $CURDIR0/android_fopen.c $SDLPATH/src/core/android
cp $CURDIR0/Android.mk $SDLPATH
cp $CURDIR0/Application.mk $SDLPATH/android-project/jni
cp $CURDIR0/SDLActivity.java $SDLPATH/android-project/src/org/libsdl/app
#cp $CURDIR0/SDLmainService.java $SDLPATH/android-project/src/org/libsdl/app
cp $CURDIR0/SDL_android.c $SDLPATH/src/core/android
cp $CURDIR0/SDL_android_main.c $SDLPATH/src/main/android
cp $CURDIR0/AndroidManifest.xml $SDLPATH/android-project
#cp $CURDIR0/ForegroundService.java $SDLPATH/android-project/src/org/libsdl/app

NDKBUILD=`which ndk-build`
if [ -z "$NDKBUILD" ];then
    echo "Could not find the ndk-build utility, install Android's NDK and add it to the path"
    exit 1
fi

ANDROID=`which android`
if [ -z "$ANDROID" ];then
    echo "Could not find the android utility, install Android's SDK and add it to the path"
    exit 1
fi

ANT=`which ant`
if [ -z "$ANT" ];then
    echo "Could not find the ant utility, install ant and add it to the path"
    exit 1
fi

ADB=`which adb`
if [ -z "$ADB" ];then
    echo "Could not find the adb utility, install Android's SDK and add it to the path"
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

APP="$1"
APPARR=(${APP//./ })
BUILDPATH="$SDLPATH/build/$APP"

export NDK_PROJECT_PATH="$BUILDPATH"

# Start Building

#rm -rf $BUILDPATH
#mkdir -p $BUILDPATH

cp -r $SDLPATH/android-project/* $BUILDPATH

# Copy SDL sources
mkdir -p $BUILDPATH/jni/SDL
if [ -z "$COPYSOURCE" ]; then
    ln -s $SDLPATH/src $BUILDPATH/jni/SDL
    ln -s $SDLPATH/include $BUILDPATH/jni/SDL
else
    cp -r $SDLPATH/src $BUILDPATH/jni/SDL
    cp -r $SDLPATH/include $BUILDPATH/jni/SDL
fi

cp -r $SDLPATH/Android.mk $BUILDPATH/jni/SDL
sed -i.bu "s|YourSourceHere.c|$MKSOURCES|g" $BUILDPATH/jni/src/Android.mk
sed -i.bu "s|org\.libsdl\.app|$APP|g" $BUILDPATH/AndroidManifest.xml

# Copy user sources
for src in "${SOURCES[@]}"
do
    cp $src $BUILDPATH/jni/src
done

# Create an inherited Activity
cd $BUILDPATH/src
for folder in "${APPARR[@]}"
do
    mkdir -p $folder
    cd $folder
done

ACTIVITY="${folder}Activity"
sed -i.bu "s|SDLActivity|$ACTIVITY|g" $BUILDPATH/AndroidManifest.xml
sed -i.bu "s|SDLActivity|$APP|g" $BUILDPATH/build.xml

# Fill in a default Activity
echo "package $APP;" >  "$ACTIVITY.java"
echo "import org.libsdl.app.SDLActivity;" >> "$ACTIVITY.java"
echo "public class $ACTIVITY extends SDLActivity {}" >> "$ACTIVITY.java"

if [ -z "$3" ]; then
	NDK;
else
	NDK1;
fi

# Update project and build
cd $BUILDPATH
$ANDROID update project --path . --target "android-19" --subprojects

exit 0

$NDKBUILD V=1 -j $NCPUS $NDKARGS
$ANT debug

cd $CURDIR

APK="$BUILDPATH/bin/$APP-debug.apk"

if [ -f "$APK" ]; then
    echo "Your APK is ready at $APK"
    echo "To install to your device: "
    echo "cd  $BUILDPATH"
    echo "ant debug install"
    exit 0
fi

echo "1:There was an error building the APK"
exit 1
