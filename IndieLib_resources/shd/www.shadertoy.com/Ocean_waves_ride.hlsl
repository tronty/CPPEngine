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
ROW_MAJOR 
float4x4 worldViewProj 
MVPSEMANTIC
;
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
float3      iResolution=float3(1,1,1);                  // viewport iResolution (in pixels)
float     iTime=0;                        // shader playback iTime (in seconds)
float2      iMouse=float2(1,1);                       // iMouse pixel coords
sampler2D iChannel0;
sampler2D iChannel1;
sampler2D iChannel2;

// https://www.shadertoy.com/view/sdcSRH

const float PARALLAX_DEPTH = 2.8;
const int   PARALLAX_STEPS = 4; // 32 ???
const float	PI_OVER_180 = 3.141582 / 180.0;
const float CAMERA_VERTICAL_FOV = 60.0;
const float3  SUN_DIRECTION = normalize(float3(1.0,1.0,1.0));
const float SEABED_DEPTH = -1.0;
const float3  WATER_COLOR = float3(0.4,0.7,1.0)*0.5;

float3 camera_ray(in float3 pos, in float y_angle_deg, in float x_angle_deg, in float z_angle_deg, in float fov_y_deg, in float2 coord, in float3 iResolution)
{
    coord = coord / iResolution.xy * 2.0 - float2(1.0,1.0);
    
    float aspect = iResolution.x / iResolution.y;
    float tan_fov = sin(fov_y_deg*PI_OVER_180*0.5) / cos(fov_y_deg*PI_OVER_180*0.5);
    
    float y = coord.y * tan_fov;
    float x = coord.x * tan_fov * aspect;
    float z = 1.0;
    
    float xa = x_angle_deg * PI_OVER_180, ya = y_angle_deg * PI_OVER_180, za = z_angle_deg * PI_OVER_180;
    
    float xs = sin(xa), xc = cos(xa);
    float ys = sin(za), yc = cos(za);
    float zs = sin(ya), zc = cos(ya);
    
    float3x3 xr = float3x3(
    	1.0, 0.0, 0.0,
		0.0,  xc,  xs,
		0.0, -xs,  xc);
    
    float3x3 yr = float3x3(
		 yc, 0.0,  ys,
		0.0, 1.0, 0.0,
		-ys, 0.0,  yc);
    
    float3x3 zr = float3x3(
		 zc,  zs, 0.0,
		-zs,  zc, 0.0,
		0.0, 0.0, 1.0);
   
    return normalize(mul(zr, mul(yr, mul(xr, float3(x,y,z)))));
}

float noise(float x)
{
    return sin(x*0.4) * sin(x*0.05 + 0.45);
}

float get_foam_for_position(in float2 pos)
{
    // foam perturbed coordinates
    float2 foam_uv = pos;
    
    float pos_noise = abs(sin(pos.x*1.0)*sin(pos.y*1.0));
    
    float fx = pos.y;
    float fy = pos.x;
    
    foam_uv.x += cos(fx*2.0 + iTime*1.0)*0.17;
    foam_uv.y += sin(fy*2.0 + iTime*1.0)*0.17;
    
    // process blob tex2D to produce foam-like pattern
    float foam = tex2D(iChannel0, foam_uv).x;
    foam = 1.0 - foam;
    foam = min( foam*1.2, 1.0 );
    foam = pow( foam, 4.0 );
    foam = min( foam*1.5, 1.0 );
    
    return foam;
}

float4 get_water_surface(in float2 pos, in float sdf)
{
    // coastal pulse wave
    float wave_pulse = sdf + iTime * 0.2;
    wave_pulse = frac(wave_pulse);
    
    // construct wave shape
    float wave_front = pow(wave_pulse, 4.0);
    float wave_back = pow(1.0 - wave_pulse, 1.0);
    
    float wave_height = max(wave_front, wave_back);
    wave_height = wave_front + wave_back;

	float foam = get_foam_for_position(pos);
    
    //foam *= abs(sin(pos.x/2.0));
    
    float wave = max( 0.0, pow(wave_height,8.0)*foam );
    
    float noise1 = abs(sin(pos.x*4.0));
    float noise2 = abs(sin(pos.x*0.017));
    float noise3 = pow(abs(sin(pos.x*2.25)), 0.25);
    
    float front_mask = wave_front - 0.5;
    front_mask -= 0.1*noise1;
    front_mask -= 0.1*noise2;
    front_mask -= 0.2*noise3;
    
    front_mask = max(0.0, front_mask);
    
    foam *= front_mask*8.0 + pow(wave_back, 8.0);
    
    foam  = lerp(foam, 0.7, pow(wave_height,16.0));
    
    foam *= abs(sin(pos.x/2.0));
    
    float3 color = float3(foam, foam, foam);
    return float4(color, wave_height);
}

float sdf(in float3 pos)
{
    float v = abs(sin(pos.x*17.0));
    v = pow(v, 128.0);

    return pos.z * 0.125 - 100.0 + sin(pos.x/5.0)*0.2 + sin(pos.x/1.0)*0.03 + sin(pos.x*32.0)*v*0.01;
}

float3 compute_parallax_waves(in float3 eye, in float3 ray, in float time, out float3 intersect_pos)
{
    //eye.xz *= 0.125;
    //ray.xz *= 0.125;

    float3 pos_bottom = eye + ray * eye.y / -ray.y;
    
    float depth_fade = length(eye.xz - pos_bottom.xz)/50.0;
    float parallax_depth = PARALLAX_DEPTH;// * (1.0 - min( 1.0, depth_fade ) );
    
    //parallax_depth *= 1.0 + sin(pos_bottom.x/5.0)*sin(pos_bottom.z/5.0)*0.5;
    
    float3 pos_top = pos_bottom - ray * parallax_depth / -ray.y;
    
    //pos.y += noise(pos.x);
    //pos2.y += noise(pos2.x);
    
    float3 pos = pos_top;
    float3 pstep = (pos_bottom - pos_top) * (1.0/float(PARALLAX_STEPS));
    
    float4 wave;
    
    wave = get_water_surface(pos_bottom.xz, sdf(pos_bottom));
    wave = get_water_surface(pos_top.xz, sdf(pos_top));
    
    //wave.x = wave.y = wave.z = frac(pos_bottom.z*0.25);
    //return wave.xyz;
    
    float4 prev_wave = wave;
    float prev_delta = 0.0;
    
    for(int i=0; i<PARALLAX_STEPS; ++i)
    {
        wave = get_water_surface(pos.xz, sdf(pos));
        wave.w *= parallax_depth;
        
        float delta = wave.w - pos.y;
        
        if(delta > 0.0)
        {
            pos += pstep * delta / (prev_delta - delta);
            
            intersect_pos = pos;
            
            wave = get_water_surface(pos.xz, sdf(pos));
            return wave.xyz;
        }
        pos += pstep;
        prev_wave = wave;
        prev_delta = delta;
    }
    intersect_pos = pos_bottom;
    return wave.xyz;
}

float detail_surface(in float3 pos)
{
    pos.x += sin(pos.z*20.5 + iTime)*0.043;
    pos.z += cos(pos.x*20.5 + iTime)*0.043;

    return 1.0 - abs(sin(pos.x*6.0)*sin(pos.z*6.0));
}

float3 get_surface_normal(in float3 pos, in float3 eye)
{
    float h00 = get_water_surface(pos.xz, sdf(pos)).w;
    float3  pos10 = pos + float3(0.1,0.0,0.0);
    float h10 = get_water_surface(pos10.xz, sdf(pos10)).w;
    float3  pos01 = pos + float3(0.0,0.0,0.1);
    float h01 = get_water_surface(pos01.xz, sdf(pos01)).w;
    
    float ds00 = detail_surface(pos);
    float ds10 = detail_surface(pos + float3(0.1, 0.0, 0.0));
    float ds01 = detail_surface(pos + float3(0.0, 0.0, 0.1));
    
    float dist = length(pos - eye);
    
    float detail_weight = lerp(0.008, 0.0, min( 1.0, dist/20.0));
    
    h10 += (ds10 - ds00)*detail_weight;
    h01 += (ds01 - ds00)*detail_weight;
    
    return normalize(float3(h10 - h00, 0.1, h01-h00));
}

float3 get_sea_bottom(in float3 eye, in float3 ray)
{
    float3 pos = eye + ray * (eye.y - SEABED_DEPTH) / -ray.y;
   
    float3 color = tex2D(iChannel1, pos.xz * 0.2).xyz;
    color *= float3(0.6, 0.25, 0.05);
    color = lerp(WATER_COLOR, color, exp(-length(pos - eye)*0.2));
    
    return color;
}

float3 refract_water(in float3 ray, in float3 N)
{
    return normalize(ray - N*0.3);  // quite fake, but cheap
}

float3 get_skybox(in float3 ray)
{
    float3 cbm = tex2D(iChannel2, ray.xyz).xyz;
    cbm = lerp(WATER_COLOR, cbm, clamp((ray.y - 0.05)*15.0, 0.0, 1.0));
    return cbm;
}

float4 main(VsOut IN): COLOR
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
    float a = iTime * 1.5;
    float R = 0.0;
    float3 eye = float3(iTime * 5.0, 5.0, 0.0);
    float3 angles = float3(-35.0, 0.0, 65.0);
    
    //float3     camera_ray(in float3 pos, in float y_angle_deg, in float x_angle_deg, in float z_angle_deg, in float fov_y_deg, in float2 coord, in float3 iResolution)
    float3 ray = camera_ray(eye, angles.y, angles.x, angles.z, CAMERA_VERTICAL_FOV, fragCoord, iResolution);
    
    float3 color;
    if( ray.y < 0.0 )
    {
        float3 intersect_pos;
        color = compute_parallax_waves(eye, ray, iTime, intersect_pos);
        float3 N = get_surface_normal(intersect_pos, eye);
        
        float3 wray = refract_water(ray, N);
        float3 sea_bottom = get_sea_bottom(intersect_pos, wray);
        
        float3 reflection = get_skybox(reflect(ray, N));
        float reflectivity = 1.0 - abs(dot(N, -ray));
        
        reflectivity = pow(reflectivity, 3.0);
        
        sea_bottom = lerp(sea_bottom, reflection, reflectivity);
       
        color += sea_bottom;
    }
    else
    {
        color = float3(0.0, 0.0, 0.0);
    }
    return float4(color,1.0);
}
