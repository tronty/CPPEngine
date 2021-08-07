#ifndef _LSYTEMDRAWER_H_
#define _LSYTEMDRAWER_H_

#include "LSystemInterpreter.h"
#include "Mesh.h"

class LSystem;
class Renderer;

class LSystemController
{
private:
	IDrawable* m_pCurrentInterpretation;
	std::unique_ptr<LSystem> m_pSystem;
	std::unique_ptr<LSystemInterpreter> m_pInterpreter;
	std::unique_ptr<Mesh> m_pMesh;
	Renderer& m_renderer;
	int m_updateNumber;

public:
	LSystemController(std::unique_ptr<LSystem> pSystem, Renderer& renderer);
	void onUpdate();
};

#endif//_LSYTEMDRAWER_H_