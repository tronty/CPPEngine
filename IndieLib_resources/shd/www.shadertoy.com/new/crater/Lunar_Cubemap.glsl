#define SCALE 0.95
#define GAMMA 2.2
#define rgba(r,g,b,a) vec4(pow(vec3(r,g,b),vec3(GAMMA)),a)
#define map(p) texture(iChannel0,p).x

vec3 normal(vec3 p) {
	vec2 e = vec2(1,0)/1e3;
    p += 0.01 * vec3(
        map(p + e.xyy) - map(p - e.xyy),
        map(p + e.yxy) - map(p - e.yxy),
        map(p + e.yyx) - map(p - e.yyx))/ (2. * length(e));
	return normalize(p);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
    vec2 p = (2. * fragCoord.xy - iResolution.xy) / iResolution.y;
    float lat = 15. * sin(0.1 * iTime);
    float lon = 7.5 * iTime + 100.;
    if (iMouse.z > 0.) {
        lat = 90.  - 180. * iMouse.y/iResolution.y;
        lon = 180. - 360. * iMouse.x/iResolution.x;
    }
    vec3 camPos = 10. * vec3(sin(lon*PI/180.) * cos(lat*PI/180.), sin(lat*PI/180.), cos(lon*PI/180.) * cos(lat*PI/180.));
    vec3 w = normalize(-camPos);
    vec3 u = normalize(cross(w, vec3(0,1,0)));
    vec3 v = normalize(cross(u, w));
    mat3 camera = mat3(u, v, w);
    
    vec3 dir = normalize(camera * vec3(p / SCALE, length(camPos)));
    float dist = iSphere(camPos, dir, vec4(0,0,0,1));
    fragColor = vec4(0);
    if (dist > 0.) {
        vec3 q = camPos + dir * dist;
        float c = map(q);
        vec3 n = normal(q);
        float light = clamp(dot(n, normalize(vec3(-4,1,2))), 0.01, 1.);
        fragColor = light * mix(rgba(0.82, 0.81, 0.8, 1), rgba(0.22, 0.21, 0.2, 1), smoothstep(0., 3., c));
    }
    fragColor.rgb = mix(fragColor.rgb, vec3(0), smoothstep(SCALE - 4./iResolution.y, SCALE + 1./iResolution.y, length(p)));
    fragColor.rgb = pow(fragColor.rgb, vec3(1./GAMMA));
}

