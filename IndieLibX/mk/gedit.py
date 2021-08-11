import os

v1=['IndieLib_Blend','IndieLib_Transitions','IndieLib_ParallaxScroll','IndieLib_Fading','IndieLib_TankEnemiesParallax','IndieLib_TankEnemies','IndieLib_TankFollowsMouseFires','IndieLib_Animation_Collision','IndieLib_FloatingText']

v2=['a_04_IND_Animation','b_04_Several_ViewPorts','b_01_IND_Surface_Grids','a_09_IND_3dMesh','a_07_IND_Input','a_03_IND_Image','c_02_Rabbits_BenchMark','b_02_Blitting_2d_Directly','a_11_Animated_Tile_Scrolling','a_05_IND_Font','a_02_IND_Surface','b_03_Blitting_3d_Directly','a_15_Parallax_Scrolling','a_01_Installing','a_06_Primitives','a_08_Collisions','a_16_IND_Timer','c_01_Alien_BenchMark','a_10_IND_Light','a_14_3d_Camera','a_13_2d_Camera']

cmd='gedit'
for i in v1:
	cmd+=' ../Libraries/IndieLib/IndieLibSamples/'+i+'/src/main.cpp'
for i in v2:
	cmd+=' ../Libraries/IndieLib/IndieLib_SDK-1.03/tutorials/source/'+i+'/common/Main_vc2008.cpp'

os.system(cmd)

