//	Simplex 3D Noise 
//	by Ian McEwan, Ashima Arts
//

vec3 mod(vec3 x, vec3 y)
{
  return x - y * floor(x / y);
}

vec3 mod289(vec3 x)
{
  return x - floor(x / 289.0) * 289.0;
}

vec4 mod289(vec4 x)
{
  return x - floor(x / 289.0) * 289.0;
}

vec4 permute(vec4 x){return mod289(((x*34.0)+1.0)*x);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}

float snoise(vec3 v){ 
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //  x0 = x0 - 0. + 0.0 * C 
  vec3 x1 = x0 - i1 + 1.0 * C.xxx;
  vec3 x2 = x0 - i2 + 2.0 * C.xxx;
  vec3 x3 = x0 - 1. + 3.0 * C.xxx;

// Permutations
  i = mod289(i); 
  vec4 p = permute( permute( permute( 
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients
// ( N*N points uniformly over a square, mapped onto an octahedron.)
  float n_ = 1.0/7.0; // N=7
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z *ns.z);  //  mod(p,N*N)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
}

#define ssnoise(x) ((2*snoise(x)*0.5+0.5)-1)

float turbulence(int octaves, vec3 P, float lacunarity, float gain)
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
float noiseScale=1.0;

struct VS_INPUT
{
    vec3 position	: POSITION;
	vec3 normal  : NORMAL;
    vec3 Binormal	: BINORMAL;
    vec3 Tangent	: TANGENT;
	vec2 color    : TEXCOORD0;
	vec2 texture0 : TEXCOORD1;
};

struct VS_OUTPUT
{
    vec4 hposition : POSITION;
	vec2 v_texCoord2D  : TEXCOORD0;
	vec3 v_texCoord3D  : TEXCOORD1;
	vec4 color	 : COLOR0;
};
mat4 TextureMatrix/*[0]*/;
float Displacement;
/*
 * Both 2D and 3D texture coordinates are defined, for testing purposes.
 */

[Vertex shader]

 mat4 modelViewProjection ;
VS_OUTPUT mainVS( const VS_INPUT IN)
{
    VS_OUTPUT OUT;

	__hposition = mul( vec4(_position, 1), modelViewProjection );

	__v_texCoord2D = _texture0.xy;

	__v_texCoord3D = _position.xyz;

	__color = vec4(0,1,0,1);//_color;

	return OUT;

}

VS_OUTPUT mainVVS( VS_INPUT IN )
{
	VS_OUTPUT OUT=(VS_OUTPUT)0;

    vec4 noisePos = mul(TextureMatrix/*[0]*/ , vec4(_position,1));

    float i = (ssnoise(noisePos.xyz/*, pg*/) + 1.0) * 0.5;
    __color = vec4(i, i, i, 1.0);

    // displacement along normal
    vec4 position = vec4(_position,1) + (vec4(_normal, 1.0) * i * Displacement);
    //vec4 position = _position + (_normal * i * Displacement);
    position.w = 1.0;

    __hposition = mul(modelViewProjection , position);
    return OUT;
};

[Fragment shader]
float time;
vec4 mainPS( VS_OUTPUT IN ) : COLOR
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
  //float n = noise(_v_texCoord2D * 32.0 + 240.0);
  //float n = snoise(_v_texCoord2D * 16.0);
  //float n = noise(vec3(4.0 * _v_texCoord3D.xyz * (2.0 + sin(0.5 * time))));
//if(NoiseTechnique==1)
  n = ssnoise(vec3(2.0 * _v_texCoord3D.xyz * noiseScale * (2.0 + sin(0.5 * time))));
/*else //if(NoiseTechnique==4)
  n = ssnoise(vec3(2.0 * _v_texCoord3D.xyz * (2.0 + sin(0.5 * time))));
  */
  //float n = noise(vec4(8.0 * _v_texCoord3D.xyz, 0.5 * time));
  //float n = snoise(vec4(4.0 * _v_texCoord3D.xyz, 0.5 * time));

  vec4 color = _color * vec4(0.5 + 0.5 * vec3(n, n, n), 1.0);
	return GammaCorrect4(color);
}

vec4 fragmentShaderPNoise(VS_OUTPUT IN) : COLOR
{

  

  _v_texCoord3D=vec3(_v_texCoord2D, 0);

  float n = ssnoise(vec3(4.0 * _v_texCoord3D.xyz));
  vec4 color = vec4(0.5 + 0.85 * vec3(n, n, n), 1.0);
	return GammaCorrect4(color);
}
vec4 fragmentShaderWood(VS_OUTPUT IN) : COLOR
{
  

  _v_texCoord3D=vec3(_v_texCoord2D, 0);

  vec3 color1 = vec3(0.658, 0.356, 0.0588);
  vec3 color2 = vec3(0.952, 0.713, 0.470);


  float x_v=_v_texCoord3D.x*0.6;
  float y_v=_v_texCoord3D.y*0.6;
  float z_v=_v_texCoord3D.z*0.6;

  vec3 t1=vec3(x_v*2,y_v*2,z_v*2);
  vec3 t11=vec3(x_v*2+1,y_v*2+1,z_v*2+1);
  vec3 t2=vec3(x_v*0,y_v*0,z_v*0);

  float temp_x=60*ssnoise(t1);
  float temp_y=60*ssnoise(t2);

  x_v=x_v-temp_x;
  y_v=y_v-temp_y;

  vec3 diffuse=lerp(color1, color2, pow(cos(sqrt(x_v*x_v+y_v*y_v)+30*turbulence(68, t2, 2, 0.6))*0.5+0.5,3));
  vec4 color = vec4(diffuse, 1.0);
	return GammaCorrect4(color);
}
vec4 fragmentShaderMarble(VS_OUTPUT IN) : COLOR
{
  

  _v_texCoord3D=vec3(_v_texCoord2D, 0);

  vec3 color1 = vec3(0.1f, 0.8f, 0.2);
  vec3 color2 = vec3(0.8f, 0.8, 0.8f);

  vec3 diffuse=lerp(color1, color2, cos(_v_texCoord3D.z*0.1+6*turbulence(5,_v_texCoord3D.xyz,2,0.6)));
  vec4 color = vec4(diffuse, 1.0);
	return GammaCorrect4(color);
}
vec4 fragmentShaderFur(VS_OUTPUT IN) : COLOR
{
  

  _v_texCoord3D=vec3(_v_texCoord2D, 0);

  vec3 color1 = vec3(0.8, 0.7, 0.0);
  vec3 color2 = vec3(0.6, 0.1, 0.0);

  vec3 diffuse=lerp(color1, color2, clamp(0.7*turbulence(6,_v_texCoord3D.xyz,3,0.9), 0,1));

  vec4 color = vec4(diffuse, 1.0);
	return GammaCorrect4(color);
}
vec4 fragmentShaderCloud(VS_OUTPUT IN) : COLOR
{
  

  _v_texCoord3D=vec3(_v_texCoord2D, 0);

  vec3 color1 = vec3(0.1f, 0.1f, 0.99);
  vec3 color2 = vec3(0.8f, 0.8, 0.8f);

  vec3 diffuse=lerp(color1, color2, cos(_v_texCoord3D.z*0.5+2*turbulence(6,_v_texCoord3D.xyz,2,0.5))*0.9);
  vec4 color = vec4(diffuse, 1.0);
	return GammaCorrect4(color);
}
       
vec4 mainVPS( VS_OUTPUT IN )
{
	vec4 color = _color;
	return GammaCorrect4(color);
};
/*	
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
/*
//------------------------------------
vertexOutput VS(vertexInput IN)
{
    vertexOutput OUT;
    __hPosition = mul(_position, worldViewProj);
    __texcoord = _texcoord * noiseScale;
    __wPosition = mul(_position, world).xyz * noiseScale;
    return OUT;
}

//-----------------------------------
vec4 PS_inoise(vertexOutput IN): COLOR
{
	vec3 p = _wPosition;
//	return abs(inoise(p));
//	return inoise(p);
	return inoise(p)*0.5+0.5;
//	return inoise(vec3(_texcoord, 0.0))*0.5+0.5;
}
*/

