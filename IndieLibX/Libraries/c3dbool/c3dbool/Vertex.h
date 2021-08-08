/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef __Vertex_H__
#define __Vertex_H__

#include <Framework3/IRenderer.h>

#define C3D_FNLN 
#define C3D_FNLN2
#define __DOUBLE__ double

typedef	D3DXFROMWINEVECTOR3 Point3d;
typedef	D3DXFROMWINEVECTOR3 Vector3d;

class Bound;
class Object
{
public:
	Object();
	virtual ~Object();
	void clone();
	Bound* getBound();
	virtual const std::string getString() const {return "Object";}
};

class Color3f : public D3DXFROMWINEVECTOR3
{
public:
	Color3f();
	Color3f(__DOUBLE__, __DOUBLE__, __DOUBLE__);
	bool equals(const Color3f& anObject);
	virtual const std::string getString() const;
};
/** 
 * Represents of a 3d face vertex.
 * 
 * <br><br>See: 
 * D. H. Laidlaw, W. B. Trumbore, and J. F. Hughes.  
 * "Constructive Solid Geometry for Polyhedral Objects" 
 * SIGGRAPH Proceedings, 1986, p.161. 
 * 
 * @author Danilo Balby Silva Castanheira (danbalby@yahoo.com)
 */
	/** tolerance value to test equalities */
	extern __DOUBLE__ TOL;
class c3dbool_Vertex : public Object
{
	public:
	/** vertex coordinate in X */
	__DOUBLE__ x;
	/** vertex coordinate in Y */
	__DOUBLE__ y;
	/** vertex coordinate in Z */
	__DOUBLE__ z;
	/** references to vertices conected to it by an edge  */
	//private:
	std::vector<c3dbool_Vertex> adjacentVertices;
	/** vertex status relative to other object */
	int status;
	/** vertex color */
	Color3f color;
	c3dbool_Vertex(const c3dbool_Vertex& rhs)
	{
		C3D_FNLN;
		x=rhs.x;
		y=rhs.y;
		z=rhs.z;
		adjacentVertices=rhs.adjacentVertices;
		status=rhs.status;

		#if 0	
		////LOG_FNLN;
		////LOG_PRINT("x=%f\n", x);
		////LOG_PRINT("y=%f\n", y);
		////LOG_PRINT("z=%f\n", z);
		#endif
	}
	
	virtual const std::string getString() const {return "Vertex";}
	
	enum eVertex
	{
	/** vertex status if it is still unknown */
		eVertex_UNKNOWN,
	/** vertex status if it is inside a solid */
		eVertex_INSIDE,
	/** vertex status if it is outside a solid */
		eVertex_OUTSIDE,
	/** vertex status if it on the boundary of a solid */
		eVertex_BOUNDARY,
	};
	
inline bool operator == (const c3dbool_Vertex& v) const
{
		C3D_FNLN;
    return 
	(fabs(x-v.x)<TOL) && 
	(fabs(y-v.y)<TOL) && 
	(fabs(z-v.z)<TOL);
}

inline bool operator != (const c3dbool_Vertex& v) const
{
		C3D_FNLN;
    return 
	(fabs(x-v.x)>TOL) || 
	(fabs(y-v.y)>TOL) || 
	(fabs(z-v.z)>TOL);
}

	//----------------------------------CONSTRUCTORS--------------------------------//
	
	/**
	* Constructs a vertex with unknown status
	* 
	* @param position vertex position
	* @param color vertex color
	*/
	c3dbool_Vertex() : Object()
	{
		C3D_FNLN;

		////LOG_FNLN;
		color = Color3f(0.0f, 0.0f, 0.0f);
		
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		
		adjacentVertices =  std::vector<c3dbool_Vertex>(0);
		status = UNKNOWN;
	}
	c3dbool_Vertex(Point3d position, Color3f acolor) : Object()
	{
		C3D_FNLN;
		color = acolor;
		
		x = position.x;
		y = position.y;
		z = position.z;

		#if 0
		////LOG_FNLN;
		////LOG_PRINT("x=%f\n", x);
		////LOG_PRINT("y=%f\n", y);
		////LOG_PRINT("z=%f\n", z);
		////LOG_PRINT("color.x=%f\n", color.x);
		////LOG_PRINT("color.y=%f\n", color.y);
		////LOG_PRINT("color.z=%f\n", color.z);
		#endif
		
		adjacentVertices =  std::vector<c3dbool_Vertex>(0);
		status = UNKNOWN;
	}
	
	/**
	* Constructs a vertex with unknown status
	* 
	* @param x coordinate on the x axis
	* @param y coordinate on the y axis
	* @param z coordinate on the z axis
	* @param color vertex color
	*/
	c3dbool_Vertex(__DOUBLE__ x, __DOUBLE__ y, __DOUBLE__ z, Color3f acolor) : Object()
	{
		C3D_FNLN;
		color = acolor;
				
		x = x;
		y = y;
		z = z;
		
		adjacentVertices =  std::vector<c3dbool_Vertex>(0);
		status = UNKNOWN;
	}
	
	/**
	* Constructs a vertex with definite status
	* 
	* @param position vertex position
	* @param color vertex color
	* @param status vertex status - UNKNOWN, BOUNDARY, INSIDE or OUTSIDE
	*/
	c3dbool_Vertex(Point3d position, Color3f acolor, int status) : Object()
	{
		C3D_FNLN;
		color = acolor;
		
		x = position.x;
		y = position.y;
		z = position.z;
		
		adjacentVertices =  std::vector<c3dbool_Vertex>(0);
		status = status;
	}
	
	/**
	* Constructs a vertex with a definite status
	* 
	* @param x coordinate on the x axis
	* @param y coordinate on the y axis
	* @param z coordinate on the z axis
	* @param color vertex color
	* @param status vertex status - UNKNOWN, BOUNDARY, INSIDE or OUTSIDE
	*/
	#if 1
	c3dbool_Vertex(__DOUBLE__ x, __DOUBLE__ y, __DOUBLE__ z, Color3f acolor, int status) : Object()
	{
		C3D_FNLN;
		color = acolor;
		
		x = x;
		y = y;
		z = z;
		
		adjacentVertices =  std::vector<c3dbool_Vertex>(0);
		status = status;
	}
	#else
	c3dbool_Vertex(float x, float y, float z, Color3f acolor, int status) : Object()
	{
		C3D_FNLN;
		color = acolor;
		
		x = x;
		y = y;
		z = z;
		
		adjacentVertices =  std::vector<c3dbool_Vertex>(0);
		status = status;
	}
	#endif
	
	//-----------------------------------OVERRIDES----------------------------------//
	
	/**
	* Clones the vertex object
	* 
	* @return cloned vertex object
	*/
	Object clone()
	{
		C3D_FNLN;
		//try
		{
			c3dbool_Vertex clone;
			clone.x = x;
			clone.y = y;
			clone.z = z;
			clone.color = color;
			clone.status = status;
			clone.adjacentVertices =  std::vector<c3dbool_Vertex>(0);
			for(int i=0;i<adjacentVertices.size();i++)
			{
				clone.adjacentVertices.push_back(((c3dbool_Vertex)adjacentVertices[i]));								
			}
			
			return clone;
		}/*
		catch(CloneNotSupportedException e)
		{	
			return 0;
		}*/
	}
	
	/**
	* Makes a string definition for the c3dbool_Vertex object
	* 
	* @return the string definition
	*/
	std::string toString()
	{
		C3D_FNLN;
		char buf[256];
		stx_snprintf(buf, 256, "(%f, %f, %f)\n", x, y, z);
		return buf;
	}
	
	/**
	* Checks if an vertex is equal to another. To be equal, they have to have the same
	* coordinates(with some tolerance) and color
	* 
	* @param anObject the other vertex to be tested
	* @return true if they are equal, false otherwise. 
	*/
	bool equals(Object anObject)
	{
		C3D_FNLN;
		if(anObject.getString()!="c3dbool_Vertex")
		{
			return false;
		}
		else
		{
			c3dbool_Vertex vertex = *(c3dbool_Vertex*)&anObject;
			return 	fabs(x-vertex.x)<TOL && fabs(y-vertex.y)<TOL 
					&& fabs(z-vertex.z)<TOL && color.equals(vertex.color); 			
		}
	}
	
	//--------------------------------------SETS------------------------------------//
		
	/**
	* Sets the vertex status
	* 
	* @param status vertex status - UNKNOWN, BOUNDARY, INSIDE or OUTSIDE
	*/
	void setStatus(int status)
	{
		C3D_FNLN;
		if(status>=eVertex_UNKNOWN && status<=eVertex_BOUNDARY)
		{
			status = status;	
		}
	}
	
	//--------------------------------------GETS------------------------------------//
	
	/**
	* Gets the vertex position
	* 
	* @return vertex position
	*/
	Point3d getPosition()
	{
		C3D_FNLN;
		#if 0
		//LOG_FNLN;
		//LOG_PRINT("x=%f\n", x);
		//LOG_PRINT("y=%f\n", y);
		//LOG_PRINT("z=%f\n", z);
		#endif
		return Point3d(x, y, z);
	} 
	
	/**
	* Gets an array with the adjacent vertices
	* 
	* @return array of the adjacent vertices 
	*/
	std::vector<c3dbool_Vertex> getAdjacentVertices()
	{
		C3D_FNLN;
		std::vector<c3dbool_Vertex> vertices(adjacentVertices.size());
		for(int i=0;i<adjacentVertices.size();i++)
		{
			vertices[i] = (c3dbool_Vertex)adjacentVertices[i]; 
		}
		return vertices;
	}
	
	/**
	* Gets the vertex status
	* 
	* @return vertex status - UNKNOWN, BOUNDARY, INSIDE or OUTSIDE
	*/	
	int getStatus()
	{
		C3D_FNLN;
		return status;
	}
	
	/**
	* Gets the vertex color
	* 
	* @return vertex color
	*/
	Color3f getColor()
	{
		C3D_FNLN;
		return color;
	}
	
	//----------------------------------OTHERS--------------------------------------//
	
	/**
	* Sets a vertex as being adjacent to it
	* 
	* @param adjacentVertex an adjacent vertex
	*/
	void addAdjacentVertex(c3dbool_Vertex adjacentVertex);
	
	/**
	* Sets the vertex status, setting equally the adjacent ones
	* 
	* @param status new status to be set
	*/
	void mark(int status)
	{
		C3D_FNLN;
		//mark vertex
		status = status;
		
		//mark adjacent vertices
		std::vector<c3dbool_Vertex> adjacentVerts = getAdjacentVertices();
		for(int i=0;i<adjacentVerts.size();i++)
		{
			if(adjacentVerts[i].getStatus()==eVertex_UNKNOWN)
			{
				adjacentVerts[i].mark(status);
			}
		}
	}
};
bool contains(const std::vector<c3dbool_Vertex>& av, const c3dbool_Vertex& a);
bool contains(const std::vector<D3DXFROMWINEVECTOR3>& av, const D3DXFROMWINEVECTOR3& a);
#endif

