#!/bin/bash
#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
source ./make.cnf

testOSX()
{
xcrun --sdk iphoneos10.3 g++ -arch armv7 -miphoneos-version-min=7.0 -std=gnu++0x -g3 -O0 -D_DEBUG -DDEBUG -DGLES2 -DUSE_GLES_SHADERS -DOS_IPHONE -stdlib=libc++ -fvisibility-inlines-hidden -isysroot "/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS10.3.sdk" -I. -I/usr/X11R6/include -I./Libraries/SDL2-2.0.10/include -D__IPHONE_OS_VERSION_MIN_REQUIRED=40100 -Wno-comment -Wno-import   -ferror-limit=100 -Wno-write-strings -Wno-unused-value -Wno-c++11-narrowing -Wno-address-of-packed-member -DGLES2 -I./Libraries/BoostWorkaround -I./Libraries/STX/include -I./Libraries/Image3 -I./Libraries/openal-soft-1.15.1/include -I./Libraries/lua-5.3.4/src -o ./Libraries/STX/src/stxCPP2.o -c ./Libraries/STX/src/stxCPP2.cpp

ls -al ./Libraries/STX/src/stxCPP2.o
rm ./Libraries/STX/src/stxCPP2.o

/usr/bin/g++ -g3 -O0  -fexceptions -fnon-call-exceptions  -std=gnu++0x -DLINUX -D_LINUX -Wno-enum-compare -rdynamic  --trigraphs -Wno-comment -Wno-format -Wno-write-strings   -fpermissive -fmax-errors=100 -fvisibility=hidden -fvisibility-inlines-hidden -D_DEBUG -DDEBUG -DGLSL1_1 -DUSE_HLSL_SHADERS -I./Libraries/BoostWorkaround -I./Libraries/Vulkan/external -I./Libraries/SDL2-2.0.10/include -I./Libraries/Molten-0.18.0/MoltenVK/include -I./Libraries/STX/include -I./Libraries/Image3 -I./Libraries/openal-soft-1.15.1/include -I./Libraries/lua-5.3.4/src -w -o ./Libraries/STX/src/stxCPP2.o -c ./Libraries/STX/src/stxCPP2.cpp

ls -al ./Libraries/STX/src/stxCPP2.o
rm ./Libraries/STX/src/stxCPP2.o
}

testVulkan()
{

g++ -g3 -O0  -fexceptions -fnon-call-exceptions  -std=gnu++0x -Wno-enum-compare -rdynamic --trigraphs -Wno-comment -Wno-format -Wno-write-strings   -fpermissive -fmax-errors=100 -D_DEBUG -DDEBUG -DUSE_HLSL_SHADERS -D_LIB -DVULKAN -D_LINUX -DLINUX -D__linux__ -I./Libraries/BoostWorkaround -I./ -I./Libraries/zip -I./Libraries/STX/include -I./Libraries/Image3 -I./Libraries/hlsl2glslfork/include -I./Libraries/Framework3/include -I./Libraries/Vulkan/base -I./Libraries/Vulkan/external/glm -I./Libraries/Vulkan/external/gli -I./Libraries/Vulkan/external/imgui -I./Libraries/Vulkan/external -I./ -o ./Libraries/Framework3/src/tinyvk.o -c ./Libraries/Framework3/src/tinyvk.cpp

g++ -g3 -O0  -fexceptions -fnon-call-exceptions  -std=gnu++0x -Wno-enum-compare -rdynamic --trigraphs -Wno-comment -Wno-format -Wno-write-strings   -fpermissive -fmax-errors=100 -D_DEBUG -DDEBUG -DUSE_HLSL_SHADERS -D_LIB -DVULKAN -D_LINUX -DLINUX -D__linux__ -I./Libraries/BoostWorkaround -I./ -I./Libraries/zip -I./Libraries/STX/include -I./Libraries/Image3 -I./Libraries/hlsl2glslfork/include -I./Libraries/Framework3/include -I./Libraries/Vulkan/base -I./Libraries/Vulkan/external/glm -I./Libraries/Vulkan/external/gli -I./Libraries/Vulkan/external/imgui -I./Libraries/Vulkan/external -I./ -o ./Libraries/Framework3/src/RendererVulkan.o -c ./Libraries/Framework3/src/RendererVulkan.cpp

ls -al ./Libraries/Framework3/src/RendererVulkan.o
rm ./Libraries/Framework3/src/RendererVulkan.o
}

testGL()
{

g++ -g3 -O0  -fexceptions -fnon-call-exceptions  -std=gnu++0x -Wno-enum-compare -rdynamic --trigraphs -Wno-comment -Wno-format -Wno-write-strings   -fpermissive -fmax-errors=1 -D_DEBUG -DDEBUG -DUSE_HLSL_SHADERS -D_LIB -DGLSL1_1 -D_LINUX -DLINUX -D__linux__ -I./Libraries/BoostWorkaround -I./ -I./Libraries/zip -I./Libraries/STX/include -I./Libraries/Image3 -I./Libraries/hlsl2glslfork/include -I./Libraries/Framework3/include -I./ -o ./Libraries/Framework3/src/RendererGL_1_1.o -c ./Libraries/Framework3/src/RendererGL_1_1.cpp

g++ -g3 -O0  -fexceptions -fnon-call-exceptions  -std=gnu++0x -Wno-enum-compare -rdynamic --trigraphs -Wno-comment -Wno-format -Wno-write-strings   -fpermissive -fmax-errors=1 -D_DEBUG -DDEBUG -DUSE_HLSL_SHADERS -D_LIB -DGLSL1_1 -D_LINUX -DLINUX -D__linux__ -I./Libraries/BoostWorkaround -I./ -I./Libraries/zip -I./Libraries/STX/include -I./Libraries/Image3 -I./Libraries/hlsl2glslfork/include -I./Libraries/Framework3/include -I./ -o ./Libraries/Framework3/src/RendererGLSLGL_1_1.o -c ./Libraries/Framework3/src/RendererGLSLGL_1_1.cpp

ls -al ./Libraries/Framework3/src/RendererGL_1_1.o
ls -al ./Libraries/Framework3/src/RendererGLSLGL_1_1.o
rm ./Libraries/Framework3/src/RendererGL_1_1.o
rm ./Libraries/Framework3/src/RendererGLSLGL_1_1.o

g++ -g3 -O0  -fexceptions -fnon-call-exceptions  -std=gnu++0x -Wno-enum-compare -rdynamic --trigraphs -Wno-comment -Wno-format -Wno-write-strings   -fpermissive -fmax-errors=1 -D_DEBUG -DDEBUG -DUSE_HLSL_SHADERS -D_LIB -DGLSL4_5 -D_LINUX -DLINUX -D__linux__ -I./Libraries/BoostWorkaround -I./ -I./Libraries/zip -I./Libraries/STX/include -I./Libraries/Image3 -I./Libraries/hlsl2glslfork/include -I./Libraries/Framework3/include -I./ -o ./Libraries/Framework3/src/RendererGL_1_1.o -c ./Libraries/Framework3/src/RendererGL_1_1.cpp

g++ -g3 -O0  -fexceptions -fnon-call-exceptions  -std=gnu++0x -Wno-enum-compare -rdynamic --trigraphs -Wno-comment -Wno-format -Wno-write-strings   -fpermissive -fmax-errors=1 -D_DEBUG -DDEBUG -DUSE_HLSL_SHADERS -D_LIB -DGLSL4_5 -D_LINUX -DLINUX -D__linux__ -I./Libraries/BoostWorkaround -I./ -I./Libraries/zip -I./Libraries/STX/include -I./Libraries/Image3 -I./Libraries/hlsl2glslfork/include -I./Libraries/Framework3/include -I./ -o ./Libraries/Framework3/src/RendererGLSLGL_4_5.o -c ./Libraries/Framework3/src/RendererGLSLGL_4_5.cpp

ls -al ./Libraries/Framework3/src/RendererGL_1_1.o
ls -al ./Libraries/Framework3/src/RendererGLSLGL_4_5.o
rm ./Libraries/Framework3/src/RendererGL_1_1.o
rm ./Libraries/Framework3/src/RendererGLSLGL_4_5.o

}
testGLES2()
{

../$NDK/toolchains/x86-4.8/prebuilt/linux-x86/bin/i686-linux-android-g++ -I../$NDK/sources/cxx-stl/gnu-libstdc++/4.8/libs/x86/include -I../$NDK/sources/cxx-stl/gnu-libstdc++/4.8/include -I../$NDK/platforms/android-19/arch-x86/usr/include -g3 -O0  -fexceptions -fnon-call-exceptions  -std=gnu++0x -Wno-enum-compare -rdynamic -DANDROID --trigraphs -Wno-comment -Wno-format -Wno-write-strings   -fpermissive -fmax-errors=1 -D_DEBUG -DDEBUG -DUSE_HLSL_SHADERS -D_LIB -DGLES2 -I./Libraries/BoostWorkaround -I./ -I./Libraries/zip -I./Libraries/STX/include -I./Libraries/Image3 -I./Libraries/hlsl2glslfork/include -I./Libraries/Framework3/include -I./ -o ./Libraries/Framework3/src/RendererGLES.o -c ./Libraries/Framework3/src/RendererGLES.cpp

../$NDK/toolchains/x86-4.8/prebuilt/linux-x86/bin/i686-linux-android-g++ -I../$NDK/sources/cxx-stl/gnu-libstdc++/4.8/libs/x86/include -I../$NDK/sources/cxx-stl/gnu-libstdc++/4.8/include -I../$NDK/platforms/android-19/arch-x86/usr/include -g3 -O0  -fexceptions -fnon-call-exceptions  -std=gnu++0x -Wno-enum-compare -rdynamic -DANDROID --trigraphs -Wno-comment -Wno-format -Wno-write-strings   -fpermissive -fmax-errors=1 -D_DEBUG -DDEBUG -DUSE_HLSL_SHADERS -D_LIB -DGLES2 -I./Libraries/BoostWorkaround -I./ -I./Libraries/zip -I./Libraries/STX/include -I./Libraries/Image3 -I./Libraries/hlsl2glslfork/include -I./Libraries/Framework3/include -I./ -o ./Libraries/Framework3/src/RendererGLES2.o -c ./Libraries/Framework3/src/RendererGLES2.cpp

ls -al ./Libraries/Framework3/src/RendererGLES.o
ls -al ./Libraries/Framework3/src/RendererGLES2.o
rm ./Libraries/Framework3/src/RendererGLES.o
rm ./Libraries/Framework3/src/RendererGLES2.o

}
testGLES3()
{
../$NDK/toolchains/x86-4.8/prebuilt/linux-x86/bin/i686-linux-android-g++ -I../$NDK/sources/cxx-stl/gnu-libstdc++/4.8/libs/x86/include -I../$NDK/sources/cxx-stl/gnu-libstdc++/4.8/include -I../$NDK/platforms/android-19/arch-x86/usr/include -g3 -O0  -fexceptions -fnon-call-exceptions  -std=gnu++0x -Wno-enum-compare -rdynamic -DANDROID --trigraphs -Wno-comment -Wno-format -Wno-write-strings   -fpermissive -fmax-errors=1 -D_DEBUG -DDEBUG -DUSE_HLSL_SHADERS -D_LIB -DGLES3_2 -I./Libraries/BoostWorkaround -I./ -I./Libraries/zip -I./Libraries/STX/include -I./Libraries/Image3 -I./Libraries/hlsl2glslfork/include -I./Libraries/Framework3/include -I./ -o ./Libraries/Framework3/src/RendererGLES.o -c ./Libraries/Framework3/src/RendererGLES.cpp

../$NDK/toolchains/x86-4.8/prebuilt/linux-x86/bin/i686-linux-android-g++ -I../$NDK/sources/cxx-stl/gnu-libstdc++/4.8/libs/x86/include -I../$NDK/sources/cxx-stl/gnu-libstdc++/4.8/include -I../$NDK/platforms/android-19/arch-x86/usr/include -g3 -O0  -fexceptions -fnon-call-exceptions  -std=gnu++0x -Wno-enum-compare -rdynamic -DANDROID --trigraphs -Wno-comment -Wno-format -Wno-write-strings   -fpermissive -fmax-errors=1 -D_DEBUG -DDEBUG -DUSE_HLSL_SHADERS -D_LIB -DGLES3_2 -I./Libraries/BoostWorkaround -I./ -I./Libraries/zip -I./Libraries/STX/include -I./Libraries/Image3 -I./Libraries/hlsl2glslfork/include -I./Libraries/Framework3/include -I./ -o ./Libraries/Framework3/src/RendererGLES_3_2.o -c ./Libraries/Framework3/src/RendererGLES_3_2.cpp

ls -al ./Libraries/Framework3/src/RendererGLES.o
ls -al ./Libraries/Framework3/src/RendererGLES_3_2.o
rm ./Libraries/Framework3/src/RendererGLES.o
rm ./Libraries/Framework3/src/RendererGLES_3_2.o

}

testSamples()
{

case "$OSTYPE" in
    darwin*)
        platform='OSX'
	buildtool='GNUMAKE'
	renderer='GLSL1_1'
        ;; 
    linux*)
        platform='LNX'
	buildtool='GNUMAKE'
	renderer='GLSL1_1'
        ;;
  *);;
esac
#./make.sh
#./make.sh AllLibs $platform
#sudo mv lib/*.* /usr/local/lib
./make.sh
./make.sh TestX $platform $buildtool $renderer
./run.sh
./make.sh
./make.sh Test $platform $buildtool $renderer
./run.sh
./make.sh
./make.sh SampleApps $platform $buildtool $renderer
./run.sh
./make.sh
#./make.sh LSystem $platform $buildtool $renderer
#./run.sh
#./make.sh
#./make.sh L_System $platform $buildtool $renderer
#./run.sh
#./make.sh
./make.sh TerrainApps $platform $buildtool $renderer
./run.sh
./make.sh
./make.sh IndieLibApps $platform $buildtool $renderer
./run.sh
./make.sh
exit 0
if [[ $OSTYPE == linux* ]]; then
cd ./scripting
make all
make clean
cd ..
fi
case "$OSTYPE" in
    darwin*)
        ./make.sh Test iOS GNUMAKE GLES2
        ;; 
    linux*)
        ./make.sh Test NDK NDK GLES2
        ;;
  *);;
esac
./make.sh

}

case "$1" in
    vk*)
	testVulkan;
	exit 0;
	;;
    gles3*)
	testGLES3;
	exit 0;
        ;;
    gles2*)
	testGLES2;
	exit 0;
        ;;
    gl*)
	testGL;
	exit 0;
        ;;
    OSX*)
	testOSX;
	exit 0;
        ;;
  *);;
esac
testSamples;

