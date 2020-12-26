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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "fft_512x512.h"

int CompileShaderFromFile( char* szFileName, std::string szEntryPoint, std::string szShaderModel, ID3DBlob** ppBlobOut );


void radix008A(CSFFT512x512_Plan* fft_plan,
			   ID3D11UnorderedAccessView* pUAV_Dst,
			   ShaderResourceViewID pSRV_Src,
			   unsigned int thread_count,
			   unsigned int istride)
{
    // Setup execution configuration
	unsigned int grid = thread_count / COHERENCY_GRANULARITY;

	// Buffers
	ShaderResourceViewID cs_srvs[1] = {pSRV_Src};
	IRenderer::GetRendererInstance()->CSSetShaderResources(0, 1, cs_srvs);

	ID3D11UnorderedAccessView* cs_uavs[1] = {pUAV_Dst};
	IRenderer::GetRendererInstance()->CSSetUnorderedAccessViews(0, 1, cs_uavs, (unsigned int*)(&cs_uavs[0]));

	// Shader
	if (istride > 1)
		IRenderer::GetRendererInstance()->CSSetShader(fft_plan->pRadix008A_CS, 0, 0);
	else
		IRenderer::GetRendererInstance()->CSSetShader(fft_plan->pRadix008A_CS2, 0, 0);

	// Execute
	IRenderer::GetRendererInstance()->Dispatch(grid, 1, 1);

	// Unbind resource
	cs_srvs[0] = 0;
	IRenderer::GetRendererInstance()->CSSetShaderResources(0, 1, cs_srvs);

	cs_uavs[0] = 0;
	IRenderer::GetRendererInstance()->CSSetUnorderedAccessViews(0, 1, cs_uavs, (unsigned int*)(&cs_uavs[0]));
}

void fft_512x512_c2c(CSFFT512x512_Plan* fft_plan,
					 ID3D11UnorderedAccessView* pUAV_Dst,
					 ShaderResourceViewID pSRV_Dst,
					 ShaderResourceViewID pSRV_Src)
{
	const unsigned int thread_count = fft_plan->slices * (512 * 512) / 8;
	ID3D11UnorderedAccessView* pUAV_Tmp = fft_plan->pUAV_Tmp;
	ShaderResourceViewID pSRV_Tmp = fft_plan->pSRV_Tmp;
	VertexBufferID cs_cbs[1];

	unsigned int istride = 512 * 512 / 8;
	cs_cbs[0] = fft_plan->pRadix008A_CB[0];
	IRenderer::GetRendererInstance()->CSSetConstantBuffers(0, 1, &cs_cbs[0]);
	radix008A(fft_plan, pUAV_Tmp, pSRV_Src, thread_count, istride);

	istride /= 8;
	cs_cbs[0] = fft_plan->pRadix008A_CB[1];
	IRenderer::GetRendererInstance()->CSSetConstantBuffers(0, 1, &cs_cbs[0]);
	radix008A(fft_plan, pUAV_Dst, pSRV_Tmp, thread_count, istride);

	istride /= 8;
	cs_cbs[0] = fft_plan->pRadix008A_CB[2];
	IRenderer::GetRendererInstance()->CSSetConstantBuffers(0, 1, &cs_cbs[0]);
	radix008A(fft_plan, pUAV_Tmp, pSRV_Dst, thread_count, istride);

	istride /= 8;
	cs_cbs[0] = fft_plan->pRadix008A_CB[3];
	IRenderer::GetRendererInstance()->CSSetConstantBuffers(0, 1, &cs_cbs[0]);
	radix008A(fft_plan, pUAV_Dst, pSRV_Tmp, thread_count, istride);

	istride /= 8;
	cs_cbs[0] = fft_plan->pRadix008A_CB[4];
	IRenderer::GetRendererInstance()->CSSetConstantBuffers(0, 1, &cs_cbs[0]);
	radix008A(fft_plan, pUAV_Tmp, pSRV_Dst, thread_count, istride);

	istride /= 8;
	cs_cbs[0] = fft_plan->pRadix008A_CB[5];
	IRenderer::GetRendererInstance()->CSSetConstantBuffers(0, 1, &cs_cbs[0]);
	radix008A(fft_plan, pUAV_Dst, pSRV_Tmp, thread_count, istride);
}

void create_cbuffers_512x512(CSFFT512x512_Plan* plan, unsigned int slices)
{
	// Create 6 cbuffers for 512x512 transform.

	
	cb_desc.Usage = USAGE_IMMUTABLE;
	cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb_desc.CPUAccessFlags = 0;
	cb_desc.MiscFlags = 0;    
	cb_desc.ByteWidth = 32;//sizeof(float) * 5;
	cb_desc.StructureByteStride = 0;

	
	cb_data.SysMemPitch = 0;
	cb_data.SysMemSlicePitch = 0;

	struct CB_Structure
	{
		unsigned int thread_count;
		unsigned int ostride;
		unsigned int istride;
		unsigned int pstride;
		float phase_base;
	};

	// Buffer 0
	const unsigned int thread_count = slices * (512 * 512) / 8;
	unsigned int ostride = 512 * 512 / 8;
	unsigned int istride = ostride;
	double phase_base = -TWO_PI / (512.0 * 512.0);
	
	CB_Structure cb_data_buf0 = {thread_count, ostride, istride, 512, (float)phase_base};
	cb_data.pSysMem = &cb_data_buf0;

	IRenderer::GetRendererInstance()->CreateBuffer(&cb_desc, &cb_data, &plan->pRadix008A_CB[0]);
	assert(plan->pRadix008A_CB[0]);

	// Buffer 1
	istride /= 8;
	phase_base *= 8.0;
	
	CB_Structure cb_data_buf1 = {thread_count, ostride, istride, 512, (float)phase_base};
	cb_data.pSysMem = &cb_data_buf1;

	IRenderer::GetRendererInstance()->CreateBuffer(&cb_desc, &cb_data, &plan->pRadix008A_CB[1]);
	assert(plan->pRadix008A_CB[1]);

	// Buffer 2
	istride /= 8;
	phase_base *= 8.0;
	
	CB_Structure cb_data_buf2 = {thread_count, ostride, istride, 512, (float)phase_base};
	cb_data.pSysMem = &cb_data_buf2;

	IRenderer::GetRendererInstance()->CreateBuffer(&cb_desc, &cb_data, &plan->pRadix008A_CB[2]);
	assert(plan->pRadix008A_CB[2]);

	// Buffer 3
	istride /= 8;
	phase_base *= 8.0;
	ostride /= 512;
	
	CB_Structure cb_data_buf3 = {thread_count, ostride, istride, 1, (float)phase_base};
	cb_data.pSysMem = &cb_data_buf3;

	IRenderer::GetRendererInstance()->CreateBuffer(&cb_desc, &cb_data, &plan->pRadix008A_CB[3]);
	assert(plan->pRadix008A_CB[3]);

	// Buffer 4
	istride /= 8;
	phase_base *= 8.0;
	
	CB_Structure cb_data_buf4 = {thread_count, ostride, istride, 1, (float)phase_base};
	cb_data.pSysMem = &cb_data_buf4;

	IRenderer::GetRendererInstance()->CreateBuffer(&cb_desc, &cb_data, &plan->pRadix008A_CB[4]);
	assert(plan->pRadix008A_CB[4]);

	// Buffer 5
	istride /= 8;
	phase_base *= 8.0;
	
	CB_Structure cb_data_buf5 = {thread_count, ostride, istride, 1, (float)phase_base};
	cb_data.pSysMem = &cb_data_buf5;

	IRenderer::GetRendererInstance()->CreateBuffer(&cb_desc, &cb_data, &plan->pRadix008A_CB[5]);
	assert(plan->pRadix008A_CB[5]);
}

void fft512x512_create_plan(CSFFT512x512_Plan* plan, unsigned int slices)
{
	plan->slices = slices;

	// Context
	IRenderer::GetRendererInstance()->GetImmediateContext(&plan->IRenderer::GetRendererInstance());
	assert(plan->IRenderer::GetRendererInstance());

	// Compute shaders
    ID3DBlob* pBlobCS = 0;
    ID3DBlob* pBlobCS2 = 0;

    CompileShaderFromFile("CSFFT/fft_512x512_c2c.hlsl", "Radix008A_CS", "cs_4_0", &pBlobCS);
    CompileShaderFromFile("CSFFT/fft_512x512_c2c.hlsl", "Radix008A_CS2", "cs_4_0", &pBlobCS2);
	assert(pBlobCS);
	assert(pBlobCS2);

    IRenderer::GetRendererInstance()->CreateComputeShader(pBlobCS->GetBufferPointer(), pBlobCS->GetBufferSize(), 0, &plan->pRadix008A_CS);
    IRenderer::GetRendererInstance()->CreateComputeShader(pBlobCS2->GetBufferPointer(), pBlobCS2->GetBufferSize(), 0, &plan->pRadix008A_CS2);
	assert(plan->pRadix008A_CS);
	assert(plan->pRadix008A_CS2);
    
    //////////////////SAFE_RELEASE(pBlobCS);
    //////////////////SAFE_RELEASE(pBlobCS2);

	// Constants
	// Create 6 cbuffers for 512x512 transform
	create_cbuffers_512x512(plan, slices);

	// Temp buffer
	
	buf_desc.ByteWidth = sizeof(float) * 2 * (512 * slices) * 512;
    buf_desc.Usage = USAGE_DEFAULT;
    buf_desc.BindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;
    buf_desc.CPUAccessFlags = 0;
    buf_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    buf_desc.StructureByteStride = sizeof(float) * 2;

	IRenderer::GetRendererInstance()->CreateBuffer(&buf_desc, 0, &plan->pBuffer_Tmp);
	assert(plan->pBuffer_Tmp);
}

void fft512x512_destroy_plan(CSFFT512x512_Plan* plan)
{
	//////////////////SAFE_RELEASE(plan->pSRV_Tmp);
	//////////////////SAFE_RELEASE(plan->pUAV_Tmp);
	//////////////////SAFE_RELEASE(plan->pBuffer_Tmp);
	//////////////////SAFE_RELEASE(plan->pRadix008A_CS);
	//////////////////SAFE_RELEASE(plan->pRadix008A_CS2);
	//////////////////SAFE_RELEASE(plan->IRenderer::GetRendererInstance());

	//for (int i = 0; i < 6; i++)
		//////////////////SAFE_RELEASE(plan->pRadix008A_CB[i]);
}

