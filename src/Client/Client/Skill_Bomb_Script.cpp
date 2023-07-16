#include "pch.h"
#include "Skill_Bomb_Script.h"

#include "Timer.h"

Bomb_Script::Bomb_Script()
{
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

	m_duration = 20.f;
}

void Bomb_Script::Update()
{
	__super::Update();

	m_fAlpha += (m_AlphaChangeSpeed * m_AlphaChangeDegree * DELTA_TIME);

	if (m_fAlpha < 0)
		m_fAlpha = 0.f;
}

void Bomb_Script::LateUpdate()
{
	__super::LateUpdate();
}