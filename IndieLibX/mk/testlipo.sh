#!/bin/bash
#
# Copyright (c) 2016 Tommi Roenty   http://www.elisanet.fi/tommi.ronty/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#

cd /Users/tronty/IndieLibX_Libraries/IndieLibX_AllLibs_iOS___/

BUILD_TYPE=Debug

types=( arm64 armv7 i386 )

#libassimp3_x-i386-Debug.a

libs=( assimp3_x Framework3 opentree otlmesher Image3 otlrenderer IndieLib_Static otlutils Lua_x spine_x MeshRenderer2 tinyxml_x NSGUI weber STX weberarbaro )

for lib in "${libs[@]}"
do
	lipo -output lib${lib}.a -create -arch armv7 ./lib${lib}-armv7-$BUILD_TYPE.a 

	#-arch arm64 ./lib${lib}-arm64-$BUILD_TYPE.a
	#-arch i386 ./lib${lib}-i386-$BUILD_TYPE.a 
done

#cd "$HOME/IndieLibX"
