#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "Engine.h"
#include "ConstantBuffer.h"
#include "Light.h"
#include "Engine.h"
#include "Resources.h"

void CScene::Awake()
{
	for (const shared_ptr<GameObject>& gameObject : m_gameObjects)
	{
		gameObject->Awake();
	}
}

void CScene::Start()
{
	for (const shared_ptr<GameObject>& gameObject : m_gameObjects)
	{
		gameObject->Start();
	}
}

void CScene::Update()
{
	for (const shared_ptr<GameObject>& gameObject : m_gameObjects)
	{
		gameObject->Update();
	}
}

void CScene::LateUpdate()
{
	for (const shared_ptr<GameObject>& gameObject : m_gameObjects)
	{
		gameObject->LateUpdate();
	}
}

void CScene::FinalUpdate()
{
	for (const shared_ptr<GameObject>& gameObject : m_gameObjects)
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

void CScene::AddGameObject(shared_ptr<GameObject> gameObject)
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

void CScene::RemoveGameObject(shared_ptr<GameObject> gameObject)
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