#include "stdafx.h"
#include "SoundManager.h"

USING(Engine)

IMPLEMENT_SINGLETON(CSoundManager)

CSoundManager::CSoundManager()
{
}

CSoundManager::~CSoundManager()
{
	Release();
}

void CSoundManager::Initialize(int MaxChannel)
{
	FMOD_RESULT SystemResult = FMOD::System_Create(&_SoundSystem);

	if (FMOD_OK != SystemResult)
	{
		CCrashDump::Crash();
	}

	SystemResult = _SoundSystem->init(MaxChannel, FMOD_INIT_NORMAL, NULL);

	if (FMOD_OK != SystemResult)
	{
		CCrashDump::Crash();
	}
}

void CSoundManager::Update()
{
	_SoundSystem->update();
}

void CSoundManager::SoundInsert(FMOD::Channel * Channel, Sound_Type Type, string & SoundName)
{
}

void CSoundManager::SoundPlay(Sound_Type Type, string & SoundName)
{
	if (true == FindSound(SoundName))
	{
		_UnMapChannel[Type][SoundName]->setPaused(false);
	}
}

void CSoundManager::SoundStop(Sound_Type Type, string & SoundName)
{
	if (true == FindSound(SoundName))
	{
		_UnMapChannel[Type][SoundName]->setPaused(true);
	}	
}

void CSoundManager::SoundAllStop()
{
	for (int Index = 0; Index < SOUND_END; ++Index)
	{
		UNMAPCHANNEL::iterator	Iter_begin = _UnMapChannel[Index].begin();
		UNMAPCHANNEL::iterator	Iter_end = _UnMapChannel[Index].end();

		for (; Iter_begin != Iter_end;)
		{
			Iter_begin->second->setPaused(true);
		}
	}
}

bool CSoundManager::FindSound(string & SoundName)
{
	bool Result = true;

	if (_UnMapSound.end() == _UnMapSound.find(SoundName))
	{
		Result = false;
	}

	return Result;
}

void CSoundManager::Release()
{
	UNMAPSOUND::iterator	Iter_begin = _UnMapSound.begin();
	UNMAPSOUND::iterator	Iter_end = _UnMapSound.end();

	for (; Iter_begin != Iter_end;)
	{
		Iter_begin->second->release();
	}

	_SoundSystem->close();
	_SoundSystem->release();
}