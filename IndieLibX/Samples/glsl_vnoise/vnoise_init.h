#ifndef VNOISE_H_INCLUDED
#define VNOISE_H_INCLUDED

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include <assert.h>
#include <math.h>
#include <iostream>
#include <string.h>

using namespace std;

#include "importgl.h"

void initVertexNoiseConstants(int table_size);

namespace glh {
	struct vec2f
	{
		vec2f()
		{
			x=0.0f;
			y=0.0f;
		}
		float x,y;
	};
	struct vec3f
	{
		vec3f()
		{
			x=0.0f;
			y=0.0f;
			z=0.0f;
		}
		vec3f(float a)
		{
			x=a;
			y=a;
			z=a;
		}
		float x,y,z;
	};
	struct vec4f
	{
		vec4f()
		{
			x=0.0f;
			y=0.0f;
			z=0.0f;
			w=0.0f;
		}
		float x,y,z,w;
		void normalize()
		{
    			float norm = sqrt( (x) * (x) + (y) * (y) + (z) * (z) + (w) * (w) );
    			x /= norm;
    			y /= norm;
    			z /= norm;
    			w /= norm;
		}
	};
};

#endif

