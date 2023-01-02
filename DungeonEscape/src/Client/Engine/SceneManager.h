#pragma once

class CScene;

class CSceneManager
{
	DECLARE_SINGLE(CSceneManager);

public:
	void Update();
	void LoadScene(std::wstring sceneName);

public:
	std::shared_ptr<CScene> GetActiveScene() { return m_activeScene; }

private:
	std::shared_ptr<CScene> LoadTestScene();

private:
	std::shared_ptr<CScene> m_activeScene;	// 현재 작동하고 있는 Scene정보
};

