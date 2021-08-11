rem
rem Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
rem Licensed under The GNU Lesser General Public License, version 2.1:
rem     http://opensource.org/licenses/LGPL-2.1
rem
CALL cnf.bat
make.bat
make.bat MeshConvert
cd bin
rem call MeshConvert.exe /xt /v /o ../../IndieLib_resources/DeferredParticles/wallchunk8.x ../../IndieLib_resources/DeferredParticles/wallchunk8.sdkmesh
rem call MeshConvert.exe /xt /v /o ../../IndieLib_resources/DeferredParticles/wallsegment.x ../../IndieLib_resources/DeferredParticles/wallsegment.sdkmesh
call MeshConvert.exe /xt /v /o ../../IndieLib_resources/MotionBlur/Warrior.x ../../IndieLib_resources/MotionBlur/Warrior.sdkmesh
cd ..
make.bat

