#!/bin/bash
#
# Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
source ../../make.cnf

filename=./SimpleTexture

if [ -n "$1" ]; then
	filename=$1
fi

hlslparser -glsl -vs ${filename}.vsh main >glsl.vsh
hlslparser -glsl -fs ${filename}.fsh main >glsl.fsh
hlslparser -hlsl -vs ${filename}.vsh main >hlsl.vsh
hlslparser -hlsl -fs ${filename}.fsh main >hlsl.fsh
hlslparser -legacyhlsl -vs ${filename}.vsh main >legacyhlsl.vsh
hlslparser -legacyhlsl -fs ${filename}.fsh main >legacyhlsl.fsh
hlslparser -metal -vs ${filename}.vsh main >metal.vsh
hlslparser -metal -fs ${filename}.fsh main >metal.fsh

