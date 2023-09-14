#include "pch.h"
#include "OneTimeDialogue_Script.h"

#include <MeshRenderer.h>
#include <Material.h>
#include <Timer.h>

OneTimeDialogue_Script::OneTimeDialogue_Script(const std::string& type) :
	m_render{ false },
	m_accTime{ 0.f },
	m_fadeDuration{ 2.f },
	m_renderDuration{ 2.f },
	m_fadeType{ FADE_TYPE::NONE },
	m_dialogueType{ magic_enum::enum_cast<DIALOGUE_TYPE>(type).value() }
{
	m_alpha = 0.f;
}

OneTimeDialogue_Script::~OneTimeDialogue_Script()
{
}

void OneTimeDialogue_Script::Awake()
{
	__super::Awake();

	ChangeObjectAlpha();
}

void OneTimeDialogue_Script::Start()
{
	__super::Start();
}

void OneTimeDialogue_Script::Update()
{
	if (m_render == false)
		return;

	__super::Update();

	m_accTime += DELTA_TIME;

	if (m_fadeType == FADE_TYPE::FADE_IN)
	{
		if (m_accTime < m_fadeDuration)
		{
			m_alpha = m_accTime / m_fadeDuration;

			if (m_alpha >= 1.f)
				m_alpha = 1.f;
		}
		else
		{
			m_fadeType = FADE_TYPE::NONE;
			m_accTime = 0.f;
		}
	}
	else if (m_fadeType == FADE_TYPE::FADE_OUT)
	{
		if (m_accTime < m_fadeDuration)
		{
			m_alpha = 1.f - m_accTime / m_fadeDuration;

			if (m_alpha <= 0.f)
				m_alpha = 0.f;
		}
		else
		{
			m_fadeType = FADE_TYPE::NONE;
			m_accTime = 0.f;
			m_render = false;
		}
	}
	else
	{
		if (m_accTime >= m_renderDuration)
		{
			m_fadeType = FADE_TYPE::FADE_OUT;
			m_accTime = 0.f;
		}
	}

	uint32_t size = GetMeshRenderer()->GetMaterialSize();

	for (uint32_t i = 0; i < size; ++i)
	{
		GetMeshRenderer()->GetMaterial(i)->SetFloat(2, m_alpha);
	}
}

void OneTimeDialogue_Script::LateUpdate()
{
	__super::LateUpdate();
}

void OneTimeDialogue_Script::StartRender(float fadeDuration, float renderDuration)
{
	m_render = true;
	m_fadeType = FADE_TYPE::FADE_IN;
	m_fadeDuration = fadeDuration;
	m_renderDuration = renderDuration;
}

void OneTimeDialogue_Script::SetTimeData(float fadeDuration, float renderDuration)
{
	m_fadeDuration = fadeDuration;
	m_renderDuration = renderDuration;
}
