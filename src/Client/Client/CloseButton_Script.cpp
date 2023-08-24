#include "pch.h"
#include "CloseButton_Script.h"

#include <UI.h>
#include <Input.h>

CloseButton_Script::CloseButton_Script() :
	m_closePopUp{ false }
{
}

CloseButton_Script::~CloseButton_Script()
{
}

void CloseButton_Script::Awake()
{
	__super::Awake();
}

void CloseButton_Script::Start()
{
	__super::Start();
}

void CloseButton_Script::Update()
{
	if (GetUI()->GetVisible() == false)
		return;

	__super::Update();

	if (m_click == true)
	{
		if (m_input->Button_Up(CInput::DIMB_LBUTTON) == true)
		{
			m_click = false;
			m_closePopUp = true;

			ChangeTexture(BUTTON);
		}
	}
	else if (m_pos.x - (m_scale.x / 2) <= m_mousePos.x and m_mousePos.x <= m_pos.x + (m_scale.x / 2))
	{
		if (m_pos.y - (m_scale.y / 2) <= m_mousePos.y and m_mousePos.y <= m_pos.y + (m_scale.y / 2))
		{
			if (m_input->Button_Down(CInput::DIMB_LBUTTON) == true and m_click == false)
				m_click = true;

			ChangeTexture(BUTTON_PRESSED);
		}
		else
		{
			ChangeTexture(BUTTON);
		}
	}
	else
	{
		ChangeTexture(BUTTON);
	}
}

void CloseButton_Script::LateUpdate()
{
	if (GetUI()->GetVisible() == false)
		return;

	__super::LateUpdate();
}

void CloseButton_Script::SetClosePopUpFlag(bool flag)
{
	m_closePopUp = flag;
}
