/*
 Copyright (c) 2024 Tommi Roenty http://www.tommironty.fi/
 Licensed under The GNU Lesser General Public License, version 2.1:
 http://opensource.org/licenses/LGPL-2.1
*/

// Simple 3D noise function
float noise(vec3 p) {
    return fract(sin(dot(p, vec3(12.9898, 78.233, 45.164))) * 43758.5453);
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

varying vec4 position;
varying vec2 UV;
varying float noise_;
varying vec3 vertexColor;

[Vertex shader]
attribute vec3 aPos;
attribute vec3 aNormal;
attribute vec3 aBiNormal;
attribute vec3 aTangent;
attribute vec3 aColor;
attribute vec2 aUV;
uniform mat4 worldViewProj;
//float time;
void main()
{
  // add time to the noise parameters so it's animated
  noise_ = 10.0 *  (-.10) * turbulence( .5 * aNormal + time );
  float b = 5.0 * pnoise( 0.05 * aPos + vec3( 2.0 * time ), vec3( 100.0 ) );
  float displacement = (- noise_) + b;

  vec3 newPosition = aPos + aNormal * displacement;
  gl_Position = worldViewProj * vec4(newPosition, 1.0);
  position=gl_Position;
  UV=aUV;
  vertexColor=aColor;
}

[Fragment shader]
#if 0
float random( vec3 scale, float seed, vec3 FragCoord ){
  return fract( sin( dot( FragCoord + seed, scale ) ) * 43758.5453 + seed ) ;
}

void main()
{
  // get a random offset
  float r = .01 * random( vec3( 12.9898, 78.233, 151.7182 ), 0.0, position.xyz );
  // lookup vertically in the texture, using noise and offset
  // to get the right RGB colour
  vec2 tPos = vec2( 1.3 * noise_ + r, 0 );
  vec4 color = texture2D( tExplosion, tPos );

  gl_FragColor = vec4( color.rgb, 1.0 );
}
#else
varying vec3 FragPos;
varying vec3 Normal;
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

	diffuse=vec3(1);
	specular=vec3(1);
	vec3 vertexColor_ = vec3(1);
	vertexColor_ = vertexColor;
    vec3 result = (ambient + shadow * (diffuse + specular)) * vertexColor_;

    gl_FragColor = vec4(result, 1.0);
    #else
    gl_FragColor = texture2D(DIFFUSE_SAMPLER, UV);
    #endif
}
#endif

