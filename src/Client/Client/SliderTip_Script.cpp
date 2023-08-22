#include "pch.h"
#include "SliderTip_Script.h"

#include <Input.h>
#include <Transform.h>
#include <GameObject.h>
#include <UI.h>

SliderTip_Script::SliderTip_Script()
{
}

SliderTip_Script::~SliderTip_Script()
{
}

void SliderTip_Script::Awake()
{
	__super::Awake();
}

void SliderTip_Script::Start()
{
	__super::Start();

	GetTransform()->SetLocalPosition(Vec3{ 57.f, m_pos.y, m_pos.z });
	m_pos = GetTransform()->GetLocalPosition();
}

void SliderTip_Script::Update()
{
	if (GetGameObject()->GetUI()->GetVisible() == false)
		return;

	__super::Update();

	if (m_click == true)
	{
		if (m_input->Button_Up(CInput::DIMB_LBUTTON) == true)
			m_click = false;
	}
	else if (m_pos.x - (m_scale.x / 2) <= m_mousePos.x and m_mousePos.x <= m_pos.x + (m_scale.x / 2))
	{
		if (m_pos.y - (m_scale.y / 2) <= m_mousePos.y and m_mousePos.y <= m_pos.y + (m_scale.y / 2))
		{
			if (m_input->Button_Down(CInput::DIMB_LBUTTON) == true and m_click == false)
			{
				m_click = true;
			}
		}
	}

	if (m_click == false)
		return;

	if (m_mousePos.x < -183 or m_mousePos.x > 297)
		return;

	m_pos.x = static_cast<float>(m_mousePos.x);
	GetTransform()->SetLocalPosition(m_pos);

	Print(m_pos.x);
}

void SliderTip_Script::LateUpdate()
{
	__super::LateUpdate();
}
