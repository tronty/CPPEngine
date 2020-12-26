rem
rem Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
rem Licensed under The GNU Lesser General Public License, version 2.1:
rem     http://opensource.org/licenses/LGPL-2.1
rem
rem DEL /S /Q *.svn
DEL /S /Q *.obj 
DEL /S /Q *.o 
DEL /S /Q *.*~ 
DEL /S /Q *~ 
DEL /S /Q mk\\*.pyc 
DEL /S /Q scripting\\*.pyc
DEL /S /Q lib\\*.*
DEL /S /Q bin\\*.*
DEL /S /Q DesktopProjects\\projectsX\\*.obj
DEL /S /Q DesktopProjects\\projectsX\\*.manifest.res
DEL /S /Q DesktopProjects\\projectsX\\*.manifest
DEL /S /Q #*.*#
DEL /S /Q *.*~~~
DEL /S /Q *.exe
DEL /S /Q *.suo
DEL /S /Q *.log
DEL /S /Q *.ilk
DEL /S /Q *.pdb
DEL /S /Q *.exe.manifest
DEL /S /Q *.user
DEL /S /Q *.ncb
DEL /S /Q *.pch
DEL /S /Q *.sbr
DEL /S /Q *.bsc
DEL /S /Q *.idb
DEL /S /Q *.dep
DEL /S /Q BuildLog.htm
DEL /S /Q *.exe.intermediate.manifest
COPY Libraries\\Framework3\\aras-p-hlsl2glslfork-de4cd58\\tools\\bison.exe.bak Libraries\\Framework3\\aras-p-hlsl2glslfork-de4cd58\\tools\\bison.exe
COPY Libraries\\Framework3\\aras-p-hlsl2glslfork-de4cd58\\tools\\flex.exe.bak Libraries\\Framework3\\aras-p-hlsl2glslfork-de4cd58\\tools\\flex.exe
DEL /S /Q .\\DesktopProjects\\projectsX\\*.*
DEL /S /Q .\\Libraries\\MVC\\NDK\\build\\*.*
DEL /S /Q .\\Libraries\\SDL2-2.0.10\\build\\*.*

