
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

#pragma once

#include "Util/Array.h"

class Config
{
public:
	Config();
	~Config();

	bool Load(const char *filename);
	bool Save(const char *filename) const;

	void CreateSection(const char *name);
	bool SetSection(const char *name);

	bool Exists(const char *name) const;

	bool GetBool(const char *name, const bool default_value) const;
	int GetInteger(const char *name, const int default_value) const;
	float GetFloat(const char *name, const float default_value) const;
	const char *GetString(const char *name, const char *default_value) const;

	void SetBool(const char *name, const bool value);
	void SetInteger(const char *name, const int value);
	void SetFloat(const char *name, const float value);
	void SetString(const char *name, const char *value);

private:
	const char *GetValue(const char *name) const;
	void SetValue(const char *name, const char *value, const int length);

	static int FindValue(const char *str, int *length = nullptr);

	struct Section
	{
		Section(){}
		Section(const char *name);
		Section(const char *name, const int length);

		int Find(const char *name) const;

		Array<char *> m_Lines;
		char *m_Name;
	};

	Array<Section *> m_Sections;
	int m_ActiveSection;
};
