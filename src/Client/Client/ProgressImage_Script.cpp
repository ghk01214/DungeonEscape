#include "pch.h"
#include "ProgressImage_Script.h"

#include <MeshRenderer.h>
#include <Material.h>
#include <UI.h>
#include <Timer.h>
#include <Transform.h>

ProgressImage_Script::ProgressImage_Script() :
	m_pos{ 0.f },
	m_fadeIn{ false },
	m_fadeOut{ false },
	m_fadeTime{ 0.f }
{
}

ProgressImage_Script::~ProgressImage_Script()
{
}

void ProgressImage_Script::Awake()
{
	__super::Awake();
}

void ProgressImage_Script::Start()
{
	__super::Start();

	//m_alpha = 0.f;
	auto pos{ GetRatio(0.f, 0.f) };
	m_pos = Vec3{ pos.x, pos.y, 400.f };
	ChangeObjectAlpha();
}

void ProgressImage_Script::Update()
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
		m_pos.x -= DELTA_TIME * (1.f / m_fadeTime);

		if (m_pos.x < GetRatio(-150.f, 0.f).x)
		{
			m_alpha = 1.f;
			m_fadeOut = false;
			m_pos.x = GetRatio(0.f, 0.f).x;
			SetVisible(false);
		}
	}

	PrintVec3(m_pos);
	GetTransform()->SetLocalPosition(m_pos);
	ChangeObjectAlpha();
}

void ProgressImage_Script::LateUpdate()
{
	__super::LateUpdate();
}

void ProgressImage_Script::ChangeImage(ICON_TYPE index)
{
	uint32_t size{ GetMeshRenderer()->GetMaterialSize() };
	for (int32_t i = 0; i < size; ++i)
	{
		GetMeshRenderer()->GetMaterial(i)->SetTexture(0, m_textures[index]);
	}
}

void ProgressImage_Script::ChangeImage(int32_t index)
{
	uint32_t size{ GetMeshRenderer()->GetMaterialSize() };
	for (int32_t i = 0; i < size; ++i)
	{
		GetMeshRenderer()->GetMaterial(i)->SetTexture(0, m_textures[index]);
	}
}

void ProgressImage_Script::FadeIn(float time)
{
	GetUI()->SetVisible(true);
	m_fadeIn = true;
	m_fadeTime = time;
	m_alpha = 0.f;
}

void ProgressImage_Script::FadeOut(float time)
{
	m_fadeOut = true;
	m_fadeTime = time;
}

void ProgressImage_Script::SetVisible(bool flag)
{
	GetUI()->SetVisible(flag);
}
