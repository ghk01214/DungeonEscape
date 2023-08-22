#include "pch.h"
#include "CharacterSelection_Button.h"
#include "Scene_CharacterSelection.h"

#include <Camera.h>
#include <Transform.h>
#include <MeshRenderer.h>
#include <Resources.h>
#include <Engine.h>
#include <UI.h>

#include "Scene_Loading.h"

#include "Creator.h"

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
	CreateCharacterImage();
	CreateCharacterNameButton();
	CreateReadyButton();
	CreateSampleUI();
}

void Scene_CharacterSelection::LoadTextures()
{
	GET_SINGLE(Resources)->Load<Texture>(L"Frame1", L"..\\Resources\\Texture\\UI\\Frame\\Frame1.png");
	GET_SINGLE(Resources)->Load<Texture>(L"Frame1_selected", L"..\\Resources\\Texture\\UI\\Frame\\Frame1_selected.png");

	GET_SINGLE(Resources)->Load<Texture>(L"Button2", L"..\\Resources\\Texture\\UI\\Button\\Button2.png");
	GET_SINGLE(Resources)->Load<Texture>(L"Button2_selected", L"..\\Resources\\Texture\\UI\\Button\\Button2_selected.png");

	GET_SINGLE(Resources)->Load<Texture>(L"Knight", L"..\\Resources\\Texture\\UI\\Font\\Knight.png");
	GET_SINGLE(Resources)->Load<Texture>(L"Mage", L"..\\Resources\\Texture\\UI\\Font\\Mage.png");
	GET_SINGLE(Resources)->Load<Texture>(L"Priest", L"..\\Resources\\Texture\\UI\\Font\\Priest.png");

	GET_SINGLE(Resources)->Load<Texture>(L"Nana", L"..\\Resources\\Texture\\UI\\Character Select\\Nana.png");
	GET_SINGLE(Resources)->Load<Texture>(L"Mistic", L"..\\Resources\\Texture\\UI\\Character Select\\Mistic.png");
	GET_SINGLE(Resources)->Load<Texture>(L"Carmel", L"..\\Resources\\Texture\\UI\\Character Select\\Carmel.png");
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

void Scene_CharacterSelection::CreateCharacterImage()
{
	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);
	float xOrigin{ -(width / 2.f) };

	// FRAME
	{
		for (int32_t i = 0; i < 3; ++i)
		{
			std::wstring name{ L"Frame1" };
			Vec2 pos{};
			pos.x = xOrigin + (width / 3.f) * i + (width / 6.f);
			pos.y = GetRatio(0.f, 15.f).y;

			std::shared_ptr<CGameObject> obj = std::make_shared<CGameObject>();
			obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
			obj->AddComponent(std::make_shared<Transform>());

			obj->GetTransform()->SetLocalScale(Vec3(width / 4.f, height / 1.5f, 1.f));
			obj->GetTransform()->SetLocalPosition(Vec3(pos.x, pos.y, 100.f));

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

			std::shared_ptr<CharacterSelection_Button> behaviour = std::make_shared<CharacterSelection_Button>(BUTTON_TYPE::CHARACTER_SELECT);
			behaviour->InsertTextures(GET_SINGLE(Resources)->Get<Texture>(name));
			behaviour->InsertTextures(GET_SINGLE(Resources)->Get<Texture>(name + L"_selected"));

			if (i == 0)
				name = L"Nana";
			else if (i == 1)
				name = L"Mistic";
			else
				name = L"Carmel";

			behaviour->SetName(name);
			m_buttons.push_back(behaviour);

			obj->AddComponent(behaviour);

			AddGameObject(obj);
		}
	}

	// IMAGE
	{
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

			Vec2 pos{};
			pos.x = xOrigin + (width / 3.f) * i + (width / 6.f);
			pos.y = GetRatio(0.f, 15.f).y;

			std::shared_ptr<CGameObject> obj = std::make_shared<CGameObject>();
			obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
			obj->AddComponent(std::make_shared<Transform>());

			obj->GetTransform()->SetLocalScale(Vec3(width / 4.5f, height / 1.7f, 1.f));
			obj->GetTransform()->SetLocalPosition(Vec3(pos.x, pos.y, 90.f));

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

			//std::shared_ptr<CharacterSelection_Button> behaviour = std::make_shared<CharacterSelection_Button>(BUTTON_TYPE::CHARACTER_SELECT);
			//behaviour->InsertTextures(GET_SINGLE(Resources)->Get<Texture>(name));
			////behaviour->InsertTextures(GET_SINGLE(Resources)->Get<Texture>(name + L"_Selected"));
			//behaviour->SetName(name);
			//m_buttons.push_back(behaviour);
			//
			//obj->AddComponent(behaviour);

			AddGameObject(obj);
		}
	}
}

void Scene_CharacterSelection::CreateCharacterNameButton()
{
	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);
	float xOrigin{ -(width / 2.f) };

	// FRAME
	{
		for (int32_t i = 0; i < 3; ++i)
		{
			std::wstring name{ L"Button2" };
			Vec2 pos{ GetRatio(0.f, -65.f) };
			pos.x = xOrigin + (width / 3.f) * i + (width / 6.f);

			std::shared_ptr<CGameObject> obj = std::make_shared<CGameObject>();
			obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
			obj->AddComponent(std::make_shared<Transform>());

			obj->GetTransform()->SetLocalScale(Vec3(257.f * 1.2f, 95.f, 1.f));
			obj->GetTransform()->SetLocalPosition(Vec3(pos.x, pos.y, 100.f));

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

			std::shared_ptr<CharacterSelection_Button> behaviour = std::make_shared<CharacterSelection_Button>(BUTTON_TYPE::CHARACTER_SELECT);
			behaviour->InsertTextures(GET_SINGLE(Resources)->Get<Texture>(name));
			behaviour->InsertTextures(GET_SINGLE(Resources)->Get<Texture>(name + L"_selected"));
			behaviour->SetName(name);
			m_buttons.push_back(behaviour);

			obj->AddComponent(behaviour);

			AddGameObject(obj);
		}
	}

	// FONT
	{
		for (int32_t i = 0; i < 3; ++i)
		{
			std::wstring name{};

			if (i == 0)
				name = L"Knight";
			else if (i == 1)
				name = L"Mage";
			else
				name = L"Priest";

			Vec2 pos{ GetRatio(0.f, -65.f) };
			pos.x = xOrigin + (width / 3.f) * i + (width / 6.f) + 5.f;

			std::shared_ptr<CGameObject> obj = std::make_shared<CGameObject>();
			obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
			obj->AddComponent(std::make_shared<Transform>());

			obj->GetTransform()->SetLocalScale(Vec3(250.f, 58.f, 1.f));
			obj->GetTransform()->SetLocalPosition(Vec3(pos.x, pos.y, 90.f));

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

			AddGameObject(obj);
		}
	}
}

void Scene_CharacterSelection::CreateReadyButton()
{
}

void Scene_CharacterSelection::CreateSampleUI()
{
	std::wstring name{ L"Button2" };
	std::shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Logo_texture");
	std::shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(name);

	{
		Vec2 pos{ GetRatio(-10.f, 0.f) };

		std::shared_ptr<CGameObject> obj = std::make_shared<CGameObject>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		obj->AddComponent(std::make_shared<Transform>());

		obj->GetTransform()->SetLocalScale(Vec3(200.f, 200.f, 1.f));
		obj->GetTransform()->SetLocalPosition(Vec3(pos.x, pos.y, 1.f));

		std::shared_ptr<MeshRenderer> meshRenderer = std::make_shared<MeshRenderer>();
		{
			std::shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			std::shared_ptr<Material> material = std::make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			material->SetFloat(2, 1.f);
			meshRenderer->SetMaterial(material);
		}
		obj->AddComponent(meshRenderer);

		std::shared_ptr<UI> ui = std::make_shared<UI>();
		{
			ui->SetVisible(true);
			ui->OnTop();
		}
		obj->AddComponent(ui);

		AddGameObject(obj);
	}


	texture = GET_SINGLE(Resources)->Get<Texture>(L"Title");
	{
		Vec2 pos{ GetRatio(0.f, 0.f) };

		std::shared_ptr<CGameObject> obj = std::make_shared<CGameObject>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		obj->AddComponent(std::make_shared<Transform>());

		obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 1.f));
		obj->GetTransform()->SetLocalPosition(Vec3(pos.x, pos.y, 1.f));

		std::shared_ptr<MeshRenderer> meshRenderer = std::make_shared<MeshRenderer>();
		{
			std::shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			std::shared_ptr<Material> material = std::make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			material->SetFloat(2, 0.5f);
			meshRenderer->SetMaterial(material);
		}
		obj->AddComponent(meshRenderer);

		std::shared_ptr<UI> ui = std::make_shared<UI>();
		{
			ui->SetVisible(true);
			//ui->OnTop();
			ui->SetPopUp(true);
		}
		obj->AddComponent(ui);

		AddGameObject(obj);
	}


	{
		// 사용할 텍스쳐와 셰이더만 인자로 넘겨 UI 객체 생성
		shared_ptr<CGameObject> obj = Creator::CreateUIObject(texture, shader);

		// 레이어 설정
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI

		// 위치 및 카메라 인덱스 설정
		Vec2 pos{ GetRatio(30.f, 30.f) };
		obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 1.f));
		obj->GetTransform()->SetLocalPosition(Vec3(pos.x, pos.y, 1.f));

		obj->GetMeshRenderer()->GetMaterial()->SetFloat(2, 1.f);

		AddGameObject(obj);
	}

	{
		texture = GET_SINGLE(Resources)->GetBlurTexture();

		Vec2 pos{ GetRatio(0.f, 0.f) };

		std::shared_ptr<CGameObject> obj = std::make_shared<CGameObject>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		obj->AddComponent(std::make_shared<Transform>());

		const auto& windowInfo = GEngine->GetWindow();
		obj->GetTransform()->SetLocalScale(Vec3(windowInfo.width, windowInfo.height, 1.f));
		obj->GetTransform()->SetLocalPosition(Vec3(pos.x, pos.y, 1.f));

		std::shared_ptr<MeshRenderer> meshRenderer = std::make_shared<MeshRenderer>();
		{
			std::shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			std::shared_ptr<Material> material = std::make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			material->SetFloat(2, 1.f);
			meshRenderer->SetMaterial(material);
		}
		obj->AddComponent(meshRenderer);

		std::shared_ptr<UI> ui = std::make_shared<UI>();
		{
			ui->SetPopUpUI(true);
		}
		obj->AddComponent(ui);

		AddGameObject(obj);

		GET_SINGLE(SceneManager)->SetBlurUI(obj);
		GET_SINGLE(SceneManager)->SetUIIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
	}
}

shared_ptr<CScene> Scene_CharacterSelection::Create()
{
	std::shared_ptr<Scene_CharacterSelection> pInstance{ std::make_shared<Scene_CharacterSelection>() };

	pInstance->Init();

	return pInstance;
}
