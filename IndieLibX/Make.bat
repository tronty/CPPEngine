rem
rem Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
rem Licensed under The GNU Lesser General Public License, version 2.1:
rem     http://opensource.org/licenses/LGPL-2.1
rem

IF not EXIST "C:\\Python27" GOTO :PY27_INSTALL

rem cd .\\DesktopProjects
rem call C:\\Python27\\python.exe .\\install.py
rem cd ..

SET x86=""
SET mscver=16
SET ok=0
IF EXIST "C:\\Program Files (x86)\\Microsoft Visual Studio\\2025\\Community\\VC\\Auxiliary\\Build\\vcvars32.bat" (
	call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2025\\Community\\VC\\Auxiliary\\Build\\vcvars32.bat"
	SET ok=1
)
IF "%ok%"=="1" GOTO EndSymLoop
IF EXIST "C:\\Program Files (x86)\\Microsoft Visual Studio\\2023\\Community\\VC\\Auxiliary\\Build\\vcvars32.bat" (
	call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2023\\Community\\VC\\Auxiliary\\Build\\vcvars32.bat"
	SET ok=1
)
IF "%ok%"=="1" GOTO EndSymLoop
IF EXIST "C:\\Program Files (x86)\\Microsoft Visual Studio\\2021\\Community\\VC\\Auxiliary\\Build\\vcvars32.bat" (
	call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2021\\Community\\VC\\Auxiliary\\Build\\vcvars32.bat"
	SET ok=1
)
IF "%ok%"=="1" GOTO EndSymLoop
IF EXIST "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvars32.bat" (
	call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvars32.bat"
	SET ok=1
)
IF "%ok%"=="1" GOTO EndSymLoop
IF EXIST "C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\VC\\Auxiliary\\Build\\vcvars32.bat" (
	call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\VC\\Auxiliary\\Build\\vcvars32.bat"
	SET ok=1
)
IF "%ok%"=="1" GOTO EndSymLoop
IF EXIST "C:\\Program Files (x86)\\Microsoft Visual Studio\\2015\\Community\\VC\\Auxiliary\\Build\\vcvars32.bat" (
	call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2015\\Community\\VC\\Auxiliary\\Build\\vcvars32.bat"
	SET ok=1
)
IF "%ok%"=="1" GOTO EndSymLoop
IF EXIST "C:\\Program Files (x86)\\Microsoft Visual Studio\\2013\\Community\\VC\\Auxiliary\\Build\\vcvars32.bat" (
	call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2013\\Community\\VC\\Auxiliary\\Build\\vcvars32.bat"
	SET ok=1
)
IF "%ok%"=="1" GOTO EndSymLoop
IF EXIST "C:\\Program Files (x86)\\Microsoft Visual Studio\\2012\\Community\\VC\\Auxiliary\\Build\\vcvars32.bat" (
	call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2012\\Community\\VC\\Auxiliary\\Build\\vcvars32.bat"
	SET ok=1
)
IF "%ok%"=="1" GOTO EndSymLoop
IF EXIST "C:\\Program Files (x86)\\Microsoft Visual Studio\\2010\\Community\\VC\\Auxiliary\\Build\\vcvars32.bat" (
	call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2010\\Community\\VC\\Auxiliary\\Build\\vcvars32.bat"
	SET ok=1
)
IF "%ok%"=="1" GOTO EndSymLoop
SET mscver=9
:SymLoop
IF EXIST "C:\\Program Files (x86)\\Microsoft Visual Studio %mscver%.0\\VC\\bin\\vcvars32.bat" (
	call "C:\\Program Files (x86)\\Microsoft Visual Studio %mscver%.0\\VC\\bin\\vcvars32.bat"
	SET ok=1
	SET x86=" (x86)"
)
IF "%ok%"=="1" GOTO EndSymLoop
IF EXIST "C:\\Program Files\\Microsoft Visual Studio %mscver%.0\\VC\\bin\\vcvars32.bat" (
	call "C:\\Program Files\\Microsoft Visual Studio %mscver%.0\\VC\\bin\\vcvars32.bat"
	SET ok=1
)
IF "%ok%"=="1" GOTO EndSymLoop
SET mscver+=1
IF %mscver%>16 GOTO EndSymLoop
GOTO SymLoop
:EndSymLoop

IF %ok%==0 (
        ECHO "vcvars32.bat not found."
	GOTO end
)

CALL cnf.bat

SET MAKE=nmake /F
IF "%ok%"=="1" IF "%3"=="MW" SET MAKE=make -f
SET EXT=msc
IF "%3"=="MW" SET EXT=mw

IF "%1"=="RadeonTerrainDemo" GOTO RadeonTerrainDemo
IF "%1"=="NatureScene" GOTO NatureScene
IF "%1"=="ParticleVS" GOTO ParticleVS
IF "%1"=="Particles" GOTO Particles
IF "%1"=="otl-nightly" GOTO otl-nightly
IF "%1"=="vulkan" GOTO vulkan
IF "%1"=="VULKAN" GOTO VULKAN
IF "%1"=="ivbuild" GOTO ivbuild
IF "%1"=="ivclean" GOTO ivclean
IF "%1"=="ivtest" GOTO ivtest

SET USERPROFILE=%USERPROFILE%

SET solution="Test"
SET eplatform="WIN"
SET makesystem="NMAKE"
SET erenderer="D3D11"
REM SET erenderer="GLCG1_1"
SET erenderer="GLSL1_1"
REM SET erenderer="VULKAN"
SET Projects_py_return_value="0"

IF "%1"=="help" GOTO help
IF "%1"=="clean" GOTO clean
IF "%1"=="" GOTO clean

IF not "%1"=="" SET solution="%1"
IF not "%2"=="" SET eplatform="%2"
IF not "%3"=="" SET makesystem="%3"
IF not "%4"=="" SET erenderer="%4"
SET g_renderer="%erenderer%"
rem IF "%3"=="MW" GOTO mingw
IF "%3"=="MW" GOTO mingwsln

rem SET platforms3=NDK NMAKE VC MW PRO CMake
rem IF not %makesystem% in (%platforms3%) GOTO usagent
GOTO all

:PY27_INSTALL
	ECHO "Could not find the Python, install C:\\Python27 and add it to the path"
	GOTO end

:help
	ECHO "Edit the xml-element <Renderer Value=\"GLSL1_1\" /> to eg. <Renderer Value=\"D3D11\" /> to change the renderer (In the file $HOME/IndieLib_resources/Framework3/Framework3.xml)."
	ECHO ""
	ECHO "Usage: %0 -> Removes Temporary Files."
	REM ECHO "Usage: %0 [< solutionname >] [<LNX|NDK|MW|OSX|iOS|WIN>] [<GNUMAKE|NDK|NMAKE|VC|XCODE>] [<GLCG1_1|GLSL1_1|GLSL4_5|GLES2|GLES3_2|D3D9|D3D10|D3D11|D3D12|VULKAN|METAL>] -> Builds an solution.";
	ECHO "Usage: %0 [< solutionname >] [<LNX|NDK|MW|OSX|iOS|WIN>] [<GNUMAKE|NDK|NMAKE|VC|XCODE>] -> Builds an solution.";

	ECHO "Example: %0 Test WIN VC";

	REM ECHO "Usage: %0 buildlibrary ProjectName [< ${projects} >] -> Builds an single project.";
	ECHO ""
	ECHO "Usage: %0 AllLibs WIN VC       	  -> Generates C++ static library .sln and .prj files for VC."
	ECHO "Usage: %0 Test WIN VC               -> Generates C++ Test Applications .sln and .prj files for VC."
	ECHO "Usage: %0 SampleApps WIN VC         -> Generates C++ Sample Applications .sln and .prj files for VC."
	ECHO "Usage: %0 TerrainApps WIN VC        -> Generates C++ Terrain Applications .sln and .prj files for VC."
	ECHO "Usage: %0 IndielibApps WIN VC       -> Generates C++ Indielib Applications .sln and .prj files for VC."
	ECHO "Usage: %0 TestApps WIN VC           -> Generates C++ TestApps Applications .sln and .prj files for VC."
	ECHO ""
	ECHO "Usage: %0 RadeonTerrainDemo -> Runs original RadeonTerrainDemo.exe"
	ECHO "Usage: %0 NatureScene       -> Runs original NatureScene.exe"
	ECHO "Usage: %0 Particles         -> Runs original Particles.exe"
	ECHO "Usage: %0 ParticleVS        -> Runs original ParticleVS.exe"
	GOTO end

:all
	del /S /Q ..\\tmp
	rmdir /S /Q ..\\tmp
	mkdir ..\\tmp
	rem copy "Dependencies\\msvcp90d.dll.bak" "bin\\msvcp90d.dll"
	copy "Dependencies\\OPENAL32.dll.bak" "bin\\OPENAL32.dll"
	rem copy "Dependencies\\OPENGL32.dll.bak" "bin\\OPENGL32.dll"
	rem copy "Dependencies\\GLU32.dll.bak" "bin\\GLU32.dll"
	rem copy "Dependencies\\glut32.dll.bak" "bin\\glut32.dll"
	rem copy "Dependencies\\cg.dll.bak" "bin\\cg.dll"
	rem copy "Dependencies\\cgGL.dll.bak" "bin\\cgGL.dll"
	rem copy "Dependencies\\cgD3D8.dll.bak" "bin\\cgD3D8.dll"
	rem copy "Dependencies\\cgD3D9.dll.bak" "bin\\cgD3D9.dll"
	rem copy "Dependencies\\cgD3D10.dll.bak" "bin\\cgD3D10.dll"
	rem copy "Dependencies\\cgD3D11.dll.bak" "bin\\cgD3D11.dll"
	rem copy "Dependencies\\D3DCompiler_43.dll.bak" "bin\\D3DCompiler_43.dll"
	rem copy "Dependencies\\d3dcompiler_47_x64.dll.bak" "bin\\d3dcompiler_47.dll"
	rem copy "Dependencies\\openal-soft-1.16.0-bin\\bin\\Win64\\soft_oal.dll
	copy "Dependencies\\SDL2-2.0.14\\lib\\x86\\SDL2.dll" "bin\\SDL2.dll"
	copy "Dependencies\\vulkan\\vulkan-1.dll" "bin\\vulkan-1.dll"
	cd .\\DesktopProjects
	CALL C:\\Python27\\python.exe Projects.py --solution %solution% --buildtool %makesystem% --platform %eplatform% --renderer %erenderer%
	CALL C:\\Python27\\python.exe Projects.py --solution None --buildtool MW --mainphoneproject %mainphoneproject% --package %package% --platform %eplatform% --renderer %erenderer%
	CALL C:\\Python27\\python.exe Projects.py --solution None --buildtool NMAKE --mainphoneproject %mainphoneproject% --package %package% --platform %eplatform% --renderer %erenderer%
	CALL C:\\Python27\\python.exe Projects.py --solution None --buildtool VC --mainphoneproject %mainphoneproject% --package %package% --platform %eplatform% --renderer %erenderer%
	cd ..
	rem mingw32-make -f Makefile.mw -C ./DesktopProjects/projectsX all
	rem %MAKE% .\\DesktopProjects\\projectsX\\Makefile.%EXT% all
	SET mscver=16
	REM ECHO "Usage: \'C:\\Program Files (x86)\\Microsoft Visual Studio %mscver%.0\\Common7\\IDE\\devenv.exe\' Makefile.sln"
	REM CALL "C:\\Program Files (x86)\\Microsoft Visual Studio %mscver%.0\\Common7\\IDE\\devenv.exe" Makefile.sln
	GOTO end

:total
	del /S /Q ..\\tmp
	rmdir /S /Q ..\\tmp
	mkdir ..\\tmp
	copy "Dependencies\\SDL2-2.0.14\\lib\\x86\\SDL2.dll" "bin\\SDL2.dll"
	del /S /Q .\\DesktopProjects\\projectsX\\Makefile.%EXT%
	del /S /Q .\\build.log
	rmdir .\\DesktopProjects\\projectsX
	mkdir .\\DesktopProjects\\projectsX
	cd .\\DesktopProjects
	CALL C:\\Python27\\python.exe Projects.py --solution %solution% --buildtool %makesystem% --platform %eplatform% --renderer %erenderer%
	CALL C:\\Python27\\python.exe Projects.py --solution None --buildtool VC --mainphoneproject %mainphoneproject% --package %package% --platform %eplatform% --renderer %erenderer%
	cd ..
	SET mscver=16
	ECHO "Usage: \'C:\\Program Files (x86)\\Microsoft Visual Studio %mscver%.0\\Common7\\IDE\\devenv.exe\' Makefile.sln"
	CALL "C:\\Program Files (x86)\\Microsoft Visual Studio %mscver%.0\\Common7\\IDE\\devenv.exe" Makefile.sln
	GOTO end

:vulkan_
	cd .\\DesktopProjects
	CALL C:\\Python27\\python.exe Projects.py --solution vulkan --buildtool VC    --platform WIN --renderer VULKAN
	CALL C:\\Python27\\python.exe Projects.py --solution None --buildtool MW    --platform WIN --renderer VULKAN --mainphoneproject %mainphoneproject% --package %package%
	CALL C:\\Python27\\python.exe Projects.py --solution None --buildtool NMAKE --platform WIN --renderer VULKAN --mainphoneproject %mainphoneproject% --package %package%
	CALL C:\\Python27\\python.exe Projects.py --solution None --buildtool VC    --platform WIN --renderer VULKAN --mainphoneproject %mainphoneproject% --package %package%
	cd ..
	GOTO end

:VULKAN_
	cd .\\DesktopProjects
	CALL C:\\Python27\\python.exe Projects.py --solution Test --buildtool VC    --platform WIN --renderer VULKAN
	CALL C:\\Python27\\python.exe Projects.py --solution None --buildtool MW    --platform WIN --renderer VULKAN --mainphoneproject %mainphoneproject% --package %package%
	CALL C:\\Python27\\python.exe Projects.py --solution None --buildtool NMAKE --platform WIN --renderer VULKAN --mainphoneproject %mainphoneproject% --package %package%
	CALL C:\\Python27\\python.exe Projects.py --solution None --buildtool VC    --platform WIN --renderer VULKAN --mainphoneproject %mainphoneproject% --package %package%
	cd ..
	GOTO end

:mingwsln
	cd .\\DesktopProjects
	CALL C:\\Python27\\python.exe Projects.py --solution %solution% --buildtool MW --platform %eplatform% --renderer %erenderer%
	CALL C:\\Python27\\python.exe Projects.py --buildtool MW --solution None --mainphoneproject %mainphoneproject% --package %package% --apps True --platform %eplatform% --renderer %erenderer%
	cd ..
	rem %MAKE% Makefile.%EXT% -C ./DesktopProjects/projectsX all
	GOTO end

:clean
	%MAKE% .\\DesktopProjects\\projectsX\\Makefile.%EXT% clean
	del /S /Q .\\build.log
	rmdir .\\DesktopProjects\\projectsX
	mkdir .\\DesktopProjects\\projectsX
	rmdir /S /Q .\\Libraries\\MVC\\NDK\\build
	mkdir .\\Libraries\\MVC\\NDK\\build
	DEL /S /Q *.obj
	DEL /S /Q *.o
	DEL /S /Q *.*~
	DEL /S /Q *~
	DEL /S /Q mk\\*.pyc
	DEL /S /Q scripting\\*.pyc
	DEL /S /Q Libraries\\otl_SDL
	DEL /S /Q Samples\\ParticleVS\\Chap12
	DEL /S /Q Samples\\RadeonTerrainDemo\\RadeonTerrainDemo
	DEL /S /Q Samples\\NatureScene\\NatureScene
	DEL /S /Q .\\lib\\*.*
	DEL /S /Q .\\bin\\*.*
	GOTO end

:Particles
	cscript j_unzip.vbs Samples\ParticleVS\Chap12.zip Samples\ParticleVS\Chap12
	cd Samples\ParticleVS\Chap12\Chap12\Particles
	CALL Particles.exe
	cd ..\\..\\..
	del /S /Q .\\Chap12
	rmdir /S /Q .\\Chap12
	cd ..\\..
	GOTO end

:ParticleVS
	cscript j_unzip.vbs Samples\ParticleVS\Chap12.zip Samples\ParticleVS\Chap12
	cd Samples\ParticleVS\Chap12\Chap12\ParticleVS
	CALL ParticlesVS.exe
	cd ..\\..\\..
	del /S /Q .\\Chap12
	rmdir /S /Q .\\Chap12
	cd ..\\..
	GOTO end

:RadeonTerrainDemo
	cscript j_unzip.vbs Samples\RadeonTerrainDemo\RadeonTerrainDemo.zip Samples\RadeonTerrainDemo
	cd Samples\RadeonTerrainDemo\RadeonTerrainDemo
	CALL RadeonTerrainDemo.exe
	cd ..
	del /S /Q .\\RadeonTerrainDemo
	rmdir /S /Q .\\RadeonTerrainDemo
	cd ..\\..
	GOTO end

:NatureScene
	cscript j_unzip.vbs Samples\NatureScene\NatureScene.zip Samples\NatureScene
	cd "Samples\NatureScene\NatureScene\Application Nature"
	CALL SceneGraph.exe
	cd ..\\..
	del /S /Q .\\NatureScene
	rmdir /S /Q .\\NatureScene
	cd ..\\..
	GOTO end

:otl-nightly
	cscript j_unzip.vbs Libraries\otl_SDL.zip Libraries\otl_SDL
	CALL %0 otl_SDL
	CALL run.bat
	del /S /Q .\\Libraries\\otl_SDL
	rmdir /S /Q .\\Libraries\\otl_SDL
	GOTO end

:ivbuild
	%MAKE% .\\DesktopProjects\\Makefile.%EXT% all
	GOTO end

:ivclean
	%MAKE% .\\DesktopProjects\\Makefile.%EXT% clean
	GOTO end

:ivtest
	%MAKE% .\\DesktopProjects\\Makefile.%EXT% test
	GOTO end

:end

