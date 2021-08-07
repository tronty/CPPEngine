#
# Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
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

clean:
	rm -fr *.pyc *.o *.*~ *~ IndieLib_wrap.cpp IndieLib_wrap.c IndieLib_wrap.cxx IndieLib.py com IndieLib.i.bu *.so; \

python: all
all:
	swig -DBUILD_MESHRENDERER -c++ -python IndieLib.i
	g++ -g3 -c ./IndieLib_wrap.cxx -std=gnu++0x -fPIC -DSWIG_ -D_DEBUG -DDEBUG -w -fmax-errors=1 -fpermissive -trigraphs -Wno-format -Wno-enum-compare -Wno-write-strings -DIL_STATIC_LIB -D__BUILD_ASSIMP -DBUILD_MESHRENDERER -DASSIMP_BUILD_BOOST_WORKAROUND -DASSIMP_BUILD_SINGLETHREADED -I../../../../Libraries/BoostWorkaround -I../../../../Libraries/Image3 -I../../../../Libraries/STX/include -I../../../../Libraries/Framework3/include -I../../../../Libraries/FW3/include -I../../../../Libraries/IndieLib/IndieLib/Common/LibHeaders -I../../../../Libraries/otl-nightly/otl/include -I../../../../Libraries/assimp-3.2/include -I../../../../Libraries/assimp-3.2/code -I../../../../Libraries/MeshRenderer2 -I/usr/include/python2.7 -I../../../../Libraries/BoostWorkaround -I../../../../Libraries/tinyxml/include -I../../../../Libraries/lua-5.3.4/src -I../../../../Libraries/NSGUI
	g++ -g3 -c ../../../../Libraries/STX/src/d3dx9mathFromWine.cpp -std=gnu++0x -fPIC -DSWIG_ -D_DEBUG -DDEBUG -w -fmax-errors=1 -fpermissive -trigraphs -Wno-format -Wno-enum-compare -Wno-write-strings -DIL_STATIC_LIB -D__BUILD_ASSIMP -DBUILD_MESHRENDERER -DASSIMP_BUILD_BOOST_WORKAROUND -DASSIMP_BUILD_SINGLETHREADED -I../../../../Libraries/BoostWorkaround -I../../../../Libraries/Image3 -I../../../../Libraries/STX/include -I../../../../Libraries/Framework3/include -I../../../../Libraries/FW3/include -I../../../../Libraries/IndieLib/IndieLib/Common/LibHeaders -I../../../../Libraries/otl-nightly/otl/include -I../../../../Libraries/assimp-3.2/include -I../../../../Libraries/assimp-3.2/code -I../../../../Libraries/MeshRenderer2 -I/usr/include/python2.7 -I../../../../Libraries/BoostWorkaround -I../../../../Libraries/tinyxml/include -I../../../../Libraries/lua-5.3.4/src -I../../../../Libraries/NSGUI
	g++ -shared -fPIC ./IndieLib_wrap.o ./d3dx9mathFromWine.o -o ./_IndieLib.so -L../../../../lib -L/usr/local/lib -lSTX -lImage3 -lSDL2 -lSDL2_image -pthread -lX11 -lstdc++ -lz -lm -ldl 
	python Test.py

