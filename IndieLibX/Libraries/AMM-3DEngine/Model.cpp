#include "Model.h"
#define DEBUG false

ShaderID Model::m_shd=-1;
VertexFormatID Model::m_vf=-1;

		Model::VERTEX* Model::GetVertices(){return (Model::VERTEX*)&m_vertices[0];}
		DWORD* Model::GetIndices(){return (DWORD*)&m_indices[0];}
		void Model::toString()
		{
#if 1
		LOG_PRINT("m_shd: %x\n", m_shd);
		LOG_PRINT("m_vf: %x\n", m_vf);
		LOG_PRINT("m_vertices(): %x\n", m_vertices.size());
		LOG_PRINT("m_indices.size(): %x\n", m_indices.size());
		LOG_PRINT("&m_indices[0]: %x\n", &m_indices[0]);
		LOG_PRINT("&m_vertices[0]: %x\n", &m_vertices[0]);
#endif
		}
		void Model::AddVertexPosition(D3DXFROMWINEVECTOR3 pos, D3DXFROMWINEVECTOR3 col)
		{
			VERTEX v(pos, col);
			m_vertices.push_back(v);
		}
		void Model::AddTri(D3DXFROMWINEVECTOR3 point1, D3DXFROMWINEVECTOR3 point2, D3DXFROMWINEVECTOR3 point3)
		{
			D3DXFROMWINEVECTOR3 color(1.0f, 0.0f, 0.0f);
			AddTri(point1, point2, point3, color, color, color);
		}/*
		void Model::AddTri(DWORD d1, DWORD d2, DWORD d3)
		{
			m_indices.push_back(d1);
			m_indices.push_back(d2);
			m_indices.push_back(d3);
		}*/
		Model::Model(const Model& rhs)
		{
			m_indices=rhs.m_indices;
			m_vertices=rhs.m_vertices;
			m_faces=rhs.m_faces;
			m_scales=rhs.m_scales;
			m_translations=rhs.m_translations;
			m_rotations=rhs.m_rotations;
			m_transformations=rhs.m_transformations;
			defaultColor=rhs.defaultColor;
			m_name=rhs.m_name;
			m_needsCompilation=rhs.m_needsCompilation;
			m_bBinded=rhs.m_bBinded;
			m_color=rhs.m_color;
			world=rhs.world;
			view=rhs.view;
			projection=rhs.projection;
			//m_collisionData=rhs.m_collisionData;
	 	}
		Model::Model(const char* aname=0)
		{
			m_name=aname;
			m_needsCompilation=false;
			m_bBinded=false;
			if(m_shd==-1)
			{
			m_shd=IRenderer::GetRendererInstance()->addShaderFromFile("/AMM-3DEngine/BasicColor.shd", "main", "main");
			FormatDesc format[] =
			{
				0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
				0, TYPE_TEXCOORD, FORMAT_FLOAT, 3
			};
			m_vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), m_shd);
			}
		}
void Model::Render(D3DXFROMWINEMATRIX& toWorld, D3DXFROMWINEMATRIX& view, D3DXFROMWINEMATRIX& persp)
{/*
	if (m_needsCompilation) {
		compileShape();
	}
	
	if (!m_bBinded)
		return;
	if (m_vertices() == 0)
		return;
*/

	// Use the buffer and shader
	IRenderer::GetRendererInstance()->setShader(m_shd);
	IRenderer::GetRendererInstance()->setVertexFormat(m_vf);

	//Final Projection of the Camera is going to be hard coded
	D3DXFROMWINEMATRIX m4Projection = persp;
	D3DXFROMWINEMATRIX m4View = view;
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("MVP", m4Projection * m4View * toWorld);

	//Color & tint
	float c[3] = { m_color.x, m_color.y, m_color.z };
	IRenderer::GetRendererInstance()->setShaderConstant3f("Color", c);
	IRenderer::GetRendererInstance()->setShaderConstant3f("Tint", c);
	/*glEnableVertexAttribArray(v4Color);
	glBindBuffer(GL_ARRAY_BUFFER, m_ColorBuffer);
	glVertexAttribPointer(v4Color, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);*/

	//Color and draw
	if(m_indices.size())
		IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(
		PRIM_TRIANGLES, 0, m_vertices.size(),
		m_indices.size()/3,
		(DWORD*)&m_indices[0],
		(DWORD*)&m_indices[0],
		4,
		(VERTEX*)&m_vertices[0],
		(VERTEX*)&m_vertices[0],
		sizeof(VERTEX));
	else
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES,
		m_vertices.size()/3,// ???
		(VERTEX*)&m_vertices[0],
		(VERTEX*)&m_vertices[0],
		sizeof(VERTEX));

	#if 1
		LOG_PRINT("m_shd: %x\n", m_shd);
		LOG_PRINT("m_vf: %x\n", m_vf);
		LOG_PRINT("m_vertices.size(): %x\n", m_vertices.size());
		LOG_PRINT("m_indices.size(): %x\n", m_indices.size());
		LOG_PRINT("&m_indices[0]: %x\n", &m_indices[0]);
		LOG_PRINT("&m_vertices[0]: %x\n", &m_vertices[0]);
		LOG_PRINT("sizeof(VERTEX): %x\n", sizeof(VERTEX));
		exit(0);
	#endif
}
#if 0

Model* CollisionData::s_boundingBox = nullptr;

void CollisionData::BindToObject(Model* object)
{
	m_object = object;
}

CollisionData::~CollisionData()
{
	//delete m_boundingBox;
}

void CollisionData::GenerateBoundingBox(std::vector<D3DXFROMWINEVECTOR3> vertices, D3DXFROMWINEVECTOR3& min, D3DXFROMWINEVECTOR3& max, D3DXFROMWINEVECTOR3& size)
{
	if (vertices.size() > 0) {
		max = vertices[0];
		min = vertices[0];
	}

	for (int i = 0; i < vertices.size(); i++)
	{
		if (vertices[i].x > max.x)
			max.x = vertices[i].x;
		else if (vertices[i].x < min.x)
			min.x = vertices[i].x;

		if (vertices[i].y > max.y)
			max.y = vertices[i].y;
		else if (vertices[i].y < min.y)
			min.y = vertices[i].y;

		if (vertices[i].z > max.z)
			max.z = vertices[i].z;
		else if (vertices[i].z < min.z)
			min.z = vertices[i].z;
	}

	D3DXFROMWINEVECTOR3 dt = D3DXFROMWINEVECTOR3(min.x, 0.0f, 0.0f)-D3DXFROMWINEVECTOR3(max.x, 0.0f, 0.0f);
	size.x = D3DXFROMWINEVec3Length(&dt);
	dt = D3DXFROMWINEVECTOR3(0.0f, min.y, 0.0f)-D3DXFROMWINEVECTOR3(0.0f, max.y, 0.0f);
	size.y = D3DXFROMWINEVec3Length(&dt);
	dt = D3DXFROMWINEVECTOR3(0.0f, 0.0f, min.z)-D3DXFROMWINEVECTOR3(0.0f, 0.0f, max.z);
	size.z = D3DXFROMWINEVec3Length(&dt);
}

CollisionData::CollisionData()
{
	m_boundingPoints = std::vector<D3DXFROMWINEVECTOR3>(8);
	m_reBoundingPoints = std::vector<D3DXFROMWINEVECTOR3>(8);
}

void CollisionData::UpdateBoundingBoxes()
{

	/* The axis realigned bounding box */
	for (unsigned int i = 0; i < m_reBoundingPoints.size(); i++)
	{
		D3DXFROMWINEVECTOR4 v1; 
		D3DXFROMWINEVECTOR4 v2=D3DXFROMWINEVECTOR4(m_boundingPoints[i].x, m_boundingPoints[i].y, m_boundingPoints[i].z, 1.0f);
		D3DXFROMWINEVec4Transform(&v1, &v2, &m_object->m_transformations);
		D3DXFROMWINEVECTOR3 v3(v1.x, v1.y, v1.z);
		m_reBoundingPoints[i] = v3;
	}

	GenerateBoundingBox(m_reBoundingPoints, m_reMin, m_reMax, m_reSize);
	m_reBoundingPoints = GenerateBoundingPoints(m_reMin, m_reMax);

	m_center = (m_max + m_min) / 2.0f; // TC: Not sure if needed yet.
}

std::vector<D3DXFROMWINEVECTOR3> CollisionData::GenerateBoundingPoints(D3DXFROMWINEVECTOR3 min, D3DXFROMWINEVECTOR3 max)
{
	std::vector<D3DXFROMWINEVECTOR3> points(8);

	// Top Points
	points[0] = D3DXFROMWINEVECTOR3(min.x, min.y, max.z);
	points[1] = D3DXFROMWINEVECTOR3(max.x, min.y, max.z);
	points[2] = D3DXFROMWINEVECTOR3(max.x, max.y, max.z);
	points[3] = D3DXFROMWINEVECTOR3(min.x, max.y, max.z);

	// Bottom Points
	points[4] = D3DXFROMWINEVECTOR3(min.x, min.y, min.z);
	points[5] = D3DXFROMWINEVECTOR3(max.x, min.y, min.z);
	points[6] = D3DXFROMWINEVECTOR3(max.x, max.y, min.z);
	points[7] = D3DXFROMWINEVECTOR3(min.x, max.y, min.z);

	return points;
}

void CollisionData::SetModel(Model* mesh)
{
/* ???
	GenerateBoundingBox(mesh->GetVertices(), m_min, m_max, m_size);
	m_boundingPoints = GenerateBoundingPoints(m_min, m_max);
	UpdateBoundingBoxes();
*/
}

void CollisionData::DrawBoundingBox()
{
/* ???
	//TODO: This currently only draws the REBB. Possibly add code to make it draw different ones
	//Depending on collision mode, or depending on debug mode.
	if (s_boundingBox == nullptr) 
	{
		s_boundingBox = new Model("Cube");
		s_boundingBox->GenCube(1.0f);
	}
*/
	D3DXFROMWINEMATRIX world;
	D3DXFROMWINEMatrixIdentity(&world);
	D3DXFROMWINEMATRIX out;
	D3DXFROMWINEMatrixTranslation(&out, m_object->m_translations.x, m_object->m_translations.y, m_object->m_translations.z);
	world *= out;
	D3DXFROMWINEMatrixScaling(&out, m_reSize.x, m_reSize.y, m_reSize.z);
	world *= out;

	s_boundingBox->Render(m_object->world, m_object->view, m_object->projection);
}

bool CollisionData::AreColliding(CollisionDetectionType type, Model* first, Model* second)
{
	bool areColliding = false;

	D3DXFROMWINEVECTOR3 min1;
	D3DXFROMWINEVECTOR3 max1;
	D3DXFROMWINEVECTOR3 min2;
	D3DXFROMWINEVECTOR3 max2;

	switch (type)
	{
	case BoundingBox:
		min1 = D3DXFROMWINEVECTOR3(first->m_transformations * D3DXFROMWINEVECTOR4(first->m_collisionData->m_min, 1.0f));
		max1 = D3DXFROMWINEVECTOR3(first->m_transformations * D3DXFROMWINEVECTOR4(first->m_collisionData->m_max, 1.0f));
		min2 = D3DXFROMWINEVECTOR3(second->m_transformations * D3DXFROMWINEVECTOR4(second->m_collisionData->m_min, 1.0f));
		max2 = D3DXFROMWINEVECTOR3(second->m_transformations * D3DXFROMWINEVECTOR4(second->m_collisionData->m_max, 1.0f));
		break;
	case AxisRealignedBoundingBox:
		min1 = first->m_collisionData->m_reMin;
		max1 = first->m_collisionData->m_reMax;
		min2 = second->m_collisionData->m_reMin;
		max2 = second->m_collisionData->m_reMax;
		break;
	default:
		LOG_PRINT("Unknown collision detection type.\n");
		break;
	}

	areColliding = Overlaps(min1, max1, min2, max2);
	//Code removed as functionality has slowed down since refactor.
	/*if (areColliding == true) {
		areColliding = SeperatingAxisTest(first, second);
	}*/

	return areColliding;
}

std::vector<D3DXFROMWINEVECTOR3> CollisionData::GetEdgeAxes(Model* first, Model* second)
{
	std::vector<D3DXFROMWINEVECTOR3> firstEdges = first->m_collisionData->m_SATEdges;
	std::vector<D3DXFROMWINEVECTOR3> secondEdges = second->m_collisionData->m_SATEdges;
	std::vector<D3DXFROMWINEVECTOR3> axes;
	//Loop through the faces of the first object
	for (int i = 0; i < firstEdges.size(); i++)
	{
		//loop through the second object
		for (int k = 0; k < secondEdges.size(); k++)
		{
		D3DXFROMWINEVECTOR3 vin;
		D3DXFROMWINEVECTOR3 v1(first->m_transformations * D3DXFROMWINEVECTOR4(firstEdges[i], 1));
		D3DXFROMWINEVECTOR3 v2(second->m_transformations * D3DXFROMWINEVECTOR4(secondEdges[k], 1));
		D3DXFROMWINEVec3Cross(&vin,&v1,&v2);
		D3DXFROMWINEVECTOR3 vout;
		D3DXFROMWINEVec3Normalize(&vout,&vin);
		axes.push_back(vout);
		}
	}
	return axes;
}

bool CollisionData::SeperatingAxisTest(Model * first, Model * second)
{
	//The first step is to obtain the seperating Axes between the two shapes, this is done through getting
	//Every Edge combiniation between the two of them.  
	std::vector<D3DXFROMWINEVECTOR3> axes = GetEdgeAxes(first, second);

	//Now add each face normal to the Axes so that all seperating axes are accounted for
	std::vector<D3DXFROMWINEVECTOR3> firstFaces = first->m_collisionData->m_SATNormals;
	for (int i = 0; i < firstFaces.size(); i++)
	{
		D3DXFROMWINEVECTOR3 vin(first->m_transformations * D3DXFROMWINEVECTOR4(firstFaces[i], 1.0));
		D3DXFROMWINEVECTOR3 vout;
		D3DXFROMWINEVec3Normalize(&vout,&vin);
		axes.push_back(vout);
	}
	//Add those of the other object as well
	std::vector<D3DXFROMWINEVECTOR3> secondFaces = second->m_collisionData->m_SATNormals;
	for (int i = 0; i < secondFaces.size(); i++)
	{
		D3DXFROMWINEVECTOR3 vin(second->m_transformations * D3DXFROMWINEVECTOR4(secondFaces[i], 1.0));
		D3DXFROMWINEVECTOR3 vout;
		D3DXFROMWINEVec3Normalize(&vout,&vin);
		axes.push_back(vout);
	}
	//Determine the plane points
	D3DXFROMWINEVECTOR3 planePoint = D3DXFROMWINEVECTOR3(0,0,0);

	//Now project each vertice, onto the plane created by the axes to see if there is a collision.
	std::vector<D3DXFROMWINEVECTOR3> firstVertices = first->m_model->GetVertices();
	std::vector<D3DXFROMWINEVECTOR3> secondVertices = second->m_model->GetVertices();
	//Transform the vertices to world coordinates
	for (int i = 0; i < firstVertices.size(); i++)
	{
		firstVertices[i] = D3DXFROMWINEVECTOR3(first->m_transformations * D3DXFROMWINEVECTOR4(firstVertices[i], 1.0f));
	}
	for (int i = 0; i < firstVertices.size(); i++)
	{
		secondVertices[i] = D3DXFROMWINEVECTOR3(second->m_transformations * D3DXFROMWINEVECTOR4(secondVertices[i], 1.0f));
	}
	for (int a = 0; a < axes.size(); a++)
	{
		//Do the calculations in the for loop once so that we have accurate starting points for the min and maxes
		D3DXFROMWINEVECTOR3 firstMin = firstVertices[0] - D3DXFROMWINEVec3Dot(&(firstVertices[0] - planePoint), &axes[a]) * axes[a];
		D3DXFROMWINEVECTOR3 firstMax = firstMin;
		D3DXFROMWINEVECTOR3 secondMin = secondVertices[0] - D3DXFROMWINEVec3Dot(&(secondVertices[0] - planePoint), &axes[a]) * axes[a];
		D3DXFROMWINEVECTOR3 secondMax = secondMin;
		D3DXFROMWINEVECTOR3 minOverlap = firstMin - secondMin;
		for (int i = 1; i < firstVertices.size(); i++)
		{
			//Find the projection onto the plane
			D3DXFROMWINEVECTOR3 projectedPoint = firstVertices[i] - D3DXFROMWINEVec3Dot(&(firstVertices[i] - planePoint), &axes[a]) * axes[a];
			//check to see if there needs to be new min max x points
			projectedPoint.x < firstMin.x ? firstMin.x = projectedPoint.x : (projectedPoint.x > firstMax.x ? firstMax.x = projectedPoint.x : 0);
			projectedPoint.y < firstMin.y ? firstMin.y = projectedPoint.y : (projectedPoint.y > firstMax.y ? firstMax.y = projectedPoint.y : 0);
			projectedPoint.z < firstMin.z ? firstMin.z = projectedPoint.z : (projectedPoint.z > firstMax.z ? firstMax.z = projectedPoint.z : 0);
		}
		for (int i = 1; i < secondVertices.size(); i++)
		{
			D3DXFROMWINEVECTOR3 projectedPoint = secondVertices[i] - 

D3DXFROMWINEVec3Dot(&(secondVertices[i] - planePoint), &axes[a]) * axes[a];
			projectedPoint.x < secondMin.x ? secondMin.x = projectedPoint.x : (projectedPoint.x > secondMax.x ? secondMax.x = projectedPoint.x : 0);
			projectedPoint.y < secondMin.y ? secondMin.y = projectedPoint.y : (projectedPoint.y > secondMax.y ? secondMax.y = projectedPoint.y : 0);
			projectedPoint.z < secondMin.z ? secondMin.z = projectedPoint.z : (projectedPoint.z > secondMax.z ? secondMax.z = projectedPoint.z : 0);
		}

		//Lastly check to see if there is overlap between the min and max for this axis and set minimum overlap.
		if (!Overlaps(firstMin, firstMax, secondMin, secondMax))
		{
			// minimum overlap checks
			if (minOverlap.x > abs(firstMin.x - secondMin.x)) { minOverlap.x = abs(firstMin.x - secondMin.x); } //x
			if (minOverlap.y > abs(firstMin.y - secondMin.y)) { minOverlap.y = abs(firstMin.y - secondMin.y); } //y
			if (minOverlap.z > abs(firstMin.z - secondMin.z)) { minOverlap.z = abs(firstMin.z - secondMin.z); } //z
			return false;
		}
	}

	return true;
}

bool CollisionData::Overlaps(D3DXFROMWINEVECTOR3 min1, D3DXFROMWINEVECTOR3 max1, D3DXFROMWINEVECTOR3 min2, D3DXFROMWINEVECTOR3 max2)
{
	bool areColliding = true;
	
	if (max1.x < min2.x)
		areColliding = false;
	if (min1.x > max2.x)
		areColliding = false;

	if (max1.y < min2.y)
		areColliding = false;
	if (min1.y > max2.y)
		areColliding = false;

	if (max1.z < min2.z)
		areColliding = false;
	if (min1.z > max2.z)
		areColliding = false;

	return areColliding;
}
#endif

void Model::AddTri(D3DXFROMWINEVECTOR3 point1, D3DXFROMWINEVECTOR3 point2, D3DXFROMWINEVECTOR3 point3, D3DXFROMWINEVECTOR3 color1, D3DXFROMWINEVECTOR3 color2, D3DXFROMWINEVECTOR3 color3)
{
	AddVertexPosition(point1);
	AddVertexPosition(point2);
	AddVertexPosition(point3);
/* ???
	m_indices.push_back(m_vertices()-3);
	m_indices.push_back(m_vertices()-2);
	m_indices.push_back(m_vertices()-1);
*/
/*
	AddVertexColor(color1);
	AddVertexColor(color2);
	AddVertexColor(color3);
*/
	Face f = Face(point1, point2, point3);
	m_faces.push_back(f);
#if 0
	//if (!m_compileSAT)
	//	return;
	for (int i = 0; i < m_faces.size(); i++)
	{
		if (f.surfaceNormal == m_faces[i].surfaceNormal)
		{
			Face other = m_faces[i];
			if (f.point1 == other.point1)
			{
				if (f.point2 == other.point2) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point2 - f.point1);
				if (f.point2 == other.point3) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point2 - f.point1);
				if (f.point3 == other.point2) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point3 - f.point1);
				if (f.point3 == other.point3) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point3 - f.point1);
			}
			else if (f.point1 == other.point2)
			{
				if (f.point2 == other.point1) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point2 - f.point1);
				if (f.point2 == other.point3) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point2 - f.point1);
				if (f.point3 == other.point1) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point3 - f.point1);
				if (f.point3 == other.point3) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point3 - f.point1);
			}
			else if (f.point1 == other.point3)
			{
				if (f.point2 == other.point2) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point2 - f.point1);
				if (f.point2 == other.point3) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point2 - f.point1);
				if (f.point3 == other.point2) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point3 - f.point1);
				if (f.point3 == other.point3) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point3 - f.point1);
			}

			if (f.point2 == other.point1)
			{
				if (f.point1 == other.point2) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point1 - f.point2);
				if (f.point1 == other.point3) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point1 - f.point2);
				if (f.point3 == other.point2) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point3 - f.point2);
				if (f.point3 == other.point3) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point3 - f.point2);
			}
			else if (f.point2 == other.point2)
			{
				if (f.point1 == other.point1) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point1 - f.point2);
				if (f.point1 == other.point3) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point1 - f.point2);
				if (f.point3 == other.point1) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point3 - f.point2);
				if (f.point3 == other.point3) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point3 - f.point2);
			}
			else if (f.point2 == other.point3)
			{
				if (f.point1 == other.point2) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point1 - f.point2);
				if (f.point1 == other.point3) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point1 - f.point2);
				if (f.point3 == other.point2) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point3 - f.point2);
				if (f.point3 == other.point3) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point3 - f.point2);
			}

			if (f.point3 == other.point1)
			{
				if (f.point2 == other.point2) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point2 - f.point3);
				if (f.point2 == other.point3) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point2 - f.point3);
				if (f.point1 == other.point2) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point1 - f.point3);
				if (f.point1 == other.point3) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point1 - f.point3);
			}
			else if (f.point3 == other.point2)
			{
				if (f.point2 == other.point1) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point2 - f.point3);
				if (f.point2 == other.point3) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point2 - f.point3);
				if (f.point1 == other.point1) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point1 - f.point3);
				if (f.point1 == other.point3) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point1 - f.point3);
			}
			else if (f.point3 == other.point3)
			{
				if (f.point2 == other.point2) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point2 - f.point3);
				if (f.point2 == other.point3) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point2 - f.point3);
				if (f.point1 == other.point2) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point1 - f.point3);
				if (f.point1 == other.point3) m_modelCollisionData->m_SATRemovalEdges.push_back(f.point1 - f.point3);
			}
		}
	}
	D3DXFROMWINEVECTOR3 e;
	D3DXFROMWINEVec3Normalize(&e, &f.edge1);
	m_modelCollisionData->m_SATEdges.push_back(e);
	D3DXFROMWINEVec3Normalize(&e, &f.edge2);
	m_modelCollisionData->m_SATEdges.push_back(e);
	D3DXFROMWINEVec3Normalize(&e, &f.edge3);
	m_modelCollisionData->m_SATEdges.push_back(e);
	m_modelCollisionData->m_SATNormals.push_back(f.surfaceNormal);
#endif
}
