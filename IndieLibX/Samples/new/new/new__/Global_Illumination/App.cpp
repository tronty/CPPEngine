//-----------------------------------------------------------------------------
// File: D3D10\Global Illumination\App.cpp
// Copyright (c) 2007 Advanced Micro Devices Inc. All rights reserved.
//-----------------------------------------------------------------------------
/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

const int SAMPLE_COUNT = 256;
D3DXFROMWINEVECTOR3 samples[SAMPLE_COUNT];

float factorial(const int x)
{
	float f = 1.0f;
	for (int i = 2; i <= x; i++)
	{
		f *= i;
	}

	return f;
}

// Evaluate an Associated Legendre Polynomial P(l, m, x) at x
float P(const int l, const int m, const float x)
{
	float pmm = 1.0f;
	if (m > 0)
	{
		float somx2 = sqrtf((1.0f - x) * (1.0f + x));

		float fact = 1.0;
		for (int i = 1; i <= m; i++)
		{
			pmm *= (-fact) * somx2;
			fact += 2.0;
		}
	}
	if (l == m) return pmm;

	float pmmp1 = x * (2.0f * m + 1.0f) * pmm;
	if (l == m + 1) return pmmp1;

	float pll = 0.0;
	for (int ll = m + 2; ll <= l; ++ll)
	{
		pll = ((2.0f * ll - 1.0f) * x * pmmp1 - (ll + m - 1.0f) * pmm) / (ll - m);
		pmm = pmmp1;
		pmmp1 = pll;
	}

	return pll;
}

// Normalization constant
float K(const int l, const int m)
{
	return sqrtf(((2.0f * l + 1.0f) * factorial(l - m)) / (4.0f * D3DXFROMWINE_PI * factorial(l + m)));
}

// SH coefficient computation
float SH(const int l, const int m, const float theta, const float phi)
{
	const float sqrt2 = 1.4142135623731f;

	if (m == 0)
		return K(l, 0) * P(l, m, cosf(theta));
	else if (m > 0)
		return sqrt2 * K(l, m) * cosf(m * phi) * P(l, m, cosf(theta));
	else
		return sqrt2 * K(l, -m) * sinf(-m * phi) * P(l, -m, cosf(theta));
}

float SH(const int l, const int m, const D3DXFROMWINEVECTOR3 &pos)
{
	float len = length(pos);

	float p = atan2f(pos.z, pos.x);
	float t = acosf(pos.y / len);

	return SH(l, m, t, p);
}

float SH_A(const int l, const int m, const D3DXFROMWINEVECTOR3 &pos)
{
	float d = dot(pos, pos);
	float len = sqrtf(d);

	float p = atan2f(pos.z, pos.x);
	float t = acosf(pos.y / len);

	return SH(l, m, t, p) * powf(d, -1.5f);
}
class Model
{
public:
	Model(){}
	virtual ~Model(){}
};
class MyModel : public Model
{
public:
MyModel::MyModel()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

MyModel::~MyModel(){}

/*
	Add a mesh from the model into the index range of a material.
	This function is used to arrange meshes in a way to reduce the
	number of draw calls, state changes and get better Hi-Z utilization.
*/
void MyModel::AddToMaterialRange(uint32 *indices, int &index, const int mat, const uint startMesh, const uint meshCount)
{
	for (uint mesh = startMesh; mesh < startMesh + meshCount; mesh++)
	{
		uint base  = m_pMesh[mesh].GetBaseTriangle();
		uint count = m_pMesh[mesh].GetNumTriangles();

		for (uint p = 0; p < count; p++)
		{
			indices[index++] = m_pTriangle[base + p].v[0];
			indices[index++] = m_pTriangle[base + p].v[1];
			indices[index++] = m_pTriangle[base + p].v[2];
		}
	}
	m_materialRange[mat + 1] = index;
}

bool MyModel::Load( const char *name, const char *texturePath)
{
	if (!Model::Load(name, texturePath)) return false;

	// Assemble the vertices into the format used in this sample
	Vertex_ *vertices = new Vertex_[m_numVertices];
	for (uint i = 0; i < m_numVertices; i++)
	{
		D3DXFROMWINEVECTOR3 pos = m_pVPosition[i];
		pos *= 0.2f;
		pos.y -= 6.2f;

		vertices[i].pos = pos;
		vertices[i].normal = m_pVNormal[i];

		// Pack texture coordinates into a uint
		uint tcX = (uint) (m_pVTexcoord[i].x * 1024.0f + 1024.5f);
		uint tcY = (uint) (m_pVTexcoord[i].y * 1024.0f + 1024.5f);
		vertices[i].texCoord = tcX | (tcY << 16);
	}

	/*
		Create the index buffer. We arrange indices to reduce the number
		of draw calls, state changes and get better Hi-Z utilization.
	*/
	uint32 *indices = new uint32[m_numTriangles * 3];

	int index = 0;
	m_materialRange[0] = 0;

	// Untextured materials
	AddToMaterialRange(indices, index, 0,  19, 1); // Hand
	AddToMaterialRange(indices, index, 1,  20, 1); // Ball
	AddToMaterialRange(indices, index, 1,  23, 1); // Horse
	AddToMaterialRange(indices, index, 1,  25, 1); // Sci-Fi weirdo
	AddToMaterialRange(indices, index, 1,  28, 1); // Bench
	AddToMaterialRange(indices, index, 1,  30, 1); // Frame
	AddToMaterialRange(indices, index, 2,  24, 1); // Horse stand
	AddToMaterialRange(indices, index, 2,  26, 1); // Sci-Fi weirdo stand
	AddToMaterialRange(indices, index, 2,  32, 1); // Globe stand
	AddToMaterialRange(indices, index, 3,  3, 15); // Ceiling, Pillars, Stands, Wall lights
	AddToMaterialRange(indices, index, 4,  0,  1); // Walls
	AddToMaterialRange(indices, index, 5,  21, 1); // Teapot
	// Masked materials
	AddToMaterialRange(indices, index, 6,  27, 1); // Globe
	// Textured materials
	AddToMaterialRange(indices, index, 7,  18, 1); // Ball-horse
	AddToMaterialRange(indices, index, 8,  22, 1); // Head
	AddToMaterialRange(indices, index, 9,  29, 1); // Picture
	AddToMaterialRange(indices, index, 10,  1, 1); // Floor
	// Lightmapped materials
	AddToMaterialRange(indices, index, 11, 2,  1); // Ceiling
	AddToMaterialRange(indices, index, 12, 31, 1); // Wall light quads

	if ((m_vertexBuffer = context->CreateVertexBuffer(m_numVertices * sizeof(Vertex_), USAGE_IMMUTABLE, vertices)) == 0) return false;
	if ((m_indexBuffer = context->CreateIndexBuffer(m_numTriangles * 3 * sizeof(uint32), USAGE_IMMUTABLE, indices)) == 0) return false;

	delete vertices;
	delete indices;



	// Load textures
	if ((m_textures[0] = (TextureID ) context->LoadTexture(("../../Media/Textures/lopal.bmp"     ), &m_textureSRVs[0])) == 0) return false;
	if ((m_textures[1] = (TextureID ) context->LoadTexture(("../../Media/Textures/headpal.bmp"   ), &m_textureSRVs[1])) == 0) return false;
	if ((m_textures[2] = (TextureID ) context->LoadTexture(("../../Media/Textures/picture.dds"   ), &m_textureSRVs[2])) == 0) return false;
	if ((m_textures[3] = (TextureID ) context->LoadTexture(("../../Media/Textures/floor.dds"     ), &m_textureSRVs[3])) == 0) return false;
	if ((m_textures[4] = (TextureID ) context->LoadTexture(("../../Media/Textures/globe.dds"     ), &m_textureSRVs[4])) == 0) return false;
	if ((m_textures[5] = (TextureID ) context->LoadTexture(("../../Media/Textures/wall_lm.bmp"   ), &m_textureSRVs[5])) == 0) return false;
	if ((m_textures[6] = (TextureID ) context->LoadTexture(("../../Media/Textures/ceiling_lm.dds"), &m_textureSRVs[6])) == 0) return false;

	// Create materials
	PerObject m0 = { D3DXFROMWINEVECTOR3(0.816f, 0.216f, 0.227f), 0, D3DXFROMWINEVECTOR4(0.45f, 0.15f, 0.15f, 16.0f) };
	PerObject m1 = { D3DXFROMWINEVECTOR3(0.435f, 0.443f, 0.682f), 0, D3DXFROMWINEVECTOR4(0.3f,  0.3f,  0.6f,  16.0f) };
	PerObject m2 = { D3DXFROMWINEVECTOR3(0.29f,  0.482f, 0.298f), 0, D3DXFROMWINEVECTOR4(0.15f, 0.3f,  0.15f, 16.0f) };
	PerObject m3 = { D3DXFROMWINEVECTOR3(0.973f, 0.894f, 0.8f  ), 0, D3DXFROMWINEVECTOR4(0.5f,  0.5f,  0.5f,  16.0f) };
	PerObject m4 = { D3DXFROMWINEVECTOR3(1.0f,   0.6f,   0.2f  ), 0, D3DXFROMWINEVECTOR4(4.0f,  2.4f,  1.6f,  24.0f) };
	PerObject m5 = { D3DXFROMWINEVECTOR3(1.0f,   1.0f,   1.0f  ), 0, D3DXFROMWINEVECTOR4(0.3f,  0.4f,  0.6f,   4.0f) };
	PerObject m6 = { D3DXFROMWINEVECTOR3(0.25f,  0.7f,   0.8f  ), 0, D3DXFROMWINEVECTOR4(0.7f,  0.7f,  0.8f,   4.0f) };
	PerObject m7 = { D3DXFROMWINEVECTOR3(0.2f,   0.2f,   0.2f  ), 0, D3DXFROMWINEVECTOR4(0.7f,  0.7f,  0.7f,  16.0f) };
	PerObject m8 = { D3DXFROMWINEVECTOR3(0.616f, 0.494f, 0.361f), 0, D3DXFROMWINEVECTOR4(0.1f,  0.1f,  0.1f,  32.0f) };
	PerObject m9 = { D3DXFROMWINEVECTOR3(0.5f,   0.5f,   0.5f  ), 0, D3DXFROMWINEVECTOR4(0.7f,  0.7f,  0.7f,  16.0f) };
	if ((m_materials[0] = context->CreateConstantBuffer(sizeof(PerObject), USAGE_IMMUTABLE, &m0)) == 0) return false;
	if ((m_materials[1] = context->CreateConstantBuffer(sizeof(PerObject), USAGE_IMMUTABLE, &m1)) == 0) return false;
	if ((m_materials[2] = context->CreateConstantBuffer(sizeof(PerObject), USAGE_IMMUTABLE, &m2)) == 0) return false;
	if ((m_materials[3] = context->CreateConstantBuffer(sizeof(PerObject), USAGE_IMMUTABLE, &m3)) == 0) return false;
	if ((m_materials[4] = context->CreateConstantBuffer(sizeof(PerObject), USAGE_IMMUTABLE, &m4)) == 0) return false;
	if ((m_materials[5] = context->CreateConstantBuffer(sizeof(PerObject), USAGE_IMMUTABLE, &m5)) == 0) return false;
	if ((m_materials[6] = context->CreateConstantBuffer(sizeof(PerObject), USAGE_IMMUTABLE, &m6)) == 0) return false;
	if ((m_materials[7] = context->CreateConstantBuffer(sizeof(PerObject), USAGE_IMMUTABLE, &m7)) == 0) return false;
	if ((m_materials[8] = context->CreateConstantBuffer(sizeof(PerObject), USAGE_IMMUTABLE, &m8)) == 0) return false;
	if ((m_materials[9] = context->CreateConstantBuffer(sizeof(PerObject), USAGE_IMMUTABLE, &m9)) == 0) return false;

	return true;
}

void MyModel::Setup()
{
	// Setup buffers and topology for drawing
	unsigned int stride = sizeof(Vertex_);
	unsigned int offset = 0;
	IRenderer::GetRendererInstance()->setVertexBuffer(0, 1, &m_vertexBuffer, &stride, &offset);
	IRenderer::GetRendererInstance()->setIndexBuffer(m_indexBuffer, FORMAT_R32_UINT, 0);

	IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_TRIANGLES);
}

void MyModel::SetupTexture( const int index)
{
	IRenderer::GetRendererInstance()->PSSetShaderResources(0, 1, &m_textureSRVs[index]);
}

void MyModel::SetupMaterial( const int index)
{
	IRenderer::GetRendererInstance()->PSSetConstantBuffers(1, 1, &m_materials[index]);
}

void MyModel::RenderMaterial( const int material, const int count = 1)
{
	int start = m_materialRange[material];
	int end = m_materialRange[material + count];
	IRenderer::GetRendererInstance()->DrawIndexed(end - start, start, 0);
}

void MyModel::RenderAll()
{
	IRenderer::GetRendererInstance()->DrawIndexed(m_numTriangles * 3, 0, 0);
}

	VertexBufferID m_vertexBuffer;
	VertexBufferID m_indexBuffer;

	TextureID m_textures[7];
	TextureID m_textureSRVs[7];

	VertexBufferID m_materials[10];

	int m_materialRange[14];
};


class App
{
public:
	const char *GetHomeDirectory(){ return ("."); }

void ResetCamera()
{
	// Set start position and rotation
	m_camera.SetPosition(D3DXFROMWINEVECTOR3(-15.9f, -0.57f, 8.05f));
	m_camera.SetRotation(0.0f, 4.4f, 0.0f);
}

bool OnKeyPress(const unsigned int key, const bool pressed)
{
#if 0
	if (pressed)
	{
		switch (key)
		{
		case 'P':
			m_showProbes = !m_showProbes;
			return true;
		case '9':
			m_animateLight = !m_animateLight;
			return true;
		case VK_ADD: // Cycle wall color
			if (!m_waitFrames)
			{
				m_currWallMaterial++;
				if (m_currWallMaterial >= WALL_MATERIAL_COUNT) m_currWallMaterial = 0;
				m_waitFrames = m_probeCount / PROBE_CUBES_PER_FRAME;
			}
			return true;
		case VK_SUBTRACT: // Cycle wall color
			if (!m_waitFrames)
			{
				if (m_currWallMaterial <= 0) m_currWallMaterial = WALL_MATERIAL_COUNT;
				m_currWallMaterial--;
				m_waitFrames = m_probeCount / PROBE_CUBES_PER_FRAME;
			}
			return true;
		case VK_F1: // Toggle help display
			m_showHelp = !m_showHelp;
			return true;
		}
	}
#endif
	return false;
}

bool Create()
{
	// Compute roughly uniformly distributed sample positions on a unit sphere
	for (int i = 0; i < SAMPLE_COUNT; i++)
	{
		D3DXFROMWINEVECTOR3 v;
		float len;
		do
		{
			v.x = ((float) stx_rand()) / RAND_MAX * 2 - 1;
			v.y = ((float) stx_rand()) / RAND_MAX * 2 - 1;
			v.z = ((float) stx_rand()) / RAND_MAX * 2 - 1;
			len = dot(v, v);
		}
		while (len < 0.9f * 0.9f || len > 1.1f * 1.1f);

		samples[i] = v * (1.0f / sqrtf(len));
	}


	// Create our rendering context. It'll take care of our D3D10 device and simplify some tasks.
	IRenderer::GetRendererInstance() = new D3D10Context();
	if (!IRenderer::GetRendererInstance()->Create(("Global Illumination"), FORMAT_R10G10B10A2_UNORM, FORMAT_D24_UNORM_S8_UINT, 800, 600, 4, false)) return false;

	// Let base-class initialize itself as well
	if (!D3D10Create()) return false;

	// Setup some camera parameters
	m_camera.SetFrustumExtents(1.5f, 0.1f, 50.0f);

	if (!m_lightCamera.LoadPath(("Path.pth"))) return false;
//	m_camera.LoadPath(("Path.pth"));


	// Initialize member variables to default values
	m_currProbe = 0;
	m_showProbes = false;
	m_showHelp = false;
	m_animateLight = true;
	m_currWallMaterial = 0;

	m_lmValCeiling1 = 1.3f;
	m_lmValCeiling2 = 1.3f;
	m_lmValWall = 2.0f;
	m_lightIntensity = 1.2f;
	m_exposure = 1.8f;

	return true;
}

void Destroy()
{
//	m_camera.SavePath(("Path.pth"));

	D3D10Destroy();
}

/*
	Compute the light transfer function SH coefficients. We're using a regular diffuse lighting model.
*/
float EvaluateTransferFunction(const D3DXFROMWINEVECTOR3 &normal, const D3DXFROMWINEVECTOR3 *samples, const int sampleCount, const float *shArray, const int l, const int m) const
{
	int index = l * (l + 1) + m;

	float sh = 0;
	for (int s = 0; s < sampleCount; s++)
	{
		float diffuse = dot(samples[s], normal);
		if (diffuse > 0)
		{
			sh += diffuse * shArray[index * sampleCount + s];
		}
	}
	return sh * (4 * D3DXFROMWINE_PI / sampleCount);
}

/*
	Create precomputed SH basis function tables. Each channel holds a basis
	function and each slice corresponds to a light probe cubemap face.
*/
bool CreateSHTextures()
{
	// Pre-compute the SH basis function values we need first to speed things up a bit
	float *shArray = new float[SH_BANDS * SH_BANDS * SAMPLE_COUNT];

	int index = 0;
	for (int l = 0; l < SH_BANDS; l++)
	{
		for (int m = -l; m <= l; m++)
		{
			for (int s = 0; s < SAMPLE_COUNT; s++)
			{
				shArray[index] = SH(l, m, samples[s]);
				index++;
			}
		}
	}



	// tex0 = SH transfer function cubemap
	// tex1 = SH basis function cubemap
	half4 *tex0 = new half4[SH_TF_CUBE_SIZE * SH_TF_CUBE_SIZE * 6];
	half4 *tex1 = new half4[PROBE_SIZE * PROBE_SIZE * 6];

	int l = 0;
	int m = 0;
	for (int i = 0; i < SH_COEFF_VECTORS; i++)
	{
		// Compute l & m for the next 4 coefficients
		int l4[4], m4[4];
		for (int k = 0; k < 4; k++)
		{
			l4[k] = l;
			m4[k] = m;
			if (m >= l)
			{
				l++;
				m = -l;
			}
			else
			{
				m++;
			}
		}

		uint index0 = 0;
		uint index1 = 0;
		D3DXFROMWINEVECTOR3 v;
		D3DXFROMWINEVECTOR4 sh;

		// Positive & negative X faces
		for (v.x = 1; v.x >= -1; v.x -= 2)
		{
			for (int y = 0; y < SH_TF_CUBE_SIZE; y++)
			{
				for (int z = 0; z < SH_TF_CUBE_SIZE; z++)
				{
					v.y =  1 - 2 * float(y + 0.5f) / SH_TF_CUBE_SIZE;
					v.z = (1 - 2 * float(z + 0.5f) / SH_TF_CUBE_SIZE) * v.x;

					D3DXFROMWINEVECTOR3 normal = normalize(v);
					sh.x = EvaluateTransferFunction(normal, samples, SAMPLE_COUNT, shArray, l4[0], m4[0]);
					sh.y = EvaluateTransferFunction(normal, samples, SAMPLE_COUNT, shArray, l4[1], m4[1]);
					sh.z = EvaluateTransferFunction(normal, samples, SAMPLE_COUNT, shArray, l4[2], m4[2]);
					sh.w = EvaluateTransferFunction(normal, samples, SAMPLE_COUNT, shArray, l4[3], m4[3]);
					tex0[index0++] = sh;
				}
			}
			for (int y = 0; y < PROBE_SIZE; y++)
			{
				for (int z = 0; z < PROBE_SIZE; z++)
				{
					v.y =  1 - 2 * float(y + 0.5f) / PROBE_SIZE;
					v.z = (1 - 2 * float(z + 0.5f) / PROBE_SIZE) * v.x;

					sh.x = SH_A(l4[0], m4[0], v);
					sh.y = SH_A(l4[1], m4[1], v);
					sh.z = SH_A(l4[2], m4[2], v);
					sh.w = SH_A(l4[3], m4[3], v);
					tex1[index1++] = sh;
				}
			}
		}
		// Positive & negative Y faces
		for (v.y = 1; v.y >= -1; v.y -= 2)
		{
			for (int z = 0; z < SH_TF_CUBE_SIZE; z++)
			{
				for (int x = 0; x < SH_TF_CUBE_SIZE; x++)
				{
					v.x =  2 * float(x + 0.5f) / SH_TF_CUBE_SIZE - 1;
					v.z = (2 * float(z + 0.5f) / SH_TF_CUBE_SIZE - 1) * v.y;

					D3DXFROMWINEVECTOR3 normal = normalize(v);
					sh.x = EvaluateTransferFunction(normal, samples, SAMPLE_COUNT, shArray, l4[0], m4[0]);
					sh.y = EvaluateTransferFunction(normal, samples, SAMPLE_COUNT, shArray, l4[1], m4[1]);
					sh.z = EvaluateTransferFunction(normal, samples, SAMPLE_COUNT, shArray, l4[2], m4[2]);
					sh.w = EvaluateTransferFunction(normal, samples, SAMPLE_COUNT, shArray, l4[3], m4[3]);
					tex0[index0++] = sh;
				}
			}

			for (int z = 0; z < PROBE_SIZE; z++)
			{
				for (int x = 0; x < PROBE_SIZE; x++)
				{
					v.x =  2 * float(x + 0.5f) / PROBE_SIZE - 1;
					v.z = (2 * float(z + 0.5f) / PROBE_SIZE - 1) * v.y;

					sh.x = SH_A(l4[0], m4[0], v);
					sh.y = SH_A(l4[1], m4[1], v);
					sh.z = SH_A(l4[2], m4[2], v);
					sh.w = SH_A(l4[3], m4[3], v);
					tex1[index1++] = sh;
				}
			}
		}
		// Positive & negative Z faces
		for (v.z = 1; v.z >= -1; v.z -= 2)
		{
			for (int y = 0; y < SH_TF_CUBE_SIZE; y++)
			{
				for (int x = 0; x < SH_TF_CUBE_SIZE; x++)
				{
					v.x = (2 * float(x + 0.5f) / SH_TF_CUBE_SIZE - 1) * v.z;
					v.y =  1 - 2 * float(y + 0.5f) / SH_TF_CUBE_SIZE;

					D3DXFROMWINEVECTOR3 normal = normalize(v);
					sh.x = EvaluateTransferFunction(normal, samples, SAMPLE_COUNT, shArray, l4[0], m4[0]);
					sh.y = EvaluateTransferFunction(normal, samples, SAMPLE_COUNT, shArray, l4[1], m4[1]);
					sh.z = EvaluateTransferFunction(normal, samples, SAMPLE_COUNT, shArray, l4[2], m4[2]);
					sh.w = EvaluateTransferFunction(normal, samples, SAMPLE_COUNT, shArray, l4[3], m4[3]);
					tex0[index0++] = sh;
				}
			}

			for (int y = 0; y < PROBE_SIZE; y++)
			{
				for (int x = 0; x < PROBE_SIZE; x++)
				{
					v.x = (2 * float(x + 0.5f) / PROBE_SIZE - 1) * v.z;
					v.y =  1 - 2 * float(y + 0.5f) / PROBE_SIZE;
					sh.x = SH_A(l4[0], m4[0], v);
					sh.y = SH_A(l4[1], m4[1], v);
					sh.z = SH_A(l4[2], m4[2], v);
					sh.w = SH_A(l4[3], m4[3], v);
					tex1[index1++] = sh;
				}
			}
		}

	{Image3 img;
	img.create(FORMAT_RGBA16f, SH_TF_CUBE_SIZE, SH_TF_CUBE_SIZE, SH_TF_CUBE_SIZE, 1);
	m_shCube[i]=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());}
	{Image3 img;
	img.create(FORMAT_RGBA16f, PROBE_SIZE, PROBE_SIZE, 1, 1);
	m_shCube[i]=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());}

	}

	delete tex0;
	delete tex1;

	delete shArray;

	return true;
}


bool Load()
{
	// Nvidia has a driver bug with comparison texture lookups
	bool useNvidiaWorkaround = (IRenderer::GetRendererInstance()->GetVendorID() == VENDOR_NVIDIA);

	// Pass on some #define values to the shader
	D3D10_SHADER_MACRO effectDefines[] =
	{
		DEF_MACRO(SH_COEFF_VECTORS),
		DEF_MACRO(PROBE_SLICES_PER_PASS),
		DEF_MACRO(SCALE),
		{ "NVIDIA_WORKAROUND", useNvidiaWorkaround? "1" : "0" },
		{ 0, 0 },
	};
	if ((m_lighting = IRenderer::GetRendererInstance()->LoadEffect("Lighting.fx", effectDefines)) == 0) return false;

	// Use our own constant buffer management for best performance
	if ((m_perFrameCB = IRenderer::GetRendererInstance()->CreateEffectConstantBuffer(m_lighting, "PerFrame")) == 0) return false;
	if ((m_gsMvpCB    = IRenderer::GetRendererInstance()->CreateEffectConstantBuffer(m_lighting, "GSMvp"   )) == 0) return false;
	if ((m_gsMvpInvCB = IRenderer::GetRendererInstance()->CreateEffectConstantBuffer(m_lighting, "GSMvpInv")) == 0) return false;


	/*
		Query some of the shaders from the effect. This is done to reduce the overhead while changing materials so
		we can just set the right texture, constant buffer or shader instead of having everything set all the time.
	*/
	
	for (int i = 0; i < LIGHTING_TECH; i++)
	{
		stx_Effect::stx_Pass *fxPass = m_lighting->GetTechniqueByIndex(0)->GetPassByIndex(i);
		fxPass->GetPixelShaderDesc(&desc);
		if (FAILED(desc.pShaderVariable->GetPixelShader(0, &m_lightingPS[i]))) return false;
		if (i < 2)
		{
			fxPass->GetGeometryShaderDesc(&desc);
			if (FAILED(desc.pShaderVariable->GetGeometryShader(0, &m_shadowGS[i]))) return false;
		}
	}


	// Manually load the light gradient texture because the D3DXFROMWINE function don't handle 1D textures
	ubyte grad[256 * 4];
	FILE *file = _tfopen(TEX_PATH ("light_grad.dds"), ("rb"));
	if (file == 0) return false;
	fseek(file, 128, SEEK_SET);
	fread(grad, 1, sizeof(grad), file);
	fclose(file);

	Image3 img;
	img.create(FORMAT_RGBA8, 256*4, 1, 1, 1);
	img.setPixels(&grad[0]);
	m_texLightGrad=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());

	// Load the model
	m_model = new MyModel();
	if (!m_model->Load( MODEL_PATH ("GIroom.am"), TEX_PATH)) return false;

	// Define the input layout
#if 0
	FormatDesc layout[] =
	{
		{ "Vertex_", 0, FORMAT_R32G32B32_FLOAT,    0, offsetOf(Vertex_, pos),    D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "Normal", 0, FORMAT_RGBA32F, 0, offsetOf(Vertex_, normal), D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
#else
	{FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_NORMAL, FORMAT_FLOAT, 4
	};
	m_lightingIL = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), -1);}
#endif

	// Create two rasterizer states so we can switch between back/none face culling.
	if ((m_cullBack = IRenderer::GetRendererInstance()->CreateRasterizerState(D3D10_CULL_BACK)) == 0) return false;
	if ((m_cullNone = IRenderer::GetRendererInstance()->CreateRasterizerState(D3D10_CULL_NONE)) == 0) return false;


	// Load the SH coefficient computation shader
	D3D10_SHADER_MACRO shDefines[] =
	{
		DEF_MACRO(PROBE_SIZE),
		{ 0, 0 },
	};
	if ((m_shEffect = IRenderer::GetRendererInstance()->LoadEffect(("SH.fx"), shDefines)) == 0) return false;


	// Define the input layout for the SH shader
#if 0
	FormatDesc shLayout[] =
	{
		{ "XY", 0, FORMAT_R32G32_FLOAT, 0, offsetOf(ShVertex, xy), D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "Z",  0, FORMAT_R32_UINT,     0, offsetOf(ShVertex,  z), D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
#else
	{FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 1
	};
	m_shCoeffIL = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), -1);}
#endif


	if (!CreateSHTextures()) return false;


	// Create the shadow map
	if ((m_shadowMap = IRenderer::GetRendererInstance()->CreateRenderTargetCube(FORMAT_R16_UNORM, SHADOWMAP_SIZE, 1, 1, &m_shadowMapRTV, 0, &m_shadowMapSRV)) == 0) return false;
	if ((m_shadowMapDepth = IRenderer::GetRendererInstance()->CreateDepthTargetCube(FORMAT_D16_UNORM, SHADOWMAP_SIZE, 1, 1, &m_shadowMapDepthDSV, 0)) == 0) return false;

	// Comparison filters are broken in current Nvidia drivers
	if (useNvidiaWorkaround)
	{
		if ((m_shadowMapSS = IRenderer::GetRendererInstance()->CreateSamplerState(D3D10_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D10_TEXTURE_ADDRESS_CLAMP, 0.0f)) == 0) return false;
	}
	else
	{
		if ((m_shadowMapSS = IRenderer::GetRendererInstance()->CreateSamplerState(D3D10_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D10_TEXTURE_ADDRESS_CLAMP, 0.0f)) == 0) return false;
	}

	// Create volume render targets that will hold the final SH values from the light probes
	for (int i = 0; i < SH_COEFF_VECTORS; i++)
	{
		for (int c = 0; c < 3; c++)
		{
			if ((m_shCoeffs[c][i] = IRenderer::GetRendererInstance()->CreateRenderTarget3D(FORMAT_RGBA16F, SIZE_X, SIZE_Y, SIZE_Z, 1, &m_shCoeffsRTV[c][i], &m_shCoeffsSRV[c][i])) == 0) return false;
		}
	}
	if ((m_shCoeffsSS = IRenderer::GetRendererInstance()->CreateSamplerState(D3D10_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D10_TEXTURE_ADDRESS_CLAMP, 0.0f)) == 0) return false;


	// Compute light probe positions and create geometry for updating the SH volume render targets
	bool probes[SIZE_Z][SIZE_Y][SIZE_X];
	stx_memset(probes, 1, sizeof(probes));

	// Clear out some empty space
	for (int y = 1; y < 3; y++)
	{
		for (int x = 3; x < 9; x++)
		{
			probes[3][y][x] = false;
		}
	}
	probes[3][2][9] = false;


	ShVertex coords[MAX_PROBE_COUNT + PROBE_CUBES_PER_FRAME - 1];
	m_probeCount = 0;
	for (int z = 0; z < SIZE_Z; z++)
	{
		for (int y = 0; y < SIZE_Y; y++)
		{
			for (int x = 0; x < SIZE_X; x++)
			{
				if (probes[z][y][x])
				{
					coords[m_probeCount].xy = D3DXFROMWINEVECTOR2((2.0f * x + 1.0f) / SIZE_X - 1.0f, 1.0f - (2.0f * y + 1.0f) / SIZE_Y);
					coords[m_probeCount].z = z;

					m_probePos[m_probeCount] = D3DXFROMWINEVECTOR3(SCALE_X * (2.0f * x + 1.0f - SIZE_X), SCALE_Y * (2.0f * y + 1.0f - SIZE_Y), SCALE_Z * (2.0f * z + 1.0f - SIZE_Z));

					m_probeCount++;
				}
			}
		}
	}
	// On first frame we wait until all probes have been updated before displaying
	m_waitFrames = m_probeCount / PROBE_CUBES_PER_FRAME;

	// Shuffle the probes to avoid a "moving line" as probes are updated over several frames
	for (int i = 0; i < 16384; i++)
	{
		int i0 = stx_rand() % m_probeCount;
		int i1 = stx_rand() % m_probeCount;

		ShVertex temp = coords[i0];
		coords[i0] = coords[i1];
		coords[i1] = temp;

		D3DXFROMWINEVECTOR3 tmp = m_probePos[i0];
		m_probePos[i0] = m_probePos[i1];
		m_probePos[i1] = tmp;
	}


	// Pad the array with the first vertices so we can wrap around without issuing multiple draw calls
	for (int i = 0; i < PROBE_CUBES_PER_FRAME - 1; i++)
	{
		coords[m_probeCount + i] = coords[i];
	}

	// Create the geometry for the SH coefficient pass
	if ((m_shCoeffVB = IRenderer::GetRendererInstance()->CreateVertexBuffer((m_probeCount + PROBE_CUBES_PER_FRAME - 1) * sizeof(ShVertex), USAGE_IMMUTABLE, coords)) == 0) return false;

	// Create the sampler states for base textures and lightmaps
	if ((m_baseSS = IRenderer::GetRendererInstance()->CreateSamplerState(D3D10_FILTER_ANISOTROPIC, D3D10_TEXTURE_ADDRESS_WRAP)) == 0) return false;
	if ((m_lightMapSS = IRenderer::GetRendererInstance()->CreateSamplerState(D3D10_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D10_TEXTURE_ADDRESS_CLAMP, 0.0f)) == 0) return false;

	/*
		Create light probe render target.
		The light probes are packed into a large 3D texture where each slice is a light probe cubemap face. By using a 3D texture
		we can render to multiple "cubemaps" in a single pass.
	*/
	if ((m_probes = IRenderer::GetRendererInstance()->CreateRenderTarget3D(FORMAT_R11G11B10_FLOAT, PROBE_SIZE, PROBE_SIZE, PROBE_PASSES_PER_FRAME * PROBE_SLICES_PER_PASS, 1, &m_probesRTV, &m_probesSRV)) == 0) return false;
	// Make an RTV for each chunk of probes that we'll render per pass
	for (int i = 0; i < PROBE_PASSES_PER_FRAME; i++)
	{
		if ((m_probesChunkRTVs[i] = IRenderer::GetRendererInstance()->CreateRenderTargetView3D(m_probes, FORMAT_R11G11B10_FLOAT, i * PROBE_SLICES_PER_PASS, PROBE_SLICES_PER_PASS)) == 0) return false;
	}

	// Make a depth buffer for the light probes
	if ((m_probesDepth = IRenderer::GetRendererInstance()->CreateDepthTarget2D(FORMAT_D16_UNORM, PROBE_SIZE, PROBE_SIZE, PROBE_SLICES_PER_PASS * PROBE_PASSES_PER_FRAME, 1, 1, &m_probesDepthDSV)) == 0) return false;
	for (int i = 0; i < PROBE_PASSES_PER_FRAME; i++)
	{
		if ((m_probesDepthChunkDSVs[i] = IRenderer::GetRendererInstance()->CreateDepthStencilView2D(m_probesDepth, FORMAT_D16_UNORM, i * PROBE_SLICES_PER_PASS, PROBE_SLICES_PER_PASS)) == 0) return false;
	}



	/*
		Initially clear all render targets
	*/
	

	IRenderer::GetRendererInstance()->ClearRenderTargetView(m_probesRTV, D3DXFROMWINEVECTOR4(0, 0, 0, 0));
	IRenderer::GetRendererInstance()->ClearRenderTargetView(m_shadowMapRTV, D3DXFROMWINEVECTOR4(0, 0, 0, 0));
	IRenderer::GetRendererInstance()->Clear(false, true, D3DXFROMWINEVECTOR4 (0.0f, 0.0f, 0.0f, 0.0f));

	for (int c = 0; c < 3; c++)
	{
		for (int i = 0; i < SH_COEFF_VECTORS; i++)
		{
			IRenderer::GetRendererInstance()->ClearRenderTargetView(m_shCoeffsRTV[c][i], D3DXFROMWINEVECTOR4(0, 0, 0, 0));
		}
	}

	return true;
}

void Unload()
{
	delete m_model;

	// Clean up all our resources
	//////////////SAFE_RELEASE(m_lighting);
	//////////////SAFE_RELEASE(m_shEffect);

	for (int i = 0; i < elementsOf(m_lightingPS); i++)
	{
		//////////////SAFE_RELEASE(m_lightingPS[i]);
	}
	for (int i = 0; i < elementsOf(m_shadowGS); i++)
	{
		//////////////SAFE_RELEASE(m_shadowGS[i]);
	}

	//////////////SAFE_RELEASE(m_baseSS);
	//////////////SAFE_RELEASE(m_lightMapSS);

	//////////////SAFE_RELEASE(m_lightingIL);

	//////////////SAFE_RELEASE(m_cullBack);
	//////////////SAFE_RELEASE(m_cullNone);

	//////////////SAFE_RELEASE(m_shadowMap);
	//////////////SAFE_RELEASE(m_shadowMapRTV);
	//////////////SAFE_RELEASE(m_shadowMapSRV);
	//////////////SAFE_RELEASE(m_shadowMapSS);

	//////////////SAFE_RELEASE(m_shadowMapDepth);
	//////////////SAFE_RELEASE(m_shadowMapDepthDSV);

	//////////////SAFE_RELEASE(m_probesDepth);
	//////////////SAFE_RELEASE(m_probesDepthDSV);

	//////////////SAFE_RELEASE(m_probes);
	//////////////SAFE_RELEASE(m_probesRTV);
	//////////////SAFE_RELEASE(m_probesSRV);

	for (int i = 0; i < PROBE_PASSES_PER_FRAME; i++)
	{
		//////////////SAFE_RELEASE(m_probesChunkRTVs[i]);
	}
	for (int i = 0; i < PROBE_PASSES_PER_FRAME; i++)
	{
		//////////////SAFE_RELEASE(m_probesDepthChunkDSVs[i]);
	}

	for (int i = 0; i < SH_COEFF_VECTORS; i++)
	{
		//////////////SAFE_RELEASE(m_shTable[i]);
		//////////////SAFE_RELEASE(m_shTableSRV[i]);

		//////////////SAFE_RELEASE(m_shCube[i]);
		//////////////SAFE_RELEASE(m_shCubeSRV[i]);

		for (int c = 0; c < 3; c++)
		{
			//////////////SAFE_RELEASE(m_shCoeffs[c][i]);
			//////////////SAFE_RELEASE(m_shCoeffsRTV[c][i]);
			//////////////SAFE_RELEASE(m_shCoeffsSRV[c][i]);
		}
	}
	//////////////SAFE_RELEASE(m_shCoeffsSS);

	//////////////SAFE_RELEASE(m_texLightGrad);
	//////////////SAFE_RELEASE(m_texLightGradSRV);

	//////////////SAFE_RELEASE(m_perFrameCB);
	//////////////SAFE_RELEASE(m_gsMvpCB);
	//////////////SAFE_RELEASE(m_gsMvpInvCB);

	//////////////SAFE_RELEASE(m_shCoeffVB);
	//////////////SAFE_RELEASE(m_shCoeffIL);
}

// Setup for rendering the scene
void SetupScene(const int passIndex)
{
	m_lighting->GetTechniqueByIndex(0)->GetPassByIndex(passIndex)->Apply(0);

	

	IRenderer::GetRendererInstance()->RSSetState(m_cullBack);
	IRenderer::GetRendererInstance()->setVertexFormat(m_lightingIL);

	m_model->Setup();

	SamplerStateID samplers[] = { m_shadowMapSS, m_baseSS, m_lightMapSS, m_shCoeffsSS };
	IRenderer::GetRendererInstance()->PSSetSamplers(0, 4, samplers);

	if (passIndex >= PROBE_PASS) // Shadow depth pass doesn't need any of these textures
	{
		TextureID textures[2 + 4 * SH_COEFF_VECTORS];
		TextureID dest = textures;

		*dest++ = m_shadowMapSRV;
		*dest++ = m_texLightGradSRV;

		// SH coefficient volumes
		for (int c = 0; c < 3; c++)
		{
			for (int i = 0; i < SH_COEFF_VECTORS; i++)
			{
				*dest++ = m_shCoeffsSRV[c][i];
			}
		}
		// SH coefficient cubemaps
		for (int i = 0; i < SH_COEFF_VECTORS; i++)
		{
			*dest++ = m_shCubeSRV[i];
		}

		IRenderer::GetRendererInstance()->PSSetShaderResources(1, elementsOf(textures), textures);
	}
}

void RenderScene(const int passIndex, const D3DXFROMWINEMATRIX *mvp, const D3DXFROMWINEMATRIX *mvpInv, const int matrixCount)
{
	

	// Update array of mvp matrices
	D3DXFROMWINEMATRIX *mvpArray;
	if (mvp)
	{
		m_gsMvpCB->Map(D3D10_MAP_WRITE_DISCARD, 0, (void **) &mvpArray);
			stx_memcpy(mvpArray, mvp, matrixCount * sizeof(D3DXFROMWINEMATRIX));
		m_gsMvpCB->Unmap();
	}
	if (mvpInv)
	{
		m_gsMvpInvCB->Map(D3D10_MAP_WRITE_DISCARD, 0, (void **) &mvpArray);
			stx_memcpy(mvpArray, mvpInv, matrixCount * sizeof(D3DXFROMWINEMATRIX));
		m_gsMvpInvCB->Unmap();
	}

	if (passIndex >= PROBE_PASS)
	{
		/*
			Untextured materials
		*/
		IRenderer::GetRendererInstance()->PSSetShader(m_lightingPS[passIndex]);

		// Hand
		m_model->SetupMaterial( 9);
		m_model->RenderMaterial( 0);

		// Ball + Horse + Sci-Fi weirdo + Bench + Frame
		m_model->SetupMaterial( 4);
		m_model->RenderMaterial( 1);

		// Horse stand + Sci-Fi weirdo stand + Globe stand
		m_model->SetupMaterial( 7);
		m_model->RenderMaterial( 2);

		// Ceiling + Pillars + Stands + Wall lights
		m_model->SetupMaterial( 5);
		m_model->RenderMaterial( 3);

		// Walls
		m_model->SetupMaterial( m_currWallMaterial);
		m_model->RenderMaterial( 4);

		IRenderer::GetRendererInstance()->RSSetState(m_cullNone);

		// Teapot
		m_model->SetupMaterial( 8);
		m_model->RenderMaterial( 5);


		/*
			Masked materials
		*/
		IRenderer::GetRendererInstance()->PSSetShader(m_lightingPS[passIndex + 1]);

		// Globe
		m_model->SetupMaterial( 6);
		m_model->SetupTexture( 4);
		m_model->RenderMaterial( 6);

		IRenderer::GetRendererInstance()->RSSetState(m_cullBack);


		/*
			Textured materials
		*/
		IRenderer::GetRendererInstance()->PSSetShader(m_lightingPS[passIndex + 2]);

		// Ball-horse
		m_model->SetupTexture( 0);
		m_model->RenderMaterial( 7);

		// Head
		m_model->SetupTexture( 1);
		m_model->RenderMaterial( 8);

		// Picture
		m_model->SetupTexture( 2);
		m_model->RenderMaterial( 9);

		// Floor
		m_model->SetupTexture( 3);
		m_model->RenderMaterial( 10);


		/*
			Lightmapped materials
		*/
		IRenderer::GetRendererInstance()->PSSetShader(m_lightingPS[passIndex + 3]);

		// Ceiling
		m_model->SetupMaterial( 5);
		m_model->SetupTexture( 6);
		m_model->RenderMaterial( 11);

		IRenderer::GetRendererInstance()->PSSetShader(m_lightingPS[passIndex + 4]);

		// Wall light quads
		m_model->SetupMaterial( m_currWallMaterial);
		m_model->SetupTexture( 5);
		m_model->RenderMaterial( 12);
	}
	else
	{
		/*
			Shadow depth pass
		*/
		IRenderer::GetRendererInstance()->GSSetShader(m_shadowGS[1]);
		IRenderer::GetRendererInstance()->PSSetShader(m_lightingPS[1]);
		IRenderer::GetRendererInstance()->RSSetState(m_cullNone);
		m_model->SetupTexture( 4);

		// Teapot + Globe
		m_model->RenderMaterial( 5, 2);

		IRenderer::GetRendererInstance()->GSSetShader(m_shadowGS[0]);
		IRenderer::GetRendererInstance()->PSSetShader(m_lightingPS[0]);
		IRenderer::GetRendererInstance()->RSSetState(m_cullBack);

		// The rest of the objects
		m_model->RenderMaterial( 1, 4);
		m_model->RenderMaterial( 7, 6);
		m_model->RenderMaterial( 0);
	}
}

void OnRender()
{
	if (m_keys['1']) m_lmValCeiling1 = max(m_lmValCeiling1 - 0.8f * m_frameTime, 0.0f);
	if (m_keys['2']) m_lmValCeiling1 = min(m_lmValCeiling1 + 0.8f * m_frameTime, 2.5f);
	if (m_keys['3']) m_lmValCeiling2 = max(m_lmValCeiling2 - 0.8f * m_frameTime, 0.0f);
	if (m_keys['4']) m_lmValCeiling2 = min(m_lmValCeiling2 + 0.8f * m_frameTime, 2.5f);
	if (m_keys['5']) m_lmValWall = max(m_lmValWall - 1.0f * m_frameTime, 0.0f);
	if (m_keys['6']) m_lmValWall = min(m_lmValWall + 1.0f * m_frameTime, 4.0f);
	if (m_keys['7']) m_lightIntensity = max(m_lightIntensity - 1.5f * m_frameTime, 0.0f);
	if (m_keys['8']) m_lightIntensity = min(m_lightIntensity + 1.5f * m_frameTime, 3.0f);
	if (m_keys[VK_NEXT])  m_exposure = max(m_exposure * powf(8.0f, -m_frameTime), 0.02f);
	if (m_keys[VK_PRIOR]) m_exposure = min(m_exposure * powf(8.0f,  m_frameTime), 8.0f);

	// 0 SRVs for resetting
	TextureID null[3 + 4 * SH_COEFF_VECTORS];
	stx_memset(null, 0, sizeof(null));


	// Update camera
	UpdateCameraPosition(8.0f * m_frameTime);
	D3DXFROMWINEMATRIX mvp = m_camera.GetModelViewProjection();
	D3DXFROMWINEVECTOR3 camPos = m_camera.GetPosition();

	// Get light position from looping camera path
	static D3DXFROMWINEVECTOR3 lightPos;
	if (m_animateLight)
	{
		static float t = 0;
		t += 0.8f * m_frameTime;
		m_lightCamera.GetNodeAt(t, &lightPos, 0, true);
	}


	// Update per frame constants
	PerFrame *pf;
	m_perFrameCB->Map(D3D10_MAP_WRITE_DISCARD, 0, (void **) &pf);
		pf->mvp = mvp;
		pf->lightPos = lightPos;
		pf->camPos = camPos;
		pf->intensity = m_lightIntensity;
		pf->exposure = m_exposure;
		pf->lmModulate[0] = D3DXFROMWINEVECTOR4(m_lmValCeiling1, m_lmValCeiling2, 0, 0);
		pf->lmModulate[1] = D3DXFROMWINEVECTOR4(m_lmValWall, 0.0f, 0, 0);
	m_perFrameCB->Unmap();

	




	D3DXFROMWINEMATRIX proj = CubemapProjectionMatrix(0.03f, 50.0f);
	D3DXFROMWINEMATRIX cubeMvp[PROBE_SLICES_PER_PASS];
	D3DXFROMWINEMATRIX cubeMvpInv[PROBE_SLICES_PER_PASS];

	/*
		Shadow map pass
	*/
	if (m_lightIntensity > 0.0f)
	{
		for (int i = 0; i < 6; i++)
		{
			D3DXFROMWINEMATRIX mv = CubemapModelviewMatrix(i);
			mv.translate(-lightPos);
			cubeMvp[i] = proj * mv;
		}

		D3DFROMWINEVIEWPORT9 vpShadowMap = { 0, 0, SHADOWMAP_SIZE, SHADOWMAP_SIZE, 0, 1 };
		IRenderer::GetRendererInstance()->RSSetViewports(1, &vpShadowMap);

		IRenderer::GetRendererInstance()->ClearDepthStencilView(m_shadowMapDepthDSV, D3D10_CLEAR_DEPTH, 1.0f, 0);
		IRenderer::GetRendererInstance()->changeRenderTarget(&m_shadowMapRTV, m_shadowMapDepthDSV);

		SetupScene(SHADOW_PASS);
		RenderScene(SHADOW_PASS, cubeMvp, 0, 6);

		IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);
	}



	/*
		Light probe passes
	*/
	D3DFROMWINEVIEWPORT9 vpProbes = { 0, 0, PROBE_SIZE, PROBE_SIZE, 0, 1 };
	IRenderer::GetRendererInstance()->RSSetViewports(1, &vpProbes);

	SetupScene(PROBE_PASS);

	IRenderer::GetRendererInstance()->ClearDepthStencilView(m_probesDepthDSV, D3D10_CLEAR_DEPTH, 1.0f, 0);

	int i = m_currProbe;
	for (int n = 0; n < PROBE_PASSES_PER_FRAME; n++)
	{
		// Compute all matrices for the chunk
		for (int c = 0; c < PROBE_CUBES_PER_PASS; c++)
		{
			for (int f = 0; f < 6; f++)
			{
				D3DXFROMWINEMATRIX mv = CubemapModelviewMatrix(f);
				mv.translate(-m_probePos[i]);
				D3DXFROMWINEMATRIX mvp = proj * mv;
				cubeMvp[c * 6 + f] = mvp;
				// This matrix transforms from screen space into world space.
				cubeMvpInv[c * 6 + f] = !mvp * translate(-1.0f, 1.0f, 0.0f) * scale4(2.0f / PROBE_SIZE, -2.0f / PROBE_SIZE, 1.0f);
			}
			i++;
			if (i >= m_probeCount) i = 0;
		}

		IRenderer::GetRendererInstance()->changeRenderTarget(&m_probesChunkRTVs[n], m_probesDepthChunkDSVs[n]);

		RenderScene(PROBE_PASS, cubeMvp, cubeMvpInv, PROBE_SLICES_PER_PASS);
	}


	/*
		Update the SH coefficient volumes
	*/
	IRenderer::GetRendererInstance()->PSSetShaderResources(0, elementsOf(null), null);

	D3DFROMWINEVIEWPORT9 shVP = { 0, 0, SIZE_X, SIZE_Y, 0, 1 };
	IRenderer::GetRendererInstance()->RSSetViewports(1, &shVP);

	IRenderer::GetRendererInstance()->SetEffect(m_shEffect);
	IRenderer::GetRendererInstance()->Apply("SH", "Main");

	IRenderer::GetRendererInstance()->setVertexFormat(m_shCoeffIL);
	IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_POINTS);

	unsigned int stride = sizeof(ShVertex);
	unsigned int offset = 0;
	IRenderer::GetRendererInstance()->setVertexBuffer(0, 1, &m_shCoeffVB, &stride, &offset);

	for (int i = 0; i < SH_COEFF_VECTORS; i++)
	{
		TextureID rt[] = { m_shCoeffsRTV[0][i], m_shCoeffsRTV[1][i], m_shCoeffsRTV[2][i]};
		IRenderer::GetRendererInstance()->OMSetRenderTargets(3, rt, 0);

		IRenderer::GetRendererInstance()->PSSetShaderResources(0, 1, &m_shTableSRV[i]);
		if (i == 0) IRenderer::GetRendererInstance()->PSSetShaderResources(1, 1, &m_probesSRV);

		IRenderer::GetRendererInstance()->Draw(PROBE_CUBES_PER_FRAME, m_currProbe);
	}



	// Step forward for the next frame
	m_currProbe += PROBE_CUBES_PER_FRAME;
	if (m_currProbe >= m_probeCount) m_currProbe -= m_probeCount;



	if (m_waitFrames)
	{
		m_waitFrames--;

		IRenderer::GetRendererInstance()->PSSetShaderResources(0, elementsOf(null), null);

		IRenderer::GetRendererInstance()->Present();
		return;
	}

	IRenderer::GetRendererInstance()->SetRTToBackBuffer();


	/*
		Final pass rendering direct and indirect lighting to the backbuffer
	*/
	IRenderer::GetRendererInstance()->Clear(false, true, D3DXFROMWINEVECTOR4 (0.0f, 0.0f, 0.0f, 0.0f));

		SetupScene(FINAL_PASS);
		RenderScene(FINAL_PASS, 0, 0, 0);


		if (m_lightIntensity > 0.0f) RenderBillboards(&lightPos, 1, 0.3f, D3DXFROMWINEVECTOR4(1, 1, 1, powf(m_lightIntensity / 3.0f, 0.25f)));
		if (m_showProbes) RenderBillboards((D3DXFROMWINEVECTOR3 *) &m_probePos, m_probeCount, 0.08f, D3DXFROMWINEVECTOR4(0.2f, 0.2f, 1.0f, 1));

		RenderCameraPath();
		RenderGUI();

		if (m_showHelp)
		{
			D3DXFROMWINEVECTOR4 color(1.0f, 0.6f, 0.25f, 1.0f);
			m_mainFont.DrawText(
				"Controls:\n\n"
				"-, + - cycle wall color\n"
				"1, 2 - control ceiling light 1\n"
				"3, 4 - control ceiling light 2\n"
				"5, 6 - control wall lights\n"
				"7, 8 - control dynamic light\n"
				"9 - toggle dynamic light animation\n"
				"PgUp/PgDn - control exposure\n"
				"P - visualize probes",
				-0.65f, 0.0f, 0.1f, 0.1f, HA_LEFT, VA_CENTER, &color);
		}

		IRenderer::GetRendererInstance()->PSSetShaderResources(0, elementsOf(null), null);

	IRenderer::GetRendererInstance()->Present();
}

	// Main effect and some extracted shaders
	stx_Effect*m_lighting, *m_shEffect;
	ShaderID m_lightingPS[LIGHTING_TECH];
	ShaderID m_shadowGS[2];
	VertexFormatID m_lightingIL;

	RasterizerStateID m_cullBack, *m_cullNone;

	SamplerStateID m_baseSS;
	SamplerStateID m_lightMapSS;

	TextureID m_shadowMap;
	TextureID m_shadowMapRTV;
	TextureID m_shadowMapSRV;
	SamplerStateID m_shadowMapSS;

	TextureID m_shadowMapDepth;
	DepthStateID m_shadowMapDepthDSV;

	D3DXFROMWINEVECTOR3 m_probePos[MAX_PROBE_COUNT];

	int m_probeCount;
	int m_currProbe;
	int m_waitFrames;
	int m_currWallMaterial;

	float m_lmValCeiling1;
	float m_lmValCeiling2;
	float m_lmValWall;
	float m_lightIntensity;
	float m_exposure;


	// Render target for the light probes
	TextureID m_probes;
	TextureID m_probesRTV;
	TextureID m_probesChunkRTVs[PROBE_PASSES_PER_FRAME];
	TextureID m_probesSRV;

	// Depth surfaces for the light probes
	TextureID m_probesDepth;
	DepthStateID m_probesDepthDSV;
	DepthStateID m_probesDepthChunkDSVs[PROBE_PASSES_PER_FRAME];



	VertexBufferID m_perFrameCB;
	VertexBufferID m_gsMvpCB;
	VertexBufferID m_gsMvpInvCB;

	TextureID m_shTable[SH_COEFF_VECTORS];
	TextureID m_shTableSRV[SH_COEFF_VECTORS];

	TextureID m_shCube[SH_COEFF_VECTORS];
	TextureID m_shCubeSRV[SH_COEFF_VECTORS];


	// 3D textures containing the resulting SH coefficients
	TextureID m_shCoeffs[3][SH_COEFF_VECTORS];
	TextureID m_shCoeffsRTV[3][SH_COEFF_VECTORS];
	TextureID m_shCoeffsSRV[3][SH_COEFF_VECTORS];
	SamplerStateID m_shCoeffsSS;

	VertexBufferID m_shCoeffVB;
	VertexFormatID m_shCoeffIL;

	TextureID m_texLightGrad;
	TextureID m_texLightGradSRV;

	D3DXFROMWINEMATRIX m_lightView;
	D3DXFROMWINEMATRIX m_lightProj;
	MyModel *m_model;

	bool m_animateLight;
	bool m_showProbes;
	bool m_showHelp;

	int init(const char* aTitle){return 0;}
	void render(){};
};
};
App app;
int init(const char* aTitle)
{
    //app.OnCreateDevice();	
	return 0;
}

void render()
{
   double fTime=0;
   float fElapsedTime=0.0f;
   //app.OnFrameRender(fTime, fElapsedTime); 
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("Global Illumination");
	IInput*    i=STX_Service::GetInputInstance();
	STX_Service::GetAudioInstance()->Init();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		IRenderer::GetRendererInstance()->BeginScene();
		render();
		IRenderer::GetRendererInstance()->EndScene();
		IRenderer::GetRendererInstance()->Present( );
	}
	return 0;
}
