﻿#include "pch.h"
#include "LoginButton_Script.h"
#include "SettingButton_Script.h"
#include "CloseButton_Script.h"
#include "SliderTip_Script.h"
#include "VolumeSlider_Script.h"
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
#include "MuteButton_Script.h"

Scene_Start::Scene_Start() :
	m_logInButton{ nullptr },
	m_settingButton{ nullptr },
	m_closeButton{ nullptr },
	m_openSetting{ false }
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

	m_openSetting = false;
}

void Scene_Start::Update()
{
	ChangeScene();

	__super::Update();

	ChangePopUpVisibility();
	ChangeVolume();
	ChangeMuteTexture();

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

		std::shared_ptr<LoginButton_Script> script{ std::make_shared<LoginButton_Script>() };
		script->InsertTextures(texture);
		script->InsertTextures(GET_TEXTURE(L"Button Selected"));
		obj->AddComponent(script);
		m_logInButton = script;

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

	std::shared_ptr<SettingButton_Script> script{ std::make_shared<SettingButton_Script>() };
	obj->AddComponent(script);
	m_settingButton = script;

	AddGameObject(obj);
}

void Scene_Start::CreatePopUp()
{
	CreateBlur();
	CreateCloseButton();
	CreateSettingFrame();
	CreateBGMButton();
	CreateBGMSlider();
	CreateSEButton();
	CreateSESlider();
}

void Scene_Start::CreateBlur()
{
	std::shared_ptr<Texture> texture = GET_TEXTURE(L"Blur");
	std::shared_ptr<Shader> shader = GET_SHADER(L"Logo_texture");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false, true) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI

		auto transform{ obj->GetTransform() };
		float width{ static_cast<float>(GEngine->GetWindow().width) };
		float height{ static_cast<float>(GEngine->GetWindow().height) };

		transform->SetLocalScale(Vec3{ width, height, 1.f });
		transform->SetLocalPosition(Vec3{ 0.f, 0.f, 450.f });

		obj->GetMeshRenderer()->GetMaterial()->SetFloat(2, 1.f);

		AddGameObject(obj);
		GET_SINGLE(SceneManager)->SetBlurUI(obj);
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

		std::shared_ptr<CloseButton_Script> script{ std::make_shared<CloseButton_Script>() };
		script->InsertTextures(texture);
		script->InsertTextures(GET_TEXTURE(L"Close Selected"));
		obj->AddComponent(script);
		m_closeButton = script;

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

void Scene_Start::CreateBGMButton()
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

		std::shared_ptr<MuteButton_Script> script{ std::make_shared<MuteButton_Script>(MuteButton_Script::SOUND_TYPE::BGM) };
		script->InsertTextures(texture);
		script->InsertTextures(GET_TEXTURE(L"BGM Mute"));
		obj->AddComponent(script);
		m_muteButton.push_back(script);

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
}

void Scene_Start::CreateBGMSlider()
{
	std::shared_ptr<Texture> texture{ GET_TEXTURE(L"Slider Frame(L)") };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };

#pragma region [FRAME]
	// SLIDER FRAME(L)
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(-22.9f, 20.f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 37.f * ratio, 76.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER FRAME(R)
	texture = GET_TEXTURE(L"Slider Frame(R)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(34.85f, 20.f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 37.f * ratio, 76.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER FRAME(C)
	texture = GET_TEXTURE(L"Slider Frame(C)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(5.98f, 20.f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 23.f * ratio * 14.37f, 76.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
#pragma endregion

#pragma region [INNER FRAME]
	// SLIDER INNER FRAME(L)
	texture = GET_TEXTURE(L"Slider Inner Frame(L)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(-21.5f, 21.3f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 16.f * ratio, 32.f * ratio * 1.2f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER INNER FRAME(R)
	texture = GET_TEXTURE(L"Slider Inner Frame(R)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(33.35f, 21.3f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 16.f * ratio, 32.f * ratio * 1.2f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER INNER FRAME(C)
	texture = GET_TEXTURE(L"Slider Inner Frame(C)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(5.98f, 21.3f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 8.f * ratio * 41.5f, 32.f * ratio * 1.2f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
#pragma endregion

#pragma region [FILL AREA]
	// SLIDER FILL AREA(L)
	texture = GET_TEXTURE(L"Slider Fill Area(L)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(-21.f, 21.3f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 9.f * ratio, 18.f * ratio * 1.5f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER FILL AREA(R)
	texture = GET_TEXTURE(L"Slider Fill Area(R)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(33.f, 21.3f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 9.f * ratio, 18.f * ratio * 1.5f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER FILL AREA(C)
	texture = GET_TEXTURE(L"Slider Fill Area(C)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(5.98f, 21.3f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 6.f * ratio * 55.7f, 18.f * ratio * 1.5f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
#pragma endregion

#pragma region [FILL]
	// SLIDER FILL(L)
	texture = GET_TEXTURE(L"Slider Fill(L)");

#ifdef RELEASE
	float ratio{ 1.5f };
#else
	float ratio{ 1.f };
#endif

	Vec3 leftScale{ 9.f * ratio, 18.f * ratio * 1.5f, 1.f };
	Vec2 leftPos{ GetRatio(-20.8f, 21.3f) };
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };

		transform->SetLocalScale(leftScale);
		transform->SetLocalPosition(Vec3{ leftPos.x, leftPos.y, 350.f });

		std::shared_ptr<VolumeSlider_Script> script{ std::make_shared<VolumeSlider_Script>(false) };
		script->InsertTextures(texture);
		script->InsertTextures(GET_TEXTURE(L"Slider Fill(L) Mute"));
		obj->AddComponent(script);
		m_volumeSliderLeftTip.push_back(script);

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER FILL(C)
	texture = GET_TEXTURE(L"Slider Fill(C)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec3 scale{ 6.f * ratio * 27.5f, 18.f * ratio * 1.5f, 1.f };
		Vec2 pos{ leftPos.x + leftScale.x * 0.5f + scale.x * 0.5f, leftPos.y };

		transform->SetLocalScale(scale);
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		std::shared_ptr<VolumeSlider_Script> script{ std::make_shared<VolumeSlider_Script>() };
		script->InsertTextures(texture);
		script->InsertTextures(GET_TEXTURE(L"Slider Fill(C) Mute"));
		obj->AddComponent(script);
		m_volumeSlider.push_back(script);

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
#pragma endregion

	// SLIDER TIP
	texture = GET_TEXTURE(L"Slider Tip");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(6.f, 20.7f) };

#ifdef RELEASE
		float ratio{ 1.f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 96.f * ratio, 96.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		std::shared_ptr<SliderTip_Script> script{ std::make_shared<SliderTip_Script>(SliderTip_Script::SLIDER_TYPE::BGM) };
		script->InsertTextures(texture);
		script->InsertTextures(GET_TEXTURE(L"Slider Tip Mute"));
		obj->AddComponent(script);
		m_sliderTip.push_back(script);

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
}

void Scene_Start::CreateSEButton()
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

		std::shared_ptr<MuteButton_Script> script{ std::make_shared<MuteButton_Script>(MuteButton_Script::SOUND_TYPE::SE) };
		script->InsertTextures(texture);
		script->InsertTextures(GET_TEXTURE(L"SE Mute"));
		obj->AddComponent(script);
		m_muteButton.push_back(script);

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
}

void Scene_Start::CreateSESlider()
{
	std::shared_ptr<Texture> texture{ GET_TEXTURE(L"Slider Frame(L)") };
	std::shared_ptr<Shader> shader{ GET_SHADER(L"Logo_texture") };

#pragma region [FRAME]
	// SLIDER FRAME(L)
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(-22.9f, -20.f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 37.f * ratio, 76.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER FRAME(R)
	texture = GET_TEXTURE(L"Slider Frame(R)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(34.85f, -20.f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 37.f * ratio, 76.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER FRAME(C)
	texture = GET_TEXTURE(L"Slider Frame(C)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(5.98f, -20.f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 23.f * ratio * 14.37f, 76.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
#pragma endregion

#pragma region [INNER FRAME]
	// SLIDER INNER FRAME(L)
	texture = GET_TEXTURE(L"Slider Inner Frame(L)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(-21.5f, -18.7f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 16.f * ratio, 32.f * ratio * 1.2f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER INNER FRAME(R)
	texture = GET_TEXTURE(L"Slider Inner Frame(R)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(33.35f, -18.7f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 16.f * ratio, 32.f * ratio * 1.2f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER INNER FRAME(C)
	texture = GET_TEXTURE(L"Slider Inner Frame(C)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(5.98f, -18.7f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 8.f * ratio * 41.5f, 32.f * ratio * 1.2f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
#pragma endregion

#pragma region [FILL AREA]
	// SLIDER FILL AREA(L)
	texture = GET_TEXTURE(L"Slider Fill Area(L)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(-21.f, -18.7f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 9.f * ratio, 18.f * ratio * 1.5f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER FILL AREA(R)
	texture = GET_TEXTURE(L"Slider Fill Area(R)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(33.f, -18.7f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 9.f * ratio, 18.f * ratio * 1.5f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER FILL AREA(C)
	texture = GET_TEXTURE(L"Slider Fill Area(C)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(5.98f, -18.7f) };

#ifdef RELEASE
		float ratio{ 1.5f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 6.f * ratio * 55.7f, 18.f * ratio * 1.5f, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
#pragma endregion

#pragma region [FILL]
	// SLIDER FILL(L)
	texture = GET_TEXTURE(L"Slider Fill(L)");

#ifdef RELEASE
	float ratio{ 1.5f };
#else
	float ratio{ 1.f };
#endif

	Vec3 leftScale{ 9.f * ratio, 18.f * ratio * 1.5f, 1.f };
	Vec2 leftPos{ GetRatio(-20.8f, -18.7f) };
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };

		transform->SetLocalScale(leftScale);
		transform->SetLocalPosition(Vec3{ leftPos.x, leftPos.y, 350.f });

		std::shared_ptr<VolumeSlider_Script> script{ std::make_shared<VolumeSlider_Script>(false) };
		script->InsertTextures(texture);
		script->InsertTextures(GET_TEXTURE(L"Slider Fill(L) Mute"));
		obj->AddComponent(script);
		m_volumeSliderLeftTip.push_back(script);

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}

	// SLIDER FILL(C)
	texture = GET_TEXTURE(L"Slider Fill(C)");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };

		Vec3 scale{ 6.f * ratio * 27.5f, 18.f * ratio * 1.5f, 1.f };
		Vec2 pos{ leftPos.x + leftScale.x * 0.5f + scale.x * 0.5f, leftPos.y };

		transform->SetLocalScale(scale);
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		std::shared_ptr<VolumeSlider_Script> script{ std::make_shared<VolumeSlider_Script>() };
		script->InsertTextures(texture);
		script->InsertTextures(GET_TEXTURE(L"Slider Fill(C) Mute"));
		obj->AddComponent(script);
		m_volumeSlider.push_back(script);

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
#pragma endregion

	// SLIDER TIP
	texture = GET_TEXTURE(L"Slider Tip");
	{
		std::shared_ptr<CGameObject> obj{ Creator::CreatePopUpObject(texture, shader, false) };
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

		auto transform{ obj->GetTransform() };
		Vec2 pos{ GetRatio(6.f, -19.3f) };

#ifdef RELEASE
		float ratio{ 1.f };
#else
		float ratio{ 1.f };
#endif

		transform->SetLocalScale(Vec3{ 96.f * ratio, 96.f * ratio, 1.f });
		transform->SetLocalPosition(Vec3{ pos.x, pos.y, 350.f });

		std::shared_ptr<SliderTip_Script> script{ std::make_shared<SliderTip_Script>(SliderTip_Script::SLIDER_TYPE::SE) };
		script->InsertTextures(texture);
		script->InsertTextures(GET_TEXTURE(L"Slider Tip Mute"));
		obj->AddComponent(script);
		m_sliderTip.push_back(script);

		AddGameObject(obj);
		m_popUp.push_back(obj);
	}
}

void Scene_Start::ChangeScene()
{
	if (m_logInButton->IsLogIn() == false)
		return;

	GET_SINGLE(SceneManager)->LoadScene(Scene_Loading::Create(SCENE_CHARACTER_SELECT));
}

void Scene_Start::ChangePopUpVisibility()
{
	if (GET_SINGLE(CInput)->GetButtonDown(KEY_TYPE::ESC) == true)
	{
		m_openSetting = !m_openSetting;

		if (m_openSetting == true)
		{
			for (auto& obj : m_popUp)
			{
				obj->GetUI()->SetVisible(true);
			}

			m_settingButton->GetGameObject()->GetUI()->SetVisible(false);
			m_logInButton->SetActive(false);
		}
		else
		{
			for (auto& obj : m_popUp)
			{
				obj->GetUI()->SetVisible(false);
			}

			m_settingButton->GetGameObject()->GetUI()->SetVisible(true);
			m_logInButton->SetActive(true);
		}
	}

	if (m_settingButton->ShowPopUp() == true)
	{
		for (auto& obj : m_popUp)
		{
			obj->GetUI()->SetVisible(true);
		}

		m_settingButton->SetShowPopUpFlag(false);
		m_logInButton->SetActive(false);
		m_openSetting = true;
	}
	else if (m_closeButton->ClosePopUp() == true)
	{
		for (auto& obj : m_popUp)
		{
			obj->GetUI()->SetVisible(false);
		}

		m_closeButton->SetClosePopUpFlag(false);
		m_settingButton->GetGameObject()->GetUI()->SetVisible(true);
		m_logInButton->SetActive(true);
		m_openSetting = false;
	}
}

void Scene_Start::ChangeVolume()
{
	if (m_sliderTip[BGM]->GetClick() == true)
		m_volumeSlider[BGM]->SetActive(true);
	else
		m_volumeSlider[BGM]->SetActive(false);

	if (m_sliderTip[SE]->GetClick() == true)
		m_volumeSlider[SE]->SetActive(true);
	else
		m_volumeSlider[SE]->SetActive(false);
}

void Scene_Start::ChangeMuteTexture()
{
	if (m_muteButton[BGM]->IsMute() == true)
	{
		m_sliderTip[BGM]->ChangeTexture(1);
		m_volumeSlider[BGM]->ChangeMuteTexture(true);
		m_volumeSliderLeftTip[BGM]->ChangeMuteTexture(true);
	}
	else
	{
		m_sliderTip[BGM]->ChangeTexture(0);
		m_volumeSlider[BGM]->ChangeMuteTexture(false);
		m_volumeSliderLeftTip[BGM]->ChangeMuteTexture(false);
	}

	if (m_muteButton[SE]->IsMute() == true)
	{
		m_sliderTip[SE]->ChangeTexture(1);
		m_volumeSlider[SE]->ChangeMuteTexture(true);
		m_volumeSliderLeftTip[SE]->ChangeMuteTexture(true);
	}
	else
	{
		m_sliderTip[SE]->ChangeTexture(0);
		m_volumeSlider[SE]->ChangeMuteTexture(false);
		m_volumeSliderLeftTip[SE]->ChangeMuteTexture(false);
	}
}

shared_ptr<CScene> Scene_Start::Create()
{
	std::shared_ptr<Scene_Start> pInstance = std::make_shared<Scene_Start>();

	pInstance->Init();

	return pInstance;
}
