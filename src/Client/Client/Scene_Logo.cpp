#include "pch.h"
#include "Scene_Logo.h"

#include "GameInstance.h"
#include "Scene_Loading.h"

Scene_Logo::Scene_Logo()
{
}

void Scene_Logo::Awake()
{
	__super::Awake();
}

void Scene_Logo::Start()
{
	__super::Start();
}

void Scene_Logo::Update()
{
	__super::Update();

	if (GET_SINGLE(CInput)->GetButtonDown(KEY_TYPE::SPACE))
	{
		GET_SINGLE(SceneManager)->LoadScene(Scene_Loading::Create(SCENE_GAMEPLAY));
	}
}

void Scene_Logo::LateUpdate()
{
	__super::LateUpdate();
}

void Scene_Logo::FinalUpdate()
{
	__super::FinalUpdate();
}

void Scene_Logo::Render()
{
	__super::Render();
}

shared_ptr<Scene_Logo> Scene_Logo::Create()
{
	shared_ptr<Scene_Logo> pInstance = std::make_shared<Scene_Logo>();

	return pInstance;
}
