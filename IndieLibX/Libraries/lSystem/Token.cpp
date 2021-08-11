#include "precomp.h"
#include <map>
#include <sstream>
#include "Token.h"


//There are two types of tokens that may be encountered in parsing of L-Systems. Normal tokens (associated with non-expanded productions). The keys in this case are not yet associated with meaningful parameters.
//The other type of tokens are Expanded tokens. These are created while applying grammar to extend L-System strings. These correctly associate keys with corresponding values.

//=====================Token impls=======================
const char* Token::createToken(const char* pCurrent)
{
	this->m_params.clear();
	std::string currentString;

	bool fShouldReadParamsNext = false;
	bool fReadingParams = false;
	bool fDoneReadingParams = false;
	std::string attribScaleValue = "";

	while (*pCurrent != '\0' && !fDoneReadingParams)
	{
		char ch = *pCurrent++;
		if (fReadingParams || fShouldReadParamsNext)
		{
			if (ch == '(')
			{
				fReadingParams = true;
			}
			else if (ch == ')')
			{
				fDoneReadingParams = true;
			}
			else
			{
				if (!fReadingParams) //We come to a new token, without encountering any parameters <Assume well formed>
				{
					pCurrent--;
					break;
				}
				else if (isalpha(ch))
				{
					float scaleValue = 1;
					if (!attribScaleValue.empty())
					{
						scaleValue = atof(attribScaleValue.c_str());
					}
					m_params.insert(std::pair<char, float>(ch, scaleValue));
					attribScaleValue = "";
				}
				else if (ch == ',')
				{
					//Do nothing. this can be exploited to allow for variable length names for params
				}
				else
				{
					attribScaleValue.push_back(ch);
				}
			}
		}
		else
		{
			m_alpha = ch;
			fShouldReadParamsNext = true;
		}
	}
	return pCurrent;
}

bool Token::getParamValueForKey(char key, __out float& value) const throw()
{
	std::map<char, float>::const_iterator it = m_params.find(key); 
	if (it != m_params.end()) 
	{
		value = it->second;
		return true;
	}
	return false;
}

bool Token::getParamValue(__out float& value) const throw()
{
	if (m_params.size() != 0)
	{
		assert(m_params.size() == 1);
		value = (m_params.begin())->second;
		return true;
	}
	return false;
}

bool Token::setParamValueForKey(char key, float value) throw()
{
	std::map<char, float>::iterator it = m_params.find(key); 
	if (it != m_params.end()) 
	{
		it->second = value;
		return true;
	}
	return false;
}

//===========Expanded Token overrides and impls=================

std::string ExpandedToken::getStringForToken() const throw()
{
	std::string stringForToken;
	stringForToken.push_back(m_alpha);
	for (std::map<char, float>::const_iterator attrib = m_params.cbegin(); attrib != m_params.cend(); attrib++)
	{
		if (attrib == m_params.cbegin())
		{
			stringForToken.push_back('(');
		}
		else
		{
			stringForToken.push_back(',');
		}
		stringForToken.push_back(attrib->first);
		stringForToken.push_back(':');
		std::stringstream ss (std::stringstream::in | std::stringstream::out);
		ss << attrib->second;
		stringForToken.append(ss.str());		
	}
	if (m_params.size() > 0)
	{
		stringForToken.push_back(')');
	}
	
	return stringForToken;
}

const char* ExpandedToken::createToken(const char* pCurrent)
{
	this->m_params.clear();
	std::string currentString;

	bool fShouldReadParamsNext = false;
	bool fReadingParams = false;
	bool fDoneReadingParams = false;
	std::string attribScaleValue = "";

	while (*pCurrent != '\0' && !fDoneReadingParams)
	{
		char ch = *pCurrent++;
		if (fReadingParams || fShouldReadParamsNext)
		{
			if (ch == '(')
			{
				fReadingParams = true;
			}
			else if (ch == ')')
			{
				fDoneReadingParams = true;
			}
			else
			{
				if (!fReadingParams) //We come to a new token, without encountering any parameters <Assume well formed>
				{
					pCurrent--;
					break;
				}
				else if (isalpha(ch))
				{
					assert(*pCurrent == ':');
					pCurrent++; //Skip the ':'

					std::string paramValue = "";
					while (*pCurrent != ')' && *pCurrent != ',')
					{
						paramValue.push_back(*pCurrent);
						pCurrent++;
					}
					float fParamValue = atof(paramValue.c_str());
					m_params.insert(std::pair<char, float>(ch, fParamValue));
					attribScaleValue = "";
				}
				else
				{
					assert(ch == ','); //This is the only other valid case
				}
			}
		}
		else
		{
			m_alpha = ch;
			fShouldReadParamsNext = true;
		}
	}
	return pCurrent;
}

void ExpandedToken::createToken(const ExpandedToken& currentToken, const ExpandedToken* pAttachedToken, const Token& tokenToUse)
{
	this->m_params.clear();
	m_alpha = tokenToUse.getChar();
	const ExpandedToken& paramSupplyingToken = (pAttachedToken == NULL)? currentToken : *pAttachedToken;
	for (std::map<char, float>::const_iterator paramIt = tokenToUse.cParamBegin(); paramIt != tokenToUse.cParamEnd(); paramIt++)
	{
		float boundValueForParam;
		bool hasKey = paramSupplyingToken.getParamValueForKey(paramIt->first, boundValueForParam);
		assert(hasKey);

		float expandedValue = paramIt->second * boundValueForParam;
		m_params.insert(std::pair<char, float>(paramIt->first, expandedValue));
	}
}

bool StringTokenizer::getNextToken(__out ExpandedToken& token) throw()
{
	if (*m_pCurPosition == '\0')
	{
		return false;
	}
	m_pCurPosition = token.createToken(m_pCurPosition);
	return true;
}