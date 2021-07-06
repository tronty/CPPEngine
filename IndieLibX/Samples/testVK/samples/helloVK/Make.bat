rem
rem Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
rem Licensed under The GNU Lesser General Public License, version 2.1:
rem     http://opensource.org/licenses/LGPL-2.1
rem

IF not EXIST "C:\\Python27" GOTO :PY27_INSTALL

SET COMPILER_VERSION = 8
SET COMPILER_VERSION = 9.0
SET COMPILER_VERSION = 14.0
SET BINDIR="C:\\Program Files (x86)\\Microsoft Visual Studio 8\\VC\\bin"

SET MAKE=%BINDIR%\\nmake /F
SET EXT=msc

SET USERPROFILE=%USERPROFILE%

IF "%1"=="help" GOTO help
IF "%1"=="clean" GOTO clean
IF "%1"=="" GOTO clean
GOTO all

:PY27_INSTALL
	ECHO "Could not find the Python, install C:\\Python27 and add it to the path"
	GOTO end

:help
	echo ""

:all
	%MAKE% .\\Makefile.%EXT% all
	GOTO end

:clean
	%MAKE% .\\Makefile.%EXT% clean
	del /S /Q *.log
	DEL /S /Q *.obj 
	DEL /S /Q *.o 
	DEL /S /Q *.*~ 
	DEL /S /Q *~ 
	GOTO end

:end

