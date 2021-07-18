#ifndef MATERIAL_H

#define MATERIAL_H

#include "MatLight.h"

#include "../Parsers/IOXMLObject.h"



class Material : public MatLight, public IOXMLObject

{

  public:

    Material();

    Material(const Material &);

    Material &operator = (const Material&);



    virtual bool loadXMLSettings(XMLElement *element);

    virtual bool exportXMLSettings(ofstream &xmlFile);



    const D3DXFROMWINEVECTOR4 &getEmissive()  const { return emissive;  }

    const float    getShininess() const { return shininess; }



    void     bindMaterial();

    void     initialize();

    void     setShininess(const float s);

    void     setEmissive(const D3DXFROMWINEVECTOR4 &v);

    void     setEmissive(const float r, const float g,

                         const float b, const float a);

  private:

    D3DXFROMWINEVECTOR4 emissive;

    float   shininess;

};

#endif

