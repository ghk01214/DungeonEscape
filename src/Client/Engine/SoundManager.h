#pragma once

#include "FMOD/fmod.h"

#define SOUND_MAX 1.0f
#define SOUND_MIN 0.0f
#define SOUND_DEFAULT 0.5f
#define SOUND_WEIGHT 0.1f

class CSoundMgr
{
	DECLARE_SINGLE(CSoundMgr);

public:
	enum CHANNELID { BGM, PLAYER, EFFECT,UI,SYSTEM_EFFECT, SYSTEM_EFFECT2, SYSTEM_EFFECT3, MAXCHANNEL };

public:
	void Init();
	void Release();

public:
	int  VolumeUp(CHANNELID eID, float _vol);
	int  VolumeDown(CHANNELID eID, float _vol);
	int  BGMVolumeUp(float _vol);
	int  BGMVolumeDown(float _vol);
	int  Pause(CHANNELID eID);
	void PlaySound(const std::wstring& pSoundKey, CHANNELID eID, float _vol);
	void PlayBGM(const std::wstring& pSoundKey);
	void StopSound(CHANNELID eID);
	void StopAll();

private:
	float m_volume = SOUND_DEFAULT;
	float m_BGMvolume = SOUND_DEFAULT;
	FMOD_BOOL m_bool;

private:
	void LoadSoundFile();

private:
	map<std::wstring, FMOD_SOUND*> m_mapSound;
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];
	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem = nullptr;
	bool		m_bPause = false;
};

