#pragma once

#include "FMOD/fmod.h"

#define SOUND_MAX 1.0f
#define SOUND_MIN 0.0f
#define SOUND_DEFAULT 0.5f
#define SOUND_WEIGHT 0.1f

#define MAX_EFFECT_CHANNEL 100

class CSoundMgr
{
	DECLARE_SINGLE(CSoundMgr);

//public:
//	enum CHANNELID { BGM, PLAYER, EFFECT, UI, SYSTEM_EFFECT, SYSTEM_EFFECT2, SYSTEM_EFFECT3, MAXCHANNEL };

public:
	void Init();
	void Release();

public:
	int  EffectVolumeUp(float _vol);
	int  EffectVolumeDown(float _vol);
	int  SetEffectVolume(float _vol);

	int  BGMVolumeUp(float _vol);
	int  BGMVolumeDown(float _vol);
	int  SetBGMVolume(float _vol);

	// 잠시 멈추는 기능, 다시 불러주면 시작함
	int  PauseEffect(void);
	int  PauseBGM(void);
	int  PauseAll(void);

	void PlayEffect(const std::wstring& pSoundKey);
	void PlayBGM(const std::wstring& pSoundKey);

	// 완전히 멈추는 기능, 정보가 사라짐
	void StopEffectSound(void);
	void StopBGMSound(void);
	void StopAll();

public:
	constexpr float GetPrevBGMVolume() const { return m_prevBGMvolume; }
	constexpr float GetPrevEffectVolume() const { return m_prevEffectvolume; }

	void MuteBGM(bool flag);
	void MuteSE(bool flag);

private:
	float m_Effectvolume = SOUND_DEFAULT;
	float m_BGMvolume = SOUND_DEFAULT;
	FMOD_BOOL m_bool;

private:
	float m_prevEffectvolume = SOUND_DEFAULT;
	float m_prevBGMvolume = SOUND_DEFAULT;

private:
	void LoadSoundFile();

private:
	map<std::wstring, FMOD_SOUND*> m_mapSound;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체
	FMOD_CHANNEL* m_pChannelBGM;
	FMOD_CHANNEL* m_pChannelEffect[MAX_EFFECT_CHANNEL];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체
	FMOD_SYSTEM* m_pSystem = nullptr;

	bool		m_bPauseBGM = false;
	bool		m_bPauseEffect = false;
};