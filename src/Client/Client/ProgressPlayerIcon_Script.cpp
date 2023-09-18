#include "pch.h"
#include "ProgressPlayerIcon_Script.h"

#include <MeshRenderer.h>
#include <Material.h>
#include <UI.h>
#include <Timer.h>
#include <Transform.h>

ProgressPlayerIcon_Script::ProgressPlayerIcon_Script() :
	m_originPos{},
	m_currPos{ 0.f },
	m_fadeTime{ 0.f },
	m_fadeIn{ false },
	m_fadeOut{ false },
	m_sign{ 1 }
{
}

ProgressPlayerIcon_Script::~ProgressPlayerIcon_Script()
{
}

void ProgressPlayerIcon_Script::Awake()
{
	__super::Awake();
}

void ProgressPlayerIcon_Script::Start()
{
	__super::Start();

	//m_alpha = 0.f;
	ChangeObjectAlpha();
}

void ProgressPlayerIcon_Script::Update()
{
	if (GetUI()->GetVisible() == false)
		return;

	__super::Update();

	if (m_fadeIn == true)
	{
		m_alpha += DELTA_TIME * (1.f / m_fadeTime);

		if (m_alpha >= 1.f)
		{
			m_alpha = 1.f;
			m_fadeIn = false;
		}
	}
	else if (m_fadeOut == true)
	{
		m_currPos.x -= DELTA_TIME * (1.f / m_fadeTime);

		if (m_currPos.x < GetRatio(-150.f, 0.f).x)
		{
			m_alpha = 1.f;
			m_fadeOut = false;
			m_currPos = m_originPos[m_currPhase];
			SetVisible(false);
		}
	}

	m_currPos.y += DELTA_TIME * m_sign * 30.f;

	if (m_currPos.y > m_originPos[m_currPhase].y + 15.f)
	{
		m_currPos.y = m_originPos[m_currPhase].y + 15.f;
		m_sign = -1;
	}
	else if (m_currPos.y < m_originPos[m_currPhase].y - 15.f)
	{
		m_currPos.y = m_originPos[m_currPhase].y - 15.f;
		m_sign = 1;
	}

	GetTransform()->SetLocalPosition(m_currPos);
	ChangeObjectAlpha();
}

void ProgressPlayerIcon_Script::LateUpdate()
{
	if (GetUI()->GetVisible() == false)
		return;

	__super::LateUpdate();
}

void ProgressPlayerIcon_Script::SetOriginPos(Vec3 pos)
{
	m_originPos[m_currPhase] = pos;
}

void ProgressPlayerIcon_Script::SetCurrentPos(Vec3 pos)
{
	m_currPos = pos;
}

void ProgressPlayerIcon_Script::SetVisible(bool flag)
{
	GetUI()->SetVisible(flag);
}

void ProgressPlayerIcon_Script::SetCurrentPhase(int32_t index)
{
	m_currPhase = index;
}

void ProgressPlayerIcon_Script::AddOriginPos(Vec3 pos)
{
	m_originPos.push_back(pos);
}

void ProgressPlayerIcon_Script::FadeIn(float time)
{
	GetUI()->SetVisible(true);
	m_fadeIn = true;
	m_fadeOut = false;
	m_fadeTime = time;
	m_alpha = 0.f;
}

void ProgressPlayerIcon_Script::FadeOut(float time)
{
	m_fadeIn = false;
	m_fadeOut = true;
	m_fadeTime = time;
}

void ProgressPlayerIcon_Script::ChangeCurrentPos(int32_t index)
{
	m_currPos = m_originPos[index];
}
