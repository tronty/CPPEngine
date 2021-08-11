
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

#include "Tokenizer.h"
#include <stdio.h>
#include <string.h>

Tokenizer::Tokenizer(uint num_buffers)
{
	m_Str = nullptr;
	m_Capacity = 0;
	
	m_Buffers.SetCapacity(num_buffers);
	m_Buffers.SetCount(num_buffers);
	memset(m_Buffers.GetArray(), 0, num_buffers * sizeof(TokBuffer));

	Reset();
}

Tokenizer::~Tokenizer()
{
	const uint count = m_Buffers.GetCount();
	for (uint i = 0; i < count; i++)
	{
		delete [] m_Buffers[i].Buffer;
	}
	delete [] m_Str;
}

void Tokenizer::SetString(const char* string)
{
	m_Length = (uint) strlen(string);

	// Increase capacity if necessary
	if (m_Length >= m_Capacity)
	{
		delete [] m_Str;

		m_Capacity = m_Length + 1;
		m_Str = new char[m_Capacity];
	}

	strcpy(m_Str, string);

	Reset();
}

bool Tokenizer::SetFile(const char* filename)
{
	delete [] m_Str;

	FILE* file = fopen(filename, "rb");
	if (file == nullptr)
		return false;

	fseek(file, 0, SEEK_END);
	m_Length = ftell(file);
	fseek(file, 0, SEEK_SET);

	m_Str = new char[(m_Length + 1) * sizeof(char)];
	fread(m_Str, m_Length, 1, file);
	m_Str[m_Length] = '\0';

	fclose(file);
		
	Reset();
	return true;
}

void Tokenizer::Reset()
{
	m_End = 0;
}

bool Tokenizer::GoToNextLine()
{
	const uint length = m_Length;
	if (m_End < length)
	{
		const char* str = m_Str;

		m_Start = m_End;

		while (m_End < length && !IsNewLine(str[m_End]))
			m_End++;

		if (IsNewLine(str[m_End + 1]) && str[m_End] != str[m_End + 1])
			m_End += 2;
		else
			m_End++;

		return true;
	}
	return false;
}

char* Tokenizer::NextLine()
{
	if (GoToNextLine())
	{
		uint size = m_End - m_Start;
		char* buffer = GetBuffer(size + 1);
		strncpy(buffer, m_Str + m_Start, size);
		buffer[size] = '\0';
		return buffer;
	}
	return nullptr;
}

char* Tokenizer::GetBuffer(uint size)
{
	m_CurrentBuffer++;
	if (m_CurrentBuffer >= m_Buffers.GetCount())
		m_CurrentBuffer = 0;

	if (size > m_Buffers[m_CurrentBuffer].BufferSize)
	{
		delete m_Buffers[m_CurrentBuffer].Buffer;
		m_Buffers[m_CurrentBuffer].Buffer = new char[size];
		m_Buffers[m_CurrentBuffer].BufferSize = size;
	}

	return m_Buffers[m_CurrentBuffer].Buffer;
}
