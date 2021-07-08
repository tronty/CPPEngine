#ifndef TRANSFORM3D_H

#define TRANSFORM3D_H



#include "../Parsers/IOXMLObject.h"



class Transform3D : public IOXMLObject

{

  private:

    D3DXFROMWINEMATRIX translations,

             rotations,
             scales,

            inverse,

             matrix,


			 currMatrix;



    bool     applyInverse,

             updated,

             apply;



    void     forceUpdate();



  public:

    Transform3D();

    Transform3D(const Transform3D &t);

    Transform3D(const D3DXFROMWINEMATRIX & m);

   	Transform3D &operator =(const Transform3D &t);



    virtual bool loadXMLSettings(XMLElement *element);

    virtual bool exportXMLSettings(ofstream &xmlFile);



    operator const float* ();

    operator float* ();



    const D3DXFROMWINEMATRIX &getInverseMatrix4f() const;

    const D3DXFROMWINEMATRIX &getMatrix4f()        const;



    void overrideUpdatedFlag(bool );

    bool updatedFlagOn();



    void undoTransform();

    void doTransform();



    void rotateAxis(const D3DXFROMWINEVECTOR3 &axis, float angle);

    void rotateXYZ(const D3DXFROMWINEVECTOR3 &angles);

    void rotateXYZ(float ax,float ay, float az);

    void rotateX(float angle);

    void rotateY(float angle);

    void rotateZ(float angle);



    void setTranslations(const D3DXFROMWINEVECTOR3 &v);

    void setTranslations(float x,float y, float z);



    void setInverseTranspose();

    void setInverse();



    void setTranspose();

    void setIdentity();



    void setScales(float scalesXYZ);

    void setScales(const D3DXFROMWINEVECTOR3 &);

    void setScales(float scaleX, float scaleY, float scaleZ);



    void set(const Transform3D &t);

    void set(const D3DXFROMWINEMATRIX &mat);



    bool isTransformIdentity();

    bool isInverseIdentity();

};

#endif
