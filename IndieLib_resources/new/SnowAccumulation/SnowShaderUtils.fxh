/*
	A couple of functions used by the different classes of snow techniques.
*/

/*
	Implements a perlin-style noise lookup using a 3D texture.  Returns a (-1..1) value
*/

#ifdef NOISESAMPLER
#ifndef NOISEOCTAVES
#define NOISEOCTAVES 3.0
#endif
FPTYPE noise(FPTYPE3 uvw)
{
	// $$ Note, these values, are tweakable.  I tweaked a little off the standard because I liked these better. YMMV
	FPTYPE sum = 0;
	FPTYPE amp = 1;
	FPTYPE3 p = uvw;
	FPTYPE lacunarity = 2;
	FPTYPE gain = 0.75;
	int octaves = (int)NOISEOCTAVES;
	for(int i=0;i<octaves;i++)
	{
		sum += amp * abs(tex3D(NOISESAMPLER, p.xyz));
		p *= lacunarity;
		amp *= gain;
	}
	return 2*sum-1;
}
#endif

/*
	This is used by the vertex shader in the SM3.0 technique to get the exposure for a vertex.
*/
#ifdef EXPOSURESAMPLER_VERTEX
float exposureAtVertex(float2 uv, float depth)
{
	float exposure = 0;
	if(uv.x > 0.0 && uv.x < 1.0 && uv.y > 0.0 && uv.y < 1.0 ) 
	{
		float sampledepth = tex2Dlod(EXPOSURESAMPLER_VERTEX,float4(uv.xy,0,0));
		if(depth < sampledepth+SnowBias)	// SnowBias is for depth fighting artifacts
		{
			exposure = 1;
		}
	}
	return exposure;
}
#endif

#ifdef EXPOSURESAMPLER_PIXEL
FPTYPE exposureAtPixel(FPTYPE2 uv, FPTYPE depth)
{
	FPTYPE exposure = 0;
	FPTYPE sampledepth = tex2D(EXPOSURESAMPLER_PIXEL,FPTYPE4(uv.xy,0,0));
	if(depth < sampledepth+SnowBias)	// SnowBias is for depth fighting artifacts
	{
		exposure = 1;
	}
	return exposure;
}
#endif

/*
	Lights a pixel using the exposure map, and noise to produce a really nice effect.
*/
FPTYPE4 SnowPixel(FPTYPE2 TextureUV, FPTYPE3 Normal, FPTYPE3 OrthoUVnDepth, FPTYPE3 ToView,
	const bool bNoiseNormal, const bool bNoiseIncline, const bool bMultiSample)
{
	// Prescale UV for noise, to have it tile a little more.
	FPTYPE3 noiseUVW = 30.0*OrthoUVnDepth.xyz;
	
	// Then just deterministically remap the coords to get 2 noise values to offset the normal
	FPTYPE3 noiseVec = FPTYPE3(noise(noiseUVW),0,noise(noiseUVW.yzx));
	
    // Calculate the per pixel exposure value.  multi-sampling around us to average
    FPTYPE exposure = 0;
    FPTYPE pixelOrthoDepth = OrthoUVnDepth.z;
	
	// current position
	exposure += exposureAtPixel(OrthoUVnDepth,pixelOrthoDepth);	

	if(bMultiSample)	// We want to sample the exposure map a couple times to blur the egdes a little
	{
		// A box around us.
		FPTYPE2 direction;	
		FPTYPE2 texelSize = SampleDistance / FPTYPE2(SceneWidth,SceneHeight);    
		direction = FPTYPE2(0,1);
		exposure += exposureAtPixel(OrthoUVnDepth.xy+direction*texelSize,pixelOrthoDepth);	
		direction = FPTYPE2(1,0);
		exposure += exposureAtPixel(OrthoUVnDepth.xy+direction*texelSize,pixelOrthoDepth);	
		direction = FPTYPE2(0,-1);
		exposure += exposureAtPixel(OrthoUVnDepth.xy+direction*texelSize,pixelOrthoDepth);	
		direction = FPTYPE2(-1,0);
		exposure += exposureAtPixel(OrthoUVnDepth.xy+direction*texelSize,pixelOrthoDepth);	
		
		exposure /= 5;
	}
	
	FPTYPE3 vNormalWorldSpace = ((2*Normal)-FPTYPE3(1,1,1));
	if(bNoiseNormal) vNormalWorldSpace = normalize(normalDistortionAmount*noiseVec + vNormalWorldSpace);	
	
	FPTYPE dotNorm = vNormalWorldSpace.y; // effectively dot the normal with the up vector
	if(dotNorm <= 0) dotNorm = 0;
	else 
	{
		if(bNoiseIncline)
		{
			dotNorm = saturate(dotNorm+dotNormalDistortionAmount*noiseVec.x);
		}
	}

    FPTYPE3 Vn = normalize(2*ToView-1);
    FPTYPE3 Hn = normalize(Vn + g_LightDir);
    FPTYPE hdn = dot(Hn,vNormalWorldSpace);
    FPTYPE ldn = dot(g_LightDir,vNormalWorldSpace);
    FPTYPE4 litVec = lit(ldn,hdn,SpecExpon);
    FPTYPE3 diffContrib = litVec.y * g_LightDiffuse;
    
    // $$4 Note this is calculated before the full exposure and is thus not based on incline...
    //		This is because we dont want to tie the specular highlight to the noise function too directly, it doesn't look good.
    FPTYPE3 specContrib = exposure*((litVec.y * litVec.z * Ks) * g_LightDiffuse);
    
    // now also tweak based on angle of incline, add in a base %
	exposure = BaseSnowPct + exposure*dotNorm*(1-BaseSnowPct);

    // choose a color based on exposure
    FPTYPE3 diffuseColor = exposure*g_SnowColor + (1-exposure)*tex2D(MeshTextureSampler, TextureUV);    
	
	FPTYPE diffuseSpec = saturate(g_LightAmbient+Kd)*diffContrib+Ks*specContrib;
	return FPTYPE4(diffuseColor.xyz*(diffuseSpec),1);
}

