#ifndef __NV_NOISE_H__
#define __NV_NOISE_H__

#include <STX/STX.h>
#include <STX/d3dx9mathFromWine.h>

class FilteredNoise {
public:
  FilteredNoise(int w, int h, int d);
  ~FilteredNoise();

  float noise2D(int x, int y);
  float noise3D(int x, int y, int z);
  float bicubicNoise2D(float x, float y);
  float bicubicNoise3D(float x, float y, float z);

  // noise types
  enum NoiseMode {
	  NOISE = 0,  // filtered noise
	  ABSNOISE,   // absolute value noise
	  VEINS,      // scale noise
	  RAW,        // raw noise
	  DISTANCE,   // distance function (for Worley patterns)
  };

  void createVectorNoiseTexture3D(int w, int h, int d, float scale, float amp, NoiseMode noiseMode);
  unsigned int createNoiseTexture3D(int w, int h, int d, float scale, float amp, NoiseMode noiseMode);

private:
  int m_w, m_h, m_d;
  float *m_noise;
};

#endif
