/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef _IRendererX2_H_
#define _IRendererX2_H_
inline int GetHeight()
{
	return STX_Service::GetWindowInstance()->GetHeight();
}
inline int GetWidth()
{
	return STX_Service::GetWindowInstance()->GetWidth();
}
inline void GetWindowPosition(int* x, int* y, unsigned int* z, unsigned int* w)
{
	STX_Service::GetWindowInstance()->GetWindowPosition(x, y, z, w);
}
inline int Getbpp()
{
	return STX_Service::GetWindowInstance()->Getbpp();
}
inline int Getheight()
{
	return STX_Service::GetWindowInstance()->Getheight();
}
inline int Getwidth()
{
	return STX_Service::GetWindowInstance()->Getwidth();
}
inline void HotKey_Iconify(void)
{
	STX_Service::GetWindowInstance()->HotKey_Iconify(void);
}
inline void HotKey_ToggleFullScreen(void)
{
	STX_Service::GetWindowInstance()->HotKey_ToggleFullScreen(void);
}
inline void HotKey_ToggleGrab(void)
{
	STX_Service::GetWindowInstance()->HotKey_ToggleGrab(void);
}
inline void Init(unsigned int awidth, unsigned int aheight, unsigned int abpp, const char* atitle)
{
	STX_Service::GetWindowInstance()->Init(awidth, aheight, abpp, atitle);
}
inline void Init2(int awidth, int aheight, int abpp)
{
	STX_Service::GetWindowInstance()->Init2(awidth, aheight, abpp);
}
inline int Quit(int returnCode=0)
{
	return STX_Service::GetWindowInstance()->Quit(returnCode);
}
inline void ReSize(int awidth, int aheight, int abpp)
{
	STX_Service::GetWindowInstance()->ReSize(awidth, aheight, abpp);
}
inline void SetCaption(const char * buf)
{
	STX_Service::GetWindowInstance()->SetCaption(buf);
}
inline void ShowCursor(bool pEnable)
{
	STX_Service::GetWindowInstance()->ShowCursor(pEnable);
}
inline int					GetNumSounds( )
{
	return STX_Service::GetAudioInstance()->GetNumSounds();
}
inline bool				Init(int  maxchannels = 64, FMOD_INITFLAGS  flags = FMOD_INIT_NORMAL)
{
	return STX_Service::GetAudioInstance()->Init(maxchannels, flags);
}
inline bool				IsPlaying(const char* szName)
{
	return STX_Service::GetAudioInstance()->IsPlaying(szName);
}
inline bool					Load(const char* file, const char* szName)
{
	return STX_Service::GetAudioInstance()->Load(file, szName);
}
inline bool				Play(const char* szName, int loopCount = 1)
{
	return STX_Service::GetAudioInstance()->Play(szName, loopCount);
}
inline void				Stop(const char* szName)
{
	STX_Service::GetAudioInstance()->Stop(szName);
}
inline void				StopAll( )
{
	STX_Service::GetAudioInstance()->StopAll();
}
inline void				Update( )
{
	STX_Service::GetAudioInstance()->Update();
}
#endif

