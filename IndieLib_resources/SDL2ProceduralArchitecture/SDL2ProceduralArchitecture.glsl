/*
  Copyright (c) 2024 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
// Vertex Shader Source
[Vertex shader]

uniform float displacementScale=100.0; // Scale of displacement

// Simple 3D noise function
float noise(vec3 p) {
    return fract(sin(dot(p, vec3(12.9898, 78.233, 45.164))) * 43758.5453);
}

// Smooth noise function using interpolation
float smoothNoise(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);
    f = f * f * (3.0 - 2.0 * f); // Smoothstep interpolation

    return mix(
        mix(
            mix(noise(i), noise(i + vec3(1.0, 0.0, 0.0)), f.x),
            mix(noise(i + vec3(0.0, 1.0, 0.0)), noise(i + vec3(1.0, 1.0, 0.0)), f.x),
            f.y
        ),
        mix(
            mix(noise(i + vec3(0.0, 0.0, 1.0)), noise(i + vec3(1.0, 0.0, 1.0)), f.x),
            mix(noise(i + vec3(0.0, 1.0, 1.0)), noise(i + vec3(1.0, 1.0, 1.0)), f.x),
            f.y
        ),
        f.z
    );
}

// Inverse function for mat4
mat4 inverse(mat4 m) {
    float 
        a00 = m[0][0], a01 = m[0][1], a02 = m[0][2], a03 = m[0][3],
        a10 = m[1][0], a11 = m[1][1], a12 = m[1][2], a13 = m[1][3],
        a20 = m[2][0], a21 = m[2][1], a22 = m[2][2], a23 = m[2][3],
        a30 = m[3][0], a31 = m[3][1], a32 = m[3][2], a33 = m[3][3];

    float 
        b00 = a00 * a11 - a01 * a10,
        b01 = a00 * a12 - a02 * a10,
        b02 = a00 * a13 - a03 * a10,
        b03 = a01 * a12 - a02 * a11,
        b04 = a01 * a13 - a03 * a11,
        b05 = a02 * a13 - a03 * a12,
        b06 = a20 * a31 - a21 * a30,
        b07 = a20 * a32 - a22 * a30,
        b08 = a20 * a33 - a23 * a30,
        b09 = a21 * a32 - a22 * a31,
        b10 = a21 * a33 - a23 * a31,
        b11 = a22 * a33 - a23 * a32;

    float det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
    if (det == 0.0) return mat4(0.0);

    float invDet = 1.0 / det;

    return mat4(
        (a11 * b11 - a12 * b10 + a13 * b09) * invDet,
        (-a01 * b11 + a02 * b10 - a03 * b09) * invDet,
        (a31 * b05 - a32 * b04 + a33 * b03) * invDet,
        (-a21 * b05 + a22 * b04 - a23 * b03) * invDet,
        (-a10 * b11 + a12 * b08 - a13 * b07) * invDet,
        (a00 * b11 - a02 * b08 + a03 * b07) * invDet,
        (-a30 * b05 + a32 * b02 - a33 * b01) * invDet,
        (a20 * b05 - a22 * b02 + a23 * b01) * invDet,
        (a10 * b10 - a11 * b08 + a13 * b06) * invDet,
        (-a00 * b10 + a01 * b08 - a03 * b06) * invDet,
        (a30 * b04 - a31 * b02 + a33 * b00) * invDet,
        (-a20 * b04 + a21 * b02 - a23 * b00) * invDet,
        (-a10 * b09 + a11 * b07 - a12 * b06) * invDet,
        (a00 * b09 - a01 * b07 + a02 * b06) * invDet,
        (-a30 * b03 + a31 * b01 - a32 * b00) * invDet,
        (a20 * b03 - a21 * b01 + a22 * b00) * invDet);
}

// Transpose function for mat4
mat4 transpose(mat4 m) {
    return mat4(
        m[0][0], m[1][0], m[2][0], m[3][0],
        m[0][1], m[1][1], m[2][1], m[3][1],
        m[0][2], m[1][2], m[2][2], m[3][2],
        m[0][3], m[1][3], m[2][3], m[3][3]
    );
}

// Periodic noise function (pnoise)
float pnoise(vec3 p, vec3 repeat) {
    vec3 i = mod(floor(p), repeat); // Periodic integer coordinates
    vec3 f = fract(p);              // Fractional part

    // Smoothstep interpolation
    f = f * f * (3.0 - 2.0 * f);

    vec3 r = mod(i + vec3(1.0), repeat); // Ensure wrapping

    return mix(
        mix(
            mix(noise(i), noise(vec3(r.x, i.y, i.z)), f.x),
            mix(noise(vec3(i.x, r.y, i.z)), noise(vec3(r.x, r.y, i.z)), f.x),
            f.y
        ),
        mix(
            mix(noise(vec3(i.x, i.y, r.z)), noise(vec3(r.x, i.y, r.z)), f.x),
            mix(noise(vec3(i.x, r.y, r.z)), noise(vec3(r.x, r.y, r.z)), f.x),
            f.y
        ),
        f.z
    );
}

// Turbulence function
float turbulence(vec3 p) {
    float t = 0.0;
    float freq = 1.0;
    float amp = 1.0;

    for (int i = 0; i < 5; i++) { // Layers of noise
        t += abs(pnoise(p * freq, vec3(10.0))) * amp;
        freq *= 2.0; // Increase frequency
        amp *= 0.5;  // Decrease amplitude
    }
    return t;
}

#define PI 3.1415926535

attribute vec3 aPos;
attribute vec3 aNormal;
attribute vec3 aBiNormal;
attribute vec3 aTangent;
attribute vec3 aColor;
attribute vec2 aUV;

varying vec3 vertexColor;
varying vec3 FragPos;
varying vec3 Normal;
varying vec3 Color;
varying vec4 ShadowCoord;
varying vec2 UV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 worldViewProj;

uniform mat4 lightSpaceMatrix;
//uniform float time;

#if 0
void main()
{
	UV=aUV;
    vec3 modifiedPos = aPos;
    #if 0
    float time_ = mod(aUV.x * 1.001 + aUV.y * 1.001, PI * 2.0);
    time_ = mod(aUV.x * 1.001 + aUV.y * 1.001, 1.0);
    modifiedPos += normalize(aNormal) * noise(aPos * 1.1 + time_) * 1.1; // Add noise-based displacement
#endif
    FragPos = vec3(model * vec4(modifiedPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    ShadowCoord = lightSpaceMatrix * vec4(modifiedPos, 1.0);
    
    #if 1
    gl_Position = projection * view * vec4(FragPos, 1.0);
    #elif 0
    gl_Position = worldViewProj * vec4(FragPos, 1.0);
    #else
    gl_Position = worldViewProj * vec4(aPos, 1.0);
    #endif
    
    vertexColor = aColor;
}
#elif 0
void main() {
	UV=aUV;
	vertexColor=aColor;
	Normal=aNormal;
    // Compute displacement using noise
    float displacement = smoothNoise(aPos + vec3(10.0*time)) * displacementScale;

    // Apply displacement along the normal
    vec3 displacedPosition = aPos + aNormal * displacement;

    // Transform positions and normals
    FragPos = vec3(model * vec4(displacedPosition, 1.0));
    Normal = normalize(mat3(model) * aNormal);

    // Apply final transformations
    gl_Position = projection * view * vec4(FragPos, 1.0);
    FragPos=gl_Position.xyz;
    Color=aColor;
}
#else
void main()
{
	UV=aUV;
	Normal=aNormal;
  // add time to the noise parameters so it's animated
  float noise_ = 10.0 *  (-.10) * turbulence( .5 * aNormal + time );
  float b = 5.0 * pnoise( 0.05 * aPos + vec3( 2.0 * time ), vec3( 100.0 ) );
  float displacement = (- noise_) + b;
//displacement = 1;
  vec3 newPos = aPos + aNormal * displacement;
    //gl_Position = projection * view * vec4(aPos, 1.0);
    gl_Position = worldViewProj * vec4(aPos, 1.0);
    FragPos=gl_Position.xyz;
    Color=aColor;
}
#endif

// Fragment Shader Source
[Fragment shader]
varying vec3 FragPos;
varying vec3 Normal;
varying vec3 Color;
varying vec4 ShadowCoord;

uniform vec3 lightPos=vec3(1.2, 1.0, 2.0);
uniform vec3 lightColor=vec3(1.0, 1.0, 1.0);
uniform vec3 viewPos=vec3(0.0, 0.0, 3.0);
uniform vec3 ambientColor=vec3(0.2, 0.2, 0.2);
uniform sampler2D DIFFUSE_SAMPLER;
uniform sampler2D shadowMap;

float ShadowCalculation(vec4 shadowCoord)
{
    // Perform perspective divide
    vec3 projCoords = shadowCoord.xyz / shadowCoord.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Get closest depth value from the shadow map
    float closestDepth = texture2D(shadowMap, projCoords.xy).r;
    // Get current depth value
    float currentDepth = projCoords.z;

    // Check if in shadow
    float shadow = currentDepth > closestDepth + 0.005 ? 0.5 : 1.0;

    return shadow;
}

void main()
{
    #if 1
    // Ambient
    vec3 ambient = 0.1 * ambientColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 1.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 1.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Shadow
    float shadow = 1;//ShadowCalculation(ShadowCoord);

	//diffuse=vec3(1);
	//specular=vec3(1);
	vec3 vertexColor_ = vec3(1);
	vertexColor_ = Color;
    vec3 result = (ambient + shadow * (diffuse + specular)) * vertexColor_;

    gl_FragColor = vec4(result, 1.0);
    #else
    gl_FragColor = texture2D(DIFFUSE_SAMPLER, UV);
    #endif
}

