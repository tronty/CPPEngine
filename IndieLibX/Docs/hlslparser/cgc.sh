#!/bin/bash
#
# Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
source ../../make.cnf

# cgc -profile glslv vert.cg

cgc -profile generic ../../Libraries/cgc-opensrc/vertexlight.cg
cgc -profile generic ../../Libraries/cgc-opensrc/vertexlight4.cg
cgc -profile generic ../../Libraries/cgc-opensrc/position.cg
cgc -profile generic ../../Libraries/cgc-opensrc/reflection.cg

