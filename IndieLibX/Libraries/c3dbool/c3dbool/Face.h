/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef __Face_H__
#define __Face_H__
/**
 * Representation of a 3D face (triangle).
 *
 * <br><br>See: 
 * D. H. Laidlaw, W. B. Trumbore, and J. F. Hughes.  
 * "Constructive Solid Geometry for Polyhedral Objects" 
 * SIGGRAPH Proceedings, 1986, p.161. 
 * 
 * @author Danilo Balby Silva Castanheira (danbalby@yahoo.com)
 */
class Object3D;
class Face : public Object// implements Cloneable
{
public:
	/** first vertex */
	c3dbool_Vertex v1;
	/** second vertex */
	c3dbool_Vertex v2;
	/** third vertex */
	c3dbool_Vertex v3;
	/** face status relative to a solid  */
	int status;
	Face(const Face& rhs)
	{
		C3D_FNLN;
		v1=rhs.v1;
		v2=rhs.v2;
		v3=rhs.v3;
		status=rhs.status;
	}
	virtual const std::string getString() const 
	{
		C3D_FNLN;
		return "Face";
	}
	
	enum __eFace__
 	{
	/** face status if it is still unknown */
		eFace_UNKNOWN,
	/** face status if it is inside a solid */
		eFace_INSIDE,
	/** face status if it is outside a solid */
		eFace_OUTSIDE,
	/** face status if it is coincident with a solid face */
		eFace_SAME,
	/** face status if it is coincident with a solid face with opposite orientation*/
		eFace_OPPOSITE,
	
	/** point status if it is up relative to an edge - see linePositionIn_ methods */
		eFace_UP,
	/** point status if it is down relative to an edge - see linePositionIn_ methods */
		eFace_DOWN,
	/** point status if it is on an edge - see linePositionIn_ methods */
		eFace_ON,
	/** point status if it isn't up, down or on relative to an edge - see linePositionIn_ methods */
		eFace_NONE,
	};
	
inline bool operator == (const Face& v) const
{
		C3D_FNLN;
    return 
	(fabs(v1.x-v.v1.x)<TOL) && 
	(fabs(v1.x-v.v1.x)<TOL) && 
	(fabs(v1.x-v.v1.x)<TOL) && 
	(fabs(v2.x-v.v2.x)<TOL) && 
	(fabs(v2.x-v.v2.x)<TOL) && 
	(fabs(v2.x-v.v2.x)<TOL) && 
	(fabs(v3.x-v.v3.x)<TOL) && 
	(fabs(v3.x-v.v3.x)<TOL) && 
	(fabs(v3.x-v.v3.x)<TOL);
}

inline bool operator != (const Face& v) const
{
		C3D_FNLN;
    return 
	(fabs(v1.x-v.v1.x)>TOL) || 
	(fabs(v1.x-v.v1.x)>TOL) || 
	(fabs(v1.x-v.v1.x)>TOL) || 
	(fabs(v2.x-v.v2.x)>TOL) || 
	(fabs(v2.x-v.v2.x)>TOL) ||
	(fabs(v2.x-v.v2.x)>TOL) || 
	(fabs(v3.x-v.v3.x)>TOL) || 
	(fabs(v3.x-v.v3.x)>TOL) || 
	(fabs(v3.x-v.v3.x)>TOL);
}

	
	//---------------------------------CONSTRUCTORS---------------------------------//
	
	/**
	 * Constructs a face with unknown status.
	 * 
	 * @param v1 a face vertex
	 * @param v2 a face vertex
	 * @param v3 a face vertex
	 */
	Face() : Object()
	{
		C3D_FNLN;
		////LOG_FNLN;
		v1 = c3dbool_Vertex(Point3d(0.0f, 0.0f, 0.0f), Color3f(0.0f, 0.0f, 0.0f));
		v2 = c3dbool_Vertex(Point3d(0.0f, 0.0f, 0.0f), Color3f(0.0f, 0.0f, 0.0f));
		v3 = c3dbool_Vertex(Point3d(0.0f, 0.0f, 0.0f), Color3f(0.0f, 0.0f, 0.0f));
		
		status = UNKNOWN;
	}
	Face(c3dbool_Vertex v1, c3dbool_Vertex v2, c3dbool_Vertex v3) : Object()
	{
		C3D_FNLN;
		v1 = v1;
		v2 = v2;
		v3 = v3;
		
		status = UNKNOWN;
		#if 0
		////LOG_FNLN;
		////LOG_PRINT("v1.x=%f\n", v1.x);
		////LOG_PRINT("v1.y=%f\n", v1.y);
		////LOG_PRINT("v1.z=%f\n", v1.z);
		////LOG_PRINT("v2.x=%f\n", v2.x);
		////LOG_PRINT("v2.y=%f\n", v2.y);
		////LOG_PRINT("v2.z=%f\n", v2.z);
		////LOG_PRINT("v3.x=%f\n", v3.x);
		////LOG_PRINT("v3.y=%f\n", v3.y);
		////LOG_PRINT("v3.z=%f\n", v3.z);
		#endif
	}
	
	//-----------------------------------OVERRIDES----------------------------------//
	
	/**
	 * Clones the face object
	 * 
	 * @return cloned face object
	 */
	Object clone()
	{
		C3D_FNLN;
		//try
		{
			Face clone;
			clone.v1 = c3dbool_Vertex();
			clone.v2 = c3dbool_Vertex();
			clone.v3 = c3dbool_Vertex();
			clone.status = status;
			return clone;
		}/*
		catch(CloneNotSupportedException e)
		{	
			return 0;
		}*/
	}
	
	/**
	 * Makes a string definition for the Face object
	 * 
	 * @return the string definition
	 */
	std::string toString()
	{
		C3D_FNLN;
		char buf[256];
		stx_snprintf(buf, 256, "%f\n%f\n%f\n", v1.toString().c_str(), v2.toString().c_str(), v3.toString().c_str());
		return buf;
	}
	
	/**
	 * Checks if a face is equal to another. To be equal, they have to have equal
	 * vertices in the same order
	 * 
	 * @param anObject the other face to be tested
	 * @return true if they are equal, false otherwise. 
	 */
	bool equals(Object anObject)
	{
		C3D_FNLN;
		if(anObject.getString()!="Face")
		{
			return false;
		}
		else
		{
			Face face = *(Face*)&anObject;
			bool cond1 = v1.equals(face.v1) && v2.equals(face.v2) && v3.equals(face.v3);
			bool cond2 = v1.equals(face.v2) && v2.equals(face.v3) && v3.equals(face.v1);
			bool cond3 = v1.equals(face.v3) && v2.equals(face.v1) && v3.equals(face.v2);
			
			return cond1 || cond2 || cond3;  	 			
		}
	}
	
	//-------------------------------------GETS-------------------------------------//
	
	/**
	 * Gets the face bound
	 * 
	 * @return face bound 
	 */
	Bound getBound();
	
	/**
	 * Gets the face normal
	 * 
	 * @return face normal
	 */
	Vector3d getNormal()
	{
		C3D_FNLN;
		Point3d p1 = v1.getPosition();
		Point3d p2 = v2.getPosition();
		Point3d p3 = v3.getPosition();
		Vector3d xy, xz, normal;
		
		xy = Vector3d(p2.x-p1.x, p2.y-p1.y, p2.z-p1.z);
		xz = Vector3d(p3.x-p1.x, p3.y-p1.y, p3.z-p1.z);
		
		normal = Vector3d();
		#if 0
		normal.cross(xy,xz);
		normal.normalize();
		#else
		D3DXFROMWINEVec3Cross(&normal, &xy, &xz);
		D3DXFROMWINEVec3Normalize(&normal, &normal);
		#endif
		
		#if 0
		//LOG_FNLN;
		//LOG_PRINT("normal.x=%f\n", normal.x);
		//LOG_PRINT("normal.y=%f\n", normal.y);
		//LOG_PRINT("normal.z=%f\n", normal.z);
		#endif
		return normal;
	}
	
	/**
	 * Gets the face status
	 * 
	 * @return face status - UNKNOWN, INSIDE, OUTSIDE, SAME OR OPPOSITE
	 */
	int getStatus()
	{
		C3D_FNLN;
		return status;
	}
	
	/**
	 * Gets the face area
	 * 
	 * @return face area
	 */
	__DOUBLE__ getArea()
	{
		C3D_FNLN;
		//area = (a * c * sen(B))/2
		Point3d p1 = v1.getPosition();
		Point3d p2 = v2.getPosition();
		Point3d p3 = v3.getPosition();
		Vector3d xy = Vector3d(p2.x-p1.x, p2.y-p1.y, p2.z-p1.z);
		Vector3d xz = Vector3d(p3.x-p1.x, p3.y-p1.y, p3.z-p1.z);
		
		#if 0
		__DOUBLE__ a = p1.distance(p2);
		__DOUBLE__ c = p1.distance(p3);
		__DOUBLE__ B = xy.angle(xz);
		#else
		__DOUBLE__ a = p1.getDistance(p2);
		__DOUBLE__ c = p1.getDistance(p3);
		__DOUBLE__ B = xy.angle(xz);
		#endif
		
		return (a * c * sinf(B))/2.0f;
	}
	
	//-------------------------------------OTHERS-----------------------------------//
	
	/** Invert face direction (normal direction) */
	void invert()
	{
		C3D_FNLN;
		c3dbool_Vertex vertexTemp = v2;
		v2 = v1;
		v1 = vertexTemp;
	}
		
	//------------------------------------CLASSIFIERS-------------------------------//
	
	/**
	 * Classifies the face if one of its vertices are classified as INSIDE or OUTSIDE
	 * 
	 * @return true if the face could be classified, false otherwise 
	 */
	bool simpleClassify()
	{
		C3D_FNLN;
		int status1 = v1.getStatus();
		int status2 = v2.getStatus();
		int status3 = v3.getStatus();
			
		if(status1==eFace_INSIDE || status1==eFace_OUTSIDE)
		{
			status = status1;
			return true; 
		}
		else if(status2==eFace_INSIDE || status2==eFace_OUTSIDE)
		{
			status = status2;
			return true;
		}
		else if(status3==eFace_INSIDE || status3==eFace_OUTSIDE)
		{
			status = status3;
			return true;
		}
		else
		{
			return false;
		}
	}
	
	/**
	 * Classifies the face based on the ray trace technique
	 * 
	 * @param object object3d used to compute the face status 
	 */
	void rayTraceClassify(Object3D* aobject);
	
	//------------------------------------PRIVATES----------------------------------//
	
	/**
	 * Checks if the the face contains a point
	 * 
	 * @param point to be tested
	 * @param true if the face contains the point, false otherwise 
	 */	
	bool hasPoint(Point3d point)
	{
		C3D_FNLN;
		int result1, result2, result3;
		bool hasUp, hasDown, hasOn;
		Vector3d normal = getNormal(); 
	
		//if x is constant...	
		if(fabs(normal.x)>TOL) 
		{
			//tests on the x plane
			result1 = linePositionInX(point, v1.getPosition(), v2.getPosition());
			result2 = linePositionInX(point, v2.getPosition(), v3.getPosition());
			result3 = linePositionInX(point, v3.getPosition(), v1.getPosition());
		}
		
		//if y is constant...
		else if(fabs(normal.y)>TOL)
		{
			//tests on the y plane
			result1 = linePositionInY(point, v1.getPosition(), v2.getPosition());
			result2 = linePositionInY(point, v2.getPosition(), v3.getPosition());
			result3 = linePositionInY(point, v3.getPosition(), v1.getPosition());
		}
		else
		{
			//tests on the z plane
			result1 = linePositionInZ(point, v1.getPosition(), v2.getPosition());
			result2 = linePositionInZ(point, v2.getPosition(), v3.getPosition());
			result3 = linePositionInZ(point, v3.getPosition(), v1.getPosition());
		}
		
		//if the point is up and down two lines...		
		if(((result1==eFace_UP)||(result2==eFace_UP)||(result3==eFace_UP))&&((result1==eFace_DOWN)||(result2==eFace_DOWN)||(result3==eFace_DOWN)))
		{
			return true;
		}
		//if the point is on of the lines...
		else if ((result1==eFace_ON)||(result2==eFace_ON)||(result3==eFace_ON))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/** 
	 * Gets the position of a point relative to a line in the x plane
	 * 
	 * @param point point to be tested
	 * @param pointLine1 one of the line ends
	 * @param pointLine2 one of the line ends
	 * @return position of the point relative to the line - UP, DOWN, ON, NONE 
	 */
	int linePositionInX(Point3d point, Point3d pointLine1, Point3d pointLine2)
	{
		C3D_FNLN;
		__DOUBLE__ a, b, z;
		if((fabs(pointLine1.y-pointLine2.y)>TOL)&&(((point.y>=pointLine1.y)&&(point.y<=pointLine2.y))||((point.y<=pointLine1.y)&&(point.y>=pointLine2.y))))
		{
			a = (pointLine2.z-pointLine1.z)/(pointLine2.y-pointLine1.y);
			b = pointLine1.z - a*pointLine1.y;
			z = a*point.y + b;
			if(z>point.z+TOL)
			{
				return eFace_UP;			
			}
			else if(z<point.z-TOL)
			{
				return eFace_DOWN;
			}
			else
			{
				return eFace_ON;
			}
		}
		else
		{
			return eFace_NONE;
		}
	}

	/** 
	 * Gets the position of a point relative to a line in the y plane
	 * 
	 * @param point point to be tested
	 * @param pointLine1 one of the line ends
	 * @param pointLine2 one of the line ends
	 * @return position of the point relative to the line - UP, DOWN, ON, NONE 
	 */

	int linePositionInY(Point3d point, Point3d pointLine1, Point3d pointLine2)
	{
		C3D_FNLN;
		__DOUBLE__ a, b, z;
		if((fabs(pointLine1.x-pointLine2.x)>TOL)&&(((point.x>=pointLine1.x)&&(point.x<=pointLine2.x))||((point.x<=pointLine1.x)&&(point.x>=pointLine2.x))))
		{
			a = (pointLine2.z-pointLine1.z)/(pointLine2.x-pointLine1.x);
			b = pointLine1.z - a*pointLine1.x;
			z = a*point.x + b;
			if(z>point.z+TOL)
			{
				return eFace_UP;			
			}
			else if(z<point.z-TOL)
			{
				return eFace_DOWN;
			}
			else
			{
				return eFace_ON;
			}
		}
		else
		{
			return eFace_NONE;
		}
	}

	/** 
	 * Gets the position of a point relative to a line in the z plane
	 * 
	 * @param point point to be tested
	 * @param pointLine1 one of the line ends
	 * @param pointLine2 one of the line ends
	 * @return position of the point relative to the line - UP, DOWN, ON, NONE 
	 */

	int linePositionInZ(Point3d point, Point3d pointLine1, Point3d pointLine2)
	{
		C3D_FNLN;
		__DOUBLE__ a, b, y;
		if((fabs(pointLine1.x-pointLine2.x)>TOL)&&(((point.x>=pointLine1.x)&&(point.x<=pointLine2.x))||((point.x<=pointLine1.x)&&(point.x>=pointLine2.x))))
		{
			a = (pointLine2.y-pointLine1.y)/(pointLine2.x-pointLine1.x);
			b = pointLine1.y - a*pointLine1.x;
			y = a*point.x + b;
			if(y>point.y+TOL)
			{
				return eFace_UP;			
			}
			else if(y<point.y-TOL)
			{
				return eFace_DOWN;
			}
			else
			{
				return eFace_ON;
			}
		}
		else
		{
			return eFace_NONE;
		}
	}
};
#endif

