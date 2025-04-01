/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*   _       _   _       _   _       _   _       _     _ _ _ _   *
*  |_|     |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|  *
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _     *
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_   *
*  |_|     |_| |_|_ _ _|_| |_|     |_| |_|_ _ _|_|  _ _ _ _|_|  *
*  |_|     |_|   |_|_|_|   |_|     |_|   |_|_|_|   |_|_|_|_|    *
*                                                               *
*                     http://www.humus.name                     *
*                                                                *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.             *
*                                                                *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
  Copyright (c) 2017 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/RendererHLSLD3D12.h>
#ifdef __Framework3D3D12_H__

static void platform_log(const char* s)
{
#if defined(_WIN32)
  OutputDebugStringA(s);
#else
  STX_PRINT("%s", s);
#endif
}

void renderer_log(tr_log_type type, const char* msg, const char* component)
{
  switch(type) {
    case tr_log_type_info  : {LOG("[INFO]" << "[" << component << "] : " << msg);} break;
    case tr_log_type_warn  : {LOG("[WARN]"  << "[" << component << "] : " << msg);} break;
    case tr_log_type_debug : {LOG("[DEBUG]" << "[" << component << "] : " << msg);} break;
    case tr_log_type_error : {LOG("[ERORR]" << "[" << component << "] : " << msg);} break;
    default: break;
  }
}

RendererD3D12::~RendererD3D12(){}
#define LOG_FNLN_A LOG_FNLN
RendererD3D12::RendererD3D12(bool fullScreen, int argc, char *argv[])
		:RendererHLSLCg(argc, argv)
	{
LOG_FNLN_A;
    	width=STX_Service::GetWindowInstance()->Getwidth();
    	height=STX_Service::GetWindowInstance()->Getheight();

		SHADING_LANGUAGE_VERSION_MAJOR=5;
		SHADING_LANGUAGE_VERSION_MINOR=0;

		scaleBias2D=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);

	kImageCount = 3;

	m_renderer = nullptr;
	m_cmd_pool = nullptr;
	m_cmds = nullptr;
	m_uniform_buffer = nullptr;
	s_frame_count = 0;

    std::vector<const char*> device_layers;

    tr_renderer_settings settings = {0};

LOG_FNLN_A;
#if 1//defined(SDL_VIDEO_DRIVER_X11) || defined(SDL_VIDEO_DRIVER_WINDOWS)
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(STX_Service::GetSDLWindow(), &wmInfo);
#endif

LOG_FNLN_A;
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
    settings.handle.hinstance               = STX_Service::GetWindowInstance()->GetHINSTANCE();
    settings.handle.hwnd                    = STX_Service::GetWindowInstance()->GetHWND();
#elif defined(LINUX)
    settings.handle.dpy              	    = STX_Service::GetWindowInstance()->GetDisplay();
    settings.handle.win                     = STX_Service::GetWindowInstance()->GetWindow();
#else
    settings.handle.connection              = XGetXCBConnection(glfwGetX11Display());
    settings.handle.window                  = glfwGetX11Window(window);
#endif
    settings.width                          = width;
    settings.height                         = height;
    settings.swapchain.image_count          = kImageCount;
    settings.swapchain.sample_count         = tr_sample_count_8;
    settings.swapchain.color_format         = tr_format_b8g8r8a8_unorm;
    settings.swapchain.depth_stencil_format = tr_format_undefined;
    settings.log_fn                         = renderer_log;
#if defined(TINY_RENDERER_DX)
    settings.dx_debug_fn                    = 0;
    settings.instance_layers.count          = static_cast<uint32_t>(instance_layers.size());
    settings.instance_layers.names          = instance_layers.empty() ? nullptr : instance_layers.data();
#endif
LOG_FNLN_A;
    tr_create_renderer("Framework3App", &settings, &m_renderer);

LOG_FNLN_A;
    tr_create_cmd_pool(m_renderer, m_renderer->graphics_queue, false, &m_cmd_pool);
LOG_FNLN_A;
    tr_create_cmd_n(m_cmd_pool, false, kImageCount, &m_cmds);
LOG_FNLN_A;

    device=m_renderer->dx_device;
	mOk=true;
LOG_FNLN_A;
}

void RendererD3D12::resetVrtl(const unsigned int flags){}
void RendererD3D12::resetToDefaults(){}
void RendererD3D12::applyTextures(){}
void RendererD3D12::applySamplerStates(){}
void RendererD3D12::applyConstants(){}
BlendStateID RendererD3D12::addBlendState(const int srcFactorRGB, const int destFactorRGB,
				const int srcFactorAlpha,
				const int destFactorAlpha, const int blendModeRGB, const int blendModeAlpha,
				const int mask, const bool alphaToCoverage){return -1;}
DepthStateID RendererD3D12::addDepthState(const bool depthTest, const bool depthWrite,
				const int depthFunc, const bool stencilTest,
				const uint8 stencilMask, const int stencilFunc,
				const int stencilFail, const int depthFail,
				const int stencilPass){return -1;}
AlphaStateID RendererD3D12::addAlphaState(const int func, const float ref){return -1;}
				//, const IDataProcessor* p){return 0;}
RasterizerStateID RendererD3D12::addRasterizerStateVrtl(const int cullMode, const int fillMode,
						const bool multiSample,
						const bool scissor){return -1;}
TextureID RendererD3D12::addRenderTargetVrtl(const int width, const int height, const int depth,
				const int mipMapCount,
				const int arraySize, const FORMAT format, const int msaaSamples,
				const SamplerStateID samplerState, unsigned int flags){return -1;}
TextureID RendererD3D12::addRenderDepthVrtl(const int width, const int height, const int arraySize,
				const FORMAT format,
				const int msaaSamples, const SamplerStateID samplerState, unsigned int flags){return -1;}

							//, const IDataProcessor* p){return 0;}
void RendererD3D12::changeToMainFramebuffer(){}
void RendererD3D12::changeShader(const ShaderID shaderID){}
void RendererD3D12::changeCullFace(const int cullFace){}
void RendererD3D12::changeRasterizerState(const RasterizerStateID rasterizerState){}
void RendererD3D12::changeBlendStateVrtl(const BlendStateID blendState, const unsigned int sampleMask){}
void RendererD3D12::changeDepthStateVrtl(const DepthStateID depthState, const unsigned int stencilRef){}
void RendererD3D12::changeRenderTargetsVrtl(const TextureID *colorRTs, const unsigned int nRenderTargets,
						const TextureID depthRT,
				 		const int depthSlice, const int *slices){}
void RendererD3D12::changeAlphaState(const AlphaStateID alphaState){}
void RendererD3D12::clearRenderTarget(const TextureID renderTarget, const D3DXFROMWINEVECTOR4 &color,
						const int slice){}
void RendererD3D12::clearDepthTarget(const TextureID depthTarget, const float depth,
						const int slice){}
void RendererD3D12::setTexture(const char *textureName, const TextureID texture,
					const SamplerStateID samplerState,
					const int vtstream,
					const VertexBufferID vtbuffer
				)
{
}
void RendererD3D12::viewport(int x, int y, int viewportWidth, int viewportHeight)
{
    tr_cmd_set_viewport(cmd, 0, 0, STX_Service::GetWindowInstance()->Getwidth(), STX_Service::GetWindowInstance()->Getheight(), 0.0f, 1.0f);
    tr_cmd_set_scissor(cmd, 0, 0, STX_Service::GetWindowInstance()->Getwidth(), STX_Service::GetWindowInstance()->Getheight());
}
void RendererD3D12::clear(const bool clearColor, const bool clearDepth, const bool clearStencil,
				const float *color,
				const float depth, const unsigned int stencil)
{
    tr_clear_value clear_value = {color[0], color[1], color[2], color[3]};
    // ??? tr_cmd_clear_color_attachment(cmd, 0, &clear_value);
}
void RendererD3D12::Clear(bool color, bool depth, D3DXFROMWINEVECTOR4 colors)
{
    tr_clear_value clear_value = {colors.x, colors.y, colors.z, colors.w};
    // ??? tr_cmd_clear_color_attachment(cmd, 0, &clear_value);
}
void RendererD3D12::BeginSceneVrtl(bool aB)
{
    frameIdx = s_frame_count % m_renderer->settings.swapchain.image_count;

    image_acquired_fence = m_renderer->image_acquired_fences[frameIdx];
    image_acquired_semaphore = m_renderer->image_acquired_semaphores[frameIdx];
    render_complete_semaphores = m_renderer->render_complete_semaphores[frameIdx];

    tr_acquire_next_image(m_renderer, image_acquired_semaphore, image_acquired_fence);

    uint32_t swapchain_image_index = m_renderer->swapchain_image_index;
    render_target = m_renderer->swapchain_render_targets[swapchain_image_index];
    tr_cmd*  cmd = m_cmds[frameIdx];

    tr_begin_cmd(cmd);
    tr_cmd_render_target_transition(cmd, render_target, tr_texture_usage_present, tr_texture_usage_color_attachment);
    tr_cmd_set_viewport(cmd, 0, 0, STX_Service::GetWindowInstance()->Getwidth(), STX_Service::GetWindowInstance()->Getheight(), 0.0f, 1.0f);
    tr_cmd_set_scissor(cmd, 0, 0, STX_Service::GetWindowInstance()->Getwidth(), STX_Service::GetWindowInstance()->Getheight());
    tr_cmd_begin_render(cmd, render_target);
}
void RendererD3D12::EndSceneVrtl(bool aB)
{
	OnEndScene();
    tr_cmd_end_render(cmd);
    tr_cmd_render_target_transition(cmd, render_target, tr_texture_usage_color_attachment, tr_texture_usage_present);
    tr_end_cmd(cmd);

    tr_queue_submit(m_renderer->graphics_queue, 1, &cmd, 1, &image_acquired_semaphore, 1, &render_complete_semaphores);
    tr_queue_present(m_renderer->present_queue, 1, &render_complete_semaphores);

    tr_queue_wait_idle(m_renderer->graphics_queue);
}

void RendererD3D12::setShaderConstantRaw(const char *name, const void *data, const int size){
	int minConstant = 0;
	if(selectedShader==-1) return;
	int maxConstant = shaders[selectedShader].nConstants - 1;
	ConstantD3D12 *constants = shaders[selectedShader].constants.getArray();

	// Do a quick lookup in the sorted table with a binary search
	while (minConstant <= maxConstant){
		int currConstant = (minConstant + maxConstant) >> 1;
		if (currConstant == -1) return;
		int res = strcmp(name, constants[currConstant].name.c_str());
		if (res == 0){
			ConstantD3D12 *c = constants + currConstant;

			if (c->vsData){
				if (stx_memcmp(c->vsData, data, size)){
					stx_memcpy(c->vsData, data, size);
					shaders[selectedShader].vsDirty[c->vsBuffer] = true;
				}
			}/*
			if (c->gsData){
				if (stx_memcmp(c->gsData, data, size)){
					stx_memcpy(c->gsData, data, size);
					shaders[selectedShader].gsDirty[c->gsBuffer] = true;
				}
			}*/
			if (c->psData){
				if (stx_memcmp(c->psData, data, size)){
					stx_memcpy(c->psData, data, size);
					shaders[selectedShader].psDirty[c->psBuffer] = true;
				}
			}
			return;

		} else if (res > 0){
			minConstant = currConstant + 1;
		} else {
			maxConstant = currConstant - 1;
		}
	}

#ifdef _DEBUG
	char str[256];
	sprintf(str, "Invalid constant \"%s\"", name);
	LOG_PRINT("%s\n", str);
#endif
}

void RendererD3D12::changeVertexFormat(const VertexFormatID vertexFormatID)
{
	//if (vertexFormatID != currentVertexFormat)
	{
		if (vertexFormatID != VF_NONE)
		{
    			tr_cmd_bind_pipeline(cmd, vertexFormats[vertexFormatID].m_pipeline);
    			tr_cmd_bind_descriptor_sets(cmd, vertexFormats[vertexFormatID].m_pipeline, vertexFormats[vertexFormatID].m_desc_set);
		}
		currentVertexFormat = vertexFormatID;
	}
}
void RendererD3D12::changeVertexBufferVrtl(const int stream, const VertexBufferID vertexBuffer,
						const intptr offset)
{
	//if (vertexBuffer != currentVertexBuffers[stream])
	{
		if (vertexBuffer != VB_NONE)
    			tr_cmd_bind_vertex_buffers(cmd, 1, &vertexBuffers[vertexBuffer].m_rect_vertex_buffer);
		currentVertexBuffers[stream] = vertexBuffer;
	}
}
void RendererD3D12::changeIndexBuffer(const IndexBufferID indexBufferID)
{
	//if (indexBufferID != currentIndexBuffer)
	{
		if (indexBufferID != IB_NONE)
    			tr_cmd_bind_index_buffer(cmd, indexBuffers[indexBufferID].m_rect_index_buffer);
		currentIndexBuffer = indexBufferID;
	}
}

unsigned int RendererD3D12::drawArrays(const Primitives primitives, const int firstVertex, const int nVertices)
{
	apply();
	tr_cmd_draw(cmd, nVertices, firstVertex);
	reset(0xFFFF);
	nDrawCalls++;
	return 0;
}

unsigned int RendererD3D12::drawElements(const Primitives primitives, const int firstIndex, const int nIndices, const int firstVertex, const int nVertices)
{
	apply();
	tr_cmd_draw_indexed(cmd, nIndices, firstIndex);
	reset(0xFFFF);
	nDrawCalls++;
	return 0;
}

unsigned int RendererD3D12::DrawPrimitive(Primitives PrimitiveType,UINT StartVertex,UINT PrimitiveCount, const unsigned int flags)
{
	if(!((selectedVertexBuffers[0]>-1)))
		return 0;
	drawArrays(PrimitiveType, StartVertex,
#if 0
				PrimitiveCount*getIndicesCount(PrimitiveType)
#else
				getVertexCount(PrimitiveType,PrimitiveCount)
#endif
				);
	return 0;
}

unsigned int RendererD3D12::DrawIndexedPrimitive(Primitives PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount, const unsigned int flags)
{
	if(!((selectedVertexBuffers[0]>-1)&&(selectedIndexBuffer>-1)))
		return 0;
	drawElements(PrimitiveType, startIndex, getIndicesCount(PrimitiveType)*primCount, BaseVertexIndex, NumVertices);
	return 0;
}

D3D12_USAGE usage[] = {
	D3D12_USAGE_IMMUTABLE,
	D3D12_USAGE_DEFAULT,
	D3D12_USAGE_DYNAMIC,
};

VertexBufferID RendererHLSLD3D12::addVertexBufferVrtl(const int size, const BufferAccess bufferAccess, const void *data, VertexBufferID old){
	VertexBufferD3D12 vb(dev, ic);
	vb.size = size;

	D3D12_BUFFER_DESC desc;
	desc.Usage = usage[bufferAccess];
	desc.ByteWidth = size;
	desc.BindFlags = D3D12_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = (bufferAccess == DYNAMIC)? D3D12_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags = 0;

	D3D12_SUBRESOURCE_DATA vbData;
	vbData.pSysMem = data;
	vbData.SysMemPitch = 0;
	vbData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&desc, data? &vbData : 0, &vb.vertexBuffer))){
        LOG_PRINT("Couldn't create vertex buffer");
		return VB_NONE;
	}

	return vertexBuffers.add(vb);
}

IndexBufferID RendererHLSLD3D12::addIndexBufferVrtl(const unsigned int nIndices, const unsigned int indexSize, const BufferAccess bufferAccess, const void *data, IndexBufferID old){
	IndexBufferD3D12 ib(dev, ic);
	ib.indexSize = indexSize;
	ib.nIndices = nIndices;

	D3D12_BUFFER_DESC desc;
	desc.Usage = usage[bufferAccess];
	desc.ByteWidth = nIndices * indexSize;
	desc.BindFlags = D3D12_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = (bufferAccess == DYNAMIC)? D3D12_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags = 0;

	D3D12_SUBRESOURCE_DATA ibData;
	ibData.pSysMem = data;
	ibData.SysMemPitch = 0;
	ibData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&desc, data? &ibData : 0, &ib.indexBuffer))){
        LOG_PRINT("Couldn't create vertex buffer");
		return IB_NONE;
	}

	return indexBuffers.add(ib);
}

SamplerStateID RendererD3D12::addSamplerStateVrtl(const Filter filter, const AddressMode s,
					const AddressMode t,
					const AddressMode r, const float lod, const unsigned int maxAniso,
					const int compareFunc, const float *border_color)
{
	SamplerStateVulkan ss;
    tr_create_sampler(m_renderer, &ss.m_sampler);
    samplerStates.push_back(ss);
    return samplerStates.size()-1;
}

TextureID RendererD3D12::addTexture(Image3 &img, const SamplerStateID samplerState,
					unsigned int flags){return addTexture(&img, true, samplerState, 0);}
TextureID RendererD3D12::addTexture(Image3* img, const bool useMipMaps,
					const SamplerStateID samplerState,
					unsigned int flags)
{
    TextureVulkan tex;
    unsigned char* image_data = img->getPixels();
    int image_width = img->getWidth();
    int image_height = img->getHeight();
    int image_channels = getChannelCount(img->getFormat());
    int image_row_stride = image_width * image_channels;
    tr_create_texture_2d(m_renderer, image_width, image_height, tr_sample_count_1, tr_format_r8g8b8a8_unorm, tr_max_mip_levels, NULL, false, tr_texture_usage_sampled_image, &tex.m_texture);
    tr_util_update_texture_uint8(m_renderer->graphics_queue, image_width, image_height, image_row_stride, image_data, image_channels, tex.m_texture, NULL, NULL);

    //tr_create_uniform_buffer(m_renderer, 16 * sizeof(float), true, &m_uniform_buffer);
    //vertexFormats[currentVertexFormat].m_desc_set->descriptors[0].uniform_buffers[0] = m_uniform_buffer;
    vertexFormats[currentVertexFormat].m_desc_set->descriptors[1].textures[0]        = tex.m_texture;
    vertexFormats[currentVertexFormat].m_desc_set->descriptors[2].samplers[0]        = samplerStates[samplerState].m_sampler;
    tr_update_descriptor_set(m_renderer, vertexFormats[currentVertexFormat].m_desc_set);

	tex.samplerState=samplerState;

    textures.push_back(tex);
    return textures.size()-1;
}

VertexFormatID RendererD3D12::addVertexFormatVrtl(const FormatDesc *formatDesc, const unsigned int nAttribs,
							const ShaderID shader)
{
	VertexFormatVulkan vertexFormat;
    tr_vertex_layout vertex_layout = {};
    vertex_layout.attrib_count = nAttribs;
			unsigned int nGeneric  = 0;
			unsigned int nTexCoord = 0;

			for (unsigned int i = 0; i < nAttribs; i++)
			{
				if (formatDesc[i].type == TYPE_VERTEX)
				{
					nGeneric = 1;
					break;
				}
			}

			for (unsigned int i = 0; i < nAttribs; i++)
			{
				int stream = formatDesc[i].stream;
				switch (formatDesc[i].type)
				{
					case TYPE_GENERIC:
					vertex_layout.attribs[nGeneric].semantic = tr_semantic_undefined;
					case TYPE_TANGENT:
					vertex_layout.attribs[nGeneric].semantic = tr_semantic_tangent;
					case TYPE_BINORMAL:
					vertex_layout.attribs[nGeneric].semantic = tr_semantic_bitangent;
					vertexFormat.generic[nGeneric].stream = stream;
					vertexFormat.generic[nGeneric].size   = formatDesc[nGeneric].size;
					vertexFormat.generic[nGeneric].offset = vertexFormat.vertexSize[stream];
					vertexFormat.generic[nGeneric].format = formatDesc[nGeneric].format;
    					vertex_layout.attribs[nGeneric].location = nGeneric;
    					vertex_layout.attribs[nGeneric].offset   = vertexFormat.vertexSize[stream];
    					vertex_layout.attribs[nGeneric].binding  = 0;
					if(formatDesc[nGeneric].size==4) vertex_layout.attribs[nGeneric].format   = tr_format_r32g32b32a32_float;
					if(formatDesc[nGeneric].size==3) vertex_layout.attribs[nGeneric].format   = tr_format_r32g32b32_float;
					if(formatDesc[nGeneric].size==2) vertex_layout.attribs[nGeneric].format   = tr_format_r32g32_float;
					if(formatDesc[nGeneric].size==1) vertex_layout.attribs[nGeneric].format   = tr_format_r32_float;
					nGeneric++;
					break;
					case TYPE_VERTEX:
					vertexFormat.vertex.stream = stream;
					vertexFormat.vertex.size   = formatDesc[i].size;
					vertexFormat.vertex.offset = vertexFormat.vertexSize[stream];
					vertexFormat.vertex.format = formatDesc[i].format;
					vertex_layout.attribs[i].semantic = tr_semantic_position;
    					vertex_layout.attribs[i].location = i;
    					vertex_layout.attribs[i].offset   = vertexFormat.vertexSize[stream];
    					vertex_layout.attribs[i].binding  = 0;
					if(formatDesc[i].size==4) vertex_layout.attribs[i].format   = tr_format_r32g32b32a32_float;
					if(formatDesc[i].size==3) vertex_layout.attribs[i].format   = tr_format_r32g32b32_float;
					if(formatDesc[i].size==2) vertex_layout.attribs[i].format   = tr_format_r32g32_float;
					if(formatDesc[i].size==1) vertex_layout.attribs[i].format   = tr_format_r32_float;
					break;
					case TYPE_NORMAL:
					vertexFormat.normal.stream = stream;
					vertexFormat.normal.size   = formatDesc[i].size;
					vertexFormat.normal.offset = vertexFormat.vertexSize[stream];
					vertexFormat.normal.format = formatDesc[i].format;
					vertex_layout.attribs[i].semantic = tr_semantic_normal;
    					vertex_layout.attribs[i].location = i;
    					vertex_layout.attribs[i].offset   = vertexFormat.vertexSize[stream];
    					vertex_layout.attribs[i].binding  = 0;
					if(formatDesc[i].size==4) vertex_layout.attribs[i].format   = tr_format_r32g32b32a32_float;
					if(formatDesc[i].size==3) vertex_layout.attribs[i].format   = tr_format_r32g32b32_float;
					if(formatDesc[i].size==2) vertex_layout.attribs[i].format   = tr_format_r32g32_float;
					if(formatDesc[i].size==1) vertex_layout.attribs[i].format   = tr_format_r32_float;
					break;
					case TYPE_TEXCOORD:
					if(0==nTexCoord) vertex_layout.attribs[i].semantic = tr_semantic_texcoord0;
					if(1==nTexCoord) vertex_layout.attribs[i].semantic = tr_semantic_texcoord1;
					if(2==nTexCoord) vertex_layout.attribs[i].semantic = tr_semantic_texcoord2;
					if(3==nTexCoord) vertex_layout.attribs[i].semantic = tr_semantic_texcoord3;
					if(4==nTexCoord) vertex_layout.attribs[i].semantic = tr_semantic_texcoord4;
					if(5==nTexCoord) vertex_layout.attribs[i].semantic = tr_semantic_texcoord5;
					if(6==nTexCoord) vertex_layout.attribs[i].semantic = tr_semantic_texcoord6;
					if(7==nTexCoord) vertex_layout.attribs[i].semantic = tr_semantic_texcoord7;
					vertexFormat.texCoord[nTexCoord].stream = stream;
					vertexFormat.texCoord[nTexCoord].size	 = formatDesc[i].size;
					vertexFormat.texCoord[nTexCoord].offset = vertexFormat.vertexSize[stream];
    					vertex_layout.attribs[i].location = i;
    					vertex_layout.attribs[i].offset   = vertexFormat.vertexSize[stream];
    					vertex_layout.attribs[i].binding  = 0;
					if(formatDesc[i].size==4) vertex_layout.attribs[i].format   = tr_format_r32g32b32a32_float;
					if(formatDesc[i].size==3) vertex_layout.attribs[i].format   = tr_format_r32g32b32_float;
					if(formatDesc[i].size==2) vertex_layout.attribs[i].format   = tr_format_r32g32_float;
					if(formatDesc[i].size==1) vertex_layout.attribs[i].format   = tr_format_r32_float;
					vertexFormat.texCoord[nTexCoord].format = formatDesc[i].format;
					nTexCoord++;
					break;
				}
				vertexFormat.vertexSize[stream] += formatDesc[i].size * getFormatSize(formatDesc[i].format);
			}

			vertexFormat.maxGeneric = nGeneric;
			vertexFormat.maxTexCoord = nTexCoord;
/*
enum RENDERERAPI Primitives {
	PRIM_TRIANGLES	    = 0,
	PRIM_TRIANGLE_FAN   = 1,
	PRIM_TRIANGLE_STRIP = 2,
	PRIM_QUADS	    = 3,
	PRIM_LINES	    = 4,
	PRIM_LINE_STRIP     = 5,
	PRIM_LINE_LOOP	    = 6,
	PRIM_POINTS	    = 7,
};

    dx_PRIMITIVE_TOPOLOGY_POINT_LIST = 0,
    dx_PRIMITIVE_TOPOLOGY_LINE_LIST = 1,
    dx_PRIMITIVE_TOPOLOGY_LINE_STRIP = 2,
    dx_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST = 3,
    dx_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP = 4,
    dx_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN = 5,

  D3D_PRIMITIVE_TOPOLOGY_POINTLIST                     = 1,
  D3D_PRIMITIVE_TOPOLOGY_LINELIST                      = 2,
  D3D_PRIMITIVE_TOPOLOGY_LINESTRIP                     = 3,
  D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST                  = 4,
  D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP                 = 5,

typedef enum tr_primitive_topo {
    tr_primitive_topo_point_list = 0,
    tr_primitive_topo_line_list,
    tr_primitive_topo_line_strip,
    tr_primitive_topo_tri_list,
    tr_primitive_topo_tri_strip,
    tr_primitive_topo_tri_fan
} tr_primitive_topo;
*/
    tr_pipeline_settings pipeline_settings = {tr_primitive_topo_tri_list };
    tr_create_pipeline(m_renderer, shaders[shader].m_shader, &vertex_layout, vertexFormat.m_desc_set, m_renderer->swapchain_render_targets[0], &pipeline_settings, &vertexFormat.m_pipeline);

	currentVertexFormat=-1;
	if(vertexFormats.size())
		currentVertexFormat=vertexFormats.size();//???

    vertexFormats.push_back(vertexFormat);
    return vertexFormats.size()-1;
}

ShaderID RendererHLSLD3D12::addShaderHLSLVrtl(	const std::string& aFileName, 
						std::vector<std::string>& avMain, 
						std::vector<ShaderType>& avType)
{
	return SHADER_NONE;
}

ShaderID RendererHLSLD3D12::addHLSLShaderVrtl( 	std::vector<std::string>& avText,
                                            	std::vector<std::string>& avMain,
                                            	std::vector<ShaderType>& avType,
                                            	const unsigned int flags)
		{
    if(avMain.size()!=avType.size()) return SHADER_NONE;
			const char *vsText0=0;
			const char *fsText0=0;
		const char *vsName=0;
		const char *psName=0;
    for(unsigned int i=0;i<avType.size();i++)
    {
        switch(i)
        {
            case ePixelShader:
                fsText0=avText[i].c_str();
                psName=avMain[i].c_str();
                break;
            case eVertexShader:
                vsText0=avText[i].c_str();
                vsName=avMain[i].c_str();
                break;
            case eGeometryShader:
                break;
	        case eHullShader:
                break;
	        case eDomainShader:
                break;
	        case eComputeShader:
                break;
        };
    }
    const char* header=0;
		if (vsText0 == 0 && fsText0 == 0) return SHADER_NONE;

	std::string vsText;
	std::string fsText;
	const char * def="#define ROW_MAJOR row_major\n#define MVPSEMANTIC\n #define WSIGN +\n#undef TEX2D\n#define SAMPLE2D(TEX, TEXCOORD) TEX\.Load\(TEXCOORD\)\n#define SAMPLER2D Texture2D\n";
	if (def) vsText.append(def);
	if (def) fsText.append(def);
	if (vsText0) vsText.append(vsText0);
	if (fsText0) fsText.append(fsText0);
	//LOG_PRINT("\nvs:\n%s\n", vsText.c_str());
	//LOG_PRINT("\nfs:\n%s\n", fsText.c_str());

		const char * mainvs=vsName;
		const char * mainps=psName;

		ShaderD3D12  shader;
		stx_memset(&shader, 0, sizeof(ShaderD3D12));

		HRESULT hr=S_OK;
		ID3DBlob *shaderBuf = 0;
		ID3DBlob *errorsBuf = 0;
		ID3D12ShaderReflection *vsRefl = 0;
		ID3D12ShaderReflection *psRefl = 0;

			ID3DBlob* pVSBlob = 0;
			ID3DBlob* pPSBlob = 0;
			//LOG_PRINT("\n%s\n",vsText.c_str());
			hr = CompileShader(	0,
						vsText.c_str(),
						mainvs,
						&(shader.vertexShader),
						&pVSBlob);

			LOG_FNLN;
			LOG_PRINT("hr=%x\n",hr);

			if( FAILED( hr ) )
			DBG_HALT;
			if ((shader).vertexShader == 0) return SHADER_NONE;
			shader.inputSignature=pVSBlob;
			//LOG_PRINT("\n%s\n",fsText.c_str());
			hr = CompileShader(1, fsText.c_str(), mainps, &(shader.pixelShader),&pPSBlob);

			LOG_FNLN;
			LOG_PRINT("hr=%x\n",hr);
			if( FAILED( hr ) )
			DBG_HALT;

			if ((shader).pixelShader == 0) return SHADER_NONE;

			hr = D3DReflect(	pVSBlob->GetBufferPointer(),
						pVSBlob->GetBufferSize(),
						IID_ID3D12ShaderReflection,
						(void **)&vsRefl);

			LOG_FNLN;
			LOG_PRINT("hr=%x\n",hr);
			if( FAILED( hr ) )
			DBG_HALT;

			hr = D3DReflect(	pPSBlob->GetBufferPointer(),
						pPSBlob->GetBufferSize(),
						IID_ID3D12ShaderReflection,
						(void **)&psRefl);

			LOG_FNLN;
			LOG_PRINT("hr=%x\n",hr);
			if( FAILED( hr ) )
			DBG_HALT;

			D3D12_SHADER_DESC vsDesc;
			D3D12_SHADER_DESC gsDesc, psDesc;
			if (vsRefl)
			{
				vsRefl->GetDesc(&vsDesc);
				if (vsDesc.ConstantBuffers)
				{
					shader.nVSCBuffers = vsDesc.ConstantBuffers;
					shader.vsConstants = new ID3D12Buffer *[shader.nVSCBuffers];
					shader.vsConstMem = new ubyte *[shader.nVSCBuffers];
					shader.vsDirty = new bool[shader.nVSCBuffers];
				}
			}

			if (psRefl)
			{
				psRefl->GetDesc(&psDesc);
				if (psDesc.ConstantBuffers){
					shader.nPSCBuffers = psDesc.ConstantBuffers;
					shader.psConstants = new ID3D12Buffer *[shader.nPSCBuffers];
					shader.psConstMem = new ubyte *[shader.nPSCBuffers];
					shader.psDirty = new bool[shader.nPSCBuffers];
				}
			}

			D3D12_SHADER_BUFFER_DESC sbDesc;
			D3D12_BUFFER_DESC cbDesc;
			cbDesc.Usage = D3D12_USAGE_DEFAULT;
			cbDesc.BindFlags = D3D12_BIND_CONSTANT_BUFFER;
			cbDesc.CPUAccessFlags = 0;
			cbDesc.MiscFlags = 0;
			VArray <ConstantD3D12> constants;

			char buf[1024];
			for (unsigned int i = 0; i < (shader).nVSCBuffers; i++)
			{
				vsRefl->GetConstantBufferByIndex(i)->GetDesc(&sbDesc);
				cbDesc.ByteWidth = sbDesc.Size;
				device->CreateBuffer(&cbDesc, 0, &shader.vsConstants[i]);
				int limit=stx_min(sbDesc.Variables,20);
				shader.vsConstMem[i] = new ubyte[sbDesc.Size];
				for (unsigned int k = 0; k < limit; k++)
				{

					if(k>20) DBG_HALT;
					D3D12_SHADER_VARIABLE_DESC vDesc;

					vsRefl->GetConstantBufferByIndex(i)->GetVariableByIndex(k)->GetDesc(&vDesc);

                    stx_snprintf(buf, 256, "n:%s",vDesc.Name);
					OutputDebugStringA(buf);
                    stx_snprintf(buf, 256, "s:%d",vDesc.Size);
					OutputDebugStringA(buf);
                    stx_snprintf(buf, 256, "o:%d",vDesc.StartOffset);
					OutputDebugStringA(buf);

					ConstantD3D12 constant;
                    			constant.name = (char*)vDesc.Name;
					constant.vsData = shader.vsConstMem[i] + vDesc.StartOffset;
					constant.psData = 0;
					constant.vsBuffer = i;
					constant.psBuffer = -1;
					constants.push_back(constant);
				}
				(shader).vsDirty[i] = false;
			}
			unsigned int maxConst = constants.getCount();

			maxConst = constants.getCount();

			for (unsigned int i = 0; i < (shader).nPSCBuffers; i++)
			{
				psRefl->GetConstantBufferByIndex(i)->GetDesc(&sbDesc);
				cbDesc.ByteWidth = sbDesc.Size;
				device->CreateBuffer(&cbDesc, 0, &shader.psConstants[i]);
				int limit=stx_min(sbDesc.Variables,20);
				shader.psConstMem[i] = new ubyte[sbDesc.Size];
				for (unsigned int k = 0; k < limit; k++)
				{

					if(k>20) DBG_HALT;
					D3D12_SHADER_VARIABLE_DESC vDesc;

					psRefl->GetConstantBufferByIndex(i)->GetVariableByIndex(k)->GetDesc(&vDesc);

                    stx_snprintf(buf, 256, "n:%s",vDesc.Name);
					OutputDebugStringA(buf);
                    stx_snprintf(buf, 256, "s:%d",vDesc.Size);
					OutputDebugStringA(buf);
                    stx_snprintf(buf, 256, "o:%d",vDesc.StartOffset);
					OutputDebugStringA(buf);

					int merge = -1;
					for (unsigned int i = 0; i < maxConst; i++)
					{
						if (strcmp(constants[i].name.c_str(), vDesc.Name) == 0)
						{
							merge = i;
							break;
						}
					}
					if (merge < 0)
					{
						ConstantD3D12 constant((char*)vDesc.Name);
                                		//size_t length = stx_strlen(vDesc.Name);
						//constant.name=(char*)vDesc.Name;
						constant.vsData = 0;
						constant.psData = shader.psConstMem[i] + vDesc.StartOffset;
						constant.vsBuffer = -1;
						constant.psBuffer = i;
						constants.push_back(constant);
					}
					else
					{
						constants[merge].psData = shader.psConstMem[i] + vDesc.StartOffset;
						constants[merge].psBuffer = i;
					}
				}
				(shader).psDirty[i] = false;
			}

			shader.nConstants = constants.getCount();
			//shader.constants = new ConstantD3D12[shader.nConstants];
			//stx_memcpy(shader.constants, constants.getArray(), shader.nConstants * sizeof(ConstantD3D12));
			shader.constants=constants;
#if 1
			if(shader.constants.getCount()>1) qsort((void*)shader.constants.getArray(), shader.nConstants, sizeof(ConstantD3D12), constantCompD3D12);
#else
			std::sort((shader).constants.begin(), (shader).constants.end(), constantCompD3D12std);
#endif

			unsigned int nMaxVSRes = vsRefl? vsDesc.BoundResources : 0;
			unsigned int nMaxPSRes = psRefl? psDesc.BoundResources : 0;
			int maxResources = nMaxVSRes + nMaxPSRes;
			if (maxResources)
			{
				//shader.textures = (SamplerD3D12 *) malloc(maxResources * sizeof(Sampler));
				//shader.samplers = (SamplerD3D12 *) malloc(maxResources * sizeof(Sampler));
				shader.nTextures = 0;
				shader.nSamplers = 0;

				D3D12_SHADER_INPUT_BIND_DESC siDesc;
				for (unsigned int i = 0; i < nMaxVSRes; i++)
				{
					vsRefl->GetResourceBindingDesc(i, &siDesc);
					if (siDesc.Type == D3D_SIT_TEXTURE)
					{
						SamplerD3D12 sampler;
						shader.textures.push_back(sampler);
						shader.textures[shader.nTextures].name = siDesc.Name;
						(shader).textures[(shader).nTextures].vsIndex = siDesc.BindPoint;
						(shader).textures[(shader).nTextures].psIndex = -1;
						(shader).nTextures++;
					}
					else if (siDesc.Type == D3D_SIT_SAMPLER)
					{
						SamplerD3D12 sampler;
						shader.samplers.push_back(sampler);
						shader.samplers[shader.nSamplers].name = siDesc.Name;


						(shader).samplers[(shader).nSamplers].vsIndex = siDesc.BindPoint;
						(shader).samplers[(shader).nSamplers].psIndex = -1;
						(shader).nSamplers++;
					}
				}
				unsigned int maxTexture = (shader).nTextures;
				unsigned int maxSampler = (shader).nSamplers;

			maxTexture = (shader).nTextures;
			maxSampler = (shader).nSamplers;
			for (unsigned int i = 0; i < nMaxPSRes; i++)
			{
				psRefl->GetResourceBindingDesc(i, &siDesc);
				if (siDesc.Type == D3D_SIT_TEXTURE)
				{
					int merge = -1;
																for (unsigned int i = 0; i < maxTexture; i++){
																	if (strcmp(shader.textures[i].name.c_str(), siDesc.Name) == 0){
																		merge = i;
																		break;
																	}
											}
					if (merge < 0)
					{
						SamplerD3D12 sampler;
						sampler.name = siDesc.Name;
						sampler.vsIndex = -1;
						sampler.psIndex = siDesc.BindPoint;
						shader.textures.push_back(sampler);
						(shader).nTextures++;
					}
					else
					{
						(shader).textures[merge].psIndex = siDesc.BindPoint;
					}
				}
				else if (siDesc.Type == D3D_SIT_SAMPLER)
				{
					int merge = -1;
																for (unsigned int i = 0; i < maxSampler; i++){
																	if (strcmp(shader.samplers[i].name.c_str(), siDesc.Name) == 0){
																		merge = i;
																		break;												}
				}

									if (merge < 0){
						SamplerD3D12 sampler;
						sampler.name = siDesc.Name;
						sampler.vsIndex = -1;
						sampler.psIndex = siDesc.BindPoint;
						shader.samplers.push_back(sampler);
						shader.nSamplers++;
									} else {
										shader.samplers[merge].psIndex = siDesc.BindPoint;
				}
				}
			}

	//shader.textures = (SamplerD3D12 *) realloc(shader.textures, shader.nTextures * sizeof(SamplerD3D12));
	//shader.samplers = (SamplerD3D12 *) realloc(shader.samplers, shader.nSamplers * sizeof(SamplerD3D12));
#if 1
	if(shader.textures.getCount()>1) qsort((void*)shader.textures.getArray(), shader.nTextures, sizeof(SamplerD3D12), samplerCompD3D12);
	if(shader.samplers.getCount()>1) qsort((void*)shader.samplers.getArray(), shader.nSamplers, sizeof(SamplerD3D12), samplerCompD3D12);
#else
	std::sort((shader).textures.begin(), (shader).textures.end(), samplerCompD3D12std);
	std::sort((shader).samplers.begin(), (shader).samplers.end(), samplerCompD3D12std);
#endif
		}

		if (vsRefl) vsRefl->Release();
		if (psRefl) psRefl->Release();

		if(pPSBlob) pPSBlob->Release();
		shaders.push_back(shader);



		LOG_PRINT("Textures:\n");
		for(int i=0;i<shader.nTextures;i++)
		{
			LOG_PRINT("vsIndex=%x\n",(shader).textures[(shader).nTextures].vsIndex);
			LOG_PRINT("psIndex=%x\n",(shader).textures[(shader).nTextures].psIndex);
		}
		LOG_PRINT("Samplers:\n");
		for(int i=0;i<shader.nSamplers;i++)
		{
			LOG_PRINT("vsIndex=%x\n",(shader).samplers[(shader).nSamplers].vsIndex);
			LOG_PRINT("psIndex=%x\n",(shader).samplers[(shader).nSamplers].psIndex);
		}

		return shaders.size()-1;
	}

unsigned int RendererD3D12::drawPrimitivesVrtl(	const Primitives primitives,
				const unsigned int firstv, const unsigned int countv,
				const unsigned int firsti, const unsigned int counti,
				const ubyte* vbdata, const unsigned int vbsize,
				const ubyte* ibdata, const unsigned int ibsize)
{
	//LOG_FNLN;
	//LOG_PRINT("IMPLEMENT\n");
#if 0
	ID3D12Buffer *vertexBuffer=0;
	ID3D12Buffer *indexBuffer=0;

	const unsigned int bufferAccess = DYNAMIC;

	D3D12_BUFFER_DESC desc;
	desc.Usage = usage[bufferAccess];
	desc.ByteWidth = vbsize;
	desc.BindFlags = D3D12_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = (bufferAccess == DYNAMIC)? D3D12_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags = 0;

	D3D12_SUBRESOURCE_DATA vbData;
	vbData.pSysMem = vbdata;
	vbData.SysMemPitch = 0;
	vbData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&desc, data? &vbData : 0, &vertexBuffer))){
        LOG_PRINT("Couldn't create vertex buffer");
		return VB_NONE;
	}

	if(ibdata)
	{
	D3D12_BUFFER_DESC desc;
	desc.Usage = usage[bufferAccess];
	desc.ByteWidth = counti * ibsize;
	desc.BindFlags = D3D12_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = (bufferAccess == DYNAMIC)? D3D12_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags = 0;

	D3D12_SUBRESOURCE_DATA ibData;
	ibData.pSysMem = ibdata;
	ibData.SysMemPitch = 0;
	ibData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&desc, data? &ibData : 0, &indexBuffer))){
        LOG_PRINT("Couldn't create index buffer");
		return IB_NONE;
	}
	}

	UINT32 vertexSize = vbsize;
	UINT32 offset = 0;
	ic->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
	DXGI_FORMAT format = indexBuffers[indexBufferID].indexSize < 4? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	if(ibdata)
		ic->IASetIndexBuffer(&indexBuffer, format, 0);
	ic->IASetPrimitiveTopology(D3D12Prim[primitives]);

	if (ibdata)
		ic->DrawIndexed(counti, firsti, firstv);
        else
        	ic->Draw(countv, firstv);

	if (vertexBuffer) { vertexBuffer->Release(); vertexBuffer=NULL;}
	if (indexBuffer) { indexBuffer->Release(); indexBuffer=NULL;}
#else
	return -1;
#endif
}
#endif

