#pragma once


/* 씬별로 로딩해야할 리소스 생성한다.(스레드로)  */
#include "Client_Defines.h"

class CLoader : public std::enable_shared_from_this<CLoader>
{
public:
	explicit CLoader(void);
	virtual ~CLoader() = default;

public:
	LPCRITICAL_SECTION Get_CriticalSection() {
		return &m_CriticalSection;
	}

	SCENE Get_SceneIndex() {
		return m_eScene;
	}

	bool Get_Finished() const {
		return m_isFinished;
	}

	const std::wstring& Get_LoadingText() const {
		return m_szLoading;
	}

private:
	HRESULT Loading(SCENE eScene);


public:
	HRESULT Loading_ForGamePlayLevel();


private:
	SCENE			m_eScene = SCENE_END;
	bool			m_isFinished = false;
	wstring			m_szLoading;

private:
	HANDLE				m_hThread = 0;
	CRITICAL_SECTION	m_CriticalSection;

public:
	static shared_ptr<CLoader> Create(SCENE eScene);
};