!!ARBfp1.0

OUTPUT output = result.color;
TEMP glow, turb, y, t, mid;

PARAM glowFallOff = 0.2;
PARAM color = { 1.70, 1.48, 1.78, 0 };

TEX		turb, fragment.texcoord[0], texture[0], 3D;
MAD		turb.x, turb.x, 2, -1;

MAD_SAT	t.x, fragment.texcoord[1].y, -fragment.texcoord[1].y, 0.15;

MAD		y, turb.x, -t.x, fragment.texcoord[1].x;
ABS		y, y;

POW		glow.x, y.x, glowFallOff.x;
MAD		glow, -glow.x, color, color;
MUL		glow, glow, glow;
MUL		output, glow, glow;

END 
