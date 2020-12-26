
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

#include "../Renderer.h"
#include "../../Util/Array.h"
#include "../../Util/IDAllocator.h"

#include <dxgi1_4.h>
#include <d3d12.h>
#include <d3dcompiler.h>

//#define USE_PIX

#ifdef USE_PIX
#include "pix3.h"
#endif

#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3d12.lib")
#pragma comment (lib, "d3dcompiler.lib")

#include <algorithm>

#define SAFE_RELEASE(b) if (b){ (b)->Release(); b = nullptr; }

// Pick the AMD or Nvidia discrete GPU on laptops with an integrated GPU
extern "C"
{
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
	__declspec(dllexport) DWORD NvOptimusEnablement = 1;
}

struct SDescriptorHeap
{
	ID3D12DescriptorHeap* m_DescHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_CPUDescStart;
	D3D12_GPU_DESCRIPTOR_HANDLE m_GPUDescStart;
	uint m_DescSize;

	IDAllocator m_Allocator;

	void Init(ID3D12Device* d3d_device, D3D12_DESCRIPTOR_HEAP_TYPE heap_type, uint size, bool shader_visible)
	{
		m_Allocator.Init(size - 1);

		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = heap_type;
		desc.NumDescriptors = size;
		desc.Flags = shader_visible? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		d3d_device->CreateDescriptorHeap(&desc, __uuidof(ID3D12DescriptorHeap), (void**) &m_DescHeap);

		m_CPUDescStart = m_DescHeap->GetCPUDescriptorHandleForHeapStart();
		m_GPUDescStart = m_DescHeap->GetGPUDescriptorHandleForHeapStart();
		m_DescSize = d3d_device->GetDescriptorHandleIncrementSize(desc.Type);
	}

	void Destroy(uint size)
	{
		m_DescHeap->Release();

		ASSERT(m_Allocator.GetAvailableIDs() == size);
		m_Allocator.Destroy();
	}

	uint Allocate(uint count)
	{
		uint index;
		bool res = m_Allocator.CreateRangeID(index, count);
		ASSERT(res);

		return index;
	}

	void Release(uint offset, uint count)
	{
		bool res = m_Allocator.DestroyRangeID(offset, count);
		ASSERT(res);
	}

	inline D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint offset) const
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_CPUDescStart;
		handle.ptr += offset * m_DescSize;
		return handle;
	}

	inline D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint offset) const
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handle = m_GPUDescStart;
		handle.ptr += offset * m_DescSize;
		return handle;
	}

};

struct SCommandListAllocator
{
	ID3D12Resource*           m_UploadBuffer;
	D3D12_GPU_VIRTUAL_ADDRESS m_Address;
	uint8*                    m_Data;
	uint                      m_Offset;
	uint                      m_Size;

	ID3D12QueryHeap* m_QueryHeap;
	ID3D12Resource*  m_QueryBuffer;
	uint             m_QueryOffset;
	const char*      m_QueryNames[MAX_QUERY_COUNT];


	ID3D12CommandAllocator* m_CommandAllocator;

	static ID3D12Resource* CreateBuffer(ID3D12Device* d3d_device, D3D12_HEAP_TYPE heap_type, size_t size)
	{
		D3D12_HEAP_PROPERTIES heap_prop = {};
		heap_prop.Type = heap_type;

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Width            = size;
		desc.Height           = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels        = 1;
		desc.SampleDesc.Count = 1;
		desc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		D3D12_RESOURCE_STATES resource_state = (heap_type == D3D12_HEAP_TYPE_READBACK)? D3D12_RESOURCE_STATE_COPY_DEST : D3D12_RESOURCE_STATE_GENERIC_READ;

		ID3D12Resource* buffer = nullptr;
		HRESULT hr = d3d_device->CreateCommittedResource(&heap_prop, D3D12_HEAP_FLAG_NONE, &desc, resource_state, nullptr, __uuidof(ID3D12Resource), (void**) &buffer);
		ASSERT(SUCCEEDED(hr));

		return buffer;
	}


	void Init(ID3D12Device* d3d_device)
	{
		HRESULT hr = d3d_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**) &m_CommandAllocator);
		ASSERT(SUCCEEDED(hr));

		D3D12_QUERY_HEAP_DESC desc = {};
		desc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
		desc.Count = MAX_QUERY_COUNT;
		hr = d3d_device->CreateQueryHeap(&desc, __uuidof(ID3D12QueryHeap), (void**) &m_QueryHeap);
		ASSERT(SUCCEEDED(hr));

		m_QueryBuffer = CreateBuffer(d3d_device, D3D12_HEAP_TYPE_READBACK, MAX_QUERY_COUNT * sizeof(uint64));
	}

	void Destroy()
	{
		SAFE_RELEASE(m_QueryBuffer);
		SAFE_RELEASE(m_QueryHeap);
		SAFE_RELEASE(m_CommandAllocator);
		SAFE_RELEASE(m_UploadBuffer);
	}

	void Begin(ID3D12Device* d3d_device, ID3D12GraphicsCommandList* cmdlist, uint upload_buffer_size)
	{
		m_CommandAllocator->Reset();
		cmdlist->Reset(m_CommandAllocator, nullptr);

		if (upload_buffer_size != m_Size)
		{
			m_Size = upload_buffer_size;

			if (m_UploadBuffer)
				m_UploadBuffer->Release();
			m_UploadBuffer = CreateBuffer(d3d_device, D3D12_HEAP_TYPE_UPLOAD, upload_buffer_size);
		}

		m_Offset = 0;
		m_Data = nullptr;

		m_QueryOffset = 0;
	}

	void End()
	{
		if (m_Data)
		{
			D3D12_RANGE range = { 0, m_Offset };
			m_UploadBuffer->Unmap(0, &range);
		}
	}

	uint32 AllocateUploadBuffer(uint size, uint alignment)
	{
		uint aligned_offset = (m_Offset + alignment - 1) & ~(alignment - 1);

		if (aligned_offset + size > m_Size)
		{
			// TODO: Reallocate
			ASSERT(false);
		}

		if (m_Data == nullptr)
		{
			m_Address = m_UploadBuffer->GetGPUVirtualAddress();

			D3D12_RANGE range = { 0, 0 };
			HRESULT hr = m_UploadBuffer->Map(0, &range, (void**) &m_Data);
			ASSERT(SUCCEEDED(hr));
		}

		m_Offset = aligned_offset + size;

		return aligned_offset;
	}
};

struct SDevice
{
	ID3D12Device* m_Device;
	ID3D12CommandQueue* m_CommandQueue;
	IDXGISwapChain3* m_SwapChain;
	D3D_FEATURE_LEVEL m_FeatureLevel;

	HWND m_Window;

	Context m_MainContext;

	RenderPass m_BackBufferRenderPass;
	Texture m_BackBuffer[BUFFER_FRAMES];
	RenderSetup m_BackBufferSetup[BUFFER_FRAMES];
	uint64 m_FenceValues[BUFFER_FRAMES];
	uint64 m_FenceCounter;
	uint32 m_FrameIndex;
	ID3D12Fence* m_Fence;
	HANDLE m_FenceEvent;


	SDescriptorHeap m_SRVHeap;
	SDescriptorHeap m_RTVHeap;
	SDescriptorHeap m_DSVHeap;
	SDescriptorHeap m_SamplerHeap;

	SCommandListAllocator m_CommandListAllocators[BUFFER_FRAMES];

	float m_TimestampFrequency;
	uint m_MSAASupportMask;

	// Default states
	BlendState m_DefaultBlendState;

	StaticArray<DisplayMode> m_DisplayModes;
};

struct SContext
{
	ID3D12GraphicsCommandList* m_CommandList;
	SCommandListAllocator* m_Allocator;

	Device m_Device;

	bool m_IsProfiling;
};


static const DXGI_FORMAT g_Formats[] =
{
	DXGI_FORMAT_UNKNOWN,

	DXGI_FORMAT_R8_UNORM,
	DXGI_FORMAT_R8_UINT,

	DXGI_FORMAT_R8G8_UNORM,
	DXGI_FORMAT_R8G8_UINT,

	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_R8G8B8A8_UINT,

	DXGI_FORMAT_R16_UNORM,
	DXGI_FORMAT_R16_UINT,
	DXGI_FORMAT_R16_FLOAT,

	DXGI_FORMAT_R16G16_UNORM,
	DXGI_FORMAT_R16G16_UINT,
	DXGI_FORMAT_R16G16_FLOAT,

	DXGI_FORMAT_R16G16B16A16_UNORM,
	DXGI_FORMAT_R16G16B16A16_UINT,
	DXGI_FORMAT_R16G16B16A16_FLOAT,

	DXGI_FORMAT_R32_UINT,
	DXGI_FORMAT_R32_FLOAT,

	DXGI_FORMAT_R32G32_UINT,
	DXGI_FORMAT_R32G32_FLOAT,

	DXGI_FORMAT_R32G32B32A32_UINT,
	DXGI_FORMAT_R32G32B32A32_FLOAT,

	DXGI_FORMAT_BC1_UNORM,
	DXGI_FORMAT_BC2_UNORM,
	DXGI_FORMAT_BC3_UNORM,
	DXGI_FORMAT_BC4_UNORM,
	DXGI_FORMAT_BC5_UNORM,
	DXGI_FORMAT_BC7_UNORM,

	DXGI_FORMAT_D16_UNORM,
};
static_assert(elementsof(g_Formats) == IMGFMT_D16 + 1, "g_Formats incorrect length");

static const DXGI_FORMAT g_AttribFormats[] =
{
	DXGI_FORMAT_R32_FLOAT,
	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32B32A32_FLOAT,

	DXGI_FORMAT_R16G16_FLOAT,
	DXGI_FORMAT_R16G16B16A16_FLOAT,

	DXGI_FORMAT_R32_UINT,
	DXGI_FORMAT_R32G32_UINT,
	DXGI_FORMAT_R32G32B32_UINT,
	DXGI_FORMAT_R32G32B32A32_UINT,
};


struct STexture
{
	ID3D12Resource* m_Texture;

	uint m_Width;
	uint m_Height;
	uint m_Depth;
	uint m_Slices;
	uint m_MipLevels;
	TextureType m_Type;
	ImageFormat m_Format;
};

struct SResourceTable
{
	uint m_Offset;
	uint m_Size;
};

struct SSamplerTable
{
	uint m_Offset;
	uint m_Count;
};

struct SRenderPass
{
	DXGI_FORMAT m_DepthFormat;
	uint m_ColorTargetCount;
	DXGI_FORMAT m_ColorFormats[1];
	RenderPassFlags m_Flags;
	uint m_MSAASamples;
};

struct SRenderSetup
{
	uint    m_RTV;
	uint    m_DSV;
	Texture m_DepthTexture;
	Texture m_ResolveTexture;
	uint    m_ColorBufferCount;
	Texture m_ColorTextures[1];
};

struct SRootSignature
{
	ID3D12RootSignature* m_Root;
};

struct SPipeline
{
	ID3D12PipelineState* m_Pipeline;
	D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveTopology;
};

struct SBlendState
{
	D3D12_BLEND_DESC m_Desc;
};

struct SVertexSetup
{
	D3D12_INDEX_BUFFER_VIEW  m_IBView;
	D3D12_VERTEX_BUFFER_VIEW m_VBView;
};

struct SBuffer
{
	ID3D12Resource* m_Buffer;
	uint            m_Size;
	HeapType        m_HeapType;
};



static bool CreateBackBufferSetups(Device device, uint width, uint height)
{
	for (uint i = 0; i < BUFFER_FRAMES; i++)
	{
		ID3D12Resource* back_buffer = nullptr;
		if (FAILED(device->m_SwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&back_buffer)))
		{
			ErrorMsg("Couldn't create back-buffer");
			return false;
		}
		back_buffer->SetName(L"BackBuffer");

		STexture* texture = new STexture();
		memset(texture, 0, sizeof(STexture));
		texture->m_Texture = back_buffer;
		texture->m_Width     = width;
		texture->m_Height    = height;
		texture->m_Depth     = 1;
		texture->m_Slices    = 1;
		texture->m_MipLevels = 1;
		texture->m_Type      = TEX_2D;
		texture->m_Format    = IMGFMT_RGBA8;

		device->m_BackBuffer[i] = texture;

		device->m_BackBufferSetup[i] = CreateRenderSetup(device, device->m_BackBufferRenderPass, &texture, 1, nullptr);
	}

	return true;
}

static void DestroyBackBufferSetups(Device device)
{
	for (uint i = 0; i < BUFFER_FRAMES; i++)
	{
		DestroyRenderSetup(device, device->m_BackBufferSetup[i]);
		DestroyTexture(device, device->m_BackBuffer[i]);
	}
}


// For sorting display modes
bool operator < (const DXGI_MODE_DESC &a, const DXGI_MODE_DESC &b)
{
	int ratio_diff = GetAspectRatio(a.Width, a.Height) - GetAspectRatio(b.Width, b.Height);

	if (ratio_diff < 0)
		return true;
	if (ratio_diff > 0)
		return false;

	return a.Width < b.Width;
}

static void SetD3DName(ID3D12Object* object, const char* name)
{
#ifdef DEBUG
	if (name)
	{
		// Convert to wchar_t
		size_t len = strlen(name);

		// Workaround for Windows 1903 bug with short strings
		size_t new_len = max(len, 4ULL);

		wchar_t* str = (wchar_t*) alloca((new_len + 1) * sizeof(wchar_t));
		size_t i = 0;
		for (; i < len; ++i)
			str[i] = name[i];
		for (; i < new_len; ++i)
			str[i] = ' ';
		str[i] = 0;
		object->SetName(str);
	}
#endif
}

Device CreateDevice(DeviceParams& params)
{
	HINSTANCE inst = GetModuleHandle(nullptr);

	DWORD style = WS_OVERLAPPEDWINDOW;
	int x = 0;
	int y = 0;
	int w = params.m_FullscreenWidth;
	int h = params.m_FullscreenHeight;

	if (!params.m_Fullscreen)
	{
		RECT rect;
		rect.left = 0;
		rect.top  = 0;
		rect.right  = params.m_WindowedWidth;
		rect.bottom = params.m_WindowedHeight;
		AdjustWindowRectEx(&rect, style, FALSE, 0);

		x = rect.left;
		y = rect.top;
		w = rect.right - rect.left;
		h = rect.bottom - rect.top;

		// Center window on screen
		RECT workarea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &workarea, 0);
		x += (workarea.left + workarea.right  - w) / 2;
		y += (workarea.top  + workarea.bottom - h) / 2;
	}

	wchar_t title[128];
	wsprintf(title, L"%s (%ux%u)", params.m_Title, params.m_Width, params.m_Height);


	HWND hwnd = CreateWindowEx(0, L"Humus", title, style, x, y, w, h, HWND_DESKTOP, nullptr, inst, nullptr);


	UINT flags = 0;
#ifdef DEBUG
	flags |= DXGI_CREATE_FACTORY_DEBUG;

	ID3D12Debug* d3d_debug = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**) &d3d_debug)))
	{
		d3d_debug->EnableDebugLayer();
		d3d_debug->Release();
	}
#endif

	IDXGIFactory2* dxgi_factory = nullptr;
	if (FAILED(CreateDXGIFactory2(flags, __uuidof(IDXGIFactory2), (void **) &dxgi_factory)))
	{
		ErrorMsg("Couldn't create DXGIFactory");
		return false;
	}

	IDXGIAdapter1* dxgi_adapter = nullptr;
	int last_device_score = -1;

	{
		// TODO: Investigate if EnumAdapterByGpuPreference() is a better choice
		uint i = 0;
		IDXGIAdapter1* adapter;
		while (dxgi_factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			// Primarily prioritize Flags in NONE, REMOTE, SOFTWARE order. Secondly, prioritize AMD or NVIDIA over other vendors (sorry Intel)
			int device_score = ((~desc.Flags & 0x3) << 1) | ((desc.VendorId == 0x1002 || desc.VendorId == 0x10de)? 1 : 0);

			if (device_score > last_device_score)
			{
				if (dxgi_adapter)
					dxgi_adapter->Release();
				dxgi_adapter = adapter;
				last_device_score = device_score;
			}
			else
			{
				adapter->Release();
			}

			++i;
		}
	}

	if (!dxgi_adapter)
	{
		ErrorMsg("No adapters found");
		return nullptr;
	}


/*
	LARGE_INTEGER version;
	dxgi_adapter->CheckInterfaceSupport(__uuidof(ID3D10Device), &version);

	char driver[256];
	sprintf(driver, "%u.%u.%u.%u", version.HighPart >> 16, version.HighPart & 0xFFFF, version.LowPart >> 16, version.LowPart & 0xFFFF);
*/

	ASSERT(dxgi_factory->IsCurrent());

	// Create device
	D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;

	ID3D12Device* d3d_device = nullptr;
	if (FAILED(D3D12CreateDevice(dxgi_adapter, feature_level, __uuidof(ID3D12Device), (void**) &d3d_device)))
	{
		ErrorMsg("Device creation failed");
		return nullptr;
	}

	D3D12_COMMAND_QUEUE_DESC queue_desc = {};
	queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ID3D12CommandQueue* d3d_queue = nullptr;
	d3d_device->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), (void**) &d3d_queue);
	SetD3DName(d3d_queue, "Graphics");
	

	DXGI_SWAP_CHAIN_DESC1 sc_desc = {};
	sc_desc.Width  = params.m_Width;
	sc_desc.Height = params.m_Height;
	sc_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sc_desc.SampleDesc.Count = 1;
	sc_desc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;// | DXGI_USAGE_SHADER_INPUT;
	sc_desc.BufferCount = 2;
	sc_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sc_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fs_desc = {};
	fs_desc.Windowed = !params.m_Fullscreen;


	// Create swap-chain
	IDXGISwapChain1* swap_chain1 = nullptr;
	if (FAILED(dxgi_factory->CreateSwapChainForHwnd(d3d_queue, hwnd, &sc_desc, &fs_desc, nullptr, &swap_chain1)))
	{
		ErrorMsg("Couldn't create swapchain");
		return nullptr;
	}

	IDXGISwapChain3* swap_chain = nullptr;
	HRESULT hr = swap_chain1->QueryInterface(__uuidof(IDXGISwapChain3), (void**) &swap_chain);
	swap_chain1->Release();
	if (FAILED(hr))
	{
		ErrorMsg("Couldn't create swapchain");
		return nullptr;
	}

	IDXGIOutput* dxgi_output = nullptr;
	hr = swap_chain->GetContainingOutput(&dxgi_output);



	// TODO: Allocate later ...
	Device device = new SDevice;
	memset(device, 0, sizeof(SDevice));
	device->m_Window = hwnd;
	device->m_MSAASupportMask = 1;

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS levels = {};
	levels.Format = sc_desc.Format;
	for (levels.SampleCount = 2; levels.SampleCount <= 8; levels.SampleCount *= 2)
	{
		if (SUCCEEDED(d3d_device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &levels, sizeof(levels))))
		{
			if (levels.NumQualityLevels > 0)
				device->m_MSAASupportMask |= levels.SampleCount;
		}
	}


	uint display_mode_count = 0;
	if (dxgi_output)
	{
		if (SUCCEEDED(dxgi_output->GetDisplayModeList(sc_desc.Format, 0, &display_mode_count, nullptr)))
		{
			DXGI_MODE_DESC* display_modes = new DXGI_MODE_DESC[display_mode_count];

			if (SUCCEEDED(dxgi_output->GetDisplayModeList(sc_desc.Format, 0, &display_mode_count, display_modes)))
			{
				// Sort display modes
				std::sort(display_modes, display_modes + display_mode_count);

				// Remove duplicates (we only care about resolution)
				uint curr = 0;
				for (uint m = 1; m < display_mode_count; m++)
				{
					if (display_modes[m].Width  != display_modes[curr].Width ||
						display_modes[m].Height != display_modes[curr].Height)
					{
						++curr;
						display_modes[curr] = display_modes[m];
					}
				}

				uint count = curr + 1;

				// Store displaymodes
				device->m_DisplayModes.SetCapacity(count);
				for (uint m = 0; m < count; m++)
				{
					device->m_DisplayModes[m].m_Width  = display_modes[m].Width;
					device->m_DisplayModes[m].m_Height = display_modes[m].Height;
				}
			}

			delete[] display_modes;
		}

		dxgi_output->Release();
	}

	// We'll handle Alt-Enter ourselves thank you very much ...
	dxgi_factory->MakeWindowAssociation(device->m_Window, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);

	dxgi_adapter->Release();
	dxgi_factory->Release();







	device->m_Device = d3d_device;
	device->m_CommandQueue = d3d_queue;
	device->m_SwapChain = swap_chain;
	device->m_FeatureLevel = feature_level;

	for (uint i = 0; i < BUFFER_FRAMES; i++)
	{
		device->m_CommandListAllocators[i].Init(d3d_device);
	}

	device->m_SRVHeap.Init(d3d_device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 64, true);
	device->m_RTVHeap.Init(d3d_device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV,         16, false);
	device->m_DSVHeap.Init(d3d_device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV,         16, false);
	device->m_SamplerHeap.Init(d3d_device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 16, true);

	device->m_MainContext = CreateContext(device, false);

	device->m_BackBufferRenderPass = CreateRenderPass(device, IMGFMT_RGBA8, IMGFMT_INVALID, FINAL_PRESENT);

	if (!CreateBackBufferSetups(device, params.m_Width, params.m_Height))
		return false;


	device->m_DefaultBlendState = CreateBlendState(device, BF_ONE, BF_ZERO, BM_ADD, 0xF, false);

	d3d_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**) &device->m_Fence);
	device->m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	uint64 freq = 0;
	d3d_queue->GetTimestampFrequency(&freq);
	device->m_TimestampFrequency = (float) (1000.0 / (double) freq);

	return device;
}

void DestroyDevice(Device& device)
{
	if (!device)
		return;

	// Wait here?
	device->m_Fence->Release();
	CloseHandle(device->m_FenceEvent);

	DestroyBlendState(device, device->m_DefaultBlendState);

	DestroyBackBufferSetups(device);

	DestroyRenderPass(device, device->m_BackBufferRenderPass);

	DestroyContext(device, device->m_MainContext);

	for (uint i = 0; i < BUFFER_FRAMES; i++)
		device->m_CommandListAllocators[i].Destroy();

	// Destroy device and swapchain
	if (device->m_SwapChain)
	{
		device->m_SwapChain->SetFullscreenState(FALSE, nullptr);

		device->m_SwapChain->Release();
		device->m_SwapChain = nullptr;
	}

	device->m_CommandQueue->Release();

	// Destroy heaps
	device->m_SamplerHeap.Destroy(16);
	device->m_DSVHeap.Destroy(16);
	device->m_RTVHeap.Destroy(16);
	device->m_SRVHeap.Destroy(64);

	if (device->m_Device)
	{
		ULONG ref_count = device->m_Device->Release();

#ifdef DEBUG
		if (ref_count)
		{
			DebugString("There are %d unreleased references left on the D3D device!", ref_count);

			ID3D12DebugDevice* debug = nullptr;
			if (SUCCEEDED(device->m_Device->QueryInterface<ID3D12DebugDevice>(&debug)))
			{
				debug->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
				debug->Release();
			}

		}
#else
		(void) ref_count; // avoid warning
#endif
		device->m_Device = nullptr;
	}

	delete device;
	device = nullptr;
}

void Present(Device device, bool vsync)
{
	device->m_SwapChain->Present(vsync? 1 : 0, 0);


	++device->m_FenceCounter;
	device->m_CommandQueue->Signal(device->m_Fence, device->m_FenceCounter);
	// Save the fence value for this frame.
	device->m_FenceValues[device->m_FrameIndex] = device->m_FenceCounter;

	device->m_FrameIndex = (device->m_FrameIndex + 1) % BUFFER_FRAMES;

	// If the previous frame has not finished yet, wait until it's done.
	if (device->m_Fence->GetCompletedValue() < device->m_FenceValues[device->m_FrameIndex])
	{
		device->m_Fence->SetEventOnCompletion(device->m_FenceValues[device->m_FrameIndex], device->m_FenceEvent);
		WaitForSingleObjectEx(device->m_FenceEvent, INFINITE, FALSE);
	}

/*
	const UINT64 currentFenceValue = device->m_FenceValues[device->m_FrameIndex];
	device->m_CommandQueue->Signal(device->m_Fence, currentFenceValue);

	device->m_FrameIndex = (device->m_FrameIndex + 1) % BUFFER_FRAMES;

	// If the next frame is not ready to be rendered yet, wait until it is ready.
	if (device->m_Fence->GetCompletedValue() < device->m_FenceValues[device->m_FrameIndex])
	{
		device->m_Fence->SetEventOnCompletion(device->m_FenceValues[device->m_FrameIndex], device->m_FenceEvent);
		WaitForSingleObjectEx(device->m_FenceEvent, INFINITE, FALSE);
	}

	// Set the fence value for the next frame.
	device->m_FenceValues[device->m_FrameIndex] = currentFenceValue + 1;
*/

}

HWND GetWindow(Device device)
{
	return device->m_Window;
}

Texture GetBackBuffer(Device device, uint buffer)
{
	return device->m_BackBuffer[buffer];
}

ImageFormat GetBackBufferFormat(Device device)
{
	return IMGFMT_RGBA8;
}

RenderPass GetBackBufferRenderPass(Device device)
{
	return device->m_BackBufferRenderPass;
}

RenderSetup GetBackBufferSetup(Device device, uint buffer)
{
	return device->m_BackBufferSetup[buffer];
}

Context GetMainContext(Device device)
{
	return device->m_MainContext;
}

uint GetBackBufferIndex(Device device)
{
	return device->m_SwapChain->GetCurrentBackBufferIndex();
}

uint GetDisplayModeCount(Device device)
{
	return device->m_DisplayModes.GetCount();
}

const DisplayMode& GetDisplayMode(Device device, uint index)
{
	return device->m_DisplayModes[index];
}

void SetDisplayMode(Device device, DeviceParams& params)
{
	params.m_ModeChangeInProgress = true;

	DestroyBackBufferSetups(device);

	DXGI_MODE_DESC desc;
	desc.RefreshRate.Numerator   = 0;
	desc.RefreshRate.Denominator = 0;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if (params.m_Fullscreen)
	{
		DebugString("SetDisplayMode(%d, %d, 1);", params.m_FullscreenWidth, params.m_FullscreenHeight);

		desc.Width  = params.m_FullscreenWidth;
		desc.Height = params.m_FullscreenHeight;

		device->m_SwapChain->ResizeTarget(&desc);
		device->m_SwapChain->SetFullscreenState(TRUE, nullptr);
	}
	else
	{
		DebugString("SetDisplayMode(%d, %d, 0);", params.m_WindowedWidth, params.m_WindowedHeight);

		//DXGI_MODE_DESC desc;
		desc.Width  = params.m_WindowedWidth;
		desc.Height = params.m_WindowedHeight;

		/*
		RECT rect;
		rect.left = 0;
		rect.top  = 0;
		rect.right  = params.m_WindowedWidth;
		rect.bottom = params.m_WindowedHeight;
		AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0);

		int x = rect.left;
		int y = rect.top;
		int w = rect.right - rect.left;
		int h = rect.bottom - rect.top;

		RECT workarea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &workarea, 0);

		x += (workarea.left + workarea.right  - w) / 2;
		y += (workarea.top  + workarea.bottom - h) / 2;

		//sprintf(str, "MoveWindow(%d, %d);\n", w, h);
		//OutputDebugStringA(str);

		MoveWindow(device->m_Window, x, y, w, h, TRUE);
		*/

		device->m_SwapChain->SetFullscreenState(FALSE, nullptr);
		device->m_SwapChain->ResizeTarget(&desc);
	}

	Resize(device, params, desc.Width, desc.Height);

	CreateBackBufferSetups(device, desc.Width, desc.Height);

	params.m_ModeChangeInProgress = false;
}

// This function is called in response to WM_SIZE messages, including synchronously during mode switch
bool Resize(Device device, DeviceParams& params, int width, int height)
{
	if (!params.m_ModeChangeInProgress)
		DestroyBackBufferSetups(device);

	DebugString("Resize(%d, %d);", width, height);

	device->m_SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);

	UpdateWindow(device, params, width, height);

	if (!params.m_ModeChangeInProgress)
		CreateBackBufferSetups(device, width, height);

	return true;
}

bool SupportsMSAAMode(Device device, uint samples)
{
	return (device->m_MSAASupportMask & samples) != 0;
}


BlendState GetDefaultBlendState(Device device)
{
	return device->m_DefaultBlendState;
}


Context CreateContext(Device device, bool deferred)
{
	ID3D12GraphicsCommandList* d3d_cmdlist = nullptr;
	HRESULT hr = device->m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, device->m_CommandListAllocators[0].m_CommandAllocator, nullptr, __uuidof(ID3D12GraphicsCommandList*), (void**) &d3d_cmdlist);
	if (FAILED(hr))
		return nullptr;

	// D3D dumbfuckery
	d3d_cmdlist->Close();


	// Allocate on aligned address
	Context context = (SContext*) _mm_malloc(sizeof(SContext), 16);
	memset(context, 0, sizeof(SContext));

	context->m_Device = device;
	context->m_CommandList = d3d_cmdlist;

	return context;
}

void DestroyContext(Device device, Context& context)
{
	if (!context)
		return;

	if (context->m_CommandList)
		context->m_CommandList->Release();

	_mm_free(context);

	context = nullptr;
}

Device GetDevice(Context context)
{
	return context->m_Device;
}

Texture CreateTexture(Device device, const STextureParams& params)
{
	D3D12_HEAP_PROPERTIES heap_prop = {};
	heap_prop.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE;

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension        = (params.m_Type <= TEX_1D_ARRAY)? D3D12_RESOURCE_DIMENSION_TEXTURE1D : (params.m_Type <= TEX_CUBE_ARRAY)? D3D12_RESOURCE_DIMENSION_TEXTURE2D : D3D12_RESOURCE_DIMENSION_TEXTURE3D;
	desc.Width            = params.m_Width;
	desc.Height           = params.m_Height;
	desc.DepthOrArraySize = (uint16) ((params.m_Type == TEX_3D)? params.m_Depth : params.m_Slices);
	desc.MipLevels        = (uint16) params.m_MipCount;
	desc.Format           = g_Formats[params.m_Format];
	desc.SampleDesc.Count = params.m_MSAASampleCount;
	desc.Layout           = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	if (params.m_RenderTarget)
		desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	if (params.m_DepthTarget)
	{
		desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		if (!params.m_ShaderResource)
			desc.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
	}
	if (params.m_UnorderedAccess)
		desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;


	D3D12_RESOURCE_STATES resource_state = D3D12_RESOURCE_STATE_COPY_DEST;
	D3D12_CLEAR_VALUE clear_value = {};

	if (params.m_DepthTarget)
	{
		resource_state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		clear_value.Format = desc.Format;
		if (params.m_ClearValue)
			clear_value.DepthStencil.Depth = *params.m_ClearValue;
	}
	else if (params.m_RenderTarget)
	{
		resource_state = D3D12_RESOURCE_STATE_RENDER_TARGET;
		if (params.m_ClearValue)
		{
			clear_value.Format = desc.Format;
			memcpy(clear_value.Color, params.m_ClearValue, sizeof(clear_value.Color));
		}
	}



	ID3D12Resource* d3d_texture = nullptr;
	HRESULT hr = device->m_Device->CreateCommittedResource(&heap_prop, flags, &desc, resource_state, clear_value.Format? &clear_value : nullptr, __uuidof(ID3D12Resource), (void**) &d3d_texture);
	if (FAILED(hr))
		return nullptr;

	SetD3DName(d3d_texture, params.m_Name);



	Texture texture = new STexture();
	texture->m_Texture = d3d_texture;
	texture->m_Width     = params.m_Width;
	texture->m_Height    = params.m_Height;
	texture->m_Depth     = params.m_Depth;
	texture->m_Slices    = params.m_Slices;
	texture->m_MipLevels = params.m_MipCount;
	texture->m_Type      = params.m_Type;
	texture->m_Format    = params.m_Format;
	return texture;
}

void DestroyTexture(Device device, Texture& texture)
{
	if (texture)
	{
		texture->m_Texture->Release();

		delete texture;
		texture = nullptr;
	}
}

ResourceTable CreateResourceTable(Device device, RootSignature root, uint32 slot, const SResourceDesc* resources, uint count)
{
	uint offset = device->m_SRVHeap.Allocate(count);

	D3D12_CPU_DESCRIPTOR_HANDLE handle = device->m_SRVHeap.m_CPUDescStart;
	handle.ptr += offset * device->m_SRVHeap.m_DescSize;

	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};

	for (uint i = 0; i < count; i++)
	{
		switch (resources[i].m_Type)
		{
			case RESTYPE_TEXTURE:
			{
				const Texture texture = (Texture) resources[i].m_Resource;

				desc.Format = g_Formats[texture->m_Format];
				desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				switch (texture->m_Type)
				{
				case TEX_1D:
					desc.ViewDimension                        = D3D12_SRV_DIMENSION_TEXTURE1D;
					desc.Texture1D.MostDetailedMip            = 0;
					desc.Texture1D.MipLevels                  = texture->m_MipLevels;
					desc.Texture1D.ResourceMinLODClamp        = 0;
					break;
				case TEX_1D_ARRAY:
					desc.ViewDimension                        = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
					desc.Texture1DArray.MostDetailedMip       = 0;
					desc.Texture1DArray.MipLevels             = texture->m_MipLevels;
					desc.Texture1DArray.FirstArraySlice       = 0;
					desc.Texture1DArray.ArraySize             = texture->m_Slices;
					desc.Texture1DArray.ResourceMinLODClamp   = 0;
					break;
				case TEX_2D:
					desc.ViewDimension                        = D3D12_SRV_DIMENSION_TEXTURE2D;
					desc.Texture2D.MostDetailedMip            = 0;
					desc.Texture2D.MipLevels                  = texture->m_MipLevels;
					desc.Texture2D.PlaneSlice                 = 0;
					desc.Texture2D.ResourceMinLODClamp        = 0;
					break;
				case TEX_2D_ARRAY:
					desc.ViewDimension                        = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
					desc.Texture2DArray.MostDetailedMip       = 0;
					desc.Texture2DArray.MipLevels             = texture->m_MipLevels;
					desc.Texture2DArray.FirstArraySlice       = 0;
					desc.Texture2DArray.ArraySize             = texture->m_Slices;
					desc.Texture2DArray.PlaneSlice            = 0;
					desc.Texture2DArray.ResourceMinLODClamp   = 0;
					break;
				case TEX_CUBE:
					desc.ViewDimension                        = D3D12_SRV_DIMENSION_TEXTURECUBE;
					desc.TextureCube.MostDetailedMip          = 0;
					desc.TextureCube.MipLevels                = texture->m_MipLevels;
					desc.TextureCube.ResourceMinLODClamp      = 0;
					break;
				case TEX_CUBE_ARRAY:
					desc.ViewDimension                        = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
					desc.TextureCubeArray.MostDetailedMip     = 0;
					desc.TextureCubeArray.MipLevels           = texture->m_MipLevels;
					desc.TextureCubeArray.First2DArrayFace    = 0;
					desc.TextureCubeArray.NumCubes            = texture->m_Slices / 6;
					desc.TextureCubeArray.ResourceMinLODClamp = 0;
					break;
				case TEX_3D:
					desc.ViewDimension                        = D3D12_SRV_DIMENSION_TEXTURE3D;
					desc.Texture3D.MostDetailedMip            = 0;
					desc.Texture3D.MipLevels                  = texture->m_MipLevels;
					desc.Texture3D.ResourceMinLODClamp        = 0;
					break;
				default:
					ASSERT(false);
				}

				device->m_Device->CreateShaderResourceView(texture->m_Texture, &desc, handle);
				break;
			}
			case RESTYPE_BUFFER:
			{
				const Buffer buffer = (Buffer) resources[i].m_Resource;
				device->m_Device->CreateShaderResourceView(buffer->m_Buffer, nullptr, handle);
				break;
			}
			default:
				ASSERT(false);
		}
	
		handle.ptr += device->m_SRVHeap.m_DescSize;
	}

	ASSERT(handle.ptr == device->m_SRVHeap.m_CPUDescStart.ptr + (offset + count) * device->m_SRVHeap.m_DescSize);

	ResourceTable rt = new SResourceTable();
	rt->m_Offset = offset;
	rt->m_Size = count;

	return rt;
}

void DestroyResourceTable(Device device, ResourceTable& table)
{
	if (table)
	{
		device->m_SRVHeap.Release(table->m_Offset, table->m_Size);

		delete table;
		table = nullptr;
	}
}

SamplerTable CreateSamplerTable(Device device, RootSignature root, uint32 slot, const SSamplerDesc* sampler_descs, uint count)
{
	uint offset = device->m_SamplerHeap.Allocate(count);

	D3D12_CPU_DESCRIPTOR_HANDLE handle = device->m_SamplerHeap.m_CPUDescStart;
	handle.ptr += offset * device->m_SamplerHeap.m_DescSize;


	static const D3D12_FILTER filters[] =
	{
		D3D12_FILTER_MIN_MAG_MIP_POINT,
		D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT,
		D3D12_FILTER_MIN_MAG_MIP_POINT,
		D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
	};


	for (uint i = 0; i < count; i++)
	{
		const SSamplerDesc& sampler_desc = sampler_descs[i];

		D3D12_SAMPLER_DESC desc;
		desc.Filter = (sampler_desc.Aniso > 1)? D3D12_FILTER_ANISOTROPIC : filters[sampler_desc.Filter];
		desc.AddressU = (sampler_desc.AddressModeU == AM_WRAP)? D3D12_TEXTURE_ADDRESS_MODE_WRAP : D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		desc.AddressV = (sampler_desc.AddressModeV == AM_WRAP)? D3D12_TEXTURE_ADDRESS_MODE_WRAP : D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		desc.AddressW = (sampler_desc.AddressModeW == AM_WRAP)? D3D12_TEXTURE_ADDRESS_MODE_WRAP : D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = sampler_desc.Aniso;
		desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		desc.BorderColor[0] = 0;
		desc.BorderColor[1] = 0;
		desc.BorderColor[2] = 0;
		desc.BorderColor[3] = 0;
		desc.MinLOD = 0.0f;
		desc.MaxLOD = (sampler_desc.Filter > FILTER_LINEAR)? D3D12_FLOAT32_MAX : 0.0f;

		device->m_Device->CreateSampler(&desc, handle);
		handle.ptr += device->m_SamplerHeap.m_DescSize;
	}

	SamplerTable table = new SSamplerTable();
	table->m_Offset = offset;
	table->m_Count  = count;

	return table;
}

void DestroySamplerTable(Device device, SamplerTable& table)
{
	if (table)
	{
		device->m_SamplerHeap.Release(table->m_Offset, table->m_Count);

		delete table;
		table = nullptr;
	}
}

RenderPass CreateRenderPass(Device device, ImageFormat color_format, ImageFormat depth_format, RenderPassFlags flags, uint msaa_samples)
{
	SRenderPass* render_pass = new SRenderPass();
	render_pass->m_DepthFormat = g_Formats[depth_format];
	render_pass->m_ColorTargetCount = 1;
	render_pass->m_ColorFormats[0] = g_Formats[color_format];
	render_pass->m_MSAASamples = msaa_samples;
	render_pass->m_Flags = flags;

	return render_pass;
}

void DestroyRenderPass(Device device, RenderPass& render_pass)
{
	if (render_pass)
	{
		delete render_pass;
		render_pass = nullptr;
	}
}

RenderSetup CreateRenderSetup(Device device, RenderPass render_pass, Texture* color_targets, uint color_target_count, Texture depth_target, Texture resolve_target)
{
	// Variable size structure
	RenderSetup setup((SRenderSetup *) new ubyte[(sizeof(SRenderSetup) - sizeof(Texture)) + color_target_count * sizeof(Texture)]);

	setup->m_DepthTexture = depth_target;
	setup->m_ResolveTexture = resolve_target;
	setup->m_ColorBufferCount = color_target_count;

	if (color_target_count)
	{
		setup->m_RTV = device->m_RTVHeap.Allocate(color_target_count);
		for (uint i = 0; i < color_target_count; i++)
		{
			setup->m_ColorTextures[i] = color_targets[i];
			device->m_Device->CreateRenderTargetView(color_targets[0]->m_Texture, nullptr, device->m_RTVHeap.GetCPUHandle(setup->m_RTV + i));
		}
	}

	if (depth_target)
	{
		setup->m_DSV = device->m_DSVHeap.Allocate(1);
		device->m_Device->CreateDepthStencilView(depth_target->m_Texture, nullptr, device->m_DSVHeap.GetCPUHandle(setup->m_DSV));
	}

	return setup;
}

void DestroyRenderSetup(Device device, RenderSetup& setup)
{
	if (setup)
	{
		if (setup->m_ColorBufferCount)
			device->m_RTVHeap.Release(setup->m_RTV, setup->m_ColorBufferCount);
		if (setup->m_DepthTexture)
			device->m_DSVHeap.Release(setup->m_DSV, 1);

		delete setup;
		setup = nullptr;
	}
}

RootSignature CreateRootSignature(Device device, const SCodeBlob& blob)
{
	ID3D12RootSignature* root = nullptr;
	HRESULT hr = device->m_Device->CreateRootSignature(0, blob.m_Code, blob.m_Size, __uuidof(ID3D12RootSignature), (void **) &root);
	if (FAILED(hr))
		return nullptr;

	RootSignature root_sig = new SRootSignature();
	root_sig->m_Root = root;

	return root_sig;
}

void DestroyRootSignature(Device device, RootSignature& root)
{
	if (root)
	{
		root->m_Root->Release();

		delete root;
		root = nullptr;
	}
}

Pipeline CreatePipeline(Device device, const SPipelineParams& params)
{
	if (params.m_RootSignature == nullptr)
		return nullptr;

	D3D12_INPUT_ELEMENT_DESC ia_desc[16];
	ASSERT(params.m_AttribCount < elementsof(ia_desc));

	uint offsets[4] = {};

	for (uint i = 0; i < params.m_AttribCount; i++)
	{
		uint stream = params.m_Attribs[i].Stream;
		AttribFormat format = params.m_Attribs[i].Format;
		ASSERT(stream < elementsof(offsets));

		ia_desc[i].SemanticName = params.m_Attribs[i].Name;
		ia_desc[i].SemanticIndex = 0;
		ia_desc[i].Format = g_AttribFormats[format];
		ia_desc[i].InputSlot = stream;
		ia_desc[i].AlignedByteOffset = offsets[stream];
		ia_desc[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		ia_desc[i].InstanceDataStepRate = 0;

		offsets[stream] += g_AttribSizes[format];
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	desc.pRootSignature = params.m_RootSignature->m_Root;
	desc.VS.pShaderBytecode = params.m_VS.m_Code;
	desc.VS.BytecodeLength  = params.m_VS.m_Size;
	desc.PS.pShaderBytecode = params.m_PS.m_Code;
	desc.PS.BytecodeLength  = params.m_PS.m_Size;

	desc.PrimitiveTopologyType = (D3D12_PRIMITIVE_TOPOLOGY_TYPE) ((params.m_PrimitiveType + 3) >> 1);

	desc.InputLayout.NumElements = params.m_AttribCount;
	desc.InputLayout.pInputElementDescs = params.m_AttribCount? ia_desc : nullptr;

	desc.BlendState.RenderTarget[0].RenderTargetWriteMask = 0xF;
	if (params.m_BlendState)
	{
		desc.BlendState = params.m_BlendState->m_Desc;
	}

	desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	desc.RasterizerState.CullMode = (D3D12_CULL_MODE)(params.m_CullMode + 1);
	desc.RasterizerState.DepthClipEnable = TRUE;
	desc.RasterizerState.MultisampleEnable = TRUE;

	desc.DepthStencilState.DepthEnable    = params.m_DepthTest;
	desc.DepthStencilState.DepthWriteMask = params.m_DepthWrite? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
	desc.DepthStencilState.DepthFunc      = (D3D12_COMPARISON_FUNC) (params.m_DepthFunc + 1);

	const uint32 rt_count = params.m_RenderPass->m_ColorTargetCount;

	desc.NumRenderTargets = rt_count;
	memcpy(desc.RTVFormats, params.m_RenderPass->m_ColorFormats, rt_count * sizeof(DXGI_FORMAT));
	desc.DSVFormat = params.m_RenderPass->m_DepthFormat;

	desc.SampleDesc.Count = params.m_RenderPass->m_MSAASamples;

	desc.SampleMask = 0xFFFFFFFF;

	ID3D12PipelineState* pso = nullptr;
	HRESULT hr = device->m_Device->CreateGraphicsPipelineState(&desc, __uuidof(ID3D12PipelineState), (void**) &pso);
	if (FAILED(hr))
		return nullptr;

	SetD3DName(pso, params.m_Name);

	Pipeline pipeline = new SPipeline();
	pipeline->m_Pipeline = pso;
	pipeline->m_PrimitiveTopology = (D3D12_PRIMITIVE_TOPOLOGY) (params.m_PrimitiveType + 1);

	return pipeline;
}

void DestroyPipeline(Device device, Pipeline& pipeline)
{
	if (pipeline)
	{
		pipeline->m_Pipeline->Release();

		delete pipeline;
		pipeline = nullptr;
	}
}

BlendState CreateBlendState(Device device, BlendFactor src, BlendFactor dst, BlendMode mode, uint32 mask, bool alpha_to_coverage)
{
	BOOL blend_enable = (src != BF_ONE || dst != BF_ZERO);

	BlendState state = new SBlendState();
	D3D12_BLEND_DESC &desc = state->m_Desc;

	desc.AlphaToCoverageEnable = (BOOL) alpha_to_coverage;
	desc.IndependentBlendEnable = FALSE;

	static const D3D12_BLEND blend_factors[] =
	{
		D3D12_BLEND_ZERO,
		D3D12_BLEND_ONE,
		D3D12_BLEND_SRC_COLOR,
		D3D12_BLEND_INV_SRC_COLOR,
		D3D12_BLEND_SRC_ALPHA,
		D3D12_BLEND_INV_SRC_ALPHA,
		D3D12_BLEND_DEST_COLOR,
		D3D12_BLEND_INV_DEST_COLOR,
		D3D12_BLEND_DEST_ALPHA,
		D3D12_BLEND_INV_DEST_ALPHA,
	};

	static const D3D12_BLEND_OP blend_modes[] =
	{
		D3D12_BLEND_OP_ADD,
		D3D12_BLEND_OP_SUBTRACT,
		D3D12_BLEND_OP_REV_SUBTRACT,
		D3D12_BLEND_OP_MIN,
		D3D12_BLEND_OP_MAX,
	};


	for (int i = 0; i < 8; i++)
	{
		desc.RenderTarget[i].BlendEnable = blend_enable;

		desc.RenderTarget[i].BlendOp        = blend_modes[mode];
		desc.RenderTarget[i].BlendOpAlpha   = blend_modes[mode];
		desc.RenderTarget[i].SrcBlend       = blend_factors[src];
		desc.RenderTarget[i].SrcBlendAlpha  = blend_factors[src];
		desc.RenderTarget[i].DestBlend      = blend_factors[dst];
		desc.RenderTarget[i].DestBlendAlpha = blend_factors[dst];

		desc.RenderTarget[i].RenderTargetWriteMask = (UINT8) mask;
	}

	return state;
}

void DestroyBlendState(Device device, BlendState& state)
{
	if (state)
	{
		delete state;
		state = nullptr;
	}
}

VertexSetup CreateVertexSetup(Device device, Buffer vertex_buffer, uint vb_stride, Buffer index_buffer, uint ib_stride)
{
	VertexSetup setup = new SVertexSetup();
	memset(setup, 0, sizeof(SVertexSetup));

	if (index_buffer)
	{
		ASSERT(ib_stride == sizeof(uint16) || ib_stride == sizeof(uint32));

		setup->m_IBView.BufferLocation = index_buffer->m_Buffer->GetGPUVirtualAddress();
		setup->m_IBView.SizeInBytes    = index_buffer->m_Size;
		setup->m_IBView.Format         = ib_stride == sizeof(uint16)? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	}

	if (vertex_buffer)
	{
		ASSERT(vb_stride > 0);

		setup->m_VBView.BufferLocation = vertex_buffer->m_Buffer->GetGPUVirtualAddress();
		setup->m_VBView.SizeInBytes    = vertex_buffer->m_Size;
		setup->m_VBView.StrideInBytes  = vb_stride;
	}


	return setup;
}

void DestroyVertexSetup(Device device, VertexSetup& state)
{
	if (state)
	{
		delete state;
		state = nullptr;
	}
}

Buffer CreateBuffer(Device device, const SBufferParams& params)
{
	D3D12_HEAP_PROPERTIES heap_prop = {};
	heap_prop.Type = (D3D12_HEAP_TYPE) (params.m_HeapType + 1);

	D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE;

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width            = params.m_Size;
	desc.Height           = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels        = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	D3D12_RESOURCE_STATES resource_state = D3D12_RESOURCE_STATE_GENERIC_READ;

	ID3D12Resource* d3d_buffer = nullptr;
	HRESULT hr = device->m_Device->CreateCommittedResource(&heap_prop, flags, &desc, resource_state, nullptr, __uuidof(ID3D12Resource), (void**) &d3d_buffer);
	if (FAILED(hr))
		return nullptr;

	SetD3DName(d3d_buffer, params.m_Name);

	Buffer buffer = new SBuffer();
	buffer->m_Buffer = d3d_buffer;
	buffer->m_Size = params.m_Size;
	buffer->m_HeapType = params.m_HeapType;
	return buffer;
}

void DestroyBuffer(Device device, Buffer& buffer)
{
	if (buffer)
	{
		buffer->m_Buffer->Release();

		delete buffer;
		buffer = nullptr;
	}
}

uint GetBufferSize(Buffer buffer)
{
	return buffer->m_Size;
}

void BeginMarker(Context context, const char* name)
{
#ifdef USE_PIX
	PIXBeginEvent(context->m_CommandList, 0, name);
#endif

	if (context->m_IsProfiling)
	{
		SCommandListAllocator* allocator = context->m_Allocator;
		ASSERT(allocator->m_QueryOffset < MAX_QUERY_COUNT);

		allocator->m_QueryNames[allocator->m_QueryOffset] = name;
		context->m_CommandList->EndQuery(allocator->m_QueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, allocator->m_QueryOffset++);
	}
}

void EndMarker(Context context)
{
	if (context->m_IsProfiling)
	{
		SCommandListAllocator* allocator = context->m_Allocator;
		ASSERT(allocator->m_QueryOffset < MAX_QUERY_COUNT);

		allocator->m_QueryNames[allocator->m_QueryOffset] = nullptr;
		context->m_CommandList->EndQuery(allocator->m_QueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, allocator->m_QueryOffset++);
	}

#ifdef USE_PIX
	PIXEndEvent(context->m_CommandList);
#endif
}

void BeginContext(Context context, uint upload_buffer_size, const char* name, bool profile)
{
	context->m_IsProfiling = profile;

	context->m_Allocator = &context->m_Device->m_CommandListAllocators[context->m_Device->m_FrameIndex];

	context->m_Allocator->Begin(context->m_Device->m_Device, context->m_CommandList, upload_buffer_size);

	ID3D12DescriptorHeap* heaps[] = { context->m_Device->m_SRVHeap.m_DescHeap, context->m_Device->m_SamplerHeap.m_DescHeap };
	context->m_CommandList->SetDescriptorHeaps(2, heaps);

	BeginMarker(context, name);
}

void EndContext(Context context)
{
	EndMarker(context);

	SCommandListAllocator* allocator = context->m_Allocator;

	if (allocator->m_QueryOffset)
	{
		context->m_CommandList->ResolveQueryData(allocator->m_QueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, 0, allocator->m_QueryOffset, allocator->m_QueryBuffer, 0);
	}

	allocator->End();
	context->m_CommandList->Close();

	context->m_Allocator = nullptr;
}

void SubmitContexts(Device device, uint count, SContext** context)
{
	ID3D12CommandList** cmdlists = StackAlloc<ID3D12CommandList*>(count);

	for (uint i = 0; i < count; i++)
	{
		cmdlists[i] = context[i]->m_CommandList;
	}

	device->m_CommandQueue->ExecuteCommandLists(count, cmdlists);
}

void Finish(Device device)
{
	++device->m_FenceCounter;
	device->m_CommandQueue->Signal(device->m_Fence, device->m_FenceCounter);

	// If the previous frame has not finished yet, wait until it's done.
	if (device->m_Fence->GetCompletedValue() < device->m_FenceCounter)
	{
		device->m_Fence->SetEventOnCompletion(device->m_FenceCounter, device->m_FenceEvent);
		WaitForSingleObjectEx(device->m_FenceEvent, INFINITE, FALSE);
	}
}

uint GetProfileData(Device device, SProfileData (&OutData)[MAX_QUERY_COUNT])
{
	const SCommandListAllocator& allocator = device->m_CommandListAllocators[device->m_FrameIndex];

	const uint count = allocator.m_QueryOffset;
	if (count)
	{
		uint64* data = nullptr;
		D3D12_RANGE range = { 0, count * sizeof(uint64) };
		allocator.m_QueryBuffer->Map(0, &range, (void**) &data);
		for (uint i = 0; i < count; i++)
		{
			OutData[i].m_TimeStamp = data[i];
			OutData[i].m_Name = allocator.m_QueryNames[i];
		}

		allocator.m_QueryBuffer->Unmap(0, nullptr);
	}

	return count;
}

float GetTimestampFrequency(Device device)
{
	return device->m_TimestampFrequency;
}

void CopyBuffer(Context context, const Buffer dest, const Buffer src)
{
	context->m_CommandList->CopyResource(dest->m_Buffer, src->m_Buffer);
}

uint8* MapBuffer(const SMapBufferParams& params)
{
	uint8* data = nullptr;

	if (params.m_Buffer->m_HeapType == HEAP_DEFAULT)
	{
		SCommandListAllocator* allocator = params.m_Context->m_Allocator;

		uint offset = allocator->AllocateUploadBuffer(params.m_Size, 16);
		data = allocator->m_Data + offset;

		params.m_InternalOffset = offset;
	}
	else
	{
		D3D12_RANGE range = { 0, 0 };
		HRESULT hr = params.m_Buffer->m_Buffer->Map(0, &range, (void**) &data);
		ASSERT(SUCCEEDED(hr));

		data += params.m_Offset;
	}

	return data;
}

void UnmapBuffer(const SMapBufferParams& params)
{
	if (params.m_Buffer->m_HeapType == HEAP_DEFAULT)
	{
		Context context = params.m_Context;

		D3D12_RESOURCE_BARRIER desc;
		desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		desc.Transition.pResource   = params.m_Buffer->m_Buffer;
		desc.Transition.Subresource = 0;

		desc.Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
		desc.Transition.StateAfter  = D3D12_RESOURCE_STATE_COPY_DEST;
		context->m_CommandList->ResourceBarrier(1, &desc);

		context->m_CommandList->CopyBufferRegion(params.m_Buffer->m_Buffer, params.m_Offset, context->m_Allocator->m_UploadBuffer, params.m_InternalOffset, params.m_Size);

		desc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		desc.Transition.StateAfter  = D3D12_RESOURCE_STATE_GENERIC_READ;
		context->m_CommandList->ResourceBarrier(1, &desc);
	}
	else
	{
		D3D12_RANGE range = { params.m_Offset, params.m_Offset + params.m_Size };
		params.m_Buffer->m_Buffer->Unmap(0, &range);
	}
}


static uint GetFormatSize(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_R8_UNORM:
		return 1;
	case DXGI_FORMAT_R8G8_UNORM:
		return 2;
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return 4;
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC4_UNORM:
		return 8;
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC7_UNORM:
		return 16;
	default:
		ASSERT(0);
	}
	return 0;
}

static bool IsCompressedFormat(DXGI_FORMAT format)
{
	return (format >= DXGI_FORMAT_BC1_TYPELESS && format <= DXGI_FORMAT_BC5_SNORM) || (format >= DXGI_FORMAT_BC6H_TYPELESS && format <= DXGI_FORMAT_BC7_UNORM_SRGB);
}

void SetTextureData(Context context, Texture texture, uint mip, uint slice, const void* data, uint size, uint pitch)
{
	D3D12_RESOURCE_DESC desc = texture->m_Texture->GetDesc();

	D3D12_TEXTURE_COPY_LOCATION dst;
	dst.pResource = texture->m_Texture;
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

	D3D12_TEXTURE_COPY_LOCATION src;
	src.pResource = context->m_Allocator->m_UploadBuffer;
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint.Footprint.Format = desc.Format;


	const uint format_size = GetFormatSize(desc.Format);
	const bool is_compressed = IsCompressedFormat(desc.Format);

	uint w = max(texture->m_Width  >> mip, 1U);
	uint h = max(texture->m_Height >> mip, 1U);
	uint d = max(texture->m_Depth  >> mip, 1U);;

	uint cols = w;
	uint rows = h;
	if (is_compressed)
	{
		cols = (cols + 3) >> 2;
		rows = (rows + 3) >> 2;
	}
	rows *= d;

	ASSERT(pitch * rows == size);

	uint src_pitch = pitch;
	uint dst_pitch = Align(pitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

	// Grab upload buffer space
	uint dst_offset = context->m_Allocator->AllocateUploadBuffer(dst_pitch * rows, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);

	// Copy data in place
	uint8* src_data = (uint8*) data;
	uint8* dst_data = context->m_Allocator->m_Data + dst_offset;
	for (uint32 r = 0; r < rows; ++r)
	{
		memcpy(dst_data, src_data, src_pitch);
		dst_data += dst_pitch;
		src_data += src_pitch;
	}

	// Issue a copy from upload buffer to texture
	dst.SubresourceIndex = mip + slice * texture->m_MipLevels;

	src.PlacedFootprint.Offset = dst_offset;
	if (is_compressed)
	{
		src.PlacedFootprint.Footprint.Width  = Align(w, 4);
		src.PlacedFootprint.Footprint.Height = Align(h, 4);
	}
	else
	{
		src.PlacedFootprint.Footprint.Width  = w;
		src.PlacedFootprint.Footprint.Height = h;
	}
	src.PlacedFootprint.Footprint.Depth  = d;
	src.PlacedFootprint.Footprint.RowPitch = dst_pitch;

	context->m_CommandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);


	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource   = texture->m_Texture;
	barrier.Transition.Subresource = dst.SubresourceIndex;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	context->m_CommandList->ResourceBarrier(1, &barrier);
}

void BeginRenderPass(Context context, const char* name, const RenderPass render_pass, const RenderSetup setup, const float* clear_color)
{
	BeginMarker(context, name);

	const uint32 count = setup->m_ColorBufferCount;
	D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle, dsv_handle;
	if (count)
	{
		rtv_handle = context->m_Device->m_RTVHeap.GetCPUHandle(setup->m_RTV);

		if (render_pass->m_Flags & CLEAR_COLOR)
		{
			for (uint i = 0; i < setup->m_ColorBufferCount; i++)
			{
				D3D12_CPU_DESCRIPTOR_HANDLE handle = context->m_Device->m_RTVHeap.GetCPUHandle(setup->m_RTV + i);
				context->m_CommandList->ClearRenderTargetView(handle, clear_color, 0, nullptr);
			}
		}
	}
	if (setup->m_DepthTexture)
	{
		dsv_handle = context->m_Device->m_DSVHeap.GetCPUHandle(setup->m_DSV);

		if (render_pass->m_Flags & CLEAR_DEPTH)
		{
			const float depth = 0.0f;

			context->m_CommandList->ClearDepthStencilView(dsv_handle, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
		}
	}

	context->m_CommandList->OMSetRenderTargets(count, count? &rtv_handle : nullptr, true, setup->m_DepthTexture? &dsv_handle : nullptr);

	D3D12_VIEWPORT vp;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width    = (float) setup->m_ColorTextures[0]->m_Width;
	vp.Height   = (float) setup->m_ColorTextures[0]->m_Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	context->m_CommandList->RSSetViewports(1, &vp);

	D3D12_RECT r;
	r.left   = 0;
	r.top    = 0;
	r.right  = setup->m_ColorTextures[0]->m_Width;
	r.bottom = setup->m_ColorTextures[0]->m_Height;
	context->m_CommandList->RSSetScissorRects(1, &r);
}

void EndRenderPass(Context context, const RenderSetup setup)
{
	if (setup->m_ResolveTexture)
	{
		D3D12_RESOURCE_BARRIER barriers[2] = {};
		barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barriers[0].Transition.pResource = setup->m_ColorTextures[0]->m_Texture;
		barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barriers[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;

		barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barriers[1].Transition.pResource = setup->m_ResolveTexture->m_Texture;
		barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barriers[1].Transition.StateAfter  = D3D12_RESOURCE_STATE_RESOLVE_DEST;

		context->m_CommandList->ResourceBarrier(2, barriers);

		context->m_CommandList->ResolveSubresource(setup->m_ResolveTexture->m_Texture, 0, setup->m_ColorTextures[0]->m_Texture, 0, g_Formats[setup->m_ResolveTexture->m_Format]);

		barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		barriers[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;

		barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RESOLVE_DEST;
		barriers[1].Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;

		context->m_CommandList->ResourceBarrier(2, barriers);

	}

	EndMarker(context);
}

void TransitionRenderSetup(Context context, const RenderSetup setup, EResourceState state_before, EResourceState state_after)
{
	D3D12_RESOURCE_BARRIER barriers[elementsof(SRenderSetup::m_ColorTextures)/*+1*/];
	for (uint i = 0; i < setup->m_ColorBufferCount; i++)
	{
		barriers[i].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barriers[i].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barriers[i].Transition.pResource = setup->m_ColorTextures[i]->m_Texture;
		barriers[i].Transition.StateBefore = (D3D12_RESOURCE_STATES) state_before;
		barriers[i].Transition.StateAfter  = (D3D12_RESOURCE_STATES) state_after;
		barriers[i].Transition.Subresource = 0;
	}

	context->m_CommandList->ResourceBarrier(setup->m_ColorBufferCount, barriers);
}

void SetRootSignature(Context context, const RootSignature root)
{
	context->m_CommandList->SetGraphicsRootSignature(root->m_Root);
}

void SetPipeline(Context context, const Pipeline pipeline)
{
	context->m_CommandList->SetPipelineState(pipeline->m_Pipeline);
	context->m_CommandList->IASetPrimitiveTopology(pipeline->m_PrimitiveTopology);
}

void SetVertexSetup(Context context, const VertexSetup setup)
{
	if (setup->m_IBView.BufferLocation)
		context->m_CommandList->IASetIndexBuffer(&setup->m_IBView);

	if (setup->m_VBView.BufferLocation)
		context->m_CommandList->IASetVertexBuffers(0, 1, &setup->m_VBView);
}

uint8* SetVertexBuffer(Context context, uint stream, uint stride, uint count)
{
	uint size = stride * count;
	uint offset = context->m_Allocator->AllocateUploadBuffer(size, 4);

	D3D12_VERTEX_BUFFER_VIEW view;
	view.BufferLocation = context->m_Allocator->m_Address + offset;
	view.SizeInBytes = size;
	view.StrideInBytes = stride;
	context->m_CommandList->IASetVertexBuffers(stream, 1, &view);

	return context->m_Allocator->m_Data + offset;
}

uint8* SetGraphicsConstantBuffer(Context context, uint slot, uint size)
{
	uint offset = context->m_Allocator->AllocateUploadBuffer(size, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

	context->m_CommandList->SetGraphicsRootConstantBufferView(slot, context->m_Allocator->m_Address + offset);
	return context->m_Allocator->m_Data + offset;
}

uint8* SetComputeConstantBuffer(Context context, uint slot, uint size)
{
	uint offset = context->m_Allocator->AllocateUploadBuffer(size, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

	context->m_CommandList->SetComputeRootConstantBufferView(slot, context->m_Allocator->m_Address + offset);
	return context->m_Allocator->m_Data + offset;
}

void SetGraphicsConstantBuffer(Context context, uint slot, const Buffer buffer)
{
	D3D12_GPU_VIRTUAL_ADDRESS addr = buffer->m_Buffer->GetGPUVirtualAddress();
	context->m_CommandList->SetGraphicsRootConstantBufferView(slot, addr);
}

void SetComputeConstantBuffer(Context context, uint slot, const Buffer buffer)
{
	D3D12_GPU_VIRTUAL_ADDRESS addr = buffer->m_Buffer->GetGPUVirtualAddress();
	context->m_CommandList->SetComputeRootConstantBufferView(slot, addr);
}

void SetRootConstants(Context context, uint slot, const void* data, uint count)
{
	context->m_CommandList->SetGraphicsRoot32BitConstants(slot, count, data, 0);
}

void SetRootTextureBuffer(Context context, uint slot, const Buffer buffer)
{
	D3D12_GPU_VIRTUAL_ADDRESS addr = buffer->m_Buffer->GetGPUVirtualAddress();
	context->m_CommandList->SetGraphicsRootShaderResourceView(slot, addr);
}

void SetGraphicsResourceTable(Context context, uint slot, const ResourceTable table)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = context->m_Device->m_SRVHeap.GetGPUHandle(table->m_Offset);
	context->m_CommandList->SetGraphicsRootDescriptorTable(slot, handle);
}

void SetComputeResourceTable(Context context, uint slot, const ResourceTable table)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = context->m_Device->m_SRVHeap.GetGPUHandle(table->m_Offset);
	context->m_CommandList->SetComputeRootDescriptorTable(slot, handle);
}

void SetGraphicsSamplerTable(Context context, uint slot, const SamplerTable table)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = context->m_Device->m_SamplerHeap.GetGPUHandle(table->m_Offset);
	context->m_CommandList->SetGraphicsRootDescriptorTable(slot, handle);
}

void SetComputeSamplerTable(Context context, uint slot, const SamplerTable table)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = context->m_Device->m_SamplerHeap.GetGPUHandle(table->m_Offset);
	context->m_CommandList->SetComputeRootDescriptorTable(slot, handle);
}

void Draw(Context context, uint start, uint count)
{
	context->m_CommandList->DrawInstanced(count, 1, start, 0);
}

void DrawIndexed(Context context, uint start, uint count)
{
	context->m_CommandList->DrawIndexedInstanced(count, 1, start, 0, 0);
}

void DrawIndexedInstanced(Context context, uint start, uint count, uint start_instance, uint instance_count)
{
	context->m_CommandList->DrawIndexedInstanced(count, instance_count, start, 0, start_instance);
}

void DrawIndexedIndirect(Context context, Buffer buffer, uint offset)
{
	ASSERT(false);
}

void DrawMeshTask(Context context, uint start, uint count)
{
	ASSERT(false);
}

void Dispatch(Context context, uint group_x, uint group_y, uint group_z)
{
	context->m_CommandList->Dispatch(group_x, group_y, group_z);
}


void DispatchIndirect(Context context, Buffer buffer, uint offset)
{
	ASSERT(false);
}

void ClearBuffer(Context context, Buffer buffer, uint32 clear_value)
{
	ASSERT(false);
}

void UAVBarrier(Context context, Buffer buffer)
{
	D3D12_RESOURCE_BARRIER desc;
	desc.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	desc.UAV.pResource = buffer? buffer->m_Buffer : nullptr;
	context->m_CommandList->ResourceBarrier(1, &desc);
}

void Barrier(Context context, const SBarrierDesc* barriers, uint count)
{
	D3D12_RESOURCE_BARRIER* desc = StackAlloc<D3D12_RESOURCE_BARRIER>(count);

	for (uint i = 0; i < count; i++)
	{
		desc[i].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		desc[i].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		switch (barriers[i].m_Type)
		{
		case RESTYPE_TEXTURE:
			desc[i].Transition.pResource = ((Texture) barriers[i].m_Resource)->m_Texture;
			break;
		case RESTYPE_BUFFER:
			desc[i].Transition.pResource = ((Buffer) barriers[i].m_Resource)->m_Buffer;
			break;
		default:
			ASSERT(false);
		}
		desc[i].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		desc[i].Transition.StateBefore = (D3D12_RESOURCE_STATES) barriers[i].m_Before;
		desc[i].Transition.StateAfter  = (D3D12_RESOURCE_STATES) barriers[i].m_After;
	}
	context->m_CommandList->ResourceBarrier(count, desc);
}
