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

	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}

int CSoundMgr::VolumeUp(CHANNELID eID, float _vol)
{
	if (m_volume < SOUND_MAX) {
		m_volume += _vol;
	}

	FMOD_Channel_SetVolume(m_pChannelArr[eID], m_volume);

	return 0;
}

int CSoundMgr::VolumeDown(CHANNELID eID, float _vol)
{
	if (m_volume > SOUND_MIN) {
		m_volume -= _vol;
	}

	FMOD_Channel_SetVolume(m_pChannelArr[eID], m_volume);

	return 0;
}

int CSoundMgr::BGMVolumeUp(float _vol)
{
	if (m_BGMvolume < SOUND_MAX) {
		m_BGMvolume += _vol;
	}

	FMOD_Channel_SetVolume(m_pChannelArr[BGM], m_BGMvolume);

	return 0;
}

int CSoundMgr::BGMVolumeDown(float _vol)
{
	if (m_BGMvolume > SOUND_MIN) {
		m_BGMvolume -= _vol;
	}

	FMOD_Channel_SetVolume(m_pChannelArr[BGM], m_BGMvolume);

	return 0;
}

int CSoundMgr::Pause(CHANNELID eID)
{
	m_bPause = !m_bPause;
	FMOD_Channel_SetPaused(m_pChannelArr[eID], m_bPause);

	return 0;
}

void CSoundMgr::PlaySound(const std::wstring& pSoundKey, CHANNELID eID, float _vol)
{
	map<const std::wstring, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		return !lstrcmp(pSoundKey.c_str(), iter.first.c_str());
	});

	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;
	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, nullptr,FALSE, &m_pChannelArr[eID]);
		if (_vol >= SOUND_MAX)
			_vol = 1.f;
		else if (_vol <= SOUND_MIN)
			_vol = 0.f;
		FMOD_Channel_SetVolume(m_pChannelArr[eID], _vol);
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

	FMOD_System_PlaySound(m_pSystem,iter->second,nullptr, FALSE, &m_pChannelArr[BGM]);
	FMOD_Channel_SetMode(m_pChannelArr[BGM], FMOD_LOOP_NORMAL);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::StopSound(CHANNELID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSoundMgr::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
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
