#include "pch.h"
#include "Scene_Start.h"

#include <Camera.h>
#include <Transform.h>
#include <MeshRenderer.h>
#include <Resources.h>
#include <Engine.h>
#include <FontManager.h>
#include <SoundManager.h>

#include "Scene_Loading.h"
#include "Start_Script.h"
#include "Start_StartButton.h"

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

	m_ipAddress.clear();

	if (playMusic == true)
		GET_SINGLE(CSoundMgr)->PlayBGM(L"Opening.ogg");
}

void Scene_Start::Update()
{
	__super::Update();

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
	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);

	CreateBackground(width, height);
	CreateTitle(width, height);
	CreateLogInButton(width, height);
}

void Scene_Start::LoadTextures(void)
{
	GET_SINGLE(Resources)->Load<Texture>(L"Lobby_InGame", L"..\\Resources\\Texture\\UI\\Start\\Lobby_InGame.PNG");
	GET_SINGLE(Resources)->Load<Texture>(L"Title", L"..\\Resources\\Texture\\UI\\Start\\Title.png");
	GET_SINGLE(Resources)->Load<Texture>(L"Log In", L"..\\Resources\\Texture\\UI\\Start\\Log In.png");
	GET_SINGLE(Resources)->Load<Texture>(L"Log In_selected", L"..\\Resources\\Texture\\UI\\Start\\Log In_selected.png");
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

void Scene_Start::CreateBackground(float width, float height)
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
		material->SetFloat(2, 1.f);
		meshRenderer->SetMaterial(material);
	}

	obj->AddComponent(meshRenderer);

	std::shared_ptr<Start_Script> behaviour = std::make_shared<Start_Script>();
	behaviour->InsertTextures(GET_SINGLE(Resources)->Get<Texture>(L"Lobby_InGame"));

	obj->AddComponent(behaviour);

	AddGameObject(obj);
}

void Scene_Start::CreateTitle(float width, float height)
{
	std::shared_ptr<CGameObject> obj = std::make_shared<CGameObject>();
	obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
	obj->AddComponent(std::make_shared<Transform>());

	float titleWidth{ 400.f };
	auto pos{ GetRatio(0.f, 0.f) };
	Print(pos.x);
	Print(pos.y);

	obj->GetTransform()->SetLocalScale(Vec3(titleWidth, titleWidth / 6.55f, 1.f));
	obj->GetTransform()->SetLocalPosition(Vec3(pos.x, pos.y, 400.f));

	std::shared_ptr<MeshRenderer> meshRenderer = std::make_shared<MeshRenderer>();
	{
		std::shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
		meshRenderer->SetMesh(mesh);
	}

	{
		std::shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Logo_texture");
		std::shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(L"Title");

		std::shared_ptr<Material> material = std::make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		material->SetFloat(2, 1.f);
		meshRenderer->SetMaterial(material);
	}

	obj->AddComponent(meshRenderer);

	std::shared_ptr<Start_Script> behaviour = std::make_shared<Start_Script>();
	behaviour->InsertTextures(GET_SINGLE(Resources)->Get<Texture>(L"Title"));

	obj->AddComponent(behaviour);

	AddGameObject(obj);
}

void Scene_Start::CreateLogInButton(float width, float height)
{
	std::shared_ptr<CGameObject> obj = std::make_shared<CGameObject>();
	obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
	obj->AddComponent(std::make_shared<Transform>());

	float yPos{ -(height / 4) };

	obj->GetTransform()->SetLocalScale(Vec3(281.f, 110.f, 1.f));
	obj->GetTransform()->SetLocalPosition(Vec3(0.f, yPos, 100.f));

	std::shared_ptr<MeshRenderer> meshRenderer = std::make_shared<MeshRenderer>();
	{
		std::shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
		meshRenderer->SetMesh(mesh);
	}

	{
		std::shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Logo_texture");
		std::shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(L"Log In");

		std::shared_ptr<Material> material = std::make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		material->SetFloat(2, 1.f);
		meshRenderer->SetMaterial(material);
	}

	obj->AddComponent(meshRenderer);

	std::shared_ptr<Start_StartButton> behaviour = std::make_shared<Start_StartButton>();
	behaviour->InsertTextures(GET_SINGLE(Resources)->Get<Texture>(L"Log In"));
	behaviour->InsertTextures(GET_SINGLE(Resources)->Get<Texture>(L"Log In_selected"));

	obj->AddComponent(behaviour);

	AddGameObject(obj);
}

shared_ptr<CScene> Scene_Start::Create()
{
	std::shared_ptr<Scene_Start> pInstance = std::make_shared<Scene_Start>();

	pInstance->Init();

	return pInstance;
}
