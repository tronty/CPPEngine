from pro import *
from assets.textures import *

erkerDepth = param(1.5)
roofPitch = param(40)

@rule
def Begin():
    texture(brickRed)
    extrude(7,
        front>>split(y,
            rel(0.8)>>split(x,
                flt(1),
                5>>Erker(),
                flt(1),
                flt(3)>>Entrance(),
                flt(1),
                5>>Erker(),
                flt(1)
            ),
            flt()
        ),
        left>>split(y,
            rel(0.8)>>split(x,
                flt(1),
                flt(1)>>Erker(),
                flt(1)  
            ),
            flt()
        ),
        top>>hip_roof(roofPitch, 0.7,
            face>>RoofTiling(),
            fascia>>Fascia(),
            soffit>>Soffit(),
            fasciaSize=0.3
        ),
        inheritMaterialSide=True
    )
    
@rule
def Entrance():
    # pitch of the entrance roof
    p = 11
    extrude2(
        0.1, 3,
        0.2, 5,
        0.3, 6,
        middle>>Door(),
        cap1>>delete(),
        cap2>>hip_roof(
            90>>delete(), 0,   p, 0.5,   p, 0.5,   p, 0.5,   p, 0.5,   p, 0.5,   p, 0.5,   p, 0.5,
            face>>RoofTiling(),
            fascia>>Fascia(),
            soffit>>Soffit(),
            fasciaSize=0.2
        ),
        inheritMaterialSection=True
    )

@rule
def Erker():
    extrude2(
        0.3, erkerDepth,
        cap1>>delete(),
        cap2>>hip_roof(
            90>>delete(), 0,   30, 0.5,   30, 0.5,   30, 0.5,
            face>>RoofTiling(),
            fascia>>Fascia(),
            soffit>>Soffit(),
            fasciaSize=0.2
        ),
        section>>Face(),
        inheritMaterialSection=True
    )

@rule
def Face():
    split(x,
        rel(0.1),
        flt()>>split(y,
            1,
            flt()>>extrude(-0.4, front>>texture(windowSimple), inheritMaterialAll=True),
            1
        ),
        rel(0.1)
    )

@rule
def Door():
    texture(brickRed)
    split(x,
        flt(),
        2>>split(y,
            flt()>>extrude(-0.4,
                front>>texture(doorPlasticRed),
                bottom>>delete(),
                inheritMaterialAll=True
            ),
            1
        ),
        flt()
    )

@rule
def RoofTiling():
    texture(roofTilesAnthracite)

@rule
def Fascia():
    texture(woodReddish)

@rule
def Soffit():
    texture(woodReddish)