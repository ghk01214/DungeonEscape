#include "pch.h"
#include "BillBoard.h"

#include "Camera.h"
#include "Resources.h"
#include "MeshRenderer.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Transform.h"

BillBoard::BillBoard()
{
}

BillBoard::~BillBoard()
{
}

void BillBoard::Awake()
{
}

void BillBoard::Start()
{
}

void BillBoard::Update()
{
	__super::Update();

	// 카메라가 있는지 확인
	shared_ptr<Scene> curScene = GET_SINGLE(SceneManager)->GetActiveScene();

	shared_ptr<Camera> camera = nullptr;

	if (nullptr != curScene)
	{
		camera = curScene->GetMainCamera();
	}

	// 카메라가 존재한다면
	if (nullptr != camera)
	{
		shared_ptr<Transform> transform = GetTransform();
		Vec3 vTargetPos = camera->GetTransform()->GetWorldPosition();
		Vec3 vPos = transform->GetWorldPosition();

		Vec3 Up = camera->GetTransform()->GetLocalToWorldMatrix().Up();

		Matrix matWorld = Matrix::CreateLookAt(vPos, vTargetPos, Up);

		// 월드 Matrix 세팅
		//transform-
	}

	// 시간을 누적한다.
	m_fAccumulateTime += DELTA_TIME;

	shared_ptr<MeshRenderer> meshRenderer = GetMeshRenderer();

	// 메쉬 렌더러가 존재할 경우
	if (nullptr != meshRenderer)
	{
		// 누적 시간이 넘어가는 시간보다 많아질 경우
		if (m_fAccumulateTime > m_fPassingTime)
		{
			m_curTextureNumber = (m_curTextureNumber + 1) % m_textures.size();

			// 텍스쳐를 바꾼다.
			uint32 size = meshRenderer->GetMaterialSize();
			for (uint32 i = 0; i < size; ++i)
			{
				meshRenderer->GetMaterial(i)->SetTexture(0, m_textures[m_curTextureNumber]);
			}
		}
		
		// 누적 시간 초기화
		m_fAccumulateTime = 0.f;
	}
}

void BillBoard::LateUpdate()
{
	__super::LateUpdate();
}

void BillBoard::FinalUpdate()
{
	__super::FinalUpdate();
}

void BillBoard::SetBBInfo(BB_TYPE eType, std::vector<shared_ptr<class Texture>> textures, float m_fTime)
{
	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();

	meshRenderer->SetMesh(mesh);
	shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Texture");

	shared_ptr<Texture> texture;
	shared_ptr<Material> material = make_shared<Material>();
	material->SetShader(shader);
	material->SetTexture(0, texture);
	meshRenderer->SetMaterial(material);
}
