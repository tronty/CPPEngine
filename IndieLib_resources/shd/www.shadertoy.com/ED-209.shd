/*
  Copyright (c) 2022 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

struct VsIn2 {
    float2 position	: POSITION;
    float2 uv		: TEXCOORD0;
};
struct VsIn3 {
    float3 position	: POSITION;
    float3 Normal	: NORMAL;
    float3 Binormal	: BINORMAL;
    float3 Tangent	: TANGENT;
    float3 Color	: TEXCOORD0;
    float2 uv		: TEXCOORD1;
};
struct VsOut {
    float4 position	: POSITION;
    float2 uv		: TEXCOORD0;
};

[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOut main2(VsIn2 In)
{
	VsOut Out=(VsOut)0;
	Out.position = float4(In.position.x, In.position.y, 0.0 , 1.0);
	Out.uv.x = In.uv.x;Out.uv.y = 1.0-In.uv.y;
	return Out;
}
VsOut main3(VsIn3 In)
{
	VsOut Out=(VsOut)0;
	Out.position = mul(worldViewProj, float4(In.position, 1.0));
	Out.uv.x = In.uv.x;Out.uv.y = 1.0-In.uv.y;
	return Out;
}

[Fragment shader]
float mod(float x, float y)
{
  return x - y * floor(x / y);
}
float2 mod(float2 x, float2 y)
{
  return x - y * floor(x / y);
}
float3 mod(float3 x, float3 y)
{
  return x - y * floor(x / y);
}
float iTime=0.0;
float2 iMouse=float2(1, 1);
float2 iResolution=float2(1, 1);
sampler2D iChannel0;
sampler2D iChannel1;
sampler2D iChannel2;

// https://www.shadertoy.com/view/
// ED-209
//
// Processed by 'GLSL Shader Shrinker' (10,705 to 9,801 characters)
// (https://github.com/deanthecoder/GLSLShaderShrinker)
//
// I've always loved the (original) Robocop movie, but have never
// felt confident enough to try modelling ED-209.
// I hope you like it!
//
// Thanks to Evvvvil, Flopine, Nusan, BigWings, Iq, Shane
// and a bunch of others for sharing their knowledge!

// License: Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License

float stretch, gunsUp, gunsForward, edWalk, edTwist, edDown, edShoot, doorOpen, glow;

//#define AA  // Enable this line if your GPU can take it!

struct MarchData {
	float d;
	float3 mat; // RGB
	float specPower; // 0: None, 30.0: Shiny
};

float2x2 rot(float a) {
	float c = cos(a),
	      s = sin(a);
	return float2x2(c, s, -s, c);
}

float remap(float f, float in1, float in2, float out1, float out2) { return mix(out1, out2, clamp((f - in1) / (in2 - in1), 0., 1.)); }

float sdBox(float3 p, float3 b) {
	float3 q = abs(p) - b;
	return length(max(q, 0.)) + min(max(q.x, max(q.y, q.z)), 0.);
}

float sdChamferedCube(float3 p, float3 r, float c) {
	float cube = sdBox(p, r);
	p.xz *= rot(.78525);
	r.xz *= -c / 1.41 + 1.41;
	return max(cube, sdBox(p, r));
}

float sdTriPrism(float3 p, float2 h) {
	float3 q = abs(p);
	return max(q.z - h.y, max(q.x * .866025 + p.y * .5, -p.y) - h.x * .5);
}

float sdCappedCone(float3 p, float3 a, float3 b, float ra, float rb) {
	float rba = rb - ra,
	      baba = dot(b - a, b - a),
	      papa = dot(p - a, p - a),
	      paba = dot(p - a, b - a) / baba,
	      x = sqrt(papa - paba * paba * baba),
	      cax = max(0., x - ((paba < .5) ? ra : rb)),
	      cay = abs(paba - .5) - .5,
	      f = clamp((rba * (x - ra) + paba * baba) / (rba * rba + baba), 0., 1.),
	      cbx = x - ra - f * rba,
	      cby = paba - f;
	return ((cbx < 0. && cay < 0.) ? -1. : 1.) * sqrt(min(cax * cax + cay * cay * baba, cbx * cbx + cby * cby * baba));
}

float sdCappedCylinder(float3 p, float h, float r) {
	float2 d = abs(float2(length(p.xy), p.z)) - float2(h, r);
	return min(max(d.x, d.y), 0.) + length(max(d, 0.));
}

float sdCapsule(float3 p, float3 a, float3 b, float r) {
	float3 pa = p - a,
	     ba = b - a;
	return length(pa - ba * clamp(dot(pa, ba) / dot(ba, ba), 0., 1.)) - r;
}

float sdOctogon(float2 p, float r) {
	const float3 k = float3(-.92387953, .38268343, .41421356);
	p = abs(p);
	p -= 2. * min(dot(k.xy, p), 0.) * k.xy;
	p -= 2. * min(dot(float2(-k.x, k.y), p), 0.) * float2(-k.x, k.y);
	p -= float2(clamp(p.x, -k.z * r, k.z * r), r);
	return length(p) * sign(p.y);
}

float3 getRayDir(float3 ro, float3 lookAt, float2 uv) {
	float3 forward = normalize(lookAt - ro),
	     right = normalize(cross(float3(0, 1, 0), forward));
	return normalize(forward + right * uv.x + cross(forward, right) * uv.y);
}

MarchData minResult(MarchData a, MarchData b) {
	if (a.d < b.d) return a;
	return b;
}

void setBodyMaterial(inout MarchData mat) {
	mat.mat = float3(.36, .45, .5);
	mat.specPower = 30.;
}

float legWalkAngle(float f) { return sin(edWalk * 3.141 * 6. * f) * .2; }

float edZ() { return mix(5., -2., edWalk); }

float fireShock() { return abs(sin(edShoot * 78.5375)); }

float headSphere(float3 p) { return (length(p / float3(1, .8, 1)) - 1.) * .8; }

MarchData headVisor(float3 p, float h, float bump) {
	bump *= sin(p.x * 150.) * sin(p.y * 150.) * .002;
	MarchData result;
	result.d = sdBox(p, float3(1, h, 2));
	result.d = max(mix(result.d, headSphere(p), .57), -p.y) - bump;
	result.mat = float3(.05);
	result.specPower = 30.;
	return result;
}

MarchData headLower(float3 p) {
	float3 op = p;

	// Start by mirroring the visor.
	MarchData r = headVisor(p * float3(.95, -1.4, .95), 1., 0.);

	// Add the side panels.
	r.d = min(r.d, max(max(headVisor((p + float3(0, .01, 0)) * float3(.95), 1., 0.).d, p.y - .35), p.y * .625 - p.z - .66));

	// 'Wings'.
	p.xy *= rot(.075 * (gunsUp - 1.) * sign(p.x));
	p.x = abs(p.x) - 1.33;
	p.y -= .1 - p.x * .1;
	r.d = min(r.d, sdBox(p, float3(.4, .06 * (1. - p.x), .3 - p.x * .2)));
	p = op;

	// Cut out a mouth grill.
	p.y = abs(abs(p.y + .147) - .0556) - .0278;
	r.d = max(r.d, -sdBox(p + float3(0, 0, 1.5), float3(mix(.25, .55, -op.y), .015, .1)));

	// 'Cheeks'.
	p = op;
	p.y = abs(p.y + .16) - .06;
	p.z -= -1.1;
	r.d = max(r.d, -max(max(sdCappedCylinder(p.xzy, 1., .03), -sdCappedCylinder(p.xzy, .55, 1.)), p.z + .2));
	setBodyMaterial(r);
	return r;
}

MarchData gunPod(float3 p) {
	MarchData r;
	setBodyMaterial(r);
	p.yz += float2(.1, .45);

	// Carousel.
	float3 pp = p;
	pp.z = abs(pp.z) - .5;
	r.d = sdCappedCone(pp, float3(0), float3(0, 0, -.1), .35 - .1, .35);
	r.d = min(r.d, sdCappedCylinder(p, .35, .4));

	// Triangle nobble.
	pp = float3(p.x, .28 - p.y, p.z);
	r.d = min(r.d, sdTriPrism(pp, float2(.1, .5)));

	// Square outriggers.
	pp = p;
	pp.x = abs(p.x);
	pp.xy *= rot(.78525);
	float fs,
	      bump = sign(sin(pp.z * 33.3)) * .003,
	      d = sdBox(pp, float3(.1 - bump, .38 - bump, .34)) - .02;

	// Barrels.
	pp = p - float3(0, 0, -.6);
	pp.x = abs(pp.x) - .1;
	d = min(min(min(d, sdCappedCylinder(pp, .06, .15)), sdCappedCylinder(pp + float3(0, .12, -.05), .06, .05)), sdBox(p + float3(0, 0, .54), float3(.1, .06, .04)));
	if (d < r.d) {
		d = max(d, -sdCappedCylinder(pp + float3(0, 0, .1), .03, .2));
		r.d = d;
		r.mat = float3(.02);
	}

	// Muzzle flash.
	fs = fireShock();
	if (fs > .5) {
		d = sdCappedCylinder(pp, .01 + pp.z * .05, fract(fs * 3322.423) * .5 + .9);
		if (d < r.d) {
			r.d = d;
			r.mat = float3(1);
			glow += .1 / (.01 + d * d * 4e2);
		}
	}

	return r;
}

MarchData arms(float3 p) {
	const float3 wrist = float3(1.5, 0, 0) - float3(0, 0, .3);
	MarchData r;
	setBodyMaterial(r);

	// Position origin.
	p.x = abs(p.x);
	p.yz += float2(.24, 0);
	p.xy *= rot(.15 * (gunsUp - 1.));

	// Shoulder and forearm.
	r.d = min(sdCapsule(p, float3(0), float3(1.5, 0, 0), .2), sdCapsule(p, float3(1.5, 0, 0), wrist, .2));

	// Gunz.
	p -= wrist;
	p.z -= gunsForward * .15;
	return minResult(r, gunPod(p));
}

float toe(float3 p) {
	p.yz += float2(.1, .32);
	return max(sdBox(p, float3(.3 + .2 * (p.z - .18) - p.y * .228, .3 + .2 * cos((p.z - .18) * 3.69), .35)), .1 - p.y);
}

float foot(float3 p) {
	p.z += .8;
	p.yz *= rot(.86);
	float d = toe(p);
	p.xz *= rot(1.57);
	p.x -= .43;
	p.z = .25 - abs(p.z);
	return min(d, toe(p));
}

MarchData waist(float3 p) {
	MarchData r;
	setBodyMaterial(r);
	p.y += .65;
	p.yz *= rot(-.2);
	float bump, d,
	      legAngle = legWalkAngle(1.);
	p.xy *= rot(legAngle * .3);
	float3 pp = p;
	pp.y += .3;
	r.d = max(sdCappedCylinder(pp.zyx, .5, .5), p.y + .15);

	// Thorax.
	bump = .5 - abs(sin(p.y * 40.)) * .03;
	d = sdBox(p, float3(bump, .15, bump));

	// Leg joins.
	bump = .3 - abs(sin(p.x * 40.)) * .03;
	pp.y += .18;
	d = min(d, sdCappedCylinder(pp.zyx, bump, .75));

	// Hips.
	pp.x = abs(pp.x);
	pp.yz *= rot(-.58525 + legAngle * sign(p.x));
	pp.x -= .98;
	r.d = min(r.d, max(sdCappedCylinder(pp.zyx, .4, .24), -pp.y));
	r.d = min(r.d, sdBox(pp, float3(.24, .2, .14 + .2 * pp.y)));

	// Thigh pistons.
	p = pp;
	pp.xz = abs(pp.xz) - float2(.12, .25);
	r.d = min(r.d, max(min(sdCappedCylinder(pp.xzy, .1, .325), sdCappedCylinder(pp.xzy, .05, .5)), pp.y));

	// 'Knees'.
	p.y += .68;
	r.d = min(r.d, sdBox(p, float3(sign(abs(p.y) - .04) * .005 + .26, .2, .34)));
	if (d < r.d) {
		// Black segments.
		r.d = d;
		r.mat = float3(.02);
	}

	return r;
}

MarchData legs(float3 p) {
	MarchData r;
	setBodyMaterial(r);
	float silver,
	      legAngle = legWalkAngle(1.);
	p.z += .27;
	p.yz *= rot(legAngle * sign(p.x));
	p.z -= .27;
	p.y += .65;
	p.yz *= rot(-.2);
	p.xy *= rot(legAngle * .3);
	float3 cp,
	     pp = p;
	pp.x = abs(pp.x);
	pp.y += .48;
	pp.yz *= rot(-.58525);
	pp.x -= .98;
	cp = pp;
	p = pp;
	pp.xz = abs(pp.xz) - float2(.12, .25);
	p.y += .68;

	// Thighs.
	p.xy = abs(p.xy) - .12;
	silver = sdBox(p, float3(.07, .05, 1.2));

	// Leg end cap.
	cp -= float3(0, -.7, 0);
	r.d = sdBox(cp - float3(0, 0, 1.15), float3(.17, .17, .07)) - .04;

	// Shin.
	cp.z++;
	r.d = min(r.d, sdChamferedCube(cp.xzy, float2(.28 - sign(abs(cp.z) - .3) * .01, .5).xyx, .18));

	// Feet.
	r.d = min(r.d, foot(cp));
	if (silver < r.d) {
		r.d = silver;
		r.mat = float3(.8);
	}

	return r;
}

MarchData ed209(float3 p) {
	p.yz += float2(legWalkAngle(2.) * .2 + .1, -edZ());
	MarchData r = legs(p);
	float f = min(stretch * 2., 1.),
	      slide = f < .5 ? smoothstep(0., .5, f) : (1. - smoothstep(.5, 1., f) * .2);
	p.yz -= slide * .5;
	gunsUp = smoothstep(0., 1., clamp((stretch - .66) * 6., 0., 1.)); // 0.66-0.83
	gunsForward = smoothstep(0., 1., clamp((stretch - .83) * 6., 0., 1.)) // 0.83-1.0
	 + fireShock() * .5;
	r = minResult(r, waist(p));
	p.yz *= rot(.1 * (-edDown + legWalkAngle(2.) + smoothstep(0., 1., clamp((stretch - .5) * 6., 0., 1.)) - 1.)); // 0.5-0.66
	p.xz *= rot(edTwist * .2);
	return minResult(minResult(minResult(r, headLower(p)), headVisor(p, .8, 1.)), arms(p));
}

MarchData room(float3 p) {
	const float3 frameInner = float3(2.8, 2.6, .1);
	MarchData r;
	r.mat = float3(.4);
	r.specPower = 1e7;
	float2 xy = p.xy - float2(0, 2);
	p.x = abs(p.x);
	p.yz += float2(.5, -3.4);
	float doorFrame, doorWidth, door, d,
	      doorHole = sdBox(p, frameInner + float3(0, 0, 1)),
	      backWall = length(p.z - 8.);
	r.d = min(backWall, max(length(p.z), -doorHole + .1));
	if (r.d == backWall) {
		float ocp = min(abs(sdOctogon(xy, 2.6)), abs(sdOctogon(xy, 1.9)));
		ocp = min(max(ocp, min(.7 - abs(xy.x + 1.2), -xy.y)), max(abs(sdOctogon(xy, 1.2)), min(xy.x, .7 - abs(xy.y))));
		if (ocp < .3) r.mat = float3(.39, .57, .71);
	}

	doorFrame = max(sdBox(p, frameInner + float3(.4, .4, .1)), -doorHole);
	doorWidth = frameInner.x * .5;
	p.x -= frameInner.x;
	p.xz *= rot(doorOpen * 2.1);
	p.x += doorWidth;
	door = sdBox(p, float3(doorWidth, frameInner.yz));
	p = abs(p) - float3(doorWidth * .5, 1.1, .14);
	d = min(doorFrame, max(door, -max(sdBox(p, float3(.45, .9, .1)), -sdBox(p, float3(.35, .8, 1)))));
	if (d < r.d) {
		r.d = d;
		r.mat = float3(.02, .02, .024);
		r.specPower = 10.;
	}

	return r;
}

// Map the scene using SDF functions.
MarchData map(float3 p) {
	MarchData r = minResult(room(p), ed209(p));
	float gnd = length(p.y + 3.);
	if (gnd < r.d) {
		r.d = gnd;
		r.mat = float3(.1);
	}

	return r;
}

float calcShadow(float3 p, float3 lightPos) {
	// Thanks iq.
	float3 rd = normalize(lightPos - p);
	float res = 1.,
	      t = .1;
	for (float i = 0.; i < 30.; i++) {
		float h = map(p + rd * t).d;
		res = min(res, 12. * h / t);
		t += h;
		if (res < .001 || t > 25.) break;
	}

	return clamp(res, 0., 1.);
}

float3 calcNormal(float3 p, float t) {
	float d = .01 * t * .33;
	float2 e = float2(1, -1) * .5773 * d;
	return normalize(e.xyy * map(p + e.xyy).d + e.yyx * map(p + e.yyx).d + e.yxy * map(p + e.yxy).d + e.xxx * map(p + e.xxx).d);
}

// Quick ambient occlusion.
float ao(float3 p, float3 n, float h) { return map(p + h * n).d / h; }

/**********************************************************************************/
float3 vignette(float3 col, float2 fragCoord) {
	float2 q = fragCoord.xy / iResolution.xy;
	col *= .5 + .5 * pow(16. * q.x * q.y * (1. - q.x) * (1. - q.y), .4);
	return col;
}

float3 applyLighting(float3 p, float3 rd, float d, MarchData data) {
	float3 sunDir = normalize(float3(10, 10, -10) - p),
	     n = calcNormal(p, d);

	// Primary light.
	float primary = max(0., dot(sunDir, n)),
	      bounce = max(0., dot(-sunDir, n)) * .2,
	      spe = pow(max(0., dot(rd, reflect(sunDir, n))), data.specPower) * 2.,
	      fre = smoothstep(.7, 1., 1. + dot(rd, n)),
	      fog = exp(-length(p) * .05);

	// Combine.
	primary *= mix(.2, 1., calcShadow(p, float3(10, 10, -10)));
	return mix(data.mat * ((primary + bounce) * ao(p, n, .33) + spe) * float3(2, 1.6, 1.7), float3(.01), fre) * fog;
}

float3 getSceneColor(float3 ro, float3 rd) {
	// Raymarch.
	float3 p;
	float g,
	      d = .01;
	MarchData h;
	for (float steps = 0.; steps < 120.; steps++) {
		p = ro + rd * d;
		h = map(p);
		if (abs(h.d) < .0015 * d) break;
		if (d > 64.) return float3(0); // Distance limit reached - Stop.
		d += h.d; // No hit, so keep marching.
	}

	// Lighting.
	g = glow;
	return applyLighting(p, rd, d, h) + fireShock() * .3 + g;
}

float4 main(float2 fragCoord : TEXCOORD0) : COLOR0
{
	edWalk = 1.;
	edTwist = 0.;
	edDown = 0.;
	edShoot = 0.;
	doorOpen = 1.;
	stretch = 1.;

	// Camera.
	float3 ro, lookAt, col;
	float startScene, endScene, dim,
	      time = mod(iTime, 55.);
	if (time < 12.) {
		startScene = 0.;
		endScene = 12.;
		edWalk = 0.;
		ro = float3(0, -1.5, -.625);
		lookAt = float3(0, -1, edZ());
		doorOpen = smoothstep(0., 1., time / 5.);
		stretch = remap(time, 7., 10., 0., 1.);
	}
	else if (time < 25.) {
		startScene = 12.;
		endScene = 25.;
		float t = time - startScene;
		edWalk = smoothstep(0., 1., remap(t, 3., 8., 0., 1.));
		ro = float3(-.5 * cos(t * .7), .5 - t * .1, edZ() - 3.);
		lookAt = float3(0, 0, edZ());
	}
	else if (time < 29.) {
		startScene = 25.;
		endScene = 29.;
		ro = float3(-2, .5 + (time - startScene) * .1, edZ() - 3.);
		lookAt = float3(0, 0, edZ());
	}
	else if (time < 37.) {
		startScene = 29.;
		endScene = 37.;
		float t = time - startScene;
		ro = float3(1.5, -1. - t * .05, edZ() - 5.);
		lookAt = float3(0, -1, edZ());
		stretch = remap(t, 2., 5., 1., 0.);
	}
	else if (time < 55.) {
		startScene = 37.;
		endScene = 55.;
		float t = time - startScene;
		ro = float3(-1.8, -.5, edZ() - 2.5);
		stretch = remap(t, 2., 3., 0., 1.) - remap(t, 11.5, 14.5, 0., 1.);
		lookAt = float3(0, stretch * .5 - .5, edZ());
		edTwist = remap(t, 3., 3.2, 0., 1.) * stretch;
		edDown = remap(t, 3.2, 3.4, 0., 1.) * stretch;
		edShoot = t <= 9.5 ? remap(t, 4., 9.5, 0., 1.) : 0.;
	}

	dim = 1. - cos(min(1., 2. * min(abs(time - startScene), abs(time - endScene))) * 1.5705);
	col = float3(0);
#ifdef AA
	for (float dx = 0.; dx <= 1.; dx++) {
		for (float dy = 0.; dy <= 1.; dy++) {
			float2 coord = fragCoord + float2(dx, dy) * .5;
#else
			float2 coord = fragCoord;
#endif
			coord += (fract(fireShock() * float2(23242.232, 978.23465)) - .5) * 10.;
			float2 uv = (coord - .5 * iResolution.xy) / iResolution.y;
			col += getSceneColor(ro, getRayDir(ro, lookAt, uv));
#ifdef AA
		}
	}

	col /= 4.;
#endif

	// Output to screen.
	return float4(vignette(pow(col * dim, float3(.4545)), fragCoord), 1);
}

