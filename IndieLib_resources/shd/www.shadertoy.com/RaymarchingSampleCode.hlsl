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
// https://www.shadertoy.com/view/XsB3Rm
// Raymarching Sample Code
const float2 iMouse=float2(1, 1);
const float2 iResolution=float2(1, 1);
float iTime=0.0;
sampler2D iChannel0;

float     iGlobalTime=0;                  // shader playback time (in seconds)
float4      iDate=float4(0,0,0,0);                        // (year, month, day, time in seconds)
float     iSampleRate=1;                  // sound sample rate (i.e., 44100)
float3      iChannelResolution[4];        // channel resolution (in pixels)
float     iChannelTime[4];              // channel playback time (in sec)

float2      ifFragCoordOffsetUniform=float2(0,0);     // used for tiled based hq rendering
float     iTimeDelta=0;                   // render time (in seconds)
int       iFrame=1;                       // shader playback frame
float     iFrameRate=1;

struct Channel {
    float3  resolution;
    //float time;
};

// ray marching
const int max_iterations = 512;
const float stop_threshold = 0.001;
const float grad_step = 0.02;
const float clip_far = 1000.0;

// math
const float PI = 3.14159265359;
#define DEG_TO_RAD PI / 180.0

// iq's distance function
float sdSphere( float3 pos, float r ) {
	return length( pos ) - r;
}

float sdBox( float3 p, float3 b ) {
  float3 d = abs(p) - b;
  return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));
}


float sdUnion( float d0, float d1 ) {
    return min( d0, d1 );
}

float sdInter( float d0, float d1 ) {
    return max( d0, d1 );
}

float sdSub( float d0, float d1 ) {
    return max( d0, -d1 );
}

float sdUnion_s( float a, float b, float k ) {
    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) - k*h*(1.0-h);
}

float sfDisp( float3 p ) {
    return sin(p.x)*sin(p.y)*sin(p.z) ;
}

float3 sdTwist( float3 p, float a ) {
    float c = cos(a*p.y);
    float s = sin(a*p.y);
    float3x3 m = float3x3(c,-s, 0,s,c, 0, 0, 0, 0);
	float3 p_ = mul(m,p);
    return float3(p_.xz,p.y);
}

float3 sdRep( float3 p, float3 c ) {
    return modf(p,c)-0.5*c;
}

// get distance in the world
float dist_field( float3 p ) {
//  p = sdRep( p, float3( 4.0 ) );
//  p = sdTwist( p, 3.0 );
    
    float d0 = sdBox( p, float3(0.5, 0.5, 0.5) );
    float d1 = sdSphere( p, 0.6 );
    
    float d = sdInter( d1, d0 );

    return d;
    //return d + sfDisp( p * 2.5 );
    //return sdUnion_s( d + sfDisp( p * 2.5 * sin( iTime * 1.01 ) ), d1, 0.1 );
}

// get gradient in the world
float3 gradient( float3 pos ) {
	const float3 dx = float3( grad_step, 0.0, 0.0 );
	const float3 dy = float3( 0.0, grad_step, 0.0 );
	const float3 dz = float3( 0.0, 0.0, grad_step );
	return normalize (
		float3(
			dist_field( pos + dx ) - dist_field( pos - dx ),
			dist_field( pos + dy ) - dist_field( pos - dy ),
			dist_field( pos + dz ) - dist_field( pos - dz )			
		)
	);
}

float3 fresnel( float3 F0, float3 h, float3 l ) {
	return F0 + ( 1.0 - F0 ) * pow( clamp( 1.0 - dot( h, l ), 0.0, 1.0 ), 5.0 );
}

// phong shading
float3 shading( float3 v, float3 n, float3 dir, float3 eye ) {
	// ...add lights here...
	
	float shininess = 16.0;
	
	float3 final = float3( 0, 0, 0 );
	
	float3 ref = reflect( dir, n );
    
    float3 Ks = float3( 0.5, 0.5, 0.5 );
    float3 Kd = float3( 1.0, 1.0, 1.0 );
	
	// light 0
	{
		float3 light_pos   = float3( 20.0, 20.0, 20.0 );
		float3 light_color = float3( 1.0, 0.7, 0.7 );
	
		float3 vl = normalize( light_pos - v );
		
		float f31  = max( 0.0, dot( vl, n ) );
		float f32 = max( 0.0, dot( vl, ref ) );
	
		float3 diffuse  = Kd * float3(f31, f31, f31);
		float3 specular = float3(f32, f32, f32);
		
        float3 F = fresnel( Ks, normalize( vl - dir ), vl );
		specular = pow( specular, float3( shininess, shininess, shininess ) );
		
		final += light_color * mix( diffuse, specular, F ); 
	}
	
	// light 1
	{
		float3 light_pos   = float3( -20.0, -20.0, -30.0 );
		float3 light_color = float3( 0.5, 0.7, 1.0 );
	
		float3 vl = normalize( light_pos - v );
	
		float f31  = max( 0.0, dot( vl, n ) );
		float f32 = max( 0.0, dot( vl, ref ) );
	
		float3 diffuse  = Kd * float3(f31, f31, f31);
		float3 specular = float3(f32, f32, f32);
        
        float3 F = fresnel( Ks, normalize( vl - dir ), vl );
		specular = pow( specular, float3( shininess, shininess, shininess ) );
		
		final += light_color * mix( diffuse, specular, F );
	}

    final += tex2D( iChannel0, ref ).rgb * fresnel( Ks, n, -dir );
    
	return final;
}


bool ray_vs_aabb(float3 o, float3 dir, float3 bmin, float3 bmax, inout float2 e ) {
    float3 a = ( bmin - o ) / dir;
    float3 b = ( bmax - o ) / dir;
    
    float3 s = min( a, b );
    float3 t = max( a, b );
    
    e.x = max( max( s.x, s.y ), max( s.z, e.x ) );
    e.y = max( min( t.x, t.y ), max( t.z, e.y ) );
    
    return e.x < e.y;
}

// ray marching
bool ray_marching( float3 o, float3 dir, inout float depth, inout float3 n ) {
	float t = 0.0;
    float d = 10000.0;
    float dt = 0.0;
    for ( int i = 0; i < 128; i++ ) {
        float3 v = o + dir * t;
        d = dist_field( v );
        if ( d < 0.001 ) {
            break;
        }
        dt = min( abs(d), 0.1 );
        t += dt;
        if ( t > depth ) {
            break;
        }
    }
    
    if ( d >= 0.001 ) {
        return false;
    }
    
    t -= dt;
    for ( int i = 0; i < 4; i++ ) {
        dt *= 0.5;
        
        float3 v = o + dir * ( t + dt );
        if ( dist_field( v ) >= 0.001 ) {
            t += dt;
        }
    }
    
    depth = t;
    n = normalize( gradient( o + dir * t ) );
    return true;
}

// get ray direction
float3 ray_dir( float fov, float2 size, float2 pos ) {
	float2 xy = pos - size * 0.5;

	float cot_half_fov = tan( ( 90.0 - fov * 0.5 ) * DEG_TO_RAD ); // ???	
	float z = size.y * 0.5 * cot_half_fov;
	
	return normalize( float3( xy.x, xy.y, -z ) );
}

// camera rotation : pitch, yaw
float4x4 rotationXY( float2 angle ) {
	float2 c = cos( angle );
	float2 s = sin( angle );
	
	return float4x4(
		c.y      ,  0.0, -s.y,  0.0,
		s.y * s.x,  c.x,  c.y * s.x,  0.0,
		s.y * c.x, -s.x,  c.y * c.x,  0.0,
		0.0,  0.0,  0.0,  0.0
	);
}

float4 main( float4 vertexin: POSITION, float2 fragCoord: TEXCOORD0 ) : COLOR
{
	// default ray dir
	float3 dir = ray_dir( 45.0, iResolution.xy, fragCoord.xy );
	
	// default ray origin
	float3 eye = float3( 0.0, 0.0, 3.5 );

	// rotate camera
	float4x4 rot = rotationXY( ( iMouse.xy - iResolution.xy * 0.5 ).yx * float2( 0.01, -0.01 ) );
	dir = mul(rot , float4(dir,1)).xyz;
	eye = mul(rot , float4(eye,1)).xyz;
	
	// ray marching
    float depth = clip_far;
    float3 n = float3( 0, 0, 0 );
	if ( !ray_marching( eye, dir, depth, n ) ) {
		return tex2D( iChannel0, dir );
        return float4(0,0,0,0);
	}
	
	// shading
	float3 pos = eye + dir * depth;
    
    float3 color = shading( pos, n, dir, eye );
    float3 f3=float3(1.0/1.2, 1.0/1.2, 1.0/1.2);
	return float4( pow( color, f3 ), 1.0 );
}

