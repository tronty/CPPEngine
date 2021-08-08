#include "precomp.h"
#include "LSystemController.h"
#include "LSystemInterpreter.h"
#include "LSystemMeshCreator.h"
#include "LSystem.h"
#include "Mesh.h"
#include "Renderer.h"
#include <time.h>
#include <iostream>

LSystemController::LSystemController(std::unique_ptr<LSystem> pSystem, Renderer& renderer) : m_pSystem(std::move(pSystem)), 
																							 m_pInterpreter(new LSystemInterpreter()), m_renderer(renderer),
																							 m_updateNumber(0),
																							 m_pCurrentInterpretation(NULL)
{
	srand(static_cast<unsigned int>(time(NULL)));
}

void LSystemController::onUpdate()
{
	if (m_updateNumber < m_pSystem->getNumDerivations())
	{
		m_pSystem->applyRules();
		std::unique_ptr<IDrawable> pDrawable = m_pInterpreter->interpret(m_pSystem->getCurrentValue());
		m_renderer.removeDrawable(m_pCurrentInterpretation);
		m_pCurrentInterpretation = pDrawable.get();
		m_renderer.addDrawable(std::move(pDrawable));
		m_updateNumber++;
		if (m_updateNumber == m_pSystem->getNumDerivations())
		{
			std::string currentValue = m_pSystem->getCurrentValue();
			std::cout<<currentValue.c_str();
		}
	}
}
