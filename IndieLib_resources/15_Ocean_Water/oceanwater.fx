[Vertex shader]
float4x4 xWorld;
float4x4 xView;
float4x4 xProjection;

float4 xWaveSpeeds;
float4 xWaveHeights;
float4 xWaveLengths;
float2 xWaveDir0;
float2 xWaveDir1;
float2 xWaveDir2;
float2 xWaveDir3;

float3 xCameraPos;
float xBumpStrength;
float xTexStretch;
float xTime;

struct OWVertexToPixel
{
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 Pos3D	: TEXCOORD1;
	float4 TTW0	: TEXCOORD2;
	float4 TTW1	: TEXCOORD3;
	float4 TTW2	: TEXCOORD4;
	float4 TTW3	: TEXCOORD5;	
};
    
//------- Technique: OceanWater --------
OWVertexToPixel main(float4 inPos: POSITION0, float2 inTexCoord: TEXCOORD0)
{
	OWVertexToPixel Output = (OWVertexToPixel)0;	
	
	float4 dotProducts;
    dotProducts.x = dot(xWaveDir0, inPos.xz);
    dotProducts.y = dot(xWaveDir1, inPos.xz);
    dotProducts.z = dot(xWaveDir2, inPos.xz);
    dotProducts.w = dot(xWaveDir3, inPos.xz);        
    
    float4 arguments = dotProducts/xWaveLengths+xTime*xWaveSpeeds;
    float4 heights = xWaveHeights*sin(arguments);
    
    float4 final3DPos = inPos;
    final3DPos.y += heights.x;
    final3DPos.y += heights.y;
    final3DPos.y += heights.z;
    final3DPos.y += heights.w;
	
	float4x4 preViewProjection = mul(xView, xProjection);
	float4x4 preWorldViewProjection = mul(xWorld, preViewProjection);
	Output.Position = mul(final3DPos, preWorldViewProjection);	
	
	float4 final3DPosW = mul(final3DPos, xWorld);
	Output.Pos3D = final3DPosW;	
	
	float4 derivatives = xWaveHeights*cos(arguments)/xWaveLengths;
	float2 deviations = 0;
    deviations += derivatives.x*xWaveDir0;
    deviations += derivatives.y*xWaveDir1;
    deviations += derivatives.z*xWaveDir2;
    deviations += derivatives.w*xWaveDir3;	
	
	float4 Normal = float4(-deviations.x, 1, -deviations.y, 1);	
	float4 Binormal = float4(1, deviations.x, 0, 1);
    float4 Tangent = float4(0, deviations.y, 1, 1);    
	
	float4x4 tangentToObject;
	tangentToObject[0] = normalize(Binormal);
	tangentToObject[1] = normalize(Tangent);
	tangentToObject[2] = normalize(Normal);		
	
	float4x4 tangentToWorld = mul(tangentToObject, xWorld);
	Output.TTW0 = tangentToWorld[0];
	Output.TTW1 = tangentToWorld[1];
	Output.TTW2 = tangentToWorld[2];
	Output.TTW3 = tangentToWorld[3];
	
	Output.TexCoord = inTexCoord+xTime/50.0f*float2(-1,0);
	
	return Output;
}
[Fragment shader]
struct OWVertexToPixel
{
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 Pos3D	: TEXCOORD1;
	float4 TTW0	: TEXCOORD2;
	float4 TTW1	: TEXCOORD3;
	float4 TTW2	: TEXCOORD4;
	float4 TTW3	: TEXCOORD5;	
};
struct OWPixelToFrame
{
    float4 Color 	: COLOR0;
};

float3 xCameraPos;
float xBumpStrength;
float xTexStretch;

//Texture xCubeMap;
samplerCUBE CubeMapSampler;// = sampler_state { texture = <xCubeMap> ; magfilter = LINEAR; minfilter = LINEAR; mipfilter=LINEAR; AddressU = mirror; AddressV = mirror;};
//Texture xBumpMap;
sampler2D BumpMapSampler;// = sampler_state { texture = <xBumpMap> ; magfilter = LINEAR; minfilter = LINEAR; mipfilter=LINEAR; AddressU = mirror; AddressV = mirror;};
OWPixelToFrame main(OWVertexToPixel PSIn) : COLOR0
{
	OWPixelToFrame Output = (OWPixelToFrame)0;					
		
	float3 bumpColor1 = tex2D(BumpMapSampler, xTexStretch*PSIn.TexCoord)-0.5f;	
	float3 bumpColor2 = tex2D(BumpMapSampler, xTexStretch*1.8*PSIn.TexCoord.yx)-0.5f;
	float3 bumpColor3 = tex2D(BumpMapSampler, xTexStretch*3.1*PSIn.TexCoord)-0.5f;
	
	float3 normalT = bumpColor1 + bumpColor2 + bumpColor3;
	
	normalT.rg *= xBumpStrength;
	normalT = normalize(normalT);

	float4x4 TTW;
	TTW[0]=PSIn.TTW0;
	TTW[1]=PSIn.TTW1;
	TTW[2]=PSIn.TTW2;
	TTW[3]=PSIn.TTW3;

	float3 normalW = mul(float4(normalT, 1), TTW).xyz;	
	
	float3 eyeVector = normalize(PSIn.Pos3D - xCameraPos);	
	float3 reflection = reflect(eyeVector, normalW);		
	float4 reflectiveColor = texCUBE(CubeMapSampler, reflection);
	
	float fresnelTerm = dot(-eyeVector, normalW);	
	fresnelTerm = fresnelTerm/2.0f+0.5f;		
	
	float sunlight = reflectiveColor.r;
	sunlight += reflectiveColor.g;
	sunlight += reflectiveColor.b;
	sunlight /= 3.0f;
	float specular = pow(sunlight,30);	
	
	float4 waterColor = float4(0,0.15,0.4,1);	
	Output.Color = waterColor*fresnelTerm + reflectiveColor*(1-fresnelTerm) + specular;	
		
	return Output;
}

