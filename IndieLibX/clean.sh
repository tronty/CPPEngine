#!/bin/bash
#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
#find . -name ".svn" -type f -delete &&
find . -name "*.obj" -type f -delete &&
find . -name "*.o" -type f -delete &&
find . -name "*.*~" -type f -delete &&
find . -name "*~" -type f -delete &&
find . -name "*.suo" -type f -delete &&
find . -name "*.log" -type f -delete &&
find . -name "*.pyc" -type f -delete &&
find . -name "\#*.*\#" -type f -delete && 
find . -name "*.*~~~" -type f -delete &&
find . -name "*.exe" -type f -delete &&
find . -name "*.ilk" -type f -delete &&
find . -name "*.pdb" -type f -delete &&
find . -name "*.exe.manifest" -type f -delete &&
find . -name "*.user" -type f -delete &&
find . -name "*.ncb" -type f -delete &&
find . -name "*.pch" -type f -delete &&
find . -name "*.sbr" -type f -delete &&
find . -name "*.bsc" -type f -delete &&
find . -name "*.idb" -type f -delete &&
find . -name "*.dep" -type f -delete &&
find . -name "BuildLog.htm" -type f -delete &&
find . -name "*.exe.intermediate.manifest" -type f -delete &&
rm -f ./lib/*.* &&
rm -f ./bin/*.* &&
rm -fr ./DesktopProjects/projectsX &&
mkdir ./DesktopProjects/projectsX &&
rm -fr ./Libraries/MVC/NDK/build &&
mkdir ./Libraries/MVC/NDK/build &&
rm -fr ./Libraries/SDL2-2.0.10/build &&
mkdir  ./Libraries/SDL2-2.0.10/build

