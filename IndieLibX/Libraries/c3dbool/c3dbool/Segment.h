/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef __Segment_H__
#define __Segment_H__

#include "Line.h"
/**
 * Represents a line segment resulting from a intersection of a face and a plane.
 * 
 * <br><br>See: 
 * D. H. Laidlaw, W. B. Trumbore, and J. F. Hughes.  
 * "Constructive Solid Geometry for Polyhedral Objects" 
 * SIGGRAPH Proceedings, 1986, p.161. 
 * 
 * @author Danilo Balby Silva Castanheira (danbalby@yahoo.com)
 */
class Segment : public Object// implements Cloneable
{
	/** line resulting from the two planes intersection */
	public:
	Line line;
	/** shows how many ends were already defined */
 	int index;
	
	/** distance from the segment starting point to the point defining the plane */
 	__DOUBLE__ startDist;
	/** distance from the segment ending point to the point defining the plane */
 	__DOUBLE__ endDist;
	
	/** starting point status relative to the face */
 	int startType;
	/** intermediate status relative to the face */
 	int middleType;
	/** ending point status relative to the face */
 	int endType;
	
	/** nearest vertex from the starting point */
 	c3dbool_Vertex startVertex;
	/** nearest vertex from the ending point */
 	c3dbool_Vertex endVertex; 
	
	/** start of the intersection point */
 	Point3d startPos;
	/** end of the intersection point */
 	Point3d endPos;
	Segment(const Segment& rhs)
	{
		C3D_FNLN;
		line=rhs.line;
		index=rhs.index;
		startDist=rhs.startDist;
		endDist=rhs.endDist;
		startType=rhs.startType;
		middleType=rhs.middleType;
		endType=rhs.endType;
		startVertex=rhs.startVertex;
		endVertex=rhs.endVertex;
		startPos=rhs.startPos;
		endPos=rhs.endPos;		
	}
	
	enum eSegment
	{
	/** define as vertex one of the segment ends */
		eSegment_VERTEX,
	/** define as face one of the segment ends */
		eSegment_FACE,
	/** define as edge one of the segment ends */
		eSegment_EDGE,
	};
	
	//---------------------------------CONSTRUCTORS---------------------------------//
		
	/**
	 * Constructs a Segment based on elements obtained from the two planes relations 
	 * 
	 * @param line resulting from the two planes intersection
	 * @param face face that intersects with the plane
	 * @param sign1 position of the face vertex1 relative to the plane (-1 behind, 1 front, 0 on)
	 * @param sign2 position of the face vertex1 relative to the plane (-1 behind, 1 front, 0 on)
	 * @param sign3 position of the face vertex1 relative to the plane (-1 behind, 1 front, 0 on)  
	 */
	Segment();
	Segment(Line line, Face face, int sign1, int sign2, int sign3);
	
	//-----------------------------------OVERRIDES----------------------------------//
	
	/**
	 * Clones the Segment object
	 * 
	 * @return cloned Segment object
	 */
	Object clone();
	
	//-------------------------------------GETS-------------------------------------//
	
	/**
	 * Gets the start vertex
	 * 
	 * @return start vertex
	 */
	c3dbool_Vertex getStartVertex();
	
	/**
	 * Gets the end vertex
	 * 
	 * @return end vertex
	 */
	c3dbool_Vertex getEndVertex();
	
	/**
	 * Gets the distance from the origin until the starting point
	 * 
	 * @return distance from the origin until the starting point
	 */
	__DOUBLE__ getStartDistance();
	
	/**
	 * Gets the distance from the origin until ending point
	 * 
	 * @return distance from the origin until the ending point
	 */
	__DOUBLE__ getEndDistance();
	
	/**
	 * Gets the type of the starting point
	 * 
	 * @return type of the starting point
	 */
	int getStartType();
	
	/**
	 * Gets the type of the segment between the starting and ending points
	 * 
	 * @return type of the segment between the starting and ending points
	 */
	int getIntermediateType();
	
	/**
	 * Gets the type of the ending point
	 * 
	 * @return type of the ending point
	 */
	int getEndType();
	
	/**
	 * Gets the number of ends already set
	 *
	 * @return number of ends already set
	 */
	int getNumEndsSet();
	
	/**
	 * Gets the starting position
	 * 
	 * @return start position
	 */
	Point3d getStartPosition();
	
	/**
	 * Gets the ending position
	 * 
	 * @return ending position
	 */
	Point3d getEndPosition();
	
	//------------------------------------OTHERS------------------------------------//
	
	/**
	 * Checks if two segments intersect
	 * 
	 * @param segment the other segment to check the intesection
	 * @return true if the segments intersect, false otherwise
	 */
	bool intersect(Segment segment);
	
	//---------------------------------PRIVATES-------------------------------------//
	
	/**
	 * Sets an end as vertex (starting point if none end were defined, ending point otherwise)
	 * 
	 * @param vertex the vertex that is an segment end 
	 * @return false if all the ends were already defined, true otherwise
	 */
 	bool setVertex(c3dbool_Vertex vertex);
	
	/**
	 * Sets an end as edge (starting point if none end were defined, ending point otherwise)
	 * 
	 * @param vertex1 one of the vertices of the intercepted edge 
	 * @param vertex2 one of the vertices of the intercepted edge
	 * @return false if all ends were already defined, true otherwise
	 */
 	bool setEdge(c3dbool_Vertex vertex1, c3dbool_Vertex vertex2);
	
	/** Swaps the starting point and the ending point */	
 	void swapEnds();
};
#endif

