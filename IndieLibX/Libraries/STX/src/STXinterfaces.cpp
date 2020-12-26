/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <STX/STX.h>
#include <STX/STXCPP.h>
#include <STX/STXinterfaces.h>

#if 1
#define STX_FNLN
#define STX_PRINT
#define LOG_FNLN
#define LOG_PRINT
#endif

#if defined(SDL_2_0_5_BUILD) && !defined(SWIG)
SDL_Window* STX_Service::g_SDL_window=0;
SDL_GLContext STX_Service::g_SDL_context;
#endif

void stx_findEdge(stx_OrderedSet<stx_Edge>& edges, const stx_Edge &edge){
	if (edges.goToObject(edge)){
		if (edges.getCurrent().count == 0){
			edges.getCurrent().norm1 = edge.norm0;
		}
		edges.getCurrent().count++;
	} else {
		edges.insert(edge);
	}
}

bool operator == (const stx_Edge &e0, const stx_Edge &e1)
{
	unsigned int min0, max0, min1, max1;

	if (e0.v0 > e0.v1){
		min0 = e0.v0;
		max0 = e0.v1;
	} else {
		min0 = e0.v1;
		max0 = e0.v0;
	}
	if (e1.v0 > e1.v1){
		min1 = e1.v0;
		max1 = e1.v1;
	} else {
		min1 = e1.v1;
		max1 = e1.v0;
	}
	return (max0 == max1 || min0 == min1);
}

bool operator != (const stx_Edge &e0, const stx_Edge &e1)
{
	unsigned int min0, max0, min1, max1;

	if (e0.v0 > e0.v1){
		min0 = e0.v0;
		max0 = e0.v1;
	} else {
		min0 = e0.v1;
		max0 = e0.v0;
	}
	if (e1.v0 > e1.v1){
		min1 = e1.v0;
		max1 = e1.v1;
	} else {
		min1 = e1.v1;
		max1 = e1.v0;
	}
	return (max0 != max1 || min0 != min1);
}

bool operator < (const stx_Edge &e0, const stx_Edge &e1)
{
	unsigned int min0, max0, min1, max1;

	if (e0.v0 > e0.v1){
		min0 = e0.v0;
		max0 = e0.v1;
	} else {
		min0 = e0.v1;
		max0 = e0.v0;
	}
	if (e1.v0 > e1.v1){
		min1 = e1.v0;
		max1 = e1.v1;
	} else {
		min1 = e1.v1;
		max1 = e1.v0;
	}

	if (max0 < max1) return true;
	if (max0 > max1) return false;
	return (min0 < min1);
}

bool constantCompEdgestd(const stx_Edge& s0, const stx_Edge &s1)
{
       if(s0.v1>s1.v1)
	      return true;
       return false;
}

#if 0
void stx_free(void *ptr)
{
#ifdef LINUX
  //__free_hook = old_free_hook;
  switch (mprobe(ptr)) {
  case MCHECK_DISABLED:
    printf("MCHECK_DISABLED\n"); break;
  case MCHECK_OK:
    printf("MCHECK_OK at %p\n",ptr); break;
  case MCHECK_HEAD:
    printf("MCHECK_HEAD at %p\n",ptr); break;
  case MCHECK_TAIL:
    printf("MCHECK_TAIL at %p\n",ptr); break;
  case MCHECK_FREE:
    printf("MCHECK_FREE at %p\n",ptr); break;
  }
#endif
  free(ptr);
  //__free_hook=my_free_hook;
}
void* stx_malloc(size_t size)
{
  //__malloc_hook = old_malloc_hook;
  void* ptr=malloc(size);
#ifdef LINUX
  switch (mprobe(ptr)) {
  case MCHECK_DISABLED:
    printf("MCHECK_DISABLED\n"); break;
  case MCHECK_OK:
    printf("MCHECK_OK at %p\n",ptr); break;
  case MCHECK_HEAD:
    printf("MCHECK_HEAD at %p\n",ptr); break;
  case MCHECK_TAIL:
    printf("MCHECK_TAIL at %p\n",ptr); break;
  case MCHECK_FREE:
    printf("MCHECK_FREE at %p\n",ptr); break;
  }
#endif
  //__malloc_hook=my_malloc_hook;
  return ptr;
}

#include "Reference_Counting.h"
#endif

/*
std::string stx_itoa(const unsigned int& ai)
{
	char buf[32];
	stx_snprintf(buf, 32, "%d", ai);
	return std::string(buf);
}
*/
#if 0
std::vector<std::string> stx_split(std::string const &input);

template<typename Out>
void stx_split(const std::string &s, char delim, Out result);

std::vector<std::string> stx_split(const std::string &s, char delim);

std::string stx_join(std::vector<std::string>& v, char delim);

std::vector<std::string> stx_split(const std::string &s, const std::string &delim);

std::string stx_join(std::vector<std::string>& v, const std::string &delim);

// trim from start
std::string &stx_ltrim(std::string &s);

// trim from end
std::string &stx_rtrim(std::string &s);

// trim from both ends
std::string &stx_trim(std::string &s);
#endif

void stx_GetAngles(float& aAngleX, float& aAngleY, float mSpeedRotation)
{
	float mAngleX_=aAngleX;
	float mAngleY_=aAngleY;
	static float mAngleX=mAngleX_;
	static float mAngleY=mAngleY_;
	float mDelta = timeGetTime() / 1000.0f;
	if((	STX_Service::GetInputInstance()->IsMouseButtonPressed(STX_MBUTTON_LEFT) && 
		STX_Service::GetInputInstance()->IsMouseButtonPressed(STX_MBUTTON_RIGHT)
	) || STX_Service::GetInputInstance()->OnMouseButtonPress(STX_MBUTTON_WHEELDOWN)
	  || STX_Service::GetInputInstance()->IsKeyPressed(STX_KEY_r)		
	)
	{
		mAngleX = mAngleX_;
		mAngleY = mAngleY_;
		mDelta = 0.0f;
	}
	else if(STX_Service::GetInputInstance()->IsMouseButtonPressed(STX_MBUTTON_LEFT))	mAngleX += mSpeedRotation * mDelta;
	else if(STX_Service::GetInputInstance()->IsMouseButtonPressed(STX_MBUTTON_RIGHT))	mAngleY += mSpeedRotation * mDelta;
	aAngleX=(((int)mAngleX)%360);
	aAngleY=(((int)mAngleY)%360);
	#if 0
	LOG_PRINT("aAngleX/360=%d\n", (((int)aAngleX)/360));
	LOG_PRINT("aAngleX%360=%d\n", (((int)aAngleX)%360));
	LOG_PRINT("aAngleY/360=%d\n", (((int)aAngleY)/360));
	LOG_PRINT("aAngleY%360=%d\n", (((int)aAngleY)%360));
	#endif
}

void stx_NormalizeMesh(stx_Mesh& m)
{
	for(unsigned int i=0;i<m.vertices.size();i++)
    		D3DXFROMWINEVec3Normalize(&m.vertices[i].Normal,&m.vertices[i].Position);
}

#if 1
	/* 	a cube inside the mesh ?
		test all cube corners ray-triange collision test
		cast a ray from a cube corner to the opposite cube corners
		inside or surface cube (neigbors list size 6?)
Triangulate the points to make a closed tesselated volume (ask again if you don't know how). Then fire a "ray" from your test point, and count its intersections with the set of triangles. If the number of intersections is odd, the test point is inside the volume.
	*/
#if 1
#define stx_innerProduct(v,q) D3DXFROMWINEVec3Dot(&v, &q)
#define stx_crossProduct(a,b,c) D3DXFROMWINEVec3Cross(&a, &b, &c)
#define stx_vector(a,b,c) \
	(a).x = (b).x - (c).x;	\
	(a).y = (b).y - (c).y;	\
	(a).z = (b).z - (c).z;
#endif
// http://www.lighthouse3d.com/tutorials/maths/ray-triangle-intersection/
bool stx_RayIntersectsTriangle(	const D3DXFROMWINEVECTOR3& p, 
					const D3DXFROMWINEVECTOR3& d,
					const D3DXFROMWINEVECTOR3& v0, 
					const D3DXFROMWINEVECTOR3& v1, 
					const D3DXFROMWINEVECTOR3& v2, float& t
					) {

	D3DXFROMWINEVECTOR3 e1,e2,h,s,q;
	float a,f,u,v;
	stx_vector(e1,v1,v0);
	stx_vector(e2,v2,v0);

	stx_crossProduct(h,d,e2);
	a = stx_innerProduct(e1,h);

	if (a > -0.00001 && a < 0.00001)
		return(0.0);

	f = 1/a;
	stx_vector(s,p,v0);
	u = f * (stx_innerProduct(s,h));

	if (u < 0.0 || u > 1.0)
		return(0.0);

	stx_crossProduct(q,s,e1);
	v = f * stx_innerProduct(d,q);

	if (v < 0.0 || u + v > 1.0)
		return(0.0);

	// at this stage we can compute t to find out where
	// the intersection point is on the line
	t = f * stx_innerProduct(e2,q);

	//t=stx_RayIntersectsTriangleDistance(p, d, v0, v1, v2);

	if (t > 0.00001) // ray intersection
		return(true);

	//else // this means that there is a line intersection
		 // but not a ray intersection
	return (false);
}
#endif

// To find orientation of ordered triplet (p1, p2, p3).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int stx_orientation3(const D3DXFROMWINEVECTOR3& p1, const D3DXFROMWINEVECTOR3& p2, const D3DXFROMWINEVECTOR3& p3)
{
    int val = (p2.y - p1.y) * (p3.x - p2.x) -
              (p2.x - p1.x) * (p3.y - p2.y);
 
    if (val == 0) return 0;  // colinear
 
    return (val > 0)? 1: 2; // clock or counterclock wise
}

int stx_orientation4(const D3DXFROMWINEVECTOR4& p1, const D3DXFROMWINEVECTOR4& p2, const D3DXFROMWINEVECTOR4& p3)
{
    int val = (p2.y - p1.y) * (p3.x - p2.x) -
              (p2.x - p1.x) * (p3.y - p2.y);
 
    if (val == 0) return 0;  // colinear
 
    return (val > 0)? 1: 2; // clock or counterclock wise
}

bool stx_EPSILON2(const D3DXFROMWINEVECTOR2& v1, const D3DXFROMWINEVECTOR2& v2)
{
	float d=0.000001;
	//float d=FLT_EPSILON;
	D3DXFROMWINEVECTOR2 vmin(v1.x-d, v1.y-d);
	D3DXFROMWINEVECTOR2 vmax(v1.x+d, v1.y+d);
	return ((v2>vmin) && (v2<vmax));
}

bool stx_EPSILON3(const D3DXFROMWINEVECTOR3& v1, const D3DXFROMWINEVECTOR3& v2)
{
	float d=0.000001;
	//float d=FLT_EPSILON;
	D3DXFROMWINEVECTOR3 vmin(v1.x-d, v1.y-d, v1.z-d);
	D3DXFROMWINEVECTOR3 vmax(v1.x+d, v1.y+d, v1.z+d);
	return ((v2>vmin) && (v2<vmax));
}

void stx_stdvector3(std::vector<D3DXFROMWINEVECTOR3>& vertices, std::vector<D3DXFROMWINEVECTOR3>& normals, std::vector<D3DXFROMWINEVECTOR2>& texcoords, std::vector<__WORD__>& indices)
{
	std::vector<D3DXFROMWINEVECTOR3> vertices2;
	std::vector<D3DXFROMWINEVECTOR3> normals2;
	std::vector<D3DXFROMWINEVECTOR2> texcoords2;
	indices.clear();
	for(unsigned int i=0;i<vertices.size();i++)
	{
		bool found=false;
		for(unsigned int j=0;j<vertices2.size();j++)
		{
			if(stx_EPSILON3(vertices[i], vertices2[j]))
			{
				indices.push_back(j);
				found=true;
				break;
			}
		}
		if(!found)
		{
			vertices2.push_back(vertices[i]);
			normals2.push_back(normals[i]);
			texcoords2.push_back(texcoords[i]);
			indices.push_back(vertices2.size()-1);
		}
	}
	vertices=vertices2;
	normals=normals2;
	texcoords=texcoords2;
}

D3DXFROMWINEVECTOR2 stx_CalculatePositionalSphericalMapping(D3DXFROMWINEVECTOR3& a)
{
/*
// determine extents
D3DXVECTOR3 _min,_max;
D3DXComputeBoundingBox(pVerts,numVerts,FVF_VERTEX,&_min,&_max);

// calculate center
D3DXVECTOR3 vCent;
vCent=(_max+_min)*0.5f;
*/
	D3DXFROMWINEVECTOR3 vCent(0.0f, 0.0f, 0.0f);
    // calculate normalized offset from center
    D3DXFROMWINEVECTOR3 v=a-vCent;
    D3DXFROMWINEVec3Normalize(&v,&v);

	D3DXFROMWINEVECTOR2 aTex;

    // calculate texture coordinates
    aTex.x=asinf(v.x)/D3DXFROMWINE_PI+0.5f;
    aTex.y=-asinf(v.y)/D3DXFROMWINE_PI-0.5f;

	return aTex;
}

void stx_readXFile(const char* aFileName, std::vector<stx_Mesh>& aMeshes)
{
    std::string line;
    unsigned int N = 0;
    std::ifstream text (aFileName);
	if (text.is_open())
	{
		while (text.good())
		{
			getline(text,line);
			N++;
			std::vector<std::string> v=stx_split(line);	
			if((v.size()==3)&&v[0]=="xof" && v[1]=="0303txt" && v[2]=="0032")
				continue;//LOG_PRINT("%d: xof 0303txt 0032\n", N);
			if((v.size()==2)&&v[0]=="Frame" && v[1]=="{")
				continue;//LOG_PRINT("%d: Frame {\n", N);
			if((v.size()==2)&&v[0]=="FrameTransformMatrix" && v[1]=="{")
			{
				//LOG_PRINT("%d: FrameTransformMatrix {\n", N);
				getline(text,line);
				N++;
				std::vector<std::string> v=stx_split(line, ',');
				//LOG_PRINT("v.size()=%d\n", v.size());
				if(1)
				for(unsigned int i=0;i<v.size();i++)
				{
					LOG_PRINT("%d/%d:%f\n", i,v.size(),atof(v[i].c_str()));
				}
			}
			if((v.size()==2)&&v[0]=="Mesh" && v[1]=="{")
			{
				//LOG_PRINT("Mesh {\n");
				getline(text,line);
				N=atoi(line.c_str());
				//LOG_PRINT("%d\n", N);
				stx_Mesh m;
				aMeshes.push_back(m);
				for(unsigned int i=0;i<N;i++)
				{
					getline(text,line);
					stx_ReplaceAll(line, ";,", ";");
					stx_ReplaceAll(line, ";;", ";");
					if(line[line.length()-1])
						line=line.substr(0,line.length()-2);
					std::vector<std::string> v_=stx_split(line, ';');
					float f1=atof(v_[0].c_str());
					float f2=atof(v_[1].c_str());
					float f3=atof(v_[2].c_str());
					D3DXFROMWINEVECTOR3 v3(f1, f2, f3);
					aMeshes[aMeshes.size()-1].vertices.push_back(v3);
				}
			}
			if((v.size()==2)&&v[0]=="MeshNormals" && v[1]=="{")
			{
				//LOG_PRINT("MeshNormals {\n");
				getline(text,line);
				N=atoi(line.c_str());
				//LOG_PRINT("%d\n", N);
				for(unsigned int i=0;i<N;i++)
				{
					getline(text,line);
					stx_ReplaceAll(line, ";,", ";");
					stx_ReplaceAll(line, ";;", ";");
					if(line[line.length()-1])
						line=line.substr(0,line.length()-2);
					std::vector<std::string> v_=stx_split(line, ';');
					float f1=atof(v_[0].c_str());
					float f2=atof(v_[1].c_str());
					float f3=atof(v_[2].c_str());
					D3DXFROMWINEVECTOR3 v3(f1, f2, f3);
					aMeshes[aMeshes.size()-1].vertices[i].Normal=v3;
				}
			}
			if((v.size()==2)&&v[0]=="MeshTextureCoords" && v[1]=="{")
			{
				//LOG_PRINT("MeshTextureCoords {\n");
				getline(text,line);
				N=atoi(line.c_str());
				//LOG_PRINT("%d\n", N);
				for(unsigned int i=0;i<N;i++)
				{
					getline(text,line);
					stx_ReplaceAll(line, ";,", ";");
					stx_ReplaceAll(line, ";;", ";");
					if(line[line.length()-1])
						line=line.substr(0,line.length()-2);
					std::vector<std::string> v_=stx_split(line, ';');
					float f1=atof(v_[0].c_str());
					float f2=atof(v_[1].c_str());
					D3DXFROMWINEVECTOR2 v2(f1, f2);
					aMeshes[aMeshes.size()-1].vertices[i].Tex=v2;
				}
			}
			if((v.size()==2)&&v[0]=="MeshMaterialList" && v[1]=="{")
			{
				getline(text,line);
				N=atoi(line.c_str());
				getline(text,line);
				N=atoi(line.c_str());
				for(unsigned int i=0;i<N;i++)
				{
					getline(text,line);
					stx_ReplaceAll(line, ";,", ";");
					stx_ReplaceAll(line, ";;", ";");
					if(line[line.length()-1])
						line=line.substr(0,line.length()-2);
					float f1=atof(line.c_str());
					aMeshes[aMeshes.size()-1].indices.push_back(f1);
				}
			}
			/*	
        MeshMaterialList {
	     21;
	     7690;
	     13,
	     13;

	84610 - 92299
			*/
		}
		text.close();
	 }
}

void stx_writeOBJFile(const char* aFileName, std::vector<stx_Mesh>& aMeshes)
{
	std::string fileName=stx_convertpath(aFileName);
	FILE * pFile = fopen (fileName.c_str(),"w");
	unsigned int i=0;
	for(unsigned int m=0;m<aMeshes.size();m++)
	{
	for(unsigned int j=0;j<aMeshes[m].indices.size()-1;j++)
	{
	for(i=0;i<aMeshes[m].vertices.size()-1;i++)
	{
		fprintf (pFile, "o %9.6f %9.6f %9.6f\n", 
		aMeshes[m].vertices[aMeshes[m].indices[j]].Position.x, aMeshes[m].vertices[aMeshes[m].indices[j]].Position.y, aMeshes[m].vertices[aMeshes[m].indices[j]].Position.z );
		fprintf (pFile, "on %9.6f %9.6f %9.6f\n", 
		aMeshes[m].vertices[aMeshes[m].indices[j]].Normal.x, aMeshes[m].vertices[aMeshes[m].indices[j]].Normal.y, aMeshes[m].vertices[aMeshes[m].indices[j]].Normal.z );
		fprintf (pFile, "ot %9.6f %9.6f\n", 
		aMeshes[m].vertices[aMeshes[m].indices[j]].Tex.x, aMeshes[m].vertices[aMeshes[m].indices[j]].Tex.y );
	}
	}}
   	fclose (pFile);
}

void stx_readOBJFile(const char* aFileName, std::vector<stx_Mesh>& aMeshes)
{
	std::string fileName=stx_convertpath(aFileName);
	FILE * pFile = fopen (fileName.c_str(),"r");
	D3DXFROMWINEVECTOR3 v3;
	D3DXFROMWINEVECTOR2 v2;
	char buf[260];
	stx_Mesh mesh;
	aMeshes.push_back(mesh);
	while(fgets(buf, sizeof(buf), pFile))
	{
		std::vector<std::string> v=stx_split(buf);
		if(v.size()==4 && v[0]=="o")
		{
			v3.x=atof(v[1].c_str());
			v3.y=atof(v[2].c_str());
			v3.z=atof(v[3].c_str());
			aMeshes[0].vertices.push_back(v3);
			aMeshes[0].indices.push_back(aMeshes[0].vertices.size()-1);
		}
		else if(v.size()==4 && v[0]=="on")
		{
			v3.x=atof(v[1].c_str());
			v3.y=atof(v[2].c_str());
			v3.z=atof(v[3].c_str());
			aMeshes[0].vertices[aMeshes[0].vertices.size()-1].Normal=v3;
		}
		else if(v.size()==3 && v[0]=="ot")
		{
			v2.x=atof(v[1].c_str());
			v2.y=atof(v[2].c_str());
			aMeshes[0].vertices[aMeshes[0].vertices.size()-1].Tex=v2;
		}
	}
   	fclose (pFile);
}

void stx_writeXFile(	const char* aFileName, 
			std::vector<stx_VertexPositionNormalTexture>& vertices, 
			std::vector<__WORD__>& indices, 
			std::vector<std::string>& textureNames, 
			std::vector<__WORD__>& faces)
{
	#if 1
	std::string fileName=stx_convertpath(aFileName);

	LOG_FNLN;
{
	LOG_PRINT("vertices.size()=%d\n", vertices.size());
	LOG_PRINT("indices.size()=%d\n", indices.size());
	LOG_PRINT("textureNames.size()=%d\n", textureNames.size());
if(faces.size())
	LOG_PRINT("faces.size()=%d\n", faces.size());
}

	//if(stx_fileExists(fileName.c_str())) return;
	FILE * pFile = fopen (fileName.c_str(),"w");

	fprintf (pFile, "xof 0303txt 0032\n");
	fprintf (pFile, "\n");
	fprintf (pFile, "Frame {\n");
	fprintf (pFile, "    FrameTransformMatrix {\n");
	fprintf (pFile, "1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;\n");
	fprintf (pFile, "}\n");
{
	if(vertices.size())
	{
	fprintf (pFile, "Mesh {\n");
	unsigned int i=0;
	fprintf (pFile, "%d;\n", vertices.size());
	for(i=0;i<vertices.size()-1;i++)
	{
		fprintf (pFile, "%9.6f; %9.6f; %9.6f;,\n", 
		vertices[i].Position.x, vertices[i].Position.y, vertices[i].Position.z );
	}
	i=vertices.size()-1;
	fprintf (pFile, "%9.6f; %9.6f; %9.6f;;\n", 
		vertices[i].Position.x, vertices[i].Position.y, vertices[i].Position.z );

	if(indices.size())
	{
	fprintf (pFile, "%d;\n", indices.size()/3);
	for(i=0;i<indices.size()/3-1;i++)
	{
		fprintf (pFile, "3;%d, %d, %d;,\n", 
		indices[3*i+0], indices[3*i+1], indices[3*i+2]);
	}
	i=indices.size()/3-1;
	fprintf (pFile, "3;%d, %d, %d;;\n", indices[3*i+0], indices[3*i+1], indices[3*i+2] );
	}

	if(vertices.size())
	{

	fprintf (pFile, "MeshNormals {\n");
	fprintf (pFile, "%d;\n", vertices.size());
	for(i=0;i<vertices.size()-1;i++)
	{
		fprintf (pFile, "%9.6f; %9.6f; %9.6f;,\n", 
		vertices[i].Normal.x, vertices[i].Normal.y, vertices[i].Normal.z );
	}
	i=vertices.size()-1;
	fprintf (pFile, "%9.6f; %9.6f; %9.6f;;\n", 
		vertices[i].Normal.x, vertices[i].Normal.y, vertices[i].Normal.z );

	

	if(indices.size())
	{
	fprintf (pFile, "%d;\n", indices.size()/3);
	for(i=0;i<indices.size()/3-1;i++)
	{
		fprintf (pFile, "3;%d, %d, %d;,\n", indices[3*i+0], indices[3*i+1], indices[3*i+2] );
	}
	i=indices.size()/3-1;
	fprintf (pFile, "3;%d, %d, %d;;\n", indices[3*i+0], indices[3*i+1], indices[3*i+2] );
	}
	fprintf (pFile, "}\n");
	}

	if(faces.size())
	{
	//std::string textureFilename= /* stx_convertpath */ (aTextureFilename);
	
	fprintf (pFile, "MeshTextureCoords {\n");
	fprintf (pFile, "%d;\n", vertices.size());
	for(i=0;i<vertices.size()-1;i++)
	{
		fprintf (pFile, "%9.6f; %9.6f;,\n", 
		vertices[i].Tex.x, vertices[i].Tex.y);
	}
	i=vertices.size()-1;
	fprintf (pFile, "%9.6f; %9.6f;;\n", vertices[i].Tex.x, vertices[i].Tex.y );
	fprintf (pFile, "        }\n");
if(faces.size())
{
	fprintf (pFile, "        MeshMaterialList {\n");
	fprintf (pFile, "	     %d;\n", textureNames.size());
	//fprintf (pFile, "	     %d;\n", mm);
	fprintf (pFile, "	     %d;\n", faces.size());

//LOG_PRINT("rindices[0].size()=%d\n", rindices[0].size());
//LOG_PRINT("faces.size()=%d\n", faces.size());

#if 1
	for(i=0;i<faces.size()-1;i++)
	{
	fprintf (pFile, "	     %d,\n", faces.at(i)); 
	}
	fprintf (pFile, "	     %d;\n", faces.at(faces.size()-1));
#endif
for(unsigned int i=0;i<textureNames.size();i++)
{
	std::string tn=textureNames[i];
	D3DXFROMWINEVECTOR3 col; // ???
	fprintf (pFile, "            Material {\n");
	fprintf (pFile, "                %9.6f;%9.6f;%9.6f;1.000000;;\n", col.x, col.y, col.z);
	fprintf (pFile, "                0.000000;\n");
	fprintf (pFile, "                0.000000;0.000000;0.000000;;\n");
	fprintf (pFile, "                0.000000;0.000000;0.000000;;\n");
	fprintf (pFile, "                TextureFilename {\n");


	fprintf (pFile, "                   \"%s\";\n", tn.c_str());
	fprintf (pFile, "        		}\n");
	fprintf (pFile, "        	}\n");
}
	fprintf (pFile, "        }\n");
}
}
 	fprintf (pFile, "   }\n");
}}
	fprintf (pFile, "}\n");

   fclose (pFile);
	#endif
}

void stx_writeXFile(const char* aFileName, std::vector<stx_Mesh>& aMeshes)
{
	#if 1
	std::string fileName=stx_convertpath(aFileName);

	LOG_FNLN;
if(aMeshes.size())
for(unsigned int zz=0;zz<aMeshes.size();zz++)
{
	LOG_PRINT("aMeshes[%d].vertices.size()=%d\n", zz, aMeshes[zz].vertices.size());
	LOG_PRINT("aMeshes[%d].indices.size()=%d\n", zz, aMeshes[zz].indices.size());
	LOG_PRINT("aMeshes[%d].textureNames.size()=%d\n", zz, aMeshes[zz].textureNames.size());
if(aMeshes[zz].faces.size())
	LOG_PRINT("aMeshes[%d].faces.size()=%d\n", zz, aMeshes[zz].faces.size());
}

	//if(stx_fileExists(fileName.c_str())) return;
	FILE * pFile = fopen (fileName.c_str(),"w");

	fprintf (pFile, "xof 0303txt 0032\n");
	fprintf (pFile, "\n");
	fprintf (pFile, "Frame {\n");
	fprintf (pFile, "    FrameTransformMatrix {\n");
	fprintf (pFile, "1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;\n");
	fprintf (pFile, "}\n");
if(aMeshes.size())
for(unsigned int zz=0;zz<aMeshes.size();zz++)
{
	if(aMeshes[zz].vertices.size())
	{
	fprintf (pFile, "Mesh {\n");
	unsigned int i=0;
	fprintf (pFile, "%d;\n", aMeshes[zz].vertices.size());
	for(i=0;i<aMeshes[zz].vertices.size()-1;i++)
	{
		fprintf (pFile, "%9.6f; %9.6f; %9.6f;,\n", 
		aMeshes[zz].vertices[i].Position.x, aMeshes[zz].vertices[i].Position.y, aMeshes[zz].vertices[i].Position.z );
	}
	i=aMeshes[zz].vertices.size()-1;
	fprintf (pFile, "%9.6f; %9.6f; %9.6f;;\n", 
		aMeshes[zz].vertices[i].Position.x, aMeshes[zz].vertices[i].Position.y, aMeshes[zz].vertices[i].Position.z );

	if(aMeshes[zz].indices.size())
	{
	fprintf (pFile, "%d;\n", aMeshes[zz].indices.size()/3);
	for(i=0;i<aMeshes[zz].indices.size()/3-1;i++)
	{
		fprintf (pFile, "3;%d, %d, %d;,\n", 
		aMeshes[zz].indices[3*i+0], aMeshes[zz].indices[3*i+1], aMeshes[zz].indices[3*i+2]);
	}
	i=aMeshes[zz].indices.size()/3-1;
	fprintf (pFile, "3;%d, %d, %d;;\n", aMeshes[zz].indices[3*i+0], aMeshes[zz].indices[3*i+1], aMeshes[zz].indices[3*i+2] );
	}

	if(aMeshes[zz].vertices.size())
	{

	fprintf (pFile, "MeshNormals {\n");
	fprintf (pFile, "%d;\n", aMeshes[zz].vertices.size());
	for(i=0;i<aMeshes[zz].vertices.size()-1;i++)
	{
		fprintf (pFile, "%9.6f; %9.6f; %9.6f;,\n", 
		aMeshes[zz].vertices[i].Normal.x, aMeshes[zz].vertices[i].Normal.y, aMeshes[zz].vertices[i].Normal.z );
	}
	i=aMeshes[zz].vertices.size()-1;
	fprintf (pFile, "%9.6f; %9.6f; %9.6f;;\n", 
		aMeshes[zz].vertices[i].Normal.x, aMeshes[zz].vertices[i].Normal.y, aMeshes[zz].vertices[i].Normal.z );

	

	if(aMeshes[zz].indices.size())
	{
	fprintf (pFile, "%d;\n", aMeshes[zz].indices.size()/3);
	for(i=0;i<aMeshes[zz].indices.size()/3-1;i++)
	{
		fprintf (pFile, "3;%d, %d, %d;,\n", aMeshes[zz].indices[3*i+0], aMeshes[zz].indices[3*i+1], aMeshes[zz].indices[3*i+2] );
	}
	i=aMeshes[zz].indices.size()/3-1;
	fprintf (pFile, "3;%d, %d, %d;;\n", aMeshes[zz].indices[3*i+0], aMeshes[zz].indices[3*i+1], aMeshes[zz].indices[3*i+2] );
	}
	fprintf (pFile, "}\n");
	}

	if(aMeshes[zz].faces.size())
	{
	//std::string textureFilename= /* stx_convertpath */ (aTextureFilename);
	
	fprintf (pFile, "MeshTextureCoords {\n");
	fprintf (pFile, "%d;\n", aMeshes[zz].vertices.size());
	for(i=0;i<aMeshes[zz].vertices.size()-1;i++)
	{
		fprintf (pFile, "%9.6f; %9.6f;,\n", 
		aMeshes[zz].vertices[i].Tex.x, aMeshes[zz].vertices[i].Tex.y);
	}
	i=aMeshes[zz].vertices.size()-1;
	fprintf (pFile, "%9.6f; %9.6f;;\n", aMeshes[zz].vertices[i].Tex.x, aMeshes[zz].vertices[i].Tex.y );
	fprintf (pFile, "        }\n");
if(aMeshes[zz].faces.size())
{
	fprintf (pFile, "        MeshMaterialList {\n");
	fprintf (pFile, "	     %d;\n", aMeshes[zz].textureNames.size());
	//fprintf (pFile, "	     %d;\n", mm);
	fprintf (pFile, "	     %d;\n", aMeshes[zz].faces.size());

//LOG_PRINT("aMeshes[zz].rindices[0].size()=%d\n", aMeshes[zz].rindices[0].size());
//LOG_PRINT("aMeshes[zz].faces.size()=%d\n", aMeshes[zz].faces.size());

#if 1
	for(i=0;i<aMeshes[zz].faces.size()-1;i++)
	{
	fprintf (pFile, "	     %d,\n", aMeshes[zz].faces.at(i)); 
	}
	fprintf (pFile, "	     %d;\n", aMeshes[zz].faces.at(aMeshes[zz].faces.size()-1));
#endif
for(unsigned int i=0;i<aMeshes[zz].textureNames.size();i++)
{
	std::string tn=aMeshes[zz].textureNames[i];
	D3DXFROMWINEVECTOR3 col; // ???
	fprintf (pFile, "            Material {\n");
	fprintf (pFile, "                %9.6f;%9.6f;%9.6f;1.000000;;\n", col.x, col.y, col.z);
	fprintf (pFile, "                0.000000;\n");
	fprintf (pFile, "                0.000000;0.000000;0.000000;;\n");
	fprintf (pFile, "                0.000000;0.000000;0.000000;;\n");
	fprintf (pFile, "                TextureFilename {\n");


	fprintf (pFile, "                   \"%s\";\n", tn.c_str());
	fprintf (pFile, "        		}\n");
	fprintf (pFile, "        	}\n");
}
	fprintf (pFile, "        }\n");
}
}
 	fprintf (pFile, "   }\n");
}}
	fprintf (pFile, "}\n");

   fclose (pFile);
	#endif
}

D3DXFROMWINEVECTOR3 stx_Vec3Transform(const D3DXFROMWINEMATRIX& aM, const D3DXFROMWINEVECTOR3& av)
{
	D3DXFROMWINEVECTOR3 ov3;
	#if 1
	D3DXFROMWINEVec3TransformCoord(&ov3, &av, &aM);
	#else
	D3DXFROMWINEVECTOR4 ov;
	D3DXFROMWINEVec3Transform(&ov, &av, &aM);
	ov3.x=ov.x;
	ov3.y=ov.y;
	ov3.z=ov.z;
	#endif
	return ov3;
}

D3DXFROMWINEMATRIX stx_GetPivotMatrix(const D3DXFROMWINEVECTOR3& aR, const D3DXFROMWINEVECTOR3& aT)
{
	D3DXFROMWINEMATRIX mRotation, mTranslation;
	D3DXFROMWINEVECTOR3 R=D3DXFROMWINEToRadian(aR);
	D3DXFROMWINEMatrixRotationYawPitchRoll(&mRotation, R.x, R.y, R.z);
	D3DXFROMWINEMatrixTranslation(&mTranslation, aT.x, aT.y, aT.z);
	D3DXFROMWINEMATRIX w=mRotation*mTranslation;
	return w;
}

D3DXFROMWINEMATRIX stx_GetPivotMatrix3(const D3DXFROMWINEVECTOR3& aR, const D3DXFROMWINEVECTOR3& aS, const D3DXFROMWINEVECTOR3& aT)
{
	D3DXFROMWINEMATRIX mRotation, mScaling, mTranslation;
	#if 0
	D3DXFROMWINEMatrixRotationYawPitchRoll(&mRotation, 0.0f, 0.0f, 0.0f);
	D3DXFROMWINEMatrixScaling(&mScaling, 1.0f/3.0f, 1.0f/3.0f, 1.0f/3.0f);
	D3DXFROMWINEMatrixTranslation(&mTranslation, 2.0f/3.0f, 2.0f/3.0f, 2.0f/3.0f);
	#else
	D3DXFROMWINEVECTOR3 R=D3DXFROMWINEToRadian(aR);
	D3DXFROMWINEMatrixRotationYawPitchRoll(&mRotation, R.x, R.y, R.z);
	D3DXFROMWINEMatrixScaling(&mScaling, aS.x, aS.y, aS.z);
	//D3DXFROMWINEMatrixIdentity(&mScaling);
	//D3DXFROMWINEMatrixScaling(&mScaling, aS.x, aS.y, 1.0f);
	D3DXFROMWINEMatrixTranslation(&mTranslation, aT.x, aT.y, aT.z);
	#endif
	D3DXFROMWINEMATRIX w=mRotation*mScaling*mTranslation;
	return w;
}

D3DXFROMWINEMATRIX stx_GetPivotMatrix2(const D3DXFROMWINEVECTOR3& aR, const D3DXFROMWINEVECTOR3& mPivotPos)
{
	D3DXFROMWINEMATRIX aM;
	D3DXFROMWINEMATRIX mPivotInv, mPivot, mRotation, mTranslation;
	D3DXFROMWINEVECTOR3 R=D3DXFROMWINEToRadian(aR);
	D3DXFROMWINEMatrixRotationYawPitchRoll(&mRotation, R.x, R.y, R.z);
	D3DXFROMWINEMatrixTranslation(&mPivot, mPivotPos.x, mPivotPos.y, mPivotPos.z);
	D3DXFROMWINEMatrixTranslation(&mPivotInv, -mPivotPos.x, -mPivotPos.y, -mPivotPos.z);
	aM = mPivotInv*mRotation*mPivot;
	return aM;
}

#define COLONWITHQUOTES ':'
#define COLONWITHDOUBLEQUOTES ":"

#define CLOSINGPARENTHESISWITHQUOTES ')'
#define CLOSINGPARENTHESISWITHDOUBLEQUOTES ")"

std::string stx_ApplyRulesS(const std::vector<stx_Rule>& arules, const std::string& asymbol)
{
	std::string rule=asymbol;
	stx_ReplaceAll(rule, "(", ":(:");
	stx_ReplaceAll(rule, ")", ":):");
	stx_ReplaceAll(rule, "[", ":[:");
	stx_ReplaceAll(rule, "]", ":]:");
	stx_ReplaceAll(rule, ",", ":");
	stx_ReplaceAll(rule, "\'", "");
	stx_ReplaceAll(rule, "%", "");
	return rule;
}
std::vector<std::string> stx_ApplyRulesE(const std::vector<stx_Rule>& arules, const std::string& asymbol)
{
	std::string rule=stx_ApplyRulesS(arules, asymbol);
	std::vector<std::string> v=stx_split(rule, ':');
	std::vector<std::string> v2;
	for(unsigned int i=0;i<v.size();i++)
	{
		v2.push_back(stx_trim(v[i]));
		//LOG_PRINT("v[%d]=%s\n", i, v2[i].c_str());
	}
	return v2;
}
void stx_DumpRules(std::vector<std::string>& av) 
{
	std::string str=stx_join(av, ':');
	LOG_PRINT("stx_DumpRules:%s\n", str.c_str());
}
std::string stx_GetRule(const std::vector<stx_Rule>& arules, const std::string& arulename)
{
	std::string rulename=arulename;
	rulename=stx_trim(rulename);
	std::size_t found = rulename.find(COLONWITHDOUBLEQUOTES);
  	if(found==std::string::npos)
	{
		for(unsigned int i=0;i<arules.size();i++)
		{
			stx_Rule rule=arules[i];
			if(rulename==rule.m_RuleName)
				return rule.m_RuleBody;
		}
	}
	return "";
}
int stx_GetRulePosition(const std::vector<stx_Rule>& arules, const std::string& arulename)
{
	std::string rulename=arulename;
	rulename=stx_trim(rulename);
	std::size_t found = rulename.find(COLONWITHDOUBLEQUOTES);
  	if(found==std::string::npos)
	{
		for(unsigned int i=0;i<arules.size();i++)
		{
			stx_Rule rule=arules[i];
			if(rulename==rule.m_RuleName)
				return i;
		}
	}
	return -1;
}
std::string stx_ApplyRules(std::vector<stx_Rule>& arules, std::string& aaxiom)
{
	std::string axiom=stx_trim(aaxiom);
	std::size_t found = axiom.find(COLONWITHDOUBLEQUOTES);
  	if(found==std::string::npos)
	{
		std::string rule=stx_GetRule(arules, axiom);
		if(!(rule==""))
		{
			std::string str=stx_ApplyRules(arules, rule);
			return str;
		}
		else
			return axiom;
	}
	else
	{
		std::vector<std::string> v=stx_split(axiom, COLONWITHQUOTES);
		std::string str=stx_ApplyRules(arules, stx_trim(v[0]));
		for(unsigned int j=1;j<v.size();j++)
		{
			str+=COLONWITHDOUBLEQUOTES+stx_ApplyRules(arules, stx_trim(v[j]));
		}
		return str;
	}
	return "";
}
std::string stx_GetRuleStr(const std::vector<stx_Rule>& arules, const std::string& axiom)
{
	//LOG_PRINT("axiom=%s\n", axiom.c_str());
	std::vector<std::string> v=stx_ApplyRulesE(arules, axiom);

	//if((v[0]=="I") && (v[1]=="("))
	//	v.erase(v.begin()+0);

	std::string str=stx_join(v, ':');
	//stx_ApplyRules3(arules, str);
	stx_ReplaceAll(str, "%", "");
	stx_ReplaceAll(str, "{", ":{:");
	stx_ReplaceAll(str, "}", ":}:");
	stx_ReplaceAll(str, "(", ":(:");
	stx_ReplaceAll(str, ")", ":):");
	stx_ReplaceAll(str, "[", ":[:");
	stx_ReplaceAll(str, "]", ":]:");
	stx_ReplaceAll(str, "::", ":");
	stx_ReplaceAll(str, ",", ":");
	stx_ReplaceAll(str, "\'", "");
	//stx_ReplaceAll(str, ":I:", ":");

	v=stx_split(str, ':');
	for(unsigned int i=0;i<v.size();i++)
		v[i]=stx_trim(v[i]);

	str=stx_join(v, ':');
	//stx_ReplaceAll(str, "::", ":");
	//LOG_PRINT("str=%s\n", str.c_str());
	return str;
}

std::vector<std::string> stx_GetRuleVec(const std::vector<stx_Rule>& arules, const std::string& axiom)
{
	std::vector<std::string> v_;
	std::vector<std::string> v=stx_split(axiom);
	if(v.size()==1)
	{
		std::string rule=stx_GetRule(arules, axiom);
		std::string str=stx_GetRuleStr(arules, rule);
		if(str.length())
		{
			v_=stx_split(str, ':');
			if(!v_.size())
				v_.push_back(str);
		}
	}
	return v_;
}

//include "Noise.h"
#define STX_NOISESCALE 0.1482f

#if 1
float clamp(const float v, const float c0, const float c1);
D3DXFROMWINEVECTOR2  clamp(const D3DXFROMWINEVECTOR2 &v, const float c0, const float c1);
D3DXFROMWINEVECTOR2  clamp(const D3DXFROMWINEVECTOR2 &v, const D3DXFROMWINEVECTOR2 &c0, const D3DXFROMWINEVECTOR2 &c1);
D3DXFROMWINEVECTOR3  clamp(const D3DXFROMWINEVECTOR3 &v, const float c0, const float c1);
D3DXFROMWINEVECTOR3  clamp(const D3DXFROMWINEVECTOR3 &v, const D3DXFROMWINEVECTOR3 &c0, const D3DXFROMWINEVECTOR3 &c1);
D3DXFROMWINEVECTOR4  clamp(const D3DXFROMWINEVECTOR4 &v, const float c0, const float c1);
D3DXFROMWINEVECTOR4  clamp(const D3DXFROMWINEVECTOR4 &v, const D3DXFROMWINEVECTOR4 &c0, const D3DXFROMWINEVECTOR4 &c1);
//#define clamp(x, lo, hi) max(min(x, hi), lo)
#define saturate(x) clamp(x, 0, 1)
#define lerp_(t, a, b) (a + t * (b - a))
#endif
Image3 stx_Create2DNoiseTexture(const unsigned int xNSIZE, const unsigned int yNSIZE)
{
	// Create a noise texture made specially for fire and store a cached copy to disc
	Image3 img;
	if (1){//!img.loadDDS("FireNoise.dds") || ((img.getFormat() == FORMAT_ATI1N) != supportsATI1N)){
		ubyte *nDest = img.create(FORMAT_I8, xNSIZE, yNSIZE, 1, 1);
		//for (int z = 0; z < NSIZE; z++){
			for (int y = 0; y < yNSIZE; y++){
				float wobble = tileableNoise1(y * 0.152f, 0.152f * yNSIZE);
				for (int x = 0; x < xNSIZE; x++){
					//float n = tileableTurbulence3(STX_NOISESCALE * x + wobble, STX_NOISESCALE * y, STX_NOISESCALE * z, NSIZE * STX_NOISESCALE, NSIZE * STX_NOISESCALE, NSIZE * STX_NOISESCALE, 16);
					float n=tileableTurbulence2(STX_NOISESCALE * x + wobble, STX_NOISESCALE * y, xNSIZE * STX_NOISESCALE, yNSIZE * STX_NOISESCALE, 16);

					float fx = saturate(float(x) / (xNSIZE - 1) * 1.1f - 0.05f + 0.2f * wobble);
					fx *= 4 * (1 - fx);

					n = fx * (n * 0.5f + 0.5f);

					*nDest++ = (ubyte) (255 * n);
				}
			}
		//}
		//img.compress(supportsATI1N? FORMAT_ATI1N : FORMAT_DXT1);
		//img.saveDDS("FireNoise.dds");
	}
	return img;
}
Image3 stx_Create3DNoiseTexture(const unsigned int xNSIZE, const unsigned int yNSIZE, const unsigned int zNSIZE)
{
	// Create a noise texture made specially for fire and store a cached copy to disc
	Image3 img;
	if (1){//!img.loadDDS("FireNoise.dds") || ((img.getFormat() == FORMAT_ATI1N) != supportsATI1N)){
		ubyte *nDest = img.create(FORMAT_I8, xNSIZE, yNSIZE, zNSIZE, 1);
		for (int z = 0; z < zNSIZE; z++){
			for (int y = 0; y < yNSIZE; y++){
				float wobble = tileableNoise1(y * 0.152f, 0.152f * yNSIZE);
				for (int x = 0; x < xNSIZE; x++){
					float n = tileableTurbulence3(STX_NOISESCALE * x + wobble, STX_NOISESCALE * y, STX_NOISESCALE * z, xNSIZE * STX_NOISESCALE, yNSIZE * STX_NOISESCALE, zNSIZE * STX_NOISESCALE, 16);
					//float n=tileableTurbulence2(STX_NOISESCALE * x + wobble, STX_NOISESCALE * y, NSIZE * STX_NOISESCALE, NSIZE * STX_NOISESCALE, 16);

					float fx = saturate(float(x) / (xNSIZE - 1) * 1.1f - 0.05f + 0.2f * wobble);
					fx *= 4 * (1 - fx);

					n = fx * (n * 0.5f + 0.5f);

					*nDest++ = (ubyte) (255 * n);
				}
			}
		}
		//img.compress(supportsATI1N? FORMAT_ATI1N : FORMAT_DXT1);
		//img.saveDDS("FireNoise.dds");
	}
	return img;
}
/*
TextureID Create3DNoiseTexture2(const float xNSIZE, const float yNSIZE, const float zNSIZE)
{
	Image3 img;
	ubyte *p=img.create(FORMAT_I8, xNSIZE,yNSIZE,zNSIZE, 1);
        stx_srand(250673);
	FilteredNoiseEx noise(16,16,16);
	noise.createNoiseTexture3D(xNSIZE,yNSIZE,zNSIZE, 4, 1, FilteredNoise:: NOISE, p);
	return IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->GetlinearAniso());	
}
TextureID CreateJitterTexture(const float xNSIZE=NSIZE, const float yNSIZE=NSIZE)
{
 // Create the shader resource view for jittering
	return Create2DNoiseTexture(xNSIZE, yNSIZE, data);
}*/

D3DXFROMWINEVECTOR3 stx_vec4tovec3(D3DXFROMWINEVECTOR4& a)
{
	D3DXFROMWINEVECTOR3 r;
	r.x=a.x;
	r.y=a.y;
	r.z=a.z;
	return r;
}

void stx_DumpRules2(const std::vector<stx_Rule>& arules) 
{
	for(unsigned int i=0;i<arules.size();i++)
	{
		stx_Rule rule=arules[i];
		LOG_PRINT("stx_DumpRules2:%s=%s\n", rule.m_RuleName.c_str(), rule.m_RuleBody.c_str());
	}
}
std::string stx_asciireplace(std::string& arulebody)
{
#if 1
	for(unsigned int c=0;c<256;c++)
	{
		int asciiValue = c;
		char character = char(asciiValue);
		char buf[6];
		std::string str_;
		str_ += character;
		stx_snprintf(buf, 6, "&#%c;", character);
		stx_ReplaceAll(arulebody, buf, str_);
	}
	//LOG_PRINT("arulebody=%s\n", arulebody.c_str());
#endif
	return arulebody;
}
std::vector<stx_Rule> stx_ParseRuleFile(const std::string& aFileName)
{
//LOG_FNLN;
	std::vector<stx_Rule> v;
	std::string fileName=stx_convertpath(aFileName);
	//LOG_PRINT("fileName=%s\n", fileName.c_str());
	std::string contents;
	STX_Service::LoadTxtFile(fileName, contents);
//LOG_FNLN;

	#if 0
	unsigned int c=0;
	std::size_t foundColon = contents.find(";");
	if(foundColon!=std::string::npos)
	while(foundColon!=std::string::npos)
	{
		foundColon = contents.find(";",foundColon+1,1);
		c++;
	}
	//LOG_PRINT("found Colon Count=%d\n", c);
	#endif
	contents=stx_asciireplace(contents);
	//stx_ReplaceAll(contents, "&#32;", " ");
	stx_ReplaceAll(contents, "\"", "");

	std::vector<std::string> rulev=stx_split(contents, ';');
//LOG_FNLN;
	for(unsigned int i=0;i<rulev.size();i++)
	{
//LOG_FNLN;
		std::string rule=stx_trim(rulev[i]);
		std::vector<std::string> ruleV=stx_split(rule, '=');
	//LOG_PRINT("ruleV.size()=%d\n", ruleV.size());
		if(ruleV.size()==2)
		{
//LOG_FNLN;
			std::string ruleName=stx_trim(ruleV[0]);
			std::string ruleBody=stx_trim(ruleV[1]);
			std::vector<std::string> v_=stx_split(ruleBody, '\\');
			ruleBody="";
			for(unsigned int j=0;j<v_.size();j++)
			{
//LOG_FNLN;
				ruleBody.append(stx_trim(v_[j]));
				//ruleBody.append(stx_asciireplace(stx_trim(v_[j])));
			}
			stx_Rule r(ruleName, ruleBody);
			v.push_back(r);
//LOG_FNLN;
		}
//LOG_FNLN;
	}
//LOG_FNLN;
	stx_DumpRules2(v);
//LOG_FNLN;
	return v;
}

void stx_AddColoredQuad(std::vector<D3DXFROMWINEVECTOR3>& aVertices3, std::vector<__WORD__>& aIndices1, D3DXFROMWINEMATRIX& aM)
{
	D3DXFROMWINEVECTOR2 asize(1.0f, 1.0f);
	{D3DXFROMWINEVECTOR3 v(-asize.x, -asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);aVertices3.push_back(vo);}
	{D3DXFROMWINEVECTOR3 v( asize.x, -asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);aVertices3.push_back(vo);}
	{D3DXFROMWINEVECTOR3 v(-asize.x,  asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);aVertices3.push_back(vo);}
	{D3DXFROMWINEVECTOR3 v( asize.x,  asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);aVertices3.push_back(vo);}
		aIndices1.push_back(aVertices3.size()-4);
		aIndices1.push_back(aVertices3.size()-3);
		aIndices1.push_back(aVertices3.size()-1);
		aIndices1.push_back(aVertices3.size()-1);
		aIndices1.push_back(aVertices3.size()-2);
		aIndices1.push_back(aVertices3.size()-4);
	#if 0
		LOG_PRINT("vo.x=%f\n", aVertices3[aIndices1[aVertices3.size()-6]].x);
		LOG_PRINT("vo.y=%f\n", aVertices3[aIndices1[aVertices3.size()-5]].y);
		LOG_PRINT("vo.z=%f\n", aVertices3[aIndices1[aVertices3.size()-4]].z);
		LOG_PRINT("vo.x=%f\n", aVertices3[aIndices1[aVertices3.size()-3]].x);
		LOG_PRINT("vo.y=%f\n", aVertices3[aIndices1[aVertices3.size()-2]].y);
		LOG_PRINT("vo.z=%f\n", aVertices3[aIndices1[aVertices3.size()-1]].z);
	#endif
}

void stx_AddTexturedQuad(std::vector<D3DXFROMWINEVECTOR3>& aVertices3, std::vector<D3DXFROMWINEVECTOR2>& aTexcoords2, std::vector<__WORD__>& aIndices1, std::vector<__WORD__>& aIndices2, D3DXFROMWINEMATRIX& aM, unsigned int vtsize)
{
	D3DXFROMWINEVECTOR2 asize(1.0f, 1.0f);
	{D3DXFROMWINEVECTOR3 v(-asize.x, -asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(0.0f, 0.0f);aVertices3.push_back(vo);aTexcoords2.push_back(tc);}
	{D3DXFROMWINEVECTOR3 v( asize.x, -asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(1.0f, 0.0f);aVertices3.push_back(vo);aTexcoords2.push_back(tc);}
	aIndices2.push_back(vtsize-1);
	{D3DXFROMWINEVECTOR3 v(-asize.x,  asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(0.0f, 1.0f);aVertices3.push_back(vo);aTexcoords2.push_back(tc);}
	{D3DXFROMWINEVECTOR3 v( asize.x,  asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(1.0f, 1.0f);aVertices3.push_back(vo);aTexcoords2.push_back(tc);}
	aIndices2.push_back(vtsize-1);
		aIndices1.push_back(aVertices3.size()-4);
		aIndices1.push_back(aVertices3.size()-3);
		aIndices1.push_back(aVertices3.size()-1);
		aIndices1.push_back(aVertices3.size()-1);
		aIndices1.push_back(aVertices3.size()-2);
		aIndices1.push_back(aVertices3.size()-4);
	#if 0
		LOG_PRINT("vo.x=%f\n", aVertices3[aIndices1[aVertices3.size()-6]].x);
		LOG_PRINT("vo.y=%f\n", aVertices3[aIndices1[aVertices3.size()-5]].y);
		LOG_PRINT("vo.z=%f\n", aVertices3[aIndices1[aVertices3.size()-4]].z);
		LOG_PRINT("vo.x=%f\n", aVertices3[aIndices1[aVertices3.size()-3]].x);
		LOG_PRINT("vo.y=%f\n", aVertices3[aIndices1[aVertices3.size()-2]].y);
		LOG_PRINT("vo.z=%f\n", aVertices3[aIndices1[aVertices3.size()-1]].z);
	#endif
}

#if 1
void stx_AddColoredCube(std::vector<D3DXFROMWINEVECTOR3>& aVertices3, std::vector<D3DXFROMWINEVECTOR3>& aNormals3, std::vector<__WORD__>& aIndices1, D3DXFROMWINEVECTOR3 aPosition, D3DXFROMWINEVECTOR3 aSize, D3DXFROMWINEVECTOR3 aAngles)
{
	D3DXFROMWINEMATRIX  w;
	D3DXFROMWINEVECTOR3 vR, vP;

	vR=D3DXFROMWINEVECTOR3(0*90.0f, 0.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(0.0f, 0.0f, -1.0f);
	w=stx_GetPivotMatrix(vR, vP);
	stx_AddColoredQuad(aVertices3, aIndices1, w);

	vR=D3DXFROMWINEVECTOR3(1*90.0f, 0.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(1.0f, 0.0f, 0.0f);
	w=stx_GetPivotMatrix(vR, vP);
	stx_AddColoredQuad(aVertices3, aIndices1, w);

	vR=D3DXFROMWINEVECTOR3(0*90.0f, 0.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 1.0f);
	w=stx_GetPivotMatrix(vR, vP);
	stx_AddColoredQuad(aVertices3, aIndices1, w);

	vR=D3DXFROMWINEVECTOR3(1*90.0f, 0.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(-1.0f, 0.0f, 0.0f);
	w=stx_GetPivotMatrix(vR, vP);
	stx_AddColoredQuad(aVertices3, aIndices1, w);

	vR=D3DXFROMWINEVECTOR3(0.0f, 1*90.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(0.0f, 1.0f, 0.0f);
	w=stx_GetPivotMatrix(vR, vP);
	stx_AddColoredQuad(aVertices3, aIndices1, w);

	vR=D3DXFROMWINEVECTOR3(0.0f, -1*90.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(0.0f, -1.0f, 0.0f);
	w=stx_GetPivotMatrix(vR, vP);
	stx_AddColoredQuad(aVertices3, aIndices1, w);

	D3DXFROMWINEMATRIX W,R,S,T;
	D3DXFROMWINEMatrixRotationYawPitchRoll(&R, aAngles.x, aAngles.y, aAngles.z);
	D3DXFROMWINEMatrixScaling(&S, aSize.x, aSize.y, aSize.z);
	D3DXFROMWINEMatrixTranslation(&T, aPosition.x, aPosition.y, aPosition.z);

	#if 0
		LOG_PRINT("aPosition.x=%f\n", aPosition.x);
		LOG_PRINT("aPosition.y=%f\n", aPosition.y);
		LOG_PRINT("aPosition.z=%f\n", aPosition.z);
		LOG_PRINT("aSize.x=%f\n", aSize.x);
		LOG_PRINT("aSize.y=%f\n", aSize.y);
		LOG_PRINT("aSize.z=%f\n", aSize.z);
	#endif

	W=R*S*T;
	const unsigned int N=6*4;
	D3DXFROMWINEVECTOR3 vN;
	for(unsigned int i=0;i<N;i++)
	{
		D3DXFROMWINEVec3TransformCoord(&aVertices3[aVertices3.size()-N+i], &aVertices3[aVertices3.size()-N+i], &W);
		D3DXFROMWINEVec3Normalize(&vN, &aVertices3[aVertices3.size()-N+i]);
		aNormals3.push_back(vN);
	}

}
void stx_AddTexturedCube(std::vector<D3DXFROMWINEVECTOR3>& aVertices3, std::vector<D3DXFROMWINEVECTOR3>& aNormals3, std::vector<D3DXFROMWINEVECTOR2>& aTexcoords2, std::vector<__WORD__>& aIndices1, std::vector<__WORD__>& aIndices2, unsigned int vtsize, D3DXFROMWINEVECTOR3 aPosition, D3DXFROMWINEVECTOR3 aSize, D3DXFROMWINEVECTOR3 aAngles)
{
	D3DXFROMWINEMATRIX  w;
	D3DXFROMWINEVECTOR3 vR, vP;

	vR=D3DXFROMWINEVECTOR3(0*90.0f, 0.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(0.0f, 0.0f, -1.0f);
	w=stx_GetPivotMatrix(vR, vP);
	stx_AddTexturedQuad(aVertices3, aTexcoords2, aIndices1, aIndices2, w, vtsize);

	vR=D3DXFROMWINEVECTOR3(1*90.0f, 0.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(1.0f, 0.0f, 0.0f);
	w=stx_GetPivotMatrix(vR, vP);
	stx_AddTexturedQuad(aVertices3, aTexcoords2, aIndices1, aIndices2, w, vtsize);

	vR=D3DXFROMWINEVECTOR3(0*90.0f, 0.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 1.0f);
	w=stx_GetPivotMatrix(vR, vP);
	stx_AddTexturedQuad(aVertices3, aTexcoords2, aIndices1, aIndices2, w, vtsize);

	vR=D3DXFROMWINEVECTOR3(1*90.0f, 0.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(-1.0f, 0.0f, 0.0f);
	w=stx_GetPivotMatrix(vR, vP);
	stx_AddTexturedQuad(aVertices3, aTexcoords2, aIndices1, aIndices2, w, vtsize);

	vR=D3DXFROMWINEVECTOR3(0.0f, 1*90.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(0.0f, 1.0f, 0.0f);
	w=stx_GetPivotMatrix(vR, vP);
	stx_AddTexturedQuad(aVertices3, aTexcoords2, aIndices1, aIndices2, w, vtsize);

	vR=D3DXFROMWINEVECTOR3(0.0f, -1*90.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(0.0f, -1.0f, 0.0f);
	w=stx_GetPivotMatrix(vR, vP);
	stx_AddTexturedQuad(aVertices3, aTexcoords2, aIndices1, aIndices2, w, vtsize);

	D3DXFROMWINEMATRIX W,R,S,T;
	D3DXFROMWINEMatrixRotationYawPitchRoll(&R, aAngles.x, aAngles.y, aAngles.z);
	D3DXFROMWINEMatrixScaling(&S, aSize.x, aSize.y, aSize.z);
	D3DXFROMWINEMatrixTranslation(&T, aPosition.x, aPosition.y, aPosition.z);

	#if 0
		LOG_PRINT("aPosition.x=%f\n", aPosition.x);
		LOG_PRINT("aPosition.y=%f\n", aPosition.y);
		LOG_PRINT("aPosition.z=%f\n", aPosition.z);
		LOG_PRINT("aSize.x=%f\n", aSize.x);
		LOG_PRINT("aSize.y=%f\n", aSize.y);
		LOG_PRINT("aSize.z=%f\n", aSize.z);
	#endif

	W=R*S*T;
	const unsigned int N=6*4;
	D3DXFROMWINEVECTOR3 vN;
	for(unsigned int i=0;i<N;i++)
	{
		D3DXFROMWINEVec3TransformCoord(&aVertices3[aVertices3.size()-N+i], &aVertices3[aVertices3.size()-N+i], &W);
		D3DXFROMWINEVec3Normalize(&vN, &aVertices3[aVertices3.size()-N+i]);
		aNormals3.push_back(vN);
	}

}
#endif
void stx_AddQuadP3TC2(std::vector<VertexP3TC2>& aVertices, std::vector<__WORD__>& aIndices, D3DXFROMWINEMATRIX& aM)
{
	D3DXFROMWINEVECTOR2 asize(1.0f, 1.0f);
	{D3DXFROMWINEVECTOR3 v(-asize.x, -asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(0.0f, 0.0f);VertexP3TC2 vertexP3TC2(vo, tc);aVertices.push_back(vertexP3TC2);}
	{D3DXFROMWINEVECTOR3 v( asize.x, -asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(1.0f, 0.0f);VertexP3TC2 vertexP3TC2(vo, tc);aVertices.push_back(vertexP3TC2);}
	{D3DXFROMWINEVECTOR3 v(-asize.x,  asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(0.0f, 1.0f);VertexP3TC2 vertexP3TC2(vo, tc);aVertices.push_back(vertexP3TC2);}
	{D3DXFROMWINEVECTOR3 v( asize.x,  asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(1.0f, 1.0f);VertexP3TC2 vertexP3TC2(vo, tc);aVertices.push_back(vertexP3TC2);}
	
		aIndices.push_back(aVertices.size()-4);
		aIndices.push_back(aVertices.size()-3);
		aIndices.push_back(aVertices.size()-1);

		aIndices.push_back(aVertices.size()-1);
		aIndices.push_back(aVertices.size()-2);
		aIndices.push_back(aVertices.size()-4);

	#if 0
		LOG_PRINT("vo.x=%f\n", aVertices[aIndices[aVertices.size()-6]].m_Position.x);
		LOG_PRINT("vo.y=%f\n", aVertices[aIndices[aVertices.size()-5]].m_Position.y);
		LOG_PRINT("vo.z=%f\n", aVertices[aIndices[aVertices.size()-4]].m_Position.z);

		LOG_PRINT("vo.x=%f\n", aVertices[aIndices[aVertices.size()-3]].m_Position.x);
		LOG_PRINT("vo.y=%f\n", aVertices[aIndices[aVertices.size()-2]].m_Position.y);
		LOG_PRINT("vo.z=%f\n", aVertices[aIndices[aVertices.size()-1]].m_Position.z);
	#endif
}

void stx_AddCubeP3TC2(std::vector<VertexP3TC2>& aVertices, std::vector<__WORD__>& aIndices, D3DXFROMWINEVECTOR3& aPosition, D3DXFROMWINEVECTOR3& aSize, D3DXFROMWINEVECTOR3& aAngles)
{
	D3DXFROMWINEMATRIX  w;
	D3DXFROMWINEVECTOR3 vR, vP;

	vR=D3DXFROMWINEVECTOR3(0*90.0f, 0.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(0.0f, 0.0f, -1.0f);
	w=stx_GetPivotMatrix3(vR, aSize, vP);
	stx_AddQuadP3TC2(aVertices, aIndices, w);

	vR=D3DXFROMWINEVECTOR3(1*90.0f, 0.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(1.0f, 0.0f, 0.0f);
	w=stx_GetPivotMatrix3(vR, aSize, vP);
	stx_AddQuadP3TC2(aVertices, aIndices, w);

	vR=D3DXFROMWINEVECTOR3(0*90.0f, 0.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 1.0f);
	w=stx_GetPivotMatrix3(vR, aSize, vP);
	stx_AddQuadP3TC2(aVertices, aIndices, w);

	vR=D3DXFROMWINEVECTOR3(1*90.0f, 0.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(-1.0f, 0.0f, 0.0f);
	stx_AddQuadP3TC2(aVertices, aIndices, w);

	vR=D3DXFROMWINEVECTOR3(0.0f, 1*90.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(0.0f, 1.0f, 0.0f);
	w=stx_GetPivotMatrix3(vR, aSize, vP);
	stx_AddQuadP3TC2(aVertices, aIndices, w);

	vR=D3DXFROMWINEVECTOR3(0.0f, -1*90.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(0.0f, -1.0f, 0.0f);
	w=stx_GetPivotMatrix3(vR, aSize, vP);
	stx_AddQuadP3TC2(aVertices, aIndices, w);

	D3DXFROMWINEMATRIX W,R,S,T;
	D3DXFROMWINEMatrixRotationYawPitchRoll(&R, aAngles.x, aAngles.y, aAngles.z);
	D3DXFROMWINEMatrixScaling(&S, aSize.x, aSize.y, aSize.z);
	D3DXFROMWINEMatrixTranslation(&T, aPosition.x, aPosition.y, aPosition.z);

	#if 1
		LOG_PRINT("aPosition.x=%f\n", aPosition.x);
		LOG_PRINT("aPosition.y=%f\n", aPosition.y);
		LOG_PRINT("aPosition.z=%f\n", aPosition.z);
		LOG_PRINT("aSize.x=%f\n", aSize.x);
		LOG_PRINT("aSize.y=%f\n", aSize.y);
		LOG_PRINT("aSize.z=%f\n", aSize.z);
	#endif

	W=R*S*T;
	for(unsigned int i=0;i<24;i++)
	{
		D3DXFROMWINEVec3TransformCoord(&aVertices[aVertices.size()-24+i].m_Position, &aVertices[aVertices.size()-24+i].m_Position, &W);
	}
}

void stx_Dump4x4f(D3DXFROMWINEMATRIX& am, const char* description)
{
	if(description)
		LOG_PRINT("%s:\n", description);
	LOG_PRINT("m[%d][%d]=%f\n", 0, 0, am.m[0][0]);
	LOG_PRINT("m[%d][%d]=%f\n", 0, 1, am.m[0][1]);
	LOG_PRINT("m[%d][%d]=%f\n", 0, 2, am.m[0][2]);
	LOG_PRINT("m[%d][%d]=%f\n", 0, 3, am.m[0][3]);
	LOG_PRINT("m[%d][%d]=%f\n", 1, 0, am.m[1][0]);
	LOG_PRINT("m[%d][%d]=%f\n", 1, 1, am.m[1][1]);
	LOG_PRINT("m[%d][%d]=%f\n", 1, 2, am.m[1][2]);
	LOG_PRINT("m[%d][%d]=%f\n", 1, 3, am.m[1][3]);
	LOG_PRINT("m[%d][%d]=%f\n", 2, 0, am.m[2][0]);
	LOG_PRINT("m[%d][%d]=%f\n", 2, 1, am.m[2][1]);
	LOG_PRINT("m[%d][%d]=%f\n", 2, 2, am.m[2][2]);
	LOG_PRINT("m[%d][%d]=%f\n", 2, 3, am.m[2][3]);
	LOG_PRINT("m[%d][%d]=%f\n", 3, 0, am.m[3][0]);
	LOG_PRINT("m[%d][%d]=%f\n", 3, 1, am.m[3][1]);
	LOG_PRINT("m[%d][%d]=%f\n", 3, 2, am.m[3][2]);
	LOG_PRINT("m[%d][%d]=%f\n", 3, 3, am.m[3][3]);
}
void stx_Dump1i(int ai, const char* description)
{
	if(description)
		LOG_PRINT("%s:\n", description);
	LOG_PRINT("ai=%x\n", ai);
}
void stx_Dump1f(float af, const char* description)
{
	if(description)
		LOG_PRINT("%s:\n", description);
	LOG_PRINT("af=%f\n", af);
}
void stx_Dump2f(D3DXFROMWINEVECTOR2& av, const char* description)
{
	if(description)
		LOG_PRINT("%s:\n", description);
	LOG_PRINT("av.x=%f\n", av.x);
	LOG_PRINT("av.y=%f\n", av.y);
}
void stx_Dump3f(D3DXFROMWINEVECTOR3& av, const char* description)
{
	if(description)
		LOG_PRINT("%s:\n", description);
	LOG_PRINT("av.x=%f\n", av.x);
	LOG_PRINT("av.y=%f\n", av.y);
	LOG_PRINT("av.z=%f\n", av.z);
}
void stx_Dump4f(D3DXFROMWINEVECTOR4& av, const char* description)
{
	if(description)
		LOG_PRINT("%s:\n", description);
	LOG_PRINT("av.x=%f\n", av.x);
	LOG_PRINT("av.y=%f\n", av.y);
	LOG_PRINT("av.z=%f\n", av.z);
	LOG_PRINT("av.w=%f\n", av.w);
}

D3DXFROMWINEVECTOR4 stx_GetTextureAtlasPosition(__DWORD__ Type, unsigned int aN, unsigned int iatlassize)
{
	unsigned int N=sqrtf(aN);
	if(N*N<aN)
		N++;
	D3DXFROMWINEVECTOR4 r;
	unsigned int isubtexturesize=iatlassize/N;
	r.x = int(Type / (iatlassize/isubtexturesize));
	r.y = int(Type-r.x*(iatlassize/isubtexturesize));
	r.z = 1.0f*iatlassize/isubtexturesize;
	r.w = 1.0f*iatlassize/isubtexturesize;
	return r;
}

int stx_AddTextureToAtlas(	Image3& atlas,Image3& img, unsigned int iatlassize, 
					unsigned int isubtexturesize, 
					unsigned int atlasTextureCount)//,float gamma)
{
	atlas.Dump();
	img.Dump();
	if(img.getPixels()==0)
		return -1;
	
	img.uncompressImage();
	FORMAT format = img.getFormat();
	if(format!=FORMAT_RGBA8) img.convert(FORMAT_RGBA8);
	format = img.getFormat();

//if(img.format==FORMAT_RGB8) img.addChannel();
/*
	if (isPlainFormat(format) && getChannelCount(format) == 3)
	{
		img.convert(FORMAT_RGBA8);
	}  */
atlas.uncompressImage();
if((atlas.getWidth()!=iatlassize)||(atlas.getHeight()!=iatlassize))
{
  //LOG_PRINT("Atlas size should be (%d x %d)\n",iatlassize,iatlassize);
	  return atlasTextureCount;
}
if((img.getWidth()!=isubtexturesize)||(img.getHeight()!=isubtexturesize))
	  img.resize(isubtexturesize, isubtexturesize, 1);

//applyGamma( img, gamma );

ubyte *fDest = atlas.getPixels();
ubyte *fSrc = img.getPixels();
int ix = atlasTextureCount / (iatlassize/isubtexturesize);
int iy = atlasTextureCount-ix*(iatlassize/isubtexturesize);
	LOG_PRINT("atlasTextureCount=%d\n", atlasTextureCount);
	LOG_PRINT("ix=%d\n", ix);
	LOG_PRINT("iy=%d\n", iy);
for (int y = 0; y < isubtexturesize; y++){
	//if(y > (isubtexturesize-1)) return ++atlasTextureCount;
__DWORD__ src1 = iatlassize * 4 * y + ix*isubtexturesize * 4+iy*iatlassize * 4 *isubtexturesize;
stx_memcpy(fDest + src1, fSrc  + isubtexturesize * 4 * y, isubtexturesize * 4);

}
return ++atlasTextureCount;
}
Image3 stx_CreateAtlas(const char* filenames[], unsigned int iatlassize, bool useMipMaps)
{
	Image3 atlas;
	std::vector<std::string> av;
	for(unsigned int i=0;i<elementsOf(filenames);i++)
		av.push_back(std::string(filenames[i]));
    	stx_CreateAtlas(atlas, av, iatlassize, useMipMaps);
	return atlas;
}
Image3 stx_CreateAtlas(std::vector<std::string>& av, unsigned int iatlassize, bool useMipMaps)
{
	Image3 atlas;
    	stx_CreateAtlas(atlas, av, iatlassize, useMipMaps);
	return atlas;
}
int stx_CreateAtlas(Image3& atlas, const char* filenames[], unsigned int iatlassize, bool useMipMaps)
{
	std::vector<std::string> av;
	for(unsigned int i=0;i<elementsOf(filenames);i++)
		av.push_back(std::string(filenames[i]));
    return stx_CreateAtlas(atlas, av, iatlassize, useMipMaps);
}
int stx_CreateAtlas(Image3& atlas, std::vector<std::string>& av, unsigned int iatlassize, bool useMipMaps)
{
	unsigned int N=sqrtf(av.size());
	if(N*N<av.size())
		N++;
	unsigned int atlasTextureCount=0;
	unsigned int isubtexturesize=4096/N;

	ubyte *fDest = atlas.create(FORMAT_RGBA8, iatlassize, iatlassize, 1, 1);
	for(unsigned int i=0;i<av.size();i++)
	{
 		Image3 img;
		img.create(FORMAT_RGBA8, isubtexturesize, isubtexturesize, 1, 1);
		if(av[i]=="")
			img=stx_Create2DNoiseTexture(64, 64);
		else
		{
			std::string f2=stx_convertpath(av[i]);
			LOG_FNLN;
			LOG_PRINT("f2.c_str()=%s\n", f2.c_str()); 
			img.loadImageLibImage(f2.c_str(), false, useMipMaps);
		}
		stx_AddTextureToAtlas(atlas, img, iatlassize, isubtexturesize, atlasTextureCount++);
	}
	return atlasTextureCount;
}

bool stx_is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

D3DXFROMWINEMATRIX stx_ScaleAsset(
	D3DXFROMWINEVECTOR3 aiVecs0,
	D3DXFROMWINEVECTOR3 aiVecs1)
{
	D3DXFROMWINEVECTOR3 vDelta = aiVecs1-aiVecs0;
	D3DXFROMWINEVECTOR3 vHalf =  aiVecs0 + (vDelta / 2.0f);
	float fScale = 10.0f / vDelta.getLength();

	D3DXFROMWINEMATRIX m_mW1(
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		-vHalf.x,-vHalf.y,-vHalf.z,1.0f);
		D3DXFROMWINEMATRIX m_mW2(
		fScale,0.0f,0.0f,0.0f,
		0.0f,fScale,0.0f,0.0f,
		0.0f,0.0f,fScale,0.0f,
		0.0f,0.0f,0.0f,1.0f);
	D3DXFROMWINEMATRIX m_mW=m_mW1*m_mW2;
	return m_mW;
}
bool stx_IsNumber(const std::string& s)
{
#if 0
	if(s.length()<1)
    		return false;
	std::istringstream iss(s);
	int num = 0;
	if (!(iss >> num).fail())
    		return true;
    	return false;
#else
   if(s.empty() || std::isspace(s[0]) || std::isalpha(s[0])) return false ;
   char * p ;
   strtod(s.c_str(), &p) ;
   return (*p == 0) ;
#endif
}
std::vector<size_t> stx_SearchListInt(const std::vector<unsigned int>& theArray, int findFor)
{
    std::vector<size_t> indexes;
    for (size_t i = 0; i < theArray.size(); ++i){
        if (theArray[i] == findFor){
            indexes.push_back(i + 1);
        }
    }
    return indexes;
}
std::vector<unsigned int> stx_SearchList(const std::vector<std::string>& theArray, std::string findFor)
{
    std::vector<unsigned int> indexes;
    for (size_t i = 0; i < theArray.size(); ++i){
        if (theArray[i] == findFor){
            indexes.push_back(i + 1);
        }
    }
    return indexes;
}
#if 0
void stx_readXFile_(const char* aFileName, std::vector<stx_Mesh>& aMeshes)
{	
	const char* symbols[] =
	{
	"AnimationSet","Animation","AnimationKey","Frame", "FrameTransformMatrix",	"Mesh", "MeshNormals", "MeshTextureCoords", "MeshMaterialList", "Material", "TextureFilename"
	};
	{
		std::string fn=stx_convertpath(aFileName);
		LOG_PRINT("%s\n", fn.c_str());
		std::ifstream t(fn);
		std::string txt((std::istreambuf_iterator<char>(t)),
                			std::istreambuf_iterator<char>());

		stx_ReplaceAll(txt, ";;;"," %endblock% ");
		stx_ReplaceAll(txt, ";;"," %endblock% ");
		stx_ReplaceAll(txt, ";,","");
		stx_ReplaceAll(txt, ";"," ");
		stx_ReplaceAll(txt, ","," ");
		stx_ReplaceAll(txt, "TextureFilename","TextureFilename");
		stx_ReplaceAll(txt, "}"," ");
		std::vector<std::string> v=stx_split(txt);
		LOG_PRINT("v.size()=%d\n", v.size());
		static char buf[1024] = {0};
		for(unsigned int y=0;y<elementsOf(symbols);y++)
		{
			buf[0] = '\0';
			std::string symbol=symbols[y];
			LOG_PRINT("%s\n", symbol.c_str());
			if(true)
			{
				if((symbol=="Mesh") || (symbol=="MeshNormals") || (symbol=="MeshTextureCoords") || (symbol=="MeshMaterialList"))
				{
					unsigned int indice_=0;
					for(unsigned int z=0;z<v.size();z++)
					{
						if(v[z]==symbol)
							indice_=z;
						else
							continue;
						//LOG_PRINT("indices_=%d\n", indice_);
						if ((indice_>0) && (v[indice_-1]=="template"))
							continue;
						if ((symbol=="Material") || (symbol=="FrameTransformMatrix"))
							continue;
						else if ((v[indice_]=="Mesh") && (v[indice_+2]=="{"))
						{
							unsigned int c = 3*atoi(v[indice_+3].c_str());
							stx_snprintf(buf, 1024, "%s: %s VN: %s IN: %s\n", symbol.c_str(), v[indice_+1].c_str(), v[indice_+3].c_str(), v[indice_+5+c].c_str());
						}
						else if (v[indice_]=="MeshTextureCoords")
						{
							stx_snprintf(buf, 1024, "%s: VN: %s\n", symbol.c_str(), v[indice_+2].c_str());
						}
						else if (symbol=="MeshMaterialList")
						{
							stx_snprintf(buf, 1024, "%s: MN: %s FN: %s\n", symbol.c_str(), v[indice_+2].c_str(), v[indice_+3].c_str());
							LOG_PRINT("%s\n", buf);
							stx_snprintf(buf, 1024, "%s: MN: %s FN: %s %s\n", symbol.c_str(), v[indice_+2].c_str(), v[indice_+3].c_str(), v[indice_+4+atoi(v[indice_+3].c_str())+1].c_str());
						}
						else
						{
							unsigned int c = 3*atoi(v[indice_+2].c_str());
							stx_snprintf(buf, 1024, "%s: VN: %s IN: %s\n", symbol.c_str(), v[indice_+2].c_str(), v[indice_+4+c].c_str());
						}
						LOG_PRINT("%s\n", buf);
					}
				}
				std::vector<std::string> texs;
				if(symbol=="TextureFilename")
				{
					unsigned int indice_=0;
					for(unsigned int z=0;z<v.size();z++)
					{
						if(v[z]==symbol)
							indice_=z;
						else
							continue;
						if ((indice_) && (v[indice_-1]=="template"))
							continue;
						std::string tx=v[indice_+2];
						unsigned int p=v[indice_+2].length()-1;
						if((p>1) && (v[indice_+2].size()>1))
							tx=v[indice_+2].substr(1,p);
						if ( std::find(texs.begin(), texs.end(), tx) == texs.end() )
							texs.push_back(tx);
					}
				}
				if(texs.size())
				{
					LOG_PRINT("texs.size()=%d\n", texs.size());
					if(texs.size()>1)
					{
						stx_snprintf(buf, 1024, "TextureFiles N: %d\n", texs.size());
					}
					else if (texs[0].length())
					{
						stx_snprintf(buf, 1024, "TextureFile: %s\n", texs[0].c_str());
					}
					LOG_PRINT("%s\n", buf);
				}
			}
		}
	}
}
#endif
#if 0
void printFrequency(std::string& aFilename) 
	fstream file; 
	std::string word;
	std::vector<std::string> v;
    	file.open(aFilename.c_str());
    	while (file >> word) 
    	{ 
        	v.push_back(word);
    	}
	LOG_PRINT("v.size()=%d\n", v.size());
} 
#endif
void XFileParser()
{
	const char* filenames[] =
{
"/Lightning/chain_target.X",
"/AdvAniCh12/chopper.X",
"/AdvAniCh12/Shadow.X",
"/AdvAniCh12/Rotor.X",
"/skeleton/skeleton.x",
"/ProceduralBuilding2.x",
"/Sparkles/cat.x",
"/dx9sdk/sphere.x",
"/dx9sdk/cube.x",
"/dx9sdk/sphere2.x",
"/dx9sdk/teapot.x",
"/dx9sdk/tiger.x",
"/dx9sdk/tiny.x",
"/20-Explosions/Ch20p2_ExplosionCluster/Ch20p2_object.x",
"/20-Explosions/Ch20p3_ComplexExplosion/Ch20p3_object.x",
"/20-Explosions/Ch20p1_SimpleExplosion/Ch20p1_object.x",
"/Billboard/seafloor.x",
"/Billboard/skybox2.x",
"/DXJune2010/Dwarf/Dwarf.x",
"/DXJune2010/Dwarf/DwarfWithEffectInstance.x",
"/DXJune2010/trees/skybox2.x",
"/15_Ocean_Water/skybox.x",
"/trex_dx/dino_videogame.x",
"/trex_dx/dino_videogame2.x",
"/GLEssentials/Assets/demon.x",
"/AdvAniCh12/backdrop.x",
"/assimp--1.0.412-sdk/dwarf.x",
"/assimp--1.0.412-sdk/test.x",
//"/Lightning/scene.X",
//"/Smoke/Gargoyle/gargoyle.x",
//"/Orcs/Bonfire_wOrcs.x",
};
	std::vector<stx_Mesh> aMeshes;
	for(unsigned int i=0;i<elementsOf(filenames);i++)
	{
		stx_readXFile(filenames[i]
				//"/ProceduralBuilding2.x"
				, aMeshes);
	}
}
#undef _SDKMESH_
#ifdef _SDKMESH_
#include <sdkmesh/obj.h>
#include <sdkmesh/sdkmesh.h>
#endif
int stx_GetSDKMESHFile(	const char* aFileName, std::vector<stx_Mesh>& aMeshes)
{
#ifdef _SDKMESH_
	std::ifstream input(aFileName, std::ios::binary | std::ios::in);
	if (!input.good())
	{
		LOG_PRINT("Input file does not exist\n");
		input.close();
		return -1;
	}
	// traverse to count size
	// a bit silly but an accurate way
	std::streampos fileSize = input.tellg();
	input.seekg(0, std::ios::end);
	fileSize = input.tellg() - fileSize;
	input.seekg(0, std::ios::beg);

	Sdkmesh sdkmesh;
	sdkmesh.CreateFromFile_9(input, fileSize);
	sdkmesh.DoCheck();

	LOG_PRINT("%d\n", input.tellg());
	LOG_PRINT("%d\n", fileSize);

	// start to dump into .obj output
	//WriteOBJ_9(sdkmesh, output_obj);

	/// we assume each mesh is bounded to one index buffer and one vertex buffer
	/// this could be hazardous, if you find any exception or have robust sulotion, please PR or email me

	if ((sdkmesh.GetSdkmeshHeader().NumMeshes != sdkmesh.GetSdkmeshHeader().NumVertexBuffers) ? 1 :
		(sdkmesh.GetSdkmeshHeader().NumMeshes != sdkmesh.GetSdkmeshHeader().NumIndexBuffers ? 1 :
			sdkmesh.GetSdkmeshHeader().NumVertexBuffers != sdkmesh.GetSdkmeshHeader().NumIndexBuffers))
	{
		LOG_PRINT("Currently doesn't support sdkmesh with different mesh number/vertex buffer/index buffer\n");
		return -1;
	}

	const std::vector<std::vector<PosNormalTexTan_9>> vertexBuffers = sdkmesh.GetSdkmeshVertexBuffer_9();
	std::vector<std::vector<int>> indexBuffers = sdkmesh.GetSdkmeshIndexBuffer();

	// traverse to encode
	unsigned cnt = 0;
	uint32_t prevVertexNumTotal = 0;
	for (; cnt < sdkmesh.GetSdkmeshHeader().NumMeshes; cnt++)
	{
		stx_Mesh mesh_;
		const Sdkmesh::SdkmeshMesh& mesh = sdkmesh.GetSdkmeshMesh()[cnt];
		// check buffer number
		if (mesh.VertexBuffers[0] > sdkmesh.GetSdkmeshHeader().NumVertexBuffers)
			return -1;

		if (mesh.NumVertexBuffers > 1)
			return -1;

		//vertices
		std::vector<PosNormalTexTan_9> vb = vertexBuffers[mesh.VertexBuffers[0]];
		//output << "# " << vb.size() << " vertices of mesh " << cnt << "\n\n";
		for (int i = 0; i < vb.size(); i++)
		{
			PosNormalTexTan_9 vertex=vb[i];
			stx_VertexPositionNormalTexture v;
			v.Position=D3DXFROMWINEVECTOR3(vertex.pos.X, vertex.pos.Y, vertex.pos.Z);
			D3DXFROMWINEVec3Normalize(&v.Normal, &v.Position);
			mesh_.vertices.push_back(v);
			//output << "v " << vertex.pos.X << " " << vertex.pos.Y << " " << vertex.pos.Z << "\n";
		}
		//output << "\n";

		//for (auto vertex : vb)
		//{
		//	// value
		//	float X, Y, Z;
		//	X = (float)((vertex.norm.total & Dec3NMaskX) >> 22);
		//	Y = (float)((vertex.norm.total & Dec3NMaskY) >> 12);
		//	Z = (float)((vertex.norm.total & Dec3NMaskZ) >> 2);
		//	// sign bit
		//	if ((vertex.norm.total & Dec3NMaskXS) >> 31)
		//		X = -X;
		//	if ((vertex.norm.total & Dec3NMaskYS) >> 21)
		//		Y = -Y;
		//	if ((vertex.norm.total & Dec3NMaskZS) >> 11)
		//		Z = -Z;
		//	// normalize [-511.0, 511.0]
		//	output<< "vn " << (X / 511.0) << " " << (Y / 511.0) << " " << (Z / 511.0) << "\n";
		//}
		//output << "\n";

		for (int i = 0; i < vb.size(); i++)
		{
			PosNormalTexTan_9 vertex=vb[i];
			D3DXFROMWINEVECTOR2 v2((float)vertex.tex.X, (float)vertex.tex.Y);
			mesh_.vertices[i].Tex=v2;
			//output << "vt " << (float)vertex.tex.X << " " << (float)vertex.tex.Y << "\n";
		}
		//output << "\n";

		// check buffer number
		if (mesh.IndexBuffer > sdkmesh.GetSdkmeshHeader().NumIndexBuffers)
			return -1;

		// triangle mesh
		std::vector<int> ib = indexBuffers[mesh.IndexBuffer];
		//output << "g mesh_" << cnt << "_" << mesh.Name << "\n";
		//output << "usemtl " << sdkmesh.GetSdkmeshMaterial()[sdkmesh.GetSdkmeshSubset()[mesh.IndexBuffer].MaterialID].Name << "\n";

		int numTriangles = ib.size() / 3;
		for (int i = 0; i < numTriangles; i++)
		{
			int fir = ib[3 * i] + 1 + prevVertexNumTotal;
			int sec = ib[3 * i + 1] + 1 + prevVertexNumTotal;
			int thr = ib[3 * i + 2] + 1 + prevVertexNumTotal;
			//output << "f " << fir << "/" << fir << "/" << fir << " ";
			//output << sec << "/" << sec << "/" << sec << " ";
			//output << thr << "/" << thr << "/" << thr << "\n";
			mesh_.indices.push_back(fir);
			mesh_.indices.push_back(sec);
			mesh_.indices.push_back(thr);

		}

		prevVertexNumTotal += sdkmesh.GetSdkmeshVertexBufferHeader()[cnt].NumVertices;

		// if (cnt == 2)
		//	break;
		aMeshes.push_back(mesh_);
	}
#endif
	return 0;
}
void stx_Materials_(std::vector<stx_Material>& aVT, std::vector<__WORD__>& aFaces, std::vector<stx_Mesh>& aMesh)
{
	stx_Mesh m;
	aMesh.push_back(m);
	for(unsigned int i = 0; i < aVT.size(); i++)
	{
		stx_Material mat=aVT[i];
		aMesh[0].textureNames.push_back(mat.m_TextureName);
		std::vector<__WORD__> rindices;
		std::vector<__WORD__> faces;	
		for(unsigned int j = 0; j < aFaces.size(); j++)
		{
			#if 0
			rindices.push_back(0);
			rindices.push_back(0);
			rindices.push_back(0);
			#else
			rindices.push_back(aMesh[0].indices[3*j+0]);
			rindices.push_back(aMesh[0].indices[3*j+1]);
			rindices.push_back(aMesh[0].indices[3*j+2]);
			#endif
			faces.push_back(aFaces[j]);
		}
		aMesh[0].rindices.push_back(rindices);
		aMesh[0].faces=faces;	
	}
}
D3DXFROMWINEVECTOR2 stx_CalculatePositionalSphericalMapping(stx_VertexPositionNormalTexture& a)
{
/*
// determine extents
D3DXVECTOR3 _min,_max;
D3DXComputeBoundingBox(pVerts,numVerts,FVF_VERTEX,&_min,&_max);

// calculate center
D3DXVECTOR3 vCent;
vCent=(_max+_min)*0.5f;
*/
	D3DXFROMWINEVECTOR3 vCent(0.0f, 0.0f, 0.0f);
    // calculate normalized offset from center
    D3DXFROMWINEVECTOR3 v=a.Position-vCent;
    D3DXFROMWINEVec3Normalize(&v,&v);

    // calculate texture coordinates
    a.Tex.x=asinf(v.x)/D3DXFROMWINE_PI+0.5f;
    a.Tex.y=-asinf(v.y)/D3DXFROMWINE_PI-0.5f;

	return a.Tex;
}

//////////////////////////////	Math Functions  ////////////////////////////////*

// This computes the magnitude of a normal.   (magnitude = sqrt(x^2 + y^2 + z^2)
#define Mag(Normal) (sqrt(Normal.x*Normal.x + Normal.y*Normal.y + Normal.z*Normal.z))

// This calculates a vector between 2 points and returns the result
CVector3 Vector(CVector3 vPoint1, CVector3 vPoint2)
{
	CVector3 vVector;							// The variable to hold the resultant vector

	vVector.x = vPoint1.x - vPoint2.x;			// Subtract point1 and point2 x's
	vVector.y = vPoint1.y - vPoint2.y;			// Subtract point1 and point2 y's
	vVector.z = vPoint1.z - vPoint2.z;			// Subtract point1 and point2 z's

	return vVector;								// Return the resultant vector
}

// This adds 2 vectors together and returns the result
CVector3 AddVector(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vResult;							// The variable to hold the resultant vector
	
	vResult.x = vVector2.x + vVector1.x;		// Add Vector1 and Vector2 x's
	vResult.y = vVector2.y + vVector1.y;		// Add Vector1 and Vector2 y's
	vResult.z = vVector2.z + vVector1.z;		// Add Vector1 and Vector2 z's

	return vResult;								// Return the resultant vector
}

// This divides a vector by a single number (scalar) and returns the result
CVector3 DivideVectorByScaler(CVector3 vVector1, float Scaler)
{
	CVector3 vResult;							// The variable to hold the resultant vector
	
	vResult.x = vVector1.x / Scaler;			// Divide Vector1's x value by the scaler
	vResult.y = vVector1.y / Scaler;			// Divide Vector1's y value by the scaler
	vResult.z = vVector1.z / Scaler;			// Divide Vector1's z value by the scaler

	return vResult;								// Return the resultant vector
}

// This returns the cross product between 2 vectors
CVector3 Cross(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vCross;								// The vector to hold the cross product
												// Get the X value
	vCross.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
												// Get the Y value
	vCross.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
												// Get the Z value
	vCross.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	return vCross;								// Return the cross product
}

// This returns the normal of a vector
CVector3 Normalize(CVector3 vNormal)
{
	double Magnitude;							// This holds the magitude			

	Magnitude = Mag(vNormal);					// Get the magnitude

	vNormal.x /= (float)Magnitude;				// Divide the vector's X by the magnitude
	vNormal.y /= (float)Magnitude;				// Divide the vector's Y by the magnitude
	vNormal.z /= (float)Magnitude;				// Divide the vector's Z by the magnitude

	return vNormal;								// Return the normal
}

#if 0
/*
 * Get the list of all files in given directory and its sub directories.
 *
 * Arguments
 * 	dirPath : Path of directory to be traversed
 * 	dirSkipList : List of folder names to be skipped
 *
 * Returns:
 * 	vector containing paths of all the files in given directory and its sub directories
 *
 */
std::vector<std::string> getAllFilesInDir(const std::string &dirPath, 	const std::vector<std::string> dirSkipList = { })
{
 
	// Create a vector of string
	std::vector<std::string> listOfFiles;
	try {
		// Check if given path exists and points to a directory
		if (filesys::exists(dirPath) && filesys::is_directory(dirPath))
		{
			// Create a Recursive Directory Iterator object and points to the starting of directory
			filesys::recursive_directory_iterator iter(dirPath);
 
			// Create a Recursive Directory Iterator object pointing to end.
			filesys::recursive_directory_iterator end;
 
			// Iterate till end
			while (iter != end)
			{
				// Check if current entry is a directory and if exists in skip list
				if (filesys::is_directory(iter->path()) &&
						(std::find(dirSkipList.begin(), dirSkipList.end(), iter->path().filename()) != dirSkipList.end()))
				{
					// Skip the iteration of current directory pointed by iterator
#ifdef USING_BOOST
					// Boost Fileystsem  API to skip current directory iteration
					iter.no_push();
#elif 0
					// c++17 Filesystem API to skip current directory iteration
					iter.disable_recursion_pending();
#endif
				}
				else
				{
					// Add the name in vector
					listOfFiles.push_back(iter->path().string());
				}
 
				error_code ec;
				// Increment the iterator to point to next entry in recursive iteration
				iter.increment(ec);
				if (ec) {
					std::cerr << "Error While Accessing : " << iter->path().string() << " :: " << ec.message() << '\n';
				}
			}
		}
	}
	catch (std::system_error & e)
	{
		std::cerr << "Exception :: " << e.what();
	}
	return listOfFiles;
}
#endif
float stx_cbrt(const float a)
{
#ifdef LINUX
	return std::cbrt(a);
#else
	return pow(a, 1.0/3.0);
#endif
}

std::string stx_LowerUpper(	const std::string& fileName, 
					//bool aPath, 
					bool aName, 
					bool aExt)
{
	std::string path = fileName;
	std::size_t botDirPos = path.find_last_of("/");
	std::size_t botExtPos = path.find_last_of(".");
/*
	if(aPath)	
		std::transform(path.begin(), path.begin()+botDirPos,    path.begin(), ::toupper);
	else
		std::transform(path.begin(), path.begin()+botDirPos,    path.begin(), ::tolower);
*/
	if(aName)	
		std::transform(path.begin()+botDirPos, path.begin()+botExtPos,    path.begin()+botDirPos, ::toupper);
	else
		std::transform(path.begin()+botDirPos, path.begin()+botExtPos,    path.begin()+botDirPos, ::tolower);
	if(aExt)	
		std::transform(path.begin()+botExtPos, path.begin()+path.length(),path.begin()+botExtPos, ::toupper);
	else
		std::transform(path.begin()+botExtPos, path.begin()+path.length(),path.begin()+botExtPos, ::tolower);
	return path;
}

//stx_Effect::


