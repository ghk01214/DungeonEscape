#include "pch.h"
#include "Scene_Lobby.h"

#include "GameInstance.h"
#include "Scene_Loading.h"

#include "Camera.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Resources.h"
#include "Engine.h"

#include "Lobby_Script.h"
#include "Lobby_StartButton.h"

Scene_Lobby::Scene_Lobby()
{
}

void Scene_Lobby::Awake()
{
	__super::Awake();
}

void Scene_Lobby::Start()
{
	__super::Start();
}

void Scene_Lobby::Update()
{
	__super::Update();

	/*if (GET_SINGLE(CInput)->GetButtonDown(KEY_TYPE::SPACE))
	{
		GET_SINGLE(SceneManager)->LoadScene(Scene_Loading::Create(SCENE_GAMEPLAY));
	}*/
}

void Scene_Lobby::LateUpdate()
{
	__super::LateUpdate();
}

void Scene_Lobby::FinalUpdate()
{
	__super::FinalUpdate();
}

void Scene_Lobby::Render()
{
	__super::Render();
}

void Scene_Lobby::Init(void)
{
	LoadTextures();

	CreateLayer();
	CreateUICamera();
	CreateUI();
	CreateLights();
}

void Scene_Lobby::CreateLayer(void)
{
	GGameInstance->SetLayerName(0, L"UI");
}

void Scene_Lobby::CreateUICamera(void)
{
	std::shared_ptr<CGameObject> camera = std::make_shared<CGameObject>();
	camera->SetName(L"Orthographic_Camera");

	camera->AddComponent(make_shared<Transform>());
	camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));

	camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, 800*600
	camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);

	uint8_t layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");

	camera->GetCamera()->SetCullingMaskAll(); // 다 끄고
	camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI만 찍음

	AddGameObject(camera);
}

void Scene_Lobby::CreateUI(void)
{
	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);
	std::vector<std::shared_ptr<Texture>> textures;

	// BACKGROUND
	{
		std::shared_ptr<CGameObject> obj = std::make_shared<CGameObject>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		obj->AddComponent(std::make_shared<Transform>());

		obj->GetTransform()->SetLocalScale(Vec3(width, height, 1.f));
		obj->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 500.f));

		std::shared_ptr<MeshRenderer> meshRenderer = std::make_shared<MeshRenderer>();
		{
			std::shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}

		{
			std::shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Logo_texture");
			std::shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(L"Lobby_InGame");

			std::shared_ptr<Material> material = std::make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}

		obj->AddComponent(meshRenderer);

		std::shared_ptr<Lobby_Script> behaviour = std::make_shared<Lobby_Script>();
		behaviour->InsertTextures(GET_SINGLE(Resources)->Get<Texture>(L"Lobby_InGame"));

		obj->AddComponent(behaviour);

		AddGameObject(obj);
	}

	// START BUTTON
	{
		std::shared_ptr<CGameObject> obj = std::make_shared<CGameObject>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		obj->AddComponent(std::make_shared<Transform>());

		float yPos{ -(height / 4) };

		obj->GetTransform()->SetLocalScale(Vec3(281.f, 110.f, 1.f));
		obj->GetTransform()->SetLocalPosition(Vec3(0.f, yPos, 1.f));

		std::shared_ptr<MeshRenderer> meshRenderer = std::make_shared<MeshRenderer>();
		{
			std::shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}

		{
			std::shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Logo_texture");
			std::shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(L"NewStart");

			std::shared_ptr<Material> material = std::make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}

		obj->AddComponent(meshRenderer);

		std::shared_ptr<Lobby_StartButton> behaviour = std::make_shared<Lobby_StartButton>();
		behaviour->InsertTextures(GET_SINGLE(Resources)->Get<Texture>(L"NewStart"));
		behaviour->InsertTextures(GET_SINGLE(Resources)->Get<Texture>(L"NewStart_Pressed"));

		obj->AddComponent(behaviour);

		AddGameObject(obj);
	}
}

void Scene_Lobby::LoadTextures(void)
{
	GET_SINGLE(Resources)->Load<Texture>(L"Lobby_InGame", L"..\\Resources\\Texture\\Lobby\\Lobby_InGame.PNG");
	GET_SINGLE(Resources)->Load<Texture>(L"NewStart", L"..\\Resources\\Texture\\Lobby\\NewStart.png");
	GET_SINGLE(Resources)->Load<Texture>(L"NewStart_Pressed", L"..\\Resources\\Texture\\Lobby\\NewStart_Pressed.png");
}

void Scene_Lobby::CreateLights(void)
{
	LightDesc lightDesc;
	lightDesc.vDirection = Vec3(0.f, -1.f, 0.5f);
	lightDesc.vDiffuse = Vec3(1.f, 1.f, 1.f);
	lightDesc.vAmbient = Vec3(0.9f, 0.9f, 0.9f);
	lightDesc.vSpecular = Vec3(0.1f, 0.1f, 0.1f);

	AddDirectionalLight(lightDesc);
}

shared_ptr<CScene> Scene_Lobby::Create()
{
	std::shared_ptr<Scene_Lobby> pInstance = std::make_shared<Scene_Lobby>();

	pInstance->Init();

	return pInstance;
}
