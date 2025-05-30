/*
  Copyright (c) 2021 Tuomas Roenty   http://www.tuomasronty.fi/
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

float2 iMouse=float2(1, 1);
float2 iResolution=float2(1, 1);
float iTime=0.0;

float3 mod(float3 x, float y)
{
  return x - y * floor(x / y);
}

// Mountains. By David Hoskins - 2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

// https://www.shadertoy.com/view/4slGD4
// A ray-marched version of my terrain renderer which uses
// streaming tex2D normals for speed:-
// http://www.youtube.com/watch?v=qzkBnCBpQAM

// It uses binary subdivision to accurately find the height map.
// Lots of thanks to Inigo and his noise functions!

// Video of my OpenGL version that 
// http://www.youtube.com/watch?v=qzkBnCBpQAM

// Stereo version code thanks to Croqueteer :)
//#define STEREO 

float treeLine = 0.0;
float treeCol = 0.0;


float3 sunLight  = normalize( float3(  0.4, 0.4,  0.48 ) );
float3 sunColour = float3(1.0, .9, .83);
float specular = 0.0;
float3 cameraPos;
float ambient;
float2 add = float2(1.0, 0.0);
#define HASHSCALE1 .1031
#define HASHSCALE3 float3(.1031, .1030, .0973)
#define HASHSCALE4 float4(1031, .1030, .0973, .1099)

// This peturbs the fractal positions for each iteration down...
// Helps make nice twisted landscapes...
float2x2 rotate2D = float2x2(1.3623, 1.7531, -1.7131, 1.4623);

// Alternative rotation:-
// float2x2 rotate2D = float2x2(1.2323, 1.999231, -1.999231, 1.22);


//  1 out, 2 in...
float Hash12(float2 p)
{
	float3 p3  = frac(float3(p.xyx) * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}
float2 Hash22(float2 p)
{
	float3 p3 = frac(float3(p.xyx) * HASHSCALE3);
    p3 += dot(p3, p3.yzx+19.19);
    return frac((p3.xx+p3.yz)*p3.zy);

}

float Noise( in float2 x )
{
    float2 p = floor(x);
    float2 f = frac(x);
    f = f*f*(3.0-2.0*f);
    
    float res = lerp(lerp( Hash12(p),          Hash12(p + add.xy),f.x),
                    lerp( Hash12(p + add.yx), Hash12(p + add.xx),f.x),f.y);
    return res;
}

float2 Noise2( in float2 x )
{
    float2 p = floor(x);
    float2 f = frac(x);
    f = f*f*(3.0-2.0*f);
    float n = p.x + p.y * 57.0;
   float2 res = lerp(lerp( Hash22(p),          Hash22(p + add.xy),f.x),
                  lerp( Hash22(p + add.yx), Hash22(p + add.xx),f.x),f.y);
    return res;
}

//--------------------------------------------------------------------------
float Trees(float2 p)
{
	
 	//return (tex2D(iChannel1,0.04*p).x * treeLine);
    return Noise(p*13.0)*treeLine;
}


//--------------------------------------------------------------------------
// Low def version for ray-marching through the height field...
// Thanks to IQ for all the noise stuff...

float Terrain( in float2 p)
{
	float2 pos = p*0.05;
	float w = (Noise(pos*.25)*0.75+.15);
	w = 66.0 * w * w;
	float2 dxy = float2(0.0, 0.0);
	float f = .0;
	for (int i = 0; i < 5; i++)
	{
		f += w * Noise(pos);
		w = -w * 0.4;	//...Flip negative and positive for variation
		pos = mul(rotate2D, pos);
	}
	float ff = Noise(pos*.002);
	
	f += pow(abs(ff), 5.0)*275.-5.0;
	return f;
}

//--------------------------------------------------------------------------
// Map to lower resolution for height field mapping for Scene function...
float Map(in float3 p)
{
	float h = Terrain(p.xz);
		

	float ff = Noise(p.xz*.3) + Noise(p.xz*3.3)*.5;
	treeLine = smoothstep(ff, .0+ff*2.0, h) * smoothstep(1.0+ff*3.0, .4+ff, h) ;
	treeCol = Trees(p.xz);
	h += treeCol;
	
    return p.y - h;
}

//--------------------------------------------------------------------------
// High def version only used for grabbing normal information.
float Terrain2( in float2 p)
{
	// There's some real magic numbers in here! 
	// The Noise calls add large mountain ranges for more variation over distances...
	float2 pos = p*0.05;
	float w = (Noise(pos*.25)*0.75+.15);
	w = 66.0 * w * w;
	float2 dxy = float2(0.0, 0.0);
	float f = .0;
	for (int i = 0; i < 5; i++)
	{
		f += w * Noise(pos);
		w =  - w * 0.4;	//...Flip negative and positive for varition	   
		pos = rotate2D * pos;
	}
	float ff = Noise(pos*.002);
	f += pow(abs(ff), 5.0)*275.-5.0;
	

	treeCol = Trees(p);
	f += treeCol;
	if (treeCol > 0.0) return f;

	
	// That's the last of the low resolution, now go down further for the Normal data...
	for (int i = 0; i < 6; i++)
	{
		f += w * Noise(pos);
		w =  - w * 0.4;
		pos = rotate2D * pos;
	}
	
	
	return f;
}

//--------------------------------------------------------------------------
float FractalNoise(in float2 xy)
{
	float w = .7;
	float f = 0.0;

	for (int i = 0; i < 4; i++)
	{
		f += Noise(xy) * w;
		w *= 0.5;
		xy *= 2.7;
	}
	return f;
}

//--------------------------------------------------------------------------
// Simply Perlin clouds that fade to the horizon...
// 200 units above the ground...
float3 GetClouds(in float3 sky, in float3 rd)
{
	if (rd.y < 0.01) return sky;
	float v = (200.0-cameraPos.y)/rd.y;
	rd.xz *= v;
	rd.xz += cameraPos.xz;
	rd.xz *= .010;
	float f = (FractalNoise(rd.xz) -.55) * 5.0;
	// Uses the ray's y component for horizon fade of fixed colour clouds...
	sky = lerp(sky, float3(.55, .55, .52), clamp(f*rd.y-.1, 0.0, 1.0));

	return sky;
}



//--------------------------------------------------------------------------
// Grab all sky information for a given ray from camera
float3 GetSky(in float3 rd)
{
	float sunAmount = max( dot( rd, sunLight), 0.0 );
	float v = pow(1.0-max(rd.y,0.0),5.)*.5;
	float3  sky = float3(v*sunColour.x*0.4+0.18, v*sunColour.y*0.4+0.22, v*sunColour.z*0.4+.4);
	// Wide glare effect...
	sky = sky + sunColour * pow(sunAmount, 6.5)*.32;
	// Actual sun...
	sky = sky+ sunColour * min(pow(sunAmount, 1150.0), .3)*.65;
	return sky;
}

//--------------------------------------------------------------------------
// Merge mountains into the sky background for correct disappearance...
float3 ApplyFog( in float3  rgb, in float dis, in float3 dir)
{
	float fogAmount = exp(-dis* 0.00005);
	return lerp(GetSky(dir), rgb, fogAmount );
}

//--------------------------------------------------------------------------
// Calculate sun light...
void DoLighting(inout float3 mat, in float3 pos, in float3 normal, in float3 eyeDir, in float dis)
{
	float h = dot(sunLight,normal);
	float c = max(h, 0.0)+ambient;
	mat = mat * sunColour * c ;
	// Specular...
	if (h > 0.0)
	{
		float3 R = reflect(sunLight, normal);
		float specAmount = pow( max(dot(R, normalize(eyeDir)), 0.0), 3.0)*specular;
		mat = lerp(mat, sunColour, specAmount);
	}
}

//--------------------------------------------------------------------------
// Hack the height, position, and normal data to create the coloured landscape
float3 TerrainColour(float3 pos, float3 normal, float dis)
{
	float3 mat;
	specular = .0;
	ambient = .1;
	float3 dir = normalize(pos-cameraPos);
	
	float3 matPos = pos * 2.0;// ... I had change scale halfway though, this lazy multiply allow me to keep the graphic scales I had

	float disSqrd = dis * dis;// Squaring it gives better distance scales.

	float f = clamp(Noise(matPos.xz*.05), 0.0,1.0);//*10.8;
	f += Noise(matPos.xz*.1+normal.yz*1.08)*.85;
	f *= .55;
	float3 m = lerp(float3(.63*f+.2, .7*f+.1, .7*f+.1), float3(f*.43+.1, f*.3+.2, f*.35+.1), f*.65);
	mat = m*float3(f*m.x+.36, f*m.y+.30, f*m.z+.28);
	// Should have used smoothstep to add colours, but left it using 'if' for sanity...
	if (normal.y < .5)
	{
		float v = normal.y;
		float c = (.5-normal.y) * 4.0;
		c = clamp(c*c, 0.1, 1.0);
		f = Noise(float2(matPos.x*.09, matPos.z*.095+matPos.yy*0.15));
		f += Noise(float2(matPos.x*2.233, matPos.z*2.23))*0.5;
		mat = lerp(mat, float3(.4*f), c);
		specular+=.1;
	}

	// Grass. Use the normal to decide when to plonk grass down...
	if (matPos.y < 45.35 && normal.y > .65)
	{

		m = float3(Noise(matPos.xz*.023)*.5+.15, Noise(matPos.xz*.03)*.6+.25, 0.0);
		m *= (normal.y- 0.65)*.6;
		mat = lerp(mat, m, clamp((normal.y-.65)*1.3 * (45.35-matPos.y)*0.1, 0.0, 1.0));
	}

	if (treeCol > 0.0)
	{
		mat = float3(.02+Noise(matPos.xz*5.0)*.03, .05, .0);
		normal = normalize(normal+float3(Noise(matPos.xz*33.0)*1.0-.5, .0, Noise(matPos.xz*33.0)*1.0-.5));
		specular = .0;
	}
	
	// Snow topped mountains...
	if (matPos.y > 80.0 && normal.y > .42)
	{
		float snow = clamp((matPos.y - 80.0 - Noise(matPos.xz * .1)*28.0) * 0.035, 0.0, 1.0);
		mat = lerp(mat, float3(.7,.7,.8), snow);
		specular += snow;
		ambient+=snow *.3;
	}
	// Beach effect...
	if (matPos.y < 1.45)
	{
		if (normal.y > .4)
		{
			f = Noise(matPos.xz * .084)*1.5;
			f = clamp((1.45-f-matPos.y) * 1.34, 0.0, .67);
			float t = (normal.y-.4);
			t = (t*t);
			mat = lerp(mat, float3(.09+t, .07+t, .03+t), f);
		}
		// Cheap under water darkening...it's wet after all...
		if (matPos.y < 0.0)
		{
			mat *= .2;
		}
	}

	DoLighting(mat, pos, normal,dir, disSqrd);
	
	// Do the water...
	if (matPos.y < 0.0)
	{
		// Pull back along the ray direction to get water surface point at y = 0.0 ...
		float time = (iTime)*.03;
		float3 watPos = matPos;
		watPos += -dir * (watPos.y/dir.y);
		// Make some dodgy waves...
		float tx = cos(watPos.x*.052) *4.5;
		float tz = sin(watPos.z*.072) *4.5;
		float2 co = Noise2(float2(watPos.x*4.7+1.3+tz, watPos.z*4.69+time*35.0-tx));
		co += Noise2(float2(watPos.z*8.6+time*13.0-tx, watPos.x*8.712+tz))*.4;
		float3 nor = normalize(float3(co.x, 20.0, co.y));
		nor = normalize(reflect(dir, nor));//normalize((-2.0*(dot(dir, nor))*nor)+dir);
		// Mix it in at depth transparancy to give beach cues..
        tx = watPos.y-matPos.y;
		mat = lerp(mat, GetClouds(GetSky(nor)*float3(.3,.3,.5), nor)*.1+float3(.0,.02,.03), clamp((tx)*.4, .6, 1.));
		// Add some extra water glint...
        mat += float3(.1)*clamp(1.-pow(tx+.5, 3.)*tex2D(iChannel1, watPos.xz*.1, -2.).x, 0.,1.0);
		float sunAmount = max( dot(nor, sunLight), 0.0 );
		mat = mat + sunColour * pow(sunAmount, 228.5)*.6;
        float3 temp = (watPos-cameraPos*2.)*.5;
        disSqrd = dot(temp, temp);
	}
	mat = ApplyFog(mat, disSqrd, dir);
	return mat;
}

//--------------------------------------------------------------------------
float BinarySubdivision(in float3 rO, in float3 rD, float2 t)
{
	// Home in on the surface by dividing by two and split...
    float halfwayT;
  
    for (int i = 0; i < 5; i++)
    {

        halfwayT = dot(t, float2(.5));
        float d = Map(rO + halfwayT*rD); 
         t = lerp(float2(t.x, halfwayT), float2(halfwayT, t.y), step(0.5, d));

    }
	return halfwayT;
}

//--------------------------------------------------------------------------
bool Scene(in float3 rO, in float3 rD, out float resT, in float2 fragCoord )
{
    float t = 1. + Hash12(fragCoord.xy)*1.;
	float oldT = 0.0;
	float delta = 0.0;
	bool fin = false;
	bool res = false;
	float2 distances;
	for( int j=0; j< 150; j++ )
	{
		if (fin || t > 240.0) break;
		float3 p = rO + t*rD;
		//if (t > 240.0 || p.y > 195.0) break;
		float h = Map(p); // ...Get this positions height mapping.
		// Are we inside, and close enough to fudge a hit?...
		if( h < 0.5)
		{
			fin = true;
			distances = float2(oldT, t);
			break;
		}
		// Delta ray advance - a fudge between the height returned
		// and the distance already travelled.
		// It's a really fiddly compromise between speed and accuracy
		// Too large a step and the tops of ridges get missed.
		delta = max(0.01, 0.3*h) + (t*0.0065);
		oldT = t;
		t += delta;
	}
	if (fin) resT = BinarySubdivision(rO, rD, distances);

	return fin;
}

//--------------------------------------------------------------------------
float3 CameraPath( float t )
{
	float m = 1.0+(iMouse.x/iResolution.x)*300.0;
	t = (iTime*1.5+m+657.0)*.006 + t;
    float2 p = 476.0*float2( sin(3.5*t), cos(1.5*t) );
	return float3(35.0-p.x, 0.6, 4108.0+p.y);
}

//--------------------------------------------------------------------------
// Some would say, most of the magic is done in post! :D
float3 PostEffects(float3 rgb, float2 uv)
{
	//#define CONTRAST 1.1
	//#define SATURATION 1.12
	//#define BRIGHTNESS 1.3
	//rgb = pow(abs(rgb), float3(0.45));
	//rgb = lerp(float3(.5), lerp(float3(dot(float3(.2125, .7154, .0721), rgb*BRIGHTNESS)), rgb*BRIGHTNESS, SATURATION), CONTRAST);
	rgb = (1.0 - exp(-rgb * 6.0)) * 1.0024;
	//rgb = clamp(rgb+hash12(fragCoord.xy*rgb.r)*0.1, 0.0, 1.0);
	return rgb;
}

//--------------------------------------------------------------------------
float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
    float2 xy = -1.0 + 2.0*fragCoord.xy / iResolution.xy;
	float2 uv = xy * float2(iResolution.x/iResolution.y,1.0);
	float3 camTar;

	#ifdef STEREO
	float isCyan = mod(fragCoord.x + mod(fragCoord.y,2.0),2.0);
	#endif

	// Use several forward heights, of decreasing influence with distance from the camera.
	float h = 0.0;
	float f = 1.0;
	for (int i = 0; i < 7; i++)
	{
		h += Terrain(CameraPath((.6-f)*.008).xz) * f;
		f -= .1;
	}
	cameraPos.xz = CameraPath(0.0).xz;
	camTar.xyz	 = CameraPath(.1).xyz;
	camTar.y = cameraPos.y = max((h*.25)+3.5, 1.5+sin(iTime*5.)*.5);
    camTar.y -= smoothstep(60.0, 300.0,cameraPos.y)*150.;
	
	float roll = 0.15*sin(iTime*.2);
	float3 cw = normalize(camTar-cameraPos);
	float3 cp = float3(sin(roll), cos(roll),0.0);
	float3 cu = normalize(cross(cw,cp));
	float3 cv = normalize(cross(cu,cw));
	float3 rd = normalize( uv.x*cu + uv.y*cv + 1.5*cw );

	#ifdef STEREO
	cameraPos += .45*cu*isCyan; // move camera to the right - the rd vector is still good
	#endif

	float3 col;
	float distance;
	if( !Scene(cameraPos,rd, distance, fragCoord) )
	{
		// Missed scene, now just get the sky value...
		col = GetSky(rd);
		col = GetClouds(col, rd);
	}
	else
	{
		// Get world coordinate of landscape...
		float3 pos = cameraPos + distance * rd;
		// Get normal from sampling the high definition height map
		// Use the distance to sample larger gaps to help stop aliasing...
		float p = .02+.00005 * distance * distance;
		float3 nor  	= float3(0.0,		    Terrain2(pos.xz), 0.0);
		float3 v2		= nor-float3(p,		Terrain2(pos.xz+float2(p,0.0)), 0.0);
		float3 v3		= nor-float3(0.0,		Terrain2(pos.xz+float2(0.0,-p)), -p);
		nor = cross(v2, v3);
		nor = normalize(nor);

		// Get the colour using all available data...
		col = TerrainColour(pos, nor, distance);
	}

	col = PostEffects(col, uv);
	
	#ifdef STEREO	
	col *= float3( isCyan, 1.0-isCyan, 1.0-isCyan );	
	#endif
	
	fragColor=float4(col,1.0);
	return fragColor;
}

