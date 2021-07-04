/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

// Copyright (c) 2011 NVIDIA Corporation. All rights reserved.
//
// TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
// *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, NONINFRINGEMENT,IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA 
// OR ITS SUPPLIERS BE  LIABLE  FOR  ANY  DIRECT, SPECIAL,  INCIDENTAL,  INDIRECT,  OR  
// CONSEQUENTIAL DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS 
// OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY 
// OTHER PECUNIARY LOSS) ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, 
// EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
// Please direct any bugs or questions to SDKFeedback@nvidia.com

#include "ocean_simulator.h"

#define HALF_SQRT_2	0.7071068f
#define GRAV_ACCEL	981.0f	// The acceleration of gravity, cm/s^2

#define BLOCK_SIZE_X 16
#define BLOCK_SIZE_Y 16

// Generating gaussian random number with mean 0 and standard deviation 1.
float Gauss()
{
	float u1 = rand() / (float)RAND_MAX;
	float u2 = rand() / (float)RAND_MAX;
	if (u1 < 1e-6f)
		u1 = 1e-6f;
	return sqrtf(-2 * logf(u1)) * cosf(2*D3DXFROMWINE_PI * u2);
}

// Phillips Spectrum
// K: normalized wave vector, W: wind direction, v: wind velocity, a: amplitude constant
float Phillips(D3DXFROMWINEVECTOR2 K, D3DXFROMWINEVECTOR2 W, float v, float a, float dir_depend)
{
	// largest possible wave from constant wind of velocity v
	float l = v * v / GRAV_ACCEL;
	// damp out waves with very small length w << l
	float w = l / 1000;

	float Ksqr = K.x * K.x + K.y * K.y;
	float Kcos = K.x * W.x + K.y * W.y;
	float phillips = a * expf(-1 / (l * l * Ksqr)) / (Ksqr * Ksqr * Ksqr) * (Kcos * Kcos);

	// filter out waves moving opposite to wind
	if (Kcos < 0)
		phillips *= dir_depend;

	// damp out waves with very small length w << l
	return phillips * expf(-Ksqr * w * w);
}

void createBufferAndUAV(void* data, unsigned int byte_width, unsigned int byte_stride,
						VertexBufferID  ppBuffer, ID3D11UnorderedAccessView** ppUAV, ShaderResourceViewID* ppSRV)
{
	// Create buffer
	
	buf_desc.ByteWidth = byte_width;
    buf_desc.Usage = USAGE_DEFAULT;
    buf_desc.BindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;
    buf_desc.CPUAccessFlags = 0;
    buf_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    buf_desc.StructureByteStride = byte_stride;

	

	->CreateBuffer(&buf_desc, data != 0 ? &init_data : 0, ppBuffer);
	assert(*ppBuffer);
}

void createTextureAndViews(unsigned int width, unsigned int height, FORMAT format,
						   TextureID  ppTex, ShaderResourceViewID* ppSRV, RenderTargetViewID * ppRTV)
{
	// Create 2D texture
	Image3 img;
	img.create(format, width, height, 1, 1);
	ppTex=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());
	assert(*ppTex);
}

OceanSimulator::OceanSimulator(OceanParameter& params)
{
	// Height map H(0)
	int height_map_size = (params.dmap_dim + 4) * (params.dmap_dim + 1);
	D3DXFROMWINEVECTOR2* h0_data = new D3DXFROMWINEVECTOR2[height_map_size * sizeof(D3DXFROMWINEVECTOR2)];
	float* omega_data = new float[height_map_size * sizeof(float)];
	initHeightMap(params, h0_data, omega_data);

	m_param = params;
	int hmap_dim = params.dmap_dim;
	int input_full_size = (hmap_dim + 4) * (hmap_dim + 1);
	// This value should be (hmap_dim / 2 + 1) * hmap_dim, but we use full sized buffer here for simplicity.
	int input_half_size = hmap_dim * hmap_dim;
	int output_size = hmap_dim * hmap_dim;

	// For filling the buffer with zeroes.
	char* zero_data = new char[3 * output_size * sizeof(float) * 2];
	stx_memset(zero_data, 0, 3 * output_size * sizeof(float) * 2);

	// RW buffer allocations
	// H0
	unsigned int float2_stride = 2 * sizeof(float);
	createBufferAndUAV( h0_data, input_full_size * float2_stride, float2_stride, &m_pBuffer_Float2_H0, &m_pUAV_H0, &m_pSRV_H0);

	// Notice: The following 3 buffers should be half sized buffer because of conjugate symmetric input. But
	// we use full sized buffers due to the CS4.0 restriction.

	// Put H(t), Dx(t) and Dy(t) into one buffer because CS4.0 allows only 1 UAV at a time
	createBufferAndUAV( zero_data, 3 * input_half_size * float2_stride, float2_stride, &m_pBuffer_Float2_Ht, &m_pUAV_Ht, &m_pSRV_Ht);

	// omega
	createBufferAndUAV( omega_data, input_full_size * sizeof(float), sizeof(float), &m_pBuffer_Float_Omega, &m_pUAV_Omega, &m_pSRV_Omega);

	// Notice: The following 3 should be real number data. But here we use the complex numbers and C2C FFT
	// due to the CS4.0 restriction.
	// Put Dz, Dx and Dy into one buffer because CS4.0 allows only 1 UAV at a time
	createBufferAndUAV( zero_data, 3 * output_size * float2_stride, float2_stride, &m_pBuffer_Float_Dxyz, &m_pUAV_Dxyz, &m_pSRV_Dxyz);

	SAFE_DELETE_ARRAY(zero_data);
	SAFE_DELETE_ARRAY(h0_data);
	SAFE_DELETE_ARRAY(omega_data);

	// D3D11 Textures
	createTextureAndViews( hmap_dim, hmap_dim, FORMAT_RGBA32f, &m_pDisplacementMap, &m_pDisplacementSRV, &m_pDisplacementRTV);
	createTextureAndViews( hmap_dim, hmap_dim, FORMAT_RGBA16f, &m_pGradientMap, &m_pGradientSRV, &m_pGradientRTV);

	// Samplers
	
	sam_desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	sam_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sam_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sam_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sam_desc.MipLODBias = 0; 
	sam_desc.MaxAnisotropy = 1; 
	sam_desc.ComparisonFunc = D3D11_COMPARISON_NEVER; 
	sam_desc.BorderColor[0] = 1.0f;
	sam_desc.BorderColor[1] = 1.0f;
	sam_desc.BorderColor[2] = 1.0f;
	sam_desc.BorderColor[3] = 1.0f;
	sam_desc.MinLOD = -FLT_MAX;
	sam_desc.MaxLOD = FLT_MAX;
	IRenderer::GetRendererInstance()->CreateSamplerState(&sam_desc, &m_pPointSamplerState);
	assert(m_pPointSamplerState);

	// Compute shaders
    ID3DBlob* pBlobUpdateSpectrumCS = 0;

    CompileShaderFromFile("ocean_simulator_cs.hlsl", "UpdateSpectrumCS", "cs_4_0", &pBlobUpdateSpectrumCS);
	assert(pBlobUpdateSpectrumCS);

    IRenderer::GetRendererInstance()->CreateComputeShader(pBlobUpdateSpectrumCS->GetBufferPointer(), pBlobUpdateSpectrumCS->GetBufferSize(), 0, &m_pUpdateSpectrumCS);
	assert(m_pUpdateSpectrumCS);
    
    //////////////////SAFE_RELEASE(pBlobUpdateSpectrumCS);

	// Vertex_ & pixel shaders
    ID3DBlob* pBlobQuadVS = 0;
    ID3DBlob* pBlobUpdateDisplacementPS = 0;
    ID3DBlob* pBlobGenGradientFoldingPS = 0;

    CompileShaderFromFile("ocean_simulator_vs_ps.hlsl", "QuadVS", "vs_4_0", &pBlobQuadVS);
    CompileShaderFromFile("ocean_simulator_vs_ps.hlsl", "UpdateDisplacementPS", "ps_4_0", &pBlobUpdateDisplacementPS);
    CompileShaderFromFile("ocean_simulator_vs_ps.hlsl", "GenGradientFoldingPS", "ps_4_0", &pBlobGenGradientFoldingPS);
	assert(pBlobQuadVS);
	assert(pBlobUpdateDisplacementPS);
	assert(pBlobGenGradientFoldingPS);

    IRenderer::GetRendererInstance()->CreateVertexShader(pBlobQuadVS->GetBufferPointer(), pBlobQuadVS->GetBufferSize(), 0, &m_pQuadVS);
    IRenderer::GetRendererInstance()->CreatePixelShader(pBlobUpdateDisplacementPS->GetBufferPointer(), pBlobUpdateDisplacementPS->GetBufferSize(), 0, &m_pUpdateDisplacementPS);
    IRenderer::GetRendererInstance()->CreatePixelShader(pBlobGenGradientFoldingPS->GetBufferPointer(), pBlobGenGradientFoldingPS->GetBufferSize(), 0, &m_pGenGradientFoldingPS);
	assert(m_pQuadVS);
	assert(m_pUpdateDisplacementPS);
	assert(m_pGenGradientFoldingPS);
	//////////////////SAFE_RELEASE(pBlobUpdateDisplacementPS);
	//////////////////SAFE_RELEASE(pBlobGenGradientFoldingPS);

	// Input layout
	FormatDesc quad_layout_desc[] = {
		0, TYPE_VERTEX, FORMAT_FLOAT, 4,
	};
	m_pQuadLayout = IRenderer::GetRendererInstance()->addVertexFormat(quad_layout_desc, elementsOf(quad_layout_desc), -1);

	assert(m_pQuadLayout);

	//////////////////SAFE_RELEASE(pBlobQuadVS);

	// Quad vertex buffer
	
	vb_desc.ByteWidth = 4 * sizeof(D3DXFROMWINEVECTOR4);
	vb_desc.Usage = USAGE_IMMUTABLE;
	vb_desc.BindFlags = BIND_VERTEX_BUFFER;
	vb_desc.CPUAccessFlags = 0;
	vb_desc.MiscFlags = 0;

	float quad_verts[] =
	{
		-1, -1, 0, 1,
		-1,  1, 0, 1,
		 1, -1, 0, 1,
		 1,  1, 0, 1,
	};
	
	init_data.pSysMem = &quad_verts[0];
	init_data.SysMemPitch = 0;
	init_data.SysMemSlicePitch = 0;
	
	m_pQuadVB=IRenderer::GetRendererInstance()->addVertexBuffer(sizeof(float) * 16, STATIC, (const void *)&quad_verts[0]);
	assert(m_pQuadVB);

	// Constant buffers
	unsigned int actual_dim = m_param.dmap_dim;
	unsigned int input_width = actual_dim + 4;
	// We use full sized data here. The value "output_width" should be actual_dim/2+1 though.
	unsigned int output_width = actual_dim;
	unsigned int output_height = actual_dim;
	unsigned int dtx_offset = actual_dim * actual_dim;
	unsigned int dty_offset = actual_dim * actual_dim * 2;
	unsigned int immutable_consts[] = {actual_dim, input_width, output_width, output_height, dtx_offset, dty_offset};
	

	
	cb_desc.Usage = USAGE_IMMUTABLE;
	cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb_desc.CPUAccessFlags = 0;
	cb_desc.MiscFlags = 0;    
	cb_desc.ByteWidth = PAD16(sizeof(immutable_consts));
	IRenderer::GetRendererInstance()->CreateBuffer(&cb_desc, &init_cb0, &m_pImmutableCB);
	assert(m_pImmutableCB);

	VertexBufferID cbs[1] = {m_pImmutableCB};
	IRenderer::GetRendererInstance()->CSSetConstantBuffers(0, 1, cbs);
	IRenderer::GetRendererInstance()->PSSetConstantBuffers(0, 1, cbs);

	cb_desc.Usage = D3D11_USAGE_DYNAMIC;
	cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb_desc.MiscFlags = 0;    
	cb_desc.ByteWidth = PAD16(sizeof(float) * 3);
	IRenderer::GetRendererInstance()->CreateBuffer(&cb_desc, 0, &m_pPerFrameCB);
	assert(m_pPerFrameCB);

	// FFT
	fft512x512_create_plan(&m_fft_plan, IRenderer::GetRendererInstance(), 3);

#ifdef CS_DEBUG_BUFFER
	
	buf_desc.ByteWidth = 3 * input_half_size * float2_stride;
    buf_desc.Usage = D3D11_USAGE_STAGING;
    buf_desc.BindFlags = 0;
    buf_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    buf_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    buf_desc.StructureByteStride = float2_stride;

	IRenderer::GetRendererInstance()->CreateBuffer(&buf_desc, 0, &m_pDebugBuffer);
	assert(m_pDebugBuffer);
#endif
}

OceanSimulator::~OceanSimulator()
{
	fft512x512_destroy_plan(&m_fft_plan);

	//////////////////SAFE_RELEASE(m_pBuffer_Float2_H0);
	//////////////////SAFE_RELEASE(m_pBuffer_Float_Omega);
	//////////////////SAFE_RELEASE(m_pBuffer_Float2_Ht);
	//////////////////SAFE_RELEASE(m_pBuffer_Float_Dxyz);

	//////////////////SAFE_RELEASE(m_pPointSamplerState);

	//////////////////SAFE_RELEASE(m_pQuadVB);

	//////////////////SAFE_RELEASE(m_pUAV_H0);
	//////////////////SAFE_RELEASE(m_pUAV_Omega);
	//////////////////SAFE_RELEASE(m_pUAV_Ht);
	//////////////////SAFE_RELEASE(m_pUAV_Dxyz);

	//////////////////SAFE_RELEASE(m_pSRV_H0);
	//////////////////SAFE_RELEASE(m_pSRV_Omega);
	//////////////////SAFE_RELEASE(m_pSRV_Ht);
	//////////////////SAFE_RELEASE(m_pSRV_Dxyz);

	//////////////////SAFE_RELEASE(m_pDisplacementMap);
	//////////////////SAFE_RELEASE(m_pDisplacementSRV);
	//////////////////SAFE_RELEASE(m_pDisplacementRTV);

	//////////////////SAFE_RELEASE(m_pGradientMap);
	//////////////////SAFE_RELEASE(m_pGradientSRV);
	//////////////////SAFE_RELEASE(m_pGradientRTV);

	//////////////////SAFE_RELEASE(m_pUpdateSpectrumCS);
	//////////////////SAFE_RELEASE(m_pQuadVS);
	//////////////////SAFE_RELEASE(m_pUpdateDisplacementPS);
	//////////////////SAFE_RELEASE(m_pGenGradientFoldingPS);

	//////////////////SAFE_RELEASE(m_pQuadLayout);

	//////////////////SAFE_RELEASE(m_pImmutableCB);
	//////////////////SAFE_RELEASE(m_pPerFrameCB);

	//////////////////SAFE_RELEASE(IRenderer::GetRendererInstance());

#ifdef CS_DEBUG_BUFFER
	//////////////////SAFE_RELEASE(m_pDebugBuffer);
#endif
}


// Initialize the vector field.
// wlen_x: width of wave tile, in meters
// wlen_y: length of wave tile, in meters
void OceanSimulator::initHeightMap(OceanParameter& params, D3DXFROMWINEVECTOR2* out_h0, float* out_omega)
{
	int i, j;
	D3DXFROMWINEVECTOR2 K, Kn;

	D3DXFROMWINEVECTOR2 wind_dir;
	D3DXFROMWINEVec2Normalize(&wind_dir, &params.wind_dir);
	float a = params.wave_amplitude * 1e-7f;	// It is too small. We must scale it for editing.
	float v = params.wind_speed;
	float dir_depend = params.wind_dependency;

	int height_map_dim = params.dmap_dim;
	float patch_length = params.patch_length;

	// initialize random generator.
	srand(0);

	for (i = 0; i <= height_map_dim; i++)
	{
		// K is wave-vector, range [-|DX/W, |DX/W], [-|DY/H, |DY/H]
		K.y = (-height_map_dim / 2.0f + i) * (2 * D3DXFROMWINE_PI / patch_length);

		for (j = 0; j <= height_map_dim; j++)
		{
			K.x = (-height_map_dim / 2.0f + j) * (2 * D3DXFROMWINE_PI / patch_length);

			float phil = (K.x == 0 && K.y == 0) ? 0 : sqrtf(Phillips(K, wind_dir, v, a, dir_depend));

			out_h0[i * (height_map_dim + 4) + j].x = float(phil * Gauss() * HALF_SQRT_2);
			out_h0[i * (height_map_dim + 4) + j].y = float(phil * Gauss() * HALF_SQRT_2);

			// The angular frequency is following the dispersion relation:
			//            out_omega^2 = g*k
			// The equation of Gerstner wave:
			//            x = x0 - K/k * A * sin(dot(K, x0) - sqrt(g * k) * t), x is a 2D vector.
			//            z = A * cos(dot(K, x0) - sqrt(g * k) * t)
			// Gerstner wave shows that a point on a simple sinusoid wave is doing a uniform circular
			// motion with the center (x0, y0, z0), radius A, and the circular plane is parallel to
			// vector K.
			out_omega[i * (height_map_dim + 4) + j] = sqrtf(GRAV_ACCEL * sqrtf(K.x * K.x + K.y * K.y));
		}
	}
}

void OceanSimulator::updateDisplacementMap(float time)
{
	// ---------------------------- H(0) -> H(t), D(x, t), D(y, t) --------------------------------
	// Compute shader
	IRenderer::GetRendererInstance()->CSSetShader(m_pUpdateSpectrumCS, 0, 0);

	// Buffers
	ShaderResourceViewID cs0_srvs[2] = {m_pSRV_H0, m_pSRV_Omega};
	IRenderer::GetRendererInstance()->CSSetShaderResources(0, 2, cs0_srvs);

	ID3D11UnorderedAccessView* cs0_uavs[1] = {m_pUAV_Ht};
	IRenderer::GetRendererInstance()->CSSetUnorderedAccessViews(0, 1, cs0_uavs, (unsigned int*)(&cs0_uavs[0]));

	// Consts
	            
	IRenderer::GetRendererInstance()->Map(m_pPerFrameCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_res);
	assert(mapped_res.pData);
	float* per_frame_data = (float*)mapped_res.pData;
	// g_Time
	per_frame_data[0] = time * m_param.time_scale;
	// g_ChoppyScale
	per_frame_data[1] = m_param.choppy_scale;
	// g_GridLen
	per_frame_data[2] = m_param.dmap_dim / m_param.patch_length;
	IRenderer::GetRendererInstance()->Unmap(m_pPerFrameCB, 0);

	VertexBufferID cs_cbs[2] = {m_pImmutableCB, m_pPerFrameCB};
	IRenderer::GetRendererInstance()->CSSetConstantBuffers(0, 2, cs_cbs);

	// Run the CS
	unsigned int group_count_x = (m_param.dmap_dim + BLOCK_SIZE_X - 1) / BLOCK_SIZE_X;
	unsigned int group_count_y = (m_param.dmap_dim + BLOCK_SIZE_Y - 1) / BLOCK_SIZE_Y;
	IRenderer::GetRendererInstance()->Dispatch(group_count_x, group_count_y, 1);

	// Unbind resources for CS
	cs0_uavs[0] = 0;
	IRenderer::GetRendererInstance()->CSSetUnorderedAccessViews(0, 1, cs0_uavs, (unsigned int*)(&cs0_uavs[0]));
	cs0_srvs[0] = 0;
	cs0_srvs[1] = 0;
	IRenderer::GetRendererInstance()->CSSetShaderResources(0, 2, cs0_srvs);


	// ------------------------------------ Perform FFT -------------------------------------------
	fft_512x512_c2c(&m_fft_plan, m_pUAV_Dxyz, m_pSRV_Dxyz, m_pSRV_Ht);

	// --------------------------------- Wrap Dx, Dy and Dz ---------------------------------------
	// Push RT
	RenderTargetViewID  old_target;
	DepthStencilViewID old_depth;
	IRenderer::GetRendererInstance()->OMGetRenderTargets(1, &old_target, &old_depth); 
	D3DFROMWINEVIEWPORT9 old_viewport;
	unsigned int num_viewport = 1;
	IRenderer::GetRendererInstance()->RSGetViewports(&num_viewport, &old_viewport);

	D3DFROMWINEVIEWPORT9 new_vp = {0, 0, (float)m_param.dmap_dim, (float)m_param.dmap_dim, 0.0f, 1.0f};
	IRenderer::GetRendererInstance()->RSSetViewports(1, &new_vp);

	// Set RT
	RenderTargetViewID  rt_views[1] = {m_pDisplacementRTV};
	IRenderer::GetRendererInstance()->changeRenderTarget(rt_views, 0);

	// VS & PS
	IRenderer::GetRendererInstance()->VSSetShader(m_pQuadVS, 0, 0);
	IRenderer::GetRendererInstance()->PSSetShader(m_pUpdateDisplacementPS, 0, 0);

	// Constants
	VertexBufferID ps_cbs[2] = {m_pImmutableCB, m_pPerFrameCB};
	IRenderer::GetRendererInstance()->PSSetConstantBuffers(0, 2, ps_cbs);

	// Buffer resources
	ShaderResourceViewID ps_srvs[1] = {m_pSRV_Dxyz};
    IRenderer::GetRendererInstance()->PSSetShaderResources(0, 1, ps_srvs);

	// IA setup
	VertexBufferID vbs[1] = {m_pQuadVB};
	unsigned int strides[1] = {sizeof(D3DXFROMWINEVECTOR4)};
	unsigned int offsets[1] = {0};
	IRenderer::GetRendererInstance()->setVertexBuffer(0, 1, &vbs[0], &strides[0], &offsets[0]);

	IRenderer::GetRendererInstance()->setVertexFormat(m_pQuadLayout);
	IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_STRIP);

	// Perform draw call
	IRenderer::GetRendererInstance()->Draw(4, 0);

	// Unbind
	ps_srvs[0] = 0;
    IRenderer::GetRendererInstance()->PSSetShaderResources(0, 1, ps_srvs);


	// ----------------------------------- Generate Normal ----------------------------------------
	// Set RT
	rt_views[0] = m_pGradientRTV;
	IRenderer::GetRendererInstance()->changeRenderTarget(rt_views, 0);

	// VS & PS
	IRenderer::GetRendererInstance()->VSSetShader(m_pQuadVS, 0, 0);
	IRenderer::GetRendererInstance()->PSSetShader(m_pGenGradientFoldingPS, 0, 0);

	// Texture resource and sampler
	ps_srvs[0] = m_pDisplacementSRV;
    IRenderer::GetRendererInstance()->PSSetShaderResources(0, 1, ps_srvs);

	SamplerStateID  samplers[1] = {m_pPointSamplerState};
	IRenderer::GetRendererInstance()->PSSetSamplers(0, 1, &samplers[0]);

	// Perform draw call
	IRenderer::GetRendererInstance()->Draw(4, 0);

	// Unbind
	ps_srvs[0] = 0;
    IRenderer::GetRendererInstance()->PSSetShaderResources(0, 1, ps_srvs);

	// Pop RT
	IRenderer::GetRendererInstance()->RSSetViewports(1, &old_viewport);
	IRenderer::GetRendererInstance()->changeRenderTarget(&old_target, old_depth);
	//////////////////SAFE_RELEASE(old_target);
	//////////////////SAFE_RELEASE(old_depth);

	IRenderer::GetRendererInstance()->GenerateMips(m_pGradientSRV);

	// Define CS_DEBUG_BUFFER to enable writing a buffer into a file.
#ifdef CS_DEBUG_BUFFER
    {
		IRenderer::GetRendererInstance()->CopyResource(m_pDebugBuffer, m_pBuffer_Float_Dxyz);
        
        IRenderer::GetRendererInstance()->Map(m_pDebugBuffer, 0, D3D11_MAP_READ, 0, &mapped_res);
        
		// set a break point below, and drag MappedResource.pData into in your Watch window
		// and cast it as (float*)

		// Write to disk
		D3DXFROMWINEVECTOR2* v = (D3DXFROMWINEVECTOR2*)mapped_res.pData;

		FILE* fp = fopen("./tmp/Ht_raw.dat", "wb");
		fwrite(v, 512*512*sizeof(float)*2*3, 1, fp);
		fclose(fp);

		IRenderer::GetRendererInstance()->Unmap(m_pDebugBuffer, 0);
    }
#endif
}

ShaderResourceViewID OceanSimulator::getD3D11DisplacementMap()
{
	return m_pDisplacementSRV;
}

ShaderResourceViewID OceanSimulator::getD3D11GradientMap()
{
	return m_pGradientSRV;
}


const OceanParameter& OceanSimulator::getParameters()
{
	return m_param;
}

