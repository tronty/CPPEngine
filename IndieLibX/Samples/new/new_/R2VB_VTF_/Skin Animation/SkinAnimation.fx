//--------------------------------------------------------------------------------------
// File: SkinAnimation.fx
//
// The effect file for the model/animation module.  
// 
// Copyright (c) 2007 Advanced Micro Devices Inc. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// defines
//--------------------------------------------------------------------------------------
#define MAX_BONES               32
#define MAX_MODELS              4096
#define BUFFER_SIZE             2048

#define FT_CONSTANTBUFFER       0
#define FT_BUFFER               1
#define FT_GPU                  2

#define RUBBY_NUM_BONES         31
#define KENJI_NUM_BONES         48

#define AMBIENT                 0.3
//--------------------------------------------------------------------------------------
// Input/Output structures
//--------------------------------------------------------------------------------------
struct VSAniKeyIn
{
	float4 Rotation : MODELROT;
	float4 Position : MODELPOS;
	uint BoneIndex  : SV_VertexID;
};

struct VSAniKeyOut
{
	float4 RotKey   : ROTKEY;
	float4 PosKey   : POSKEY;
};

struct VSSkinnedIn
{
	uint VIndex     : SV_VertexID;
	uint InstanceID : SV_InstanceID;
};

struct VSSkinnedStreamOut
{
	float4 Pos      : SKINNED;          //Position, w->tex coord u
	float4 Norm     : NORMAL;           //Normal, w->tex coord v
};

struct VSStaticIn
{
	float3 Pos      : POSITION;         //Position
	float3 Norm     : NORMAL;           //Normal
	float2 Tex      : TEXCOORD;         //Texture coordinate
	uint InstanceId : SV_InstanceID;
};

struct VSFloorOut
{
	float4 Pos      : SV_Position;      //Position
	float2 Tex      : TexCoord0;        //Texture coordinate
	float3 Normal   : NORMAL;
	float3 ViewDir  : VIEW;
	float3 LightDir : LIGHT;
	float4 LightViewPos : LIGHTVIEWPOS;
};

struct PSSkinnedIn
{
	float4 Pos      : SV_Position;      //Position
	float2 Tex      : TexCoord0;        //Texture coordinate
	float3 Normal   : NORMAL;
	float3 ViewDir  : VIEW;
	float3 LightDir : LIGHT;
};

struct PSShadowIn
{
	float4 Pos      : SV_Position;    //Position
};

struct SkinningOutput
{
	float4 Position;
	float3 Normal;
	float2 Tex;
};

struct OutputKey
{
	float4 Position;
	float4 Rotate;
};
//--------------------------------------------------------------------------------------
// Constant buffers
//--------------------------------------------------------------------------------------
// Dynamic constants
cbuffer cbGlobal
{
	float4x4 mvp;
	float4x4 lightMvp;
	float3 cameraPos;
	float4x4 clipToEnv;     // Clip space to environment direction matrix
};

// Constant buffer for bone matrices
cbuffer cbAnimMatrices
{
	float4x4 boneMatrixConstantBuffer[MAX_BONES];
};

cbuffer cbStatic
{
	static float3 lightPos = float3( -60.0, 60.0, -60.0);
};
//--------------------------------------------------------------------------------------
// Textures
//--------------------------------------------------------------------------------------
TextureCube Env;

Texture2D diffuseMap;
Texture2D bumpMap;
Texture2D shadowMap;

Texture2D skinVertexPos;            // Skinning vertex position, w component is the weight value of the first bined bone.
Texture2D skinVertexNormal;         // Skinning vertex normal, w component is the weight value of the second bined bone. 
Texture2D skinVertexTexCoord;       // Skinning vertex texture coordination, z,w components are the weight value of the third and fourth bined bones. 
Texture2D skinVertexBoneIndex;      // Skinning vertex binded bone index. 

Texture2D positionKeys;             // Position keys of this animation.
Texture2D rotationKeys;             // Rotation keys of this animation.

//--------------------------------------------------------------------------------------
// Buffers 
//--------------------------------------------------------------------------------------
// Bone matrix buffer or stream out buffer 
Buffer<float4> boneMatrixBuffer;

//--------------------------------------------------------------------------------------
// Texture samplers
//--------------------------------------------------------------------------------------
SamplerComparisonState g_shadowmapSampler
{
	Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = Border;
	AddressV = Border;
	BorderColor = float4(1, 1, 1, 1);
	ComparisonFunc = GREATER_EQUAL;
	MAXLOD = 0;
};

SamplerState g_samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

SamplerState g_samLinearClamp
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;
	AddressU = Border;
	AddressV = Border;
	BorderColor = float4(1, 1, 1, 1);
	MAXLOD = 0;
};

SamplerState g_samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
};

BlendState bsNoBlend
{
	BlendEnable[0] = false;
};
//--------------------------------------------------------------------------------------
// States
//--------------------------------------------------------------------------------------
DepthStencilState DisableDepth
{
	DepthEnable = FALSE;
	DepthWriteMask = 0;
};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

RasterizerState rsNoCull
{
	CullMode = None;
};

RasterizerState rsCullCCW
{
	CullMode = BACK;
};

//--------------------------------------------------------------------------------------
// Convert quternion to a 4x4 matrix.
//--------------------------------------------------------------------------------------
matrix Quaternion2float4x4(float4 q)
{
	matrix m;
	float s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;

	s = 2.0f;

	float3 vs;
	float4 qsy, qsz;

	vs = q.xyz*s;

	wx = q.w*vs.x;
	xx = q.x*vs.x;

	qsy.xyw = q.xyw*vs.y;
	qsz = q*vs.z;
	
	m[0][0] = 1.0f - (qsy.y+qsz.z);
	m[0][1] = qsy.x + qsz.w;
	m[0][2] = qsz.x - qsy.w;
	m[0][3] = 0;

	m[1][0] = qsy.x - qsz.w;
	m[1][1] = 1.0f - (xx+qsz.z);
	m[1][2] = qsz.y + wx;
	m[1][3] = 0;

	m[2][0] = qsz.x + qsy.w;
	m[2][1] = qsz.y - wx;
	m[2][2] = 1.0f - (xx + qsy.y);
	m[2][3] = 0;

	m[3][3] = 1.0f;

	return m;
}
//--------------------------------------------------------------------------------------
// Compute the quaternion from a 4x4 matrix
//--------------------------------------------------------------------------------------
float4 float4x42Quaternion(float4x4 m)
{
	float           tr, s;
	int             i, j, k;
	float4x4        M;
	float           Q[3];
	float4          q;
	int             nxt[3] = {1, 2, 0};

	tr = m[0][0] + m[1][1] + m[2][2];
	if (tr > 0.0f)
	{
		s = (float)sqrt(tr + 1.0f);
		q.w = s * 0.5f;

		s = 0.5f/s;
		q.x = (m[1][2] - m[2][1])*s;
		q.y = (m[2][0] - m[0][2])*s;
		q.z = (m[0][1] - m[1][0])*s;
	}
	else
	{
		M= m;
		i = 0;
		if (M[1][1] > M[0][0])
			i = 1;
		if (M[2][2] > M[i][i])
			i = 2;
		j = nxt[i];
		k = nxt[j];

		s = (float)sqrt(M[i][i] - ( M[j][j] + M[k][k] ) + 1.0f);

		Q[i] = s*0.5f;
		s = 0.5f/s;
		q.w = (M[j][k] - M[k][j])*s;

		Q[j] = (M[i][j] + M[j][i])*s;
		Q[k] = (M[i][k] + M[k][i])*s;
		q.x = Q[0];
		q.y = Q[1];
		q.z = Q[2];
	}
	return q;
}
//--------------------------------------------------------------------------------------
// Compute animation matrix from position and rotation<quternion> key
//--------------------------------------------------------------------------------------
matrix ComputeMatrixFromKey(float3 poskey, float4 rotkey)
{
	matrix m;

	m = Quaternion2float4x4(rotkey);
	m[3].xyz = poskey.xyz;

	return m;
}
//--------------------------------------------------------------------------------------
// Compute Euler from a 4x4 matrix
//--------------------------------------------------------------------------------------
float3 float4x42Euler(float4x4 m)
{
	float cx,sx,x;
	float cy,sy,y,yr;
	float cz,sz,z;
	float3 Euler;

	sy = -m[0][2];
	cy = sqrt(1 - (sy * sy));
	Euler.y = atan2(sy,cy);

	if (sy != 1.0f && sy != -1.0f)
	{
		cx = m[2][2] / cy;
		sx = m[1][2] / cy;
		Euler.x = atan2(sx,cx);

		cz = m[0][0] / cy;
		sz = m[0][1] / cy;
		Euler.z = atan2(sz,cz);
	}
	else
	{
		cx = m[1][1];
		sx = -m[2][1];
		Euler.x = atan2(sx,cx);

		cz = 1.0f;
		sz = 0.0f;
		Euler.z = atan2(sz,cz);
	}

	return Euler;
}
//--------------------------------------------------------------------------------------
// Compute a 4x4 matrix from a Euler
//--------------------------------------------------------------------------------------
float4x4 Euler2float4x4(float3 e)
{
	float               fCos, fSin;
	float4x4            tm, m;

	fCos = cos(e.x);
	fSin = sin(e.x);
	float4x4 MX = float4x4(1.0f, 0.0f, 0.0f, 0.0f,
	                       0.0f, fCos, fSin, 0.0f,
	                       0.0f, -fSin, fCos, 0.0f,
	                       0.0f, 0.0f, 0.0f, 1.0f);

	fCos = cos(e.y);
	fSin = sin(e.y);
	float4x4 MY = float4x4(fCos, 0.0f, -fSin, 0.0f,
	                       0.0f, 1.0f, 0.0f, 0.0f,
	                       fSin, 0.0f, fCos, 0.0f,
	                       0.0f, 0.0f, 0.0f, 1.0f);

	fCos = cos(e.z);
	fSin = sin(e.z);
	float4x4 MZ = float4x4(fCos,  fSin, 0.0f, 0.0f,
	                       -fSin, fCos, 0.0f, 0.0f,
	                       0.0f,  0.0f, 1.0f, 0.0f,
	                       0.0f,  0.0f, 0.0f, 1.0f);
	tm = mul(MX, MY); 
	m = mul(tm, MZ);
	
	return m;
}
//--------------------------------------------------------------------------------------
// Compute animation matrix from position and rotation<Euler> key
//--------------------------------------------------------------------------------------
matrix ComputeMatrixFromEuler(float3 poskey, float3 rotkey)
{
	matrix m;
	
	m = Euler2float4x4(rotkey);
	m[3].xyz = poskey.xyz;
	
	return m;	
}
//--------------------------------------------------------------------------------------
// FetchBoneTransform fetches a bone transformation using one of several methods
//
//		FT_CONSTANTBUFFER:
//			With this approach, the bone matrices are stored in a constant buffer.
//			The shader will index into the constant buffer to grab the correct
//			transformation matrices for each vertex.
//		FT_BUFFER:
//			This loads the matrices into a buffer that is bound to the shader.  The
//			shader calls Load on the buffer object to load the different matrices from
//			the stream.  This should give better linear access performance.
//		FT_GPU:
//			This loads the matrices into a stream out buffer that is bound to the shader.  
//			The shader calls Load on the buffer object to load the different matrices from
//			the stream.  This should give better linear access performance.
//--------------------------------------------------------------------------------------
matrix FetchBoneTransform( uint iBone, uniform uint iFetchType, uint instance )
{
	matrix mret;
	if( FT_CONSTANTBUFFER == iFetchType )
	{
		mret = boneMatrixConstantBuffer[ iBone ];
	}
	else if ( FT_BUFFER == iFetchType )
	{
		iBone *= 4;
		float4 row1 = boneMatrixBuffer.Load( iBone );
		float4 row2 = boneMatrixBuffer.Load( iBone + 1 );
		float4 row3 = boneMatrixBuffer.Load( iBone + 2 );
		float4 row4 = boneMatrixBuffer.Load( iBone + 3 );

		mret = float4x4( row1, row2, row3, row4 );
	}
	else if ( FT_GPU == iFetchType )
	{
		iBone <<= 1;
		iBone += instance*(RUBBY_NUM_BONES<<1);
		float3 Rot = boneMatrixBuffer.Load( iBone ).xyz;
		float3 Pos = boneMatrixBuffer.Load( iBone + 1 ).xyz;
		mret = ComputeMatrixFromEuler( Pos, Rot);
	}

	return mret;
}
//--------------------------------------------------------------------------------------
// Skins a single vertex with 4 bones binded.
//--------------------------------------------------------------------------------------
SkinningOutput SkinVert( VSSkinnedIn Input, uniform uint iFetchType )
{
	SkinningOutput Output;
	uint x, y;

	Output.Position = float4(0, 0, 0, 0);
	//Bone0
	float4 Vertex = skinVertexPos.Load(float3(Input.VIndex, 0, 0));
	float4 Normal = skinVertexNormal.Load(float3(Input.VIndex, 0, 0));
	uint4 iBone = skinVertexBoneIndex.Load(float3(Input.VIndex, 0, 0));
	float4 Tex = skinVertexTexCoord.Load(float3(Input.VIndex, 0, 0));

	float4 Pos = float4(Vertex.xyz, 1);	
	float fWeight = Vertex.w;
	matrix m = FetchBoneTransform( iBone.x, iFetchType, Input.InstanceID );
	Output.Position = fWeight * mul( Pos, m );
	Output.Normal = mul( Normal.xyz, m );

	[branch] if (Normal.w)
	{
		//Bone1
		m = FetchBoneTransform( iBone.y, iFetchType, Input.InstanceID );
		Output.Position += Normal.w * mul( Pos, m );

		[branch] if (Tex.z)
		{
			//Bone2
			m = FetchBoneTransform( iBone.z, iFetchType, Input.InstanceID );
			Output.Position += Tex.z * mul( Pos, m );

			[branch] if (Tex.w)
			{ 
				//Bone3
				m = FetchBoneTransform( iBone.w, iFetchType, Input.InstanceID );
				Output.Position += Tex.w * mul( Pos, m );
			}
		}
	}

	Output.Tex = Tex.xy;
	x = (Input.InstanceID%4);
	y = ((Input.InstanceID>>2)%4);
	Output.Tex += (float2(x, y)*0.25);

	return Output;
}
//--------------------------------------------------------------------------------------
// Vertex shader used for skinning the mesh for immediate render
//--------------------------------------------------------------------------------------
PSSkinnedIn VSSkinningMain(VSSkinnedIn input, uniform uint iFetchType )
{
	PSSkinnedIn output;

	SkinningOutput vSkinned = SkinVert( input, iFetchType );
	output.Pos = mul( vSkinned.Position, mvp );
	output.Normal = vSkinned.Normal;
	output.Tex = vSkinned.Tex;
	output.ViewDir = cameraPos - vSkinned.Position;
	output.LightDir = lightPos - vSkinned.Position;

	return output;
}
//--------------------------------------------------------------------------------------
// Vertex shader used for skinning the mesh for immediate render
//--------------------------------------------------------------------------------------
VSSkinnedStreamOut VSSkinningStreamOutMain(VSSkinnedIn input, uniform uint iFetchType )
{
	VSSkinnedStreamOut output;

	SkinningOutput vSkinned = SkinVert( input, iFetchType );
	output.Pos = float4(vSkinned.Position.xyz, vSkinned.Tex.x);
	output.Norm = float4(vSkinned.Normal.xyz, vSkinned.Tex.y);

	return output;
}
//--------------------------------------------------------------------------------------
// Vertex shader used for inputing animation key and output<StreamOut> a position and a Euler.
// Following shader will use the output position and Euler to computer cooresponding matrix.
// This method only outputs 2 float4 data to save bandwidth.
//--------------------------------------------------------------------------------------
VSAniKeyOut VSGPUResolveAnimationKeyMain(VSAniKeyIn input)
{
	matrix M, PM;
	VSAniKeyOut output;

	float3 Pos0, Pos1;
	float4 Rot0, Rot1;

	uint K1, K2;
	float Lerp;

	K1 = floor(input.Position.w);
	K2 = K1+1;
	K1 *= RUBBY_NUM_BONES;
	K2 *= RUBBY_NUM_BONES;
	K1 += input.BoneIndex;
	K2 += input.BoneIndex;
	Lerp = frac(input.Position.w);

	int3 Offset;
	Offset.x = ((K1)%BUFFER_SIZE);
	Offset.y = ((K1)/BUFFER_SIZE);
	Offset.z = 0; 
	Pos0 = positionKeys.Load(Offset).xyz;
	Rot0 = rotationKeys.Load(Offset);

	Offset.z = 0;
	Offset.x = ((K2)%BUFFER_SIZE);
	Offset.y = ((K2)/BUFFER_SIZE);
	Pos1 = positionKeys.Load(Offset).xyz;
	Rot1 = rotationKeys.Load(Offset);

	Pos0 = lerp(Pos0, Pos1, Lerp);
	Rot0 = lerp(Rot0, Rot1, Lerp);

	// Get parent transformation matrix
	PM = ComputeMatrixFromKey(input.Position, input.Rotation);
	M = mul(ComputeMatrixFromKey(Pos0, Rot0), PM);

	// Only output animated position and Euler. 
	output.PosKey = M[3];
	output.RotKey = float4(float4x42Euler(M), 0);

	return output;
}
//--------------------------------------------------------------------------------------
// Vertex shader used for transforming the static mesh for immediate render.
//--------------------------------------------------------------------------------------
PSSkinnedIn VSStreamOutMain(VSSkinnedStreamOut input)
{
	PSSkinnedIn output;

	output.Pos = mul( float4(input.Pos.xyz, 1.0), mvp );
	output.Tex = float2(input.Pos.w, input.Norm.w);
	output.Normal = input.Norm.xyz;
	output.ViewDir = cameraPos - input.Pos.xyz;
	output.LightDir = lightPos - input.Pos.xyz;

	return output;
}
//--------------------------------------------------------------------------------------
// Vertex shader used for transforming the static mesh for immediate render.
//--------------------------------------------------------------------------------------
PSShadowIn VSStreamOutShadowMain(VSSkinnedStreamOut input)
{
	PSShadowIn output;

	output.Pos = mul( float4(input.Pos.xyz, 1.0), mvp );

	return output;
}
//--------------------------------------------------------------------------------------
// Vertex shader used for transforming the static mesh for immediate render.
//--------------------------------------------------------------------------------------
PSSkinnedIn VSStaticMain(VSStaticIn input)
{
	PSSkinnedIn output;

	output.Pos = mul( float4(input.Pos, 1), mvp );
	output.Tex = input.Tex;
	output.Normal = input.Norm;
	output.ViewDir = cameraPos - input.Pos;
	output.LightDir = lightPos - input.Pos;

	return output;
}
//--------------------------------------------------------------------------------------
// Pixel shader that performs simple texture look up.
//--------------------------------------------------------------------------------------
float4 PSMain(PSSkinnedIn input) : SV_Target
{
	float3 LightDir = normalize(input.LightDir);
	float4 diffuse = diffuseMap.Sample( g_samLinear, input.Tex );
	diffuse.rgb *= saturate(dot(normalize(input.Normal), LightDir)) + AMBIENT;
	float specular = pow(saturate(dot(reflect(normalize(-input.ViewDir), normalize(input.Normal)), LightDir)), 16.0);
	diffuse.rgb += specular;

	return saturate(diffuse);
}
//--------------------------------------------------------------------------------------
// Pixel shader that performs simple texture look up.
//--------------------------------------------------------------------------------------
float4 PSShadowMain(PSShadowIn input) : SV_Target
{
	float4 diffuse = float4(0, 0, 0, 1);

	return diffuse;
}
//--------------------------------------------------------------------------------------
// Pixel shader that performs simple texture look up.
//--------------------------------------------------------------------------------------
float4 PSMain_NoSpecular(PSSkinnedIn input) : SV_Target
{
	float3 LightDir = normalize(input.LightDir);
	float4 diffuse = diffuseMap.Sample( g_samLinear, input.Tex );
	diffuse.rgb *= saturate(dot(normalize(input.Normal), LightDir)) + AMBIENT;

	return saturate(diffuse);
}
//--------------------------------------------------------------------------------------
// Render the scene by fetching bone matrices from a constant buffer.
//--------------------------------------------------------------------------------------
technique10 techConstantBuffer
{
	pass P0
	{
		SetDepthStencilState( EnableDepth, 0 );
		SetRasterizerState(rsCullCCW);
		
		SetVertexShader( CompileShader( vs_4_0, VSSkinningMain( FT_CONSTANTBUFFER ) ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PSMain() ) );
	}
}
//--------------------------------------------------------------------------------------
// Render the scene by fetching bone matrices from a buffer.
//--------------------------------------------------------------------------------------
technique10 techBuffer
{
	pass P0
	{
		SetDepthStencilState( EnableDepth, 0 );
		SetRasterizerState(rsCullCCW);

		SetVertexShader( CompileShader( vs_4_0, VSSkinningMain( FT_BUFFER ) ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PSMain() ) );
	}
}
//--------------------------------------------------------------------------------------
// Fetching animation key data and output cooresponding postion and Euler.
//--------------------------------------------------------------------------------------
technique10 techGPUResolveSkinning
{
	pass P0
	{
		SetDepthStencilState( DisableDepth, 0 );
		SetRasterizerState(rsNoCull);

		SetVertexShader( CompileShader( vs_4_0, VSGPUResolveAnimationKeyMain() ) );
		SetGeometryShader( ConstructGSWithSO( CompileShader( vs_4_0, VSGPUResolveAnimationKeyMain()), "ROTKEY.xyzw; POSKEY.xyzw"));
		SetPixelShader( NULL );
	}
}
//--------------------------------------------------------------------------------------
// Rendering the scene by fetching bone matrices from a output of previous pass.
// This pass can render multi-instanced model at one time.
//--------------------------------------------------------------------------------------
technique10 techGPUDrawMultiModels
{
	pass P0
	{
		SetDepthStencilState( EnableDepth, 0 );
		SetRasterizerState(rsCullCCW);

		SetVertexShader( CompileShader( vs_4_0, VSSkinningMain( FT_GPU ) ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PSMain() ) );
	}
}
//--------------------------------------------------------------------------------------
// Rendering the scene shadow by fetching bone matrices from a output of previous pass.
// This pass can render multi-instanced model at one time.
//--------------------------------------------------------------------------------------
technique10 techGPUDrawMultiModelShadows
{
	pass P0
	{
		SetDepthStencilState( DisableDepth, 0 );
		SetRasterizerState(rsCullCCW);

		SetVertexShader( CompileShader( vs_4_0, VSSkinningMain( FT_GPU ) ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PSShadowMain() ) );
	}
}
//--------------------------------------------------------------------------------------
// Streaming out the scene by fetching bone matrices from a output of previous pass.
// This pass can stream out multi-instanced model at one time.
//--------------------------------------------------------------------------------------
VertexShader multiModelVS = CompileShader( vs_4_0, VSSkinningStreamOutMain( FT_GPU ) );

technique10 techGPUStreamoutMultiModels
{
	pass P0
	{
		SetDepthStencilState( DisableDepth, 0 );
		SetRasterizerState(rsNoCull);

		SetVertexShader( multiModelVS );
		SetGeometryShader( ConstructGSWithSO( multiModelVS, "SKINNED.xyzw; NORMAL.xyzw") );
		SetPixelShader( NULL );
	}
}
//--------------------------------------------------------------------------------------
// Rendering the scene by fetching stream out buffer.
//--------------------------------------------------------------------------------------
technique10 techGPUDrawStreamoutModels
{
	pass P0
	{
		SetDepthStencilState( EnableDepth, 0 );
		SetRasterizerState(rsCullCCW);

		SetVertexShader( CompileShader( vs_4_0, VSStreamOutMain( ) ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PSMain() ) );
	}
}
//--------------------------------------------------------------------------------------
// Rendering the scene shadow by fetching stream out buffer.
//--------------------------------------------------------------------------------------
technique10 techGPUDrawStreamoutShadows
{
	pass P0
	{
		SetDepthStencilState( DisableDepth, 0 );
		SetRasterizerState(rsCullCCW);

		SetVertexShader( CompileShader( vs_4_0, VSStreamOutShadowMain( ) ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PSShadowMain() ) );
	}
}
//--------------------------------------------------------------------------------------
// Rendering the static mesh. 
//--------------------------------------------------------------------------------------
technique10 techStatic
{
	pass P0
	{
		SetDepthStencilState( EnableDepth, 0 );
		SetRasterizerState(rsCullCCW);

		SetVertexShader( CompileShader( vs_4_0, VSStaticMain() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PSMain() ) );
	}
}


//--------------------------------------------------------------------------------------
// Vertex shader used for transforming the static mesh for immediate render.
//--------------------------------------------------------------------------------------
VSFloorOut VSFloorMain(VSStaticIn input)
{
	VSFloorOut output;

	output.Pos = mul( float4(input.Pos, 1), mvp );
	output.LightViewPos = mul( float4(input.Pos, 1), lightMvp );
	output.Tex = input.Tex;
	output.Normal = input.Norm;
	output.ViewDir = cameraPos - input.Pos;
	output.LightDir = lightPos - input.Pos;

	return output;
}
//--------------------------------------------------------------------------------------
// 3X3 PCF shadow map filtering.
//--------------------------------------------------------------------------------------
float4 PCF3X3Shadow(Texture2D sm, float2 tex)
{
	float Offset = 1.0/2048.0;


	float4 ShadowData = sm.Sample(g_samLinearClamp, float3(tex, 0)).rrrr;
	ShadowData += sm.Sample(g_samLinearClamp, float3(tex+float2(Offset, 0), 0)).rrrr;
	ShadowData += sm.Sample(g_samLinearClamp, float3(tex+float2(0, Offset), 0)).rrrr;
	ShadowData += sm.Sample(g_samLinearClamp, float3(tex+float2(-Offset, 0), 0)).rrrr;
	ShadowData += sm.Sample(g_samLinearClamp, float3(tex+float2(0, -Offset), 0)).rrrr;
	ShadowData += sm.Sample(g_samLinearClamp, float3(tex+float2(Offset, Offset), 0)).rrrr;
	ShadowData += sm.Sample(g_samLinearClamp, float3(tex+float2(-Offset, -Offset), 0)).rrrr;
	ShadowData += sm.Sample(g_samLinearClamp, float3(tex+float2(Offset, -Offset), 0)).rrrr;
	ShadowData += sm.Sample(g_samLinearClamp, float3(tex+float2(-Offset, Offset), 0)).rrrr;

	return (ShadowData/9.0);
}
//--------------------------------------------------------------------------------------
// 2X2 PCF shadow map filtering.
//--------------------------------------------------------------------------------------
float4 PCF2X2Shadow(Texture2D sm, float2 tex)
{
	float Offset = 1.0/2048.0;

	float4 ShadowData = sm.Sample(g_samLinearClamp, float3(tex, 0)).rrrr;
	ShadowData += sm.Sample(g_samLinearClamp, float3(tex+float2(0, Offset), 0)).rrrr;
	ShadowData += sm.Sample(g_samLinearClamp, float3(tex+float2(Offset, 0), 0)).rrrr;
	ShadowData += sm.Sample(g_samLinearClamp, float3(tex+float2(Offset, Offset), 0)).rrrr;

	return (ShadowData/4.0);
}
//--------------------------------------------------------------------------------------
// No PCF shadow map filtering.
//--------------------------------------------------------------------------------------
float4 NoPCFShadow(Texture2D sm, float2 tex)
{
	float4 ShadowData = sm.Sample(g_samLinearClamp, float3(tex, 0)).rrrr;

	return (ShadowData);
}
//--------------------------------------------------------------------------------------
// Pixel shader that performs simple lighing and shadowing.
//--------------------------------------------------------------------------------------
float4 PSFloorMain(VSFloorOut input) : SV_Target
{
	float3 LightDir = normalize(input.LightDir);
	float4 diffuse = diffuseMap.Sample( g_samLinear, input.Tex );
	float3 Normal = (bumpMap.Sample( g_samLinear, input.Tex ).rbg-0.5)*2.0;
	Normal.z *= -1;
	float2 ShadowTex = input.LightViewPos.xy/input.LightViewPos.w;
	float4 ShadowData = PCF3X3Shadow(shadowMap, ShadowTex);
	//float4 ShadowData = PCF2X2Shadow(shadowMap, ShadowTex);
	//float4 ShadowData = NoPCFShadow(shadowMap, ShadowTex);
	
	float3 specular = pow(saturate(dot(reflect(normalize(-input.ViewDir), normalize(Normal)), LightDir)), 16.0);
	diffuse.rgb *= saturate(dot(normalize(Normal), LightDir))*ShadowData + AMBIENT;
	diffuse.rgb += specular;

	return saturate(diffuse);
}
//--------------------------------------------------------------------------------------
// Render the static mesh with shadow map. 
//--------------------------------------------------------------------------------------
technique10 techShadowFloor
{
	pass P0
	{
		SetDepthStencilState( DisableDepth, 0 );
		SetRasterizerState(rsCullCCW);

		SetVertexShader( CompileShader( vs_4_0, VSFloorMain() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PSFloorMain() ) );
	}
}

// Input to vertex shader
struct VsInSkybox
{
	float4 vertex: Vertex;
};

// Input to pixel shader
struct PsInSkybox
{
	float4 position: SV_Position;
	float3 texCoord: TexCoord;
};

PsInSkybox vsSkybox(VsInSkybox In)
{
	PsInSkybox Out;

	Out.position = float4(In.vertex.xy, 1, 1);
	Out.texCoord = mul(clipToEnv, Out.position);

	return Out;
}

float4 psSkybox(PsInSkybox In) : SV_Target
{
	return Env.Sample(g_samLinear, In.texCoord);
}

technique10 Skybox
{
	pass P0
	{
		SetBlendState(bsNoBlend, float4(0, 0, 0, 0), 0xFFFFFFFF);
		SetDepthStencilState( DisableDepth, 0 );

		SetVertexShader(CompileShader(vs_4_0, vsSkybox()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, psSkybox()));
	}
}