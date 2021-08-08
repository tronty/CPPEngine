
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

#pragma once

#include "../Util/StringHash.h"
#include "../Platform.h"
#include "../Util/Image.h"

// Forward declaration of types
/*
typedef NulledPointer <struct SContext         > Context;
typedef NulledPointer <struct STexture         > Texture;
typedef NulledPointer <struct SRenderSetup     > RenderSetup;
typedef NulledPointer <struct SShader          > Shader;
typedef NulledPointer <struct SSamplerState    > SamplerState;
typedef NulledPointer <struct SBlendState      > BlendState;
typedef NulledPointer <struct SVertexSetup     > VertexSetup;
typedef NulledPointer <struct SVertexFormat    > VertexFormat;
typedef NulledPointer <struct SBuffer          > Buffer;
*/

typedef struct SDevice*          Device;
typedef struct SContext*         Context;
typedef struct STexture*         Texture;
typedef struct SBuffer*          Buffer;
typedef struct SResourceTable*   ResourceTable;
typedef struct SSamplerTable*    SamplerTable;
typedef struct SRenderPass*      RenderPass;
typedef struct SRenderSetup*     RenderSetup;
typedef struct SRootSignature*   RootSignature;
typedef struct SPipeline*        Pipeline;
typedef struct SShader*          Shader;
typedef struct SBlendState*      BlendState;
typedef struct SVertexSetup*     VertexSetup;

const uint BUFFER_FRAMES = 2;
const uint MAX_QUERY_COUNT = 256;


/* Enums */
enum HeapType
{
	HEAP_DEFAULT,
	HEAP_UPLOAD,
	HEAP_READBACK,
};

enum Usage
{
	NONE             = 0,
	VERTEX_BUFFER    = 1 << 0,
	INDEX_BUFFER     = 1 << 1,
	SHADER_RESOURCE  = 1 << 2,
	CONSTANT_BUFFER  = 1 << 3,
	INDIRECT_PARAM   = 1 << 4,
};
inline Usage operator | (Usage a, Usage b) { return Usage(int(a) | int(b)); }

enum TextureType
{
	TEX_1D,
	TEX_1D_ARRAY,
	TEX_2D,
	TEX_2D_ARRAY,
	TEX_CUBE,
	TEX_CUBE_ARRAY,
	TEX_3D,
};

enum EComparisonFunc : uint8
{
	NEVER,
	LESS,
	EQUAL,
	LESS_EQUAL,
	GREATER,
	NOT_EQUAL,
	GREATER_EQUAL,
	ALWAYS,
};

enum ECullMode : uint8
{
	CULL_NONE,
	CULL_FRONT,
	CULL_BACK,
};

enum TextureFilter
{
	FILTER_POINT,
	FILTER_LINEAR,
	FILTER_POINT_MIP,
	FILTER_BILINEAR,
	FILTER_TRILINEAR,
};

enum AddressMode
{
	AM_WRAP,
	AM_CLAMP,
};

enum BlendFactor
{
	BF_ZERO,
	BF_ONE,
	BF_SRC_COLOR,
	BF_INV_SRC_COLOR,
	BF_SRC_ALPHA,
	BF_INV_SRC_ALPHA,
	BF_DST_COLOR,
	BF_INV_DST_COLOR,
	BF_DST_ALPHA,
	BF_INV_DST_ALPHA,
};

enum BlendMode
{
	BM_ADD,
	BM_SUB,
	BM_REV_SUB,
	BM_MIN,
	BM_MAX,
};

enum PrimitiveType
{
	PRIM_POINTS,
	PRIM_LINES,
	PRIM_LINESTRIP,
	PRIM_TRIANGLES,
	PRIM_TRIANGLESTRIP,
};

enum AttribFormat
{
	VF_FLOAT,
	VF_FLOAT2,
	VF_FLOAT3,
	VF_FLOAT4,

	VF_HALF2,
	VF_HALF4,

	VF_UINT,
	VF_UINT2,
	VF_UINT3,
	VF_UINT4,
};

struct AttribDesc
{
	uint Stream;
	AttribFormat Format;
	const char *Name;
};

enum EResourceState
{
	RS_COMMON            = 0x0,
	RS_PRESENT           = 0x0,
	RS_VERTEX_BUFFER     = 0x1,
	RS_INDEX_BUFFER      = 0x2,
	RS_RENDER_TARGET     = 0x4,
	RS_UNORDERED_ACCESS  = 0x8,
	RS_INDIRECT_ARGUMENT = 0x200,
};

enum Feature : uint8
{
	Disable,
	Enable,
	Require,
};

struct DeviceParams
{
	DeviceParams()
	{
		memset(this, 0, sizeof(*this));
	}

	int m_Width;
	int m_Height;
	int m_MSAA;
	bool m_Fullscreen;
	bool m_VSync;
	bool m_ModeChangeInProgress;

	int m_FullscreenWidth;
	int m_FullscreenHeight;
	int m_WindowedWidth;
	int m_WindowedHeight;

	HWND m_Window;
	const wchar_t* m_Title;

	Feature m_MeshShaders;
	Feature m_8bitStorage;
};

struct DisplayMode
{
	int m_Width;
	int m_Height;
};

enum AspectRatio
{
	ASPECT_RATIO_4_3,
	ASPECT_RATIO_16_10,
	ASPECT_RATIO_16_9,
	ASPECT_RATIO_5_4,
	ASPECT_RATIO_5_3,
	ASPECT_RATIO_3_2,
	ASPECT_RATIO_OTHER,
};

AspectRatio GetAspectRatio(const int width, const int height);
const char *GetAspectRatioString(const AspectRatio aspect_ratio);

extern const uint g_AttribSizes[];

struct TexData
{
	void* Data;
	uint  Pitch;
	uint  SlicePitch;
};




Device CreateDevice(DeviceParams& params);
void DestroyDevice(Device& device);

void Present(Device device, bool vsync);

HWND GetWindow(Device device);

Texture GetBackBuffer(Device device, uint buffer);
ImageFormat GetBackBufferFormat(Device device);
RenderPass GetBackBufferRenderPass(Device device);
RenderSetup GetBackBufferSetup(Device device, uint buffer);

Context GetMainContext(Device device);
uint GetBackBufferIndex(Device device);

uint GetDisplayModeCount(Device device);
const DisplayMode& GetDisplayMode(Device device, uint index);
void SetDisplayMode(Device device, DeviceParams& params);
void UpdateWindow(Device device, DeviceParams& params, int width, int height);
bool Resize(Device device, DeviceParams& params, int width, int height);
bool SupportsMSAAMode(Device device, uint samples);

BlendState GetDefaultBlendState(Device device);

Context CreateContext(Device device, bool deferred);
void DestroyContext(Device device, Context& context);
Device GetDevice(Context context);


Texture CreateTexture(Device device, const char* filename, uint mipmap_count = 0);
Texture CreateTexture(Device device, const char** filenames, uint slices, uint mipmap_count = 0);
Texture CreateTexture(Device device, const Image& image, const char* filename = nullptr);

struct STextureParams
{
	TextureType m_Type     = TEX_2D;
	ImageFormat m_Format   = IMGFMT_INVALID;
	uint m_Width           = 1;
	uint m_Height          = 1;
	uint m_Depth           = 1;
	uint m_Slices          = 1;
	uint m_MipCount        = 1;
	uint m_MSAASampleCount = 1;

	bool m_ShaderResource  = false;
	bool m_RenderTarget    = false;
	bool m_DepthTarget     = false;
	bool m_UnorderedAccess = false;

	const float* m_ClearValue = nullptr;

	const char* m_Name     = nullptr;
};
Texture CreateTexture(Device device, const STextureParams& params);
void DestroyTexture(Device device, Texture& texture);

enum ResourceType
{
	RESTYPE_TEXTURE,
	RESTYPE_BUFFER,
};

struct SResourceDesc
{
	SResourceDesc() = default;

	SResourceDesc(const Texture texture) : m_Resource(texture), m_Type(RESTYPE_TEXTURE)
	{
	}
	SResourceDesc(const Buffer buffer) : m_Resource(buffer), m_Type(RESTYPE_BUFFER)
	{
	}

	const void* m_Resource;
	ResourceType m_Type;
};
no_inline ResourceTable CreateResourceTable(Device device, RootSignature root, uint32 slot, const SResourceDesc* resources, uint count);
template<int N> force_inline ResourceTable CreateResourceTable(Device device, RootSignature root, uint32 slot, const SResourceDesc(&resources)[N]) { return CreateResourceTable(device, root, slot, resources, N); }
void DestroyResourceTable(Device device, ResourceTable& table);

struct SSamplerDesc
{
	TextureFilter Filter;
	uint          Aniso;
	AddressMode   AddressModeU;
	AddressMode   AddressModeV;
	AddressMode   AddressModeW;
};
no_inline SamplerTable CreateSamplerTable(Device device, RootSignature root, uint32 slot, const SSamplerDesc* sampler_descs, uint count);
template<int N> force_inline SamplerTable CreateSamplerTable(Device device, RootSignature root, uint32 slot, const SSamplerDesc(&samplers)[N]) { return CreateSamplerTable(device, root, slot, samplers, N); }
void DestroySamplerTable(Device device, SamplerTable& table);

enum RenderPassFlags
{
	CLEAR_COLOR   = 0x1,
	CLEAR_DEPTH   = 0x2,
	FINAL_PRESENT = 0x4,
};
inline RenderPassFlags operator | (RenderPassFlags a, RenderPassFlags b) { return RenderPassFlags(int(a) | int(b)); }

RenderPass CreateRenderPass(Device device, ImageFormat color_format, ImageFormat depth_format, RenderPassFlags flags, uint msaa_samples = 1);
void DestroyRenderPass(Device device, RenderPass& render_pass);

RenderSetup CreateRenderSetup(Device device, RenderPass render_pass, Texture* color_targets, uint color_target_count, Texture depth_target = nullptr, Texture resolve_target = nullptr);
void DestroyRenderSetup(Device device, RenderSetup& setup);

struct SCodeBlob
{
	SCodeBlob() = default;

	SCodeBlob(const void* code, size_t size)
	{
		m_Code = code;
		m_Size = size;
	}

	// Template to let you assign a SCodeBlob straight from a static string
	template<int N>
	SCodeBlob(const char(&arr)[N]) : SCodeBlob(&arr, sizeof(arr) - 1)
	{
	}

	const void*  m_Code;
	size_t m_Size;
};

RootSignature CreateRootSignature(Device device, const SCodeBlob& blob);
void DestroyRootSignature(Device device, RootSignature& root);

struct SPipelineParams
{
	RenderPass m_RenderPass = nullptr;

	RootSignature m_RootSignature = nullptr;
	SCodeBlob m_TS = { nullptr, 0 };
	SCodeBlob m_MS = { nullptr, 0 };
	SCodeBlob m_VS = { nullptr, 0 };
	SCodeBlob m_PS = { nullptr, 0 };
	uint* m_SpecializationConstants = nullptr;
	uint m_NumSpecializationConstants = 0;

	PrimitiveType m_PrimitiveType = PRIM_TRIANGLES;

	const AttribDesc* m_Attribs = nullptr;
	uint32 m_AttribCount        = 0;

	BlendState m_BlendState = nullptr;

	ECullMode m_CullMode = CULL_NONE;

	// DepthStencil
	bool m_DepthTest  = false;
	bool m_DepthWrite = false;
	EComparisonFunc m_DepthFunc = GREATER;

	const char* m_Name = nullptr;
};
struct SComputePipelineParams
{
	RootSignature m_RootSignature = nullptr;
	SCodeBlob m_CS = { nullptr, 0 };
	uint* m_SpecializationConstants = nullptr;
	uint m_NumSpecializationConstants = 0;

	const char* m_Name = nullptr;
};

Pipeline CreatePipeline(Device device, const SPipelineParams& params);
Pipeline CreatePipeline(Device device, const SComputePipelineParams& params);
void DestroyPipeline(Device device, Pipeline& pipeline);




BlendState CreateBlendState(Device device, BlendFactor src, BlendFactor dst, BlendMode mode = BM_ADD, uint32 mask = 0xF, bool alpha_to_coverage = false);
void DestroyBlendState(Device device, BlendState& state);

VertexSetup CreateVertexSetup(Device device, Buffer vertex_buffer, uint vb_stride, Buffer index_buffer = nullptr, uint ib_stride = 0);
void DestroyVertexSetup(Device device, VertexSetup& state);

struct SBufferParams
{
	SBufferParams(uint size, HeapType heap_type, Usage usage, const char* name)
		: m_Size(size)
		, m_HeapType(heap_type)
		, m_Usage(usage)
		, m_Name(name)
	{
	}
	SBufferParams(uint element_size, uint element_count, HeapType heap_type, Usage usage, const char* name)
		: m_Size(element_size * element_count)
		, m_HeapType(heap_type)
		, m_Usage(usage)
		, m_Name(name)
	{
	}

	uint        m_Size;
	HeapType    m_HeapType;
	Usage		m_Usage;
	const char* m_Name;
};
Buffer CreateBuffer(Device device, const SBufferParams& params);
void DestroyBuffer(Device device, Buffer& buffer);
uint GetBufferSize(Buffer buffer);

void BeginMarker(Context context, const char* name);
void EndMarker(Context context);
struct ScopeMarkerHelper
{
	explicit ScopeMarkerHelper(Context context, const char* name) : m_Context(context)
	{
		BeginMarker(context, name);
	}
	~ScopeMarkerHelper()
	{
		EndMarker(m_Context);
	}
private:
	Context m_Context;
};
#define STR_CAT0(a, b) a##b
#define STR_CAT1(a, b) STR_CAT0(a, b)
#define ScopeMarker(context, name) ScopeMarkerHelper STR_CAT1(scope_marker, __COUNTER__)(context, name)

void BeginContext(Context context, uint upload_buffer_size, const char* name, bool profile);
void EndContext(Context context);
void SubmitContexts(Device device, uint count, Context* context);
void Finish(Device device);

struct SProfileData
{
	uint64 m_TimeStamp;
	const char* m_Name;
};
uint GetProfileData(Device device, SProfileData (&OutData)[MAX_QUERY_COUNT]);
float GetTimestampFrequency(Device device);

void CopyBuffer(Context context, const Buffer dest, const Buffer src);



// Data management
struct SMapBufferParams
{
	SMapBufferParams(Device device, const Buffer buffer)
		: m_Device(device)
		, m_Context(nullptr)
		, m_Buffer(buffer)
		, m_Offset(0)
		, m_Size(GetBufferSize(buffer))
	{
	}
	SMapBufferParams(Context context, const Buffer buffer)
		: m_Device(GetDevice(context))
		, m_Context(context)
		, m_Buffer(buffer)
		, m_Offset(0)
		, m_Size(GetBufferSize(buffer))
	{
	}
	SMapBufferParams(Device device, const Buffer buffer, uint offset, uint size)
		: m_Device(device)
		, m_Context(nullptr)
		, m_Buffer(buffer)
		, m_Offset(offset)
		, m_Size(size)
	{
	}
	SMapBufferParams(Context context, const Buffer buffer, uint offset, uint size)
		: m_Device(GetDevice(context))
		, m_Context(context)
		, m_Buffer(buffer)
		, m_Offset(offset)
		, m_Size(size)
	{
	}
private:
	Device m_Device;
	Context m_Context;
	const Buffer m_Buffer;
	uint m_Offset;
	uint m_Size;

	mutable uint m_InternalOffset;

	friend uint8* MapBuffer(const SMapBufferParams& params);
	friend void UnmapBuffer(const SMapBufferParams& params);
};
uint8* MapBuffer(const SMapBufferParams& params);
template<typename T> force_inline T* MapBuffer(const SMapBufferParams& params) { return (T*) MapBuffer(params); }
void UnmapBuffer(const SMapBufferParams& params);

void SetBufferData(Context context, Buffer buffer, const void* data, uint size);
void SetTextureData(Context context, Texture texture, uint mip, uint slice, const void* data, uint size, uint pitch);


// Rendering
void BeginRenderPass(Context context, const char* name, const RenderPass render_pass, const RenderSetup setup, const float* clear_color = nullptr);
void EndRenderPass(Context context, const RenderSetup setup);

void TransitionRenderSetup(Context context, const RenderSetup setup, EResourceState state_before, EResourceState state_after);

void SetRootSignature(Context context, const RootSignature root);
void SetPipeline(Context context, const Pipeline pipeline);

void SetVertexSetup(Context context, const VertexSetup setup);

uint8* SetVertexBuffer(Context context, uint stream, uint stride, uint count);

uint8* SetGraphicsConstantBuffer(Context context, uint slot, uint size);
uint8* SetComputeConstantBuffer(Context context, uint slot, uint size);

void SetGraphicsConstantBuffer(Context context, uint slot, const Buffer buffer);
void SetComputeConstantBuffer(Context context, uint slot, const Buffer buffer);

void SetRootConstants(Context context, uint slot, const void* data, uint count);

void SetRootTextureBuffer(Context context, uint slot, const Buffer buffer);

void SetGraphicsResourceTable(Context context, uint slot, const ResourceTable table);
void SetComputeResourceTable(Context context, uint slot, const ResourceTable table);
void SetGraphicsSamplerTable(Context context, uint slot, const SamplerTable table);
void SetComputeSamplerTable(Context context, uint slot, const SamplerTable table);

void Draw(Context context, uint start, uint count);
void DrawIndexed(Context context, uint start, uint count);
void DrawIndexedInstanced(Context context, uint start, uint count, uint start_instance, uint instance_count);
void DrawIndexedIndirect(Context context, Buffer buffer, uint offset);

void DrawMeshTask(Context context, uint start, uint count);

void Dispatch(Context context, uint group_x, uint group_y, uint group_z);
void DispatchIndirect(Context context, Buffer buffer, uint offset);

void ClearBuffer(Context context, Buffer buffer, uint32 clear_value);


struct SBarrierDesc
{
	SBarrierDesc() = default;

	SBarrierDesc(const Texture texture, EResourceState before, EResourceState after) : m_Resource(texture), m_Type(RESTYPE_TEXTURE), m_Before(before), m_After(after)
	{
	}
	SBarrierDesc(const Buffer buffer, EResourceState before, EResourceState after) : m_Resource(buffer), m_Type(RESTYPE_BUFFER), m_Before(before), m_After(after)
	{
	}

	const void* m_Resource;
	ResourceType m_Type;
	EResourceState m_Before;
	EResourceState m_After;
};

void UAVBarrier(Context context, Buffer buffer = nullptr);
void Barrier(Context context, const SBarrierDesc* barriers, uint count);
template<int N> force_inline void Barrier(Context context, const SBarrierDesc(&barriers)[N]) { Barrier(context, barriers, N); }
