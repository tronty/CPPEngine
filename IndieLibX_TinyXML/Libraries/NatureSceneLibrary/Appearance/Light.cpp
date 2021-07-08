#include "Light.h"

#include "../Tools/Logger.h"


using namespace NatureScene;

static int  lightCount = 0;



Light::Light() : MatLight()

{

  initialize();

}



Light::Light(const Light &light) : MatLight(){

  if(this != &light)

    initialize();



  this->operator =(light);

}



void Light::initialize()

{

  lightID     =    0;

  enabled     = true;



  switch(lightCount)

  {

    case 0: lightID = lightCount;break;//GL_LIGHT0; break;

    case 1: lightID = lightCount;break;//GL_LIGHT1; break;

    case 2: lightID = lightCount;break;//GL_LIGHT2; break;

    case 3: lightID = lightCount;break;//GL_LIGHT3; break;

    case 4: lightID = lightCount;break;//GL_LIGHT4; break;

    case 5: lightID = lightCount;break;//GL_LIGHT5; break;

    case 6: lightID = lightCount;break;//GL_LIGHT6; break;

    case 7: lightID = lightCount;break;//GL_LIGHT7; break;

    default:

      LOG_PRINT("Can't use more than 8 lights"));

  }



  if(lightID)

    lightCount++;

}

void Light::bind()

{

  if(!enabled && lightID)

    return;

/*

	D3DLIGHT9 Light0;

	Light0.Type = D3DLIGHT_POINT;//DIRECTIONAL;

	Light0.Direction = D3DXFROMWINEVECTOR3( 1.0f, 0.0f, 1.0f );



	Light0.Position.x = position.x;

	Light0.Position.y = position.y;

	Light0.Position.z = position.z;



	Light0.Ambient.a = getAmbient().w;

	Light0.Ambient.b = getAmbient().z;

	Light0.Ambient.g = getAmbient().y;

	Light0.Ambient.r = getAmbient().x;



	Light0.Diffuse.a = getDiffuse().w;

	Light0.Diffuse.b = getDiffuse().z;

	Light0.Diffuse.g = getDiffuse().y;

	Light0.Diffuse.r = getDiffuse().x;



	Light0.Specular.a = getSpecular().w;

	Light0.Specular.b = getSpecular().z;

	Light0.Specular.g = getSpecular().y;

	Light0.Specular.r = getSpecular().x;



    (g_pApp->m_pRenderer->SetLight( lightID, &Light0 ));

    (g_pApp->m_pRenderer->LightEnable( lightID, TRUE ));
*/


}



void Light::stop(){

  if(!enabled && lightID)

    return;

  //  (g_pApp->m_pRenderer->LightEnable( lightID, FALSE ));

}



Light &Light::operator = (const Light& light){
/*
  if(this != &light)

  {

    setAttenuation(light.attenuation);

    setEnabled    (light.enabled);

    setPosition   (light.position);

    setSpecular   (light.specular);

    setDiffuse    (light.diffuse);

    setAmbient    (light.ambient);

    setLightID    (light.lightID);

  }
*/
  return *this;

}



void Light::setAttenuation(const D3DXFROMWINEVECTOR4 &att)

{

  attenuation = att;

}



void Light::setAttenuation(const float x, const float y,

						               const float z, const float w)

{

  attenuation=D3DXFROMWINEVECTOR4(x,y,z,w);

}



void Light::setPosition(const D3DXFROMWINEVECTOR4  &pos)

{

  position = pos;

}



void Light::setPosition(const float x, const float y,

					              const float z, const float w)

{

  position=D3DXFROMWINEVECTOR4(x,y,z,w);

}



void Light::setLightID(int id)

{

  lightID = id;

}



void Light::setEnabled(bool en)

{

  enabled = en;

}
