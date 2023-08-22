#include "pch.h"
#include "Setting_Script.h"

#include <Material.h>
#include <MeshRenderer.h>
#include <GameObject.h>
#include <UI.h>
#include <Input.h>

Setting_Script::Setting_Script()
{
}

Setting_Script::~Setting_Script()
{
}

void Setting_Script::Awake()
{
	__super::Awake();
}

void Setting_Script::Start()
{
	__super::Start();
}

void Setting_Script::Update()
{
	if (m_showPopUp == true)
		return;

	__super::Update();

	if (m_pos.x - (m_scale.x / 2) <= m_mousePos.x and m_mousePos.x <= m_pos.x + (m_scale.x / 2))
	{
		if (m_pos.y - (m_scale.y / 2) <= m_mousePos.y and m_mousePos.y <= m_pos.y + (m_scale.y / 2))
		{
			if (m_input->Button_Down(CInput::DIMB_LBUTTON) == true and m_click == false)
			{
				m_click = true;
				m_showPopUp = true;

				GetGameObject()->GetUI()->SetVisible(false);
			}
			else if (m_input->Button_Up(CInput::DIMB_LBUTTON) == true and m_click == true)
			{
				m_click = false;
			}
		}
	}
}

void Setting_Script::LateUpdate()
{
	__super::LateUpdate();
}

void Setting_Script::SetShowPopUpFlat(bool flag)
{
	m_showPopUp = flag;
}
