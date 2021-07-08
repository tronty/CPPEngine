rem
rem Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
rem Licensed under The GNU Lesser General Public License, version 2.1:
rem     http://opensource.org/licenses/LGPL-2.1
rem
copy Libraries\\aras-p-hlsl2glslfork-de4cd58\\tools\\bison.exe.bak Libraries\\aras-p-hlsl2glslfork-de4cd58\\tools\\bison.exe
copy Libraries\\aras-p-hlsl2glslfork-de4cd58\\tools\\flex.exe.bak Libraries\\aras-p-hlsl2glslfork-de4cd58\\tools\\flex.exe

cd Libraries\\aras-p-hlsl2glslfork-de4cd58\\hlslang\\MachineIndependent
call Libraries\\aras-p-hlsl2glslfork-de4cd58\\tools\\flex.exe hlslang.l
set BISON_SIMPLE=..\\..\\tools\\bison.simple
set BISON_HAIRY=..\\..\\tools\\bison.simple
call Libraries\\aras-p-hlsl2glslfork-de4cd58\\tools\\bison.exe -o hlslang_tab.cpp -d -t -v hlslang.y
copy hlslang_tab.hpp hlslang_tab.h
copy Gen_hlslang.cpp Gen_hlslang0.cpp
copy hlslang_tab.cpp hlslang_tab0.cpp

