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
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/RendererVulkan.h>
#ifdef __Framework3Vulkan_H__

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

#if defined(TINY_RENDERER_VK)
VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug(
    VkDebugReportFlagsEXT      flags,
    VkDebugReportObjectTypeEXT objectType,
    uint64_t                   object,
    size_t                     location,
    int32_t                    messageCode,
    const char*                pLayerPrefix,
    const char*                pMessage,
    void*                      pUserData
)
{
    if( flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT ) {
        //LOG("[INFO]" << "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")");
    }
    else if( flags & VK_DEBUG_REPORT_WARNING_BIT_EXT ) {
        //LOG("[WARN]" << "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")");
    }
    else if( flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT ) {
        //LOG("[PERF]" << "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")");
    }
    else if( flags & VK_DEBUG_REPORT_ERROR_BIT_EXT ) {
        LOG("[ERROR]" << "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")");
    }
    else if( flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT ) {
        LOG("[DEBUG]" << "[" << pLayerPrefix << "] : " << pMessage << " (" << messageCode << ")");
    }
    return VK_FALSE;
}
#endif
RendererVulkan::~RendererVulkan(){}
#define LOG_FNLN_A LOG_FNLN
RendererVulkan::RendererVulkan(bool fullScreen, int argc, char *argv[])
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

    std::vector<const char*> instance_layers = {
#if defined(_DEBUG)
        "VK_LAYER_LUNARG_api_dump",
        "VK_LAYER_LUNARG_core_validation",
        "VK_LAYER_LUNARG_swapchain",
        "VK_LAYER_LUNARG_parameter_validation"
#endif
    };

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
#if defined(TINY_RENDERER_VK)
    settings.vk_debug_fn                    = vulkan_debug;
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

    device=m_renderer->vk_device;
	mOk=true;
LOG_FNLN_A;
}

void RendererVulkan::resetVrtl(const unsigned int flags){}
void RendererVulkan::resetToDefaults(){}
void RendererVulkan::applyTextures(){}
void RendererVulkan::applySamplerStates(){}
void RendererVulkan::applyConstants(){}
BlendStateID RendererVulkan::addBlendState(const int srcFactorRGB, const int destFactorRGB,
				const int srcFactorAlpha,
				const int destFactorAlpha, const int blendModeRGB, const int blendModeAlpha,
				const int mask, const bool alphaToCoverage){return -1;}
DepthStateID RendererVulkan::addDepthState(const bool depthTest, const bool depthWrite,
				const int depthFunc, const bool stencilTest,
				const uint8 stencilMask, const int stencilFunc,
				const int stencilFail, const int depthFail,
				const int stencilPass){return -1;}
AlphaStateID RendererVulkan::addAlphaState(const int func, const float ref){return -1;}
				//, const IDataProcessor* p){return 0;}
RasterizerStateID RendererVulkan::addRasterizerStateVrtl(const int cullMode, const int fillMode,
						const bool multiSample,
						const bool scissor){return -1;}
TextureID RendererVulkan::addRenderTargetVrtl(const int width, const int height, const int depth,
				const int mipMapCount,
				const int arraySize, const FORMAT format, const int msaaSamples,
				const SamplerStateID samplerState, unsigned int flags){return -1;}
TextureID RendererVulkan::addRenderDepthVrtl(const int width, const int height, const int arraySize,
				const FORMAT format,
				const int msaaSamples, const SamplerStateID samplerState, unsigned int flags){return -1;}

							//, const IDataProcessor* p){return 0;}
void RendererVulkan::changeToMainFramebuffer(){}
void RendererVulkan::changeShader(const ShaderID shaderID){}
void RendererVulkan::changeCullFace(const int cullFace){}
void RendererVulkan::changeRasterizerState(const RasterizerStateID rasterizerState){}
void RendererVulkan::changeBlendStateVrtl(const BlendStateID blendState, const unsigned int sampleMask){}
void RendererVulkan::changeDepthStateVrtl(const DepthStateID depthState, const unsigned int stencilRef){}
void RendererVulkan::changeRenderTargetsVrtl(const TextureID *colorRTs, const unsigned int nRenderTargets,
						const TextureID depthRT,
				 		const int depthSlice, const int *slices){}
void RendererVulkan::changeAlphaState(const AlphaStateID alphaState){}
void RendererVulkan::clearRenderTarget(const TextureID renderTarget, const D3DXFROMWINEVECTOR4 &color,
						const int slice){}
void RendererVulkan::clearDepthTarget(const TextureID depthTarget, const float depth,
						const int slice){}
void RendererVulkan::setTexture(const char *textureName, const TextureID texture,
					const SamplerStateID samplerState,
					const int vtstream,
					const VertexBufferID vtbuffer
				)
{
}
void RendererVulkan::viewport(int x, int y, int viewportWidth, int viewportHeight)
{
    tr_cmd_set_viewport(cmd, 0, 0, STX_Service::GetWindowInstance()->Getwidth(), STX_Service::GetWindowInstance()->Getheight(), 0.0f, 1.0f);
    tr_cmd_set_scissor(cmd, 0, 0, STX_Service::GetWindowInstance()->Getwidth(), STX_Service::GetWindowInstance()->Getheight());
}
void RendererVulkan::clear(const bool clearColor, const bool clearDepth, const bool clearStencil,
				const float *color,
				const float depth, const unsigned int stencil)
{
    tr_clear_value clear_value = {color[0], color[1], color[2], color[3]};
    // ??? tr_cmd_clear_color_attachment(cmd, 0, &clear_value);
}
void RendererVulkan::Clear(bool color, bool depth, D3DXFROMWINEVECTOR4 colors)
{
    tr_clear_value clear_value = {colors.x, colors.y, colors.z, colors.w};
    // ??? tr_cmd_clear_color_attachment(cmd, 0, &clear_value);
}
void RendererVulkan::BeginSceneVrtl(bool aB)
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
void RendererVulkan::EndSceneVrtl(bool aB)
{
	OnEndScene();
    tr_cmd_end_render(cmd);
    tr_cmd_render_target_transition(cmd, render_target, tr_texture_usage_color_attachment, tr_texture_usage_present);
    tr_end_cmd(cmd);

    tr_queue_submit(m_renderer->graphics_queue, 1, &cmd, 1, &image_acquired_semaphore, 1, &render_complete_semaphores);
    tr_queue_present(m_renderer->present_queue, 1, &render_complete_semaphores);

    tr_queue_wait_idle(m_renderer->graphics_queue);
}


void RendererVulkan::setShaderConstantRaw(const char *name, const void *data, const int size)
{
	uint32_t offset=0;
	for(unsigned int i=0;i<shaders[currentShader].uniforms.size();i++)
	{
		if(shaders[currentShader].uniforms[i].name==name)
		{
			offset=shaders[currentShader].uniforms[i].offset;
			break;
		}
	}
#if 0
	stx_memcpy(m_uniform_buffer->cpu_mapped_address, data+offset, size);
#else
	vkCmdPushConstants(
			cmd->vk_cmd_buf,
			vertexFormats[currentVertexFormat].m_pipeline->vk_pipeline_layout,
			VK_SHADER_STAGE_VERTEX_BIT,
			offset,
			size,
			data);
	vkCmdPushConstants(
			cmd->vk_cmd_buf,
			vertexFormats[currentVertexFormat].m_pipeline->vk_pipeline_layout,
			VK_SHADER_STAGE_FRAGMENT_BIT,
			offset,
			size,
			data);
#endif
}
void RendererVulkan::changeVertexFormat(const VertexFormatID vertexFormatID)
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
void RendererVulkan::changeVertexBufferVrtl(const int stream, const VertexBufferID vertexBuffer,
						const intptr offset)
{
	//if (vertexBuffer != currentVertexBuffers[stream])
	{
		if (vertexBuffer != VB_NONE)
    			tr_cmd_bind_vertex_buffers(cmd, 1, &vertexBuffers[vertexBuffer].m_rect_vertex_buffer);
		currentVertexBuffers[stream] = vertexBuffer;
	}
}
void RendererVulkan::changeIndexBuffer(const IndexBufferID indexBufferID)
{
	//if (indexBufferID != currentIndexBuffer)
	{
		if (indexBufferID != IB_NONE)
    			tr_cmd_bind_index_buffer(cmd, indexBuffers[indexBufferID].m_rect_index_buffer);
		currentIndexBuffer = indexBufferID;
	}
}

unsigned int RendererVulkan::drawArrays(const Primitives primitives, const int firstVertex, const int nVertices)
{
	apply();
	tr_cmd_draw(cmd, nVertices, firstVertex);
	reset(0xFFFF);
	nDrawCalls++;
	return 0;
}

unsigned int RendererVulkan::drawElements(const Primitives primitives, const int firstIndex, const int nIndices, const int firstVertex, const int nVertices)
{
	apply();
	tr_cmd_draw_indexed(cmd, nIndices, firstIndex);
	reset(0xFFFF);
	nDrawCalls++;
	return 0;
}

unsigned int RendererVulkan::DrawPrimitive(Primitives PrimitiveType,UINT StartVertex,UINT PrimitiveCount, const unsigned int flags)
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

unsigned int RendererVulkan::DrawIndexedPrimitive(Primitives PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount, const unsigned int flags)
{
	if(!((selectedVertexBuffers[0]>-1)&&(selectedIndexBuffer>-1)))
		return 0;
	drawElements(PrimitiveType, startIndex, getIndicesCount(PrimitiveType)*primCount, BaseVertexIndex, NumVertices);
	return 0;
}

VertexBufferID RendererVulkan::addVertexBufferVrtl(const int size, const BufferAccess bufferAccess,
							const void *data,
					 		VertexBufferID old)
{
    VertexBufferVulkan vb;
    uint64_t vertexStride = vertexFormats[currentVertexFormat].vertexSize[0];// ???
    tr_create_vertex_buffer(m_renderer, size, true, vertexStride, &vb.m_rect_vertex_buffer);
    stx_memcpy(vb.m_rect_vertex_buffer->cpu_mapped_address, data, size);
    vertexBuffers.push_back(vb);
    return vertexBuffers.size()-1;
}

IndexBufferID RendererVulkan::addIndexBufferVrtl(const unsigned int nIndices, const unsigned int indexSize,
				 const BufferAccess bufferAccess,
				 const void *data, IndexBufferID old)
{
    IndexBufferVulkan ib;
    uint64_t indexDataSize = nIndices*indexSize;
    tr_create_index_buffer(m_renderer, indexDataSize, true, (indexSize==2)?tr_index_type_uint16:tr_index_type_uint32, &ib.m_rect_index_buffer);
    stx_memcpy(ib.m_rect_index_buffer->cpu_mapped_address, data, indexDataSize);
    indexBuffers.push_back(ib);
    return indexBuffers.size()-1;
}

SamplerStateID RendererVulkan::addSamplerStateVrtl(const Filter filter, const AddressMode s,
					const AddressMode t,
					const AddressMode r, const float lod, const unsigned int maxAniso,
					const int compareFunc, const float *border_color)
{
	SamplerStateVulkan ss;
    tr_create_sampler(m_renderer, &ss.m_sampler);
    samplerStates.push_back(ss);
    return samplerStates.size()-1;
}

TextureID RendererVulkan::addTexture(Image3 &img, const SamplerStateID samplerState,
					unsigned int flags){return addTexture(&img, true, samplerState, 0);}
TextureID RendererVulkan::addTexture(Image3* img, const bool useMipMaps,
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

VertexFormatID RendererVulkan::addVertexFormatVrtl(const FormatDesc *formatDesc, const unsigned int nAttribs,
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

    VK_PRIMITIVE_TOPOLOGY_POINT_LIST = 0,
    VK_PRIMITIVE_TOPOLOGY_LINE_LIST = 1,
    VK_PRIMITIVE_TOPOLOGY_LINE_STRIP = 2,
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST = 3,
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP = 4,
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN = 5,

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

		// Load a GLSL shader (text)
		// Note: GLSL support requires vendor-specific extensions to be enabled and is not a core-feature of Vulkan

		VkShaderModule RendererVulkan::loadShaderGLSL(const char *shaderSrc_, VkShaderStageFlagBits stage)
		{
			//std::string shaderSrc = readTextFile(fileName);
			std::string shaderSrc = shaderSrc_;
			const char *shaderCode = shaderSrc.c_str();
			size_t size = strlen(shaderCode);
			assert(size > 0);

			VkShaderModule shaderModule;
			VkShaderModuleCreateInfo moduleCreateInfo;
			moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			moduleCreateInfo.pNext = NULL;
			moduleCreateInfo.codeSize = 3 * sizeof(uint32_t) + size + 1;
			moduleCreateInfo.pCode = (uint32_t*)malloc(moduleCreateInfo.codeSize);
			moduleCreateInfo.flags = 0;

			// Magic SPV number
			((uint32_t *)moduleCreateInfo.pCode)[0] = 0x07230203;
			((uint32_t *)moduleCreateInfo.pCode)[1] = 0;
			((uint32_t *)moduleCreateInfo.pCode)[2] = stage;
			stx_memcpy(((uint32_t *)moduleCreateInfo.pCode + 3), shaderCode, size + 1);

			//if(VK_SUCCESS!=
			vkCreateShaderModule(device, &moduleCreateInfo, NULL, &shaderModule);

			return shaderModule;
		}

#include <Framework3/importgl.h>
void RendererVulkan::reflectGLShaderVK(unsigned int aprogram, std::vector<std::string>& uniformnames, std::vector<unsigned int>& uniformoffsets)
{
	unsigned int offset=0;
	glUseProgramARB(aprogram);
				//checkGlError("");
	GLint uniformCount, maxLength;
	glGetProgramivARB(aprogram, GL_ACTIVE_UNIFORMS, &uniformCount);
				//checkGlError("");

	std::vector<char> bbuf(maxLength);
	char *name = (char *)&bbuf[0];

	#ifdef __APPLE__
	GLint prog=handle_to_uint(&aprogram);
	#else
	GLint prog=aprogram;
	#endif

	for(unsigned int i = 0; i < uniformCount; i++)
	{
		GLenum type;
		GLint length, size;
		#ifdef __APPLE__
		glGetActiveUniform(handle_to_uint(aprogram), i, maxLength, &length, &size, &type, name);
		#else
		glGetActiveUniformARB(aprogram, i, maxLength, &length, &size, &type, name);
		#endif
				//checkGlError("");
		uniformnames.push_back(name);
		uniformoffsets.push_back(offset);
		offset+=size;
	}
}

#include <Framework3/Hlsl2Glsl_Translate.h>
//include <Framework3/Glsl_Translate.h>
#include <Framework3/GLShaders.h>
ShaderID RendererGLSLGL_1_1::addShaderHLSLVrtl(	const std::string& aFileName, 
						                        std::vector<std::string>& avMain, 
						                        std::vector<ShaderType>& avType)
{
	return SHADER_NONE;
}

ShaderID RendererVulkan::addHLSLShaderVrtl(	std::vector<std::string>& avText,
                                            std::vector<std::string>& avMain,
                                            std::vector<ShaderType>& avType,
                                            const unsigned int flags)
		{
    if(avMain.size()!=avType.size()) return SHADER_NONE;
			const char *vsText=0;
			const char *fsText=0;
		const char *vsName=0;
		const char *psName=0;
    for(unsigned int i=0;i<avType.size();i++)
    {
        switch(i)
        {
            case ePixelShader:
                fsText=avText[i].c_str();
                psName=avMain[i].c_str();
                break;
            case eVertexShader:
                vsText=avText[i].c_str();
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
	ShaderVulkan shader;
//???
#if 0//defined(USE_HLSL_SHADERS)
	static bool Hlsl2Glsl_init=true;
if(Hlsl2Glsl_init)
{
	Hlsl2Glsl_init=false;
	Hlsl2Glsl_Initialize ();
}
/* ???
	ETargetGLSL_ES_100,	 // OpenGL ES 2.0
	ETargetGLSL_110,	 // OpenGL 2.0
	ETargetGLSL_120,
	ETargetGLSL_140,	 // OpenGL 3.1
	ETargetGLSL_ES_300,	 // OpenGL ES 3.0
	// ETargetGLSL_330,
*/
	const ETargetVersion version=ETargetGLSL_140;
	ShHandle parser;
	std::string vstext;
	bool ret=Hlsl2Glsl_Translate_ (true, vsText, vstext, vsName, parser, version);
	std::string fstext;
	     ret=Hlsl2Glsl_Translate_ (false, fsText, fstext, psName, parser, version);
#if 1
	{std::regex e("#version 140\\b");
	vstext=std::regex_replace (vstext,e,"#version 440 core");}

	{std::regex e("#version 140\\b");
	fstext=std::regex_replace (fstext,e,"#version 440 core");}
#endif
        shader.program = compileGLShaders(vstext.c_str(), fstext.c_str());
#else
	shader.program = compileGLShaders(vsText, fsText);
#endif
	std::vector<std::string>  uniformnames;
	std::vector<unsigned int> uniformoffsets;

	reflectGLShaderVK(shader.program, uniformnames, uniformoffsets);

	for(unsigned int i=0;i<uniformnames.size();i++)
	{
		UniformVulkan uniform;
		uniform.name=uniformnames[i];
		uniform.offset=uniformoffsets[i];
		shader.uniforms.push_back(uniform);
	}
	shader.m_shader->vk_vert=loadShaderGLSL(vsText, VK_SHADER_STAGE_VERTEX_BIT);
	shader.m_shader->vk_frag=loadShaderGLSL(fsText, VK_SHADER_STAGE_FRAGMENT_BIT);

	currentShader=-1;
	if(shaders.size())
		currentShader=shaders.size();//???

    shaders.push_back(shader);
    return shaders.size()-1;
}

unsigned int RendererVulkan::drawPrimitivesVrtl(	const Primitives primitives,
				const unsigned int firstv, const unsigned int countv,
				const unsigned int firsti, const unsigned int counti,
				const ubyte* vbdata, const unsigned int vbsize,
				const ubyte* ibdata, const unsigned int ibsize)
{
	//LOG_FNLN;
	//LOG_PRINT("IMPLEMENT\n");
#if 0
VkBuffer vertexBuffer;
VkDeviceMemory vertexBufferMemory;
VkBuffer indexBuffer;
VkDeviceMemory indexBufferMemory;

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(vertices[0]) * vertices.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex buffer!");
    }
	if(ibdata)
	{
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    stx_memcpy(data, indices.data(), (size_t) bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

    copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}

vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
	if(ibdata)
{
	if(ibsize==16)
		vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT16);
	else
		vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32);
}
	if(ibdata)
		vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
	else
	{
		uint32_t instanceCount=0;
		uint32_t firstInstance=0;
		vkCmdDraw(commandBuffers[i], 	static_cast<uint32_t>(countv),
						static_cast<uint32_t>(instanceCount),
						static_cast<uint32_t>(firstv),
						static_cast<uint32_t>(firstInstance);
	}
    vkDestroyBuffer(device, indexBuffer, nullptr);
    vkFreeMemory(device, indexBufferMemory, nullptr);

    vkDestroyBuffer(device, vertexBuffer, nullptr);
    vkFreeMemory(device, vertexBufferMemory, nullptr);
#endif
	return 0;
}
#endif
