#!/bin/bash

createInfoplist() {

   if [ -n "$1" ]
   then
	FRAMEWORK_DIR=$1
   fi

   if [ -n "$2" ]
   then
	FRAMEWORK_NAME=$2
   fi

    #$FRAMEWORK_DIR/Resources/Info.plist
    if [ 1 ] ; then
        cat > "$FRAMEWORK_DIR/Resources/Info.plist" <<HERE1
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN"\
 "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
        <key>CFBundleDevelopmentRegion</key>
        <string>English</string>
        <key>CFBundleDisplayName</key>
        <string>$FRAMEWORK_NAME</string>
        <key>CFBundleExecutable</key>
        <string>$FRAMEWORK_NAME</string>
        <key>CFBundleIconFile</key>
        <string>Icon.png</string>
        <key>CFBundleIdentifier</key>
        <string>com.example.$FRAMEWORK_NAME</string>
        <key>CFBundleInfoDictionaryVersion</key>
        <string>6.0</string>
        <key>CFBundleName</key>
        <string>$FRAMEWORK_NAME</string>
        <key>CFBundlePackageType</key>
        <string>APPL</string>
        <key>CFBundleSignature</key>
        <string>????</string>
        <key>CFBundleShortVersionString</key>
        <string>1.0</string>
        <key>CFBundleVersion</key>
        <string>1.0.0</string>
        <key>UIStatusBarStyle</key>
        <string>UIStatusBarStyleBlackOpaque</string>
        <key>LSRequiresIPhoneOS</key>
        <true/>
</dict>
</plist>
HERE1

    fi

    # Add device specifications to Info.plist (normally done by Xcode).
    # Without these, Instruments reports the app as AWOL.
    #
    iphonesimulator="iphonesimulator"
    iPhoneSimulator="iPhoneSimulator"
    if [ "$i386" != "y" ]; then
	iphonesimulator="iphoneos"
	iPhoneSimulator="iPhoneOS"
    fi
    FAMILY2=1 # phone
    if [ "$FAMILY" == "y" ]; then
    	FAMILY2=2 # pad
    fi
    python -c '
import plistlib
import sys
p = plistlib.readPlist(sys.argv[1])
p["CFBundleSupportedPlatforms"] = ["'+$iPhoneSimulator+'"]
p["DTPlatformName"] = "'+$iphonesimulator+'"
p["DTSDKName"] = "'+$iphonesimulator+'6.0"
p["UIDeviceFamily"] = ['$FAMILY2']
plistlib.writePlist(p, sys.argv[1])
' "$FRAMEWORK_DIR/Resources/Info.plist"

} 

name=''
arm6=n
arm7=n
i386=n
arm64=n
debug=n
FAMILY=n
dir=../../../lib
dir2=../../../../IndieLibX_Libraries

usage() { echo "Usage: $0 [-a <string>] [-b <y|n>] [-c <y|n>] [-d <y|n>] [-e <y|n>] [-f <y|n>] [-g <y|n>] [-h <string>]" 1>&2; exit 1; }

while getopts ":a:b:c:d:e:f:g:h:" o; do
    case "${o}" in
        a)
            name=${OPTARG}
            ;;
        b)
            arm6=${OPTARG}
            ;;
        c)
            arm7=${OPTARG}
            ;;
        d)
            i386=${OPTARG}
            ;;
        e)
            arm64=${OPTARG}
            ;;
        f)
            debug=${OPTARG}
            ;;
        g)
            FAMILY=${OPTARG}
            ;;
        h)
            dir=${OPTARG}
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

if [ -z "${name}" ] || [ -z "${arm6}" ] || [ -z "${arm7}" ] || [ -z "${i386}" ] || [ -z "${arm64}" ] || [ -z "${debug}" ] || [ -z "${FAMILY}" ]; then
    usage
fi


    # Define these to suit your nefarious purposes  
                     FRAMEWORK_NAME=$name  
                  FRAMEWORK_VERSION=A  
          FRAMEWORK_CURRENT_VERSION=1  
    FRAMEWORK_COMPATIBILITY_VERSION=1  
                         BUILD_TYPE=Debug

    if [ "$debug" != "y" ]; then
	BUILD_TYPE=Release
    fi

    # Check that this is what your static libraries  
    # are called 
    FRAMEWORK_INPUT_ARM_FILES=  
    FRAMEWORK_INPUT_ARM7_FILES=
    FRAMEWORK_INPUT_I386_FILES=
    FRAMEWORK_INPUT_ARM64_FILES=
    FRAMEWORK_FILE_="$dir2/IndieLibX_AllLibs_iOS/lib$FRAMEWORK_NAME-$BUILD_TYPE.framework"
    #rm -fr $dir2/IndieLibX_AllLibs_iOS
    mkdir -p $dir2/IndieLibX_AllLibs_iOS

    if [ "$arm6" != "n" ]; then
	FRAMEWORK_INPUT_ARM_FILES=
	#"-arch arm6 $dir/lib$FRAMEWORK_NAME-arm6-$BUILD_TYPE.a"
    fi

    if [ "$arm7" != "n" ]; then
	FRAMEWORK_INPUT_ARM7_FILES="-arch armv7 $dir/lib$FRAMEWORK_NAME-armv7-$BUILD_TYPE.a"
    fi

    if [ "$i386" != "n" ]; then
	FRAMEWORK_INPUT_I386_FILES="-arch i386 $dir/lib$FRAMEWORK_NAME-i386-$BUILD_TYPE.a"
    fi

    if [ "$arm64" != "n" ]; then
	FRAMEWORK_INPUT_ARM64_FILES=
	#"-arch arm64 $dir/lib$FRAMEWORK_NAME-arm64-$BUILD_TYPE.a"
    fi
 
    # Where we'll put the build framework.  
    # The script presumes we're in the project root  
    # directory. Xcode builds in "build" by default  
    FRAMEWORK_BUILD_PATH="$dir"  
 
    # Clean any existing framework that might be there  
    # already  
    echo "Framework: Cleaning framework..."  
    [ -d "$FRAMEWORK_BUILD_PATH" ] && rm -rf "$FRAMEWORK_BUILD_PATH/$FRAMEWORK_NAME.framework"  
 
    # This is the full name of the framework we'll  
    # build  
    FRAMEWORK_DIR=$FRAMEWORK_BUILD_PATH/$FRAMEWORK_NAME.framework 
 
    # Build the canonical Framework bundle directory  
    # structure  
    echo "Framework: Setting up directories..."  
    mkdir -p $FRAMEWORK_DIR/Versions/Current
    mkdir -p $FRAMEWORK_DIR/Versions/$FRAMEWORK_VERSION/Resources
    mkdir -p $FRAMEWORK_DIR/Versions/$FRAMEWORK_VERSION/Headers
  
    echo "Framework: Creating symlinks..."  
    ln -s $FRAMEWORK_VERSION $FRAMEWORK_DIR/Versions/Current
    ln -s Versions/Current/Headers $FRAMEWORK_DIR/Headers
    ln -s Versions/Current/Resources $FRAMEWORK_DIR/Resources
    ln -s Versions/Current/$FRAMEWORK_NAME $FRAMEWORK_DIR/$FRAMEWORK_NAME

    # The trick for creating a fully usable library is  
    # to use lipo to glue the different library  
    # versions together into one file. When an  
    # application is linked to this library, the  
    # linker will extract the appropriate platform  
    # version and use that.  
    # The library file is given the same name as the  
    # framework with no .a extension.  
    echo "Framework: Creating library..."
    #lipo -create libdevice.a libsimulator.a -output libcombined.a
    # lipo -output $dir/libOmnitureAppMeasurement-fat.a -create -arch arm6 libOmnitureAppMeasurement-iPhoneDevice.a -arch l386 libOmnitureAppMeasurement-iPhoneSimulator_4_0_GM.a
	
    echo "lipo -output libcombined.a -create $FRAMEWORK_INPUT_ARM_FILES $FRAMEWORK_INPUT_ARM7_FILES $FRAMEWORK_INPUT_I386_FILES $FRAMEWORK_INPUT_ARM64_FILES"
    if [[ $OSTYPE == darwin* ]]; then
	lipo -output libcombined.a -create $FRAMEWORK_INPUT_ARM_FILES $FRAMEWORK_INPUT_ARM7_FILES $FRAMEWORK_INPUT_I386_FILES $FRAMEWORK_INPUT_ARM64_FILES
	if [ -f ./libcombined.a ]; then
		mv ./libcombined.a $FRAMEWORK_DIR/Versions/Current/$FRAMEWORK_NAME
	fi
    fi

    # Now copy the final assets over: your library  
    # header files and the plist file  
    echo "Framework: Copying assets into current version..."  
    #cp Include/$FRAMEWORK_NAME/* $FRAMEWORK_DIR/Headers/  
    #cp Resources/Framework.plist $FRAMEWORK_DIR/Resources/Info.plist
    if [ 0 ] ; then
    tee $FRAMEWORK_DIR/Resources/Info.plist <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>CFBundleDevelopmentRegion</key>
	<string>en</string>
	<key>CFBundleExecutable</key>
	<string>$FRAMEWORK_NAME</string>
	<key>CFBundleIdentifier</key>
	<string>dev.mxcl.x</string>
	<key>CFBundleInfoDictionaryVersion</key>
	<string>6.0</string>
	<key>CFBundleName</key>
	<string>$FRAMEWORK_NAME</string>
	<key>CFBundlePackageType</key>
	<string>FMWK</string>
	<key>CFBundleShortVersionString</key>
	<string>1.0.7</string>
	<key>CFBundleSignature</key>
	<string>????</string>
	<key>CFBundleVersion</key>
	<string>$FRAMEWORK_CURRENT_VERSION</string>
	<key>NSPrincipalClass</key>
	<string></string>
</dict>
</plist>
EOF
	else
		createInfoplist FRAMEWORK_DIR FRAMEWORK_NAME;
	fi
	mv $FRAMEWORK_DIR $FRAMEWORK_FILE_

