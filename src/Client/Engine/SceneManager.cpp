#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"

#include "Engine.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"

#include "TestCameraScript.h"

void CSceneManager::Update()
{
	if (m_activeScene == nullptr)
		return;

	m_activeScene->Update();
	m_activeScene->LateUpdate();
	m_activeScene->FinalUpdate();
}

void CSceneManager::Render()
{
	if (m_activeScene == nullptr)
		return;

	const std::vector<std::shared_ptr<CGameObject>>& gameObjects = m_activeScene->GetGameObjects();
	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetCamera() == nullptr)
			continue;

		gameObject->GetCamera()->Render();
	}
}

void CSceneManager::LoadScene(std::wstring sceneName)
{
	// TODO : 기존 Scene 정리
	// TODO : 파일에서 Scene 정보 로드

	m_activeScene = LoadTestScene();

	m_activeScene->Awake();
	m_activeScene->Start();
}

std::shared_ptr<CScene> CSceneManager::LoadTestScene()
{
	// Scene 객체 생성
	std::shared_ptr<CScene> scene = std::make_shared<CScene>();

#pragma region TestObject
	// TestObject 생성
	std::shared_ptr<CGameObject> gameObject = std::make_shared<CGameObject>();

	// 사각형 정점 4개(버텍스 버퍼) 생성, 위치 / 색상 / uv값 세팅
	std::vector<Vertex> vec(4);
	vec[0].pos = Vec3(-0.5f, 0.5f, 0.5f);
	vec[0].color = Vec4(1.f, 0.f, 0.f, 1.f);
	vec[0].uv = Vec2(0.f, 0.f);
	vec[1].pos = Vec3(0.5f, 0.5f, 0.5f);
	vec[1].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vec[1].uv = Vec2(1.f, 0.f);
	vec[2].pos = Vec3(0.5f, -0.5f, 0.5f);
	vec[2].color = Vec4(0.f, 0.f, 1.f, 1.f);
	vec[2].uv = Vec2(1.f, 1.f);
	vec[3].pos = Vec3(-0.5f, -0.5f, 0.5f);
	vec[3].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vec[3].uv = Vec2(0.f, 1.f);

	// 인덱스 버퍼 생성
	std::vector<uint32> indexVec;
	{
		indexVec.push_back(0);
		indexVec.push_back(1);
		indexVec.push_back(2);
	}
	{
		indexVec.push_back(0);
		indexVec.push_back(2);
		indexVec.push_back(3);
	}


	// Transform 컴포넌트 생성후 오브젝트에 추가
	gameObject->AddComponent(std::make_shared<CTransform>());

	// 오브젝트 존재하는 Transform 컴포넌트를 가져와 값을 넣어줌
	std::shared_ptr<CTransform> transform = gameObject->GetTransform();
	transform->SetLocalPosition(Vec3(0.f, 100.f, 200.f));
	transform->SetLocalScale(Vec3(100.f, 100.f, 1.f));

	// MeshRenderer 컴포넌트 생성
	std::shared_ptr<CMeshRenderer> meshRenderer = std::make_shared<CMeshRenderer>();

	{
		// 메쉬 생성및 컴포넌트에 삽입
		std::shared_ptr<CMesh> mesh = std::make_shared<CMesh>();
		mesh->Init(vec, indexVec);
		meshRenderer->SetMesh(mesh);
	}

	{
		std::shared_ptr<CShader> shader = std::make_shared<CShader>();
		std::shared_ptr<CTexture> texture = std::make_shared<CTexture>();
		
		shader->Init(L"..\\Resources\\Shader\\default.hlsli");
		texture->Init(L"..\\Resources\\Texture\\blue_archive_celebration.png");

		std::shared_ptr<CMaterial> material = std::make_shared<CMaterial>();
		material->SetShader(shader);
		material->SetFloat(0, 0.3f);
		material->SetFloat(1, 0.4f);
		material->SetFloat(2, 0.3f);
		material->SetTexture(0, texture);
		meshRenderer->SetMaterial(material);
	}

	gameObject->AddComponent(meshRenderer);

	scene->AddGameObject(gameObject);
#pragma endregion


#pragma region Camera
	std::shared_ptr<CGameObject> camera = std::make_shared<CGameObject>();
	camera->AddComponent(std::make_shared<CTransform>());
	camera->AddComponent(std::make_shared<CCamera>()); // Near=1, Far=1000, FOV=45도
	camera->AddComponent(std::make_shared<CTestCameraScript>());
	camera->GetTransform()->SetLocalPosition(Vec3(0.f, 100.f, 0.f));
	scene->AddGameObject(camera);
#pragma endregion

	return scene;
}