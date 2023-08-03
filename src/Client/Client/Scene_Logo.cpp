#include "pch.h"
#include "Scene_Logo.h"

#include <Camera.h>
#include <Transform.h>
#include <MeshRenderer.h>
#include <Resources.h>
#include <Engine.h>
#include <FontManager.h>

#include "Scene_Start.h"
#include "Scene_Loading.h"
#include "Logo_Start.h"

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

void Scene_Logo::Init(void)
{
	LoadTextures();

	CreateLayer();
	CreateUICamera();
	CreateUI();
	CreateLights();
}

void Scene_Logo::CreateLayer(void)
{
	GGameInstance->SetLayerName(0, L"UI");

	GET_SINGLE(FontManager)->SetUIIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));
}

void Scene_Logo::CreateUICamera(void)
{
	shared_ptr<CGameObject> camera = std::make_shared<CGameObject>();
	camera->SetName(L"Orthographic_Camera");

	camera->AddComponent(make_shared<Transform>());
	camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));

	camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, 800*600
	camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);

	uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");

	camera->GetCamera()->SetCullingMaskAll(); // 다 끄고
	camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI만 찍음

	AddGameObject(camera);
}

void Scene_Logo::CreateUI(void)
{
	shared_ptr<CGameObject> obj = std::make_shared<CGameObject>();
	obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI

	obj->AddComponent(make_shared<Transform>());

	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);

	obj->GetTransform()->SetLocalScale(Vec3(width, height, 1.f));
	obj->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 500.f));

	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
		meshRenderer->SetMesh(mesh);
	}

	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Logo_texture");

		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(L"Logo_School");

		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		meshRenderer->SetMaterial(material);
	}

	obj->AddComponent(meshRenderer);

	vector<shared_ptr<Texture>> textures;
	textures.push_back(GET_SINGLE(Resources)->Get<Texture>(L"Logo_School"));

	shared_ptr<Logo_Start> behaviour = make_shared<Logo_Start>();
	behaviour->SetLogoInfo(2.f, 1.f, 1.f, textures);
	obj->AddComponent(behaviour);

	AddGameObject(obj);
}

void Scene_Logo::LoadTextures(void)
{
	GET_SINGLE(Resources)->Load<Texture>(L"Logo_School", L"..\\Resources\\Texture\\Logo\\Logo_School.png");
}

void Scene_Logo::CreateLights(void)
{
	LightDesc lightDesc;
	lightDesc.vDirection = Vec3(0.f, -1.f, 0.5f);
	lightDesc.vDiffuse = Vec3(1.f, 1.f, 1.f);
	lightDesc.vAmbient = Vec3(0.9f, 0.9f, 0.9f);
	lightDesc.vSpecular = Vec3(0.1f, 0.1f, 0.1f);

	AddDirectionalLight(lightDesc);
}

shared_ptr<CScene> Scene_Logo::Create()
{
	shared_ptr<Scene_Logo> pInstance = std::make_shared<Scene_Logo>();

	pInstance->Init();

	return pInstance;
}
