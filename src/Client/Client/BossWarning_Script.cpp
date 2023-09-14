#include "pch.h"
#include "BossWarning_Script.h"

#include <MeshRenderer.h>
#include <Material.h>
#include <Timer.h>

BossWarning_Script::BossWarning_Script() :
	m_render{ false },
	m_accTime{ 0.f },
	m_fadeDuration{ 2.f },
	m_renderDuration{ 2.f },
	m_fadeType{ FADE_TYPE::NONE },
	m_blinkSpeed{ 1.f },
	m_maxAlpha{ 1.f },
	m_blinkCount{ 0 }
{
	m_alpha = 0.f;
}

BossWarning_Script::~BossWarning_Script()
{
}

void BossWarning_Script::Awake()
{
	__super::Awake();

	ChangeObjectAlpha();
}

void BossWarning_Script::Start()
{
	__super::Start();
}

void BossWarning_Script::Update()
{
	if (m_render == false)
		return;

	__super::Update();

	m_accTime += DELTA_TIME * m_blinkSpeed;

	if (m_fadeType == FADE_TYPE::FADE_IN)
	{
		if (m_accTime < m_fadeDuration)
		{
			m_alpha = m_accTime / m_fadeDuration;

			if (m_alpha >= m_maxAlpha)
				m_alpha = m_maxAlpha;
		}
		else
		{
			m_fadeType = FADE_TYPE::FADE_OUT;
			m_accTime = 0.f;
		}
	}
	else if (m_fadeType == FADE_TYPE::FADE_OUT)
	{
		if (m_accTime < m_fadeDuration)
		{
			m_alpha = m_maxAlpha - m_accTime / m_fadeDuration;

			if (m_alpha <= 0.f)
				m_alpha = 0.f;
		}
		else
		{
			m_fadeType = FADE_TYPE::FADE_IN;
			m_accTime = 0.f;
			++m_blinkCount;

			if (m_blinkCount >= 4)
			{
				m_blinkCount = 0.f;
				m_fadeType = FADE_TYPE::NONE;
				m_render = false;
			}
		}
	}

	uint32_t size = GetMeshRenderer()->GetMaterialSize();

	for (uint32_t i = 0; i < size; ++i)
	{
		GetMeshRenderer()->GetMaterial(i)->SetFloat(2, m_alpha);
	}
}

void BossWarning_Script::LateUpdate()
{
	__super::LateUpdate();
}

void BossWarning_Script::StartBlink(float fadeDuration, float renderDuration, float blinkSpeed, float maxAlpha)
{
	m_render = true;
	m_fadeType = FADE_TYPE::FADE_IN;
	m_fadeDuration = fadeDuration;
	m_renderDuration = renderDuration;
	m_blinkSpeed = blinkSpeed;
	m_maxAlpha = maxAlpha;
}

void BossWarning_Script::SetData(float fadeDuration, float renderDuration, float blinkSpeed, float maxAlpha)
{
	SetTimeData(fadeDuration, renderDuration);
	SetSpeedDate(blinkSpeed, maxAlpha);
}

void BossWarning_Script::SetTimeData(float fadeDuration, float renderDuration)
{
	m_fadeDuration = fadeDuration;
	m_renderDuration = renderDuration;
}

void BossWarning_Script::SetSpeedDate(float blinkSpeed, float maxAlpha)
{
	m_blinkSpeed = blinkSpeed;
	m_maxAlpha = maxAlpha;
}
