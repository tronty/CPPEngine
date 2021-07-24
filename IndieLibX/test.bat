rem
rem Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
rem Licensed under The GNU Lesser General Public License, version 2.1:
rem     http://opensource.org/licenses/LGPL-2.1
rem
rem call Make.bat clean
rem call Make.bat AllLibs
rem copy C:\\IndieLibX\\lib\\*.lib %USERPROFILE%\\Documents\\lib
call Make.bat clean
call Make.bat TestX
call run.bat
call Make.bat clean
call Make.bat Test
call run.bat
call Make.bat clean
call Make.bat SampleApps
call run.bat
call Make.bat clean
call Make.bat TerrainApps
call run.bat
call Make.bat clean
call Make.bat IndieLibApps
call run.bat
call Make.bat clean
rem cd .\\scripting
rem make all
rem make clean
rem cd ..
rem call Make.bat Test NDK
rem call Make.bat clean

