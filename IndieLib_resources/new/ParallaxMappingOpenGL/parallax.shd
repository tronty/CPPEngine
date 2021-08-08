//-----------------------------------------------------------------------------
// File: parallax.shd
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





/*
	This shader implements the main lighting, including the parallax techniques.
*/

[Vertex shader]

uniform vec3 lightPos;
uniform vec3 camPos;

attribute vec2 textureCoord;
attribute vec3 tangent;
attribute vec3 binormal;
attribute vec3 normal;
attribute vec2 lightmapCoord;

varying vec3 texCoord;
varying vec2 lmCoord;
varying vec3 lVec;
varying vec3 vVec;

void main(){
	gl_Position = ftransform();

	texCoord = vec3(textureCoord, 1.0);
#ifdef STATIC_SHADOW
	lmCoord = lightmapCoord;
#endif
	vec3 lightVec = lightPos - gl_Vertex.xyz;
	vec3 viewVec = camPos - gl_Vertex.xyz;

	// Transform vectors into tangent space
	lVec.x = dot(lightVec, tangent);
	lVec.y = dot(lightVec, binormal);
	lVec.z = dot(lightVec, normal);

	vVec.x = dot(viewVec, tangent);
	vVec.y = dot(viewVec, binormal);
	vVec.z = dot(viewVec, normal);
}


[Fragment shader]

#if defined(PARALLAX_DISTANCE) || defined(PARALLAX_OCCLUSION_MAPPING)
#extension GL_ATI_shader_texture_lod : require
#endif

uniform sampler2D Base;
uniform sampler2D Bump;
uniform sampler2D LightMap;

uniform float ambient;

uniform sampler3D DistanceMap;
uniform vec2 scaleBias;
uniform float threshold;
uniform float maxStepCount;

varying vec3 texCoord;
varying vec2 lmCoord;
varying vec3 lVec;
varying vec3 vVec;

vec4 computeSample(vec3 texCoord, vec2 dx, vec2 dy, vec3 lightVec, vec3 viewVec, float shadow){
#ifdef PARALLAX_MAPPING

#if defined(PARALLAX_DISTANCE)
	/*
		Parallax mapping with distance function works by looking up the closest distance to
		the surface from a volume texture. This indicates the maximum step one can safely
		take in that direction without missing any bumps. This is iterated until we're close
		enough to the surface, at which point we have our new texture coordinate.
	*/
	vec3 dir = viewVec;
	dir.z *= 8.0;
	dir = -normalize(dir);

	for (int i = 0; i < 64; i++){
		float step = texture3D(DistanceMap, texCoord).x * scaleBias.x + scaleBias.y;
		texCoord.xyz += step * dir;

		// When step = 0 we're on the surface. Quit when we get close enough.
		if (abs(step) < threshold) break;
	}

#define parallaxTexCoord texCoord.xy

#elif defined(PARALLAX_OCCLUSION_MAPPING)

	/*
		Parallax occlusion mapping ray-traces the height-map to find the intersection
		with the view vector. We step along the view direction until we find a point
		below the height-map.
	*/
	float nSteps = maxStepCount * length(viewVec.xy);

	vec3 dir = viewVec;
	dir.xy /= 8.0;
	dir /= -nSteps * dir.z;

	/*
		First test. Check this separately outside the loop in case the first sample
		is already on the surface (true for large parts of the wood texture in this
		sample), in which case we don't need to enter the loop at all. This saves us
		one texture sample and some computations in this case, plus guarantees that
		we never get a division by zero below.
	*/
	float diff0, diff1 = 1.0 - texture2DLod(Bump, texCoord.xy, 0.0).w; // texCoord.z = 1.0 initially
	if (diff1 > 0.0){
		do {
			texCoord += dir;

			diff0 = diff1;
			diff1 = texCoord.z - texture2DLod(Bump, texCoord.xy, 0.0).w;
		} while (diff1 > 0.0);

		/*
			Refine coordinate by intersecting the view direction with the surface.
			Assume the surface is linear between the last two height-map samples.
		*/
		texCoord.xy += (diff1 / (diff0 - diff1)) * dir.xy;
	}

	vec2 parallaxTexCoord = texCoord.xy;

#else
	/*
		Plain parallax mapping works by offsetting the texture coordinate by approximating
		where the ray would hit the surface. The heightmap indicates how far to offset.
		The approximation assumes that all heights in the height-map is equal, which of
		course won't be the case, but if the bumpmap is fairly smooth it works well enough.
	*/
	float parallax = 0.035;

	float height = texture2D(Bump, texCoord.xy).a;
	float offset = parallax * (2.0 * height - 1.0);
	vec2 parallaxTexCoord = texCoord.xy + offset * viewVec.xy;

#ifdef PARALLAX_REFINEMENT
	/*
		Refine the parallax by making another lookup at where we ended up in the first
		parallax computation, then averaging the results.
	*/
	height += texture2D(Bump, parallaxTexCoord).a;
	offset = parallax * (height - 1.0);
	parallaxTexCoord = texCoord.xy + offset * viewVec.xy;
#endif

#endif

#else // !PARALLAX_MAPPING
#  define parallaxTexCoord texCoord.xy
#endif



#if defined(PARALLAX_DISTANCE) || defined(PARALLAX_OCCLUSION_MAPPING)
	vec4 base = texture2D_ATI(Base, parallaxTexCoord, dx, dy);
#else
	vec4 base = texture2D(Base, parallaxTexCoord);
#endif

#ifdef BUMP_MAPPING
#if defined(PARALLAX_DISTANCE) || defined(PARALLAX_OCCLUSION_MAPPING)
	vec3 bump = texture2D_ATI(Bump, parallaxTexCoord, dx, dy).xyz * 2.0 - 1.0;
	// With parallax mapping with distance function the bumps are much steeper,
	// so we adjust the bumps a bit to match this.
	bump.z /= 4.0;
#else
	vec3 bump = texture2D(Bump, parallaxTexCoord).xyz * 2.0 - 1.0;
#endif

	bump = normalize(bump);
#else // !BUMP_MAPPING
	vec3 bump = vec3(0.0, 0.0, 1.0);
#endif

	// Standard Phong lighting
	float diffuse = saturate(dot(lightVec, bump));
	float specular = pow(saturate(dot(reflect(-viewVec, bump), lightVec)), 12.0);

	return shadow * (diffuse * base + 0.5 * specular) + ambient * base;
}

void main(){
	vec3 lightVec = normalize(lVec);
	vec3 viewVec  = normalize(vVec);

#ifdef STATIC_SHADOW
	float shadow = texture2D(LightMap, lmCoord).x;
#else
#  define shadow 1.0
#endif

#if defined(PARALLAX_DISTANCE) || defined(PARALLAX_OCCLUSION_MAPPING)
	vec3 dx = dFdx(texCoord);
	vec3 dy = dFdy(texCoord);
#else
	vec3 dx = vec3(0.0);
	vec3 dy = vec3(0.0);
#endif

#ifdef SUPERSAMPLE
	/*
		The derivatives indicate the difference between neighboring pixels in a pixel quad.
		It essentially tells us how much the texture coordinate varies to the neighboring
		pixels. This can be used to compute sample points for supersampling in a shader.
		Below a 4x rotated grid sample pattern is used.
	*/
	vec4 lighting = 0.25 * (
		computeSample(texCoord + 0.25 * 0.5 * dx + 0.75 * 0.5 * dy, dx.xy, dy.xy, lightVec, viewVec, shadow) +
		computeSample(texCoord - 0.25 * 0.5 * dx - 0.75 * 0.5 * dy, dx.xy, dy.xy, lightVec, viewVec, shadow) +
		computeSample(texCoord - 0.75 * 0.5 * dx + 0.25 * 0.5 * dy, dx.xy, dy.xy, lightVec, viewVec, shadow) +
		computeSample(texCoord + 0.75 * 0.5 * dx - 0.25 * 0.5 * dy, dx.xy, dy.xy, lightVec, viewVec, shadow)
	);

#else
	vec4 lighting = computeSample(texCoord, dx.xy, dy.xy, lightVec, viewVec, shadow);
#endif

	gl_FragColor = lighting;
}
