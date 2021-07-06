#include "MatLight.h"

#include "../Tools/Logger.h"



void MatLight::setDiffuse(const D3DXFROMWINEVECTOR4 &t)

{

  diffuse = t;

}



void MatLight::setDiffuse(const float r, const float g,

						              const float b, const float a)

{

  diffuse=D3DXFROMWINEVECTOR4(r, g, b, a);

}



void MatLight::setSpecular(const D3DXFROMWINEVECTOR4 &t)

{

  specular = t;

}



void MatLight::setSpecular(const float r, const float g,

						               const float b, const float a)

{

  specular=D3DXFROMWINEVECTOR4(r, g, b, a);

}



void MatLight::setAmbient(const D3DXFROMWINEVECTOR4 &t)

{

  ambient = t;

}



void MatLight::setAmbient(const float r, const float g,

						              const float b, const float a)

{

  ambient=D3DXFROMWINEVECTOR4(r, g, b, a);

}
