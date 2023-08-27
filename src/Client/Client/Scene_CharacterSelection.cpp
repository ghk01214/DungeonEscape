#include "pch.h"
#include "CloseButton_Script.h"
#include "CharacterSelectButton_Script.h"
#include "DescriptionButton_Script.h"
#include "ReadyButton_Script.h"
#include "Scene_CharacterSelection.h"

#include <Camera.h>
#include <Transform.h>
#include <MeshRenderer.h>
#include <Resources.h>
#include <Engine.h>
#include <UI.h>
#include <Timer.h>

#include "Scene_Loading.h"
#include "Creator.h"

Scene_CharacterSelection::Scene_CharacterSelection() :
	m_selected{ false },
	m_accTime{ 0.f },
	m_characterIndex{ -1 },
	m_descriptionIndex{ -1 },
	m_popUpActive{ false }
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

	ChangePopUpVisibility();

	auto character{ DetectCharacterSelection() };
	DeactivateCharacterUI();
	StartGame(character);
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
	CreateLayer();
	CreateUICamera();
	CreateUI();
	CreateLights();
}

void Scene_CharacterSelection::CreateLayer()
{
	GGameInstance->SetLayerName(0, L"UI");

	GET_SINGLE(SceneManager)->SetUIIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
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

	CreatePopUp();
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
	std::shared_ptr<Texture> texture{ GET_TEXTURE(L"Frame1") };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };

	float width{ static_cast<float>(GEngine->GetWindow().width) };
	float height{ static_cast<float>(GEngine->GetWindow().height) };
	Vec2 pos{ GetRatio(0.f, 15.f) };

	//FRAME
	for (int32_t i = 0; i < 3; ++i)
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		pos.x = -(width / 2.f) + (width / 3.f) * i + (width / 6.f);

		transform->SetLocalScale(Vec3{ width / 4.f, height / 1.5f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 450.f });

		std::shared_ptr<CharacterSelectButton_Script> script{ std::make_shared<CharacterSelectButton_Script>() };
		script->InsertTextures(texture);
		script->InsertTextures(GET_TEXTURE(L"Frame1_selected"));
		script->SetCharacterType(i + 1);

		obj->AddComponent(script);
		m_characterButton.push_back(script);

		AddGameObject(obj);
	}

	// IMAGE
	for (int32_t i = 0; i < 3; ++i)
	{
		if (i == 0)
			texture = GET_TEXTURE(L"Nana");
		else if (i == 1)
			texture = GET_TEXTURE(L"Mistic");
		else
			texture = GET_TEXTURE(L"Carmel");

		std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		pos.x = -(width / 2.f) + (width / 3.f) * i + (width / 6.f);

		transform->SetLocalScale(Vec3{ width / 4.5f, height / 1.7f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 400.f });

		AddGameObject(obj);
	}
}

void Scene_CharacterSelection::CreateCharacterNameButton()
{
	std::shared_ptr<Texture> texture{ GET_TEXTURE(L"Button2") };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };

	float width{ static_cast<float>(GEngine->GetWindow().width) };
	Vec2 pos{ GetRatio(0.f, -65.f) };

	// BUTTON
	for (int32_t i = 0; i < 3; ++i)
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		pos.x = -(width / 2.f) + (width / 3.f) * i + (width / 6.f);

		transform->SetLocalScale(Vec3{ 257.f * 1.2f, 95.f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 450.f });

		std::shared_ptr<DescriptionButton_Script> script{ std::make_shared<DescriptionButton_Script>() };
		script->InsertTextures(texture);
		script->InsertTextures(GET_TEXTURE(L"Button2_selected"));
		script->SetCharacterType(i + 1);

		obj->AddComponent(script);
		m_descriptionButton.push_back(script);

		AddGameObject(obj);
	}

	// FONT
	for (int32_t i = 0; i < 3; ++i)
	{
		if (i == 0)
			texture = GET_TEXTURE(L"Knight");
		else if (i == 1)
			texture = GET_TEXTURE(L"Mage");
		else
			texture = GET_TEXTURE(L"Priest");

		std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		pos.x = -(width / 2.f) + (width / 3.f) * i + (width / 6.f);

		transform->SetLocalScale(Vec3{ 250.f, 58.f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 400.f });

		AddGameObject(obj);
	}
}

void Scene_CharacterSelection::CreateReadyButton()
{
	std::shared_ptr<Texture> texture{ GET_TEXTURE(L"Button2") };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };
	auto pos{ GetRatio(86.f, -88.f) };

	// BUTTON
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		transform->SetLocalScale(Vec3{ 257.f, 95.f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 450.f });

		std::shared_ptr<ReadyButton_Script> script{ std::make_shared<ReadyButton_Script>() };
		script->InsertTextures(texture);
		script->InsertTextures(GET_TEXTURE(L"Button2_selected"));

		obj->AddComponent(script);
		m_readyButton = script;

		AddGameObject(obj);
	}

	// FONT
	texture = GET_TEXTURE(L"Ready");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		transform->SetLocalScale(Vec3{ 202.f, 42.f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 400.f });

		AddGameObject(obj);
	}
}

void Scene_CharacterSelection::CreatePopUp()
{
	CreateBlur();
	CreateCloseButton();
	CreateCharacterDescription();
}

void Scene_CharacterSelection::CreateBlur()
{
	std::shared_ptr<Texture> texture{ GET_RESOURCE->GetBlurTexture() };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };

	std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false, true) };
	obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

	float width{ static_cast<float>(GEngine->GetWindow().width) };
	float height{ static_cast<float>(GEngine->GetWindow().height) };

	auto transform{ obj->GetTransform() };
	transform->SetLocalScale(Vec3{ width, height, 1.f });
	transform->SetLocalPosition(Vec3{ 0.f, 0.f, 450.f });

	obj->GetMeshRenderer()->GetMaterial()->SetFloat(2, 1.f);

	AddGameObject(obj);
	GET_SINGLE(SceneManager)->SetBlurUI(obj);
}

void Scene_CharacterSelection::CreateCloseButton()
{
	std::shared_ptr<Texture> texture{ GET_TEXTURE(L"Close") };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };

	std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
	obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

	auto transform{ obj->GetTransform() };
	Vec2 pos{ GetRatio(-93.f, 86.f) };

#ifdef RELEASE
	float ratio{ 1.f };
#else
	float ratio{ 0.5f };
#endif

	transform->SetLocalScale(Vec3{ 118.f * ratio, 118.f * ratio, 1.f });
	transform->SetLocalPosition(Vec3{ pos.x, pos.y, 400.f });

	std::shared_ptr<CloseButton_Script> script{ std::make_shared<CloseButton_Script>() };
	script->InsertTextures(texture);
	script->InsertTextures(GET_TEXTURE(L"Close Selected"));
	obj->AddComponent(script);
	m_closeButton = script;

	AddGameObject(obj);
	m_popUp.push_back(obj);
}

void Scene_CharacterSelection::CreateCharacterDescription()
{
	std::shared_ptr<Texture> texture{ GET_TEXTURE(L"Pop Up Frame") };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };

	float width{ static_cast<float>(GEngine->GetWindow().width) };
	float height{ static_cast<float>(GEngine->GetWindow().height) };

	// FRAME
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		auto pos{ GetRatio(20.f, 0.f) };

		transform->SetLocalScale(Vec3{ 1498.f, 1020.f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, 0.f, 400.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// FONT
	for (int32_t i = 0; i < 3; ++i)
	{
		if (i == 0)
			texture = GET_TEXTURE(L"Knight2");
		else if (i == 1)
			texture = GET_TEXTURE(L"Mage2");
		else
			texture = GET_TEXTURE(L"Priest2");

		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		auto pos{ GetRatio(20.f, 84.f) };
		float ratio{ 1.5f };

		transform->SetLocalScale(Vec3{ 139.f * ratio, 27.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 400.f });

		AddGameObject(obj);
		m_characterDescription.push_back(obj);
	}

	// IMAGE
	for (int32_t i = 0; i < 3; ++i)
	{
		if (i == 0)
			texture = GET_TEXTURE(L"Nana");
		else if (i == 1)
			texture = GET_TEXTURE(L"Mistic");
		else
			texture = GET_TEXTURE(L"Carmel");

		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		auto pos{ GetRatio(-77.f, 0.f) };

		transform->SetLocalScale(Vec3{ width / 5.f, height / 2.f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, 0.f, 400.f });

		AddGameObject(obj);
		m_characterDescription.push_back(obj);
	}

	// DESCRIPTION
	{

	}
}

server::FBX_TYPE Scene_CharacterSelection::DetectCharacterSelection()
{
	for (int32_t i = 0; i < m_characterButton.size(); ++i)
	{
		if (m_characterButton[i]->IsSelect() == false)
			continue;

		m_selected = true;
		m_readyButton->SetCharacterSelectFlag(true);
		m_characterIndex = i;

		return m_characterButton[i]->GetCharacterType();
	}

	m_selected = false;
	m_characterIndex = -1;
	m_readyButton->SetCharacterSelectFlag(false);

	if (m_popUpActive == false)
	{
		for (auto& obj : m_characterButton)
		{
			obj->SetActive(true);
		}
	}

	return server::FBX_TYPE::NONE;
}

void Scene_CharacterSelection::StartGame(server::FBX_TYPE character)
{
	if (m_selected == false)
		return;

	if (m_readyButton->IsReady() == false)
		return;

	m_accTime += DELTA_TIME;

	if (m_accTime < 3.f)
		return;

	auto nextScene{ std::dynamic_pointer_cast<Scene_Loading>(Scene_Loading::Create(SCENE_GAMEPLAY)) };
	nextScene->SetSelectedCharacter(character);

	GET_SINGLE(SceneManager)->LoadScene(nextScene);
}

void Scene_CharacterSelection::DeactivateCharacterUI()
{
	if (m_selected == false)
		return;

	for (int32_t i = 0; i < m_characterButton.size(); ++i)
	{
		if (i == m_characterIndex)
			continue;

		m_characterButton[i]->SetActive(false);
	}
}

void Scene_CharacterSelection::ChangePopUpVisibility()
{
	if (m_closeButton->ClosePopUp() == true or GET_SINGLE(CInput)->GetButtonDown(KEY_TYPE::ESC))
	{
		for (auto& obj : m_popUp)
		{
			obj->GetUI()->SetVisible(false);
		}

		SetUIActive(true);

		m_characterDescription[m_descriptionIndex]->GetUI()->SetVisible(false);
		m_characterDescription[m_descriptionIndex + 3]->GetUI()->SetVisible(false);
		//m_characterDescription[m_descriptionIndex + 6]->GetUI()->SetVisible(false);
		m_closeButton->SetClosePopUpFlag(false);
		m_descriptionIndex = -1;
		m_popUpActive = false;

		return;
	}

	if (m_popUpActive == true)
	{
		static float accTime{ 0.f };

		accTime += DELTA_TIME;

		if (accTime > 0.2f)
		{
			SetUIActive(false);

			//m_popUpActive = false;
			accTime = 0.f;
		}
	}

	int32_t index{ -1 };

	for (int32_t i = 0; i < m_descriptionButton.size(); ++i)
	{
		if (m_descriptionButton[i]->IsSelect() == false)
			continue;

		index = i;
		m_popUpActive = true;
		break;
	}

	if (index == -1)
		return;

	for (auto& obj : m_popUp)
	{
		obj->GetUI()->SetVisible(true);
	}

	m_characterDescription[index]->GetUI()->SetVisible(true);
	m_characterDescription[index + 3]->GetUI()->SetVisible(true);
	//m_characterDescription[index + 6]->GetUI()->SetVisible(true);

	m_descriptionIndex = index;
}

void Scene_CharacterSelection::SetUIActive(bool flag)
{
	for (auto& obj : m_characterButton)
	{
		obj->SetActive(flag);
	}

	for (auto& obj : m_descriptionButton)
	{
		obj->SetActive(flag);
	}
}

shared_ptr<CScene> Scene_CharacterSelection::Create()
{
	std::shared_ptr<Scene_CharacterSelection> pInstance{ std::make_shared<Scene_CharacterSelection>() };

	pInstance->Init();

	return pInstance;
}
