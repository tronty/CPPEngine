
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

#include "../Platform.h"
#include "Array.h"

class Tokenizer
{
public:
	Tokenizer(uint num_buffers = 1);
	~Tokenizer();

	void SetString(const char* string);
	bool SetFile(const char* filename);
	void Reset();

	template <bool (&isAlpha)(const char ch)>
	bool GoToNext()
	{
		const char* str = m_Str;
		const uint length = m_Length;

		m_Start = m_End;

		while (m_Start < length && IsWhiteSpace(str[m_Start]))
			m_Start++;
		m_End = m_Start + 1;

		if (m_Start < length)
		{
			if (IsNumeric(str[m_Start]))
			{
				while (IsNumeric(str[m_End]) || str[m_End] == '.')
					m_End++;
			}
			else if (isAlpha(str[m_Start]))
			{
				while (isAlpha(str[m_End]) || IsNumeric(str[m_End]))
					m_End++;
			}
			return true;
		}
		return false;
	}

	inline bool GoToNext()
	{
		return GoToNext<IsAlphabetical>();
	}

	bool GoToNextLine();

	template <bool (&isAlpha)(const char ch)>
	char* Next()
	{
		if (GoToNext<isAlpha>())
		{
			uint size = m_End - m_Start;
			char* buffer = GetBuffer(size + 1);
			strncpy(buffer, m_Str + m_Start, size);
			buffer[size] = '\0';
			return buffer;
		}
		return nullptr;
	}

	inline char* Next()
	{
		return Next<IsAlphabetical>();
	}

	template <bool (&isAlpha)(const char ch)>
	char* NextAfterToken(const char* token)
	{
		while (GoToNext<isAlpha>())
		{
			if (strncmp(m_Str + m_Start, token, m_End - m_Start) == 0)
			{
				return Next<isAlphabetical>();
			}
		}

		return nullptr;
	}

	char* NextLine();
private:
	char* m_Str;
	uint m_Length;
	uint m_Start, m_End;
	uint m_Capacity;

	uint m_CurrentBuffer;

	struct TokBuffer
	{
		char* Buffer;
		uint  BufferSize;
	};
	Array <TokBuffer> m_Buffers;

	char* GetBuffer(uint size);
};
