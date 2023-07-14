#include "pch.h"
#include "Scene_GamePlay.h"

Scene_GamePlay::Scene_GamePlay()
{
}

void Scene_GamePlay::Awake()
{
	__super::Awake();
}

void Scene_GamePlay::Start()
{
	__super::Start();
}

void Scene_GamePlay::Update()
{
	__super::Update();
}

void Scene_GamePlay::LateUpdate()
{
	__super::LateUpdate();
}

void Scene_GamePlay::FinalUpdate()
{
	__super::FinalUpdate();
}

void Scene_GamePlay::Render()
{
	__super::Render();
}

shared_ptr<CScene> Scene_GamePlay::Create()
{
	shared_ptr<CScene> pInstance = std::make_shared<Scene_GamePlay>();

	pInstance->Awake();

	return pInstance;
}