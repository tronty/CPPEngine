from pro import *

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
	extrude(height)
	Building()

@rule
def Building():
	decompose(
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
	split(x,
		flt(1)>>DoorWall(),
		2>>split(y, 2.5>>Door(), flt(2)>>DoorWall()),
		flt(1)>>DoorWall()
	)

@rule
def Window():
	texture("MarekPlainWindow00003.jpg")

@rule	
def Door():
	texture("431px-PL20F1SzczecinPlasticDoorRed.jpg")

@rule
def SideWall():
	texture("MarekBrick004.jpg", 0.625, 0.625)

@rule
def DoorWall():
	texture("MarekBrick004.jpg", 0.625, 0.625)
