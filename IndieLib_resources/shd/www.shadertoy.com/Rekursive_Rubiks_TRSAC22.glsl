/*
The code started nice[citation needed], but quickly
went to shit when deadline stress increased and quick hacks were written. Also
doesn't help to be sleep deprived and experiencing vertigo from the journey
towards the party :D
Originally written in GLSL and now adapted to work with GLSL ES in ShaderToy
*/

#define shadertoy 1
#define debugmov 0 //noexport
#if shadertoy == 0
#define iTime fpar[0].x
layout (location=0) uniform vec4 fpar[2];
layout (location=2) uniform vec4 debug[2]; //noexport
#endif
#define PI 3.14159265359
#define HALFPI 1.5707963268
#define _x_ 0 /*'___' : macro name with a double underscore is reserved - unintented behavior is possible*/
#define RED 1
#define BLU 2
#define GRN 3
#define YLW 4
#define WHI 5
#define ORG 6
#define SHA 7
vec3 COLORS[8];
int i;
float gRounding, gSide, gUnit, gOffsetStuff;
const float ROUNDING = 2.5;
const float SIDE = 12. - ROUNDING;
const float UNIT = SIDE * 2. + 2. * ROUNDING;
const float SPACING = UNIT;
const vec3 off = vec3(0, SPACING, -SPACING);
const vec4 rot = vec4(0, HALFPI, -HALFPI, PI);
vec3 gHitPosition = vec3(0);
vec3 gCubeOff[26];
vec3 gCubeRot[26];
int gCubeCol0[26], gCubeCol1[26], gCubeCol2[26];
float gCubeOpacity[26];
bool gCubeHidden[26];
int gHitIndex = 0;
int gExclusiveCube = -1; // if set, only that cube will be considered.
                         //useful for consecutive map() calls for lighting after we already know what cube gets hit
#define F 0 // front
#define L 2 // left
#define R 4 // right
#define U 6 // up
#define D 8 // down
#define B 10 // back
const int gNumMovements = 12;
int gMovements[gNumMovements];
int gCurrentMovement;
float gCurrentMovementProgress = 0.;
const float MOVEMENT_TIME_SECONDS = .3;
const float HIDE_TIME_SECONDS = .2;
const float FADE_TIME_SECONDS = .4;
int gCubeHiddenOrder[25];
bool gHackFadeStuff;
bool gShaft;
float gTimeMod;

mat2 rot2(float a){float s=sin(a),c=cos(a);return mat2(c,s,-s,c);}

vec2 oneSidedCube(vec3 p, int cubeIndex)
{
	vec2 mc = vec2(length(max(abs(p) - gSide, 0.)) - gRounding, 0);
	float tc = length(max(abs(p + vec3(0., 0., gRounding + .02)) - gSide, 0.));

	return mc.x < tc ? mc : vec2(tc, float(gCubeCol0[cubeIndex]));
}

vec2 twoSidedCube(vec3 p, int cubeIndex)
{
	vec2 mc = oneSidedCube(p, cubeIndex);
	float fc = length(max(abs(p + vec3(0, -gRounding - .02, 0.)) - gSide, 0.));

	return mc.x < fc ? mc : vec2(fc, float(gCubeCol1[cubeIndex]));
}

// also includes the shaft
vec2 centerCube(vec3 p, int cubeIndex)
{
	vec2	mc = oneSidedCube(p, cubeIndex),
		sh = vec2(max(length(p.xy)-gUnit/4., length(max(abs(p-vec3(0.,0.,gUnit*.375)) - gUnit*.75, 0.))), 7);
	
	mc.x = max(mc.x, -(length(p+vec3(0.,0.,-gUnit))-gUnit*1.2));
	return mc.x < sh.x || !gShaft ? mc : sh;
}

vec2 middleCube(vec3 p, int cubeIndex)
{
	vec2 res = twoSidedCube(p, cubeIndex);
	res.x = max(res.x, -(length(p+vec3(0., 62., -62.))-80.));
	res.x = min(res.x, length(max(abs(p - vec3(0., -7., 7.)) - vec3(7., 9., 9.), 0.)));
	return res;
}

vec2 cornerCube(vec3 p, int cubeIndex)
{
	vec2 mc = twoSidedCube(p, cubeIndex);
	float sc = length(max(abs(p + vec3(-gRounding - .02, 0., 0.)) - gSide, 0.));

	if (sc < mc.x) mc = vec2(sc, float(gCubeCol2[cubeIndex]));
	vec3 cubepos = p + vec3(9., 9., -9.);
	float bit = max(length(max(abs(cubepos) - vec3(7.), 0.)), length(p+vec3(gUnit,gUnit,-gUnit))-gUnit*1.18);
	mc.x = min(mc.x, bit);
	return mc;
}

vec2 map(vec3 p)
{
	float tt = clamp(gTimeMod / 9., 0., 1.) * PI * 2.;
	p.xy *= rot2(tt * .9 + PI * 2. * .1);
	if (gTimeMod >= 9.) {
		p.xy *= rot2(PI * 2. * .1 * clamp(gTimeMod - 9., 0., 1.));
	}
	//p.zy *= rot2(tt);
	float boundingbox = length(max(abs(p) - vec3(gUnit * 2.1), 0.));
	if (boundingbox > .1) {
		return vec2(boundingbox, 0.);
	}
	vec2 res = vec2(9e9, 0.);
	for (i = 0; i < 26; i++) {
		if (gCubeHidden[i] || (gExclusiveCube != -1 && gExclusiveCube != i)) {
			continue;
		}
		vec3 offset = gCubeOff[i];
		vec2 cub;
		vec3 pa = p;
		switch (gCurrentMovement) {
		case F:
			if (offset.y == off.y) {
				pa.xz *= rot2(HALFPI * gCurrentMovementProgress);
			}
			break;
		case L:
			if (offset.x == off.z) {
				pa.yz *= rot2(HALFPI * gCurrentMovementProgress);
			}
			break;
		case R:
			if (offset.x == off.y) {
				pa.zy *= rot2(HALFPI * gCurrentMovementProgress);
			}
			break;
		case B:
			if (offset.y == off.z) {
				pa.zx *= rot2(HALFPI * gCurrentMovementProgress);
			}
			break;
		case U:
			if (offset.z == off.z) {
				pa.xy *= rot2(HALFPI * gCurrentMovementProgress);
			}
			break;
		case D:
			if (offset.z == off.y) {
				pa.yx *= rot2(HALFPI * gCurrentMovementProgress);
			}
			break;
		}
		/*
		if (offset.x == off.z) {
			pa.yz *= rot2(gTimeMod);
		}
		if (offset.x == off.y) {
			pa.yz *= rot2(-gTimeMod);
		}
		if (offset.x == off.x) {
			pa.yz *= rot2(-gTimeMod * 0.8);
		}
		if (offset.y == off.z) {
			//pa.xz *= rot2(gTimeMod);
		}
		if (
			!(offset.x == 0. && offset.y == 0.) &&
			!(offset.x == 0. && offset.z == 0.) &&
			!(offset.y == 0. && offset.z == 0.)
		) {
			offset *= sin(gTimeMod) + 2.;
		}
		*/
		pa -= offset * gOffsetStuff;
		pa.xy *= rot2(gCubeRot[i].x);
		pa.yz *= rot2(gCubeRot[i].y);
		pa.xz *= rot2(gCubeRot[i].z);
		if (gCubeCol1[i] == _x_) {
			float boundingbox = length(max(abs(pa - vec3(0., 0., gUnit * .36)) - vec3(gUnit * .55, gUnit * .55, gUnit * .9), 0.));
			cub = boundingbox < .1 ? centerCube(pa, i) : vec2(boundingbox, 0.);
		} else if (gCubeCol2[i] == _x_) {
			float boundingbox = length(max(abs(pa) - vec3(gUnit * .7), 0.));
			cub = boundingbox < .1 ? middleCube(pa, i) : vec2(boundingbox, 0.);
		} else {
			float boundingbox = length(max(abs(pa) - vec3(gUnit * .7), 0.));
			cub = boundingbox < .1 ? cornerCube(pa, i) : vec2(boundingbox, 0.);
		}
		if (cub.x < res.x) {
			res = cub;
			gHitIndex = i;
		}
	}
	return res;
}

vec3 norm(vec3 p, float dist_to_p)
{
	vec2 e = vec2(.001, 0);
	return normalize(dist_to_p - vec3(map(p-e.xyy).x, map(p-e.yxy).x, map(p-e.yyx).x));
}

// x=hit y=dist_to_p z=dist_to_ro w=material(if hit)
vec4 march(vec3 ro, vec3 rd, int maxSteps)
{
	vec4 r = vec4(0);
	for (i = 0; i < maxSteps && r.z < 350.; i++){
		gHitPosition = ro + rd * r.z;

		//p.y += 100.;
		//p.z -= 10.;
		//p.yz *= rot2(-.9);
		//p = mod(p, 30.) - 15.;
		//p.xy *= rot2(gTimeMod/2.);
		//p.yz *= rot2(gTimeMod/3.);
		//p.xz*=rot2(sin(p.z*0.2)*0.2+gTimeMod);
		vec2 m = map(gHitPosition);
		float distance = m.x;
		if (distance < .03) {
			r.x = 1.;
			r.y = distance;
			r.w = m.y;
			break;
		}
		r.z += distance;
	}
	return r;
}

vec3 colorHit(vec4 result, vec3 rd)
{
	gExclusiveCube = gHitIndex;
	vec3 shade = vec3(0);
	int material = int(result.w);
	if (0 <= material && material <= 7) {
		shade = COLORS[material];
	}
	vec3 normal = norm(gHitPosition, result.y);
	// coloring magic from https://www.shadertoy.com/view/sdVczz
	float diffuse = max(0., dot(normal, -rd));
	float fresnel = pow(1. + dot(normal, rd), 4.);
	float specular = pow(max(dot(reflect(rd, normal), -rd), 0.), 30.);

	// we could not do ambient occlusion and save map() call for more performance
	//float ambientOcc = clamp(map(gHitPosition + normal * .05).x / .05, 0., 1.);
	float ambientOcc = .9; // .9 on purpose, because it's brighter and looks slighty better

	float scat = smoothstep(0., 1., map(gHitPosition - rd * .4).x / .4); // "sub surface scattering"
	shade = mix(specular + shade * (ambientOcc + .2) * (diffuse + scat * .1), shade, fresnel);
	//shade = mix(background, shade, exp(-.002 * result.y * result.y * result.y));
	gExclusiveCube = -1;
	return shade;
}

void main()
{
	gMovements[0] = F;
	gMovements[1] = F;
	gMovements[2] = B;
	gMovements[3] = B;
	gMovements[4] = L;
	gMovements[5] = L;
	gMovements[6] = R;
	gMovements[7] = R;
	gMovements[8] = U;
	gMovements[9] = U;
	gMovements[10] = D;
	gMovements[11] = D;
	COLORS[0] = vec3(.03);
	COLORS[1] = vec3(1, 0, 0);
	COLORS[2] = vec3(0, 0, 1);
	COLORS[3] = vec3(0, 1, 0);
	COLORS[4] = vec3(1, 1, 0);
	COLORS[5] = vec3(1);
	COLORS[6] = vec3(1., .3, .0);
	COLORS[7] = vec3(.9, .9, .8);
	gCubeOff[ 0] = off.zyz;
	gCubeOff[ 1] = off.xyz;
	gCubeOff[ 2] = off.yyz;
	gCubeOff[ 3] = off.zxz;
	gCubeOff[ 4] = off.xxz;
	gCubeOff[ 5] = off.yxz;
	gCubeOff[ 6] = off.zzz;
	gCubeOff[ 7] = off.xzz;
	gCubeOff[ 8] = off.yzz;
	gCubeOff[ 9] = off.zyx;
	gCubeOff[10] = off.xyx;
	gCubeOff[11] = off.yyx;
	gCubeOff[12] = off.zxx;
	gCubeOff[13] = off.yxx;
	gCubeOff[14] = off.zzx;
	gCubeOff[15] = off.xzx;
	gCubeOff[16] = off.yzx;
	gCubeOff[17] = off.zyy;
	gCubeOff[18] = off.xyy;
	gCubeOff[19] = off.yyy;
	gCubeOff[20] = off.zxy;
	gCubeOff[21] = off.xxy;
	gCubeOff[22] = off.yxy;
	gCubeOff[23] = off.zzy;
	gCubeOff[24] = off.xzy;
	gCubeOff[25] = off.yzy;
	gCubeRot[ 0] = rot.yxx;
	gCubeRot[ 1] = rot.xxx;
	gCubeRot[ 2] = rot.xxx;
	gCubeRot[ 3] = rot.yxx;
	gCubeRot[ 4] = rot.xxx;
	gCubeRot[ 5] = rot.zxx;
	gCubeRot[ 6] = rot.wxx;
	gCubeRot[ 7] = rot.wxx;
	gCubeRot[ 8] = rot.zxx;
	gCubeRot[ 9] = rot.xxz;
	gCubeRot[10] = rot.xyx;
	gCubeRot[11] = rot.zxz;
	gCubeRot[12] = rot.yyx;
	gCubeRot[13] = rot.zyx;
	gCubeRot[14] = rot.yxz;
	gCubeRot[15] = rot.xzx;
	gCubeRot[16] = rot.wxz;
	gCubeRot[17] = rot.yyx;
	gCubeRot[18] = rot.xyx;
	gCubeRot[19] = rot.xyx;
	gCubeRot[20] = rot.yyx;
	gCubeRot[21] = rot.xwx;
	gCubeRot[22] = rot.zyx;
	gCubeRot[23] = rot.wyx;
	gCubeRot[24] = rot.xwx;
	gCubeRot[25] = rot.xwx;
	gCubeHiddenOrder[ 0] = 9;
	gCubeHiddenOrder[ 1] = 5;
	gCubeHiddenOrder[ 2] = 25;
	gCubeHiddenOrder[ 3] = 24;
	gCubeHiddenOrder[ 4] = 17;
	gCubeHiddenOrder[ 5] = 11;
	gCubeHiddenOrder[ 6] = 3;
	gCubeHiddenOrder[ 7] = 6;
	gCubeHiddenOrder[ 8] = 8;
	gCubeHiddenOrder[ 9] = 2;
	gCubeHiddenOrder[10] = 14;
	gCubeHiddenOrder[11] = 19;
	gCubeHiddenOrder[12] = 1;
	gCubeHiddenOrder[13] = 20;
	gCubeHiddenOrder[14] = 22;
	gCubeHiddenOrder[15] = 16;
	gCubeHiddenOrder[16] = 23;
	gCubeHiddenOrder[17] = 7;
	gCubeHiddenOrder[18] = 18;
	gCubeHiddenOrder[19] = 4;
	gCubeHiddenOrder[20] = 10;
	gCubeHiddenOrder[21] = 12;
	gCubeHiddenOrder[22] = 13;
	gCubeHiddenOrder[23] = 15;
	gCubeHiddenOrder[24] = 21;
	//0,
	gTimeMod = mod(iTime, 10.);
	gCubeCol0[ 0] = RED; gCubeCol1[ 0] = YLW; gCubeCol2[ 0] = BLU;
	gCubeCol0[ 1] = RED; gCubeCol1[ 1] = BLU; gCubeCol2[ 1] = _x_;
	gCubeCol0[ 2] = RED; gCubeCol1[ 2] = BLU; gCubeCol2[ 2] = WHI;
	gCubeCol0[ 3] = RED; gCubeCol1[ 3] = YLW; gCubeCol2[ 3] = _x_;
	gCubeCol0[ 4] = RED; gCubeCol1[ 4] = _x_; gCubeCol2[ 4] = _x_;
	gCubeCol0[ 5] = RED; gCubeCol1[ 5] = WHI; gCubeCol2[ 5] = _x_;
	gCubeCol0[ 6] = RED; gCubeCol1[ 6] = GRN; gCubeCol2[ 6] = YLW;
	gCubeCol0[ 7] = RED; gCubeCol1[ 7] = GRN; gCubeCol2[ 7] = _x_;
	gCubeCol0[ 8] = RED; gCubeCol1[ 8] = WHI; gCubeCol2[ 8] = GRN;
	gCubeCol0[ 9] = YLW; gCubeCol1[ 9] = BLU; gCubeCol2[ 9] = _x_;
	gCubeCol0[10] = BLU; gCubeCol1[10] = _x_; gCubeCol2[10] = _x_;
	gCubeCol0[11] = BLU; gCubeCol1[11] = WHI; gCubeCol2[11] = _x_;
	gCubeCol0[12] = YLW; gCubeCol1[12] = _x_; gCubeCol2[12] = _x_;
	gCubeCol0[13] = WHI; gCubeCol1[13] = _x_; gCubeCol2[13] = _x_;
	gCubeCol0[14] = GRN; gCubeCol1[14] = YLW; gCubeCol2[14] = _x_;
	gCubeCol0[15] = GRN; gCubeCol1[15] = _x_; gCubeCol2[15] = _x_;
	gCubeCol0[16] = WHI; gCubeCol1[16] = GRN; gCubeCol2[16] = _x_;
	gCubeCol0[17] = YLW; gCubeCol1[17] = ORG; gCubeCol2[17] = BLU;
	gCubeCol0[18] = BLU; gCubeCol1[18] = ORG; gCubeCol2[18] = _x_;
	gCubeCol0[19] = BLU; gCubeCol1[19] = ORG; gCubeCol2[19] = WHI;
	gCubeCol0[20] = YLW; gCubeCol1[20] = ORG; gCubeCol2[20] = _x_;
	gCubeCol0[21] = ORG; gCubeCol1[21] = _x_; gCubeCol2[21] = _x_;
	gCubeCol0[22] = WHI; gCubeCol1[22] = ORG; gCubeCol2[22] = _x_;
	gCubeCol0[23] = GRN; gCubeCol1[23] = ORG; gCubeCol2[23] = YLW;
	gCubeCol0[24] = ORG; gCubeCol1[24] = GRN; gCubeCol2[24] = _x_;
	gCubeCol0[25] = ORG; gCubeCol1[25] = GRN; gCubeCol2[25] = WHI;
	gCubeHidden[0] = false;
	gCubeOpacity[0] = 1.;
	gHackFadeStuff = false;
	for (i = 0; i < 26 - 1; i++) {
		int index = gCubeHiddenOrder[i];
		gCubeOpacity[index] = 1.;
		float time = gTimeMod >= 9. ? 0. : gTimeMod;
		int whatever = i >= 19 ? 21 : i + 1;
		float until = float(gNumMovements) * MOVEMENT_TIME_SECONDS + float(whatever) * HIDE_TIME_SECONDS;
		if (float(until) < time) {
			gCubeHidden[index] = true;
		} else {
			gCubeHidden[index] = false;
			if (float(until - FADE_TIME_SECONDS) < time) {
				if (i > 21) {
					gHackFadeStuff = true;
				}
				gCubeOpacity[index] = (float(until) - time) / FADE_TIME_SECONDS;
			}
		}
	}

	gCurrentMovement = -1;
	gCurrentMovementProgress = 0.;
	for (i = 0; gTimeMod < 9. && i < gNumMovements; i++) {
		float until = float(i + 1) * MOVEMENT_TIME_SECONDS;
		if (float(until) < gTimeMod) {
			int tmp;
#define _0 gCubeCol0
#define _1 gCubeCol1
#define _2 gCubeCol2
#define swap(a,b,c,d,e,f,g,h) tmp=a[b];a[b]=c[d];c[d]=e[f];e[f]=g[h];g[h]=tmp;
			switch (gMovements[i]) {
			case F:
				swap(_0, 2, _1, 0, _1, 17, _2, 19);
				swap(_1, 11, _0, 1, _0, 9, _1, 18);
				swap(_1, 19, _2, 2, _0, 0, _0, 17);
				swap(_1, 2, _2, 0, _2, 17, _0, 19);
				swap(_1, 1, _1, 9, _0, 18, _0, 11);
				break;
			case L:
				swap(_0, 0, _1, 6, _1, 23, _2, 17);
				swap(_2, 0, _0, 6, _0, 23, _1, 17);
				swap(_1, 9, _0, 3, _0, 14, _1, 20);
				swap(_2, 6, _2, 23, _0, 17, _1, 0);
				swap(_1, 3, _1, 14, _0, 20, _0, 9);
				break;
			case R:
				swap(_0, 2, _0, 19, _0, 25, _2, 8);
				swap(_1, 25, _0, 8, _1, 2, _1, 19);
				swap(_0, 5, _0, 11, _1, 22, _1, 16);
				swap(_2, 2, _2, 19, _2, 25, _1, 8);
				swap(_1, 5, _1, 11, _0, 22, _0, 16);
				break;
			case B:
				swap(_0, 8, _2, 25, _1, 23, _2, 6);
				swap(_0, 6, _1, 8, _0, 25, _2, 23);
				swap(_0, 7, _0, 16, _0, 24, _1, 14);
				swap(_2, 8, _1, 25, _0, 23, _1, 6);
				swap(_1, 7, _1, 16, _1, 24, _0, 14);
				break;
			case U:
				swap(_1, 6, _1, 0, _1, 2, _1, 8);
				swap(_2, 8, _2, 6, _2, 0, _2, 2);
				swap(_1, 7, _1, 3, _1, 1, _1, 5);
				swap(_0, 6, _0, 0, _0, 2, _0, 8);
				swap(_0, 7, _0, 3, _0, 1, _0, 5);
				break;
			case D:
				swap(_2, 17, _2, 23, _1, 25, _2, 19);
				swap(_2, 25, _0, 19, _0, 17, _0, 23);
				swap(_0, 18, _0, 20, _1, 24, _0, 22);
				swap(_1, 17, _1, 23, _0, 25, _1, 19);
				swap(_1, 18, _1, 20, _0, 24, _1, 22);
				break;
			}
		} else {
			gCurrentMovement = gMovements[i];
			gCurrentMovementProgress = 1. - (float(until) - gTimeMod) / MOVEMENT_TIME_SECONDS;
			break;
		}
	}

#if shadertoy == 1
	vec2 uv = (xlv_TEXCOORD0-.5*iResolution.xy)/iResolution.y;
#else
	vec2 uv=xlv_TEXCOORD0;uv.y/=1.77;
#endif

	vec3 ro = vec3(-80, 80, -70);
	vec3 at = vec3(0, 0, 10);

#if debugmov == 1 //noexport
	ro = debug[0].xyz; //noexport
	float vertAngle = debug[1].y/20.; //noexport
	float horzAngle = debug[1].x/20.; //noexport
	if (abs(vertAngle) < .001) { //noexport
		vertAngle = .001; //noexport
	} //noexport
	float xylen = sin(vertAngle); //noexport
	vertAngle = cos(vertAngle); //noexport
	at.x = ro.x + cos(horzAngle) * xylen; //noexport
	at.y = ro.y + sin(horzAngle) * xylen; //noexport
	at.z = ro.z + vertAngle; //noexport
#endif //noexport

	gRounding = ROUNDING;
	gSide = SIDE;
	gUnit = UNIT;
	gOffsetStuff = 1.;
	gShaft = true;
	if (gTimeMod >= 9.) {
		float tt = gTimeMod - 9.;
		gRounding = mix(3.4, ROUNDING, tt);
		gSide = 12. - gRounding;
		gUnit = gSide * 2. + 2. * gRounding;
		gOffsetStuff = mix(.23, 1., tt);
		ro.z += mix(4., 0., tt);
		at.z += mix(4., 0., tt);
		gShaft = false;
	}

	vec3	cf = normalize(at-ro),
		cl = normalize(cross(cf,vec3(0,0,-1))),
		rd = mat3(cl,normalize(cross(cl,cf)),cf)*normalize(vec3(uv,1)),
		col = vec3(.1) - length(uv) * .05;

	vec4 result = march(ro, rd, 100);

	if (result.x > 0.) { // hit
		vec3 shade = colorHit(result, rd);
		if (gCubeOpacity[gHitIndex] < 1.) {
			float opacity = gCubeOpacity[gHitIndex];
			gCubeHidden[gHitIndex] = true;
			result = march(gHitPosition, rd, 50); // TODO: how many steps?
			vec3 without = result.x > 0. && (!gHackFadeStuff || gHitIndex == 0) ? colorHit(result, rd) : col;
			shade = mix(without, shade, opacity);
		}
		col = shade;
	}

	gl_FragColor = vec4(pow(col, vec3(.4545)), 1.0); // pow for gamma correction because all the cool kids do it
}

