#include "pch.h"
#include "Skill_Bomb_Script.h"

#include "Timer.h"
#include <Transform.h>

Bomb_Script::Bomb_Script(float scaleSpeed, float alpha) :
	m_alphaChangeSpeed{ 0.5f },
	m_alphaChangeDegree{ -1.f },
	m_scaleChangeSpeed{ scaleSpeed }
{
	m_alpha = alpha;
	//m_duration = 5.f;
	m_duration = 100.f;
}

Bomb_Script::~Bomb_Script()
{
}

void Bomb_Script::Awake()
{
	__super::Awake();
}

void Bomb_Script::Start()
{
	__super::Start();
}

void Bomb_Script::Update()
{
	__super::Update();

	m_scale += Vec3{ DELTA_TIME * m_scaleChangeSpeed };
	m_alpha += (m_alphaChangeSpeed * m_alphaChangeDegree * DELTA_TIME);

	if (m_alpha < 0.f)
	{
		m_alpha = 1.f;
		m_scale = Vec3{ 1.f };
		m_duration = m_accTime;
	}

	ChangeTransform(m_scale);
	ChangeAlpha();
}

void Bomb_Script::LateUpdate()
{
	__super::LateUpdate();
}