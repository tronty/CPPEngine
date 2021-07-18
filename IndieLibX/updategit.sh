#!/bin/bash
#
# Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
source ./make.cnf

updategit()
{
#if [ "X"=="X" ]; then
#if [ -e "../CPPEngine/.git/index" ]; then
	CURDIR0=`pwd -P`
	echo $CURDIR0
    rm -fr ../../CPPEngine/IndieLibX
    rm -fr ../../CPPEngine/IndieLib_resources
    cp -r ../IndieLibX ../../CPPEngine/.
    cp -r ../IndieLib_resources ../../CPPEngine/.
    cd ../../CPPEngine
    git add IndieLibX
    git add IndieLib_resources
    git commit -m "XXX"
    git push
    cd $CURDIR0
#fi;
#fi;
}

updatezip()
{
#if [ "X"=="X" ]; then
	CURDIR0=`pwd -P`
	echo $CURDIR0
    	zip -r IndieLib_resources.zip $CURDIR0/../IndieLib_resources
    	zip -r IndieLibX.zip $CURDIR0
    	#zip -r IndieLib.zip $CURDIR0/..
    cd $CURDIR0
#fi;
}

updategit;
#supdatezip;

