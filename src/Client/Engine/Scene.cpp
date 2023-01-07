#include "pch.h"
#include "Scene.h"
#include "GameObject.h"

void CScene::Awake()
{
	for (const std::shared_ptr<CGameObject>& gameObject : m_gameObjects)
	{
		gameObject->Awake();
	}
}

void CScene::Start()
{
	for (const std::shared_ptr<CGameObject>& gameObject : m_gameObjects)
	{
		gameObject->Start();
	}
}

void CScene::Update()
{
	for (const std::shared_ptr<CGameObject>& gameObject : m_gameObjects)
	{
		gameObject->Update();
	}
}

void CScene::LateUpdate()
{
	for (const std::shared_ptr<CGameObject>& gameObject : m_gameObjects)
	{
		gameObject->LateUpdate();
	}
}

void CScene::FinalUpdate()
{
	for (const std::shared_ptr<CGameObject>& gameObject : m_gameObjects)
	{
		gameObject->FinalUpdate();
	}
}

void CScene::AddGameObject(std::shared_ptr<CGameObject> gameObject)
{
	m_gameObjects.push_back(gameObject);
}

void CScene::RemoveGameObject(std::shared_ptr<CGameObject> gameObject)
{
	auto findIt = std::find(m_gameObjects.begin(), m_gameObjects.end(), gameObject);
	if (findIt != m_gameObjects.end())
	{
		m_gameObjects.erase(findIt);
	}
}