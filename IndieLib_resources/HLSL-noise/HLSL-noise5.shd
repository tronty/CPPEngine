//	Simplex 3D Noise 
//	by Ian McEwan, Ashima Arts
//

float3 mod(float3 x, float3 y)
{
  return x - y * floor(x / y);
}

float3 mod289(float3 x)
{
  return x - floor(x / 289.0) * 289.0;
}

float4 mod289(float4 x)
{
  return x - floor(x / 289.0) * 289.0;
}

float4 permute(float4 x){return mod289(((x*34.0)+1.0)*x);}
float4 taylorInvSqrt(float4 r){return 1.79284291400159 - 0.85373472095314 * r;}

float snoise(float3 v){ 
  const float2  C = float2(1.0/6.0, 1.0/3.0) ;
  const float4  D = float4(0.0, 0.5, 1.0, 2.0);

// First corner
  float3 i  = floor(v + dot(v, C.yyy) );
  float3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  float3 g = step(x0.yzx, x0.xyz);
  float3 l = 1.0 - g;
  float3 i1 = min( g.xyz, l.zxy );
  float3 i2 = max( g.xyz, l.zxy );

  //  x0 = x0 - 0. + 0.0 * C 
  float3 x1 = x0 - i1 + 1.0 * C.xxx;
  float3 x2 = x0 - i2 + 2.0 * C.xxx;
  float3 x3 = x0 - 1. + 3.0 * C.xxx;

// Permutations
  i = mod289(i); 
  float4 p = permute( permute( permute( 
             i.z + float4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + float4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + float4(0.0, i1.x, i2.x, 1.0 ));

// Gradients
// ( N*N points uniformly over a square, mapped onto an octahedron.)
  float n_ = 1.0/7.0; // N=7
  float3  ns = n_ * D.wyz - D.xzx;

  float4 j = p - 49.0 * floor(p * ns.z *ns.z);  //  mod(p,N*N)

  float4 x_ = floor(j * ns.z);
  float4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  float4 x = x_ *ns.x + ns.yyyy;
  float4 y = y_ *ns.x + ns.yyyy;
  float4 h = 1.0 - abs(x) - abs(y);

  float4 b0 = float4( x.xy, y.xy );
  float4 b1 = float4( x.zw, y.zw );

  float4 s0 = floor(b0)*2.0 + 1.0;
  float4 s1 = floor(b1)*2.0 + 1.0;
  float4 sh = -step(h, float4(0.0));

  float4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  float4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  float3 p0 = float3(a0.xy,h.x);
  float3 p1 = float3(a0.zw,h.y);
  float3 p2 = float3(a1.xy,h.z);
  float3 p3 = float3(a1.zw,h.w);

//Normalise gradients
  float4 norm = taylorInvSqrt(float4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  float4 m = max(0.6 - float4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, float4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
}

#define ssnoise(x) ((2*snoise(x)*0.5+0.5)-1)

float turbulence(int octaves, float3 P, float lacunarity, float gain)
{
  float sum = 0;
  float scale = 1;
  float totalgain = 1;
  for(int i=0;i<octaves;i++){
    sum += totalgain*ssnoise(P*scale);
    scale *= lacunarity;
    totalgain *= gain;
  }
  return abs(sum);
}

struct VS_INPUT
{
    float3 position	: POSITION;
	float3 normal  : NORMAL;
    float3 Binormal	: BINORMAL;
    float3 Tangent	: TANGENT;
	float2 color    : TEXCOORD0;
	float2 texture0 : TEXCOORD1;
};

struct VS_OUTPUT
{
    float4 hposition : POSITION;
	float2 v_texCoord2D  : TEXCOORD0;
	float3 v_texCoord3D  : TEXCOORD1;
	float4 color	 : COLOR0;
};
float4x4 TextureMatrix/*[0]*/;
float Displacement;
[Vertex shader]

ROW_MAJOR float4x4 modelViewProjection MVPSEMANTIC;
/*
 * Both 2D and 3D texture coordinates are defined, for testing purposes.
 */

VS_OUTPUT mainVS( const VS_INPUT IN)
{
    VS_OUTPUT OUT;

	OUT.hposition = mul( float4(IN.position, 1), modelViewProjection );

	OUT.v_texCoord2D = IN.texture0.xy;

	OUT.v_texCoord3D = IN.position.xyz;

	OUT.color = float4(0,1,0,1);//IN.color;

	return OUT;

}

VS_OUTPUT mainVVS( VS_INPUT IN )
{
	VS_OUTPUT OUT=(VS_OUTPUT)0;

    float4 noisePos = mul(TextureMatrix/*[0]*/ , float4(IN.position,1));

    float i = (ssnoise(noisePos.xyz/*, pg*/) + 1.0) * 0.5;
    color = float4(i, i, i, 1.0);

    // displacement along normal
    float4 position = float4(IN.position,1) + (float4(IN.normal, 1.0) * i * Displacement);
    //float4 position = IN.position + (IN.normal * i * Displacement);
    position.w = 1.0;

    OUT.hposition = mul(modelViewProjection , position);
    return OUT;
};

VS_OUTPUT VertexShaderFunction(VS_INPUT input)
{
	VS_OUTPUT OUT=(VS_OUTPUT)0;

    // calc the height displacement using 12 octaves of fBm
    float heightValue = fBm(input.position.xyz, 12, 2.0, 0.5);
    
    // add the height displacement to the vertex position
    input.position.xyz = input.position.xyz + heightValue;
    
    OUT.hposition = mul(modelViewProjection , float4(input.position,1));

    return OUT;
}
[Fragment shader]
float time;
float4 mainPS( VS_OUTPUT IN ) : COLOR
{
    

  /* These lines test, in order, 2D classic noise, 2D simplex noise,
   * 3D classic noise, 3D simplex noise, 4D classic noise, and finally
   * 4D simplex noise.
   * Everything but the 4D simpex noise will make some uniform
   * variables remain unused and be optimized away by the compiler,
   * so OpenGL will fail to bind them. It's safe to ignore these
   * warnings from the C program. The program is designed to work anyway.
   */
   float n = 0;
  //float n = noise(IN.v_texCoord2D * 32.0 + 240.0);
  //float n = snoise(IN.v_texCoord2D * 16.0);
  //float n = noise(float3(4.0 * IN.v_texCoord3D.xyz * (2.0 + sin(0.5 * time))));
//if(NoiseTechnique==1)
  n = ssnoise(float3(2.0 * IN.v_texCoord3D.xyz * noiseScale * (2.0 + sin(0.5 * time))));
/*else //if(NoiseTechnique==4)
  n = ssnoise(float3(2.0 * IN.v_texCoord3D.xyz * (2.0 + sin(0.5 * time))));
  */
  //float n = noise(float4(8.0 * IN.v_texCoord3D.xyz, 0.5 * time));
  //float n = snoise(float4(4.0 * IN.v_texCoord3D.xyz, 0.5 * time));

  float4 color = IN.color * float4(0.5 + 0.5 * float3(n, n, n), 1.0);

	return GammaCorrect4(color);
}
float4 fragmentShaderPNoise(VS_OUTPUT IN) : COLOR
{

  

  IN.v_texCoord3D=float3(IN.v_texCoord2D, 0);

  float n = ssnoise(float3(4.0 * IN.v_texCoord3D.xyz));
  float4 color = float4(0.5 + 0.85 * float3(n, n, n), 1.0);

	return GammaCorrect4(color);
}
float4 fragmentShaderWood(VS_OUTPUT IN) : COLOR
{
  

  IN.v_texCoord3D=float3(IN.v_texCoord2D, 0);

  float3 color1 = float3(0.658, 0.356, 0.0588);
  float3 color2 = float3(0.952, 0.713, 0.470);


  float x_v=IN.v_texCoord3D.x*0.6;
  float y_v=IN.v_texCoord3D.y*0.6;
  float z_v=IN.v_texCoord3D.z*0.6;

  float3 t1=float3(x_v*2,y_v*2,z_v*2);
  float3 t11=float3(x_v*2+1,y_v*2+1,z_v*2+1);
  float3 t2=float3(x_v*0,y_v*0,z_v*0);

  float temp_x=60*ssnoise(t1);
  float temp_y=60*ssnoise(t2);

  x_v=x_v-temp_x;
  y_v=y_v-temp_y;

  float3 diffuse=lerp(color1, color2, pow(cos(sqrt(x_v*x_v+y_v*y_v)+30*turbulence(68, t2, 2, 0.6))*0.5+0.5,3));
  float4 color = float4(diffuse, 1.0);
	return GammaCorrect4(color);
}
float4 fragmentShaderMarble(VS_OUTPUT IN) : COLOR
{
  

  IN.v_texCoord3D=float3(IN.v_texCoord2D, 0);

  float3 color1 = float3(0.1f, 0.8f, 0.2);
  float3 color2 = float3(0.8f, 0.8, 0.8f);

  float3 diffuse=lerp(color1, color2, cos(IN.v_texCoord3D.z*0.1+6*turbulence(5,IN.v_texCoord3D.xyz,2,0.6)));
  float4 color = float4(diffuse, 1.0);
	return GammaCorrect4(color);
}
float4 fragmentShaderFur(VS_OUTPUT IN) : COLOR
{
  

  IN.v_texCoord3D=float3(IN.v_texCoord2D, 0);

  float3 color1 = float3(0.8, 0.7, 0.0);
  float3 color2 = float3(0.6, 0.1, 0.0);

  float3 diffuse=lerp(color1, color2, clamp(0.7*turbulence(6,IN.v_texCoord3D.xyz,3,0.9), 0,1));

  float4 color = float4(diffuse, 1.0);
	return GammaCorrect4(color);
}
float4 fragmentShaderCloud(VS_OUTPUT IN) : COLOR
{
  

  IN.v_texCoord3D=float3(IN.v_texCoord2D, 0);

  float3 color1 = float3(0.1f, 0.1f, 0.99);
  float3 color2 = float3(0.8f, 0.8, 0.8f);

  float3 diffuse=lerp(color1, color2, cos(IN.v_texCoord3D.z*0.5+2*turbulence(6,IN.v_texCoord3D.xyz,2,0.5))*0.9);
  float4 color = float4(diffuse, 1.0);
	return GammaCorrect4(color);
}
float4 mainVPS( VS_OUTPUT IN )
{
	float4 color = IN.color;
	return GammaCorrect4(color);
};
#if 0
technique techniqueCloud0
{
    pass p0
    {
		//Sampler[0] = (permTexture); // Needed by pixel shader
		VertexShader = compile vs_3_0 mainVS();
		PixelShader  = compile ps_3_0 fragmentShaderCloud();
    }
}

technique techniqueFur0
{
    pass p0
    {
		//Sampler[0] = (permTexture); // Needed by pixel shader
		VertexShader = compile vs_3_0 mainVS();
		PixelShader  = compile ps_3_0 fragmentShaderFur();
    }
}

technique techniqueMarble0
{
    pass p0
    {
		//Sampler[0] = (permTexture); // Needed by pixel shader
		VertexShader = compile vs_3_0 mainVS();
		PixelShader  = compile ps_3_0 fragmentShaderMarble();
    }
}

technique techniqueWood0
{
    pass p0
    {
		//Sampler[0] = (permTexture); // Needed by pixel shader
		VertexShader = compile vs_3_0 mainVS();
		PixelShader  = compile ps_3_0 fragmentShaderWood();
    }
}

technique techniquePNoise0
{
    pass p0
    {
		//Sampler[0] = (permTexture); // Needed by pixel shader
		VertexShader = compile vs_3_0 mainVS();
		PixelShader  = compile ps_3_0 fragmentShaderPNoise();
    }
}

technique Technique0
{
    pass Pass0
    {
		Lighting = FALSE;
		//Sampler[0] = (Sampler); // Needed by pixel shader
		VertexShader = compile vs_3_0 mainVS();
		PixelShader  = compile ps_3_0 mainPS();
    }
}
/*
technique techniqueV0
{
	pass pass0
	{
	//Sampler[0] = (testTexture); // Needed by pixel shader
	VertexShader = compile vs_3_0 mainVVS( );
	PixelShader = compile ps_3_0 mainVPS( );
	}
}
*/
#endif
/*
technique Displacement
{
    pass Pass0
    {
        VertexShader = compile vs_3_0 VertexShaderFunction();
        PixelShader = compile ps_3_0 mainPS();
    }
}
*/

