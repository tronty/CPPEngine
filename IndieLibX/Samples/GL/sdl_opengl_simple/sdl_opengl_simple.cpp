#if !defined(__APPLE__)
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#endif
#if defined(LINUX) || defined(__APPLE__)
/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <STX/STX.h>
#include <STX/STXCPP.h>
#include <Framework3/IRenderer.h>

const char* title="opengl_simple";
int   width = 640;
int   height = 512;

GLuint g_textureID = 0;

static float g_fSpinX=0.0f;
static float g_fSpinY=0.0f;
static float g_fSpinZ=0.0f;

struct Vertex_m_
{
    float tu, tv;
    float x, y, z;
	Vertex_m_(D3DXFROMWINEVECTOR3 pos, D3DXFROMWINEVECTOR2 uv)
	{
		x=pos.x;
		y=pos.y;
		z=pos.z;
		tu=uv.x;
		tv=uv.y;
	}
};
std::vector<Vertex_m_> g_vertices_;

std::vector<D3DXFROMWINEVECTOR3> g_vertices3;
std::vector<D3DXFROMWINEVECTOR2> g_texcoords2;
std::vector<__WORD__> g_indices1;

void DrawQuad(D3DXFROMWINEMATRIX& aM, D3DXFROMWINEVECTOR2 asize=D3DXFROMWINEVECTOR2(1.0f, 1.0f))
{
	{D3DXFROMWINEVECTOR3 v(-asize.x, -asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(0.0f, 0.0f);Vertex_m_ vx(vo, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);}
	{D3DXFROMWINEVECTOR3 v( asize.x, -asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(1.0f, 0.0f);Vertex_m_ vx(vo, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);}
	{D3DXFROMWINEVECTOR3 v( -asize.x, asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(0.0f, 1.0f);Vertex_m_ vx(vo, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);}
	
	{D3DXFROMWINEVECTOR3 v(-asize.x, asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(0.0f, 1.0f);Vertex_m_ vx(vo, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);}
	{D3DXFROMWINEVECTOR3 v( asize.x, asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(1.0f, 1.0f);Vertex_m_ vx(vo, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);}
	{D3DXFROMWINEVECTOR3 v(asize.x, -asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(1.0f, 0.0f);Vertex_m_ vx(vo, tc);g_vertices_.push_back(vx);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);}
	
}

GLuint loadTexture( const char* textureFilename=0 )    
{
	GLuint _textureID = 0;
	Image3 img;
	bool useMipMaps=false;
	img.loadImageLibImage(textureFilename, useMipMaps);

    glGenTextures( 1, & _textureID );
    glBindTexture( GL_TEXTURE_2D, _textureID );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    glTexImage2D( GL_TEXTURE_2D, 0, 3, img.getWidth(), img.getHeight(), 
                  0, GL_RGB, GL_UNSIGNED_BYTE, img.getPixels() );
	return  _textureID;
}

int init(const char* aTitle)
{
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glEnable( GL_TEXTURE_2D );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 45.0f, 1.0f*width/height, 0.1f, 100.0f);
 
#ifdef __APPLE__
    g_textureID=loadTexture("../../../../../IndieLib_resources/test.bmp");
#else
    g_textureID=loadTexture("../../IndieLib_resources/test.bmp");
#endif

	D3DXFROMWINEVECTOR2 size(1.0f,  1.0f);
	D3DXFROMWINEMATRIX  w;
	D3DXFROMWINEVECTOR3 vR, vP;

	vR=D3DXFROMWINEVECTOR3(0*90.0f, 0.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(0.0f, 0.0f, -1.0f);
	w=stx_GetPivotMatrix(vR, vP);
	DrawQuad(w, size);

	vR=D3DXFROMWINEVECTOR3(1*90.0f, 0.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(1.0f, 0.0f, 0.0f);
	w=stx_GetPivotMatrix(vR, vP);
	DrawQuad(w, size);

	vR=D3DXFROMWINEVECTOR3(0*90.0f, 0.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 1.0f);
	w=stx_GetPivotMatrix(vR, vP);
	DrawQuad(w, size);

	vR=D3DXFROMWINEVECTOR3(1*90.0f, 0.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(-1.0f, 0.0f, 0.0f);
	w=stx_GetPivotMatrix(vR, vP);
	DrawQuad(w, size);

	vR=D3DXFROMWINEVECTOR3(0.0f, 1*90.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(0.0f, 1.0f, 0.0f);
	w=stx_GetPivotMatrix(vR, vP);
	DrawQuad(w, size);

	vR=D3DXFROMWINEVECTOR3(0.0f, -1*90.0f, 0.0f);
	vP=D3DXFROMWINEVECTOR3(0.0f, -1.0f, -0.0f);
	w=stx_GetPivotMatrix(vR, vP);
	DrawQuad(w, size);

	glEnableClientState(GL_VERTEX_ARRAY);
    	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glClientActiveTexture(GL_TEXTURE0);
    
    glBindTexture( GL_TEXTURE_2D, g_textureID );

	return 0;
}
	
void render()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
	glClearDepth(1.0f);
  
    glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
    glTranslatef( 0.0f, 0.0f, -5.0f );
    glRotatef( -g_fSpinY, 1.0f, 0.0f, 0.0f );
    glRotatef( -g_fSpinX, 0.0f, 1.0f, 0.0f );

    const size_t vx_loc = 8;//offsetof(Vertex_m_, Vertex_m_::x);
    const size_t vtx_loc = 0;//offsetof(Vertex_m_, Vertex_m_::tu);

    	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_m_), (char*)&g_vertices_[0] + vx_loc);

	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_m_), (char*)&g_vertices_[0] + vtx_loc);
#if 0
    	glDrawArrays(GL_TRIANGLES, 0, g_vertices_.size());
#elif 1
	glDrawElements(GL_TRIANGLES, g_indices1.size(), GL_UNSIGNED_SHORT, (void*)&g_indices1[0]);
#else
	glDrawElements(GL_TRIANGLES, g_indices1.size(), GL_UNSIGNED_INT, (void*)0);
#endif
    	SDL_GL_SwapWindow(STX_Service::GetSDLWindow());
#if 1
	{
		if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_q))
			stx_exit(0);
		if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_w))
			{LOG_PRINT("SDLK_w\n");}
		if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_s))
			{LOG_PRINT("SDLK_s\n");}
		if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_a))
			{LOG_PRINT("SDLK_a\n");}
		if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_d))
			{LOG_PRINT("SDLK_d\n");}
		//LOG_PRINT("event.key.keysym.mod=%x\n", event.key.keysym.mod);

		if(STX_Service::GetInputInstance()->OnShiftKeyPress())
			{LOG_PRINT("KMOD_SHIFT\n");}
		if(STX_Service::GetInputInstance()->OnLShiftKeyPress())
			{LOG_PRINT("KMOD_LSHIFT\n");}
		if(STX_Service::GetInputInstance()->OnRShiftKeyPress())
			{LOG_PRINT("KMOD_RSHIFT\n");}

		if(STX_Service::GetInputInstance()->OnCTRLKeyPress())
			{LOG_PRINT("KMOD_CTRL\n");}
		if(STX_Service::GetInputInstance()->OnLCTRLKeyPress())
			{LOG_PRINT("KMOD_LCTRL\n");}
		if(STX_Service::GetInputInstance()->OnRCTRLKeyPress())
			{LOG_PRINT("KMOD_RCTRL\n");}

		if(STX_Service::GetInputInstance()->OnALTKeyPress())
			{LOG_PRINT("KMOD_ALT\n");}
		if(STX_Service::GetInputInstance()->OnLALTKeyPress())
			{LOG_PRINT("KMOD_LALT\n");}
		if(STX_Service::GetInputInstance()->OnRALTKeyPress())
			{LOG_PRINT("KMOD_RALT\n");}

		if(STX_Service::GetInputInstance()->OnNUMKeyPress())
			{LOG_PRINT("KMOD_NUM\n");}
		if(STX_Service::GetInputInstance()->OnCAPSKeyPress())
			{LOG_PRINT("KMOD_CAPS\n");}
		if(STX_Service::GetInputInstance()->OnMODEKeyPress())
			{LOG_PRINT("KMOD_MODE\n");}
}
#endif
}	

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("opengl_simple_GL 3D");
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE))
	{
		i->Update();
		stx_GetAngles(g_fSpinX, g_fSpinY, 0.25f);
		render();
	}
	return 0;
}
#endif
#endif

