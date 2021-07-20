#include "Material.h"

#include "../Managers/MediaPathManager.h"

using namespace NatureScene;


Material::Material() : MatLight(), IOXMLObject("Material")

{

  shininess   =   64;

}



Material::Material(const Material &mat) : MatLight() , IOXMLObject("Material")

{

  this->operator =(mat);

}



Material  &Material::operator = (const Material& copy)

{

  if(this != &copy)

  {

    shininess = copy.shininess;

    specular  = copy.specular;

    emissive  = copy.emissive;

    diffuse   = copy.diffuse;

    ambient   = copy.ambient;

  }

  return *this;

}



void Material::bindMaterial()

{
/*
	D3DMATERIAL9 Mtrl;

	ZeroMemory( &Mtrl, sizeof(D3DMATERIAL9) );



    Mtrl.Diffuse.r = diffuse.x;

    Mtrl.Diffuse.g = diffuse.y;

    Mtrl.Diffuse.b = diffuse.z;

    Mtrl.Diffuse.a = diffuse.w;



    Mtrl.Ambient.r = ambient.x;

    Mtrl.Ambient.g = ambient.y;

    Mtrl.Ambient.b = ambient.z;

    Mtrl.Ambient.a = ambient.w;



    Mtrl.Emissive.r = emissive.x;

    Mtrl.Emissive.g = emissive.y;

    Mtrl.Emissive.b = emissive.z;

    Mtrl.Emissive.a = emissive.w;



    Mtrl.Specular.r = specular.x;

    Mtrl.Specular.g = specular.y;

    Mtrl.Specular.b = specular.z;

    Mtrl.Specular.a = specular.w;



    Mtrl.Power = shininess;//???

    (g_pApp->m_pRenderer->SetMaterial( &Mtrl ));
*/
}



bool Material::loadXMLSettings(XMLElement *element)

{

  XMLElement  *child     = 0;



  if(!isSuitable(element))

    return false;



  if(child = element->getChildByName("description"))

    return  IOXMLObject::loadXMLSettings(child->getValuec());



  for(size_t i = 0; i < element->getChildrenCount(); i++)

  {

    if(!(child = element->getChild(i)))

      continue;



    const NSString &childName = child->getName();



    if(childName == "shininess")

    {

      setShininess(child->getValuef());

      continue;

    }



    if(childName == "Ambient")

    {

      D3DXFROMWINEVECTOR4 attributes;

      XMLElement::loadRX_GY_BZ_AWf(*child, attributes);

      setAmbient(attributes);

      continue;

    }



    if(childName == "Diffuse")

    {

      D3DXFROMWINEVECTOR4 attributes;

      XMLElement::loadRX_GY_BZ_AWf(*child, attributes);

      setDiffuse(attributes);

      continue;

    }



    if(childName == "Emissive")

    {

      D3DXFROMWINEVECTOR4 attributes;

      XMLElement::loadRX_GY_BZ_AWf(*child, attributes);

      setEmissive(attributes);

      continue;

    }



    if(childName == "Specular")

    {

      D3DXFROMWINEVECTOR4 attributes;

      XMLElement::loadRX_GY_BZ_AWf(*child, attributes);

      setSpecular(attributes);

      continue;

    }

  }

  return true;

}



bool Material::exportXMLSettings(ofstream &xmlFile)

{

  if(!xmlFile.is_open())

  {

	  LOG_PRINT("Cannot export Material to XML: file not ready."));

	  return false;

  }



  NSString  matAttr[4]   = { "Ambient ",  "Diffuse ", "Emissive", "Specular" };

  D3DXFROMWINEVECTOR4 attributes;

  xmlFile << "  <Material shininess = \"" << getShininess() << "\"> \n";



  for(int i = 0; i < 4; i++)

  {

    switch(i)

    {

      case 0:  attributes = getAmbient();    break;

      case 1:  attributes = getDiffuse();    break;

      case 2:  attributes = getEmissive();   break;

      case 3:  attributes = getSpecular();   break;

    }

    if(attributes)

    {

      xmlFile << "    <"    << matAttr[i]   << " ";

      xmlFile << " r = \""  << attributes.x << "\" "

              << " g = \""  << attributes.y << "\" "

              << " b = \""  << attributes.z << "\" "

              << " a = \""  << attributes.w << "\"/>\n";

    }

  }



  xmlFile << "  </Material>\n";

  return true;

}



void Material::setShininess(const float s)

{

  shininess = s;

}



void Material::setEmissive(const D3DXFROMWINEVECTOR4 &v)

{

    emissive = v;

}



void Material::setEmissive(const float r, const float g,

                             const float b, const float a){

  emissive=D3DXFROMWINEVECTOR4(r, g, b, a);

}
