/* 
  Functions to create OpenGL textures containing pre-filtered noise patterns for procedural texturing.

  Pre-filtering the texture with a bicubic filter avoids some of the artifacts associated with linear filtering,
  and allows us to pre-compute the abs() function required for Perlin-style turbulence.

  sgreen@nvidia.com 7/2000
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//include <glh/glh_extensions.h>

#include <STX/STX.h>
#include <STX/STXCPP.h>
#include <STX/FilteredNoise.h>
#include <STX/Matrix.h>
/*
// clamp x to be between a and b
float clamp(float x, float a, float b)
{
    return (x < a ? a : (x > b ? b : x));
}
*/
// a nice piecewise-cubic spline function, defined between 0.0<=x<=2.0
// approximates a windowed sinc function - negative lobes (sharpens slightly)
// a = contraint parameter -0.5<=a<=-1.0 recommended, -0.75 is good
float cubic(float x, float a)
{
  double w;

  if ((x >= 0.0) && (x < 1.0)) {
	// Over [0,1) interval
    // (a+2)x^3 - (a+3)x^2 + 1
    w = ((a + 2.0) * x - (a + 3.0)) * x * x + 1.0;
  } else if ((x >= 1.0) && (x <= 2.0)) {
	// Over [1,2] interval
    // ax^3 - 5ax^2 + 8ax - 4a
    w = ((a * x - 5.0 * a) * x + 8.0 * a) * x - 4.0 * a;
  } else {
    w = 0.0;
  }
  return (float)w;
}


#define A	-0.75		// constraint parameter

// 1-dimensional cubic interpolation
// not very fast, but easy to understand
// 0<=x<=1
float cubicFilter4f(float x, float vm1, float v, float vp1, float vp2)
{
  return vm1 * cubic(1.0f+x, A) +
		 v   * cubic(x, A) +
		 vp1 * cubic(1.0f-x, A) +
		 vp2 * cubic(2.0f-x, A);
}

float cubicFilter4fv(float x, float *v)
{
  return v[0] * cubic(1.0f+x, A) +
		 v[1] * cubic(x, A) +
		 v[2] * cubic(1.0f-x, A) +
		 v[3] * cubic(2.0f-x, A);
}

/*
  1D cubic interpolator (this is just a faster version of the above)
  assumes x > 0.0
  constraint parameter = -1
*/
float cubicFilterFast4fv(float x, float *i)
{
  float x1, x2, x3;
  float h1, h2, h3, h4;
  float result;

  x1 = x + 1.0f;
  x2 = x1 * x1;
  x3 = x2 * x1;
  h4 = -x3 + 5 * x2 - 8 * x1 + 4;
  result = h4 * i[0];

  x1 = x;
  x2 = x1 * x1;
  x3 = x2 * x1;
  h1 = x3 - 2 * x2 + 1;
  result += h1 * i[1];

  x1 = 1.0f - x;
  x2 = x1 * x1;
  x3 = x2 * x1;
  h2 = x3 - 2 * x2 + 1;
  result += h2 * i[2];

  x1 = 2.0f - x;
  x2 = x1 * x1;
  x3 = x2 * x1;
  h3 = -x3 + 5 * x2 - 8 * x1 + 4;
  result += h3 * i[3];

  return result;
}

// distance from a point
float distance(float x, float y, float z, float cx, float cy, float cz)
{
  float dx = x-cx;
  float dy = y-cy;
  float dz = y-cz;
  return (float)sqrt(dx*dx + dy*dy + dz*dz);
}

// create a 3d table of random floating point values between -1.0 and 1.0
// width, height, depth must be powers of two
FilteredNoise::FilteredNoise(int w, int h, int d)
{
  m_w = w;
  m_h = h;
  m_d = d;

  m_noise = new float[w * h * d];

  float *ptr = m_noise;
  for(int i=0; i<w*h*d; i++) {
    *ptr++ = (2.0f * rand() / (float) RAND_MAX) - 1.0f;
  }
}

FilteredNoise::~FilteredNoise()
{
	if(m_noise)
		delete [] m_noise;
}

// get a repeatable random value for a given integer position
// returns random float between -1.0 and 1.0
// nb - lots of other ways to do this (hash functions etc.)
float
FilteredNoise::noise2D(int x, int y)
{
  x = x & (m_w - 1);
  y = y & (m_h - 1);

  return m_noise[(y*m_w) + x];
}

float
FilteredNoise::noise3D(int x, int y, int z)
{
  x = x & (m_w - 1);
  y = y & (m_h - 1);
  z = z & (m_d - 1);

  return m_noise[(z*m_w*m_h) + (y*m_w) + x];
}

#define FLOOR(x) ((int)(x) - ((x) < 0 && (x) != (int)(x)))

// 2D bicubic-filtered noise
// filters 16 points
// returns float between -1.0 and 1.0
float
FilteredNoise::bicubicNoise2D(float x, float y)
{
  int ix = FLOOR(x);
  float fx = x - ix;
  int iy = FLOOR(y);
  float fy = y - iy;

  float r = cubicFilter4f(fy,
            cubicFilter4f(fx, noise2D(ix-1, iy-1), noise2D(ix, iy-1), noise2D(ix+1, iy-1), noise2D(ix+2, iy-1)),
            cubicFilter4f(fx, noise2D(ix-1, iy),   noise2D(ix, iy),   noise2D(ix+1, iy),   noise2D(ix+2, iy)),
            cubicFilter4f(fx, noise2D(ix-1, iy+1), noise2D(ix, iy+1), noise2D(ix+1, iy+1), noise2D(ix+2, iy+1)),
            cubicFilter4f(fx, noise2D(ix-1, iy+2), noise2D(ix, iy+2), noise2D(ix+1, iy+2), noise2D(ix+2, iy+2)) );

  return clamp(r, -1.0, 1.0);
}


// 3D bicubic-filtered noise
// filters between 64 points
float
FilteredNoise::bicubicNoise3D(float x, float y, float z)
{
  int ix = FLOOR(x);
  float fx = x - ix;
  int iy = FLOOR(y);
  float fy = y - iy;
  int iz = FLOOR(z);
  float fz = z - iz;

  float xknots[4], yknots[4], zknots[4];

  for (int k = -1; k <= 2; k++) {
    for (int j = -1; j <= 2; j++) {
      for (int i = -1; i <= 2; i++) {
        xknots[i+1] = noise3D(ix+i, iy+j, iz+k);
	  }
      yknots[j+1] = cubicFilterFast4fv(fx, xknots);
    }
    zknots[k+1] = cubicFilterFast4fv(fy, yknots);
  }
  float r = cubicFilterFast4fv(fz, zknots);

  return clamp(r, -1.0f, 1.0f);
}


// Create an OpenGL 3D texture containing filtered 3D (un-normalized vector) noise
void
FilteredNoise::createVectorNoiseTexture3D(int w, int h, int d, float scale, float amp, NoiseMode noiseMode)
{
  float *img = new float[w * h * d * 3];

  float *p = img;
  for(int i=0; i<d; i++) {
	for(int j=0; j<h; j++) {
	  for(int k=0; k<w; k++) {

		float fx = (k / scale) - 0.5f;
		float fy = (j / scale) - 0.5f;
		float fz = (i / scale) - 0.5f;

		switch(noiseMode) {
		case RAW:
		  // raw unfiltered noise
		  *p++ = amp * noise3D((int)fx,  (int)fy,  (int)fz);
		  *p++ = amp * noise3D((int)fx, -(int)fy,  (int)fz);
		  *p++ = amp * noise3D((int)fx,  (int)fy, -(int)fz);
		  break;

        case NOISE:
		  // filtered noise
		  *p++ = bicubicNoise3D(fx,  fy,  fz);
		  *p++ = bicubicNoise3D(fx, -fy,  fz);
		  *p++ = bicubicNoise3D(fx,  fy, -fz);
		  break;

		case ABSNOISE:
		  // absolute value of filtered noise (for turbulence)
	 	  *p++ = fabsf(bicubicNoise3D(fx,  fy,  fz));
	 	  *p++ = fabsf(bicubicNoise3D(fx, -fy,  fz));
	 	  *p++ = fabsf(bicubicNoise3D(fx,  fy, -fz));
		  break;

        default:
          LOG_PRINT("Noise mode not supported\n");
		}
	  }
	}
  }
#if 0
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glTexImage3D(GL_TEXTURE_3D, 0, GL_SIGNED_RGB_NV, w, h, d, 0, GL_RGB, GL_FLOAT, img);
#else
#endif

  delete [] img;
}

// Create an OpenGL 3D texture containing filtered noise
unsigned int
FilteredNoise::createNoiseTexture3D(int w, int h, int d, float scale, float amp, NoiseMode noiseMode)
{
  ubyte *img = new ubyte[w * h * d];

  ubyte *p = img;
  for(int i=0; i<d; i++) {
	for(int j=0; j<h; j++) {
	  for(int k=0; k<w; k++) {

		float fx = (k / scale) - 0.5f;
		float fy = (j / scale) - 0.5f;
		float fz = (i / scale) - 0.5f;

        float n;

		switch(noiseMode) {
		case RAW:
		  // raw unfiltered noise
		  n = (noise3D((int)fx, (int)fy, (int)fz) + 1.0f) * 0.5f;


		  break;

        case NOISE:
		  // filtered noise
		  n = (bicubicNoise3D(fx, fy, fz) + 1.0f) * 0.5f;


		  break;

		case ABSNOISE:
		  // absolute value of filtered noise (for turbulence)
	 	  n = (float)(fabs(bicubicNoise3D(fx, fy, fz)));
		  break;

		case VEINS:
		  // scaled filtered noise - gives cheesy "Star Trek" type effect
		  n = 1.0f - 2.0f * (float)(fabs(bicubicNoise3D(fx, fy, fz)));
		  break;

		case DISTANCE:
		  // distance from origin
		  n = distance(fx / (float) w, fy / (float) h, fz / (float) d, 0.5f, 0.5f, 0.5f) * 2.0f;
		  break;

        default:
          LOG_PRINT("Noise mode not supported\n");
		  delete [] img;
          return 0;
		}

		n = clamp(n, 0.0, 1.0f);
		*p++ = (ubyte) (n * amp * 0xff);
	  }
	}
  }

  unsigned int texid;
#if 0
  glGenTextures(1, &texid);
  glBindTexture(GL_TEXTURE_3D, texid);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  if (glTexImage3D != NULL)
    glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, w, h, d, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, img);
#else
	texid=-1;
#endif
  delete [] img;

  return texid;
}
