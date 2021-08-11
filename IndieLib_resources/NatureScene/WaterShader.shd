[Vertex shader]
struct WaterVS_INPUT
{
    float3 position     : POSITION;
        float2 MultiTexCoord0 : TEXCOORD0;
};
struct WaterVS_OUTPUT
{
    float4 position : POSITION;
    float4 TexCoord0  : TEXCOORD0;
    float4 TexCoord1  : TEXCOORD1;
    float4 TexCoord2  : TEXCOORD2;
    float4 TexCoord3  : TEXCOORD3;

};

ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
ROW_MAJOR float4x4 TextureMatrix1;
float4  translation=float4(510.0, -10.0, 1500.0, 1.0);
float4  camera=float4(-510.0, 68.0, -2210.0, 1.0);
float timer=0.0;

WaterVS_OUTPUT main_(WaterVS_INPUT IN)
{
        WaterVS_OUTPUT OUT;
	float4x4 TextureMatrix1=worldViewProj;
        float4 regularPosition    = float4(IN.position,1);
             regularPosition.y += cos(timer*0.75);
        float4 correctPosition    = regularPosition + translation;
        OUT.TexCoord0       = mul(TextureMatrix1 , correctPosition);
        OUT.TexCoord1.xy            = IN.MultiTexCoord0*2.0;
        OUT.TexCoord1.z=0;
        OUT.TexCoord1.w=1;
        OUT.TexCoord2       = camera - correctPosition;
        OUT.TexCoord3       = correctPosition;
        //OUT.position = mul(worldViewProj,regularPosition);
        OUT.position = mul(worldViewProj,float4(IN.position,1));
        return OUT;
      }
WaterVS_OUTPUT main(WaterVS_INPUT IN)
{
        WaterVS_OUTPUT OUT;

        float4 regularPosition    = float4(IN.position,1);
             regularPosition.y += cos(timer*0.75*0.001);

        float4 correctPosition    = regularPosition + translation;

//        OUT.TexCoord0       = gl_TextureMatrix[1] * correctPosition;
        OUT.TexCoord0       = mul(worldViewProj,correctPosition);

//        OUT.TexCoord0       = mul(correctPosition,ogl_TextureMatrix1 );

//        OUT.TexCoord1       = IN.MultiTexCoord0*2.0;

        OUT.TexCoord1.xy            = IN.MultiTexCoord0*2.0;
        OUT.TexCoord1.z=0;
        OUT.TexCoord1.w=1;

        OUT.TexCoord2       = camera - correctPosition;
        OUT.TexCoord3       = correctPosition;
        OUT.position = mul(worldViewProj,regularPosition);
        return OUT;
      }
[Fragment shader]
struct WaterVS_OUTPUT
{
    float4 position : POSITION;
    float4 TexCoord0  : TEXCOORD0;
    float4 TexCoord1  : TEXCOORD1;
    float4 TexCoord2  : TEXCOORD2;
    float4 TexCoord3  : TEXCOORD3;

};

struct WaterPS_OUTPUT
{
        float4 color : COLOR;
};
      sampler2D  reflection;
      sampler2D  normalmap;
      sampler2D  depthmap;

float timer=0.0;
float4 terrainInfo=float4(-2560.0, -2560.0, 5100.0, 1.0);
   
WaterPS_OUTPUT main_(WaterVS_OUTPUT IN)
{       
        WaterPS_OUTPUT OUT;

        float4  displacement    = IN.TexCoord1;
        float3  viewVector      = normalize(IN.TexCoord2.xyz);
        float2  projectedCoords = IN.TexCoord0.xy/IN.TexCoord0.w,
              uvDepthCoords   = float2((IN.TexCoord3.x - terrainInfo.x)/terrainInfo.z,
                                     (IN.TexCoord3.z - terrainInfo.y)/terrainInfo.w);

        float scaledTimer     = timer*0.05;

        displacement.x       += scaledTimer;
        displacement.y       -= scaledTimer;

        float depth    = tex2D(depthmap, uvDepthCoords).x;
        float3  normal   = normalize(tex2D(normalmap, displacement.xy).xyz*2.0 - 1.0)*0.025;
        OUT.color.xyz   = tex2D(reflection, projectedCoords + normal.xy).xyz;

        float alpha    = clamp(dot(float3(0.0, 1.0, 0.0), viewVector), 0.0, 1.0);
        OUT.color.a = 2.0 - (alpha*alpha + depth);

	OUT.color.rgb=GammaCorrect3(OUT.color.rgb);
	OUT.color.a=1.0;
        return OUT;
}
   
WaterPS_OUTPUT main(WaterVS_OUTPUT IN)
{       
        WaterPS_OUTPUT OUT;

        float4  displacement    = IN.TexCoord1;
        float3  viewVector      = normalize(IN.TexCoord2.xyz);
        float2  projectedCoords = IN.TexCoord0.xy/IN.TexCoord0.w,
              uvDepthCoords   = float2((IN.TexCoord3.x - terrainInfo.x)/terrainInfo.z,
                                     (IN.TexCoord3.z - terrainInfo.y)/terrainInfo.w);

        float scaledTimer     = timer*0.05;

        displacement.x       += scaledTimer;
        displacement.y       -= scaledTimer;

        float depth    = tex2D(depthmap, uvDepthCoords).x;
        float3  normal   = normalize(tex2D(normalmap, displacement.xy).xyz*2.0 - 1.0)*0.025;
        OUT.color   = tex2D(reflection, projectedCoords + normal.xy);

        float alpha    = clamp(dot(float3(0.0, 1.0, 0.0), viewVector), 0.0, 1.0);
        OUT.color.a = 2.0 - (alpha*alpha + depth);
        OUT.color.a  = clamp(depth, 0.0, 1.0);

	if((OUT.color.r>0.99)&&(OUT.color.g>0.99)&&(OUT.color.b>0.99))
		OUT.color.a  = 0.0;

	OUT.color.rgb=GammaCorrect3(OUT.color.rgb);
	OUT.color.a=1.0;
        return OUT;
}

