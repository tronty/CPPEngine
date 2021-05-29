#ifndef GLLIGHT_H

#define GLLIGHT_H

#include "MatLight.h"



class Light : public MatLight

{

  public:

    Light();

    Light(const Light &);

    Light &operator =(const Light &);



    const D3DXFROMWINEVECTOR4 &getAttenuation() const { return attenuation; }

    const D3DXFROMWINEVECTOR4 &getPosition()    const { return position;    }

    const D3DXFROMWINEVECTOR4 &getColor()       const { return color;       }

    const int      getLightID()     const { return lightID;     }



    void     initialize();

    void     bind();

    void     stop();



    void     setAttenuation(const D3DXFROMWINEVECTOR4  &v);

    void     setAttenuation(const float x, const float y,

					                  const float z, const float w);



    void     setEnabled(bool);



    void     setPosition   (const D3DXFROMWINEVECTOR4 &v);

    void     setPosition   (const float x, const float y,

					                  const float z, const float w);



    void     setLightID(int);

    bool     isEnabled(){ return enabled; }

  private:

    D3DXFROMWINEVECTOR4  attenuation,

             position,

			       color;

	  bool     enabled;

    int      lightID;



};

#endif

