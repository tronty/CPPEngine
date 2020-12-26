#!/bin/bash
#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#

CURDIR=`pwd -P`

install1()
{

if [[ $OSTYPE == darwin* ]]; then
	XCODE=`which xcodebuild`
	if [ -z "$XCODE" ]; then
		echo "Could not find Xcode. Install Xcode:"
		exit 1
	fi
	BREW=`which brew`
	if [ -z "$BREW" ]; then
		echo "Could not find brew. Install brew:"
		echo "ruby -e \"$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)\""
		exit 1
	fi
else
	APTGET=`which apt-get`
	if [ -z "$APTGET" ]; then
		echo "Could not find apt-get."
		exit 1
	fi
fi

CURL=`which curl`
if [ -z "$CURL" ];then
	echo "Could not find curl. Install curl:"
	if [[ $OSTYPE == darwin* ]]; then
		brew install curl
	fi
	if [[ $OSTYPE == linux* ]]; then
		apt-get -y install curl
	fi
fi

url="http://www.tommironty.fi"
sdlurl="https://www.libsdl.org"
androidurl="http://dl.google.com/android"
adt="20131030"
ndk="android-ndk-r9d"
sdl2version="2.0.8"
sdlimage2version="2.0.0"
OSXversion="10.11.1"
Debianversion="8.4"
Ubuntuversion="16.04"

OS="Windows"
OK="N"
if [ -f "/etc/lsb-release" ]; then
	DINFO=$(cat /etc/lsb-release | grep DISTRIB_ID | sed s/.*=//)
	if [ $DINFO == "Ubuntu" ]; then
		OS="Ubuntu"
		sr=$(lsb_release -sr)
		if [[ "$sr" == "$Ubuntuversion" ]]; then
			OK="Y"
		fi
	fi
elif [ -f "/etc/debian_version" ]; then
	OS="Debian"
	sr=$(lsb_release -sr)
	if [[ "$sr" == "$Debianversion" ]]; then
		OK="Y"
	fi
elif [ -d "/Applications/Xcode.app" ]; then
	OS="Darwin"
	sr=$(sw_vers -productVersion)
	if [[ "$sr" == "$OSXversion" ]]; then
		OK="Y"
	fi
elif [ -d "/Applications/Xcode-Beta.app" ]; then
	OS="Darwin"
	sr=$(sw_vers -productVersion)
	if [[ "$sr" == "$OSXversion" ]]; then
		OK="Y"
	fi
fi

bitness="x86"
if [[ "$arch" == "x86_64" ]]; then
	bitness="x86_64"
elif [[ "$arch" == "i386" ]]; then
	bitness="x86_64"
fi

INC="/usr/include"
if [[ $OSTYPE == darwin* ]]; then
	INC="/usr/local/include"
fi
}

clean()
{
	cd $CURDIR
	if [[ $OSTYPE == darwin* ]]; then
		if [ -e ./$ndk-darwin-$bitness.tar ]; then
			rm -f ./$ndk-darwin-$bitness.tar
			rm -f ./$ndk-darwin-$bitness.tar.bz2
			rm -f ./adt-bundle-mac-$bitness-$adt.zip
		fi
	fi
	if [[ $OSTYPE == linux* ]]; then
		if [ -e ./$ndk-linux-$bitness.tar ]; then
			rm -f ./$ndk-linux-$bitness.tar
			rm -f ./$ndk-linux-$bitness.tar.bz2
			rm -f ./adt-bundle-linux-$bitness-$adt.zip
		fi
	fi
	rm -f ./SDL2-$sdl2version.tar
	rm -f ./SDL2_image-$sdlimage2version.tar
	rm -f ./SDL2-$sdl2version.tar.gz
	rm -f ./SDL2_image-$sdlimage2version.tar.gz
	rm -f ./IndieLibX.zip
	rm -f ./IndieLib_resources.zip
	rm -f ./IndieLibX_AllLibs_LNX64.zip
	rm -f ./IndieLibX_AllLibs_LNX32.zip
	rm -f ./IndieLibX_AllLibs_OSX64.zip
	rm -f ./IndieLibX_AllLibs_LNX64.zip
	rm -f ./IndieLibX_AllLibs_LNX32.zip
	rm -f ./IndieLibX_AllLibs_OSX64.zip
}

install2()
{
cd $CURDIR

YmdHMS=$(date +%Y-%m-%d-%H-%M-%S)

if [ "IndieLibX" == "IndieLibX" ]; then
	if [[ $OSTYPE == darwin* ]]; then		
		if [ -d ./IndieLib_resources ]; then
			mv -f ./IndieLib_resources "./IndieLib_resources_"$YmdHMS
		fi
		if [ -d ./IndieLibX ]; then
			mv -f ./IndieLibX "./IndieLibX_"$YmdHMS
		fi
		rm -fr $HOME/Documents/IndieLib_resources
	fi
	if [[ $OSTYPE == linux* ]]; then		
		if [ -f ./IndieLib_resources ]; then
			mv -f ./IndieLib_resources "./IndieLib_resources_"$YmdHMS
		fi
		if [ -f ./IndieLibX ]; then
			mv -f ./IndieLibX "./IndieLibX_"$YmdHMS
		fi
		rm -fr $HOME/Asiakirjat/IndieLib_resources
		rm -fr $HOME/Documents/IndieLib_resources
	fi
	#mv -f ./$ndk "./$ndk_"$YmdHMS
	#mv -f ./adt-bundle "./adt-bundle_"$YmdHMS
	clean
fi

if [[ $OSTYPE == darwin* ]]; then
	brew install cvs git subversion mercurial gcc valgrind cmake python bison flex libtool autoconf swig
	#brew install android-sdk android-ndk
	#brew install ant
	#brew install sdl2 sdl2_image
	brew tap caskroom/cask
	brew unlink brew-cask
	brew install brew-cask
	brew cask install java xquartz
	brew install wine
	brew install ios-sim
	#brew install node
	#npm install -g ios-deploy
	#sudo gem install cocoapods
	#sudo gem install xcodeproj
	sudo xcodebuild -license
fi
if [[ $OSTYPE == linux* ]]; then
	apt-get update
	apt-get upgrade
	apt-get dist-upgrade
	apt-get -y install gedit cvs git subversion mercurial gcc g++ build-essential valgrind filezilla cmake rar libsdl1.2-dev libsdl-image1.2-dev python bison flex libtool autoconf swig mypaint pencil blender libjpeg-dev libpng12-dev libdevil-dev libassimp-dev libpng-dev libjpeg-dev libtiff-dev libgif-dev libtinyxml-dev libsdl2-dev libsdl2-image-dev libopenal-dev openjdk-8-jdk mono-mcs lmms libusb-dev asunder python-dev libz-dev flashplugin-installer
	#gsettings set org.gnome.desktop.screensaver lock-enabled false
	# pip install PyOpenGL PyOpenGL_accelerate
fi

if [ ! -f $INC/SDL2/SDL.h ]; then
	rm -fr ./SDL2-$sdl2version
	$CURL -O $sdlurl/release/SDL2-$sdl2version.tar.gz
	gunzip ./SDL2-$sdl2version.tar.gz
	tar -xf ./SDL2-$sdl2version.tar
	cd ./SDL2-$sdl2version
	./configure
	make
	make install
	cd ..
	rm -fr ./SDL2-$sdl2version
fi
if [ ! -f $INC/SDL2/SDL_image.h ]; then
	rm -fr ./SDL2_image-$sdlimage2version
	$CURL -O $sdlurl/projects/SDL_image/release/SDL2_image-$sdlimage2version.tar.gz
	gunzip ./SDL2_image-$sdlimage2version.tar.gz
	tar -xf ./SDL2_image-$sdlimage2version.tar
	cd ./SDL2_image-$sdlimage2version
	./configure
	make
	make install
	cd ..
	rm -fr ./SDL2_image-$sdlimage2version
fi
}

install3()
{
if [ "IndieLibX" == "IndieLibX" ]; then
	$CURL -O $url/IndieLibX.zip
	unzip -x ./IndieLibX.zip #-d ./IndieLibX
	#find ./IndieLibX -type d -exec chmod a+x {} \;
	#find ./IndieLibX -type d -exec chmod a+r {} \;
	#find ./IndieLibX -type f -exec chmod a+r {} \;
	$CURL -O $url/IndieLib_resources.zip
	unzip -x ./IndieLib_resources.zip #-d ./IndieLib_resources
	#find ./IndieLib_resources -type d -exec chmod a+x {} \;
	#find ./IndieLib_resources -type d -exec chmod a+r {} \;
	#find ./IndieLib_resources -type f -exec chmod a+r {} \;
	if [[ $OSTYPE == darwin* ]]; then
		cp -fr ./IndieLib_resources ./Documents
	fi
	if [[ $OSTYPE == linux* ]]; then
	if [ -f $HOME/Asiakirjat ]; then
		cp -fr ./IndieLib_resources $HOME/Asiakirjat
	fi
	if [ -f $HOME/Documents ]; then
		cp -fr ./IndieLib_resources $HOME/Documents
	fi
	fi

	if [[ $OSTYPE == linux* ]]; then
	if [[ "$bitness" == "x86_64" ]]; then
		rm -fr ./IndieLibX_AllLibs_LNX64
		$CURL -O $url/IndieLibX_AllLibs_LNX64.zip
		unzip -x ./IndieLibX_AllLibs_LNX64.zip
		mv ./IndieLibX_AllLibs_LNX64/lib*.a /usr/local/lib
		rm -fr ./IndieLibX_AllLibs_LNX64
	else
		rm -fr ./IndieLibX_AllLibs_LNX32
		$CURL -O $url/IndieLibX_AllLibs_LNX32.zip
		unzip -x ./IndieLibX_AllLibs_LNX32.zip
		mv ./IndieLibX_AllLibs_LNX32/lib*.a /usr/local/lib
		rm -fr ./IndieLibX_AllLibs_LNX32
	fi
	fi
	if [[ $OSTYPE == darwin* ]]; then
		rm -fr ./IndieLibX_AllLibs_OSX64
		$CURL -O $url/IndieLibX_AllLibs_OSX64.zip
		unzip -x ./IndieLibX_AllLibs_OSX64.zip
		mv ./IndieLibX_AllLibs_OSX64/lib*.a /usr/local/lib
		rm -fr ./IndieLibX_AllLibs_OSX64
	fi
fi
}

install4()
{
if [[ $OSTYPE == darwin* ]]; then
	#echo "export ANDROID_SDK=/usr/local/Cellar/android-sdk/r17" >> .bash_profile
	#echo "export ANDROID_NDK=/usr/local/Cellar/android-ndk/r7b" >> .bash_profile
	if [ ! -d ./$ndk ]; then
		$CURL -O $androidurl/ndk/$ndk-darwin-$bitness.tar.bz2
		bunzip2 -d ./$ndk-darwin-$bitness.tar.bz2
		tar -xf ./$ndk-darwin-$bitness.tar
		#mv ./$ndk-darwin-$bitness ./$ndk
		#find ./$ndk -type d -exec chmod a+x {} \;
		#find ./$ndk -type d -exec chmod a+r {} \;
		#find ./$ndk -type f -exec chmod a+r {} \;
		echo "export NDK=$ndk" >> .bash_profile
		echo "export ANDROID_NDK=$CURDIR/$ndk" >> .bash_profile
		echo "export PATH=$ANDROID_NDK:$PATH" >> .bash_profile
		source ~/.bash_profile
	fi
	if [ ! -d ./adt-bundle ]; then
		$CURL -O $androidurl/adt/adt-bundle-mac-$bitness-$adt.zip
		unzip -x ./adt-bundle-mac-$bitness-$adt.zip  #-d ./adt-bundle
		mv ./adt-bundle-mac-$bitness-$adt ./adt-bundle
		#find ./adt-bundle -type d -exec chmod a+x {} \;
		#find ./adt-bundle -type d -exec chmod a+r {} \;
		#find ./adt-bundle -type f -exec chmod a+r {} \;
		echo "export ANDROID_SDK=$CURDIR/adt-bundle/sdk" >> .bash_profile
		echo "export PATH=$ANDROID_SDK:$ANDROID_SDK/tools:$ANDROID_SDK/platform-tools:$PATH" >> .bash_profile
		source ~/.bash_profile
	fi
fi

if [[ $OSTYPE == linux* ]]; then
	if [ ! -f ./$ndk ]; then
		$CURL -O $androidurl/ndk/$ndk-linux-$bitness.tar.bz2
		bunzip2 -d ./$ndk-linux-$bitness.tar.bz2
		tar -xf ./$ndk-linux-$bitness.tar
		#mv ./$ndk-linux-$bitness ./$ndk
		#find ./$ndk -type d -exec chmod a+x {} \;
		#find ./$ndk -type d -exec chmod a+r {} \;
		#find ./$ndk -type f -exec chmod a+r {} \;
		echo "export NDK=$ndk" >> .bashrc
		echo "export ANDROID_NDK=$CURDIR/$ndk" >> .bashrc
		echo "export PATH=$ANDROID_NDK:$PATH" >> .bashrc
		source ~/.bashrc
	fi
	if [ ! -f ./adt-bundle ]; then
		$CURL -O $androidurl/adt/adt-bundle-linux-$bitness-$adt.zip
		unzip -x ./adt-bundle-linux-$bitness-$adt.zip #-d ./adt-bundle
		mv ./adt-bundle-linux-$bitness-$adt ./adt-bundle
		#find ./adt-bundle -type d -exec chmod a+x {} \;
		#find ./adt-bundle -type d -exec chmod a+r {} \;
		#find ./adt-bundle -type f -exec chmod a+r {} \;
		echo "export ANDROID_SDK=$CURDIR/adt-bundle/sdk" >> .bashrc
		echo "export PATH=$ANDROID_SDK:$ANDROID_SDK/tools:$ANDROID_SDK/platform-tools:$PATH" >> .bashrc
		source ~/.bashrc
	fi
fi

echo "#"
echo "#"
echo "#"
echo "# Open Android SDK Manager window and mark checkbox Android SDK Build-tools 19.1 to be installed."
echo "#"
echo "#"
echo "#"
#ANDROID=`which android`
ANDROID='$CURDIR/adt-bundle/sdk/tools/android'
if [ "X" == "Y" ]; then
	$ANDROID update sdk --no-ui
	$ANDROID update sdk --filter tools,platform-tools,build-tools-19.1.0
else
	$ANDROID
fi
if [ -n $ANDROID ]; then
	emulator_images=`$ANDROID list avds | grep "Name:" | cut -f 2 -d ":"`
	emulator_list=($emulator_images)
	if [[ " ${emulator_list[@]} " =~ "avd19" ]]; then
		echo "avd19 found."
	else
		$ANDROID create avd -n avd19 -t 1
	fi
fi

CURDIR0="$CURDIR/IndieLibX/DesktopProjects"
SDLPATH="$CURDIR/IndieLibX/Libraries/SDL2-$sdl2version"

clean
}

if [[ $OSTYPE == linux* ]]; then
if [ ! -f $INC/GL/gl.h ]; then
	echo "Could not find $INC/GL/gl.h. Install mesa-common-dev:"
	apt-get -y install mesa-common-dev
fi
if [ ! -f $INC/GL/glu.h ]; then
	echo "Could not find $INC/GL/glu.h. Install libglu1-mesa-dev:"
	apt-get -y install libglu1-mesa-dev
fi
if [ ! -f /usr/include/Cg/cg.h ]; then
	echo "Could not find /usr/include/Cg/cg.h. Install nvidia-cg-toolkit:"
	apt-get -y install nvidia-cg-toolkit
fi
fi

install()
{
	install3
	install4
	su root
	install1
	install2
}

if [[ "$OK" != "Y" ]]; then
	yn="Y"
	echo "This system is not one of: Ubuntu $Ubuntuversion, Debian $Debianversion or OSX $OSXversion ."
	echo "Continue at your own risk."
	while true; do
    		read -p "Do you wish to install IndieLibX [YyNn]?" yn
    		case $yn in 
        		[Yy]* ) install; break;;
        		[Nn]* ) exit 0;;
        		* ) echo "Please answer yes or no.";;
    		esac
	done
else
	install;
fi
