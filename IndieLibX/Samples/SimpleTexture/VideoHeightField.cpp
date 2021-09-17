/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

//define STX_PRINT(...) printf(__VA_ARGS__)
//define STX_FNLN printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__)


float3      iResolution=float3(1,1,1);                  // viewport resolution (in pixels)
float     iTime=0;                        // shader playback time (in seconds)

/*
 * Copyright 2020 Simon Green (@simesgreen)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

int _Steps = 64;
float3 lightDir = float3(0.577, 0.577, 0.577);

bool intersectBox(float3 ro, float3 rd, float3 boxmin, float3 boxmax, out float tnear, out float tfar)
{
	// compute intersection of ray with all six bbox planes
	float3 invR = 1.0 / rd;
	float3 tbot = invR * (boxmin - ro);
	float3 ttop = invR * (boxmax - ro);
	// re-order intersections to find smallest and largest on each axis
	float3 tmin = min (ttop, tbot);
	float3 tmax = max (ttop, tbot);
	// find the largest tmin and the smallest tmax
	float2 t0 = max (tmin.xx, tmin.yz);
	tnear = max (t0.x, t0.y);
	t0 = min (tmax.xx, tmax.yz);
	tfar = min (t0.x, t0.y);
	// check for hit
	bool hit;
	if ((tnear > tfar)) 
		hit = false;
	else
		hit = true;
	return hit;
}

float2 worldToTex(float3 p)
{
	float2 uv = p.xz*0.5+0.5;
	uv.y = 1.0 - uv.y;
	return uv;
}

float heightField(float3 p)
{
	return sin(p.x*1.0);
	return sin(p.x*1.0)*sin(p.z*1.0);
	//return luminance(iChannel0, worldToTex(p))*0.5;
}

bool traceHeightField(float3 ro, float3 rayStep, out float3 hitPos)
{
	float3 p = ro;
	bool hit = false;
	float pH = 0.0;
	float3 pP = p;
	for(int i=0; i<_Steps; i++) {
		float h = heightField(p);
		if ((p.y < h) && !hit) {
			hit = true;
			//hitPos = p;
			// interpolate based on height
            hitPos = lerp(pP, p, (pH - pP.y) / ((p.y - pP.y) - (h - pH)));
		}
		pH = h;
		pP = p;
		p += rayStep;
	}
	return hit;
}

float3 background(float3 rd)
{
     return lerp(float3(1.0, 1.0, 1.0), float3(0.0, 0.5, 1.0), abs(rd.y));
}

float4 main()
{
	float4 fragColor;
	float2 fragCoord=float2(0.5, 0.5);

    float2 pixel = (fragCoord.xy / iResolution.xy)*2.0-1.0;

    // compute ray origin and direction
    float asp = iResolution.x / iResolution.y;
    float3 rd = normalize(float3(asp*pixel.x, pixel.y, -2.0));
    float3 ro = float3(0.0, 0.0, 2.0);

	// intersect with bounding box
    bool hit;	
	const float3 boxMin = float3(-1.0, -0.01, -1.0);
	const float3 boxMax = float3(1.0, 0.5, 1.0);
	float tnear, tfar;
	hit = intersectBox(ro, rd, boxMin, boxMax, tnear, tfar);

	tnear -= 0.0001;
	float3 pnear = ro + rd*tnear;
    float3 pfar = ro + rd*tfar;
	
    float stepSize = length(pfar - pnear) / float(_Steps);
	
    float3 rgb = background(rd);
    if(hit)
    {
    	// intersect with heightfield
		ro = pnear;
		float3 hitPos;
		hit = traceHeightField(ro, rd*stepSize, hitPos);
		if (hit)
		{
			float2 uv = worldToTex(hitPos);
			//rgb = tex2D(iChannel0, uv).xyz;
			rgb=float3(0,1,0);
		}
     }

    fragColor=float4(rgb, 1.0);
    return fragColor;
}

