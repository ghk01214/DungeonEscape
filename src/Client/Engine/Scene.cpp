#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "Engine.h"
#include "ConstantBuffer.h"
#include "Light.h"
#include "Engine.h"
#include "Resources.h"
#include "Transform.h"
#include "MeshData.h"
#include "MonoBehaviour.h"
#include "MeshRenderer.h"

void CScene::Awake()
{
	m_requestQueue.clear();
	m_requestQueueSize = 0;

	for (const shared_ptr<CGameObject>& gameObject : m_gameObjects)
	{
		gameObject->Awake();
	}
}

void CScene::Start()
{
	for (const shared_ptr<CGameObject>& gameObject : m_gameObjects)
	{
		gameObject->Start();
	}
}

void CScene::Update()
{
	for (const shared_ptr<CGameObject>& gameObject : m_gameObjects)
	{
		gameObject->Update();
	}
}

void CScene::LateUpdate()
{
	for (const shared_ptr<CGameObject>& gameObject : m_gameObjects)
	{
		gameObject->LateUpdate();
	}
}

void CScene::FinalUpdate()
{
	for (const shared_ptr<CGameObject>& gameObject : m_gameObjects)
	{
		gameObject->FinalUpdate();
	}
}

shared_ptr<Camera> CScene::GetMainCamera()
{
	if (m_cameras.empty())
		return nullptr;

	return m_cameras[0];
}

void CScene::Render()
{
	PushLightData();

	ClearRTV();

	RenderShadow();

	RenderDeferred();

	RenderLights();

	RenderFinal();

	RenderForward();
}

void CScene::ClearRTV()
{
	// SwapChain Group 초기화
	int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->ClearRenderTargetView(backIndex);
	// Shadow Group 초기화
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->ClearRenderTargetView();
	// Deferred Group 초기화
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->ClearRenderTargetView();
	// Lighting Group 초기화
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->ClearRenderTargetView();
}

void CScene::RenderShadow()
{
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->OMSetRenderTargets();

	for (auto& light : m_lights)
	{
		if (light->GetLightType() != LIGHT_TYPE::DIRECTIONAL_LIGHT)
			continue;

		light->RenderShadow();
	}

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->WaitTargetToResource();
}

void CScene::RenderDeferred()
{
	// Deferred OMSet
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->OMSetRenderTargets();

	shared_ptr<Camera> mainCamera = m_cameras[0];
	mainCamera->SortGameObject();
	mainCamera->Render_Deferred();

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->WaitTargetToResource();
}

void CScene::RenderLights()
{
	shared_ptr<Camera> mainCamera = m_cameras[0];
	Camera::S_MatView = mainCamera->GetViewMatrix();
	Camera::S_MatProjection = mainCamera->GetProjectionMatrix();

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->OMSetRenderTargets();

	// 광원을 그린다.
	for (auto& light : m_lights)
	{
		light->Render();
	}

	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->WaitTargetToResource();
}

void CScene::RenderFinal()
{
	// Swapchain OMSet
	int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->OMSetRenderTargets(1, backIndex);

	GET_SINGLE(Resources)->Get<Material>(L"Final")->PushGraphicsData();
	GET_SINGLE(Resources)->Get<Mesh>(L"Rectangle")->Render();
}

void CScene::RenderForward()
{
	shared_ptr<Camera> mainCamera = m_cameras[0];
	mainCamera->Render_Forward();

	for (auto& camera : m_cameras)
	{
		if (camera == mainCamera)
			continue;

		camera->SortGameObject();
		camera->Render_Forward();
	}
}

void CScene::PushServerRequest(network::CPacket& packet)
{
	m_requestQueue.push_back(packet);
	++m_requestQueueSize;
}

void CScene::PopRequestQueue(int32_t size)
{
	for (int32_t i = 0; i < size; ++i)
	{
		if (m_requestQueue.empty() == true)
			return;

		m_requestQueue.pop_front();
		--m_requestQueueSize;
	}
}

void CScene::PushLightData()
{
	LightParams lightParams = {};

	for (auto& light : m_lights)
	{
		const LightInfo& lightInfo = light->GetLightInfo();

		light->SetLightIndex(lightParams.lightCount);

		lightParams.lights[lightParams.lightCount] = lightInfo;
		lightParams.lightCount++;
	}

	CONST_BUFFER(CONSTANT_BUFFER_TYPE::GLOBAL)->SetGraphicsGlobalData(&lightParams, sizeof(lightParams));
}

void CScene::AddDirectionalLight(LightDesc& lightDesc)
{
	shared_ptr<CGameObject> light = std::make_shared<CGameObject>();

	// 빛 생성
	light->AddComponent(make_shared<Light>());
	light->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);

	// 방향광은 위치 의미 없음. 0,0,0
	light->AddComponent(make_shared<Transform>());
	light->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));

	// 방향 정보
	light->GetLight()->SetLightDirection(lightDesc.vDirection);

	// 색상 정보
	light->GetLight()->SetDiffuse(lightDesc.vDiffuse);
	light->GetLight()->SetAmbient(lightDesc.vAmbient);
	light->GetLight()->SetSpecular(lightDesc.vSpecular);

	// light
	AddGameObject(light);
}

void CScene::AddPointLight(LightDesc& lightDesc)
{
	shared_ptr<CGameObject> light = std::make_shared<CGameObject>();

	// 빛 생성
	light->AddComponent(make_shared<Light>());
	light->GetLight()->SetLightType(LIGHT_TYPE::POINT_LIGHT);

	// 위치 정보
	light->AddComponent(make_shared<Transform>());
	light->GetTransform()->SetLocalPosition(lightDesc.vPosition);

	// 방향 정보
	light->GetLight()->SetLightDirection(lightDesc.vDirection);

	// 색상 정보
	light->GetLight()->SetDiffuse(lightDesc.vDiffuse);
	light->GetLight()->SetAmbient(lightDesc.vAmbient);
	light->GetLight()->SetSpecular(lightDesc.vSpecular);

	// 빛의 길이
	light->GetLight()->SetLightRange(lightDesc.lightRange);

	// light
	AddGameObject(light);
}

void CScene::AddSpotLight(LightDesc& lightDesc)
{
	shared_ptr<CGameObject> light = std::make_shared<CGameObject>();

	// 빛 생성
	light->AddComponent(make_shared<Light>());
	light->GetLight()->SetLightType(LIGHT_TYPE::SPOT_LIGHT);

	// 위치 정보
	light->AddComponent(make_shared<Transform>());
	light->GetTransform()->SetLocalPosition(lightDesc.vPosition);

	// 방향 정보
	light->GetLight()->SetLightDirection(lightDesc.vDirection);

	// 색상 정보
	light->GetLight()->SetDiffuse(lightDesc.vDiffuse);
	light->GetLight()->SetAmbient(lightDesc.vAmbient);
	light->GetLight()->SetSpecular(lightDesc.vSpecular);

	// 빛의 길이
	light->GetLight()->SetLightRange(lightDesc.lightRange);
	// 빛의 범위
	light->GetLight()->SetLightAngle(lightDesc.lightAngle);

	// light
	AddGameObject(light);
}


void CScene::AddGameObject(shared_ptr<CGameObject> gameObject)
{
	if (gameObject->GetCamera() != nullptr)
	{
		m_cameras.push_back(gameObject->GetCamera());
	}
	else if (gameObject->GetLight() != nullptr)
	{
		m_lights.push_back(gameObject->GetLight());
	}

	m_gameObjects.push_back(gameObject);
}

void CScene::AddGameObject(std::vector<std::shared_ptr<CGameObject>> gameObjects)
{
	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetCamera() != nullptr)
		{
			m_cameras.push_back(gameObject->GetCamera());
		}
		else if (gameObject->GetLight() != nullptr)
		{
			m_lights.push_back(gameObject->GetLight());
		}

		m_gameObjects.push_back(gameObject);
	}
}

void CScene::RemoveGameObject(shared_ptr<CGameObject> gameObject)
{
	if (gameObject->GetCamera())
	{
		auto findIt = std::find(m_cameras.begin(), m_cameras.end(), gameObject->GetCamera());
		if (findIt != m_cameras.end())
			m_cameras.erase(findIt);
	}
	else if (gameObject->GetLight())
	{
		auto findIt = std::find(m_lights.begin(), m_lights.end(), gameObject->GetLight());
		if (findIt != m_lights.end())
			m_lights.erase(findIt);
	}

	auto findIt = std::find(m_gameObjects.begin(), m_gameObjects.end(), gameObject);
	if (findIt != m_gameObjects.end())
		m_gameObjects.erase(findIt);
}

void CScene::RemoveGameObject(std::vector<std::shared_ptr<CGameObject>> gameObjects)
{
	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetCamera())
		{
			auto findIt = std::find(m_cameras.begin(), m_cameras.end(), gameObject->GetCamera());
			if (findIt != m_cameras.end())
				m_cameras.erase(findIt);
		}
		else if (gameObject->GetLight())
		{
			auto findIt = std::find(m_lights.begin(), m_lights.end(), gameObject->GetLight());
			if (findIt != m_lights.end())
				m_lights.erase(findIt);
		}

		auto findIt = std::find(m_gameObjects.begin(), m_gameObjects.end(), gameObject);
		if (findIt != m_gameObjects.end())
			m_gameObjects.erase(findIt);
	}
}

void CScene::AddPlayer(std::vector<std::shared_ptr<CGameObject>> gameObject)
{
	for (auto& object : gameObject)
		m_player.push_back(object);

	AddGameObject(gameObject);
}

void CScene::AddMonster(std::vector<std::shared_ptr<CGameObject>> gameObject)
{
	for (auto& object : gameObject)
		m_monster.push_back(object);

	AddGameObject(gameObject);
}

void CScene::AddBoss(std::vector<std::shared_ptr<CGameObject>> gameObject)
{
	for (auto& object : gameObject)
		m_boss.push_back(object);

	AddGameObject(gameObject);
}

void CScene::AddSceneObject(std::vector<std::shared_ptr<CGameObject>> gameObject)
{
	for (auto& object : gameObject)
		m_sceneObject.push_back(object);

	AddGameObject(gameObject);
}

void CScene::SetSkyBox(std::wstring key, std::wstring texture_path)
{
	// loading thread 적용 이후, 함수의 인자를 key로 받도록 수정
	// 현재는 value를 인자로 받고 있음
	shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(key, texture_path);

	// skybox가 존재할 경우
	if (nullptr != m_skyBox)
	{
		// texture만 교체
		m_skyBox->GetMeshRenderer()->GetMaterial()->SetTexture(0, texture);
	}
	else
	{
		shared_ptr<CGameObject> skybox = shared_ptr<CGameObject>();
		skybox->AddComponent(make_shared<Transform>());
		skybox->SetCheckFrustum(false);
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Skybox");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		skybox->AddComponent(meshRenderer);

		// skybox 추가
		m_skyBox = skybox;
		AddGameObject(skybox);
	}
}

void CScene::RemovePlayer(std::vector<std::shared_ptr<CGameObject>> gameObject)
{
	for (auto& object : gameObject)
	{
		auto findIt = std::find(m_player.begin(), m_player.end(), object);
		if (findIt != m_player.end())
			m_player.erase(findIt);
	}

	RemoveGameObject(gameObject);
}

void CScene::RemoveMonster(std::vector<std::shared_ptr<CGameObject>> gameObject)
{
	for (auto& object : gameObject)
	{
		auto findIt = std::find(m_monster.begin(), m_monster.end(), object);
		if (findIt != m_monster.end())
			m_monster.erase(findIt);
	}

	RemoveGameObject(gameObject);
}

void CScene::RemoveBoss(std::vector<std::shared_ptr<CGameObject>> gameObject)
{
	for (auto& object : gameObject)
	{
		auto findIt = std::find(m_boss.begin(), m_boss.end(), object);
		if (findIt != m_boss.end())
			m_boss.erase(findIt);
	}

	RemoveGameObject(gameObject);
}

void CScene::RemoveSceneObject(std::vector<std::shared_ptr<CGameObject>> gameObject)
{
	for (auto& object : gameObject)
	{
		auto findIt = std::find(m_sceneObject.begin(), m_sceneObject.end(), object);
		if (findIt != m_boss.end())
			m_sceneObject.erase(findIt);
	}

	RemoveGameObject(gameObject);
}