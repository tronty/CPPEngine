/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

float *generateGaussianWeights(float s, int &n);
const unsigned char *generate1DConvolutionFP(float *weights, int n, bool vertical, bool tex2D, int img_width, int img_height);
const unsigned char *generate1DConvolutionFP_filter(float *weights, int width, bool vertical, bool tex2D, int img_width, int img_height);

