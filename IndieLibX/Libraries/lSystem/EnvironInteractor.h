#ifndef _ENVIRONINTERACTOR_H_
#define _ENVIRONINTERACTOR_H_

#include "LSystemInterpreter.h"
#include "IlluminationTable.h"
class ExpandedToken;
class VoxelWorld;

class EnvironmentInteractor
{
private:
	std::unique_ptr<LSystemQueryableInterpreter> m_pLSystemInterpreter;
	const VoxelWorld& m_world;
	std::unique_ptr<IlluminationTableConstructor> m_pIlluminationTableConstructor;
	EnvironmentInteractor(const VoxelWorld& world);
	EnvironmentInteractor(const EnvironmentInteractor& other);
	static EnvironmentInteractor* s_pEnvironmentInteractor;

public:
	void getEnvironParameters(ExpandedToken& tokenToWriteTo, const std::string& currentString);
	static void createEnvironmentInteractor(const VoxelWorld& world) throw() { assert(s_pEnvironmentInteractor == NULL); s_pEnvironmentInteractor = new EnvironmentInteractor(world); }
	static EnvironmentInteractor& getEnvironmentInteractor() throw() { assert(s_pEnvironmentInteractor != NULL); return *s_pEnvironmentInteractor; }
	static void deleteEnvironmentInteractor() { delete s_pEnvironmentInteractor; s_pEnvironmentInteractor = NULL;}
};

#endif//ENVIRONINTERACTOR_H_