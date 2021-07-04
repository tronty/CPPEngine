#!/bin/bash
source ../../../make.cnf
installresources() {
   TOPDIR_="./Documents/IndieLib_resources"
   if [ -n "$1" ]
   then
	TOPDIR_="$1/Documents/IndieLib_resources"
   fi

#mkdir -p "$TOPDIR_"

CURDIR=`pwd -P`


}
#
# runsim   Install and run apps in the iOS Simulator
#
# Copyright (c) 2012 Psellos   http://psellos.com/
# Licensed under the MIT License:
#     http://www.opensource.org/licenses/mit-license.php
#
USAGE='usage:  runsim  [ -i { phone | pad } ] [ -srdl ]  executable  file ...'
#
# -iphone   Install as iPhone app
# -ipad     Install as iPad app
# -s        Start iOS Simulator
# -r        Run the app in the simulator
# -d        Delete the installed app
# -l        List names of installed apps
#
# file ...  Additional files to install with the executable
#
# Default flags are -iphone -s (install as iPhone app and start simulator).
#
# Currently the -r flag uses Instruments and thus requires
# authentication as a member of the _developer group.
#
VERSION=2.0.0

INSTALL=n
START=n
RUN=n
DELETE=n
LIST=n
while getopts i:srdl opt; do
    case "$opt" in
    i)
        INSTALL=y
        case "$OPTARG" in
        phone) FAMILY=1 ;;
        pad) FAMILY=2 ;;
        *)
            echo "runsim: unrecognized device family: $OPTARG" >&2
            echo "$USAGE" >&2
            exit 1
            ;;
        esac
        ;;
    s) START=y ;;
    r) RUN=y ;;
    d) DELETE=y ;;
    l) LIST=y ;;
    ?) echo "$USAGE" >&2; exit 1 ;;
    esac
done
shift $(($OPTIND - 1))

case "$INSTALL$START$RUN$DELETE$LIST" in nnnnn)
    INSTALL=y
    FAMILY=1
    START=y
esac

if [ "$INSTALL$RUN$DELETE" != nnn -a $# -lt 1 ]; then
    echo 'runsim: need an executable name for -i -r or -d' >&2
    echo "$USAGE" >&2
    exit 1
fi
EXEC="$1"
shift

#APPDIR="$HOME/Library/Application Support/iPhone Simulator/6.0/Applications"
APPDIR1="$HOME/Library/Developer/CoreSimulator/Devices/63233E1B-5676-4A43-99B2-17FE5E0685E0/data/Containers/Bundle/Application/$(GEN_UUID)"
APPDIR2="$HOME/Library/Developer/Xcode/DerivedData/$1-dkweevvyzfvqrsdkcdagnzeprznr/Build/Products/Debug-iphonesimulator"

TRCSUB=Contents/Applications/Instruments.app/Contents/PlugIns/AutomationInstrument.bundle/Contents/Resources/Automation.tracetemplate

xcodeloc() {
    # Get location of Xcode, otherwise use default
    if [ -f runsim.xcloc ]; then
        cat runsim.xcloc
    else
        echo /Applications/Xcode.app
    fi
}

appuuid() {
    # Get UUID for an app. If installed, re-use existing one. Otherwise
    # create a new one and return it.
    #
    for f in "$APPDIR"/*/"$1.app" ; do
        if [ -d "$f" ]; then
            basename "$(dirname "$f")"
            return 0
        fi
    done
    uuidgen
}

install2() {

   APPDIR=""
   if [ -n "$1" ]
   then
	APPDIR="$1"
   fi

   UUID=""
   if [ -n "$2" ]
   then
	UUID="/$2"
   fi


    # Install app and associated files.
    #
    TOPDIR="$APPDIR/$UUID"
    TOPDIR2="$TOPDIR"
    EXEC2="$mainphoneproject"
    mkdir -p "$TOPDIR"
    mkdir -p "$TOPDIR/Documents"
    mkdir -p "$TOPDIR/Library"
    mkdir -p "$TOPDIR/tmp"
    installresources "$TOPDIR"
    cp "$EXEC" "$TOPDIR2"
    if [ "$NIBFILE" != "" ]; then
        cp "$NIBFILE.nib" "$TOPDIR2"
    elif [ "$STORYFILE" != "" ]; then
        cp "$STORYFILE.storyboard" "$TOPDIR2"
    fi
    # If an Info.plist exists, use it.  Otherwise make one.
    if [ -f Info.plist ] ; then
        plutil -convert xml1 -o "$TOPDIR2/Info.plist" Info.plist
    else
        cat > "$TOPDIR2/Info.plist" <<HERE1
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN"\
 "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
        <key>CFBundleDevelopmentRegion</key>
        <string>English</string>
        <key>CFBundleDisplayName</key>
        <string>$EXEC2</string>
        <key>CFBundleExecutable</key>
        <string>$EXEC2</string>
        <key>CFBundleIconFile</key>
        <string>Icon.png</string>
        <key>CFBundleIdentifier</key>
        <string>com.example.$EXEC2</string>
        <key>CFBundleInfoDictionaryVersion</key>
        <string>6.0</string>
        <key>CFBundleName</key>
        <string>$EXEC2</string>
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
HERE1
        if [ "$NIBFILE" != "" ]; then
            cat >> "$TOPDIR2/Info.plist" << HERE2
        <key>NSMainNibFile</key>
        <string>$NIBFILE</string>
HERE2
        elif [ "$STORYFILE" != "" ]; then
            cat >> "$TOPDIR2/Info.plist" << HERE3
        <key>NSMainStoryboardFile</key>
        <string>$STORYFILE</string>
HERE3
        fi
        cat >> "$TOPDIR2/Info.plist" <<HERE4
</dict>
</plist>
HERE4
    fi
    # Add device specifications to Info.plist (normally done by Xcode).
    # Without these, Instruments reports the app as AWOL.
    #
    python -c '
import plistlib
import sys
p = plistlib.readPlist(sys.argv[1])
p["CFBundleSupportedPlatforms"] = ["iPhoneSimulator"]
p["DTPlatformName"] = "iphonesimulator"
p["DTSDKName"] = "iphonesimulator6.0"
p["UIDeviceFamily"] = ['$FAMILY']
plistlib.writePlist(p, sys.argv[1])
' "$TOPDIR2/Info.plist"

    echo -n 'AAPL????' > "$TOPDIR2/PkgInfo"
    # Install conventional image files if they exist.
    #
    if [ -f Icon.png ]; then
        cp Icon.png "$TOPDIR2"
    fi
    if [ -f Default.png ]; then
        cp Default.png "$TOPDIR2"
    fi
    # Install any other given files.
    #
    for f; do
        if [ "$f" = "$NIBFILE.nib" ]; then continue; fi
        if [ "$f" = "$STORYFILE.storyboard" ]; then continue; fi
        cp "$f" "$TOPDIR2"
    done
}

install() {
    # Install executable $EXEC and associated files into simulator's
    # file system.
    #

    # Figure out startup file, if any. If a nibfile or storyboard file
    # is given, the first one is the startup file. Otherwise if there's
    # a file $EXEC.nib or $EXEC.storyboard, that is the startup file.
    # Otherwise there is no startup file.
    #
    NIBFILE=
    STORYFILE=
    if [ -f "$EXEC.nib" ]; then
        NIBFILE="$EXEC"
    elif [ -f "$EXEC.storyboard" ]; then
        STORYFILE="$EXEC"
    fi
    for f ; do
        case "$f" in
        *.nib)
            STORYFILE=; NIBFILE="$(basename "$f" .nib)"; break ;;
        *.storyboard)
            NIBFILE=; STORYFILE="$(basename "$f" .storyboard)"; break ;;
        esac
    done
    UUID=$(appuuid "$EXEC")

    install2 $APPDIR1 $UUID;
    install2 $APPDIR2 $UUID;
}

start() {
    # Start the iOS Simulator
    #
    #open "$(xcodeloc)"/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/Applications/iPhone\ Simulator.app
    open "$(xcodeloc)"/Contents/Developer/Applications/Simulator.app
    #open /Applications/Xcode.app/Contents/Developer/Applications/Simulator.app
}


run() {
    # Run the app inside iOS Simulator by asking Instruments to trace it
    # with null trace. If you haven't agreed to the licensing terms of
    # Xcode, this will fail until you do.  The first time in each login
    # session, this will ask for authentication as an admin or
    # developer.
    #
    TOPDIR="$APPDIR/$(appuuid "$EXEC")"
    if [ ! -d "$TOPDIR2" ]; then
        echo "runsim: app \"$EXEC\" not installed" >&2
        exit 1
    fi
    (instruments -D /tmp/runsim$$.trace -t "$(xcodeloc)/$TRCSUB" \
            "$TOPDIR2" < /dev/null 2>&1 > /dev/null | \
            grep 'xcodebuild -license' >&2 ; \
        rm -rf /tmp/runsim$$.trace) &
}

delete() {
    # Delete an installed app.
    #
    TOPDIR="$APPDIR/$(appuuid "$EXEC")"
    if [ ! -d "$TOPDIR" ]; then
        echo "runsim: app \"$EXEC\" not installed" >&2
        exit 1
    fi
    rm -rf "$TOPDIR"
}

list() {
    # List installed apps.
    #
    for f in "$APPDIR"/*/*.app ; do
        if [ -d "$f" ]; then
            basename "$f" .app
        fi
    done
}


case $INSTALL in y) install "$@" ;; esac
case $START in y) start ;; esac
case $RUN in y) run ;; esac
case $DELETE in y) delete ;; esac
case $LIST in y) list ;; esac
