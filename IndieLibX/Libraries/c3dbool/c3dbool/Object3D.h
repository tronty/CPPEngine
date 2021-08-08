/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef __Object3D_H__
#define __Object3D_H__
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
class Object3D : public Object
{
	/** solid vertices  */
	std::vector<c3dbool_Vertex> vertices;
	/** solid faces */
	std::vector<Face> faces;
	/** object representing the solid extremes */
	Bound bound;

	//----------------------------------CONSTRUCTOR---------------------------------//
	
	/** 
	* Constructs a Object3d object based on a solid file.
	*
	* @param solid solid used to construct the Object3d object  
	*/	
	public:
	Object3D(const Object3D& rhs)
	{
		C3D_FNLN;
		vertices=rhs.vertices;
		faces=rhs.faces;
		bound=rhs.bound;		
	}
	Object3D();
	Object3D(const Solid& aSolid);
	void Initialize(Solid solid);
	
	//-----------------------------------OVERRIDES----------------------------------//
	
	/**
	* Clones the Object3D object
	* 
	* @return cloned Object3D object
	*/
	Object clone();
	
	//--------------------------------------GETS------------------------------------//
	
	/**
	* Gets the number of faces
	* 
	* @return number of faces
	*/
	__WORD__ getNumFaces();
	
	/**
	* Gets a face reference for a given position
	* 
	* @param index required face position
	* @return face reference , 0 if the position is invalid
	*/
	Face getFace(__WORD__ index);
	
	/**
	* Gets the solid bound
	* 
	* @return solid bound
	*/
	Bound getBound();
	
	//------------------------------------ADDS----------------------------------------//
		
	/**
	* Method used to add a face properly for internal methods
	* 
	* @param v1 a face vertex
	* @param v2 a face vertex
	* @param v3 a face vertex
	*/
	Face addFace(c3dbool_Vertex v1, c3dbool_Vertex v2, c3dbool_Vertex v3);
	
	/**
	* Method used to add a vertex properly for internal methods
	* 
	* @param pos vertex position
	* @param color vertex color
	* @param status vertex status
	* @return the vertex inserted (if a similar vertex already exists, this is returned)
	*/
	c3dbool_Vertex addVertex(Point3d pos, Color3f color, int status);
		
	//-------------------------FACES_SPLITTING_METHODS------------------------------//
	
	/**
	* Split faces so that none face is intercepted by a face of other object
	* 
	* @param object the other object 3d used to make the split 
	*/
	void splitFaces(Object3D object);
	
	/**
	* Computes closest distance from a vertex to a plane
	* 
	* @param vertex vertex used to compute the distance
	* @param face face representing the plane where it is contained
	* @return the closest distance from the vertex to the plane
	*/
	__DOUBLE__ computeDistance(c3dbool_Vertex vertex, Face face);
	/**
	* Split an individual face
	* 
	* @param facePos face position on the array of faces
	* @param segment1 segment representing the intersection of the face with the plane
	* of another face
	* @return segment2 segment representing the intersection of other face with the
	* plane of the current face plane
	*/	 
	void splitFace(int facePos, Segment segment1, Segment segment2);
	 
	/**
	* Face breaker for VERTEX-EDGE-EDGE / EDGE-EDGE-VERTEX
	* 
	* @param facePos face position on the faces array
	* @param newPos new vertex position
	* @param edge that will be split 
	*/		
	void breakFaceInTwo(int facePos, Point3d newPos, int splitEdge);
	
	/**
	* Face breaker for VERTEX-FACE-EDGE / EDGE-FACE-VERTEX
	* 
	* @param facePos face position on the faces array
	* @param newPos new vertex position
	* @param endVertex vertex used for splitting 
	*/		
	void breakFaceInTwo(int facePos, Point3d newPos, c3dbool_Vertex endVertex);
	
	/**
	* Face breaker for EDGE-EDGE-EDGE
	* 
	* @param facePos face position on the faces array
	* @param newPos1 new vertex position
	* @param newPos2 new vertex position 
	* @param splitEdge edge that will be split
	*/
	void breakFaceInThree(int facePos, Point3d newPos1, Point3d newPos2, int splitEdge);
		
	/**
	* Face breaker for VERTEX-FACE-FACE / FACE-FACE-VERTEX
	* 
	* @param facePos face position on the faces array
	* @param newPos new vertex position
	* @param endVertex vertex used for the split
	*/
	void breakFaceInThree(int facePos, Point3d newPos, c3dbool_Vertex endVertex);
	
	/**
	* Face breaker for EDGE-FACE-EDGE
	* 
	* @param facePos face position on the faces array
	* @param newPos1 new vertex position
	* @param newPos2 new vertex position 
	* @param startVertex vertex used the new faces creation
	* @param endVertex vertex used for the new faces creation
	*/
	void breakFaceInThree(int facePos, Point3d newPos1, Point3d newPos2, c3dbool_Vertex startVertex, c3dbool_Vertex endVertex);
		
	/**
	* Face breaker for FACE-FACE-FACE (a point only)
	* 
	* @param facePos face position on the faces array
	* @param newPos new vertex position
	*/
	void breakFaceInThree(int facePos, Point3d newPos);
	
	/**
	* Face breaker for EDGE-FACE-FACE / FACE-FACE-EDGE
	* 
	* @param facePos face position on the faces array
	* @param newPos1 new vertex position
	* @param newPos2 new vertex position 
	* @param endVertex vertex used for the split
	*/	
	void breakFaceInFour(int facePos, Point3d newPos1, Point3d newPos2, c3dbool_Vertex endVertex);
	
	/**
	* Face breaker for FACE-FACE-FACE
	* 
	* @param facePos face position on the faces array
	* @param newPos1 new vertex position
	* @param newPos2 new vertex position 
	* @param linedVertex what vertex is more lined with the interersection found
	*/		
	void breakFaceInFive(int facePos, Point3d newPos1, Point3d newPos2, int linedVertex);
	
	//-----------------------------------OTHERS-------------------------------------//
	
	/**
	* Classify faces as being inside, outside or on boundary of other object
	* 
	* @param object object 3d used for the comparison
	*/
	void classifyFaces(Object3D object);
	
	/** Inverts faces classified as INSIDE, making its normals point outside. Usually
	*  used into the second solid when the difference is applied. */
	void invertInsideFaces();
};
#endif

