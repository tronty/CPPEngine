#include "precomp.h"
#include "LSystemGrowthForest.h"

/*void LSystemGrowthForest::createLSystemForest()
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
}
*/