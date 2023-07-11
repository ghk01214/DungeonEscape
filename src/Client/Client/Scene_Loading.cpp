#include "pch.h"
#include "Scene_Loading.h"
#include "Loader.h"

#include "Scene_GamePlay.h"
#include "Scene_Test.h"
#include "GameInstance.h"

Scene_Loading::Scene_Loading()
{
}

void Scene_Loading::Loading(SCENE eNextScene)
{
	m_eNextScene = eNextScene;

	m_pLoader = CLoader::Create(eNextScene);

	if (nullptr == m_pLoader)
	{
		MSG_BOX(L"Failed to load Scene_Loading::Loading");
		return;
	}
}

void Scene_Loading::Awake()
{
	__super::Awake();
}

void Scene_Loading::Start()
{
	__super::Start();
}

void Scene_Loading::Update()
{
	__super::Update();

	// 특정 키를 누르면 넘어가도록 설정

	// 로더의 로딩이 완료되었을 경우
	if (m_pLoader->Get_Finished())
	{
		shared_ptr<CScene> pScene = nullptr;

		switch (m_eNextScene)
		{
			case SCENE_GAMEPLAY:
			//pScene = Scene_GamePlay::Create();
			pScene = Scene_Test::Create(server::FBX_TYPE::MISTIC);
			break;
			/*case LEVEL_BOSS:
			break;*/
		}

		// 기존 씬이 존재할 경우, 해당 씬의 정보를 삭제하고 새로운 정보를 출력하는 함수
		GGameInstance->LoadScene(pScene);
	}

}

void Scene_Loading::LateUpdate()
{
	__super::LateUpdate();
}

void Scene_Loading::FinalUpdate()
{
	__super::FinalUpdate();
}

void Scene_Loading::Render()
{
	__super::Render();
}

shared_ptr<CScene> Scene_Loading::Create(SCENE eNextScene)
{
	shared_ptr<Scene_Loading> pScene = std::make_shared<Scene_Loading>();

	pScene->Loading(eNextScene);

	return pScene;
}
