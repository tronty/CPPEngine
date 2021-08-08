#include "precomp.h"
#include "LSystem.h"
#include "EnvironInteractor.h"
#include <iostream>
#include <fstream>
#include <iostream>
#include <string>
#include "utils.h"
#include "VoxelWorld.h"

namespace
{
	enum ParserState
	{
		Invalid,
		ParseAxiom,
		ParseProduction,
	};
}

ProductionLHS::ProductionLHS(const std::string& lhs)
{
	const char* tokenEnd = m_token.createToken(lhs.c_str());
}

bool ProductionLHS::matches(const Token& token) const throw()
{
	return (token.getChar() == m_token.getChar());
}

ProductionRHS::ProductionRHS(const std::string& rhs)
{
	const char* pCur = rhs.c_str();
	while (*pCur != '\0')
	{
		Token currentToken;
		pCur = currentToken.createToken(pCur);
		m_tokens.push_back(currentToken);
	}
}

void Production::applyTo(const ExpandedToken& currentToken, std::string& resultString) const throw()
{
	//TODO msati3: can add conditions here
	std::unique_ptr<ExpandedToken> pAttachedToken = NULL;	
	EnvironmentInteractor& environInteractor = EnvironmentInteractor::getEnvironmentInteractor();
	for (ProductionRHS::const_iterator tokenIt = m_rhs.cbegin(); tokenIt != m_rhs.cend(); tokenIt++)
	{
		ExpandedToken substitutedToken;
		substitutedToken.createToken(currentToken, pAttachedToken.get(), *tokenIt);

		if (substitutedToken.getChar() == '?') //This is a call to a query point
		{
			environInteractor.getEnvironParameters(substitutedToken, resultString);
			pAttachedToken.reset(new ExpandedToken(substitutedToken));
		}
		else
		{
			resultString.append(substitutedToken.getStringForToken());
			pAttachedToken.release();
		}
	}
	assert(pAttachedToken == NULL);
}

std::unique_ptr<LSystem> LSystemBuilder::buildLSystem(std::string fileName, const VoxelWorld& world)
{
	std::fstream lsystemFile;
	lsystemFile.open(fileName, std::ios_base::in);
	ParserState parserState;
	int numDerivations = -1;

	std::unique_ptr<ExpandedToken> pAxiom(new ExpandedToken());
	std::unique_ptr<std::vector<Production>> productions(new std::vector<Production>());
	std::vector<std::string> productionValues;
	
    std::string readLine;
	while (std::getline(lsystemFile, readLine))
	{
		const char* pDerivations = NULL;
		if ((pDerivations = strstr(readLine.c_str(), "numDerivations")) !=  nullptr)
		{
			while (!isdigit(*(++pDerivations)));
			numDerivations = atoi(pDerivations);
			continue;
		}
		if (std::string("Axiom").compare(readLine) == 0)
		{
			parserState = ParseAxiom;
			continue;
		}
		else if (std::string("Productions").compare(readLine) == 0)
		{
			parserState = ParseProduction;
			continue;
		}

		switch(parserState)
		{
		case ParseAxiom: pAxiom->createToken(readLine.c_str());
			break;
		case ParseProduction: split(readLine, '>', productionValues);
			assert(productionValues.size() == 2);
			
			std::vector<std::string> rhsSideProductionValues;
			split(productionValues[1], ':', rhsSideProductionValues);
			if (rhsSideProductionValues.size() == 1)
			{
				rhsSideProductionValues.push_back("1.00");
			}
			assert(rhsSideProductionValues.size() == 2);

			productions->push_back(Production(productionValues[0], rhsSideProductionValues[0], atof(rhsSideProductionValues[1].c_str())));
			productionValues.clear();
			break;
		}
	}

	std::unique_ptr<LSystem> pSystem(new LSystem(numDerivations, std::move(pAxiom), productions));
	EnvironmentInteractor::createEnvironmentInteractor(world);
	return pSystem;
}

void LSystem::getProductionsFor(const Token& currentToken, __out std::vector<const Production*>& matchingProductions) const throw()
{
	for (std::vector<Production>::const_iterator production = m_pProductions->cbegin(); production != m_pProductions->cend(); production++)
	{
		if (production->matches(currentToken))
		{
			matchingProductions.push_back(&*production);
		}
	}
}

void LSystem::expandCurrentToken(const ExpandedToken& currentToken, __inout std::string& resultString, const std::vector<const Production*>& matchingProductions) const throw()
{
	if (matchingProductions.size() == 0)
	{
		resultString.append(currentToken.getStringForToken());
		return;
	}
	int random = rand();
	float value = (float)random/RAND_MAX;
	float cumulativeProbability = 0;
	for (std::vector<const Production*>::const_iterator production = matchingProductions.cbegin(); production != matchingProductions.cend(); production++)
	{
		cumulativeProbability += (*production)->probability();
		if (cumulativeProbability >= value)
		{
			(*production)->applyTo(currentToken, resultString);
			break;
		}
	}
}

void LSystem::applyRules()
{
	std::string newString;

	const char* pCur = m_currentString.c_str();
	while (*pCur != '\0')
	{
		ExpandedToken currentToken;
		pCur = currentToken.createToken(pCur);
		std::vector<const Production*> matchingProductions;
		getProductionsFor(currentToken, matchingProductions);
		expandCurrentToken(currentToken, newString, matchingProductions);
	}
	m_currentString = newString;
}
