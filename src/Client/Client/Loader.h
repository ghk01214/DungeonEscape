#pragma once


/* 씬별로 로딩해야할 리소스 생성한다.(스레드로)  */
#include "Client_Defines.h"

class CLoader : public std::enable_shared_from_this<CLoader>
{
public:
	explicit CLoader(void);
	virtual ~CLoader() = default;

public:
	LPCRITICAL_SECTION Get_CriticalSection() { return &m_CriticalSection; }
	SCENE Get_SceneIndex() { return m_eScene; }
	constexpr bool Get_Finished() const { return m_isFinished; }
	const std::wstring& Get_LoadingText() const { return m_szLoading; }
	constexpr int32_t GetProgress() const { return m_progress; }

private:
	HRESULT Loading(SCENE eScene);

public:
	HRESULT Loading_ForGamePlayLevel();

	HRESULT Loading_GamePlayLevel_Font();
	HRESULT Loading_GamePlayLevel_Texture();
	HRESULT Loading_GamePlayLevel_Shader();
	HRESULT Loading_GamePlayLevel_Fbx();

	HRESULT Loading_GamePlayLevel_UI();
	HRESULT Loading_GamePlayLevel_Effect();

public:
	HRESULT Loading_ForLobby();
	HRESULT Loading_ForLobby_Font();
	HRESULT Loading_ForLobby_Texture();

public:
	HRESULT Loading_ForCharacterSelection();
	HRESULT Loading_ForCharacterSelection_Font();
	HRESULT Loading_ForCharacterSelection_Texture();

private:
	SCENE			m_eScene = SCENE_END;
	bool			m_isFinished = false;
	wstring			m_szLoading;
	int32_t			m_progress;

private:
	HANDLE				m_hThread = 0;
	CRITICAL_SECTION	m_CriticalSection;

public:
	static shared_ptr<CLoader> Create(SCENE eScene);
};