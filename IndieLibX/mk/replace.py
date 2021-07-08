import os

replacement='''#if defined(ANDROID_BUILD) && !defined(FALSE_ANDROID_BUILD)'''

def replaceStringInFile(filePath):
	"replaces all string by a regex substitution"
	tempName=filePath+'~~~'
	print filePath
	input = open(filePath)
	outtext=input.read()
	input.close()
	
	output = open(tempName,'w')
	
	outtext = outtext.replace('#if defined(ANDROID_BUILD)',replacement)
	
	output.write(outtext)
	output.close()
	
	os.unlink(filePath)
	os.rename(tempName,filePath)

#files='''../Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_01_Installing/common/Main_vc2008.cpp'''

files='''../Libraries/IndieLib/IndieLibSamples/IndieLib-TankFollowsMouseFires/src/main.cpp
../Libraries/IndieLib/IndieLibSamples/IndieLib-TankEnemiesParallax/src/main.cpp
../Libraries/IndieLib/IndieLibSamples/IndieLib-FloatingText/src/main.cpp
../Libraries/IndieLib/IndieLibSamples/IndieLib-TankEnemies/src/main.cpp
../Libraries/IndieLib/IndieLibSamples/IndieLib-Transitions/src/main.cpp
../Libraries/IndieLib/IndieLibSamples/IndieLib-ParallaxScroll/src/main.cpp
../Libraries/IndieLib/IndieLibSamples/IndieLib-Animation-Collision/src/main.cpp
../Libraries/IndieLib/IndieLibSamples/IndieLib-Blend/src/main.cpp
../Libraries/IndieLib/IndieLibSamples/IndieLib-Fading/src/main.cpp
../Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_06_Primitives/common/Main_vc2008.cpp
../Libraries/IndieLib/IndieLib_SDK/tutorials/source/b_01_IND_Surface_Grids/common/Main_vc2008.cpp
../Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_05_IND_Font/common/Main_vc2008.cpp
../Libraries/IndieLib/IndieLib_SDK/tutorials/source/c_02_Rabbits_BenchMark/common/Main_vc2008.cpp
../Libraries/IndieLib/IndieLib_SDK/tutorials/source/b_04_Several_ViewPorts/common/Main_vc2008.cpp
../Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_15_Parallax_Scrolling/common/Main_vc2008.cpp
../Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_04_IND_Animation/common/Main_vc2008.cpp
../Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_09_IND_3dMesh/common/Main_vc2008.cpp
../Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_01_Installing/common/Main_vc2008.cpp
../Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_11_Animated_Tile_Scrolling/common/Main_vc2008.cpp
../Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_16_IND_Timer/common/Main_vc2008.cpp
../Libraries/IndieLib/IndieLib_SDK/tutorials/source/b_03_Blitting_3d_Directly/common/Main_vc2008.cpp
../Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_08_Collisions/common/Main_vc2008.cpp
../Libraries/IndieLib/IndieLib_SDK/tutorials/source/c_01_Alien_BenchMark/common/Main_vc2008.cpp
../Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_13_2d_Camera/common/Main_vc2008.cpp
../Libraries/IndieLib/IndieLib_SDK/tutorials/source/b_02_Blitting_2d_Directly/common/Main_vc2008.cpp
../Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_02_IND_Surface/common/Main_vc2008.cpp
../Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_14_3d_Camera/common/Main_vc2008.cpp
../Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_10_IND_Light/common/Main_vc2008.cpp
../Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_03_IND_Image/common/Main_vc2008.cpp
../Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_07_IND_Input/common/Main_vc2008.cpp'''

c=1
for i in files.split():
	replaceStringInFile(i)


