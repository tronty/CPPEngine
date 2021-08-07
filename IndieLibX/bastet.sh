#!/bin/bash
#
# Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
source ./make.cnf
./make.sh
mv ./Libraries/BoostWorkaround ./Libraries/BoostWorkaround_
./make.sh tetris_tutorial
#cd ./bin
#./tetris_tutorial.bin
#cd ..
#./make.sh
mv ./Libraries/BoostWorkaround_ ./Libraries/BoostWorkaround


