#pragma once

class CGameObject;

class CScene
{
public:
	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FinalUpdate();

	void AddGameObject(std::shared_ptr<CGameObject> gameObject);
	void RemoveGameObject(std::shared_ptr<CGameObject> gameObject);

	const std::vector<std::shared_ptr<CGameObject>>& GetGameObjects() { return m_gameObjects; }	// 모든 게임 오브젝트 정보를 넘겨주는 함수

private:
	std::vector<std::shared_ptr<CGameObject>> m_gameObjects;
};

