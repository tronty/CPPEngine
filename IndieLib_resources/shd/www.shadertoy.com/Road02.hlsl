/*
  Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
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
	Out.position = mul(worldViewProj, float4(In.position.x, In.position.y, In.position.z, 1.0));
	Out.uv.x = In.uv.x;Out.uv.y = 1.0-In.uv.y;
	return Out;
}

[Fragment shader]
// https://www.shadertoy.com/view/MlBBWm
// Road02
float3 mod(float3 x, float y)
{
  //return x;
  return x - y * floor(x / y); // ???
}

float snoise(float3 uv, float res)
{
	//return 1;
	const float3 s = float3(1e0, 1e2, 1e3);
	
	uv *= res;
	
	float3 uv0 = floor(mod(uv, res))*s;
	float3 uv1 = floor(mod(uv+float3(1,1,1), res))*s;
	
	float3 f = fract(uv); f = f*f*(3.0-2.0*f);

	float4 v = float4(uv0.x+uv0.y+uv0.z, uv1.x+uv0.y+uv0.z,
		      	  uv0.x+uv1.y+uv0.z, uv1.x+uv1.y+uv0.z);

	float4 r = fract(sin(v*1e-1)*1e3);
	float r0 = mix(mix(r.x, r.y, f.x), mix(r.z, r.w, f.x), f.y);
	
	r = fract(sin((v + uv1.z - uv0.z)*1e-1)*1e3);
	float r1 = mix(mix(r.x, r.y, f.x), mix(r.z, r.w, f.x), f.y);
	
	return mix(r0, r1, f.z)*2.-1.;
}

const float2 iResolution=float2(1, 1);
float iTime=0.0;
const float speed = 16.0;

// Maps a position in the world to a position on the road

float3 pos_road_map(float3 pos)
{
	float z = pos.z;

	float x = pos.x +
		0.4 * cos(z * 0.1) +
		1.2 * cos(z * 0.01) +
		3.0 * sin((z + pos.x) * 0.01);

	float y =
		1.3 * cos(z * 0.007) * sin(z * 0.08) +
		0.25 * sin(z * 0.03 + 0.3) +
		0.1 * cos(x * 0.0348 + 0.24) +
		0.3 * sin(x * 0.1 + z * 0.2);

	return float3(x, y, z);
}

float3 normal_road_map(float3 pos)
{
	const float eps = 0.02;

	float3 dir_z = pos_road_map(float3(pos.xy, pos.z - eps)) - pos_road_map(float3(pos.xy, pos.z + eps));
	float3 dir_x = pos_road_map(float3(pos.x - eps, pos.yz)) - pos_road_map(float3(pos.x + eps, pos.yz));
	float3 normal = cross(dir_z, dir_x);

	return normalize(normal);
}

float3 col_road_map(float3 pos)
{
	float3 normal = normal_road_map(pos);

	float3 dir_light = normalize(float3(
		0.5,
		1.0 + sin(iTime * 0.134),
		4.0 * (1.0 + cos(iTime * 0.1))));

	float mix_light = clamp(dot(normal, dir_light), 0.0, 1.0);
	float3 col_light = float3(0.9, 0.75, 0.3);

	float3 col_stripe = float3(0.2, 0.2, 0.2);
	float mix_stripe = step(2.0, mod(pos.z, 4.0)) * 0.1;

	float3 col_ground = float3(0.135, 0.2, 0.1) +
		col_stripe * mix_stripe +
		col_light * mix_light;

	float midline = abs(pos_road_map(pos).x);

	float mix_road = 1.0 - smoothstep(0.0, 0.01, pow(midline, 16.0));
	float3 col_road = float3(0.2, -0.1, 0.2);
	col_ground = mix(col_ground, col_road, mix_road * 0.7);

	float mix_line = (1.0 - smoothstep(0.0, 0.0005, pow(midline, 2.5))) * smoothstep(0.2, 0.3, abs(mod(pos.z, 1.5))-0.75);
	float3 col_line = float3(0.7, 0.7, 0.7);
	col_ground = mix(col_ground, col_line, mix_line * 0.7);

	return col_ground;
}

float3 col_sky(float2 dxy_frag)
{
    float2 uv = dxy_frag.xy / iResolution.xy;
    return float3(0.4, 0.6, 0.9 * uv.y);
}

bool intersect_ray_road(float3 pos_origin, float3 dir, out float3 pos_intersect)
{
	//return true;
    	//return false;
	const float t_min = 0.001;
	const float t_max = 100.0;
	const float dt = 1.0;
	float y_road = 0.0;
	float y_ray = 0.0;

	for (float t = t_min; t < t_max; t += dt)
	{
		float3 pos_ray = pos_origin + dir * t;
		float3 pos_road = pos_road_map(pos_ray);
		if (pos_ray.y < pos_road.y)
		{
			// interpolate to the value of t where the ray actually intersects
			// the road
			
			t = (t - dt) + dt * (y_road - y_ray) / ((pos_ray.y - y_ray) - (pos_road.y - y_road));

			pos_intersect = pos_origin + dir * t;
			return true;
		}
		y_road = pos_road.y;
		y_ray = pos_ray.y;
	}
	return false;
}

float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
	const float fov = radians(50.0);
	const float tan_hfov = tan(fov * 0.5);
	const float near = 0.1;
	const float far = 1.0;

    float2 uv = (fragCoord - 0.5 * iResolution.xy) / iResolution.y;

	float3 pos_road_cam = pos_road_map(float3(0, 0, speed * iTime));

	float len_car = 1.5;
	float h_car = 0.5;

	float x_next = -pos_road_map(float3(0, 0, speed * iTime + len_car * 4.0)).x;

	float3 pos_cam = float3(
		0.35-pos_road_cam.x,
		pos_road_cam.y + h_car,
		pos_road_cam.z - len_car);

	float swivel = (x_next - pos_cam.x) * 0.07;

	float3 dir_ray = normalize(
		float3(
			tan_hfov * uv.x + swivel,
			tan_hfov * uv.y - 0.05,
			1.0)
		* (far - near));
	

	float3 pos_intersect;
	#if 0
	intersect_ray_road(pos_cam, dir_ray, pos_intersect);
    	fragColor = float4(col_road_map(pos_intersect), 1.0);
    	#else
    	if (intersect_ray_road(pos_cam, dir_ray, pos_intersect))
	{
		fragColor = float4(col_road_map(pos_intersect), 1.0);
	}
	else
	{
		fragColor = float4(col_sky(fragCoord.xy), 1.0);
	}
	#endif
	//return float4(1,0,0,1);
	return fragColor;
}

