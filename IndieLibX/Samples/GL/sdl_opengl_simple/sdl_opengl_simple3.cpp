#if defined(LINUX) || defined(__APPLE__)
/*
  Copyright (c) 2018 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>

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

struct Vertex_
{
    float tu, tv;
    float x, y, z;
};

Vertex_ g_quadVertices[] =
{
    { 0.0f,0.0f, -1.0f,-1.0f, 0.0f },
    { 1.0f,0.0f,  1.0f,-1.0f, 0.0f },
    { 1.0f,1.0f,  1.0f, 1.0f, 0.0f },
    { 0.0f,1.0f, -1.0f, 1.0f, 0.0f }
};

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
#if 0
    const size_t vx_loc = 8;//offsetof(Vertex_, Vertex_::x);
    const size_t vtx_loc = 0;//offsetof(Vertex_, Vertex_::tu);

    	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_), (char*)&g_vertices_[0] + vx_loc);

	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_), (char*)&g_vertices_[0] + vtx_loc);
#endif
    glDrawArrays( GL_QUADS, 0, 4 );
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

