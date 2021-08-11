
#include <STX/STX.h>
#include <STX/STXCPP.h>
#include <STX/STXinterfaces.h>
#include <STX/CInput.h>
#include <STX/Noise.h>
#include <Framework3/IRenderer.h>
#include <Framework3/RendererHLSLCg.h>
#include "Transform3D.h"

#include "../Managers/MediaPathManager.h"

Transform3D::Transform3D() : IOXMLObject("Transform")

{

  setIdentity();

}



Transform3D::Transform3D(const Transform3D &transform)	: IOXMLObject("Transform")

{

  this->operator=(transform);

}



Transform3D::Transform3D(const D3DXFROMWINEMATRIX & matrix)  : IOXMLObject("Transform")

{

  set(matrix);

}



Transform3D &Transform3D::operator= (const Transform3D &copy)

{

  if(this != &copy)

    set(copy);

  return *this;

}



Transform3D::operator const float* ()

{

  return &matrix[0];

}



 Transform3D::operator float* ()

{

  return &matrix[0];

}



const D3DXFROMWINEMATRIX &Transform3D::getMatrix4f() const

{

  return matrix;

}



const D3DXFROMWINEMATRIX &Transform3D::getInverseMatrix4f() const

{

  return inverse;

}



void Transform3D::forceUpdate()

{

  matrix=identity4();

  matrix=matrix * translations;

  matrix=matrix * rotations;

  matrix=matrix * scales;



  inverse = matrix;

//???  invertMatrix(inverse,inverse);

  updated = true;



      D3DXFROMWINEMATRIX IDENTITY;

      IDENTITY=identity4();



  applyInverse = (inverse != IDENTITY);

  apply        = (matrix  != IDENTITY);

}



void Transform3D::doTransform()

{

  if(!apply)

    return;



  //glPushMatrix();

  //glMultMatrixf(matrix);

  //multMatrix(currMatrix,currMatrix,matrix);
	currMatrix=currMatrix*matrix;
}



void Transform3D::undoTransform()

{

  if(!apply)

    return;

  //glPopMatrix();

}



void Transform3D::setScales(const D3DXFROMWINEVECTOR3 &v)

{

  //scales=scale(v.x, v.y, v.z);
	D3DXFROMWINEMatrixScaling(&scales,v.x, v.y, v.z);

  forceUpdate();

}



void Transform3D::setScales(float s)

{

  setScales(s, s, s);

}



void Transform3D::setScales(float scalex, float scaley, float scalez)

{

  //scales=scale(scalex, scaley, scalez);
 D3DXFROMWINEMatrixScaling(&scales, scalex, scaley, scalez);

  forceUpdate();

}



void Transform3D::setIdentity()

{

  applyInverse = false;

  updated      = true;

  apply        = false;



  translations=identity4();

  rotations=identity4();

  scales=identity4();


  inverse=identity4();

  matrix=identity4();

  currMatrix=identity4();

}



void Transform3D::setTranspose()

{

  //matrix=transpose(matrix);
D3DXFROMWINEMatrixTranspose(&matrix, &matrix);
  inverse = matrix;

  //invertMatrix(inverse,inverse);
float f;
D3DXFROMWINEMatrixInverse(&inverse,&f,&inverse);


	D3DXFROMWINEMATRIX IDENTITY;

      IDENTITY=identity4();



  applyInverse = (inverse != IDENTITY);

  updated      = true;

  apply        = (matrix  != IDENTITY);

}



void Transform3D::rotateAxis(const D3DXFROMWINEVECTOR3 &axis, float angle)

{

  D3DXFROMWINEMatrixRotationAxis(&rotations, &axis, angle);
/*
makeRotateMatrix(angle,
		      axis.x, axis.y, axis.z,
		      rotations);

*/
  forceUpdate();

}



void Transform3D::rotateXYZ(const D3DXFROMWINEVECTOR3 &angles)

{

  //rotations=rotateZXY(angles.x, angles.y, angles.z);
D3DXFROMWINEMatrixRotationYawPitchRoll(&rotations, angles.z, angles.x, angles.y);
  forceUpdate();

}



void Transform3D::rotateXYZ(float ax,float ay, float az)

{

  //rotations=rotateZXY( ax, ay, az);
D3DXFROMWINEMatrixRotationYawPitchRoll(&rotations, az, ax, ay);
  forceUpdate();

}



void Transform3D::rotateX(float angle)

{

  //rotations.rotateX(angle);
D3DXFROMWINEMatrixRotationX(&rotations, angle);
  forceUpdate();

}



void Transform3D::rotateY(float angle)

{

  //rotations.rotateY(angle);
D3DXFROMWINEMatrixRotationY(&rotations, angle);

  forceUpdate();

}



void Transform3D::rotateZ(float angle)

{

  //rotations.rotateZ(angle);
D3DXFROMWINEMatrixRotationZ(&rotations, angle);

  forceUpdate();

}



void Transform3D::setTranslations(const D3DXFROMWINEVECTOR3 &v)

{

  //translations=translate(v.x, v.y, v.z);
D3DXFROMWINEMatrixTranslation(&translations,v.x, v.y, v.z);
  forceUpdate();

}



void Transform3D::setTranslations(float x,float y, float z)

{

  //translations=translate(x, y, z);
D3DXFROMWINEMatrixTranslation(&translations,x, y, z);
  forceUpdate();

}



void Transform3D::set(const Transform3D &copy)

{

  if(this != &copy)

  {

    updated	   = copy.updated;



    translations   = copy.translations;

    rotations	   = copy.rotations;

    inverse	   = copy.inverse;

    scales	   = copy.scales;

    matrix	   = copy.matrix;

	currMatrix=copy.currMatrix;



      D3DXFROMWINEMATRIX IDENTITY;

      IDENTITY=identity4();



applyInverse   = (inverse != IDENTITY);

    apply	   = (matrix  != IDENTITY);

  }

}



void Transform3D::set(const D3DXFROMWINEMATRIX &matrixArg)

{

  setIdentity();

  matrix  = matrixArg;

  inverse = matrix;

  //invertMatrix(inverse,inverse);
float f;
D3DXFROMWINEMatrixInverse(&inverse,&f,&inverse);



	D3DXFROMWINEMATRIX IDENTITY;

      IDENTITY=identity4();



  applyInverse = (inverse != IDENTITY);

  apply        = (matrix  != IDENTITY);

}



bool Transform3D::isInverseIdentity()

{

  return !applyInverse;

}



bool Transform3D::isTransformIdentity()

{

  return apply;

}



void Transform3D::setInverseTranspose()

{

  //matrix.setInverseTranspose();

  //invertMatrix(matrix,matrix);
float f;
D3DXFROMWINEMatrixInverse(&matrix,&f,&matrix);
  //transposeMatrix(matrix,matrix);
D3DXFROMWINEMatrixTranspose(&matrix,&matrix);


  inverse=matrix;

//  invertMatrix(inverse,inverse);
D3DXFROMWINEMatrixInverse(&inverse,&f,&inverse);


      D3DXFROMWINEMATRIX IDENTITY;

      IDENTITY=identity4();



  applyInverse = (inverse != IDENTITY);

  updated      = true;

  apply        = (matrix  != IDENTITY);

}



void Transform3D::setInverse()

{

	/*

  matrix.setInverseTranspose();

  inverse = matrix;

  inverse.setInverse();

	*/
/*
  invertMatrix(matrix,matrix);

  matrix=transpose(matrix);
*/
  

float f;
D3DXFROMWINEMatrixTranspose(&matrix,&matrix);
D3DXFROMWINEMatrixInverse(&matrix,&f,&matrix);
inverse = matrix;

      D3DXFROMWINEMATRIX IDENTITY;

      IDENTITY=identity4();



  applyInverse = (inverse != IDENTITY);

  updated      = true;

  apply        = (matrix  != IDENTITY);

}



bool Transform3D::loadXMLSettings(XMLElement *element)

{

  if(!isSuitable(element))

    return  false;



  XMLElement *child;



  if(child = element->getChildByName("Row"))

  {

    XMLElement *subChild;

    D3DXFROMWINEVECTOR4     xyzw;

    int 	offset = 0;

    for(size_t i = 0; i < element->getChildrenCount(); i++)

    {

      if(offset >= 16)

	return true;



      if(!(subChild = element->getChild(i)))

	continue;

      xyzw=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);

      if(subChild->getName() == "Row")

      {

	XMLElement::loadRX_GY_BZ_AWf(*subChild, xyzw);

	updated = true;

	matrix[offset + 0] = xyzw.x;

	matrix[offset + 1] = xyzw.y;

	matrix[offset + 2] = xyzw.z;

	matrix[offset + 3] = xyzw.w;

	offset += 4;

      }

    }

    return true;

  }



  D3DXFROMWINEVECTOR3 translations,

	  rotations,

	  scales(1.0f, 1.0f, 1.0f);



  if(child = element->getChildByName("Scales"))

    XMLElement::loadRX_GY_BZf(*child, scales);





  if(child = element->getChildByName("Rotations"))

    XMLElement::loadRX_GY_BZf(*child, rotations);





  if(child = element->getChildByName("Translations"))

    XMLElement::loadRX_GY_BZf(*child, translations);


/*
  this->translations=translate(translations.x, translations.y, translations.z);

  this->rotations=rotateZXY(rotations.x, rotations.y, rotations.z);

  this->scales=scale(scales.x, scales.y, scales.z);
*/
D3DXFROMWINEMatrixTranslation(&this->translations,translations.x, translations.y, translations.z);
D3DXFROMWINEMatrixRotationYawPitchRoll(&this->rotations, rotations.z, rotations.x, rotations.y);
D3DXFROMWINEMatrixScaling(&this->scales,scales.x, scales.y, scales.z);
  currMatrix=identity4();

  forceUpdate();

  return true;

}



void Transform3D::overrideUpdatedFlag(bool flag)

{

  updated = flag;

}



bool Transform3D::updatedFlagOn()

{

  return updated;

}



bool Transform3D::exportXMLSettings(ofstream &xmlFile  )

{

  if(!xmlFile.is_open())

    {LOG_PRINT_NONE("\n");return false;}

  forceUpdate();

  xmlFile << "	<Transform>\n"

	  << "	  <Row x = \"" << matrix[ 0] << "\" y = \"" << matrix[ 1] << "\" z = \"" << matrix[ 2] << "\" w = \"" << matrix[ 3] << "\" />\n"

	  << "	  <Row x = \"" << matrix[ 4] << "\" y = \"" << matrix[ 5] << "\" z = \"" << matrix[ 6] << "\" w = \"" << matrix[ 7] << "\" />\n"

	  << "	  <Row x = \"" << matrix[ 8] << "\" y = \"" << matrix[ 9] << "\" z = \"" << matrix[10] << "\" w = \"" << matrix[11] << "\" />\n"

	  << "	  <Row x = \"" << matrix[12] << "\" y = \"" << matrix[13] << "\" z = \"" << matrix[14] << "\" w = \"" << matrix[15] << "\" />\n"

	  << "	</Transform>\n";

  return true;

}

