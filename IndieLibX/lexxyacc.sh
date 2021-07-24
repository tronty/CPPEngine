#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
cd ./Libraries/hlsl2glslfork/hlslang/MachineIndependent &&
flex hlslang.l &&
set BISON_SIMPLE=../../tools/bison.simple &&
set BISON_HAIRY=../../tools/bison.simple &&
bison -o hlslang_tab.cpp -d -t -v hlslang.y &&
cp -f hlslang_tab.hpp hlslang_tab.h &&
cp -f Gen_hlslang.cpp Gen_hlslang0.cpp &&
cp -f hlslang_tab.cpp hlslang_tab0.cpp

