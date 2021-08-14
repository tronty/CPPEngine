#include "precomp.h"
#include "LSystemMeshCreator.h"
#include "Mesh.h"
#include "utils.h"

/*std::unique_ptr<std::vector<std::unique_ptr<Mesh>>> LSystemMeshCreator::createMesh(const std::string& lSysString)
{
	m_turtle.reset();
	float deltaAngle = 45*PI/180;
	//std::unique_ptr<std::vector<std::unique_ptr<Mesh>>> pMeshVector(new std::vector<std::unique_ptr<Mesh>>());
	std::vector<std::unique_ptr<Mesh>> meshVector;
	
	for (std::string::const_iterator character = lSysString.cbegin(); character != lSysString.cend(); character++)
	{
		switch (*character)
		{
		case 'F': {
					std::unique_ptr<Mesh> pNewMesh = createCylinderBlock();
					meshVector.push_back(pNewMesh);
					break;
				  }
			case '+': m_turtle.changeHeading(deltaAngle);
				break;
			case '-': m_turtle.changeHeading(-deltaAngle);
				break;
		}
	}
	return 0;
	//return std::move(pMeshVector);
}

int LSystemMeshCreator::createCylinderBase(Mesh& mesh, int cylResolution)
{
	float cylRadius = 10;

	const Vector3 currentHeading = m_turtle.heading();
	const Vector3 curLeft = m_turtle.left();
	const Point3Df& curPosition = m_turtle.position();
	
	Point3Df pointBorder(curPosition, cylRadius, curLeft);
	Vector3 centerToBorder(curPosition, pointBorder); 

	for (int i = 0; i < cylResolution; i++)
	{
		centerToBorder.rotate(2*PI/cylResolution, currentHeading);
		Point3Df point(curPosition, cylRadius, centerToBorder);
		mesh.addVertex(point);
	}
	return mesh.numVerts() - cylResolution;
}

std::unique_ptr<Mesh> LSystemMeshCreator::createCylinderBlock()
{
    std::unique_ptr<Mesh> pMesh(new Mesh);
	int cylLength = 10;
	int cylResolution = 6;

	int index1 = createCylinderBase(*pMesh, cylResolution);
	m_turtle.translate(cylLength);
	int index2 = createCylinderBase(*pMesh, cylResolution);

	for (int i = 0; i < cylResolution; i++)
	{
		pMesh->addTriangle(index1 + i, index2 + i, index1 + (i + 1)%cylResolution);
		pMesh->addTriangle(index1 + (i + 1)%cylResolution, index2 + i, index2 + (i + 1)%cylResolution);
	}

	m_turtle.translate(cylLength);
	return pMesh;
}
*/