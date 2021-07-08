#include "MeshGenerator.h"

D3DXFROMWINEVECTOR3 glm_rotate(D3DXFROMWINEQUATERNION& rotationX, D3DXFROMWINEVECTOR3& translationVector)
{
	D3DXFROMWINEMATRIX out;
	D3DXFROMWINEMatrixRotationQuaternion(&out, &rotationX);
	D3DXFROMWINEVECTOR4 out2;
	D3DXFROMWINEVec3Transform(&out2, &translationVector, &out);
	translationVector.x=out2.x;
	translationVector.y=out2.y;
	translationVector.z=out2.z;
	return translationVector;
}

void MeshGenerator::GenerateModel(std::string system, int iterations, std::string modelName, D3DXFROMWINEVECTOR3 startingPoint, float radius, int pointsPerLevel)
{
			//
	m_model = new Model(modelName.c_str());
	//m_model->GetMesh()->m_compileSAT = false;
	system = m_system.ApplyAxioms(system, iterations);
	std::vector< std::pair<D3DXFROMWINEVECTOR3, D3DXFROMWINEVECTOR3>> pointCollections;
	D3DXFROMWINEVECTOR3 radiusVector = D3DXFROMWINEVECTOR3(radius, 0, 0);
	std::vector<std::pair<D3DXFROMWINEVECTOR3, std::pair<D3DXFROMWINEVECTOR3, D3DXFROMWINEVECTOR3>>> pointStack;
	std::vector<int> closeOffIndeces;
	D3DXFROMWINEVECTOR3 translationVector = D3DXFROMWINEVECTOR3(0, translationOffset, 0);
	D3DXFROMWINEQUATERNION rotationX = D3DXFROMWINEQUATERNION(D3DXFROMWINEVECTOR3(rotationOffset, 0, 0));
	D3DXFROMWINEQUATERNION negRotationX = D3DXFROMWINEQUATERNION(D3DXFROMWINEVECTOR3(-rotationOffset, 0, 0));
	D3DXFROMWINEQUATERNION rotationY = D3DXFROMWINEQUATERNION(D3DXFROMWINEVECTOR3(0, rotationOffset, 0));
	D3DXFROMWINEQUATERNION negRotationY = D3DXFROMWINEQUATERNION(D3DXFROMWINEVECTOR3(0, -rotationOffset, 0));
	D3DXFROMWINEQUATERNION rotationZ = D3DXFROMWINEQUATERNION(D3DXFROMWINEVECTOR3(0, 0, rotationOffset));
	D3DXFROMWINEQUATERNION negRotationZ = D3DXFROMWINEQUATERNION(D3DXFROMWINEVECTOR3(0, 0, -rotationOffset));
	for (int i = 0; i < system.size(); i++)
	{
		switch (system[i])
		{
		case 't': //translate
			startingPoint -= translationVector;
			break;
		case 'T':
			startingPoint += translationVector;
			break;
		case 's': //scale smaller
			radiusVector /= scaleOffset;
			break;
		case 'S': //scale larger
			radiusVector *= scaleOffset;
			break;
		case 'x': 
			translationVector = glm_rotate(rotationX, translationVector);
			break;
		case 'X': 
			translationVector = glm_rotate(negRotationX, translationVector);
			break;
		case 'y':
			translationVector = glm_rotate(rotationY, translationVector);
			break;
		case 'Y':
			translationVector = glm_rotate(negRotationY, translationVector);
			break;
		case 'z':
			translationVector = glm_rotate(rotationZ, translationVector);
			break;
		case 'Z':
			translationVector = glm_rotate(negRotationZ, translationVector);
			break;
		case '[': //push point onto the stack
			if (pointCollections.size() > 0)
			{
				std::pair<D3DXFROMWINEVECTOR3, D3DXFROMWINEVECTOR3> pointCollectionBack = pointCollections.back();
				pointStack.push_back(std::make_pair(pointCollectionBack.first, std::make_pair(pointCollectionBack.second, translationVector)));
			}
			else
				pointStack.push_back(std::make_pair(startingPoint, std::make_pair(radiusVector, translationVector)));
			break;
		case ']': { //pop point from the stack
			std::pair<D3DXFROMWINEVECTOR3, std::pair<D3DXFROMWINEVECTOR3, D3DXFROMWINEVECTOR3>> popped = pointStack.back();
			pointStack.pop_back();
			startingPoint = popped.first;
			radiusVector = popped.second.first;
			translationVector = popped.second.second;
		}
			break;
		case 'c':
		case 'C':
			closeOffIndeces.push_back(pointCollections.size() - 1);
			break;
		case '+': //pushes the current point to be used on the geometry
			pointCollections.push_back(std::make_pair(startingPoint, radiusVector));
			break;
		case 'F':
			startingPoint += translationVector;
			pointCollections.push_back(std::make_pair(startingPoint, radiusVector));
			break;
		default:
			break;
		}
	}
	//Create the vertices for the mesh
	std::vector<std::vector<D3DXFROMWINEVECTOR3>> meshGeometry;
	for (int i = 0; i < pointCollections.size(); i++)
	{
		std::vector<D3DXFROMWINEVECTOR3> pointsForIteration;
		for (int n = 0; n < pointsPerLevel; n++)
		{
			D3DXFROMWINEVECTOR3 vertex;
			vertex = pointCollections[i].first;
			D3DXFROMWINEVECTOR3 radVec = pointCollections[i].second;
			D3DXFROMWINEVECTOR3 rotAxis = vertex;
			if (i - 1 > 0)
				rotAxis = rotAxis - pointCollections[i - 1].first;
			if (D3DXFROMWINEVec3Length(&vertex) > 0)
				D3DXFROMWINEVec3Normalize(&rotAxis,&rotAxis);
			else if (i + 1 < pointCollections.size())
				rotAxis = pointCollections[i + 1].first - rotAxis;
			else //Assume 0,1,0
				rotAxis = D3DXFROMWINEVECTOR3(0, 1, 0);
			float theta = 2 * D3DXFROMWINE_PI / pointsPerLevel * n;
			D3DXFROMWINEVECTOR3 crossVec;
			D3DXFROMWINEVec3Cross(&crossVec, &rotAxis, &radVec);
			radVec = radVec * cos(theta) + (crossVec) * sin(theta) + rotAxis * (D3DXFROMWINEVec3Dot(&rotAxis, &radVec))* (1 - cos(theta));
			pointsForIteration.push_back(vertex + radVec);
		}
		meshGeometry.push_back(pointsForIteration);
	}
	//Create the face geometry for the mesh
	for (int i = 0; i < meshGeometry.size(); i++)
	{
		int currentSize = meshGeometry[i].size();
		int nextRowSize = meshGeometry[(i + 1) % meshGeometry.size()].size();
		bool closedOff = false;
		for (int k = 0; k < currentSize; k++)
		{
			
			//Draw to the next layer of points, unless this is the first or last layer.
			//In those situations the shape needs to close, so connect those to the center points.
			if (i == 0)
			{
			
				m_model->AddTri(meshGeometry[i][k], meshGeometry[i][(k + 1) % currentSize], pointCollections[i].first);
			}
			if (i == meshGeometry.size() - 1)
			{
			
				m_model->AddTri(meshGeometry[i][k], meshGeometry[i][(k + 1) % currentSize], pointCollections[i].first);
				continue; //There are no more points to draw up to
			}
			//Don't connect points to the next layer of the index is a close off point
			if (closedOff || (closeOffIndeces.size() > 0 && i == closeOffIndeces.front()))
			{
			
				m_model->AddTri(meshGeometry[i][k], meshGeometry[i][(k + 1) % currentSize], pointCollections[i].first);
				closedOff = true;
				continue;
			}
			//Need to draw the face
			// __
			//|\ |
			//|_\|
			//Two triangles per face
			//Draw the first triangle of the face
			
			m_model->AddTri(meshGeometry[i][k], meshGeometry[i][(k + 1) % currentSize], meshGeometry[i + 1][k]);
			m_model->AddTri(meshGeometry[i + 1][k], meshGeometry[i + 1][(k + 1) % currentSize], meshGeometry[i][(k + 1) % currentSize]);
		}
		if(closedOff)
			closeOffIndeces.erase(closeOffIndeces.begin());
	}
	//Call compile shape to do this when this function is called instead of when the object needs to be rendered
	// ??? m_model->compileShape();
	LOG_PRINT("m_lVertex: %d\n", m_model->m_vertices.size());
	LOG_PRINT("m_indices: %d\n", m_model->m_indices.size());
	LOG_PRINT("m_faces: %d\n", m_model->m_faces.size());
}
