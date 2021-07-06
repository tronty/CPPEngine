/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include "../c3dbool/Vertex.h"
#include "../c3dbool/Face.h"
#include "../c3dbool/Solid.h"
#include "../c3dbool/Bound.h"
#include "../c3dbool/Segment.h"
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
	Segment::Segment() : Object()
	{
		C3D_FNLN;
		Line line;Face face;
		Segment(line, face, 0, 0, 0);
	}
	Segment::Segment(Line line, Face face, int sign1, int sign2, int sign3) : Object()
	{
		C3D_FNLN;
		line = line;
		index = 0;
		
		//VERTEX is an end
		if(sign1 == 0)
		{
			setVertex(face.v1);
			//other vertices on the same side - VERTEX-VERTEX VERTEX
			if(sign2 == sign3)
			{
				setVertex(face.v1);
			}
		}
		
		//VERTEX is an end
		if(sign2 == 0)
		{
			setVertex(face.v2);
			//other vertices on the same side - VERTEX-VERTEX VERTEX
			if(sign1 == sign3)
			{
				setVertex(face.v2);
			}
		}
		
		//VERTEX is an end
		if(sign3 == 0)
		{
			setVertex(face.v3);
			//other vertices on the same side - VERTEX-VERTEX VERTEX
			if(sign1 == sign2)
			{
				setVertex(face.v3);
			}
		}
		
		//There are undefined ends - one or more edges cut the planes intersection line
		if(getNumEndsSet() != 2)
		{
			//EDGE is an end
			if((sign1==1 && sign2==-1)||(sign1==-1 && sign2==1))
			{
				setEdge(face.v1, face.v2);
			}
			//EDGE is an end
			if((sign2==1 && sign3==-1)||(sign2==-1 && sign3==1))
			{
				setEdge(face.v2, face.v3);
			}
			//EDGE is an end
			if((sign3==1 && sign1==-1)||(sign3==-1 && sign1==1))
			{
				setEdge(face.v3, face.v1);
			}
		}
	}
	
	//-----------------------------------OVERRIDES----------------------------------//
	
	/**
	 * Clones the Segment object
	 * 
	 * @return cloned Segment object
	 */
	Object Segment::clone()
	{
		C3D_FNLN;
		//try
		{
			Segment clone;
			clone.line = line;
			clone.index = index;
			clone.startDist = startDist;
			clone.endDist = endDist;
			clone.startDist = startType;
			clone.middleType = middleType;
			clone.endType = endType;
			clone.startVertex = startVertex;
			clone.endVertex = endVertex;
			clone.startPos = startPos;
			clone.endPos = endPos;
			
			return clone;
		} /*
		catch(CloneNotSupportedException e)
		{	
			return 0;
		} */
	}
	
	//-------------------------------------GETS-------------------------------------//
	
	/**
	 * Gets the start vertex
	 * 
	 * @return start vertex
	 */
	c3dbool_Vertex Segment::getStartVertex()
	{
		C3D_FNLN;
		return startVertex;
	}
	
	/**
	 * Gets the end vertex
	 * 
	 * @return end vertex
	 */
	c3dbool_Vertex Segment::getEndVertex()
	{
		C3D_FNLN;
		return endVertex;
	}
	
	/**
	 * Gets the distance from the origin until the starting point
	 * 
	 * @return distance from the origin until the starting point
	 */
	__DOUBLE__ Segment::getStartDistance()
	{
		C3D_FNLN;
		return startDist;
	}
	
	/**
	 * Gets the distance from the origin until ending point
	 * 
	 * @return distance from the origin until the ending point
	 */
	__DOUBLE__ Segment::getEndDistance()
	{
		C3D_FNLN;
		return endDist;
	}
	
	/**
	 * Gets the type of the starting point
	 * 
	 * @return type of the starting point
	 */
	int Segment::getStartType()
	{
		C3D_FNLN;
		return startType;
	}
	
	/**
	 * Gets the type of the segment between the starting and ending points
	 * 
	 * @return type of the segment between the starting and ending points
	 */
	int Segment::getIntermediateType()
	{
		C3D_FNLN;
		return middleType;
	}
	
	/**
	 * Gets the type of the ending point
	 * 
	 * @return type of the ending point
	 */
	int Segment::getEndType()
	{
		C3D_FNLN;
		return endType;
	}
	
	/**
	 * Gets the number of ends already set
	 *
	 * @return number of ends already set
	 */
	int Segment::getNumEndsSet()
	{
		C3D_FNLN;
		return index;
	}
	
	/**
	 * Gets the starting position
	 * 
	 * @return start position
	 */
	Point3d Segment::getStartPosition()
	{
		C3D_FNLN;
		return startPos;
	}
	
	/**
	 * Gets the ending position
	 * 
	 * @return ending position
	 */
	Point3d Segment::getEndPosition()
	{
		C3D_FNLN;
		return endPos;
	}
	
	//------------------------------------OTHERS------------------------------------//
	
	/**
	 * Checks if two segments intersect
	 * 
	 * @param segment the other segment to check the intesection
	 * @return true if the segments intersect, false otherwise
	 */
	bool Segment::Segment::intersect(Segment segment)
	{
		C3D_FNLN;
		if(endDist<segment.startDist+TOL ||segment.endDist<startDist+TOL)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	
	//---------------------------------PRIVATES-------------------------------------//
	
	/**
	 * Sets an end as vertex (starting point if none end were defined, ending point otherwise)
	 * 
	 * @param vertex the vertex that is an segment end 
	 * @return false if all the ends were already defined, true otherwise
	 */
 	bool Segment::setVertex(c3dbool_Vertex vertex)
	{
		C3D_FNLN;
		//none end were defined - define starting point as VERTEX
		if(index == 0)
		{
			startVertex = vertex;
		 	startType = eSegment_VERTEX;
		 	startDist = line.computePointToPointDistance(vertex.getPosition());
		 	startPos = startVertex.getPosition();
		 	index++;
		 	return true;
		}
		//starting point were defined - define ending point as VERTEX
		if(index == 1)
		{
			endVertex = vertex;
			endType = eSegment_VERTEX;
			endDist = line.computePointToPointDistance(vertex.getPosition());
			endPos = endVertex.getPosition();
			index++;
			
			//defining middle based on the starting point
			//VERTEX-VERTEX-VERTEX
			if(startVertex.equals(endVertex))
			{
				middleType = eSegment_VERTEX;
			}
			//VERTEX-EDGE-VERTEX
			else if(startType==eSegment_VERTEX)
			{
				middleType = eSegment_EDGE;
			}
			
			//the ending point distance should be smaller than  starting point distance 
			if(startDist>endDist)
			{
				swapEnds();
			}
			
			return true;
		}
		else
		{
			return false;
		}
	}
	
	/**
	 * Sets an end as edge (starting point if none end were defined, ending point otherwise)
	 * 
	 * @param vertex1 one of the vertices of the intercepted edge 
	 * @param vertex2 one of the vertices of the intercepted edge
	 * @return false if all ends were already defined, true otherwise
	 */
 	bool Segment::setEdge(c3dbool_Vertex vertex1, c3dbool_Vertex vertex2)
	{
		C3D_FNLN;
		Point3d point1 = vertex1.getPosition();
		Point3d point2 = vertex2.getPosition();
		Vector3d edgeDirection = Vector3d(point2.x - point1.x, point2.y - point1.y, point2.z - point1.z);
		Line edgeLine = Line(edgeDirection, point1);
		
		if(index==0)
		{
			startVertex = vertex1;
			startType = eSegment_EDGE;
			startPos = *line.computeLineIntersection(edgeLine);
			startDist = line.computePointToPointDistance(startPos);
			middleType = eSegment_FACE;
			index++;
			return true;		
		}
		else if(index==1)
		{
			endVertex = vertex1;
			endType = eSegment_EDGE;
			endPos = *line.computeLineIntersection(edgeLine);
			endDist = line.computePointToPointDistance(endPos);
			middleType = eSegment_FACE;
			index++;
			
			//the ending point distance should be smaller than  starting point distance 
			if(startDist>endDist)
			{
			  	swapEnds();
			}
			
			return true;
		}
		else
		{
			return false;
		}
	}
	
	/** Swaps the starting point and the ending point */	
 	void Segment::swapEnds()
	{
		C3D_FNLN;
		__DOUBLE__ distTemp = startDist;
		startDist = endDist;
		endDist = distTemp;
		
		int typeTemp = startType;
		startType = endType;
		endType = typeTemp;
		
		c3dbool_Vertex vertexTemp = startVertex;
		startVertex = endVertex;
		endVertex = vertexTemp;
		
		Point3d posTemp = startPos;
		startPos = endPos;
		endPos = posTemp;		
	}

