// https://www.gamasutra.com/view/feature/2866/implementing_lighting_models_with_.php?print=1
// Implementing Lighting Models With HLSL
//varying vec4 Pos;
varying vec3 Light;
varying vec3 Norm;
varying vec3 View;
varying vec2 uv;
[Vertex shader]
uniform mat4 WorldViewProjection;
uniform mat4 World;
uniform vec4 vecLightDir;
uniform vec4 vecEye;

//attribute vec3 Pos;
//attribute vec3 Normal;
//attribute vec3 BiNormal;
//attribute vec3 Tangent;
//attribute vec3 color;
//attribute vec2 uv;

void main(void)
{
	Light = vecLightDir.xyz;
	vec3 PosWorld = normalize(gl_Vertex*World).xyz;
	View = vecEye.xyz - PosWorld;
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
	vec3 Normal = normalize(Norm);
	vec3 LightDir = normalize(Light);
	vec3 ViewDir = normalize(View);
	vec4 diff = vec4(saturate(dot(Normal, LightDir)), 1.0); // diffuse component
	vec3 Reflect = normalize(2 * diff * Normal - LightDir);// R = 2 * (N.L) * N - L
	vec4 specular = vec4(pow(saturate(dot(Reflect, ViewDir)), 8), 1.0); // R.V^n
	vec4 texcol = clamp(texture2D(DIFFUSE_SAMPLER,uv), 0.75, 1.0);
	gl_FragColor = texcol*(AmbientColor + DiffuseColor * diff + specular);
}

