
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

#include "InitCheck.h"

RangeChecker::RangeChecker(uint32 *end)
{
	intptr size = intptr(end) - intptr(&m_Tag + 1);

	ASSERT((size & 0x3) == 0); // Aligned on 4 bytes
	size /= sizeof(uint32);

	// Set start tag to magic value
	m_Tag = 0xDEADBEEF;

	// Fill area between with other magic value
	uint32 *start = (&m_Tag + 1);
	for (intptr i = 0; i < size; ++i)
	{
		start[i] = 0xBAADC0DE;
	}

	// Set end tag to another magic value
	*end = 0xB00BFACE;
}

void RangeChecker::Verify(uint32 *end)
{
	intptr size = intptr(end) - intptr(&m_Tag + 1);
	ASSERT((size & 0x3) == 0); // Aligned on 4 bytes
	size /= sizeof(uint32);

	// Verify that start and end tags have not been tampered with
	ASSERT(m_Tag == 0xDEADBEEF);
	ASSERT(*end == 0xB00BFACE);

	// Verify that all memory has been initialized
	uint32 *start = (&m_Tag + 1);
	for (intptr i = 0; i < size; ++i)
	{
		ASSERT(start[i] != 0xBAADC0DE);
	}
}
