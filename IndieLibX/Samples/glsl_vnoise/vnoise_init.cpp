#include <math.h>
#include <stdlib.h>
#include <stdio.h>

//include <glh/glh_extensions.h>
//include <glh/glh_linear.h>
#include "vnoise_init.h"
extern GLint permGradTableParam;

float sfrand()
{
    return (rand() * 2.0f / (float) RAND_MAX) - 1.0f;
}

// build permuation/gradient table for noise
void initVertexNoiseConstants(int table_size)
{
	int *p = new int[table_size*2+2];                // permutation table
    glh::vec4f *g = new glh::vec4f[table_size*2+2];  // gradient table
	int i;

    // initalize random gradients
	for(i=0; i<table_size; i++) {
		p[i] = i;
		g[i].x = sfrand();
		g[i].y = sfrand();
		g[i].z = sfrand();
        g[i].normalize();
	}

	// initialize permutation table (random shuffle)
	for(i=0; i<table_size; i++) {
		int j, t;
		j = (rand() >> 4) % table_size;
		t = p[i];
		p[i] = p[j];
		p[j] = t;

        g[i].w = (float) p[i];

        // mirror first half of table into second half
        g[i+table_size].x = g[i].x;
        g[i+table_size].y = g[i].y;
        g[i+table_size].z = g[i].z;
        g[i+table_size].w = g[i].w;
	}

	#if 1
    glUniform4fvARB(permGradTableParam, table_size*2, &g[0].x);
    glUniform4fvARB(permGradTableParam + (table_size*2), 1, &g[0].x);
    glUniform4fvARB(permGradTableParam + (table_size*2)+1, 1, &g[1].x);
	#endif
}
