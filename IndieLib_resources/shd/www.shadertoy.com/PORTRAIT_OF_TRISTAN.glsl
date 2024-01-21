uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;
uniform sampler2D iChannel3;
#define PI 3.14159265359
#define FOV 45.
#define TWO_PI 6.28318530718
/*

    Exploring modelling and shading a head. Use mouse to move camera.
    Change FOV in Common for a closer look at the details.

    Inspired by:
        https://www.shadertoy.com/view/wlf3WX - Head by tdhooper
        https://www.shadertoy.com/view/Md3GR4 - Meditation for dummies by iapafoto
        https://www.shadertoy.com/view/WsSBzh - Selfie Girl by iq
        https://www.shadertoy.com/view/ldd3DX - Surfer Boy by iq
    
    The geometry is ray marched only when the camera moves or the resolution changes.
    The normal and depth are written to a texture2D in Buffer C and the Image tab shades
    in screen space using the reconstructed position and normal with detail and albedo 
    maps from Buffer A and a pre-integrated subsurface scattering texture2D from Buffer B.
    Screen space shadows are also rendered using the depth information. Buffer D 
    generates height maps for eyebrows and lip detail.
    
    The geometry is hand-modelled using signed distance function primitives and
    constructive solid geometry operations. Shading uses PBR for specular and 
    pre-integrated subsurface scattering for diffuse.

    Do I think it's a real person? Of course not.
    Did I stare at it long enough to form some sort of attachment? Maybe.
    Did I start talking to it? That's not a problem until it talks back.

    TODO:
        Lenses
        Eyelashes
        
    References (not necessarily used):
    
    https://renderman.pixar.com/photorealistic-head
    https://www.3dscanstore.com/hd-head-scans/hd-head-models/hd-male-3d-head-model-21
    http://simonstechblog.blogspot.com/2015/02/pre-integrated-skin-shading.html
    https://dennisruan171085578.wordpress.com/2020/09/06/pre-integrated-skin-shading-under-spherical-harmonic-indirect-lighting/

    
    https://help.autodesk.com/view/ARNOL/ENU/?guid=arnold_user_guide_ac_legacy_shaders_ac_legacy_skin_html
    https://docs.unrealengine.com/4.26/en-US/Resources/Showcases/DigitalHumans/
    https://marmoset.co/posts/how-to-create-realistic-hair-peach-fuzz-and-eyes/
    https://www.youtube.com/watch?v=B3TnEMoNIr4
    
    https://www.youtube.com/watch?v=s0DhvFML7oM
    https://www.youtube.com/watch?v=HlHoIGE2Ocs
    https://www.youtube.com/watch?v=PYnmn3hp14s
    https://www.artstation.com/artwork/zDv3gd
    https://www.artstation.com/artwork/8w8Raw
    
    EDIT: Fix texture2D mapping artefacts

*/

#define SHADOWS

// Variable iterator initializer to stop loop unrolling
#define ZERO (min(iFrame,0))

const vec3 eyeColour = 0.12 * vec3(0.2, 0.5, 1.0);
const float AMBIENT_STRENGTH = 0.125;
const float DETAIL_EPSILON = 2e-3;
const float DETAIL_HEIGHT = 0.003;
const vec3 DETAIL_SCALE = vec3(4.0);
const vec3 BLENDING_SHARPNESS = vec3(1.0);

//----------------------------- Normal mapping -----------------------------

// https://tinyurl.com/y5ebd7w7
float getTriplanar(vec3 position, vec3 normal){
    float xaxis = texture2D(iChannel0, DETAIL_SCALE.x*(position.zy)).a;
    float yaxis = texture2D(iChannel0, DETAIL_SCALE.y*(position.zx)).a;
    float zaxis = texture2D(iChannel0, DETAIL_SCALE.z*(position.xy)).a;

    vec3 blending = abs(normal);
	blending = normalize(max(blending, 0.00001));
    blending = pow(blending, BLENDING_SHARPNESS);
	float b = (blending.x + blending.y + blending.z);
	blending /= b;

    return	xaxis * blending.x + 
       		yaxis * blending.y + 
        	zaxis * blending.z;
}

// Return the position of p extruded in the normal direction by normal map
vec3 getDetailExtrusion(vec3 p, vec3 normal){
    float detail = DETAIL_HEIGHT * getTriplanar(p, normal);
    
    vec3 ap = normalize(p);
    vec2 uvv = vec2((atan(ap.z, -ap.x) / (0.5*PI)) + 0.5, ap.y + 0.7);
    float wx = min(smoothstep(0.0, 0.05, uvv.x), smoothstep(1.0, 0.95, uvv.x));
    float wy = min(smoothstep(0.0, 0.05, uvv.y), smoothstep(1.0, 0.95, uvv.y));
    float w = min(wx, wy);

    // Lip
    if(uvv.x > 0.0 && uvv.x < 1.0 && uvv.y > 0.0 && uvv.y < 1.0){
        detail += w * 0.75*texture2D(iChannel3, uvv).r;
    }
    
    // Eyebrows
    if(uvv.x > 0.0 && uvv.x < 1.0 && uvv.y > 0.0 && uvv.y < 1.0){
        detail += w * 0.75*texture2D(iChannel3, 2.3*uvv-vec2(1.15, 1.375)).b;
        detail += w * 0.75*texture2D(iChannel3, vec2(-1, 1) * (2.3*uvv-vec2(1.15, 1.35))).b;
    }
    
    return p + detail * normal;
}


// Get orthonormal basis from surface normal
// https://graphics.pixar.com/library/OrthonormalB/paper.pdf
void pixarONB(vec3 n, out vec3 b1, out vec3 b2){
	float sign_ = n.z >= 0.0 ? 1.0 : -1.0;
	float a = -1.0 / (sign_ + n.z);
	float b = n.x * n.y * a;
	b1 = vec3(1.0 + sign_ * n.x * n.x * a, sign_ * b, -sign_ * n.x);
	b2 = vec3(b, sign_ + n.y * n.y * a, -n.y);
}

// Return the normal after applying a normal map
vec3 getDetailNormal(vec3 p, vec3 normal){

    vec3 tangent;
    vec3 bitangent;
    
    // Construct orthogonal directions tangent and bitangent to sample detail gradient in
    pixarONB(normal, tangent, bitangent);
    
    tangent = normalize(tangent);
    bitangent = normalize(bitangent);

    vec3 delTangent = vec3(0);
    vec3 delBitangent = vec3(0);
    
    for(int i = ZERO; i < 2; i++){
        
        //i to  s
        //0 ->  1
        //1 -> -1
        float s = 1.0 - 2.0 * float(i&1);
    
        delTangent += s * getDetailExtrusion(p + s * tangent * DETAIL_EPSILON, normal);
        delBitangent += s * getDetailExtrusion(p + s * bitangent * DETAIL_EPSILON, normal);

    }
    
    return normalize(cross(delTangent, delBitangent));
}

//------------------------------- Shading -------------------------------

float depth(vec2 uv){
    return texture2D(iChannel2, uv).a;
}

vec3 normal(vec2 uv){
    return texture2D(iChannel2, uv).rgb;
}


vec3 getAmbient(vec3 normal){

    // Flip environment map data
    vec4 n = vec4(normal * vec3(-1,1,1), 1.0);
    
    // Venice Sunrise spherical harmonics matrices
    mat4 red = mat4(0.16553553938865662, 0.12402049452066422, 
                    0.2091294825077057, 0.39199331402778625, 
                    0.12402049452066422, -0.16553553938865662, 
                    0.10027865320444107, 0.3196796178817749, 
                    0.2091294825077057, 0.10027865320444107, 
                    0.03354411944746971, 0.31643790006637573, 
                    0.39199331402778625, 0.3196796178817749, 
                    0.31643790006637573, 1.4789215326309204);
                    
    mat4 grn = mat4(0.08535411208868027, 0.07948554307222366, 
                    0.13153763115406036, 0.22119151055812836, 
                    0.07948554307222366, -0.08535411208868027, 
                    0.09292984008789062, 0.3931199014186859, 
                    0.13153763115406036, 0.09292984008789062, 
                    0.000118217445560731, 0.24447447061538696, 
                    0.22119151055812836, 0.3931199014186859, 
                    0.24447447061538696, 1.4334945678710938);
                    
    mat4 blu = mat4(0.01650789938867092, 0.04344102740287781, 
                    0.09565411508083344, 0.10879706591367722, 
                    0.04344102740287781, -0.01650789938867092, 
                    0.0988568365573883, 0.5883923768997192, 
                    0.09565411508083344, 0.0988568365573883, 
                    -0.05795031040906906, 0.2256385236978531, 
                    0.10879706591367722, 0.5883923768997192, 
                    0.2256385236978531, 1.7560255527496338);

    float r = dot(n, red * n);
    float g = dot(n, grn * n);
    float b = dot(n, blu * n);
    
    return vec3(r, g, b);
}

float getOcclusion(vec3 p){
    p.y -= 0.25;
    p.x -= 0.21;
    
    vec3 q = p;
    q.z = abs(q.z);
    q.z -= 0.06;
    q.x += 1.03;
    q.y += 0.58;
    float nostrils = smoothstep(0.0, 0.04, sphereSDF(q, 0.01));
    
    q = p;
    q.z = abs(q.z);
    q.z -= 0.07;
    q.x += 1.032;
    q.y += 0.59;
    nostrils = min(nostrils, smoothstep(0.005, 0.02, sphereSDF(q, 0.01)));
    
    q = p;
    q.z = abs(q.z);
    q.x += 0.22;
    q.y += 0.45;
    q.z -= 0.55;
    float ear = smoothstep(0.0, 0.1, sphereSDF(q, 0.01));
    return min(ear, nostrils);
}

//---------------------------- PBR ----------------------------

// Trowbridge-Reitz
float distribution(vec3 n, vec3 h, float roughness){
    float a_2 = roughness * roughness;
	return a_2/(PI * pow(pow(dot_c(n, h), 2.0) * (a_2 - 1.0) + 1.0, 2.0));
}

// GGX and Schlick-Beckmann
float geometry(float cosTheta, float k){
	return (cosTheta) / (cosTheta * (1.0 - k) + k);
}

float smiths(float NdotV, float NdotL, float roughness){
    float k = pow(roughness + 1.0, 2.0) / 8.0; 
	return geometry(NdotV, k) * geometry(NdotL, k);
}

// Fresnel-Schlick
vec3 fresnel(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
} 

// Specular part of Cook-Torrance BRDF
vec3 BRDF(vec3 p, vec3 n, vec3 viewDir, vec3 lightDir, vec3 F0, float roughness){
    vec3 h = normalize(viewDir + lightDir);
    float NdotL = dot_c(lightDir, n);
    float NdotV = dot_c(viewDir, n);

    float cosTheta = dot_c(h, viewDir);
    float D = distribution(n, h, roughness);
    vec3 F = fresnel(cosTheta, F0);

    float G = smiths(NdotV, NdotL, roughness);
    
    vec3 specular =  D * F * G / max(0.0001, (4.0 * NdotV * NdotL));
    
    return specular;
}


#define SIZE 2048.0

// https://www.shadertoy.com/view/4djSRW
vec3 hash(vec3 p3){
    p3 = mod(p3, SIZE);
    p3 = fract(p3 * vec3(0.1031, 0.1030, 0.0973));
    p3 += dot(p3, p3.yxz + 33.33);
    return 2.0 * fract((p3.xxy + p3.yxx) * p3.zyx) - 1.0;
}

float worley(vec3 pos, float numCells){
	vec3 p = pos * numCells;
	float d = 1.0e10;
	for (int x = -1; x <= 1; x++){
		for (int y = -1; y <= 1; y++){
			for (int z = -1; z <= 1; z++){
                vec3 tp = floor(p) + vec3(x, y, z);
                tp = p - tp - (0.5 + 0.5 * hash(mod(tp, numCells)));
                d = min(d, dot(tp, tp));
            }
        }
    }
	return saturate(d);
}

vec3 getFreckles(vec3 p){

    float dist = smoothstep(0.2, -0.5, worley(p, 24.0));
    dist = max(dist, smoothstep(0.2, -0.5, worley(p, 48.0)));
    
    float area = smoothstep(1.2, -0.0, length(p + vec3(1, 0.2, 0)));
    area = max(area, smoothstep(0.5, 0.35, length(vec3(p.x, p.y, abs(p.z)) + vec3(1, 0.2, -0.35))));
    
    area *= 1.0-0.75*smoothstep(0.65, 0.3, length(vec3(p.x, p.y, abs(p.z)) + vec3(1, 0.85, -0.25)));
    
    vec3 col = mix(vec3(1), vec3(0.4, 0.15, 0.0), dist * area);
    
    return col;
}

float getHairArea(vec3 p){

    // Top
    vec3 q = p;
    q.x -= 0.2;
    q.y -= 0.6;
    float d = smoothstep(0.875, 0.975, length(q));
    
    // Back
    q = p;
    q.x -= 0.5;
    q.y -= 0.15;
    q.z = abs(q.z);
    q.z -= 0.1;
    d = min(d, smoothstep(0.5, 0.8, length(q)));

    
    // Top sides
    q = p;
    q.x += 0.15;
    q.y -= 0.3;
    q.z = abs(q.z);
    q.z -= 0.4;
    d = min(d, smoothstep(0.325, 0.5, length(q)));
    
    
    // Sideburns
    q = p;
    q.x += 0.2;
    q.z = abs(q.z);
    q.z -= 0.4;
    float sideBurnD = smoothstep(-0.0, 0.25, sdBox(q, vec3(0.05, 0.1, 0.5)));
    
    q = p;
    q.x += 0.2;
    q.y += 0.4;
    q.z = abs(q.z);
    q.z -= 0.4;
    sideBurnD = max(sideBurnD, smoothstep(0.25, 0.0, sdBox(q, vec3(0.5, 0.1, 0.5))));
    
    d = min(d, sideBurnD);
    
    // Back sides
    q = p;
    q.x -= 0.5;
    q.y -= 0.15;
    q.z = abs(q.z);
    q.z -= 0.3;
    d = min(d, smoothstep(0.5, 0.8, length(q)));
    
    // Ear hollow
    q = p;
    q.x -= 0.045;
    q.y += 0.15;
    q.z = abs(q.z);
    q.z -= 0.75;
    d = max(d, smoothstep(0.275, 0.2, length(q)));
    
        
    // Back hollow
    q = p;
    q.x -= 0.075;
    q.y += 0.4;
    q.z = abs(q.z);
    q.z -= 0.5;
    d = max(d, smoothstep(0.4, 0.2, length(q)));
    
    
    return 1.0-d;
}

vec3 shadeEyes(vec3 p, vec3 normal){
    vec3 lookDir = normalize(vec3(-1,-0.1,0.0));
    vec3 col = 0.8 * vec3(1.0, 0.7, 0.7);
    float pupil = getPupilSDF(p);
    
    float bloodVessels = worley(p, 64.0);
    col = mix(mix(col, vec3(0.8, 0.3, 0.3), bloodVessels), col, smoothstep(0.05, 0.0, pupil));
    
    col = mix(col, vec3(0.05), smoothstep(0.013, 0.01, pupil));
    
    vec3 irisCol = mix(0.5*eyeColour, eyeColour, saturate(2.0*worley(p, 128.0)));
    irisCol = irisCol + 0.1*vec3(1,1,0) * saturate(worley(p, 100.0));
    irisCol = mix(eyeColour, irisCol, 0.5);
    col = mix(col, irisCol, smoothstep(0.01, 0.008, pupil));
    col = mix(col, vec3(0), smoothstep(0.0015, 0.0005, pupil));
   
      return col;
}

mat4 perspective(float fieldOfView, float aspect, float near, float far) {
    float f = tan(PI * 0.5 - 0.5 * radians(fieldOfView));
    float rangeInv = 1.0 / (near - far);

    return mat4(
      f / aspect, 0.0, 0.0, 0.0,
      0.0, f, 0.0, 0.0,
      0.0, 0.0, (near + far) * rangeInv, -1.0,
      0.0, 0.0, near * far * rangeInv * 2.0, 0.0
    );
}


const int SAMPLES = 32;
const float SHADOW_SHARPNESS = 0.05;
const float SHADOW_DISTANCE = 0.5;
float getShadow(vec2 uv, vec3 p, vec3 cameraPos, vec3 lightDir, vec2 res, mat4 viewMatrix){

    float shadow = 0.0;
    
    mat4 perspectiveMatrix = perspective(FOV, res.x/res.y, 0.0, 1.0);
 
    float d = depth(uv);
    
    const float del = SHADOW_DISTANCE/float(SAMPLES);
    for(int i = 0; i < SAMPLES; i++) {
        vec3 pos = p + lightDir * del * float(i);
        vec4 clip = perspectiveMatrix * (inverse(viewMatrix) * vec4(pos, 1.0));
        clip.xyz /= clip.w;  
        clip.xy = clip.xy * 0.5 + 0.5;

        float traversedDepth = depth(clip.xy);
        bool onScreen = clip.x >= 0.0 && clip.x <= 1.0 && clip.y >= 0.0 && clip.y <= 1.0;
        if((length(pos - cameraPos) - traversedDepth) > 1e-4 && onScreen) {
            shadow += del / SHADOW_SHARPNESS;
        }
    }
    return 1.0-saturate(shadow);
}

vec3 shading(vec3 p, vec3 n, vec3 rayDir, vec3 geoNormal, float shadow, vec3 lightDir){
    vec3 specular = vec3(0);

    vec3 ap = normalize(p);
    vec4 data = texture2D(iChannel0, vec2((atan(ap.z, -ap.x) / TWO_PI) + 0.5, acos(-ap.y) / PI));
    vec3 albedo = data.rgb;
    
    albedo *= getFreckles(p);
    
    vec2 uvv = vec2((atan(ap.z, -ap.x) / (0.5*PI)) + 0.5, ap.y + 0.7);
    float lips = 0.0;
    
    float wx = min(smoothstep(0.0, 0.05, uvv.x), smoothstep(1.0, 0.95, uvv.x));
    float wy = min(smoothstep(0.0, 0.05, uvv.y), smoothstep(1.0, 0.95, uvv.y));
    float w = min(wx, wy);
    
    if(uvv.x > 0.0 && uvv.x < 1.0 && uvv.y > 0.0 && uvv.y < 1.0){
        lips = 1.0-texture2D(iChannel3, uvv).g;
        albedo = mix(albedo, albedo * vec3(1.0, 0.8, 0.9), w * smoothstep(0.0, 1.0, lips));
        
        float eyebrows = max(texture2D(iChannel3, 2.3*uvv-vec2(1.15, 1.375)).b, 
                             texture2D(iChannel3, vec2(-1, 1) * (2.3*uvv-vec2(1.15, 1.35))).b);
    
        albedo = mix(albedo, 0.125*vec3(0.4, 0.2, 0.1), w * smoothstep(0.0, 1.0, 200.0*eyebrows));
    
        albedo = saturate(albedo);
    }
    
    float hair = getHairArea(p) * (1.0-worley(p, 128.0));
    albedo = mix(albedo, vec3(0.04, 0.02, 0.01), smoothstep(0.25, 1.25, hair));
    
    vec3 q = p;
    q.y -= 0.25;
    q.x -= 0.2;
    float collar = collarSDF(q);
    albedo = mix(0.1 * vec3(0.1, 0.2, 0.1), albedo, smoothstep(0.0, 0.01, collar));
    albedo = mix(albedo, 0.1 * vec3(0.1, 0.2, 0.1), smoothstep(-1.0, -1.2, rotateZ(q, -0.2).y));
    
    float h = getTriplanar(p, geoNormal);
    float roughness = 0.15;
    roughness = mix(roughness, 0.1, h); 
    roughness = mix(roughness, 0.9, hair);
    roughness = mix(roughness, 0.01, smoothstep(0.0, 1.0, lips));
    
    if(getEyeballSDF(q) < 0.001){
        albedo = shadeEyes(q, geoNormal);
        roughness = 0.01;
        n = geoNormal;
    }
    
    float IOR = mix(1.3, 1.5, smoothstep(0.0, 0.25, roughness));
    
    // Reflectance of the surface when looking straight at it along the negative normal
    vec3 F0 = vec3(pow(IOR - 1.0, 2.0) / pow(IOR + 1.0, 2.0));
    
    roughness = mix(0.35, roughness, smoothstep(0.0, 0.01, collar));
    specular += 0.35*BRDF(p, n, -rayDir, lightDir, F0, roughness) * dot_c(n, lightDir);

    roughness = 0.25;
    specular +=  BRDF(p, n, -rayDir, lightDir, F0, roughness) * dot_c(n, lightDir);
    
    float d = 0.5+0.5*dot(geoNormal, lightDir);

    float curvature = length(fwidth(geoNormal));
    curvature = mix(0.1, 0.5, curvature);
    curvature = mix(curvature, 0.0, hair);

    vec3 transmittance = texture2D(iChannel1, vec2(d, curvature)).rgb;
    
    vec3 diffuse = transmittance;
    
    // Mix detail normal with geometry normal to soften ambient light contribution
    // Add reddish hue to simulate ambient subsurface scattering
    vec3 ambientCol = mix(vec3(1,0,0), getAmbient(mix(geoNormal, n, 0.15)), 0.9);
    vec3 ambient = AMBIENT_STRENGTH * ambientCol;
    

    // Reflection of the environment.
    vec3 reflectedDir = normalize(reflect(rayDir, n));
    vec3 reflectedCol = getAmbient(reflectedDir);
    
    float cosTheta = dot_c(mix(geoNormal, n, 0.25), -rayDir);
    float F = fresnel(cosTheta, F0).x;
    
    ambient = mix(ambient, reflectedCol, saturate(F*0.35));

    vec3 radiance = vec3(1.0);
    return getOcclusion(p) * radiance * (shadow * specular + albedo * (ambient + shadow * diffuse));
}


//----------------------------- Tonemapping and output ------------------------------

// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 ACESFilm(vec3 x){
    return clamp((x * (2.51 * x + 0.03)) / (x * (2.43 * x + 0.59) + 0.14), 0.0, 1.0);
}

void main( ){
    
    //----------------- Define a camera -----------------
    
    vec3 rayDir = rayDirection(FOV, xlv_TEXCOORD0, iResolution.xy);

    vec3 cameraPos = texture2D(iChannel0, ivec2(0.5, 1.5)).xyz;

    vec3 targetDir = -cameraPos;
    

    vec3 up = vec3(0.0, 1.0, 0.0);

    // Get the view matrix from the camera orientation.
    mat3 viewMatrix = lookAt(cameraPos, targetDir, up);

    // Transform the ray to point in the correct direction.
    rayDir = normalize(viewMatrix * rayDir);

    //---------------------------------------------------

    vec2 uv = xlv_TEXCOORD0/iResolution.xy;
    vec4 data = texture2D(iChannel2, uv);
    
    vec3 col;
    
    if(data.a < 10.0){
        vec3 p = cameraPos + rayDir * data.a;
        vec3 geoNormal = data.xyz;
        vec3 detailNormal = normalize(getDetailNormal(p, geoNormal));
        
        float t = -0.5 * iTime;
        vec3 lightDir = normalize(vec3(sin(t), 0.65, cos(t)));
        mat4 viewMatrix4 = lookAt4(cameraPos, targetDir, up);
        float shadow = 1.0;
        
    #ifdef SHADOWS
        shadow = getShadow(uv, p, cameraPos, lightDir, iResolution.xy, viewMatrix4);
    #endif
    
        col = shading(p, detailNormal, rayDir, geoNormal, shadow, lightDir);

    }else{
        col = 0.01 * getAmbient(rayDir);
    }
    
    // Tonemapping
    col = ACESFilm(col);

    // Gamma
    col = pow(col, vec3(0.4545));

    gl_FragColor = vec4(col, 1.0);
}
