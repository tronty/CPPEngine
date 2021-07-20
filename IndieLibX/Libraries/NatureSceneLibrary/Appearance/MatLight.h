#ifndef MATLIGHT_H

#define MATLIGHT_H



#include "../Tools/Logger.h"



class MatLight

{

  public:

    void setSpecular(const D3DXFROMWINEVECTOR4 &v);

    void setDiffuse (const D3DXFROMWINEVECTOR4 &v);

    void setAmbient (const D3DXFROMWINEVECTOR4 &v);



    void setSpecular(const float r, const float g,

	                   const float b, const float a);

    void setDiffuse (const float r, const float g,

	                   const float b, const float a);

    void setAmbient (const float r, const float g,

	                   const float b, const float a);



    const D3DXFROMWINEVECTOR4 &getSpecular() const { return specular; }

    const D3DXFROMWINEVECTOR4 &getDiffuse () const { return diffuse;  }

    const D3DXFROMWINEVECTOR4 &getAmbient () const { return ambient;  }



  protected:

    D3DXFROMWINEVECTOR4  specular,

             diffuse,

	           ambient;

};

#endif
