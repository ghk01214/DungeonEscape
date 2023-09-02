#include "pch.h"
#include "LoginButton_Script.h"

#include <Input.h>
#include <SceneManager.h>
#include <UI.h>

LoginButton_Script::LoginButton_Script(bool active) :
	m_showPopUp{ false },
	m_logIn{ false },
	temp{ 0 }
{
	m_active = active;
}

LoginButton_Script::~LoginButton_Script()
{
}

void LoginButton_Script::Awake()
{
	__super::Awake();
}

void LoginButton_Script::Start()
{
	__super::Start();
}

void LoginButton_Script::Update()
{
	if (GetUI()->GetVisible() == false)
		return;

	__super::Update();

	if (m_click == true)
	{
		if (m_input->Button_Up(CInput::DIMB_LBUTTON) == true)
		{
			m_click = false;
			m_showPopUp = true;
			m_logIn = true;

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

void LoginButton_Script::LateUpdate()
{
	__super::LateUpdate();
}

void LoginButton_Script::SetShowPopUpFlag(bool flag)
{
	m_showPopUp = flag;
}
