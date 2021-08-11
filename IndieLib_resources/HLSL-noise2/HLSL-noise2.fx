float noiseScale
<
    string UIWidget = "slider";
    string UIName = "noise scale";
    float UIMin = 0.0; float UIMax = 20.0; float UIStep = 0.01;
> = 5.0;

//Fragment shader to write color for each pixel

//uniform sampler2D permTexture2;
texture2D permTextureT2; // This texture will be loaded by the application
sampler2D permTexture2 = sampler_state
{
    Texture = <permTextureT2>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

#define ONE222 0.00390625
#define ONEHALF222 0.001953125

//float3 v_texCoord3D;

float fade222(float t) {
  //return t*t*(3.0-2.0*t); // Old fade
  return t*t*t*(t*(t*6.0-15.0)+10.0); // Improved fade
}

float noise222(float3 P)
{
  float3 Pi = ONE222*floor(P)+ONEHALF222; 
				 
  float3 Pf = P-floor(P);
  
  // Noise contributions from (x=0, y=0), z=0 and z=1
  float perm00 = tex2D(permTexture2, Pi.xy).a ;
  float3  grad000 = tex2D(permTexture2, float2(perm00, Pi.z)).rgb * 4.0 - 1.0;
  float n000 = dot(grad000, Pf);
  float3  grad001 = tex2D(permTexture2, float2(perm00, Pi.z + ONE222)).rgb * 4.0 - 1.0;
  float n001 = dot(grad001, Pf - float3(0.0, 0.0, 1.0));

  // Noise contributions from (x=0, y=1), z=0 and z=1
  float perm01 = tex2D(permTexture2, Pi.xy+float2(0.0, ONE222)).a ;
  float3  grad010 = tex2D(permTexture2, float2(perm01, Pi.z)).rgb * 4.0 - 1.0;
  float n010 = dot(grad010, Pf - float3(0.0, 1.0, 0.0));
  float3  grad011 = tex2D(permTexture2, float2(perm01, Pi.z + ONE222)).rgb * 4.0 - 1.0;
  float n011 = dot(grad011, Pf - float3(0.0, 1.0, 1.0));

  // Noise contributions from (x=1, y=0), z=0 and z=1
  float perm10 = tex2D(permTexture2, Pi.xy + float2(ONE222, 0.0)).a ;
  float3  grad100 = tex2D(permTexture2, float2(perm10, Pi.z)).rgb * 4.0 - 1.0;
  float n100 = dot(grad100, Pf - float3(1.0, 0.0, 0.0));
  float3  grad101 = tex2D(permTexture2, float2(perm10, Pi.z + ONE222)).rgb * 4.0 - 1.0;
  float n101 = dot(grad101, Pf - float3(1.0, 0.0, 1.0));

  // Noise contributions from (x=1, y=1), z=0 and z=1
  float perm11 = tex2D(permTexture2, Pi.xy + float2(ONE222, ONE222)).a ;
  float3  grad110 = tex2D(permTexture2, float2(perm11, Pi.z)).rgb * 4.0 - 1.0;
  float n110 = dot(grad110, Pf - float3(1.0, 1.0, 0.0));
  float3  grad111 = tex2D(permTexture2, float2(perm11, Pi.z + ONE222)).rgb * 4.0 - 1.0;
  float n111 = dot(grad111, Pf - float3(1.0, 1.0, 1.0));

  // Blend contributions along x
  float4 n_x = lerp(float4(n000, n001, n010, n011), float4(n100, n101, n110, n111), fade222(Pf.x));

  // Blend contributions along y
  float2 n_xy = lerp(n_x.xy, n_x.zw, fade222(Pf.y));

  // Blend contributions along z
  float n_xyz = lerp(n_xy.x, n_xy.y, fade222(Pf.z));
 
  return n_xyz;
}

float turbulence222(int octaves, float3 P, float lacunarity, float gain)
{	
  float sum = 0;
  float scale = 1;
  float totalgain = 1;
  for(int i=0;i<octaves;i++){
    sum += totalgain*noise222(P*scale);
    scale *= lacunarity;
    totalgain *= gain;
  }
  return abs(sum);
}


#define ssnoise(x) ((2*noise222(x))-1)

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
    float3 normal	: NORMAL;
    float3 Binormal	: BINORMAL;
    float3 Tangent	: TANGENT;
    float3 Color	: TEXCOORD0;
    float2 texture0	: TEXCOORD1;
};

struct VS_OUTPUT
{
    float4 hposition : POSITION;
	float2 v_texCoord2D  : TEXCOORD0;
	float3 v_texCoord3D  : TEXCOORD1;
	float4 color	 : COLOR0;
};

struct PS_OUTPUT
{
	float4 color : COLOR;
};

ROW_MAJOR float4x4 modelViewProjection MVPSEMANTIC;
float time;

/*
 * Both 2D and 3D texture coordinates are defined, for testing purposes.
 */

VS_OUTPUT mainVS( const VS_INPUT IN)
{
    VS_OUTPUT OUT;

	OUT.hposition = mul( modelViewProjection, float4(IN.position, 1) );

	OUT.v_texCoord2D = IN.texture0.xy;

	OUT.v_texCoord3D = IN.position.xyz;

	OUT.color = float4(0,1,0,1);//IN.color;

	return OUT;

}

PS_OUTPUT mainPS( VS_OUTPUT IN )
{
    PS_OUTPUT OUT;

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

  OUT.color = IN.color * float4(0.5 + 0.5 * float3(n, n, n), 1.0);
  OUT.color=GammaCorrect4(OUT.color);

  return OUT;

}

PS_OUTPUT fragmentShaderPNoise(VS_OUTPUT IN)
{

  PS_OUTPUT OUT;

  IN.v_texCoord3D=float3(IN.v_texCoord2D, 0);

  float n = ssnoise(float3(4.0 * IN.v_texCoord3D.xyz));
  OUT.color = float4(0.5 + 0.85 * float3(n, n, n), 1.0);
  OUT.color=GammaCorrect4(OUT.color);
  return OUT;


}
PS_OUTPUT fragmentShaderWood(VS_OUTPUT IN)
{
  PS_OUTPUT OUT;

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
  OUT.color = float4(diffuse, 1.0);
  OUT.color=GammaCorrect4(OUT.color);
  return OUT;


}
PS_OUTPUT fragmentShaderMarble(VS_OUTPUT IN)
{
  PS_OUTPUT OUT;

  IN.v_texCoord3D=float3(IN.v_texCoord2D, 0);

  float3 color1 = float3(0.1f, 0.8f, 0.2);
  float3 color2 = float3(0.8f, 0.8, 0.8f);

  float3 diffuse=lerp(color1, color2, cos(IN.v_texCoord3D.z*0.1+6*turbulence(5,IN.v_texCoord3D.xyz,2,0.6)));
  OUT.color = float4(diffuse, 1.0);
  OUT.color=GammaCorrect4(OUT.color);
  return OUT;

}
PS_OUTPUT fragmentShaderFur(VS_OUTPUT IN)
{
  PS_OUTPUT OUT;

  IN.v_texCoord3D=float3(IN.v_texCoord2D, 0);

  float3 color1 = float3(0.8, 0.7, 0.0);
  float3 color2 = float3(0.6, 0.1, 0.0);

  float3 diffuse=lerp(color1, color2, clamp(0.7*turbulence(6,IN.v_texCoord3D.xyz,3,0.9), 0,1));

  OUT.color = float4(diffuse, 1.0);
  OUT.color=GammaCorrect4(OUT.color);
  return OUT;

}
PS_OUTPUT fragmentShaderCloud(VS_OUTPUT IN)
{
  PS_OUTPUT OUT;

  IN.v_texCoord3D=float3(IN.v_texCoord2D, 0);

  float3 color1 = float3(0.1f, 0.1f, 0.99);
  float3 color2 = float3(0.8f, 0.8, 0.8f);

  float3 diffuse=lerp(color1, color2, cos(IN.v_texCoord3D.z*0.5+2*turbulence(6,IN.v_texCoord3D.xyz,2,0.5))*0.9);
  OUT.color = float4(diffuse, 1.0);
  OUT.color=GammaCorrect4(OUT.color);
  return OUT;


}
       
#define TextureMatrix modelViewProjection
float Displacement=0.5;

[Vertex shader]
VS_OUTPUT main( VS_INPUT IN )
{
	VS_OUTPUT OUT=(VS_OUTPUT)0;
    float4 noisePos = mul(TextureMatrix/#[0]#/ , float4(IN.position,1));

    float i = (ssnoise(noisePos.xyz/#, pg#/) + 1.0) * 0.5;
    OUT.color = float4(i, i, i, 1.0);

    // displacement along normal
    float4 position = float4(IN.position,1) + (float4(IN.normal, 1.0) * i * Displacement);
    //float4 position = IN.position + (IN.normal * i * Displacement);
    position.w = 1.0;

    OUT.hposition = mul(modelViewProjection , position);
    return OUT;
};
[Fragment shader]
PS_OUTPUT main( VS_OUTPUT IN )
{
	PS_OUTPUT OUT;

	OUT.color = IN.color;
  OUT.color=GammaCorrect4(OUT.color);
	return OUT;
};
/*
//------------------------------------
vertexOutput VS(vertexInput IN)
{
    vertexOutput OUT;
    OUT.hPosition = mul(IN.position, worldViewProj);
    OUT.texcoord = IN.texcoord * noiseScale;
    OUT.wPosition = mul(IN.position, world).xyz * noiseScale;
    return OUT;
}

//-----------------------------------
float4 PS_inoise(vertexOutput IN): COLOR
{
	float3 p = IN.wPosition;
//	return abs(inoise(p));
//	return inoise(p);
	return inoise(p)*0.5+0.5;
//	return inoise(float3(IN.texcoord, 0.0))*0.5+0.5;
}
*/

