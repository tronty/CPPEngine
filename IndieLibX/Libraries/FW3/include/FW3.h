/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef __FW3__
#define __FW3__

//using namespace AssimpView;

enum ___FW3___
{
	FW3_ALPHA,
	FW3_32,
	FW3_LEFT,
	FW3_OPAQUE,
	FW3_DIRECTIONAL_LIGHT,
	FW3_FILTER_LINEAR,
	FW3_SRCALPHA,
	FW3_INVSRCALPHA
};
typedef D3DXFROMWINEMATRIX FW3_Matrix;

struct FW3_Animation;
struct FW3_AnimationManager;
struct FW3_SurfaceManager;
struct FW3_LightManager;
struct FW3_FontManager;
struct FW3_Entity2dManager;
struct FW3_Entity3dManager;

struct FW3_Entity2d;
struct FW3_Entity3d;
struct FW3_3dMesh;
struct FW3_Camera2d;
struct FW3_Camera3d;
struct FW3_Light;

struct FW3_Surface;
struct FW3_Timer;
struct FW3_Font;
struct FW3_MeshManager;
struct FW3_Entity2dManager;
struct FW3_Entity3dManager;

//struct MeshRenderer2;
struct FW3
{
	protected:
		static FW3* s_pFW3;
		FW3(){}
	public:
		static FW3_Entity2d* GetEntity2d();
		static FW3_Entity3d* GetEntity3d();
		static FW3_3dMesh* Get3dMesh();
		static FW3_Camera2d* GetCamera2d();
		static FW3_Camera3d* GetCamera3d();
		static FW3_Light* GetLight();
		static FW3_Animation* GetAnimation(const char* filename);
		static FW3_AnimationManager* GetAnimationManager();
		static FW3_SurfaceManager* GetSurfaceManager();
		static FW3_LightManager* GetLightManager();
		static FW3_FontManager* GetFontManager();
		static FW3_Surface* GetSurface();
		static FW3_Timer* GetTimer();
		static FW3_Font* GetFont();
		static FW3_MeshManager* GetMeshManager();
		static FW3_Entity2dManager* GetEntity2dManager();
		static FW3_Entity3dManager* GetEntity3dManager();
		virtual ~FW3(){}

		int GetMesh(MeshRenderer2& m, const char* filename, tShader aShader);
#ifndef SWIG

		FW3_AnimationManager* AnimationManager;
		FW3_SurfaceManager* SurfaceManager;
		FW3_LightManager* LightManager;
		FW3_FontManager* FontManager;
		FW3_Entity2dManager* Entity2dManager;
		FW3_Entity3dManager* Entity3dManager;
		static VArray <FW3_Animation*> animations;
#endif
		static FW3* GetInstance();
};
struct FW3_Surface;
struct FW3_Font;
struct FW3_Entity2d
{
	D3DXFROMWINEVECTOR3 m_pos;
	D3DXFROMWINEVECTOR2 m_scale;
	FW3_Entity2d()
	{
		m_pos=D3DXFROMWINEVECTOR3(0.0f,0.0f,0.0f);
		m_scale=D3DXFROMWINEVECTOR2(1.0f,1.0f);
	}
	virtual ~FW3_Entity2d(){}
	void SetSurface(FW3_Surface* s){}
	void SetPosition(float x, float y, float z)
	{
		m_pos=D3DXFROMWINEVECTOR3(x,y,z);
		m_scale=D3DXFROMWINEVECTOR2(1.0f,1.0f);
	}
	void SetScale(float x, float y){m_scale=D3DXFROMWINEVECTOR2(x,y);}
	void SetHotSpot(float x, float y){}
	void SetAngleXYZ(int , int, float){}
	void SetBoundingCircle(const char*, int , int, float){}
	int GetPosX(){return m_pos.x;}
	int GetPosY(){return m_pos.y;}
	int GetPosZ(){return m_pos.z;}
	FW3_Surface* GetSurface(){return 0;}
	void SetAnimation(FW3_Animation*){}
	void SetSequence(int){}
	FW3_Animation* GetAnimation(){return 0;}
	float GetAngleZ(){return 0.0f;}
	void SetShow(bool){}
	void SetText(const char*){}
	void SetFont(FW3_Font*){}
	void SetLineSpacing(int){}
	void SetCharSpacing(int){}
	void SetAlign(FW3){}
	void SetTransparency(int){}
	void SetRegion(int, int, int, int){}
	virtual void render(D3DXFROMWINEVECTOR2 aposition, D3DXFROMWINEVECTOR2 asize){}
};
struct FW3_3dMesh;
struct FW3_Entity3d
{
	FW3_3dMesh* m_Mesh;
	FW3_Entity3d(){m_Mesh=0;}
	virtual ~FW3_Entity3d(){}
	void Set3dMesh(FW3_3dMesh* a){m_Mesh=a;}
	virtual void render(D3DXFROMWINEMATRIX& aMVP){}
};
struct FW3_3dMesh : public FW3_Entity3d
{
	FW3_3dMesh():FW3_Entity3d(){}
	virtual ~FW3_3dMesh(){}
    int GetNumSequences(){return 0;}//return m_Mesh->}
};
struct FW3_Camera2d
{
	FW3_Camera2d(){}
	FW3_Camera2d(int, int){}
	FW3_Camera2d(FW3_Camera2d&){}
	virtual ~FW3_Camera2d(){}
};
struct FW3_Camera3d
{
	FW3_Camera3d(){}
	FW3_Camera3d(float, float, float){}
	FW3_Camera3d(FW3_Camera3d&){}
	virtual ~FW3_Camera3d(){}


	void MoveWalk(float){}
	void MoveStrafe(float){}
	void SetYaw(float){}
	void SetAspect(float){}


};
struct FW3_Light
{
	FW3_Light(){}
	virtual ~FW3_Light(){}

	void SetPosition(float, float, float){}
	void SetColor(float, float, float, float){}
	void SetDirection(float, float, float){}
	void SetRange(float){}

};


struct FW3_Animation : public FW3_Entity2d
{
#ifndef SWIG
	struct Frame
	{
		Frame()
		{
			m_name="";
			m_file="";
			m_collision="";
			m_time=0;
			m_TextureID=-1;
		}
		Frame(const char* aname, const char* afile, const char* acollision=0);
		virtual ~Frame(){}
		std::string m_name;
		std::string m_file;
		std::string m_collision;
		__DWORD__ m_time;
		TextureID m_TextureID;
	};
	struct Sequence
	{
	struct FrameInSequence
	{
		FrameInSequence()
		{
			m_name="";
			m_then=0;
			m_time=0;
			m_TextureID=-1;
		}
		FrameInSequence(const char* aname, __DWORD__ atime);
		virtual ~FrameInSequence(){}
		virtual void render(D3DXFROMWINEVECTOR2 aposition, D3DXFROMWINEVECTOR2 asize);
		std::string m_name;
		__DWORD__ m_time;
		__DWORD__ m_then;
		TextureID m_TextureID;
	};
		Sequence()
		{
			m_sequence="";
			m_index=0;
			m_dt=0;
		}
		Sequence(const char* name, TiXmlHandle& adocHandle);
		virtual ~Sequence()
		{
			m_sequence="";
			m_index=0;
		};
		virtual void render(D3DXFROMWINEVECTOR2 aposition, D3DXFROMWINEVECTOR2 asize);
		std::string m_sequence;
		int m_index;
		__DWORD__ m_dt;
		std::vector<Sequence::FrameInSequence> frames;
	};
	std::vector<Frame> frames;
	std::vector<Sequence> sequences;
	D3DXFROMWINEVECTOR2 position;
	D3DXFROMWINEVECTOR2 size;
	int m_sequence;
#endif
	FW3_Animation():FW3_Entity2d()
	{
		position=D3DXFROMWINEVECTOR2(0.0f, 0.0f);
		size=D3DXFROMWINEVECTOR2(0.0f, 0.0f);
		m_sequence=0;
	}
	FW3_Animation(const char* name);
	virtual ~FW3_Animation(){}
	virtual void Load(const char* name);
	virtual void SetSequence(const char* name);
	virtual void SetPosition(float x, float y);
	virtual void SetSize(float sx, float sy);
	virtual void render();

	int GetNumFrames(int a){return 0;}//frames[a].frames.size();}
	int GetActualFramePos(int ){return 0;}

};

struct FW3_AnimationManager
{
	std::vector<FW3_Animation*> m_v;
	FW3_AnimationManager(){}
	virtual ~FW3_AnimationManager(){}
	bool Add(FW3_Animation* a){m_v.push_back(a); return true;}
	void Delete(FW3_Animation*){}
	bool AddToSurface(FW3_Animation*, const char*, ___FW3___, ___FW3___);
};
struct FW3_SurfaceManager
{
	std::vector<FW3_Surface*> m_v;
	FW3_SurfaceManager(){}
	virtual ~FW3_SurfaceManager(){}
	bool Add(FW3_Surface* a, const char* fn, ___FW3___, ___FW3___);
	void Delete(FW3_Surface*){}
	void Clone(FW3_Surface*, FW3_Surface*){}
};
struct FW3_LightManager
{
	FW3_LightManager(){}
	virtual ~FW3_LightManager(){}
	bool Add(FW3_Light*, ___FW3___){return true;}
	void Delete(FW3_Light*){}
	void Update(){}
};
struct FW3_FontManager
{
	FW3_FontManager(){}
	virtual ~FW3_FontManager(){}
	bool Add(FW3_Font*, const char*, const char*, ___FW3___, ___FW3___){return true;}
	void Delete(FW3_Font*){}
};
struct FW3_Surface : public FW3_Entity2d
{
	int m_TextureID;
	FW3_Surface():FW3_Entity2d(),m_Width(0),m_Height(0),m_TextureID(-1){}
	virtual ~FW3_Surface(){}
	int GetWidth(){return m_Width;}
	int GetHeight(){return m_Height;}
	int m_Width;
	int m_Height;
	int GetBlocksX(){return 0;}
	int GetBlocksY(){return 0;}
	int GetWidthBlock(){return 0;}
	int GetHeightBlock(){return 0;}

	void SetGrid(int, int){}
	void SetVertexPos(int, int, int, int){}
	virtual void render(D3DXFROMWINEVECTOR2 aposition, D3DXFROMWINEVECTOR2 asize){}

};
struct FW3_Timer
{
	FW3_Timer(){}
	virtual ~FW3_Timer(){}
	void Start(){}
	void Stop(){}
    __DWORD__ GetTicks(){return 0;}
};
struct FW3_Font : public FW3_Entity2d
{
	FW3_Font():FW3_Entity2d(){}
	virtual ~FW3_Font(){}
};
struct FW3_MeshManager
{
	std::vector<FW3_3dMesh*> m_v;
	FW3_MeshManager(){}
	virtual ~FW3_MeshManager(){}
	bool Add(FW3_3dMesh* a, const char*, const char*){m_v.push_back(a); return true;}
	void Delete(FW3_3dMesh*){}
};

struct FW3_Entity2dManager
{
	std::vector<FW3_Entity2d*> m_v;
	FW3_Entity2dManager(){}
	virtual ~FW3_Entity2dManager(){}
	bool Add(FW3_Entity2d* a){m_v.push_back(a); return true;}
	bool Add(int, FW3_Entity2d* a){m_v.push_back(a); return true;}
	void Delete(FW3_Entity2d*){}
	void RenderEntities2d(int a=0);
	void RenderCollisionAreas(int, int, int, int, int){}
	bool IsCollision(FW3_Entity2d*, const char*, FW3_Entity2d*, const char*){return false;}
};
struct FW3_Entity3dManager
{
	std::vector<FW3_Entity3d*> m_v;
	FW3_Entity3dManager(){}
	virtual ~FW3_Entity3dManager(){}
	bool Add(FW3_Entity3d* a){m_v.push_back(a); return true;}
	bool Add(int, FW3_Entity3d* a){m_v.push_back(a); return true;}
	void Delete(FW3_Entity3d*){}
	void RenderEntities3d(int a=0){}
	void RenderCollisionAreas(int, int, int, int, int){}
	bool IsCollision(FW3_Entity3d*, const char*, FW3_Entity3d*, const char*){return false;}
};
#endif

