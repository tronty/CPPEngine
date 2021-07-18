#include "precomp.h"
#include "utils.h"
#include <GL\glew.h>
#include <sdl\SDL.h>
#include <string>
#include <sstream>

std::vector<std::string>& split(const std::string&s, char delim, std::vector<std::string>& elems)
{
	std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) 
	{
        elems.push_back(item);
    }
    return elems;
}

GLuint createTexture(const std::string& fileName)
{
	GLuint texture;			// This is a handle to our texture object
	SDL_Surface *surface;	// This surface will tell us the details of the image
	GLenum texture_format;
	GLint  nOfColors;
 
	if ( (surface = SDL_LoadBMP(fileName.c_str())) )
	{ 
 
		// Check that the image's width is a power of 2
		if ( (surface->w & (surface->w - 1)) != 0 ) 
		{
			printf("warning: image.bmp's width is not a power of 2\n");
		}
 
		// Also check if the height is a power of 2
		if ( (surface->h & (surface->h - 1)) != 0 ) 
		{
			printf("warning: image.bmp's height is not a power of 2\n");
		}
 
		// get the number of channels in the SDL surface
		nOfColors = surface->format->BytesPerPixel;
		if (nOfColors == 4)     // contains an alpha channel
		{
				if (surface->format->Rmask == 0x000000ff)
						texture_format = GL_RGBA;
				else
						texture_format = GL_BGRA;
		} else if (nOfColors == 3)     // no alpha channel
		{
				if (surface->format->Rmask == 0x000000ff)
						texture_format = GL_RGB;
				else
						texture_format = GL_BGR;
		} else 
		{
				printf("warning: the image is not truecolor..  this will probably break\n");
				// this error should not go unhandled
		}
 
		// Have OpenGL generate a texture object handle for us
		glGenTextures( 1, &texture );
 
		// Bind the texture object
		glBindTexture( GL_TEXTURE_2D, texture );
 
		// Set the texture's stretching properties
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
 
		// Edit the texture object's image data using the information SDL_Surface gives us
		gluBuild2DMipmaps( GL_TEXTURE_2D, 3, surface->w, surface->h, texture_format, GL_UNSIGNED_BYTE, surface->pixels );
		//glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0,
		//				  texture_format, GL_UNSIGNED_BYTE, surface->pixels );
	} 
	else
	{
		printf("SDL could not load image.bmp: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}
 
	// Free the SDL_Surface only if it was successfully created
	if ( surface )
	{ 
		SDL_FreeSurface( surface );
	}
	return texture;
}

void DrawableVector::draw() const
{
	glColor3f(0,255,0);
	Point3Df pointFinal = m_initPoint;
	pointFinal += this->m_vector;
	glBegin(GL_LINES);
		glVertex3f(m_initPoint.X(), m_initPoint.Y(), m_initPoint.Z());
		glVertex3f(pointFinal.X(), pointFinal.Y(), pointFinal.Z());
	glEnd();
	glColor3f(255,255,255);
}

static void
drawBox(GLfloat size, GLenum type)
{
  static GLfloat n[6][3] =
  {
    {-1.0, 0.0, 0.0},
    {0.0, 1.0, 0.0},
    {1.0, 0.0, 0.0},
    {0.0, -1.0, 0.0},
    {0.0, 0.0, 1.0},
    {0.0, 0.0, -1.0}
  };
  static GLint faces[6][4] =
  {
    {0, 1, 2, 3},
    {3, 2, 6, 7},
    {7, 6, 5, 4},
    {4, 5, 1, 0},
    {5, 6, 2, 1},
    {7, 4, 0, 3}
  };
  GLfloat v[8][3];
  GLint i;

  v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2;
  v[4][0] = v[5][0] = v[6][0] = v[7][0] = size / 2;
  v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
  v[2][1] = v[3][1] = v[6][1] = v[7][1] = size / 2;
  v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
  v[1][2] = v[2][2] = v[5][2] = v[6][2] = size / 2;

  for (i = 5; i >= 0; i--) {
    glBegin(type);
    glNormal3fv(&n[i][0]);
	glTexCoord2f(0,0);
    glVertex3fv(&v[faces[i][0]][0]);
	glTexCoord2f(1,0);
    glVertex3fv(&v[faces[i][1]][0]);
	glTexCoord2f(1,1);
    glVertex3fv(&v[faces[i][2]][0]);
	glTexCoord2f(0,1);
    glVertex3fv(&v[faces[i][3]][0]);
    glEnd();
  }
}

void solidCube(GLdouble size)
{
  drawBox(size, GL_QUADS);
}