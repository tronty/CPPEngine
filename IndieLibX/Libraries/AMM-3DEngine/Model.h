#pragma once
#include <Framework3/IRenderer.h>
/*
class CollisionData;
CollisionData* m_modelCollisionData;
*/
class Model
{
	protected:

struct Face {
	D3DXFROMWINEVECTOR3 point1;
	D3DXFROMWINEVECTOR3 point2;
	D3DXFROMWINEVECTOR3 point3;
	D3DXFROMWINEVECTOR3 edge1;
	D3DXFROMWINEVECTOR3 edge2;
	D3DXFROMWINEVECTOR3 edge3;
	D3DXFROMWINEVECTOR3 surfaceNormal;
	Face(const Face& rhs)
	{
		point1=rhs.point1;
		point2=rhs.point2;
		point3=rhs.point3;
		edge1=rhs.edge1;
		edge2=rhs.edge2;
		edge3=rhs.edge3;
		surfaceNormal=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
	}
	Face(D3DXFROMWINEVECTOR3 apoint1, D3DXFROMWINEVECTOR3 apoint2, D3DXFROMWINEVECTOR3 apoint3)
	{
		point1=apoint1;
		point2=apoint2;
		point3=apoint3;
		edge1=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
		edge2=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
		edge3=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
		surfaceNormal=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
	}
	Face()
	{
		point1=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
		point2=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
		point3=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
		edge1=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
		edge2=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
		edge3=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
		surfaceNormal=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
	}
};
	public:
		std::vector<WORD> m_indices;
		std::vector<VERTEX> m_vertices;
		std::vector<Face> m_faces;
		VERTEX* GetVertices();
		WORD* GetIndices();
		D3DXFROMWINEVECTOR3 m_scales;
		D3DXFROMWINEVECTOR3 m_translations;
		D3DXFROMWINEMATRIX m_rotations;
		D3DXFROMWINEMATRIX m_transformations;
		D3DXFROMWINEVECTOR3 defaultColor;
		std::string m_name;
		bool m_needsCompilation;
		bool m_bBinded;
		D3DXFROMWINEVECTOR3 m_color;
		static ShaderID m_shd;
		static VertexFormatID m_vf;
		D3DXFROMWINEMATRIX world,view,projection;
		//CollisionData* m_collisionData;
	public:
		void toString();
		void AddVertexPosition(D3DXFROMWINEVECTOR3 pos, D3DXFROMWINEVECTOR3 col=D3DXFROMWINEVECTOR3(1.0f, 0.0f, 0.0f));
		void AddTri(D3DXFROMWINEVECTOR3 point1, D3DXFROMWINEVECTOR3 point2, D3DXFROMWINEVECTOR3 point3);
		void AddTri(WORD d1, WORD d2, WORD d3);
		Model(const Model& rhs);
		Model(const char* aname=0);
		void AddTri(D3DXFROMWINEVECTOR3 point1, D3DXFROMWINEVECTOR3 point2, D3DXFROMWINEVECTOR3 point3, D3DXFROMWINEVECTOR3 color1, D3DXFROMWINEVECTOR3 color2, D3DXFROMWINEVECTOR3 color3);
		void Render(D3DXFROMWINEMATRIX& toWorld, D3DXFROMWINEMATRIX& view, D3DXFROMWINEMATRIX& persp);
};



///<summary>
///This is the core collision data for every object.
///Every object has Axis-aligned bounding boxes.
///</summary>
class CollisionData
{
private:
	Model* m_object;
	static Model* s_boundingBox;

	static bool Overlaps(D3DXFROMWINEVECTOR3 min1, D3DXFROMWINEVECTOR3 max1, D3DXFROMWINEVECTOR3 min2, D3DXFROMWINEVECTOR3 max2);

	void GenerateBoundingBox(std::vector<D3DXFROMWINEVECTOR3> vertices, D3DXFROMWINEVECTOR3& min, D3DXFROMWINEVECTOR3& max, D3DXFROMWINEVECTOR3& size);

public:
	CollisionData();
	~CollisionData();
	D3DXFROMWINEVECTOR3 m_max, m_min, m_size, m_center;
	D3DXFROMWINEVECTOR3 m_reMax, m_reMin, m_reSize;
	std::vector<D3DXFROMWINEVECTOR3> m_boundingPoints;
	std::vector<D3DXFROMWINEVECTOR3> m_reBoundingPoints;

	enum CollisionDetectionType
	{
		BoundingBox = 0,
		AxisRealignedBoundingBox = 1,
	};

	void BindToObject(Model* object);

	///<summary>
	///The collision mask for an object.  Initial set to 1 so that every base collision object 
	///will collide with each other. The bitmask will bit-wise & two masks together and if the 
	///result is greater than 0 then collision will be checked. If a mask is set to 0 it will 
	///not check for any collisions.
	///</summary>
	int m_collisionMask = 0x1;

	///<summary>
	///Generate bounding boxes from the currently model assigned to the collision
	///Data.
	///</summary>
	void UpdateBoundingBoxes();

	static std::vector<D3DXFROMWINEVECTOR3> GenerateBoundingPoints(D3DXFROMWINEVECTOR3 min, D3DXFROMWINEVECTOR3 max);

	void SetModel(Model* mesh);

	void DrawBoundingBox();

	static bool AreColliding(CollisionDetectionType type, Model* first, Model* second);

	static std::vector<D3DXFROMWINEVECTOR3> GetEdgeAxes(Model * first, Model * second);

	static bool SeperatingAxisTest(Model* first, Model* second);

	//<summary>
	//This is the speciailized list of edges that need to be cheked for SAT.
	//It is optimized to remove extra edges that will have already been accounted for by 
	//Other edges in the geometry.
	//</sumarry>
	std::vector<D3DXFROMWINEVECTOR3> m_SATEdges;

	//<summary>
	//Similar to the m_SATEEdges this is an optimized collection of face normals to use
	//with SAT.
	//</summary>
	std::vector<D3DXFROMWINEVECTOR3> m_SATNormals;

	//<summary>
	//Similar to the m_SATEEdges this is an optimized collection of face normals to use
	//with SAT.
	//</summary>
	std::vector<D3DXFROMWINEVECTOR3> m_SATRemovalEdges;
};

extern std::vector<Model*> m_objects;


