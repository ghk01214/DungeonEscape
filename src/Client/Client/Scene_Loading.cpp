#include "pch.h"
#include "Scene_Loading.h"

#include <Camera.h>
#include <Transform.h>
#include <MeshRenderer.h>
#include <Resources.h>
#include <Engine.h>
#include <NetworkManager.h>
#include <SoundManager.h>

#include "Scenes.hpp"
#include "Loader.h"
#include "Loading_Script.h"

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

void Scene_Loading::LogIn()
{
	std::wstring SERVER_ADDR{};
	std::wstring ID{};
	std::wstring PWD{};
	//std::wcout << L"IP ADDRESS : ";
	//std::wcin >> SERVER_ADDR;
	//std::wcout << L"ID : ";
	//std::wcin >> ID;
	//std::wcout << L"PASSWORD : ";
	//std::wcin >> PWD;
	SERVER_ADDR = L"127.0.0.1";

	GET_NETWORK->Init(serverIP_Address);
	GET_NETWORK->SendLoginPacket(ID, PWD);
}

void Scene_Loading::SetSelectedCharacter(server::FBX_TYPE character)
{
	m_character = character;
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
			{
				//pScene = Scene_GamePlay::Create();
				LogIn();

				pScene = Scene_Test::Create(m_character);
			}
			break;
			case SCENE_LOBBY:
			{
				pScene = Scene_Start::Create();
			}
			break;
			case SCENE_CHARACTER_SELECT:
			{
				pScene = Scene_CharacterSelection::Create();
			}
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

void Scene_Loading::Init()
{
	LoadTextures();

	CreateLayer();
	CreateUICamera();
	CreateUI();
	CreateLights();
}

void Scene_Loading::CreateLayer()
{
	GGameInstance->SetLayerName(0, L"UI");
}

void Scene_Loading::CreateUICamera()
{
	std::shared_ptr<CGameObject> camera = std::make_shared<CGameObject>();
	camera->SetName(L"Orthographic_Camera");

	camera->AddComponent(std::make_shared<Transform>());
	camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));

	camera->AddComponent(std::make_shared<Camera>()); // Near=1, Far=1000, 800*600
	camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);

	uint8_t layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");

	camera->GetCamera()->SetCullingMaskAll(); // 다 끄고
	camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI만 찍음

	AddGameObject(camera);
}

void Scene_Loading::CreateUI()
{
	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);

	std::shared_ptr<CGameObject> obj = std::make_shared<CGameObject>();
	obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
	obj->AddComponent(std::make_shared<Transform>());

	obj->GetTransform()->SetLocalScale(Vec3(387.f, 55.f, 1.f));
	obj->GetTransform()->SetLocalPosition(Vec3(width / 4.f, -(height / 4.f), 1.f));

	std::shared_ptr<MeshRenderer> meshRenderer = std::make_shared<MeshRenderer>();
	{
		std::shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
		meshRenderer->SetMesh(mesh);
	}

	{
		std::shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Logo_texture");
		std::shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(L"Loading0");

		std::shared_ptr<Material> material = std::make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		material->SetFloat(2, 1.f);
		meshRenderer->SetMaterial(material);
	}

	obj->AddComponent(meshRenderer);
	std::shared_ptr<Loading_Script> behaviour = std::make_shared<Loading_Script>();

	for (int32_t i = 0; i < 10; ++i)
	{
		std::wstring name{ L"Loading" + std::to_wstring(i) };

		behaviour->InsertTextures(GET_SINGLE(Resources)->Get<Texture>(name));
		behaviour->SetName(name);
	}

	obj->AddComponent(behaviour);

	AddGameObject(obj);
}

void Scene_Loading::LoadTextures()
{
	for (int32_t i = 0; i < 10; ++i)
	{
		std::wstring name{ L"Loading" };
		std::wstring fileName{ L"bar" + std::to_wstring(i) + L".png" };

		GET_SINGLE(Resources)->Load<Texture>(name, L"..\\Resources\\Texture\\Loading\\" + fileName);
	}
}

void Scene_Loading::CreateLights()
{
	LightDesc lightDesc;
	lightDesc.vDirection = Vec3(0.f, -1.f, 0.5f);
	lightDesc.vDiffuse = Vec3(1.f, 1.f, 1.f);
	lightDesc.vAmbient = Vec3(0.9f, 0.9f, 0.9f);
	lightDesc.vSpecular = Vec3(0.1f, 0.1f, 0.1f);

	AddDirectionalLight(lightDesc);
}

shared_ptr<CScene> Scene_Loading::Create(SCENE eNextScene)
{
	shared_ptr<Scene_Loading> pScene = std::make_shared<Scene_Loading>();

	//pScene->Init();
	pScene->Loading(eNextScene);

	return pScene;
}
