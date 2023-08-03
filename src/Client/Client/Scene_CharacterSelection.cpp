#include "pch.h"
#include "CharacterSelection_Button.h"
#include "Scene_CharacterSelection.h"

#include <Camera.h>
#include <Transform.h>
#include <MeshRenderer.h>
#include <Resources.h>
#include <Engine.h>

#include "Scene_Loading.h"

Scene_CharacterSelection::Scene_CharacterSelection() :
	m_selected{ false }
{
}

void Scene_CharacterSelection::Awake()
{
	__super::Awake();
}

void Scene_CharacterSelection::Start()
{
	__super::Start();
}

void Scene_CharacterSelection::Update()
{
	__super::Update();

	if (m_selected == false)
	{
		for (auto& button : m_buttons)
		{
			if (button->GetBlinkCount() > 6)
			{
				m_selected = true;
				break;
			}
		}
	}
	else
	{
		for (auto& button : m_buttons)
		{
			button->SetBlinkCount(7);
		}
	}
}

void Scene_CharacterSelection::LateUpdate()
{
	__super::LateUpdate();
}

void Scene_CharacterSelection::FinalUpdate()
{
	__super::FinalUpdate();
}

void Scene_CharacterSelection::Render()
{
	__super::Render();
}

void Scene_CharacterSelection::Init()
{
	LoadTextures();

	CreateLayer();
	CreateUICamera();
	CreateUI();
	CreateLights();
}

void Scene_CharacterSelection::CreateLayer()
{
	GGameInstance->SetLayerName(0, L"UI");
}

void Scene_CharacterSelection::CreateUICamera()
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

void Scene_CharacterSelection::CreateUI()
{
	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);

	float xOrigin{ -(width / 2.f) };

	for (int32_t i = 0; i < 3; ++i)
	{
		std::wstring name{};
		float xPos{ 0.f };

		if (i == 0)
			name = L"Nana";
		else if (i == 1)
			name = L"Mistic";
		else
			name = L"Carmel";

		xPos = xOrigin + (width / 3.f) * i + (width / 6.f);

		std::shared_ptr<CGameObject> obj = std::make_shared<CGameObject>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		obj->AddComponent(std::make_shared<Transform>());

		obj->GetTransform()->SetLocalScale(Vec3(width / 3.f, height, 1.f));
		obj->GetTransform()->SetLocalPosition(Vec3(xPos, 0.f, 1.f));

		std::shared_ptr<MeshRenderer> meshRenderer = std::make_shared<MeshRenderer>();
		{
			std::shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}

		{
			std::shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Logo_texture");
			std::shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(name);

			std::shared_ptr<Material> material = std::make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			material->SetFloat(2, 1.f);
			meshRenderer->SetMaterial(material);
		}

		obj->AddComponent(meshRenderer);

		std::shared_ptr<CharacterSelection_Button> behaviour = std::make_shared<CharacterSelection_Button>();
		behaviour->InsertTextures(GET_SINGLE(Resources)->Get<Texture>(name));
		behaviour->InsertTextures(GET_SINGLE(Resources)->Get<Texture>(name + L"_Selected"));
		behaviour->SetName(name);
		m_buttons.push_back(behaviour);

		obj->AddComponent(behaviour);

		AddGameObject(obj);
	}
}

void Scene_CharacterSelection::LoadTextures()
{
	GET_SINGLE(Resources)->Load<Texture>(L"Nana", L"..\\Resources\\Texture\\Character Select\\Nana.png");
	GET_SINGLE(Resources)->Load<Texture>(L"Mistic", L"..\\Resources\\Texture\\Character Select\\Mistic.png");
	GET_SINGLE(Resources)->Load<Texture>(L"Carmel", L"..\\Resources\\Texture\\Character Select\\Carmel.png");

	GET_SINGLE(Resources)->Load<Texture>(L"Nana_Selected", L"..\\Resources\\Texture\\Character Select\\Nana_Selected.png");
	GET_SINGLE(Resources)->Load<Texture>(L"Mistic_Selected", L"..\\Resources\\Texture\\Character Select\\Mistic_Selected.png");
	GET_SINGLE(Resources)->Load<Texture>(L"Carmel_Selected", L"..\\Resources\\Texture\\Character Select\\Carmel_Selected.png");
}

void Scene_CharacterSelection::CreateLights()
{
	LightDesc lightDesc;
	lightDesc.vDirection = Vec3(0.f, -1.f, 0.5f);
	lightDesc.vDiffuse = Vec3(1.f, 1.f, 1.f);
	lightDesc.vAmbient = Vec3(0.9f, 0.9f, 0.9f);
	lightDesc.vSpecular = Vec3(0.1f, 0.1f, 0.1f);

	AddDirectionalLight(lightDesc);
}

shared_ptr<CScene> Scene_CharacterSelection::Create()
{
	std::shared_ptr<Scene_CharacterSelection> pInstance{ std::make_shared<Scene_CharacterSelection>() };

	pInstance->Init();

	return pInstance;
}
