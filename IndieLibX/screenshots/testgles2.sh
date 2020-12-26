#!/bin/bash
#
# Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#

CURDIR=`pwd -P`

sdl2version="2.0.8"
avd="avd19"

# echo "export PATH=$ANDROID_NDK:$ANDROID_SDK:$ANDROID_SDK/tools:$ANDROID_SDK/platform-tools:$PATH"

# If you have not created android avd named $avd create it with command:
# $ANDROID create avd -n $avd -t 1
# You need two console windows to run this script:
# On the first console: ./testgles2.sh 1 # starts an emulator instance
# On the second console: ./testgles2.sh # installs an apk package to the emulator

ANDROID=`which android`
if [ -z "$ANDROID" ]; then
	echo "Could not find android. Install it and add it to the path."
	exit 1
fi

ADB=`which adb`
if [ -z "$ADB" ]; then
	echo "Could not find adb. Install it and add it to the path."
	exit 1
fi

ANT=`which ant`
if [ -z "$ANT" ]; then
	echo "Could not find ant. Install it and add it to the path."
	exit 1
fi

EMULATOR=`which emulator`
if [ -z "$EMULATOR" ]; then
	echo "Could not find emulator. Install it and add it to the path."
	exit 1
fi

help()
{
	#echo "If you have not created android avd named $avd create it with command:"
	#echo "$ANDROID create avd -n $avd -t 1"
	echo "You need two console windows to run this script:"
	echo "On the first console: ./testgles2.sh 1 # starts an emulator instance"
	echo "On the second console: ./testgles2.sh # installs an apk package to the emulator"
}

emulator_images=`$ANDROID list avds | grep "Name:" | cut -f 2 -d ":"`
emulator_list=($emulator_images)
if [[ " ${emulator_list[@]} " =~ "$avd" ]]; then
	echo "$avd found."
else
	$ANDROID create avd -n $avd -t 1
fi

help;

if [ ! -z "$1" ]; then
	#$ANDROID list avds
	#$ANDROID list targets
	#$ANDROID create avd -n $avd -t 1
	$ADB logcat > NDK.log &
	$EMULATOR -force-32bit -partition-size 1024 -avd $avd -gpu on &
fi

if [ -z "$1" ]; then

cd $CURDIR
rm -f ./SDL2-$sdl2version.tar
rm -fr ./SDL2-$sdl2version
curl -O https://www.libsdl.org/release/SDL2-$sdl2version.tar.gz
gunzip -q ./SDL2-$sdl2version.tar.gz
tar -xf ./SDL2-$sdl2version.tar

cd $CURDIR/SDL2-$sdl2version/android-project
$ANDROID update project --path . --target "android-19"

cat > "$CURDIR/SDL2-$sdl2version/android-project/jni/Application.mk" <<HERE1
# Uncomment this if you're using STL in your project
# See CPLUSPLUS-SUPPORT.html in the NDK documentation for more information
# APP_STL := stlport_static 

APP_ABI := armeabi armeabi-v7a x86 # works on emulator
#APP_ABI := x86 # does not work on emulator
HERE1

cd ../build-scripts
./androidbuild.sh com.yourcompany.yourapp ../test/testgles2.c

cd $CURDIR/SDL2-$sdl2version/build/com.yourcompany.yourapp
$ANT debug install

fi

