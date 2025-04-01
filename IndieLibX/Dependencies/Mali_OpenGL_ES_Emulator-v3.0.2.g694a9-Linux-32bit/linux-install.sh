#!/bin/bash

set -e

# Installation prefix
PREFIX=/usr
BIN_DIR=$PREFIX/bin
LIB_DIR=$PREFIX/lib
INC_DIR=$PREFIX/include
SHARE_DIR=$PREFIX/share

LD_SO_CONF_DIR=/etc/ld.so.conf.d
LD_SO_CONF=$LD_SO_CONF_DIR/25-mali-opengl-es-emulator.conf

UNINSTALLER=./linux-uninstall.sh

# Check if we're being run by root
if ! { mkdir -p "$DESTDIR/" && [ -w "$DESTDIR/" ]; }; then
    echo "This script should be run as root."
    exit 1
fi

# Display the welcome message
echo "OpenGL ES Emulator Linux Installer"
echo ""
echo "This script will install the OpenGL ES Emulator in the following locations:"
echo ""
echo "    $DESTDIR$BIN_DIR        OpenGL ES Emulator Binaries"
echo "    $DESTDIR$LIB_DIR        OpenGL ES Emulator Libraries"
echo "    $DESTDIR$INC_DIR    OpenGL ES Header Files"
echo "    $DESTDIR$SHARE_DIR/doc  OpenGL ES Emulator documentation"
echo ""

if [ "$BATCH" != true ]; then
    # Wait for input
    read -n1 -r -s -p "Press any key to continue..." INPUT
    echo ""
fi

# creating uninstaller file
echo "#!/bin/bash" > $UNINSTALLER
echo "# ARM Mali OpenGL Emulator uninstaller" >> $UNINSTALLER

# creating section in uninstaller that checks for root permissions
echo 'if [ $LOGNAME != "root" ]; then' >> $UNINSTALLER
echo '    echo "This script should be run as root."' >> $UNINSTALLER
echo '    exit 1' >> $UNINSTALLER
echo 'fi' >> $UNINSTALLER

chmod +x $UNINSTALLER

# Copy the binaries to $BIN_DIR
echo "Copying OpenGL ES Emulator Binaries to $DESTDIR$BIN_DIR..."

if [ ! -d "$DESTDIR$BIN_DIR" ]; then
    mkdir -p $DESTDIR$BIN_DIR
fi

# Copy the binaries
cp -av bin/* $DESTDIR$BIN_DIR/
find -L bin -type f| sed -e "s@bin/@rm  $BIN_DIR/@" >> $UNINSTALLER

# Copy the libraries to $LIB_DIR
echo "Copying OpenGL ES Emulator Libraries to $DESTDIR$LIB_DIR..."

if [ ! -d "$DESTDIR$LIB_DIR" ]; then
    mkdir -p $DESTDIR$LIB_DIR
fi

# Copy the libraries to their destination directory
cp -av lib/* $DESTDIR$LIB_DIR/
find lib/mali* -type d | sed -e "s@lib/@rm -rf $LIB_DIR/@" >> $UNINSTALLER

# Create the ld.so.conf
echo "Creating $DESTDIR$LD_SO_CONF..."

if [ ! -d "$DESTDIR$LD_SO_CONF_DIR" ]; then
    mkdir -p $DESTDIR$LD_SO_CONF_DIR
fi

echo "$LIB_DIR/mali-opengl-es-emulator" > "$DESTDIR$LD_SO_CONF"
echo rm "$LD_SO_CONF" >> "$UNINSTALLER"

# Copy the header files to $INC_DIR
echo "Copying OpenGL ES Emulator Header Files to $INC_DIR..."

if [ ! -d "$DESTDIR$INC_DIR" ]; then
    mkdir -p $DESTDIR$INC_DIR
fi

# Copy the headers
cp -avR include/* $DESTDIR$INC_DIR/
find include/* -type f | sed -e "s@include/@rm $INC_DIR/@" >> $UNINSTALLER

# Copy the shared files to $SHARED_DIR
echo "Copying Documentation and Other Resources Files to $DESTDIR$SHARE_DIR..."

if [ ! -d "$DESTDIR$SHARE_DIR" ]; then
    mkdir -p $DESTDIR$SHARE_DIR
fi

# Copy the headers
cp -avR share/* $DESTDIR$SHARE_DIR/
find share -type f | sed -e "s@share/@rm \"$SHARE_DIR/@;s/$/\"/" >> $UNINSTALLER
find share/doc/* -type d  | sed -e "s@share/@rm -rf $SHARE_DIR/@"  >> $UNINSTALLER
find share/mali* -type d  | sed -e "s@share/@rm -rf $SHARE_DIR/@" >> $UNINSTALLER

# re-build library cache if installing directly
[ -z "$DESTDIR" ] && ldconfig


# Display a message saying we're done
echo "...done."
