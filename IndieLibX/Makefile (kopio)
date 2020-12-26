#
# Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
export shell = /bin/bash

include make.cnf

help:
	@echo "Usage:	make RadeonTerrainDemo -> Runs original RadeonTerrainDemo.exe with wine"
	@echo "Usage:	make NatureScene       -> Runs original NatureScene.exe with wine"
	@echo "Usage:	make Particles         -> Runs original Particles.exe with wine"
	@echo "Usage:	make ParticleVS        -> Runs original ParticleVS.exe with wine"
	@echo "Usage:	./make.sh help         -> Prints some usage notes for ./make.sh."
	@echo "Usage:	./make.sh AllLibs      -> Builds the static libraries needed in the linking phase."
	@echo "Usage:	./make.sh Test         -> Builds some C++ Test Applications."
	@echo "Usage:	./make.sh SampleApps   -> Builds some C++ Sample Applications."
	@echo "Usage:	./make.sh TerrainApps  -> Builds some C++ Terrain Applications."
	@echo "Usage:	./make.sh IndieLibApps -> Builds some C++ IndieLib Applications."
	@echo "Usage:	./make.sh TestApps     -> Builds All C++ Working Applications."
	@echo "Usage:	./run.sh               -> Runs the built applications."
	@echo "Usage:	./make.sh              -> Removes temporary files."
	@echo ""

all: help

newer:
	touch -t 201806221231.43 ./wotsit
	find ./Libraries/Framework3 -newer ./wotsit -print
	rm -f ./wotsit
	#touch -t 201806221231.43 ./wotsit
	#find ./Samples -newer ./wotsit -print
	#rm -f ./wotsit

newer2:
	touch -t 201806251231.43 ./wotsit
	find ../IndieLib_resources -newer ./wotsit -print
	rm -f ./wotsit

recursivefilesize:
	du -ah . | grep -v "/$" | sort -rh

recursivefiletouch:
	find  -type f  -exec touch {} +
	find  -type d  -exec touch {} +

recursivefiletouch2:
	# touch -a -m -t 201512180130.09 fileName.ext
	find  -type f  -exec touch -a -m -t 202002142020.00 {} +
	find  -type d  -exec touch -a -m -t 202002142020.00 {} +

recursivefilereplace:
	grep -rli 'SDL2-2.0.8' * | xargs -i@ sed -i 's/SDL2-2.0.8/SDL2-2.0.10/g' @

filesizesinsortedorderrecursively1:
	find . -type f -printf '%s %p\n' | sort -rn

filesizesinsortedorderrecursively2:
	du -h . | sort -rh

modifyaccess:
    find . -type f -exec chmod a+w {} \;
    find . -type d -exec chmod a+w {} \;

diff3:
	#cd ./Samples/GeneratingaDungeonbytesandstuff/DungeonGeneration01_03
	diff ./Samples/GeneratingaDungeonbytesandstuff/DungeonGeneration01_03/GeneratedMap1.cpp ./Samples/GeneratingaDungeonbytesandstuff/DungeonGeneration01_03/GeneratedMap2.cpp
	#cd ../../..

diff:
	diff /home/tronty/IndieLibX_NSGUI_ei_toimi/Libraries/Framework3/src/RendererGLSLGL_1_1.cpp ./Libraries/Framework3/src/RendererGLSLGL_1_1.cpp
	diff /home/tronty/IndieLibX_NSGUI_ei_toimi/Libraries/Framework3/src/RendererGL_1_1.cpp ./Libraries/Framework3/src/RendererGL_1_1.cpp

IndieLibX_AllLibs_LNX32_ZIP:
	rm -fr ~/IndieLibX_AllLibs_LNX32
	mkdir ~/IndieLibX_AllLibs_LNX32
	rm -f ~/IndieLibX_AllLibs_LNX32.zip
	cp /usr/local/lib/lib*.a ~/IndieLibX_AllLibs_LNX32
	zip -r ~/IndieLibX_AllLibs_LNX32.zip ~/IndieLibX_AllLibs_LNX32

IndieLibX_AllLibs_LNX64_ZIP:
	rm -fr ~/IndieLibX_AllLibs_LNX64
	mkdir ~/IndieLibX_AllLibs_LNX64
	rm -f ~/IndieLibX_AllLibs_LNX64.zip
	cp  ~/IndieLibX/lib/lib*.a ~/IndieLibX_AllLibs_LNX64
	zip -r ~/IndieLibX_AllLibs_LNX64.zip ~/IndieLibX_AllLibs_LNX64

file_IndieLibX_ZIP := IndieLibX_$(shell date +%FT%T%Z).zip
IndieLibX_ZIP:
	rm -f ../IndieLibX.zip
	zip -r ../IndieLibX.zip ../IndieLibX
	cp ../IndieLibX.zip /media/tronty/Transcend/IndieLibX/$(file_IndieLibX_ZIP)
	cp ../IndieLibX.zip /media/tronty/Transcend1/IndieLibX/$(file_IndieLibX_ZIP)
	#cp ../IndieLibX.zip /media/tronty/x16/$(file_IndieLibX_ZIP)

file_IndieLib_resources_ZIP := IndieLib_resources_$(shell date +%FT%T%Z).zip
IndieLib_resources_ZIP:
	rm -f ../IndieLib_resources.zip
	zip -r ../IndieLib_resources.zip ../IndieLib_resources
	cp ../IndieLib_resources.zip /media/tronty/Transcend/IndieLibX/$(file_IndieLib_resources_ZIP)
	cp ../IndieLib_resources.zip /media/tronty/Transcend1/IndieLibX/$(file_IndieLib_resources_ZIP)
	#cp ../IndieLib_resources.zip /media/tronty/x16/$(file_IndieLib_resources_ZIP)

list_ZIPs:
	ls -al /media/tronty/Transcend/IndieLibX/IndieLibX_*.zip
	ls -al /media/tronty/Transcend/IndieLibX/IndieLib_resources_*.zip
	ls -al /media/tronty/Transcend1/IndieLibX/IndieLibX_*.zip
	ls -al /media/tronty/Transcend1/IndieLibX/IndieLib_resources_*.zip
	ls -al /media/tronty/X16/IndieLibX_*.zip
	ls -al /media/tronty/X16/IndieLib_resources_*.zip

IndieLibX_AllLibs_NDK_ZIP:
	rm -f ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK.zip
	rm -fr ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK
	cp -fr ./Libraries/MVC/NDK/obj/local ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK
	rm -fr ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK/armeabi/objs
	rm -fr ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK/mips/objs
	rm -fr ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK/x86/objs
	rm -f ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK/armeabi/libmain.so
	rm -f ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK/mips/libmain.so
	rm -f ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK/x86/libmain.so
	zip -r ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK.zip ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK
	cp ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK.zip ../IndieLibX_AllLibs_NDK.zip

VULKAN:
	./make.sh Test LNX GNUMAKE VULKAN

vulkan:
	./make.sh vulkan LNX GNUMAKE VULKAN

Particles:
	if [ -e "/usr/bin/wine" ]; then \
	cd ./Samples/ParticleVS; \
	unzip -x ./Chap12.zip -d ./Particle; \
	cd ./Particle/Chap12/Particles; \
	wine ./Particles.exe; \
	cd ../../..; \
	rm -fr ./Particle; \
	fi;

ParticleVS:
	if [ -e "/usr/bin/wine" ]; then \
	cd ./Samples/ParticleVS; \
	unzip -x ./Chap12.zip -d ./ParticleVS; \
	cd ./ParticleVS/Chap12/ParticleVS; \
	wine ./ParticlesVS.exe; \
	cd ../../..; \
	rm -fr ./ParticleVS; \
	fi;

RadeonTerrainDemo:
	if [ -e "/usr/bin/wine" ]; then \
	cd ./Samples/RadeonTerrainDemo; \
	unzip -x ./RadeonTerrainDemo.zip -d ./RadeonTerrainDemo; \
	cd ./RadeonTerrainDemo/RadeonTerrainDemo;  \
	wine ./RadeonTerrainDemo.exe; \
	cd ../..; \
	rm -fr ./RadeonTerrainDemo; \
	fi;

NatureScene:
	if [ -e "/usr/bin/wine" ]; then \
	cd ./Samples/NatureScene; \
	unzip -x ./NatureScene.zip -d ./NatureScene; \
	cd ./NatureScene/NatureScene/Application\ Nature; \
	wine ./SceneGraph.exe; \
	cd ../../..; \
	rm -fr ./NatureScene; \
	fi;

otl_SDL:
	cd ./Libraries; \
	unzip -x ./otl_SDL.zip -d ./otl_SDL; \
	cd ..; \
	./make.sh; \
	./make.sh otl_SDL; \
	./run.sh; \
	./make.sh; \
	rm -fr ./Libraries/otl_SDL

GLEssentials-Clean:
	rm -fr ./Samples/iOS/GLEssentials/build

GLEssentials-OSX:
	xcodebuild -project ./Samples/iOS/GLEssentials/GLEssentials.xcodeproj -target GLEssentials-OSX -configuration Debug
	./Samples/iOS/GLEssentials/build/Debug-iphonesimulator/GLEssentials.app/Contents/MacOS/GLEssentials

GLEssentials-iOS:
	xcodebuild -sdk iphonesimulator -project ./Samples/iOS/GLEssentials/GLEssentials.xcodeproj -target GLEssentials-iOS -configuration Debug
	ios-sim launch ./Samples/iOS/GLEssentials/build/Debug-iphonesimulator/GLEssentials.app --devicetypeid com.apple.CoreSimulator.SimDeviceType.iPad-2

Minecraft:
	#sudo chmod +x ./Dependencies/Minecraft/Linux/Minecraft.jar
	java -jar ./Dependencies/Minecraft/Linux/Minecraft.jar

Minecraftserver:
	#sudo chmod +x ./Dependencies/Minecraft/server.jar
	java -Xmx1024M -Xms1024M -jar ./Dependencies/Minecraft/server.jar nogui

