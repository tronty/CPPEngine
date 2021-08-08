#!/bin/bash
#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#

#MESA_GL_VERSION_OVERRIDE=3.3
MESA_GL_VERSION_OVERRIDE=4.3

source ./make.cnf

PYTHON=`which python`
if [ -z "$PYTHON" ]; then
	echo "Could not find python."
	exit 1
fi

install()
{
	cd ./DesktopProjects
	$PYTHON ./install.py
	cd ..
}

OS="Windows"
platformsArray=( NDK NMAKE VC MW PRO CMake )
makesystem="NMAKE"
platform="WIN"
if [ -f "/etc/lsb-release" ]; then
	DINFO=$(cat /etc/lsb-release | grep DISTRIB_ID | sed s/.*=//)
	if [ $DINFO == "Ubuntu" ]; then
		OS="Ubuntu"
		echo "1:OS: $OS"
		platformsArray=( NDK LNX MW PRO CMake )
		makesystem="GNUMAKE"
		platform="LNX"
		rm -fr ../tmp
		mkdir ../tmp
	fi
	if [ $DINFO == "LinuxMint" ]; then
		OS="LinuxMint"
		echo "1:OS: $OS"
		platformsArray=( NDK LNX MW PRO CMake )
		makesystem="GNUMAKE"
		platform="LNX"
		rm -fr ../tmp
		mkdir ../tmp
	fi
	install;
elif [ -f "/etc/debian_version" ]; then
	OS="Debian"
	echo "2:OS: $OS"
	platformsArray=( NDK LNX MW PRO CMake )
	makesystem="GNUMAKE"
	platform="LNX"
	rm -fr ../tmp
	mkdir ../tmp
	install;
elif [ -d "/Applications/Xcode.app" ]; then
	OS="Darwin"
	echo "3:OS: $OS"
	platformsArray=( OSX iOS )
	makesystem="GNUMAKE"
	platform="OSX"
elif [ -d "/Applications/Xcode-Beta.app" ]; then
	OS="Darwin"
	echo "4:OS: $OS"
	platformsArray=( NDK LNX iOS OSX MW PRO CMake )
	makesystem="GNUMAKE"
	platform="OSX"
fi

Bit32p64=""
if [ $(uname -m) == "i686" ]; then
	Bit32p64="i686"
fi

PTHREAD=""
PTHREADOSX="-pthread"
if [ -f "/usr/include/pthread.h" ];
then
	PTHREAD="-pthread"
fi

export PTHREAD
export PTHREADOSX
export OS
export Bit32p64
export platform

echo "OS: $OS"
#exit 0

deltmp()
{
rm -fr ./DesktopProjects/projectsX
mkdir ./DesktopProjects/projectsX
rm -fr ./Libraries/MVC/NDK/build
mkdir -p ./Libraries/MVC/NDK/build
rm -fr ./Libraries/SDL2-${sdl2version}/build
mkdir  ./Libraries/SDL2-${sdl2version}/build
rm -f ./scripting/*.pyc ./scripting/*.o ./scripting/IndieLib.so ./scripting/*.*~ ./scripting/*~ ./scripting/IndieLib_wrap.cpp ./scripting/IndieLib_wrap.c ./scripting/IndieLib_wrap.cxx ./NDK.log ./NDKbuild.log ./scripting/IndieLib.py ./build.log ./iOSbuild.log
find . -name "*.obj" -type f -delete
find . -name "*.o" -type f -delete
find . -name "*.*~" -type f -delete
find . -name "*~" -type f -delete
find . -name "*.log" -type f -delete
find . -name "*.js" -type f -delete
find . -name "*.css" -type f -delete
find ./DesktopProjects -name "*.pyc" -type f -delete
find ./scripting -name "*.pyc" -type f -delete
find ./mk -name "*.pyc" -type f -delete
cd ./Libraries/MVC/NDK
rm -fr build jni obj assets libs bin gen
cd ../../..
rm -f ./bin/*.dll
rm -f ./bin/*.log
rm -f ./bin/*.txt
rm -f ./bin/__tmp__.xml
rm -f ./bin/__tmp__.fx
		rm -fr ./lib 
		mkdir ./lib
}

clean()
{
	if [ -e "./DesktopProjects/projectsX/Makefile" ]; then
		make -C ./DesktopProjects/projectsX -f Makefile clean;
	fi;
	if [ -e "./DesktopProjects/projectsX/Makefile.mw" ]; then
		make -C ./DesktopProjects/projectsX -f Makefile.mw clean;
	fi;
	if [ -e "./DesktopProjects/projectsX/Makefile.iOS" ]; then
		make -C ./DesktopProjects/projectsX -f Makefile.iOS clean;
	fi;
	if [ "$platform" == "OSX" ]; then
		if [ ! -f "./DesktopProjects/projectsX/Makefile.iOS" ]; then
			#rm -fr ../IndieLibX_Libraries/IndieLibX_AllLibs_OSX
			#rm -fr ../IndieLibX_Libraries/IndieLibX_AllLibs_OSX\ \(copy\)
			#cp -fr ./lib ../IndieLibX_Libraries/IndieLibX_AllLibs_OSX
			#cp -fr ./lib ../IndieLibX_Libraries/IndieLibX_AllLibs_OSX\ \(copy\)
			echo ""
		fi
	fi
	if [ -f "./DesktopProjects/projectsX/Makefile.iOS" ]; then
		#rm -fr ../IndieLibX_Libraries/IndieLibX_AllLibs_iOS
		echo ""
	fi
	if [ -f "./Libraries/MVC/NDK/obj/local" ]; then
		rm -fr ./Libraries/MVC/NDK/obj/local
	fi
deltmp
}

updategit()
{
if [ "X"=="Y" ]; then
if [ -e "../CPPEngine/.git/index" ]; then
    cd ..
    rm -fr ./CPPEngine/IndieLibX
    rm -fr ./CPPEngine/IndieLib_resources
    cp -r ./IndieLibX ./CPPEngine/.
    cp -r ./IndieLib_resources ./CPPEngine/.
    cd ./CPPEngine
    git add ./IndieLibX
    git add ./IndieLib_resources
    git commit -m "XXX"
    git push
    cd ../IndieLibX
fi;
fi;
}

updatezip()
{
if [ "X"=="X" ]; then
    cd ../..
    zip -r IndieLib_resources.zip ./IndieLib/IndieLib_resources
    zip -r IndieLibX.zip ./IndieLib/IndieLibX
    zip -r IndieLib.zip ./IndieLib
    cd ./IndieLib/IndieLibX
fi;
}

touchX()
{
	mkdir -p ../IndieLibX_Libraries
	cd ../IndieLibX_Libraries
	find  -type d -exec touch {} +
	find  -type f -exec touch {} +
	cd ../IndieLib_resources
	find  -type d -exec touch {} +
	find  -type f -exec touch {} +
	cd ../IndieLibX
	find  -type d -exec touch {} +
	find  -type f -exec touch {} +
	rm -fr ../tmp
	# rm -fr ../IndieLibX_Libraries
	# deltmp
    # updategit;
    # supdatezip;
}

if [ -z "$1" ] && [ -z "$2" ] && [ -z "$3" ] && [ -z "$4" ]; then
	clean;
    touchX;
	exit 0
fi

# ./make.sh Test LNX GNUMAKE GLSL4_5

solution="Test"
platform="LNX"
makesystem="GNUMAKE"
renderer="GLSL1_1"
#renderer="GLCG1_1"
#renderer="VULKAN"

if [ -n "$1" ]; then
	solution=$1
fi
if [ -n "$2" ]; then
	platform=$2
fi
if [ -n "$3" ]; then
	makesystem=$3
fi
if [ -n "$4" ]; then
	renderer=$4
fi
g_renderer=$renderer
export g_renderer
if [ "$platform"=="WIN" ]; then
if [ -z "${renderer}" ]; then
	renderer="VULKAN"
	renderer="D3D11"
fi
if [ -z "${makesystem}" ]; then
	makesystem="NMAKE"
fi
elif [ "$platform"=="MW" ]; then
if [ -z "${renderer}" ]; then
	renderer="VULKAN"
	renderer="D3D11"
fi
if [ -z "${makesystem}" ]; then
	if [ "$OS" == "Windows" ]; then
		makesystem="NMAKE"
	else
		makesystem="GNUMAKE"
	fi
fi
else
if [ -z "${renderer}" ]; then
	renderer=${g_renderer}
fi
if [ -z "${makesystem}" ]; then
	makesystem="GNUMAKE"
fi
fi

if [ "$2" == "NDK" ]; then
	makesystem=NDK
	#renderer=GLES2
fi

#RAKE=`which rake`
if [ "$OS" == "Darwin" ]; then
	#if [ -z "$RAKE" ]; then
	#	echo "Could not find rake."
	#	exit 1
	#fi

	# Global vars
	if [ -z ${SDK_NAME} ]; then
		UFW_GREP_RESULT=$(xcodebuild -showsdks | grep -o "iphoneos.*$")
		while read -r line; do
		iOS_VERSION="${line}"
		done <<< "${UFW_GREP_RESULT}"
		UFW_GREP_RESULT2=$(xcodebuild -showsdks | grep -o "macosx.*$")
		while read -r line; do
		MACOSX_VERSION="${line}"
		done <<< "${UFW_GREP_RESULT2}"

	fi

	iOS_VERSION=$(echo "${iOS_VERSION}" | grep -o "[0-9].*$")
	MACOSX_VERSION=$(echo "${MACOSX_VERSION}" | grep -o "[0-9].*$")


	# Extract the latest SDK version from the final field of the form: iphoneosX.Y
	readonly iOS_SDK=$(xcodebuild -showsdks | grep iphoneos | sort | tail -n 1 | awk '{print substr($NF, 9)}')
	readonly OSX_SDK=$(xcodebuild -showsdks | grep macosx | sort | tail -n 1 | awk '{print substr($NF, 7)}')
	# Extract Xcode version.
	readonly XCODE=$(xcodebuild -version | grep Xcode | cut -d " " -f2)

	echo "iOS_VERSION: $iOS_VERSION"
	echo "MACOSX_VERSION: $MACOSX_VERSION"
	echo "iOS_SDK: $iOS_SDK"
	echo "OSX_SDK: $OSX_SDK"
	echo "XCODE: $XCODE"

	export iOS_VERSION
	export MACOSX_VERSION
	export iOS_SDK
	export OSX_SDK
	export XCODE
fi

s=${solution}s
p=$platforms

usage()
{
echo "Usage: $0 -> Removes Temporary Files."
echo "Usage: $0 [< solutionname >] [<LNX|NDK|MW|OSX|iOS|WIN>] [<GNUMAKE|NDK|NMAKE|VC|XCODE>] [<GLCG1_1|GLSL1_1|GLSL4_5|GLES2|GLES3_2|D3D9|D3D10|D3D11|D3D12|VULKAN|METAL>] -> Builds an solution.";

echo "Example: ./make.sh Test LNX GNUMAKE GLSL1_1";
echo "Example: ./make.sh Test LNX GNUMAKE GLCG1_1";
echo "Example: ./make.sh Test LNX GNUMAKE VULKAN";
echo "Example: ./make.sh NDK NDK NDK GLES2";
echo "Example: ./make.sh Test OSX GNUMAKE GLSL1_1";
echo "Example: ./make.sh Test iOS GNUMAKE GLES2";

#echo "Usage: $0 buildlibrary ProjectName [< ${projects} >] -> Builds an single project.";
echo "Usage: $0 buildprj [< projectname >] [< LNX|NDK|OSX|iOS|NMAKE >] -> Builds an single project.";
echo "Usage: $0 buildsln [< solutionname >] [< LNX|NDK|OSX|iOS|NMAKE >] -> Builds an solution.";
}

# ./make.sh buildsln AllLibs LNX
if [ "$1" == "buildsln" ]; then
	MINPARAMS=2
	if [ $# -lt "$MINPARAMS" ]
	then
		usage
		exit 0
	fi
	cd ./DesktopProjects
	#echo "#4:$PYTHON build.py --buildsln $2 --buildtool $makesystem"
	$PYTHON build.py --buildsln $2 --buildtool $makesystem --platform $platform 2>&1 | tee ../build.log
	cd ..
	exit 0
fi

# ./make.sh buildprj STX LNX
if [ "$1" == "buildprj" ]; then
	MINPARAMS=2
	if [ $# -lt "$MINPARAMS" ]
	then
		usage
		exit 0
	fi
	cd ./DesktopProjects
	#echo "#4:$PYTHON build.py --buildprj $2 --buildtool $makesystem"
	$PYTHON build.py --buildprj $2 --buildtool $makesystem --platform ${platform} 2>&1 | tee ../build.log
	cd ..
	exit 0
fi

# ./make.sh buildlibrary STX LNX
if [ "$1" == "buildlibrary" ]; then

	MINPARAMS=2
	if [ $# -lt "$MINPARAMS" ]
	then
		usage
		exit 0
	fi
	export esolution=AllLibs
	export emakesystem=$makesystem
	cd ./DesktopProjects
	#echo "#4a:$PYTHON Projects.py --solution AllLibs --buildtool $makesystem --buildproject $2 --platform ${platform} --renderer ${renderer}"
	$PYTHON Projects.py --solution AllLibs --buildtool $makesystem --buildproject $2 --platform ${platform} --renderer ${renderer}
	#echo "#4b:$PYTHON Projects.py --buildtool $makesystem --solution None --mainphoneproject a_02_IND_Surface --package com.IndieLibApps.a_02_IND_Surface --apps True --buildproject $2 --platform ${platform} --renderer ${renderer}"
	$PYTHON Projects.py --buildtool $makesystem --solution None --mainphoneproject a_02_IND_Surface --package com.IndieLibApps.a_02_IND_Surface --apps True --buildproject $2 --platform ${platform} --renderer ${renderer}
	cd ..
	if [ -e "./DesktopProjects/projectsX/Makefile" ]; then
	make -C ./DesktopProjects/projectsX all 2>&1 | tee build.log
	fi
	exit 0
fi

solution1=$1

NDK_gl_code()
{
	JNI="./Libraries/MVC/NDK/build/$package/jni"
	rm -fr $JNI/libs
	mkdir -p $JNI/libs
	cd ./DesktopProjects
	#echo "#1a:$PYTHON Projects.py --solution ${solution} --buildtool NDK --platform ${platform} --renderer ${renderer}"
	$PYTHON Projects.py --solution ${solution} --buildtool NDK --platform NDK --renderer ${renderer}
	cd ..
	cd ./DesktopProjects
	#echo "#1b:$PYTHON Projects.py --buildtool NDK --solution None  --platform ${platform} --renderer ${renderer} --mainphoneproject $mainphoneproject --package $package --TargetDirectory ../Libraries/MVC/NDK/jni --OpenAL True --NDKPlatform 19"
	$PYTHON Projects.py --buildtool NDK --solution None --mainphoneproject $mainphoneproject --package $package --TargetDirectory ../Libraries/MVC/NDK/jni --OpenAL True --NDKPlatform 19 --platform NDK --renderer ${renderer}
	cd ..
	cd ./DesktopProjects/NDK_gl_code
	./androidbuild.sh $package ../test/testgles2.c
	cd ../..
	rm -fr $JNI/src
	cd ./DesktopProjects/NDK_gl_code
	./androidbuild3.sh $package
	cd ../..
}

NDK_gl_code2()
{
	#echo "make.sh:NDK_gl_code2:1"
	JNI="./Libraries/MVC/NDK/build/$package/jni"
	rm -fr $JNI/libs
	mkdir -p $JNI/libs
	cd ./DesktopProjects
	#echo "#2a:$PYTHON Projects.py --solution $solution1 --buildtool NDK"
	$PYTHON Projects.py --solution $solution --buildtool NDK --platform NDK --renderer ${renderer}
	cd ..
	cd ./DesktopProjects
	#echo "#2b:$PYTHON Projects.py --buildtool NDK --solution None --mainphoneproject $mainphoneproject --package $package --TargetDirectory ../Libraries/MVC/NDK/jni --OpenAL True --NDKPlatform 19"
	#echo "make.sh:NDK_gl_code2:2"
	$PYTHON Projects.py --solution None --mainphoneproject $mainphoneproject --package $package --TargetDirectory ../Libraries/MVC/NDK/jni --OpenAL True --NDKPlatform 19 --buildtool NDK --platform NDK --renderer ${renderer}
	#echo "make.sh:NDK_gl_code2:3"
	cd ..
	cd ./DesktopProjects/NDK_gl_code2
	#echo "make.sh:NDK_gl_code2:4"
	./androidbuild.sh $package ../test/testgles2.c
	#echo "make.sh:NDK_gl_code2:5"
	cd ../..
	cd ./DesktopProjects/NDK_gl_code2
	#echo "make.sh:NDK_gl_code2:6"
	./androidbuild3.sh $package
	#echo "make.sh:NDK_gl_code2:7"
	rm -fr $JNI/src
	cd ../..
	rm -fr ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK
	#cp -fr ./Libraries/MVC/NDK/obj/local ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK
}

NDK_SDL2()
{
	JNI="./Libraries/SDL2-$sdl2version/build/$package/jni"
	rm -fr $JNI/libs
	mkdir -p $JNI/libs
	cd ./DesktopProjects
	#echo "#3a:$PYTHON Projects.py --solution ${solution} --buildtool NDK --platform ${platform} --renderer ${renderer}"
	$PYTHON Projects.py --solution ${solution} --buildtool NDK --platform NDK --renderer ${renderer}
	cd ..
	cd ./DesktopProjects
	#echo "#3b:$PYTHON Projects.py --buildtool NDK --solution None --platform ${platform} --renderer ${renderer} --mainphoneproject $mainphoneproject --package $package --TargetDirectory ../Libraries/SDL2-$sdl2version/build/$package/jni --OpenAL True --NDKPlatform 19"
	$PYTHON Projects.py --buildtool NDK --solution None --mainphoneproject $mainphoneproject --package $package --TargetDirectory ../Libraries/SDL2-$sdl2version/build/$package/jni --OpenAL True --NDKPlatform 19 --platform NDK --renderer ${renderer}
	cd ..
	rm -fr $JNI/SDL2
	cd ./DesktopProjects/NDK_SDL2
	./androidbuild.sh $package ../test/testgles2.c
	cd ../..
	rm -fr $JNI/src
	cd ./DesktopProjects/NDK_SDL2
	./androidbuild3.sh $package
	cd ../..
}

if [ "$2" == "OSX" ]; then
	echo "2 Building to MacOS$MACOSX_VERSION"
fi
if [ "$2" == "iOS" ]; then
	echo "2 Building to iOS$iOS_VERSION"
	#cd ./DesktopProjects
	#echo "$PYTHON Projects.py --solution ${solution} --buildtool iOS --platform ${platform} --renderer ${renderer}"
	#$PYTHON Projects.py --solution ${solution} --buildtool iOS --platform ${platform} --renderer ${renderer}
	#echo "$PYTHON Projects.py --buildtool iOS --solution None --mainphoneproject $mainphoneproject --package $package --apps True" --renderer ${renderer} --platform ${platform}
	#$PYTHON Projects.py --buildtool iOS --solution None --mainphoneproject $mainphoneproject --package $package --apps True --renderer ${renderer} --platform ${platform}
	#cd ..
	#if [ -n "$RAKE" ]; then
	#	cd ./DesktopProjects/projectsX
	#	rake xcode && rake xcodebuild
	#	cd ../..
	#fi
	#make solution
	#exit 0
fi

if [ "$2" == "NDK" ]; then
	#NDK_SDL2
	NDK_gl_code2
	rm -fr ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK
	mv ./Libraries/MVC/NDK/obj/local ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK
	rm -fr ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK/armeabi/objs
	rm -fr ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK/mips/objs
	rm -fr ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK/x86/objs
	rm -f ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK/armeabi/*.so
	rm -f ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK/mips/*.so
	rm -f ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK/x86/*.so
	#zip -r ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK.zip ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK
	exit 0
fi

if [ "$1" == "help" ]; then
	usage
	exit 0
fi

#if [[ ${platformsArray[*]} =~ "${makesystem}" ]]; then
#	echo ""
#else
#	usage
#	exit 0
#fi

run()
{
export esolution=${solution}
export emakesystem=${makesystem}
export eplatform=${platform}
export erenderer=${renderer}
cd ./DesktopProjects
#echo "#4:$PYTHON Projects.py --solution ${solution} --buildtool ${makesystem} --platform ${platform} --renderer ${renderer}"

# buildtool GNUMAKE|NDK|NMAKE|VC|XCODE
# platform  LNX|MW|OSX|WIN|NDK|iOS
# renderer  GLCG1_1|GLSL1_1|GLSL4_5|GLES2|GLES3_2|D3D9|D3D10|D3D11|D3D12|VULKAN|METAL

$PYTHON Projects.py --solution ${solution} --buildtool ${makesystem} --platform ${platform} --renderer ${renderer}
$PYTHON Projects.py --buildtool ${makesystem} --solution None --mainphoneproject ${mainphoneproject} --package ${package} --apps True --platform ${platform} --renderer ${renderer}

cd ..
if [ -e "./DesktopProjects/projectsX/Makefile" ]; then
	make -C ./DesktopProjects/projectsX -f Makefile all 2>&1 | tee build.log;
fi;
if [ -e "./DesktopProjects/projectsX/Makefile.mw" ]; then
	make -C ./DesktopProjects/projectsX -f Makefile.mw all 2>&1 | tee build.log;
fi;
if [ -e "./DesktopProjects/projectsX/Makefile.iOS" ]; then
	make -C ./DesktopProjects/projectsX -f Makefile.iOS all 2>&1 | tee iOSbuild.log;
fi;
}

run

if [ "$platform" == "MW" ]; then
	cp ./Dependencies/libstdc++-6.dll.bak ./bin/libstdc++-6.dll
fi

if [ "$solution" == "AllLibs" ]; then
	rm -fr ../tmp
	if [ ! -f "../IndieLibX_Libraries" ]; then
		#rm -fr ../IndieLibX_Libraries # ???
		mkdir -p ../IndieLibX_Libraries
	fi
	if [ "$platform" == "LNX" ]; then
		if [ $(uname -m) == "i686" ]; then
			rm -fr ../IndieLibX_Libraries/IndieLibX_AllLibs_LNX32
			rm -fr ../IndieLibX_Libraries/IndieLibX_AllLibs_LNX32\ \(copy\)
			cp -fr ./lib ../IndieLibX_Libraries/IndieLibX_AllLibs_LNX32
			cp -fr ./lib ../IndieLibX_Libraries/IndieLibX_AllLibs_LNX32\ \(copy\)
			#zip -r ../IndieLibX_Libraries/IndieLibX_AllLibs_LNX32.zip ../IndieLibX_Libraries/IndieLibX_AllLibs_LNX32
		else
			rm -fr ../IndieLibX_Libraries/IndieLibX_AllLibs_LNX64
			rm -fr ../IndieLibX_Libraries/IndieLibX_AllLibs_LNX64\ \(copy\)
			cp -fr ./lib ../IndieLibX_Libraries/IndieLibX_AllLibs_LNX64
			cp -fr ./lib ../IndieLibX_Libraries/IndieLibX_AllLibs_LNX64\ \(copy\)
			#zip -r ../IndieLibX_Libraries/IndieLibX_AllLibs_LNX64.zip ../IndieLibX_Libraries/IndieLibX_AllLibs_LNX64
		fi
	fi
	if [ "$platform" == "OSX" ]; then
		if [ ! -f "./DesktopProjects/projectsX/Makefile.iOS" ]; then
			rm -fr ../IndieLibX_Libraries/IndieLibX_AllLibs_OSX
			rm -fr ../IndieLibX_Libraries/IndieLibX_AllLibs_OSX\ \(copy\)
			cp -fr ./lib ../IndieLibX_Libraries/IndieLibX_AllLibs_OSX
			cp -fr ./lib ../IndieLibX_Libraries/IndieLibX_AllLibs_OSX\ \(copy\)
			#zip -r ../IndieLibX_Libraries/IndieLibX_AllLibs_OSX.zip ../IndieLibX_Libraries/IndieLibX_AllLibs_OSX
		fi
	fi
	if [ -f "./DesktopProjects/projectsX/Makefile.iOS" ]; then
		rm -fr ../IndieLibX_Libraries/IndieLibX_AllLibs_iOS
	#mkdir -p ../IndieLibX_Libraries
	#mv ./lib/*.framework ../IndieLibX_Libraries/IndieLibX_AllLibs_iOS/.
		cp -fr ./lib ../IndieLibX_Libraries/IndieLibX_AllLibs_iOS
		#mkdir ./lib
		#zip -r ../IndieLibX_Libraries/IndieLibX_AllLibs_iOS.zip ../IndieLibX_Libraries/IndieLibX_AllLibs_iOS
	fi
	if [ -f "./Libraries/MVC/NDK/obj/local" ]; then
		rm -fr ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK
		mv ./Libraries/MVC/NDK/obj/local ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK
		rm -fr ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK/armeabi/objs
		rm -fr ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK/mips/objs
		rm -fr ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK/x86/objs
		rm -f ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK/armeabi/*.so
		rm -f ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK/mips/*.so
		rm -f ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK/x86/*.so
		#zip -r ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK.zip ../IndieLibX_Libraries/IndieLibX_AllLibs_NDK
	fi
fi
touchX;

