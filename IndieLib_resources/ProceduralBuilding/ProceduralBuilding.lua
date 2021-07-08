--
-- Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
-- Licensed under The GNU Lesser General Public License, version 2.1:
--     http://opensource.org/licenses/LGPL-2.1
--

textures ="	/test.bmp: \
		/textures/ATI_SDK/512px-brick.png: \
		/textures/ATI_SDK/1024px-brick.png: \
		/textures/ATI_SDK/1536px-brick.png: \
		/textures/ATI_SDK/2048px-brick.png: \
		/TextureLibrary/128px-Frankfurter_Pfanne_rot.png: \
		/TextureLibrary/640px-Frankfurter_Pfanne_rot.png: \
		/TextureLibrary/Finland/Texture_Library_Finland-OpenStreetMap_Wiki_tiedostot/128px-FI20F2_OdlWoodWindow0001.jpg: \
		/AdvAniCh12/window.bmp: \
		/RadeonTerrainDemo/CastleDoor.bmp: \
		/Orcs/Black.png: \
		/RadeonTerrainDemo/CastleWindow256px.png: \
		/ViewportProjectionContent/redtexture.png: \
		/textures/NVIDIA_Corporation/2d/relief_rockwall.jpg: \
		/textures/ATI_SDK/mortarjoint.png: \
		/textures/ATI_SDK/redbrick.png: \
		/RadeonTerrainDemo/Castlebase.bmp: \
		/NatureScene/Terrain/dirt.png: \
		/textures/texturelib.com/pavement_textures-Texturelib_tiedostot/brick_pavement_0128_01_thumb.jpg: \
		/RadeonTerrainDemo/CastleWindow256px2.png: \
		/17_Racing_Track/road.jpg: \
		/lSystem/brick.bmp: \
		/lSystem/brick.jpg: \
        /bcga-examples/431px-PL20F1SzczecinPlasticDoorRed.jpg: \
        /bcga-examples/Fabi2-holz1.jpg: \
        /bcga-examples/Flachdachpfanne_anthrazit.jpg: \
        /bcga-examples/MarekBrick002.jpg: \
        /bcga-examples/MarekBrick004.jpg: \
        /bcga-examples/MarekPlainWindow00003.jpg: \
        /bcga-examples/MarekSeamlessBrick003.jpg";

symbolsF = {

f02=0.2;
f06=0.6;
f08=0.8;
f12=1.2;
f033=0.33;

}

symbols = {

Triangle="	Texture(0) \
		TexCoord(0.5, 1)Vertex( 0, 1, 0) \
		TexCoord(  0, 0)Vertex(-1,-1, 0) \
		TexCoord(  1, 0)Vertex( 1,-1, 0)";

Triangle2="	Texture(13) \
		TexCoord(0.5, 1)Vertex( 0, 1, 0) \
		TexCoord(  0, 0)Vertex(-1,-1, 0) \
		TexCoord(  1, 0)Vertex( 1,-1, 0)";
	
LowerTriangle="	TexCoord(0, 0)Vertex(-1,-1, 0) \
		TexCoord(1, 0)Vertex( 1,-1, 0) \
		TexCoord(0, 1)Vertex(-1, 1, 0)";

HigherTriangle="TexCoord(0, 1)Vertex(-1, 1, 0) \
		TexCoord(1, 1)Vertex( 1, 1, 0) \
		TexCoord(1, 0)Vertex( 1,-1, 0)";

Quad="	TexCoord(0, 0)Vertex(-1, 1, 0) \
	TexCoord(1, 0)Vertex( 1, 1, 0) \
	TexCoord(1, 1)Vertex( 1,-1, 0) \
	TexCoord(0, 1)Vertex(-1,-1, 0) \
	TexCoord(1, 1)Vertex( 1,-1, 0) \
	TexCoord(0, 0)Vertex(-1, 1, 0)";

BlackQuad="	Texture(-1)Color(0,0,0) \
		TexCoord(0, 0)Vertex(-1, 1, 0) \
		TexCoord(1, 0)Vertex( 1, 1, 0) \
		TexCoord(1, 1)Vertex( 1,-1, 0) \
		TexCoord(0, 1)Vertex(-1,-1, 0) \
		TexCoord(1, 1)Vertex( 1,-1, 0) \
		TexCoord(0, 0)Vertex(-1, 1, 0)";

WhiteQuad="	Texture(-1)Color(1,1,1) \
		TexCoord(0, 0)Vertex(-1, 1, 0) \
		TexCoord(1, 0)Vertex( 1, 1, 0) \
		TexCoord(1, 1)Vertex( 1,-1, 0) \
		TexCoord(0, 1)Vertex(-1,-1, 0) \
		TexCoord(1, 1)Vertex( 1,-1, 0) \
		TexCoord(0, 0)Vertex(-1, 1, 0)";

ridgeRoof="	Texture(5) \
		TexCoord(0, 0)Vertex(-f06, 0, f02) \
		TexCoord(1, 0)Vertex( f06, 0, f02) \
		TexCoord(1, 1)Vertex( f06,-f02, 0) \
		TexCoord(0, 1)Vertex(-f06,-f02, 0) \
		TexCoord(1, 1)Vertex( f06,-f02, 0) \
		TexCoord(0, 0)Vertex(-f06, 0, f02)\
						\
		TexCoord(0, 0)Vertex(-f06, f02, 0) \
		TexCoord(1, 0)Vertex( f06, f02, 0) \
		TexCoord(1, 1)Vertex( f06, 0, f02) \
		TexCoord(0, 1)Vertex(-f06, 0, f02) \
		TexCoord(1, 1)Vertex( f06, 0, f02) \
		TexCoord(0, 0)Vertex(-f06, f02, 0)";

ridgeRoof2="	Texture(5)R(-90,-90,0) \
		TexCoord(0, 0)Vertex(-1, 0, 1) \
		TexCoord(1, 0)Vertex( 1, 0, 1) \
		TexCoord(1, 1)Vertex( 1,-1, 0) \
		TexCoord(0, 1)Vertex(-1,-1, 0) \
		TexCoord(1, 1)Vertex( 1,-1, 0) \
		TexCoord(0, 0)Vertex(-1, 0, 1)\
						\
		TexCoord(0, 0)Vertex(-1, 1, 0) \
		TexCoord(1, 0)Vertex( 1, 1, 0) \
		TexCoord(1, 1)Vertex( 1, 0, 1) \
		TexCoord(0, 1)Vertex(-1, 0, 1) \
		TexCoord(1, 1)Vertex( 1, 0, 1) \
		TexCoord(0, 0)Vertex(-1, 1, 0) \
						\
		Texture(13)R( 180, 0,0) \
		TexCoord(0.5, 1)Vertex( 0, 1, 1) \
		TexCoord(  0, 0)Vertex(-1, 0, 1) \
		TexCoord(  1, 0)Vertex( 1, 0, 1) \
						\
		Texture(13)R(-180,0,0) \
		TexCoord(0.5, 1)Vertex( 0, 1, -1) \
		TexCoord(  0, 0)Vertex(-1, 0, -1) \
		TexCoord(  1, 0)Vertex( 1, 0, -1) ";

Floor="		Texture(10)Color(0, 0, 0) \
		TexCoord(0, 1)Vertex(-1, 1, 0) \
		TexCoord(1, 1)Vertex( 1, 1, 0) \
		TexCoord(1, 0)Vertex( 1,-1, 0) \
		TexCoord(0, 0)Vertex(-1,-1, 0) \
		TexCoord(1, 0)Vertex( 1,-1, 0) \
		TexCoord(0, 1)Vertex(-1, 1, 0)";

Box="	Texture(0)Color(0, 1, 0) \
	R(  0,  0,0)S(1,1,1)T( 0, 0, 1)I(Quad) \
	R(  0,  0,0)S(1,1,1)T( 0, 0,-1)I(Quad) \
	R( 90,  0,0)S(1,1,1)T( 1, 0, 0)I(Quad) \
	R(-90,  0,0)S(1,1,1)T(-1, 0, 0)I(Quad) \
	R(  0, 90,0)S(1,1,1)T( 0, 1, 0)I(Quad) \
	R(  0,-90,0)S(1,1,1)T( 0,-1, 0)I(Quad)";

Box2="	Texture(0)Color(0, 1, 0) \
	R(  0,  0,0)S(1,1,1)T( 0, 0, 1)I(Quad) \
	R(  0,  0,0)S(1,1,1)T( 0, 0,-1)I(Quad) \
	R( 90,  0,0)S(1,1,1)T( 1, 0, 0)I(Quad) \
	R(-90,  0,0)S(1,1,1)T(-1, 0, 0)I(Quad)";

Prism="	Texture(5) \
	TexCoord(0.5, 1)Vertex( 0, 1, 0) \
	TexCoord(  0, 0)Vertex(-1,-1, 1) \
	TexCoord(  1, 0)Vertex( 1,-1, 1) \
	TexCoord(0.5, 1)Vertex( 0, 1, 0) \
	TexCoord(  1, 0)Vertex( 1,-1, 1) \
	TexCoord(  1, 0)Vertex( 1,-1,-1) \
	TexCoord(0.5, 1)Vertex( 0, 1, 0) \
	TexCoord(  1, 0)Vertex( 1,-1,-1) \
	TexCoord(  0, 0)Vertex(-1,-1,-1) \
	TexCoord(0.5, 1)Vertex( 0, 1, 0) \
	TexCoord(  0, 0)Vertex(-1,-1,-1) \
	TexCoord(  0, 0)Vertex(-1,-1, 1)";

WallEmpty="Split(X,[50%:50%],[Wall1:Empty])";
WallEmpty30="Repeat(X, 3, WallEmpty)";

iron_grid ="Repeat(X, 10, barX)T(0, 0.001, 0)Repeat(Z, 10, barZ)";

iron_grid2="Repeat(X, 10, barX)Repeat(Z, 10, barZ)";
barZ="SizeZ(0.1)SizeY(0.1)I(Box)";
barX="SizeX(0.1)SizeY(0.1)I(Box)";

barZ1="	Texture(13) \
	R(  0,  0,0)S(1,1,1)T( 0, 0, 0.01)I(Quad) \
	R(  0,  0,0)S(1,1,1)T( 0, 0,-0.01)I(Quad) \
	R( 90,  0,0)S(1,1,1)T( 1, 0,    0)I(Quad) \
	R(-90,  0,0)S(1,1,1)T(-1, 0,    0)I(Quad) \
	R(  0, 90,0)S(1,1,1)T( 0, 0.01, 0)I(Quad) \
	R(  0,-90,0)S(1,1,1)T( 0,-0.01, 0)I(Quad)";

barX1="	Texture(13) \
	R(  0,  0,0)S(1,1,1)T( 0,    0, 1)I(Quad) \
	R(  0,  0,0)S(1,1,1)T( 0,    0,-1)I(Quad) \
	R( 90,  0,0)S(1,1,1)T( 0.01, 0, 0)I(Quad) \
	R(-90,  0,0)S(1,1,1)T(-0.01, 0, 0)I(Quad) \
	R(  0, 90,0)S(1,1,1)T( 0, 0.01, 0)I(Quad) \
	R(  0,-90,0)S(1,1,1)T( 0,-0.01, 0)I(Quad)";

window_pillar_irr_left="Repeat(Z, window pillar dim, stone_small_brick_pillar_left, 0.5)";

stone_small_brick_pillar_left="ResizeAligned(-X, prnd(1, f06)ResizeAligned(-Y, prnd(1, f06)T(rnd(0.01, 1), rnd(0.01, 1), 0)I(stone)";

Wall1 ="Texture(1) Color(1,0,0)I(Quad)";
Wall11="Texture(11)Color(1,0,0)I(Quad)";

Window="Texture(7)Color(0,0,0)I(Quad)";

Door="Texture(9)Color(0,0,0)I(Quad)";

Wall33YW="Split(Y,[33%:34%:33%],[Wall1:Window:Wall1])";
Wall33Y="Split(Y,[33%:34%:33%],[Wall1:Wall1:Wall1])";
Wall33="Split(X,[33%:34%:33%],[Wall33Y:Wall33YW:Wall33Y])";

simpleRectangularTower="Split(Y,[60%:40%],[Box2:Prism])";

Cylinder="Texture(11)Shape(Cylinder)";
Cone="Texture(5)Shape(Cone)";
simpleRoundTower="Split(Y,[60%:40%],[Cylinder:Cone])";

vertical_window_wall="Split(Y,[40%:40%:20%],[Wall1:Window:Wall1])";

vertical_door_wall="Split(Y,[80%:20%],[Door:Wall1])";

wall_window_wall="Split(X,[30%:40%:30%],[Wall1:vertical_window_wall:Wall1])";

wall_door_wall="Split(X,[30%:40%:30%],[Wall1:vertical_door_wall:Wall1])";

simpleHouseWall="Repeat(X, 2, wall_window_wall)";

simpleHouseBody1="Align(SIDE_FACE_X, 100, wall_door_wall)";
simpleHouseBody2="Align(SIDE_FACE_X, 100, wall_window_wall)";

simpleHouse="	Align(SIDE_FACE_X,  100, wall_window_wall) \
		Align(SIDE_FACE_Y,  100, wall_door_wall) \
		Align(SIDE_FACE_Z,  100, ridgeRoof2) \
		Align(SIDE_FACE_NX, 100, wall_window_wall) \
		Align(SIDE_FACE_NY, 100, wall_window_wall) \
		Align(SIDE_FACE_NZ, 100, Floor)";

castleTowerYS="Split(Y,[33%:34%:33%],[Wall33:Wall33:Wall33])";
castleWallYS="Split(Y,[33%:34%:33%],[Wall33:Wall33:Empty])";
castleWallYS2="Split(Y,[33%:34%:33%],[Empty:Empty:Wall33])";
castlePortYS="Split(Y,[33%:34%:33%],[Door:Wall33:Empty])";

castleWallXSI="Split(X,[20%:20%:20%:20%:20%],[castleWallYS2:castleWallYS:castleWallYS:castleWallYS:castleWallYS2])";
castlePortXSI="Split(X,[20%:20%:20%:20%:20%],[castleWallYS2:castleWallYS:castlePortYS:castleWallYS:castleWallYS2])";

castleWallXS="Split(X,[20%:20%:20%:20%:20%],[castleTowerYS:castleWallYS:castleWallYS:castleWallYS:castleTowerYS])";
castlePortXS="Split(X,[20%:20%:20%:20%:20%],[castleTowerYS:castleWallYS:castlePortYS:castleWallYS:castleTowerYS])";

castleInnerWall="	Align(SIDE_FACE_X,  60%, castlePortXSI) \
			Align(SIDE_FACE_Y,  60%, castleWallXSI) \
			Align(SIDE_FACE_NX, 60%, castleWallXSI) \
			Align(SIDE_FACE_NY, 60%, castleWallXSI)";

castleOuterWall="	Align(SIDE_FACE_X,  100%, castlePortXS) \
			Align(SIDE_FACE_Y,  100%, castleWallXS) \
			Align(SIDE_FACE_NX, 100%, castleWallXS) \
			Align(SIDE_FACE_NY, 100%, castleWallXS)";

castleWallRoof1="R(180,-90,0)T( 0,f033, f08)I(ridgeRoof)";
castleWallRoof2="R(180,-90,0)T( 0,f033,-f08)I(ridgeRoof)";
castleWallRoof3="R( 90,-90,0)T( f08,f033, 0)I(ridgeRoof)";
castleWallRoof4="R( 90,-90,0)T(-f08,f033, 0)I(ridgeRoof)";

Prism02="	Texture(5) \
		TexCoord(0.5, 1)Vertex( 0,   f02, 0) \
		TexCoord(  0, 0)Vertex(-f02,-f02, f02) \
		TexCoord(  1, 0)Vertex( f02,-f02, f02) \
		TexCoord(0.5, 1)Vertex( 0,   f02, 0) \
		TexCoord(  1, 0)Vertex( f02,-f02, f02) \
		TexCoord(  1, 0)Vertex( f02,-f02,-f02) \
		TexCoord(0.5, 1)Vertex( 0,   f02, 0) \
		TexCoord(  1, 0)Vertex( f02,-f02,-f02) \
		TexCoord(  0, 0)Vertex(-f02,-f02,-f02) \
		TexCoord(0.5, 1)Vertex( 0,   f02, 0) \
		TexCoord(  0, 0)Vertex(-f02,-f02,-f02) \
		TexCoord(  0, 0)Vertex(-f02,-f02, f02)";

castleTowerRoof1="R(0,0,0)T(-f08,f12,-f08)I(Prism02)";
castleTowerRoof2="R(0,0,0)T( f08,f12, f08)I(Prism02)";
castleTowerRoof3="R(0,0,0)T(-f08,f12, f08)I(Prism02)";
castleTowerRoof4="R(0,0,0)T( f08,f12,-f08)I(Prism02)";

chimney="	Texture(0) \
		R(  0,0,0)S(1,0.5,1)T( 0, 0,   1)I(Quad) \
		R(  0,0,0)S(1,1,  1)T( 0,-0.5,-1)I(Quad) \
		R( 90,0,0)S(1,0.5,1)T( 1, 0,   0)I(Quad) \
		R(-90,0,0)S(1,0.5,1)T(-1, 0,   0)I(Quad) \
		R( 90,0,0)S(1,0.5,1)T( 1,-1,   0)I(HigherTriangle) \
		R( 90,0,0)S(1,0.5,1)T(-1,-1,   0)I(HigherTriangle)";

castle="I(castleOuterWall)I(castleInnerWall) \
I(castleWallRoof1)I(castleWallRoof2)I(castleWallRoof3)I(castleWallRoof4) \
I(castleTowerRoof1)I(castleTowerRoof2)I(castleTowerRoof3)I(castleTowerRoof4)";

--shape3D="Shape(TexturedBox)";
--shape3D="Shape(TexturedQuad)";
--shape3D="Shape(Box)";
--shape3D="Shape(Sphere)";
----shape3D="Shape(Cone)";
----shape3D="Shape(Hemis)";
----shape3D="Shape(Torus)";
--shape3D="Shape(Octa)";
--shape3D="Shape(Tetra)";
--shape3D="Shape(Ellipsoid)";
shape3D="Shape(Cylinder)";

H11="Split(Y,[33%:34%:33%],[BlackQuad:WhiteQuad:BlackQuad])";
H12="Split(X,[33%:34%:33%],[WhiteQuad:BlackQuad:WhiteQuad])";
V1= "Split(Y,[50%:50%],[H11:H12])";

H1="Split(X,[50%:50%],[V1:V1])";
H2="Split(X,[50%:50%],[V1:V1])";
V= "Split(Y,[50%:50%],[H1:H2])";

H3="Split(X,[50%:50%],[Wall1:Empty])";

V11="Repeat(X, 3, H11)";

V2="	Align(SIDE_FACE_X,  100, V11) \
	Align(SIDE_FACE_Y,  100, V11) \
	Align(SIDE_FACE_Z,  100, V11) \
	Align(SIDE_FACE_NX, 100, V11) \
	Align(SIDE_FACE_NY, 100, V11) \
	Align(SIDE_FACE_NZ, 100, V11)";

}

--axiom="I(chimney)";
--axiom="I(simpleHouse)";
--axiom="I(iron_grid)";
axiom="I(castle)";
--axiom="I(simpleRoundTower)";
--axiom="I(Wall11)";
--axiom="I(V2)";
--axiom="I(Box)";

