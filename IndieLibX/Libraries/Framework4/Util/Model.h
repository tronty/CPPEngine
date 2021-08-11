
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

#include "../Graphics/Renderer.h"
#include "../Util/Array.h"

enum AttribSemantic
{
	AS_VERTEX,
	AS_TEXCOORD,
	AS_NORMAL,
	AS_TANGENT,
	AS_BITANGENT,
};



class VertexStream
{
public:
	VertexStream()
		: m_Vertices(nullptr)
		, m_VertexCount(0)
		, m_VertexSize(0)
		, m_Indices(nullptr)
		, m_IndexCount(0)
		, m_Semantic(AS_VERTEX)
	{
	}

	VertexStream(AttribSemantic semantic, float* vertices, uint vertex_count, uint vertex_size, uint32* indices, uint index_count)
		: m_Vertices(vertices)
		, m_VertexCount(vertex_count)
		, m_VertexSize(vertex_size)
		, m_Indices(indices)
		, m_IndexCount(index_count)
		, m_Semantic(semantic)
	{
	}

	VertexStream::~VertexStream()
	{
		delete [] m_Vertices;
		delete [] m_Indices;
	}



	// Utility functions
	void CreateSphere(const uint subdiv_levels);

	void FillIndexRemapBuffer(uint32 *remap_buffer);
	void Optimize();
	
	uint GetVertexCount() const { return m_VertexCount; }
	uint GetVertexSize() const { return m_VertexSize; }
	uint GetIndexCount() const { return m_IndexCount; }

	float *GetVertices() const { return m_Vertices; }

private:
	float *m_Vertices;
	uint m_VertexCount;
	uint m_VertexSize;

	uint32 *m_Indices;
	uint m_IndexCount;

	AttribSemantic m_Semantic;
};

struct Batch
{
	uint StartIndex;
	uint IndexCount;
};

class Model
{
public:
	Model();
	~Model();

	bool Load(Device device, const char* filename);

	bool LoadObj(const char* filename);
	bool LoadT3d(const char* fileName, const bool removePortals = true, const bool removeInvisible = true, const bool removeTwoSided = false, const float texSize = 256.0f);


	uint* GetArrayIndices(const uint nVertices)
	{
		uint* indices = new uint[nVertices];
		for (uint i = 0; i < nVertices; i++)
			indices[i] = i;

		return indices;
	}

	int FindStream(AttribSemantic semantic, uint index = 0) const;

	int AddStream(AttribSemantic semantic, uint num_components, uint num_vertices, float* vertices, uint* indices, bool optimized);
	bool RemoveStream(int stream);
	bool RemoveStream(AttribSemantic semantic, uint index = 0){ return RemoveStream(FindStream(semantic, index)); }

	int AddBatch(uint start_index, uint index_count)
	{
		Batch batch;
		batch.StartIndex = start_index;
		batch.IndexCount = index_count;

		return m_Batches.Add(batch);
	}

	void Scale(uint stream, const float* scale)
	{
		const uint comp = m_Streams[stream].NumComponents;
		const uint count = m_Streams[stream].NumVertices;

		float* dest = m_Streams[stream].Vertices;
		for (uint i = 0; i < count; i++)
		{
			for (uint j = 0; j < comp; j++)
				*dest++ *= scale[j];
		}
	}

	void ScaleBias(uint stream, const float* scale, const float* bias)
	{
		const uint comp = m_Streams[stream].NumComponents;
		const uint count = m_Streams[stream].NumVertices;

		float* dest = m_Streams[stream].Vertices;
		for (uint i = 0; i < count; i++)
		{
			for (uint j = 0; j < comp; j++)
			{
				float new_val = (*dest) * scale[j] + bias[j];
				*dest++ = new_val;
			}
		}
	}

	void FlipComponents(uint stream, uint c0, uint c1)
	{
		const uint count = m_Streams[stream].NumVertices;
		for (uint i = 0; i < count; i++)
		{
			float *src = m_Streams[stream].Vertices + i * m_Streams[stream].NumComponents;

			float temp = src[c0];
			src[c0] = src[c1];
			src[c1] = temp;
		}
	}

	void ReverseWinding();
	bool ComputeNormals(const bool flat = false);
	bool ComputeTangentSpace(const bool flat = false);

	uint GetVertexSize() const;
	uint GetComponentCount() const;
	uint GetComponentCount(const int* cStreams, const uint nStreams) const;

	void Optimize();
	void OptimizeStream(const uint streamID);

	uint Assemble(const int* aStreams, const uint nStreams, float** destVertices, uint32** destIndices, bool separateArrays);

	bool CreateBuffers(Device device, const char* cache_filename = nullptr);
	void DestroyBuffers(Device device);

	void Finalize();

	uint GetAttribDesc(AttribDesc* desc, uint count) const;

	VertexSetup GetVertexSetup() const { return m_VertexSetup; }
	uint GetIndexCount() const { return m_IndexCount; }

	Batch GetBatch(uint batch) const { return m_Batches[batch]; }
	uint GetBatchCount() const { return m_Batches.GetCount(); }

protected:
	void CreateBuffers(Device device, uint32 vertex_size, uint32 vb_size);
	bool SaveToDisk(const char* filename, const float* vertices, uint32 vertex_count, const uint32* indices) const;

	struct Stream
	{
		AttribSemantic Type;
		uint32 NumComponents;

		float* Vertices;
		uint* Indices;
		uint NumVertices;

		bool Optimized;
	};

	Array <Stream> m_Streams;
	Array <Batch> m_Batches;
	uint m_IndexCount;

	Buffer m_VertexBuffer;
	Buffer m_IndexBuffer;

	VertexSetup m_VertexSetup;
};
