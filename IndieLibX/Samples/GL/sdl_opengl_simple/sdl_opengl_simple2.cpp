/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/importgl.h>

#include <STX/STX.h>
#include <STX/STXCPP.h>
#include <Framework3/IRenderer.h>

const char* title="opengl_simple2";
int   width = 640;
int   height = 512;
GLuint g_textureID = 0;
GLint vbo_id=0;

struct Vertex
{
    float x, y, z;
    float tu, tv;
};

Vertex g_quadVertices[] =
{
    { -1.0f,-1.0f, 0.0f, 0.0f,0.0f },
    {  1.0f,-1.0f, 0.0f, 1.0f,0.0f },
    {  1.0f, 1.0f, 0.0f, 1.0f,1.0f },
    {  1.0f, 1.0f, 0.0f, 1.0f,1.0f },
    { -1.0f, 1.0f, 0.0f, 0.0f,1.0f },
    { -1.0f,-1.0f, 0.0f, 0.0f,0.0f },
    
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

	char* ptr=0;
#if 0
	ptr=(char*)&g_quadVertices;
    	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), ptr + 0);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), ptr + 12);
#else
	//glDeleteBuffers(1, &vbo_id );
	if(0==vbo_id)
	{
		glGenBuffersARB( 1, &vbo_id );
		LOG_PRINT("vbo_id=%d\n", vbo_id );
	}
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_id);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, 30*sizeof(float), (char*)&g_quadVertices, GL_STATIC_DRAW_ARB);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), ptr + 0);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), ptr + 12);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_id);
#endif

    	glDrawArrays(GL_TRIANGLES, 0, 6);

    	SDL_GL_SwapWindow(STX_Service::GetSDLWindow());
}	

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("opengl_simple_GL2");
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE))
	{
		i->Update();
		render();
	}
	return 0;
}


