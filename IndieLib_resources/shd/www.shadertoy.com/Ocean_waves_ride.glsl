uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;

const float PARALLAX_DEPTH = 2.8;
const int   PARALLAX_STEPS = 32;
const float	PI_OVER_180 = 3.14159265359 / 180.0;
const float CAMERA_VERTICAL_FOV = 60.0;
const vec3  SUN_DIRECTION = normalize(vec3(1.0,1.0,1.0));
const float SEABED_DEPTH = -1.0;
const vec3  WATER_COLOR = vec3(0.4,0.7,1.0)*0.5;

vec3 camera_ray(in vec3 pos, in float y_angle_deg, in float x_angle_deg, in float z_angle_deg, in float fov_y_deg, in vec2 coord, in vec3 iResolution)
{
    coord = coord / iResolution.xy * 2.0 - vec2(1.0,1.0);
    
    float aspect = iResolution.x / iResolution.y;
    float tan_fov = sin(fov_y_deg*PI_OVER_180*0.5) / cos(fov_y_deg*PI_OVER_180*0.5);
    
    float y = coord.y * tan_fov;
    float x = coord.x * tan_fov * aspect;
    float z = 1.0;
    
    float xa = x_angle_deg * PI_OVER_180, ya = y_angle_deg * PI_OVER_180, za = z_angle_deg * PI_OVER_180;
    
    float xs = sin(xa), xc = cos(xa);
    float ys = sin(za), yc = cos(za);
    float zs = sin(ya), zc = cos(ya);
    
    mat3 xr = mat3(
    	1.0, 0.0, 0.0,
		0.0,  xc,  xs,
		0.0, -xs,  xc);
    
    mat3 yr = mat3(
		 yc, 0.0,  ys,
		0.0, 1.0, 0.0,
		-ys, 0.0,  yc);
    
    mat3 zr = mat3(
		 zc,  zs, 0.0,
		-zs,  zc, 0.0,
		0.0, 0.0, 1.0);
   
    return normalize(vec3(x,y,z) * xr * yr * zr );
}

float noise(float x)
{
    return sin(x*0.4) * sin(x*0.05 + 0.45);
}

float get_foam_for_position(in vec2 pos)
{
    // foam perturbed coordinates
    vec2 foam_uv = pos;
    
    float pos_noise = abs(sin(pos.x*1.0)*sin(pos.y*1.0));
    
    float fx = pos.y;
    float fy = pos.x;
    
    foam_uv.x += cos(fx*2.0 + iTime*1.0)*0.17;
    foam_uv.y += sin(fy*2.0 + iTime*1.0)*0.17;
    
    // process blob texture to produce foam-like pattern
    float foam = texture(iChannel0, foam_uv).x;
    foam = 1.0 - foam;
    foam = min( foam*1.2, 1.0 );
    foam = pow( foam, 4.0 );
    foam = min( foam*1.5, 1.0 );
    
    return foam;
}

vec4 get_water_surface(in vec2 pos, in float sdf)
{
    // coastal pulse wave
    float wave_pulse = sdf + iTime * 0.2;
    wave_pulse = fract(wave_pulse);
    
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
    
    foam  = mix(foam, 0.7, pow(wave_height,16.0));
    
    foam *= abs(sin(pos.x/2.0));
    
    vec3 color = vec3(foam, foam, foam);
    return vec4(color, wave_height);
}

float sdf(in vec3 pos)
{
    float v = abs(sin(pos.x*17.0));
    v = pow(v, 128.0);

    return pos.z * 0.125 - 100.0 + sin(pos.x/5.0)*0.2 + sin(pos.x/1.0)*0.03 + sin(pos.x*32.0)*v*0.01;
}

vec3 compute_parallax_waves(in vec3 eye, in vec3 ray, in float time, out vec3 intersect_pos)
{
    //eye.xz *= 0.125;
    //ray.xz *= 0.125;

    vec3 pos_bottom = eye + ray * eye.y / -ray.y;
    
    float depth_fade = length(eye.xz - pos_bottom.xz)/50.0;
    float parallax_depth = PARALLAX_DEPTH;// * (1.0 - min( 1.0, depth_fade ) );
    
    //parallax_depth *= 1.0 + sin(pos_bottom.x/5.0)*sin(pos_bottom.z/5.0)*0.5;
    
    vec3 pos_top = pos_bottom - ray * parallax_depth / -ray.y;
    
    //pos.y += noise(pos.x);
    //pos2.y += noise(pos2.x);
    
    vec3 pos = pos_top;
    vec3 pstep = (pos_bottom - pos_top) * (1.0/float(PARALLAX_STEPS));
    
    vec4 wave;
    
    wave = get_water_surface(pos_bottom.xz, sdf(pos_bottom));
    wave = get_water_surface(pos_top.xz, sdf(pos_top));
    
    //wave.x = wave.y = wave.z = fract(pos_bottom.z*0.25);
    //return wave.xyz;
    
    vec4 prev_wave = wave;
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

float detail_surface(in vec3 pos)
{
    pos.x += sin(pos.z*20.5 + iTime)*0.043;
    pos.z += cos(pos.x*20.5 + iTime)*0.043;

    return 1.0 - abs(sin(pos.x*6.0)*sin(pos.z*6.0));
}

vec3 get_surface_normal(in vec3 pos, in vec3 eye)
{
    float h00 = get_water_surface(pos.xz, sdf(pos)).w;
    vec3  pos10 = pos + vec3(0.1,0.0,0.0);
    float h10 = get_water_surface(pos10.xz, sdf(pos10)).w;
    vec3  pos01 = pos + vec3(0.0,0.0,0.1);
    float h01 = get_water_surface(pos01.xz, sdf(pos01)).w;
    
    float ds00 = detail_surface(pos);
    float ds10 = detail_surface(pos + vec3(0.1, 0.0, 0.0));
    float ds01 = detail_surface(pos + vec3(0.0, 0.0, 0.1));
    
    float dist = length(pos - eye);
    
    float detail_weight = mix(0.008, 0.0, min( 1.0, dist/20.0));
    
    h10 += (ds10 - ds00)*detail_weight;
    h01 += (ds01 - ds00)*detail_weight;
    
    return normalize(vec3(h10 - h00, 0.1, h01-h00));
}

vec3 get_sea_bottom(in vec3 eye, in vec3 ray)
{
    vec3 pos = eye + ray * (eye.y - SEABED_DEPTH) / -ray.y;
   
    vec3 color = texture(iChannel1, pos.xz * 0.2).xyz;
    color *= vec3(0.6, 0.25, 0.05);
    color = mix(WATER_COLOR, color, exp(-length(pos - eye)*0.2));
    
    return color;
}

vec3 refract_water(in vec3 ray, in vec3 N)
{
    return normalize(ray - N*0.3);  // quite fake, but cheap
}

vec3 get_skybox(in vec3 ray)
{
    vec3 cbm = texture(iChannel2, ray.xyz).xyz;
    cbm = mix(WATER_COLOR, cbm, clamp((ray.y - 0.05)*15.0, 0.0, 1.0));
    return cbm;
}

void main( )
{
    float a = iTime * 1.5;
    float R = 0.0;
    vec3 eye = vec3(iTime * 5.0, 5.0, 0.0);
    vec3 angles = vec3(-35.0, 0.0, 65.0);
    
    vec3 ray = camera_ray(eye, angles.y, angles.x, angles.z, CAMERA_VERTICAL_FOV, xlv_TEXCOORD0, iResolution);
    
    vec3 color;
    if( ray.y < 0.0 )
    {
        vec3 intersect_pos;
        color = compute_parallax_waves(eye, ray, iTime, intersect_pos);
        vec3 N = get_surface_normal(intersect_pos, eye);
        
        vec3 wray = refract_water(ray, N);
        vec3 sea_bottom = get_sea_bottom(intersect_pos, wray);
        
        vec3 reflection = get_skybox(reflect(ray, N));
        float reflectivity = 1.0 - abs(dot(N, -ray));
        
        reflectivity = pow(reflectivity, 3.0);
        
        sea_bottom = mix(sea_bottom, reflection, reflectivity);
       
        color += sea_bottom;
    }
    else
    {
        color = vec3(0.0, 0.0, 0.0);
    }
    gl_FragColor = vec4(color,1.0);
}
