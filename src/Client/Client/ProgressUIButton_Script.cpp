#include "pch.h"
#include "ProgressUIButton_Script.h"

#include <UI.h>
#include <Input.h>

ProgressUIButton_Script::ProgressUIButton_Script() :
	m_openProgress{ false }
{
}

ProgressUIButton_Script::~ProgressUIButton_Script()
{
}

void ProgressUIButton_Script::Awake()
{
	__super::Awake();
}

void ProgressUIButton_Script::Start()
{
	__super::Start();
}

void ProgressUIButton_Script::Update()
{
	if (GetUI()->GetVisible() == false)
		return;

	__super::Update();

	if (m_click == true)
	{
		if (m_input->Button_Up(CInput::DIMB_LBUTTON) == true)
		{
			m_click = false;
			m_openProgress = true;

			ChangeTexture(BUTTON);
			GetUI()->SetVisible(false);
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

void ProgressUIButton_Script::LateUpdate()
{
	if (GetUI()->GetVisible() == false)
		return;

	__super::LateUpdate();
}

void ProgressUIButton_Script::SetOpenProgressFlag(bool flag)
{
	m_openProgress = flag;
}

void ProgressUIButton_Script::SetVisible(bool flag)
{
	GetUI()->SetVisible(flag);
}
