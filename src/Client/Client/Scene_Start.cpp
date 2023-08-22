﻿#include "pch.h"
#include "Setting_Script.h"
#include "Scene_Start.h"

#include <Camera.h>
#include <Transform.h>
#include <MeshRenderer.h>
#include <Resources.h>
#include <Engine.h>
#include <FontManager.h>
#include <SoundManager.h>
#include <UI.h>

#include "Creator.h"
#include "Scene_Loading.h"
#include "Start_StartButton.h"
#include "Start_Script.h"
#include "Close_Script.h"
#include "Mute_Script.h"

Scene_Start::Scene_Start()
{
}

void Scene_Start::Awake()
{
	__super::Awake();
}

void Scene_Start::Start()
{
	__super::Start();

	if (playMusic == true)
		GET_SINGLE(CSoundMgr)->PlayBGM(L"Opening.ogg");
}

void Scene_Start::Update()
{
	__super::Update();

	if (m_settingButton->ShowPopUp() == true)
	{
		for (auto& obj : m_popUp)
		{
			obj->GetUI()->SetVisible(true);
		}
	}
	else
	{
		/*for (auto& obj : m_popUp)
		{
			obj->GetUI()->SetVisible(false);
		}*/
	}

#pragma region [INPUT]
	//auto input{ GET_SINGLE(CInput) };

	//if (input->GetButtonDown(KEY_TYPE::KEY_1) == true or input->GetButtonDown(KEY_TYPE::NUMPAD_1))
	//	m_ipAddress += L'1';
	//else if (input->GetButtonDown(KEY_TYPE::KEY_2) == true or input->GetButtonDown(KEY_TYPE::NUMPAD_2))
	//	m_ipAddress += L'2';
	//else if (input->GetButtonDown(KEY_TYPE::KEY_3) == true or input->GetButtonDown(KEY_TYPE::NUMPAD_3))
	//	m_ipAddress += L'3';
	//else if (input->GetButtonDown(KEY_TYPE::KEY_4) == true or input->GetButtonDown(KEY_TYPE::NUMPAD_4))
	//	m_ipAddress += L'4';
	//else if (input->GetButtonDown(KEY_TYPE::KEY_5) == true or input->GetButtonDown(KEY_TYPE::NUMPAD_5))
	//	m_ipAddress += L'5';
	//else if (input->GetButtonDown(KEY_TYPE::KEY_6) == true or input->GetButtonDown(KEY_TYPE::NUMPAD_6))
	//	m_ipAddress += L'6';
	//else if (input->GetButtonDown(KEY_TYPE::KEY_7) == true or input->GetButtonDown(KEY_TYPE::NUMPAD_7))
	//	m_ipAddress += L'7';
	//else if (input->GetButtonDown(KEY_TYPE::KEY_8) == true or input->GetButtonDown(KEY_TYPE::NUMPAD_8))
	//	m_ipAddress += L'8';
	//else if (input->GetButtonDown(KEY_TYPE::KEY_9) == true or input->GetButtonDown(KEY_TYPE::NUMPAD_9))
	//	m_ipAddress += L'9';
	//else if (input->GetButtonDown(KEY_TYPE::KEY_0) == true or input->GetButtonDown(KEY_TYPE::NUMPAD_0))
	//	m_ipAddress += L'0';
	//else if (input->GetButtonDown(KEY_TYPE::DOT) == true or input->GetButtonDown(KEY_TYPE::NUMPAD_DOT))
	//	m_ipAddress += L'.';
	//else if (input->GetButtonDown(KEY_TYPE::BACKSAPCE) == true and m_ipAddress.empty() == false)
	//	m_ipAddress.pop_back();
	//else if (input->GetButtonDown(KEY_TYPE::ENTER) == true or input->GetButtonDown(KEY_TYPE::NUMPAD_ENTER) == true)
	//	serverIP_Address = m_ipAddress;

	//if (m_ipAddress.empty() == false)
	//	GET_SINGLE(FontManager)->RenderFonts(m_ipAddress, Vec2(-300.f, 200.f), Vec2(10.f, 10.f), 10.f);
#pragma endregion
}

void Scene_Start::LateUpdate()
{
	__super::LateUpdate();
}

void Scene_Start::FinalUpdate()
{
	__super::FinalUpdate();
}

void Scene_Start::Render()
{
	__super::Render();
}

void Scene_Start::Init(void)
{
	LoadTextures();

	CreateLayer();
	CreateUICamera();
	CreateUI();
	CreateLights();
}

void Scene_Start::CreateLayer(void)
{
	GGameInstance->SetLayerName(0, L"UI");

	GET_SINGLE(FontManager)->SetUIIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
}

void Scene_Start::CreateUICamera(void)
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

void Scene_Start::CreateUI(void)
{
	CreateBackground();
	CreateTitle();
	CreateLogInButton();
	CreateSettingButton();

	CreatePopUp();
}

void Scene_Start::LoadTextures(void)
{
	GET_SINGLE(Resources)->Load<Texture>(L"Lobby_InGame", L"..\\Resources\\Texture\\UI\\Start\\Lobby_InGame.PNG");

	GET_SINGLE(Resources)->Load<Texture>(L"Title", L"..\\Resources\\Texture\\UI\\Font\\Title.png");
	GET_SINGLE(Resources)->Load<Texture>(L"Log In", L"..\\Resources\\Texture\\UI\\Font\\Log In.png");

	GET_SINGLE(Resources)->Load<Texture>(L"Button", L"..\\Resources\\Texture\\UI\\Button\\Button.png");
	GET_SINGLE(Resources)->Load<Texture>(L"Button_selected", L"..\\Resources\\Texture\\UI\\Button\\Button_selected.png");
	GET_SINGLE(Resources)->Load<Texture>(L"Setting", L"..\\Resources\\Texture\\UI\\Button\\Setting.png");
}

void Scene_Start::CreateLights(void)
{
	LightDesc lightDesc;
	lightDesc.vDirection = Vec3(0.f, -1.f, 0.5f);
	lightDesc.vDiffuse = Vec3(1.f, 1.f, 1.f);
	lightDesc.vAmbient = Vec3(0.9f, 0.9f, 0.9f);
	lightDesc.vSpecular = Vec3(0.1f, 0.1f, 0.1f);

	AddDirectionalLight(lightDesc);
}

void Scene_Start::CreateBackground()
{
	std::shared_ptr<Texture> texture = GET_TEXTURE(L"Lobby_InGame");
	std::shared_ptr<Shader> shader = GET_SHADER(L"Logo_texture");

	// 사용할 텍스쳐와 셰이더만 인자로 넘겨 UI 객체 생성
	std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };

	// 레이어 설정
	obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI

	// 위치 및 카메라 인덱스 설정
	Vec2 pos{ GetRatio(30.f, 30.f) };
	auto transform{ obj->GetTransform() };

	float width{ static_cast<float>(GEngine->GetWindow().width) };
	float height{ static_cast<float>(GEngine->GetWindow().height) };

	transform->SetLocalScale(Vec3{ width, height, 1.f });
	transform->SetLocalPosition(Vec3{ 0.f, 0.f, 500.f });

	AddGameObject(obj);
}

void Scene_Start::CreateTitle()
{
	std::shared_ptr<Texture> texture = GET_TEXTURE(L"Title");
	std::shared_ptr<Shader> shader = GET_SHADER(L"Logo_texture");

	std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
	obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

#ifdef RELEASE
	float titleWidth{ 800.f };
#else
	float titleWidth{ 400.f };
#endif

	auto pos{ GetRatio(0.f, 45.f) };
	auto transform{ obj->GetTransform() };

	transform->SetLocalScale(Vec3{ titleWidth, titleWidth / 6.55f, 1.f });
	transform->SetLocalPosition(Vec3{ pos.x, pos.y, 400.f });

	AddGameObject(obj);
}

void Scene_Start::CreateLogInButton()
{
	std::shared_ptr<Shader> shader = GET_SHADER(L"Logo_texture");

	// BUTTON
	{
		std::shared_ptr<Texture> texture = GET_TEXTURE(L"Button");

		std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		Vec2 pos{ GetRatio(0.f, -49.f) };
		auto transform{ obj->GetTransform() };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 281.f * ratio, 110.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 100.f });

		std::shared_ptr<Start_StartButton> behaviour{ std::make_shared<Start_StartButton>() };
		behaviour->InsertTextures(texture);
		behaviour->InsertTextures(GET_TEXTURE(L"Button Selected"));
		obj->AddComponent(behaviour);

		AddGameObject(obj);
	}

	// LOG IN TEXT
	{
		std::shared_ptr<Texture> texture = GET_TEXTURE(L"Log In");

		std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		Vec2 pos{ GetRatio(0.f, -48.f) };
		auto transform{ obj->GetTransform() };

#ifdef RELEASE
		float ratio{ 1.f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 268.f * ratio, 58.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 99.f });

		AddGameObject(obj);
	}
}

void Scene_Start::CreateSettingButton()
{
	std::shared_ptr<Texture> texture = GET_TEXTURE(L"Setting");
	std::shared_ptr<Shader> shader = GET_SHADER(L"Logo_texture");

	std::shared_ptr<CGameObject> obj{ Creator::CreateUIObject(texture, shader, true) };
	obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

	// 위치 및 카메라 인덱스 설정
	Vec2 pos{ GetRatio(-93.f, 86.f) };
	auto transform{ obj->GetTransform() };

	transform->SetLocalScale(Vec3{ 121.f, 121.f, 1.f });
	transform->SetLocalPosition(Vec3{ pos.x, pos.y, 100.f });

	std::shared_ptr<Setting_Script> behaviour{ std::make_shared<Setting_Script>() };
	obj->AddComponent(behaviour);
	m_settingButton = behaviour;

	AddGameObject(obj);
}

void Scene_Start::CreatePopUp()
{
	CreateBlur();
	CreateSettingFrame();
	CreateBGM();
	CreateSE();

	for (auto& obj : m_popUp)
	{
		obj->GetUI()->SetVisible(true);
	}
}

void Scene_Start::CreateBlur()
{
	std::shared_ptr<Texture> texture = GET_TEXTURE(L"White Blur");
	std::shared_ptr<Shader> shader = GET_SHADER(L"Logo_texture");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI

		auto transform{ obj->GetTransform() };
		float width{ static_cast<float>(GEngine->GetWindow().width) };
		float height{ static_cast<float>(GEngine->GetWindow().height) };

		transform->SetLocalScale(Vec3{ width, height, 1.f });
		transform->SetLocalPosition(Vec3{ 0.f, 0.f, 450.f });

		obj->GetMeshRenderer()->GetMaterial()->SetFloat(2, 0.5f);

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
}

void Scene_Start::CreateCloseButton()
{
	std::shared_ptr<Texture> texture{ GET_TEXTURE(L"Close") };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(-93.f, 86.f) };

#ifdef RELEASE
		float ratio{ 1.f };
#else
		float ratio{ 0.5f };
#endif

		transform->SetLocalScale(Vec3{ 118.f * ratio, 118.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 400.f });

		std::shared_ptr<Close_Script> behaviour{ std::make_shared<Close_Script>() };
		behaviour->InsertTextures(texture);
		behaviour->InsertTextures(GET_TEXTURE(L"Close Selected"));
		obj->AddComponent(behaviour);

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
}

void Scene_Start::CreateSettingFrame()
{
	std::shared_ptr<Texture> texture{ GET_TEXTURE(L"Setting Frame") };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI

		auto transform{ obj->GetTransform() };

#ifdef RELEASE
		float ratio{ 1.f };
#else
		float ratio{ 0.5f };
#endif

		transform->SetLocalScale(Vec3{ 898.f * ratio, 1029.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ 0.f, 0.f, 400.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
}

void Scene_Start::CreateBGM()
{
	std::shared_ptr<Texture> texture{ GET_TEXTURE(L"BGM") };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };

	// BGM BUTTON
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(-33.f, 20.f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 45.f * ratio, 60.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		std::shared_ptr<Mute_Script> behaviour{ std::make_shared<Mute_Script>(Mute_Script::SOUND_TYPE::BGM) };
		behaviour->InsertTextures(texture);
		behaviour->InsertTextures(GET_TEXTURE(L"BGM Mute"));
		obj->AddComponent(behaviour);

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
}

void Scene_Start::CreateSE()
{
	std::shared_ptr<Texture> texture{ GET_TEXTURE(L"SE") };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };

	// BGM BUTTON
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(-33.f, -20.f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 45.f * ratio, 60.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		std::shared_ptr<Mute_Script> behaviour{ std::make_shared<Mute_Script>(Mute_Script::SOUND_TYPE::SE) };
		behaviour->InsertTextures(texture);
		behaviour->InsertTextures(GET_TEXTURE(L"SE Mute"));
		obj->AddComponent(behaviour);

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
}

shared_ptr<CScene> Scene_Start::Create()
{
	std::shared_ptr<Scene_Start> pInstance = std::make_shared<Scene_Start>();

	pInstance->Init();

	return pInstance;
}
