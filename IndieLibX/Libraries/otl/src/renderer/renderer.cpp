/*
  Copyright (c) 2004 Pascal Kirchdorfer, Seth Yastrov

  This software is provided 'as-is', without any express or implied warranty. In
  no event will the authors be held liable for any damages arising from the use
  of this software.

  Permission is granted to anyone to use this software for any purpose, including
  commercial applications, and to alter it and redistribute it freely, subject to
  the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim
       that you wrote the original software. If you use this software in a product,
       an acknowledgment in the product documentation would be appreciated but is
       not required.

    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/

#include <Framework3/IRenderer.h>
#include "renderer/light.h"
#include "renderer/renderer.h"
#include "renderer/texture.h"
#include "renderer/triangles.h"
#include "renderer/vertices.h"
#include "renderer/tangents.h"

#define LOG_PRINT_(...) printf(__VA_ARGS__);
#define LOG_FNLN_ printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
#define LOG_FNLN
#define LOG_PRINT

//SDL_Window* g_SDL_window;
//SDL_GLContext g_SDL_context;

unsigned int g_numvertices=0;
//Tangent* g_tangents=0;

D3DXFROMWINEVECTOR4 g_Ambient(0.0f, 0.1f, 0.0f, 1.0f);
D3DXFROMWINEVECTOR4 g_Diffuse(0.0f, 1.0f, 0.0f, 1.0f);
D3DXFROMWINEVECTOR4 g_Specular(0.0f, 1.0f, 0.0f, 1.0f);
D3DXFROMWINEVECTOR4 g_Pos(0.0f, 0.0f, 0.0f, 1.0f);

TextureID TexDiffuse=-1;
TextureID TexNormal=-1;
TextureID TexHeight=-1;

D3DXFROMWINEMATRIX g_proj;
D3DXFROMWINEMATRIX g_view;

extern ShaderID g_program;
extern VertexFormatID g_vf;

Renderer::Renderer()
: hasInited(false), hasVboExt(false), last(0), fps(0), wireframe(false), textured(true),
  normals(false), points(false), vlist(0)//, tlist(0)
{
}

Renderer::~Renderer()
{
#if 0
  if (hasInited)
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
#else
#endif
}

void Renderer::init()
{
#if 0
#if 0
  SDL_InitSubSystem(SDL_INIT_VIDEO);

  SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, 32 );
  SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 ); 

  const SDL_VideoInfo * videoInfo = SDL_GetVideoInfo( );

  int videoFlags = 0;
  videoFlags  = SDL_OPENGL;          //Enable OpenGL
  videoFlags |= SDL_GL_DOUBLEBUFFER; //Enable double buffering

  if ( videoInfo->hw_available )
      videoFlags |= SDL_HWSURFACE;   //Hardware Surface
  else
      videoFlags |= SDL_SWSURFACE;   //Software Surface

  if ( videoInfo->blit_hw )
      videoFlags |= SDL_HWACCEL;     //Hardware Acceleration

  //Set double buffering
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

  SDL_SetVideoMode( 1024, 768, 32, videoFlags );
#else
	SDL_Init(
		SDL_INIT_TIMER|
		 SDL_INIT_AUDIO|
		//SDL_INIT_EVENTTHREAD|
		 SDL_INIT_JOYSTICK|SDL_INIT_VIDEO);
    g_SDL_window = SDL_CreateWindow( "OpenTreeLibrary", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_OPENGL );
#endif
  hasInited = true;

  //Enable OTL_Texture Mapping
  glEnable( GL_TEXTURE_2D );

  //Enable Coloring
  //glColorMaterial( GL_FRONT, GL_DIFFUSE );
  //glEnable( GL_COLOR_MATERIAL );

  //Enable smooth shading
  glShadeModel( GL_SMOOTH );

  //Set the background black
  glClearColor( 0.3f, 0.1f, 0.0f, 0.0f );

  //Depth buffer setup
  glClearDepth( 1.0f );

  //Enables Depth Testing
  glEnable( GL_DEPTH_TEST );

  //The Type Of Depth Test To Do
  glDepthFunc( GL_LEQUAL );

  //Really Nice Perspective Calculations
  glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
  glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
  
  //Enable Light One
  glEnable( GL_LIGHT0 );
  glEnable( GL_LIGHTING );

  //Setup The Ambient Light
  float ambiente[] = { 0.2f, 0.2f, 0.2f, 1.0f };
  glLightfv( GL_LIGHT0, GL_AMBIENT, ambiente );

  //Setup The Diffuse Light
  float diffuse[] = { 0.8f, 0.8f, 0.8f, 0.8f };
  glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );

  //Setup The Diffuse Light
  float specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glLightfv( GL_LIGHT0, GL_SPECULAR, specular );

  //Position The Light
  float pos[] = { -1.5f, 1.0f, -4.0f, 1.0f };
  glLightfv( GL_LIGHT0, GL_POSITION, pos );

  float mcolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mcolor);

  float specReflection[] = { 0.8f, 0.8f, 0.8f, 1.0f };
  glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);

  glMateriali(GL_FRONT, GL_SHININESS, 96);

  //Blending Function For Translucency Based On Source Alpha Value
  glEnable(GL_BLEND);	
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  //Enable Face Culling
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnableClientState (GL_VERTEX_ARRAY);
  glEnableClientState (GL_NORMAL_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glEnableClientState (GL_TEXTURE_COORD_ARRAY);

  glAlphaFunc(GL_GEQUAL,0.1f);
  glEnable(GL_ALPHA_TEST);

  //glEnable(GL_NORMALIZE);

#ifndef GL_ARB_multitexture
  // Multitexure extension
  glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC) SDL_GL_GetProcAddress("glActiveTextureARB");
  glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC) SDL_GL_GetProcAddress("glClientActiveTextureARB");

  if (!glActiveTextureARB)
  {
    LOG_PRINT ("Multitexture extensions not supported");
  }
#endif

  glPointParameterfARB = (PFNGLPOINTPARAMETERFARBPROC) SDL_GL_GetProcAddress("glPointParameterfARB");
  glPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC) SDL_GL_GetProcAddress("glPointParameterfvARB");

  if (glPointParameterfARB)
  {
    // This is how will our point sprite's size will be modified by 
    // distance from the viewer
    float quadratic[] =  { 1.0f, 0.0f, 0.01f };
    glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );

    // Query for the max_ point size supported by the hardware
    float maxSize = 0.0f;
    glGetFloatv( GL_POINT_SIZE_MAX_ARB, &maxSize );

    glPointParameterfARB( GL_POINT_FADE_THRESHOLD_SIZE_ARB, 60.0f );

    glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, 1.0f );
    glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, maxSize );

    // Specify point sprite texture coordinate replacement mode for each 
    // texture unit
    glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );

    glPointSize( 10 );
  }
  else
  {
    LOG_PRINT ("Point spite extensions not supported\n");
  }

  // VBO Extension
  glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) SDL_GL_GetProcAddress("glGenBuffersARB");
  glBindBufferARB = (PFNGLBINDBUFFERARBPROC) SDL_GL_GetProcAddress("glBindBufferARB");
  glBufferDataARB = (PFNGLBUFFERDATAARBPROC) SDL_GL_GetProcAddress("glBufferDataARB");
  glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) SDL_GL_GetProcAddress("glDeleteBuffersARB");

  if (glGenBuffersARB)
  {
    hasVboExt = true;
  }
  else
  {
    hasVboExt = false;
    LOG_PRINT("OTL_Vertex buffer object extensions not supported\n");
  }
#else
#endif
}

void Renderer::getViewport(int& x, int& y, int& w, int& h)
{
#if 0
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  x = viewport[0];
  y = viewport[1];
  w = viewport[2];
  h = viewport[3];
#else
  w = IRenderer::GetRendererInstance()->GetViewportWidth();
  h = IRenderer::GetRendererInstance()->GetViewportHeight();
  x = IRenderer::GetRendererInstance()->GetViewportX();
  y = IRenderer::GetRendererInstance()->GetViewportY();
#endif
}

void Renderer::setViewport(int x, int y, int w, int h)
{
#if 0
  glViewport(x,y,w,h);
#else
  IRenderer::GetRendererInstance()->setViewport(x,y,w,h);
#endif
}

void Renderer::drawMesh(unsigned num_triangles, const Triangle* triangles)
{
#if 0
  if (textured)
  {
    glDepthFunc( GL_LEQUAL );
    glPolygonMode(GL_FRONT, GL_FILL);
    glDrawElements(GL_TRIANGLES, num_triangles*3, GL_UNSIGNED_INT, triangles);
  }

  if (wireframe)
  {
    glColor3f(0.5f,0.4f,0.1f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, num_triangles*3, GL_UNSIGNED_INT, triangles);
  }

  if (points)
  {
    glColor3f(0.5f,0.4f,0.1f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    glDrawElements(GL_TRIANGLES, num_triangles*3, GL_UNSIGNED_INT, triangles);
  }

  for (GLenum x = glGetError(); x != 0; x=glGetError())
    LOG_PRINT("%s\n",(const char*)glGetString(x));
#else
  if(!g_numvertices)
	return;
  if(!num_triangles)
	return;

  IRenderer::GetRendererInstance()->setShader(g_program);
  IRenderer::GetRendererInstance()->setVertexFormat(g_vf);
  D3DXFROMWINEMATRIX g_modelViewProjMatrix=g_view*g_proj;
  D3DXFROMWINEMATRIX g_modelViewMatrix=g_view;
    D3DXFROMWINEMATRIX g_modelViewMatrixI;
    D3DXFROMWINEMATRIX g_modelViewMatrixIT;
  float f=0;
  D3DXFROMWINEMatrixInverse(&g_modelViewMatrixI, &f, &g_modelViewMatrix);
  D3DXFROMWINEMatrixTranspose(&g_modelViewMatrixIT, &g_modelViewMatrixI);
  IRenderer::GetRendererInstance()->setShaderConstant4x4f("ModelViewProj", g_modelViewProjMatrix);
  IRenderer::GetRendererInstance()->setShaderConstant4x4f("ModelView", g_modelViewMatrix);
  IRenderer::GetRendererInstance()->setShaderConstant4x4f("ModelViewI", g_modelViewMatrixI);
  IRenderer::GetRendererInstance()->setShaderConstant4x4f("ModelViewIT", g_modelViewMatrixIT);

  IRenderer::GetRendererInstance()->setShaderConstant4f("LightPos", g_Pos);
  IRenderer::GetRendererInstance()->setShaderConstant4f("lightcolor", g_Diffuse);
  IRenderer::GetRendererInstance()->setShaderConstant4f("LightSpecular", g_Specular);
  IRenderer::GetRendererInstance()->setShaderConstant4f("LightAmbiente", g_Ambient);

  IRenderer::GetRendererInstance()->setTexture("TexDiffuse", TexDiffuse);
  IRenderer::GetRendererInstance()->setTexture("TexNormal", TexNormal);
  IRenderer::GetRendererInstance()->setTexture("TexHeight", TexHeight);

  if(num_triangles)
  IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES,
		0,
		g_numvertices,
		num_triangles,
		triangles,
		triangles,
		CONSTANT_INDEX4,
		vlist,
		vlist,
		sizeof(OTL_Vertex));
  else if(g_numvertices)
  	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, g_numvertices, &vlist, &vlist, sizeof(OTL_Vertex));
#if 1
  LOG_FNLN;
  LOG_PRINT("g_program=%d\n", g_program);
  LOG_PRINT("g_vf=%d\n", g_vf);
#if 0
  stx_Dump4x4f(g_modelViewProjMatrix, "g_modelViewProjMatrix");
  stx_Dump4x4f(g_modelViewMatrix, "g_modelViewMatrix");
  stx_Dump4x4f(g_modelViewMatrixI, "g_modelViewMatrixI");
  stx_Dump4x4f(g_modelViewMatrixIT, "g_modelViewMatrixIT");
  stx_Dump4f(g_Pos, "g_Pos");
  stx_Dump4f(g_Diffuse, "g_Diffuse");
  stx_Dump4f(g_Specular, "g_Specular");
  stx_Dump4f(g_Ambient, "g_Ambient");
#endif
  LOG_PRINT("g_numvertices=%d\n", g_numvertices);
  LOG_PRINT("num_triangles=%d\n", num_triangles);
  LOG_PRINT("triangles=%x\n", triangles);
  LOG_PRINT("vlist=%x\n", vlist);
  LOG_PRINT("sizeof(OTL_Vertex)=%d\n", sizeof(OTL_Vertex));
#endif
#endif
}

void Renderer::setupArray(OTL_Vertex* vertices, const unsigned int numvertices)//, const Tangent* tangents)
{
#if 0
  if (vlist != vertices)
  {
    vlist = vertices;

    if (vertices !=0)
      setupVBOArray(0);

    glVertexPointer(3, GL_FLOAT, sizeof(OTL_Vertex), &vertices[0].x);
    glNormalPointer(GL_FLOAT, sizeof(OTL_Vertex), &vertices[0].nx);
    //glColorPointer(3, GL_FLOAT, sizeof(OTL_Vertex), &vertices[0].r);
    glTexCoordPointer(2, GL_FLOAT, sizeof(OTL_Vertex), &vertices[0].u);

  }
  if (tangents)
  {
    if (tlist != tangents)
    {
      tlist = tangents;
      glClientActiveTextureARB(GL_TEXTURE1_ARB);
      glEnableClientState (GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(3, GL_FLOAT, sizeof(Tangent), &tangents[0].nsx);
      glClientActiveTextureARB(GL_TEXTURE2_ARB);
      glEnableClientState (GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(3, GL_FLOAT, sizeof(Tangent), &tangents[0].ntx);
      glClientActiveTextureARB(GL_TEXTURE0_ARB);
    }
  }
  else
  {
    tlist = 0;
    glClientActiveTextureARB(GL_TEXTURE1_ARB);
    glDisableClientState (GL_TEXTURE_COORD_ARRAY);
    glClientActiveTextureARB(GL_TEXTURE2_ARB);
    glDisableClientState (GL_TEXTURE_COORD_ARRAY);
    glClientActiveTextureARB(GL_TEXTURE0_ARB);
  }
#else
  if (vlist != vertices)
  {
	vlist=vertices;
  	g_numvertices=numvertices;
  }
  //g_tangents=tangents;
#endif
}

void Renderer::setupVBOArray(unsigned int vertices)
{
#if 0
  if (!hasVboExt)
    return;

  if (vertices != 0)
    setupArray(0,0);

  glBindBufferARB( GL_ARRAY_BUFFER_ARB, vertices );
  glVertexPointer( 3, GL_FLOAT, sizeof(OTL_Vertex), (char *) 0 );
  glNormalPointer(GL_FLOAT, sizeof(OTL_Vertex), (char *) (sizeof(float) * 3));
  glTexCoordPointer( 2, GL_FLOAT, sizeof(OTL_Vertex), (char *) (sizeof(float) * 9) );
#elif 0
  vlist=vertices;
#endif
}

unsigned int Renderer::createVBO(OTL_Vertex* vertices, const unsigned int count)
{
  unsigned int buffer=0;
#if 0
  if (!hasVboExt)
    return 0;
  
  // Generate and bind the OTL_Vertex Buffer
  glGenBuffersARB( 1, &buffer );
  glBindBufferARB( GL_ARRAY_BUFFER_ARB, buffer );

  // Load the data
  glBufferDataARB( GL_ARRAY_BUFFER_ARB, count*sizeof(OTL_Vertex), &vertices[0].x, GL_STATIC_DRAW_ARB );
#else
  if(vertices)
  {
  	vlist=vertices;
  	g_numvertices=count;
  }
#endif

  return buffer;
}

void Renderer::setLight(Light* light, unsigned int number)
{
#if 0
  if (light)
  {
    //Enable Light
    glEnable( GL_LIGHT0 + number );

    //Setup The Ambient Light
    glLightfv( GL_LIGHT0 + number, GL_AMBIENT, light->getAmbiente() );

    //Setup The Diffuse Light
    glLightfv( GL_LIGHT0 + number, GL_DIFFUSE, light->getDiffuse() );

    //Setup The Diffuse Light
    glLightfv( GL_LIGHT0 + number, GL_SPECULAR, light->getSpecular() );

    //Position The Light
    glLightfv( GL_LIGHT0 + number, GL_POSITION, light->getPos() );
  }
  else
  {
    glDisable( GL_LIGHT0 + number );
  }
#else
  float* f=light->getAmbiente();
  g_Ambient=D3DXFROMWINEVECTOR4(f[0], f[1], f[2], f[3]);
  f=light->getDiffuse();
  g_Diffuse=D3DXFROMWINEVECTOR4(f[0], f[1], f[2], f[3]);
  f=light->getSpecular();
  g_Specular=D3DXFROMWINEVECTOR4(f[0], f[1], f[2], f[3]);
  f=light->getPos();
  g_Pos=D3DXFROMWINEVECTOR4(f[0], f[1], f[2], f[3]);
#endif
}

void Renderer::getLight(Light* light, unsigned int number)
{
#if 0
  if (light)
  {
    float ambiente[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glGetLightfv(GL_LIGHT0 + number, GL_AMBIENT, ambiente);
    light->setAmbiente(ambiente);

    float diffuse[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glGetLightfv(GL_LIGHT0 + number, GL_DIFFUSE, diffuse);
    light->setDiffuse(diffuse);

    float specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glGetLightfv(GL_LIGHT0 + number, GL_SPECULAR, specular);
    light->setSpecular(specular);

    float pos[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glGetLightfv(GL_LIGHT0 + number, GL_POSITION, pos);
    light->setPos(pos);
  }
#else
    light->setAmbiente(&g_Ambient.x);
    light->setDiffuse(&g_Diffuse.x);
    light->setSpecular(&g_Specular.x);
    light->setPos(&g_Pos.x);
#endif
}

void Renderer::setTexture(OTL_Texture* tex, unsigned int layer)
{
#if 0
  glActiveTextureARB(GL_TEXTURE0_ARB+layer);
  glBindTexture(GL_TEXTURE_2D, tex->id);
#else
  switch(layer)
  {
	case 0:
		TexDiffuse=tex->id;
		break;
	case 1:
		TexNormal=tex->id;
		break;
	case 2:
		TexHeight=tex->id;
		break;
  };
#endif
}

OTL_Texture* Renderer::createTexture(unsigned x, unsigned y)
{
  OTL_Texture* tex = new OTL_Texture();
#if 0
  tex->width = x;
  tex->height = y;

  unsigned int txtnumber;

  glGenTextures(1, &txtnumber);
  glBindTexture(GL_TEXTURE_2D, txtnumber);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  tex->id = txtnumber;
#else
	Image3 img;
	img.create(FORMAT_RGBA8, x, y, 1, 1);
	tex->id=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());
  tex->width = x;
  tex->height = y;
#endif
  return tex;
}

void Renderer::render2Texture(unsigned tex, unsigned x, unsigned y)
{
#if 0
  glBindTexture(GL_TEXTURE_2D,tex);
  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, x, y, 0);
#else
#endif
  LOG_FNLN_;
  LOG_PRINT_("tex=%d\n", tex);
}

void Renderer::renderDepth2Texture(unsigned tex, unsigned x, unsigned y)
{
#if 0
  glBindTexture(GL_TEXTURE_2D,tex);
  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, x, y, 0);
#else
#endif
  LOG_FNLN_;
  LOG_PRINT_("tex=%d\n", tex);
}

void Renderer::clear(float r,float g,float b,float a)
{
#if 0
  glClearColor(r, g, b, a);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
#else
  LOG_FNLN;
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
#endif
}

void Renderer::blit()
{
#if 0
  //SDL_GL_SwapBuffers();
  SDL_GL_SwapWindow(g_SDL_window);
#else
  LOG_FNLN;
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
  //stx_exit(0);
#endif
}

void Renderer::enableLight(bool state)
{
#if 0
  if (state)
    glEnable(GL_LIGHTING);
  else
    glDisable(GL_LIGHTING);
#else
#endif
}

void Renderer::enableTransparency(bool state)
{
#if 0
  if (state) glEnable(GL_BLEND);
  else glDisable(GL_BLEND);
#else
#endif
}

void Renderer::enableAlphaTest(bool state)
{
#if 0
  if (state) glEnable(GL_ALPHA_TEST);
  else glDisable(GL_ALPHA_TEST);
#else
#endif
}

void Renderer::enablePointSprites(bool state)
{
#if 0
  if (state)
  {
    points = true;
    textured = false;
    glEnable(GL_POINT_SPRITE_ARB);
  }
  else 
  {
    points = false;
    textured = true;
    glDisable(GL_POINT_SPRITE_ARB);
  }
#else
#endif
}

void Renderer::sleep(unsigned time)
{
#if 0
  SDL_Delay(time);
#else
  STX_Service::GetTimerInstance()->stx_Delay(time);
#endif
}

void Renderer::updateFPS(const char* name, unsigned vert, unsigned tria)
{
#if 1
  fps++;

  unsigned now = timeGetTime();

  if( timeGetTime() - last > 1000 )
  {
    //titel is limited to 256 (char length
    stx_snprintf(title, 256,"%s FPS: %.2f Vertices: %i Faces: %i", name, fps/(float)(now - last)*1000, vert, tria);
    //SDL_WM_SetCaption(title,title);
	//SDL_SetWindowTitle(g_SDL_window, title);
	STX_Service::GetWindowInstance()->SetCaption(title);
    last = timeGetTime();
    fps = 0;
  }
#else
#endif
}

void Renderer::setPerspective(float fov, float ratio, float nearplane, float farplane)
{
#if 0
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fov, ratio, nearplane, farplane);
#else
  D3DXFROMWINEMatrixPerspectiveFovLH(&g_proj, fov, ratio, nearplane, farplane);
#endif
}

void Renderer::setCamera(opentree::otMatrix aview)
{
#if 0
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glLoadMatrixf(view);
#else
  stx_memcpy(&g_view.m[0][0], &aview.M[0], sizeof(float)*16);
#endif
}

OTL_Texture* Renderer::loadTexture(const char* afilename)
{
  OTL_Texture* tex = new OTL_Texture();
#if 0

  SDL_Surface *img = IMG_Load(filename);

  unsigned texture = 0;

  if(!img) 
  {
    return 0;
  }

  glGenTextures(1, &texture);

  glBindTexture(GL_TEXTURE_2D, texture);

  tex->width = img->w;
  tex->height = img->h;

  if (img->format->BytesPerPixel == 3) // RGB 24bit
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->w, img->h, 0, GL_RGB, GL_UNSIGNED_BYTE, img->pixels);
  }
  else if (img->format->BytesPerPixel == 4) // RGBA 32bit
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);
  }
  else
  {
    SDL_FreeSurface(img);
    return 0;
  }

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  SDL_FreeSurface(img);

  tex->id = texture;
#else
#if 0
  LOG_FNLN;
  std::string filename=std::string("../../IndieLib_resources/")+std::string(afilename);
  LOG_PRINT("filename=%s\n", afilename);
#endif
#if 1
	Image3 img;
	img.loadImageLibImage(afilename);

  tex->width = img.getWidth();
  tex->height = img.getHeight();
  tex->name = afilename;
	tex->id=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());
#else
  tex->id = IRenderer::GetRendererInstance()->addImageLibTexture(afilename, false, IRenderer::GetRendererInstance()->Getlinear());
#endif
#endif
  LOG_FNLN_;
  LOG_PRINT_("tex->id=%d\n", tex->id);
  return tex;
}

