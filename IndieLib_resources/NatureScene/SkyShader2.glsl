varying vec4 position;
varying vec2 TexCoord;
varying vec2 intensityintensitySqrd;

[Vertex shader]
uniform mat4 worldViewProj;

struct SkyVS_INPUT
{
    vec3 Normal	: NORMAL;
    vec3 BiNormal  	: BINORMAL;
    vec3 Tangent  	: TANGENT;
    vec3 Color  	: TEXCOORD0;
};

#if 1
uniform vec4  params=vec4(-0.167888, -0.035063, 0.930527, 1.0);
uniform vec3  offset=vec3(2560.000000, 2560.000000, -5100.000000);
#else
uniform vec4  params=vec4(0.000000, 0.000000, 1.000333, 1.0);
uniform vec3  offset=vec3(324.739990, 193.405914, -2237.400879);
#endif

void main()
{
#if 0
        vec2 newUV  = vec2((gl_Vertex.x - offset.x)/params.x,
			   (gl_Vertex.z - offset.z)/params.z);
	float paramsw=1000.0*params.w;

	TexCoord.x	= newUV.x + paramsw*0.66;
	TexCoord.y	= newUV.y + paramsw*0.33;
	TexCoord.xy *= 2.0;

	TexCoord.z = newUV.x + paramsw*1.33;
	TexCoord.w = newUV.y + paramsw*1.66;
	TexCoord.zw *= 3.0;

	//Get the vertex height and scale it to 0 - 1 range
	intensityintensitySqrd.x = gl_Vertex.y/params.y;
	intensityintensitySqrd.x = exp(clamp(intensityintensitySqrd.x, 0.0, 1.0))/2.8;
	intensityintensitySqrd.y = intensityintensitySqrd.x*intensityintensitySqrd.x;
#else
	TexCoord=vec2(gl_MultiTexCoord0.x+params.w, gl_MultiTexCoord0.y+params.w);
#endif
	gl_Position = worldViewProj * vec4(gl_Vertex,1);
	//gl_Position = vec4(gl_Vertex,1) * worldViewProj);
	return OUT;
      }
[Fragment shader]

uniform sampler2D noise;
uniform vec4 skyInfo=vec4(0.25, 0.5, 1.0, 0.0);

void main()
{
#if 0
	vec4 noise1	  = texture2D(noise, TexCoord.xy),
	     noise2	= texture2D(noise, TexCoord.zw);

	vec4 cloudFrag  = (noise1 + noise2) *  intensityintensitySqrd.y,
	     cloudColor = vec4((1.0 - intensityintensitySqrd.x)*skyInfo.x,
			       (1.0 - intensityintensitySqrd.x)*skyInfo.y,
			       intensityintensitySqrd.x*skyInfo.z, 0.0);

	gl_FragColor    = cloudColor * (1.0 - cloudFrag.x) + cloudFrag;
	//gl_FragColor=(noise1 + noise2);
	//gl_FragColor=noise2;

	gl_FragColor.rgb=GammaCorrect3(gl_FragColor.rgb);
	gl_FragColor.a=1.0;
#else
	gl_FragColor=texture2D(noise, TexCoord)*vec4(116.0, 162.0, 195.0, 256.0)/256.0*skyInfo;
#endif
	//gl_FragColor=vec4(0, 0, 1, 1);
}

