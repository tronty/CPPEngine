#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include <glh/glh_extensions.h>
#include <glh/glh_linear.h>

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
		g[i][0] = sfrand();
		g[i][1] = sfrand();
		g[i][2] = sfrand();
        g[i].normalize();
	}

	// initialize permutation table (random shuffle)
	for(i=0; i<table_size; i++) {
		int j, t;
		j = (rand() >> 4) % table_size;
		t = p[i];
		p[i] = p[j];
		p[j] = t;

        g[i][3] = (float) p[i];

        // mirror first half of table into second half
        g[i+table_size][0] = g[i][0];
        g[i+table_size][1] = g[i][1];
        g[i+table_size][2] = g[i][2];
        g[i+table_size][3] = g[i][3];
	}

    glUniform4fvARB(permGradTableParam, table_size*2, g[0].v);
    glUniform4fvARB(permGradTableParam + (table_size*2), 1, g[0].v);
    glUniform4fvARB(permGradTableParam + (table_size*2)+1, 1, g[1].v);
}
