#include "pch.h"
#include "BossHP_Script.h"

#include <Timer.h>
#include <Transform.h>

BossHP_Script::BossHP_Script(int32_t hp) :
	m_currHP{ hp },
	m_prevHP{ hp },
	m_hpRatio{ hp / 100.f }
{
}

BossHP_Script::~BossHP_Script()
{
}

void BossHP_Script::Awake()
{
	__super::Awake();
}

void BossHP_Script::Start()
{
	__super::Start();

	float leftPos{ m_pos.x - m_scale.x / 2.f };
	m_barLengthRatio = (m_pos.x - leftPos) / 100.f;
}

void BossHP_Script::Update()
{
	__super::Update();

	ChangeHPTransform();
}

void BossHP_Script::LateUpdate()
{
	__super::LateUpdate();
}

void BossHP_Script::ChangeHPTransform()
{
	if (m_currHP <= 0.f)
		return;

	if (m_prevHP == m_currHP)
		return;

	int32_t hpRatio{ m_prevHP - m_currHP };
	float hpMultiple{ hpRatio / m_hpRatio };

	m_pos.x -= m_barLengthRatio * hpMultiple;
	m_scale.x -= m_barLengthRatio * hpMultiple * 2.f;

	GetTransform()->SetLocalPosition(m_pos);
	GetTransform()->SetLocalScale(m_scale);

	m_prevHP = m_currHP;
}

void BossHP_Script::SetHP(int32_t hp)
{
	m_currHP = hp;
}

void BossHP_Script::SetByDamage(int32_t damage)
{
	m_currHP -= damage;
}
