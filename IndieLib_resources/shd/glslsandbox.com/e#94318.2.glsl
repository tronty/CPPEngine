/*****************************************************/
/***************** VARIABLES DE CONTROL  *************/
/*****************************************************/
/*********************** INICIO **********************/
/*****************************************************/


// Encendido: 0 -> OFF; 1 -> ON
int v_Encendido = 1;
// Ejecutar: 0 -> STOP; 1 -> PLAY
int v_Ejecutar = 1;
// Movimiento del Sol: 0 -> MANUAL; 1 -> AUTO
int v_Sol_Automatico = 0;
// Posicion del Sol: XXYY -> XX horas, YY centi-horas
int v_Sol_Posicion = 590;
// Escenario: 0 -> posicion fija dentro atmosfera
//			1 -> posicion fija fuera atmosfera
//			2 -> posicion dinamica dentro atmosfera
//			3 -> posicion dinamica fuera atmosfera
int v_Escenario = 2;


/*****************************************************/
/************************ FIN ************************/
/*****************************************************/


/*****************************************************/
/***************** VARIABLES ESPECIALES  *************/
/*****************************************************/
/*********************** INICIO **********************/
/*****************************************************/


const int s_Nivel_Calidad = 1;


/*****************************************************/
/************************ FIN ************************/
/*****************************************************/

/*
#extension GL_OES_standard_derivatives : enable

precision highp float;
*/

#define _betaR vec3(5.5e-6, 13.0e-6, 22.4e-6)
#define _betaM vec3(21e-6)
#define _betaAmb vec3(0.0)
#define _betaAbs vec3(2.04e-5, 4.97e-5, 1.95e-6)
#define G 0.7
#define _heigR 8000.0
#define _heigM 1200.0
#define _heigAbs 30000.0
#define _fallAbs 4000.0
#define Pi 3.14159265359
#define MAX_STEPS 24

vec3 Planeta_Posicion = vec3(0.0);
float Planeta_Radio = 6371000.0;
float Atmosfera_Radio = 6471000.0;
uniform float time;
uniform vec2 resolution;
int Primary_Steps;
int Light_Steps;

vec3 calculate_scattering(vec3 start, vec3 dir, float max_dist, vec3 scene_color, vec3 light_dir, vec3 light_intensity, vec3 planet_position, float planet_radius, float atmo_radius, vec3 beta_ray, vec3 beta_mie, vec3 beta_absorption, vec3 beta_ambient, float g, float height_ray, float height_mie, float height_absorption, float absorption_falloff, int steps_i, int steps_l )
{
	start -= planet_position;
	float a = dot(dir, dir);
	float b = 2.0*dot(dir, start);
	float c = dot(start, start) - (atmo_radius*atmo_radius);
	float d = (b*b) - 4.0*a*c;
	if (d < 0.0) return scene_color;
	vec2 ray_length = vec2(max((-b - sqrt(d))/(2.0*a), 0.0), min((-b + sqrt(d))/(2.0*a), max_dist));
	if (ray_length.x > ray_length.y) return scene_color;
	bool allow_mie = max_dist > ray_length.y;
	ray_length.y = min(ray_length.y, max_dist);
	ray_length.x = max(ray_length.x, 0.0);
	float step_size_i = (ray_length.y - ray_length.x)/float(steps_i);
	float ray_pos_i = ray_length.x + step_size_i*0.5;
	vec3 total_ray = vec3(0.0);
	vec3 total_mie = vec3(0.0);
	vec3 opt_i = vec3(0.0);
	vec2 scale_height = vec2(height_ray, height_mie);
	float mu = dot(dir, light_dir);
	float mumu = mu*mu;
	float gg = g*g;
	float phase_ray = (3.0/50.2654824574)*(1.0 + mumu);
	float phase_mie = allow_mie? (3.0/25.1327412287)*((1.0 - gg)*(mumu + 1.0))/(pow(1.0 + gg - 2.0*mu*g, 1.0005)*(2.0 + gg)): 0.0;
	for (int i = 0; i < MAX_STEPS; ++i)
	{
		vec3 pos_i = start + dir*ray_pos_i;
		float height_i = length(pos_i) - planet_radius;
		vec3 density = vec3(exp(-height_i/scale_height), 0.0);
		float denom = (height_absorption - height_i)/absorption_falloff;
		density.z = (1.0/(denom*denom + 1.0))*density.x;
		density *= step_size_i;
		opt_i += density;
		a = dot(light_dir, light_dir);
		b = 2.0*dot(light_dir, pos_i);
		c = dot(pos_i, pos_i) - (atmo_radius*atmo_radius);
		d = (b*b) - 4.0*a*c;
		float step_size_l = (-b + sqrt(d))/(2.0*a*float(steps_l));
		float ray_pos_l = step_size_l*0.5;
		vec3 opt_l = vec3(0.0);
		for (int l = 0; l < MAX_STEPS; ++l)
		{
			vec3 pos_l = pos_i + light_dir*ray_pos_l;
			float height_l = length(pos_l) - planet_radius;
			vec3 density_l = vec3(exp(-height_l/scale_height), 0.0);
			float denom = (height_absorption - height_l)/absorption_falloff;
			density_l.z = (1.0/(denom*denom + 1.0))*density_l.x;
			density_l *= step_size_l;
			opt_l += density_l;
			ray_pos_l += step_size_l;
			if (l >= steps_l) break;
		}
		vec3 attn = exp(-beta_ray*(opt_i.x + opt_l.x) - beta_mie*(opt_i.y + opt_l.y) - beta_absorption*(opt_i.z + opt_l.z));
		total_ray += density.x*attn;
		total_mie += density.y*attn;
		ray_pos_i += step_size_i;
		if (i >= steps_i) break;
	}
	vec3 opacity = exp(-(beta_mie*opt_i.y + beta_ray*opt_i.x + beta_absorption*opt_i.z));
	return (phase_ray*beta_ray*total_ray + phase_mie*beta_mie*total_mie + opt_i.x*beta_ambient)*light_intensity + scene_color*opacity;
}
vec2 ray_sphere_intersect(vec3 start, vec3 dir, float radius)
{
	float a = dot(dir, dir);
	float b = 2.0*dot(dir, start);
	float c = dot(start, start) - (radius*radius);
	float d = (b*b) - 4.0*a*c;
	if (d < 0.0) return vec2(1e5,-1e5);
	return vec2((-b - sqrt(d))/(2.0*a), (-b + sqrt(d))/(2.0*a));
}
vec3 skylight(vec3 sample_pos, vec3 surface_normal, vec3 light_dir, vec3 background_col)
{
	surface_normal = normalize(mix(surface_normal, light_dir, 0.6));
	return calculate_scattering(sample_pos, surface_normal, 3.0*Atmosfera_Radio, background_col, light_dir, vec3(40.0), Planeta_Posicion, Planeta_Radio, Atmosfera_Radio, _betaR, _betaM, _betaAbs, _betaAmb, G, _heigR, _heigM, _heigAbs, _fallAbs, Light_Steps, Light_Steps);
}
vec4 render_scene(vec3 pos, vec3 dir, vec3 light_dir)
{
	vec4 color = vec4(0.0, 0.0, 0.0, 1e12);
	float f1 = smoothstep(0.9990, 0.99999, dot(dir, light_dir));
	float f2 = smoothstep(0.9987, 0.99999, dot(dir, light_dir));
	color.xyz = f1*vec3(3.0, 2.5, 0.0) + f2*vec3(1.4, 1.6, 1.8);
	vec2 planet_intersect = ray_sphere_intersect(pos - Planeta_Posicion, dir, Planeta_Radio);
	if (0.0 < planet_intersect.y)
	{
		color.w = max(planet_intersect.x, 0.0);
		vec3 sample_pos = pos + (dir*planet_intersect.x) - Planeta_Posicion;
		vec3 surface_normal = normalize(sample_pos);
		color.xyz = vec3(0.0, 0.25, 0.05);
		vec3 N = surface_normal;
		vec3 V = -dir;
		vec3 L = light_dir;
		float dotNV = max(1e-6, dot(N, V));
		float dotNL = max(1e-6, dot(N, L));
		float shadow = dotNL/(dotNL + dotNV);
		color.xyz *= shadow;
		color.xyz += clamp(skylight(sample_pos, surface_normal, light_dir, vec3(0.0))*vec3(0.0, 0.25, 0.05), 0.0, 1.0);
	}
	return color;
}
void main(void)
{
	vec2 uv = xlv_TEXCOORD0.xy/resolution.xy;
	vec3 light_dir;
	float t;
if (s_Nivel_Calidad != 0)
{
	Primary_Steps = 24;
	Light_Steps = 8;
}
else
{
	Primary_Steps = 12;
	Light_Steps = 4;
}
if (v_Encendido != 0)
{
if (v_Ejecutar != 0)
	t = time;
else
	t = 0.0;
	uv = 2.0*uv - 1.0;
	uv.x *= resolution.x/resolution.y;
	vec3 camera_vector = normalize(vec3(uv, -1.0));
	vec3 camera_position;
	float offset;
if (v_Escenario == 0)
	camera_position = vec3(0.0, Planeta_Radio + 100.0, 0.0);
else if (v_Escenario == 1)
	camera_position = vec3(0.0, Atmosfera_Radio , Atmosfera_Radio);
else if (v_Escenario == 2)
	camera_position = vec3(0.0, Atmosfera_Radio + (-cos(t/2.0)*(Atmosfera_Radio - Planeta_Radio - 1.0)), 0.0);
else if (v_Escenario == 3)
{
	offset = (1.0 - cos(t/2.0))*Atmosfera_Radio;
	camera_position = vec3(0.0, Planeta_Radio + 1.0, offset);
}
if (v_Sol_Automatico != 0)
	light_dir = normalize(vec3(0.0, cos(-t/8.0), sin(-t/8.0)));
else
{
	light_dir = normalize(vec3(0.0, cos(Pi*(mod(float(v_Sol_Posicion), 2400.0)-1200.0)/1200.0), sin(Pi*(mod(float(v_Sol_Posicion), 2400.0)-1200.0)/1200.0)));
}
	vec4 scene = render_scene(camera_position, camera_vector, light_dir);
	vec3 col = vec3(0.0);
	col += calculate_scattering(camera_position, camera_vector, scene.w, scene.xyz, light_dir, vec3(40.0), Planeta_Posicion, Planeta_Radio, Atmosfera_Radio, _betaR, _betaM, _betaAbs, _betaAmb, G, _heigR, _heigM, _heigAbs, _fallAbs, Primary_Steps, Light_Steps);
	col = 1.0 - exp(-col);
	gl_FragColor = vec4(col, 1.0);
}
else
{
	gl_FragColor = vec4(0.0);
}
}
