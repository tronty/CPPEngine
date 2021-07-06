#!/bin/bash
#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#

CURDIR=`pwd -P`

NDK=$NDK
#/usr/local/lib/NDK/
#$CURDIR/$NDK/platforms/

platforms=( android-12  android-14  android-16  android-18  android-3  android-5  android-9 android-13  android-15  android-17  android-19  android-4  android-8 )

archs=( arch-arm  arch-mips  arch-x86 )

libs=( OpenAL_x assimp3 jpeg_x png_x zlib_x IL_x ILU_x weber weberarbaro opentree otlmesher otlrenderer forest otlutils spine STX SDL2 hlsl2glsl Framework3 MeshRenderer2 tinyxml IndieLib_Static NSGUI assimp3_x SDL2_x tinyxml_x)

#for varplatform in "${platforms[@]}"
#do
#for vararch in "${archs[@]}"
#do
#for varlib in "${libs[@]}"
#do
#	cp $CURDIR/IndieLibX/Libraries/MVC/NDK/build/a_02_IND_Surface/obj/local/armeabi/lib${varlib}.a /usr/local/lib/NDK/${vararch}/lib${varlib}.a
#done
#done
#done

for varplatform in "${platforms[@]}"
do
for vararch in "${archs[@]}"
do
for varlib in "${libs1[@]}"
do
  if [ -d "$CURDIR/$NDK/platforms/${varplatform}/${vararch}" ]; then
     rm -f $CURDIR/$NDK/platforms/${varplatform}/${vararch}/usr/lib/lib${varlib}.a
  fi
done
done
done

exit 0

for varplatform in "${platforms[@]}"
do
for vararch in "${archs[@]}"
do
for varlib in "${libs2[@]}"
do
  if [ -d "$CURDIR/$NDK/platforms/${varplatform}/${vararch}" ]; then
     rm -f $CURDIR/$NDK/platforms/${varplatform}/${vararch}/usr/lib/lib${varlib}.a
    ln -s /usr/local/lib/NDK/${vararch}/lib${varlib}.a $CURDIR/$NDK/platforms/${varplatform}/${vararch}/usr/lib/lib${varlib}.a
  fi
done
done
done

exit 0

exts=( a so )
for varext in "${exts[@]}"
do
FILES=$CURDIR/IndieLibX/lib/*.$varext
TARGETDIR=/usr/local/lib
for f in $FILES
do
	dir=${f%/*}
	file=${f##*/}
	if [ ! -f "$TARGETDIR/$file" ]; then
		sudo cp $f $TARGETDIR/.
	fi
done
done

archs=( armeabi  mips  x86 )
for varext in "${exts[@]}"
do
for vararch in "${archs[@]}"
do
case "$vararch" in
    armeabi*)
        arch=arch-arm  
	;; 
    mips*)
        arch=arch-mips  
        ;; 
    x86*)
        arch=arch-x86
        ;;
  *);;
esac
FILES=$CURDIR/IndieLibX/Libraries/MVC/NDK/build/$mainphoneproject/obj/local/$vararch/*.$varext
TARGETDIR=$CURDIR/$NDK/platforms/android-$NDKplatform/$arch/usr/lib
if [ -d "$TARGETDIR" ]; then
for f in $FILES
do
	dir=${f%/*}
	file=${f##*/}
	if [ ! -f "$TARGETDIR/$file" ]; then
		sudo cp $f $TARGETDIR/.
	fi
done
fi
done
done

