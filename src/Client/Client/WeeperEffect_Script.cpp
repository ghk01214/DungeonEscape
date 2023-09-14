#include "pch.h"
#include "WeeperEffect_Script.h"

#include <Transform.h>
#include <MeshRenderer.h>
#include <Timer.h>
#include <Material.h>
#include <Input.h>

WeeperEffect_Script::WeeperEffect_Script() :
	m_passingTime{ 1.f },
	m_accTime{ 0.f },
	m_curTextureNumber{ 0 },
	m_alpha{ 0.f },
	m_size{ 100.f },
	m_targetPos{ 0.f },
	m_distanceFromPoint{ 100.f },
	m_rotation{ 0.f },
	m_rotationSpeed{ 90.f },
	m_fadeIn{ false },
	m_fadeOut{ false },
	m_fadeSpeed{ 1.f },
	m_active{ true }
{
}

WeeperEffect_Script::~WeeperEffect_Script()
{
}

void WeeperEffect_Script::Start()
{
}

void WeeperEffect_Script::Update()
{
	if (m_active == false)
		return;

	if (m_fadeIn == true)
	{
		m_alpha += DELTA_TIME * (1.f / m_fadeSpeed);

		if (m_alpha >= 1.f)
		{
			m_alpha = 1.f;
			m_fadeIn = false;
			m_fadeOut = false;
		}
	}
	else if (m_fadeOut == true)
	{
		m_alpha -= DELTA_TIME * (1.f / m_fadeSpeed);

		if (m_alpha <= 0.f)
		{
			m_alpha = 0.f;
			m_fadeIn = false;
			m_fadeOut = false;
		}
	}

	auto transform{ GetTransform() };
	// look 방향으로 이동
	//auto look{ transform->GetLook() };
	//look.Normalize();
	//m_targetPos -= look * 100.f;

	// 크기 x 회전 x 이동 x 회전
	transform->SetLocalScale(Vec3{ m_size.x, m_size.y, 1.f });
	Matrix matWorld{ Matrix::CreateTranslation(Vec3{ 0.f, 0.f, -m_distanceFromPoint }) *
					 Matrix::CreateRotationY(XMConvertToRadians(m_rotation)) *
					 Matrix::CreateTranslation(m_targetPos) };

	// 월드 Matrix 세팅
	transform->SetWorldMatrix(matWorld);

	// 회전 관련 설정
	m_rotation += m_rotationSpeed * DELTA_TIME;

	if (m_rotation > 360.f)
		m_rotation = std::fmodf(m_rotation, 360.f);

	// 시간을 누적
	m_accTime += DELTA_TIME;

	auto meshRenderer{ GetMeshRenderer() };

	// 메쉬 렌더러가 존재할 경우
	if (meshRenderer != nullptr)
	{
		// 누적 시간이 넘어가는 시간보다 많아질 경우
		if (m_accTime > m_passingTime)
		{
			m_curTextureNumber = (m_curTextureNumber + 1) % m_textures.size();

			// 텍스쳐를 바꾼다.
			uint32_t size{ meshRenderer->GetMaterialSize() };
			for (uint32_t i = 0; i < size; ++i)
			{
				meshRenderer->GetMaterial(i)->SetTexture(0, m_textures[m_curTextureNumber]);
			}

			// 누적 시간 초기화
			m_accTime = 0.f;
		}
	}

	uint32_t size{ meshRenderer->GetMaterialSize() };
	for (uint32_t i = 0; i < size; ++i)
	{
		meshRenderer->GetMaterial(i)->SetFloat(2, m_alpha);
	}
}

void WeeperEffect_Script::LateUpdate()
{
	//static int curAxis = 0;

	//if (GET_SINGLE(CInput)->GetButton(KEY_TYPE::NUM_6))
	//	curAxis = 1;
	//if (GET_SINGLE(CInput)->GetButton(KEY_TYPE::NUM_7))
	//	curAxis = 2;
	//if (GET_SINGLE(CInput)->GetButton(KEY_TYPE::NUM_8))
	//	curAxis = 3;

	//if (GET_SINGLE(CInput)->GetButton(KEY_TYPE::NUM_9))
	//{
	//	switch (curAxis)
	//	{
	//		case 1:
	//		m_targetPos.x -= DELTA_TIME * 1000.f;
	//		break;
	//		case 2:
	//		m_targetPos.y -= DELTA_TIME * 1000.f;
	//		break;
	//		case 3:
	//		m_targetPos.z -= DELTA_TIME * 1000.f;
	//		break;
	//		default:
	//		break;
	//	}
	//}
	//if (GET_SINGLE(CInput)->GetButton(KEY_TYPE::NUM_0))
	//{
	//	switch (curAxis)
	//	{
	//		case 1:
	//		m_targetPos.x += DELTA_TIME * 1000.f;
	//		break;
	//		case 2:
	//		m_targetPos.y += DELTA_TIME * 1000.f;
	//		break;
	//		case 3:
	//		m_targetPos.z += DELTA_TIME * 1000.f;
	//		break;
	//		default:
	//		break;
	//	}
	//}
}

void WeeperEffect_Script::SetTexture(std::vector<shared_ptr<class Texture>> textures)
{
	m_textures = textures;
}

void WeeperEffect_Script::SetStartRotation(float startRotation)
{
	m_rotation = startRotation;
};

void WeeperEffect_Script::SetRotationSpeed(float rotationSpeed)
{
	m_rotationSpeed = rotationSpeed;
}

void WeeperEffect_Script::SetDistanceFromPoint(float size)
{
	m_distanceFromPoint = size;
}

void WeeperEffect_Script::SetTargetPoint(Vec3 point)
{
	m_targetPos = point;
}

void WeeperEffect_Script::SetAlpha(float alpha)
{
	m_alpha = alpha;
}

void WeeperEffect_Script::SetPassingTime(float time)
{
	m_passingTime = time;
}

void WeeperEffect_Script::SetActive(bool flag)
{
	m_active = flag;
}

void WeeperEffect_Script::FadeIn(float fadeSpeed)
{
	// 1.f는 1초에 걸쳐, 2.f는 2초에 걸쳐 fade됨.
	m_fadeIn = true;
	m_fadeSpeed = fadeSpeed;
}

void WeeperEffect_Script::FadeOut(float fadeSpeed)
{
	// 1.f는 1초에 걸쳐, 2.f는 2초에 걸쳐 fade됨.
	m_fadeOut = true;
	m_fadeSpeed = fadeSpeed;
}
