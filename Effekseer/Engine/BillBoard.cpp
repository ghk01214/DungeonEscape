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

	// ī�޶� �ִ��� Ȯ��
	shared_ptr<Scene> curScene = GET_SINGLE(SceneManager)->GetActiveScene();

	shared_ptr<Camera> camera = nullptr;

	if (nullptr != curScene)
	{
		camera = curScene->GetMainCamera();
	}

	// ī�޶� �����Ѵٸ�
	if (nullptr != camera)
	{
		shared_ptr<Transform> transform = GetTransform();
		Vec3 vTargetPos = camera->GetTransform()->GetWorldPosition();
		Vec3 vPos = transform->GetWorldPosition();

		Vec3 Up = camera->GetTransform()->GetLocalToWorldMatrix().Up();

		Matrix matWorld = Matrix::CreateLookAt(vPos, vTargetPos, Up);

		// ���� Matrix ����
		//transform-
	}

	// �ð��� �����Ѵ�.
	m_fAccumulateTime += DELTA_TIME;

	shared_ptr<MeshRenderer> meshRenderer = GetMeshRenderer();

	// �޽� �������� ������ ���
	if (nullptr != meshRenderer)
	{
		// ���� �ð��� �Ѿ�� �ð����� ������ ���
		if (m_fAccumulateTime > m_fPassingTime)
		{
			m_curTextureNumber = (m_curTextureNumber + 1) % m_textures.size();

			// �ؽ��ĸ� �ٲ۴�.
			uint32 size = meshRenderer->GetMaterialSize();
			for (uint32 i = 0; i < size; ++i)
			{
				meshRenderer->GetMaterial(i)->SetTexture(0, m_textures[m_curTextureNumber]);
			}
		}
		
		// ���� �ð� �ʱ�ȭ
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
