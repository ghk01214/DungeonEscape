#include "pch.h"
#include "VolumeSlider_Script.h"

#include <MeshRenderer.h>
#include <Material.h>
#include <Transform.h>
#include <UI.h>
#include <Engine.h>

VolumeSlider_Script::VolumeSlider_Script(bool active)
{
	m_active = active;
}

VolumeSlider_Script::~VolumeSlider_Script()
{
}

void VolumeSlider_Script::Awake()
{
	__super::Awake();
}

void VolumeSlider_Script::Start()
{
	__super::Start();
}

void VolumeSlider_Script::Update()
{
	if (GetUI()->GetVisible() == false)
		return;

	if (m_active == false)
		return;

	int32_t mouseXPos{ GET_SINGLE(CInput)->GetMousePos().x };
	float width{ static_cast<float>(GEngine->GetWindow().width) };
	mouseXPos -= width / 2;

	if (mouseXPos < -183 or mouseXPos > 297)
		return;

	__super::Update();

	ChangeSliderTransform();
}

void VolumeSlider_Script::LateUpdate()
{
	__super::LateUpdate();
}

void VolumeSlider_Script::ChangeMuteTexture(bool mute)
{
	for (int32_t i = 0; i < GetMeshRenderer()->GetMaterialSize(); ++i)
	{
		if (mute == true)
			GetMeshRenderer()->GetMaterial(i)->SetTexture(0, m_textures[1]);
		else
			GetMeshRenderer()->GetMaterial(i)->SetTexture(0, m_textures[0]);
	}
}

void VolumeSlider_Script::ChangeSliderTransform()
{
	auto deltaMouseMove{ GET_SINGLE(CInput)->GetMouseMove().x };

	m_pos.x += deltaMouseMove / 2.f;

	if (m_pos.x <= -187.f)
		m_pos.x = -187.f;
	else if (m_pos.x >= 53.f)
		m_pos.x = 53.f;

	GetTransform()->SetLocalPosition(m_pos);

	m_scale.x += deltaMouseMove;

	if (m_scale.x <= 0.f)
		m_scale.x = 0.f;
	else if (m_scale.x >= 500.f)
		m_scale.x = 500.f;

	GetTransform()->SetLocalScale(m_scale);
}
