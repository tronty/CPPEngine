/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

	class HDRtexture {
public:
  HDRtexture(char *filename);
  ~HDRtexture();

  unsigned char *GetPixel(int x, int y);
  float *GetPixelHilo(int x, int y);

  GLuint Create2DTextureRGBE(GLenum target);
  GLuint Create2DTextureHILO(GLenum target, bool rg);
  GLuint CreateCubemap(GLenum format);
  GLuint CreateCubemapRGBE();
  GLuint CreateCubemapHILO(bool rg);

  int GetWidth() { return m_width; };
  int GetHeight() { return m_height; };

  bool IsValid() { return m_valid; };

  void Analyze();
  void ConvertHILO();

  bool m_valid;
  int m_width, m_height;
  unsigned char *m_data;
  float *m_hilodata;

  GLenum m_target;

  float m_max_r, m_max_g, m_max_b;
  float m_min_r, m_min_g, m_min_b;
  float m_max;
};

