#!/bin/bash
source ../../make.cnf

if [ -a ../../count.txt ]; then
	num=$(<../../count.txt)
	num2=$((num + 1)) 
	echo "$num2" > ../../count.txt
else
	echo "1" > ../../count.txt
fi
#echo "num:$num"

NDK1()
{

rm -fr $BUILDPATH/libs
mkdir -p $BUILDPATH/libs
#rm -fr $BUILDPATH/assets/IndieLib_resources
#mkdir -p $BUILDPATH/assets/IndieLib_resources

ln -s $CURDIR/../../../Samples $BUILDPATH/jni
ln -s $CURDIR/../../../Dependencies $BUILDPATH/jni
ln -s $CURDIR/../../../../$NDK $BUILDPATH/jni

ln -s $CURDIR/../../../Libraries/BoostWorkaround $BUILDPATH/jni/Libraries
}

NDK()
{

rm -fr $BUILDPATH/libs
mkdir -p $BUILDPATH/libs
#rm -fr $BUILDPATH/assets/IndieLib_resources
#mkdir -p $BUILDPATH/assets/IndieLib_resources

ln -s $CURDIR/../../../Samples $BUILDPATH/jni
ln -s $CURDIR/../../../Dependencies $BUILDPATH/jni
ln -s $CURDIR/../../../../$NDK $BUILDPATH/jni

ln -s $CURDIR/../../../Libraries/BoostWorkaround $BUILDPATH/jni/Libraries
}

#echo "\$0=$0"
#echo "\$1=$1"
#echo "\$2=$2"
#echo "\$3=$3"
#exit 0

CURDIR0=`pwd -P`
CURDIR="$CURDIR0/../../Libraries/MVC/NDK"

if [ -z "$1" ]; then
    echo "Usage: androidbuild.sh com.yourcompany.yourapp < sources.list"
    echo "Usage: androidbuild.sh com.yourcompany.yourapp source1.c source2.c ...sourceN.c"
    echo "To copy SDL source instead of symlinking: COPYSOURCE=1 androidbuild.sh ... "
    echo "You can pass additional arguments to ndk-build with the NDKARGS variable: NDKARGS=\"-s\" androidbuild.sh ..."
    exit 1
fi

#APPPATH="$( cd "$(dirname "$0")/.." ; pwd -P )"
APPPATH="$CURDIR0/../../Libraries/MVC/NDK"

NDKBUILD=`which ndk-build`
#if [ -z "$NDKBUILD" ];then
#    echo "Could not find the ndk-build utility, install Android's NDK and add it to the path"
#    exit 1
#fi

ANDROID=`which android`
#if [ -z "$ANDROID" ];then
#    echo "Could not find the android utility, install Android's SDK and add it to the path"
#    exit 1
#fi

ANT=`which ant`
#if [ -z "$ANT" ];then
#    echo "Could not find the ant utility, install ant and add it to the path"
#    exit 1
#fi

ADB=`which adb`
#if [ -z "$ADB" ];then
#    echo "Could not find the adb utility, install Android's SDK and add it to the path"
#    exit 1
#fi

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
BUILDPATH="$APPPATH"

export NDK_PROJECT_PATH="$BUILDPATH"

# Start Building

# APPPATH="$CURDIR0/../../Libraries/MVC/NDK"

CURDIR1=`pwd -P`
echo "CURDIR1=$CURDIR1"

# cp: tiedoston ”/home/tronty/IndieLibX/DesktopProjects/NDK_gl_code2/Android.mk” tilaa ei voi lukea: Tiedostoa tai hakemistoa ei ole

#rm -fr $APPPATH
mkdir -p $APPPATH/jni/MVC
# cp -fr ./hello-gl2/*.* $APPPATH

#echo "#:8:cp $CURDIR0/AndroidManifest.xml $APPPATH"
cp $CURDIR0/AndroidManifest.xml $APPPATH
#echo "#:9:cp $CURDIR0/Android.mk $APPPATH/jni"
cp $CURDIR0/Android.mk $APPPATH/jni
#echo "#:10:cp $CURDIR0/Application.mk $APPPATH/jni"
cp $CURDIR0/Application.mk $APPPATH/jni

#echo "#:11:cp $CURDIR0/android_fopen.h $APPPATH/jni/MVC"
cp $CURDIR0/android_fopen.h $APPPATH/jni/MVC

#echo "#:12:cp $CURDIR0/hello-gl2/default.properties $APPPATH"
cp $CURDIR0/hello-gl2/default.properties $APPPATH
#echo "#:13:cp -fr $CURDIR0/hello-gl2/res $APPPATH"
cp -fr $CURDIR0/hello-gl2/res $APPPATH  
#echo "#:14:cp -fr $CURDIR0/hello-gl2/src $APPPATH"
cp -fr $CURDIR0/hello-gl2/src $APPPATH

rm -fr $APPPATH/libs
mkdir -p $APPPATH/libs

cd "$CURDIR"

#echo "androidbuild.sh:1"

#if [ -z "$3" ]; then
#	NDK;
#else
#	NDK1;
#fi
NDK1;

#echo "androidbuild.sh:2"

# ??? sed -i.bu "s|org\.libsdl\.app|$APP|g" $BUILDPATH/AndroidManifest.xml

rm -fr $APPPATH/jni/SDL2

cat > "$APPPATH/jni/Android.mk" <<HERE1
include \$(call all-subdir-makefiles)
HERE1

#cat > "$APPPATH/res/values/strings.xml" <<HERE2
#<?xml version="1.0" encoding="utf-8"?>
#<resources>
#    <string name="gl2jni_activity">GL2JNI_${mainphoneproject}_$num</string>
#</resources>
#HERE2

sed -i.bu "s|>GL2JNI<|>GL2JNI_${mainphoneproject}_$num<|g" $APPPATH/res/values/strings.xml
rm -f $APPPATH/res/values/strings.xml.bu

# Update project and build
cd $BUILDPATH

#echo "androidbuild.sh:3"
if [ -n "$ANDROID" ]; then
$ANDROID update project --path . --target "android-19" --subprojects
fi

#echo "androidbuild.sh:4"

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

echo "3:There was an error building the APK"
exit 1
