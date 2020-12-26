/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

// Generate shader code for Gaussian blur filters
#include <string>
#include <sstream>
#include "glh/glh_linear.h"

// 1d Gaussian distribution, s is standard deviation
float gaussian(float x, float s) {
//  return exp(-x*x);
  return exp(-x*x/(2*s*s)) / (s*sqrt(2*GLH_PI));
}

// generate array of weights for Gaussian blur
float *
generateGaussianWeights(float s, int &width)
{
    width = (int) floor(3.0f*s)-1;
    int size = width*2+1;
    float *weight = new float [size];

    float sum = 0.0;
    for(int x=0; x<size; x++) {
        weight[x] = gaussian((float) x-width, s);
        sum += weight[x];
    }

//    LOG_PRINT("sum = %f\n", sum);
//    LOG_PRINT("gaussian weights, s = %f, n = %d\n", s, n);
    for(x=0; x<size; x++) {
        weight[x] /= sum;
//        LOG_PRINT("%d: %f\n", x-width, weight[x]);
    }
    return weight;
}

// generate fragment program code for separable convolution
const unsigned char *generate1DConvolutionFP(float *weights, int width, bool vertical, bool tex2D, int img_width, int img_height)
{
  std::ostringstream ost;
  ost <<
  "!!FP1.0\n";

  int nsamples = 2*width+1;
  LOG_PRINT("nsamples = %d\n", nsamples);

  for(int i=0; i<nsamples; i++) {
    float x_offset = 0, y_offset = 0;
    if (vertical) {
      y_offset = (float) i-width;
    } else {
      x_offset = (float) i-width;
    }
    if (tex2D) {
      x_offset = x_offset / img_width;
      y_offset = y_offset / img_height;
    }
    float weight = weights[i];

    ost << "ADDH H0, f[TEX0], {" << x_offset << ", " << y_offset << "};\n";
    if (tex2D) {
      ost << "TEX  H1, H0, TEX0, 2D;\n";
    } else {
      ost << "TEX  H1, H0, TEX0, RECT;\n";
    }

    if (i==-width) {
      ost << "MULH H2, H1, {" << weight << "}.x;\n";
    } else {
      ost << "MADH H2, H1, {" << weight << "}.x, H2;\n";
    }
  }

  ost << 
  "MOVH o[COLH].xyz, H2;\n"
  "END\n";

  char *cstr = new char [ost.str().size()+1];
  strcpy(cstr, ost.str().c_str());

  return (unsigned char *) cstr;
}

/*
  Generate fragment program code for a separable convolution, taking advantage of linear filtering.
  This requires roughly half the number of texture lookups.

  We want the general convolution:
    a*f(i) + b*f(i+1)
  Linear texture filtering gives us:
    f(x) = (1-alpha)*f(i) + alpha*f(i+1);
  It turns out by using the correct weight and offset we can use a linear lookup to achieve this:
    (a+b) * f(i + b/(a+b))
  as long as 0 <= b/(a+b) <= 1
*/

const unsigned char *generate1DConvolutionFP_filter(float *weights, int width, bool vertical, bool tex2D, int img_width, int img_height)
{
  // calculate new set of weights and offsets
  int nsamples = 2*width+1;
  int nsamples2 = (int) ceilf(nsamples/2.0f);
  float *weights2 = new float [nsamples2];
  float *offsets = new float [nsamples2];

  for(int i=0; i<nsamples2; i++) {
    float a = weights[i*2];
    float b;
    if (i*2+1 > nsamples-1)
      b = 0;
    else
      b = weights[i*2+1];
    weights2[i] = a + b;
    offsets[i] = b / (a + b);
//    LOG_PRINT("%d: %f %f\n", i, weights2[i], offsets[i]);
  }
  LOG_PRINT("nsamples = %d\n", nsamples2);

  std::ostringstream ost;
  ost <<
  "!!FP1.0\n";

  for(int i=0; i<nsamples2; i++) {
    float x_offset = 0, y_offset = 0;
    if (vertical) {
      y_offset = (i*2)-width+offsets[i];
    } else {
      x_offset = (i*2)-width+offsets[i];
    }
    if (tex2D) {
      x_offset = x_offset / img_width;
      y_offset = y_offset / img_height;
    }
    float weight = weights2[i];

    ost << "ADDH H0, f[TEX0], {" << x_offset << ", " << y_offset << "};\n";
    if (tex2D) {
      ost << "TEX  H1, H0, TEX0, 2D;\n";
    } else {
      ost << "TEX  H1, H0, TEX0, RECT;\n";
    }

    if (i==-width) {
      ost << "MULH H2, H1, {" << weight << "}.x;\n";
    } else {
      ost << "MADH H2, H1, {" << weight << "}.x, H2;\n";
    }
  }

  ost << 
  "MOVH o[COLH].xyz, H2;\n"
  "END\n";

  char *cstr = new char [ost.str().size()+1];
  strcpy(cstr, ost.str().c_str());

  delete [] weights2;
  delete [] offsets;

  return (unsigned char *) cstr;
}

