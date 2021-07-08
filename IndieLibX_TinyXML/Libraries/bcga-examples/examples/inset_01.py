from pro import *
from assets.textures import *

tileWidth = param(3)

def init():
    global numParts, partHeight
    numParts = 3
    partHeight = 30

@rule
def Begin():
    init()
    rectangle(35, 15, replace=True)
    extrude(partHeight,
        front>>split(y,
            3>>GroundFloor(),
            repeat(flt(2.7)>>Floor())
        ),
        side>>split(y,
            3>>Floor(),
            repeat(flt(2.7)>>Floor())
        ),
        top>>Section()
    )

@rule
def Section():
    global numParts, partHeight
    
    numParts -= 1
    
    if numParts:
        partHeight -= 5
        
        inset(2,
            cap>>extrude(partHeight,
                side>>split(y,
                    3>>Floor(),
                    repeat(flt(2.7)>>Floor())
                ),
                top>>Section()
            ),
            side>>inset(
                0,
                1>>texture(brickDark),
                0,
                1>>texture(brickDark),
                cap>>texture(roofTilesAnthracite)
            ),
            height=5
        )
    else:
        hip_roof(60, face>>texture(roofTilesAnthracite))

@rule
def GroundFloor():
    texture(brickGrey)
    split(x,
        flt()>>GroundFloor2(False),
        tileWidth>>EntranceTile(),
        flt()>>GroundFloor2(True)
    )

@rule
def GroundFloor2(reverse):
    split(x,
        1>>SideWall(),
        repeat(flt(tileWidth)>>Tile()),
        reverse = reverse
    )

@rule
def Floor():
    texture(brickRed)
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
    texture(windowSimple)

@rule    
def Door():
    texture(doorPlasticRed)

@rule
def SideWall():
    texture(brickDark)

@rule
def DoorWall():
    texture(brickDark)
