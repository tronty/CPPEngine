// https://www.gamasutra.com/view/feature/2866/implementing_lighting_models_with_.php?print=1
// Implementing Lighting Models With HLSL
//varying vec4 Pos;
varying vec3 Light;
varying vec3 Norm;
varying vec3 View;
varying vec2 uv;

[Vertex shader]

//attribute vec3 Pos;
//attribute vec3 Normal;
//attribute vec3 BiNormal;
//attribute vec3 Tangent;
//attribute vec3 color;
//attribute vec2 uv;

uniform mat4 WorldViewProjection;
uniform mat4 World;
uniform vec4 vecLightDir;
uniform vec4 vecEye;

void main(void)
{
	Light = vecLightDir.xyz;
	Norm = normalize(vec4(gl_Normal, 1.0)*World).xyz;
        gl_Position    = WorldViewProjection*gl_Vertex;
        uv = gl_MultiTexCoord0.xy;
}

[Fragment shader]

uniform vec4 AmbientColor;
uniform vec4 DiffuseColor;
uniform sampler2D DIFFUSE_SAMPLER;

void main (void)
{
	vec4 texcol = clamp(texture2D(DIFFUSE_SAMPLER,uv), 0.75, 1.0);
	gl_FragColor = texcol*AmbientColor;
}
