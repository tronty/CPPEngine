#!/bin/bash
#
# Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
# Commands below in this file were tested on following operating systems:
# 	macOS High Sierra (64-bit) (any other version of 64-bit macOS may also do as well)
# 	Ubuntu 18.04.2 LTS Bionic Beaver (64-bit)
#	Ubuntu 20.04.2 LTS Focal Fossa (64-bit)
# 	Linux Mint 19.1 "Tessa"  - Cinnamon (32-bit and 64-bit)
# 	Linux Mint 19.2 "Tina" 	 - Cinnamon (32-bit and 64-bit) also now available should do equally well
# 	Linux Mint 19.3 "Tricia" - Cinnamon (32-bit and 64-bit) also now available should do equally well
# 	Linux Mint 20 	"Ulyana" - Cinnamon (64-bit) also now available should do equally well
# 	Linux Mint 20.1 "Ulyssa" - Cinnamon (64-bit) also now available should do equally well
# 	I use currently mainly Linux Mint 19.1 "Tessa" - Cinnamon where there is the lightdm as a display manager
# 	and system snapshots are available on update manager
# 	(may save time if the system breaks and must be restored to a some previous state)

OS="Windows"
platform="WIN"
UbuntuBased="N"
OS_="linux"
bitness="x86"

	if [ -f "/etc/lsb-release" ]; then
		DINFO=$(cat /etc/lsb-release | grep DISTRIB_ID | sed s/.*=//)
		if [ $DINFO == "Ubuntu" ]; then
			OS="Ubuntu"
			platform="LNX"
			UbuntuBased="Y"
		fi
		if [ $DINFO == "LinuxMint" ]; then
			OS="LinuxMint"
			platform="LNX"
			UbuntuBased="Y"
		fi
	elif [ -f "/etc/debian_version" ]; then
		OS="Debian"
		platform="LNX"
		UbuntuBased="N"
	elif [ -d "/Applications/Xcode.app" ]; then
		OS="Darwin"
		platform="OSX"
		UbuntuBased="N"
	elif [ -d "/Applications/Xcode-Beta.app" ]; then
		OS="Darwin"
		platform="OSX"
		UbuntuBased="N"
	fi

	if [[ $OSTYPE == darwin* ]]; then
		OS_="darwin"
	fi

	if [[ "$arch" == "x86_64" ]]; then
		bitness="x86_64"
	elif [[ "$arch" == "i386" ]]; then
		bitness="x86_64"
	fi

DISPLAY_MANAGER="lightdm"
SDL2_VERSION="2.0.9"
SDL2_IMAGE_VERSION="2.0.5"
NDK_VERSION="r9d"
ANDROID_NDK_URL="http://dl.google.com/android/ndk/android-ndk-$NDK_VERSION-$OS_-$bitness.tar.bz2"
INDIELIBX_URL="http://tommironty.fi/IndieLibX.zip"
INDIELIB_RESOURCES_URL="http://tommironty.fi/IndieLib_resources.zip"
AMDVLK_DRIVER="amdvlk_2019.Q3.2_amd64.deb"
AMDVLK_DRIVER_URL="https://github.com/GPUOpen-Drivers/AMDVLK/releases/download/v-2019.Q3.2/$AMDVLK_DRIVER"
NVIDIA_DRIVER="NVIDIA-Linux-$bitness-390.129.run"
NVIDIA_DRIVER_URL="http://us.download.nvidia.com/XFree86/Linux-$bitness/390.129/$NVIDIA_DRIVER"
SDL2_URL="https://www.libsdl.org/release/SDL2-$SDL2_VERSION.tar.gz"
SDL2_IMAGE_URL="https://www.libsdl.org/projects/SDL_image/release/SDL2_image-$SDL2_IMAGE_VERSION.tar.gz"
HOMEBREW_URL="https://raw.githubusercontent.com/Homebrew/install/master/install"
CG="Cg-3.1_April2012.dmg"
CG_URL="http://developer.download.nvidia.com/cg/Cg_3.1/$CG"
XQUARTZ="XQuartz-2.7.11.dmg"
XQUARTZ_URL="https://dl.bintray.com/xquartz/downloads/$XQUARTZ"

cd $HOME

update()
{
	apt-get update
	apt-get upgrade
	#apt-get dist-upgrade
}

if [[ $OSTYPE == linux* ]]; then
	update
	apt-get -y install curl
	update
fi

installXQUARTZ()
{
    TOOLS=XQUARTZ.dmg
    if [ ! -f "$TOOLS" ]; then
        curl "$XQUARTZ_URL" -o "$TOOLS"
    fi
    TMPMOUNT=`/usr/bin/mktemp -d /tmp/clitools.XXXX`
    hdiutil attach "$TOOLS" -mountpoint "$TMPMOUNT"
    installer -pkg "$(find $TMPMOUNT -name '*.mpkg' -o -name '*.pkg')" -target /
    hdiutil detach "$TMPMOUNT"
    rm -rf "$TMPMOUNT"
    rm "$TOOLS"
}

installCG()
{
    TOOLS=CG.dmg
    if [ ! -f "$TOOLS" ]; then
        curl "$CG_URL" -o "$TOOLS"
    fi
    TMPMOUNT=`/usr/bin/mktemp -d /tmp/clitools.XXXX`
    hdiutil attach "$TOOLS" -mountpoint "$TMPMOUNT"
    installer -pkg "$(find $TMPMOUNT -name '*.mpkg' -o -name '*.pkg')" -target /
    hdiutil detach "$TMPMOUNT"
    rm -rf "$TMPMOUNT"
    rm "$TOOLS"
}

installXcodeCLItools()
{
    # https://gist.github.com/trinitronx/6217746
    OSX_VERS=$(sw_vers -productVersion | awk -F "." '{print $2}')

    # Get Xcode CLI tools
    # https://devimages.apple.com.edgekey.net/downloads/xcode/simulators/index-3905972D-B609-49CE-8D06-51ADC78E07BC.dvtdownloadableindex
    # https://developer.apple.com/downloads/index.action
    TOOLS=clitools.dmg
    if [ ! -f "$TOOLS" ]; then
        if [ "$OSX_VERS" -eq 7 ]; then
	        DMGURL=http://devimages.apple.com/downloads/xcode/command_line_tools_for_xcode_os_x_lion_april_2013.dmg
        elif [ "$OSX_VERS" -eq 8 ]; then
  	        DMGURL=http://devimages.apple.com/downloads/xcode/command_line_tools_for_xcode_os_x_mountain_lion_april_2013.dmg
        elif [ "$OSX_VERS" -eq 9 ]; then
	        DMGURL=http://adcdownload.apple.com/Developer_Tools/command_line_tools_os_x_mavericks_for_xcode__late_october_2013/command_line_tools_os_x_mavericks_for_xcode__late_october_2013.dmg
        elif [ "$OSX_VERS" -eq 10 ]; then
	        DMGURL=http://adcdownload.apple.com/Developer_Tools/Command_Line_Tools_OS_X_10.10_for_Xcode_6.3.2/commandlinetoolsosx10.10forxcode6.3.2.dmg
        elif [ "$OSX_VERS" -eq 11 ]; then
	        DMGURL=http://adcdownload.apple.com/Developer_Tools/Command_Line_Tools_OS_X_10.11_for_Xcode_7.3.1/Command_Line_Tools_OS_X_10.11_for_Xcode_7.3.1.dmg
        elif [ "$OSX_VERS" -eq 12 ]; then
	        DMGURL=http://adcdownload.apple.com/Developer_Tools/Command_Line_Tools_macOS_10.12_for_Xcode_8.1/Command_Line_Tools_macOS_10.12_for_Xcode_8.1.dmg		
		#Command_Line_Tools_macOS_10.12_for_Xcode_9.1.dmg	
	        #DMGURL=http://adcdownload.apple.com/Developer_Tools/Command_Line_Tools_macOS_10.12_for_Xcode_9.1/Command_Line_Tools_macOS_10.12_for_Xcode_9.1.dmg
        fi
        curl "$DMGURL" -o "$TOOLS"
    fi
    TMPMOUNT=`/usr/bin/mktemp -d /tmp/clitools.XXXX`
    hdiutil attach "$TOOLS" -mountpoint "$TMPMOUNT"
    installer -pkg "$(find $TMPMOUNT -name '*.mpkg' -o -name '*.pkg')" -target /
    hdiutil detach "$TMPMOUNT"
    rm -rf "$TMPMOUNT"
    rm "$TOOLS"
}

installNDK()
{
	echo ""
	echo "Installing android-ndk-$NDK_VERSION-$OS_-$bitness."
	echo ""

	if [ ! -e "$HOME/android-ndk-$NDK_VERSION-$OS_-$bitness" ]; then
		if [ ! -f "./android-ndk-$NDK_VERSION-$OS_-$bitness.tar" ]; then
			curl -O $ANDROID_NDK_URL
    			bunzip2 -v -d ./android-ndk-$NDK_VERSION-$OS_-$bitness.tar.bz2
		fi
    		tar xvf ./android-ndk-$NDK_VERSION-$OS_-$bitness.tar
		#rm -fr $HOME/android-ndk-$NDK_VERSION-$OS_-$bitness
		#mv ./android-ndk-$NDK_VERSION-$OS_-$bitness $HOME/android-ndk-$NDK_VERSION-$OS_-$bitness
		if [[ $OSTYPE == linux* ]]; then
    			echo 'export NDK=android-ndk-$NDK_VERSION-$OS_-$bitness' >>$HOME/.bashrc
    			echo 'export MESA_GL_VERSION_OVERRIDE=3.3' >>$HOME/.bashrc
    			echo 'export MESA_GL_VERSION_OVERRIDE=4.3' >>$HOME/.bashrc
		fi		
		if [[ $OSTYPE == darwin* ]]; then
    			echo 'export NDK=android-ndk-$NDK_VERSION-$OS_-$bitness' >>$HOME/.bash_profile
    			echo 'export MESA_GL_VERSION_OVERRIDE=3.3' >>$HOME/.bash_profile
    			echo 'export MESA_GL_VERSION_OVERRIDE=4.3' >>$HOME/.bash_profile
		fi
	fi
}

installIndieLibX()
{
	echo ""
	echo "Installing IndieLibX."
	echo ""

	if [ ! -e "$HOME/IndieLibX" ]; then
		if [ ! -f "./IndieLibX.zip" ]; then
			curl -O $INDIELIBX_URL
    			unzip -x ./IndieLibX.zip
		fi
	fi
}

installIndieLib_resources()
{
	echo ""
	echo "Installing IndieLib_resources."
	echo ""

	if [ ! -e "$HOME/IndieLib_resources" ]; then
		if [ ! -f "./IndieLib_resources.zip" ]; then
			curl -O $INDIELIB_RESOURCES_URL
    			unzip -x ./IndieLib_resources.zip
		fi
	fi
}

installGraphicsdriver()
{
	sudo ubuntu-drivers autoinstall
	sudo reboot
}

installGraphicsdriver_()
{
	sudo apt install nvidia-driver-460
	sudo reboot
}

installAMDdriver()
{
	if [[ "$bitness" == "x86_64" ]]; then
		#update

		if [ ! -f "./$AMDVLK_DRIVER" ]; then
			echo "curl -O $AMDVLK_DRIVER_URL"
		fi
		echo "dpkg -r amdvlk # If old version is installed on the machine, remove it first"
		echo "dpkg -i $AMDVLK_DRIVER"
		echo "sudo apt-get -f install"
		echo "sudo apt-get -y install libvulkan1 vulkan-utils"

		#update
	fi
}
installNVIDIAdriver()
{
	#update

	echo "add-apt-repository ppa:graphics-drivers/ppa"
	echo "sudo apt update"
	#sudo apt-get -y install mesa-vulkan-drivers # If you wish to use the Vulkan drivers
	#sudo apt-get -y install bumblebee-nvidia
	#echo "sudo apt-get -y install nvidia-driver-390 libnvidia-gl-390"
	echo "sudo apt-get -y install nvidia-driver-430 libnvidia-gl-430 libnvidia-gl-430:i386"
	echo "sudo apt-get -y install nvidia-settings"

	#add-apt-repository ppa:oibaf/graphics-drivers
	#sudo apt install nvidia-graphics-drivers-396
	echo "sudo apt-get -y install libvulkan1 vulkan-utils"
	#update
}

installNVIDIAdriver_()
{
	if [ ! -f "./$NVIDIA_DRIVER" ]; then
		echo "curl -O $NVIDIA_DRIVER_URL"
	fi
	echo "service $DISPLAY_MANAGER stop"
	echo "sh ./$NVIDIA_DRIVER -a -q -X --ui=none -f -n"
	echo "service $DISPLAY_MANAGER start"
	#update
	echo "sudo apt-get -y install nvidia-settings"
	echo "sudo apt-get -y install libvulkan1 vulkan-utils"
	#update
}
installubuntudrivers()
{
	if [[ "$UbuntuBased" == "Y" ]]; then
	    echo "ubuntu-drivers devices"
	    echo "sudo ubuntu-drivers autoinstall"
	echo "ubuntu-drivers list"
    	echo "lspci -vnn | grep VGA"
    	echo "sudo reboot"
    	echo "dkms status"
    	echo "sudo apt-get install libvulkan1 vulkan-utils"
    	echo "#sudo apt-get -y install nvidia-settings"
    fi
}
installvideodriver()
{
	echo ""
	echo "Printing video driver installing instructions"
	echo ""
	if [[ "$UbuntuBased" == "Y" ]]; then
		installubuntudrivers
	else
		if [[ "$platform" == "LNX" ]]; then
			# ??? GRAPHICSCARDVENDOR="lshw -C video | awk -F'product: ' '/product/{print $2}'"
			if [[ "NVIDIA Corporation" == "$GRAPHICSCARDVENDOR" ]]; then
				installNVIDIAdriver	# If you have an NVIDIA graphics card attached to the computer
			fi
			if [[ "AMD" == "$GRAPHICSCARDVENDOR" ]]; then
				installAMDdriver	# If you have an AMD graphics card attached to the computer
			fi
		fi
	fi
	echo "glxinfo | more"
	echo "vulkaninfo | more"
}
installadditionalpackages()
{
	if [[ "Y" == "Y" ]]; then
		echo ""
		echo "Installing additional packages."
		echo ""
		update
		apt-get -y install cvs
		apt-get -y install svn
		apt-get -y install subversion
		apt-get -y install mercurial
		apt-get -y install maven
		apt-get -y install valgrind
		apt-get -y install swig
		apt-get -y install mypaint
		apt-get -y install pencil
		apt-get -y install blender
		apt-get -y install libjpeg-dev
		apt-get -y install libpng12-dev
		apt-get -y install libdevil-dev
		apt-get -y install libtinyxml-dev
		apt-get -y install openjdk-8-jdk
		apt-get -y install mono-mcs
		apt-get -y install lmms
		apt-get -y install libusb-dev
		apt-get -y install libz-dev
		apt-get -y install flashplugin-installer
		#gsettings set org.gnome.desktop.screensaver lock-enabled false

		apt-get -y install libtinyxml2-dev
		apt-get -y install python-pydispatch
		apt-get -y install python3-pydispatch
		apt-get -y install python3-numpy
		apt-get -y install python-numpy

		apt-get -y install libjogl2-java-doc
		apt-get -y install libjogl2-java
		apt-get -y install libjutils-java-doc
		apt-get -y install libjutils-java
		apt-get -y install libjogl2-toolkits
		apt-get -y install libjogl2-jni
		apt-get -y install games-python3-dev
		#apt-get -y install conda
		#apt-get -y install python3-keras
		#apt-get -y install python3-lasagne

		apt-get -y install unifdef
		apt-get -y install exuberant-ctags
		apt-get -y install cscope
		apt-get -y install seascope
		apt-get -y install codequery
		apt-get -y install castxml
		apt-get -y install python-pygccxml
		apt-get -y install python-pygccxml-doc

		apt-get -y install libfreetype6-dev
		apt-get -y install libglfw3-dev
		apt-get -y install libglew-dev
		apt-get -y install mingw-w64-i686-dev

		update
	fi
}
OSXinfo()
{
	echo ""
	echo "Notes for smooth installation:"
	echo ""
	echo "OSX crash: libpng warning: Application built with libpng-1.6.26 but running with 1.5.0 ???"
	echo "I solved this by removing the libpng included in the X11 of the OSX:"
	echo "??? sudo mv /opt/X11/include/libpng16/png.h /opt/X11/include/libpng16/png_.h"
	echo ""
	echo "header file typedef collision on OSX: ???"
	echo "typedef __char16_t char16_t"
	echo "typedef __char32_t char32_t"
	echo "on header file /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/../include/c++/v1/__config"
	echo "I excluded these with following preprocessor directives"
	echo "(I modified the __config file):"
	echo "#if 0"
	echo "typedef __char16_t char16_t"
	echo "typedef __char32_t char32_t"
	echo "#endif"
	echo ""
	yn1="Y"
	while true; do
		read -p "Do you wish to continue [YyNn]?" yn1
		case $yn1 in
			[Yy]* ) break;;
			[Nn]* ) exit 0;;
			* ) echo "Please answer yes or no.";;
		esac
	done
}
linuxinfo()
{
	echo ""
	echo "Notes for smooth installation:"
	echo ""
	echo "Following two commands (or lines) may be used ideally"
	echo "right after a newly installed linux distribution."
	echo "	apt-mark hold linux-image-generic linux-headers-generic"
	echo "	apt-mark showhold"
	echo ""
	echo "Following two commands (or lines) may fix the situation"
	echo "whenever the apt system fails to function properly"
	echo "(May require rerunning this script)."
	echo "	dpkg --configure -a"
	echo "	apt-get install -f"
	echo ""
	# echo "	sudo apt install --fix-broken"
	yn1="Y"
	while true; do
		read -p "Do you wish to continue [YyNn]?" yn1
		case $yn1 in
			[Yy]* ) break;;
			[Nn]* ) exit 0;;
			* ) echo "Please answer yes or no.";;
		esac
	done
}
installlinuxpackages()
{
	if [[ "Y" == "Y" ]]; then
		update
		apt-get -y install curl
		apt-get -y install audacious
		apt-get -y install mesa-common-dev
		apt-get -y install libglu1-mesa-dev
		apt-get -y install freeglut3-dev
		apt-get -y install nvidia-cg-toolkit
		apt-get -y install libopenal-dev
		apt-get -y install gcc
		apt-get -y install g++
		apt-get -y install build-essential
  		apt-get -y install ant
		apt-get -y install libsdl1.2-dev
		apt-get -y install libsdl-image1.2-dev
		apt-get -y install libsdl2-dev
		apt-get -y install libsdl2-image-dev
		apt-get -y install bison
		apt-get -y install flex
		apt-get -y install libtool
		apt-get -y install autoconf
		#apt-get remove libassimp-dev
		apt-get -y install libassimp-dev

		apt-get -y install python
		apt-get -y install python-dev
		apt-get -y install libpython3-dev
		apt-get -y install python3-dev
		apt-get -y install python-setuptools
		apt-get -y install python-pip
		apt-get -y install python3-setuptools
		apt-get -y install python3-pip

		#apt-get -y install libwine-dev
  		apt-get -y install wine-development

		apt-get -y install filezilla
		apt-get -y install xfburn
		apt-get -y install asunder
		apt-get -y install gedit
		apt-get -y install cmake
		apt-get -y install rar
		apt-get -y install git
		apt-get -y install firefox
		update
	fi
}
installSDL2()
{
	echo ""
	echo "Installing SDL2."
	echo ""
	if [ ! -e "./SDL2-$SDL2_VERSION" ]; then
		if [ ! -f "./SDL2-$SDL2_VERSION.tar" ]; then
			curl -O $SDL2_URL
			gunzip -v ./SDL2-$SDL2_VERSION.tar.gz
		fi
			tar xvf ./SDL2-$SDL2_VERSION.tar
	fi
	cd ./SDL2-$SDL2_VERSION
	./configure
	make
	make install
	cd ..
	echo ""
	echo "Installing SDL2_image."
	echo ""
	if [ ! -e "./SDL2_image-$SDL2_IMAGE_VERSION" ]; then
		if [ ! -f "./SDL2_image-$SDL2_IMAGE_VERSION.tar" ]; then
			curl -O $SDL2_IMAGE_URL
			gunzip -v ./SDL2_image-$SDL2_IMAGE_VERSION.tar.gz
		fi
			tar xvf ./SDL2_image-$SDL2_IMAGE_VERSION.tar
	fi
	cd ./SDL2_image-$SDL2_IMAGE_VERSION
	./configure
	make
	make install
	cd ..
}

installBrew()
{
	echo ""
	echo "Installing Homebrew."
	echo ""
	#ruby -e "$(curl -fsSL $HOMEBREW_URL)"
	ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

	echo ""
	echo "Installing packages."
	echo ""
	brew install curl
	##brew tap homebrew/dupes
  	#brew install unzip
  	#mv /usr/local/bin/unzip /usr/local/bin/unzip_original
	#ln -s /usr/local/opt/unzip/bin/unzip /usr/local/bin/unzip

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
	#gem install cocoapods
	#gem install xcodeproj
}
installXcode()
{
	xcode-select --install
	xcodebuild -license
	#installXcodeCLItools
}
installdarwinpackages()
{
	if [[ "Y" == "Y" ]]; then
		yn2="Y"
		while true; do
			read -p "Do you wish to install Xcode [YyNn]?" yn2
			case $yn2 in
				[Yy]* ) installXcode; break;;
				[Nn]* ) break;;
				* ) echo "Please answer yes or no.";;
			esac
		done
	fi
	if [[ "Y" == "Y" ]]; then
		yn2="Y"
		while true; do
			read -p "Do you wish to install Xcode Command Line Tools [YyNn]?" yn2
			case $yn2 in
				[Yy]* ) installXcodeCLItools; break;;
				[Nn]* ) break;;
				* ) echo "Please answer yes or no.";;
			esac
		done
	fi
	if [[ "Y" == "Y" ]]; then
		yn2="Y"
		while true; do
			read -p "Do you wish to install Brew package manager [YyNn]?" yn2
			case $yn2 in
				[Yy]* ) installBrew; break;;
				[Nn]* ) break;;
				* ) echo "Please answer yes or no.";;
			esac
		done
	fi
	if [[ "Y" == "Y" ]]; then
		yn2="Y"
		while true; do
			read -p "Do you wish to install SDL2 [YyNn]?" yn2
			case $yn2 in
				[Yy]* ) installSDL2; break;;
				[Nn]* ) break;;
				* ) echo "Please answer yes or no.";;
			esac
		done
	fi
	if [[ "Y" == "Y" ]]; then
		yn2="Y"
		while true; do
			read -p "Do you wish to install NVIDIA CG Toolkit [YyNn]?" yn2
			case $yn2 in
				[Yy]* ) installCG; break;;
				[Nn]* ) break;;
				* ) echo "Please answer yes or no.";;
			esac
		done
	fi
}

removetmpfiles()
{
	echo ""
	echo "Removing temporary files."
	echo ""
	rm -fr ./android-ndk-$NDK_VERSION-linux-$bitness.tar.bz2
	rm -fr ./android-ndk-$NDK_VERSION-darwin-$bitness.tar.bz2
	rm -fr ./android-ndk-$NDK_VERSION-linux-$bitness.tar
	rm -fr ./android-ndk-$NDK_VERSION-darwin-$bitness.tar
	rm -fr ./android-ndk-$NDK_VERSION
	rm -fr ./$AMDVLK_DRIVER
	rm -fr ./$NVIDIA_DRIVER
	rm -fr ./SDL2-$SDL2_VERSION
	rm -fr ./SDL2-$SDL2_VERSION.tar.gz
	rm -fr ./SDL2-$SDL2_VERSION.tar
	rm -fr ./SDL2_image-$SDL2_IMAGE_VERSION
	rm -fr ./SDL2_image-$SDL2_IMAGE_VERSION.tar.gz
	rm -fr ./SDL2_image-$SDL2_IMAGE_VERSION.tar
	rm -fr ./clitools.dmg
	rm -fr ./CG.dmg
	rm -fr ./XQUARTZ.dmg
}

aptmarkhold()
{
    apt-mark hold linux-image-generic linux-headers-generic
}

fixaptget()
{
	dpkg --configure -a
	apt-get install -f
}

if [[ $OSTYPE == linux* ]]; then

	linuxinfo
    	yn="Y"
    	while true; do
	    	read -p "Do you wish to hold linux-image [YyNn]?" yn
	    	case $yn in
	        	[Yy]* ) aptmarkhold; break;;
	        	[Nn]* ) break;;
	        	* ) echo "Please answer yes or no.";;
	    	esac
    	done
    	yn="Y"
    	while true; do
	    	read -p "Do you wish to fix apt-get [YyNn]?" yn
	    	case $yn in
	        	[Yy]* ) fixaptget; break;;
	        	[Nn]* ) break;;
	        	* ) echo "Please answer yes or no.";;
	    	esac
    	done
	installlinuxpackages

	if [[ "Y" == "Y" ]]; then
		yn2="Y"
		while true; do
			read -p "Do you wish to install additional packages [YyNn]?" yn2
			case $yn2 in
				[Yy]* ) installadditionalpackages; break;;
				[Nn]* ) break;;
				* ) echo "Please answer yes or no.";;
			esac
		done
	fi
fi
if [[ $OSTYPE == darwin* ]]; then
	installdarwinpackages

	yn5="Y"
	while true; do
		read -p "Do you wish to install XQUARTZ [YyNn]?" yn5
		case $yn5 in
		  [Yy]* ) installXQUARTZ; break;;
		  [Nn]* ) break;;
		  * ) echo "Please answer yes or no.";;
		esac
	done
fi
yn5="Y"
while true; do
	read -p "Do you wish to install NDK (android-ndk-$NDK_VERSION-$OS_-$bitness) [YyNn]?" yn5
	case $yn5 in
	  [Yy]* ) installNDK; break;;
	  [Nn]* ) break;;
	  * ) echo "Please answer yes or no.";;
	esac
done
if [[ $OSTYPE == linux* ]]; then
	if [[ "Y" == "N" ]]; then
		yn4="Y"
		while true; do
			read -p "Do you wish to install video driver [YyNn]?" yn4
			case $yn4 in
				[Yy]* ) installvideodriver; break;;
		  		[Nn]* ) break;;
		  		* ) echo "Please answer yes or no.";;
			esac
		done
	fi
fi
if [[ $OSTYPE == darwin* ]]; then
	if [ -f "/opt/X11/include/libpng16/png.h" ]; then
		mv /opt/X11/include/libpng16/png.h /opt/X11/include/libpng16/png_.h
	fi
fi
if [[ "N" == "Y" ]]; then
if [[ $OS != "Windows" ]]; then
if [ ! -e "$HOME/IndieLibX" ]; then
		yn4="Y"
		while true; do
			read -p "Do you wish to install IndieLibX [YyNn]?" yn4
			case $yn4 in
				[Yy]* ) installIndieLibX; break;;
		  		[Nn]* ) break;;
		  		* ) echo "Please answer yes or no.";;
			esac
		done
fi
fi
fi
if [[ "N" == "Y" ]]; then
if [[ $OS != "Windows" ]]; then
if [ ! -e "$HOME/IndieLib_resources" ]; then
		yn4="Y"
		while true; do
			read -p "Do you wish to install IndieLib_resources [YyNn]?" yn4
			case $yn4 in
				[Yy]* ) installIndieLib_resources; break;;
		  		[Nn]* ) break;;
		  		* ) echo "Please answer yes or no.";;
			esac
		done
fi
fi
fi
if [[ $OSTYPE == darwin* ]]; then
	OSXinfo
fi
echo "Removing unneeded files:"
echo "Skip this if you want to rerun this script."
yn6="Y"
while true; do
	read -p "Do you wish to remove unneeded downloaded files [YyNn]?" yn6
	case $yn6 in
		[Yy]* ) removetmpfiles; break;;
		[Nn]* ) break;;
		* ) echo "Please answer yes or no.";;
	esac
done

