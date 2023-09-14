#include "pch.h"
#include "BossCounterEffect_Script.h"

#include <Transform.h>
#include <MeshRenderer.h>
#include <Timer.h>
#include <Material.h>
#include <Input.h>

BossCounterEffect_Script::BossCounterEffect_Script() :
	m_passingTime{ 1.f },
	m_accTime{ 0.f },
	m_curTextureNumber{ 0 },
	m_alpha{ 0.f },
	m_pos{ 0.f },
	m_size{ 100.f },
	m_fadeIn{ false },
	m_fadeOut{ false },
	m_fadeSpeed{ 1.f },
	m_active{ true }
{
}

BossCounterEffect_Script::~BossCounterEffect_Script()
{
	__super::Awake();
}

void BossCounterEffect_Script::Start()
{
	__super::Start();

	Matrix matWorld{ Matrix::CreateRotationX(XMConvertToRadians(90.f)) };
	matWorld *= Matrix::CreateTranslation(m_pos);

	GetTransform()->SetWorldMatrix(matWorld);
}

void BossCounterEffect_Script::Update()
{
	if (m_active == false)
		return;

	__super::Update();

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

	transform->SetLocalScale(Vec3{ m_size.x, m_size.y, 1.f });
	Matrix matWorld{ transform->GetWorldMatrix() };
	matWorld.Translation(m_pos);

	// 월드 Matrix 세팅
	transform->SetWorldMatrix(matWorld);

	// 시간을 누적
	m_accTime += DELTA_TIME;

	auto meshRenderer{ GetMeshRenderer() };

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

	uint32_t size{ meshRenderer->GetMaterialSize() };
	for (uint32_t i = 0; i < size; ++i)
	{
		meshRenderer->GetMaterial(i)->SetFloat(2, m_alpha);
	}
}

void BossCounterEffect_Script::LateUpdate()
{
	__super::LateUpdate();
}

void BossCounterEffect_Script::SetTexture(std::vector<shared_ptr<class Texture>> textures)
{
	m_textures = textures;
}

void BossCounterEffect_Script::SetPos(Vec3 pos)
{
	m_pos = pos;
}

void BossCounterEffect_Script::SetSize(Vec2 size)
{
	m_size = size;
}

void BossCounterEffect_Script::SetAlpha(float alpha)
{
	m_alpha = alpha;
}

void BossCounterEffect_Script::SetPassingTime(float time)
{
	m_passingTime = time;
}

void BossCounterEffect_Script::SetActive(bool flag)
{
	m_active = flag;
}

void BossCounterEffect_Script::FadeIn(float fadeSpeed)
{
	// 1.f는 1초에 걸쳐, 2.f는 2초에 걸쳐 fade됨.
	m_fadeIn = true;
	m_fadeSpeed = fadeSpeed;
}

void BossCounterEffect_Script::FadeOut(float fadeSpeed)
{
	// 1.f는 1초에 걸쳐, 2.f는 2초에 걸쳐 fade됨.
	m_fadeOut = true;
	m_fadeSpeed = fadeSpeed;
}
