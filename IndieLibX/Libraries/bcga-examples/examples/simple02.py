from pro import *

# Same as simple01.py but with extruded windows and doors

height = param(random(20, 40))
groundFloorHeight = param(4)
floorHeight = param(3.5)
tileWidth = param(3)
wallColor = param("#fefefe")
roofColor = "#eeeeee"

# All textures used in this prokitektura rule set are in public domain
# The textures were taken from http://wiki.openstreetmap.org/wiki/Texture_Library

@rule
def Begin():
	extrude(height,
		front>>FrontFacade(),
		side>>SideFacade(),
		top>>Roof()
	)

@rule
def FrontFacade():
	texture("MarekSeamlessBrick003.jpg", 0.5, 0.5)
	split(y,
		groundFloorHeight>>GroundFloor(),
		repeat(flt(floorHeight)>>Floor())
	)

@rule
def SideFacade():
	color(wallColor)
	split(y,
		groundFloorHeight>>Floor(),
		repeat(flt(floorHeight)>>Floor())
	)

@rule
def Roof():
	color(roofColor)

@rule
def GroundFloor():
	texture("MarekBrick002.jpg", 0.5, 0.5)
	split(x,
		1>>SideWall(),
		repeat(flt(tileWidth)>>Tile()),
		flt(tileWidth)>>EntranceTile(),
		1>>SideWall()
	)

@rule
def Floor():
	split(x,
		1>>SideWall(),
		repeat(flt(tileWidth)>>Tile()),
		1>>SideWall()
	)

@rule
def Tile():
	split(x,
		flt(1),
		2>>split(y, 1, 1.5>>Window(), flt(1)),
		flt(1)
	)

@rule
def EntranceTile():
	texture("MarekBrick004.jpg", 0.625, 0.625)
	split(x,
		flt(1),
		2>>split(y, 2.5>>Door(), flt(2)),
		flt(1)
	)

@rule
def Window():
	extrude(-0.2,
		front>>texture("MarekPlainWindow00003.jpg"),
		inheritMaterialSide=True
	)

@rule	
def Door():
	extrude(-0.3,
		front>>texture("431px-PL20F1SzczecinPlasticDoorRed.jpg"),
		bottom>>delete(),
		inheritMaterialSide=True
	)

@rule
def SideWall():
	texture("MarekBrick004.jpg", 0.625, 0.625)
