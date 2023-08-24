#include "pch.h"
#include "SettingButton_Script.h"

#include <Material.h>
#include <MeshRenderer.h>
#include <GameObject.h>
#include <UI.h>
#include <Input.h>

SettingButton_Script::SettingButton_Script()
{
}

SettingButton_Script::~SettingButton_Script()
{
}

void SettingButton_Script::Awake()
{
	__super::Awake();
}

void SettingButton_Script::Start()
{
	__super::Start();
}

void SettingButton_Script::Update()
{
	if (GetGameObject()->GetUI()->GetVisible() == false)
		return;

	__super::Update();

	if (m_pos.x - (m_scale.x / 2) <= m_mousePos.x and m_mousePos.x <= m_pos.x + (m_scale.x / 2))
	{
		if (m_pos.y - (m_scale.y / 2) <= m_mousePos.y and m_mousePos.y <= m_pos.y + (m_scale.y / 2))
		{
			if (m_input->Button_Down(CInput::DIMB_LBUTTON) == true and m_click == false)
			{
				m_click = true;
			}
			else if (m_input->Button_Up(CInput::DIMB_LBUTTON) == true and m_click == true)
			{
				m_click = false;
				m_showPopUp = true;

				GetGameObject()->GetUI()->SetVisible(false);
			}
		}
	}
}

void SettingButton_Script::LateUpdate()
{
	if (GetGameObject()->GetUI()->GetVisible() == false)
		return;

	__super::LateUpdate();
}

void SettingButton_Script::SetShowPopUpFlag(bool flag)
{
	m_showPopUp = flag;
}
