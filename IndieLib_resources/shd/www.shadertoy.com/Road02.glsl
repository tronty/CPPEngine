const float speed = 16.0;



// Maps a position in the world to a position on the road

vec3 pos_road_map(vec3 pos)
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

	return vec3(x, y, z);
}

vec3 normal_road_map(vec3 pos)
{
	const float eps = 0.02;

	vec3 dir_z = pos_road_map(vec3(pos.xy, pos.z - eps)) - pos_road_map(vec3(pos.xy, pos.z + eps));
	vec3 dir_x = pos_road_map(vec3(pos.x - eps, pos.yz)) - pos_road_map(vec3(pos.x + eps, pos.yz));
	vec3 normal = cross(dir_z, dir_x);

	return normalize(normal);
}

vec3 col_road_map(vec3 pos)
{
	vec3 normal = normal_road_map(pos);

	vec3 dir_light = normalize(vec3(
		0.5,
		1.0 + sin(iTime * 0.134),
		4.0 * (1.0 + cos(iTime * 0.1))));

	float mix_light = clamp(dot(normal, dir_light), 0.0, 1.0);
	vec3 col_light = vec3(0.9, 0.75, 0.3);

	vec3 col_stripe = vec3(0.2);
	float mix_stripe = step(2.0, mod(pos.z, 4.0)) * 0.1;

	vec3 col_ground = vec3(0.135, 0.2, 0.1) +
		col_stripe * mix_stripe +
		col_light * mix_light;

	float midline = abs(pos_road_map(pos).x);

	float mix_road = 1.0 - smoothstep(0.0, 0.01, pow(midline, 16.0));
	vec3 col_road = vec3(0.2, -0.1, 0.2);
	col_ground = mix(col_ground, col_road, mix_road * 0.7);

	float mix_line = (1.0 - smoothstep(0.0, 0.0005, pow(midline, 2.5))) * smoothstep(0.2, 0.3, abs(mod(pos.z, 1.5))-0.75);
	vec3 col_line = vec3(0.7, 0.7, 0.7);
	col_ground = mix(col_ground, col_line, mix_line * 0.7);

	return col_ground;
}

vec3 col_sky(vec2 dxy_frag)
{
    vec2 uv = dxy_frag.xy / iResolution.xy;
    return vec3(0.4, 0.6, 0.9 * uv.y);
}

bool intersect_ray_road(vec3 pos_origin, vec3 dir, out vec3 pos_intersect)
{
	const float t_min = 0.001;
	const float t_max = 100.0;
	const float dt = 1.0;
	float y_road = 0.0;
	float y_ray = 0.0;

	for (float t = t_min; t < t_max; t += dt)
	{
		vec3 pos_ray = pos_origin + dir * t;
		vec3 pos_road = pos_road_map(pos_ray);
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

void main()
{
	const float fov = radians(50.0);
	const float tan_hfov = tan(fov * 0.5);
	const float near = 0.1;
	const float far = 1.0;

    vec2 uv = (xlv_TEXCOORD0 - 0.5 * iResolution.xy) / iResolution.y;

	vec3 pos_road_cam = pos_road_map(vec3(0, 0, speed * iTime));

	float len_car = 1.5;
	float h_car = 0.5;

	float x_next = -pos_road_map(vec3(0, 0, speed * iTime + len_car * 4.0)).x;

	vec3 pos_cam = vec3(
		0.35-pos_road_cam.x,
		pos_road_cam.y + h_car,
		pos_road_cam.z - len_car);

	float swivel = (x_next - pos_cam.x) * 0.07;

	vec3 dir_ray = normalize(
		vec3(
			tan_hfov * uv.x + swivel,
			tan_hfov * uv.y - 0.05,
			1.0)
		* (far - near));
	

	vec3 pos_intersect;
	if (intersect_ray_road(pos_cam, dir_ray, pos_intersect))
	{
		gl_FragColor = vec4(col_road_map(pos_intersect), 1.0);
	}
	else
	{
		gl_FragColor = vec4(col_sky(xlv_TEXCOORD0.xy), 1.0);
	}
}

