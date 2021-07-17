#!/bin/bash
#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
source $(dirname $0)/make.cnf

CURDIR=`pwd -P`

# pactl load-module module-bluetooth-discover

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CURDIR/IndieLib0/SWIG/PyBindGen
export PYTHONPATH=$PYTHONPATH:$CURDIR/IndieLib0/SWIG/PyBindGen

_MSC_VER=1600
__GNUC__=4

# https://dl.google.com/dl/android/studio/ide-zips/1.0.1/android-studio-ide-135.1641136-linux.zip

curl -O $androidurl/adt/adt-bundle-mac-x86_64-20140321.zip # does not work
curl -O $androidurl/adt/adt-bundle-linux-x86-$adt.zip

curl -O $androidurl/ndk/$NDK-darwin-x86_64.tar.bz2
curl -O $androidurl/ndk/$NDK-linux-x86.tar.bz2

# XXXXX &&
# XXXXX &&
# XXXXX &&
https://help.ubuntu.com/community/Installation/MinimalCD
# XXXXX &&
http://www.nic.funet.fi/ubuntu/dists/precise/main/installer-i386/current/images/netboot/mini.iso
#curl -O http://www.nic.funet.fi/ubuntu/dists/lucid/main/installer-i386/current/images/netboot/mini.iso
#curl -O http://www.nic.funet.fi/ubuntu/dists/precise/main/installer-amd64/current/images/netboot/mini.iso
#curl -O http://www.nic.funet.fi/ubuntu/dists/lucid/main/installer-amd64/current/images/netboot/mini.iso
# XXXXX &&
curl -O http://archive.ubuntu.com/ubuntu/dists/precise/main/installer-i386/current/images/netboot/mini.iso
#curl -O http://archive.ubuntu.com/ubuntu/dists/lucid/main/installer-i386/current/images/netboot/mini.iso
#curl -O http://archive.ubuntu.com/ubuntu/dists/precise/main/installer-amd64/current/images/netboot/mini.iso
#curl -O http://archive.ubuntu.com/ubuntu/dists/lucid/main/installer-amd64/current/images/netboot/mini.iso
# XXXXX &&
# XXXXX &&
# XXXXX &&
# curl -O http://sourceforge.net/projects/uck/files/latest/download
# XXXXX &&
# XXXXX &&
# XXXXX &&
sudo apt-get update &&
sudo apt-get upgrade &&
sudo apt-get dist-upgrade &&
#sudo apt-get install ubuntu-desktop --no-install-recommends &&
#sudo apt-get install indicator-applet-complete indicator-applet firefox xorg twm fluxbox openbox &&
sudo apt-get install gedit cvs git subversion mercurial gcc g++ build-essential mesa-common-dev libglu1-mesa-dev valgrind filezilla cmake nvidia-cg-toolkit rar libsdl1.2-dev libsdl-image1.2-dev python bison flex libtool autoconf swig mypaint pencil blender curl libjpeg-dev libpng12-dev libdevil-dev libassimp-dev libpng-dev libjpeg-dev libtiff-dev libgif-dev libtinyxml-dev libsdl2-dev libsdl2-image-dev openjdk-7-jdk mono-mcs lmms libusb-dev asunder python-dev libz-dev flashplugin-installer libcgal-dev libcgal-demo
sudo pip install PyOpenGL PyOpenGL_accelerate
sudo apt-get install libeigen3-dev libgmp-dev libgmpxx4ldbl libmpfr-dev libboost-dev libboost-thread-dev libtbb-dev python3-dev
#sudo apt-get install wine-dev wine eclipse libboost-all-dev qtcreator qtcreator-doc && 
# XXXXX &&
# XXXXX &&
# XXXXX &&
#sudo reboot h now &&
curl -O http://www.tommironty.fi/IndieLibX.zip &&
unzip -x ./IndieLibX.zip &&
# XXXXX &&
# XXXXX &&
# XXXXX &&
cp ./IndieLibX/bashrc .bashrc &&
cd ./IndieLibX &&
#make shortconfiguration &&
make fullconfiguration &&
make all &&
./run.txt

#python-opengl python-ocempgui python-pyglet python-pygame

#libjasper-dev libjpeg62-dev liblcms1-dev libmng-dev libpng12-dev libtiff4-dev libghc6-zlib-dev

#zynaddsubfx yoshimi

# grep -Rn " m_pd3dDevice->" * > dx.log
# grep -Rn " gl" * > gl.log
# find . -name "*.obj" -type f -delete

For Ubuntu Linux:

# Install standard development tools and language support...
sudo aptitude install gcc freeglut3 libgle3 python-dev python-setuptools python-virtualenv
# Now the "real" packages
sudo aptitude install python-pygame python-numpy python-imaging
# Normally you'll want to set up a "virtualenv" here and activate it *before* doing the easy_install

# For Just PyOpenGL
easy_install PyOpenGL PyOpenGL-accelerate

# Or, for the OpenGLContext testing/demo environment...
easy_install OpenGLContext-full

the recommendation there to use a virtualenv should be considered very strongly, as it allows you to isolate your OpenGL work.  Note, however, that you will often need to use the --no-site-packages flag and then link the system PIL, Pygame and Numpy into the virtualenv's lib/python2.*/site-packages directory.

For Win32, install setuptools or distribute and any dependencies you need (e.g. Numpy, PIL, see below) then:

c:\python26\scripts\easy_install.exe PyOpenGL PyOpenGL-accelerate

# or, for the full OpenGLContext testing/demo environment...
c:\python26\scripts\easy_install.exe OpenGLContext-full

