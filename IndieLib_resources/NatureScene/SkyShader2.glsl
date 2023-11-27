struct SkyVS_OUTPUT
{
    vec4 position : POSITION;
    vec2 TexCoord : TEXCOORD0;
    vec2 intensityintensitySqrd : TEXCOORD1;
};
[Vertex shader]
 mat4 worldViewProj ;

struct SkyVS_INPUT
{
    vec3 position	: POSITION;
    vec3 Normal	: NORMAL;
    vec3 BiNormal  	: BINORMAL;
    vec3 Tangent  	: TANGENT;
    vec3 Color  	: TEXCOORD0;
    vec2 Tex  	: TEXCOORD1;
};

#if 1
vec4  params=vec4(-0.167888, -0.035063, 0.930527, 1.0);
vec3  offset=vec3(2560.000000, 2560.000000, -5100.000000);
#else
vec4  params=vec4(0.000000, 0.000000, 1.000333, 1.0);
vec3  offset=vec3(324.739990, 193.405914, -2237.400879);
#endif

SkyVS_OUTPUT main(SkyVS_INPUT IN)
{
	SkyVS_OUTPUT OUT;
#if 0
        vec2 newUV  = vec2((_position.x - offset.x)/params.x,
			   (_position.z - offset.z)/params.z);
	float paramsw=1000.0*params.w;

	__TexCoord.x	= newUV.x + paramsw*0.66;
	__TexCoord.y	= newUV.y + paramsw*0.33;
	__TexCoord.xy *= 2.0;

	__TexCoord.z = newUV.x + paramsw*1.33;
	__TexCoord.w = newUV.y + paramsw*1.66;
	__TexCoord.zw *= 3.0;

	//Get the vertex height and scale it to 0 - 1 range
	__intensityintensitySqrd.x = _position.y/params.y;
	__intensityintensitySqrd.x = exp(clamp(__intensityintensitySqrd.x, 0.0, 1.0))/2.8;
	__intensityintensitySqrd.y = __intensityintensitySqrd.x*__intensityintensitySqrd.x;
#else
	__TexCoord=vec2(_Tex.x+params.w, _Tex.y+params.w);
#endif
	__position = mul(worldViewProj,vec4(_position,1));
	//__position = mul(vec4(_position,1),worldViewProj);
	return OUT;
      }
[Fragment shader]

struct SkyPS_OUTPUT
{
	vec4 color : COLOR;
};

sampler2D noise;
vec4 skyInfo=vec4(0.25, 0.5, 1.0, 0.0);

SkyPS_OUTPUT main(SkyVS_OUTPUT IN)
{
	SkyPS_OUTPUT OUT;
#if 0
	vec4 noise1	  = texture2D(noise, _TexCoord.xy),
	     noise2	= texture2D(noise, _TexCoord.zw);

	vec4 cloudFrag  = (noise1 + noise2) *  _intensityintensitySqrd.y,
	     cloudColor = vec4((1.0 - _intensityintensitySqrd.x)*skyInfo.x,
			       (1.0 - _intensityintensitySqrd.x)*skyInfo.y,
			       _intensityintensitySqrd.x*skyInfo.z, 0.0);

	__color    = cloudColor * (1.0 - cloudFrag.x) + cloudFrag;
	//__color=(noise1 + noise2);
	//__color=noise2;

	__color.rgb=GammaCorrect3(__color.rgb);
	__color.a=1.0;
#else
	__color=texture2D(noise, _TexCoord)*vec4(116.0, 162.0, 195.0, 256.0)/256.0*skyInfo;
#endif
	//__color=vec4(0, 0, 1, 1);
	return OUT;
}

