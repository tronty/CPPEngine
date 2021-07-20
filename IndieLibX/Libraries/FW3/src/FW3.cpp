/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>


FW3* FW3::s_pFW3=0;
FW3* FW3::GetInstance()
{
	if(s_pFW3==0)
		s_pFW3=new FW3();
	return s_pFW3;
}

int FW3::GetMesh(MeshRenderer2& mesh, const char* filename, tShader aShader=eSimpleShader)
{
	mesh.Load(filename, aShader);
	return 0;
}

FW3_Entity2d* FW3::GetEntity2d(){return new FW3_Entity2d();}
FW3_Entity3d* FW3::GetEntity3d(){return new FW3_Entity3d();}
FW3_3dMesh* FW3::Get3dMesh(){return new FW3_3dMesh();}
FW3_Camera2d* FW3::GetCamera2d(){return new FW3_Camera2d();}
FW3_Camera3d* FW3::GetCamera3d(){return new FW3_Camera3d();}
FW3_Light* FW3::GetLight(){return new FW3_Light();}
FW3_Animation* FW3::GetAnimation(const char* filename){return new FW3_Animation(filename);}
FW3_AnimationManager* FW3::GetAnimationManager(){return new FW3_AnimationManager();}
FW3_SurfaceManager* FW3::GetSurfaceManager(){return new FW3_SurfaceManager();}
FW3_LightManager* FW3::GetLightManager(){return new FW3_LightManager();}
FW3_FontManager* FW3::GetFontManager(){return new FW3_FontManager();}
FW3_Surface* FW3::GetSurface(){return new FW3_Surface();}
FW3_Timer* FW3::GetTimer(){return new FW3_Timer();}
FW3_Font* FW3::GetFont(){return new FW3_Font();}
FW3_MeshManager* FW3::GetMeshManager(){return new FW3_MeshManager();}
FW3_Entity2dManager* FW3::GetEntity2dManager(){return new FW3_Entity2dManager();}
FW3_Entity3dManager* FW3::GetEntity3dManager(){return new FW3_Entity3dManager();}

FW3_Animation::FW3_Animation(const char* name)
{
	Load(name);
}


void FW3_Animation::Load(const char* name)
{
	LOG_FNLN;
	LOG_PRINT("name=%s\n", name);
	std::string fn=stx_convertpath(name);
	frames.clear();
	sequences.clear();
	m_sequence=0;
	TiXmlDocument document(fn.c_str());
	document.LoadFile();
	TiXmlHandle docHandle( &document );
	{
	TiXmlHandle framesHandle = docHandle.FirstChild("animation").FirstChild("frames");
	TiXmlHandle frameHandle = framesHandle.FirstChild("frame");
	TiXmlElement* frameElement = frameHandle.Element();

	int i=1;
	while ( frameElement )
	{
	std::string nm=std::string(frameElement->Attribute("name"));
	//LOG_PRINT("Frame name=%s\n", nm.c_str());
	std::string fn2=frameElement->Attribute("file");

	Frame frame(nm.c_str(), fn2.c_str(), 0);//child2->Attribute("collision"));

	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	#if 0
	Image3 img;
	img.loadImageLibImage(fn2.c_str(), false);
	img.flipY();
	frame.m_TextureID=IRenderer::GetRendererInstance()->addTexture(&img, false, ss);
	#else
	frame.m_TextureID=IRenderer::GetRendererInstance()->addImageLibTexture(fn2.c_str(), false, ss);
	#endif

	frames.push_back(frame);
	frameHandle = framesHandle.Child( "frame", i++ );
	frameElement = frameHandle.Element();
	}
	}{
	int i=0;
	TiXmlHandle sequencesHandle = docHandle.FirstChild( "animation" ).FirstChild( "sequences" );
	TiXmlHandle sequenceHandle = sequencesHandle.Child( "sequence", i++);
	TiXmlElement* sequenceElement = sequenceHandle.Element();

	while ( sequenceElement )
	{
	int j=0;
	std::string nm=std::string(sequenceElement->Attribute("name"));
	Sequence sequence(nm.c_str(), docHandle);
	TiXmlHandle frameHandle = sequenceHandle.Child("frame", j++);
	TiXmlElement* frameElement = frameHandle.Element();
	while ( frameElement )
	{
	__DWORD__ t=0;
	if(frameElement->Attribute("time"))
		t=atoi(frameElement->Attribute("time"));

	//LOG_PRINT("t=%d\n", t);

	Sequence::FrameInSequence frame(frameElement->Attribute("name"), t);	
	//LOG_PRINT("FrameInSequence name=%s\n", frameElement->Attribute("name"));
	sequence.frames.push_back(frame);

	frameHandle = sequenceHandle.Child( "frame", j++ );
	frameElement = frameHandle.Element();

	}
	sequences.push_back(sequence);
	sequenceHandle = sequencesHandle.Child( "sequence", i++ );
	sequenceElement = sequenceHandle.Element();
	}
	}

	for(unsigned int i=0;i<sequences.size();i++)
	{
		const char* ns=sequences[i].m_sequence.c_str();
	for(unsigned int j=0;j<sequences[i].frames.size();j++)
	{
		const char* nf=sequences[i].frames[j].m_name.c_str();
		for(unsigned int z=0;z<frames.size();z++)
		{
			if(stx_strcmp(frames[z].m_name.c_str(), nf)==0)
			{
				sequences[i].frames[j].m_TextureID=frames[z].m_TextureID;
				//frames[z].m_time=sequences[i].frames[j].m_time;
				break;
			}
		}
	}
	}
//stx_exit(0);
}
void FW3_Animation::SetSequence(const char* name)
{
	for(unsigned int i=0;i<sequences.size();i++)
	{
       		if (stx_strcmp(sequences[i].m_sequence.c_str(),name)==0)
		{
			m_sequence=i;
			return;
		}
	}
	m_sequence=-1;
}
void FW3_Animation::SetPosition(float x, float y)
{
	position=D3DXFROMWINEVECTOR2(x,y);
}
void FW3_Animation::SetSize(float sx, float sy)
{
	size=D3DXFROMWINEVECTOR2(sx,sy);
}
void FW3_Animation::render()
{
	if(m_sequence==-1) return;
	if(m_sequence>(sequences.size()-1)) return;
	sequences[m_sequence].render(position, size);
}
void FW3_Animation::Sequence::render(D3DXFROMWINEVECTOR2 aposition, D3DXFROMWINEVECTOR2 asize)
{
	if(!frames.size()) return;
	if(m_index>(frames.size()-1))
		m_index=0;
	
	frames[m_index].render(aposition, asize);

	__DWORD__ now=timeGetTime();

	__DWORD__ dt=now-frames[m_index].m_then;

	//LOG_PRINT("dt=%d\n", dt);
	//LOG_PRINT("m_dt=%d\n", m_dt);

	if (dt>m_dt)
	{
		//LOG_PRINT("frames[%d].m_time=%d\n", m_index, frames[m_index].m_time);
		m_dt+=frames[m_index].m_time;
		m_index++;
		frames[m_index].m_then=now;
	}
}

FW3_Animation::Frame::Frame(const char* aname, const char* afile, const char* acollision)
{
	m_name=std::string(aname);
	m_file=std::string(afile);
	m_collision=std::string("");
	m_time=0;
	if (acollision)
		m_collision=std::string(acollision);
	m_TextureID=-1;
}

FW3_Animation::Sequence::FrameInSequence::FrameInSequence(const char* aname, __DWORD__ atime)
{
	m_name=std::string(aname);
	m_time=atime;
	m_then=0;		
	m_TextureID=-1;
}
void FW3_Animation::Sequence::FrameInSequence::render(D3DXFROMWINEVECTOR2 aposition, D3DXFROMWINEVECTOR2 asize)
{
	if(m_TextureID>-1)
	{
		TexVertex dest[4];
		float x=aposition.x;
		float y=aposition.y;
		dest[0].position = D3DXFROMWINEVECTOR2(x + asize.x, y);
		dest[0].texCoord = D3DXFROMWINEVECTOR2(1.0f, 0.0f);
		dest[1].position = D3DXFROMWINEVECTOR2(x + asize.x, y + asize.y);
		dest[1].texCoord = D3DXFROMWINEVECTOR2(1.0f, 1.0f);
		dest[2].position = D3DXFROMWINEVECTOR2(x, y);
		dest[2].texCoord = D3DXFROMWINEVECTOR2(0.0f, 0.0f);
		dest[3].position = D3DXFROMWINEVECTOR2(x, y + asize.y);
		dest[3].texCoord = D3DXFROMWINEVECTOR2(0.0f, 1.0f);
		IRenderer::GetRendererInstance()->drawTextured(PRIM_TRIANGLE_STRIP, dest, 2, m_TextureID,
IRenderer::GetRendererInstance()->GetblendSrcAlpha(), IRenderer::GetRendererInstance()->GetnoDepthTest());
	}
}

FW3_Animation::Sequence::Sequence(const char* name, TiXmlHandle& adocHandle)
{
	m_index=0;
	m_dt=0;
	m_sequence=std::string("");
	TiXmlHandle child2h = adocHandle.FirstChild("animation").FirstChild("sequences" ).Child( "sequence", 0 );
	TiXmlElement* child2 = child2h.Element();
	int i=0;
	while ( child2 )
	{
		child2 = child2h.Element();
		i++;
		if(!( child2 )) break;
		const char* n1=child2->Attribute("name");

       		if(stx_strcmp(n1, name)==0)
		{
			m_sequence=std::string(name);
			return;
		}
		child2h = adocHandle.FirstChild( "animation" ).FirstChild( "sequences" ).Child( "sequence", i );
		child2 = child2h.Element();
	}
}
bool FW3_SurfaceManager::Add(FW3_Surface* a, const char* fn, ___FW3___, ___FW3___)
{
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();

	#if 0
	Image3 img;
	img.loadImageLibImage(fn, false);
	img.flipY();
	frame.m_TextureID=IRenderer::GetRendererInstance()->addTexture(&img, false, ss);
	#elif 0
	frame.m_TextureID=IRenderer::GetRendererInstance()->addImageLibTexture(fn, false, ss);
	#endif


	m_v.push_back(a);
	return true;
}

bool FW3_AnimationManager::AddToSurface(FW3_Animation* a, const char* fn, ___FW3___, ___FW3___)
{
	a->Load(fn);
	//m_v.push_back(a);
	return true;
}

void FW3_Entity2dManager::RenderEntities2d(int a)
{
	for(unsigned int i=0;i<FW3::GetInstance()->AnimationManager->m_v.size();i++)
	{
		FW3::GetInstance()->AnimationManager->m_v[i]->SetPosition(
FW3::GetInstance()->AnimationManager->m_v[i]->m_pos.x,
FW3::GetInstance()->AnimationManager->m_v[i]->m_pos.y
);
		FW3::GetInstance()->AnimationManager->m_v[i]->SetSize(50.0f, 50.0f);
		FW3::GetInstance()->AnimationManager->m_v[i]->render();
		FW3::GetInstance()->SurfaceManager->m_v[i]->render(
			D3DXFROMWINEVECTOR2(
				FW3::GetInstance()->SurfaceManager->m_v[i]->m_pos.x,
				FW3::GetInstance()->SurfaceManager->m_v[i]->m_pos.y
			),
			D3DXFROMWINEVECTOR2(50.0f, 50.0f));
	}
}

