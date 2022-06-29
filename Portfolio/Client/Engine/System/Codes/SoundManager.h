#ifndef SOUNDMANAGER_H__
#define SOUNDMANAGER_H__

#include "fmod.h"
#include "fmod.hpp"
#include "fmod_dsp.h"

#include "Engine_Macro.h"
#include "Engine_CrashDump.h"

BEGIN(Engine)

class ENGINE_DLL CSoundManager
{
	DECLARE_SINGLETON(CSoundManager)	
private:
	enum Sound_Type
	{
		SOUND_BASE			= 0,
		SOUND_BACKGROUND	= SOUND_BASE + 1,
		SOUND_VOICE			= SOUND_BASE + 2,
		SOUND_EFFECT		= SOUND_BASE + 3,
		SOUND_END			= SOUND_BASE + 10
	};
	enum System_Type
	{
		SYSTEM_STREAM = 0,
		SYSTEM_EFFECT = SYSTEM_STREAM + 1,
		SYSTEM_END = SYSTEM_STREAM + 2,
	};
private:
	explicit CSoundManager();
public:
	~CSoundManager();
public:
	void Set_VolumeUP(Sound_Type Type, string& SoundName, double Volume);
	void Set_VolumeDown(Sound_Type Type, string& SoundName, double Volume);
public:	
	void Initialize(int MaxChannel);
	void Update();
	void SoundInsert(FMOD::Channel* Channel, Sound_Type Type, string& SoundName);
	void SoundPlay(Sound_Type Type, string & SoundName);
	void SoundStop(Sound_Type Type, string& SoundName);
	void SoundAllStop();
private:
	//bool Find
	bool FindSound(string & SoundName);
	void Release();
private:
	unordered_map<string, FMOD::Sound*>				_UnMapSound;
	typedef unordered_map<string, FMOD::Sound*>		UNMAPSOUND;
	unordered_map<string, FMOD::Channel*>			_UnMapChannel[SOUND_END];
	typedef unordered_map<string, FMOD::Channel*>	UNMAPCHANNEL;
	FMOD::System*									_SoundSystem;
};

END

#endif // !SOUNDMANAGER_H__