from pro import *
from lib.stairs import SimpleStairs as Stairs
from assets.textures import *

stairsHeight = param(7.5)
stepHeight = param(0.5)
stepWidth = param(0.3)

@rule
def Begin():
    texture(brickRed)
    extrude(14, front>>Front(), side>>Side(), inheritMaterialAll=True)

@rule
def Front():
    split(y,
        stairsHeight>>split(x,
            flt(),
            2>>Stairs(stepWidth, stepHeight, texture(brickGrey)),
            flt()
        ),
        2>>split(x,
            flt(),
            1.5>>texture(doorPlasticRed),
            flt()
        ),
        flt(),
        0.5,
        2.5>>Window()
    )

@rule
def Side():
    split(y,
        flt(),
        2.5>>Window()
    )

@rule
def Window():
    extrude(0.5,
        front>>split(y,
            flt(),
            1.5>>split(x,
                flt(),
                1.5>>texture(windowSimple),
                flt()
            ),
            flt()
        ),
        inheritMaterialAll=True
    )
