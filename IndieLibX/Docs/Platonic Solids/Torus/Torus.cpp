typedef struct Model_t *       Model;

struct Model_t
{
    int        vertex, index;
    float *    vertices;
    float *    tex;
    float *    normals;
    uint16_t * indices;
};
void vecNormalize(float A[3])
{
    float len = 1 / sqrt(A[0]*A[0] + A[1]*A[1] + A[2]*A[2]);
    A[0] *= len;
    A[1] *= len;
    A[2] *= len;
}
#define D_TO_R     (M_PI/180)
Model modelGenTorus(int sides, int cs_sides, float radius, float cs_radius)
{
    int numVertices = (sides+1) * (cs_sides+1);
    int numIndices = (2*sides+4) * cs_sides;

    Model torus = malloc(sizeof *torus + numVertices * 8 * sizeof (float) + numIndices * 2);
    float * Vertices;
    float * TexCoord;
    uint16_t * Indices;

    torus->vertices = Vertices = (float *) (torus + 1);
    torus->normals  = Normals  = Vertices + numVertices * 3;
    torus->tex      = TexCoord = Normals + numVertices * 3;
    torus->indices  = Indices  = (uint16_t *) (torus->tex + numVertices * 2);
    torus->index    = numIndices;
    torus->vertex   = numVertices;

    int angleincs = 360/sides;
    int cs_angleincs = 360/cs_sides;
    float currentradius, zval;
    int i, j, nextrow;

    /* iterate cs_sides: inner ring */
    for (j = 0; j <= 360; j += cs_angleincs)
    {
        currentradius = radius + (cs_radius * cosf(j * D_TO_R));
        zval = cs_radius * sinf(j * D_TO_R);

        /* iterate sides: outer ring */
        for (i = 0; i <= 360; i += angleincs, Vertices += 3, TexCoord += 2)
        {
            Vertices[0] = currentradius * cosf(i * D_TO_R);
            Vertices[1] = currentradius * sinf(i * D_TO_R);
            Vertices[2] = zval;

            float u = i / 360.;
            float v = 2. * j / 360 - 1;
            if (v < 0) v = -v;

            TexCoord[0] = u;
            TexCoord[1] = v;
        }
    }

    /* compute normals: loops are swapped */
    for (i = 0, nextrow = (sides+1) * 3, Vertices = torus->vertices; i <= 360; i += angleincs, Normals += 3, Vertices += 3)
    {
        float * vert;
        float * norm;
        float xc = radius * cos(i * D_TO_R);
        float yc = radius * sin(i * D_TO_R);
        for (j = 0, vert = Vertices, norm = Normals; j <= 360; j += cs_angleincs, norm += nextrow, vert += nextrow)
        {
            norm[0] = vert[0] - xc;
            norm[1] = vert[1] - yc;
            norm[2] = vert[2];
            vecNormalize(norm);
        }
    }

    /* indices grouped by GL_TRIANGLE_STRIP, face oriented clock-wise */

    /* inner ring */
    for (i = 0, nextrow = sides + 1; i < cs_sides; i ++)
    {
        /* outer ring */
        for (j = 0; j < sides; j ++)
        {
            *Indices++ = i * nextrow + j;
            *Indices++ = (i + 1) * nextrow + j;
        }

        /* generate dummy triangle to avoid messing next ring */
        Indices[0] = i * nextrow + j;
        Indices[1] = Indices[2] = Indices[3] = Indices[0] + nextrow;
        Indices += 4;
    }
    return torus;
}
