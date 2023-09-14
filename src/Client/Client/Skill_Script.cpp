#include "pch.h"
#include "Skill_Script.h"

#include <Timer.h>
#include <GameObject.h>
#include <MeshRenderer.h>
#include <Material.h>
#include <Transform.h>

Skill_Script::Skill_Script() :
	m_repeatCount{ 1 },
	m_curCount{ 0 },
	m_duration{ 5.f },
	m_accTime{ 0.f },
	m_speed{ 1.f },
	m_alpha{ 1.f },
	m_scale{ 1.f }
{
}

Skill_Script::~Skill_Script()
{
}

void Skill_Script::Awake()
{
	__super::Awake();

	ChangeAlpha();
}

void Skill_Script::Start()
{
	__super::Start();
}

void Skill_Script::Update()
{
	__super::Update();

	m_accTime += DELTA_TIME;
}

void Skill_Script::LateUpdate()
{
	__super::LateUpdate();

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
		*m_recv = false;
	}
}

void Skill_Script::ChangeTransform(Vec3& scale)
{
	Matrix mat{ Matrix::CreateScale(scale) };
	mat *= Matrix::CreateTranslation(GetTransform()->GetWorldPosition());
	GetTransform()->SetWorldMatrix(mat);
}

void Skill_Script::ChangeAlpha()
{
	for (uint32_t i = 0; i < GetMeshRenderer()->GetMaterialSize(); ++i)
	{
		GetMeshRenderer()->GetMaterial(i)->SetFloat(2, m_alpha);
	}
}

void Skill_Script::SetRecvFlag(std::shared_ptr<bool> flag)
{
	m_recv = flag;
}
