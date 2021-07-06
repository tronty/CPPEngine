[Vertex shader]
//------- XNA interface --------
float4x4 xView;
float4x4 xProjection;
float4x4 xWorld;
float3 xCamPos;
//float3 xAllowedRotDir;
float3 xCamUp;
float xTime;

struct ExpVertexToPixel
{
	float4 Position : POSITION;
	float2 TexCoord	: TEXCOORD0;
	float4 Color	: COLOR0;
};
//------- Technique: Explosion --------
float3 BillboardVertex(float3 billboardCenter, float2 cornerID, float size)
{
	float3 eyeVector = billboardCenter - xCamPos;		
	
	float3 sideVector = cross(eyeVector,xCamUp);
	sideVector = normalize(sideVector);
	float3 upVector = cross(sideVector,eyeVector);
	upVector = normalize(upVector);
	
	float3 finalPosition = billboardCenter;
	finalPosition += (cornerID.x-0.5f)*sideVector*size;
	finalPosition += (0.5f-cornerID.y)*upVector*size;	
	
	return finalPosition;
}

ExpVertexToPixel main(float3 inPos: POSITION0, float4 inTexCoord: TEXCOORD0, float4 inExtra: TEXCOORD1)
{
	ExpVertexToPixel Output = (ExpVertexToPixel)0;	

	float3 startingPosition = mul(float4(inPos.x, inPos.y, inPos.z, 0.0), xWorld).xyz;
	
	float2 texCoords = inTexCoord.xy;
	float birthTime = inTexCoord.z;
	float maxAge = inTexCoord.w;
	float3 moveDirection = inExtra.xyz;
	float random = inExtra.w;	
	
	float age = xTime - birthTime;	
	float relAge = age/maxAge;	
	
	float sizer = saturate(1-relAge*relAge/2.0f);
	float size = 5.0f*random*sizer;
    
	float totalDisplacement = sin(relAge*6.28f/4.0f)*3.0f*random;
	float3 billboardCenter = startingPosition + totalDisplacement*moveDirection;	
	billboardCenter += age*float3(0,-1,0)/1000.0f;		
	
	float3 finalPosition = BillboardVertex(billboardCenter, texCoords, size);
	float4 finalPosition4 = float4(finalPosition, 1);
		
	float4x4 preViewProjection = mul (xView, xProjection);
	Output.Position = mul(finalPosition4, preViewProjection);
	
	float alpha = 1-relAge*relAge;
	Output.Color = float4(0.5f,0.5f,0.5f,alpha);	
	
	Output.TexCoord = texCoords;	
	
	return Output;
}
[Fragment shader]
struct ExpVertexToPixel
{
	float4 Position : POSITION;
	float2 TexCoord	: TEXCOORD0;
	float4 Color	: COLOR0;
};
struct ExpPixelToFrame
{
    float4 Color 	: COLOR0;
};

//------- Texture Samplers --------
//Texture xExplosionTexture;
sampler2D textureSampler;// = sampler_state { texture = <xExplosionTexture> ; magfilter = LINEAR; minfilter = LINEAR; mipfilter=LINEAR; AddressU = CLAMP; AddressV = CLAMP;};
#if 1
ExpPixelToFrame main(ExpVertexToPixel PSIn) : COLOR0
{
	ExpPixelToFrame Output = (ExpPixelToFrame)0;		
	Output.Color = tex2D(textureSampler, PSIn.TexCoord)*PSIn.Color;
	return Output;
}
#else
ExpPixelToFrame main(ExpVertexToPixel PSIn) : COLOR0
{
	ExpPixelToFrame Output = (ExpPixelToFrame)0;
	float4 Color1=tex2D(textureSampler, PSIn.TexCoord);
	float4 Color2=PSIn.Color;
	Output.Color = Color1*Color2;
	return Output;
}
#endif

