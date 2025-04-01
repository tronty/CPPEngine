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
float iTime=0.0;
#define MAX_DIST 10.0
#define MAX_STEPS 100
#define EPSILON 0.0001

float boundingBoxSDF( float3 p, float3 b, float e ) {
   p = abs(p)-b;
  float3 q = abs(p+e)-e;
  return min(min(
      length(max(float3(p.x,q.y,q.z),0.0))+min(max(p.x,max(q.y,q.z)),0.0),
      length(max(float3(q.x,p.y,q.z),0.0))+min(max(q.x,max(p.y,q.z)),0.0)),
      length(max(float3(q.x,q.y,p.z),0.0))+min(max(q.x,max(q.y,p.z)),0.0));
}
float sphereSDF( float3 pos, float radius) {
	return length(pos) - radius;
}

float mandelbulbSDF( float3 p, float power ) {
	float3 w = p;
    float m = dot(w,w);
	float dz = 1.0;

	for( int i=0; i<3; i++ )
    {
        dz = power*pow(sqrt(m), power - 1.0 )*dz + 1.0;

        float r = length(w);
        float b = power*acos( w.y/r);
        float a = power*atan2( w.x, w.z );
        w = p + pow(r,power) * float3( sin(b)*sin(a), cos(b), sin(b)*cos(a) );

        m = dot(w,w);
		if( m > 256.0 )
            break;
    }

    return 0.25*log(m)*sqrt(m)/dz;
}

float sceneSDF( float3 pos ) {
	#if 1
 	return sphereSDF( pos, 4.0);
 	#elif 0
  float sphere = sphereSDF(pos, 40.0);
  float box = boundingBoxSDF(pos, float3( 30.0, 30.0, 30.0 ), 5.0);
  return lerp( sphere, box, 0.5 + sin(iTime)/2 );
 	#else
    return mandelbulbSDF( pos, 7.0 );
    #endif
}

float3 estimateNormals(float3 pos) {
  return normalize( float3(
  	sceneSDF(float3( pos.x + EPSILON, pos.yz ) )
    - sceneSDF(float3( pos.x - EPSILON, pos.yz) ),
    sceneSDF( float3( pos.x, pos.y + EPSILON, pos.z) )
    - sceneSDF( float3( pos.x, pos.y - EPSILON, pos.z ) ),
    sceneSDF( float3(pos.xy, pos.z + EPSILON) )
    - sceneSDF( float3( pos.xy, pos.z - EPSILON ) )
  ));
}

// Parameters: Camera Position and Ray Direction
// Output: Distance to closest surface along ray
float raymarch( float3 camPos, float3 rayDir )
{
    float depth = 0;
    for ( int i = 0; i < MAX_STEPS; i++ ) {
      	// Calculate next position along view ray
        float3 pos = camPos + ( rayDir * depth );
        // Calcualte shortest distance to scene at position
        float dist = sceneSDF( pos );
        depth += dist;
      	// finish when very close to a surface or when ray has travelled to far
        if ( dist <= EPSILON || depth > MAX_DIST ) {
            break;
        }
    }
    return depth;
}

float4 main( float2 fragCoord : TEXCOORD0) : COLOR0
{
	float3 CameraPosition=float3(0,0,MAX_DIST*0.9);
	float3 RayDirection=float3(0,0,-1);
	float depth = raymarch( CameraPosition, RayDirection );	
	// if no surface was hit then return black
	if (depth >= MAX_DIST) {
	  return float4( 0.0, 0.0, 0.0, 0.0 );
	}
	#if 0
	// otherwise return white
	return float4( 1.0, 1.0, 1.0, 1.0 );
	#elif 1
	// visualize the estimated normals
	float3 normal = estimateNormals( CameraPosition + (RayDirection * depth));
	return float4( normal, 1.0 );
	#else
	float3 lightDir = normalize( float3( -0.3, -1.0, -0.3 ));	
	float3 normal = estimateNormals( CameraPosition + RayDirection*depth);	
	// the diffuse color of the surface
	float3 diffuseColor = float3( 0.8, 0.2, 0.0 );	
	// calculate the amount of light hitting the surface
	float diffuseLight = clamp( dot( -lightDir, normal ), 0, 1 );	
	// amount of ambient light applied to entire surface
	float ambientLight = 0.2;	
	// calculate specular reflections
	float3 reflectDir = reflect( -lightDir, normal );
	float specular = pow( clamp( dot( reflectDir, RayDirection ), 0.0, 1.0 ), 16.0 );	
	float3 result = (diffuseLight + ambientLight) * diffuseColor + specular;
	return float4(result, 1.0);
	#endif
}

