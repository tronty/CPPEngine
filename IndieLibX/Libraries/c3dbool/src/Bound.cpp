/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include "../c3dbool/Vertex.h"
#include "../c3dbool/Face.h"
#include "../c3dbool/Solid.h"
#include "../c3dbool/Bound.h"
/**
 * Representation of a bound - the extremes of a 3d component for each coordinate.
 *
 * <br><br>See: 
 * D. H. Laidlaw, W. B. Trumbore, and J. F. Hughes.  
 * "Constructive Solid Geometry for Polyhedral Objects" 
 * SIGGRAPH Proceedings, 1986, p.161. 
 *  
 * @author Danilo Balby Silva Castanheira (danbalby@yahoo.com) 
 */
	
	//---------------------------------CONSTRUCTORS---------------------------------//
	
	/**
	 * Bound constructor for a face
	 * 
	 * @param p1 point relative to the first vertex
	 * @param p2 point relative to the second vertex
	 * @param p3 point relative to the third vertex
	 */	
	Bound::Bound() : Object()
	{
		C3D_FNLN;
		Point3d p1;
		Point3d p2;
		Point3d p3;
		xMax = xMin = p1.x;
		yMax = yMin = p1.y;
		zMax = zMin = p1.z;
		
		checkVertex(p2);
		checkVertex(p3);
	}
	Bound::Bound(Point3d p1, Point3d p2, Point3d p3) : Object()
	{
		C3D_FNLN;
		xMax = xMin = p1.x;
		yMax = yMin = p1.y;
		zMax = zMin = p1.z;
		
		checkVertex(p2);
		checkVertex(p3);
	}
	
	/**
	 * Bound constructor for a object 3d
	 * 
	 * @param vertices the object vertices
	 */
	Bound::Bound(std::vector<Point3d> vertices) : Object()
	{
		C3D_FNLN;
		xMax = xMin = vertices[0].x;
		yMax = yMin = vertices[0].y;
		zMax = zMin = vertices[0].z;
		
		for(int i=1;i<vertices.size();i++)
		{
			checkVertex(vertices[i]);
		}
	}
	
	//----------------------------------OVERRIDES-----------------------------------//
	
	/**
	 * Makes a string definition for the bound object
	 * 
	 * @return the string definition
	 */
	std::string Bound::toString()
	{
		C3D_FNLN;
		char buf[256];
		stx_snprintf(buf, 256, "x: %f .. %f\ny: %f .. %f\nz: %f .. %f)\n", xMin, xMax, yMin, yMax, zMin, zMax);
		return buf;
	}
	
	//--------------------------------------OTHERS----------------------------------//
	
	/**
	 * Checks if a bound overlaps other one
	 * 
	 * @param bound other bound to make the comparison
	 * @return true if they insersect, false otherwise
	 */
	bool Bound::overlap(Bound bound)
	{
		C3D_FNLN;
		if((xMin>bound.xMax+TOL)||(xMax<bound.xMin-TOL)||(yMin>bound.yMax+TOL)||(yMax<bound.yMin-TOL)||(zMin>bound.zMax+TOL)||(zMax<bound.zMin-TOL))
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	
	//-------------------------------------PRIVATES---------------------------------//
	
	/**
	 * Checks if one of the coordinates of a vertex exceed the ones found before 
	 * 
	 * @param vertex vertex to be tested
	 */
	void Bound::checkVertex(Point3d vertex)
	{
		C3D_FNLN;
		if(vertex.x>xMax)
		{
			xMax = vertex.x;
		}
		else if(vertex.x<xMin)
		{
			xMin = vertex.x;
		}
		
		if(vertex.y>yMax)
		{
			yMax = vertex.y;
		}
		else if(vertex.y<yMin)
		{
			yMin = vertex.y;
		}
		
		if(vertex.z>zMax)
		{
			zMax = vertex.z;
		}
		else if(vertex.z<zMin)
		{
			zMin = vertex.z;
		}
	}

	Object::Object()
	{
		C3D_FNLN;
	}
	Object::~Object()
	{
		C3D_FNLN;
	}
	void Object::clone()
	{
		C3D_FNLN;
	}
	Bound* Object::getBound()
	{
		C3D_FNLN;
		Point3d p1(0.0f, 0.0f, 0.0f);
		Point3d p2(0.0f, 0.0f, 0.0f);
		Point3d p3(0.0f, 0.0f, 0.0f);
		return new Bound(p1, p2, p3);
	}
