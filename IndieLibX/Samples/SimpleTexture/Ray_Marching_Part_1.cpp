#include <Framework3/IRenderer.h>

#define STX_PRINT(...) printf(__VA_ARGS__)
#define STX_FNLN printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__)
/*
  Copyright (c) 2021 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

float iTime=0;
const float2 iResolution=float2(1,1);//800,600);
float2 iMouse=float2(1,1);
//sampler2D iChannel0;
float mod(float x, float y)
{
  return x - y * floor(x / y);
}

/**
 * Part 1 Challenges
 * - Make the circle yellow
 * - Make the circle smaller by decreasing its radius
 * - Make the circle smaller by moving the camera back
 * - Make the size of the circle oscillate using the sin() function and the iTime
 *   uniform provided by shadertoy
 */

int MAX_MARCHING_STEPS = 255;
float MIN_DIST = 0.0;
float MAX_DIST = 100.0;
float _EPSILON_ = 0.0001;

/**
 * Signed distance function for a sphere centered at the origin with radius 1.0;
 */
float sphereSDF(float3 samplePoint) {
    return length(samplePoint) - 0.25;
}

/**
 * Signed distance function describing the scene.
 * 
 * Absolute value of the return value indicates the distance to the surface.
 * Sign indicates whether the point is inside or outside the surface,
 * negative indicating inside.
 */
float sceneSDF(float3 samplePoint) {
    return sphereSDF(samplePoint);
}

/**
 * Return the shortest distance from the eyepoint to the scene surface along
 * the marching direction. If no part of the surface is found between start and end,
 * return end.
 * 
 * eye: the eye point, acting as the origin of the ray
 * marchingDirection: the normalized direction to march in
 * start: the starting distance away from the eye
 * end: the max distance away from the ey to march before giving up
 */
float shortestDistanceToSurface(float3 eye, float3 marchingDirection, float start, float end) {
    float depth = start;
    for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
        float dist = sceneSDF(eye + depth * marchingDirection);
        if (dist < _EPSILON_) {
	STX_FNLN;
			return depth;
        }
        depth += dist;
        if (depth >= end) {
	STX_FNLN;
            return end;
        }
    }
	STX_FNLN;
    return end;
}

/**
 * Return the normalized direction to march in from the eye point for a single pixel.
 * 
 * fieldOfView: vertical field of view in degrees
 * size: resolution of the output image
 * fragCoord: the x,y coordinate of the pixel in the output image
 */
float3 rayDirection(float fieldOfView, float2 size, float2 fragCoord) {
    float2 xy = fragCoord - size / 2.0;
    float z = size.y / tan(fieldOfView / 2.0);
    return normalize(float3(xy, -z));
}


float4 main_(float2 fragCoord)// : TEXCOORD0 ) : COLOR0
{
	float3 dir = rayDirection(3.14/4.0, float2(iResolution.x, iResolution.y), fragCoord);
    float3 eye = float3(0.0, 0.0, 1.0);
    float dist = shortestDistanceToSurface(eye, dir, MIN_DIST, MAX_DIST);
    STX_PRINT("shortestDistanceToSurface: dist=%f\n", dist);
    STX_PRINT("MAX_DIST=%f\n, _EPSILON_=%f\n", MAX_DIST, _EPSILON_);
    STX_PRINT("MAX_DIST - _EPSILON_=%f\n", MAX_DIST - _EPSILON_);
    if (dist > (MAX_DIST - _EPSILON_)) {
        // Didn't hit anything
	STX_FNLN;
        return float4(1.0, 0.0, 0.0, 1.0);		
    }
	STX_FNLN;
    return float4(0.0, 1.0, 0.0, 1.0);
}
int ApplicationLogic()
{
	STX_FNLN;
	IRenderer* r=IRenderer::GetRendererInstance("SimpleTexture");
	STX_FNLN;
	IInput*    i=STX_Service::GetInputInstance();
	STX_FNLN;
	float2 fragCoord(0.0,0.0);
	float4 v4=main_(fragCoord);
	stx_Dump4f(v4);
	fragCoord=float2(0.5,0.5);
	v4=main_(fragCoord);
	stx_Dump4f(v4);
	fragCoord=float2(1.0,1.0);
	v4=main_(fragCoord);
	stx_Dump4f(v4);
	STX_FNLN;
	return 0;
}

