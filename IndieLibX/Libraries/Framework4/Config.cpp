
/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*   _       _   _       _   _       _   _       _     _ _ _ _   *
*  |_|     |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|  *
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _     *
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_   *
*  |_|     |_| |_|_ _ _|_| |_|     |_| |_|_ _ _|_|  _ _ _ _|_|  *
*  |_|     |_|   |_|_|_|   |_|     |_|   |_|_|_|   |_|_|_|_|    *
*                                                               *
*                     http://www.humus.name                     *
*                                                                *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.             *
*                                                                *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "Config.h"

#include <stdio.h>
#include <string.h>

Config::Config() : m_ActiveSection(0)
{
}

Config::~Config()
{
	// Free all memory
	uint section_count = m_Sections.GetCount();
	while (section_count)
	{
		--section_count;

		Section *section = m_Sections[section_count];

		uint count = section->m_Lines.GetCount();
		while (count)
		{
			--count;
			delete [] section->m_Lines[count];
		}

		delete [] section->m_Name;
		delete section;
	}
}

bool Config::Load(const char *filename)
{
	FILE *file = fopen(filename, "rt");
	if (file == nullptr)
		return false;

	Section *section = nullptr;

	// Read settings line by line
	char line[256];
	while (fgets(line, sizeof(line), file))
	{
		if (line[0] == '[')
		{
			// New section
			char *start = line + 1;
			char *end = strchr(start, ']');
			if (!end)
			{
				end = start;
				while (*end && *end != ' ' && *end != '\t') ++end;
			}
			int len = (int) (end - start);

			section = new Section(start, len);
			m_Sections.Add(section);
		}
		else
		{
			if (section == nullptr)
			{
				// Add a default section
				section = new Section();
				m_Sections.Add(section);
				section->m_Name = nullptr;
			}

			size_t end = strlen(line) - 1;
			// Trim new-line characters at end of string
			while (line[end] == '\n' || line[end] == '\r')
			{
				line[end] = '\0';
				--end;
			}

			size_t size = end + 2;

			char *new_line = new char[size];
			memcpy(new_line, line, size * sizeof(char));
			section->m_Lines.Add(new_line);
		}
	}

	fclose(file);

	return true;
}

bool Config::Save(const char *filename) const
{
	FILE *file = fopen(filename, "wt");
	if (file == nullptr)
		return false;

	uint section_count = m_Sections.GetCount();
	for (uint s = 0; s < section_count; s++)
	{
		Section *section = m_Sections[s];
		if (section->m_Name)
		{
			fprintf(file, "[%s]\n", section->m_Name);
		}

		uint count = section->m_Lines.GetCount();
		for (uint i = 0; i < count; i++)
		{
			fprintf(file, "%s\n", section->m_Lines[i]);
		}
	}

	fclose(file);

	return true;
}

void Config::CreateSection(const char *name)
{
	if (SetSection(name))
		return;

	Section *section = new Section(name);
	m_ActiveSection = m_Sections.Add(section);
}

bool Config::SetSection(const char *name)
{
	if (name == nullptr)
	{
		// Select default section, if it exists
		if (m_Sections.GetCount() && m_Sections[0]->m_Name == nullptr)
		{
			m_ActiveSection = 0;
			return true;
		}
	}
	else
	{
		uint count = m_Sections.GetCount();
		for (uint i = 0; i < count; i++)
		{
			if (strcmp(name, m_Sections[i]->m_Name) == 0)
			{
				m_ActiveSection = i;
				return true;
			}
		}
	}

	return false;
}

bool Config::Exists(const char *name) const
{
	return (m_Sections[m_ActiveSection]->Find(name) >= 0);
}

bool Config::GetBool(const char *name, const bool default_value) const
{
	const char *value = GetValue(name);
	if (value)
		return (atoi(value) != 0);
	return default_value;
}

int Config::GetInteger(const char *name, const int default_value) const
{
	const char *value = GetValue(name);
	if (value)
		return atoi(value);
	return default_value;
}

float Config::GetFloat(const char *name, const float default_value) const
{
	const char *value = GetValue(name);
	if (value)
		return (float) atof(value);
	return default_value;
}

const char *Config::GetString(const char *name, const char *default_value) const
{
	const char *value = GetValue(name);
	if (value)
		return value;
	return default_value;
}

void Config::SetBool(const char *name, const bool value)
{
	SetValue(name, value? "1" : "0", 1);
}

void Config::SetInteger(const char *name, const int value)
{
	char str[16];
	int len = sprintf(str, "%d", value);

	SetValue(name, str, len);
}

void Config::SetFloat(const char *name, const float value)
{
	char str[16];
	int len = sprintf(str, "%g", value);

	SetValue(name, str, len);
}

void Config::SetString(const char *name, const char *value)
{
	SetValue(name, value, (int) strlen(value));
}

const char *Config::GetValue(const char *name) const
{
	if (m_Sections.GetCount())
	{
		int index = m_Sections[m_ActiveSection]->Find(name);
		if (index >= 0)
		{
			const char *str = m_Sections[m_ActiveSection]->m_Lines[index];
			int location = FindValue(str);
			if (location >= 0)
				return str + location;
		}
	}

	return nullptr;
}

void Config::SetValue(const char *name, const char *value, const int length)
{
	char str[256];

	Section *section = m_Sections[m_ActiveSection];

	int index = section->Find(name);
	if (index >= 0)
	{
		const char *line = section->m_Lines[index];
		int value_len;
		int location = FindValue(line, &value_len);
		if (location >= 0)
		{
			// Variable with previous value. Replace value but preserve all other existing text.
			char *start = section->m_Lines[index];

			if (value_len == length)
			{
				// Same length, just overwrite
				strncpy(start + location, value, length);
			}
			else
			{
				strncpy(str, start, location);
				int len = location + sprintf(str + location, "%s%s", value, start + location + value_len);

				char *new_line = new char[len + 1];
				strcpy(new_line, str);

				delete [] section->m_Lines[index];
				section->m_Lines[index] = new_line;
			}
		}
		else
		{
			// Variable without value. Insert value and preserve any existing text after the variable name.
			int len = sprintf(str, "%s = %s%s", name, value, line + strlen(name));

			char *new_line = new char[len + 1];
			strcpy(new_line, str);

			delete [] section->m_Lines[index];
			section->m_Lines[index] = new_line;
		}	
	}
	else
	{
		// Insert new variable and value
		int len = sprintf(str, "%s = %s", name, value);

		char *new_line = new char[len + 1];
		strcpy(new_line, str);

		section->m_Lines.Add(new_line);
	}
}

int Config::FindValue(const char *line, int *length)
{
	// Find equals sign
	const char *str = strchr(line, '=');
	if (str == nullptr)
		return -1;

	// Scan past any space after '='
	do
	{
		++str;
	} while (*str && IsWhiteSpace(*str));

	if (length)
	{
		// Find length of value in characters
		int len = 0;
		while (str[len] && !IsWhiteSpace(str[len])) ++len;
		*length = len;
	}
	return (int) (str - line);
}

int Config::Section::Find(const char *name) const
{
	int n = (int) strlen(name);

	uint count = m_Lines.GetCount();
	for (uint i = 0; i < count; i++)
	{
		// Compare to the length of <name> only since the string also contains the value
		if (strncmp(m_Lines[i], name, n) == 0)
		{
			// Make sure the variable name really ended here
			char ch = m_Lines[i][n];
			if (!IsAlphaNumeric(ch))
				return i;
		}
	}

	return -1;
}

Config::Section::Section(const char *name)
{
	size_t length = strlen(name);
	m_Name = new char[length + 1];
	strncpy(m_Name, name, length);
	m_Name[length] = '\0';
}

Config::Section::Section(const char *name, const int length)
{
	m_Name = new char[length + 1];
	strncpy(m_Name, name, length);
	m_Name[length] = '\0';
}
