float hash1(vec2 p) {
	p = 57.0 * fract(p * 1.4142135623);
	return fract(p.x * p.y);
}

vec3 noised(vec2 x) {
	vec2 p = floor(x);
	vec2 w = fract(x);
	vec2 s = vec2(1.0, 0.0);

	float a = hash1(p + s.yy);
	float b = hash1(p + s.xy);
	float c = hash1(p + s.yx);
	float d = hash1(p + s.xx);

	float k0 = a;
	float k1 = b - a;
	float k2 = c - a;
	float k4 = a - b - c + d;

	return vec3( -1.0 + 2.0 * (a + (b - a) * w.x + (c - a) * w.y + (a - b - c + d) * w.x * w.y), 2.0 * vec2(k1 + k4 * w.y, k2 + k4 * w.x));
}

float c01(float p) {
	return clamp(p, 0.0, 1.0);
}

const mat2 ma = mat2(
	 0.8,  0.6,
	-0.6,  0.8
);

const mat2 mai = mat2( 0.80, -0.60,
                       0.60,  0.80 );

// https://iquilezles.org/articles/fbm/
vec3 fbmd(vec2 x) {
	float f = 1.9;
	float s = 0.55;
	float a = 0.0;
	float b = 0.5;
    mat2  m = mat2(1.0,0.0,0.0,1.0);
	vec2 d = vec2(0.0);
	for (int i = 0; i < 7; i++) {
		vec2 offset = vec2(1.0, 1.5) * float(i);
		vec3 n = noised(x );
		a += b * n.x;
		d += b * m * n.yz;
		b *= s;
		x = f * ma * x;
        m = f*mai*m;

	}
	return vec3(a, d);
}


vec3 sdfPlaned(vec3 p) {
    vec3 n = fbmd(p.xz * 0.8) ;
	return vec3(p.y + n.x, n.yz);
}

vec3 minSdfd(in vec3 a, in vec3 b) {
	return a.x < b.x ? a : b;
}

vec3 sdfd(vec3 p) {
	vec3 a = sdfPlaned(p + vec3(10.0, 2.0, 10.0));
	vec3 b = vec3(abs(p.y + 1.7), 0.,0.);
	return minSdfd(a, b);
}

vec3 raymarchd(vec3 rayOrigin, vec3 rayDirection) {
	float distance = 0.0;
	float maxDistance = 50.0;
	float minHitDistance = 0.001;


	for (int i = 0; i < 256; i++) {
		if (distance > maxDistance) break;

		vec3 pos = rayOrigin + rayDirection * distance;

		vec3 res = sdfd(pos);

		if (res.x < minHitDistance) return vec3(distance + res.x, res.y, res.z);

		distance += res.x * 0.4;
	}

	return vec3(-1.0);
}

float shadow(vec3 ro, vec3 rd, float mint, float tmax) {
	float res = 1.0;
	float t = mint;
	for (int i = 0; i < 256; i++) {
		float h = sdfd(ro + rd * t).x;

		if (t > tmax) return res;
		if (h < 0.001) return 0.0;
		res = min(res, 18.0 * h / t);
		t += h;
	}
	return res;
}

vec3 directionalLight(
	vec3 rayDirection,
	vec3 normal,
	vec3 point,
	vec3 baseColor,
	vec3 lightDir,
	vec3 lightColor,
	float shininess
) {
	lightDir = normalize(lightDir);
	vec3 hal = normalize(lightDir - rayDirection);
	float diffuse = dot(normal, lightDir);
	diffuse = c01(diffuse);
	diffuse *= shadow(point, lightDir, 0.02, 2.5);

	float pho = c01(pow(dot(normal, hal), shininess));
	float spe = pho * diffuse * 0.3;

	return baseColor * 2.2 * diffuse * lightColor + 5.0 * spe * lightColor * 0.4;

}

vec3 lightning(float distance, vec3 col, vec3 point, vec3 rayDirection, vec3 rayOrigin, vec3 normal) {
	vec3 newColor = col;

	newColor += directionalLight(rayDirection, normal, point, col, vec3(2.0, 2.0, 2.0), vec3(0.902, 0.902, 0.902), 64.0);
	newColor += directionalLight(rayDirection, normal, point, col, vec3(0.0, 2.0, 2.0), vec3(0.8392, 0.8078, 0.3529), 64.0);
	newColor += directionalLight(rayDirection, normal, point, col, vec3(-2.0, 2.0, -1.5), vec3(0.8863, 0.3608, 0.3608), 64.0);

	return newColor / 4.0;
}

vec3 render(vec3 rayOrigin, vec3 rayDirection) {
	vec3 col = vec3(0.0);

	vec3 hit = raymarchd(rayOrigin, rayDirection);

	if (hit.x > 0.0) {
		vec3 p = rayOrigin + rayDirection * hit.x;
		vec3 n = normalize(vec3(hit.y, 1., hit.z));
		float l = smoothstep(0.6, .8, n.y);
		if (hit.y == 0. && hit.z == 0.) {
			col = vec3(0.1882, 0.1882, 0.5216);
		} else {
			col = vec3(0.0941, 0.0784, 0.0275) * (1.-l) + l * vec3(0.0588, 0.1176, 0.0471);
		}
		col = lightning(hit.x, col, p, rayDirection, rayOrigin, n);
	}

	return col;
}

void main() {
	float time = iTime / 25.0 * 1.5;

	float camspeed = 7.0;
	vec3 ta = vec3(0.0, .0, .0);
	vec3 ro = vec3(1.2 , .8, .0);
	vec3 t = -vec3(iTime, 0., iTime);

	ta +=t;
	ro +=t;

	vec3 cf = normalize(ta-ro);
	vec3 cs = normalize(cross(cf, vec3(sin(0.0), cos(0.0), 0.0)));
	vec3 cu = normalize(cross(cs, cf));

	vec2 p = (2.0 * xlv_TEXCOORD0.xy - iResolution.xy) / iResolution.y;
	vec3 rd = normalize(p.x * cs + p.y * cu + 2.5 * cf);

	vec3 col = render(ro, rd);
	col = pow(col, vec3(1.0 / 2.2));

	gl_FragColor = vec4(col, 1.0);
}

