#include <Framework3/IRenderer.h>
#include "../Parsers/XMLParser.h"
#include "../Parsers/IOXMLObject.h"
#include "Appearance.h"



Appearance::Appearance() : IOXMLObject("Appearance")

{

  shadersEnabled = true;

  colorEnabled   = false;

//  material       = 0;

  color=D3DXFROMWINEVECTOR4(1,1,1,0);

  shaders=0;

    for(int i = 0; i < MAX_TEX_UNITS; i++)

      textures[i] = -1;

}



Appearance::Appearance(const Appearance &app) : IOXMLObject("Appearance")

{

  if(this != &app)

    this->operator=(app);

}



Appearance &Appearance::operator =(const Appearance &app){



  if(this != &app){

    destroy();



    for(int i = 0; i < MAX_TEX_UNITS; i++)

      textures[i] = app.textures[i];



    shadersEnabled  = app.shadersEnabled;

    colorEnabled    = app.colorEnabled;

    blendAttr       = app.blendAttr;

    alphaAttr       = app.alphaAttr;

    shaders         = app.shaders;

    color           = app.color;



//    if(app.material)

  //    setMaterial(*app.material);

  }

  return *this;

}



bool Appearance::loadXMLSettings(XMLElement *element)

{

  XMLElement  *child        = 0;

  int          textureIndex = 0;



  if(!isSuitable(element))

    return false;



  if(child = element->getChildByName("description"))

    return  IOXMLObject::loadXMLSettings(child->getValuec());



  for(size_t i = 0; i < element->getChildrenCount(); i++)

  {

    if(!(child = element->getChild(i)))

      continue;



    const NSString &childName = child->getName();

    textureIndex = clampNS(textureIndex, 0, MAX_TEX_UNITS-1);



    if(childName == "Material")

    {

//      if(!material)

  //      material = new Material();



    //  material->loadXMLSettings(child);

      continue;

    }



    if(childName == "TextureID")

    {

//      if(textures[textureIndex].loadXMLSettings(child))

  //      textureIndex++;

      continue;

    }



    if(childName == "AlphaAttributes")

    {

//      alphaAttr.loadXMLSettings(child);

      continue;

    }



    if(childName == "BlendAttributes")

    {

//      blendAttr.loadXMLSettings(child);

      continue;

    }



    if(childName == "Color")

    {

      XMLElement::loadRX_GY_BZ_AWf(*child, color);

      continue;

    }



    if(childName == "colorEnabled")

    {

      enableColor(child->getValue() == "true");

      continue;

    }



    if(childName == "shadersEnabled")

    {

      enableShaders(child->getValue() == "true");

      continue;

    }

  }

  return true;

}



bool Appearance::exportXMLSettings(ofstream &xmlFile)

{

  if(!xmlFile.is_open())

    {LOG_PRINT_NONE("\n");return false;}



  xmlFile << "  <Appearance colorEnabled   = \"" << (colorEnabled  ? "true" : "false") << "\"\n"

          << "              shadersEnabled = \"" << (shadersEnabled? "true" : "false") << "\">\n";



  xmlFile << "    <Color r = \""  << color.x << "\" "

          << "g = \""  << color.y << "\" "

          << "b = \""  << color.z << "\" "

          << "a = \""  << color.w << "\"/>\n";



  //alphaAttr.exportXMLSettings(xmlFile);

  //blendAttr.exportXMLSettings(xmlFile);



  //if(material)

    //material->exportXMLSettings(xmlFile);



  //for(int i = 0; i < MAX_TEX_UNITS; i++)

  //  if(textures[i])

//      textures[i].exportXMLSettings(xmlFile);



  xmlFile << "  </Appearance>\n";

  return true;

}



void Appearance::setTexture(int index, const TextureID texture)

{

  if(index < 0 || index > MAX_TEX_UNITS - 1)

    return;



  textures[index] = texture;

}



TextureID Appearance::getTexture(int index)

{

  int r= (index < 0) || (index >= MAX_TEX_UNITS) ? -1 : textures[index];

  return r;

}



void Appearance::setBlendAttributes(const BlendStateID &attri)

{

  blendAttr = attri;

}



BlendStateID &Appearance::getBlendAttributes()

{

  return blendAttr;

}



void Appearance::setAlphaAttributes(const BlendStateID &attri)

{

  alphaAttr = attri;

}



BlendStateID &Appearance::getAlphaAttributes()

{

  return alphaAttr;

}

/*

void      Appearance::setMaterial(const Material &mat)

{

  if(!material)

    material = new Material(mat);

  else

    material->operator =(mat);

}

Material *Appearance::getMaterial()                   { return material; }

*/

void  Appearance::setColor(const float* c){ color=D3DXFROMWINEVECTOR4(c[0], c[1], c[2], c[3]);      }

void  Appearance::setColor(float r, float g, float b, float a){ color=D3DXFROMWINEVECTOR4(r,g,b,a); }

const D3DXFROMWINEVECTOR4 &Appearance::getColor() const { return color; }



void     Appearance::setShaders(ShaderID &shadersArg){ shaders = shadersArg; }

ShaderID &Appearance::getShaders()                   { return shaders;       }



void Appearance::enableShaders(bool enabled){ shadersEnabled = enabled; }

void Appearance::enableColor(bool enabled)  { colorEnabled   = enabled; }



bool Appearance::shadersOn(){ return shadersEnabled; }

bool Appearance::colorOn()  { return colorEnabled;   }



void Appearance::destroy()

{

  
	//for(int i = 0; i < MAX_TEX_UNITS; i++)

//    textures[i].destroy();



//  deleteObject(material);


  shadersEnabled  = false;

}



Appearance::~Appearance(){

  destroy();

}

