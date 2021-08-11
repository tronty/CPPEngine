#ifdef NO_USE_OF_SOUND
	#define	SND_INIT()
	#define SND_UPDATE(yes)
	#define SND_START()
	#define SND_BOOM();
#else
#include "nv_rtModules/nv_rtSoundItf.h"
static IRTFactory *pSndFactory;
static SmartPtr<ISound> pSndBgnd;
static SmartPtr<ISound> pSndExplode[2];
static int currentexplodechannel = 0;
static int mute = true;
static float bgndvolume = 0.5;
DECL_PLUG(bgndvolume);
static float prevtrigger = 0;
static float sndtrigger = 0;
DECL_PLUG(sndtrigger);

extern data_path progpath;


void SND_INIT()
{
	HWND hWnd;
	hWnd = FindWindow(NULL,"NRenderGL");
	if(!hWnd)
	{
		LOGMSG(LOG_WARN, "No window found to attach sound effect");
		return;
	}
	GETRTFACTORY(pSndFactory, "nv_rtSound.dll");
	if(pSndFactory)
	{
		std::string fname;
		LOGMSG(LOG_SND , "found sound module nv_rtSound.dll");
		fname = progpath.get_file("atomicbomb.wav");
		pSndBgnd = (ISound *)pSndFactory->CreateInstance("sound");
		if(pSndBgnd && (!fname.empty()))
		{
			LOGMSG(LOG_SND , "creating sound bgnd with %s", fname.c_str());
			pSndBgnd->Init(fname.c_str(), true, 0.5, (unsigned int)hWnd);
		}
		else 
		{ 
			LOGMSG(LOG_WARN, "couldn't create bgnd sound"); 
		}
		fname = progpath.get_file("bomb.mp3");
		pSndExplode[0] = (ISound *)pSndFactory->CreateInstance("sound");
		pSndExplode[1] = (ISound *)pSndFactory->CreateInstance("sound");
		if(pSndExplode[0]&& (!fname.empty()))
		{
			LOGMSG(LOG_SND , "creating sound 0 with %s", fname.c_str());
			pSndExplode[0]->Init(fname.c_str(), false, 0.3, (unsigned int)hWnd);
		}
		if(pSndExplode[1]&& (!fname.empty()))
		{
			LOGMSG(LOG_SND , "creating sound 1 with %s", fname.c_str());
			pSndExplode[1]->Init(fname.c_str(), false, 0.3, (unsigned int)hWnd);
		}
		if((!pSndExplode[0]) ||(!pSndExplode[1]) || fname.empty())
		{ 
			LOGMSG(LOG_WARN, "couldn't create forgnd sounds"); 
		}
		currentexplodechannel = 0;
	}
	else 
	{ 
		LOGMSG(LOG_WARN, "nv_rtSound.dll not found"); 
	}
	INIT_PLUG_FLOAT(bgndvolume, NULL, NULL);
	INIT_PLUG_FLOAT(sndtrigger, NULL, NULL);

}
void SND_START()
{
	if(mute)
		return;
 	if(pSndBgnd)
	{
		pSndBgnd->Play();
	}
	if(pSndExplode[0])
		pSndExplode[0]->Stop();
	if(pSndExplode[1])
		pSndExplode[1]->Stop();
}

void SND_BOOM()
{
	if(mute)
		return;
	if(pSndExplode[currentexplodechannel])
	{
		pSndExplode[currentexplodechannel]->Play();
		pSndExplode[currentexplodechannel]->SetVolume(0.4f);
	}

	currentexplodechannel ^= 1;
}
void SND_UPDATE(int yes)
{
	if(mute ^ yes)
	{
		if(yes)
		{
			mute = 1;
			if(pSndBgnd)
				pSndBgnd->Stop();
			/*if(pSndExplode[0])
				pSndExplode[0]->Stop();
			if(pSndExplode[1])
				pSndExplode[1]->Stop();*/
		}
		else
		{
			mute = 0;
			if(pSndBgnd)
				pSndBgnd->Play();
		}
	}
	if(!mute)
	{
	 	if(pSndBgnd)
			pSndBgnd->SetVolume(bgndvolume);
		/*if(((int)(prevtrigger+0.5)) < ((int)(sndtrigger+0.5)))
		{
			if(pSndExplode[currentexplodechannel])
				pSndExplode[currentexplodechannel]->Play();

			currentexplodechannel ^= 1;
		}*/
	}
	prevtrigger = sndtrigger;
}

#endif