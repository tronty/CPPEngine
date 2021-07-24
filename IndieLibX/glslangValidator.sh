#!/bin/bash
#
# Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
source ./make.cnf

glslangValidator()
{
glslangValidator ../Libraries/glslang/Test/100Limits.vert
glslangValidator ../Libraries/glslang/Test/100scope.vert
glslangValidator ../Libraries/glslang/Test/110scope.vert
glslangValidator ../Libraries/glslang/Test/120.vert >glslangValidator.log
glslangValidator ../Libraries/glslang/Test/130.vert
glslangValidator ../Libraries/glslang/Test/140.vert
glslangValidator ../Libraries/glslang/Test/150.vert
glslangValidator ../Libraries/glslang/Test/300.vert
glslangValidator ../Libraries/glslang/Test/310.vert
glslangValidator ../Libraries/glslang/Test/400.vert
glslangValidator ../Libraries/glslang/Test/410.vert
glslangValidator ../Libraries/glslang/Test/420.vert
glslangValidator ../Libraries/glslang/Test/430.vert
glslangValidator ../Libraries/glslang/Test/440.vert
glslangValidator ../Libraries/glslang/Test/450.vert

glslangValidator ../Libraries/glslang/Test/100.frag
glslangValidator ../Libraries/glslang/Test/120.frag >>glslangValidator.log
glslangValidator ../Libraries/glslang/Test/130.frag
glslangValidator ../Libraries/glslang/Test/140.frag
glslangValidator ../Libraries/glslang/Test/150.frag
glslangValidator ../Libraries/glslang/Test/300.frag
glslangValidator ../Libraries/glslang/Test/310.frag
glslangValidator ../Libraries/glslang/Test/330.frag
glslangValidator ../Libraries/glslang/Test/400.frag
glslangValidator ../Libraries/glslang/Test/420.frag
glslangValidator ../Libraries/glslang/Test/430AofA.frag
glslangValidator ../Libraries/glslang/Test/440.frag
glslangValidator ../Libraries/glslang/Test/450.frag
}

#./make.sh
#./make.sh glslangValidator
cd ./bin

spirv_cross --version 310 --es ../Libraries/glslang/Test/120.vert --output test.GLES
spirv_cross --version 330 ../Libraries/glslang/Test/120.vert --output test.GLSL
spirv_cross --version 330 --msl ../Libraries/glslang/Test/120.vert --output test.METAL
spirv_cross --version 330 --hlsl ../Libraries/glslang/Test/120.vert --output test.HLSL

spirv_cross --version 310 --es ../Libraries/glslang/Test/120.frag --output test.GLES
spirv_cross --version 330 ../Libraries/glslang/Test/120.frag --output test.GLSL
spirv_cross --version 330 --msl ../Libraries/glslang/Test/120.frag --output test.METAL
spirv_cross --version 330 --hlsl ../Libraries/glslang/Test/120.frag --output test.HLSL

cd ..
#./make.sh


