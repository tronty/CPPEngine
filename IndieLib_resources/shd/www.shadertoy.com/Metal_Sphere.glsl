vec3 gold = vec3(.9, .7, 0.);
vec3 ambientColor = vec3(0.8, 0.6, 0.4);
vec3 diffuseColor = vec3(1.6, 1.2, 0.8);
vec3 specularColor = vec3(1.2, 0.75, 0.3);
vec3 light1Dir = normalize(vec3(0.45, 0.15, -1.0));
vec3 light2Dir = normalize(vec3(-1.55, 0.3, 1.5));
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;

const float sphereRadius = 1.0;
vec3 spherePos = vec3(0.0, sphereRadius, 0.5);

float raytraceSphere(in vec3 ro, in vec3 rd, float tmin, float tmax) {
    vec3 ce = ro - spherePos;
    float b = dot(rd, ce);
    float c = dot(ce, ce) - (sphereRadius * sphereRadius);
    float t = b * b - c;
    if (t > tmin) {
        t = -b - sqrt(t);
        if (t < tmax)
            return t;
    }
    return -1.0;
}

float specular(in vec3 rd, in vec3 norm, in vec3 lightDir, float roughness, float fresnel) {

    float NdotL = dot(norm, lightDir);
    float NdotV = dot(norm, -rd);

    float spe = 0.0;
    if (NdotL > 0.0 && NdotV > 0.0) {

        vec3 h = normalize(-rd + lightDir);

        float NdotH = max(dot(norm, h), 0.0);
        float VdotH = max(dot(-rd, h), 0.000001);
        float LdotH = max(dot(lightDir, h), 0.000001);

        // Beckmann distrib
        float cos2a = NdotH * NdotH;
        float tan2a = (cos2a - 1.0) / cos2a;
        float r = max(roughness, 0.01);
        float r2 = r * r;
        float D = exp(tan2a / r2) / (r2 * cos2a * cos2a);

        // Fresnel term - Schlick approximation
        float F = fresnel + (1.0 - fresnel) * pow(1.0 - VdotH, 5.0);

        // Geometric attenuation term
        float G = min(1.0, (2.0 * NdotH / VdotH) * min(NdotV, NdotL));

        // Cook Torrance
        spe = D * F * G / (4.0 * NdotV * NdotL);
    }

    return spe;
}

void main( ) {
    vec2 p = (-iResolution.xy + 2.0 * xlv_TEXCOORD0.xy) / iResolution.y;

    vec3 eye = vec3(0.0, 1.0, 2.0);
    vec2 rot = 6.2831 * (vec2(0.6 + iTime * 0.025, sin(iTime * 0.1) * 0.06) + vec2(1.0, 0.0) * (iMouse.xy - iResolution.xy * 0.5) / iResolution.x);
    eye.yz = cos(rot.y) * eye.yz + sin(rot.y) * eye.zy * vec2(-1.0, 1.0);
    eye.xz = cos(rot.x) * eye.xz + sin(rot.x) * eye.zx * vec2(1.0, -1.0);

    vec3 ro = eye;
    vec3 ta = vec3(0.0, 1.0, 0.0);

    vec3 cw = normalize(ta - eye);
    vec3 cu = normalize(cross(vec3(0.0, 1.0, 0.0), cw));
    vec3 cv = normalize(cross(cw, cu));
    mat3 cam = mat3(cu, cv, cw);

    vec3 rd = cam * normalize(vec3(p.xy, 1.0));

    vec3 color = gold;//texture2D(iChannel0, rd).xyz;

    float tmin = 0.1;
    float tmax = 50.0;
    float t = raytraceSphere(ro, rd, tmin, tmax);
    if (t > tmin) {
        vec3 pos = ro + rd * t;
        vec3 norm = normalize(pos - spherePos);
        float occ = 0.5 + 0.5 * norm.y;

        float roughness = 0.15;
        float fresnel = 0.85;

        float amb = clamp(0.5 + 0.5 * norm.y, 0.0, 1.0);
        float dif1 = clamp(dot(light1Dir, norm), 0.0, 1.0);
        float spe1 = specular(rd, norm, light1Dir, roughness, fresnel);
        float dif2 = clamp(dot(light2Dir, norm), 0.0, 1.0);
        float spe2 = specular(rd, norm, light2Dir, roughness, fresnel);
        float fre = fresnel + (1.0 - fresnel) * pow(1.0 - dot(norm, -rd), 5.0);

        vec3 light1Color = dif1 * diffuseColor * occ;
        light1Color += dif1 * spe1 * specularColor * occ;
        vec3 light2Color = dif2 * diffuseColor * occ;
        light2Color += dif2 * spe2 * specularColor * occ;
#if 0
        vec2 uv2=reflect(rd, norm).xy;
        vec3 cub = texture2D(iChannel0, uv2).xyz;
        cub = mix(cub, vec3(0.4, 0.6, 0.8), 0.25);
        vec2 uv = vec2(atan(norm.x, norm.z), acos(norm.y));
        color = texture2D(iChannel1, uv).xyz * occ;
        color += amb * ambientColor * occ;
        color += light1Color;
        color += light2Color;
        color *= pow(cub, vec3(5.0)) * fre;
#endif
        vec3 gamma = vec3(1.0 / 2.2);
        color = pow(color, gamma);
    }

    gl_FragColor = vec4(color, 1.0);
}

0:124(13): error: no matching function for call to `texture2D(sampler2D, vec3)'; candidates are:

