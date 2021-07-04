#!/bin/bash
#
# Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
source ./make.cnf
cd ./Docs/lsys
python ./cube.py
python ./Lsystem2D.py
python ./Lsystem3D.py
cd ../..


