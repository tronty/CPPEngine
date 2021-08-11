float noiseScale
<
    string UIWidget = "slider";
    string UIName = "noise scale";
    float UIMin = 0.0; float UIMax = 20.0; float UIStep = 0.01;
> = 5.0;

/*
    FX implementation of Ken Perlin's "Improved Noise"
    sgg 6/26/04
    http://mrl.nyu.edu/~perlin/noise/
*/

// permutation table
const int permutation[] = { 151,160,137,91,90,15,
131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

// gradients for 3d noise
const float3 g[] = {
    1,1,0,
    -1,1,0,
    1,-1,0,
    -1,-1,0,
    1,0,1,
    -1,0,1,
    1,0,-1,
    -1,0,-1,
    0,1,1,
    0,-1,1,
    0,1,-1,
    0,-1,-1,
    1,1,0,
    0,-1,1,
    -1,1,0,
    0,-1,-1,
};

// gradients for 4D noise
const float4 g4[] = {
	0, -1, -1, -1,
	0, -1, -1, 1,
	0, -1, 1, -1,
	0, -1, 1, 1,
	0, 1, -1, -1,
	0, 1, -1, 1,
	0, 1, 1, -1,
	0, 1, 1, 1,
	-1, -1, 0, -1,
	-1, 1, 0, -1,
	1, -1, 0, -1,
	1, 1, 0, -1,
	-1, -1, 0, 1,
	-1, 1, 0, 1,
	1, -1, 0, 1,
	1, 1, 0, 1,

	-1, 0, -1, -1,
	1, 0, -1, -1,
	-1, 0, -1, 1,
	1, 0, -1, 1,
	-1, 0, 1, -1,
	1, 0, 1, -1,
	-1, 0, 1, 1,
	1, 0, 1, 1,
	0, -1, -1, 0,
	0, -1, -1, 0,
	0, -1, 1, 0,
	0, -1, 1, 0,
	0, 1, -1, 0,
	0, 1, -1, 0,
	0, 1, 1, 0,
	0, 1, 1, 0,
};

/*const*/ float one = 1.0 / 256.0;

// Textures

texture permTextureT1
<
    string texturetype = "2D";
    string format = "l8";
	string function = "GeneratePermTexture";
	int width = 256, height = 1;
>;

texture permTexture2dT1
<
    string texturetype = "2D";
    string format = "a8r8g8b8";
	string function = "GeneratePermTexture2d";
	int width = 256, height = 256;
>;

texture gradTextureT1
<
    string texturetype = "2D";
	string format = "q8w8v8u8";
	string function = "GenerateGradTexture";
	int width = 16, height = 1;
>;

texture permGradTextureT1
<
    string texturetype = "2D";
	string format = "q8w8v8u8";
	string function = "GeneratePermGradTexture";
	int width = 256, height = 1;
>;

texture permGrad4dTextureT1
<
    string texturetype = "2D";
	string format = "q8w8v8u8";
	string function = "GeneratePermGrad4dTexture";
	int width = 256, height = 1;
>;

texture gradTexture4dT1
<
    string texturetype = "2D";
	string format = "q8w8v8u8";
	string function = "GenerateGradTexture4d";
	int width = 32, height = 1;
>;

// Functions to generate textures using CPU runtime

float4 GeneratePermTexture(float p : POSITION) : COLOR
{
	return permutation[p*256] / 255.0;
}

// 2d permutation texture for optimized version
int perm(int i)
{
	return permutation[i % 256];
}

float4 GeneratePermTexture2d(float2 p : POSITION) : COLOR
{
	p *= 256;
	int A = perm(p.x) + p.y;
	int AA = perm(A);
	int AB = perm(A + 1);
	int B =  perm(p.x + 1) + p.y;
	int BA = perm(B);
	int BB = perm(B + 1);
	return float4(AA, AB, BA, BB) / 255.0;
}

float3 GenerateGradTexture(float p : POSITION) : COLOR
{
	return g[p*16];
}

// permuted gradient texture for optimized version
float3 GeneratePermGradTexture(float p : POSITION) : COLOR
{
	return g[ permutation[p*256] % 16 ];
}

float3 GeneratePermGrad4dTexture(float p : POSITION) : COLOR
{
	return g4[ permutation[p*256] % 32 ];
}

float4 GenerateGradTexture4d(float p : POSITION) : COLOR
{
	return g4[p*32];
}

sampler permSampler1 = sampler_state
{
    texture = <permTextureT1>;
    AddressU  = Wrap;
    AddressV  = Clamp;
    MAGFILTER = POINT;
    MINFILTER = POINT;
    MIPFILTER = NONE;
};

sampler permSampler2d1 = sampler_state
{
    texture = <permTexture2dT1>;
    AddressU  = Wrap;
    AddressV  = Wrap;
    MAGFILTER = POINT;
    MINFILTER = POINT;
    MIPFILTER = NONE;
};

sampler gradSampler1 = sampler_state
{
    texture = <gradTextureT1>;
    AddressU  = Wrap;
    AddressV  = Clamp;
    MAGFILTER = POINT;
    MINFILTER = POINT;
    MIPFILTER = NONE;
};

sampler permGradSampler1 = sampler_state
{
    texture = <permGradTextureT1>;
    AddressU  = Wrap;
    AddressV  = Clamp;
    MAGFILTER = POINT;
    MINFILTER = POINT;
    MIPFILTER = NONE;
};

sampler permGrad4dSampler1 = sampler_state
{
    texture = <permGrad4dTextureT1>;
    AddressU  = Wrap;
    AddressV  = Clamp;
    MAGFILTER = POINT;
    MINFILTER = POINT;
    MIPFILTER = NONE;
};

sampler gradSampler4d1 = sampler_state
{
    texture = <gradTexture4dT1>;
    AddressU  = Wrap;
    AddressV  = Clamp;
    MAGFILTER = POINT;
    MINFILTER = POINT;
    MIPFILTER = NONE;
};

float3 fade111(float3 t)
{
	return t * t * t * (t * (t * 6 - 15) + 10); // new curve
//	return t * t * (3 - 2 * t); // old curve
}

float4 fade111(float4 t)
{
	return t * t * t * (t * (t * 6 - 15) + 10); // new curve
//	return t * t * (3 - 2 * t); // old curve
}

float perm111(float x)
{
	return tex1D(permSampler1, x);
}

float4 perm2d111(float2 p)
{
	return tex2D(permSampler2d1, p);
}

float grad111(float x, float3 p)
{
	//???return dot(tex1D(gradSampler1, x*16), p);
	return dot(tex1Dlod(gradSampler1, float4(x, 0, 0, 0)), p);	
}

float gradperm111(float x, float3 p)
{
	return dot(tex1D(permGradSampler1, x), p);
}

// 4d versions
float grad111(float x, float4 p)
{
	return dot(tex1D(gradSampler4d1, x), p);
}

float gradperm111(float x, float4 p)
{
	return dot(tex1D(permGrad4dSampler1, x), p);
}

// 3D noise
#if 0

// original version
float inoise111(float3 p)
{
	float3 P = fmod(floor(p), 256.0);	// FIND UNIT CUBE THAT CONTAINS POINT
	p -= floor(p);			    // FIND RELATIVE X,Y,Z OF POINT IN CUBE.
	float3 f = fade111(p);		       // COMPUTE FADE CURVES FOR EACH OF X,Y,Z.

	P = P / 256.0;

    // HASH COORDINATES OF THE 8 CUBE CORNERS
	float A = perm111(P.x) + P.y;
	float4 AA;
	AA.x = perm111(A) + P.z;
	AA.y = perm111(A + one) + P.z;
	float B =  perm111(P.x + one) + P.y;
	AA.z = perm111(B) + P.z;
	AA.w = perm111(B + one) + P.z;
 
	// AND ADD BLENDED RESULTS FROM 8 CORNERS OF CUBE
	return lerp( lerp( lerp( grad(perm111(AA.x    ), p ),
			     grad(perm111AA.z	 ), p + float3(-1, 0, 0) ), f.x),
		       lerp( grad(perm111(AA.y	  ), p + float3(0, -1, 0) ),
			     grad(perm111(AA.w	  ), p + float3(-1, -1, 0) ), f.x), f.y),
			     
		 lerp( lerp( grad(perm111(AA.x+one), p + float3(0, 0, -1) ),
			     grad(perm111(AA.z+one), p + float3(-1, 0, -1) ), f.x),
		       lerp( grad(perm111(AA.y+one), p + float3(0, -1, -1) ),
			     grad(perm111(AA.w+one), p + float3(-1, -1, -1) ), f.x), f.y), f.z);
}

#else

// optimized version
float inoise111(float3 p)
{
	float3 P = fmod(floor(p), 256.0);	// FIND UNIT CUBE THAT CONTAINS POINT
	p -= floor(p);			    // FIND RELATIVE X,Y,Z OF POINT IN CUBE.
	float3 f = fade111(p);		       // COMPUTE FADE CURVES FOR EACH OF X,Y,Z.

	P = P / 256.0;

    // HASH COORDINATES OF THE 8 CUBE CORNERS
	float4 AA = perm2d111(P.xy) + P.z;
 
	// AND ADD BLENDED RESULTS FROM 8 CORNERS OF CUBE
	return lerp( lerp( lerp( gradperm111(AA.x, p ),
			     gradperm111(AA.z, p + float3(-1, 0, 0) ), f.x),
		       lerp( gradperm111(AA.y, p + float3(0, -1, 0) ),
			     gradperm111(AA.w, p + float3(-1, -1, 0) ), f.x), f.y),
			     
		 lerp( lerp( gradperm111(AA.x+one, p + float3(0, 0, -1) ),
			     gradperm111(AA.z+one, p + float3(-1, 0, -1) ), f.x),
		       lerp( gradperm111(AA.y+one, p + float3(0, -1, -1) ),
			     gradperm111(AA.w+one, p + float3(-1, -1, -1) ), f.x), f.y), f.z);
}

#endif

// 4D noise
float inoise111(float4 p)
{
	float4 P = fmod(floor(p), 256.0);	// FIND UNIT HYPERCUBE THAT CONTAINS POINT
	p -= floor(p);			    // FIND RELATIVE X,Y,Z OF POINT IN CUBE.
	float4 f = fade111(p);		       // COMPUTE FADE CURVES FOR EACH OF X,Y,Z, W
	P = P / 256.0;

    // HASH COORDINATES OF THE 16 CORNERS OF THE HYPERCUBE
	float A = perm111(P.x) + P.y;
	float AA = perm111(A) + P.z;
	float AB = perm111(A + one) + P.z;
	float B =  perm111(P.x + one) + P.y;
	float BA = perm111(B) + P.z;
	float BB = perm111(B + one) + P.z;

	float AAA = perm111(AA)+P.w, AAB = perm111(AA+one)+P.w;
    float ABA = perm111(AB)+P.w, ABB = perm111(AB+one)+P.w;
    float BAA = perm111(BA)+P.w, BAB = perm111(BA+one)+P.w;
    float BBA = perm111(BB)+P.w, BBB = perm111(BB+one)+P.w;

	// INTERPOLATE DOWN
	return lerp(
				lerp( lerp( lerp( grad111(perm111(AAA), p ),
				  grad111(perm111(BAA), p + float4(-1, 0, 0, 0) ), f.x),
			    lerp( grad111(perm111(ABA), p + float4(0, -1, 0, 0) ),
				  grad111(perm111(BBA), p + float4(-1, -1, 0, 0) ), f.x), f.y),
				  
		      lerp( lerp( grad111(perm111(AAB), p + float4(0, 0, -1, 0) ),
				  grad111(perm111(BAB), p + float4(-1, 0, -1, 0) ), f.x),
			    lerp( grad111(perm111(ABB), p + float4(0, -1, -1, 0) ),
				  grad111(perm111(BBB), p + float4(-1, -1, -1, 0) ), f.x), f.y), f.z),
			    
				 lerp( lerp( lerp( grad111(perm111(AAA+one), p + float4(0, 0, 0, -1)),
				   grad111(perm111(BAA+one), p + float4(-1, 0, 0, -1) ), f.x),
			     lerp( grad111(perm111(ABA+one), p + float4(0, -1, 0, -1) ),
				   grad111(perm111(BBA+one), p + float4(-1, -1, 0, -1) ), f.x), f.y),
				   
		       lerp( lerp( grad111(perm111(AAB+one), p + float4(0, 0, -1, -1) ),
				   grad111(perm111(BAB+one), p + float4(-1, 0, -1, -1) ), f.x),
			     lerp( grad111(perm111(ABB+one), p + float4(0, -1, -1, -1) ),
				   grad111(perm111(BBB+one), p + float4(-1, -1, -1, -1) ), f.x), f.y), f.z), f.w);
}

// utility functions

// calculate gradient of noise (expensive!)
float3 inoiseGradient111(float3 p, float d)
{
	float f0 = inoise111(p);
	float fx = inoise111(p + float3(d, 0, 0));
	float fy = inoise111(p + float3(0, d, 0));
	float fz = inoise111(p + float3(0, 0, d));
	return float3(fx - f0, fy - f0, fz - f0) / d;
}

// fractal sum
float fBm111(float3 p, int octaves, float lacunarity = 2.0, float gain = 0.5)
{
	float freq = 1.0, amp = 0.5;
	float sum = 0;	
	for(int i=0; i<octaves; i++) {
		sum += inoise111(p*freq)*amp;
		freq *= lacunarity;
		amp *= gain;
	}
	return sum;
}

float turbulence111(float3 p, int octaves, float lacunarity = 2.0, float gain = 0.5)
{
	float sum = 0;
	float freq = 1.0, amp = 1.0;
	for(int i=0; i<octaves; i++) {
		sum += abs(inoise111(p*freq))*amp;
		freq *= lacunarity;
		amp *= gain;
	}
	return sum;
}

// Ridged multifractal
// See "Texturing & Modeling, A Procedural Approach", Chapter 12
float ridge111(float h, float offset)
{
    h = abs(h);
    h = offset - h;
    h = h * h;
    return h;
}

float ridgedmf111(float3 p, int octaves, float lacunarity = 2.0, float gain = 0.5, float offset = 1.0)
{
	float sum = 0;
	float freq = 1.0, amp = 0.5;
	float prev = 1.0;
	for(int i=0; i<octaves; i++) {
		float n = ridge111(inoise111(p*freq), offset);
		sum += n*amp*prev;
		prev = n;
		freq *= lacunarity;
		amp *= gain;
	}
	return sum;
}
#define ssnoise(x) ((2*inoise111(x)*0.5+0.5)-1)

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

