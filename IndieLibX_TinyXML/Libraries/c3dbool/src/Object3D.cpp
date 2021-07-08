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
#include "../c3dbool/Object3D.h"

#if 1
__DOUBLE__ TOL = 5e-10f;
#else
__DOUBLE__ TOL = 0.000005f;
#endif
	
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
/**
 * Data structure about a 3d solid to apply bool operations in it.
 * 
 * <br><br>Tipically, two 'Object3d' objects are created to apply bool operation. The
 * methods splitFaces() and classifyFaces() are called in this sequence for both objects,
 * always using the other one as parameter. Then the faces from both objects are collected
 * according their status.
 * 
 * <br><br>See: 
 * D. H. Laidlaw, W. B. Trumbore, and J. F. Hughes.  
 * "Constructive Solid Geometry for Polyhedral Objects" 
 * SIGGRAPH Proceedings, 1986, p.161. 
 *  
 * @author Danilo Balby Silva Castanheira (danbalby@yahoo.com)
 */

	//----------------------------------CONSTRUCTOR---------------------------------//
	
	/** 
	* Constructs a Object3d object based on a solid file.
	*
	* @param solid solid used to construct the Object3d object  
	*/
	Object3D::Object3D() : Object(){}
	void Object3D::Initialize(Solid solid)
	{
		C3D_FNLN;
		c3dbool_Vertex v1, v2, v3, vertex;
		std::vector<Point3d> verticesPoints = solid.getVertices();
		std::vector<__WORD__> indices = solid.getIndices();
		std::vector<Color3f> colors = solid.getColors();
		std::vector<c3dbool_Vertex> verticesTemp =  std::vector<c3dbool_Vertex>(0);
		
		//create vertices
		vertices =  std::vector<c3dbool_Vertex>(0);
		for(int i=0;i<verticesPoints.size();i++)
		{
			vertex = addVertex(verticesPoints[i], colors[i], eVertex::eVertex_UNKNOWN);
			verticesTemp.push_back(vertex); 
		}
		
		//create faces
		faces =  std::vector<Face>(0);
		for(__WORD__ i=0; i<indices.size(); i=i+3)
		{
			v1 = (c3dbool_Vertex)verticesTemp[indices[i]];
			v2 = (c3dbool_Vertex)verticesTemp[indices[i+1]];
			v3 = (c3dbool_Vertex)verticesTemp[indices[i+2]];
			addFace(v1, v2, v3);
		}
		
		//create bound
		bound = Bound(verticesPoints);
	}
	
	//-----------------------------------OVERRIDES----------------------------------//
	
	/**
	* Clones the Object3D object
	* 
	* @return cloned Object3D object
	*/
	Object Object3D::clone()
	{
		C3D_FNLN;
		//try
		{
			Object3D cclone;
			cclone.vertices =  std::vector<c3dbool_Vertex>(0);
			for(int i=0;i<vertices.size();i++)
			{
				cclone.vertices.push_back(((c3dbool_Vertex)vertices[i]));
			}
			cclone.faces =  std::vector<Face>(0);
			for(__WORD__ i=0;i<faces.size();i++)
			{
				cclone.faces.push_back(((Face)faces[i]));
			}
			cclone.bound = bound;
			
			return cclone;
		}/*
		catch(CloneNotSupportedException e)
		{	
			return 0;
		}*/
	}
	
	//--------------------------------------GETS------------------------------------//
	
	/**
	* Gets the number of faces
	* 
	* @return number of faces
	*/
	__WORD__ Object3D::getNumFaces()
	{
		C3D_FNLN;
		return faces.size();
	}
	
	/**
	* Gets a face reference for a given position
	* 
	* @param index required face position
	* @return face reference , 0 if the position is invalid
	*/
	Face Object3D::getFace(__WORD__ index)
	{
		C3D_FNLN;
		return (Face)faces[index];
	}
	
	/**
	* Gets the solid bound
	* 
	* @return solid bound
	*/
	Bound Object3D::getBound()
	{
		C3D_FNLN;
		return bound;
	}
	
	//------------------------------------ADDS----------------------------------------//
		
	/**
	* Method used to add a face properly for internal methods
	* 
	* @param v1 a face vertex
	* @param v2 a face vertex
	* @param v3 a face vertex
	*/
	Face Object3D::addFace(c3dbool_Vertex v1, c3dbool_Vertex v2, c3dbool_Vertex v3)
	{
		C3D_FNLN;
		if(!(v1.equals(v2)||v1.equals(v3)||v2.equals(v3)))
		{
			Face face = Face(v1, v2, v3);
			if(face.getArea()>TOL)
			{		
				faces.push_back(face);
				return face;
			}
			else
			{
				return Face();
			}
		}
		else
		{
			return Face();
		}
	}
	
	/**
	* Method used to add a vertex properly for internal methods
	* 
	* @param pos vertex position
	* @param color vertex color
	* @param status vertex status
	* @return the vertex inserted (if a similar vertex already exists, this is returned)
	*/
	c3dbool_Vertex Object3D::addVertex(Point3d pos, Color3f color, int status)
	{
		C3D_FNLN;
		int i;
		//if already there is an equal vertex, it is not inserted
		c3dbool_Vertex vertex = c3dbool_Vertex(pos, color, status);
		for(i=0;i<vertices.size();i++)
		{
			if(vertex.equals(vertices[i])) break;			
		}
		if(i==vertices.size())
		{
			vertices.push_back(vertex);
			return vertex;
		}
		else
		{
			vertex = (c3dbool_Vertex)vertices[i];
			vertex.setStatus(status);
			return vertex;
		}
		
	}
		
	//-------------------------FACES_SPLITTING_METHODS------------------------------//
	
	/**
	* Split faces so that none face is intercepted by a face of other object
	* 
	* @param object the other object 3d used to make the split 
	*/
	void Object3D::splitFaces(Object3D object)
	{
		C3D_FNLN;
		Line line;
		Face face1, face2;
		std::vector<Segment> segments;
		Segment segment1, segment2;
		__DOUBLE__ distFace1Vert1, distFace1Vert2, distFace1Vert3, distFace2Vert1, distFace2Vert2, distFace2Vert3;
		int signFace1Vert1, signFace1Vert2, signFace1Vert3, signFace2Vert1, signFace2Vert2, signFace2Vert3;
		int numFacesBefore = getNumFaces();
		int numFacesStart = getNumFaces();
		int facesIgnored = 0;
						
		//if the objects bounds overlap...								
		if(getBound().overlap(object.getBound()))
		{			
			//for each object1 face...
			for(int i=0;i<getNumFaces();i++)
			{
				//if object1 face bound and object2 bound overlap ...
				face1 = getFace(i);
				
				if(face1.getBound().overlap(object.getBound()))
				{
					//for each object2 face...
					for(int j=0;j<object.getNumFaces();j++)
					{
						//if object1 face bound and object2 face bound overlap...  
						face2 = object.getFace(j);
						if(face1.getBound().overlap(face2.getBound()))
						{
							//PART I - DO TWO POLIGONS INTERSECT?
							//POSSIBLE RESULTS: INTERSECT, NOT_INTERSECT, COPLANAR
							
							//distance from the face1 vertices to the face2 plane
							distFace1Vert1 = computeDistance(face1.v1, face2);
							distFace1Vert2 = computeDistance(face1.v2, face2);
							distFace1Vert3 = computeDistance(face1.v3, face2);
							
							//distances signs from the face1 vertices to the face2 plane 
							signFace1Vert1 = (distFace1Vert1>TOL? 1 :(distFace1Vert1<-TOL? -1 : 0)); 
							signFace1Vert2 = (distFace1Vert2>TOL? 1 :(distFace1Vert2<-TOL? -1 : 0));
							signFace1Vert3 = (distFace1Vert3>TOL? 1 :(distFace1Vert3<-TOL? -1 : 0));
							
							//if all the signs are zero, the planes are coplanar
							//if all the signs are positive or negative, the planes do not intersect
							//if the signs are not equal...
							if (!(signFace1Vert1==signFace1Vert2 && signFace1Vert2==signFace1Vert3))
							{
								//distance from the face2 vertices to the face1 plane
								distFace2Vert1 = computeDistance(face2.v1, face1);
								distFace2Vert2 = computeDistance(face2.v2, face1);
								distFace2Vert3 = computeDistance(face2.v3, face1);
								
								//distances signs from the face2 vertices to the face1 plane
								signFace2Vert1 = (distFace2Vert1>TOL? 1 :(distFace2Vert1<-TOL? -1 : 0)); 
								signFace2Vert2 = (distFace2Vert2>TOL? 1 :(distFace2Vert2<-TOL? -1 : 0));
								signFace2Vert3 = (distFace2Vert3>TOL? 1 :(distFace2Vert3<-TOL? -1 : 0));
							
								//if the signs are not equal...
								if (!(signFace2Vert1==signFace2Vert2 && signFace2Vert2==signFace2Vert3))
								{
									line = Line(face1, face2);
							
									//intersection of the face1 and the plane of face2
									segment1 = Segment(line, face1, signFace1Vert1, signFace1Vert2, signFace1Vert3);
																	
									//intersection of the face2 and the plane of face1
									segment2 = Segment(line, face2, signFace2Vert1, signFace2Vert2, signFace2Vert3);
																
									//if the two segments intersect...
									if(segment1.intersect(segment2))
									{
										//PART II - SUBDIVIDING NON-COPLANAR POLYGONS
										int lastNumFaces = getNumFaces();
										splitFace(i, segment1, segment2);
																			
										//prevent from infinite loop (with a loss of faces...)
										//if(numFacesStart*20<getNumFaces())
										//{
										//	////LOG_PRINT("possible infinite loop situation: terminating faces split");
										//	return;
										//}
								
										//if the face in the position isn't the same, there was a break 
										if(face1!=getFace(i)) 
										{
											//if the generated solid is equal the origin...
											if(face1.equals(getFace(getNumFaces()-1)))
											{
												//return it to its position and jump it
												if(i!=(getNumFaces()-1))
												{
													LOG_FNLN;
													//faces.remove(getNumFaces()-1);
faces.erase(faces.begin() + getNumFaces()-1);// ???
													//faces.push_back(i, face1);
faces.insert(faces.begin()+i, face1);// ???
												}
												else
												{
													continue;
												}
											}
											//else: test next face
											else
											{
												i--;
												break;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	/**
	* Computes closest distance from a vertex to a plane
	* 
	* @param vertex vertex used to compute the distance
	* @param face face representing the plane where it is contained
	* @return the closest distance from the vertex to the plane
	*/
	__DOUBLE__ Object3D::computeDistance(c3dbool_Vertex vertex, Face face)
	{
		C3D_FNLN;
		Vector3d normal = face.getNormal();
		__DOUBLE__ a = normal.x;
		__DOUBLE__ b = normal.y;
		__DOUBLE__ c = normal.z;
		__DOUBLE__ d = -(a*face.v1.x + b*face.v1.y + c*face.v1.z);
		return a*vertex.x + b*vertex.y + c*vertex.z + d;
	}

	/**
	* Split an individual face
	* 
	* @param facePos face position on the array of faces
	* @param segment1 segment representing the intersection of the face with the plane
	* of another face
	* @return segment2 segment representing the intersection of other face with the
	* plane of the current face plane
	*/	 
	void Object3D::splitFace(int facePos, Segment segment1, Segment segment2)
	{
		C3D_FNLN;
		c3dbool_Vertex startPosVertex, endPosVertex;
		Point3d startPos, endPos;
		int startType, endType, middleType;
		__DOUBLE__ startDist, endDist;
		
		Face face = (Face)getFace(facePos);
		c3dbool_Vertex startVertex = segment1.getStartVertex();
		c3dbool_Vertex endVertex = segment1.getEndVertex();
		
		//starting point: deeper starting point 		
		if (segment2.getStartDistance() > segment1.getStartDistance()+TOL)
		{
			startDist = segment2.getStartDistance();
			startType = segment1.getIntermediateType();
			startPos = segment2.getStartPosition();
		}
		else
		{
			startDist = segment1.getStartDistance();
			startType = segment1.getStartType();
			startPos = segment1.getStartPosition();
		}
		
		//ending point: deepest ending point
		if (segment2.getEndDistance() < segment1.getEndDistance()-TOL)
		{
			endDist = segment2.getEndDistance();
			endType = segment1.getIntermediateType();
			endPos = segment2.getEndPosition();
		}
		else
		{
			endDist = segment1.getEndDistance();
			endType = segment1.getEndType();
			endPos = segment1.getEndPosition();
		}		
		middleType = segment1.getIntermediateType();
		
		//set vertex to BOUNDARY if it is start type		
		if (startType == Segment::eSegment_VERTEX)
		{
			startVertex.setStatus(c3dbool_Vertex::eVertex_BOUNDARY);
		}
				
		//set vertex to BOUNDARY if it is end type
		if (endType == Segment::eSegment_VERTEX)
		{
			endVertex.setStatus(c3dbool_Vertex::eVertex_BOUNDARY);
		}
		
		//VERTEX-_______-VERTEX 
		if (startType == Segment::eSegment_VERTEX && endType == Segment::eSegment_VERTEX)
		{
			return;
		}
		
		//______-EDGE-______
		else if (middleType == Segment::eSegment_EDGE)
		{
			//gets the edge 
			int splitEdge;
			if ((startVertex == face.v1 && endVertex == face.v2) || (startVertex == face.v2 && endVertex == face.v1))
			{
				splitEdge = 1;
			}
			else if ((startVertex == face.v2 && endVertex == face.v3) || (startVertex == face.v3 && endVertex == face.v2))
			{	 
				splitEdge = 2; 
			} 
			else
			{
				splitEdge = 3;
			} 
			
			//VERTEX-EDGE-EDGE
			if (startType == Segment::eSegment_VERTEX)
			{
				breakFaceInTwo(facePos, endPos, splitEdge);
				return;
			}
			
			//EDGE-EDGE-VERTEX
			else if (endType == Segment::eSegment_VERTEX)
			{
				breakFaceInTwo(facePos, startPos, splitEdge);
				return;
			}
        
			// EDGE-EDGE-EDGE
			else if (startDist == endDist)
			{
				breakFaceInTwo(facePos, endPos, splitEdge);
			}
			else
			{
				if((startVertex == face.v1 && endVertex == face.v2) || (startVertex == face.v2 && endVertex == face.v3) || (startVertex == face.v3 && endVertex == face.v1))
				{
					breakFaceInThree(facePos, startPos, endPos, splitEdge);
				}
				else
				{
					breakFaceInThree(facePos, endPos, startPos, splitEdge);
				}
			}
			return;
		}
		
		//______-FACE-______
		
		//VERTEX-FACE-EDGE
		else if (startType == Segment::eSegment_VERTEX && endType == Segment::eSegment_EDGE)
		{
			breakFaceInTwo(facePos, endPos, endVertex);
		}
		//EDGE-FACE-VERTEX
		else if (startType == Segment::eSegment_EDGE && endType == Segment::eSegment_VERTEX)
		{
			breakFaceInTwo(facePos, startPos, startVertex);
		}
		//VERTEX-FACE-FACE
		else if (startType == Segment::eSegment_VERTEX && endType == Segment::eSegment_FACE)
		{
			breakFaceInThree(facePos, endPos, startVertex);
		}
		//FACE-FACE-VERTEX
		else if (startType == Segment::eSegment_FACE && endType == Segment::eSegment_VERTEX)
		{
			breakFaceInThree(facePos, startPos, endVertex);
		}
		//EDGE-FACE-EDGE
		else if (startType == Segment::eSegment_EDGE && endType == Segment::eSegment_EDGE)
		{
			breakFaceInThree(facePos, startPos, endPos, startVertex, endVertex);
		}
		//EDGE-FACE-FACE
		else if (startType == Segment::eSegment_EDGE && endType == Segment::eSegment_FACE)
		{
			breakFaceInFour(facePos, startPos, endPos, startVertex);
		}
		//FACE-FACE-EDGE
		else if (startType == Segment::eSegment_FACE && endType == Segment::eSegment_EDGE)
		{
			breakFaceInFour(facePos, endPos, startPos, endVertex);
		}
		//FACE-FACE-FACE
		else if (startType == Segment::eSegment_FACE && endType == Segment::eSegment_FACE)
		{
			Vector3d segmentVector = Vector3d(startPos.x-endPos.x, startPos.y-endPos.y, startPos.z-endPos.z);
						
			//if the intersection segment is a point only...
			if (fabs(segmentVector.x)<TOL && fabs(segmentVector.y)<TOL && fabs(segmentVector.z)<TOL)
			{
				breakFaceInThree(facePos, startPos);
				return;
			}
			
			//gets the vertex more lined with the intersection segment
			int linedVertex;
			Point3d linedVertexPos;
			Vector3d vertexVector = Vector3d(endPos.x-face.v1.x, endPos.y-face.v1.y, endPos.z-face.v1.z);
			vertexVector.normalize();
			//__DOUBLE__ dot1 = fabs(segmentVector.dot(vertexVector));
			__DOUBLE__ dot1 = fabs(D3DXFROMWINEVec3Dot(&segmentVector, &vertexVector));
			vertexVector = Vector3d(endPos.x-face.v2.x, endPos.y-face.v2.y, endPos.z-face.v2.z);
			vertexVector.normalize();
			//__DOUBLE__ dot2 = fabs(segmentVector.dot(vertexVector));
			__DOUBLE__ dot2 = fabs(D3DXFROMWINEVec3Dot(&segmentVector, &vertexVector));
			vertexVector = Vector3d(endPos.x-face.v3.x, endPos.y-face.v3.y, endPos.z-face.v3.z);
			vertexVector.normalize();
			//__DOUBLE__ dot3 = fabs(segmentVector.dot(vertexVector));
			__DOUBLE__ dot3 = fabs(D3DXFROMWINEVec3Dot(&segmentVector, &vertexVector));
			if (dot1 > dot2 && dot1 > dot3)
			{
				linedVertex = 1;
				linedVertexPos = face.v1.getPosition();
			}
			else if (dot2 > dot3 && dot2 > dot1)
			{
				linedVertex = 2;
				linedVertexPos = face.v2.getPosition();
			}
			else
			{
				linedVertex = 3;
				linedVertexPos = face.v3.getPosition();
			}
        
			// Now find which of the intersection endpoints is nearest to that vertex.
			D3DXFROMWINEVECTOR3 d1=linedVertexPos-startPos;
			D3DXFROMWINEVECTOR3 d2=linedVertexPos-endPos;
			//if (linedVertexPos.distance(startPos) > linedVertexPos.distance(endPos))
			if (D3DXFROMWINEVec3Length(&d1) > D3DXFROMWINEVec3Length(&d2))
			{
				breakFaceInFive(facePos, startPos, endPos, linedVertex);
			}
			else
			{
				breakFaceInFive(facePos, endPos, startPos, linedVertex);
			}
		}
	}
	 
	/**
	* Face breaker for VERTEX-EDGE-EDGE / EDGE-EDGE-VERTEX
	* 
	* @param facePos face position on the faces array
	* @param newPos new vertex position
	* @param edge that will be split 
	*/		
	void Object3D::breakFaceInTwo(int facePos, Point3d newPos, int splitEdge)
	{
		C3D_FNLN;
		Face face = (Face)faces[facePos];
		//faces.remove(facePos);
		faces.erase(faces.begin() + facePos);
		
		c3dbool_Vertex vertex = addVertex(newPos, face.v1.getColor(), c3dbool_Vertex::eVertex_BOUNDARY); 
						
		if (splitEdge == 1)
		{
			addFace(face.v1, vertex, face.v3);
			addFace(vertex, face.v2, face.v3);
		}
		else if (splitEdge == 2)
		{
			addFace(face.v2, vertex, face.v1);
			addFace(vertex, face.v3, face.v1);
		}
		else
		{
			addFace(face.v3, vertex, face.v2);
			addFace(vertex, face.v1, face.v2);
		}
	}
	
	/**
	* Face breaker for VERTEX-FACE-EDGE / EDGE-FACE-VERTEX
	* 
	* @param facePos face position on the faces array
	* @param newPos new vertex position
	* @param endVertex vertex used for splitting 
	*/		
	void Object3D::breakFaceInTwo(int facePos, Point3d newPos, c3dbool_Vertex endVertex)
	{
		C3D_FNLN;
		Face face = (Face)faces[facePos];
		//faces.remove(facePos);
		faces.erase(faces.begin() + facePos);
		
		c3dbool_Vertex vertex = addVertex(newPos, face.v1.getColor(), c3dbool_Vertex::eVertex_BOUNDARY);
					
		if (endVertex.equals(face.v1))
		{
			addFace(face.v1, vertex, face.v3);
			addFace(vertex, face.v2, face.v3);
		}
		else if (endVertex.equals(face.v2))
		{
			addFace(face.v2, vertex, face.v1);
			addFace(vertex, face.v3, face.v1);
		}
		else
		{
			addFace(face.v3, vertex, face.v2);
			addFace(vertex, face.v1, face.v2);
		}
	}
	
	/**
	* Face breaker for EDGE-EDGE-EDGE
	* 
	* @param facePos face position on the faces array
	* @param newPos1 new vertex position
	* @param newPos2 new vertex position 
	* @param splitEdge edge that will be split
	*/
	void Object3D::breakFaceInThree(int facePos, Point3d newPos1, Point3d newPos2, int splitEdge)
	{
		C3D_FNLN;
		Face face = (Face)faces[facePos];
		//faces.remove(facePos);
		faces.erase(faces.begin() + facePos);

		c3dbool_Vertex vertex1 = addVertex(newPos1, face.v1.getColor(), c3dbool_Vertex::eVertex_BOUNDARY);	
		c3dbool_Vertex vertex2 = addVertex(newPos2, face.v1.getColor(), c3dbool_Vertex::eVertex_BOUNDARY);
						
		if (splitEdge == 1)
		{
			addFace(face.v1, vertex1, face.v3);
			addFace(vertex1, vertex2, face.v3);
			addFace(vertex2, face.v2, face.v3);
		}
		else if (splitEdge == 2)
		{
			addFace(face.v2, vertex1, face.v1);
			addFace(vertex1, vertex2, face.v1);
			addFace(vertex2, face.v3, face.v1);
		}
		else
		{
			addFace(face.v3, vertex1, face.v2);
			addFace(vertex1, vertex2, face.v2);
			addFace(vertex2, face.v1, face.v2);
		}
	}
		
	/**
	* Face breaker for VERTEX-FACE-FACE / FACE-FACE-VERTEX
	* 
	* @param facePos face position on the faces array
	* @param newPos new vertex position
	* @param endVertex vertex used for the split
	*/
	void Object3D::breakFaceInThree(int facePos, Point3d newPos, c3dbool_Vertex endVertex)
	{
		C3D_FNLN;
		Face face = (Face)faces[facePos];
		//faces.remove(facePos);
		faces.erase(faces.begin() + facePos);
		
		c3dbool_Vertex vertex = addVertex(newPos, face.v1.getColor(), c3dbool_Vertex::eVertex_BOUNDARY);
						
		if (endVertex.equals(face.v1))
		{
			addFace(face.v1, face.v2, vertex);
			addFace(face.v2, face.v3, vertex);
			addFace(face.v3, face.v1, vertex);
		}
		else if (endVertex.equals(face.v2))
		{
			addFace(face.v2, face.v3, vertex);
			addFace(face.v3, face.v1, vertex);
			addFace(face.v1, face.v2, vertex);
		}
		else
		{
			addFace(face.v3, face.v1, vertex);
			addFace(face.v1, face.v2, vertex);
			addFace(face.v2, face.v3, vertex);
		}
	}
	
	/**
	* Face breaker for EDGE-FACE-EDGE
	* 
	* @param facePos face position on the faces array
	* @param newPos1 new vertex position
	* @param newPos2 new vertex position 
	* @param startVertex vertex used the new faces creation
	* @param endVertex vertex used for the new faces creation
	*/
	void Object3D::breakFaceInThree(int facePos, Point3d newPos1, Point3d newPos2, c3dbool_Vertex startVertex, c3dbool_Vertex endVertex)
	{
		C3D_FNLN;
		Face face = (Face)faces[facePos];
		//faces.remove(facePos);
		faces.erase(faces.begin() + facePos);
		
		c3dbool_Vertex vertex1 = addVertex(newPos1, face.v1.getColor(), c3dbool_Vertex::eVertex_BOUNDARY);
		c3dbool_Vertex vertex2 = addVertex(newPos2, face.v1.getColor(), c3dbool_Vertex::eVertex_BOUNDARY);
						
		if (startVertex.equals(face.v1) && endVertex.equals(face.v2))
		{
			addFace(face.v1, vertex1, vertex2);
			addFace(face.v1, vertex2, face.v3);
			addFace(vertex1, face.v2, vertex2);
		}
		else if (startVertex.equals(face.v2) && endVertex.equals(face.v1))
		{
			addFace(face.v1, vertex2, vertex1);
			addFace(face.v1, vertex1, face.v3);
			addFace(vertex2, face.v2, vertex1);
		}
		else if (startVertex.equals(face.v2) && endVertex.equals(face.v3))
		{
			addFace(face.v2, vertex1, vertex2);
			addFace(face.v2, vertex2, face.v1);
			addFace(vertex1, face.v3, vertex2);
		}
		else if (startVertex.equals(face.v3) && endVertex.equals(face.v2))
		{
			addFace(face.v2, vertex2, vertex1);
			addFace(face.v2, vertex1, face.v1);
			addFace(vertex2, face.v3, vertex1);
		}
		else if (startVertex.equals(face.v3) && endVertex.equals(face.v1))
		{
			addFace(face.v3, vertex1, vertex2);
			addFace(face.v3, vertex2, face.v2);
			addFace(vertex1, face.v1, vertex2);
		}
		else
		{
			addFace(face.v3, vertex2, vertex1);
			addFace(face.v3, vertex1, face.v2);
			addFace(vertex2, face.v1, vertex1);
		}
	}
		
	/**
	* Face breaker for FACE-FACE-FACE (a point only)
	* 
	* @param facePos face position on the faces array
	* @param newPos new vertex position
	*/
	void Object3D::breakFaceInThree(int facePos, Point3d newPos)
	{
		C3D_FNLN;
		Face face = (Face)faces[facePos];
		//faces.remove(facePos);
		faces.erase(faces.begin() + facePos);
		
		c3dbool_Vertex vertex = addVertex(newPos, face.v1.getColor(), c3dbool_Vertex::eVertex_BOUNDARY);
				
		addFace(face.v1, face.v2, vertex);
		addFace(face.v2, face.v3, vertex);
		addFace(face.v3, face.v1, vertex);
	}
	
	/**
	* Face breaker for EDGE-FACE-FACE / FACE-FACE-EDGE
	* 
	* @param facePos face position on the faces array
	* @param newPos1 new vertex position
	* @param newPos2 new vertex position 
	* @param endVertex vertex used for the split
	*/	
	void Object3D::breakFaceInFour(int facePos, Point3d newPos1, Point3d newPos2, c3dbool_Vertex endVertex)
	{
		C3D_FNLN;
		Face face = (Face)faces[facePos];
		//faces.remove(facePos);
		faces.erase(faces.begin() + facePos);
		
		c3dbool_Vertex vertex1 = addVertex(newPos1, face.v1.getColor(), c3dbool_Vertex::eVertex_BOUNDARY);
		c3dbool_Vertex vertex2 = addVertex(newPos2, face.v1.getColor(), c3dbool_Vertex::eVertex_BOUNDARY);
		
		if (endVertex.equals(face.v1))
		{
			addFace(face.v1, vertex1, vertex2);
			addFace(vertex1, face.v2, vertex2);
			addFace(face.v2, face.v3, vertex2);
			addFace(face.v3, face.v1, vertex2);
		}
		else if (endVertex.equals(face.v2))
		{
			addFace(face.v2, vertex1, vertex2);
			addFace(vertex1, face.v3, vertex2);
			addFace(face.v3, face.v1, vertex2);
			addFace(face.v1, face.v2, vertex2);
		}
		else
		{
			addFace(face.v3, vertex1, vertex2);
			addFace(vertex1, face.v1, vertex2);
			addFace(face.v1, face.v2, vertex2);
			addFace(face.v2, face.v3, vertex2);
		}
	}
	
	/**
	* Face breaker for FACE-FACE-FACE
	* 
	* @param facePos face position on the faces array
	* @param newPos1 new vertex position
	* @param newPos2 new vertex position 
	* @param linedVertex what vertex is more lined with the interersection found
	*/		
	void Object3D::breakFaceInFive(int facePos, Point3d newPos1, Point3d newPos2, int linedVertex)
	{
		C3D_FNLN;
		Face face = (Face)faces[facePos];
		//faces.remove(facePos);
		faces.erase(faces.begin() + facePos);
		
		c3dbool_Vertex vertex1 = addVertex(newPos1, face.v1.getColor(), c3dbool_Vertex::eVertex_BOUNDARY);
		c3dbool_Vertex vertex2 = addVertex(newPos2, face.v1.getColor(), c3dbool_Vertex::eVertex_BOUNDARY);
		
		__DOUBLE__ cont = 0.0f;		
		if (linedVertex == 1)
		{
			addFace(face.v2, face.v3, vertex1);
			addFace(face.v2, vertex1, vertex2);
			addFace(face.v3, vertex2, vertex1);
			addFace(face.v2, vertex2, face.v1);
			addFace(face.v3, face.v1, vertex2);
		}
		else if(linedVertex == 2)
		{
			addFace(face.v3, face.v1, vertex1);
			addFace(face.v3, vertex1, vertex2);
			addFace(face.v1, vertex2, vertex1);
			addFace(face.v3, vertex2, face.v2);
			addFace(face.v1, face.v2, vertex2);
		}
		else
		{
			addFace(face.v1, face.v2, vertex1);
			addFace(face.v1, vertex1, vertex2);
			addFace(face.v2, vertex2, vertex1);
			addFace(face.v1, vertex2, face.v3);
			addFace(face.v2, face.v3, vertex2);
		}
	}
	
	//-----------------------------------OTHERS-------------------------------------//
	
	/**
	* Classify faces as being inside, outside or on boundary of other object
	* 
	* @param object object 3d used for the comparison
	*/
	void Object3D::classifyFaces(Object3D object)
	{
		C3D_FNLN;

		//calculate adjacency information
		Face face;

		for(int i=0;i<getNumFaces();i++)
		{
			face = getFace(i);
			face.v1.addAdjacentVertex(face.v2);
			face.v1.addAdjacentVertex(face.v3);
			face.v2.addAdjacentVertex(face.v1);
			face.v2.addAdjacentVertex(face.v3);
			face.v3.addAdjacentVertex(face.v1);
			face.v3.addAdjacentVertex(face.v2);
		}

		
		//for each face
		for(int i=0;i<getNumFaces();i++)
		{
////LOG_PRINT("i=%d\n", i);
			face = getFace(i);

			
			//if the face vertices aren't classified to make the simple classify
			if(face.simpleClassify()==false)
			{

				//makes the ray trace classification
				face.rayTraceClassify(&object);

				
				//mark the vertices
				if(face.v1.getStatus()==c3dbool_Vertex::eVertex_UNKNOWN) 
				{
					face.v1.mark(face.getStatus());
				}

				if(face.v2.getStatus()==c3dbool_Vertex::eVertex_UNKNOWN) 
				{
					face.v2.mark(face.getStatus());
				}

				if(face.v3.getStatus()==c3dbool_Vertex::eVertex_UNKNOWN) 
				{
					face.v3.mark(face.getStatus());
				}

			}
		}

	}
	/** Inverts faces classified as INSIDE, making its normals point outside. Usually
	*  used into the second solid when the difference is applied. */
	void Object3D::invertInsideFaces()
	{
		C3D_FNLN;
		Face face;
		for(int i=0;i<getNumFaces();i++)
		{
			face = getFace(i);
			if(face.getStatus()==Face::eFace_INSIDE)
			{
				face.invert();
			}
		}
	}
	Color3f::Color3f()
	{
		C3D_FNLN;
		x=0.0f;
		y=0.0f;
		z=0.0f;
	}
	Color3f::Color3f(__DOUBLE__ ax, __DOUBLE__ ay, __DOUBLE__ az)
	{
		C3D_FNLN;
		x=ax;
		y=ay;
		z=az;
	}
	
	const std::string Color3f::getString() const 
	{
		C3D_FNLN;
		return "Color3f";
	}
	
	bool Color3f::equals(const Color3f& anObject)
	{
		C3D_FNLN;
		if(anObject.getString()!="Color3f")
		{
			return false;
		}
		return (fabs(x-anObject.x)<TOL && fabs(y-anObject.y)<TOL && fabs(z-anObject.z)<TOL); 
	}
	
bool contains(const std::vector<c3dbool_Vertex>& av, const c3dbool_Vertex& a)
{
		C3D_FNLN;
	for(unsigned int i=0;i<av.size();i++)
		if(fabs(a.x-av[i].x)<TOL && fabs(a.y-av[i].y)<TOL && fabs(a.z-av[i].z)<TOL)
			return true; 
	return false;
}
	
bool contains(const std::vector<D3DXFROMWINEVECTOR3>& av, const D3DXFROMWINEVECTOR3& a)
{
		C3D_FNLN;
	for(unsigned int i=0;i<av.size();i++)
		if(fabs(a.x-av[i].x)<TOL && fabs(a.y-av[i].y)<TOL && fabs(a.z-av[i].z)<TOL)
			return true; 
	return false;
}

bool contains(const std::vector<Point3d> av, const c3dbool_Vertex& a)
{
		C3D_FNLN;
	for(unsigned int i=0;i<av.size();i++)
		if(fabs(a.x-av[i].x)<TOL && fabs(a.y-av[i].y)<TOL && fabs(a.z-av[i].z)<TOL)
			return true; 
	return false;
}


	void c3dbool_Vertex::addAdjacentVertex(c3dbool_Vertex adjacentVertex)
	{
		C3D_FNLN;
		if(!contains(adjacentVertices, adjacentVertex))
		{
			adjacentVertices.push_back(adjacentVertex);
		} 
	}
	Bound Face::getBound()
	{
		C3D_FNLN;
		return Bound(v1.getPosition(),v2.getPosition(),v3.getPosition());
	}
	void Face::rayTraceClassify(Object3D* aobject)
	{
		C3D_FNLN2;

		Object3D object=*aobject;
		//creating a ray starting starting at the face baricenter going to the normal direction
		Point3d p0;
		p0.x = (v1.x + v2.x + v3.x)/3.0f;
		p0.y = (v1.y + v2.y + v3.y)/3.0f;
		p0.z = (v1.z + v2.z + v3.z)/3.0f;

		Line ray = Line(getNormal(),p0);

		
		bool success;
		__DOUBLE__ dotProduct, distance; 
		Point3d* intersectionPoint=0;
		Face* closestFace=0;
		__DOUBLE__ closestDistance; 
				
					
		do
		{
			C3D_FNLN2;

			success = true;
			closestDistance = FLT_MAX;
			//for each face from the other solid...
			for(int i=0;i<object.getNumFaces();i++)
			{
			C3D_FNLN2;
			//LOG_PRINT("Face:%d/%d\n", i, object.getNumFaces());
/* ???
normal.x=0.000000 Vector3d normal=face.getNormal(); Object3D.cpp:1030
normal.y=0.000000
normal.z=0.000000
distance=0.000000 __DOUBLE__ distance = otherPoint.getDistance(point); Object3D.cpp:1049, Line.h:199
*/
				Face face = object.getFace(i);

				Vector3d direction_=ray.getDirection();
				Vector3d normal=face.getNormal();
		#if 1
		//LOG_PRINT("normal.x=%f\n", normal.x);
		//LOG_PRINT("normal.y=%f\n", normal.y);
		//LOG_PRINT("normal.z=%f\n", normal.z);
		#endif

				//dotProduct = face.getNormal().dot(ray.getDirection());
				dotProduct = fabs(D3DXFROMWINEVec3Dot(&normal, &direction_));
				ray.computePlaneIntersection(intersectionPoint, face.getNormal(), face.v1.getPosition());

								
				//if ray intersects the plane...  
				if(intersectionPoint!=0)
				{
			C3D_FNLN2;
/*
../../../Libraries/c3dbool/src/Object3D.cpp:rayTraceClassify:1021
Face:0/12
normal.x=0.000000
normal.y=0.000000
normal.z=0.000000
../../../Libraries/c3dbool/src/Object3D.cpp:rayTraceClassify:1056
fabs(distance)=0.000000
../../../Libraries/c3dbool/src/Object3D.cpp:rayTraceClassify:1021
Face:0/12
normal.x=0.000000
normal.y=0.000000
normal.z=0.000000
../../../Libraries/c3dbool/src/Object3D.cpp:rayTraceClassify:1056
fabs(distance)=0.000000
../../../Libraries/c3dbool/src/Object3D.cpp:rayTraceClassify:1021
*/
					distance = ray.computePointToPointDistance(*intersectionPoint);
						//stx_exit(0);
					if(	(fabs(distance)<0.000001f)||
						(normal.x<0.000001f)||
						(normal.y<0.000001f)||
						(normal.z<0.000001f))
					{		
						//LOG_FNLN;
						#if 1
						//LOG_PRINT("fabs(distance)=%f\n", fabs(distance));
						#endif
					}
					//if ray lies in plane...
					if(fabs(distance)<TOL && fabs(dotProduct)<TOL)
					{
			C3D_FNLN2;
			////LOG_PRINT("TOL=%f\n", TOL);
			////LOG_PRINT("fabs(distance)=%f\n", fabs(distance));
			////LOG_PRINT("fabs(dotProduct)=%f\n", fabs(dotProduct));
						//disturb the ray in order to not lie into another plane 
						ray.perturbDirection();
						success = false;
						break;
					}
			C3D_FNLN2;
					//if ray starts in plane...
					if(fabs(distance)<TOL && fabs(dotProduct)>TOL)
					{
			C3D_FNLN2;

						//if ray intersects the face...
						if(face.hasPoint(*intersectionPoint))
						{
			C3D_FNLN2;

							//faces coincide
							closestFace = &face;
							closestDistance = 0.0f;
							break;
						}
			C3D_FNLN2;
					}
			
					//if ray intersects plane... 
					else if(fabs(dotProduct)>TOL && distance>TOL)
					{
			C3D_FNLN2;

						if(distance<closestDistance)
						{
			C3D_FNLN2;

							//if ray intersects the face;
							if(face.hasPoint(*intersectionPoint))
							{
			C3D_FNLN2;

								//this face is the closest face untill now
								closestDistance = distance;
								closestFace = &face;
			////LOG_FNLN;
							}
			C3D_FNLN2;
						}
			C3D_FNLN2;
					}
			C3D_FNLN2;
				}
			C3D_FNLN2;
			}
			C3D_FNLN2;
		}while(success==false);
			C3D_FNLN2;

		
		//none face found: outside face
		if(closestFace==0)
		{
			C3D_FNLN2;
			status = eFace_OUTSIDE;
			////LOG_FNLN;
		}
		//face found: test dot product
		else
		{
			C3D_FNLN2;
			//dotProduct = closestFace->getNormal().dot(ray.getDirection());
			{
			Vector3d direction_=ray.getDirection();
			Vector3d normal=closestFace->getNormal();
			dotProduct = fabs(D3DXFROMWINEVec3Dot(&normal, &direction_));
			}
			C3D_FNLN2;		
			
			//distance = 0: coplanar faces
			if(fabs(closestDistance)<TOL)
			{
			C3D_FNLN2;
				if(dotProduct>TOL)
				{
			C3D_FNLN2;
					status = eFace_SAME;
				}
				else if(dotProduct<-TOL)
				{
			C3D_FNLN2;
					status = eFace_OPPOSITE;
				}
			C3D_FNLN2;
			}
			
			
			//dot product > 0 (same direction): inside face
			else if(dotProduct>TOL)
			{
			C3D_FNLN2;
				status = eFace_INSIDE;
			}
			
			//dot product < 0 (opposite direction): outside face
			else if(dotProduct<-TOL)
			{
			C3D_FNLN2;
				status = eFace_OUTSIDE;
			}
			C3D_FNLN2;
		}
			C3D_FNLN2;
	}
	Solid::Solid(MeshRenderer2& shape3D) : Object()
	{
		C3D_FNLN;
			////LOG_FNLN;
		for(unsigned int i=0;i<shape3D.vertices.size();i++)
			vertices.push_back(Point3d(shape3D.vertices[i].Position.x, shape3D.vertices[i].Position.y, shape3D.vertices[i].Position.z));
			////LOG_FNLN;
		for(unsigned int i=0;i<shape3D.indices.size();i++)
			indices.push_back(shape3D.indices[i]);
			////LOG_FNLN;
	}
	void Solid::GetMesh(MeshRenderer2& shape3D)
	{
		C3D_FNLN;
			////LOG_FNLN;
		shape3D.vertices.clear();
		shape3D.indices.clear();
			////LOG_FNLN;
		for(unsigned int i=0;i<vertices.size();i++)
		{
			stx_VertexPositionNormalTexture vv;
			vv.Position=D3DXFROMWINEVECTOR3(vertices[i].x, vertices[i].y, vertices[i].z);
			D3DXFROMWINEVec3Normalize(&vv.Normal, &vv.Position);
			vv.Tex=stx_CalculatePositionalSphericalMapping(vv);
			shape3D.vertices.push_back(vv);
		}
			////LOG_FNLN;
		for(unsigned int i=0;i<indices.size();i++)
			shape3D.indices.push_back(indices[i]);
			////LOG_FNLN;
	}

	Object3D::Object3D(const Solid& aSolid)
	{
		C3D_FNLN;
			////LOG_FNLN;
		for(unsigned int i=0;i<aSolid.vertices.size();i++)
			vertices.push_back(	c3dbool_Vertex(	aSolid.vertices[i].x,
							aSolid.vertices[i].y,
							aSolid.vertices[i].z, Color3f(0, 0, 0)));
			////LOG_FNLN;
	
		for(unsigned int i=0;i<aSolid.indices.size()/3;i++)
			faces.push_back(Face(
c3dbool_Vertex( aSolid.vertices[aSolid.indices[3*i+0]].x,
	aSolid.vertices[aSolid.indices[3*i+0]].y,
	aSolid.vertices[aSolid.indices[3*i+0]].z, Color3f(0, 0, 0)),
							c3dbool_Vertex( aSolid.vertices[aSolid.indices[3*i+1]].x,
	aSolid.vertices[aSolid.indices[3*i+1]].y,
	aSolid.vertices[aSolid.indices[3*i+1]].z, Color3f(0, 0, 0)),
							c3dbool_Vertex( aSolid.vertices[aSolid.indices[3*i+2]].x,
	aSolid.vertices[aSolid.indices[3*i+2]].y,
	aSolid.vertices[aSolid.indices[3*i+2]].z, Color3f(0, 0, 0)) ));
			////LOG_FNLN;
	}

