[Vertex shader]
	ROW_MAJOR float4x4 WorldViewProjection MVPSEMANTIC;
	float4 right;
	float4 up;
	struct VS_INPUT
	{
		float3 Position : POSITION;
		float2 vecOffset : TEXCOORD0;
		//float4 Diffuse : TEXCOORD1;
		float2 uv : TEXCOORD1;
	};
	struct VS_OUTPUT
	{
		float4 Position : POSITION;
	//	float4 Diffuse : TEXCOORD0;
		float2 uv : TEXCOORD0;
	};
	VS_OUTPUT main(VS_INPUT IN)
	{
		VS_OUTPUT Out = (VS_OUTPUT)0;
		//if (IN.Diffuse.rgb == float3(0.0,0.0,0.0)) discard;
		float3 objPos =  IN.vecOffset.y*up.xyz+ IN.vecOffset.x*right.xyz+ IN.Position;
		Out.Position = mul( WorldViewProjection,float4( objPos.x, objPos.y ,objPos.z, 1.0f));
		Out.uv=IN.uv;
		return Out;
	}

[Fragment shader]
	struct VS_OUTPUT
	{
		float4 Position : POSITION;
	//	float4 Diffuse : TEXCOORD0;
		float2 uv : TEXCOORD0;
		};

		sampler2D Sampler;
	float4 main(VS_OUTPUT IN) : COLOR
	{
		float4 OUT = tex2D(Sampler,IN.uv);
		//if (OUT.rgb == float3(0.0,0.0,0.0)) discard;
		return OUT;
	}

