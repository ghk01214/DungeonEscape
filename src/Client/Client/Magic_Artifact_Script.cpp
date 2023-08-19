#include "pch.h"
#include "Magic_Artifact_Script.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Timer.h"
#include "Material.h"
#include "Input.h"

Magic_Artifact_Script::Magic_Artifact_Script()
{
}

Magic_Artifact_Script::~Magic_Artifact_Script()
{
}

void Magic_Artifact_Script::Start()
{
}

void Magic_Artifact_Script::Update()
{
	if (m_bFade)
	{
		if (m_fAlpha <= 0.f)
		{
			m_fAlpha = 0.f;
			return;
		}

		m_fAlpha -= DELTA_TIME * (1.f / m_fFadeSpeed);
	}

	shared_ptr<Transform> transform = GetTransform();

	// 크기 x 회전 x 이동 x 회전
	transform->SetLocalScale(Vec3(m_size.x, m_size.y, 1.f));
	Matrix matWorld =
		Matrix::CreateTranslation(Vec3(0.f, 0.f, -m_distanceFromPoint)) *
		Matrix::CreateRotationY(XMConvertToRadians(m_rotation)) *
		Matrix::CreateTranslation(m_targetPos);

	// 월드 Matrix 세팅
	transform->SetWorldMatrix(matWorld);



	// 회전 관련 설정
	m_rotation += m_rotationSpeed * DELTA_TIME;
	if (m_rotation > 360.f)
		m_rotation = fmodf(m_rotation, 360.f);





	// 시간을 누적
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

			// 누적 시간 초기화
			m_fAccumulateTime = 0.f;
		}
	}

	uint32 size = meshRenderer->GetMaterialSize();
	for (uint32 i = 0; i < size; ++i)
	{
		meshRenderer->GetMaterial(i)->SetFloat(2, m_fAlpha);
	}
}

void Magic_Artifact_Script::LateUpdate()
{
	static int curAxis = 0;

	if (GET_SINGLE(CInput)->GetButton(KEY_TYPE::KEY_6))
		curAxis = 1;
	if (GET_SINGLE(CInput)->GetButton(KEY_TYPE::KEY_7))
		curAxis = 2;
	if (GET_SINGLE(CInput)->GetButton(KEY_TYPE::KEY_8))
		curAxis = 3;

	if (GET_SINGLE(CInput)->GetButton(KEY_TYPE::KEY_9))
	{
		switch (curAxis)
		{
			case 1:
			m_targetPos.x -= DELTA_TIME * 1000.f;
			break;
			case 2:
			m_targetPos.y -= DELTA_TIME * 1000.f;
			break;
			case 3:
			m_targetPos.z -= DELTA_TIME * 1000.f;
			break;
			default:
			break;
		}
	}
	if (GET_SINGLE(CInput)->GetButton(KEY_TYPE::KEY_0))
	{
		switch (curAxis)
		{
			case 1:
			m_targetPos.x += DELTA_TIME * 1000.f;
			break;
			case 2:
			m_targetPos.y += DELTA_TIME * 1000.f;
			break;
			case 3:
			m_targetPos.z += DELTA_TIME * 1000.f;
			break;
			default:
			break;
		}
	}
}
