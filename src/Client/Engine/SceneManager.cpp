#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"

#include "Engine.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"
#include "Light.h"
#include "Input.h"

#include "TestCameraScript.h"
#include "Resources.h"
#include "ParticleSystem.h"
#include "Terrain.h"
#include "SphereCollider.h"
#include "MeshData.h"
#include "FontManager.h"
#include "EffectManager.h"

void SceneManager::Start()
{
	m_activeScene->Start();
}

void SceneManager::Update()
{
	if (m_activeScene != m_nextScene)
		m_activeScene = m_nextScene;

	if (m_activeScene == nullptr)
		return;

	m_activeScene->Update();

	const vector<shared_ptr<CGameObject>>& fonts = GET_SINGLE(FontManager)->GetFontObject();
	for (auto& font : fonts)
	{
		font->Update();
	}

	const vector<shared_ptr<CGameObject>>& effects = GET_SINGLE(EffectManager)->GetEffectObject();
	for (auto& effect : effects)
	{
		effect->Update();
	}
}

void SceneManager::LateUpdate()
{
	if (m_activeScene == nullptr)
		return;

	m_activeScene->LateUpdate();

	const vector<shared_ptr<CGameObject>>& fonts = GET_SINGLE(FontManager)->GetFontObject();
	for (auto& font : fonts)
	{
		font->LateUpdate();
	}

	const vector<shared_ptr<CGameObject>>& effects = GET_SINGLE(EffectManager)->GetEffectObject();
	for (auto& effect : effects)
	{
		effect->LateUpdate();
	}
}

void SceneManager::FinalUpdate()
{
	if (m_activeScene == nullptr)
		return;

	m_activeScene->FinalUpdate();

	const vector<shared_ptr<CGameObject>>& fonts = GET_SINGLE(FontManager)->GetFontObject();
	for (auto& font : fonts)
	{
		font->FinalUpdate();
	}

	const vector<shared_ptr<CGameObject>>& effects = GET_SINGLE(EffectManager)->GetEffectObject();
	for (auto& effect : effects)
	{
		effect->FinalUpdate();
	}
}

// TEMP
void SceneManager::Render()
{
	if (m_activeScene)
		m_activeScene->Render();
}

void SceneManager::LoadScene(std::shared_ptr<CScene> scene)
{
	m_nextScene = scene;

	m_nextScene->Awake();
	m_nextScene->Start();
}

void SceneManager::SetLayerName(uint8 index, const wstring& name)
{
	// 기존 데이터 삭제
	const wstring& prevName = m_layerNames[index];
	m_layerIndex.erase(prevName);

	m_layerNames[index] = name;
	m_layerIndex[name] = index;
}

uint8 SceneManager::LayerNameToIndex(const wstring& name)
{
	auto findIt = m_layerIndex.find(name);
	if (findIt == m_layerIndex.end())
		return 0;

	return findIt->second;
}

shared_ptr<CGameObject> SceneManager::Pick(int32 screenX, int32 screenY)
{
	shared_ptr<Camera> camera = GetActiveScene()->GetMainCamera();

	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);

	Matrix projectionMatrix = camera->GetProjectionMatrix();

	// ViewSpace에서 Picking 진행
	float viewX = (+2.0f * screenX / width - 1.0f) / projectionMatrix(0, 0);
	float viewY = (-2.0f * screenY / height + 1.0f) / projectionMatrix(1, 1);

	Matrix viewMatrix = camera->GetViewMatrix();
	Matrix viewMatrixInv = viewMatrix.Invert();

	auto& gameObjects = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObjects();

	float minDistance = FLT_MAX;
	shared_ptr<CGameObject> picked;

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetCollider() == nullptr)
			continue;

		// ViewSpace에서의 Ray 정의
		Vec4 rayOrigin = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		Vec4 rayDir = Vec4(viewX, viewY, 1.0f, 0.0f);

		// WorldSpace에서의 Ray 정의
		rayOrigin = XMVector3TransformCoord(rayOrigin, viewMatrixInv);
		rayDir = XMVector3TransformNormal(rayDir, viewMatrixInv);
		rayDir.Normalize();

		// WorldSpace에서 연산
		float distance = 0.f;
		if (gameObject->GetCollider()->Intersects(rayOrigin, rayDir, OUT distance) == false)
			continue;

		if (distance < minDistance)
		{
			minDistance = distance;
			picked = gameObject;
		}
	}

	return picked;
}

void SceneManager::SetUIIndex(int num)
{
	if (nullptr != m_blurUI)
		m_blurUI->SetLayerIndex(num);
}
