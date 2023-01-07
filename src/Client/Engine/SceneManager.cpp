#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"

#include "Engine.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"

#include "Input.h"
#include "TestCameraScript.h"

void CSceneManager::Update()
{
	if (m_activeScene == nullptr)
		return;

	m_activeScene->Update();
	m_activeScene->LateUpdate();
	m_activeScene->FinalUpdate();

	KeyInput();
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

	// 해당 씬에 추가할 오브젝트 생성
#pragma region CreateObjects
	// 함수 인자로 텍스쳐 경로, 위치, 사이즈 값 입력
	CreateObject(scene, L"..\\Resources\\Texture\\blue_archive_celebration.png", Vec3(25.f, 25.f, 0.f), Vec3(50.f, 50.f, 0.f));
	CreateObject(scene, L"..\\Resources\\Texture\\blue_archive_celebration.png", Vec3(25.f, -25.f, 0.f), Vec3(50.f, 50.f, 0.f));
	CreateObject(scene, L"..\\Resources\\Texture\\blue_archive_celebration.png", Vec3(-25.f, 25.f, 0.f), Vec3(50.f, 50.f, 0.f));
	CreateObject(scene, L"..\\Resources\\Texture\\blue_archive_celebration.png", Vec3(-25.f, -25.f, 0.f), Vec3(50.f, 50.f, 0.f));
#pragma endregion


#pragma region Camera
	std::shared_ptr<CGameObject> camera = std::make_shared<CGameObject>();
	camera->AddComponent(std::make_shared<CTransform>());
	camera->AddComponent(std::make_shared<CCamera>()); // Near=1, Far=1000, FOV=45도
	camera->AddComponent(std::make_shared<CTestCameraScript>());
	camera->GetTransform()->SetLocalPosition(Vec3(50.f, 50.f, -50.f));
	camera->GetTransform()->SetLocalRotation(Vec3(XMConvertToRadians(45.f), -XMConvertToRadians(45.f), 0.f));		// 시계방향이 +, 반시계방향이 -
	scene->AddGameObject(camera);

	Matrix matWorld = XMMatrixLookAtLH(XMVectorSet(5.f, 5.f, -5.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
#pragma endregion

	return scene;
}

void CSceneManager::CreateObject(std::shared_ptr<CScene> scene, const std::wstring& texturePath, Vec3 vPos, Vec3 vScale)
{
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
	transform->SetLocalPosition(vPos);
	transform->SetLocalScale(vScale);

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
		texture->Init(texturePath);

		std::shared_ptr<CMaterial> material = std::make_shared<CMaterial>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		meshRenderer->SetMaterial(material);
	}

	gameObject->AddComponent(meshRenderer);

	scene->AddGameObject(gameObject);

	// 현재 씬의 게임 오브젝트만 보아놓은 vector에 오브젝트 추가
	m_gameObjects.push_back(gameObject);

	// 생성된 오브젝트를 현재 선택된 오브젝트로 바꿈
	m_targetObject = gameObject;
}

void CSceneManager::KeyInput(void)
{
	// targetObject 바꾸기
	if (INPUT->GetButtonDown(KEY_TYPE::Number1) || INPUT->GetButtonDown(KEY_TYPE::NumPad1))
	{
		if (m_gameObjects.size() >= 1)
		{
			m_targetObject = m_gameObjects.at(0);
		}
	}

	if (INPUT->GetButtonDown(KEY_TYPE::Number2) || INPUT->GetButtonDown(KEY_TYPE::NumPad2))
	{
		if (m_gameObjects.size() >= 2)
		{
			m_targetObject = m_gameObjects.at(1);
		}
	}

	if (INPUT->GetButtonDown(KEY_TYPE::Number3) || INPUT->GetButtonDown(KEY_TYPE::NumPad3))
	{
		if (m_gameObjects.size() >= 3)
		{
			m_targetObject = m_gameObjects.at(2);
		}
	}

	if (INPUT->GetButtonDown(KEY_TYPE::Number4) || INPUT->GetButtonDown(KEY_TYPE::NumPad4))
	{
		if (m_gameObjects.size() >= 4)
		{
			m_targetObject = m_gameObjects.at(3);
		}
	}
}
