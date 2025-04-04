/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
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
// https://www.shadertoy.com/view/Mdj3zD
// Campfire
float3      iResolution=float3(1,1,1);                  // viewport resolution (in pixels)
float     iTime=0;                        // shader playback time (in seconds)
float4      iMouse=float4(1,1,1,1);                       // mouse pixel coords
sampler2D iChannel0;
sampler2D iChannel1;
float3      iChannelResolution[4];        // channel resolution (in pixels)
// Campfire.	By Dave Hoskins. Nov. 2013

// Video:-  http://youtu.be/VBkYDxfO-7Y

// Using ray-marching to step through the volume around the fire,
// colliding with logs, rocks, and also adding flames as it goes
// with a 3D noise_ algorithm.

#define TAU 6.28318530718

//=================================================================================================
// https://www.shadertoy.com/view/4djSRW
float Hash11(float p)
{
	float3 p3  = frac(float3(p,p,p) * .1031);
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}

//=================================================================================================
float Noise( in float x )
{
    float p = floor(x);
    float f = frac(x);
    f = f*f*(3.0-2.0*f);
    return lerp(Hash11(p), Hash11(p+1.0), f);
}

//=================================================================================================
float Bump( in float3 x )
{
    float3 p = floor(x);
    float3 f = frac(x);
	f = f*f*(3.0-2.0*f);
	
	float2 uv = (p.xy + float2(37.0, 17.0) * p.z) + f.xy;
	float2 rg = tex2D( iChannel0, (uv+ 0.5) / 256.0).yx;
	return lerp(rg.x, rg.y, f.z);
}

//=================================================================================================
float Noise( in float3 x )
{
	x.y -= iTime *4.0;	
    float3 p = floor(x);
    float3 f = frac(x);
	f = f*f*(3.0-2.0*f);
	
	float2 uv = (p.xy + float2(37.0, 17.0) * p.z) + f.xy;
	float2 rg = tex2D( iChannel0, (uv+ 0.5) / 256.0).yx;
	return lerp(rg.x, rg.y, f.z);
}

//=================================================================================================
float FireLog(float3 p, float3 a, float3 b, float r)
{
	float3 pa = p - a;
	float3 ba = b - a;
	p = abs(p);
	float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
	return length( pa - ba*h ) - r- Bump(p*6.4)*.03 - Bump(p*2.4)*.3;
}

//=================================================================================================
float RoundBox( float3 p, float3 add)
{
	return length(max(abs(p+add)-1.1,0.0)) - Bump(p*float3(1.3, .1, 1.3))*.5;
}

//=================================================================================================
float DE_Fire(float3 p)
{
	p.xz += (Noise(p * .8))* p.y * .3;
	float3 shape = p * float3(1.5, .35, 1.5);
	if (dot(shape, shape) > 70.0) return 1.0;
	
	p += 2.5 * (Noise( shape * 1.5) -
				Noise(-shape * 0.945) *.5 +
				Noise( shape * 9.6)*.3);
	float f = (length( shape) - (1.+Noise(p)*10.0));

	f -= max(3.4-p.y, 0.0)*3.0;
	f -= pow(abs(Noise(shape*3.9)), 45.0) * 300.0 * pow(abs(Noise(shape*1.1)), 5.0);
	return f;
}

//=================================================================================================
float DE_Stones(float3 p)
{
	p.xz = abs(p.xz);

	float d =  RoundBox(p, float3(-7.0, 0.9, 0.0));
	d = min(d, RoundBox(p, float3(0.0, 0.85, -7.0)));
	d = min(d, RoundBox(p, float3(-5.5, 0.9, -3.)));
	d = min(d, RoundBox(p, float3(-3.0, 0.95, -5.5)));
	p.y -= 4.0;
	return max(-(length(p)-7.0 - Bump(p)*.3), d);
}

//=================================================================================================
float DE_Logs(float3 p)
{
	float  d = FireLog(p, float3(2.0, 0.3, -4.0),  float3(-3.0, 0.65, 4.0), .5);
	d = min(d, FireLog(p, float3(3.0, 0.1, 4.0),  float3(-4.0, 2.4, 0.5), .5));
	d = min(d, FireLog(p, float3(-2.2, 0.65, -4.5),  float3(2.0, 1.5, 3.0), .3));
	d = min(d, FireLog(p, float3(-2.5, 0.0, -2.0),  float3(3., 0.0, 1.5), .65));
	d = min(d, FireLog(p, float3(4.5, 0.0, -0.9),  float3(-4.0, 3.5, 0.9), .1));
	return d;
}

//=================================================================================================
float MapAll(float3 p)
{
	float d = DE_Logs(p);
	if (d > .05)
		d = min(d, DE_Stones(p));
	return d;
}
//=================================================================================================
float3 Normal( in float3 pos )
{
	float2 eps = float2( 0.05, 0.0);
	float3 nor = float3(
	    MapAll(pos+eps.xyy) - MapAll(pos-eps.xyy),
	    MapAll(pos+eps.yxy) - MapAll(pos-eps.yxy),
	    MapAll(pos+eps.yyx) - MapAll(pos-eps.yyx) );
	return normalize(nor);
}

//=================================================================================================
float4 Raymarch( in float3 ro, in float3 rd, inout int hit, in float2 fragCoord)
{
	float sum = 0.0;
	// Starting point plus dither to prevent banding...
	float t = 4.2 + .1 * tex2D(iChannel0, fragCoord.xy / iChannelResolution[0].xy).y;
	float3 pos = float3(0.0, 0.0, 0.0);
	float d = 100.0;
	for(int i=0; i < 200; i++)
	{
		if (hit > 0 || pos.y < 0.0)
		{
			// Skip the loop code quickly...
			break;
		}
		pos = ro + t*rd;
		
		float3 shape = pos * float3(1.5, .4, 1.5);
		if (dot(shape, shape) < 77.0)
		{
			d = DE_Logs(pos);
			if (d < 0.05)
			{
				pos = ro + (t + d) * rd;
				hit = 1;
			}
			else if (d < 0.45)
			{
				// Glow effect around log...
				sum += (.45-d) * .06;
			}
		}
		else
		{
			d = DE_Stones(pos);
			if (d < 0.05)
			{
				pos = ro + (t + d) * rd;
				hit = 2;
			}
		}
		
		float v = 1.0-DE_Fire( pos );
		v = max(v, 0.0) * .00187;
		sum += v;
		
    	t += max(.075, t*.005);
	}
	
	return float4(pos, clamp(sum*sum*sum, 0.0, 1.0 ));
}

//=================================================================================================
float2 RotateCamera(float2 p, float a)
{
	float si = sin(a);
	float co = cos(a);
	return mul(float2x2(si, co, -co, si) , p);
}

//=================================================================================================
float3 FlameColour(float f)
{
	f = f*f*(3.0-2.0*f);
	return  min(float3(f+.8, f*f*1.4+.1, f*f*f*.7) * f, 1.0);
}

//=================================================================================================

float4 main(VsOut IN): COLOR 
{
	float2 fragCoord=IN.uv;
	float2 uv = fragCoord.xy / iResolution.xy;
	float2 p = -1.0 + 2.0 * uv;
	p.x *= iResolution.x/iResolution.y;

	// Camera...
	float2 mouse;
	if (iMouse.z <= 0.5)
	{
		float time = iTime+39.5;
		mouse.x = time * 0.025;
		mouse.y = sin(time*.3)*.5 + .5;
	}
	else
	{
		mouse = iMouse.xy / iResolution.xy;
	}

	float3 origin = float3(0., 5.3, -13.0+mouse.y *5.0 );
	float3 target = float3( 0.0, 4.3-mouse.y*3.0, 0.0 );
	// Spin it with the mouse X...
	origin.xz = RotateCamera(origin.xz, .4 + (mouse.x * TAU));
	
	// Make camera ray using origin and target positions...
	float3 cw = normalize( target-origin);
	float3 cp = float3(0.0, 1.0, 0.0);
	float3 cu = normalize( cross(cw, cp) );
	float3 cv = ( cross(cu,cw) );
	float3 ray = normalize(p.x*cu + p.y*cv + (1.5+(Noise(iTime*.5))*.1) * cw );
	
	int hit = 0;
	float4 ret = Raymarch(origin, ray, hit, fragCoord);
	float3 col = float3(0.3, 0.3, 0.3);
	float flicker = Noise(iTime*8.0);
	float3 light = float3(0.0, 3.25 + flicker, 0.0);
	float3 nor, ldir;
	if (hit > 0 && hit < 3)
	{
		nor  = Normal(ret.xyz);
		ldir = normalize(light - ret.xyz);
	}

	if (hit == 1)
	{
		// Logs...
		float bri = max(dot(ldir, nor), .4);
		bri = bri*bri * 7.0;
		float f = Bump(ret.xyz*4.53 - float3 (0.0, iTime*.37, 0.0))*.8 + Bump(ret.xyz*17.3)*.4;
		f += Bump(ret.xyz * 1.0);
		f = pow(abs(f), 13.0) * .01  + max(.7-dot(ret.xyz, ret.xyz)*.03, 0.0);
		float3 mat = f * float3(.8, .4, 0.);
		col = mat * float3(.1, .04, 0.0) * bri;
		col *= clamp(1.0-length(ret.xz)*.04, 0.0, 1.0);
	}
	else if (hit == 2)
	{
//		// Stones...
		float3 ref  = reflect(ray, nor);
		float bri = max(dot(ldir, nor), 0.05);
		float spe = max(dot(ldir, ref), 0.0);
		spe = pow(abs(spe), 10.0);
		float f3=Bump(ret.xyz * 12.3)*.5+.5;
		float3 mat = float3(f3, f3, f3) * float3(1.0, .2+tex2D(iChannel0, ret.xz*.002).x*.5, .12);
		col = mat * bri + float3(.9, .6, .3) * spe;
	}else
	{
		// Grab the forest tex2D...
		float3 frst = tex2D(iChannel1, ray-float3(0.0, .08, 0.0) /* , -100.0 */ ).xyz;
		frst = frst*frst;
		col = frst * float3(.13, .13, .13);
//		// Is ray looking at ground area?...
		if (ray.y < 0.0)
		{
//			// Dodgy re-projection onto floor...
			float3 pos = origin+float3(0.0, -7.0, 0.0) + ray * (-origin.y / ray.y);
			float3 mat = tex2D(iChannel1, pos /* , -100.0 */ ).xyz;
			// Rudimentary bump map...
			float2 bmp = tex2D(iChannel0, pos.xz*.02 /* , -100.0 */ ).xy;
			
			nor = normalize(float3(bmp.x-.5, 1.0, bmp.y-.5));
			ldir = normalize(light - pos);
			float bri = max(dot(ldir, nor), 0.0);
			mat = mat * mat * float3(.25, .55, .47) * bri;

			// Do ground FX with pos location...
			float d = dot(pos, pos);
			bri = max(3.0 - (sqrt(d * .005+flicker)), .01);
			
			float3 hearth = float3(0.005, 0.0, 0.0);
			float f = Bump(float3(pos.x, iTime*.024, pos.z) * 21.0);
			f += Bump(float3(pos.x, iTime*.001, pos.z) * 5.3);
			f = pow(abs(f), 15.0) * .0001;
			
			hearth += f * float3(.7, .15, 0.) * max(85.-d, 0.0);
			mat = lerp(hearth, mat, smoothstep(80.0, 180.0, d));
			
			mat *= float3(1.0, .3, 0.1) * bri;
			col = lerp(col, mat, min((ray.y*ray.y) * 146.0, 1.0));
		}
		else
		{
//			// Wolf eyes...
			float lum  = max(sin(iTime*.5 - 1.2), 0.0);
			float eye1 = max( dot(normalize(float3(-.04,  .05, -1.0)), normalize(ray* float3(1.0, 3.0, 1.0)))-.99994, 0.0);
			float eye2 = max( dot(normalize(float3(-.085, .05, -1.0)), normalize(ray* float3(1.0, 3.0, 1.0)))-.99994, 0.0);
			float f = sin(iTime*.33-1.0);
			eye2 *= smoothstep(0.1, .0, f) + smoothstep(0.1, .2, f);
			col.x = clamp(col.x + (eye1+eye2)*5000.0 * lum, .0, 1.0);
		}
	}
	
	col += FlameColour(ret.w);
	
	// Contrasts...
	col = (1.0-exp(-col*2.0))*1.15;
	col = sqrt(col);	
	
	//col = min(lerp(float3(length(col)),col, 1.1), 1.0);
	// Vignette...
	col *= 0.5 + 0.5 * pow(150.0*uv.x*uv.y*(1.0-uv.x)*(1.0-uv.y), .5 );	
	
	return float4(col,1.0);	
}
