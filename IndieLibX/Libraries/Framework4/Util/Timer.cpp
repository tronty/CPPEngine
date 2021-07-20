
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

#include "Timer.h"

Timer::Timer()
{
#ifdef WIN32
	m_Frequency = 0;
	m_Start.QuadPart = 0;
	m_Current.QuadPart = 0;
#else
	m_Start.tv_sec = 0;
	m_Start.tv_usec = 0;
	m_Current.tv_sec = 0;
	m_Current.tv_usec = 0;
#endif

	m_FrameTime = 0.01f;
}

Timer::~Timer()
{
}

void Timer::Reset()
{
#ifdef WIN32
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&m_Start);
	m_Frequency = 1.0f / freq.QuadPart;
#else
	gettimeofday(&m_Start, nullptr);
#endif

	m_Current = m_Start;
}

void Timer::Update()
{
#ifdef WIN32
	LARGE_INTEGER last = m_Current;
	QueryPerformanceCounter(&m_Current);

	m_FrameTime = float(m_Current.QuadPart - last.QuadPart) * m_Frequency;
	m_TotalTime = float(m_Current.QuadPart - m_Start.QuadPart) * m_Frequency;
#else
	timeval last = m_Current;
	gettimeofday(&m_Current, nullptr);

	m_FrameTime = float(m_Current.tv_sec - last.tv_sec) + 0.000001f * float(m_Current.tv_usec - last.tv_usec);
	m_TotalTime = float(m_Current.tv_sec - m_Start.tv_sec) + 0.000001f * float(m_Current.tv_usec - m_Start.tv_usec);
#endif
}
