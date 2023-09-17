#include "pch.h"
#include "PageChangeButton_Script.h"

#include <Input.h>
#include <UI.h>

PageChangeButton_Script::PageChangeButton_Script()
{
}

PageChangeButton_Script::~PageChangeButton_Script()
{
}

void PageChangeButton_Script::Awake()
{
	__super::Awake();
}

void PageChangeButton_Script::Start()
{
	__super::Start();

	SetVisible(false);
}

void PageChangeButton_Script::Update()
{
	if (GetUI()->GetVisible() == false)
		return;

	__super::Update();

	if (m_click == true)
	{
		if (m_input->Button_Up(CInput::DIMB_LBUTTON) == true)
		{
			m_click = false;
			m_changePage = true;

			ChangeTexture(BUTTON);
			SetVisible(false);
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

void PageChangeButton_Script::LateUpdate()
{
	if (GetUI()->GetVisible() == false)
		return;

	__super::LateUpdate();
}

void PageChangeButton_Script::SetVisible(bool flag)
{
	GetUI()->SetVisible(flag);
}

void PageChangeButton_Script::SetChangePageFlag(bool flag)
{
	m_changePage = flag;
}
