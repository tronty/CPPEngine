rem
rem Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
rem Licensed under The GNU Lesser General Public License, version 2.1:
rem     http://opensource.org/licenses/LGPL-2.1
rem

if "%1"=="RadeonTerrainDemo" goto RadeonTerrainDemo
if "%1"=="NatureScene" goto NatureScene
if "%1"=="ParticleVS" goto ParticleVS
if "%1"=="Particles" goto Particles
if "%1"=="otl_SDL" goto otl_SDL
if "%1"=="clean" goto clean

:all
	goto end

:clean
	del /S /Q .\\test
	rmdir /S /Q .\\test
	goto end

:otl_SDL
	rem cd .\\Libraries
	rem cscript j_unzip.vbs otl_SDL.zip test\otl_SDL
	rem cd Libraries\\otl_SDL
	rem .\\Make.bat otl_SDL
	rem .\\run.bat
	rem rmdir /S /Q .\\Libraries\\otl_SDL
	goto end

:Particles
	cscript j_unzip.vbs Chap12.zip test\Chap12
	cd test\Chap12\Chap12\Particles
	call Particles.exe
	cd ..\\..\\..\\..
	del /S /Q test
	rmdir /S /Q test
	goto end

:ParticleVS
	cscript j_unzip.vbs Chap12.zip test\Chap12
	cd test\Chap12\Chap12\ParticleVS
	call ParticlesVS.exe
	cd ..\\..\\..\\..
	del /S /Q test
	rmdir /S /Q test
	goto end

:RadeonTerrainDemo
	cscript j_unzip.vbs RadeonTerrainDemo.zip test\RadeonTerrainDemo
	cd test\RadeonTerrainDemo
	call RadeonTerrainDemo.exe
	cd ..\\..
	del /S /Q test
	rmdir /S /Q .\\test
	rem cd ..\\..
	goto end

:NatureScene
	cscript j_unzip.vbs NatureScene.zip test\NatureScene
	cd "test\NatureScene\Application Nature"
	call SceneGraph.exe
	cd ..\\..\\..
	del /S /Q test
	rmdir /S /Q .\\test
	goto end
	
:end
