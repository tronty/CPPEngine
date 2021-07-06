/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef __Bound_H__
#define __Bound_H__
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
class Bound : public Object
{
public:
	/** maximum from the x coordinate */
	__DOUBLE__ xMax;
	/** minimum from the x coordinate */
	__DOUBLE__ xMin;
	/** maximum from the y coordinate */
	__DOUBLE__ yMax;
	/** minimum from the y coordinate */
	__DOUBLE__ yMin;
	/** maximum from the z coordinate */
	__DOUBLE__ zMax;
	/** minimum from the z coordinate */
	__DOUBLE__ zMin;
	Bound(const Bound& rhs)
	{
		C3D_FNLN;
		xMax=rhs.xMax;
		xMin=rhs.xMin;
		yMax=rhs.yMax;
		yMin=rhs.yMin;
		zMax=rhs.zMax;
		zMin=rhs.zMin;
	}
	
	//---------------------------------CONSTRUCTORS---------------------------------//
	
	/**
	 * Bound constructor for a face
	 * 
	 * @param p1 point relative to the first vertex
	 * @param p2 point relative to the second vertex
	 * @param p3 point relative to the third vertex
	 */
	Bound();	
	Bound(Point3d p1, Point3d p2, Point3d p3);
	
	/**
	 * Bound constructor for a object 3d
	 * 
	 * @param vertices the object vertices
	 */
	Bound(std::vector<Point3d> vertices);
	
	//----------------------------------OVERRIDES-----------------------------------//
	
	/**
	 * Makes a string definition for the bound object
	 * 
	 * @return the string definition
	 */
	std::string toString();
	
	//--------------------------------------OTHERS----------------------------------//
	
	/**
	 * Checks if a bound overlaps other one
	 * 
	 * @param bound other bound to make the comparison
	 * @return true if they insersect, false otherwise
	 */
	bool overlap(Bound bound);
	
	//-------------------------------------PRIVATES---------------------------------//
	
	/**
	 * Checks if one of the coordinates of a vertex exceed the ones found before 
	 * 
	 * @param vertex vertex to be tested
	 */
	void checkVertex(Point3d vertex);
};
#endif

