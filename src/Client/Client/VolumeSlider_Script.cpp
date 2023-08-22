#include "pch.h"
#include "VolumeSlider_Script.h"

#include <GameObject.h>
#include <MeshRenderer.h>
#include <Material.h>
#include <Transform.h>
#include <Input.h>
#include <UI.h>

VolumeSlider_Script::VolumeSlider_Script() :
	m_sliderActive{ false },
	m_sliderTipXPos{ 0 }
{
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
	if (GetGameObject()->GetUI()->GetVisible() == false)
		return;

	if (m_sliderActive == false)
		return;

	if (m_sliderTipXPos < -183 or m_sliderTipXPos > 297)
		return;

	__super::Update();

	float deltaMouseMove{ GET_SINGLE(CInput)->GetMouseMove().x };

	m_pos.x += deltaMouseMove;
	GetTransform()->SetLocalPosition(m_pos);
	//GetTransform()->SetLocalScale(Vec3{ m_scale.x * 1.2f, m_scale.y, m_scale.z });

	// 한 쪽 방향으로 수축하는 바 작성 법
	// 델타 타임 대신 줄이고 싶은 비율대로 줄이기
	/*Vec3 pos{ GetTransform()->GetLocalPosition() };
	Vec3 scale{ GetTransform()->GetLocalScale() };

	pos.x -= DELTA_TIME / 2.f;
	scale.x -= DELTA_TIME;

	for (int32_t i = 0; i < GetMeshRenderer()->GetMaterialSize(); ++i)
	{
		GetTransform()->SetLocalPosition(pos);
		GetTransform()->SetLocalScale(scale);
	}*/
}

void VolumeSlider_Script::LateUpdate()
{
	__super::LateUpdate();
}

void VolumeSlider_Script::SetSliderActive(bool flag)
{
	m_sliderActive = flag;
}

void VolumeSlider_Script::SetSliderTipPos(int32_t x)
{
	m_sliderTipXPos = x;
}
