#ifndef __Transform__
#define __Transform__
/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <string>
#include <stack>
#include <vector>
#include <list>

class Transform
{
    protected:
    // the parent transform of this transform
    // if it is 0 then the parent transform
    // is the world coordinate system
    Transform* parent;
    // all of the transforms that have this
    // transform set as their parent
    std::vector<Transform*> children;
    
    // the position relative to the parent transform
    D3DXFROMWINEVECTOR3 localPosition;
    // rotation relative to the parent
    D3DXFROMWINEVECTOR3 localRotation;
    // scale relative to the parent
    D3DXFROMWINEVECTOR3 localScale;
    
    // specifies if the localToWorldTransform
    // needs to be recalulated
    bool isDirty;
    // the transform that converts local coordinates
    // to world coordinates
    D3DXFROMWINEMATRIX localToWorldMatrix;
    
    // specifies if the worldToLocalMatrix
    // needs to be recalculated
    bool isInverseDirty;
    // the transform that converts world cooridnates
    // to local coordinates
    D3DXFROMWINEMATRIX worldToLocalMatrix;

    Transform()
    {
	parent=0;
    	localPosition = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
    	localRotation = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
    	localScale = D3DXFROMWINEVECTOR3(1.0f, 1.0f, 1.0f);
    	isDirty = false;
    	D3DXFROMWINEMATRIXIdentity(&localToWorldMatrix);
    	isInverseDirty = false;
    	D3DXFROMWINEMATRIXIdentity(&worldToLocalMatrix);
    }

    Transform(const Transform& rhs)
    {
    	parent=rhs.parent;
    	localPosition=rhs.localPosition;
    	localRotation=rhs.localRotation;
    	localScale=rhs.localScale;
    	isDirty=rhs.isDirty;
    	isInverseDirty=rhs.isInverseDirty;
    }
    
    /*
     * Whenever any change happens that changes the localToWorldMatrix
     * this should be called. That way the next time localToWorldMatrix
     * is requested it will be recalculated
     */
    void setDirty()
    {
    	// only update dirty boolean if it isn't already dirty
        if (!isDirty)
        {
     		isDirty = true;
            isInverseDirty = true;
            
            // set all children to be dirty since any modification
            // of a parent transform also effects its children's
            // localToWorldTransform
            for(unsigned int i=0;i<children.size();i++)
            {
                children[i]->setDirty();
            }
        }
    }
    public:
    // change the parent transform.
    // setting it to 0 makes the
    // transform a child of world coordinates
    void setParent(Transform* value=0)
    {
        // remove this from the previous parent
        if (parent != 0)
        {
        	parent->children.remove(this);
        }
        
        // assign new parent
        parent = value;
        
        // add this to new parent
        if (parent)
        {
            parent->children.push_back(this);   
        }
        
        // changes parents effects 
        // the world position
        setDirty();
    }
    
    Transform* getParent()
    {
    	return parent;   
    }
    
    // calculates the transform matrix that converts
    // from local coordinates to the coordinate space
    // of the parent transform
    D3DXFROMWINEMATRIX calculateLocalToParentMatrix()
    {
        // D3DXFROMWINEMATRIX.translate creates a translation matrix
        // that shifts by (localPosition.x, localPosition.y)
        // D3DXFROMWINEMATRIX.rotate rotates by localRotation radians
        // D3DXFROMWINEMATRIX.scale scales by a factor of (localScale.x, localScale.y)
        // These are the basic transforms that are described previously
        // in this article
        D3DXFROMWINEMATRIX poutT;
        D3DXFROMWINEMatrixTranslation(&poutT, localPosition.x, localPosition.y, localPosition.z);
        D3DXFROMWINEMATRIX poutR;
        D3DXFROMWINEMatrixRotationYawPitchRoll(&poutR, localRotation.x, localRotation.y, localRotation.z);
        D3DXFROMWINEMATRIX poutS;
        D3DXFROMWINEMatrixScaling(&poutS, localScale.x, localScale.y, localScale.z);
        return poutT * 
                poutR * 
                poutS;
    }
    
    // gets the matrix that converts from local
    // coordinates to world coordinates
    D3DXFROMWINEMATRIX getLocalToWorldMatrix()
    {
        // if the dirty flag is set, the the
        // localToWorldMatrix is out of date
        // and needs to be reclaculated
    	if (isDirty)
        {
            if (parent == 0)
            {
           		// if the parent is 0 then the parent is
           		// the world so the localToWorldMatrix
           	 	// is the same as local to parent matrix
                localToWorldMatrix = calculateLocalToParentMatrix();
            }
           	else
            {
                // if there is a parent, then the localToWorldMatrix
                // is calcualted recursively using the parent's localToWorldMatrix
                // concatenated with the local to parent matrix
                localToWorldMatrix = parent->getLocalToWorldMatrix() * calculateLocalToParentMatrix();
            }
            
            // clear the dirty flag since the 
            // matrix is now up to date
            isDirty = false;
        }
        
        return localToWorldMatrix;
    }
    
    D3DXFROMWINEMATRIX getWorldToLocalMatrix()
    {
        if (isInverseDirty)
        {
            // the inverse is out of date
            // so it needs to be updated
            
            // the worldToLocalMatrix is the inverse of
            // the localToWorldMatrix
         	D3DXFROMWINEMatrixInverse(&worldToLocalMatrix, 0, &getLocalToWorldMatrix()); 
            
            // clear the dirty flag since the 
            // matrix is now up to date
            isInverseDirty = false;
        }
        
        return worldToLocalMatrix;
    }
    
    D3DXFROMWINEVECTOR3 getLocalPosition()
    {
    	return localPosition;
    }
    
    // sets the position relative to the parent
    // and marks the transform as dirty
    void setLocalPosition(D3DXFROMWINEVECTOR3 value)
    {
        localPosition = value;
        // set the dirty flag since the localToWorldMatrix needs to be updated
        setDirty();
    }
    
    D3DXFROMWINEVECTOR3 getLocalScale()
    {
    	return localScale;
    }
    
    void setLocalScale(D3DXFROMWINEVECTOR3 value)
    {
        localScale = value;
        setDirty();
    }
    
    D3DXFROMWINEVECTOR3 getLocalRotation()
    {
    	return localRotation;
    }
    
    void setLocalRotation(D3DXFROMWINEVECTOR3 value)
    {
        localRotation = value;
        setDirty();
    }
    
    // transforms a point from local coordinates to world coordinates
    D3DXFROMWINEVECTOR3 transformPoint(D3DXFROMWINEVECTOR3 point)
    {
        D3DXFROMWINEMATRIX T;
        D3DXFROMWINEMatrixTranslation(&T, point.x, point.y, point.z);
        // matrix multiply padding the extra element with a 1
     	D3DXFROMWINEMATRIX transformResult = getLocalToWorldMatrix() * T;
        return D3DXFROMWINEVECTOR3(transformResult[1,1], transformResult[1,2], transformResult[1,3]);
    }
    
    // transforms a direction from local coordinates to world coordinates
    D3DXFROMWINEVECTOR3 transformDirection(D3DXFROMWINEVECTOR3 point)
    {
        D3DXFROMWINEMATRIX T;
        D3DXFROMWINEMatrixTranslation(&T, point.x, point.y, point.z);
        // matrix multiply padding the extra element with a 0
        // notice that the worldToLocalMatrix is used here
        // and the point is multiplied as a row matrix before the
        // transform matrix. This is the proper way to transform
        // directions as described before in this article
     	D3DXFROMWINEMATRIX transformResult = T * getWorldToLocalMatrix();
        return D3DXFROMWINEVECTOR3(transformResult[1,1], transformResult[2,1], transformResult[3,1]);
    }
    
    // transforms a point from world coordinates to local coordinates
    D3DXFROMWINEVECTOR3 inverseTransformPoint(D3DXFROMWINEVECTOR3 point)
    {
        D3DXFROMWINEMATRIX T;
        D3DXFROMWINEMatrixTranslation(&T, point.x, point.y, point.z);
        // same logic as transformPoint only with the inverse matrix
     	D3DXFROMWINEMATRIX transformResult = getWorldToLocalMatrix() * T;
        return D3DXFROMWINEVECTOR3(transformResult[1,1], transformResult[1,2], transformResult[1,3]);
    }

    // transforms a direction from world coordinates to local coordinates
    D3DXFROMWINEVECTOR3 inverseTransformDirection(D3DXFROMWINEVECTOR3 point)
    {
        D3DXFROMWINEMATRIX T;
        D3DXFROMWINEMatrixTranslation(&T, point.x, point.y, point.z);
        // same logic as transformDirection only with the inverse of the 
        // inverse localToWorldMatrix which is just the localToWorldMatrix
     	D3DXFROMWINEMATRIX transformResult = T * getLocalToWorldMatrix();
        return D3DXFROMWINEVECTOR3(transformResult[1,1], transformResult[2,1], transformResult[3,1]);
    }
}
#endif

