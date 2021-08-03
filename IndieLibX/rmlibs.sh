#!/bin/bash
#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#

CURDIR=`pwd -P`

platforms=( android-12  android-14  android-16  android-18  android-3  android-5  android-9 android-13  android-15  android-17  android-19  android-4  android-8 )

platforms=( android-19 )

archs=( arch-arm  arch-mips  arch-x86 )

libs=( SDL2.so OpenAL_x.a hlsl2glsl.a zlib_x.a png_x.a jpeg_x.a IL_x.a ILU_x.a STX.a Framework3.a main.so)

for varplatform in "${platforms[@]}"
do
for vararch in "${archs[@]}"
do
for varlib in "${libs[@]}"
do
  if [ -d "$CURDIR/$NDK/platforms/${varplatform}/${vararch}" ]; then
     rm -f $CURDIR/$NDK/platforms/${varplatform}/${vararch}/usr/lib/lib${varlib}
  fi
done
done
done

