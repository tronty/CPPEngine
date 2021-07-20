!!ARBfp1.0

OUTPUT output = result.color;
TEMP turb, heat, flame, t, wobble, turbCoord;

PARAM	time = program.local[0];

MAD		t, fragment.texcoord[1].z, fragment.texcoord[1].z, time.x;
SIN		wobble, t.x;

MOV		turbCoord, fragment.texcoord[0];
MAD		turbCoord.x, wobble, fragment.texcoord[1].w, turbCoord.x;

TEX		turb, turbCoord, texture[0], 3D;

SUB		heat, turb, fragment.texcoord[1].y;
MAD		heat, -fragment.texcoord[1].x, fragment.texcoord[1].x, heat;

TEX		output, heat, texture[1], 2D;

END 
