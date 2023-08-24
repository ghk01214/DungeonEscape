#include "pch.h"
#include "ReadyButton_Script.h"

#include <Input.h>

#include "Scene_Loading.h"

ReadyButton_Script::ReadyButton_Script() :
	m_ready{ false },
	m_characterSelect{ false }
{
}

ReadyButton_Script::~ReadyButton_Script()
{
}

void ReadyButton_Script::Awake()
{
	__super::Awake();
}

void ReadyButton_Script::Start()
{
	__super::Start();
}

void ReadyButton_Script::Update()
{
	if (m_characterSelect == false)
		return;

	__super::Update();

	if (m_click == true)
	{
		if (m_input->Button_Up(CInput::DIMB_LBUTTON) == true)
		{
			m_ready = !m_ready;
			m_click = false;

			//ChangeTexture(BUTTON);
		}
	}
	else if (m_pos.x - (m_scale.x / 2) <= m_mousePos.x and m_mousePos.x <= m_pos.x + (m_scale.x / 2))
	{
		if (m_pos.y - (m_scale.y / 2) <= m_mousePos.y and m_mousePos.y <= m_pos.y + (m_scale.y / 2))
		{
			if (m_input->Button_Down(CInput::DIMB_LBUTTON) == true)
			{
				m_click = true;

				//ChangeTexture(BUTTON_PRESSED);
			}
		}
	}

	if (m_ready == true)
		ChangeTexture(BUTTON_PRESSED);
	else
		ChangeTexture(BUTTON);
}

void ReadyButton_Script::LateUpdate()
{
	__super::LateUpdate();
}

void ReadyButton_Script::SetCharacterSelectFlag(bool flag)
{
	m_characterSelect = flag;
}
