#pragma once

class CGameObject;

class CScene
{
public:
	void Awake();
	void Start();
	void Update();
	void LateUpdate();

	void AddGameObject(std::shared_ptr<CGameObject> gameObject);
	void RemoveGameObject(std::shared_ptr<CGameObject> gameObject);

private:
	std::vector<std::shared_ptr<CGameObject>> m_gameObjects;
};

