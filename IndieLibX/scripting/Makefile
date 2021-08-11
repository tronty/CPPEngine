#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
export shell = /bin/sh
export python=python

#find .. -type f -name "*.h" -exec grep -l "ifndef SWIG" {} \;

help:
	@echo "make clean	-> Removes temporary files."
	@echo "make python	-> Runs python scripts."
	@echo "make all  	-> Runs python scripts."
	@echo "make perl	-> Runs perl scripts."
	@echo "make java	-> Runs java scripts."
	@echo "make csharp	-> Runs csharp scripts."

clean:
	rm -fr *.pyc *.pm *.o *.*~ *~ IndieLib_wrap.cpp IndieLib_wrap.c IndieLib_wrap.cxx IndieLib.py com IndieLib.i.bu *.so; \
	if [ -e "./DesktopProjects/projectsX/Makefile" ]; then \
	make -f Makefile -C ./DesktopProjects/projectsX clean; \
	fi;

2D:
	sudo rm /usr/local/lib/libFW3.a; \
	cd ..; \
	./make.sh AllLibs; \
	sudo mv ./lib/libFW3.a /usr/local/lib; \
	rm ./bin/*.*; \
	cd ./scripting; \
	swig3.0 -DBUILD_MESHRENDERER -c++ -python IndieLib.i; \
	g++ -g3 -c ./IndieLib_wrap.cxx -std=gnu++0x -fPIC -DSWIG_ -D_DEBUG -DDEBUG -w -fmax-errors=1 -fpermissive -trigraphs -Wno-format -Wno-enum-compare -Wno-write-strings -DIL_STATIC_LIB -D__BUILD_ASSIMP -DBUILD_MESHRENDERER -DASSIMP_BUILD_BOOST_WORKAROUND  -DASSIMP_BUILD_SINGLETHREADED -DMESHRENDERER2_ORIGINAL  -I../Libraries/BoostWorkaround -I../Libraries/Image3 -I../Libraries/STX/include -I../Libraries/Framework3/include -I../Libraries/FW3/include -I../Libraries/IndieLib/IndieLib/Common/LibHeaders -I../Libraries/otl-nightly/otl/include -I../Libraries/assimp-3.2/include -I../Libraries/assimp-3.2/code -I../Libraries/MeshRenderer2 -I/usr/include/python2.7 -I../Libraries/BoostWorkaround -I../Libraries/tinyxml/include -I../Libraries/lua-5.3.4/src -I../Libraries/NSGUI; \
	g++ -shared -fPIC ./IndieLib_wrap.o -L../lib -L../../IndieLibX_Libraries/IndieLibX_AllLibs_LNX64 -lc3dbool -lopentree -lotlutils -lotlmesher -lweberarbaro -lweber -lnova -lFW3 -lRadeonTerrainDemoLibrary -lNSGUI -lspine_x -lMeshRenderer2 -lFramework3 -lImage3 -lSTX -ltinyxml_x -lLua_x -ltiff -ljpeg-8c -lpng-1.5.0 -lhlsl2glsl -lSOIL -lnoise -lassimp -lSDL2 -lopenal -lstdc++ -lz -lm -lGL -lGLU -o ./_IndieLib.so; \
	python 2DAnimation.py; \
	rm -fr *.pyc *.pm *.o *.*~ *~ IndieLib_wrap.cpp IndieLib_wrap.c IndieLib_wrap.cxx IndieLib.py com IndieLib.i.bu *.so; \
	if [ -e "./DesktopProjects/projectsX/Makefile" ]; then \
	make -f Makefile -C ./DesktopProjects/projectsX clean; \
	fi; \
	cd ..; \
	./make.sh; \
	cd ./scripting;

3D:
	sudo rm /usr/local/lib/libMeshRenderer2.a; \
	cd ..; \
	./make.sh AllLibs; \
	sudo mv ./lib/libMeshRenderer2.a /usr/local/lib; \
	rm ./bin/*.*; \
	cd ./scripting; \
	swig3.0 -DBUILD_MESHRENDERER -c++ -python IndieLib.i; \
	g++ -g3 -c ./IndieLib_wrap.cxx -std=gnu++0x -fPIC -DSWIG_ -D_DEBUG -DDEBUG -w -fmax-errors=1 -fpermissive -trigraphs -Wno-format -Wno-enum-compare -Wno-write-strings -DIL_STATIC_LIB -D__BUILD_ASSIMP -DBUILD_MESHRENDERER -DASSIMP_BUILD_BOOST_WORKAROUND  -DASSIMP_BUILD_SINGLETHREADED -DMESHRENDERER2_ORIGINAL  -I../Libraries/BoostWorkaround -I../Libraries/Image3 -I../Libraries/STX/include -I../Libraries/Framework3/include -I../Libraries/FW3/include -I../Libraries/IndieLib/IndieLib/Common/LibHeaders -I../Libraries/otl-nightly/otl/include -I../Libraries/assimp-3.2/include -I../Libraries/assimp-3.2/code -I../Libraries/MeshRenderer2 -I/usr/include/python2.7 -I../Libraries/BoostWorkaround -I../Libraries/tinyxml/include -I../Libraries/lua-5.3.4/src -I../Libraries/NSGUI; \
	g++ -shared -fPIC ./IndieLib_wrap.o -L../lib -L../../IndieLibX_Libraries/IndieLibX_AllLibs_LNX64 -lc3dbool -lopentree -lotlutils -lotlmesher -lweberarbaro -lweber -lnova -lFW3 -lRadeonTerrainDemoLibrary -lNSGUI -lspine_x -lMeshRenderer2 -lFramework3 -lImage3 -lSTX -ltinyxml_x -lLua_x -ltiff -ljpeg-8c -lpng-1.5.0 -lhlsl2glsl -lSOIL -lnoise -lassimp -lSDL2 -lopenal -lstdc++ -lz -lm -lGL -lGLU -o ./_IndieLib.so; \
	python 3DAnimation.py; \
	rm -fr *.pyc *.pm *.o *.*~ *~ IndieLib_wrap.cpp IndieLib_wrap.c IndieLib_wrap.cxx IndieLib.py com IndieLib.i.bu *.so; \
	if [ -e "./DesktopProjects/projectsX/Makefile" ]; then \
	make -f Makefile -C ./DesktopProjects/projectsX clean; \
	fi; \
	cd ..; \
	./make.sh; \
	cd ./scripting;

python: all
all:
	swig3.0 -DBUILD_MESHRENDERER -c++ -python IndieLib.i
	g++ -g3 -c ./IndieLib_wrap.cxx -std=gnu++0x -fPIC -DSWIG_ -D_DEBUG -DDEBUG -w -fmax-errors=1 -fpermissive -trigraphs -Wno-format -Wno-enum-compare -Wno-write-strings -DIL_STATIC_LIB -D__BUILD_ASSIMP -DBUILD_MESHRENDERER -DASSIMP_BUILD_BOOST_WORKAROUND  -DASSIMP_BUILD_SINGLETHREADED -DMESHRENDERER2_ORIGINAL  -I../Libraries/BoostWorkaround -I../Libraries/Image3 -I../Libraries/STX/include -I../Libraries/Framework3/include -I../Libraries/FW3/include -I../Libraries/IndieLib/IndieLib/Common/LibHeaders -I../Libraries/otl-nightly/otl/include -I../Libraries/assimp-3.2/include -I../Libraries/assimp-3.2/code -I../Libraries/MeshRenderer2 -I/usr/include/python2.7 -I../Libraries/BoostWorkaround -I../Libraries/tinyxml/include -I../Libraries/lua-5.3.4/src -I../Libraries/NSGUI
	g++ -shared -fPIC ./IndieLib_wrap.o -L../lib -L../../IndieLibX_Libraries/IndieLibX_AllLibs_LNX64 -lc3dbool -lopentree -lotlutils -lotlmesher -lweberarbaro -lweber -lnova -lFW3 -lRadeonTerrainDemoLibrary -lNSGUI -lspine_x -lMeshRenderer2 -lFramework3 -lImage3 -lSTX -ltinyxml_x -lLua_x -ltiff -ljpeg-8c -lpng-1.5.0 -lhlsl2glsl -lSOIL -lnoise -lassimp -lSDL2 -lopenal -lstdc++ -lz -lm -lGL -lGLU -o ./_IndieLib.so
	python a_02_IND_Surface.py                     
	python SimpleTexture.py     

test:                 
	python a_01_Installing.py               
	python a_02_IND_Surface.py              
	python a_03_IND_Image.py                
	python a_04_IND_Animation.py            
	python a_05_IND_Font.py            
	python a_07_IND_Input.py                
	python a_08_Collisions.py
	python a_11_Animated_Tile_Scrolling.py 
	python a_13_2d_Camera.py                          
	python a_15_Parallax_Scrolling.py 
	python a_16_IND_Timer.py   
	python b_04_Several_ViewPorts.py
	python IndieLib-Animation-Collision.py
	python IndieLib-Blend.py
	python IndieLib-Fading.py
	python IndieLib-FloatingText.py
	python IndieLib-ParallaxScroll.py                     
	python SimpleTexture.py     
	python 2DAnimation.py                   
	python 3D2Animation.py               
	python 3DAnimation.py 
	python a_09_IND_3dMesh.py                  
	python a_14_3d_Camera.py      
	python b_03_Blitting_3d_Directly.py         
	python a_10_IND_Light.py 
	python c_01_Alien_BenchMark.py

test2:            
	#python a_06_Primitives.py
	#python b_01_IND_Surface_Grids.py        
	#python b_02_Blitting_2d_Directly.py        
	#python IndieLib-TankEnemiesParallax.py
	#python IndieLib-TankEnemies.py
	#python IndieLib-TankFollowsMouseFires.py
	#python IndieLib-Transitions.py
	python c_02_Rabbits_BenchMark.py
	python texture.py
	python blending.py 
	python CantorDust3D.py
	python particles.py
	python testspine.py
	python testspine2.py

perl:
	mkdir -p com/IndieLib/swig3.0
	swig3.0 -DBUILD_MESHRENDERER -c++ -perl -outdir com/IndieLib/swig3.0 IndieLib.i
	gcc -g3 -c IndieLib_wrap.cxx -std=gnu++0x -fpic -DSWIG_ -D_DEBUG -DDEBUG -w -fmax-errors=1 -fpermissive -trigraphs -Wno-format -Wno-enum-compare -Wno-write-strings -DIL_STATIC_LIB -D__BUILD_ASSIMP -DBUILD_MESHRENDERER -DASSIMP_BUILD_BOOST_WORKAROUND  -DASSIMP_BUILD_SINGLETHREADED -DMESHRENDERER2_ORIGINAL  -I../Libraries/BoostWorkaround -I../Libraries/Image3 -I../Libraries/STX/include -I../Libraries/Framework3/include -I../Libraries/FW3/include -I../Libraries/IndieLib/IndieLib/Common/LibHeaders -I../Libraries/otl-nightly/otl/include -I../Libraries/assimp-3.2/include -I../Libraries/assimp-3.2/code -I../Libraries/MeshRenderer2 -I../Libraries/BoostWorkaround -I/usr/lib/perl/5.18/CORE -I../Libraries/tinyxml/include -I../Libraries/lua-5.3.4/src -I../Libraries/NSGUI
	gcc `perl -MConfig -e 'print $Config{lddlflags}'` IndieLib.o IndieLib_wrap.o -L../lib -L../../IndieLibX_Libraries/IndieLibX_AllLibs_LNX64 -lc3dbool -lopentree -lotlutils -lotlmesher -lweberarbaro -lweber -lnova -lFW3 -lRadeonTerrainDemoLibrary -lNSGUI -lspine_x -lMeshRenderer2 -lFramework3 -lImage3 -lSTX -ltinyxml_x -lLua_x -ltiff -ljpeg-8c -lpng-1.5.0 -lhlsl2glsl -lSOIL -lnoise -lassimp -lSDL2 -lopenal -lstdc++ -lz -lm -lGL -lGLU -o IndieLib.so
	perl a_01_Installing.pl
   
java:
	mkdir -p com/IndieLib/swig3.0
	swig3.0 -DBUILD_MESHRENDERER -c++ -java -package com.IndieLib.swig3.0 -outdir com/IndieLib/swig3.0 IndieLib.i
	gcc -g3 -c IndieLib_wrap.cxx -std=gnu++0x -fpic -DSWIG_ -D_DEBUG -DDEBUG -w -fmax-errors=1 -fpermissive -trigraphs -Wno-format -Wno-enum-compare -Wno-write-strings -DIL_STATIC_LIB -D__BUILD_ASSIMP -DBUILD_MESHRENDERER -DASSIMP_BUILD_BOOST_WORKAROUND  -DASSIMP_BUILD_SINGLETHREADED -DMESHRENDERER2_ORIGINAL  -I../Libraries/BoostWorkaround -I../Libraries/Image3 -I../Libraries/STX/include -I../Libraries/Framework3/include -I../Libraries/FW3/include -I../Libraries/IndieLib/IndieLib/Common/LibHeaders -I../Libraries/otl-nightly/otl/include -I../Libraries/assimp-3.2/include -I../Libraries/assimp-3.2/code -I../Libraries/MeshRenderer2 -I../Libraries/BoostWorkaround -I/usr/lib/jdk1.7.0_79/include -I/usr/lib/jvm/java-7-openjdk-i386/include -I../Libraries/tinyxml/include -I../Libraries/lua-5.3.4/src -I../Libraries/NSGUI
	#gcc -shared IndieLib_wrap.o -L../lib -L../../IndieLibX_Libraries/IndieLibX_AllLibs_LNX64 -lc3dbool -lopentree -lotlutils -lotlmesher -lweberarbaro -lweber -lnova -lFW3 -lRadeonTerrainDemoLibrary -lNSGUI -lspine_x -lMeshRenderer2 -lFramework3 -lImage3 -lSTX -ltinyxml_x -lLua_x -ltiff -ljpeg-8c -lpng-1.5.0 -lhlsl2glsl -lSOIL -lnoise -lassimp -lSDL2 -lopenal -lstdc++ -lz -lm -lGL -lGLU --add-stdcall-alias -o IndieLib.dll
	gcc -shared IndieLib_wrap.o -L../lib -L../../IndieLibX_Libraries/IndieLibX_AllLibs_LNX64 -lc3dbool -lopentree -lotlutils -lotlmesher -lweberarbaro -lweber -lnova -lFW3 -lRadeonTerrainDemoLibrary -lNSGUI -lspine_x -lMeshRenderer2 -lFramework3 -lImage3 -lSTX -ltinyxml_x -lLua_x -ltiff -ljpeg-8c -lpng-1.5.0 -lhlsl2glsl -lSOIL -lnoise -lassimp -lSDL2 -lopenal -lstdc++ -lz -lm -lGL -lGLU -o IndieLib.so
	javac a_01_Installing.java
	java -cp . test

csharp:
	mkdir -p com/IndieLib/swig3.0
	swig3.0 -DBUILD_MESHRENDERER -c++ -csharp -namespace com.IndieLib.swig3.0 -outdir com/IndieLib/swig3.0 IndieLib.i
	gcc -g3 -c IndieLib_wrap.cxx -std=gnu++0x -fpic -DSWIG_ -D_DEBUG -DDEBUG -w -fmax-errors=1 -fpermissive -trigraphs -Wno-format -Wno-enum-compare -Wno-write-strings -DIL_STATIC_LIB -D__BUILD_ASSIMP -DBUILD_MESHRENDERER -DASSIMP_BUILD_BOOST_WORKAROUND  -DASSIMP_BUILD_SINGLETHREADED -DMESHRENDERER2_ORIGINAL  -I../Libraries/BoostWorkaround -I../Libraries/Image3 -I../Libraries/STX/include -I../Libraries/Framework3/include -I../Libraries/FW3/include -I../Libraries/IndieLib/IndieLib/Common/LibHeaders -I../Libraries/otl-nightly/otl/include -I../Libraries/assimp-3.2/include -I../Libraries/assimp-3.2/code -I../Libraries/MeshRenderer2 -I../Libraries/BoostWorkaround -I../Libraries/tinyxml/include -I../Libraries/lua-5.3.4/src -I../Libraries/NSGUI
	gcc -shared IndieLib_wrap.o -L../lib -L../../IndieLibX_Libraries/IndieLibX_AllLibs_LNX64 -lc3dbool -lopentree -lotlutils -lotlmesher -lweberarbaro -lweber -lnova -lFW3 -lRadeonTerrainDemoLibrary -lNSGUI -lspine_x -lMeshRenderer2 -lFramework3 -lImage3 -lSTX -ltinyxml_x -lLua_x -ltiff -ljpeg-8c -lpng-1.5.0 -lhlsl2glsl -lSOIL -lnoise -lassimp -lSDL2 -lopenal -lstdc++ -lz -lm -lGL -lGLU -o IndieLib.so
	mcs a_01_Installing.cs
	mono a_01_Installing.exe

lua:
	swig3.0 -DBUILD_MESHRENDERER -c++ -lua IndieLib.i
	g++ -g3 -c ./min.cpp -I../Libraries/STX/include -I../Libraries/lua-5.3.4/src 
	g++ -g3 -c ./IndieLib_wrap.cxx -std=gnu++0x -fPIC -DSWIG_ -D_DEBUG -DDEBUG -w -fmax-errors=1 -fpermissive -trigraphs -Wno-format -Wno-enum-compare -Wno-write-strings -DIL_STATIC_LIB -D__BUILD_ASSIMP -DBUILD_MESHRENDERER -DASSIMP_BUILD_BOOST_WORKAROUND  -DASSIMP_BUILD_SINGLETHREADED -DMESHRENDERER2_ORIGINAL  -I../Libraries/BoostWorkaround -I../Libraries/Image3 -I../Libraries/STX/include -I../Libraries/Framework3/include -I../Libraries/FW3/include -I../Libraries/IndieLib/IndieLib/Common/LibHeaders -I../Libraries/otl-nightly/otl/include -I../Libraries/assimp-3.2/include -I../Libraries/assimp-3.2/code -I../Libraries/MeshRenderer2 -I/usr/include/python2.7 -I../Libraries/BoostWorkaround -I../Libraries/tinyxml/include -I../Libraries/lua-5.3.4/src -I../Libraries/NSGUI
	#g++ ./min.o ./IndieLib_wrap.o -L../lib -L../../IndieLibX_Libraries/IndieLibX_AllLibs_LNX64 -lc3dbool -lopentree -lotlutils -lotlmesher -lweberarbaro -lweber -lnova -lFW3 -lRadeonTerrainDemoLibrary -lNSGUI -lspine_x -lMeshRenderer2 -lFramework3 -lImage3 -lSTX -ltinyxml_x -lLua_x -ltiff -ljpeg-8c -lpng-1.5.0 -lhlsl2glsl -lSOIL -lnoise -lassimp -lSDL2 -lopenal -lstdc++ -lz -lm -lGL -lGLU -o ./luaMain
	g++ -L../../../../IndieLibX_Libraries/IndieLibX_AllLibs_LNX32 -g3 -O0 ./min.o ./IndieLib_wrap.o -L../../../lib -L../../../Dependencies/vulkan -L/usr/local/lib -lc3dbool -lopentree -lotlutils -lotlmesher -lweberarbaro -lweber -lnova -lFW3 -lRadeonTerrainDemoLibrary -lNSGUI -lspine_x -lMeshRenderer2 -lFramework3 -lImage3 -lSTX -ltinyxml_x -lLua_x -ltiff -ljpeg-8c -lpng-1.5.0 -lhlsl2glsl -lSOIL -lnoise -lassimp  -lGL -lGLU -lglut  -lSDL2 -lSDL2_image -lopenal -pthread -lX11  -lstdc++ -lz -lm -ldl -o ./luaMain

