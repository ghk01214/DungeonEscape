#include "pch.h"
#include"FMOD/fmod.hpp"
#include"FMOD/fmod_errors.h"
#include "SoundManager.h"

void CSoundMgr::Init()
{
	FMOD_System_Create(&m_pSystem, FMOD_VERSION);

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식)
	FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);

	LoadSoundFile();
}

void CSoundMgr::Release()
{
	for (auto& Mypair : m_mapSound)
	{
		FMOD_Sound_Release(Mypair.second);
	}
	m_mapSound.clear();

	FMOD_System_Close(m_pSystem);
	FMOD_System_Release(m_pSystem);
}

int CSoundMgr::EffectVolumeUp(float _vol)
{
	if (m_Effectvolume < SOUND_MAX) {
		m_Effectvolume += _vol;
	}

	for (auto& sound : m_pChannelEffect)
		FMOD_Channel_SetVolume(sound, m_Effectvolume);

	return 0;
}

int CSoundMgr::EffectVolumeDown(float _vol)
{
	if (m_Effectvolume > SOUND_MIN) {
		m_Effectvolume -= _vol;
	}

	for (auto& sound : m_pChannelEffect)
		FMOD_Channel_SetVolume(sound, m_Effectvolume);

	return 0;
}

int CSoundMgr::SetEffectVolume(float _vol)
{
	m_Effectvolume = _vol;

	if (m_Effectvolume < SOUND_MIN)
		m_Effectvolume = SOUND_MIN;
	
	if (m_Effectvolume > SOUND_MAX)
		m_Effectvolume = SOUND_MAX;

	return 0;
}

int CSoundMgr::BGMVolumeUp(float _vol)
{
	if (m_BGMvolume < SOUND_MAX) {
		m_BGMvolume += _vol;
	}

	FMOD_Channel_SetVolume(m_pChannelBGM, m_BGMvolume);

	return 0;
}

int CSoundMgr::BGMVolumeDown(float _vol)
{
	if (m_BGMvolume > SOUND_MIN) {
		m_BGMvolume -= _vol;
	}

	FMOD_Channel_SetVolume(m_pChannelBGM, m_BGMvolume);

	return 0;
}

int CSoundMgr::SetBGMVolume(float _vol)
{
	m_BGMvolume = _vol;

	if (m_BGMvolume < SOUND_MIN)
		m_BGMvolume = SOUND_MIN;

	if (m_BGMvolume > SOUND_MAX)
		m_BGMvolume = SOUND_MAX;

	return 0;
}

int CSoundMgr::PauseEffect(void)
{
	m_bPauseEffect = !m_bPauseEffect;

	for (auto& sound : m_pChannelEffect)
		FMOD_Channel_SetPaused(sound, m_bPauseEffect);

	return 0;
}

int CSoundMgr::PauseBGM(void)
{
	m_bPauseBGM = !m_bPauseBGM;

	FMOD_Channel_SetPaused(m_pChannelBGM, m_bPauseBGM);

	return 0;
}

int CSoundMgr::PauseAll(void)
{
	PauseEffect();
	PauseBGM();

	return 0;
}

void CSoundMgr::PlayEffect(const std::wstring& pSoundKey)
{
	map<const std::wstring, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		return !lstrcmp(pSoundKey.c_str(), iter.first.c_str());
	});

	if (iter == m_mapSound.end())
		return;

	for (auto& sound : m_pChannelEffect)
	{
		FMOD_BOOL bPlay = FALSE;
		if (FMOD_Channel_IsPlaying(sound, &bPlay))
		{
			FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &sound);

			if (m_Effectvolume >= SOUND_MAX)
				m_Effectvolume = 1.f;
			else if (m_Effectvolume <= SOUND_MIN)
				m_Effectvolume = 0.f;
			FMOD_Channel_SetVolume(sound, m_Effectvolume);
			break;
		}
		else
			continue;
	}

	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::PlayBGM(const std::wstring& pSoundKey)
{
	map<const std::wstring, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		return !lstrcmp(pSoundKey.c_str(), iter.first.c_str());
	});

	if (iter == m_mapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem,iter->second, nullptr, FALSE, &m_pChannelBGM);
	FMOD_Channel_SetMode(m_pChannelBGM, FMOD_LOOP_NORMAL);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::StopEffectSound(void)
{
	for (auto& sound : m_pChannelEffect)
		FMOD_Channel_Stop(sound);
}

void CSoundMgr::StopBGMSound(void)
{
	FMOD_Channel_Stop(m_pChannelBGM);
}

void CSoundMgr::StopAll()
{
	StopEffectSound();
	StopBGMSound();
}

void CSoundMgr::LoadSoundFile()
{
	std::string strPath = "..\\Resources\\Sound\\";
	std::string fullPath{};
	std::string fileName{};
	for (const auto& file : filesystem::directory_iterator(strPath))
	{
		fullPath = file.path().generic_string();
		fileName = file.path().filename().generic_string();

		FMOD_SOUND* pSound = nullptr;

		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, fullPath.c_str(), FMOD_DEFAULT, 0, &pSound);
		if (eRes == FMOD_OK)
		{
			int iLength = fileName.length() + 1;

			wstring strSoundKey{ fileName.begin(), fileName.end() };
			m_mapSound.emplace(strSoundKey, pSound);
		}
	}

	FMOD_System_Update(m_pSystem);
}
