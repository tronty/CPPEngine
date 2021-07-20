#include "../Util/Tokenizer.h"
#include "../Graphics/Vulkan/VulkanRootSignature.h"
#include <vector>
#include <string>

#include <d3d12.h>
#include <d3dcompiler.h>
#pragma comment (lib, "d3d12.lib")
#pragma comment (lib, "d3dcompiler.lib")

#define ARRAY_COUNT(x) (sizeof(x) / sizeof(x[0]))

static void ErrorMsg(const char* error, const char* tok)
{
	char str[256];
	if (tok)
		sprintf_s(str, "%s%s\n", error, tok);
	else
		sprintf_s(str, "%s\n", error);
	printf(str);
#ifdef _DEBUG
	OutputDebugStringA(str);
#endif
}

enum Error
{
	SUCCESS,
	INVALID_INPUT,
	UNKNOWN_OPTION,
	FILE_NOT_FOUND,
	ACCESS_DENIED,
	SYNTAX_ERROR,
	COMPILE_ERROR,
	INVALID_TYPE,
	UNKNOWN_TYPE,
	UNEXPECTED_TOKEN,
	INVALID_ROOTSIG,
};

enum ShaderLanguage
{
	HLSL,
	GLSL,
};

struct SItem
{
	ItemType m_ItemType;
	int32 m_Count;
	std::string m_Type;
	std::string m_Name;

	std::vector<SItem> m_SubItems;
};

/*
static const char* g_RootConstTypes[] =
{
	// Common types
	"float", "uint", "int",   

	// HLSL types
	"float2", "float3", "float4",
	"uint2",  "uint3",  "uint4",
	"int2",   "int3",   "int4",

	// GLSL types
	"vec2",  "vec3",  "vec4",
	"uvec2", "uvec3", "uvec4",
	"ivec2", "ivec3", "ivec4",
};

struct SKnowntype
{
	const char* Name;
	size_t Length;
};
#define KT(x) { x, sizeof(x)-1 }

static SKnowntype g_SRVTypes[] =
{
	// HLSL types
	KT("Texture1D"),
	KT("Texture2D"),
	KT("Texture3D"),
	KT("TextureCube"),
	KT("Buffer"),
	KT("StructuredBuffer"),

	// GLSL types
	KT("texture1D"),
	KT("texture2D"),
	KT("texture3D"),
	KT("textureCube"),
	KT("image1D"),
	KT("image2D"),
	KT("image3D"),
	KT("imageCube"),

};
static SKnowntype g_UAVTypes[] =
{
	KT("RWTexture1D"),
	KT("RWTexture2D"),
	KT("RWTexture3D"),
	KT("RWTextureCube"),
	KT("RWBuffer"),
	KT("RWStructuredBuffer"),
};
static SKnowntype g_SamplerTypes[] =
{
	// HLSL types
	KT("SamplerState"),
	KT("SamplerComparisonState"),

	// GLSL types
	KT("sampler"),
	KT("samplerShadow"),
};


static bool IsKnownType(const SKnowntype* types, int count, const char* name)
{
	for (int i = 0; i < count; i++)
	{
		if (strncmp(types[i].Name, name, types[i].Length) == 0)
		{
			return true;
		}
	}

	return false;
}

static bool IsType(const char** types, int count, const char* name)
{
	for (int i = 0; i < count; i++)
	{
		if (strcmp(types[i], name) == 0)
			return true;
	}

	return false;
}

static bool IsType(const std::string* types, int count, const char* name)
{
	for (int i = 0; i < count; i++)
	{
		if (strcmp(types[i].c_str(), name) == 0)
			return true;
	}

	return false;
}
*/

static ItemType GetItemType(const char* type)
{
	if (strcmp(type, "CONSTANT") == 0)
		return CONSTANT;
	else if (strcmp(type, "TEXTURE") == 0)
		return TEXTURE;
	else if (strcmp(type, "STRUCTUREDBUFFER") == 0)
		return STRUCTUREDBUFFER;
	else if (strcmp(type, "RWTEXTURE") == 0)
		return RWTEXTURE;
	else if (strcmp(type, "RWSTRUCTUREDBUFFER") == 0)
		return RWSTRUCTUREDBUFFER;
	else if (strcmp(type, "CBUFFER") == 0)
		return CBV;
	else if (strcmp(type, "SAMPLER") == 0)
		return SMP;

	return INVALID;

/*
	if (IsType(g_RootConstTypes, ARRAY_COUNT(g_RootConstTypes), tok))
		return ROOT_CONSTANT;
	else if (IsKnownType(g_SRVTypes, ARRAY_COUNT(g_SRVTypes), tok))
		return SRV;
	else if (IsKnownType(g_UAVTypes, ARRAY_COUNT(g_UAVTypes), tok))
		return UAV;
	else if (IsType(struct_types.data(), (int) struct_types.size(), tok))
		return CBV;
	else
		return -1;
*/
}

static int GetNumDWORDs(const char* type)
{
	int num = atoi(type + strlen(type) - 1);
	return num ? num : 1;
};

static void WriteD3DBlob(FILE* file, const void* blob, size_t size, const char* name)
{
/*
	int count = int(size / sizeof(uint32_t));
	const uint32_t* data = (uint32_t*) blob;

	fprintf(file, "\nstatic const uint32 %s[] =\n{", name);
	for (int i = 0; i < count; i++)
	{
		if ((i & 15) == 0)
			fprintf(file, "\n\t");

		fprintf(file, "0x%08X, ", data[i]);
	}
	fprintf(file, "\n};\n");
*/

	char str[512];
	int offset = 0;

	const int count = int(size);
	const uint8_t* data = (uint8_t*)blob;

	fprintf(file, "\nstatic const char %s[] =\n", name);

	bool last_escaped = false;

	for (int i = 0; i < count; i++)
	{
		const uint8 c = data[i];

		if (c < 32 || c >= 127 || (last_escaped && c >= '0' && c <= '9')) // Technically valid to only escape 0-7, but MSVC warns on 8/9
		{
			str[offset++] = '\\';
			if (c >= 64)
				str[offset++] = '0' + (c >> 6);
			if (c >= 8)
				str[offset++] = '0' + ((c >> 3) & 7);
			str[offset++] = '0' + (c & 7);

			last_escaped = true;
		}
		else
		{
			if (c == '\n')
			{
				str[offset++] = '\\';
				str[offset++] = 'n';
			}
			else if (c == '\r')
			{
				str[offset++] = '\\';
				str[offset++] = 'r';
			}
			else
			{
				if (c == '\\' || c == '"')
					str[offset++] = '\\';
				str[offset++] = c;
			}

			last_escaped = false;
		}

		if (offset >= 128 || i == count - 1)
		{
			str[offset] = '\0';
			fprintf(file, "\t\"%s\"%s\n", str, i == count - 1? ";" : "");
			offset = 0;
		}
	}
}

static bool ReadFile(const char* file_name, void*& data, size_t& size)
{
	data = nullptr;
	size = 0;

	FILE* file = fopen(file_name, "rb");
	if (file == nullptr)
		return false;

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	data = new char[size];

	bool result = (fread(data, 1, size, file) == size);
	if (!result)
	{
		delete[] data;
		data = nullptr;
	}

	fclose(file);

	return result;
}

static bool WriteFile(const char* file_name, const void* data, size_t size)
{
	FILE* file = fopen(file_name, "wb");
	if (file == nullptr)
		return false;

	bool result = (fwrite(data, 1, size, file) == size);
	
	fclose(file);

	return result;
}

static char* WriteShaderHeader(const char* header, size_t header_size, const char* footer, const std::vector<SItem>& items, ShaderLanguage shader_language)
{
	char* buffer = new char[65536]; // TEMP

	char* str = buffer;
	memcpy(str, header, header_size);
	str += header_size;

	if (shader_language == HLSL)
	{
		int srv_reg = 0;
		int uav_reg = 0;
		int cbv_reg = 0;
		int smp_reg = 0;

		for (const SItem& it : items)
		{
			switch (it.m_ItemType)
			{
			case TEXTURE:
			case STRUCTUREDBUFFER:
				str += sprintf(str, "%s %s : register(t%d);\n", it.m_Type.c_str(), it.m_Name.c_str(), srv_reg);
				srv_reg++;
				break;
			case RWTEXTURE:
			case RWSTRUCTUREDBUFFER:
				str += sprintf(str, "%s %s : register(u%d);\n", it.m_Type.c_str(), it.m_Name.c_str(), uav_reg);
				uav_reg++;
				break;
			case CONSTANT:
			case CBV:
				str += sprintf(str, "cbuffer cb%d : register(b%d) { %s %s; }\n", cbv_reg, cbv_reg, it.m_Type.c_str(), it.m_Name.c_str());
				cbv_reg++;
				break;
			case RESOURCE_TABLE:
				for (const SItem& sit : it.m_SubItems)
				{
					switch (sit.m_ItemType)
					{
					case TEXTURE:
					case STRUCTUREDBUFFER:
						if (sit.m_Count)
						{
							str += sprintf(str, "%s %s[%d] : register(t%d);\n", sit.m_Type.c_str(), sit.m_Name.c_str(), sit.m_Count, srv_reg);
							srv_reg += sit.m_Count;
						}
						else
						{
							str += sprintf(str, "%s %s : register(t%d);\n", sit.m_Type.c_str(), sit.m_Name.c_str(), srv_reg);
							srv_reg++;
						}
						break;
					case RWTEXTURE:
					case RWSTRUCTUREDBUFFER:
						if (sit.m_Count)
						{
							str += sprintf(str, "%s %s[%d] : register(u%d);\n", sit.m_Type.c_str(), sit.m_Name.c_str(), sit.m_Count, uav_reg);
							uav_reg += sit.m_Count;
						}
						else
						{
							str += sprintf(str, "%s %s : register(u%d);\n", sit.m_Type.c_str(), sit.m_Name.c_str(), uav_reg);
							uav_reg++;
						}
						break;
					case CBV:
						str += sprintf(str, "cbuffer cb%d : register(b%d) { %s %s; }\n", cbv_reg, cbv_reg, sit.m_Type.c_str(), sit.m_Name.c_str());
						cbv_reg++;
						break;
					}
				}
				break;
			case SAMPLER_TABLE:
				for (const SItem& sit : it.m_SubItems)
				{
					str += sprintf(str, "%s %s : register(s%d);\n", sit.m_Type.c_str(), sit.m_Name.c_str(), smp_reg);
					smp_reg++;
				}
				break;
			}
		}
	}
	else
	{
		int push_buffer_offset = 0;
		for (const SItem& it : items)
		{
			if (it.m_ItemType == CONSTANT)
			{
				if (push_buffer_offset == 0)
					str += sprintf(str, "layout(push_constant) uniform pcb {\n");
				
				str += sprintf(str, "\tlayout(offset = %d) %s %s;\n", push_buffer_offset, it.m_Type.c_str(), it.m_Name.c_str());
				push_buffer_offset += GetNumDWORDs(it.m_Type.c_str()) * sizeof(uint32);
			}
		}

		if (push_buffer_offset)
			str += sprintf(str, "};\n");

		int descriptor_set = 0;
		for (int i = 0; i < items.size(); i++)
		{
			const SItem& it = items[i];

			switch (it.m_ItemType)
			{
			/*case SRV:
				break;
			case UAV:
				break;*/
			case CONSTANT:
				// Push constants already handled above
				break;
			case CBV:
				str += sprintf(str, "layout(set = %d, binding = 0) uniform cb%d { %s %s; };\n", descriptor_set, i, it.m_Type.c_str(), it.m_Name.c_str());
				++descriptor_set;
				break;
			case RESOURCE_TABLE:
				for (int j = 0; j < it.m_SubItems.size(); j++)
				{
					const SItem& sit = it.m_SubItems[j];

					switch (sit.m_ItemType)
					{
					case TEXTURE:
					case RWTEXTURE:
						if (sit.m_Count)
						{
							str += sprintf(str, "layout(set = %d, binding = %d) uniform %s %s[%d];\n", descriptor_set, j, sit.m_Type.c_str(), sit.m_Name.c_str(), sit.m_Count);
						}
						else
						{
							str += sprintf(str, "layout(set = %d, binding = %d) uniform %s %s;\n", descriptor_set, j, sit.m_Type.c_str(), sit.m_Name.c_str());
						}
						break;
					case STRUCTUREDBUFFER:
						if (sit.m_Count)
						{
							str += sprintf(str, "layout(set = %d, binding = %d, std430) readonly buffer sb_%d_%d { %s %s[][%d]; };\n", descriptor_set, j, descriptor_set, j, sit.m_Type.c_str(), sit.m_Name.c_str(), sit.m_Count);
						}
						else
						{
							str += sprintf(str, "layout(set = %d, binding = %d, std430) readonly buffer sb_%d_%d { %s %s[]; };\n", descriptor_set, j, descriptor_set, j, sit.m_Type.c_str(), sit.m_Name.c_str());
						}
						break;
					case RWSTRUCTUREDBUFFER:
						if (sit.m_Count)
						{
							str += sprintf(str, "layout(set = %d, binding = %d, std430) buffer sb_%d_%d { %s %s[][%d]; };\n", descriptor_set, j, descriptor_set, j, sit.m_Type.c_str(), sit.m_Name.c_str(), sit.m_Count);
						}
						else
						{
							str += sprintf(str, "layout(set = %d, binding = %d, std430) buffer sb_%d_%d { %s %s[]; };\n", descriptor_set, j, descriptor_set, j, sit.m_Type.c_str(), sit.m_Name.c_str());
						}
						break;
					/*
					case CBV:
						break;*/
					}
				}
				++descriptor_set;
				break;
			case SAMPLER_TABLE:
				for (int j = 0; j < it.m_SubItems.size(); j++)
				{
					const SItem& sit = it.m_SubItems[j];
					str += sprintf(str, "layout(set = %d, binding = %d) uniform %s %s;\n", descriptor_set, j, sit.m_Type.c_str(), sit.m_Name.c_str());
				}
				++descriptor_set;
				break;
			}
		}
	}

	str += sprintf(str, "%s", footer);



#ifdef _DEBUG
	// Debug dump header
	FILE* file = fopen("Dump.h", "wb");
	fwrite(buffer, str-buffer, 1, file);
	fclose(file);
#endif


	return buffer;
}

static Error WriteCppHeader(FILE* file, const char* header, size_t header_size, const std::vector<SItem>& items, ShaderLanguage shader_language)
{
	fwrite(header, header_size, 1, file);

	// Root slots
	fprintf(file, "\nenum Root\n{\n");
	for (const SItem& it : items)
	{
		fprintf(file, "\t%s,\n", it.m_Name.c_str());
	}
	fprintf(file, "};\n");

	// ResourceTable slots
	for (const SItem& it : items)
	{
		if (it.m_ItemType == RESOURCE_TABLE)
		{
			fprintf(file, "\nenum N%s\n{\n", it.m_Name.c_str());
			for (const SItem& sit : it.m_SubItems)
			{
				fprintf(file, "\t%s,\n", sit.m_Name.c_str());
				if (sit.m_Count)
				{
					fprintf(file, "\t%s0 = %s,\n", sit.m_Name.c_str(), sit.m_Name.c_str());
					for (int32 i = 1; i < sit.m_Count; i++)
					{
						fprintf(file, "\t%s%d,\n", sit.m_Name.c_str(), i);
					}
				}
			}
			fprintf(file, "\n\tCount\n};\n");
		}
	}

	if (shader_language == HLSL)
	{
		// Write root signature blob
		const int MAX_ROOT_PARAMS = 16;
		const int MAX_RANGES = 64;
		D3D12_ROOT_PARAMETER1 root_param[MAX_ROOT_PARAMS] = {};
		D3D12_DESCRIPTOR_RANGE1 ranges[MAX_RANGES] = {};
		D3D12_DESCRIPTOR_RANGE1* range = &ranges[0] - 1;


		D3D12_VERSIONED_ROOT_SIGNATURE_DESC desc = {};
		desc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		desc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		desc.Desc_1_1.NumParameters = (UINT) items.size();
		desc.Desc_1_1.pParameters = root_param;

		int srv_reg = 0;
		int uav_reg = 0;
		int cbv_reg = 0;
		int smp_reg = 0;

		for (int i = 0; i < items.size(); i++)
		{
			const SItem& it = items[i];

			switch (it.m_ItemType)
			{
			case CONSTANT:
				root_param[i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
				root_param[i].Constants.ShaderRegister = cbv_reg;
				root_param[i].Constants.Num32BitValues = GetNumDWORDs(it.m_Type.c_str());
				cbv_reg++;
				break;
			case TEXTURE:
			case STRUCTUREDBUFFER:
				root_param[i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
				root_param[i].Descriptor.ShaderRegister = srv_reg;
				root_param[i].Descriptor.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE;//D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;
				srv_reg++;
				break;
			case RWTEXTURE:
			case RWSTRUCTUREDBUFFER:
				root_param[i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
				root_param[i].Descriptor.ShaderRegister = uav_reg;
				root_param[i].Descriptor.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE;//D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;
				uav_reg++;
				break;
			case CBV:
				root_param[i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
				root_param[i].Descriptor.ShaderRegister = cbv_reg;
				root_param[i].Descriptor.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE;//D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;
				cbv_reg++;
				break;
			case RESOURCE_TABLE:
			case SAMPLER_TABLE:
				root_param[i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				root_param[i].DescriptorTable.NumDescriptorRanges = 0;
				root_param[i].DescriptorTable.pDescriptorRanges = range + 1;

				ItemType last_type = RESOURCE_TABLE;

				for (int s = 0; s < it.m_SubItems.size(); s++)
				{
					const SItem& sit = items[i].m_SubItems[s];

					int count = sit.m_Count? sit.m_Count : 1;

					int reg = -1;
					D3D12_DESCRIPTOR_RANGE_TYPE range_type;
					switch (sit.m_ItemType)
					{
					case TEXTURE:
					case STRUCTUREDBUFFER:
						reg = srv_reg;
						srv_reg += count;
						range_type = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
						break;
					case RWTEXTURE:
					case RWSTRUCTUREDBUFFER:
						reg = uav_reg;
						uav_reg += count;
						range_type = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
						break;
					case CBV:
						reg = cbv_reg;
						cbv_reg++;
						range_type = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
						break;
					case SMP:
						reg = smp_reg;
						smp_reg++;
						range_type = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
						break;
					}

					if (sit.m_ItemType == last_type)
					{
						range->NumDescriptors += count;
					}
					else
					{
						++range;
						range->RangeType = range_type;
						range->NumDescriptors = count;
						range->BaseShaderRegister = reg;
						range->RegisterSpace = 0;
						range->Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
						range->OffsetInDescriptorsFromTableStart = s;

						root_param[i].DescriptorTable.NumDescriptorRanges++;
					}

					last_type = sit.m_ItemType;
				}
				break;
			}
		}

		ID3DBlob* errors = nullptr;
		ID3DBlob* blob = nullptr;
		HRESULT hr = D3D12SerializeVersionedRootSignature(&desc, &blob, &errors);
		if (FAILED(hr))
		{
			ErrorMsg((const char*) errors->GetBufferPointer(), nullptr);
			return INVALID_ROOTSIG;
		}

		WriteD3DBlob(file, blob->GetBufferPointer(), blob->GetBufferSize(), "RootSig");


		if (errors)
			errors->Release();
		if (blob)
			blob->Release();
	}
	else
	{
		const int count = (int) items.size();

		int resource_table_item_count = 0;
		for (int i = 0; i < count; i++)
		{
			const SItem& it = items[i];
			if (it.m_ItemType == RESOURCE_TABLE || it.m_ItemType == SAMPLER_TABLE)
			{
				resource_table_item_count += (int) it.m_SubItems.size();
			}
		}


		const size_t slots_size = sizeof(SVulkanRoot) + (count - 1) * sizeof(SVulkanRootSlot);
		const size_t total_size = slots_size + resource_table_item_count * sizeof(SVulkanResourceTableItem);

		char* mem = new char[total_size];
		SVulkanRoot* root = (SVulkanRoot*) mem;
		SVulkanResourceTableItem* resource_table_items = (SVulkanResourceTableItem*) (mem + slots_size);

		root->m_NumSlots = count;

		for (int i = 0; i < count; i++)
		{
			const SItem& it = items[i];
			
			root->m_Slots[i].m_Type = it.m_ItemType;
			switch (it.m_ItemType)
			{
			case CONSTANT:
				root->m_Slots[i].m_Size = GetNumDWORDs(it.m_Type.c_str());
				break;
			case RESOURCE_TABLE:
			case SAMPLER_TABLE:
				root->m_Slots[i].m_Size = (uint32) it.m_SubItems.size();
				for (const SItem& it : it.m_SubItems)
				{
					resource_table_items->m_Type = it.m_ItemType;
					resource_table_items->m_NumElements = it.m_Count? it.m_Count : 1;
					resource_table_items++;
				}
				break;
			default:
				root->m_Slots[i].m_Size = it.m_Count;
			}
		}

		ASSERT((char*) resource_table_items - mem == total_size);
		WriteD3DBlob(file, mem, total_size, "RootSig");

		delete [] mem;
	}

	return SUCCESS;
}

static inline bool IsValidTypeCharacter(const char ch)
{
	return IsAlphaNumeric(ch) || (ch == '<') || (ch == '>') || (ch == '(') || (ch == ')');
}

static inline bool IsTypeOrName(const char ch)
{
	return ch != ';' && ch != '[';
}

static void ParseTypeAndName(SItem& item, Tokenizer& tokenizer)
{
	// Parse type and name, then split out name and take everything to the left as the type. This allows complex types with whitespace, commas, <>, () etc.
	char* type_and_name = tokenizer.Next<IsTypeOrName>();
	// Find end of string
	char* name = type_and_name;
	while (*name) name++;
	// Parse backwards while valid character for a name
	do { --name; } while (IsAlphaNumeric(*name));
	// Terminate type string and move forward
	*name = 0;
	name++;

	item.m_Type = type_and_name;
	item.m_Name = name;
}

Error ParseRootSig(FILE* file, ShaderLanguage shader_language, char*& hlsl_header, char* root_sig)
{
	char* begin = strstr(root_sig, "ROOT_BEGIN");
	if (begin == nullptr)
		return SYNTAX_ERROR;

	char* footer = strstr(begin, "ROOT_END");
	if (footer == nullptr)
		return SYNTAX_ERROR;
	footer += 9;


	// Save header before parsing, because parser will stomp all over the string
	size_t header_size = begin - root_sig;
	//char* header = new char[header_size];
	//memcpy(header, root_sig, header_size);


	*begin = '\0';


	// Parse struct names out of the header
/*
	std::vector<std::string> struct_types;

	Tokenizer tokenizer(2);
	tokenizer.SetString(root_sig);
	char* next;
	while ((next = tokenizer.Next()) != nullptr)
	{
		if (strcmp(next, "struct") == 0)
		{
			char* name = tokenizer.Next();
			struct_types.push_back(name);
		}
	}
*/

	//char* end = strstr(begin, "ROOT_END");
	//if (end == nullptr)
	//	return -2;
	// To 
	//*end = '\0';


	std::vector<SItem> items;

	Tokenizer tokenizer(2);
	tokenizer.SetString(begin + 10);

	char* tok = tokenizer.Next();
	while (tok && strcmp(tok, "ROOT_END") != 0)
	{
		if (strcmp(tok, "RT_BEGIN") == 0 || strcmp(tok, "ST_BEGIN") == 0)
		{
			const bool is_sampler_table = (tok[0] == 'S');

			SItem item;
			item.m_ItemType = is_sampler_table? SAMPLER_TABLE : RESOURCE_TABLE;
			//item.m_Type = nullptr;
			item.m_Name = tokenizer.Next();
			item.m_Count = 0;

			while (true)
			{
				tok = tokenizer.Next();
				if (strcmp(tok, "RT_END") == 0 || strcmp(tok, "ST_END") == 0)
					break;

				SItem subitem;
				subitem.m_ItemType = GetItemType(tok);

				if (is_sampler_table)
				{
					if (subitem.m_ItemType != SMP)
					{
						ErrorMsg("Invalid type in SamplerTable: ", tok);
						return INVALID_TYPE;
					}
				}
				else
				{
					if (subitem.m_ItemType != TEXTURE &&
						subitem.m_ItemType != STRUCTUREDBUFFER &&
						subitem.m_ItemType != RWTEXTURE &&
						subitem.m_ItemType != RWSTRUCTUREDBUFFER &&
						subitem.m_ItemType != CBV)
					{
						ErrorMsg("Invalid type in ResourceTable: ", tok);
						return INVALID_TYPE;
					}
				}

				ParseTypeAndName(subitem, tokenizer);

				tok = tokenizer.Next();

				subitem.m_Count = 0;
				if (strcmp(tok, "[") == 0)
				{
					tok = tokenizer.Next();
					subitem.m_Count = atoi(tok);
					tok = tokenizer.Next();
					if (strcmp(tok, "]") != 0)
					{
						ErrorMsg("Expected \"]\", got: ", tok);
						return UNEXPECTED_TOKEN;
					}

					tok = tokenizer.Next();
				}

				if (strcmp(tok, ";") != 0)
				{
					ErrorMsg("Expected \";\", got: ", tok);
					return UNEXPECTED_TOKEN;
				}

				item.m_SubItems.push_back(subitem);
			}

			items.push_back(item);
		}
		else
		{
			SItem item;
			item.m_ItemType = GetItemType(tok);
			if (item.m_ItemType == INVALID)
			{
				ErrorMsg("Unknown type: ", tok);
				return UNKNOWN_TYPE;
			}

			item.m_Count = 1;
			ParseTypeAndName(item, tokenizer);

/*
			if (IsType(g_RootConstTypes, ARRAY_COUNT(g_RootConstTypes), tok))
			{
				item.m_ItemType = ROOT_CONSTANT;
			}
			else if (IsKnownType(g_SRVTypes, ARRAY_COUNT(g_SRVTypes), tok))
			{
				item.m_ItemType = SRV;
			}
			else if (IsKnownType(g_UAVTypes, ARRAY_COUNT(g_UAVTypes), tok))
			{
				item.m_ItemType = UAV;
			}
			else if (IsType(struct_types.data(), (int) struct_types.size(), tok))
			{
				item.m_ItemType = CBV;
			}
			else
			{
				ErrorMsg("Unknown type: ", tok);
				return -3;
			}
*/

			tok = tokenizer.Next();

			if (strcmp(tok, ";") != 0)
			{
				ErrorMsg("Expected \";\", got: ", tok);
				return UNEXPECTED_TOKEN;
			}

			items.push_back(item);
		}

		tok = tokenizer.Next();
	}


	//if (!WriteHLSLHeader(hlsl_filename, header, header_size, footer, items))
	//	return false;

	hlsl_header = WriteShaderHeader(root_sig, header_size, footer, items, shader_language);

	Error error = WriteCppHeader(file, root_sig, header_size, items, shader_language);


//	delete [] header;

	return error;
}

int RunProgram(const char* command, const char* temp_file, const char* real_file)
{
	FILE* pipe = _popen(command, "rt");
	if (pipe == nullptr)
		return -1;

	// Read pipe until end of file, or an error occurs
	char str[4096];
	while (fgets(str, sizeof(str), pipe))
	{
		char* temp_str = strstr(str, temp_file);
		if (temp_str)
		{
			/*
			char save_ch = *temp_str;
			*temp_str = '\0';
			printf("%s", str);
			*temp_str = save_ch;

			printf("%s", real_file);
			temp_str += strlen(temp_file);
			printf("%s", temp_str);
			*/

			printf("%s", real_file);

			*temp_str = '\0';
			temp_str += strlen(temp_file);
			if (*temp_str == ':')
			{
				char* line = ++temp_str;
				while (IsNumeric(*temp_str))
					++temp_str;
				char save_ch = *temp_str;
				*temp_str = '\0';
				printf("(%s):", line);

				// If it happened to be a ':', then skip it since we included one above. Other it might've been a useful char, so restore it.
				if (save_ch == ':')
					++temp_str;
				else
					*temp_str = save_ch;
			}

			printf(" %s%s", str, temp_str);
		}
		else
		{
			printf("%s", str);
		}
	}

	if (!feof(pipe))
		return -2;

	return _pclose(pipe);
}

struct SShaderSource
{
	char* m_Start;
	char* m_Code;
	std::vector<char*> m_Names;
	uint  m_Line;
};

bool CompileShader(FILE* file, const char* external_compiler, const char* options, const SShaderSource& source, ShaderLanguage shader_language, const bool debug, const char* header, const char* root_header, const char* filename, const char *profile)
{
	const int num_shaders = (int) source.m_Names.size();
	for (int sh = 0; sh < num_shaders; sh++)
	{
		const char* name = source.m_Names[sh];

		std::string shader;
		if (shader_language == GLSL)
			shader = "#version 460\nlayout(row_major) uniform;\n#extension GL_GOOGLE_include_directive : enable\n";
		shader += header;
		shader += root_header;
		shader += "#define ";
		shader += name;
		shader += "\n#line " + std::to_string(source.m_Line) + "\n";
		shader += source.m_Code;

		if (external_compiler)
		{
			// Use process ID to generate a unique temp-file, so that we can compile multiple .pipeline files in parallel
			DWORD process_id = GetCurrentProcessId();

			char temp_src[128];
			sprintf_s(temp_src, ".\\_temp_shader_%s_%d_%d.txt", name, sh, process_id);

			char temp_dst[128];
			sprintf_s(temp_dst, ".\\_temp_blob_%s_%d_%d.blb", name, sh, process_id);

			WriteFile(temp_src, shader.c_str(), shader.length());

			char command[1024];
			sprintf_s(command, options, external_compiler, profile, temp_dst, temp_src);

			//sprintf_s(command, "\"%s\" -Zpr -WX -Qstrip_reflect -nologo -T %s -Fo %s %s", external_compiler, profile, temp_dst, temp_src);

			int res = RunProgram(command, temp_src, filename);

			DeleteFile(temp_src);

			if (res == 0 && shader_language == GLSL && !debug)
			{
				sprintf_s(command, "spirv-opt.exe"
					" --strip-debug"
					" -O"
					" %s -o %s", temp_dst, temp_dst);

				res = RunProgram(command, temp_dst, filename);
			}


			if (res)
			{
				DeleteFile(temp_dst);
				return false;
			}

			void* data;
			size_t size;
			bool result = ReadFile(temp_dst, data, size);
			if (result)
			{
				WriteD3DBlob(file, data, size, name);
				delete [] data;
			}

			DeleteFile(temp_dst);

			if (!result)
				return false;
		}
		else
		{
			D3D_SHADER_MACRO defines[] =
			{
				{ nullptr, nullptr },
			};

			UINT flags = D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
			if (debug)
				flags |= D3DCOMPILE_DEBUG;

			ID3DBlob* shader_blob = nullptr;
			ID3DBlob* errors_blob = nullptr;
			HRESULT hr = D3DCompile(shader.c_str(), shader.length(), filename, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", profile, flags, 0, &shader_blob, &errors_blob);
			if (FAILED(hr))
			{
				char str[256];
				sprintf_s(str, "%s\n", ((const char *)errors_blob->GetBufferPointer()));
				printf(str);
				//#ifdef _DEBUG
				OutputDebugStringA(str);
				//#endif

			}

			if (errors_blob)
				errors_blob->Release();

			if (shader_blob)
			{
				WriteD3DBlob(file, shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), name);
				shader_blob->Release();
			}

			if (FAILED(hr))
				return false;
		}
	}

	return true;
}

uint GetLineNumber(const char* start, const char* pos)
{
	uint count = 1;
	while (start < pos)
	{
		if (*start == '\n')
			count++;
		start++;
	}
	return count;
}

void ParseShaders(std::vector<SShaderSource>& shaders, char* str, const char* marker, int len)
{
	// Find all shaders, parse their names, but don't terminate any strings yet
	char *vs = str;
	while (true)
	{
		vs = strstr(vs, marker);
		if (vs == nullptr)
			break;

		SShaderSource src;
		src.m_Start = vs;

		vs += len;

		while (true)
		{
			while (IsWhiteSpace(*vs)) vs++;
			src.m_Names.push_back(vs);

			while (IsAlphaNumeric(*vs)) vs++;
			while (IsWhiteSpace(*vs)) vs++;

			char ch = *vs++;
			if (ch == ']')
				break;
		}
		src.m_Code = vs;

		src.m_Line = GetLineNumber(str, vs);
		shaders.push_back(src);
	}
}

void PrepareShaders(const std::vector<SShaderSource>& shaders)
{
	// Terminate all strings as needed to separate shaders and finalize name strings
	for (const SShaderSource& s : shaders)
	{
		*s.m_Start = '\0';
		s.m_Code[-1] = '\0';

		for (char* str : s.m_Names)
		{
			while (IsAlphaNumeric(*str)) str++;
			*str = '\0';
		}
	}
}

int main(int argc, char **argv)
{
	const char* compiler = nullptr;
	const char* options = nullptr;

	const char* ts_profile = nullptr;
	const char* ms_profile = nullptr;
	const char* vs_profile = "vs_5_1";
	const char* ps_profile = "ps_5_1";
	const char* cs_profile = "cs_5_1";

	ShaderLanguage shader_language = HLSL;

	bool debug = false;
	bool fxc = false;
	bool dxc = false;
	bool glslang = false;

	if (argc < 2)
	{
		printf("Error: No input file\n");
		return INVALID_INPUT;
	}

	for (int i = 2; i < argc; i++)
	{
		if (strcmp(argv[i], "dxc") == 0)
			dxc = true;
		else if (strcmp(argv[i], "fxc") == 0)
			fxc = true;
		else if (strcmp(argv[i], "glslang") == 0)
			glslang = true;
		else if (strcmp(argv[i], "debug") == 0)
			debug = true;
		else
		{
			printf("Error: Unknown option \"%s\"\n", argv[i]);
			return UNKNOWN_OPTION;
		}
	}

	char sdk_path[MAX_PATH];
	if (fxc || dxc)
	{
		// Find the location of the DXC or FXC compiler
		HKEY key;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\WOW6432Node\\Microsoft\\Microsoft SDKs\\Windows\\v10.0", 0, KEY_READ, &key) == ERROR_SUCCESS)
		{
			DWORD sdk_path_len = sizeof(sdk_path);
			if (RegGetValueA(key, nullptr, "InstallationFolder", RRF_RT_REG_SZ, nullptr, sdk_path, &sdk_path_len) == ERROR_SUCCESS)
			{
				strcpy(sdk_path + sdk_path_len - 1, "bin\\");

				DWORD sdk_path_len2 = sizeof(sdk_path) - sdk_path_len - 4;
				if (RegGetValueA(key, nullptr, "ProductVersion", RRF_RT_REG_SZ, nullptr, sdk_path + sdk_path_len + 3, &sdk_path_len2) == ERROR_SUCCESS)
				{
					sprintf(sdk_path + sdk_path_len + 2 + sdk_path_len2, ".0\\x64\\%s.exe", dxc? "dxc" : "fxc");

					compiler = sdk_path;
				}
			}
			RegCloseKey(key);
		}

		if (debug)
			options = "\"%s\" -Zpr -WX -Zi -Od -nologo -T %s -Fo %s %s";
		else
			options = "\"%s\" -Zpr -WX -O3 -Qstrip_reflect -nologo -T %s -Fo %s %s";

		shader_language = HLSL;
		if (dxc)
		{
			vs_profile = "vs_6_0";
			//ms_profile = "ms_6_0";
			ps_profile = "ps_6_0";
			cs_profile = "cs_6_0";
		}
	}
	else if (glslang)
	{
		compiler = "glslangvalidator.exe";
		options = "\"%s\" -V --target-env vulkan1.1 -S %s -o %s %s";
		ts_profile = "task";
		ms_profile = "mesh";
		vs_profile = "vert";
		ps_profile = "frag";
		cs_profile = "comp";

		shader_language = GLSL;
	}


	const char* in_filename = argv[1];


	FILE* file = fopen(in_filename, "rb");
	if (file == nullptr)
	{
		printf("Error: Can't open \"%s\"\n", in_filename);
		return FILE_NOT_FOUND;
	}

	// Find file size
	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* str = new char[size + 1];
	fread(str, 1, size, file);
	str[size] = '\0';
	fclose(file);

	char* root_sig = strstr(str, "[Rootsig]");

	std::vector<SShaderSource> task_shaders;
	std::vector<SShaderSource> mesh_shaders;
	std::vector<SShaderSource> vertex_shaders;
	std::vector<SShaderSource> pixel_shaders;
	std::vector<SShaderSource> compute_shaders;

	ParseShaders(task_shaders,    str, "[Task:",    6);
	ParseShaders(mesh_shaders,    str, "[Mesh:",    6);
	ParseShaders(vertex_shaders,  str, "[Vertex:",  8);
	ParseShaders(pixel_shaders,   str, "[Pixel:",   7);
	ParseShaders(compute_shaders, str, "[Compute:", 9);

	if (root_sig)
	{
		*root_sig = '\0';
		root_sig += 9;
	}

	PrepareShaders(task_shaders);
	PrepareShaders(mesh_shaders);
	PrepareShaders(vertex_shaders);
	PrepareShaders(pixel_shaders);
	PrepareShaders(compute_shaders);

	const char* header = str;

	char name[MAX_PATH];
	const char* last_slash = strrchr(in_filename, '\\');

	strcpy_s(name, last_slash? last_slash + 1 : in_filename);
	char* ext = strchr(name, '.');
	if (ext)
		*ext = '\0';

	char out_filename_cpp[MAX_PATH];
	sprintf_s(out_filename_cpp, "%s.h", in_filename);

	file = fopen(out_filename_cpp, "wb");
	if (file == nullptr)
		return ACCESS_DENIED;

	fprintf(file, "#pragma once\n\nnamespace N%s {\n", name);

	char* root_header = nullptr;

	Error res = SUCCESS;
	if (root_sig)
	{
		Error error = ParseRootSig(file, shader_language, root_header, root_sig);
		if (error)
		{
			res = error;
			goto quit;
		}
	}

	const int ts_count = (int) task_shaders.size();
	for (int i = 0; i < ts_count; i++)
	{
		if (!CompileShader(file, compiler, options, task_shaders[i], shader_language, debug, header, root_header, in_filename, ts_profile))
		{
			res = COMPILE_ERROR;
			goto quit;
		}
	}

	const int ms_count = (int) mesh_shaders.size();
	for (int i = 0; i < ms_count; i++)
	{
		if (!CompileShader(file, compiler, options, mesh_shaders[i], shader_language, debug, header, root_header, in_filename, ms_profile))
		{
			res = COMPILE_ERROR;
			goto quit;
		}
	}

	const int vs_count = (int) vertex_shaders.size();
	for (int i = 0; i < vs_count; i++)
	{
		if (!CompileShader(file, compiler, options, vertex_shaders[i], shader_language, debug, header, root_header, in_filename, vs_profile))
		{
			res = COMPILE_ERROR;
			goto quit;
		}
	}

	const int ps_count = (int) pixel_shaders.size();
	for (int i = 0; i < ps_count; i++)
	{
		if (!CompileShader(file, compiler, options, pixel_shaders[i], shader_language, debug, header, root_header, in_filename, ps_profile))
		{
			res = COMPILE_ERROR;
			goto quit;
		}
	}

	const int cs_count = (int) compute_shaders.size();
	for (int i = 0; i < cs_count; i++)
	{
		if (!CompileShader(file, compiler, options, compute_shaders[i], shader_language, debug, header, root_header, in_filename, cs_profile))
		{
			res = COMPILE_ERROR;
			goto quit;
		}
	}

quit:

	fprintf(file, "\n}; // namespace N%s\n", name);
	fclose(file);

	delete [] root_header;
	delete [] str;

	return res;
}
