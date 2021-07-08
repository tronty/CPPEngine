#include "precomp.h"
#include "EnvironInteractor.h"
#include "Token.h"
#include "LSystemInterpreter.h"
#include "drawable.h"
#include "VoxelWorld.h"

EnvironmentInteractor* EnvironmentInteractor::s_pEnvironmentInteractor = NULL;

EnvironmentInteractor::EnvironmentInteractor(const VoxelWorld& world) : m_world(world), m_pLSystemInterpreter(new LSystemQueryableInterpreter()),
																		m_pIlluminationTableConstructor(new IlluminationTableConstructor(world))
{
}

void EnvironmentInteractor::getEnvironParameters(ExpandedToken& tokenToWriteTo, const std::string& currentString)
{
	m_pLSystemInterpreter->interpret(currentString);
	Point3Df currentPosition = m_pLSystemInterpreter->getPosition();
	Matrix4 orientation = m_pLSystemInterpreter->getOrientation();

	std::unique_ptr<IlluminationTable> pIlluminationTable = m_pIlluminationTableConstructor->constructTable(currentPosition, orientation);
	
	IlluminationResult result = pIlluminationTable->findMaxIllumination(m_pLSystemInterpreter->getHeading());
	Vector4 maxIlluminationGridLocationLocal = result.diretion();
	Vector4 maxIlluminationGridLocationGlobal = orientation * maxIlluminationGridLocationLocal;
	Vector3 maxIlluminationGridLocationGlobal3(maxIlluminationGridLocationGlobal.x, maxIlluminationGridLocationGlobal.y, maxIlluminationGridLocationGlobal.z);
	
	if (tokenToWriteTo.hasParam('a') && !tokenToWriteTo.hasParam('l'))
	{		
		float angle;
		tokenToWriteTo.getParamValueForKey('a', angle);
		float angleDiff = 180*maxIlluminationGridLocationGlobal3.angle(m_pLSystemInterpreter->getHeading())/PI;
		tokenToWriteTo.setParamValueForKey('a', angleDiff);
	}
	if (tokenToWriteTo.hasParam('b') && !tokenToWriteTo.hasParam('l'))
	{
		float angle;
		tokenToWriteTo.getParamValueForKey('b', angle);
		float angleDiff = 180*maxIlluminationGridLocationGlobal3.angle(m_pLSystemInterpreter->getHeading())/PI;
		tokenToWriteTo.setParamValueForKey('b', angleDiff);
	}
	if (tokenToWriteTo.hasParam('l'))
	{
		float length;
		tokenToWriteTo.getParamValueForKey('l', length);
		tokenToWriteTo.setParamValueForKey('l', length * result.illumination());
	}
}
