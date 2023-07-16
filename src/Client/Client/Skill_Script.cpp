#include "pch.h"
#include "Skill_Script.h"

#include "Timer.h"
#include "GameObject.h"

#include "MeshRenderer.h"
#include "Material.h"

Skill_Script::Skill_Script()
{
}

Skill_Script::~Skill_Script()
{
}

void Skill_Script::Awake()
{
}

void Skill_Script::Start()
{
}

void Skill_Script::Update()
{
	m_accTime += (DELTA_TIME * m_speed);

	uint32 size = GetMeshRenderer()->GetMaterialSize();

	for (uint32 i = 0; i < size; ++i)
	{
		GetMeshRenderer()->GetMaterial(i)->SetFloat(2, m_fAlpha);
	}
}

void Skill_Script::LateUpdate()
{
	// 지속시간이 축척되고 있는 시간보다 크다면
	if (m_duration < m_accTime)
	{
		++m_curCount;
		m_accTime = 0.f;
	}

	// 현재 반복되고 있는 횟수가 최대 반복 횟수보다 크거나 같을 경우
	if (m_curCount >= m_repeatCount)
	{
		GetGameObject()->SetDead(true);
	}
}
