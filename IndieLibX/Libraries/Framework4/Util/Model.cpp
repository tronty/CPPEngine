
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

#include "Model.h"
#include "Tokenizer.h"
#include "KdTree.h"
#include "Hash.h"
#include "../Math/Vector.h"
#include <stdio.h>
#include <algorithm>

static const char* g_AttribSemantics[] =
{
	"POSITION",
	"TEXCOORD",
	"NORMAL",
	"TANGENT",
	"BITANGENT",
};


static void SubDivide(float3 *&dest, const float3 &v0, const float3 &v1, const float3 &v2, uint level)
{
	if (level)
	{
		level--;
		float3 v3 = normalize(v0 + v1);
		float3 v4 = normalize(v1 + v2);
		float3 v5 = normalize(v2 + v0);

		SubDivide(dest, v0, v3, v5, level);
		SubDivide(dest, v3, v4, v5, level);
		SubDivide(dest, v3, v1, v4, level);
		SubDivide(dest, v5, v4, v2, level);
	}
	else
	{
		*dest++ = v0;
		*dest++ = v1;
		*dest++ = v2;
	}
}

void VertexStream::CreateSphere(const uint subdiv_levels)
{
	m_VertexCount = 8 * 3 * (1 << (2 * subdiv_levels));

	float3 *vertices = new float3[m_VertexCount];

	// Tessellate an octahedron
	float3 px0(-1,  0,  0);
	float3 px1( 1,  0,  0);
	float3 py0( 0, -1,  0);
	float3 py1( 0,  1,  0);
	float3 pz0( 0,  0, -1);
	float3 pz1( 0,  0,  1);

	float3 *dest = vertices;
	SubDivide(dest, py0, px0, pz0, subdiv_levels);
	SubDivide(dest, py0, pz0, px1, subdiv_levels);
	SubDivide(dest, py0, px1, pz1, subdiv_levels);
	SubDivide(dest, py0, pz1, px0, subdiv_levels);
	SubDivide(dest, py1, pz0, px0, subdiv_levels);
	SubDivide(dest, py1, px0, pz1, subdiv_levels);
	SubDivide(dest, py1, pz1, px1, subdiv_levels);
	SubDivide(dest, py1, px1, pz0, subdiv_levels);

	ASSERT(uint(dest - vertices) == m_VertexCount);

	m_Vertices = (float *) vertices;
	m_VertexSize = 3;
	m_Semantic = AS_VERTEX;
}

void VertexStream::FillIndexRemapBuffer(uint32 *remap_buffer)
{
	const uint count = m_VertexCount;
	const uint size = m_VertexSize;

	KdTree <float> kd_tree(size, count);

	for (uint i = 0; i < count; i++)
	{
		float *vertex = m_Vertices + i * size;
		uint index = kd_tree.AddUnique(vertex);

		remap_buffer[i] = index;
	}

}

void VertexStream::Optimize()
{
	uint32 *remap_buffer = new uint32[m_VertexCount];

	FillIndexRemapBuffer(remap_buffer);

	const uint count = m_IndexCount;
	for (uint i = 0; i < count; i++)
	{
		m_Indices[i] = remap_buffer[m_Indices[i]];		
	}

	delete [] remap_buffer;
}





Model::Model()
{
	m_IndexCount = 0;
	m_VertexBuffer = nullptr;
	m_IndexBuffer  = nullptr;
	m_VertexSetup  = nullptr;
}

Model::~Model()
{
	Finalize();
}

int Model::FindStream(AttribSemantic semantic, uint index) const
{
	uint count = 0;
	for (uint i = 0; i < m_Streams.GetCount(); i++)
	{
		if (m_Streams[i].Type == semantic)
		{
			if (count == index)
				return i;
			count++;
		}
	}
	return -1;
}

int Model::AddStream(AttribSemantic semantic, uint num_components, uint num_vertices, float* vertices, uint* indices, bool optimized)
{
	if (indices == nullptr)
		indices = GetArrayIndices(num_vertices);

	//VertexStream stream(semantic, vertices, nVertices, nComponents, indices, 0);

	Stream stream;
	stream.Type = semantic;
	stream.NumComponents = num_components;
	stream.NumVertices = num_vertices;
	stream.Vertices  = vertices;
	stream.Indices   = indices;
	stream.Optimized = optimized;

	return m_Streams.Add(stream);
}

bool Model::RemoveStream(int stream)
{
	if (uint(stream) >= m_Streams.GetCount())
		return false;

	delete [] m_Streams[stream].Vertices;
	delete [] m_Streams[stream].Indices;

	m_Streams.OrderedRemove(stream);
	return true;
}

// Fast version of atoi, supports only plain digits, positive numbers and doesn't check overflow
int atoi_fast(const char* p)
{
	int value = 0;
	for (;;)
	{
		uint v = uint(*p) - '0';
		if (v > 9)
			return value;
		
		value = value * 10 + v;
		++p;
	}
}

// Faster version of atof() based on http://www.leapsecond.com/tools/fast_atof.c
double atof_fast(const char* p)
{
	// Get sign, if any.
	double sign = 1.0;
	if (*p == '-')
	{
		sign = -1.0;
		++p;
	}
	else if (*p == '+')
	{
		++p;
	}

	// Get digits before decimal point or exponent, if any.
	double value = 0.0;
	for (;;)
	{
		uint v = uint(*p) - '0';
		if (v > 9)
			break;
		value = value * 10.0 + v;
		++p;
	}

	// Get digits after decimal point, if any.
	if (*p == '.')
	{
		double pow10 = 10.0;
		++p;

		for (;;)
		{
			uint v = uint(*p) - '0';
			if (v > 9)
				break;
			value += double(v) / pow10;
			pow10 *= 10.0;
			++p;
		}
	}

	// Handle exponent, if any.
	int frac = 0;
	double scale = 1.0;
	if ((*p == 'e') || (*p == 'E'))
	{
		// Get sign of exponent, if any.
		++p;
		if (*p == '-')
		{
			frac = 1;
			++p;
		}
		else if (*p == '+')
		{
			++p;
		}

		// Get digits of exponent, if any.
		unsigned int expon = 0;
		for (;;)
		{
			uint v = uint(*p) - '0';
			if (v > 9)
				break;
			expon = expon * 10 + v;
			++p;
		}

		if (expon > 308)
			expon = 308;

		// Calculate scaling factor.
		while (expon >= 50) { scale *= 1e50; expon -= 50; }
		while (expon >=  8) { scale *= 1e8;  expon -=  8; }
		while (expon >   0) { scale *= 10.0; expon -=  1; }
	}

	// Return signed and scaled floating point result.
	return sign * (frac ? (value / scale) : (value * scale));
}

static float readFloat(Tokenizer& tok)
{
	char* str = tok.Next();
	if (str[0] == '-')
	{
		str = tok.Next();
		return -(float) atof_fast(str);
	}
	else
	{
		if (str[0] == '+')
			str = tok.Next();

		return (float) atof_fast(str);
	}
}

struct ObjMaterial
{
	char* name;
	Array<uint>* indices;
};

static uint GetObjMaterial(Array<ObjMaterial>& materials, const char* str)
{
	for (uint i = 0; i < materials.GetCount(); i++)
	{
		if (strcmp(str, materials[i].name) == 0)
			return i;
	}

	ObjMaterial mat;
	mat.name = new char[strlen(str) + 1];
	mat.indices = new Array<uint>();
	strcpy(mat.name, str);
	return materials.Add(mat);
}

static uint* Rearrange(const Array<ObjMaterial>& materials, const uint* srcIndices, const uint nIndices)
{
	uint *indices = new uint[nIndices];

	uint currIndex = 0;
	for (uint i = 0; i < materials.GetCount(); i++)
	{
		Array<uint> *mInds = materials[i].indices;
		for (uint j = 0; j < mInds->GetCount(); j += 2)
		{
			memcpy(indices + currIndex, srcIndices + (*mInds)[j], (*mInds)[j + 1] * sizeof(uint));
			currIndex += (*mInds)[j + 1];
		}
	}

	ASSERT(currIndex == nIndices);

	return indices;
}

bool Model::LoadObj(const char* filename)
{
	Tokenizer tok, lineTok;
	if (!tok.SetFile(filename))
	{
		char str[256];
		sprintf(str, "Couldn't open \"%s\"", filename);
		ErrorMsg(str);
		return false;
	}

	Array<float3> vertices;
	Array<float3> normals;
	Array<float2> texCoords;
	Array<float3> tangents;
	Array<float3> binormals;

	Array<uint> vtxIndices;
	Array<uint> nrmIndices;
	Array<uint> txcIndices;

	Array<ObjMaterial> materials;

	ObjMaterial mat;
	mat.name = new char[2];
	strcpy(mat.name, " ");
	mat.indices = new Array <uint>();
	materials.Add(mat);
	uint currIndex = 0;
	uint currMaterial = 0;


	uint n, vStart, nStart, tStart;
	char* str;

	while ((str = tok.Next()) != nullptr)
	{
		switch (str[0])
		{
		case 'f':
			lineTok.SetString(tok.NextLine());

			vStart = vtxIndices.GetCount();
			nStart = nrmIndices.GetCount();
			tStart = txcIndices.GetCount();
			n = 0;
			while ((str = lineTok.Next()) != nullptr)
			{
				if (n > 2)
				{
					vtxIndices.Add(vtxIndices[vStart]);
					vtxIndices.Add(vtxIndices[vtxIndices.GetCount() - 2]);
					if (texCoords.GetCount())
					{
						txcIndices.Add(txcIndices[tStart]);
						txcIndices.Add(txcIndices[txcIndices.GetCount() - 2]);
					}
					if (normals.GetCount())
					{
						nrmIndices.Add(nrmIndices[nStart]);
						nrmIndices.Add(nrmIndices[nrmIndices.GetCount() - 2]);
					}
				}

				int index;
				if (str[0] == '-')
					index = vertices.GetCount() - atoi_fast(lineTok.Next());
				else
					index = atoi_fast(str) - 1;
				vtxIndices.Add(index);

				if (texCoords.GetCount() || normals.GetCount()) lineTok.GoToNext();
				if (texCoords.GetCount())
				{
					str = lineTok.Next();
					if (str[0] == '-')
						index = texCoords.GetCount() - atoi_fast(lineTok.Next());
					else
						index = atoi_fast(str) - 1;
					txcIndices.Add(index);
				}
				if (normals.GetCount())
				{
					lineTok.GoToNext();

					str = lineTok.Next();
					if (str[0] == '-')
						index = normals.GetCount() - atoi_fast(lineTok.Next());
					else
						index = atoi_fast(str) - 1;
					nrmIndices.Add(index);
				}

				n++;
			}
			break;
		case 'v':
			float x, y, z;
			switch (str[1]){
			case '\0':
				x = readFloat(tok);
				y = readFloat(tok);
				z = readFloat(tok);
				vertices.Add(float3(x, y, z));
				break;
			case 'n':
				x = readFloat(tok);
				y = readFloat(tok);
				z = readFloat(tok);
				normals.Add(float3(x, y, z));
				break;
			case 't':
				x = readFloat(tok);
				y = readFloat(tok);
				texCoords.Add(float2(x, y));
				break;
			}
			break;
		case '#':
			/*if (*tok.Next() == '_')
			{
				tok.GoToNext();
				str = tok.Next();
				if (stricmp(str, "tangent") == 0)
				{
					x = readFloat(tok);
					y = readFloat(tok);
					z = readFloat(tok);
					tangents.Add(float3(x, y, z));
				}
				else if (stricmp(str, "binormal") == 0)
				{
					x = readFloat(tok);
					y = readFloat(tok);
					z = readFloat(tok);
					binormals.Add(float3(x, y, z));
				}
			}
			else*/
			{
				tok.GoToNextLine();
			}
			break;
		case 'u':
			if (vtxIndices.GetCount())
			{
				materials[currMaterial].indices->Add(currIndex);
				materials[currMaterial].indices->Add(vtxIndices.GetCount() - currIndex);
				currIndex = vtxIndices.GetCount();
			}
			else
			{
				// Ensure we don't get an empty material set in the beginning
				delete [] materials[0].name;
				delete materials[0].indices;
				materials.Clear();
			}

			currMaterial = GetObjMaterial(materials, tok.Next());
			break;
		default:
			tok.GoToNextLine();
			break;
		}
	}

	materials[currMaterial].indices->Add(currIndex);
	materials[currMaterial].indices->Add(vtxIndices.GetCount() - currIndex);

	m_IndexCount = vtxIndices.GetCount();

	uint vtx_count = vertices.GetCount();
	AddStream(AS_VERTEX, 3, vtx_count, (float *) vertices.AbandonArray(), Rearrange(materials, vtxIndices.GetArray(), m_IndexCount), false);
	if (texCoords.GetCount())
	{
		ASSERT(txcIndices.GetCount() == m_IndexCount);

		uint *indices = Rearrange(materials, txcIndices.GetArray(), m_IndexCount);

		uint tc_count = texCoords.GetCount();
		AddStream(AS_TEXCOORD, 2, tc_count, (float *) texCoords.AbandonArray(), indices, false);
		if (tangents.GetCount())
		{
			uint *tIndices = new uint[m_IndexCount];
			memcpy(tIndices, indices, m_IndexCount * sizeof(uint));

			uint tan_count = tangents.GetCount();
			AddStream(AS_TEXCOORD, 3, tan_count, (float *) tangents.AbandonArray(), tIndices, false);
		}
		if (binormals.GetCount())
		{
			uint *bIndices = new uint[m_IndexCount];
			memcpy(bIndices, indices, m_IndexCount * sizeof(uint));

			uint bin_count = binormals.GetCount();
			AddStream(AS_TEXCOORD, 3, bin_count, (float *) binormals.AbandonArray(), bIndices, false);
		}
	}
	if (normals.GetCount())
	{
		ASSERT(nrmIndices.GetCount() == m_IndexCount);

		uint nrm_count = normals.GetCount();
		AddStream(AS_NORMAL, 3, nrm_count, (float *) normals.AbandonArray(), Rearrange(materials, nrmIndices.GetArray(), m_IndexCount), false);
	}

	// Fix batches
	currIndex = 0;
	uint startIndex = 0;
	for (uint i = 0; i < materials.GetCount(); i++)
	{
		Array <uint> *mInds = materials[i].indices;
		for (uint j = 0; j < mInds->GetCount(); j += 2)
		{
			currIndex += (*mInds)[j + 1];
		}
		AddBatch(startIndex, currIndex - startIndex);
		startIndex = currIndex;

		delete materials[i].indices;
		delete [] materials[i].name;
	}

	return true;
}

struct T3dPoly
{
	char *textureName;
	unsigned long flags;
	Array <float3> vertices;
	float3 origin;
	float3 normal;
	float3 sVec;
	float3 tVec;
};

inline bool textureNameAlphabetical(const char ch)
{
	return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_' || ch == '.' || ch == '-');
}

bool Model::LoadT3d(const char* fileName, const bool removePortals, const bool removeInvisible, const bool removeTwoSided, const float texSize)
{
	Tokenizer tok;
	if (!tok.SetFile(fileName))
	{
		char str[256];
		sprintf(str, "Couldn't open \"%s\"", fileName);
		ErrorMsg(str);
		return false;
	}

	uint polyMask = 0;
	if (removePortals)   polyMask |= 0x4000000;
	if (removeInvisible) polyMask |= 0x1;
	if (removeTwoSided)  polyMask |= 0x100;


	Array <T3dPoly *> polys;
	char* str;
	while ((str = tok.Next()) != nullptr)
	{
		if (stricmp(str, "Begin") == 0)
		{
			str = tok.Next();
			if (stricmp(str, "Polygon") == 0)
			{
				T3dPoly *poly = new T3dPoly;
				poly->textureName = nullptr;
				poly->flags = 0;

				str = tok.Next();
				while (stricmp(str, "End") != 0)
				{
					if (stricmp(str, "Vertex") == 0)
					{
						float3 v;
						v.x = readFloat(tok);
						tok.GoToNext();
						v.z = -readFloat(tok);
						tok.GoToNext();
						v.y = readFloat(tok);
						poly->vertices.Add(v);
					}
					else if (stricmp(str, "Texture") == 0)
					{
						tok.GoToNext();
						str = tok.Next<textureNameAlphabetical>();
						poly->textureName = new char[strlen(str) + 1];
						strcpy(poly->textureName, str);
					}
					else if (stricmp(str, "Flags") == 0)
					{
						tok.GoToNext();
						str = tok.Next();
						poly->flags = strtoul(str, nullptr, 10);
					}
					else if (stricmp(str, "Origin") == 0)
					{
						poly->origin.x = readFloat(tok);
						tok.GoToNext();
						poly->origin.z = -readFloat(tok);
						tok.GoToNext();
						poly->origin.y = readFloat(tok);
					}
					else if (stricmp(str, "Normal") == 0)
					{
						poly->normal.x = readFloat(tok);
						tok.GoToNext();
						poly->normal.z = -readFloat(tok);
						tok.GoToNext();
						poly->normal.y = readFloat(tok);
					}
					else if (stricmp(str, "TextureU") == 0)
					{
						poly->sVec.x = readFloat(tok);
						tok.GoToNext();
						poly->sVec.z = -readFloat(tok);
						tok.GoToNext();
						poly->sVec.y = readFloat(tok);
					}
					else if (stricmp(str, "TextureV") == 0)
					{
						poly->tVec.x = readFloat(tok);
						tok.GoToNext();
						poly->tVec.z = -readFloat(tok);
						tok.GoToNext();
						poly->tVec.y = readFloat(tok);
					}
					str = tok.Next();
				}
				tok.GoToNext();

				if (poly->textureName && (poly->flags & polyMask) == 0)
				{
					polys.Add(poly);
				}
				else
				{
					delete [] poly->textureName;
					delete poly;
				}
			}
		}
	}

	std::sort(
		polys.GetArray(),
		polys.GetArray() + polys.GetCount(), 
		[](const T3dPoly* elem0, const T3dPoly* elem1) -> bool
		{ 
			return (strcmp(elem0->textureName, elem1->textureName) < 0);
		}
	);


	uint nVertices = 0;
	for (uint i = 0; i < polys.GetCount(); i++)
	{
		nVertices += polys[i]->vertices.GetCount();
	}
	m_IndexCount = 3 * nVertices - 6 * polys.GetCount();

	float3* vertices  = new float3[nVertices];
	float2* texCoords = new float2[nVertices];
	float3* tangents  = new float3[polys.GetCount()];
	float3* binormals = new float3[polys.GetCount()];
	float3* normals   = new float3[polys.GetCount()];

	uint* vtxIndices = new uint[m_IndexCount];
	uint* tanIndices = new uint[m_IndexCount];
	uint* binIndices = new uint[m_IndexCount];
	uint* nrmIndices = new uint[m_IndexCount];

	uint iDest = 0;
	uint startIndex = 0;
	for (uint i = 0; i < polys.GetCount(); i++)
	{
		T3dPoly* poly = polys[i];

		float3 *vtxDest = vertices  + startIndex;
		float2 *texDest = texCoords + startIndex;

		for (uint j = 0; j < 3 * poly->vertices.GetCount() - 6; j++)
		{
			tanIndices[iDest + j] = i;
			binIndices[iDest + j] = i;
			nrmIndices[iDest + j] = i;
		}

		for (uint j = 0; j < poly->vertices.GetCount(); j++)
		{
			vtxDest[j] = poly->vertices[j];
			texDest[j].x = dot(poly->vertices[j] - poly->origin, poly->sVec / texSize);
			texDest[j].y = dot(poly->vertices[j] - poly->origin, poly->tVec / texSize);
			if (j > 2)
			{
				vtxIndices[iDest++] = startIndex;
				vtxIndices[iDest++] = startIndex + j - 1;
			}
			vtxIndices[iDest++] = startIndex + j;
		}

		tangents [i] = normalize(poly->sVec);
		binormals[i] = normalize(poly->tVec);
		normals  [i] = normalize(poly->normal);

		startIndex += poly->vertices.GetCount();
	}

	uint *texIndices = new uint[m_IndexCount];
	memcpy(texIndices, vtxIndices, m_IndexCount * sizeof(uint));

	AddStream(AS_VERTEX,    3, nVertices,        (float *) vertices,  vtxIndices, false);
	AddStream(AS_TEXCOORD,  2, nVertices,        (float *) texCoords, texIndices, false);
	AddStream(AS_TANGENT,   3, polys.GetCount(), (float *) tangents,  tanIndices, false);
	AddStream(AS_BITANGENT, 3, polys.GetCount(), (float *) binormals, binIndices, false);
	AddStream(AS_NORMAL,    3, polys.GetCount(), (float *) normals,   nrmIndices, false);

	// Extract batches
	{
		char *currName = polys[0]->textureName;
		uint i = 0;
		startIndex = 0;
		while (true)
		{
			uint indexCount = 0;
			while (i < polys.GetCount() && strcmp(polys[i]->textureName, currName) == 0)
			{
				indexCount += 3 * (polys[i]->vertices.GetCount() - 2);
				i++;
			}
			AddBatch(startIndex, indexCount);
			startIndex += indexCount;
			if (i < polys.GetCount())
			{
				currName = polys[i]->textureName;
			}
			else
			{
				break;
			}
		}
	}

	// Clean up
	for (uint i = 0; i < polys.GetCount(); i++)
	{
		delete [] polys[i]->textureName;
		delete polys[i];
	}
	return true;
}

void Model::ReverseWinding()
{
	for (uint j = 0; j < m_Streams.GetCount(); j++)
	{
		uint* indices = m_Streams[j].Indices;
		for (uint i = 0; i < m_IndexCount; i += 3)
		{
			uint temp = indices[i];
			indices[i] = indices[i + 2];
			indices[i + 2] = temp;
		}
	}
}

bool Model::ComputeNormals(const bool flat)
{
	int vertexStream = FindStream(AS_VERTEX);
	if (vertexStream < 0)
		return false;

	OptimizeStream(vertexStream);

	float3 *vertices = (float3 *) m_Streams[vertexStream].Vertices;
	uint *indices = m_Streams[vertexStream].Indices;

	if (flat)
	{
		uint nFaces = m_IndexCount / 3;

		float3 *normals = new float3[nFaces];
		uint *nrmIndices = new uint[m_IndexCount];
		for (uint i = 0; i < nFaces; i++)
		{
			float3 v0 = vertices[indices[3 * i    ]];
			float3 v1 = vertices[indices[3 * i + 1]];
			float3 v2 = vertices[indices[3 * i + 2]];

			normals[i] = normalize(cross(v1 - v0, v2 - v0));
			nrmIndices[3 * i    ] = i;
			nrmIndices[3 * i + 1] = i;
			nrmIndices[3 * i + 2] = i;
		}
		AddStream(AS_NORMAL, 3, nFaces, (float *) normals, nrmIndices, false);

	}
	else
	{
		uint nVertices = m_Streams[vertexStream].NumVertices;

		float3 *normals = new float3[nVertices];
		memset(normals, 0, nVertices * sizeof(float3));

		for (uint i = 0; i < m_IndexCount; i += 3)
		{
			float3 v0 = vertices[indices[i    ]];
			float3 v1 = vertices[indices[i + 1]];
			float3 v2 = vertices[indices[i + 2]];

			float3 normal = normalize(cross(v1 - v0, v2 - v0));

			normals[indices[i    ]] += normal;
			normals[indices[i + 1]] += normal;
			normals[indices[i + 2]] += normal;
		}

		for (uint j = 0; j < nVertices; j++){
			normals[j] = normalize(normals[j]);
		}

		uint *nrmIndices = new uint[m_IndexCount];
		memcpy(nrmIndices, indices, m_IndexCount * sizeof(uint));

		AddStream(AS_NORMAL, 3, nVertices, (float *) normals, nrmIndices, false);
	}

	return true;
}

static void tangentVectors(const float3 &v0, const float3 &v1, const float3 &v2, const float2 &t0, const float2 &t1, const float2 &t2, float3 &sdir, float3 &tdir, float3 &normal)
{
	float3 dv0 = v1 - v0;
	float3 dv1 = v2 - v0;

	float2 dt0 = t1 - t0;
	float2 dt1 = t2 - t0;

	const float r = rcpf(dt0.x * dt1.y - dt1.x * dt0.y);
	sdir = float3(dt1.y * dv0.x - dt0.y * dv1.x, dt1.y * dv0.y - dt0.y * dv1.y, dt1.y * dv0.z - dt0.y * dv1.z) * r;
	tdir = float3(dt0.x * dv1.x - dt1.x * dv0.x, dt0.x * dv1.y - dt1.x * dv0.y, dt0.x * dv1.z - dt1.x * dv0.z) * r;
	normal = normalize(cross(dv0, dv1));
}

bool Model::ComputeTangentSpace(const bool flat)
{
	int streams[2] = { FindStream(AS_VERTEX), FindStream(AS_TEXCOORD) };

	if (streams[0] < 0 || streams[1] < 0)
		return false;

	float *vertexArrays[2];
	uint *indices;

	uint nVertices = Assemble(streams, 2, vertexArrays, &indices, true);

	float3 *vertices  = (float3 *) vertexArrays[0];
	float2 *texCoords = (float2 *) vertexArrays[1];

	if (flat)
	{
		uint nFaces = m_IndexCount / 3;

		float3 *tangents   = new float3[nFaces];
		float3 *bitangents = new float3[nFaces];
		float3 *normals    = new float3[nFaces];

		uint *indicesS = new uint[m_IndexCount];
		uint *indicesT = new uint[m_IndexCount];
		uint *indicesN = new uint[m_IndexCount];

		for (uint i = 0; i < nFaces; i++)
		{
			float3 v0 = vertices[indices[3 * i    ]];
			float3 v1 = vertices[indices[3 * i + 1]];
			float3 v2 = vertices[indices[3 * i + 2]];

			float2 t0 = texCoords[indices[3 * i    ]];
			float2 t1 = texCoords[indices[3 * i + 1]];
			float2 t2 = texCoords[indices[3 * i + 2]];

			float3 sdir, tdir, normal;
			tangentVectors(v0, v1, v2, t0, t1, t2, sdir, tdir, normal);

			sdir = normalize(sdir);
			tdir = normalize(tdir);

			tangents  [i] = normalize(sdir);
			bitangents[i] = normalize(tdir);
			normals   [i] = normal;

			indicesS[3 * i] = indicesS[3 * i + 1] = indicesS[3 * i + 2] = i;
			indicesT[3 * i] = indicesT[3 * i + 1] = indicesT[3 * i + 2] = i;
			indicesN[3 * i] = indicesN[3 * i + 1] = indicesN[3 * i + 2] = i;
		}

		AddStream(AS_TANGENT,   3, nFaces, (float *) tangents,   indicesS, false);
		AddStream(AS_BITANGENT, 3, nFaces, (float *) bitangents, indicesT, false);
		AddStream(AS_NORMAL,    3, nFaces, (float *) normals,    indicesN, false);

		delete [] indices;

	}
	else
	{
		float3 *tangents   = new float3[nVertices];
		float3 *bitangents = new float3[nVertices];
		float3 *normals    = new float3[nVertices];

		memset(tangents,   0, nVertices * sizeof(float3));
		memset(bitangents, 0, nVertices * sizeof(float3));
		memset(normals,    0, nVertices * sizeof(float3));

		for (uint i = 0; i < m_IndexCount; i += 3)
		{
			float3 v0 = vertices[indices[i    ]];
			float3 v1 = vertices[indices[i + 1]];
			float3 v2 = vertices[indices[i + 2]];

			float2 t0 = texCoords[indices[i    ]];
			float2 t1 = texCoords[indices[i + 1]];
			float2 t2 = texCoords[indices[i + 2]];

			float3 sdir, tdir, normal;
			tangentVectors(v0, v1, v2, t0, t1, t2, sdir, tdir, normal);

			sdir = normalize(sdir);
			tdir = normalize(tdir);

			tangents[indices[i    ]] += sdir;
			tangents[indices[i + 1]] += sdir;
			tangents[indices[i + 2]] += sdir;

			bitangents[indices[i    ]] += tdir;
			bitangents[indices[i + 1]] += tdir;
			bitangents[indices[i + 2]] += tdir;

			normals[indices[i    ]] += normal;
			normals[indices[i + 1]] += normal;
			normals[indices[i + 2]] += normal;
		}

		for (uint j = 0; j < nVertices; j++)
		{
			tangents  [j] = normalize(tangents  [j]);
			bitangents[j] = normalize(bitangents[j]);
			normals   [j] = normalize(normals   [j]);
		}

		uint *indicesS = new uint[m_IndexCount];
		uint *indicesT = new uint[m_IndexCount];
		memcpy(indicesS, indices, m_IndexCount * sizeof(uint));
		memcpy(indicesT, indices, m_IndexCount * sizeof(uint));

		AddStream(AS_TANGENT,   3, nVertices, (float *) tangents,   indicesS, false);
		AddStream(AS_BITANGENT, 3, nVertices, (float *) bitangents, indicesT, false);
		AddStream(AS_NORMAL,    3, nVertices, (float *) normals,    indices,  false);
	}

	delete [] vertices;
	delete [] texCoords;

	return true;
}

uint Model::GetVertexSize() const
{
	return GetComponentCount() * sizeof(float);
}

uint Model::GetComponentCount() const
{
	uint count = 0;
	for (uint i = 0; i < m_Streams.GetCount(); i++)
	{
		count += m_Streams[i].NumComponents;
	}
	return count;
}

uint Model::GetComponentCount(const int* cStreams, const uint nStreams) const
{
	uint count = 0;
	for (uint i = 0; i < nStreams; i++)
	{
		count += m_Streams[cStreams[i]].NumComponents;
	}
	return count;
}

void Model::Optimize()
{
	for (uint i = 0; i < m_Streams.GetCount(); i++)
		OptimizeStream(i);
}

void Model::OptimizeStream(const uint streamID)
{
	if (m_Streams[streamID].Optimized)
		return;

	uint nComp = m_Streams[streamID].NumComponents;
	uint nVert = m_Streams[streamID].NumVertices;

	KdTree <float> kdTree(nComp, nVert);

	uint* indexRemap = new uint[nVert];
	float* vertices = m_Streams[streamID].Vertices;
	for (uint i = 0; i < nVert; i++)
	{
		float* vertex = vertices + i * nComp;
		uint index = kdTree.AddUnique(vertex);

		indexRemap[i] = index;
		memcpy(vertices + index * nComp, vertex, nComp * sizeof(float));
	}

	uint* indices = m_Streams[streamID].Indices;
	const uint index_count = m_IndexCount;
	for (uint j = 0; j < index_count; j++)
	{
		ASSERT(indices[j] < nVert);
		indices[j] = indexRemap[indices[j]];
	}

	delete [] indexRemap;
	m_Streams[streamID].NumVertices = kdTree.GetCount();
	m_Streams[streamID].Vertices = (float *) realloc(vertices, kdTree.GetCount() * nComp * sizeof(float));
	m_Streams[streamID].Optimized = true;
}

uint Model::Assemble(const int* aStreams, const uint nStreams, float** destVertices, uint32** destIndices, bool separateArrays)
{
	uint nComp = GetComponentCount(aStreams, nStreams);
	for (uint i = 0; i < nStreams; i++)
		OptimizeStream(aStreams[i]);

	const uint index_count = m_IndexCount;

	if (separateArrays)
	{
		for (uint i = 0; i < nStreams; i++)
			destVertices[i] = new float[m_Streams[aStreams[i]].NumComponents * index_count];
	}
	else
	{
		*destVertices = new float[nComp * index_count];
	}
	uint *iDest = *destIndices = new uint[index_count];

	uint *iIndex = new uint[nStreams];
	Hash hash(nStreams, index_count >> 3, index_count);
	for (uint j = 0; j < index_count; j++)
	{
		for (uint i = 0; i < nStreams; i++)
			iIndex[i] = m_Streams[aStreams[i]].Indices[j];

		uint index;
		if (!hash.Insert(iIndex, &index))
		{
			if (separateArrays)
			{
				for (uint i = 0; i < nStreams; i++)
				{
					uint nc = m_Streams[aStreams[i]].NumComponents;
					float *dest = destVertices[i] + index * nc;
					memcpy(dest, m_Streams[aStreams[i]].Vertices + m_Streams[aStreams[i]].Indices[j] * nc, nc * sizeof(float));
				}
			}
			else
			{
				float *dest = *destVertices + index * nComp;
				for (uint i = 0; i < nStreams; i++)
				{
					uint nc = m_Streams[aStreams[i]].NumComponents;
					memcpy(dest, m_Streams[aStreams[i]].Vertices + m_Streams[aStreams[i]].Indices[j] * nc, nc * sizeof(float));
					dest += nc;
				}
			}
		}

		*iDest++ = index;
	}
	delete [] iIndex;

	if (separateArrays)
	{
		for (uint i = 0; i < nStreams; i++)
			destVertices[i] = (float *) realloc(destVertices[i], hash.GetCount() * m_Streams[aStreams[i]].NumComponents * sizeof(float));
	}
	else
	{
		*destVertices = (float *) realloc(*destVertices, hash.GetCount() * nComp * sizeof(float));
	}
	return hash.GetCount();
}

void Model::CreateBuffers(Device device, uint32 vertex_size, uint32 vb_size)
{
	SBufferParams vb_params(vertex_size, vb_size, HEAP_DEFAULT, VERTEX_BUFFER, "Model");
	m_VertexBuffer = CreateBuffer(device, vb_params);

	SBufferParams ib_params(m_IndexCount * sizeof(uint32), HEAP_DEFAULT, INDEX_BUFFER, "Model");
	m_IndexBuffer = CreateBuffer(device, ib_params);

	m_VertexSetup = CreateVertexSetup(device, m_VertexBuffer, vertex_size, m_IndexBuffer, sizeof(uint32));
}

bool Model::CreateBuffers(Device device, const char* cache_filename)
{
	uint vertex_size = GetVertexSize();
	if (vertex_size == 0)
		return false;

	int* aStreams = new int[m_Streams.GetCount()];
	for (uint i = 0; i < m_Streams.GetCount(); i++)
		aStreams[i] = i;

	float* vertices;
	uint32* indices;
	uint num_vertices = Assemble(aStreams, m_Streams.GetCount(), &vertices, &indices, false);

/*
	if (nVertices > 65535){
		if ((indexBuffer = renderer->addIndexBuffer(nIndices, 4, STATIC, indices)) == IB_NONE) return 0;
	} else {
		convertToShorts(indices, nIndices, nVertices);

		if ((indexBuffer = renderer->addIndexBuffer(nIndices, 2, STATIC, indices)) == IB_NONE) return 0;
	}
*/
	uint vb_size = vertex_size * num_vertices;

	CreateBuffers(device, vertex_size, vb_size);

	SetBufferData(GetMainContext(device), m_VertexBuffer, vertices, vb_size);
	SetBufferData(GetMainContext(device), m_IndexBuffer, indices, m_IndexCount * sizeof(uint32));

	if (cache_filename)
	{
		Finalize();
		SaveToDisk(cache_filename, vertices, num_vertices, indices);
	}


	delete [] aStreams;

	delete [] vertices;
	delete [] indices;

	return true;
}

void Model::DestroyBuffers(Device device)
{
	DestroyVertexSetup(device, m_VertexSetup);

	DestroyBuffer(device, m_VertexBuffer);
	DestroyBuffer(device, m_IndexBuffer);
}

void Model::Finalize()
{
	for (uint i = 0; i < m_Streams.GetCount(); i++)
	{
		delete[] m_Streams[i].Vertices;
		delete[] m_Streams[i].Indices;

		m_Streams[i].Vertices = nullptr;
		m_Streams[i].Indices  = nullptr;
		m_Streams[i].NumVertices = 0;
		m_Streams[i].Optimized = false;
	}
}

struct CachedModelHeader
{
	uint32 Version;
	uint32 NumStreams;
	uint32 NumBatches;
	uint32 NumVertices;
	uint32 NumIndices;
};

bool Model::Load(Device device, const char* filename)
{
	FILE* file = fopen(filename, "rb");
	if (!file)
		return false;

	CachedModelHeader header;
	fread(&header, sizeof(header), 1, file);

	m_Streams.SetCount(header.NumStreams);
	m_Batches.SetCount(header.NumBatches);
	m_IndexCount = header.NumIndices;

	fread(m_Streams.GetArray(), sizeof(m_Streams[0]), header.NumStreams, file);
	fread(m_Batches.GetArray(), sizeof(m_Batches[0]), header.NumBatches, file);

	uint vertex_size = GetVertexSize();
	uint vb_size = vertex_size * header.NumVertices;
	uint ib_size = sizeof(uint32) * header.NumIndices;

	CreateBuffers(device, vertex_size, vb_size);

	Context context = GetMainContext(device);

	SMapBufferParams map_vb_params(context, m_VertexBuffer, 0, vb_size);
	uint8* vb = MapBuffer(map_vb_params);
	fread(vb, vb_size, 1, file);
	UnmapBuffer(map_vb_params);

	SMapBufferParams map_ib_params(context, m_IndexBuffer, 0, ib_size);
	uint8* ib = MapBuffer(map_ib_params);
	fread(ib, ib_size, 1, file);
	UnmapBuffer(map_ib_params);

	fclose(file);
	return true;
}

bool Model::SaveToDisk(const char* filename, const float* vertices, uint32 vertex_count, const uint32* indices) const
{
	FILE* file = fopen(filename, "wb");
	if (!file)
		return false;

	CachedModelHeader header;
	header.Version = 1;
	header.NumStreams  = m_Streams.GetCount();
	header.NumBatches  = m_Batches.GetCount();
	header.NumVertices = vertex_count;
	header.NumIndices  = m_IndexCount;
	fwrite(&header, sizeof(header), 1, file);

	fwrite(m_Streams.GetArray(), sizeof(m_Streams[0]), m_Streams.GetCount(), file);
	fwrite(m_Batches.GetArray(), sizeof(m_Batches[0]), m_Batches.GetCount(), file);

	uint vertex_size = GetVertexSize();

	fwrite(vertices, vertex_size, vertex_count, file);
	fwrite(indices, sizeof(uint32), m_IndexCount, file);

	fclose(file);
	return true;
}

uint Model::GetAttribDesc(AttribDesc* desc, uint count) const
{
	const uint stream_count = m_Streams.GetCount();
	ASSERT(stream_count <= count);

	for (uint i = 0; i < stream_count; i++)
	{
		desc[i].Stream = 0;
		desc[i].Format = AttribFormat(VF_FLOAT + (m_Streams[i].NumComponents - 1));
		desc[i].Name   = g_AttribSemantics[m_Streams[i].Type];
	}

	return stream_count;
}
