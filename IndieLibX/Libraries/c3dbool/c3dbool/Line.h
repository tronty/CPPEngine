/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef __Line_H__
#define __Line_H__

inline __DOUBLE__ randd() {
    return (__DOUBLE__)stx_rand() / ((__DOUBLE__)(RAND_MAX)+(__DOUBLE__)(1));
}
/**
 * Representation of a 3d line or a ray (represented by a direction and a point).
 * 
 * <br><br>See: 
 * D. H. Laidlaw, W. B. Trumbore, and J. F. Hughes.  
 * "Constructive Solid Geometry for Polyhedral Objects" 
 * SIGGRAPH Proceedings, 1986, p.161.
 *  
 * @author Danilo Balby Silva Castanheira (danbalby@yahoo.com)
 */
class Line : public Object// implements Cloneable
{
public:
	/** a line point */
	Point3d point;
	/** line direction */
	Vector3d direction;

	Line(const Line& rhs)
	{
		C3D_FNLN;
		point=rhs.point;
		direction=rhs.direction;
	}
	
	//----------------------------------CONSTRUCTORS---------------------------------//
	Line() : Object()
	{
		C3D_FNLN;
		Vector3d adirection;
		Point3d apoint;
		direction = adirection;
		point = apoint;
		direction.normalize();
	}
	
	/**
	 * Constructor for a line. The line created is the intersection between two planes 
	 * 
	 * @param face1 face representing one of the planes 
	 * @param face2 face representing one of the planes
	 */
	Line(Face face1, Face face2) : Object()
	{
		C3D_FNLN;
		Vector3d normalFace1 = face1.getNormal();
		Vector3d normalFace2 = face2.getNormal();
		
		//direction: cross product of the faces normals
		direction = Vector3d(); 
		//direction.cross(normalFace1, normalFace2);
		D3DXFROMWINEVec3Cross(&direction, &normalFace1, &normalFace2);
				
		//if direction lenght is not zero (the planes aren't parallel )...
		if (!(D3DXFROMWINEVec3Length(&direction)<TOL))
		{
			//getting a line point, zero is set to a coordinate whose direction 
			//component isn't zero (line intersecting its origin plan)
			point = Point3d();
			__DOUBLE__ d1 = -(normalFace1.x*face1.v1.x + normalFace1.y*face1.v1.y + normalFace1.z*face1.v1.z);
			__DOUBLE__ d2 = -(normalFace2.x*face2.v1.x + normalFace2.y*face2.v1.y + normalFace2.z*face2.v1.z);
			if(fabs(direction.x)>TOL)
			{
				point.x = 0.0f;
				point.y = (d2*normalFace1.z - d1*normalFace2.z)/direction.x;
				point.z = (d1*normalFace2.y - d2*normalFace1.y)/direction.x;
			}
			else if(fabs(direction.y)>TOL)
			{
				point.x = (d1*normalFace2.z - d2*normalFace1.z)/direction.y;
				point.y = 0.0f;
				point.z = (d2*normalFace1.x - d1*normalFace2.x)/direction.y;
			}
			else
			{
				point.x = (d2*normalFace1.y - d1*normalFace2.y)/direction.z;
				point.y = (d1*normalFace2.x - d2*normalFace1.x)/direction.z;
				point.z = 0.0f;
			}
		}
				
		direction.normalize();
	}
	
	/**
	 * Constructor for a ray
	 * 
	 * @param direction direction ray
	 * @param point beginning of the ray
	 */
	Line(Vector3d adirection, Point3d apoint) : Object()
	{
		C3D_FNLN;
		direction = adirection;
		point = apoint;
		direction.normalize();
	}
	
	//---------------------------------OVERRIDES------------------------------------//
	
	/**
	 * Clones the Line object
	 * 
	 * @return cloned Line object
	 */
	Object clone()
	{
		C3D_FNLN;
		//try
		{
			Line clone;
			clone.direction = Vector3d();
			clone.point = Point3d();
			return clone;
		} /*
		catch(CloneNotSupportedException e)
		{	
			return 0;
		} */
	}
	
	/**
	 * Makes a string definition for the Line object
	 * 
	 * @return the string definition
	 */
	std::string toString()
	{
		C3D_FNLN;
		char buf[256];
		stx_snprintf(buf, 256, "Direction: %f %f %f\nPoint: %f %f %f\n", direction.x, direction.y, direction.z, point.x, point.y, point.z);
		return buf;
	}
	
	//-----------------------------------GETS---------------------------------------//
	
	/**
	 * Gets the point used to represent the line
	 * 
	 * @return point used to represent the line
	 */
	Point3d getPoint()
	{
		C3D_FNLN;
		return point;
	}
	
	/**
	 * Gets the line direction
	 * 
	 * @return line direction
	 */
	Vector3d getDirection()
	{
		C3D_FNLN;
		return direction;
	}
	
	//-----------------------------------SETS---------------------------------------//
	
	/**
	 * Sets a new point
	 * 
	 * @param point new point
	 */
	void setPoint(Point3d apoint)
	{
		C3D_FNLN;
		point = apoint;
	}
	
	/**
	 * Sets a new direction
	 * 
	 * @param direction new direction
	 */
	void setDirection(Vector3d adirection)
	{
		C3D_FNLN;
		direction = adirection;
	}
		
	//--------------------------------OTHERS----------------------------------------//
		
	/**
	 * Computes the distance from the line point to another point
	 * 
	 * @param otherPoint the point to compute the distance from the line point. The point 
	 * is supposed to be on the same line.
	 * @return points distance. If the point submitted is behind the direction, the 
	 * distance is negative 
	 */
	__DOUBLE__ computePointToPointDistance(Point3d otherPoint)
	{
		C3D_FNLN;
#if 0
		__DOUBLE__ distance = otherPoint.getDistance(point);
#else
      __DOUBLE__ distance = sqrtf((point.x - otherPoint.x) * (point.x - otherPoint.x) +
                   (point.y - otherPoint.y) * (point.y - otherPoint.y) +
                   (point.z - otherPoint.z) * (point.z - otherPoint.z));
#endif
		#if 0
		//LOG_FNLN;
		//LOG_PRINT("point.x=%f\n", point.x);
		//LOG_PRINT("point.y=%f\n", point.y);
		//LOG_PRINT("point.z=%f\n", point.z);
		//LOG_PRINT("otherPoint.x=%f\n", otherPoint.x);
		//LOG_PRINT("otherPoint.y=%f\n", otherPoint.y);
		//LOG_PRINT("otherPoint.z=%f\n", otherPoint.z);
		
		//LOG_PRINT("distance=%f\n", distance);
		#endif
		Vector3d vec = Vector3d(otherPoint.x - point.x, otherPoint.y - point.y, otherPoint.z - point.z);
		vec.normalize();
		if (D3DXFROMWINEVec3Dot(&vec, &direction)<0.0f)
		{
			return -distance;			
		}
		else
		{
			return distance;
		}
		return 0;
	}
	
	/**
	 * Computes the point resulting from the intersection with another line
	 * 
	 * @param otherLine the other line to apply the intersection. The lines are supposed
	 * to intersect
	 * @return point resulting from the intersection. If the point coundn't be obtained, return 0
	 */
	Point3d* computeLineIntersection(Line otherLine)
	{
		C3D_FNLN;
		//x = x1 + a1*t = x2 + b1*s
		//y = y1 + a2*t = y2 + b2*s
		//z = z1 + a3*t = z2 + b3*s
		
		Point3d linePoint = otherLine.getPoint(); 
		Vector3d lineDirection = otherLine.getDirection();
				
		__DOUBLE__ t;
		if(fabs(direction.y*lineDirection.x-direction.x*lineDirection.y)>TOL)
		{
			t = (-point.y*lineDirection.x+linePoint.y*lineDirection.x+lineDirection.y*point.x-lineDirection.y*linePoint.x)/(direction.y*lineDirection.x-direction.x*lineDirection.y);
		}
		else if (fabs(-direction.x*lineDirection.z+direction.z*lineDirection.x)>TOL)
		{
			t=-(-lineDirection.z*point.x+lineDirection.z*linePoint.x+lineDirection.x*point.z-lineDirection.x*linePoint.z)/(-direction.x*lineDirection.z+direction.z*lineDirection.x);
		}
		else if (fabs(-direction.z*lineDirection.y+direction.y*lineDirection.z)>TOL)
		{
			t = (point.z*lineDirection.y-linePoint.z*lineDirection.y-lineDirection.z*point.y+lineDirection.z*linePoint.y)/(-direction.z*lineDirection.y+direction.y*lineDirection.z);
		}
		else return 0;
		
		__DOUBLE__ x = point.x + direction.x*t;
		__DOUBLE__ y = point.y + direction.y*t;
		__DOUBLE__ z = point.z + direction.z*t;
				
		return new Point3d(x,y,z);
	}
	
	/**
	 * Compute the point resulting from the intersection with a plane
	 * 
	 * @param normal the plane normal
	 * @param planePoint a plane point. 
	 * @return intersection point. If they don't intersect, return 0
	 */
	void computePlaneIntersection(Point3d* aPoint, Vector3d normal, Point3d planePoint)
	{
		C3D_FNLN;
		//Ax + By + Cz + D = 0
		//x = x0 + t(x1 – x0)
		//y = y0 + t(y1 – y0)
		//z = z0 + t(z1 – z0)
		//(x1 - x0) = dx, (y1 - y0) = dy, (z1 - z0) = dz
		//t = -(A*x0 + B*y0 + C*z0 )/(A*dx + B*dy + C*dz)
		
		__DOUBLE__ A = normal.x;
		__DOUBLE__ B = normal.y;
		__DOUBLE__ C = normal.z;
		__DOUBLE__ D = -(normal.x*planePoint.x+normal.y*planePoint.y+normal.z*planePoint.z);
			
		__DOUBLE__ numerator = A*point.x + B*point.y + C*point.z + D;
		__DOUBLE__ denominator = A*direction.x + B*direction.y + C*direction.z;
				
		//if line is paralel to the plane...
		if(fabs(denominator)<TOL)
		{
			//if line is contained in the plane...
			if(fabs(numerator)<TOL)
			{
				aPoint=new Point3d(point.x, point.y, point.z);
				return;
			}
			else
			{
				aPoint=0;
				return;
			}
		}
		//if line intercepts the plane...
		else
		{
			__DOUBLE__ t = -numerator/denominator;
			Point3d resultPoint = Point3d();
			resultPoint.x = point.x + t*direction.x; 
			resultPoint.y = point.y + t*direction.y;
			resultPoint.z = point.z + t*direction.z;
			
			aPoint=new Point3d(resultPoint.x, resultPoint.y, resultPoint.z);
			return;
		}
		return;
	}
	
	/** Changes slightly the line direction */
	void perturbDirection()
	{
		C3D_FNLN;
		LOG_FNLN;	
		LOG_PRINT("direction1.x=%f\n", direction.x);
		LOG_PRINT("direction1.y=%f\n", direction.y);
		LOG_PRINT("direction1.z=%f\n", direction.z);
		#if 1
		direction.x += 1e-5*randd();			
		direction.y += 1e-5*randd();
		direction.z += 1e-5*randd();
		#else
		direction.x += 10.0f*randd();			
		direction.y += 10.0f*randd();
		direction.z += 10.0f*randd();
		#endif
		LOG_PRINT("direction2.x=%f\n", direction.x);
		LOG_PRINT("direction2.y=%f\n", direction.y);
		LOG_PRINT("direction2.z=%f\n", direction.z);
	}
};
#endif

