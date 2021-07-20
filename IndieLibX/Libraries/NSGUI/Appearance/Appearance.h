/*

 *Author: Abdul Bezrati

 *Email : abezrati@hotmail.com

 */



#ifndef APPEARANCE_H

#define APPEARANCE_H





//include "../Shaders/Shaders.h"

//include "../Tools/MathUtils.h"

#include "BlendAttributes.h"

#include "AlphaAttributes.h"

//include "Material.h"

#include "Texture.h"

#ifndef COLORVALUE_DEFINED
typedef struct _COLORVALUE {
    float r;
    float g;
    float b;
    float a;
} COLORVALUE;
#define COLORVALUE_DEFINED
#endif

typedef struct _MATERIAL9 {
    COLORVALUE   Diffuse;        /* Diffuse color RGBA */
    COLORVALUE   Ambient;        /* Ambient color RGB */
    COLORVALUE   Specular;       /* Specular 'shininess' */
    COLORVALUE   Emissive;       /* Emissive color RGB */
    float           Power;          /* Sharpness if specular highlight */
} Material9;

class Appearance : public IOXMLObject

{



  private:
#if 0
    BlendAttributes  blendAttr;

    AlphaAttributes  alphaAttr;
#else
    BlendStateID  blendAttr;

    BlendStateID  alphaAttr;
#endif
    //Material	    *material;

    ShaderID	      shaders;

    TextureID	       textures[MAX_TEX_UNITS];

    D3DXFROMWINEVECTOR4	    color;

    bool	     shadersEnabled,

		     colorEnabled;



  public:

    Appearance();

   ~Appearance();

    Appearance(const Appearance&);

	Appearance &operator=(const Appearance&);



    void destroy();



    virtual bool loadXMLSettings(XMLElement *element);

    virtual bool exportXMLSettings(ofstream &xmlFile);



    void		   setBlendAttributes(const BlendStateID &attri);

    BlendStateID &getBlendAttributes();



    void		   setAlphaAttributes(const BlendStateID &attri);

    BlendStateID &getAlphaAttributes();


/*
    void      setMaterial(const Material &material);

    Material *getMaterial();
*/


    void     setTexture(int index, const TextureID texture);

    TextureID getTexture(int index);



    void     setShaders(ShaderID &shaders);

    ShaderID &getShaders();



    void setTriangleWinding(unsigned int w);

    void setDepthMask(unsigned int m);



    void  setColor(const float* c);

    void  setColor(float r, float g, float b, float a);

    const D3DXFROMWINEVECTOR4 &getColor() const;



    void enableShaders(bool );

    void enableColor(bool);



    bool shadersOn();

    bool colorOn();

};

#endif

