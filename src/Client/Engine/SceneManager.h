#pragma once

class CScene;
class CGameObject;

class CSceneManager
{
	DECLARE_SINGLE(CSceneManager);

public:
	void Update();
	void Render();
	void LoadScene(std::wstring sceneName);

public:
	std::shared_ptr<CScene> GetActiveScene() { return m_activeScene; }

private:
	std::shared_ptr<CScene> LoadTestScene();

private:
	void CreateObject(std::shared_ptr<CScene> scene, const std::wstring& texturePath, Vec3 vPos, Vec3 vScale);
	void KeyInput(void);

private:
	std::shared_ptr<CScene> m_activeScene;			// 현재 작동하고 있는 Scene정보
	std::shared_ptr<CGameObject> m_targetObject;	// 현재 선택된 오브젝트 정보
	std::vector<std::shared_ptr<CGameObject>> m_gameObjects;	// 현재 선택된 씬에 있는 게임 오브젝트들의 정보
};

