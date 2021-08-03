#!/bin/bash
#
# Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
source ./make.cnf

if [ "X"=="X" ]; then

g++ -g3 -O0  -fexceptions -fnon-call-exceptions  -std=gnu++0x -Wno-enum-compare -rdynamic --trigraphs -Wno-comment -Wno-format -Wno-write-strings   -fpermissive -fmax-errors=100 -D_DEBUG -DDEBUG -DUSE_HLSL_SHADERS -D_LIB -DGLSL1_1 -D_LINUX -DLINUX -D__linux__ -I./Libraries/BoostWorkaround -I./ -I./Libraries/zip -I./Libraries/STX/include -I./Libraries/Image3 -I./Libraries/hlsl2glslfork/include -I./Libraries/Framework3/include -I./ -o ./Libraries/Framework3/src/RendererGLSLGL_1_1.o -c ./Libraries/Framework3/src/RendererGLSLGL_1_1.cpp

ls -al ./Libraries/Framework3/src/RendererGLSLGL_1_1.*

elif [ "X"=="Y" ]; then

../$NDK/toolchains/x86-4.8/prebuilt/linux-x86/bin/i686-linux-android-g++ -I../$NDK/sources/cxx-stl/gnu-libstdc++/4.8/libs/x86/include -I../$NDK/sources/cxx-stl/gnu-libstdc++/4.8/include -I../$NDK/platforms/android-19/arch-x86/usr/include -g3 -O0  -fexceptions -fnon-call-exceptions  -std=gnu++0x -Wno-enum-compare -rdynamic -DANDROID --trigraphs -Wno-comment -Wno-format -Wno-write-strings   -fpermissive -fmax-errors=100 -D_DEBUG -DDEBUG -DUSE_HLSL_SHADERS -D_LIB -DGLES3_2 -I./Libraries/BoostWorkaround -I./ -I./Libraries/zip -I./Libraries/STX/include -I./Libraries/Image3 -I./Libraries/hlsl2glslfork/include -I./Libraries/Framework3/include -I./ -o ./Libraries/Framework3/src/RendererGLES.o -c ./Libraries/Framework3/src/RendererGLES.cpp

../$NDK/toolchains/x86-4.8/prebuilt/linux-x86/bin/i686-linux-android-g++ -I../$NDK/sources/cxx-stl/gnu-libstdc++/4.8/libs/x86/include -I../$NDK/sources/cxx-stl/gnu-libstdc++/4.8/include -I../$NDK/platforms/android-19/arch-x86/usr/include -g3 -O0  -fexceptions -fnon-call-exceptions  -std=gnu++0x -Wno-enum-compare -rdynamic -DANDROID --trigraphs -Wno-comment -Wno-format -Wno-write-strings   -fpermissive -fmax-errors=100 -D_DEBUG -DDEBUG -DUSE_HLSL_SHADERS -D_LIB -DGLES3_2 -I./Libraries/BoostWorkaround -I./ -I./Libraries/zip -I./Libraries/STX/include -I./Libraries/Image3 -I./Libraries/hlsl2glslfork/include -I./Libraries/Framework3/include -I./ -o ./Libraries/Framework3/src/RendererGLES_3_2.o -c ./Libraries/Framework3/src/RendererGLES_3_2.cpp

../$NDK/toolchains/x86-4.8/prebuilt/linux-x86/bin/i686-linux-android-g++ -I../$NDK/sources/cxx-stl/gnu-libstdc++/4.8/libs/x86/include -I../$NDK/sources/cxx-stl/gnu-libstdc++/4.8/include -I../$NDK/platforms/android-19/arch-x86/usr/include -g3 -O0  -fexceptions -fnon-call-exceptions  -std=gnu++0x -Wno-enum-compare -rdynamic -DANDROID --trigraphs -Wno-comment -Wno-format -Wno-write-strings   -fpermissive -fmax-errors=100 -D_DEBUG -DDEBUG -DUSE_HLSL_SHADERS -D_LIB -DGLES3_2 -I./Libraries/BoostWorkaround -I./ -I./Libraries/zip -I./Libraries/STX/include -I./Libraries/Image3 -I./Libraries/hlsl2glslfork/include -I./Libraries/Framework3/include -I./ -o ./Libraries/Framework3/src/RendererHLSLCg.o -c ./Libraries/Framework3/src/RendererHLSLCg.cpp

../$NDK/toolchains/x86-4.8/prebuilt/linux-x86/bin/i686-linux-android-g++ -I../$NDK/sources/cxx-stl/gnu-libstdc++/4.8/libs/x86/include -I../$NDK/sources/cxx-stl/gnu-libstdc++/4.8/include -I../$NDK/platforms/android-19/arch-x86/usr/include -g3 -O0  -fexceptions -fnon-call-exceptions  -std=gnu++0x -Wno-enum-compare -rdynamic -DANDROID --trigraphs -Wno-comment -Wno-format -Wno-write-strings   -fpermissive -fmax-errors=100 -D_DEBUG -DDEBUG -DUSE_HLSL_SHADERS -D_LIB -DGLES3_2 -I./Libraries/BoostWorkaround -I./ -I./Libraries/zip -I./Libraries/STX/include -I./Libraries/Image3 -I./Libraries/hlsl2glslfork/include -I./Libraries/Framework3/include -I./ -o ./Samples/Test/main2.o -c ./Samples/Test/main2.cpp

g++ -shared -g3 -O0 ./Libraries/Framework3/src/RendererHLSLCg.o ./Libraries/Framework3/src/RendererGLES_3_2.o ./Libraries/Framework3/src/RendererGLES.o ./Samples/Test/main2.o -L/usr/local/lib -lImage3 -lSTX -lLua_x -ljpeg-8c -lpng-1.5.0 -lhlsl2glsl -lVulkanBase -lGL -lGLU  -lSDL2 -lSDL2_image -lopenal -lCg -lCgGL -pthread -lX11 -lstdc++ -lz -lm -o ./bin/main.so

ls -al ./Libraries/Framework3/src/RendererGLES.*
ls -al ./Libraries/Framework3/src/RendererGLES_3_2.*
ls -al ./Samples/Test/main2.*
ls -al ./bin/main.so

elif [ "X"=="Y" ]; then

g++ -g3 -O0  -fexceptions -fnon-call-exceptions  -std=gnu++0x -Wno-enum-compare -rdynamic --trigraphs -Wno-comment -Wno-format -Wno-write-strings   -fpermissive -fmax-errors=100 -D_DEBUG -DDEBUG -DUSE_HLSL_SHADERS -D_LIB -DGLSL4_5 -D_LINUX -DLINUX -D__linux__ -I./Libraries/BoostWorkaround -I./ -I./Libraries/zip -I./Libraries/STX/include -I./Libraries/Image3 -I./Libraries/hlsl2glslfork/include -I./Libraries/Framework3/include -I./ -o ./Libraries/Framework3/src/RendererGL_1_1.o -c ./Libraries/Framework3/src/RendererGL_1_1.cpp

g++ -g3 -O0  -fexceptions -fnon-call-exceptions  -std=gnu++0x -Wno-enum-compare -rdynamic --trigraphs -Wno-comment -Wno-format -Wno-write-strings   -fpermissive -fmax-errors=100 -D_DEBUG -DDEBUG -DUSE_HLSL_SHADERS -D_LIB -DGLSL4_5 -D_LINUX -DLINUX -D__linux__ -I./Libraries/BoostWorkaround -I./ -I./Libraries/zip -I./Libraries/STX/include -I./Libraries/Image3 -I./Libraries/hlsl2glslfork/include -I./Libraries/Framework3/include -I./ -o ./Libraries/Framework3/src/RendererGLSLGL_4_5.o -c ./Libraries/Framework3/src/RendererGLSLGL_4_5.cpp 

g++ -g3 -O0  -fexceptions -fnon-call-exceptions  -std=gnu++0x -Wno-enum-compare -rdynamic --trigraphs -Wno-comment -Wno-format -Wno-write-strings   -fpermissive -fmax-errors=100 -D_DEBUG -DDEBUG -DUSE_HLSL_SHADERS -D_LIB -DGLSL4_5 -D_LINUX -DLINUX -D__linux__ -I./Libraries/BoostWorkaround -I./ -I./Libraries/zip -I./Libraries/STX/include -I./Libraries/Image3 -I./Libraries/hlsl2glslfork/include -I./Libraries/Framework3/include -I./ -o ./Libraries/Framework3/src/RendererHLSLCg.o -c ./Libraries/Framework3/src/RendererHLSLCg.cpp 

g++ -g3 -O0  -fexceptions -fnon-call-exceptions  -std=gnu++0x -Wno-enum-compare -rdynamic --trigraphs -Wno-comment -Wno-format -Wno-write-strings   -fpermissive -fmax-errors=100 -D_DEBUG -DDEBUG -DUSE_HLSL_SHADERS -D_LIB -DGLSL4_5 -D_LINUX -DLINUX -D__linux__ -I./Libraries/BoostWorkaround -I./ -I./Libraries/zip -I./Libraries/STX/include -I./Libraries/Image3 -I./Libraries/hlsl2glslfork/include -I./Libraries/Framework3/include -I./ -o ./Samples/Test/main2.o -c ./Samples/Test/main2.cpp

g++ -g3 -O0 ./Libraries/Framework3/src/RendererHLSLCg.o ./Libraries/Framework3/src/RendererGLSLGL_4_5.o ./Libraries/Framework3/src/RendererGL_1_1.o ./Samples/Test/main2.o -L/usr/local/lib -lImage3 -lSTX -lLua_x -ljpeg-8c -lpng-1.5.0 -lhlsl2glsl -lVulkanBase -lGL -lGLU  -lSDL2 -lSDL2_image -lopenal -lCg -lCgGL -pthread -lX11 -lstdc++ -lz -lm -o ./bin/main.bin

ls -al ./Libraries/Framework3/src/RendererGL_1_1.*
ls -al ./Libraries/Framework3/src/RendererGLSLGL_4_5.*
ls -al ./Samples/Test/main2.*
ls -al ./bin/main.bin

fi

