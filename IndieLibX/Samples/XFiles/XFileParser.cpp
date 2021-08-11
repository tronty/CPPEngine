const char* filename[] =
{
"/AdvAniCh12/Shadow.X",
"/AdvAniCh12/chopper.X",
"/AdvAniCh12/Rotor.X",
"/Lightning/chain_target.X",
"/Lightning/scene.X",
"/triangle.x",
"/ColoredCube.x",
"/Sparkles/cat.x",
"/GLEssentials/Assets/demon.x",
"/ProceduralBuilding3.x",
"/DXJune2010/Dwarf/Dwarf.x",
"/DXJune2010/Dwarf/DwarfWithEffectInstance.x",
"/DXJune2010/trees/skybox2.x",
"/20-Explosions/Ch20p1_SimpleExplosion/Ch20p1_object.x",
"/20-Explosions/Ch20p3_ComplexExplosion/Ch20p3_object.x",
"/20-Explosions/Ch20p2_ExplosionCluster/Ch20p2_object.x",
"/dx9sdk/sphere2.x",
"/dx9sdk/teapot.x",
"/dx9sdk/cube.x",
"/dx9sdk/sphere.x",
"/dx9sdk/tiny.x",
"/dx9sdk/tiger.x",
"/ProceduralBuilding2.x",
"/turtle.x",
"/Smoke/Gargoyle/gargoyle.x",
"/ProceduralBuilding4.x",
"/AdvAniCh12/backdrop.x",
"/TexturedCube.x",
"/CantorDust3D5.x",
"/ProceduralBuilding3_castle2.x",
"/trex_dx/dino_videogame2.x",
"/trex_dx/dino_videogame.x",
"/Billboard/seafloor.x",
"/Billboard/skybox2.x",
"/15_Ocean_Water/skybox.x",
"/skeleton/skeleton.bin.x",
"/skeleton/skeleton.x",
"/SimpleTiling.x",
"/ProceduralBuilding.x",
"/assimp--1.0.412-sdk/dwarf.x",
"/assimp--1.0.412-sdk/test.x",
"/Orcs/Bonfire_wOrcsc
};


int ApplicationLogic()
{
	c_grammar g;
    if (2 == argc)
        //parse(g, argv[1]);
	parse(g, prog1);
    else
        LOG_PRINT("No filename given\n");

    return 0;
}
