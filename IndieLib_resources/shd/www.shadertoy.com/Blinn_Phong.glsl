/* http://learnwebgl.brown37.net/10_surface_properties/surface_properties_color.html

Material 	Ambient color 	Diffuse color 	Specular color 	Shininess
Brass 	
0.329412
0.223529
0.027451
1.0
	
0.780392
0.568627
0.113725
1.0
	
0.992157
0.941176
0.807843
1.0
	27.8974
Bronze 	
0.2125
0.1275
0.054
1.0
	
0.714
0.4284
0.18144
1.0
	
0.393548
0.271906
0.166721
1.0
	25.6
Polished Bronze 	
0.25
0.148
0.06475
1.0
	
0.4
0.2368
0.1036
1.0
	
0.774597
0.458561
0.200621
1.0
	76.8
Chrome 	
0.25
0.25
0.25
1.0
	
0.4
0.4
0.4
1.0
	
0.774597
0.774597
0.774597
1.0
	76.8
Copper 	
0.19125
0.0735
0.0225
1.0
	
0.7038
0.27048
0.0828
1.0
	
0.256777
0.137622
0.086014
1.0
	12.8
Polished Copper 	
0.2295
0.08825
0.0275
1.0
	
0.5508
0.2118
0.066
1.0
	
0.580594
0.223257
0.0695701
1.0
	51.2
Gold 	
0.24725
0.1995
0.0745
1.0
	
0.75164
0.60648
0.22648
1.0
	
0.628281
0.555802
0.366065
1.0
	51.2
Polished Gold 	
0.24725
0.2245
0.0645
1.0
	
0.34615
0.3143
0.0903
1.0
	
0.797357
0.723991
0.208006
1.0
	83.2
Pewter 	
0.105882
0.058824
0.113725
1.0
	
0.427451
0.470588
0.541176
1.0
	
0.333333
0.333333
0.521569
1.0
	9.84615
Silver 	
0.19225
0.19225
0.19225
1.0
	
0.50754
0.50754
0.50754
1.0
	
0.508273
0.508273
0.508273
1.0
	51.2
Polished Silver 	
0.23125
0.23125
0.23125
1.0
	
0.2775
0.2775
0.2775
1.0
	
0.773911
0.773911
0.773911
1.0
	89.6
Emerald 	
0.0215
0.1745
0.0215
0.55
	
0.07568
0.61424
0.07568
0.55
	
0.633
0.727811
0.633
0.55
	76.8
Jade 	
0.135
0.2225
0.1575
0.95
	
0.54
0.89
0.63
0.95
	
0.316228
0.316228
0.316228
0.95
	12.8
Obsidian 	
0.05375
0.05
0.06625
0.82
	
0.18275
0.17
0.22525
0.82
	
0.332741
0.328634
0.346435
0.82
	38.4
Pearl 	
0.25
0.20725
0.20725
0.922
	
1.0
0.829
0.829
0.922
	
0.296648
0.296648
0.296648
0.922
	11.264
Ruby 	
0.1745
0.01175
0.01175
0.55
	
0.61424
0.04136
0.04136
0.55
	
0.727811
0.626959
0.626959
0.55
	76.8
Turquoise 	
0.1
0.18725
0.1745
0.8
	
0.396
0.74151
0.69102
0.8
	
0.297254
0.30829
0.306678
0.8
	12.8
Black Plastic 	
0.0
0.0
0.0
1.0
	
0.01
0.01
0.01
1.0
	
0.50
0.50
0.50
1.0
	32
Black Rubber 	
0.02
0.02
0.02
1.0
	
0.01
0.01
0.01
1.0
	
0.4
0.4
0.4
1.0
*/

// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
#if 0
vec3 ambientColor = vec3(0.05, 0.15, 0.2);
vec3 diffuseColor = vec3(0.2, 0.6, 0.8);
#elif 1 // Gold
vec3 ambientColor = vec3(0.24725, 0.1995, 0.0745);
vec3 diffuseColor = vec3(0.75164, 0.60648, 0.22648);
#elif 0 // Polished Gold
vec3 ambientColor = vec3(0.24725, 0.2245, 0.0645);
vec3 diffuseColor = vec3(0.34615, 0.3143, 0.0903);
#endif
vec3 specularColor = vec3(1.0, 1.0, 1.0);
vec3 lightDir = normalize(vec3(0.0, 4.0, 5.0));
vec3 spherePos = vec3(0.0, 0.5, 0.0);

float raytraceSphere(in vec3 ro, in vec3 rd, float tmin, float tmax, float r) {
    vec3 ce = ro - spherePos;
    float b = dot(rd, ce);
    float c = dot(ce, ce) - r * r;
    float t = b * b - c;
    if (t > tmin) {
        t = -b - sqrt(t);
        if (t < tmax)
            return t;
        }
    return -1.0;
}

void main( ) {
    vec2 p = (-iResolution.xy + 2.0 * xlv_TEXCOORD0.xy) / iResolution.y;
    vec3 eye = vec3(0.0, 1.0, 2.0);
    vec2 rot = 6.2831 * (vec2(0.1 + iTime * 0.25, 0.0) + vec2(1.0, 0.0) * (iMouse.xy - iResolution.xy * 0.25) / iResolution.x);
    eye.yz = cos(rot.y) * eye.yz + sin(rot.y) * eye.zy * vec2(-1.0, 1.0);
    eye.xz = cos(rot.x) * eye.xz + sin(rot.x) * eye.zx * vec2(1.0, -1.0);

    vec3 ro = eye;
    vec3 ta = vec3(0.0, 0.5, 0.0);

    vec3 cw = normalize(ta - eye);
    vec3 cu = normalize(cross(vec3(0.0, 1.0, 0.0), cw));
    vec3 cv = normalize(cross(cw, cu));
    mat3 cam = mat3(cu, cv, cw);

    vec3 rd = cam * normalize(vec3(p.xy, 1.5));

    vec3 color;

    float tmin = 0.1;
    float tmax = 50.0;
    float t = raytraceSphere(ro, rd, tmin, tmax, 1.0);
    if (t > tmin && t < tmax) {
        vec3 pos = ro + rd * t;
        vec3 norm = normalize(pos - spherePos);
        float occ = 0.5 + 0.5 * norm.y;

        float amb = clamp(0.5 + 0.5 * norm.y, 0.0, 1.0);
        float dif = clamp(dot(lightDir, norm), 0.0, 1.0);

        vec3 h = normalize(-rd + lightDir);
        float spe = pow(clamp(dot(h, norm), 0.0, 1.0), 64.0);

        color = amb * ambientColor * occ;
        color += dif * diffuseColor * occ;
        color += dif * spe * specularColor * occ;
    }

    vec3 gamma = vec3(1.0 / 2.2);
    gl_FragColor = vec4(pow(color, gamma), 1.0);
}

