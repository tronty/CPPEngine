#ifndef __NOISE_EX__
#define __NOISE_EX__

#include <STX/STX.h>
#include <Framework3/CrackFVF.h>
#include <STX/d3dx9mathFromWine.h>
#include <Framework3/Colors.h>
#include <STX/Matrix.h>
#include <STX/STXinterfaces.h>
#include <Framework3/noise.h>

//
/// Extended noise func
//
class FilteredNoiseEx : public FilteredNoise
{
public:
    FilteredNoiseEx(int w, int h, int d) : FilteredNoise(w, h, d) {}
    unsigned int createNoiseTexture3D(int w, int h, int d, float scale, float amp, NoiseMode noiseMode, UBYTE *p)
    {
      //UBYTE *img = new UBYTE[w * h * d];

      //UBYTE *p = img;
      //PROGRESSMSG("computing the 3D noise texture...");
      //PROGRESSSHOW();
      for(int i=0; i<d; i++) {
        //PROGRESSSET((float)i*100.0f/(float)d);
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
                extern float distance(float x, float y, float z, float cx, float cy, float cz);
              n = distance(fx / (float) w, fy / (float) h, fz / (float) d, 0.5f, 0.5f, 0.5f) * 2.0f;
              break;

            default:
              fprintf(stderr, "Noise mode not supported\n");
              return 0;
            }

            extern float clamp(float x, float a, float b);
            n = clamp(n, 0.0, 1.0f);
            *p++ = (UBYTE) (n * amp * 0xff);
          }
        }
      }
#if 0
        m_noisetex.bind();
        m_noisetex.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        m_noisetex.parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        m_noisetex.parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
        m_noisetex.parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
        m_noisetex.parameter(GL_TEXTURE_WRAP_R, GL_REPEAT);

      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      if (glTexImage3D != NULL)
        glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, w, h, d, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, img);
#else
#endif

      //delete [] img;

      //PROGRESSHIDE();
      return -1;//m_noisetex.texture;
    }
	// Create an OpenGL 3D texture containing filtered 3D (un-normalized vector) noise
	void createVectorNoiseTexture3D(int w, int h, int d, float scale, float amp, NoiseMode noiseMode, UBYTE *p)
	{
	  //float *img = new float[w * h * d * 3];

      //PROGRESSMSG("computing the 3D noise RGB texture...");
      //PROGRESSSHOW();
	  //float *p = img;
	  for(int i=0; i<d; i++) {
        //PROGRESSSET((float)i*100.0f/(float)d);
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
			  fprintf(stderr, "Noise mode not supported\n");
			}
		  }
		}
	  }
#if 0
        m_noisetex.bind();
        m_noisetex.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        m_noisetex.parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        m_noisetex.parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
        m_noisetex.parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
        m_noisetex.parameter(GL_TEXTURE_WRAP_R, GL_REPEAT);

      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      if (glTexImage3D != NULL)
		  glTexImage3D(GL_TEXTURE_3D, 0, GL_SIGNED_RGB_NV, w, h, d, 0, GL_RGB, GL_FLOAT, img);
#else
#endif

      //PROGRESSHIDE();
	  //delete [] img;
	}

};
#define FLOOR(x) ((int)(x) - ((x) < 0 && (x) != (int)(x)))
#define LERP3D(dst,v1,v2,t)\
	dst[0] = v1[0]*(1.0f-t) + v2[0]*t;\
	dst[1] = v1[1]*(1.0f-t) + v2[1]*t;\
	dst[2] = v1[2]*(1.0f-t) + v2[2]*t;
/*----------------------------------------------------------------------------------*/ /**



**/ //----------------------------------------------------------------------------------
class CPerlinNoise
{
public:
	enum PerlinMode {
	  RAW = 0,  // filtered noise
	  ABSNOISE,   // absolute value noise
	  VEINS,      // scale noise
	  VEINS2      // scale noise
	};
	typedef float Vec3[3];
	CPerlinNoise()
	{
		permute = NULL;
		pg = NULL;
		loadPermuteTable();
	}
	~CPerlinNoise()
	{
		if(pg)
			delete [] pg;
		if(permute)
			delete [] permute;
		pg = NULL;
		permute = NULL;
	}
	float sfrand()
	{
		return (float) (rand() * 2.0/ (float) RAND_MAX) - 1.0;
	}
	float s_curve(float t)
	{
	  return t*t*( 3.0f - 2.0f*t);
	}
	float eval3d(float x, float y, float z)
	{
		//x *= (float)table_size;
		//y *= (float)table_size;
		//z *= (float)table_size;
		Vec3 vfrac;
		vfrac[0] = x - FLOOR(x); 
		vfrac[1] = y - FLOOR(y); 
		vfrac[2] = z - FLOOR(z); 
		int ii[3];
		ii[0] = (int)(x+10000.0) & 0x1F;
		ii[1] = (int)(y+10000.0) & 0x1F; 
		ii[2] = (int)(z+10000.0) & 0x1F; 
		//
		// lookup in permutation table
		//
		int p[2];
		p[0] = permute[ ii[0]     ];
		p[1] = permute[ ii[0] + 1 ];
		p[0] = p[0] + ii[1];
		p[1] = p[1] + ii[1];

		int b[4];
		b[0] = permute[ p[0] ];
		b[1] = permute[ p[1] ];
		b[2] = permute[ p[0] + 1 ];
		b[3] = permute[ p[1] + 1 ];
		b[0] = b[0] + ii[2];
		b[1] = b[1] + ii[2];
		b[2] = b[2] + ii[2];
		b[3] = b[3] + ii[2];
		//
		// compute dot products between gradients and vectors
		//
		float r[4];
		r[0] = pg[ b[0] ][0]*(vfrac[0]-0) + pg[ b[0] ][1]*(vfrac[1]-0) + pg[ b[0] ][2]*(vfrac[2]-0);
		r[1] = pg[ b[1] ][0]*(vfrac[0]-1) + pg[ b[1] ][1]*(vfrac[1]-0) + pg[ b[1] ][2]*(vfrac[2]-0);
		r[2] = pg[ b[2] ][0]*(vfrac[0]-0) + pg[ b[2] ][1]*(vfrac[1]-1) + pg[ b[2] ][2]*(vfrac[2]-0);
		r[3] = pg[ b[3] ][0]*(vfrac[0]-1) + pg[ b[3] ][1]*(vfrac[1]-1) + pg[ b[3] ][2]*(vfrac[2]-0);

		float r1[4];
		r1[0] = pg[ b[0] + 1 ][0]*(vfrac[0]-0) + pg[ b[0] + 1 ][1]*(vfrac[1]-0) + pg[ b[0] + 1 ][2]*(vfrac[2]-1);
		r1[1] = pg[ b[1] + 1 ][0]*(vfrac[0]-1) + pg[ b[1] + 1 ][1]*(vfrac[1]-0) + pg[ b[1] + 1 ][2]*(vfrac[2]-1);
		r1[2] = pg[ b[2] + 1 ][0]*(vfrac[0]-0) + pg[ b[2] + 1 ][1]*(vfrac[1]-1) + pg[ b[2] + 1 ][2]*(vfrac[2]-1);
		r1[3] = pg[ b[3] + 1 ][0]*(vfrac[0]-1) + pg[ b[3] + 1 ][1]*(vfrac[1]-1) + pg[ b[3] + 1 ][2]*(vfrac[2]-1);

		// interpolate
		vfrac[0] = s_curve(vfrac[0]);
		vfrac[1] = s_curve(vfrac[1]);
		vfrac[2] = s_curve(vfrac[2]);
		float t1,t2;
		t1 = (1.0f-vfrac[2]);
		t2 = vfrac[2];
		r[0] = r[0]*t1 + r1[0]*t2;
		r[1] = r[1]*t1 + r1[1]*t2;
		r[2] = r[2]*t1 + r1[2]*t2;
		r[3] = r[3]*t1 + r1[3]*t2;

		t1 = (1.0f-vfrac[1]);
		t2 = vfrac[1];
		r[0] = r[0]*t1 + r[2]*t2;
		r[1] = r[1]*t1 + r[3]*t2;
		//r[2] = r[1]*t1 + r[3]*t2;
		//r[3] = r[1]*t1 + r[3]*t2;
		t1 = (1.0f-vfrac[0]);
		t2 = vfrac[0];
		r[0] = r[0]*t1 + r[1]*t2;

		return r[0];
	}
	void eval3d(float x, float y, float z, float *vres)
	{
		x *= (float)table_size;
		y *= (float)table_size;
		z *= (float)table_size;
		Vec3 vfrac;
		vfrac[0] = x - FLOOR(x); 
		vfrac[1] = y - FLOOR(y); 
		vfrac[2] = z - FLOOR(z); 
		int ii[3];
		ii[0] = (int)(x); 
		ii[1] = (int)(y); 
		ii[2] = (int)(z); 
		//
		// lookup in permutation table
		//
		int p[2];
		p[0] = permute[ ii[0]     ];
		p[1] = permute[ ii[0] + 1 ];
		p[0] = p[0] + ii[1];
		p[1] = p[1] + ii[1];

		int b[4];
		b[0] = permute[ p[0] ];
		b[1] = permute[ p[1] ];
		b[2] = permute[ p[0] + 1 ];
		b[3] = permute[ p[1] + 1 ];
		b[0] = b[0] + ii[2];
		b[1] = b[1] + ii[2];
		b[2] = b[2] + ii[2];
		b[3] = b[3] + ii[2];
		//
		// Find the 8 gradients of the cube in which we are
		//
		Vec3 grad[8];
		grad[0][0] = pg[ b[0] ][0];		//(0,0,0)
		grad[0][1] = pg[ b[0] ][1];
		grad[0][2] = pg[ b[0] ][2];
		grad[1][0] = pg[ b[1] ][0];		//(1,0,0)
		grad[1][1] = pg[ b[1] ][1];
		grad[1][2] = pg[ b[1] ][2];
		grad[2][0] = pg[ b[2] ][0];		//(0,1,0)
		grad[2][1] = pg[ b[2] ][1];
		grad[2][2] = pg[ b[2] ][2];
		grad[3][0] = pg[ b[3] ][0];		//(1,1,0)
		grad[3][1] = pg[ b[3] ][1];
		grad[3][2] = pg[ b[3] ][2];
		grad[4][0] = pg[ b[0]+1 ][0];	//(0,0,1)
		grad[4][1] = pg[ b[0]+1 ][1];
		grad[4][2] = pg[ b[0]+1 ][2];
		grad[5][0] = pg[ b[1]+1 ][0];	//(1,0,1)
		grad[5][1] = pg[ b[1]+1 ][1];
		grad[5][2] = pg[ b[1]+1 ][2];
		grad[6][0] = pg[ b[2]+1 ][0];	//(0,1,1)
		grad[6][1] = pg[ b[2]+1 ][1];
		grad[6][2] = pg[ b[2]+1 ][2];
		grad[7][0] = pg[ b[3]+1 ][0];	//(1,1,1)
		grad[7][1] = pg[ b[3]+1 ][1];
		grad[7][2] = pg[ b[3]+1 ][2];
		//
		// interpolate
		//
		vfrac[0] = s_curve(vfrac[0]);
		vfrac[1] = s_curve(vfrac[1]);
		vfrac[2] = s_curve(vfrac[2]);
		Vec3 v1, v4, v2, v3;
		LERP3D(v1, grad[0], grad[1], vfrac[0] ); // (0,0,0) <> (1,0,0) : X axis
		LERP3D(v2, grad[2], grad[3], vfrac[0] ); // (0,1,0) <> (1,1,0) : X axis
		LERP3D(v3, v1, v2, vfrac[1] );			// Y axis
		LERP3D(v1, grad[4], grad[5], vfrac[0] ); // (0,0,1) <> (1,0,1) : X axis
		LERP3D(v2, grad[6], grad[7], vfrac[0] ); // (0,1,1) <> (1,1,1) : X axis
		LERP3D(v4, v1, v2, vfrac[1] );			// Y axis
		LERP3D(vres, v3, v4, vfrac[2] );		// Z axis
	}
	/*----------------------------------------------------------------------------------*/ /**
	
	build permuation/gradient table for noise
	
	**/ //----------------------------------------------------------------------------------
	void loadPermuteTable()
	{
		table_size = 32;
		srand(42);
		if(pg)
			delete [] pg;
		if(permute)
			delete [] permute;
		pg = new Vec3[(table_size*2 + 2)];
		permute = new int[(table_size*2 + 2)];
	
		int i;

		// initalize random gradients
		for(i=0; i<table_size; i++) {
			permute[i] = i;
			pg[i][0] = sfrand();
			pg[i][1] = sfrand();
			pg[i][2] = sfrand();
			float n = sqrt(pg[i][0]*pg[i][0] + pg[i][1]*pg[i][1] + pg[i][2]*pg[i][2]);
			pg[i][0] /= n;
			pg[i][1] /= n;
			pg[i][2] /= n;
		}

		// initialize permutation table (random shuffle)
		for(i=0; i<table_size; i++) {
			int j, t;
			j = (rand() >> 4) % table_size;
			t = permute[i];
			permute[i] = permute[j];
			permute[j] = t;

			// mirror first half of table into second half
			pg[i+table_size][0] = pg[i][0];
			pg[i+table_size][1] = pg[i][1];
			pg[i+table_size][2] = pg[i][2];
			pg[i+table_size][3] = pg[i][3];
			permute[i+table_size] = permute[i];
		}
	}
    unsigned int createPerlinTexture3D_Luminance(int w, int h, int d, UBYTE *p, float amp=1, PerlinMode noiseMode=RAW)
    {
      //UBYTE *img = new UBYTE[w * h * d];

      //UBYTE *p = img;
      //PROGRESSMSG("computing the 3D perlin texture...");
      //PROGRESSSHOW();
      for(int i=0; i<d; i++) {
        //PROGRESSSET((float)i*100.0f/(float)d);
        for(int j=0; j<h; j++) {
          for(int k=0; k<w; k++) {

            float fx = ((float)k / (float)w);
            float fy = ((float)j / (float)h);
            float fz = ((float)i / (float)d);

            float n = eval3d(fx, fy, fz);

			switch(noiseMode)
			{
            case ABSNOISE:
              n = fabs(n);
              break;
            case VEINS:
              // scaled filtered noise - gives cheesy "Star Trek" type effect
              n = 1.0f - 2.0f * (float)(fabs(n));
              break;
            case VEINS2:
              // scaled filtered noise - gives cheesy "Star Trek" type effect
              n = 0.5f + 0.5f * n;
              break;
			}
            n = (n < 0 ? 0 : (n > 1 ? 1 : n));
            *p++ = (UBYTE) (n * amp * 0xff);
          }
        }
      }
#if 0
        m_noisetex.bind();
        m_noisetex.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        m_noisetex.parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        m_noisetex.parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
        m_noisetex.parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
        m_noisetex.parameter(GL_TEXTURE_WRAP_R, GL_REPEAT);

      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      if (glTexImage3D != NULL)
        glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, w, h, d, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, img);
#else
#endif
      //delete [] img;

      //PROGRESSHIDE();
      return -1;//m_noisetex.texture;
    }
    unsigned int createPerlinTexture3D_RGB(int w, int h, int d, UBYTE *p, float amp=1, PerlinMode noiseMode=RAW)
    {
      //UBYTE *img = new UBYTE[w * h * d * 3];

      //UBYTE *p = img;
      //PROGRESSMSG("computing the 3D perlin texture...");
      //PROGRESSSHOW();
      for(int i=0; i<d; i++) {
        //PROGRESSSET((float)i*100.0f/(float)d);
        for(int j=0; j<h; j++) {
          for(int k=0; k<w; k++) {

            float fx = ((float)k / (float)w);
            float fy = ((float)j / (float)h);
            float fz = ((float)i / (float)d);

			float v3[3];
            eval3d(fx, fy, fz, v3);

/*			switch(noiseMode)
			{
            case ABSNOISE:
              n = fabs(n);
              break;
            case VEINS:
              // scaled filtered noise - gives cheesy "Star Trek" type effect
              n = 1.0f - 2.0f * (float)(fabs(n));
              break;
            case VEINS2:
              // scaled filtered noise - gives cheesy "Star Trek" type effect
              n = 0.5f + 0.5f * n;
              break;
			}*/
            v3[0] = (v3[0] < 0 ? 0 : (v3[0] > 1 ? 1 : v3[0]));
            v3[1] = (v3[1] < 0 ? 0 : (v3[1] > 1 ? 1 : v3[1]));
            v3[2] = (v3[2] < 0 ? 0 : (v3[2] > 1 ? 1 : v3[2]));
            *p++ = (UBYTE) (v3[0] * amp * 0xff);
            *p++ = (UBYTE) (v3[1] * amp * 0xff);
            *p++ = (UBYTE) (v3[2] * amp * 0xff);
          }
        }
      }
#if 0
        m_noisetex.bind();
        m_noisetex.parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        m_noisetex.parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        m_noisetex.parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
        m_noisetex.parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
        m_noisetex.parameter(GL_TEXTURE_WRAP_R, GL_REPEAT);

      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      if (glTexImage3D != NULL)
		glTexImage3D(GL_TEXTURE_3D, 0, GL_SIGNED_RGB_NV, w, h, d, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
#else
#endif

      //delete [] img;

      //PROGRESSHIDE();
      return -1;//m_noisetex.texture;
    }
	void getNoisePos(	float *noisePosDst, float *NoiseMatrix, 
						float Scale, 
						float *offset, float *pos)
	{
	  float noisePos[4];
	  noisePos[0] = pos[0] - offset[0];
	  noisePos[1] = pos[1] - offset[1];
	  noisePos[2] = pos[2] - offset[2];
	  noisePos[3] = 1;
	  noisePosDst[0] =  NoiseMatrix[0] * noisePos[0] 
					+ NoiseMatrix[4] * noisePos[1] 
					+ NoiseMatrix[8] * noisePos[2] 
					+ NoiseMatrix[12] * noisePos[3];
	  noisePosDst[1] =  NoiseMatrix[0+1] * noisePos[0] 
					+ NoiseMatrix[4+1] * noisePos[1] 
					+ NoiseMatrix[8+1] * noisePos[2] 
					+ NoiseMatrix[12+1] * noisePos[3];
	  noisePosDst[2] =  NoiseMatrix[0+2] * noisePos[0] 
					+ NoiseMatrix[4+2] * noisePos[1] 
					+ NoiseMatrix[8+2] * noisePos[2] 
					+ NoiseMatrix[12+2] * noisePos[3];
	  noisePosDst[0] += offset[0];
	  noisePosDst[1] += offset[1];
	  noisePosDst[2] += offset[2];
	  noisePosDst[0] *= Scale;
	  noisePosDst[1] *= Scale;
	  noisePosDst[2] *= Scale;
	}
	//
	// same evaluation than the vertex program of the nv30effectsphere
	//
	void EvalFractalNoise(float *VtxPos,float *N, float Scale, float Displacement, float *NoiseMatrix)
	{
		float i;
		float noisePos[3];
		float pos[3];

		pos[0] = VtxPos[0];
		pos[1] = VtxPos[1];
		pos[2] = VtxPos[2];
		noisePos[0] = VtxPos[0] * Scale;
		noisePos[1] = VtxPos[1] * Scale;
		noisePos[2] = VtxPos[2] * Scale;
		i = fabs(eval3d(noisePos[0], noisePos[1], noisePos[2]));
		pos[0] += (N[0] * i * Displacement);
		pos[1] += (N[1] * i * Displacement);
		pos[2] += (N[2] * i * Displacement);

		getNoisePos(noisePos, NoiseMatrix, Scale*2.0, VtxPos, pos);
		i = fabs(eval3d(noisePos[0], noisePos[1], noisePos[2]));
		pos[0] += (N[0] * i * Displacement * 0.5);
		pos[1] += (N[1] * i * Displacement * 0.5);
		pos[2] += (N[2] * i * Displacement * 0.5);

		getNoisePos(noisePos, NoiseMatrix, Scale*4.0, VtxPos, pos);
		i = fabs(eval3d(noisePos[0], noisePos[1], noisePos[2]));
		pos[0] += (N[0] * i * Displacement * 0.25);
		pos[1] += (N[1] * i * Displacement * 0.25);
		pos[2] += (N[2] * i * Displacement * 0.25);

		getNoisePos(noisePos, NoiseMatrix, Scale*8.0, VtxPos, pos);
		i = fabs(eval3d(noisePos[0], noisePos[1], noisePos[2]));
		pos[0] += (N[0] * i * Displacement * 0.125);
		pos[1] += (N[1] * i * Displacement * 0.125);
		pos[2] += (N[2] * i * Displacement * 0.125);
		
		VtxPos[0] = pos[0];
		VtxPos[1] = pos[1];
		VtxPos[2] = pos[2];
	}
private:
	int table_size;
	Vec3 *pg;
	int	*permute;
};


#endif
