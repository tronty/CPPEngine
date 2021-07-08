//-----------------------------------------------------------------------------
//           Name: sdl_opengl_simple.cpp
//         Author: Kevin Harris
//  Last Modified: 08/09/04
//    Description: This sample demonstrates how to use SDL to render 3D scenes
//                 with OpenGL.
//-----------------------------------------------------------------------------
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>

#include <STX/STX.h>
#include <STX/STXCPP.h>
#include <Framework3/IRenderer.h>

//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------
GLuint g_textureID = 0;

float g_fSpinX           = 0.0f;
float g_fSpinY           = 0.0f;
int   g_nLastMousePositX = 0;
int   g_nLastMousePositY = 0;
bool  g_bMousing         = false;

struct Vertex
{
    float tu, tv;
    float x, y, z;
};

Vertex g_quadVertices[] =
{
    { 0.0f,0.0f, -1.0f,-1.0f, 0.0f },
    { 1.0f,0.0f,  1.0f,-1.0f, 0.0f },
    { 1.0f,1.0f,  1.0f, 1.0f, 0.0f },
    { 0.0f,1.0f, -1.0f, 1.0f, 0.0f }
};

struct BMPImage
{
    int   width;
    int   height;
    char *data;
};

//-----------------------------------------------------------------------------
// PROTOTYPES
//-----------------------------------------------------------------------------
int main(int argc, char *argv[]);
void render(void);
void init(void);
void getBitmapImageData(char *pFileName, BMPImage *pImage);
void loadTexture(void);

//-----------------------------------------------------------------------------
// Name: main()
// Desc:
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    SDL_Init( SDL_INIT_VIDEO ); // Initialise the SDL Video bit

    SDL_WM_SetCaption( "SDL + OpenGL", NULL );

    const SDL_VideoInfo *pSDLVideoInfo = SDL_GetVideoInfo();

    if( !pSDLVideoInfo )
    {
        LOG_PRINT("SDL_GetVideoInfo() failed. SDL Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    int nFlags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE;
    
    if( pSDLVideoInfo->hw_available ) // Hardware surfaces enabled?
        nFlags |= SDL_HWSURFACE;
    else
        nFlags |= SDL_SWSURFACE;
  
    if( pSDLVideoInfo->blit_hw ) // Hardware supported blitting?
        nFlags |= SDL_HWACCEL;
  
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 ); // Enable OpenGL Doublebuffering
  
    // Create our rendering surface
    SDL_Surface *pSDLSurface = SDL_SetVideoMode( 800, 600, 32, nFlags );
  
    if( !pSDLSurface )
    {
        LOG_PRINT( "Call to SDL_SetVideoMode() failed! - SDL_Error: %s\n", SDL_GetError() );
        SDL_Quit();
        return 1;
    }

    // Init OpenGL...
    init();

    bool bDone = false;
    SDL_Event Event;

    while( !bDone )
    {
        SDL_PollEvent( &Event );

        switch( Event.type )
        {
            case SDL_QUIT:
                bDone = true;
                break;

            case SDL_MOUSEBUTTONDOWN:
            {
                switch( Event.button.button )
                {
                    case SDL_BUTTON_LEFT:
                        g_nLastMousePositX = Event.motion.x;
                        g_nLastMousePositY = Event.motion.y;
                        g_bMousing = true;
                        break;
                }
            }
            break;
                
            case SDL_MOUSEBUTTONUP:
            {
                switch( Event.button.button )
                {
                  case SDL_BUTTON_LEFT:
                        g_bMousing = false;
                        break;
                }
            }
            break;
       
            case SDL_MOUSEMOTION:
            {

            }
            break;
                
            case SDL_KEYDOWN:
            {
                if( Event.key.keysym.sym == SDLK_ESCAPE )
                {
                    bDone = true;
                    break;
                }
            }
            
            default: break;
        }

        render();
    }

    SDL_Quit();

    return 0;
}

//-----------------------------------------------------------------------------
// Name: init()
// Desc: Init OpenGL context for rendering.
//-----------------------------------------------------------------------------
void init( void )
{
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glEnable( GL_TEXTURE_2D );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 45.0f, 640.0f / 480.0f, 0.1f, 100.0f);
 
    loadTexture();
}

//-----------------------------------------------------------------------------
// Name: getBitmapImageData()
// Desc: Simply image loader for 24 bit BMP files.
//-----------------------------------------------------------------------------
void getBitmapImageData( char *pFileName, BMPImage *pImage )
{
    FILE *pFile = NULL;
    unsigned short nNumPlanes;
    unsigned short nNumBPP;
    int i;

    if( (pFile = fopen(pFileName, "rb") ) == NULL )
        LOG_PRINT("ERROR: getBitmapImageData - %s not found\n",pFileName);

    // Seek forward to width and height info
    fseek( pFile, 18, SEEK_CUR );

    if( (i = fread(&pImage->width, 4, 1, pFile) ) != 1 )
      LOG_PRINT("ERROR: getBitmapImageData - Couldn't read width from %s.\n", pFileName);

    if( (i = fread(&pImage->height, 4, 1, pFile) ) != 1 )
        LOG_PRINT("ERROR: getBitmapImageData - Couldn't read height from %s.\n", pFileName);

    if( (fread(&nNumPlanes, 2, 1, pFile) ) != 1 )
       LOG_PRINT("ERROR: getBitmapImageData - Couldn't read plane count from %s.\n", pFileName);
 
    if( nNumPlanes != 1 )
     LOG_PRINT( "ERROR: getBitmapImageData - Plane count from %s is not 1: %u\n", pFileName, nNumPlanes );

    if( (i = fread(&nNumBPP, 2, 1, pFile)) != 1 )
      LOG_PRINT( "ERROR: getBitmapImageData - Couldn't read BPP from %s.\n", pFileName );
   
    if( nNumBPP != 24 )
       LOG_PRINT( "ERROR: getBitmapImageData - BPP from %s is not 24: %u\n", pFileName, nNumBPP );

    // Seek forward to image data
    fseek( pFile, 24, SEEK_CUR );

    // Calculate the image's total size in bytes. Note how we multiply the 
    // result of (width * height) by 3. This is becuase a 24 bit color BMP 
    // file will give you 3 bytes per pixel.
    int nTotalImagesize = (pImage->width * pImage->height) * 3;

    pImage->data = (char*) malloc( nTotalImagesize );
    
    if( (i = fread(pImage->data, nTotalImagesize, 1, pFile) ) != 1 )
      LOG_PRINT("ERROR: getBitmapImageData - Couldn't read image data from %s.\n", pFileName);

    //
    // Finally, rearrange BGR to RGB
    //
 
    char charTemp;
    for( i = 0; i < nTotalImagesize; i += 3 )
    { 
        charTemp = pImage->data[i];
        pImage->data[i] = pImage->data[i+2];
        pImage->data[i+2] = charTemp;
    }
}

//-----------------------------------------------------------------------------
// Name: loadTexture()
// Desc: 
//-----------------------------------------------------------------------------
void loadTexture( void )    
{
    BMPImage textureImage;
 
    getBitmapImageData( "test.bmp", &textureImage );

    glGenTextures( 1, &g_textureID );
    glBindTexture( GL_TEXTURE_2D, g_textureID );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    glTexImage2D( GL_TEXTURE_2D, 0, 3, textureImage.width, textureImage.height, 
                  0, GL_RGB, GL_UNSIGNED_BYTE, textureImage.data );
}

//-----------------------------------------------------------------------------
// Name: render()
// Desc: Called when the SDL window is ready to render
//-----------------------------------------------------------------------------
void render( void )
{
    if( g_bMousing )
    {
        int x;
        int y;
        //SDL_PumpEvents();
        SDL_GetMouseState( &x, &y );

        g_fSpinX -= (x - g_nLastMousePositX);
        g_fSpinY -= (y - g_nLastMousePositY);

        g_nLastMousePositX = x;
        g_nLastMousePositY = y;
    }
 
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatef( 0.0f, 0.0f, -5.0f );
    glRotatef( -g_fSpinY, 1.0f, 0.0f, 0.0f );
    glRotatef( -g_fSpinX, 0.0f, 1.0f, 0.0f );
    
    glBindTexture( GL_TEXTURE_2D, g_textureID );
    glInterleavedArrays( GL_T2F_V3F, 0, g_quadVertices );
    glDrawArrays( GL_QUADS, 0, 4 );
#if defined(SDL_2_0_5_BUILD)
	SDL_GL_SwapWindow(STX_Service::GetSDLWindow());
#elif defined(SDL_1_2_15_BUILD)
	SDL_GL_SwapBuffers();
#endif
}
