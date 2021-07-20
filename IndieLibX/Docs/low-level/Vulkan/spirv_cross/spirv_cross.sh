#!/bin/bash
#
# Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
source ./make.cnf

glslangValidator()
{
./glslangValidator.bin ../Libraries/glslang/Test/100Limits.vert
./glslangValidator.bin ../Libraries/glslang/Test/100scope.vert
./glslangValidator.bin ../Libraries/glslang/Test/110scope.vert
./glslangValidator.bin ../Libraries/glslang/Test/120.vert >glslangValidator.log
./glslangValidator.bin ../Libraries/glslang/Test/130.vert
./glslangValidator.bin ../Libraries/glslang/Test/140.vert
./glslangValidator.bin ../Libraries/glslang/Test/150.vert
./glslangValidator.bin ../Libraries/glslang/Test/300.vert
./glslangValidator.bin ../Libraries/glslang/Test/310.vert
./glslangValidator.bin ../Libraries/glslang/Test/400.vert
./glslangValidator.bin ../Libraries/glslang/Test/410.vert
./glslangValidator.bin ../Libraries/glslang/Test/420.vert
./glslangValidator.bin ../Libraries/glslang/Test/430.vert
./glslangValidator.bin ../Libraries/glslang/Test/440.vert
./glslangValidator.bin ../Libraries/glslang/Test/450.vert

./glslangValidator.bin ../Libraries/glslang/Test/100.frag
./glslangValidator.bin ../Libraries/glslang/Test/120.frag >>glslangValidator.log
./glslangValidator.bin ../Libraries/glslang/Test/130.frag
./glslangValidator.bin ../Libraries/glslang/Test/140.frag
./glslangValidator.bin ../Libraries/glslang/Test/150.frag
./glslangValidator.bin ../Libraries/glslang/Test/300.frag
./glslangValidator.bin ../Libraries/glslang/Test/310.frag
./glslangValidator.bin ../Libraries/glslang/Test/330.frag
./glslangValidator.bin ../Libraries/glslang/Test/400.frag
./glslangValidator.bin ../Libraries/glslang/Test/420.frag
./glslangValidator.bin ../Libraries/glslang/Test/430AofA.frag
./glslangValidator.bin ../Libraries/glslang/Test/440.frag
./glslangValidator.bin ../Libraries/glslang/Test/450.frag
}

#./make.sh
#./make.sh spirv_cross
cd ./bin

./glslangValidator.bin -H -V -o test.vert.spv ../Docs/spirv_cross/test.vert 
./spirv_cross.bin --version 310 --es test.vert.spv --output test.vert.GLES
./spirv_cross.bin --version 420 test.vert.spv --output test.vert.GLSL
./spirv_cross.bin --version 330 --msl test.vert.spv --output test.vert.METAL
./spirv_cross.bin --version 420 --hlsl test.vert.spv --output test.vert.HLSL

./glslangValidator.bin -H -V -o test.frag.spv ../Docs/spirv_cross/test.frag 
./spirv_cross.bin --version 310 --es test.frag.spv --output test.frag.GLES
./spirv_cross.bin --version 430 test.frag.spv --output test.frag.GLSL
./spirv_cross.bin --version 330 --msl test.frag.spv --output test.frag.METAL
./spirv_cross.bin --version 330 --hlsl test.frag.spv --output test.frag.HLSL

cd ..
#./make.sh


